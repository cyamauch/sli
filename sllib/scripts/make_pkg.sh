#!/bin/sh

PKGNAME="sllib"
if [ ! "$1" = "" ]; then
  PKGNAME="${PKGNAME}-$1"
fi
rm -rf $PKGNAME
rm -f ${PKGNAME}.tar.gz

tar cf - -C .. --exclude obsolete --exclude .svn src | tar xf -
mv src $PKGNAME

rm -f $PKGNAME/sli
make -C $PKGNAME clean
rm -f $PKGNAME/*~
rm -f $PKGNAME/.[a-z]*
rm -f $PKGNAME/*.orig
rm -f $PKGNAME/private/*~
rm -f $PKGNAME/private/*.orig
rm -f $PKGNAME/private/.[a-z]*
rm -rf $PKGNAME/private/old
rm -f $PKGNAME/test/*~
rm -f $PKGNAME/test/*.orig
rm -f $PKGNAME/test/.[a-z]*
rm -f $PKGNAME/examples/*~
rm -f $PKGNAME/examples/*.orig
rm -f $PKGNAME/examples/.[a-z]*
rm -rf $PKGNAME/_tst
chmod 755 $PKGNAME/configure
( cd $PKGNAME ; mv examples examples_sllib )
( cd $PKGNAME ; ln -s examples_sllib examples )

tar cvf ${PKGNAME}.tar $PKGNAME
gzip ${PKGNAME}.tar
