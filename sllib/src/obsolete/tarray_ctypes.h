/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2009-01-10 15:32:50 cyamauch> */

#ifndef _TARRAY_CTYPES_H
#define _TARRAY_CTYPES_H 1


#ifdef TARRAY_PLAIN__USE_SOLO_NARG
# undef TARRAY_PLAIN__USE_SOLO_NARG
#endif
//#define TARRAY_PLAIN__USE_SOLO_NARG 1

/* */
#define _TARRAY_PLAIN__USE_SPLIT 1
#define _TARRAY_PLAIN__USE_VARG 1
#define _TARRAY_PLAIN__USE_OP 1

#define _TARRAY_PLAIN tarray_double
#define _TARRAY_PLAIN__TYPE double
#define _TARRAY_PLAIN__VATYPE double
#include "_tarray_plain.h"

#define _TARRAY_PLAIN tarray_float
#define _TARRAY_PLAIN__TYPE float
#define _TARRAY_PLAIN__VATYPE double
#include "_tarray_plain.h"

#define _TARRAY_PLAIN tarray_longlong
#define _TARRAY_PLAIN__TYPE long long
#define _TARRAY_PLAIN__VATYPE long long
#include "_tarray_plain.h"

#define _TARRAY_PLAIN tarray_long
#define _TARRAY_PLAIN__TYPE long
#define _TARRAY_PLAIN__VATYPE long
#include "_tarray_plain.h"

#define _TARRAY_PLAIN tarray_int
#define _TARRAY_PLAIN__TYPE int
#define _TARRAY_PLAIN__VATYPE int
#include "_tarray_plain.h"

#define _TARRAY_PLAIN tarray_short
#define _TARRAY_PLAIN__TYPE short
#define _TARRAY_PLAIN__VATYPE int
#include "_tarray_plain.h"

#define _TARRAY_PLAIN tarray_char
#define _TARRAY_PLAIN__TYPE char
#define _TARRAY_PLAIN__VATYPE int
#include "_tarray_plain.h"

/* */
#undef _TARRAY_PLAIN__USE_OP

typedef void * pointer_t;

#define _TARRAY_PLAIN tarray_pointer_t
#define _TARRAY_PLAIN__TYPE pointer_t
#define _TARRAY_PLAIN__VATYPE pointer_t
#include "_tarray_plain.h"

#undef _TARRAY_PLAIN__USE_VARG
#undef _TARRAY_PLAIN__USE_SPLIT


#endif	/* _TARRAY_CTYPES_H */
