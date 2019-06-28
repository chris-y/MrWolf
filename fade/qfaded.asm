.include 'tao'
.include 'time'
.include 'socket'
.include 'chris/tcall/tcall.inc'
.include 'chris/fade.inc'

tool 'app/fade/qfaded',VP,TF_MAIN,8192,0
	ent - : -
	defbegin
		defp server,tms,tmp,host,dip,proto,pport,proton,daemon,saddr
		defi itmp,err,ipaddy,port,curserv,ipproto,i_tmp,sockt,sock,fhand

		cpy.p tcp,proton
		
			        qcall lib/argcargv,(-:p0 i0)
printf "qfaded - fade daemon quit utility\n"
printf "WARNING: Other faded sessions using the same service may also quit!\n\n"

			switch
				whencase i0=4
				otherwise
					;if i0>3 ; 1=fname only
					printf "Template: app/fade/qfaded server service protocol\n"
					printf "Where service is 'daytime' 'time' or 'sntp'\n"
                                        printf "      protocol is 'tcp' or 'udp'\n\n"
					; we need to break out of the program and clean up here
		        	qcall lib/exit,(0:-)
				break
				case i0=4
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
			;cpy.i 13,port
			qcall lib/socket/getservbyname,(daemon,proton:tmp)
			if tmp==0
				printf "ERROR: Unable to determine correct port.\nPlease specify port number on the command line.\n\n"
				qcall lib/exit,(-:-)
			endif
	;		printf "%s\n",[tmp+serv_ent_port]
			cpy.i [tmp+serv_ent_port],port
			qcall lib/socket/ntohs,(port:port)
;			printf "resolved port: %d\n",port
			; do a reverse lookup on service name here
		endif

qcall lib/open,(qfile.p,O_WRONLY|O_CREAT,S_IRUSR|S_IWUSR:fhand)
qcall lib/write,(fhand,daemon,10:itmp)
qcall lib/close,(fhand:i~)

		printf "Connecting to %s service on %s port %d...\n\n",daemon,proton,port

		qcall lib/socket/socket,(AF_INET,sockt,ipproto : sock)
		qcall lib/malloc,(SOCKADDR_IN_SIZE : saddr)
		qcall lib/socket/htons,(port:port)
		cpy.s port,[saddr+sin_port]
		cpy.s AF_INET,[saddr+sin_family]
		cpy.i ipaddy,[saddr+sin_addr]
		
		qcall lib/socket/connect,(sock,saddr,sockaddr_in_size : i_tmp)

		;printf "connect %d (-1 = error) ; %d\n",i_tmp,[gp+proc_errno]

		if i_tmp=-1
			printf "Unable to connect.\n"
			qcall lib/exit,(-:-)
		endif

		tcall proto,(sock ipproto : l~,err),itmp ;,VIRTUAL
		qcall lib/close,(sock : i_tmp)

		;app/fade/client/daytime
		if itmp==0
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

;			qcall lib/exit,(-:-)
		endif

qcall lib/sleep,(1:i~)

qcall lib/open,(qfile.p,O_WRONLY|O_CREAT,S_IRUSR|S_IWUSR:fhand)
qcall lib/write,(fhand,blank.p,4:itmp)
qcall lib/close,(fhand:i~)

qcall lib/exit,(-:-)
	ret
	defend

data
	version:
		dc.b 0,'$VER: app/fade/qfaded 2.1 (22.06.2002)',0
	qfile:
		dc.b 'app/fade/data/quit',0
	quit:
		dc.b 'quit',0
	blank:
		dc.b '    ',0
	tcp:
		dc.b 'tcp',0
	tool:
		dc.b 'app/fade/client/',0

toolend

.end

