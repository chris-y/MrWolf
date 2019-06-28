; Usage:  toolup  name, head, code
.macro toolup
	.check	%N == 3
	qcall	sys/kn/tool/open, (%1, NULL.p : %2, %3)
.endm

; Usage:  gos  code, (args:args)

; Usage:  tooldown  head
.macro tooldown
	.check	%N == 1
	qcall	sys/kn/tool/deref, (%1:-)
.endm


; This one macro combines all of the above functionality.

; Usage:  tcall  name
; Usage:  tcall  name, (args:args)
; Usage:  tcall  name, (args:args), success
; Example:  tcall  toolname.p, (p0, p1, i0 : i4), tool_was_found
; NOTE:  the name follows the same rules as qcall's first argument:
;          1) full path
;          2) no leading slash
;          3) no trailing processor type (ie, '.00')
;        it will fail if any of these are not followed,
;        or if the tool is otherwise not found.
._qmacro tcall
	.check	%N >= 1

	.if __def_level < 1
		.error	tcall must be within defbegin block.
	.else
	
	defbegin
	defp	tcall_head, tcall_code
	
	qcall	sys/kn/tool/open, (%1, NULL.p : tcall_head, tcall_code)
	if	tcall_head == NULL
		.if	%N > 2
			cpy	0, %3
		.endif
	else
	.if	%N == 1
		gos	tcall_code, (-:-)
	.elseif	%N == 2
		gos	tcall_code, %2
	.elseif	%N == 3
		gos	tcall_code, %2
		cpy	-1, %3
	.endif
		qcall	sys/kn/tool/deref, (tcall_head:-)
	endif
	defend

	.endif
.endm

	
