/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2013-04-14 17:10:53 cyamauch> */

#ifndef _SLI__COMPLEX_DEFS_H
#define _SLI__COMPLEX_DEFS_H 1

/**
 * @file   complex_defs.h
 * @brief  複素数型の定義
 * @note   C99と同様に定義しています．
 */

namespace sli
{
    typedef float _Complex fcomplex;
    typedef double _Complex dcomplex;
    typedef long double _Complex ldcomplex;
}

#endif
