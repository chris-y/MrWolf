.include 'tao'
.include 'socket'

tool 'tmp/testport',VP,TF_MAIN,8192,0
	ent - : -

	qcall lib/socket/getservbyname,(service.p,protocol.p:p0)
	if i0==0
		printf "Error looking up port\n\n"
		qcall lib/exit,(-:-)
	endif
	cpy.i [p0+serv_ent_port],i0
	printf "Port: %d\n\n",i0

	qcall lib/exit,(-:-)

data
	service:
		dc.b 'telnet',0
	protocol:
		dc.b 'tcp',0
toolend

.end