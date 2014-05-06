#SUBDIRS = sample
#ACLOCAL_AMFLAGS = -I m4

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = sfitsio.pc

docdir = $(datadir)/doc/@PACKAGE@

#bin_PROGRAMS=

lib_LTLIBRARIES = libsfitsio.la
libsfitsio_la_LDFLAGS = -version-info 3:0
libsfitsio_la_SOURCES = \
	fits_header_record.cc fits_header.cc fits_hdu.cc \
	fits_table_col.cc fits_table.cc fits_image.cc fitscc.cc \
	fits_hdu.h \
	fits_header.h \
	fits_header_record.h \
	fits_table.h \
	fitscc.h \
	fits_image.h \
	fits.h \
	fits_table_col.h

pkgincludedir = $(includedir)/sli
pkginclude_HEADERS = \
	fits_hdu.h \
	fits_header.h \
	fits_header_record.h \
	fits_table.h \
	fitscc.h \
	fits_image.h \
	fits.h \
	fits_table_col.h

nobase_dist_doc_DATA = README \
	examples/combine_fits_images_md.cc \
	examples/create_fits_asciitable.cc \
	examples/create_fits_bintable.cc \
	examples/create_fits_image.cc \
	examples/create_fits_image_and_header.cc \
	examples/create_fits_vl_array.cc \
	examples/dump_fits_table.cc \
	examples/read_and_write_fits.cc \
	examples/read_fits_header.cc \
	examples/stat_fits_image_pixels_md.cc \
	tools/conv_fits_image_bitpix.cc \
	tools/create_fits_from_template.cc \
	tools/fill_fits_header_comments.cc \
	tools/fits_dataunit_md5.cc \
	tools/hv.cc \
	tools/rotate_fits_image.cc \
	tools/transpose_fits_image.cc

EXTRA_DIST = private

noinst_SCRIPTS = s++.sh
CLEANFILES = $(noinst_SCRIPTS)

s++.sh:
	echo '#!/bin/sh__exec s++ $$@' | tr '_' '\n' > s++.sh
