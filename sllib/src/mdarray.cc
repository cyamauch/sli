/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2018-05-28 02:30:39 cyamauch> */

#define CLASS_NAME "mdarray"

/**
 * @file   mdarray.cc
 * @brief  ¿��������򰷤�����δ��쥯�饹 mdarray �Υ�����
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

/* shallow copy �ط��� debug �� */
//#define debug_report_scopy(arg) err_report(__FUNCTION__,"DEBUG",arg)
#define debug_report_scopy(arg)
//#define debug_report_scopy1(a1,a2) err_report1(__FUNCTION__,"DEBUG",a1,a2)
#define debug_report_scopy1(a1,a2)

namespace sli
{

/**
 * @brief  ssize_t ���ͤ������ꡤsize_t �������ͤ��֤� (������)
 *
 * @note   private �ʥޥ���Ǥ���
 */
#define abs_sz2z(val) ( ((val) < 0) ? (size_t)(0-(val)) : (size_t)(val) )

/**
 * @brief  size_type(mdarray�η�����) ����Х���Ĺ���Ѵ� (������)
 *
 * @note   private �ʴؿ��Ǥ���
 */
inline static size_t zt2bytes( ssize_t sz_type )
{
    if ( 0 <= sz_type ) return (size_t)sz_type;
    else {
	size_t ret = (size_t)((ssize_t)0 - sz_type);
	if ( ret % 2 != 0 ) ret++;		/* ʣ�ǿ��ξ�� */
	return ret;
    }
}

/*
 * ��ư������ �� ���� �Ѵ��Ѵؿ�
 */

/* float */

/**
 * @brief  float ������ unsigned char ���ؤ��Ѵ� (�������ʲ����ڼΤ�)
 *
 * @note   private �ʴؿ��Ǥ���
 */
inline static unsigned char tnc_f2c( float v )
{
    return (unsigned char)v;
}
/**
 * @brief  float ������ unsigned char ���ؤ��Ѵ� (�������ʲ���ͼθ���)
 *
 * @note   private �ʴؿ��Ǥ���
 */
inline static unsigned char rnd_f2c( float v )
{
    if ( v < 0 ) return (unsigned char)(v-0.5);
    else return (unsigned char)(v+0.5);
}

/**
 * @brief  float ������ int16_t ���ؤ��Ѵ� (�������ʲ����ڼΤ�)
 *
 * @note   private �ʴؿ��Ǥ���
 */
inline static int16_t tnc_f2i16( float v )
{
    return (int16_t)v;
}
/**
 * @brief  float ������ int16_t ���ؤ��Ѵ� (�������ʲ���ͼθ���)
 *
 * @note   private �ʴؿ��Ǥ���
 */
inline static int16_t rnd_f2i16( float v )
{
    if ( v < 0 ) return (int16_t)(v-0.5);
    else return (int16_t)(v+0.5);
}

/**
 * @brief  float ������ int32_t ���ؤ��Ѵ� (�������ʲ����ڼΤ�)
 *
 * @note   private �ʴؿ��Ǥ���
 */
inline static int32_t tnc_f2i32( float v )
{
    return (int32_t)v;
}
/**
 * @brief  float ������ int32_t ���ؤ��Ѵ� (�������ʲ���ͼθ���)
 *
 * @note   private �ʴؿ��Ǥ���
 */
inline static int32_t rnd_f2i32( float v )
{
    if ( v < 0 ) return (int32_t)(v-0.5);
    else return (int32_t)(v+0.5);
}

/**
 * @brief  float ������ int64_t ���ؤ��Ѵ� (�������ʲ����ڼΤ�)
 *
 * @note   private �ʴؿ��Ǥ���
 */
inline static int64_t tnc_f2i64( float v )
{
    return (int64_t)v;
}
/**
 * @brief  float ������ int64_t ���ؤ��Ѵ� (�������ʲ���ͼθ���)
 *
 * @note   private �ʴؿ��Ǥ���
 */
inline static int64_t rnd_f2i64( float v )
{
    if ( v < 0 ) return (int64_t)(v-0.5);
    else return (int64_t)(v+0.5);
}

/* double */

/**
 * @brief  double ������ unsigned char ���ؤ��Ѵ� (�������ʲ����ڼΤ�)
 *
 * @note   private �ʴؿ��Ǥ���
 */
inline static unsigned char tnc_d2c( double v )
{
    return (unsigned char)v;
}
/**
 * @brief  double ������ unsigned char ���ؤ��Ѵ� (�������ʲ���ͼθ���)
 *
 * @note   private �ʴؿ��Ǥ���
 */
inline static unsigned char rnd_d2c( double v )
{
    if ( v < 0 ) return (unsigned char)(v-0.5);
    else return (unsigned char)(v+0.5);
}

/**
 * @brief  double ������ int16_t ���ؤ��Ѵ� (�������ʲ����ڼΤ�)
 *
 * @note   private �ʴؿ��Ǥ���
 */
inline static int16_t tnc_d2i16( double v )
{
    return (int16_t)v;
}
/**
 * @brief  double ������ int16_t ���ؤ��Ѵ� (�������ʲ���ͼθ���)
 *
 * @note   private �ʴؿ��Ǥ���
 */
inline static int16_t rnd_d2i16( double v )
{
    if ( v < 0 ) return (int16_t)(v-0.5);
    else return (int16_t)(v+0.5);
}

/**
 * @brief  double ������ int32_t ���ؤ��Ѵ� (�������ʲ����ڼΤ�)
 *
 * @note   private �ʴؿ��Ǥ���
 */
inline static int32_t tnc_d2i32( double v )
{
    return (int32_t)v;
}
/**
 * @brief  double ������ int32_t ���ؤ��Ѵ� (�������ʲ���ͼθ���)
 *
 * @note   private �ʴؿ��Ǥ���
 */
inline static int32_t rnd_d2i32( double v )
{
    if ( v < 0 ) return (int32_t)(v-0.5);
    else return (int32_t)(v+0.5);
}

/**
 * @brief  double ������ int64_t ���ؤ��Ѵ� (�������ʲ����ڼΤ�)
 *
 * @note   private �ʴؿ��Ǥ���
 */
inline static int64_t tnc_d2i64( double v )
{
    return (int64_t)v;
}
/**
 * @brief  double ������ int64_t ���ؤ��Ѵ� (�������ʲ���ͼθ���)
 *
 * @note   private �ʴؿ��Ǥ���
 */
inline static int64_t rnd_d2i64( double v )
{
    if ( v < 0 ) return (int64_t)(v-0.5);
    else return (int64_t)(v+0.5);
}

/* long double */

/**
 * @brief  long double ������ unsigned char ���ؤ��Ѵ� (�������ʲ����ڼΤ�)
 *
 * @note   private �ʴؿ��Ǥ���
 */
inline static unsigned char tnc_ld2c( long double v )
{
    return (unsigned char)v;
}
/**
 * @brief  long double ������ unsigned char ���ؤ��Ѵ� (�������ʲ���ͼθ���)
 *
 * @note   private �ʴؿ��Ǥ���
 */
inline static unsigned char rnd_ld2c( long double v )
{
    if ( v < 0 ) return (unsigned char)(v-0.5);
    else return (unsigned char)(v+0.5);
}

/**
 * @brief  long double ������ int16_t ���ؤ��Ѵ� (�������ʲ����ڼΤ�)
 *
 * @note   private �ʴؿ��Ǥ���
 */
inline static int16_t tnc_ld2i16( long double v )
{
    return (int16_t)v;
}
/**
 * @brief  long double ������ int16_t ���ؤ��Ѵ� (�������ʲ���ͼθ���)
 *
 * @note   private �ʴؿ��Ǥ���
 */
inline static int16_t rnd_ld2i16( long double v )
{
    if ( v < 0 ) return (int16_t)(v-0.5);
    else return (int16_t)(v+0.5);
}

/**
 * @brief  long double ������ int32_t ���ؤ��Ѵ� (�������ʲ����ڼΤ�)
 *
 * @note   private �ʴؿ��Ǥ���
 */
inline static int32_t tnc_ld2i32( long double v )
{
    return (int32_t)v;
}
/**
 * @brief  long double ������ int32_t ���ؤ��Ѵ� (�������ʲ���ͼθ���)
 *
 * @note   private �ʴؿ��Ǥ���
 */
inline static int32_t rnd_ld2i32( long double v )
{
    if ( v < 0 ) return (int32_t)(v-0.5);
    else return (int32_t)(v+0.5);
}

/**
 * @brief  long double ������ int64_t ���ؤ��Ѵ� (�������ʲ����ڼΤ�)
 *
 * @note   private �ʴؿ��Ǥ���
 */
inline static int64_t tnc_ld2i64( long double v )
{
    return (int64_t)v;
}
/**
 * @brief  long double ������ int64_t ���ؤ��Ѵ� (�������ʲ���ͼθ���)
 *
 * @note   private �ʴؿ��Ǥ���
 */
inline static int64_t rnd_ld2i64( long double v )
{
    if ( v < 0 ) return (int64_t)(v-0.5);
    else return (int64_t)(v+0.5);
}

/* fcomplex */

/**
 * @brief  fcomplex ������ unsigned char ���ؤ��Ѵ�
 *         (�¿����ξ������ʲ����ڼΤ�)
 *
 * @note   private �ʴؿ��Ǥ���
 */
inline static unsigned char tnc_fx2c( fcomplex v )
{
    /* �¿����Τ��Ѵ����� */
    return (unsigned char)(creal( v ));
}
/**
 * @brief  fcomplex ������ unsigned char ���ؤ��Ѵ�
 *         (�¿����ξ������ʲ���ͼθ���)
 *
 * @note   private �ʴؿ��Ǥ���
 */
inline static unsigned char rnd_fx2c( fcomplex v )
{
    /* �¿����Τ��Ѵ����� */
    float freal = creal( v );
    
    if ( freal < 0.0 ) return (unsigned char)(freal-0.5);
    else return (unsigned char)(freal+0.5);
}

/**
 * @brief  fcomplex ������ int16_t ���ؤ��Ѵ�
 *         (�¿����ξ������ʲ����ڼΤ�)
 *
 * @note   private �ʴؿ��Ǥ���
 */
inline static int16_t tnc_fx2i16( fcomplex v )
{
    /* �¿����Τ��Ѵ����� */
    return (int16_t)(creal( v ));
}
/**
 * @brief  fcomplex ������ int16_t ���ؤ��Ѵ�
 *         (�¿����ξ������ʲ���ͼθ���)
 *
 * @note   private �ʴؿ��Ǥ���
 */
inline static int16_t rnd_fx2i16( fcomplex v )
{
    /* �¿����Τ��Ѵ����� */
    float freal = creal( v );

    if ( freal < 0.0 ) return (int16_t)(freal-0.5);
    else return (int16_t)(freal+0.5);
}

/**
 * @brief  fcomplex ������ int32_t ���ؤ��Ѵ�
 *         (�¿����ξ������ʲ����ڼΤ�)
 *
 * @note   private �ʴؿ��Ǥ���
 */
inline static int32_t tnc_fx2i32( fcomplex v )
{
    /* �¿����Τ��Ѵ����� */
    return (int32_t)(creal( v ));
}
/**
 * @brief  fcomplex ������ int32_t ���ؤ��Ѵ�
 *         (�¿����ξ������ʲ���ͼθ���)
 *
 * @note   private �ʴؿ��Ǥ���
 */
inline static int32_t rnd_fx2i32( fcomplex v )
{
    /* �¿����Τ��Ѵ����� */
    float freal = creal( v );

    if ( freal < 0.0 ) return (int32_t)(freal-0.5);
    else return (int32_t)(freal+0.5);
}

/**
 * @brief  fcomplex ������ int64_t ���ؤ��Ѵ�
 *         (�¿����ξ������ʲ����ڼΤ�)
 *
 * @note   private �ʴؿ��Ǥ���
 */
inline static int64_t tnc_fx2i64( fcomplex v )
{
    /* �¿����Τ��Ѵ����� */
    return (int64_t)(creal( v ));
}
/**
 * @brief  fcomplex ������ int64_t ���ؤ��Ѵ�
 *         (�¿����ξ������ʲ���ͼθ���)
 *
 * @note   private �ʴؿ��Ǥ���
 */
inline static int64_t rnd_fx2i64( fcomplex v )
{
    /* �¿����Τ��Ѵ����� */
    float freal = creal( v );

    if ( freal < 0.0 ) return (int64_t)(freal-0.5);
    else return (int64_t)(freal+0.5);
}

/* dcomplex */

/**
 * @brief  dcomplex ������ unsigned char ���ؤ��Ѵ�
 *         (�¿����ξ������ʲ����ڼΤ�)
 *
 * @note   private �ʴؿ��Ǥ���
 */
inline static unsigned char tnc_dx2c( dcomplex v )
{
    /* �¿����Τ��Ѵ����� */
    return (unsigned char)(creal( v ));
}
/**
 * @brief  dcomplex ������ unsigned char ���ؤ��Ѵ�
 *         (�¿����ξ������ʲ���ͼθ���)
 *
 * @note   private �ʴؿ��Ǥ���
 */
inline static unsigned char rnd_dx2c( dcomplex v )
{
    /* �¿����Τ��Ѵ����� */
    double dreal = creal( v );

    if ( dreal < 0 ) return (unsigned char)(dreal-0.5);
    else return (unsigned char)(dreal+0.5);
}

/**
 * @brief  dcomplex ������ int16_t ���ؤ��Ѵ�
 *         (�¿����ξ������ʲ����ڼΤ�)
 *
 * @note   private �ʴؿ��Ǥ���
 */
inline static int16_t tnc_dx2i16( dcomplex v )
{
    /* �¿����Τ��Ѵ����� */
    return (int16_t)(creal( v ));
}
/**
 * @brief  dcomplex ������ int16_t ���ؤ��Ѵ�
 *         (�¿����ξ������ʲ���ͼθ���)
 *
 * @note   private �ʴؿ��Ǥ���
 */
inline static int16_t rnd_dx2i16( dcomplex v )
{
    /* �¿����Τ��Ѵ����� */
    double dreal = creal( v );

    if ( dreal < 0 ) return (int16_t)(dreal-0.5);
    else return (int16_t)(dreal+0.5);
}

/**
 * @brief  dcomplex ������ int32_t ���ؤ��Ѵ�
 *         (�¿����ξ������ʲ����ڼΤ�)
 *
 * @note   private �ʴؿ��Ǥ���
 */
inline static int32_t tnc_dx2i32( dcomplex v )
{
    /* �¿����Τ��Ѵ����� */
    return (int32_t)(creal( v ));
}
/**
 * @brief  dcomplex ������ int32_t ���ؤ��Ѵ�
 *         (�¿����ξ������ʲ���ͼθ���)
 *
 * @note   private �ʴؿ��Ǥ���
 */
inline static int32_t rnd_dx2i32( dcomplex v )
{
    /* �¿����Τ��Ѵ����� */
    double dreal = creal( v );

    if ( dreal < 0 ) return (int32_t)(dreal-0.5);
    else return (int32_t)(dreal+0.5);
}

/**
 * @brief  dcomplex ������ int64_t ���ؤ��Ѵ�
 *         (�¿����ξ������ʲ����ڼΤ�)
 *
 * @note   private �ʴؿ��Ǥ���
 */
inline static int64_t tnc_dx2i64( dcomplex v )
{
    /* �¿����Τ��Ѵ����� */
    return (int64_t)(creal( v ));
}
/**
 * @brief  dcomplex ������ int64_t ���ؤ��Ѵ�
 *         (�¿����ξ������ʲ���ͼθ���)
 *
 * @note   private �ʴؿ��Ǥ���
 */
inline static int64_t rnd_dx2i64( dcomplex v )
{
    /* �¿����Τ��Ѵ����� */
    double dreal = creal( v );

    if ( dreal < 0 ) return (int64_t)(dreal-0.5);
    else return (int64_t)(dreal+0.5);
}

/* ldcomplex */

/**
 * @brief  ldcomplex ������ unsigned char ���ؤ��Ѵ�
 *         (�¿����ξ������ʲ����ڼΤ�)
 *
 * @note   private �ʴؿ��Ǥ���
 */
inline static unsigned char tnc_ldx2c( ldcomplex v )
{
    /* �¿����Τ��Ѵ����� */
    return (unsigned char)(creal( v ));
}
/**
 * @brief  ldcomplex ������ unsigned char ���ؤ��Ѵ�
 *         (�¿����ξ������ʲ���ͼθ���)
 *
 * @note   private �ʴؿ��Ǥ���
 */
inline static unsigned char rnd_ldx2c( ldcomplex v )
{
    /* �¿����Τ��Ѵ����� */
    long double ldreal = creal( v );

    if ( ldreal < 0 ) return (unsigned char)(ldreal-0.5);
    else return (unsigned char)(ldreal+0.5);
}

/**
 * @brief  ldcomplex ������ int16_t ���ؤ��Ѵ�
 *         (�¿����ξ������ʲ����ڼΤ�)
 *
 * @note   private �ʴؿ��Ǥ���
 */
inline static int16_t tnc_ldx2i16( ldcomplex v )
{
    /* �¿����Τ��Ѵ����� */
    return (int16_t)(creal( v ));
}
/**
 * @brief  ldcomplex ������ int16_t ���ؤ��Ѵ�
 *         (�¿����ξ������ʲ���ͼθ���)
 *
 * @note   private �ʴؿ��Ǥ���
 */
inline static int16_t rnd_ldx2i16( ldcomplex v )
{
    /* �¿����Τ��Ѵ����� */
    long double ldreal = creal( v );

    if ( ldreal < 0 ) return (int16_t)(ldreal-0.5);
    else return (int16_t)(ldreal+0.5);
}

/**
 * @brief  ldcomplex ������ int32_t ���ؤ��Ѵ�
 *         (�¿����ξ������ʲ����ڼΤ�)
 *
 * @note   private �ʴؿ��Ǥ���
 */
inline static int32_t tnc_ldx2i32( ldcomplex v )
{
    /* �¿����Τ��Ѵ����� */
    return (int32_t)(creal( v ));
}
/**
 * @brief  ldcomplex ������ int32_t ���ؤ��Ѵ�
 *         (�¿����ξ������ʲ���ͼθ���)
 *
 * @note   private �ʴؿ��Ǥ���
 */
inline static int32_t rnd_ldx2i32( ldcomplex v )
{
    /* �¿����Τ��Ѵ����� */
    long double ldreal = creal( v );

    if ( ldreal < 0 ) return (int32_t)(ldreal-0.5);
    else return (int32_t)(ldreal+0.5);
}

/**
 * @brief  ldcomplex ������ int64_t ���ؤ��Ѵ�
 *         (�¿����ξ������ʲ����ڼΤ�)
 *
 * @note   private �ʴؿ��Ǥ���
 */
inline static int64_t tnc_ldx2i64( ldcomplex v )
{
    /* �¿����Τ��Ѵ����� */
    return (int64_t)(creal( v ));
}
/**
 * @brief  ldcomplex ������ int64_t ���ؤ��Ѵ�
 *         (�¿����ξ������ʲ���ͼθ���)
 *
 * @note   private �ʴؿ��Ǥ���
 */
inline static int64_t rnd_ldx2i64( ldcomplex v )
{
    /* �¿����Τ��Ѵ����� */
    long double ldreal = creal( v );

    if ( ldreal < 0 ) return (int64_t)(ldreal-0.5);
    else return (int64_t)(ldreal+0.5);
}

/* ��ư������ �� "long" or "long long" */

/**
 * @brief  float ������ long ���ؤ��Ѵ� (�������ʲ����ڼΤ�)
 *
 * @note   private �ʴؿ��Ǥ���
 */
inline static long tnc_f2l( float v )
{
    return (long)v;
}
/**
 * @brief  float ������ long ���ؤ��Ѵ� (�������ʲ���ͼθ���)
 *
 * @note   private �ʴؿ��Ǥ���
 */
inline static long rnd_f2l( float v )
{
    if ( v < 0 ) return (long)(v-0.5);
    else return (long)(v+0.5);
}

/**
 * @brief  float ������ long long ���ؤ��Ѵ� (�������ʲ����ڼΤ�)
 *
 * @note   private �ʴؿ��Ǥ���
 */
inline static long long tnc_f2ll( float v )
{
    return (long long)v;
}
/**
 * @brief  float ������ long long ���ؤ��Ѵ� (�������ʲ���ͼθ���)
 *
 * @note   private �ʴؿ��Ǥ���
 */
inline static long long rnd_f2ll( float v )
{
    if ( v < 0 ) return (long long)(v-0.5);
    else return (long long)(v+0.5);
}

/**
 * @brief  double ������ long ���ؤ��Ѵ� (�������ʲ����ڼΤ�)
 *
 * @note   private �ʴؿ��Ǥ���
 */
inline static long tnc_d2l( double v )
{
    return (long)v;
}
/**
 * @brief  double ������ long ���ؤ��Ѵ� (�������ʲ���ͼθ���)
 *
 * @note   private �ʴؿ��Ǥ���
 */
inline static long rnd_d2l( double v )
{
    if ( v < 0 ) return (long)(v-0.5);
    else return (long)(v+0.5);
}

/**
 * @brief  double ������ long long ���ؤ��Ѵ� (�������ʲ����ڼΤ�)
 *
 * @note   private �ʴؿ��Ǥ���
 */
inline static long long tnc_d2ll( double v )
{
    return (long long)v;
}
/**
 * @brief  double ������ long long ���ؤ��Ѵ� (�������ʲ���ͼθ���)
 *
 * @note   private �ʴؿ��Ǥ���
 */
inline static long long rnd_d2ll( double v )
{
    if ( v < 0 ) return (long long)(v-0.5);
    else return (long long)(v+0.5);
}

/**
 * @brief  long double ������ long ���ؤ��Ѵ� (�������ʲ����ڼΤ�)
 *
 * @note   private �ʴؿ��Ǥ���
 */
inline static long tnc_ld2l( long double v )
{
    return (long)v;
}
/**
 * @brief  long double ������ long ���ؤ��Ѵ� (�������ʲ���ͼθ���)
 *
 * @note   private �ʴؿ��Ǥ���
 */
inline static long rnd_ld2l( long double v )
{
    if ( v < 0 ) return (long)(v-0.5);
    else return (long)(v+0.5);
}

/**
 * @brief  long double ������ long long ���ؤ��Ѵ� (�������ʲ����ڼΤ�)
 *
 * @note   private �ʴؿ��Ǥ���
 */
inline static long long tnc_ld2ll( long double v )
{
    return (long long)v;
}
/**
 * @brief  long double ������ long long ���ؤ��Ѵ� (�������ʲ���ͼθ���)
 *
 * @note   private �ʴؿ��Ǥ���
 */
inline static long long rnd_ld2ll( long double v )
{
    if ( v < 0 ) return (long long)(v-0.5);
    else return (long long)(v+0.5);
}

/* ʣ�ǿ� �� ��ư������ */

/**
 * @brief  float ������ fcomplex ���ؤ��Ѵ�
 *
 * @note   private �ʴؿ��Ǥ���
 */
inline static fcomplex f2fx( float v )
{
    return ( (float)v + 0.0*I );
}
/**
 * @brief  double ������ dcomplex ���ؤ��Ѵ�
 *
 * @note   private �ʴؿ��Ǥ���
 */
inline static dcomplex d2dx( double v )
{
    return ( (double)v + 0.0*I );
}
/**
 * @brief  long double ������ ldcomplex ���ؤ��Ѵ�
 *
 * @note   private �ʴؿ��Ǥ���
 */
inline static ldcomplex ld2ldx( long double v )
{
    return ( (long double)v + 0.0*I );
}

/* */

struct mdarray_cnv_nd_prms {
    /* ���饤���Ȥ����(sizeof��Ĺ����Τ�����!) */
    size_t total_len_elements;
    bool round_flg;
};

/* const ���ʤ�������������double, float ���� ���쥤���Ѵ�������˻Ȥ� */
static struct mdarray_cnv_nd_prms Null_cnv_nd_prms = {0, false};

/* �����餫����餫����������ޤäƤ����硥                */
/* f_b = -1 �Ǹ���顤f_b = 1 �������顤f_b = 0 �ǥ����顼�� */
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
 * @brief  �Ƽ������ǿ�����¸���������ʣ�� (������)
 *
 * @note   malloc() �Ǽ��������ΰ���֤����Ȥä���ɬ�� free() �ǳ������롥<br>
 *         private �ʴؿ��Ǥ���
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
 * @brief  ���󥹥ȥ饯���Ѥ����̤ʥ��˥���饤��
 *
 * @param  sz_type �ѿ��η�����
 * @param  is_constructor ���󥹥ȥ饯���ˤ�äƸƤӽФ������� true ��
 *         init()������Ƥ־��� false �򥻥åȤ��롥
 *
 * @note   ���Υ��дؿ��� private �Ǥ���
 *
 */
void mdarray::__force_init( ssize_t sz_type, bool is_constructor )
{
    /* FITS�Х��ʥ�ơ��֥�ǥ����Ĺ0��������ᡤsz_type=0 ��OK�Ȥ��� */
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
	/* ������ .init() �ǤϽ��������ʤ� */
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
 * @brief  ���󥹥ȥ饯��
 *
 */
mdarray::mdarray()
{
    this->__force_init(this->default_size_type(), true);

    return;
}

/**
 * @brief  ���󥹥ȥ饯��
 *
 * @param  sz_type �ѿ��η�����
 * @throw  sz_type���ͤ�̵���ʾ��
 */
mdarray::mdarray( ssize_t sz_type )
{
    this->__force_init(sz_type, true);

    return;
}

/**
 * @brief  ���󥹥ȥ饯��
 *
 * @param  sz_type �ѿ��η�����
 * @throw  sz_type���ͤ�̵���ʾ��
 */
mdarray::mdarray( ssize_t sz_type, bool auto_resize )
{
    this->__force_init(sz_type, true);

    this->set_auto_resize(auto_resize);

    return;
}

/**
 * @brief  ���󥹥ȥ饯��
 *
 * @param      sz_type �ѿ��η�����
 * @param      extptt_ptr �桼���Υݥ����ѿ��Υ��ɥ쥹
 * @throw      sz_type���ͤ�̵���ʾ��
 * @attention  �桼���Υݥ����ѿ�����Ͽ����� shallow copy ����ǽ���ʤ��ʤ�
 *             �ޤ���
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
 * @brief  ���󥹥ȥ饯��
 *
 * @param  sz_type �ѿ��η�����
 * @param  naxisx[] �Ƽ��������ǿ�
 * @param  ndim ���󼡸���
 * @param  init_buf �����ͤ�ǥե�����ͤ�����ʤ� true
 * @throw  sz_type���ͤ�̵���ʾ��
 * @throw  �����Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
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
 * @brief  ���󥹥ȥ饯��
 *
 * @deprecated  ���ߤ�̤����
 * @param  sz_type �ѿ��η�����
 * @param  naxis0 �����ֹ�0 �μ���(1 ������) �����ǿ�
 * @throw  sz_type���ͤ�̵���ʾ��
 * @throw  �����Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
 */
mdarray::mdarray( ssize_t sz_type, size_t naxis0 )
{
    const size_t naxisx[] = {naxis0};

    this->__force_init(sz_type, true);

    this->mdarray::init(sz_type, naxisx, 1, true);

    return;
}

/**
 * @brief  ���󥹥ȥ饯��
 *
 * @deprecated  ���ߤ�̤����
 * @param  sz_type �ѿ��η�����
 * @param  naxis0 �����ֹ�0 �μ���(1 ������) �����ǿ�
 * @param  naxis1 �����ֹ�1 �μ���(2 ������) �����ǿ�
 * @throw  sz_type���ͤ�̵���ʾ��
 * @throw  �����Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
 */
mdarray::mdarray( ssize_t sz_type, size_t naxis0, size_t naxis1 )
{
    const size_t naxisx[] = {naxis0, naxis1};

    this->__force_init(sz_type, true);

    this->mdarray::init(sz_type, naxisx, 2, true);

    return;
}

/**
 * @brief  ���󥹥ȥ饯��
 *
 * @deprecated  ���ߤ�̤����
 * @param  sz_type �ѿ��η�����
 * @param  naxis0 �����ֹ�0 �μ���(1 ������) �����ǿ�
 * @param  naxis1 �����ֹ�1 �μ���(2 ������) �����ǿ�
 * @param  naxis2 �����ֹ�2 �μ���(3 ������) �����ǿ�
 * @throw  sz_type���ͤ�̵���ʾ��
 * @throw  �����Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
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
 * @brief  ���ԡ����󥹥ȥ饯��
 *
 * @param  obj ���ԡ����Ȥʤ륪�֥�������
 * @throw  �����Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
 */
mdarray::mdarray( const mdarray &obj )
{
    this->__force_init(obj.size_type(), true);

    this->mdarray::init(obj);
}

/**
 * @brief  destructor �Ѥ����̤ʥ��곫���ѥ��дؿ�
 *
 * @note   ���Υ��дؿ��� private �Ǥ���
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
 * @brief  �ǥ��ȥ饯��
 *
 */
mdarray::~mdarray()
{
    /* shallow copy ���줿�塤�����˾��Ǥ����� (copy �� src ¦�ν���) */
    if ( this->shallow_copy_dest_obj != NULL ) {
	/* ������¦�����Ǥ����������Τ��� */
	this->shallow_copy_dest_obj->shallow_copy_src_obj = NULL;
	/* �Хåե��ϳ������ƤϤ����ʤ� */
    	debug_report_scopy("destructor not free() [0]");
	return;
    }
    /* shallow copy �� src ���ޤ������Ƥ����� (copy �� dest ¦�ν���) */
    else if ( this->shallow_copy_src_obj != NULL ) {
	/* shallow copy �򥭥�󥻥뤹�� */
	this->shallow_copy_src_obj->cancel_shallow_copy(this);
	/* �Хåե��ϳ������ƤϤ����ʤ� */
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
 * @brief  shallow copy ����ǽ�����֤� (src¦�Ǽ¹�)
 *
 * @param   from_obj �ꥯ�����Ȥ���������¦�Υ��֥������ȤΥ��ɥ쥹
 * @return  shallow copy����ǽ�ʤ鿿<br>
 *          ����ʳ��λ��ϵ�
 * @note    SLLIB�μ����Ǥϡ�shallow copy ����������ȤΤ߲ġ�<br>
 *          ���Υ��дؿ��� private �Ǥ���
 */
bool mdarray::request_shallow_copy( mdarray *from_obj ) const
{
    if ( this->shallow_copy_ok == true ) {
	/* �����Ȥ� shallow copy ��̵�ط��Ǥ����������å� */
	if ( this->shallow_copy_dest_obj == NULL &&
	     this->shallow_copy_src_obj == NULL &&
	     from_obj->shallow_copy_dest_obj == NULL &&
	     from_obj->shallow_copy_src_obj == NULL ) {
	    /* ξ�Ԥδط�����Ͽ */
	    mdarray *thisp = (mdarray *)this;
	    /* ���򼫿ȤΥ����ѿ��˥ޡ��� */
	    /* (��꤬���Ǥޤ��� deep copy �������NULL�ˤʤ�) */
	    thisp->shallow_copy_dest_obj = from_obj;
	    /* ���Ȥ����Υ����ѿ��˥ޡ��� */
	    /* (���Ȥ����Ǥ�����NULL�ˤʤ롥See �ǥ��ȥ饯��) */
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
 * @brief  shallow copy �Υ���󥻥� (src¦�Ǽ¹�)
 * 
 * @param  from_obj �ꥯ�����Ȥ���������¦�Υ��֥������ȤΥ��ɥ쥹
 * @note   from_obj ����Ƥ��ľ��� from_obj �ˤ� __force_init() ��¹Ԥ���
 *         ɬ�פ�����ޤ���<br>
 *         ���Υ��дؿ��� private �Ǥ���
 */
void mdarray::cancel_shallow_copy( mdarray *from_obj ) const
{
    debug_report_scopy1("arg: from_obj = %zx",(size_t)from_obj);

    if ( this->shallow_copy_dest_obj == from_obj ) {
	mdarray *thisp = (mdarray *)this;
	/* ���¦��NULL�� */
	from_obj->shallow_copy_src_obj = NULL;
	/* ���ȤΤ�NULL�� */
	thisp->shallow_copy_dest_obj = NULL;
    }
    else {
 	err_throw(__FUNCTION__,"FATAL","internal error");
    }

    return;
}

/**
 * @brief  ���ȤˤĤ��ơ�shallow copy �ط��ˤĤ��ƤΥ��꡼�󥢥åפ�»�
 *
 * @param  do_deep_copy_for_this shallow copy �Υ���󥻥��ˡ����ȤˤĤ���
 *                               deep copy ��Ԥʤ����� true �򥻥åȤ��롥
 * @note   ���ξ��˸ƤӽФ�ɬ�פ����롥<br>
 *          1. �Хåե��˽񤭹�����<br>
 *          2. �Хåե��Υ��ɥ쥹���֤����<br>
 *          3. __shallow_init(), __deep_init() �ƤӽФ���ľ��<br>
 *         ���Υ��дؿ��� private �Ǥ���
 */
void mdarray::cleanup_shallow_copy( bool do_deep_copy_for_this ) const
{
    //debug_report_scopy1("arg: do_deep_copy_for_this = %d",
    //			(int)do_deep_copy_for_this);

    mdarray *thisp = (mdarray *)this;

    /* ǰ�Τ���... */
    if ( this->__copying == true ) {
	err_report(__FUNCTION__,"WARNING","This should not be reported");
    }

    /* shallow copy �� src ���ޤ������Ƥ�����(dest ¦�ν���)��*/
    /* shallow copy �Υ���󥻥������Ԥʤ� */
    if ( thisp->shallow_copy_src_obj != NULL ) {

	const mdarray &_src_obj = *(thisp->shallow_copy_src_obj);
	mdarray *_dest_obj = thisp;

	/* shallow copy �򥭥�󥻥뤷����������� */
	_src_obj.cancel_shallow_copy(_dest_obj);
	_dest_obj->__force_init(_dest_obj->size_type(), false);

	if ( do_deep_copy_for_this == true ) {
	    /* deep copy ��¹� */
	    _dest_obj->__deep_init(_src_obj);
	}
    }
    /* dest_obj �� shallow copy �� src �ˤʤäƤ�����硤�ڤ�Ϥʤ� */
    else if ( thisp->shallow_copy_dest_obj != NULL ) {

	const mdarray &_src_obj = *thisp;
	mdarray *_dest_obj = thisp->shallow_copy_dest_obj;

	/* shallow copy �򥭥�󥻥뤷����������� */
	_src_obj.cancel_shallow_copy(_dest_obj);
	_dest_obj->__force_init(_dest_obj->size_type(), false);

	/* deep copy ��¹� */
	_dest_obj->__deep_init(_src_obj);
    }

    return;
}

/**
 * @brief  obj �����Ƥ򼫿Ȥ˥��ԡ� (shallow copy; Ķ���٥�)
 *
 *  shallow copy ��Ԥʤ���礫���ǡ����ΰ�ư��Ԥʤ��������Ѥ��롥
 *
 * @param   obj ���ԡ������֥������ȤΥ��ɥ쥹
 * @param   is_move �ǡ�������ƥ�ĤΡְ�ư�פξ�硤true �򥻥å�
 * @return  ���Ȥλ���
 * @note   �����ѿ������˽���������ԡ����롥�Хåե��ϥ��ɥ쥹���������ԡ�
 *         ����롥������ this->shallow_copy_src_obj �����Ͼõ��ʤ���
 *         �Хåե������Ƴ�������ñ��˥Хåե��� share ���Ƥ��ޤ��Τǡ����Ѥ�
 *         ����դ�ɬ�ס�<br>
 *         ���Υ��дؿ���Ȥ����� shallow copy �ط��򥯥꡼��ˤ��Ƥ�����<br>
 *         ���Υ��дؿ��� private �Ǥ���
 */
mdarray &mdarray::__shallow_init( const mdarray &obj, bool is_move )
{
    /*
     *  �����Ǥϡ�this �Ǥ� obj �Ǥ� cleanup_shallow_copy() ��ȤäƤ�����
     *  ��ƤФʤ��褦��ա�
     */

    debug_report_scopy1("arg: obj = %zx",(size_t)(&obj));

    mdarray *const obj_bak = this->shallow_copy_src_obj;

    if ( is_move == false ) {
	/* ������ shallow copy �ξ�硥����ptr�λ��Ѥ϶ػߤ���� */
	if ( this->extptr_rec != NULL || 
	     this->extptr_2d_rec != NULL || this->extptr_3d_rec != NULL ) {
	    err_throw(__FUNCTION__,"FATAL",
		      "Internal error: cannot perform shallow copy");
	}
    }

    /* ��������� */
    this->__force_free();
    this->__force_init(this->size_type(), false);

    mdarray *objp = (mdarray *)(&obj);
    objp->__copying = true;
    this->__copying = true;

    try {

	/* ���ɥ쥹�������ԡ� */
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

	    /* ������ʬ�ϡ��ǡ����ιԤ��褬���ꤷ�Ƥ���ְ�ư�פξ�礷�� */
	    /* ������ʤ�                                                 */

	    /* �����ݥ��󥿤ι��� */

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
 * @brief  obj �����Ƥ򼫿Ȥ˥��ԡ� (deep copy; Ķ���٥�)
 *
 * @param   obj ���ԡ������֥������ȤΥ��ɥ쥹
 * @return  ���Ȥλ���
 * @note   obj ¦�ˤĤ��Ƥϡ�_defalut_rec �ʤɤ�ľ�ܤߤˤ����Τ���ա�<br>
 *         ���Υ��дؿ���Ȥ����� shallow copy �ط��򥯥꡼��ˤ��Ƥ���
 *         ����<br>
 *         ���Υ��дؿ��� private �Ǥ���
 */
mdarray &mdarray::__deep_init( const mdarray &obj )
{
    /*
     *  �����Ǥϡ�obj ¦�� cleanup_shallow_copy() ��ȤäƤ�����
     *  ��ƤФʤ��褦��ա�
     */

    bool needs_update_2d_ptr = true;
    bool needs_update_3d_ptr = true;

    debug_report_scopy1("arg: obj = %zx",(size_t)(&obj));

    mdarray *objp = (mdarray *)(&obj);
    objp->__copying = true;

    try {

	/* ����������Ĺ������������ dest_obj�θ��ߤΥХåե��򤽤Τޤ޻Ȥ� */
	if ( this->size_type() == obj.size_type() &&
	     this->length() == obj.length() ) {

	    bool do_update = true;

	    /* �ǥե�����ͤ򥳥ԡ� */
	    this->assign_default( obj._default_rec );

	    /* �����Ʊ�����ɤ��������å� */
	    if ( this->dim_length() == obj.dim_length() ) {
		size_t i;
		for ( i=0 ; i < obj.dim_length() ; i++ ) {
		    if ( this->length(i) != obj.length(i) ) break;
		}
		if ( i == obj.dim_length() ) {	     /* ������������������� */
		    do_update = false;
		    needs_update_2d_ptr = false;
		    needs_update_3d_ptr = false;
		}
	    }
	    if ( do_update == true ) {
		/* ������Ĺ��Ʊ���ʤΤǡ������Ǥ�������������������� */
		this->reallocate(obj._size_rec, obj.dim_length(), false);
	    }
	
	}
	else {

	    /* ��������� */
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

	/* update_length_info() ��ľ��˸Ƥ�ɬ�פ����� */
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
 * @brief  ���֥������ȤΥ��ԡ��ޤ�������
 *
 *  ���֥������Ȥ���������obj �����Ƥ򤹤٤�(����Ĺ�����ʤ�) ���Ȥ˥��ԡ�
 *  ���ޤ���obj ���ؿ��ˤ�ä��֤����ƥ�ݥ�ꥪ�֥������Ȥξ�硤���Ĥ����
 *  ����� shallow  copy ���Ԥʤ��ޤ���<br>
 *  ���Ȥ�obj�η����ѹ����������ؤ����Ƥ����硤���ʳ��Τ��٤�(����Ĺ���
 *  ��°��)�򥳥ԡ����������ͤΥ��ԡ��ϥ����顼�ͤΡ�=�ױ黻�Ҥ�Ʊ�ͤε�§�ǹ�
 *  �ʤ��ޤ���
 *
 * @param     obj mdarray���饹�Υ��֥�������(���ԡ���)
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
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
 * @brief  current �� src �μ����������Ӥ��ơ��礭������������֤� (������)
 *
 * @param  enalrged current ��� src ���礭���ʤäƤ���� true
 * @note   private �ʴؿ��Ǥ���
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


/* ���δؿ��ϺƵ�Ū�˸ƤФ�� */

/**
 * @brief  mdarray �κƵ��黻���� (+=,-=,*=,/=) (������)
 *
 *  1. ����!=0�ʤ�С��Ƶ�Ū�ˤ��δؿ���ƽФ���<br>
 *  2. Ϳ����줿�黻�Ѵؿ������Ѥ��ơ�src_obj �� dest_obj �Ȥ�黻���롥<br>
 *  3. 2.�����Ǥο����������֤���
 *
 * @param  src_obj   �黻�оݤ� mdarray���֥������� (read)
 * @param  dest_obj  �黻�оݤ� mdarray���֥������� (write)
 * @param  dim_idx   �����ֹ�
 * @param  len_src   obj�����ǤθĿ�
 * @param  len_dest  ���Ȥ����ǤθĿ�
 * @param  pos_src   �ǡ����ɤ߹��߰���
 * @param  pos_dest  �ǡ����񤭹��߰���
 * @param  func_calc �黻�Ѵؿ��Υݥ���
 * @note   private �ʴؿ��Ǥ���
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
    else {	/* ����=0 �Ǥν��� */
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
/* _step0 �� 0 �ʤ饹���顼�Ȥα黻��1 �ʤ�����Ȥα黻 */
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
 * @brief  ���Ȥ�����ˡ����ꤵ�줿�����û� (���٥�)
 *
 *  ���Ȥ������obj�������û����ޤ���<br>
 *   1. �����ȥꥵ�����ξ�硤Ϳ����줿 mdarray �����ǿ��˹�碌�ơ����ǿ���
 *      �ꥵ�������롥<br>
 *   2. �û������Τ���δؿ������򤹤�(SEL_FUNC)��<br>
 *   3. �Ƶ��黻�����δؿ�(x_equal_r)��ƽФ���
 *
 * @param    obj �û��оݤ� mdarray
 * @return   ���Ȥλ���
 * @throw    �����Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
 * @note     ���Υ��дؿ��� protected �Ǥ���
 */
mdarray &mdarray::ope_plus_equal(const mdarray &obj)
{
    return this->ope_plus_equal(obj, this->auto_resize());
}

/**
 * @brief  ���Ȥ�����ˡ����ꤵ�줿�����û� (���٥�)
 * @note   ���Υ��дؿ��� protected �Ǥ���
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

    /* �ؿ������� */
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
    if ( obj.sz_type_rec == org_sz_type && this->sz_type_rec == new_sz_type ) { \
	func_calc = &xeq_n::fncname; \
    }
    SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(SEL_FUNC,,,,,,,,,,,,,,,,,,,,,,,,,,,,else);
#undef SEL_FUNC

    /* �ؿ��������������Ԥʤ� */
    if ( func_calc != NULL ) {
	size_t len_src, len_dest, ndim;

	/* �礭������Ȥ� */
	ndim = this->dim_length();
	if ( obj.dim_length() < ndim ) ndim = obj.dim_length();

	/* �ǽ�ϡ���������������ԥ�������� len_xxx �˥��åȤ��� */
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
 * @brief  ���Ȥ�����ˡ����ꤵ�줿�����û�
 *
 *  �黻�Ҥα�¦(����) �ǻ��ꤵ�줿 mdarray ���饹�Υ��֥������Ȥ�����򼫿Ȥ�
 *  �û����ޤ���<br>
 *  ���ȤȤϰۤʤ뷿�����ꤵ�줿���֥������Ȥ����ꤵ�줿��硤�̾�Υ����顼
 *  �黻�ξ���Ʊ�ͤη��Ѵ��������Ԥʤ��ޤ���<br>
 * �ּ�ư�ꥵ�����⡼�ɡפ����ꤵ��Ƥ����硤�Ƽ����������˴ؤ��Ƽ��Ȥ���
 *  obj �������礭����硤��ưŪ�˥ꥵ�������ޤ���
 *
 * @param     mdarray ���饹�Υ��֥�������
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
 */
mdarray &mdarray::operator+=(const mdarray &obj)
{
    debug_report("(const mdarray &obj)");
    return this->ope_plus_equal(obj);
}


/**
 * @brief  ���Ȥ�����򡤻��ꤵ�줿����Ǹ��� (���٥�)
 *
 *  ���Ȥ������obj������Ǹ������ޤ���<br>
 *   1. �����ȥꥵ�����ξ�硤Ϳ����줿 mdarray �����ǿ��˹�碌�ơ����ǿ���
 *      �ꥵ�������롥<br>
 *   2. ���������Τ���δؿ������򤹤�(SEL_FUNC)��<br>
 *   3. �Ƶ��黻�����δؿ�(x_equal_r)��ƽФ���
 *
 * @param    obj �����оݤ� mdarray���֥�������
 * @return   ���Ȥλ���
 * @throw    �����Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
 * @note     ���Υ��дؿ��� protected �Ǥ���
 */
mdarray &mdarray::ope_minus_equal(const mdarray &obj)
{
    return this->ope_minus_equal(obj, this->auto_resize());
}

/**
 * @brief  ���Ȥ�����򡤻��ꤵ�줿����Ǹ��� (���٥�)
 * @note   ���Υ��дؿ��� protected �Ǥ���
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

    /* �ؿ������� */
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
    if ( obj.sz_type_rec == org_sz_type && this->sz_type_rec == new_sz_type ) { \
	func_calc = &xeq_n::fncname; \
    }
    SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(SEL_FUNC,,,,,,,,,,,,,,,,,,,,,,,,,,,,else);
#undef SEL_FUNC

    /* �ؿ��������������Ԥʤ� */
    if ( func_calc != NULL ) {
	size_t len_src, len_dest, ndim;

	/* �礭������Ȥ� */
	ndim = this->dim_length();
	if ( obj.dim_length() < ndim ) ndim = obj.dim_length();

	/* �ǽ�ϡ���������������ԥ�������� len_xxx �˥��åȤ��� */
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
 * @brief  ���Ȥ�����򡤻��ꤵ�줿����Ǹ���
 *
 *  �黻�Ҥα�¦(����) �ǻ��ꤵ�줿 mdarray ���饹�Υ��֥������Ȥ�����򼫿�
 *  ���鸺�����ޤ���<br>
 *  ���ȤȤϰۤʤ뷿�����ꤵ�줿���֥������Ȥ����ꤵ�줿��硤�̾�Υ����顼
 *  �黻�ξ���Ʊ�ͤη��Ѵ��������Ԥʤ��ޤ���<br>
 *  �ּ�ư�ꥵ�����⡼�ɡפ����ꤵ��Ƥ����硤�Ƽ����������˴ؤ��Ƽ��Ȥ���
 *  obj �������礭����硤��ưŪ�˥ꥵ�������ޤ���
 *
 * @param     mdarray ���饹�Υ��֥�������
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
 */
mdarray &mdarray::operator-=(const mdarray &obj)
{
    debug_report("(const mdarray &obj)");
    return this->ope_minus_equal(obj);
}


/**
 * @brief  ���Ȥ�����ˡ����ꤵ�줿�����軻 (���٥�)
 *
 *  ���Ȥ������obj������Ǿ軻���ޤ���<br>
 *   1. �����ȥꥵ�����ξ�硤Ϳ����줿 mdarray �����ǿ��˹�碌�ơ����ǿ���
 *      �ꥵ�������롥<br>
 *   2. �軻�����Τ���δؿ������򤹤�(SEL_FUNC)��<br>
 *   3. �Ƶ��黻�����δؿ�(x_equal_r)��ƽФ���
 *
 * @param    obj �軻�оݤ� mdarray���֥�������
 * @return   ���Ȥλ���
 * @throw    �����Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
 * @note     ���Υ��дؿ��� protected �Ǥ���
 */
mdarray &mdarray::ope_star_equal(const mdarray &obj)
{
    return this->ope_star_equal(obj, this->auto_resize());
}

/**
 * @brief  ���Ȥ�����ˡ����ꤵ�줿�����軻 (���٥�)
 * @note   ���Υ��дؿ��� protected �Ǥ���
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

    /* �ؿ������� */
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
    if ( obj.sz_type_rec == org_sz_type && this->sz_type_rec == new_sz_type ) { \
	func_calc = &xeq_n::fncname; \
    }
    SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(SEL_FUNC,,,,,,,,,,,,,,,,,,,,,,,,,,,,else);
#undef SEL_FUNC

    /* �ؿ��������������Ԥʤ� */
    if ( func_calc != NULL ) {
	size_t len_src, len_dest, ndim;

	/* �礭������Ȥ� */
	ndim = this->dim_length();
	if ( obj.dim_length() < ndim ) ndim = obj.dim_length();

	/* �ǽ�ϡ���������������ԥ�������� len_xxx �˥��åȤ��� */
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
 * @brief  ���Ȥ�����ˡ����ꤵ�줿�����軻
 *
 *  �黻�Ҥα�¦(����) �ǻ��ꤵ�줿 mdarray ���饹�Υ��֥������Ȥ�����򼫿Ȥ�
 *  �軻���ޤ���<br>
 *  ���ȤȤϰۤʤ뷿�����ꤵ�줿���֥������Ȥ����ꤵ�줿��硤�̾�Υ����顼
 *  �黻�ξ���Ʊ�ͤη��Ѵ��������Ԥʤ��ޤ���<br>
 *  �ּ�ư�ꥵ�����⡼�ɡפ����ꤵ��Ƥ����硤�Ƽ����������˴ؤ��Ƽ��Ȥ���
 *  obj �������礭����硤��ưŪ�˥ꥵ�������ޤ���
 *
 * @param     mdarray ���饹�Υ��֥�������
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
 */
mdarray &mdarray::operator*=(const mdarray &obj)
{
    debug_report("(const mdarray &obj)");
    return this->ope_star_equal(obj);
}


/**
 * @brief  ���Ȥ�����򡤻��ꤵ�줿����ǽ��� (���٥�)
 *
 *  ���Ȥ������obj������ǽ������ޤ���<br>
 *   1. �����ȥꥵ�����ξ�硤Ϳ����줿 mdarray �����ǿ��˹�碌�ơ����ǿ���
 *      �ꥵ�������롥<br>
 *   2. ���������Τ���δؿ������򤹤�(SEL_FUNC)��<br>
 *   3. �Ƶ��黻�����δؿ�(x_equal_r)��ƽФ���
 *
 * @param    obj �����оݤ� mdarray���֥�������
 * @return   ���Ȥλ���
 * @throw    �����Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
 * @note     ���Υ��дؿ��� protected �Ǥ���
 */
mdarray &mdarray::ope_slash_equal(const mdarray &obj)
{
    return this->ope_slash_equal(obj, this->auto_resize());
}

/**
 * @brief  ���Ȥ�����򡤻��ꤵ�줿����ǽ��� (���٥�)
 * @note   ���Υ��дؿ��� protected �Ǥ���
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

    /* �ؿ������� */
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
    if ( obj.sz_type_rec == org_sz_type && this->sz_type_rec == new_sz_type ) { \
	func_calc = &xeq_n::fncname; \
    }
    SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(SEL_FUNC,,,,,,,,,,,,,,,,,,,,,,,,,,,,else);
#undef SEL_FUNC

    /* �ؿ��������������Ԥʤ� */
    if ( func_calc != NULL ) {
	size_t len_src, len_dest, ndim;

	/* �礭������Ȥ� */
	ndim = this->dim_length();
	if ( obj.dim_length() < ndim ) ndim = obj.dim_length();

	/* �ǽ�ϡ���������������ԥ�������� len_xxx �˥��åȤ��� */
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
 * @brief  ���Ȥ�����򡤻��ꤵ�줿����ǽ���
 *
 *  �黻�Ҥα�¦(����) �ǻ��ꤵ�줿 mdarray ���饹�Υ��֥������Ȥ�����򼫿�
 *  ����������ޤ���<br>
 *  ���ȤȤϰۤʤ뷿�����ꤵ�줿���֥������Ȥ����ꤵ�줿��硤�̾�Υ����顼
 *  �黻�ξ���Ʊ�ͤη��Ѵ��������Ԥʤ��ޤ���<br>
 *  �ּ�ư�ꥵ�����⡼�ɡפ����ꤵ��Ƥ����硤�Ƽ����������˴ؤ��Ƽ��Ȥ���
 *  obj �������礭����硤��ưŪ�˥ꥵ�������ޤ���
 *
 * @param     obj mdarray ���饹�Υ��֥�������
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
 */
mdarray &mdarray::operator/=(const mdarray &obj)
{
    debug_report("(const mdarray &obj)");
    return this->ope_slash_equal(obj);
}

/**
 * @brief  ���Ȥ�����ˡ������顼�ͤ����� (���٥�)
 *
 *  �����顼�ͤ򼫿ȤΤ��٤Ƥ����Ǥ��������ޤ���
 *  ��ưŪ�ʥ�������ĥ�ϹԤ��ޤ���<br>
 *   1. Ϳ����줿�����̤ȡ����ߤ� mdarray �����ķ����̤��顤Ŭ�ڤʡ�=�׽�����
 *      �δؿ����������(SEL_FUNC)��<br>
 *   2. mdarray �������Ǥ��оݤȤ��ơ��嵭�Ǽ���������=�׽����Ѥδؿ����Ѥ��ơ�
 *      Ϳ����줿�ѿ��򡤸��ߤ� mdarray �������ѿ��ˡ�=�ױ黻���롥
 *
 * @param    szt   �����顼�ͤη��򼨤���(������)
 * @param    v_ptr �����顼�ͤΥ��ɥ쥹
 * @return   ���Ȥλ���
 * @note     ���Υ��дؿ��� protected �Ǥ���
 */
mdarray &mdarray::ope_equal(ssize_t szt, const void *v_ptr)
{
    void (*func_d2dest)(const void *,void *,size_t,int,void *);
    size_t len = this->length();

    func_d2dest = NULL;
    /* szt�ʷ��򥢥쥤�η����Ѵ�����ؿ������� */
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
	prms.total_len_elements = 0;		/* SSE2 �ѡ����ʤΤ�0 */
	prms.round_flg = 0;			/* �ڤ�ΤƤ��Ѵ� */
	(*func_d2dest)(v_ptr, this->data_ptr(), len, 0, (void *)&prms);
    }
    return *this;
}

/**
 * @brief  ���Ȥ�����ˡ������顼�ͤ�����
 *
 *  �黻�Ҥα�¦(����) �ǻ��ꤵ�줿����(�����顼��) �������򤷤ޤ���<br>
 *  ��ưŪ�ʥ�������ĥ�ϹԤ��ޤ���ͽ�����ǿ������ꤷ���Хåե�����ݤ���ɬ��
 *  ������ޤ���
 *
 * @param     v �����顼��
 * @return    ���Ȥλ���
 */
mdarray &mdarray::operator=(dcomplex v)
{
    debug_report("(dcomplex v)");
    return this->ope_equal(DCOMPLEX_ZT,(const void *)&v);
}

/**
 * @brief  ���Ȥ�����ˡ������顼�ͤ�����
 *
 *  �黻�Ҥα�¦(����) �ǻ��ꤵ�줿����(�����顼��) �������򤷤ޤ���<br>
 *  ��ưŪ�ʥ�������ĥ�ϹԤ��ޤ���ͽ�����ǿ������ꤷ���Хåե�����ݤ���ɬ��
 *  ������ޤ���
 *
 * @param     v �����顼��
 * @return    ���Ȥλ���
 */
mdarray &mdarray::operator=(double v)
{
    debug_report("(double v)");
    return this->ope_equal(DOUBLE_ZT,(const void *)&v);
}

/**
 * @brief  ���Ȥ�����ˡ������顼�ͤ�����
 *
 *  �黻�Ҥα�¦(����) �ǻ��ꤵ�줿����(�����顼��) �������򤷤ޤ���<br>
 *  ��ưŪ�ʥ�������ĥ�ϹԤ��ޤ���ͽ�����ǿ������ꤷ���Хåե�����ݤ���ɬ��
 *  ������ޤ���
 *
 * @param     v �����顼��
 * @return    ���Ȥλ���
 */
mdarray &mdarray::operator=(long long v)
{
    debug_report("(long long v)");
    return this->ope_equal(sizeof(v),(const void *)&v);
}

/**
 * @brief  ���Ȥ�����ˡ������顼�ͤ�����
 *
 *  �黻�Ҥα�¦(����) �ǻ��ꤵ�줿����(�����顼��) �������򤷤ޤ���<br>
 *  ��ưŪ�ʥ�������ĥ�ϹԤ��ޤ���ͽ�����ǿ������ꤷ���Хåե�����ݤ���ɬ��
 *  ������ޤ���
 *
 * @param     v �����顼��
 * @return    ���Ȥλ���
 */
mdarray &mdarray::operator=(long v)
{
    debug_report("(long v)");
    return this->ope_equal(sizeof(v),(const void *)&v);
}

/**
 * @brief  ���Ȥ�����ˡ������顼�ͤ�����
 *
 *  �黻�Ҥα�¦(����) �ǻ��ꤵ�줿����(�����顼��) �������򤷤ޤ���<br>
 *  ��ưŪ�ʥ�������ĥ�ϹԤ��ޤ���ͽ�����ǿ������ꤷ���Хåե�����ݤ���ɬ��
 *  ������ޤ���
 *
 * @param     v �����顼��
 * @return    ���Ȥλ���
 */
mdarray &mdarray::operator=(int v)
{
    debug_report("(int v)");
    return this->ope_equal(sizeof(v),(const void *)&v);
}


/**
 * @brief  ���Ȥ�����򡤥����顼�ͤǲû� (���٥�)
 *
 *  ���Ȥ����Ǥ��٤Ƥ��Ф���,�����顼�ͤ�û����ޤ���<br>
 *   1. �û������Τ���δؿ������򤹤�(SEL_FUNC)��<br>
 *   2. �û������δؿ���ƽФ��������Ǥ��Ф��û����롥
 *
 * @param    szt   �����顼�ͤη��򼨤���(������)
 * @param    v_ptr �û��оݤΥ����顼�ͤΥ��ɥ쥹
 * @return   ���Ȥλ���
 * @note     ���Υ��дؿ��� protected �Ǥ���
 */
mdarray &mdarray::ope_plus_equal(ssize_t szt, const void *v_ptr)
{
    void (*func_calc)(void *,const void *,size_t,size_t,int);
    func_calc = NULL;

    /* �ؿ������� */
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
 * @brief  ���Ȥ�����򡤥����顼�ͤǲû�
 *
 *  �黻�Ҥα�¦(����) �ǻ��ꤵ�줿�����顼�ͤ򼫿Ȥ����Ǥ��٤Ƥ��Ф��Ʋû�
 *  ���ޤ���<br>
 *  ���ȤȤϷ����ۤʤ�����ξ�硤�̾�Υ����顼�黻�ξ���Ʊ�ͤη��Ѵ�������
 *  �Ԥʤ��ޤ���
 *
 * @param     v  �����顼��
 * @return    ���Ȥλ���
 */
mdarray &mdarray::operator+=(dcomplex v)
{
    debug_report("(complex v)");
    return this->ope_plus_equal(DCOMPLEX_ZT,(const void *)&v);
}

/**
 * @brief  ���Ȥ�����򡤥����顼�ͤǲû�
 *
 *  �黻�Ҥα�¦(����) �ǻ��ꤵ�줿�����顼�ͤ򼫿Ȥ����Ǥ��٤Ƥ��Ф��Ʋû�
 *  ���ޤ���<br>
 *  ���ȤȤϷ����ۤʤ�����ξ�硤�̾�Υ����顼�黻�ξ���Ʊ�ͤη��Ѵ�������
 *  �Ԥʤ��ޤ���
 *
 * @param     v  �����顼��
 * @return    ���Ȥλ���
 */
mdarray &mdarray::operator+=(double v)
{
    debug_report("(double v)");
    return this->ope_plus_equal(DOUBLE_ZT,(const void *)&v);
}

/**
 * @brief  ���Ȥ�����򡤥����顼�ͤǲû�
 *
 *  �黻�Ҥα�¦(����) �ǻ��ꤵ�줿�����顼�ͤ򼫿Ȥ����Ǥ��٤Ƥ��Ф��Ʋû�
 *  ���ޤ���<br>
 *  ���ȤȤϷ����ۤʤ�����ξ�硤�̾�Υ����顼�黻�ξ���Ʊ�ͤη��Ѵ�������
 *  �Ԥʤ��ޤ���
 *
 * @param     v  �����顼��
 * @return    ���Ȥλ���
 */
mdarray &mdarray::operator+=(long long v)
{
    debug_report("(long long v)");
    return this->ope_plus_equal(sizeof(v),(const void *)&v);
}

/**
 * @brief  ���Ȥ�����򡤥����顼�ͤǲû�
 *
 *  �黻�Ҥα�¦(����) �ǻ��ꤵ�줿�����顼�ͤ򼫿Ȥ����Ǥ��٤Ƥ��Ф��Ʋû�
 *  ���ޤ���<br>
 *  ���ȤȤϷ����ۤʤ�����ξ�硤�̾�Υ����顼�黻�ξ���Ʊ�ͤη��Ѵ�������
 *  �Ԥʤ��ޤ���
 *
 * @param     v  �����顼��
 * @return    ���Ȥλ���
 */
mdarray &mdarray::operator+=(long v)
{
    debug_report("(long v)");
    return this->ope_plus_equal(sizeof(v),(const void *)&v);
}

/**
 * @brief  ���Ȥ�����򡤥����顼�ͤǲû�
 *
 *  �黻�Ҥα�¦(����) �ǻ��ꤵ�줿�����顼�ͤ򼫿Ȥ����Ǥ��٤Ƥ��Ф��Ʋû�
 *  ���ޤ���<br>
 *  ���ȤȤϷ����ۤʤ�����ξ�硤�̾�Υ����顼�黻�ξ���Ʊ�ͤη��Ѵ�������
 *  �Ԥʤ��ޤ���
 *
 * @param     v  �����顼��
 * @return    ���Ȥλ���
 */
mdarray &mdarray::operator+=(int v)
{
    debug_report("(int v)");
    return this->ope_plus_equal(sizeof(v),(const void *)&v);
}


/**
 * @brief  ���Ȥ�����򡤥����顼�ͤǸ��� (���٥�)
 *
 *  ���Ȥ����Ǥ��٤Ƥ��Ф���,�����顼�ͤǸ�����Ԥ��ޤ���<br>
 *   1. ���������Τ���δؿ������򤹤�(SEL_FUNC)��<br>
 *   2. ���������δؿ���ƽФ��������Ǥ��Ф��������롥
 *
 * @param    szt   �����顼�ͤη��򼨤���(������)
 * @param    v_ptr �����оݤΥ����顼�ͤΥ��ɥ쥹
 * @return   ���Ȥλ���
 * @note     ���Υ��дؿ��� protected �Ǥ���
 */
mdarray &mdarray::ope_minus_equal(ssize_t szt, const void *v_ptr)
{
    void (*func_calc)(void *,const void *,size_t,size_t,int);
    func_calc = NULL;

    /* �ؿ������� */
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
 * @brief  ���Ȥ�����򡤥����顼�ͤǸ���
 *
 *  ���Ȥ����Ǥ��٤Ƥ��Ф��ơ��黻�Ҥα�¦(����) �ǻ��ꤵ�줿�����顼�ͤǸ���
 *  ���ޤ���<br>
 *  ���ȤȤϷ����ۤʤ�����ξ�硤�̾�Υ����顼�黻�ξ���Ʊ�ͤη��Ѵ�������
 *  �Ԥʤ��ޤ���
 *
 * @param     v �����顼��
 * @return    ���Ȥλ���
 */
mdarray &mdarray::operator-=(dcomplex v)
{
    debug_report("(complex v)");
    return this->ope_minus_equal(DCOMPLEX_ZT,(const void *)&v);
}

/**
 * @brief  ���Ȥ�����򡤥����顼�ͤǸ���
 *
 *  ���Ȥ����Ǥ��٤Ƥ��Ф��ơ��黻�Ҥα�¦(����) �ǻ��ꤵ�줿�����顼�ͤǸ���
 *  ���ޤ���<br>
 *  ���ȤȤϷ����ۤʤ�����ξ�硤�̾�Υ����顼�黻�ξ���Ʊ�ͤη��Ѵ�������
 *  �Ԥʤ��ޤ���
 *
 * @param     v �����顼��
 * @return    ���Ȥλ���
 */
mdarray &mdarray::operator-=(double v)
{
    debug_report("(double v)");
    return this->ope_minus_equal(DOUBLE_ZT,(const void *)&v);
}

/**
 * @brief  ���Ȥ�����򡤥����顼�ͤǸ���
 *
 *  ���Ȥ����Ǥ��٤Ƥ��Ф��ơ��黻�Ҥα�¦(����) �ǻ��ꤵ�줿�����顼�ͤǸ���
 *  ���ޤ���<br>
 *  ���ȤȤϷ����ۤʤ�����ξ�硤�̾�Υ����顼�黻�ξ���Ʊ�ͤη��Ѵ�������
 *  �Ԥʤ��ޤ���
 *
 * @param     v �����顼��
 * @return    ���Ȥλ���
 */
mdarray &mdarray::operator-=(long long v)
{
    debug_report("(long long v)");
    return this->ope_minus_equal(sizeof(v),(const void *)&v);
}

/**
 * @brief  ���Ȥ�����򡤥����顼�ͤǸ���
 *
 *  ���Ȥ����Ǥ��٤Ƥ��Ф��ơ��黻�Ҥα�¦(����) �ǻ��ꤵ�줿�����顼�ͤǸ���
 *  ���ޤ���<br>
 *  ���ȤȤϷ����ۤʤ�����ξ�硤�̾�Υ����顼�黻�ξ���Ʊ�ͤη��Ѵ�������
 *  �Ԥʤ��ޤ���
 *
 * @param     v �����顼��
 * @return    ���Ȥλ���
 *
 */
mdarray &mdarray::operator-=(long v)
{
    debug_report("(long v)");
    return this->ope_minus_equal(sizeof(v),(const void *)&v);
}

/**
 * @brief  ���Ȥ�����򡤥����顼�ͤǸ���
 *
 *  ���Ȥ����Ǥ��٤Ƥ��Ф��ơ��黻�Ҥα�¦(����) �ǻ��ꤵ�줿�����顼�ͤǸ���
 *  ���ޤ���<br>
 *  ���ȤȤϷ����ۤʤ�����ξ�硤�̾�Υ����顼�黻�ξ���Ʊ�ͤη��Ѵ�������
 *  �Ԥʤ��ޤ���
 *
 * @param     v �����顼��
 * @return    ���Ȥλ���
 */
mdarray &mdarray::operator-=(int v)
{
    debug_report("(int v)");
    return this->ope_minus_equal(sizeof(v),(const void *)&v);
}

/**
 * @brief  ���Ȥ�����ˡ������顼�ͤ�軻 (���٥�)
 *
 *  ���Ȥ����Ǥ��٤Ƥ��Ф���,�����顼�ͤǾ軻���ޤ���<br>
 *   1. �軻�����Τ���δؿ������򤹤�(SEL_FUNC)��<br>
 *   2. �軻�����δؿ���ƽФ��������Ǥ��Ф��軻���롥
 *
 * @param    szt   �����顼�ͤη��򼨤���(������)
 * @param    v_ptr �軻�оݤΥ����顼�ͤΥ��ɥ쥹
 * @return   ���Ȥλ���
 * @note     ���Υ��дؿ��� protected �Ǥ���
 */
mdarray &mdarray::ope_star_equal(ssize_t szt, const void *v_ptr)
{
    void (*func_calc)(void *,const void *,size_t,size_t,int);
    func_calc = NULL;

    /* �ؿ������� */
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
 * @brief  ���Ȥ�����ˡ������顼�ͤ�軻
 *
 *  �黻�Ҥα�¦(����) �ǻ��ꤵ�줿�����顼�ͤ򼫿Ȥ����Ǥ��٤Ƥ��Ф��ƾ軻
 *  ���ޤ���<br>
 *  ���ȤȤϷ����ۤʤ�����ξ�硤�̾�Υ����顼�黻�ξ���Ʊ�ͤη��Ѵ�������
 *  �Ԥʤ��ޤ���
 *
 * @param     v �����顼��
 * @return    ���Ȥλ���
 */
mdarray &mdarray::operator*=(dcomplex v)
{
    debug_report("(complex v)");
    return this->ope_star_equal(DCOMPLEX_ZT,(const void *)&v);
}

/**
 * @brief  ���Ȥ�����ˡ������顼�ͤ�軻
 *
 *  �黻�Ҥα�¦(����) �ǻ��ꤵ�줿�����顼�ͤ򼫿Ȥ����Ǥ��٤Ƥ��Ф��ƾ軻
 *  ���ޤ���<br>
 *  ���ȤȤϷ����ۤʤ�����ξ�硤�̾�Υ����顼�黻�ξ���Ʊ�ͤη��Ѵ�������
 *  �Ԥʤ��ޤ���
 *
 * @param     v �����顼��
 * @return    ���Ȥλ���
 */
mdarray &mdarray::operator*=(double v)
{
    debug_report("(double v)");
    return this->ope_star_equal(DOUBLE_ZT,(const void *)&v);
}

/**
 * @brief  ���Ȥ�����ˡ������顼�ͤ�軻
 *
 *  �黻�Ҥα�¦(����) �ǻ��ꤵ�줿�����顼�ͤ򼫿Ȥ����Ǥ��٤Ƥ��Ф��ƾ軻
 *  ���ޤ���<br>
 *  ���ȤȤϷ����ۤʤ�����ξ�硤�̾�Υ����顼�黻�ξ���Ʊ�ͤη��Ѵ�������
 *  �Ԥʤ��ޤ���
 *
 * @param     v �����顼��
 * @return    ���Ȥλ���
 */
mdarray &mdarray::operator*=(long long v)
{
    debug_report("(long long v)");
    return this->ope_star_equal(sizeof(v),(const void *)&v);
}

/**
 * @brief  ���Ȥ�����ˡ������顼�ͤ�軻
 *
 *  �黻�Ҥα�¦(����) �ǻ��ꤵ�줿�����顼�ͤ򼫿Ȥ����Ǥ��٤Ƥ��Ф��ƾ軻
 *  ���ޤ���<br>
 *  ���ȤȤϷ����ۤʤ�����ξ�硤�̾�Υ����顼�黻�ξ���Ʊ�ͤη��Ѵ�������
 *  �Ԥʤ��ޤ���
 *
 * @param     v �����顼��
 * @return    ���Ȥλ���
 */
mdarray &mdarray::operator*=(long v)
{
    debug_report("(long v)");
    return this->ope_star_equal(sizeof(v),(const void *)&v);
}

/**
 * @brief  ���Ȥ�����ˡ������顼�ͤ�軻
 *
 *  �黻�Ҥα�¦(����) �ǻ��ꤵ�줿�����顼�ͤ򼫿Ȥ����Ǥ��٤Ƥ��Ф��ƾ軻
 *  ���ޤ���<br>
 *  ���ȤȤϷ����ۤʤ�����ξ�硤�̾�Υ����顼�黻�ξ���Ʊ�ͤη��Ѵ�������
 *  �Ԥʤ��ޤ���
 *
 * @param     v �����顼��
 * @return    ���Ȥλ���
 */
mdarray &mdarray::operator*=(int v)
{
    debug_report("(int v)");
    return this->ope_star_equal(sizeof(v),(const void *)&v);
}


/**
 * @brief  ���Ȥ�����򡤥����顼�ͤǽ��� (���٥�)
 *
 *  ���Ȥ����Ǥ��٤Ƥ��Ф��ơ������顼�ͤǽ������ޤ���<br>
 *   1. ���������Τ���δؿ������򤹤�(SEL_FUNC)��<br>
 *   2. ���������δؿ���ƽФ��������Ǥ��Ф��������롥
 *
 * @param    szt   �����顼�ͤη��򼨤���(������)
 * @param    v_ptr �����оݤΥ����顼�ͤΥ��ɥ쥹
 * @return   ���Ȥλ���
 * @note     ���Υ��дؿ��� protected �Ǥ���
 */
mdarray &mdarray::ope_slash_equal(ssize_t szt, const void *v_ptr)
{
    void (*func_calc)(void *,const void *,size_t,size_t,int);
    func_calc = NULL;

    /* �ؿ������� */
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
 * @brief  ���Ȥ�����򡤥����顼�ͤǽ���
 *
 *  ���Ȥ����Ǥ��٤Ƥ��Ф��ơ��黻�Ҥα�¦(����) �ǻ��ꤵ�줿�����顼�ͤǽ���
 *  ���ޤ���<br>
 *  ���ȤȤϷ����ۤʤ�����ξ�硤�̾�Υ����顼�黻�ξ���Ʊ�ͤη��Ѵ�������
 *  �Ԥʤ��ޤ���
 *
 * @param     v �����顼��
 * @return    ���Ȥλ���
 */
mdarray &mdarray::operator/=(dcomplex v)
{
    debug_report("(complex v)");
    return this->ope_slash_equal(DCOMPLEX_ZT,(const void *)&v);
}

/**
 * @brief  ���Ȥ�����򡤥����顼�ͤǽ���
 *
 *  ���Ȥ����Ǥ��٤Ƥ��Ф��ơ��黻�Ҥα�¦(����) �ǻ��ꤵ�줿�����顼�ͤǽ���
 *  ���ޤ���<br>
 *  ���ȤȤϷ����ۤʤ�����ξ�硤�̾�Υ����顼�黻�ξ���Ʊ�ͤη��Ѵ�������
 *  �Ԥʤ��ޤ���
 *
 * @param     v �����顼��
 * @return    ���Ȥλ���
 */
mdarray &mdarray::operator/=(double v)
{
    debug_report("(double v)");
    return this->ope_slash_equal(DOUBLE_ZT,(const void *)&v);
}

/**
 * @brief  ���Ȥ�����򡤥����顼�ͤǽ���
 *
 *  ���Ȥ����Ǥ��٤Ƥ��Ф��ơ��黻�Ҥα�¦(����) �ǻ��ꤵ�줿�����顼�ͤǽ���
 *  ���ޤ���<br>
 *  ���ȤȤϷ����ۤʤ�����ξ�硤�̾�Υ����顼�黻�ξ���Ʊ�ͤη��Ѵ�������
 *  �Ԥʤ��ޤ���
 *
 * @param     v �����顼��
 * @return    ���Ȥλ���
 */
mdarray &mdarray::operator/=(long long v)
{
    debug_report("(long long v)");
    return this->ope_slash_equal(sizeof(v),(const void *)&v);
}

/**
 * @brief  ���Ȥ�����򡤥����顼�ͤǽ���
 *
 *  ���Ȥ����Ǥ��٤Ƥ��Ф��ơ��黻�Ҥα�¦(����) �ǻ��ꤵ�줿�����顼�ͤǽ���
 *  ���ޤ���<br>
 *  ���ȤȤϷ����ۤʤ�����ξ�硤�̾�Υ����顼�黻�ξ���Ʊ�ͤη��Ѵ�������
 *  �Ԥʤ��ޤ���
 *
 * @param     v �����顼��
 * @return    ���Ȥλ���
 */
mdarray &mdarray::operator/=(long v)
{
    debug_report("(long v)");
    return this->ope_slash_equal(sizeof(v),(const void *)&v);
}

/**
 * @brief  ���Ȥ�����򡤥����顼�ͤǽ���
 *
 *  ���Ȥ����Ǥ��٤Ƥ��Ф��ơ��黻�Ҥα�¦(����) �ǻ��ꤵ�줿�����顼�ͤǽ���
 *  ���ޤ���<br>
 *  ���ȤȤϷ����ۤʤ�����ξ�硤�̾�Υ����顼�黻�ξ���Ʊ�ͤη��Ѵ�������
 *  �Ԥʤ��ޤ���
 *
 * @param     v �����顼��
 * @return    ���Ȥλ���
 */
mdarray &mdarray::operator/=(int v)
{
    debug_report("(int v)");
    return this->ope_slash_equal(sizeof(v),(const void *)&v);
}

/**
 * @brief  ���Ȥ�����Ȼ��ꤵ�줿���֥������Ȥ�����Ȥ���� (���٥�)
 *
 *  ���Ȥ�obj���������������ɤ������֤��ޤ���<br>
 *  ����η����ۤʤäƤ�����Ĺ���ͤ���������п����֤��ޤ�. 
 *
 * @param   obj mdarray���饹�Υ��֥�������
 * @return  true ���󥵥���,���Ǥ��ͤ����פ������<br>
 *          false ���󥵥���,���Ǥ��ͤ��԰��פǤ�����
 * @note    ���Υ��дؿ��� protected �Ǥ���
 */
bool mdarray::ope_equal_equal(const mdarray &obj) const
{
    return this->mdarray::compare(obj);
}

/**
 * @brief  ���Ȥ�����Ȼ��ꤵ�줿���֥������Ȥ�����Ȥ����
 *
 *  �黻�Ҥα�¦(����)�ǻ��ꤵ�줿 mdarray(�Ѿ�)���饹�Υ��֥������Ȥ򼫿ȤȤ�
 *  ��Ӥ��ޤ���<br>
 *  obj �����󥵥��������Ǥ��ͤ���������С�true ���֤��ޤ���<br>
 *  obj �����󥵥��������Ǥ��ͤ��������ʤ���С�false ���֤��ޤ���
 *
 * @param     obj mdarray ���饹�Υ��֥�������
 * @return    true : ���󥵥��������Ǥ��ͤ����פ������<br>
 *            false : ���󥵥��������Ǥ��ͤ��԰��פǤ�����
 * @note      ���Υ��дؿ��ϡ������� compare() ���дؿ���ȤäƤ��ޤ���
 */
bool mdarray::operator==(const mdarray &obj) const
{
    return this->ope_equal_equal(obj);
}

/**
 * @brief  ���Ȥ�����Ȼ��ꤵ�줿���֥������Ȥ�����Ȥ���� (�����)
 *
 *  �黻�Ҥα�¦(����)�ǻ��ꤵ�줿 mdarray(�Ѿ�)���饹�Υ��֥������Ȥ򼫿ȤȤ�
 *  ���(�����)���ޤ���<br>
 *  obj �����󥵥��������Ǥ��ͤ��������ʤ���С�true ���֤��ޤ���<br>
 *  obj �����󥵥��������Ǥ��ͤ���������С�false ���֤��ޤ���
 *
 * @param     obj mdarray ���饹�Υ��֥�������
 * @return    true : ���󥵥��������Ǥ��ͤ��԰��פǤ�����<br>
 *            false : ���󥵥��������Ǥ��ͤ����פ������
 * @note      ���Υ��дؿ��ϡ������� compare() ���дؿ���ȤäƤ��ޤ���
 */
bool mdarray::operator!=(const mdarray &obj) const
{
    return (this->ope_equal_equal(obj) == true) ? false : true;
}


/* ********************************************** */
/* ���� : ��³��Ū���ڥ졼����ɬ�פʥ����� : ���� */
/* ********************************************** */

/* ��³��Ū���ڥ졼���Ȥ�����֤��� */

/**
 * @brief  ���ڥ졼���κ�����2�Ĥη����顤�ɤη����֤�������� (������)
 *
 * @param   szt1 �����̤��Σ�
 * @param   szt2 �����̤��Σ�
 * @return  ���򤵤줿������
 * @note    private �ʴؿ��Ǥ���
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
    /* �¿����ΤߤΥѥ����� */
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
    /* ʣ�ǿ�����ޤ�ѥ����� */
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
    /* �����������ơ��֥� */
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
 * @brief  mdarray::ope_plus(const mdarray &obj) ���ǻȤ��Ƶ��ƤӽФ��ؿ�
 *
 * @note   ���δؿ��� namespace *_nno_n, namespace *_non_n �δؿ���ƤӽФ�<br>
 *         private �ʴؿ��Ǥ���
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
    /* src?_p �� NULL �ʤ顤NULL �򥭡��פ���褦�� blen_blk_src? �� set */
    if ( src0_p == NULL ) blen_blk_src0 = 0;
    else blen_blk_src0 = blen_block_src0;
    if ( src1_p == NULL ) blen_blk_src1 = 0;
    else blen_blk_src1 = blen_block_src1;
    if ( 0 < dim_ix ) {
	size_t i;
	/* ���: �����Ǥ� len_src0 <= len_ret && len_src1 <= len_ret ���� */
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
    else {	/* ����=0 �Ǥν��� */
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
/* _step0, _step1 �� 0 �ʤ������ϥ����顼��1 �ʤ����� */
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
/* _step0, _step1 �� 0 �ʤ������ϥ����顼��1 �ʤ����� */
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
 * @brief  ���Ȥ�����ˡ������顼�ͤ�û�������̤������֥������Ȥ��֤� (���٥�)
 *
 *  �����顼�ͤ򼫿Ȥ����Ǥ��줾��˲û�������̤��Ǽ�������֥������Ȥ����
 *  ����������֤��ޤ���<br>
 *   1. Ϳ����줿�����̤ȡ����ߤ� mdarray �����ķ����̤��顤�黻��̤Ȥʤ�
 *      �����̤��������(get_sz_type_from_two)��<br>
 *   2. �嵭�η����̤���Ŀ����� mdarray ������������ߤ� mdarray �ξ����
 *      ���ԡ�����(init��init_properties)��<br>
 *   3. ������ mdarray �������Ǥ��оݤȤ��� Ϳ����줿�ѿ����+�ױ黻���롥<br>
 *   4. �Ǹ�� shallow copy ����Ĥ���ե饰�򥻥åȤ���return��
 *
 * @param   szt �����顼�ͤη�����
 * @param   v_ptr �����顼�ͤΥ��ɥ쥹
 * @return  �黻��̤��Ǽ�������֥�������
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 * @note    ���Υ��дؿ��� protected �Ǥ���
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
    /* ��̷����ɤ��餫�˰��פ����硤ľ�ܱ黻��Ԥʤ� */
    if ( ret_szt == this->size_type() ) {
	/* ���Ѵ��Τ���δؿ������� */
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
	if ( szt == org_sz_type && ret_szt == new_sz_type ) { \
	    func_ope = &x_nno_n::fncname; \
	}
	SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(SEL_FUNC,,,,,,,,,,,,,,,,,,,,,,,,,,,,else);
#undef SEL_FUNC
    } else if ( ret_szt == szt ) {
	/* ���Ѵ��Τ���δؿ������� */
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
    /* fallback (�٤����Ȥ�����Ϥʤ��Ϥ�) */
    else {
	ret.ope_plus_equal(*this, true);
	/* ope */
	ret.ope_plus_equal(szt,v_ptr);
    }

    ret.set_scopy_flag();
    return ret;
}

/**
 * @brief  ���Ȥ�����ˡ������顼�ͤ�û�������̤������֥������Ȥ��֤�
 *
 *  �黻�Ҥα�¦(����) �ǻ��ꤵ�줿�����顼�ͤ򡤼��Ȥ����Ǥ��٤Ƥ��Ф��Ʋû�
 *  ������̤��Ǽ�������֥������Ȥ���������֤��ޤ���<br>
 *  ���ȤȤϷ����ۤʤ�����ξ�硤�̾�Υ����顼�黻�ξ���Ʊ�ͤη��Ѵ�������
 *  �Ԥʤ��ޤ���<br>
 *  �֤���륪�֥������Ȥ�ư��⡼�ɤ� rounding ����°���ϡ����Ȥξ���Ʊ����
 *  ����
 *
 * @param     v  �����顼��
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
 */
mdarray mdarray::operator+(dcomplex v) const
{
    return this->ope_plus(DCOMPLEX_ZT,(const void*)&v);
}

/**
 * @brief  ���Ȥ�����ˡ������顼�ͤ�û�������̤������֥������Ȥ��֤�
 *
 *  �黻�Ҥα�¦(����) �ǻ��ꤵ�줿�����顼�ͤ򡤼��Ȥ����Ǥ��٤Ƥ��Ф��Ʋû�
 *  ������̤��Ǽ�������֥������Ȥ���������֤��ޤ���<br>
 *  ���ȤȤϷ����ۤʤ�����ξ�硤�̾�Υ����顼�黻�ξ���Ʊ�ͤη��Ѵ�������
 *  �Ԥʤ��ޤ���<br>
 *  �֤���륪�֥������Ȥ�ư��⡼�ɤ� rounding ����°���ϡ����Ȥξ���Ʊ����
 *  ����
 *
 * @param     v  �����顼��
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
 */
mdarray mdarray::operator+(fcomplex v) const
{
    return this->ope_plus(FCOMPLEX_ZT,(const void*)&v);
}

/**
 * @brief  ���Ȥ�����ˡ������顼�ͤ�û�������̤������֥������Ȥ��֤�
 *
 *  �黻�Ҥα�¦(����) �ǻ��ꤵ�줿�����顼�ͤ򡤼��Ȥ����Ǥ��٤Ƥ��Ф��Ʋû�
 *  ������̤��Ǽ�������֥������Ȥ���������֤��ޤ���<br>
 *  ���ȤȤϷ����ۤʤ�����ξ�硤�̾�Υ����顼�黻�ξ���Ʊ�ͤη��Ѵ�������
 *  �Ԥʤ��ޤ���<br>
 *  �֤���륪�֥������Ȥ�ư��⡼�ɤ� rounding ����°���ϡ����Ȥξ���Ʊ����
 *  ����
 *
 * @param     v  �����顼��
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
 */
mdarray mdarray::operator+(double v) const
{
    return this->ope_plus(DOUBLE_ZT,(const void *)&v);
}

/**
 * @brief  ���Ȥ�����ˡ������顼�ͤ�û�������̤������֥������Ȥ��֤�
 *
 *  �黻�Ҥα�¦(����) �ǻ��ꤵ�줿�����顼�ͤ򡤼��Ȥ����Ǥ��٤Ƥ��Ф��Ʋû�
 *  ������̤��Ǽ�������֥������Ȥ���������֤��ޤ���<br>
 *  ���ȤȤϷ����ۤʤ�����ξ�硤�̾�Υ����顼�黻�ξ���Ʊ�ͤη��Ѵ�������
 *  �Ԥʤ��ޤ���<br>
 *  �֤���륪�֥������Ȥ�ư��⡼�ɤ� rounding ����°���ϡ����Ȥξ���Ʊ����
 *  ����
 *
 * @param     v  �����顼��
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
 */
mdarray mdarray::operator+(float v) const
{
    return this->ope_plus(FLOAT_ZT,(const void *)&v);
}

/**
 * @brief  ���Ȥ�����ˡ������顼�ͤ�û�������̤������֥������Ȥ��֤�
 *
 *  �黻�Ҥα�¦(����) �ǻ��ꤵ�줿�����顼�ͤ򡤼��Ȥ����Ǥ��٤Ƥ��Ф��Ʋû�
 *  ������̤��Ǽ�������֥������Ȥ���������֤��ޤ���<br>
 *  ���ȤȤϷ����ۤʤ�����ξ�硤�̾�Υ����顼�黻�ξ���Ʊ�ͤη��Ѵ�������
 *  �Ԥʤ��ޤ���<br>
 *  �֤���륪�֥������Ȥ�ư��⡼�ɤ� rounding ����°���ϡ����Ȥξ���Ʊ����
 *  ����
 *
 * @param     v  �����顼��
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
 */
mdarray mdarray::operator+(long long v) const
{
    return this->ope_plus(sizeof(v),(const void *)&v);
}

/**
 * @brief  ���Ȥ�����ˡ������顼�ͤ�û�������̤������֥������Ȥ��֤�
 *
 *  �黻�Ҥα�¦(����) �ǻ��ꤵ�줿�����顼�ͤ򡤼��Ȥ����Ǥ��٤Ƥ��Ф��Ʋû�
 *  ������̤��Ǽ�������֥������Ȥ���������֤��ޤ���<br>
 *  ���ȤȤϷ����ۤʤ�����ξ�硤�̾�Υ����顼�黻�ξ���Ʊ�ͤη��Ѵ�������
 *  �Ԥʤ��ޤ���<br>
 *  �֤���륪�֥������Ȥ�ư��⡼�ɤ� rounding ����°���ϡ����Ȥξ���Ʊ����
 *  ����
 *
 * @param     v  �����顼��
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
 */
mdarray mdarray::operator+(long v) const
{
    return this->ope_plus(sizeof(v),(const void *)&v);
}

/**
 * @brief  ���Ȥ�����ˡ������顼�ͤ�û�������̤������֥������Ȥ��֤�
 *
 *  �黻�Ҥα�¦(����) �ǻ��ꤵ�줿�����顼�ͤ򡤼��Ȥ����Ǥ��٤Ƥ��Ф��Ʋû�
 *  ������̤��Ǽ�������֥������Ȥ���������֤��ޤ���<br>
 *  ���ȤȤϷ����ۤʤ�����ξ�硤�̾�Υ����顼�黻�ξ���Ʊ�ͤη��Ѵ�������
 *  �Ԥʤ��ޤ���<br>
 *  �֤���륪�֥������Ȥ�ư��⡼�ɤ� rounding ����°���ϡ����Ȥξ���Ʊ����
 *  ����
 *
 * @param     v  �����顼��
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
 */
mdarray mdarray::operator+(int v) const
{
    return this->ope_plus(sizeof(v),(const void *)&v);
}

/**
 * @brief  ���Ȥ�����ˡ����ꤵ�줿�����û�������̤������֥������Ȥ��֤� (���٥�)
 *
 *  mdarray���饹�Υ��֥������Ȥ�����򡤼��Ȥ˲û�������̤��Ǽ����
 *  ���֥������Ȥ���������֤��ޤ�. 
 *
 * @param   obj mdarray���饹�Υ��֥�������
 * @return  �黻��̤��Ǽ�������֥�������
 * @throw   �����Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
 * @note    ���Υ��дؿ��� protected �Ǥ���
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
    /* ��̷����ɤ��餫�˰��פ����硤ľ�ܱ黻��Ԥʤ� */
    if ( ret_szt == this->size_type() ) {
	/* ���Ѵ��Τ���δؿ������� */
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
	if ( obj.size_type() == org_sz_type && ret_szt == new_sz_type ) { \
	    func_ope = &x_nno_n::fncname; \
	}
	SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(SEL_FUNC,,,,,,,,,,,,,,,,,,,,,,,,,,,,else);
#undef SEL_FUNC
    } else if ( ret_szt == obj.size_type() ) {
	/* ���Ѵ��Τ���δؿ������� */
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
	    /* �Ƶ��ƤӽФ��� arr �� arr �η׻���Ԥʤ� */
	    ope_arr_x_arr_r(&ret, *this, obj, Ope_plus, func_ope, 
		     ret_ndim - 1,
		     ret.byte_length(), this->byte_length(), obj.byte_length(),
		     ret.data_ptr(), this->data_ptr_cs(),obj.data_ptr_cs());
	}
    }
    /* fallback (�٤����Ȥ�����Ϥʤ��Ϥ�) */
    else {
	ret.ope_plus_equal(*this, true);
	/* ope */
	ret.ope_plus_equal(obj, true);
    }

    ret.set_scopy_flag();
    return ret;
}

/**
 * @brief  ���Ȥ�����ˡ����ꤵ�줿�����û�������̤������֥������Ȥ��֤�
 *
 *  �黻�Ҥα�¦(����) �ǻ��ꤵ�줿 mdarray ���饹�Υ��֥������Ȥ�����ȡ�����
 *  �Ȥ�û�������̤��Ǽ�������֥������Ȥ���������֤��ޤ���<br>
 *  �����ϡ��ƥ��饹�Ǥ���mdarray ���饹�Ǥ��Τǡ����ȤȤϰۤʤ뷿�����ꤵ�줿
 *  ���֥������Ȥ����Ǥ��ޤ������ξ�硤�̾�Υ����顼�黻�ξ���Ʊ�ͤη�
 *  �Ѵ��������Ԥʤ��ޤ���<br>
 *  �֤���륪�֥������Ȥ�ư��⡼�ɤ� rounding ����°���ϡ����Ȥξ���Ʊ����
 *  ����
 *
 * @param     mdarray ���饹�Υ��֥�������
 * @return    �黻��̤��Ǽ�������֥�������
 * @throw     �����Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
 */
mdarray mdarray::operator+(const mdarray &obj) const
{
    return this->ope_plus(obj);
}


/**
 * @brief  ���Ȥ�����򡤥����顼�ͤǸ���������̤������֥������Ȥ��֤� (���٥�)
 *
 *  �����顼�ͤ򼫿Ȥ����Ǥ��줾�줫�鸺��������̤��Ǽ�������֥������Ȥ�
 *  ��������������֤��ޤ���<br>
 *   1. Ϳ����줿�����̤ȡ����ߤ� mdarray �����ķ����̤��顤�黻��̤Ȥʤ�
 *      �����̤��������(get_sz_type_from_two)��<br>
 *   2. �嵭�η����̤���Ŀ����� mdarray ������������ߤ� mdarray �ξ����
 *      ���ԡ�����(init��init_properties)��<br>
 *   3. ������ mdarray �������Ǥ��оݤȤ��� Ϳ����줿�ѿ����-�ױ黻���롥<br>
 *   4. �Ǹ�� shallow copy ����Ĥ���ե饰�򥻥åȤ���return��
 *
 * @param   szt �����顼�ͤη�����
 * @param   v_ptr �����顼�ͤΥ��ɥ쥹
 * @return  �黻��̤��Ǽ�������֥�������
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 * @note    ���Υ��дؿ��� protected �Ǥ���
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
    /* ��̷����ɤ��餫�˰��פ����硤ľ�ܱ黻��Ԥʤ� */
    if ( ret_szt == this->size_type() ) {
	/* ���Ѵ��Τ���δؿ������� */
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
	if ( szt == org_sz_type && ret_szt == new_sz_type ) { \
	    func_ope = &x_nno_n::fncname; \
	}
	SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(SEL_FUNC,,,,,,,,,,,,,,,,,,,,,,,,,,,,else);
#undef SEL_FUNC
    } else if ( ret_szt == szt ) {
	/* ���Ѵ��Τ���δؿ������� */
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
    /* fallback (�٤����Ȥ�����Ϥʤ��Ϥ�) */
    else {
	ret.ope_plus_equal(*this, true);
	/* ope */
	ret.ope_minus_equal(szt,v_ptr);
    }

    ret.set_scopy_flag();
    return ret;
}

/**
 * @brief  ���Ȥ�����򡤥����顼�ͤǸ���������̤������֥������Ȥ��֤�
 *
 *  �黻�Ҥα�¦(����) �ǻ��ꤵ�줿�����顼�ͤ򡤼��Ȥ����Ǥ��줾�줫�鸺������
 *  ��̤��Ǽ�������֥������Ȥ���������֤��ޤ���<br>
 *  ���ȤȤϷ����ۤʤ�����ξ�硤�̾�Υ����顼�黻�ξ���Ʊ�ͤη��Ѵ�������
 *  �Ԥʤ��ޤ���<br>
 *  �֤���륪�֥������Ȥ�ư��⡼�ɤ� rounding ����°���ϡ����Ȥξ���Ʊ����
 *  ����
 *
 * @param     v  �����顼��
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
 */
mdarray mdarray::operator-(dcomplex v) const
{
    return this->ope_minus(DCOMPLEX_ZT,(const void *)&v);
}

/**
 * @brief  ���Ȥ�����򡤥����顼�ͤǸ���������̤������֥������Ȥ��֤�
 *
 *  �黻�Ҥα�¦(����) �ǻ��ꤵ�줿�����顼�ͤ򡤼��Ȥ����Ǥ��줾�줫�鸺������
 *  ��̤��Ǽ�������֥������Ȥ���������֤��ޤ���<br>
 *  ���ȤȤϷ����ۤʤ�����ξ�硤�̾�Υ����顼�黻�ξ���Ʊ�ͤη��Ѵ�������
 *  �Ԥʤ��ޤ���<br>
 *  �֤���륪�֥������Ȥ�ư��⡼�ɤ� rounding ����°���ϡ����Ȥξ���Ʊ����
 *  ����
 *
 * @param     v  �����顼��
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
 */
mdarray mdarray::operator-(fcomplex v) const
{
    return this->ope_minus(FCOMPLEX_ZT,(const void *)&v);
}

/**
 * @brief  ���Ȥ�����򡤥����顼�ͤǸ���������̤������֥������Ȥ��֤�
 *
 *  �黻�Ҥα�¦(����) �ǻ��ꤵ�줿�����顼�ͤ򡤼��Ȥ����Ǥ��줾�줫�鸺������
 *  ��̤��Ǽ�������֥������Ȥ���������֤��ޤ���<br>
 *  ���ȤȤϷ����ۤʤ�����ξ�硤�̾�Υ����顼�黻�ξ���Ʊ�ͤη��Ѵ�������
 *  �Ԥʤ��ޤ���<br>
 *  �֤���륪�֥������Ȥ�ư��⡼�ɤ� rounding ����°���ϡ����Ȥξ���Ʊ����
 *  ����
 *
 * @param     v  �����顼��
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
 */
mdarray mdarray::operator-(double v) const
{
    return this->ope_minus(DOUBLE_ZT,(const void *)&v);
}

/**
 * @brief  ���Ȥ�����򡤥����顼�ͤǸ���������̤������֥������Ȥ��֤�
 *
 *  �黻�Ҥα�¦(����) �ǻ��ꤵ�줿�����顼�ͤ򡤼��Ȥ����Ǥ��줾�줫�鸺������
 *  ��̤��Ǽ�������֥������Ȥ���������֤��ޤ���<br>
 *  ���ȤȤϷ����ۤʤ�����ξ�硤�̾�Υ����顼�黻�ξ���Ʊ�ͤη��Ѵ�������
 *  �Ԥʤ��ޤ���<br>
 *  �֤���륪�֥������Ȥ�ư��⡼�ɤ� rounding ����°���ϡ����Ȥξ���Ʊ����
 *  ����
 *
 * @param     v  �����顼��
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
 */
mdarray mdarray::operator-(float v) const
{
    return this->ope_minus(FLOAT_ZT,(const void *)&v);
}

/**
 * @brief  ���Ȥ�����򡤥����顼�ͤǸ���������̤������֥������Ȥ��֤�
 *
 *  �黻�Ҥα�¦(����) �ǻ��ꤵ�줿�����顼�ͤ򡤼��Ȥ����Ǥ��줾�줫�鸺������
 *  ��̤��Ǽ�������֥������Ȥ���������֤��ޤ���<br>
 *  ���ȤȤϷ����ۤʤ�����ξ�硤�̾�Υ����顼�黻�ξ���Ʊ�ͤη��Ѵ�������
 *  �Ԥʤ��ޤ���<br>
 *  �֤���륪�֥������Ȥ�ư��⡼�ɤ� rounding ����°���ϡ����Ȥξ���Ʊ����
 *  ����
 *
 * @param     v  �����顼��
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
 */
mdarray mdarray::operator-(long long v) const
{
    return this->ope_minus(sizeof(v),(const void *)&v);
}

/**
 * @brief  ���Ȥ�����򡤥����顼�ͤǸ���������̤������֥������Ȥ��֤�
 *
 *  �黻�Ҥα�¦(����) �ǻ��ꤵ�줿�����顼�ͤ򡤼��Ȥ����Ǥ��줾�줫�鸺������
 *  ��̤��Ǽ�������֥������Ȥ���������֤��ޤ���<br>
 *  ���ȤȤϷ����ۤʤ�����ξ�硤�̾�Υ����顼�黻�ξ���Ʊ�ͤη��Ѵ�������
 *  �Ԥʤ��ޤ���<br>
 *  �֤���륪�֥������Ȥ�ư��⡼�ɤ� rounding ����°���ϡ����Ȥξ���Ʊ����
 *  ����
 *
 * @param     v  �����顼��
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
 */
mdarray mdarray::operator-(long v) const
{
    return this->ope_minus(sizeof(v),(const void *)&v);
}

/**
 * @brief  ���Ȥ�����򡤥����顼�ͤǸ���������̤������֥������Ȥ��֤�
 *
 *  �黻�Ҥα�¦(����) �ǻ��ꤵ�줿�����顼�ͤ򡤼��Ȥ����Ǥ��줾�줫�鸺������
 *  ��̤��Ǽ�������֥������Ȥ���������֤��ޤ���<br>
 *  ���ȤȤϷ����ۤʤ�����ξ�硤�̾�Υ����顼�黻�ξ���Ʊ�ͤη��Ѵ�������
 *  �Ԥʤ��ޤ���<br>
 *  �֤���륪�֥������Ȥ�ư��⡼�ɤ� rounding ����°���ϡ����Ȥξ���Ʊ����
 *  ����
 *
 * @param     v  �����顼��
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
 */
mdarray mdarray::operator-(int v) const
{
    return this->ope_minus(sizeof(v),(const void *)&v);
}

/**
 * @brief  ���Ȥ�����򡤻��ꤵ�줿����Ǹ���������̤������֥������Ȥ��֤� (���٥�)
 *
 *  mdarray���饹�Υ��֥������Ȥ�����򼫿Ȥ��鸺��������̤��Ǽ����
 *  ���֥������Ȥ���������֤��ޤ���
 * 
 * @param   obj mdarray���饹�Υ��֥�������
 * @return  �黻��̤��Ǽ�������֥�������
 * @throw   �����Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
 * @note    ���Υ��дؿ��� protected �Ǥ���
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
    /* ��̷����ɤ��餫�˰��פ����硤ľ�ܱ黻��Ԥʤ� */
    if ( ret_szt == this->size_type() ) {
	/* ���Ѵ��Τ���δؿ������� */
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
	if ( obj.size_type() == org_sz_type && ret_szt == new_sz_type ) { \
	    func_ope = &x_nno_n::fncname; \
	}
	SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(SEL_FUNC,,,,,,,,,,,,,,,,,,,,,,,,,,,,else);
#undef SEL_FUNC
    } else if ( ret_szt == obj.size_type() ) {
	/* ���Ѵ��Τ���δؿ������� */
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
	    /* �Ƶ��ƤӽФ��� arr �� arr �η׻���Ԥʤ� */
	    ope_arr_x_arr_r(&ret, *this, obj, Ope_minus, func_ope, 
		     ret_ndim - 1,
		     ret.byte_length(), this->byte_length(), obj.byte_length(),
		     ret.data_ptr(), this->data_ptr_cs(),obj.data_ptr_cs());
	}
    }
    /* fallback (�٤����Ȥ�����Ϥʤ��Ϥ�) */
    else {
	ret.ope_plus_equal(*this, true);
	/* ope */
	ret.ope_minus_equal(obj, true);
    }

    ret.set_scopy_flag();
    return ret;
}

/**
 * @brief  ���Ȥ�����򡤻��ꤵ�줿����Ǹ���������̤������֥������Ȥ��֤�
 *
 *  �黻�Ҥα�¦(����) �ǻ��ꤵ�줿mdarray ���饹�Υ��֥������Ȥ�����򼫿Ȥ���
 *  ����������̤��Ǽ�������֥������Ȥ���������֤��ޤ���<br>
 *  �����ϡ��ƥ��饹�Ǥ���mdarray ���饹�Ǥ��Τǡ����ȤȤϰۤʤ뷿�����ꤵ�줿
 *  ���֥������Ȥ����Ǥ��ޤ������ξ�硤�̾�Υ����顼�黻�ξ���Ʊ�ͤη�
 *  �Ѵ��������Ԥʤ��ޤ���<br>
 *  �֤���륪�֥������Ȥ�ư��⡼�ɤ� rounding ����°���ϡ����Ȥξ���Ʊ����
 *  ����
 *
 * @param     obj mdarray ���饹�Υ��֥�������
 * @return    �黻��̤��Ǽ�������֥�������
 * @throw     �����Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
 */
mdarray mdarray::operator-(const mdarray &obj) const
{
    return this->ope_minus(obj);
}


/**
 * @brief  ���Ȥ�����ˡ������顼�ͤ�軻������̤������֥������Ȥ��֤� (���٥�)
 *
 *  ���Ȥ��饹���顼�ͤ�軻������̤��Ǽ�������֥������Ȥ�������������
 *  �֤��ޤ���<br>
 *   1. Ϳ����줿�����̤ȡ����ߤ� mdarray �����ķ����̤��顤�黻��̤Ȥʤ�
 *      �����̤��������(get_sz_type_from_two)��<br>
 *   2. �嵭�η����̤���Ŀ����� mdarray ������������ߤ� mdarray �ξ����
 *      ���ԡ�����(init��init_properties)��<br>
 *   3. ������ mdarray �������Ǥ��оݤȤ��� Ϳ����줿�ѿ����*�ױ黻���롥<br>
 *   4. �Ǹ�� shallow copy ����Ĥ���ե饰�򥻥åȤ���return��
 *
 * @param   szt �����顼�ͤη�����
 * @param   v_ptr �����顼�ͤΥ��ɥ쥹
 * @return  �黻��̤��Ǽ�������֥�������
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 * @note    ���Υ��дؿ��� protected �Ǥ���
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
    /* ��̷����ɤ��餫�˰��פ����硤ľ�ܱ黻��Ԥʤ� */
    if ( ret_szt == this->size_type() ) {
	/* ���Ѵ��Τ���δؿ������� */
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
	if ( szt == org_sz_type && ret_szt == new_sz_type ) { \
	    func_ope = &x_nno_n::fncname; \
	}
	SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(SEL_FUNC,,,,,,,,,,,,,,,,,,,,,,,,,,,,else);
#undef SEL_FUNC
    } else if ( ret_szt == szt ) {
	/* ���Ѵ��Τ���δؿ������� */
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
    /* fallback (�٤����Ȥ�����Ϥʤ��Ϥ�) */
    else {
	ret.ope_plus_equal(*this, true);
	/* ope */
	ret.ope_star_equal(szt,v_ptr);
    }

    ret.set_scopy_flag();
    return ret;
}

/**
 * @brief  ���Ȥ�����ˡ������顼�ͤ�軻������̤������֥������Ȥ��֤�
 *
 *  �黻�Ҥα�¦(����) �ǻ��ꤵ�줿�����顼�ͤ򡤼��Ȥ����Ǥ��٤Ƥ��Ф��ƾ軻
 *  ������̤��Ǽ�������֥������Ȥ���������֤��ޤ���<br>
 *  ���ȤȤϷ����ۤʤ�����ξ�硤�̾�Υ����顼�黻�ξ���Ʊ�ͤη��Ѵ�������
 *  �Ԥʤ��ޤ���<br>
 *  �֤���륪�֥������Ȥ�ư��⡼�ɤ� rounding ����°���ϡ����Ȥξ���Ʊ����
 *  ����
 *
 * @param     v  �����顼��
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
 */
mdarray mdarray::operator*(dcomplex v) const
{
    return this->ope_star(DCOMPLEX_ZT,(const void *)&v);
}

/**
 * @brief  ���Ȥ�����ˡ������顼�ͤ�軻������̤������֥������Ȥ��֤�
 *
 *  �黻�Ҥα�¦(����) �ǻ��ꤵ�줿�����顼�ͤ򡤼��Ȥ����Ǥ��٤Ƥ��Ф��ƾ軻
 *  ������̤��Ǽ�������֥������Ȥ���������֤��ޤ���<br>
 *  ���ȤȤϷ����ۤʤ�����ξ�硤�̾�Υ����顼�黻�ξ���Ʊ�ͤη��Ѵ�������
 *  �Ԥʤ��ޤ���<br>
 *  �֤���륪�֥������Ȥ�ư��⡼�ɤ� rounding ����°���ϡ����Ȥξ���Ʊ����
 *  ����
 *
 * @param     v  �����顼��
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
 */
mdarray mdarray::operator*(fcomplex v) const
{
    return this->ope_star(FCOMPLEX_ZT,(const void *)&v);
}

/**
 * @brief  ���Ȥ�����ˡ������顼�ͤ�軻������̤������֥������Ȥ��֤�
 *
 *  �黻�Ҥα�¦(����) �ǻ��ꤵ�줿�����顼�ͤ򡤼��Ȥ����Ǥ��٤Ƥ��Ф��ƾ軻
 *  ������̤��Ǽ�������֥������Ȥ���������֤��ޤ���<br>
 *  ���ȤȤϷ����ۤʤ�����ξ�硤�̾�Υ����顼�黻�ξ���Ʊ�ͤη��Ѵ�������
 *  �Ԥʤ��ޤ���<br>
 *  �֤���륪�֥������Ȥ�ư��⡼�ɤ� rounding ����°���ϡ����Ȥξ���Ʊ����
 *  ����
 *
 * @param     v  �����顼��
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
 */
mdarray mdarray::operator*(double v) const
{
    return this->ope_star(DOUBLE_ZT,(const void *)&v);
}

/**
 * @brief  ���Ȥ�����ˡ������顼�ͤ�軻������̤������֥������Ȥ��֤�
 *
 *  �黻�Ҥα�¦(����) �ǻ��ꤵ�줿�����顼�ͤ򡤼��Ȥ����Ǥ��٤Ƥ��Ф��ƾ軻
 *  ������̤��Ǽ�������֥������Ȥ���������֤��ޤ���<br>
 *  ���ȤȤϷ����ۤʤ�����ξ�硤�̾�Υ����顼�黻�ξ���Ʊ�ͤη��Ѵ�������
 *  �Ԥʤ��ޤ���<br>
 *  �֤���륪�֥������Ȥ�ư��⡼�ɤ� rounding ����°���ϡ����Ȥξ���Ʊ����
 *  ����
 *
 * @param     v  �����顼��
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
 */
mdarray mdarray::operator*(float v) const
{
    return this->ope_star(FLOAT_ZT,(const void *)&v);
}

/**
 * @brief  ���Ȥ�����ˡ������顼�ͤ�軻������̤������֥������Ȥ��֤�
 *
 *  �黻�Ҥα�¦(����) �ǻ��ꤵ�줿�����顼�ͤ򡤼��Ȥ����Ǥ��٤Ƥ��Ф��ƾ軻
 *  ������̤��Ǽ�������֥������Ȥ���������֤��ޤ���<br>
 *  ���ȤȤϷ����ۤʤ�����ξ�硤�̾�Υ����顼�黻�ξ���Ʊ�ͤη��Ѵ�������
 *  �Ԥʤ��ޤ���<br>
 *  �֤���륪�֥������Ȥ�ư��⡼�ɤ� rounding ����°���ϡ����Ȥξ���Ʊ����
 *  ����
 *
 * @param     v  �����顼��
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
 */
mdarray mdarray::operator*(long long v) const
{
    return this->ope_star(sizeof(v),(const void *)&v);
}

/**
 * @brief  ���Ȥ�����ˡ������顼�ͤ�軻������̤������֥������Ȥ��֤�
 *
 *  �黻�Ҥα�¦(����) �ǻ��ꤵ�줿�����顼�ͤ򡤼��Ȥ����Ǥ��٤Ƥ��Ф��ƾ軻
 *  ������̤��Ǽ�������֥������Ȥ���������֤��ޤ���<br>
 *  ���ȤȤϷ����ۤʤ�����ξ�硤�̾�Υ����顼�黻�ξ���Ʊ�ͤη��Ѵ�������
 *  �Ԥʤ��ޤ���<br>
 *  �֤���륪�֥������Ȥ�ư��⡼�ɤ� rounding ����°���ϡ����Ȥξ���Ʊ����
 *  ����
 *
 * @param     v  �����顼��
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
 */
mdarray mdarray::operator*(long v) const
{
    return this->ope_star(sizeof(v),(const void *)&v);
}

/**
 * @brief  ���Ȥ�����ˡ������顼�ͤ�軻������̤������֥������Ȥ��֤�
 *
 *  �黻�Ҥα�¦(����) �ǻ��ꤵ�줿�����顼�ͤ򡤼��Ȥ����Ǥ��٤Ƥ��Ф��ƾ軻
 *  ������̤��Ǽ�������֥������Ȥ���������֤��ޤ���<br>
 *  ���ȤȤϷ����ۤʤ�����ξ�硤�̾�Υ����顼�黻�ξ���Ʊ�ͤη��Ѵ�������
 *  �Ԥʤ��ޤ���<br>
 *  �֤���륪�֥������Ȥ�ư��⡼�ɤ� rounding ����°���ϡ����Ȥξ���Ʊ����
 *  ����
 *
 * @param     v  �����顼��
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
 */
mdarray mdarray::operator*(int v) const
{
    return this->ope_star(sizeof(v),(const void *)&v);
}

/**
 * @brief  ���Ȥ�����ˡ����ꤵ�줿�����軻������̤������֥������Ȥ��֤� (���٥�)
 *
 *  mdarray���饹�Υ��֥������Ȥ�����Ǽ��Ȥ�軻������̤��Ǽ�������֥�������
 *  ����������֤��ޤ�. 
 *
 * @param   obj mdarray���饹�Υ��֥�������
 * @return  �黻��̤��Ǽ�������֥�������
 * @throw   �����Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
 * @note    ���Υ��дؿ��� protected �Ǥ���
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
    /* ��̷����ɤ��餫�˰��פ����硤ľ�ܱ黻��Ԥʤ� */
    if ( ret_szt == this->size_type() ) {
	/* ���Ѵ��Τ���δؿ������� */
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
	if ( obj.size_type() == org_sz_type && ret_szt == new_sz_type ) { \
	    func_ope = &x_nno_n::fncname; \
	}
	SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(SEL_FUNC,,,,,,,,,,,,,,,,,,,,,,,,,,,,else);
#undef SEL_FUNC
    } else if ( ret_szt == obj.size_type() ) {
	/* ���Ѵ��Τ���δؿ������� */
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
	    /* �Ƶ��ƤӽФ��� arr �� arr �η׻���Ԥʤ� */
	    ope_arr_x_arr_r(&ret, *this, obj, Ope_star, func_ope, 
		     ret_ndim - 1,
		     ret.byte_length(), this->byte_length(), obj.byte_length(),
		     ret.data_ptr(), this->data_ptr_cs(),obj.data_ptr_cs());
	}
    }
    /* fallback (�٤����Ȥ�����Ϥʤ��Ϥ�) */
    else {
	ret.ope_plus_equal(*this, true);
	/* ope */
	ret.ope_star_equal(obj, true);
    }

    ret.set_scopy_flag();
    return ret;
}

/**
 * @brief  ���Ȥ�����ˡ����ꤵ�줿�����軻������̤������֥������Ȥ��֤�
 *
 *  �黻�Ҥα�¦(����) �ǻ��ꤵ�줿mdarray ���饹�Υ��֥������Ȥ�����ȡ�����
 *  �Ȥ�軻������̤��Ǽ�������֥������Ȥ���������֤��ޤ���<br>
 *  �����ϡ��ƥ��饹�Ǥ���mdarray ���饹�Ǥ��Τǡ����ȤȤϰۤʤ뷿�����ꤵ�줿
 *  ���֥������Ȥ����Ǥ��ޤ������ξ�硤�̾�Υ����顼�黻�ξ���Ʊ�ͤη�
 *  �Ѵ��������Ԥʤ��ޤ���<br>
 *  �֤���륪�֥������Ȥ�ư��⡼�ɤ� rounding ����°���ϡ����Ȥξ���Ʊ����
 *  ����
 *
 * @param     obj mdarray ���饹�Υ��֥�������
 * @return    �黻��̤��Ǽ�������֥�������
 * @throw     �����Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
 */
mdarray mdarray::operator*(const mdarray &obj) const
{
    return this->ope_star(obj);
}


/**
 * @brief  ���Ȥ�����򡤥����顼�ͤǽ���������̤������֥������Ȥ��֤� (���٥�)
 *
 *  ���Ȥ��饹���顼�ͤ����������̤��Ǽ�������֥������Ȥ���������֤��ޤ�.
 *
 * @param   szt �����顼�ͤη��򼨤���(������)
 * @param   v_ptr �����顼��
 * @return  �黻��̤��Ǽ�������֥�������
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 * @note    ���Υ��дؿ��� protected �Ǥ���
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
    /* ��̷����ɤ��餫�˰��פ����硤ľ�ܱ黻��Ԥʤ� */
    if ( ret_szt == this->size_type() ) {
	/* ���Ѵ��Τ���δؿ������� */
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
	if ( szt == org_sz_type && ret_szt == new_sz_type ) { \
	    func_ope = &x_nno_n::fncname; \
	}
	SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(SEL_FUNC,,,,,,,,,,,,,,,,,,,,,,,,,,,,else);
#undef SEL_FUNC
    } else if ( ret_szt == szt ) {
	/* ���Ѵ��Τ���δؿ������� */
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
    /* fallback (�٤����Ȥ�����Ϥʤ��Ϥ�) */
    else {
	ret.ope_plus_equal(*this, true);
	/* ope */
	ret.ope_slash_equal(szt,v_ptr);
    }

    ret.set_scopy_flag();
    return ret;
}

/**
 * @brief  ���Ȥ�����򡤥����顼�ͤǽ���������̤������֥������Ȥ��֤�
 *
 *  �黻�Ҥα�¦(����) �ǻ��ꤵ�줿�����顼�ͤ򡤼��Ȥ����Ǥ��줾�줫���������
 *  ��̤��Ǽ�������֥������Ȥ���������֤��ޤ���<br>
 *  ���ȤȤϷ����ۤʤ�����ξ�硤�̾�Υ����顼�黻�ξ���Ʊ�ͤη��Ѵ�������
 *  �Ԥʤ��ޤ���<br>
 *  �֤���륪�֥������Ȥ�ư��⡼�ɤ�rounding °���ϡ����Ȥξ���Ʊ���Ǥ���
 *
 * @param     v  �����顼��
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
 */
mdarray mdarray::operator/(dcomplex v) const
{
    return this->ope_slash(DCOMPLEX_ZT,(const void *)&v);
}

/**
 * @brief  ���Ȥ�����򡤥����顼�ͤǽ���������̤������֥������Ȥ��֤�
 *
 *  �黻�Ҥα�¦(����) �ǻ��ꤵ�줿�����顼�ͤ򡤼��Ȥ����Ǥ��줾�줫���������
 *  ��̤��Ǽ�������֥������Ȥ���������֤��ޤ���<br>
 *  ���ȤȤϷ����ۤʤ�����ξ�硤�̾�Υ����顼�黻�ξ���Ʊ�ͤη��Ѵ�������
 *  �Ԥʤ��ޤ���<br>
 *  �֤���륪�֥������Ȥ�ư��⡼�ɤ�rounding °���ϡ����Ȥξ���Ʊ���Ǥ���
 *
 * @param     v  �����顼��
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
 */
mdarray mdarray::operator/(fcomplex v) const
{
    return this->ope_slash(FCOMPLEX_ZT,(const void *)&v);
}

/**
 * @brief  ���Ȥ�����򡤥����顼�ͤǽ���������̤������֥������Ȥ��֤�
 *
 *  �黻�Ҥα�¦(����) �ǻ��ꤵ�줿�����顼�ͤ򡤼��Ȥ����Ǥ��줾�줫���������
 *  ��̤��Ǽ�������֥������Ȥ���������֤��ޤ���<br>
 *  ���ȤȤϷ����ۤʤ�����ξ�硤�̾�Υ����顼�黻�ξ���Ʊ�ͤη��Ѵ�������
 *  �Ԥʤ��ޤ���<br>
 *  �֤���륪�֥������Ȥ�ư��⡼�ɤ�rounding °���ϡ����Ȥξ���Ʊ���Ǥ���
 *
 * @param     v  �����顼��
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
 */
mdarray mdarray::operator/(double v) const
{
    return this->ope_slash(DOUBLE_ZT,(const void *)&v);
}

/**
 * @brief  ���Ȥ�����򡤥����顼�ͤǽ���������̤������֥������Ȥ��֤�
 *
 *  �黻�Ҥα�¦(����) �ǻ��ꤵ�줿�����顼�ͤ򡤼��Ȥ����Ǥ��줾�줫���������
 *  ��̤��Ǽ�������֥������Ȥ���������֤��ޤ���<br>
 *  ���ȤȤϷ����ۤʤ�����ξ�硤�̾�Υ����顼�黻�ξ���Ʊ�ͤη��Ѵ�������
 *  �Ԥʤ��ޤ���<br>
 *  �֤���륪�֥������Ȥ�ư��⡼�ɤ�rounding °���ϡ����Ȥξ���Ʊ���Ǥ���
 *
 * @param     v  �����顼��
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
 */
mdarray mdarray::operator/(float v) const
{
    return this->ope_slash(FLOAT_ZT,(const void *)&v);
}

/**
 * @brief  ���Ȥ�����򡤥����顼�ͤǽ���������̤������֥������Ȥ��֤�
 *
 *  �黻�Ҥα�¦(����) �ǻ��ꤵ�줿�����顼�ͤ򡤼��Ȥ����Ǥ��줾�줫���������
 *  ��̤��Ǽ�������֥������Ȥ���������֤��ޤ���<br>
 *  ���ȤȤϷ����ۤʤ�����ξ�硤�̾�Υ����顼�黻�ξ���Ʊ�ͤη��Ѵ�������
 *  �Ԥʤ��ޤ���<br>
 *  �֤���륪�֥������Ȥ�ư��⡼�ɤ�rounding °���ϡ����Ȥξ���Ʊ���Ǥ���
 *
 * @param     v  �����顼��
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
 */
mdarray mdarray::operator/(long long v) const
{
    return this->ope_slash(sizeof(v),(const void *)&v);
}

/**
 * @brief  ���Ȥ�����򡤥����顼�ͤǽ���������̤������֥������Ȥ��֤�
 *
 *  �黻�Ҥα�¦(����) �ǻ��ꤵ�줿�����顼�ͤ򡤼��Ȥ����Ǥ��줾�줫���������
 *  ��̤��Ǽ�������֥������Ȥ���������֤��ޤ���<br>
 *  ���ȤȤϷ����ۤʤ�����ξ�硤�̾�Υ����顼�黻�ξ���Ʊ�ͤη��Ѵ�������
 *  �Ԥʤ��ޤ���<br>
 *  �֤���륪�֥������Ȥ�ư��⡼�ɤ�rounding °���ϡ����Ȥξ���Ʊ���Ǥ���
 *
 * @param     v  �����顼��
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
 */
mdarray mdarray::operator/(long v) const
{
    return this->ope_slash(sizeof(v),(const void *)&v);
}

/**
 * @brief  ���Ȥ�����򡤥����顼�ͤǽ���������̤������֥������Ȥ��֤�
 *
 *  �黻�Ҥα�¦(����) �ǻ��ꤵ�줿�����顼�ͤ򡤼��Ȥ����Ǥ��줾�줫���������
 *  ��̤��Ǽ�������֥������Ȥ���������֤��ޤ���<br>
 *  ���ȤȤϷ����ۤʤ�����ξ�硤�̾�Υ����顼�黻�ξ���Ʊ�ͤη��Ѵ�������
 *  �Ԥʤ��ޤ���<br>
 *  �֤���륪�֥������Ȥ�ư��⡼�ɤ�rounding °���ϡ����Ȥξ���Ʊ���Ǥ���
 *
 * @param     v  �����顼��
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
 */
mdarray mdarray::operator/(int v) const
{
    return this->ope_slash(sizeof(v),(const void *)&v);
}

/**
 * @brief  ���Ȥ�����򡤻��ꤵ�줿����ǽ���������̤������֥������Ȥ��֤� (���٥�)
 *
 *  mdarray���饹�Υ��֥������Ȥ�����Ǽ��Ȥ����������̤�
 *  ��Ǽ�������֥������Ȥ���������֤��ޤ�. 
 *
 * @param   obj mdarray���饹�Υ��֥�������
 * @return  �黻��̤��Ǽ�������֥�������
 * @throw   �����Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
 * @note    ���Υ��дؿ��� protected �Ǥ���
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
    /* ��̷����ɤ��餫�˰��פ����硤ľ�ܱ黻��Ԥʤ� */
    if ( ret_szt == this->size_type() ) {
	/* ���Ѵ��Τ���δؿ������� */
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
	if ( obj.size_type() == org_sz_type && ret_szt == new_sz_type ) { \
	    func_ope = &x_nno_n::fncname; \
	}
	SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(SEL_FUNC,,,,,,,,,,,,,,,,,,,,,,,,,,,,else);
#undef SEL_FUNC
    } else if ( ret_szt == obj.size_type() ) {
	/* ���Ѵ��Τ���δؿ������� */
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
	    /* �Ƶ��ƤӽФ��� arr �� arr �η׻���Ԥʤ� */
	    ope_arr_x_arr_r(&ret, *this, obj, Ope_slash, func_ope, 
		     ret_ndim - 1,
		     ret.byte_length(), this->byte_length(), obj.byte_length(),
		     ret.data_ptr(), this->data_ptr_cs(),obj.data_ptr_cs());
	}
    }
    /* fallback (�٤����Ȥ�����Ϥʤ��Ϥ�) */
    else {
	ret.ope_plus_equal(*this, true);
	/* ope */
	ret.ope_slash_equal(obj, true);
    }

    ret.set_scopy_flag();
    return ret;
}

/**
 * @brief  ���Ȥ�����򡤻��ꤵ�줿����ǽ���������̤������֥������Ȥ��֤�
 *
 *  �黻�Ҥα�¦(����) �ǻ��ꤵ�줿mdarray ���饹�Υ��֥������Ȥ������
 *  ���Ȥ������������̤��Ǽ�������֥������Ȥ���������֤��ޤ���<br>
 *  �����ϡ��ƥ��饹�Ǥ���mdarray ���饹�Ǥ��Τǡ����ȤȤϰۤʤ뷿�����ꤵ�줿
 *  ���֥������Ȥ����Ǥ��ޤ������ξ�硤�̾�Υ����顼�黻�ξ���Ʊ�ͤη�
 *  �Ѵ��������Ԥʤ��ޤ���<br>
 *  �֤���륪�֥������Ȥ�ư��⡼�ɤ� rounding ����°���ϡ����Ȥξ���Ʊ����
 *  ����
 *
 * @param     obj mdarray ���饹�Υ��֥�������
 * @return    �黻��̤��Ǽ�������֥�������
 * @throw     �����Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
 */
mdarray mdarray::operator/(const mdarray &obj) const
{
    return this->ope_slash(obj);
}

/* ********************************************** */
/* ��λ : ��³��Ū���ڥ졼����ɬ�פʥ����� : ��λ */
/* ********************************************** */


/*
 * member functions to handle the whole object and properties
 */

/* 
 *  ���Ȥν������Ԥ��ޤ�(���٥�).
 * 
 * @deprecated  ���ߤ�̤����
 * @return	���Ȥλ���
 * @note	���Υ��дؿ���private�Ǥ���<br>
 *              this->extptr_rec �ι�����this->update_length_info() �θƤӽФ�
 *              �ϹԤʤ�ʤ��Τǡ��ƤӽФ�¦���н褷�Ƥ���������
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
 * @brief  ���֥������Ȥν���� (��������ѹ��ʤ�)
 *
 *  ���Ȥ�����°�����������ޤ���<br>
 *  ������Ϥ��Τޤޡ����󥵥��� 0 �Ȥ��ƥ��֥������Ȥ��������ޤ���
 *
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
 * @note      �ּ�ư�ꥵ�����⡼�ɡפǽ�������ޤ���
 */
mdarray &mdarray::init()
{
    debug_report("()");

    /* shallow copy �ط��Υ��꡼�󥢥å� */
    this->cleanup_shallow_copy(false);

    /* ��������� */
    this->__force_free();
    this->__force_init(this->size_type(), false);	/* keep sz_type */

    if ( this->extptr_rec != NULL ) *(this->extptr_rec) = this->data_ptr();

    /* update_length_info() ��ľ��˸Ƥ�ɬ�פ����� */
    this->update_arr_ptr_2d();
    this->update_arr_ptr_3d();

    return *this;
}

/**
 * @brief  ���֥������Ȥν����
 *
 *  ���Ȥ�����°�����������ޤ���<br>
 *  ��������Ǥη��� sz_type �ǻ��ꤷ�ޤ���
 *
 * @param     sz_type �ѿ��η�����
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
 * @note      �ּ�ư�ꥵ�����⡼�ɡפǽ�������ޤ���
 */
mdarray &mdarray::init( ssize_t sz_type )
{
    if ( this->is_acceptable_size_type(sz_type) == false ) {
    	err_throw1(__FUNCTION__,"ERROR","prohibited sz_type: %d",(int)sz_type);
    }

    /* shallow copy �ط��Υ��꡼�󥢥å� */
    this->cleanup_shallow_copy(false);

    /* ��������� */
    this->__force_free();
    this->__force_init(sz_type, false);

    if ( this->extptr_rec != NULL ) *(this->extptr_rec) = this->data_ptr();

    /* update_length_info() ��ľ��˸Ƥ�ɬ�פ����� */
    this->update_arr_ptr_2d();
    this->update_arr_ptr_3d();

    return *this;
}

/**
 * @brief  ���֥������Ȥν����
 *
 *  ���Ȥ�����°�����������ޤ���<br>
 *  ��������Ǥη��� sz_type �ǻ��ꤷ�ޤ���
 *
 * @param     sz_type �ѿ��η�����
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
 * @note      �ּ�ư�ꥵ�����⡼�ɡפǽ�������ޤ���
 */
mdarray &mdarray::init( ssize_t sz_type, bool auto_resize )
{
    if ( this->is_acceptable_size_type(sz_type) == false ) {
    	err_throw1(__FUNCTION__,"ERROR","prohibited sz_type: %d",(int)sz_type);
    }

    /* shallow copy �ط��Υ��꡼�󥢥å� */
    this->cleanup_shallow_copy(false);

    /* ��������� */
    this->__force_free();
    this->__force_init(sz_type, false);

    this->set_auto_resize(auto_resize);

    if ( this->extptr_rec != NULL ) *(this->extptr_rec) = this->data_ptr();

    /* update_length_info() ��ľ��˸Ƥ�ɬ�פ����� */
    this->update_arr_ptr_2d();
    this->update_arr_ptr_3d();

    return *this;
}

/**
 * @brief  ���֥������Ȥν���� (n����)
 *
 *  ���Ȥ�����°�����������ޤ���<br>
 *  sz_type �����Ǥη���ndim �Ǽ�������naxisx[] �ǳƼ��������ǿ�����ꤷ�ޤ���
 *
 * @param     sz_type �ѿ��η�����
 * @param     auto_resize ��ư�ꥵ�����⡼�ɤ����ꤹ����� true
 * @param     naxisx[] �Ƽ��������ǿ�
 * @param     ndim ���󼡸���
 * @param     init_buf �����ͤ�ǥե�����ͤ�����ʤ� true
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
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

    /* shallow copy �ط��Υ��꡼�󥢥å� */
    this->cleanup_shallow_copy(false);

    /* ��������� */
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
    /* update_length_info() ��ľ��˸Ƥ�ɬ�פ����� */
    this->update_arr_ptr_2d();
    this->update_arr_ptr_3d();

    return *this;
}

#if 0 /* SLI__DISABLE_INCOMPATIBLE_V1_4 */
/* 1���� */
/**
 * @brief  ����ν���� (1����)
 *
 *  ���Ȥ�������������ޤ���<br>
 *  sz_type �����Ǥη���naxis0 �� 1 �����ܤ����ǿ�����ꤷ��
 *  1 �������������ĥ��֥������Ȥ�������ޤ���
 *
 * @deprecated  ��侩
 * @param     sz_type �ѿ��η�����
 * @param     naxis0 �����ֹ�0 �μ���(1 ������) �����ǿ�
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
 * @note      �ּ�ư�ꥵ�����⡼�ɡפǽ�������ޤ���
 */
mdarray &mdarray::init( ssize_t sz_type, size_t naxis0 )
{
    const size_t naxisx[] = {naxis0};

    this->mdarray::init(sz_type, naxisx, 1, true);

    return *this;
}

/* 2���� */
/**
 * @brief  ����ν���� (2����)
 *
 *  ���Ȥ�������������ޤ���<br>
 *  sz_type �����Ǥη���naxis0 �� 1 �����ܤ����ǿ���
 *  naxis1 �� 2 �����ܤ����ǿ����ꤷ��2 �������������ĥ��֥������Ȥ�
 *  �������ޤ���
 *
 * @deprecated  ��侩
 * @param     sz_type �ѿ��η�����
 * @param     naxis0 �����ֹ�0 �μ���(1������) �����ǿ�
 * @param     naxis1 �����ֹ�1 �μ���(2������) �����ǿ�
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
 * @note      �ּ�ư�ꥵ�����⡼�ɡפǽ�������ޤ���
 */
mdarray &mdarray::init( ssize_t sz_type, size_t naxis0, size_t naxis1 )
{
    const size_t naxisx[] = {naxis0, naxis1};

    this->mdarray::init(sz_type, naxisx, 2, true);

    return *this;
}

/* 3���� */
/**
 * @brief  ����ν���� (3����)
 *
 *  ���Ȥ�������������ޤ���<br>
 *  sz_type �����Ǥη���naxis0 �� 1 �����ܤ����ǿ���
 *  naxis1 �� 2 �����ܤ����ǿ���naxis2 �� 3 �����ܤ����ǿ�����ꤷ��
 *  3 �������������ĥ��֥������Ȥ�������ޤ���
 *
 * @deprecated  ��侩
 * @param     sz_type �ѿ��η�����
 * @param     naxis0 �����ֹ�0 �μ���(1������) �����ǿ�
 * @param     naxis1 �����ֹ�1 �μ���(2������) �����ǿ�
 * @param     naxis2 �����ֹ�2 �μ���(3������) �����ǿ�
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
 * @note      �ּ�ư�ꥵ�����⡼�ɡפǽ�������ޤ���
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
 * @brief  ���֥������ȤΥ��ԡ�
 *
 *  obj ����������Ƥ�°�������٤Ƥ򼫿Ȥ˥��ԡ����ޤ���<br>
 *  obj ���ؿ��ˤ�ä��֤��줿�ƥ�ݥ�ꥪ�֥������Ȥξ�硤���Ĥ���Ƥ����
 *  shallow copy ���Ԥʤ��ޤ���
 *
 * @param     obj ���ԡ����Ȥʤ륪�֥�������
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
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

	/* shallow copy �ط��Υ��꡼�󥢥å� */
	this->cleanup_shallow_copy(false);

	/* ��ǽ�ʤ�С�shallow copy ��Ԥʤ� */
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
 * @brief  ��ư�ꥵ��������ư��������ͼθ��������������ˡ������򥳥ԡ�
 *
 *  src_obj �� auto_resize, auto_init, rounding, alloc_strategy �������
 *  ���ԡ����ޤ���
 *
 * @param   src_obj ���ԡ�����륪�֥�������
 * @return  ���Ȥλ���
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
 * @brief  ���ߤμ�ư�ꥵ�����β��ݤ�������ѹ�
 *
 *  �ꥵ�����⡼�ɤ�(true)���ޤ��ϵ�(false) �����ꤷ�ޤ���<br>
 *  ư��⡼�ɤ��ּ�ư�ꥵ�����⡼�ɡפξ��� true��
 *  ư��⡼�ɤ��ּ�ư�ꥵ�����⡼�ɡפξ��� false �Ǥ���
 *
 * @param     tf �ꥵ�����⡼��
 * @return    ���Ȥλ���
 *            
 */
mdarray &mdarray::set_auto_resize( bool tf )
{
    this->auto_resize_rec = tf;
    return *this;
}

/**
 * @brief  ���ߤμ�ư������β��ݤ�������ѹ�
 *
 *  ��ư������⡼�ɤ�(true)���ޤ��ϵ�(false) �����ꤷ�ޤ���
 *
 * @param     tf ��ư������⡼��
 * @return    ���Ȥλ���
 *            
 */
mdarray &mdarray::set_auto_init( bool tf )
{
    this->auto_init_rec = tf;
    return *this;
}

/**
 * @brief  ���ߤλͼθ����β��ݤ�������ѹ�
 *
 *  �����Ĥ��ι��٥���дؿ��ˤ����ơ���ư�����������������Ѵ�������ˡ�
 *  �ͼθ�����Ԥ����ݤ����ꤷ�ޤ���<br>
 *  �ͼθ�������褦�����ꤵ��Ƥ�����Ͽ�(true)��
 *  �ͼθ������ʤ��褦�����ꤵ��Ƥ�����ϵ�(false) ����ꤷ�ޤ���
 *
 * @param   tf �ͼθ���������
 * @return  ���Ȥλ���
 * @note    �ͼθ�����°������ǽ������дؿ��ϡ�
 *          lvalue()��llvalue() ���дؿ���assign default() ���дؿ���
 *          assign() ���дؿ���convert() ���дؿ��������������дؿ����̡�
 *            
 */
mdarray &mdarray::set_rounding( bool tf )
{
    this->rounding_rec = tf;
    return *this;
}

/**
 * @brief  ���������ˡ��������ѹ�
 *
 *  �����ѥ������ݤ�����ˡ��ɤΤ褦����ˡ�ǹԤʤ�������ꤷ�ޤ���
 *  ���� 3 �Ĥ������򤷤ޤ���<br>
 *    "min"  ... �Ǿ��¤���ݡ��ꥵ��������ɬ�� realloc() ���ƤФ�ޤ���<br>
 *    "pow"  ... 2��n��ǳ��ݡ�<br>
 *    "auto" ... ��ư�ꥵ�����⡼�ɤλ��ˤ� "pow"�������Ǥʤ����� "min" ��
 *               �ꥵ����������¹Ԥ��ޤ���<br>
 *    NULL��¾ ... ���ߤ���ˡ��ݻ����ޤ���
 *
 * @param   strategy ���������ˡ������
 * @return  ���Ȥλ���
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
 * @brief  ����������Ǥη��Ѵ�
 *
 *  ���Ȥ������Ǥ��ͤ�Ϳ����줿������(sz_type)���Ѵ������񤭴����ޤ���<br>
 *  �Ѵ�����η��ˤ�äƤϡ��������夢�դ줬ȯ�����ޤ��Τ���դ��Ʋ�������
 *
 * @param     sz_type ���Ǥη�����
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
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
	prms.total_len_elements = 0;			/* �����ʤ� */
	prms.round_flg = this->rounding();		/* �ͼθ���/�ڤ�Τ� */
	return this->convert_via_udf(sz_type, func_cnv, (void *)&prms);
    }

    err_report(__FUNCTION__,"WARNING","unsupported sz_type; resizing only");
    return this->convert_via_udf(sz_type,
			(void (*)(const void *,void *,size_t,int,void *))NULL,
			(void *)NULL);
}

/**
 * @brief  ����������Ǥη��Ѵ� (�桼���ؿ���ͳ)
 *
 *  ���Ȥ������Ǥ��ͤ� �桼������ؿ� func ��ͳ�� sz_type �����������Ѵ�������
 *  �������ޤ����桼������ؿ���Ϳ�����Ѵ����ε�ư���Ѥ��뤳�Ȥ��Ǥ��ޤ���<br>
 *  �桼������ؿ�����1�����ˤ�����θ��γ����ǥ��ɥ쥹������2�����ˤ��Ѵ����
 *  �����ǤΥ��ɥ쥹������3�����ˤ���1����2������Ϳ����줿���ǤθĿ�����
 *  ��4�����ˤ��Ѵ�������(������ʤ����������ʤ���)����5�����ˤ� user_ptr 
 *  ��Ϳ�����ޤ���<br>
 *  �Ѵ�����η��ˤ�äƤϡ��������夢�դ줬ȯ�����ޤ��Τ���դ��Ʋ�������
 *
 * @param      sz_type  ���Ǥη�����
 * @param      func     �桼���ؿ��Υ��ɥ쥹
 * @param      user_ptr �桼���ؿ��κǸ��Ϳ������桼���Υݥ���
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
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

    /* �礭���ʤ���ϡ��ǽ�˺Ƴ��ݤ��� */
    if ( org_bytes < new_bytes ) {
	if ( this->realloc_arr_rec(new_bytes * all_len) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","realloc() failed");
	}
	if ( this->extptr_rec != NULL ) *(this->extptr_rec) = this->data_ptr();
    }
    /* �������ʤ���ϺǸ��realloc����褦�˥ե饰���Ƥ� */
    else if ( new_bytes < org_bytes ) {
	buffer_smaller = true;
    }

    if ( func != NULL ) {
	/* �礭���ʤ���ϸ���� */
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

    /* �������ʤ��� */
    if ( buffer_smaller == true ) {
	if ( this->realloc_arr_rec(new_bytes * all_len) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","realloc() failed");
	}
	if ( this->extptr_rec != NULL ) *(this->extptr_rec) = this->data_ptr();
    }

 done:
    this->sz_type_rec = sz_type;
    /* default�ͤ�Ĥ��Ǥ��Ѵ����� */
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
    /* update_length_info() ��ľ��˸Ƥ�ɬ�פ����� */
    this->update_arr_ptr_2d();
    this->update_arr_ptr_3d();

    return *this;
}

/**
 * @brief  2�ĤΥ��֥������ȴ֤Ǥ����ƤΥ���å�
 *
 *  ���ꤵ�줿���֥�������sobj �����Ƥȼ��Ȥ����Ƥ������ؤ��ޤ���
 *  ��������ơ�����Υ�������°�������٤Ƥξ��֤������ؤ��ޤ���
 *
 * @param     sobj mdarray ���饹�Υ��֥�������
 * @return    ���Ȥλ���    
 * @throw     sobj �ǻ��ꤵ�줿���֥������Ȥ������ʾ��
 */
mdarray &mdarray::swap( mdarray &sobj )
{
    if ( &sobj == this ) return *this;

    /* �Хåե���ľ�����뤿��Ƥ� */
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

    /* �����ݥ��󥿤�������Τ� update_arr_ptr_2d3d() ��Ƥ� */
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
 * @brief  ���Ȥ����Ƥ���ꥪ�֥������Ȥإ��ԡ�
 *
 *  ���Ȥ����Ƥ����Ƥ���ꤵ�줿���֥������� dest_obj �إ��ԡ����ޤ���
 *  ��������Ĺ���������Ƥ�°�������ԡ�����ޤ�������(���ԡ���) ������ϲ��Ѥ���
 *  �ޤ���<br>
 *  ����������Ĺ�� dest_obj �ȼ��ȤȤ����������ϡ������ѥХåե��κƳ��ݤ�
 *  �Ԥʤ�줺����¸�ΥХåե�����������Ƥ����ԡ�����ޤ���<br>
 *  ��=�ױ黻�Ҥ� .init(obj) �Ȥϰۤʤꡤ��� deep copy ���¹Ԥ���ޤ���
 *
 * @param     dest_obj ���ԡ���Υ��֥������ȤΥ��ɥ쥹
 * @return    ���ԡ��������ǿ�(����߹Կ��ߥ쥤���)��<br>
 *            �����������ʾ��������
 * @throw     dest_obj �ǻ��ꤵ�줿���֥������Ȥ������ʾ��
 * @throw     �Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
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

	    /* shallow copy �ط��Υ��꡼�󥢥å� */
	    dest_obj->cleanup_shallow_copy(false);
	
	    /* deep copy */
	    dest_obj->__deep_init(src_obj);
	}
	return dest_obj->length();
    }
    else return -1;
}

/**
 * @brief  ���Ȥ����Ƥ���ꥪ�֥������Ȥإ��ԡ� (��侩)
 * @deprecated ��侩��<br>
 * ssize_t mdarray::copy( mdarray *dest_obj ) const <br>
 * �򤪻Ȥ�����������
 */
ssize_t mdarray::copy( mdarray &dest_obj ) const
{
    debug_report("( mdarray &dest_obj )");
    return this->copy(&dest_obj);
}

/**
 * @brief  ���Ȥ����Ƥ���ꥪ�֥������Ȥذܴ�
 *
 *  ���Ȥ���������Ƥ�dest_obj �ˤ����ꤵ�줿���֥������Ȥءְܴɡפ��ޤ�
 *  (��������Ĺ���Ƽ�°�������ꤵ�줿���֥������Ȥ˥��ԡ�����ޤ�)���ܴɤ�
 *  ��̡����Ȥ�����Ĺ�ϥ���ˤʤ�ޤ���<br>
 *  dest_obj �ˤĤ��Ƥ������ѥХåե��κƳ��ݤϹԤʤ�줺�����Ȥ������ѥХåե�
 *  �ˤĤ��Ƥδ������¤� dest_obj �˾��Ϥ�������ˤʤäƤ��� ��®��ư��ޤ���
 *
 * @param     dest_obj �ܴɡ����ԡ���Υ��֥������ȤΥ��ɥ쥹
 * @return    ���Ȥλ���
 * @throw     dest_obj �ǻ��ꤵ�줿���֥������Ȥ������ʾ��
 * @throw     �Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
 */
mdarray &mdarray::cut( mdarray *dest_obj )
{
    debug_report("( mdarray *dest_obj )");

    if ( dest_obj == NULL ) {

	if ( 0 < this->dim_size_rec ) this->reallocate(NULL,0,false);

    }
    /* dest_obj �����Ȥʤ鲿�⤷�ʤ� */
    else if ( dest_obj != this ) {

	if ( dest_obj->is_acceptable_size_type(this->size_type()) == false ) {
	    err_throw1(__FUNCTION__,"ERROR","prohibited size_type: %d",
		       (int)(this->size_type()));
	}

	/* �Хåե���ľ�����뤿��Ƥ� */
	this->cleanup_shallow_copy(true);
	dest_obj->cleanup_shallow_copy(false);

	/* shallow copy �ε�ǽ��ȤäƥХåե����ɥ쥹�򥳥ԡ� */
	try {
	    dest_obj->__shallow_init(*this, true);
	}
	catch (...) {
	    this->__force_init(this->size_type(), false);
	    err_throw(__FUNCTION__,"FATAL","caught exception");
	}
	this->__force_init(this->size_type(), false);

	/* auto_resize �ʤɤ�°���� dest_obj �������� */
	this->init_properties(*dest_obj);

	/* default�� �� dest_obj �������� */
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
	/* update_length_info() ��ľ��˸Ƥ�ɬ�פ����� */
	this->update_arr_ptr_2d( (dest_obj->_arr_ptr_2d_rec != NULL) );
	this->update_arr_ptr_3d( (dest_obj->_arr_ptr_3d_rec != NULL) );

    }

    return *this;
}

/**
 * @brief  ���Ȥ������ (x,y) �ǤΥȥ�󥹥ݡ���
 *
 *  ���Ȥ�����Υ����ȥ��Ȥ������ؤ��ޤ���
 * 
 * @note  ��®�ʥ��르�ꥺ�����Ѥ��Ƥ��ޤ���
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
 * @brief  ���Ȥ������ (x,y,z)��(z,x,y) �Υȥ�󥹥ݡ���
 *
 *  ���Ȥ�����μ� (x,y,z) �� (z,x,y) ���Ѵ����ޤ���
 * 
 * @note  ��®�ʥ��르�ꥺ�����Ѥ��Ƥ��ޤ���
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
 * @brief  ���Ȥ������ (x,y) �Ǥβ�ž (90��ñ��)
 *
 *  ���Ȥ������ (x,y) �̤ˤĤ��Ƥβ�ž(90��ñ��)��Ԥʤ��ޤ���<br>
 *  ���������Ȥ�����硤ȿ���פޤ��ǻ��ꤷ�ޤ���
 *
 * @param  angle 90,-90, 180 �Τ����줫�����
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
 * @brief  ���ߤμ�ư�ꥵ�����β��ݤ���������
 *
 *  �ꥵ�����⡼�ɤ�(true)���ޤ��ϵ�(false) �Ǽ������ޤ���<br>
 *  ư��⡼�ɤ��ּ�ư�ꥵ�����⡼�ɡפξ��� true��
 *  ư��⡼�ɤ��ּ�ư�ꥵ�����⡼�ɡפξ��� false �Ǥ���
 *
 * @return    ư��⡼��(��ư�ꥵ�����⡼�ɤξ���true)
 *            
 */
bool mdarray::auto_resize() const
{
    return this->auto_resize_rec;
}

/**
 * @brief  ���ߤμ�ư������β��ݤ���������
 *
 *  ��ư������⡼�ɤ�(true)���ޤ��ϵ�(false) �Ǽ������ޤ���
 *
 * @return    ư��⡼��(��ư������⡼�ɤξ���true)
 *            
 */
bool mdarray::auto_init() const
{
    return this->auto_init_rec;
}

/**
 * @brief  ���ߤλͼθ����β��ݤ���������
 *
 *  �ͼθ����β��ݤ�������ɤ߼��ޤ���<br>
 *  �ͼθ�������褦�����ꤵ��Ƥ�����Ͽ�(true)��
 *  �ͼθ������ʤ��褦�����ꤵ��Ƥ�����ϵ�(false) ���֤�ޤ���<br>
 *  mdarray ���饹�Υ��֥��������������ν�����֤Ǥϡ��ͼθ������ʤ��褦��
 *  ���ꤵ��Ƥ��ޤ���
 *
 * @return  �ͼθ���ư���°�� (�ͼθ�������褦�����ꤵ��Ƥ������true)
 * @note    �ͼθ�����°������ǽ������дؿ��ϡ�
 *          lvalue()��llvalue() ���дؿ���assign default() ���дؿ���
 *          assign() ���дؿ���convert() ���дؿ��������������дؿ����̡�
 *            
 */
bool mdarray::rounding() const
{
    return this->rounding_rec;
}

/**
 * @brief  ���ߤΥ��������ˡ����������
 *
 *  �����ѥ������ݤ�����ˡ��ɤΤ褦����ˡ�ǹԤʤ��褦���ꤵ��Ƥ��뤫��
 *  �ɤ߼��ޤ���
 *
 * @return  ���ߤΥ��������ˡ "min", "pow", "auto" �Τ����줫
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
 * @brief  �ǽ�μ����ˤĤ��Ƥλ��ꤵ�줿��֤�ѥǥ���
 *
 *  ���Ȥ���������������ֹ� idx �ˡ����ɥ쥹 value_ptr �Ǽ����줿1�Ĥ��ͤ�
 *  len �Ľ񤭹��ߤޤ��������ֹ椪��Ӽ����ֹ�� 0 ����Ϥޤ���ͤǡ�idx �� len
 *  ��Ǥ�դ��ͤ�������Ǥ��ޤ���<br>
 *  �ּ�ư�ꥵ�����⡼�ɡפξ�硤�����λ�����Ф��ƥ��֥��������������Ĺ��
 *  ��­���Ƥ�����ϡ���ưŪ������Υꥵ������Ԥʤ��ޤ������λ����ɲä���
 *  ���ǤΤ����ͤ��񤭹��ޤ�ʤ���ʬ�ϡ��ǥե�����ͤǥѥǥ��󥰤��ޤ���<br>
 *  �ּ�ư�ꥵ�����⡼�ɡפξ��ϡ�idx��len �ǻ��ꤵ�줿��ʬ�Τ���������Ĺ��
 *  �ۤ�����ʬ�ˤĤ��ƤϽ������Ԥʤ��ޤ���
 *
 * @param     value_ptr �ͤؤδ��ܻ���
 * @param     idx �����ֹ�
 * @param     len ���ǤθĿ�
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������(��ư�ꥵ�����⡼�ɤξ��)
 * @throw     �����˲��򵯤��������
 */
mdarray &mdarray::put( const void *value_ptr, ssize_t idx, size_t len )
{
    return this->put(value_ptr, 0, idx, len);
}

/**
 * @brief  Ǥ�դ�1�Ĥμ����ˤĤ��Ƥλ��ꤵ�줿��֤�ѥǥ���
 *
 *  ���Ȥ���������μ����ֹ� dim_index �������ֹ� idx �ˡ����ɥ쥹 value_ptr ��
 *  �����줿1�Ĥ��ͤ� len �Ľ񤭹��ߤޤ��������ֹ椪��Ӽ����ֹ�� 0 ����Ϥޤ�
 *  ���ͤǡ�idx �� len ��Ǥ�դ��ͤ�������Ǥ��ޤ���<br>
 *  �ּ�ư�ꥵ�����⡼�ɡפξ�硤�����λ�����Ф��ƥ��֥��������������Ĺ��
 *  ��­���Ƥ�����ϡ���ưŪ������Υꥵ������Ԥʤ��ޤ������λ����ɲä���
 *  ���ǤΤ����ͤ��񤭹��ޤ�ʤ���ʬ�ϡ��ǥե�����ͤǥѥǥ��󥰤��ޤ���<br>
 *  �ּ�ư�ꥵ�����⡼�ɡפξ��ϡ�idx��len �ǻ��ꤵ�줿��ʬ�Τ���������Ĺ��
 *  �ۤ�����ʬ�ˤĤ��ƤϽ������Ԥʤ��ޤ���<br>
 *  dim_index �� 1 �ʾ�ξ�硤���̼����������Ǥ˽񤭹��ޤ�ޤ���
 *
 * @param     value_ptr �ͤؤδ��ܻ���
 * @param     dim_index �����ֹ�
 * @param     idx �����ֹ�
 * @param     len ���ǤθĿ�
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������(��ư�ꥵ�����⡼�ɤξ��)
 * @throw     �����˲��򵯤��������
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
 * @brief  ���Ȥ���������Ƥ�桼�����Хåե��إ��ԡ� (���ǿ��Ǥλ���)
 *
 *  ���Ȥ���������Ƥ� dest_buf�ǻ��ꤵ�줿�桼�����Хåե��إ��ԡ����ޤ���<br>
 *  �Хåե����礭�� elem_size �����ǤθĿ���Ϳ���ޤ���idx0, idx1, idx2 ��
 *  ���Ȥ�����Ǥγ���������ꤷ�ޤ���<br>
 *  �����ˡ�MDARRAY_INDEF ������Ū��Ϳ���ʤ��Ǥ���������
 *
 * @param     dest_buf �桼�����Хåե��Υ��ɥ쥹 (���ԡ���)
 * @param     elem_size ���ԡ��������ǤθĿ�
 * @param     idx0 �����ֹ� 0 �μ���(1������)�������ֹ� (���ԡ�������ά��)
 * @param     idx1 �����ֹ� 1 �μ���(2������)�������ֹ� (���ԡ�������ά��)
 * @param     idx2 �����ֹ� 2 �μ���(3������)�������ֹ� (���ԡ�������ά��)
 * @return    �桼���ΥХåե�Ĺ����ʬ�ʾ��˥��ԡ���������ǿ�
 * @throw     �����˲��򵯤��������
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
 * @brief  �桼�����Хåե������Ƥ򼫿Ȥ�����إ��ԡ� (���ǿ��Ǥλ���)
 *
 *  src_buf �ǻ��ꤵ�줿�桼�����Хåե������Ƥ򼫿Ȥ�����إ��ԡ����ޤ���<br>
 *  �Хåե����礭�� elem_size �ϡ����ǤθĿ���Ϳ���ޤ���idx0, idx1, idx2 ��
 *  ���Ȥ�����Ǥγ���������ꤷ�ޤ���<br>
 *  �����ˡ�MDARRAY_INDEF ������Ū��Ϳ���ʤ��Ǥ���������
 *
 * @param     src_buf  �桼�����Хåե��Υ��ɥ쥹 (���ԡ���)
 * @param     elem_size ���ԡ��������ǤθĿ�
 * @param     idx0 �����ֹ� 0 �μ���(1������)�������ֹ� (���ԡ��衤��ά��)
 * @param     idx1 �����ֹ� 1 �μ���(2������)�������ֹ� (���ԡ��衤��ά��)
 * @param     idx2 �����ֹ� 2 �μ���(3������)�������ֹ� (���ԡ��衤��ά��)
 * @return    �桼���ΥХåե�Ĺ����ʬ�ʾ��˥��ԡ���������ǿ�
 * @throw     �����˲��򵯤��������
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
 * @brief  ���Ȥ���������Ƥ�桼�����Хåե��إ��ԡ� (�Х��ȿ��Ǥλ���)
 *
 *  ���Ȥ���������Ƥ� dest_buf�ǻ��ꤵ�줿�桼�����Хåե��إ��ԡ����ޤ���<br>
 *  �Хåե����礭�� buf_size �ϡ��Х���ñ�̤�Ϳ���ޤ���idx0, idx1, idx2 ��
 *  ���Ȥ�����Ǥγ���������ꤷ�ޤ���<br>
 *  �����ˡ�MDARRAY_INDEF ������Ū��Ϳ���ʤ��Ǥ���������
 *
 * @param     dest_buf �桼�����Хåե��Υ��ɥ쥹 (���ԡ���)
 * @param     buf_size �Хåե������� (�Х���ñ��)
 * @param     idx0 �����ֹ� 0 �μ���(1������)�������ֹ� (��ά��)
 * @param     idx1 �����ֹ� 1 �μ���(2������)�������ֹ� (��ά��)
 * @param     idx2 �����ֹ� 2 �μ���(3������)�������ֹ� (��ά��)
 * @return    �桼���ΥХåե�Ĺ(buf_size) ����ʬ�ʾ��˥��ԡ������
 *            �Х��ȥ�����
 * @throw     �����˲��򵯤��������
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
 * @brief  �桼�����Хåե������Ƥ򼫿Ȥ�����إ��ԡ� (�Х��ȿ��Ǥλ���)
 *
 *  src_buf �ǻ��ꤵ�줿�桼�����Хåե������Ƥ򼫿Ȥ�����إ��ԡ����ޤ���<br>
 *  �Хåե����礭��buf_size �ϡ��Х���ñ�̤�Ϳ���ޤ���idx0, idx1, idx2 ��
 *  ���Ȥ�����Ǥγ���������ꤷ�ޤ���<br>
 *  �����ˡ�MDARRAY_INDEF ������Ū��Ϳ���ʤ��Ǥ���������
 *
 * @param     src_buf  �桼�����Хåե��Υ��ɥ쥹 (���ԡ���)
 * @param     buf_size �桼�����Хåե��Υ����� (�Х���ñ��)
 * @param     idx0 �����ֹ� 0 �μ���(1������)�������ֹ� (��ά��)
 * @param     idx1 �����ֹ� 1 �μ���(2������)�������ֹ� (��ά��)
 * @param     idx2 �����ֹ� 2 �μ���(3������)�������ֹ� (��ά��)
 * @return    �桼���ΥХåե�Ĺ(buf_size) ����ʬ�ʾ��˥��ԡ������
 *            �Х��ȥ�����
 * @throw     �����˲��򵯤��������
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
 * @brief  ���Ȥ���������Ƥ�桼�����Хåե��إ��ԡ� (SIMD���ȥ꡼��̿�����)
 *
 *  ���Ȥ���������Ƥ� dest_buf �ǻ��ꤵ�줿�桼�����Хåե��إ��ԡ����ޤ���
 *  ���ԡ�������ΰ褬��ʬ���礭����硤SIMD ���ȥ꡼��̿���Ȥäƹ�®��
 *  ���ԡ���Ԥʤ��ޤ���<br>
 *  �Хåե����礭�� buf_size �ϡ��Х���ñ�̤�Ϳ���ޤ���idx, idx1, idx2 ��
 *  ���Ȥ�����Ǥγ���������ꤷ�ޤ���<br>
 *  �����ˡ�MDARRAY_INDEF ������Ū��Ϳ���ʤ��Ǥ���������
 *
 * @param     dest_buf �桼�����Хåե��Υ��ɥ쥹 (���ԡ���)
 * @param     buf_size �Хåե������� (�Х���ñ��)
 * @param     total_copied_size getdata_fast()��ʣ����ƤӽФ�������
 *                              �ȡ�����ǥ��ԡ������Х��ȥ�����
 * @param     idx0 �����ֹ� 0 �μ���(1������)�������ֹ� (��ά��)
 * @param     idx1 �����ֹ� 1 �μ���(2������)�������ֹ� (��ά��)
 * @param     idx2 �����ֹ� 2 �μ���(3������)�������ֹ� (��ά��)
 * @return    �桼���ΥХåե�Ĺ(buf_size) ����ʬ�ʾ��˥��ԡ������
 *            �Х��ȥ�����
 * @throw     �����˲��򵯤��������
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
 * @brief  �桼�����Хåե������Ƥ򼫿Ȥ�����إ��ԡ� (SIMD���ȥ꡼��̿�����)
 *
 *  src_buf �ǻ��ꤵ�줿�桼�����Хåե������Ƥ򼫿Ȥ�����إ��ԡ����ޤ���
 *  ���ԡ�������ΰ褬��ʬ���礭����硤SIMD ���ȥ꡼��̿���Ȥäƹ�®��
 *  ���ԡ���Ԥʤ��ޤ���<br>
 *  �Хåե����礭�� buf_size �ϡ��Х���ñ�̤�Ϳ���ޤ���idx0, idx1, idx2 ��
 *  ���Ȥ�����Ǥγ���������ꤷ�ޤ���<br>
 *  �����ˡ�MDARRAY_INDEF ������Ū��Ϳ���ʤ��Ǥ���������
 *
 * @param     src_buf  �桼�����Хåե��Υ��ɥ쥹 (���ԡ���)
 * @param     buf_size �桼�����Хåե��Υ����� (�Х���ñ��)
 * @param     total_copied_size getdata_fast()��ʣ����ƤӽФ�������
 *                              �ȡ�����ǥ��ԡ������Х��ȥ�����
 * @param     idx0 �����ֹ� 0 �μ���(1������)�������ֹ� (��ά��)
 * @param     idx1 �����ֹ� 1 �μ���(2������)�������ֹ� (��ά��)
 * @param     idx2 �����ֹ� 2 �μ���(3������)�������ֹ� (��ά��)
 * @return    �桼���ΥХåե�Ĺ(buf_size) ����ʬ�ʾ��˥��ԡ������
 *            �Х��ȥ�����
 * @throw     �����˲��򵯤��������
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
 * @brief  ��������1�ĳ�ĥ
 *
 *  ���Ȥ���������μ������� 1�ĳ�ĥ���ޤ���
 *
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
 */
mdarray &mdarray::increase_dim()
{
    size_t sz;
    size_t *srec_ptr;

    if ( this->realloc_size_rec(this->dim_size_rec + 1) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","realloc() failed");
    }
    /* ��ǽ�ʤ��1��ʬ����ݤ��� */
    if ( this->data_ptr() == NULL ) {
	size_t i;
	sz = this->bytes();
	/* this->_size_rec[i] �˥������äƤʤ�����ǧ */
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
    /* update_length_info() ��ľ��˸Ƥ�ɬ�פ����� */
    this->update_arr_ptr_2d();
    this->update_arr_ptr_3d();

    return *this;
}

/**
 * @brief  ��������1�Ľ̾�
 *
 *  ���Ȥ���������μ����� 1�Ľ̾����ޤ���
 *
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
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
    /* update_length_info() ��ľ��˸Ƥ�ɬ�פ����� */
    this->update_arr_ptr_2d();
    this->update_arr_ptr_3d();

    return *this;
}

/**
 * @brief  �ǽ�μ����ˤĤ��Ƥ�����Ĺ���ѹ�
 *
 *  ���Ȥ����������Ĺ�����ѹ����ޤ���<br>
 *  ����Ĺ���ĥ�����硤���Ǥ��ͤϥǥե�����ͤ������ޤ���<br>
 *  ����Ĺ����̤����硤len�ʹߤ����ǤϺ������ޤ���<br>
 *  ��˼����ֹ� 0 �μ���(1������)������оݤȤ��ޤ���
 *
 * @param     len ���ǤθĿ�
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
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
 * @brief  Ǥ�դ�1�Ĥμ����ˤĤ��Ƥ�����Ĺ���ѹ�
 *
 *  ���Ȥ����������Ĺ�����ѹ����ޤ���<br>
 *  ����Ĺ���ĥ�����硤���Ǥ��ͤϥǥե�����ͤ������ޤ���<br>
 *  ����Ĺ����̤����硤len�ʹߤ����ǤϺ������ޤ���<br>
 *  �����ֹ� dim_index �ǽ����оݤȤ��뼡�������Ǥ��ޤ���
 *
 * @param     dim_index �����ֹ�
 * @param     len ���ǤθĿ�
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
 */
mdarray &mdarray::resize( size_t dim_index, size_t len )
{
    return this->do_resize(dim_index, len, this->auto_init());
}

/**
 * @brief  ����Ĺ���ѹ� (¾���֥������Ȥ��饳�ԡ�)
 *
 *  ���Ȥμ�����������Ĺ�򡤥��֥�������src �����Ĥ�Τ�Ʊ���礭���ˤ��ޤ���
 *  ����Ĺ���ĥ�����硤���Ǥ��ͤϥǥե�����ͤ������ޤ���<br>
 *  ����Ĺ����̤����硤����Ĺ�������ʤ���ʬ�����ǤϺ������ޤ���<br>
 *
 * @param     src ����Ĺ�Υ��ԡ���
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
 */
mdarray &mdarray::resize( const mdarray &src )
{
    debug_report("(const mdarray &src)");
    this->resize( src.cdimarray(), src.dim_size_rec, this->auto_init());
    return *this;
}

/**
 * @brief  ����Ĺ���ѹ� (1����)
 *
 *  ���Ȥ����������Ĺ�����ѹ���������Ĺ x_len ��1��������Ȥ��ޤ���
 */
mdarray &mdarray::resize_1d( size_t x_len )
{
    const size_t nx[] = {x_len};
    return this->resize(nx,1,true);
}

/**
 * @brief  ����Ĺ���ѹ� (2����)
 *
 *  ���Ȥ����������Ĺ�����ѹ���������Ĺ (x_len, y_len) ��2��������Ȥ��ޤ���
 */
mdarray &mdarray::resize_2d( size_t x_len, size_t y_len )
{
    const size_t nx[] = {x_len, y_len};
    return this->resize(nx,2,true);
}

/**
 * @brief  ����Ĺ���ѹ� (3����)
 *
 *  ���Ȥ����������Ĺ�����ѹ���������Ĺ (x_len, y_len, z_len) ��3���������
 *  ���ޤ���
 */
mdarray &mdarray::resize_3d( size_t x_len, size_t y_len, size_t z_len )
{
    const size_t nx[] = {x_len, y_len, z_len};
    return this->resize(nx,3,true);
}

/**
 * @brief  �ꥵ�������ɬ�פȤ������ʬ�����Ǥ� �Хåե��������˵ͤ�� (������)
 *
 * @note   ���δؿ��ϼ��� mdarray::resize() ����Ƶ�Ū�˸ƤФ�롥<br>
 *         private �ʴؿ��Ǥ���
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
 * @brief  accumulate_required_data() �ǥХåե��������˽��᤿�ǡ������ʬ��
 *
 * @note  ���δؿ��ϼ��� mdarray::resize() ����Ƶ�Ū�˸ƤФ�롥<br>
 *        private �ʴؿ��Ǥ���
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
 * @brief  ����Ĺ���ѹ� (ʣ���μ���������)
 *
 *  ���Ȥ����������Ĺ�����ѹ����ޤ���<br>
 *  ����Ĺ���ĥ�����硤���Ǥ��ͤ�ǥե�����ͤ���뤫�ɤ����� init_buf ��
 *  ����Ǥ��ޤ���<br>
 *  ����Ĺ����̤����硤����Ĺ�������ʤ���ʬ�����ǤϺ������ޤ���<br>
 *
 * @param     naxisx[] �Ƽ��������ǿ�
 * @param     ndim ���󼡸���
 * @param     init_buf ����Ĺ�γ�ĥ���������ͤ�ǥե�����ͤ�����ʤ� true
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
 * @note      2�����ʾ�Υꥵ�����ξ�硤�㥳���ȤǹԤʤ��ޤ���1���������ꥵ��
 *            ������ʤ顤resize(dim_index, ...) �������㥳���ȤǤ���
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

    /* ��������롥MDARRAY_ALL �����ꤵ�줿���ϸ����ݻ��ˤ��� */
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
	/* ���줾��μ���Ĺ�Ǹ��ߤΤ� naxisx[] �Ȥ���٤ƺǾ��Τ�Τ���¸ */
	size_t n_min_naxisx;
	size_t i, this_blen_all, min_blen_all;
	const unsigned char *back_p0 = (const unsigned char *)(this->data_ptr());
	unsigned char *front_p0 = (unsigned char *)(this->data_ptr());
	unsigned char *back_p1;
	const unsigned char *front_p1;
	bool eq_flag = (this->dim_size_rec == ndim);
	/* ���������μ����� */
	if ( ndim < this->dim_size_rec ) n_min_naxisx = ndim;
	else n_min_naxisx = this->dim_size_rec;
	/* �Ƽ���������Ĺ�ξ���������ȤäƤ��� */
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
	/* ����Ĺ���Ƽ���������Ĺ�����٤�Ʊ���ʤ齪λ */
	if ( eq_flag == true ) goto quit;
	/* �ꥵ�������ɬ�פ���ʬ�Υǡ����Τߡ����٤ƥХåե��������˽���� */
	accumulate_required_data(bytes, this->cdimarray(), min_naxisx.ptr(), 
				 n_min_naxisx - 1, this_blen_all,
				 &back_p0, &front_p0);
	/* �Хåե��Υꥵ���� */
	this->reallocate(naxisx.ptr(), ndim, false);
	/* ���줾��ΰ��ֺǸ� + 1 */
	back_p1 = (unsigned char *)(this->data_ptr()) + this->byte_length();
	front_p1 = (const unsigned char *)(this->data_ptr()) + min_blen_all;
	/* �Хåե��������˽��᤿�ǡ��������ΰ��֤˺����֤��� */
	distribute_required_data(bytes, this->cdimarray(), min_naxisx.ptr(), 
				 n_min_naxisx, this->dim_size_rec - 1, 
				 this->length(), &back_p1, &front_p1, 
				 init_buf, this->default_value_ptr());
    }

 quit:
    return *this;
}

/**
 * @brief  "10,10" �Τ褦��ʸ���󤫤�����Ĺ�������� (������)
 *
 * @note  resizef(), vresizef() �ǻ��ѡ�<br>
 *        private �ʴؿ��Ǥ���
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
	else sizeinfo[n_el] = obj.length(n_el);		/* Ĺ���ѹ����� */
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
 * @brief  ����Ĺ���ѹ� (IDL�ε�ˡ)
 *
 *  ���Ȥ����������Ĺ�����ѹ����ޤ���Ĺ���λ���ϡ�printf() �����β���Ĺ������
 *  �����ǽ�ʥ���޶��ڤ��ʸ����(�㤨��"10,5")�ˤ�äƹԤʤ��ޤ���<br>
 *  ���ꤵ�줿ʸ����������Ǥ������ǤϤʤ���硤���μ�����Ĺ�����ѹ����ޤ���
 *  �㤨�С� ",2" �����ꤵ�줿��硤�ǽ�μ�����Ĺ�����ѹ�����ޤ���
 *
 * @param   exp_fmt csv������Ĺ������ʸ����Τ���Υե����ޥåȻ���
 * @param   ...     exp_fmt���б���������Ĺ�����γ����ǥǡ���
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
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
 * @brief  ����Ĺ���ѹ� (IDL�ε�ˡ)
 *
 *  ���Ȥ����������Ĺ�����ѹ����ޤ���Ĺ���λ���ϡ�printf() �����β���Ĺ������
 *  �����ǽ�ʥ���޶��ڤ��ʸ����(�㤨��"10,5")�ˤ�äƹԤʤ��ޤ���<br>
 *  ���ꤵ�줿ʸ����������Ǥ������ǤϤʤ���硤���μ�����Ĺ�����ѹ����ޤ���
 *  �㤨�С� ",2" �����ꤵ�줿��硤�ǽ�μ�����Ĺ�����ѹ�����ޤ���
 *
 * @param   exp_fmt csv������Ĺ������ʸ����Τ���Υե����ޥåȻ���
 * @param   ap      exp_fmt���б���������Ĺ�����Υꥹ��
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
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
 * @brief  �ǽ�μ����ˤĤ�������Ĺ������Ū���ѹ�
 *
 *  ���Ȥ����������Ĺ���� len �λ���ʬ����ĥ���̾����ޤ���<br>
 *  resizeby() ������󥵥����ϡ����������Ĺ���� len ��ä�����ΤȤʤ�ޤ���
 *  �������ν̾��ϡ�len �˥ޥ��ʥ��ͤ���ꤹ�뤳�Ȥˤ�äƹԤ��ޤ���<br>
 *  ��˼����ֹ� 0 �μ���(1������)������оݤȤ��ޤ���
 *
 * @param     len ���ǸĿ�����ʬ����ʬ
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
 */
mdarray &mdarray::resizeby( ssize_t len )
{
    return this->resizeby(0, len);
}

/**
 * @brief  Ǥ�դ�1�Ĥμ����ˤĤ�������Ĺ������Ū���ѹ�
 *
 *  ���Ȥ����������Ĺ���� len �λ���ʬ����ĥ���̾����ޤ���<br>
 *  resizeby() ������󥵥����ϡ����������Ĺ���� len ��ä�����ΤȤʤ�ޤ���
 *  �������ν̾��ϡ�len �˥ޥ��ʥ��ͤ���ꤹ�뤳�Ȥˤ�äƹԤ��ޤ���<br>
 *  �����ֹ� dim_index �ǽ����оݤȤ��뼡�������Ǥ��ޤ���
 *
 * @param     dim_index �����ֹ�
 * @param     len ���ǸĿ�����ʬ����ʬ
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
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
 * @brief  ����Ĺ������Ū���ѹ� (1������)
 */
mdarray &mdarray::resizeby_1d( ssize_t x_len )
{
    const ssize_t nx[] = {x_len};
    return this->resizeby(nx, 1, true);
}

/* change the length of the 2-d array relatively */
/**
 * @brief  ����Ĺ������Ū���ѹ� (2������)
 */
mdarray &mdarray::resizeby_2d( ssize_t x_len, ssize_t y_len )
{
    const ssize_t nx[] = {x_len, y_len};
    return this->resizeby(nx, 2, true);
}

/* change the length of the 3-d array relatively */
/**
 * @brief  ����Ĺ������Ū���ѹ� (3������)
 */
mdarray &mdarray::resizeby_3d(ssize_t x_len, ssize_t y_len, ssize_t z_len)
{
    const ssize_t nx[] = {x_len, y_len, z_len};
    return this->resizeby(nx, 3, true);
}

/**
 * @brief  ����Ĺ������Ū���ѹ� (ʣ���μ���������)
 *
 *  ���Ȥ����������Ĺ���� naxisx_rel[] �λ���ʬ����ĥ���̾����ޤ���<br>
 *  resizeby() ������󥵥����ϡ����������Ĺ���� naxisx_rel[] ��ä������
 *  �Ȥʤ�ޤ����������ν̾��ϡ�naxisx_rel[] �˥ޥ��ʥ��ͤ���ꤹ�뤳�Ȥˤ�ä�
 *  �Ԥ��ޤ���<br>
 *  init_buf �ǥ�������ĥ���˿����˺�����������Ǥν�����򤹤뤫�ɤ�����
 *  ����Ǥ��ޤ���
 *
 * @param     naxisx_rel ���ǸĿ�����ʬ����ʬ
 * @param     ndim naxisx_rel[] �θĿ�
 * @param     init_buf �����˺�����������Ǥν������Ԥʤ����� true
 * @return    ���Ȥλ���
 * @note      ����Ĺ�����䤹���ϤǤ��ޤ��������餹���ϤǤ��ޤ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
 */
mdarray &mdarray::resizeby( const ssize_t naxisx_rel[], size_t ndim, 
			    bool init_buf )
{
    heap_mem<size_t> n_axisx;
    size_t new_ndim = this->dim_size_rec;	/* ���ߤμ����� */
    size_t i;

    if ( 0 < ndim && naxisx_rel == NULL ) {
	err_throw(__FUNCTION__,"ERROR","0 < ndim, but NULL naxisx arg");
    }

    if ( new_ndim < ndim ) new_ndim = ndim;	/* ���������������� */

    if ( 0 < new_ndim ) {
	if ( n_axisx.allocate(new_ndim) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","malloc() failed");
	}
    }

    for ( i=0 ; i < new_ndim ; i++ ) {
	size_t len = this->length(i);		/* length() ��Ǥ���� */
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
 * @brief  "10,-10" �Τ褦��ʸ���󤫤���������Ĺ�������� (������)
 *
 * @note  resizebyf(), vresizebyf() �ǻ��ѡ�<br>
 *        private �ʴؿ��Ǥ���
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
	else sizeinfo[n_el] = 0;	/* �ѹ����ʤ����ϥ��� */
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
 * @brief  ����Ĺ������Ū���ѹ� (IDL�ε�ˡ)
 *
 *  ���Ȥ����������Ĺ�������ʬ����ĥ���̾����ޤ���Ĺ���λ���ϡ�printf() ����
 *  �β���Ĺ�����ǻ����ǽ�ʥ���޶��ڤ��ʸ����(�㤨��"10,-5")�ˤ�äƹԤʤ�
 *  �ޤ���<br>
 *  resizebyf()������󥵥����ϡ����������Ĺ���˻���ʬ��ä�����ΤȤʤ�ޤ���
 *  �������ν̾��ϡ��ޥ��ʥ��ͤ���ꤹ�뤳�Ȥˤ�äƹԤ��ޤ���<br>
 *  ���ꤵ�줿ʸ����������Ǥ������ǤϤʤ���硤���μ�����Ĺ�����ѹ����ޤ���
 *  �㤨�С� ",2" �����ꤵ�줿��硤�ǽ�μ�����Ĺ�����ѹ�����ޤ���
 *
 * @param   exp_fmt csv������Ĺ������ʸ����Τ���Υե����ޥåȻ���
 * @param   ...     exp_fmt���б���������Ĺ�����γ����ǥǡ���
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
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
 * @brief  ����Ĺ������Ū���ѹ� (IDL�ε�ˡ)
 *
 *  ���Ȥ����������Ĺ�������ʬ����ĥ���̾����ޤ���Ĺ���λ���ϡ�printf() ����
 *  �β���Ĺ�����ǻ����ǽ�ʥ���޶��ڤ��ʸ����(�㤨��"10,-5")�ˤ�äƹԤʤ�
 *  �ޤ���<br>
 *  resizebyf()������󥵥����ϡ����������Ĺ���˻���ʬ��ä�����ΤȤʤ�ޤ���
 *  �������ν̾��ϡ��ޥ��ʥ��ͤ���ꤹ�뤳�Ȥˤ�äƹԤ��ޤ���<br>
 *  ���ꤵ�줿ʸ����������Ǥ������ǤϤʤ���硤���μ�����Ĺ�����ѹ����ޤ���
 *  �㤨�С� ",2" �����ꤵ�줿��硤�ǽ�μ�����Ĺ�����ѹ�����ޤ���
 *
 * @param   exp_fmt csv������Ĺ������ʸ����Τ���Υե����ޥåȻ���
 * @param   ap      exp_fmt���б���������Ĺ�����Υꥹ��
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
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
 * @brief  ����ΥХåե�����֤�Ĵ���򤻤��ˡ������ѥХåե���Ƴ���
 *
 *  ����ΥХåե�����֤�Ĵ���򤻤��ˡ����Ȥ����ļ������礭���������Ĺ����
 *  �ѹ����ޤ����Ĥޤꡤ�����ѥХåե����Ф��Ƥ�ñ���realloc()��Ƥ֤����ν���
 *  ��Ԥʤ��ޤ���
 *
 * @param   naxisx[] �Ƽ��������ǿ�
 * @param   ndim ���󼡸���
 * @param   init_buf �����˳��ݤ�����ʬ�������ͤ�ǥե�����ͤ�����ʤ� true
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
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

    /* ������������Ĺ������å� */
    if ( ndim == 0 ) len = 0;
    else {
	len = 1;
	for ( i=0 ; i < ndim ; i++ ) {
	    /* ���ǿ�0����� */
	    if ( naxisx[i] == MDARRAY_ALL ) len *= this->length(i);
	    else len *= naxisx[i];
	}
    }
    /* �����ѥХåե�Ĺ���礭���ʤ��硤��� realloc ���Ƥ��� */
    if ( old_len < len ) {
	if ( this->realloc_arr_rec(bytes * len) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","realloc() failed");
	}
	if ( this->extptr_rec != NULL ) *(this->extptr_rec) = this->data_ptr();
	/* ɬ�פʾ���������� */
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
    /* ������������Ѥ�����ι��� */
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
    /* �����ѥХåե�Ĺ���������ʤ����realloc */
    if ( len < old_len ) {
	if ( this->realloc_arr_rec(bytes * len) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","realloc() failed");
	}
	if ( this->extptr_rec != NULL ) *(this->extptr_rec) = this->data_ptr();
    }

    /* update cached length info and call user's callback func */
    this->update_length_info();
    /* update_length_info() ��ľ��˸Ƥ�ɬ�פ����� */
    this->update_arr_ptr_2d();
    this->update_arr_ptr_3d();

    return *this;
}

/* free current buffer and alloc new memory */
/**
 * @brief  �����ѥХåե����ö�������������˳���
 *
 *  ��������Ƥ��ö�˴����������Ĺ�����ѹ����ޤ����Ĥޤꡤ�����ѥХåե���
 *  �Ф��� free()��malloc() ��Ƥ֤����ν�����Ԥʤ��ޤ���
 *
 * @param   naxisx[] �Ƽ��������ǿ�
 * @param   ndim ���󼡸���
 * @param   init_buf �����˳��ݤ�����ʬ�������ͤ�ǥե�����ͤ�����ʤ� true
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
mdarray &mdarray::allocate( const size_t naxisx[], size_t ndim, 
			    bool init_buf )
{
    const size_t bytes = zt2bytes(this->sz_type_rec);
    size_t i, len;

    if ( 0 < ndim && naxisx == NULL ) {
	err_throw(__FUNCTION__,"ERROR","0 < ndim, but NULL naxisx arg");
    }

    /* ������������Ĺ������å� */
    if ( ndim == 0 ) len = 0;
    else {
	len = 1;
	for ( i=0 ; i < ndim ; i++ ) {
	    /* ���ǿ�0����� */
	    if ( naxisx[i] == MDARRAY_ALL ) len *= this->length(i);
	    else len *= naxisx[i];
	}
    }

    /* ���� */
    if ( this->realloc_arr_rec(0) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","realloc() failed");
    }
    if ( this->realloc_size_rec(0) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","realloc() failed");
    }
    this->dim_size_rec = 0;

    /* ���� */
    if ( this->realloc_size_rec(ndim) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","realloc() failed");
    }
    if ( this->realloc_arr_rec(bytes * len) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","realloc() failed");
    }
    if ( this->extptr_rec != NULL ) *(this->extptr_rec) = this->data_ptr();

    /* ������������Ѥ�����ι��� */
    this->dim_size_rec = ndim;
    for ( i=0 ; i < ndim ; i++ ) {
	size_t *srec_ptr = this->size_rec_ptr();
	srec_ptr[i] = naxisx[i];
    }

    /* ɬ�פʾ���������� */
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
    /* update_length_info() ��ľ��˸Ƥ�ɬ�פ����� */
    this->update_arr_ptr_2d();
    this->update_arr_ptr_3d();

    return *this;
}

/**
 * @brief  ��������ĥ���ν���ͤ�����
 *
 *  ���󥵥�����ĥ���ν���ͤ����ꤷ�ޤ������ꤵ�줿�ͤϴ�¸�����ǤˤϺ���
 *  ��������������ĥ����ͭ���Ȥʤ�ޤ���
 *
 * @param     value ���󥵥�����ĥ���ν����
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
 */
mdarray &mdarray::assign_default( double value )
{
    void (*func_d2dest)(const void *,void *,size_t,int,void *);
    struct mdarray_cnv_nd_prms prms;

    prms.total_len_elements = 0;			/* �����ʤ� */
    prms.round_flg = this->rounding();			/* �ͼθ���/�ڤ�Τ� */

    /* ���Ѵ��Τ���δؿ� (double�����Ȥη�) */
    func_d2dest = this->func_cnv_nd_d2x;

    if ( func_d2dest == NULL ) {
	err_report(__FUNCTION__,"WARNING","invalid sz_type; ignored");
	return *this;
    }

    (*func_d2dest)(&value, this->junk_rec, 1, +1, (void *)&prms);

    return this->assign_default(this->junk_rec);
}

/**
 * @brief  ��������ĥ���ν���ͤ����� (���٥�)
 *
 *  ���󥵥�����ĥ���ν���ͤ����ꤷ�ޤ������ꤵ�줿�ͤϴ�¸�����ǤˤϺ���
 *  ��������������ĥ����ͭ���Ȥʤ�ޤ���
 *
 * @param     value_ptr ���󥵥�����ĥ���ν���ͤΥ��ɥ쥹
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
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
 * @brief  �ǽ�μ����ˤĤ��ƿ����������Ǥ�����
 *
 *  ���Ȥ���������ǰ��� idx �ˡ�len ��ʬ�����Ǥ��������ޤ����ʤ������������
 *  ���Ǥ��ͤϥǥե�����ͤǤ���<br>
 *  ��˼����ֹ� 0 �μ���(1������)������оݤȤ��ޤ���
 *
 * @param     idx �������֤������ֹ�
 * @param     len ���ǤθĿ�
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
 */
mdarray &mdarray::insert( ssize_t idx, size_t len )
{
    return this->insert(0,idx,len);
}

/* this->dim_size_rec <= dim_index �ξ�硤this->resize() �ˤ�ä�
   ��ưŪ�˼������ɲä���� */
/**
 * @brief  Ǥ�դ�1�Ĥμ����ˤĤ��ƿ����������Ǥ�����
 *
 *  ���Ȥ���������ǰ��� idx �ˡ�len ��ʬ�����Ǥ��������ޤ����ʤ������������
 *  ���Ǥ��ͤϥǥե�����ͤǤ���<br>
 *  �����ֹ� dim_index �ǽ����оݤȤ��뼡�������Ǥ��ޤ���
 *
 * @param     dim_index �����ֹ�
 * @param     idx �������֤������ֹ�
 * @param     len ���ǤθĿ�
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
 */
mdarray &mdarray::insert( size_t dim_index, ssize_t idx, size_t len )
{
    return this->do_insert(dim_index, idx, len, this->auto_init());
}

/**
 * @brief  �ǽ�μ����ˤĤ����������Ǥκ��
 *
 *  ���Ȥ����󤫤���ꤵ�줿��ʬ�����Ǥ������ޤ����������ʬ��Ĺ����û���ʤ�
 *  �ޤ���<br>
 *  ��˼����ֹ� 0 �μ���(1������)������оݤȤ��ޤ�.
 *
 * @param     idx ���ϰ��֤������ֹ�
 * @param     len ���ǤθĿ�
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
 */
mdarray &mdarray::erase( ssize_t idx, size_t len )
{
    this->erase(0, idx, len);
    if ( this->dim_size_rec == 1 && this->_size_rec[0] == 0 ) {
	this->decrease_dim();
    }
    return *this;
}

/* this->dim_size_rec <= dim_index �ξ�硤0 < dim_index �ʤ�
   this->resize() �ˤ�äƼ�ưŪ�˼������ɲä���� */
/**
 * @brief  Ǥ�դ�1�Ĥμ����ˤĤ����������Ǥκ��
 *
 *  ���Ȥ����󤫤���ꤵ�줿��ʬ�����Ǥ������ޤ����������ʬ��Ĺ����û���ʤ�
 *  �ޤ���<br>
 *  �����ֹ� dim_index �ǽ����оݤȤ��뼡�������Ǥ��ޤ���
 *
 * @param     dim_index �����ֹ�
 * @param     idx ���ϰ��֤������ֹ�
 * @param     len ���ǤθĿ�
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
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
 * @brief  �ǽ�μ����ˤĤ����������Ǵ֤Ǥ��ͤΥ��ԡ�
 *
 * ���Ȥ��������Ǵ֤��ͤ򥳥ԡ����ޤ���<br>
 * ���� clr �� false �����ꤵ�줿���ϥ��ԡ������ͤϻĤ�ޤ�����true ������
 * ���줿���ϥ��ԡ������ͤϻĤ餺�ǥե�����ͤ������ޤ���<br>
 * idx_dst �˴�¸������Ĺ����礭���ͤ����ꤷ�Ƥ⡤���󥵥������Ѥ��ޤ���
 * ������������ cpy() ���дؿ��Ȥϰۤʤ�ޤ���<br>
 * ��˼����ֹ� 0 �μ���(1������)������оݤȤ��ޤ���
 *
 * @param     idx_src ���ԡ����������ֹ�
 * @param     len ���ԡ��������Ǥ�Ĺ��
 * @param     idx_dst ���ԡ���������ֹ�
 * @param     clr ���ԡ������ͤΥ��ꥢ����
 * @return    ���Ȥλ���
 */
mdarray &mdarray::move( ssize_t idx_src, size_t len, ssize_t idx_dst, bool clr )
{
    return this->move(0, idx_src, len, idx_dst, clr);
}

/**
 * @brief  Ǥ�դ�1�Ĥμ����ˤĤ����������Ǵ֤Ǥ��ͤΥ��ԡ�
 *
 *  ���Ȥ��������Ǵ֤��ͤ򥳥ԡ����ޤ���<br>
 *  ���� clr �� false �����ꤵ�줿���ϥ��ԡ������ͤϻĤ�ޤ�����true ������
 *  ���줿���ϥ��ԡ������ͤϻĤ餺�ǥե�����ͤ������ޤ���<br>
 *  idx_dst �˴�¸������Ĺ����礭���ͤ����ꤷ�Ƥ⡤���󥵥������Ѥ��ޤ���
 *  ������������ cpy() ���дؿ��Ȥϰۤʤ�ޤ���<br>
 *  �����ֹ� dim_index �ǽ����оݤȤ��뼡�������Ǥ��ޤ���
 *
 * @param     dim_index �����ֹ�
 * @param     idx_src ���ԡ����������ֹ�
 * @param     len ���ԡ��������Ǥ�Ĺ��
 * @param     idx_dst ���ԡ���������ֹ�
 * @param     clr ���ԡ������ͤΥ��ꥢ����
 * @return    ���Ȥλ���
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
 * @brief  �ǽ�μ����ˤĤ����������Ǵ֤Ǥ��ͤΥ��ԡ� (������ϼ�ư��ĥ)
 *
 *  ���Ȥ��������Ǵ֤��ͤ򥳥ԡ����ޤ���<br>
 *  ���� clr �� false �����ꤵ�줿���ϥ��ԡ������ͤϻĤ�ޤ�����true ������
 *  ���줿���ϥ��ԡ������ͤϻĤ餺�ǥե�����ͤ������ޤ���<br>
 *  idx_dst + len ����¸������Ĺ����礭����硤���󥵥����ϼ�ư��ĥ�����
 *  ����<br>
 *  ��˼����ֹ� 0 �μ���(1������)������оݤȤ��ޤ���
 *
 * @param     idx_src ���ԡ����������ֹ�
 * @param     len ���ԡ��������Ǥ�Ĺ��
 * @param     idx_dst ���ԡ���������ֹ�
 * @param     clr ���ԡ������ͤΥ��ꥢ����
 * @return    ���Ȥλ���
 */
mdarray &mdarray::cpy( ssize_t idx_src, size_t len, ssize_t idx_dst, bool clr )
{
    return this->cpy(0, idx_src, len, idx_dst, clr);
}

/**
 * @brief  Ǥ�դ�1�Ĥμ����ˤĤ����������Ǵ֤Ǥ��ͤΥ��ԡ� (������ϼ�ư��ĥ)
 *
 *  ���Ȥ��������Ǵ֤��ͤ򥳥ԡ����ޤ���<br>
 *  ���� clr �� false �����ꤵ�줿���ϥ��ԡ������ͤϻĤ�ޤ�����true ������
 *  ���줿���ϥ��ԡ������ͤϻĤ餺�ǥե�����ͤ������ޤ���<br>
 *  idx_dst + len ����¸������Ĺ����礭����硤���󥵥����ϼ�ư��ĥ�����
 *  ����<br>
 *  �����ֹ� dim_index �ǽ����оݤȤ��뼡�������Ǥ��ޤ���
 *
 * @param     dim_index �����ֹ�
 * @param     idx_src ���ԡ����������ֹ�
 * @param     len ���ԡ��������Ǥ�Ĺ��
 * @param     idx_dst ���ԡ���������ֹ�
 * @param     clr ���ԡ������ͤΥ��ꥢ����
 * @return    ���Ȥλ���
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
 * @brief  �ǽ�μ����ˤĤ����������Ǵ֤Ǥ��ͤ����촹��
 *
 *  ���Ȥ��������Ǵ֤��ͤ������ؤ��ޤ���<br>
 *  �����ֹ� idx_src ���� len ��ʬ�����Ǥ������ֹ� idx_dst ���� len ��ʬ����
 *  �Ǥ������ؤ��ޤ���<br>
 *  idx_dst + len �����󥵥�����Ķ������ϡ����󥵥����ޤǤν������Ԥ���
 *  ���������ؤ����ΰ褬�Ťʤä���硤�ŤʤäƤ��ʤ� src ���ΰ���Ф��ƤΤ���
 *  ���ؤ��������Ԥ��ޤ���
 *
 * @param     idx_src �����ؤ����������ֹ�
 * @param     len �����ؤ��������Ǥ�Ĺ��
 * @param     idx_dst �����ؤ���������ֹ�
 * @return    ���Ȥλ���
 */
mdarray &mdarray::swap( ssize_t idx_src, size_t len, ssize_t idx_dst )
{
    return this->swap(0, idx_src, len, idx_dst);
}

/**
 * @brief  Ǥ�դ�1�Ĥμ����ˤĤ����������Ǵ֤Ǥ��ͤ����촹��
 *
 *  ���Ȥ��������Ǵ֤��ͤ������ؤ��ޤ���<br>
 *  �����ֹ� dim_index �������ֹ� idx_src ���� len ��ʬ�����Ǥ������ֹ� 
 *  idx_dst ���� len ��ʬ�����Ǥ������ؤ��ޤ���<br>
 *  idx_dst + len �����󥵥�����Ķ������ϡ����󥵥����ޤǤν������Ԥ���
 *  ���������ؤ����ΰ褬�Ťʤä���硤�ŤʤäƤ��ʤ� src ���ΰ���Ф��ƤΤ���
 *  ���ؤ��������Ԥ��ޤ���
 *
 * @param     dim_index �����ֹ�
 * @param     idx_src �����ؤ����������ֹ�
 * @param     len �����ؤ��������Ǥ�Ĺ��
 * @param     idx_dst �����ؤ���������ֹ�
 * @return    ���Ȥλ���
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

    /* �ΰ褬�Ťʤ�ʤ��褦�ˤ��� */
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
 * @brief  �ǽ�μ����ˤĤ��������������ʬ�ξõ�
 *
 *  ���Ȥ���������ǰ��� idx ���� len �Ĥ����Ǥ����ˤ��ޤ���<br>
 *  ��˼����ֹ� 0 �μ���(1������)������оݤȤ��ޤ���
 *
 * @param     idx �ڤ�Ф����ϰ��֤������ֹ�
 * @param     len ���ǤθĿ�
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
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
 * @brief  Ǥ�դ�1�Ĥμ����ˤĤ��������������ʬ�ξõ�
 *
 *  ���Ȥ���������ǰ��� idx ���� len �Ĥ����Ǥ����ˤ��ޤ���<br>
 *  �����ֹ� dim_index �ǽ����оݤȤ��뼡�������Ǥ��ޤ���
 *
 * @param     dim_index �����ֹ�
 * @param     idx �ڤ�Ф����ϰ��֤������ֹ�
 * @param     len ���ǤθĿ�
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
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
 * @brief  �ǽ�μ����ˤĤ���������¤Ӥ�ȿž
 *
 *  ���Ȥ���������ǰ��� idx ���� len �Ĥ����Ǥ�ȿž�����ޤ���<br>
 *  ��˼����ֹ� 0 �μ���(1������)������оݤȤ��ޤ���
 *
 * @param     idx ���ϰ��֤������ֹ�
 * @param     len ���ǤθĿ�
 * @return    ���Ȥλ���
 */
mdarray &mdarray::flip( ssize_t idx, size_t len )		/* for 1-d */
{
    this->flip(0, idx, len);
    return *this;
}

/**
 * @brief  Ǥ�դ�1�Ĥμ����ˤĤ���������¤Ӥ�ȿž
 *
 *  ���Ȥ���������ǰ��� idx ���� len �Ĥ����Ǥ�ȿž�����ޤ���<br>
 *  �����ֹ� dim_index �ǽ����оݤȤ��뼡�������Ǥ��ޤ���
 *
 * @param     dim_index �����ֹ�
 * @param     idx ���ϰ��֤������ֹ�
 * @param     len ���ǤθĿ�
 * @return    ���Ȥλ���
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
 * @brief  ��ư�����������������ͤ��ڤ�夲
 *
 *  ���Ȥ�����(��ư��������)�������Ǥξ��������ͤ��ڤ�夲�ޤ���
 *
 * @return     ���Ȥλ���
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
 * @brief  ��ư�����������������ͤξ��������ڤ겼��
 *
 *  ���Ȥ�����(��ư��������)�������Ǥξ������򡤤��줾������Ǥ��ͤ�ۤ��ʤ�
 *  ����������ͤ��ڤ겼���ޤ���
 *
 * @return     ���Ȥλ���
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
 * @brief  ��ư�����������������ͤλͼθ���
 *
 *  ���Ȥ�����(��ư��������)���������ͤξ�������ͼθ������������ͤˤ��ޤ���
 *
 * @return     ���Ȥλ���
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
 * @brief  ��ư�����������������ͤξ��������ڤ�Τ�
 *
 *  ���Ȥ�����(��ư��������)�������Ǥ��ͤξ��������ڤ�Τơ�0 �˶ᤤ����������
 *  �ˤ��ޤ���
 *
 * @return     ���Ȥλ���
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
 * @brief  �������ͤ��Ф������ͤ�Ȥ�
 *
 *  ���Ȥ�����������ǤˤĤ��������ͤ�Ȥ�ޤ���<br>
 *  ʣ�ǿ��ξ��ϡ��¿������������Τ��줾��ˤĤ��������ͤ�Ȥ�ޤ���
 *
 * @note �桼��������Ū��ʣ�ǿ��������ͤ��������뤫�⤷��ʤ��Τ�ʣ�ǿ��� abs()
 *       ���б��������㳰�������ɤ����⤷��ʤ�
 *
 * @return     ���Ȥλ���
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
    /* �桼��������Ū��ʣ�ǿ��������ͤ��������뤫�⤷��ʤ��Τ� */
    /* ʣ�ǿ��� abs() ���б��������㳰�������ɤ����⤷��ʤ�    */
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
 * @brief  "0:99,*" "[1:100,*]" �Τ褦��ʸ���󤫤饻�������������� (������)
 *
 * @note  cleanf(), copyf() ���ǻ��ѡ�<br>
 *        private �ʴؿ��Ǥ���
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
 * @brief  sectionf() ���ΰ�����Ĵ�٤�Ŭ�ڤʺ�ɸ�Υѥ�᡼�������� (���٥�)
 *
 * secinfo ������å��������������ΤϤ߽Ф���������н������롥<br>
 * ���֥������Ȥμ���������٤� secinfo �Τ��줬­��ʤ������ɲä��롥
 *
 * @note    private �ʴؿ��Ǥ���
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
	/* �����ΰ褬�Ϥ߽Ф��Ƥ�����ν��� */
	if ( secinfo[i].begin < 0 ) {
	    if ( secinfo[i].begin + secinfo[i].length <= 0 ) {
		/* �ɤ�٤�Ĺ���ϥ��� */
		secinfo[i].length = 0;
	    }
	    else {
		/* �ɤ�٤�Ĺ���򾮤������� */
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
	/* �⤷ 0 �ʤ� �ٹ��Ф��� return */
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
	/* �ϰϤ��������줿��� */
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
 * @brief  �����黻�Ѥδؿ� (���ߤϤۤ�̤����)
 *
 * @note �ޥ���Ÿ���ˤ���®�����ɤ�ư���Τǡ����ߤϤۤȤ�ɻȤ�����Ϥʤ���
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
 * @brief  �����黻�Ѥδؿ� (���ߤϤۤ�̤����)
 *
 * @note �ޥ���Ÿ���ˤ���®�����ɤ�ư���Τǡ����ߤϤۤȤ�ɻȤ�����Ϥʤ���
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
 * @brief  �����黻�Ѥδؿ� (���ߤϤۤ�̤����)
 *
 * @note �ޥ���Ÿ���ˤ���®�����ɤ�ư���Τǡ����ߤϤۤȤ�ɻȤ�����Ϥʤ���
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
 * @brief  �����黻�Ѥδؿ� (���ߤϤۤ�̤����)
 *
 * @note �ޥ���Ÿ���ˤ���®�����ɤ�ư���Τǡ����ߤϤۤȤ�ɻȤ�����Ϥʤ���
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
 * @brief  �����黻�Ѥδؿ� (���ߤϤۤ�̤����)
 *
 * @note �ޥ���Ÿ���ˤ���®�����ɤ�ư���Τǡ����ߤϤۤȤ�ɻȤ�����Ϥʤ���
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
 * @brief  �����黻�Ѥδؿ� (���ߤϤۤ�̤����)
 *
 * @note �ޥ���Ÿ���ˤ���®�����ɤ�ư���Τǡ����ߤϤۤȤ�ɻȤ�����Ϥʤ���
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
 * @brief  �����黻�Ѥδؿ� (���ߤϤۤ�̤����)
 *
 * @note �ޥ���Ÿ���ˤ���®�����ɤ�ư���Τǡ����ߤϤۤȤ�ɻȤ�����Ϥʤ���
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
 * @brief  �����黻�Ѥδؿ� (���ߤϤۤ�̤����)
 *
 * @note �ޥ���Ÿ���ˤ���®�����ɤ�ư���Τǡ����ߤϤۤȤ�ɻȤ�����Ϥʤ���
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
    /* ���饤���Ȥ����(sizeof��Ĺ����Τ�����!) */
    int ope;
    bool round_flg;
};

/* �����Ȳ����α黻����                                        */
/* �����餫����餫����������ޤäƤ����硥                */
/* f_b = -1 �Ǹ���顤f_b = 1 �������顤f_b = 0 �ǥ����顼�� */
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
 * @brief  �������Ǥ��ڤ�Ф��������ƥ�ݥ�ꥪ�֥������ȤǼ��� (IDL�ε�ˡ)
 *
 * �����ǡ��������Υ��дؿ��ǡ����Ȥ�����Τ������ꤵ�줿��ʬ������ƥ�ݥ��
 * ���֥������Ȥ˥��ԡ�����������֤��ޤ���<br>
 * ���Ф���뤵����ϰϤ�Ǥ�դμ����ޤǻ����ǽ�� "0:99,*" �Τ褦�� IDL ��
 * IRAF ��Ʊ�ͤ�ʸ����ǻ��ꤷ�ޤ�������ʸ������������ֹ�� 0-indexed �Ǥ���
 *
 * @param   exp_fmt  csv�������ϰϻ���ʸ����Τ���Υե����ޥåȻ���
 * @param   ...      exp_fmt���б���������Ĺ�����γ����ǥǡ���
 * @return  �ƥ�ݥ�ꥪ�֥�������
 * @note    "[1:100,*]" �Τ褦�� [] �ǰϤ�ȡ�1-indexed �Ȥ��ư����ޤ���
 * @throw   �Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
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
 * @brief  �������Ǥ��ڤ�Ф��������ƥ�ݥ�ꥪ�֥������ȤǼ��� (IDL�ε�ˡ)
 *
 * �����ǡ��������Υ��дؿ��ǡ����Ȥ�����Τ������ꤵ�줿��ʬ������ƥ�ݥ��
 * ���֥������Ȥ˥��ԡ�����������֤��ޤ���<br>
 * ���Ф���뤵����ϰϤ�Ǥ�դμ����ޤǻ����ǽ�� "0:99,*" �Τ褦�� IDL ��
 * IRAF ��Ʊ�ͤ�ʸ����ǻ��ꤷ�ޤ�������ʸ������������ֹ�� 0-indexed �Ǥ���
 *
 * @param   exp_fmt  csv�������ϰϻ���ʸ����Τ���Υե����ޥåȻ���
 * @param   ap       exp_fmt���б���������Ĺ�����Υꥹ��
 * @return  �ƥ�ݥ�ꥪ�֥�������
 * @note    "[1:100,*]" �Τ褦�� [] �ǰϤ�ȡ�1-indexed �Ȥ��ư����ޤ���
 * @throw   �Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
 */
mdarray mdarray::vsectionf( const char *exp_fmt, va_list ap ) const
{
    mdarray ret;

    this->vcopyf(&ret, exp_fmt, ap);

    ret.set_scopy_flag();
    return ret;
}

/**
 * @brief  �������Ǥ��̥��֥������Ȥإ��ԡ� (IDL�ε�ˡ)
 *
 *  �����ǡ��������Υ��дؿ��ǡ����Ȥ���������Ƥΰ���ʬ����ꤵ�줿���֥���
 *  ���� dest_obj �إ��ԡ����ޤ���<br>
 *  ���ԡ�������ϰϤ�Ǥ�դμ����ޤǻ����ǽ�� "0:99,*" �Τ褦�� IDL �� IRAF ��
 *  Ʊ�ͤ�ʸ����ǻ��ꤷ�ޤ�������ʸ������������ֹ�� 0-indexed �Ǥ���<br>
 *  ���ԡ���ؤϡ�����������ͤȥ��ԡ����η����ǥե�����͡��ݤ�ե饰��°����
 *  �ɤ��٤Ƥ򥳥ԡ����ޤ�������(���ԡ���) ������ϲ��Ѥ���ޤ���<br>
 *  dest_obj �˼��Ȥ�Ϳ������硤trimf()��¹Ԥ�������Ʊ����̤ˤʤ�ޤ���
 *
 * @param   dest_obj ���ԡ���Υ��֥�������
 * @param   exp_fmt  csv�������ϰϻ���ʸ����Τ���Υե����ޥåȻ���
 * @param   ...      exp_fmt���б���������Ĺ�����γ����ǥǡ���
 * @return  ���ԡ��������ǿ�(��� �� �Կ� �� �쥤��� �� ...)
 * @note    "[1:100,*]" �Τ褦�� [] �ǰϤ�ȡ�1-indexed �Ȥ��ư����ޤ���
 * @throw   �Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
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
 * @brief  �������Ǥ��̥��֥������Ȥإ��ԡ� (IDL�ε�ˡ)
 *
 *  �����ǡ��������Υ��дؿ��ǡ����Ȥ���������Ƥΰ���ʬ����ꤵ�줿���֥���
 *  ���� dest_obj �إ��ԡ����ޤ���<br>
 *  ���ԡ�������ϰϤ�Ǥ�դμ����ޤǻ����ǽ�� "0:99,*" �Τ褦�� IDL �� IRAF ��
 *  Ʊ�ͤ�ʸ����ǻ��ꤷ�ޤ�������ʸ������������ֹ�� 0-indexed �Ǥ���<br>
 *  ���ԡ���ؤϡ�����������ͤȥ��ԡ����η����ǥե�����͡��ݤ�ե饰��°����
 *  �ɤ��٤Ƥ򥳥ԡ����ޤ�������(���ԡ���) ������ϲ��Ѥ���ޤ���<br>
 *  dest_obj �˼��Ȥ�Ϳ������硤trimf()��¹Ԥ�������Ʊ����̤ˤʤ�ޤ���
 *
 * @param   dest_obj ���ԡ���Υ��֥�������
 * @param   exp_fmt  csv�������ϰϻ���ʸ����Τ���Υե����ޥåȻ���
 * @param   ap       exp_fmt���б���������Ĺ�����Υꥹ��
 * @return  ���ԡ��������ǿ�(��� �� �Կ� �� �쥤��� �� ...)
 * @note    "[1:100,*]" �Τ褦�� [] �ǰϤ�ȡ�1-indexed �Ȥ��ư����ޤ���
 * @throw   �Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
 */
ssize_t mdarray::vcopyf( mdarray *dest_obj, const char *exp_fmt, va_list ap ) const
{
    return this->image_vcopyf(dest_obj, exp_fmt, ap);
}

/* copy an convet all or a section to another mdarray object */
/* Flipping elements is supported                            */
/* [not implemented] */
/**
 * @brief  ���Ȥΰ������Ǥ��Ѵ�����������֤� (IDL�ε�ˡ)
 * @deprecated  ̤����
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
 * @brief  ���Ȥΰ������Ǥ��Ѵ�����������֤� (IDL�ε�ˡ)
 * @deprecated  ̤����
 */
ssize_t mdarray::vconvertf_copy( mdarray *dest_obj, const char *exp_fmt, va_list ap ) const
{
    return -1;
}

/* for SFITSIO */
/* [not implemented] */
/**
 * @brief  ���Ȥΰ������Ǥ��Ѵ�����������֤� (IDL�ε�ˡ)
 * @deprecated  ̤����
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
 * @brief  ���Ȥΰ������Ǥ��Ѵ�����������֤� (IDL�ε�ˡ)
 * @deprecated  ̤����
 */
ssize_t mdarray::vconvertf_copy_via_udf( mdarray *dest_obj, 
	  void (*func)(const void *,void *,size_t,int,void *), void *user_ptr,
	  const char *exp_fmt, va_list ap ) const
{
    return -1;
}

/**
 * @brief  ������ʬ�ʳ����������Ǥξõ� (�����ǡ����Υȥ�ߥ󥰡�IDL�ε�ˡ)
 *
 *  �����ǡ��������Υ��дؿ��ǡ����Ȥ�����Τ������ꤵ�줿��ʬ������Ĥ���
 *  ���곰����ʬ��õ�ޤ���<br>
 *  �ڤ�Ф���뤵����ϰϤ�Ǥ�դμ����ޤǻ����ǽ�� "0:99,*" �Τ褦�� IDL ��
 *  IRAF ��Ʊ�ͤ�ʸ����ǻ��ꤷ�ޤ�������ʸ������������ֹ�� 0-indexed �Ǥ���
 *
 * @param   exp_fmt  csv�������ϰϻ���ʸ����Τ���Υե����ޥåȻ���
 * @param   ...      exp_fmt���б���������Ĺ�����γ����ǥǡ���
 * @return  ���Ȥλ���
 * @note    "[1:100,*]" �Τ褦�� [] �ǰϤ�ȡ�1-indexed �Ȥ��ư����ޤ���
 * @throw   �Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
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
 * @brief  ������ʬ�ʳ����������Ǥξõ� (�����ǡ����Υȥ�ߥ󥰡�IDL�ε�ˡ)
 *
 *  �����ǡ��������Υ��дؿ��ǡ����Ȥ�����Τ������ꤵ�줿��ʬ������Ĥ���
 *  ���곰����ʬ��õ�ޤ���<br>
 *  �ڤ�Ф���뤵����ϰϤ�Ǥ�դμ����ޤǻ����ǽ�� "0:99,*" �Τ褦�� IDL ��
 *  IRAF ��Ʊ�ͤ�ʸ����ǻ��ꤷ�ޤ�������ʸ������������ֹ�� 0-indexed �Ǥ���
 *
 * @param   exp_fmt  csv�������ϰϻ���ʸ����Τ���Υե����ޥåȻ���
 * @param   ap       exp_fmt���б���������Ĺ�����Υꥹ��
 * @return  ���Ȥλ���
 * @note    "[1:100,*]" �Τ褦�� [] �ǰϤ�ȡ�1-indexed �Ȥ��ư����ޤ���
 * @throw   �Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
 */
mdarray &mdarray::vtrimf( const char *exp_fmt, va_list ap )
{
    this->vcopyf(this, exp_fmt, ap);
    return *this;
}

/* flip elements in a section */
/**
 * @brief  Ǥ�դμ���(ʣ�����)��������¤Ӥ�ȿž (IDL�ε�ˡ)
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
 * @brief  Ǥ�դμ���(ʣ�����)��������¤Ӥ�ȿž (IDL�ε�ˡ)
 */
mdarray &mdarray::vflipf( const char *exp_fmt, va_list ap )
{
    this->image_vflipf(exp_fmt, ap);
    return *this;
}

/**
 * @brief  (x,y)�Υȥ�󥹥ݡ�����Ԥʤä��������� (IDL�ε�ˡ)
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
 * @brief  (x,y)�Υȥ�󥹥ݡ�����Ԥʤä��������� (IDL�ε�ˡ)
 */
ssize_t mdarray::vtransposef_xy_copy( mdarray *dest, 
				      const char *exp_fmt, va_list ap ) const
{
    return this->image_vtransposef_xy_copy(dest, exp_fmt, ap);
}

/**
 * @brief  (x,y,z)��(z,x,y)�Υȥ�󥹥ݡ�����Ԥʤä��������� (IDL�ε�ˡ)
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
 * @brief  (x,y,z)��(z,x,y)�Υȥ�󥹥ݡ�����Ԥʤä��������� (IDL�ε�ˡ)
 */
ssize_t mdarray::vtransposef_xyz2zxy_copy( mdarray *dest, 
				      const char *exp_fmt, va_list ap ) const
{
    return this->image_vtransposef_xyz2zxy_copy(dest, exp_fmt, ap);
}

/* padding existing values in an array */
/**
 * @brief  ���Ȥ������ǥե�����ͤǥѥǥ��� (IDL�ε�ˡ)
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
 * @brief  ���Ȥ������ǥե�����ͤǥѥǥ��� (IDL�ε�ˡ)
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
 * @brief  ���Ȥ��������ꤵ�줿�����顼�ͤǽ񤭴��� (IDL�ε�ˡ)
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
 * @brief  ���Ȥ��������ꤵ�줿�����顼�ͤǽ񤭴��� (IDL�ε�ˡ)
 */
mdarray &mdarray::vfillf( double value, const char *exp_fmt, va_list ap )
{
    void (*func_d2dest)(const void *,void *,size_t,int,void *);
    struct mdarray_cnv_nd_prms prms;

    prms.total_len_elements = 0;			/* �����ʤ� */
    prms.round_flg = this->rounding();			/* �ͼθ���/�ڤ�Τ� */

    /* ���Ѵ��Τ���δؿ� (double�����Ȥη�) */
    func_d2dest = this->func_cnv_nd_d2x;

    if ( func_d2dest == NULL ) {
	err_report(__FUNCTION__,"WARNING","invalid sz_type; ignored");
	return *this;
    }

    (*func_d2dest)(&value, this->junk_rec, 1, +1, (void *)&prms);

    return this->image_vcleanf( this->junk_rec, exp_fmt, ap );
}

/**
 * @brief  ���Ȥ�����򥹥��顼�ͤǽ񤭴��� (�桼���ؿ���ͳ��IDL�ε�ˡ)
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
 * @brief  ���Ȥ�����򥹥��顼�ͤǽ񤭴��� (�桼���ؿ���ͳ��IDL�ε�ˡ)
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
 * @brief  ���Ȥ�����򥹥��顼�ͤǽ񤭴��� (�桼���ؿ���ͳ��IDL�ε�ˡ)
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
 * @brief  ���Ȥ�����򥹥��顼�ͤǽ񤭴��� (�桼���ؿ���ͳ��IDL�ε�ˡ)
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
 * @brief  ���Ȥ�����򥹥��顼�ͤǽ񤭴��� (�桼���ؿ���ͳ��IDL�ε�ˡ)
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
 * @brief  ���Ȥ�����򥹥��顼�ͤǽ񤭴��� (�桼���ؿ���ͳ��IDL�ε�ˡ)
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
 * @brief  ���Ȥ�����ˡ����ꤵ�줿�����顼�ͤǲû� (IDL�ε�ˡ)
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
 * @brief  ���Ȥ�����ˡ����ꤵ�줿�����顼�ͤǲû� (IDL�ε�ˡ)
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
 * @brief  ���Ȥ�����򡤻��ꤵ�줿�����顼�ͤǸ��� (IDL�ε�ˡ)
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
 * @brief  ���Ȥ�����򡤻��ꤵ�줿�����顼�ͤǸ��� (IDL�ε�ˡ)
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
 * @brief  ���Ȥ�����ˡ����ꤵ�줿�����顼�ͤ�軻 (IDL�ε�ˡ)
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
 * @brief  ���Ȥ�����ˡ����ꤵ�줿�����顼�ͤ�軻 (IDL�ε�ˡ)
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
 * @brief  ���Ȥ�����򡤻��ꤵ�줿�����顼�ͤǽ��� (IDL�ε�ˡ)
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
 * @brief  ���Ȥ�����򡤻��ꤵ�줿�����顼�ͤǽ��� (IDL�ε�ˡ)
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
/* �黻�ʤ��ڡ�����: convert() ��Ʊ����ˡ���Ѵ�������̤��®�˳�Ǽ�� */
/**
 * @brief  ���Ȥ�����˻��ꤵ�줿���֥������Ȥ������Ž���դ� (IDL�ε�ˡ)
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
 * @brief  ���Ȥ�����˻��ꤵ�줿���֥������Ȥ������Ž���դ� (IDL�ε�ˡ)
 */
mdarray &mdarray::vpastef( const mdarray &src,
			   const char *exp_fmt, va_list ap )
{
    void (*func)(const void *,void *,size_t,int,void *);
    func = NULL;
    return this->vpastef_via_udf(src,func,NULL,exp_fmt,ap);
}

/**
 * @brief  ���Ȥ�����˻��ꤵ�줿�����Ž���դ� (�桼���ؿ���ͳ��IDL�ε�ˡ)
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
 * @brief  ���Ȥ�����˻��ꤵ�줿�����Ž���դ� (�桼���ؿ���ͳ��IDL�ε�ˡ)
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
/* �黻����ڡ�����: ��ö double �����Ѵ�����Ƥ���黻������̤��Ǽ��  */
/* ���������α黻�ʤ��Ǥ���٤��٤���                                    */
/**
 * @brief  ���Ȥ�����˻��ꤵ�줿�����Ž���դ� (�桼���ؿ���ͳ��IDL�ε�ˡ)
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
 * @brief  ���Ȥ�����˻��ꤵ�줿�����Ž���դ� (�桼���ؿ���ͳ��IDL�ε�ˡ)
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
 * @brief  ���Ȥ�����˻��ꤵ�줿�����Ž���դ� (�桼���ؿ���ͳ��IDL�ε�ˡ)
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
 * @brief  ���Ȥ�����˻��ꤵ�줿�����Ž���դ� (�桼���ؿ���ͳ��IDL�ε�ˡ)
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
 * @brief  ���Ȥ�����ˡ����ꤵ�줿���֥������Ȥ������û� (IDL�ε�ˡ)
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
 * @brief  ���Ȥ�����ˡ����ꤵ�줿���֥������Ȥ������û� (IDL�ε�ˡ)
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
 * @brief  ���Ȥ�����򡤻��ꤵ�줿���֥������Ȥ�����Ǹ��� (IDL�ε�ˡ)
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
 * @brief  ���Ȥ�����򡤻��ꤵ�줿���֥������Ȥ�����Ǹ��� (IDL�ε�ˡ)
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
 * @brief  ���Ȥ�����ˡ����ꤵ�줿���֥������Ȥ������軻 (IDL�ε�ˡ)
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
 * @brief  ���Ȥ�����ˡ����ꤵ�줿���֥������Ȥ������軻 (IDL�ε�ˡ)
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
 * @brief  ���Ȥ�����򡤻��ꤵ�줿���֥������Ȥ�����ǽ��� (IDL�ε�ˡ)
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
 * @brief  ���Ȥ�����򡤻��ꤵ�줿���֥������Ȥ�����ǽ��� (IDL�ε�ˡ)
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
 * @brief  �������Ǥ��ڤ�Ф��������ƥ�ݥ�ꥪ�֥������ȤǼ���
 *
 * �����ǡ��������Υ��дؿ��ǡ����Ȥ�����Τ������ꤵ�줿��ʬ������ƥ�ݥ��
 * ���֥������Ȥ˥��ԡ�����������֤��ޤ���
 *
 * @param     col_idx ���ԡ����������
 * @param     col_len ���ԡ������󥵥���
 * @param     row_idx ���ԡ����ι԰���
 * @param     row_len ���ԡ����ιԥ�����
 * @param     layer_idx ���ԡ����Υ쥤�����
 * @param     layer_len ���ԡ����Υ쥤�䥵����
 * @return    �ƥ�ݥ�ꥪ�֥�������
 * @throw     �Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
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
 * @brief  �������Ǥ��̥��֥������Ȥإ��ԡ�
 *
 *  �����ǡ��������Υ��дؿ��ǡ����Ȥ���������Ƥΰ���ʬ����ꤵ�줿���֥���
 *  ���� dest_obj �إ��ԡ����ޤ���<br>
 *  ���ԡ���ؤϡ�����������ͤȥ��ԡ����η����ǥե�����͡��ݤ�ե饰��°����
 *  �ɤ��٤Ƥ򥳥ԡ����ޤ�������(���ԡ���) ������ϲ��Ѥ���ޤ���<br>
 *  dest_obj �˼��Ȥ�Ϳ������硤trim()��¹Ԥ�������Ʊ����̤ˤʤ�ޤ���
 *
 * @param     dest_obj ���ԡ���Υ��֥�������
 * @param     col_index ���ԡ����������
 * @param     col_size ���ԡ������󥵥���
 * @param     row_index ���ԡ����ι԰���
 * @param     row_size ���ԡ����ιԥ�����
 * @param     layer_idx ���ԡ����Υ쥤�����
 * @param     layer_size ���ԡ����Υ쥤�䥵����
 * @return    ���ԡ��������ǿ�(����� �Կ��� �쥤���)
 * @throw     �Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
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
 * @brief  ���Ȥΰ������Ǥ��Ѵ�����������֤�
 * @deprecated  ̤����
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
 * @brief  ���Ȥΰ������Ǥ��Ѵ�����������֤�
 * @deprecated  ̤����
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
 * @brief  ����� move_to() �ˤ���ư�������ΰ�λ���
 */
mdarray &mdarray::move_from( ssize_t col_index, size_t col_size,
			     ssize_t row_index, size_t row_size,
			     ssize_t layer_index, size_t layer_size )
{
    if ( this->fix_section_args( &col_index, &col_size, 
				 &row_index, &row_size,
				 &layer_index, &layer_size ) < 0 ) {
	/* ���ꤵ�줿�ϰϤ�¸�ߤ��ʤ����Ͻ���� */
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
 * @brief  move_from() �ǻ��ꤵ�줿�ΰ�ΰ�ư
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

    /* source ¦�Υ����å� */
    if ( this->fix_section_args( &col_index, &col_size, 
				 &row_index, &row_size,
				 &layer_index, &layer_size ) < 0 ) goto quit;

    /* dest ¦�Υ����å� */
    org_dest_col = dest_col;
    org_dest_row = dest_row;
    org_dest_layer = dest_layer;
    if ( this->fix_section_args( &dest_col, &col_size, 
				 &dest_row, &row_size,
				 &dest_layer, &layer_size ) < 0 ) goto quit;
    //err_report1(__FUNCTION__,"DEBUG","col_size = %zu",col_size);
    //err_report1(__FUNCTION__,"DEBUG","row_size = %zu",row_size);
    //err_report1(__FUNCTION__,"DEBUG","layer_size = %zu",layer_size);
    /* ���λ�����֤�����ͤ��ä��顤���줬���� */
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
	if ( dest_ptr < src_ptr ) {			/* ��������� */
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
	else if ( src_ptr < dest_ptr ) {		/* ��������� */
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
 * @brief  ������ʬ�ʳ����������Ǥξõ� (�����ǡ����Υȥ�ߥ�)
 *
 * �����ǡ��������Υ��дؿ��ǡ����Ȥ�����Τ������ꤵ�줿��ʬ������Ĥ���
 * ���곰����ʬ��õ�ޤ���<br>
 *
 * @param     col_idx �����
 * @param     col_len �󥵥���
 * @param     row_idx �԰���
 * @param     row_len �ԥ�����
 * @param     layer_idx �쥤�����
 * @param     layer_len �쥤�䥵����
 * @return    ���Ȥλ���
 * @throw     �Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
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
 * @brief  ����ο�ʿ�����Ǥ�ȿž
 *
 * ���Ȥ�����λ��ꤵ�줿�ϰϤ����ǤˤĤ��ơ��������Ƥ��ʿ������ȿž�����ޤ���
 *
 * @param     col_index �����
 * @param     col_size �󥵥���
 * @param     row_index �԰���
 * @param     row_size �ԥ�����
 * @param     layer_index �쥤�����
 * @param     layer_size �쥤�䥵����
 * @return    ���Ȥλ���
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
 * @brief  ����ο�ľ�����Ǥ�ȿž
 *
 * ���Ȥ�����λ��ꤵ�줿�ϰϤ����ǤˤĤ��ơ��������Ƥ��ľ������ȿž�����ޤ���
 *
 * @param     col_index �����
 * @param     col_size �󥵥���
 * @param     row_index �԰���
 * @param     row_size �ԥ�����
 * @param     layer_index �쥤�����
 * @param     layer_size �쥤�䥵����
 * @return    ���Ȥλ���
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


/* ���������Ҥä��꤫���� */
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
	    p0 ++;		/* ���˿ʤ�� */ \
	    p1 += len;		/* �Ĥ˿ʤ�� */ \
	} \
	tmp_p0 += len_plus_1;	/* �Ф�˿ʤ�� */ \
	tmp_p1 += len_plus_1;	/* �Ф�˿ʤ�� */ \
    } \
}
SLI__MDARRAY__DO_OPERATION_1TYPE_ALL(MAKE_FUNC,);
#undef MAKE_FUNC
}

/* ����2�����ǡ����Υ��ԡ��ؿ� */
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
 *  ��������ʰ���Хåե���Ȥäơ���® transpose ��Ԥʤ���
 *
 *                  src_x_len
 *           src_x_pos
 *            +--------------------+
 *            |                    |
 *   src_y_pos|  - - - - - - - -   |
 *            |  |              |<==== ������ʬ���� transpose ���ƥ��ԡ�
 *            |  |              |  |
 *            |  - - - - - - - -   |  +--+
 *            |    dest_height     |  |  | block_len(���������1�դ�Ĺ��)
 *            |                    |  +--+
 *            +--------------------+
 *
 *  src_y_step:  �̾��1����ꡥ�������ӤˤȤꤿ������2�ʾ����ꡥ
 *               (�����Ȥä� x-z �� transpose ����ǽ)
 *  dest_width:  ���ԡ���� transpose �������
 *  dest_height: ���ԡ���� transpose ��ι⤵��
 *
 */
/**
 * @brief  ��������ʰ���Хåե���Ȥäơ���® transpose ��Ԥʤ� (������)
 *
 * @note   private �ʴؿ��Ǥ���
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
    unsigned char *tmp_block = (unsigned char *)_tmp_block;  /* ����Хåե� */
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
	    /* tmp_block �˥��ԡ����ƤҤä����֤� */
	    for ( hlen_left = dest_height ;
		  block_len <= hlen_left ; 
		  hlen_left -= block_len ) {
		const unsigned char *src1;
		/* tmp_block �˥��ԡ� */
		unsigned char *tmp_p0 = tmp_block;
		unsigned char *tmp_p1;
		src1 = src0;
		for ( j=block_len ; 0 < j ; --j ) {
		    c_memcpy(tmp_p0, src1, block_blen);
		    tmp_p0 += block_blen;
		    src1 += src_bytes * src_x_step;
		}
		/* tmp_block����� transpose */
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
			    p0 += src_bytes;		       /* ���˿ʤ�� */
			    p1 += src_bytes * block_len;       /* �Ĥ˿ʤ�� */
			}
			tmp_p0 += src_bytes * (block_len + 1);  /* �Ф�˿ʤ�� */
			tmp_p1 += src_bytes * (block_len + 1);  /* �Ф�˿ʤ�� */
		    }
		}
		/* dest�إ��ԡ� */
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
	    /* �Ĥä���ʬ�����̤ˤҤä��꤫���� */
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

    /* �Ĥä���ʬ�����̤ˤҤä��꤫���� */
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
 * @brief  (x,y)�Υȥ�󥹥ݡ�����Ԥʤä���������
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
    heap_mem<unsigned char> tmp_block;		/* ���������ѥХåե� */
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

    if ( dest_ret == this ) dest_obj = &tmpobj;	     /* ��ʬ���Ȥξ�� */
    else dest_obj = dest_ret;			     /* ��ʬ���ȤǤ�̵����� */

    /* ���μ�������򥳥ԡ� */
    tmp_dim_size = this->dim_size_rec;
    tmp_size = dup_size_rec(this->cdimarray(), this->dim_size_rec);
    if ( tmp_size.ptr() == NULL ) {
	err_throw(__FUNCTION__,"FATAL","malloc() failed");
    }

    if ( row_ok == false ) {				/* 1 �����ξ�� */
	const size_t ndim = 2;
	const size_t naxisx[ndim] = {row_size, col_size};
	if ( dest_obj->size_type() == this->size_type() &&
	     dest_obj->length() == col_size * row_size ) {
	    dest_obj->reallocate(naxisx, ndim, false);
	}
	else dest_obj->init(this->size_type(), true, naxisx, ndim, true);
    }
    else if ( layer_ok == false ) {			/* 2 �����ξ�� */
	const size_t ndim = 2;
	const size_t naxisx[ndim] = {row_size, col_size};
	if ( dest_obj->size_type() == this->size_type() &&
	     dest_obj->length() == col_size * row_size ) {
	    dest_obj->reallocate(naxisx, ndim, false);
	}
	else dest_obj->init(this->size_type(), true, naxisx, ndim, true);
    }
    else {
	/* 4�����ʾ�ξ�硤���쥤�����ʤ�4�����ʹߤμ������Ĥ� */
	if ( 3 < this->dim_length() && layer_all == true ) {
	    size_t len_dest = 1;
	    /* 1��2���������ѹ� */
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
	/* 3 �����ξ�� */
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

    /* PentiumM �ޥ���ǤΥƥ��ȷ�� */
    /* (4096x4096 double��)          */
    /* �Х���Ĺ  �в����  */
    /*   16^2  ... 1.53s   */
    /*   32^2  ... 1.03s   */
    /*   64^2  ... 0.84s   */
    /*   128^2 ... 0.74s   */
    /*   256^2 ... 0.71s   */
    /*   512^2 ... 0.79s   */

    /* ��������Хåե��ΥХ���Ĺ�� 16^2 ���� 256^2 ���ͤ�Ȥ�褦������ */
    min_len_block = (size_t)(1.0 + sqrt((double)(16*16) / this->bytes()));
    max_len_block = (size_t)(1.0 + sqrt((double)(256*256) / this->bytes()));
    if ( row_size < col_size ) min_size = row_size;
    else min_size = col_size;

    if ( min_size <= min_len_block ) len_block = min_len_block;
    else {
	size_t ndiv = ( (min_size - 1) / max_len_block ) + 1;
	len_block = min_size / ndiv;
    }

    /* ��������Хåե�(����Хåե�)����� */
    if ( tmp_block.allocate(this->bytes() * len_block * len_block) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","malloc() failed");
    }

    /* transpose �Ѥ����ѥ��ԡ��ؿ������� */
    func_gencpy2d = NULL;
#define SEL_FUNC(fncname,src_sztp,src_tp,dest_sztp,dest_tp,fnc) \
    if ( this->size_type() == src_sztp && this->size_type() == dest_sztp ) { \
	func_gencpy2d = &gencpy2d::fncname; \
    }
    SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(SEL_FUNC,,,,,,,,,,,,,,,,,,,,,,,,,,,,else);
#undef SEL_FUNC

    if ( this->rounding() == true ) f_idx = 1;		/* �ͼθ������Ѵ� */
    else f_idx = 0;					/* �ڤ�ΤƤ��Ѵ� */

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

	dest_ptr += this_bytes * row_size * col_size;    /* ���Υ쥤��� */
	j_src_ptr += colrow_blen;

    }

    /* ��ʬ���Ȥξ�� */
    if ( dest_ret == this ) {
	dest_ret->swap(tmpobj);
    }

    ret = layer_size * row_size * col_size;

 quit:
    return ret;
}

/* interchange xyz to zxy and copy */
/**
 * @brief  (x,y,z)��(z,x,y)�Υȥ�󥹥ݡ�����Ԥʤä���������
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
    heap_mem<unsigned char> tmp_block;		/* ���������ѥХåե� */
    size_t len_block, min_len_block, max_len_block, min_size;
    void (*func_gencpy2d)(const void *, void *, 
		       size_t, size_t, size_t, size_t, size_t, size_t, void *);
    size_t f_idx;

    if ( dest_ret == NULL || this->length() == 0 ) goto quit;

    if ( this->test_section_args( &col_index, &col_size, 
				 &row_index, &row_size,
				 &layer_index, &layer_size,
				 &col_ok, &row_ok, &layer_ok ) < 0 ) goto quit;

    if ( dest_ret == this ) dest_obj = &tmpobj;	     /* ��ʬ���Ȥξ�� */
    else dest_obj = dest_ret;			     /* ��ʬ���ȤǤ�̵����� */

    /* ���μ�������򥳥ԡ� */
    tmp_dim_size = this->dim_size_rec;
    tmp_size = dup_size_rec(this->cdimarray(), this->dim_size_rec);
    if ( tmp_size.ptr() == NULL ) {
	err_throw(__FUNCTION__,"FATAL","malloc() failed");
    }

    if ( row_ok == false ) {				/* 1 �����ξ�� */
	const size_t ndim = 3;
	const size_t naxisx[ndim] = {layer_size, col_size, row_size};
	if ( dest_obj->size_type() == this->size_type() &&
	     dest_obj->length() == col_size * row_size * layer_size ) {
	    dest_obj->reallocate(naxisx, ndim, false);
	}
	else dest_obj->init(this->size_type(), true, naxisx, ndim, true);
    }
    else if ( layer_ok == false ) {			/* 2 �����ξ�� */
	const size_t ndim = 3;
	const size_t naxisx[ndim] = {layer_size, col_size, row_size};
	if ( dest_obj->size_type() == this->size_type() &&
	     dest_obj->length() == col_size * row_size * layer_size ) {
	    dest_obj->reallocate(naxisx, ndim, false);
	}
	else dest_obj->init(this->size_type(), true, naxisx, ndim, true);
    }
    else {
	/* 4�����ʾ�ξ�硤���쥤�����ʤ�4�����ʹߤμ������Ĥ� */
	if ( 3 < this->dim_length() && layer_all == true ) {
	    size_t len_dest = 1;
	    /* 1��3���������ѹ� */
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
	/* 3 �����ξ�� */
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

    /* ��������Хåե��ΥХ���Ĺ�� 16^2 ���� 256^2 ���ͤ�Ȥ�褦������ */
    min_len_block = (size_t)(1.0 + sqrt((double)(16*16) / this->bytes()));
    max_len_block = (size_t)(1.0 + sqrt((double)(256*256) / this->bytes()));
    if ( layer_size < col_size ) min_size = layer_size;
    else min_size = col_size;

    if ( min_size <= min_len_block ) len_block = min_len_block;
    else {
	size_t ndiv = ( (min_size - 1) / max_len_block ) + 1;
	len_block = min_size / ndiv;
    }

    /* ��������Хåե�(����Хåե�)����� */
    if ( tmp_block.allocate(this->bytes() * len_block * len_block) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","malloc() failed");
    }

    /* transpose �Ѥ����ѥ��ԡ��ؿ������� */
    func_gencpy2d = NULL;
#define SEL_FUNC(fncname,src_sztp,src_tp,dest_sztp,dest_tp,fnc) \
    if ( this->size_type() == src_sztp && this->size_type() == dest_sztp ) { \
	func_gencpy2d = &gencpy2d::fncname; \
    }
    SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(SEL_FUNC,,,,,,,,,,,,,,,,,,,,,,,,,,,,else);
#undef SEL_FUNC

    if ( this->rounding() == true ) f_idx = 1;		/* �ͼθ������Ѵ� */
    else f_idx = 0;					/* �ڤ�ΤƤ��Ѵ� */

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

    /* ��ʬ���Ȥξ�� */
    if ( dest_ret == this ) {
	dest_ret->swap(tmpobj);
    }

    ret = layer_size * row_size * col_size * i_cnt;

 quit:
    return ret;
}

/* rotate and copy a region to another mdarray object */
/**
 * @brief  (x,y)�ǲ�ž�������������� (��ž��90��ñ��)
 */
ssize_t mdarray::rotate_xy_copy( mdarray *dest_ret, int angle,
				 ssize_t col_index, size_t col_size, 
				 ssize_t row_index, size_t row_size,
				 ssize_t layer_index, size_t layer_size ) const
{
    ssize_t ret = -1;
    int q;

    /* q=1: ��90�ٲ�ž��q=2: 180�ٲ�ž��q=3: ��90�ٲ�ž */
    if ( 0 <= angle ) q = (angle % 360) / 90;
    else q = (360 + (angle % 360)) / 90;

    if ( dest_ret == NULL || this->length() == 0 ) goto quit;

    /* ��ž�ʤ��ξ�� */
    if ( q == 0 ) {
	ret = this->copy(dest_ret, col_index, col_size, 
			 row_index, row_size, layer_index, layer_size);
    }
    /* 180�ٲ�ž�ξ�� */
    else if ( q == 2 ) {
	ret = this->copy(dest_ret, col_index, col_size, 
			 row_index, row_size, layer_index, layer_size);
	dest_ret->flip(0, 0, dest_ret->length(0));
	dest_ret->flip(1, 0, dest_ret->length(1));
    }
    /* ��90�ٲ�ž�ξ�� */
    else if ( q == 1 ) {
	ret = this->transpose_xy_copy(dest_ret, col_index, col_size, 
			 row_index, row_size, layer_index, layer_size);
	dest_ret->flip(0, 0, dest_ret->length(0));
    }
    /* ��90�ٲ�ž�ξ�� */
    else if ( q == 3 ) {
	ret = this->transpose_xy_copy(dest_ret, col_index, col_size, 
			 row_index, row_size, layer_index, layer_size);
	dest_ret->flip(1, 0, dest_ret->length(1));
    }
    
 quit:
    return ret;
}


/**
 * @brief  ���Ȥ������ǥե�����ͤǥѥǥ���
 *
 *  ���Ȥ���������Ǥ�ǥե�����ͤǥѥǥ��󥰤��ޤ���<br>
 *  �����ϻ��ꤷ�ʤ��Ƥ���ѤǤ��ޤ������ξ��ϡ������Ǥ������оݤǤ���
 *  clean() ��¹Ԥ��Ƥ�����Ĺ���Ѳ����ޤ���<br>
 *  �����ǡ��������Υ��дؿ��Ǥ���<br>
 *
 * @param     col_index �����
 * @param     col_size �󥵥���
 * @param     row_index �԰���
 * @param     row_size �ԥ�����
 * @param     layer_index �쥤�����
 * @param     layer_size �쥤�䥵����
 * @return    ���Ȥλ���
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
 * @brief  ���Ȥ��������ꤵ�줿�����顼�ͤǽ񤭴���
 *
 *  ���Ȥ�����λ��ꤵ�줿�ϰϤ����Ǥ򡤻��ꤵ�줿�ͤǽ񴹤��ޤ���<br>
 *  �����ǡ��������Υ��дؿ��Ǥ���
 *
 * @param     value �񤭹�����
 * @param     col_index �����
 * @param     col_size �󥵥���
 * @param     row_index �԰���
 * @param     row_size �ԥ�����
 * @param     layer_index �쥤�����
 * @param     layer_size �쥤�䥵����
 * @return    ���Ȥλ���
 * @note      ���дؿ�1
 */
mdarray &mdarray::fill( double value, ssize_t col_index, size_t col_size, 
			ssize_t row_index, size_t row_size,
			ssize_t layer_index, size_t layer_size )
{
    debug_report("( double value, ssize_t ... )");

    void (*func_d2dest)(const void *,void *,size_t,int,void *);
    struct mdarray_cnv_nd_prms prms;

    prms.total_len_elements = 0;			/* �����ʤ� */
    prms.round_flg = this->rounding();			/* �ͼθ���/�ڤ�Τ� */

    /* ���Ѵ��Τ���δؿ� (double�����Ȥη�) */
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
 * @brief  ���Ȥ��������ꤵ�줿�����顼�ͤǽ񤭴��� (�桼���ؿ���ͳ)
 *
 *  ���Ȥ�����λ��ꤵ�줿�ϰϤ����Ǥ򡤥桼������ؿ���ͳ�ǽ񴹤��ޤ���<br>
 *  �����ǡ��������Υ��дؿ��Ǥ���
 *
 * @param      value �񤭹�����
 * @param      func ���Ѵ��ΰ٤Υ桼���ؿ��Υ��ɥ쥹
 * @param      user_ptr func �κǸ��Ϳ������桼���Υݥ���
 * @param      col_index �����
 * @param      col_size �󥵥���
 * @param      row_index �԰���
 * @param      row_size �ԥ�����
 * @param      layer_index �쥤�����
 * @param      layer_size �쥤�䥵����
 * @return     ���Ȥλ���
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
 * @brief  ���Ȥ��������ꤵ�줿�����顼�ͤǽ񤭴��� (�桼���ؿ���ͳ)
 *
 *  ���Ȥ�����λ��ꤵ�줿�ϰϤ����Ǥ򡤻��ꤵ�줿�ͤȥ桼������ؿ���ͳ�ǽ�
 *  ���ޤ���<br>
 *  �桼������ؿ� func �ΰ����ˤϽ�ˡ����Ȥ�������(����)��value ��Ϳ����줿
 *  �͡�����(�ǽ�ΰ���)�θĿ�������֡��԰��֡��쥤����֡����ȤΥ��֥�������
 *  �Υ��ɥ쥹���桼���ݥ��� user_ptr ���͡���Ϳ�����ޤ���<br>
 *  �����ǡ��������Υ��дؿ��Ǥ���
 *
 * @param     value �񤭹�����
 * @param     func ���Ѵ��ΰ٤Υ桼���ؿ��Υ��ɥ쥹
 * @param     user_ptr func �κǸ��Ϳ������桼���Υݥ���
 * @param     col_index �����
 * @param     col_size �󥵥���
 * @param     row_index �԰���
 * @param     row_size �ԥ�����
 * @param     layer_index �쥤�����
 * @param     layer_size �쥤�䥵����
 * @return    ���Ȥλ���
 * @note      ���дؿ�2
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
 * @brief  ���Ȥ��������ꤵ�줿�����顼�ͤǽ񤭴��� (�桼���ؿ���ͳ)
 *
 *  ���Ȥ�����λ��ꤵ�줿�ϰϤ����Ǥ򡤻��ꤵ�줿�ͤȥ桼������ؿ���ͳ�ǽ�
 *  ���ޤ���<br>
 *  �桼������ؿ�func �ΰ����ˤϽ�ˡ����Ȥ������͡�value ��Ϳ����줿�͡����
 *  �֡��԰��֡��쥤����֡����ȤΥ��֥������ȤΥ��ɥ쥹���桼���ݥ���
 *  user_ptr ���͡���Ϳ�����ޤ���<br>
 *  �����ǡ��������Υ��дؿ��Ǥ���
 *
 * @param     value �񤭹�����
 * @param     func_dest2d ���Ȥ����ǡ�����double �����Ѵ����뤿��Υ桼���ؿ��Υ��ɥ쥹
 * @param     user_ptr_dest2d func dest2d �κǸ��Ϳ������桼���Υݥ���
 * @param     func_d2dest double �����ͤ��鼫�Ȥ����ǡ������Ѵ����뤿��Υ桼���ؿ��Υ��ɥ쥹
 * @param     user_ptr_d2dest func d2dest �κǸ��Ϳ������桼���Υݥ���
 * @param     func ���Ѵ��ΰ٤Υ桼���ؿ��Υ��ɥ쥹
 * @param     user_ptr func �κǸ��Ϳ������桼���Υݥ���
 * @param     col_index �����
 * @param     col_size �󥵥���
 * @param     row_index �԰���
 * @param     row_size �ԥ�����
 * @param     layer_index �쥤�����
 * @param     layer_size �쥤�䥵����
 * @return    ���Ȥλ���
 * @note      ���дؿ�3
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
 * @brief  ���Ȥ�����ˡ����ꤵ�줿�����顼�ͤ�û�
 *
 *  ���Ȥ�����λ��ꤵ�줿�ϰϤ����Ǥ� value ��û����ޤ���
 *  �����ǡ��������Υ��дؿ��Ǥ���
 *
 * @param     value �û�������
 * @param     col_index �����
 * @param     col_size �󥵥���
 * @param     row_index �԰���
 * @param     row_size �ԥ�����
 * @param     layer_index �쥤�����
 * @param     layer_size �쥤�䥵����
 * @return    ���Ȥλ���
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
 * @brief  ���Ȥ�����򡤻��ꤵ�줿�����顼�ͤǸ���
 *
 *  ���Ȥ�����λ��ꤵ�줿�ϰϤ����Ǥ��� value �򸺻����ޤ���
 *  �����ǡ��������Υ��дؿ��Ǥ���
 *
 * @param     value ����������
 * @param     col_index �����
 * @param     col_size �󥵥���
 * @param     row_index �԰���
 * @param     row_size �ԥ�����
 * @param     layer_index �쥤�����
 * @param     layer_size �쥤�䥵����
 * @return    ���Ȥλ���
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
 * @brief  ���Ȥ�����ˡ����ꤵ�줿�����顼�ͤ�軻
 *
 *  ���Ȥ�����λ��ꤵ�줿�ϰϤ����Ǥ��ͤ� value ��軻���ޤ���
 *  �����ǡ��������Υ��дؿ��Ǥ���
 *
 * @param     value �軻������
 * @param     col_index �����
 * @param     col_size �󥵥���
 * @param     row_index �԰���
 * @param     row_size �ԥ�����
 * @param     layer_index �쥤�����
 * @param     layer_size �쥤�䥵����
 * @return    ���Ȥλ���
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
 * @brief  ���Ȥ�����򡤻��ꤵ�줿�����顼�ͤǽ���
 *
 *  ���Ȥ�����λ��ꤵ�줿�ϰϤ����Ǥ��ͤˤĤ��� value �ǽ������ޤ���
 *  �����ǡ��������Υ��дؿ��Ǥ���
 *
 * @param     value ����������
 * @param     col_index �����
 * @param     col_size �󥵥���
 * @param     row_index �԰���
 * @param     row_size �ԥ�����
 * @param     layer_index �쥤�����
 * @param     layer_size �쥤�䥵����
 * @return    ���Ȥλ���
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
/* �黻�ʤ��ڡ�����: convert() ��Ʊ����ˡ���Ѵ�������̤��®�˳�Ǽ�� */

/**
 * @brief  ���Ȥ�����˻��ꤵ�줿���֥������Ȥ������Ž���դ�
 *
 *  ���Ȥ�����λ��ꤵ�줿�ϰϤ������ͤˡ�src_img �ǻ��ꤵ�줿���֥������Ȥ�
 *  �������ͤ�Ž���դ��ޤ���<br>
 *  �����ǡ��������Υ��дؿ��Ǥ���
 *
 * @param     src_img �����Ȥʤ��������ĥ��֥�������
 * @param     dest_col �����
 * @param     dest_row �԰���
 * @param     dest_layer �쥤�����
 * @return    ���Ȥλ���
 * @note      ���дؿ�1
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
 * @brief  ���Ȥ�����˻��ꤵ�줿���֥������Ȥ������Ž���դ� (�桼���ؿ���ͳ)
 *
 *  ���Ȥ�����λ��ꤵ�줿�ϰϤ������ͤˡ�src_img �ǻ��ꤵ�줿���֥������Ȥ�
 *  �������ͤ�桼������ؿ���ͳ��Ž���դ��ޤ����桼������ؿ���Ϳ����Ž����
 *  �����ε�ư���Ѥ��뤳�Ȥ��Ǥ��ޤ���<br>
 *  �����ǡ��������Υ��дؿ��Ǥ���
 *
 * @param      src_img �����Ȥʤ��������ĥ��֥�������
 * @param      func ���Ѵ��Τ���Υ桼���ؿ��Υ��ɥ쥹
 * @param      user_ptr func �κǸ��Ϳ������桼���Υݥ���
 * @param      dest_col �����
 * @param      dest_row �԰���
 * @param      dest_layer �쥤�����
 * @return     ���Ȥλ���
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
 * @brief  ���Ȥ�����˻��ꤵ�줿���֥������Ȥ������Ž���դ� (�桼���ؿ���ͳ)
 *
 *  ���Ȥ�����λ��ꤵ�줿�ϰϤ������ͤˡ�src_img �ǻ��ꤵ�줿���֥�������
 *  �� �������ͤ�桼������ؿ���ͳ��Ž���դ��ޤ����桼������ؿ���Ϳ����
 *  Ž���դ����ε�ư���Ѥ��뤳�Ȥ��Ǥ��ޤ���<br>
 *  �桼������ؿ� func �ΰ����ˤϽ�ˡ����Ȥ�������(����)�����֥������� 
 *  src_img ��������(����)���ǽ��2�Ĥΰ���������θĿ�������֡��԰��֡�
 *  �쥤����֡����ȤΥ��֥������ȤΥ��ɥ쥹���桼���ݥ��� user_ptr ����
 *  ��Ϳ�����ޤ���<br>
 *  �����ǡ��������Υ��дؿ��Ǥ���
 *
 * @param     src_img �����Ȥʤ��������ĥ��֥�������
 * @param     func ���Ѵ��Τ���Υ桼���ؿ��Υ��ɥ쥹
 * @param     user_ptr func �κǸ��Ϳ������桼���Υݥ���
 * @param     dest_col �����
 * @param     dest_row �԰���
 * @param     dest_layer �쥤�����
 * @return    ���Ȥλ���
 * @note      ���дؿ�2
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
 * @brief  ���Ȥ�����˻��ꤵ�줿���֥������Ȥ������Ž���դ� (�桼���ؿ���ͳ)
 *
 *  ���Ȥ�����λ��ꤵ�줿�ϰϤ������ͤˡ�src_img �ǻ��ꤵ�줿���֥������Ȥ�
 *  �������ͤ�桼������ؿ���ͳ��Ž���դ��ޤ����桼������ؿ���Ϳ����Ž����
 *  �����ε�ư���Ѥ��뤳�Ȥ��Ǥ��ޤ���<br>
 *  �����ǡ��������Υ��дؿ��Ǥ���
 *
 * @param     src_img �����Ȥʤ��������ĥ��֥�������
 * @param     func_src2d ���֥������� src_img �����ǡ�����double �����Ѵ����뤿��Υ桼���ؿ��ؤΥ��ɥ쥹
 * @param     user_ptr_src2d func_src2d �κǸ��Ϳ������桼���Υݥ���
 * @param     func_dest2d ���Ȥ����ǡ�����double �����Ѵ����뤿��Υ桼���ؿ��ؤΥ��ɥ쥹
 * @param     user_ptr_dest2d func dest2d �κǸ��Ϳ������桼���Υݥ���
 * @param     func_d2dest double ���ͤ��鼫�Ȥ����ǡ������Ѵ����뤿��Υ桼���ؿ��ؤΥ��ɥ쥹
 * @param     user_ptr_d2dest func d2dest �κǸ��Ϳ������桼���Υݥ���
 * @param     func ���Ѵ��Τ���Υ桼���ؿ��Υ��ɥ쥹
 * @param     user_ptr func �κǸ��Ϳ������桼���Υݥ���
 * @param     dest_col �����
 * @param     dest_row �԰���
 * @param     dest_layer �쥤�����
 * @return    ���Ȥλ���
 * @note      ���дؿ�3
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
 * @brief  ���Ȥ�����ˡ����ꤵ�줿���֥������Ȥ������û�
 *
 *  ���Ȥ����Ǥ˥��֥������� src_img �����������û����ޤ���
 *  �󡦹ԡ��쥤��ˤĤ��Ƥ��줾��βû�Ŭ�ѳ��ϰ��֤����Ǥ��ޤ���<br>
 *  �����ǡ��������Υ��дؿ��Ǥ���
 *
 * @param     src_img �黻�˻Ȥ��������ĥ��֥�������
 * @param     dest_col �û����ϰ���(��)
 * @param     dest_row �û����ϰ���(��)
 * @param     dest_layer �û����ϰ���(�쥤��)
 * @return    ���Ȥλ���
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
 * @brief  ���Ȥ�����򡤻��ꤵ�줿���֥������Ȥ�����Ǹ���
 *
 *  ���Ȥ�����������ͤ��饪�֥������� src_img ����������������ͤ򸺻����ޤ���
 *  �󡦹ԡ��쥤��ˤĤ��Ƥ��줾��θ���Ŭ�ѳ��ϰ��֤����Ǥ��ޤ���<br>
 *  �����ǡ��������Υ��дؿ��Ǥ���
 *
 * @param     src_img �黻�˻Ȥ��������ĥ��֥�������
 * @param     dest_col �������ϰ���(��)
 * @param     dest_row �������ϰ���(��)
 * @param     dest_layer �������ϰ���(�쥤��)
 * @return    ���Ȥλ���
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
 * @brief  ���Ȥ�����ˡ����ꤵ�줿���֥������Ȥ������軻
 *
 *  ���Ȥ�����������ͤ˥��֥������� src_img �����������軻���ޤ���
 *  �󡦹ԡ��쥤��ˤĤ��Ƥ��줾��ξ軻Ŭ�ѳ��ϰ��֤����Ǥ��ޤ���<br>
 *  �����ǡ��������Υ��дؿ��Ǥ���
 *
 * @param     src_img �黻�˻Ȥ��������ĥ��֥�������
 * @param     dest_col �軻���ϰ���(��)
 * @param     dest_row �軻���ϰ���(��)
 * @param     dest_layer �軻���ϰ���(�쥤��)
 * @return    ���Ȥλ���
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
 * @brief  ���Ȥ�����򡤻��ꤵ�줿���֥������Ȥ�����ǽ���
 *
 *  ���Ȥ�����������ͤ��饪�֥������� src_img �����������������ޤ���
 *  �󡦹ԡ��쥤��ˤĤ��Ƥ��줾��ν���Ŭ�ѳ��ϰ��֤����Ǥ��ޤ���<br>
 *  �����ǡ��������Υ��дؿ��Ǥ���
 *
 * @param     src_img �黻�˻Ȥ��������ĥ��֥�������
 * @param     dest_col �������ϰ���(��)
 * @param     dest_row �������ϰ���(��)
 * @param     dest_layer �������ϰ���(�쥤��)
 * @return    ���Ȥλ���
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
 * @brief  section() ���ΰ�����Ĵ�٤�Ŭ�ڤʺ�ɸ�Υѥ�᡼��������
 *
 *  �����κ�ɸ�����������֤򼨤��Ƥ뤫��Ƚ�Ǥ���
 *  �������ʤ����(�Ϥ߽Ф��Ƥ�����)�Ϻ�ɸ�Υѥ�᡼����Ĵ�����ޤ���
 *
 * @param   r_col_index �����
 * @param   r_col_size �󥵥���
 * @param   r_row_index �԰���
 * @param   r_row_size �ԥ�����
 * @param   r_layer_index �쥤�����
 * @param   r_layer_size �쥤�䥵����
 * @return  ���������Ȥ�������ΰ���򼨤��Ƥ�������0��
 *          �ΰ褫��Ϥ߽Ф��Ƥ��뤬ͭ���ΰ褬¸�ߤ�����������͡�
 *          ͭ���ΰ褬̵���������͡�
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
 * @brief  �������Ǥ�x������ʬ��ǤΥ�����󡦥������󥹤γ���
 *
 * ���Ȥ�����λ��ꤵ�줿�ϰϤ����Ǥ򡤹�ñ�̤ǥ������(�饹�����������)���뤿
 * ��ν����򤷤ޤ�����ñ�̤Υ�������Ԥʤ���硤���μ��ǰ�Ϣ��API��Ȥ���
 * ����<br>
 * - beginf_scan_along_x() �ǽ����򤹤롥<br>
 * - scan_along_x() ��Ԥο������ƤӽФ����Ԥ��Ȥν�����Ԥʤ���<br>
 * - end_scan_along_x() �ǽ�λ����(����Хåե��γ���)��Ԥʤ���<br>
 * scan_along_x() ��1��ʬ�����Ǥ����ä�����Хåե��Υ��ɥ쥹(32�Х��Ȥǥ��饤
 * �󤵤�Ƥ���)���֤��ޤ������ΰ���Хåե��ϥ��֥������Ȥ������������Ǥ��ä�
 * �� double �����󶡤���ޤ����ʤ������ΥХåե��ϥ��֥������ȤǴ�������Ƥ���
 * ���ᡤ�ץ���ޤ��������ƤϤ����ޤ���<br>
 * scan_along_x() �ϻ����ϰ����ƤΥ�����󤬴�λ���Ƥ�����ˤ� NULL ���֤���
 * ����<br>
 * �����ǡ��������פ�Ȥ���ʤɤ˻Ȥ����дؿ��Ǥ���
 *
 * @param     exp_fmt ������󤹤��ϰϤ򼨤�ʸ����(��: "0:99,*")�Τ����
 *                    �ե����ޥåȻ���(printf��Ʊ��)
 * @param     ... exp_fmt���б���������Ĺ�����γ����ǥǡ���
 * @return    �������ˤ��ͭ�����Ǥο������顼�ξ������͡�
 * @note      "[1:100,*]" �Τ褦�� [] �ǰϤ�ȡ�1-indexed �Ȥ��ư����ޤ���
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
 * @brief  �������Ǥ�x������ʬ��ǤΥ�����󡦥������󥹤γ���
 *
 * ���Ȥ�����λ��ꤵ�줿�ϰϤ����Ǥ򡤹�ñ�̤ǥ������(�饹�����������)���뤿
 * ��ν����򤷤ޤ�����ñ�̤Υ�������Ԥʤ���硤���μ��ǰ�Ϣ��API��Ȥ���
 * ����<br>
 * - vbeginf_scan_along_x() �ǽ����򤹤롥<br>
 * - scan_along_x() ��Ԥο������ƤӽФ����Ԥ��Ȥν�����Ԥʤ���<br>
 * - end_scan_along_x() �ǽ�λ����(����Хåե��γ���)��Ԥʤ���<br>
 * scan_along_x() ��1��ʬ�����Ǥ����ä�����Хåե��Υ��ɥ쥹(32�Х��Ȥǥ��饤
 * �󤵤�Ƥ���)���֤��ޤ������ΰ���Хåե��ϥ��֥������Ȥ������������Ǥ��ä�
 * �� double �����󶡤���ޤ����ʤ������ΥХåե��ϥ��֥������ȤǴ�������Ƥ���
 * ���ᡤ�ץ���ޤ��������ƤϤ����ޤ���<br>
 * scan_along_x() �ϻ����ϰ����ƤΥ�����󤬴�λ���Ƥ�����ˤ� NULL ���֤���
 * ����<br>
 * �����ǡ��������פ�Ȥ���ʤɤ˻Ȥ����дؿ��Ǥ���
 *
 * @param     exp_fmt ������󤹤��ϰϤ򼨤�ʸ����(��: "0:99,*")�Τ����
 *                    �ե����ޥåȻ���(printf��Ʊ��)
 * @param     ap exp_fmt���б���������Ĺ�����Υꥹ��
 * @return    �������ˤ��ͭ�����Ǥο������顼�ξ������͡�
 * @note      "[1:100,*]" �Τ褦�� [] �ǰϤ�ȡ�1-indexed �Ȥ��ư����ޤ���
 */
ssize_t mdarray::vbeginf_scan_along_x( const char *exp_fmt, va_list ap ) const
{
    ssize_t ret_len = -1;
    mdarray *thisp = (mdarray *)this;			/* ����Ū�˥��㥹�� */
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

    /* �ѡ�������secinfo ������ */
    if ( parse_section_exp(&secinfo, &n_secinfo, secexp.ptr()) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","Error in parse_section_exp()");
    }
    if ( n_secinfo == 0 ) goto quit;

    /* secinfo ������å������� */
    if ( test_section_info( secexp.ptr(), *this, 
			    &secinfo, &n_secinfo, NULL ) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","Error in test_section_info()");
    }
    if ( n_secinfo == 0 ) goto quit;

    /* ����Ǥ�3�����Ȥ���(�֤���x,y,z�Τ���) */
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

    /* ���߰��֤򥻥å� */
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
 * @brief  �������Ǥ�x������ʬ��ǤΥ�����󡦥������󥹤γ���
 *
 * ���Ȥ�����λ��ꤵ�줿�ϰϤ����Ǥ򡤹�ñ�̤ǥ������(�饹�����������)���뤿
 * ��ν����򤷤ޤ�����ñ�̤Υ�������Ԥʤ���硤���μ��ǰ�Ϣ��API��Ȥ���
 * ����<br>
 * - begin_scan_along_x() �ǽ����򤹤롥<br>
 * - scan_along_x() ��Ԥο������ƤӽФ����Ԥ��Ȥν�����Ԥʤ���<br>
 * - end_scan_along_x() �ǽ�λ����(����Хåե��γ���)��Ԥʤ���<br>
 * scan_along_x() ��1��ʬ�����Ǥ����ä�����Хåե��Υ��ɥ쥹(32�Х��Ȥǥ��饤
 * �󤵤�Ƥ���)���֤��ޤ������ΰ���Хåե��ϥ��֥������Ȥ������������Ǥ��ä�
 * �� double �����󶡤���ޤ����ʤ������ΥХåե��ϥ��֥������ȤǴ�������Ƥ���
 * ���ᡤ�ץ���ޤ��������ƤϤ����ޤ���<br>
 * scan_along_x() �ϻ����ϰ����ƤΥ�����󤬴�λ���Ƥ�����ˤ� NULL ���֤���
 * ����<br>
 * �����ǡ��������פ�Ȥ���ʤɤ˻Ȥ����дؿ��Ǥ���
 *
 * @param     col_index �����
 * @param     col_size �󥵥���
 * @param     row_index �԰���
 * @param     row_size �ԥ�����
 * @param     layer_index �쥤�����
 * @param     layer_size �쥤�䥵����
 * @return    �������ˤ��ͭ�����Ǥο������顼�ξ������͡�
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
    mdarray *thisp = (mdarray *)this;			/* ����Ū�˥��㥹�� */

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

    thisp->scanx_begin[0] = col_index;	/* ��������ΰ�ΰ��֤���¸ */
    thisp->scanx_begin[1] = row_index;
    thisp->scanx_begin[2] = layer_index;

    thisp->scanx_len[0] = col_size;	/* ��������ΰ���礭������¸ */
    thisp->scanx_len[1] = row_size;
    thisp->scanx_len[2] = layer_size;

    thisp->scanx_cntr[0] = 0;		/* �����󥿤Υꥻ�å�([0]��̤����) */
    thisp->scanx_cntr[1] = 0;
    thisp->scanx_cntr[2] = 0;

    /* ���߰��֤򥻥å� */
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
 * @brief  this->scanx_len[1] <= this->scanx_cntr[1] �λ�������layer�ؤ�������
 * @note   ���Υ��дؿ��� private �Ǥ���
 */
int mdarray::scan_along_x_goto_next() const
{
    int status = 1;
    mdarray *thisp = (mdarray *)this;			/* ����Ū�˥��㥹�� */

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
	/* ���Υ쥤���(�����Υ��å����񤷤���orz) */
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
 * @brief  �������Ǥ�x������ʬ���Ϣ³Ū�˥������ (�֤��ͤ�double��)
 */
double *mdarray::scan_along_x( 
			 size_t *n, ssize_t *x, ssize_t *y, ssize_t *z ) const
{
    return this->scan_along_x_via_udf( NULL, NULL, n, x, y, z );
}

/**
 * @brief  �������Ǥ�x������ʬ���Ϣ³Ū�˥������ (�֤��ͤ�double��)
 */
double *mdarray::scan_along_x_via_udf( 
		       void (*func)(const void *, void *, size_t, int, void *),
		       void *user_ptr,
		       size_t *n, ssize_t *x, ssize_t *y, ssize_t *z ) const
{
    double *ret_ptr = NULL;
    mdarray *thisp = (mdarray *)this;			/* ����Ū�˥��㥹�� */
    size_t buf_blen;

    if ( thisp->scanx_ndim == 0 ) goto quit;
    if ( thisp->scanx_len[0] == 0 ) goto quit;

    if ( thisp->scanx_len[1] <= thisp->scanx_cntr[1] ) {
	/* ���Υ쥤��� */
	if ( this->scan_along_x_goto_next() != 0 ) goto quit;	/* finish */
    }

    if ( func == NULL ) {
	/* �ؿ������� */
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
	/* �֤����ѥХåե������ */
	if ( thisp->scanx_ret_buf.allocate_aligned32(
			     buf_blen, &(thisp->scanx_aligned_ret_ptr)) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","malloc() failed");
	}
	thisp->scanx_ret_buf_blen = buf_blen;
    }

    /* double �ΥХåե��إ��ԡ� */
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
 * @brief  �������Ǥ�x������ʬ���Ϣ³Ū�˥������ (�֤��ͤ�float��)
 */
float *mdarray::scan_along_x_f( 
			 size_t *n, ssize_t *x, ssize_t *y, ssize_t *z ) const
{
    return this->scan_along_x_f_via_udf( NULL, NULL, n, x, y, z );
}

/**
 * @brief  �������Ǥ�x������ʬ���Ϣ³Ū�˥������ (�֤��ͤ�float��)
 */
float *mdarray::scan_along_x_f_via_udf( 
		       void (*func)(const void *, void *, size_t, int, void *),
		       void *user_ptr,
		       size_t *n, ssize_t *x, ssize_t *y, ssize_t *z ) const
{
    float *ret_ptr = NULL;
    mdarray *thisp = (mdarray *)this;			/* ����Ū�˥��㥹�� */
    size_t buf_blen;

    if ( thisp->scanx_ndim == 0 ) goto quit;
    if ( thisp->scanx_len[0] == 0 ) goto quit;

    if ( thisp->scanx_len[1] <= thisp->scanx_cntr[1] ) {
	/* ���Υ쥤��� */
	if ( this->scan_along_x_goto_next() != 0 ) goto quit;	/* finish */
    }

    if ( func == NULL ) {
	/* �ؿ������� */
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
	/* �֤����ѥХåե������ */
	if ( thisp->scanx_ret_buf.allocate_aligned32(
			     buf_blen, &(thisp->scanx_aligned_ret_ptr)) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","malloc() failed");
	}
	thisp->scanx_ret_buf_blen = buf_blen;
    }

    /* float �ΥХåե��إ��ԡ� */
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

/* ���: ����� mdarray::update_length_info() ����Ͽ����Τ�˺��ʤ��� */
/**
 * @brief  begin_scan_along_x() �ǳ��Ϥ���������󡦥������󥹤ν�λ
 */
void mdarray::end_scan_along_x() const
{
    mdarray *thisp = (mdarray *)this;			/* ����Ū�˥��㥹�� */

    thisp->scanx_begin = NULL;				/* ���� & ����� */
    thisp->scanx_len   = NULL;
    thisp->scanx_cntr  = NULL;
    thisp->scanx_ndim  = 0;

    thisp->scanx_ret_buf = NULL;			/* ���� & ����� */
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
 * @brief  �������Ǥ�y������ʬ��ǤΥ�����󡦥������󥹤γ���
 *
 * ���Ȥ�����λ��ꤵ�줿�ϰϤ����Ǥ���ñ�̤ǥ������(�������Υ������)����
 * ����ν����򤷤ޤ��������ǥ֥�å�ñ�̤ι�®transpose(�ǡ������쥤�ˤ�����
 * x����y���Ȥ����촹��)��Ԥʤ����ᡤ�⤤�ѥե����ޥ󥹤������ޤ���<br>
 * ��ñ�̤Υ�������Ԥʤ���硤���μ��ǰ�Ϣ��API��Ȥ��ޤ���<br>
 * - beginf_scan_along_y() �ǽ����򤹤롥<br>
 * - scan_along_y() ����ο������ƤӽФ����󤴤Ȥν�����Ԥʤ���<br>
 * - end_scan_along_y() �ǽ�λ����(����Хåե��γ���)��Ԥʤ���<br>
 * scan_along_y() ��1��ʬ�����Ǥ����ä�����Хåե��Υ��ɥ쥹(32�Х��Ȥǥ��饤
 * �󤵤�Ƥ���)���֤��ޤ������ΰ���Хåե��ϥ��֥������Ȥ������������Ǥ��ä�
 * �� double �����󶡤���ޤ����ʤ������ΥХåե��ϥ��֥������ȤǴ�������Ƥ���
 * ���ᡤ�ץ���ޤ��������ƤϤ����ޤ���<br>
 * scan_along_y() �ϻ����ϰ����ƤΥ�����󤬴�λ���Ƥ�����ˤ� NULL ���֤���
 * ����<br>
 * �����ǡ��������פ�Ȥ���ʤɤ˻Ȥ����дؿ��Ǥ���
 *
 * @param     exp_fmt ������󤹤��ϰϤ򼨤�ʸ����(��: "0:99,*")�Τ����
 *                    �ե����ޥåȻ���(printf��Ʊ��)
 * @param     ... exp_fmt���б���������Ĺ�����γ����ǥǡ���
 * @return    �������ˤ��ͭ�����Ǥο������顼�ξ������͡�
 * @note      "[1:100,*]" �Τ褦�� [] �ǰϤ�ȡ�1-indexed �Ȥ��ư����ޤ���
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
 * @brief  �������Ǥ�y������ʬ��ǤΥ�����󡦥������󥹤γ���
 *
 * ���Ȥ�����λ��ꤵ�줿�ϰϤ����Ǥ���ñ�̤ǥ������(�������Υ������)����
 * ����ν����򤷤ޤ��������ǥ֥�å�ñ�̤ι�®transpose(�ǡ������쥤�ˤ�����
 * x����y���Ȥ����촹��)��Ԥʤ����ᡤ�⤤�ѥե����ޥ󥹤������ޤ���<br>
 * ��ñ�̤Υ�������Ԥʤ���硤���μ��ǰ�Ϣ��API��Ȥ��ޤ���<br>
 * - vbeginf_scan_along_y() �ǽ����򤹤롥<br>
 * - scan_along_y() ����ο������ƤӽФ����󤴤Ȥν�����Ԥʤ���<br>
 * - end_scan_along_y() �ǽ�λ����(����Хåե��γ���)��Ԥʤ���<br>
 * scan_along_y() ��1��ʬ�����Ǥ����ä�����Хåե��Υ��ɥ쥹(32�Х��Ȥǥ��饤
 * �󤵤�Ƥ���)���֤��ޤ������ΰ���Хåե��ϥ��֥������Ȥ������������Ǥ��ä�
 * �� double �����󶡤���ޤ����ʤ������ΥХåե��ϥ��֥������ȤǴ�������Ƥ���
 * ���ᡤ�ץ���ޤ��������ƤϤ����ޤ���<br>
 * scan_along_y() �ϻ����ϰ����ƤΥ�����󤬴�λ���Ƥ�����ˤ� NULL ���֤���
 * ����<br>
 * �����ǡ��������פ�Ȥ���ʤɤ˻Ȥ����дؿ��Ǥ���
 *
 * @param     exp_fmt ������󤹤��ϰϤ򼨤�ʸ����(��: "0:99,*")�Τ����
 *                    �ե����ޥåȻ���(printf��Ʊ��)
 * @param     ap exp_fmt���б���������Ĺ�����Υꥹ��
 * @return    �������ˤ��ͭ�����Ǥο������顼�ξ������͡�
 * @note      "[1:100,*]" �Τ褦�� [] �ǰϤ�ȡ�1-indexed �Ȥ��ư����ޤ���
 */
ssize_t mdarray::vbeginf_scan_along_y( const char *exp_fmt, va_list ap ) const
{
    ssize_t ret_len = -1;
    mdarray *thisp = (mdarray *)this;			/* ����Ū�˥��㥹�� */
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

    /* �ѡ�������secinfo ������ */
    if ( parse_section_exp(&secinfo, &n_secinfo, secexp.ptr()) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","Error in parse_section_exp()");
    }
    if ( n_secinfo == 0 ) goto quit;

    /* secinfo ������å������� */
    if ( test_section_info( secexp.ptr(), *this, 
			    &secinfo, &n_secinfo, NULL ) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","Error in test_section_info()");
    }
    if ( n_secinfo == 0 ) goto quit;

    /* ����Ǥ�3�����Ȥ���(�֤���x,y,z�Τ���) */
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
 * @brief  �������Ǥ�y������ʬ��ǤΥ�����󡦥������󥹤γ���
 *
 * ���Ȥ�����λ��ꤵ�줿�ϰϤ����Ǥ���ñ�̤ǥ������(�������Υ������)����
 * ����ν����򤷤ޤ��������ǥ֥�å�ñ�̤ι�®transpose(�ǡ������쥤�ˤ�����
 * x����y���Ȥ����촹��)��Ԥʤ����ᡤ�⤤�ѥե����ޥ󥹤������ޤ���<br>
 * ��ñ�̤Υ�������Ԥʤ���硤���μ��ǰ�Ϣ��API��Ȥ��ޤ���<br>
 * - begin_scan_along_y() �ǽ����򤹤롥<br>
 * - scan_along_y() ����ο������ƤӽФ����󤴤Ȥν�����Ԥʤ���<br>
 * - end_scan_along_y() �ǽ�λ����(����Хåե��γ���)��Ԥʤ���<br>
 * scan_along_y() ��1��ʬ�����Ǥ����ä�����Хåե��Υ��ɥ쥹(32�Х��Ȥǥ��饤
 * �󤵤�Ƥ���)���֤��ޤ������ΰ���Хåե��ϥ��֥������Ȥ������������Ǥ��ä�
 * �� double �����󶡤���ޤ����ʤ������ΥХåե��ϥ��֥������ȤǴ�������Ƥ���
 * ���ᡤ�ץ���ޤ��������ƤϤ����ޤ���<br>
 * scan_along_y() �ϻ����ϰ����ƤΥ�����󤬴�λ���Ƥ�����ˤ� NULL ���֤���
 * ����<br>
 * �����ǡ��������פ�Ȥ���ʤɤ˻Ȥ����дؿ��Ǥ���
 *
 * @param     col_index �����
 * @param     col_size �󥵥���
 * @param     row_index �԰���
 * @param     row_size �ԥ�����
 * @param     layer_index �쥤�����
 * @param     layer_size �쥤�䥵����
 * @return    �������ˤ��ͭ�����Ǥο������顼�ξ������͡�
 */
ssize_t mdarray::begin_scan_along_y( 
				 ssize_t col_index, size_t col_size,
				 ssize_t row_index, size_t row_size,
				 ssize_t layer_index, size_t layer_size ) const
{
    ssize_t ret_len = -1;
    mdarray *thisp = (mdarray *)this;			/* ����Ū�˥��㥹�� */

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

    thisp->scany_begin[0] = col_index;	/* ��������ΰ�ΰ��֤���¸ */
    thisp->scany_begin[1] = row_index;
    thisp->scany_begin[2] = layer_index;

    thisp->scany_len[0] = col_size;	/* ��������ΰ���礭������¸ */
    thisp->scany_len[1] = row_size;
    thisp->scany_len[2] = layer_size;

    ret_len = this->begin_scan_along_y_common();

 quit:
    return ret_len;
}

/* private */
/**
 * @brief  *begin*_scan_along_y() �ζ�����ʬ
 * @note   ���Υ��дؿ��� private �Ǥ���
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
    mdarray *thisp = (mdarray *)this;			/* ����Ū�˥��㥹�� */

    col_size = thisp->scany_len[0];			/* ��������ΰ� */
    row_size = thisp->scany_len[1];

    /* ��������Хåե��ΥХ���Ĺ�� 16^2 ���� 128^2 ���ͤ�Ȥ�褦������ */
    /* �ܺ٤� transpose_xy_copy() �Υ�������Υ����Ȥ򻲾�             */
    min_len_block = (size_t)(1.0 + sqrt((double)(16*16) / this_bytes));
    max_len_block = (size_t)(1.0 + sqrt((double)(128*128) / this_bytes));
    if ( row_size < col_size ) min_size = row_size;
    else min_size = col_size;

    if ( min_size <= min_len_block ) len_block = min_len_block;
    else {
	size_t ndiv = ( (min_size - 1) / max_len_block ) + 1;
	len_block = min_size / ndiv;
    }

    /* ����Хåե��ι⤵ (��ǽ�ʤ���������Υ������ˤ��碌��) */
    if ( col_size < len_block ) thisp->scany_height_trans = col_size;
    else thisp->scany_height_trans = len_block;

    /* transpose�Ѱ���Хåե� */
    buf_blen = this_bytes * row_size * thisp->scany_height_trans;
    if ( thisp->scany_trans_buf_blen < buf_blen ) {
	if ( thisp->scany_trans_buf.allocate( buf_blen ) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","malloc() failed");
	}
	thisp->scany_trans_buf_blen = buf_blen;
    }

    /* transpose�����������1�դ�Ĺ�� */
    thisp->scany_len_block = len_block;

    /* ���������ѥХåե� */
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
	thisp->scany_cntr[i] = 0;	/* �����󥿤Υꥻ�å�([1]��̤����) */
	ret_len *= thisp->scany_len[i];
	if ( 2 <= i ) {
	    thisp->scany_ret_z += blk3 * thisp->scany_begin[i];
	    blk3 *= this->length(i);
	}
    }

    /* ���߰��֤򥻥å� */
    thisp->scany_cptr = (const char *)(this->data_ptr_cs())
			+ colrow_blen * thisp->scany_ret_z;	/* offset */

    /* transpose �Ѥ����ѥ��ԡ��ؿ������� */
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
 * @brief  this->scany_len[0] <= this->scany_cntr[0] �λ��Ρ�����layer�ؤ������ν���
 * @note   ���Υ��дؿ��� private �Ǥ���
 */
int mdarray::scan_along_y_goto_next() const
{
    int status = 1;
    mdarray *thisp = (mdarray *)this;			/* ����Ū�˥��㥹�� */

    size_t blen_block = this->bytes() * this->col_length();
    size_t len_b3 = 1;
    size_t dim_ix = 2;
    /* ���Υ쥤��� */
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
	/* �����Υ��å����񤷤���orz */
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
 * @brief  ��ʬŪ�� transpose ��Ԥʤ�
 * @note   ���Υ��дؿ��� private �Ǥ���
 */
void mdarray::scan_along_y_do_partial_transpose() const
{
    mdarray *thisp = (mdarray *)this;			/* ����Ū�˥��㥹�� */
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
    if ( this->rounding() == true ) f_idx = 1;	/* �ͼθ������Ѵ� */
    else f_idx = 0;					/* �ڤ�ΤƤ��Ѵ� */
    /* ��ʬŪ�� transpose ��Ԥʤ� */
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
 * @brief  �������Ǥ�y������ʬ���Ϣ³Ū�˥������ (�֤��ͤ�double��)
 */
double *mdarray::scan_along_y( 
			 size_t *n, ssize_t *x, ssize_t *y, ssize_t *z ) const
{
    return this->scan_along_y_via_udf( NULL, NULL, n, x, y, z );
}

/**
 * @brief  �������Ǥ�y������ʬ���Ϣ³Ū�˥������ (�֤��ͤ�double��)
 */
double *mdarray::scan_along_y_via_udf( 
		       void (*func)(const void *, void *, size_t, int, void *),
		       void *user_ptr,
		       size_t *n, ssize_t *x, ssize_t *y, ssize_t *z ) const
{
    double *ret_ptr = NULL;
    mdarray *thisp = (mdarray *)this;			/* ����Ū�˥��㥹�� */
    size_t idx_trans_buf;
    size_t buf_blen;

    if ( thisp->scany_ndim == 0 ) goto quit;
    if ( thisp->scany_len[1] == 0 ) goto quit;

    if ( thisp->scany_len[0] <= thisp->scany_cntr[0] ) {
	/* ���Υ쥤��� */
	if ( this->scan_along_y_goto_next() != 0 ) goto quit;	/* finish */
    }

    if ( func == NULL ) {
	/* �ؿ������� */
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
	/* �֤����ѥХåե������ */
	if ( thisp->scany_ret_buf.allocate_aligned32(
			     buf_blen, &(thisp->scany_aligned_ret_ptr)) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","malloc() failed");
	}
	thisp->scany_ret_buf_blen = buf_blen;
    }

    idx_trans_buf = (thisp->scany_cntr[0] % thisp->scany_height_trans);
    if ( idx_trans_buf == 0 ) {
	/* ��ʬŪ�� transpose ��Ԥʤ� */
	this->scan_along_y_do_partial_transpose();
    }

    /* ��ʬŪ�� transpose ���줿�Хåե����� double �ΥХåե��إ��ԡ� */
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
 * @brief  �������Ǥ�y������ʬ���Ϣ³Ū�˥������ (�֤��ͤ�float��)
 */
float *mdarray::scan_along_y_f( 
			 size_t *n, ssize_t *x, ssize_t *y, ssize_t *z ) const
{
    return this->scan_along_y_f_via_udf( NULL, NULL, n, x, y, z );
}

/**
 * @brief  �������Ǥ�y������ʬ���Ϣ³Ū�˥������ (�֤��ͤ�float��)
 */
float *mdarray::scan_along_y_f_via_udf( 
		       void (*func)(const void *, void *, size_t, int, void *),
		       void *user_ptr,
		       size_t *n, ssize_t *x, ssize_t *y, ssize_t *z ) const
{
    float *ret_ptr = NULL;
    mdarray *thisp = (mdarray *)this;			/* ����Ū�˥��㥹�� */
    size_t idx_trans_buf;
    size_t buf_blen;

    if ( thisp->scany_ndim == 0 ) goto quit;
    if ( thisp->scany_len[1] == 0 ) goto quit;

    if ( thisp->scany_len[0] <= thisp->scany_cntr[0] ) {
	/* ���Υ쥤��� */
	if ( this->scan_along_y_goto_next() != 0 ) goto quit;	/* finish */
    }

    if ( func == NULL ) {
	/* �ؿ������� */
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
	/* �֤����ѥХåե������ */
	if ( thisp->scany_ret_buf.allocate_aligned32(
			     buf_blen, &(thisp->scany_aligned_ret_ptr)) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","malloc() failed");
	}
	thisp->scany_ret_buf_blen = buf_blen;
    }

    idx_trans_buf = (thisp->scany_cntr[0] % thisp->scany_height_trans);
    if ( idx_trans_buf == 0 ) {
	/* ��ʬŪ�� transpose ��Ԥʤ� */
	this->scan_along_y_do_partial_transpose();
    }

    /* ��ʬŪ�� transpose ���줿�Хåե����� float �ΥХåե��إ��ԡ� */
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

/* ���: ����� mdarray::update_length_info() ����Ͽ����Τ�˺��ʤ��� */
/**
 * @brief  begin_scan_along_y() �ǳ��Ϥ���������󡦥������󥹤ν�λ
 */
void mdarray::end_scan_along_y() const
{
    mdarray *thisp = (mdarray *)this;			/* ����Ū�˥��㥹�� */

    thisp->scany_begin = NULL;				/* ���� & ����� */
    thisp->scany_len   = NULL;
    thisp->scany_cntr  = NULL;
    thisp->scany_ndim  = 0;

    thisp->scany_trans_buf = NULL;			/* ���� & ����� */
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
 * @brief  �������Ǥ�z������ʬ��ǤΥ�����󡦥������󥹤γ���
 *
 * ���Ȥ�����λ��ꤵ�줿�ϰϤ����Ǥ�(x,y)�ΰ���1��1�ĤˤĤ��ơ�z�����˥�
 * ����󤹤뤿��ν����򤷤ޤ��������ǥ֥�å�ñ�̤ι�®transpose(�ǡ�����
 * �쥤�ˤ�����x����z���Ȥ����촹��)��Ԥʤ����ᡤ�⤤�ѥե����ޥ󥹤�����
 * ��ޤ���<br>
 * z������������Ԥʤ���硤���μ��ǰ�Ϣ��API��Ȥ��ޤ���<br>
 * - beginf_scan_along_z() �ǽ����򤹤롥<br>
 * - scan_along_z() ��ɬ�פʿ������ƤӽФ���(x,y)���Ȥν�����Ԥʤ���<br>
 * - end_scan_along_z() �ǽ�λ����(����Хåե��γ���)��Ԥʤ���<br>
 * scan_along_z() ��(x,y)�ΰ���1�ĤˤĤ��Ƥ�z���������Ǥ����ä�����Хåե���
 * ���ɥ쥹(32�Х��Ȥǥ��饤�󤵤�Ƥ���)���֤��ޤ������ΰ���Хåե��ϥ���
 * �������Ȥ������������Ǥ��äƤ� double �����󶡤���ޤ����ʤ������ΥХåե�
 * �ϥ��֥������ȤǴ�������Ƥ��뤿�ᡤ�ץ���ޤ��������ƤϤ����ޤ���<br>
 * scan_along_z() �ϻ����ϰ����ƤΥ�����󤬴�λ���Ƥ�����ˤ� NULL ���֤���
 * ����<br>
 * �����ǡ��������פ�Ȥ���ʤɤ˻Ȥ����дؿ��Ǥ���z�������礭������ʬ�Ǥʤ�
 * ��硤�ѥե����ޥ󥹤������ʤ���������ޤ������ξ��ϡ�scan_zx_planes()
 * �򤪻����������
 *
 * @param     exp_fmt ������󤹤��ϰϤ򼨤�ʸ����(��: "0:99,*")�Τ����
 *                    �ե����ޥåȻ���(printf��Ʊ��)
 * @param     ... exp_fmt���б���������Ĺ�����γ����ǥǡ���
 * @return    �������ˤ��ͭ�����Ǥο������顼�ξ������͡�
 * @note      "[1:100,*]" �Τ褦�� [] �ǰϤ�ȡ�1-indexed �Ȥ��ư����ޤ���
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
 * @brief  �������Ǥ�z������ʬ��ǤΥ�����󡦥������󥹤γ���
 *
 * ���Ȥ�����λ��ꤵ�줿�ϰϤ����Ǥ�(x,y)�ΰ���1��1�ĤˤĤ��ơ�z�����˥�
 * ����󤹤뤿��ν����򤷤ޤ��������ǥ֥�å�ñ�̤ι�®transpose(�ǡ�����
 * �쥤�ˤ�����x����z���Ȥ����촹��)��Ԥʤ����ᡤ�⤤�ѥե����ޥ󥹤�����
 * ��ޤ���<br>
 * z������������Ԥʤ���硤���μ��ǰ�Ϣ��API��Ȥ��ޤ���<br>
 * - vbeginf_scan_along_z() �ǽ����򤹤롥<br>
 * - scan_along_z() ��ɬ�פʿ������ƤӽФ���(x,y)���Ȥν�����Ԥʤ���<br>
 * - end_scan_along_z() �ǽ�λ����(����Хåե��γ���)��Ԥʤ���<br>
 * scan_along_z() ��(x,y)�ΰ���1�ĤˤĤ��Ƥ�z���������Ǥ����ä�����Хåե���
 * ���ɥ쥹(32�Х��Ȥǥ��饤�󤵤�Ƥ���)���֤��ޤ������ΰ���Хåե��ϥ���
 * �������Ȥ������������Ǥ��äƤ� double �����󶡤���ޤ����ʤ������ΥХåե�
 * �ϥ��֥������ȤǴ�������Ƥ��뤿�ᡤ�ץ���ޤ��������ƤϤ����ޤ���<br>
 * scan_along_z() �ϻ����ϰ����ƤΥ�����󤬴�λ���Ƥ�����ˤ� NULL ���֤���
 * ����<br>
 * �����ǡ��������פ�Ȥ���ʤɤ˻Ȥ����дؿ��Ǥ���z�������礭������ʬ�Ǥʤ�
 * ��硤�ѥե����ޥ󥹤������ʤ���������ޤ������ξ��ϡ�scan_zx_planes()
 * �򤪻����������
 *
 * @param     exp_fmt ������󤹤��ϰϤ򼨤�ʸ����(��: "0:99,*")�Τ����
 *                    �ե����ޥåȻ���(printf��Ʊ��)
 * @param     ap exp_fmt���б���������Ĺ�����Υꥹ��
 * @return    �������ˤ��ͭ�����Ǥο������顼�ξ������͡�
 * @note      "[1:100,*]" �Τ褦�� [] �ǰϤ�ȡ�1-indexed �Ȥ��ư����ޤ���
 */
ssize_t mdarray::vbeginf_scan_along_z( const char *exp_fmt, va_list ap ) const
{
    ssize_t ret_len = -1;
    mdarray *thisp = (mdarray *)this;			/* ����Ū�˥��㥹�� */
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

    /* �ѡ�������secinfo ������ */
    if ( parse_section_exp(&secinfo, &n_secinfo, secexp.ptr()) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","Error in parse_section_exp()");
    }
    if ( n_secinfo == 0 ) goto quit;

    /* secinfo ������å������� */
    if ( test_section_info( secexp.ptr(), *this, 
			    &secinfo, &n_secinfo, NULL ) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","Error in test_section_info()");
    }
    if ( n_secinfo == 0 ) goto quit;

    /* ����Ǥ�3�����Ȥ���(�֤���x,y,z�Τ���) */
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
 * @brief  �������Ǥ�z������ʬ��ǤΥ�����󡦥������󥹤γ���
 *
 * ���Ȥ�����λ��ꤵ�줿�ϰϤ����Ǥ�(x,y)�ΰ���1��1�ĤˤĤ��ơ�z�����˥�
 * ����󤹤뤿��ν����򤷤ޤ��������ǥ֥�å�ñ�̤ι�®transpose(�ǡ�����
 * �쥤�ˤ�����x����z���Ȥ����촹��)��Ԥʤ����ᡤ�⤤�ѥե����ޥ󥹤�����
 * ��ޤ���<br>
 * z������������Ԥʤ���硤���μ��ǰ�Ϣ��API��Ȥ��ޤ���<br>
 * - begin_scan_along_z() �ǽ����򤹤롥<br>
 * - scan_along_z() ��ɬ�פʿ������ƤӽФ���(x,y)���Ȥν�����Ԥʤ���<br>
 * - end_scan_along_z() �ǽ�λ����(����Хåե��γ���)��Ԥʤ���<br>
 * scan_along_z() ��(x,y)�ΰ���1�ĤˤĤ��Ƥ�z���������Ǥ����ä�����Хåե���
 * ���ɥ쥹(32�Х��Ȥǥ��饤�󤵤�Ƥ���)���֤��ޤ������ΰ���Хåե��ϥ���
 * �������Ȥ������������Ǥ��äƤ� double �����󶡤���ޤ����ʤ������ΥХåե�
 * �ϥ��֥������ȤǴ�������Ƥ��뤿�ᡤ�ץ���ޤ��������ƤϤ����ޤ���<br>
 * scan_along_z() �ϻ����ϰ����ƤΥ�����󤬴�λ���Ƥ�����ˤ� NULL ���֤���
 * ����<br>
 * �����ǡ��������פ�Ȥ���ʤɤ˻Ȥ����дؿ��Ǥ���z�������礭������ʬ�Ǥʤ�
 * ��硤�ѥե����ޥ󥹤������ʤ���������ޤ������ξ��ϡ�scan_zx_planes()
 * �򤪻����������
 *
 * @param     col_index �����
 * @param     col_size �󥵥���
 * @param     row_index �԰���
 * @param     row_size �ԥ�����
 * @param     layer_index �쥤�����
 * @param     layer_size �쥤�䥵����
 * @return    �������ˤ��ͭ�����Ǥο������顼�ξ������͡�
 */
ssize_t mdarray::begin_scan_along_z( 
				 ssize_t col_index, size_t col_size,
				 ssize_t row_index, size_t row_size,
				 ssize_t layer_index, size_t layer_size ) const
{
    ssize_t ret_len = -1;
    mdarray *thisp = (mdarray *)this;			/* ����Ū�˥��㥹�� */

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

    thisp->scanz_begin[0] = col_index;	/* ��������ΰ�ΰ��֤���¸ */
    thisp->scanz_begin[1] = row_index;
    thisp->scanz_begin[2] = layer_index;

    thisp->scanz_len[0] = col_size;	/* ��������ΰ���礭������¸ */
    thisp->scanz_len[1] = row_size;
    thisp->scanz_len[2] = layer_size;

    ret_len = this->begin_scan_along_z_common();

 quit:
    return ret_len;
}

/* private */
/**
 * @brief  *begin*_scan_along_z() �ζ�����ʬ
 * @note   ���Υ��дؿ��� private �Ǥ���
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
    mdarray *thisp = (mdarray *)this;			/* ����Ū�˥��㥹�� */

    col_size = thisp->scanz_len[0];			/* ��������ΰ� */
    layer_size = thisp->scanz_len[2];

    /* ��������Хåե��ΥХ���Ĺ�� 16^2 ���� 128^2 ���ͤ�Ȥ�褦������ */
    /* �ܺ٤� transpose_xy_copy() �Υ�������Υ����Ȥ򻲾�             */
    min_len_block = (size_t)(1.0 + sqrt((double)(16*16) / this_bytes));
    max_len_block = (size_t)(1.0 + sqrt((double)(128*128) / this_bytes));
    if ( layer_size < col_size ) min_size = layer_size;
    else min_size = col_size;

    if ( min_size <= min_len_block ) len_block = min_len_block;
    else {
	size_t ndiv = ( (min_size - 1) / max_len_block ) + 1;
	len_block = min_size / ndiv;
    }

    /* ����Хåե��ι⤵ (��ǽ�ʤ���������Υ������ˤ��碌��) */
    if ( col_size < len_block ) thisp->scanz_height_trans = col_size;
    else thisp->scanz_height_trans = len_block;

    /* transpose�Ѱ���Хåե� */
    buf_blen = this_bytes * layer_size * thisp->scanz_height_trans;
    if ( thisp->scanz_trans_buf_blen < buf_blen ) {
	if ( thisp->scanz_trans_buf.allocate( buf_blen ) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","malloc() failed");
	}
	thisp->scanz_trans_buf_blen = buf_blen;
    }

    /* transpose�����������1�դ�Ĺ�� */
    thisp->scanz_len_block = len_block;

    /* ���������ѥХåե� */
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
	thisp->scanz_cntr[i] = 0;	/* �����󥿤Υꥻ�å�([2]��̤����) */
	ret_len *= thisp->scanz_len[i];
	if ( 2 <= i ) {
	    thisp->scanz_ret_z += blk3 * thisp->scanz_begin[i];
	    blk3 *= this->length(i);
	}
    }

    /* ���߰��֤򥻥å� */
    thisp->scanz_cptr = (const char *)(this->data_ptr_cs())
			+ colrow_blen * thisp->scanz_ret_z;	/* offset */

    /* transpose �Ѥ����ѥ��ԡ��ؿ������� */
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
 * @brief  this->scanz_len[0] <= this->scanz_cntr[0] �λ��Ρ�����line�ؤ������ν���
 * @note   ���Υ��дؿ��� private �Ǥ���
 */
int mdarray::scan_along_z_goto_next() const
{
    int status = 1;
    mdarray *thisp = (mdarray *)this;			/* ����Ū�˥��㥹�� */

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
	    /* �����Υ��å����񤷤���orz */
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
 * @brief  ��ʬŪ�� transpose ��Ԥʤ� (����� x-z �Ǥ�transpose)
 * @note   ���Υ��дؿ��� private �Ǥ���
 */
void mdarray::scan_along_z_do_partial_transpose() const
{
    mdarray *thisp = (mdarray *)this;			/* ����Ū�˥��㥹�� */
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
    if ( this->rounding() == true ) f_idx = 1;	/* �ͼθ������Ѵ� */
    else f_idx = 0;				/* �ڤ�ΤƤ��Ѵ� */
    /* ��ʬŪ�� transpose ��Ԥʤ� (����� x-z �Ǥ�transpose) */
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
 * @brief  �������Ǥ�z������ʬ���Ϣ³Ū�˥������ (�֤��ͤ�double��)
 */
double *mdarray::scan_along_z( 
			  size_t *n, ssize_t *x, ssize_t *y, ssize_t *z ) const
{
    return this->scan_along_z_via_udf( NULL, NULL, n, x, y, z );
}

/**
 * @brief  �������Ǥ�z������ʬ���Ϣ³Ū�˥������ (�֤��ͤ�double��)
 */
double *mdarray::scan_along_z_via_udf( 
		       void (*func)(const void *, void *, size_t, int, void *),
		       void *user_ptr,
		       size_t *n, ssize_t *x, ssize_t *y, ssize_t *z ) const
{
    double *ret_ptr = NULL;
    mdarray *thisp = (mdarray *)this;			/* ����Ū�˥��㥹�� */
    size_t idx_trans_buf;
    size_t buf_blen;

    if ( thisp->scanz_ndim == 0 ) goto quit;
    if ( thisp->scanz_len[2] == 0 ) goto quit;

    if ( thisp->scanz_len[0] <= thisp->scanz_cntr[0] ) {
	/* ���Υ饤��� */
	if ( this->scan_along_z_goto_next() != 0 ) goto quit;	/* finish */
    }

    if ( func == NULL ) {
	/* �ؿ������� */
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
	/* �֤����ѥХåե������ */
	if ( thisp->scanz_ret_buf.allocate_aligned32(
			     buf_blen, &(thisp->scanz_aligned_ret_ptr)) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","malloc() failed");
	}
	thisp->scanz_ret_buf_blen = buf_blen;
    }

    idx_trans_buf = (thisp->scanz_cntr[0] % thisp->scanz_height_trans);
    if ( idx_trans_buf == 0 ) {
	/* ��ʬŪ�� transpose ��Ԥʤ� */
	this->scan_along_z_do_partial_transpose();
    }

    /* ��ʬŪ�� transpose ���줿�Хåե����� double �ΥХåե��إ��ԡ� */
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
 * @brief  �������Ǥ�z������ʬ���Ϣ³Ū�˥������ (�֤��ͤ�float��)
 */
float *mdarray::scan_along_z_f( 
			  size_t *n, ssize_t *x, ssize_t *y, ssize_t *z ) const
{
    return this->scan_along_z_f_via_udf( NULL, NULL, n, x, y, z );
}

/**
 * @brief  �������Ǥ�z������ʬ���Ϣ³Ū�˥������ (�֤��ͤ�float��)
 */
float *mdarray::scan_along_z_f_via_udf( 
		       void (*func)(const void *, void *, size_t, int, void *),
		       void *user_ptr,
		       size_t *n, ssize_t *x, ssize_t *y, ssize_t *z ) const
{
    float *ret_ptr = NULL;
    mdarray *thisp = (mdarray *)this;			/* ����Ū�˥��㥹�� */
    size_t idx_trans_buf;
    size_t buf_blen;

    if ( thisp->scanz_ndim == 0 ) goto quit;
    if ( thisp->scanz_len[2] == 0 ) goto quit;

    if ( thisp->scanz_len[0] <= thisp->scanz_cntr[0] ) {
	/* ���Υ饤��� */
	if ( this->scan_along_z_goto_next() != 0 ) goto quit;	/* finish */
    }

    if ( func == NULL ) {
	/* �ؿ������� */
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
	/* �֤����ѥХåե������ */
	if ( thisp->scanz_ret_buf.allocate_aligned32(
			     buf_blen, &(thisp->scanz_aligned_ret_ptr)) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","malloc() failed");
	}
	thisp->scanz_ret_buf_blen = buf_blen;
    }

    idx_trans_buf = (thisp->scanz_cntr[0] % thisp->scanz_height_trans);
    if ( idx_trans_buf == 0 ) {
	/* ��ʬŪ�� transpose ��Ԥʤ� */
	this->scan_along_z_do_partial_transpose();
    }

    /* ��ʬŪ�� transpose ���줿�Хåե����� float �ΥХåե��إ��ԡ� */
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

/* ���: ����� mdarray::update_length_info() ����Ͽ����Τ�˺��ʤ��� */
/**
 * @brief  begin_scan_along_z() �ǳ��Ϥ���������󡦥������󥹤ν�λ
 */
void mdarray::end_scan_along_z() const
{
    mdarray *thisp = (mdarray *)this;			/* ����Ū�˥��㥹�� */

    thisp->scanz_begin = NULL;				/* ���� & ����� */
    thisp->scanz_len   = NULL;
    thisp->scanz_cntr  = NULL;
    thisp->scanz_ndim  = 0;

    thisp->scanz_trans_buf = NULL;			/* ���� & ����� */
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
 * @brief  �������Ǥ�zx��ñ�̤ǤΥ�����󡦥������󥹤γ���
 *
 * ���Ȥ�����λ��ꤵ�줿�ϰϤ����Ǥ�zx��ñ�̤ǥ�����󤹤뤿��ν����򤷤�
 * ���������ǥ֥�å�ñ�̤ι�®transpose(�ǡ������쥤�ˤ�����x����z���Ȥ�����
 * ����)��Ԥʤ����ᡤ�⤤�ѥե����ޥ󥹤������ޤ���<br>
 * zx��ñ�̤ǥ�������Ԥʤ���硤���μ��ǰ�Ϣ��API��Ȥ��ޤ���<br>
 * - beginf_scan_zx_planes() �ǽ����򤹤롥<br>
 * - scan_zx_planes() ��ɬ�פʿ������ƤӽФ����̤��Ȥν�����Ԥʤ���<br>
 * - end_scan_zx_planes() �ǽ�λ����(����Хåե��γ���)��Ԥʤ���<br>
 * scan_zx_planes() ��y�ΰ���1�ĤˤĤ��Ƥ�zx�̤����Ǥ����ä�����Хåե��Υ���
 * �쥹(32�Х��Ȥǥ��饤�󤵤�Ƥ���)���֤��ޤ������ΰ���Хåե��ϥ��֥�������
 * �������������Ǥ��äƤ� double �����󶡤���ޤ����ʤ������ΥХåե��ϥ��֥���
 * ���ȤǴ�������Ƥ��뤿�ᡤ�ץ���ޤ��������ƤϤ����ޤ���<br>
 * scan_zx_planes() �ϻ����ϰ����ƤΥ�����󤬴�λ���Ƥ�����ˤ� NULL ���֤�
 * �ޤ���<br>
 * �����ǡ��������פ�Ȥ���ʤɤ˻Ȥ����дؿ��Ǥ���
 *
 * @param     exp_fmt ������󤹤��ϰϤ򼨤�ʸ����(��: "0:99,*")�Τ����
 *                    �ե����ޥåȻ���(printf��Ʊ��)
 * @param     ... exp_fmt���б���������Ĺ�����γ����ǥǡ���
 * @return    �������ˤ��ͭ�����Ǥο������顼�ξ������͡�
 * @note      "[1:100,*]" �Τ褦�� [] �ǰϤ�ȡ�1-indexed �Ȥ��ư����ޤ���
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
 * @brief  �������Ǥ�zx��ñ�̤ǤΥ�����󡦥������󥹤γ���
 *
 * ���Ȥ�����λ��ꤵ�줿�ϰϤ����Ǥ�zx��ñ�̤ǥ�����󤹤뤿��ν����򤷤�
 * ���������ǥ֥�å�ñ�̤ι�®transpose(�ǡ������쥤�ˤ�����x����z���Ȥ�����
 * ����)��Ԥʤ����ᡤ�⤤�ѥե����ޥ󥹤������ޤ���<br>
 * zx��ñ�̤ǥ�������Ԥʤ���硤���μ��ǰ�Ϣ��API��Ȥ��ޤ���<br>
 * - vbeginf_scan_zx_planes() �ǽ����򤹤롥<br>
 * - scan_zx_planes() ��ɬ�פʿ������ƤӽФ����̤��Ȥν�����Ԥʤ���<br>
 * - end_scan_zx_planes() �ǽ�λ����(����Хåե��γ���)��Ԥʤ���<br>
 * scan_zx_planes() ��y�ΰ���1�ĤˤĤ��Ƥ�zx�̤����Ǥ����ä�����Хåե��Υ���
 * �쥹(32�Х��Ȥǥ��饤�󤵤�Ƥ���)���֤��ޤ������ΰ���Хåե��ϥ��֥�������
 * �������������Ǥ��äƤ� double �����󶡤���ޤ����ʤ������ΥХåե��ϥ��֥���
 * ���ȤǴ�������Ƥ��뤿�ᡤ�ץ���ޤ��������ƤϤ����ޤ���<br>
 * scan_zx_planes() �ϻ����ϰ����ƤΥ�����󤬴�λ���Ƥ�����ˤ� NULL ���֤�
 * �ޤ���<br>
 * �����ǡ��������פ�Ȥ���ʤɤ˻Ȥ����дؿ��Ǥ���
 *
 * @param     exp_fmt ������󤹤��ϰϤ򼨤�ʸ����(��: "0:99,*")�Τ����
 *                    �ե����ޥåȻ���(printf��Ʊ��)
 * @param     ap exp_fmt���б���������Ĺ�����Υꥹ��
 * @return    �������ˤ��ͭ�����Ǥο������顼�ξ������͡�
 * @note      "[1:100,*]" �Τ褦�� [] �ǰϤ�ȡ�1-indexed �Ȥ��ư����ޤ���
 */
ssize_t mdarray::vbeginf_scan_zx_planes( const char *exp_fmt, va_list ap ) const
{
    ssize_t ret_len = -1;
    mdarray *thisp = (mdarray *)this;			/* ����Ū�˥��㥹�� */
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

    /* �ѡ�������secinfo ������ */
    if ( parse_section_exp(&secinfo, &n_secinfo, secexp.ptr()) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","Error in parse_section_exp()");
    }
    if ( n_secinfo == 0 ) goto quit;

    /* secinfo ������å������� */
    if ( test_section_info( secexp.ptr(), *this, 
			    &secinfo, &n_secinfo, NULL ) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","Error in test_section_info()");
    }
    if ( n_secinfo == 0 ) goto quit;

    /* ����Ǥ�3�����Ȥ���(�֤���x,y,z�Τ���) */
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
 * @brief  �������Ǥ�zx��ñ�̤ǤΥ�����󡦥������󥹤γ���
 *
 * ���Ȥ�����λ��ꤵ�줿�ϰϤ����Ǥ�zx��ñ�̤ǥ�����󤹤뤿��ν����򤷤�
 * ���������ǥ֥�å�ñ�̤ι�®transpose(�ǡ������쥤�ˤ�����x����z���Ȥ�����
 * ����)��Ԥʤ����ᡤ�⤤�ѥե����ޥ󥹤������ޤ���<br>
 * zx��ñ�̤ǥ�������Ԥʤ���硤���μ��ǰ�Ϣ��API��Ȥ��ޤ���<br>
 * - begin_scan_zx_planes() �ǽ����򤹤롥<br>
 * - scan_zx_planes() ��ɬ�פʿ������ƤӽФ����̤��Ȥν�����Ԥʤ���<br>
 * - end_scan_zx_planes() �ǽ�λ����(����Хåե��γ���)��Ԥʤ���<br>
 * scan_zx_planes() ��y�ΰ���1�ĤˤĤ��Ƥ�zx�̤����Ǥ����ä�����Хåե��Υ���
 * �쥹(32�Х��Ȥǥ��饤�󤵤�Ƥ���)���֤��ޤ������ΰ���Хåե��ϥ��֥�������
 * �������������Ǥ��äƤ� double �����󶡤���ޤ����ʤ������ΥХåե��ϥ��֥���
 * ���ȤǴ�������Ƥ��뤿�ᡤ�ץ���ޤ��������ƤϤ����ޤ���<br>
 * scan_zx_planes() �ϻ����ϰ����ƤΥ�����󤬴�λ���Ƥ�����ˤ� NULL ���֤�
 * �ޤ���<br>
 * �����ǡ��������פ�Ȥ���ʤɤ˻Ȥ����дؿ��Ǥ���
 *
 * @param     col_index �����
 * @param     col_size �󥵥���
 * @param     row_index �԰���
 * @param     row_size �ԥ�����
 * @param     layer_index �쥤�����
 * @param     layer_size �쥤�䥵����
 * @return    �������ˤ��ͭ�����Ǥο������顼�ξ������͡�
 */
ssize_t mdarray::begin_scan_zx_planes( 
				 ssize_t col_index, size_t col_size,
				 ssize_t row_index, size_t row_size,
				 ssize_t layer_index, size_t layer_size ) const
{
    ssize_t ret_len = -1;
    mdarray *thisp = (mdarray *)this;			/* ����Ū�˥��㥹�� */

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

    thisp->scanzx_begin[0] = col_index;	/* ��������ΰ�ΰ��֤���¸ */
    thisp->scanzx_begin[1] = row_index;
    thisp->scanzx_begin[2] = layer_index;

    thisp->scanzx_len[0] = col_size;	/* ��������ΰ���礭������¸ */
    thisp->scanzx_len[1] = row_size;
    thisp->scanzx_len[2] = layer_size;

    ret_len = this->begin_scan_zx_planes_common();

 quit:
    return ret_len;
}

/**
 * @brief  *begin*_scan_zx_planes() �ζ�����ʬ
 * @note   ���Υ��дؿ��� private �Ǥ���
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
    mdarray *thisp = (mdarray *)this;			/* ����Ū�˥��㥹�� */

    col_size = thisp->scanzx_len[0];			/* ��������ΰ� */
    layer_size = thisp->scanzx_len[2];

    /* ��������Хåե��ΥХ���Ĺ�� 16^2 ���� 128^2 ���ͤ�Ȥ�褦������ */
    /* �ܺ٤� transpose_xy_copy() �Υ�������Υ����Ȥ򻲾�             */
    min_len_block = (size_t)(1.0 + sqrt((double)(16*16) / this_bytes));
    max_len_block = (size_t)(1.0 + sqrt((double)(128*128) / this_bytes));
    if ( layer_size < col_size ) min_size = layer_size;
    else min_size = col_size;

    if ( min_size <= min_len_block ) len_block = min_len_block;
    else {
	size_t ndiv = ( (min_size - 1) / max_len_block ) + 1;
	len_block = min_size / ndiv;
    }

    /* transpose�����������1�դ�Ĺ�� */
    thisp->scanzx_len_block = len_block;

    /* ���������ѥХåե� */
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
	thisp->scanzx_cntr[i] = 0;	/* �����󥿤Υꥻ�å�([0,2]��̤����) */
	ret_len *= thisp->scanzx_len[i];
	if ( 2 <= i ) {
	    thisp->scanzx_ret_z += blk3 * thisp->scanzx_begin[i];
	    blk3 *= this->length(i);
	}
    }

    /* ���߰��֤򥻥å� */
    thisp->scanzx_cptr = (const char *)(this->data_ptr_cs())
			 + colrow_blen * thisp->scanzx_ret_z;	/* offset */

    return ret_len;
}

/* private */
/**
 * @brief  this->scanzx_len[1] <= this->scanzx_cntr[1] �λ��Ρ�����zx�̤ؤ������ν���
 * @note   ���Υ��дؿ��� private �Ǥ���
 */
int mdarray::scan_zx_goto_next() const
{
    int status = 1;
    mdarray *thisp = (mdarray *)this;			/* ����Ū�˥��㥹�� */
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
	/* �����Υ��å����񤷤���orz */
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
 * @brief  �������Ǥ�zx��ñ�̤�Ϣ³Ū�˥������ (�֤��ͤ�double��)
 */
double *mdarray::scan_zx_planes( 
	   size_t *n_z, size_t *n_x, ssize_t *x, ssize_t *y, ssize_t *z ) const
{
    return this->scan_zx_planes_via_udf( NULL, NULL, n_z, n_x, x, y, z );
}

/**
 * @brief  �������Ǥ�zx��ñ�̤�Ϣ³Ū�˥������ (�֤��ͤ�double��)
 */
double *mdarray::scan_zx_planes_via_udf( 
	   void (*func_gencpy2d)(const void *, void *, 
		       size_t, size_t, size_t, size_t, size_t, size_t, void *),
	   void *user_ptr,
	   size_t *n_z, size_t *n_x, ssize_t *x, ssize_t *y, ssize_t *z ) const
{
    double *ret_ptr = NULL;
    mdarray *thisp = (mdarray *)this;			/* ����Ū�˥��㥹�� */
    size_t buf_blen;
    size_t f_idx;

    if ( thisp->scanzx_ndim == 0 ) goto quit;
    if ( thisp->scanzx_len[2] == 0 ) goto quit;

    if ( thisp->scanzx_len[1] <= thisp->scanzx_cntr[1] ) {
	/* ���� zx �̤� */
	if ( this->scan_zx_goto_next() != 0 ) goto quit;	/* finish */
    }

    buf_blen = sizeof(double) * thisp->scanzx_len[2] * thisp->scanzx_len[0];
    if ( thisp->scanzx_ret_buf_blen < buf_blen ) {
	/* �֤����ѥХåե������ */
	if ( thisp->scanzx_ret_buf.allocate_aligned32(
			    buf_blen, &(thisp->scanzx_aligned_ret_ptr)) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","malloc() failed");
	}
	thisp->scanzx_ret_buf_blen = buf_blen;
    }

    if ( func_gencpy2d == NULL ) {
	if ( this->rounding() == true ) f_idx = 1;	/* �ͼθ������Ѵ� */
	else f_idx = 0;					/* �ڤ�ΤƤ��Ѵ� */
	user_ptr = &f_idx;
	/* transpose �Ѥ����ѥ��ԡ��ؿ�������                             */
	/* (����Хåե���ͳ�Ǥʤ���ľ�� double �ΥХåե�������Ƥ��ޤ�) */
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
 * @brief  �������Ǥ�zx��ñ�̤�Ϣ³Ū�˥������ (�֤��ͤ�float��)
 */
float *mdarray::scan_zx_planes_f( 
	   size_t *n_z, size_t *n_x, ssize_t *x, ssize_t *y, ssize_t *z ) const
{
    return this->scan_zx_planes_f_via_udf( NULL, NULL, n_z, n_x, x, y, z );
}

/**
 * @brief  �������Ǥ�zx��ñ�̤�Ϣ³Ū�˥������ (�֤��ͤ�float��)
 */
float *mdarray::scan_zx_planes_f_via_udf( 
	   void (*func_gencpy2d)(const void *, void *, 
		       size_t, size_t, size_t, size_t, size_t, size_t, void *),
	   void *user_ptr,
	   size_t *n_z, size_t *n_x, ssize_t *x, ssize_t *y, ssize_t *z ) const
{
    float *ret_ptr = NULL;
    mdarray *thisp = (mdarray *)this;			/* ����Ū�˥��㥹�� */
    size_t buf_blen;
    size_t f_idx;

    if ( thisp->scanzx_ndim == 0 ) goto quit;
    if ( thisp->scanzx_len[2] == 0 ) goto quit;

    if ( thisp->scanzx_len[1] <= thisp->scanzx_cntr[1] ) {
	/* ���� zx �̤� */
	if ( this->scan_zx_goto_next() != 0 ) goto quit;	/* finish */
    }

    buf_blen = sizeof(float) * thisp->scanzx_len[2] * thisp->scanzx_len[0];
    if ( thisp->scanzx_ret_buf_blen < buf_blen ) {
	/* �֤����ѥХåե������ */
	if ( thisp->scanzx_ret_buf.allocate_aligned32(
			    buf_blen, &(thisp->scanzx_aligned_ret_ptr)) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","malloc() failed");
	}
	thisp->scanzx_ret_buf_blen = buf_blen;
    }

    if ( func_gencpy2d == NULL ) {
	if ( this->rounding() == true ) f_idx = 1;	/* �ͼθ������Ѵ� */
	else f_idx = 0;					/* �ڤ�ΤƤ��Ѵ� */
	user_ptr = &f_idx;
	/* transpose �Ѥ����ѥ��ԡ��ؿ�������                             */
	/* (����Хåե���ͳ�Ǥʤ���ľ�� float �ΥХåե�������Ƥ��ޤ�) */
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

/* ���: ����� mdarray::update_length_info() ����Ͽ����Τ�˺��ʤ��� */
/**
 * @brief  begin_scan_zx_planes() �ǳ��Ϥ���������󡦥������󥹤ν�λ
 */
void mdarray::end_scan_zx_planes() const
{
    mdarray *thisp = (mdarray *)this;			/* ����Ū�˥��㥹�� */

    thisp->scanzx_begin = NULL;				/* ���� & ����� */
    thisp->scanzx_len   = NULL;
    thisp->scanzx_cntr  = NULL;
    thisp->scanzx_ndim  = 0;

    thisp->scanzx_mtx_buf = NULL;			/* ���� & ����� */
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
 * @brief  3�������ϰϤ����ǤΤ���Υ�����󡦥������󥹤γ���
 *
 * ���Ȥ�����λ��ꤵ�줿3�������ϰϤ����Ǥ�1�٤˥�����󤹤뤿��ν�����
 * �ޤ���3������祹������Ԥʤ���硤���μ��ǰ�Ϣ��API��Ȥ��ޤ���<br>
 * - beginf_scan_a_cube() �ǽ����򤹤롥<br>
 * - scan_a_cube() ��ɬ�פʿ������ƤӽФ������塼�֤��Ȥν�����Ԥʤ���<br>
 * - end_scan_a_cube() �ǽ�λ����(����Хåե��γ���)��Ԥʤ���<br>
 * scan_a_cube() �ϻ�����ʬ�����Ǥ����ä�����Хåե��Υ��ɥ쥹(32�Х��Ȥǥ���
 * ���󤵤�Ƥ���)���֤��ޤ������ΰ���Хåե��ϡ�scan_a_cube() �ξ��ϥ���
 * �������Ȥ������������Ǥ��äƤ� double ���ǡ�scan_a_cube_f() �ξ��� float
 * �����󶡤���ޤ����ʤ������ΥХåե��ϥ��֥������ȤǴ�������Ƥ��뤿�ᡤ��
 * ����ޤ��������ƤϤ����ޤ���<br>
 * �����ǡ��������פ�Ȥ���ʤɤ˻Ȥ����дؿ��Ǥ���
 *
 * @param     exp_fmt ������󤹤��ϰϤ򼨤�ʸ����(��: "0:99,*")�Τ����
 *                    �ե����ޥåȻ���(printf��Ʊ��)
 * @param     ... exp_fmt���б���������Ĺ�����γ����ǥǡ���
 * @return    �������ˤ��ͭ�����Ǥο������顼�ξ������͡�
 * @note      "[1:100,*]" �Τ褦�� [] �ǰϤ�ȡ�1-indexed �Ȥ��ư����ޤ���
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
 * @brief  3�������ϰϤ����ǤΤ���Υ�����󡦥������󥹤γ���
 *
 * ���Ȥ�����λ��ꤵ�줿3�������ϰϤ����Ǥ�1�٤˥�����󤹤뤿��ν�����
 * �ޤ���3������祹������Ԥʤ���硤���μ��ǰ�Ϣ��API��Ȥ��ޤ���<br>
 * - beginf_scan_a_cube() �ǽ����򤹤롥<br>
 * - scan_a_cube() ��ɬ�פʿ������ƤӽФ������塼�֤��Ȥν�����Ԥʤ���<br>
 * - end_scan_a_cube() �ǽ�λ����(����Хåե��γ���)��Ԥʤ���<br>
 * scan_a_cube() �ϻ�����ʬ�����Ǥ����ä�����Хåե��Υ��ɥ쥹(32�Х��Ȥǥ���
 * ���󤵤�Ƥ���)���֤��ޤ������ΰ���Хåե��ϡ�scan_a_cube() �ξ��ϥ���
 * �������Ȥ������������Ǥ��äƤ� double ���ǡ�scan_a_cube_f() �ξ��� float
 * �����󶡤���ޤ����ʤ������ΥХåե��ϥ��֥������ȤǴ�������Ƥ��뤿�ᡤ��
 * ����ޤ��������ƤϤ����ޤ���<br>
 * �����ǡ��������פ�Ȥ���ʤɤ˻Ȥ����дؿ��Ǥ���
 *
 * @param     exp_fmt ������󤹤��ϰϤ򼨤�ʸ����(��: "0:99,*")�Τ����
 *                    �ե����ޥåȻ���(printf��Ʊ��)
 * @param     ap exp_fmt���б���������Ĺ�����Υꥹ��
 * @return    �������ˤ��ͭ�����Ǥο������顼�ξ������͡�
 * @note      "[1:100,*]" �Τ褦�� [] �ǰϤ�ȡ�1-indexed �Ȥ��ư����ޤ���
 */
ssize_t mdarray::vbeginf_scan_a_cube( const char *exp_fmt, va_list ap ) const
{
    ssize_t ret_len = -1;
    mdarray *thisp = (mdarray *)this;			/* ����Ū�˥��㥹�� */
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

    /* �ѡ�������secinfo ������ */
    if ( parse_section_exp(&secinfo, &n_secinfo, secexp.ptr()) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","Error in parse_section_exp()");
    }
    if ( n_secinfo == 0 ) goto quit;

    /* secinfo ������å������� */
    if ( test_section_info( secexp.ptr(), *this, 
			    &secinfo, &n_secinfo, NULL ) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","Error in test_section_info()");
    }
    if ( n_secinfo == 0 ) goto quit;

    /* ����Ǥ�3�����Ȥ���(�֤���x,y,z�Τ���) */
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
 * @brief  3�������ϰϤ����ǤΤ���Υ�����󡦥������󥹤γ���
 *
 * ���Ȥ�����λ��ꤵ�줿3�������ϰϤ����Ǥ�1�٤˥�����󤹤뤿��ν�����
 * �ޤ���3������祹������Ԥʤ���硤���μ��ǰ�Ϣ��API��Ȥ��ޤ���<br>
 * - begin_scan_a_cube() �ǽ����򤹤롥<br>
 * - scan_a_cube() ��ɬ�פʿ������ƤӽФ������塼�֤��Ȥν�����Ԥʤ���<br>
 * - end_scan_a_cube() �ǽ�λ����(����Хåե��γ���)��Ԥʤ���<br>
 * scan_a_cube() �ϻ�����ʬ�����Ǥ����ä�����Хåե��Υ��ɥ쥹(32�Х��Ȥǥ���
 * ���󤵤�Ƥ���)���֤��ޤ������ΰ���Хåե��ϡ�scan_a_cube() �ξ��ϥ���
 * �������Ȥ������������Ǥ��äƤ� double ���ǡ�scan_a_cube_f() �ξ��� float
 * �����󶡤���ޤ����ʤ������ΥХåե��ϥ��֥������ȤǴ�������Ƥ��뤿�ᡤ��
 * ����ޤ��������ƤϤ����ޤ���<br>
 * �����ǡ��������פ�Ȥ���ʤɤ˻Ȥ����дؿ��Ǥ���
 *
 * @param     col_index �����
 * @param     col_size �󥵥���
 * @param     row_index �԰���
 * @param     row_size �ԥ�����
 * @param     layer_index �쥤�����
 * @param     layer_size �쥤�䥵����
 * @return    �������ˤ��ͭ�����Ǥο������顼�ξ������͡�
 */
ssize_t mdarray::begin_scan_a_cube( 
				ssize_t col_index, size_t col_size,
				ssize_t row_index, size_t row_size,
				ssize_t layer_index, size_t layer_size ) const
{
    ssize_t ret_len = -1;
    mdarray *thisp = (mdarray *)this;			/* ����Ū�˥��㥹�� */

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

    thisp->scancube_begin[0] = col_index;	/* ��������ΰ�ΰ��֤���¸ */
    thisp->scancube_begin[1] = row_index;
    thisp->scancube_begin[2] = layer_index;

    thisp->scancube_len[0] = col_size;        /* ��������ΰ���礭������¸ */
    thisp->scancube_len[1] = row_size;
    thisp->scancube_len[2] = layer_size;

    thisp->scancube_cntr[0] = 0;	      /* �����󥿤Υꥻ�å�(̤����) */
    thisp->scancube_cntr[1] = 0;
    thisp->scancube_cntr[2] = 0;

    thisp->scancube_ret_z = layer_index;

 quit:
    return ret_len;
}

/* private */
/**
 * @brief  this->scancube_len[3] <= this->scancube_cntr[3] �λ��Ρ�����cube�ΰ٤ν���
 * @note   ���Υ��дؿ��� private �Ǥ���
 */
int mdarray::scan_a_cube_goto_next() const
{
    int status = 1;
    mdarray *thisp = (mdarray *)this;			/* ����Ū�˥��㥹�� */

    size_t len_b3 = 1;
    size_t dim_ix = 3;
    while ( 1 ) {
	size_t stp;
	if ( thisp->scancube_ndim <= dim_ix + 1 ) {
	    /* finish */
	    thisp->scancube_len[0] = 0;
	    goto quit;
	}
	/* �����Υ��å����񤷤���orz */
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
 * @brief  3�������ϰϤ����Ǥ�Ϣ³Ū�˥������ (�֤��ͤ�double��)
 */
double *mdarray::scan_a_cube( size_t *n_x, size_t *n_y, size_t *n_z, 
			      ssize_t *x, ssize_t *y, ssize_t *z ) const
{
    return this->scan_a_cube_via_udf( NULL, NULL, n_x, n_y, n_z, x, y, z );
}

/**
 * @brief  3�������ϰϤ����Ǥ�Ϣ³Ū�˥������ (�֤��ͤ�double��)
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
    mdarray *thisp = (mdarray *)this;			/* ����Ū�˥��㥹�� */
    const char *s_ptr0;
    double *d_ptr;
    size_t len_elements;
    size_t buf_blen;
    size_t i, j;

    if ( thisp->scancube_ndim == 0 ) goto quit;
    if ( thisp->scancube_len[0] == 0 ) goto quit;

    if ( 4 <= thisp->scancube_ndim ) {
	if ( thisp->scancube_len[3] <= thisp->scancube_cntr[3] ) {
	    /* ���� cube �� */
	    if ( this->scan_a_cube_goto_next() != 0 ) goto quit;   /* finish */
	}
    }

    if ( func == NULL ) {
	/* �ؿ������� */
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
	/* �֤����ѥХåե������ */
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
    else thisp->scancube_len[0] = 0;		/* 1�󤭤�ʤΤǥꥻ�å� */

 quit:
    return ret_ptr;
}

/**
 * @brief  3�������ϰϤ����Ǥ�Ϣ³Ū�˥������ (�֤��ͤ�float��)
 */
float *mdarray::scan_a_cube_f( size_t *n_x, size_t *n_y, size_t *n_z, 
			       ssize_t *x, ssize_t *y, ssize_t *z ) const
{
    return this->scan_a_cube_f_via_udf( NULL, NULL, n_x, n_y, n_z, x, y, z );
}

/**
 * @brief  3�������ϰϤ����Ǥ�Ϣ³Ū�˥������ (�֤��ͤ�float��)
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
    mdarray *thisp = (mdarray *)this;			/* ����Ū�˥��㥹�� */
    const char *s_ptr0;
    float *d_ptr;
    size_t len_elements;
    size_t buf_blen;
    size_t i, j;

    if ( thisp->scancube_ndim == 0 ) goto quit;
    if ( thisp->scancube_len[0] == 0 ) goto quit;

    if ( 4 <= thisp->scancube_ndim ) {
	if ( thisp->scancube_len[3] <= thisp->scancube_cntr[3] ) {
	    /* ���� cube �� */
	    if ( this->scan_a_cube_goto_next() != 0 ) goto quit;   /* finish */
	}
    }

    if ( func == NULL ) {
	/* �ؿ������� */
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
	/* �֤����ѥХåե������ */
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
    else thisp->scancube_len[0] = 0;		/* 1�󤭤�ʤΤǥꥻ�å� */

 quit:
    return ret_ptr;
}

/* ���: ����� mdarray::update_length_info() ����Ͽ����Τ�˺��ʤ��� */
/**
 * @brief  begin_scan_a_cube() �ǳ��Ϥ���������󡦥������󥹤ν�λ
 */
void mdarray::end_scan_a_cube() const
{
    mdarray *thisp = (mdarray *)this;			/* ����Ū�˥��㥹�� */

    thisp->scancube_begin = NULL;			/* ���� & ����� */
    thisp->scancube_len   = NULL;
    thisp->scancube_cntr  = NULL;
    thisp->scancube_ndim  = 0;

    thisp->scancube_ret_buf = NULL;			/* ���� & ����� */
    thisp->scancube_ret_buf_blen = 0;

    return;
}

/* */

/*
 * ����Ĺ��Хåե�Ĺ���ѹ��ˤʤä����Ρ��桼���Υ�����Хå��ؿ���
 * ��Ͽ���ޤ���<br>
 * ������Хåե�Ĺ����¾�˥ߥ顼���Ƥ����������˻Ȥ��ޤ���
 *
 * @return    ���Ȥλ���
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

/* ��ư�ꥵ�����Ϥ��ʤ� */
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
 * @brief  �桼���Υݥ����ѿ�����Ͽ
 *
 * �桼���Υݥ����ѿ��򥪥֥������Ȥ���Ͽ���ޤ���<br>
 * ���Υ��дؿ���Ȥäƥ桼���Υݥ����ѿ��Υ��ɥ쥹����Ͽ����С����֥�����
 * �Ȥ���������Хåե�����Ƭ���ɥ쥹���˥桼���Υݥ����ѿ����ݻ������Ƥ���
 * �����Ǥ��ޤ���
 *
 * @param   extptr_address �桼���Υݥ����ѿ��Υ��ɥ쥹
 * @return  ���Ȥλ���
 * @note    ������Ϳ������ˡ��ݥ����ѿ��ˡ�&�פ�Ĥ���Τ�˺��ʤ��褦��
 *          ���ޤ��礦
 * @attention  �桼���Υݥ����ѿ�����Ͽ����� shallow copy ����ǽ���ʤ��ʤ�
 *             �ޤ���
 */
mdarray &mdarray::register_extptr( void *extptr_address )
{
    this->extptr_rec = (void **)extptr_address;
    if ( this->extptr_rec != NULL ) *(this->extptr_rec) = this->data_ptr();
    return *this;
}

/**
 * @brief  �桼���Υݥ����ѿ�(2d��)����Ͽ
 *
 * �桼���Υݥ����ѿ�(2d��)�򥪥֥������Ȥ���Ͽ���ޤ���<br>
 * ���Υ��дؿ���Ȥäƥ桼���Υݥ����ѿ��Υ��ɥ쥹����Ͽ����С����֥�����
 * �Ȥ��������륢�ɥ쥹�ơ��֥����Ƭ���ɥ쥹���˥桼���Υݥ����ѿ����ݻ�
 * �����Ƥ��������Ǥ��ޤ���
 *
 * @param   extptr2d_address �桼���Υݥ����ѿ��Υ��ɥ쥹
 * @return  ���Ȥλ���
 * @note    ������Ϳ������ˡ��ݥ����ѿ��ˡ�&�פ�Ĥ���Τ�˺��ʤ��褦��
 *          ���ޤ��礦
 * @attention  �桼���Υݥ����ѿ�����Ͽ����� shallow copy ����ǽ���ʤ��ʤ�
 *             �ޤ���
 */
mdarray &mdarray::register_extptr_2d( void *extptr2d_address )
{
    this->extptr_2d_rec = (void ***)extptr2d_address;
    if ( this->extptr_2d_rec != NULL ) this->update_arr_ptr_2d();
    else this->free_arr_ptr_2d();
    return *this;
}

/**
 * @brief  �桼���Υݥ����ѿ�(3d��)����Ͽ
 *
 * �桼���Υݥ����ѿ�(3d��)�򥪥֥������Ȥ���Ͽ���ޤ���<br>
 * ���Υ��дؿ���Ȥäƥ桼���Υݥ����ѿ��Υ��ɥ쥹����Ͽ����С����֥�����
 * �Ȥ��������륢�ɥ쥹�ơ��֥����Ƭ���ɥ쥹���˥桼���Υݥ����ѿ����ݻ���
 * ���Ƥ��������Ǥ��ޤ���
 *
 * @param   extptr3d_address �桼���Υݥ����ѿ��Υ��ɥ쥹
 * @return  ���Ȥλ���
 * @note    ������Ϳ������ˡ��ݥ����ѿ��ˡ�&�פ�Ĥ���Τ�˺��ʤ��褦��
 *          ���ޤ��礦
 * @attention  �桼���Υݥ����ѿ�����Ͽ����� shallow copy ����ǽ���ʤ��ʤ�
 *             �ޤ���
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
 * @brief  ���Ȥ�����Ȼ��ꤵ�줿���֥������Ȥ�����Ȥ����
 *
 * ���ȤȻ��ꤵ�줿���֥�������obj ���������������ɤ������֤��ޤ���<br>
 * ����η����ۤʤäƤ�����Ĺ���ͤ���������п�(true)���ۤʤ�е�(false) ��
 * �֤��ޤ���
 *
 * @param     obj mdarray ���饹�Υ��֥�������
 * @return    true : ���󥵥��������Ǥ��ͤ����פ������<br>
 *            false : ���󥵥��������Ǥ��ͤ��԰��פǤ�����
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

    /* �礭������Ȥ� */
    if ( this->dim_length() < obj.dim_length() ) dim_len = obj.dim_length();
    else dim_len = this->dim_length();

    for ( i = 0 ; i < dim_len ; i++ ) {
	if ( this->length(i) != obj.length(i) ) return false;
    }

    if ( this->length() == 0 && obj.length() == 0 ) return true;
    if ( this->length() == 0 ) return false;
    if ( obj.length() == 0 ) return false;

    /* ���쥤�η��� double ���Ѵ�����ؿ� */
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
	double d0[1024];				/* ����ΰ� */
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
 * @brief  ���Ȥη��˱������Х��ȥ�������Ĵ��
 *
 * ���Υ��дؿ��ϡ����Ȥ������Х��ʥ�ǡ����Ȥ��ƥե��������¸����������
 * ���뤤�ϥե�����ΥХ��ʥ�ǡ����򼫿Ȥ�����˼����ߤ������˻Ȥ��ޤ���<br>
 * (�ܺ٤� reverse_byte_order( bool, ssize_t, size_t, size_t ) �򻲾�)
 *
 * @param     is_little_endian 1 ���ܤν�����Υ����Υ���ǥ�����
 * @return    ���Ȥλ���
 * @note      ��®���Τ��ᡤSIMD̿���ȤäƤ��ޤ���
 */
mdarray &mdarray::reverse_endian( bool is_little_endian )
{
    return this->reverse_byte_order(is_little_endian, 0,
				    0, this->cached_length_rec);
}

/**
 * @brief  ���Ȥη��˱������Х��ȥ�������Ĵ��
 *
 * ���Υ��дؿ��ϡ����Ȥ������Х��ʥ�ǡ����Ȥ��ƥե��������¸����������
 * ���뤤�ϥե�����ΥХ��ʥ�ǡ����򼫿Ȥ�����˼����ߤ������˻Ȥ��ޤ���<br>
 * (�ܺ٤� reverse_byte_order( bool, ssize_t, size_t, size_t ) �򻲾�)
 *
 * @param     is_little_endian 1 ���ܤν�����Υ����Υ���ǥ�����
 * @param     begin �����򳫻Ϥ��������ֹ�
 * @param     length �����оݤȤʤ�Ĺ��
 * @return    ���Ȥλ���
 * @note      ��®���Τ��ᡤSIMD̿���ȤäƤ��ޤ���
 */
mdarray &mdarray::reverse_endian( bool is_little_endian,
				  size_t begin, size_t length )
{
    return this->reverse_byte_order(is_little_endian, 0, begin, length);
}

/**
 * @brief  Ǥ�դη��ΥХ��ȥ�������Ĵ��
 *
 * ���Υ��дؿ��ϡ����Ȥ������Х��ʥ�ǡ����Ȥ��ƥե��������¸����������
 * ���뤤�ϥե�����ΥХ��ʥ�ǡ����򼫿Ȥ�����˼����ߤ������˻Ȥ��ޤ���<br>
 * (�ܺ٤� reverse_byte_order( bool, ssize_t, size_t, size_t ) �򻲾�)
 *
 * @param     is_little_endian 1 ���ܤν�����Υ����Υ���ǥ�����
 * @param     sz_type ������
 * @return    ���Ȥλ���
 * @note      ��®���Τ��ᡤSIMD̿���ȤäƤ��ޤ���
 */
mdarray &mdarray::reverse_byte_order( bool is_little_endian, ssize_t sz_type )
{
    return this->reverse_byte_order(is_little_endian, sz_type, 
				    0, this->cached_length_rec);
}

/**
 * @brief  Ǥ�դη��ΥХ��ȥ�������Ĵ��
 *
 * ���Υ��дؿ��ϡ����Ȥ������Х��ʥ�ǡ����Ȥ��ƥե��������¸����������
 * ���뤤�ϥե�����ΥХ��ʥ�ǡ����򼫿Ȥ�����˼����ߤ������˻Ȥ��ޤ���<br>
 * �ե�����˥ǡ�������¸���������ϡ����Υ��дؿ���ƤӽФ��ƥե�������¸��Ŭ
 * ��������ǥ�������Ѵ�����data_ptr() ���дؿ��ʤɤǼ����������ɥ쥹�򥹥�
 * �꡼��񤭹����Ѥδؿ���Ϳ�������Ƥ�񤭹�����塤���٤��Υ��дؿ���Ƥӽ�
 * ���ơ�����ǥ�����򸵤��ᤷ�ޤ���<br>
 * �ե����뤫��ǡ������ɤ߹��ߤ������ϡ�data_ptr() ���дؿ��ʤɤǼ���������
 * �ɥ쥹�򥹥ȥ꡼���ɤ߼���Ѥδؿ���Ϳ�������Ƥ��ɤ߹�����塤���Υ��дؿ�
 * ��ƤӽФ��ƽ����Ϥ�Ŭ��������ǥ�������Ѵ����ޤ���<br>
 * �嵭�Τ�����ξ��⡤�ե��������¸�����٤��ǡ������ӥå�����ǥ�����ʤ�
 * �С���1������ false �򥻥åȤ��ޤ�(��ȥ륨��ǥ�����ʤ� true �Ǥ�)��<br>
 * ���Υ��дؿ��ϡ������Ϥˤ�äƻȤ�ʬ����ɬ�פȤʤ�ʤ��褦�˺���Ƥ���
 * �����㤨�С��ե�����˥ӥå�����ǥ�����Υǡ�������¸�������Τǡ�
 * is_little_endian �� false ����ꤷ�����Υ��дؿ���ƤӽФ����Ȥ��ޤ���
 * ���λ����ޥ��󤬥ӥå�����ǥ�����Ǥ���С��ºݤˤ�ȿž�����ϹԤ��ޤ���
 * (�ޥ��󤬥�ȥ륨��ǥ�����Ǥ���С�ȿž�������Ԥ��ޤ�)��
 * ���ˡ����֥���������ΥХ��ʥ�ǡ����򤽤Τޤޥե��������¸����С�
 * ���ꤵ�줿�Х��ȥ��������ΥХ��ʥ�ե����뤬�Ǥ��ޤ���
 * ���θ塤����Ʊ�������Ǥ��Υ��дؿ���ƤӽФ��ơ�����ǥ�����ȿž����Ƥ�
 * ����ϸ����᤹������Ԥ��ޤ���<br>
 * ���äơ��ե�����ؤ���¸�˺ݤ��Ƥϡ����Υ��дؿ���Ʊ�������ǣ���ƤӽФ���
 * ������Ǥ���
 *
 * @param     is_little_endian 1 ���ܤν�����Υ����Υ���ǥ�����
 * @param     sz_type ������
 * @param     begin �����򳫻Ϥ��������ֹ�
 * @param     length �����оݤȤʤ�Ĺ�� (���Ȥη���ñ�̤Ȥ������ǿ�)
 * @return    ���Ȥλ���
 * @note      ��®���Τ��ᡤSIMD̿���ȤäƤ��ޤ���
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
 * @brief  ���Ȥη��Ȼ��ꤵ�줿�� szt �Ȥα黻���Ԥʤ�줿���η����֤�
 *
 * @param     szt ��
 * @return    �黻���Ԥʤ�줿���η�
 *            
 */
ssize_t mdarray::ope_size_type( ssize_t szt ) const
{
    return get_sz_type_from_two(this->size_type(), szt);
}

/**
 * @brief  �ƥ�ݥ�ꥪ�֥������ȤΤ����shallow copy°������Ϳ
 *
 *  ���Ȥ���=�ױ黻�Ҥޤ��� init(obj) �ΰ�����Ϳ����줿���� shallow copy ��
 *  ���Ĥ��������Ԥʤ��ޤ�����������ϡ��ؿ��ޤ��ϥ��дؿ��ˤ�ä��֤���
 *  ��ƥ�ݥ�ꥪ�֥������ȤˤΤ���Ϳ�����٤���ΤǤ���<br>
 *
 *  mdarray ���饹�� shallow copy �β���Υ����ߥ󥰤ϡ�������Ф��뤢����
 *  �ɤ߽񤭤Ǥ��äơֽ񤭹��ߡפǤϤ���ޤ����ɤ߼�����ѤΥ��дؿ�������
 *  ���줿���⡤shallow copy ����Τ���� deep copy ������ޤ����������äơ�
 *  �ƥ�ݥ�ꥪ�֥������Ȱʳ��ξ��� set_scopy_flag() ��ȤäƤ�������̣��
 *  ����ޤ���
 *
 */
/* ���: ���Ȥ��֤������Ȥ��������return ʸ�Ǥ������ȥ��󥹥��� */
/*       ��2�󥳥ԡ������Ȥ����������ʻ��ˤʤ�Τ� void �ˤʤäƤ��� */
void mdarray::set_scopy_flag()
{
    this->shallow_copy_ok = true;
    return;
}

/**
 * @brief  ���ޤ��Ĺ��ʸ����κǸ����ʬ���...�פ��ִ� (������)
 *
 * @note   dprint() ���дؿ��ǻȤ��롥<br>
 *         private �ʴؿ��Ǥ���
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
 * @brief  ���֥������Ⱦ����ɸ�२�顼���Ϥؤν���
 *
 *  ���ȤΥ��֥������Ⱦ����ɸ�२�顼���Ϥؽ��Ϥ��ޤ���<br>
 *  �桼�����ץ����ΥǥХå�����Ū�Ȥ������дؿ��Ǥ���
 *
 * @param  msg �ǥХå�������Ϥ���Ƭ��ɽ�������ʸ����
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
      /* ��äĤ��Ż� */
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
		    /* 16�ʤǥ���� */
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
 * @brief  mdarray �α黻 (���ߤ�̤����)
 *
 *  ���Ȥ򡤻��ꤵ�줿�黻�Ѵؿ�(func��funcf��funcl)�ˤ��黻����
 *  ���η�̤��֤���
 *
 * @param      func    double ���α黻�Ѵؿ��ؤΥ��ɥ쥹
 * @param      funcf   float ���α黻�Ѵؿ��ؤΥ��ɥ쥹
 * @param      funcl   long double ���α黻�Ѵؿ��ؤΥ��ɥ쥹
 * @return     ���Ȥλ���
 * @note       �ؿ��ƤӽФ��Υ����ХإåɤΤ��ᡤ���ߤϻȤ��Ƥ��ʤ���
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
 * @brief  mdarray �ɤ����α黻 (���ߤ�̤����)
 *
 *  ���Ȥȡ�Ϳ����줿 mdarray ���֥������ȤˤĤ��ơ����ꤵ�줿�黻�Ѵؿ�
 *  (func��funcf��funcl)�ˤ��黻�������η�̤��֤���
 *
 * @param     obj     �黻���� mdarray ���֥������� 
 * @param     obj_1st �黻���� obj �����ΰ��֡�
 *                    �Ĥޤꡤx,y�ΰ�����func(x,y) or func(y,x)��<br>
 *                    true �ξ����裱������ obj �����ꤵ���<br>
 * @param     func    double ���α黻�Ѵؿ��ؤΥ��ɥ쥹
 * @param     funcf   float ���α黻�Ѵؿ��ؤΥ��ɥ쥹
 * @param     funcl   long double ���α黻�Ѵؿ��ؤΥ��ɥ쥹
 * @return    ���Ȥλ���
 * @throw     ʣ�ǿ����ʳ��� obj ���������ꤵ�줿���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
 * @note      ������r_calc2����Ѥ��ޤ�.
 *            �ؿ��ƤӽФ��Υ����ХإåɤΤ��ᡤ���ߤϻȤ��Ƥ��ʤ���
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

    /* ʣ�ǿ��ξ����㳰���֤� */

    /* ʣ�ǿ��� */
    if ( this_szt == FCOMPLEX_ZT || this_szt == DCOMPLEX_ZT ||
	 this_szt == LDCOMPLEX_ZT ) {
	err_throw(__FUNCTION__,"ERROR","complex type cannot be used");
    }
    /* �¿��� */
    else if ( this_szt == FLOAT_ZT || this_szt == DOUBLE_ZT ||
	      this_szt == LDOUBLE_ZT || this_szt == UCHAR_ZT ||
	      this_szt == INT16_ZT || this_szt == INT32_ZT ||
	      this_szt == INT64_ZT ) {
	/* NO PROBLEM */
    }
    /* ����¾�η� */
    else {
	err_throw(__FUNCTION__,"ERROR","unsupported type");
    }

    /* ʣ�ǿ��� */
    if ( obj_szt == FCOMPLEX_ZT || obj_szt == DCOMPLEX_ZT ||
	 obj_szt == LDCOMPLEX_ZT ) {
	err_throw(__FUNCTION__,"ERROR","complex type cannot be used");
    }
    /* �¿��� */
    else if ( obj_szt == FLOAT_ZT || obj_szt == DOUBLE_ZT ||
	      obj_szt == LDOUBLE_ZT || obj_szt == UCHAR_ZT ||
	      obj_szt == INT16_ZT || obj_szt == INT32_ZT ||
	      obj_szt == INT64_ZT ) {
	/* NO PROBLEM */
    }
    /* ����¾�η� */
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

    /* �ؿ������� */
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
    if ( obj.sz_type_rec == org_sz_type && this->sz_type_rec == new_sz_type ) { \
	func_calc = &_calc2::fncname; \
    }
    SLI__MDARRAY__DO_OPERATION_2TYPES(SEL_FUNC,,,,,,,,,,,,,else);
#undef SEL_FUNC

    /* �ؿ��������������Ԥʤ� */
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

	/* �礭������Ȥ� */
	ndim = this->dim_length();
	if ( ndim < obj.dim_length() ) ndim = obj.dim_length();

	/* �ǽ�ϡ���������������ԥ�������� len_xxx �˥��åȤ��� */
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
 * @brief  mdarray �α黻 (���ߤ�̤����)
 *
 *  ���Ȥ򡤻��ꤵ�줿�黻�Ѵؿ�(func��funcf��funcl)�ˤ��黻����
 *  ���η�̤��֤���
 *
 * @param      func    dcomplex ���α黻�Ѵؿ��ؤΥ��ɥ쥹
 * @param      funcf   fcomplex ���α黻�Ѵؿ��ؤΥ��ɥ쥹
 * @param      funcl   ldcomplex ���α黻�Ѵؿ��ؤΥ��ɥ쥹
 * @return     ���Ȥλ���
 * @note       �ؿ��ƤӽФ��Υ����ХإåɤΤ��ᡤ���ߤϻȤ��Ƥ��ʤ���
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
 * @brief  mdarray �ɤ����α黻 (���ߤ�̤����)
 *
 *  ���Ȥȡ�Ϳ����줿 mdarray ���֥������ȤȤ򡤻��ꤵ�줿�黻�Ѵؿ�
 *  (func��funcf��funcl)�ˤ��黻�������η�̤��֤���
 *
 * @param      obj     �黻���� mdarray ���֥������� 
 * @param      obj_1st �黻���� obj �����ΰ��֡�
 *                     true �ξ����裱������ obj �����ꤵ���
 * @param      func    dcomplex ���α黻�Ѵؿ��ؤΥ��ɥ쥹
 * @param      funcf   fcomplex ���α黻�Ѵؿ��ؤΥ��ɥ쥹
 * @param      funcl   ldcomplex ���α黻�Ѵؿ��ؤΥ��ɥ쥹
 * @throw              ʣ�ǿ����ʳ��� obj ���������ꤵ�줿���
 * @return     ���Ȥλ���
 * @note       �ؿ��ƤӽФ��Υ����ХإåɤΤ��ᡤ���ߤϻȤ��Ƥ��ʤ���
 */
mdarray &mdarray::calcx2(const mdarray &obj, bool obj_1st,
			 dcomplex (*func)(dcomplex,dcomplex),
			 fcomplex (*funcf)(fcomplex,fcomplex),
			 ldcomplex (*funcl)(ldcomplex,ldcomplex))
{
    const ssize_t this_szt =  this->size_type();
    const ssize_t obj_szt = obj.size_type();

    /* �¿��ξ����㳰���֤� */

    /* ʣ�ǿ��� */
    if ( this_szt == FCOMPLEX_ZT || this_szt == DCOMPLEX_ZT ||
	 this_szt == LDCOMPLEX_ZT ) {
	/* NO PROBLEM */
    }
    /* �¿��� */
    else if ( this_szt == FLOAT_ZT || this_szt == DOUBLE_ZT ||
	      this_szt == LDOUBLE_ZT || this_szt == UCHAR_ZT ||
	      this_szt == INT16_ZT || this_szt == INT32_ZT ||
	      this_szt == INT64_ZT ) {
	err_throw(__FUNCTION__,"ERROR","real type cannot be used");
    }
    /* ����¾�η� */
    else {
	err_throw(__FUNCTION__,"ERROR","unsupported type");
    }

    /* ʣ�ǿ��� */
    if ( obj_szt == FCOMPLEX_ZT || obj_szt == DCOMPLEX_ZT ||
	 obj_szt == LDCOMPLEX_ZT ) {
	/* NO PROBLEM */
    }
    /* �¿��� */
    else if ( obj_szt == FLOAT_ZT || obj_szt == DOUBLE_ZT ||
	      obj_szt == LDOUBLE_ZT || obj_szt == UCHAR_ZT ||
	      obj_szt == INT16_ZT || obj_szt == INT32_ZT ||
	      obj_szt == INT64_ZT ) {
	err_throw(__FUNCTION__,"ERROR","real type cannot be used");
    }
    /* ����¾�η� */
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

    /* �ؿ������� */
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type) \
    if ( obj.sz_type_rec == org_sz_type && this->sz_type_rec == new_sz_type ) { \
	func_calc = &_calcx2::fncname; \
    }
    SLI__MDARRAY__DO_OPERATION_2TYPES_COMPLEX(SEL_FUNC,else);
#undef SEL_FUNC

    /* �ؿ��������������Ԥʤ� */
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

	/* �礭������Ȥ� */
	ndim = this->dim_length();
	if ( ndim < obj.dim_length() ) ndim = obj.dim_length();

	/* �ǽ�ϡ���������������ԥ�������� len_xxx �˥��åȤ��� */
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
 * @brief  class_level_rec ���ͤμ���
 * 
 * @return  class_level_rec����
 * @note    ���Υ��дؿ��� protected �Ǥ���
 */
int mdarray::class_level() const
{
    return this->class_level_rec;
}

/**
 * @brief  �Ѿ����饹�ˤ����롤�ǥե���ȷ�������
 *
 *  �ǥե���Ȥη����ѹ���������硤�Ѿ����饹�ǥ����С��饤�ɤ��ޤ�
 * 
 * @return  ��� 1
 * @note    ���Υ��дؿ��� protected �Ǥ���
 */
ssize_t mdarray::default_size_type()
{
    return 1;
}

/**
 * @brief  �Ѿ����饹�ˤ����롤���������ǽ�ʷ�������
 *
 *  ����η��ʳ��ϵ��ݤ�������硤�Ѿ����饹�ǥ����С��饤�ɤ��ޤ���
 * 
 * @param   sz_type
 * 
 * @return  ��� true
 * @note    ���Υ��дؿ��� protected �Ǥ���
 */
bool mdarray::is_acceptable_size_type( ssize_t sz_type )
{
    return true;
}

/**
 * @brief  �����������ǿ�������������ι���
 *
 *  �����������ǿ�����Ĺ������򥭥�å��夷�Ƥ��� private ���Ф򹹿�
 *  ���ޤ���<br>
 *  �Ƽ�Ĺ��������������ɬ���ƤӤ������Τǡ��Ѿ��������饹�ǥ����Х饤��
 *  ����ȡ��Ƽ�Ĺ���Υߥ顼��󥰤���ǽ�Ǥ���
 * 
 * @note  ���Υ��дؿ��� protected �Ǥ���
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
     * scan_along_{x,y,z} �Ѿ���ν����
     */
    this->end_scan_along_x();
    this->end_scan_along_y();
    this->end_scan_along_z();
    this->end_scan_zx_planes();
    this->end_scan_a_cube();


    /* ���Ѵ��Τ���δؿ������� (���Ȥη���double) */
    this->func_cnv_nd_x2d = NULL;
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
    if ( this->sz_type_rec == org_sz_type && DOUBLE_ZT == new_sz_type ) { \
	this->func_cnv_nd_x2d = &cnv_nd::fncname; \
    }
    SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(SEL_FUNC,,,,,,,,,,,,,,,,,,,,,,,,,,,,else);
#undef SEL_FUNC

    /* ���Ѵ��Τ���δؿ������� (double�����Ȥη�) */
    this->func_cnv_nd_d2x = NULL;
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
    if ( DOUBLE_ZT == org_sz_type && this->sz_type_rec == new_sz_type ) { \
	this->func_cnv_nd_d2x = &cnv_nd::fncname; \
    }
    SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(SEL_FUNC,,,,,,,,,,,,,,,,,,,,,,,,,,,,else);
#undef SEL_FUNC

    /* ���Ѵ��Τ���δؿ������� (���Ȥη���float) */
    this->func_cnv_nd_x2f = NULL;
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
    if ( this->sz_type_rec == org_sz_type && FLOAT_ZT == new_sz_type ) { \
	this->func_cnv_nd_x2f = &cnv_nd::fncname; \
    }
    SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(SEL_FUNC,,,,,,,,,,,,,,,,,,,,,,,,,,,,else);
#undef SEL_FUNC


    /* ���������ȿž������ؿ������������ */
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
 * @brief  �����Ĺ���ѹ� (���٥�)
 * 
 *  ���Ȥ����������Ĺ�����ѹ����ޤ�.
 * 
 * @param   dim_index �����оݤμ����ֹ�
 * @param   len �ѹ�������ǿ�
 * @param   clr �������̵ͭ�λ���
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 * @note    ���Υ��дؿ��� private �Ǥ���
 */
mdarray &mdarray::do_resize( size_t dim_index, size_t len, bool clr )
{
    const size_t bytes = this->bytes();
    size_t i;
    size_t unit, new_unit, old_unit;
    size_t count;

    //if ( len == 0 ) {
	/* 1�����ξ��ǡ�dim_index==0�ξ�� */
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
    
    /* �������ʤä���� */
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
    /* �礭���ʤä���� */
    else {
	/* old |--|--|--| */
	/* new |----|----|----| */
	/* */
	/* step 1:  |--|--|==| */
	/*          |--|--|????|==??| */
	/* */
	/* step 2:  |--|==|????|--??| */
	/*          |--???|==??|--??| */
	/* �ޤ����Ƴ��� */
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

	    /* ��������ʬ�� zero �Ǥ��᤿�� */
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
    /* update_length_info() ��ľ��˸Ƥ�ɬ�פ����� */
    this->update_arr_ptr_2d();
    this->update_arr_ptr_3d();

    return *this;
}

/**
 * @brief  ���Ǥ����� (���٥�)
 * 
 *  ���ꤵ�줿���������ǰ��� idx �� len �Ĥ����Ǥ��������ޤ���
 * 
 * @param   dim_index �����оݤμ����ֹ�
 * @param   idx �������֤������ֹ�
 * @param   len �����Ŀ�
 * @param   clr �������̵ͭ�λ���
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 * @note    ���Υ��дؿ��� private �Ǥ���
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
 * @brief  mdarray �ɤ����α黻 (�Ƶ���)
 *
 *  ���Ȥȡ�Ϳ����줿 mdarray �Ȥ򡤻��ꤵ�줿�黻�Ѵؿ�(func��funcf��funcl)��
 *  ���Ƶ�Ū�˱黻�������η�̤��֤���
 *
 * @param      obj       �黻���� mdarray ���֥������� 
 * @param      obj_1st   �黻���� obj �����ΰ��֡�
 *                       �Ĥޤꡤx,y�ΰ�����func(x,y) or func(y,x)��<br>
 *                       true �ξ����裱������ obj �����ꤵ���
 * @param      dim_idx   ���ߤμ����ֹ�
 * @param      len_src   src �ˤĤ��Ƥ� �����������Ĺ��
 * @param      len_dest  dest �ˤĤ��Ƥ� �����������Ĺ��
 * @param      pos_src   src �ˤĤ��Ƥθ��ߤΥݥ������
 * @param      pos_dest  dest �ˤĤ��Ƥθ��ߤΥݥ������
 * @param      func      double ���α黻�Ѵؿ��ؤΥ��ɥ쥹
 * @param      funcf     float ���α黻�Ѵؿ��ؤΥ��ɥ쥹
 * @param      funcl     long double ���α黻�Ѵؿ��ؤΥ��ɥ쥹
 * @param      func_calc ����Ū�˱黻���뤿��δؿ��ؤΥ��ɥ쥹
 * @return     ���Ȥλ���
 * @note       ���δؿ��ϺƵ�Ū�˸ƤФ�ޤ���<br>
 *             ���Υ��дؿ��� private �Ǥ���
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
	/* default �ͤ��Ѥ����׻��򤹤� */
	for ( ; i < len_large ; i++ ) {
	    this->r_calc2( obj, obj_1st, dim_idx-1, 0, len_dest,
			   pos_src + 0 * i, pos_dest + len_dest * i,
			   func, funcf, funcl, func_calc);
	}
    }
    else {	/* ����=0 �Ǥν��� */
	const size_t src_bytes = obj.bytes();
	const size_t dest_bytes = this->bytes();
	char *dest = (char *)(this->data_ptr()) + dest_bytes * pos_dest;
	const char *src =
	    (const char *)(obj.data_ptr_cs()) + src_bytes * pos_src;
	/* �ؿ�����ư����������2�Ĥ������ʤΤǡ�    */
	/* ���٤Ƥ��ȹ礻�ˤĤ��Ʒ׻�����ɬ�פ����� */
	(*func_calc)((const void *)src, (void *)dest, obj_1st, len,
		     obj.default_value_ptr(), func,funcf,funcl);
	dest += dest_bytes * len;
	/* default �ͤ��Ѥ����׻��򤹤� */
	(*func_calc)(NULL, (void *)dest, obj_1st, len_large - len,
		     obj.default_value_ptr(), func,funcf,funcl);
    }
    return *this;
}

/**
 * @brief  mdarray �ɤ����α黻 (�Ƶ���)
 *
 *  ���Ȥȡ�Ϳ����줿 mdarray ���֥������ȤȤ򡤻��ꤵ�줿�黻�Ѵؿ�
 *  (func��funcf��funcl)�ˤ��Ƶ�Ū�˱黻�������η�̤��֤���
 *
 * @param      obj       �黻���� mdarray ���֥������� 
 * @param      obj_1st   �黻���� obj �����ΰ��֡�
 *                       �Ĥޤꡤx,y�ΰ�����func(x,y) or func(y,x)��<br>
 *                       true �ξ����裱������ obj �����ꤵ���
 * @param      dim_idx   ���ߤμ����ֹ�
 * @param      len_src   src �ˤĤ��Ƥ� �����������Ĺ��
 * @param      len_dest  dest �ˤĤ��Ƥ� �����������Ĺ��
 * @param      pos_src   src �ˤĤ��Ƥθ��ߤΥݥ������
 * @param      pos_dest  dest �ˤĤ��Ƥθ��ߤΥݥ������
 * @param      func      dcomplex ���α黻�Ѵؿ��ؤΥ��ɥ쥹
 * @param      funcf     fcomplex ���α黻�Ѵؿ��ؤΥ��ɥ쥹
 * @param      funcl     ldcomplex ���α黻�Ѵؿ��ؤΥ��ɥ쥹
 * @param      func_calc ����Ū�˱黻���뤿��δؿ��ؤΥ��ɥ쥹
 * @return     ���Ȥλ���
 * @note       ���δؿ��ϺƵ�Ū�˸ƤФ�ޤ���<br>
 *             ���Υ��дؿ��� private �Ǥ���
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
	/* default �ͤ��Ѥ����׻��򤹤� */
	for ( ; i < len_large ; i++ ) {
	    this->r_calcx2( obj, obj_1st, dim_idx-1, 0, len_dest,
			    pos_src + 0 * i, pos_dest + len_dest * i,
			    func, funcf, funcl, func_calc);
	}
    }
    else {	/* ����=0 �Ǥν��� */
	const size_t src_bytes = obj.bytes();
	const size_t dest_bytes = this->bytes();
	char *dest = (char *)(this->data_ptr()) + dest_bytes * pos_dest;
	const char *src =
	    (const char *)(obj.data_ptr_cs()) + src_bytes * pos_src;
	/* �ؿ�����ư����������2�Ĥ������ʤΤǡ�    */
	/* ���٤Ƥ��ȹ礻�ˤĤ��Ʒ׻�����ɬ�פ����� */
	(*func_calc)((const void *)src, (void *)dest, obj_1st, len,
		     obj.default_value_ptr(), func,funcf,funcl);
	dest += dest_bytes * len;
	/* default �ͤ��Ѥ����׻��򤹤� */
	(*func_calc)(NULL, (void *)dest, obj_1st, len_large - len,
		     obj.default_value_ptr(), func,funcf,funcl);
    }
    return *this;
}

/* */

/**
 * @brief  secinfo �˽��äƺƵ�Ū�˥��������� flip ��Ԥʤ� (������)
 *
 *  flip_buf �� non-null �ʾ�硤this_buf �Ȥ������ؤ���Ԥʤ�
 *
 * @note   private �ʴؿ��Ǥ���
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
	p0 = d_ptr;					/* ��¸ */
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
 * @brief  ����ΰ���(�����ΰ���)��ȿž (���٥�)
 * @note   ���Υ��дؿ��� private �Ǥ���
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

    /* �ѡ�������secinfo ������ */
    if ( parse_section_exp(&secinfo, &n_secinfo, secexp.ptr()) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","Error in parse_section_exp()");
    }
    if ( n_secinfo == 0 ) goto quit;

    /* secinfo ������å������� */
    if ( test_section_info( secexp.ptr(), *this, 
			    &secinfo, &n_secinfo, &naxisx ) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","Error in test_section_info()");
    }
    if ( n_secinfo == 0 ) goto quit;

    this_total_elements = 1;
    for ( i=0 ; i < n_secinfo ; i++ ) {
	this_total_elements *= this->length(i);
    }

    /* �Ƶ�Ū�� n �����Υ��������� flip ��Ԥʤ� */
    flip_section_r( this->bytes(), this->data_ptr(), NULL,
		    this->cdimarray(), secinfo, n_secinfo - 1,
		    this_total_elements );

 quit:
    return *this;
}


/**
 * @brief  �Ƶ�Ū�˥��������� xy transpose copy ��Ԥʤ� (������)
 *
 * @note   private �ʴؿ��Ǥ���
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
 * @brief  (x,y)��(y,x)�ȥ�󥹥ݡ�����������̥��֥������Ȥس�Ǽ(���٥�)
 * @note   ���Υ��дؿ��� private �Ǥ���
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
    heap_mem<unsigned char> tmp_block;		/* ���������ѥХåե� */
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

    /* ����2�����ˤ���ɬ�פ����� */
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
	/* ����������Ĺ�����������ϡ�dest_obj�θ��ߤΥХåե��� */
	/* ���Τޤ޻Ȥ�                                           */
	if ( dest_obj->size_type() == this->size_type() &&
	     dest_obj->length() == len_dest ) {
	    /* ������Ĺ��Ʊ���ʤΤǡ������Ǥ�������������������� */
	    dest_obj->reallocate(naxisx.ptr(), n_secinfo, false);
	}
	else {
	    dest_obj->init(this->size_type(), true, 
			   naxisx.ptr(), n_secinfo, false);
	}
	dest_obj->init_properties(*this);
	dest_obj->assign_default(this->default_value_ptr());
    }


    /* ��������Хåե��ΥХ���Ĺ�� 16^2 ���� 256^2 ���ͤ�Ȥ�褦������ */
    min_len_block = (size_t)(1.0 + sqrt((double)(16*16) / this->bytes()));
    max_len_block = (size_t)(1.0 + sqrt((double)(256*256) / this->bytes()));
    if ( row_size < col_size ) min_size = row_size;
    else min_size = col_size;

    if ( min_size <= min_len_block ) len_block = min_len_block;
    else {
	size_t ndiv = ( (min_size - 1) / max_len_block ) + 1;
	len_block = min_size / ndiv;
    }

    /* ��������Хåե�(����Хåե�)����� */
    if ( tmp_block.allocate(this->bytes() * len_block * len_block) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","malloc() failed");
    }

    /* transpose �Ѥ����ѥ��ԡ��ؿ������� */
    func_gencpy2d = NULL;
#define SEL_FUNC(fncname,src_sztp,src_tp,dest_sztp,dest_tp,fnc) \
    if ( this->size_type() == src_sztp && this->size_type() == dest_sztp ) { \
	func_gencpy2d = &gencpy2d::fncname; \
    }
    SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(SEL_FUNC,,,,,,,,,,,,,,,,,,,,,,,,,,,,else);
#undef SEL_FUNC

    if ( this->rounding() == true ) f_idx = 1;		/* �ͼθ������Ѵ� */
    else f_idx = 0;					/* �ڤ�ΤƤ��Ѵ� */


    this_total_elements = 1;
    for ( i=0 ; i < n_secinfo ; i++ ) {
	this_total_elements *= this->length(i);
    }
    sec_total_elements = 1;
    for ( i=0 ; i < n_secinfo ; i++ ) {
	sec_total_elements *= secinfo[i].length;
    }

    /* �Ƶ�Ū�� n �����Υ��������� xy transpose copy ��Ԥʤ� */
    transposef_xy_copy_section_r(*this, this->data_ptr_cs(), dest_obj->data_ptr(),
				  secinfo, sec_total_elements, n_secinfo - 1,
				  this_total_elements, sec_total_elements,
				  len_block, tmp_block.ptr(),
				  func_gencpy2d, &f_idx, 
				  this->func_transp_mtx );

    /* ����ȿž������Ԥʤ� */
    for ( i=0 ; i < n_secinfo ; i++ ) {
	if ( secinfo[i].flip_flag == true ) {
	    size_t ix;
	    if ( i == 0 ) ix = 1;
	    else if ( i == 1 ) ix = 0;
	    else ix = i;
	    dest_obj->flip(ix, 0, dest_obj->length(ix));
	}
    }

    /* ��ʬ���Ȥξ�� */
    if ( dest_ret == this ) {
	dest_ret->swap(tmpobj);
    }

    ret = sec_total_elements;

 quit:
    return ret;
}


/**
 * @brief  �Ƶ�Ū�˥��������� xyz��zxy �� transpose copy ��Ԥʤ� (������)
 *
 * @note   private �ʴؿ��Ǥ���
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
 * @brief  (x,y,z)��(z,x,y)�ȥ�󥹥ݡ�����������̥��֥������Ȥس�Ǽ(���٥�)
 * @note   ���Υ��дؿ��� private �Ǥ���
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
    heap_mem<unsigned char> tmp_block;		/* ���������ѥХåե� */
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

    /* ����3�����ˤ���ɬ�פ����� */
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
	/* ����������Ĺ�����������ϡ�dest_obj�θ��ߤΥХåե��� */
	/* ���Τޤ޻Ȥ�                                           */
	if ( dest_obj->size_type() == this->size_type() &&
	     dest_obj->length() == len_dest ) {
	    /* ������Ĺ��Ʊ���ʤΤǡ������Ǥ�������������������� */
	    dest_obj->reallocate(naxisx.ptr(), n_secinfo, false);
	}
	else {
	    dest_obj->init(this->size_type(), true, 
			   naxisx.ptr(), n_secinfo, false);
	}
	dest_obj->init_properties(*this);
	dest_obj->assign_default(this->default_value_ptr());
    }


    /* ��������Хåե��ΥХ���Ĺ�� 16^2 ���� 256^2 ���ͤ�Ȥ�褦������ */
    min_len_block = (size_t)(1.0 + sqrt((double)(16*16) / this->bytes()));
    max_len_block = (size_t)(1.0 + sqrt((double)(256*256) / this->bytes()));
    if ( layer_size < col_size ) min_size = layer_size;
    else min_size = col_size;

    if ( min_size <= min_len_block ) len_block = min_len_block;
    else {
	size_t ndiv = ( (min_size - 1) / max_len_block ) + 1;
	len_block = min_size / ndiv;
    }

    /* ��������Хåե�(����Хåե�)����� */
    if ( tmp_block.allocate(this->bytes() * len_block * len_block) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","malloc() failed");
    }

    /* transpose �Ѥ����ѥ��ԡ��ؿ������� */
    func_gencpy2d = NULL;
#define SEL_FUNC(fncname,src_sztp,src_tp,dest_sztp,dest_tp,fnc) \
    if ( this->size_type() == src_sztp && this->size_type() == dest_sztp ) { \
	func_gencpy2d = &gencpy2d::fncname; \
    }
    SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(SEL_FUNC,,,,,,,,,,,,,,,,,,,,,,,,,,,,else);
#undef SEL_FUNC

    if ( this->rounding() == true ) f_idx = 1;		/* �ͼθ������Ѵ� */
    else f_idx = 0;					/* �ڤ�ΤƤ��Ѵ� */


    this_total_elements = 1;
    for ( i=0 ; i < n_secinfo ; i++ ) {
	this_total_elements *= this->length(i);
    }
    sec_total_elements = 1;
    for ( i=0 ; i < n_secinfo ; i++ ) {
	sec_total_elements *= secinfo[i].length;
    }

    /* �Ƶ�Ū�� n �����Υ��������� xz transpose copy ��Ԥʤ� */
    transposef_xyz2zxy_copy_section_r(*this, this->data_ptr_cs(), dest_obj->data_ptr(),
				  secinfo, sec_total_elements, n_secinfo - 1,
				  this_total_elements, sec_total_elements,
				  len_block, tmp_block.ptr(),
				  func_gencpy2d, &f_idx, 
				  this->func_transp_mtx );

    /* ����ȿž������Ԥʤ� */
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

    /* ��ʬ���Ȥξ�� */
    if ( dest_ret == this ) {
	dest_ret->swap(tmpobj);
    }

    ret = sec_total_elements;

 quit:
    return ret;
}


/**
 * @brief  secinfo �˽��äƺƵ�Ū�˥��������� clean ��Ԥʤ� (������)
 *
 * @note   private �ʴؿ��Ǥ���
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
 * @brief  ��¸���������Ǥ�1�ĤΥǡ����ǥѥǥ��� (���٥�)
 * @note   ���Υ��дؿ��� private �Ǥ���
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

    /* �ѡ�������secinfo ������ */
    if ( parse_section_exp(&secinfo, &n_secinfo, secexp.ptr()) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","Error in parse_section_exp()");
    }
    if ( n_secinfo == 0 ) goto quit;

    /* secinfo ������å������� */
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

    /* �Ƶ�Ū�� n �����Υ��������� clean ��Ԥʤ� */
    clean_section_r( value_ptr, this->bytes(),
		     this->data_ptr(), this->cdimarray(),
		     secinfo, sec_total_elements, n_secinfo - 1,
		     this_total_elements );

 quit:
    return *this;
}

/**
 * @brief  ��¸���������Ǥ�1�ĤΥǡ����ǥѥǥ��� (���٥�)
 * 
 * @param   value_ptr �ǡ����γ�Ǽ���
 * @param   col_index �����
 * @param   col_size �󥵥���
 * @param   row_index �԰���
 * @param   row_size �ԥ�����
 * @param   layer_index �쥤�����
 * @param   layer_size �쥤�䥵����
 * @return  ���Ȥλ���
 * @note    ���Υ��дؿ��� private �Ǥ���
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
 * @brief  secinfo �˽��äƺƵ�Ū�˥��������Υ��ԡ���Ԥʤ� (������)
 *
 * @note   private �ʴؿ��Ǥ���
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
 * @brief  ����ΰ������̥��֥������Ȥ˥��ԡ� (���٥�)
 * @note   ���Υ��дؿ��� private �Ǥ���
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

    /* ��ʬ���ȤǤ�̵����� */
    if ( dest_obj != NULL && dest_obj != this ) {
	size_t len_dest = 1;
	for ( i=0 ; i < n_secinfo ; i++ ) len_dest *= naxisx[i];
	/* ����������Ĺ�����������ϡ�dest_obj�θ��ߤΥХåե��� */
	/* ���Τޤ޻Ȥ�                                           */
	if ( dest_obj->size_type() == this->size_type() &&
	     dest_obj->length() == len_dest ) {
	    /* ������Ĺ��Ʊ���ʤΤǡ������Ǥ�������������������� */
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

    /* �Ƶ�Ū�� n �����Υ��������Υ��ԡ���Ԥʤ� */
    if ( dest_obj != NULL ) {
	copy_or_move_section_r( (dest_obj == this), this->bytes(),
				this->data_ptr_cs(), this->cdimarray(),
				dest_obj->data_ptr(), 
				secinfo, sec_total_elements, n_secinfo - 1,
				this_total_elements, sec_total_elements);
    }

    /* ��ʬ���Ȥξ�� */
    if ( dest_obj == this ) {
	dest_obj->reallocate(naxisx.ptr(), n_secinfo, false);
    }

    /* ����ȿž������Ԥʤ� */
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
 * @brief  ����ΰ������̥��֥������Ȥ˥��ԡ� (���٥�)
 * 
 * @param  dest_obj ���ԡ���Υ��֥�������
 * @param  col_index ���ԡ�������ΰ���
 * @param  col_size ���ԡ������󥵥���
 * @param  row_index ���ԡ����ι԰���
 * @param  row_size ���ԡ����ιԥ�����
 * @param  layer_index ���ԡ����Υ쥤�����
 * @param  layer_size ���ԡ����Υ쥤�䥵����
 * @return ���ԡ��������ǿ�
 * @throw  �Хåե��γ��ݤ˼��Ԥ�����硤�����˲��򵯤��������
 * @note   ���Υ��дؿ��� private �Ǥ���
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

    /* ���μ�������򥳥ԡ� */
    tmp_dim_size = this->dim_size_rec;
    tmp_size = dup_size_rec(this->cdimarray(), this->dim_size_rec);
    if ( tmp_size.ptr() == NULL ) {
	err_throw(__FUNCTION__,"FATAL","malloc() failed");
    }

    /* ��ʬ���ȤǤ�̵����� */
    if ( dest_obj != NULL && dest_obj != this ) {
	if ( row_ok == false ) {			/* 1 ���� */
	    const size_t naxisx[1] = {col_size};
	    const size_t ndim = 1;
	    if ( dest_obj->size_type() == this->size_type() &&
		 dest_obj->length() == col_size ) {
		dest_obj->reallocate(naxisx, ndim, false);
	    }
	    else dest_obj->init(this->size_type(), true, naxisx, ndim, false);
	}
	else if ( layer_ok == false ) {			/* 2 ���� */
	    const size_t naxisx[2] = {col_size,row_size};
	    const size_t ndim = 2;
	    if ( dest_obj->size_type() == this->size_type() &&
		 dest_obj->length() == col_size * row_size ) {
		dest_obj->reallocate(naxisx, ndim, false);
	    }
	    else dest_obj->init(this->size_type(), true, naxisx, ndim, false);
	}
	else {						/* 3 �����ʾ� */
	    /* 4�����ʾ�ξ�硤���쥤�����ʤ�4�����ʹߤμ������Ĥ� */
	    if ( 3 < this->dim_length() && layer_all == true ) {
		size_t len_dest = 1;
		/* 1��2���������ѹ� */
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

	/* dest_obj ����ʬ���Ȥξ�硤�ǡ��������˰�ư��������ˤʤ� */
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

    /* ��ʬ���Ȥξ�� */
    if ( dest_obj == this ) {
	if ( row_ok == false ) {		/* 1 ���� */
	    const size_t naxisx[1] = {col_size};
	    const size_t ndim = 1;
	    dest_obj->reallocate(naxisx, ndim, false);
	}
	else if ( layer_ok == false ) {		/* 2 ���� */
	    const size_t naxisx[2] = {col_size,row_size};
	    const size_t ndim = 2;
	    dest_obj->reallocate(naxisx, ndim, false);
	}
	else {					/* 3 ���� */
	    /* 4�����ʾ�ξ�硤���쥤�����ʤ�4�����ʹߤμ������Ĥ� */
	    if ( 3 < this->dim_length() && layer_all == true ) {
		/* 1��2���������ѹ� */
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
 * @brief  ����ΰ���ʬ����ꤵ�줿�ͤǽ񤭴��� (���٥롦��®��) (������)
 *
 *  secinfo �˽��äƺƵ�Ū�˥��������� fill ��Ԥʤ�
 *
 * @note   private �ʴؿ��Ǥ���
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
 * @brief  ����ΰ���ʬ����ꤵ�줿�ͤǽ񤭴��� (���٥롦��®��)
 * @note   ���Υ��дؿ��� private �Ǥ���
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

    prms.total_len_elements = 0;			/* �������� */
    prms.round_flg = this->rounding();			/* �ͼθ���/�ڤ�Τ� */

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

    /* �ѡ�������secinfo ������ */
    if ( parse_section_exp(&secinfo, &n_secinfo, secexp.ptr()) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","Error in parse_section_exp()");
    }
    if ( n_secinfo == 0 ) goto quit;

    /* secinfo ������å������� */
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

    prms.total_len_elements = sec_total_elements;		/* SSE2 �� */

    /* �Ƶ�Ū�� n �����Υ��������� fill ��Ԥʤ� */
    fill_section_r( value, func, user_ptr, this->data_ptr(), this,
		    secinfo, n_secinfo - 1, this_total_elements );

 quit:
    return *this;
}


/**
 * @brief  ����ΰ���ʬ����ꤵ�줿�ͤǽ񤭴��� (���٥�) (������)
 *
 *  secinfo �˽��äƺƵ�Ū�˥��������� fill ��Ԥʤ�
 *
 * @note   private �ʴؿ��Ǥ���
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
 * @brief  ����ΰ���ʬ����ꤵ�줿�ͤǽ񤭴��� (���٥�)
 * @note   ���Υ��дؿ��� private �Ǥ���
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

    prms.total_len_elements = 0;			/* �����ʤ� */
    prms.round_flg = this->rounding();			/* �ͼθ���/�ڤ�Τ� */

    if ( func_dest2d == NULL ) {
	/* ���Ѵ��Τ���δؿ� (���Ȥη���double) */
	func_dest2d = this->func_cnv_nd_x2d;
	user_ptr_dest2d = (void *)&prms;		/* �ºݤϻ��Ѥ��줺 */
    }
    if ( func_d2dest == NULL ) {
	/* ���Ѵ��Τ���δؿ� (double�����Ȥη�) */
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

    /* �ѡ�������secinfo ������ */
    if ( parse_section_exp(&secinfo, &n_secinfo, secexp.ptr()) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","Error in parse_section_exp()");
    }
    if ( n_secinfo == 0 ) goto quit;

    /* secinfo ������å������� */
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
	/* �Ƶ�Ū�� n �����Υ��������� fill ��Ԥʤ� */
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
 * @brief  ����ΰ���ʬ����ꤵ�줿�ͤǽ񤭴��� (���٥롦��®��)
 * @note   ���Υ��дؿ��� private �Ǥ���
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

    prms.total_len_elements = 0;			/* �������� */
    prms.round_flg = this->rounding();			/* �ͼθ���/�ڤ�Τ� */

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

    prms.total_len_elements = col_size * row_size * layer_size;   /* SSE2 �� */

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
 * @brief  ����ΰ���ʬ����ꤵ�줿�ͤǽ񤭴��� (���٥�)
 *
 *  ���Ȥ�����λ��ꤵ�줿�ϰϤ����Ǥ򡤻��ꤵ�줿�ͤޤ��ϥ桼������ؿ���ͳ��
 *  �񤭴����ޤ�.
 * 
 * @param  value �񤭹��ޤ����
 * @param  func_dest2d ���Ȥ����ǡ�����double�����Ѵ����뤿���
 *                     �桼���ؿ��Υ��ɥ쥹.
 * @param  user_ptr_dest2d func_dest2d�κǸ��Ϳ������桼���Υݥ���
 * @param  func_d2dest double�����ͤ��鼫�Ȥ����ǡ������Ѵ����뤿���
 *                     �桼���ؿ��Υ��ɥ쥹.
 * @param  user_ptr_d2dest func_d2dest�κǸ��Ϳ������桼���Υݥ���
 * @param  func ���Ѵ��ΰ٤Υ桼���ؿ��Υ��ɥ쥹
 * @param  user_ptr_func func�κǸ��Ϳ������桼���Υݥ���
 * @param  col_index �����
 * @param  col_size �󥵥���
 * @param  row_index �԰���
 * @param  row_size �ԥ�����
 * @param  layer_index �쥤�����
 * @param  layer_size �쥤�䥵����
 * 
 * @return  ���Ȥλ���
 * @note    ���Υ��дؿ��� private �Ǥ���
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

    prms.total_len_elements = 0;			/* �����ʤ� */
    prms.round_flg = this->rounding();			/* �ͼθ���/�ڤ�Τ� */

    if ( func_dest2d == NULL ) {
	/* ���Ѵ��Τ���δؿ� (���Ȥη���double) */
	func_dest2d = this->func_cnv_nd_x2d;
	user_ptr_dest2d = (void *)&prms;		/* �ºݤϻ��Ѥ��줺 */
    }
    if ( func_d2dest == NULL ) {
	/* ���Ѵ��Τ���δؿ� (double�����Ȥη�) */
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
 * @brief  section() ���ΰ�����Ĵ�٤�Ŭ�ڤʺ�ɸ�Υѥ�᡼�������� (���٥�)
 *
 *  �����κ�ɸ�����������֤򼨤��Ƥ뤫��Ƚ�Ǥ����������ʤ����Ϻ�ɸ��
 *  �ѥ�᡼����Ĵ������.
 *
 * @param  r_col_index �����
 * @param  r_col_size �󥵥���
 * @param  r_row_index �԰���
 * @param  r_row_size �ԥ�����
 * @param  r_layer_index �쥤�����
 * @param  r_layer_size �쥤�䥵����
 * @param  r_col_ok ��¸�ߤ�����Ͽ��ˡ�
 *                  ¸�ߤ��ʤ����ϵ��˽񤭴�������
 * @param  r_row_ok �Ԥ�¸�ߤ�����Ͽ��ˡ�
 *                  ¸�ߤ��ʤ����ϵ��˽񤭴�������
 * @param  r_layer_ok �쥤�䤬¸�ߤ�����Ͽ��ˡ�
 *                    ¸�ߤ��ʤ����ϵ��˽񤭴�������
 * @return  ���������Ȥ�������ΰ���򼨤��Ƥ�������0��
 *          �ΰ褫��Ϥ߽Ф��Ƥ��뤬ͭ���ΰ褬¸�ߤ�����������͡�
 *          ͭ���ΰ褬̵���������͡�
 * @note    ���Υ��дؿ��� private �Ǥ���
 */
/*
 * ������ index=-1, size=1 �λ���OK �Ȥ���Ƥ������ְ㤤�ʤΤǽ��� 2012/3/27
 * col_ok �� true �ξ��Τߡ�return_status �� 0 �Ȥʤ�褦���� 2012/3/27
 * ���ν����ˤ�ꡤͭ���ΰ褬̵�����ϡ�����֤��ͤ����ͤˤʤ�Ϥ���
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
	if ( layer_index < 0 ) { /* ����ΰ賰����ꤷ����硤length�򸺤餹 */
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
	/* layer_ok �� false */
	layer_index = 0;
	layer_size = 1;
    }

    if ( 2 <= this->dim_length() && 0 < this->row_length() &&
	 0 < row_size && 0 < col_size ) {
	size_t row_index_0;
	if ( row_index < 0 ) {	/* ����ΰ賰����ꤷ����硤length�򸺤餹 */
	    if ( row_size <= abs_sz2z(row_index) ) goto quit;
	    row_size -= abs_sz2z(row_index);
	    row_index = 0;
	}
	row_index_0 = abs_sz2z(row_index);
	if ( layer_ok == false ) {	/* 2�����ξ�� */
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
	else {				/* 3�����ξ�� */
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
	/* row_ok �� false */
	row_index = 0;
	row_size = 1;
    }

    if ( 1 <= this->dim_length() && 0 < this->col_length() &&
	 0 < col_size ) {
	size_t col_index_0;
	if ( col_index < 0 ) {	/* ����ΰ賰����ꤷ����硤length�򸺤餹 */
	    if ( col_size <= abs_sz2z(col_index) ) goto quit;
	    col_size -= abs_sz2z(col_index);
	    col_index = 0;
	}
	col_index_0 = abs_sz2z(col_index);
	if ( row_ok == false ) {	/* 1�����ξ�� */
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
	else {				/* 2�����ξ�� */
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
 * @brief  pastef() ���ΰ�����Ĵ�٤�Ŭ�ڤʺ�ɸ�Υѥ�᡼�������� (���٥�)
 *
 *  paste�Ѥ�secinfo������å�����paste�Ǥ����ϰϤ�̵������ -1 ���֤���<br>
 *  mdarray::test_paste_arg ��Ʊ�ͤΥ����ɤˤʤ�褦�ˤ��Ƥ��롥
 *
 * @note   private �ʴؿ��Ǥ���
 */
static int test_paste_secinfo( const mdarray &src, const mdarray &dest,
		   const heap_mem<mdarray_section_expinfo> &secinfo, size_t n_secinfo )
{
    size_t i;
    for ( i=0 ; i < n_secinfo ; i++ ) {
	size_t abs_begin = abs_sz2z(secinfo[i].begin);
	size_t length = secinfo[i].length;
	if ( length == MDARRAY_ALL || src.length(i) < length ) {
	    length = src.length(i);	/* ����������Ȥ� */
	}
	if ( length == 0 || dest.length(i) == 0 ) return -1;
	if ( 0 <= secinfo[i].begin ) {
	    if ( dest.length(i) <= abs_begin ) return -1;
	}
	else {	/* begin����ξ�� */
	    if ( length <= abs_begin ) return -1;
	}
    }
    return 0;
}

/**
 * @brief  �Ƶ��ƤӽФ��ˤ�ꡤn�����Υڡ����Ƚ�����Ԥʤ� (��®��) (������)
 *
 * @note  test_paste_secinfo() �ǥ����å����Ƥ���Ȥ�����<br>
 *        private �ʴؿ��Ǥ���
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

    /* secinfo ��ͭ����Ϥ����餫����� */
    if ( dim_ix < n_secinfo ) {
	begin_paste = secinfo[dim_ix].begin;
	abs_begin = abs_sz2z(secinfo[dim_ix].begin);
	len_paste = secinfo[dim_ix].length;
    }

    //err_report1(__FUNCTION__,"DEBUG","begin_paste = %zd",begin_paste);
    //err_report1(__FUNCTION__,"DEBUG","len_paste = %zu",len_paste);
 
    if ( len_paste == MDARRAY_ALL || len_src < len_paste ) {
	len_paste = len_src;	/* ����������Ȥ� */
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

    if ( 0 < dim_ix ) {		/* ����=1�ʾ�Ǥν��� */
	if ( dest_p < src_p ) {
	    for ( i=0 ; i < len_paste ; i++ ) {		/* �������� */
		image_paste_fast_r(src, dest, secinfo, n_secinfo, 
				   func, user_ptr,
				   dim_ix - 1, blen_block_src, blen_block_dest,
				   src_p + blen_block_src * i,
				   dest_p + blen_block_dest * i);
	    }
	}
	else {
	    for ( i = len_paste ; 0 < i ; ) {		/* ������� */
		i--;
		image_paste_fast_r(src, dest, secinfo, n_secinfo, 
				   func, user_ptr,
				   dim_ix - 1, blen_block_src, blen_block_dest,
				   src_p + blen_block_src * i,
				   dest_p + blen_block_dest * i);
	    }
	}
    }
    else {			/* ����=0 �Ǥν��� */
	if ( dest_p < src_p ) {
	    (*func)(src_p,dest_p,len_paste,+1,user_ptr);       /* �������� */
	}
	else {
	    (*func)(src_p,dest_p,len_paste,-1,user_ptr);       /* ������� */
	}
    }

    return;
}

/**
 * @brief  ����ǡ���(�����ǡ���)�Υڡ����� (���٥롦��®��)
 * @note   ���Υ��дؿ��� private �Ǥ���
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

    prms.total_len_elements = 0;			/* �����ʤ� */
    prms.round_flg = this->rounding();			/* �ͼθ���/�ڤ�Τ� */

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

    /* expression ��Ÿ�� */
    if ( c_vasprintf(secexp.ptr_address(), exp_fmt, ap) < 0 ) {
	err_report(__FUNCTION__,"FATAL","c_vasprintf() failed");
	goto quit;
    }

    /* expression �Υѡ��� */
    if ( parse_section_exp(&secinfo, &n_secinfo, secexp.ptr()) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","Error in parse_section_exp()");
    }
    if ( n_secinfo == 0 ) goto quit;

    /* ������������Υ����å� */
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

    /* ndim ����������ǤθĿ�����Ƥ��� */
    src_blen = src_img.bytes();
    dest_blen = this->bytes();
    for ( i=0 ; i < ndim ; i++ ) {
	src_blen *= src_img.length(i);
	dest_blen *= this->length(i);
    }

    /* �Ƶ��ƤӽФ��ǥڡ����Ȥ��� */
    image_paste_fast_r( src_img, this, secinfo, n_secinfo, 
			func, user_ptr, 
			ndim - 1,  src_blen, dest_blen,
			src_img.data_ptr_cs(), this->data_ptr() );

 quit:
    return *this;
}


/**
 * @brief  �Ƶ��ƤӽФ��ˤ�ꡤn�����Υڡ����Ƚ�����Ԥʤ� (������)
 *
 * @note  test_paste_secinfo() �ǥ����å����Ƥ���Ȥ�����<br>
 *        private �ʴؿ��Ǥ���
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

    /* secinfo ��ͭ����Ϥ����餫����� */
    if ( dim_ix < n_secinfo ) {
	begin_paste = secinfo[dim_ix].begin;
	abs_begin = abs_sz2z(secinfo[dim_ix].begin);
	len_paste = secinfo[dim_ix].length;
    }

    //err_report1(__FUNCTION__,"DEBUG","begin_paste = %zd",begin_paste);
    //err_report1(__FUNCTION__,"DEBUG","len_paste = %zu",len_paste);
 
    if ( len_paste == MDARRAY_ALL || len_src < len_paste ) {
	len_paste = len_src;	/* ����������Ȥ� */
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

    if ( 0 < dim_ix ) {		/* ����=1�ʾ�Ǥν��� */
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
	    for ( i=0 ; i < len_paste ; i++ ) {		/* �������� */
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
	    for ( i = len_paste ; 0 < i ; ) {		/* ������� */
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
    else {			/* ����=0 �Ǥν��� */
	size_t ii = off_dest;
	/* ����Хåե��ˤΤ���Τǡ���������������������ɤ� */
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
 * @brief  ����ǡ���(�����ǡ���)�Υڡ����� (���٥�)
 * @note   ���Υ��дؿ��� private �Ǥ���
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

    prms.total_len_elements = 0;			/* �����ʤ� */
    prms.round_flg = this->rounding();			/* �ͼθ���/�ڤ�Τ� */

    if ( func_src2d == NULL ) {
	/* ���Ѵ��Τ���δؿ� (����obj�η���double) */
	func_src2d = src_img.func_cnv_nd_x2d;
	user_ptr_src2d = (void *)&prms;			/* �ºݤϻ��Ѥ��줺 */
    }
    if ( func_dest2d == NULL ) {
	/* ���Ѵ��Τ���δؿ� (���Ȥη���double) */
	func_dest2d = this->func_cnv_nd_x2d;
	user_ptr_dest2d = (void *)&prms;		/* �ºݤϻ��Ѥ��줺 */
    }
    if ( func_d2dest == NULL ) {
	/* ���Ѵ��Τ���δؿ� (double�����Ȥη�) */
	func_d2dest = this->func_cnv_nd_d2x;
	user_ptr_d2dest = (void *)&prms;
    }

    if ( func_src2d == NULL || func_dest2d == NULL || func_d2dest == NULL ) {
	err_report(__FUNCTION__,"WARNING","invalid size of type; ignored");
	goto quit;
    }

    /* expression ��Ÿ�� */
    if ( c_vasprintf(secexp.ptr_address(), exp_fmt, ap) < 0 ) {
	err_report(__FUNCTION__,"FATAL","c_vasprintf() failed");
	goto quit;
    }

    /* expression �Υѡ��� */
    if ( parse_section_exp(&secinfo, &n_secinfo, secexp.ptr()) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","Error in parse_section_exp()");
    }
    if ( n_secinfo == 0 ) goto quit;

    /* ������������Υ����å� */
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

    /* ndim ����������ǤθĿ�����Ƥ��� */
    src_blen = src_img.bytes();
    dest_blen = this->bytes();
    for ( i=0 ; i < ndim ; i++ ) {
	src_blen *= src_img.length(i);
	dest_blen *= this->length(i);
    }

    /* x_buf_xxx ��Ĺ������� (image_paste_r() �ΰ��������ɤ��Τޤ�) */
    len_x_buf = secinfo[0].length;
    if ( len_x_buf == MDARRAY_ALL || src_img.length(0) < len_x_buf ) {
	len_x_buf = src_img.length(0);	/* ����������Ȥ� */
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

    /* ����Хåե������ */
    if ( x_buf_src.allocate_aligned32(len_x_buf, &x_bptr_src) < 0 ||
	 x_buf_dst.allocate_aligned32(len_x_buf, &x_bptr_dst) < 0 ) {
 	err_throw(__FUNCTION__,"FATAL","malloc() failed");
    }

    /* �Ƶ��ƤӽФ��ǥڡ����Ȥ��� */
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
 * @brief  ����ǡ���(�����ǡ���)�Υڡ����� (���٥롦��®��)
 *
 * ���Ȥ�����λ��ꤵ�줿�ϰϤ������ͤˡ�src_img�ǻ��ꤵ�줿���֥������Ȥ�
 * �������͡��ޤ��ϥ桼������ؿ���ͳ���Ѵ����줿�����Ǥ�Ž���դ��ޤ�. 
 * 
 * @param  src_img �����Ȥʤ��������ĥ��֥�������
 * @param  func ���Ѵ��ΰ٤Υ桼���ؿ��Υ��ɥ쥹
 * @param  user_ptr func�κǸ��Ϳ������桼���Υݥ���
 * @param  dst_col �����
 * @param  dst_row �԰���
 * @param  dst_layer �쥤�����
 * 
 * @return  ���Ȥλ���
 * @note    ���Υ��дؿ��� private �Ǥ���
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
    ssize_t col_begin, row_begin, layer_begin;    /* for()�롼�פǻ���    */
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

    prms.total_len_elements = 0;			/* �����ʤ� */
    prms.round_flg = this->rounding();			/* �ͼθ���/�ڤ�Τ� */

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

    /* ����������å� */
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

    /* src_img ����ʬ���ȤǤ⤤���褦�ˤ��� */
    if ( dst_k_ptr + dst_col_blen * (dst_row + row_begin) + 
	 dst_bytes * (dst_col + col_begin) <=
	 src_k_ptr + src_col_blen * row_begin + src_bytes * col_begin ) {
	/* ������� */
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
	/* ������ */
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
 * @brief   ����ǡ���(�����ǡ���)�Υڡ����� (���٥�)
 *
 *  ���Ȥ�����λ��ꤵ�줿�ϰϤ������ͤˡ�src_img �ǻ��ꤵ�줿���֥������Ȥ�
 *  �������ͤޤ��ϥ桼������ؿ���ͳ���Ѵ����줿�����Ǥ�Ž���դ��ޤ���
 *
 * @param  src_img �����Ȥʤ��������ĥ��֥�������
 * @param  func_src2d ���֥�������src�����ǡ�����
 *                    double�����Ѵ����뤿��Υ桼���ؿ��ؤΥ��ɥ쥹
 * @param  user_ptr_src2d func_src2d�κǸ��Ϳ������
 *                        �桼���Υݥ���.
 * @param  func_dest2d ���Ȥ����ǡ�����double�����Ѵ����뤿���
 *                     �桼���ؿ��ؤΥ��ɥ쥹.
 * @param  user_ptr_dest2d func_dest2d�κǸ��Ϳ������桼���Υݥ���
 * @param  func_d2dest double�����ͤ��鼫�Ȥ����ǡ������Ѵ����뤿���
 *                     �桼���ؿ��ؤΥ��ɥ쥹
 * @param  user_ptr_d2dest func_d2dest�κǸ��Ϳ������桼���Υݥ���
 * @param  func ���Ѵ��Τ���Υ桼���ؿ��Υ��ɥ쥹
 * @param  user_ptr_func func�κǸ��Ϳ������桼���Υݥ���
 * @param  dst_col �����
 * @param  dst_row �԰���
 * @param  dst_layer �쥤�����
 * 
 * @return  ���Ȥλ���
 * @note    ���Υ��дؿ��� private �Ǥ���
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
    ssize_t col_begin, row_begin, layer_begin;    /* for()�롼�פǻ���    */
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

    prms.total_len_elements = 0;			/* �����ʤ� */
    prms.round_flg = this->rounding();			/* �ͼθ���/�ڤ�Τ� */

    if ( func_src2d == NULL ) {
	/* ���Ѵ��Τ���δؿ� (����obj�η���double) */
	func_src2d = src_img.func_cnv_nd_x2d;
	user_ptr_src2d = (void *)&prms;			/* �ºݤϻ��Ѥ��줺 */
    }
    if ( func_dest2d == NULL ) {
	/* ���Ѵ��Τ���δؿ� (���Ȥη���double) */
	func_dest2d = this->func_cnv_nd_x2d;
	user_ptr_dest2d = (void *)&prms;		/* �ºݤϻ��Ѥ��줺 */
    }
    if ( func_d2dest == NULL ) {
	/* ���Ѵ��Τ���δؿ� (double�����Ȥη�) */
	func_d2dest = this->func_cnv_nd_d2x;
	user_ptr_d2dest = (void *)&prms;
    }

    if ( func_src2d == NULL || func_dest2d == NULL || func_d2dest == NULL ) {
	err_report(__FUNCTION__,"WARNING","invalid size of type; ignored");
	goto quit;
    }

    /* ����������å����� */
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

    /* ����ΰ����� */
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

      /* src_img ����ʬ���ȤǤ⤤���褦�ˤ��� */
      if ( dst_k_ptr + dst_col_blen * (dst_row + row_begin) + 
	 dst_bytes * (dst_col + col_begin) <=
	 src_k_ptr + src_col_blen * row_begin + src_bytes * col_begin ) {
	/* ������� */
	size_t j, k;
	//err_report(__FUNCTION__,"DEBUG","=>=>=>=>=>=>=>=>");
	for ( k=layer_begin ; k < layer_size ; k++ ) {
	  ssize_t kk = dst_layer + k;
	  src_j_ptr = src_k_ptr + src_col_blen * row_begin;
	  dst_j_ptr = dst_k_ptr + dst_col_blen * (dst_row + row_begin);
	  for ( j=row_begin ; j < row_size ; j++ ) {
	    ssize_t ii, jj = dst_row + j;

	    /* ����Хåե��ˤΤ���Τǡ�����������������ɤ� */
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
	/* ������ */
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

	    /* ����Хåե��ˤΤ���Τǡ�����������������ɤ� */
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

/* paste() ���ΰ�����Ĵ�٤�Ŭ�ڤʺ�ɸ�Υѥ�᡼�������ꤹ�� */
/* r_col_begin �� ssize_t �ˤʤäƤ뤬������ͤ��֤����Ϥʤ� */
/**
 * @brief   paste() ���ΰ�����Ĵ�٤�Ŭ�ڤʺ�ɸ�Υѥ�᡼�������� (���٥�)
 *
 *  ���� src_img �Ǽ����줿���֥������Ȥ������ ���Ȥ������Ž���դ��������
 *  �����롤��ɸ�ΰ��֤�Ĺ���Υ����å���»ܤ���Ŭ�ڤʺ�ɸ�ѥ�᡼����������
 *  ���ޤ�.
 * 	
 * @param  src_img �����Ȥʤ��������ĥ��֥�������
 * @param  r_col_idx �����(dest�Ĥޤ꼫��)
 * @param  r_row_idx �԰���(dest�Ĥޤ꼫��)
 * @param  r_layer_idx �쥤�����(dest�Ĥޤ꼫��)
 * @param  r_col_ok ��¸�ߤ�����Ͽ��ˡ�
 *                  ¸�ߤ��ʤ����ϵ��˽񤭴�������
 * @param  r_row_ok �Ԥ�¸�ߤ�����Ͽ��ˡ�
 *                  ¸�ߤ��ʤ����ϵ��˽񤭴�������
 * @param  r_layer_ok �쥤�䤬¸�ߤ�����Ͽ��ˡ�
 *                    ¸�ߤ��ʤ����ϵ��˽񤭴�������
 * @param  r_col_size �󥵥���(src)
 * @param  r_row_size �ԥ�����(src)
 * @param  r_layer_size �쥤�䥵����(src)
 * @param  r_col_begin ��γ��ϰ���(src)
 * @param  r_row_begin �Ԥγ��ϰ���(src)
 * @param  r_layer_begin �쥤��γ��ϰ���(src)
 * @return  ���ꤵ�줿���֡�Ĺ����������ϰ����ͭ�����0��
 *          �ϰϳ��ˤ��뤬ͭ���ΰ褬ͭ����������͡�ͭ���ΰ褬̵���������
 * @note   ���Υ��дؿ��� private �Ǥ���
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
	if ( 0 <= layer_idx ) {		/* idx ��0�ʾ�ξ�� */
	    if ( this->layer_length() <= abs_idx ) goto quit;
	    if ( this->layer_length() < abs_idx + layer_size ) {
		layer_size = this->layer_length() - abs_idx;
	    }
	}
	else {				/* idx ����ξ�� */
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
	if ( 0 <= row_idx ) {		/* idx ��0�ʾ�ξ�� */
	    if ( this->row_length() <= abs_idx ) goto quit;
	    if ( this->row_length() < abs_idx + row_size ) {
		row_size = this->row_length() - abs_idx;
	    }
	}
	else {				/* idx ����ξ�� */
	    if ( row_size <= abs_idx ) goto quit;
	    if ( this->row_length() + abs_idx < row_size ) {
		row_size = this->row_length() + abs_idx;
	    }
	    row_begin = - row_idx;

	}
	row_ok = true;
	/*
	if ( layer_ok == false ) {	// 2�����ξ��
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
	else {				// 3�����ξ��
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
	if ( 0 <= col_idx ) {		/* idx ��0�ʾ�ξ�� */
	    if ( this->col_length() <= abs_idx ) goto quit;
	    if ( this->col_length() < abs_idx + col_size ) {
		col_size = this->col_length() - abs_idx;
	    }
	}
	else {				/* idx ����ξ�� */
	    if ( col_size <= abs_idx ) goto quit;
	    if ( this->col_length() + abs_idx < col_size ) {
		col_size = this->col_length() + abs_idx;
	    }
	    col_begin = - col_idx;

	}
	col_ok = true;
	/*
	if ( row_ok == false ) {	// 1�����ξ��
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
	else {				// 2�����ξ��
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
 * @brief   ptr�Υ��ɥ쥹�����ȤΥǡ����ΰ��⤫������å�
 * 
 * @param   ptr �����å����륢�ɥ쥹
 * @return  ���Ȥ��ΰ���λ��Ͽ���<br>
 *          ����ʳ��λ��ϵ�
 * @note    ���Υ��дؿ��� private �Ǥ���
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
 * @brief  _arr_rec �ǻ��Ѥ������κƳ���
 * @note   ���Υ��дؿ��� private �Ǥ���
 */
int mdarray::realloc_arr_rec( size_t len_bytes )
{
    this->cleanup_shallow_copy(true);	/* �Хåե���ľ�����뤿��Ƥ� */

    int strategy;
    size_t len_alloc;

    if ( this->_arr_rec == NULL && len_bytes == 0 ) return 0;

    /* "auto" �����ꤵ��Ƥ�����ϡ�auto_resize �Ƿ��� */
    if ( this->alloc_strategy_rec == Alloc_strategy_auto ) {
	if ( this->auto_resize() == true ) strategy = Alloc_strategy_pow;
	else strategy = Alloc_strategy_min;
    }
    else {
	strategy = this->alloc_strategy_rec;
    }

    /* "pow" �����ꤵ��Ƥ��ơ�1GB ̤���λ� */
    if ( strategy == Alloc_strategy_pow && 
	 0 < len_bytes && len_bytes < (size_t)1024*1024*1024 ) {
	if ( len_bytes <= 32 ) {		/* �׵᤬��������� */
	    len_alloc = 32;
	}
	else {					/* 2^n �ǤȤ�褦�˼��� */
	    const double base = 2.0;
	    size_t nn = (size_t)c_ceil( log((double)len_bytes) / log(base) );
	    size_t len = (size_t)pow(base, (double)nn);
	    /* ǰ�Τ�������å� */
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
 * @brief  _arr_rec �ǻ��Ѥ��Ƥ������������� NULL �ͤ򥻥å�
 * @note   ���Υ��дؿ��� protected �Ǥ���
 */
void mdarray::free_arr_rec()
{
    this->cleanup_shallow_copy(true);	/* �Хåե���ľ�����뤿��Ƥ� */

    if ( this->_arr_rec != NULL ) {
	free(this->_arr_rec);
	this->_arr_rec = NULL;
	this->arr_alloc_blen_rec = 0;
    }
    return;
}

/**
 * @brief  _default_rec �ǻ��Ѥ������κƳ���
 * @note   ���Υ��дؿ��� private �Ǥ���
 */
int mdarray::realloc_default_rec( size_t len_bytes )
{
    this->cleanup_shallow_copy(true);	/* �Хåե���ľ�����뤿��Ƥ� */

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
 * @brief  _size_rec �ǻ��Ѥ������κƳ���
 * @note   ���Υ��дؿ��� private �Ǥ���
 */
int mdarray::realloc_size_rec( size_t len_elements )
{
    this->cleanup_shallow_copy(true);	/* �Хåե���ľ�����뤿��Ƥ� */

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
 * @brief  _default_rec �ǻ��Ѥ��Ƥ������������� NULL �ͤ򥻥å�
 * @note   ���Υ��дؿ��� private �Ǥ���
 */
void mdarray::free_default_rec()
{
    this->cleanup_shallow_copy(true);	/* �Хåե���ľ�����뤿��Ƥ� */

    if ( this->_default_rec != NULL ) {
	free(this->_default_rec);
	this->_default_rec = NULL;
    }

    return;
}

/**
 * @brief  _size_rec �ǻ��Ѥ��Ƥ������������� NULL �ͤ򥻥å�
 * @note   ���Υ��дؿ��� private �Ǥ���
 */
void mdarray::free_size_rec()
{
    this->cleanup_shallow_copy(true);	/* �Хåե���ľ�����뤿��Ƥ� */

    if ( this->_size_rec != NULL ) {
	free(this->_size_rec);
	this->_size_rec = NULL;
    }
    return;
}


/**
 * @brief  _arr_ptr_2d_rec �򹹿�
 *
 * @param  on_2d true�ξ�硤_arr_ptr_2d_rec �����Ƥ򿷵��˺������ޤ���
 *               false�ξ�硤_arr_ptr_2d_rec ��non-NULL�ʤ����Ƥ򹹿����ޤ���
 * 
 * @note   ���Υ��дؿ��� private �Ǥ���
 */
void mdarray::update_arr_ptr_2d( bool on_2d )
{
    this->cleanup_shallow_copy(true);	/* �Хåե���ľ�����뤿��Ƥ� */

    this->_update_arr_ptr_2d(on_2d);
}

/**
 * @brief  _arr_ptr_2d_rec �򹹿� (���٥�)
 *
 * @note   ���Υ��дؿ��� private �Ǥ���
 */
void mdarray::_update_arr_ptr_2d( bool on_2d )
{
    /*
     * ���: ���Υ��дؿ������٥�ʤΤǡ�������ǽ񤭹����ѤΥ��дؿ���
     *       �ȤäƤϤʤ�ʤ���
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
	if ( this->_arr_ptr_2d_rec == NULL /* �ǽ�ξ�� */ ||
	     i+1 < buf_len /* �Хåե���­��ʤ���� */ || 
	     this->_arr_ptr_2d_rec[i] != NULL /* �Хåե���¿����� */ ) {
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

    /* �����ݥ��󥿤ι��� */
    if ( this->extptr_2d_rec != NULL ) 
	*(this->extptr_2d_rec) = this->_arr_ptr_2d_rec;

    return;
}

/**
 * @brief  _arr_ptr_3d_rec �򹹿�
 *
 * @param  on_3d true�ξ�硤_arr_ptr_3d_rec �����Ƥ򿷵��˹��ۤ��ޤ���
 *               false�ξ�硤_arr_ptr_3d_rec ��non-NULL�ʤ����Ƥ򹹿����ޤ���
 * 
 * @note   ���Υ��дؿ��� private �Ǥ���
 */
void mdarray::update_arr_ptr_3d( bool on_3d )
{
    this->cleanup_shallow_copy(true);	/* �Хåե���ľ�����뤿��Ƥ� */

    this->_update_arr_ptr_3d(on_3d);
}

/**
 * @brief  _arr_ptr_3d_rec �򹹿� (���٥�)
 *
 * @note   ���Υ��дؿ��� private �Ǥ���
 */
void mdarray::_update_arr_ptr_3d( bool on_3d )
{
    /*
     * ���: ���Υ��дؿ������٥�ʤΤǡ�������ǽ񤭹����ѤΥ��дؿ���
     *       �ȤäƤϤʤ�ʤ���
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
	    /* �ƹԤ���Ƭ���ɥ쥹����Ͽ */
	    /* NULL �����ޤ�³���� */
	    for ( ; this->_arr_ptr_3d_rec[i] != NULL ; i++ ) {
		size_t j;
		if ( i+1 < buf_len1 ) {
		    /* �Ԥ���Ƭ���ɥ쥹�����륢�ɥ쥹�ơ��֥� */
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
			/* ���פˤʤä���ʬ */
			free(this->_arr_ptr_3d_rec[ii]);
		    }
		    flag_buf_is_large = true;
		    break;
		}
	    }
	}
	/* ���������ѹ��ˤʤäƤ�������realloc()���� */
	if ( this->_arr_ptr_3d_rec == NULL /* �ǽ�ξ�� */ ||
	     i+1 < buf_len1 /* �Хåե���­��ʤ���� */ || 
	     flag_buf_is_large == true /* �Хåե���¿����� */ ) {
	    /* �쥤�����Ƭ���ɥ쥹�����륢�ɥ쥹�ơ��֥� */
	    tmp_ptr = realloc(this->_arr_ptr_3d_rec,
			      sizeof(*(this->_arr_ptr_3d_rec)) * buf_len1);
	    if ( tmp_ptr == NULL ) {
		err_throw(__FUNCTION__,"FATAL","realloc() failed");
	    }
	    this->_arr_ptr_3d_rec = (void ***)tmp_ptr;
	}
	/* �ƹԤ���Ƭ���ɥ쥹����Ͽ(����ޤǤ�����Ͽ�Ǥ��ʤ��ä�����³��) */
	for ( ; i+1 < buf_len1 ; i++ ) {
	    size_t j;
	    this->_arr_ptr_3d_rec[i] = NULL;
	    /* �Ԥ���Ƭ���ɥ쥹�����륢�ɥ쥹�ơ��֥� */
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

    /* �����ݥ��󥿤ι��� */
    if ( this->extptr_3d_rec != NULL ) 
	*(this->extptr_3d_rec) = this->_arr_ptr_3d_rec;

    return;
}

/**
 * @brief  _arr_ptr_2d_rec �ǻ��Ѥ��Ƥ������������� NULL �ͤ򥻥å�
 *
 * @note ���Υ��дؿ��� private �Ǥ���
 */
void mdarray::free_arr_ptr_2d()
{
    this->cleanup_shallow_copy(true);	/* �Хåե���ľ�����뤿��Ƥ� */

    if ( this->_arr_ptr_2d_rec != NULL ) {
	free(this->_arr_ptr_2d_rec);
	this->_arr_ptr_2d_rec = NULL;
    }
    return;
}

/**
 * @brief  _arr_ptr_3d_rec �ǻ��Ѥ��Ƥ������������� NULL �ͤ򥻥å�
 *
 * @note ���Υ��дؿ��� private �Ǥ���
 */
void mdarray::free_arr_ptr_3d()
{
    this->cleanup_shallow_copy(true);	/* �Хåե���ľ�����뤿��Ƥ� */

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
 * @brief  inline���дؿ��� throw ������˻���
 *
 */
void mdarray::err_throw_int_arg( const char *fncname, const char *lev, 
				 const char *mes, int v ) const
{
    err_throw1(fncname, lev, mes, v);
    return;
}


/**
 * @brief  ����ѥ�����˥��顼����Ϥ����뤿��Υ��ߡ��Υ��дؿ�
 * @deprecated  �����Բ�
 */
mdarray::mdarray( ssize_t sz_type, int naxis0 )
{
    err_throw(__FUNCTION__,"FATAL","do not use");
}

/**
 * @brief  ����ѥ�����˥��顼����Ϥ����뤿��Υ��ߡ��Υ��дؿ�
 * @deprecated  �����Բ�
 */
mdarray::mdarray( ssize_t sz_type, size_t naxis0 )
{
    err_throw(__FUNCTION__,"FATAL","do not use");
}

/**
 * @brief  ����ѥ�����˥��顼����Ϥ����뤿��Υ��ߡ��Υ��дؿ�
 * @deprecated  �����Բ�
 */
mdarray &mdarray::init( ssize_t sz_type, int naxis0 )
{
    err_throw(__FUNCTION__,"FATAL","do not use");
    return *this;
}

/**
 * @brief  ����ѥ�����˥��顼����Ϥ����뤿��Υ��ߡ��Υ��дؿ�
 * @deprecated  �����Բ�
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
