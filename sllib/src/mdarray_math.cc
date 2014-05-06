/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2013-04-27 20:59:04 cyamauch> */

/**
 * @file   mdarray_math.cc
 * @brief  mdarrayとその継承クラスのオブジェクトで使用可能な数学関数のコード
 * @note   パフォーマンスを重視し，関数ポインタを使わない実装をしており，
 *         オブジェクトファイル肥大化の要因となっている．
 */

#define CLASS_NAME "-"

#include "config.h"

#include <math.h>
#include "mdarray_math.h"

#include "private/err_report.h"

#ifdef MD_NAME_MFNCL
#undef MD_NAME_MFNCL
#endif

/* cbrt() */
#define MD_NAME_NAMESPC __sli__cbrt
#define MD_NAME_FUNCTION cbrt
#define MD_NAME_MFNC cbrt
#define MD_NAME_MFNCF cbrtf
#include "skel/mdarray_math_calc1_noptr.cc"
#undef MD_NAME_NAMESPC
#undef MD_NAME_FUNCTION
#undef MD_NAME_MFNC
#undef MD_NAME_MFNCF
/* sqrt() */
#define MD_NAME_NAMESPC __sli__sqrt
#define MD_NAME_FUNCTION sqrt
#define MD_NAME_MFNC sqrt
#define MD_NAME_MFNCF sqrtf
#include "skel/mdarray_math_calc1_noptr.cc"
#undef MD_NAME_NAMESPC
#undef MD_NAME_FUNCTION
#undef MD_NAME_MFNC
#undef MD_NAME_MFNCF
/* asin() */
#define MD_NAME_NAMESPC __sli__asin
#define MD_NAME_FUNCTION asin
#define MD_NAME_MFNC asin
#define MD_NAME_MFNCF asinf
#include "skel/mdarray_math_calc1_noptr.cc"
#undef MD_NAME_NAMESPC
#undef MD_NAME_FUNCTION
#undef MD_NAME_MFNC
#undef MD_NAME_MFNCF
/* acos() */
#define MD_NAME_NAMESPC __sli__acos
#define MD_NAME_FUNCTION acos
#define MD_NAME_MFNC acos
#define MD_NAME_MFNCF acosf
#include "skel/mdarray_math_calc1_noptr.cc"
#undef MD_NAME_NAMESPC
#undef MD_NAME_FUNCTION
#undef MD_NAME_MFNC
#undef MD_NAME_MFNCF
/* atan() */
#define MD_NAME_NAMESPC __sli__atan
#define MD_NAME_FUNCTION atan
#define MD_NAME_MFNC atan
#define MD_NAME_MFNCF atanf
#include "skel/mdarray_math_calc1_noptr.cc"
#undef MD_NAME_NAMESPC
#undef MD_NAME_FUNCTION
#undef MD_NAME_MFNC
#undef MD_NAME_MFNCF
/* acosh() */
#define MD_NAME_NAMESPC __sli__acosh
#define MD_NAME_FUNCTION acosh
#define MD_NAME_MFNC acosh
#define MD_NAME_MFNCF acoshf
#include "skel/mdarray_math_calc1_noptr.cc"
#undef MD_NAME_NAMESPC
#undef MD_NAME_FUNCTION
#undef MD_NAME_MFNC
#undef MD_NAME_MFNCF
/* asinh() */
#define MD_NAME_NAMESPC __sli__asinh
#define MD_NAME_FUNCTION asinh
#define MD_NAME_MFNC asinh
#define MD_NAME_MFNCF asinhf
#include "skel/mdarray_math_calc1_noptr.cc"
#undef MD_NAME_NAMESPC
#undef MD_NAME_FUNCTION
#undef MD_NAME_MFNC
#undef MD_NAME_MFNCF
/* atanh() */
#define MD_NAME_NAMESPC __sli__atanh
#define MD_NAME_FUNCTION atanh
#define MD_NAME_MFNC atanh
#define MD_NAME_MFNCF atanhf
#include "skel/mdarray_math_calc1_noptr.cc"
#undef MD_NAME_NAMESPC
#undef MD_NAME_FUNCTION
#undef MD_NAME_MFNC
#undef MD_NAME_MFNCF
/* exp() */
#define MD_NAME_NAMESPC __sli__exp
#define MD_NAME_FUNCTION exp
#define MD_NAME_MFNC exp
#define MD_NAME_MFNCF expf
#include "skel/mdarray_math_calc1_noptr.cc"
#undef MD_NAME_NAMESPC
#undef MD_NAME_FUNCTION
#undef MD_NAME_MFNC
#undef MD_NAME_MFNCF
/* exp2() */
#define MD_NAME_NAMESPC __sli__exp2
#define MD_NAME_FUNCTION exp2
#define MD_NAME_MFNC exp2
#define MD_NAME_MFNCF exp2f
#include "skel/mdarray_math_calc1_noptr.cc"
#undef MD_NAME_NAMESPC
#undef MD_NAME_FUNCTION
#undef MD_NAME_MFNC
#undef MD_NAME_MFNCF
/* expm1() */
#define MD_NAME_NAMESPC __sli__expm1
#define MD_NAME_FUNCTION expm1
#define MD_NAME_MFNC expm1
#define MD_NAME_MFNCF expm1f
#include "skel/mdarray_math_calc1_noptr.cc"
#undef MD_NAME_NAMESPC
#undef MD_NAME_FUNCTION
#undef MD_NAME_MFNC
#undef MD_NAME_MFNCF
/* log() */
#define MD_NAME_NAMESPC __sli__log
#define MD_NAME_FUNCTION log
#define MD_NAME_MFNC log
#define MD_NAME_MFNCF logf
#include "skel/mdarray_math_calc1_noptr.cc"
#undef MD_NAME_NAMESPC
#undef MD_NAME_FUNCTION
#undef MD_NAME_MFNC
#undef MD_NAME_MFNCF
/* log1p() */
#define MD_NAME_NAMESPC __sli__log1p
#define MD_NAME_FUNCTION log1p
#define MD_NAME_MFNC log1p
#define MD_NAME_MFNCF log1pf
#include "skel/mdarray_math_calc1_noptr.cc"
#undef MD_NAME_NAMESPC
#undef MD_NAME_FUNCTION
#undef MD_NAME_MFNC
#undef MD_NAME_MFNCF
/* log10() */
#define MD_NAME_NAMESPC __sli__log10
#define MD_NAME_FUNCTION log10
#define MD_NAME_MFNC log10
#define MD_NAME_MFNCF log10f
#include "skel/mdarray_math_calc1_noptr.cc"
#undef MD_NAME_NAMESPC
#undef MD_NAME_FUNCTION
#undef MD_NAME_MFNC
#undef MD_NAME_MFNCF
/* sin() */
#define MD_NAME_NAMESPC __sli__sin
#define MD_NAME_FUNCTION sin
#define MD_NAME_MFNC sin
#define MD_NAME_MFNCF sinf
#include "skel/mdarray_math_calc1_noptr.cc"
#undef MD_NAME_NAMESPC
#undef MD_NAME_FUNCTION
#undef MD_NAME_MFNC
#undef MD_NAME_MFNCF
/* cos() */
#define MD_NAME_NAMESPC __sli__cos
#define MD_NAME_FUNCTION cos
#define MD_NAME_MFNC cos
#define MD_NAME_MFNCF cosf
#include "skel/mdarray_math_calc1_noptr.cc"
#undef MD_NAME_NAMESPC
#undef MD_NAME_FUNCTION
#undef MD_NAME_MFNC
#undef MD_NAME_MFNCF
/* tan() */
#define MD_NAME_NAMESPC __sli__tan
#define MD_NAME_FUNCTION tan
#define MD_NAME_MFNC tan
#define MD_NAME_MFNCF tanf
#include "skel/mdarray_math_calc1_noptr.cc"
#undef MD_NAME_NAMESPC
#undef MD_NAME_FUNCTION
#undef MD_NAME_MFNC
#undef MD_NAME_MFNCF
/* sinh() */
#define MD_NAME_NAMESPC __sli__sinh
#define MD_NAME_FUNCTION sinh
#define MD_NAME_MFNC sinh
#define MD_NAME_MFNCF sinhf
#include "skel/mdarray_math_calc1_noptr.cc"
#undef MD_NAME_NAMESPC
#undef MD_NAME_FUNCTION
#undef MD_NAME_MFNC
#undef MD_NAME_MFNCF
/* cosh() */
#define MD_NAME_NAMESPC __sli__cosh
#define MD_NAME_FUNCTION cosh
#define MD_NAME_MFNC cosh
#define MD_NAME_MFNCF coshf
#include "skel/mdarray_math_calc1_noptr.cc"
#undef MD_NAME_NAMESPC
#undef MD_NAME_FUNCTION
#undef MD_NAME_MFNC
#undef MD_NAME_MFNCF
/* tanh() */
#define MD_NAME_NAMESPC __sli__tanh
#define MD_NAME_FUNCTION tanh
#define MD_NAME_MFNC tanh
#define MD_NAME_MFNCF tanhf
#include "skel/mdarray_math_calc1_noptr.cc"
#undef MD_NAME_NAMESPC
#undef MD_NAME_FUNCTION
#undef MD_NAME_MFNC
#undef MD_NAME_MFNCF
/* erf() */
#define MD_NAME_NAMESPC __sli__erf
#define MD_NAME_FUNCTION erf
#define MD_NAME_MFNC erf
#define MD_NAME_MFNCF erff
#include "skel/mdarray_math_calc1_noptr.cc"
#undef MD_NAME_NAMESPC
#undef MD_NAME_FUNCTION
#undef MD_NAME_MFNC
#undef MD_NAME_MFNCF
/* erfc() */
#define MD_NAME_NAMESPC __sli__erfc
#define MD_NAME_FUNCTION erfc
#define MD_NAME_MFNC erfc
#define MD_NAME_MFNCF erfcf
#include "skel/mdarray_math_calc1_noptr.cc"
#undef MD_NAME_NAMESPC
#undef MD_NAME_FUNCTION
#undef MD_NAME_MFNC
#undef MD_NAME_MFNCF
/* ceil() */
#define MD_NAME_NAMESPC __sli__ceil
#define MD_NAME_FUNCTION ceil
#define MD_NAME_MFNC ceil
#define MD_NAME_MFNCF ceilf
#include "skel/mdarray_math_calc1_noptr.cc"
#undef MD_NAME_NAMESPC
#undef MD_NAME_FUNCTION
#undef MD_NAME_MFNC
#undef MD_NAME_MFNCF
/* floor() */
#define MD_NAME_NAMESPC __sli__floor
#define MD_NAME_FUNCTION floor
#define MD_NAME_MFNC floor
#define MD_NAME_MFNCF floorf
#include "skel/mdarray_math_calc1_noptr.cc"
#undef MD_NAME_NAMESPC
#undef MD_NAME_FUNCTION
#undef MD_NAME_MFNC
#undef MD_NAME_MFNCF
/* round() */
#define MD_NAME_NAMESPC __sli__round
#define MD_NAME_FUNCTION round
#define MD_NAME_MFNC round
#define MD_NAME_MFNCF roundf
#include "skel/mdarray_math_calc1_noptr.cc"
#undef MD_NAME_NAMESPC
#undef MD_NAME_FUNCTION
#undef MD_NAME_MFNC
#undef MD_NAME_MFNCF
/* trunc() */
#define MD_NAME_NAMESPC __sli__trunc
#define MD_NAME_FUNCTION trunc
#define MD_NAME_MFNC trunc
#define MD_NAME_MFNCF truncf
#include "skel/mdarray_math_calc1_noptr.cc"
#undef MD_NAME_NAMESPC
#undef MD_NAME_FUNCTION
#undef MD_NAME_MFNC
#undef MD_NAME_MFNCF
/* fabs() */
#define MD_NAME_NAMESPC __sli__fabs
#define MD_NAME_FUNCTION fabs
#define MD_NAME_MFNC fabs
#define MD_NAME_MFNCF fabsf
#include "skel/mdarray_math_calc1_noptr.cc"
#undef MD_NAME_NAMESPC
#undef MD_NAME_FUNCTION
#undef MD_NAME_MFNC
#undef MD_NAME_MFNCF

/* hypot() */
#define MD_NAME_NAMESPC __sli__calc2_hypot
#define MD_NAME_FUNCTION calc2_hypot
#define MD_NAME_MFNC hypot
#define MD_NAME_MFNCF hypotf
#include "skel/mdarray_math_calc2_noptr.cc"
namespace sli
{
mdarray hypot( const mdarray &obj, float v )
{
    mdarray dest;
    calc2_hypot(obj, v, FLOAT_ZT, false, &dest);
    dest.set_scopy_flag();
    return dest;
}
mdarray hypot( const mdarray &obj, double v )
{
    mdarray dest;
    calc2_hypot(obj, v, DOUBLE_ZT, false, &dest);
    dest.set_scopy_flag();
    return dest;
}
mdarray hypot( float v, const mdarray &obj )
{
    mdarray dest;
    calc2_hypot(obj, v, FLOAT_ZT, true, &dest);
    dest.set_scopy_flag();
    return dest;
}
mdarray hypot( double v, const mdarray &obj )
{
    mdarray dest;
    calc2_hypot(obj, v, DOUBLE_ZT, true, &dest);
    dest.set_scopy_flag();
    return dest;
}
}	/* namespace */
#undef MD_NAME_NAMESPC
#undef MD_NAME_FUNCTION
#undef MD_NAME_MFNC
#undef MD_NAME_MFNCF
/* pow() */
#define MD_NAME_NAMESPC __sli__calc2_pow
#define MD_NAME_FUNCTION calc2_pow
#define MD_NAME_MFNC pow
#define MD_NAME_MFNCF powf
#include "skel/mdarray_math_calc2_noptr.cc"
namespace sli
{
mdarray pow( const mdarray &obj, float v )
{
    mdarray dest;
    calc2_pow(obj, v, FLOAT_ZT, false, &dest);
    dest.set_scopy_flag();
    return dest;
}
mdarray pow( const mdarray &obj, double v )
{
    mdarray dest;
    calc2_pow(obj, v, DOUBLE_ZT, false, &dest);
    dest.set_scopy_flag();
    return dest;
}
mdarray pow( float v, const mdarray &obj )
{
    mdarray dest;
    calc2_pow(obj, v, FLOAT_ZT, true, &dest);
    dest.set_scopy_flag();
    return dest;
}
mdarray pow( double v, const mdarray &obj )
{
    mdarray dest;
    calc2_pow(obj, v, DOUBLE_ZT, true, &dest);
    dest.set_scopy_flag();
    return dest;
}
}	/* namespace */
#undef MD_NAME_NAMESPC
#undef MD_NAME_FUNCTION
#undef MD_NAME_MFNC
#undef MD_NAME_MFNCF
/* fmod() */
#define MD_NAME_NAMESPC __sli__calc2_fmod
#define MD_NAME_FUNCTION calc2_fmod
#define MD_NAME_MFNC fmod
#define MD_NAME_MFNCF fmodf
#include "skel/mdarray_math_calc2_noptr.cc"
namespace sli
{
mdarray fmod( const mdarray &obj, float v )
{
    mdarray dest;
    calc2_fmod(obj, v, FLOAT_ZT, false, &dest);
    dest.set_scopy_flag();
    return dest;
}
mdarray fmod( const mdarray &obj, double v )
{
    mdarray dest;
    calc2_fmod(obj, v, DOUBLE_ZT, false, &dest);
    dest.set_scopy_flag();
    return dest;
}
mdarray fmod( float v, const mdarray &obj )
{
    mdarray dest;
    calc2_fmod(obj, v, FLOAT_ZT, true, &dest);
    dest.set_scopy_flag();
    return dest;
}
mdarray fmod( double v, const mdarray &obj )
{
    mdarray dest;
    calc2_fmod(obj, v, DOUBLE_ZT, true, &dest);
    dest.set_scopy_flag();
    return dest;
}
}	/* namespace */
#undef MD_NAME_NAMESPC
#undef MD_NAME_FUNCTION
#undef MD_NAME_MFNC
#undef MD_NAME_MFNCF
/* remainder() */
#define MD_NAME_NAMESPC __sli__calc2_remainder
#define MD_NAME_FUNCTION calc2_remainder
#define MD_NAME_MFNC remainder
#define MD_NAME_MFNCF remainderf
#include "skel/mdarray_math_calc2_noptr.cc"
namespace sli
{
mdarray remainder( const mdarray &obj, float v )
{
    mdarray dest;
    calc2_remainder(obj, v, FLOAT_ZT, false, &dest);
    dest.set_scopy_flag();
    return dest;
}
mdarray remainder( const mdarray &obj, double v )
{
    mdarray dest;
    calc2_remainder(obj, v, DOUBLE_ZT, false, &dest);
    dest.set_scopy_flag();
    return dest;
}
mdarray remainder( float v, const mdarray &obj )
{
    mdarray dest;
    calc2_remainder(obj, v, FLOAT_ZT, true, &dest);
    dest.set_scopy_flag();
    return dest;
}
mdarray remainder( double v, const mdarray &obj )
{
    mdarray dest;
    calc2_remainder(obj, v, DOUBLE_ZT, true, &dest);
    dest.set_scopy_flag();
    return dest;
}
}	/* namespace */
#undef MD_NAME_NAMESPC
#undef MD_NAME_FUNCTION
#undef MD_NAME_MFNC
#undef MD_NAME_MFNCF

/*
 * create: mdarray name( const mdarray &src0, const mdarray &src1 )
 */

/* hypot() */
#define MD_NAME_NAMESPC __sli__calc2s_hypot
#define MD_NAME_R_FUNCTION r_calc2s_hypot
#define MD_NAME_S_FUNCTION s_calc2s_hypot
#define MD_NAME_FUNCTION hypot
#define MD_NAME_MFNC hypot
#define MD_NAME_MFNCF hypotf
#include "skel/mdarray_math_calc2s_noptr.cc"
#undef MD_NAME_NAMESPC
#undef MD_NAME_R_FUNCTION
#undef MD_NAME_S_FUNCTION
#undef MD_NAME_FUNCTION
#undef MD_NAME_MFNC
#undef MD_NAME_MFNCF
/* pow() */
#define MD_NAME_NAMESPC __sli__calc2s_pow
#define MD_NAME_R_FUNCTION r_calc2s_pow
#define MD_NAME_S_FUNCTION s_calc2s_pow
#define MD_NAME_FUNCTION pow
#define MD_NAME_MFNC pow
#define MD_NAME_MFNCF powf
#include "skel/mdarray_math_calc2s_noptr.cc"
#undef MD_NAME_NAMESPC
#undef MD_NAME_R_FUNCTION
#undef MD_NAME_S_FUNCTION
#undef MD_NAME_FUNCTION
#undef MD_NAME_MFNC
#undef MD_NAME_MFNCF
/* fmod() */
#define MD_NAME_NAMESPC __sli__calc2s_fmod
#define MD_NAME_R_FUNCTION r_calc2s_fmod
#define MD_NAME_S_FUNCTION s_calc2s_fmod
#define MD_NAME_FUNCTION fmod
#define MD_NAME_MFNC fmod
#define MD_NAME_MFNCF fmodf
#include "skel/mdarray_math_calc2s_noptr.cc"
#undef MD_NAME_NAMESPC
#undef MD_NAME_R_FUNCTION
#undef MD_NAME_S_FUNCTION
#undef MD_NAME_FUNCTION
#undef MD_NAME_MFNC
#undef MD_NAME_MFNCF
/* remainder() */
#define MD_NAME_NAMESPC __sli__calc2s_remainder
#define MD_NAME_R_FUNCTION r_calc2s_remainder
#define MD_NAME_S_FUNCTION s_calc2s_remainder
#define MD_NAME_FUNCTION remainder
#define MD_NAME_MFNC remainder
#define MD_NAME_MFNCF remainderf
#include "skel/mdarray_math_calc2s_noptr.cc"
#undef MD_NAME_NAMESPC
#undef MD_NAME_R_FUNCTION
#undef MD_NAME_S_FUNCTION
#undef MD_NAME_FUNCTION
#undef MD_NAME_MFNC
#undef MD_NAME_MFNCF


namespace sli
{

namespace _calc1
{
#define MAKE_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
static void fncname( const void *org_val_ptr, void *new_val_ptr, size_t len, \
		     double (*func)(double), float (*funcf)(float), \
		     long double (*funcl)(long double) ) \
{ \
    size_t i; \
    new_type *dest = (new_type *)new_val_ptr; \
    const org_type *src = (const org_type *)org_val_ptr; \
    if ( func != NULL ) { \
	for ( i=0 ; i < len ; i++ ) dest[i] = (new_type)((*func)(src[i])); \
    } \
    else if ( funcf != NULL ) { \
	for ( i=0 ; i < len ; i++ ) dest[i] = (new_type)((*funcf)(src[i])); \
    } \
    else if ( funcl != NULL ) { \
	for ( i=0 ; i < len ; i++ ) dest[i] = (new_type)((*funcl)(src[i])); \
    } \
    return; \
}
SLI__MDARRAY__DO_OPERATION_2TYPES(MAKE_FUNC,,,,,,,,,,,,,);
#undef MAKE_FUNC
}

/**
 * @brief  mdarray の演算 (未使用)
 */
mdarray calc1( const mdarray &src,
	       double (*func)(double), float (*funcf)(float),
	       long double (*funcl)(long double) )
{
    mdarray dest;
    ssize_t dest_sz_type;
    void (*fnc_calc)(const void *, void *, size_t,
	   double (*)(double), float (*)(float), long double (*)(long double));

    /* 実数型でない場合は例外を返す */
    switch( src.size_type() ) {
      /* 実数型 */
      case FLOAT_ZT     :
      case DOUBLE_ZT    :
      case LDOUBLE_ZT   :
      case UCHAR_ZT     :
      case INT16_ZT     :
      case INT32_ZT     :
      case INT64_ZT     :
	/* NO PROBLEM */
	break;
      /* 複素数型 */
      case FCOMPLEX_ZT  :
      case DCOMPLEX_ZT  :
      case LDCOMPLEX_ZT :
	err_throw(__FUNCTION__,"ERROR","complex type cannot be used");
	break;
      /* その他の型 */
      default           :
	err_throw(__FUNCTION__,"ERROR","unsupported type");
	break;
    }

    if ( src.length() == 0 ) goto quit;

    fnc_calc = NULL;
    dest_sz_type = (0 < src.size_type()) ? -8 : src.size_type();

    /* 関数を選択 */
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
    if (src.size_type() == org_sz_type && dest_sz_type == new_sz_type) { \
        fnc_calc = &_calc1::fncname; \
    }
    SLI__MDARRAY__DO_OPERATION_2TYPES(SEL_FUNC,,,,,,,,,,,,,else);
#undef SEL_FUNC

    if ( fnc_calc != NULL ) {
	const size_t dim_len = src.dim_length();
	const size_t *szs = src.cdimarray();
	dest.init(dest_sz_type, true, szs, dim_len, false);
	if ( dest_sz_type < -8 ) {
	    if ( funcl != NULL ) {
		func = NULL;
		funcf = NULL;
	    }
	}
	if ( dest_sz_type == -4 ) {
	    if ( funcf != NULL ) {
		func = NULL;
		funcl = NULL;
	    }
	}
	(*fnc_calc)(src.data_ptr_cs(), dest.data_ptr(), dest.length(),
		    func, funcf, funcl);
    } else {
	err_report(__FUNCTION__,"WARNING","invalid size of type; ignored");
    }
 quit:
    dest.set_auto_resize(src.auto_resize());
    dest.set_auto_init(src.auto_init());
    dest.set_rounding(src.rounding());

    dest.set_scopy_flag();
    return dest;
}

namespace _calc2
{
#define MAKE_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
static void fncname( const void *org_val_ptr, long double s_v, bool rv, \
		     void *new_val_ptr, size_t len, \
		     double (*func)(double,double), \
		     float (*funcf)(float,float), \
		     long double (*funcl)(long double,long double) ) \
{ \
  size_t i; \
  new_type *dest = (new_type *)new_val_ptr; \
  const org_type *src = (const org_type *)org_val_ptr; \
  if ( rv == false ) { \
    if ( func != NULL ) { \
       for (i=0 ; i < len ; i++) dest[i] = (new_type)((*func)(src[i], s_v)); \
    } \
    else if ( funcf != NULL ) { \
       for (i=0 ; i < len ; i++) dest[i] = (new_type)((*funcf)(src[i], s_v)); \
    } \
    else if ( funcl != NULL ) { \
       for (i=0 ; i < len ; i++) dest[i] = (new_type)((*funcl)(src[i], s_v)); \
    } \
  } \
  else { \
    if ( func != NULL ) { \
       for (i=0 ; i < len ; i++) dest[i] = (new_type)((*func)(s_v, src[i])); \
    } \
    else if ( funcf != NULL ) { \
       for (i=0 ; i < len ; i++) dest[i] = (new_type)((*funcf)(s_v, src[i])); \
    } \
    else if ( funcl != NULL ) { \
       for (i=0 ; i < len ; i++) dest[i] = (new_type)((*funcl)(s_v, src[i])); \
    } \
  } \
  return; \
}
SLI__MDARRAY__DO_OPERATION_2TYPES(MAKE_FUNC,,,,,,,,,,,,,);
#undef MAKE_FUNC
}


/**
 * @brief  mdarray の演算 (未使用)
 */
mdarray calc2( const mdarray &src0, 
	       long double src1, ssize_t szt_src1, bool rv,
	       double (*func)(double,double), float (*funcf)(float,float),
	       long double (*funcl)(long double,long double))
{
    mdarray dest;
    ssize_t zt0, zt1, dest_sz_type;
    void (*fnc_calc)(const void *, long double, bool, void *, size_t,
		     double (*)(double,double), float (*)(float,float), 
		     long double (*)(long double,long double));

    /* 実数型でない場合は例外を返す */
    switch( src0.size_type() ) {
      /* 実数型 */
      case FLOAT_ZT     :
      case DOUBLE_ZT    :
      case LDOUBLE_ZT   :
      case UCHAR_ZT     :
      case INT16_ZT     :
      case INT32_ZT     :
      case INT64_ZT     :
	/* NO PROBLEM */
	break;
      /* 複素数型 */
      case FCOMPLEX_ZT  :
      case DCOMPLEX_ZT  :
      case LDCOMPLEX_ZT :
	err_throw(__FUNCTION__,"ERROR","complex type cannot be used");
	break;
      /* その他の型 */
      default           :
	err_throw(__FUNCTION__,"ERROR","unsupported type");
	break;
    }

    if ( src0.length() == 0 ) goto quit;

    fnc_calc = NULL;
    zt0 = (0 < src0.size_type()) ? -8 : src0.size_type();
    zt1 = szt_src1;
    dest_sz_type = (zt0 < zt1) ? zt0 : zt1;

    /* 関数を選択 */
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
    if (src0.size_type() == org_sz_type && dest_sz_type == new_sz_type) { \
        fnc_calc = &_calc2::fncname; \
    }
    SLI__MDARRAY__DO_OPERATION_2TYPES(SEL_FUNC,,,,,,,,,,,,,else);
#undef SEL_FUNC

    if ( fnc_calc != NULL ) {
	const size_t dim_len = src0.dim_length();
	const size_t *szs = src0.cdimarray();
	dest.init(dest_sz_type, true, szs, dim_len, false);
	if ( dest_sz_type < -8 ) {
	    if ( funcl != NULL ) {
		func = NULL;
		funcf = NULL;
	    }
	}
	if ( dest_sz_type == -4 ) {
	    if ( funcf != NULL ) {
		func = NULL;
		funcl = NULL;
	    }
	}
	(*fnc_calc)(src0.data_ptr_cs(), src1, rv, dest.data_ptr(),
		    dest.length(), func, funcf, funcl);
    } else {
	err_report(__FUNCTION__,"WARNING","invalid size of type; ignored");
    }
 quit:
    dest.set_auto_resize(src0.auto_resize());
    dest.set_auto_init(src0.auto_init());
    dest.set_rounding(src0.rounding());

    dest.set_scopy_flag();
    return dest;
}

/**
 * @brief  mdarray の演算 (未使用)
 */
mdarray calc2( const mdarray &src0, const mdarray &src1,
	       double (*func)(double,double), float (*funcf)(float,float),
	       long double (*funcl)(long double,long double) )
{
    const ssize_t zt0 = (0 < src0.size_type()) ? -8 : src0.size_type();
    const ssize_t zt1 = (0 < src1.size_type()) ? -8 : src1.size_type();
    const ssize_t dest_sz_type = (zt0 < zt1) ? zt0 : zt1;
    size_t dim_len = src0.dim_length();
    mdarray ret(dest_sz_type, true);
    mdarray_size nx;
    size_t i;
    if ( dim_len < src1.dim_length() ) dim_len = src1.dim_length();
    nx.resize(dim_len);
    for ( i=0 ; i < dim_len ; i++ ) {
	size_t n = src0.length(i);
	if ( n < src1.length(i) ) n = src1.length(i);
	nx[i] = n;
    }

    /* 実数型でない場合は例外を返す */
    switch( src0.size_type() ) {
      /* 実数型 */
      case FLOAT_ZT     :
      case DOUBLE_ZT    :
      case LDOUBLE_ZT   :
      case UCHAR_ZT     :
      case INT16_ZT     :
      case INT32_ZT     :
      case INT64_ZT     :
	/* NO PROBLEM */
	break;
      /* 複素数型 */
      case FCOMPLEX_ZT  :
      case DCOMPLEX_ZT  :
      case LDCOMPLEX_ZT :
	err_throw(__FUNCTION__,"ERROR","complex type cannot be used");
	break;
      /* その他の型 */
      default           :
	err_throw(__FUNCTION__,"ERROR","unsupported type");
	break;
    }

    /* 実数型でない場合は例外を返す */
    switch( src1.size_type() ) {
      /* 実数型 */
      case FLOAT_ZT     :
      case DOUBLE_ZT    :
      case LDOUBLE_ZT   :
      case UCHAR_ZT     :
      case INT16_ZT     :
      case INT32_ZT     :
      case INT64_ZT     :
	/* NO PROBLEM */
	break;
      /* 複素数型 */
      case FCOMPLEX_ZT  :
      case DCOMPLEX_ZT  :
      case LDCOMPLEX_ZT :
	err_throw(__FUNCTION__,"ERROR","complex type cannot be used");
	break;
      /* その他の型 */
      default           :
	err_throw(__FUNCTION__,"ERROR","unsupported type");
	break;
    }

    ret.reallocate(nx.array_ptr(), dim_len, true);
    ret.set_auto_resize(true);
    ret += src0;
    ret.calc2(src1, false, func, funcf, funcl);
    ret.set_auto_resize(src0.auto_resize());
    ret.set_auto_init(src0.auto_init());
    ret.set_rounding(src0.rounding());

    ret.set_scopy_flag();
    return ret;
}


}
