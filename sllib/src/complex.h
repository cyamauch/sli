/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2013-06-24 14:23:55 cyamauch> */

#ifndef _SLI__COMPLEX_H
#define _SLI__COMPLEX_H 1

/**
 * @file   complex.h
 * @brief  複素関数の定義 (C99に準拠)
 * @attention  C++ 標準ライブラリの <cmath> を使う場合，マクロ SLI__USE_CMATH
 *             を定義してから complex.h を include してください．
 */

#ifdef SLI__USE_CMATH
#include <cmath>
#else
#include <math.h>
#endif

#include "complex_defs.h"

#define _Complex_I (__extension__ 1.0iF)
#define I _Complex_I

namespace sli
{

#ifdef SLI__USE_CMATH
    /* 注意: std::hypot は無いらしい */
    using std::atan2;
    using std::exp;
    using std::cos;
    using std::sin;
    using std::sqrt;
    using std::log;
    using std::cosh;
    using std::sinh;
    using std::isinf;
    using std::signbit;
#else
#undef atan2
#undef exp
#undef cos
#undef sin
#undef sqrt
#undef log
#undef cosh
#undef sinh
    using ::atan2;
    using ::exp;
    using ::cos;
    using ::sin;
    using ::sqrt;
    using ::log;
    using ::cosh;
    using ::sinh;
#endif


/*
 * inline functions
 */

inline static float creal( fcomplex cv )
{
    return (__real__ (cv));
}

inline static double creal( dcomplex cv )
{
    return (__real__ (cv));
}

inline static long double creal( ldcomplex cv )
{
    return (__real__ (cv));
}

inline static float &creal( fcomplex *cv )
{
    return ((float *)cv)[0];
}

inline static double &creal( dcomplex *cv )
{
    return ((double *)cv)[0];
}

inline static long double &creal( ldcomplex *cv )
{
    return ((long double *)cv)[0];
}

inline static float cimag( fcomplex cv )
{
    return (__imag__ (cv));
}

inline static double cimag( dcomplex cv )
{
    return (__imag__ (cv));
}

inline static long double cimag( ldcomplex cv )
{
    return (__imag__ (cv));
}

inline static float &cimag( fcomplex *cv )
{
    return ((float *)cv)[1];
}

inline static double &cimag( dcomplex *cv )
{
    return ((double *)cv)[1];
}

inline static long double &cimag( ldcomplex *cv )
{
    return ((long double *)cv)[1];
}

inline static fcomplex conj( fcomplex cv )
{
    return (creal(cv) - I * cimag(cv));
}

inline static dcomplex conj( dcomplex cv )
{
    return (creal(cv) - I * cimag(cv));
}

inline static ldcomplex conj( ldcomplex cv )
{
    return (creal(cv) - I * cimag(cv));
}

/* double complex */

/**
 * @brief 複素数の絶対値を計算する
 * @param  v 複素数
 * @return 計算結果
 */
inline static double cabs( dcomplex v )
{ 
    double v_r = creal(v);
    double v_i = cimag(v);
    return sqrt(v_r * v_r + v_i * v_i);
}

/**
 * @brief 複素平面上の偏角を計算する
 * @param  v 複素数
 * @return 計算結果
 */
inline static double carg( dcomplex v )
{ 
    return atan2( cimag(v), creal(v) );
}

/**
 * @brief 複素数の exp を計算する
 * @param  v 複素数
 * @return 計算結果
 */
inline static dcomplex cexp( dcomplex v )
{ 
    return exp( creal(v) ) * cos( cimag(v) )  
	   + exp( creal(v) ) * sin( cimag(v) ) * I;
}

/**
 * @brief 複素数の平方根を計算する
 * @param  v 複素数
 * @return 計算結果
 */
inline static dcomplex csqrt( dcomplex v )
{ 
    double v_r = creal(v);
    double v_i = cimag(v);
    double vah = atan2(v_i, v_r) * 0.5;
    return sqrt(sqrt(v_r * v_r + v_i * v_i)) * (cos(vah) + I * sin(vah));
}

/**
 * @brief 複素自然対数を計算する
 * @param  v 複素数
 * @return 計算結果
 */
inline static dcomplex clog( dcomplex v )
{ 
    return log(cabs(v)) + I * carg(v);
}

/**
 * @brief リーマン球への射影を計算する
 * @param  v 複素数
 * @return 計算結果
 * @note コードがトリッキーか? sample/MDARRAY の 75番でテストすべし．
 *       glibc の cproj() はおかしな値を返すらしい．
 */
inline static dcomplex cproj( dcomplex v )
{
    const double m_infval = -1.0 * INFINITY;		/* -INF */
    const double m_zero = 1.0 / m_infval;		/* -0.0 */
    const double v_i = cimag(v);
    if ( isinf(creal(v)) || isinf(v_i) ) {
	dcomplex ret;
	if ( signbit(v_i) ) {
	    /* 注意: 負のゼロは a + b*I の形を使うとうまく代入できない */
	    creal(&ret) = INFINITY;
	    cimag(&ret) = m_zero;
	    return ret;
	}
	else {
	    creal(&ret) = INFINITY;
	    cimag(&ret) = 0.0;
	    return ret;
	}
    }
    else return v;
}

/**
 * @brief 複素数の sin を計算する
 * @param  v 複素数
 * @return 計算結果
 */
inline static dcomplex csin( dcomplex v )
{
    return sin( creal(v) ) * cosh( cimag(v) )
	   + cos( creal(v) ) * sinh( cimag(v) )*I;
}

/**
 * @brief 複素数の cos を計算する
 * @param  v 複素数
 * @return 計算結果
 */
inline static dcomplex ccos( dcomplex v )
{
    return cos( creal(v) ) * cosh( cimag(v) )
	   - sin( creal(v) ) * sinh( cimag(v) )*I;
}

/**
 * @brief 複素数の tan を計算する
 * @param  v 複素数
 * @return 計算結果
 */
inline static dcomplex ctan( dcomplex v )
{
    return (sin(2.0 * creal(v)) + I * sinh(2.0*cimag(v))) /
	   (cos(2.0 * creal(v)) + cosh(2.0*cimag(v)));
}

/**
 * @brief 複素数双曲線正弦を計算する
 * @param  v 複素数
 * @return 計算結果
 */
inline static dcomplex csinh( dcomplex v )
{ 
    return sinh(creal(v)) * cos(cimag(v)) + I * cosh(creal(v)) * sin(cimag(v));
}

/**
 * @brief 複素数双曲線余弦を計算する
 * @param  v 複素数
 * @return 計算結果
 */
inline static dcomplex ccosh( dcomplex v )
{ 
    return cosh(creal(v)) * cos(cimag(v)) + I * sinh(creal(v)) * sin(cimag(v));
}

/**
 * @brief 複素数双曲線正接を計算する
 * @param  v 複素数
 * @return 計算結果
 */
inline static dcomplex ctanh( dcomplex v )
{
    return (sinh(2.0 * creal(v)) + I * sin(2.0 * cimag(v))) / 
	   (cosh(2.0 * creal(v)) + cos(2.0 * cimag(v)));
}

/**
 * @brief 複素数逆正弦を計算する
 * @param  v 複素数
 * @return 計算結果
 */
inline static dcomplex casin( dcomplex v )
{
    return -1.0 * I * clog(v * I + csqrt(1.0 - v * v));
}

/**
 * @brief 複素数逆余弦を計算する
 * @param  v 複素数
 * @return 計算結果
 */
inline static dcomplex cacos( dcomplex v )
{
    return -2.0 * I * clog(csqrt((1.0 + v) * 0.5) +
	   I * csqrt((1.0 - v) * 0.5));
}

/**
 * @brief 複素数逆正接を計算する
 * @param  v 複素数
 * @return 計算結果
 */
inline static dcomplex catan( dcomplex v )
{
    return I * 0.5 * clog((I + v) / (I - v));
}

/**
 * @brief 複素数逆双曲線正弦を計算する
 * @param  v 複素数
 * @return 計算結果
 */
inline static dcomplex casinh( dcomplex v )
{
    return clog(v + csqrt(1.0 + v * v));
}

/**
 * @brief 複素数逆双曲線余弦を計算する
 * @param  v 複素数
 * @return 計算結果
 */
inline static dcomplex cacosh( dcomplex v )
{
    return clog(v + csqrt(v - 1.0) * csqrt(v + 1.0));
}

/**
 * @brief 複素数逆双曲線正接を計算する
 * @param  v 複素数
 * @return 計算結果
 */
inline static dcomplex catanh( dcomplex v )
{
    return clog((1.0 + v) / (1.0 - v)) * 0.5;
}

/**
 * @brief 複素数のべき乗を計算する
 * @param  v 複素数
 * @return 計算結果
 */
inline static dcomplex cpow( dcomplex v1, dcomplex v2 )
{
    return cexp(v2 * clog(v1));
}


}

#endif
