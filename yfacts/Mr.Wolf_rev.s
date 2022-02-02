VERSION = 2
REVISION = 1

.macro DATE
.ascii "2.2.2022"
.endm

.macro VERS
.ascii "Mr.Wolf 2.1"
.endm

.macro VSTRING
.ascii "Mr.Wolf 2.1 (2.2.2022)"
.byte 13,10,0
.endm

.macro VERSTAG
.byte 0
.ascii "$VER: Mr.Wolf 2.1 (2.2.2022)"
.byte 0
.endm
