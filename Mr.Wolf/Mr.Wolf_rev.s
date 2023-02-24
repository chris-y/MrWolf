VERSION = 2
REVISION = 2

.macro DATE
.ascii "24.2.2023"
.endm

.macro VERS
.ascii "Mr.Wolf 2.2"
.endm

.macro VSTRING
.ascii "Mr.Wolf 2.2 (24.2.2023)"
.byte 13,10,0
.endm

.macro VERSTAG
.byte 0
.ascii "$VER: Mr.Wolf 2.2 (24.2.2023)"
.byte 0
.endm
