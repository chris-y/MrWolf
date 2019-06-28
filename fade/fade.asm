.include 'tao'
.include 'time'
.include 'socket'
.include 'chris/tcall/tcall.inc'
.include 'chris/fade.inc'

tool 'app/fade/nettime',VP,TF_MAIN,8192,0
	ent - : -
	defbegin 0
		defp server,tms,tmp,host,dip,proto,pport,proton,daemon,saddr
		defi itmp,err,ipaddy,port,curserv,ipproto,i_tmp,sockt,sock
		defl secs,micros

		cpy.i 0,ipproto
		cpy.p tcp,proton

	        qcall lib/argcargv,(-:p0 i0)

		printf "fade 2.1... by chris young\n"
		printf "(c) 2001-2 unsatisfactory software\nhttp://www.unsatisfactorysoftware.co.uk\n\n"

		;cpy.p vers,tmp
;		printf "%s\n",tmp
				cpy.p [p0+8],tmp
		cpy.i 1,curserv
		
;		repeat
			switch
				whencase i0=4
				whencase i0=3
				whencase i0=2
				whencase i0=1
				otherwise
					;if i0>3 ; 1=fname only
					printf "Template: app/fade/nettime server service protocol\n"
					printf "Where service is 'daytime' 'time' 'sntp' 'fade'\n"
                                        printf "      protocol is 'tcp' or 'udp'\n\n"
					; we need to break out of the program and clean up here
		        	qcall lib/exit,(0:-)
				break
				case i0=2
					cpy.p [p0+4],server ; note that server and p0 are the same thing
					cpy.p sntp,daemon
					cpy.p udp,proton
				break
				case i0=1
					qcall app/fade/lib/readline,(slist.p,curserv,100:tmp)
					if tmp=0
						printf 'No more servers in app/fade/data/servers\n'
						qcall lib/exit,(-:-)
					endif
					qcall app/fade/lib/splstr,(tmp,1,' ',100,1:server)
					qcall app/fade/lib/splstr,(tmp,2,' ',100,1:daemon)
					qcall app/fade/lib/splstr,(tmp,3,' ',100,1:proton)
					cpy.b 0,[proton+3]
					; add protocol to file!!
				break
				case i0=3
					cpy.p [p0+8],daemon
					cpy.p [p0+4],server ; note that server and p0 are the same thing
				break
				case i0=4
					qcall lib/malloc,(4:proton)
					cpy.p [p0+12],proton
					cpy.p [p0+8],daemon
					cpy.p [p0+4],server ; note that server and p0 are the same thing
				break

			endswitch

		qcall lib/malloc,(30:proto)
		qcall lib/strcpy,(proto,tool.p:p~)
		qcall app/fade/lib/joinstr,(proto,daemon:p~)

		qcall app/fade/lib/splstr,(server,2,':',100,0:pport)
		qcall app/fade/lib/splstr,(server,1,':',100,0:server)

		printf "Server: %s ",server

		qcall lib/atoi,(pport:port)
;		printf ": %d ",port

		qcall lib/socket/gethostbyname,(server : host)
		if host==0
			printf "\n\nError with DNS lookup\n"
			qcall lib/exit,(-:-)
		endif
		cpy.i [[[host+h_addr_list]]],ipaddy
		qcall lib/socket/inet_ntoa,(ipaddy:dip)
		printf '[%s]\n\n',dip

qcall lib/strcmp,(tcp.p,proton:ipproto)
if ipproto=0
	cpy.i SOCK_STREAM,sockt
	cpy.i IPPROTO_TCP,ipproto
else
	cpy.i SOCK_DGRAM,sockt
	cpy.i IPPROTO_UDP,ipproto
endif

		if port==0
			qcall app/fade/lib/servport,(daemon,proton:port)
			if port ==0
				printf "ERROR: Unable to determine correct port.\nPlease specify port number on the command line.\n\n"
				qcall lib/exit,(-:-)
			endif
		endif

			printf "Connecting to %s/%s service on port %d...\n\n",daemon,proton,port

		qcall lib/socket/socket,(AF_INET,sockt,ipproto : sock)
		qcall lib/malloc,(SOCKADDR_IN_SIZE : saddr)
		qcall lib/socket/htons,(port:port)
		cpy.s port,[saddr+sin_port]
		cpy.s AF_INET,[saddr+sin_family]
		cpy.i ipaddy,[saddr+sin_addr]

;qcall lib/socket/setsockopt,(sock,SOL_SOCKET,SO_SNDTIMEO,timeout.p,1:i_tmp)
;printf 'sndtimeo %d\n',i_tmp
;qcall lib/socket/setsockopt,(sock,SOL_SOCKET,SO_RCVTIMEO,timeout.p,1:i_tmp)
;printf 'rcvtimeo %d\n',i_tmp
		
		qcall lib/socket/connect,(sock,saddr,sockaddr_in_size : i_tmp)

		;printf "connect %d (-1 = error) ; %d\n",i_tmp,[gp+proc_errno]

		if i_tmp=-1
			printf "Unable to connect.\n"
			qcall lib/exit,(-:-)
		endif

		tcall proto,(sock ipproto : micros,err),itmp ;,VIRTUAL
		qcall lib/close,(sock : i_tmp)

		;app/fade/client/daytime
		if itmp==0
			cpy.i 3,err
		endif
		if err>0
			switch
				whencase err=1
				whencase err=3
				otherwise
					printf 'An unknown error occured.\n'
				break
				case err=1
					printf 'Error receiving data\n'
				break
				case err=3
					printf 'Protocol module %s not found\n',proto
				break
			endswitch
			qcall lib/exit,(-:-)
		endif
;		cpy.l (secs*1e6)+micros,micros

		; *** SHOW CURRENT LOCAL TIME ***
		qcall app/fade/lib/timestr,(- : tmp)
		printf "Client time: %s",tmp

		; *** END CURRENT LOCAL TIME BLOCK ***

		qcall lib/setmicrotime,(micros:itmp)
		qcall app/fade/lib/timestr,(- : tmp)
;		qcall lib/malloc,(256:tmp)
;		qcall lib/time,(tmp:l0)
;		qcall lib/ctime,(tmp : tmp)
		printf "Server time: %s\n",tmp
;		qcall lib/free,(tmp : -)
		if itmp=-1
			printf 'Unable to set clock!\n'
		else
			printf 'Clock synchronised with server.\n'
		endif
;	if itmp=-1
;		cpy.i 1,itmp
;	endif
;	until itmp<>1
	qcall lib/exit,(-:-)
	defend
;	ret

data
	tool:
		dc.b 'app/fade/client/',0
	slist:
		dc.b 'app/fade/data/servers',0
	vers:
		dc.b 0,'$VER: fade 2.1 (12.05.2002)',0
	tcp:
		dc.b 'tcp',0
	udp:
		dc.b 'udp',0
	sntp:
		dc.b 'sntp',0
	timeout:
		dc.b 10,0
toolend

tool 'app/fade/client/daytime',VP
	ent i0 i1 : l0 i0
	; inputs  	p0 = socket
	;			p1 = protocol TCP or UDP
	; outputs 	l0 = seconds
	;			l1 = micros
	;			i0 = error code
	defbegin 0
		defp server,host,buffer,saddr,tmp,tms
		defi sock,ipproto,ipaddy,port,i_tmp,bytes
		defl micros,secs
		
;		if port=0
;			cpy.i 13,port
;		endif
		
		cpy.l 0,micros


		qcall lib/malloc,(50 : buffer)

if ipproto==IPPROTO_UDP
		qcall lib/socket/send,(sock,buffer,0,0 : bytes)
endif	
;		qcall lib/socket/send,(sock,buffer,0,0 : bytes)
		qcall lib/socket/recv,(sock,buffer,50,0 : bytes)
		
		if bytes==-1
			cpy.i 1,i0
			ret
		endif
		cpy.b 0,[buffer+bytes-2]
;		printf "%s %d\n",buffer,bytes
		
		qcall lib/malloc,(TM_SIZE:tms)

		qcall lib/strptime,(buffer,format.p,tms:p0)
;		printf '%d/%d/%d - %s - %ld - %s\n',[tms+tm_mday],[tms+tm_mon],[tms+tm_year],buffer,l0,p0
		qcall lib/mktime,(tms:secs)
		cpy.l (secs*1e6),micros ;+micros,micros

	; old stuff
	;qcall lib/time,(p2 : l0)
	;qcall lib/malloc,(8 : p1)
	;cpy.l [buffer],[p1] ; dummy line for testing porpoises
	;cpy.p p1,p0
		cpy.i 0,i0
	ret
	defend

data
	serv:
		dc.b 'time',0
	proto:
		dc.b 'tcp',0
	format:
		dc.b '%c',0
	version:
		dc.b 0,'$VER: app/fade/client/daytime 2.1 (12.05.2002)',0
;	form2:
;		dc.b '%a %b %d %H:%M:%S %Y',0
toolend

tool 'app/fade/client/time',VP
	ent i0 i1 : l0 i0
	; inputs  	i0 = socket
	;			i1 = ipproto (IPPROTO_TCP or _UDP)
	; outputs
	;			l0 = micros
	;			i0 = error code
	defbegin 0
		defp server,host,buffer,saddr,tmp,tms,buftwo
		defi sock,ipproto,ipaddy,port,i_tmp,bytes
		defl micros,secs
		
;		if port=0
;			cpy.i 37,port
;		endif

		cpy.l 0,micros

		qcall lib/malloc,(8 : buffer)
	
if ipproto==IPPROTO_UDP
		qcall lib/socket/send,(sock,buffer,0,0 : bytes)
endif
		qcall lib/socket/recv,(sock,buffer,4,0 : bytes)
		
				if bytes==-1
			cpy.i 1,i0
			ret
		endif
		
		qcall lib/socket/ntohl,([buffer]:i_tmp)
;		cpy.i (70*365*24*3600)+(17*24*3600),i_tmp
		cpy.i i_tmp-2208988800,[buffer] ; subtract secs between 1900-1970
;		cpy.i 0,[buffer+4]
		cpy.l i2l([buffer]),secs
;		printf "%ld %d\n",secs,bytes
		cpy.l (secs*1e6),micros ;+micros,micros
		cpy.i 0,i0
	ret
	defend

data
	version:
		dc.b '$VER: app/fade/client/time 2.1 (12.05.2002)',0
toolend

tool 'app/fade/client/fade',VP
	ent i0 i1 : l0 i0
	; inputs  	i0 = socket
	;		i1 = ip protocol
	; outputs 	l0 = seconds
	;			l0 = micros
	;			i0 = error code
	defbegin 0
		defp server,host,buffer,saddr,tmp,tms,buftwo
		defi sock,ipproto,ipaddy,port,i_tmp,bytes
		defl micros,secs
		
	;	if port=0
	;		cpy.i 3233,port
	;	endif

		cpy.l 0,micros

		qcall lib/malloc,(256 : buffer)

if ipproto==IPPROTO_UDP
		qcall lib/socket/send,(sock,buffer,0,0 : bytes)
endif

		qcall lib/socket/recv,(sock,buffer,256,0 : bytes)

		if bytes==-1
			cpy.i 1,i0
			ret
		endif

		cpy.l [buffer],micros
		cpy.i 0,i0
	ret
	defend

data
	version:
		dc.b '$VER: app/fade/client/fade 2.1 (12.05.2002)',0
toolend

tool 'app/fade/client/sntp',VP
	ent i0 i1 : l0 i0
	; inputs  	i0 = sock
	;			i1 = proto
	; outputs 
	;			l0 = micros
	;			i0 = error code
	defbegin 0
		defp server,host,buffer,saddr,tmp,tms,buftwo,longint
		defi sock,ipproto,ipaddy,port,i_tmp,bytes,secs,micros,fhand,osecs,omicros
		defl lmicros,lsecs,origl,refl,recvl,sendl,destl,delay,offset
		defd timestamp

		qcall lib/malloc,(ntp_size : buffer)

		cpy.b 27,[buffer+ntp_livnmode] ;00011011
		cpy.b 0,[buffer+ntp_stratum]
		cpy.b 0,[buffer+ntp_poll]
		cpy.b 0,[buffer+ntp_precision]
		cpy.i 0,[buffer+ntp_rootdelay]
		cpy.i 0,[buffer+ntp_rootdisp]
		cpy.i 0,[buffer+ntp_refid]
		cpy.l 0,[buffer+ntp_reftime]
		cpy.l 0,[buffer+ntp_origtime]
		cpy.l 0,[buffer+ntp_recvtime]
		cpy.l 0,[buffer+ntp_sendtime]

		qcall lib/socket/send,(sock,buffer,ntp_size,0 : bytes)
		qcall lib/socket/recv,(sock,buffer,ntp_size,0 : bytes)

		if bytes==-1
			cpy.i 1,i0
			ret
		endif
		
		qcall app/fade/lib/ntp2micro,(buffer+ntp_sendtime : lmicros)
;qcall lib/open,(pktfile.p,O_WRONLY|O_CREAT,S_IRUSR|S_IWUSR:fhand)
;qcall lib/write,(fhand,buffer,ntp_size:i~)
;qcall lib/close,(fhand:i~)


		qcall lib/free,(buffer:-)
		cpy.i 0,i0
	ret
	defend

data
	version:
		dc.b 0,'$VER: app/fade/client/sntp 2.2 (12.05.2002)',0
	pktfile:
		dc.b 'app/fade/data/lastpkt',0
toolend

tool 'app/fade/client/ntp',VP
	ent i0 i1 : l0 i0
	; inputs  	i0 = sock
	;			i1 = proto
	; outputs
	;			l0 = micros
	;			i0 = error code
	defbegin 0
		defp server,host,buffer,saddr,tmp,tms,buftwo,longint
		defi sock,ipproto,ipaddy,port,i_tmp,bytes,secs,micros,fhand,osecs,omicros
		defl lmicros,lsecs,origl,refl,recvl,sendl,destl,delay,offset,origs
		defd timestamp

;		if port=0
;			cpy.i 123,port
;		endif
		
		;cpy.l 0,micros

		qcall lib/malloc,(256:longint)
		qcall lib/malloc,(ntp_size : buffer)

		cpy.b 27,[buffer+ntp_livnmode] ;00011011
		cpy.b 0,[buffer+ntp_stratum]
		cpy.b 0,[buffer+ntp_poll]
		cpy.b 0,[buffer+ntp_precision]
		cpy.i 0,[buffer+ntp_rootdelay]
		cpy.i 0,[buffer+ntp_rootdisp]
		cpy.i 0,[buffer+ntp_refid]
		cpy.l 0,[buffer+ntp_reftime]
		cpy.l 0,[buffer+ntp_origtime]
		cpy.l 0,[buffer+ntp_recvtime]
		cpy.l 0,[buffer+ntp_sendtime]

; work out originating time and send it to the server
		qcall lib/time,(0:origs)
		qcall lib/microtime,(0:origl)
;		cpy.l origl-(lsecs*1e6),lmicros ; subtract whole secs from mics
;		cpy.i l2i(lsecs+2208988800),secs
;		cpy.i l2i((lmicros*1e6)*2^32),micros

;		printf "secs: %x micros: %x\n",secs,micros


;		qcall lib/socket/htonl,(secs:osecs)
;		cpy.i osecs,[buffer+ntp_origtime]
;		qcall lib/socket/htonl,(micros:omicros)
;		cpy.i omicros,[buffer+ntp_origtime+4]

	qcall app/fade/lib/micro2ntp,(origl,origs,buffer+ntp_origtime :-)
		
; **
		qcall lib/socket/send,(sock,buffer,ntp_size,0 : bytes)
		qcall lib/socket/recv,(sock,buffer,ntp_size,0 : bytes)
		
				if bytes==-1
			cpy.i 1,i0
			ret
		endif
		
		
;		printf 'received %d bytes\n',bytes
		
; SHOULD BE USING TRANSMIT TIME, NOT RECV TIME
;		cpy.d [buffer+ntp_sendtime],timestamp
		qcall lib/socket/ntohl,([buffer+ntp_sendtime]:secs)
		cpy.i secs-2208988800,secs ; subtract secs between 1900-1970

	;	cpy.i ([buffer+ntp_sendtime+32]*1000000)>>32,micros
		qcall lib/socket/ntohl,([buffer+ntp_sendtime+4]:micros)
		cpy.i (micros/(2^32))/1e6,micros
;		qcall lib/socket/ntohl,([buffer+ntp_sendtime+32]:micros)
;		cpy.l ([buffer+ntp_sendtime])>>32,lmicros
;		cpy.d timestamp/(2^32),timestamp
;		cpy.d timestamp,lmicros
;		qcall lib/free,(buffer:-)
;		qcall lib/malloc,(8 : buffer)
		cpy.l i2l(secs),lsecs ;[buffer]
;		cpy.i 0,[buffer+4]
;		cpy.l [buffer],lsecs
		cpy.l i2l(micros),lmicros ;[buffer]
;		cpy.i 0,[buffer+4]
;		cpy.l [buffer],lmicros
;printf ".. %ld ..",lmicros
cpy.l (lsecs*1e6)+lmicros,lmicros

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

; ***** calculate
qcall app/fade/lib/ntp2micro,(buffer+ntp_recvtime : recvl)
qcall app/fade/lib/ntp2micro,(buffer+ntp_sendtime : sendl)
qcall lib/microtime,(0:destl)

cpy.l ((destl-origl)-(recvl-sendl)),delay
cpy.l (((recvl-origl)+(sendl-destl))/2),offset

;Root Delay: This is a 32-bit signed fixed-point number indicating the
;total roundtrip delay to the primary reference source, in seconds with
;fraction point between bits 15 and 16. Note that this variable can take
;on both positive and negative values, depending on clock precision and
;skew.

;scale micros to 16.16 bit value.  65536   (micros*65536)/1e6
		cpy.l (delay/1e6),lsecs ; subtract whole secs from mics
		cpy.i l2i(lsecs),secs
		qcall lib/socket/htons,(secs:secs)
		cpy.l (delay*65536)/1e6,delay
		cpy.i l2i(delay),micros
		qcall lib/socket/htons,(micros:micros)


cpy.s secs+[buffer+ntp_rootdelay],[buffer+ntp_rootdelay]
cpy.s micros+[buffer+ntp_rootdelay+2],[buffer+ntp_rootdelay+2]

	qcall app/fade/lib/micro2ntp,(origl,origs,buffer+ntp_origtime :-)
;	cpy.l [buffer+ntp_sendtime],[buffer+ntp_reftime]
	
;		cpy.i osecs,[buffer+ntp_origtime]
;		cpy.i omicros,[buffer+ntp_origtime+4]

qcall lib/open,(pktfile.p,O_WRONLY|O_CREAT,S_IRUSR|S_IWUSR:fhand)
qcall lib/write,(fhand,buffer,ntp_size:i~)
qcall lib/close,(fhand:i~)


		qcall lib/free,(buffer:-)
		
; micros problem!!!!   ,lmicros ;
; 2mins 18 secs out
;		qcall lib/close,(sock : i_tmp)
		cpy.i 0,i0
	ret
	defend

data
	pktfile:
		dc.b 'app/fade/data/lastpkt',0
	version:
		dc.b 0,'$VER: app/fade/client/ntp 2.0 (12.05.2002)',0
toolend

tool 'app/fade/lib/ntp2micro',VP
	ent p0 : l0
	; p0 = pointer to ntp timestamp
	; l0 = intent micros
	defbegin 0

		defp ntpstamp
		defl lmicros,lsecs
		defi secs,micros
		
		qcall lib/socket/ntohl,([ntpstamp]:secs)
		cpy.i secs-2208988800,secs ; subtract secs between 1900-1970
		qcall lib/socket/ntohl,([ntpstamp+4]:micros)
		cpy.l i2l((micros*1e6)/(2^32)),lmicros
		cpy.l i2l(secs),lsecs ;[buffer]
		cpy.l (lsecs*1e6)+lmicros,lmicros
		ret
	defend 0
data
	ver:
		dc.b 0,'$VER: app/fade/lib/ntp2micro 1.0 (12.05.2002)',0
toolend

tool 'app/fade/lib/micro2ntp',VP
	ent l0 l1 p0 : -
	; l0 = micros
	; l1 = secs
	; po = buffer
	
	defbegin 0

		defp ntptime
		defl lmicros,lsecs
		defi micros,secs
		
		cpy.l lmicros-(lsecs*1e6),lmicros ; subtract whole secs from mics
		cpy.i l2i(lsecs+2208988800),secs
		cpy.l (lmicros*(2^32))/1e6,lmicros
		cpy.i l2i(lmicros),micros
		printf "secs: %x micros: %x\n",secs,micros

		qcall lib/socket/htonl,(secs:secs)
		cpy.i secs,[ntptime]
		qcall lib/socket/htonl,(micros:micros)
		cpy.i micros,[ntptime+4]
		ret
	defend 0
data
	ver:
		dc.b 0,'$VER: app/fade/lib/micro2ntp 1.0 (12.05.2002)',0
toolend

tool 'app/fade/lib/readline',VP

	ent p0 i0 i1: p0
	; inputs  p0 = filename
	;         i0 = line number
	;	  i1 = max size
	; outputs p0 = line
	defbegin 0
		defp fname,buffer,data,tmp,fhand
		defi line,size,itmp,tchar,cline,cchar

		qcall lib/fopen,(fname,mread.p : fhand)
		
		if fhand==0
			cpy.p 0,p0
			ret
		endif

		qcall lib/malloc,(size : buffer)

		cpy.i 0,cline
		while cline<line
			qcall lib/fgets,(buffer,size,fhand : tmp)		
				inc cline
			if tmp<=0
				qcall lib/fclose,(fhand : i~)
				qcall lib/free,(buffer :-)
				cpy.p 0,p0
				ret
			endif
		endwhile

		qcall lib/fclose,(fhand : i~)
	
		;printf '%s\n',buffer

		;qcall lib/free,(buffer :-)
		
		cpy.p buffer,p0
		ret
	defend 0
	
data
	version:
		dc.b 0,'$VER: lib/unsat/readline 2.0 (05.05.2002)',0
	mread:
		dc.b 'r',0
toolend

tool 'app/fade/lib/splstr',VP

	ent p0 i0 i1 i2 i3: p0
	; inputs  p0 = string
	;         i0 = field number
	;         i1 = delimiter
	;         i2 = max length
	;		  i3 = merge (1=yes, 0=no)
	; outputs p0 = string
	defbegin 0
		defp str,splitstr
		defi field,delimiter,size,merge,itmp,tchar,cfield,cchar

		qcall lib/malloc,(size : splitstr)
		cpy.i 0,itmp
		cpy.i 0,cfield
		cpy.i 0,cchar
		while cfield<field
			cpy.b [str+cchar],tchar
			cpy.b tchar,[splitstr+itmp]
			inc cchar
			inc itmp
			if tchar=delimiter or cchar>=size
				if itmp>merge
					inc cfield
				endif
				cpy.b 0,[splitstr+itmp-1]
				cpy.i 0,itmp
			endif
		endwhile

		cpy.p splitstr,p0
		ret
	defend 0
	
data
	version:
		dc.b 0,'$VER: lib/unsat/splstr 1.0 (25.10.2001)',0
toolend

tool 'app/fade/lib/joinstr',VP

	ent p0 p1 : p0
	; inputs  p0 = string
	;         p1 = string to join
	; outputs p0 = string
	defbegin 0
		defp str,jstr
		defi itmp
		cpy.i 0,itmp
		qcall lib/strlen,(str : itmp)
		qcall lib/strcpy,(str+itmp,jstr : p~ )

		ret
	defend 0
	
data
	version:
		dc.b 0,'$VER: lib/unsat/joinstr 1.0 (24.10.2001)',0
toolend

tool 'lib/unsat/substr',VP
; not needed for FADE

	ent p0 i0 i1 : p0
	; inputs  p0 = string
	;         i0 = char to find
	;         i1 = char to replace with
	;
	; outputs p0 = string
	
	defbegin 0
		defp str
		defi char1,char2,tchar,itmp,len

		cpy.i 0,itmp
		qcall lib/strlen,(str : len)
		while itmp<len
			cpy.b [str+itmp],tchar
			if tchar=char1
				cpy.b char2,[str+itmp]
			endif
			inc itmp
		endwhile
		;cpy.p str,p0
		ret
	defend 0
	
data
	version:
		dc.b 0,'$VER: lib/unsat/substr 1.0 (24.10.2001)',0
toolend

tool 'app/fade/lib/timestr',VP
; returns current time as a time string
	ent - : p0
	defbegin 0
		defp atm,tmp ;,tms
		
		; *** SHOW CURRENT LOCAL TIME ***
		qcall lib/malloc,(256:tmp)
		qcall lib/time,(tmp : l0)
		;qcall lib/localtime,(tmp : tms)
		qcall lib/ctime,(tmp : atm)
		qcall lib/free,(tmp:-)
		; *** END CURRENT LOCAL TIME BLOCK ***
		ret
	defend 0

data
	version:
		dc.b 0,'$VER: app/fade/lib/timestr 1.0 (11.05.2002)',0
		
toolend

tool 'app/fade/lib/servport',VP
; returns port number given service and protocol
	ent p0 p1 : i0
	defbegin 0
		defp daemon,proton,tmp
		defi port
		
			;cpy.i 13,port
			qcall lib/socket/getservbyname,(daemon,proton:tmp)
			if tmp==0
				cpy.i 0,i0
				ret
			endif
	;		printf "%s\n",[tmp+serv_ent_port]
			cpy.i [tmp+serv_ent_port],port
			qcall lib/socket/ntohs,(port:port)
;			printf "resolved port: %d\n",port
			; do a reverse lookup on service name here

		ret
	defend 0

data
	version:
		dc.b 0,'$VER: app/fade/lib/servport 1.0 (11.05.2002)',0
		
toolend




.end

