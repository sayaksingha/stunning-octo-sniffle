#ifndef INCLUDED_DTLSLIB_H
#define INCLUDED_DTLSLIB_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dltslib_t dtlslib;

dtlslib* dtlslib_init(unsigned char* session_context, unsigned int session_context_len);
void dtlslib_free(dtlslib* lib);

enum dtlslib_hash_type {
	dtlslib_hash_type_sha_1,
	dtlslib_hash_type_sha_224,
	dtlslib_hash_type_sha_256,
	dtlslib_hash_type_sha_384,
	dtlslib_hash_type_sha_512,
};

typedef struct dltslib_cert_t dtlslib_cert;

dtlslib_cert* dtlslib_create_cert(enum dtlslib_hash_type ht, int bits, int serial, long lifetime_seconds, const unsigned char* country, const unsigned char* common_name);
int dtlslib_get_cert_fingerprint(dtlslib_cert* c, char* dest, unsigned int dest_len);
void dtlslib_free_cert(dtlslib_cert* c);

typedef struct dltslib_session_t dtlslib_session;
typedef int (*dtlslib_send_data_cb)(void* ctx, const char* data, unsigned len);
typedef int (*dtlslib_read_data_cb)(void* ctx, char* data, unsigned max_len);

dtlslib_session* dtlslib_create_session(dtlslib* lib, dtlslib_cert* c, dtlslib_send_data_cb send_cb, void* send_ctx, dtlslib_read_data_cb read_cb, void* read_ctx);
void dtlslib_free_session(dtlslib_session* sess);
int dtlslib_start_handshake(dtlslib_session* sess, int is_server);
int dtlslib_continue(dtlslib_session* sess);
int dtlslib_get_timeout_ms(dtlslib_session* sess);
int dtlslib_get_remote_fingerprint(dtlslib_session* sess, enum dtlslib_hash_type ht, char* dest, unsigned int dest_len);

enum dtlslib_srtp_mode {
	dtlslib_srtp_mode_AES_CM_128_HMAC_SHA1_80,
	dtlslib_srtp_mode_AES_CM_128_HMAC_SHA1_32,
};
int dtlslib_extract_srtp_keys(dtlslib_session* sess, enum dtlslib_srtp_mode* mode, char* srtp_tx_key, char* srtp_rx_key);

#ifdef __cplusplus
}
#endif

#endif
