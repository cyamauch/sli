/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2013-04-14 17:22:31 cyamauch> */

#ifndef _SLI__NUMERIC_MINMAX_H
#define _SLI__NUMERIC_MINMAX_H 1

/**
 * @file   numeric_minmax.h
 * @brief  mdarray���饹�ǻ��Ѥ�����ͷ��κǾ��͡������ͤ����
 */

#include <stdint.h>
#include <sys/types.h>

namespace sli
{

  const unsigned char MIN_UCHAR = 0;
  const short MIN_SHORT = (((short)1) << (8*sizeof(short)-1));
  const int MIN_INT = (((int)1) << (8*sizeof(int)-1));
  const long MIN_LONG = (1L << (8*sizeof(long)-1));
  const long long MIN_LLONG = (1LL << (8*sizeof(long long)-1));
  const int16_t MIN_INT16 = (((int16_t)1) << (8*sizeof(int16_t)-1));
  const int32_t MIN_INT32 = (((int32_t)1) << (8*sizeof(int32_t)-1));
  const int64_t MIN_INT64 = (((int64_t)1) << (8*sizeof(int64_t)-1));

  const unsigned char MAX_UCHAR = 255;
  const short MAX_SHORT = ~(MIN_SHORT);
  const int MAX_INT = ~(MIN_INT);
  const long MAX_LONG = ~(MIN_LONG);
  const long long MAX_LLONG = ~(MIN_LLONG);
  const int16_t MAX_INT16 = ~(MIN_INT16);
  const int32_t MAX_INT32 = ~(MIN_INT32);
  const int64_t MAX_INT64 = ~(MIN_INT64);

  const float MAX_FLOAT_ROUND_UCHAR = 255.49998;
  const float MAX_FLOAT_ROUND_INT16 = 32767.498;
  const float MAX_FLOAT_ROUND_INT32 = 2147483520.0;
  const float MAX_FLOAT_ROUND_INT64 = 9223371487098961920.0L;
  const float MAX_FLOAT_ROUND_SHORT = MAX_FLOAT_ROUND_INT16;
  const float MAX_FLOAT_ROUND_INT = 
        (sizeof(int) == 8) ? MAX_FLOAT_ROUND_INT64 : MAX_FLOAT_ROUND_INT32;
  const float MAX_FLOAT_ROUND_LONG = 
        (sizeof(long) == 8) ? MAX_FLOAT_ROUND_INT64 : MAX_FLOAT_ROUND_INT32;
  const float MAX_FLOAT_ROUND_LLONG = MAX_FLOAT_ROUND_INT64;

  const float MIN_FLOAT_ROUND_UCHAR = -0.49999997;
  const float MIN_FLOAT_ROUND_INT16 = -32768.496;
  const float MIN_FLOAT_ROUND_INT32 = MIN_INT32;
  const float MIN_FLOAT_ROUND_INT64 = MIN_INT64;
  const float MIN_FLOAT_ROUND_SHORT = MIN_FLOAT_ROUND_INT16;
  const float MIN_FLOAT_ROUND_INT = 
        (sizeof(int) == 8) ? MIN_FLOAT_ROUND_INT64 : MIN_FLOAT_ROUND_INT32;
  const float MIN_FLOAT_ROUND_LONG = 
        (sizeof(long) == 8) ? MIN_FLOAT_ROUND_INT64 : MIN_FLOAT_ROUND_INT32;
  const float MIN_FLOAT_ROUND_LLONG = MIN_FLOAT_ROUND_INT64;

  const float MAX_FLOAT_TRUNC_UCHAR = 255.99998;
  const float MAX_FLOAT_TRUNC_INT16 = 32767.998;
  const float MAX_FLOAT_TRUNC_INT32 = 2147483520.0;
  const float MAX_FLOAT_TRUNC_INT64 = 9223371487098961920.0L;
  const float MAX_FLOAT_TRUNC_SHORT = MAX_FLOAT_TRUNC_INT16;
  const float MAX_FLOAT_TRUNC_INT = 
        (sizeof(int) == 8) ? MAX_FLOAT_TRUNC_INT64 : MAX_FLOAT_TRUNC_INT32;
  const float MAX_FLOAT_TRUNC_LONG = 
        (sizeof(long) == 8) ? MAX_FLOAT_TRUNC_INT64 : MAX_FLOAT_TRUNC_INT32;
  const float MAX_FLOAT_TRUNC_LLONG = MAX_FLOAT_TRUNC_INT64;

  const float MIN_FLOAT_TRUNC_UCHAR = -0.99999994;
  const float MIN_FLOAT_TRUNC_INT16 = -32768.996;
  const float MIN_FLOAT_TRUNC_INT32 = MIN_INT32;
  const float MIN_FLOAT_TRUNC_INT64 = MIN_INT64;
  const float MIN_FLOAT_TRUNC_SHORT = MIN_FLOAT_TRUNC_INT16;
  const float MIN_FLOAT_TRUNC_INT = 
        (sizeof(int) == 8) ? MIN_FLOAT_TRUNC_INT64 : MIN_FLOAT_TRUNC_INT32;
  const float MIN_FLOAT_TRUNC_LONG = 
        (sizeof(long) == 8) ? MIN_FLOAT_TRUNC_INT64 : MIN_FLOAT_TRUNC_INT32;
  const float MIN_FLOAT_TRUNC_LLONG = MIN_FLOAT_TRUNC_INT64;

  const double MAX_DOUBLE_ROUND_UCHAR = 255.49999999999997L;
  const double MAX_DOUBLE_ROUND_INT16 = 32767.499999999996L;
  const double MAX_DOUBLE_ROUND_INT32 = 2147483647.4999998L;
  const double MAX_DOUBLE_ROUND_INT64 = 9223372036854774784.0L;
  const double MAX_DOUBLE_ROUND_SHORT = MAX_DOUBLE_ROUND_INT16;
  const double MAX_DOUBLE_ROUND_INT = 
        (sizeof(int) == 8) ? MAX_DOUBLE_ROUND_INT64 : MAX_DOUBLE_ROUND_INT32;
  const double MAX_DOUBLE_ROUND_LONG = 
        (sizeof(long) == 8) ? MAX_DOUBLE_ROUND_INT64 : MAX_DOUBLE_ROUND_INT32;
  const double MAX_DOUBLE_ROUND_LLONG = MAX_DOUBLE_ROUND_INT64;

  const double MIN_DOUBLE_ROUND_UCHAR = -0.49999999999999994L;
  const double MIN_DOUBLE_ROUND_INT16 = -32768.499999999993L;
  const double MIN_DOUBLE_ROUND_INT32 = -2147483648.4999995L;
  const double MIN_DOUBLE_ROUND_INT64 = MIN_INT64;
  const double MIN_DOUBLE_ROUND_SHORT = MIN_DOUBLE_ROUND_INT16;
  const double MIN_DOUBLE_ROUND_INT = 
        (sizeof(int) == 8) ? MIN_DOUBLE_ROUND_INT64 : MIN_DOUBLE_ROUND_INT32;
  const double MIN_DOUBLE_ROUND_LONG = 
        (sizeof(long) == 8) ? MIN_DOUBLE_ROUND_INT64 : MIN_DOUBLE_ROUND_INT32;
  const double MIN_DOUBLE_ROUND_LLONG = MIN_DOUBLE_ROUND_INT64;

  const double MAX_DOUBLE_TRUNC_UCHAR = 255.99999999999997L;
  const double MAX_DOUBLE_TRUNC_INT16 = 32767.999999999996L;
  const double MAX_DOUBLE_TRUNC_INT32 = 2147483647.9999998L;
  const double MAX_DOUBLE_TRUNC_INT64 = 9223372036854774784.0L;
  const double MAX_DOUBLE_TRUNC_SHORT = MAX_DOUBLE_TRUNC_INT16;
  const double MAX_DOUBLE_TRUNC_INT = 
        (sizeof(int) == 8) ? MAX_DOUBLE_TRUNC_INT64 : MAX_DOUBLE_TRUNC_INT32;
  const double MAX_DOUBLE_TRUNC_LONG = 
        (sizeof(long) == 8) ? MAX_DOUBLE_TRUNC_INT64 : MAX_DOUBLE_TRUNC_INT32;
  const double MAX_DOUBLE_TRUNC_LLONG = MAX_DOUBLE_TRUNC_INT64;

  const double MIN_DOUBLE_TRUNC_UCHAR = -0.99999999999999989L;
  const double MIN_DOUBLE_TRUNC_INT16 = -32768.999999999993L;
  const double MIN_DOUBLE_TRUNC_INT32 = -2147483648.9999995L;
  const double MIN_DOUBLE_TRUNC_INT64 = MIN_INT64;
  const double MIN_DOUBLE_TRUNC_SHORT = MIN_DOUBLE_TRUNC_INT16;
  const double MIN_DOUBLE_TRUNC_INT = 
        (sizeof(int) == 8) ? MIN_DOUBLE_TRUNC_INT64 : MIN_DOUBLE_TRUNC_INT32;
  const double MIN_DOUBLE_TRUNC_LONG = 
        (sizeof(long) == 8) ? MIN_DOUBLE_TRUNC_INT64 : MIN_DOUBLE_TRUNC_INT32;
  const double MIN_DOUBLE_TRUNC_LLONG = MIN_DOUBLE_TRUNC_INT64;

  const long double MAX_LDOUBLE_ROUND_UCHAR = 255.49999999999999999L;
  const long double MAX_LDOUBLE_ROUND_INT16 = 32767.499999999999998L;
  const long double MAX_LDOUBLE_ROUND_INT32 = 2147483647.4999999999L;
  const long double MAX_LDOUBLE_ROUND_INT64 = 9223372036854775807.0L;
  const long double MAX_LDOUBLE_ROUND_SHORT = MAX_LDOUBLE_ROUND_INT16;
  const long double MAX_LDOUBLE_ROUND_INT = 
       (sizeof(int) == 8) ? MAX_LDOUBLE_ROUND_INT64 : MAX_LDOUBLE_ROUND_INT32;
  const long double MAX_LDOUBLE_ROUND_LONG = 
       (sizeof(long) == 8) ? MAX_LDOUBLE_ROUND_INT64 : MAX_LDOUBLE_ROUND_INT32;
  const long double MAX_LDOUBLE_ROUND_LLONG = MAX_LDOUBLE_ROUND_INT64;

  const long double MIN_LDOUBLE_ROUND_UCHAR = -0.49999999999999999997L;
  const long double MIN_LDOUBLE_ROUND_INT16 = -32768.499999999999996L;
  const long double MIN_LDOUBLE_ROUND_INT32 = -2147483648.4999999998L;
  const long double MIN_LDOUBLE_ROUND_INT64 = -9223372036854775807.5L;
  const long double MIN_LDOUBLE_ROUND_SHORT = MIN_LDOUBLE_ROUND_INT16;
  const long double MIN_LDOUBLE_ROUND_INT = 
       (sizeof(int) == 8) ? MIN_LDOUBLE_ROUND_INT64 : MIN_LDOUBLE_ROUND_INT32;
  const long double MIN_LDOUBLE_ROUND_LONG = 
       (sizeof(long) == 8) ? MIN_LDOUBLE_ROUND_INT64 : MIN_LDOUBLE_ROUND_INT32;
  const long double MIN_LDOUBLE_ROUND_LLONG = MIN_LDOUBLE_ROUND_INT64;

  const long double MAX_LDOUBLE_TRUNC_UCHAR = 255.99999999999999999L;
  const long double MAX_LDOUBLE_TRUNC_INT16 = 32767.999999999999998L;
  const long double MAX_LDOUBLE_TRUNC_INT32 = 2147483647.9999999999L;
  const long double MAX_LDOUBLE_TRUNC_INT64 = 9223372036854775807.5L;
  const long double MAX_LDOUBLE_TRUNC_SHORT = MAX_LDOUBLE_TRUNC_INT16;
  const long double MAX_LDOUBLE_TRUNC_INT = 
       (sizeof(int) == 8) ? MAX_LDOUBLE_TRUNC_INT64 : MAX_LDOUBLE_TRUNC_INT32;
  const long double MAX_LDOUBLE_TRUNC_LONG = 
       (sizeof(long) == 8) ? MAX_LDOUBLE_TRUNC_INT64 : MAX_LDOUBLE_TRUNC_INT32;
  const long double MAX_LDOUBLE_TRUNC_LLONG = MAX_LDOUBLE_TRUNC_INT64;

  const long double MIN_LDOUBLE_TRUNC_UCHAR = -0.99999999999999999995L;
  const long double MIN_LDOUBLE_TRUNC_INT16 = -32768.999999999999996L;
  const long double MIN_LDOUBLE_TRUNC_INT32 = -2147483648.9999999998L;
  const long double MIN_LDOUBLE_TRUNC_INT64 = -9223372036854775808.0L;
  const long double MIN_LDOUBLE_TRUNC_SHORT = MIN_LDOUBLE_TRUNC_INT16;
  const long double MIN_LDOUBLE_TRUNC_INT = 
       (sizeof(int) == 8) ? MIN_LDOUBLE_TRUNC_INT64 : MIN_LDOUBLE_TRUNC_INT32;
  const long double MIN_LDOUBLE_TRUNC_LONG = 
       (sizeof(long) == 8) ? MIN_LDOUBLE_TRUNC_INT64 : MIN_LDOUBLE_TRUNC_INT32;
  const long double MIN_LDOUBLE_TRUNC_LLONG = MIN_LDOUBLE_TRUNC_INT64;

}

#endif	/* _SLI__NUMERIC_MINMAX_H */
