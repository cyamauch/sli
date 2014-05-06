/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2009-01-10 15:43:21 cyamauch> */

#include "tarray_ctypes.h"

/* */
#define _TARRAY_PLAIN__GET_SOURCE 1

#define _TARRAY_PLAIN__USE_SPLIT 1
#define _TARRAY_PLAIN__USE_VARG 1
#define _TARRAY_PLAIN__USE_OP 1

#define _TARRAY_PLAIN tarray_double
#define _TARRAY_PLAIN__TYPE double
#define _TARRAY_PLAIN__VATYPE double
#define _TARRAY_PLAIN__ATOX atof
#include "_tarray_plain.h"

#define _TARRAY_PLAIN tarray_float
#define _TARRAY_PLAIN__TYPE float
#define _TARRAY_PLAIN__VATYPE double
#define _TARRAY_PLAIN__ATOX atof
#include "_tarray_plain.h"

#define _TARRAY_PLAIN tarray_longlong
#define _TARRAY_PLAIN__TYPE long long
#define _TARRAY_PLAIN__VATYPE long long
#define _TARRAY_PLAIN__ATOX atoll
#include "_tarray_plain.h"

#define _TARRAY_PLAIN tarray_long
#define _TARRAY_PLAIN__TYPE long
#define _TARRAY_PLAIN__VATYPE long
#define _TARRAY_PLAIN__ATOX atol
#include "_tarray_plain.h"

#define _TARRAY_PLAIN tarray_int
#define _TARRAY_PLAIN__TYPE int
#define _TARRAY_PLAIN__VATYPE int
#define _TARRAY_PLAIN__ATOX atoi
#include "_tarray_plain.h"

#define _TARRAY_PLAIN tarray_short
#define _TARRAY_PLAIN__TYPE short
#define _TARRAY_PLAIN__VATYPE int
#define _TARRAY_PLAIN__ATOX atoi
#include "_tarray_plain.h"

#define _TARRAY_PLAIN tarray_char
#define _TARRAY_PLAIN__TYPE char
#define _TARRAY_PLAIN__VATYPE int
#define _TARRAY_PLAIN__ATOX atoi
#include "_tarray_plain.h"

/* */
#undef _TARRAY_PLAIN__USE_OP

#define _TARRAY_PLAIN tarray_pointer_t
#define _TARRAY_PLAIN__TYPE pointer_t
#define _TARRAY_PLAIN__VATYPE pointer_t
#define _TARRAY_PLAIN__ATOX atol
#include "_tarray_plain.h"

