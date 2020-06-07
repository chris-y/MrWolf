.include 'tao'
.include 'time'
.include 'socket'

tool 'app/fade/nettime',VP,TF_MAIN,8192,0
	ent - : -
	defbegin 0
		defp server,tms,tmp,secs
		; defl secs

	        qcall lib/argcargv,(-:p0 i0)

		printf "fade... by chris young\n"
		printf "(c) 2001 unsatisfactory software\n\n"

		;cpy.p vers,tmp
;		printf "%s\n",tmp

		if i0==1
			printf "Please specify a server on the command line\n"
			; we need to break out of the program and clean up here
		        qcall lib/exit,(0:-)
		endif

		cpy.p [p0+4],server ; note that server and p0 are the same thing

		printf "Server: %s\n",server

		allocstruct TM_SIZE,tms

		; *** SHOW CURRENT LOCAL TIME ***
		qcall lib/time,(tmp : l0)
		qcall lib/malloc,(8 : tmp)
		cpy.l l0,[tmp]
		qcall lib/localtime,(tmp : tms)
		qcall lib/free,(tmp : -)
		qcall lib/asctime,(tms : tmp)
		printf "Local time: %s\n",tmp
		; *** END CURRENT LOCAL TIME BLOCK ***

		qcall app/fade/timetcp,(server : secs),VIRTUAL
		qcall lib/localtime,(secs : tms)
		qcall lib/asctime,(tms : tmp)
		printf "Server time: %s\n",tmp
	defend
        qcall lib/exit,(0:-)
	ret

data
;	.include 'fade_ver.inc'
vers:
	dc.b 0,'$VER: fade 1.0 (09.09.2001)',0
toolend

tool 'app/fade/timetcp',VP
	ent p0 : p0
	; inputs  p0 = server name
	; outputs p0 = seconds
	defbegin 0
		defp server,host,buffer,saddr,serve,tmp,inetp,resi
		defi i_tmp,ihost,sock
		; dodgy tcp/ip code...
		
;		qcall sys/kn/dev/lookup,(sockdev.p : inetp,resi)
;		
;		printf "%d %s\n",inetp,[resi]
;		ncall inetp,open,(inetp,resi,0,0:resi)
;
;printf "open"
;		ncall inetp,_init,(inetp,0,0:i_tmp)
		

;			printf "init\n"


;		ncall inetp,_deinit,(inetp:i_tmp)
		
;		if i_tmp!=0
;			printf "deinit error\n"
;		endif
;		ncall inetp,close,(inetp,0:i_tmp)
		
;		ret
		
		qcall lib/malloc,(50 : buffer)

;		qcall lib/malloc,(servent_size : serve)
;		qcall lib/socket/getservbyname,(serv.p,proto.p : serve)

;		if serve==0
;			printf "error\n"
;		endif

;		cpy.i [serve+s_port],i_tmp
;		cpy.i 13,i_tmp
		printf "%s\n",server
		
		qcall lib/malloc,(hostent_size : host)
;		qcall lib/socket/inet_addr,(sip.p : ihost) ;[host+h_addr_list])
;qcall lib/socket/inet_ntoa,(ihost : serve)
;printf "%s\n",serve
;qcall lib/socket/htonl,(ihost : ihost)

;qcall lib/socket/inet_ntoa,(ihost : serve)
;printf "%s\n",serve

;		qcall lib/socket/gethostbyname,(servr.p : host)
		qcall lib/socket/gethostbyname,(server : host)
		if host==0
			printf "gethostbyname ERROR!\n"
			ret
		endif
;		cpy.p [host+h_addr_list],tmp
;		qcall lib/socket/inet_addr,(tmp : ihost)
	
		qcall lib/socket/socket,(AF_INET,SOCK_STREAM,IPPROTO_TCP : sock)
;		qcall lib/socket/inet_ntoa,(tmp:tmp) ;ihost
		
		printf "addr: %d\n",[host+h_addr_list]
		
		qcall lib/malloc,(SOCKADDR_IN_SIZE : saddr)

;		qcall lib/socket/inet_ntoa,([host] : host)
		cpy.s 21,[saddr+sin_port]
		cpy.s AF_INET,[saddr+sin_family]
		cpy.i [host+h_addr_list],[saddr+sin_addr]
;		cpy.i ihost,[saddr+sin_addr]

;printf "%d\n",[saddr+sin_port]

		qcall lib/socket/connect,(sock,saddr,sockaddr_in_size : i_tmp)

;qcall lib/socket/recvfrom,(sock,buffer,50,0,saddr,sockaddr_in_size : i_tmp)

		printf "connect %d (-1 = error) ; %d\n",i_tmp,[gp+proc_errno]

		
;		qcall lib/socket/send,(sock,buffer,0,0 : i_tmp)
		qcall lib/socket/recv,(sock,buffer,50,0 : i1)
		printf "%s %d\n",[buffer],i1
	; old stuff
	;qcall lib/time,(p2 : l0)
	qcall lib/malloc,(8 : p1)
	cpy.l [buffer],[p1] ; dummy line for testing porpoises
	cpy.p p1,p0
	ret
	defend

data
	serv:
		dc.b 'time',0
	proto:
		dc.b 'tcp',0
;	servr:
;		dc.b 'ntp2a.mcc.ac.uk',0
;	sip:
;		dc.b '194.151.255.141',0
;	sockdev:
;		dc.b 'device/inet'
toolend
;130.88.202.49
.end

