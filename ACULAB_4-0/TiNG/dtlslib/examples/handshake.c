// Example code illustrating server awaiting a DTLS client hello on its RTP socket
// then completing handshake and configuring VMPRx to use agreed encode/decode keys
// for SRTP encoded audio.
//
// Typical good DTLS UDP packet exchange:
// Client <-> Server
//   -> Client-Hello
//   <- Hello-Verify-Req
//   -> Client-Hello
//   <- Server Hello + Certificate + Certificate-Req + Server-Hello_Done
//   -> Certificate + Client-Key-Xcg + Cerificate-Verify + Change-Cipher-Spec + Enc-Handshake-Msg
//   <- New-Session-Ticket + Change-Cipher-Spec + Enc-Handshake-Msg
//
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#ifndef _WIN32
#include <arpa/inet.h>
#endif
#include <memory.h>
#include <time.h>
#include "prosody_x.h"
#include "smrtp.h"
#include "smpath.h"

// TiNG DTLS library layered over OpenSSL DTLS
#include "../dtlslib/dtlslib.h"

// Callbacks layered over PS DTLS UDP primitives.
#include "psdtlssocket.h"

// Maximum time we permit for whole DTLS handshake.
#define kMaxHandshakeDurationInSecs 60

// Sha512 has maximum MD of 64 (OpenSSL EVP_MAX_MD_SIZE)
// Fingerprint is MD -> text, each octet as 2 hex digits + space.
#define kMaxFingerprint (3*64+1)

// A structure to obtain 240 bit cipher key vectors.
typedef struct {
	char 					fEncodeKey[30];	// encode/auth our RTP
	char 					fDecodeKey[30];	// decode/auth peer RTP
	enum dtlslib_srtp_mode 	fSRTPMode;		// indicates tag len we should use for HMAC auth
} tKeyDetails;

// Parameters for certificate generation
typedef struct {
	enum dtlslib_hash_type	fHashType;			// Signing algorithm for certificate
	int 					fBitCount;			// Strength of RSA keys (eg. 2048)
	int 					fSerial;			// Serial number for certificate (or -1 to set random)
	int 					fLifetimeSeconds;	// Life time of certificate.
	uint8_t* 				fCountry;			// Two letter country code for certificate or NULL.
	uint8_t*				fCommonName;		// Name indicating issuer of certificate.	
} tCertParams;


// If set, verbose output.
static int gVerbose;

// Data indication to DTLS the application using OpenSSL stack.
static uint8_t gOpenSSLSCId[8] = {'A','C','U','X','D','T','L','S'};

// Set socket to send DTLS UDPs to given IPv4 address.
static void setDestAddr( tPSDTLSSocketAccess sc, char* ip, uint16_t port )
{
	int 		rc;
	SOCKADDR_IN	destAddr;

	memset(&destAddr,0,sizeof(destAddr));

	if (inet_pton(AF_INET,ip,&destAddr.sin_addr) != 1)
	{
		printf("setDestAddr - bad IPv4 address %s\n",ip);
		exit(2);
	}

	destAddr.sin_port = htons(port);

	rc = psDTLSSocketConfigIPv4((tPSDTLSSocketAccess) sc,NULL,&destAddr);

	if (rc != 0)
	{
		printf("setDestAddr: psDTLSSocketConfigIPv4 failed\n");
		exit(2);
	}
}

// Dump to stdout the SRTP keys that were agreed.
static void dumpKey( char* entity, tKeyDetails* keyDetails )
{
	int		i;
	char* 	tag = (keyDetails->fSRTPMode == dtlslib_srtp_mode_AES_CM_128_HMAC_SHA1_80) ? "SHA1_80" : "SHA1_32";

	printf("%s: %s: Tx ",entity,tag);

	for (i = 0; i < (int) sizeof(keyDetails->fEncodeKey); i++)
	{
		printf("%02x ",((uint8_t)keyDetails->fEncodeKey[i]));
	}

	printf("\n%s: %s: Rx ",entity,tag);

	for (i = 0; i < (int) sizeof(keyDetails->fDecodeKey); i++)
	{
		printf("%02x ",((uint8_t)keyDetails->fDecodeKey[i]));
	}

	printf("\n");
}

// Perform the DTLS handshake sequence
static int performDTLSHandshake( tPSDTLSSocketAccess sa, dtlslib_session* session, int maxTimeInSecs, tKeyDetails* keys )
{
	int				rc,ms;
	int				getKeys;
	int				pollResult;

	if (gVerbose)
	{
		printf("server: invoking dtlslib_start_handshake\n");
	}

	getKeys = 0;

	if (gVerbose)
	{
		printf("server: maximum time allowed for handshake %ds\n",maxTimeInSecs);
	}

	psDTLSSocketSetLimitTime(sa,maxTimeInSecs);

	// Start a handshake in the role of server
	rc = dtlslib_start_handshake(session,1);

	if (rc == -1)
	{
		printf("error %d - dtlslib_start_handshake - server\n",rc);
		exit(1);
	}

	ms = -1;

	// check if handshake has immediately completed.
	if (rc == 1)
	{
		getKeys = 1;
	}
	else
	{
		// retranmission timeout in case we don't get response.
		ms = dtlslib_get_timeout_ms(session);
	}

	if (gVerbose)
	{
		printf("server: dtlslib_start_handshake returned %d\n",rc);
	}

	// keep going until handshake complete.
	while (!getKeys)
	{
		if (gVerbose)
		{
			if (ms < 0)
			{
				printf("server: await packet up to overall timelimit\n");
			}
			else
			{
				printf("server: await packet within %dms\n",ms);
			}
		}

		pollResult = psDTLSSocketPoll(sa,ms);

		if (pollResult <= 0)
		{
			printf("server: expected packet did NOT arrive within %dms\n",ms);

			if (pollResult < 0)
			{
				printf("server: handshake timelimit reached\n");
				break;
			}
		}

		// We have had response packet or a retransmission timeout.
		// continue with handshake processing.
		if (gVerbose)
		{
			printf("server: invoking dtlslib_continue\n");
		}

		rc = dtlslib_continue(session);

		ms = -1;

		if (rc != 1)
		{
			ms = dtlslib_get_timeout_ms(session);
		}

		if (gVerbose)
		{
			printf("server: dtlslib_continue returned %d (next timeout %d ms)\n",rc,ms);
		}

		if (rc < 0)
		{
			// An error occurred, give up.
			break;
		}
		else if (rc == 1)
		{
			// The handshake has now completed.
			getKeys = 1;
		}
	}

	if (getKeys)
	{
		char fingerprint[kMaxFingerprint];

		if (gVerbose)
		{
			printf("server: Attempting to extract SRTP keys\n");
		}

		rc = dtlslib_extract_srtp_keys(session, &keys->fSRTPMode,&keys->fEncodeKey[0],&keys->fDecodeKey[0]);

		if (rc == 0) 
		{
			dumpKey("server",keys);

			// Get the fingerprint, if example included SIP/SDP, we would now check this against the fingerprint
			// we received in SDP.
			rc = dtlslib_get_remote_fingerprint(session,dtlslib_hash_type_sha_256,&fingerprint[0],sizeof(fingerprint));

			if (rc <= 0)
			{
				printf("error getting fingerprint\n");
				exit(2);
			}

			printf("server: remote fingerprint %s\n",&fingerprint[0]);
		} 
		else 
		{
			printf("Error extracting keys %d\n",rc);
		}
	}

	return getKeys;
}

// Given keys that were established in DTLS exchange, set up SRTP encryption.
static void setUpEncryptedSRTP( tKeyDetails* keys, tSMVMPrxId vmpRx, tSMVMPtxId vmpTx )
{
	int												rc;
	SM_VMPTX_CONFIG_AUTHENTICATION_HMAC_SHA1_PARMS	hmpTx;
	SM_VMPTX_CONFIG_ENCRYPTION_AES_CM_PARMS			cmpTx;
	SM_VMPRX_CONFIG_AUTHENTICATION_HMAC_SHA1_PARMS	hmpRx;
	SM_VMPRX_CONFIG_ENCRYPTION_AES_CM_PARMS			cmp;

	memset(&cmpTx,0,sizeof(cmpTx));

	cmpTx.vmptx  = vmpTx;
	cmpTx.keylen = sizeof(keys->fEncodeKey);
	cmpTx.key    = &keys->fEncodeKey[0];

	rc = sm_vmptx_config_encryption_aes_cm(&cmpTx);

	if (rc != 0)
	{
		printf("sm_vmptx_config_encryption_aes_cm %d\n",rc);
		exit(2);
	}

	memset(&hmpTx,0,sizeof(hmpTx));

	hmpTx.vmptx  = vmpTx;
	hmpTx.keylen = sizeof(keys->fEncodeKey);
	hmpTx.key    = &keys->fEncodeKey[0];
	hmpTx.taglen = (keys->fSRTPMode == dtlslib_srtp_mode_AES_CM_128_HMAC_SHA1_80) ? 80 : 32;

	rc = sm_vmptx_config_authentication_hmac_sha1(&hmpTx);

	if (rc != 0)
	{
		printf("sm_vmptx_config_authentication_hmac_sha1 %d\n",rc);
		exit(2);
	}

	memset(&cmp,0,sizeof(cmp));

	cmp.vmprx  = vmpRx;
	cmp.keylen = sizeof(keys->fDecodeKey);

	cmp.key = &keys->fDecodeKey[0];

	rc = sm_vmprx_config_encryption_aes_cm(&cmp);

	if (rc != 0)
	{
		printf("sm_vmprx_config_encryption_aes_cm error %d\n",rc);
		exit(2);
	}

	memset(&hmpRx,0,sizeof(hmpRx));

	hmpRx.vmprx  = vmpRx;
	hmpRx.keylen = sizeof(keys->fDecodeKey);
	hmpRx.key    = &keys->fDecodeKey[0];
	hmpRx.taglen = (keys->fSRTPMode == dtlslib_srtp_mode_AES_CM_128_HMAC_SHA1_80) ? 80 : 32;

	rc = sm_vmprx_config_authentication_hmac_sha1(&hmpRx);

	if (rc != 0)
	{
		printf("sm_vmprx_config_authentication_hmac_sha1 %d\n",rc);
		exit(2);
	}
}


static void usage( void )
{
	printf("dtls_server [-v] <ps-ip> <ps-key> <client-ip> <client-port>\n");
}

int main( int argc, char* argv[] )
{
	tPSDTLSSocketAccess		sa;
	uint16_t				serverPort;
	int						argIx;
	char*					psIP;
	char*					psKey;
	char*					clientIP;
	int  					clientPort;
	tCertParams				certParams;
	dtlslib* 				lib;
	dtlslib_cert*			cert;
	dtlslib_session*		session;
	tSMModuleId				moduleId;
	tSMVMPrxId				serverVMPRx;
	tSMVMPtxId				serverVMPTx;
	int						rc;
	SM_OPEN_PROSODY_X_PARMS opx;
	SM_OPEN_MODULE_PARMS	openModuleParms;
	SM_CLOSE_MODULE_PARMS 	closeModuleParms;
	SM_VMPRX_CREATE_PARMS	vmpRxCreateParms;
	SM_VMPTX_CREATE_PARMS	vmpTxCreateParms;
	SM_VMPRX_PORT_PARMS		vmpRxGetPortsParms;
	char 					fingerprint[kMaxFingerprint];
	char					locator[256];
	tKeyDetails				keys;

	// Set up parameters for cerificate to be created - determines strength of RSA keypair
	// used in DTLS exchange - 2048 bits typical these days.
	certParams.fHashType   		= dtlslib_hash_type_sha_256;
	certParams.fBitCount   		= 2048;
	certParams.fCommonName 		= (uint8_t*) "Aculab DTLS-SRTP";
	certParams.fSerial     		= -1;
	certParams.fLifetimeSeconds = 7 * 24 * 60 * 60;
	certParams.fCountry         = (uint8_t*) "UK";

	gVerbose = 0;

	argIx = 1;

	while ((argIx < argc) && (*(argv[argIx]) == '-'))
	{
		switch(*(argv[argIx]+1))
		{
			case 'v':
				gVerbose = 1;
				break;
			default:
				usage();
				exit(2);
		}
		argIx += 1;
	}

	if (argc-argIx < 4)
	{
		usage();
		exit(2);
	}

	// ProsodyS credentials.
	psIP       = argv[argIx+0];
	psKey      = argv[argIx+1];

	// Peer entity - DTLS client
	clientIP   = argv[argIx+2];
	clientPort = atoi(argv[argIx+3]);

	// Gain access to Prosody module.
	sprintf(&locator[0],"%s/%s",psIP,psKey);

	opx.card_id    = 99;
	opx.px_locator = &locator[0];

	rc = sm_open_prosody_x(&opx);

	if (rc != 0)
	{
		printf("error sm_open_prosody_x %s %d\n",&locator[0],rc);
		exit(2);
	}

	memset(&openModuleParms,0,sizeof(openModuleParms));

	openModuleParms.card_id   = opx.card_id;
	openModuleParms.module_ix = 0;

	rc = sm_open_module(&openModuleParms);

	if (rc != 0)
	{
		printf("error sm_open_module %d\n",rc);
		exit(2);
	}

	moduleId = openModuleParms.module_id;

	// Init interface to PS DTLS packet tx/rx
	rc = psDTLSSocketInitLib(1);

	if (rc != 0)
	{
		printf("error psDTLSSocketInitLib %d\n",rc);
		exit(2);
	}

	// Initialse DTLS use for application.
	lib = dtlslib_init(&gOpenSSLSCId[0],sizeof(gOpenSSLSCId));

	// Set up RTP resources for call.
	memset(&vmpRxCreateParms,0,sizeof(vmpRxCreateParms));

	vmpRxCreateParms.module = moduleId;
	vmpRxCreateParms.type   = kSMVMPrxTypeIPv4;

	rc = sm_vmprx_create(&vmpRxCreateParms);

	if (rc != 0)
	{
		printf("sm_vmprx_create failed %d\n",rc);
		exit(2);
	}

	memset(&vmpTxCreateParms,0,sizeof(vmpTxCreateParms));

	vmpTxCreateParms.module = moduleId;

	rc = sm_vmptx_create(&vmpTxCreateParms); 

	if (rc != 0)
	{
		printf("sm_vmptx_create failed %d\n",rc);
		exit(2);
	}

	serverVMPRx = vmpRxCreateParms.vmprx;
	serverVMPTx = vmpTxCreateParms.vmptx;

	memset(&vmpRxGetPortsParms,0,sizeof(vmpRxGetPortsParms));

	vmpRxGetPortsParms.vmprx = serverVMPRx;

	rc = sm_vmprx_get_ports(&vmpRxGetPortsParms);

	if (rc != 0)
	{
		printf("sm_vmprx_get_ports failed %d\n",rc);
		exit(2);
	}

	serverPort = vmpRxGetPortsParms.RTP_port;

	printf("server port %d\n",serverPort);

	// Set up DTLS UDP communcation - sharing the VMPRx socket.
	if (psDTLSSocketCreate(moduleId,serverVMPRx,&sa) != 0)
	{
		printf("Error: psDTLSSocketCreate failed\n");
		exit(1);
	}

	// Set the peer entity's address
	setDestAddr(sa,clientIP,clientPort);

	// Create the certificate containing public key to keep DTLS exchange confidential.
	cert = dtlslib_create_cert(	dtlslib_hash_type_sha_256,
								certParams.fBitCount,
								certParams.fSerial,
								certParams.fLifetimeSeconds,
								certParams.fCountry,
								certParams.fCommonName			);

	if (cert == 0)
	{
		printf("failed to create certificate\n");
		exit(1);
	}

	// Get fingerprint for this certificate, if this example included SIP/SDP processing
	// we would send this fingerprint in SDP so peer could check it against
	// certificate it receives during DTLS handshake.
	if (dtlslib_get_cert_fingerprint(cert,&fingerprint[0],sizeof(fingerprint)) <= 0)
	{
		printf("error: dtlslib_get_cert_fingerprint\n");
		exit(1);
	}

	printf("fingerprint from our certificate %s\n",&fingerprint[0]);

	// Set up resources for the DTLS handshake.
	session = dtlslib_create_session(lib,cert,psDTLSSocketSend,sa,psDTLSSocketRcv,sa);

	if (session == 0)
	{
		printf("error creating DTLS session\n");
		exit(1);
	}

	// Do the DTLS handshake
	if (performDTLSHandshake(sa,session,kMaxHandshakeDurationInSecs,&keys))
	{
		// DTLS hanshake succeeded - set up SRTP encryption accordingly.
		setUpEncryptedSRTP(&keys,serverVMPRx,serverVMPTx);
	}
	else
	{
		printf("DTLS handshake failed\n");
	}

	// Free resources associated with handshake
	dtlslib_free_cert(cert);
	dtlslib_free_session(session);

	psDTLSSocketDestroy(sa);

	rc = sm_vmprx_destroy(serverVMPRx);

	if (rc != 0)
	{
		fprintf(stderr, "Error: sm_vmprx_destroy %d\n",rc);
		exit(2);
	}

	rc = sm_vmptx_destroy(serverVMPTx);

	if (rc != 0)
	{
		fprintf(stderr, "Error: sm_vmptx_destroy %d\n",rc);
		exit(2);
	}

	// Application has finished with DTLS
	dtlslib_free(lib);

	// Finished use with Prosody module
	memset(&closeModuleParms,0,sizeof(closeModuleParms));

	closeModuleParms.module_id = moduleId;

	rc = sm_close_module(&closeModuleParms);

	if (rc != 0)
	{
		printf("error sm_close_module %d\n",rc);
		exit(2);
	}

	return 0;
}

