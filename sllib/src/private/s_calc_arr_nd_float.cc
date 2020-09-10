#include "_test_sse2.h"
#include "s_ope.h"

#ifdef _SSE2_IS_OK
#include <emmintrin.h>
#include "_tune_sse2.h"
#endif

/* NOTE: *org_val_ptr is Always double type when f_b is 0. */
static void s_calc_arr_nd_float(const void *org_val_ptr, void *new_val_ptr, size_t n, int f_b, void *u_ptr)
{
    size_t i;
    const int *ope_ptr = (const int *)u_ptr;
    const int ope = *(ope_ptr);
    const float *src = (const float *)org_val_ptr;
    float *dst = (float *)new_val_ptr;
    float float_v = 0;
    if ( f_b == 0 ) {
	float_v = (float)(( (const double *)org_val_ptr )[0]);
	src = &float_v;
    }
#ifdef _SSE2_IS_OK
    const size_t step_sse32 = 32 / sizeof(float);	/* 8 */
    const size_t n_sse32 = n / step_sse32;
    const size_t nel_sse32 = step_sse32 * n_sse32;
#endif
    if ( f_b < 0 ) {
	size_t i_break = 0;
	i = n;
#ifdef _SSE2_IS_OK
	if ( _SSE2_CALC_MIN_NBYTES <= sizeof(float) * n ) {
	    i_break = step_sse32 * n_sse32;
	}
#endif
	if ( ope == S_OPE_PLUS ) {
	    while ( i_break < i ) {
		i--;
		dst[i] += src[i];
	    }
#ifdef _SSE2_IS_OK
	    {
		__m128 s0, s1, d0, d1;
		while ( 0 < i ) {
		    i -= step_sse32;
		    s1 = _mm_loadu_ps( src + i + 4 );
		    s0 = _mm_loadu_ps( src + i );
		    d1 = _mm_loadu_ps( dst + i + 4 );
		    d0 = _mm_loadu_ps( dst + i );
		    d1 = _mm_add_ps(d1, s1);
		    d0 = _mm_add_ps(d0, s0);
		    _mm_storeu_ps(dst + i + 4, d1);
		    _mm_storeu_ps(dst + i, d0);
		}
	    }
#endif
	}
	else if ( ope == S_OPE_MINUS ) {
	    while ( i_break < i ) {
		i--;
		dst[i] -= src[i];
	    }
#ifdef _SSE2_IS_OK
	    {
		__m128 s0, s1, d0, d1;
		while ( 0 < i ) {
		    i -= step_sse32;
		    s1 = _mm_loadu_ps( src + i + 4 );
		    s0 = _mm_loadu_ps( src + i );
		    d1 = _mm_loadu_ps( dst + i + 4 );
		    d0 = _mm_loadu_ps( dst + i );
		    d1 = _mm_sub_ps(d1, s1);
		    d0 = _mm_sub_ps(d0, s0);
		    _mm_storeu_ps(dst + i + 4, d1);
		    _mm_storeu_ps(dst + i, d0);
		}
	    }
#endif
	}
	else if ( ope == S_OPE_STAR ) {
	    while ( i_break < i ) {
		i--;
		dst[i] *= src[i];
	    }
#ifdef _SSE2_IS_OK
	    {
		__m128 s0, s1, d0, d1;
		while ( 0 < i ) {
		    i -= step_sse32;
		    s1 = _mm_loadu_ps( src + i + 4 );
		    s0 = _mm_loadu_ps( src + i );
		    d1 = _mm_loadu_ps( dst + i + 4 );
		    d0 = _mm_loadu_ps( dst + i );
		    d1 = _mm_mul_ps(d1, s1);
		    d0 = _mm_mul_ps(d0, s0);
		    _mm_storeu_ps(dst + i + 4, d1);
		    _mm_storeu_ps(dst + i, d0);
		}
	    }
#endif
	}
	else {
	    while ( i_break < i ) {
		i--;
		dst[i] /= src[i];
	    }
#ifdef _SSE2_IS_OK
	    {
		__m128 s0, s1, d0, d1;
		while ( 0 < i ) {
		    i -= step_sse32;
		    s1 = _mm_loadu_ps( src + i + 4 );
		    s0 = _mm_loadu_ps( src + i );
		    d1 = _mm_loadu_ps( dst + i + 4 );
		    d0 = _mm_loadu_ps( dst + i );
		    d1 = _mm_div_ps(d1, s1);
		    d0 = _mm_div_ps(d0, s0);
		    _mm_storeu_ps(dst + i + 4, d1);
		    _mm_storeu_ps(dst + i, d0);
		}
	    }
#endif
	}
    }
    else {
	const size_t s_step = ((0 < f_b) ? 1 : 0);
	i = 0;
	if ( ope == S_OPE_PLUS ) {
#ifdef _SSE2_IS_OK
	    if ( _SSE2_CALC_MIN_NBYTES <= sizeof(float) * n ) {
		if ( s_step == 1 ) {
		    __m128 s0, s1, d0, d1;
		    for ( ; i < nel_sse32 ; i+=step_sse32 ) {
			s0 = _mm_loadu_ps( src + i );
			s1 = _mm_loadu_ps( src + i + 4 );
			d0 = _mm_loadu_ps( dst + i );
			d1 = _mm_loadu_ps( dst + i + 4 );
			d0 = _mm_add_ps(d0, s0);
			d1 = _mm_add_ps(d1, s1);
			_mm_storeu_ps(dst + i, d0);
			_mm_storeu_ps(dst + i + 4, d1);
		    }
		    src += step_sse32 * n_sse32;
		}
		else if ( s_step == 0 ) {
		    __m128 s0, d0, d1;
		    s0 = _mm_set1_ps(*src);
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
	    for ( ; i < n ; i++ ) {
		dst[i] += *src;
		src += s_step;
	    }
	}
	else if ( ope == S_OPE_MINUS ) {
#ifdef _SSE2_IS_OK
	    if ( _SSE2_CALC_MIN_NBYTES <= sizeof(float) * n ) {
		if ( s_step == 1 ) {
		    __m128 s0, s1, d0, d1;
		    for ( ; i < nel_sse32 ; i+=step_sse32 ) {
			s0 = _mm_loadu_ps( src + i );
			s1 = _mm_loadu_ps( src + i + 4 );
			d0 = _mm_loadu_ps( dst + i );
			d1 = _mm_loadu_ps( dst + i + 4 );
			d0 = _mm_sub_ps(d0, s0);
			d1 = _mm_sub_ps(d1, s1);
			_mm_storeu_ps(dst + i, d0);
			_mm_storeu_ps(dst + i + 4, d1);
		    }
		    src += step_sse32 * n_sse32;
		}
		else if ( s_step == 0 ) {
		    __m128 s0, d0, d1;
		    s0 = _mm_set1_ps(*src);
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
	    for ( ; i < n ; i++ ) {
		dst[i] -= *src;
		src += s_step;
	    }
	}
	else if ( ope == S_OPE_STAR ) {
#ifdef _SSE2_IS_OK
	    if ( _SSE2_CALC_MIN_NBYTES <= sizeof(float) * n ) {
		if ( s_step == 1 ) {
		    __m128 s0, s1, d0, d1;
		    for ( ; i < nel_sse32 ; i+=step_sse32 ) {
			s0 = _mm_loadu_ps( src + i );
			s1 = _mm_loadu_ps( src + i + 4 );
			d0 = _mm_loadu_ps( dst + i );
			d1 = _mm_loadu_ps( dst + i + 4 );
			d0 = _mm_mul_ps(d0, s0);
			d1 = _mm_mul_ps(d1, s1);
			_mm_storeu_ps(dst + i, d0);
			_mm_storeu_ps(dst + i + 4, d1);
		    }
		    src += step_sse32 * n_sse32;
		}
		else if ( s_step == 0 ) {
		    __m128 s0, d0, d1;
		    s0 = _mm_set1_ps(*src);
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
	    for ( ; i < n ; i++ ) {
		dst[i] *= *src;
		src += s_step;
	    }
	}
	else {
#ifdef _SSE2_IS_OK
	    if ( _SSE2_CALC_MIN_NBYTES <= sizeof(float) * n ) {
		if ( s_step == 1 ) {
		    __m128 s0, s1, d0, d1;
		    for ( ; i < nel_sse32 ; i+=step_sse32 ) {
			s0 = _mm_loadu_ps( src + i );
			s1 = _mm_loadu_ps( src + i + 4 );
			d0 = _mm_loadu_ps( dst + i );
			d1 = _mm_loadu_ps( dst + i + 4 );
			d0 = _mm_div_ps(d0, s0);
			d1 = _mm_div_ps(d1, s1);
			_mm_storeu_ps(dst + i, d0);
			_mm_storeu_ps(dst + i + 4, d1);
		    }
		    src += step_sse32 * n_sse32;
		}
		else if ( s_step == 0 ) {
		    __m128 s0, d0, d1;
		    s0 = _mm_set1_ps(*src);
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
	    for ( ; i < n ; i++ ) {
		dst[i] /= *src;
		src += s_step;
	    }
	}
    }
}
