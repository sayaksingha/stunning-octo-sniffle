#include "dtlslib.h"

// make certificate valid from 1h10m in the past to allow for DST and clock skew
#define CERTIFICATE_CLOCK_SKEW_ALLOWANCE (-60 * (60 + 10))

// the initial MTU to use, this is a conservative value as we don't support MTU discovery
#define DTLS_INITIAL_MTU 1200

// the number of random bits used to create the certificate serial numbers
#define SERIAL_RAND_BITS 64

//#define DTLSLIB_SSL_DEBUG
//#define PRINT_ERRORS
//#define LOG_ERRORS

#include <memory.h>
#ifdef DTLSLIB_SSL_DEBUG
#include <stdio.h>
#endif

#ifdef WIN32
#include <windows.h>
#endif


#ifdef PRINT_ERRORS
#include <stdio.h>
#endif

#include <openssl/pem.h>
#include <openssl/conf.h>
#include <openssl/comp.h>
#include <openssl/x509v3.h>
#include <openssl/engine.h>
#include <openssl/ssl.h>
#include <openssl/srtp.h>

#ifdef LOG_ERRORS
#include "logging/log.h"

static const char *Acu_Log_Unique_Id = __FILE__;

static int log_error_callback(const char* str, size_t len, void* u)
{
	(void)len;
	if (u != NULL) {
		LogModuleError("dtlslib_session %p OpenSSL error:%s", u, str);
	} else {
		LogModuleError("OpenSSL error:%s", str);
	}
	return 0;
}
#endif

#if OPENSSL_VERSION_NUMBER >= 0x10100000L
#if defined(_MSC_VER) || defined(__MINGW32__)
#  include <time.h>
#ifndef _TIMEVAL_DEFINED
#define _TIMEVAL_DEFINED
struct timeval {
    long tv_sec;
    long tv_usec;
};
#endif /* _TIMEVAL_DEFINED */
#endif
#endif

struct dltslib_t {
	SSL_CTX* ssl_ctx;
};

static int verify_callback(int preverify_ok, X509_STORE_CTX *ctx)
{
	if (!preverify_ok) {
		int err = X509_STORE_CTX_get_error(ctx);
		switch(err) {
		case X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT:
		case X509_V_ERR_UNABLE_TO_GET_CRL:
		case X509_V_ERR_DEPTH_ZERO_SELF_SIGNED_CERT:
		case X509_V_ERR_SELF_SIGNED_CERT_IN_CHAIN:
		case X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY:
			preverify_ok = 1; // accept certs where we can't verify the trust chain, including self-signed certs
			break;
		case X509_V_ERR_CERT_NOT_YET_VALID:
		case X509_V_ERR_CERT_HAS_EXPIRED:
#ifdef LOG_ERRORS
			LogModuleWarning("Allowing %s DTLS certificate", err == X509_V_ERR_CERT_HAS_EXPIRED ? "expired" : "not yet valid");
#endif
			preverify_ok = 1; // accept expired or not yet valid certs
			break;
		default:
#ifdef LOG_ERRORS
			LogModuleError("verify certificate error: %d", err);
#endif
			break;
		}
	}
	return preverify_ok;
}

static int generate_cookie_callback(SSL *ssl, unsigned char *cookie, unsigned int *cookie_len);
#if OPENSSL_VERSION_NUMBER < 0x10100000L
static int verify_cookie_callback(SSL *ssl, unsigned char *cookie, unsigned int cookie_len);
#else
static int verify_cookie_callback(SSL *ssl, const unsigned char *cookie, unsigned int cookie_len);
static void init_methods_dtlslib_bio(void);
static void free_bio_methods(void);
#endif


dtlslib* dtlslib_init(unsigned char* session_context, unsigned int session_context_len)
{
	dtlslib* lib;

	if (session_context_len > SSL_MAX_SSL_SESSION_ID_LENGTH) return NULL;
	lib = malloc(sizeof(dtlslib));
#if OPENSSL_VERSION_NUMBER < 0x10100000L
	// Done automatically for SSL 1.1
	CRYPTO_malloc_init();
	ERR_load_crypto_strings();
	OpenSSL_add_all_algorithms();
	ENGINE_load_builtin_engines();
	SSL_load_error_strings();
#endif
	SSL_library_init();
	if (lib) {
#if OPENSSL_VERSION_NUMBER >= 0x10100000L
		init_methods_dtlslib_bio();
#endif
		lib->ssl_ctx = SSL_CTX_new(DTLS_method());
		if (!lib->ssl_ctx) {
#ifdef PRINT_ERRORS
			ERR_print_errors_fp(stdout);
#endif
#ifdef LOG_ERRORS
			ERR_print_errors_cb(log_error_callback, NULL);
#endif
#if OPENSSL_VERSION_NUMBER >= 0x10100000L
			free_bio_methods();
#endif
			free(lib);
			lib = NULL;
		} else {
			SSL_CTX_set_verify(lib->ssl_ctx, SSL_VERIFY_PEER|SSL_VERIFY_FAIL_IF_NO_PEER_CERT, verify_callback);
			SSL_CTX_set_quiet_shutdown(lib->ssl_ctx,1);
			SSL_CTX_set_read_ahead(lib->ssl_ctx, 1);
			SSL_CTX_set_tlsext_use_srtp(lib->ssl_ctx, "SRTP_AES128_CM_SHA1_80:SRTP_AES128_CM_SHA1_32");
			SSL_CTX_set_cookie_generate_cb(lib->ssl_ctx, generate_cookie_callback);
			SSL_CTX_set_cookie_verify_cb(lib->ssl_ctx, verify_cookie_callback);
			SSL_CTX_set_options(lib->ssl_ctx, SSL_OP_COOKIE_EXCHANGE);
			SSL_CTX_set_session_id_context(lib->ssl_ctx, session_context, session_context_len);
#if OPENSSL_VERSION_NUMBER < 0x30000000L
			SSL_CTX_set_ecdh_auto(lib->ssl_ctx, 1);
#endif
		}
	}
	return lib;
}

void dtlslib_free(dtlslib* lib)
{
	if (lib) {
		if (lib->ssl_ctx) SSL_CTX_free(lib->ssl_ctx);
		free(lib);
	}
	// NB SSL compression store leaks two blocks (in 1.0.1c these are 20 and 16 bytes in size)
	CONF_modules_unload(1);
	OBJ_cleanup();
	EVP_cleanup();
	ENGINE_cleanup();
	CRYPTO_cleanup_all_ex_data();
#if OPENSSL_VERSION_NUMBER < 0x10100000L
	ERR_remove_thread_state(NULL);
#endif
	ERR_free_strings();
	COMP_zlib_cleanup();
#if OPENSSL_VERSION_NUMBER >= 0x10100000L
	free_bio_methods();
#endif
}

struct dltslib_cert_t {
	X509 *x509p;
	EVP_PKEY *pk;
	const EVP_MD *md;
};
/*
enum dtlslib_hash_type {
	dtlslib_hash_type_sha_224,
	dtlslib_hash_type_sha_256,
	dtlslib_hash_type_sha_384,
	dtlslib_hash_type_sha_512,
};
*/
dtlslib_cert* dtlslib_create_cert(enum dtlslib_hash_type ht, int bits, int serial, long lifetime_seconds, const unsigned char* country, const unsigned char* common_name)
{
	dtlslib_cert* ret = NULL;
	X509 *x = NULL;
	EVP_PKEY *pk = NULL;
#if OPENSSL_VERSION_NUMBER < 0x30000000L
	BIGNUM *bn = NULL;
	RSA *rsa = NULL;
#endif
	X509_NAME *name=NULL;

	ret = malloc(sizeof(dtlslib_cert));
	if (ret == NULL) {
		goto err;
	}

	switch (ht) {
	case dtlslib_hash_type_sha_1:
		ret->md = EVP_sha1();
		break;
	case dtlslib_hash_type_sha_224:
		ret->md = EVP_sha224();
		break;
	case dtlslib_hash_type_sha_256:
		ret->md = EVP_sha256();
		break;
	case dtlslib_hash_type_sha_384:
		ret->md = EVP_sha384();
		break;
	case dtlslib_hash_type_sha_512:
		ret->md = EVP_sha512();
		break;
	default:
		goto err;
	}
	
	if ((x=X509_new()) == NULL) {
		goto err;
	}

#if OPENSSL_VERSION_NUMBER >= 0x30000000L
	pk = EVP_RSA_gen(bits);
	if (pk == NULL) {
		goto err;
	}
	if (serial < 0) {
		uint64_t rand_serial;
		if (RAND_bytes((unsigned char*)&rand_serial, sizeof(rand_serial)) != 1
			|| ASN1_INTEGER_set_uint64(X509_get_serialNumber(x), rand_serial) != 1) {
			goto err;
		}
	} else {
		if (ASN1_INTEGER_set(X509_get_serialNumber(x),serial) != 1) {
			goto err;
		}
	}
#else
	if ((pk=EVP_PKEY_new()) == NULL) {
		goto err;
	}
	bn = BN_new();
	rsa = RSA_new();
	if(!bn || !rsa || !BN_set_word(bn, RSA_F4)) {
		goto err;
	}
	if(!RSA_generate_key_ex(rsa,bits,bn,NULL)) {
		goto err;
	}
	if (!EVP_PKEY_assign_RSA(pk,rsa)) {
		goto err;
	}
	rsa = NULL; // now owned by pk

	X509_set_version(x,2);

	if (serial < 0) {
		if (!BN_pseudo_rand(bn, SERIAL_RAND_BITS, 0, 0)) {
			goto err;
		}
		if (!BN_to_ASN1_INTEGER(bn, X509_get_serialNumber(x))) {
			goto err;
		}
	}
	else
	{
		if (ASN1_INTEGER_set(X509_get_serialNumber(x),serial) != 1) {
			goto err;
		}
	}

	BN_free(bn);
	bn = NULL;
#endif

	X509_gmtime_adj(X509_get_notBefore(x),CERTIFICATE_CLOCK_SKEW_ALLOWANCE);
	X509_gmtime_adj(X509_get_notAfter(x),lifetime_seconds);
	X509_set_pubkey(x,pk);
	name=X509_get_subject_name(x);

	/* This function creates and adds the entry, working out the
	 * correct string type and performing checks on its length.
	 * Normally we'd check the return value for errors...
	 */
	if (country) X509_NAME_add_entry_by_txt(name, "C", MBSTRING_ASC, country, -1, -1, 0);
	X509_NAME_add_entry_by_txt(name, "CN", MBSTRING_ASC, common_name, -1, -1, 0);

	/* Its self signed so set the issuer name to be the same as the
 	 * subject.
	 */
	X509_set_issuer_name(x,name);
	if (!X509_sign(x,pk,ret->md)) {
		goto err;
	}

	ret->x509p = x;
	ret->pk = pk;
	return ret;
err:
#ifdef PRINT_ERRORS
	ERR_print_errors_fp(stdout);
#endif
#ifdef LOG_ERRORS
	ERR_print_errors_cb(log_error_callback, NULL);
#endif
	if (ret) free(ret);
	if (x) X509_free(x);
	if (pk) EVP_PKEY_free(pk);
#if OPENSSL_VERSION_NUMBER < 0x30000000L
	if (rsa) RSA_free(rsa);
	if (bn) BN_free(bn);
#endif
	return NULL;
}

int dtlslib_get_cert_fingerprint(dtlslib_cert* c, char* dest, unsigned int dest_len)
{
	unsigned int j;
	unsigned int n;
	unsigned char md[EVP_MAX_MD_SIZE];

	if (!X509_digest(c->x509p,c->md,md,&n)) {
		return -1;
	}
	if (dest_len >= 3*n) {
		n--;
		for (j = 0; j < n; j++) {
			sprintf(dest,"%02X:", md[j]);
			dest += 3;
		}
		sprintf(dest,"%02X", md[j]);
		return 3*n + 2;
	}
	return 0;
}

void dtlslib_free_cert(dtlslib_cert* c)
{
	if (c) {
		if (c->x509p) X509_free(c->x509p);
		if (c->pk) EVP_PKEY_free(c->pk);
		free(c);
	}
}

#define DTLSLIB_SESSION_COOKIE_LEN 16
struct dltslib_session_t {
	unsigned char cookie[DTLSLIB_SESSION_COOKIE_LEN];
	SSL* ssl;
	int is_server;
	int server_done_listen;
	dtlslib_send_data_cb send_cb;
	void* send_cb_ctx;
	dtlslib_read_data_cb read_cb;
	void* read_cb_ctx;
	int in_peek_mode;
	int peeked_len;
	char peeked_data[2048];
};

static int dtlslib_bio_write(BIO *b, const char *buf, int num);
static int dtlslib_bio_read(BIO *b, char *buf, int size);
static int dtlslib_bio_puts(BIO *b, const char *str);
static long dtlslib_bio_ctrl(BIO *b, int cmd, long arg1, void *arg2);
static int dtlslib_bio_new(BIO *b);
static int dtlslib_bio_free(BIO *b);

#if OPENSSL_VERSION_NUMBER < 0x10100000L

static BIO_METHOD methods_dtlslib_bio=
	{
	99|BIO_TYPE_SOURCE_SINK,
	"Aculab dtlslib bio",
	dtlslib_bio_write,
	dtlslib_bio_read,
	dtlslib_bio_puts,
	NULL, /* int (*bgets) (BIO *, char *, int) */
	dtlslib_bio_ctrl,
	dtlslib_bio_new,
	dtlslib_bio_free,
	NULL,   /* long (*callback_ctrl) (BIO *, int, bio_info_cb *) */
	};

#else


static BIO_METHOD *methods_dtlslib_bio = NULL;

void init_methods_dtlslib_bio(void)
{
    methods_dtlslib_bio = BIO_meth_new(99|BIO_TYPE_SOURCE_SINK, "Aculab dtlslib bio");
    BIO_meth_set_write(methods_dtlslib_bio, &dtlslib_bio_write);
    BIO_meth_set_read(methods_dtlslib_bio, &dtlslib_bio_read);
    BIO_meth_set_puts(methods_dtlslib_bio, &dtlslib_bio_puts);
    // BIO_meth_set_gets(methods_dtlslib_bio, &dtlslib_bio_gets); /* NULL */
    BIO_meth_set_ctrl(methods_dtlslib_bio, &dtlslib_bio_ctrl);
    BIO_meth_set_create(methods_dtlslib_bio, &dtlslib_bio_new);
    BIO_meth_set_destroy(methods_dtlslib_bio, &dtlslib_bio_free);
}

void free_bio_methods(void)
{
    BIO_meth_free(methods_dtlslib_bio);
}

#endif

typedef struct dtlslib_bio_data_t {
	unsigned int mtu;
	dtlslib_session* sess;
} dtlslib_bio_data;

#define BIO_CTRL_DTLSLIB_BIO_SET_SESS 99

static BIO_METHOD *BIO_s_dtlslib_bio(void) {
#if OPENSSL_VERSION_NUMBER < 0x10100000L
	return(&methods_dtlslib_bio);
#else
	return(methods_dtlslib_bio);
#endif
}

static BIO *BIO_new_dtlslib_bio(dtlslib_session* sess) {
	BIO *ret;

	ret=BIO_new(BIO_s_dtlslib_bio());
	if (ret == NULL) return(NULL);
	BIO_ctrl(ret, BIO_CTRL_DTLSLIB_BIO_SET_SESS, 0, sess);
	return ret;
}

static int dtlslib_bio_write(BIO *b, const char *buf, int num)
{
#if OPENSSL_VERSION_NUMBER < 0x10100000L
	dtlslib_bio_data* data = (dtlslib_bio_data*)(b->ptr);
#else
	dtlslib_bio_data* data = (dtlslib_bio_data*)BIO_get_data(b);
#endif
	int ret = -1;
	if (data && data->sess && data->sess->send_cb) {
		ret = data->sess->send_cb(data->sess->send_cb_ctx, buf, num);
	}
	BIO_clear_retry_flags(b);
#ifdef LOG_ERRORS
	LogModuleDebugMax("dtlsbio %p write %d returned %d", b, num, ret);
#endif
	return ret; 
}

static int dtlslib_bio_read(BIO *b, char *buf, int size)
{
#if OPENSSL_VERSION_NUMBER < 0x10100000L
	dtlslib_bio_data* data = (dtlslib_bio_data*)(b->ptr);
#else
	dtlslib_bio_data* data = (dtlslib_bio_data*)BIO_get_data(b);
#endif
	int ret = -1;
	if (data && data->sess && data->sess->read_cb) {
		if (data->sess->peeked_len > 0) {
			ret = data->sess->peeked_len;
			if (ret > size) {
				ret = size;
			}
			memcpy(buf,&data->sess->peeked_data[0],ret);
			if (!data->sess->in_peek_mode) {
				data->sess->peeked_len = 0;
			}
		} else {
			ret = data->sess->read_cb(data->sess->read_cb_ctx, buf, size);
			if ((ret > 0) && data->sess->in_peek_mode) {
				if (sizeof(data->sess->peeked_data) < (unsigned)ret) {
					data->sess->peeked_len = (int) sizeof(data->sess->peeked_data);
				} else {
					data->sess->peeked_len = ret;
				}
				memcpy(&data->sess->peeked_data[0],buf,data->sess->peeked_len);
			}
		}
	}
	BIO_clear_retry_flags(b);
	if (ret == 0) {
		BIO_set_retry_read(b);
		ret = -1;
	}
#ifdef LOG_ERRORS
	LogModuleDebugMax("dtlsbio %p read %d returned %d", b, size, ret);
#endif
	return ret;
}

static int dtlslib_bio_puts(BIO *b, const char *str)
{
	int n,ret;
	n=(int)strlen(str);
	ret=dtlslib_bio_write(b,str,n);
	return(ret);
}

static long dtlslib_bio_ctrl(BIO *b, int cmd, long arg1, void *arg2)
{
	long ret = 1;
#if OPENSSL_VERSION_NUMBER < 0x10100000L
	dtlslib_bio_data* data = (dtlslib_bio_data*)(b->ptr);
#else
	dtlslib_bio_data* data = (dtlslib_bio_data*)BIO_get_data(b);
#endif
	switch (cmd) {
	case BIO_CTRL_FLUSH:
		break;
	case BIO_CTRL_DGRAM_MTU_DISCOVER:
		ret = 0;
		break;
	case BIO_CTRL_DGRAM_QUERY_MTU:
		ret = data->mtu;
		break;
	case BIO_CTRL_DGRAM_GET_FALLBACK_MTU:
		ret = 576 - 20 - 8;
		break;
	case BIO_CTRL_DGRAM_GET_MTU:
		ret = data->mtu;
		break;
	case BIO_CTRL_DGRAM_SET_MTU:
		data->mtu = arg1;
		ret = arg1;
		break;
	case BIO_CTRL_DGRAM_MTU_EXCEEDED:
		ret = 0;
		break;
	case BIO_CTRL_DTLSLIB_BIO_SET_SESS:
		if (data) {
			data->sess = (dtlslib_session*)arg2;
#if OPENSSL_VERSION_NUMBER < 0x10100000L
			b->init = 1;
#else
		    BIO_set_init(b, 1);
#endif
		}
		ret = 1;
		break;
#if OPENSSL_VERSION_NUMBER >= 0x10100000L
	case BIO_CTRL_DGRAM_SET_PEEK_MODE:
		if (data && data->sess && data->sess->read_cb) {
			data->sess->in_peek_mode = arg1;
		}
		break;
#endif
	default:
		ret = 0;
		break;
	}
#ifdef LOG_ERRORS
	LogModuleDebugMax("dtlsbio %p ctrl %d returned %d", b, cmd, ret);
#endif
	return ret;
}

static int dtlslib_bio_new(BIO *b)
{
	dtlslib_bio_data *data = NULL;
#if OPENSSL_VERSION_NUMBER < 0x10100000L
	b->init = 0;
	b->num = 0;
#else
	BIO_set_init(b, 0);

	// no setter for num field - we don't believe it is used
#endif
	data = malloc(sizeof(dtlslib_bio_data));
	if (data == NULL)
		return 0;
	memset(data, 0x00, sizeof(dtlslib_bio_data));
#if OPENSSL_VERSION_NUMBER < 0x10100000L
    b->ptr = data;
#else
	BIO_set_data(b, data);
#endif
	data->mtu = DTLS_INITIAL_MTU;
#if OPENSSL_VERSION_NUMBER < 0x10100000L
	b->flags = 0;
#else
	BIO_set_flags(b,0);
#endif
	return 1;
}

static int dtlslib_bio_free(BIO *b)
{
#if OPENSSL_VERSION_NUMBER < 0x10100000L
	free(b->ptr);
#else
	free(BIO_get_data(b));
#endif
	return 1;
}



static int generate_cookie_callback(SSL *ssl, unsigned char *cookie, unsigned int *cookie_len)
{
	dtlslib_session* sess = SSL_get_ex_data(ssl, 0);
	if (!sess) {
		return 0;
	}
	memcpy(cookie, sess->cookie, DTLSLIB_SESSION_COOKIE_LEN);
	*cookie_len = DTLSLIB_SESSION_COOKIE_LEN;
	return 1;
}

#if OPENSSL_VERSION_NUMBER < 0x10100000L
static int verify_cookie_callback(SSL *ssl, unsigned char *cookie, unsigned int cookie_len)
#else
static int verify_cookie_callback(SSL *ssl, const unsigned char *cookie, unsigned int cookie_len)
#endif
{
	dtlslib_session* sess = SSL_get_ex_data(ssl, 0);
	if (!sess) {
		return 0;
	}
	if (cookie_len == DTLSLIB_SESSION_COOKIE_LEN && memcmp(cookie, sess->cookie, DTLSLIB_SESSION_COOKIE_LEN) == 0) return 1;
	return 0;
}

#ifdef DTLSLIB_SSL_DEBUG
static BIO* bio_c_out=NULL;

long bio_dump_callback(BIO *bio, int cmd, const char *argp,
				   int argi, long argl, long ret)
	{
	BIO *out;

	out=(BIO *)BIO_get_callback_arg(bio);
	if (out == NULL) return(ret);

	if (cmd == (BIO_CB_READ|BIO_CB_RETURN))
		{
		BIO_printf(out,"read from %p [%p] (%lu bytes => %ld (0x%lX))\n",
 			(void *)bio,argp,(unsigned long)argi,ret,ret);
		BIO_dump(out,argp,(int)ret);
		return(ret);
		}
	else if (cmd == (BIO_CB_WRITE|BIO_CB_RETURN))
		{
		BIO_printf(out,"write to %p [%p] (%lu bytes => %ld (0x%lX))\n",
			(void *)bio,argp,(unsigned long)argi,ret,ret);
		BIO_dump(out,argp,(int)ret);
		}
	return(ret);
	}
#endif

dtlslib_session* dtlslib_create_session(dtlslib* lib, dtlslib_cert* c, dtlslib_send_data_cb send_cb, void* send_ctx, dtlslib_read_data_cb read_cb, void* read_ctx)
{
	dtlslib_session* sess;
	BIO* b;
	sess = malloc(sizeof(dtlslib_session));
	if (sess) {
		memset(sess, 0, sizeof(dtlslib_session));
		sess->send_cb = send_cb;
		sess->send_cb_ctx = send_ctx;
		sess->read_cb = read_cb;
		sess->read_cb_ctx = read_ctx;
		sess->is_server = -1;
		b = BIO_new_dtlslib_bio(sess);
		if (b) {
			sess->ssl = SSL_new(lib->ssl_ctx);
			if (sess->ssl) {
				int r;
#ifdef DTLSLIB_SSL_DEBUG
				SSL_set_debug(sess->ssl, 1);
#endif
				SSL_set_bio(sess->ssl,b,b); // this now owns the bio
#ifdef DTLSLIB_SSL_DEBUG
				if (!bio_c_out) bio_c_out = BIO_new_fp(stdout,BIO_NOCLOSE);
				if (bio_c_out) {
					BIO_set_callback(SSL_get_rbio(sess->ssl),bio_dump_callback);
					BIO_set_callback_arg(SSL_get_rbio(sess->ssl),(char *)bio_c_out);
				}
#endif
				r = RAND_bytes(sess->cookie, DTLSLIB_SESSION_COOKIE_LEN);
				if (r == 1) {
					r = SSL_set_ex_data(sess->ssl, 0, sess);
				}
				if (r == 1) {
					r = SSL_use_certificate(sess->ssl, c->x509p);
				}
				if (r == 1) {
					r = SSL_use_PrivateKey(sess->ssl, c->pk);
				}
				if (r != 1) {
#ifdef PRINT_ERRORS
					ERR_print_errors_fp(stdout);
#endif
#ifdef LOG_ERRORS
					ERR_print_errors_cb(log_error_callback, NULL);
#endif
					SSL_free(sess->ssl);
					free(sess);
					sess = NULL;
				}
				return sess;
			} else {
#ifdef PRINT_ERRORS
				ERR_print_errors_fp(stdout);
#endif
#ifdef LOG_ERRORS
				ERR_print_errors_cb(log_error_callback, NULL);
#endif
				BIO_free(b);
			}
		}

	}
	free(sess);
	return NULL;
}

void dtlslib_free_session(dtlslib_session* sess)
{
	if (sess) {
		if (sess->ssl) {
			SSL_free(sess->ssl);
		}
		free(sess);
	}
}

int dtlslib_start_handshake(dtlslib_session* sess, int is_server)
{
	int ret = 0;
	int doContinue = 1;
#if OPENSSL_VERSION_NUMBER >= 0x10100000L
	BIO_ADDR *peer = 0;
#else
	void* peer = 0;
#endif

#if 0
	STACK_OF(SSL_CIPHER) *stack;
	const char *cipher_name;
	int i;

	stack = SSL_get_ciphers (sess->ssl);
	for (i = 0; i < sk_SSL_CIPHER_num (stack); i++) {
		cipher_name = SSL_CIPHER_get_name (sk_SSL_CIPHER_value (stack, i));
		LogModuleDebugMax("dtlssess %p cipher: %s", sess, cipher_name);
	}
#endif
	if (is_server) {
		sess->is_server = 1;
		sess->server_done_listen = 0;
#if OPENSSL_VERSION_NUMBER >= 0x10100000L
		peer = BIO_ADDR_new();
#endif
		if (DTLSv1_listen(sess->ssl, peer) == 1) {
			sess->server_done_listen = 1;
		} else {
#if OPENSSL_VERSION_NUMBER >= 0x10100000L
			doContinue = 0;
#endif
		}
#if OPENSSL_VERSION_NUMBER >= 0x10100000L
		BIO_ADDR_free(peer);
#endif
	} else {
		sess->is_server = 0;
		SSL_set_connect_state(sess->ssl);
	}
	if (doContinue) {
		ret = dtlslib_continue(sess);
	}
	return ret;
}

int dtlslib_continue(dtlslib_session* sess)
{
	int ret = 0;
	int canProcessError = 1;
#if OPENSSL_VERSION_NUMBER >= 0x10100000L
	BIO_ADDR *peer = 0;
#else
	void* peer = 0;
#endif
	if (sess->is_server && !sess->server_done_listen) {
#if OPENSSL_VERSION_NUMBER >= 0x10100000L
		peer = BIO_ADDR_new();
#endif
		ret = DTLSv1_listen(sess->ssl, peer);
#if OPENSSL_VERSION_NUMBER >= 0x10100000L
		BIO_ADDR_free(peer);
#endif
		if (ret == 1) {
			sess->server_done_listen = 1;
			// not done yet, just got ClientHello with cookie
			ret = SSL_do_handshake(sess->ssl);
		} else {
#if OPENSSL_VERSION_NUMBER >= 0x10100000L
			canProcessError = 0;
#endif
		}
	} else {
		DTLSv1_handle_timeout(sess->ssl); // checks if timeout has occurred, safe to call when timeout not pending
		ret = SSL_do_handshake(sess->ssl);
		if (ret == 1) {
			// a dummy read to see if retransmissions are needed NB. dtls-srtp sends no dtls data
			char buf[2048];
			SSL_read(sess->ssl, buf, sizeof(buf));
		}
	}

		// SSL 1.1 Do not invoke this on error from
		// non-blocking DTLSv1_listen as that invokes
		// invokes SSL_clear, setting rw_state to NOTHING,
		// that would mean we would get spurious SSL_ERROR_SYSCALL
		// and not SL_ERROR_WANT_READ or SSL_ERROR_WANT_WRITE
	if ((canProcessError) && (ret <= 0)) {
		int err = SSL_get_error(sess->ssl, ret);
#ifdef PRINT_ERRORS
		ERR_print_errors_fp(stdout);
#endif
#ifdef LOG_ERRORS
		ERR_print_errors_cb(log_error_callback, sess);
#endif
		if (err == SSL_ERROR_WANT_READ || err == SSL_ERROR_WANT_WRITE) {
			ret = 0; // not done yet
		} else {
			ret = -1; // error occurred
		}
	}
	return ret;
}


int dtlslib_get_timeout_ms(dtlslib_session* sess)
{
	struct timeval timeout;
	int ms = -1; // wait forever?
	if (DTLSv1_get_timeout(sess->ssl, &timeout)) {
		ms = timeout.tv_sec * 1000;
		ms += timeout.tv_usec / 1000;
	}
	return ms;
}

int dtlslib_get_remote_fingerprint(dtlslib_session* sess, enum dtlslib_hash_type ht, char* dest, unsigned int dest_len)
{
	X509* peer;
	int ret = -1;
	const EVP_MD *md;
	switch (ht) {
	case dtlslib_hash_type_sha_1:
		md = EVP_sha1();
		break;
	case dtlslib_hash_type_sha_224:
		md = EVP_sha224();
		break;
	case dtlslib_hash_type_sha_256:
		md = EVP_sha256();
		break;
	case dtlslib_hash_type_sha_384:
		md = EVP_sha384();
		break;
	case dtlslib_hash_type_sha_512:
		md = EVP_sha512();
		break;
	default:
		md = NULL;
	}
	if (md != NULL) {
		peer = SSL_get_peer_certificate(sess->ssl);
		if (peer != NULL) {
			unsigned int j;
			unsigned int n;
			unsigned char mdbuf[EVP_MAX_MD_SIZE];

			if (X509_digest(peer,md,mdbuf,&n)) {
				if (dest_len >= 3*n) {
					n--;
					for (j = 0; j < n; j++) {
						sprintf(dest,"%02X:", mdbuf[j]);
						dest += 3;
					}
					sprintf(dest,"%02X", mdbuf[j]);
					ret = 3*n + 2;
				} else {
					ret = 0;
				}
			}
			X509_free(peer);
		}
	}
	return ret;
}

static const char* dtlslib_srtp_extractor_label = "EXTRACTOR-dtls_srtp";
#define SRTP_MASTER_KEY_KEY_LEN 16
#define SRTP_MASTER_KEY_SALT_LEN 14
#define SRTP_DATA_EXTRACT_LEN  ((SRTP_MASTER_KEY_KEY_LEN * 2) + (SRTP_MASTER_KEY_SALT_LEN * 2))

int dtlslib_extract_srtp_keys(dtlslib_session* sess, enum dtlslib_srtp_mode* mode, char* srtp_tx_key, char* srtp_rx_key)
{
	unsigned char data[SRTP_DATA_EXTRACT_LEN];
	SRTP_PROTECTION_PROFILE *srtp_profile = SSL_get_selected_srtp_profile(sess->ssl);
	if (srtp_profile) {
		switch (srtp_profile->id) {
		case SRTP_AES128_CM_SHA1_80:
			*mode = dtlslib_srtp_mode_AES_CM_128_HMAC_SHA1_80;
			break;
		case SRTP_AES128_CM_SHA1_32:
			*mode = dtlslib_srtp_mode_AES_CM_128_HMAC_SHA1_32;
			break;
		default:
#ifdef LOG_ERRORS
			LogModuleError("unknown srtp profile %d", srtp_profile->id);
#endif
			srtp_profile = NULL; // so we don't extract the srtp keys
			break;
		}
	}
	if (srtp_profile) {
		// now extract the key
		if (SSL_export_keying_material(sess->ssl, 
				data, SRTP_DATA_EXTRACT_LEN, 
				dtlslib_srtp_extractor_label, strlen(dtlslib_srtp_extractor_label),
				NULL, 0, 0)) {
			char* pclient;
			char* pserver;
			unsigned char* pdata = data;
			// got the data, copy the right bits to each key
			if (sess->is_server == 0) { // we are client
				pclient = srtp_tx_key;
				pserver = srtp_rx_key;
			} else { // server
				pclient = srtp_rx_key;
				pserver = srtp_tx_key;
			}

			memcpy(pclient, pdata, SRTP_MASTER_KEY_KEY_LEN);
			pdata += SRTP_MASTER_KEY_KEY_LEN;
			pclient += SRTP_MASTER_KEY_KEY_LEN;
			memcpy(pserver, pdata, SRTP_MASTER_KEY_KEY_LEN);
			pdata += SRTP_MASTER_KEY_KEY_LEN;
			pserver += SRTP_MASTER_KEY_KEY_LEN;

			memcpy(pclient, pdata, SRTP_MASTER_KEY_SALT_LEN);
			pdata += SRTP_MASTER_KEY_SALT_LEN;
			memcpy(pserver, pdata, SRTP_MASTER_KEY_SALT_LEN);

			return 0;
		}
	} else {
#ifdef LOG_ERRORS
		LogModuleInfo("no srtp profile returned", 0);
#endif
	}
#ifdef PRINT_ERRORS
	ERR_print_errors_fp(stdout);
#endif
#ifdef LOG_ERRORS
	ERR_print_errors_cb(log_error_callback, sess);
#endif
	return -1;
}

