#!/bin/sh

OS="`uname -s | tr '[A-Z]' '[a-z]'`"
if [ "`echo $OS | cut -b 1-7`" = "cygwin_" ]; then
  OS="cygwin"
fi

echo '#ifndef _SLI_ENDIAN_H'
echo '#define _SLI_ENDIAN_H 1'
echo

if [ "$OS" = "linux" ]; then
  echo "#include <endian.h>"
elif [ "$OS" = "sunos" ]; then
  echo "#include <sys/isa_defs.h>"
elif [ "$OS" = "cygwin" ]; then
  echo "#include <sys/param.h>"
else
  # FreeBSD, MacOSX, etc.
  echo "#include <machine/endian.h>"
fi

echo

cat <<EOF
#ifndef LITTLE_ENDIAN
# ifdef _LITTLE_ENDIAN
#  define LITTLE_ENDIAN _LITTLE_ENDIAN
# else
#  ifdef __LITTLE_ENDIAN
#   define LITTLE_ENDIAN __LITTLE_ENDIAN
#  endif
# endif
#endif

#ifndef BIG_ENDIAN
# ifdef _BIG_ENDIAN
#  define BIG_ENDIAN _BIG_ENDIAN
# else
#  ifdef __BIG_ENDIAN
#   define BIG_ENDIAN __BIG_ENDIAN
#  endif
# endif
#endif

#if (!defined(LITTLE_ENDIAN) && !defined(BIG_ENDIAN))
# error "Cannot get ENDIAN macro symbols"
#endif

#if (defined(LITTLE_ENDIAN) && defined(BIG_ENDIAN))
# ifndef BYTE_ORDER
#  ifdef _BYTE_ORDER
#   define BYTE_ORDER _BYTE_ORDER
#  else
#   ifdef __BYTE_ORDER
#    define BYTE_ORDER __BYTE_ORDER
#   else
#    error "Cannot get BYTE_ORDER macro symbol"
#   endif
#  endif
# endif
# ifndef FLOAT_WORD_ORDER
#  ifdef _FLOAT_WORD_ORDER
#   define FLOAT_WORD_ORDER _FLOAT_WORD_ORDER
#  else
#   ifdef __FLOAT_WORD_ORDER
#    define FLOAT_WORD_ORDER __FLOAT_WORD_ORDER
#   endif
#  endif
# endif
#endif

#if (defined(LITTLE_ENDIAN) && !defined(BIG_ENDIAN))
# undef LITTLE_ENDIAN
# define LITTLE_ENDIAN 1234
# define BIG_ENDIAN    4321
# define BYTE_ORDER LITTLE_ENDIAN
#endif

#if (!defined(LITTLE_ENDIAN) && defined(BIG_ENDIAN))
# undef BIG_ENDIAN
# define LITTLE_ENDIAN 1234
# define BIG_ENDIAN    4321
# define BYTE_ORDER BIG_ENDIAN
#endif

#ifndef FLOAT_WORD_ORDER
# define FLOAT_WORD_ORDER BYTE_ORDER
#endif
EOF

echo
echo '#endif  /* _SLI_ENDIAN_H */'
