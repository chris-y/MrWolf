.include 'tao'

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

.end