#include "_test_sse2.h"
#include "s_ope.h"

#ifdef _SSE2_IS_OK
#include <emmintrin.h>
#include "_tune_sse2.h"
#endif

static void s_xeq_n_float( void *_dst, const void *_src0, size_t _step0, size_t n, int ope )
{
    size_t i=0;
    float *dst = (float *)_dst;
    const float *src0 = (const float *)_src0;
    const size_t step0 = _step0;
#ifdef _SSE2_IS_OK
    const size_t step_sse32 = 32 / sizeof(float);	/* 8 */
    const size_t n_sse32 = n / step_sse32;
    const size_t nel_sse32 = step_sse32 * n_sse32;
#endif
    if ( ope == S_OPE_PLUS ) {
#ifdef _SSE2_IS_OK
	if ( _SSE2_CALC_MIN_NBYTES <= sizeof(float) * n ) {
	    if ( step0 == 1 ) {
		__m128 s0, s1, d0, d1;
		for ( ; i < nel_sse32 ; i+=step_sse32 ) {
		    s0 = _mm_loadu_ps( src0 + i );
		    s1 = _mm_loadu_ps( src0 + i + 4 );
		    d0 = _mm_loadu_ps( dst + i );
		    d1 = _mm_loadu_ps( dst + i + 4 );
		    d0 = _mm_add_ps(d0, s0);
		    d1 = _mm_add_ps(d1, s1);
		    _mm_storeu_ps(dst + i, d0);
		    _mm_storeu_ps(dst + i + 4, d1);
		}
		src0 += step_sse32 * n_sse32;
	    }
	    else if ( step0 == 0 ) {
		__m128 s0, d0, d1;
		s0 = _mm_set1_ps(*src0);
		for ( ; i < nel_sse32 ; i+=step_sse32 ) {
		    d0 = _mm_loadu_ps( dst + i );
		    d1 = _mm_loadu_ps( dst + i + 4 );
		    d0 = _mm_add_ps(d0, s0);
		    d1 = _mm_add_ps(d1, s0);
		    _mm_storeu_ps(dst + i, d0);
		    _mm_storeu_ps(dst + i + 4, d1);
		}
	    }
	}
#endif
	for ( ; i < n ; i++,src0+=step0 ) dst[i] += (*src0);
    }
    else if ( ope == S_OPE_MINUS ) {
#ifdef _SSE2_IS_OK
	if ( _SSE2_CALC_MIN_NBYTES <= sizeof(float) * n ) {
	    if ( step0 == 1 ) {
		__m128 s0, s1, d0, d1;
		for ( ; i < nel_sse32 ; i+=step_sse32 ) {
		    s0 = _mm_loadu_ps( src0 + i );
		    s1 = _mm_loadu_ps( src0 + i + 4 );
		    d0 = _mm_loadu_ps( dst + i );
		    d1 = _mm_loadu_ps( dst + i + 4 );
		    d0 = _mm_sub_ps(d0, s0);
		    d1 = _mm_sub_ps(d1, s1);
		    _mm_storeu_ps(dst + i, d0);
		    _mm_storeu_ps(dst + i + 4, d1);
		}
		src0 += step_sse32 * n_sse32;
	    }
	    else if ( step0 == 0 ) {
		__m128 s0, d0, d1;
		s0 = _mm_set1_ps(*src0);
		for ( ; i < nel_sse32 ; i+=step_sse32 ) {
		    d0 = _mm_loadu_ps( dst + i );
		    d1 = _mm_loadu_ps( dst + i + 4 );
		    d0 = _mm_sub_ps(d0, s0);
		    d1 = _mm_sub_ps(d1, s0);
		    _mm_storeu_ps(dst + i, d0);
		    _mm_storeu_ps(dst + i + 4, d1);
		}
	    }
	}
#endif
	for ( ; i < n ; i++,src0+=step0 ) dst[i] -= (*src0);
    }
    else if ( ope == S_OPE_STAR ) {
#ifdef _SSE2_IS_OK
	if ( _SSE2_CALC_MIN_NBYTES <= sizeof(float) * n ) {
	    if ( step0 == 1 ) {
		__m128 s0, s1, d0, d1;
		for ( ; i < nel_sse32 ; i+=step_sse32 ) {
		    s0 = _mm_loadu_ps( src0 + i );
		    s1 = _mm_loadu_ps( src0 + i + 4 );
		    d0 = _mm_loadu_ps( dst + i );
		    d1 = _mm_loadu_ps( dst + i + 4 );
		    d0 = _mm_mul_ps(d0, s0);
		    d1 = _mm_mul_ps(d1, s1);
		    _mm_storeu_ps(dst + i, d0);
		    _mm_storeu_ps(dst + i + 4, d1);
		}
		src0 += step_sse32 * n_sse32;
	    }
	    else if ( step0 == 0 ) {
		__m128 s0, d0, d1;
		s0 = _mm_set1_ps(*src0);
		for ( ; i < nel_sse32 ; i+=step_sse32 ) {
		    d0 = _mm_loadu_ps( dst + i );
		    d1 = _mm_loadu_ps( dst + i + 4 );
		    d0 = _mm_mul_ps(d0, s0);
		    d1 = _mm_mul_ps(d1, s0);
		    _mm_storeu_ps(dst + i, d0);
		    _mm_storeu_ps(dst + i + 4, d1);
		}
	    }
	}
#endif
	for ( ; i < n ; i++,src0+=step0 ) dst[i] *= (*src0);
    }
    else {
#ifdef _SSE2_IS_OK
	if ( _SSE2_CALC_MIN_NBYTES <= sizeof(float) * n ) {
	    if ( step0 == 1 ) {
		__m128 s0, s1, d0, d1;
		for ( ; i < nel_sse32 ; i+=step_sse32 ) {
		    s0 = _mm_loadu_ps( src0 + i );
		    s1 = _mm_loadu_ps( src0 + i + 4 );
		    d0 = _mm_loadu_ps( dst + i );
		    d1 = _mm_loadu_ps( dst + i + 4 );
		    d0 = _mm_div_ps(d0, s0);
		    d1 = _mm_div_ps(d1, s1);
		    _mm_storeu_ps(dst + i, d0);
		    _mm_storeu_ps(dst + i + 4, d1);
		}
		src0 += step_sse32 * n_sse32;
	    }
	    else if ( step0 == 0 ) {
		__m128 s0, d0, d1;
		s0 = _mm_set1_ps(*src0);
		for ( ; i < nel_sse32 ; i+=step_sse32 ) {
		    d0 = _mm_loadu_ps( dst + i );
		    d1 = _mm_loadu_ps( dst + i + 4 );
		    d0 = _mm_div_ps(d0, s0);
		    d1 = _mm_div_ps(d1, s0);
		    _mm_storeu_ps(dst + i, d0);
		    _mm_storeu_ps(dst + i + 4, d1);
		}
	    }
	}
#endif
	for ( ; i < n ; i++,src0+=step0 ) dst[i] /= (*src0);
    }
}
