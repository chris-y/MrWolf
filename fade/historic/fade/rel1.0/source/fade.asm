.include 'tao'
.include 'time'
.include 'socket'
.include 'chris/tcall/tcall.inc'
.include 'chris/fade.inc'

tool 'app/fade/nettime',VP,TF_MAIN,8192,0
	ent - : -
	defbegin 0
		defp server,tms,tmp,host,dip,proto
		defi itmp,err,ipaddy
		defl secs,micros

	        qcall lib/argcargv,(-:p0 i0)

		printf "fade... by chris young\n"
		printf "(c) 2001 unsatisfactory software\n\n"

		;cpy.p vers,tmp
;		printf "%s\n",tmp

		if i0<3 ; 1=fname only
			printf "Template: app/fade/nettime server/a protocol/a\n"
			printf "Where protocol is 'daytime' 'timetcp' 'timeudp' or 'sntp'\n\n"
			; we need to break out of the program and clean up here
		        qcall lib/exit,(0:-)
		endif

		qcall lib/malloc,(20:proto)
		qcall lib/strcpy,(proto,tool.p:p~)
		cpy.p [p0+8],tmp
		qcall lib/unsat/joinstr,(proto,tmp:p~)
		cpy.p [p0+4],server ; note that server and p0 are the same thing

		printf "Server: %s ",server

		qcall lib/socket/gethostbyname,(server : host)
		if host==0
			printf "\n\nError with DNS lookup\n"
			qcall lib/exit,(-:-)
		endif
		cpy.i [[[host+h_addr_list]]],ipaddy
		qcall lib/socket/inet_ntoa,(ipaddy:dip)
		printf '[%s]\n\n',dip

		; *** SHOW CURRENT LOCAL TIME ***
		qcall lib/malloc,(8:tmp)
		qcall lib/time,(tmp : l0)
;		cpy.l l0,[tmp]
		qcall lib/localtime,(tmp : tms)
;		qcall lib/malloc,(TM_SIZE:tms)
		qcall lib/asctime,(tms : tmp)
		printf "Client time: %s",tmp
;		qcall lib/free,(tms : -)
		; *** END CURRENT LOCAL TIME BLOCK ***

		tcall proto,(ipaddy : secs,micros,err),itmp ;,VIRTUAL
		;app/fade/client/daytime
		if itmp=0
			cpy.i 3,err
		endif
		if err>0
			switch
				whencase err=1
				whencase err=2
				whencase err=3
				otherwise
					printf 'An unknown error occured.\n'
				break
				case err=1
					printf 'DNS lookup failed\n'
				break
				case err=2
					printf 'Unable to connect\n'
				break
				case err=3
					printf 'Protocol module %s not found\n',proto
				break
			endswitch
			qcall lib/exit,(-:-)
		endif
		cpy.l (secs*1e6)+micros,micros
		qcall lib/setmicrotime,(micros:itmp)
		cpy.l secs,[tmp]
		qcall lib/localtime,(tmp : tms)
		qcall lib/asctime,(tms : tmp)
		printf "Server time: %s\n",tmp
		qcall lib/free,(tmp : -)
		if itmp=-1
			printf 'Unable to set clock!\n'
		else
			printf 'Clock synchronised with server.\n'
		endif
        qcall lib/exit,(-:-)
	defend
;	ret

data
	tool:
		dc.b 'app/fade/client/',0
	vers:
		dc.b 0,'$VER: fade 1.0 (29.10.2001)',0
toolend

tool 'app/fade/client/daytime',VP
	ent i0 : l0 l1 i0
	; inputs  	p0 = ip address
	; outputs 	l0 = seconds
	;			l1 = micros
	;			i0 = error code
	defbegin 0
		defp server,host,buffer,saddr,tmp,tms
		defi ipaddy,i_tmp,bytes,sock,port
		defl secs,micros
		
		cpy.l 0,micros

		qcall lib/socket/socket,(AF_INET,SOCK_STREAM,IPPROTO_TCP : sock)
		qcall lib/malloc,(SOCKADDR_IN_SIZE : saddr)
		qcall lib/socket/htons,(13:port)
		cpy.s port,[saddr+sin_port]
		cpy.s AF_INET,[saddr+sin_family]
		cpy.i ipaddy,[saddr+sin_addr]
		
		qcall lib/socket/connect,(sock,saddr,sockaddr_in_size : i_tmp)

		;printf "connect %d (-1 = error) ; %d\n",i_tmp,[gp+proc_errno]

		if i_tmp=-1
			cpy 2,i0
			ret
		endif
		qcall lib/malloc,(50 : buffer)
	
;		qcall lib/socket/send,(sock,buffer,0,0 : bytes)
		qcall lib/socket/recv,(sock,buffer,50,0 : bytes)
		cpy.b 0,[buffer+bytes-2]
;		printf "%s %d\n",buffer,bytes
		qcall lib/close,(sock : i_tmp)
		
		qcall lib/malloc,(TM_SIZE:tms)

		qcall lib/strptime,(buffer,format.p,tms:p0)
;		printf '%d/%d/%d - %s - %ld - %s\n',[tms+tm_mday],[tms+tm_mon],[tms+tm_year],buffer,l0,p0
		qcall lib/mktime,(tms:l0)
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
		dc.b '$VER: app/fade/client/daytime 1.0 (29.10.2001)',0
;	form2:
;		dc.b '%a %b %d %H:%M:%S %Y',0
toolend

tool 'app/fade/client/timetcp',VP
	ent i0 : l0 l1 i0
	; inputs  	i0 = ip address
	; outputs 	l0 = seconds
	;			l1 = micros
	;			i0 = error code
	defbegin 0
		defp server,host,buffer,saddr,tmp,tms,buftwo
		defi ipaddy,i_tmp,bytes,sock,port
		defl secs,micros
		
		cpy.l 0,micros

		qcall lib/socket/socket,(AF_INET,SOCK_STREAM,IPPROTO_TCP : sock)
		qcall lib/malloc,(SOCKADDR_IN_SIZE : saddr)
		qcall lib/socket/htons,(37:port)
		cpy.s port,[saddr+sin_port]
		cpy.s AF_INET,[saddr+sin_family]
		cpy.i ipaddy,[saddr+sin_addr]
		
		qcall lib/socket/connect,(sock,saddr,sockaddr_in_size : i_tmp)

		;printf "connect %d (-1 = error) ; %d\n",i_tmp,[gp+proc_errno]

		if i_tmp=-1
			cpy 2,i0
			ret
		endif
		qcall lib/malloc,(8 : buffer)
	
;		qcall lib/socket/send,(sock,buffer,0,0 : bytes)
		qcall lib/socket/recv,(sock,buffer,4,0 : bytes)
		qcall lib/socket/ntohl,([buffer]:i_tmp)
;		cpy.i (70*365*24*3600)+(17*24*3600),i_tmp
		cpy.i i_tmp-2208988800,[buffer] ; subtract secs between 1900-1970
		cpy.i 0,[buffer+4]
		cpy.l [buffer],secs
;		printf "%ld %d\n",secs,bytes
		qcall lib/close,(sock : i_tmp)
		cpy.l [buffer],secs
		cpy.l 0,micros
		cpy.i 0,i0
	ret
	defend

data
	version:
		dc.b '$VER: app/fade/client/timetcp 1.0 (03.11.2001)',0
toolend

tool 'app/fade/client/timeudp',VP
	ent i0 : l0 l1 i0
	; inputs  	i0 = ip address
	; outputs 	l0 = seconds
	;			l1 = micros
	;			i0 = error code
	defbegin 0
		defp server,host,buffer,saddr,tmp,tms,buftwo
		defi ipaddy,i_tmp,bytes,sock,port
		defl secs,micros
		
		cpy.l 0,micros

		qcall lib/socket/socket,(AF_INET,SOCK_DGRAM,IPPROTO_UDP : sock)
		qcall lib/malloc,(SOCKADDR_IN_SIZE : saddr)
		qcall lib/socket/htons,(37:port)
		cpy.s port,[saddr+sin_port]
		cpy.s AF_INET,[saddr+sin_family]
		cpy.i ipaddy,[saddr+sin_addr]
		
		qcall lib/socket/connect,(sock,saddr,sockaddr_in_size : i_tmp)

		;printf "connect %d (-1 = error) ; %d\n",i_tmp,[gp+proc_errno]

		if i_tmp=-1
			cpy 2,i0
			ret
		endif
		qcall lib/malloc,(8 : buffer)
	
		qcall lib/socket/send,(sock,buffer,0,0 : bytes)
		qcall lib/socket/recv,(sock,buffer,4,0 : bytes)
		qcall lib/socket/ntohl,([buffer]:i_tmp)
;		cpy.i (70*365*24*3600)+(17*24*3600),i_tmp
		cpy.i i_tmp-2208988800,[buffer] ; subtract secs between 1900-1970
		cpy.i 0,[buffer+4]
		cpy.l [buffer],secs
;		printf "%ld %d\n",secs,bytes
		qcall lib/close,(sock : i_tmp)
		cpy.l [buffer],secs
		cpy.l 0,micros
		cpy.i 0,i0
	ret
	defend

data
	version:
		dc.b '$VER: app/fade/client/timeudp 1.0 (03.11.2001)',0
toolend

tool 'app/fade/client/sntp',VP
	ent i0 : l0 l1 i0
	; inputs  	i0 = ip address
	; outputs 	l0 = seconds
	;			l1 = micros
	;			i0 = error code
	defbegin 0
		defp server,host,buffer,saddr,tmp,tms,buftwo
		defi ipaddy,i_tmp,bytes,sock,port,secs,micros
		defl lsecs,lmicros
		defd timestamp
		
		;cpy.l 0,micros

		qcall lib/socket/socket,(AF_INET,SOCK_DGRAM,IPPROTO_UDP : sock)
		qcall lib/malloc,(SOCKADDR_IN_SIZE : saddr)
		qcall lib/socket/htons,(123:port)
		cpy.s port,[saddr+sin_port]
		cpy.s AF_INET,[saddr+sin_family]
		cpy.i ipaddy,[saddr+sin_addr]
		
		qcall lib/socket/connect,(sock,saddr,sockaddr_in_size : i_tmp)

		;printf "connect %d (-1 = error) ; %d\n",i_tmp,[gp+proc_errno]

		if i_tmp=-1
			cpy 2,i0
			ret
		endif
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

		cpy.d [buffer+ntp_recvtime],timestamp
		qcall lib/socket/ntohl,([buffer+ntp_recvtime]:secs)
		cpy.i secs-2208988800,secs ; subtract secs between 1900-1970
		qcall lib/socket/ntohl,([buffer+ntp_recvtime+32]:micros)
		cpy.i (micros*1000000)>>32,micros
;		cpy.d timestamp/(2^32),timestamp
;		cpy.d timestamp,lmicros
		qcall lib/free,(buffer:-)
		qcall lib/malloc,(8 : buffer)
		cpy.i secs,[buffer]
		cpy.i 0,[buffer+4]
		cpy.l [buffer],lsecs
		cpy.i micros,[buffer]
		cpy.l [buffer],lmicros
		qcall lib/close,(sock : i_tmp)
		cpy.i 0,i0
	ret
	defend

data
	version:
		dc.b '$VER: app/fade/client/sntp 1.0 (03.11.2001)',0
toolend

.end

