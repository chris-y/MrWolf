/* Find TZ using ip-api.com */
#include <proto/exec.h>
#include <proto/bsdsocket.h>
#include "ipapi.h"
#include "error.h"
#include "mrwolf.h"

#include <string.h>
#include <stdio.h>
#include <stdint.h>

static struct sockaddr_in *addrs = NULL;
static uint32 offset = -1;

static void ipapi_closesock(long sock)
{
	if(sock >= 0) CloseSocket(sock);
}

static long ipapi_connect(char *server, int port)
{
	struct hostent *remote;
	struct TV_compat time;
	long sock = -1;
	long conn = -1;

	if(addrs == NULL) {
		/* lookup address and cache it */
		addrs = AllocVec(sizeof(struct sockaddr_in), MEMF_CLEAR);
		if(addrs == NULL) return(-4);

		remote = gethostbyname((char *)server);

		if(remote == 0) {
			ipapi_closesock(sock);
			return(-1);
		}

		addrs->sin_family = AF_INET;
		addrs->sin_len = sizeof(struct sockaddr_in);
		addrs->sin_port = port;

		CopyMem(remote->h_addr, &addrs->sin_addr, remote->h_length);
	}

	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(sock == -1) {
		addrs = NULL; /* Clear cache */
		ipapi_closesock(sock);
		return(-2);
	}

	time.Seconds = 30; //timeout
	time.Microseconds = 0;

	setsockopt(sock, SOL_SOCKET,
		SO_RCVTIMEO, &time, sizeof(struct TimeVal));

	struct sockaddr *saddr = (struct sockaddr *)addrs;

	conn = connect(sock, saddr, saddr->sa_len);
	if(conn != 0) {
		printf("conn %d error %d\n", conn, errno);
		addrs = NULL; /* Clear cache */
		ipapi_closesock(sock);
		return(-3);
	}

	return sock;
}

#define HTTP_REQ "GET /line/?fields=status,message,offset HTTP/1.1\r\nHost:ip-api.com\r\n\r\n"
#define BUF_LEN 255

static int ipapi_tz(void)
{
	if(offset != -1) return offset;

	long sock = -1;
	long size = 0;

	char buf[BUF_LEN+1];

	if(SocketBase = NULL) {
		if(SocketBase = OpenLibrary("bsdsocket.library",4)) {
#ifdef __amigaos4__
			ISocket = (struct SocketIFace *)GetInterface(SocketBase,"main",1,NULL);
#endif
		} else {
			return ERR_LIB;
		}
	}

	sock = ipapi_connect("ip-api.com", 80);
	if(sock < 0) {
		printf("%ld\n", sock);
		return ERR_CONN;
	}

	send(sock, HTTP_REQ, strlen(HTTP_REQ), 0);
	size = recv(sock, buf, BUF_LEN, 0);

	//printf("%d: %s\n", size, buf);

	char *b = strstr(buf, "success\n");

	if(b != NULL) b+=8;
	
	offset = - (atol(b) / 60);

	ipapi_closesock(sock);

	return offset;
}

void ipapi_register(struct module_functions *funcs)
{
	funcs->tz = ipapi_tz;
}
