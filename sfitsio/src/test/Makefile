
CCFLAGS  =

# DO NOT CHANGE
ALL_CCFLAGS  = -O2 -Wall $(CCFLAGS) -DBUILD_SFITSIO
SLIINCDIR = /usr/local/include
IINC      = -I. -I.. -I$(SLIINCDIR)
LLIB      = -L..

.cc.o:   ; sh s++.sh $(ALL_CCFLAGS) $(IINC) -c $*.cc

all::	s++.sh libsfitsio.a \
	continue_header create_from_template read_header_only access_bte_heap \
	edit_bintable tzerotscale_bintable rename_header \
	image_copy_paste

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

continue_header:	continue_header.o
	sh s++.sh $(CPPC) $(LLIB) -o $@ $@.o libsfitsio.a

create_from_template:		create_from_template.o
	sh s++.sh $(CPPC) $(LLIB) -o $@ $@.o libsfitsio.a

read_header_only:	read_header_only.o
	sh s++.sh $(CPPC) $(LLIB) -o $@ $@.o libsfitsio.a

access_bte_heap:	access_bte_heap.o
	sh s++.sh $(CPPC) $(LLIB) -o $@ $@.o libsfitsio.a

edit_bintable:		edit_bintable.o
	sh s++.sh $(CPPC) $(LLIB) -o $@ $@.o libsfitsio.a

tzerotscale_bintable:	tzerotscale_bintable.o
	sh s++.sh $(CPPC) $(LLIB) -o $@ $@.o libsfitsio.a

rename_header:	rename_header.o
	sh s++.sh $(CPPC) $(LLIB) -o $@ $@.o libsfitsio.a

image_copy_paste:	image_copy_paste.o
	sh s++.sh $(CPPC) $(LLIB) -o $@ $@.o libsfitsio.a

clean::
	rm -f *.o *.exe continue_header create_from_template read_header_only \
			access_bte_heap edit_bintable tzerotscale_bintable \
			rename_header image_copy_paste
	rm -f s++.sh *.a sli
