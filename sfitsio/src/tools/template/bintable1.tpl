#
# Primary
#
FMTTYPE  = 'ASTRO-X event table'
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
EXTNAME = XXXX_TEST
EXTVER   =
EXTLEVEL =
TXFLDKWD = 'TNOTE,TLMIN,TLMAX,TDMIN,TDMAX'
 TTYPE#  = TIME
 TALAS#  = DATE
 TFORM#  = 1D
 TDESC#  = This is JAXA's time / description of this field
 TTYPE#  = COUNTER
 TFORM#  = 8J
 TDESC#  = 'This is 'J' counter' / description of this field
 TLMIN#  = 1
 TLMAX#  = 16777216
 TDMIN#  = 0
 TDMAX#  = 0
 TTYPE#  = XNAME
 TFORM#  = 16A
 TNULL#  = ' a'
 TTYPE#  = VLA
 TFORM#  = 1PJ(0)
 TNOTE#  = 'You can define variable length array in SFITSIO template&'
 CONTINUE  '' / annotation of this field
