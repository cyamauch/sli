
CCFLAGS  =

# DO NOT CHANGE
ALL_CCFLAGS  = -O2 -Wall $(CCFLAGS) -DBUILD_SFITSIO
SLIINCDIR = /usr/local/include
IINC      = -I. -I.. -I$(SLIINCDIR)
LLIB      = -L..

.cc.o:   ; sh s++.sh $(ALL_CCFLAGS) $(IINC) -c $*.cc

all::	s++.sh libsfitsio.a \
		read_fits_header read_and_write_fits \
		create_fits_image create_fits_image_and_header \
		stat_fits_image_pixels_md stat_fits_image_pixels \
		combine_fits_images_md combine_fits_images \
		dump_fits_table create_fits_bintable create_fits_asciitable \
	        create_fits_vl_array

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

read_fits_header:	read_fits_header.o
	sh s++.sh $(CPPC) $(LLIB) -o $@ $@.o libsfitsio.a

read_and_write_fits:	read_and_write_fits.o
	sh s++.sh $(CPPC) $(LLIB) -o $@ $@.o libsfitsio.a

create_fits_image:	create_fits_image.o
	sh s++.sh $(CPPC) $(LLIB) -o $@ $@.o libsfitsio.a

create_fits_image_and_header:	create_fits_image_and_header.o
	sh s++.sh $(CPPC) $(LLIB) -o $@ $@.o libsfitsio.a

stat_fits_image_pixels_md:	stat_fits_image_pixels_md.o
	sh s++.sh $(CPPC) $(LLIB) -o $@ $@.o libsfitsio.a

stat_fits_image_pixels:	stat_fits_image_pixels.o
	sh s++.sh $(CPPC) $(LLIB) -o $@ $@.o libsfitsio.a

combine_fits_images_md:	combine_fits_images_md.o
	sh s++.sh $(CPPC) $(LLIB) -o $@ $@.o libsfitsio.a

combine_fits_images:	combine_fits_images.o
	sh s++.sh $(CPPC) $(LLIB) -o $@ $@.o libsfitsio.a

dump_fits_table:	dump_fits_table.o
	sh s++.sh $(CPPC) $(LLIB) -o $@ $@.o libsfitsio.a

create_fits_bintable:	create_fits_bintable.o
	sh s++.sh $(CPPC) $(LLIB) -o $@ $@.o libsfitsio.a

create_fits_asciitable:	create_fits_asciitable.o
	sh s++.sh $(CPPC) $(LLIB) -o $@ $@.o libsfitsio.a

create_fits_vl_array:	create_fits_vl_array.o
	sh s++.sh $(CPPC) $(LLIB) -o $@ $@.o libsfitsio.a


clean::
	rm -f *.o *.exe \
		read_fits_header read_and_write_fits \
		create_fits_image create_fits_image_and_header \
		stat_fits_image_pixels_md stat_fits_image_pixels \
		combine_fits_images_md combine_fits_images \
		dump_fits_table create_fits_bintable create_fits_asciitable \
	        create_fits_vl_array
	rm -f s++.sh *.a sli
