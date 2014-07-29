AC_INIT(sfitsio, 1.4.4, cyamauch@ir.isas.jaxa.jp)
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

AC_ARG_ENABLE(simd,
[  --disable-simd          disable code using SIMD(SSE2)],
[case "${enableval}" in
  yes) CONF_SIMD=conf_simd ;;
  no)  CONF_SIMD="" ;;
  *) AC_MSG_ERROR(bad value ${enableval} for --enable-simd) ;;
esac],[CONF_SIMD=conf_simd])
AC_SUBST(CONF_SIMD)

if test x$CONF_SIMD = xconf_simd
then
  AC_DEFINE(USE_SIMD, 1, [use optimized code using SIMD])
fi

CPPFLAGS="$CPPFLAGS -I${prefix}/include"
LDFLAGS="$LDFLAGS -L${prefix}/$LIBnn"

AC_CHECK_LIB([sllib],[main], ,AC_MSG_ERROR([sllib is not installed!]))
AC_CHECK_HEADER([sli/cstreamio.h],[],[AC_MSG_ERROR([sllib is not installed!])])

AC_DEFINE([BUILD_SFITSIO], [1], [Indicating SFITSIO sources])
# for Solaris
AC_CHECK_HEADER(sys/isa_defs.h, AC_DEFINE([__C99FEATURES__], [1], [C99 support for Solaris]))

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile 
                 sfitsio.pc])
AC_OUTPUT
