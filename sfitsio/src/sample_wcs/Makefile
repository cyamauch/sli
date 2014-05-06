
CCFLAGS  =

WCSTOOLS = wcstools-3.8.1

# DO NOT CHANGE
ALL_CCFLAGS  = -O2 -Wall $(CCFLAGS) -DBUILD_SFITSIO
SLIINCDIR = /usr/local/include
IINC      = -I./$(WCSTOOLS) -I. -I.. -I$(SLIINCDIR)
LLIB      = -L..

.cc.o:   ; sh s++.sh $(ALL_CCFLAGS) $(IINC) -c $*.cc

all::	s++.sh libsfitsio.a libwcs.a \
	get_region_info wcs_test plot_starlist

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

libwcs.a::
	gzip -dc $(WCSTOOLS).tar.gz | tar xf -
	( cd $(WCSTOOLS)/libwcs ; make CC=gcc CFLAGS=-O )
	rm -f libwcs.a
	ln -s $(WCSTOOLS)/libwcs/libwcs.a .

get_region_info:	get_region_info.o
	sh s++.sh $(CPPC) $(LLIB) -o $@ $@.o libwcs.a libsfitsio.a

wcs_test:		wcs_test.o
	sh s++.sh $(CPPC) $(LLIB) -o $@ $@.o libwcs.a libsfitsio.a

plot_starlist:		plot_starlist.o
	sh s++.sh $(CPPC) $(LLIB) -o $@ $@.o libwcs.a libsfitsio.a

clean::
	rm -rf libwcs.a $(WCSTOOLS)
	rm -f *.o *.exe get_region_info wcs_test plot_starlist
	rm -f s++.sh *.a sli
