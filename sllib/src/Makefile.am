#SUBDIRS = sample
#ACLOCAL_AMFLAGS = -I m4

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = sllib.pc

docdir = $(datadir)/doc/@PACKAGE@

#bin_PROGRAMS=

lib_LTLIBRARIES = libsllib.la
libsllib_la_LDFLAGS = -version-info 4:0
libsllib_la_SOURCES = \
	cstreamio.cc stdstreamio.cc gzstreamio.cc bzstreamio.cc \
	pipestreamio.cc termlineio.cc termscreenio.cc inetstreamio.cc \
	httpstreamio.cc ftpstreamio.cc digeststreamio.cc \
	tregex.cc tstring.cc tarray_tstring.cc asarray_tstring.cc \
	mdarray.cc mdarray_float.cc mdarray_double.cc \
	mdarray_uchar.cc mdarray_short.cc mdarray_int.cc mdarray_long.cc \
	mdarray_llong.cc mdarray_int16.cc mdarray_int32.cc \
	mdarray_int64.cc mdarray_size.cc mdarray_ssize.cc \
	mdarray_bool.cc mdarray_uintptr.cc mdarray_fcomplex.cc \
	mdarray_dcomplex.cc mdarray_math.cc mdarray_complex.cc \
	complex.cc xmlparser.cc ctindex.cc ldsrpc.cc sli_funcs.cc \
	asarray.h \
	asarray_tstring.h \
	bzstreamio.h \
	complex_defs.h \
	complex.h \
	config.h \
	cstreamio.h \
	ctindex.h \
	digeststreamio.h \
	ftpstreamio.h \
	gzstreamio.h \
	heap_mem.h \
	httpstreamio.h \
	inetstreamio.h \
	ldsrpc.h \
	mdarray.h \
	mdarray_bool.h \
	mdarray_double.h \
	mdarray_float.h \
	mdarray_int.h \
	mdarray_int16.h \
	mdarray_int32.h \
	mdarray_int64.h \
	mdarray_llong.h \
	mdarray_long.h \
	mdarray_math.h \
	mdarray_complex.h \
	mdarray_short.h \
	mdarray_size.h \
	mdarray_ssize.h \
	mdarray_uchar.h \
	mdarray_uintptr.h \
	mdarray_fcomplex.h \
	mdarray_dcomplex.h \
	mdarray_statistics.h \
	numeric_indefs.h \
	numeric_minmax.h \
	pipestreamio.h \
	size_types.h \
	sli_config.h \
	sli_eof.h \
	sli_funcs.h \
	sli_seek.h \
	slierr.h \
	stdstreamio.h \
	tarray.h \
	tarray_tstring.h \
	termlineio.h \
	termscreenio.h \
	tregex.h \
	tstring.h \
	xmlparser.h

pkgincludedir = $(includedir)/sli
pkginclude_HEADERS = \
	asarray.h \
	asarray_tstring.h \
	bzstreamio.h \
	complex_defs.h \
	complex.h \
	cstreamio.h \
	ctindex.h \
	digeststreamio.h \
	ftpstreamio.h \
	gzstreamio.h \
	heap_mem.h \
	httpstreamio.h \
	inetstreamio.h \
	ldsrpc.h \
	mdarray.h \
	mdarray_bool.h \
	mdarray_double.h \
	mdarray_float.h \
	mdarray_int.h \
	mdarray_int16.h \
	mdarray_int32.h \
	mdarray_int64.h \
	mdarray_llong.h \
	mdarray_long.h \
	mdarray_math.h \
	mdarray_complex.h \
	mdarray_short.h \
	mdarray_size.h \
	mdarray_ssize.h \
	mdarray_uchar.h \
	mdarray_uintptr.h \
	mdarray_fcomplex.h \
	mdarray_dcomplex.h \
	mdarray_statistics.h \
	numeric_indefs.h \
	numeric_minmax.h \
	pipestreamio.h \
	size_types.h \
	sli_config.h \
	sli_eof.h \
	sli_funcs.h \
	sli_seek.h \
	slierr.h \
	stdstreamio.h \
	tarray.h \
	tarray_tstring.h \
	termlineio.h \
	termscreenio.h \
	tregex.h \
	tstring.h \
	xmlparser.h

nobase_dist_doc_DATA = README \
	LICENSE LICENSE.GPL LICENSE.MIT \
	examples/array_basic.cc examples/array_digest.cc \
	examples/array_edit.cc examples/array_edit.cc \
	examples/array_fast_access.cc examples/array_idl.cc \
	examples/array_math.cc examples/array_statistics.cc \
	examples/associative_string_array.cc \
	examples/gnuplot_animation.cc examples/stdout_stderr.cc \
	examples/read_local_text_file.cc examples/read_text_from_net.cc \
	examples/readline.cc \
	examples/regexp_back_reference.cc examples/split_string.cc \
	examples/string_array_basic.cc examples/string_basic.cc \
	examples/string_bracket.cc examples/string_edit.cc \
	examples/string_match.cc examples/string_regexp.cc \
	examples/verbose_grep.cc examples/http_client.cc

EXTRA_DIST = private

bin_SCRIPTS = s++
CLEANFILES = $(bin_SCRIPTS)

do_filter = sed -e 's|^CPPC=[^=]*|CPPC='"$(CXX)"'|' \
	   -e 's|^DOCDIR=|DOCDIR=\"'"$(docdir)"'\"|' \
	   -e 's|^XARG_IINC=|XARG_IINC=-I'"$(includedir)"'|' \
           -e 's|^XARG_LIBSLLIB=|XARG_LIBSLLIB=-lsllib|' \
           -e 's|^XARG_LIBPRIVATE=|XARG_LIBPRIVATE="$(LIBS)"|' \
           -e 's|^XARG_LLIB=|XARG_LLIB=\"-L'"$(libdir)"'\"|'

s++: s++.src
	$(do_filter) < s++.src > s++
	chmod 755 s++

