#include "_test_sse2.h"

#ifdef _SSE2_IS_OK
#include <emmintrin.h>
#include "_tune_sse2.h"
#endif

inline static void s_abs_vector_double( const double *src, size_t n, double *dst )
{
    const uint64_t *_src = (const uint64_t *)src;
    uint64_t *_dst = (uint64_t *)dst;
    union _sign_mask_double {
	uint64_t i;
	double r;
    } sign_mask;
    size_t i=0;
    sign_mask.i = 0x7fffffffffffffffULL;
#ifdef _SSE2_IS_OK
    if ( _SSE2_CALC_MIN_NBYTES <= sizeof(double) * n ) {
	const size_t step_sse32 = 32 / sizeof(double);	/* 4 */
	const size_t n_sse32 = n / step_sse32;
	const size_t nel_sse32 = step_sse32 * n_sse32;
	const __m128d smask = _mm_set1_pd(sign_mask.r);
	__m128d r0, r1;
	for ( ; i < nel_sse32 ; i += step_sse32 ) {
	    r0 = _mm_loadu_pd( src + i );
	    r1 = _mm_loadu_pd( src + i + 2 );
	    r0 = _mm_and_pd(r0, smask);
	    r1 = _mm_and_pd(r1, smask);
	    _mm_storeu_pd(dst + i, r0);
	    _mm_storeu_pd(dst + i + 2, r1);
	}
    }
#endif
    for ( ; i < n ; i++ ) {
	_dst[i] = _src[i] & sign_mask.i;
    }
}
