#!/bin/sh

LIBDIR=$1
L32DIR=$2
L64DIR=$3
L_64DIR=$4

if [ -d /lib64 ]; then
    # Linux 64-bit
    echo "[HINT]"
    echo "  Use 'make install32' for ${LIBDIR}, or"
    echo "  use 'make install64' for ${L64DIR}."
elif [ -d /usr/lib/64 ]; then
    # Solaris 64-bit
    echo "[HINT]"
    echo "  Use 'make install32' for ${LIBDIR}, or"
    echo "  use 'make install64' for ${L_64DIR}."
elif [ -d /usr/lib32 ]; then
    # FreeBSD 64-bit
    echo "[HINT]"
    echo "  Use 'make install32' for ${L32DIR}, or"
    echo "  use 'make install64' for ${LIBDIR}."
else
    echo "[HINT]"
    echo "  Use 'make installany'."
fi
