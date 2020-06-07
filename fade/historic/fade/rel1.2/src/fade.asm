.include 'tao'
.include 'time'
.include 'socket'
.include 'chris/tcall/tcall.inc'
.include 'chris/fade.inc'

tool 'app/fade/nettime',VP,TF_MAIN,8192,0
	ent - : -
	defbegin 0
		defp server,tms,tmp,host,dip,proto,pport
		defi itmp,err,ipaddy,port,curserv
		defl secs,micros

	        qcall lib/argcargv,(-:p0 i0)

		printf "fade 1.2... by chris young\n"
		printf "(c) 2001-2 unsatisfactory software\n\n"

		;cpy.p vers,tmp
;		printf "%s\n",tmp
				cpy.p [p0+8],tmp
		cpy.i 1,curserv
		
;		repeat
			switch
				whencase i0=3
;				whencase i0=2
				whencase i0=1
				otherwise
					;if i0>3 ; 1=fname only
					printf "Template: app/fade/nettime server/a protocol/a\n"
					printf "Where protocol is 'daytime' 'timetcp' 'timeudp' or 'sntp'\n\n"
					; we need to break out of the program and clean up here
		        	qcall lib/exit,(0:-)
				break
;				case i0=2
;					cpy.p 'sntp',[p0+8]
;				break
				case i0=1
					qcall lib/unsat/readline,(slist.p,curserv,100:tmp)
					if tmp=0
						printf 'No more servers in app/fade/servers\n'
						qcall lib/exit,(-:-)
					endif
					qcall lib/unsat/splstr,(tmp,1,' ',100,1:server)
					qcall lib/unsat/splstr,(tmp,2,' ',100,1:tmp)
				break
				case i0=3
					cpy.p [p0+8],tmp
					cpy.p [p0+4],server ; note that server and p0 are the same thing
				break
			endswitch

		qcall lib/malloc,(20:proto)
		qcall lib/strcpy,(proto,tool.p:p~)
		qcall lib/unsat/joinstr,(proto,tmp:p~)

		qcall lib/unsat/splstr,(server,2,':',100,0:pport)
		qcall lib/unsat/splstr,(server,1,':',100,0:server)

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

		tcall proto,(ipaddy,port : secs,micros,err),itmp ;,VIRTUAL
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
		dc.b 'app/fade/servers',0
	vers:
		dc.b 0,'$VER: fade 1.2 (24.02.2002)',0
toolend

tool 'app/fade/client/daytime',VP
	ent i0 i1 : l0 l1 i0
	; inputs  	p0 = ip address
	;			p1 = port
	; outputs 	l0 = seconds
	;			l1 = micros
	;			i0 = error code
	defbegin 0
		defp server,host,buffer,saddr,tmp,tms
		defi ipaddy,port,i_tmp,bytes,sock
		defl secs,micros
		
		if port=0
			cpy.i 13,port
		endif
		
		cpy.l 0,micros

		qcall lib/socket/socket,(AF_INET,SOCK_STREAM,IPPROTO_TCP : sock)
		qcall lib/malloc,(SOCKADDR_IN_SIZE : saddr)
		qcall lib/socket/htons,(port:port)
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
		dc.b '$VER: app/fade/client/daytime 1.1 (15.11.2001)',0
;	form2:
;		dc.b '%a %b %d %H:%M:%S %Y',0
toolend

tool 'app/fade/client/timetcp',VP
	ent i0 i1 : l0 l1 i0
	; inputs  	i0 = ip address
	;			i1 = port
	; outputs 	l0 = seconds
	;			l1 = micros
	;			i0 = error code
	defbegin 0
		defp server,host,buffer,saddr,tmp,tms,buftwo
		defi ipaddy,port,i_tmp,bytes,sock
		defl secs,micros
		
		if port=0
			cpy.i 37,port
		endif

		cpy.l 0,micros

		qcall lib/socket/socket,(AF_INET,SOCK_STREAM,IPPROTO_TCP : sock)
		qcall lib/malloc,(SOCKADDR_IN_SIZE : saddr)
		qcall lib/socket/htons,(port:port)
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
		dc.b '$VER: app/fade/client/timetcp 1.1 (15.11.2001)',0
toolend

tool 'app/fade/client/timeudp',VP
	ent i0 i1 : l0 l1 i0
	; inputs  	i0 = ip address
	;			i1 = port
	; outputs 	l0 = seconds
	;			l1 = micros
	;			i0 = error code
	defbegin 0
		defp server,host,buffer,saddr,tmp,tms,buftwo
		defi ipaddy,port,i_tmp,bytes,sock
		defl secs,micros

		if port=0
			cpy.i 37,port
		endif
		
		cpy.l 0,micros

		qcall lib/socket/socket,(AF_INET,SOCK_DGRAM,IPPROTO_UDP : sock)
		qcall lib/malloc,(SOCKADDR_IN_SIZE : saddr)
		qcall lib/socket/htons,(port:port)
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
		dc.b '$VER: app/fade/client/timeudp 1.1 (15.11.2001)',0
toolend

tool 'app/fade/client/sntp',VP
	ent i0 i1 : l0 l1 i0
	; inputs  	i0 = ip address
	;			i1 = port
	; outputs 	l0 = seconds
	;			l1 = micros
	;			i0 = error code
	defbegin 0
		defp server,host,buffer,saddr,tmp,tms,buftwo
		defi ipaddy,port,i_tmp,bytes,sock,secs,micros
		defl lsecs,lmicros
		defd timestamp

		if port=0
			cpy.i 123,port
		endif
		
		;cpy.l 0,micros

		qcall lib/socket/socket,(AF_INET,SOCK_DGRAM,IPPROTO_UDP : sock)
		qcall lib/malloc,(SOCKADDR_IN_SIZE : saddr)
		qcall lib/socket/htons,(port:port)
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
		cpy.i 0,[buffer+4]
		cpy.l [buffer],lmicros
		qcall lib/close,(sock : i_tmp)
		cpy.i 0,i0
	ret
	defend

data
	version:
		dc.b '$VER: app/fade/client/sntp 1.1 (15.11.2001)',0
toolend

tool 'lib/unsat/readline',VP

	ent p0 i0 i1: p0
	; inputs  p0 = filename
	;         i0 = line number
	;	  i1 = max size
	; outputs p0 = line
	defbegin 0
		defp fname,buffer,data
		defi line,size,fhand,itmp,tchar,cline,cchar

		qcall lib/open,(fname,O_RDONLY : fhand)
		if fhand=-1
			cpy.p 0,p0
			ret
		endif

		qcall lib/malloc,(size : buffer)
		cpy.i 0,cchar
		cpy.i 0,cline
		while cline<line
			qcall lib/read,(fhand,buffer+cchar,1 : itmp)
			
			cpy.b [buffer+cchar],tchar
			;cpy.b tchar,[data+cchar]
			;printf '%s\n',buffer
			inc cchar
			if tchar=10
				inc cline
				cpy.b 0,[buffer+cchar-1]
				cpy.i 0,cchar
			endif
			if itmp<=0
				qcall lib/close,(fhand : i~)
				qcall lib/free,(buffer :-)
				cpy.p 0,p0
				ret
			endif
		endwhile

		qcall lib/close,(fhand : i~)
	
		;printf '%s\n',buffer

		;qcall lib/free,(buffer :-)
		
		cpy.p buffer,p0
		ret
	defend 0
	
data
	version:
		dc.b 0,'$VER: lib/unsat/readline 1.0 (24.10.2001)',0
toolend

tool 'lib/unsat/splstr',VP

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

tool 'lib/unsat/joinstr',VP

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

.end

