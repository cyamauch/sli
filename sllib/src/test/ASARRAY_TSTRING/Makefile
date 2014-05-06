
CCFLAGS  =

# DO NOT CHANGE
ALL_CCFLAGS = -O -Wall $(CCFLAGS)
IINC     = -I..
LLIB     = -L../..

.cc.o:   ; sh ../../s++.sh $(ALL_CCFLAGS) $(IINC) -c $*.cc

all::	01_opeEqu_sample    011_opeElem_sample  02_length_sample       03_cstrarray_sample    04_cstr_sample     05_at_sample
	06_init_sample      07_clean_sample     08_split_keys_sample   09_assign_sample       10_assign_sample
	11_append_sample    12_append_sample    13_insert_sample       14_insert_sample       15_erase_sample
	16_swap_sample      17_index_sample     18_key_sample

01_opeEqu_sample:	01_opeEqu_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libslib.a

011_opeElem_sample:	011_opeElem_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libslib.a

02_length_sample:	02_length_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libslib.a

03_cstrarray_sample:	03_cstrarray_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libslib.a

04_cstr_sample:		04_cstr_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libslib.a

05_at_sample:		05_at_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libslib.a

06_init_sample:		06_init_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libslib.a

07_clean_sample:	07_clean_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libslib.a

08_split_keys_sample:	08_split_keys_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libslib.a

09_assign_sample:	09_assign_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libslib.a

10_assign_sample:	10_assign_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libslib.a

11_append_sample:	11_append_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libslib.a

12_append_sample:	12_append_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libslib.a

13_insert_sample:	13_insert_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libslib.a

14_insert_sample:	14_insert_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libslib.a

15_erase_sample:	15_erase_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libslib.a

16_swap_sample:		16_swap_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libslib.a

17_index_sample:	17_index_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libslib.a

18_key_sample:		18_key_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libslib.a


clean::
	rm -f *.o *.exe 01_opeEqu_sample     011_opeElem_sample  02_length_sample        03_cstrarray_sample   04_cstr_sample     05_at_sample
			06_init_sample       07_clean_sample     08_split_keys_sample    09_assign_sample      10_assign_sample
			11_append_sample     12_append_sample    13_insert_sample        14_insert_sample      15_erase_sample
			16_swap_sample       17_index_sample     18_key_sample
