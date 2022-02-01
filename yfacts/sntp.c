/* SNTP module */
#include <proto/exec.h>
#include <proto/bsdsocket.h>
#include <proto/timer.h>

#include "sntp.h"
#include "error.h"
#include "yfacts.h"

#include <string.h>
#include <stdio.h>
#include <stdint.h>

struct Library *SocketBase = NULL;
struct SocketIFace *ISocket = NULL;

static struct sockaddr_in *addrs = NULL;

struct ntp_pkt {
	uint8_t li_vn_mode;
	uint8_t stratum;
	uint8_t poll;
	uint8_t precision;
	uint32_t root_delay;
	uint32_t root_disp;
	uint32_t reference_id;
	uint32_t reference_time_s;
	uint32_t reference_time_f;
	uint32_t originate_time_s;
	uint32_t originate_time_f;
	uint32_t receive_time_s;
	uint32_t receive_time_f;
	uint32_t transmit_time_s;
	uint32_t transmit_time_f;
};

#define GET_NTP_LI(PKT) ((PKT->li_vn_mode) >> 6) & 0x3
#define GET_NTP_VN(PKT) ((PKT->li_vn_mode) >> 3) & 0x7
#define GET_NTP_MODE(PKT) ((PKT->li_vn_mode)) & 0x7

#define SET_NTP_VN(PKT,VN) PKT->li_vn_mode |= ((VN & 0x7) << 3)
#define SET_NTP_MODE(PKT,MODE) PKT->li_vn_mode |= ((MODE & 0x7))

#define AMIGA_SECS_TO_NTP_S(S) ((S < 1833517696) ? (S + 2461449600) : (S - 1833517696))
#define AMIGA_MICRO_TO_NTP_F(M) (uint32_t)(((uint64_t)M << 32) / 1000000)
#define NTP_S_TO_AMIGA_SECS(S) ((S >> 31) ? (S - 2461449600) : (S + 1833517696))
#define NTP_F_TO_AMIGA_MICRO(F) (uint32_t)(((uint64_t)F * 1000000) >> 32)

//#define CALCDEBUG 1

static void sntp_closesock(long sock)
{
	if(sock >= 0) CloseSocket(sock);

#ifdef __amigaos4__
	DropInterface((struct Interface *)ISocket);
#endif
	CloseLibrary(SocketBase);
	ISocket = NULL;
	SocketBase = NULL;
}

static void sntp_cleanup(void)
{
	if(addrs != NULL) FreeVec(addrs);
	addrs = NULL;
}

static long sntp_connect(char *server, int port)
{
	struct hostent *remote;
	struct TimeVal time;
	long sock = -1;
	long conn = -1;

	if(port == 0) port = 123;

	if(addrs == NULL) {
		/* lookup address and cache it */
		addrs = AllocVec(sizeof(struct sockaddr_in), MEMF_CLEAR);
		if(addrs == NULL) return(-4);

		remote = gethostbyname((char *)server);

		if(remote == 0) {
			sntp_closesock(sock);
			return(-1);
		}

		addrs->sin_family = AF_INET;
		addrs->sin_len = sizeof(struct sockaddr_in);
		addrs->sin_port = port;

		CopyMem(remote->h_addr, &addrs->sin_addr, remote->h_length);
	}

	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(sock == -1) {
		addrs = NULL; /* Clear cache */
		sntp_closesock(sock);
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
		sntp_closesock(sock);
		return(-3);
	}

	return sock;
}


static int sntp_sync(char *server, int port, BOOL savesys, BOOL savebc)
{
	struct TV_compat or_tv;
	struct TV_compat rx_tv;
	struct TV_compat tx_tv;
	struct TV_compat de_tv;
	BOOL neg = FALSE;

	long sock = -1;
	long size = 0;

	if(SocketBase = OpenLibrary("bsdsocket.library",4)) {
#ifdef __amigaos4__
		ISocket = (struct SocketIFace *)GetInterface(SocketBase,"main",1,NULL);
#endif
	} else {
		return ERR_LIB;
	}

	sock = sntp_connect(server, port);
	if(sock < 0) {
		printf("%ld\n", sock);
		return ERR_CONN;
	}

	struct ntp_pkt *pkt = AllocVec(sizeof(struct ntp_pkt), MEMF_CLEAR | MEMF_PRIVATE);

	if(pkt == NULL) return ERR_MEM;

	SET_NTP_VN(pkt, 4); /* Version 4 */
	SET_NTP_MODE(pkt, 3); /* Client */

	GetSysTime(&tx_tv);
	tz_offset(&tx_tv);

	pkt->transmit_time_s = AMIGA_SECS_TO_NTP_S(tx_tv.Seconds);
	pkt->transmit_time_f = AMIGA_MICRO_TO_NTP_F(tx_tv.Microseconds);

	send(sock,pkt,48,NULL);
	size = recv(sock,pkt,48,NULL);

	GetSysTime(&de_tv);
	tz_offset(&de_tv);

	sntp_closesock(sock);

	if(pkt->stratum == 0) {
		sntp_cleanup();
		return ERR_KOD;
	}

	or_tv.Seconds = NTP_S_TO_AMIGA_SECS(pkt->originate_time_s);
	or_tv.Microseconds = NTP_F_TO_AMIGA_MICRO(pkt->originate_time_f);	

	rx_tv.Seconds = NTP_S_TO_AMIGA_SECS(pkt->receive_time_s);
	rx_tv.Microseconds = NTP_F_TO_AMIGA_MICRO(pkt->receive_time_f);	

	tx_tv.Seconds = NTP_S_TO_AMIGA_SECS(pkt->transmit_time_s);
	tx_tv.Microseconds = NTP_F_TO_AMIGA_MICRO(pkt->transmit_time_f);	

	/* roundtrip delay
	SubTime(&de_tv, &or_tv);
	SubTime(&tx_tv, &rx_tv);
	SubTime(&de_tv, &tx_tv);
	*/

	/* clock offset */
#if CALCDEBUG
	printf("or = %d.%d\n", or_tv.Seconds, or_tv.Microseconds);
	printf("rx = %d.%d\n", rx_tv.Seconds, rx_tv.Microseconds);
	printf("tx = %d.%d\n", tx_tv.Seconds, tx_tv.Microseconds);
	printf("de = %d.%d\n", de_tv.Seconds, de_tv.Microseconds);
#endif

	if(CmpTime(&rx_tv, &or_tv) > 0) {
		neg = TRUE;
		SubTime(&or_tv, &rx_tv);
		SubTime(&de_tv, &tx_tv);

		CopyMem(&or_tv, &rx_tv, sizeof(struct TV_compat));
		CopyMem(&de_tv, &tx_tv, sizeof(struct TV_compat));

	} else {
		SubTime(&rx_tv, &or_tv);
		SubTime(&tx_tv, &de_tv);
	}

#if CALCDEBUG
	printf("rx - or = %d.%d\n", rx_tv.Seconds, rx_tv.Microseconds);
	printf("tx - de = %d.%d\n", tx_tv.Seconds, tx_tv.Microseconds);
#endif

	AddTime(&rx_tv, &tx_tv);

#if CALCDEBUG
	printf("above added together = %d.%d\n", rx_tv.Seconds, rx_tv.Microseconds);
#endif

	rx_tv.Seconds /= 2;
	rx_tv.Microseconds /= 2;

#if CALCDEBUG
	printf("diff = %s %d.%d\n", neg ? "-" : "+", rx_tv.Seconds, rx_tv.Microseconds);
#endif

	GetSysTime(&de_tv);
	if(neg) {
		SubTime(&de_tv, &rx_tv); /* subtract time difference */
	} else {
		AddTime(&de_tv, &rx_tv); /* add time difference */
	}

	if(savesys) {
		set_sys_time(&de_tv);
	}

	if(savebc) {
		set_bc_time(&de_tv);
	}


	if(pkt) FreeVec(pkt);

	return ERR_OK;
}

static char *sntp_default_server(void)
{
	/* should be unsat.pool.ntp.org, but vendor domain request still pending */
	return strdup("pool.ntp.org\0"); 	
}

void sntp_register(struct module_functions *funcs)
{
	funcs->sync = sntp_sync;
	funcs->default_server = sntp_default_server;
	funcs->cleanup = sntp_cleanup;
}


