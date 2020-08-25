/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2018-05-28 02:30:39 cyamauch> */

#define CLASS_NAME "mdarray"

/**
 * @file   mdarray.cc
 * @brief  多次元配列を扱うための基底クラス mdarray のコード
 */

#include "config.h"

#include "mdarray.h"
#include "numeric_minmax.h"
#include "numeric_indefs.h"
#include "heap_mem.h"

#include <stdlib.h>
#include <math.h>

#include "sli_funcs.h"
#include "complex.h"

#include "private/err_report.h"

#include "private/c_trunc.h"
#include "private/c_round.h"
#include "private/c_floor.h"
#include "private/c_ceil.h"

#include "private/c_strcmp.h"
#include "private/c_strtol.h"
#include "private/c_strtoul.h"
#include "private/c_snprintf.h"
#include "private/c_vasprintf.h"

#include "private/c_memset.h"
#include "private/c_memmove.h"
#include "private/c_memcpy.h"
#include "private/memfill.h"

/* SSE2 */
#include "private/s_memset.h"
#include "private/s_memfill.h"
#include "private/s_memcpy.h"
#include "private/s_memmove.h"
#include "private/s_memflip.h"
#include "private/s_memswap.h"
#include "private/s_byteswap.h"

#ifndef debug_report
//#define debug_report(xx) err_report(__FUNCTION__,"DEBUG",xx)
#define debug_report(xx)
#endif

/* shallow copy 関係の debug 用 */
//#define debug_report_scopy(arg) err_report(__FUNCTION__,"DEBUG",arg)
#define debug_report_scopy(arg)
//#define debug_report_scopy1(a1,a2) err_report1(__FUNCTION__,"DEBUG",a1,a2)
#define debug_report_scopy1(a1,a2)

namespace sli
{

/**
 * @brief  ssize_t の値を受け取り，size_t で絶対値を返す (内部用)
 *
 * @note   private なマクロです．
 */
#define abs_sz2z(val) ( ((val) < 0) ? (size_t)(0-(val)) : (size_t)(val) )

/**
 * @brief  size_type(mdarrayの型種別) からバイト長に変換 (内部用)
 *
 * @note   private な関数です．
 */
inline static size_t zt2bytes( ssize_t sz_type )
{
    if ( 0 <= sz_type ) return (size_t)sz_type;
    else {
	size_t ret = (size_t)((ssize_t)0 - sz_type);
	if ( ret % 2 != 0 ) ret++;		/* 複素数の場合 */
	return ret;
    }
}

/*
 * 浮動小数点 → 整数 変換用関数
 */

/* float */

/**
 * @brief  float 型から unsigned char 型への変換 (小数点以下を切捨て)
 *
 * @note   private な関数です．
 */
inline static unsigned char tnc_f2c( float v )
{
    return (unsigned char)v;
}
/**
 * @brief  float 型から unsigned char 型への変換 (小数点以下を四捨五入)
 *
 * @note   private な関数です．
 */
inline static unsigned char rnd_f2c( float v )
{
    if ( v < 0 ) return (unsigned char)(v-0.5);
    else return (unsigned char)(v+0.5);
}

/**
 * @brief  float 型から int16_t 型への変換 (小数点以下を切捨て)
 *
 * @note   private な関数です．
 */
inline static int16_t tnc_f2i16( float v )
{
    return (int16_t)v;
}
/**
 * @brief  float 型から int16_t 型への変換 (小数点以下を四捨五入)
 *
 * @note   private な関数です．
 */
inline static int16_t rnd_f2i16( float v )
{
    if ( v < 0 ) return (int16_t)(v-0.5);
    else return (int16_t)(v+0.5);
}

/**
 * @brief  float 型から int32_t 型への変換 (小数点以下を切捨て)
 *
 * @note   private な関数です．
 */
inline static int32_t tnc_f2i32( float v )
{
    return (int32_t)v;
}
/**
 * @brief  float 型から int32_t 型への変換 (小数点以下を四捨五入)
 *
 * @note   private な関数です．
 */
inline static int32_t rnd_f2i32( float v )
{
    if ( v < 0 ) return (int32_t)(v-0.5);
    else return (int32_t)(v+0.5);
}

/**
 * @brief  float 型から int64_t 型への変換 (小数点以下を切捨て)
 *
 * @note   private な関数です．
 */
inline static int64_t tnc_f2i64( float v )
{
    return (int64_t)v;
}
/**
 * @brief  float 型から int64_t 型への変換 (小数点以下を四捨五入)
 *
 * @note   private な関数です．
 */
inline static int64_t rnd_f2i64( float v )
{
    if ( v < 0 ) return (int64_t)(v-0.5);
    else return (int64_t)(v+0.5);
}

/* double */

/**
 * @brief  double 型から unsigned char 型への変換 (小数点以下を切捨て)
 *
 * @note   private な関数です．
 */
inline static unsigned char tnc_d2c( double v )
{
    return (unsigned char)v;
}
/**
 * @brief  double 型から unsigned char 型への変換 (小数点以下を四捨五入)
 *
 * @note   private な関数です．
 */
inline static unsigned char rnd_d2c( double v )
{
    if ( v < 0 ) return (unsigned char)(v-0.5);
    else return (unsigned char)(v+0.5);
}

/**
 * @brief  double 型から int16_t 型への変換 (小数点以下を切捨て)
 *
 * @note   private な関数です．
 */
inline static int16_t tnc_d2i16( double v )
{
    return (int16_t)v;
}
/**
 * @brief  double 型から int16_t 型への変換 (小数点以下を四捨五入)
 *
 * @note   private な関数です．
 */
inline static int16_t rnd_d2i16( double v )
{
    if ( v < 0 ) return (int16_t)(v-0.5);
    else return (int16_t)(v+0.5);
}

/**
 * @brief  double 型から int32_t 型への変換 (小数点以下を切捨て)
 *
 * @note   private な関数です．
 */
inline static int32_t tnc_d2i32( double v )
{
    return (int32_t)v;
}
/**
 * @brief  double 型から int32_t 型への変換 (小数点以下を四捨五入)
 *
 * @note   private な関数です．
 */
inline static int32_t rnd_d2i32( double v )
{
    if ( v < 0 ) return (int32_t)(v-0.5);
    else return (int32_t)(v+0.5);
}

/**
 * @brief  double 型から int64_t 型への変換 (小数点以下を切捨て)
 *
 * @note   private な関数です．
 */
inline static int64_t tnc_d2i64( double v )
{
    return (int64_t)v;
}
/**
 * @brief  double 型から int64_t 型への変換 (小数点以下を四捨五入)
 *
 * @note   private な関数です．
 */
inline static int64_t rnd_d2i64( double v )
{
    if ( v < 0 ) return (int64_t)(v-0.5);
    else return (int64_t)(v+0.5);
}

/* long double */

/**
 * @brief  long double 型から unsigned char 型への変換 (小数点以下を切捨て)
 *
 * @note   private な関数です．
 */
inline static unsigned char tnc_ld2c( long double v )
{
    return (unsigned char)v;
}
/**
 * @brief  long double 型から unsigned char 型への変換 (小数点以下を四捨五入)
 *
 * @note   private な関数です．
 */
inline static unsigned char rnd_ld2c( long double v )
{
    if ( v < 0 ) return (unsigned char)(v-0.5);
    else return (unsigned char)(v+0.5);
}

/**
 * @brief  long double 型から int16_t 型への変換 (小数点以下を切捨て)
 *
 * @note   private な関数です．
 */
inline static int16_t tnc_ld2i16( long double v )
{
    return (int16_t)v;
}
/**
 * @brief  long double 型から int16_t 型への変換 (小数点以下を四捨五入)
 *
 * @note   private な関数です．
 */
inline static int16_t rnd_ld2i16( long double v )
{
    if ( v < 0 ) return (int16_t)(v-0.5);
    else return (int16_t)(v+0.5);
}

/**
 * @brief  long double 型から int32_t 型への変換 (小数点以下を切捨て)
 *
 * @note   private な関数です．
 */
inline static int32_t tnc_ld2i32( long double v )
{
    return (int32_t)v;
}
/**
 * @brief  long double 型から int32_t 型への変換 (小数点以下を四捨五入)
 *
 * @note   private な関数です．
 */
inline static int32_t rnd_ld2i32( long double v )
{
    if ( v < 0 ) return (int32_t)(v-0.5);
    else return (int32_t)(v+0.5);
}

/**
 * @brief  long double 型から int64_t 型への変換 (小数点以下を切捨て)
 *
 * @note   private な関数です．
 */
inline static int64_t tnc_ld2i64( long double v )
{
    return (int64_t)v;
}
/**
 * @brief  long double 型から int64_t 型への変換 (小数点以下を四捨五入)
 *
 * @note   private な関数です．
 */
inline static int64_t rnd_ld2i64( long double v )
{
    if ( v < 0 ) return (int64_t)(v-0.5);
    else return (int64_t)(v+0.5);
}

/* fcomplex */

/**
 * @brief  fcomplex 型から unsigned char 型への変換
 *         (実数部の小数点以下を切捨て)
 *
 * @note   private な関数です．
 */
inline static unsigned char tnc_fx2c( fcomplex v )
{
    /* 実数部のみ変換する */
    return (unsigned char)(creal( v ));
}
/**
 * @brief  fcomplex 型から unsigned char 型への変換
 *         (実数部の小数点以下を四捨五入)
 *
 * @note   private な関数です．
 */
inline static unsigned char rnd_fx2c( fcomplex v )
{
    /* 実数部のみ変換する */
    float freal = creal( v );
    
    if ( freal < 0.0 ) return (unsigned char)(freal-0.5);
    else return (unsigned char)(freal+0.5);
}

/**
 * @brief  fcomplex 型から int16_t 型への変換
 *         (実数部の小数点以下を切捨て)
 *
 * @note   private な関数です．
 */
inline static int16_t tnc_fx2i16( fcomplex v )
{
    /* 実数部のみ変換する */
    return (int16_t)(creal( v ));
}
/**
 * @brief  fcomplex 型から int16_t 型への変換
 *         (実数部の小数点以下を四捨五入)
 *
 * @note   private な関数です．
 */
inline static int16_t rnd_fx2i16( fcomplex v )
{
    /* 実数部のみ変換する */
    float freal = creal( v );

    if ( freal < 0.0 ) return (int16_t)(freal-0.5);
    else return (int16_t)(freal+0.5);
}

/**
 * @brief  fcomplex 型から int32_t 型への変換
 *         (実数部の小数点以下を切捨て)
 *
 * @note   private な関数です．
 */
inline static int32_t tnc_fx2i32( fcomplex v )
{
    /* 実数部のみ変換する */
    return (int32_t)(creal( v ));
}
/**
 * @brief  fcomplex 型から int32_t 型への変換
 *         (実数部の小数点以下を四捨五入)
 *
 * @note   private な関数です．
 */
inline static int32_t rnd_fx2i32( fcomplex v )
{
    /* 実数部のみ変換する */
    float freal = creal( v );

    if ( freal < 0.0 ) return (int32_t)(freal-0.5);
    else return (int32_t)(freal+0.5);
}

/**
 * @brief  fcomplex 型から int64_t 型への変換
 *         (実数部の小数点以下を切捨て)
 *
 * @note   private な関数です．
 */
inline static int64_t tnc_fx2i64( fcomplex v )
{
    /* 実数部のみ変換する */
    return (int64_t)(creal( v ));
}
/**
 * @brief  fcomplex 型から int64_t 型への変換
 *         (実数部の小数点以下を四捨五入)
 *
 * @note   private な関数です．
 */
inline static int64_t rnd_fx2i64( fcomplex v )
{
    /* 実数部のみ変換する */
    float freal = creal( v );

    if ( freal < 0.0 ) return (int64_t)(freal-0.5);
    else return (int64_t)(freal+0.5);
}

/* dcomplex */

/**
 * @brief  dcomplex 型から unsigned char 型への変換
 *         (実数部の小数点以下を切捨て)
 *
 * @note   private な関数です．
 */
inline static unsigned char tnc_dx2c( dcomplex v )
{
    /* 実数部のみ変換する */
    return (unsigned char)(creal( v ));
}
/**
 * @brief  dcomplex 型から unsigned char 型への変換
 *         (実数部の小数点以下を四捨五入)
 *
 * @note   private な関数です．
 */
inline static unsigned char rnd_dx2c( dcomplex v )
{
    /* 実数部のみ変換する */
    double dreal = creal( v );

    if ( dreal < 0 ) return (unsigned char)(dreal-0.5);
    else return (unsigned char)(dreal+0.5);
}

/**
 * @brief  dcomplex 型から int16_t 型への変換
 *         (実数部の小数点以下を切捨て)
 *
 * @note   private な関数です．
 */
inline static int16_t tnc_dx2i16( dcomplex v )
{
    /* 実数部のみ変換する */
    return (int16_t)(creal( v ));
}
/**
 * @brief  dcomplex 型から int16_t 型への変換
 *         (実数部の小数点以下を四捨五入)
 *
 * @note   private な関数です．
 */
inline static int16_t rnd_dx2i16( dcomplex v )
{
    /* 実数部のみ変換する */
    double dreal = creal( v );

    if ( dreal < 0 ) return (int16_t)(dreal-0.5);
    else return (int16_t)(dreal+0.5);
}

/**
 * @brief  dcomplex 型から int32_t 型への変換
 *         (実数部の小数点以下を切捨て)
 *
 * @note   private な関数です．
 */
inline static int32_t tnc_dx2i32( dcomplex v )
{
    /* 実数部のみ変換する */
    return (int32_t)(creal( v ));
}
/**
 * @brief  dcomplex 型から int32_t 型への変換
 *         (実数部の小数点以下を四捨五入)
 *
 * @note   private な関数です．
 */
inline static int32_t rnd_dx2i32( dcomplex v )
{
    /* 実数部のみ変換する */
    double dreal = creal( v );

    if ( dreal < 0 ) return (int32_t)(dreal-0.5);
    else return (int32_t)(dreal+0.5);
}

/**
 * @brief  dcomplex 型から int64_t 型への変換
 *         (実数部の小数点以下を切捨て)
 *
 * @note   private な関数です．
 */
inline static int64_t tnc_dx2i64( dcomplex v )
{
    /* 実数部のみ変換する */
    return (int64_t)(creal( v ));
}
/**
 * @brief  dcomplex 型から int64_t 型への変換
 *         (実数部の小数点以下を四捨五入)
 *
 * @note   private な関数です．
 */
inline static int64_t rnd_dx2i64( dcomplex v )
{
    /* 実数部のみ変換する */
    double dreal = creal( v );

    if ( dreal < 0 ) return (int64_t)(dreal-0.5);
    else return (int64_t)(dreal+0.5);
}

/* ldcomplex */

/**
 * @brief  ldcomplex 型から unsigned char 型への変換
 *         (実数部の小数点以下を切捨て)
 *
 * @note   private な関数です．
 */
inline static unsigned char tnc_ldx2c( ldcomplex v )
{
    /* 実数部のみ変換する */
    return (unsigned char)(creal( v ));
}
/**
 * @brief  ldcomplex 型から unsigned char 型への変換
 *         (実数部の小数点以下を四捨五入)
 *
 * @note   private な関数です．
 */
inline static unsigned char rnd_ldx2c( ldcomplex v )
{
    /* 実数部のみ変換する */
    long double ldreal = creal( v );

    if ( ldreal < 0 ) return (unsigned char)(ldreal-0.5);
    else return (unsigned char)(ldreal+0.5);
}

/**
 * @brief  ldcomplex 型から int16_t 型への変換
 *         (実数部の小数点以下を切捨て)
 *
 * @note   private な関数です．
 */
inline static int16_t tnc_ldx2i16( ldcomplex v )
{
    /* 実数部のみ変換する */
    return (int16_t)(creal( v ));
}
/**
 * @brief  ldcomplex 型から int16_t 型への変換
 *         (実数部の小数点以下を四捨五入)
 *
 * @note   private な関数です．
 */
inline static int16_t rnd_ldx2i16( ldcomplex v )
{
    /* 実数部のみ変換する */
    long double ldreal = creal( v );

    if ( ldreal < 0 ) return (int16_t)(ldreal-0.5);
    else return (int16_t)(ldreal+0.5);
}

/**
 * @brief  ldcomplex 型から int32_t 型への変換
 *         (実数部の小数点以下を切捨て)
 *
 * @note   private な関数です．
 */
inline static int32_t tnc_ldx2i32( ldcomplex v )
{
    /* 実数部のみ変換する */
    return (int32_t)(creal( v ));
}
/**
 * @brief  ldcomplex 型から int32_t 型への変換
 *         (実数部の小数点以下を四捨五入)
 *
 * @note   private な関数です．
 */
inline static int32_t rnd_ldx2i32( ldcomplex v )
{
    /* 実数部のみ変換する */
    long double ldreal = creal( v );

    if ( ldreal < 0 ) return (int32_t)(ldreal-0.5);
    else return (int32_t)(ldreal+0.5);
}

/**
 * @brief  ldcomplex 型から int64_t 型への変換
 *         (実数部の小数点以下を切捨て)
 *
 * @note   private な関数です．
 */
inline static int64_t tnc_ldx2i64( ldcomplex v )
{
    /* 実数部のみ変換する */
    return (int64_t)(creal( v ));
}
/**
 * @brief  ldcomplex 型から int64_t 型への変換
 *         (実数部の小数点以下を四捨五入)
 *
 * @note   private な関数です．
 */
inline static int64_t rnd_ldx2i64( ldcomplex v )
{
    /* 実数部のみ変換する */
    long double ldreal = creal( v );

    if ( ldreal < 0 ) return (int64_t)(ldreal-0.5);
    else return (int64_t)(ldreal+0.5);
}

/* 浮動小数点 → "long" or "long long" */

/**
 * @brief  float 型から long 型への変換 (小数点以下を切捨て)
 *
 * @note   private な関数です．
 */
inline static long tnc_f2l( float v )
{
    return (long)v;
}
/**
 * @brief  float 型から long 型への変換 (小数点以下を四捨五入)
 *
 * @note   private な関数です．
 */
inline static long rnd_f2l( float v )
{
    if ( v < 0 ) return (long)(v-0.5);
    else return (long)(v+0.5);
}

/**
 * @brief  float 型から long long 型への変換 (小数点以下を切捨て)
 *
 * @note   private な関数です．
 */
inline static long long tnc_f2ll( float v )
{
    return (long long)v;
}
/**
 * @brief  float 型から long long 型への変換 (小数点以下を四捨五入)
 *
 * @note   private な関数です．
 */
inline static long long rnd_f2ll( float v )
{
    if ( v < 0 ) return (long long)(v-0.5);
    else return (long long)(v+0.5);
}

/**
 * @brief  double 型から long 型への変換 (小数点以下を切捨て)
 *
 * @note   private な関数です．
 */
inline static long tnc_d2l( double v )
{
    return (long)v;
}
/**
 * @brief  double 型から long 型への変換 (小数点以下を四捨五入)
 *
 * @note   private な関数です．
 */
inline static long rnd_d2l( double v )
{
    if ( v < 0 ) return (long)(v-0.5);
    else return (long)(v+0.5);
}

/**
 * @brief  double 型から long long 型への変換 (小数点以下を切捨て)
 *
 * @note   private な関数です．
 */
inline static long long tnc_d2ll( double v )
{
    return (long long)v;
}
/**
 * @brief  double 型から long long 型への変換 (小数点以下を四捨五入)
 *
 * @note   private な関数です．
 */
inline static long long rnd_d2ll( double v )
{
    if ( v < 0 ) return (long long)(v-0.5);
    else return (long long)(v+0.5);
}

/**
 * @brief  long double 型から long 型への変換 (小数点以下を切捨て)
 *
 * @note   private な関数です．
 */
inline static long tnc_ld2l( long double v )
{
    return (long)v;
}
/**
 * @brief  long double 型から long 型への変換 (小数点以下を四捨五入)
 *
 * @note   private な関数です．
 */
inline static long rnd_ld2l( long double v )
{
    if ( v < 0 ) return (long)(v-0.5);
    else return (long)(v+0.5);
}

/**
 * @brief  long double 型から long long 型への変換 (小数点以下を切捨て)
 *
 * @note   private な関数です．
 */
inline static long long tnc_ld2ll( long double v )
{
    return (long long)v;
}
/**
 * @brief  long double 型から long long 型への変換 (小数点以下を四捨五入)
 *
 * @note   private な関数です．
 */
inline static long long rnd_ld2ll( long double v )
{
    if ( v < 0 ) return (long long)(v-0.5);
    else return (long long)(v+0.5);
}

/* 複素数 → 浮動小数点 */

/**
 * @brief  float 型から fcomplex 型への変換
 *
 * @note   private な関数です．
 */
inline static fcomplex f2fx( float v )
{
    return ( (float)v + 0.0*I );
}
/**
 * @brief  double 型から dcomplex 型への変換
 *
 * @note   private な関数です．
 */
inline static dcomplex d2dx( double v )
{
    return ( (double)v + 0.0*I );
}
/**
 * @brief  long double 型から ldcomplex 型への変換
 *
 * @note   private な関数です．
 */
inline static ldcomplex ld2ldx( long double v )
{
    return ( (long double)v + 0.0*I );
}

/* */

struct mdarray_cnv_nd_prms {
    /* アライメントに注意(sizeofが長いものから順に!) */
    size_t total_len_elements;
    bool round_flg;
};

/* const がないがこれは定数．double, float 型へ アレイを変換する時に使う */
static struct mdarray_cnv_nd_prms Null_cnv_nd_prms = {0, false};

/* 前からか後ろからか，方向が決まっている場合．                */
/* f_b = -1 で後ろから，f_b = 1 で前から，f_b = 0 でスカラー値 */
namespace cnv_nd
{
#define MAKE_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
static void fncname(const void *src_p, void *dst_p, size_t n_vals, int f_b, void *u_ptr) \
{ \
    const struct mdarray_cnv_nd_prms *prms = (const struct mdarray_cnv_nd_prms *)u_ptr; \
    const org_type *src = (const org_type *)src_p; \
    new_type *dst = (new_type *)dst_p; \
    const size_t n = n_vals; \
    const bool flg = prms->round_flg; \
    size_t i; \
    if ( f_b < 0 ) { \
	for ( i=n ; 0 < i ; ) { \
	    i--; \
	    dst[i] = (new_type)(fnc(src[i])); \
	} \
    } \
    else { \
	const size_t n_v = ((0 < f_b) ? n : ((0 < n) ? 1 : 0)); \
	for ( i=0 ; i < n_v ; i++ ) dst[i] = (new_type)(fnc(src[i])); \
	if ( i < n ) s_memfill(dst+i, dst, sizeof(new_type), n - i, prms->total_len_elements); \
    } \
    i = flg; /* junk code for warning */ \
}
SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(MAKE_FUNC, flg ? rnd_f2c(src[i]) : tnc_f2c,flg ? rnd_f2i16(src[i]) : tnc_f2i16,flg ? rnd_f2i32(src[i]) : tnc_f2i32,flg ? rnd_f2i64(src[i]) : tnc_f2i64, flg ? rnd_d2c(src[i]) : tnc_d2c,flg ? rnd_d2i16(src[i]) : tnc_d2i16,flg ? rnd_d2i32(src[i]) : tnc_d2i32,flg ? rnd_d2i64(src[i]) : tnc_d2i64, flg ? rnd_ld2c(src[i]) : tnc_ld2c,flg ? rnd_ld2i16(src[i]) : tnc_ld2i16,flg ? rnd_ld2i32(src[i]) : tnc_ld2i32,flg ? rnd_ld2i64(src[i]) : tnc_ld2i64, flg ? rnd_fx2c(src[i]) : tnc_fx2c,flg ? rnd_fx2i16(src[i]) : tnc_fx2i16,flg ? rnd_fx2i32(src[i]) : tnc_fx2i32,flg ? rnd_fx2i64(src[i]) : tnc_fx2i64, flg ? rnd_dx2c(src[i]) : tnc_dx2c,flg ? rnd_dx2i16(src[i]) : tnc_dx2i16,flg ? rnd_dx2i32(src[i]) : tnc_dx2i32,flg ? rnd_dx2i64(src[i]) : tnc_dx2i64, flg ? rnd_ldx2c(src[i]) : tnc_ldx2c,flg ? rnd_ldx2i16(src[i]) : tnc_ldx2i16,flg ? rnd_ldx2i32(src[i]) : tnc_ldx2i32,flg ? rnd_ldx2i64(src[i]) : tnc_ldx2i64,f2fx,d2dx,ld2ldx,);
#undef MAKE_FUNC
}


/* 
 * endian info
 */
static const bool Byte_order_is_little = Sli__byte_order_is_little_endian;
static const bool Float_word_order_is_little = Sli__float_word_order_is_little_endian;

/*
 * for alloc strategy
 */
static const int Alloc_strategy_auto = 0;
static const int Alloc_strategy_min = 1;
static const int Alloc_strategy_pow = 2;

/**
 * @brief  各軸の要素数を保存した配列を複製 (内部用)
 *
 * @note   malloc() で取得した領域を返す．使ったら必ず free() で開放する．<br>
 *         private な関数です．
 */
static size_t *dup_size_rec( const size_t *size_rec, size_t dim_len )
{
    if ( size_rec == NULL || dim_len == 0 ) return NULL;
    else {
	size_t *ret;
	size_t i;
	ret = (size_t *)malloc(sizeof(*ret) * dim_len);
	if ( ret == NULL ) {
	    return ret;
	}
	for ( i=0 ; i < dim_len ; i++ ) ret[i] = size_rec[i];
	return ret;
    }
}


/* ========================================================================= */

/**
 * @brief  コンストラクタ用の特別なイニシャライザ
 *
 * @param  sz_type 変数の型種別
 * @param  is_constructor コンストラクタによって呼び出される場合は true を，
 *         init()等から呼ぶ場合は false をセットする．
 *
 * @note   このメンバ関数は private です．
 *
 */
void mdarray::__force_init( ssize_t sz_type, bool is_constructor )
{
    /* FITSバイナリテーブルでカラム長0を許すため，sz_type=0 もOKとする */
    this->sz_type_rec = sz_type;
    this->set_auto_resize(true);
    this->set_auto_init(true);
    this->set_rounding(false);
    this->set_alloc_strategy("auto");
    this->_default_rec = NULL;
    this->dim_size_rec = 0;
    this->_size_rec = NULL;
    this->_arr_rec = NULL;
    this->arr_alloc_blen_rec = 0;
    this->_arr_ptr_2d_rec = NULL;
    this->_arr_ptr_3d_rec = NULL;
    c_memset(this->move_from_idx_rec,0,sizeof(this->move_from_idx_rec));
    c_memset(this->move_from_len_rec,0,sizeof(this->move_from_len_rec));

    if ( is_constructor == true ) {
	/* これらは .init() では初期化されない */
	this->extptr_rec = NULL;
	this->extptr_2d_rec = NULL;
	this->extptr_3d_rec = NULL;
	/* this->length_change_notify_func = NULL; */
	/* this->length_change_notify_user_ptr = NULL; */
	this->shallow_copy_ok = false;
	this->shallow_copy_dest_obj = NULL;
	this->shallow_copy_src_obj = NULL;
	this->__copying = false;
	this->class_level_rec = 0;
    }

    this->update_length_info();

    return;
}

/**
 * @brief  コンストラクタ
 *
 */
mdarray::mdarray()
{
    this->__force_init(this->default_size_type(), true);

    return;
}

/**
 * @brief  コンストラクタ
 *
 * @param  sz_type 変数の型種別
 * @throw  sz_typeの値が無効な場合
 */
mdarray::mdarray( ssize_t sz_type )
{
    this->__force_init(sz_type, true);

    return;
}

/**
 * @brief  コンストラクタ
 *
 * @param  sz_type 変数の型種別
 * @throw  sz_typeの値が無効な場合
 */
mdarray::mdarray( ssize_t sz_type, bool auto_resize )
{
    this->__force_init(sz_type, true);

    this->set_auto_resize(auto_resize);

    return;
}

/**
 * @brief  コンストラクタ
 *
 * @param      sz_type 変数の型種別
 * @param      extptt_ptr ユーザのポインタ変数のアドレス
 * @throw      sz_typeの値が無効な場合
 * @attention  ユーザのポインタ変数を登録すると shallow copy が機能しなくなり
 *             ます．
 */
mdarray::mdarray( ssize_t sz_type, bool auto_resize, void *extptr_address )
{
    this->__force_init(sz_type, true);

    this->set_auto_resize(auto_resize);
    this->extptr_rec = (void **)extptr_address;

    if ( this->extptr_rec != NULL ) *(this->extptr_rec) = this->_arr_rec;

    return;
}

/**
 * @brief  コンストラクタ
 *
 * @param  sz_type 変数の型種別
 * @param  naxisx[] 各次元の要素数
 * @param  ndim 配列次元数
 * @param  init_buf 要素値をデフォルト値で埋めるなら true
 * @throw  sz_typeの値が無効な場合
 * @throw  内部バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 */
mdarray::mdarray( ssize_t sz_type, bool auto_resize, 
		  const size_t naxisx[], size_t ndim, bool init_buf )
{
    this->__force_init(sz_type, true);

    this->mdarray::init(sz_type, auto_resize, naxisx, ndim, init_buf);

    return;
}

#if 0 /* SLI__DISABLE_INCOMPATIBLE_V1_4 */

/**
 * @brief  コンストラクタ
 *
 * @deprecated  現在は未使用
 * @param  sz_type 変数の型種別
 * @param  naxis0 次元番号0 の次元(1 次元目) の要素数
 * @throw  sz_typeの値が無効な場合
 * @throw  内部バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 */
mdarray::mdarray( ssize_t sz_type, size_t naxis0 )
{
    const size_t naxisx[] = {naxis0};

    this->__force_init(sz_type, true);

    this->mdarray::init(sz_type, naxisx, 1, true);

    return;
}

/**
 * @brief  コンストラクタ
 *
 * @deprecated  現在は未使用
 * @param  sz_type 変数の型種別
 * @param  naxis0 次元番号0 の次元(1 次元目) の要素数
 * @param  naxis1 次元番号1 の次元(2 次元目) の要素数
 * @throw  sz_typeの値が無効な場合
 * @throw  内部バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 */
mdarray::mdarray( ssize_t sz_type, size_t naxis0, size_t naxis1 )
{
    const size_t naxisx[] = {naxis0, naxis1};

    this->__force_init(sz_type, true);

    this->mdarray::init(sz_type, naxisx, 2, true);

    return;
}

/**
 * @brief  コンストラクタ
 *
 * @deprecated  現在は未使用
 * @param  sz_type 変数の型種別
 * @param  naxis0 次元番号0 の次元(1 次元目) の要素数
 * @param  naxis1 次元番号1 の次元(2 次元目) の要素数
 * @param  naxis2 次元番号2 の次元(3 次元目) の要素数
 * @throw  sz_typeの値が無効な場合
 * @throw  内部バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 */
mdarray::mdarray( ssize_t sz_type,
		  size_t naxis0, size_t naxis1, size_t naxis2 )
{
    const size_t naxisx[] = {naxis0, naxis1, naxis2};

    this->__force_init(sz_type, true);

    this->mdarray::init(sz_type, naxisx, 3, true);

    return;
}
#endif	/* SLI__DISABLE_INCOMPATIBLE_V1_4 */

/**
 * @brief  コピーコンストラクタ
 *
 * @param  obj コピー元となるオブジェクト
 * @throw  内部バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 */
mdarray::mdarray( const mdarray &obj )
{
    this->__force_init(obj.size_type(), true);

    this->mdarray::init(obj);
}

/**
 * @brief  destructor 用の特別なメモリ開放用メンバ関数
 *
 * @note   このメンバ関数は private です．
 *
 */
void mdarray::__force_free()
{
    if ( this->_default_rec != NULL ) {
	free(this->_default_rec);
    }
    if ( this->_size_rec != NULL ) {
	free(this->_size_rec);
    }
    if ( this->_arr_rec != NULL ) {
	free(this->_arr_rec);
    }
    if ( this->_arr_ptr_2d_rec != NULL ) {
	free(this->_arr_ptr_2d_rec);
    }
    if ( this->_arr_ptr_3d_rec != NULL ) {
	size_t i = 0;
	while ( this->_arr_ptr_3d_rec[i] != NULL ) {
	    free(this->_arr_ptr_3d_rec[i]);
	    i++;
	}
	free(this->_arr_ptr_3d_rec);
    }

    return;
}

/**
 * @brief  デストラクタ
 *
 */
mdarray::~mdarray()
{
    /* shallow copy された後，順当に消滅する場合 (copy の src 側の処理) */
    if ( this->shallow_copy_dest_obj != NULL ) {
	/* こちら側が消滅した事を通知する */
	this->shallow_copy_dest_obj->shallow_copy_src_obj = NULL;
	/* バッファは開放してはいけない */
    	debug_report_scopy("destructor not free() [0]");
	return;
    }
    /* shallow copy の src がまだ生きている場合 (copy の dest 側の処理) */
    else if ( this->shallow_copy_src_obj != NULL ) {
	/* shallow copy をキャンセルする */
	this->shallow_copy_src_obj->cancel_shallow_copy(this);
	/* バッファは開放してはいけない */
    	debug_report_scopy("destructor not free() [1]");
	return;
    }
    else {
	this->__force_free();
    	debug_report_scopy("destructor done free()");
	return;
    }
}

/* private */

/**
 * @brief  shallow copy が可能かを返す (src側で実行)
 *
 * @param   from_obj リクエストを送信する側のオブジェクトのアドレス
 * @return  shallow copyが可能なら真<br>
 *          それ以外の時は偽
 * @note    SLLIBの実装では，shallow copy は特定の相手とのみ可．<br>
 *          このメンバ関数は private です．
 */
bool mdarray::request_shallow_copy( mdarray *from_obj ) const
{
    if ( this->shallow_copy_ok == true ) {
	/* 双方とも shallow copy に無関係である事をチェック */
	if ( this->shallow_copy_dest_obj == NULL &&
	     this->shallow_copy_src_obj == NULL &&
	     from_obj->shallow_copy_dest_obj == NULL &&
	     from_obj->shallow_copy_src_obj == NULL ) {
	    /* 両者の関係を登録 */
	    mdarray *thisp = (mdarray *)this;
	    /* 相手を自身のメンバ変数にマーク */
	    /* (相手が消滅または deep copy 処理後にNULLになる) */
	    thisp->shallow_copy_dest_obj = from_obj;
	    /* 自身を相手のメンバ変数にマーク */
	    /* (自身が消滅したらNULLになる．See デストラクタ) */
	    from_obj->shallow_copy_src_obj = thisp;
	    /* */
	    debug_report_scopy("accepted shallow copy");
	    return true;
	}
    }
    debug_report_scopy("rejected shallow copy");
    return false;
}

/**
 * @brief  shallow copy のキャンセル (src側で実行)
 * 
 * @param  from_obj リクエストを送信した側のオブジェクトのアドレス
 * @note   from_obj から呼んだ直後に from_obj にて __force_init() を実行する
 *         必要があります．<br>
 *         このメンバ関数は private です．
 */
void mdarray::cancel_shallow_copy( mdarray *from_obj ) const
{
    debug_report_scopy1("arg: from_obj = %zx",(size_t)from_obj);

    if ( this->shallow_copy_dest_obj == from_obj ) {
	mdarray *thisp = (mdarray *)this;
	/* 相手側をNULLに */
	from_obj->shallow_copy_src_obj = NULL;
	/* 自身のもNULLに */
	thisp->shallow_copy_dest_obj = NULL;
    }
    else {
 	err_throw(__FUNCTION__,"FATAL","internal error");
    }

    return;
}

/**
 * @brief  自身について，shallow copy 関係についてのクリーンアップを実施
 *
 * @param  do_deep_copy_for_this shallow copy のキャンセル後に，自身について
 *                               deep copy を行なう場合は true をセットする．
 * @note   次の場合に呼び出す必要がある．<br>
 *          1. バッファに書き込む場合<br>
 *          2. バッファのアドレスを返す場合<br>
 *          3. __shallow_init(), __deep_init() 呼び出しの直前<br>
 *         このメンバ関数は private です．
 */
void mdarray::cleanup_shallow_copy( bool do_deep_copy_for_this ) const
{
    //debug_report_scopy1("arg: do_deep_copy_for_this = %d",
    //			(int)do_deep_copy_for_this);

    mdarray *thisp = (mdarray *)this;

    /* 念のため... */
    if ( this->__copying == true ) {
	err_report(__FUNCTION__,"WARNING","This should not be reported");
    }

    /* shallow copy の src がまだ生きている場合(dest 側の処理)，*/
    /* shallow copy のキャンセル処理を行なう */
    if ( thisp->shallow_copy_src_obj != NULL ) {

	const mdarray &_src_obj = *(thisp->shallow_copy_src_obj);
	mdarray *_dest_obj = thisp;

	/* shallow copy をキャンセルし，強制初期化 */
	_src_obj.cancel_shallow_copy(_dest_obj);
	_dest_obj->__force_init(_dest_obj->size_type(), false);

	if ( do_deep_copy_for_this == true ) {
	    /* deep copy を実行 */
	    _dest_obj->__deep_init(_src_obj);
	}
    }
    /* dest_obj が shallow copy の src になっていた場合，切りはなす */
    else if ( thisp->shallow_copy_dest_obj != NULL ) {

	const mdarray &_src_obj = *thisp;
	mdarray *_dest_obj = thisp->shallow_copy_dest_obj;

	/* shallow copy をキャンセルし，強制初期化 */
	_src_obj.cancel_shallow_copy(_dest_obj);
	_dest_obj->__force_init(_dest_obj->size_type(), false);

	/* deep copy を実行 */
	_dest_obj->__deep_init(_src_obj);
    }

    return;
}

/**
 * @brief  obj の内容を自身にコピー (shallow copy; 超低レベル)
 *
 *  shallow copy を行なう場合か，データの移動を行なう場合に利用する．
 *
 * @param   obj コピー元オブジェクトのアドレス
 * @param   is_move データコンテンツの「移動」の場合，true をセット
 * @return  自身の参照
 * @note   メンバ変数を完全に初期化・コピーする．バッファはアドレスだけがコピー
 *         される．ただし this->shallow_copy_src_obj だけは消去されない．
 *         バッファを全て開放し，単純にバッファを share してしまうので，使用に
 *         は注意が必要．<br>
 *         このメンバ関数を使う前に shallow copy 関係をクリーンにしておく．<br>
 *         このメンバ関数は private です．
 */
mdarray &mdarray::__shallow_init( const mdarray &obj, bool is_move )
{
    /*
     *  ここでは，this でも obj でも cleanup_shallow_copy() を使っているもの
     *  を呼ばないよう注意．
     */

    debug_report_scopy1("arg: obj = %zx",(size_t)(&obj));

    mdarray *const obj_bak = this->shallow_copy_src_obj;

    if ( is_move == false ) {
	/* いわゆる shallow copy の場合．外部ptrの使用は禁止される */
	if ( this->extptr_rec != NULL || 
	     this->extptr_2d_rec != NULL || this->extptr_3d_rec != NULL ) {
	    err_throw(__FUNCTION__,"FATAL",
		      "Internal error: cannot perform shallow copy");
	}
    }

    /* 強制初期化 */
    this->__force_free();
    this->__force_init(this->size_type(), false);

    mdarray *objp = (mdarray *)(&obj);
    objp->__copying = true;
    this->__copying = true;

    try {

	/* アドレスだけコピー */
	this->_default_rec = obj._default_rec;
	this->_size_rec = obj._size_rec;
	this->_arr_rec = obj._arr_rec;
	this->arr_alloc_blen_rec = obj.arr_alloc_blen_rec;
	this->_arr_ptr_2d_rec = obj._arr_ptr_2d_rec;
	this->_arr_ptr_3d_rec = obj._arr_ptr_3d_rec;

	this->init_properties(obj);

	this->sz_type_rec = obj.sz_type_rec;
	this->dim_size_rec = obj.dim_size_rec;

	c_memcpy(this->move_from_idx_rec, obj.move_from_idx_rec,
		 sizeof(this->move_from_idx_rec));
	c_memcpy(this->move_from_len_rec, obj.move_from_len_rec,
		 sizeof(this->move_from_len_rec));

	/* update cached length info and call user's callback func */
	this->update_length_info();

	if ( is_move == true ) {

	    /* この部分は，データの行き先が確定している「移動」の場合しか */
	    /* 許されない                                                 */

	    /* 外部ポインタの更新 */

	    if ( this->extptr_rec != NULL ) 
		*(this->extptr_rec) = this->_arr_rec;

	    if ( this->extptr_2d_rec != NULL ) {
		if ( this->_arr_ptr_2d_rec == NULL ) {
		    this->_update_arr_ptr_2d();
		}
		else {
		    *(this->extptr_2d_rec) = this->_arr_ptr_2d_rec;
		}
	    }

	    if ( this->extptr_3d_rec != NULL ) {
		if ( this->_arr_ptr_3d_rec == NULL ) {
		    this->_update_arr_ptr_3d();
		}
		else {
		    *(this->extptr_3d_rec) = this->_arr_ptr_3d_rec;
		}
	    }

	}

    }
    catch (...) {
	objp->__copying = false;
	this->__copying = false;
	this->shallow_copy_src_obj = obj_bak;
	err_throw(__FUNCTION__,"FATAL","caught exception");
    }
    objp->__copying = false;
    this->__copying = false;
    this->shallow_copy_src_obj = obj_bak;

    return *this;
}

/**
 * @brief  obj の内容を自身にコピー (deep copy; 超低レベル)
 *
 * @param   obj コピー元オブジェクトのアドレス
 * @return  自身の参照
 * @note   obj 側については，_defalut_rec などを直接みにいくので注意．<br>
 *         このメンバ関数を使う前に shallow copy 関係をクリーンにしておく
 *         事．<br>
 *         このメンバ関数は private です．
 */
mdarray &mdarray::__deep_init( const mdarray &obj )
{
    /*
     *  ここでは，obj 側で cleanup_shallow_copy() を使っているもの
     *  を呼ばないよう注意．
     */

    bool needs_update_2d_ptr = true;
    bool needs_update_3d_ptr = true;

    debug_report_scopy1("arg: obj = %zx",(size_t)(&obj));

    mdarray *objp = (mdarray *)(&obj);
    objp->__copying = true;

    try {

	/* 型と全配列長が等しい場合は dest_objの現在のバッファをそのまま使う */
	if ( this->size_type() == obj.size_type() &&
	     this->length() == obj.length() ) {

	    bool do_update = true;

	    /* デフォルト値をコピー */
	    this->assign_default( obj._default_rec );

	    /* 配列も同じかどうかチェック */
	    if ( this->dim_length() == obj.dim_length() ) {
		size_t i;
		for ( i=0 ; i < obj.dim_length() ; i++ ) {
		    if ( this->length(i) != obj.length(i) ) break;
		}
		if ( i == obj.dim_length() ) {	     /* 全次元の配列数が一致 */
		    do_update = false;
		    needs_update_2d_ptr = false;
		    needs_update_3d_ptr = false;
		}
	    }
	    if ( do_update == true ) {
		/* 全配列長が同じなので，ここでは配列情報だけ更新される */
		this->reallocate(obj._size_rec, obj.dim_length(), false);
	    }
	
	}
	else {

	    /* 強制初期化 */
	    this->__force_free();
	    this->__force_init(this->size_type(), false);    /* keep sz_type */

	    if ( obj._default_rec != NULL ) {
		if ( this->realloc_default_rec(obj.bytes()) < 0 ) {
		    err_throw(__FUNCTION__,"FATAL","realloc() failed");
		}
		c_memcpy(this->default_rec_ptr(), obj._default_rec, obj.bytes());
	    }

	    if ( 0 < obj.dim_length() ) {
		size_t i;
		if ( this->realloc_size_rec(obj.dim_length()) < 0 ) {
		    this->free_default_rec();
		    err_throw(__FUNCTION__,"FATAL","realloc() failed");
		}
		for ( i=0 ; i < obj.dim_length() ; i++ ) {
		    size_t *srec_ptr = this->size_rec_ptr();
		    srec_ptr[i] = obj._size_rec[i];
		}
	    }

	    if ( 0 < obj.length() ) {
		if ( this->realloc_arr_rec(obj.byte_length()) < 0 ) {
		    this->free_default_rec();
		    this->free_size_rec();
		    err_throw(__FUNCTION__,"FATAL","realloc() failed");
		}
	    }
	}

	if ( 0 < obj.length() ) {
	    s_memcpy(this->_arr_rec, obj._arr_rec, obj.byte_length(), 0);
	}
    
	this->init_properties(obj);

	this->sz_type_rec = obj.sz_type_rec;
	this->dim_size_rec = obj.dim_size_rec;

	c_memcpy(this->move_from_idx_rec, obj.move_from_idx_rec,
		 sizeof(this->move_from_idx_rec));
	c_memcpy(this->move_from_len_rec, obj.move_from_len_rec,
		 sizeof(this->move_from_len_rec));

	if ( this->extptr_rec != NULL ) *(this->extptr_rec) = this->_arr_rec;
	/* update cached length info and call user's callback func */
	this->update_length_info();

	/* update_length_info() の直後に呼ぶ必要がある */
	if ( needs_update_2d_ptr == true ) this->_update_arr_ptr_2d();
	if ( needs_update_3d_ptr == true ) this->_update_arr_ptr_3d();

    }
    catch (...) {
	objp->__copying = false;
	err_throw(__FUNCTION__,"FATAL","caught exception");
    }
    objp->__copying = false;

    return *this;
}


/**
 * @brief  オブジェクトのコピーまたは代入
 *
 *  オブジェクトを初期化し，obj の内容をすべて(配列長，型など) 自身にコピー
 *  します．obj が関数によって返されるテンポラリオブジェクトの場合，許可されて
 *  いれば shallow  copy が行なわれます．<br>
 *  自身がobjの型に変更される事が禁じられている場合，型以外のすべて(配列長や各
 *  種属性)をコピーし，要素値のコピーはスカラー値の「=」演算子と同様の規則で行
 *  なわれます．
 *
 * @param     obj mdarrayクラスのオブジェクト(コピー元)
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 */
mdarray &mdarray::operator=(const mdarray &obj)
{
    debug_report("(const mdarray &obj)");
    if ( &obj == this ) return *this;

    if ( this->is_acceptable_size_type(obj.sz_type_rec) == true ) {

	this->init(obj);

    }
    else {

	if ( obj.dim_length() == 0 ) this->init();
	else {
	    this->init(this->size_type(), true, 
		       obj.cdimarray(), obj.dim_length(), true);
	    this->ope_plus_equal(obj);
	}
	this->init_properties(obj);

    }

    return *this;
}


/**
 * @brief  current と src の次元情報を比較して，大きい次元情報を返す (内部用)
 *
 * @param  enalrged current より src が大きくなっていれば true
 * @note   private な関数です．
 */
static int enlarged_dim_info( const mdarray &current,
			      const mdarray &src,
			      heap_mem<size_t> *ret_dim_size_info,
			      size_t *n_dim_info,
			      bool *enlarged )
{
    int ret_status = -1;
    heap_mem<size_t> &dim_size_info = *(ret_dim_size_info);
    size_t larger_ndim, i;
    bool needs_resize = false;
    if ( current.dim_length() < src.dim_length() ) {
	needs_resize = true;
	larger_ndim = src.dim_length();
    }
    else {
	larger_ndim = current.dim_length();
    }
    if ( dim_size_info.allocate(larger_ndim) < 0 ) {
	goto quit;	/* malloc() error */
    }
    for ( i=0 ; i < larger_ndim ; i++ ) {
	if ( current.length(i) < src.length(i) ) {
	    needs_resize = true;
	    dim_size_info[i] = src.length(i);
	}
	else dim_size_info[i] = current.length(i);
    }
    *n_dim_info = larger_ndim;
    if ( enlarged != NULL ) *enlarged = needs_resize;
    ret_status = 0;
 quit:
    return ret_status;
}


/* この関数は再帰的に呼ばれる */

/**
 * @brief  mdarray の再帰演算処理 (+=,-=,*=,/=) (内部用)
 *
 *  1. 次元!=0ならば，再帰的にこの関数を呼出す．<br>
 *  2. 与えられた演算用関数を利用して，src_obj と dest_obj とを演算する．<br>
 *  3. 2.を，要素の数だけ繰り返す．
 *
 * @param  src_obj   演算対象の mdarrayオブジェクト (read)
 * @param  dest_obj  演算対象の mdarrayオブジェクト (write)
 * @param  dim_idx   次元番号
 * @param  len_src   objの要素の個数
 * @param  len_dest  自身の要素の個数
 * @param  pos_src   データ読み込み位置
 * @param  pos_dest  データ書き込み位置
 * @param  func_calc 演算用関数のポインタ
 * @note   private な関数です．
 */
static void x_equal_r( const mdarray &src_obj, mdarray *dest_obj, int ope,
		      void (*func_calc)(void *,const void *,size_t,size_t,int),
		      size_t dim_idx,
		      size_t len_src, size_t len_dest,
		      size_t pos_src, size_t pos_dest )
{
    size_t len;
    len = dest_obj->length(dim_idx);
    if ( src_obj.length(dim_idx) < len ) len = src_obj.length(dim_idx);
    if ( 0 < dim_idx ) {
	size_t i;
	len_src /= src_obj.length(dim_idx);
	len_dest /= dest_obj->length(dim_idx);
	for ( i=0 ; i < len ; i++ ) {
	    x_equal_r( src_obj, dest_obj, ope, func_calc, 
		       dim_idx-1, len_src, len_dest,
		       pos_src + len_src * i, pos_dest + len_dest * i );
	}
    }
    else {	/* 次元=0 での処理 */
	const size_t src_bytes = src_obj.bytes();
	const size_t dest_bytes = dest_obj->bytes();
	char *dest = (char *)(dest_obj->data_ptr()) + dest_bytes * pos_dest;
	const char *src =
	    (const char *)(src_obj.data_ptr_cs()) + src_bytes * pos_src;
	(*func_calc)((void *)dest, (const void *)src, 1, len, ope);
    }
    return;
}

static const int Ope_plus  = 0;
static const int Ope_minus = 1;
static const int Ope_star  = 2;
static const int Ope_slash = 3;

namespace xeq_n
{
/* _step0 が 0 ならスカラーとの演算，1 なら配列との演算 */
#define MAKE_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
static void fncname(void *_dst, const void *_src0, size_t _step0, size_t n, int ope) \
{ \
    size_t i=0; \
    new_type *dst = (new_type *)_dst; \
    const org_type *src0 = (const org_type *)_src0; \
    const size_t step0 = _step0; \
    if ( ope == Ope_plus )       { for ( ; i < n ; i++,src0+=step0 ) dst[i] += (new_type)fnc(*src0); } \
    else if ( ope == Ope_minus ) { for ( ; i < n ; i++,src0+=step0 ) dst[i] -= (new_type)fnc(*src0); } \
    else if ( ope == Ope_star )  { for ( ; i < n ; i++,src0+=step0 ) dst[i] *= (new_type)fnc(*src0); } \
    else			 { for ( ; i < n ; i++,src0+=step0 ) dst[i] /= (new_type)fnc(*src0); } \
}
SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(MAKE_FUNC,,,,,,,,,,,,,creal,creal,creal,creal,creal,creal,creal,creal,creal,creal,creal,creal,f2fx,d2dx,ld2ldx,);
#undef MAKE_FUNC
}

/**
 * @brief  自身の配列に，指定された配列を加算 (低レベル)
 *
 *  自身の配列にobjの配列を加算します．<br>
 *   1. オートリサイズの場合，与えられた mdarray の要素数に合わせて，要素数を
 *      リサイズする．<br>
 *   2. 加算処理のための関数を選択する(SEL_FUNC)．<br>
 *   3. 再帰演算処理の関数(x_equal_r)を呼出す．
 *
 * @param    obj 加算対象の mdarray
 * @return   自身の参照
 * @throw    内部バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 * @note     このメンバ関数は protected です．
 */
mdarray &mdarray::ope_plus_equal(const mdarray &obj)
{
    return this->ope_plus_equal(obj, this->auto_resize());
}

/**
 * @brief  自身の配列に，指定された配列を加算 (低レベル)
 * @note   このメンバ関数は protected です．
 */
mdarray &mdarray::ope_plus_equal(const mdarray &obj, bool do_resize)
{
    void (*func_calc)(void *,const void *,size_t,size_t,int);

    if ( obj.length() == 0 ) return *this;

    if ( do_resize == true ) {
	heap_mem<size_t> tmp_size;
	size_t larger_ndim;
	bool needs_resize;
	if ( enlarged_dim_info( *this, obj, &tmp_size, 
				&larger_ndim, &needs_resize ) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","malloc() failed");
	}
	if ( needs_resize == true ) {
	    this->resize(tmp_size.ptr(), larger_ndim, true);
	}
    }

    if ( this->length() == 0 ) return *this;

    func_calc = NULL;

    /* 関数を選択 */
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
    if ( obj.sz_type_rec == org_sz_type && this->sz_type_rec == new_sz_type ) { \
	func_calc = &xeq_n::fncname; \
    }
    SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(SEL_FUNC,,,,,,,,,,,,,,,,,,,,,,,,,,,,else);
#undef SEL_FUNC

    /* 関数があれば代入を行なう */
    if ( func_calc != NULL ) {
	size_t len_src, len_dest, ndim;

	/* 大きい方をとる */
	ndim = this->dim_length();
	if ( obj.dim_length() < ndim ) ndim = obj.dim_length();

	/* 最初は，扱う次元内の全ピクセル数を len_xxx にセットする */
	len_src = obj.length();
	len_dest = this->length();

	x_equal_r( obj, this, Ope_plus, func_calc, 
		   ndim-1, len_src, len_dest, 0, 0 );
    }
    else {
	err_report(__FUNCTION__,"WARNING","invalid size of type; ignored");
    }

    return *this;
}

/**
 * @brief  自身の配列に，指定された配列を加算
 *
 *  演算子の右側(引数) で指定された mdarray クラスのオブジェクトの配列を自身に
 *  加算します．<br>
 *  自身とは異なる型が設定されたオブジェクトが指定された場合，通常のスカラー
 *  演算の場合と同様の型変換処理が行なわれます．<br>
 * 「自動リサイズモード」が設定されている場合，各次元サイズに関して自身よりも
 *  obj の方が大きい場合，自動的にリサイズします．
 *
 * @param     mdarray クラスのオブジェクト
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 */
mdarray &mdarray::operator+=(const mdarray &obj)
{
    debug_report("(const mdarray &obj)");
    return this->ope_plus_equal(obj);
}


/**
 * @brief  自身の配列を，指定された配列で減算 (低レベル)
 *
 *  自身の配列をobjの配列で減算します．<br>
 *   1. オートリサイズの場合，与えられた mdarray の要素数に合わせて，要素数を
 *      リサイズする．<br>
 *   2. 減算処理のための関数を選択する(SEL_FUNC)．<br>
 *   3. 再帰演算処理の関数(x_equal_r)を呼出す．
 *
 * @param    obj 減算対象の mdarrayオブジェクト
 * @return   自身の参照
 * @throw    内部バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 * @note     このメンバ関数は protected です．
 */
mdarray &mdarray::ope_minus_equal(const mdarray &obj)
{
    return this->ope_minus_equal(obj, this->auto_resize());
}

/**
 * @brief  自身の配列を，指定された配列で減算 (低レベル)
 * @note   このメンバ関数は protected です．
 */
mdarray &mdarray::ope_minus_equal(const mdarray &obj, bool do_resize)
{
    void (*func_calc)(void *,const void *,size_t,size_t,int);

    if ( obj.length() == 0 ) return *this;

    if ( do_resize == true ) {
	heap_mem<size_t> tmp_size;
	size_t larger_ndim;
	bool needs_resize;
	if ( enlarged_dim_info( *this, obj, &tmp_size, 
				&larger_ndim, &needs_resize ) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","malloc() failed");
	}
	if ( needs_resize == true ) {
	    this->resize(tmp_size.ptr(), larger_ndim, true);
	}
    }

    if ( this->length() == 0 ) return *this;

    func_calc = NULL;

    /* 関数を選択 */
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
    if ( obj.sz_type_rec == org_sz_type && this->sz_type_rec == new_sz_type ) { \
	func_calc = &xeq_n::fncname; \
    }
    SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(SEL_FUNC,,,,,,,,,,,,,,,,,,,,,,,,,,,,else);
#undef SEL_FUNC

    /* 関数があれば代入を行なう */
    if ( func_calc != NULL ) {
	size_t len_src, len_dest, ndim;

	/* 大きい方をとる */
	ndim = this->dim_length();
	if ( obj.dim_length() < ndim ) ndim = obj.dim_length();

	/* 最初は，扱う次元内の全ピクセル数を len_xxx にセットする */
	len_src = obj.length();
	len_dest = this->length();
	
	x_equal_r( obj, this, Ope_minus, func_calc,
		   ndim-1, len_src, len_dest, 0, 0 );
    }
    else {
	err_report(__FUNCTION__,"WARNING","invalid size of type; ignored");
    }

    return *this;
}

/**
 * @brief  自身の配列を，指定された配列で減算
 *
 *  演算子の右側(引数) で指定された mdarray クラスのオブジェクトの配列を自身
 *  から減算します．<br>
 *  自身とは異なる型が設定されたオブジェクトが指定された場合，通常のスカラー
 *  演算の場合と同様の型変換処理が行なわれます．<br>
 *  「自動リサイズモード」が設定されている場合，各次元サイズに関して自身よりも
 *  obj の方が大きい場合，自動的にリサイズします．
 *
 * @param     mdarray クラスのオブジェクト
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 */
mdarray &mdarray::operator-=(const mdarray &obj)
{
    debug_report("(const mdarray &obj)");
    return this->ope_minus_equal(obj);
}


/**
 * @brief  自身の配列に，指定された配列を乗算 (低レベル)
 *
 *  自身の配列をobjの配列で乗算します．<br>
 *   1. オートリサイズの場合，与えられた mdarray の要素数に合わせて，要素数を
 *      リサイズする．<br>
 *   2. 乗算処理のための関数を選択する(SEL_FUNC)．<br>
 *   3. 再帰演算処理の関数(x_equal_r)を呼出す．
 *
 * @param    obj 乗算対象の mdarrayオブジェクト
 * @return   自身の参照
 * @throw    内部バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 * @note     このメンバ関数は protected です．
 */
mdarray &mdarray::ope_star_equal(const mdarray &obj)
{
    return this->ope_star_equal(obj, this->auto_resize());
}

/**
 * @brief  自身の配列に，指定された配列を乗算 (低レベル)
 * @note   このメンバ関数は protected です．
 */
mdarray &mdarray::ope_star_equal(const mdarray &obj, bool do_resize)
{
    void (*func_calc)(void *,const void *,size_t,size_t,int);

    if ( obj.length() == 0 ) return *this;

    if ( do_resize == true ) {
	heap_mem<size_t> tmp_size;
	size_t larger_ndim;
	bool needs_resize;
	if ( enlarged_dim_info( *this, obj, &tmp_size, 
				&larger_ndim, &needs_resize ) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","malloc() failed");
	}
	if ( needs_resize == true ) {
	    this->resize(tmp_size.ptr(), larger_ndim, true);
	}
    }

    if ( this->length() == 0 ) return *this;

    func_calc = NULL;

    /* 関数を選択 */
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
    if ( obj.sz_type_rec == org_sz_type && this->sz_type_rec == new_sz_type ) { \
	func_calc = &xeq_n::fncname; \
    }
    SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(SEL_FUNC,,,,,,,,,,,,,,,,,,,,,,,,,,,,else);
#undef SEL_FUNC

    /* 関数があれば代入を行なう */
    if ( func_calc != NULL ) {
	size_t len_src, len_dest, ndim;

	/* 大きい方をとる */
	ndim = this->dim_length();
	if ( obj.dim_length() < ndim ) ndim = obj.dim_length();

	/* 最初は，扱う次元内の全ピクセル数を len_xxx にセットする */
	len_src = obj.length();
	len_dest = this->length();
	
	x_equal_r( obj, this, Ope_star, func_calc, 
		   ndim-1, len_src, len_dest, 0, 0 );
    }
    else {
	err_report(__FUNCTION__,"WARNING","invalid size of type; ignored");
    }

    return *this;
}

/**
 * @brief  自身の配列に，指定された配列を乗算
 *
 *  演算子の右側(引数) で指定された mdarray クラスのオブジェクトの配列を自身に
 *  乗算します．<br>
 *  自身とは異なる型が設定されたオブジェクトが指定された場合，通常のスカラー
 *  演算の場合と同様の型変換処理が行なわれます．<br>
 *  「自動リサイズモード」が設定されている場合，各次元サイズに関して自身よりも
 *  obj の方が大きい場合，自動的にリサイズします．
 *
 * @param     mdarray クラスのオブジェクト
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 */
mdarray &mdarray::operator*=(const mdarray &obj)
{
    debug_report("(const mdarray &obj)");
    return this->ope_star_equal(obj);
}


/**
 * @brief  自身の配列を，指定された配列で除算 (低レベル)
 *
 *  自身の配列をobjの配列で除算します．<br>
 *   1. オートリサイズの場合，与えられた mdarray の要素数に合わせて，要素数を
 *      リサイズする．<br>
 *   2. 除算処理のための関数を選択する(SEL_FUNC)．<br>
 *   3. 再帰演算処理の関数(x_equal_r)を呼出す．
 *
 * @param    obj 除算対象の mdarrayオブジェクト
 * @return   自身の参照
 * @throw    内部バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 * @note     このメンバ関数は protected です．
 */
mdarray &mdarray::ope_slash_equal(const mdarray &obj)
{
    return this->ope_slash_equal(obj, this->auto_resize());
}

/**
 * @brief  自身の配列を，指定された配列で除算 (低レベル)
 * @note   このメンバ関数は protected です．
 */
mdarray &mdarray::ope_slash_equal(const mdarray &obj, bool do_resize)
{
    void (*func_calc)(void *,const void *,size_t,size_t,int);

    if ( obj.length() == 0 ) return *this;

    if ( do_resize == true ) {
	heap_mem<size_t> tmp_size;
	size_t larger_ndim;
	bool needs_resize;
	if ( enlarged_dim_info( *this, obj, &tmp_size, 
				&larger_ndim, &needs_resize ) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","malloc() failed");
	}
	if ( needs_resize == true ) {
	    this->resize(tmp_size.ptr(), larger_ndim, true);
	}
    }

    if ( this->length() == 0 ) return *this;

    func_calc = NULL;

    /* 関数を選択 */
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
    if ( obj.sz_type_rec == org_sz_type && this->sz_type_rec == new_sz_type ) { \
	func_calc = &xeq_n::fncname; \
    }
    SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(SEL_FUNC,,,,,,,,,,,,,,,,,,,,,,,,,,,,else);
#undef SEL_FUNC

    /* 関数があれば代入を行なう */
    if ( func_calc != NULL ) {
	size_t len_src, len_dest, ndim;

	/* 大きい方をとる */
	ndim = this->dim_length();
	if ( obj.dim_length() < ndim ) ndim = obj.dim_length();

	/* 最初は，扱う次元内の全ピクセル数を len_xxx にセットする */
	len_src = obj.length();
	len_dest = this->length();
	
	x_equal_r( obj, this, Ope_slash, func_calc, 
		   ndim-1, len_src, len_dest, 0, 0 );
    }
    else {
	err_report(__FUNCTION__,"WARNING","invalid size of type; ignored");
    }

    return *this;
}

/**
 * @brief  自身の配列を，指定された配列で除算
 *
 *  演算子の右側(引数) で指定された mdarray クラスのオブジェクトの配列を自身
 *  から除算します．<br>
 *  自身とは異なる型が設定されたオブジェクトが指定された場合，通常のスカラー
 *  演算の場合と同様の型変換処理が行なわれます．<br>
 *  「自動リサイズモード」が設定されている場合，各次元サイズに関して自身よりも
 *  obj の方が大きい場合，自動的にリサイズします．
 *
 * @param     obj mdarray クラスのオブジェクト
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 */
mdarray &mdarray::operator/=(const mdarray &obj)
{
    debug_report("(const mdarray &obj)");
    return this->ope_slash_equal(obj);
}

/**
 * @brief  自身の配列に，スカラー値を代入 (低レベル)
 *
 *  スカラー値を自身のすべての要素に代入します．
 *  自動的なサイズ拡張は行いません．<br>
 *   1. 与えられた型種別と，現在の mdarray が持つ型種別から，適切な「=」処理用
 *      の関数を取得する(SEL_FUNC)．<br>
 *   2. mdarray の全要素を対象として，上記で取得した「=」処理用の関数を用いて，
 *      与えられた変数を，現在の mdarray が持つ変数に「=」演算する．
 *
 * @param    szt   スカラー値の型を示す値(型種別)
 * @param    v_ptr スカラー値のアドレス
 * @return   自身の参照
 * @note     このメンバ関数は protected です．
 */
mdarray &mdarray::ope_equal(ssize_t szt, const void *v_ptr)
{
    void (*func_d2dest)(const void *,void *,size_t,int,void *);
    size_t len = this->length();

    func_d2dest = NULL;
    /* sztな型をアレイの型に変換する関数を選択 */
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
    if ( szt == org_sz_type && this->sz_type_rec == new_sz_type ) { \
	func_d2dest = &cnv_nd::fncname; \
    }
    SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(SEL_FUNC,,,,,,,,,,,,,,,,,,,,,,,,,,,,else);
#undef SEL_FUNC

    if ( func_d2dest == NULL ) {
	err_report(__FUNCTION__,"WARNING","invalid size of type; ignored");
    }
    else if ( 0 < len ) {
	struct mdarray_cnv_nd_prms prms;
	prms.total_len_elements = 0;		/* SSE2 用．一括なので0 */
	prms.round_flg = 0;			/* 切り捨てで変換 */
	(*func_d2dest)(v_ptr, this->data_ptr(), len, 0, (void *)&prms);
    }
    return *this;
}

/**
 * @brief  自身の配列に，スカラー値を代入
 *
 *  演算子の右側(引数) で指定された数値(スカラー値) を代入をします．<br>
 *  自動的なサイズ拡張は行いません．予め要素数を設定し，バッファを確保する必要
 *  があります．
 *
 * @param     v スカラー値
 * @return    自身の参照
 */
mdarray &mdarray::operator=(dcomplex v)
{
    debug_report("(dcomplex v)");
    return this->ope_equal(DCOMPLEX_ZT,(const void *)&v);
}

/**
 * @brief  自身の配列に，スカラー値を代入
 *
 *  演算子の右側(引数) で指定された数値(スカラー値) を代入をします．<br>
 *  自動的なサイズ拡張は行いません．予め要素数を設定し，バッファを確保する必要
 *  があります．
 *
 * @param     v スカラー値
 * @return    自身の参照
 */
mdarray &mdarray::operator=(double v)
{
    debug_report("(double v)");
    return this->ope_equal(DOUBLE_ZT,(const void *)&v);
}

/**
 * @brief  自身の配列に，スカラー値を代入
 *
 *  演算子の右側(引数) で指定された数値(スカラー値) を代入をします．<br>
 *  自動的なサイズ拡張は行いません．予め要素数を設定し，バッファを確保する必要
 *  があります．
 *
 * @param     v スカラー値
 * @return    自身の参照
 */
mdarray &mdarray::operator=(long long v)
{
    debug_report("(long long v)");
    return this->ope_equal(sizeof(v),(const void *)&v);
}

/**
 * @brief  自身の配列に，スカラー値を代入
 *
 *  演算子の右側(引数) で指定された数値(スカラー値) を代入をします．<br>
 *  自動的なサイズ拡張は行いません．予め要素数を設定し，バッファを確保する必要
 *  があります．
 *
 * @param     v スカラー値
 * @return    自身の参照
 */
mdarray &mdarray::operator=(long v)
{
    debug_report("(long v)");
    return this->ope_equal(sizeof(v),(const void *)&v);
}

/**
 * @brief  自身の配列に，スカラー値を代入
 *
 *  演算子の右側(引数) で指定された数値(スカラー値) を代入をします．<br>
 *  自動的なサイズ拡張は行いません．予め要素数を設定し，バッファを確保する必要
 *  があります．
 *
 * @param     v スカラー値
 * @return    自身の参照
 */
mdarray &mdarray::operator=(int v)
{
    debug_report("(int v)");
    return this->ope_equal(sizeof(v),(const void *)&v);
}


/**
 * @brief  自身の配列を，スカラー値で加算 (低レベル)
 *
 *  自身の要素すべてに対して,スカラー値を加算します．<br>
 *   1. 加算処理のための関数を選択する(SEL_FUNC)．<br>
 *   2. 加算処理の関数を呼出し，全要素に対し加算する．
 *
 * @param    szt   スカラー値の型を示す値(型種別)
 * @param    v_ptr 加算対象のスカラー値のアドレス
 * @return   自身の参照
 * @note     このメンバ関数は protected です．
 */
mdarray &mdarray::ope_plus_equal(ssize_t szt, const void *v_ptr)
{
    void (*func_calc)(void *,const void *,size_t,size_t,int);
    func_calc = NULL;

    /* 関数を選択 */
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
    if ( szt == org_sz_type && this->sz_type_rec == new_sz_type ) { \
	func_calc = &xeq_n::fncname; \
    }
    SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(SEL_FUNC,,,,,,,,,,,,,,,,,,,,,,,,,,,,else);
#undef SEL_FUNC

    if ( func_calc == NULL ) {
	err_report(__FUNCTION__,"WARNING","invalid size of type; ignored");
    }
    else {
	const size_t all_len = this->length();
	(*func_calc)(this->data_ptr(),v_ptr,0,all_len,Ope_plus);
    }
    return *this;
}

/**
 * @brief  自身の配列を，スカラー値で加算
 *
 *  演算子の右側(引数) で指定されたスカラー値を自身の要素すべてに対して加算
 *  します．<br>
 *  自身とは型が異なる引数の場合，通常のスカラー演算の場合と同様の型変換処理が
 *  行なわれます．
 *
 * @param     v  スカラー値
 * @return    自身の参照
 */
mdarray &mdarray::operator+=(dcomplex v)
{
    debug_report("(complex v)");
    return this->ope_plus_equal(DCOMPLEX_ZT,(const void *)&v);
}

/**
 * @brief  自身の配列を，スカラー値で加算
 *
 *  演算子の右側(引数) で指定されたスカラー値を自身の要素すべてに対して加算
 *  します．<br>
 *  自身とは型が異なる引数の場合，通常のスカラー演算の場合と同様の型変換処理が
 *  行なわれます．
 *
 * @param     v  スカラー値
 * @return    自身の参照
 */
mdarray &mdarray::operator+=(double v)
{
    debug_report("(double v)");
    return this->ope_plus_equal(DOUBLE_ZT,(const void *)&v);
}

/**
 * @brief  自身の配列を，スカラー値で加算
 *
 *  演算子の右側(引数) で指定されたスカラー値を自身の要素すべてに対して加算
 *  します．<br>
 *  自身とは型が異なる引数の場合，通常のスカラー演算の場合と同様の型変換処理が
 *  行なわれます．
 *
 * @param     v  スカラー値
 * @return    自身の参照
 */
mdarray &mdarray::operator+=(long long v)
{
    debug_report("(long long v)");
    return this->ope_plus_equal(sizeof(v),(const void *)&v);
}

/**
 * @brief  自身の配列を，スカラー値で加算
 *
 *  演算子の右側(引数) で指定されたスカラー値を自身の要素すべてに対して加算
 *  します．<br>
 *  自身とは型が異なる引数の場合，通常のスカラー演算の場合と同様の型変換処理が
 *  行なわれます．
 *
 * @param     v  スカラー値
 * @return    自身の参照
 */
mdarray &mdarray::operator+=(long v)
{
    debug_report("(long v)");
    return this->ope_plus_equal(sizeof(v),(const void *)&v);
}

/**
 * @brief  自身の配列を，スカラー値で加算
 *
 *  演算子の右側(引数) で指定されたスカラー値を自身の要素すべてに対して加算
 *  します．<br>
 *  自身とは型が異なる引数の場合，通常のスカラー演算の場合と同様の型変換処理が
 *  行なわれます．
 *
 * @param     v  スカラー値
 * @return    自身の参照
 */
mdarray &mdarray::operator+=(int v)
{
    debug_report("(int v)");
    return this->ope_plus_equal(sizeof(v),(const void *)&v);
}


/**
 * @brief  自身の配列を，スカラー値で減算 (低レベル)
 *
 *  自身の要素すべてに対して,スカラー値で減算を行います．<br>
 *   1. 減算処理のための関数を選択する(SEL_FUNC)．<br>
 *   2. 減算処理の関数を呼出し，全要素に対し減算する．
 *
 * @param    szt   スカラー値の型を示す値(型種別)
 * @param    v_ptr 減算対象のスカラー値のアドレス
 * @return   自身の参照
 * @note     このメンバ関数は protected です．
 */
mdarray &mdarray::ope_minus_equal(ssize_t szt, const void *v_ptr)
{
    void (*func_calc)(void *,const void *,size_t,size_t,int);
    func_calc = NULL;

    /* 関数を選択 */
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
    if ( szt == org_sz_type && this->sz_type_rec == new_sz_type ) { \
	func_calc = &xeq_n::fncname; \
    }
    SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(SEL_FUNC,,,,,,,,,,,,,,,,,,,,,,,,,,,,else);
#undef SEL_FUNC

    if ( func_calc == NULL ) {
	err_report(__FUNCTION__,"WARNING","invalid size of type; ignored");
    }
    else {
	const size_t all_len = this->length();
	(*func_calc)(this->data_ptr(),v_ptr,0,all_len,Ope_minus);
    }
    return *this;
}

/**
 * @brief  自身の配列を，スカラー値で減算
 *
 *  自身の要素すべてに対して，演算子の右側(引数) で指定されたスカラー値で減算
 *  します．<br>
 *  自身とは型が異なる引数の場合，通常のスカラー演算の場合と同様の型変換処理が
 *  行なわれます．
 *
 * @param     v スカラー値
 * @return    自身の参照
 */
mdarray &mdarray::operator-=(dcomplex v)
{
    debug_report("(complex v)");
    return this->ope_minus_equal(DCOMPLEX_ZT,(const void *)&v);
}

/**
 * @brief  自身の配列を，スカラー値で減算
 *
 *  自身の要素すべてに対して，演算子の右側(引数) で指定されたスカラー値で減算
 *  します．<br>
 *  自身とは型が異なる引数の場合，通常のスカラー演算の場合と同様の型変換処理が
 *  行なわれます．
 *
 * @param     v スカラー値
 * @return    自身の参照
 */
mdarray &mdarray::operator-=(double v)
{
    debug_report("(double v)");
    return this->ope_minus_equal(DOUBLE_ZT,(const void *)&v);
}

/**
 * @brief  自身の配列を，スカラー値で減算
 *
 *  自身の要素すべてに対して，演算子の右側(引数) で指定されたスカラー値で減算
 *  します．<br>
 *  自身とは型が異なる引数の場合，通常のスカラー演算の場合と同様の型変換処理が
 *  行なわれます．
 *
 * @param     v スカラー値
 * @return    自身の参照
 */
mdarray &mdarray::operator-=(long long v)
{
    debug_report("(long long v)");
    return this->ope_minus_equal(sizeof(v),(const void *)&v);
}

/**
 * @brief  自身の配列を，スカラー値で減算
 *
 *  自身の要素すべてに対して，演算子の右側(引数) で指定されたスカラー値で減算
 *  します．<br>
 *  自身とは型が異なる引数の場合，通常のスカラー演算の場合と同様の型変換処理が
 *  行なわれます．
 *
 * @param     v スカラー値
 * @return    自身の参照
 *
 */
mdarray &mdarray::operator-=(long v)
{
    debug_report("(long v)");
    return this->ope_minus_equal(sizeof(v),(const void *)&v);
}

/**
 * @brief  自身の配列を，スカラー値で減算
 *
 *  自身の要素すべてに対して，演算子の右側(引数) で指定されたスカラー値で減算
 *  します．<br>
 *  自身とは型が異なる引数の場合，通常のスカラー演算の場合と同様の型変換処理が
 *  行なわれます．
 *
 * @param     v スカラー値
 * @return    自身の参照
 */
mdarray &mdarray::operator-=(int v)
{
    debug_report("(int v)");
    return this->ope_minus_equal(sizeof(v),(const void *)&v);
}

/**
 * @brief  自身の配列に，スカラー値を乗算 (低レベル)
 *
 *  自身の要素すべてに対して,スカラー値で乗算します．<br>
 *   1. 乗算処理のための関数を選択する(SEL_FUNC)．<br>
 *   2. 乗算処理の関数を呼出し，全要素に対し乗算する．
 *
 * @param    szt   スカラー値の型を示す値(型種別)
 * @param    v_ptr 乗算対象のスカラー値のアドレス
 * @return   自身の参照
 * @note     このメンバ関数は protected です．
 */
mdarray &mdarray::ope_star_equal(ssize_t szt, const void *v_ptr)
{
    void (*func_calc)(void *,const void *,size_t,size_t,int);
    func_calc = NULL;

    /* 関数を選択 */
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
    if ( szt == org_sz_type && this->sz_type_rec == new_sz_type ) { \
	func_calc = &xeq_n::fncname; \
    }
    SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(SEL_FUNC,,,,,,,,,,,,,,,,,,,,,,,,,,,,else);
#undef SEL_FUNC

    if ( func_calc == NULL ) {
	err_report(__FUNCTION__,"WARNING","invalid size of type; ignored");
    }
    else {
	const size_t all_len = this->length();
	(*func_calc)(this->data_ptr(),v_ptr,0,all_len,Ope_star);
    }
    return *this;
}

/**
 * @brief  自身の配列に，スカラー値を乗算
 *
 *  演算子の右側(引数) で指定されたスカラー値を自身の要素すべてに対して乗算
 *  します．<br>
 *  自身とは型が異なる引数の場合，通常のスカラー演算の場合と同様の型変換処理が
 *  行なわれます．
 *
 * @param     v スカラー値
 * @return    自身の参照
 */
mdarray &mdarray::operator*=(dcomplex v)
{
    debug_report("(complex v)");
    return this->ope_star_equal(DCOMPLEX_ZT,(const void *)&v);
}

/**
 * @brief  自身の配列に，スカラー値を乗算
 *
 *  演算子の右側(引数) で指定されたスカラー値を自身の要素すべてに対して乗算
 *  します．<br>
 *  自身とは型が異なる引数の場合，通常のスカラー演算の場合と同様の型変換処理が
 *  行なわれます．
 *
 * @param     v スカラー値
 * @return    自身の参照
 */
mdarray &mdarray::operator*=(double v)
{
    debug_report("(double v)");
    return this->ope_star_equal(DOUBLE_ZT,(const void *)&v);
}

/**
 * @brief  自身の配列に，スカラー値を乗算
 *
 *  演算子の右側(引数) で指定されたスカラー値を自身の要素すべてに対して乗算
 *  します．<br>
 *  自身とは型が異なる引数の場合，通常のスカラー演算の場合と同様の型変換処理が
 *  行なわれます．
 *
 * @param     v スカラー値
 * @return    自身の参照
 */
mdarray &mdarray::operator*=(long long v)
{
    debug_report("(long long v)");
    return this->ope_star_equal(sizeof(v),(const void *)&v);
}

/**
 * @brief  自身の配列に，スカラー値を乗算
 *
 *  演算子の右側(引数) で指定されたスカラー値を自身の要素すべてに対して乗算
 *  します．<br>
 *  自身とは型が異なる引数の場合，通常のスカラー演算の場合と同様の型変換処理が
 *  行なわれます．
 *
 * @param     v スカラー値
 * @return    自身の参照
 */
mdarray &mdarray::operator*=(long v)
{
    debug_report("(long v)");
    return this->ope_star_equal(sizeof(v),(const void *)&v);
}

/**
 * @brief  自身の配列に，スカラー値を乗算
 *
 *  演算子の右側(引数) で指定されたスカラー値を自身の要素すべてに対して乗算
 *  します．<br>
 *  自身とは型が異なる引数の場合，通常のスカラー演算の場合と同様の型変換処理が
 *  行なわれます．
 *
 * @param     v スカラー値
 * @return    自身の参照
 */
mdarray &mdarray::operator*=(int v)
{
    debug_report("(int v)");
    return this->ope_star_equal(sizeof(v),(const void *)&v);
}


/**
 * @brief  自身の配列を，スカラー値で除算 (低レベル)
 *
 *  自身の要素すべてに対して，スカラー値で除算します．<br>
 *   1. 除算処理のための関数を選択する(SEL_FUNC)．<br>
 *   2. 除算処理の関数を呼出し，全要素に対し除算する．
 *
 * @param    szt   スカラー値の型を示す値(型種別)
 * @param    v_ptr 除算対象のスカラー値のアドレス
 * @return   自身の参照
 * @note     このメンバ関数は protected です．
 */
mdarray &mdarray::ope_slash_equal(ssize_t szt, const void *v_ptr)
{
    void (*func_calc)(void *,const void *,size_t,size_t,int);
    func_calc = NULL;

    /* 関数を選択 */
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
    if ( szt == org_sz_type && this->sz_type_rec == new_sz_type ) { \
	func_calc = &xeq_n::fncname; \
    }
    SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(SEL_FUNC,,,,,,,,,,,,,,,,,,,,,,,,,,,,else);
#undef SEL_FUNC

    if ( func_calc == NULL ) {
	err_report(__FUNCTION__,"WARNING","invalid size of type; ignored");
    }
    else {
	const size_t all_len = this->length();
	(*func_calc)(this->data_ptr(),v_ptr,0,all_len,Ope_slash);
    }
    return *this;
}

/**
 * @brief  自身の配列を，スカラー値で除算
 *
 *  自身の要素すべてに対して，演算子の右側(引数) で指定されたスカラー値で除算
 *  します．<br>
 *  自身とは型が異なる引数の場合，通常のスカラー演算の場合と同様の型変換処理が
 *  行なわれます．
 *
 * @param     v スカラー値
 * @return    自身の参照
 */
mdarray &mdarray::operator/=(dcomplex v)
{
    debug_report("(complex v)");
    return this->ope_slash_equal(DCOMPLEX_ZT,(const void *)&v);
}

/**
 * @brief  自身の配列を，スカラー値で除算
 *
 *  自身の要素すべてに対して，演算子の右側(引数) で指定されたスカラー値で除算
 *  します．<br>
 *  自身とは型が異なる引数の場合，通常のスカラー演算の場合と同様の型変換処理が
 *  行なわれます．
 *
 * @param     v スカラー値
 * @return    自身の参照
 */
mdarray &mdarray::operator/=(double v)
{
    debug_report("(double v)");
    return this->ope_slash_equal(DOUBLE_ZT,(const void *)&v);
}

/**
 * @brief  自身の配列を，スカラー値で除算
 *
 *  自身の要素すべてに対して，演算子の右側(引数) で指定されたスカラー値で除算
 *  します．<br>
 *  自身とは型が異なる引数の場合，通常のスカラー演算の場合と同様の型変換処理が
 *  行なわれます．
 *
 * @param     v スカラー値
 * @return    自身の参照
 */
mdarray &mdarray::operator/=(long long v)
{
    debug_report("(long long v)");
    return this->ope_slash_equal(sizeof(v),(const void *)&v);
}

/**
 * @brief  自身の配列を，スカラー値で除算
 *
 *  自身の要素すべてに対して，演算子の右側(引数) で指定されたスカラー値で除算
 *  します．<br>
 *  自身とは型が異なる引数の場合，通常のスカラー演算の場合と同様の型変換処理が
 *  行なわれます．
 *
 * @param     v スカラー値
 * @return    自身の参照
 */
mdarray &mdarray::operator/=(long v)
{
    debug_report("(long v)");
    return this->ope_slash_equal(sizeof(v),(const void *)&v);
}

/**
 * @brief  自身の配列を，スカラー値で除算
 *
 *  自身の要素すべてに対して，演算子の右側(引数) で指定されたスカラー値で除算
 *  します．<br>
 *  自身とは型が異なる引数の場合，通常のスカラー演算の場合と同様の型変換処理が
 *  行なわれます．
 *
 * @param     v スカラー値
 * @return    自身の参照
 */
mdarray &mdarray::operator/=(int v)
{
    debug_report("(int v)");
    return this->ope_slash_equal(sizeof(v),(const void *)&v);
}

/**
 * @brief  自身の配列と指定されたオブジェクトの配列との比較 (低レベル)
 *
 *  自身とobjの配列が等しいかどうかを返します．<br>
 *  配列の型が異なっても配列長と値が等しければ真を返します. 
 *
 * @param   obj mdarrayクラスのオブジェクト
 * @return  true 配列サイズ,要素の値が一致した場合<br>
 *          false 配列サイズ,要素の値が不一致である場合
 * @note    このメンバ関数は protected です．
 */
bool mdarray::ope_equal_equal(const mdarray &obj) const
{
    return this->mdarray::compare(obj);
}

/**
 * @brief  自身の配列と指定されたオブジェクトの配列との比較
 *
 *  演算子の右側(引数)で指定された mdarray(継承)クラスのオブジェクトを自身とを
 *  比較します．<br>
 *  obj と配列サイズ・要素の値が等しければ，true を返します．<br>
 *  obj と配列サイズ・要素の値が等しくなければ，false を返します．
 *
 * @param     obj mdarray クラスのオブジェクト
 * @return    true : 配列サイズ，要素の値が一致した場合<br>
 *            false : 配列サイズ，要素の値が不一致である場合
 * @note      このメンバ関数は，内部で compare() メンバ関数を使っています．
 */
bool mdarray::operator==(const mdarray &obj) const
{
    return this->ope_equal_equal(obj);
}

/**
 * @brief  自身の配列と指定されたオブジェクトの配列との比較 (否定形)
 *
 *  演算子の右側(引数)で指定された mdarray(継承)クラスのオブジェクトを自身とを
 *  比較(否定形)します．<br>
 *  obj と配列サイズ・要素の値が等しくなければ，true を返します．<br>
 *  obj と配列サイズ・要素の値が等しければ，false を返します．
 *
 * @param     obj mdarray クラスのオブジェクト
 * @return    true : 配列サイズ，要素の値が不一致である場合<br>
 *            false : 配列サイズ，要素の値が一致した場合
 * @note      このメンバ関数は，内部で compare() メンバ関数を使っています．
 */
bool mdarray::operator!=(const mdarray &obj) const
{
    return (this->ope_equal_equal(obj) == true) ? false : true;
}


/* ********************************************** */
/* 開始 : 手続き的オペレータに必要なコード : 開始 */
/* ********************************************** */

/* 手続き的オペレータとその仲間たち */

/**
 * @brief  オペレータの左右の2つの型から，どの型を返すかを決定 (内部用)
 *
 * @param   szt1 型種別その１
 * @param   szt2 型種別その２
 * @return  選択された型種別
 * @note    private な関数です．
 */
static ssize_t get_sz_type_from_two( ssize_t szt1, ssize_t szt2 )
{
    const uint8_t      i1 = 1;
    const int16_t      i2 = 1;
    const int32_t      i4 = 1;
    const int64_t      i8 = 1;
    const float         f = 1.0;
    const double        d = 1.0;
    const long double   l = 1.0;
    const fcomplex     fx = 1.0 + 1.0*I;
    const dcomplex     dx = 1.0 + 1.0*I;
    const ldcomplex   ldx = 1.0 + 1.0*I;
    /* 実数型のみのパターン */
    const ssize_t _1x1 = sizeof(i1 + i1);
    const ssize_t _1x2 = sizeof(i1 + i2);
    const ssize_t _1x4 = sizeof(i1 + i4);
    const ssize_t _1x8 = sizeof(i1 + i8);
    const ssize_t _2x2 = sizeof(i2 + i2);
    const ssize_t _2x4 = sizeof(i2 + i4);
    const ssize_t _2x8 = sizeof(i2 + i8);
    const ssize_t _4x4 = sizeof(i4 + i4);
    const ssize_t _4x8 = sizeof(i4 + i8);
    const ssize_t _8x8 = sizeof(i8 + i8);
    const ssize_t _fx1 = -(ssize_t)sizeof(f + i1);
    const ssize_t _fx2 = -(ssize_t)sizeof(f + i2);
    const ssize_t _fx4 = -(ssize_t)sizeof(f + i4);
    const ssize_t _fx8 = -(ssize_t)sizeof(f + i8);
    const ssize_t _dx1 = -(ssize_t)sizeof(d + i1);
    const ssize_t _dx2 = -(ssize_t)sizeof(d + i2);
    const ssize_t _dx4 = -(ssize_t)sizeof(d + i4);
    const ssize_t _dx8 = -(ssize_t)sizeof(d + i8);
    const ssize_t _lx1 = -(ssize_t)sizeof(l + i1);
    const ssize_t _lx2 = -(ssize_t)sizeof(l + i2);
    const ssize_t _lx4 = -(ssize_t)sizeof(l + i4);
    const ssize_t _lx8 = -(ssize_t)sizeof(l + i8);
    const ssize_t _fxf = -(ssize_t)sizeof(f + f);
    const ssize_t _fxd = -(ssize_t)sizeof(f + d);
    const ssize_t _fxl = -(ssize_t)sizeof(f + l);
    const ssize_t _dxd = -(ssize_t)sizeof(d + d);
    const ssize_t _dxl = -(ssize_t)sizeof(d + l);
    const ssize_t _lxl = -(ssize_t)sizeof(l + l);
    /* 複素数型を含むパターン */
    const ssize_t _1xfz    = -((ssize_t)sizeof(i1 + fx)-1);
    const ssize_t _1xdz    = -((ssize_t)sizeof(i1 + dx)-1);
    const ssize_t _1xldz   = -((ssize_t)sizeof(i1 + ldx)-1);
    const ssize_t _2xfz    = -((ssize_t)sizeof(i2 + fx)-1);
    const ssize_t _2xdz    = -((ssize_t)sizeof(i2 + dx)-1);
    const ssize_t _2xldz   = -((ssize_t)sizeof(i2 + ldx)-1);
    const ssize_t _4xfz    = -((ssize_t)sizeof(i4 + fx)-1);
    const ssize_t _4xdz    = -((ssize_t)sizeof(i4 + dx)-1);
    const ssize_t _4xldz   = -((ssize_t)sizeof(i4 + ldx)-1);
    const ssize_t _8xfz    = -((ssize_t)sizeof(i8 + fx)-1);
    const ssize_t _8xdz    = -((ssize_t)sizeof(i8 + dx)-1);
    const ssize_t _8xldz   = -((ssize_t)sizeof(i8 + ldx)-1);
    const ssize_t _fxfz    = -((ssize_t)sizeof(f + fx)-1);
    const ssize_t _fxdz    = -((ssize_t)sizeof(f + dx)-1);
    const ssize_t _fxldz   = -((ssize_t)sizeof(f + ldx)-1);
    const ssize_t _dxfz    = -((ssize_t)sizeof(d + fx)-1);
    const ssize_t _dxdz    = -((ssize_t)sizeof(d + dx)-1);
    const ssize_t _dxldz   = -((ssize_t)sizeof(d + ldx)-1);
    const ssize_t _lxfz    = -((ssize_t)sizeof(l + fx)-1);
    const ssize_t _lxdz    = -((ssize_t)sizeof(l + dx)-1);
    const ssize_t _lxldz   = -((ssize_t)sizeof(l + ldx)-1);
    const ssize_t _fzxfz   = -((ssize_t)sizeof(fx + fx)-1);
    const ssize_t _fzxdz   = -((ssize_t)sizeof(fx + dx)-1);
    const ssize_t _fzxldz  = -((ssize_t)sizeof(fx + ldx)-1);
    const ssize_t _dzxdz   = -((ssize_t)sizeof(dx + dx)-1);
    const ssize_t _dzxldz  = -((ssize_t)sizeof(dx + ldx)-1);
    const ssize_t _ldzxldz = -((ssize_t)sizeof(ldx + ldx)-1);
    /* サイズ検索テーブル */
    const ssize_t type_tbl[][40] = 
    /*                    -31 x x x      -27 x x x      -23 x x x      -19 x x     -16      -15     -14 x     -12 x     -10 x      -8       -7 x x      -4 x x x x       1       2 x       4 x x x       8 */
	{
	  /* -31 */ {_ldzxldz,0,0,0,_ldzxldz,0,0,0,_ldzxldz,0,0,0,_ldzxldz,0,0, _lxldz, _dzxldz, _lxldz,0, _lxldz,0, _lxldz,0, _dxldz, _fzxldz,0,0, _fxldz,0,0,0,0, _1xldz, _2xldz,0, _4xldz,0,0,0, _8xldz},
	  /* x   */ {0},
	  /* x   */ {0},
	  /* x   */ {0},
	  /* -27 */ {_ldzxldz,0,0,0,_ldzxldz,0,0,0,_ldzxldz,0,0,0,_ldzxldz,0,0, _lxldz, _dzxldz, _lxldz,0, _lxldz,0, _lxldz,0, _dxldz, _fzxldz,0,0, _fxldz,0,0,0,0, _1xldz, _2xldz,0, _4xldz,0,0,0, _8xldz},
	  /* x   */ {0},
	  /* x   */ {0},
	  /* x   */ {0},
	  /* -23 */ {_ldzxldz,0,0,0,_ldzxldz,0,0,0,_ldzxldz,0,0,0,_ldzxldz,0,0, _lxldz, _dzxldz, _lxldz,0, _lxldz,0, _lxldz,0, _dxldz, _fzxldz,0,0, _fxldz,0,0,0,0, _1xldz, _2xldz,0, _4xldz,0,0,0, _8xldz},
	  /* x   */ {0},
	  /* x   */ {0},
	  /* x   */ {0},
	  /* -19 */ {_ldzxldz,0,0,0,_ldzxldz,0,0,0,_ldzxldz,0,0,0,_ldzxldz,0,0, _lxldz, _dzxldz, _lxldz,0, _lxldz,0, _lxldz,0, _dxldz, _fzxldz,0,0, _fxldz,0,0,0,0, _1xldz, _2xldz,0, _4xldz,0,0,0, _8xldz},
	  /* x   */ {0},
	  /* x   */ {0},
          /* -16 */ {  _lxldz,0,0,0,  _lxldz,0,0,0,  _lxldz,0,0,0,  _lxldz,0,0,   _lxl,   _lxdz,   _lxl,0,   _lxl,0,   _lxl,0,   _dxl,   _lxfz,0,0,   _fxl,0,0,0,0,   _lx1,   _lx2,0,   _lx4,0,0,0,   _lx8},
	  /* -15 */ { _dzxldz,0,0,0, _dzxldz,0,0,0, _dzxldz,0,0,0, _dzxldz,0,0,  _lxdz,  _dzxdz,  _lxdz,0,  _lxdz,0,  _lxdz,0,  _dxdz,  _fzxdz,0,0,  _fxdz,0,0,0,0,  _1xdz,  _2xdz,0,  _4xdz,0,0,0,  _8xdz},
          /* -14 */ {  _lxldz,0,0,0,  _lxldz,0,0,0,  _lxldz,0,0,0,  _lxldz,0,0,   _lxl,   _lxdz,   _lxl,0,   _lxl,0,   _lxl,0,   _dxl,   _lxfz,0,0,   _fxl,0,0,0,0,   _lx1,   _lx2,0,   _lx4,0,0,0,   _lx8},
	  /* x   */ {0}, 
          /* -12 */ {  _lxldz,0,0,0,  _lxldz,0,0,0,  _lxldz,0,0,0,  _lxldz,0,0,   _lxl,   _lxdz,   _lxl,0,   _lxl,0,   _lxl,0,   _dxl,   _lxfz,0,0,   _fxl,0,0,0,0,   _lx1,   _lx2,0,   _lx4,0,0,0,   _lx8},
	  /* x   */ {0}, 
          /* -10 */ {  _lxldz,0,0,0,  _lxldz,0,0,0,  _lxldz,0,0,0,  _lxldz,0,0,   _lxl,   _lxdz,   _lxl,0,   _lxl,0,   _lxl,0,   _dxl,   _lxfz,0,0,   _fxl,0,0,0,0,   _lx1,   _lx2,0,   _lx4,0,0,0,   _lx8},
	  /* x   */ {0}, 
	  /* -8  */ {  _dxldz,0,0,0,  _dxldz,0,0,0,  _dxldz,0,0,0,  _dxldz,0,0,   _dxl,   _dxdz,   _dxl,0,   _dxl,0,   _dxl,0,   _dxd,   _dxfz,0,0,   _fxd,0,0,0,0,   _dx1,   _dx2,0,   _dx4,0,0,0,   _dx8},
	  /* -7  */ { _fzxldz,0,0,0, _fzxldz,0,0,0, _fzxldz,0,0,0, _fzxldz,0,0,  _lxfz,  _fzxdz,  _lxfz,0,  _lxfz,0,  _lxfz,0,  _dxfz,  _fzxfz,0,0,  _fxfz,0,0,0,0,  _1xfz,  _2xfz,0,  _4xfz,0,0,0,  _8xfz},
	  /* x   */ {0}, {0},
	  /* -4  */ {  _fxldz,0,0,0,  _fxldz,0,0,0,  _fxldz,0,0,0,  _fxldz,0,0,   _fxl,   _fxdz,   _fxl,0,   _fxl,0,   _fxl,0,   _fxd,   _fxfz,0,0,   _fxf,0,0,0,0,   _fx1,   _fx2,0,   _fx4,0,0,0,   _fx8},
	  /* x   */ {0}, {0}, {0}, {0},
	  /*  1  */ {  _1xldz,0,0,0,  _1xldz,0,0,0,  _1xldz,0,0,0,  _1xldz,0,0,   _lx1,   _1xdz,   _lx1,0,   _lx1,0,   _lx1,0,   _dx1,   _1xfz,0,0,   _fx1,0,0,0,0,   _1x1,   _1x2,0,   _1x4,0,0,0,   _1x8},
	  /*  2  */ {  _2xldz,0,0,0,  _2xldz,0,0,0,  _2xldz,0,0,0,  _2xldz,0,0,   _lx2,   _2xdz,   _lx2,0,   _lx2,0,   _lx2,0,   _dx2,   _2xfz,0,0,   _fx2,0,0,0,0,   _1x2,   _2x2,0,   _2x4,0,0,0,   _2x8},
	  /* x   */ {0},
	  /*  4  */ {  _4xldz,0,0,0,  _4xldz,0,0,0,  _4xldz,0,0,0,  _4xldz,0,0,   _lx4,   _4xdz,   _lx4,0,   _lx4,0,   _lx4,0,   _dx4,   _4xfz,0,0,   _fx4,0,0,0,0,   _1x4,   _2x4,0,   _4x4,0,0,0,   _4x8},
	  /* x   */ {0}, {0}, {0},
	  /*  8  */ {  _8xldz,0,0,0,  _8xldz,0,0,0,  _8xldz,0,0,0,  _8xldz,0,0,   _lx8,   _8xdz,   _lx8,0,   _lx8,0,   _lx8,0,   _dx8,   _8xfz,0,0,   _fx8,0,0,0,0,   _1x8,   _2x8,0,   _4x8,0,0,0,   _8x8}
	};
    szt1 += 31;
    szt2 += 31;
    if ( 0 <= szt1 && szt1 < 40 && 0 <= szt2 && szt2 < 40 ) {
	return type_tbl[szt1][szt2];
    }
    else {
	return 0;
    }
}

/**
 * @brief  mdarray::ope_plus(const mdarray &obj) 等で使う再帰呼び出し関数
 *
 * @note   この関数は namespace *_nno_n, namespace *_non_n の関数を呼び出す<br>
 *         private な関数です．
 */
static void ope_arr_x_arr_r(
   mdarray *ret, const mdarray &src0, const mdarray &src1, int ope,
   void (*func_ope)(void *,const void *,size_t,const void *,size_t,size_t,int),
   size_t dim_ix,
   size_t blen_block_ret, size_t blen_block_src0, size_t blen_block_src1,
   void *_ret_p, const void *_src0_p, const void *_src1_p)
{
    char *ret_p = (char *)_ret_p;
    const char *src0_p = (const char *)_src0_p;
    const char *src1_p = (const char *)_src1_p;
    size_t len_ret = ret->length(dim_ix);
    size_t len_src0 = src0.length(dim_ix);
    size_t len_src1 = src1.length(dim_ix);
    size_t blen_blk_src0, blen_blk_src1;
    blen_block_ret /= len_ret;
    blen_block_src0 /= len_src0;
    blen_block_src1 /= len_src1;
    /* src?_p が NULL なら，NULL をキープするように blen_blk_src? を set */
    if ( src0_p == NULL ) blen_blk_src0 = 0;
    else blen_blk_src0 = blen_block_src0;
    if ( src1_p == NULL ) blen_blk_src1 = 0;
    else blen_blk_src1 = blen_block_src1;
    if ( 0 < dim_ix ) {
	size_t i;
	/* 注意: ここでは len_src0 <= len_ret && len_src1 <= len_ret を仮定 */
	if ( len_src0 < len_src1 ) {
	    for ( i=0 ; i < len_src0 ; i++ ) {
		ope_arr_x_arr_r(ret, src0, src1, ope, func_ope, 
			      dim_ix - 1,
			      blen_block_ret, blen_block_src0, blen_block_src1,
			      ret_p + blen_block_ret * i,
			      src0_p + blen_blk_src0 * i,
			      src1_p + blen_blk_src1 * i);
	    }
	    for (     ; i < len_src1 ; i++ ) {
		ope_arr_x_arr_r(ret, src0, src1, ope, func_ope,
			      dim_ix - 1,
			      blen_block_ret, blen_block_src0, blen_block_src1,
			      ret_p + blen_block_ret * i,
			      NULL,
			      src1_p + blen_blk_src1 * i);
	    }
	}
	else {
	    for ( i=0 ; i < len_src1 ; i++ ) {
		ope_arr_x_arr_r(ret, src0, src1, ope, func_ope, 
			      dim_ix - 1,
			      blen_block_ret, blen_block_src0, blen_block_src1,
			      ret_p + blen_block_ret * i,
			      src0_p + blen_blk_src0 * i,
			      src1_p + blen_blk_src1 * i);
	    }
	    for (     ; i < len_src0 ; i++ ) {
		ope_arr_x_arr_r(ret, src0, src1, ope, func_ope, 
			      dim_ix - 1,
			      blen_block_ret, blen_block_src0, blen_block_src1,
			      ret_p + blen_block_ret * i,
			      src0_p + blen_blk_src0 * i,
			      NULL);
	    }
	}
    }
    else {	/* 次元=0 での処理 */
	if ( len_src0 < len_src1 ) {
	    (*func_ope)(ret_p, src0_p, 1, src1_p, 1, len_src0, ope);
	    (*func_ope)(ret_p  + (blen_block_ret  * len_src0), 
	    		NULL, 1, 
	    		src1_p + (blen_blk_src1 * len_src0), 1, 
	    		len_src1 - len_src0, ope);
	}
	else {
	    (*func_ope)(ret_p, src0_p, 1, src1_p, 1, len_src1, ope);
	    (*func_ope)(ret_p  + (blen_block_ret  * len_src1), 
	    		src0_p + (blen_blk_src0 * len_src1), 1, 
	    		NULL, 1, 
	    		len_src0 - len_src1, ope);
	}
    }
    return;
}

/* dst[]     =  src0[]   +   src1[]                   */
/* type:new     type:new     type:org   <= N,N,O      */
/* _step0, _step1 が 0 なら当該はスカラー，1 なら配列 */
#define MAKE_FUNC_NNO(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
static void fncname(void *_dst, const void *_src0, size_t _step0, const void *_src1, size_t _step1, size_t n, int ope) \
{ \
    size_t k=0; \
    new_type *dst = (new_type *)_dst; \
    const new_type *src0 = (const new_type *)_src0; \
    const org_type *src1 = (const org_type *)_src1; \
    const size_t step0 = _step0; \
    const size_t step1 = _step1; \
    if ( src0 != NULL ) { \
      if ( src1 != NULL ) { \
	if ( ope == Ope_plus )       { for ( ; k < n ; k++, src0+=step0, src1+=step1 ) dst[k] = (*src0) + (new_type)fnc(*src1); } \
	else if ( ope == Ope_minus ) { for ( ; k < n ; k++, src0+=step0, src1+=step1 ) dst[k] = (*src0) - (new_type)fnc(*src1); } \
	else if ( ope == Ope_star )  { for ( ; k < n ; k++, src0+=step0, src1+=step1 ) dst[k] = (*src0) * (new_type)fnc(*src1); } \
	else			     { for ( ; k < n ; k++, src0+=step0, src1+=step1 ) dst[k] = (*src0) / (new_type)fnc(*src1); } \
      } else { \
	for ( ; k < n ; k++, src0+=step0 ) dst[k] = (*src0); \
      } \
    } else { \
      new_type zero; \
      c_memset(&zero,0,sizeof(zero)); \
      if ( src1 != NULL ) { \
	if ( ope == Ope_plus )       { for ( ; k < n ; k++, src1+=step1 ) dst[k] = zero + (new_type)fnc(*src1); } \
	else if ( ope == Ope_minus ) { for ( ; k < n ; k++, src1+=step1 ) dst[k] = zero - (new_type)fnc(*src1); } \
	else if ( ope == Ope_star )  { for ( ; k < n ; k++, src1+=step1 ) dst[k] = zero * (new_type)fnc(*src1); } \
	else			     { for ( ; k < n ; k++, src1+=step1 ) dst[k] = zero / (new_type)fnc(*src1); } \
      } else { \
	for ( ; k < n ; k++ ) dst[k] = zero; \
      } \
    } \
}
/* dst[]     =  src0[]    +  src1[]                   */
/* type:new     type:org     type:new    <= N,O,N     */
/* _step0, _step1 が 0 なら当該はスカラー，1 なら配列 */
#define MAKE_FUNC_NON(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
static void fncname(void *_dst, const void *_src0, size_t _step0, const void *_src1, size_t _step1, size_t n, int ope) \
{ \
    size_t k=0; \
    new_type *dst = (new_type *)_dst; \
    const org_type *src0 = (const org_type *)_src0; \
    const new_type *src1 = (const new_type *)_src1; \
    const size_t step0 = _step0; \
    const size_t step1 = _step1; \
    if ( src0 != NULL ) { \
      if ( src1 != NULL ) { \
	if ( ope == Ope_plus )       { for ( ; k < n ; k++, src0+=step0, src1+=step1 ) dst[k] = (new_type)fnc(*src0) + (*src1); } \
	else if ( ope == Ope_minus ) { for ( ; k < n ; k++, src0+=step0, src1+=step1 ) dst[k] = (new_type)fnc(*src0) - (*src1); } \
	else if ( ope == Ope_star )  { for ( ; k < n ; k++, src0+=step0, src1+=step1 ) dst[k] = (new_type)fnc(*src0) * (*src1); } \
	else			     { for ( ; k < n ; k++, src0+=step0, src1+=step1 ) dst[k] = (new_type)fnc(*src0) / (*src1); } \
      } else { \
	for ( ; k < n ; k++, src0+=step0 ) dst[k] = (new_type)fnc(*src0); \
      } \
    } else { \
      new_type zero; \
      c_memset(&zero,0,sizeof(zero)); \
      if ( src1 != NULL ) { \
	if ( ope == Ope_plus )       { for ( ; k < n ; k++, src1+=step1 ) dst[k] = zero + (*src1); } \
	else if ( ope == Ope_minus ) { for ( ; k < n ; k++, src1+=step1 ) dst[k] = zero - (*src1); } \
	else if ( ope == Ope_star )  { for ( ; k < n ; k++, src1+=step1 ) dst[k] = zero * (*src1); } \
	else			     { for ( ; k < n ; k++, src1+=step1 ) dst[k] = zero / (*src1); } \
      } else { \
	for ( ; k < n ; k++ ) dst[k] = zero; \
      } \
    } \
}
namespace x_nno_n
{
SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(MAKE_FUNC_NNO,,,,,,,,,,,,,creal,creal,creal,creal,creal,creal,creal,creal,creal,creal,creal,creal,f2fx,d2dx,ld2ldx,);
}
namespace x_non_n
{
SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(MAKE_FUNC_NON,,,,,,,,,,,,,creal,creal,creal,creal,creal,creal,creal,creal,creal,creal,creal,creal,f2fx,d2dx,ld2ldx,);
}
#undef MAKE_FUNC_NNO
#undef MAKE_FUNC_NON

/**
 * @brief  自身の配列に，スカラー値を加算した結果を一時オブジェクトで返す (低レベル)
 *
 *  スカラー値を自身の要素それぞれに加算した結果を格納したオブジェクトを作成
 *  し，それを返します．<br>
 *   1. 与えられた型種別と，現在の mdarray が持つ型種別から，演算結果となる
 *      型種別を取得する(get_sz_type_from_two)．<br>
 *   2. 上記の型種別を持つ新規の mdarray を作成し，現在の mdarray の情報を
 *      コピーする(init，init_properties)．<br>
 *   3. 新規の mdarray の全要素を対象として 与えられた変数を「+」演算する．<br>
 *   4. 最後に shallow copy を許可するフラグをセットし，return．
 *
 * @param   szt スカラー値の型種別
 * @param   v_ptr スカラー値のアドレス
 * @return  演算結果を格納したオブジェクト
 * @throw   内部バッファの確保に失敗した場合
 * @note    このメンバ関数は protected です．
 */
mdarray mdarray::ope_plus(ssize_t szt, const void *v_ptr) const
{
    mdarray ret;
    void (*func_ope)(void *,const void *,size_t,const void *,size_t,size_t,int);
    ssize_t ret_szt = get_sz_type_from_two(this->size_type(), szt);

    if ( ret_szt == 0 ) {
	err_throw(__FUNCTION__,"FATAL","Argument has invalid size_type.");
    }
    ret.init(ret_szt, true);
    ret.init_properties(*this);
    func_ope = NULL;
    /* 結果型がどちらかに一致する場合，直接演算を行なう */
    if ( ret_szt == this->size_type() ) {
	/* 型変換のための関数を選択 */
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
	if ( szt == org_sz_type && ret_szt == new_sz_type ) { \
	    func_ope = &x_nno_n::fncname; \
	}
	SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(SEL_FUNC,,,,,,,,,,,,,,,,,,,,,,,,,,,,else);
#undef SEL_FUNC
    } else if ( ret_szt == szt ) {
	/* 型変換のための関数を選択 */
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
	if ( this->size_type() == org_sz_type && ret_szt == new_sz_type ) { \
	    func_ope = &x_non_n::fncname; \
	}
	SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(SEL_FUNC,,,,,,,,,,,,,,,,,,,,,,,,,,,,else);
#undef SEL_FUNC
    }
    if ( func_ope != NULL ) {
	ret.reallocate(this->cdimarray(), this->dim_length(), false);
	(*func_ope)(ret.data_ptr(), this->data_ptr_cs(),1, v_ptr,0, ret.length(), Ope_plus);
    }
    /* fallback (遅いが使われる事はないはず) */
    else {
	ret.ope_plus_equal(*this, true);
	/* ope */
	ret.ope_plus_equal(szt,v_ptr);
    }

    ret.set_scopy_flag();
    return ret;
}

/**
 * @brief  自身の配列に，スカラー値を加算した結果を一時オブジェクトで返す
 *
 *  演算子の右側(引数) で指定されたスカラー値を，自身の要素すべてに対して加算
 *  した結果を格納したオブジェクトを作成して返します．<br>
 *  自身とは型が異なる引数の場合，通常のスカラー演算の場合と同様の型変換処理が
 *  行なわれます．<br>
 *  返されるオブジェクトの動作モードや rounding 等の属性は，自身の場合と同じで
 *  す．
 *
 * @param     v  スカラー値
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合
 */
mdarray mdarray::operator+(dcomplex v) const
{
    return this->ope_plus(DCOMPLEX_ZT,(const void*)&v);
}

/**
 * @brief  自身の配列に，スカラー値を加算した結果を一時オブジェクトで返す
 *
 *  演算子の右側(引数) で指定されたスカラー値を，自身の要素すべてに対して加算
 *  した結果を格納したオブジェクトを作成して返します．<br>
 *  自身とは型が異なる引数の場合，通常のスカラー演算の場合と同様の型変換処理が
 *  行なわれます．<br>
 *  返されるオブジェクトの動作モードや rounding 等の属性は，自身の場合と同じで
 *  す．
 *
 * @param     v  スカラー値
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合
 */
mdarray mdarray::operator+(fcomplex v) const
{
    return this->ope_plus(FCOMPLEX_ZT,(const void*)&v);
}

/**
 * @brief  自身の配列に，スカラー値を加算した結果を一時オブジェクトで返す
 *
 *  演算子の右側(引数) で指定されたスカラー値を，自身の要素すべてに対して加算
 *  した結果を格納したオブジェクトを作成して返します．<br>
 *  自身とは型が異なる引数の場合，通常のスカラー演算の場合と同様の型変換処理が
 *  行なわれます．<br>
 *  返されるオブジェクトの動作モードや rounding 等の属性は，自身の場合と同じで
 *  す．
 *
 * @param     v  スカラー値
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合
 */
mdarray mdarray::operator+(double v) const
{
    return this->ope_plus(DOUBLE_ZT,(const void *)&v);
}

/**
 * @brief  自身の配列に，スカラー値を加算した結果を一時オブジェクトで返す
 *
 *  演算子の右側(引数) で指定されたスカラー値を，自身の要素すべてに対して加算
 *  した結果を格納したオブジェクトを作成して返します．<br>
 *  自身とは型が異なる引数の場合，通常のスカラー演算の場合と同様の型変換処理が
 *  行なわれます．<br>
 *  返されるオブジェクトの動作モードや rounding 等の属性は，自身の場合と同じで
 *  す．
 *
 * @param     v  スカラー値
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合
 */
mdarray mdarray::operator+(float v) const
{
    return this->ope_plus(FLOAT_ZT,(const void *)&v);
}

/**
 * @brief  自身の配列に，スカラー値を加算した結果を一時オブジェクトで返す
 *
 *  演算子の右側(引数) で指定されたスカラー値を，自身の要素すべてに対して加算
 *  した結果を格納したオブジェクトを作成して返します．<br>
 *  自身とは型が異なる引数の場合，通常のスカラー演算の場合と同様の型変換処理が
 *  行なわれます．<br>
 *  返されるオブジェクトの動作モードや rounding 等の属性は，自身の場合と同じで
 *  す．
 *
 * @param     v  スカラー値
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合
 */
mdarray mdarray::operator+(long long v) const
{
    return this->ope_plus(sizeof(v),(const void *)&v);
}

/**
 * @brief  自身の配列に，スカラー値を加算した結果を一時オブジェクトで返す
 *
 *  演算子の右側(引数) で指定されたスカラー値を，自身の要素すべてに対して加算
 *  した結果を格納したオブジェクトを作成して返します．<br>
 *  自身とは型が異なる引数の場合，通常のスカラー演算の場合と同様の型変換処理が
 *  行なわれます．<br>
 *  返されるオブジェクトの動作モードや rounding 等の属性は，自身の場合と同じで
 *  す．
 *
 * @param     v  スカラー値
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合
 */
mdarray mdarray::operator+(long v) const
{
    return this->ope_plus(sizeof(v),(const void *)&v);
}

/**
 * @brief  自身の配列に，スカラー値を加算した結果を一時オブジェクトで返す
 *
 *  演算子の右側(引数) で指定されたスカラー値を，自身の要素すべてに対して加算
 *  した結果を格納したオブジェクトを作成して返します．<br>
 *  自身とは型が異なる引数の場合，通常のスカラー演算の場合と同様の型変換処理が
 *  行なわれます．<br>
 *  返されるオブジェクトの動作モードや rounding 等の属性は，自身の場合と同じで
 *  す．
 *
 * @param     v  スカラー値
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合
 */
mdarray mdarray::operator+(int v) const
{
    return this->ope_plus(sizeof(v),(const void *)&v);
}

/**
 * @brief  自身の配列に，指定された配列を加算した結果を一時オブジェクトで返す (低レベル)
 *
 *  mdarrayクラスのオブジェクトの配列を，自身に加算した結果を格納した
 *  オブジェクトを作成して返します. 
 *
 * @param   obj mdarrayクラスのオブジェクト
 * @return  演算結果を格納したオブジェクト
 * @throw   内部バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 * @note    このメンバ関数は protected です．
 */
mdarray mdarray::ope_plus(const mdarray &obj) const
{
    mdarray ret;
    void (*func_ope)(void *,const void *,size_t,const void *,size_t,size_t,int);
    ssize_t ret_szt = get_sz_type_from_two(this->size_type(), obj.size_type());
 
    if ( ret_szt == 0 ) {
	err_throw(__FUNCTION__,"FATAL","Argument has invalid size_type.");
    }
    ret.init(ret_szt, true);
    ret.init_properties(*this);
    func_ope = NULL;
    /* 結果型がどちらかに一致する場合，直接演算を行なう */
    if ( ret_szt == this->size_type() ) {
	/* 型変換のための関数を選択 */
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
	if ( obj.size_type() == org_sz_type && ret_szt == new_sz_type ) { \
	    func_ope = &x_nno_n::fncname; \
	}
	SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(SEL_FUNC,,,,,,,,,,,,,,,,,,,,,,,,,,,,else);
#undef SEL_FUNC
    } else if ( ret_szt == obj.size_type() ) {
	/* 型変換のための関数を選択 */
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
	if ( this->size_type() == org_sz_type && ret_szt == new_sz_type ) { \
	    func_ope = &x_non_n::fncname; \
	}
	SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(SEL_FUNC,,,,,,,,,,,,,,,,,,,,,,,,,,,,else);
#undef SEL_FUNC
    }
    if ( func_ope != NULL ) {
	heap_mem<size_t> ret_cdimarr;
	size_t ret_ndim;
	if ( enlarged_dim_info( *this, obj, &ret_cdimarr, 
				&ret_ndim, NULL ) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","malloc() failed");
	}
	if ( 0 < ret_ndim ) {
	    ret.reallocate(ret_cdimarr.ptr(), ret_ndim, false);
	    /* 再帰呼び出しで arr と arr の計算を行なう */
	    ope_arr_x_arr_r(&ret, *this, obj, Ope_plus, func_ope, 
		     ret_ndim - 1,
		     ret.byte_length(), this->byte_length(), obj.byte_length(),
		     ret.data_ptr(), this->data_ptr_cs(),obj.data_ptr_cs());
	}
    }
    /* fallback (遅いが使われる事はないはず) */
    else {
	ret.ope_plus_equal(*this, true);
	/* ope */
	ret.ope_plus_equal(obj, true);
    }

    ret.set_scopy_flag();
    return ret;
}

/**
 * @brief  自身の配列に，指定された配列を加算した結果を一時オブジェクトで返す
 *
 *  演算子の右側(引数) で指定された mdarray クラスのオブジェクトの配列と，自身
 *  とを加算した結果を格納したオブジェクトを作成して返します．<br>
 *  引数は，親クラスであるmdarray クラスですので，自身とは異なる型が設定された
 *  オブジェクトも指定できます．その場合，通常のスカラー演算の場合と同様の型
 *  変換処理が行なわれます．<br>
 *  返されるオブジェクトの動作モードや rounding 等の属性は，自身の場合と同じで
 *  す．
 *
 * @param     mdarray クラスのオブジェクト
 * @return    演算結果を格納したオブジェクト
 * @throw     内部バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 */
mdarray mdarray::operator+(const mdarray &obj) const
{
    return this->ope_plus(obj);
}


/**
 * @brief  自身の配列を，スカラー値で減算した結果を一時オブジェクトで返す (低レベル)
 *
 *  スカラー値を自身の要素それぞれから減算した結果を格納したオブジェクトを
 *  作成し，それを返します．<br>
 *   1. 与えられた型種別と，現在の mdarray が持つ型種別から，演算結果となる
 *      型種別を取得する(get_sz_type_from_two)．<br>
 *   2. 上記の型種別を持つ新規の mdarray を作成し，現在の mdarray の情報を
 *      コピーする(init，init_properties)．<br>
 *   3. 新規の mdarray の全要素を対象として 与えられた変数を「-」演算する．<br>
 *   4. 最後に shallow copy を許可するフラグをセットし，return．
 *
 * @param   szt スカラー値の型種別
 * @param   v_ptr スカラー値のアドレス
 * @return  演算結果を格納したオブジェクト
 * @throw   内部バッファの確保に失敗した場合
 * @note    このメンバ関数は protected です．
 */
mdarray mdarray::ope_minus(ssize_t szt, const void *v_ptr) const
{
    mdarray ret;
    void (*func_ope)(void *,const void *,size_t,const void *,size_t,size_t,int);
    ssize_t ret_szt = get_sz_type_from_two(this->size_type(), szt);

    if ( ret_szt == 0 ) {
	err_throw(__FUNCTION__,"FATAL","Argument has invalid size_type.");
    }
    ret.init(ret_szt, true);
    ret.init_properties(*this);
    func_ope = NULL;
    /* 結果型がどちらかに一致する場合，直接演算を行なう */
    if ( ret_szt == this->size_type() ) {
	/* 型変換のための関数を選択 */
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
	if ( szt == org_sz_type && ret_szt == new_sz_type ) { \
	    func_ope = &x_nno_n::fncname; \
	}
	SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(SEL_FUNC,,,,,,,,,,,,,,,,,,,,,,,,,,,,else);
#undef SEL_FUNC
    } else if ( ret_szt == szt ) {
	/* 型変換のための関数を選択 */
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
	if ( this->size_type() == org_sz_type && ret_szt == new_sz_type ) { \
	    func_ope = &x_non_n::fncname; \
	}
	SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(SEL_FUNC,,,,,,,,,,,,,,,,,,,,,,,,,,,,else);
#undef SEL_FUNC
    }
    if ( func_ope != NULL ) {
	ret.reallocate(this->cdimarray(), this->dim_length(), false);
	(*func_ope)(ret.data_ptr(), this->data_ptr_cs(),1, v_ptr,0, ret.length(), Ope_minus);
    }
    /* fallback (遅いが使われる事はないはず) */
    else {
	ret.ope_plus_equal(*this, true);
	/* ope */
	ret.ope_minus_equal(szt,v_ptr);
    }

    ret.set_scopy_flag();
    return ret;
}

/**
 * @brief  自身の配列を，スカラー値で減算した結果を一時オブジェクトで返す
 *
 *  演算子の右側(引数) で指定されたスカラー値を，自身の要素それぞれから減算した
 *  結果を格納したオブジェクトを作成して返します．<br>
 *  自身とは型が異なる引数の場合，通常のスカラー演算の場合と同様の型変換処理が
 *  行なわれます．<br>
 *  返されるオブジェクトの動作モードや rounding 等の属性は，自身の場合と同じで
 *  す．
 *
 * @param     v  スカラー値
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合
 */
mdarray mdarray::operator-(dcomplex v) const
{
    return this->ope_minus(DCOMPLEX_ZT,(const void *)&v);
}

/**
 * @brief  自身の配列を，スカラー値で減算した結果を一時オブジェクトで返す
 *
 *  演算子の右側(引数) で指定されたスカラー値を，自身の要素それぞれから減算した
 *  結果を格納したオブジェクトを作成して返します．<br>
 *  自身とは型が異なる引数の場合，通常のスカラー演算の場合と同様の型変換処理が
 *  行なわれます．<br>
 *  返されるオブジェクトの動作モードや rounding 等の属性は，自身の場合と同じで
 *  す．
 *
 * @param     v  スカラー値
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合
 */
mdarray mdarray::operator-(fcomplex v) const
{
    return this->ope_minus(FCOMPLEX_ZT,(const void *)&v);
}

/**
 * @brief  自身の配列を，スカラー値で減算した結果を一時オブジェクトで返す
 *
 *  演算子の右側(引数) で指定されたスカラー値を，自身の要素それぞれから減算した
 *  結果を格納したオブジェクトを作成して返します．<br>
 *  自身とは型が異なる引数の場合，通常のスカラー演算の場合と同様の型変換処理が
 *  行なわれます．<br>
 *  返されるオブジェクトの動作モードや rounding 等の属性は，自身の場合と同じで
 *  す．
 *
 * @param     v  スカラー値
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合
 */
mdarray mdarray::operator-(double v) const
{
    return this->ope_minus(DOUBLE_ZT,(const void *)&v);
}

/**
 * @brief  自身の配列を，スカラー値で減算した結果を一時オブジェクトで返す
 *
 *  演算子の右側(引数) で指定されたスカラー値を，自身の要素それぞれから減算した
 *  結果を格納したオブジェクトを作成して返します．<br>
 *  自身とは型が異なる引数の場合，通常のスカラー演算の場合と同様の型変換処理が
 *  行なわれます．<br>
 *  返されるオブジェクトの動作モードや rounding 等の属性は，自身の場合と同じで
 *  す．
 *
 * @param     v  スカラー値
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合
 */
mdarray mdarray::operator-(float v) const
{
    return this->ope_minus(FLOAT_ZT,(const void *)&v);
}

/**
 * @brief  自身の配列を，スカラー値で減算した結果を一時オブジェクトで返す
 *
 *  演算子の右側(引数) で指定されたスカラー値を，自身の要素それぞれから減算した
 *  結果を格納したオブジェクトを作成して返します．<br>
 *  自身とは型が異なる引数の場合，通常のスカラー演算の場合と同様の型変換処理が
 *  行なわれます．<br>
 *  返されるオブジェクトの動作モードや rounding 等の属性は，自身の場合と同じで
 *  す．
 *
 * @param     v  スカラー値
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合
 */
mdarray mdarray::operator-(long long v) const
{
    return this->ope_minus(sizeof(v),(const void *)&v);
}

/**
 * @brief  自身の配列を，スカラー値で減算した結果を一時オブジェクトで返す
 *
 *  演算子の右側(引数) で指定されたスカラー値を，自身の要素それぞれから減算した
 *  結果を格納したオブジェクトを作成して返します．<br>
 *  自身とは型が異なる引数の場合，通常のスカラー演算の場合と同様の型変換処理が
 *  行なわれます．<br>
 *  返されるオブジェクトの動作モードや rounding 等の属性は，自身の場合と同じで
 *  す．
 *
 * @param     v  スカラー値
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合
 */
mdarray mdarray::operator-(long v) const
{
    return this->ope_minus(sizeof(v),(const void *)&v);
}

/**
 * @brief  自身の配列を，スカラー値で減算した結果を一時オブジェクトで返す
 *
 *  演算子の右側(引数) で指定されたスカラー値を，自身の要素それぞれから減算した
 *  結果を格納したオブジェクトを作成して返します．<br>
 *  自身とは型が異なる引数の場合，通常のスカラー演算の場合と同様の型変換処理が
 *  行なわれます．<br>
 *  返されるオブジェクトの動作モードや rounding 等の属性は，自身の場合と同じで
 *  す．
 *
 * @param     v  スカラー値
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合
 */
mdarray mdarray::operator-(int v) const
{
    return this->ope_minus(sizeof(v),(const void *)&v);
}

/**
 * @brief  自身の配列を，指定された配列で減算した結果を一時オブジェクトで返す (低レベル)
 *
 *  mdarrayクラスのオブジェクトの配列を自身から減算した結果を格納した
 *  オブジェクトを作成して返します．
 * 
 * @param   obj mdarrayクラスのオブジェクト
 * @return  演算結果を格納したオブジェクト
 * @throw   内部バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 * @note    このメンバ関数は protected です．
 */
mdarray mdarray::ope_minus(const mdarray &obj) const
{
    mdarray ret;
    void (*func_ope)(void *,const void *,size_t,const void *,size_t,size_t,int);
    ssize_t ret_szt = get_sz_type_from_two(this->size_type(), obj.size_type());
 
    if ( ret_szt == 0 ) {
	err_throw(__FUNCTION__,"FATAL","Argument has invalid size_type.");
    }
    ret.init(ret_szt, true);
    ret.init_properties(*this);
    func_ope = NULL;
    /* 結果型がどちらかに一致する場合，直接演算を行なう */
    if ( ret_szt == this->size_type() ) {
	/* 型変換のための関数を選択 */
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
	if ( obj.size_type() == org_sz_type && ret_szt == new_sz_type ) { \
	    func_ope = &x_nno_n::fncname; \
	}
	SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(SEL_FUNC,,,,,,,,,,,,,,,,,,,,,,,,,,,,else);
#undef SEL_FUNC
    } else if ( ret_szt == obj.size_type() ) {
	/* 型変換のための関数を選択 */
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
	if ( this->size_type() == org_sz_type && ret_szt == new_sz_type ) { \
	    func_ope = &x_non_n::fncname; \
	}
	SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(SEL_FUNC,,,,,,,,,,,,,,,,,,,,,,,,,,,,else);
#undef SEL_FUNC
    }
    if ( func_ope != NULL ) {
	heap_mem<size_t> ret_cdimarr;
	size_t ret_ndim;
	if ( enlarged_dim_info( *this, obj, &ret_cdimarr, 
				&ret_ndim, NULL ) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","malloc() failed");
	}
	if ( 0 < ret_ndim ) {
	    ret.reallocate(ret_cdimarr.ptr(), ret_ndim, false);
	    /* 再帰呼び出しで arr と arr の計算を行なう */
	    ope_arr_x_arr_r(&ret, *this, obj, Ope_minus, func_ope, 
		     ret_ndim - 1,
		     ret.byte_length(), this->byte_length(), obj.byte_length(),
		     ret.data_ptr(), this->data_ptr_cs(),obj.data_ptr_cs());
	}
    }
    /* fallback (遅いが使われる事はないはず) */
    else {
	ret.ope_plus_equal(*this, true);
	/* ope */
	ret.ope_minus_equal(obj, true);
    }

    ret.set_scopy_flag();
    return ret;
}

/**
 * @brief  自身の配列を，指定された配列で減算した結果を一時オブジェクトで返す
 *
 *  演算子の右側(引数) で指定されたmdarray クラスのオブジェクトの配列を自身から
 *  減算した結果を格納したオブジェクトを作成して返します．<br>
 *  引数は，親クラスであるmdarray クラスですので，自身とは異なる型が設定された
 *  オブジェクトも指定できます．その場合，通常のスカラー演算の場合と同様の型
 *  変換処理が行なわれます．<br>
 *  返されるオブジェクトの動作モードや rounding 等の属性は，自身の場合と同じで
 *  す．
 *
 * @param     obj mdarray クラスのオブジェクト
 * @return    演算結果を格納したオブジェクト
 * @throw     内部バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 */
mdarray mdarray::operator-(const mdarray &obj) const
{
    return this->ope_minus(obj);
}


/**
 * @brief  自身の配列に，スカラー値を乗算した結果を一時オブジェクトで返す (低レベル)
 *
 *  自身からスカラー値を乗算した結果を格納したオブジェクトを作成し，それを
 *  返します．<br>
 *   1. 与えられた型種別と，現在の mdarray が持つ型種別から，演算結果となる
 *      型種別を取得する(get_sz_type_from_two)．<br>
 *   2. 上記の型種別を持つ新規の mdarray を作成し，現在の mdarray の情報を
 *      コピーする(init，init_properties)．<br>
 *   3. 新規の mdarray の全要素を対象として 与えられた変数を「*」演算する．<br>
 *   4. 最後に shallow copy を許可するフラグをセットし，return．
 *
 * @param   szt スカラー値の型種別
 * @param   v_ptr スカラー値のアドレス
 * @return  演算結果を格納したオブジェクト
 * @throw   内部バッファの確保に失敗した場合
 * @note    このメンバ関数は protected です．
 */
mdarray mdarray::ope_star(ssize_t szt, const void *v_ptr) const
{
    mdarray ret;
    void (*func_ope)(void *,const void *,size_t,const void *,size_t,size_t,int);
    ssize_t ret_szt = get_sz_type_from_two(this->size_type(), szt);

    if ( ret_szt == 0 ) {
	err_throw(__FUNCTION__,"FATAL","Argument has invalid size_type.");
    }
    ret.init(ret_szt, true);
    ret.init_properties(*this);
    func_ope = NULL;
    /* 結果型がどちらかに一致する場合，直接演算を行なう */
    if ( ret_szt == this->size_type() ) {
	/* 型変換のための関数を選択 */
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
	if ( szt == org_sz_type && ret_szt == new_sz_type ) { \
	    func_ope = &x_nno_n::fncname; \
	}
	SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(SEL_FUNC,,,,,,,,,,,,,,,,,,,,,,,,,,,,else);
#undef SEL_FUNC
    } else if ( ret_szt == szt ) {
	/* 型変換のための関数を選択 */
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
	if ( this->size_type() == org_sz_type && ret_szt == new_sz_type ) { \
	    func_ope = &x_non_n::fncname; \
	}
	SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(SEL_FUNC,,,,,,,,,,,,,,,,,,,,,,,,,,,,else);
#undef SEL_FUNC
    }
    if ( func_ope != NULL ) {
	ret.reallocate(this->cdimarray(), this->dim_length(), false);
	(*func_ope)(ret.data_ptr(), this->data_ptr_cs(),1, v_ptr,0, ret.length(), Ope_star);
    }
    /* fallback (遅いが使われる事はないはず) */
    else {
	ret.ope_plus_equal(*this, true);
	/* ope */
	ret.ope_star_equal(szt,v_ptr);
    }

    ret.set_scopy_flag();
    return ret;
}

/**
 * @brief  自身の配列に，スカラー値を乗算した結果を一時オブジェクトで返す
 *
 *  演算子の右側(引数) で指定されたスカラー値を，自身の要素すべてに対して乗算
 *  した結果を格納したオブジェクトを作成して返します．<br>
 *  自身とは型が異なる引数の場合，通常のスカラー演算の場合と同様の型変換処理が
 *  行なわれます．<br>
 *  返されるオブジェクトの動作モードや rounding 等の属性は，自身の場合と同じで
 *  す．
 *
 * @param     v  スカラー値
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合
 */
mdarray mdarray::operator*(dcomplex v) const
{
    return this->ope_star(DCOMPLEX_ZT,(const void *)&v);
}

/**
 * @brief  自身の配列に，スカラー値を乗算した結果を一時オブジェクトで返す
 *
 *  演算子の右側(引数) で指定されたスカラー値を，自身の要素すべてに対して乗算
 *  した結果を格納したオブジェクトを作成して返します．<br>
 *  自身とは型が異なる引数の場合，通常のスカラー演算の場合と同様の型変換処理が
 *  行なわれます．<br>
 *  返されるオブジェクトの動作モードや rounding 等の属性は，自身の場合と同じで
 *  す．
 *
 * @param     v  スカラー値
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合
 */
mdarray mdarray::operator*(fcomplex v) const
{
    return this->ope_star(FCOMPLEX_ZT,(const void *)&v);
}

/**
 * @brief  自身の配列に，スカラー値を乗算した結果を一時オブジェクトで返す
 *
 *  演算子の右側(引数) で指定されたスカラー値を，自身の要素すべてに対して乗算
 *  した結果を格納したオブジェクトを作成して返します．<br>
 *  自身とは型が異なる引数の場合，通常のスカラー演算の場合と同様の型変換処理が
 *  行なわれます．<br>
 *  返されるオブジェクトの動作モードや rounding 等の属性は，自身の場合と同じで
 *  す．
 *
 * @param     v  スカラー値
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合
 */
mdarray mdarray::operator*(double v) const
{
    return this->ope_star(DOUBLE_ZT,(const void *)&v);
}

/**
 * @brief  自身の配列に，スカラー値を乗算した結果を一時オブジェクトで返す
 *
 *  演算子の右側(引数) で指定されたスカラー値を，自身の要素すべてに対して乗算
 *  した結果を格納したオブジェクトを作成して返します．<br>
 *  自身とは型が異なる引数の場合，通常のスカラー演算の場合と同様の型変換処理が
 *  行なわれます．<br>
 *  返されるオブジェクトの動作モードや rounding 等の属性は，自身の場合と同じで
 *  す．
 *
 * @param     v  スカラー値
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合
 */
mdarray mdarray::operator*(float v) const
{
    return this->ope_star(FLOAT_ZT,(const void *)&v);
}

/**
 * @brief  自身の配列に，スカラー値を乗算した結果を一時オブジェクトで返す
 *
 *  演算子の右側(引数) で指定されたスカラー値を，自身の要素すべてに対して乗算
 *  した結果を格納したオブジェクトを作成して返します．<br>
 *  自身とは型が異なる引数の場合，通常のスカラー演算の場合と同様の型変換処理が
 *  行なわれます．<br>
 *  返されるオブジェクトの動作モードや rounding 等の属性は，自身の場合と同じで
 *  す．
 *
 * @param     v  スカラー値
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合
 */
mdarray mdarray::operator*(long long v) const
{
    return this->ope_star(sizeof(v),(const void *)&v);
}

/**
 * @brief  自身の配列に，スカラー値を乗算した結果を一時オブジェクトで返す
 *
 *  演算子の右側(引数) で指定されたスカラー値を，自身の要素すべてに対して乗算
 *  した結果を格納したオブジェクトを作成して返します．<br>
 *  自身とは型が異なる引数の場合，通常のスカラー演算の場合と同様の型変換処理が
 *  行なわれます．<br>
 *  返されるオブジェクトの動作モードや rounding 等の属性は，自身の場合と同じで
 *  す．
 *
 * @param     v  スカラー値
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合
 */
mdarray mdarray::operator*(long v) const
{
    return this->ope_star(sizeof(v),(const void *)&v);
}

/**
 * @brief  自身の配列に，スカラー値を乗算した結果を一時オブジェクトで返す
 *
 *  演算子の右側(引数) で指定されたスカラー値を，自身の要素すべてに対して乗算
 *  した結果を格納したオブジェクトを作成して返します．<br>
 *  自身とは型が異なる引数の場合，通常のスカラー演算の場合と同様の型変換処理が
 *  行なわれます．<br>
 *  返されるオブジェクトの動作モードや rounding 等の属性は，自身の場合と同じで
 *  す．
 *
 * @param     v  スカラー値
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合
 */
mdarray mdarray::operator*(int v) const
{
    return this->ope_star(sizeof(v),(const void *)&v);
}

/**
 * @brief  自身の配列に，指定された配列を乗算した結果を一時オブジェクトで返す (低レベル)
 *
 *  mdarrayクラスのオブジェクトの配列で自身を乗算した結果を格納したオブジェクト
 *  を作成して返します. 
 *
 * @param   obj mdarrayクラスのオブジェクト
 * @return  演算結果を格納したオブジェクト
 * @throw   内部バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 * @note    このメンバ関数は protected です．
 */
mdarray mdarray::ope_star(const mdarray &obj) const
{
    mdarray ret;
    void (*func_ope)(void *,const void *,size_t,const void *,size_t,size_t,int);
    ssize_t ret_szt = get_sz_type_from_two(this->size_type(), obj.size_type());
 
    if ( ret_szt == 0 ) {
	err_throw(__FUNCTION__,"FATAL","Argument has invalid size_type.");
    }
    ret.init(ret_szt, true);
    ret.init_properties(*this);
    func_ope = NULL;
    /* 結果型がどちらかに一致する場合，直接演算を行なう */
    if ( ret_szt == this->size_type() ) {
	/* 型変換のための関数を選択 */
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
	if ( obj.size_type() == org_sz_type && ret_szt == new_sz_type ) { \
	    func_ope = &x_nno_n::fncname; \
	}
	SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(SEL_FUNC,,,,,,,,,,,,,,,,,,,,,,,,,,,,else);
#undef SEL_FUNC
    } else if ( ret_szt == obj.size_type() ) {
	/* 型変換のための関数を選択 */
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
	if ( this->size_type() == org_sz_type && ret_szt == new_sz_type ) { \
	    func_ope = &x_non_n::fncname; \
	}
	SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(SEL_FUNC,,,,,,,,,,,,,,,,,,,,,,,,,,,,else);
#undef SEL_FUNC
    }
    if ( func_ope != NULL ) {
	heap_mem<size_t> ret_cdimarr;
	size_t ret_ndim;
	if ( enlarged_dim_info( *this, obj, &ret_cdimarr, 
				&ret_ndim, NULL ) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","malloc() failed");
	}
	if ( 0 < ret_ndim ) {
	    ret.reallocate(ret_cdimarr.ptr(), ret_ndim, false);
	    /* 再帰呼び出しで arr と arr の計算を行なう */
	    ope_arr_x_arr_r(&ret, *this, obj, Ope_star, func_ope, 
		     ret_ndim - 1,
		     ret.byte_length(), this->byte_length(), obj.byte_length(),
		     ret.data_ptr(), this->data_ptr_cs(),obj.data_ptr_cs());
	}
    }
    /* fallback (遅いが使われる事はないはず) */
    else {
	ret.ope_plus_equal(*this, true);
	/* ope */
	ret.ope_star_equal(obj, true);
    }

    ret.set_scopy_flag();
    return ret;
}

/**
 * @brief  自身の配列に，指定された配列を乗算した結果を一時オブジェクトで返す
 *
 *  演算子の右側(引数) で指定されたmdarray クラスのオブジェクトの配列と，自身
 *  とを乗算した結果を格納したオブジェクトを作成して返します．<br>
 *  引数は，親クラスであるmdarray クラスですので，自身とは異なる型が設定された
 *  オブジェクトも指定できます．その場合，通常のスカラー演算の場合と同様の型
 *  変換処理が行なわれます．<br>
 *  返されるオブジェクトの動作モードや rounding 等の属性は，自身の場合と同じで
 *  す．
 *
 * @param     obj mdarray クラスのオブジェクト
 * @return    演算結果を格納したオブジェクト
 * @throw     内部バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 */
mdarray mdarray::operator*(const mdarray &obj) const
{
    return this->ope_star(obj);
}


/**
 * @brief  自身の配列を，スカラー値で除算した結果を一時オブジェクトで返す (低レベル)
 *
 *  自身からスカラー値を除算した結果を格納したオブジェクトを作成して返します.
 *
 * @param   szt スカラー値の型を示す値(型種別)
 * @param   v_ptr スカラー値
 * @return  演算結果を格納したオブジェクト
 * @throw   内部バッファの確保に失敗した場合
 * @note    このメンバ関数は protected です．
 */
mdarray mdarray::ope_slash(ssize_t szt, const void *v_ptr) const
{
    mdarray ret;
    void (*func_ope)(void *,const void *,size_t,const void *,size_t,size_t,int);
    ssize_t ret_szt = get_sz_type_from_two(this->size_type(), szt);

    if ( ret_szt == 0 ) {
	err_throw(__FUNCTION__,"FATAL","Argument has invalid size_type.");
    }
    ret.init(ret_szt, true);
    ret.init_properties(*this);
    func_ope = NULL;
    /* 結果型がどちらかに一致する場合，直接演算を行なう */
    if ( ret_szt == this->size_type() ) {
	/* 型変換のための関数を選択 */
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
	if ( szt == org_sz_type && ret_szt == new_sz_type ) { \
	    func_ope = &x_nno_n::fncname; \
	}
	SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(SEL_FUNC,,,,,,,,,,,,,,,,,,,,,,,,,,,,else);
#undef SEL_FUNC
    } else if ( ret_szt == szt ) {
	/* 型変換のための関数を選択 */
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
	if ( this->size_type() == org_sz_type && ret_szt == new_sz_type ) { \
	    func_ope = &x_non_n::fncname; \
	}
	SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(SEL_FUNC,,,,,,,,,,,,,,,,,,,,,,,,,,,,else);
#undef SEL_FUNC
    }
    if ( func_ope != NULL ) {
	ret.reallocate(this->cdimarray(), this->dim_length(), false);
	(*func_ope)(ret.data_ptr(), this->data_ptr_cs(),1, v_ptr,0, ret.length(), Ope_slash);
    }
    /* fallback (遅いが使われる事はないはず) */
    else {
	ret.ope_plus_equal(*this, true);
	/* ope */
	ret.ope_slash_equal(szt,v_ptr);
    }

    ret.set_scopy_flag();
    return ret;
}

/**
 * @brief  自身の配列を，スカラー値で除算した結果を一時オブジェクトで返す
 *
 *  演算子の右側(引数) で指定されたスカラー値を，自身の要素それぞれから除算した
 *  結果を格納したオブジェクトを作成して返します．<br>
 *  自身とは型が異なる引数の場合，通常のスカラー演算の場合と同様の型変換処理が
 *  行なわれます．<br>
 *  返されるオブジェクトの動作モードやrounding 属性は，自身の場合と同じです．
 *
 * @param     v  スカラー値
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合
 */
mdarray mdarray::operator/(dcomplex v) const
{
    return this->ope_slash(DCOMPLEX_ZT,(const void *)&v);
}

/**
 * @brief  自身の配列を，スカラー値で除算した結果を一時オブジェクトで返す
 *
 *  演算子の右側(引数) で指定されたスカラー値を，自身の要素それぞれから除算した
 *  結果を格納したオブジェクトを作成して返します．<br>
 *  自身とは型が異なる引数の場合，通常のスカラー演算の場合と同様の型変換処理が
 *  行なわれます．<br>
 *  返されるオブジェクトの動作モードやrounding 属性は，自身の場合と同じです．
 *
 * @param     v  スカラー値
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合
 */
mdarray mdarray::operator/(fcomplex v) const
{
    return this->ope_slash(FCOMPLEX_ZT,(const void *)&v);
}

/**
 * @brief  自身の配列を，スカラー値で除算した結果を一時オブジェクトで返す
 *
 *  演算子の右側(引数) で指定されたスカラー値を，自身の要素それぞれから除算した
 *  結果を格納したオブジェクトを作成して返します．<br>
 *  自身とは型が異なる引数の場合，通常のスカラー演算の場合と同様の型変換処理が
 *  行なわれます．<br>
 *  返されるオブジェクトの動作モードやrounding 属性は，自身の場合と同じです．
 *
 * @param     v  スカラー値
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合
 */
mdarray mdarray::operator/(double v) const
{
    return this->ope_slash(DOUBLE_ZT,(const void *)&v);
}

/**
 * @brief  自身の配列を，スカラー値で除算した結果を一時オブジェクトで返す
 *
 *  演算子の右側(引数) で指定されたスカラー値を，自身の要素それぞれから除算した
 *  結果を格納したオブジェクトを作成して返します．<br>
 *  自身とは型が異なる引数の場合，通常のスカラー演算の場合と同様の型変換処理が
 *  行なわれます．<br>
 *  返されるオブジェクトの動作モードやrounding 属性は，自身の場合と同じです．
 *
 * @param     v  スカラー値
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合
 */
mdarray mdarray::operator/(float v) const
{
    return this->ope_slash(FLOAT_ZT,(const void *)&v);
}

/**
 * @brief  自身の配列を，スカラー値で除算した結果を一時オブジェクトで返す
 *
 *  演算子の右側(引数) で指定されたスカラー値を，自身の要素それぞれから除算した
 *  結果を格納したオブジェクトを作成して返します．<br>
 *  自身とは型が異なる引数の場合，通常のスカラー演算の場合と同様の型変換処理が
 *  行なわれます．<br>
 *  返されるオブジェクトの動作モードやrounding 属性は，自身の場合と同じです．
 *
 * @param     v  スカラー値
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合
 */
mdarray mdarray::operator/(long long v) const
{
    return this->ope_slash(sizeof(v),(const void *)&v);
}

/**
 * @brief  自身の配列を，スカラー値で除算した結果を一時オブジェクトで返す
 *
 *  演算子の右側(引数) で指定されたスカラー値を，自身の要素それぞれから除算した
 *  結果を格納したオブジェクトを作成して返します．<br>
 *  自身とは型が異なる引数の場合，通常のスカラー演算の場合と同様の型変換処理が
 *  行なわれます．<br>
 *  返されるオブジェクトの動作モードやrounding 属性は，自身の場合と同じです．
 *
 * @param     v  スカラー値
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合
 */
mdarray mdarray::operator/(long v) const
{
    return this->ope_slash(sizeof(v),(const void *)&v);
}

/**
 * @brief  自身の配列を，スカラー値で除算した結果を一時オブジェクトで返す
 *
 *  演算子の右側(引数) で指定されたスカラー値を，自身の要素それぞれから除算した
 *  結果を格納したオブジェクトを作成して返します．<br>
 *  自身とは型が異なる引数の場合，通常のスカラー演算の場合と同様の型変換処理が
 *  行なわれます．<br>
 *  返されるオブジェクトの動作モードやrounding 属性は，自身の場合と同じです．
 *
 * @param     v  スカラー値
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合
 */
mdarray mdarray::operator/(int v) const
{
    return this->ope_slash(sizeof(v),(const void *)&v);
}

/**
 * @brief  自身の配列を，指定された配列で除算した結果を一時オブジェクトで返す (低レベル)
 *
 *  mdarrayクラスのオブジェクトの配列で自身を除算した結果を，
 *  格納したオブジェクトを作成して返します. 
 *
 * @param   obj mdarrayクラスのオブジェクト
 * @return  演算結果を格納したオブジェクト
 * @throw   内部バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 * @note    このメンバ関数は protected です．
 */
mdarray mdarray::ope_slash(const mdarray &obj) const
{
    mdarray ret;
    void (*func_ope)(void *,const void *,size_t,const void *,size_t,size_t,int);
    ssize_t ret_szt = get_sz_type_from_two(this->size_type(), obj.size_type());
 
    if ( ret_szt == 0 ) {
	err_throw(__FUNCTION__,"FATAL","Argument has invalid size_type.");
    }
    ret.init(ret_szt, true);
    ret.init_properties(*this);
    func_ope = NULL;
    /* 結果型がどちらかに一致する場合，直接演算を行なう */
    if ( ret_szt == this->size_type() ) {
	/* 型変換のための関数を選択 */
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
	if ( obj.size_type() == org_sz_type && ret_szt == new_sz_type ) { \
	    func_ope = &x_nno_n::fncname; \
	}
	SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(SEL_FUNC,,,,,,,,,,,,,,,,,,,,,,,,,,,,else);
#undef SEL_FUNC
    } else if ( ret_szt == obj.size_type() ) {
	/* 型変換のための関数を選択 */
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
	if ( this->size_type() == org_sz_type && ret_szt == new_sz_type ) { \
	    func_ope = &x_non_n::fncname; \
	}
	SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(SEL_FUNC,,,,,,,,,,,,,,,,,,,,,,,,,,,,else);
#undef SEL_FUNC
    }
    if ( func_ope != NULL ) {
	heap_mem<size_t> ret_cdimarr;
	size_t ret_ndim;
	if ( enlarged_dim_info( *this, obj, &ret_cdimarr, 
				&ret_ndim, NULL ) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","malloc() failed");
	}
	if ( 0 < ret_ndim ) {
	    ret.reallocate(ret_cdimarr.ptr(), ret_ndim, false);
	    /* 再帰呼び出しで arr と arr の計算を行なう */
	    ope_arr_x_arr_r(&ret, *this, obj, Ope_slash, func_ope, 
		     ret_ndim - 1,
		     ret.byte_length(), this->byte_length(), obj.byte_length(),
		     ret.data_ptr(), this->data_ptr_cs(),obj.data_ptr_cs());
	}
    }
    /* fallback (遅いが使われる事はないはず) */
    else {
	ret.ope_plus_equal(*this, true);
	/* ope */
	ret.ope_slash_equal(obj, true);
    }

    ret.set_scopy_flag();
    return ret;
}

/**
 * @brief  自身の配列を，指定された配列で除算した結果を一時オブジェクトで返す
 *
 *  演算子の右側(引数) で指定されたmdarray クラスのオブジェクトの配列を，
 *  自身から除算した結果を格納したオブジェクトを作成して返します．<br>
 *  引数は，親クラスであるmdarray クラスですので，自身とは異なる型が設定された
 *  オブジェクトも指定できます．その場合，通常のスカラー演算の場合と同様の型
 *  変換処理が行なわれます．<br>
 *  返されるオブジェクトの動作モードや rounding 等の属性は，自身の場合と同じで
 *  す．
 *
 * @param     obj mdarray クラスのオブジェクト
 * @return    演算結果を格納したオブジェクト
 * @throw     内部バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 */
mdarray mdarray::operator/(const mdarray &obj) const
{
    return this->ope_slash(obj);
}

/* ********************************************** */
/* 終了 : 手続き的オペレータに必要なコード : 終了 */
/* ********************************************** */


/*
 * member functions to handle the whole object and properties
 */

/* 
 *  自身の初期化を行います(低レベル).
 * 
 * @deprecated  現在は未使用
 * @return	自身の参照
 * @note	このメンバ関数はprivateです．<br>
 *              this->extptr_rec の更新，this->update_length_info() の呼び出し
 *              は行なわないので，呼び出し側で対処してください．
 */
/*
mdarray &mdarray::_init()
{
    this->set_auto_resize(true);
    this->set_auto_init(true);
    this->set_rounding(false);
    this->free_default_rec();
    this->sz_type_rec = this->default_size_type();
    this->dim_size_rec = 0;
    this->free_size_rec();
    this->free_arr_rec();
    this->free_arr_ptr_2d();
    this->free_arr_ptr_3d();
    c_memset(this->move_from_idx_rec,0,sizeof(this->move_from_idx_rec));
    c_memset(this->move_from_len_rec,0,sizeof(this->move_from_len_rec));

    return *this;
}
*/

/**
 * @brief  オブジェクトの初期化 (型情報は変更なし)
 *
 *  自身の配列・属性を初期化します．<br>
 *  型情報はそのまま，配列サイズ 0 としてオブジェクトを初期化します．
 *
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 * @note      「自動リサイズモード」で初期化します．
 */
mdarray &mdarray::init()
{
    debug_report("()");

    /* shallow copy 関係のクリーンアップ */
    this->cleanup_shallow_copy(false);

    /* 強制初期化 */
    this->__force_free();
    this->__force_init(this->size_type(), false);	/* keep sz_type */

    if ( this->extptr_rec != NULL ) *(this->extptr_rec) = this->data_ptr();

    /* update_length_info() の直後に呼ぶ必要がある */
    this->update_arr_ptr_2d();
    this->update_arr_ptr_3d();

    return *this;
}

/**
 * @brief  オブジェクトの初期化
 *
 *  自身の配列・属性を初期化します．<br>
 *  配列の要素の型を sz_type で指定します．
 *
 * @param     sz_type 変数の型種別
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 * @note      「自動リサイズモード」で初期化します．
 */
mdarray &mdarray::init( ssize_t sz_type )
{
    if ( this->is_acceptable_size_type(sz_type) == false ) {
    	err_throw1(__FUNCTION__,"ERROR","prohibited sz_type: %d",(int)sz_type);
    }

    /* shallow copy 関係のクリーンアップ */
    this->cleanup_shallow_copy(false);

    /* 強制初期化 */
    this->__force_free();
    this->__force_init(sz_type, false);

    if ( this->extptr_rec != NULL ) *(this->extptr_rec) = this->data_ptr();

    /* update_length_info() の直後に呼ぶ必要がある */
    this->update_arr_ptr_2d();
    this->update_arr_ptr_3d();

    return *this;
}

/**
 * @brief  オブジェクトの初期化
 *
 *  自身の配列・属性を初期化します．<br>
 *  配列の要素の型を sz_type で指定します．
 *
 * @param     sz_type 変数の型種別
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 * @note      「自動リサイズモード」で初期化します．
 */
mdarray &mdarray::init( ssize_t sz_type, bool auto_resize )
{
    if ( this->is_acceptable_size_type(sz_type) == false ) {
    	err_throw1(__FUNCTION__,"ERROR","prohibited sz_type: %d",(int)sz_type);
    }

    /* shallow copy 関係のクリーンアップ */
    this->cleanup_shallow_copy(false);

    /* 強制初期化 */
    this->__force_free();
    this->__force_init(sz_type, false);

    this->set_auto_resize(auto_resize);

    if ( this->extptr_rec != NULL ) *(this->extptr_rec) = this->data_ptr();

    /* update_length_info() の直後に呼ぶ必要がある */
    this->update_arr_ptr_2d();
    this->update_arr_ptr_3d();

    return *this;
}

/**
 * @brief  オブジェクトの初期化 (n次元)
 *
 *  自身の配列・属性を初期化します．<br>
 *  sz_type で要素の型，ndim で次元数，naxisx[] で各次元の要素数を指定します．
 *
 * @param     sz_type 変数の型種別
 * @param     auto_resize 自動リサイズモードに設定する場合は true
 * @param     naxisx[] 各次元の要素数
 * @param     ndim 配列次元数
 * @param     init_buf 要素値をデフォルト値で埋めるなら true
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 */
mdarray &mdarray::init( ssize_t sz_type, bool auto_resize, 
			const size_t naxisx[], size_t ndim, bool init_buf )
{
    size_t i, len = 1;

    if ( 0 < ndim && naxisx == NULL ) {
	err_throw(__FUNCTION__,"ERROR","0 < ndim, but NULL naxisx arg");
    }
    if ( this->is_acceptable_size_type(sz_type) == false ) {
    	err_throw1(__FUNCTION__,"ERROR","prohibited sz_type: %d",(int)sz_type);
    }

    /* shallow copy 関係のクリーンアップ */
    this->cleanup_shallow_copy(false);

    /* 強制初期化 */
    this->__force_free();
    this->__force_init(sz_type, false);

    this->set_auto_resize(auto_resize);

    if ( ndim == 0 ) goto done;

    if ( this->realloc_size_rec(ndim) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","realloc() failed");
    }
    for ( i=0 ; i < ndim ; i++ ) {
	size_t *srec_ptr = this->size_rec_ptr();
	//if ( naxisx[i] == 0 ) {
	//    err_report(__FUNCTION__,"WARNING","zero naxisx[i]; ignored");
	//    srec_ptr[i] = 1;
	//    srec_ptr[i] = 0;
	//}
	//else {
	//    srec_ptr[i] = naxisx[i];
	//}
	srec_ptr[i] = naxisx[i];
	len *= srec_ptr[i];
    }
    if ( 0 < len ) {
	if ( this->realloc_arr_rec(zt2bytes(sz_type) * len) < 0 ) {
	    this->free_size_rec();
	    err_throw(__FUNCTION__,"FATAL","realloc() failed");
	}
	if ( init_buf == true ) {
	    s_memset(this->data_ptr(), 0, zt2bytes(sz_type) * len, 0);
	}
    }

    this->dim_size_rec = ndim;

 done:
    if ( this->extptr_rec != NULL ) *(this->extptr_rec) = this->data_ptr();
    /* update cached length info and call user's callback func */
    this->update_length_info();
    /* update_length_info() の直後に呼ぶ必要がある */
    this->update_arr_ptr_2d();
    this->update_arr_ptr_3d();

    return *this;
}

#if 0 /* SLI__DISABLE_INCOMPATIBLE_V1_4 */
/* 1次元 */
/**
 * @brief  配列の初期化 (1次元)
 *
 *  自身の配列を初期化します．<br>
 *  sz_type で要素の型，naxis0 で 1 次元目の要素数を指定し，
 *  1 次元の配列を持つオブジェクトを作成します．
 *
 * @deprecated  非推奨
 * @param     sz_type 変数の型種別
 * @param     naxis0 次元番号0 の次元(1 次元目) の要素数
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 * @note      「手動リサイズモード」で初期化します．
 */
mdarray &mdarray::init( ssize_t sz_type, size_t naxis0 )
{
    const size_t naxisx[] = {naxis0};

    this->mdarray::init(sz_type, naxisx, 1, true);

    return *this;
}

/* 2次元 */
/**
 * @brief  配列の初期化 (2次元)
 *
 *  自身の配列を初期化します．<br>
 *  sz_type で要素の型，naxis0 で 1 次元目の要素数，
 *  naxis1 で 2 次元目の要素数指定し，2 次元の配列を持つオブジェクトを
 *  作成します．
 *
 * @deprecated  非推奨
 * @param     sz_type 変数の型種別
 * @param     naxis0 次元番号0 の次元(1次元目) の要素数
 * @param     naxis1 次元番号1 の次元(2次元目) の要素数
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 * @note      「手動リサイズモード」で初期化します．
 */
mdarray &mdarray::init( ssize_t sz_type, size_t naxis0, size_t naxis1 )
{
    const size_t naxisx[] = {naxis0, naxis1};

    this->mdarray::init(sz_type, naxisx, 2, true);

    return *this;
}

/* 3次元 */
/**
 * @brief  配列の初期化 (3次元)
 *
 *  自身の配列を初期化します．<br>
 *  sz_type で要素の型，naxis0 で 1 次元目の要素数，
 *  naxis1 で 2 次元目の要素数，naxis2 で 3 次元目の要素数を指定し，
 *  3 次元の配列を持つオブジェクトを作成します．
 *
 * @deprecated  非推奨
 * @param     sz_type 変数の型種別
 * @param     naxis0 次元番号0 の次元(1次元目) の要素数
 * @param     naxis1 次元番号1 の次元(2次元目) の要素数
 * @param     naxis2 次元番号2 の次元(3次元目) の要素数
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 * @note      「手動リサイズモード」で初期化します．
 */
mdarray &mdarray::init( ssize_t sz_type, size_t naxis0, size_t naxis1,
			size_t naxis2 )
{
    const size_t naxisx[] = {naxis0, naxis1, naxis2};

    this->mdarray::init(sz_type, naxisx, 3, true);

    return *this;
}
#endif	/* SLI__DISABLE_INCOMPATIBLE_V1_4 */

/**
 * @brief  オブジェクトのコピー
 *
 *  obj の配列の内容や属性等すべてを自身にコピーします．<br>
 *  obj が関数によって返されたテンポラリオブジェクトの場合，許可されていれば
 *  shallow copy が行なわれます．
 *
 * @param     obj コピー元となるオブジェクト
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 */
mdarray &mdarray::init( const mdarray &obj )
{
    debug_report("( const mdarray &obj )");

    if ( &obj == this ) return *this;

    debug_report_scopy1("arg: obj = %zx",(size_t)(&obj));

    if ( this->is_acceptable_size_type(obj.sz_type_rec) == false ) {

        err_throw1(__FUNCTION__,"ERROR","prohibited size_type: %d",
                   (int)obj.sz_type_rec);

    }
    else {

	bool do_shallow_copy = false;

	/* shallow copy 関係のクリーンアップ */
	this->cleanup_shallow_copy(false);

	/* 可能ならば，shallow copy を行なう */
	if ( obj._arr_rec != NULL && this->extptr_rec == NULL && 
	     this->extptr_2d_rec == NULL && this->extptr_3d_rec == NULL ) {
	    do_shallow_copy = obj.request_shallow_copy(this);
	}
	if ( do_shallow_copy == true ) this->__shallow_init(obj,false);
	else this->__deep_init(obj);

    }

    return *this;
}


/**
 * @brief  自動リサイズ，自動初期化，四捨五入，メモリ確保方法の設定をコピー
 *
 *  src_obj の auto_resize, auto_init, rounding, alloc_strategy の設定を
 *  コピーします．
 *
 * @param   src_obj コピーされるオブジェクト
 * @return  自身の参照
 *            
 */
mdarray &mdarray::init_properties( const mdarray &src_obj )
{
    this->set_auto_resize( src_obj.auto_resize() );
    this->set_auto_init( src_obj.auto_init() );
    this->set_rounding( src_obj.rounding() );
    this->set_alloc_strategy( src_obj.alloc_strategy() );
    return *this;
}

/**
 * @brief  現在の自動リサイズの可否の設定を変更
 *
 *  リサイズモードを真(true)，または偽(false) で設定します．<br>
 *  動作モードが「自動リサイズモード」の場合は true，
 *  動作モードが「手動リサイズモード」の場合は false です．
 *
 * @param     tf リサイズモード
 * @return    自身の参照
 *            
 */
mdarray &mdarray::set_auto_resize( bool tf )
{
    this->auto_resize_rec = tf;
    return *this;
}

/**
 * @brief  現在の自動初期化の可否の設定を変更
 *
 *  自動初期化モードを真(true)，または偽(false) で設定します．
 *
 * @param     tf 自動初期化モード
 * @return    自身の参照
 *            
 */
mdarray &mdarray::set_auto_init( bool tf )
{
    this->auto_init_rec = tf;
    return *this;
}

/**
 * @brief  現在の四捨五入の可否の設定を変更
 *
 *  いくつかの高レベルメンバ関数において，浮動小数点数を整数に変換する時に，
 *  四捨五入を行うか否を設定します．<br>
 *  四捨五入するように設定されている場合は真(true)，
 *  四捨五入しないように設定されている場合は偽(false) を指定します．
 *
 * @param   tf 四捨五入の設定
 * @return  自身の参照
 * @note    四捨五入の属性が機能するメンバ関数は，
 *          lvalue()，llvalue() メンバ関数，assign default() メンバ関数，
 *          assign() メンバ関数，convert() メンバ関数，画像向きメンバ関数全般．
 *            
 */
mdarray &mdarray::set_rounding( bool tf )
{
    this->rounding_rec = tf;
    return *this;
}

/**
 * @brief  メモリ確保方法の設定を変更
 *
 *  配列用メモリを確保する時に，どのような方法で行なうかを決定します．
 *  次の 3 つから選択します．<br>
 *    "min"  ... 最小限を確保．リサイズ時に必ず realloc() が呼ばれます．<br>
 *    "pow"  ... 2のn乗で確保．<br>
 *    "auto" ... 自動リサイズモードの時には "pow"，そうでない場合は "min" で
 *               リサイズ処理を実行します．<br>
 *    NULL，他 ... 現在の方法を維持します．
 *
 * @param   strategy メモリ確保方法の設定
 * @return  自身の参照
 *
 */
mdarray &mdarray::set_alloc_strategy( const char *strategy )
{
    if ( strategy != NULL ) {
	if ( c_strcmp(strategy,"auto") == 0 ) {
	    this->alloc_strategy_rec = Alloc_strategy_auto;
	}
	else if ( c_strcmp(strategy,"min") == 0 ) {
	    this->alloc_strategy_rec = Alloc_strategy_min;
	}
	else if ( c_strcmp(strategy,"pow") == 0 ) {
	    this->alloc_strategy_rec = Alloc_strategy_pow;
	}
    }
    return *this;
}

/**
 * @brief  配列の全要素の型変換
 *
 *  自身の全要素の値を，与えられた型種別(sz_type)に変換し，書き換えます．<br>
 *  変換前後の型によっては，桁落ちや桁あふれが発生しますので注意して下さい．
 *
 * @param     sz_type 要素の型種別
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合
 */
mdarray &mdarray::convert( ssize_t sz_type )
{
    void (*func_cnv)(const void *, void *, size_t, int, void *);

    func_cnv = NULL;
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
    if ( this->sz_type_rec == org_sz_type && sz_type == new_sz_type ) { \
	func_cnv = &cnv_nd::fncname; \
    }
    SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(SEL_FUNC,,,,,,,,,,,,,,,,,,,,,,,,,,,,else);
#undef SEL_FUNC

    if ( func_cnv != NULL ) {
	struct mdarray_cnv_nd_prms prms;
	prms.total_len_elements = 0;			/* 効かない */
	prms.round_flg = this->rounding();		/* 四捨五入/切り捨て */
	return this->convert_via_udf(sz_type, func_cnv, (void *)&prms);
    }

    err_report(__FUNCTION__,"WARNING","unsupported sz_type; resizing only");
    return this->convert_via_udf(sz_type,
			(void (*)(const void *,void *,size_t,int,void *))NULL,
			(void *)NULL);
}

/**
 * @brief  配列の全要素の型変換 (ユーザ関数経由)
 *
 *  自身の全要素の値を ユーザ定義関数 func 経由で sz_type が示す型に変換し，書
 *  き換えます．ユーザ定義関数を与えて変換時の挙動を変えることができます．<br>
 *  ユーザ定義関数の第1引数には配列の元の各要素アドレスが，第2引数には変換後の
 *  各要素のアドレスが，第3引数には第1・第2引数に与えられた要素の個数が，
 *  第4引数には変換の方向(前からなら正，後ろからなら負)，第5引数には user_ptr 
 *  が与えられます．<br>
 *  変換前後の型によっては，桁落ちや桁あふれが発生しますので注意して下さい．
 *
 * @param      sz_type  要素の型種別
 * @param      func     ユーザ関数のアドレス
 * @param      user_ptr ユーザ関数の最後に与えられるユーザのポインタ
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 */
mdarray &mdarray::convert_via_udf( ssize_t sz_type,
			   void (*func)(const void *,void *,size_t,int,void *),
			   void *user_ptr )
{
    const size_t all_len = this->length();
    const size_t org_bytes = this->bytes();
    const size_t new_bytes = zt2bytes(sz_type);
    bool buffer_smaller = false;

    if ( sz_type == 0 ) {
	err_throw(__FUNCTION__,"ERROR","invalid sz_type: 0");
    }
    if ( this->is_acceptable_size_type(sz_type) == false ) {
	err_throw1(__FUNCTION__,"ERROR","prohibited sz_type: %d",(int)sz_type);
    }
    if ( all_len == 0 ) goto done;

    /* 大きくなる場合は，最初に再確保する */
    if ( org_bytes < new_bytes ) {
	if ( this->realloc_arr_rec(new_bytes * all_len) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","realloc() failed");
	}
	if ( this->extptr_rec != NULL ) *(this->extptr_rec) = this->data_ptr();
    }
    /* 小さくなる場合は最後にreallocするようにフラグたてる */
    else if ( new_bytes < org_bytes ) {
	buffer_smaller = true;
    }

    if ( func != NULL ) {
	/* 大きくなる場合は後ろから */
	if ( org_bytes < new_bytes ) {
	    //size_t i;
	    //char *dest = (char *)(this->data_ptr()) + new_bytes * all_len;
	    //const char *src =
	    //	(const char *)(this->data_ptr()) + org_bytes * all_len;
	    //for ( i=0 ; i < all_len ; i++ ) {
	    //	src -= org_bytes;
	    //	dest -= new_bytes;
	    //	(*func)((const void *)src,(void *)dest,user_ptr);
	    //}
	    //err_report(__FUNCTION__,"DEBUG","to large : true");
	    (*func)((const void *)(this->data_ptr()),
		    (void *)(this->data_ptr()),all_len,-1,user_ptr);
	}
	else {
	    //size_t i;
	    //char *dest = (char *)(this->data_ptr());
	    //const char *src = (const char *)(this->data_ptr());
	    //for ( i=0 ; i < all_len ; i++ ) {
	    //	(*func)((const void *)src,(void *)dest,user_ptr);
	    //	src += org_bytes;
	    //	dest += new_bytes;
	    //}
	    //err_report(__FUNCTION__,"DEBUG","to small : false");
	    (*func)((const void *)(this->data_ptr()),
		    (void *)(this->data_ptr()),all_len,+1,user_ptr);
	}
    }

    /* 小さくなる場合 */
    if ( buffer_smaller == true ) {
	if ( this->realloc_arr_rec(new_bytes * all_len) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","realloc() failed");
	}
	if ( this->extptr_rec != NULL ) *(this->extptr_rec) = this->data_ptr();
    }

 done:
    this->sz_type_rec = sz_type;
    /* default値もついでに変換する */
    if ( func != NULL ) {
	if ( this->default_rec_ptr() != NULL ) {
	    if ( org_bytes < new_bytes ) {
		if ( this->realloc_default_rec(new_bytes) < 0 ) {
		    err_throw(__FUNCTION__,"FATAL","realloc() failed");
		}
		(*func)(this->default_value_ptr(),
			this->default_rec_ptr(), 1, +1, user_ptr);
	    }
	    else if ( new_bytes < org_bytes ) {
		(*func)(this->default_value_ptr(),
			this->default_rec_ptr(), 1, +1, user_ptr);
		if ( this->realloc_default_rec(new_bytes) < 0 ) {
		    err_throw(__FUNCTION__,"FATAL","realloc() failed");
		}
	    }
	    else {
		(*func)(this->default_value_ptr(),
			this->default_rec_ptr(), 1, +1, user_ptr);
	    }
	}
    }
    else {
	this->free_default_rec();
    }

    /* update cached length info and call user's callback func */
    this->update_length_info();
    /* update_length_info() の直後に呼ぶ必要がある */
    this->update_arr_ptr_2d();
    this->update_arr_ptr_3d();

    return *this;
}

/**
 * @brief  2つのオブジェクト間での内容のスワップ
 *
 *  指定されたオブジェクトsobj の内容と自身の内容を入れ替えます．
 *  配列の内容，配列のサイズ，属性等すべての状態が入れ替わります．
 *
 * @param     sobj mdarray クラスのオブジェクト
 * @return    自身の参照    
 * @throw     sobj で指定されたオブジェクトが不正な場合
 */
mdarray &mdarray::swap( mdarray &sobj )
{
    if ( &sobj == this ) return *this;

    /* バッファを直接操作するため呼ぶ */
    this->cleanup_shallow_copy(true);
    sobj.cleanup_shallow_copy(true);

    bool tmp__auto_resize_rec = sobj.auto_resize_rec;
    bool tmp__auto_init_rec = sobj.auto_init_rec;
    bool tmp__rounding_rec = sobj.rounding_rec;
    const char *tmp__alloc_strategy = sobj.alloc_strategy();
    void *tmp__default_rec = sobj._default_rec;
    ssize_t tmp__sz_type_rec = sobj.sz_type_rec;
    size_t tmp__dim_size_rec = sobj.dim_size_rec;
    size_t *tmp__size_rec = sobj._size_rec;
    void *tmp__arr_rec = sobj._arr_rec;
    size_t tmp__arr_alloc_blen_rec = sobj.arr_alloc_blen_rec;
    void **tmp__arr_ptr_2d_rec = sobj._arr_ptr_2d_rec;
    void ***tmp__arr_ptr_3d_rec = sobj._arr_ptr_3d_rec;

    if ( this->is_acceptable_size_type(sobj.sz_type_rec) == false ) {
    	err_throw1(__FUNCTION__,"ERROR","prohibited sz_type: %d",
		   (int)(sobj.sz_type_rec));
    }
    if ( sobj.is_acceptable_size_type(this->sz_type_rec) == false ) {
    	err_throw1(__FUNCTION__,"ERROR","prohibited sz_type: %d",
		   (int)(this->sz_type_rec));
    }

    sobj.set_auto_resize(this->auto_resize_rec);
    sobj.set_auto_init(this->auto_init_rec);
    sobj.set_rounding(this->rounding_rec);
    sobj.set_alloc_strategy(this->alloc_strategy());
    sobj._default_rec = this->_default_rec;
    sobj.sz_type_rec = this->sz_type_rec;
    sobj.dim_size_rec = this->dim_size_rec;
    sobj._size_rec = this->_size_rec;
    sobj._arr_rec = this->_arr_rec;
    sobj.arr_alloc_blen_rec = this->arr_alloc_blen_rec;
    sobj._arr_ptr_2d_rec = this->_arr_ptr_2d_rec;
    sobj._arr_ptr_3d_rec = this->_arr_ptr_3d_rec;
    if ( sobj.extptr_rec != NULL ) *(sobj.extptr_rec) = sobj._arr_rec;
    if ( sobj.extptr_2d_rec != NULL && sobj._arr_ptr_2d_rec != NULL ) 
	*(sobj.extptr_2d_rec) = sobj._arr_ptr_2d_rec;
    if ( sobj.extptr_3d_rec != NULL && sobj._arr_ptr_3d_rec != NULL ) 
	*(sobj.extptr_3d_rec) = sobj._arr_ptr_3d_rec;

    this->set_auto_resize(tmp__auto_resize_rec);
    this->set_auto_init(tmp__auto_init_rec);
    this->set_rounding(tmp__rounding_rec);
    this->set_alloc_strategy(tmp__alloc_strategy);
    this->_default_rec = tmp__default_rec;
    this->sz_type_rec = tmp__sz_type_rec;
    this->dim_size_rec = tmp__dim_size_rec;
    this->_size_rec = tmp__size_rec;
    this->_arr_rec = tmp__arr_rec;
    this->arr_alloc_blen_rec = tmp__arr_alloc_blen_rec;
    this->_arr_ptr_2d_rec = tmp__arr_ptr_2d_rec;
    this->_arr_ptr_3d_rec = tmp__arr_ptr_3d_rec;
    if ( this->extptr_rec != NULL ) *(this->extptr_rec) = this->_arr_rec;
    if ( this->extptr_2d_rec != NULL && this->_arr_ptr_2d_rec != NULL ) 
	*(this->extptr_2d_rec) = this->_arr_ptr_2d_rec;
    if ( this->extptr_3d_rec != NULL && this->_arr_ptr_3d_rec != NULL ) 
	*(this->extptr_3d_rec) = this->_arr_ptr_3d_rec;

    s_memswap(this->move_from_idx_rec, sobj.move_from_idx_rec, 
	      sizeof(this->move_from_idx_rec));
    s_memswap(this->move_from_len_rec, sobj.move_from_len_rec, 
	      sizeof(this->move_from_len_rec));

    /* update cached length info and call user's callback func */
    this->update_length_info();
    sobj.update_length_info();

    /* 外部ポインタがある時のみ update_arr_ptr_2d3d() を呼ぶ */
    if ( sobj.extptr_2d_rec != NULL && sobj._arr_ptr_2d_rec == NULL ) 
	sobj.update_arr_ptr_2d();
    if ( sobj.extptr_3d_rec != NULL && sobj._arr_ptr_3d_rec == NULL )
	sobj.update_arr_ptr_3d();
    if ( this->extptr_2d_rec != NULL && this->_arr_ptr_2d_rec == NULL )
	this->update_arr_ptr_2d();
    if ( this->extptr_3d_rec != NULL && this->_arr_ptr_3d_rec == NULL )
	this->update_arr_ptr_3d();

    return *this;
}

/**
 * @brief  自身の内容を指定オブジェクトへコピー
 *
 *  自身の全ての内容を指定されたオブジェクト dest_obj へコピーします．
 *  型，配列長，値等全ての属性がコピーされます．自身(コピー元) の配列は改変され
 *  ません．<br>
 *  型と全配列長が dest_obj と自身とで等しい場合は，配列用バッファの再確保は
 *  行なわれず，既存のバッファに配列の内容がコピーされます．<br>
 *  「=」演算子や .init(obj) とは異なり，常に deep copy が実行されます．
 *
 * @param     dest_obj コピー先のオブジェクトのアドレス
 * @return    コピーした要素数(列数×行数×レイヤ数)．<br>
 *            引数が不正な場合は負の値
 * @throw     dest_obj で指定されたオブジェクトが不正な場合
 * @throw     バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 */
ssize_t mdarray::copy( mdarray *dest_obj ) const
{
    debug_report("( mdarray *dest_obj )");
    if ( dest_obj != NULL ) {
	if ( dest_obj != this ) {
	    const mdarray &src_obj = *this;

	    if ( dest_obj->is_acceptable_size_type(src_obj.sz_type_rec) 
		 == false ) {
		err_throw1(__FUNCTION__,"ERROR","prohibited size_type: %d",
			   (int)src_obj.sz_type_rec);
	    }

	    /* shallow copy 関係のクリーンアップ */
	    dest_obj->cleanup_shallow_copy(false);
	
	    /* deep copy */
	    dest_obj->__deep_init(src_obj);
	}
	return dest_obj->length();
    }
    else return -1;
}

/**
 * @brief  自身の内容を指定オブジェクトへコピー (非推奨)
 * @deprecated 非推奨．<br>
 * ssize_t mdarray::copy( mdarray *dest_obj ) const <br>
 * をお使いください．
 */
ssize_t mdarray::copy( mdarray &dest_obj ) const
{
    debug_report("( mdarray &dest_obj )");
    return this->copy(&dest_obj);
}

/**
 * @brief  自身の内容を指定オブジェクトへ移管
 *
 *  自身の配列の内容を，dest_obj により指定されたオブジェクトへ「移管」します
 *  (型，配列長，各種属性が指定されたオブジェクトにコピーされます)．移管の
 *  結果，自身の配列長はゼロになります．<br>
 *  dest_obj についての配列用バッファの再確保は行なわれず，自身の配列用バッファ
 *  についての管理権限を dest_obj に譲渡する実装になっており 高速に動作します．
 *
 * @param     dest_obj 移管・コピー先のオブジェクトのアドレス
 * @return    自身の参照
 * @throw     dest_obj で指定されたオブジェクトが不正な場合
 * @throw     バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 */
mdarray &mdarray::cut( mdarray *dest_obj )
{
    debug_report("( mdarray *dest_obj )");

    if ( dest_obj == NULL ) {

	if ( 0 < this->dim_size_rec ) this->reallocate(NULL,0,false);

    }
    /* dest_obj が自身なら何もしない */
    else if ( dest_obj != this ) {

	if ( dest_obj->is_acceptable_size_type(this->size_type()) == false ) {
	    err_throw1(__FUNCTION__,"ERROR","prohibited size_type: %d",
		       (int)(this->size_type()));
	}

	/* バッファを直接操作するため呼ぶ */
	this->cleanup_shallow_copy(true);
	dest_obj->cleanup_shallow_copy(false);

	/* shallow copy の機能を使ってバッファアドレスをコピー */
	try {
	    dest_obj->__shallow_init(*this, true);
	}
	catch (...) {
	    this->__force_init(this->size_type(), false);
	    err_throw(__FUNCTION__,"FATAL","caught exception");
	}
	this->__force_init(this->size_type(), false);

	/* auto_resize などの属性を dest_obj から復元 */
	this->init_properties(*dest_obj);

	/* default値 を dest_obj から復元 */
	if ( dest_obj->_default_rec != NULL ) {
	    if ( this->realloc_default_rec(dest_obj->bytes()) < 0 ) {
		err_throw(__FUNCTION__,"FATAL","realloc() failed");
	    }
	    c_memcpy(this->_default_rec, dest_obj->_default_rec, 
		     dest_obj->bytes());
	}

	if ( this->extptr_rec != NULL ) {
	    *(this->extptr_rec) = this->_arr_rec;
	}

	/* update cached length info and call user's callback func */
	this->update_length_info();
	/* update_length_info() の直後に呼ぶ必要がある */
	this->update_arr_ptr_2d( (dest_obj->_arr_ptr_2d_rec != NULL) );
	this->update_arr_ptr_3d( (dest_obj->_arr_ptr_3d_rec != NULL) );

    }

    return *this;
}

/**
 * @brief  自身の配列の (x,y) でのトランスポーズ
 *
 *  自身の配列のカラムとロウとを入れ替えます．
 * 
 * @note  高速なアルゴリズムを使用しています．
 */
mdarray &mdarray::transpose_xy()
{
    const size_t org_length = this->length();

    /* transpose */
    this->transpose_xy_copy(this);

    if ( org_length != this->length() ) {
	err_throw(__FUNCTION__,"FATAL","unexpected internal error");
    }

    return *this;
}

/**
 * @brief  自身の配列の (x,y,z)→(z,x,y) のトランスポーズ
 *
 *  自身の配列の軸 (x,y,z) を (z,x,y) へ変換します．
 * 
 * @note  高速なアルゴリズムを使用しています．
 */
mdarray &mdarray::transpose_xyz2zxy()
{
    const size_t org_length = this->length();

    /* transpose */
    this->transpose_xyz2zxy_copy(this);

    if ( org_length != this->length() ) {
	err_throw(__FUNCTION__,"FATAL","unexpected internal error");
    }

    return *this;
}

/**
 * @brief  自身の配列の (x,y) での回転 (90度単位)
 *
 *  自身の配列の (x,y) 面についての回転(90度単位)を行ないます．<br>
 *  左下を原点とした場合，反時計まわりで指定します．
 *
 * @param  angle 90,-90, 180 のいずれかを指定
 * 
 */
mdarray &mdarray::rotate_xy( int angle )
{
    this->rotate_xy_copy(this, angle);
    return *this;
}


/*
 * APIs to obtain basic information of object
 */

/**
 * @brief  現在の自動リサイズの可否の設定を取得
 *
 *  リサイズモードを真(true)，または偽(false) で取得します．<br>
 *  動作モードが「自動リサイズモード」の場合は true，
 *  動作モードが「手動リサイズモード」の場合は false です．
 *
 * @return    動作モード(自動リサイズモードの場合はtrue)
 *            
 */
bool mdarray::auto_resize() const
{
    return this->auto_resize_rec;
}

/**
 * @brief  現在の自動初期化の可否の設定を取得
 *
 *  自動初期化モードを真(true)，または偽(false) で取得します．
 *
 * @return    動作モード(自動初期化モードの場合はtrue)
 *            
 */
bool mdarray::auto_init() const
{
    return this->auto_init_rec;
}

/**
 * @brief  現在の四捨五入の可否の設定を取得
 *
 *  四捨五入の可否の設定を読み取ります．<br>
 *  四捨五入するように設定されている場合は真(true)，
 *  四捨五入しないように設定されている場合は偽(false) が返ります．<br>
 *  mdarray クラスのオブジェクト生成時の初期状態では，四捨五入しないように
 *  設定されています．
 *
 * @return  四捨五入動作の属性 (四捨五入するように設定されている場合はtrue)
 * @note    四捨五入の属性が機能するメンバ関数は，
 *          lvalue()，llvalue() メンバ関数，assign default() メンバ関数，
 *          assign() メンバ関数，convert() メンバ関数，画像向きメンバ関数全般．
 *            
 */
bool mdarray::rounding() const
{
    return this->rounding_rec;
}

/**
 * @brief  現在のメモリ確保方法の設定を取得
 *
 *  配列用メモリを確保する時に，どのような方法で行なうよう設定されているかを
 *  読み取ります．
 *
 * @return  現在のメモリ確保方法 "min", "pow", "auto" のいずれか
 *
 */
const char *mdarray::alloc_strategy() const
{
    if ( this->alloc_strategy_rec == Alloc_strategy_min ) {
	return "min";
    }
    else if ( this->alloc_strategy_rec == Alloc_strategy_pow ) {
	return "pow";
    }
    else {
	return "auto";
    }
}


/*
 * C-like APIs to input and output data
 */

/**
 * @brief  最初の次元についての指定された区間をパディング
 *
 *  自身が持つ配列の要素番号 idx に，アドレス value_ptr で示された1つの値を
 *  len 個書き込みます．要素番号および次元番号は 0 から始まる数値で，idx と len
 *  は任意の値を取る事ができます．<br>
 *  「自動リサイズモード」の場合，引数の指定に対してオブジェクト内の配列長が
 *  不足している場合は，自動的に配列のリサイズを行ないます．この時，追加した
 *  要素のうち値が書き込まれない部分は，デフォルト値でパディングします．<br>
 *  「手動リサイズモード」の場合は，idx，len で指定された部分のうち，配列長を
 *  越える部分については処理が行なわれません．
 *
 * @param     value_ptr 値への間接参照
 * @param     idx 要素番号
 * @param     len 要素の個数
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合(自動リサイズモードの場合)
 * @throw     メモリ破壊を起こした場合
 */
mdarray &mdarray::put( const void *value_ptr, ssize_t idx, size_t len )
{
    return this->put(value_ptr, 0, idx, len);
}

/**
 * @brief  任意の1つの次元についての指定された区間をパディング
 *
 *  自身が持つ配列の次元番号 dim_index の要素番号 idx に，アドレス value_ptr で
 *  示された1つの値を len 個書き込みます．要素番号および次元番号は 0 から始まる
 *  数値で，idx と len は任意の値を取る事ができます．<br>
 *  「自動リサイズモード」の場合，引数の指定に対してオブジェクト内の配列長が
 *  不足している場合は，自動的に配列のリサイズを行ないます．この時，追加した
 *  要素のうち値が書き込まれない部分は，デフォルト値でパディングします．<br>
 *  「手動リサイズモード」の場合は，idx，len で指定された部分のうち，配列長を
 *  越える部分については処理が行なわれません．<br>
 *  dim_index が 1 以上の場合，下位次元の全要素に書き込まれます．
 *
 * @param     value_ptr 値への間接参照
 * @param     dim_index 次元番号
 * @param     idx 要素番号
 * @param     len 要素の個数
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合(自動リサイズモードの場合)
 * @throw     メモリ破壊を起こした場合
 */
mdarray &mdarray::put( const void *value_ptr,
		       size_t dim_index, ssize_t idx, size_t len )
{
    const size_t bytes = this->cached_bytes_rec;
    size_t org_length = this->length(dim_index);
    size_t i, idx_0;
    size_t b_unit0, b_unit1, unit0;
    size_t count;

    if ( this->dim_size_rec <= dim_index ) {
	if ( this->auto_resize() == true ) {
	    this->resize(dim_index, 1);
	    org_length = 1;
	}
	else return *this;	/* invalid */
    }

    if ( idx < 0 ) {
	if ( abs_sz2z(idx) < len ) {
	    len -= abs_sz2z(idx);
	    idx = 0;
	} else {
	    return *this;
	}
    }
    else {
	if ( this->auto_resize() == false ) {
	    if ( org_length <= abs_sz2z(idx) ) return *this;
	}
    }

    idx_0 = (size_t)idx;

    if ( org_length < idx_0 + len ) {
	if ( this->auto_resize() == true ) {
	    this->resize(dim_index, idx_0 + len);
	    org_length = this->length(dim_index);
	}
	else {
	    len = org_length - idx_0;
	}
    }

    if ( len <= 0 ) return *this;

    /* */
    unit0 = 1;
    for ( i=0 ; i < dim_index ; i++ ) {
	unit0 *= this->_size_rec[i];
    }
    b_unit0 = bytes * unit0;
    b_unit1 = b_unit0 * this->_size_rec[dim_index];

    count = 1;
    for ( i=dim_index+1 ; i < this->dim_size_rec ; i++ ) {
	count *= this->_size_rec[i];
    }

    void *dptr = this->data_ptr();
    for ( i=0 ; i < count ; i++ ) {
	if ( value_ptr != NULL ) {
	    char *dest_ptr = (char *)dptr + b_unit1 * i + b_unit0 * idx_0;
	    s_memfill(dest_ptr, value_ptr, bytes, unit0 * len, 
		      unit0 * len * count);
	}
	else {
	    char *dest_ptr = (char *)dptr + b_unit1 * i + b_unit0 * idx_0;
	    s_memset(dest_ptr, 0, b_unit0 * len, b_unit0 * len * count);
	}
    }

    return *this;
}

/**
 * @brief  自身の配列の内容をユーザ・バッファへコピー (要素数での指定)
 *
 *  自身の配列の内容を dest_bufで指定されたユーザ・バッファへコピーします．<br>
 *  バッファの大きさ elem_size は要素の個数で与えます．idx0, idx1, idx2 で
 *  自身の配列での開始点を指定します．<br>
 *  引数に，MDARRAY_INDEF を明示的に与えないでください．
 *
 * @param     dest_buf ユーザ・バッファのアドレス (コピー先)
 * @param     elem_size コピーする要素の個数
 * @param     idx0 次元番号 0 の次元(1次元目)の要素番号 (コピー元，省略可)
 * @param     idx1 次元番号 1 の次元(2次元目)の要素番号 (コピー元，省略可)
 * @param     idx2 次元番号 2 の次元(3次元目)の要素番号 (コピー元，省略可)
 * @return    ユーザのバッファ長が十分な場合にコピーされる要素数
 * @throw     メモリ破壊を起こした場合
 */
ssize_t mdarray::get_elements( void *dest_buf, size_t elem_size,
			       ssize_t idx0, ssize_t idx1, ssize_t idx2 ) const
{
    const size_t sz = this->cached_bytes_rec;
    ssize_t ret = this->getdata(dest_buf, elem_size * sz, idx0, idx1, idx2);
    if ( 0 <= ret ) return ret / sz;
    else return ret;
}

/**
 * @brief  ユーザ・バッファの内容を自身の配列へコピー (要素数での指定)
 *
 *  src_buf で指定されたユーザ・バッファの内容を自身の配列へコピーします．<br>
 *  バッファの大きさ elem_size は，要素の個数で与えます．idx0, idx1, idx2 で
 *  自身の配列での開始点を指定します．<br>
 *  引数に，MDARRAY_INDEF を明示的に与えないでください．
 *
 * @param     src_buf  ユーザ・バッファのアドレス (コピー元)
 * @param     elem_size コピーする要素の個数
 * @param     idx0 次元番号 0 の次元(1次元目)の要素番号 (コピー先，省略可)
 * @param     idx1 次元番号 1 の次元(2次元目)の要素番号 (コピー先，省略可)
 * @param     idx2 次元番号 2 の次元(3次元目)の要素番号 (コピー先，省略可)
 * @return    ユーザのバッファ長が十分な場合にコピーされる要素数
 * @throw     メモリ破壊を起こした場合
 */
ssize_t mdarray::put_elements( const void *src_buf, size_t elem_size,
			       ssize_t idx0, ssize_t idx1, ssize_t idx2 )
{
    const size_t sz = this->cached_bytes_rec;
    ssize_t ret = this->putdata(src_buf, elem_size * sz, idx0, idx1, idx2);
    if ( 0 <= ret ) return ret / sz;
    else return ret;
}

/**
 * @brief  自身の配列の内容をユーザ・バッファへコピー (バイト数での指定)
 *
 *  自身の配列の内容を dest_bufで指定されたユーザ・バッファへコピーします．<br>
 *  バッファの大きさ buf_size は，バイト単位で与えます．idx0, idx1, idx2 で
 *  自身の配列での開始点を指定します．<br>
 *  引数に，MDARRAY_INDEF を明示的に与えないでください．
 *
 * @param     dest_buf ユーザ・バッファのアドレス (コピー先)
 * @param     buf_size バッファサイズ (バイト単位)
 * @param     idx0 次元番号 0 の次元(1次元目)の要素番号 (省略可)
 * @param     idx1 次元番号 1 の次元(2次元目)の要素番号 (省略可)
 * @param     idx2 次元番号 2 の次元(3次元目)の要素番号 (省略可)
 * @return    ユーザのバッファ長(buf_size) が十分な場合にコピーされる
 *            バイトサイズ
 * @throw     メモリ破壊を起こした場合
 */
ssize_t mdarray::getdata( void *dest_buf, size_t buf_size,
			  ssize_t idx0, ssize_t idx1, ssize_t idx2 ) const
{
    const ssize_t idx = this->get_idx_3d_cs(idx0,idx1,idx2);
    size_t sz, full_sz;
    if ( idx < 0 ) {
	err_report(__FUNCTION__,"WARNING","invalid args; ignored");
	return -1;
    }
    if ( 0 < buf_size && dest_buf == NULL ) return -1;
    sz = this->cached_bytes_rec * (this->cached_length_rec - idx);
    full_sz = sz;
    if ( buf_size < sz ) sz = buf_size;
    s_memmove( dest_buf, 
       (const char *)(this->data_ptr_cs()) + this->cached_bytes_rec * idx,
       sz );
    return full_sz;
}

/**
 * @brief  ユーザ・バッファの内容を自身の配列へコピー (バイト数での指定)
 *
 *  src_buf で指定されたユーザ・バッファの内容を自身の配列へコピーします．<br>
 *  バッファの大きさbuf_size は，バイト単位で与えます．idx0, idx1, idx2 で
 *  自身の配列での開始点を指定します．<br>
 *  引数に，MDARRAY_INDEF を明示的に与えないでください．
 *
 * @param     src_buf  ユーザ・バッファのアドレス (コピー元)
 * @param     buf_size ユーザ・バッファのサイズ (バイト単位)
 * @param     idx0 次元番号 0 の次元(1次元目)の要素番号 (省略可)
 * @param     idx1 次元番号 1 の次元(2次元目)の要素番号 (省略可)
 * @param     idx2 次元番号 2 の次元(3次元目)の要素番号 (省略可)
 * @return    ユーザのバッファ長(buf_size) が十分な場合にコピーされる
 *            バイトサイズ
 * @throw     メモリ破壊を起こした場合
 */
ssize_t mdarray::putdata( const void *src_buf, size_t buf_size,
			  ssize_t idx0, ssize_t idx1, ssize_t idx2 )
{
    const ssize_t idx = this->get_idx_3d_cs(idx0,idx1,idx2);
    size_t sz, full_sz;
    if ( idx < 0 ) {
	err_report(__FUNCTION__,"WARNING","invalid args; ignored");
	return -1;
    }
    if ( 0 < buf_size && src_buf == NULL ) return -1;
    sz = this->cached_bytes_rec * (this->cached_length_rec - idx);
    full_sz = sz;
    if ( buf_size < sz ) sz = buf_size;
    s_memmove( (char *)(this->data_ptr()) + this->cached_bytes_rec * idx,
	       src_buf, sz );
    return full_sz;
}

/**
 * @brief  自身の配列の内容をユーザ・バッファへコピー (SIMDストリーム命令使用)
 *
 *  自身の配列の内容を dest_buf で指定されたユーザ・バッファへコピーします．
 *  コピーされる領域が十分に大きい場合，SIMD ストリーム命令を使って高速な
 *  コピーを行ないます．<br>
 *  バッファの大きさ buf_size は，バイト単位で与えます．idx, idx1, idx2 で
 *  自身の配列での開始点を指定します．<br>
 *  引数に，MDARRAY_INDEF を明示的に与えないでください．
 *
 * @param     dest_buf ユーザ・バッファのアドレス (コピー先)
 * @param     buf_size バッファサイズ (バイト単位)
 * @param     total_copied_size getdata_fast()が複数回呼び出される時の
 *                              トータルでコピーされるバイトサイズ
 * @param     idx0 次元番号 0 の次元(1次元目)の要素番号 (省略可)
 * @param     idx1 次元番号 1 の次元(2次元目)の要素番号 (省略可)
 * @param     idx2 次元番号 2 の次元(3次元目)の要素番号 (省略可)
 * @return    ユーザのバッファ長(buf_size) が十分な場合にコピーされる
 *            バイトサイズ
 * @throw     メモリ破壊を起こした場合
 */
ssize_t mdarray::getdata_fast( void *dest_buf, size_t buf_size,
			       size_t total_copied_size,
			       ssize_t idx0, ssize_t idx1, ssize_t idx2 ) const
{
    const ssize_t idx = this->get_idx_3d_cs(idx0,idx1,idx2);
    size_t sz, full_sz;
    if ( idx < 0 ) {
	err_report(__FUNCTION__,"WARNING","invalid args; ignored");
	return -1;
    }
    if ( 0 < buf_size && dest_buf == NULL ) return -1;
    sz = this->cached_bytes_rec * (this->cached_length_rec - idx);
    full_sz = sz;
    if ( buf_size < sz ) sz = buf_size;
    s_memcpy( dest_buf, 
	    (const char *)(this->data_ptr_cs()) + this->cached_bytes_rec * idx,
	    sz, total_copied_size );
    return full_sz;
}

/**
 * @brief  ユーザ・バッファの内容を自身の配列へコピー (SIMDストリーム命令使用)
 *
 *  src_buf で指定されたユーザ・バッファの内容を自身の配列へコピーします．
 *  コピーされる領域が十分に大きい場合，SIMD ストリーム命令を使って高速な
 *  コピーを行ないます．<br>
 *  バッファの大きさ buf_size は，バイト単位で与えます．idx0, idx1, idx2 で
 *  自身の配列での開始点を指定します．<br>
 *  引数に，MDARRAY_INDEF を明示的に与えないでください．
 *
 * @param     src_buf  ユーザ・バッファのアドレス (コピー元)
 * @param     buf_size ユーザ・バッファのサイズ (バイト単位)
 * @param     total_copied_size getdata_fast()が複数回呼び出される時の
 *                              トータルでコピーされるバイトサイズ
 * @param     idx0 次元番号 0 の次元(1次元目)の要素番号 (省略可)
 * @param     idx1 次元番号 1 の次元(2次元目)の要素番号 (省略可)
 * @param     idx2 次元番号 2 の次元(3次元目)の要素番号 (省略可)
 * @return    ユーザのバッファ長(buf_size) が十分な場合にコピーされる
 *            バイトサイズ
 * @throw     メモリ破壊を起こした場合
 */
ssize_t mdarray::putdata_fast( const void *src_buf, size_t buf_size,
			       size_t total_copied_size,
			       ssize_t idx0, ssize_t idx1, ssize_t idx2 )
{
    const ssize_t idx = this->get_idx_3d_cs(idx0,idx1,idx2);
    size_t sz, full_sz;
    if ( idx < 0 ) {
	err_report(__FUNCTION__,"WARNING","invalid args; ignored");
	return -1;
    }
    if ( 0 < buf_size && src_buf == NULL ) return -1;
    sz = this->cached_bytes_rec * (this->cached_length_rec - idx);
    full_sz = sz;
    if ( buf_size < sz ) sz = buf_size;
    s_memcpy( (char *)(this->data_ptr()) + this->cached_bytes_rec * idx,
	       src_buf, sz, total_copied_size );
    return full_sz;
}


/*
 * member functions to update length, type, etc.
 */

/**
 * @brief  次元数を1つ拡張
 *
 *  自身が持つ配列の次元数を 1つ拡張します．
 *
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合
 */
mdarray &mdarray::increase_dim()
{
    size_t sz;
    size_t *srec_ptr;

    if ( this->realloc_size_rec(this->dim_size_rec + 1) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","realloc() failed");
    }
    /* 可能ならば1個分を確保する */
    if ( this->data_ptr() == NULL ) {
	size_t i;
	sz = this->bytes();
	/* this->_size_rec[i] にゼロが入ってないか確認 */
	for ( i=0 ; i < this->dim_size_rec ; i++ ) sz *= this->_size_rec[i];
	if ( 0 < sz ) {
	    if ( this->realloc_arr_rec(sz) < 0 ) {
		err_throw(__FUNCTION__,"FATAL","realloc() failed");
	    }
	    if ( this->extptr_rec != NULL ) {
		*(this->extptr_rec) = this->data_ptr();
	    }
	    if ( this->default_value_ptr() != NULL ) {
		c_memcpy(this->data_ptr(), this->default_value_ptr(), sz);
	    }
	    else {
		c_memset(this->data_ptr(), 0, sz);
	    }
	}
    }
    srec_ptr = this->size_rec_ptr();
    srec_ptr[this->dim_size_rec] = 1;
    this->dim_size_rec ++;

    /* update cached length info and call user's callback func */
    this->update_length_info();
    /* update_length_info() の直後に呼ぶ必要がある */
    this->update_arr_ptr_2d();
    this->update_arr_ptr_3d();

    return *this;
}

/**
 * @brief  次元数を1つ縮小
 *
 *  自身が持つ配列の次元を 1つ縮小します．
 *
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 */
mdarray &mdarray::decrease_dim()
{
    size_t i;

    if ( this->dim_size_rec == 0 ) {
	return *this;
    }
    else if ( this->dim_size_rec == 1 ) {
	this->free_size_rec();
	this->free_arr_rec();
	if ( this->extptr_rec != NULL ) *(this->extptr_rec) = this->data_ptr();
	this->dim_size_rec = 0;
    }
    else {
	size_t new_buf_size = this->bytes();
	for ( i=0 ; i + 1 < this->dim_size_rec ; i++ ) {
	    new_buf_size *= this->_size_rec[i];
	}
	this->dim_size_rec--;
	/* */
	if ( this->realloc_arr_rec(new_buf_size) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","realloc() failed");
	}
	if ( this->extptr_rec != NULL ) *(this->extptr_rec) = this->data_ptr();
    }

    /* update cached length info and call user's callback func */
    this->update_length_info();
    /* update_length_info() の直後に呼ぶ必要がある */
    this->update_arr_ptr_2d();
    this->update_arr_ptr_3d();

    return *this;
}

/**
 * @brief  最初の次元についての配列長の変更
 *
 *  自身が持つ配列の長さを変更します．<br>
 *  配列長を拡張する場合，要素の値はデフォルト値で埋められます．<br>
 *  配列長を収縮する場合，len以降の要素は削除されます．<br>
 *  常に次元番号 0 の次元(1次元目)を処理対象とします．
 *
 * @param     len 要素の個数
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合
 */
mdarray &mdarray::resize( size_t len )
{
    this->resize(0,len);
    if ( this->dim_size_rec == 1 && this->_size_rec[0] == 0 ) {
	this->decrease_dim();
    }
    return *this;
}

/**
 * @brief  任意の1つの次元についての配列長の変更
 *
 *  自身が持つ配列の長さを変更します．<br>
 *  配列長を拡張する場合，要素の値はデフォルト値で埋められます．<br>
 *  配列長を収縮する場合，len以降の要素は削除されます．<br>
 *  次元番号 dim_index で処理対象とする次元を指定できます．
 *
 * @param     dim_index 次元番号
 * @param     len 要素の個数
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合
 */
mdarray &mdarray::resize( size_t dim_index, size_t len )
{
    return this->do_resize(dim_index, len, this->auto_init());
}

/**
 * @brief  配列長の変更 (他オブジェクトからコピー)
 *
 *  自身の次元数と配列長を，オブジェクトsrc が持つものと同じ大きさにします．
 *  配列長を拡張する場合，要素の値はデフォルト値で埋められます．<br>
 *  配列長を収縮する場合，配列長に満たない部分の要素は削除されます．<br>
 *
 * @param     src 配列長のコピー元
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合
 */
mdarray &mdarray::resize( const mdarray &src )
{
    debug_report("(const mdarray &src)");
    this->resize( src.cdimarray(), src.dim_size_rec, this->auto_init());
    return *this;
}

/**
 * @brief  配列長の変更 (1次元)
 *
 *  自身が持つ配列の長さを変更し，配列長 x_len の1次元配列とします．
 */
mdarray &mdarray::resize_1d( size_t x_len )
{
    const size_t nx[] = {x_len};
    return this->resize(nx,1,true);
}

/**
 * @brief  配列長の変更 (2次元)
 *
 *  自身が持つ配列の長さを変更し，配列長 (x_len, y_len) の2次元配列とします．
 */
mdarray &mdarray::resize_2d( size_t x_len, size_t y_len )
{
    const size_t nx[] = {x_len, y_len};
    return this->resize(nx,2,true);
}

/**
 * @brief  配列長の変更 (3次元)
 *
 *  自身が持つ配列の長さを変更し，配列長 (x_len, y_len, z_len) の3次元配列と
 *  します．
 */
mdarray &mdarray::resize_3d( size_t x_len, size_t y_len, size_t z_len )
{
    const size_t nx[] = {x_len, y_len, z_len};
    return this->resize(nx,3,true);
}

/**
 * @brief  リサイズ後に必要とされる部分の要素を バッファの前方に詰める (内部用)
 *
 * @note   この関数は次の mdarray::resize() から再帰的に呼ばれる．<br>
 *         private な関数です．
 */
static void accumulate_required_data( size_t bytes, const size_t *naxisx,
				      const size_t *min_naxisx, size_t dim_ix,
				      size_t blen_block_back,
				      const unsigned char **back_p, 
				      unsigned char **front_p )
{
    if ( dim_ix == 0 ) {
	c_memmove(*front_p, *back_p, bytes * min_naxisx[dim_ix]);
	*front_p += bytes * min_naxisx[dim_ix];
	*back_p += bytes * naxisx[dim_ix];
    }
    else {
	size_t i;
	blen_block_back /= naxisx[dim_ix];
	for ( i=0 ; i < min_naxisx[dim_ix] ; i++ ) {
	    accumulate_required_data(bytes, naxisx, min_naxisx, dim_ix - 1,
				     blen_block_back,
				     back_p, front_p);
	}
	*back_p += blen_block_back * (naxisx[dim_ix] - min_naxisx[dim_ix]);
    }
    return;
}

/**
 * @brief  accumulate_required_data() でバッファの前方に集めたデータを再分配
 *
 * @note  この関数は次の mdarray::resize() から再帰的に呼ばれる．<br>
 *        private な関数です．
 */
static void distribute_required_data( size_t bytes, const size_t *naxisx,
				      const size_t *min_naxisx, 
				      size_t n_min_naxisx, size_t dim_ix,
				      size_t len_block_back,
				      unsigned char **back_p, 
				      const unsigned char **front_p,
				      bool init_buf, const void *default_v )
{
    if ( dim_ix == 0 ) {
	size_t blen_move = bytes * min_naxisx[dim_ix];
	*front_p -= blen_move;
	*back_p -= bytes * naxisx[dim_ix];
	c_memmove(*back_p, *front_p, blen_move);
	if ( init_buf == true ) {
	    size_t n_gap = naxisx[dim_ix] - min_naxisx[dim_ix];
	    if ( 0 < n_gap ) {
		if ( default_v == NULL ) 
		    c_memset(*back_p + blen_move, 0, bytes * n_gap);
		else 
		    memfill(*back_p + blen_move, default_v, bytes, n_gap);
	    }
	}
    }
    else {
	size_t i;
	size_t n_gap;
	size_t current_min_naxisx = 1;
	if ( dim_ix < n_min_naxisx ) current_min_naxisx = min_naxisx[dim_ix];
	/* */
	len_block_back /= naxisx[dim_ix];
	/* */
	n_gap = len_block_back * (naxisx[dim_ix] - current_min_naxisx);
	if ( 0 < n_gap ) {
	    *back_p -= bytes * n_gap;
	    if ( init_buf == true ) {
		if ( default_v == NULL ) c_memset(*back_p, 0, bytes * n_gap);
		else memfill(*back_p, default_v, bytes, n_gap);
	    }
	}
	for ( i=0 ; i < current_min_naxisx ; i++ ) {
	    distribute_required_data(bytes, naxisx, min_naxisx, n_min_naxisx, 
	     dim_ix - 1, len_block_back, back_p, front_p, init_buf, default_v);
	}
    }
    return;
}

/* change the length of the array */
/**
 * @brief  配列長の変更 (複数の次元を指定可)
 *
 *  自身が持つ配列の長さを変更します．<br>
 *  配列長を拡張する場合，要素の値をデフォルト値で埋るかどうかは init_buf で
 *  設定できます．<br>
 *  配列長を収縮する場合，配列長に満たない部分の要素は削除されます．<br>
 *
 * @param     naxisx[] 各次元の要素数
 * @param     ndim 配列次元数
 * @param     init_buf 配列長の拡張時，要素値をデフォルト値で埋めるなら true
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合
 * @note      2次元以上のリサイズの場合，低コストで行なえます．1次元だけリサイ
 *            ズするなら，resize(dim_index, ...) の方が低コストです．
 */
mdarray &mdarray::resize(const size_t naxisx_in[], size_t ndim, bool init_buf)
{
    heap_mem<size_t> naxisx;
    heap_mem<size_t> min_naxisx;
    size_t bytes = zt2bytes(this->sz_type_rec);
    size_t i;

    if ( 0 < ndim && naxisx_in == NULL ) {
	err_throw(__FUNCTION__,"ERROR","0 < ndim, but NULL naxisx arg");
    }

    if ( ndim == 0 ) {
	this->reallocate(NULL,0,init_buf);
	goto quit;
    }

    /* 軸情報を作る．MDARRAY_ALL が指定された場合は現状維持にする */
    if ( naxisx.allocate(ndim) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","malloc() failed");
    }
    for ( i=0 ; i < ndim ; i++ ) {
	if ( naxisx_in[i] == MDARRAY_ALL ) naxisx[i] = this->length(i);
	else naxisx[i] = naxisx_in[i];
    }

    if ( this->length() == 0 ) {
	this->reallocate(naxisx.ptr(), ndim, init_buf);
    }
    else {
	/* それぞれの次元長で現在のと naxisx[] とを比べて最小のものを保存 */
	size_t n_min_naxisx;
	size_t i, this_blen_all, min_blen_all;
	const unsigned char *back_p0 = (const unsigned char *)(this->data_ptr());
	unsigned char *front_p0 = (unsigned char *)(this->data_ptr());
	unsigned char *back_p1;
	const unsigned char *front_p1;
	bool eq_flag = (this->dim_size_rec == ndim);
	/* 小さい方の次元数 */
	if ( ndim < this->dim_size_rec ) n_min_naxisx = ndim;
	else n_min_naxisx = this->dim_size_rec;
	/* 各次元で配列長の小さい方をとってくる */
	if ( min_naxisx.allocate(n_min_naxisx) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","malloc() failed");
	}
	this_blen_all = bytes;
	min_blen_all = bytes;
	for ( i=0 ; i < n_min_naxisx ; i++ ) {
	    if ( naxisx[i] < this->_size_rec[i] ) min_naxisx[i] = naxisx[i];
	    else min_naxisx[i] = this->_size_rec[i];
	    this_blen_all *= this->_size_rec[i];
	    min_blen_all *= min_naxisx[i];
	    if ( naxisx[i] != this->_size_rec[i] ) eq_flag = false;
	}
	/* 次元長，各次元の配列長がすべて同じなら終了 */
	if ( eq_flag == true ) goto quit;
	/* リサイズ後に必要な部分のデータのみ，すべてバッファの前方に集める */
	accumulate_required_data(bytes, this->cdimarray(), min_naxisx.ptr(), 
				 n_min_naxisx - 1, this_blen_all,
				 &back_p0, &front_p0);
	/* バッファのリサイズ */
	this->reallocate(naxisx.ptr(), ndim, false);
	/* それぞれの一番最後 + 1 */
	back_p1 = (unsigned char *)(this->data_ptr()) + this->byte_length();
	front_p1 = (const unsigned char *)(this->data_ptr()) + min_blen_all;
	/* バッファの前方に集めたデータを所定の位置に再配置する */
	distribute_required_data(bytes, this->cdimarray(), min_naxisx.ptr(), 
				 n_min_naxisx, this->dim_size_rec - 1, 
				 this->length(), &back_p1, &front_p1, 
				 init_buf, this->default_value_ptr());
    }

 quit:
    return *this;
}

/**
 * @brief  "10,10" のような文字列から配列長情報を取得 (内部用)
 *
 * @note  resizef(), vresizef() で使用．<br>
 *        private な関数です．
 */
static int parse_size_exp( heap_mem<size_t> *rt_szinfo, size_t *rt_n_szinfo, 
			  const mdarray &obj, const char *exp_fmt, va_list ap )
{
    int ret_status = -1;
    heap_mem<size_t> &sizeinfo = *rt_szinfo;
    heap_mem<char> sizeexp;
    size_t ix, n_el;

    if ( exp_fmt == NULL ) {
	ret_status = 0;
	goto quit;
    }

    if ( c_vasprintf(sizeexp.ptr_address(), exp_fmt, ap) < 0 ) {
	err_report(__FUNCTION__,"FATAL","c_vasprintf() failed");
	goto quit;
    }

    /* count elements */
    ix = 0;  n_el = 0;
    while ( 0 < sizeexp[ix] && sizeexp[ix] <= ' ' ) ix ++;
    if (sizeexp[ix] == '(' || sizeexp[ix] == '[' || sizeexp[ix] == '{') ix ++;
    while ( 1 ) {
	while ( 0 < sizeexp[ix] && sizeexp[ix] <= ' ' ) ix ++;
	while ( sizeexp[ix] != ',' && sizeexp[ix] != '\0' ) ix ++;
	n_el ++;
	if ( sizeexp[ix] == ',' ) ix ++;
	else break;
    };
    *rt_n_szinfo = n_el;

    if ( sizeinfo.allocate(*rt_n_szinfo) < 0 ) {
	err_report(__FUNCTION__,"FATAL","malloc() failed");
	goto quit;
    }

    /* get elements */
    ix = 0;  n_el = 0;
    while ( 0 < sizeexp[ix] && sizeexp[ix] <= ' ' ) ix ++;
    if (sizeexp[ix] == '(' || sizeexp[ix] == '[' || sizeexp[ix] == '{') ix ++;
    while ( 1 ) {
	while ( 0 < sizeexp[ix] && sizeexp[ix] <= ' ' ) ix ++;
	if ( ('0' <= sizeexp[ix] && sizeexp[ix] <= '9') || 
	     sizeexp[ix] == '+' ) 
	    sizeinfo[n_el] = c_strtoul(sizeexp.ptr() + ix, (char **)NULL, 10);
	else sizeinfo[n_el] = obj.length(n_el);		/* 長さ変更せず */
	while ( sizeexp[ix] != ',' && sizeexp[ix] != '\0' ) ix ++;
	n_el ++;
	if ( sizeexp[ix] == ',' ) ix ++;
	else break;
    };
	
    ret_status = 0;
 quit:
    return ret_status;
}

/**
 * @brief  配列長の変更 (IDLの記法)
 *
 *  自身が持つ配列の長さを変更します．長さの指定は，printf() 形式の可変長引数で
 *  指定可能なカンマ区切りの文字列(例えば"10,5")によって行ないます．<br>
 *  指定された文字列中の要素が数字ではない場合，その次元は長さを変更しません．
 *  例えば， ",2" が指定された場合，最初の次元の長さは変更されません．
 *
 * @param   exp_fmt csv形式の長さ指定文字列のためのフォーマット指定
 * @param   ...     exp_fmtに対応した可変長引数の各要素データ
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
mdarray &mdarray::resizef( const char *exp_fmt, ... )
{
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	this->vresizef(exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vresizef() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  配列長の変更 (IDLの記法)
 *
 *  自身が持つ配列の長さを変更します．長さの指定は，printf() 形式の可変長引数で
 *  指定可能なカンマ区切りの文字列(例えば"10,5")によって行ないます．<br>
 *  指定された文字列中の要素が数字ではない場合，その次元は長さを変更しません．
 *  例えば， ",2" が指定された場合，最初の次元の長さは変更されません．
 *
 * @param   exp_fmt csv形式の長さ指定文字列のためのフォーマット指定
 * @param   ap      exp_fmtに対応した可変長引数のリスト
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
mdarray &mdarray::vresizef( const char *exp_fmt, va_list ap )
{
    heap_mem<size_t> nx;
    size_t len_nx = 0;
    if ( parse_size_exp(&nx, &len_nx, *this, exp_fmt, ap) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","Error in parse_size_exp()");
    }
    if ( nx.ptr() == NULL ) goto quit;
    this->resize(nx.ptr(), len_nx, this->auto_init());
 quit:
    return *this;
}

/**
 * @brief  最初の次元について配列長の相対的な変更
 *
 *  自身が持つ配列の長さを len の指定分，拡張・縮小します．<br>
 *  resizeby() 後の配列サイズは，元の配列の長さに len を加えたものとなります．
 *  サイズの縮小は，len にマイナス値を指定することによって行います．<br>
 *  常に次元番号 0 の次元(1次元目)を処理対象とします．
 *
 * @param     len 要素個数の増分・減分
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合
 */
mdarray &mdarray::resizeby( ssize_t len )
{
    return this->resizeby(0, len);
}

/**
 * @brief  任意の1つの次元について配列長の相対的な変更
 *
 *  自身が持つ配列の長さを len の指定分，拡張・縮小します．<br>
 *  resizeby() 後の配列サイズは，元の配列の長さに len を加えたものとなります．
 *  サイズの縮小は，len にマイナス値を指定することによって行います．<br>
 *  次元番号 dim_index で処理対象とする次元を指定できます．
 *
 * @param     dim_index 次元番号
 * @param     len 要素個数の増分・減分
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合
 */
mdarray &mdarray::resizeby( size_t dim_index, ssize_t len )
{
    size_t new_len;
    if ( len < 0 ) {
	if ( abs_sz2z(len) < this->length(dim_index) ) 
	    new_len = this->length(dim_index) - abs_sz2z(len);
	else
	    new_len = 0;
    }
    else {
	new_len = this->length(dim_index) + abs_sz2z(len);
    }

    return this->do_resize(dim_index, new_len, this->auto_init());
}

/* change the length of the 1-d array relatively */
/**
 * @brief  配列長の相対的な変更 (1次元用)
 */
mdarray &mdarray::resizeby_1d( ssize_t x_len )
{
    const ssize_t nx[] = {x_len};
    return this->resizeby(nx, 1, true);
}

/* change the length of the 2-d array relatively */
/**
 * @brief  配列長の相対的な変更 (2次元用)
 */
mdarray &mdarray::resizeby_2d( ssize_t x_len, ssize_t y_len )
{
    const ssize_t nx[] = {x_len, y_len};
    return this->resizeby(nx, 2, true);
}

/* change the length of the 3-d array relatively */
/**
 * @brief  配列長の相対的な変更 (3次元用)
 */
mdarray &mdarray::resizeby_3d(ssize_t x_len, ssize_t y_len, ssize_t z_len)
{
    const ssize_t nx[] = {x_len, y_len, z_len};
    return this->resizeby(nx, 3, true);
}

/**
 * @brief  配列長の相対的な変更 (複数の次元を指定可)
 *
 *  自身が持つ配列の長さを naxisx_rel[] の指定分，拡張・縮小します．<br>
 *  resizeby() 後の配列サイズは，元の配列の長さに naxisx_rel[] を加えたもの
 *  となります．サイズの縮小は，naxisx_rel[] にマイナス値を指定することによって
 *  行います．<br>
 *  init_buf でサイズ拡張時に新規に作られる配列要素の初期化をするかどうかを
 *  指定できます．
 *
 * @param     naxisx_rel 要素個数の増分・減分
 * @param     ndim naxisx_rel[] の個数
 * @param     init_buf 新規に作られる配列要素の初期化を行なう場合は true
 * @return    自身の参照
 * @note      次元長を増やす事はできますが，減らす事はできません．
 * @throw     内部バッファの確保に失敗した場合
 */
mdarray &mdarray::resizeby( const ssize_t naxisx_rel[], size_t ndim, 
			    bool init_buf )
{
    heap_mem<size_t> n_axisx;
    size_t new_ndim = this->dim_size_rec;	/* 現在の次元数 */
    size_t i;

    if ( 0 < ndim && naxisx_rel == NULL ) {
	err_throw(__FUNCTION__,"ERROR","0 < ndim, but NULL naxisx arg");
    }

    if ( new_ndim < ndim ) new_ndim = ndim;	/* 次元数が増える場合 */

    if ( 0 < new_ndim ) {
	if ( n_axisx.allocate(new_ndim) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","malloc() failed");
	}
    }

    for ( i=0 ; i < new_ndim ; i++ ) {
	size_t len = this->length(i);		/* length() に任せる */
	if ( i < ndim ) {
	    ssize_t rinc = naxisx_rel[i];
	    if ( 0 <= rinc ) len += rinc;
	    else if ( abs_sz2z(rinc) <= len ) len -= abs_sz2z(rinc);
	}
	n_axisx[i] = len;
    }

    this->resize(n_axisx.ptr(), new_ndim, init_buf);

    return *this;
}

/**
 * @brief  "10,-10" のような文字列から相対配列長情報を取得 (内部用)
 *
 * @note  resizebyf(), vresizebyf() で使用．<br>
 *        private な関数です．
 */
static int parse_relsize_exp(heap_mem<ssize_t> *rt_szinfo, size_t *rt_n_szinfo,
			     const char *exp_fmt, va_list ap)
{
    int ret_status = -1;
    heap_mem<ssize_t> &sizeinfo = *rt_szinfo;
    heap_mem<char> sizeexp;
    size_t ix, n_el;

    if ( exp_fmt == NULL ) {
	ret_status = 0;
	goto quit;
    }

    if ( c_vasprintf(sizeexp.ptr_address(), exp_fmt, ap) < 0 ) {
	err_report(__FUNCTION__,"FATAL","c_vasprintf() failed");
	goto quit;
    }

    /* count elements */
    ix = 0;  n_el = 0;
    while ( 0 < sizeexp[ix] && sizeexp[ix] <= ' ' ) ix ++;
    if (sizeexp[ix] == '(' || sizeexp[ix] == '[' || sizeexp[ix] == '{') ix ++;
    while ( 1 ) {
	while ( 0 < sizeexp[ix] && sizeexp[ix] <= ' ' ) ix ++;
	while ( sizeexp[ix] != ',' && sizeexp[ix] != '\0' ) ix ++;
	n_el ++;
	if ( sizeexp[ix] == ',' ) ix ++;
	else break;
    };
    *rt_n_szinfo = n_el;

    if ( sizeinfo.allocate(*rt_n_szinfo) < 0 ) {
	err_report(__FUNCTION__,"FATAL","malloc() failed");
	goto quit;
    }

    /* get elements */
    ix = 0;  n_el = 0;
    while ( 0 < sizeexp[ix] && sizeexp[ix] <= ' ' ) ix ++;
    if (sizeexp[ix] == '(' || sizeexp[ix] == '[' || sizeexp[ix] == '{') ix ++;
    while ( 1 ) {
	while ( 0 < sizeexp[ix] && sizeexp[ix] <= ' ' ) ix ++;
	if ( ('0' <= sizeexp[ix] && sizeexp[ix] <= '9') || 
	     sizeexp[ix] == '+' || sizeexp[ix] == '-' ) 
	    sizeinfo[n_el] = c_strtol(sizeexp.ptr() + ix, (char **)NULL, 10);
	else sizeinfo[n_el] = 0;	/* 変更しない場合はゼロ */
	while ( sizeexp[ix] != ',' && sizeexp[ix] != '\0' ) ix ++;
	n_el ++;
	if ( sizeexp[ix] == ',' ) ix ++;
	else break;
    };
	
    ret_status = 0;
 quit:
    return ret_status;
}

/**
 * @brief  配列長の相対的な変更 (IDLの記法)
 *
 *  自身が持つ配列の長さを指定分，拡張・縮小します．長さの指定は，printf() 形式
 *  の可変長引数で指定可能なカンマ区切りの文字列(例えば"10,-5")によって行ない
 *  ます．<br>
 *  resizebyf()後の配列サイズは，元の配列の長さに指定分を加えたものとなります．
 *  サイズの縮小は，マイナス値を指定することによって行います．<br>
 *  指定された文字列中の要素が数字ではない場合，その次元は長さを変更しません．
 *  例えば， ",2" が指定された場合，最初の次元の長さは変更されません．
 *
 * @param   exp_fmt csv形式の長さ指定文字列のためのフォーマット指定
 * @param   ...     exp_fmtに対応した可変長引数の各要素データ
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
mdarray &mdarray::resizebyf( const char *exp_fmt, ... )
{
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	this->vresizebyf(exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vresizebyf() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  配列長の相対的な変更 (IDLの記法)
 *
 *  自身が持つ配列の長さを指定分，拡張・縮小します．長さの指定は，printf() 形式
 *  の可変長引数で指定可能なカンマ区切りの文字列(例えば"10,-5")によって行ない
 *  ます．<br>
 *  resizebyf()後の配列サイズは，元の配列の長さに指定分を加えたものとなります．
 *  サイズの縮小は，マイナス値を指定することによって行います．<br>
 *  指定された文字列中の要素が数字ではない場合，その次元は長さを変更しません．
 *  例えば， ",2" が指定された場合，最初の次元の長さは変更されません．
 *
 * @param   exp_fmt csv形式の長さ指定文字列のためのフォーマット指定
 * @param   ap      exp_fmtに対応した可変長引数のリスト
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
mdarray &mdarray::vresizebyf( const char *exp_fmt, va_list ap )
{
    heap_mem<ssize_t> nx;
    size_t len_nx = 0;
    if ( parse_relsize_exp(&nx, &len_nx, exp_fmt, ap) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","Error in parse_relsize_exp()");
    }
    if ( nx.ptr() == NULL ) goto quit;
    this->resizeby(nx.ptr(), len_nx, this->auto_init());
 quit:
    return *this;
}

/* change length of array without adjusting buffer contents */
/**
 * @brief  配列のバッファ内位置の調整をせずに，配列用バッファを再確保
 *
 *  配列のバッファ内位置に調整をせずに，自身が持つ次元の大きさと配列の長さを
 *  変更します．つまり，配列用バッファに対しては単純にrealloc()を呼ぶだけの処理
 *  を行ないます．
 *
 * @param   naxisx[] 各次元の要素数
 * @param   ndim 配列次元数
 * @param   init_buf 新たに確保した部分の要素値をデフォルト値で埋めるなら true
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
mdarray &mdarray::reallocate( const size_t naxisx[], size_t ndim, 
			      bool init_buf )
{
    const size_t bytes = zt2bytes(this->sz_type_rec);
    const size_t old_len = this->length();
    const size_t old_dim_len = this->dim_size_rec;
    size_t i, len;

    if ( 0 < ndim && naxisx == NULL ) {
	err_throw(__FUNCTION__,"ERROR","0 < ndim, but NULL naxisx arg");
    }

    /* 新しい全配列長をチェック */
    if ( ndim == 0 ) len = 0;
    else {
	len = 1;
	for ( i=0 ; i < ndim ; i++ ) {
	    /* 要素数0も許す */
	    if ( naxisx[i] == MDARRAY_ALL ) len *= this->length(i);
	    else len *= naxisx[i];
	}
    }
    /* 配列用バッファ長が大きくなる場合，先に realloc しておく */
    if ( old_len < len ) {
	if ( this->realloc_arr_rec(bytes * len) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","realloc() failed");
	}
	if ( this->extptr_rec != NULL ) *(this->extptr_rec) = this->data_ptr();
	/* 必要な場合だけ初期化 */
	if ( init_buf == true ) {
	    char *begin_ptr = (char *)(this->data_ptr()) + (bytes * old_len);
	    size_t n_spc = len - old_len;
	    if ( this->default_value_ptr() != NULL ) {
		s_memfill(begin_ptr, this->default_value_ptr(), bytes, n_spc, 0);
	    }
	    else {
		s_memset(begin_ptr, 0, bytes * n_spc, 0);
	    }
	}
    }
    /* 次元情報管理用の配列の更新 */
    if ( old_dim_len != ndim ) {
	if ( this->realloc_size_rec(ndim) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","realloc() failed");
	}
	this->dim_size_rec = ndim;
    }
    for ( i=0 ; i < ndim ; i++ ) {
	size_t *srec_ptr = this->size_rec_ptr();
	srec_ptr[i] = naxisx[i];
    }
    /* 配列用バッファ長が小さくなる場合のrealloc */
    if ( len < old_len ) {
	if ( this->realloc_arr_rec(bytes * len) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","realloc() failed");
	}
	if ( this->extptr_rec != NULL ) *(this->extptr_rec) = this->data_ptr();
    }

    /* update cached length info and call user's callback func */
    this->update_length_info();
    /* update_length_info() の直後に呼ぶ必要がある */
    this->update_arr_ptr_2d();
    this->update_arr_ptr_3d();

    return *this;
}

/* free current buffer and alloc new memory */
/**
 * @brief  配列用バッファを一旦開放し，新規に確保
 *
 *  配列の内容を一旦破棄し，配列の長さを変更します．つまり，配列用バッファに
 *  対して free()，malloc() を呼ぶだけの処理を行ないます．
 *
 * @param   naxisx[] 各次元の要素数
 * @param   ndim 配列次元数
 * @param   init_buf 新たに確保した部分の要素値をデフォルト値で埋めるなら true
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
mdarray &mdarray::allocate( const size_t naxisx[], size_t ndim, 
			    bool init_buf )
{
    const size_t bytes = zt2bytes(this->sz_type_rec);
    size_t i, len;

    if ( 0 < ndim && naxisx == NULL ) {
	err_throw(__FUNCTION__,"ERROR","0 < ndim, but NULL naxisx arg");
    }

    /* 新しい全配列長をチェック */
    if ( ndim == 0 ) len = 0;
    else {
	len = 1;
	for ( i=0 ; i < ndim ; i++ ) {
	    /* 要素数0も許す */
	    if ( naxisx[i] == MDARRAY_ALL ) len *= this->length(i);
	    else len *= naxisx[i];
	}
    }

    /* 開放 */
    if ( this->realloc_arr_rec(0) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","realloc() failed");
    }
    if ( this->realloc_size_rec(0) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","realloc() failed");
    }
    this->dim_size_rec = 0;

    /* 確保 */
    if ( this->realloc_size_rec(ndim) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","realloc() failed");
    }
    if ( this->realloc_arr_rec(bytes * len) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","realloc() failed");
    }
    if ( this->extptr_rec != NULL ) *(this->extptr_rec) = this->data_ptr();

    /* 次元情報管理用の配列の更新 */
    this->dim_size_rec = ndim;
    for ( i=0 ; i < ndim ; i++ ) {
	size_t *srec_ptr = this->size_rec_ptr();
	srec_ptr[i] = naxisx[i];
    }

    /* 必要な場合だけ初期化 */
    if ( init_buf == true ) {
	if ( this->default_value_ptr() != NULL ) {
	    s_memfill(this->data_ptr(), this->default_value_ptr(), bytes, len, 0);
	}
	else {
	    s_memset(this->data_ptr(), 0, bytes * len, 0);
	}
    }

    /* update cached length info and call user's callback func */
    this->update_length_info();
    /* update_length_info() の直後に呼ぶ必要がある */
    this->update_arr_ptr_2d();
    this->update_arr_ptr_3d();

    return *this;
}

/**
 * @brief  サイズ拡張時の初期値の設定
 *
 *  配列サイズ拡張時の初期値を設定します．設定された値は既存の要素には作用
 *  せず，サイズ拡張時に有効となります．
 *
 * @param     value 配列サイズ拡張時の初期値
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合
 */
mdarray &mdarray::assign_default( double value )
{
    void (*func_d2dest)(const void *,void *,size_t,int,void *);
    struct mdarray_cnv_nd_prms prms;

    prms.total_len_elements = 0;			/* 効かない */
    prms.round_flg = this->rounding();			/* 四捨五入/切り捨て */

    /* 型変換のための関数 (double→自身の型) */
    func_d2dest = this->func_cnv_nd_d2x;

    if ( func_d2dest == NULL ) {
	err_report(__FUNCTION__,"WARNING","invalid sz_type; ignored");
	return *this;
    }

    (*func_d2dest)(&value, this->junk_rec, 1, +1, (void *)&prms);

    return this->assign_default(this->junk_rec);
}

/**
 * @brief  サイズ拡張時の初期値の設定 (低レベル)
 *
 *  配列サイズ拡張時の初期値を設定します．設定された値は既存の要素には作用
 *  せず，サイズ拡張時に有効となります．
 *
 * @param     value_ptr 配列サイズ拡張時の初期値のアドレス
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合
 */
mdarray &mdarray::assign_default( const void *value_ptr )
{
    const size_t sz = this->bytes();
    if ( value_ptr != NULL ) {
      if ( 0 < sz ) {
	if ( this->default_rec_ptr() != NULL ) {
	    c_memmove(this->default_rec_ptr(),value_ptr,sz);
	}
	else {
	    if ( this->realloc_default_rec(sz) < 0 ) {
		err_throw(__FUNCTION__,"FATAL","realloc() failed");
	    }
	    c_memcpy(this->default_rec_ptr(),value_ptr,sz);
	}
      }
    }
    else {
	this->free_default_rec();
    }
    return *this;
}

/**
 * @brief  最初の次元について新規配列要素の挿入
 *
 *  自身の配列の要素位置 idx に，len 個分の要素を挿入します．なお，挿入される
 *  要素の値はデフォルト値です．<br>
 *  常に次元番号 0 の次元(1次元目)を処理対象とします．
 *
 * @param     idx 挿入位置の要素番号
 * @param     len 要素の個数
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合
 */
mdarray &mdarray::insert( ssize_t idx, size_t len )
{
    return this->insert(0,idx,len);
}

/* this->dim_size_rec <= dim_index の場合，this->resize() によって
   自動的に次元が追加される */
/**
 * @brief  任意の1つの次元について新規配列要素の挿入
 *
 *  自身の配列の要素位置 idx に，len 個分の要素を挿入します．なお，挿入される
 *  要素の値はデフォルト値です．<br>
 *  次元番号 dim_index で処理対象とする次元を指定できます．
 *
 * @param     dim_index 次元番号
 * @param     idx 挿入位置の要素番号
 * @param     len 要素の個数
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合
 */
mdarray &mdarray::insert( size_t dim_index, ssize_t idx, size_t len )
{
    return this->do_insert(dim_index, idx, len, this->auto_init());
}

/**
 * @brief  最初の次元について配列要素の削除
 *
 *  自身の配列から指定された部分の要素を削除します．削除した分，長さは短くなり
 *  ます．<br>
 *  常に次元番号 0 の次元(1次元目)を処理対象とします.
 *
 * @param     idx 開始位置の要素番号
 * @param     len 要素の個数
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合
 */
mdarray &mdarray::erase( ssize_t idx, size_t len )
{
    this->erase(0, idx, len);
    if ( this->dim_size_rec == 1 && this->_size_rec[0] == 0 ) {
	this->decrease_dim();
    }
    return *this;
}

/* this->dim_size_rec <= dim_index の場合，0 < dim_index なら
   this->resize() によって自動的に次元が追加される */
/**
 * @brief  任意の1つの次元について配列要素の削除
 *
 *  自身の配列から指定された部分の要素を削除します．削除した分，長さは短くなり
 *  ます．<br>
 *  次元番号 dim_index で処理対象とする次元を指定できます．
 *
 * @param     dim_index 次元番号
 * @param     idx 開始位置の要素番号
 * @param     len 要素の個数
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合
 */
mdarray &mdarray::erase( size_t dim_index, ssize_t idx, size_t len )
{
    const size_t bytes = this->bytes();
    size_t org_length = this->length(dim_index);
    size_t i, idx_0;
    size_t b_unit0, b_unit1, unit0;
    size_t count;

    if ( this->dim_size_rec <= dim_index ) {
	if ( dim_index == 0 ) return *this;
	this->resize(dim_index, 1);
	org_length = 1;
    }

    if ( len <= 0 ) return *this;
    if ( 0 <= idx && org_length <= abs_sz2z(idx) ) return *this;

    if ( idx < 0 ) {
	if ( abs_sz2z(idx) < len ) {
	    len -= abs_sz2z(idx);
	    idx = 0;
	} else {
	    return *this;
	}
    }

    idx_0 = (size_t)idx;

    if ( org_length < idx_0 + len ) len = org_length - idx_0;
    if ( len == 0 ) return *this;

    unit0 = 1;
    for ( i=0 ; i < dim_index ; i++ ) {
	unit0 *= this->_size_rec[i];
    }
    b_unit0 = bytes * unit0;
    b_unit1 = b_unit0 * this->_size_rec[dim_index];

    count = 1;
    for ( i=dim_index+1 ; i < this->dim_size_rec ; i++ ) {
	count *= this->_size_rec[i];
    }

    void *dptr = this->data_ptr();
    for ( i=0 ; i < count ; i++ ) {
      s_memmove( (char *)dptr + b_unit1 * i + b_unit0 * idx_0,
		 (char *)dptr + b_unit1 * i + b_unit0 * (idx_0 + len),
		 b_unit0 * (org_length - idx_0 - len) );
    }

    this->resize(dim_index, org_length - len);

    return *this;
}

/**
 * @brief  最初の次元について配列要素間での値のコピー
 *
 * 自身の配列要素間で値をコピーします．<br>
 * 引数 clr に false が指定された場合はコピー元の値は残りますが，true が指定
 * された場合はコピー元の値は残らずデフォルト値で埋められます．<br>
 * idx_dst に既存の配列長より大きな値を設定しても，配列サイズは変わりません．
 * この点が次の cpy() メンバ関数とは異なります．<br>
 * 常に次元番号 0 の次元(1次元目)を処理対象とします．
 *
 * @param     idx_src コピー元の要素番号
 * @param     len コピー元の要素の長さ
 * @param     idx_dst コピー先の要素番号
 * @param     clr コピー元の値のクリア可否
 * @return    自身の参照
 */
mdarray &mdarray::move( ssize_t idx_src, size_t len, ssize_t idx_dst, bool clr )
{
    return this->move(0, idx_src, len, idx_dst, clr);
}

/**
 * @brief  任意の1つの次元について配列要素間での値のコピー
 *
 *  自身の配列要素間で値をコピーします．<br>
 *  引数 clr に false が指定された場合はコピー元の値は残りますが，true が指定
 *  された場合はコピー元の値は残らずデフォルト値で埋められます．<br>
 *  idx_dst に既存の配列長より大きな値を設定しても，配列サイズは変わりません．
 *  この点が次の cpy() メンバ関数とは異なります．<br>
 *  次元番号 dim_index で処理対象とする次元を指定できます．
 *
 * @param     dim_index 次元番号
 * @param     idx_src コピー元の要素番号
 * @param     len コピー元の要素の長さ
 * @param     idx_dst コピー先の要素番号
 * @param     clr コピー元の値のクリア可否
 * @return    自身の参照
 */
mdarray &mdarray::move( size_t dim_index,
			ssize_t idx_src, size_t len, ssize_t idx_dst, bool clr )
{
    const size_t bytes = this->bytes();
    size_t org_length = this->length(dim_index);
    size_t i, idx_src_0, idx_dst_0;
    size_t b_unit0, b_unit1, unit0;
    size_t count;

    if ( this->dim_size_rec <= dim_index ) {
	if ( dim_index == 0 ) return *this;
	this->resize(dim_index, 1);
	org_length = 1;
    }

    if ( idx_src == idx_dst ) return *this;
    if ( 0 <= idx_src && org_length <= abs_sz2z(idx_src) ) return *this;
    if ( 0 <= idx_dst && org_length <= abs_sz2z(idx_dst) ) return *this;

    if ( idx_src < 0 ) {
	if ( abs_sz2z(idx_src) < len ) {
	    len -= abs_sz2z(idx_src);
	    idx_dst -= idx_src;
	    idx_src -= idx_src;
	} else {
	    return *this;
	}
    }
    if ( idx_dst < 0 ) {
	if ( abs_sz2z(idx_dst) < len ) {
	    len -= abs_sz2z(idx_dst);
	    idx_src -= idx_dst;
	    idx_dst -= idx_dst;
	} else {
	    return *this;
	}
    }

    idx_src_0 = (size_t)idx_src;
    idx_dst_0 = (size_t)idx_dst;

    if ( org_length < idx_src_0 + len ) len = org_length - idx_src_0;
    if ( org_length < idx_dst_0 + len ) len = org_length - idx_dst_0;
    if ( len <= 0 ) return *this;

    /* */
    unit0 = 1;
    for ( i=0 ; i < dim_index ; i++ ) {
	unit0 *= this->_size_rec[i];
    }
    b_unit0 = bytes * unit0;
    b_unit1 = b_unit0 * this->_size_rec[dim_index];

    count = 1;
    for ( i=dim_index+1 ; i < this->dim_size_rec ; i++ ) {
	count *= this->_size_rec[i];
    }

    const void *default_val_ptr = this->default_value_ptr();
    void *dptr = this->data_ptr();
    for ( i=0 ; i < count ; i++ ) {
       s_memmove( (char *)dptr + b_unit1 * i + b_unit0 * idx_dst_0,
		  (char *)dptr + b_unit1 * i + b_unit0 * idx_src_0,
		  b_unit0 * len );
       if ( clr ) {
	  size_t idx, flen;
	  if ( idx_src_0 < idx_dst_0 ) {
	      idx = idx_src_0;
	      if ( idx_dst_0 < idx_src_0 + len ) flen = idx_dst_0 - idx_src_0;
	      else flen = len;
	  }
	  else {
	      if ( idx_src_0 < idx_dst_0 + len ) {
		  idx = idx_dst_0 + len;
		  flen = idx_src_0 - idx_dst_0;
	      }
	      else {
		  idx = idx_src_0;
		  flen = len;
	      }
	  }
	  if ( default_val_ptr != NULL ) {
	      char *dest_ptr = (char *)dptr + b_unit1 * i + b_unit0 * idx;
	      s_memfill(dest_ptr, default_val_ptr, bytes, unit0 * flen,
			unit0 * flen * count);
	  }
	  else {
	      char *dest_ptr = (char *)dptr + b_unit1 * i + b_unit0 * idx;
	      s_memset(dest_ptr, 0, b_unit0 * flen, b_unit0 * flen * count);
	  }
       }
    }

    return *this;
}

/**
 * @brief  最初の次元について配列要素間での値のコピー (配列数は自動拡張)
 *
 *  自身の配列要素間で値をコピーします．<br>
 *  引数 clr に false が指定された場合はコピー元の値は残りますが，true が指定
 *  された場合はコピー元の値は残らずデフォルト値で埋められます．<br>
 *  idx_dst + len が既存の配列長より大きい場合，配列サイズは自動拡張されま
 *  す．<br>
 *  常に次元番号 0 の次元(1次元目)を処理対象とします．
 *
 * @param     idx_src コピー元の要素番号
 * @param     len コピー元の要素の長さ
 * @param     idx_dst コピー先の要素番号
 * @param     clr コピー元の値のクリア可否
 * @return    自身の参照
 */
mdarray &mdarray::cpy( ssize_t idx_src, size_t len, ssize_t idx_dst, bool clr )
{
    return this->cpy(0, idx_src, len, idx_dst, clr);
}

/**
 * @brief  任意の1つの次元について配列要素間での値のコピー (配列数は自動拡張)
 *
 *  自身の配列要素間で値をコピーします．<br>
 *  引数 clr に false が指定された場合はコピー元の値は残りますが，true が指定
 *  された場合はコピー元の値は残らずデフォルト値で埋められます．<br>
 *  idx_dst + len が既存の配列長より大きい場合，配列サイズは自動拡張されま
 *  す．<br>
 *  次元番号 dim_index で処理対象とする次元を指定できます．
 *
 * @param     dim_index 次元番号
 * @param     idx_src コピー元の要素番号
 * @param     len コピー元の要素の長さ
 * @param     idx_dst コピー先の要素番号
 * @param     clr コピー元の値のクリア可否
 * @return    自身の参照
 */
mdarray &mdarray::cpy( size_t dim_index,
		       ssize_t idx_src, size_t len, ssize_t idx_dst, bool clr )
{
    size_t idx_src_0, idx_dst_0;
    size_t org_length = this->length(dim_index);

    if ( this->dim_size_rec <= dim_index ) {
	if ( dim_index == 0 ) return *this;
	this->resize(dim_index, 1);
	org_length = 1;
    }

    if ( idx_src == idx_dst ) return *this;
    if ( 0 <= idx_src && org_length <= abs_sz2z(idx_src) ) return *this;

    if ( idx_src < 0 ) {
	if ( abs_sz2z(idx_src) < len ) {
	    len -= abs_sz2z(idx_src);
	    idx_dst -= idx_src;
	    idx_src -= idx_src;
	} else {
	    return *this;
	}
    }
    if ( idx_dst < 0 ) {
	if ( abs_sz2z(idx_dst) < len ) {
	    len -= abs_sz2z(idx_dst);
	    idx_src -= idx_dst;
	    idx_dst -= idx_dst;
	} else {
	    return *this;
	}
    }

    idx_src_0 = (size_t)idx_src;
    idx_dst_0 = (size_t)idx_dst;

    if ( org_length < idx_src_0 + len ) len = org_length - idx_src_0;

    /* */
    if ( org_length < idx_dst_0 + len ) {
	this->do_resize(dim_index, idx_dst_0 + len, this->auto_init());
    }
    return this->move(dim_index, idx_src, len, idx_dst, clr);
}

/**
 * @brief  最初の次元について配列要素間での値の入れ換え
 *
 *  自身の配列要素間で値を入れ替えます．<br>
 *  要素番号 idx_src から len 個分の要素を，要素番号 idx_dst から len 個分の要
 *  素と入れ替えます．<br>
 *  idx_dst + len が配列サイズを超える場合は，配列サイズまでの処理が行われま
 *  す．入れ替える領域が重なった場合，重なっていない src の領域に対してのみ入
 *  れ替え処理が行われます．
 *
 * @param     idx_src 入れ替え元の要素番号
 * @param     len 入れ替え元の要素の長さ
 * @param     idx_dst 入れ替え先の要素番号
 * @return    自身の参照
 */
mdarray &mdarray::swap( ssize_t idx_src, size_t len, ssize_t idx_dst )
{
    return this->swap(0, idx_src, len, idx_dst);
}

/**
 * @brief  任意の1つの次元について配列要素間での値の入れ換え
 *
 *  自身の配列要素間で値を入れ替えます．<br>
 *  次元番号 dim_index の要素番号 idx_src から len 個分の要素を，要素番号 
 *  idx_dst から len 個分の要素と入れ替えます．<br>
 *  idx_dst + len が配列サイズを超える場合は，配列サイズまでの処理が行われま
 *  す．入れ替える領域が重なった場合，重なっていない src の領域に対してのみ入
 *  れ替え処理が行われます．
 *
 * @param     dim_index 次元番号
 * @param     idx_src 入れ替え元の要素番号
 * @param     len 入れ替え元の要素の長さ
 * @param     idx_dst 入れ替え先の要素番号
 * @return    自身の参照
 */
mdarray &mdarray::swap( size_t dim_index,
			ssize_t idx_src, size_t len, ssize_t idx_dst )
{
    const size_t bytes = this->bytes();
    size_t org_length = this->length(dim_index);
    size_t i, idx_src_0, idx_dst_0;
    size_t b_unit0, b_unit1, unit0;
    size_t count;

    if ( this->dim_size_rec <= dim_index ) {
	if ( dim_index == 0 ) return *this;
	this->resize(dim_index, 1);
	org_length = 1;
    }

    if ( idx_src == idx_dst ) return *this;
    if ( 0 <= idx_src && org_length <= abs_sz2z(idx_src) ) return *this;
    if ( 0 <= idx_dst && org_length <= abs_sz2z(idx_dst) ) return *this;

    if ( idx_src < 0 ) {
	if ( abs_sz2z(idx_src) < len ) {
	    len -= abs_sz2z(idx_src);
	    idx_dst -= idx_src;
	    idx_src -= idx_src;
	} else {
	    return *this;
	}
    }
    if ( idx_dst < 0 ) {
	if ( abs_sz2z(idx_dst) < len ) {
	    len -= abs_sz2z(idx_dst);
	    idx_src -= idx_dst;
	    idx_dst -= idx_dst;
	} else {
	    return *this;
	}
    }

    if ( idx_dst < idx_src ) {
	idx_dst_0 = (size_t)idx_src;
	idx_src_0 = (size_t)idx_dst;
    }
    else {
	idx_src_0 = (size_t)idx_src;
	idx_dst_0 = (size_t)idx_dst;
    }

    if ( org_length < idx_src_0 + len ) len = org_length - idx_src_0;
    if ( org_length < idx_dst_0 + len ) len = org_length - idx_dst_0;
    if ( len <= 0 ) return *this;
    if ( idx_dst_0 == idx_src_0 ) return *this;

    /* 領域が重ならないようにする */
    if ( idx_dst_0 < idx_src_0 + len ) {
	len = idx_dst_0 - idx_src_0;
    }

    /* */
    unit0 = 1;
    for ( i=0 ; i < dim_index ; i++ ) {
	unit0 *= this->_size_rec[i];
    }
    b_unit0 = bytes * unit0;
    b_unit1 = b_unit0 * this->_size_rec[dim_index];

    count = 1;
    for ( i=dim_index+1 ; i < this->dim_size_rec ; i++ ) {
	count *= this->_size_rec[i];
    }

    /* */
    void *dptr = this->data_ptr();
    for ( i=0 ; i < count ; i++ ) {
	char *p0 = (char *)dptr + b_unit1 * i + b_unit0 * idx_dst_0;
	char *p1 = (char *)dptr + b_unit1 * i + b_unit0 * idx_src_0;
	size_t j;
	for ( j=0 ; j < b_unit0 * len ; j++ ) {
	    char tmp;
	    tmp = p0[j];
	    p0[j] = p1[j];
	    p1[j] = tmp;
	}
    }

    return *this;
}

/**
 * @brief  最初の次元について配列の不要部分の消去
 *
 *  自身の配列を，要素位置 idx から len 個の要素だけにします．<br>
 *  常に次元番号 0 の次元(1次元目)を処理対象とします．
 *
 * @param     idx 切り出し開始位置の要素番号
 * @param     len 要素の個数
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合
 */
mdarray &mdarray::crop( ssize_t idx, size_t len )
{
    this->crop(0, idx, len);
    if ( this->dim_size_rec == 1 && this->_size_rec[0] == 0 ) {
	this->decrease_dim();
    }
    return *this;
}

/**
 * @brief  任意の1つの次元について配列の不要部分の消去
 *
 *  自身の配列を，要素位置 idx から len 個の要素だけにします．<br>
 *  次元番号 dim_index で処理対象とする次元を指定できます．
 *
 * @param     dim_index 次元番号
 * @param     idx 切り出し開始位置の要素番号
 * @param     len 要素の個数
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合
 */
mdarray &mdarray::crop( size_t dim_index, ssize_t idx, size_t len )
{
    size_t idx_0;
    size_t org_length = this->length(dim_index);

    if ( this->dim_size_rec <= dim_index ) {
	if ( dim_index == 0 ) return *this;
	this->resize(dim_index, 1);
	org_length = 1;
    }

    if ( 0 <= idx && org_length <= abs_sz2z(idx) ) return *this;

    if ( idx < 0 ) {
	if ( abs_sz2z(idx) < len ) {
	    len -= abs_sz2z(idx);
	    idx = 0;
	} else {
	    return *this;
	}
    }

    idx_0 = (size_t)idx;

    if ( org_length < idx_0 + len ) len = org_length - idx_0;
    if ( len <= 0 ) return *this;

    this->move(dim_index, idx, len, 0, false);
    this->resize(dim_index, len);

    return *this;
}

/**
 * @brief  最初の次元について配列の並びを反転
 *
 *  自身の配列の要素位置 idx から len 個の要素を反転させます．<br>
 *  常に次元番号 0 の次元(1次元目)を処理対象とします．
 *
 * @param     idx 開始位置の要素番号
 * @param     len 要素の個数
 * @return    自身の参照
 */
mdarray &mdarray::flip( ssize_t idx, size_t len )		/* for 1-d */
{
    this->flip(0, idx, len);
    return *this;
}

/**
 * @brief  任意の1つの次元について配列の並びを反転
 *
 *  自身の配列の要素位置 idx から len 個の要素を反転させます．<br>
 *  次元番号 dim_index で処理対象とする次元を指定できます．
 *
 * @param     dim_index 次元番号
 * @param     idx 開始位置の要素番号
 * @param     len 要素の個数
 * @return    自身の参照
 */
mdarray &mdarray::flip( size_t dim_index, ssize_t idx, size_t len )
{
    const size_t bytes = this->bytes();
    const size_t len_elem = this->length(dim_index);
    unsigned char *d_ptr = (unsigned char *)(this->data_ptr());

    if ( this->dim_size_rec <= dim_index ) return *this;	/* invalid */

    /* check idx */
    if ( 0 <= idx && len_elem <= abs_sz2z(idx) ) return *this;

    if ( idx < 0 ) {
	if ( abs_sz2z(idx) < len ) {
	    len -= abs_sz2z(idx);
	    idx = 0;
	} else {
	    return *this;
	}
    }

    /* check len */
    if ( len_elem < (size_t)idx + len ) len = len_elem - idx;

    if ( len < 2 ) return *this;

    if ( dim_index == 0 ) {
	size_t blen0 = bytes * this->length(dim_index);
	size_t n_lines = 1;
	size_t i;
	for ( i=1 ; i < this->dim_size_rec ; i++ ) {
	    n_lines *= this->length(i);
	}
	d_ptr += bytes * idx;					/* offset */
	for ( i=0 ; i < n_lines ; i++ ) {
	    s_memflip(d_ptr, bytes, len);
	    d_ptr += blen0;
	}
    }
    else {
	size_t blen0, bstep, n_blk;
	unsigned char *p0;
	unsigned char *p1;
	size_t i, j;
	blen0 = bytes;
	for ( i=0 ; i < dim_index ; i++ ) blen0 *= this->length(i);
	bstep = blen0 * this->length(i);
	n_blk = 1;
	for ( i=dim_index+1 ; i < this->dim_size_rec ; i++ ) {
	    n_blk *= this->length(i);
	}
	d_ptr += blen0 * idx;					/* offset */
	for ( i=0 ; i < n_blk ; i++ ) {
	    p0 = d_ptr;
	    p1 = d_ptr + blen0 * len;
	    for ( j=0 ; j < len/2 ; j++ ) {
		p1 -= blen0;
		s_memswap(p0, p1, blen0);
		p0 += blen0;
	    }
	    d_ptr += bstep;
	}
    }

    return *this;
}

/**
 * @brief  浮動小数点型の全要素値の切り上げ
 *
 *  自身の配列(浮動小数点型)の全要素の小数部の値を切り上げます．
 *
 * @return     自身の参照
 */
mdarray &mdarray::ceil()
{
    const size_t len = this->length();
    size_t i;
    if ( this->sz_type_rec == DOUBLE_ZT ) {
      double *pp = (double *)(this->data_ptr());
      for ( i=0 ; i < len ; i++ ) {
	pp[i] = c_ceil(pp[i]);
      }
    }
    else if ( this->sz_type_rec == FLOAT_ZT ) {
      float *pp = (float *)(this->data_ptr());
      for ( i=0 ; i < len ; i++ ) {
	pp[i] = c_ceil(pp[i]);
      }
    }
    else if ( this->sz_type_rec == LDOUBLE_ZT ) {
      long double *pp = (long double *)(this->data_ptr());
      for ( i=0 ; i < len ; i++ ) {
	pp[i] = c_ceil((double)(pp[i]));
      }
    }
    else if ( this->sz_type_rec == FCOMPLEX_ZT ) {
      fcomplex *pp = (fcomplex *)(this->data_ptr());
      for ( i=0 ; i < len ; i++ ) {
	pp[i] = c_ceil(creal(pp[i])) + c_ceil(cimag(pp[i]))*I;
      }
    }
    else if ( this->sz_type_rec == DCOMPLEX_ZT ) {
      dcomplex *pp = (dcomplex *)(this->data_ptr());
      for ( i=0 ; i < len ; i++ ) {
	pp[i] = c_ceil(creal(pp[i])) + c_ceil(cimag(pp[i]))*I;
      }
    }
    else if ( this->sz_type_rec == LDCOMPLEX_ZT ) {
      ldcomplex *pp = (ldcomplex *)(this->data_ptr());
      for ( i=0 ; i < len ; i++ ) {
	pp[i] = c_ceil((double)(creal(pp[i]))) + c_ceil((double)(cimag(pp[i])))*I;
      }
    }
    return *this;
}

/**
 * @brief  浮動小数点型の全要素値の小数部の切り下げ
 *
 *  自身の配列(浮動小数点型)の全要素の小数部を，それぞれの要素の値を越えない
 *  最大の整数値に切り下げます．
 *
 * @return     自身の参照
 */
mdarray &mdarray::floor()
{
    const size_t len = this->length();
    size_t i;
    if ( this->sz_type_rec == DOUBLE_ZT ) {
      double *pp = (double *)(this->data_ptr());
      for ( i=0 ; i < len ; i++ ) {
	pp[i] = c_floor(pp[i]);
      }
    }
    else if ( this->sz_type_rec == FLOAT_ZT ) {
      float *pp = (float *)(this->data_ptr());
      for ( i=0 ; i < len ; i++ ) {
	pp[i] = c_floor(pp[i]);
      }
    }
    else if ( this->sz_type_rec == LDOUBLE_ZT ) {
      long double *pp = (long double *)(this->data_ptr());
      for ( i=0 ; i < len ; i++ ) {
	pp[i] = c_floor((double)(pp[i]));
      }
    }
    else if ( this->sz_type_rec == FCOMPLEX_ZT ) {
      fcomplex *pp = (fcomplex *)(this->data_ptr());
      for ( i=0 ; i < len ; i++ ) {
	pp[i] = c_floor(creal(pp[i])) + c_floor(cimag(pp[i]))*I;
      }
    }
    else if ( this->sz_type_rec == DCOMPLEX_ZT ) {
      dcomplex *pp = (dcomplex *)(this->data_ptr());
      for ( i=0 ; i < len ; i++ ) {
	pp[i] = c_floor(creal(pp[i])) + c_floor(cimag(pp[i]))*I;
      }
    }
    else if ( this->sz_type_rec == LDCOMPLEX_ZT ) {
      ldcomplex *pp = (ldcomplex *)(this->data_ptr());
      for ( i=0 ; i < len ; i++ ) {
	pp[i] = c_floor((double)(creal(pp[i]))) + c_floor((double)(cimag(pp[i])))*I;
      }
    }
    return *this;
}

/**
 * @brief  浮動小数点型の全要素値の四捨五入
 *
 *  自身の配列(浮動小数点型)の全要素値の小数部を四捨五入し，整数値にします．
 *
 * @return     自身の参照
 */
mdarray &mdarray::round()
{
    const size_t len = this->length();
    size_t i;
    if ( this->sz_type_rec == DOUBLE_ZT ) {
      double *pp = (double *)(this->data_ptr());
      for ( i=0 ; i < len ; i++ ) {
	pp[i] = c_round(pp[i]);
      }
    }
    else if ( this->sz_type_rec == FLOAT_ZT ) {
      float *pp = (float *)(this->data_ptr());
      for ( i=0 ; i < len ; i++ ) {
	pp[i] = c_round(pp[i]);
      }
    }
    else if ( this->sz_type_rec == LDOUBLE_ZT ) {
      long double *pp = (long double *)(this->data_ptr());
      for ( i=0 ; i < len ; i++ ) {
	pp[i] = c_round((double)(pp[i]));
      }
    }
    else if ( this->sz_type_rec == FCOMPLEX_ZT ) {
      fcomplex *pp = (fcomplex *)(this->data_ptr());
      for ( i=0 ; i < len ; i++ ) {
	pp[i] = c_round(creal(pp[i])) + c_round(cimag(pp[i]))*I;
      }
    }
    else if ( this->sz_type_rec == DCOMPLEX_ZT ) {
      dcomplex *pp = (dcomplex *)(this->data_ptr());
      for ( i=0 ; i < len ; i++ ) {
	pp[i] = c_round(creal(pp[i])) + c_round(cimag(pp[i]))*I;
      }
    }
    else if ( this->sz_type_rec == LDCOMPLEX_ZT ) {
      ldcomplex *pp = (ldcomplex *)(this->data_ptr());
      for ( i=0 ; i < len ; i++ ) {
	pp[i] = c_round((double)(creal(pp[i]))) + c_round((double)(cimag(pp[i])))*I;
      }
    }
    return *this;
}

/**
 * @brief  浮動小数点型の全要素値の小数部の切り捨て
 *
 *  自身の配列(浮動小数点型)の全要素の値の小数部を切り捨て，0 に近い方の正数値
 *  にします．
 *
 * @return     自身の参照
 */
mdarray &mdarray::trunc()
{
    const size_t len = this->length();
    size_t i;
    if ( this->sz_type_rec == DOUBLE_ZT ) {
      double *pp = (double *)(this->data_ptr());
      for ( i=0 ; i < len ; i++ ) {
	pp[i] = c_trunc(pp[i]);
      }
    }
    else if ( this->sz_type_rec == FLOAT_ZT ) {
      float *pp = (float *)(this->data_ptr());
      for ( i=0 ; i < len ; i++ ) {
	pp[i] = c_trunc(pp[i]);
      }
    }
    else if ( this->sz_type_rec == LDOUBLE_ZT ) {
      long double *pp = (long double *)(this->data_ptr());
      for ( i=0 ; i < len ; i++ ) {
	pp[i] = c_trunc((double)(pp[i]));
      }
    }
    else if ( this->sz_type_rec == FCOMPLEX_ZT ) {
      fcomplex *pp = (fcomplex *)(this->data_ptr());
      for ( i=0 ; i < len ; i++ ) {
	pp[i] = c_trunc(creal(pp[i])) + c_trunc(cimag(pp[i]))*I;
      }
    }
    else if ( this->sz_type_rec == DCOMPLEX_ZT ) {
      dcomplex *pp = (dcomplex *)(this->data_ptr());
      for ( i=0 ; i < len ; i++ ) {
	pp[i] = c_trunc(creal(pp[i])) + c_trunc(cimag(pp[i]))*I;
      }
    }
    else if ( this->sz_type_rec == LDCOMPLEX_ZT ) {
      ldcomplex *pp = (ldcomplex *)(this->data_ptr());
      for ( i=0 ; i < len ; i++ ) {
	pp[i] = c_trunc((double)(creal(pp[i]))) + c_trunc((double)(cimag(pp[i])))*I;
      }
    }
    return *this;
}

/**
 * @brief  全要素値に対し絶対値をとる
 *
 *  自身の配列の全要素について絶対値をとります．<br>
 *  複素数の場合は，実数部／虚数部のそれぞれについて絶対値をとります．
 *
 * @note ユーザが数学的な複素数の絶対値を想像するかもしれないので複素数の abs()
 *       は対応せず，例外の方が良いかもしれない
 *
 * @return     自身の参照
 */
mdarray &mdarray::abs()
{
    const size_t len = this->length();
    size_t i;
    if ( this->sz_type_rec == DOUBLE_ZT ) {
	double *pp = (double *)(this->data_ptr());
	double v;
	for ( i=0 ; i < len ; i++ ) {
	    v = pp[i];
	    if ( v < 0 ) pp[i] = -v;
	}
    }
    else if ( this->sz_type_rec == FLOAT_ZT ) {
	float *pp = (float *)(this->data_ptr());
	float v;
	for ( i=0 ; i < len ; i++ ) {
	    v = pp[i];
	    if ( v < 0 ) pp[i] = -v;
	}
    }
    else if ( this->sz_type_rec == LDOUBLE_ZT ) {
	long double *pp = (long double *)(this->data_ptr());
	long double v;
	for ( i=0 ; i < len ; i++ ) {
	    v = pp[i];
	    if ( v < 0 ) pp[i] = -v;
	}
    }
    else if ( this->sz_type_rec == INT32_ZT ) {
	int32_t *pp = (int32_t *)(this->data_ptr());
	int32_t v;
	for ( i=0 ; i < len ; i++ ) {
	    v = pp[i];
	    if ( v < 0 ) pp[i] = -v;
	}
    }
    else if ( this->sz_type_rec == INT16_ZT ) {
	int16_t *pp = (int16_t *)(this->data_ptr());
	int16_t v;
	for ( i=0 ; i < len ; i++ ) {
	    v = pp[i];
	    if ( v < 0 ) pp[i] = -v;
	}
    }
    else if ( this->sz_type_rec == INT64_ZT ) {
	int64_t *pp = (int64_t *)(this->data_ptr());
	int64_t v;
	for ( i=0 ; i < len ; i++ ) {
	    v = pp[i];
	    if ( v < 0 ) pp[i] = -v;
	}
    }
    /* ユーザが数学的な複素数の絶対値を想像するかもしれないので */
    /* 複素数の abs() は対応せず，例外の方が良いかもしれない    */
    else if ( this->sz_type_rec == FCOMPLEX_ZT ) {
	fcomplex *pp = (fcomplex *)(this->data_ptr());
	float v_real, v_imag;
	for ( i=0 ; i < len ; i++ ) {
	    v_real = creal(pp[i]);
	    if( v_real < 0 ) v_real = -v_real;
	    v_imag = cimag(pp[i]);
	    if( v_imag < 0 ) v_imag = -v_imag;
	    pp[i] = v_real + v_imag*I;
	}
    }
    else if ( this->sz_type_rec == DCOMPLEX_ZT ) {
	dcomplex *pp = (dcomplex *)(this->data_ptr());
	double v_real, v_imag;
	for ( i=0 ; i < len ; i++ ) {
	    v_real = creal(pp[i]);
	    if( v_real < 0 ) v_real = -v_real;
	    v_imag = cimag(pp[i]);
	    if( v_imag < 0 ) v_imag = -v_imag;
	    pp[i] = v_real + v_imag*I;
	}
    }
    else if ( this->sz_type_rec == LDCOMPLEX_ZT ) {
	ldcomplex *pp = (ldcomplex *)(this->data_ptr());
	long double v_real, v_imag;
	for ( i=0 ; i < len ; i++ ) {
	    v_real = creal(pp[i]);
	    if( v_real < 0 ) v_real = -v_real;
	    v_imag = cimag(pp[i]);
	    if( v_imag < 0 ) v_imag = -v_imag;
	    pp[i] = v_real + v_imag*I;
	}
    }
    return *this;
}


/* ------------------------------------------------------------------------- */


typedef struct _mdarray_section_expinfo {
    ssize_t begin;		/* start position        */
    size_t length;		/* length or MDARRAY_ALL */
    bool flip_flag;		/* flag for flip image   */
} mdarray_section_expinfo;

/**
 * @brief  "0:99,*" "[1:100,*]" のような文字列からセクション情報を取得 (内部用)
 *
 * @note  cleanf(), copyf() 等で使用．<br>
 *        private な関数です．
 */
static int parse_section_exp( 
		   heap_mem<mdarray_section_expinfo> *rt_secinfo, size_t *rt_n_secinfo,
		   const char *secexp )
{
    int ret_status = -1;
    heap_mem<mdarray_section_expinfo> &secinfo = *rt_secinfo;
    bool zero_indexed = true;
    size_t ix, n_el;

    if ( secexp == NULL ) {
	*rt_n_secinfo = 0;
	ret_status = 0;
	goto quit;
    }

    /* count elements */
    ix = 0;  n_el = 0;
    while ( 0 < secexp[ix] && secexp[ix] <= ' ' ) ix ++;
    if ( secexp[ix] == '(' || secexp[ix] == '[' || secexp[ix] == '{' ) ix ++;
    while ( 1 ) {
	while ( 0 < secexp[ix] && secexp[ix] <= ' ' ) ix ++;
	while ( secexp[ix] != ',' && secexp[ix] != '\0' ) ix ++;
	n_el ++;
	if ( secexp[ix] == ',' ) ix ++;
	else break;
    };
    *rt_n_secinfo = n_el;

    if ( secinfo.allocate(*rt_n_secinfo) < 0 ) {
	err_report(__FUNCTION__,"FATAL","malloc() failed");
	goto quit;
    }

    /* get elements */
    ix = 0;  n_el = 0;
    while ( 0 < secexp[ix] && secexp[ix] <= ' ' ) ix ++;
    if ( secexp[ix] == '[' ) {
	zero_indexed = false;
	ix ++;
    }
    else if ( secexp[ix] == '(' || secexp[ix] == '{' ) {
	ix ++;
    }
    while ( 1 ) {
	long v0=0, v1=0;
	bool v0_is_set=false, v1_is_set=false;
	bool flip_all=false;
	bool colon_found;
	/* parse 1st param */
	while ( 0 < secexp[ix] && secexp[ix] <= ' ' ) ix ++;
	if ( secexp[ix] == '-' && secexp[ix+1] == '*' ) {
	    flip_all = true;
	    ix += 2;
	}
	else if ( ('0' <= secexp[ix] && secexp[ix] <= '9') || 
	     secexp[ix] == '+' || secexp[ix] == '-' ) {
	    v0 = c_strtol(secexp + ix, (char **)NULL, 10);
	    v0_is_set = true;
	}
	while (secexp[ix] != ':' && secexp[ix] != ',' && secexp[ix] != '\0') {
	    ix ++;
	}
	if ( secexp[ix] == ':' ) {
	    colon_found = true;
	    ix ++;
	    /* parse 2nd param */
	    while ( 0 < secexp[ix] && secexp[ix] <= ' ' ) ix ++;
	    if ( secexp[ix] == '-' && secexp[ix+1] == '*' ) {
		flip_all = true;
		ix += 2;
	    }
	    else if ( ('0' <= secexp[ix] && secexp[ix] <= '9') || 
		 secexp[ix] == '+' || secexp[ix] == '-' ) {
		v1 = c_strtol(secexp + ix, (char **)NULL, 10);
		v1_is_set = true;
	    }
	    while ( secexp[ix] != ',' && secexp[ix] != '\0' ) ix ++;
	}
	else {
	    colon_found = false;
	}
	/* setup section info */
	if ( v1_is_set == true ) {
	    if ( v0_is_set == false ) {
		if ( zero_indexed == true ) v0 = 0;
		else v0 = 1;
	    }
	    if ( v1 < v0 ) {
		secinfo[n_el].begin = v1;
		secinfo[n_el].length = 1 + v0 - v1;
		secinfo[n_el].flip_flag = true;
	    }
	    else {
		secinfo[n_el].begin = v0;
		secinfo[n_el].length = 1 + v1 - v0;
		secinfo[n_el].flip_flag = false;
	    }
	    if ( zero_indexed == false ) secinfo[n_el].begin --;
	}
	else if ( v0_is_set == true ) {		/* only v0 is set */
	    if ( colon_found == true ) {
		secinfo[n_el].begin = v0;
		secinfo[n_el].length = MDARRAY_ALL;
		secinfo[n_el].flip_flag = false;
	    }
	    else {
		secinfo[n_el].begin = v0;
		secinfo[n_el].length = 1;
		secinfo[n_el].flip_flag = false;
	    }
	    if ( zero_indexed == false ) secinfo[n_el].begin --;
	}
	else {					/* fallback */
	    secinfo[n_el].begin = 0;
	    secinfo[n_el].length = MDARRAY_ALL;
	    secinfo[n_el].flip_flag = false;
	}
	/* set flag for "-*" expression */
	if ( flip_all == true ) secinfo[n_el].flip_flag = true;
	/* next ... */
	n_el ++;
	if ( secexp[ix] == ',' ) ix ++;
	else break;
    };

    ret_status = 0;
 quit:
    return ret_status;
}

/**
 * @brief  sectionf() 等の引数を調べて適切な座標のパラメータを設定 (低レベル)
 *
 * secinfo をチェックし，セクションのはみ出し等があれば修正する．<br>
 * オブジェクトの次元数に比べて secinfo のそれが足りない場合は追加する．
 *
 * @note    private な関数です．
 */
static int test_section_info( const char *secexp, const mdarray &obj, 
			      heap_mem<mdarray_section_expinfo> *rt_secinfo,
			      size_t *rt_n_secinfo,
			      heap_mem<size_t> *rt_naxisx )
{
    int ret_status = -1;
    heap_mem<mdarray_section_expinfo> &secinfo = *rt_secinfo;
    size_t n_test = *rt_n_secinfo;
    size_t i;

    if ( n_test < obj.dim_length() ) {
	if ( secinfo.reallocate(obj.dim_length()) < 0 ) {
	    err_report(__FUNCTION__,"FATAL","realloc() failed");
	    goto quit;
	}
	for ( i = n_test ; i < obj.dim_length() ; i ++ ) {
	    secinfo[i].begin = 0;
	    secinfo[i].length = obj.length(i);
	    secinfo[i].flip_flag = false;
	}
	*rt_n_secinfo = obj.dim_length();
    }
    else if ( obj.dim_length() < n_test ) {
	*rt_n_secinfo = obj.dim_length();
	n_test = obj.dim_length();
    }

    for ( i=0 ; i < n_test ; i++ ) {
	bool fixed_vals = false;
	if ( secinfo[i].length == MDARRAY_ALL ) {
	    if ( secinfo[i].begin < 0 ) {
		secinfo[i].begin = 0;
		fixed_vals = true;
	    }
	    secinfo[i].length = obj.length(i) - secinfo[i].begin;
	}
	/* 指定領域がはみ出している場合の処置 */
	if ( secinfo[i].begin < 0 ) {
	    if ( secinfo[i].begin + secinfo[i].length <= 0 ) {
		/* 読むべき長さはゼロ */
		secinfo[i].length = 0;
	    }
	    else {
		/* 読むべき長さを小さくする */
		secinfo[i].length += secinfo[i].begin;
	    }
	    secinfo[i].begin = 0;
	    fixed_vals = true;
	}
	else if ( obj.length(i) <= (size_t)(secinfo[i].begin) ) {
	    secinfo[i].begin = 0;
	    secinfo[i].length = 0;
	}
	if ( obj.length(i) < secinfo[i].begin + secinfo[i].length ) {
	    secinfo[i].length = obj.length(i) - secinfo[i].begin;
	    fixed_vals = true;
	}
	/* もし 0 なら 警告を出して return */
	if ( secinfo[i].length == 0 ) {
	    char rng_str[256];
	    c_snprintf(rng_str,256,
		       "expression \"%s\" indicates out of range; "
		       "no data will be selected", secexp);
	    *rt_n_secinfo = 0;
	    *rt_secinfo = NULL;
	    if ( rt_naxisx != NULL ) *rt_naxisx = NULL;
	    err_report1(__FUNCTION__,"WARNING","%s",rng_str);
	    break;
	}
	/* 範囲が修正された場合 */
	if ( fixed_vals == true ) {
	    long v0, v1;
	    char rng_str[256];
	    v0 = (long)(secinfo[i].begin);
	    v1 = (long)(secinfo[i].begin + secinfo[i].length - 1);
	    if ( secinfo[i].flip_flag == true ) {
		long tmp = v0;
		v0 = v1;
		v1 = tmp;
	    }
	    c_snprintf(rng_str,256,
		       "expression \"%s\": "
		       "fixed range to \"%ld:%ld\" (dim=%ld)",
		       secexp, v0, v1, (long)i);
	    err_report1(__FUNCTION__,"NOTICE","%s",rng_str);
	}
    }

    if ( rt_naxisx != NULL ) {
	heap_mem<size_t> &naxisx = *rt_naxisx;
	if ( naxisx.allocate(*rt_n_secinfo) < 0 ) {
	    err_report(__FUNCTION__,"FATAL","malloc() failed");
	    goto quit;
	}
	for ( i=0 ; i < *rt_n_secinfo ; i++ ) {
	    naxisx[i] = secinfo[i].length;
	}
    }

    ret_status = 0;
 quit:
    return ret_status;
}


/* ----------------------------------------------------------------------------

static void fill_func( double orig_pix_vals[], double src_pix_val, 
		       size_t n,
		       ssize_t col_idx, ssize_t row_idx, ssize_t layer_idx, 
		       mdarray *objp, void *any )
{
    size_t i;
    for ( i=0 ; i < n ; i++ ) orig_pix_vals[i] = src_pix_val;
    return;
}

static void fill_func_n( double orig_pix_vals[], double src_pix_vals[], 
			 size_t n,
			 ssize_t col_idx, ssize_t row_idx, ssize_t layer_idx, 
			 mdarray *objp, void *any )
{
    size_t i;
    for ( i=0 ; i < n ; i++ ) orig_pix_vals[i] = src_pix_vals[i];
    return;
}

 --------------------------------------------------------------------------- */

/**
 * @brief  画像演算用の関数 (現在はほぼ未使用)
 *
 * @note マクロ展開による高速コードが動くので，現在はほとんど使われる事はない．
 */
static void add_func( double orig_pix_vals[], double src_pix_val, 
		      size_t n,
		      ssize_t col_idx, ssize_t row_idx, ssize_t layer_idx, 
		      mdarray *objp, void *any )
{
    size_t i;
    for ( i=0 ; i < n ; i++ ) orig_pix_vals[i] += src_pix_val;
    return;
}

/**
 * @brief  画像演算用の関数 (現在はほぼ未使用)
 *
 * @note マクロ展開による高速コードが動くので，現在はほとんど使われる事はない．
 */
static void subtract_func( double orig_pix_vals[], double src_pix_val, 
			   size_t n,
			   ssize_t col_idx, ssize_t row_idx, ssize_t layer_idx,
			   mdarray *objp, void *any )
{
    size_t i;
    for ( i=0 ; i < n ; i++ ) orig_pix_vals[i] -= src_pix_val;
    return;
}

/**
 * @brief  画像演算用の関数 (現在はほぼ未使用)
 *
 * @note マクロ展開による高速コードが動くので，現在はほとんど使われる事はない．
 */
static void multiply_func( double orig_pix_vals[], double src_pix_val, 
			   size_t n,
			   ssize_t col_idx, ssize_t row_idx, ssize_t layer_idx,
			   mdarray *objp, void *any )
{
    size_t i;
    for ( i=0 ; i < n ; i++ ) orig_pix_vals[i] *= src_pix_val;
    return;
}

/**
 * @brief  画像演算用の関数 (現在はほぼ未使用)
 *
 * @note マクロ展開による高速コードが動くので，現在はほとんど使われる事はない．
 */
static void divide_func( double orig_pix_vals[], double src_pix_val, 
			 size_t n,
			 ssize_t col_idx, ssize_t row_idx, ssize_t layer_idx,
			 mdarray *objp, void *any )
{
    size_t i;
    for ( i=0 ; i < n ; i++ ) orig_pix_vals[i] /= src_pix_val;
    return;
}

/**
 * @brief  画像演算用の関数 (現在はほぼ未使用)
 *
 * @note マクロ展開による高速コードが動くので，現在はほとんど使われる事はない．
 */
static void add_func_n( double orig_pix_vals[], double src_pix_vals[], 
			size_t n,
			ssize_t col_idx, ssize_t row_idx, ssize_t layer_idx, 
			mdarray *objp, void *any )
{
    size_t i;
    for ( i=0 ; i < n ; i++ ) orig_pix_vals[i] += src_pix_vals[i];
    return;
}

/**
 * @brief  画像演算用の関数 (現在はほぼ未使用)
 *
 * @note マクロ展開による高速コードが動くので，現在はほとんど使われる事はない．
 */
static void subtract_func_n( double orig_pix_vals[], double src_pix_vals[], 
			     size_t n,
			     ssize_t col_idx, ssize_t row_idx, ssize_t layer_idx,
			     mdarray *objp, void *any )
{
    size_t i;
    for ( i=0 ; i < n ; i++ ) orig_pix_vals[i] -= src_pix_vals[i];
    return;
}

/**
 * @brief  画像演算用の関数 (現在はほぼ未使用)
 *
 * @note マクロ展開による高速コードが動くので，現在はほとんど使われる事はない．
 */
static void multiply_func_n( double orig_pix_vals[], double src_pix_vals[], 
			     size_t n,
			     ssize_t col_idx, ssize_t row_idx, ssize_t layer_idx,
			     mdarray *objp, void *any )
{
    size_t i;
    for ( i=0 ; i < n ; i++ ) orig_pix_vals[i] *= src_pix_vals[i];
    return;
}

/**
 * @brief  画像演算用の関数 (現在はほぼ未使用)
 *
 * @note マクロ展開による高速コードが動くので，現在はほとんど使われる事はない．
 */
static void divide_func_n( double orig_pix_vals[], double src_pix_vals[], 
			   size_t n,
			   ssize_t col_idx, ssize_t row_idx, ssize_t layer_idx,
			   mdarray *objp, void *any )
{
    size_t i;
    for ( i=0 ; i < n ; i++ ) orig_pix_vals[i] /= src_pix_vals[i];
    return;
}

/* */

struct mdarray_calc_arr_nd_prms {
    /* アライメントに注意(sizeofが長いものから順に!) */
    int ope;
    bool round_flg;
};

/* 画像と画像の演算専用                                        */
/* 前からか後ろからか，方向が決まっている場合．                */
/* f_b = -1 で後ろから，f_b = 1 で前から，f_b = 0 でスカラー値 */
namespace calc_arr_nd
{
#define PM05_FOR_ROUND(dv) (((dv) < 0) ? (dv)-0.5 : (dv)+0.5)
#define MAKE_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
static void fncname(const void *org_val_ptr, void *new_val_ptr, size_t n, int f_b, void *u_ptr) \
{ \
    size_t i; \
    struct mdarray_calc_arr_nd_prms *prms = (struct mdarray_calc_arr_nd_prms *)u_ptr; \
    const bool pm05_flg = ((new_sz_type < 0) ? false : prms->round_flg); \
    const int ope = prms->ope; \
    const org_type *src = (const org_type *)org_val_ptr; \
    new_type *dst = (new_type *)new_val_ptr; \
    double v0; \
    if ( f_b < 0 ) { \
	if ( ope == Ope_plus ) { \
	    for ( i=n ; 0 < i ; ) { \
		i--; \
		v0 = (double)(dst[i]) + (double)(src[i]); \
		dst[i] = (new_type)(pm05_flg ? PM05_FOR_ROUND(v0) : v0); \
	    } \
	} \
	else if ( ope == Ope_minus ) { \
	    for ( i=n ; 0 < i ; ) { \
		i--; \
		v0 = (double)(dst[i]) - (double)(src[i]); \
		dst[i] = (new_type)(pm05_flg ? PM05_FOR_ROUND(v0) : v0); \
	    } \
	} \
	else if ( ope == Ope_star ) { \
	    for ( i=n ; 0 < i ; ) { \
		i--; \
		v0 = (double)(dst[i]) * (double)(src[i]); \
		dst[i] = (new_type)(pm05_flg ? PM05_FOR_ROUND(v0) : v0); \
	    } \
	} \
	else { \
	    for ( i=n ; 0 < i ; ) { \
		i--; \
		v0 = (double)(dst[i]) / (double)(src[i]); \
		dst[i] = (new_type)(pm05_flg ? PM05_FOR_ROUND(v0) : v0); \
	    } \
	} \
    } \
    else { \
	const size_t s_step = ((0 < f_b) ? 1 : 0); \
	if ( ope == Ope_plus ) { \
	    for ( i=0 ; i < n ; i++ ) { \
		v0 = (double)(dst[i]) + (double)(*src); \
		dst[i] = (new_type)(pm05_flg ? PM05_FOR_ROUND(v0) : v0); \
		src += s_step; \
	    } \
	} \
	else if ( ope == Ope_minus ) { \
	    for ( i=0 ; i < n ; i++ ) { \
		v0 = (double)(dst[i]) - (double)(*src); \
		dst[i] = (new_type)(pm05_flg ? PM05_FOR_ROUND(v0) : v0); \
		src += s_step; \
	    } \
	} \
	else if ( ope == Ope_star ) { \
	    for ( i=0 ; i < n ; i++ ) { \
		v0 = (double)(dst[i]) * (double)(*src); \
		dst[i] = (new_type)(pm05_flg ? PM05_FOR_ROUND(v0) : v0); \
		src += s_step; \
	    } \
	} \
	else { \
	    for ( i=0 ; i < n ; i++ ) { \
		v0 = (double)(dst[i]) / (double)(*src); \
		dst[i] = (new_type)(pm05_flg ? PM05_FOR_ROUND(v0) : v0); \
		src += s_step; \
	    } \
	} \
    } \
}
SLI__MDARRAY__DO_OPERATION_2TYPES(MAKE_FUNC,,,,,,,,,,,,,);
#undef MAKE_FUNC
#undef PM05_FOR_ROUND
}

/* */

/* 
 * member functions for image processing using IDL style argument 
 * such as "0:99,*".  The expression is set to exp_fmt argument in 
 * member functions.
 *
 * Number of dimension in the expression is unlimited.
 * Note that integer numbers in the string is 0-indexed. 
 *
 * [example]
 *   array1 = array0.sectionf("0:99,*");
 *   array1 = array0.sectionf("%ld:%ld,*", begin, end);
 *
 * Flipping elements in dimensions is supported in sectionf(), copyf(),
 * trimf(), and flipf().  Here is an example to perform flipping elements
 * in first 2 dimensions:
 *   array1 = array0.sectionf("99:0,-*");
 * 
 */

/* returns trimmed array using IDL style argument such as */
/*   array1 = array0.sectionf("0:99,100:199,*");          */
/**
 * @brief  一部要素を切り出した配列をテンポラリオブジェクトで取得 (IDLの記法)
 *
 * 画像データ向きのメンバ関数で，自身の配列のうち指定された部分だけをテンポラリ
 * オブジェクトにコピーし，それを返します．<br>
 * 取り出されるされる範囲は任意の次元まで指定可能で "0:99,*" のように IDL や
 * IRAF と同様の文字列で指定します．この文字列中の要素番号は 0-indexed です．
 *
 * @param   exp_fmt  csv形式の範囲指定文字列のためのフォーマット指定
 * @param   ...      exp_fmtに対応した可変長引数の各要素データ
 * @return  テンポラリオブジェクト
 * @note    "[1:100,*]" のように [] で囲むと，1-indexed として扱われます．
 * @throw   バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 */
mdarray mdarray::sectionf( const char *exp_fmt, ... ) const
{
    debug_report("sectionf( const char *exp_fmt, ... ) const");
    mdarray ret;

    va_list ap;
    va_start(ap, exp_fmt);
    try {
	this->vcopyf(&ret, exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vcopyf() failed");
    }
    va_end(ap);

    ret.set_scopy_flag();
    return ret;
}

/**
 * @brief  一部要素を切り出した配列をテンポラリオブジェクトで取得 (IDLの記法)
 *
 * 画像データ向きのメンバ関数で，自身の配列のうち指定された部分だけをテンポラリ
 * オブジェクトにコピーし，それを返します．<br>
 * 取り出されるされる範囲は任意の次元まで指定可能で "0:99,*" のように IDL や
 * IRAF と同様の文字列で指定します．この文字列中の要素番号は 0-indexed です．
 *
 * @param   exp_fmt  csv形式の範囲指定文字列のためのフォーマット指定
 * @param   ap       exp_fmtに対応した可変長引数のリスト
 * @return  テンポラリオブジェクト
 * @note    "[1:100,*]" のように [] で囲むと，1-indexed として扱われます．
 * @throw   バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 */
mdarray mdarray::vsectionf( const char *exp_fmt, va_list ap ) const
{
    mdarray ret;

    this->vcopyf(&ret, exp_fmt, ap);

    ret.set_scopy_flag();
    return ret;
}

/**
 * @brief  一部要素を別オブジェクトへコピー (IDLの記法)
 *
 *  画像データ向きのメンバ関数で，自身の配列の内容の一部分を指定されたオブジェ
 *  クト dest_obj へコピーします．<br>
 *  コピーされる範囲は任意の次元まで指定可能で "0:99,*" のように IDL や IRAF と
 *  同様の文字列で指定します．この文字列中の要素番号は 0-indexed です．<br>
 *  コピー先へは，配列の要素値とコピー元の型，デフォルト値，丸めフラグの属性な
 *  どすべてをコピーします．自身(コピー元) の配列は改変されません．<br>
 *  dest_obj に自身を与えた場合，trimf()を実行した場合と同じ結果になります．
 *
 * @param   dest_obj コピー先のオブジェクト
 * @param   exp_fmt  csv形式の範囲指定文字列のためのフォーマット指定
 * @param   ...      exp_fmtに対応した可変長引数の各要素データ
 * @return  コピーした要素数(列数 × 行数 × レイヤ数 × ...)
 * @note    "[1:100,*]" のように [] で囲むと，1-indexed として扱われます．
 * @throw   バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 */
ssize_t mdarray::copyf( mdarray *dest_obj, const char *exp_fmt, ... ) const
{
    ssize_t ret;
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	ret = this->vcopyf(dest_obj, exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vcopyf() failed");
    }
    va_end(ap);
    return ret;
}

/**
 * @brief  一部要素を別オブジェクトへコピー (IDLの記法)
 *
 *  画像データ向きのメンバ関数で，自身の配列の内容の一部分を指定されたオブジェ
 *  クト dest_obj へコピーします．<br>
 *  コピーされる範囲は任意の次元まで指定可能で "0:99,*" のように IDL や IRAF と
 *  同様の文字列で指定します．この文字列中の要素番号は 0-indexed です．<br>
 *  コピー先へは，配列の要素値とコピー元の型，デフォルト値，丸めフラグの属性な
 *  どすべてをコピーします．自身(コピー元) の配列は改変されません．<br>
 *  dest_obj に自身を与えた場合，trimf()を実行した場合と同じ結果になります．
 *
 * @param   dest_obj コピー先のオブジェクト
 * @param   exp_fmt  csv形式の範囲指定文字列のためのフォーマット指定
 * @param   ap       exp_fmtに対応した可変長引数のリスト
 * @return  コピーした要素数(列数 × 行数 × レイヤ数 × ...)
 * @note    "[1:100,*]" のように [] で囲むと，1-indexed として扱われます．
 * @throw   バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 */
ssize_t mdarray::vcopyf( mdarray *dest_obj, const char *exp_fmt, va_list ap ) const
{
    return this->image_vcopyf(dest_obj, exp_fmt, ap);
}

/* copy an convet all or a section to another mdarray object */
/* Flipping elements is supported                            */
/* [not implemented] */
/**
 * @brief  自身の一部要素を型変換した配列を返す (IDLの記法)
 * @deprecated  未実装
 */
ssize_t mdarray::convertf_copy( mdarray *dest_obj, const char *exp_fmt, ... ) const
{
    ssize_t ret;
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	ret = this->vconvertf_copy(dest_obj, exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vconvertf_copy() failed");
    }
    va_end(ap);
    return ret;
}

/**
 * @brief  自身の一部要素を型変換した配列を返す (IDLの記法)
 * @deprecated  未実装
 */
ssize_t mdarray::vconvertf_copy( mdarray *dest_obj, const char *exp_fmt, va_list ap ) const
{
    return -1;
}

/* for SFITSIO */
/* [not implemented] */
/**
 * @brief  自身の一部要素を型変換した配列を返す (IDLの記法)
 * @deprecated  未実装
 */
ssize_t mdarray::convertf_copy_via_udf( mdarray *dest_obj, 
	  void (*func)(const void *,void *,size_t,int,void *), void *user_ptr,
	  const char *exp_fmt, ... ) const
{
    ssize_t ret;
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	ret = this->vconvertf_copy_via_udf(dest_obj, func, user_ptr, 
					   exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vconvertf_copy_via_udf() failed");
    }
    va_end(ap);
    return ret;
}

/**
 * @brief  自身の一部要素を型変換した配列を返す (IDLの記法)
 * @deprecated  未実装
 */
ssize_t mdarray::vconvertf_copy_via_udf( mdarray *dest_obj, 
	  void (*func)(const void *,void *,size_t,int,void *), void *user_ptr,
	  const char *exp_fmt, va_list ap ) const
{
    return -1;
}

/**
 * @brief  指定部分以外の配列要素の消去 (画像データのトリミング・IDLの記法)
 *
 *  画像データ向きのメンバ関数で，自身の配列のうち指定された部分だけを残し，
 *  指定外の部分を消去します．<br>
 *  切り出されるされる範囲は任意の次元まで指定可能で "0:99,*" のように IDL や
 *  IRAF と同様の文字列で指定します．この文字列中の要素番号は 0-indexed です．
 *
 * @param   exp_fmt  csv形式の範囲指定文字列のためのフォーマット指定
 * @param   ...      exp_fmtに対応した可変長引数の各要素データ
 * @return  自身の参照
 * @note    "[1:100,*]" のように [] で囲むと，1-indexed として扱われます．
 * @throw   バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 */
mdarray &mdarray::trimf( const char *exp_fmt, ... )
{
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	this->vtrimf(exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vtrimf() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  指定部分以外の配列要素の消去 (画像データのトリミング・IDLの記法)
 *
 *  画像データ向きのメンバ関数で，自身の配列のうち指定された部分だけを残し，
 *  指定外の部分を消去します．<br>
 *  切り出されるされる範囲は任意の次元まで指定可能で "0:99,*" のように IDL や
 *  IRAF と同様の文字列で指定します．この文字列中の要素番号は 0-indexed です．
 *
 * @param   exp_fmt  csv形式の範囲指定文字列のためのフォーマット指定
 * @param   ap       exp_fmtに対応した可変長引数のリスト
 * @return  自身の参照
 * @note    "[1:100,*]" のように [] で囲むと，1-indexed として扱われます．
 * @throw   バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 */
mdarray &mdarray::vtrimf( const char *exp_fmt, va_list ap )
{
    this->vcopyf(this, exp_fmt, ap);
    return *this;
}

/* flip elements in a section */
/**
 * @brief  任意の次元(複数も可)で配列の並びを反転 (IDLの記法)
 */
mdarray &mdarray::flipf( const char *exp_fmt, ... )
{
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	this->vflipf(exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vflipf() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  任意の次元(複数も可)で配列の並びを反転 (IDLの記法)
 */
mdarray &mdarray::vflipf( const char *exp_fmt, va_list ap )
{
    this->image_vflipf(exp_fmt, ap);
    return *this;
}

/**
 * @brief  (x,y)のトランスポーズを行なった配列を取得 (IDLの記法)
 */
ssize_t mdarray::transposef_xy_copy( mdarray *dest, 
				     const char *exp_fmt, ... ) const
{
    ssize_t ret;
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	ret = this->vtransposef_xy_copy(dest, exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vtransposef_xy_copy() failed");
    }
    va_end(ap);
    return ret;
}

/**
 * @brief  (x,y)のトランスポーズを行なった配列を取得 (IDLの記法)
 */
ssize_t mdarray::vtransposef_xy_copy( mdarray *dest, 
				      const char *exp_fmt, va_list ap ) const
{
    return this->image_vtransposef_xy_copy(dest, exp_fmt, ap);
}

/**
 * @brief  (x,y,z)→(z,x,y)のトランスポーズを行なった配列を取得 (IDLの記法)
 */
ssize_t mdarray::transposef_xyz2zxy_copy( mdarray *dest, 
					  const char *exp_fmt, ... ) const
{
    ssize_t ret;
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	ret = this->vtransposef_xyz2zxy_copy(dest, exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vtransposef_xyz2zxy_copy() failed");
    }
    va_end(ap);
    return ret;
}

/**
 * @brief  (x,y,z)→(z,x,y)のトランスポーズを行なった配列を取得 (IDLの記法)
 */
ssize_t mdarray::vtransposef_xyz2zxy_copy( mdarray *dest, 
				      const char *exp_fmt, va_list ap ) const
{
    return this->image_vtransposef_xyz2zxy_copy(dest, exp_fmt, ap);
}

/* padding existing values in an array */
/**
 * @brief  自身の配列をデフォルト値でパディング (IDLの記法)
 */
mdarray &mdarray::cleanf( const char *exp_fmt, ... )
{
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	this->vcleanf(exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vcleanf() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  自身の配列をデフォルト値でパディング (IDLの記法)
 */
mdarray &mdarray::vcleanf( const char *exp_fmt, va_list ap )
{
    if ( this->default_value_ptr() != NULL ) {
	this->image_vcleanf( this->default_value_ptr(), exp_fmt, ap );
    }
    else {
	this->image_vcleanf( NULL, exp_fmt, ap );
    }
    return *this;
}

/* rewrite element values with a value in a section */
/**
 * @brief  自身の配列を指定されたスカラー値で書き換え (IDLの記法)
 */
mdarray &mdarray::fillf( double value, const char *exp_fmt, ... )
{
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	this->vfillf(value, exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vfillf() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  自身の配列を指定されたスカラー値で書き換え (IDLの記法)
 */
mdarray &mdarray::vfillf( double value, const char *exp_fmt, va_list ap )
{
    void (*func_d2dest)(const void *,void *,size_t,int,void *);
    struct mdarray_cnv_nd_prms prms;

    prms.total_len_elements = 0;			/* 効かない */
    prms.round_flg = this->rounding();			/* 四捨五入/切り捨て */

    /* 型変換のための関数 (double→自身の型) */
    func_d2dest = this->func_cnv_nd_d2x;

    if ( func_d2dest == NULL ) {
	err_report(__FUNCTION__,"WARNING","invalid sz_type; ignored");
	return *this;
    }

    (*func_d2dest)(&value, this->junk_rec, 1, +1, (void *)&prms);

    return this->image_vcleanf( this->junk_rec, exp_fmt, ap );
}

/**
 * @brief  自身の配列をスカラー値で書き換え (ユーザ関数経由・IDLの記法)
 */
mdarray &mdarray::fillf_via_udf( double value,
	  void (*func)(const void *,void *,size_t,int,void *), void *user_ptr,
						     const char *exp_fmt, ... )
{
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	this->vfillf_via_udf(value, func, user_ptr, exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vfillf_via_udf() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  自身の配列をスカラー値で書き換え (ユーザ関数経由・IDLの記法)
 */
mdarray &mdarray::vfillf_via_udf( double value,
	  void (*func)(const void *,void *,size_t,int,void *), void *user_ptr,
					      const char *exp_fmt, va_list ap )
{
    if ( func == NULL ) {
	return this->vfillf( value, exp_fmt, ap );
    }
    else {
	return this->image_vfillf( value, func, user_ptr, exp_fmt, ap );
    }
}

/**
 * @brief  自身の配列をスカラー値で書き換え (ユーザ関数経由・IDLの記法)
 */
mdarray &mdarray::fillf_via_udf( double value, 
	void (*func)(double [],double,size_t, ssize_t,ssize_t,ssize_t,mdarray *,void *),
	void *user_ptr, const char *exp_fmt, ... )
{
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	this->vfillf_via_udf(value, func, user_ptr, exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vfillf_via_udf() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  自身の配列をスカラー値で書き換え (ユーザ関数経由・IDLの記法)
 */
mdarray &mdarray::vfillf_via_udf( double value, 
	void (*func)(double [],double,size_t, ssize_t,ssize_t,ssize_t,mdarray *,void *),
	void *user_ptr, const char *exp_fmt, va_list ap )
{
    if ( func == NULL ) {
	return this->vfillf( value, exp_fmt, ap );
    }
    else {
	return this->image_vfillf( value, 
				   NULL, (void *)NULL, NULL, (void *)NULL,
				   func, user_ptr, exp_fmt, ap );
    }
}

/**
 * @brief  自身の配列をスカラー値で書き換え (ユーザ関数経由・IDLの記法)
 */
mdarray &mdarray::fillf_via_udf( double value, 
	void (*func_dest2d)(const void *,void *,size_t,int,void *), void *user_ptr_dest2d,
	void (*func_d2dest)(const void *,void *,size_t,int,void *), void *user_ptr_d2dest,
	void (*func)(double [],double,size_t, ssize_t,ssize_t,ssize_t,mdarray *,void *),
	void *user_ptr_func,
	const char *exp_fmt, ... )
{
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	this->vfillf_via_udf(value, func_dest2d, user_ptr_dest2d,
			     func_d2dest, user_ptr_d2dest,
			     func, user_ptr_func, exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vfillf_via_udf() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  自身の配列をスカラー値で書き換え (ユーザ関数経由・IDLの記法)
 */
mdarray &mdarray::vfillf_via_udf( double value, 
	void (*func_dest2d)(const void *,void *,size_t,int,void *), void *user_ptr_dest2d,
	void (*func_d2dest)(const void *,void *,size_t,int,void *), void *user_ptr_d2dest,
	void (*func)(double [],double,size_t, ssize_t,ssize_t,ssize_t,mdarray *,void *),
	void *user_ptr_func,
	const char *exp_fmt, va_list ap )
{
    return this->image_vfillf( value, 
			       func_dest2d, user_ptr_dest2d,
			       func_d2dest, user_ptr_d2dest,
			       func, user_ptr_func, exp_fmt, ap );
}

/* add a scalar value to element values in a section */
/**
 * @brief  自身の配列に，指定されたスカラー値で加算 (IDLの記法)
 */
mdarray &mdarray::addf( double value, const char *exp_fmt, ... )
{
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	this->vaddf(value, exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vaddf() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  自身の配列に，指定されたスカラー値で加算 (IDLの記法)
 */
mdarray &mdarray::vaddf( double value, const char *exp_fmt, va_list ap )
{
    struct mdarray_calc_arr_nd_prms prms;
    void (*func)(const void *,void *,size_t,int,void *);

    prms.ope = Ope_plus;
    prms.round_flg = this->rounding();

    func = NULL;
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
    if ( DOUBLE_ZT == org_sz_type && this->size_type() == new_sz_type ) { \
	func = &calc_arr_nd::fncname; \
    }
    SLI__MDARRAY__DO_OPERATION_2TYPES(SEL_FUNC,,,,,,,,,,,,,else);
#undef SEL_FUNC

    if ( func != NULL ) {
	this->image_vfillf( value, func, &prms, exp_fmt, ap );
    }
    else {
	/* for complex number : not fast */
	this->image_vfillf( value, 
			    NULL, (void *)NULL, NULL, (void *)NULL, 
			    &add_func, NULL, exp_fmt, ap );
    }

    return *this;

    /*
    return this->image_vcalcf_scalar( value, Ope_plus, exp_fmt, ap );
    */
}

/* subtract a scalar value from element values in a section */
/**
 * @brief  自身の配列を，指定されたスカラー値で減算 (IDLの記法)
 */
mdarray &mdarray::subtractf( double value, const char *exp_fmt, ... )
{
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	this->vsubtractf(value, exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vsubtractf() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  自身の配列を，指定されたスカラー値で減算 (IDLの記法)
 */
mdarray &mdarray::vsubtractf( double value, const char *exp_fmt, va_list ap )
{
    struct mdarray_calc_arr_nd_prms prms;
    void (*func)(const void *,void *,size_t,int,void *);

    prms.ope = Ope_minus;
    prms.round_flg = this->rounding();

    func = NULL;
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
    if ( DOUBLE_ZT == org_sz_type && this->size_type() == new_sz_type ) { \
	func = &calc_arr_nd::fncname; \
    }
    SLI__MDARRAY__DO_OPERATION_2TYPES(SEL_FUNC,,,,,,,,,,,,,else);
#undef SEL_FUNC

    if ( func != NULL ) {
	this->image_vfillf( value, func, &prms, exp_fmt, ap );
    }
    else {
	/* for complex number : not fast */
	this->image_vfillf( value, 
			    NULL, (void *)NULL, NULL, (void *)NULL, 
			    &subtract_func, NULL, exp_fmt, ap );
    }

    return *this;

    /*
    return this->image_vcalcf_scalar( value, Ope_minus, exp_fmt, ap );
    */
}

/* multiply element values in a section by a scalar value */
/**
 * @brief  自身の配列に，指定されたスカラー値を乗算 (IDLの記法)
 */
mdarray &mdarray::multiplyf( double value, const char *exp_fmt, ... )
{
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	this->vmultiplyf(value, exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vmultiplyf() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  自身の配列に，指定されたスカラー値を乗算 (IDLの記法)
 */
mdarray &mdarray::vmultiplyf( double value, const char *exp_fmt, va_list ap )
{
    struct mdarray_calc_arr_nd_prms prms;
    void (*func)(const void *,void *,size_t,int,void *);

    prms.ope = Ope_star;
    prms.round_flg = this->rounding();

    func = NULL;
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
    if ( DOUBLE_ZT == org_sz_type && this->size_type() == new_sz_type ) { \
	func = &calc_arr_nd::fncname; \
    }
    SLI__MDARRAY__DO_OPERATION_2TYPES(SEL_FUNC,,,,,,,,,,,,,else);
#undef SEL_FUNC

    if ( func != NULL ) {
	this->image_vfillf( value, func, &prms, exp_fmt, ap );
    }
    else {
	/* for complex number : not fast */
	this->image_vfillf( value, 
			    NULL, (void *)NULL, NULL, (void *)NULL, 
			    &multiply_func, NULL, exp_fmt, ap );
    }

    return *this;

    /*
    return this->image_vcalcf_scalar( value, Ope_star, exp_fmt, ap );
    */
}

/* divide element values in a section by a scalar value */
/**
 * @brief  自身の配列を，指定されたスカラー値で除算 (IDLの記法)
 */
mdarray &mdarray::dividef( double value, const char *exp_fmt, ... )
{
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	this->vdividef(value, exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vdividef() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  自身の配列を，指定されたスカラー値で除算 (IDLの記法)
 */
mdarray &mdarray::vdividef( double value, const char *exp_fmt, va_list ap )
{
    struct mdarray_calc_arr_nd_prms prms;
    void (*func)(const void *,void *,size_t,int,void *);

    prms.ope = Ope_slash;
    prms.round_flg = this->rounding();

    func = NULL;
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
    if ( DOUBLE_ZT == org_sz_type && this->size_type() == new_sz_type ) { \
	func = &calc_arr_nd::fncname; \
    }
    SLI__MDARRAY__DO_OPERATION_2TYPES(SEL_FUNC,,,,,,,,,,,,,else);
#undef SEL_FUNC

    if ( func != NULL ) {
	this->image_vfillf( value, func, &prms, exp_fmt, ap );
    }
    else {
	/* for complex number : not fast */
	this->image_vfillf( value, 
			    NULL, (void *)NULL, NULL, (void *)NULL, 
			    &divide_func, NULL, exp_fmt, ap );
    }

    return *this;

    /*
    return this->image_vcalcf_scalar( value, Ope_slash, exp_fmt, ap );
    */
}

/* paste without operation using fast method of that of .convert().   */
/* 演算なしペースト: convert() と同じ手法で変換した結果を高速に格納． */
/**
 * @brief  自身の配列に指定されたオブジェクトの配列を貼り付け (IDLの記法)
 */
mdarray &mdarray::pastef( const mdarray &src,
			  const char *exp_fmt, ... )
{
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	void (*func)(const void *,void *,size_t,int,void *);
	func = NULL;
	this->vpastef_via_udf(src,func,NULL,exp_fmt,ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vpastef() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  自身の配列に指定されたオブジェクトの配列を貼り付け (IDLの記法)
 */
mdarray &mdarray::vpastef( const mdarray &src,
			   const char *exp_fmt, va_list ap )
{
    void (*func)(const void *,void *,size_t,int,void *);
    func = NULL;
    return this->vpastef_via_udf(src,func,NULL,exp_fmt,ap);
}

/**
 * @brief  自身の配列に指定された配列を貼り付け (ユーザ関数経由・IDLの記法)
 */
mdarray &mdarray::pastef_via_udf( const mdarray &src,
	  void (*func)(const void *,void *,size_t,int,void *), void *user_ptr,
	  const char *exp_fmt, ... )
{
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	this->vpastef_via_udf(src,func,user_ptr,exp_fmt,ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vpastef_via_udf() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  自身の配列に指定された配列を貼り付け (ユーザ関数経由・IDLの記法)
 */
mdarray &mdarray::vpastef_via_udf( const mdarray &src,
	  void (*func)(const void *,void *,size_t,int,void *), void *user_ptr,
	  const char *exp_fmt, va_list ap )
{
    /* private member function */
    return this->image_vpastef(src, func, user_ptr, exp_fmt, ap);
}

/* paste with operation: all elements are converted into double type, so */
/* the performance is inferior to above paste().                         */
/* 演算ありペースト: 一旦 double 型に変換されてから演算した結果を格納．  */
/* 当然，↑の演算なし版に比べて遅い．                                    */
/**
 * @brief  自身の配列に指定された配列を貼り付け (ユーザ関数経由・IDLの記法)
 */
mdarray &mdarray::pastef_via_udf( const mdarray &src,
	void (*func)(double [],double [],size_t, ssize_t,ssize_t,ssize_t,mdarray *,void *),
	void *user_ptr,
	const char *exp_fmt, ... )
{
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	this->vpastef_via_udf(src,func,user_ptr,exp_fmt,ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vpastef_via_udf() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  自身の配列に指定された配列を貼り付け (ユーザ関数経由・IDLの記法)
 */
mdarray &mdarray::vpastef_via_udf( const mdarray &src,
	void (*func)(double [],double [],size_t, ssize_t,ssize_t,ssize_t,mdarray *,void *),
	void *user_ptr,
	const char *exp_fmt, va_list ap )
{
    this->image_vpastef(src, NULL, NULL, NULL, NULL, NULL, NULL,
			func, user_ptr, exp_fmt, ap);
    return *this;
}

/**
 * @brief  自身の配列に指定された配列を貼り付け (ユーザ関数経由・IDLの記法)
 */
mdarray &mdarray::pastef_via_udf( const mdarray &src,
	void (*func_src2d)(const void *,void *,size_t,int,void *), 
	void *user_ptr_src2d,
	void (*func_dest2d)(const void *,void *,size_t,int,void *), 
	void *user_ptr_dest2d,
	void (*func_d2dest)(const void *,void *,size_t,int,void *), 
	void *user_ptr_d2dest,
	void (*func)(double [],double [],size_t, ssize_t,ssize_t,ssize_t,mdarray *,void *),
	void *user_ptr_func,
	const char *exp_fmt, ... )
{
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	this->vpastef_via_udf(src, func_src2d, user_ptr_src2d,
			      func_dest2d, user_ptr_dest2d,
			      func_d2dest, user_ptr_d2dest,
			      func, user_ptr_func, exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vpastef_via_udf() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  自身の配列に指定された配列を貼り付け (ユーザ関数経由・IDLの記法)
 */
mdarray &mdarray::vpastef_via_udf( const mdarray &src,
	void (*func_src2d)(const void *,void *,size_t,int,void *), 
	void *user_ptr_src2d,
	void (*func_dest2d)(const void *,void *,size_t,int,void *), 
	void *user_ptr_dest2d,
	void (*func_d2dest)(const void *,void *,size_t,int,void *), 
	void *user_ptr_d2dest,
	void (*func)(double [],double [],size_t, ssize_t,ssize_t,ssize_t,mdarray *,void *),
	void *user_ptr_func,
	const char *exp_fmt, va_list ap )
{
    this->image_vpastef(src, func_src2d, user_ptr_src2d,
			func_dest2d, user_ptr_dest2d,
			func_d2dest, user_ptr_d2dest,
			func, user_ptr_func, exp_fmt, ap);
    return *this;
}

/* add an array object */
/**
 * @brief  自身の配列に，指定されたオブジェクトの配列を加算 (IDLの記法)
 */
mdarray &mdarray::addf( const mdarray &src_img, const char *exp_fmt, ... )
{
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	this->vaddf(src_img, exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vaddf() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  自身の配列に，指定されたオブジェクトの配列を加算 (IDLの記法)
 */
mdarray &mdarray::vaddf( const mdarray &src_img, const char *exp_fmt, va_list ap )
{
    struct mdarray_calc_arr_nd_prms prms;
    void (*func)(const void *,void *,size_t,int,void *);

    prms.ope = Ope_plus;
    prms.round_flg = this->rounding();

    func = NULL;
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
    if ( src_img.size_type() == org_sz_type && this->size_type() == new_sz_type ) { \
	func = &calc_arr_nd::fncname; \
    }
    SLI__MDARRAY__DO_OPERATION_2TYPES(SEL_FUNC,,,,,,,,,,,,,else);
#undef SEL_FUNC

    if ( func != NULL ) {
	this->image_vpastef( src_img, func, &prms, exp_fmt, ap );
    }
    else {
	/* for complex number : not fast */
	this->image_vpastef( src_img, 
		    NULL, (void *)NULL, NULL, (void *)NULL, NULL, (void *)NULL,
		    &add_func_n, NULL, exp_fmt, ap );
    }

    return *this;
}

/* subtract an array object */
/**
 * @brief  自身の配列を，指定されたオブジェクトの配列で減算 (IDLの記法)
 */
mdarray &mdarray::subtractf( const mdarray &src_img, const char *exp_fmt, ... )
{
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	this->vsubtractf(src_img, exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vsubtractf() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  自身の配列を，指定されたオブジェクトの配列で減算 (IDLの記法)
 */
mdarray &mdarray::vsubtractf( const mdarray &src_img, const char *exp_fmt, va_list ap )
{
    struct mdarray_calc_arr_nd_prms prms;
    void (*func)(const void *,void *,size_t,int,void *);

    prms.ope = Ope_minus;
    prms.round_flg = this->rounding();

    func = NULL;
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
    if ( src_img.size_type() == org_sz_type && this->size_type() == new_sz_type ) { \
	func = &calc_arr_nd::fncname; \
    }
    SLI__MDARRAY__DO_OPERATION_2TYPES(SEL_FUNC,,,,,,,,,,,,,else);
#undef SEL_FUNC

    if ( func != NULL ) {
	this->image_vpastef( src_img, func, &prms, exp_fmt, ap );
    }
    else {
	/* for complex number : not fast */
	this->image_vpastef( src_img, 
		    NULL, (void *)NULL, NULL, (void *)NULL, NULL, (void *)NULL,
		    &subtract_func_n, NULL, exp_fmt, ap );
    }

    return *this;
}

/* multiply an array object */
/**
 * @brief  自身の配列に，指定されたオブジェクトの配列を乗算 (IDLの記法)
 */
mdarray &mdarray::multiplyf( const mdarray &src_img, const char *exp_fmt, ... )
{
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	this->vmultiplyf(src_img, exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vmultiplyf() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  自身の配列に，指定されたオブジェクトの配列を乗算 (IDLの記法)
 */
mdarray &mdarray::vmultiplyf( const mdarray &src_img, const char *exp_fmt, va_list ap )
{
    struct mdarray_calc_arr_nd_prms prms;
    void (*func)(const void *,void *,size_t,int,void *);

    prms.ope = Ope_star;
    prms.round_flg = this->rounding();

    func = NULL;
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
    if ( src_img.size_type() == org_sz_type && this->size_type() == new_sz_type ) { \
	func = &calc_arr_nd::fncname; \
    }
    SLI__MDARRAY__DO_OPERATION_2TYPES(SEL_FUNC,,,,,,,,,,,,,else);
#undef SEL_FUNC

    if ( func != NULL ) {
	this->image_vpastef( src_img, func, &prms, exp_fmt, ap );
    }
    else {
	/* for complex number : not fast */
	this->image_vpastef( src_img, 
		    NULL, (void *)NULL, NULL, (void *)NULL, NULL, (void *)NULL,
		    &multiply_func_n, NULL, exp_fmt, ap );
    }

    return *this;
}

/* divide an array object */
/**
 * @brief  自身の配列を，指定されたオブジェクトの配列で除算 (IDLの記法)
 */
mdarray &mdarray::dividef( const mdarray &src_img, const char *exp_fmt, ... )
{
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	this->vdividef(src_img, exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vdividef() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  自身の配列を，指定されたオブジェクトの配列で除算 (IDLの記法)
 */
mdarray &mdarray::vdividef( const mdarray &src_img, const char *exp_fmt, va_list ap )
{
    struct mdarray_calc_arr_nd_prms prms;
    void (*func)(const void *,void *,size_t,int,void *);

    prms.ope = Ope_slash;
    prms.round_flg = this->rounding();

    func = NULL;
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
    if ( src_img.size_type() == org_sz_type && this->size_type() == new_sz_type ) { \
	func = &calc_arr_nd::fncname; \
    }
    SLI__MDARRAY__DO_OPERATION_2TYPES(SEL_FUNC,,,,,,,,,,,,,else);
#undef SEL_FUNC

    if ( func != NULL ) {
	this->image_vpastef( src_img, func, &prms, exp_fmt, ap );
    }
    else {
	/* for complex number : not fast */
	this->image_vpastef( src_img, 
		    NULL, (void *)NULL, NULL, (void *)NULL, NULL, (void *)NULL,
		    &divide_func_n, NULL, exp_fmt, ap );
    }

    return *this;
}


/* 
 * member functions for image processing
 */


/* returns trimmed array */
/**
 * @brief  一部要素を切り出した配列をテンポラリオブジェクトで取得
 *
 * 画像データ向きのメンバ関数で，自身の配列のうち指定された部分だけをテンポラリ
 * オブジェクトにコピーし，それを返します．
 *
 * @param     col_idx コピー元の列位置
 * @param     col_len コピー元の列サイズ
 * @param     row_idx コピー元の行位置
 * @param     row_len コピー元の行サイズ
 * @param     layer_idx コピー元のレイヤ位置
 * @param     layer_len コピー元のレイヤサイズ
 * @return    テンポラリオブジェクト
 * @throw     バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 */
mdarray mdarray::section( ssize_t col_idx, size_t col_len,
			  ssize_t row_idx, size_t row_len, 
			  ssize_t layer_idx, size_t layer_len ) const
{
    mdarray ret;

    this->copy(&ret, col_idx,col_len, row_idx,row_len, layer_idx,layer_len);

    ret.set_scopy_flag();
    return ret;
}


/**
 * @brief  一部要素を別オブジェクトへコピー
 *
 *  画像データ向きのメンバ関数で，自身の配列の内容の一部分を指定されたオブジェ
 *  クト dest_obj へコピーします．<br>
 *  コピー先へは，配列の要素値とコピー元の型，デフォルト値，丸めフラグの属性な
 *  どすべてをコピーします．自身(コピー元) の配列は改変されません．<br>
 *  dest_obj に自身を与えた場合，trim()を実行した場合と同じ結果になります．
 *
 * @param     dest_obj コピー先のオブジェクト
 * @param     col_index コピー元の列位置
 * @param     col_size コピー元の列サイズ
 * @param     row_index コピー元の行位置
 * @param     row_size コピー元の行サイズ
 * @param     layer_idx コピー元のレイヤ位置
 * @param     layer_size コピー元のレイヤサイズ
 * @return    コピーした要素数(列数× 行数× レイヤ数)
 * @throw     バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 */
ssize_t mdarray::copy( mdarray *dest_obj,
		       ssize_t col_index, size_t col_size, 
		       ssize_t row_index, size_t row_size,
		       ssize_t layer_idx, size_t layer_size ) const
{
    debug_report("( mdarray *dest_obj, ssize_t, ... )");
    return this->image_copy( dest_obj, col_index, col_size,
			     row_index, row_size, layer_idx, layer_size );
}

/* convert and copy the value of selected array element. */
/* [not implemented] */
/**
 * @brief  自身の一部要素を型変換した配列を返す
 * @deprecated  未実装
 */
ssize_t mdarray::convert_copy( mdarray *dest,
			       ssize_t col_idx, size_t col_len,
			       ssize_t row_idx, size_t row_len, 
			       ssize_t layer_idx, size_t layer_len ) const
{
    return -1;
}

/* for SFITSIO */
/* [not implemented] */
/**
 * @brief  自身の一部要素を型変換した配列を返す
 * @deprecated  未実装
 */
ssize_t mdarray::convert_copy_via_udf( mdarray *dest,
	   void (*func)(const void *,void *,size_t,int,void *), void *user_ptr,
				    ssize_t col_idx, size_t col_len,
				    ssize_t row_idx, size_t row_len, 
				    ssize_t layer_idx, size_t layer_len ) const
{
    return -1;
}

/* set a section to be copied by move_to() */
/**
 * @brief  次回の move_to() により移動させる領域の指定
 */
mdarray &mdarray::move_from( ssize_t col_index, size_t col_size,
			     ssize_t row_index, size_t row_size,
			     ssize_t layer_index, size_t layer_size )
{
    if ( this->fix_section_args( &col_index, &col_size, 
				 &row_index, &row_size,
				 &layer_index, &layer_size ) < 0 ) {
	/* 指定された範囲が存在しない場合は初期化 */
	c_memset(this->move_from_idx_rec,0,sizeof(this->move_from_idx_rec));
	c_memset(this->move_from_len_rec,0,sizeof(this->move_from_len_rec));
    }
    else {
	this->move_from_idx_rec[0] = col_index;
	this->move_from_idx_rec[1] = row_index;
	this->move_from_idx_rec[2] = layer_index;
	this->move_from_len_rec[0] = col_size;
	this->move_from_len_rec[1] = row_size;
	this->move_from_len_rec[2] = layer_size;
    }
    return *this;
}

/* copy a section specified by move_from() */
/**
 * @brief  move_from() で指定された領域の移動
 */
mdarray &mdarray::move_to( 
		       ssize_t dest_col, ssize_t dest_row, ssize_t dest_layer )
{
    const size_t this_bytes = this->bytes();
    ssize_t col_index, row_index, layer_index;
    size_t col_size, row_size, layer_size;
    ssize_t org_dest_col, org_dest_row, org_dest_layer;

    col_index = this->move_from_idx_rec[0];
    row_index = this->move_from_idx_rec[1];
    layer_index = this->move_from_idx_rec[2];
    col_size = this->move_from_len_rec[0];
    row_size = this->move_from_len_rec[1];
    layer_size = this->move_from_len_rec[2];

    if ( this->length() == 0 ) goto quit;

    /* source 側のチェック */
    if ( this->fix_section_args( &col_index, &col_size, 
				 &row_index, &row_size,
				 &layer_index, &layer_size ) < 0 ) goto quit;

    /* dest 側のチェック */
    org_dest_col = dest_col;
    org_dest_row = dest_row;
    org_dest_layer = dest_layer;
    if ( this->fix_section_args( &dest_col, &col_size, 
				 &dest_row, &row_size,
				 &dest_layer, &layer_size ) < 0 ) goto quit;
    //err_report1(__FUNCTION__,"DEBUG","col_size = %zu",col_size);
    //err_report1(__FUNCTION__,"DEBUG","row_size = %zu",row_size);
    //err_report1(__FUNCTION__,"DEBUG","layer_size = %zu",layer_size);
    /* 元の指定位置が負の値だったら，これが効く */
    if ( org_dest_col < dest_col ) col_index += (dest_col - org_dest_col);
    if ( org_dest_row < dest_row ) row_index += (dest_row - org_dest_row);
    if ( org_dest_layer < dest_layer ) layer_index += (dest_layer - org_dest_layer);

    {
	const size_t col_blen = this_bytes * this->col_length();
	const size_t colrow_blen = col_blen * this->row_length();
	const size_t dest_line_unit = this_bytes * col_size;
	const char *src_ptr = (const char *)(this->data_ptr_cs()) 
			      + colrow_blen * layer_index	/* offset */
			      + col_blen * row_index
			      + this_bytes * col_index;
	char *dest_ptr = (char *)(this->data_ptr())
			 + colrow_blen * dest_layer		/* offset */
			 + col_blen * dest_row
			 + this_bytes * dest_col;
	size_t j, i;
	if ( dest_ptr < src_ptr ) {			/* 後ろから前へ */
	    const char *j_src_ptr = src_ptr;
	    char *j_dest_ptr = dest_ptr;
	    const char *i_src_ptr;
	    char *i_dest_ptr;
	    for ( j=0 ; j < layer_size ; j++ ) {
		i_src_ptr = j_src_ptr;
		i_dest_ptr = j_dest_ptr;
		for ( i=0 ; i < row_size ; i++ ) {
		    s_memmove( i_dest_ptr, i_src_ptr, dest_line_unit );
		    i_dest_ptr += col_blen;
		    i_src_ptr += col_blen;
		}
		j_dest_ptr += colrow_blen;
		j_src_ptr += colrow_blen;
	    }
	}
	else if ( src_ptr < dest_ptr ) {		/* 前から後ろへ */
	    const char *j_src_ptr = src_ptr + (colrow_blen * layer_size);
	    char *j_dest_ptr = dest_ptr + (colrow_blen * layer_size);
	    const char *i_src_ptr;
	    char *i_dest_ptr;
	    for ( j=0 ; j < layer_size ; j++ ) {
		j_dest_ptr -= colrow_blen;
		j_src_ptr -= colrow_blen;
		i_src_ptr = j_src_ptr + (col_blen * row_size);
		i_dest_ptr = j_dest_ptr + (col_blen * row_size);
		for ( i=0 ; i < row_size ; i++ ) {
		    i_dest_ptr -= col_blen;
		    i_src_ptr -= col_blen;
		    s_memmove( i_dest_ptr, i_src_ptr, dest_line_unit );
		}
	    }
	}
    }

 quit:
    return *this;
}

/* trim a section */

/**
 * @brief  指定部分以外の配列要素の消去 (画像データのトリミング)
 *
 * 画像データ向きのメンバ関数で，自身の配列のうち指定された部分だけを残し，
 * 指定外の部分を消去します．<br>
 *
 * @param     col_idx 列位置
 * @param     col_len 列サイズ
 * @param     row_idx 行位置
 * @param     row_len 行サイズ
 * @param     layer_idx レイヤ位置
 * @param     layer_len レイヤサイズ
 * @return    自身の参照
 * @throw     バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 */
mdarray &mdarray::trim( ssize_t col_idx, size_t col_len,
			ssize_t row_idx, size_t row_len, 
			ssize_t layer_idx, size_t layer_len )
{
    this->copy(this, col_idx, col_len, row_idx, row_len, layer_idx, layer_len);
    return *this;
}


/* flip horizontal within a rectangular region */
/**
 * @brief  配列の水平方向での反転
 *
 * 自身の配列の指定された範囲の要素について，その内容を水平方向に反転させます．
 *
 * @param     col_index 列位置
 * @param     col_size 列サイズ
 * @param     row_index 行位置
 * @param     row_size 行サイズ
 * @param     layer_index レイヤ位置
 * @param     layer_size レイヤサイズ
 * @return    自身の参照
 */
mdarray &mdarray::flip_cols( ssize_t col_index, size_t col_size,
			     ssize_t row_index, size_t row_size, 
			     ssize_t layer_index, size_t layer_size )
{
    const size_t this_bytes = this->bytes();
    size_t col_blen, colrow_blen;
    size_t j, k;
    char *k_ptr;
    char *j_ptr;

    if ( this->length() == 0 ) goto quit;

    if ( this->fix_section_args( &col_index, &col_size, 
				 &row_index, &row_size,
				 &layer_index, &layer_size ) < 0 ) goto quit;

    col_blen = this_bytes * this->col_length();
    colrow_blen = col_blen * this->row_length();

    k_ptr = (char *)(this->data_ptr()) 
	    + colrow_blen * layer_index		/* offset */
	    + col_blen * row_index
	    + this_bytes * col_index;
    for ( k=0 ; k < layer_size ; k++ ) {
	j_ptr = k_ptr;
	for ( j=0 ; j < row_size ; j++ ) {
	    s_memflip(j_ptr, this_bytes, col_size);
	    j_ptr += col_blen;
	}
	k_ptr += colrow_blen;
    }

 quit:
    return *this;
}

/* flip vertical within a rectangular region */
/**
 * @brief  配列の垂直方向での反転
 *
 * 自身の配列の指定された範囲の要素について，その内容を垂直方向に反転させます．
 *
 * @param     col_index 列位置
 * @param     col_size 列サイズ
 * @param     row_index 行位置
 * @param     row_size 行サイズ
 * @param     layer_index レイヤ位置
 * @param     layer_size レイヤサイズ
 * @return    自身の参照
 */
mdarray &mdarray::flip_rows( ssize_t col_index, size_t col_size,
			     ssize_t row_index, size_t row_size, 
			     ssize_t layer_index, size_t layer_size )
{
    const size_t this_bytes = this->bytes();
    size_t col_blen, colrow_blen, col_bsize;
    size_t j, k, n_j;
    char *k_ptr;
    char *j_ptr;
    char *j_ptr1;

    if ( this->length() == 0 ) goto quit;

    if ( this->fix_section_args( &col_index, &col_size, 
				 &row_index, &row_size,
				 &layer_index, &layer_size ) < 0 ) goto quit;

    col_blen = this_bytes * this->col_length();
    colrow_blen = col_blen * this->row_length();

    k_ptr = (char *)(this->data_ptr()) 
	    + colrow_blen * layer_index		/* offset */
	    + col_blen * row_index
	    + this_bytes * col_index;
    n_j = row_size / 2;
    col_bsize = this_bytes * col_size;
    for ( k=0 ; k < layer_size ; k++ ) {
	j_ptr = k_ptr;
	j_ptr1 = k_ptr + col_blen * row_size;	/* last + 1 */
	for ( j=0 ; j < n_j ; j++ ) {
	    j_ptr1 -= col_blen;
	    s_memswap(j_ptr, j_ptr1, col_bsize);
	    j_ptr += col_blen;
	}
	k_ptr += colrow_blen;
    }

 quit:
    return *this;
}


/* 正方行列をひっくりかえす */
namespace transp_sq
{
#define MAKE_FUNC(fncname,sz_type,type) \
static void fncname(void *arr_ptr, size_t _len) \
{ \
    const size_t len = _len; \
    const size_t len_plus_1 = len + 1; \
    type *tmp_p0 = (type *)arr_ptr + 1; \
    type *tmp_p1 = (type *)arr_ptr + len * 1; \
    size_t j, k; \
    type tmp_val; \
    for ( j=1 ; j < len ; j++ ) { \
	type *p0 = tmp_p0; \
	type *p1 = tmp_p1; \
	for ( k=j ; k < len ; k++ ) { \
	    tmp_val = *p0; \
	    *p0 = *p1; \
	    *p1 = tmp_val; \
	    p0 ++;		/* 横に進める */ \
	    p1 += len;		/* 縦に進める */ \
	} \
	tmp_p0 += len_plus_1;	/* 斜めに進める */ \
	tmp_p1 += len_plus_1;	/* 斜めに進める */ \
    } \
}
SLI__MDARRAY__DO_OPERATION_1TYPE_ALL(MAKE_FUNC,);
#undef MAKE_FUNC
}

/* 汎用2次元データのコピー関数 */
namespace gencpy2d
{
#define MAKE_FUNC(fncname,src_sz_type,src_type,dest_sz_type,dest_type,fnc) \
static void fncname(const void *_src, void *_dest, size_t len_0, size_t _src_step_0, size_t _dest_step_0, size_t len_1, size_t _src_step_1, size_t _dest_step_1, void *u_ptr) \
{ \
    const src_type *src = (const src_type *)_src; \
    dest_type *dest = (dest_type *)_dest; \
    const size_t flg = *((size_t *)u_ptr); \
    const size_t src_step_1 = _src_step_1; \
    const size_t dest_step_1 = _dest_step_1; \
    const size_t src_step_0 = _src_step_0; \
    const size_t dest_step_0 = _dest_step_0; \
    src_type v0; \
    size_t i, j; \
    for ( j=len_1 ; 0 < j ; --j ) { \
	const src_type *s_p = src; \
	dest_type *d_p = dest; \
	for ( i=len_0 ; 0 < i ; --i ) { \
	    v0 = *(s_p); \
	    *d_p = (dest_type)(fnc(v0)); \
	    s_p += src_step_0; \
	    d_p += dest_step_0; \
	} \
	src += src_step_1; \
	dest += dest_step_1; \
    } \
    j = flg; /* junk code for warning */ \
    return; \
}
SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(MAKE_FUNC, flg ? rnd_f2c(v0) : tnc_f2c,flg ? rnd_f2i16(v0) : tnc_f2i16,flg ? rnd_f2i32(v0) : tnc_f2i32,flg ? rnd_f2i64(v0) : tnc_f2i64, flg ? rnd_d2c(v0) : tnc_d2c,flg ? rnd_d2i16(v0) : tnc_d2i16,flg ? rnd_d2i32(v0) : tnc_d2i32,flg ? rnd_d2i64(v0) : tnc_d2i64, flg ? rnd_ld2c(v0) : tnc_ld2c,flg ? rnd_ld2i16(v0) : tnc_ld2i16,flg ? rnd_ld2i32(v0) : tnc_ld2i32,flg ? rnd_ld2i64(v0) : tnc_ld2i64, flg ? rnd_fx2c(v0) : tnc_fx2c,flg ? rnd_fx2i16(v0) : tnc_fx2i16,flg ? rnd_fx2i32(v0) : tnc_fx2i32,flg ? rnd_fx2i64(v0) : tnc_fx2i64, flg ? rnd_dx2c(v0) : tnc_dx2c,flg ? rnd_dx2i16(v0) : tnc_dx2i16,flg ? rnd_dx2i32(v0) : tnc_dx2i32,flg ? rnd_dx2i64(v0) : tnc_dx2i64, flg ? rnd_ldx2c(v0) : tnc_ldx2c,flg ? rnd_ldx2i16(v0) : tnc_ldx2i16,flg ? rnd_ldx2i32(v0) : tnc_ldx2i32,flg ? rnd_ldx2i64(v0) : tnc_ldx2i64,f2fx,d2dx,ld2ldx,);
#undef MAKE_FUNC
}

/*
 *  正方行列な一時バッファを使って，高速 transpose を行なう．
 *
 *                  src_x_len
 *           src_x_pos
 *            +--------------------+
 *            |                    |
 *   src_y_pos|  - - - - - - - -   |
 *            |  |              |<==== この部分だけ transpose してコピー
 *            |  |              |  |
 *            |  - - - - - - - -   |  +--+
 *            |    dest_height     |  |  | block_len(正方行列の1辺の長さ)
 *            |                    |  +--+
 *            +--------------------+
 *
 *  src_y_step:  通常は1を指定．飛び飛びにとりたい場合は2以上を指定．
 *               (これを使って x-z の transpose が可能)
 *  dest_width:  コピー先の transpose 後の幅．
 *  dest_height: コピー先の transpose 後の高さ．
 *
 */
/**
 * @brief  正方行列な一時バッファを使って，高速 transpose を行なう (内部用)
 *
 * @note   private な関数です．
 */
static void do_transpose_xy_copy( const void *_src, void *_dest,
		  int src_sz_type, size_t src_bytes, 
		  int dest_sz_type, size_t dest_bytes,
		  size_t src_x_pos, size_t src_y_pos, 
		  size_t src_x_len, size_t src_y_step,
		  size_t dest_width, size_t dest_height,
		  size_t block_len, void *_tmp_block,
		  void (*func_gencpy2d)(const void *, void *, 
		    size_t, size_t, size_t, size_t, size_t, size_t, void *),
		  void *user_ptr_func_gencpy2d,
		  void (*func_sq)(void *, size_t) )
{
    const size_t block_blen = src_bytes * block_len;
    const size_t src_x_step = src_x_len * src_y_step;
    unsigned char *tmp_block = (unsigned char *)_tmp_block;  /* 一時バッファ */
    const unsigned char *src = (const unsigned char *)_src;
    unsigned char *dest = (unsigned char *)_dest;
    size_t i, j, k;
    size_t hlen_left, wlen_left;

    if ( func_gencpy2d == NULL ) {
	if ( src_sz_type != dest_sz_type ) {
	    err_throw(__FUNCTION__,"FATAL","invalid sz_type");
	}
    }

    src += src_bytes * (src_x_len * src_y_pos + src_x_pos);	/* offset */

    wlen_left = dest_width;

    if ( 1 < block_len ) {

	for ( ; block_len <= wlen_left ; wlen_left -= block_len ) {
	    const unsigned char *src0 = src;
	    unsigned char *dest0 = dest;
	    /* tmp_block にコピーしてひっくり返す */
	    for ( hlen_left = dest_height ;
		  block_len <= hlen_left ; 
		  hlen_left -= block_len ) {
		const unsigned char *src1;
		/* tmp_block にコピー */
		unsigned char *tmp_p0 = tmp_block;
		unsigned char *tmp_p1;
		src1 = src0;
		for ( j=block_len ; 0 < j ; --j ) {
		    c_memcpy(tmp_p0, src1, block_blen);
		    tmp_p0 += block_blen;
		    src1 += src_bytes * src_x_step;
		}
		/* tmp_blockの中で transpose */
		if ( func_sq != NULL ) {
		    /* each type */
		    (*func_sq)(tmp_block, block_len);
		}
		else {
		    /* fallback */
		    unsigned char tmp_ch;
		    tmp_p0 = tmp_block + (src_bytes * 1);
		    tmp_p1 = tmp_block + (src_bytes * block_len * 1);
		    for ( j=1 ; j < block_len ; j++ ) {
			unsigned char *p0 = tmp_p0;
			unsigned char *p1 = tmp_p1;
			for ( k=j ; k < block_len ; k++ ) {
			    for ( i=0 ; i < src_bytes ; i++ ) {
				tmp_ch = p0[i];
				p0[i] = p1[i];
				p1[i] = tmp_ch;
			    }
			    p0 += src_bytes;		       /* 横に進める */
			    p1 += src_bytes * block_len;       /* 縦に進める */
			}
			tmp_p0 += src_bytes * (block_len + 1);  /* 斜めに進める */
			tmp_p1 += src_bytes * (block_len + 1);  /* 斜めに進める */
		    }
		}
		/* destヘコピー */
		if ( func_gencpy2d != NULL ) {
		    (*func_gencpy2d)( tmp_block, dest0, 
				      block_len,  1, 1,
				      block_len,  block_len, dest_width, 
				      user_ptr_func_gencpy2d );
		    dest0 += dest_bytes * dest_width * block_len;
		}
		else {
		    /* fallback */
		    tmp_p0 = tmp_block;
		    for ( j=0 ; j < block_len ; j++ ) {
			c_memcpy(dest0, tmp_p0, src_bytes * block_len);
			tmp_p0 += src_bytes * block_len;
			dest0 += dest_bytes * dest_width;
		    }
		}
		src0 += src_bytes * block_len;
	    }
	    /* 残った部分を普通にひっくりかえす */
	    if ( func_gencpy2d != NULL ) {
		(*func_gencpy2d)( src0, dest0, 
				  block_len,  src_x_step, 1,
				  hlen_left,  1, dest_width,
				  user_ptr_func_gencpy2d );
	    }
	    else {
		for ( k=0 ; k < hlen_left ; k++ ) {
		    const unsigned char *s_p = src0;
		    unsigned char *d_p = dest0;
		    for ( j=0 ; j < block_len ; j++ ) {
			for ( i=0 ; i < src_bytes ; i++ ) d_p[i] = s_p[i];
			s_p += src_bytes * src_x_step;
			d_p += dest_bytes;
		    }
		    src0 += src_bytes;
		    dest0 += dest_bytes * dest_width;
		}
	    }
	    
	    src += src_bytes * src_x_step * block_len;
	    dest += dest_bytes * block_len;
	}
    }

    /* 残った部分を普通にひっくりかえす */
    if ( func_gencpy2d != NULL ) {
	(*func_gencpy2d)( src, dest, 
			  wlen_left,    src_x_step, 1,
			  dest_height,  1, dest_width,
			  user_ptr_func_gencpy2d );
    }
    else {
	for ( k=0 ; k < dest_height ; k++ ) {
	    const unsigned char *s_p = src;
	    unsigned char *d_p = dest;
	    for ( j=0 ; j < wlen_left ; j++ ) {
		for ( i=0 ; i < src_bytes ; i++ ) d_p[i] = s_p[i];
		s_p += src_bytes * src_x_step;
		d_p += dest_bytes;
	    }
	    src += src_bytes;
	    dest += dest_bytes * dest_width;
	}
    }

    return;
}

/* interchange rows and columns and copy */
/**
 * @brief  (x,y)のトランスポーズを行なった配列を取得
 */
ssize_t mdarray::transpose_xy_copy( mdarray *dest_ret,
				 ssize_t col_index, size_t col_size,
				 ssize_t row_index, size_t row_size, 
				 ssize_t layer_index, size_t layer_size ) const
{
    const size_t this_bytes = this->bytes();
    const size_t col_blen = this_bytes * this->col_length();
    const size_t colrow_blen = col_blen * this->row_length();
    const bool layer_all = (layer_index == 0 && layer_size == MDARRAY_ALL);
    ssize_t ret = -1;
    bool col_ok, row_ok, layer_ok;
    size_t j;
    const char *j_src_ptr;
    char *dest_ptr;
    heap_mem<size_t> tmp_size;
    mdarray tmpobj;
    mdarray *dest_obj;
    size_t tmp_dim_size;
    heap_mem<unsigned char> tmp_block;		/* 正方行列用バッファ */
    size_t len_block, min_len_block, max_len_block, min_size;
    void (*func_gencpy2d)(const void *, void *, 
		       size_t, size_t, size_t, size_t, size_t, size_t, void *);
    size_t f_idx;

    if ( dest_ret == NULL || this->length() == 0 ) goto quit;

    if ( this->test_section_args( &col_index, &col_size, 
				 &row_index, &row_size,
				 &layer_index, &layer_size,
				 &col_ok, &row_ok, &layer_ok ) < 0 ) goto quit;

    //err_report1(__FUNCTION__,"DEBUG","col_size: %zd",col_size);
    //err_report1(__FUNCTION__,"DEBUG","row_size: %zd",row_size);
    //err_report1(__FUNCTION__,"DEBUG","layer_size: %zd",layer_size);

    if ( dest_ret == this ) dest_obj = &tmpobj;	     /* 自分自身の場合 */
    else dest_obj = dest_ret;			     /* 自分自身では無い場合 */

    /* 元の次元情報をコピー */
    tmp_dim_size = this->dim_size_rec;
    tmp_size = dup_size_rec(this->cdimarray(), this->dim_size_rec);
    if ( tmp_size.ptr() == NULL ) {
	err_throw(__FUNCTION__,"FATAL","malloc() failed");
    }

    if ( row_ok == false ) {				/* 1 次元の場合 */
	const size_t ndim = 2;
	const size_t naxisx[ndim] = {row_size, col_size};
	if ( dest_obj->size_type() == this->size_type() &&
	     dest_obj->length() == col_size * row_size ) {
	    dest_obj->reallocate(naxisx, ndim, false);
	}
	else dest_obj->init(this->size_type(), true, naxisx, ndim, true);
    }
    else if ( layer_ok == false ) {			/* 2 次元の場合 */
	const size_t ndim = 2;
	const size_t naxisx[ndim] = {row_size, col_size};
	if ( dest_obj->size_type() == this->size_type() &&
	     dest_obj->length() == col_size * row_size ) {
	    dest_obj->reallocate(naxisx, ndim, false);
	}
	else dest_obj->init(this->size_type(), true, naxisx, ndim, true);
    }
    else {
	/* 4次元以上の場合，全レイヤ指定なら4次元以降の軸情報を残す */
	if ( 3 < this->dim_length() && layer_all == true ) {
	    size_t len_dest = 1;
	    /* 1〜2次元だけ変更 */
	    tmp_size[0] = row_size;
	    tmp_size[1] = col_size;
	    for ( j=0 ; j < tmp_dim_size ; j++ ) len_dest *= tmp_size[j];
	    if ( dest_obj->size_type() == this->size_type() &&
		 dest_obj->length() == len_dest ) {
		dest_obj->reallocate(tmp_size.ptr(), tmp_dim_size, false);
	    }
	    else {
		dest_obj->init(this->size_type(), true, 
			       tmp_size.ptr(), tmp_dim_size, false);
	    }
	}
	/* 3 次元の場合 */
	else {
	    const size_t ndim = 3;
	    const size_t naxisx[ndim] = {row_size, col_size, layer_size};
	    if ( dest_obj->size_type() == this->size_type() &&
		 dest_obj->length() == col_size * row_size * layer_size ) {
		dest_obj->reallocate(naxisx, ndim, false);
	    }
	    else dest_obj->init(this->size_type(), true, naxisx, ndim, true);
	}
    }

    dest_obj->init_properties(*this);
    dest_obj->assign_default(this->default_value_ptr());
    //err_report(__FUNCTION__,"DEBUG","mark 3");

    dest_ptr = (char *)dest_obj->data_ptr();
    j_src_ptr = (const char *)(this->data_ptr_cs())
		+ colrow_blen * layer_index;		/* offset */

    /* PentiumM マシンでのテスト結果 */
    /* (4096x4096 double型)          */
    /* バイト長  経過時間  */
    /*   16^2  ... 1.53s   */
    /*   32^2  ... 1.03s   */
    /*   64^2  ... 0.84s   */
    /*   128^2 ... 0.74s   */
    /*   256^2 ... 0.71s   */
    /*   512^2 ... 0.79s   */

    /* 正方行列バッファのバイト長が 16^2 から 256^2 の値をとるように設定 */
    min_len_block = (size_t)(1.0 + sqrt((double)(16*16) / this->bytes()));
    max_len_block = (size_t)(1.0 + sqrt((double)(256*256) / this->bytes()));
    if ( row_size < col_size ) min_size = row_size;
    else min_size = col_size;

    if ( min_size <= min_len_block ) len_block = min_len_block;
    else {
	size_t ndiv = ( (min_size - 1) / max_len_block ) + 1;
	len_block = min_size / ndiv;
    }

    /* 正方行列バッファ(一時バッファ)を確保 */
    if ( tmp_block.allocate(this->bytes() * len_block * len_block) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","malloc() failed");
    }

    /* transpose 用の汎用コピー関数を選択 */
    func_gencpy2d = NULL;
#define SEL_FUNC(fncname,src_sztp,src_tp,dest_sztp,dest_tp,fnc) \
    if ( this->size_type() == src_sztp && this->size_type() == dest_sztp ) { \
	func_gencpy2d = &gencpy2d::fncname; \
    }
    SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(SEL_FUNC,,,,,,,,,,,,,,,,,,,,,,,,,,,,else);
#undef SEL_FUNC

    if ( this->rounding() == true ) f_idx = 1;		/* 四捨五入で変換 */
    else f_idx = 0;					/* 切り捨てで変換 */

    for ( j=0 ; j < layer_size ; j++ ) {

	do_transpose_xy_copy( j_src_ptr, dest_ptr,
			      this->size_type(), this->bytes(), 
			      this->size_type(), this->bytes(), 
			      col_index, row_index, 
			      this->col_length(), 1,
			      row_size, col_size,
			      len_block, tmp_block.ptr(),
			      func_gencpy2d, &f_idx, 
			      this->func_transp_mtx );

	dest_ptr += this_bytes * row_size * col_size;    /* 次のレイヤへ */
	j_src_ptr += colrow_blen;

    }

    /* 自分自身の場合 */
    if ( dest_ret == this ) {
	dest_ret->swap(tmpobj);
    }

    ret = layer_size * row_size * col_size;

 quit:
    return ret;
}

/* interchange xyz to zxy and copy */
/**
 * @brief  (x,y,z)→(z,x,y)のトランスポーズを行なった配列を取得
 */
ssize_t mdarray::transpose_xyz2zxy_copy( mdarray *dest_ret,
				 ssize_t col_index, size_t col_size,
				 ssize_t row_index, size_t row_size, 
				 ssize_t layer_index, size_t layer_size ) const
{
    const size_t this_bytes = this->bytes();
    const size_t col_blen = this_bytes * this->col_length();
    const size_t colrow_blen = col_blen * this->row_length();
    const bool layer_all = (layer_index == 0 && layer_size == MDARRAY_ALL);
    ssize_t ret = -1;
    bool col_ok, row_ok, layer_ok;
    size_t i_cnt, i, j;
    const char *j_src_ptr;
    char *dest_ptr;
    heap_mem<size_t> tmp_size;
    mdarray tmpobj;
    mdarray *dest_obj;
    size_t tmp_dim_size;
    heap_mem<unsigned char> tmp_block;		/* 正方行列用バッファ */
    size_t len_block, min_len_block, max_len_block, min_size;
    void (*func_gencpy2d)(const void *, void *, 
		       size_t, size_t, size_t, size_t, size_t, size_t, void *);
    size_t f_idx;

    if ( dest_ret == NULL || this->length() == 0 ) goto quit;

    if ( this->test_section_args( &col_index, &col_size, 
				 &row_index, &row_size,
				 &layer_index, &layer_size,
				 &col_ok, &row_ok, &layer_ok ) < 0 ) goto quit;

    if ( dest_ret == this ) dest_obj = &tmpobj;	     /* 自分自身の場合 */
    else dest_obj = dest_ret;			     /* 自分自身では無い場合 */

    /* 元の次元情報をコピー */
    tmp_dim_size = this->dim_size_rec;
    tmp_size = dup_size_rec(this->cdimarray(), this->dim_size_rec);
    if ( tmp_size.ptr() == NULL ) {
	err_throw(__FUNCTION__,"FATAL","malloc() failed");
    }

    if ( row_ok == false ) {				/* 1 次元の場合 */
	const size_t ndim = 3;
	const size_t naxisx[ndim] = {layer_size, col_size, row_size};
	if ( dest_obj->size_type() == this->size_type() &&
	     dest_obj->length() == col_size * row_size * layer_size ) {
	    dest_obj->reallocate(naxisx, ndim, false);
	}
	else dest_obj->init(this->size_type(), true, naxisx, ndim, true);
    }
    else if ( layer_ok == false ) {			/* 2 次元の場合 */
	const size_t ndim = 3;
	const size_t naxisx[ndim] = {layer_size, col_size, row_size};
	if ( dest_obj->size_type() == this->size_type() &&
	     dest_obj->length() == col_size * row_size * layer_size ) {
	    dest_obj->reallocate(naxisx, ndim, false);
	}
	else dest_obj->init(this->size_type(), true, naxisx, ndim, true);
    }
    else {
	/* 4次元以上の場合，全レイヤ指定なら4次元以降の軸情報を残す */
	if ( 3 < this->dim_length() && layer_all == true ) {
	    size_t len_dest = 1;
	    /* 1〜3次元だけ変更 */
	    layer_size = this->length(2);
	    tmp_size[0] = layer_size;
	    tmp_size[1] = col_size;
	    tmp_size[2] = row_size;
	    for ( j=0 ; j < tmp_dim_size ; j++ ) len_dest *= tmp_size[j];
	    if ( dest_obj->size_type() == this->size_type() &&
		 dest_obj->length() == len_dest ) {
		dest_obj->reallocate(tmp_size.ptr(), tmp_dim_size, false);
	    }
	    else {
		dest_obj->init(this->size_type(), true, 
			       tmp_size.ptr(), tmp_dim_size, false);
	    }
	}
	/* 3 次元の場合 */
	else {
	    const size_t ndim = 3;
	    const size_t naxisx[ndim] = {layer_size, col_size, row_size};
	    if ( dest_obj->size_type() == this->size_type() &&
		 dest_obj->length() == col_size * row_size * layer_size ) {
		dest_obj->reallocate(naxisx, ndim, false);
	    }
	    else dest_obj->init(this->size_type(), true, naxisx, ndim, true);
	}
    }

    dest_obj->init_properties(*this);
    dest_obj->assign_default(this->default_value_ptr());

    dest_ptr = (char *)dest_obj->data_ptr();
    j_src_ptr = (const char *)(this->data_ptr_cs())
		+ colrow_blen * layer_index;		/* offset */

    /* 正方行列バッファのバイト長が 16^2 から 256^2 の値をとるように設定 */
    min_len_block = (size_t)(1.0 + sqrt((double)(16*16) / this->bytes()));
    max_len_block = (size_t)(1.0 + sqrt((double)(256*256) / this->bytes()));
    if ( layer_size < col_size ) min_size = layer_size;
    else min_size = col_size;

    if ( min_size <= min_len_block ) len_block = min_len_block;
    else {
	size_t ndiv = ( (min_size - 1) / max_len_block ) + 1;
	len_block = min_size / ndiv;
    }

    /* 正方行列バッファ(一時バッファ)を確保 */
    if ( tmp_block.allocate(this->bytes() * len_block * len_block) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","malloc() failed");
    }

    /* transpose 用の汎用コピー関数を選択 */
    func_gencpy2d = NULL;
#define SEL_FUNC(fncname,src_sztp,src_tp,dest_sztp,dest_tp,fnc) \
    if ( this->size_type() == src_sztp && this->size_type() == dest_sztp ) { \
	func_gencpy2d = &gencpy2d::fncname; \
    }
    SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(SEL_FUNC,,,,,,,,,,,,,,,,,,,,,,,,,,,,else);
#undef SEL_FUNC

    if ( this->rounding() == true ) f_idx = 1;		/* 四捨五入で変換 */
    else f_idx = 0;					/* 切り捨てで変換 */

    i_cnt = 1;
    if ( 3 < this->dim_length() && layer_all == true ) {
	for ( j=3 ; j < tmp_dim_size ; j++ ) i_cnt *= tmp_size[j];
    }

    for ( i=0 ; i < i_cnt ; i++ ) {
	for ( j=0 ; j < row_size ; j++ ) {

	    do_transpose_xy_copy( j_src_ptr, dest_ptr,
				  this->size_type(), this->bytes(), 
				  this->size_type(), this->bytes(), 
				  col_index, row_index + j, 
				  this->col_length(), this->row_length(),
				  layer_size, col_size,
				  len_block, tmp_block.ptr(),
				  func_gencpy2d, &f_idx, 
				  this->func_transp_mtx );

	    dest_ptr += this_bytes * layer_size * col_size;

	}
	j_src_ptr += colrow_blen * this->length(2);
    }

    /* 自分自身の場合 */
    if ( dest_ret == this ) {
	dest_ret->swap(tmpobj);
    }

    ret = layer_size * row_size * col_size * i_cnt;

 quit:
    return ret;
}

/* rotate and copy a region to another mdarray object */
/**
 * @brief  (x,y)で回転させた配列を取得 (回転は90度単位)
 */
ssize_t mdarray::rotate_xy_copy( mdarray *dest_ret, int angle,
				 ssize_t col_index, size_t col_size, 
				 ssize_t row_index, size_t row_size,
				 ssize_t layer_index, size_t layer_size ) const
{
    ssize_t ret = -1;
    int q;

    /* q=1: 左90度回転，q=2: 180度回転，q=3: 右90度回転 */
    if ( 0 <= angle ) q = (angle % 360) / 90;
    else q = (360 + (angle % 360)) / 90;

    if ( dest_ret == NULL || this->length() == 0 ) goto quit;

    /* 回転なしの場合 */
    if ( q == 0 ) {
	ret = this->copy(dest_ret, col_index, col_size, 
			 row_index, row_size, layer_index, layer_size);
    }
    /* 180度回転の場合 */
    else if ( q == 2 ) {
	ret = this->copy(dest_ret, col_index, col_size, 
			 row_index, row_size, layer_index, layer_size);
	dest_ret->flip(0, 0, dest_ret->length(0));
	dest_ret->flip(1, 0, dest_ret->length(1));
    }
    /* 左90度回転の場合 */
    else if ( q == 1 ) {
	ret = this->transpose_xy_copy(dest_ret, col_index, col_size, 
			 row_index, row_size, layer_index, layer_size);
	dest_ret->flip(0, 0, dest_ret->length(0));
    }
    /* 右90度回転の場合 */
    else if ( q == 3 ) {
	ret = this->transpose_xy_copy(dest_ret, col_index, col_size, 
			 row_index, row_size, layer_index, layer_size);
	dest_ret->flip(1, 0, dest_ret->length(1));
    }
    
 quit:
    return ret;
}


/**
 * @brief  自身の配列をデフォルト値でパディング
 *
 *  自身の配列の要素をデフォルト値でパディングします．<br>
 *  引数は指定しなくても使用できます．その場合は，全要素が処理対象です．
 *  clean() を実行しても配列長は変化しません．<br>
 *  画像データ向きのメンバ関数です．<br>
 *
 * @param     col_index 列位置
 * @param     col_size 列サイズ
 * @param     row_index 行位置
 * @param     row_size 行サイズ
 * @param     layer_index レイヤ位置
 * @param     layer_size レイヤサイズ
 * @return    自身の参照
 */
mdarray &mdarray::clean( ssize_t col_index, size_t col_size,
			 ssize_t row_index, size_t row_size,
			 ssize_t layer_index, size_t layer_size )
{
    if ( this->default_value_ptr() != NULL ) {
	this->image_clean( this->default_value_ptr(),
	     col_index,col_size, row_index,row_size, layer_index, layer_size );
    }
    else {
	this->image_clean( NULL,
	     col_index,col_size, row_index,row_size, layer_index, layer_size );
    }
    return *this;
}

/* */

/**
 * @brief  自身の配列を指定されたスカラー値で書き換え
 *
 *  自身の配列の指定された範囲の要素を，指定された値で書換えます．<br>
 *  画像データ向きのメンバ関数です．
 *
 * @param     value 書き込む値
 * @param     col_index 列位置
 * @param     col_size 列サイズ
 * @param     row_index 行位置
 * @param     row_size 行サイズ
 * @param     layer_index レイヤ位置
 * @param     layer_size レイヤサイズ
 * @return    自身の参照
 * @note      メンバ関数1
 */
mdarray &mdarray::fill( double value, ssize_t col_index, size_t col_size, 
			ssize_t row_index, size_t row_size,
			ssize_t layer_index, size_t layer_size )
{
    debug_report("( double value, ssize_t ... )");

    void (*func_d2dest)(const void *,void *,size_t,int,void *);
    struct mdarray_cnv_nd_prms prms;

    prms.total_len_elements = 0;			/* 効かない */
    prms.round_flg = this->rounding();			/* 四捨五入/切り捨て */

    /* 型変換のための関数 (double→自身の型) */
    func_d2dest = this->func_cnv_nd_d2x;

    if ( func_d2dest == NULL ) {
	err_report(__FUNCTION__,"WARNING","invalid sz_type; ignored");
	return *this;
    }

    (*func_d2dest)(&value, this->junk_rec, 1, +1, (void *)&prms);

    return this->image_clean( this->junk_rec,
	     col_index,col_size, row_index,row_size, layer_index, layer_size );
}

/**
 * @brief  自身の配列を指定されたスカラー値で書き換え (ユーザ関数経由)
 *
 *  自身の配列の指定された範囲の要素を，ユーザ定義関数経由で書換えます．<br>
 *  画像データ向きのメンバ関数です．
 *
 * @param      value 書き込む値
 * @param      func 値変換の為のユーザ関数のアドレス
 * @param      user_ptr func の最後に与えられるユーザのポインタ
 * @param      col_index 列位置
 * @param      col_size 列サイズ
 * @param      row_index 行位置
 * @param      row_size 行サイズ
 * @param      layer_index レイヤ位置
 * @param      layer_size レイヤサイズ
 * @return     自身の参照
 *
 */
mdarray &mdarray::fill_via_udf( double value,
	  void (*func)(const void *,void *,size_t,int,void *), void *user_ptr,
				ssize_t col_index, size_t col_size,
				ssize_t row_index, size_t row_size,
				ssize_t layer_index, size_t layer_size )
{
    if ( func == NULL ) {
	return this->fill( value, 
			   col_index, col_size, row_index, row_size,
			   layer_index, layer_size );
    }
    else {
	return this->image_fill( value, func, user_ptr,
				 col_index, col_size, row_index, row_size,
				 layer_index, layer_size );
    }
}

/**
 * @brief  自身の配列を指定されたスカラー値で書き換え (ユーザ関数経由)
 *
 *  自身の配列の指定された範囲の要素を，指定された値とユーザ定義関数経由で書換
 *  えます．<br>
 *  ユーザ定義関数 func の引数には順に，自身の要素値(配列)，value で与えられた
 *  値，配列(最初の引数)の個数，列位置，行位置，レイヤ位置，自身のオブジェクト
 *  のアドレス，ユーザポインタ user_ptr の値，が与えられます．<br>
 *  画像データ向きのメンバ関数です．
 *
 * @param     value 書き込む値
 * @param     func 値変換の為のユーザ関数のアドレス
 * @param     user_ptr func の最後に与えられるユーザのポインタ
 * @param     col_index 列位置
 * @param     col_size 列サイズ
 * @param     row_index 行位置
 * @param     row_size 行サイズ
 * @param     layer_index レイヤ位置
 * @param     layer_size レイヤサイズ
 * @return    自身の参照
 * @note      メンバ関数2
 */
mdarray &mdarray::fill_via_udf( double value, 
	void (*func)(double [],double,size_t, ssize_t,ssize_t,ssize_t,mdarray *,void *),
	void *user_ptr,
	ssize_t col_index, size_t col_size, 
	ssize_t row_index, size_t row_size,
	ssize_t layer_index, size_t layer_size )
{
    debug_report("( double value, double (*func)() ... )");
    if ( func == NULL ) {
	return this->fill( value, 
			   col_index, col_size, row_index, row_size,
			   layer_index, layer_size );
    }
    else {
	return this->image_fill( value, NULL, (void *)NULL, NULL, (void *)NULL,
				 func, user_ptr,
				 col_index, col_size, row_index, row_size,
				 layer_index, layer_size );
    }
}


/**
 * @brief  自身の配列を指定されたスカラー値で書き換え (ユーザ関数経由)
 *
 *  自身の配列の指定された範囲の要素を，指定された値とユーザ定義関数経由で書換
 *  えます．<br>
 *  ユーザ定義関数func の引数には順に，自身の要素値，value で与えられた値，列位
 *  置，行位置，レイヤ位置，自身のオブジェクトのアドレス，ユーザポインタ
 *  user_ptr の値，が与えられます．<br>
 *  画像データ向きのメンバ関数です．
 *
 * @param     value 書き込む値
 * @param     func_dest2d 自身の生データをdouble 型へ変換するためのユーザ関数のアドレス
 * @param     user_ptr_dest2d func dest2d の最後に与えられるユーザのポインタ
 * @param     func_d2dest double 型の値から自身の生データへ変換するためのユーザ関数のアドレス
 * @param     user_ptr_d2dest func d2dest の最後に与えられるユーザのポインタ
 * @param     func 値変換の為のユーザ関数のアドレス
 * @param     user_ptr func の最後に与えられるユーザのポインタ
 * @param     col_index 列位置
 * @param     col_size 列サイズ
 * @param     row_index 行位置
 * @param     row_size 行サイズ
 * @param     layer_index レイヤ位置
 * @param     layer_size レイヤサイズ
 * @return    自身の参照
 * @note      メンバ関数3
 */
mdarray &mdarray::fill_via_udf( double value, 
	void (*func_dest2d)(const void *,void *,size_t,int,void *), void *user_ptr_dest2d,
	void (*func_d2dest)(const void *,void *,size_t,int,void *), void *user_ptr_d2dest,
	void (*func)(double [],double,size_t, ssize_t,ssize_t,ssize_t,mdarray *,void *),
	void *user_ptr_func,
	ssize_t col_index, size_t col_size,
	ssize_t row_index, size_t row_size,
	ssize_t layer_index, size_t layer_size )
{
    return this->image_fill( value, func_dest2d, user_ptr_dest2d,
			     func_d2dest, user_ptr_d2dest,
			     func, user_ptr_func,
			     col_index, col_size, row_index, row_size,
			     layer_index, layer_size );

    return *this;
}

/**
 * @brief  自身の配列に，指定されたスカラー値を加算
 *
 *  自身の配列の指定された範囲の要素に value を加算します．
 *  画像データ向きのメンバ関数です．
 *
 * @param     value 加算する値
 * @param     col_index 列位置
 * @param     col_size 列サイズ
 * @param     row_index 行位置
 * @param     row_size 行サイズ
 * @param     layer_index レイヤ位置
 * @param     layer_size レイヤサイズ
 * @return    自身の参照
 */
mdarray &mdarray::add( double value, ssize_t col_index, size_t col_size, 
		       ssize_t row_index, size_t row_size,
		       ssize_t layer_index, size_t layer_size )
{
    struct mdarray_calc_arr_nd_prms prms;
    void (*func)(const void *,void *,size_t,int,void *);

    prms.ope = Ope_plus;
    prms.round_flg = this->rounding();

    func = NULL;
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
    if ( DOUBLE_ZT == org_sz_type && this->size_type() == new_sz_type ) { \
	func = &calc_arr_nd::fncname; \
    }
    SLI__MDARRAY__DO_OPERATION_2TYPES(SEL_FUNC,,,,,,,,,,,,,else);
#undef SEL_FUNC

    if ( func != NULL ) {
	this->image_fill( value, func, &prms, 
			  col_index, col_size, row_index, row_size,
			  layer_index, layer_size );
    }
    else {
	/* for complex number : not fast */
	this->image_fill( value, 
			  NULL, (void *)NULL, NULL, (void *)NULL, 
			  &add_func, NULL, 
			  col_index, col_size, row_index, row_size,
			  layer_index, layer_size );
    }

    return *this;

    /*
    return this->image_calc_scalar( value, Ope_plus, 
				    col_index, col_size, row_index, row_size,
				    layer_index, layer_size );
    */
}


/**
 * @brief  自身の配列を，指定されたスカラー値で減算
 *
 *  自身の配列の指定された範囲の要素から value を減算します．
 *  画像データ向きのメンバ関数です．
 *
 * @param     value 減算する値
 * @param     col_index 列位置
 * @param     col_size 列サイズ
 * @param     row_index 行位置
 * @param     row_size 行サイズ
 * @param     layer_index レイヤ位置
 * @param     layer_size レイヤサイズ
 * @return    自身の参照
 */
mdarray &mdarray::subtract( double value, ssize_t col_index, size_t col_size, 
			    ssize_t row_index, size_t row_size,
			    ssize_t layer_index, size_t layer_size )
{
    struct mdarray_calc_arr_nd_prms prms;
    void (*func)(const void *,void *,size_t,int,void *);

    prms.ope = Ope_minus;
    prms.round_flg = this->rounding();

    func = NULL;
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
    if ( DOUBLE_ZT == org_sz_type && this->size_type() == new_sz_type ) { \
	func = &calc_arr_nd::fncname; \
    }
    SLI__MDARRAY__DO_OPERATION_2TYPES(SEL_FUNC,,,,,,,,,,,,,else);
#undef SEL_FUNC

    if ( func != NULL ) {
	this->image_fill( value, func, &prms, 
			  col_index, col_size, row_index, row_size,
			  layer_index, layer_size );
    }
    else {
	/* for complex number : not fast */
	this->image_fill( value, 
			  NULL, (void *)NULL, NULL, (void *)NULL, 
			  &subtract_func, NULL, 
			  col_index, col_size, row_index, row_size,
			  layer_index, layer_size );
    }

    return *this;

    /*
    return this->image_calc_scalar( value, Ope_minus, 
				    col_index, col_size, row_index, row_size,
				    layer_index, layer_size );
    */
}


/**
 * @brief  自身の配列に，指定されたスカラー値を乗算
 *
 *  自身の配列の指定された範囲の要素の値に value を乗算します．
 *  画像データ向きのメンバ関数です．
 *
 * @param     value 乗算する値
 * @param     col_index 列位置
 * @param     col_size 列サイズ
 * @param     row_index 行位置
 * @param     row_size 行サイズ
 * @param     layer_index レイヤ位置
 * @param     layer_size レイヤサイズ
 * @return    自身の参照
 */
mdarray &mdarray::multiply( double value, ssize_t col_index, size_t col_size, 
			    ssize_t row_index, size_t row_size,
			    ssize_t layer_index, size_t layer_size )
{
    struct mdarray_calc_arr_nd_prms prms;
    void (*func)(const void *,void *,size_t,int,void *);

    prms.ope = Ope_star;
    prms.round_flg = this->rounding();

    func = NULL;
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
    if ( DOUBLE_ZT == org_sz_type && this->size_type() == new_sz_type ) { \
	func = &calc_arr_nd::fncname; \
    }
    SLI__MDARRAY__DO_OPERATION_2TYPES(SEL_FUNC,,,,,,,,,,,,,else);
#undef SEL_FUNC

    if ( func != NULL ) {
	this->image_fill( value, func, &prms, 
			  col_index, col_size, row_index, row_size,
			  layer_index, layer_size );
    }
    else {
	/* for complex number : not fast */
	this->image_fill( value, 
			  NULL, (void *)NULL, NULL, (void *)NULL, 
			  &multiply_func, NULL, 
			  col_index, col_size, row_index, row_size,
			  layer_index, layer_size );
    }

    return *this;

    /*
    return this->image_calc_scalar( value, Ope_star, 
				    col_index, col_size, row_index, row_size,
				    layer_index, layer_size );
    */
}


/**
 * @brief  自身の配列を，指定されたスカラー値で除算
 *
 *  自身の配列の指定された範囲の要素の値について value で除算します．
 *  画像データ向きのメンバ関数です．
 *
 * @param     value 除算する値
 * @param     col_index 列位置
 * @param     col_size 列サイズ
 * @param     row_index 行位置
 * @param     row_size 行サイズ
 * @param     layer_index レイヤ位置
 * @param     layer_size レイヤサイズ
 * @return    自身の参照
 */
mdarray &mdarray::divide( double value, ssize_t col_index, size_t col_size, 
			  ssize_t row_index, size_t row_size,
			  ssize_t layer_index, size_t layer_size )
{
    struct mdarray_calc_arr_nd_prms prms;
    void (*func)(const void *,void *,size_t,int,void *);

    prms.ope = Ope_slash;
    prms.round_flg = this->rounding();

    func = NULL;
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
    if ( DOUBLE_ZT == org_sz_type && this->size_type() == new_sz_type ) { \
	func = &calc_arr_nd::fncname; \
    }
    SLI__MDARRAY__DO_OPERATION_2TYPES(SEL_FUNC,,,,,,,,,,,,,else);
#undef SEL_FUNC

    if ( func != NULL ) {
	this->image_fill( value, func, &prms, 
			  col_index, col_size, row_index, row_size,
			  layer_index, layer_size );
    }
    else {
	/* for complex number : not fast */
	this->image_fill( value, 
			  NULL, (void *)NULL, NULL, (void *)NULL, 
			  &divide_func, NULL, 
			  col_index, col_size, row_index, row_size,
			  layer_index, layer_size );
    }

    return *this;

    /*
    return this->image_calc_scalar( value, Ope_slash, 
				    col_index, col_size, row_index, row_size,
				    layer_index, layer_size );
    */
}


/* paste without operation using fast method of that of .convert().   */
/* 演算なしペースト: convert() と同じ手法で変換した結果を高速に格納． */

/**
 * @brief  自身の配列に指定されたオブジェクトの配列を貼り付け
 *
 *  自身の配列の指定された範囲の要素値に，src_img で指定されたオブジェクトの
 *  各要素値を貼り付けます．<br>
 *  画像データ向きのメンバ関数です．
 *
 * @param     src_img 源泉となる配列を持つオブジェクト
 * @param     dest_col 列位置
 * @param     dest_row 行位置
 * @param     dest_layer レイヤ位置
 * @return    自身の参照
 * @note      メンバ関数1
 */
mdarray &mdarray::paste( const mdarray &src_img,
		       ssize_t dest_col, ssize_t dest_row, ssize_t dest_layer )
{
    debug_report("( const mdarray &src, ssize_t ... )");

    return this->image_paste(src_img, 
		       (void (*)(const void *,void *,size_t,int,void *))NULL, 
		       (void *)NULL,
		       dest_col, dest_row, dest_layer);
}

/**
 * @brief  自身の配列に指定されたオブジェクトの配列を貼り付け (ユーザ関数経由)
 *
 *  自身の配列の指定された範囲の要素値に，src_img で指定されたオブジェクトの
 *  各要素値をユーザ定義関数経由で貼り付けます．ユーザ定義関数を与えて貼り付
 *  け時の挙動を変えることができます．<br>
 *  画像データ向きのメンバ関数です．
 *
 * @param      src_img 源泉となる配列を持つオブジェクト
 * @param      func 値変換のためのユーザ関数のアドレス
 * @param      user_ptr func の最後に与えられるユーザのポインタ
 * @param      dest_col 列位置
 * @param      dest_row 行位置
 * @param      dest_layer レイヤ位置
 * @return     自身の参照
 */
mdarray &mdarray::paste_via_udf( const mdarray &src_img,
			 void (*func)(const void *,void *,size_t,int,void *),
			 void *user_ptr,
			 ssize_t dst_col, ssize_t dst_row, ssize_t dst_layer )
{
    return this->image_paste( src_img, func, user_ptr, 
			      dst_col, dst_row, dst_layer );
}


/**
 * @brief  自身の配列に指定されたオブジェクトの配列を貼り付け (ユーザ関数経由)
 *
 *  自身の配列の指定された範囲の要素値に，src_img で指定されたオブジェクト
 *  の 各要素値をユーザ定義関数経由で貼り付けます．ユーザ定義関数を与えて
 *  貼り付け時の挙動を変えることができます．<br>
 *  ユーザ定義関数 func の引数には順に，自身の要素値(配列)，オブジェクト 
 *  src_img の要素値(配列)，最初の2つの引数の配列の個数，列位置，行位置，
 *  レイヤ位置，自身のオブジェクトのアドレス，ユーザポインタ user_ptr の値
 *  が与えられます．<br>
 *  画像データ向きのメンバ関数です．
 *
 * @param     src_img 源泉となる配列を持つオブジェクト
 * @param     func 値変換のためのユーザ関数のアドレス
 * @param     user_ptr func の最後に与えられるユーザのポインタ
 * @param     dest_col 列位置
 * @param     dest_row 行位置
 * @param     dest_layer レイヤ位置
 * @return    自身の参照
 * @note      メンバ関数2
 */
mdarray &mdarray::paste_via_udf( const mdarray &src_img,
       void (*func)(double [],double [],size_t, ssize_t,ssize_t,ssize_t,mdarray *,void *),
       void *user_ptr, ssize_t dest_col, ssize_t dest_row, ssize_t dest_layer )
{
    debug_report("( const mdarray &src_img, double (*func)() ... )");
    if ( func == NULL ) {
	return this->paste_via_udf(src_img, 
		       (void (*)(const void *,void *,size_t,int,void *))NULL, 
		       (void *)NULL,
		       dest_col, dest_row, dest_layer);
    } else {
	return this->image_paste( src_img, 
				  NULL, (void *)NULL,
				  NULL, (void *)NULL,
				  NULL, (void *)NULL,
				  func, user_ptr,
				  dest_col, dest_row, dest_layer );
    }
}


/**
 * @brief  自身の配列に指定されたオブジェクトの配列を貼り付け (ユーザ関数経由)
 *
 *  自身の配列の指定された範囲の要素値に，src_img で指定されたオブジェクトの
 *  各要素値をユーザ定義関数経由で貼り付けます．ユーザ定義関数を与えて貼り付
 *  け時の挙動を変えることができます．<br>
 *  画像データ向きのメンバ関数です．
 *
 * @param     src_img 源泉となる配列を持つオブジェクト
 * @param     func_src2d オブジェクト src_img の生データをdouble 型へ変換するためのユーザ関数へのアドレス
 * @param     user_ptr_src2d func_src2d の最後に与えられるユーザのポインタ
 * @param     func_dest2d 自身の生データをdouble 型へ変換するためのユーザ関数へのアドレス
 * @param     user_ptr_dest2d func dest2d の最後に与えられるユーザのポインタ
 * @param     func_d2dest double の値から自身の生データへ変換するためのユーザ関数へのアドレス
 * @param     user_ptr_d2dest func d2dest の最後に与えられるユーザのポインタ
 * @param     func 値変換のためのユーザ関数のアドレス
 * @param     user_ptr func の最後に与えられるユーザのポインタ
 * @param     dest_col 列位置
 * @param     dest_row 行位置
 * @param     dest_layer レイヤ位置
 * @return    自身の参照
 * @note      メンバ関数3
 */
mdarray &mdarray::paste_via_udf( const mdarray &src_img,
	void (*func_src2d)(const void *,void *,size_t,int,void *), void *user_ptr_src2d,
	void (*func_dest2d)(const void *,void *,size_t,int,void *), void *user_ptr_dest2d,
	void (*func_d2dest)(const void *,void *,size_t,int,void *), void *user_ptr_d2dest,
	void (*func)(double [],double [],size_t, ssize_t,ssize_t,ssize_t,mdarray *,void *),
	void *user_ptr_func,
	ssize_t dest_col, ssize_t dest_row, ssize_t dest_layer )
{
    return this->image_paste( src_img,
			      func_src2d, user_ptr_src2d,
			      func_dest2d, user_ptr_dest2d,
			      func_d2dest, user_ptr_d2dest,
			      func, user_ptr_func,
			      dest_col, dest_row, dest_layer);
}


/**
 * @brief  自身の配列に，指定されたオブジェクトの配列を加算
 *
 *  自身の要素にオブジェクト src_img が持つ配列を加算します．
 *  列・行・レイヤについてそれぞれの加算適用開始位置を指定できます．<br>
 *  画像データ向きのメンバ関数です．
 *
 * @param     src_img 演算に使う配列を持つオブジェクト
 * @param     dest_col 加算開始位置(列)
 * @param     dest_row 加算開始位置(行)
 * @param     dest_layer 加算開始位置(レイヤ)
 * @return    自身の参照
 */
mdarray &mdarray::add( const mdarray &src_img,
		       ssize_t dest_col, ssize_t dest_row, ssize_t dest_layer )
{
    struct mdarray_calc_arr_nd_prms prms;
    void (*func)(const void *,void *,size_t,int,void *);

    prms.ope = Ope_plus;
    prms.round_flg = this->rounding();

    func = NULL;
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
    if ( src_img.size_type() == org_sz_type && this->size_type() == new_sz_type ) { \
	func = &calc_arr_nd::fncname; \
    }
    SLI__MDARRAY__DO_OPERATION_2TYPES(SEL_FUNC,,,,,,,,,,,,,else);
#undef SEL_FUNC

    if ( func != NULL ) {
	this->image_paste( src_img, func, &prms, 
			   dest_col, dest_row, dest_layer );
    }
    else {
	/* for complex number : not fast */
	this->image_paste( src_img, 
		    NULL, (void *)NULL, NULL, (void *)NULL, NULL, (void *)NULL,
		    &add_func_n, NULL, dest_col, dest_row, dest_layer );
    }

    return *this;
}

/**
 * @brief  自身の配列を，指定されたオブジェクトの配列で減算
 *
 *  自身の配列の要素値からオブジェクト src_img が持つ配列の要素値を減算します．
 *  列・行・レイヤについてそれぞれの減算適用開始位置を指定できます．<br>
 *  画像データ向きのメンバ関数です．
 *
 * @param     src_img 演算に使う配列を持つオブジェクト
 * @param     dest_col 減算開始位置(列)
 * @param     dest_row 減算開始位置(行)
 * @param     dest_layer 減算開始位置(レイヤ)
 * @return    自身の参照
 */
mdarray &mdarray::subtract( const mdarray &src_img,
		       ssize_t dest_col, ssize_t dest_row, ssize_t dest_layer )
{
    struct mdarray_calc_arr_nd_prms prms;
    void (*func)(const void *,void *,size_t,int,void *);

    prms.ope = Ope_minus;
    prms.round_flg = this->rounding();

    func = NULL;
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
    if ( src_img.size_type() == org_sz_type && this->size_type() == new_sz_type ) { \
	func = &calc_arr_nd::fncname; \
    }
    SLI__MDARRAY__DO_OPERATION_2TYPES(SEL_FUNC,,,,,,,,,,,,,else);
#undef SEL_FUNC

    if ( func != NULL ) {
	this->image_paste( src_img, func, &prms, 
			   dest_col, dest_row, dest_layer );
    }
    else {
	/* for complex number : not fast */
	this->image_paste( src_img, 
		    NULL, (void *)NULL, NULL, (void *)NULL, NULL, (void *)NULL,
		    &subtract_func_n, NULL, dest_col, dest_row, dest_layer );
    }

    return *this;
}

/**
 * @brief  自身の配列に，指定されたオブジェクトの配列を乗算
 *
 *  自身の配列の要素値にオブジェクト src_img が持つ配列を乗算します．
 *  列・行・レイヤについてそれぞれの乗算適用開始位置を指定できます．<br>
 *  画像データ向きのメンバ関数です．
 *
 * @param     src_img 演算に使う配列を持つオブジェクト
 * @param     dest_col 乗算開始位置(列)
 * @param     dest_row 乗算開始位置(行)
 * @param     dest_layer 乗算開始位置(レイヤ)
 * @return    自身の参照
 */
mdarray &mdarray::multiply( const mdarray &src_img,
		       ssize_t dest_col, ssize_t dest_row, ssize_t dest_layer )
{
    struct mdarray_calc_arr_nd_prms prms;
    void (*func)(const void *,void *,size_t,int,void *);

    prms.ope = Ope_star;
    prms.round_flg = this->rounding();

    func = NULL;
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
    if ( src_img.size_type() == org_sz_type && this->size_type() == new_sz_type ) { \
	func = &calc_arr_nd::fncname; \
    }
    SLI__MDARRAY__DO_OPERATION_2TYPES(SEL_FUNC,,,,,,,,,,,,,else);
#undef SEL_FUNC

    if ( func != NULL ) {
	this->image_paste( src_img, func, &prms, 
			   dest_col, dest_row, dest_layer );
    }
    else {
	/* for complex number : not fast */
	this->image_paste( src_img, 
		    NULL, (void *)NULL, NULL, (void *)NULL, NULL, (void *)NULL,
		    &multiply_func_n, NULL, dest_col, dest_row, dest_layer );
    }

    return *this;
}

/**
 * @brief  自身の配列を，指定されたオブジェクトの配列で除算
 *
 *  自身の配列の要素値からオブジェクト src_img が持つ配列を除算します．
 *  列・行・レイヤについてそれぞれの除算適用開始位置を指定できます．<br>
 *  画像データ向きのメンバ関数です．
 *
 * @param     src_img 演算に使う配列を持つオブジェクト
 * @param     dest_col 除算開始位置(列)
 * @param     dest_row 除算開始位置(行)
 * @param     dest_layer 除算開始位置(レイヤ)
 * @return    自身の参照
 */
mdarray &mdarray::divide( const mdarray &src_img,
		       ssize_t dest_col, ssize_t dest_row, ssize_t dest_layer )
{
    struct mdarray_calc_arr_nd_prms prms;
    void (*func)(const void *,void *,size_t,int,void *);

    prms.ope = Ope_slash;
    prms.round_flg = this->rounding();

    func = NULL;
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
    if ( src_img.size_type() == org_sz_type && this->size_type() == new_sz_type ) { \
	func = &calc_arr_nd::fncname; \
    }
    SLI__MDARRAY__DO_OPERATION_2TYPES(SEL_FUNC,,,,,,,,,,,,,else);
#undef SEL_FUNC

    if ( func != NULL ) {
	this->image_paste( src_img, func, &prms, 
			   dest_col, dest_row, dest_layer );
    }
    else {
	/* for complex number : not fast */
	this->image_paste( src_img, 
		    NULL, (void *)NULL, NULL, (void *)NULL, NULL, (void *)NULL,
		    &divide_func_n, NULL, dest_col, dest_row, dest_layer );
    }

    return *this;
}


/*
 * member functions to scan pixels (for image statistics, etc.)
 */


/**
 * @brief  section() 等の引数を調べて適切な座標のパラメータを設定
 *
 *  引数の座標が正しい位置を示してるかを判断し，
 *  正しくない場合(はみ出している場合)は座標のパラメータを調整します．
 *
 * @param   r_col_index 列位置
 * @param   r_col_size 列サイズ
 * @param   r_row_index 行位置
 * @param   r_row_size 行サイズ
 * @param   r_layer_index レイヤ位置
 * @param   r_layer_size レイヤサイズ
 * @return  引数が自身の配列の領域内を示していた場合は0，
 *          領域からはみ出しているが有効領域が存在する場合は正の値，
 *          有効領域が無い場合は負値　
 */
int mdarray::fix_section_args( ssize_t *r_col_index, size_t *r_col_size,
			       ssize_t *r_row_index, size_t *r_row_size,
			       ssize_t *r_layer_index, size_t *r_layer_size
			     ) const
{
    int status;
    ssize_t col_index = 0;
    size_t col_size   = MDARRAY_ALL;
    ssize_t row_index = 0;
    size_t row_size   = MDARRAY_ALL;
    ssize_t layer_index = 0;
    size_t layer_size   = MDARRAY_ALL;
    bool col_ok, row_ok, layer_ok;

    if ( r_col_index != NULL ) col_index = *r_col_index;
    if ( r_col_size != NULL ) col_size = *r_col_size;

    if ( r_row_index != NULL ) row_index = *r_row_index;
    if ( r_row_size != NULL ) row_size = *r_row_size;

    if ( r_layer_index != NULL ) layer_index = *r_layer_index;
    if ( r_layer_size != NULL ) layer_size = *r_layer_size;

    status = this->test_section_args( &col_index, &col_size, 
				      &row_index, &row_size,
				      &layer_index, &layer_size,
				      &col_ok, &row_ok, &layer_ok );

    if ( r_col_index != NULL ) *r_col_index = col_index;
    if ( r_col_size != NULL ) *r_col_size = col_size;
    if ( r_row_index != NULL ) *r_row_index = row_index;
    if ( r_row_size != NULL ) *r_row_size = row_size;
    if ( r_layer_index != NULL ) *r_layer_index = layer_index;
    if ( r_layer_size != NULL ) *r_layer_size = layer_size;

    //if ( r_col_ok != NULL ) *r_col_ok = col_ok;
    //if ( r_row_ok != NULL ) *r_row_ok = row_ok;
    //if ( r_layer_ok != NULL ) *r_layer_ok = layer_ok;

    return status;
}


/* horizontally scans the specified section.  A temporary buffer of 1-D */
/* array is prepared and scan_along_x() returns it.                     */
/* A scan order is displayed in pseudocode:                             */
/*  for(...) {      <- layer                                            */
/*    for(...) {    <- row                                              */
/*      for(...) {  <- column                                           */

/**
 * @brief  配列要素のx方向線分毎でのスキャン・シーケンスの開始
 *
 * 自身の配列の指定された範囲の要素を，行単位でスキャン(ラスタースキャン)するた
 * めの準備をします．行単位のスキャンを行なう場合，次の手順で一連のAPIを使いま
 * す．<br>
 * - beginf_scan_along_x() で準備をする．<br>
 * - scan_along_x() を行の数だけ呼び出し，行ごとの処理を行なう．<br>
 * - end_scan_along_x() で終了処理(一時バッファの開放)を行なう．<br>
 * scan_along_x() は1行分の要素が入った一時バッファのアドレス(32バイトでアライ
 * ンされている)を返します．この一時バッファはオブジェクトの内部型が何であって
 * も double 型で提供されます．なお，このバッファはオブジェクトで管理されている
 * ため，プログラマが開放してはいけません．<br>
 * scan_along_x() は指定範囲全てのスキャンが完了している場合には NULL を返しま
 * す．<br>
 * 画像データの統計をとる時などに使うメンバ関数です．
 *
 * @param     exp_fmt スキャンする範囲を示す文字列(例: "0:99,*")のための
 *                    フォーマット指定(printfと同様)
 * @param     ... exp_fmtに対応した可変長引数の各要素データ
 * @return    スキャンによる有効要素の数．エラーの場合は負値．
 * @note      "[1:100,*]" のように [] で囲むと，1-indexed として扱われます．
 */
ssize_t mdarray::beginf_scan_along_x( const char *exp_fmt, ... ) const
{
    ssize_t ret;
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	ret = this->vbeginf_scan_along_x(exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vbeginf_scan_along_x() failed");
    }
    va_end(ap);
    return ret;
}

/**
 * @brief  配列要素のx方向線分毎でのスキャン・シーケンスの開始
 *
 * 自身の配列の指定された範囲の要素を，行単位でスキャン(ラスタースキャン)するた
 * めの準備をします．行単位のスキャンを行なう場合，次の手順で一連のAPIを使いま
 * す．<br>
 * - vbeginf_scan_along_x() で準備をする．<br>
 * - scan_along_x() を行の数だけ呼び出し，行ごとの処理を行なう．<br>
 * - end_scan_along_x() で終了処理(一時バッファの開放)を行なう．<br>
 * scan_along_x() は1行分の要素が入った一時バッファのアドレス(32バイトでアライ
 * ンされている)を返します．この一時バッファはオブジェクトの内部型が何であって
 * も double 型で提供されます．なお，このバッファはオブジェクトで管理されている
 * ため，プログラマが開放してはいけません．<br>
 * scan_along_x() は指定範囲全てのスキャンが完了している場合には NULL を返しま
 * す．<br>
 * 画像データの統計をとる時などに使うメンバ関数です．
 *
 * @param     exp_fmt スキャンする範囲を示す文字列(例: "0:99,*")のための
 *                    フォーマット指定(printfと同様)
 * @param     ap exp_fmtに対応した可変長引数のリスト
 * @return    スキャンによる有効要素の数．エラーの場合は負値．
 * @note      "[1:100,*]" のように [] で囲むと，1-indexed として扱われます．
 */
ssize_t mdarray::vbeginf_scan_along_x( const char *exp_fmt, va_list ap ) const
{
    ssize_t ret_len = -1;
    mdarray *thisp = (mdarray *)this;			/* 強制的にキャスト */
    heap_mem<char> secexp;
    heap_mem<mdarray_section_expinfo> secinfo;
    size_t n_secinfo = 0;
    size_t scan_ndim, bblk, blk3, i ;

    if ( this->length() == 0 ) goto quit;

    if ( exp_fmt == NULL ) goto quit;

    if ( c_vasprintf(secexp.ptr_address(), exp_fmt, ap) < 0 ) {
	err_report(__FUNCTION__,"FATAL","c_vasprintf() failed");
	goto quit;
    }

    /* パースし，secinfo を生成 */
    if ( parse_section_exp(&secinfo, &n_secinfo, secexp.ptr()) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","Error in parse_section_exp()");
    }
    if ( n_secinfo == 0 ) goto quit;

    /* secinfo をチェック，修正 */
    if ( test_section_info( secexp.ptr(), *this, 
			    &secinfo, &n_secinfo, NULL ) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","Error in test_section_info()");
    }
    if ( n_secinfo == 0 ) goto quit;

    /* 最低でも3次元とする(返り値x,y,zのため) */
    if ( this->dim_length() < 3 ) scan_ndim = 3;
    else scan_ndim = this->dim_length();

    if ( thisp->scanx_ndim != scan_ndim ) {
	if ( thisp->scanx_begin.allocate( scan_ndim ) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","malloc() failed");
	}
	if ( thisp->scanx_len.allocate( scan_ndim ) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","malloc() failed");
	}
	if ( thisp->scanx_cntr.allocate( scan_ndim ) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","malloc() failed");
	}
	thisp->scanx_ndim = scan_ndim;
    }

    for ( i=0 ; i < n_secinfo ; i++ ) {
	thisp->scanx_begin[i] = secinfo[i].begin;
	thisp->scanx_len[i]   = secinfo[i].length;
    }
    for ( ; i < scan_ndim ; i++ ) {
	thisp->scanx_begin[i] = 0;
	thisp->scanx_len[i]   = this->length(i);
    }

    /* 現在位置をセット */
    bblk = this->bytes();
    blk3 = 1;
    thisp->scanx_cptr = (const char *)(this->data_ptr_cs());
    ret_len = 1;
    thisp->scanx_ret_z = 0;
    for ( i=0 ; i < scan_ndim ; i++ ) {
	thisp->scanx_cntr[i]  = 0;
	thisp->scanx_cptr += bblk * thisp->scanx_begin[i];
	bblk *= this->length(i);
	ret_len *= thisp->scanx_len[i];
	if ( 2 <= i ) {
	    thisp->scanx_ret_z += blk3 * thisp->scanx_begin[i];
	    blk3 *= this->length(i);
	}
    }

 quit:
    return ret_len;
}

/**
 * @brief  配列要素のx方向線分毎でのスキャン・シーケンスの開始
 *
 * 自身の配列の指定された範囲の要素を，行単位でスキャン(ラスタースキャン)するた
 * めの準備をします．行単位のスキャンを行なう場合，次の手順で一連のAPIを使いま
 * す．<br>
 * - begin_scan_along_x() で準備をする．<br>
 * - scan_along_x() を行の数だけ呼び出し，行ごとの処理を行なう．<br>
 * - end_scan_along_x() で終了処理(一時バッファの開放)を行なう．<br>
 * scan_along_x() は1行分の要素が入った一時バッファのアドレス(32バイトでアライ
 * ンされている)を返します．この一時バッファはオブジェクトの内部型が何であって
 * も double 型で提供されます．なお，このバッファはオブジェクトで管理されている
 * ため，プログラマが開放してはいけません．<br>
 * scan_along_x() は指定範囲全てのスキャンが完了している場合には NULL を返しま
 * す．<br>
 * 画像データの統計をとる時などに使うメンバ関数です．
 *
 * @param     col_index 列位置
 * @param     col_size 列サイズ
 * @param     row_index 行位置
 * @param     row_size 行サイズ
 * @param     layer_index レイヤ位置
 * @param     layer_size レイヤサイズ
 * @return    スキャンによる有効要素の数．エラーの場合は負値．
 */
ssize_t mdarray::begin_scan_along_x( 
				ssize_t col_index, size_t col_size,
				ssize_t row_index, size_t row_size,
				ssize_t layer_index, size_t layer_size ) const
{
    ssize_t ret_len = -1;
    const size_t this_bytes = this->bytes();
    const size_t col_blen = this_bytes * this->col_length();
    const size_t colrow_blen = col_blen * this->row_length();
    mdarray *thisp = (mdarray *)this;			/* 強制的にキャスト */

    if ( this->length() == 0 ) goto quit;

    if ( this->fix_section_args( &col_index, &col_size, 
				 &row_index, &row_size,
				 &layer_index, &layer_size ) < 0 ) goto quit;

    if ( thisp->scanx_ndim != 3 ) {
	if ( thisp->scanx_begin.allocate( 3 ) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","malloc() failed");
	}
	if ( thisp->scanx_len.allocate( 3 ) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","malloc() failed");
	}
	if ( thisp->scanx_cntr.allocate( 3 ) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","malloc() failed");
	}
	thisp->scanx_ndim = 3;
    }

    thisp->scanx_begin[0] = col_index;	/* スキャン領域の位置を保存 */
    thisp->scanx_begin[1] = row_index;
    thisp->scanx_begin[2] = layer_index;

    thisp->scanx_len[0] = col_size;	/* スキャン領域の大きさを保存 */
    thisp->scanx_len[1] = row_size;
    thisp->scanx_len[2] = layer_size;

    thisp->scanx_cntr[0] = 0;		/* カウンタのリセット([0]は未使用) */
    thisp->scanx_cntr[1] = 0;
    thisp->scanx_cntr[2] = 0;

    /* 現在位置をセット */
    thisp->scanx_cptr = (const char *)(this->data_ptr_cs())
			+ colrow_blen * layer_index		/* offset */
			+ col_blen * row_index
			+ this_bytes * col_index;
    thisp->scanx_ret_z = layer_index;

    ret_len = col_size * row_size * layer_size;

 quit:
    return ret_len;
}

/* private */
/**
 * @brief  this->scanx_len[1] <= this->scanx_cntr[1] の時，次のlayerへいく処理
 * @note   このメンバ関数は private です．
 */
int mdarray::scan_along_x_goto_next() const
{
    int status = 1;
    mdarray *thisp = (mdarray *)this;			/* 強制的にキャスト */

    size_t blen_block = this->bytes();
    size_t len_b3 = 1;
    size_t dim_ix = 1;
    while ( 1 ) {
	size_t stp;
	if ( thisp->scanx_ndim <= dim_ix + 1 ) {
	    /* finish */
	    thisp->scanx_len[0] = 0;
	    goto quit;
	}
	/* 次のレイヤへ(ここのロジックは難しい…orz) */
	stp = this->length(dim_ix) - thisp->scanx_len[dim_ix];
	blen_block *= this->length(dim_ix - 1);
	thisp->scanx_cptr += blen_block * stp;
	if ( dim_ix == 1 ) thisp->scanx_ret_z += 1;
	else if ( dim_ix == 2 ) thisp->scanx_ret_z += stp;
	else {
	    len_b3 *= this->length(dim_ix - 1);
	    thisp->scanx_ret_z += len_b3 * stp;
	}
	thisp->scanx_cntr[dim_ix] = 0;
	dim_ix ++;
	thisp->scanx_cntr[dim_ix] ++;
	if ( thisp->scanx_cntr[dim_ix] < thisp->scanx_len[dim_ix] ) {
	    break;
	}
    }
    status = 0;
 quit:
    return status;
}

/**
 * @brief  配列要素をx方向線分毎に連続的にスキャン (返り値はdouble型)
 */
double *mdarray::scan_along_x( 
			 size_t *n, ssize_t *x, ssize_t *y, ssize_t *z ) const
{
    return this->scan_along_x_via_udf( NULL, NULL, n, x, y, z );
}

/**
 * @brief  配列要素をx方向線分毎に連続的にスキャン (返り値はdouble型)
 */
double *mdarray::scan_along_x_via_udf( 
		       void (*func)(const void *, void *, size_t, int, void *),
		       void *user_ptr,
		       size_t *n, ssize_t *x, ssize_t *y, ssize_t *z ) const
{
    double *ret_ptr = NULL;
    mdarray *thisp = (mdarray *)this;			/* 強制的にキャスト */
    size_t buf_blen;

    if ( thisp->scanx_ndim == 0 ) goto quit;
    if ( thisp->scanx_len[0] == 0 ) goto quit;

    if ( thisp->scanx_len[1] <= thisp->scanx_cntr[1] ) {
	/* 次のレイヤへ */
	if ( this->scan_along_x_goto_next() != 0 ) goto quit;	/* finish */
    }

    if ( func == NULL ) {
	/* 関数を選択 */
	func = this->func_cnv_nd_x2d;
	user_ptr = (void *)&Null_cnv_nd_prms;
	if ( func == NULL ) {
	    err_report1(__FUNCTION__,"ERROR","unsupported size_type: %zd",
			this->size_type());
	    goto quit;
	}
    }

    buf_blen = sizeof(double) * thisp->scanx_len[0];
    if ( thisp->scanx_ret_buf_blen < buf_blen ) {
	/* 返り値用バッファを確保 */
	if ( thisp->scanx_ret_buf.allocate_aligned32(
			     buf_blen, &(thisp->scanx_aligned_ret_ptr)) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","malloc() failed");
	}
	thisp->scanx_ret_buf_blen = buf_blen;
    }

    /* double のバッファへコピー */
    (*func)(thisp->scanx_cptr, thisp->scanx_aligned_ret_ptr,
	    thisp->scanx_len[0], +1, user_ptr);
    ret_ptr = (double *)(thisp->scanx_aligned_ret_ptr);

    if ( n != NULL ) *n = thisp->scanx_len[0];
    if ( x != NULL ) *x = thisp->scanx_begin[0];
    if ( y != NULL ) *y = thisp->scanx_begin[1] + thisp->scanx_cntr[1];
    if ( z != NULL ) *z = thisp->scanx_ret_z;

    thisp->scanx_cptr += this->bytes() * this->col_length();
    thisp->scanx_cntr[1] ++;

 quit:
    return ret_ptr;
}

/**
 * @brief  配列要素をx方向線分毎に連続的にスキャン (返り値はfloat型)
 */
float *mdarray::scan_along_x_f( 
			 size_t *n, ssize_t *x, ssize_t *y, ssize_t *z ) const
{
    return this->scan_along_x_f_via_udf( NULL, NULL, n, x, y, z );
}

/**
 * @brief  配列要素をx方向線分毎に連続的にスキャン (返り値はfloat型)
 */
float *mdarray::scan_along_x_f_via_udf( 
		       void (*func)(const void *, void *, size_t, int, void *),
		       void *user_ptr,
		       size_t *n, ssize_t *x, ssize_t *y, ssize_t *z ) const
{
    float *ret_ptr = NULL;
    mdarray *thisp = (mdarray *)this;			/* 強制的にキャスト */
    size_t buf_blen;

    if ( thisp->scanx_ndim == 0 ) goto quit;
    if ( thisp->scanx_len[0] == 0 ) goto quit;

    if ( thisp->scanx_len[1] <= thisp->scanx_cntr[1] ) {
	/* 次のレイヤへ */
	if ( this->scan_along_x_goto_next() != 0 ) goto quit;	/* finish */
    }

    if ( func == NULL ) {
	/* 関数を選択 */
	func = this->func_cnv_nd_x2f;
	user_ptr = (void *)&Null_cnv_nd_prms;
	if ( func == NULL ) {
	    err_report1(__FUNCTION__,"ERROR","unsupported size_type: %zd",
			this->size_type());
	    goto quit;
	}
    }

    buf_blen = sizeof(float) * thisp->scanx_len[0];
    if ( thisp->scanx_ret_buf_blen < buf_blen ) {
	/* 返り値用バッファを確保 */
	if ( thisp->scanx_ret_buf.allocate_aligned32(
			     buf_blen, &(thisp->scanx_aligned_ret_ptr)) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","malloc() failed");
	}
	thisp->scanx_ret_buf_blen = buf_blen;
    }

    /* float のバッファへコピー */
    (*func)(thisp->scanx_cptr, thisp->scanx_aligned_ret_ptr,
	    thisp->scanx_len[0], +1, user_ptr);
    ret_ptr = (float *)(thisp->scanx_aligned_ret_ptr);

    if ( n != NULL ) *n = thisp->scanx_len[0];
    if ( x != NULL ) *x = thisp->scanx_begin[0];
    if ( y != NULL ) *y = thisp->scanx_begin[1] + thisp->scanx_cntr[1];
    if ( z != NULL ) *z = thisp->scanx_ret_z;

    thisp->scanx_cptr += this->bytes() * this->col_length();
    thisp->scanx_cntr[1] ++;

 quit:
    return ret_ptr;
}

/* 注意: これを mdarray::update_length_info() に登録するのを忘れない事 */
/**
 * @brief  begin_scan_along_x() で開始したスキャン・シーケンスの終了
 */
void mdarray::end_scan_along_x() const
{
    mdarray *thisp = (mdarray *)this;			/* 強制的にキャスト */

    thisp->scanx_begin = NULL;				/* 開放 & 初期化 */
    thisp->scanx_len   = NULL;
    thisp->scanx_cntr  = NULL;
    thisp->scanx_ndim  = 0;

    thisp->scanx_ret_buf = NULL;			/* 開放 & 初期化 */
    thisp->scanx_ret_buf_blen = 0;

    return;
}


/* vertically scans the specified section.  A temporary buffer of 1-D */
/* array is prepared and scan_along_y() returns it.                   */
/* scan order is displayed by pseudocode:                             */
/*  for(...) {      <- layer                                          */
/*    for(...) {    <- column                                         */
/*      for(...) {  <- row                                            */
/**
 * @brief  配列要素のy方向線分毎でのスキャン・シーケンスの開始
 *
 * 自身の配列の指定された範囲の要素を，列単位でスキャン(縦方向のスキャン)する
 * ための準備をします．内部でブロック単位の高速transpose(データアレイにおける
 * x軸とy軸との入れ換え)を行なうため，高いパフォーマンスが得られます．<br>
 * 行単位のスキャンを行なう場合，次の手順で一連のAPIを使います．<br>
 * - beginf_scan_along_y() で準備をする．<br>
 * - scan_along_y() を列の数だけ呼び出し，列ごとの処理を行なう．<br>
 * - end_scan_along_y() で終了処理(一時バッファの開放)を行なう．<br>
 * scan_along_y() は1列分の要素が入った一時バッファのアドレス(32バイトでアライ
 * ンされている)を返します．この一時バッファはオブジェクトの内部型が何であって
 * も double 型で提供されます．なお，このバッファはオブジェクトで管理されている
 * ため，プログラマが開放してはいけません．<br>
 * scan_along_y() は指定範囲全てのスキャンが完了している場合には NULL を返しま
 * す．<br>
 * 画像データの統計をとる時などに使うメンバ関数です．
 *
 * @param     exp_fmt スキャンする範囲を示す文字列(例: "0:99,*")のための
 *                    フォーマット指定(printfと同様)
 * @param     ... exp_fmtに対応した可変長引数の各要素データ
 * @return    スキャンによる有効要素の数．エラーの場合は負値．
 * @note      "[1:100,*]" のように [] で囲むと，1-indexed として扱われます．
 */
ssize_t mdarray::beginf_scan_along_y( const char *exp_fmt, ... ) const
{
    ssize_t ret;
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	ret = this->vbeginf_scan_along_y(exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vbeginf_scan_along_y() failed");
    }
    va_end(ap);
    return ret;
}

/**
 * @brief  配列要素のy方向線分毎でのスキャン・シーケンスの開始
 *
 * 自身の配列の指定された範囲の要素を，列単位でスキャン(縦方向のスキャン)する
 * ための準備をします．内部でブロック単位の高速transpose(データアレイにおける
 * x軸とy軸との入れ換え)を行なうため，高いパフォーマンスが得られます．<br>
 * 行単位のスキャンを行なう場合，次の手順で一連のAPIを使います．<br>
 * - vbeginf_scan_along_y() で準備をする．<br>
 * - scan_along_y() を列の数だけ呼び出し，列ごとの処理を行なう．<br>
 * - end_scan_along_y() で終了処理(一時バッファの開放)を行なう．<br>
 * scan_along_y() は1列分の要素が入った一時バッファのアドレス(32バイトでアライ
 * ンされている)を返します．この一時バッファはオブジェクトの内部型が何であって
 * も double 型で提供されます．なお，このバッファはオブジェクトで管理されている
 * ため，プログラマが開放してはいけません．<br>
 * scan_along_y() は指定範囲全てのスキャンが完了している場合には NULL を返しま
 * す．<br>
 * 画像データの統計をとる時などに使うメンバ関数です．
 *
 * @param     exp_fmt スキャンする範囲を示す文字列(例: "0:99,*")のための
 *                    フォーマット指定(printfと同様)
 * @param     ap exp_fmtに対応した可変長引数のリスト
 * @return    スキャンによる有効要素の数．エラーの場合は負値．
 * @note      "[1:100,*]" のように [] で囲むと，1-indexed として扱われます．
 */
ssize_t mdarray::vbeginf_scan_along_y( const char *exp_fmt, va_list ap ) const
{
    ssize_t ret_len = -1;
    mdarray *thisp = (mdarray *)this;			/* 強制的にキャスト */
    heap_mem<char> secexp;
    heap_mem<mdarray_section_expinfo> secinfo;
    size_t n_secinfo = 0;
    size_t scan_ndim, i;

    if ( this->length() == 0 ) goto quit;

    if ( exp_fmt == NULL ) goto quit;

    if ( c_vasprintf(secexp.ptr_address(), exp_fmt, ap) < 0 ) {
	err_report(__FUNCTION__,"FATAL","c_vasprintf() failed");
	goto quit;
    }

    /* パースし，secinfo を生成 */
    if ( parse_section_exp(&secinfo, &n_secinfo, secexp.ptr()) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","Error in parse_section_exp()");
    }
    if ( n_secinfo == 0 ) goto quit;

    /* secinfo をチェック，修正 */
    if ( test_section_info( secexp.ptr(), *this, 
			    &secinfo, &n_secinfo, NULL ) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","Error in test_section_info()");
    }
    if ( n_secinfo == 0 ) goto quit;

    /* 最低でも3次元とする(返り値x,y,zのため) */
    if ( this->dim_length() < 3 ) scan_ndim = 3;
    else scan_ndim = this->dim_length();

    if ( thisp->scany_ndim != scan_ndim ) {
	if ( thisp->scany_begin.allocate( scan_ndim ) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","malloc() failed");
	}
	if ( thisp->scany_len.allocate( scan_ndim ) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","malloc() failed");
	}
	if ( thisp->scany_cntr.allocate( scan_ndim ) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","malloc() failed");
	}
	thisp->scany_ndim = scan_ndim;
    }

    for ( i=0 ; i < n_secinfo ; i++ ) {
	thisp->scany_begin[i] = secinfo[i].begin;
	thisp->scany_len[i]   = secinfo[i].length;
    }
    for ( ; i < scan_ndim ; i++ ) {
	thisp->scany_begin[i] = 0;
	thisp->scany_len[i]   = this->length(i);
    }

    ret_len = this->begin_scan_along_y_common();

 quit:
    return ret_len;
}

/**
 * @brief  配列要素のy方向線分毎でのスキャン・シーケンスの開始
 *
 * 自身の配列の指定された範囲の要素を，列単位でスキャン(縦方向のスキャン)する
 * ための準備をします．内部でブロック単位の高速transpose(データアレイにおける
 * x軸とy軸との入れ換え)を行なうため，高いパフォーマンスが得られます．<br>
 * 行単位のスキャンを行なう場合，次の手順で一連のAPIを使います．<br>
 * - begin_scan_along_y() で準備をする．<br>
 * - scan_along_y() を列の数だけ呼び出し，列ごとの処理を行なう．<br>
 * - end_scan_along_y() で終了処理(一時バッファの開放)を行なう．<br>
 * scan_along_y() は1列分の要素が入った一時バッファのアドレス(32バイトでアライ
 * ンされている)を返します．この一時バッファはオブジェクトの内部型が何であって
 * も double 型で提供されます．なお，このバッファはオブジェクトで管理されている
 * ため，プログラマが開放してはいけません．<br>
 * scan_along_y() は指定範囲全てのスキャンが完了している場合には NULL を返しま
 * す．<br>
 * 画像データの統計をとる時などに使うメンバ関数です．
 *
 * @param     col_index 列位置
 * @param     col_size 列サイズ
 * @param     row_index 行位置
 * @param     row_size 行サイズ
 * @param     layer_index レイヤ位置
 * @param     layer_size レイヤサイズ
 * @return    スキャンによる有効要素の数．エラーの場合は負値．
 */
ssize_t mdarray::begin_scan_along_y( 
				 ssize_t col_index, size_t col_size,
				 ssize_t row_index, size_t row_size,
				 ssize_t layer_index, size_t layer_size ) const
{
    ssize_t ret_len = -1;
    mdarray *thisp = (mdarray *)this;			/* 強制的にキャスト */

    if ( this->length() == 0 ) goto quit;

    if ( this->fix_section_args( &col_index, &col_size, 
				 &row_index, &row_size,
				 &layer_index, &layer_size ) < 0 ) goto quit;

    if ( thisp->scany_ndim != 3 ) {
	if ( thisp->scany_begin.allocate( 3 ) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","malloc() failed");
	}
	if ( thisp->scany_len.allocate( 3 ) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","malloc() failed");
	}
	if ( thisp->scany_cntr.allocate( 3 ) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","malloc() failed");
	}
	thisp->scany_ndim = 3;
    }

    thisp->scany_begin[0] = col_index;	/* スキャン領域の位置を保存 */
    thisp->scany_begin[1] = row_index;
    thisp->scany_begin[2] = layer_index;

    thisp->scany_len[0] = col_size;	/* スキャン領域の大きさを保存 */
    thisp->scany_len[1] = row_size;
    thisp->scany_len[2] = layer_size;

    ret_len = this->begin_scan_along_y_common();

 quit:
    return ret_len;
}

/* private */
/**
 * @brief  *begin*_scan_along_y() の共通部分
 * @note   このメンバ関数は private です．
 */
ssize_t mdarray::begin_scan_along_y_common() const
{
    ssize_t ret_len = -1;
    const size_t this_bytes = this->bytes();
    const size_t col_blen = this_bytes * this->col_length();
    const size_t colrow_blen = col_blen * this->row_length();
    size_t col_size, row_size;
    size_t len_block, min_len_block, max_len_block, min_size;
    size_t buf_blen;
    size_t blk3, i;
    mdarray *thisp = (mdarray *)this;			/* 強制的にキャスト */

    col_size = thisp->scany_len[0];			/* スキャン領域 */
    row_size = thisp->scany_len[1];

    /* 正方行列バッファのバイト長が 16^2 から 128^2 の値をとるように設定 */
    /* 詳細は transpose_xy_copy() のコード中のコメントを参照             */
    min_len_block = (size_t)(1.0 + sqrt((double)(16*16) / this_bytes));
    max_len_block = (size_t)(1.0 + sqrt((double)(128*128) / this_bytes));
    if ( row_size < col_size ) min_size = row_size;
    else min_size = col_size;

    if ( min_size <= min_len_block ) len_block = min_len_block;
    else {
	size_t ndiv = ( (min_size - 1) / max_len_block ) + 1;
	len_block = min_size / ndiv;
    }

    /* 一時バッファの高さ (可能なら正方行列のサイズにあわせる) */
    if ( col_size < len_block ) thisp->scany_height_trans = col_size;
    else thisp->scany_height_trans = len_block;

    /* transpose用一時バッファ */
    buf_blen = this_bytes * row_size * thisp->scany_height_trans;
    if ( thisp->scany_trans_buf_blen < buf_blen ) {
	if ( thisp->scany_trans_buf.allocate( buf_blen ) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","malloc() failed");
	}
	thisp->scany_trans_buf_blen = buf_blen;
    }

    /* transpose用正方行列の1辺の長さ */
    thisp->scany_len_block = len_block;

    /* 正方行列用バッファ */
    buf_blen = this_bytes * len_block * len_block;
    if ( thisp->scany_mtx_buf_blen < buf_blen ) {
	if ( thisp->scany_mtx_buf.allocate( buf_blen ) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","malloc() failed");
	}
	thisp->scany_mtx_buf_blen = buf_blen;
    }

    ret_len = 1;
    blk3 = 1;
    thisp->scany_ret_z = 0;
    for ( i=0 ; i < thisp->scany_ndim ; i++ ) {
	thisp->scany_cntr[i] = 0;	/* カウンタのリセット([1]は未使用) */
	ret_len *= thisp->scany_len[i];
	if ( 2 <= i ) {
	    thisp->scany_ret_z += blk3 * thisp->scany_begin[i];
	    blk3 *= this->length(i);
	}
    }

    /* 現在位置をセット */
    thisp->scany_cptr = (const char *)(this->data_ptr_cs())
			+ colrow_blen * thisp->scany_ret_z;	/* offset */

    /* transpose 用の汎用コピー関数を選択 */
    thisp->scany_func_gencpy2d = NULL;
#define SEL_FUNC(fncname,src_sztp,src_tp,dest_sztp,dest_tp,fnc) \
    if ( this->size_type() == src_sztp && this->size_type() == dest_sztp ) { \
	thisp->scany_func_gencpy2d = &gencpy2d::fncname; \
    }
    SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(SEL_FUNC,,,,,,,,,,,,,,,,,,,,,,,,,,,,else);
#undef SEL_FUNC

    return ret_len;
}

/* private */
/**
 * @brief  this->scany_len[0] <= this->scany_cntr[0] の時の，次のlayerへいく時の処理
 * @note   このメンバ関数は private です．
 */
int mdarray::scan_along_y_goto_next() const
{
    int status = 1;
    mdarray *thisp = (mdarray *)this;			/* 強制的にキャスト */

    size_t blen_block = this->bytes() * this->col_length();
    size_t len_b3 = 1;
    size_t dim_ix = 2;
    /* 次のレイヤへ */
    thisp->scany_cntr[0] = 0;
    thisp->scany_cptr += blen_block * this->row_length();
    thisp->scany_cntr[2] ++;
    thisp->scany_ret_z ++;
    while ( 1 ) {
	size_t stp;
	if ( thisp->scany_cntr[dim_ix] < thisp->scany_len[dim_ix] ) break;
	if ( thisp->scany_ndim <= dim_ix + 1 ) {
	    /* finish */
	    thisp->scany_len[1] = 0;
	    goto quit;
	}
	/* ここのロジックは難しい…orz */
	stp = this->length(dim_ix) - thisp->scany_len[dim_ix];
	blen_block *= this->length(dim_ix - 1);
	thisp->scany_cptr += blen_block * stp;
	if ( dim_ix == 2 ) thisp->scany_ret_z += stp;
	else {
	    len_b3 *= this->length(dim_ix - 1);
	    thisp->scany_ret_z += len_b3 * stp;
	}
	thisp->scany_cntr[dim_ix] = 0;
	dim_ix ++;
	thisp->scany_cntr[dim_ix] ++;
    }

    status = 0;
 quit:
    return status;
}

/* private */
/**
 * @brief  部分的に transpose を行なう
 * @note   このメンバ関数は private です．
 */
void mdarray::scan_along_y_do_partial_transpose() const
{
    mdarray *thisp = (mdarray *)this;			/* 強制的にキャスト */
    size_t cur_height_trans;
    size_t f_idx;
    if ( thisp->scany_len[0] < 
	 thisp->scany_cntr[0] + thisp->scany_height_trans ) {
	/* this is the last */
	cur_height_trans = thisp->scany_len[0] - thisp->scany_cntr[0];
    }
    else {
	cur_height_trans = thisp->scany_height_trans;
    }
    if ( this->rounding() == true ) f_idx = 1;	/* 四捨五入で変換 */
    else f_idx = 0;					/* 切り捨てで変換 */
    /* 部分的に transpose を行なう */
    do_transpose_xy_copy( thisp->scany_cptr, thisp->scany_trans_buf.ptr(),
			  this->size_type(), this->bytes(),
			  this->size_type(), this->bytes(), 
			  thisp->scany_begin[0] + thisp->scany_cntr[0],
			  thisp->scany_begin[1],
			  this->col_length(), 1,
			  thisp->scany_len[1], cur_height_trans,
			  thisp->scany_len_block, 
			  thisp->scany_mtx_buf.ptr(),
			  thisp->scany_func_gencpy2d, &f_idx,
			  this->func_transp_mtx );
    return;
}

/**
 * @brief  配列要素をy方向線分毎に連続的にスキャン (返り値はdouble型)
 */
double *mdarray::scan_along_y( 
			 size_t *n, ssize_t *x, ssize_t *y, ssize_t *z ) const
{
    return this->scan_along_y_via_udf( NULL, NULL, n, x, y, z );
}

/**
 * @brief  配列要素をy方向線分毎に連続的にスキャン (返り値はdouble型)
 */
double *mdarray::scan_along_y_via_udf( 
		       void (*func)(const void *, void *, size_t, int, void *),
		       void *user_ptr,
		       size_t *n, ssize_t *x, ssize_t *y, ssize_t *z ) const
{
    double *ret_ptr = NULL;
    mdarray *thisp = (mdarray *)this;			/* 強制的にキャスト */
    size_t idx_trans_buf;
    size_t buf_blen;

    if ( thisp->scany_ndim == 0 ) goto quit;
    if ( thisp->scany_len[1] == 0 ) goto quit;

    if ( thisp->scany_len[0] <= thisp->scany_cntr[0] ) {
	/* 次のレイヤへ */
	if ( this->scan_along_y_goto_next() != 0 ) goto quit;	/* finish */
    }

    if ( func == NULL ) {
	/* 関数を選択 */
	func = this->func_cnv_nd_x2d;
	user_ptr = (void *)&Null_cnv_nd_prms;
	if ( func == NULL ) {
	    err_report1(__FUNCTION__,"ERROR","unsupported size_type: %zd",
			this->size_type());
	    goto quit;
	}
    }

    buf_blen = sizeof(double) * thisp->scany_len[1];
    if ( thisp->scany_ret_buf_blen < buf_blen ) {
	/* 返り値用バッファを確保 */
	if ( thisp->scany_ret_buf.allocate_aligned32(
			     buf_blen, &(thisp->scany_aligned_ret_ptr)) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","malloc() failed");
	}
	thisp->scany_ret_buf_blen = buf_blen;
    }

    idx_trans_buf = (thisp->scany_cntr[0] % thisp->scany_height_trans);
    if ( idx_trans_buf == 0 ) {
	/* 部分的に transpose を行なう */
	this->scan_along_y_do_partial_transpose();
    }

    /* 部分的に transpose されたバッファから double のバッファへコピー */
    (*func)(thisp->scany_trans_buf.ptr() 
	    + (this->bytes() * thisp->scany_len[1]) * idx_trans_buf,
	    thisp->scany_aligned_ret_ptr, thisp->scany_len[1], +1, user_ptr);
    ret_ptr = (double *)(thisp->scany_aligned_ret_ptr);

    if ( n != NULL ) *n = thisp->scany_len[1];
    if ( x != NULL ) *x = thisp->scany_begin[0] + thisp->scany_cntr[0];
    if ( y != NULL ) *y = thisp->scany_begin[1];
    if ( z != NULL ) *z = thisp->scany_ret_z;

    thisp->scany_cntr[0] ++;

 quit:
    return ret_ptr;
}

/**
 * @brief  配列要素をy方向線分毎に連続的にスキャン (返り値はfloat型)
 */
float *mdarray::scan_along_y_f( 
			 size_t *n, ssize_t *x, ssize_t *y, ssize_t *z ) const
{
    return this->scan_along_y_f_via_udf( NULL, NULL, n, x, y, z );
}

/**
 * @brief  配列要素をy方向線分毎に連続的にスキャン (返り値はfloat型)
 */
float *mdarray::scan_along_y_f_via_udf( 
		       void (*func)(const void *, void *, size_t, int, void *),
		       void *user_ptr,
		       size_t *n, ssize_t *x, ssize_t *y, ssize_t *z ) const
{
    float *ret_ptr = NULL;
    mdarray *thisp = (mdarray *)this;			/* 強制的にキャスト */
    size_t idx_trans_buf;
    size_t buf_blen;

    if ( thisp->scany_ndim == 0 ) goto quit;
    if ( thisp->scany_len[1] == 0 ) goto quit;

    if ( thisp->scany_len[0] <= thisp->scany_cntr[0] ) {
	/* 次のレイヤへ */
	if ( this->scan_along_y_goto_next() != 0 ) goto quit;	/* finish */
    }

    if ( func == NULL ) {
	/* 関数を選択 */
	func = this->func_cnv_nd_x2f;
	user_ptr = (void *)&Null_cnv_nd_prms;
	if ( func == NULL ) {
	    err_report1(__FUNCTION__,"ERROR","unsupported size_type: %zd",
			this->size_type());
	    goto quit;
	}
    }

    buf_blen = sizeof(float) * thisp->scany_len[1];
    if ( thisp->scany_ret_buf_blen < buf_blen ) {
	/* 返り値用バッファを確保 */
	if ( thisp->scany_ret_buf.allocate_aligned32(
			     buf_blen, &(thisp->scany_aligned_ret_ptr)) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","malloc() failed");
	}
	thisp->scany_ret_buf_blen = buf_blen;
    }

    idx_trans_buf = (thisp->scany_cntr[0] % thisp->scany_height_trans);
    if ( idx_trans_buf == 0 ) {
	/* 部分的に transpose を行なう */
	this->scan_along_y_do_partial_transpose();
    }

    /* 部分的に transpose されたバッファから float のバッファへコピー */
    (*func)(thisp->scany_trans_buf.ptr() 
	    + (this->bytes() * thisp->scany_len[1]) * idx_trans_buf,
	    thisp->scany_aligned_ret_ptr, thisp->scany_len[1], +1, user_ptr);
    ret_ptr = (float *)(thisp->scany_aligned_ret_ptr);

    if ( n != NULL ) *n = thisp->scany_len[1];
    if ( x != NULL ) *x = thisp->scany_begin[0] + thisp->scany_cntr[0];
    if ( y != NULL ) *y = thisp->scany_begin[1];
    if ( z != NULL ) *z = thisp->scany_ret_z;

    thisp->scany_cntr[0] ++;

 quit:
    return ret_ptr;
}

/* 注意: これを mdarray::update_length_info() に登録するのを忘れない事 */
/**
 * @brief  begin_scan_along_y() で開始したスキャン・シーケンスの終了
 */
void mdarray::end_scan_along_y() const
{
    mdarray *thisp = (mdarray *)this;			/* 強制的にキャスト */

    thisp->scany_begin = NULL;				/* 開放 & 初期化 */
    thisp->scany_len   = NULL;
    thisp->scany_cntr  = NULL;
    thisp->scany_ndim  = 0;

    thisp->scany_trans_buf = NULL;			/* 開放 & 初期化 */
    thisp->scany_trans_buf_blen = 0;
    thisp->scany_mtx_buf = NULL;
    thisp->scany_mtx_buf_blen = 0;
    thisp->scany_ret_buf = NULL;
    thisp->scany_ret_buf_blen = 0;

    return;
}


/* scan the specified section along z-axis.  A temporary buffer of 1-D */
/* array is prepared and scan_along_z() returns it.                    */
/* scan order is displayed by pseudocode:                              */
/*  for(...) {      <- row                                             */
/*    for(...) {    <- column                                          */
/*      for(...) {  <- layer                                           */
/**
 * @brief  配列要素のz方向線分毎でのスキャン・シーケンスの開始
 *
 * 自身の配列の指定された範囲の要素を，(x,y)の位置1つ1つについて，z方向にス
 * キャンするための準備をします．内部でブロック単位の高速transpose(データア
 * レイにおけるx軸とz軸との入れ換え)を行なうため，高いパフォーマンスが得ら
 * れます．<br>
 * z方向スキャンを行なう場合，次の手順で一連のAPIを使います．<br>
 * - beginf_scan_along_z() で準備をする．<br>
 * - scan_along_z() を必要な数だけ呼び出し，(x,y)ごとの処理を行なう．<br>
 * - end_scan_along_z() で終了処理(一時バッファの開放)を行なう．<br>
 * scan_along_z() は(x,y)の位置1つについてのz方向の要素が入った一時バッファの
 * アドレス(32バイトでアラインされている)を返します．この一時バッファはオブ
 * ジェクトの内部型が何であっても double 型で提供されます．なお，このバッファ
 * はオブジェクトで管理されているため，プログラマが開放してはいけません．<br>
 * scan_along_z() は指定範囲全てのスキャンが完了している場合には NULL を返しま
 * す．<br>
 * 画像データの統計をとる時などに使うメンバ関数です．z方向の大きさが十分でない
 * 場合，パフォーマンスが得られない事があります．その場合は，scan_zx_planes()
 * をお試しください．
 *
 * @param     exp_fmt スキャンする範囲を示す文字列(例: "0:99,*")のための
 *                    フォーマット指定(printfと同様)
 * @param     ... exp_fmtに対応した可変長引数の各要素データ
 * @return    スキャンによる有効要素の数．エラーの場合は負値．
 * @note      "[1:100,*]" のように [] で囲むと，1-indexed として扱われます．
 */
ssize_t mdarray::beginf_scan_along_z( const char *exp_fmt, ... ) const
{
    ssize_t ret;
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	ret = this->vbeginf_scan_along_z(exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vbeginf_scan_along_z() failed");
    }
    va_end(ap);
    return ret;
}

/**
 * @brief  配列要素のz方向線分毎でのスキャン・シーケンスの開始
 *
 * 自身の配列の指定された範囲の要素を，(x,y)の位置1つ1つについて，z方向にス
 * キャンするための準備をします．内部でブロック単位の高速transpose(データア
 * レイにおけるx軸とz軸との入れ換え)を行なうため，高いパフォーマンスが得ら
 * れます．<br>
 * z方向スキャンを行なう場合，次の手順で一連のAPIを使います．<br>
 * - vbeginf_scan_along_z() で準備をする．<br>
 * - scan_along_z() を必要な数だけ呼び出し，(x,y)ごとの処理を行なう．<br>
 * - end_scan_along_z() で終了処理(一時バッファの開放)を行なう．<br>
 * scan_along_z() は(x,y)の位置1つについてのz方向の要素が入った一時バッファの
 * アドレス(32バイトでアラインされている)を返します．この一時バッファはオブ
 * ジェクトの内部型が何であっても double 型で提供されます．なお，このバッファ
 * はオブジェクトで管理されているため，プログラマが開放してはいけません．<br>
 * scan_along_z() は指定範囲全てのスキャンが完了している場合には NULL を返しま
 * す．<br>
 * 画像データの統計をとる時などに使うメンバ関数です．z方向の大きさが十分でない
 * 場合，パフォーマンスが得られない事があります．その場合は，scan_zx_planes()
 * をお試しください．
 *
 * @param     exp_fmt スキャンする範囲を示す文字列(例: "0:99,*")のための
 *                    フォーマット指定(printfと同様)
 * @param     ap exp_fmtに対応した可変長引数のリスト
 * @return    スキャンによる有効要素の数．エラーの場合は負値．
 * @note      "[1:100,*]" のように [] で囲むと，1-indexed として扱われます．
 */
ssize_t mdarray::vbeginf_scan_along_z( const char *exp_fmt, va_list ap ) const
{
    ssize_t ret_len = -1;
    mdarray *thisp = (mdarray *)this;			/* 強制的にキャスト */
    heap_mem<char> secexp;
    heap_mem<mdarray_section_expinfo> secinfo;
    size_t n_secinfo = 0;
    size_t scan_ndim, i;

    if ( this->length() == 0 ) goto quit;

    if ( exp_fmt == NULL ) goto quit;

    if ( c_vasprintf(secexp.ptr_address(), exp_fmt, ap) < 0 ) {
	err_report(__FUNCTION__,"FATAL","c_vasprintf() failed");
	goto quit;
    }

    /* パースし，secinfo を生成 */
    if ( parse_section_exp(&secinfo, &n_secinfo, secexp.ptr()) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","Error in parse_section_exp()");
    }
    if ( n_secinfo == 0 ) goto quit;

    /* secinfo をチェック，修正 */
    if ( test_section_info( secexp.ptr(), *this, 
			    &secinfo, &n_secinfo, NULL ) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","Error in test_section_info()");
    }
    if ( n_secinfo == 0 ) goto quit;

    /* 最低でも3次元とする(返り値x,y,zのため) */
    if ( this->dim_length() < 3 ) scan_ndim = 3;
    else scan_ndim = this->dim_length();

    if ( thisp->scanz_ndim != scan_ndim ) {
	if ( thisp->scanz_begin.allocate( scan_ndim ) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","malloc() failed");
	}
	if ( thisp->scanz_len.allocate( scan_ndim ) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","malloc() failed");
	}
	if ( thisp->scanz_cntr.allocate( scan_ndim ) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","malloc() failed");
	}
	thisp->scanz_ndim = scan_ndim;
    }

    for ( i=0 ; i < n_secinfo ; i++ ) {
	thisp->scanz_begin[i] = secinfo[i].begin;
	thisp->scanz_len[i]   = secinfo[i].length;
    }
    for ( ; i < scan_ndim ; i++ ) {
	thisp->scanz_begin[i] = 0;
	thisp->scanz_len[i]   = this->length(i);
    }

    ret_len = this->begin_scan_along_z_common();

 quit:
    return ret_len;
}


/**
 * @brief  配列要素のz方向線分毎でのスキャン・シーケンスの開始
 *
 * 自身の配列の指定された範囲の要素を，(x,y)の位置1つ1つについて，z方向にス
 * キャンするための準備をします．内部でブロック単位の高速transpose(データア
 * レイにおけるx軸とz軸との入れ換え)を行なうため，高いパフォーマンスが得ら
 * れます．<br>
 * z方向スキャンを行なう場合，次の手順で一連のAPIを使います．<br>
 * - begin_scan_along_z() で準備をする．<br>
 * - scan_along_z() を必要な数だけ呼び出し，(x,y)ごとの処理を行なう．<br>
 * - end_scan_along_z() で終了処理(一時バッファの開放)を行なう．<br>
 * scan_along_z() は(x,y)の位置1つについてのz方向の要素が入った一時バッファの
 * アドレス(32バイトでアラインされている)を返します．この一時バッファはオブ
 * ジェクトの内部型が何であっても double 型で提供されます．なお，このバッファ
 * はオブジェクトで管理されているため，プログラマが開放してはいけません．<br>
 * scan_along_z() は指定範囲全てのスキャンが完了している場合には NULL を返しま
 * す．<br>
 * 画像データの統計をとる時などに使うメンバ関数です．z方向の大きさが十分でない
 * 場合，パフォーマンスが得られない事があります．その場合は，scan_zx_planes()
 * をお試しください．
 *
 * @param     col_index 列位置
 * @param     col_size 列サイズ
 * @param     row_index 行位置
 * @param     row_size 行サイズ
 * @param     layer_index レイヤ位置
 * @param     layer_size レイヤサイズ
 * @return    スキャンによる有効要素の数．エラーの場合は負値．
 */
ssize_t mdarray::begin_scan_along_z( 
				 ssize_t col_index, size_t col_size,
				 ssize_t row_index, size_t row_size,
				 ssize_t layer_index, size_t layer_size ) const
{
    ssize_t ret_len = -1;
    mdarray *thisp = (mdarray *)this;			/* 強制的にキャスト */

    if ( this->length() == 0 ) goto quit;

    if ( this->fix_section_args( &col_index, &col_size, 
				 &row_index, &row_size,
				 &layer_index, &layer_size ) < 0 ) goto quit;

    if ( thisp->scanz_ndim != 3 ) {
	if ( thisp->scanz_begin.allocate( 3 ) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","malloc() failed");
	}
	if ( thisp->scanz_len.allocate( 3 ) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","malloc() failed");
	}
	if ( thisp->scanz_cntr.allocate( 3 ) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","malloc() failed");
	}
	thisp->scanz_ndim = 3;
    }

    thisp->scanz_begin[0] = col_index;	/* スキャン領域の位置を保存 */
    thisp->scanz_begin[1] = row_index;
    thisp->scanz_begin[2] = layer_index;

    thisp->scanz_len[0] = col_size;	/* スキャン領域の大きさを保存 */
    thisp->scanz_len[1] = row_size;
    thisp->scanz_len[2] = layer_size;

    ret_len = this->begin_scan_along_z_common();

 quit:
    return ret_len;
}

/* private */
/**
 * @brief  *begin*_scan_along_z() の共通部分
 * @note   このメンバ関数は private です．
 */
ssize_t mdarray::begin_scan_along_z_common() const
{
    ssize_t ret_len = -1;
    const size_t this_bytes = this->bytes();
    const size_t col_blen = this_bytes * this->col_length();
    const size_t colrow_blen = col_blen * this->row_length();
    size_t col_size, layer_size;
    size_t len_block, min_len_block, max_len_block, min_size;
    size_t buf_blen;
    size_t blk3, i;
    mdarray *thisp = (mdarray *)this;			/* 強制的にキャスト */

    col_size = thisp->scanz_len[0];			/* スキャン領域 */
    layer_size = thisp->scanz_len[2];

    /* 正方行列バッファのバイト長が 16^2 から 128^2 の値をとるように設定 */
    /* 詳細は transpose_xy_copy() のコード中のコメントを参照             */
    min_len_block = (size_t)(1.0 + sqrt((double)(16*16) / this_bytes));
    max_len_block = (size_t)(1.0 + sqrt((double)(128*128) / this_bytes));
    if ( layer_size < col_size ) min_size = layer_size;
    else min_size = col_size;

    if ( min_size <= min_len_block ) len_block = min_len_block;
    else {
	size_t ndiv = ( (min_size - 1) / max_len_block ) + 1;
	len_block = min_size / ndiv;
    }

    /* 一時バッファの高さ (可能なら正方行列のサイズにあわせる) */
    if ( col_size < len_block ) thisp->scanz_height_trans = col_size;
    else thisp->scanz_height_trans = len_block;

    /* transpose用一時バッファ */
    buf_blen = this_bytes * layer_size * thisp->scanz_height_trans;
    if ( thisp->scanz_trans_buf_blen < buf_blen ) {
	if ( thisp->scanz_trans_buf.allocate( buf_blen ) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","malloc() failed");
	}
	thisp->scanz_trans_buf_blen = buf_blen;
    }

    /* transpose用正方行列の1辺の長さ */
    thisp->scanz_len_block = len_block;

    /* 正方行列用バッファ */
    buf_blen = this_bytes * len_block * len_block;
    if ( thisp->scanz_mtx_buf_blen < buf_blen ) {
	if ( thisp->scanz_mtx_buf.allocate( buf_blen ) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","malloc() failed");
	}
	thisp->scanz_mtx_buf_blen = buf_blen;
    }

    ret_len = 1;
    blk3 = 1;
    thisp->scanz_ret_z = 0;
    for ( i=0 ; i < thisp->scanz_ndim ; i++ ) {
	thisp->scanz_cntr[i] = 0;	/* カウンタのリセット([2]は未使用) */
	ret_len *= thisp->scanz_len[i];
	if ( 2 <= i ) {
	    thisp->scanz_ret_z += blk3 * thisp->scanz_begin[i];
	    blk3 *= this->length(i);
	}
    }

    /* 現在位置をセット */
    thisp->scanz_cptr = (const char *)(this->data_ptr_cs())
			+ colrow_blen * thisp->scanz_ret_z;	/* offset */

    /* transpose 用の汎用コピー関数を選択 */
    thisp->scanz_func_gencpy2d = NULL;
#define SEL_FUNC(fncname,src_sztp,src_tp,dest_sztp,dest_tp,fnc) \
    if ( this->size_type() == src_sztp && this->size_type() == dest_sztp ) { \
	thisp->scanz_func_gencpy2d = &gencpy2d::fncname; \
    }
    SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(SEL_FUNC,,,,,,,,,,,,,,,,,,,,,,,,,,,,else);
#undef SEL_FUNC

    return ret_len;
}

/* private */
/**
 * @brief  this->scanz_len[0] <= this->scanz_cntr[0] の時の，次のlineへいく時の処理
 * @note   このメンバ関数は private です．
 */
int mdarray::scan_along_z_goto_next() const
{
    int status = 1;
    mdarray *thisp = (mdarray *)this;			/* 強制的にキャスト */

    thisp->scanz_cntr[0] = 0;
    thisp->scanz_cntr[1] ++;

    if ( thisp->scanz_len[1] <= thisp->scanz_cntr[1] ) {
	size_t blen_block = this->bytes() * this->col_length();
	size_t len_b3 = 1;
	size_t dim_ix = 2;
	thisp->scanz_cntr[1] = 0;
	while ( 1 ) {
	    if ( thisp->scanz_ndim <= dim_ix + 1 ) {
		/* finish */
		thisp->scanz_len[2] = 0;
		goto quit;
	    }
	    /* ここのロジックは難しい…orz */
	    blen_block *= this->length(dim_ix - 1);
	    if ( dim_ix == 2 ) {
		thisp->scanz_cptr += blen_block * this->length(2);
		thisp->scanz_ret_z += this->length(2);
	    }
	    else {
		size_t stp = this->length(dim_ix) - thisp->scanz_len[dim_ix];
		thisp->scanz_cptr += blen_block * stp;
		len_b3 *= this->length(dim_ix - 1);
		thisp->scanz_ret_z += len_b3 * stp;
	    }
	    thisp->scanz_cntr[dim_ix] = 0;
	    dim_ix ++;
	    thisp->scanz_cntr[dim_ix] ++;
	    if ( thisp->scanz_cntr[dim_ix] < thisp->scanz_len[dim_ix] ) {
		break;
	    }
	}
    }

    status = 0;
 quit:
    return status;
}

/* private */
/**
 * @brief  部分的に transpose を行なう (これは x-z でのtranspose)
 * @note   このメンバ関数は private です．
 */
void mdarray::scan_along_z_do_partial_transpose() const
{
    mdarray *thisp = (mdarray *)this;			/* 強制的にキャスト */
    size_t cur_height_trans;
    size_t f_idx;
    if ( thisp->scanz_len[0] < 
	 thisp->scanz_cntr[0] + thisp->scanz_height_trans ) {
	/* this is the last */
	cur_height_trans = thisp->scanz_len[0] - thisp->scanz_cntr[0];
    }
    else {
	cur_height_trans = thisp->scanz_height_trans;
    }
    if ( this->rounding() == true ) f_idx = 1;	/* 四捨五入で変換 */
    else f_idx = 0;				/* 切り捨てで変換 */
    /* 部分的に transpose を行なう (これは x-z でのtranspose) */
    do_transpose_xy_copy( thisp->scanz_cptr, thisp->scanz_trans_buf.ptr(),
			  this->size_type(), this->bytes(),
			  this->size_type(), this->bytes(), 
			  thisp->scanz_begin[0] + thisp->scanz_cntr[0],
			  thisp->scanz_begin[1] + thisp->scanz_cntr[1],
			  this->col_length(), this->row_length(),
			  thisp->scanz_len[2], cur_height_trans,
			  thisp->scanz_len_block, 
			  thisp->scanz_mtx_buf.ptr(),
			  thisp->scanz_func_gencpy2d, &f_idx,
			  this->func_transp_mtx );
    return;
}

/**
 * @brief  配列要素をz方向線分毎に連続的にスキャン (返り値はdouble型)
 */
double *mdarray::scan_along_z( 
			  size_t *n, ssize_t *x, ssize_t *y, ssize_t *z ) const
{
    return this->scan_along_z_via_udf( NULL, NULL, n, x, y, z );
}

/**
 * @brief  配列要素をz方向線分毎に連続的にスキャン (返り値はdouble型)
 */
double *mdarray::scan_along_z_via_udf( 
		       void (*func)(const void *, void *, size_t, int, void *),
		       void *user_ptr,
		       size_t *n, ssize_t *x, ssize_t *y, ssize_t *z ) const
{
    double *ret_ptr = NULL;
    mdarray *thisp = (mdarray *)this;			/* 強制的にキャスト */
    size_t idx_trans_buf;
    size_t buf_blen;

    if ( thisp->scanz_ndim == 0 ) goto quit;
    if ( thisp->scanz_len[2] == 0 ) goto quit;

    if ( thisp->scanz_len[0] <= thisp->scanz_cntr[0] ) {
	/* 次のラインへ */
	if ( this->scan_along_z_goto_next() != 0 ) goto quit;	/* finish */
    }

    if ( func == NULL ) {
	/* 関数を選択 */
	func = this->func_cnv_nd_x2d;
	user_ptr = (void *)&Null_cnv_nd_prms;
	if ( func == NULL ) {
	    err_report1(__FUNCTION__,"ERROR","unsupported size_type: %zd",
			this->size_type());
	    goto quit;
	}
    }

    buf_blen = sizeof(double) * thisp->scanz_len[2];
    if ( thisp->scanz_ret_buf_blen < buf_blen ) {
	/* 返り値用バッファを確保 */
	if ( thisp->scanz_ret_buf.allocate_aligned32(
			     buf_blen, &(thisp->scanz_aligned_ret_ptr)) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","malloc() failed");
	}
	thisp->scanz_ret_buf_blen = buf_blen;
    }

    idx_trans_buf = (thisp->scanz_cntr[0] % thisp->scanz_height_trans);
    if ( idx_trans_buf == 0 ) {
	/* 部分的に transpose を行なう */
	this->scan_along_z_do_partial_transpose();
    }

    /* 部分的に transpose されたバッファから double のバッファへコピー */
    (*func)(thisp->scanz_trans_buf.ptr() 
	    + (this->bytes() * thisp->scanz_len[2]) * idx_trans_buf,
	    thisp->scanz_aligned_ret_ptr, thisp->scanz_len[2], +1, user_ptr);
    ret_ptr = (double *)(thisp->scanz_aligned_ret_ptr);

    if ( n != NULL ) *n = thisp->scanz_len[2];
    if ( x != NULL ) *x = thisp->scanz_begin[0] + thisp->scanz_cntr[0];
    if ( y != NULL ) *y = thisp->scanz_begin[1] + thisp->scanz_cntr[1];
    if ( z != NULL ) *z = thisp->scanz_ret_z;

    thisp->scanz_cntr[0] ++;

 quit:
    return ret_ptr;
}

/**
 * @brief  配列要素をz方向線分毎に連続的にスキャン (返り値はfloat型)
 */
float *mdarray::scan_along_z_f( 
			  size_t *n, ssize_t *x, ssize_t *y, ssize_t *z ) const
{
    return this->scan_along_z_f_via_udf( NULL, NULL, n, x, y, z );
}

/**
 * @brief  配列要素をz方向線分毎に連続的にスキャン (返り値はfloat型)
 */
float *mdarray::scan_along_z_f_via_udf( 
		       void (*func)(const void *, void *, size_t, int, void *),
		       void *user_ptr,
		       size_t *n, ssize_t *x, ssize_t *y, ssize_t *z ) const
{
    float *ret_ptr = NULL;
    mdarray *thisp = (mdarray *)this;			/* 強制的にキャスト */
    size_t idx_trans_buf;
    size_t buf_blen;

    if ( thisp->scanz_ndim == 0 ) goto quit;
    if ( thisp->scanz_len[2] == 0 ) goto quit;

    if ( thisp->scanz_len[0] <= thisp->scanz_cntr[0] ) {
	/* 次のラインへ */
	if ( this->scan_along_z_goto_next() != 0 ) goto quit;	/* finish */
    }

    if ( func == NULL ) {
	/* 関数を選択 */
	func = this->func_cnv_nd_x2f;
	user_ptr = (void *)&Null_cnv_nd_prms;
	if ( func == NULL ) {
	    err_report1(__FUNCTION__,"ERROR","unsupported size_type: %zd",
			this->size_type());
	    goto quit;
	}
    }

    buf_blen = sizeof(float) * thisp->scanz_len[2];
    if ( thisp->scanz_ret_buf_blen < buf_blen ) {
	/* 返り値用バッファを確保 */
	if ( thisp->scanz_ret_buf.allocate_aligned32(
			     buf_blen, &(thisp->scanz_aligned_ret_ptr)) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","malloc() failed");
	}
	thisp->scanz_ret_buf_blen = buf_blen;
    }

    idx_trans_buf = (thisp->scanz_cntr[0] % thisp->scanz_height_trans);
    if ( idx_trans_buf == 0 ) {
	/* 部分的に transpose を行なう */
	this->scan_along_z_do_partial_transpose();
    }

    /* 部分的に transpose されたバッファから float のバッファへコピー */
    (*func)(thisp->scanz_trans_buf.ptr() 
	    + (this->bytes() * thisp->scanz_len[2]) * idx_trans_buf,
	    thisp->scanz_aligned_ret_ptr, thisp->scanz_len[2], +1, user_ptr);
    ret_ptr = (float *)(thisp->scanz_aligned_ret_ptr);

    if ( n != NULL ) *n = thisp->scanz_len[2];
    if ( x != NULL ) *x = thisp->scanz_begin[0] + thisp->scanz_cntr[0];
    if ( y != NULL ) *y = thisp->scanz_begin[1] + thisp->scanz_cntr[1];
    if ( z != NULL ) *z = thisp->scanz_ret_z;

    thisp->scanz_cntr[0] ++;

 quit:
    return ret_ptr;
}

/* 注意: これを mdarray::update_length_info() に登録するのを忘れない事 */
/**
 * @brief  begin_scan_along_z() で開始したスキャン・シーケンスの終了
 */
void mdarray::end_scan_along_z() const
{
    mdarray *thisp = (mdarray *)this;			/* 強制的にキャスト */

    thisp->scanz_begin = NULL;				/* 開放 & 初期化 */
    thisp->scanz_len   = NULL;
    thisp->scanz_cntr  = NULL;
    thisp->scanz_ndim  = 0;

    thisp->scanz_trans_buf = NULL;			/* 開放 & 初期化 */
    thisp->scanz_trans_buf_blen = 0;
    thisp->scanz_mtx_buf = NULL;
    thisp->scanz_mtx_buf_blen = 0;
    thisp->scanz_ret_buf = NULL;
    thisp->scanz_ret_buf_blen = 0;

    return;
}


/* scan the specified 3-D section with plane by plane (zx plane). */
/* A temporary buffer of 2-D array is prepared.                   */
/* scan order is displayed by pseudocode:                         */
/*  for(...) {      <- row                                        */
/*    for(...) {    <- column                                     */
/*      for(...) {  <- layer                                      */
/**
 * @brief  配列要素のzx面単位でのスキャン・シーケンスの開始
 *
 * 自身の配列の指定された範囲の要素を，zx面単位でスキャンするための準備をしま
 * す．内部でブロック単位の高速transpose(データアレイにおけるx軸とz軸との入れ
 * 換え)を行なうため，高いパフォーマンスが得られます．<br>
 * zx面単位でスキャンを行なう場合，次の手順で一連のAPIを使います．<br>
 * - beginf_scan_zx_planes() で準備をする．<br>
 * - scan_zx_planes() を必要な数だけ呼び出し，面ごとの処理を行なう．<br>
 * - end_scan_zx_planes() で終了処理(一時バッファの開放)を行なう．<br>
 * scan_zx_planes() はyの位置1つについてのzx面の要素が入った一時バッファのアド
 * レス(32バイトでアラインされている)を返します．この一時バッファはオブジェクト
 * の内部型が何であっても double 型で提供されます．なお，このバッファはオブジェ
 * クトで管理されているため，プログラマが開放してはいけません．<br>
 * scan_zx_planes() は指定範囲全てのスキャンが完了している場合には NULL を返し
 * ます．<br>
 * 画像データの統計をとる時などに使うメンバ関数です．
 *
 * @param     exp_fmt スキャンする範囲を示す文字列(例: "0:99,*")のための
 *                    フォーマット指定(printfと同様)
 * @param     ... exp_fmtに対応した可変長引数の各要素データ
 * @return    スキャンによる有効要素の数．エラーの場合は負値．
 * @note      "[1:100,*]" のように [] で囲むと，1-indexed として扱われます．
 */
ssize_t mdarray::beginf_scan_zx_planes( const char *exp_fmt, ... ) const
{
    ssize_t ret;
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	ret = this->vbeginf_scan_zx_planes(exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vbeginf_scan_zx_planes() failed");
    }
    va_end(ap);
    return ret;
}

/**
 * @brief  配列要素のzx面単位でのスキャン・シーケンスの開始
 *
 * 自身の配列の指定された範囲の要素を，zx面単位でスキャンするための準備をしま
 * す．内部でブロック単位の高速transpose(データアレイにおけるx軸とz軸との入れ
 * 換え)を行なうため，高いパフォーマンスが得られます．<br>
 * zx面単位でスキャンを行なう場合，次の手順で一連のAPIを使います．<br>
 * - vbeginf_scan_zx_planes() で準備をする．<br>
 * - scan_zx_planes() を必要な数だけ呼び出し，面ごとの処理を行なう．<br>
 * - end_scan_zx_planes() で終了処理(一時バッファの開放)を行なう．<br>
 * scan_zx_planes() はyの位置1つについてのzx面の要素が入った一時バッファのアド
 * レス(32バイトでアラインされている)を返します．この一時バッファはオブジェクト
 * の内部型が何であっても double 型で提供されます．なお，このバッファはオブジェ
 * クトで管理されているため，プログラマが開放してはいけません．<br>
 * scan_zx_planes() は指定範囲全てのスキャンが完了している場合には NULL を返し
 * ます．<br>
 * 画像データの統計をとる時などに使うメンバ関数です．
 *
 * @param     exp_fmt スキャンする範囲を示す文字列(例: "0:99,*")のための
 *                    フォーマット指定(printfと同様)
 * @param     ap exp_fmtに対応した可変長引数のリスト
 * @return    スキャンによる有効要素の数．エラーの場合は負値．
 * @note      "[1:100,*]" のように [] で囲むと，1-indexed として扱われます．
 */
ssize_t mdarray::vbeginf_scan_zx_planes( const char *exp_fmt, va_list ap ) const
{
    ssize_t ret_len = -1;
    mdarray *thisp = (mdarray *)this;			/* 強制的にキャスト */
    heap_mem<char> secexp;
    heap_mem<mdarray_section_expinfo> secinfo;
    size_t n_secinfo = 0;
    size_t scan_ndim, i;

    if ( this->length() == 0 ) goto quit;

    if ( exp_fmt == NULL ) goto quit;

    if ( c_vasprintf(secexp.ptr_address(), exp_fmt, ap) < 0 ) {
	err_report(__FUNCTION__,"FATAL","c_vasprintf() failed");
	goto quit;
    }

    /* パースし，secinfo を生成 */
    if ( parse_section_exp(&secinfo, &n_secinfo, secexp.ptr()) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","Error in parse_section_exp()");
    }
    if ( n_secinfo == 0 ) goto quit;

    /* secinfo をチェック，修正 */
    if ( test_section_info( secexp.ptr(), *this, 
			    &secinfo, &n_secinfo, NULL ) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","Error in test_section_info()");
    }
    if ( n_secinfo == 0 ) goto quit;

    /* 最低でも3次元とする(返り値x,y,zのため) */
    if ( this->dim_length() < 3 ) scan_ndim = 3;
    else scan_ndim = this->dim_length();

    if ( thisp->scanzx_ndim != scan_ndim ) {
	if ( thisp->scanzx_begin.allocate( scan_ndim ) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","malloc() failed");
	}
	if ( thisp->scanzx_len.allocate( scan_ndim ) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","malloc() failed");
	}
	if ( thisp->scanzx_cntr.allocate( scan_ndim ) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","malloc() failed");
	}
	thisp->scanzx_ndim = scan_ndim;
    }

    for ( i=0 ; i < n_secinfo ; i++ ) {
	thisp->scanzx_begin[i] = secinfo[i].begin;
	thisp->scanzx_len[i]   = secinfo[i].length;
    }
    for ( ; i < scan_ndim ; i++ ) {
	thisp->scanzx_begin[i] = 0;
	thisp->scanzx_len[i]   = this->length(i);
    }

    ret_len = this->begin_scan_zx_planes_common();

 quit:
    return ret_len;
}

/**
 * @brief  配列要素のzx面単位でのスキャン・シーケンスの開始
 *
 * 自身の配列の指定された範囲の要素を，zx面単位でスキャンするための準備をしま
 * す．内部でブロック単位の高速transpose(データアレイにおけるx軸とz軸との入れ
 * 換え)を行なうため，高いパフォーマンスが得られます．<br>
 * zx面単位でスキャンを行なう場合，次の手順で一連のAPIを使います．<br>
 * - begin_scan_zx_planes() で準備をする．<br>
 * - scan_zx_planes() を必要な数だけ呼び出し，面ごとの処理を行なう．<br>
 * - end_scan_zx_planes() で終了処理(一時バッファの開放)を行なう．<br>
 * scan_zx_planes() はyの位置1つについてのzx面の要素が入った一時バッファのアド
 * レス(32バイトでアラインされている)を返します．この一時バッファはオブジェクト
 * の内部型が何であっても double 型で提供されます．なお，このバッファはオブジェ
 * クトで管理されているため，プログラマが開放してはいけません．<br>
 * scan_zx_planes() は指定範囲全てのスキャンが完了している場合には NULL を返し
 * ます．<br>
 * 画像データの統計をとる時などに使うメンバ関数です．
 *
 * @param     col_index 列位置
 * @param     col_size 列サイズ
 * @param     row_index 行位置
 * @param     row_size 行サイズ
 * @param     layer_index レイヤ位置
 * @param     layer_size レイヤサイズ
 * @return    スキャンによる有効要素の数．エラーの場合は負値．
 */
ssize_t mdarray::begin_scan_zx_planes( 
				 ssize_t col_index, size_t col_size,
				 ssize_t row_index, size_t row_size,
				 ssize_t layer_index, size_t layer_size ) const
{
    ssize_t ret_len = -1;
    mdarray *thisp = (mdarray *)this;			/* 強制的にキャスト */

    if ( this->length() == 0 ) goto quit;

    if ( this->fix_section_args( &col_index, &col_size, 
				 &row_index, &row_size,
				 &layer_index, &layer_size ) < 0 ) goto quit;

    if ( thisp->scanzx_ndim != 3 ) {
	if ( thisp->scanzx_begin.allocate( 3 ) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","malloc() failed");
	}
	if ( thisp->scanzx_len.allocate( 3 ) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","malloc() failed");
	}
	if ( thisp->scanzx_cntr.allocate( 3 ) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","malloc() failed");
	}
	thisp->scanzx_ndim = 3;
    }

    thisp->scanzx_begin[0] = col_index;	/* スキャン領域の位置を保存 */
    thisp->scanzx_begin[1] = row_index;
    thisp->scanzx_begin[2] = layer_index;

    thisp->scanzx_len[0] = col_size;	/* スキャン領域の大きさを保存 */
    thisp->scanzx_len[1] = row_size;
    thisp->scanzx_len[2] = layer_size;

    ret_len = this->begin_scan_zx_planes_common();

 quit:
    return ret_len;
}

/**
 * @brief  *begin*_scan_zx_planes() の共通部分
 * @note   このメンバ関数は private です．
 */
ssize_t mdarray::begin_scan_zx_planes_common() const
{
    ssize_t ret_len = -1;
    const size_t this_bytes = this->bytes();
    const size_t col_blen = this_bytes * this->col_length();
    const size_t colrow_blen = col_blen * this->row_length();
    size_t col_size, layer_size;
    size_t len_block, min_len_block, max_len_block, min_size;
    size_t buf_blen;
    size_t blk3, i;
    mdarray *thisp = (mdarray *)this;			/* 強制的にキャスト */

    col_size = thisp->scanzx_len[0];			/* スキャン領域 */
    layer_size = thisp->scanzx_len[2];

    /* 正方行列バッファのバイト長が 16^2 から 128^2 の値をとるように設定 */
    /* 詳細は transpose_xy_copy() のコード中のコメントを参照             */
    min_len_block = (size_t)(1.0 + sqrt((double)(16*16) / this_bytes));
    max_len_block = (size_t)(1.0 + sqrt((double)(128*128) / this_bytes));
    if ( layer_size < col_size ) min_size = layer_size;
    else min_size = col_size;

    if ( min_size <= min_len_block ) len_block = min_len_block;
    else {
	size_t ndiv = ( (min_size - 1) / max_len_block ) + 1;
	len_block = min_size / ndiv;
    }

    /* transpose用正方行列の1辺の長さ */
    thisp->scanzx_len_block = len_block;

    /* 正方行列用バッファ */
    buf_blen = this_bytes * len_block * len_block;
    if ( thisp->scanzx_mtx_buf_blen < buf_blen ) {
	if ( thisp->scanzx_mtx_buf.allocate( buf_blen ) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","malloc() failed");
	}
	thisp->scanzx_mtx_buf_blen = buf_blen;
    }

    ret_len = 1;
    blk3 = 1;
    thisp->scanzx_ret_z = 0;
    for ( i=0 ; i < thisp->scanzx_ndim ; i++ ) {
	thisp->scanzx_cntr[i] = 0;	/* カウンタのリセット([0,2]は未使用) */
	ret_len *= thisp->scanzx_len[i];
	if ( 2 <= i ) {
	    thisp->scanzx_ret_z += blk3 * thisp->scanzx_begin[i];
	    blk3 *= this->length(i);
	}
    }

    /* 現在位置をセット */
    thisp->scanzx_cptr = (const char *)(this->data_ptr_cs())
			 + colrow_blen * thisp->scanzx_ret_z;	/* offset */

    return ret_len;
}

/* private */
/**
 * @brief  this->scanzx_len[1] <= this->scanzx_cntr[1] の時の，次のzx面へいく時の処理
 * @note   このメンバ関数は private です．
 */
int mdarray::scan_zx_goto_next() const
{
    int status = 1;
    mdarray *thisp = (mdarray *)this;			/* 強制的にキャスト */
    size_t blen_block = this->bytes() * this->col_length();
    size_t len_b3 = 1;
    size_t dim_ix = 2;
    thisp->scanzx_cntr[1] = 0;
    while ( 1 ) {
	if ( thisp->scanzx_ndim <= dim_ix + 1 ) {
	    /* finish */
	    thisp->scanzx_len[2] = 0;
	    goto quit;
	}
	/* ここのロジックは難しい…orz */
	blen_block *= this->length(dim_ix - 1);
	if ( dim_ix == 2 ) {
	    thisp->scanzx_cptr += blen_block * this->length(2);
	    thisp->scanzx_ret_z += this->length(2);
	}
	else {
	    size_t stp = this->length(dim_ix) - thisp->scanzx_len[dim_ix];
	    thisp->scanzx_cptr += blen_block * stp;
	    len_b3 *= this->length(dim_ix - 1);
	    thisp->scanzx_ret_z += len_b3 * stp;
	}
	thisp->scanzx_cntr[dim_ix] = 0;
	dim_ix ++;
	thisp->scanzx_cntr[dim_ix] ++;
	if ( thisp->scanzx_cntr[dim_ix] < thisp->scanzx_len[dim_ix] ) {
	    break;
	}
    }
    status = 0;
 quit:
    return status;
}

/**
 * @brief  配列要素をzx面単位で連続的にスキャン (返り値はdouble型)
 */
double *mdarray::scan_zx_planes( 
	   size_t *n_z, size_t *n_x, ssize_t *x, ssize_t *y, ssize_t *z ) const
{
    return this->scan_zx_planes_via_udf( NULL, NULL, n_z, n_x, x, y, z );
}

/**
 * @brief  配列要素をzx面単位で連続的にスキャン (返り値はdouble型)
 */
double *mdarray::scan_zx_planes_via_udf( 
	   void (*func_gencpy2d)(const void *, void *, 
		       size_t, size_t, size_t, size_t, size_t, size_t, void *),
	   void *user_ptr,
	   size_t *n_z, size_t *n_x, ssize_t *x, ssize_t *y, ssize_t *z ) const
{
    double *ret_ptr = NULL;
    mdarray *thisp = (mdarray *)this;			/* 強制的にキャスト */
    size_t buf_blen;
    size_t f_idx;

    if ( thisp->scanzx_ndim == 0 ) goto quit;
    if ( thisp->scanzx_len[2] == 0 ) goto quit;

    if ( thisp->scanzx_len[1] <= thisp->scanzx_cntr[1] ) {
	/* 次の zx 面へ */
	if ( this->scan_zx_goto_next() != 0 ) goto quit;	/* finish */
    }

    buf_blen = sizeof(double) * thisp->scanzx_len[2] * thisp->scanzx_len[0];
    if ( thisp->scanzx_ret_buf_blen < buf_blen ) {
	/* 返り値用バッファを確保 */
	if ( thisp->scanzx_ret_buf.allocate_aligned32(
			    buf_blen, &(thisp->scanzx_aligned_ret_ptr)) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","malloc() failed");
	}
	thisp->scanzx_ret_buf_blen = buf_blen;
    }

    if ( func_gencpy2d == NULL ) {
	if ( this->rounding() == true ) f_idx = 1;	/* 四捨五入で変換 */
	else f_idx = 0;					/* 切り捨てで変換 */
	user_ptr = &f_idx;
	/* transpose 用の汎用コピー関数を選択                             */
	/* (一時バッファ経由でなく，直接 double のバッファに入れてしまう) */
#define SEL_FUNC(fncname,src_sztp,src_tp,dest_sztp,dest_tp,fnc) \
	if ( this->size_type() == src_sztp && DOUBLE_ZT == dest_sztp ) { \
	    func_gencpy2d = &gencpy2d::fncname; \
	}
	SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(SEL_FUNC,,,,,,,,,,,,,,,,,,,,,,,,,,,,else);
#undef SEL_FUNC
	if ( func_gencpy2d == NULL ) {
	    err_report1(__FUNCTION__,"ERROR","unsupported size_type: %zd",
			this->size_type());
	    goto quit;
	}
    }

    do_transpose_xy_copy( thisp->scanzx_cptr, thisp->scanzx_aligned_ret_ptr,
			  this->size_type(), this->bytes(),
			  DOUBLE_ZT, sizeof(double), 
			  thisp->scanzx_begin[0] + 0,
			  thisp->scanzx_begin[1] + thisp->scanzx_cntr[1],
			  this->col_length(), this->row_length(),
			  thisp->scanzx_len[2], thisp->scanzx_len[0],
			  thisp->scanzx_len_block,
			  thisp->scanzx_mtx_buf.ptr(),
			  func_gencpy2d, user_ptr,
			  this->func_transp_mtx );

    ret_ptr = (double *)(thisp->scanzx_aligned_ret_ptr);

    if ( n_z != NULL ) *n_z = thisp->scanzx_len[2];
    if ( n_x != NULL ) *n_x = thisp->scanzx_len[0];
    if ( x != NULL ) *x = thisp->scanzx_begin[0];
    if ( y != NULL ) *y = thisp->scanzx_begin[1] + thisp->scanzx_cntr[1];
    if ( z != NULL ) *z = thisp->scanzx_ret_z;

    thisp->scanzx_cntr[1] ++;

 quit:
    return ret_ptr;
}

/**
 * @brief  配列要素をzx面単位で連続的にスキャン (返り値はfloat型)
 */
float *mdarray::scan_zx_planes_f( 
	   size_t *n_z, size_t *n_x, ssize_t *x, ssize_t *y, ssize_t *z ) const
{
    return this->scan_zx_planes_f_via_udf( NULL, NULL, n_z, n_x, x, y, z );
}

/**
 * @brief  配列要素をzx面単位で連続的にスキャン (返り値はfloat型)
 */
float *mdarray::scan_zx_planes_f_via_udf( 
	   void (*func_gencpy2d)(const void *, void *, 
		       size_t, size_t, size_t, size_t, size_t, size_t, void *),
	   void *user_ptr,
	   size_t *n_z, size_t *n_x, ssize_t *x, ssize_t *y, ssize_t *z ) const
{
    float *ret_ptr = NULL;
    mdarray *thisp = (mdarray *)this;			/* 強制的にキャスト */
    size_t buf_blen;
    size_t f_idx;

    if ( thisp->scanzx_ndim == 0 ) goto quit;
    if ( thisp->scanzx_len[2] == 0 ) goto quit;

    if ( thisp->scanzx_len[1] <= thisp->scanzx_cntr[1] ) {
	/* 次の zx 面へ */
	if ( this->scan_zx_goto_next() != 0 ) goto quit;	/* finish */
    }

    buf_blen = sizeof(float) * thisp->scanzx_len[2] * thisp->scanzx_len[0];
    if ( thisp->scanzx_ret_buf_blen < buf_blen ) {
	/* 返り値用バッファを確保 */
	if ( thisp->scanzx_ret_buf.allocate_aligned32(
			    buf_blen, &(thisp->scanzx_aligned_ret_ptr)) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","malloc() failed");
	}
	thisp->scanzx_ret_buf_blen = buf_blen;
    }

    if ( func_gencpy2d == NULL ) {
	if ( this->rounding() == true ) f_idx = 1;	/* 四捨五入で変換 */
	else f_idx = 0;					/* 切り捨てで変換 */
	user_ptr = &f_idx;
	/* transpose 用の汎用コピー関数を選択                             */
	/* (一時バッファ経由でなく，直接 float のバッファに入れてしまう) */
#define SEL_FUNC(fncname,src_sztp,src_tp,dest_sztp,dest_tp,fnc) \
	if ( this->size_type() == src_sztp && FLOAT_ZT == dest_sztp ) { \
	    func_gencpy2d = &gencpy2d::fncname; \
	}
	SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(SEL_FUNC,,,,,,,,,,,,,,,,,,,,,,,,,,,,else);
#undef SEL_FUNC
	if ( func_gencpy2d == NULL ) {
	    err_report1(__FUNCTION__,"ERROR","unsupported size_type: %zd",
			this->size_type());
	    goto quit;
	}
    }

    do_transpose_xy_copy( thisp->scanzx_cptr, thisp->scanzx_aligned_ret_ptr,
			  this->size_type(), this->bytes(),
			  FLOAT_ZT, sizeof(float), 
			  thisp->scanzx_begin[0] + 0,
			  thisp->scanzx_begin[1] + thisp->scanzx_cntr[1],
			  this->col_length(), this->row_length(),
			  thisp->scanzx_len[2], thisp->scanzx_len[0],
			  thisp->scanzx_len_block,
			  thisp->scanzx_mtx_buf.ptr(),
			  func_gencpy2d, user_ptr,
			  this->func_transp_mtx );

    ret_ptr = (float *)(thisp->scanzx_aligned_ret_ptr);

    if ( n_z != NULL ) *n_z = thisp->scanzx_len[2];
    if ( n_x != NULL ) *n_x = thisp->scanzx_len[0];
    if ( x != NULL ) *x = thisp->scanzx_begin[0];
    if ( y != NULL ) *y = thisp->scanzx_begin[1] + thisp->scanzx_cntr[1];
    if ( z != NULL ) *z = thisp->scanzx_ret_z;

    thisp->scanzx_cntr[1] ++;

 quit:
    return ret_ptr;
}

/* 注意: これを mdarray::update_length_info() に登録するのを忘れない事 */
/**
 * @brief  begin_scan_zx_planes() で開始したスキャン・シーケンスの終了
 */
void mdarray::end_scan_zx_planes() const
{
    mdarray *thisp = (mdarray *)this;			/* 強制的にキャスト */

    thisp->scanzx_begin = NULL;				/* 開放 & 初期化 */
    thisp->scanzx_len   = NULL;
    thisp->scanzx_cntr  = NULL;
    thisp->scanzx_ndim  = 0;

    thisp->scanzx_mtx_buf = NULL;			/* 開放 & 初期化 */
    thisp->scanzx_mtx_buf_blen = 0;
    thisp->scanzx_ret_buf = NULL;
    thisp->scanzx_ret_buf_blen = 0;

    return;
}


/* scan the specified 3-D section.  A temporary buffer of 1-D array is */
/* prepared and scan_a_cube() returns it.                              */
/*  for(...) {      <- layer                                           */
/*    for(...) {    <- row                                             */
/*      for(...) {  <- column                                          */
/**
 * @brief  3次元の範囲の要素のためのスキャン・シーケンスの開始
 *
 * 自身の配列の指定された3次元の範囲の要素を，1度にスキャンするための準備をし
 * ます．3次元一括スキャンを行なう場合，次の手順で一連のAPIを使います．<br>
 * - beginf_scan_a_cube() で準備をする．<br>
 * - scan_a_cube() を必要な数だけ呼び出し，キューブごとの処理を行なう．<br>
 * - end_scan_a_cube() で終了処理(一時バッファの開放)を行なう．<br>
 * scan_a_cube() は指定部分の要素が入った一時バッファのアドレス(32バイトでアラ
 * インされている)を返します．この一時バッファは，scan_a_cube() の場合はオブ
 * ジェクトの内部型が何であっても double 型で，scan_a_cube_f() の場合は float
 * 型で提供されます．なお，このバッファはオブジェクトで管理されているため，プ
 * ログラマが開放してはいけません．<br>
 * 画像データの統計をとる時などに使うメンバ関数です．
 *
 * @param     exp_fmt スキャンする範囲を示す文字列(例: "0:99,*")のための
 *                    フォーマット指定(printfと同様)
 * @param     ... exp_fmtに対応した可変長引数の各要素データ
 * @return    スキャンによる有効要素の数．エラーの場合は負値．
 * @note      "[1:100,*]" のように [] で囲むと，1-indexed として扱われます．
 */
ssize_t mdarray::beginf_scan_a_cube( const char *exp_fmt, ... ) const
{
    ssize_t ret;
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	ret = this->vbeginf_scan_a_cube(exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vbeginf_scan_a_cube() failed");
    }
    va_end(ap);
    return ret;
}

/**
 * @brief  3次元の範囲の要素のためのスキャン・シーケンスの開始
 *
 * 自身の配列の指定された3次元の範囲の要素を，1度にスキャンするための準備をし
 * ます．3次元一括スキャンを行なう場合，次の手順で一連のAPIを使います．<br>
 * - beginf_scan_a_cube() で準備をする．<br>
 * - scan_a_cube() を必要な数だけ呼び出し，キューブごとの処理を行なう．<br>
 * - end_scan_a_cube() で終了処理(一時バッファの開放)を行なう．<br>
 * scan_a_cube() は指定部分の要素が入った一時バッファのアドレス(32バイトでアラ
 * インされている)を返します．この一時バッファは，scan_a_cube() の場合はオブ
 * ジェクトの内部型が何であっても double 型で，scan_a_cube_f() の場合は float
 * 型で提供されます．なお，このバッファはオブジェクトで管理されているため，プ
 * ログラマが開放してはいけません．<br>
 * 画像データの統計をとる時などに使うメンバ関数です．
 *
 * @param     exp_fmt スキャンする範囲を示す文字列(例: "0:99,*")のための
 *                    フォーマット指定(printfと同様)
 * @param     ap exp_fmtに対応した可変長引数のリスト
 * @return    スキャンによる有効要素の数．エラーの場合は負値．
 * @note      "[1:100,*]" のように [] で囲むと，1-indexed として扱われます．
 */
ssize_t mdarray::vbeginf_scan_a_cube( const char *exp_fmt, va_list ap ) const
{
    ssize_t ret_len = -1;
    mdarray *thisp = (mdarray *)this;			/* 強制的にキャスト */
    heap_mem<char> secexp;
    heap_mem<mdarray_section_expinfo> secinfo;
    size_t n_secinfo = 0;
    size_t scan_ndim, blk3, i;

    if ( this->length() == 0 ) goto quit;

    if ( exp_fmt == NULL ) goto quit;

    if ( c_vasprintf(secexp.ptr_address(), exp_fmt, ap) < 0 ) {
	err_report(__FUNCTION__,"FATAL","c_vasprintf() failed");
	goto quit;
    }

    /* パースし，secinfo を生成 */
    if ( parse_section_exp(&secinfo, &n_secinfo, secexp.ptr()) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","Error in parse_section_exp()");
    }
    if ( n_secinfo == 0 ) goto quit;

    /* secinfo をチェック，修正 */
    if ( test_section_info( secexp.ptr(), *this, 
			    &secinfo, &n_secinfo, NULL ) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","Error in test_section_info()");
    }
    if ( n_secinfo == 0 ) goto quit;

    /* 最低でも3次元とする(返り値x,y,zのため) */
    if ( this->dim_length() < 3 ) scan_ndim = 3;
    else scan_ndim = this->dim_length();

    if ( thisp->scancube_ndim != scan_ndim ) {
	if ( thisp->scancube_begin.allocate( scan_ndim ) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","malloc() failed");
	}
	if ( thisp->scancube_len.allocate( scan_ndim ) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","malloc() failed");
	}
	if ( thisp->scancube_cntr.allocate( scan_ndim ) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","malloc() failed");
	}
	thisp->scancube_ndim = scan_ndim;
    }

    for ( i=0 ; i < n_secinfo ; i++ ) {
	thisp->scancube_begin[i] = secinfo[i].begin;
	thisp->scancube_len[i]   = secinfo[i].length;
    }
    for ( ; i < scan_ndim ; i++ ) {
	thisp->scancube_begin[i] = 0;
	thisp->scancube_len[i]   = this->length(i);
    }

    blk3 = 1;
    ret_len = 1;
    thisp->scancube_ret_z = 0;
    for ( i=0 ; i < scan_ndim ; i++ ) {
	thisp->scancube_cntr[i]  = 0;
	ret_len *= thisp->scancube_len[i];
	if ( 2 <= i ) {
	    thisp->scancube_ret_z += blk3 * thisp->scancube_begin[i];
	    blk3 *= this->length(i);
	}
    }

 quit:
    return ret_len;
}

/**
 * @brief  3次元の範囲の要素のためのスキャン・シーケンスの開始
 *
 * 自身の配列の指定された3次元の範囲の要素を，1度にスキャンするための準備をし
 * ます．3次元一括スキャンを行なう場合，次の手順で一連のAPIを使います．<br>
 * - begin_scan_a_cube() で準備をする．<br>
 * - scan_a_cube() を必要な数だけ呼び出し，キューブごとの処理を行なう．<br>
 * - end_scan_a_cube() で終了処理(一時バッファの開放)を行なう．<br>
 * scan_a_cube() は指定部分の要素が入った一時バッファのアドレス(32バイトでアラ
 * インされている)を返します．この一時バッファは，scan_a_cube() の場合はオブ
 * ジェクトの内部型が何であっても double 型で，scan_a_cube_f() の場合は float
 * 型で提供されます．なお，このバッファはオブジェクトで管理されているため，プ
 * ログラマが開放してはいけません．<br>
 * 画像データの統計をとる時などに使うメンバ関数です．
 *
 * @param     col_index 列位置
 * @param     col_size 列サイズ
 * @param     row_index 行位置
 * @param     row_size 行サイズ
 * @param     layer_index レイヤ位置
 * @param     layer_size レイヤサイズ
 * @return    スキャンによる有効要素の数．エラーの場合は負値．
 */
ssize_t mdarray::begin_scan_a_cube( 
				ssize_t col_index, size_t col_size,
				ssize_t row_index, size_t row_size,
				ssize_t layer_index, size_t layer_size ) const
{
    ssize_t ret_len = -1;
    mdarray *thisp = (mdarray *)this;			/* 強制的にキャスト */

    if ( this->length() == 0 ) goto quit;

    if ( this->fix_section_args( &col_index, &col_size, 
				 &row_index, &row_size,
				 &layer_index, &layer_size ) < 0 ) goto quit;

    if ( thisp->scancube_ndim != 3 ) {
	if ( thisp->scancube_begin.allocate( 3 ) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","malloc() failed");
	}
	if ( thisp->scancube_len.allocate( 3 ) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","malloc() failed");
	}
	if ( thisp->scancube_cntr.allocate( 3 ) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","malloc() failed");
	}
	thisp->scancube_ndim = 3;
    }

    ret_len = col_size * row_size * layer_size;

    thisp->scancube_begin[0] = col_index;	/* スキャン領域の位置を保存 */
    thisp->scancube_begin[1] = row_index;
    thisp->scancube_begin[2] = layer_index;

    thisp->scancube_len[0] = col_size;        /* スキャン領域の大きさを保存 */
    thisp->scancube_len[1] = row_size;
    thisp->scancube_len[2] = layer_size;

    thisp->scancube_cntr[0] = 0;	      /* カウンタのリセット(未使用) */
    thisp->scancube_cntr[1] = 0;
    thisp->scancube_cntr[2] = 0;

    thisp->scancube_ret_z = layer_index;

 quit:
    return ret_len;
}

/* private */
/**
 * @brief  this->scancube_len[3] <= this->scancube_cntr[3] の時の，次のcubeの為の処理
 * @note   このメンバ関数は private です．
 */
int mdarray::scan_a_cube_goto_next() const
{
    int status = 1;
    mdarray *thisp = (mdarray *)this;			/* 強制的にキャスト */

    size_t len_b3 = 1;
    size_t dim_ix = 3;
    while ( 1 ) {
	size_t stp;
	if ( thisp->scancube_ndim <= dim_ix + 1 ) {
	    /* finish */
	    thisp->scancube_len[0] = 0;
	    goto quit;
	}
	/* ここのロジックは難しい…orz */
	stp = this->length(dim_ix) - thisp->scancube_len[dim_ix];
	len_b3 *= this->length(dim_ix - 1);
	thisp->scancube_ret_z += len_b3 * stp;
	thisp->scancube_cntr[dim_ix] = 0;
	dim_ix ++;
	thisp->scancube_cntr[dim_ix] ++;
	if ( thisp->scancube_cntr[dim_ix] < thisp->scancube_len[dim_ix] ) {
	    break;
	}
    }

    status = 0;
 quit:
    return status;
}

/**
 * @brief  3次元の範囲の要素を連続的にスキャン (返り値はdouble型)
 */
double *mdarray::scan_a_cube( size_t *n_x, size_t *n_y, size_t *n_z, 
			      ssize_t *x, ssize_t *y, ssize_t *z ) const
{
    return this->scan_a_cube_via_udf( NULL, NULL, n_x, n_y, n_z, x, y, z );
}

/**
 * @brief  3次元の範囲の要素を連続的にスキャン (返り値はdouble型)
 */
double *mdarray::scan_a_cube_via_udf( 
		       void (*func)(const void *, void *, size_t, int, void *),
		       void *user_ptr,
		       size_t *n_x, size_t *n_y, size_t *n_z, 
		       ssize_t *x, ssize_t *y, ssize_t *z ) const
{
    double *ret_ptr = NULL;
    const size_t this_bytes = this->bytes();
    const size_t col_blen = this_bytes * this->col_length();
    const size_t colrow_blen = col_blen * this->row_length();
    mdarray *thisp = (mdarray *)this;			/* 強制的にキャスト */
    const char *s_ptr0;
    double *d_ptr;
    size_t len_elements;
    size_t buf_blen;
    size_t i, j;

    if ( thisp->scancube_ndim == 0 ) goto quit;
    if ( thisp->scancube_len[0] == 0 ) goto quit;

    if ( 4 <= thisp->scancube_ndim ) {
	if ( thisp->scancube_len[3] <= thisp->scancube_cntr[3] ) {
	    /* 次の cube へ */
	    if ( this->scan_a_cube_goto_next() != 0 ) goto quit;   /* finish */
	}
    }

    if ( func == NULL ) {
	/* 関数を選択 */
	func = this->func_cnv_nd_x2d;
	user_ptr = (void *)&Null_cnv_nd_prms;
	if ( func == NULL ) {
	    err_report1(__FUNCTION__,"ERROR","unsupported size_type: %zd",
			this->size_type());
	    goto quit;
	}
    }

    len_elements = thisp->scancube_len[0] * thisp->scancube_len[1]
		   * thisp->scancube_len[2];

    buf_blen = sizeof(double) * len_elements;
    if ( thisp->scancube_ret_buf_blen < buf_blen ) {
	/* 返り値用バッファを確保 */
	if ( thisp->scancube_ret_buf.allocate_aligned32(
			  buf_blen, &(thisp->scancube_aligned_ret_ptr)) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","malloc() failed");
	}
	thisp->scancube_ret_buf_blen = buf_blen;
    }

    ret_ptr = (double *)(thisp->scancube_aligned_ret_ptr);

    s_ptr0 = (const char *)(this->data_ptr_cs())
	     + colrow_blen * thisp->scancube_ret_z		/* offset */
	     + col_blen * thisp->scancube_begin[1]
	     + this_bytes * thisp->scancube_begin[0];
    d_ptr = ret_ptr;

    for ( i=0 ; i < thisp->scancube_len[2] ; i++ ) {
	const char *s_ptr1 = s_ptr0;
	for ( j=0 ; j < thisp->scancube_len[1] ; j++ ) {
	    (*func)(s_ptr1, d_ptr, thisp->scancube_len[0], +1, user_ptr);
	    s_ptr1 += col_blen;
	    d_ptr += thisp->scancube_len[0];
	}
	s_ptr0 += colrow_blen;
    }

    if ( n_x != NULL ) *n_x = thisp->scancube_len[0];
    if ( n_y != NULL ) *n_y = thisp->scancube_len[1];
    if ( n_z != NULL ) *n_z = thisp->scancube_len[2];
    if ( x != NULL ) *x = thisp->scancube_begin[0];
    if ( y != NULL ) *y = thisp->scancube_begin[1];
    if ( z != NULL ) *z = thisp->scancube_ret_z;

    if ( 4 <= thisp->scancube_ndim ) {
	thisp->scancube_cntr[3] ++;
	thisp->scancube_ret_z += this->length(2);
    }
    else thisp->scancube_len[0] = 0;		/* 1回きりなのでリセット */

 quit:
    return ret_ptr;
}

/**
 * @brief  3次元の範囲の要素を連続的にスキャン (返り値はfloat型)
 */
float *mdarray::scan_a_cube_f( size_t *n_x, size_t *n_y, size_t *n_z, 
			       ssize_t *x, ssize_t *y, ssize_t *z ) const
{
    return this->scan_a_cube_f_via_udf( NULL, NULL, n_x, n_y, n_z, x, y, z );
}

/**
 * @brief  3次元の範囲の要素を連続的にスキャン (返り値はfloat型)
 */
float *mdarray::scan_a_cube_f_via_udf( 
		       void (*func)(const void *, void *, size_t, int, void *),
		       void *user_ptr,
		       size_t *n_x, size_t *n_y, size_t *n_z, 
		       ssize_t *x, ssize_t *y, ssize_t *z ) const
{
    float *ret_ptr = NULL;
    const size_t this_bytes = this->bytes();
    const size_t col_blen = this_bytes * this->col_length();
    const size_t colrow_blen = col_blen * this->row_length();
    mdarray *thisp = (mdarray *)this;			/* 強制的にキャスト */
    const char *s_ptr0;
    float *d_ptr;
    size_t len_elements;
    size_t buf_blen;
    size_t i, j;

    if ( thisp->scancube_ndim == 0 ) goto quit;
    if ( thisp->scancube_len[0] == 0 ) goto quit;

    if ( 4 <= thisp->scancube_ndim ) {
	if ( thisp->scancube_len[3] <= thisp->scancube_cntr[3] ) {
	    /* 次の cube へ */
	    if ( this->scan_a_cube_goto_next() != 0 ) goto quit;   /* finish */
	}
    }

    if ( func == NULL ) {
	/* 関数を選択 */
	func = this->func_cnv_nd_x2f;
	user_ptr = (void *)&Null_cnv_nd_prms;
	if ( func == NULL ) {
	    err_report1(__FUNCTION__,"ERROR","unsupported size_type: %zd",
			this->size_type());
	    goto quit;
	}
    }

    len_elements = thisp->scancube_len[0] * thisp->scancube_len[1]
		   * thisp->scancube_len[2];

    buf_blen = sizeof(float) * len_elements;
    if ( thisp->scancube_ret_buf_blen < buf_blen ) {
	/* 返り値用バッファを確保 */
	if ( thisp->scancube_ret_buf.allocate_aligned32(
			  buf_blen, &(thisp->scancube_aligned_ret_ptr)) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","malloc() failed");
	}
	thisp->scancube_ret_buf_blen = buf_blen;
    }

    ret_ptr = (float *)(thisp->scancube_aligned_ret_ptr);

    s_ptr0 = (const char *)(this->data_ptr_cs())
	     + colrow_blen * thisp->scancube_ret_z		/* offset */
	     + col_blen * thisp->scancube_begin[1]
	     + this_bytes * thisp->scancube_begin[0];
    d_ptr = ret_ptr;

    for ( i=0 ; i < thisp->scancube_len[2] ; i++ ) {
	const char *s_ptr1 = s_ptr0;
	for ( j=0 ; j < thisp->scancube_len[1] ; j++ ) {
	    (*func)(s_ptr1, d_ptr, thisp->scancube_len[0], +1, user_ptr);
	    s_ptr1 += col_blen;
	    d_ptr += thisp->scancube_len[0];
	}
	s_ptr0 += colrow_blen;
    }

    if ( n_x != NULL ) *n_x = thisp->scancube_len[0];
    if ( n_y != NULL ) *n_y = thisp->scancube_len[1];
    if ( n_z != NULL ) *n_z = thisp->scancube_len[2];
    if ( x != NULL ) *x = thisp->scancube_begin[0];
    if ( y != NULL ) *y = thisp->scancube_begin[1];
    if ( z != NULL ) *z = thisp->scancube_ret_z;

    if ( 4 <= thisp->scancube_ndim ) {
	thisp->scancube_cntr[3] ++;
	thisp->scancube_ret_z += this->length(2);
    }
    else thisp->scancube_len[0] = 0;		/* 1回きりなのでリセット */

 quit:
    return ret_ptr;
}

/* 注意: これを mdarray::update_length_info() に登録するのを忘れない事 */
/**
 * @brief  begin_scan_a_cube() で開始したスキャン・シーケンスの終了
 */
void mdarray::end_scan_a_cube() const
{
    mdarray *thisp = (mdarray *)this;			/* 強制的にキャスト */

    thisp->scancube_begin = NULL;			/* 開放 & 初期化 */
    thisp->scancube_len   = NULL;
    thisp->scancube_cntr  = NULL;
    thisp->scancube_ndim  = 0;

    thisp->scancube_ret_buf = NULL;			/* 開放 & 初期化 */
    thisp->scancube_ret_buf_blen = 0;

    return;
}

/* */

/*
 * 次元長やバッファ長が変更になった場合の，ユーザのコールバック関数を
 * 登録します．<br>
 * 次元やバッファ長を常に他にミラーしておきたい場合に使います．
 *
 * @return    自身の参照
 */
/*
mdarray &mdarray::register_length_change_notify_callback( 
					 void (*func)(const mdarray *, void *),
					 void *user_ptr )
{
    this->length_change_notify_func = func;
    this->length_change_notify_user_ptr = user_ptr;
    return *this;
}
*/

/* 自動リサイズはしない */
#if 0
const void *mdarray::carray( ssize_t idx0, ssize_t idx1, ssize_t idx2 ) const
{
    ssize_t idx;
    size_t sz = this->cached_bytes_rec;
    idx = this->get_idx_3d_cs(idx0,idx1,idx2);
    if ( idx < 0 ) return NULL;
    else return (const void *)((const char *)(this->data_ptr_cs()) + sz * idx);
}
#endif

/* */

/*
 * APIs for direct pointer access
 */

/**
 * @brief  ユーザのポインタ変数の登録
 *
 * ユーザのポインタ変数をオブジェクトに登録します．<br>
 * このメンバ関数を使ってユーザのポインタ変数のアドレスを登録すれば，オブジェク
 * トが管理するバッファの先頭アドレスを常にユーザのポインタ変数に保持させておく
 * 事ができます．
 *
 * @param   extptr_address ユーザのポインタ変数のアドレス
 * @return  自身の参照
 * @note    引数を与える時に，ポインタ変数に「&」をつけるのを忘れないように
 *          しましょう
 * @attention  ユーザのポインタ変数を登録すると shallow copy が機能しなくなり
 *             ます．
 */
mdarray &mdarray::register_extptr( void *extptr_address )
{
    this->extptr_rec = (void **)extptr_address;
    if ( this->extptr_rec != NULL ) *(this->extptr_rec) = this->data_ptr();
    return *this;
}

/**
 * @brief  ユーザのポインタ変数(2d用)の登録
 *
 * ユーザのポインタ変数(2d用)をオブジェクトに登録します．<br>
 * このメンバ関数を使ってユーザのポインタ変数のアドレスを登録すれば，オブジェク
 * トが管理するアドレステーブルの先頭アドレスを常にユーザのポインタ変数に保持
 * させておく事ができます．
 *
 * @param   extptr2d_address ユーザのポインタ変数のアドレス
 * @return  自身の参照
 * @note    引数を与える時に，ポインタ変数に「&」をつけるのを忘れないように
 *          しましょう
 * @attention  ユーザのポインタ変数を登録すると shallow copy が機能しなくなり
 *             ます．
 */
mdarray &mdarray::register_extptr_2d( void *extptr2d_address )
{
    this->extptr_2d_rec = (void ***)extptr2d_address;
    if ( this->extptr_2d_rec != NULL ) this->update_arr_ptr_2d();
    else this->free_arr_ptr_2d();
    return *this;
}

/**
 * @brief  ユーザのポインタ変数(3d用)の登録
 *
 * ユーザのポインタ変数(3d用)をオブジェクトに登録します．<br>
 * このメンバ関数を使ってユーザのポインタ変数のアドレスを登録すれば，オブジェク
 * トが管理するアドレステーブルの先頭アドレスを常にユーザのポインタ変数に保持さ
 * せておく事ができます．
 *
 * @param   extptr3d_address ユーザのポインタ変数のアドレス
 * @return  自身の参照
 * @note    引数を与える時に，ポインタ変数に「&」をつけるのを忘れないように
 *          しましょう
 * @attention  ユーザのポインタ変数を登録すると shallow copy が機能しなくなり
 *             ます．
 */
mdarray &mdarray::register_extptr_3d( void *extptr3d_address )
{
    this->extptr_3d_rec = (void ****)extptr3d_address;
    if ( this->extptr_3d_rec != NULL ) this->update_arr_ptr_3d();
    else this->free_arr_ptr_3d();
    return *this;
}


/*
 * etc. 
 */

/**
 * @brief  自身の配列と指定されたオブジェクトの配列との比較
 *
 * 自身と指定されたオブジェクトobj の配列が等しいかどうかを返します．<br>
 * 配列の型が異なっても配列長と値が等しければ真(true)，異なれば偽(false) を
 * 返します．
 *
 * @param     obj mdarray クラスのオブジェクト
 * @return    true : 配列サイズ，要素の値が一致した場合<br>
 *            false : 配列サイズ，要素の値が不一致である場合
 */
bool mdarray::compare(const mdarray &obj) const
{
    size_t i;
    void (*func_dest2d)(const void *,void *,size_t,int,void *);
    void (*func_src2d)(const void *,void *,size_t,int,void *);
    func_dest2d = NULL;
    func_src2d = NULL;
    size_t dim_len;

    if ( &obj == this ) return true;

    /* 大きい方をとる */
    if ( this->dim_length() < obj.dim_length() ) dim_len = obj.dim_length();
    else dim_len = this->dim_length();

    for ( i = 0 ; i < dim_len ; i++ ) {
	if ( this->length(i) != obj.length(i) ) return false;
    }

    if ( this->length() == 0 && obj.length() == 0 ) return true;
    if ( this->length() == 0 ) return false;
    if ( obj.length() == 0 ) return false;

    /* アレイの型を double に変換する関数 */
    func_dest2d = this->func_cnv_nd_x2d;
    func_src2d = obj.func_cnv_nd_x2d;

    if ( func_dest2d == NULL || func_src2d == NULL ) {
	if ( this->sz_type_rec != obj.sz_type_rec ) return false;
	size_t all_len = this->length() * this->bytes();
	const char *dest = (const char *)(this->data_ptr_cs());
	const char *src = (const char *)(obj.data_ptr_cs());
	for ( i=0 ; i < all_len ; i++ ) {
	    if ( dest[i] != src[i] ) return false;
	}
	return true;
    }
    else {
	const size_t dest_bytes = this->bytes();
	const size_t src_bytes = obj.bytes();
	double d0[1024];				/* 一時領域 */
	double d1[1024];
	size_t all_len = this->length();
	const char *dest = (const char *)(this->data_ptr_cs());
	const char *src = (const char *)(obj.data_ptr_cs());
	size_t j, n_blk;
	void *user_ptr = (void *)&Null_cnv_nd_prms;
	/* */
	n_blk = all_len / 1024;
	for ( j=0 ; j < n_blk ; j++ ) {
	    (*func_src2d)((const void *)src,(void *)d0,1024,+1,user_ptr);
	    (*func_dest2d)((const void *)dest,(void *)d1,1024,+1,user_ptr);
	    for ( i=0 ; i < 1024 ; i++ ) {
		if ( d0[i] != d1[i] ) return false;
	    }
	    src += src_bytes * 1024;
	    dest += dest_bytes * 1024;
	    all_len -= 1024;
	}
	(*func_src2d)((const void *)src,(void *)d0,all_len,+1,user_ptr);
	(*func_dest2d)((const void *)dest,(void *)d1,all_len,+1,user_ptr);
	for ( i=0 ; i < all_len ; i++ ) {
	    if ( d0[i] != d1[i] ) return false;
	}
	return true;
    }
}

/**
 * @brief  自身の型に応じたバイトオーダの調整
 *
 * このメンバ関数は，自身の配列をバイナリデータとしてファイルに保存したい時，
 * あるいはファイルのバイナリデータを自身の配列に取り込みたい時に使います．<br>
 * (詳細は reverse_byte_order( bool, ssize_t, size_t, size_t ) を参照)
 *
 * @param     is_little_endian 1 回目の処理後のメモリ上のエンディアン
 * @return    自身の参照
 * @note      高速化のため，SIMD命令を使っています．
 */
mdarray &mdarray::reverse_endian( bool is_little_endian )
{
    return this->reverse_byte_order(is_little_endian, 0,
				    0, this->cached_length_rec);
}

/**
 * @brief  自身の型に応じたバイトオーダの調整
 *
 * このメンバ関数は，自身の配列をバイナリデータとしてファイルに保存したい時，
 * あるいはファイルのバイナリデータを自身の配列に取り込みたい時に使います．<br>
 * (詳細は reverse_byte_order( bool, ssize_t, size_t, size_t ) を参照)
 *
 * @param     is_little_endian 1 回目の処理後のメモリ上のエンディアン
 * @param     begin 処理を開始する要素番号
 * @param     length 処理対象となる長さ
 * @return    自身の参照
 * @note      高速化のため，SIMD命令を使っています．
 */
mdarray &mdarray::reverse_endian( bool is_little_endian,
				  size_t begin, size_t length )
{
    return this->reverse_byte_order(is_little_endian, 0, begin, length);
}

/**
 * @brief  任意の型のバイトオーダの調整
 *
 * このメンバ関数は，自身の配列をバイナリデータとしてファイルに保存したい時，
 * あるいはファイルのバイナリデータを自身の配列に取り込みたい時に使います．<br>
 * (詳細は reverse_byte_order( bool, ssize_t, size_t, size_t ) を参照)
 *
 * @param     is_little_endian 1 回目の処理後のメモリ上のエンディアン
 * @param     sz_type 型種別
 * @return    自身の参照
 * @note      高速化のため，SIMD命令を使っています．
 */
mdarray &mdarray::reverse_byte_order( bool is_little_endian, ssize_t sz_type )
{
    return this->reverse_byte_order(is_little_endian, sz_type, 
				    0, this->cached_length_rec);
}

/**
 * @brief  任意の型のバイトオーダの調整
 *
 * このメンバ関数は，自身の配列をバイナリデータとしてファイルに保存したい時，
 * あるいはファイルのバイナリデータを自身の配列に取り込みたい時に使います．<br>
 * ファイルにデータを保存したい時は，このメンバ関数を呼び出してファイル保存に適
 * したエンディアンに変換し，data_ptr() メンバ関数などで取得したアドレスをスト
 * リーム書き込み用の関数に与えて内容を書き込んだ後，再度このメンバ関数を呼び出
 * して，エンディアンを元に戻します．<br>
 * ファイルからデータを読み込みたい時は，data_ptr() メンバ関数などで取得したア
 * ドレスをストリーム読み取り用の関数に与えて内容を読み込んだ後，このメンバ関数
 * を呼び出して処理系に適したエンディアンに変換します．<br>
 * 上記のいずれの場合も，ファイルに保存されるべきデータがビッグエンディアンなら
 * ば，第1引数に false をセットしまず(リトルエンディアンなら true です)．<br>
 * このメンバ関数は，処理系によって使い分けが必要とならないように作られていま
 * す．例えば，ファイルにビッグエンディアンのデータを保存したいので，
 * is_little_endian に false を指定し，このメンバ関数を呼び出したとします．
 * この時，マシンがビッグエンディアンであれば，実際には反転処理は行われません
 * (マシンがリトルエンディアンであれば，反転処理が行われます)．
 * 次に，オブジェクト内のバイナリデータをそのままファイルに保存すれば，
 * 指定されたバイトオーダーのバイナリファイルができます．
 * その後，再度同じ引数でこのメンバ関数を呼び出して，エンディアンが反転されてい
 * る場合は元に戻す処理を行います．<br>
 * 従って，ファイルへの保存に際しては，このメンバ関数は同じ引数で２回呼び出す事
 * が前提です．
 *
 * @param     is_little_endian 1 回目の処理後のメモリ上のエンディアン
 * @param     sz_type 型種別
 * @param     begin 処理を開始する要素番号
 * @param     length 処理対象となる長さ (自身の型を単位とした要素数)
 * @return    自身の参照
 * @note      高速化のため，SIMD命令を使っています．
 */
mdarray &mdarray::reverse_byte_order( bool is_little_endian, ssize_t sz_type,
				      size_t begin, size_t length )
{
    size_t sz_bytes;
    bool rv;

    //err_report1(__FUNCTION__,"DEBUG","begin = %ld",(long)begin);
    //err_report1(__FUNCTION__,"DEBUG","length = %ld",(long)length);

    /* check out of range */
    if ( this->cached_length_rec <= begin ) return *this;

    if ( sz_type == 0 ) sz_type = this->sz_type_rec;

    /* obtain bytes of an element */
    sz_bytes = zt2bytes(sz_type);

    if ( sz_bytes <= 1 ) return *this;

    /* for floating point numbers */
    if ( sz_type < 0 ) {
	/* for complex numbers */
	if ( sz_type == FCOMPLEX_ZT || sz_type == DCOMPLEX_ZT || 
	     sz_type == LDCOMPLEX_ZT ) {
	    sz_bytes /= 2;
	}
	/* check */
	if ( is_little_endian == Float_word_order_is_little ) rv = false;
	else rv = true;
    }
    /* for integers */
    else {
	/* check */
	if ( is_little_endian == Byte_order_is_little ) rv = false;
	else rv = true;
    }

    //err_report(__FUNCTION__,"DEBUG","reverse_byte_order ... begin");
    if ( rv == true ) {
	unsigned char *dest_ptr = 
	   (unsigned char *)(this->data_ptr()) + (begin * this->cached_bytes_rec);
	size_t len_all;
	/* determine len_all */
	if ( this->cached_length_rec < begin + length ) 
	    length = this->cached_length_rec - begin;
	len_all = (length * this->cached_bytes_rec) / sz_bytes;

	s_byteswap( dest_ptr, sz_bytes, len_all );

    }
    //err_report(__FUNCTION__,"DEBUG","reverse_byte_order ... end");

    return *this;
}

/**
 * @brief  自身の型と指定された型 szt との演算が行なわれた時の型を返す
 *
 * @param     szt 型
 * @return    演算が行なわれた時の型
 *            
 */
ssize_t mdarray::ope_size_type( ssize_t szt ) const
{
    return get_sz_type_from_two(this->size_type(), szt);
}

/**
 * @brief  テンポラリオブジェクトのためのshallow copy属性の付与
 *
 *  自身が「=」演算子または init(obj) の引数に与えられた時の shallow copy を
 *  許可する設定を行ないます．この設定は，関数またはメンバ関数によって返され
 *  るテンポラリオブジェクトにのみ付与されるべきものです．<br>
 *
 *  mdarray クラスの shallow copy の解除のタイミングは，配列に対するあらゆる
 *  読み書きであって「書き込み」ではありません．読み取り専用のメンバ関数が使用
 *  された場合も，shallow copy 解除のための deep copy が走ります．したがって，
 *  テンポラリオブジェクト以外の場合に set_scopy_flag() を使っても全く意味が
 *  ありません．
 *
 */
/* 注意: 参照を返したいところだが，return 文でそれをやるとインスタンス */
/*       を2回コピーされるというおかしな事になるので void になっている */
void mdarray::set_scopy_flag()
{
    this->shallow_copy_ok = true;
    return;
}

/**
 * @brief  あまりに長い文字列の最後の部分を「...」に置換 (内部用)
 *
 * @note   dprint() メンバ関数で使われる．<br>
 *         private な関数です．
 */
static int limited_str_cat( char *buf, int *buf_pos, int len_limit, 
			    const char *src )
{
    int bufpos = *buf_pos;
    int i = 0;

    while ( bufpos < len_limit && src[i] != '\0' ) {
	buf[bufpos] = src[i];
	bufpos ++;
	i ++;
    }
    buf[bufpos] = '\0';

    if ( bufpos == len_limit ) {
	buf[len_limit - 1] = '.';
	buf[len_limit - 2] = '.';
	buf[len_limit - 3] = '.';
	buf[len_limit - 4] = ' ';
	*buf_pos = bufpos;
	return 1;
    }
    else {
	*buf_pos = bufpos;
	return 0;
    }
}

/**
 * @brief  オブジェクト情報の標準エラー出力への出力
 *
 *  自身のオブジェクト情報を，標準エラー出力へ出力します．<br>
 *  ユーザ・プログラムのデバッグを目的としたメンバ関数です．
 *
 * @param  msg デバッグ情報出力の先頭に表示される文字列
 *            
 */
void mdarray::dprint( const char *msg ) const
{
    size_t i,j,k;

    if ( msg != NULL ) {
	sli__eprintf("%s sli::%s[sz_type=%zd, dim=(", msg, CLASS_NAME,
		     this->size_type());
    }
    else {
	sli__eprintf("sli::%s[obj=0x%zx, sz_type=%zd, dim=(", CLASS_NAME,
		 (const size_t)this, /* (const size_t)(this->data_ptr_cs()), */
		 this->size_type());
    }
    for ( i=0 ; i < this->dim_length() ; i++ ) {
	if ( 0 < i ) sli__eprintf(",");
	sli__eprintf("%zu", this->length(i));
    }
    sli__eprintf(")] = {\n");

    if ( 0 < this->length() ) {
      /* やっつけ仕事 */
      for ( i=0 ; i < this->layer_length() ; i++ ) {
	 if ( 3 < i ) {
	    sli__eprintf("        :\n");
	    sli__eprintf("        :\n");
	    break;
	 }
	 for ( j=0 ; j < this->row_length() ; j++ ) {
	    const int len_limit = 74;
	    char outbuf[len_limit + 1] = {'\0'};
	    int pos_outbuf = 0;
	    if ( 6 < j ) {
		sli__eprintf("                :\n");
		sli__eprintf("                :\n");
		break;
	    }
	    /* */
	    if ( 1 < this->dim_length() ) {
		if ( 2 < this->dim_length() ) {
		    if ( j == 0 ) limited_str_cat(outbuf,&pos_outbuf,len_limit,
						  " { { ");
		    else limited_str_cat(outbuf,&pos_outbuf,len_limit,"   { ");
		}
		else limited_str_cat(outbuf,&pos_outbuf,len_limit," { ");
	    }
	    else limited_str_cat(outbuf,&pos_outbuf,len_limit," ");
	    /* */
	    if ( 0 < this->dim_length() ) {
		char pbuf[64];
		if ( this->size_type() == DOUBLE_ZT ) {
		    for ( k=0 ; k < this->col_length() ; k++ ) {
			if ( k+1 < this->col_length() ) {
			    sli__snprintf(pbuf,64,"%.10g, ", 
					  this->dvalue(k,j,i));
			}
			else {
			    sli__snprintf(pbuf,64,"%.10g", 
					  this->dvalue(k,j,i));
			}
			if ( limited_str_cat(outbuf,&pos_outbuf,len_limit,
					     pbuf) ) break;
		    }
		}
		else if ( this->size_type() == FLOAT_ZT ) {
		    for ( k=0 ; k < this->col_length() ; k++ ) {
			if ( k+1 < this->col_length() ) {
			    sli__snprintf(pbuf,64,"%g, ", this->dvalue(k,j,i));
			}
			else {
			    sli__snprintf(pbuf,64,"%g", this->dvalue(k,j,i));
			}
			if ( limited_str_cat(outbuf,&pos_outbuf,len_limit,
					     pbuf) ) break;
		    }
		}
		else if ( this->size_type() == UCHAR_ZT ||
			  this->size_type() == INT16_ZT ||
			  this->size_type() == INT32_ZT ||
			  this->size_type() == INT64_ZT ) {
		    for ( k=0 ; k < this->col_length() ; k++ ) {
			if ( k+1 < this->col_length() ) {
			    sli__snprintf(pbuf,64,"%lld, ", 
					  this->llvalue(k,j,i));
			}
			else {
			    sli__snprintf(pbuf,64,"%lld", 
					  this->llvalue(k,j,i));
			}
			if ( limited_str_cat(outbuf,&pos_outbuf,len_limit,
					     pbuf) ) break;
		    }
		}
		else {
		    /* 16進でダンプ */
		    for ( k=0 ; k < this->col_length() ; k++ ) {
			const unsigned char *p =
			     (const unsigned char *)(this->data_ptr_cs(k,j,i));
			size_t l;
			if ( limited_str_cat(outbuf,&pos_outbuf,len_limit,
					     "0x") ) break;
			for ( l=0 ; l < this->bytes() ; l++ ) {
			    sli__snprintf(pbuf,64,"%02hhx", p[l]);
			    if ( limited_str_cat(outbuf,&pos_outbuf,len_limit,
						 pbuf) ) break;
			}
			if ( k+1 < this->col_length() ) {
			    if ( limited_str_cat(outbuf,&pos_outbuf,len_limit,
						 ",") ) break;
			}
		    }
		}
		sli__eprintf("%s ", outbuf);
	    }
	    if ( 1 < this->dim_length() ) {
		if ( 2 < this->dim_length() ) {
		    if ( j+1 == this->row_length() ) {
			sli__eprintf("} }");
			if (i+1 < this->layer_length()) sli__eprintf(",");
		    }
		    else sli__eprintf("}");
		}
		else sli__eprintf("}");
	    }
	    if ( j+1 < this->row_length() ) sli__eprintf(",");
	    sli__eprintf("\n");
	 }
      }
    }
    sli__eprintf("}\n");

    return;
}

/**
 * @brief  mdarray の演算 (現在は未使用)
 *
 *  自身を，指定された演算用関数(func，funcf，funcl)により演算し，
 *  その結果を返す．
 *
 * @param      func    double 型の演算用関数へのアドレス
 * @param      funcf   float 型の演算用関数へのアドレス
 * @param      funcl   long double 型の演算用関数へのアドレス
 * @return     自身の参照
 * @note       関数呼び出しのオーバヘッドのため，現在は使われていない．
 */
mdarray &mdarray::calc1( double (*func)(double), float (*funcf)(float),
			 long double (*funcl)(long double) )
{
    const size_t len = this->length();
    size_t i;
    if ( this->sz_type_rec == DOUBLE_ZT ) {
	double *pp = (double *)(this->data_ptr());
	if ( func != NULL )
	    for ( i=0 ; i < len ; i++ ) pp[i] = (*func)(pp[i]);
	else if ( funcl != NULL )
	    for ( i=0 ; i < len ; i++ ) pp[i] = (*funcl)(pp[i]);
	else if ( funcf != NULL )
	    for ( i=0 ; i < len ; i++ ) pp[i] = (*funcf)(pp[i]);
    }
    else if ( this->sz_type_rec == FLOAT_ZT ) {
	float *pp = (float *)(this->data_ptr());
	if ( funcf != NULL )
	    for ( i=0 ; i < len ; i++ ) pp[i] = (*funcf)(pp[i]);
	else if ( func != NULL )
	    for ( i=0 ; i < len ; i++ ) pp[i] = (*func)(pp[i]);
	else if ( funcl != NULL )
	    for ( i=0 ; i < len ; i++ ) pp[i] = (*funcl)(pp[i]);
    }
    else if ( this->sz_type_rec == LDOUBLE_ZT ) {
	long double *pp = (long double *)(this->data_ptr());
	if ( funcl != NULL )
	    for ( i=0 ; i < len ; i++ ) pp[i] = (*funcl)(pp[i]);
	else if ( func != NULL )
	    for ( i=0 ; i < len ; i++ ) pp[i] = (*func)(pp[i]);
	else if ( funcf != NULL )
	    for ( i=0 ; i < len ; i++ ) pp[i] = (*funcf)(pp[i]);
    }
    else if ( this->sz_type_rec == INT32_ZT ) {
	int32_t *pp = (int32_t *)(this->data_ptr());
	if ( func != NULL )
	    for ( i=0 ; i < len ; i++ ) pp[i] = (int32_t)((*func)(pp[i]));
	else if ( funcl != NULL )
	    for ( i=0 ; i < len ; i++ ) pp[i] = (int32_t)((*funcl)(pp[i]));
	else if ( funcf != NULL )
	    for ( i=0 ; i < len ; i++ ) pp[i] = (int32_t)((*funcf)(pp[i]));
    }
    else if ( this->sz_type_rec == INT16_ZT ) {
	int16_t *pp = (int16_t *)(this->data_ptr());
	if ( func != NULL )
	    for ( i=0 ; i < len ; i++ ) pp[i] = (int16_t)((*func)(pp[i]));
	else if ( funcl != NULL )
	    for ( i=0 ; i < len ; i++ ) pp[i] = (int16_t)((*funcl)(pp[i]));
	else if ( funcf != NULL )
	    for ( i=0 ; i < len ; i++ ) pp[i] = (int16_t)((*funcf)(pp[i]));
    }
    else if ( this->sz_type_rec == INT64_ZT ) {
	int64_t *pp = (int64_t *)(this->data_ptr());
	if ( func != NULL )
	    for ( i=0 ; i < len ; i++ ) pp[i] = (int64_t)((*func)(pp[i]));
	else if ( funcl != NULL )
	    for ( i=0 ; i < len ; i++ ) pp[i] = (int64_t)((*funcl)(pp[i]));
	else if ( funcf != NULL )
	    for ( i=0 ; i < len ; i++ ) pp[i] = (int64_t)((*funcf)(pp[i]));
    }
    else if ( this->sz_type_rec == UCHAR_ZT ) {
	unsigned char *pp = (unsigned char *)(this->data_ptr());
	if ( func != NULL )
	  for ( i=0 ; i < len ; i++ ) pp[i] = (unsigned char)((*func)(pp[i]));
	else if ( funcl != NULL )
	  for ( i=0 ; i < len ; i++ ) pp[i] = (unsigned char)((*funcl)(pp[i]));
	else if ( funcf != NULL )
	  for ( i=0 ; i < len ; i++ ) pp[i] = (unsigned char)((*funcf)(pp[i]));
    }
    else if ( this->sz_type_rec == FCOMPLEX_ZT ) {
	fcomplex *pp = (fcomplex *)(this->data_ptr());
	if ( funcf != NULL )
	  for ( i=0 ; i < len ; i++ ) pp[i] = f2fx((*funcf)(creal(pp[i])));
	else if ( func != NULL )
	  for ( i=0 ; i < len ; i++ ) pp[i] = f2fx((*func)(creal(pp[i])));
	else if ( funcl != NULL )
	  for ( i=0 ; i < len ; i++ ) pp[i] = f2fx((*funcl)(creal(pp[i])));
    }
    else if ( this->sz_type_rec == DCOMPLEX_ZT ) {
	dcomplex *pp = (dcomplex *)(this->data_ptr());
	if ( func != NULL )
	  for ( i=0 ; i < len ; i++ ) pp[i] = d2dx((*func)(creal(pp[i])));
	else if ( funcl != NULL )
	  for ( i=0 ; i < len ; i++ ) pp[i] = d2dx((*funcl)(creal(pp[i])));
	else if ( funcf != NULL )
	  for ( i=0 ; i < len ; i++ ) pp[i] = d2dx((*funcf)(creal(pp[i])));
    }
    else if ( this->sz_type_rec == LDCOMPLEX_ZT ) {
	ldcomplex *pp = (ldcomplex *)(this->data_ptr());
	if ( funcl != NULL )
	  for ( i=0 ; i < len ; i++ ) pp[i] = ld2ldx((*funcl)(creal(pp[i])));
	else if ( func != NULL )
	  for ( i=0 ; i < len ; i++ ) pp[i] = ld2ldx((*func)(creal(pp[i])));
	else if ( funcf != NULL )
	  for ( i=0 ; i < len ; i++ ) pp[i] = ld2ldx((*funcf)(creal(pp[i])));
    }
    return *this;
}

namespace _calc2
{
#define MAKE_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
static void fncname(const void *org_val_ptr, void *new_val_ptr, \
		    bool rv, size_t len, const void *default_v_ptr, \
		    double (*func)(double,double), \
		    float (*funcf)(float,float), \
		    long double (*funcl)(long double,long double) ) \
{ \
  size_t i; \
  new_type *dest = (new_type *)new_val_ptr; \
  const org_type *src = (const org_type *)org_val_ptr; \
  if ( src != NULL && rv == false ) { \
    if ( func != NULL ) { \
      for (i=0 ; i<len ; i++) dest[i] = (new_type)((*func)((double)dest[i],(double)src[i])); \
    } \
    else if ( funcf != NULL ) { \
      for (i=0 ; i<len ; i++) dest[i] = (new_type)((*funcf)((float)dest[i],(float)src[i])); \
    } \
    else if ( funcl != NULL ) { \
      for (i=0 ; i<len ; i++) dest[i] = (new_type)((*funcl)((long double)dest[i],(long double)src[i])); \
    } \
  } \
  else if ( src != NULL && rv == true ) { \
    if ( func != NULL ) { \
      for (i=0 ; i<len ; i++) dest[i] = (new_type)((*func)((double)src[i],(double)dest[i])); \
    } \
    else if ( funcf != NULL ) { \
      for (i=0 ; i<len ; i++) dest[i] = (new_type)((*funcf)((float)src[i],(float)dest[i])); \
    } \
    else if ( funcl != NULL ) { \
      for (i=0 ; i<len ; i++) dest[i] = (new_type)((*funcl)((long double)src[i],(long double)dest[i])); \
    } \
  } \
  else if ( src == NULL && rv == false ) { \
    org_type def_v = (org_type)0; \
    if ( default_v_ptr != NULL ) def_v = *((const org_type *)default_v_ptr); \
    if ( func != NULL ) { \
      for (i=0 ; i<len ; i++) dest[i] = (new_type)((*func)((double)dest[i],(double)def_v)); \
    } \
    else if ( funcf != NULL ) { \
      for (i=0 ; i<len ; i++) dest[i] = (new_type)((*funcf)((float)dest[i],(float)def_v)); \
    } \
    else if ( funcl != NULL ) { \
      for (i=0 ; i<len ; i++) dest[i] = (new_type)((*funcl)((long double)dest[i],(long double)def_v)); \
    } \
    /* for (i=0 ; i<len ; i++) dest[i] = 0; */ \
  } \
  else if ( src == NULL && rv == true ) { \
    org_type def_v = (org_type)0; \
    if ( default_v_ptr != NULL ) def_v = *((const org_type *)default_v_ptr); \
    if ( func != NULL ) { \
      for (i=0 ; i<len ; i++) dest[i] = (new_type)((*func)((double)def_v,(double)dest[i])); \
    } \
    else if ( funcf != NULL ) { \
      for (i=0 ; i<len ; i++) dest[i] = (new_type)((*funcf)((float)def_v,(float)dest[i])); \
    } \
    else if ( funcl != NULL ) { \
      for (i=0 ; i<len ; i++) dest[i] = (new_type)((*funcl)((long double)def_v,(long double)dest[i])); \
    } \
    /* for (i=0 ; i<len ; i++) dest[i] = 0; */ \
  } \
}
SLI__MDARRAY__DO_OPERATION_2TYPES(MAKE_FUNC,,,,,,,,,,,,,);
#undef MAKE_FUNC
}

/**
 * @brief  mdarray どうしの演算 (現在は未使用)
 *
 *  自身と，与えられた mdarray オブジェクトについて，指定された演算用関数
 *  (func，funcf，funcl)により演算し，その結果を返す．
 *
 * @param     obj     演算する mdarray オブジェクト 
 * @param     obj_1st 演算時の obj 引数の位置．
 *                    つまり，x,yの扱い：func(x,y) or func(y,x)．<br>
 *                    true の場合は第１引数に obj が指定される<br>
 * @param     func    double 型の演算用関数へのアドレス
 * @param     funcf   float 型の演算用関数へのアドレス
 * @param     funcl   long double 型の演算用関数へのアドレス
 * @return    自身の参照
 * @throw     複素数型以外の obj 引数が指定された場合
 * @throw     内部バッファの確保に失敗した場合
 * @note      内部でr_calc2を使用します.
 *            関数呼び出しのオーバヘッドのため，現在は使われていない．
 */
mdarray &mdarray::calc2(const mdarray &obj, bool obj_1st,
			double (*func)(double, double), 
			float (*funcf)(float, float),
			long double (*funcl)(long double, long double))
{
    void (*func_calc)(const void *, void *, bool, size_t, const void *,
		      double (*)(double,double),
		      float (*)(float,float),
		      long double (*)(long double,long double));

    const ssize_t this_szt =  this->size_type();
    const ssize_t obj_szt = obj.size_type();

    /* 複素数の場合は例外を返す */

    /* 複素数型 */
    if ( this_szt == FCOMPLEX_ZT || this_szt == DCOMPLEX_ZT ||
	 this_szt == LDCOMPLEX_ZT ) {
	err_throw(__FUNCTION__,"ERROR","complex type cannot be used");
    }
    /* 実数型 */
    else if ( this_szt == FLOAT_ZT || this_szt == DOUBLE_ZT ||
	      this_szt == LDOUBLE_ZT || this_szt == UCHAR_ZT ||
	      this_szt == INT16_ZT || this_szt == INT32_ZT ||
	      this_szt == INT64_ZT ) {
	/* NO PROBLEM */
    }
    /* その他の型 */
    else {
	err_throw(__FUNCTION__,"ERROR","unsupported type");
    }

    /* 複素数型 */
    if ( obj_szt == FCOMPLEX_ZT || obj_szt == DCOMPLEX_ZT ||
	 obj_szt == LDCOMPLEX_ZT ) {
	err_throw(__FUNCTION__,"ERROR","complex type cannot be used");
    }
    /* 実数型 */
    else if ( obj_szt == FLOAT_ZT || obj_szt == DOUBLE_ZT ||
	      obj_szt == LDOUBLE_ZT || obj_szt == UCHAR_ZT ||
	      obj_szt == INT16_ZT || obj_szt == INT32_ZT ||
	      obj_szt == INT64_ZT ) {
	/* NO PROBLEM */
    }
    /* その他の型 */
    else {
	err_throw(__FUNCTION__,"ERROR","unsupported type");
    }

    if ( this->auto_resize() == true ) {
	size_t i;
	for ( i=0 ; i < obj.dim_length() ; i++ ) {
	    if ( this->length(i) < obj.length(i) ) {
		this->resize(i, obj.length(i));
	    }
	}
    }

    if ( this->length() == 0 || obj.length() == 0 ) return *this;

    func_calc = NULL;

    /* 関数を選択 */
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
    if ( obj.sz_type_rec == org_sz_type && this->sz_type_rec == new_sz_type ) { \
	func_calc = &_calc2::fncname; \
    }
    SLI__MDARRAY__DO_OPERATION_2TYPES(SEL_FUNC,,,,,,,,,,,,,else);
#undef SEL_FUNC

    /* 関数があれば代入を行なう */
    if ( func_calc != NULL ) {
	const ssize_t dest_sz_type = this->size_type();
	size_t len_src, len_dest, ndim;

	if ( dest_sz_type < DOUBLE_ZT ) {
	    if ( funcl != NULL ) {
		func = NULL;
		funcf = NULL;
	    }
	}
	if ( dest_sz_type == FLOAT_ZT ) {
	    if ( funcf != NULL ) {
		func = NULL;
		funcl = NULL;
	    }
	}

	/* 大きい方をとる */
	ndim = this->dim_length();
	if ( ndim < obj.dim_length() ) ndim = obj.dim_length();

	/* 最初は，扱う次元内の全ピクセル数を len_xxx にセットする */
	len_src = obj.length();
	len_dest = this->length();
	
	this->r_calc2(obj, obj_1st, ndim-1, len_src, len_dest, 0, 0, 
		      func, funcf, funcl, func_calc);
    }
    else {
	err_report(__FUNCTION__,"WARNING","invalid size of type; ignored");
    }

    return *this;
}

/**
 * @brief  mdarray の演算 (現在は未使用)
 *
 *  自身を，指定された演算用関数(func，funcf，funcl)により演算し，
 *  その結果を返す．
 *
 * @param      func    dcomplex 型の演算用関数へのアドレス
 * @param      funcf   fcomplex 型の演算用関数へのアドレス
 * @param      funcl   ldcomplex 型の演算用関数へのアドレス
 * @return     自身の参照
 * @note       関数呼び出しのオーバヘッドのため，現在は使われていない．
 */
mdarray &mdarray::calcx1( dcomplex (*func)(dcomplex),
			  fcomplex (*funcf)(fcomplex),
			  ldcomplex (*funcl)(ldcomplex) )
{
    const size_t len = this->length();
    size_t i;
    if ( this->sz_type_rec == DCOMPLEX_ZT ) {
	dcomplex *pp = (dcomplex *)(this->data_ptr());
	if ( func != NULL )
	  for ( i=0 ; i < len ; i++ ) pp[i] = (dcomplex)((*func)((dcomplex)pp[i]));
	else if ( funcl != NULL )
	  for ( i=0 ; i < len ; i++ ) pp[i] = (dcomplex)((*funcl)((ldcomplex)pp[i]));
	else if ( funcf != NULL )
	  for ( i=0 ; i < len ; i++ ) pp[i] = (dcomplex)((*funcf)((fcomplex)pp[i]));
    }
    else if ( this->sz_type_rec == FCOMPLEX_ZT ) {
	fcomplex *pp = (fcomplex *)(this->data_ptr());
	if ( funcf != NULL )
	  for ( i=0 ; i < len ; i++ ) pp[i] = (fcomplex)((*funcf)((fcomplex)pp[i]));
	else if ( func != NULL )
	  for ( i=0 ; i < len ; i++ ) pp[i] = (fcomplex)((*func)((dcomplex)pp[i]));
	else if ( funcl != NULL )
	  for ( i=0 ; i < len ; i++ ) pp[i] = (fcomplex)((*funcl)((ldcomplex)pp[i]));
    }
    else if ( this->sz_type_rec == LDCOMPLEX_ZT ) {
	ldcomplex *pp = (ldcomplex *)(this->data_ptr());
	if ( funcl != NULL )
	  for ( i=0 ; i < len ; i++ ) pp[i] = (ldcomplex)((*funcl)((ldcomplex)pp[i]));
	else if ( func != NULL )
	  for ( i=0 ; i < len ; i++ ) pp[i] = (ldcomplex)((*func)((dcomplex)pp[i]));
	else if ( funcf != NULL )
	  for ( i=0 ; i < len ; i++ ) pp[i] = (ldcomplex)((*funcf)((fcomplex)pp[i]));
    }
    else if ( this->sz_type_rec == DOUBLE_ZT ) {
	double *pp = (double *)(this->data_ptr());
	if ( func != NULL )
	    for ( i=0 ; i < len ; i++ ) pp[i] = (double)creal((*func)(d2dx(pp[i])));
	else if ( funcl != NULL )
	    for ( i=0 ; i < len ; i++ ) pp[i] = (double)creal((*funcl)(d2dx(pp[i])));
	else if ( funcf != NULL )
	    for ( i=0 ; i < len ; i++ ) pp[i] = (double)creal((*funcf)(d2dx(pp[i])));
    }
    else if ( this->sz_type_rec == FLOAT_ZT ) {
	float *pp = (float *)(this->data_ptr());
	if ( funcf != NULL )
	    for ( i=0 ; i < len ; i++ ) pp[i] = (float)creal((*funcf)(f2fx(pp[i])));
	else if ( func != NULL )
	    for ( i=0 ; i < len ; i++ ) pp[i] = (float)creal((*func)(f2fx(pp[i])));
	else if ( funcl != NULL )
	    for ( i=0 ; i < len ; i++ ) pp[i] = (float)creal((*funcl)(f2fx(pp[i])));
    }
    else if ( this->sz_type_rec == LDOUBLE_ZT ) {
	long double *pp = (long double *)(this->data_ptr());
	if ( funcl != NULL )
	    for ( i=0 ; i < len ; i++ ) pp[i] = (long double)creal((*funcl)(ld2ldx(pp[i])));
	else if ( func != NULL )
	    for ( i=0 ; i < len ; i++ ) pp[i] = (long double)creal((*func)(ld2ldx(pp[i])));
	else if ( funcf != NULL )
	    for ( i=0 ; i < len ; i++ ) pp[i] = (long double)creal((*funcf)(ld2ldx(pp[i])));
    }
    else if ( this->sz_type_rec == INT32_ZT ) {
	int32_t *pp = (int32_t *)(this->data_ptr());
	if ( func != NULL )
	    for ( i=0 ; i < len ; i++ ) pp[i] = (int32_t)creal((*func)(d2dx(pp[i])));
	else if ( funcl != NULL )
	    for ( i=0 ; i < len ; i++ ) pp[i] = (int32_t)creal((*funcl)(d2dx(pp[i])));
	else if ( funcf != NULL )
	    for ( i=0 ; i < len ; i++ ) pp[i] = (int32_t)creal((*funcf)(d2dx(pp[i])));
    }
    else if ( this->sz_type_rec == INT16_ZT ) {
	int16_t *pp = (int16_t *)(this->data_ptr());
	if ( func != NULL )
	    for ( i=0 ; i < len ; i++ ) pp[i] = (int16_t)creal((*func)(f2fx(pp[i])));
	else if ( funcl != NULL )
	    for ( i=0 ; i < len ; i++ ) pp[i] = (int16_t)creal((*funcl)(f2fx(pp[i])));
	else if ( funcf != NULL )
	    for ( i=0 ; i < len ; i++ ) pp[i] = (int16_t)creal((*funcf)(f2fx(pp[i])));
    }
    else if ( this->sz_type_rec == INT64_ZT ) {
	int64_t *pp = (int64_t *)(this->data_ptr());
	if ( func != NULL )
	    for ( i=0 ; i < len ; i++ ) pp[i] = (int64_t)creal((*func)(d2dx(pp[i])));
	else if ( funcl != NULL )
	    for ( i=0 ; i < len ; i++ ) pp[i] = (int64_t)creal((*funcl)(d2dx(pp[i])));
	else if ( funcf != NULL )
	    for ( i=0 ; i < len ; i++ ) pp[i] = (int64_t)creal((*funcf)(d2dx(pp[i])));
    }
    else if ( this->sz_type_rec == UCHAR_ZT ) {
	unsigned char *pp = (unsigned char *)(this->data_ptr());
	if ( func != NULL )
	  for ( i=0 ; i < len ; i++ ) pp[i] = (unsigned char)creal((*func)(f2fx(pp[i])));
	else if ( funcl != NULL )
	  for ( i=0 ; i < len ; i++ ) pp[i] = (unsigned char)creal((*funcl)(f2fx(pp[i])));
	else if ( funcf != NULL )
	  for ( i=0 ; i < len ; i++ ) pp[i] = (unsigned char)creal((*funcf)(f2fx(pp[i])));
    }
    return *this;
}

namespace _calcx2
{
#define MAKE_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type) \
static void fncname(const void *org_val_ptr, void *new_val_ptr, \
		    bool rv, size_t len, const void *default_v_ptr, \
		    dcomplex (*func)(dcomplex,dcomplex), \
		    fcomplex (*funcf)(fcomplex,fcomplex), \
		    ldcomplex (*funcl)(ldcomplex,ldcomplex) ) \
{ \
  size_t i; \
  new_type *dest = (new_type *)new_val_ptr; \
  const org_type *src = (const org_type *)org_val_ptr; \
  if ( src != NULL && rv == false ) { \
    if ( func != NULL ) { \
      for (i=0 ; i<len ; i++) dest[i] = (new_type)((*func)((dcomplex)dest[i],(dcomplex)src[i])); \
    } \
    else if ( funcf != NULL ) { \
      for (i=0 ; i<len ; i++) dest[i] = (new_type)((*funcf)((fcomplex)dest[i],(fcomplex)src[i])); \
    } \
    else if ( funcl != NULL ) { \
      for (i=0 ; i<len ; i++) dest[i] = (new_type)((*funcl)((ldcomplex)dest[i],(ldcomplex)src[i])); \
    } \
  } \
  else if ( src != NULL && rv == true ) { \
    if ( func != NULL ) { \
      for (i=0 ; i<len ; i++) dest[i] = (new_type)((*func)((dcomplex)src[i],(dcomplex)dest[i])); \
    } \
    else if ( funcf != NULL ) { \
      for (i=0 ; i<len ; i++) dest[i] = (new_type)((*funcf)((fcomplex)src[i],(fcomplex)dest[i])); \
    } \
    else if ( funcl != NULL ) { \
      for (i=0 ; i<len ; i++) dest[i] = (new_type)((*funcl)((ldcomplex)src[i],(ldcomplex)dest[i])); \
    } \
  } \
  else if ( src == NULL && rv == false ) { \
    org_type def_v = (org_type)(0.0 + 0.0*I); \
    if ( default_v_ptr != NULL ) def_v = *((const org_type *)default_v_ptr); \
    if ( func != NULL ) { \
      for (i=0 ; i<len ; i++) dest[i] = (new_type)((*func)((dcomplex)dest[i],(dcomplex)def_v)); \
    } \
    else if ( funcf != NULL ) { \
      for (i=0 ; i<len ; i++) dest[i] = (new_type)((*funcf)((fcomplex)dest[i],(fcomplex)def_v)); \
    } \
    else if ( funcl != NULL ) { \
      for (i=0 ; i<len ; i++) dest[i] = (new_type)((*funcl)((ldcomplex)dest[i],(ldcomplex)def_v)); \
    } \
    /* for (i=0 ; i<len ; i++) dest[i] = 0; */ \
  } \
  else if ( src == NULL && rv == true ) { \
    org_type def_v = (org_type)(0.0 + 0.0*I); \
    if ( default_v_ptr != NULL ) def_v = *((const org_type *)default_v_ptr); \
    if ( func != NULL ) { \
      for (i=0 ; i<len ; i++) dest[i] = (new_type)((*func)((dcomplex)def_v,(dcomplex)dest[i])); \
    } \
    else if ( funcf != NULL ) { \
      for (i=0 ; i<len ; i++) dest[i] = (new_type)((*funcf)((fcomplex)def_v,(fcomplex)dest[i])); \
    } \
    else if ( funcl != NULL ) { \
      for (i=0 ; i<len ; i++) dest[i] = (new_type)((*funcl)((ldcomplex)def_v,(ldcomplex)dest[i])); \
    } \
    /* for (i=0 ; i<len ; i++) dest[i] = 0; */ \
  } \
}
SLI__MDARRAY__DO_OPERATION_2TYPES_COMPLEX(MAKE_FUNC,);
#undef MAKE_FUNC
}

/**
 * @brief  mdarray どうしの演算 (現在は未使用)
 *
 *  自身と，与えられた mdarray オブジェクトとを，指定された演算用関数
 *  (func，funcf，funcl)により演算し，その結果を返す．
 *
 * @param      obj     演算する mdarray オブジェクト 
 * @param      obj_1st 演算時の obj 引数の位置．
 *                     true の場合は第１引数に obj が指定される
 * @param      func    dcomplex 型の演算用関数へのアドレス
 * @param      funcf   fcomplex 型の演算用関数へのアドレス
 * @param      funcl   ldcomplex 型の演算用関数へのアドレス
 * @throw              複素数型以外の obj 引数が指定された場合
 * @return     自身の参照
 * @note       関数呼び出しのオーバヘッドのため，現在は使われていない．
 */
mdarray &mdarray::calcx2(const mdarray &obj, bool obj_1st,
			 dcomplex (*func)(dcomplex,dcomplex),
			 fcomplex (*funcf)(fcomplex,fcomplex),
			 ldcomplex (*funcl)(ldcomplex,ldcomplex))
{
    const ssize_t this_szt =  this->size_type();
    const ssize_t obj_szt = obj.size_type();

    /* 実数の場合は例外を返す */

    /* 複素数型 */
    if ( this_szt == FCOMPLEX_ZT || this_szt == DCOMPLEX_ZT ||
	 this_szt == LDCOMPLEX_ZT ) {
	/* NO PROBLEM */
    }
    /* 実数型 */
    else if ( this_szt == FLOAT_ZT || this_szt == DOUBLE_ZT ||
	      this_szt == LDOUBLE_ZT || this_szt == UCHAR_ZT ||
	      this_szt == INT16_ZT || this_szt == INT32_ZT ||
	      this_szt == INT64_ZT ) {
	err_throw(__FUNCTION__,"ERROR","real type cannot be used");
    }
    /* その他の型 */
    else {
	err_throw(__FUNCTION__,"ERROR","unsupported type");
    }

    /* 複素数型 */
    if ( obj_szt == FCOMPLEX_ZT || obj_szt == DCOMPLEX_ZT ||
	 obj_szt == LDCOMPLEX_ZT ) {
	/* NO PROBLEM */
    }
    /* 実数型 */
    else if ( obj_szt == FLOAT_ZT || obj_szt == DOUBLE_ZT ||
	      obj_szt == LDOUBLE_ZT || obj_szt == UCHAR_ZT ||
	      obj_szt == INT16_ZT || obj_szt == INT32_ZT ||
	      obj_szt == INT64_ZT ) {
	err_throw(__FUNCTION__,"ERROR","real type cannot be used");
    }
    /* その他の型 */
    else {
	err_throw(__FUNCTION__,"ERROR","unsupported type");
    }

    void (*func_calc)(const void *, void *, bool, size_t, const void *,
		      dcomplex (*)(dcomplex,dcomplex),
		      fcomplex (*)(fcomplex,fcomplex),
		      ldcomplex (*)(ldcomplex,ldcomplex));

    if ( this->auto_resize() == true ) {
	size_t i;
	for ( i=0 ; i < obj.dim_length() ; i++ ) {
	    if ( this->length(i) < obj.length(i) ) {
		this->resize(i, obj.length(i));
	    }
	}
    }

    if ( this->length() == 0 || obj.length() == 0 ) return *this;

    func_calc = NULL;

    /* 関数を選択 */
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type) \
    if ( obj.sz_type_rec == org_sz_type && this->sz_type_rec == new_sz_type ) { \
	func_calc = &_calcx2::fncname; \
    }
    SLI__MDARRAY__DO_OPERATION_2TYPES_COMPLEX(SEL_FUNC,else);
#undef SEL_FUNC

    /* 関数があれば代入を行なう */
    if ( func_calc != NULL ) {
	const ssize_t dest_sz_type = this->size_type();
	size_t len_src, len_dest, ndim;

	if ( dest_sz_type < DCOMPLEX_ZT ) {
	    if ( funcl != NULL ) {
		func = NULL;
		funcf = NULL;
	    }
	}
	if ( dest_sz_type == FCOMPLEX_ZT ) {
	    if ( funcf != NULL ) {
		func = NULL;
		funcl = NULL;
	    }
	}

	/* 大きい方をとる */
	ndim = this->dim_length();
	if ( ndim < obj.dim_length() ) ndim = obj.dim_length();

	/* 最初は，扱う次元内の全ピクセル数を len_xxx にセットする */
	len_src = obj.length();
	len_dest = this->length();
	
	this->r_calcx2(obj, obj_1st, ndim-1, len_src, len_dest, 0, 0, 
		       func, funcf, funcl, func_calc);
    }
    else {
	err_report(__FUNCTION__,"WARNING","invalid size of type; ignored");
    }

    return *this;
}


/*
 * protected member functions
 */

/**
 * @brief  class_level_rec の値の取得
 * 
 * @return  class_level_recの値
 * @note    このメンバ関数は protected です．
 */
int mdarray::class_level() const
{
    return this->class_level_rec;
}

/**
 * @brief  継承クラスにおける，デフォルト型の設定
 *
 *  デフォルトの型を変更したい場合，継承クラスでオーバーライドします
 * 
 * @return  常に 1
 * @note    このメンバ関数は protected です．
 */
ssize_t mdarray::default_size_type()
{
    return 1;
}

/**
 * @brief  継承クラスにおける，受け入れ可能な型の設定
 *
 *  特定の型以外は拒否したい場合，継承クラスでオーバーライドします．
 * 
 * @param   sz_type
 * 
 * @return  常に true
 * @note    このメンバ関数は protected です．
 */
bool mdarray::is_acceptable_size_type( ssize_t sz_type )
{
    return true;
}

/**
 * @brief  次元数，要素数等の内部情報の更新
 *
 *  次元数，要素数等の長さ情報をキャッシュしている private メンバを更新
 *  します．<br>
 *  各種長さが更新されると必ず呼びだされるので，継承したクラスでオーバライド
 *  すると，各種長さのミラーリングが可能です．
 * 
 * @note  このメンバ関数は protected です．
 */
void mdarray::update_length_info()
{
    //err_report(__FUNCTION__,"DEBUG","called");
    /*
     * for this->length()
     */
    if ( this->dim_size_rec == 1 ) {
	this->cached_length_rec = this->_size_rec[0];
    }
    else if ( this->dim_size_rec == 0 ) {
	this->cached_length_rec = 0;
    }
    else {
	size_t i;
	size_t num_pixels = 1;
	for ( i=0 ; i < this->dim_size_rec ; i++ ) {
	    num_pixels *= this->_size_rec[i];
	}
	this->cached_length_rec = num_pixels;
    }

    /*
     * for this->col_length()
     */
    if ( 0 < this->dim_size_rec ) {
	this->cached_col_length_rec = this->_size_rec[0];
    }
    else {
	this->cached_col_length_rec = 0;
    }

    /*
     * for this->row_length()
     */
    if ( 1 < this->dim_size_rec ) {
	this->cached_row_length_rec = this->_size_rec[1];
    }
    else if ( 0 < this->dim_size_rec ) {
	this->cached_row_length_rec = 1;
    }
    else {
	this->cached_row_length_rec = 0;
    }

    /*
     * for this->layer_length()
     */
    if ( 2 < this->dim_size_rec ) {
	size_t i, sz = 1;
	for ( i=2 ; i < this->dim_size_rec ; i++ ) {
	    sz *= this->_size_rec[i];
	}
	this->cached_layer_length_rec = sz;
    }
    else if ( 0 < this->dim_size_rec ) {
	this->cached_layer_length_rec = 1;
    }
    else {
	this->cached_layer_length_rec = 0;
    }

    /*
     * for this->bytes()
     */
    this->cached_bytes_rec = zt2bytes(this->sz_type_rec);

    /* */
    this->cached_col_row_length_rec = 
	this->cached_col_length_rec * this->cached_row_length_rec;
    this->cached_row_layer_length_rec = 
	this->cached_row_length_rec * this->cached_layer_length_rec;

    /*
     * scan_along_{x,y,z} 用情報の初期化
     */
    this->end_scan_along_x();
    this->end_scan_along_y();
    this->end_scan_along_z();
    this->end_scan_zx_planes();
    this->end_scan_a_cube();


    /* 型変換のための関数を選択 (自身の型→double) */
    this->func_cnv_nd_x2d = NULL;
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
    if ( this->sz_type_rec == org_sz_type && DOUBLE_ZT == new_sz_type ) { \
	this->func_cnv_nd_x2d = &cnv_nd::fncname; \
    }
    SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(SEL_FUNC,,,,,,,,,,,,,,,,,,,,,,,,,,,,else);
#undef SEL_FUNC

    /* 型変換のための関数を選択 (double→自身の型) */
    this->func_cnv_nd_d2x = NULL;
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
    if ( DOUBLE_ZT == org_sz_type && this->sz_type_rec == new_sz_type ) { \
	this->func_cnv_nd_d2x = &cnv_nd::fncname; \
    }
    SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(SEL_FUNC,,,,,,,,,,,,,,,,,,,,,,,,,,,,else);
#undef SEL_FUNC

    /* 型変換のための関数を選択 (自身の型→float) */
    this->func_cnv_nd_x2f = NULL;
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
    if ( this->sz_type_rec == org_sz_type && FLOAT_ZT == new_sz_type ) { \
	this->func_cnv_nd_x2f = &cnv_nd::fncname; \
    }
    SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(SEL_FUNC,,,,,,,,,,,,,,,,,,,,,,,,,,,,else);
#undef SEL_FUNC


    /* 正方行列を反転させる関数を選択を選択 */
    this->func_transp_mtx = NULL;
#define SEL_FUNC(fncname,sztp,tp) \
    if ( this->sz_type_rec == sztp ) { \
	this->func_transp_mtx = &transp_sq::fncname; \
    }
    SLI__MDARRAY__DO_OPERATION_1TYPE_ALL(SEL_FUNC,else);
#undef SEL_FUNC

    /*
     * Call user-defined callback if required
     */
    /* call callback func */
    /*
    if ( this->length_change_notify_func != NULL ) {
	(*(this->length_change_notify_func))(this, 
					  this->length_change_notify_user_ptr);
    }
    */

    return;
}


/*
 * private member functions
 */

/**
 * @brief  配列の長の変更 (低レベル)
 * 
 *  自身が持つ配列の長さを変更します.
 * 
 * @param   dim_index 処理対象の次元番号
 * @param   len 変更後の要素数
 * @param   clr 初期化の有無の指定
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 * @note    このメンバ関数は private です．
 */
mdarray &mdarray::do_resize( size_t dim_index, size_t len, bool clr )
{
    const size_t bytes = this->bytes();
    size_t i;
    size_t unit, new_unit, old_unit;
    size_t count;

    //if ( len == 0 ) {
	/* 1次元の場合で，dim_index==0の場合 */
	//if ( this->dim_size_rec == 1 && dim_index == 0 ) {
	//    return this->decrease_dim();
	//}
	//else {
	//    err_report(__FUNCTION__,"WARNING","invalid len; ignored");
	//    return *this;
	//}
    //}

    while ( this->dim_size_rec <= dim_index ) {
	this->increase_dim();
    }
    if ( len == this->_size_rec[dim_index] ) {
	return *this;
    }

    unit = bytes;
    for ( i=0 ; i < dim_index ; i++ ) {
	unit *= this->_size_rec[i];
    }
    new_unit = unit * len;
    old_unit = unit * this->_size_rec[dim_index];
    count = 1;
    for ( i=dim_index+1 ; i < this->dim_size_rec ; i++ ) {
	count *= this->_size_rec[i];
    }
    
    /* 小さくなった場合 */
    if ( len < this->_size_rec[dim_index] ) {
	/* old |----|----|----| */
	/* new |--|--|--| */
	/* */
	/* step 1:  |----|====|----| */
	/*          |--|==|???|----| */
	/* */
	/* step 2:  |--|--|???|====| */
	/*          |--|--|==| */
	void *dptr = this->data_ptr();
	for ( i=1 ; i < count ; i++ ) {
	    s_memmove( (char *)dptr + new_unit * i,
		       (char *)dptr + old_unit * i,
		       new_unit);
	}
	size_t *srec_ptr = this->size_rec_ptr();
	srec_ptr[dim_index] = len;
	/* */
	if ( this->realloc_arr_rec(new_unit * count) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","realloc() failed");
	}
	if ( this->extptr_rec != NULL ) *(this->extptr_rec) = this->data_ptr();
    }
    /* 大きくなった場合 */
    else {
	/* old |--|--|--| */
	/* new |----|----|----| */
	/* */
	/* step 1:  |--|--|==| */
	/*          |--|--|????|==??| */
	/* */
	/* step 2:  |--|==|????|--??| */
	/*          |--???|==??|--??| */
	/* まず，再確保 */
	if ( this->realloc_arr_rec(new_unit * count) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","realloc() failed");
	}
	if ( this->extptr_rec != NULL ) *(this->extptr_rec) = this->data_ptr();
	/* */
	void *dptr = this->data_ptr();
	for ( i=count ; 1 < i ; ) {
	    i--;
	    s_memmove( (char *)dptr + new_unit * i,
		       (char *)dptr + old_unit * i,
		       old_unit);
	}

	if ( clr ) {
	    size_t n_spc;

	    /* 空いた部分を zero でうめたて */
	    n_spc = (new_unit - old_unit) / bytes;

	    if ( this->default_value_ptr() != NULL ) {
		char *begin_ptr = (char *)(this->data_ptr()) + old_unit;
		for ( i=0 ; i < count ; i++ ) {
		    s_memfill(begin_ptr, this->default_value_ptr(), bytes, n_spc,
			      n_spc * count);
		    begin_ptr += new_unit;
		}
	    }
	    else {
		char *begin_ptr = (char *)(this->data_ptr()) + old_unit;
		for ( i=0 ; i < count ; i++ ) {
		    s_memset(begin_ptr, 0, bytes * n_spc, 
			     bytes * n_spc * count);
		    begin_ptr += new_unit;
		}
	    }
	}

	size_t *srec_ptr = this->size_rec_ptr();
	srec_ptr[dim_index] = len;
    }

    /* update cached length info and call user's callback func */
    this->update_length_info();
    /* update_length_info() の直後に呼ぶ必要がある */
    this->update_arr_ptr_2d();
    this->update_arr_ptr_3d();

    return *this;
}

/**
 * @brief  要素の挿入 (低レベル)
 * 
 *  指定された次元の要素位置 idx に len 個の要素を挿入します．
 * 
 * @param   dim_index 処理対象の次元番号
 * @param   idx 挿入位置の要素番号
 * @param   len 挿入個数
 * @param   clr 初期化の有無の指定
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 * @note    このメンバ関数は private です．
 */
mdarray &mdarray::do_insert(size_t dim_index, ssize_t idx, size_t len, bool clr)
{
    const size_t bytes = this->bytes();
    size_t org_length = this->length(dim_index);
    size_t i, idx_0;
    size_t b_unit0, b_unit1, unit0;
    size_t count;

    if ( this->dim_size_rec <= dim_index ) {
	if ( dim_index == 0 && len <= 0 ) return *this;
	this->resize(dim_index, 1);
	org_length = 1;
    }

    if ( 0 <= idx && org_length <= abs_sz2z(idx) ) {
	len += (abs_sz2z(idx) - org_length);
	idx = org_length;
    }

    if ( idx < 0 ) {
	if ( abs_sz2z(idx) < len ) {
	    len -= abs_sz2z(idx);
	    idx = 0;
	} else {
	    return *this;
	}
    }

    if ( len <= 0 ) return *this;

    idx_0 = (size_t)idx;

    /* */
    this->do_resize(dim_index, org_length + len, false);

    unit0 = 1;
    for ( i=0 ; i < dim_index ; i++ ) {
	unit0 *= this->_size_rec[i];
    }
    b_unit0 = bytes * unit0;
    b_unit1 = b_unit0 * this->_size_rec[dim_index];

    count = 1;
    for ( i=dim_index+1 ; i < this->dim_size_rec ; i++ ) {
	count *= this->_size_rec[i];
    }

    void *dptr = this->data_ptr();
    for ( i=0 ; i < count ; i++ ) {
       s_memmove((char *)dptr + b_unit1 * i + b_unit0 * (idx_0 + len),
		 (char *)dptr + b_unit1 * i + b_unit0 * idx_0,
		 b_unit0 * (org_length - idx_0) );
       if ( clr ) {
	  if ( this->default_value_ptr() != NULL ) {
	      char *dest_ptr = (char *)dptr + b_unit1 * i + b_unit0 * idx_0;
	      s_memfill(dest_ptr, this->default_value_ptr(), bytes, unit0 * len,
			unit0 * len * count);
	  }
	  else {
	      char *dest_ptr = (char *)dptr + b_unit1 * i + b_unit0 * idx_0;
	      s_memset(dest_ptr, 0, b_unit0 * len, b_unit0 * len * count);
	  }
       }
    }

    return *this;
}


/**
 * @brief  mdarray どうしの演算 (再帰用)
 *
 *  自身と，与えられた mdarray とを，指定された演算用関数(func，funcf，funcl)に
 *  より再帰的に演算し，その結果を返す．
 *
 * @param      obj       演算する mdarray オブジェクト 
 * @param      obj_1st   演算時の obj 引数の位置．
 *                       つまり，x,yの扱い：func(x,y) or func(y,x)．<br>
 *                       true の場合は第１引数に obj が指定される
 * @param      dim_idx   現在の次元番号
 * @param      len_src   src についての 扱う次元内の長さ
 * @param      len_dest  dest についての 扱う次元内の長さ
 * @param      pos_src   src についての現在のポジション
 * @param      pos_dest  dest についての現在のポジション
 * @param      func      double 型の演算用関数へのアドレス
 * @param      funcf     float 型の演算用関数へのアドレス
 * @param      funcl     long double 型の演算用関数へのアドレス
 * @param      func_calc 統轄的に演算するための関数へのアドレス
 * @return     自身の参照
 * @note       この関数は再帰的に呼ばれます．<br>
 *             このメンバ関数は private です．
 */
mdarray &mdarray::r_calc2(const mdarray &obj, bool obj_1st, size_t dim_idx,
			  size_t len_src, size_t len_dest,
			  size_t pos_src, size_t pos_dest,
			  double (*func)(double,double),
			  float (*funcf)(float,float),
			  long double (*funcl)(long double,long double),
			  void (*func_calc)(const void *, void *, bool, size_t,
				   const void *, double (*)(double,double),
				   float (*)(float,float),
				   long double (*)(long double,long double)) )
{
    size_t len, len_large;
    len = this->length(dim_idx);
    len_large = len;
    if ( len_src == 0 ) len = 0;
    else if ( obj.length(dim_idx) < len ) len = obj.length(dim_idx);
    if ( 0 < dim_idx ) {
	size_t i;
	len_src /= obj.length(dim_idx);
	len_dest /= this->length(dim_idx);
	for ( i=0 ; i < len ; i++ ) {
	    this->r_calc2( obj, obj_1st, dim_idx-1, len_src, len_dest,
			   pos_src + len_src * i, pos_dest + len_dest * i,
			   func, funcf, funcl, func_calc);
	}
	/* default 値を用いた計算をする */
	for ( ; i < len_large ; i++ ) {
	    this->r_calc2( obj, obj_1st, dim_idx-1, 0, len_dest,
			   pos_src + 0 * i, pos_dest + len_dest * i,
			   func, funcf, funcl, func_calc);
	}
    }
    else {	/* 次元=0 での処理 */
	const size_t src_bytes = obj.bytes();
	const size_t dest_bytes = this->bytes();
	char *dest = (char *)(this->data_ptr()) + dest_bytes * pos_dest;
	const char *src =
	    (const char *)(obj.data_ptr_cs()) + src_bytes * pos_src;
	/* 関数の浮動小数点引数2つは対等なので，    */
	/* すべての組合せについて計算する必要がある */
	(*func_calc)((const void *)src, (void *)dest, obj_1st, len,
		     obj.default_value_ptr(), func,funcf,funcl);
	dest += dest_bytes * len;
	/* default 値を用いた計算をする */
	(*func_calc)(NULL, (void *)dest, obj_1st, len_large - len,
		     obj.default_value_ptr(), func,funcf,funcl);
    }
    return *this;
}

/**
 * @brief  mdarray どうしの演算 (再帰用)
 *
 *  自身と，与えられた mdarray オブジェクトとを，指定された演算用関数
 *  (func，funcf，funcl)により再帰的に演算し，その結果を返す．
 *
 * @param      obj       演算する mdarray オブジェクト 
 * @param      obj_1st   演算時の obj 引数の位置．
 *                       つまり，x,yの扱い：func(x,y) or func(y,x)．<br>
 *                       true の場合は第１引数に obj が指定される
 * @param      dim_idx   現在の次元番号
 * @param      len_src   src についての 扱う次元内の長さ
 * @param      len_dest  dest についての 扱う次元内の長さ
 * @param      pos_src   src についての現在のポジション
 * @param      pos_dest  dest についての現在のポジション
 * @param      func      dcomplex 型の演算用関数へのアドレス
 * @param      funcf     fcomplex 型の演算用関数へのアドレス
 * @param      funcl     ldcomplex 型の演算用関数へのアドレス
 * @param      func_calc 統轄的に演算するための関数へのアドレス
 * @return     自身の参照
 * @note       この関数は再帰的に呼ばれます．<br>
 *             このメンバ関数は private です．
 */
mdarray &mdarray::r_calcx2(const mdarray &obj, bool obj_1st, size_t dim_idx,
			   size_t len_src, size_t len_dest,
			   size_t pos_src, size_t pos_dest,
			   dcomplex (*func)(dcomplex,dcomplex),
			   fcomplex (*funcf)(fcomplex,fcomplex),
			   ldcomplex (*funcl)(ldcomplex,ldcomplex),
			   void (*func_calc)(const void *, void *, bool, size_t,
				   const void *, dcomplex (*)(dcomplex,dcomplex),
				   fcomplex (*)(fcomplex,fcomplex),
				   ldcomplex (*)(ldcomplex,ldcomplex)) )
{
    size_t len, len_large;
    len = this->length(dim_idx);
    len_large = len;
    if ( len_src == 0 ) len = 0;
    else if ( obj.length(dim_idx) < len ) len = obj.length(dim_idx);
    if ( 0 < dim_idx ) {
	size_t i;
	len_src /= obj.length(dim_idx);
	len_dest /= this->length(dim_idx);
	for ( i=0 ; i < len ; i++ ) {
	    this->r_calcx2( obj, obj_1st, dim_idx-1, len_src, len_dest,
			    pos_src + len_src * i, pos_dest + len_dest * i,
			    func, funcf, funcl, func_calc);
	}
	/* default 値を用いた計算をする */
	for ( ; i < len_large ; i++ ) {
	    this->r_calcx2( obj, obj_1st, dim_idx-1, 0, len_dest,
			    pos_src + 0 * i, pos_dest + len_dest * i,
			    func, funcf, funcl, func_calc);
	}
    }
    else {	/* 次元=0 での処理 */
	const size_t src_bytes = obj.bytes();
	const size_t dest_bytes = this->bytes();
	char *dest = (char *)(this->data_ptr()) + dest_bytes * pos_dest;
	const char *src =
	    (const char *)(obj.data_ptr_cs()) + src_bytes * pos_src;
	/* 関数の浮動小数点引数2つは対等なので，    */
	/* すべての組合せについて計算する必要がある */
	(*func_calc)((const void *)src, (void *)dest, obj_1st, len,
		     obj.default_value_ptr(), func,funcf,funcl);
	dest += dest_bytes * len;
	/* default 値を用いた計算をする */
	(*func_calc)(NULL, (void *)dest, obj_1st, len_large - len,
		     obj.default_value_ptr(), func,funcf,funcl);
    }
    return *this;
}

/* */

/**
 * @brief  secinfo に従って再帰的にセクションの flip を行なう (内部用)
 *
 *  flip_buf が non-null な場合，this_buf との入れ替えを行なう
 *
 * @note   private な関数です．
 */
static void flip_section_r( size_t bytes, void *this_buf, void *flip_buf,
			    const size_t *this_dim,
			    const heap_mem<mdarray_section_expinfo> &secinfo,
			    size_t dim_ix, size_t this_len_block )
{
    char *d_ptr = (char *)this_buf;
    char *s_ptr = (char *)flip_buf;
    if ( dim_ix == 0 ) {
	d_ptr += bytes * secinfo[0].begin;
	if ( s_ptr != NULL ) {
	    s_ptr += bytes * secinfo[0].begin;
	    s_memswap(d_ptr, s_ptr, bytes * secinfo[0].length);
	}
	else {	/* s_ptr == NULL */
	    if ( secinfo[0].flip_flag == true ) {
		s_memflip(d_ptr, bytes, secinfo[0].length);
	    }
	}
    }
    else {
	char *p0;
	size_t blen_blk, i;
	/* */
	this_len_block /= this_dim[dim_ix];
	blen_blk = bytes * this_len_block;
	/* */
	d_ptr += blen_blk * secinfo[dim_ix].begin;
	p0 = d_ptr;					/* 保存 */
	if ( s_ptr != NULL ) {
	    s_ptr += blen_blk * secinfo[dim_ix].begin;
	    for ( i=0 ; i < secinfo[dim_ix].length ; i++ ) {
		flip_section_r(bytes, d_ptr, s_ptr, this_dim,
			       secinfo, dim_ix - 1, this_len_block);
		s_ptr += blen_blk;
		d_ptr += blen_blk;
	    }
	}
	else {	/* s_ptr == NULL */
	    if ( secinfo[dim_ix].flip_flag == true ) {
		char *p1 = p0 + (blen_blk * secinfo[dim_ix].length);
		size_t hlen = secinfo[dim_ix].length / 2;
		for ( i=0 ; i < hlen ; i++ ) {
		    p1 -= blen_blk;
		    flip_section_r(bytes, p0, p1, this_dim,
				   secinfo, dim_ix - 1, this_len_block);
		    p0 += blen_blk;
		}
	    }
	    for ( i=0 ; i < secinfo[dim_ix].length ; i++ ) {
		flip_section_r(bytes, d_ptr, s_ptr, this_dim,
			       secinfo, dim_ix - 1, this_len_block);
		d_ptr += blen_blk;
	    }
	}
    }
    return;
}

/**
 * @brief  配列の一部(画像の一部)の反転 (低レベル)
 * @note   このメンバ関数は private です．
 */
mdarray &mdarray::image_vflipf( const char *exp_fmt, va_list ap )
{
    heap_mem<char> secexp;
    heap_mem<mdarray_section_expinfo> secinfo;
    heap_mem<size_t> naxisx;
    size_t n_secinfo = 0;
    size_t this_total_elements, i;

    if ( this->length() == 0 ) goto quit;

    if ( exp_fmt == NULL ) goto quit;

    if ( c_vasprintf(secexp.ptr_address(), exp_fmt, ap) < 0 ) {
	err_report(__FUNCTION__,"FATAL","c_vasprintf() failed");
	goto quit;
    }

    /* パースし，secinfo を生成 */
    if ( parse_section_exp(&secinfo, &n_secinfo, secexp.ptr()) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","Error in parse_section_exp()");
    }
    if ( n_secinfo == 0 ) goto quit;

    /* secinfo をチェック，修正 */
    if ( test_section_info( secexp.ptr(), *this, 
			    &secinfo, &n_secinfo, &naxisx ) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","Error in test_section_info()");
    }
    if ( n_secinfo == 0 ) goto quit;

    this_total_elements = 1;
    for ( i=0 ; i < n_secinfo ; i++ ) {
	this_total_elements *= this->length(i);
    }

    /* 再帰的に n 次元のセクションの flip を行なう */
    flip_section_r( this->bytes(), this->data_ptr(), NULL,
		    this->cdimarray(), secinfo, n_secinfo - 1,
		    this_total_elements );

 quit:
    return *this;
}


/**
 * @brief  再帰的にセクションの xy transpose copy を行なう (内部用)
 *
 * @note   private な関数です．
 */
static void transposef_xy_copy_section_r( const mdarray &src_obj, 
					  const void *src_buf, void *dest_buf, 
		  const heap_mem<mdarray_section_expinfo> &secinfo,
		  size_t total_elements, size_t dim_ix,
		  size_t src_len_block, size_t dest_len_block,
		  size_t block_len, void *_tmp_block,
		  void (*func_gencpy2d)(const void *, void *, 
		       size_t, size_t, size_t, size_t, size_t, size_t, void *),
		  void *user_ptr_func_gencpy2d,
		  void (*func_sq)(void *, size_t) )
{
    const size_t bytes = src_obj.bytes();
    const unsigned char *s_ptr = (const unsigned char *)src_buf;
    unsigned char *d_ptr = (unsigned char *)dest_buf;
    if ( dim_ix == 1 ) {
	do_transpose_xy_copy( s_ptr, d_ptr,
			      src_obj.size_type(), bytes, 
			      src_obj.size_type(), bytes, 
			      secinfo[0].begin, secinfo[1].begin, 
			      src_obj.col_length(), 1,
			      secinfo[1].length, secinfo[0].length,
			      block_len, _tmp_block,
			      func_gencpy2d, &user_ptr_func_gencpy2d, 
			      func_sq );
    }
    else {
	size_t i;
	/* */
	src_len_block /= src_obj.length(dim_ix);
	dest_len_block /= secinfo[dim_ix].length;
	/* */
	s_ptr += bytes * src_len_block * secinfo[dim_ix].begin;
	for ( i=0 ; i < secinfo[dim_ix].length ; i++ ) {
	    transposef_xy_copy_section_r( src_obj, s_ptr, d_ptr, 
				    secinfo, total_elements, dim_ix - 1,
				    src_len_block, dest_len_block,
				    block_len, _tmp_block,
				    func_gencpy2d, user_ptr_func_gencpy2d,
				    func_sq );
	    s_ptr += bytes * src_len_block;
	    d_ptr += bytes * dest_len_block;
	}
    }
    return;
}

/**
 * @brief  (x,y)→(y,x)トランスポーズ済配列を別オブジェクトへ格納(低レベル)
 * @note   このメンバ関数は private です．
 */
ssize_t mdarray::image_vtransposef_xy_copy( mdarray *dest_ret,
					const char *exp_fmt, va_list ap ) const
{
    ssize_t ret = -1;
    heap_mem<char> secexp;
    heap_mem<mdarray_section_expinfo> secinfo;
    heap_mem<size_t> naxisx;
    size_t n_secinfo = 0;
    size_t this_total_elements, sec_total_elements, i;
    mdarray tmpobj;
    mdarray *dest_obj;

    size_t col_size, row_size;
    heap_mem<unsigned char> tmp_block;		/* 正方行列用バッファ */
    size_t len_block, min_len_block, max_len_block, min_size;
    void (*func_gencpy2d)(const void *, void *, 
		       size_t, size_t, size_t, size_t, size_t, size_t, void *);
    size_t f_idx;

    if ( dest_ret == NULL || this->length() == 0 ) goto quit;

    if ( exp_fmt == NULL ) goto quit;

    if ( c_vasprintf(secexp.ptr_address(), exp_fmt, ap) < 0 ) {
	err_report(__FUNCTION__,"FATAL","c_vasprintf() failed");
	goto quit;
    }

    if ( parse_section_exp(&secinfo, &n_secinfo, secexp.ptr()) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","Error in parse_section_exp()");
    }
    if ( n_secinfo == 0 ) goto quit;

    if ( test_section_info( secexp.ptr(), *this, 
			    &secinfo, &n_secinfo, &naxisx ) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","Error in test_section_info()");
    }
    if ( n_secinfo == 0 ) goto quit;

    /* 最低2次元にする必要がある */
    if ( n_secinfo < 2 ) {
	if ( secinfo.reallocate(2) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","realloc() failed");
	}
	if ( naxisx.reallocate(2) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","realloc() failed");
	}
	secinfo[1].begin = 0;
	secinfo[1].length = 1;
	secinfo[1].flip_flag = 0;
	naxisx[1] = 1;
	n_secinfo = 2;
    }
    col_size = secinfo[0].length;
    row_size = secinfo[1].length;

    if ( dest_ret == this ) dest_obj = &tmpobj;
    else dest_obj = dest_ret;

    {
	size_t len_dest = 1;
	size_t tmp_len;
	tmp_len = naxisx[0];  naxisx[0] = naxisx[1];  naxisx[1] = tmp_len;
	for ( i=0 ; i < n_secinfo ; i++ ) len_dest *= naxisx[i];
	/* 型と全配列長が等しい場合は，dest_objの現在のバッファを */
	/* そのまま使う                                           */
	if ( dest_obj->size_type() == this->size_type() &&
	     dest_obj->length() == len_dest ) {
	    /* 全配列長が同じなので，ここでは配列情報だけ更新される */
	    dest_obj->reallocate(naxisx.ptr(), n_secinfo, false);
	}
	else {
	    dest_obj->init(this->size_type(), true, 
			   naxisx.ptr(), n_secinfo, false);
	}
	dest_obj->init_properties(*this);
	dest_obj->assign_default(this->default_value_ptr());
    }


    /* 正方行列バッファのバイト長が 16^2 から 256^2 の値をとるように設定 */
    min_len_block = (size_t)(1.0 + sqrt((double)(16*16) / this->bytes()));
    max_len_block = (size_t)(1.0 + sqrt((double)(256*256) / this->bytes()));
    if ( row_size < col_size ) min_size = row_size;
    else min_size = col_size;

    if ( min_size <= min_len_block ) len_block = min_len_block;
    else {
	size_t ndiv = ( (min_size - 1) / max_len_block ) + 1;
	len_block = min_size / ndiv;
    }

    /* 正方行列バッファ(一時バッファ)を確保 */
    if ( tmp_block.allocate(this->bytes() * len_block * len_block) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","malloc() failed");
    }

    /* transpose 用の汎用コピー関数を選択 */
    func_gencpy2d = NULL;
#define SEL_FUNC(fncname,src_sztp,src_tp,dest_sztp,dest_tp,fnc) \
    if ( this->size_type() == src_sztp && this->size_type() == dest_sztp ) { \
	func_gencpy2d = &gencpy2d::fncname; \
    }
    SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(SEL_FUNC,,,,,,,,,,,,,,,,,,,,,,,,,,,,else);
#undef SEL_FUNC

    if ( this->rounding() == true ) f_idx = 1;		/* 四捨五入で変換 */
    else f_idx = 0;					/* 切り捨てで変換 */


    this_total_elements = 1;
    for ( i=0 ; i < n_secinfo ; i++ ) {
	this_total_elements *= this->length(i);
    }
    sec_total_elements = 1;
    for ( i=0 ; i < n_secinfo ; i++ ) {
	sec_total_elements *= secinfo[i].length;
    }

    /* 再帰的に n 次元のセクションの xy transpose copy を行なう */
    transposef_xy_copy_section_r(*this, this->data_ptr_cs(), dest_obj->data_ptr(),
				  secinfo, sec_total_elements, n_secinfo - 1,
				  this_total_elements, sec_total_elements,
				  len_block, tmp_block.ptr(),
				  func_gencpy2d, &f_idx, 
				  this->func_transp_mtx );

    /* 画像反転処理を行なう */
    for ( i=0 ; i < n_secinfo ; i++ ) {
	if ( secinfo[i].flip_flag == true ) {
	    size_t ix;
	    if ( i == 0 ) ix = 1;
	    else if ( i == 1 ) ix = 0;
	    else ix = i;
	    dest_obj->flip(ix, 0, dest_obj->length(ix));
	}
    }

    /* 自分自身の場合 */
    if ( dest_ret == this ) {
	dest_ret->swap(tmpobj);
    }

    ret = sec_total_elements;

 quit:
    return ret;
}


/**
 * @brief  再帰的にセクションの xyz→zxy の transpose copy を行なう (内部用)
 *
 * @note   private な関数です．
 */
static void transposef_xyz2zxy_copy_section_r( const mdarray &src_obj, 
					  const void *src_buf, void *dest_buf, 
		  const heap_mem<mdarray_section_expinfo> &secinfo,
		  size_t total_elements, size_t dim_ix,
		  size_t src_len_block, size_t dest_len_block,
		  size_t block_len, void *_tmp_block,
		  void (*func_gencpy2d)(const void *, void *, 
		       size_t, size_t, size_t, size_t, size_t, size_t, void *),
		  void *user_ptr_func_gencpy2d,
		  void (*func_sq)(void *, size_t) )
{
    const size_t bytes = src_obj.bytes();
    const unsigned char *s_ptr = (const unsigned char *)src_buf;
    unsigned char *d_ptr = (unsigned char *)dest_buf;
    if ( dim_ix == 2 ) {
	size_t i;
	src_len_block /= src_obj.length(dim_ix);
	s_ptr += bytes * src_len_block * secinfo[dim_ix].begin;
	for ( i=0 ; i < secinfo[1].length ; i++ ) {
	    do_transpose_xy_copy( s_ptr, d_ptr,
			      src_obj.size_type(), bytes, 
			      src_obj.size_type(), bytes, 
			      secinfo[0].begin, secinfo[1].begin + i, 
			      src_obj.col_length(), src_obj.row_length(),
			      secinfo[2].length, secinfo[0].length,
			      block_len, _tmp_block,
			      func_gencpy2d, &user_ptr_func_gencpy2d, 
			      func_sq );
	    d_ptr += bytes * secinfo[2].length * secinfo[0].length;
	}
    }
    else {
	size_t i;
	/* */
	src_len_block /= src_obj.length(dim_ix);
	dest_len_block /= secinfo[dim_ix].length;
	/* */
	s_ptr += bytes * src_len_block * secinfo[dim_ix].begin;
	for ( i=0 ; i < secinfo[dim_ix].length ; i++ ) {
	    transposef_xyz2zxy_copy_section_r( src_obj, s_ptr, d_ptr, 
				    secinfo, total_elements, dim_ix - 1,
				    src_len_block, dest_len_block,
				    block_len, _tmp_block,
				    func_gencpy2d, user_ptr_func_gencpy2d,
				    func_sq );
	    s_ptr += bytes * src_len_block;
	    d_ptr += bytes * dest_len_block;
	}
    }
    return;
}

/**
 * @brief  (x,y,z)→(z,x,y)トランスポーズ済配列を別オブジェクトへ格納(低レベル)
 * @note   このメンバ関数は private です．
 */
ssize_t mdarray::image_vtransposef_xyz2zxy_copy( mdarray *dest_ret,
					const char *exp_fmt, va_list ap ) const
{
    ssize_t ret = -1;
    heap_mem<char> secexp;
    heap_mem<mdarray_section_expinfo> secinfo;
    heap_mem<size_t> naxisx;
    size_t n_secinfo = 0;
    size_t this_total_elements, sec_total_elements, i;
    mdarray tmpobj;
    mdarray *dest_obj;

    size_t col_size, row_size, layer_size;
    heap_mem<unsigned char> tmp_block;		/* 正方行列用バッファ */
    size_t len_block, min_len_block, max_len_block, min_size;
    void (*func_gencpy2d)(const void *, void *, 
		       size_t, size_t, size_t, size_t, size_t, size_t, void *);
    size_t f_idx;

    if ( dest_ret == NULL || this->length() == 0 ) goto quit;

    if ( exp_fmt == NULL ) goto quit;

    if ( c_vasprintf(secexp.ptr_address(), exp_fmt, ap) < 0 ) {
	err_report(__FUNCTION__,"FATAL","c_vasprintf() failed");
	goto quit;
    }

    if ( parse_section_exp(&secinfo, &n_secinfo, secexp.ptr()) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","Error in parse_section_exp()");
    }
    if ( n_secinfo == 0 ) goto quit;

    if ( test_section_info( secexp.ptr(), *this, 
			    &secinfo, &n_secinfo, &naxisx ) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","Error in test_section_info()");
    }
    if ( n_secinfo == 0 ) goto quit;

    /* 最低3次元にする必要がある */
    if ( n_secinfo < 3 ) {
	if ( secinfo.reallocate(3) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","realloc() failed");
	}
	if ( naxisx.reallocate(3) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","realloc() failed");
	}
	secinfo[1].begin = 0;
	secinfo[1].length = 1;
	secinfo[1].flip_flag = 0;
	naxisx[1] = 1;
	secinfo[2].begin = 0;
	secinfo[2].length = 1;
	secinfo[2].flip_flag = 0;
	naxisx[2] = 1;
	n_secinfo = 3;
    }
    col_size = secinfo[0].length;
    row_size = secinfo[1].length;
    layer_size = secinfo[2].length;

    if ( dest_ret == this ) dest_obj = &tmpobj;
    else dest_obj = dest_ret;

    {
	size_t len_dest = 1;
	naxisx[0] = layer_size;
	naxisx[1] = col_size;
	naxisx[2] = row_size;
	for ( i=0 ; i < n_secinfo ; i++ ) len_dest *= naxisx[i];
	/* 型と全配列長が等しい場合は，dest_objの現在のバッファを */
	/* そのまま使う                                           */
	if ( dest_obj->size_type() == this->size_type() &&
	     dest_obj->length() == len_dest ) {
	    /* 全配列長が同じなので，ここでは配列情報だけ更新される */
	    dest_obj->reallocate(naxisx.ptr(), n_secinfo, false);
	}
	else {
	    dest_obj->init(this->size_type(), true, 
			   naxisx.ptr(), n_secinfo, false);
	}
	dest_obj->init_properties(*this);
	dest_obj->assign_default(this->default_value_ptr());
    }


    /* 正方行列バッファのバイト長が 16^2 から 256^2 の値をとるように設定 */
    min_len_block = (size_t)(1.0 + sqrt((double)(16*16) / this->bytes()));
    max_len_block = (size_t)(1.0 + sqrt((double)(256*256) / this->bytes()));
    if ( layer_size < col_size ) min_size = layer_size;
    else min_size = col_size;

    if ( min_size <= min_len_block ) len_block = min_len_block;
    else {
	size_t ndiv = ( (min_size - 1) / max_len_block ) + 1;
	len_block = min_size / ndiv;
    }

    /* 正方行列バッファ(一時バッファ)を確保 */
    if ( tmp_block.allocate(this->bytes() * len_block * len_block) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","malloc() failed");
    }

    /* transpose 用の汎用コピー関数を選択 */
    func_gencpy2d = NULL;
#define SEL_FUNC(fncname,src_sztp,src_tp,dest_sztp,dest_tp,fnc) \
    if ( this->size_type() == src_sztp && this->size_type() == dest_sztp ) { \
	func_gencpy2d = &gencpy2d::fncname; \
    }
    SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(SEL_FUNC,,,,,,,,,,,,,,,,,,,,,,,,,,,,else);
#undef SEL_FUNC

    if ( this->rounding() == true ) f_idx = 1;		/* 四捨五入で変換 */
    else f_idx = 0;					/* 切り捨てで変換 */


    this_total_elements = 1;
    for ( i=0 ; i < n_secinfo ; i++ ) {
	this_total_elements *= this->length(i);
    }
    sec_total_elements = 1;
    for ( i=0 ; i < n_secinfo ; i++ ) {
	sec_total_elements *= secinfo[i].length;
    }

    /* 再帰的に n 次元のセクションの xz transpose copy を行なう */
    transposef_xyz2zxy_copy_section_r(*this, this->data_ptr_cs(), dest_obj->data_ptr(),
				  secinfo, sec_total_elements, n_secinfo - 1,
				  this_total_elements, sec_total_elements,
				  len_block, tmp_block.ptr(),
				  func_gencpy2d, &f_idx, 
				  this->func_transp_mtx );

    /* 画像反転処理を行なう */
    for ( i=0 ; i < n_secinfo ; i++ ) {
	if ( secinfo[i].flip_flag == true ) {
	    size_t ix;
	    if ( i == 0 ) ix = 2;
	    else if ( i == 1 ) ix = 0;
	    else if ( i == 2 ) ix = 1;
	    else ix = i;
	    dest_obj->flip(ix, 0, dest_obj->length(ix));
	}
    }

    /* 自分自身の場合 */
    if ( dest_ret == this ) {
	dest_ret->swap(tmpobj);
    }

    ret = sec_total_elements;

 quit:
    return ret;
}


/**
 * @brief  secinfo に従って再帰的にセクションの clean を行なう (内部用)
 *
 * @note   private な関数です．
 */
static void clean_section_r( const void *value_ptr, size_t bytes, 
			     void *this_buf, const size_t *this_dim,
			     const heap_mem<mdarray_section_expinfo> &secinfo,
			     size_t total_elements, size_t dim_ix,
			     size_t this_len_block )
{
    char *d_ptr = (char *)this_buf;
    if ( dim_ix == 0 ) {
	d_ptr += bytes * secinfo[0].begin;
	if ( value_ptr != NULL ) {
	    s_memfill(d_ptr, value_ptr, bytes, secinfo[0].length, 
		      bytes * total_elements);
	}
	else {
	    s_memset(d_ptr, 0, bytes * secinfo[0].length, 
		     bytes * total_elements);
	}
    }
    else {
	size_t i;
	/* */
	this_len_block /= this_dim[dim_ix];
	/* */
	d_ptr += bytes * this_len_block * secinfo[dim_ix].begin;
	for ( i=0 ; i < secinfo[dim_ix].length ; i++ ) {
	    clean_section_r(value_ptr, bytes, d_ptr, this_dim,
			    secinfo, total_elements, dim_ix - 1,
			    this_len_block);
	    d_ptr += bytes * this_len_block;
	}
    }
    return;
}

/**
 * @brief  既存の配列要素を1つのデータでパディング (低レベル)
 * @note   このメンバ関数は private です．
 */
mdarray &mdarray::image_vcleanf( const void *value_ptr,
				 const char *exp_fmt, va_list ap )
{
    heap_mem<char> secexp;
    heap_mem<mdarray_section_expinfo> secinfo;
    heap_mem<size_t> naxisx;
    size_t n_secinfo = 0;
    size_t this_total_elements, sec_total_elements, i;

    if ( this->length() == 0 ) goto quit;

    if ( exp_fmt == NULL ) goto quit;

    if ( c_vasprintf(secexp.ptr_address(), exp_fmt, ap) < 0 ) {
	err_report(__FUNCTION__,"FATAL","c_vasprintf() failed");
	goto quit;
    }

    /* パースし，secinfo を生成 */
    if ( parse_section_exp(&secinfo, &n_secinfo, secexp.ptr()) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","Error in parse_section_exp()");
    }
    if ( n_secinfo == 0 ) goto quit;

    /* secinfo をチェック，修正 */
    if ( test_section_info( secexp.ptr(), *this, 
			    &secinfo, &n_secinfo, &naxisx ) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","Error in test_section_info()");
    }
    if ( n_secinfo == 0 ) goto quit;

    this_total_elements = 1;
    for ( i=0 ; i < n_secinfo ; i++ ) {
	this_total_elements *= this->length(i);
    }
    sec_total_elements = 1;
    for ( i=0 ; i < n_secinfo ; i++ ) {
	sec_total_elements *= secinfo[i].length;
    }

    /* 再帰的に n 次元のセクションの clean を行なう */
    clean_section_r( value_ptr, this->bytes(),
		     this->data_ptr(), this->cdimarray(),
		     secinfo, sec_total_elements, n_secinfo - 1,
		     this_total_elements );

 quit:
    return *this;
}

/**
 * @brief  既存の配列要素を1つのデータでパディング (低レベル)
 * 
 * @param   value_ptr データの格納場所
 * @param   col_index 列位置
 * @param   col_size 列サイズ
 * @param   row_index 行位置
 * @param   row_size 行サイズ
 * @param   layer_index レイヤ位置
 * @param   layer_size レイヤサイズ
 * @return  自身の参照
 * @note    このメンバ関数は private です．
 */
mdarray &mdarray::image_clean( const void *value_ptr,
			       ssize_t col_index, size_t col_size,
			       ssize_t row_index, size_t row_size,
			       ssize_t layer_index, size_t layer_size )
{
    const size_t this_bytes = this->bytes();
    size_t j, k;
    size_t col_blen, colrow_blen;
    unsigned char *k_ptr;
    unsigned char *j_ptr;

    if ( this->length() == 0 ) goto quit;

    if ( this->fix_section_args( &col_index, &col_size, 
				 &row_index, &row_size,
				 &layer_index, &layer_size ) < 0 ) goto quit;

    col_blen = this_bytes * this->col_length();
    colrow_blen = col_blen * this->row_length();

    k_ptr = (unsigned char *)(this->data_ptr()) 
	    + colrow_blen * layer_index			/* offset */
	    + col_blen * row_index
	    + this_bytes * col_index;
    for ( k=0 ; k < layer_size ; k++ ) {
	j_ptr = k_ptr;
	for ( j=0 ; j < row_size ; j++ ) {
	    if ( value_ptr != NULL ) {
		s_memfill(j_ptr, value_ptr, this_bytes, col_size,
			  col_size * row_size * layer_size);
	    }
	    else {
		s_memset(j_ptr, 0, this_bytes * col_size,
			 this_bytes * col_size * row_size * layer_size);
	    }
	    j_ptr += col_blen;
	}
	k_ptr += colrow_blen;
    }

 quit:
    return *this;
}


/**
 * @brief  secinfo に従って再帰的にセクションのコピーを行なう (内部用)
 *
 * @note   private な関数です．
 */
static void copy_or_move_section_r( bool same_buf, size_t bytes, 
				  const void *src_buf, const size_t *src_dim,
				  void *dest_buf, 
				  const heap_mem<mdarray_section_expinfo> &secinfo,
				  size_t total_elements, size_t dim_ix,
				  size_t src_len_block, size_t dest_len_block )
{
    const unsigned char *s_ptr = (const unsigned char *)src_buf;
    unsigned char *d_ptr = (unsigned char *)dest_buf;
    if ( dim_ix == 0 ) {
	s_ptr += bytes * secinfo[0].begin;
	if ( same_buf == true ) {
	    s_memmove(d_ptr, s_ptr, bytes * secinfo[0].length);
	}
	else {
	    s_memcpy(d_ptr, s_ptr, bytes * secinfo[0].length, 
		     bytes * total_elements);
	}
    }
    else {
	size_t i;
	/* */
	src_len_block /= src_dim[dim_ix];
	dest_len_block /= secinfo[dim_ix].length;
	/* */
	s_ptr += bytes * src_len_block * secinfo[dim_ix].begin;
	for ( i=0 ; i < secinfo[dim_ix].length ; i++ ) {
	    copy_or_move_section_r(same_buf, bytes, s_ptr, src_dim,
				   d_ptr, secinfo, total_elements, dim_ix - 1,
				   src_len_block, dest_len_block);
	    s_ptr += bytes * src_len_block;
	    d_ptr += bytes * dest_len_block;
	}
    }
    return;
}

/**
 * @brief  配列の一部を別オブジェクトにコピー (低レベル)
 * @note   このメンバ関数は private です．
 */
ssize_t mdarray::image_vcopyf( mdarray *dest_obj,
			       const char *exp_fmt, va_list ap ) const
{
    ssize_t ret = -1;
    heap_mem<char> secexp;
    heap_mem<mdarray_section_expinfo> secinfo;
    heap_mem<size_t> naxisx;
    size_t n_secinfo = 0;
    size_t this_total_elements, sec_total_elements, i;

    if ( this->length() == 0 ) goto quit;

    if ( exp_fmt == NULL ) goto quit;

    if ( c_vasprintf(secexp.ptr_address(), exp_fmt, ap) < 0 ) {
	err_report(__FUNCTION__,"FATAL","c_vasprintf() failed");
	goto quit;
    }

    if ( parse_section_exp(&secinfo, &n_secinfo, secexp.ptr()) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","Error in parse_section_exp()");
    }
    if ( n_secinfo == 0 ) goto quit;

#if 0
    {
      err_report1(__FUNCTION__,"DEBUG","n_secinfo = %zu", n_secinfo);
      for ( i=0 ; i < n_secinfo ; i++ ) {
	err_report(__FUNCTION__,"DEBUG","====");
	err_report1(__FUNCTION__,"DEBUG","begin = %zd", secinfo[i].begin);
	err_report1(__FUNCTION__,"DEBUG","length = %zu", secinfo[i].length);
	err_report1(__FUNCTION__,"DEBUG","flag = %d", secinfo[i].flip_flag);
      }
    }
#endif

    if ( test_section_info( secexp.ptr(), *this, 
			    &secinfo, &n_secinfo, &naxisx ) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","Error in test_section_info()");
    }
    if ( n_secinfo == 0 ) goto quit;

#if 0
    {
      err_report1(__FUNCTION__,"DEBUG","n_secinfo = %zu", n_secinfo);
      for ( i=0 ; i < n_secinfo ; i++ ) {
	err_report(__FUNCTION__,"DEBUG","====");
	err_report1(__FUNCTION__,"DEBUG","begin = %zd", secinfo[i].begin);
	err_report1(__FUNCTION__,"DEBUG","length = %zu", secinfo[i].length);
	err_report1(__FUNCTION__,"DEBUG","flag = %d", secinfo[i].flip_flag);
      }
    }
#endif

    /* 自分自身では無い場合 */
    if ( dest_obj != NULL && dest_obj != this ) {
	size_t len_dest = 1;
	for ( i=0 ; i < n_secinfo ; i++ ) len_dest *= naxisx[i];
	/* 型と全配列長が等しい場合は，dest_objの現在のバッファを */
	/* そのまま使う                                           */
	if ( dest_obj->size_type() == this->size_type() &&
	     dest_obj->length() == len_dest ) {
	    /* 全配列長が同じなので，ここでは配列情報だけ更新される */
	    dest_obj->reallocate(naxisx.ptr(), n_secinfo, false);
	}
	else {
	    dest_obj->init(this->size_type(), true, 
			   naxisx.ptr(), n_secinfo, false);
	}
	dest_obj->init_properties(*this);
	dest_obj->assign_default(this->default_value_ptr());
    }

    this_total_elements = 1;
    for ( i=0 ; i < n_secinfo ; i++ ) {
	this_total_elements *= this->length(i);
    }
    sec_total_elements = 1;
    for ( i=0 ; i < n_secinfo ; i++ ) {
	sec_total_elements *= secinfo[i].length;
    }

    /* 再帰的に n 次元のセクションのコピーを行なう */
    if ( dest_obj != NULL ) {
	copy_or_move_section_r( (dest_obj == this), this->bytes(),
				this->data_ptr_cs(), this->cdimarray(),
				dest_obj->data_ptr(), 
				secinfo, sec_total_elements, n_secinfo - 1,
				this_total_elements, sec_total_elements);
    }

    /* 自分自身の場合 */
    if ( dest_obj == this ) {
	dest_obj->reallocate(naxisx.ptr(), n_secinfo, false);
    }

    /* 画像反転処理を行なう */
    if ( dest_obj != NULL ) {
	for ( i=0 ; i < n_secinfo ; i++ ) {
	    if ( secinfo[i].flip_flag == true ) {
		dest_obj->flip(i, 0, dest_obj->length(i));
	    }
	}
    }

    ret = sec_total_elements;

 quit:
    return ret;
}


/**
 * @brief  配列の一部を別オブジェクトにコピー (低レベル)
 * 
 * @param  dest_obj コピー先のオブジェクト
 * @param  col_index コピー元の列の位置
 * @param  col_size コピー元の列サイズ
 * @param  row_index コピー元の行位置
 * @param  row_size コピー元の行サイズ
 * @param  layer_index コピー元のレイヤ位置
 * @param  layer_size コピー元のレイヤサイズ
 * @return コピーした要素数
 * @throw  バッファの確保に失敗した場合，メモリ破壊を起こした場合
 * @note   このメンバ関数は private です．
 */
ssize_t mdarray::image_copy( mdarray *dest_obj,
			     ssize_t col_index, size_t col_size, 
			     ssize_t row_index, size_t row_size,
			     ssize_t layer_index, size_t layer_size ) const
{
    const size_t this_bytes = this->bytes();
    const bool layer_all = (layer_index == 0 && layer_size == MDARRAY_ALL);
    ssize_t ret = -1;
    bool col_ok, row_ok, layer_ok;
    size_t i, j;
    const char *j_src_ptr;
    const char *i_src_ptr;
    char *dest_ptr;
    heap_mem<size_t> tmp_size;
    size_t tmp_dim_size;

    if ( this->length() == 0 ) goto quit;

    if ( this->test_section_args( &col_index, &col_size, 
				 &row_index, &row_size,
				 &layer_index, &layer_size,
				 &col_ok, &row_ok, &layer_ok ) < 0 ) goto quit;

    /* 元の次元情報をコピー */
    tmp_dim_size = this->dim_size_rec;
    tmp_size = dup_size_rec(this->cdimarray(), this->dim_size_rec);
    if ( tmp_size.ptr() == NULL ) {
	err_throw(__FUNCTION__,"FATAL","malloc() failed");
    }

    /* 自分自身では無い場合 */
    if ( dest_obj != NULL && dest_obj != this ) {
	if ( row_ok == false ) {			/* 1 次元 */
	    const size_t naxisx[1] = {col_size};
	    const size_t ndim = 1;
	    if ( dest_obj->size_type() == this->size_type() &&
		 dest_obj->length() == col_size ) {
		dest_obj->reallocate(naxisx, ndim, false);
	    }
	    else dest_obj->init(this->size_type(), true, naxisx, ndim, false);
	}
	else if ( layer_ok == false ) {			/* 2 次元 */
	    const size_t naxisx[2] = {col_size,row_size};
	    const size_t ndim = 2;
	    if ( dest_obj->size_type() == this->size_type() &&
		 dest_obj->length() == col_size * row_size ) {
		dest_obj->reallocate(naxisx, ndim, false);
	    }
	    else dest_obj->init(this->size_type(), true, naxisx, ndim, false);
	}
	else {						/* 3 次元以上 */
	    /* 4次元以上の場合，全レイヤ指定なら4次元以降の軸情報を残す */
	    if ( 3 < this->dim_length() && layer_all == true ) {
		size_t len_dest = 1;
		/* 1〜2次元だけ変更 */
		tmp_size[0] = col_size;
		tmp_size[1] = row_size;
		for ( i=0 ; i < tmp_dim_size ; i++ ) len_dest *= tmp_size[i];
		if ( dest_obj->size_type() == this->size_type() &&
		     dest_obj->length() == len_dest ) {
		    dest_obj->reallocate(tmp_size.ptr(), tmp_dim_size, false);
		}
		else {
		    dest_obj->init(this->size_type(), true, 
				   tmp_size.ptr(), tmp_dim_size, false);
		}
	    }
	    else {
		const size_t naxisx[3] = {col_size,row_size,layer_size};
		const size_t ndim = 3;
		if ( dest_obj->size_type() == this->size_type() &&
		     dest_obj->length() == col_size * row_size * layer_size ) {
		    dest_obj->reallocate(naxisx, ndim, false);
		}
		else dest_obj->init(this->size_type(), true, 
				    naxisx, ndim, false);
	    }
	}
	dest_obj->init_properties(*this);
	dest_obj->assign_default(this->default_value_ptr());
    }

    if ( dest_obj != NULL ) {
	const size_t col_blen = this_bytes * this->col_length();
	const size_t colrow_blen = col_blen * this->row_length();
	const size_t dest_line_unit = this_bytes * col_size;

	/* dest_obj が自分自身の場合，データを前に移動させる事になる */
	dest_ptr = (char *)dest_obj->data_ptr();
	j_src_ptr = (const char *)(this->data_ptr_cs()) 
		    + colrow_blen * layer_index		/* offset */
		    + col_blen * row_index
		    + this_bytes * col_index;
	for ( j=0 ; j < layer_size ; j++ ) {
	    i_src_ptr = j_src_ptr;
	    for ( i=0 ; i < row_size ; i++ ) {
		if ( dest_obj == this ) 
		    s_memmove( dest_ptr, i_src_ptr, dest_line_unit );
		else 
		    s_memcpy( dest_ptr, i_src_ptr, dest_line_unit,
			      dest_line_unit * row_size * layer_size );
		dest_ptr += dest_line_unit;
		i_src_ptr += col_blen;
	    }
	    j_src_ptr += colrow_blen;
	}
    }

    /* 自分自身の場合 */
    if ( dest_obj == this ) {
	if ( row_ok == false ) {		/* 1 次元 */
	    const size_t naxisx[1] = {col_size};
	    const size_t ndim = 1;
	    dest_obj->reallocate(naxisx, ndim, false);
	}
	else if ( layer_ok == false ) {		/* 2 次元 */
	    const size_t naxisx[2] = {col_size,row_size};
	    const size_t ndim = 2;
	    dest_obj->reallocate(naxisx, ndim, false);
	}
	else {					/* 3 次元 */
	    /* 4次元以上の場合，全レイヤ指定なら4次元以降の軸情報を残す */
	    if ( 3 < this->dim_length() && layer_all == true ) {
		/* 1〜2次元だけ変更 */
		tmp_size[0] = col_size;
		tmp_size[1] = row_size;
		dest_obj->reallocate(tmp_size.ptr(), tmp_dim_size, false);
	    }
	    else {
		const size_t naxisx[3] = {col_size,row_size,layer_size};
		const size_t ndim = 3;
		dest_obj->reallocate(naxisx, ndim, false);
	    }
	}
    }

    ret = layer_size * row_size * col_size;

 quit:
    return ret;
}


/**
 * @brief  配列の一部分を指定された値で書き換え (低レベル・高速版) (内部用)
 *
 *  secinfo に従って再帰的にセクションの fill を行なう
 *
 * @note   private な関数です．
 */
static void fill_section_r( double value, 
	void (*func)(const void *,void *,size_t,int,void *), void *user_ptr,
	void *this_buf, mdarray *thisp,
	const heap_mem<mdarray_section_expinfo> &secinfo,
	size_t dim_ix,
	size_t this_len_block )
{
    const size_t bytes = thisp->bytes();
    const size_t *this_dim = thisp->cdimarray();
    char *d_ptr = (char *)this_buf;
    if ( dim_ix == 0 ) {
	d_ptr += bytes * secinfo[0].begin;
	/* */
	(*func)((const void *)&value, (void *)d_ptr,
		secinfo[0].length, 0, user_ptr);
	/* */
    }
    else {
	size_t i;
	this_len_block /= this_dim[dim_ix];
	/* */
	d_ptr += bytes * this_len_block * secinfo[dim_ix].begin;
	for ( i=0 ; i < secinfo[dim_ix].length ; i++ ) {
	    fill_section_r( value, func, user_ptr, d_ptr, thisp,
			    secinfo, dim_ix - 1,
			    this_len_block );
	    d_ptr += bytes * this_len_block;
	}
    }
    return;
}

/**
 * @brief  配列の一部分を指定された値で書き換え (低レベル・高速版)
 * @note   このメンバ関数は private です．
 */
mdarray &mdarray::image_vfillf( double value,
	void (*func)(const void *,void *,size_t,int,void *), void *user_ptr,
	const char *exp_fmt, va_list ap )
{
    struct mdarray_cnv_nd_prms prms;
    heap_mem<char> secexp;
    heap_mem<mdarray_section_expinfo> secinfo;
    heap_mem<size_t> naxisx;
    size_t n_secinfo = 0;
    size_t this_total_elements, sec_total_elements, i;

    if ( this->length() == 0 ) goto quit;
    if ( exp_fmt == NULL ) goto quit;

    prms.total_len_elements = 0;			/* 一応初期化 */
    prms.round_flg = this->rounding();			/* 四捨五入/切り捨て */

    if ( func == NULL ) {

	func = this->func_cnv_nd_d2x;
	user_ptr = (void *)&prms;

	if ( func == NULL ) {
	    err_report(__FUNCTION__,"WARNING","unsupported sz_type; do nothing");
	    goto quit;
	}
    }

    if ( c_vasprintf(secexp.ptr_address(), exp_fmt, ap) < 0 ) {
	err_report(__FUNCTION__,"FATAL","c_vasprintf() failed");
	goto quit;
    }

    /* パースし，secinfo を生成 */
    if ( parse_section_exp(&secinfo, &n_secinfo, secexp.ptr()) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","Error in parse_section_exp()");
    }
    if ( n_secinfo == 0 ) goto quit;

    /* secinfo をチェック，修正 */
    if ( test_section_info( secexp.ptr(), *this, 
			    &secinfo, &n_secinfo, &naxisx ) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","Error in test_section_info()");
    }
    if ( n_secinfo == 0 ) goto quit;

    this_total_elements = 1;
    for ( i=0 ; i < n_secinfo ; i++ ) {
	this_total_elements *= this->length(i);
    }
    sec_total_elements = 1;
    for ( i=0 ; i < n_secinfo ; i++ ) {
	sec_total_elements *= secinfo[i].length;
    }

    prms.total_len_elements = sec_total_elements;		/* SSE2 用 */

    /* 再帰的に n 次元のセクションの fill を行なう */
    fill_section_r( value, func, user_ptr, this->data_ptr(), this,
		    secinfo, n_secinfo - 1, this_total_elements );

 quit:
    return *this;
}


/**
 * @brief  配列の一部分を指定された値で書き換え (低レベル) (内部用)
 *
 *  secinfo に従って再帰的にセクションの fill を行なう
 *
 * @note   private な関数です．
 */
static void fill_section_r( double value, 
	void (*func_dest2d)(const void *,void *,size_t,int,void *), void *user_ptr_dest2d,
	void (*func_d2dest)(const void *,void *,size_t,int,void *), void *user_ptr_d2dest,
	void (*func)(double [],double,size_t, ssize_t,ssize_t,ssize_t,mdarray *,void *),
	void *user_ptr_func, double *x_buf_ptr32,
	void *this_buf, mdarray *thisp,
	const heap_mem<mdarray_section_expinfo> &secinfo,
	size_t total_elements, size_t dim_ix,
	size_t this_len_block, size_t jj, size_t kk )
{
    const size_t bytes = thisp->bytes();
    const size_t *this_dim = thisp->cdimarray();
    char *d_ptr = (char *)this_buf;
    if ( dim_ix == 0 ) {
	size_t ii = secinfo[0].begin;
	d_ptr += bytes * secinfo[0].begin;
	/* */
	(*func_dest2d)((const void *)d_ptr, (void *)x_buf_ptr32,
		       secinfo[0].length, +1, user_ptr_dest2d);
	(*func)(x_buf_ptr32, value, secinfo[0].length, 
		ii,jj,kk, thisp, user_ptr_func);
	(*func_d2dest)((const void *)x_buf_ptr32, (void *)d_ptr,
		       secinfo[0].length, +1, user_ptr_d2dest);
	/* */
    }
    else {
	size_t blk_jj = 0;
	size_t blk_kk = 0;
	size_t off_dest, i;
	if ( 2 <= dim_ix ) {
	    blk_kk = 1;
	    for ( i=2 ; i < dim_ix ; i++ ) blk_kk *= this_dim[i];
	}
	else if ( dim_ix == 1 ) {
	    blk_jj = 1;
	}
	/* */
	this_len_block /= this_dim[dim_ix];
	/* */
	d_ptr += bytes * this_len_block * secinfo[dim_ix].begin;
	off_dest = secinfo[dim_ix].begin;
	for ( i=0 ; i < secinfo[dim_ix].length ; i++ ) {
	    fill_section_r(value, 
			   func_dest2d, user_ptr_dest2d,
			   func_d2dest, user_ptr_d2dest,
			   func, user_ptr_func, x_buf_ptr32,
			   d_ptr, thisp,
			   secinfo, total_elements, dim_ix - 1,
			   this_len_block, 
			   jj + blk_jj*(off_dest+i), 
			   kk + blk_kk*(off_dest+i) );
	    d_ptr += bytes * this_len_block;
	}
    }
    return;
}

/**
 * @brief  配列の一部分を指定された値で書き換え (低レベル)
 * @note   このメンバ関数は private です．
 */
mdarray &mdarray::image_vfillf( double value,
	void (*func_dest2d)(const void *,void *,size_t,int,void *), void *user_ptr_dest2d,
	void (*func_d2dest)(const void *,void *,size_t,int,void *), void *user_ptr_d2dest,
	void (*func)(double [],double,size_t, ssize_t,ssize_t,ssize_t,mdarray *,void *),
	void *user_ptr_func,
	const char *exp_fmt, va_list ap )
{
    heap_mem<char> secexp;
    heap_mem<mdarray_section_expinfo> secinfo;
    heap_mem<size_t> naxisx;
    size_t n_secinfo = 0;
    size_t this_total_elements, sec_total_elements, i;
    struct mdarray_cnv_nd_prms prms;

    heap_mem<double> x_buffer;	/* buffer for 1st arg of user's function */
    double *x_buf_ptr32;	/* 32-byte aligned pointer               */

    if ( this->length() == 0 ) goto quit;
    if ( exp_fmt == NULL ) goto quit;

    prms.total_len_elements = 0;			/* 効かない */
    prms.round_flg = this->rounding();			/* 四捨五入/切り捨て */

    if ( func_dest2d == NULL ) {
	/* 型変換のための関数 (自身の型→double) */
	func_dest2d = this->func_cnv_nd_x2d;
	user_ptr_dest2d = (void *)&prms;		/* 実際は使用されず */
    }
    if ( func_d2dest == NULL ) {
	/* 型変換のための関数 (double→自身の型) */
	func_d2dest = this->func_cnv_nd_d2x;
	user_ptr_d2dest = (void *)&prms;
    }

    if ( func_dest2d == NULL || func_d2dest == NULL ) {
	err_report(__FUNCTION__,"WARNING","invalid size of type; ignored");
	goto quit;
    }

    if ( c_vasprintf(secexp.ptr_address(), exp_fmt, ap) < 0 ) {
	err_report(__FUNCTION__,"FATAL","c_vasprintf() failed");
	goto quit;
    }

    /* パースし，secinfo を生成 */
    if ( parse_section_exp(&secinfo, &n_secinfo, secexp.ptr()) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","Error in parse_section_exp()");
    }
    if ( n_secinfo == 0 ) goto quit;

    /* secinfo をチェック，修正 */
    if ( test_section_info( secexp.ptr(), *this, 
			    &secinfo, &n_secinfo, &naxisx ) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","Error in test_section_info()");
    }
    if ( n_secinfo == 0 ) goto quit;

    this_total_elements = 1;
    for ( i=0 ; i < n_secinfo ; i++ ) {
	this_total_elements *= this->length(i);
    }
    sec_total_elements = 1;
    for ( i=0 ; i < n_secinfo ; i++ ) {
	sec_total_elements *= secinfo[i].length;
    }

    if ( x_buffer.allocate_aligned32(secinfo[0].length, &x_buf_ptr32) < 0 ) {
 	err_throw(__FUNCTION__,"FATAL","malloc() failed");
    }
    else {
	/* 再帰的に n 次元のセクションの fill を行なう */
	fill_section_r( value, 
			func_dest2d, user_ptr_dest2d,
			func_d2dest, user_ptr_d2dest,
			func, user_ptr_func, x_buf_ptr32,
			this->data_ptr(), this,
			secinfo, sec_total_elements, n_secinfo - 1,
			this_total_elements, 0,0 );
    }

 quit:
    return *this;
}


/**
 * @brief  配列の一部分を指定された値で書き換え (低レベル・高速版)
 * @note   このメンバ関数は private です．
 */
mdarray &mdarray::image_fill( double value,
	void (*func)(const void *,void *,size_t,int,void *), void *user_ptr,
	ssize_t col_index, size_t col_size,
	ssize_t row_index, size_t row_size,
	ssize_t layer_index, size_t layer_size )
{
    const size_t this_bytes = this->bytes();
    struct mdarray_cnv_nd_prms prms;
    size_t j, k;
    size_t col_blen, colrow_blen;
    char *k_ptr;
    char *j_ptr;

    if ( this->length() == 0 ) goto quit;

    prms.total_len_elements = 0;			/* 一応初期化 */
    prms.round_flg = this->rounding();			/* 四捨五入/切り捨て */

    if ( func == NULL ) {

	func = this->func_cnv_nd_d2x;
	user_ptr = (void *)&prms;

	if ( func == NULL ) {
	    err_report(__FUNCTION__,"WARNING","unsupported sz_type; do nothing");
	    goto quit;
	}
    }

    if ( this->fix_section_args( &col_index, &col_size, 
				 &row_index, &row_size,
				 &layer_index, &layer_size ) < 0 ) goto quit;

    if ( col_size == 0 ) goto quit;

    col_blen = this_bytes * this->col_length();
    colrow_blen = col_blen * this->row_length();

    prms.total_len_elements = col_size * row_size * layer_size;   /* SSE2 用 */

    k_ptr = (char *)(this->data_ptr()) 
	    + colrow_blen * layer_index			/* offset */
	    + col_blen * row_index
	    + this_bytes * col_index;
    for ( k=0 ; k < layer_size ; k++ ) {
	j_ptr = k_ptr;
	for ( j=0 ; j < row_size ; j++ ) {
	   /* */
	   (*func)((const void *)&value, (void *)j_ptr, col_size, 0, user_ptr);
	   /* */
	   j_ptr += col_blen;
	}
	k_ptr += colrow_blen;
    }

 quit:
    return *this;
}

/**
 * @brief  配列の一部分を指定された値で書き換え (低レベル)
 *
 *  自身の配列の指定された範囲の要素を，指定された値またはユーザ定義関数経由で
 *  書き換えます.
 * 
 * @param  value 書き込まれる値
 * @param  func_dest2d 自身の生データをdouble型へ変換するための
 *                     ユーザ関数のアドレス.
 * @param  user_ptr_dest2d func_dest2dの最後に与えられるユーザのポインタ
 * @param  func_d2dest double型の値から自身の生データへ変換するための
 *                     ユーザ関数のアドレス.
 * @param  user_ptr_d2dest func_d2destの最後に与えられるユーザのポインタ
 * @param  func 値変換の為のユーザ関数のアドレス
 * @param  user_ptr_func funcの最後に与えられるユーザのポインタ
 * @param  col_index 列位置
 * @param  col_size 列サイズ
 * @param  row_index 行位置
 * @param  row_size 行サイズ
 * @param  layer_index レイヤ位置
 * @param  layer_size レイヤサイズ
 * 
 * @return  自身の参照
 * @note    このメンバ関数は private です．
 */
mdarray &mdarray::image_fill( double value,
	void (*func_dest2d)(const void *,void *,size_t,int,void *), void *user_ptr_dest2d,
	void (*func_d2dest)(const void *,void *,size_t,int,void *), void *user_ptr_d2dest,
	void (*func)(double [],double,size_t, ssize_t,ssize_t,ssize_t,mdarray *,void *),
	void *user_ptr_func,
	ssize_t col_index, size_t col_size,
	ssize_t row_index, size_t row_size,
	ssize_t layer_index, size_t layer_size )
{
    const size_t this_bytes = this->bytes();
    size_t j, k;
    size_t col_blen, colrow_blen;
    char *k_ptr;
    char *j_ptr;
    struct mdarray_cnv_nd_prms prms;

    heap_mem<double> x_buffer;	/* buffer for 1st arg of user's function */
    double *x_buf_ptr32;	/* 32-byte aligned pointer               */

    if ( this->length() == 0 ) goto quit;

    prms.total_len_elements = 0;			/* 効かない */
    prms.round_flg = this->rounding();			/* 四捨五入/切り捨て */

    if ( func_dest2d == NULL ) {
	/* 型変換のための関数 (自身の型→double) */
	func_dest2d = this->func_cnv_nd_x2d;
	user_ptr_dest2d = (void *)&prms;		/* 実際は使用されず */
    }
    if ( func_d2dest == NULL ) {
	/* 型変換のための関数 (double→自身の型) */
	func_d2dest = this->func_cnv_nd_d2x;
	user_ptr_d2dest = (void *)&prms;
    }

    if ( func_dest2d == NULL || func_d2dest == NULL ) {
	err_report(__FUNCTION__,"WARNING","invalid size of type; ignored");
	goto quit;
    }

    if ( this->fix_section_args( &col_index, &col_size, 
				 &row_index, &row_size,
				 &layer_index, &layer_size ) < 0 ) goto quit;

    if ( col_size == 0 ) goto quit;

    col_blen = this_bytes * this->col_length();
    colrow_blen = col_blen * this->row_length();

    if ( x_buffer.allocate_aligned32(col_size, &x_buf_ptr32) < 0 ) {
 	err_throw(__FUNCTION__,"FATAL","malloc() failed");
    }
    else {
	ssize_t ii = col_index;					/* fixed */
	k_ptr = (char *)(this->data_ptr()) 
		+ colrow_blen * layer_index			/* offset */
		+ col_blen * row_index
		+ this_bytes * col_index;
	for ( k=0 ; k < layer_size ; k++ ) {
	    ssize_t kk = layer_index + k;
	    j_ptr = k_ptr;
	    for ( j=0 ; j < row_size ; j++ ) {
		ssize_t jj = row_index + j;
		/* */
		(*func_dest2d)((const void *)j_ptr, (void *)x_buf_ptr32,
			       col_size, +1, user_ptr_dest2d);
		(*func)(x_buf_ptr32,value,col_size, ii,jj,kk,this,user_ptr_func);
		(*func_d2dest)((const void *)x_buf_ptr32, (void *)j_ptr,
			       col_size, +1, user_ptr_d2dest);
		/* */
		j_ptr += col_blen;
	    }
	    k_ptr += colrow_blen;
	}
    }

 quit:
    return *this;
}

/**
 * @brief  section() 等の引数を調べて適切な座標のパラメータを設定 (低レベル)
 *
 *  引数の座標が正しい位置を示してるかを判断し，正しくない場合は座標の
 *  パラメータを調整する.
 *
 * @param  r_col_index 列位置
 * @param  r_col_size 列サイズ
 * @param  r_row_index 行位置
 * @param  r_row_size 行サイズ
 * @param  r_layer_index レイヤ位置
 * @param  r_layer_size レイヤサイズ
 * @param  r_col_ok 列が存在する場合は真に，
 *                  存在しない場合は偽に書き換えられる
 * @param  r_row_ok 行が存在する場合は真に，
 *                  存在しない場合は偽に書き換えられる
 * @param  r_layer_ok レイヤが存在する場合は真に，
 *                    存在しない場合は偽に書き換えられる
 * @return  引数が自身の配列の領域内を示していた場合は0，
 *          領域からはみ出しているが有効領域が存在する場合は正の値，
 *          有効領域が無い場合は負値　
 * @note    このメンバ関数は private です．
 */
/*
 * 引数の index=-1, size=1 の時，OK とされていたが間違いなので修正 2012/3/27
 * col_ok が true の場合のみ，return_status が 0 となるよう修正 2012/3/27
 * この修正により，有効領域が無い場合は，常に返り値が負値になるはず．
 */
int mdarray::test_section_args( ssize_t *r_col_index, size_t *r_col_size,
				ssize_t *r_row_index, size_t *r_row_size,
				ssize_t *r_layer_index, size_t *r_layer_size,
				bool *r_col_ok, bool *r_row_ok, 
				bool *r_layer_ok ) const
{
    int return_status = -1;
    ssize_t col_index = *r_col_index;
    size_t col_size = *r_col_size;
    ssize_t row_index = *r_row_index;
    size_t row_size = *r_row_size;
    ssize_t layer_index = *r_layer_index;
    size_t layer_size = *r_layer_size;
    bool col_ok = false;
    bool row_ok = false;
    bool layer_ok = false;

    if ( col_size == MDARRAY_ALL ) {
	if ( col_index < 0 ) col_index = 0;
	else if ( this->col_length() < (size_t)col_index ) {
	    col_index = this->col_length();
	}
	col_size = this->col_length() - col_index;
    }

    if ( row_size == MDARRAY_ALL ) {
	if ( row_index < 0 ) row_index = 0;
	else if ( this->row_length() < (size_t)row_index ) {
	    row_index = this->row_length();
	}
	row_size = this->row_length() - row_index;
    }

    if ( layer_size == MDARRAY_ALL ) {
	if ( layer_index < 0 ) layer_index = 0;
	else if ( this->layer_length() < (size_t)layer_index ) {
	    layer_index = this->layer_length();
	}
	layer_size = this->layer_length() - layer_index;
    }

    if ( 3 <= this->dim_length() && 0 < this->layer_length() &&
	 0 < layer_size && 0 < row_size && 0 < col_size ) {
	size_t layer_index_0;
	if ( layer_index < 0 ) { /* 矩形領域外を指定した場合，lengthを減らす */
	    if ( layer_size <= abs_sz2z(layer_index) ) goto quit;
	    layer_size -= abs_sz2z(layer_index);
	    layer_index = 0;
	}
	layer_index_0 = abs_sz2z(layer_index);
	if ( this->layer_length() <= layer_index_0 ) {
	    goto quit;
	}
	else {
	    if ( this->layer_length() < layer_index_0 + layer_size ) {
		layer_size = this->layer_length() - layer_index_0;
	    }
	    layer_ok = true;
	}
    }
    else {
	/* layer_ok は false */
	layer_index = 0;
	layer_size = 1;
    }

    if ( 2 <= this->dim_length() && 0 < this->row_length() &&
	 0 < row_size && 0 < col_size ) {
	size_t row_index_0;
	if ( row_index < 0 ) {	/* 矩形領域外を指定した場合，lengthを減らす */
	    if ( row_size <= abs_sz2z(row_index) ) goto quit;
	    row_size -= abs_sz2z(row_index);
	    row_index = 0;
	}
	row_index_0 = abs_sz2z(row_index);
	if ( layer_ok == false ) {	/* 2次元の場合 */
	    if ( this->row_length() * this->layer_length() <= row_index_0 ) {
		//sli__eprintf("debug: Q1 rl_sz=%ld\n",this->row_length() * this->layer_length());
		//sli__eprintf("debug: Q1 row_size=%ld\n",this->row_length());
		//sli__eprintf("debug: Q1 lay_size=%ld\n",this->layer_length());
		//sli__eprintf("debug: Q1 lay_idx=%ld\n",layer_index);
		goto quit;
	    }
	    else {
		if ( this->row_length() * this->layer_length() < row_index_0 + row_size ) {
		    row_size = this->row_length() * this->layer_length() - row_index_0;
		}
		row_ok = true;
	    }
	}
	else {				/* 3次元の場合 */
	    if ( this->row_length() <= row_index_0 ) {
		//sli__eprintf("debug: Q2\n");
		goto quit;
	    }
	    else {
		if ( this->row_length() < row_index_0 + row_size ) {
		    row_size = this->row_length() - row_index_0;
		}
		row_ok = true;
	    }
	}
    }
    else {
	layer_index = 0;
	layer_size = 1;
	layer_ok = false;
	/* row_ok は false */
	row_index = 0;
	row_size = 1;
    }

    if ( 1 <= this->dim_length() && 0 < this->col_length() &&
	 0 < col_size ) {
	size_t col_index_0;
	if ( col_index < 0 ) {	/* 矩形領域外を指定した場合，lengthを減らす */
	    if ( col_size <= abs_sz2z(col_index) ) goto quit;
	    col_size -= abs_sz2z(col_index);
	    col_index = 0;
	}
	col_index_0 = abs_sz2z(col_index);
	if ( row_ok == false ) {	/* 1次元の場合 */
	    if ( this->length() <= col_index_0 ) {
		//sli__eprintf("debug: Q3\n");
		goto quit;
	    }
	    else {
		if ( this->length() < col_index_0 + col_size ) {
		    col_size = this->length() - col_index_0;
		}
		col_ok = true;
	    }

	}
	else {				/* 2次元の場合 */
	    if ( this->col_length() <= col_index_0 ) {
		//sli__eprintf("debug: Q4\n");
		goto quit;
	    }
	    else {
		if ( this->col_length() < col_index_0 + col_size ) {
		    col_size = this->col_length() - col_index_0;
		}
		col_ok = true;
	    }
	}
    }
    else {
	layer_index = 0;
	layer_size = 1;
	layer_ok = false;
	row_index = 0;
	row_size = 1;
	row_ok = false;
	col_index = 0;
	col_size = 1;
    }

    if ( col_ok == true ) {
	if ( *r_col_index   == col_index &&
	     *r_row_index   == row_index &&
	     *r_layer_index == layer_index &&
	     (*r_col_size   == col_size   || *r_col_size   == MDARRAY_ALL) &&
	     (*r_row_size   == row_size   || *r_row_size   == MDARRAY_ALL) &&
	     (*r_layer_size == layer_size || *r_layer_size == MDARRAY_ALL) ) {
	    return_status = 0;
	}
	else {
	    return_status = 1;	/* flag for fixed */
	}
    }

    *r_col_index   = col_index;
    *r_col_size    = col_size;
    *r_row_index   = row_index;
    *r_row_size    = row_size;
    *r_layer_index = layer_index;
    *r_layer_size = layer_size;

    *r_col_ok      = col_ok;
    *r_row_ok      = row_ok;
    *r_layer_ok    = layer_ok;

 quit:
    return return_status;
}


/**
 * @brief  pastef() 等の引数を調べて適切な座標のパラメータを設定 (低レベル)
 *
 *  paste用のsecinfoをチェックし，pasteできる範囲が無い場合は -1 を返す．<br>
 *  mdarray::test_paste_arg と同様のコードになるようにしている．
 *
 * @note   private な関数です．
 */
static int test_paste_secinfo( const mdarray &src, const mdarray &dest,
		   const heap_mem<mdarray_section_expinfo> &secinfo, size_t n_secinfo )
{
    size_t i;
    for ( i=0 ; i < n_secinfo ; i++ ) {
	size_t abs_begin = abs_sz2z(secinfo[i].begin);
	size_t length = secinfo[i].length;
	if ( length == MDARRAY_ALL || src.length(i) < length ) {
	    length = src.length(i);	/* 小さい方をとる */
	}
	if ( length == 0 || dest.length(i) == 0 ) return -1;
	if ( 0 <= secinfo[i].begin ) {
	    if ( dest.length(i) <= abs_begin ) return -1;
	}
	else {	/* beginが負の場合 */
	    if ( length <= abs_begin ) return -1;
	}
    }
    return 0;
}

/**
 * @brief  再帰呼び出しにより，n次元のペースト処理を行なう (高速版) (内部用)
 *
 * @note  test_paste_secinfo() でチェックしてから使う事．<br>
 *        private な関数です．
 */
static void image_paste_fast_r( const mdarray &src, mdarray *dest, 
		    const heap_mem<mdarray_section_expinfo> &secinfo, size_t n_secinfo,
		    void (*func)(const void *,void *,size_t,int,void *),
		    void *user_ptr, 
		    size_t dim_ix, 
		    size_t blen_block_src, size_t blen_block_dest, 
		    const void *_src_p, void *_dest_p )
{
    char *dest_p = (char *)_dest_p;
    const char *src_p = (const char *)_src_p;
    size_t len_dest = dest->length(dim_ix);
    size_t len_src = src.length(dim_ix);
    ssize_t begin_paste = 0;
    size_t abs_begin = 0;
    size_t len_paste = MDARRAY_ALL;
    size_t i;
    blen_block_dest /= len_dest;
    blen_block_src /= len_src;

    //err_report1(__FUNCTION__,"DEBUG","dim_ix = %zu",dim_ix);

    /* secinfo が有る場合はそちらから取得 */
    if ( dim_ix < n_secinfo ) {
	begin_paste = secinfo[dim_ix].begin;
	abs_begin = abs_sz2z(secinfo[dim_ix].begin);
	len_paste = secinfo[dim_ix].length;
    }

    //err_report1(__FUNCTION__,"DEBUG","begin_paste = %zd",begin_paste);
    //err_report1(__FUNCTION__,"DEBUG","len_paste = %zu",len_paste);
 
    if ( len_paste == MDARRAY_ALL || len_src < len_paste ) {
	len_paste = len_src;	/* 小さい方をとる */
    }

    /* update len_paste */
    if ( 0 <= begin_paste ) {
	if ( len_dest < abs_begin + len_paste ) {
	    len_paste = len_dest - abs_begin;
	}
	dest_p += blen_block_dest * abs_begin;
    }
    else {
	len_paste -= abs_begin;
	if ( len_dest < len_paste ) len_paste = len_dest;
	src_p += blen_block_src * abs_begin;
    }

    //err_report1(__FUNCTION__,"DEBUG"," begin_paste = %zd",begin_paste);
    //err_report1(__FUNCTION__,"DEBUG"," len_paste = %zu",len_paste);

    if ( 0 < dim_ix ) {		/* 次元=1以上での処理 */
	if ( dest_p < src_p ) {
	    for ( i=0 ; i < len_paste ; i++ ) {		/* 前から後ろ */
		image_paste_fast_r(src, dest, secinfo, n_secinfo, 
				   func, user_ptr,
				   dim_ix - 1, blen_block_src, blen_block_dest,
				   src_p + blen_block_src * i,
				   dest_p + blen_block_dest * i);
	    }
	}
	else {
	    for ( i = len_paste ; 0 < i ; ) {		/* 後ろから前 */
		i--;
		image_paste_fast_r(src, dest, secinfo, n_secinfo, 
				   func, user_ptr,
				   dim_ix - 1, blen_block_src, blen_block_dest,
				   src_p + blen_block_src * i,
				   dest_p + blen_block_dest * i);
	    }
	}
    }
    else {			/* 次元=0 での処理 */
	if ( dest_p < src_p ) {
	    (*func)(src_p,dest_p,len_paste,+1,user_ptr);       /* 前から後ろ */
	}
	else {
	    (*func)(src_p,dest_p,len_paste,-1,user_ptr);       /* 後ろから前 */
	}
    }

    return;
}

/**
 * @brief  配列データ(画像データ)のペースト (低レベル・高速版)
 * @note   このメンバ関数は private です．
 */
mdarray &mdarray::image_vpastef( const mdarray &src_img,
	  void (*func)(const void *,void *,size_t,int,void *), void *user_ptr,
	  const char *exp_fmt, va_list ap )
{
    struct mdarray_cnv_nd_prms prms;
    heap_mem<char> secexp;
    heap_mem<mdarray_section_expinfo> secinfo;
    size_t n_secinfo = 0;
    size_t ndim, src_blen, dest_blen, i;

    if ( exp_fmt == NULL ) goto quit;

    if ( src_img.length() == 0 || this->length() == 0 ) goto quit;

    prms.total_len_elements = 0;			/* 効かない */
    prms.round_flg = this->rounding();			/* 四捨五入/切り捨て */

    if ( func == NULL ) {

	user_ptr = (void *)&prms;

#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
	if ( src_img.size_type() == org_sz_type && this->sz_type_rec == new_sz_type ) { \
	    func = &cnv_nd::fncname; \
	}
	SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(SEL_FUNC,,,,,,,,,,,,,,,,,,,,,,,,,,,,else);
#undef SEL_FUNC

	if ( func == NULL ) {
	    err_report(__FUNCTION__,"WARNING","unsupported sz_type; do nothing");
	    goto quit;
	}
    }

    /* expression の展開 */
    if ( c_vasprintf(secexp.ptr_address(), exp_fmt, ap) < 0 ) {
	err_report(__FUNCTION__,"FATAL","c_vasprintf() failed");
	goto quit;
    }

    /* expression のパース */
    if ( parse_section_exp(&secinfo, &n_secinfo, secexp.ptr()) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","Error in parse_section_exp()");
    }
    if ( n_secinfo == 0 ) goto quit;

    /* セクション情報のチェック */
    if ( test_paste_secinfo(src_img, *this, secinfo, n_secinfo) < 0 ) {
	goto quit;
    }

    for ( i=0 ; i < n_secinfo ; i++ ) {
	if ( secinfo[i].flip_flag == true ) {
	   err_report1(__FUNCTION__,"WARNING",
	   "'%s' : cannot use expression for flipping elements.",secexp.ptr());
	   break;
	}
    }

    ndim = this->dim_length();

    /* ndim 次元内の要素の個数を求めておく */
    src_blen = src_img.bytes();
    dest_blen = this->bytes();
    for ( i=0 ; i < ndim ; i++ ) {
	src_blen *= src_img.length(i);
	dest_blen *= this->length(i);
    }

    /* 再帰呼び出しでペーストする */
    image_paste_fast_r( src_img, this, secinfo, n_secinfo, 
			func, user_ptr, 
			ndim - 1,  src_blen, dest_blen,
			src_img.data_ptr_cs(), this->data_ptr() );

 quit:
    return *this;
}


/**
 * @brief  再帰呼び出しにより，n次元のペースト処理を行なう (内部用)
 *
 * @note  test_paste_secinfo() でチェックしてから使う事．<br>
 *        private な関数です．
 */
static void image_paste_r( const mdarray &src, mdarray *dest, 
	const heap_mem<mdarray_section_expinfo> &secinfo, size_t n_secinfo,
	void (*func_src2d)(const void *,void *,size_t,int,void *), void *user_ptr_src2d,
	void (*func_dest2d)(const void *,void *,size_t,int,void *), void *user_ptr_dest2d,
	void (*func_d2dest)(const void *,void *,size_t,int,void *), void *user_ptr_d2dest,
	void (*func)(double [],double [],size_t,ssize_t,ssize_t,ssize_t,mdarray *,void *),
	void *user_ptr_func,
	double *x_bptr_src, double *x_bptr_dst,
	size_t dim_ix, 
	size_t blen_block_src, size_t blen_block_dest, 
	const void *_src_p, void *_dest_p,
	size_t jj, size_t kk )
{
    char *dest_p = (char *)_dest_p;
    const char *src_p = (const char *)_src_p;
    size_t len_dest = dest->length(dim_ix);
    size_t len_src = src.length(dim_ix);
    ssize_t begin_paste = 0;
    size_t abs_begin = 0;
    size_t len_paste = MDARRAY_ALL;
    size_t off_dest = 0;
    size_t i;
    blen_block_dest /= len_dest;
    blen_block_src /= len_src;

    //err_report1(__FUNCTION__,"DEBUG","dim_ix = %zu",dim_ix);

    /* secinfo が有る場合はそちらから取得 */
    if ( dim_ix < n_secinfo ) {
	begin_paste = secinfo[dim_ix].begin;
	abs_begin = abs_sz2z(secinfo[dim_ix].begin);
	len_paste = secinfo[dim_ix].length;
    }

    //err_report1(__FUNCTION__,"DEBUG","begin_paste = %zd",begin_paste);
    //err_report1(__FUNCTION__,"DEBUG","len_paste = %zu",len_paste);
 
    if ( len_paste == MDARRAY_ALL || len_src < len_paste ) {
	len_paste = len_src;	/* 小さい方をとる */
    }

    /* update len_paste */
    if ( 0 <= begin_paste ) {
	if ( len_dest < abs_begin + len_paste ) {
	    len_paste = len_dest - abs_begin;
	}
	dest_p += blen_block_dest * abs_begin;
	off_dest = abs_begin;
    }
    else {
	len_paste -= abs_begin;
	if ( len_dest < len_paste ) len_paste = len_dest;
	src_p += blen_block_src * abs_begin;
    }

    //err_report1(__FUNCTION__,"DEBUG"," begin_paste = %zd",begin_paste);
    //err_report1(__FUNCTION__,"DEBUG"," len_paste = %zu",len_paste);

    if ( 0 < dim_ix ) {		/* 次元=1以上での処理 */
	size_t blk_jj = 0;
	size_t blk_kk = 0;
	if ( 2 <= dim_ix ) {
	    blk_kk = 1;
	    for ( i=2 ; i < dim_ix ; i++ ) blk_kk *= dest->length(i);
	}
	else if ( dim_ix == 1 ) {
	    blk_jj = 1;
	}
	if ( dest_p < src_p ) {
	    for ( i=0 ; i < len_paste ; i++ ) {		/* 前から後ろ */
		image_paste_r(src, dest, secinfo, n_secinfo,
			      func_src2d, user_ptr_src2d,
			      func_dest2d, user_ptr_dest2d,
			      func_d2dest, user_ptr_d2dest,
			      func, user_ptr_func,
			      x_bptr_src, x_bptr_dst,
			      dim_ix - 1, blen_block_src, blen_block_dest,
			      src_p + blen_block_src * i,
			      dest_p + blen_block_dest * i,
			      jj + blk_jj*(off_dest+i), 
			      kk + blk_kk*(off_dest+i) );
	    }
	}
	else {
	    for ( i = len_paste ; 0 < i ; ) {		/* 後ろから前 */
		i--;
		image_paste_r(src, dest, secinfo, n_secinfo, 
			      func_src2d, user_ptr_src2d,
			      func_dest2d, user_ptr_dest2d,
			      func_d2dest, user_ptr_d2dest,
			      func, user_ptr_func,
			      x_bptr_src, x_bptr_dst,
			      dim_ix - 1, blen_block_src, blen_block_dest,
			      src_p + blen_block_src * i,
			      dest_p + blen_block_dest * i,
			      jj + blk_jj*(off_dest+i), 
			      kk + blk_kk*(off_dest+i) );
	    }
	}
    }
    else {			/* 次元=0 での処理 */
	size_t ii = off_dest;
	/* 一時バッファにのせるので，ここは前から後ろだけで良い */
	(*func_src2d)((const void *)src_p, (void *)x_bptr_src,
		      len_paste, +1, user_ptr_src2d);
	(*func_dest2d)((const void *)dest_p, (void *)x_bptr_dst,
		       len_paste, +1, user_ptr_dest2d);
	(*func)(x_bptr_dst,x_bptr_src,len_paste,ii,jj,kk,dest,user_ptr_func);
	(*func_d2dest)((const void *)x_bptr_dst, (void *)dest_p,
		       len_paste, +1, user_ptr_d2dest);
    }

    return;
}

/**
 * @brief  配列データ(画像データ)のペースト (低レベル)
 * @note   このメンバ関数は private です．
 */
mdarray &mdarray::image_vpastef( const mdarray &src_img,
	void (*func_src2d)(const void *,void *,size_t,int,void *), void *user_ptr_src2d,
	void (*func_dest2d)(const void *,void *,size_t,int,void *), void *user_ptr_dest2d,
	void (*func_d2dest)(const void *,void *,size_t,int,void *), void *user_ptr_d2dest,
	void (*func)(double [],double [],size_t,ssize_t,ssize_t,ssize_t,mdarray *,void *),
	void *user_ptr_func,
	const char *exp_fmt, va_list ap )
{
    heap_mem<char> secexp;
    heap_mem<mdarray_section_expinfo> secinfo;
    size_t n_secinfo = 0;
    size_t ndim, src_blen, dest_blen, i;
    struct mdarray_cnv_nd_prms prms;

    heap_mem<double> x_buf_src;
    heap_mem<double> x_buf_dst;
    double *x_bptr_src;		/* aligned pointer */
    double *x_bptr_dst;		/* aligned pointer */
    size_t len_x_buf;

    if ( exp_fmt == NULL ) goto quit;

    if ( src_img.length() == 0 || this->length() == 0 ) goto quit;

    prms.total_len_elements = 0;			/* 効かない */
    prms.round_flg = this->rounding();			/* 四捨五入/切り捨て */

    if ( func_src2d == NULL ) {
	/* 型変換のための関数 (引数objの型→double) */
	func_src2d = src_img.func_cnv_nd_x2d;
	user_ptr_src2d = (void *)&prms;			/* 実際は使用されず */
    }
    if ( func_dest2d == NULL ) {
	/* 型変換のための関数 (自身の型→double) */
	func_dest2d = this->func_cnv_nd_x2d;
	user_ptr_dest2d = (void *)&prms;		/* 実際は使用されず */
    }
    if ( func_d2dest == NULL ) {
	/* 型変換のための関数 (double→自身の型) */
	func_d2dest = this->func_cnv_nd_d2x;
	user_ptr_d2dest = (void *)&prms;
    }

    if ( func_src2d == NULL || func_dest2d == NULL || func_d2dest == NULL ) {
	err_report(__FUNCTION__,"WARNING","invalid size of type; ignored");
	goto quit;
    }

    /* expression の展開 */
    if ( c_vasprintf(secexp.ptr_address(), exp_fmt, ap) < 0 ) {
	err_report(__FUNCTION__,"FATAL","c_vasprintf() failed");
	goto quit;
    }

    /* expression のパース */
    if ( parse_section_exp(&secinfo, &n_secinfo, secexp.ptr()) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","Error in parse_section_exp()");
    }
    if ( n_secinfo == 0 ) goto quit;

    /* セクション情報のチェック */
    if ( test_paste_secinfo(src_img, *this, secinfo, n_secinfo) < 0 ) {
	goto quit;
    }

    for ( i=0 ; i < n_secinfo ; i++ ) {
	if ( secinfo[i].flip_flag == true ) {
	   err_report1(__FUNCTION__,"WARNING",
	   "'%s' : cannot use expression for flipping elements.",secexp.ptr());
	   break;
	}
    }

    ndim = this->dim_length();

    /* ndim 次元内の要素の個数を求めておく */
    src_blen = src_img.bytes();
    dest_blen = this->bytes();
    for ( i=0 ; i < ndim ; i++ ) {
	src_blen *= src_img.length(i);
	dest_blen *= this->length(i);
    }

    /* x_buf_xxx の長さを求める (image_paste_r() の一部コードそのまま) */
    len_x_buf = secinfo[0].length;
    if ( len_x_buf == MDARRAY_ALL || src_img.length(0) < len_x_buf ) {
	len_x_buf = src_img.length(0);	/* 小さい方をとる */
    }
    if ( 0 <= secinfo[0].begin ) {
	if ( this->length(0) < abs_sz2z(secinfo[0].begin) + len_x_buf ) {
	    len_x_buf = this->length(0) - abs_sz2z(secinfo[0].begin);
	}
    }
    else {
	len_x_buf -= abs_sz2z(secinfo[0].begin);
	if ( this->length(0) < len_x_buf ) len_x_buf = this->length(0);
    }

    //err_report1(__FUNCTION__,"DEBUG", "len_x_buf = %zu\n", len_x_buf);

    /* 一時バッファを確保 */
    if ( x_buf_src.allocate_aligned32(len_x_buf, &x_bptr_src) < 0 ||
	 x_buf_dst.allocate_aligned32(len_x_buf, &x_bptr_dst) < 0 ) {
 	err_throw(__FUNCTION__,"FATAL","malloc() failed");
    }

    /* 再帰呼び出しでペーストする */
    image_paste_r( src_img, this, secinfo, n_secinfo, 
		   func_src2d, user_ptr_src2d,
		   func_dest2d, user_ptr_dest2d,
		   func_d2dest, user_ptr_d2dest,
		   func, user_ptr_func,
		   x_bptr_src, x_bptr_dst,
		   ndim - 1,  src_blen, dest_blen,
		   src_img.data_ptr_cs(), this->data_ptr(), 0,0 );

 quit:
    return *this;
}


/**
 * @brief  配列データ(画像データ)のペースト (低レベル・高速版)
 *
 * 自身の配列の指定された範囲の要素値に，src_imgで指定されたオブジェクトの
 * 各要素値，またはユーザ定義関数経由で変換された各要素を貼り付けます. 
 * 
 * @param  src_img 源泉となる配列を持つオブジェクト
 * @param  func 値変換の為のユーザ関数のアドレス
 * @param  user_ptr funcの最後に与えられるユーザのポインタ
 * @param  dst_col 列位置
 * @param  dst_row 行位置
 * @param  dst_layer レイヤ位置
 * 
 * @return  自身の参照
 * @note    このメンバ関数は private です．
 */
mdarray &mdarray::image_paste( const mdarray &src_img,
			 void (*func)(const void *,void *,size_t,int,void *),
			 void *user_ptr,
			 ssize_t dst_col, ssize_t dst_row, ssize_t dst_layer )
{
    const size_t src_bytes = src_img.bytes();
    const size_t dst_bytes = this->bytes();
    struct mdarray_cnv_nd_prms prms;
    size_t col_size, row_size, layer_size;
    ssize_t col_begin, row_begin, layer_begin;    /* for()ループで使用    */
    bool col_ok, row_ok, layer_ok;
    size_t src_col_blen, src_row_len;
    size_t dst_col_blen, dst_row_len;
    const char *src_k_ptr;
    char *dst_k_ptr;
    const char *src_j_ptr;
    char *dst_j_ptr;
    const char *src_i_ptr;
    char *dst_i_ptr;

    if ( src_img.length() == 0 || this->length() == 0 ) goto quit;

    prms.total_len_elements = 0;			/* 効かない */
    prms.round_flg = this->rounding();			/* 四捨五入/切り捨て */

    if ( func == NULL ) {

	user_ptr = (void *)&prms;

#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
	if ( src_img.size_type() == org_sz_type && this->sz_type_rec == new_sz_type ) { \
	    func = &cnv_nd::fncname; \
	}
	SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(SEL_FUNC,,,,,,,,,,,,,,,,,,,,,,,,,,,,else);
#undef SEL_FUNC

	if ( func == NULL ) {
	    err_report(__FUNCTION__,"WARNING","unsupported sz_type; do nothing");
	    goto quit;
	}
    }

    //err_report1(__FUNCTION__,"DEBUG","dst_col=%zd",dst_col);
    //err_report1(__FUNCTION__,"DEBUG","dst_row=%zd",dst_row);
    //err_report1(__FUNCTION__,"DEBUG","dst_layer=%zd",dst_layer);

    /* 引数をチェック */
    if ( this->test_paste_arg( src_img, 
		       &dst_col, &dst_row, &dst_layer,
		       &col_ok, &row_ok, &layer_ok,
		       &col_size, &row_size, &layer_size,
		       &col_begin, &row_begin, &layer_begin ) < 0 ) goto quit;

    //err_report1(__FUNCTION__,"DEBUG","=>dst_col=%zd",dst_col);
    //err_report1(__FUNCTION__,"DEBUG","=>dst_row=%zd",dst_row);
    //err_report1(__FUNCTION__,"DEBUG","=>dst_layer=%zd",dst_layer);
    //err_report1(__FUNCTION__,"DEBUG","=>col_size=%zd",col_size);
    //err_report1(__FUNCTION__,"DEBUG","=>row_size=%zd",row_size);
    //err_report1(__FUNCTION__,"DEBUG","=>layer_size=%zd",layer_size);
    //err_report1(__FUNCTION__,"DEBUG","=>col_begin=%zd",col_begin);
    //err_report1(__FUNCTION__,"DEBUG","=>row_begin=%zd",row_begin);
    //err_report1(__FUNCTION__,"DEBUG","=>layer_begin=%zd",layer_begin);

    //if ( layer_ok == true ) layer_size = src_img.layer_length();
    //else layer_size = 1;

    src_col_blen = src_bytes * src_img.col_length();
    src_row_len = src_img.row_length();

    dst_col_blen = dst_bytes * this->col_length();
    dst_row_len = this->row_length();

    dst_k_ptr = (char *)(this->data_ptr()) + 
	(dst_col_blen * dst_row_len) * (dst_layer + layer_begin);
    src_k_ptr = (const char *)(src_img.data_ptr_cs()) +
	(src_col_blen * src_row_len) * layer_begin;

    /* src_img が自分自身でもいいようにする */
    if ( dst_k_ptr + dst_col_blen * (dst_row + row_begin) + 
	 dst_bytes * (dst_col + col_begin) <=
	 src_k_ptr + src_col_blen * row_begin + src_bytes * col_begin ) {
	/* 前から〜 */
	size_t j, k;
	//err_report(__FUNCTION__,"DEBUG","=>=>=>=>=>=>=>=>");
	for ( k=layer_begin ; k < layer_size ; k++ ) {
	  src_j_ptr = src_k_ptr + src_col_blen * row_begin;
	  dst_j_ptr = dst_k_ptr + dst_col_blen * (dst_row + row_begin);
	  for ( j=row_begin ; j < row_size ; j++ ) {

	    src_i_ptr = src_j_ptr + src_bytes * col_begin;
	    dst_i_ptr = dst_j_ptr + dst_bytes * (dst_col + col_begin);

	    (*func)((const void *)src_i_ptr,
		    (void *)dst_i_ptr,col_size - col_begin,+1,user_ptr);

	    src_j_ptr += src_col_blen;
	    dst_j_ptr += dst_col_blen;
	  }
	  src_k_ptr += (src_col_blen * src_row_len);
	  dst_k_ptr += (dst_col_blen * dst_row_len);
	}
    }
    else {
	/* 後ろから〜 */
	ssize_t j, k;
	//err_report(__FUNCTION__,"DEBUG","<=<=<=<=<=<=<=<=");
	dst_k_ptr = (char *)(this->data_ptr()) + 
	    (dst_col_blen * dst_row_len) * (dst_layer + layer_size);
	src_k_ptr = (const char *)(src_img.data_ptr_cs()) +
	    (src_col_blen * src_row_len) * layer_size;
	for ( k=layer_size ; layer_begin < k-- ; ) {
	  src_k_ptr -= (src_col_blen * src_row_len);
	  dst_k_ptr -= (dst_col_blen * dst_row_len);
	  src_j_ptr = src_k_ptr + src_col_blen * row_size;
	  dst_j_ptr = dst_k_ptr + dst_col_blen * (dst_row + row_size);
	  for ( j=row_size ; row_begin < j-- ; ) {
	    src_j_ptr -= src_col_blen;
	    dst_j_ptr -= dst_col_blen;

	    src_i_ptr = src_j_ptr + src_bytes * col_begin;
	    dst_i_ptr = dst_j_ptr + dst_bytes * (dst_col + col_begin);

	    (*func)((const void *)src_i_ptr,
		    (void *)dst_i_ptr,col_size - col_begin,-1,user_ptr);

	  }
	}
    }		/* else */

 quit:
    return *this;
}


/**
 * @brief   配列データ(画像データ)のペースト (低レベル)
 *
 *  自身の配列の指定された範囲の要素値に，src_img で指定されたオブジェクトの
 *  各要素値またはユーザ定義関数経由で変換された各要素を貼り付けます．
 *
 * @param  src_img 源泉となる配列を持つオブジェクト
 * @param  func_src2d オブジェクトsrcの生データを
 *                    double型へ変換するためのユーザ関数へのアドレス
 * @param  user_ptr_src2d func_src2dの最後に与えられる
 *                        ユーザのポインタ.
 * @param  func_dest2d 自身の生データをdouble型へ変換するための
 *                     ユーザ関数へのアドレス.
 * @param  user_ptr_dest2d func_dest2dの最後に与えられるユーザのポインタ
 * @param  func_d2dest double型の値から自身の生データへ変換するための
 *                     ユーザ関数へのアドレス
 * @param  user_ptr_d2dest func_d2destの最後に与えられるユーザのポインタ
 * @param  func 値変換のためのユーザ関数のアドレス
 * @param  user_ptr_func funcの最後に与えられるユーザのポインタ
 * @param  dst_col 列位置
 * @param  dst_row 行位置
 * @param  dst_layer レイヤ位置
 * 
 * @return  自身の参照
 * @note    このメンバ関数は private です．
 */
mdarray &mdarray::image_paste( const mdarray &src_img,
	void (*func_src2d)(const void *,void *,size_t,int,void *), void *user_ptr_src2d,
	void (*func_dest2d)(const void *,void *,size_t,int,void *), void *user_ptr_dest2d,
	void (*func_d2dest)(const void *,void *,size_t,int,void *), void *user_ptr_d2dest,
	void (*func)(double [],double [],size_t,ssize_t,ssize_t,ssize_t,mdarray *,void *),
	void *user_ptr_func,
	ssize_t dst_col, ssize_t dst_row, ssize_t dst_layer )
{
    const size_t src_bytes = src_img.bytes();
    const size_t dst_bytes = this->bytes();
    size_t col_size, row_size, layer_size;
    ssize_t col_begin, row_begin, layer_begin;    /* for()ループで使用    */
    bool col_ok, row_ok, layer_ok;
    size_t src_col_blen, src_row_len;
    size_t dst_col_blen, dst_row_len;
    const char *src_k_ptr;
    char *dst_k_ptr;
    const char *src_j_ptr;
    char *dst_j_ptr;
    const char *src_i_ptr;
    char *dst_i_ptr;
    struct mdarray_cnv_nd_prms prms;

    heap_mem<double> x_buf_src;
    heap_mem<double> x_buf_dst;
    double *x_bptr_src;		/* aligned pointer */
    double *x_bptr_dst;		/* aligned pointer */
    size_t col_len;

    if ( src_img.length() == 0 || this->length() == 0 ) goto quit;

    prms.total_len_elements = 0;			/* 効かない */
    prms.round_flg = this->rounding();			/* 四捨五入/切り捨て */

    if ( func_src2d == NULL ) {
	/* 型変換のための関数 (引数objの型→double) */
	func_src2d = src_img.func_cnv_nd_x2d;
	user_ptr_src2d = (void *)&prms;			/* 実際は使用されず */
    }
    if ( func_dest2d == NULL ) {
	/* 型変換のための関数 (自身の型→double) */
	func_dest2d = this->func_cnv_nd_x2d;
	user_ptr_dest2d = (void *)&prms;		/* 実際は使用されず */
    }
    if ( func_d2dest == NULL ) {
	/* 型変換のための関数 (double→自身の型) */
	func_d2dest = this->func_cnv_nd_d2x;
	user_ptr_d2dest = (void *)&prms;
    }

    if ( func_src2d == NULL || func_dest2d == NULL || func_d2dest == NULL ) {
	err_report(__FUNCTION__,"WARNING","invalid size of type; ignored");
	goto quit;
    }

    /* 引数をチェックする */
    if ( this->test_paste_arg( src_img, 
		       &dst_col, &dst_row, &dst_layer,
		       &col_ok, &row_ok, &layer_ok,
		       &col_size, &row_size, &layer_size,
		       &col_begin, &row_begin, &layer_begin ) < 0 ) goto quit;

    //if ( layer_ok == true ) layer_size = src_img.layer_length();
    //else layer_size = 1;

    src_col_blen = src_bytes * src_img.col_length();
    src_row_len = src_img.row_length();

    dst_col_blen = dst_bytes * this->col_length();
    dst_row_len = this->row_length();

    /* 一時領域を確保 */
    col_len = col_size - col_begin;
    if ( x_buf_src.allocate_aligned32(col_len, &x_bptr_src) < 0 ||
	 x_buf_dst.allocate_aligned32(col_len, &x_bptr_dst) < 0 ) {
 	err_throw(__FUNCTION__,"FATAL","malloc() failed");
    }
    else {
      dst_k_ptr = (char *)(this->data_ptr()) + 
	  (dst_col_blen * dst_row_len) * (dst_layer + layer_begin);
      src_k_ptr = (const char *)(src_img.data_ptr_cs()) +
	  (src_col_blen * src_row_len) * layer_begin;

      /* src_img が自分自身でもいいようにする */
      if ( dst_k_ptr + dst_col_blen * (dst_row + row_begin) + 
	 dst_bytes * (dst_col + col_begin) <=
	 src_k_ptr + src_col_blen * row_begin + src_bytes * col_begin ) {
	/* 前から〜 */
	size_t j, k;
	//err_report(__FUNCTION__,"DEBUG","=>=>=>=>=>=>=>=>");
	for ( k=layer_begin ; k < layer_size ; k++ ) {
	  ssize_t kk = dst_layer + k;
	  src_j_ptr = src_k_ptr + src_col_blen * row_begin;
	  dst_j_ptr = dst_k_ptr + dst_col_blen * (dst_row + row_begin);
	  for ( j=row_begin ; j < row_size ; j++ ) {
	    ssize_t ii, jj = dst_row + j;

	    /* 一時バッファにのせるので，ここは前から後ろで良い */
	    src_i_ptr = src_j_ptr + src_bytes * col_begin;
	    dst_i_ptr = dst_j_ptr + dst_bytes * (dst_col + col_begin);

	    ii = dst_col + col_begin;
	    (*func_src2d)((const void *)src_i_ptr, (void *)x_bptr_src,
			  col_len, +1, user_ptr_src2d);
	    (*func_dest2d)((const void *)dst_i_ptr, (void *)x_bptr_dst,
			  col_len, +1, user_ptr_dest2d);
	    (*func)(x_bptr_dst,x_bptr_src,col_len,ii,jj,kk,this,user_ptr_func);
	    (*func_d2dest)((const void *)x_bptr_dst, (void *)dst_i_ptr,
			   col_len, +1, user_ptr_d2dest);

	    src_j_ptr += src_col_blen;
	    dst_j_ptr += dst_col_blen;
	  }
	  src_k_ptr += (src_col_blen * src_row_len);
	  dst_k_ptr += (dst_col_blen * dst_row_len);
	}
      }
      else {
	/* 後ろから〜 */
	ssize_t j, k;
	//err_report(__FUNCTION__,"DEBUG","<=<=<=<=<=<=<=<=");
	dst_k_ptr = (char *)(this->data_ptr()) + 
	    (dst_col_blen * dst_row_len) * (dst_layer + layer_size);
	src_k_ptr = (const char *)(src_img.data_ptr_cs()) +
	    (src_col_blen * src_row_len) * layer_size;
	for ( k=layer_size ; layer_begin < k-- ; ) {
	  ssize_t kk = dst_layer + k;
	  src_k_ptr -= (src_col_blen * src_row_len);
	  dst_k_ptr -= (dst_col_blen * dst_row_len);
	  src_j_ptr = src_k_ptr + src_col_blen * row_size;
	  dst_j_ptr = dst_k_ptr + dst_col_blen * (dst_row + row_size);
	  for ( j=row_size ; row_begin < j-- ; ) {
	    ssize_t ii, jj = dst_row + j;
	    src_j_ptr -= src_col_blen;
	    dst_j_ptr -= dst_col_blen;

	    /* 一時バッファにのせるので，ここは前から後ろで良い */
	    src_i_ptr = src_j_ptr + src_bytes * col_begin;
	    dst_i_ptr = dst_j_ptr + dst_bytes * (dst_col + col_begin);

	    ii = dst_col + col_begin;
	    (*func_src2d)((const void *)src_i_ptr, (void *)x_bptr_src,
			  col_len, +1, user_ptr_src2d);
	    (*func_dest2d)((const void *)dst_i_ptr, (void *)x_bptr_dst,
			  col_len, +1, user_ptr_dest2d);
	    (*func)(x_bptr_dst,x_bptr_src,col_len,ii,jj,kk,this,user_ptr_func);
	    (*func_d2dest)((const void *)x_bptr_dst, (void *)dst_i_ptr,
			   col_len, +1, user_ptr_d2dest);

	  }
	}
      }		/* else */
    }

 quit:
    return *this;
}

/* paste() 等の引数を調べて適切な座標のパラメータを設定する */
/* r_col_begin は ssize_t になってるが，負の値を返す事はない */
/**
 * @brief   paste() 等の引数を調べて適切な座標のパラメータを設定 (低レベル)
 *
 *  引数 src_img で示されたオブジェクトの配列を 自身の配列に貼り付けする場合に
 *  おける，座標の位置と長さのチェックを実施し，適切な座標パラメータの設定を行
 *  います.
 * 	
 * @param  src_img 源泉となる配列を持つオブジェクト
 * @param  r_col_idx 列位置(destつまり自身)
 * @param  r_row_idx 行位置(destつまり自身)
 * @param  r_layer_idx レイヤ位置(destつまり自身)
 * @param  r_col_ok 列が存在する場合は真に，
 *                  存在しない場合は偽に書き換えられる
 * @param  r_row_ok 行が存在する場合は真に，
 *                  存在しない場合は偽に書き換えられる
 * @param  r_layer_ok レイヤが存在する場合は真に，
 *                    存在しない場合は偽に書き換えられる
 * @param  r_col_size 列サイズ(src)
 * @param  r_row_size 行サイズ(src)
 * @param  r_layer_size レイヤサイズ(src)
 * @param  r_col_begin 列の開始位置(src)
 * @param  r_row_begin 行の開始位置(src)
 * @param  r_layer_begin レイヤの開始位置(src)
 * @return  指定された位置・長さが配列の範囲内に有る場合は0，
 *          範囲外にあるが有効領域が有る場合は正の値，有効領域が無い場合負値
 * @note   このメンバ関数は private です．
 */
int mdarray::test_paste_arg( const mdarray &src_img,
     ssize_t *r_col_idx, ssize_t *r_row_idx, ssize_t *r_layer_idx,
     bool *r_col_ok, bool *r_row_ok, bool *r_layer_ok,
     size_t *r_col_size, size_t *r_row_size, size_t *r_layer_size,
     ssize_t *r_col_begin, ssize_t *r_row_begin, ssize_t *r_layer_begin ) const
{
    int return_status = -1;
    ssize_t col_idx = *r_col_idx;
    ssize_t row_idx = *r_row_idx;
    ssize_t layer_idx = *r_layer_idx;
    bool col_ok = false;
    bool row_ok = false;
    bool layer_ok = false;
    size_t col_size = 1;
    size_t row_size = 1;
    size_t layer_size = 1;
    ssize_t col_begin = 0;
    ssize_t row_begin = 0;
    ssize_t layer_begin = 0;

    //if ( 0 < this->layer_length() &&
    //	 0 <= layer_idx && layer_idx < (ssize_t)(this->layer_length()) ) {
    //	layer_ok = true;
    //}
    //else {
    //	layer_idx = 0;
    //}

    //err_report1(__FUNCTION__,"DEBUG","src_img.layer_col() = %zd",src_img.col_length());
    //err_report1(__FUNCTION__,"DEBUG","src_img.layer_row() = %zd",src_img.row_length());
    //err_report1(__FUNCTION__,"DEBUG","src_img.layer_length() = %zd",src_img.layer_length());

    if ( 1 <= this->dim_length() && 1 <= src_img.dim_length() && 
	 0 < this->layer_length() && 0 < src_img.layer_length() ) {
	size_t abs_idx = abs_sz2z(layer_idx);
	layer_size = src_img.layer_length();
	if ( 0 <= layer_idx ) {		/* idx が0以上の場合 */
	    if ( this->layer_length() <= abs_idx ) goto quit;
	    if ( this->layer_length() < abs_idx + layer_size ) {
		layer_size = this->layer_length() - abs_idx;
	    }
	}
	else {				/* idx が負の場合 */
	    if ( layer_size <= abs_idx ) goto quit;
	    if ( this->layer_length() + abs_idx < layer_size ) {
		layer_size = this->layer_length() + abs_idx;
	    }
	    layer_begin = - layer_idx;

	}
	layer_ok = true;
	/*
	if ( layer_idx + layer_size <= 0 || 
	    (0 <= layer_idx && this->layer_length() <= abs_idx) ) {
	    goto quit;
	}
	else {
	    if ( this->layer_length() < layer_idx + layer_size ) {
		layer_size = this->layer_length() - layer_idx;
	    }
	    if ( layer_idx < 0 ) layer_begin = - layer_idx;
	    layer_ok = true;
	}
	*/
    }
    else {
	layer_idx = 0;
	layer_size = 1;
	layer_begin = 0;
    }

    if ( 1 <= this->dim_length() && 1 <= src_img.dim_length() &&
	 0 < this->row_length() && 0 < src_img.row_length() ) {
	size_t abs_idx = abs_sz2z(row_idx);
	row_size = src_img.row_length();
	if ( 0 <= row_idx ) {		/* idx が0以上の場合 */
	    if ( this->row_length() <= abs_idx ) goto quit;
	    if ( this->row_length() < abs_idx + row_size ) {
		row_size = this->row_length() - abs_idx;
	    }
	}
	else {				/* idx が負の場合 */
	    if ( row_size <= abs_idx ) goto quit;
	    if ( this->row_length() + abs_idx < row_size ) {
		row_size = this->row_length() + abs_idx;
	    }
	    row_begin = - row_idx;

	}
	row_ok = true;
	/*
	if ( layer_ok == false ) {	// 2次元の場合
	    row_size = src_img.row_length() * src_img.layer_length();
	    if ( row_idx + row_size <= 0 ||
		 (0 <= row_idx && this->row_length() * this->layer_length() <= abs_sz2z(row_idx)) ) {
		goto quit;
	    }
	    else {
		if ( this->row_length() * this->layer_length() < row_idx + row_size ) {
		    row_size = this->row_length() * this->layer_length() - row_idx;
		}
		if ( row_idx < 0 ) row_begin = - row_idx;
		row_ok = true;
	    }
	}
	else {				// 3次元の場合
	    row_size = src_img.row_length();
	    if ( row_idx + row_size <= 0 || 
		 (0 <= row_idx && this->row_length() <= abs_sz2z(row_idx)) ) {
		goto quit;
	    }
	    else {
		if ( this->row_length() < row_idx + row_size ) {
		    row_size = this->row_length() - row_idx;
		}
		if ( row_idx < 0 ) row_begin = - row_idx;
		row_ok = true;
	    }
	}
	*/
    }
    else {
	layer_idx = 0;
	layer_size = 1;
	layer_begin = 0;
	layer_ok = false;
	row_idx = 0;
	row_size = 1;
	row_begin = 0;
    }

    if ( 1 <= this->dim_length() && 1 <= src_img.dim_length() &&
	 0 < this->col_length() && 0 < src_img.col_length() ) {
	size_t abs_idx = abs_sz2z(col_idx);
	col_size = src_img.col_length();
	if ( 0 <= col_idx ) {		/* idx が0以上の場合 */
	    if ( this->col_length() <= abs_idx ) goto quit;
	    if ( this->col_length() < abs_idx + col_size ) {
		col_size = this->col_length() - abs_idx;
	    }
	}
	else {				/* idx が負の場合 */
	    if ( col_size <= abs_idx ) goto quit;
	    if ( this->col_length() + abs_idx < col_size ) {
		col_size = this->col_length() + abs_idx;
	    }
	    col_begin = - col_idx;

	}
	col_ok = true;
	/*
	if ( row_ok == false ) {	// 1次元の場合
	    col_size = src_img.length();
	    if ( col_idx + col_size <= 0 || 
		 (0 <= col_idx && this->length() <= abs_sz2z(col_idx)) ) {
		goto quit;
	    }
	    else {
		if ( this->length() < col_idx + col_size ) {
		    col_size = this->length() - col_idx;
		}
		if ( col_idx < 0 ) col_begin = - col_idx;
		col_ok = true;
	    }
	}
	else {				// 2次元の場合
	    col_size = src_img.col_length();
	    if ( col_idx + col_size <= 0 || 
		 (0 <= col_idx && this->col_length() <= abs_sz2z(col_idx)) ) {
		goto quit;
	    }
	    else {
		if ( this->col_length() < col_idx + col_size ) {
		    col_size = this->col_length() - col_idx;
		}
		if ( col_idx < 0 ) col_begin = - col_idx;
		col_ok = true;
	    }
	}
	*/
    }
    else {
	layer_idx = 0;
	layer_size = 1;
	layer_begin = 0;
	layer_ok = false;
	row_idx = 0;
	row_size = 1;
	row_begin = 0;
	row_ok = false;
	col_idx = 0;
	col_size = 1;
	col_begin = 0;
    }

    if ( col_ok == true ) {
	if ( *r_col_idx   == col_idx &&
	     *r_row_idx   == row_idx &&
	     *r_layer_idx == layer_idx ) {
	    return_status = 0;
	}
	else {
	    return_status = 1;	/* flag for fixed */
	}
    }

    *r_col_idx = col_idx;
    *r_row_idx = row_idx;
    *r_layer_idx = layer_idx;
    *r_col_ok = col_ok;
    *r_row_ok = row_ok;
    *r_layer_ok = layer_ok;
    *r_col_size = col_size;
    *r_row_size = row_size;
    *r_layer_size = layer_size;
    *r_col_begin = col_begin;
    *r_row_begin = row_begin;
    *r_layer_begin = layer_begin;

 quit:
    return return_status;
}

/* */

/**
 * @brief   ptrのアドレスが自身のデータ領域内かをチェック
 * 
 * @param   ptr チェックするアドレス
 * @return  自身の領域内の時は真，<br>
 *          それ以外の時は偽
 * @note    このメンバ関数は private です．
 */
bool mdarray::is_my_buffer( const void *ptr ) const
{
    const unsigned char *p0 = (const unsigned char *)(this->data_ptr_cs());
    const unsigned char *p1 =
	(const unsigned char *)(this->data_ptr_cs()) + this->byte_length();
    if ( ptr == NULL || this->data_ptr_cs() == NULL ) return false;
    if ( p0 <= (const unsigned char *)ptr && (const unsigned char *)ptr < p1 )
	return true;
    else
	return false;
}

/**
 * @brief  _arr_rec で使用するメモリの再確保
 * @note   このメンバ関数は private です．
 */
int mdarray::realloc_arr_rec( size_t len_bytes )
{
    this->cleanup_shallow_copy(true);	/* バッファを直接操作するため呼ぶ */

    int strategy;
    size_t len_alloc;

    if ( this->_arr_rec == NULL && len_bytes == 0 ) return 0;

    /* "auto" が設定されている時は，auto_resize で決める */
    if ( this->alloc_strategy_rec == Alloc_strategy_auto ) {
	if ( this->auto_resize() == true ) strategy = Alloc_strategy_pow;
	else strategy = Alloc_strategy_min;
    }
    else {
	strategy = this->alloc_strategy_rec;
    }

    /* "pow" が設定されていて，1GB 未満の時 */
    if ( strategy == Alloc_strategy_pow && 
	 0 < len_bytes && len_bytes < (size_t)1024*1024*1024 ) {
	if ( len_bytes <= 32 ) {		/* 要求が小さい場合 */
	    len_alloc = 32;
	}
	else {					/* 2^n でとるように手配 */
	    const double base = 2.0;
	    size_t nn = (size_t)c_ceil( log((double)len_bytes) / log(base) );
	    size_t len = (size_t)pow(base, (double)nn);
	    /* 念のためチェック */
	    if ( len < len_bytes ) {
		len = (size_t)pow(base, (double)(nn + 1));
		if ( len < len_bytes ) {
		    err_throw(__FUNCTION__,"FATAL","internal error");
		}
	    }
	    /* */
	    len_alloc = len;
	}
    }
    else {
	len_alloc = len_bytes;
    }

    //err_report1(__FUNCTION__,"DEBUG","strategy = %d", strategy);
    //err_report1(__FUNCTION__,"DEBUG"," len_bytes = %zd", len_bytes);
    //err_report1(__FUNCTION__,"DEBUG"," len_alloc = %zd", len_alloc);

    if ( this->arr_alloc_blen_rec != len_alloc ) {

	void *tmp_ptr;

	tmp_ptr = realloc(this->_arr_rec, len_alloc);
	if ( tmp_ptr == NULL && 0 < len_alloc ) return -1;
	else {
	    this->_arr_rec = tmp_ptr;
	    this->arr_alloc_blen_rec = len_alloc;
	    return 0;
	}

    }
    else {

	return 0;

    }

}

/**
 * @brief  _arr_rec で使用しているメモリを開放して NULL 値をセット
 * @note   このメンバ関数は protected です．
 */
void mdarray::free_arr_rec()
{
    this->cleanup_shallow_copy(true);	/* バッファを直接操作するため呼ぶ */

    if ( this->_arr_rec != NULL ) {
	free(this->_arr_rec);
	this->_arr_rec = NULL;
	this->arr_alloc_blen_rec = 0;
    }
    return;
}

/**
 * @brief  _default_rec で使用するメモリの再確保
 * @note   このメンバ関数は private です．
 */
int mdarray::realloc_default_rec( size_t len_bytes )
{
    this->cleanup_shallow_copy(true);	/* バッファを直接操作するため呼ぶ */

    void *tmp_ptr;

    if ( this->_default_rec == NULL && len_bytes == 0 ) return 0;

    tmp_ptr = realloc(this->_default_rec, len_bytes);
    if ( tmp_ptr == NULL && 0 < len_bytes ) return -1;
    else {
	this->_default_rec = tmp_ptr;
	return 0;
    }
}

/**
 * @brief  _size_rec で使用するメモリの再確保
 * @note   このメンバ関数は private です．
 */
int mdarray::realloc_size_rec( size_t len_elements )
{
    this->cleanup_shallow_copy(true);	/* バッファを直接操作するため呼ぶ */

    void *tmp_ptr;

    if ( this->_size_rec == NULL && len_elements == 0 ) return 0;
    tmp_ptr = realloc(this->_size_rec, sizeof(size_t)*len_elements);
    if ( tmp_ptr == NULL && 0 < len_elements ) return -1;
    else {
	this->_size_rec = (size_t *)tmp_ptr;
	return 0;
    }
}

/**
 * @brief  _default_rec で使用しているメモリを開放して NULL 値をセット
 * @note   このメンバ関数は private です．
 */
void mdarray::free_default_rec()
{
    this->cleanup_shallow_copy(true);	/* バッファを直接操作するため呼ぶ */

    if ( this->_default_rec != NULL ) {
	free(this->_default_rec);
	this->_default_rec = NULL;
    }

    return;
}

/**
 * @brief  _size_rec で使用しているメモリを開放して NULL 値をセット
 * @note   このメンバ関数は private です．
 */
void mdarray::free_size_rec()
{
    this->cleanup_shallow_copy(true);	/* バッファを直接操作するため呼ぶ */

    if ( this->_size_rec != NULL ) {
	free(this->_size_rec);
	this->_size_rec = NULL;
    }
    return;
}


/**
 * @brief  _arr_ptr_2d_rec を更新
 *
 * @param  on_2d trueの場合，_arr_ptr_2d_rec の内容を新規に作成します．
 *               falseの場合，_arr_ptr_2d_rec がnon-NULLなら内容を更新します．
 * 
 * @note   このメンバ関数は private です．
 */
void mdarray::update_arr_ptr_2d( bool on_2d )
{
    this->cleanup_shallow_copy(true);	/* バッファを直接操作するため呼ぶ */

    this->_update_arr_ptr_2d(on_2d);
}

/**
 * @brief  _arr_ptr_2d_rec を更新 (低レベル)
 *
 * @note   このメンバ関数は private です．
 */
void mdarray::_update_arr_ptr_2d( bool on_2d )
{
    /*
     * 注意: このメンバ関数は低レベルなので，この中で書き込み用のメンバ関数は
     *       使ってはならない．
     */

    const size_t this_bytes = this->bytes();
    const size_t step = this_bytes * this->cached_col_length_rec;

    if ( on_2d == true || 
	 this->_arr_ptr_2d_rec != NULL || this->extptr_2d_rec != NULL ) {
	const size_t buf_len = this->cached_row_layer_length_rec + 1;
	char *arr_p = (char *)(this->_arr_rec);
	void *tmp_ptr;
	size_t i;
	i = 0;
	if ( this->_arr_ptr_2d_rec != NULL ) {
	    for ( ; this->_arr_ptr_2d_rec[i] != NULL ; i++ ) {
		if ( i+1 < buf_len ) {
		    this->_arr_ptr_2d_rec[i] = (void *)arr_p;
		    arr_p += step;
		}
		else break;
	    }
	}
	if ( this->_arr_ptr_2d_rec == NULL /* 最初の場合 */ ||
	     i+1 < buf_len /* バッファが足りない場合 */ || 
	     this->_arr_ptr_2d_rec[i] != NULL /* バッファが多い場合 */ ) {
	    tmp_ptr = realloc(this->_arr_ptr_2d_rec,
			      sizeof(*(this->_arr_ptr_2d_rec)) * buf_len);
	    if ( tmp_ptr == NULL ) {
		err_throw(__FUNCTION__,"FATAL","realloc() failed");
	    }
	    this->_arr_ptr_2d_rec = (void **)tmp_ptr;
	}
	for ( ; i+1 < buf_len ; i++ ) {
	    this->_arr_ptr_2d_rec[i] = (void *)arr_p;
	    arr_p += step;
	}
	this->_arr_ptr_2d_rec[i] = NULL;				/* terminate */
    }

    /* 外部ポインタの更新 */
    if ( this->extptr_2d_rec != NULL ) 
	*(this->extptr_2d_rec) = this->_arr_ptr_2d_rec;

    return;
}

/**
 * @brief  _arr_ptr_3d_rec を更新
 *
 * @param  on_3d trueの場合，_arr_ptr_3d_rec の内容を新規に構築します．
 *               falseの場合，_arr_ptr_3d_rec がnon-NULLなら内容を更新します．
 * 
 * @note   このメンバ関数は private です．
 */
void mdarray::update_arr_ptr_3d( bool on_3d )
{
    this->cleanup_shallow_copy(true);	/* バッファを直接操作するため呼ぶ */

    this->_update_arr_ptr_3d(on_3d);
}

/**
 * @brief  _arr_ptr_3d_rec を更新 (低レベル)
 *
 * @note   このメンバ関数は private です．
 */
void mdarray::_update_arr_ptr_3d( bool on_3d )
{
    /*
     * 注意: このメンバ関数は低レベルなので，この中で書き込み用のメンバ関数は
     *       使ってはならない．
     */

    const size_t this_bytes = this->bytes();
    const size_t step = this_bytes * this->cached_col_length_rec;

    if ( on_3d == true || 
	 this->_arr_ptr_3d_rec != NULL || this->extptr_3d_rec != NULL ) {
	const size_t buf_len1 = this->cached_layer_length_rec + 1;
	const size_t buf_len0 = this->cached_row_length_rec + 1;
	char *arr_p = (char *)(this->_arr_rec);
	bool flag_buf_is_large = false;
	void *tmp_ptr;
	size_t i;
	i = 0 ;
	if ( this->_arr_ptr_3d_rec != NULL ) {
	    /* 各行の先頭アドレスを登録 */
	    /* NULL が来るまで続ける */
	    for ( ; this->_arr_ptr_3d_rec[i] != NULL ; i++ ) {
		size_t j;
		if ( i+1 < buf_len1 ) {
		    /* 行の先頭アドレスが入るアドレステーブル */
		    for ( j=0 ; this->_arr_ptr_3d_rec[i][j] != NULL ; j++ ) {
			if ( j+1 < buf_len0 ) {
			    this->_arr_ptr_3d_rec[i][j] = (void *)arr_p;
			    arr_p += step;
			}
			else break;
		    }
		    if (j+1 < buf_len0 || this->_arr_ptr_3d_rec[i][j] != NULL) {
			tmp_ptr = realloc(this->_arr_ptr_3d_rec[i], 
				sizeof(*(this->_arr_ptr_3d_rec[i])) * buf_len0);
			if ( tmp_ptr == NULL ) {
			    err_throw(__FUNCTION__,"FATAL","realloc() failed");
			}
			this->_arr_ptr_3d_rec[i] = (void **)tmp_ptr;
		    }
		    for ( ; j+1 < buf_len0 ; j++ ) {
			this->_arr_ptr_3d_rec[i][j] = (void *)arr_p;
			arr_p += step;
		    }
		    this->_arr_ptr_3d_rec[i][j] = NULL;		/* terminate */
		}
		else {
		    size_t ii;
		    free(this->_arr_ptr_3d_rec[i]);
		    this->_arr_ptr_3d_rec[i] = NULL;
		    for ( ii=i+1 ; this->_arr_ptr_3d_rec[ii] != NULL ; ii++ ) {
			/* 不要になった部分 */
			free(this->_arr_ptr_3d_rec[ii]);
		    }
		    flag_buf_is_large = true;
		    break;
		}
	    }
	}
	/* サイズが変更になっていた場合はrealloc()する */
	if ( this->_arr_ptr_3d_rec == NULL /* 最初の場合 */ ||
	     i+1 < buf_len1 /* バッファが足りない場合 */ || 
	     flag_buf_is_large == true /* バッファが多い場合 */ ) {
	    /* レイヤの先頭アドレスが入るアドレステーブル */
	    tmp_ptr = realloc(this->_arr_ptr_3d_rec,
			      sizeof(*(this->_arr_ptr_3d_rec)) * buf_len1);
	    if ( tmp_ptr == NULL ) {
		err_throw(__FUNCTION__,"FATAL","realloc() failed");
	    }
	    this->_arr_ptr_3d_rec = (void ***)tmp_ptr;
	}
	/* 各行の先頭アドレスを登録(途中までしか登録できなかった場合の続き) */
	for ( ; i+1 < buf_len1 ; i++ ) {
	    size_t j;
	    this->_arr_ptr_3d_rec[i] = NULL;
	    /* 行の先頭アドレスが入るアドレステーブル */
	    tmp_ptr = realloc(this->_arr_ptr_3d_rec[i], 
			      sizeof(*(this->_arr_ptr_3d_rec[i])) * buf_len0);
	    if ( tmp_ptr == NULL ) {
		err_throw(__FUNCTION__,"FATAL","realloc() failed");
	    }
	    this->_arr_ptr_3d_rec[i] = (void **)tmp_ptr;
	    for ( j=0 ; j+1 < buf_len0 ; j++ ) {
		this->_arr_ptr_3d_rec[i][j] = (void *)arr_p;
		arr_p += step;
	    }
	    this->_arr_ptr_3d_rec[i][j] = NULL;			/* terminate */
	}
	this->_arr_ptr_3d_rec[i] = NULL;				/* terminate */
    }

    /* 外部ポインタの更新 */
    if ( this->extptr_3d_rec != NULL ) 
	*(this->extptr_3d_rec) = this->_arr_ptr_3d_rec;

    return;
}

/**
 * @brief  _arr_ptr_2d_rec で使用しているメモリを開放して NULL 値をセット
 *
 * @note このメンバ関数は private です．
 */
void mdarray::free_arr_ptr_2d()
{
    this->cleanup_shallow_copy(true);	/* バッファを直接操作するため呼ぶ */

    if ( this->_arr_ptr_2d_rec != NULL ) {
	free(this->_arr_ptr_2d_rec);
	this->_arr_ptr_2d_rec = NULL;
    }
    return;
}

/**
 * @brief  _arr_ptr_3d_rec で使用しているメモリを開放して NULL 値をセット
 *
 * @note このメンバ関数は private です．
 */
void mdarray::free_arr_ptr_3d()
{
    this->cleanup_shallow_copy(true);	/* バッファを直接操作するため呼ぶ */

    if ( this->_arr_ptr_3d_rec != NULL ) {
	size_t i = 0;
	while ( this->_arr_ptr_3d_rec[i] != NULL ) {
	    free(this->_arr_ptr_3d_rec[i]);
	    i++;
	}
	free(this->_arr_ptr_3d_rec);
	this->_arr_ptr_3d_rec = NULL;
    }
    return;
}

/**
 * @brief  inlineメンバ関数で throw する時に使用
 *
 */
void mdarray::err_throw_int_arg( const char *fncname, const char *lev, 
				 const char *mes, int v ) const
{
    err_throw1(fncname, lev, mes, v);
    return;
}


/**
 * @brief  コンパイル時にエラーを出力させるためのダミーのメンバ関数
 * @deprecated  使用不可
 */
mdarray::mdarray( ssize_t sz_type, int naxis0 )
{
    err_throw(__FUNCTION__,"FATAL","do not use");
}

/**
 * @brief  コンパイル時にエラーを出力させるためのダミーのメンバ関数
 * @deprecated  使用不可
 */
mdarray::mdarray( ssize_t sz_type, size_t naxis0 )
{
    err_throw(__FUNCTION__,"FATAL","do not use");
}

/**
 * @brief  コンパイル時にエラーを出力させるためのダミーのメンバ関数
 * @deprecated  使用不可
 */
mdarray &mdarray::init( ssize_t sz_type, int naxis0 )
{
    err_throw(__FUNCTION__,"FATAL","do not use");
    return *this;
}

/**
 * @brief  コンパイル時にエラーを出力させるためのダミーのメンバ関数
 * @deprecated  使用不可
 */
mdarray &mdarray::init( ssize_t sz_type, size_t naxis0 )
{
    err_throw(__FUNCTION__,"FATAL","do not use");
    return *this;
}


}	/* namespace */

#include "private/c_trunc.cc"
#include "private/c_round.cc"
#include "private/c_floor.cc"
#include "private/c_ceil.cc"

#include "private/c_strcmp.cc"
#include "private/c_strtol.cc"
#include "private/c_strtoul.cc"
#include "private/c_snprintf.c"
#include "private/c_vasprintf.c"

#include "private/c_memset.cc"
#include "private/c_memmove.cc"
#include "private/c_memcpy.cc"
#include "private/memfill.cc"

/* SSE2 */
#include "private/s_memset.cc"
#include "private/s_memfill.cc"
#include "private/s_memcpy.cc"
#include "private/s_memmove.cc"
#include "private/s_memflip.cc"
#include "private/s_memswap.cc"
#include "private/s_byteswap.cc"
