/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2013-04-14 17:18:41 cyamauch> */

#ifndef _SLI__MDARRAY_MATH
#define _SLI__MDARRAY_MATH 1

/**
 * @file   mdarray_math.h
 * @brief  mdarrayクラスとその継承クラスのオブジェクトで使用可能な数学関数
 */

#include "mdarray.h"

namespace sli
{

extern mdarray cbrt( const mdarray &obj );
extern mdarray sqrt( const mdarray &obj );
extern mdarray asin( const mdarray &obj );
extern mdarray acos( const mdarray &obj );
extern mdarray atan( const mdarray &obj );
extern mdarray acosh( const mdarray &obj );
extern mdarray asinh( const mdarray &obj );
extern mdarray atanh( const mdarray &obj );
extern mdarray exp( const mdarray &obj );
extern mdarray exp2( const mdarray &obj );
extern mdarray expm1( const mdarray &obj );
extern mdarray log( const mdarray &obj );
extern mdarray log1p( const mdarray &obj );
extern mdarray log10( const mdarray &obj );
extern mdarray sin( const mdarray &obj );
extern mdarray cos( const mdarray &obj );
extern mdarray tan( const mdarray &obj );
extern mdarray sinh( const mdarray &obj );
extern mdarray cosh( const mdarray &obj );
extern mdarray tanh( const mdarray &obj );
extern mdarray erf( const mdarray &obj );
extern mdarray erfc( const mdarray &obj );
extern mdarray ceil( const mdarray &obj );
extern mdarray floor( const mdarray &obj );
extern mdarray round( const mdarray &obj );
extern mdarray trunc( const mdarray &obj );
extern mdarray fabs( const mdarray &obj );
extern mdarray hypot( const mdarray &obj, double v );
extern mdarray hypot( double v, const mdarray &obj );
extern mdarray hypot( const mdarray &src0, const mdarray &src1 );
extern mdarray pow( const mdarray &obj, double v );
extern mdarray pow( double v, const mdarray &obj );
extern mdarray pow( const mdarray &src0, const mdarray &src1 );
extern mdarray fmod( const mdarray &obj, double v );
extern mdarray fmod( double v, const mdarray &obj );
extern mdarray fmod( const mdarray &src0, const mdarray &src1 );
extern mdarray remainder( const mdarray &obj, double v );
extern mdarray remainder( double v, const mdarray &obj );
extern mdarray remainder( const mdarray &src0, const mdarray &src1 );

#if 0
extern double cbrt( double v );
extern double sqrt( double v );
extern double asin( double v );
extern double acos( double v );
extern double atan( double v );
extern double acosh( double v );
extern double asinh( double v );
extern double atanh( double v );
extern double exp( double v );
extern double exp2( double v );
extern double expm1( double v );
extern double log( double v );
extern double log1p( double v );
extern double log10( double v );
extern double sin( double v );
extern double cos( double v );
extern double tan( double v );
extern double sinh( double v );
extern double cosh( double v );
extern double tanh( double v );
extern double erf( double v );
extern double erfc( double v );
extern double ceil( double v );
extern double floor( double v );
extern double round( double v );
extern double trunc( double v );
extern double fabs( double v );

extern double pow( double v0, double v1 );
extern double fmod( double v0, double v1 );
extern double remainder( double v0, double v1 );
extern double hypot( double v0, double v1 );
#endif	/* 0 */

extern mdarray calc1( const mdarray &src,
		      double (*func)(double), float (*funcf)(float),
		      long double (*funcl)(long double) );
extern mdarray calc2( const mdarray &src0, 
		      long double src1, ssize_t szt_src1, bool rv,
		      double (*func)(double,double),
		      float (*funcf)(float,float),
		      long double (*funcl)(long double,long double) );
extern mdarray calc2( const mdarray &src0, const mdarray &src1,
		      double (*func)(double,double),
		      float (*funcf)(float,float),
		      long double (*funcl)(long double,long double) );


}	/* namespace sli */

#if 0
inline double sli::cbrt( double v )
{
    return cbrt(v);
}
inline double sli::sqrt( double v )
{
    return sqrt(v);
}
inline double sli::asin( double v )
{
    return asin(v);
}
inline double sli::acos( double v )
{
    return acos(v);
}
inline double sli::atan( double v )
{
    return atan(v);
}
inline double sli::acosh( double v )
{
    return acosh(v);
}
inline double sli::asinh( double v )
{
    return asinh(v);
}
inline double sli::atanh( double v )
{
    return atanh(v);
}
inline double sli::exp( double v )
{
    return exp(v);
}
inline double sli::exp2( double v )
{
    return exp2(v);
}
inline double sli::expm1( double v )
{
    return expm1(v);
}
inline double sli::log( double v )
{
    return log(v);
}
inline double sli::log1p( double v )
{
    return log1p(v);
}
inline double sli::log10( double v )
{
    return log10(v);
}
inline double sli::sin( double v )
{
    return sin(v);
}
inline double sli::cos( double v )
{
    return cos(v);
}
inline double sli::tan( double v )
{
    return tan(v);
}
inline double sli::sinh( double v )
{
    return sinh(v);
}
inline double sli::cosh( double v )
{
    return cosh(v);
}
inline double sli::tanh( double v )
{
    return tanh(v);
}
inline double sli::erf( double v )
{
    return erf(v);
}
inline double sli::erfc( double v )
{
    return erfc(v);
}
inline double sli::ceil( double v )
{
    return ceil(v);
}
inline double sli::floor( double v )
{
    return floor(v);
}
inline double sli::round( double v )
{
    return round(v);
}
inline double sli::trunc( double v )
{
    return trunc(v);
}
inline double sli::fabs( double v )
{
    return fabs(v);
}

inline double sli::pow( double v0, double v1 )
{
    return pow(v0, v1);
}
inline double sli::fmod( double v0, double v1 )
{
    return fmod(v0, v1);
}
inline double sli::remainder( double v0, double v1 )
{
    return remainder(v0, v1);
}
inline double sli::hypot( double v0, double v1 )
{
    return hypot(v0, v1);
}
#endif	/* 0 */

#endif	/* _SLI__MDARRAY_MATH */
