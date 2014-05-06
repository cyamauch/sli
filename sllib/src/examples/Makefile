
CCFLAGS  =

# DO NOT CHANGE
ALL_CCFLAGS = -O2 -Wall $(CCFLAGS)
IINC     = -I.
LLIB     = -L.

.cc.o:   ; sh s++ $(ALL_CCFLAGS) $(IINC) -c $*.cc

all::	s++ \
	      stdout_stderr read_local_text_file read_text_from_net readline \
	      gnuplot_animation http_client \
	      string_basic string_edit \
	      string_regexp string_match string_bracket verbose_grep \
	      string_array_basic regexp_back_reference split_string \
	      associative_string_array \
	      array_basic array_edit array_fast_access array_math array_idl \
	      array_statistics array_digest

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

stdout_stderr:	s++ stdout_stderr.o
	sh s++ $(CPPC) $(LLIB) -o $@ $@.o libsllib.a

read_local_text_file:	s++ read_local_text_file.o
	sh s++ $(CPPC) $(LLIB) -o $@ $@.o libsllib.a

read_text_from_net:	s++ read_text_from_net.o
	sh s++ $(CPPC) $(LLIB) -o $@ $@.o libsllib.a

readline:	s++ readline.o
	sh s++ $(CPPC) $(LLIB) -o $@ $@.o libsllib.a

gnuplot_animation:	s++ gnuplot_animation.o
	sh s++ $(CPPC) $(LLIB) -o $@ $@.o libsllib.a

http_client:		s++ http_client.o
	sh s++ $(CPPC) $(LLIB) -o $@ $@.o libsllib.a

string_basic:	s++ string_basic.o
	sh s++ $(CPPC) $(LLIB) -o $@ $@.o libsllib.a

string_edit:	s++ string_edit.o
	sh s++ $(CPPC) $(LLIB) -o $@ $@.o libsllib.a

string_regexp:	s++ string_regexp.o
	sh s++ $(CPPC) $(LLIB) -o $@ $@.o libsllib.a

string_match:	s++ string_match.o
	sh s++ $(CPPC) $(LLIB) -o $@ $@.o libsllib.a

string_bracket:	s++ string_bracket.o
	sh s++ $(CPPC) $(LLIB) -o $@ $@.o libsllib.a

verbose_grep:	s++ verbose_grep.o
	sh s++ $(CPPC) $(LLIB) -o $@ $@.o libsllib.a

string_array_basic:	s++ string_array_basic.o
	sh s++ $(CPPC) $(LLIB) -o $@ $@.o libsllib.a

regexp_back_reference:	s++ regexp_back_reference.o
	sh s++ $(CPPC) $(LLIB) -o $@ $@.o libsllib.a

split_string:	s++ split_string.o
	sh s++ $(CPPC) $(LLIB) -o $@ $@.o libsllib.a

associative_string_array:	s++ associative_string_array.o
	sh s++ $(CPPC) $(LLIB) -o $@ $@.o libsllib.a

array_basic:	s++ array_basic.o
	sh s++ $(CPPC) $(LLIB) -o $@ $@.o libsllib.a

array_edit:	s++ array_edit.o
	sh s++ $(CPPC) $(LLIB) -o $@ $@.o libsllib.a

array_fast_access:	s++ array_fast_access.o
	sh s++ $(CPPC) $(LLIB) -o $@ $@.o libsllib.a

array_math:	s++ array_math.o
	sh s++ $(CPPC) $(LLIB) -o $@ $@.o libsllib.a

array_idl:	s++ array_idl.o
	sh s++ $(CPPC) $(LLIB) -o $@ $@.o libsllib.a

array_statistics:	s++ array_statistics.o
	sh s++ $(CPPC) $(LLIB) -o $@ $@.o libsllib.a

array_digest:	s++ array_digest.o
	sh s++ $(CPPC) $(LLIB) -o $@ $@.o libsllib.a

clean::
	rm -f *.o *.exe *.a s++ s++.sh sli \
	      stdout_stderr read_local_text_file read_text_from_net readline \
	      gnuplot_animation http_client \
	      string_basic string_edit \
	      string_regexp string_match string_bracket verbose_grep \
	      string_array_basic regexp_back_reference split_string \
	      associative_string_array \
	      array_basic array_edit array_fast_access array_math array_idl \
	      array_statistics array_digest
