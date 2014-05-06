#!/bin/sh

PKGNAME="sfitsio"
if [ ! "$1" = "" ]; then
  PKGNAME="${PKGNAME}-$1"
fi
rm -rf $PKGNAME
rm -f ${PKGNAME}.tar.gz

tar cf - -C .. --exclude .svn src | tar xf -
mv src $PKGNAME

rm -f $PKGNAME/sli
make -C $PKGNAME clean
rm -f $PKGNAME/*~
rm -f $PKGNAME/.[a-z]*
rm -f $PKGNAME/*.orig
rm -f $PKGNAME/private/*~
rm -f $PKGNAME/private/*.orig
rm -f $PKGNAME/private/.[a-z]*
rm -f $PKGNAME/examples/*~
rm -f $PKGNAME/examples/*.orig
rm -f $PKGNAME/examples/.[a-z]*
rm -f $PKGNAME/examples/*.fits.bz2
rm -f $PKGNAME/examples/*.fits.gz
rm -f $PKGNAME/examples/*.fits
rm -rf $PKGNAME/obsolete
rm -rf $PKGNAME/tools/obsolete
rm -rf $PKGNAME/test/obsolete
rm -rf $PKGNAME/_tst
chmod 755 $PKGNAME/configure
( cd $PKGNAME ; mv examples examples_sfitsio )
( cd $PKGNAME ; ln -s examples_sfitsio examples )
( cd $PKGNAME ; mv tools tools_sfitsio )
( cd $PKGNAME ; ln -s tools_sfitsio tools )

tar cvf ${PKGNAME}.tar $PKGNAME
gzip ${PKGNAME}.tar
