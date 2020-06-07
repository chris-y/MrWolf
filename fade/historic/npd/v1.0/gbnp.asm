.include 'tao'
.include 'time'

; some info from http://members.tripod.com/mghorizon//html/usefulstuff/regnos.html

tool 'app/stdio/npd',VP,TF_MAIN,8192,0
	ent - : -
	defbegin 0
		defp tmp,plate,tms
		defi itmp,char1,char2,char3,char4,char5,char6,char7
		defi insane,regyear,tmpc,plen,yadj,ageid,ptwo

	        qcall lib/argcargv,(-:p0 i0)
	        
		printf "Numberplate Decoder by Chris Young\n"
		printf "(c) 2001 Unsatisfactory Software\n\n"

		if i0==1
			printf "Usage: npd REGMARK\n\n"
			; we need to break out of the program and clean up here
		        qcall lib/exit,(0:-)
		endif

		cpy.p [p0+4],plate
		qcall lib/strupc,(plate : plate)

		;printf "Registration: %s\n",plate

		; split the registration up into individual characters

		qcall lib/strlen,(plate:plen)

if plen>0
		cpy.b [plate+0],char1
else
	cpy.b ' ',char1
endif
if plen>1
		cpy.b [plate+1],char2
else
	cpy.b ' ',char2
endif
if plen>2
		cpy.b [plate+2],char3
else
	cpy.b ' ',char3
endif
if plen>3
		cpy.b [plate+3],char4
else
	cpy.b ' ',char4
endif
if plen>4
		cpy.b [plate+4],char5
else
	cpy.b ' ',char5
endif
if plen>5
		cpy.b [plate+5],char6
else
	cpy.b ' ',char6
endif
if plen>6
		cpy.b [plate+6],char7
else
	cpy.b ' ',char7
endif

		cpy.i 0,insane
		; sanity checking
		qcall lib/isalpha,(char1 : itmp)
		if itmp=0
			cpy.i 1,insane
		endif
		qcall lib/isalpha,(char2 : itmp)
		if itmp=0
			if insane=0
				cpy.i 2,insane ; old style
			else
				cpy.i 1,insane
			endif
		endif
		qcall lib/isdigit,(char3 : itmp)
		if itmp=0
			if insane=2
				; don't panic - old style with less
				; than three digits
				cpy.b char5,char7
				cpy.b char4,char6
				cpy.b char3,char5
				cpy.b char2,char4
				cpy.b ' ',char3
				cpy.b ' ',char2
			else
				cpy.i 3,insane ; old old style?
			endif
		endif
		qcall lib/isdigit,(char4 : itmp)
		if itmp=0
			if insane=2
				; don't panic - old style with less
				; than three digits
				cpy.b char6,char7
				cpy.b char5,char6
				cpy.b char4,char5
				cpy.b char3,char4
				cpy.b char2,char3
				cpy.b ' ',char2
			else
				cpy.i 1,insane
			endif
		endif
		qcall lib/isalpha,(char5 : itmp)
		if itmp=0
			if insane<>3
				cpy.i 1,insane
			endif
		elseif insane=3
			;cpy.b char5,char7
			cpy.b char5,char6
			cpy.b ' ',char5
		endif
		qcall lib/isalpha,(char6 : itmp)
		if itmp=0
			cpy.i 3,insane ; really old style
		elseif insane=3
			; whoa there!  we might have a future-style plate
			qcall lib/isalpha,(char7 : itmp)
			if itmp<>0
				cpy.i -1,insane
			else
				cpy.b char6,char7
				cpy.b ' ',char6
			endif
		endif
		qcall lib/isalpha,(char7 : itmp)
		if itmp=0
			cpy.i 1,insane
		endif

printf "Registration: %c%c%c%c%c%c%c\n",char1,char2,char3,char4,char5,char6,char7

if insane=1
; have a quick scout through the code - if there is only one
; letter then it is an early number plate and we can at least
; work out the place of registration
; (assuming it hasn't been munged by the shuffling the earlier
; code does)

cpy.i 0,insane
cpy.i 0,tmpc
cpy.i 0,char2
	while tmpc<plen
		cpy.b [plate+tmpc],char1
		qcall lib/isalpha,(char1 : itmp)
		if itmp<>0
			if char2=1
				cpy.i 9,insane
			endif
			cpy.i insane+1,insane
			if insane=1
				cpy.b char1,char6
				cpy.b ' ',char7
			elseif insane=2
				cpy.b char1,char7
			elseif insane=3
				cpy.b char6,char5
				cpy.b char7,char6
				cpy.b char1,char7
			endif
		else
			if insane>0
				cpy.i 1,char2
			endif
		endif
		cpy.i tmpc+1,tmpc

	endwhile
	if insane>3
		printf "Invalid registration code\n\n"
        	qcall lib/exit,(0:-)
        else
        	cpy.i 0,regyear
        	cpy.i 4,insane
        endif
elseif insane=2
	; don't be too hasty - we might be dealing with an old-style code
	; nb numeric part of old codes has to be padded out with 0s

	printf "Reg Period: "

   switch
	whencase char1='A'
	whencase char1='B'
	whencase char1='C'
	whencase char1='D'
	whencase char1='E'
	whencase char1='F'
	whencase char1='G'
	whencase char1='H'
	whencase char1='J'
	whencase char1='K'
	whencase char1='L'
	whencase char1='M'
	whencase char1='N'
	whencase char1='P'
	whencase char1='R'
	whencase char1='S'
	whencase char1='T'
	whencase char1='V'
	whencase char1='W'
	whencase char1='X'
	whencase char1='Y'
	whencase char1='Q'

	otherwise
		printf "%s\n",invalid
	break
	case char1='A'
		cpy.i 1983,regyear
		printf "August %d ",regyear
	break
	case char1='B'
		cpy.i 1984,regyear
		printf "August %d ",regyear
	break
	case char1='C'
		cpy.i 1985,regyear
		printf "August %d ",regyear
	break
	case char1='D'
		cpy.i 1986,regyear
		printf "August %d ",regyear
	break
	case char1='E'
		cpy.i 1987,regyear
		printf "August %d ",regyear
	break
	case char1='F'
		cpy.i 1988,regyear
		printf "August %d ",regyear
	break
	case char1='G'
		cpy.i 1989,regyear
		printf "August %d ",regyear
	break
	case char1='H'
		cpy.i 1990,regyear
		printf "August %d ",regyear
	break
	case char1='J'
		cpy.i 1991,regyear
		printf "August %d ",regyear
	break
	case char1='K'
		cpy.i 1992,regyear
		printf "August %d ",regyear
	break
	case char1='L'
		cpy.i 1993,regyear
		printf "August %d ",regyear
	break
	case char1='M'
		cpy.i 1994,regyear
		printf "August %d ",regyear
	break
	case char1='N'
		cpy.i 1995,regyear
		printf "August %d ",regyear
	break
	case char1='P'
		cpy.i 1996,regyear
		printf "August %d ",regyear
	break
	case char1='R'
		cpy.i 1997,regyear
		printf "August %d ",regyear
	break
	case char1='S'
		cpy.i 1998,regyear
		printf "August %d ",regyear
	break
	case char1='T'
		cpy.i 1999,regyear
		printf "March %d ",regyear
	break
	case char1='V'
		cpy.i 1999,regyear
		printf "September %d ",regyear
	break
	case char1='W'
		cpy.i 2000,regyear
		printf "March %d ",regyear
	break
	case char1='X'
		cpy.i 2000,regyear
		printf "September %d ",regyear
	break
	case char1='Y'
		cpy.i 2001,regyear
		printf "March %d ",regyear
	break
	case char1='Q'
		cpy.i 0,regyear
		printf "Dubious Origin\n"
	break
   endswitch

elseif insane=3
; don't be too hasty - we might be dealing with an really old-style code
; nb numeric part of old codes has to be padded out with 0s
; really old style codes need their own sanity checking

	printf "Reg Period: "
	
   switch
	whencase char7='A'
	whencase char7='B'
	whencase char7='C'
	whencase char7='D'
	whencase char7='E'
	whencase char7='F'
	whencase char7='G'
	whencase char7='H'
	whencase char7='J'
	whencase char7='K'
	whencase char7='L'
	whencase char7='M'
	whencase char7='N'
	whencase char7='P'
	whencase char7='R'
	whencase char7='S'
	whencase char7='T'
	whencase char7='V'
	whencase char7='W'
	whencase char7='X'
	whencase char7='Y'

	otherwise
		printf "%s\n",invalid
	break
	case char7='A'
		cpy.i 1963,regyear
		printf "January %d ",regyear
	break
	case char7='B'
		cpy.i 1964,regyear
		printf "January %d ",regyear
	break
	case char7='C'
		cpy.i 1965,regyear
		printf "January %d ",regyear
	break
	case char7='D'
		cpy.i 1966,regyear
		printf "January %d ",regyear
	break
	case char7='E'
		cpy.i 1967,regyear
		printf "January %d ",regyear
	break
	case char7='F'
		cpy.i 1967,regyear
		printf "August %d ",regyear
	break
	case char7='G'
		cpy.i 1968,regyear
		printf "August %d ",regyear
	break
	case char7='H'
		cpy.i 1969,regyear
		printf "August %d ",regyear
	break
	case char7='J'
		cpy.i 1970,regyear
		printf "August %d ",regyear
	break
	case char7='K'
		cpy.i 1971,regyear
		printf "August %d ",regyear
	break
	case char7='L'
		cpy.i 1972,regyear
		printf "August %d ",regyear
	break
	case char7='M'
		cpy.i 1973,regyear
		printf "August %d ",regyear
	break
	case char7='N'
		cpy.i 1974,regyear
		printf "August %d ",regyear
	break
	case char7='P'
		cpy.i 1975,regyear
		printf "August %d ",regyear
	break
	case char7='R'
		cpy.i 1976,regyear
		printf "August %d ",regyear
	break
	case char7='S'
		cpy.i 1977,regyear
		printf "August %d ",regyear
	break
	case char7='T'
		cpy.i 1978,regyear
		printf "August %d ",regyear
	break
	case char7='V'
		cpy.i 1979,regyear
		printf "August %d ",regyear
	break
	case char7='W'
		cpy.i 1980,regyear
		printf "August %d ",regyear
	break
	case char7='X'
		cpy.i 1981,regyear
		printf "August %d ",regyear
	break
	case char7='Y'
		cpy.i 1982,regyear
		printf "August %d ",regyear
	break
   endswitch

endif ; end elseif insane=3

if insane=-1
; possible future style
; move the lmt so we are looking at the right thing
	cpy.b char6,char1
	cpy.b char7,char2
; make adjustments for working out the age
	cpy.i 3,ageid
	cpy.i 2050,yadj
	cpy.i 2100,ptwo
else
	cpy.i 2,ageid
	cpy.i 2000,yadj
	cpy.i 2050,ptwo
endif

if insane<=0

printf "Local Memory Tag: "

switch
	whencase char1='A'
	whencase char1='B'
	whencase char1='C'
	whencase char1='D'
	whencase char1='E'
	whencase char1='F'
	whencase char1='G'
	whencase char1='H'
	whencase char1='K'
	whencase char1='L'
	whencase char1='M'
	whencase char1='N'
	whencase char1='O'
	whencase char1='P'
	whencase char1='R'
	whencase char1='S'
	whencase char1='V'
	whencase char1='W'
	whencase char1='Y'
	otherwise
		printf "%s\n",invalid
	break

	case char1='A'
		switch
			whencase char2>='A' and char2<='N'
			whencase char2>='O' and char2<='U'
			whencase char2>='V' and char2<='Y'
			otherwise
				printf "%s\n",invalid
			break
			case char2>='A' and char2<='N'
				printf 'Peterborough'
			break
			case char2>='O' and char2<='U'
				printf 'Norwich'
			break
			case char2>='V' and char2<='Y'
				printf 'Ipswich'
			break
		endswitch
	break
	case char1='B'
		switch
			whencase char2>='A' and char2<='Y'
			otherwise
				printf "%s\n",invalid
			break
			case char2>='A' and char2<='Y'
				printf 'Birmingham'
			break
		endswitch
	break
	case char1='C'
		switch
			whencase char2>='A' and char2<='O'
			whencase char2>='P' and char2<='V'
			whencase char2>='W' and char2<='Y'
			otherwise
				printf "%s\n",invalid
			break
			case char2>='A' and char2<='O'
				printf 'Cardiff'
			break
			case char2>='P' and char2<='V'
				printf 'Swansea'
			break
			case char2>='W' and char2<='Y'
				printf 'Bangor'
			break
		endswitch
	break
	case char1='D'
		switch
			whencase char2>='A' and char2<='K'
			whencase char2>='L' and char2<='Y'
			otherwise
				printf "%s\n",invalid
			break
			case char2>='A' and char2<='K'
				printf 'Chester'
			break
			case char2>='L' and char2<='Y'
				printf 'Shrewsbury'
			break
		endswitch
	break
	case char1='E'
		switch
			whencase char2>='A' and char2<='Y'
			otherwise
				printf "%s\n",invalid
			break
			case char2>='A' and char2<='Y'
				printf 'Chelmsford'
			break
		endswitch
	break
	case char1='F'
		switch
			whencase char2>='A' and char2<='P'
			whencase char2>='R' and char2<='Y'
			otherwise
				printf "%s\n",invalid
			break
			case char2>='A' and char2<='P'
				printf 'Nottingham'
			break
			case char2>='R' and char2<='Y'
				printf 'Lincoln'
			break
		endswitch
	break
	case char1='G'
		switch
			whencase char2>='A' and char2<='O'
			whencase char2>='P' and char2<='Y'
			otherwise
				printf "%s\n",invalid
			break
			case char2>='A' and char2<='O'
				printf 'Maidstone'
			break
			case char2>='P' and char2<='Y'
				printf 'Brighton'
			break
		endswitch
	break
	case char1='H'
		switch
			whencase char2>='A' and char2<='J'
			whencase char2>='K' and char2<='Y'
			otherwise
				printf "%s\n",invalid
			break
			case char2>='A' and char2<='J'
				printf 'Bournemouth'
			break
			case char2>='K' and char2<='Y'
				if char2='W'
					printf 'Portsmouth (Isle of Wight)'
				else
					printf 'Portsmouth'
				endif
			break
		endswitch
	break
	case char1='K'
		switch
			whencase char2>='A' and char2<='L'
			whencase char2>='M' and char2<='Y'
			otherwise
				printf "%s\n",invalid
			break
			case char2>='A' and char2<='L'
				printf 'Luton'
			break
			case char2>='M' and char2<='Y'
				printf 'Northampton'
			break
		endswitch
	break
	case char1='L'
		switch
			whencase char2>='A' and char2<='J'
			whencase char2>='K' and char2<='T'
			whencase char2>='U' and char2<='Y'
			otherwise
				printf "%s\n",invalid
			break
			case char2>='A' and char2<='J'
				printf 'Wimbledon'
			break
			case char2>='K' and char2<='T'
				printf 'Stanmore'
			break
			case char2>='U' and char2<='Y'
				printf 'Sidcup'
			break
		endswitch
	break
	case char1='M'
		switch
			whencase char2>='A' and char2<='Y'
			otherwise
				printf "%s\n",invalid
			break
			case char2>='A' and char2<='Y'
				printf 'Manchester'
			break
		endswitch
	break
	case char1='N'
		switch
			whencase char2>='A' and char2<='O'
			whencase char2>='P' and char2<='Y'
			otherwise
				printf "%s\n",invalid
			break
			case char2>='A' and char2<='O'
				printf 'Newcastle'
			break
			case char2>='P' and char2<='Y'
				printf 'Stockton'
			break
		endswitch
	break
	case char1='O'
		switch
			whencase char2>='A' and char2<='Y'
			otherwise
				printf "%s\n",invalid
			break
			case char2>='A' and char2<='Y'
				printf 'Oxford'
			break
		endswitch
	break
	case char1='P'
		switch
			whencase char2>='A' and char2<='T'
			whencase char2>='U' and char2<='Y'
			otherwise
				printf "%s\n",invalid
			break
			case char2>='A' and char2<='T'
				printf 'Preston'
			break
			case char2>='U' and char2<='Y'
				printf 'Carlisle'
			break
		endswitch
	break
	case char1='R'
		switch
			whencase char2>='A' and char2<='Y'
			otherwise
				printf "%s\n",invalid
			break
			case char2>='A' and char2<='Y'
				printf 'Reading'
			break
		endswitch
	break
	case char1='S'
		switch
			whencase char2>='A' and char2<='J'
			whencase char2>='K' and char2<='O'
			whencase char2>='P' and char2<='T'
			whencase char2>='U' and char2<='W'
			whencase char2>='X' and char2<='Y'
			otherwise
				printf "%s\n",invalid
			break
			case char2>='A' and char2<='J'
				printf 'Glasgow'
			break
			case char2>='K' and char2<='O'
				printf 'Edinburgh'
			break
			case char2>='P' and char2<='T'
				printf 'Dundee'
			break
			case char2>='U' and char2<='W'
				printf 'Aberdeen'
			break
			case char2>='X' and char2<='Y'
				printf 'Inverness'
			break
		endswitch
	break
	case char1='V'
		switch
			whencase char2>='A' and char2<='Y'
			otherwise
				printf "%s\n",invalid
			break
			case char2>='A' and char2<='Y'
				printf 'Worcester'
			break
		endswitch
	break
	case char1='W'
		switch
			whencase char2>='A' and char2<='J'
			whencase char2>='K' and char2<='L'
			whencase char2>='M' and char2<='Y'
			otherwise
				printf "%s\n",invalid
			break
			case char2>='A' and char2<='J'
				printf 'Exeter'
			break
			case char2>='K' and char2<='L'
				printf 'Truro'
			break
			case char2>='M' and char2<='Y'
				printf 'Bristol'
			break
		endswitch
	break
	case char1='Y'
		switch
			whencase char2>='A' and char2<='K'
			whencase char2>='L' and char2<='U'
			whencase char2>='V' and char2<='Y'
			otherwise
				printf "%s\n",invalid
			break
			case char2>='A' and char2<='K'
				printf 'Leeds'
			break
			case char2>='L' and char2<='U'
				printf 'Sheffield'
			break
			case char2>='V' and char2<='Y'
				printf 'Beverley'
			break
		endswitch
	break

endswitch

	qcall lib/atoi,(plate+ageid : regyear)
	cpy.i regyear+yadj,regyear

printf "\nReg Period: "

if regyear>ptwo
	cpy regyear-50,regyear
	printf "September %d ",regyear
else
	printf "March %d ",regyear
endif

endif ; end if insane=0 (ie. newstyle)

		allocstruct TM_SIZE,tms

		; *** SHOW CURRENT LOCAL TIME ***
		qcall lib/time,(tmp : l0)
		qcall lib/malloc,(8 : tmp)
		cpy.l l0,[tmp]
		qcall lib/localtime,(tmp : tms)
		qcall lib/free,(tmp : -)
		cpy [tms+TM_YEAR]+1900,i0 ; current year so we can work out the approx age
		;printf "Year: %d\n",i0
		; *** END CURRENT LOCAL TIME BLOCK ***

if regyear > 1950 and regyear <= i0
cpy i0-regyear,itmp
	printf "(~%d years old)",itmp
	if insane>0
		printf '\n'
	endif
endif

if insane>1

if insane=3
; make sure we're all looking at the same stuff
	cpy.b char2,char6
	cpy.b char3,char7
endif

printf "Reg Area: "

switch
	whencase char6='A'
	whencase char6='B'
	whencase char6='C'
	whencase char6='D'
	whencase char6='E'
	whencase char6='F'
	whencase char6='G'
	whencase char6='H'
	whencase char6='I'
	whencase char6='J'
	whencase char6='K'
	whencase char6='L'
	whencase char6='M'
	whencase char6='N'
	whencase char6='O'
	whencase char6='P'
	whencase char6='Q'
	whencase char6='R'
	whencase char6='S'
	whencase char6='T'
	whencase char6='U'
	whencase char6='V'
	whencase char6='W'
	whencase char6='X'
	whencase char6='Y'
	whencase char6='Z'
	otherwise
		printf "%s\n",invalid
	break

	case char6='A'
		switch
			whencase char7='A'
			whencase char7='B'
			whencase char7='C'
			whencase char7='D'
			whencase char7='E'
			whencase char7='F'
			whencase char7='G'
			whencase char7='H'
			whencase char7='I'
			whencase char7='J'
			whencase char7='K'
			whencase char7='L'
			whencase char7='M'
			whencase char7='N'
			whencase char7='O'
			whencase char7='P'
			whencase char7='R'
			whencase char7='S'
			whencase char7='T'
			whencase char7='U'
			whencase char7='V'
			whencase char7='W'
			whencase char7='X'
			whencase char7='Y'
			whencase char7='Z'
			whencase char7=' '
			otherwise
				printf "%s",invalid
			break
			case char7=' '
				printf 'London'
			break
			case char7='A'
				printf 'Hampshire (Southampton)'
			break
			case char7='B'
				printf 'Worcestershire'
			break
			case char7='C'
				printf 'Warwickshire'
			break
			case char7='D'
				printf 'Gloucestershire'
			break
			case char7='E'
				if regyear>=1974
					printf 'Inverness'
				else
					printf 'Bristol'
				endif
			break
			case char7='F'
				printf 'Cornwall'
			break
			case char7='G'
				if regyear>=1974
					printf 'Hull'
				else
					printf 'Ayrshire'
				endif
			break
			case char7='H'
				printf 'Norfolk'
			break
			case char7='I'
				printf 'Meath'
			break
			case char7='J'
				if regyear>=1974
					printf 'Middlesborough'
				else
					printf 'Yorkshire (N. Riding)'
				endif
			break
			case char7='K'
				if regyear>=1974
					printf 'Sheffield'
				else
					printf 'Bradford'
				endif
			break
			case char7='L'
				printf 'Nottinghamshire'
			break
			case char7='M'
				printf 'Wiltshire'
			break
			case char7='N'
				if regyear>=1974
					printf 'Reading'
				else
					printf 'West Ham'
				endif
			break
			case char7='O'
				printf 'Cumberland'
			break
			case char7='P'
				printf 'Sussex (East)'
			break
			case char7='R'
				if regyear>=1974
					printf 'Chelmsford'
				else
					printf 'Hertfordshire'
				endif
			break
			case char7='S'
				printf 'Nairn'
			break
			case char7='T'
				printf 'Kingston-upon-Hull'
			break
			case char7='U'
				printf 'Nottingham'
			break
			case char7='V'
				if regyear>=1974
					printf 'Peterborough'
				else
					printf 'Aberdeenshire'
				endif
			break
			case char7='W'
				printf 'Sallop'
			break
			case char7='X'
				printf 'Monmouthshire'
			break
			case char7='Y'
				printf 'Leicestershire'
			break
			case char7='Z'
				printf 'Belfast'
			break
		endswitch
	break
	case char6='B'
		switch
			whencase char7='A'
			whencase char7='B'
			whencase char7='C'
			whencase char7='D'
			whencase char7='E'
			whencase char7='G'
			whencase char7='H'
			whencase char7='I'
			whencase char7='J'
			whencase char7='K'
			whencase char7='L'
			whencase char7='M'
			whencase char7='N'
			whencase char7='O'
			whencase char7='P'
			whencase char7='R'
			whencase char7='S'
			whencase char7='T'
			whencase char7='U'
			whencase char7='V'
			whencase char7='W'
			whencase char7='X'
			whencase char7='Y'
			whencase char7='Z'
			whencase char7=' '
			otherwise
				printf "%s",invalid
			break
			case char7=' '
				printf 'Lancashire'
			break
			case char7='A'
				printf 'Salford'
			break
			case char7='B'
				printf 'Newcastle-upon-Tyne'
			break
			case char7='C'
				printf 'Leicester'
			break
			case char7='D'
				printf 'Northamptonshire'
			break
			case char7='E'
				printf 'Lincolnshire (Lindsey)'
			break
			case char7='G'
				printf 'Birkenhead'
			break
			case char7='H'
				if regyear>=1974
					printf 'Luton'
				else
					printf 'Buckinghamshire'
				endif
			break
			case char7='I'
				printf 'Monaghan'
			break
			case char7='J'
				printf 'Suffolk (East)'
			break
			case char7='K'
				printf 'Portsmouth'
			break
			case char7='L'
				printf 'Berkshire'
			break
			case char7='M'
				if regyear>=1974
					printf 'Luton'
				else
					printf 'Bedfordshire'
				endif
			break
			case char7='N'
				printf 'Bolton'
			break
			case char7='O'
				printf 'Cardiff'
			break
			case char7='P'
				if regyear>=1974
					printf 'Portsmouth'
				else
					printf 'Sussex (West)'
				endif
			break
			case char7='R'
				if regyear>=1974
					printf 'Newcastle'
				else
					printf 'Sunderland'
				endif
			break
			case char7='S'
				if regyear>=1981
					printf 'Inverness'
				elseif regyear>=1974
					printf 'Kirkwall'
				else
					printf 'Orkney'
				endif
			break
			case char7='T'
				if regyear>=1981
					printf 'Leeds'
				else
					printf 'Yorkshire (E.Riding)'
				endif
			break
			case char7='U'
				printf 'Oldham'
			break
			case char7='V'
				printf 'Blackburn'
			break
			case char7='W'
				printf 'Oxfordshire'
			break
			case char7='X'
				printf 'Carmarthenshire'
			break
			case char7='Y'
				if regyear>=1974
					printf 'London'
				elseif regyear>=1965
					printf 'Greater London Council'
				else
					printf 'Croydon'
				endif
			break
			case char7='Z'
				printf 'Down'
			break
		endswitch
	break
	case char6='C'
		switch
			whencase char7='A'
			whencase char7='B'
			whencase char7='C'
			whencase char7='D'
			whencase char7='E'
			whencase char7='F'
			whencase char7='G'
			whencase char7='H'
			whencase char7='I'
			whencase char7='J'
			whencase char7='K'
			whencase char7='L'
			whencase char7='M'
			whencase char7='N'
			whencase char7='O'
			whencase char7='P'
			whencase char7='R'
			whencase char7='S'
			whencase char7='T'
			whencase char7='U'
			whencase char7='V'
			whencase char7='W'
			whencase char7='X'
			whencase char7='Y'
			whencase char7='Z'
			whencase char7=' '
			otherwise
				printf "%s",invalid
			break
			case char7=' '
				printf 'Yorkshire'
			break
			case char7='A'
				if regyear>=1974
					printf 'Cheshire'
				else
					printf 'Denbighshire'
				endif
			break
			case char7='B'
				if regyear>=1981
					printf 'Manchester'
				elseif regyear>=1974
					printf 'Bolton'
				else
					printf 'Blackburn'
				endif
			break
			case char7='C'
				if regyear>=1974
					printf 'Bangor'
				else
					printf 'Caernarvonshire'
				endif
			break
			case char7='D'
				printf 'Brighton'
			break
			case char7='E'
				if regyear>=1981
					printf 'Peterborough'
				else
					printf 'Cambridgeshire'
				endif
			break
			case char7='F'
				if regyear>=1974
					printf 'Reading'
				else
					printf 'Suffolk (West)'
				endif
			break
			case char7='G'
				if regyear>=1981
					printf 'Bournemouth'
				elseif regyear>=1974
					printf 'Salisbury'
				else
					printf 'Hampshire (Southampton)'
				endif
			break
			case char7='H'
				printf 'Derby'
			break
			case char7='I'
				printf 'Laois'
			break
			case char7='J'
				if regyear>=1974
					printf 'Gloucester'
				else
					printf 'Herefordshire'
				endif
			break
			case char7='K'
				printf 'Preston'
			break
			case char7='L'
				printf 'Norwich'
			break
			case char7='M'
				if regyear>=1974
					printf 'Liverpool'
				else
					printf 'Birkenhead'
				endif
			break
			case char7='N'
				printf 'Gateshead'
			break
			case char7='O'
				if regyear>=1981
					printf 'Exeter'
				else
					printf 'Plymouth'
				endif
			break		
			case char7='P'
				if regyear>=1974
					printf 'Huddersfield'
				else
					printf 'Halifax'
				endif
			break		
				case char7='R'
				if regyear>=1974
					printf 'Portsmouth'
				else
					printf 'Southampton'
				endif
			break		
				case char7='S'
				if regyear>=1981
					printf 'Glasgow'
				else
					printf 'Ayrshire'
				endif
			break		
			case char7='T'
				printf 'Lincolnshire (Kesteven)'
			break
			case char7='U'
				if regyear>=1974
					printf 'Newcastle'
				else
					printf 'South Shields'
				endif
			break
			case char7='V'
				printf 'Cornwall'
			break
			case char7='W'
				if regyear>=1974
					printf 'Preston'
				else
					printf 'Burnley'
				endif
			break
			case char7='X'
				printf 'Huddersfield'
			break
			case char7='Y'
				printf 'Swansea'
			break
			case char7='Z'
				printf 'Belfast'
			break
		endswitch	
	break
	case char6='D'
		switch
			whencase char7='A'
			whencase char7='B'
			whencase char7='C'
			whencase char7='D'
			whencase char7='E'
			whencase char7='F'
			whencase char7='G'
			whencase char7='H'
			whencase char7='I'
			whencase char7='J'
			whencase char7='K'
			whencase char7='L'
			whencase char7='M'
			whencase char7='N'
			whencase char7='O'
			whencase char7='P'
			whencase char7='R'
			whencase char7='S'
			whencase char7='T'
			whencase char7='U'
			whencase char7='V'
			whencase char7='W'
			whencase char7='X'
			whencase char7='Y'
			whencase char7='Z'
			whencase char7=' '
			otherwise
				printf "%s",invalid
			break
			case char7=' '
				printf 'Kent'
			break
			case char7='A'
				if regyear>=1974
					printf 'Birmingham'
				else
					printf 'Wolverhampton'
				endif
			break
			case char7='B'
				if regyear>=1974
					printf 'Manchester'
				else
					printf 'Stockport'
				endif
			break
			case char7='C'
				if regyear>=1981
					printf 'Middlesborough'
				elseif regyear>=1974
					printf 'Teeside'
				else
					printf 'Middlesborough'
				endif
			break			
			case char7='D'
				printf 'Gloucestershire'
			break			
			case char7='E'
				if regyear>=1974
					printf 'Haverfordwest'
				else
					printf 'Pembrokeshire'
				endif
			break			
			case char7='F'
				printf 'Gloucestershire'
			break		
			case char7='G'
				printf 'Gloucestershire'
			break			
			case char7='H'
				if regyear>=1974
					printf 'Dudley'
				else
					printf 'Walsall'
				endif
			break					
			case char7='I'
				printf 'Roscommon'
			break		
			case char7='J'
				if regyear>=1981
					printf 'Liverpool'
				elseif regyear>=1974
					printf 'Warrington'
				else
					printf 'St Helens'
				endif
			break					
			case char7='K'
				if regyear>=1981
					printf 'Manchester'
				elseif regyear>=1974
					printf 'Bolton'
				else
					printf 'Rochdale'
				endif
			break
			case char7='L'
				if regyear>=1981
					printf 'Portsmouth'
				elseif regyear>=1974
					printf 'Newport IoW'
				else
					printf 'Isle of Wight (W.R.)'
				endif
			break
			case char7='M'
				if regyear>=1974
					printf 'Chester'
				else
					printf 'Flintshire'
				endif
			break		
			case char7='N'
				if regyear>=1981
					printf 'Leeds'
				else
					printf 'York'
				endif
			break		
			case char7='O'
				if regyear>=1981
					printf 'Lincoln'
				elseif regyear>=1974
					printf 'Boston'
				else
					printf 'Lincolnshire (Holland)'
				endif
			break
			case char7='P'
				printf 'Reading'
			break	
			case char7='R'
				if regyear>=1981
					printf 'Exeter'
				else
					printf 'Plymouth'
				endif
			break		
			case char7='S'
				if regyear>=1974
					printf 'Glasgow'
				else
					printf 'Peeblesshire'
				endif
			break		
			case char7='T'
				if regyear>=1974
					printf 'Sheffield'
				else
					printf 'Doncaster'
				endif
			break		
			case char7='U'
				printf 'Coventry'
			break	
			case char7='V'
				if regyear>=1974
					printf 'Exeter'
				else
					printf 'Devonshire'
				endif
			break		
			case char7='W'
				if regyear>=1974
					printf 'Cardiff'
				else
					printf 'Newport (Mon.)'
				endif
			break		
			case char7='X'
				printf 'Ipswich'
			break	
			case char7='Y'
				if regyear>=1981
					printf 'Brighton'
				else
					printf 'Hastings'
				endif
			break	
			case char7='Z'
				printf 'Antrim'
			break	
		endswitch
	break
	case char6='E'
		switch
			whencase char7='A'
			whencase char7='B'
			whencase char7='C'
			whencase char7='D'
			whencase char7='E'
			whencase char7='F'
			whencase char7='G'
			whencase char7='H'
			whencase char7='I'
			whencase char7='J'
			whencase char7='K'
			whencase char7='L'
			whencase char7='M'
			whencase char7='N'
			whencase char7='O'
			whencase char7='P'
			whencase char7='R'
			whencase char7='S'
			whencase char7='T'
			whencase char7='U'
			whencase char7='V'
			whencase char7='W'
			whencase char7='X'
			whencase char7='Y'
			whencase char7='Z'
			whencase char7=' '
			otherwise
				printf "%s",invalid
			break
			case char7=' '
				printf 'Staffordshire'
			break
			case char7='A'
				if regyear>=1974
					printf 'Dudley'
				else
					printf 'West Bromwich'
				endif
			break
			case char7='B'
				if regyear>=1981
					printf 'Peterborough'
				elseif regyear>=1965
					printf 'Cambridge'
				else
					printf 'Isle of Ely'
				endif
			break
			case char7='C'
				if regyear>=1981
					printf 'Preston'
				elseif regyear>=1974
					printf 'Kendal'
				else
					printf 'Westmorland'
				endif
			break
			case char7='D'
				if regyear>=1981
					printf 'Liverpool'
				else
					printf 'Warrington'
				endif
			break
			case char7='E'
				printf 'Grimsby'
			break
			case char7='F'
				if regyear>=1974
					printf 'Middlesborough'
				else
					printf 'West Hartlepool'
				endif
			break
			case char7='G'
				printf 'Peterborough'
			break
			case char7='H'
				printf 'Stoke-on-Trent'
			break
			case char7='I'
				printf 'Sligo'
			break
			case char7='J'
				if regyear>=1983
					printf 'Haverfordwest'
				elseif regyear>=1981
					printf 'Bangor'
				elseif regyear>=1974
					printf 'Aberyswyth'
				else
					printf 'Cardiganshire'
				endif
			break
			case char7='K'
				if regyear>=1981
					printf 'Liverpool'
				elseif regyear>=1974
					printf 'Warrington'
				else
					printf 'Wigan'
				endif
			break
			case char7='L'
				printf 'Bournemouth'
			break
			case char7='M'
				if regyear>=1974
					printf 'Liverpool'
				else
					printf 'Bootle'
				endif
			break
			case char7='N'
				if regyear>=1981
					printf 'Manchester'
				elseif regyear>=1974
					printf 'Bolton'
				else
					printf 'Bury'
				endif
			break
			case char7='O'
				if regyear>=1974
					printf 'Preston'
				else
					printf 'Barrow-in-Furness'
				endif
			break
			case char7='P'
				if regyear>=1974
					printf 'Swansea'
				else
					printf 'Montgomeryshire'
				endif
			break
			case char7='R'
				if regyear>=1981
					printf 'Peterborough'
				else
					printf 'Cambridgeshire'
				endif
			break
			case char7='S'
				if regyear>=1974
					printf 'Dundee'
				else
					printf 'Perthshire'
				endif
			break
			case char7='T'
				printf 'Rotherham'
			break
			case char7='U'
				if regyear>=1974
					printf 'Bristol'
				else
					printf 'Brecknockshire'
				endif
			break
			case char7='V'
				if regyear>=1974
					printf 'Chelmsford'
				else
					printf 'Essex'
				endif
			break
			case char7='W'
				if regyear>=1974
					printf 'Peterborough'
				else
					printf 'Huntingdonshire'
				endif
			break
			case char7='X'
				if regyear>=1974
					printf 'Norwich'
				else
					printf 'Great Yarmouth'
				endif
			break
			case char7='Y'
				if regyear>=1974
					printf 'Bangor'
				else
					printf 'Anglesey'
				endif
			break
			case char7='Z'
				printf 'Belfast'
			break
		endswitch
	break
	case char6='F'
		switch
			whencase char7='A'
			whencase char7='B'
			whencase char7='C'
			whencase char7='D'
			whencase char7='E'
			whencase char7='F'
			whencase char7='G'
			whencase char7='H'
			whencase char7='I'
			whencase char7='J'
			whencase char7='K'
			whencase char7='L'
			whencase char7='M'
			whencase char7='N'
			whencase char7='O'
			whencase char7='P'
			whencase char7='R'
			whencase char7='S'
			whencase char7='T'
			whencase char7='U'
			whencase char7='V'
			whencase char7='W'
			whencase char7='X'
			whencase char7='Y'
			whencase char7='Z'
			whencase char7=' '
			otherwise
				printf "%s",invalid
			break
			case char7=' '
				printf 'Essex'
			break
			case char7='A'
				if regyear>=1974
					printf 'Stoke-on-Trent'
				else
					printf 'Burton-on-Trent'
				endif
			break
			case char7='B'
				if regyear>=1974
					printf 'Bristol'
				else
					printf 'Bath'
				endif
			break
			case char7='C'
				printf 'Oxford'
			break
			case char7='D'
				printf 'Dudley'
			break
			case char7='E'
				printf 'Lincoln'
			break
			case char7='F'
				if regyear>=1981
					printf 'Bangor'
				elseif regyear>=1974
					printf 'Aberyswyth'
				else
					printf 'Merionethshire'
				endif
			break
			case char7='G'
				if regyear>=1974
					printf 'Brighton'
				else
					printf 'Fifeshire'
				endif
			break
			case char7='H'
				printf 'Gloucester'
			break
			case char7='I'
				printf 'Tipperary'
			break
			case char7='J'
				printf 'Exeter'
			break
			case char7='K'
				if regyear>=1974
					printf 'Dudley'
				else
					printf 'Worcester'
				endif
			break
			case char7='L'
				printf 'Peterborough'
			break
			case char7='M'
				printf 'Chester'
			break
			case char7='N'
				if regyear>=1974
					printf 'Maidstone'
				else
					printf 'Canterbury'
				endif
			break
			case char7='O'
				if regyear>=1981
					printf 'Gloucester'
				elseif regyear>=1974
					printf 'Hereford'
				else
					printf 'Radnorshire'
				endif
			break
			case char7='P'
				if regyear>=1974
					printf 'Leicester'
				else
					printf 'Rutlandshire'
				endif
			break
			case char7='R'
				if regyear>=1974
					printf 'Preston'
				else
					printf 'Blackpool'
				endif
			break
			case char7='S'
				printf 'Edinburgh'
			break
			case char7='T'
				if regyear>=1974
					printf 'Newcastle'
				else
					printf 'Tynemouth'
				endif
			break
			case char7='U'
				printf 'Lincolnshire (Lindsey)'
			break
			case char7='V'
				if regyear>=1974
					printf 'Preston'
				else
					printf 'Blackpool'
				endif
			break
			case char7='W'
				printf 'Lincolnshire (Lindsey)'
			break
			case char7='X'
				if regyear>=1974
					printf 'Bournemouth'
				else
					printf 'Dorsetshire'
				endif
			break
			case char7='Y'
				if regyear>=1974
					printf 'Liverpool'
				else
					printf 'Southport'
				endif
			break
			case char7='Z'
				printf 'Belfast'
			break
		endswitch
	break
	case char6='G'
		switch
			whencase char7='A'
			whencase char7='B'
			whencase char7='C'
			whencase char7='D'
			whencase char7='E'
			whencase char7='F'
			whencase char7='G'
			whencase char7='H'
			whencase char7='J'
			whencase char7='K'
			whencase char7='L'
			whencase char7='M'
			whencase char7='N'
			whencase char7='O'
			whencase char7='P'
			whencase char7='R'
			whencase char7='S'
			whencase char7='T'
			whencase char7='U'
			whencase char7='V'
			whencase char7='W'
			whencase char7='X'
			whencase char7='Y'
			whencase char7='Z'
			whencase char7=' '
			otherwise
				printf "%s",invalid
			break
			case char7=' '
				printf 'Glasgow'
			break
			case char7='A'
				printf 'Glasgow'
			break
			case char7='B'
				printf 'Glasgow'
			break
			case char7='C'
				printf 'London'
			break
			case char7='D'
				printf 'Glasgow'
			break
			case char7='E'
				printf 'Glasgow'
			break
			case char7='F'
				printf 'London'
			break
			case char7='G'
				printf 'Glasgow'
			break
			case char7='H'
				printf 'London'
			break
			case char7='J'
				printf 'London'
			break
			case char7='K'
				printf 'London'
			break
			case char7='L'
				if regyear>=1974
					printf 'Truro'
				else
					printf 'Bath'
				endif
			break
			case char7='M'
				if regyear>=1974
					printf 'Reading'
				else
					printf 'Motherwell/Wishaw'
				endif
			break
			case char7='N'
				printf 'London'
			break
			case char7='O'
				printf 'London'
			break
			case char7='P'
				printf 'London'
			break
			case char7='R'
				if regyear>=1981
					printf 'Newcastle'
				elseif regyear>=1974
					printf 'Durham'
				else
					printf 'Sunderland'
				endif
			break
			case char7='S'
				if regyear>=1974
					printf 'Luton'
				else
					printf 'Perthshire'
				endif
			break
			case char7='T'
				printf 'London'
			break
			case char7='U'
				printf 'London'
			break
			case char7='V'
				if regyear>=1974
					printf 'Ipswich'
				else
					printf 'Suffolk (West)'
				endif
			break
			case char7='W'
				printf 'London'
			break
			case char7='X'
				printf 'London'
			break
			case char7='Y'
				printf 'London'
			break
			case char7='Z'
				printf 'Belfast'
			break
		endswitch
	break
	case char6='H'
		switch
			whencase char7='A'
			whencase char7='B'
			whencase char7='C'
			whencase char7='D'
			whencase char7='E'
			whencase char7='F'
			whencase char7='G'
			whencase char7='H'
			whencase char7='I'
			whencase char7='J'
			whencase char7='K'
			whencase char7='L'
			whencase char7='M'
			whencase char7='N'
			whencase char7='O'
			whencase char7='P'
			whencase char7='R'
			whencase char7='S'
			whencase char7='T'
			whencase char7='U'
			whencase char7='V'
			whencase char7='W'
			whencase char7='X'
			whencase char7='Y'
			whencase char7='Z'
			whencase char7=' '
			otherwise
				printf "%s",invalid
			break
			case char7=' '
				printf 'Middlesex'
			break
			case char7='A'
				if regyear>=1981
					printf 'Dudley'
				elseif regyear>=1974
					printf 'Warley'
				else
					printf 'Smethwick'
				endif
			break
			case char7='B'
				if regyear>=1974
					printf 'Cardiff'
				else
					printf 'Merthyr Tydfil'
				endif
			break
			case char7='C'
				if regyear>=1981
					printf 'Brighton'
				elseif regyear>=1974
					printf 'Hastings'
				else
					printf 'Eastbourne'
				endif
			break
			case char7='D'
				if regyear>=1974
					printf 'Huddersfield'
				else
					printf 'Dewsbury'
				endif
			break
			case char7='E'
				if regyear>=1974
					printf 'Sheffield'
				else
					printf 'Harnsley'
				endif
			break
			case char7='F'
				if regyear>=1974
					printf 'Liverpool'
				else
					printf 'Wallasey'
				endif
			break
			case char7='G'
				if regyear>=1974
					printf 'Preston'
				else
					printf 'Burnley'
				endif
			break
			case char7='H'
				printf 'Carlisle'
			break
			case char7='I'
				printf 'Tipperary (S.R.)'
			break
			case char7='J'
				if regyear>=1974
					printf 'Chelmsford'
				else
					printf 'Southend'
				endif
			break
			case char7='K'
				if regyear>=1974
					printf 'Chelmsford'
				else
					printf 'Essex'
				endif
			break
			case char7='L'
				if regyear>=1974
					printf 'Sheffield'
				else
					printf 'Wakefield'
				endif
			break
			case char7='M'
				if regyear>=1974
					printf 'London'
				elseif regyear>=1965
					printf 'Greater London Council'
				else
					printf 'East Ham'
				endif
			break
			case char7='N'
				if regyear>=1981
					printf 'Middlesborough'
				else
					printf 'Darlington'
				endif
			break
			case char7='O'
				if regyear>=1981
					printf 'Bournemouth'
				elseif regyear>=1974
					printf 'Salisbury'
				else
					printf 'Hampshire (Southampton)'
				endif
			break
			case char7='P'
				printf 'Coventry'
			break
			case char7='R'
				printf 'Wiltshire'
			break
			case char7='S'
				if regyear>=1974
					printf 'Glasgow'
				else
					printf 'Renfrewshire'
				endif
			break
			case char7='T'
				printf 'Bristol'
			break
			case char7='U'
				printf 'Bristol'
			break
			case char7='V'
				if regyear>=1974
					printf 'London'
				elseif regyear>=1965
					printf 'Greater London Council'
				else
					printf 'East Ham'
				endif
			break
			case char7='W'
				printf 'Bristol'
			break
			case char7='X'
				if regyear>=1981
					printf 'London'
				elseif regyear>=1974
					printf 'Greater London Council'
				else
					printf 'Middlesex'
				endif
			break
			case char7='Y'
				printf 'Bristol'
			break
			case char7='Z'
				printf 'Tyrone'
			break
		endswitch
	break
	case char6='I'
		switch
			whencase char7='A'
			whencase char7='B'
			whencase char7='C'
			whencase char7='D'
			whencase char7='E'
			whencase char7='F'
			whencase char7='H'
			whencase char7='J'
			whencase char7='K'
			whencase char7='L'
			whencase char7='M'
			whencase char7='N'
			whencase char7='O'
			whencase char7='P'
			whencase char7='R'
			whencase char7='T'
			whencase char7='U'
			whencase char7='W'
			whencase char7='X'
			whencase char7='Y'
			whencase char7='Z'
			otherwise
				printf "%s",invalid
			break
			case char7='A'
				printf 'Antrim'
			break
			case char7='B'
				printf 'Armagh'
			break
			case char7='C'
				printf 'Carlow'
			break
			case char7='D'
				printf 'Cavan'
			break
			case char7='E'
				printf 'Clare'
			break
			case char7='F'
				printf 'Cork (County)'
			break
			case char7='H'
				printf 'Donegal'
			break
			case char7='J'
				printf 'Down'
			break
			case char7='K'
				printf 'City and County of Dublin'
			break
			case char7='L'
				printf 'Permanagh'
			break
			case char7='M'
				printf 'Galway'
			break
			case char7='N'
				printf 'Kerry'
			break
			case char7='O'
				printf 'Kildare'
			break
			case char7='P'
				printf 'Kilkenny'
			break
			case char7='R'
				printf 'Offaly'
			break
			case char7='T'
				printf 'Leitrim'
			break
			case char7='U'
				printf 'Limerick'
			break
			case char7='W'
				printf 'Londonderry'
			break
			case char7='X'
				printf 'Longford'
			break
			case char7='Y'
				printf 'Louth'
			break
			case char7='Z'
				printf 'Mayo'
			break
		endswitch
	break
	case char6='J'
		switch
			whencase char7='A'
			whencase char7='B'
			whencase char7='C'
			whencase char7='D'
			whencase char7='E'
			whencase char7='F'
			whencase char7='G'
			whencase char7='H'
			whencase char7='I'
			whencase char7='J'
			whencase char7='K'
			whencase char7='L'
			whencase char7='M'
			whencase char7='N'
			whencase char7='O'
			whencase char7='P'
			whencase char7='R'
			whencase char7='S'
			whencase char7='T'
			whencase char7='U'
			whencase char7='V'
			whencase char7='W'
			whencase char7='X'
			whencase char7='Y'
			whencase char7='Z'
			whencase char7=' '
			otherwise
				printf "%s",invalid
			break
			case char7=' '
				printf 'Durham (County)'
			break
			case char7='A'
				if regyear>=1974
					printf 'Manchester'
				else
					printf 'Stockport'
				endif
			break
			case char7='B'
				if regyear>=1974
					printf 'Reading'
				else
					printf 'Berkshire'
				endif
			break
			case char7='C'
				if regyear>=1974
					printf 'Bangor'
				else
					printf 'Caernarvonshire'
				endif
			break
			case char7='D'
				if regyear>=1974
					printf 'London'
				elseif regyear>=1965
					printf 'Greater London Council'
				else
					printf 'West Ham'
				endif
			break
			case char7='E'
				if regyear>=1974
					printf 'Peterborough'
				elseif regyear>=1965
					printf 'Cambridgeshire'
				else
					printf 'Isle of Ely'
				endif
			break
			case char7='F'
				printf 'Leicester'
			break
			case char7='G'
				if regyear>=1981
					printf 'Maidstone'
				else
					printf 'Canterbury'
				endif
			break
			case char7='H'
				if regyear>=1974
					printf 'Reading'
				else
					printf 'Hertfordshire'
				endif
			break
			case char7='I'
				printf 'Tyrone'
			break
			case char7='J'
				if regyear>=1981
					printf 'Maidstone'
				elseif regyear>=1974
					printf 'Canterbury'
				else
					printf 'London'
				endif
			break
			case char7='K'
				if regyear>=1981
					printf 'Brighton'
				elseif regyear>=1974
					printf 'Hastings'
				else
					printf 'Eastbourne'
				endif
			break
			case char7='L'
				if regyear>=1981
					printf 'Lincoln'
				elseif regyear>=1974
					printf 'Boston'
				else
					printf 'Lincolnshire (Holland)'
				endif
			break
			case char7='M'
				if regyear>=1974
					printf 'Reading'
				else
					printf 'Westmorland'
				endif
			break
			case char7='N'
				if regyear>=1974
					printf 'Chelmsford'
				else
					printf 'Southend'
				endif
			break
			case char7='O'
				printf 'Oxford'
			break
			case char7='P'
				if regyear>=1981
					printf 'Liverpool'
				elseif regyear>=1974
					printf 'Warrington'
				else
					printf 'Wigan'
				endif
			break
			case char7='R'
				if regyear>=1974
					printf 'Newcastle'
				else
					printf 'Northumberland'
				endif
			break
			case char7='S'
				if regyear>=1981
					printf 'Inverness'
				elseif regyear>=1974
					printf 'Stornoway'
				else
					printf 'Ross & Cromarty'
				endif
			break
			case char7='T'
				if regyear>=1974
					printf 'Bournemouth'
				else
					printf 'Dorsetshire'
				endif
			break
			case char7='U'
				printf 'Leicestershire'
			break
			case char7='V'
				if regyear>=1974
					printf 'Lincoln'
				else
					printf 'Grimsby'
				endif
			break
			case char7='W'
				if regyear>=1974
					printf 'Wolverhampton'
				else
					printf 'Birmingham'
				endif
			break
			case char7='X'
				if regyear>=1974
					printf 'Huddersfield'
				else
					printf 'Halifax'
				endif
			break
			case char7='Y'
				if regyear>=1981
					printf 'Exeter'
				else
					printf 'Plymouth'
				endif
			break
			case char7='Z'
				printf 'Down'
			break
		endswitch
	break
	case char6='K'
		switch
			whencase char7>='A' and char7<='D'
			whencase char7='E'
			whencase char7='F'
			whencase char7='G'
			whencase char7='H'
			whencase char7='I'
			whencase char7>='J' and char7<='R'
			whencase char7='S'
			whencase char7='T'
			whencase char7='U'
			whencase char7='V'
			whencase char7='W'
			whencase char7='X'
			whencase char7='Y'
			whencase char7='Z'
			whencase char7=' '
			otherwise
				printf "%s",invalid
			break
			case char7=' '
				printf 'Liverpool'
			break
			case char7>='A' and char7<='D'
				printf 'Liverpool'
			break
			case char7='E'
				if regyear>=1974
					printf 'Maidstone'
				else
					printf 'Kent'
				endif
			break
			case char7='F'
				printf 'Liverpool'
			break
			case char7='G'
				printf 'Cardiff'
			break
			case char7='H'
				printf 'Kingston-upon-Hull'
			break
			case char7='I'
				printf 'Waterford'
			break
			case char7>='J' and char7<='R'
				if regyear>=1974
					printf 'Maidstone'
				else
					printf 'Kent'
				endif
			break
			case char7='S'
				if regyear>=1981
					printf 'Edinburgh'
				elseif regyear>=1974
					printf 'Selkirk'
				else
					printf 'Roxburghshire'
				endif
			break
			case char7='T'
				if regyear>=1981
					printf 'Maidstone'
				elseif regyear>=1974
					printf 'Canterbury'
				else
					printf 'Kent'
				endif
			break
			case char7='U'
				if regyear>=1974
					printf 'Sheffield'
				else
					printf 'Bradford'
				endif
			break
			case char7='V'
				printf 'Coventry'
			break
			case char7='W'
				if regyear>=1974
					printf 'Sheffield'
				else
					printf 'Bradford'
				endif
			break
			case char7='X'
				if regyear>=1974
					printf 'Luton'
				else
					printf 'Buckinghamshire'
				endif
			break
			case char7='Y'
				if regyear>=1974
					printf 'Sheffield'
				else
					printf 'Bradford'
				endif
			break
			case char7='Z'
				printf 'Antrim'
			break
		endswitch
	break
	case char6='L'
		switch
			whencase char7>='A' and char7<='F'
			whencase char7='G'
			whencase char7='H'
			whencase char7='I'
			whencase char7='J'
			whencase char7>='K' and char7<='R'
			whencase char7='S'
			whencase char7>='T' and char7<='Y'
			whencase char7='Z'
			whencase char7=' '
			otherwise
				printf "%s",invalid
			break
			case char7=' '
				printf 'Glamorganshire'
			break
			case char7>='A' and char7<='F'
				printf 'London'
			break
			case char7='G'
				printf 'Cheshire'
			break
			case char7='H'
				printf 'Cheshire'
			break
			case char7='I'
				printf 'Westmeath'
			break
			case char7='J'
				printf 'Bournemouth'
			break
			case char7>='K' and char7<='R'
				printf 'London'
			break
			case char7='S'
				if regyear>=1981
					printf 'Edinburgh'
				elseif regyear>=1974
					printf 'Selkirk/Stirling'
				else
					printf 'Selkirkshire'
				endif
			break
			case char7>='T' and char7<='Y'
				printf 'London'
			break
			case char7='Z'
				printf 'Armagh'
			break
		endswitch
	break
	case char6='M'
		switch
			whencase char7>='A' and char7<='B'
			whencase char7>='C' and char7<='H'
			whencase char7='I'
			whencase char7='J'
			whencase char7>='K' and char7<='M'
			whencase char7='N'
			whencase char7='O'
			whencase char7='P'
			whencase char7='R'
			whencase char7='S'
			whencase char7>='T' and char7<='Y'
			whencase char7='Z'
			whencase char7=' '
			otherwise
				printf "%s",invalid
			break
			case char7=' '
				printf 'Cheshire'
			break
			case char7>='A' and char7<='B'
				printf 'Cheshire'
			break
			case char7>='C' and char7<='H'
				if regyear>=1974
					printf 'London'
				else
					printf 'Middlesex'
				endif
			break
			case char7='I'
				printf 'Wexford'
			break
			case char7='J'
				if regyear>=1974
					printf 'Luton'
				else
					printf 'Bedfordshire'
				endif
			break
			case char7>='K' and char7<='M'
				if regyear>=1974
					printf 'London'
				else
					printf 'Middlesex'
				endif
			break
			case char7='N'
				printf 'Isle of Man'
			break
			case char7='O'
				if regyear>=1974
					printf 'Reading'
				else
					printf 'Berkshire'
				endif
			break
			case char7='P'
				if regyear>=1974
					printf 'London'
				else
					printf 'Middlesex'
				endif
			break
			case char7='R'
				if regyear>=1974
					printf 'Swindon'
				else
					printf 'Wiltshire'
				endif
			break
			case char7='S'
				if regyear>=1981
					printf 'Edinburgh'
				else
					printf 'Stirlingshire'
				endif
			break
			case char7>='T' and char7<='Y'
				if regyear>=1974
					printf 'London'
				else
					printf 'Middlesex'
				endif
			break
			case char7='Z'
				printf 'Belfast'
			break
		endswitch
	break
	case char6='N'
		switch
			whencase char7>='A' and char7<='F'
			whencase char7='G'
			whencase char7='H'
			whencase char7='I'
			whencase char7='J'
			whencase char7='K'
			whencase char7='L'
			whencase char7='M'
			whencase char7='N'
			whencase char7='O'
			whencase char7='P'
			whencase char7='R'
			whencase char7='S'
			whencase char7='T'
			whencase char7='U'
			whencase char7='V'
			whencase char7='W'
			whencase char7='X'
			whencase char7='Y'
			whencase char7='Z'
			whencase char7=' '
			otherwise
				printf "%s",invalid
			break
			case char7=' '
				printf 'Manchester'
			break
			case char7>='A' and char7<='F'
				printf 'Manchester'
			break
			case char7='G'
				if regyear>=1974
					printf 'Norwich'
				else
					printf 'Norfolk'
				endif
			break
			case char7='H'
				printf 'Northampton'
			break
			case char7='I'
				printf 'Wicklow'
			break
			case char7='J'
				if regyear>=1974
					printf 'Brighton'
				else
					printf 'Sussex (East)'
				endif
			break
			case char7='K'
				if regyear>=1974
					printf 'Luton'
				else
					printf 'Hertfordshire'
				endif
			break
			case char7='L'
				if regyear>=1974
					printf 'Newcastle'
				else
					printf 'Northumberland'
				endif
			break
			case char7='M'
				if regyear>=1974
					printf 'Luton'
				else
					printf 'Bedfordshire'
				endif
			break
			case char7='N'
				printf 'Nottinghamshire'
			break
			case char7='O'
				if regyear>=1974
					printf 'Chelmsford'
				else
					printf 'Essex'
				endif
			break
			case char7='P'
				printf 'Worcestershire'
			break
			case char7='R'
				printf 'Leicestershire'
			break
			case char7='S'
				if regyear>=1974
					printf 'Glasgow'
				else
					printf 'Sutherlandshire'
				endif
			break
			case char7='T'
				if regyear>=1974
					printf 'Shrewsbury'
				else
					printf 'Salop'
				endif
			break
			case char7='U'
				if regyear>=1974
					printf 'Nottingham'
				else
					printf 'Derbyshire'
				endif
			break
			case char7='V'
				printf 'Northamptonshire'
			break
			case char7='W'
				printf 'Leeds'
			break
			case char7='X'
				if regyear>=1974
					printf 'Dudley'
				else
					printf 'Warwickshire'
				endif
			break
			case char7='Y'
				if regyear>=1974
					printf 'Cardiff'
				else
					printf 'Glamorganshire'
				endif
			break
			case char7='Z'
				printf 'Londonderry'
			break
		endswitch
	break
	case char6='O'
		switch
			whencase char7>='A' and char7<='C'
			whencase char7='D'
			whencase char7>='E' and char7<='H'
			whencase char7='I'
			whencase char7>='J' and char7<='N'
			whencase char7='O'
			whencase char7='P'
			whencase char7='R'
			whencase char7='S'
			whencase char7='T'
			whencase char7='U'
			whencase char7='V'
			whencase char7='W'
			whencase char7='X'
			whencase char7='Y'
			whencase char7='Z'
			whencase char7=' '
			otherwise
				printf "%s",invalid
			break
			case char7=' '
				printf 'Birmingham'
			break
			case char7>='A' and char7<='C'
				printf 'Birmingham'
			break
			case char7='D'
				printf 'Devonshire'
			break
			case char7>='E' and char7<='H'
				printf 'Birmingham'
			break
			case char7='I'
				printf 'Belfast'
			break
			case char7>='J' and char7<='N'
				printf 'Birmingham'
			break
			case char7='O'
				if regyear>=1974
					printf 'Chelmsford'
				else
					printf 'Essex'
				endif
			break
			case char7='P'
				printf 'Birmingham'
			break
			case char7='R'
				if regyear>=1974
					printf 'Portsmouth'
				else
					printf 'Hampshire (Southampton)'
				endif
			break
			case char7='S'
				if regyear>=1981
					printf 'Glasgow'
				elseif regyear>=1974
					printf 'Stanraer'
				else
					printf 'Wigtownshire'
				endif
			break
			case char7='T'
				if regyear>=1974
					printf 'Portsmouth'
				else
					printf 'Hampshire (Southampton)'
				endif
			break
			case char7='U'
				printf 'Hampshire (Southampton)'
			break
			case char7='V'
				printf 'Birmingham'
			break
			case char7='W'
				if regyear>=1974
					printf 'Portsmouth'
				else
					printf 'Southampton'
				endif
			break
			case char7='X'
				printf 'Birmingham'
			break
			case char7='Y'
				if regyear>=1974
					printf 'London'
				else
					printf 'Croydon'
				endif
			break
			case char7='Z'
				printf 'Belfast'
			break
		endswitch
	break
	case char6='P'
		switch
			whencase char7>='A' and char7<='H'
			whencase char7='I'
			whencase char7>='J' and char7<='L'
			whencase char7='M'
			whencase char7='N'
			whencase char7='O'
			whencase char7='P'
			whencase char7='R'
			whencase char7='S'
			whencase char7='T'
			whencase char7='U'
			whencase char7='V'
			whencase char7='W'
			whencase char7='X'
			whencase char7='Y'
			whencase char7='Z'
			whencase char7=' '
			otherwise
				printf "%s",invalid
			break
			case char7=' '
				printf 'Surrey'
			break
			case char7>='A' and char7<='H'
				if regyear>=1974
					printf 'Guildford'
				else
					printf 'Surrey'
				endif
			break
			case char7='I'
				printf 'Cork'
			break
			case char7>='J' and char7<='L'
				if regyear>=1974
					printf 'Guildford'
				else
					printf 'Surrey'
				endif
			break
			case char7='M'
				if regyear>=1974
					printf 'Guildford'
				else
					printf 'Sussex (East)'
				endif
			break
			case char7='N'
				if regyear>=1974
					printf 'Brighton'
				else
					printf 'Sussex (East)'
				endif
			break
			case char7='O'
				if regyear>=1974
					printf 'Portsmouth'
				else
					printf 'Sussex (West)'
				endif
			break
			case char7='P'
				if regyear>=1974
					printf 'Luton'
				else
					printf 'Buckinghamshire'
				endif
			break
			case char7='R'
				if regyear>=1974
					printf 'Bournemouth'
				else
					printf 'Dorset'
				endif
			break
			case char7='S'
				if regyear>=1981
					printf 'Aberdeen'
				elseif regyear>=1974
					printf 'Lerwick'
				else
					printf 'Shetland'
				endif
			break
			case char7='T'
				if regyear>=1981
					printf 'Newcastle'
				else
					printf 'Durham (County)'
				endif
			break
			case char7='U'
				if regyear>=1974
					printf 'Chelmsford'
				else
					printf 'Essex'
				endif
			break
			case char7='V'
				printf 'Ipswich'
			break
			case char7='W'
				if regyear>=1974
					printf 'Norwich'
				else
					printf 'Norfolk'
				endif
			break
			case char7='X'
				if regyear>=1974
					printf 'Portsmouth'
				else
					printf 'Sussex (West)'
				endif
			break
			case char7='Y'
				if regyear>=1974
					printf 'Middlesborough'
				else
					printf 'Yorkshire (N.Riding)'
				endif
			break
			case char7='Z'
					printf 'Belfast'
			break
		endswitch
	break
	case char6='Q'
		switch
			whencase char7>='A' and char7<='F'
			whencase char7='Q'
			whencase char7='S'
			otherwise
				printf "%s",invalid
			break
			case char7>='A' and char7<='F'
				printf 'London'
			break
			case char7='Q'
				printf 'London'
			break
			case char7='S'
				printf 'London'
			break
		endswitch
	break
	case char6='R'
		switch
			whencase char7>='A' and char7<='C'
			whencase char7='D'
			whencase char7>='E' and char7<='F'
			whencase char7='G'
			whencase char7='H'
			whencase char7='I'
			whencase char7='J'
			whencase char7='K'
			whencase char7='L'
			whencase char7='M'
			whencase char7='N'
			whencase char7='O'
			whencase char7='P'
			whencase char7='R'
			whencase char7='S'
			whencase char7='T'
			whencase char7='U'
			whencase char7='V'
			whencase char7='W'
			whencase char7='X'
			whencase char7='Y'
			whencase char7='Z'
			whencase char7=' '
			otherwise
				printf "%s",invalid
			break
			case char7=' '
				printf 'Derbyshire'
			break
			case char7>='A' and char7<='C'
				if regyear>=1974
					printf 'Nottingham'
				else
					printf 'Derbyshire'
				endif
			break
			case char7='D'
				printf 'Reading'
			break
			case char7>='E' and char7<='F'
				if regyear>=1974
					printf 'Stoke-on-Trent'
				else
					printf 'Staffordshire'
				endif
			break
			case char7='G'
				if regyear>=1974
					printf 'Newcastle'
				else
					printf 'Aberdeen'
				endif
			break
			case char7='H'
				if regyear>=1974
					printf 'Hull'
				else
					printf 'Kingston-upon-Hull'
				endif
			break
			case char7='I'
				printf 'City and County of Dublin'
			break
			case char7='J'
				if regyear>=1974
					printf 'Manchester'
				else
					printf 'Salford'
				endif
			break
			case char7='K'
				if regyear>=1974
					printf 'London'
				else
					printf 'Croydon'
				endif
			break
			case char7='L'
				if regyear>=1974
					printf 'Truro'
				else
					printf 'Cornwall'
				endif
			break
			case char7='M'
				if regyear>=1974
					printf 'Carlisle'
				else
					printf 'Cumberland'
				endif
			break
			case char7='N'
				printf 'Preston'
			break
			case char7='O'
				if regyear>=1974
					printf 'Luton'
				else
					printf 'Hertfordshire'
				endif
			break
			case char7='P'
				printf 'Northamptonshire'
			break
			case char7='R'
				printf 'Nottinghamshire'
			break
			case char7='S'
				printf 'Aberdeen'
			break
			case char7='T'
				if regyear>=1974
					printf 'Ipswich'
				else
					printf 'Suffolk (East)'
				endif
			break
			case char7='U'
				printf 'Bournemouth'
			break
			case char7='V'
				printf 'Portsmouth'
			break
			case char7='W'
				printf 'Coventry'
			break
			case char7='X'
				if regyear>=1974
					printf 'Reading'
				else
					printf 'Berkshire'
				endif
			break
			case char7='Y'
				printf 'Leicester'
			break

			case char7='Z'
				printf 'Antrim'
			break
		endswitch
	break
	case char6='S'
		switch
			whencase char7='A'
			whencase char7='B'
			whencase char7='C'
			whencase char7='D'
			whencase char7='E'
			whencase char7='F'
			whencase char7='G'
			whencase char7='H'
			whencase char7='J'
			whencase char7='K'
			whencase char7='L'
			whencase char7='M'
			whencase char7='N'
			whencase char7='O'
			whencase char7='P'
			whencase char7='R'
			whencase char7='S'
			whencase char7='T'
			whencase char7='U'
			whencase char7='V'
			whencase char7='W'
			whencase char7='X'
			whencase char7='Y'
			whencase char7='Z'
			whencase char7=' '
			otherwise
				printf "%s",invalid
			break
			case char7=' '
				printf 'Edinburgh'
			break
			case char7='A'
				printf 'Aberdeenshire'
			break
			case char7='B'
				if regyear>=1981
					printf 'Glasgow'
				elseif regyear>=1974
					printf 'Oban'
				else
					printf 'Argyllshire'
				endif
			break
			case char7='C'
				printf 'Edinburgh'
			break
			case char7='D'
				if regyear>=1981
					printf 'Glasgow'
				else
					printf 'Ayrshire'
				endif
			break
			case char7='E'
				if regyear>=1981
					printf 'Aberdeen'
				elseif regyear>=1974
					printf 'Keith'
				else
					printf 'Banffshire'
				endif
			break
			case char7='F'
				printf 'Edinburgh'
			break
			case char7='G'
				printf 'Edinburgh'
			break
			case char7='H'
				if regyear>=1981
					printf 'Edinburgh'
				elseif regyear>=1974
					printf 'Selkirk'
				else
					printf 'Berwickshire'
				endif
			break
			case char7='J'
				if regyear>=1981
					printf 'Glasgow'
				elseif regyear>=1974
					printf 'Ayr'
				else
					printf 'Buteshire'
				endif
			break
			case char7='K'
				if regyear>=1981
					printf 'Inverness'
				elseif regyear>=1974
					printf 'Wick'
				else
					printf 'Caithness-shire'
				endif
			break
			case char7='L'
				if regyear>=1974
					printf 'Dundee'
				else
					printf 'Clackmannanshire'
				endif
			break
			case char7='M'
				if regyear>=1974
					printf 'Carlisle'
				else
					printf 'Dumfriesshire'
				endif
			break
			case char7='N'
				if regyear>=1974
					printf 'Dundee'
				else
					printf 'Dunbartonshire'
				endif
			break
			case char7='O'
				if regyear>=1974
					printf 'Aberdeen'
				else
					printf 'Morayshire'
				endif
			break
			case char7='P'
				if regyear>=1974
					printf 'Dundee'
				else
					printf 'Fifeshire'
				endif
			break
			case char7='R'
				if regyear>=1974
					printf 'Dundee'
				else
					printf 'Angus'
				endif
			break
			case char7='S'
				if regyear>=1974
					printf 'Aberdeen'
				else
					printf 'East Lothian'
				endif
			break
			case char7='T'
				printf 'Inverness-shire'
			break
			case char7='U'
				if regyear>=1974
					printf 'Glasgow'
				else
					printf 'Kincardineshire'
				endif
			break
			case char7='V'
				if regyear>=1974
					printf '%s',invalid
				else
					printf 'Kinross-shire'
				endif
			break
			case char7='W'
				if regyear>=1981
					printf 'Carlisle'
				elseif regyear>=1974
					printf 'Dumfries'
				else
					printf 'Kirkcudbrightshire'
				endif
			break
			case char7='X'
				if regyear>=1981
					printf 'Edinburgh'
				else
					printf 'West Lothian'
				endif
			break
			case char7='Y'
				printf 'Midlothian'
			break
			case char7='Z'
				printf 'Down'
			break

		endswitch
	break
	case char6='T'
		switch
			whencase char7='A'
			whencase char7='B'
			whencase char7='C'
			whencase char7='D'
			whencase char7='E'
			whencase char7='F'
			whencase char7='G'
			whencase char7='H'
			whencase char7='I'
			whencase char7='J'
			whencase char7='K'
			whencase char7='L'
			whencase char7='M'
			whencase char7='N'
			whencase char7='O'
			whencase char7='P'
			whencase char7='R'
			whencase char7='S'
			whencase char7='T'
			whencase char7='U'
			whencase char7='V'
			whencase char7='W'
			whencase char7='X'
			whencase char7='Y'
			whencase char7='Z'
			whencase char7=' '
			otherwise
				printf "%s",invalid
			break
			case char7=' '
				printf 'Devonshire'
			break
			case char7='A'
				if regyear>=1974
					printf 'Exeter'
				else
					printf 'Devonshire'
				endif
			break
			case char7='B'
				if regyear>=1981
					printf 'Liverpool'
				elseif regyear>=1974
					printf 'Warrington'
				else
					printf 'Lancashire'
				endif
			break
			case char7='C'
				if regyear>=1974
					printf 'Bristol'
				else
					printf 'Lancashire'
				endif
			break
			case char7='D'
				if regyear>=1981
					printf 'Manchester'
				elseif regyear>=1974
					printf 'Bolton'
				else
					printf 'Lancashire'
				endif
			break
			case char7='E'
				if regyear>=1981
					printf 'Manchester'
				elseif regyear>=1974
					printf 'Bolton'
				else
					printf 'Lancashire'
				endif
			break
			case char7='F'
				if regyear>=1974
					printf 'Reading'
				else
					printf 'Lancashire'
				endif
			break
			case char7='G'
				if regyear>=1974
					printf 'Cardiff'
				else
					printf 'Glamorganshire'
				endif
			break
			case char7='H'
				if regyear>=1974
					printf 'Swansea'
				else
					printf 'Carmarthenshire'
				endif
			break
			case char7='I'
				printf 'Limerick'
			break
			case char7='J'
				if regyear>=1974
					printf 'Liverpool'
				else
					printf 'Lancashire'
				endif
			break
			case char7='K'
				if regyear>=1981
					printf 'Exeter'
				elseif regyear>=1974
					printf 'Plymouth'
				else
					printf 'Dorsetshire'
				endif
			break
			case char7='L'
				if regyear>=1974
					printf 'Lincoln'
				else
					printf 'Lincolnshire (Kesteven)'
				endif
			break
			case char7='M'
				if regyear>=1974
					printf 'Luton'
				else
					printf 'Bedfordshire'
				endif
			break
			case char7='N'
				printf 'Newcastle-upon-Tyne'
			break
			case char7='O'
				printf 'Nottingham'
			break
			case char7='P'
				printf 'Portsmouth'
			break
			case char7='R'
				if regyear>=1974
					printf 'Portsmouth'
				else
					printf 'Southampton'
				endif
			break
			case char7='S'
				printf 'Dundee'
			break
			case char7='T'
				if regyear>=1974
					printf 'Exeter'
				else
					printf 'Devonshire'
				endif
			break
			case char7='U'
				if regyear>=1974
					printf 'Chester'
				else
					printf 'Cheshire'
				endif
			break
			case char7='V'
				printf 'Nottingham'
			break
			case char7='W'
				if regyear>=1974
					printf 'Chelmsford'
				else
					printf 'Essex'
				endif
			break
			case char7='X'
				if regyear>=1974
					printf 'Cardiff'
				else
					printf 'Glamorganshire'
				endif
			break
			case char7='Y'
				if regyear>=1974
					printf 'Newcastle'
				else
					printf 'Northumberland'
				endif
			break
			case char7='Z'
				printf 'Belfast'
			break
		endswitch
	break
	case char6='U'
		switch
			whencase char7>='A' and char7<='B'
			whencase char7='C'
			whencase char7='D'
			whencase char7='E'
			whencase char7='F'
			whencase char7='G'
			whencase char7='H'
			whencase char7='I'
			whencase char7='J'
			whencase char7='K'
			whencase char7='L'
			whencase char7='M'
			whencase char7='N'
			whencase char7='O'
			whencase char7='P'
			whencase char7='R'
			whencase char7='S'
			whencase char7='T'
			whencase char7>='U' and char7<='W'
			whencase char7='X'
			whencase char7='Y'
			whencase char7='Z'
			whencase char7=' '
			otherwise
				printf "%s",invalid
			break
			case char7=' '
				printf 'Leeds'
			break
			case char7>='A' and char7<='B'
				printf 'Leeds'
			break
			case char7='C'
				printf 'London'
			break
			case char7='D'
				printf 'Oxfordshire'
			break
			case char7='E'
				if regyear>=1974
					printf 'Dudley'
				else
					printf 'Warwickshire'
				endif
			break
			case char7='F'
				printf 'Brighton'
			break
			case char7='G'
				printf 'Leeds'
			break
			case char7='H'
				printf 'Cardiff'
			break
			case char7='I'
				printf 'Londonderry'
			break
			case char7='J'
				if regyear>=1974
					printf 'Shrewsbury'
				else
					printf 'Salop'
				endif
			break
			case char7='K'
				if regyear>=1974
					printf 'Birmingham'
				else
					printf 'Wolverhampton'
				endif
			break
			case char7='L'
				printf 'London'
			break
			case char7='M'
				printf 'Leeds'
			break
			case char7='N'
				if regyear>=1981
					printf 'Exeter'
				elseif regyear>=1974
					printf 'Barnstable'
				else
					printf 'Denbighshire'
				endif
			break
			case char7='O'
				if regyear>=1981
					printf 'Exeter'
				elseif regyear>=1974
					printf 'Barnstable'
				else
					printf 'Devonshire'
				endif
			break
			case char7='P'
				if regyear>=1981
					printf 'Newcastle'
				else
					printf 'Durham (County)'
				endif
			break
			case char7='R'
				if regyear>=1974
					printf 'Luton'
				else
					printf 'Hertfordshire'
				endif
			break
			case char7='S'
				printf 'Glasgow'
			break
			case char7='T'
				printf 'Leicestershire'
			break
			case char7>='U' and char7<='W'
				printf 'London'
			break
			case char7='X'
				if regyear>=1974
					printf 'Shrewsbury'
				else
					printf 'Salop'
				endif
			break
			case char7='Y'
				printf 'Worcestershire'
			break
			case char7='Z'
				printf 'Belfast'
			break
		endswitch
	break
	case char6='V'
		switch
			whencase char7='A'
			whencase char7='B'
			whencase char7='C'
			whencase char7='D'
			whencase char7='E'
			whencase char7='F'
			whencase char7='G'
			whencase char7='H'
			whencase char7='J'
			whencase char7='K'
			whencase char7='L'
			whencase char7='M'
			whencase char7='N'
			whencase char7='O'
			whencase char7='P'
			whencase char7='R'
			whencase char7='S'
			whencase char7='T'
			whencase char7='U'
			whencase char7='V'
			whencase char7='W'
			whencase char7='X'
			whencase char7='Y'
			whencase char7=' '
			otherwise
				printf "%s",invalid
			break
			case char7=' '
				printf 'Lanarkshire'
			break
			case char7='A'
				if regyear>=1981
					printf 'Peterborough'
				elseif regyear>=1974
					printf 'Cambridge'
				else
					printf 'Lanarkshire'
				endif
			break
			case char7='B'
				if regyear>=1981
					printf 'Maidstone'
				elseif regyear>=1974
					printf 'Canterbury'
				elseif regyear>=1965
					printf 'London'
				else
					printf 'Croydon'
				endif
			break
			case char7='C'
				printf 'Coventry'
			break
			case char7='D'
				if regyear>=1974
					printf 'Luton'
				else
					printf 'Lanarkshire'
				endif
			break
			case char7='E'
				if regyear>=1981
					printf 'Peterborough'
				else
					printf 'Cambridgeshire'
				endif
			break
			case char7='F'
				if regyear>=1974
					printf 'Norwich'
				else
					printf 'Norfolk'
				endif
			break
			case char7='G'
				printf 'Norwich'
			break
			case char7='H'
				printf 'Huddersfield'
			break
			case char7='J'
				if regyear>=1981
					printf 'Gloucester'
				else
					printf 'Herefordshire'
				endif
			break
			case char7='K'
				printf 'Newcastle-upon-Tyne'
			break
			case char7='L'
				printf 'Lincoln'
			break
			case char7='M'
				printf 'Manchester'
			break
			case char7='N'
				if regyear>=1974
					printf 'Middlesborough'
				else
					printf 'Yorkshire (N.Riding)'
				endif
			break
			case char7='O'
				printf 'Nottinghamshire'
			break
			case char7='P'
				printf 'Birmingham'
			break
			case char7='R'
				printf 'Manchester'
			break
			case char7='S'
				if regyear>=1974
					printf 'Luton'
				else
					printf 'Greenock'
				endif
			break
			case char7='T'
				printf 'Stoke-on-Trent'
			break
			case char7='U'
				printf 'Manchester'
			break
			case char7='V'
				printf 'Northampton'
			break
			case char7='W'
				if regyear>=1974
					printf 'Chelmsford'
				else
					printf 'Essex'
				endif
			break
			case char7='X'
				if regyear>=1974
					printf 'Chelmsford'
				else
					printf 'Essex'
				endif
			break
			case char7='Y'
				if regyear>=1974
					printf 'Leeds'
				else
					printf 'York'
				endif
			break
		endswitch
	break
	case char6='W'
		switch
			whencase char7='A'
			whencase char7='B'
			whencase char7='C'
			whencase char7='D'
			whencase char7='E'
			whencase char7='F'
			whencase char7='G'
			whencase char7='H'
			whencase char7='I'
			whencase char7='J'
			whencase char7='K'
			whencase char7='L'
			whencase char7='M'
			whencase char7='N'
			whencase char7='O'
			whencase char7='P'
			whencase char7='R'
			whencase char7='S'
			whencase char7='T'
			whencase char7='U'
			whencase char7='V'
			whencase char7='W'
			whencase char7='X'
			whencase char7='Y'
			whencase char7=' '
			otherwise
				printf "%s",invalid
			break
			case char7=' '
				printf 'Sheffield'
			break
			case char7='A'
				printf 'Sheffield'
			break
			case char7='B'
				printf 'Sheffield'
			break
			case char7='C'
				if regyear>=1974
					printf 'Chelmsford'
				else
					printf '%s',invalid
				endif
			break
			case char7='D'
				if regyear>=1974
					printf 'Dudley'
				else
					printf 'Warwickshire'
				endif
			break
			case char7='E'
				printf 'Sheffield'
			break
			case char7='F'
				if regyear>=1974
					printf 'Sheffield'
				else
					printf 'Yorkshire (E.Riding)'
				endif
			break
			case char7='G'
				if regyear>=1974
					printf 'Sheffield'
				else
					printf 'Stirlingshire'
				endif
			break
			case char7='H'
				if regyear>=1981
					printf 'Manchester'
				else
					printf 'Bolton'
				endif
			break
			case char7='I'
				printf 'Waterford'
			break
			case char7='J'
				printf 'Sheffield'
			break
			case char7='K'
				printf 'Coventry'
			break
			case char7='L'
				printf 'Oxford'
			break
			case char7='M'
				if regyear>=1974
					printf 'Liverpool'
				else
					printf 'Southport'
				endif
			break
			case char7='N'
				printf 'Swansea'
			break
			case char7='O'
				if regyear>=1974
					printf 'Cardiff'
				else
					printf 'Monmouthshire'
				endif
			break
			case char7='P'
				printf 'Worcestershire'
			break
			case char7='R'
				if regyear>=1974
					printf 'Leeds'
				else
					printf 'Yorkshire (E.Riding)'
				endif
			break
			case char7='S'
				if regyear>=1981
					printf 'Bristol'
				else
					printf 'Edinburgh'
				endif
			break
			case char7='T'
				if regyear>=1974
					printf 'Leeds'
				else
					printf 'Yorkshire (W.Riding)'
				endif
			break
			case char7='U'
				if regyear>=1974
					printf 'Leeds'
				else
					printf 'Yorkshire (W.Riding)'
				endif
			break
			case char7='V'
				if regyear>=1974
					printf 'Brighton'
				else
					printf 'Wiltshire'
				endif
			break
			case char7='W'
				if regyear>=1974
					printf 'Brighton'
				else
					printf 'Yorkshire (W.Riding)'
				endif
			break
			case char7='X'
				if regyear>=1974
					printf 'Leeds'
				else
					printf 'Yorkshire (W.Riding)'
				endif
			break
			case char7='Y'
				if regyear>=1974
					printf 'Leeds'
				else
					printf 'Yorkshire (W.Riding)'
				endif
			break
		endswitch
	break
	case char6='X'
		switch
			whencase char7>='A' and char7<='F'
			whencase char7='G'
			whencase char7='H'
			whencase char7='I'
			whencase char7='J'
			whencase char7>='K' and char7<='R'
			whencase char7='S'
			whencase char7>='T' and char7<='Y'
			whencase char7=' '
			otherwise
				printf "%s",invalid
			break
			case char7=' '
				printf 'Northumberland'
			break
			case char7>='A' and char7<='F'
				printf 'London'
			break
			case char7='G'
				if regyear>=1974
					printf 'Teeside'
				else
					printf 'Middlesborough'
				endif
			break
			case char7='H'
				printf 'London'
			break
			case char7='I'
				printf 'Belfast'
			break
			case char7='J'
				printf 'Manchester'
			break
			case char7>='K' and char7<='R'
				printf 'London'
			break
			case char7='S'
				printf 'Paisley'
			break
			case char7>='T' and char7<='Y'
				printf 'London'
			break
		endswitch
	break
	case char6='Y'
		switch
			whencase char7>='A' and char7<='D'
			whencase char7>='E' and char7<='F'
			whencase char7='G'
			whencase char7='H'
			whencase char7='I'
			whencase char7='J'
			whencase char7>='K' and char7<='R'
			whencase char7='S'
			whencase char7>='T' and char7<='Y'
			whencase char7=' '
			otherwise
				printf "%s",invalid
			break
			case char7=' '
				printf 'Somersetshire'
			break
			case char7>='A' and char7<='D'
				if regyear>=1974
					printf 'Taunton'
				else
					printf 'Somersetshire'
				endif
			break
			case char7>='E' and char7<='F'
				printf 'London'
			break
			case char7='G'
				if regyear>=1974
					printf 'Leeds'
				else
					printf 'Yorkshire (W.Riding)'
				endif
			break
			case char7='H'
				printf 'London'
			break
			case char7='I'
				printf 'City and County of Dublin'
			break
			case char7='J'
				if regyear>=1974
					printf 'Brighton'
				else
					printf 'Dundee'
				endif
			break
			case char7>='K' and char7<='R'
				printf 'London'
			break
			case char7='S'
				printf 'Glasgow'
			break
			case char7>='T' and char7<='Y'
				printf 'London'
			break
		endswitch
	break
	case char6='Z'
		switch
			whencase char7='A'
			whencase char7='B'
			whencase char7>='C' and char7<='E'
			whencase char7='F'
			whencase char7>='H' and char7<='L'
			whencase char7='M'
			whencase char7='N'
			whencase char7='O'
			whencase char7='P'
			whencase char7='R'
			whencase char7='T'
			whencase char7='U'
			whencase char7='W'
			whencase char7='X'
			whencase char7='Z'
			whencase char7=' '
			otherwise
				printf "%s",invalid
			break
			case char7=' '
				printf 'City and County of Dublin'
			break
			case char7='A'
				printf 'City and County of Dublin'
			break
			case char7='B'
				printf 'Cork (County)'
			break
			case char7>='C' and char7<='E'
				printf 'City and County of Dublin'
			break
			case char7='F'
				printf 'Cork (County)'
			break
			case char7>='H' and char7<='L'
				printf 'City and County of Dublin'
			break
			case char7='M'
				printf 'Galway'
			break
			case char7='N'
				printf 'Meath'
			break
			case char7='O'
				printf 'City and County of Dublin'
			break
			case char7='P'
				printf 'Donegal'
			break
			case char7='R'
				printf 'Wexford'
			break
			case char7='T'
				printf 'Cork (County)'
			break
			case char7='U'
				printf 'City and County of Dublin'
			break
			case char7='W'
				printf 'Kildare'
			break
			case char7='X'
				printf 'Kerry'
			break
			case char7='Z'
				printf 'Irish International Circulation Mark'
			break
		endswitch
	break
endswitch
endif

printf "\n\n" ; make it look pretty
	defend
        qcall lib/exit,(0:-)
	ret

data
	invalid:
		dc.b 'Unrecognised or invalid',0
	version:
		dc.b 0,'$VER: npd 1.0 (09.09.2001)',0
toolend

.end

