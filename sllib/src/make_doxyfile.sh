#!/bin/sh

rm -f Doxyfile
doxygen -g
cat Doxyfile | sed \
 -e 's/\(DOXYFILE_ENCODING[ ]*\)\([=]\)\([^=]*\)/\1\2 EUC-JP/' \
 -e 's/\(PROJECT_NAME[ ]*\)\([=]\)\([^=]*\)/\1\2 SLLIB/' \
 -e 's/\(OUTPUT_LANGUAGE[ ]*\)\([=]\)\([^=]*\)/\1\2 Japanese/' \
 -e 's/\(INPUT[ ]*\)\([=]\)\([^=]*\)/\1\2 . examples_sllib/' \
 -e 's/\(INPUT_ENCODING[ ]*\)\([=]\)\([^=]*\)/\1\2 EUC-JP/' \
 -e 's/\(EXAMPLE_PATH[ ]*\)\([=]\)\([^=]*\)/\1\2 examples_sllib/' \
 -e 's/\(EXAMPLE_PATTERNS[ ]*\)\([=]\)\([^=]*\)/\1\2 *.cc/' \
 -e 's/\(SOURCE_BROWSER[ ]*\)\([=]\)\([^=]*\)/\1\2 YES/' \
 -e 's/\(EXTRACT_ALL[ ]*\)\([=]\)\([^=]*\)/\1\2 YES/' \
 -e 's/\(EXTRACT_STATIC[ ]*\)\([=]\)\([^=]*\)/\1\2 YES/' \
 > _Doxyfile
rm Doxyfile
mv _Doxyfile Doxyfile

