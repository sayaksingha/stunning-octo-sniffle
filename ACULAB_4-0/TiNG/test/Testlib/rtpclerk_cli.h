#ifdef TiNGTYPE_LINUX
typedef int RTPCLERK_SOCKET;
#endif

#ifdef TiNGTYPE_QNX
typedef int RTPCLERK_SOCKET;
#endif


#ifdef TiNGTYPE_WINNT
#include "../../libutil/WINNT/wind.h"
typedef SOCKET RTPCLERK_SOCKET;
#endif

RTPCLERK_SOCKET rtpclerk_cli_connect(char *ip, unsigned short port);
int rtpclerk_cli_post_port(RTPCLERK_SOCKET sock, char *myip, unsigned short port);
int rtpclerk_cli_request_port(RTPCLERK_SOCKET sock, char **msg);
int rtpclerk_cli_msg2port(char *msg, char *ipaddr, short *port);
