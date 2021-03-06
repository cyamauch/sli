#include "_test_sse2.h"
#include "s_ope.h"

#ifdef _SSE2_IS_OK
#include <emmintrin.h>
#include "_tune_sse2.h"
#endif

/* dst[]     =  src0[]   +   src1[]                   */
/* type:new     type:new     type:new   <= N,N,N      */
/* _step0, _step1 が 0 なら当該はスカラー，1 なら配列 */
static void s_x_nnn_n_double( void *_dst, const void *_src0, size_t _step0, const void *_src1, size_t _step1, size_t n, int ope )
{
    size_t k=0;
    double *dst = (double *)_dst;
    const double *src0 = (const double *)_src0;
    const double *src1 = (const double *)_src1;
    const size_t step0 = _step0;
    const size_t step1 = _step1;
#ifdef _SSE2_IS_OK
    const size_t step_sse32 = 32 / sizeof(double);	/* 4 */
    const size_t n_sse32 = n / step_sse32;
    const size_t nel_sse32 = step_sse32 * n_sse32;
#endif
    if ( src0 != NULL ) {
      if ( src1 != NULL ) {
	if ( ope == S_OPE_PLUS ) {
#ifdef _SSE2_IS_OK
	    if ( step0 == 1 ) {
		if ( step1 == 1 ) {
		    __m128d s0, s1, r0, r1;
		    for ( ; k < nel_sse32 ; k+=step_sse32 ) {
			s0 = _mm_loadu_pd( src0 + k );
			s1 = _mm_loadu_pd( src0 + k + 2 );
			r0 = _mm_loadu_pd( src1 + k );
			r1 = _mm_loadu_pd( src1 + k + 2 );
			s0 = _mm_add_pd(s0, r0);
			s1 = _mm_add_pd(s1, r1);
			_mm_storeu_pd(dst + k, s0);
			_mm_storeu_pd(dst + k + 2, s1);
		    }
		    src0 += step_sse32 * n_sse32;
		    src1 += step_sse32 * n_sse32;
		}
		else if ( step1 == 0 ) {
		    __m128d s0, s1, r0;
		    r0 = _mm_set1_pd(*src1);
		    for ( ; k < nel_sse32 ; k+=step_sse32 ) {
			s0 = _mm_loadu_pd( src0 + k );
			s1 = _mm_loadu_pd( src0 + k + 2 );
			s0 = _mm_add_pd(s0, r0);
			s1 = _mm_add_pd(s1, r0);
			_mm_storeu_pd(dst + k, s0);
			_mm_storeu_pd(dst + k + 2, s1);
		    }
		    src0 += step_sse32 * n_sse32;
		}
	    }
	    else if ( step0 == 0 ) {
		if ( step1 == 1 ) {
		    __m128d s0, r0, r1;
		    s0 = _mm_set1_pd(*src0);
		    for ( ; k < nel_sse32 ; k+=step_sse32 ) {
			r0 = _mm_loadu_pd( src1 + k );
			r1 = _mm_loadu_pd( src1 + k + 2 );
			r0 = _mm_add_pd(s0, r0);
			r1 = _mm_add_pd(s0, r1);
			_mm_storeu_pd(dst + k, r0);
			_mm_storeu_pd(dst + k + 2, r1);
		    }
		    src1 += step_sse32 * n_sse32;
		}
		else if ( step1 == 0 ) {
		    __m128d s0, r0;
		    s0 = _mm_set1_pd(*src0);
		    r0 = _mm_set1_pd(*src1);
		    s0 = _mm_add_pd(s0, r0);
		    for ( ; k < nel_sse32 ; k+=step_sse32 ) {
			_mm_storeu_pd(dst + k, s0);
			_mm_storeu_pd(dst + k + 2, s0);
		    }
		}
	    }
#endif
	    for ( ; k < n ; k++, src0+=step0, src1+=step1 )
		dst[k] = (*src0) + (*src1);
	}
	else if ( ope == S_OPE_MINUS ) {
#ifdef _SSE2_IS_OK
	    if ( step0 == 1 ) {
		if ( step1 == 1 ) {
		    __m128d s0, s1, r0, r1;
		    for ( ; k < nel_sse32 ; k+=step_sse32 ) {
			s0 = _mm_loadu_pd( src0 + k );
			s1 = _mm_loadu_pd( src0 + k + 2 );
			r0 = _mm_loadu_pd( src1 + k );
			r1 = _mm_loadu_pd( src1 + k + 2 );
			s0 = _mm_sub_pd(s0, r0);
			s1 = _mm_sub_pd(s1, r1);
			_mm_storeu_pd(dst + k, s0);
			_mm_storeu_pd(dst + k + 2, s1);
		    }
		    src0 += step_sse32 * n_sse32;
		    src1 += step_sse32 * n_sse32;
		}
		else if ( step1 == 0 ) {
		    __m128d s0, s1, r0;
		    r0 = _mm_set1_pd(*src1);
		    for ( ; k < nel_sse32 ; k+=step_sse32 ) {
			s0 = _mm_loadu_pd( src0 + k );
			s1 = _mm_loadu_pd( src0 + k + 2 );
			s0 = _mm_sub_pd(s0, r0);
			s1 = _mm_sub_pd(s1, r0);
			_mm_storeu_pd(dst + k, s0);
			_mm_storeu_pd(dst + k + 2, s1);
		    }
		    src0 += step_sse32 * n_sse32;
		}
	    }
	    else if ( step0 == 0 ) {
		if ( step1 == 1 ) {
		    __m128d s0, r0, r1;
		    s0 = _mm_set1_pd(*src0);
		    for ( ; k < nel_sse32 ; k+=step_sse32 ) {
			r0 = _mm_loadu_pd( src1 + k );
			r1 = _mm_loadu_pd( src1 + k + 2 );
			r0 = _mm_sub_pd(s0, r0);
			r1 = _mm_sub_pd(s0, r1);
			_mm_storeu_pd(dst + k, r0);
			_mm_storeu_pd(dst + k + 2, r1);
		    }
		    src1 += step_sse32 * n_sse32;
		}
		else if ( step1 == 0 ) {
		    __m128d s0, r0;
		    s0 = _mm_set1_pd(*src0);
		    r0 = _mm_set1_pd(*src1);
		    s0 = _mm_sub_pd(s0, r0);
		    for ( ; k < nel_sse32 ; k+=step_sse32 ) {
			_mm_storeu_pd(dst + k, s0);
			_mm_storeu_pd(dst + k + 2, s0);
		    }
		}
	    }
#endif
	    for ( ; k < n ; k++, src0+=step0, src1+=step1 )
		dst[k] = (*src0) - (*src1);
	}
	else if ( ope == S_OPE_STAR )  {
#ifdef _SSE2_IS_OK
	    if ( step0 == 1 ) {
		if ( step1 == 1 ) {
		    __m128d s0, s1, r0, r1;
		    for ( ; k < nel_sse32 ; k+=step_sse32 ) {
			s0 = _mm_loadu_pd( src0 + k );
			s1 = _mm_loadu_pd( src0 + k + 2 );
			r0 = _mm_loadu_pd( src1 + k );
			r1 = _mm_loadu_pd( src1 + k + 2 );
			s0 = _mm_mul_pd(s0, r0);
			s1 = _mm_mul_pd(s1, r1);
			_mm_storeu_pd(dst + k, s0);
			_mm_storeu_pd(dst + k + 2, s1);
		    }
		    src0 += step_sse32 * n_sse32;
		    src1 += step_sse32 * n_sse32;
		}
		else if ( step1 == 0 ) {
		    __m128d s0, s1, r0;
		    r0 = _mm_set1_pd(*src1);
		    for ( ; k < nel_sse32 ; k+=step_sse32 ) {
			s0 = _mm_loadu_pd( src0 + k );
			s1 = _mm_loadu_pd( src0 + k + 2 );
			s0 = _mm_mul_pd(s0, r0);
			s1 = _mm_mul_pd(s1, r0);
			_mm_storeu_pd(dst + k, s0);
			_mm_storeu_pd(dst + k + 2, s1);
		    }
		    src0 += step_sse32 * n_sse32;
		}
	    }
	    else if ( step0 == 0 ) {
		if ( step1 == 1 ) {
		    __m128d s0, r0, r1;
		    s0 = _mm_set1_pd(*src0);
		    for ( ; k < nel_sse32 ; k+=step_sse32 ) {
			r0 = _mm_loadu_pd( src1 + k );
			r1 = _mm_loadu_pd( src1 + k + 2 );
			r0 = _mm_mul_pd(s0, r0);
			r1 = _mm_mul_pd(s0, r1);
			_mm_storeu_pd(dst + k, r0);
			_mm_storeu_pd(dst + k + 2, r1);
		    }
		    src1 += step_sse32 * n_sse32;
		}
		else if ( step1 == 0 ) {
		    __m128d s0, r0;
		    s0 = _mm_set1_pd(*src0);
		    r0 = _mm_set1_pd(*src1);
		    s0 = _mm_mul_pd(s0, r0);
		    for ( ; k < nel_sse32 ; k+=step_sse32 ) {
			_mm_storeu_pd(dst + k, s0);
			_mm_storeu_pd(dst + k + 2, s0);
		    }
		}
	    }
#endif
	    for ( ; k < n ; k++, src0+=step0, src1+=step1 )
		dst[k] = (*src0) * (*src1);
	}
	else {
#ifdef _SSE2_IS_OK
	    if ( step0 == 1 ) {
		if ( step1 == 1 ) {
		    __m128d s0, s1, r0, r1;
		    for ( ; k < nel_sse32 ; k+=step_sse32 ) {
			s0 = _mm_loadu_pd( src0 + k );
			s1 = _mm_loadu_pd( src0 + k + 2 );
			r0 = _mm_loadu_pd( src1 + k );
			r1 = _mm_loadu_pd( src1 + k + 2 );
			s0 = _mm_div_pd(s0, r0);
			s1 = _mm_div_pd(s1, r1);
			_mm_storeu_pd(dst + k, s0);
			_mm_storeu_pd(dst + k + 2, s1);
		    }
		    src0 += step_sse32 * n_sse32;
		    src1 += step_sse32 * n_sse32;
		}
		else if ( step1 == 0 ) {
		    __m128d s0, s1, r0;
		    r0 = _mm_set1_pd(*src1);
		    for ( ; k < nel_sse32 ; k+=step_sse32 ) {
			s0 = _mm_loadu_pd( src0 + k );
			s1 = _mm_loadu_pd( src0 + k + 2 );
			s0 = _mm_div_pd(s0, r0);
			s1 = _mm_div_pd(s1, r0);
			_mm_storeu_pd(dst + k, s0);
			_mm_storeu_pd(dst + k + 2, s1);
		    }
		    src0 += step_sse32 * n_sse32;
		}
	    }
	    else if ( step0 == 0 ) {
		if ( step1 == 1 ) {
		    __m128d s0, r0, r1;
		    s0 = _mm_set1_pd(*src0);
		    for ( ; k < nel_sse32 ; k+=step_sse32 ) {
			r0 = _mm_loadu_pd( src1 + k );
			r1 = _mm_loadu_pd( src1 + k + 2 );
			r0 = _mm_div_pd(s0, r0);
			r1 = _mm_div_pd(s0, r1);
			_mm_storeu_pd(dst + k, r0);
			_mm_storeu_pd(dst + k + 2, r1);
		    }
		    src1 += step_sse32 * n_sse32;
		}
		else if ( step1 == 0 ) {
		    __m128d s0, r0;
		    s0 = _mm_set1_pd(*src0);
		    r0 = _mm_set1_pd(*src1);
		    s0 = _mm_div_pd(s0, r0);
		    for ( ; k < nel_sse32 ; k+=step_sse32 ) {
			_mm_storeu_pd(dst + k, s0);
			_mm_storeu_pd(dst + k + 2, s0);
		    }
		}
	    }
#endif
	    for ( ; k < n ; k++, src0+=step0, src1+=step1 )
		dst[k] = (*src0) / (*src1);
	}
      } else {	/* src1 == NULL */
#ifdef _SSE2_IS_OK
	  if ( step0 == 1 ) {
	      __m128d s0, s1;
	      for ( ; k < nel_sse32 ; k+=step_sse32 ) {
		  s0 = _mm_loadu_pd( src0 + k );
		  s1 = _mm_loadu_pd( src0 + k + 2 );
		  _mm_storeu_pd(dst + k, s0);
		  _mm_storeu_pd(dst + k + 2, s1);
	      }
	      src0 += step_sse32 * n_sse32;
	  }
	  else if ( step0 == 0 ) {
	      __m128d s0;
	      s0 = _mm_set1_pd(*src0);
	      for ( ; k < nel_sse32 ; k+=step_sse32 ) {
		  _mm_storeu_pd(dst + k, s0);
		  _mm_storeu_pd(dst + k + 2, s0);
	      }
	  }
#endif
	  for ( ; k < n ; k++, src0+=step0 )
	      dst[k] = (*src0);
      }
    } else {	/* src0 == NULL */
      double zero;
      c_memset(&zero,0,sizeof(zero));
      if ( src1 != NULL ) {
        if ( ope == S_OPE_PLUS ) {
#ifdef _SSE2_IS_OK
	    if ( step1 == 1 ) {
		__m128d s0, r0, r1;
		s0 = _mm_set1_pd(zero);
		for ( ; k < nel_sse32 ; k+=step_sse32 ) {
		    r0 = _mm_loadu_pd( src1 + k );
		    r1 = _mm_loadu_pd( src1 + k + 2 );
		    r0 = _mm_add_pd(s0, r0);
		    r1 = _mm_add_pd(s0, r1);
		    _mm_storeu_pd(dst + k, r0);
		    _mm_storeu_pd(dst + k + 2, r1);
		}
		src1 += step_sse32 * n_sse32;
	    }
	    else if ( step1 == 0 ) {
		__m128d s0, r0;
		s0 = _mm_set1_pd(zero);
		r0 = _mm_set1_pd(*src1);
		s0 = _mm_add_pd(s0, r0);
		for ( ; k < nel_sse32 ; k+=step_sse32 ) {
		    _mm_storeu_pd(dst + k, s0);
		    _mm_storeu_pd(dst + k + 2, s0);
		}
	    }
#endif
	    for ( ; k < n ; k++, src1+=step1 ) dst[k] = zero + (*src1);
	}
	else if ( ope == S_OPE_MINUS ) {
#ifdef _SSE2_IS_OK
	    if ( step1 == 1 ) {
		__m128d s0, r0, r1;
		s0 = _mm_set1_pd(zero);
		for ( ; k < nel_sse32 ; k+=step_sse32 ) {
		    r0 = _mm_loadu_pd( src1 + k );
		    r1 = _mm_loadu_pd( src1 + k + 2 );
		    r0 = _mm_sub_pd(s0, r0);
		    r1 = _mm_sub_pd(s0, r1);
		    _mm_storeu_pd(dst + k, r0);
		    _mm_storeu_pd(dst + k + 2, r1);
		}
		src1 += step_sse32 * n_sse32;
	    }
	    else if ( step1 == 0 ) {
		__m128d s0, r0;
		s0 = _mm_set1_pd(zero);
		r0 = _mm_set1_pd(*src1);
		s0 = _mm_sub_pd(s0, r0);
		for ( ; k < nel_sse32 ; k+=step_sse32 ) {
		    _mm_storeu_pd(dst + k, s0);
		    _mm_storeu_pd(dst + k + 2, s0);
		}
	    }
#endif
	    for ( ; k < n ; k++, src1+=step1 ) dst[k] = zero - (*src1);
	}
	else if ( ope == S_OPE_STAR ) {
#ifdef _SSE2_IS_OK
	    if ( step1 == 1 ) {
		__m128d s0, r0, r1;
		s0 = _mm_set1_pd(zero);
		for ( ; k < nel_sse32 ; k+=step_sse32 ) {
		    r0 = _mm_loadu_pd( src1 + k );
		    r1 = _mm_loadu_pd( src1 + k + 2 );
		    r0 = _mm_mul_pd(s0, r0);
		    r1 = _mm_mul_pd(s0, r1);
		    _mm_storeu_pd(dst + k, r0);
		    _mm_storeu_pd(dst + k + 2, r1);
		}
		src1 += step_sse32 * n_sse32;
	    }
	    else if ( step1 == 0 ) {
		__m128d s0, r0;
		s0 = _mm_set1_pd(zero);
		r0 = _mm_set1_pd(*src1);
		s0 = _mm_mul_pd(s0, r0);
		for ( ; k < nel_sse32 ; k+=step_sse32 ) {
		    _mm_storeu_pd(dst + k, s0);
		    _mm_storeu_pd(dst + k + 2, s0);
		}
	    }
#endif
	    for ( ; k < n ; k++, src1+=step1 ) dst[k] = zero * (*src1);
	}
	else {
#ifdef _SSE2_IS_OK
	    if ( step1 == 1 ) {
		__m128d s0, r0, r1;
		s0 = _mm_set1_pd(zero);
		for ( ; k < nel_sse32 ; k+=step_sse32 ) {
		    r0 = _mm_loadu_pd( src1 + k );
		    r1 = _mm_loadu_pd( src1 + k + 2 );
		    r0 = _mm_div_pd(s0, r0);
		    r1 = _mm_div_pd(s0, r1);
		    _mm_storeu_pd(dst + k, r0);
		    _mm_storeu_pd(dst + k + 2, r1);
		}
		src1 += step_sse32 * n_sse32;
	    }
	    else if ( step1 == 0 ) {
		__m128d s0, r0;
		s0 = _mm_set1_pd(zero);
		r0 = _mm_set1_pd(*src1);
		s0 = _mm_div_pd(s0, r0);
		for ( ; k < nel_sse32 ; k+=step_sse32 ) {
		    _mm_storeu_pd(dst + k, s0);
		    _mm_storeu_pd(dst + k + 2, s0);
		}
	    }
#endif
	    for ( ; k < n ; k++, src1+=step1 ) dst[k] = zero / (*src1);
	}
      } else {	/* src1 == NULL */
#ifdef _SSE2_IS_OK
	  __m128d s0;
	  s0 = _mm_set1_pd(zero);
	  for ( ; k < nel_sse32 ; k+=step_sse32 ) {
	      _mm_storeu_pd(dst + k, s0);
	      _mm_storeu_pd(dst + k + 2, s0);
	  }
#endif
	  for ( ; k < n ; k++ ) dst[k] = zero;
      }
    }
}
