#
# Primary
#
FMTTYPE  = 'ASTRO-X XXXX table format'
FTYPEVER = 101
EXTNAME  = 'Primary'
ORIGIN   = 'JAXA'
#
# 2nd HDU : Binary Table
#
XTENSION = BINTABLE
NAXIS2 = 24
PCOUNT = 65536
THEAP  = 32768
EXTNAME = XXXX_'TEST
#TXFLDKWD = 'TNOTE,TLMIN,TLMAX,TDMIN,TDMAX'
#TXFLDKWD = 'TDMAX'
 TTYPE#  = TIME
 TALAS#  = DATE
 TFORM#  = 1D
 TDESC#  = 'This is time' / description of this field
 TTYPE#  = COUNTER
 TFORM#  = 8J
 TLMIN#  = 1
 TLMAX#  = 16777216
 TDMIN#  = 0
 TDMAX#  = 0
 TTYPE#  = XNAME
 TFORM#  = 16A
 TTYPE#  = VLA
 TFORM#  = 1PJ(0)
 TNOTE#  = 'You can define variable length array in SFITSIO template&'
 CONTINUE  '' / annotation of this field
