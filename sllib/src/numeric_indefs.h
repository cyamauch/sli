/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2013-04-14 17:21:54 cyamauch> */

#ifndef _SLI__NUMERIC_INDEFS_H
#define _SLI__NUMERIC_INDEFS_H 1

/**
 * @file   numeric_indefs.h
 * @brief  mdarrayクラスで使用する数値型の未定義値の定義
 */

#include <stdint.h>
#include <sys/types.h>

namespace sli
{

  const unsigned char INDEF_UCHAR = 0;
  const short INDEF_SHORT = ((short)1 << (8*sizeof(short)-1));
  const int INDEF_INT = ((int)1 << (8*sizeof(int)-1));
  const long INDEF_LONG = (1L << (8*sizeof(long)-1));
  const long long INDEF_LLONG = (1LL << (8*sizeof(long long)-1));
  const int16_t INDEF_INT16 = ((int16_t)1 << (8*sizeof(int16_t)-1));
  const int32_t INDEF_INT32 = ((int32_t)1 << (8*sizeof(int32_t)-1));
  const int64_t INDEF_INT64 = ((int64_t)1 << (8*sizeof(int64_t)-1));

}

#endif	/* _SLI__NUMERIC_INDEFS_H */
