	record ntp
		byte livnmode ; LI[2],VN[3],MODE[3]
		byte stratum
		byte poll
		byte precision
		int  rootdelay
		int  rootdisp
		int  refid
		long reftime    ; double float
		long origtime   ; double float
		long recvtime   ; double float
		long sendtime   ; double float
;		int  keyid      ;optional
;		long msgdigest  ;optional (128-bit)  
;		long msgdigestb ;long is only 64-bit
	endrecord
	ntp_size = sizeof(ntp)	; size of this structure	
; divide by 2^32

dbitstart
	dbit li,bli
	dbit li2,bli2
	dbit vn,bvn
	dbit vn2,bvn2
	dbit vn3,bvn3
	dbit mode,bmode
	dbit mode2,bmode2
	dbit mode3,bmode3
