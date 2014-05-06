
CCFLAGS  =

# DO NOT CHANGE
ALL_CCFLAGS  = -O2 -Wall $(CCFLAGS) -DBUILD_SFITSIO
SLIINCDIR = /usr/local/include
IINC      = -I. -I.. -I$(SLIINCDIR)
LLIB      = -L..

.cc.o:   ; sh s++.sh $(ALL_CCFLAGS) $(IINC) -c $*.cc

all::	s++.sh libsfitsio.a \
	conv_fits_image_bitpix fill_fits_header_comments hv \
	create_fits_from_template \
	fits_dataunit_md5 transpose_fits_image rotate_fits_image

s++.sh:
	if [ ! -f $(SLIINCDIR)/sli/cstreamio.h -a ! -d ../sli ]; then \
	  echo "[INFO] If ERROR is reported, edit Makefile and set SLIINCDIR variable";\
	  exit -1; \
	fi
	rm -f s++.sh
	ln -s ../s++.sh .

libsfitsio.a:
	rm -f sli
	ln -s .. sli
	rm -f libsfitsio.a
	if [ -f ../libsfitsio.a ]; then ln -s ../libsfitsio.a . ; fi
	if [ -f ../.libs/libsfitsio.a ]; then ln -s ../.libs/libsfitsio.a . ; fi

conv_fits_image_bitpix:	conv_fits_image_bitpix.o
	sh s++.sh $(CPPC) $(LLIB) -o $@ $@.o libsfitsio.a

fill_fits_header_comments:	fill_fits_header_comments.o
	sh s++.sh $(CPPC) $(LLIB) -o $@ $@.o libsfitsio.a

create_fits_from_template:	create_fits_from_template.o
	sh s++.sh $(CPPC) $(LLIB) -o $@ $@.o libsfitsio.a

hv:	hv.o
	sh s++.sh $(CPPC) $(LLIB) -o $@ $@.o libsfitsio.a

fits_dataunit_md5:	fits_dataunit_md5.o
	sh s++.sh $(CPPC) $(LLIB) -o $@ $@.o libsfitsio.a

transpose_fits_image:	transpose_fits_image.o
	sh s++.sh $(CPPC) $(LLIB) -o $@ $@.o libsfitsio.a

rotate_fits_image:		rotate_fits_image.o
	sh s++.sh $(CPPC) $(LLIB) -o $@ $@.o libsfitsio.a


clean::
	rm -f s++.sh *.a sli
	rm -f *.o *.exe \
	conv_fits_image_bitpix fill_fits_header_comments hv \
	create_fits_from_template \
	fits_dataunit_md5 transpose_fits_image rotate_fits_image
