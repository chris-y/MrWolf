VERSION = 1
REVISION = 5

.macro DATE
.ascii "16.3.2013"
.endm

.macro VERS
.ascii "yFacts 1.5"
.endm

.macro VSTRING
.ascii "yFacts 1.5 (16.3.2013)"
.byte 13,10,0
.endm

.macro VERSTAG
.byte 0
.ascii "$VER: yFacts 1.5 (16.3.2013)"
.byte 0
.endm
