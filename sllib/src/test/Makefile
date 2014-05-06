
CCFLAGS  =

# DO NOT CHANGE
ALL_CCFLAGS = -O2 -Wall $(CCFLAGS)
IINC     = -I.
LLIB     = -L.

.cc.o:   ; sh s++ $(ALL_CCFLAGS) $(IINC) -c $*.cc

all::	s++ super_cat super_gzbz split \
	quot_chars complex_array mdarray_performance cmath_test

s++:
	rm -f sli
	ln -s .. sli
	rm -f libsllib.a
	if [ -f ../libsllib.a ]; then ln -s ../libsllib.a . ; fi
	if [ -f ../.libs/libsllib.a ]; then ln -s ../.libs/libsllib.a . ; fi
	cat ../s++ | sed -e 's|^XARG_IINC=[^=]*|XARG_IINC=|' \
           -e 's|^XARG_LLIB=[^=]*|XARG_LLIB=|' > s++.sh
	rm -f s++
	ln -s s++.sh s++

super_cat:	s++ super_cat.o
	sh s++ $(CPPC) $(LLIB) -o $@ $@.o libsllib.a

super_gzbz:	s++ super_gzbz.o
	sh s++ $(CPPC) $(LLIB) -o $@ $@.o libsllib.a

split:	s++ split.o
	sh s++ $(CPPC) $(LLIB) -o $@ $@.o libsllib.a

quot_chars:	s++ quot_chars.o
	sh s++ $(CPPC) $(LLIB) -o $@ $@.o libsllib.a

complex_array:	s++ complex_array.o
	sh s++ $(CPPC) $(LLIB) -o $@ $@.o libsllib.a

mdarray_performance:	s++ mdarray_performance.o
	sh s++ $(CPPC) $(LLIB) -o $@ $@.o libsllib.a

cmath_test:	s++ cmath_test.o
	sh s++ $(CPPC) $(LLIB) -o $@ $@.o libsllib.a

clean::
	rm -f *.o *.exe *.a s++ s++.sh sli super_cat super_gzbz \
	      split quot_chars complex_array \
	      mdarray_performance cmath_test
