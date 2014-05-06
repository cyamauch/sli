/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2013-04-14 17:18:00 cyamauch> */

#ifndef _SLI__MDARRAY_COMPLEX
#define _SLI__MDARRAY_COMPLEX 1

/**
 * @file   mdarray_complex.h
 * @brief  mdarrayクラスとその継承クラスのオブジェクトで使用可能な複素関数
 */

#include "complex.h"
#include "mdarray.h"

namespace sli
{

extern mdarray creal( const mdarray &src );
extern mdarray cimag( const mdarray &src );
extern mdarray conj( const mdarray &src );
extern mdarray cabs( const mdarray &src );
extern mdarray carg( const mdarray &src );
extern mdarray cexp( const mdarray &obj );
extern mdarray csqrt( const mdarray &obj );
extern mdarray clog( const mdarray &obj );
extern mdarray cproj( const mdarray &obj );
extern mdarray csin( const mdarray &obj );
extern mdarray ccos( const mdarray &obj );
extern mdarray ctan( const mdarray &obj );
extern mdarray csinh( const mdarray &obj );
extern mdarray ccosh( const mdarray &obj );
extern mdarray ctanh( const mdarray &obj );
extern mdarray casin( const mdarray &obj );
extern mdarray cacos( const mdarray &obj );
extern mdarray catan( const mdarray &obj );
extern mdarray casinh( const mdarray &obj );
extern mdarray cacosh( const mdarray &obj );
extern mdarray catanh( const mdarray &obj );
extern mdarray cpow( const mdarray &obj, dcomplex v );
extern mdarray cpow( dcomplex v, const mdarray &obj );
extern mdarray cpow( const mdarray &src0, const mdarray &src1 );

extern mdarray calcx1( const mdarray &src,
		       dcomplex (*func)(dcomplex), fcomplex (*funcf)(fcomplex),
		       ldcomplex (*funcl)(ldcomplex) );
extern mdarray calcx2( const mdarray &src0, 
		       ldcomplex src1, ssize_t szt_src1, bool rv,
		       dcomplex (*func)(dcomplex,dcomplex),
		       fcomplex (*funcf)(fcomplex,fcomplex),
		       ldcomplex (*funcl)(ldcomplex,ldcomplex) );
extern mdarray calcx2( const mdarray &src0, const mdarray &src1,
		       dcomplex (*func)(dcomplex,dcomplex),
		       fcomplex (*funcf)(fcomplex,fcomplex),
		       ldcomplex (*funcl)(ldcomplex,ldcomplex) );

}

#if 1
/* */
#include "mdarray_fcomplex.h"
#include "mdarray_dcomplex.h"
/* */
#endif

#endif	/* _SLI__MDARRAY_COMPLEX */

