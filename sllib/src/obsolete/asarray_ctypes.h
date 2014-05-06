/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2009-01-10 15:44:09 cyamauch> */

#ifndef _ASARRAY_CTYPES_H
#define _ASARRAY_CTYPES_H 1

#include "tarray_ctypes.h"

/* */
#define _ASARRAY_PLAIN__CREATE_STRUCT 1
#define _ASARRAY_PLAIN__USE_SPLIT 1
#define _ASARRAY_PLAIN__USE_VARG 1
#define _ASARRAY_PLAIN__USE_OP 1

#define _ASARRAY_PLAIN asarray_double
#define _ASARRAY_PLAIN__ASARRDEF asarrdef_double
#define _ASARRAY_PLAIN__TYPE double
#define _ASARRAY_PLAIN__VATYPE double
#include "_asarray_plain.h"

#define _ASARRAY_PLAIN asarray_float
#define _ASARRAY_PLAIN__ASARRDEF asarrdef_float
#define _ASARRAY_PLAIN__TYPE float
#define _ASARRAY_PLAIN__VATYPE double
#include "_asarray_plain.h"

#define _ASARRAY_PLAIN asarray_longlong
#define _ASARRAY_PLAIN__ASARRDEF asarrdef_longlong
#define _ASARRAY_PLAIN__TYPE long long
#define _ASARRAY_PLAIN__VATYPE long long
#include "_asarray_plain.h"

#define _ASARRAY_PLAIN asarray_long
#define _ASARRAY_PLAIN__ASARRDEF asarrdef_long
#define _ASARRAY_PLAIN__TYPE long
#define _ASARRAY_PLAIN__VATYPE long
#include "_asarray_plain.h"

#define _ASARRAY_PLAIN asarray_int
#define _ASARRAY_PLAIN__ASARRDEF asarrdef_int
#define _ASARRAY_PLAIN__TYPE int
#define _ASARRAY_PLAIN__VATYPE int
#include "_asarray_plain.h"

#define _ASARRAY_PLAIN asarray_short
#define _ASARRAY_PLAIN__ASARRDEF asarrdef_short
#define _ASARRAY_PLAIN__TYPE short
#define _ASARRAY_PLAIN__VATYPE int
#include "_asarray_plain.h"

#define _ASARRAY_PLAIN asarray_char
#define _ASARRAY_PLAIN__ASARRDEF asarrdef_char
#define _ASARRAY_PLAIN__TYPE char
#define _ASARRAY_PLAIN__VATYPE int
#include "_asarray_plain.h"

/* */
#undef _ASARRAY_PLAIN__USE_OP

#define _ASARRAY_PLAIN asarray_pointer_t
#define _ASARRAY_PLAIN__ASARRDEF asarrdef_pointer_t
#define _ASARRAY_PLAIN__TYPE pointer_t
#define _ASARRAY_PLAIN__VATYPE pointer_t
#include "_asarray_plain.h"

#undef _ASARRAY_PLAIN__USE_VARG
#undef _ASARRAY_PLAIN__USE_SPLIT
#undef _ASARRAY_PLAIN__CREATE_STRUCT

#endif	/* _ASARRAY_CTYPES_H */
