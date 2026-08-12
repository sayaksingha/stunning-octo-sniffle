typedef void* tPSDTLSSocketAccess;

int psDTLSSocketInitLib( int maxSockets );
void psDTLSSocketDtorLib( void );

int psDTLSSocketCreate( tSMModuleId moduleId, tSMVMPrxId vmprx, tPSDTLSSocketAccess* pSA );
int psDTLSSocketDestroy( tPSDTLSSocketAccess sa );

int psDTLSSocketConfigIPv4( tPSDTLSSocketAccess sa, SOCKADDR_IN* src, SOCKADDR_IN* dst );
int psDTLSSocketConfigIPv6( tPSDTLSSocketAccess sa, SOCKADDR_IN6* src, SOCKADDR_IN6* dst );

void psDTLSSocketSetLimitTime( tPSDTLSSocketAccess sa, int secsFromNow );

// return 0 if no packet arrived, 1 if packet is waiting, -1 if limit time reached, -2 if limit not set
int psDTLSSocketPoll( tPSDTLSSocketAccess sa, int timeoutInMilliSecs );

// abort any ongoing psDTLSSocketPoll
void psDTLSSocketPollAbort( tPSDTLSSocketAccess sa );


// All these functions return either the amount of data successfully read or written 
// (if the return value is positive) or that no data was successfully read or written 
// if the result is 0 or -1. If the return value is -2 then the operation is not 
// implemented in the specific BIO type.
int psDTLSSocketSend(void* ctx, const char* data, unsigned len);
int psDTLSSocketRcv(void* ctx, char* data, unsigned max_len);

