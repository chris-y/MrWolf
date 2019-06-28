.include 'tao'
.include 'time'
.include 'socket'
.include 'chris/tcall/tcall.inc'
.include 'chris/fade.inc'

tool 'app/fade/faded',VP,TF_MAIN,8192,0
	ent - : -
	defbegin
		defp ptmp,tmp,buffer,host,saddr,bindsaddr,tms,daemon,proton,fdset,sin,qsaddr,ntpbuffer
		defi len,ipaddy,port,itmp,sockt,proto,sock,connsock,ipproto,qsock,qit,fhand,bufsize

		printf "faded 2.0\n(c) 2002 unsatisfactory software\nhttp://www.unsatisfactorysoftware.co.uk\n\n"

	        qcall lib/argcargv,(-:p0 i0)
		cpy.i 0,port
		cpy.i 0,ipproto
		qcall lib/malloc,(4 : proton)
		cpy.p tcp,proton
		cpy.i 9,qit
		qcall lib/malloc,(ntp_size:ntpbuffer)
		
			switch
				whencase i0=4
				whencase i0=3
				whencase i0=2
				otherwise
					;if i0>3 ; 1=fname only
					printf "Template: app/fade/faded service/a protocol port\n"
					printf "Where service is 'daytime' 'time' 'sntp' or 'fade'\n\n"
					; we need to break out of the program and clean up here
		        	qcall lib/exit,(0:-)
				break

				case i0=4
					cpy.p [p0+8],proton
					cpy.p [p0+12],tmp
					qcall lib/atoi,(tmp:port)
					cpy.p [p0+4],daemon
				break

				case i0=3
					cpy.p [p0+8],proton
					;qcall lib/atoi,(tmp:port)
					cpy.p [p0+4],daemon
				break
				case i0=2
					cpy.p [p0+4],daemon
				break
			endswitch

		qcall lib/malloc,(100 : buffer)
		qcall lib/socket/gethostname,(buffer,100:i~)
		qcall lib/socket/gethostbyname,(buffer : host)
		;qcall lib/free,(buffer:-)
		if host==0
			printf "\n\nError with DNS lookup\n"
			qcall lib/exit,(-:-)
		endif
		cpy.i [[[host+h_addr_list]]],ipaddy

qcall lib/strcmp,(tcp.p,proton:ipproto)
if ipproto=0
	cpy.i SOCK_STREAM,sockt
	cpy.i IPPROTO_TCP,proto
else
	cpy.i SOCK_DGRAM,sockt
	cpy.i IPPROTO_UDP,proto
endif

		if port==0
			qcall app/fade/lib/servport,(daemon,proton:port)
			if port ==0
				printf "ERROR: Unable to determine correct port.\nPlease specify port number on the command line.\n\n"
				qcall lib/exit,(-:-)
			endif
		endif
		qcall lib/socket/inet_ntoa,(ipaddy:tmp)
		printf "Attempting to initialise %s/%s daemon on %s:%d...\n",daemon,proton,tmp,port
		qcall lib/socket/htons,(port:port)

		qcall lib/malloc,(sockaddr_in_size : bindsaddr)
		qcall lib/malloc,(sockaddr_in_size : saddr)
;		qcall lib/malloc,(sockaddr_in_size : qsaddr)

		qcall lib/socket/socket,(AF_INET,sockt,proto : sock)
		cpy.s port,[bindsaddr+sin_port]
		cpy.s AF_INET,[bindsaddr+sin_family]
		cpy.i ipaddy,[bindsaddr+sin_addr]
		qcall lib/socket/bind,(sock,bindsaddr,sockaddr_in_size:itmp)

; set up quit socket
;		qcall lib/socket/socket,(AF_INET,SOCK_STREAM,IPPROTO_TCP : qsock)
;		qcall lib/socket/htons,(3233:port)
;		cpy.s port,[qsaddr+sin_port]
;		cpy.s AF_INET,[qsaddr+sin_family]
;		cpy.i ipaddy,[qsaddr+sin_addr]
;		qcall lib/socket/bind,(qsock,qsaddr,sockaddr_in_size:itmp)

;printf "fds\n"

;		FD_ZERO	fdset	
;		FD_SET sock,fdset
;		FD_SET qsock,fdset

;** quit sock
;qcall lib/socket/setsockopt,(qsock,SOL_SOCKET,SO_NOBLOCK,one.p,1 : itmp)
;printf "%d\n",itmp
;		qcall lib/socket/listen,(qsock,1:itmp)

;		qcall lib/socket/select,(qsock+1,fdset,fdset,fdset,0:itmp)
;printf "select %d\n",itmp
	if proto==IPPROTO_TCP
		qcall lib/socket/listen,(sock,5:itmp) ; 5=backlog
		if itmp!=0		
			printf 'Error listening on port.\n'
			qcall lib/exit,(-:-)
		endif
	endif
repeat
			printf 'Waiting for connection...\n'

	if proto==IPPROTO_TCP
		qcall lib/socket/accept,(sock,saddr,sockaddr_in_size:connsock)
	else
		qcall app/fade/lib/udpget,(ntpbuffer,sock,ntp_size : saddr,itmp)
		if itmp==-1
			printf 'Error receiving data from client\n\n'
			qcall lib/exit,(-:-)
		endif
	endif
		qcall lib/socket/inet_ntoa,([saddr+sin_addr]:tmp)
		printf 'Connection accepted from %s\n',tmp
		printf 'Sending time...\n'
	
;printf "Attempting to start %s daemon on port ",tmp
;if port==0
;	printf "[default port]...\n"
;else
;printf "%d...\n",port
;endif
		qcall lib/strcpy,(buffer,tool.p:p~)
		qcall app/fade/lib/joinstr,(buffer,daemon:p~)

		tcall buffer,(ntpbuffer,ntp_size : ntpbuffer,bufsize),itmp ;,VIRTUAL

	;;;send data
	if proto==IPPROTO_TCP
		qcall lib/socket/send,(connsock,ntpbuffer,bufsize,0 : itmp)
	else
		qcall lib/socket/sendto,(sock,ntpbuffer,saddr,bufsize,0,sockaddr_in_size : itmp)
	endif

;qcall lib/exit,(-:-) ; udp

if itmp==0
	printf "ERROR: %s daemon not found.\n\n",tmp
	qcall lib/exit,(-:-)
else
	if proto==IPPROTO_TCP
		qcall lib/close,(connsock : itmp)
	endif
endif

;		qcall app/fade/server/daytime,(ipaddy,port:-)

qcall lib/open,(qfile.p,O_RDONLY,0:fhand)
qcall lib/read,(fhand,buffer,4:itmp)
qcall lib/close,(fhand:i~)
qcall lib/strncmp,(buffer,daemon,4:qit)
until qit==0
printf "Quit message received, server shutting down...\n\n"
		qcall lib/exit,(-:-)
	defend
data
	tool:
		dc.b 'app/fade/server/',0
	version:
		dc.b 0,'$VER: app/fade/faded 2.0 (11.05.2002)',0
	tcp:
		dc.b 'tcp',0
	quit:
		dc.b 'quit',0
	qfile:
		dc.b 'app/fade/data/quit',0
toolend

tool 'app/fade/server/daytime',VP
	ent p0 i0 : p0 i0
	;*inputs
	;p0 = received buffer (udp)
	;i0 = buffer size (udp)
	;*outputs
	;p0 = buffer
	;i0 = size
	
	defbegin 0
		defi itmp
		defp atm

		qcall app/fade/lib/timestr,(-:atm)

		qcall lib/strlen,(atm:itmp)

		;printf '%s\n',atm

	ret
	defend

data
	version:
		dc.b 0,'$VER: app/fade/server/daytime 2.0 (11.05.2002)',0
toolend

tool 'app/fade/lib/udpget',VP
	ent p0 i0 i1 : p0 i0
	;*inputs
	;i0 = connected socket
	;p0 = buffer to recv to (or 0 to allocate a dummy buffer)
	;i1 = buffer size
	;*outputs
	;p0 = sockaddr
	;i0 = return code (-1 = error)
	
	defbegin 0
		defi connsock,bufsize,itmp,rtn
		defp buffer,tmp,atm,saddr,slen

	;udp test
		qcall lib/malloc,(4:slen)

	if p0==0
		cpy.p slen,buffer
		cpy.i 4,bufsize
	endif
		qcall lib/malloc,(sockaddr_in_size:saddr)
		cpy.i sockaddr_in_size,[slen]
		
		qcall lib/socket/recvfrom,(connsock,buffer,bufsize,0,saddr,slen : rtn)
		
		qcall lib/free,(slen:-)
		cpy.p saddr,p0
		cpy.i rtn,i0
		ret

	defend

data
	version:
		dc.b 0,'$VER: app/fade/lib/udpget 1.0 (11.05.2002)',0
toolend

tool 'app/fade/server/time',VP
	ent p0 i0 : p0 i0
	;*inputs
	;p0 = received buffer (udp)
	;i0 = buffer size (udp)
	;*outputs
	;p0 = buffer
	;i0 = size

	defbegin 0
		defi bufsize,connsock,itmp,secs
		defp buffer,tmp,saddr,bindsaddr,tms

		; *** SHOW CURRENT LOCAL TIME ***
		;qcall lib/malloc,(256:tmp)
		qcall lib/time,(0 : l0)
		cpy.i l2i(l0+2208988800),itmp ; add secs between 1900-1970
cpy.i 4,bufsize

qcall lib/socket/htonl,(itmp:itmp)
cpy.i itmp,[buffer]

;cpy.i l0>>32,l0
;		qcall lib/socket/htonl,(secs:secs)

;		qcall lib/gmtime,(l0 : tms)
;		qcall lib/asctime,(tms : tmp)
;		printf "Sending time...\n"
		; *** END CURRENT LOCAL TIME BLOCK ***
;		cpy.l (secs*1e6)+micros,micros

;		cpy.i (70*365*24*3600)+(17*24*3600),i_tmp
;		cpy.i 0,[buffer+4]
;		cpy.l [buffer],secs

;		qcall lib/strlen,(tmp:itmp)
				
;		qcall lib/socket/send,(connsock,secs,4,0 : itmp)
	ret
	defend

data
	version:
		dc.b 0,'$VER: app/fade/server/time 2.0 (12.05.2002)',0
toolend

tool 'app/fade/server/sntp',VP
; WARNING: THIS TOOL DOES NOT WORK!!!
	ent p0 i0 : p0 i0
	;*inputs
	;p0 = received buffer (udp)
	;i0 = buffer size (udp)
	;*outputs
	;p0 = buffer
	;i0 = size

	defbegin 0
		defi bufsize,connsock,itmp,secs
		defp buffer,tmp,saddr,bindsaddr,tms

		qcall lib/time,(0 : l1)
		qcall lib/microtime,(0 : l0)
		qcall app/fade/lib/micro2ntp,(l0,l1,buffer+ntp_recvtime :-)
		
		cpy.b 27,[buffer+ntp_livnmode] ;00011100 011  FIND RFC!
		cpy.b 0,[buffer+ntp_stratum]
		cpy.b 10,[buffer+ntp_poll]
		cpy.b -6,[buffer+ntp_precision]
		cpy.i 0,[buffer+ntp_rootdelay] ; total roundtrip delay - need to calculate
                                               ; from last sync
		cpy.i 0,[buffer+ntp_rootdisp] ; nominal error relative to primary ref
		cpy.i 0,[buffer+ntp_refid] ; ip address of reference source or LOCL
		cpy.l 0,[buffer+ntp_reftime] ; last set or corrected, copy from sync		
		qcall lib/time,(0 : l1)
		qcall lib/microtime,(0 : l0)
		qcall app/fade/lib/micro2ntp,(l0,l1,buffer+ntp_sendtime :-)
		
		;printf '%x %x\n',[buffer+ntp_sendtime],[buffer+ntp_sendtime+4]
	ret
	defend

data
	version:
		dc.b 0,'$VER: app/fade/server/sntp 2.0 (12.05.2002)',0
toolend

tool 'app/fade/server/ntp',VP
	ent p0 i0 : p0 i0
	;*inputs
	;p0 = buffer
	;i0 = size
	;*outputs
	;p0 = buffer
	;i0 = size
; WARNING: DOES NOT WORK YET!
	defbegin 0
		defi connsock,itmp
		defp buffer,tmp,saddr,bindsaddr,tms

	;	qcall lib/malloc,(ntp_size:buffer)
	;	qcall lib/socket/recv,(connsock,buffer,ntp_size,0 : itmp)

		; check livnmode is correct, before replacing with:
		cpy.b [buffer+ntp_livnmode]+1,[buffer+ntp_livnmode] ;00011100 011  FIND RFC!
		; adding one to change from client to server packet   NTOHL needed???
		cpy.b 0,[buffer+ntp_stratum] ; set to sync'd stratum plus 1
		cpy.b 10,[buffer+ntp_poll]
		cpy.b -6,[buffer+ntp_precision]
		cpy.i 0,[buffer+ntp_rootdelay] ; total roundtrip delay - need to calculate
                                               ; from last sync
		cpy.i 0,[buffer+ntp_rootdisp] ; nominal error relative to primary ref
		cpy.i 0,[buffer+ntp_refid] ; ip address of reference source or LOCL
		cpy.l 0,[buffer+ntp_reftime] ; last set or corrected, copy from sync
		; filled in by client cpy.l 0,[buffer+ntp_origtime]
		cpy.l 0,[buffer+ntp_recvtime] ; time recvd
		cpy.l 0,[buffer+ntp_sendtime] ; time sent

;      Timestamp Name          ID   When Generated
;      ------------------------------------------------------------
;      Originate Timestamp     T1   time request sent by client
;      Receive Timestamp       T2   time request received by server
;      Transmit Timestamp      T3   time reply sent by server
;      Destination Timestamp   T4   time reply received by client
;
;   The roundtrip delay d and local clock offset t are defined as
;
;      d = (T4 - T1) - (T2 - T3)     t = ((T2 - T1) + (T3 - T4)) / 2.

;		cpy.d [buffer+ntp_recvtime],timestamp
;		qcall lib/socket/ntohl,([buffer+ntp_recvtime]:secs)
;		cpy.i secs-2208988800,secs ; subtract secs between 1900-1970
;		qcall lib/socket/ntohl,([buffer+ntp_recvtime+32]:micros)
;		cpy.i (micros*1000000)>>32,micros
;		cpy.d timestamp/(2^32),timestamp
;		cpy.d timestamp,lmicros
;		qcall lib/free,(buffer:-)
;		qcall lib/malloc,(8 : buffer)
;		cpy.i secs,[buffer]
;		cpy.i 0,[buffer+4]
;		cpy.l [buffer],lsecs
;		cpy.i micros,[buffer]
;		cpy.i 0,[buffer+4]
;		cpy.l [buffer],lmicros
;		qcall lib/close,(sock : i_tmp)
;		cpy.i 0,i0
				
;		qcall lib/socket/send,(connsock,buffer,ntp_size,0 : itmp)
;		qcall lib/free,(buffer:-)
	ret
	defend

data
	version:
		dc.b 0,'$VER: app/fade/server/ntp 2.0 (12.05.2002)',0
	lastpkt:
		dc.b 'app/fade/data/lastpkt',0
toolend

tool 'app/fade/server/fade',VP

	ent p0 i0 : p0 i0
	;*inputs
	;p0 = udp packet
	;i0 packet size
	;*outputs
	;p0 = buffer
	;i0 = size

	;	qcall lib/malloc,(256:p0)
		qcall lib/microtime,(p0 : l0)
		cpy.i 256,i0
	ret

data
	version:
		dc.b 0,'$VER: app/fade/server/fade 2.0 (11.05.2002)',0
toolend


.end

