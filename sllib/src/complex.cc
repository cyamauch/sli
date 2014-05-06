/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2012-09-24 22:32:47 cyamauch> */

#include "config.h"

#include "complex.h"

namespace sli
{

#if 0

float creal( fcomplex cv )
{
    return (__real__ (cv));
}

double creal( dcomplex cv )
{
    return (__real__ (cv));
}

long double creal( ldcomplex cv )
{
    return (__real__ (cv));
}

float cimag( fcomplex cv )
{
    return (__imag__ (cv));
}

double cimag( dcomplex cv )
{
    return (__imag__ (cv));
}

long double cimag( ldcomplex cv )
{
    return (__imag__ (cv));
}

fcomplex conj( fcomplex cv )
{
    return (creal(cv) - I * cimag(cv));
}

dcomplex conj( dcomplex cv )
{
    return (creal(cv) - I * cimag(cv));
}

ldcomplex conj( ldcomplex cv )
{
    return (creal(cv) - I * cimag(cv));
}

#endif	/* #if 0 */

}	/* namespace sli */
