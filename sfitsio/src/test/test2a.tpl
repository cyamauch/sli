
# Test for Binary table

fmttype = 'JAXA	xxxx table format'
origin  = 'JAXA'
smplbase = 2005-01-01T00:00:00

comment

message = 'FITS (Flexible Image Transport System) format is defined in "Astronomy and Astrophysics", volume 376, page 359; bibcode: 2001A&A...376..359H' / In SFITSIO, this message is not written automatically.

comment this is Primary HDU.

#

xtension = bintable

 extname = mytable
 extver  = 101

 naxis2 = 24
 pcount = 65536
 theap = 32768

	txfldkwd = 'tdesc,tnote,tlmin,tlmax,tdmin,tdmax,ttnam'

	ttype#  = TIME
	talas#  = DATE
	tform#  = 1d
	tdesc#  = 'This is time'
	ttnam#  = @TI

	ttype#  = STATUS
	tdmax#  = 0
	tdmin#  = 0
	tlmax#  = 16777216
	tlmin#  = 1
	tform#  = 8j

	ttype#  = XNAME
	tform#  = 16A

	ttype#  = TRIG
	tform#  = 8x
	tdesc#  = 'Trigger type flag,\n&'
	continue  '  b1000000:SUD(trigd by SuperUpper Discriminator),\n&'
	continue  '  b0100000:ANODE,\n&'
	continue  '  b0010000:PIN0 b0001000:PIN1 b0000100:PIN2,\n&'
	continue  '  b0000010:PIN3 b0000001:PSEUDO' / description of this column
	tnote#  = 'SFITSIO splits a string value into some header records by "\" + "n".  This improves readability of FITS header and is useful to convert the string into HTML, LaTeX, etc. ' / annotation

	ttype#  = PHASE
	tform#  = 1m

	ttype#  = VLA
	tform#  = 1pj(0)
	tnote#  = 'You can define variable length array in SFITSIO template'

	checksum = ''
	datasum  = ''

 comment this is 2nd HDU.

