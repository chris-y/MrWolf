.include 'tao'
.include 'time'
.include 'socket'
.include 'chris/tcall/tcall.inc'
.include 'chris/fade.inc'

tool 'app/fade/faded',VP,TF_MAIN,8192,0
	ent - : -
	defbegin
		defp buffer,host
		defi len,ipaddy,port
		
		cpy.i 2000,port
		qcall lib/malloc,(100 : buffer)
		qcall lib/socket/gethostname,(buffer,100:i~)
		qcall lib/socket/gethostbyname,(buffer : host)
		if host==0
			printf "\n\nError with DNS lookup\n"
			qcall lib/exit,(-:-)
		endif
		cpy.i [[[host+h_addr_list]]],ipaddy

		qcall app/fade/server/daytime,(ipaddy,port:-)

		qcall lib/exit,(-:-)
	defend
toolend

tool 'app/fade/server/daytime',VP
	ent i0 i1 : -
	;*inputs
	;i0 = port
	;*outputs

	defbegin 0
		defi ipaddy,port,sock,connsock,itmp
		defp tmp,saddr,bindsaddr,tms

		if port=0
			cpy.i 13,port
		endif
				
		qcall lib/malloc,(SOCKADDR_IN_SIZE : saddr)

		qcall lib/socket/socket,(AF_INET,SOCK_STREAM,IPPROTO_TCP : sock)
;qcall lib/socket/setsockopt,(sock,SOL_SOCKET,SO_NOBLOCK,zero.p,1 : itmp)
;printf '%d\n',itmp
;qcall lib/socket/setsockopt,(sock,SOL_SOCKET,SO_REUSEADDR,one.p,1 : itmp)
;printf '%d\n',itmp
		qcall lib/malloc,(SOCKADDR_IN_SIZE : bindsaddr)
		if port=0
			cpy.i 13,port
		endif
		qcall lib/socket/htons,(port:port)
		cpy.s port,[bindsaddr+sin_port]
		cpy.s AF_INET,[bindsaddr+sin_family]
		cpy.i ipaddy,[bindsaddr+sin_addr]
;		qcall lib/socket/htonl,(INADDR_ANY : ipaddy)
;		cpy.i ipaddy,[bindsaddr+sin_addr]
		qcall lib/socket/bind,(sock,bindsaddr,sockaddr_in_size:itmp)
		printf '%d\n',itmp
		qcall lib/socket/listen,(sock,5:itmp)
		if itmp==0		
			printf 'Waiting for connection...\n'
		else
			printf 'Error listening on port.\n'
			qcall lib/exit,(-:-)
		endif
		qcall lib/socket/accept,(sock,saddr,sockaddr_in_size:connsock)
		qcall lib/socket/inet_ntoa,([saddr+sin_addr]:tmp)
		printf 'Connection accepted from %s\n',tmp

;printf '%d\n',connsock

;		qcall lib/socket/connect,(connsock,saddr,sockaddr_in_size : itmp)

		; *** SHOW CURRENT LOCAL TIME ***
		qcall lib/malloc,(25:tmp)
		qcall lib/time,(tmp : l0)
		qcall lib/localtime,(tmp : tms)
		qcall lib/asctime,(tms : tmp)
		printf "Sending time...\n"
		; *** END CURRENT LOCAL TIME BLOCK ***

		qcall lib/strlen,(tmp:itmp)
				
		qcall lib/socket/send,(connsock,tmp,itmp+1,0 : itmp)
		qcall lib/close,(connsock : itmp)

	ret
	defend

data
	format:
		dc.b '%c',0
	version:
		dc.b '$VER: app/fade/server/daytime 1.0 (04.05.2002)',0
	zero:
		dc.b '0',0
	one:
		dc.b '1',0
;	form2:
;		dc.b '%a %b %d %H:%M:%S %Y',0
toolend

.end

