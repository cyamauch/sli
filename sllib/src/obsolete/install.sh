#!/bin/sh

umask 0022

OS="`uname -s | tr '[A-Z]' '[a-z]'`"
if [ "`echo $OS | cut -b 1-7`" = "cygwin_" ]; then
  OS="cygwin"
fi

if [ "$OS" = "linux" ]; then
  ROOT_ROOT="root:root"
elif [ "$OS" = "sunos" ]; then
  ROOT_ROOT="root:root"
elif [ "$OS" = "cygwin" ]; then
  ROOT_ROOT=""
else
  # FreeBSD, MacOSX, etc.
  ROOT_ROOT="root:wheel"
fi

USER="`whoami`"
if [ "$USER" = "" ]; then
  USER="`/usr/ucb/whoami`"
fi

DIR=""
FILES=""
MODE=""
MKDIR=0

NEXT_M=0
NARG=0
for i in $@ ; do
  NARG=`expr $NARG + 1`
done
CNT=1
for i in $@ ; do
  if [ $NEXT_M = 1 ]; then
    MODE="$i"
    NEXT_M=0
  elif [ "$i" = "-d" ]; then
    MKDIR=1
  elif [ "$i" = "-m" ]; then
    NEXT_M=1
  elif [ $CNT = $NARG ]; then
    DIR="$i"
  else
    FILES="$FILES $i"
  fi
  CNT=`expr $CNT + 1`
done

if [ $MKDIR = 1 ]; then
  mkdir -p $DIR
else
  cp -p $FILES $DIR/.
  for i in $FILES ; do
    if [ "$MODE" != "" ]; then
      chmod $MODE $DIR/$i
    fi
    if [ "$USER" = "root" ]; then
      if [ "$ROOT_ROOT" != "" ]; then
	chown $ROOT_ROOT $DIR/$i
      fi
    fi
  done
fi

