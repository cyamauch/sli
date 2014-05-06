/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2013-04-14 17:23:20 cyamauch> */

#ifndef _SLI__SIZE_TYPES_H
#define _SLI__SIZE_TYPES_H 1

/**
 * @file   size_types.h
 * @brief  mdarrayクラスで使用するプリミティブ型の型種別の定義
 */

#include <stdint.h>
#include <sys/types.h>

#include "complex_defs.h"

namespace sli
{

  const ssize_t FLOAT_ZT = -(ssize_t)sizeof(float);
  const ssize_t DOUBLE_ZT = -(ssize_t)sizeof(double);
  const ssize_t LDOUBLE_ZT = -(ssize_t)sizeof(long double);
  const ssize_t UCHAR_ZT = sizeof(unsigned char);
  const ssize_t SHORT_ZT = sizeof(short);
  const ssize_t INT_ZT = sizeof(int);
  const ssize_t LONG_ZT = sizeof(long);
  const ssize_t LLONG_ZT = sizeof(long long);
  const ssize_t INT16_ZT = sizeof(int16_t);
  const ssize_t INT32_ZT = sizeof(int32_t);
  const ssize_t INT64_ZT = sizeof(int64_t);
  const ssize_t SIZE_ZT = sizeof(size_t);
  const ssize_t SSIZE_ZT = sizeof(ssize_t);
  const ssize_t BOOL_ZT = sizeof(bool);
  const ssize_t UINTPTR_ZT = sizeof(uintptr_t);

  const ssize_t FCOMPLEX_ZT = -(ssize_t)(sizeof(fcomplex) - 1);
  const ssize_t DCOMPLEX_ZT = -(ssize_t)(sizeof(dcomplex) - 1);
  const ssize_t LDCOMPLEX_ZT = -(ssize_t)(sizeof(ldcomplex) - 1);

}

#endif	/* _SLI__SIZE_TYPES_H */
