#!/bin/sh

#           #
# FUNCTIONS #
#           #
is_cc_file () {
  if [ "`echo $1 | egrep -i '\.cc$|\.cpp$'`" = "" ]; then
    return 1
  else
    return 0
  fi
}

#
# creating default config.h
#
V=`cat configure.ac | egrep '^AC_INIT\(' | sed -e 's/[^(]*\((\)\([^()]*\)\()\)/\2/g' | tr ',' ' '`
ARG=`echo $V | awk '{printf("-e s/@NAME@/%s/ -e s/@VER@/%s/ -e s/@MAIL@/%s/\n",$1,$2,$3);}'`
cat config.h.src | sed $ARG > config.h

#
# creating default Makefile
#
V=`cat Makefile.am | tr '\n' '@'`
L=`echo $V | sed -e 's/\\\\@//g' | tr '@' '\n' | egrep '^lib[^_]*_la_SOURCES[^a-zA-Z0-9_]' | sed -e 's/.*=//'`
LIST0=""
LIST1=""
for i in $L ; do
  if is_cc_file $i ; then
    V=`echo $i | sed -e 's/\.cc$/.o/'`
    # ~ => \t  % => \n
    LIST0="${LIST0}$V:~s++.sh%%"
    LIST1="${LIST1} $V"
  fi
done
cat Makefile.src | sed -e "s/@OLIST0@/$LIST0/g" -e "s/@OLIST1@/$LIST1/g" | tr '~' '\t' | tr '%' '\n' > Makefile

#
# setup configure
#

rm -f aclocal.m4
rm -f ltmain.sh
rm -f config.sub
rm -f config.guess
rm -f depcomp
rm -f missing
rm -f install-sh
rm -f libtool

rm -f config.h.in
rm -rf auto*.cache
rm -f stamp-*

rm -f configure
rm -f *.pc
rm -f Makefile.in
rm -f config.status

#rm -f config.h
#rm -f Makefile

aclocal

libtoolize

aclocal

autoheader

automake -a

autoconf

rm -rf auto*.cache
rm -f stamp-*

LIST="ltmain.sh config.sub config.guess missing install-sh depcomp"
for i in $LIST ; do
  if [ -h $i ]; then
    mv $i _$i
    cp _$i $i
    rm -f _$i
  fi
done

