AC_INIT(sllib, 1.4.5, cyamauch@ir.isas.jaxa.jp)
AM_INIT_AUTOMAKE([foreign])
#LT_INIT([disable-shared])


#AC_CANONICAL_HOST
libnn=lib
if test -d /lib64; then
  libnn=lib64
fi
: ${LIBnn=$libnn}

AC_SUBST(LIBnn)
# take care not to override user's setting
if test "${libdir}" = '${exec_prefix}/lib'; then
  libdir='${exec_prefix}/${LIBnn}'
fi


AC_PROG_CXX
if test "$GXX" = "yes"; then
  CXXFLAGS="$CXXFLAGS -Wall"
fi
#AC_CONFIG_MACRO_DIR([m4])
AC_PROG_LIBTOOL

AC_LANG([C++])

AC_ARG_VAR([LIBnn], ['lib', 'lib64' or 'lib/64' for dynamic libraries])

AC_ARG_ENABLE(readline,
[  --disable-readline      do not use GNU readline],
[case "${enableval}" in
  yes) CONF_READLINE=conf_readline ;;
  no)  CONF_READLINE="" ;;
  *) AC_MSG_ERROR(bad value ${enableval} for --enable-readline) ;;
esac],[CONF_READLINE=conf_readline])
AC_SUBST(CONF_READLINE)

AC_ARG_ENABLE(bz2lib,
[  --disable-bz2lib        do not use bz2lib],
[case "${enableval}" in
  yes) CONF_BZ2LIB=conf_bz2lib ;;
  no)  CONF_BZ2LIB="" ;;
  *) AC_MSG_ERROR(bad value ${enableval} for --enable-bz2lib) ;;
esac],[CONF_BZ2LIB=conf_bz2lib])
AC_SUBST(CONF_BZ2LIB)

AC_ARG_ENABLE(zlib,
[  --disable-zlib          do not use zlib],
[case "${enableval}" in
  yes) CONF_ZLIB=conf_zlib ;;
  no)  CONF_ZLIB="" ;;
  *) AC_MSG_ERROR(bad value ${enableval} for --enable-zlib) ;;
esac],[CONF_ZLIB=conf_zlib])
AC_SUBST(CONF_ZLIB)

AC_ARG_ENABLE(simd,
[  --disable-simd          disable code using SIMD(SSE2)],
[case "${enableval}" in
  yes) CONF_SIMD=conf_simd ;;
  no)  CONF_SIMD="" ;;
  *) AC_MSG_ERROR(bad value ${enableval} for --enable-simd) ;;
esac],[CONF_SIMD=conf_simd])
AC_SUBST(CONF_SIMD)

if test x$CONF_ZLIB = xconf_zlib
then
AC_CHECK_LIB([z],[main], ,AC_MSG_ERROR([zlib is not installed!]))
AC_CHECK_HEADER([zlib.h],[],[AC_MSG_ERROR([zlib is not installed!])])
fi

if test x$CONF_BZ2LIB = xconf_bz2lib
then
AC_CHECK_LIB([bz2],[main], ,AC_MSG_ERROR([bzip2-libs is not installed!]))
AC_CHECK_HEADER([bzlib.h],[],[AC_MSG_ERROR([bzip2-libs is not installed!])])
fi

if test x$CONF_READLINE = xconf_readline
then
  AC_CHECK_LIB([curses],[main], ,AC_MSG_ERROR([curses is not installed!]))
  AC_CHECK_LIB([readline],[main], ,AC_MSG_ERROR([readline is not installed!]))
  AC_CHECK_HEADER([readline/readline.h],[],[AC_MSG_ERROR([readline is not installed!])])
fi

if test x$CONF_SIMD = xconf_simd
then
  AC_DEFINE(USE_SIMD, 1, [use optimized code using SIMD])
fi

# for Solaris

AC_CHECK_HEADER(sys/isa_defs.h, AC_DEFINE([__C99FEATURES__], [1], [C99 support for Solaris]))

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile 
                 sllib.pc])
AC_OUTPUT
