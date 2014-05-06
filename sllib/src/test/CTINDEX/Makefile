
CCFLAGS  =

# DO NOT CHANGE
ALL_CCFLAGS = -O -Wall $(CCFLAGS)
IINC     = -I..
LLIB     = -L../..

.cc.o:   ; sh ../../s++.sh $(ALL_CCFLAGS) $(IINC) -c $*.cc

all::	01_opeEqu_sample 02_init_sample  03_append_sample 04_update_sample 05_erase_sample 06_index_sample

test::
	sh ./do_test.sh > test_result.txt
	diff -C 6 test_result_ok.txt test_result.txt

01_opeEqu_sample:	01_opeEqu_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

02_init_sample:		02_init_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

03_append_sample:	03_append_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

04_update_sample:	04_update_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

05_erase_sample:	05_erase_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

06_index_sample:	06_index_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

clean::
	rm -f *.o *.exe 01_opeEqu_sample 02_init_sample 03_append_sample 04_update_sample 05_erase_sample 06_index_sample

