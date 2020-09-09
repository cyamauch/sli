#include "_test_sse2.h"

#ifdef _SSE2_IS_OK
#include <emmintrin.h>
#include "_tune_sse2.h"
#endif

inline static void s_abs_vector_float( const float *src, size_t n, float *dst )
{
    const uint32_t *_src = (const uint32_t *)src;
    uint32_t *_dst = (uint32_t *)dst;
    union _sign_mask_float {
	uint32_t i;
	float r;
    } sign_mask;
    size_t i=0;
    sign_mask.i = 0x7fffffffUL;
#ifdef _SSE2_IS_OK
    if ( _SSE2_CALC_MIN_NBYTES <= sizeof(float) * n ) {
	const size_t step_sse32 = 32 / sizeof(float);	/* 8 */
	const size_t n_sse32 = n / step_sse32;
	const size_t nel_sse32 = step_sse32 * n_sse32;
	const __m128 smask = _mm_set1_ps(sign_mask.r);
	__m128 r0, r1;
	for ( ; i < nel_sse32 ; i += step_sse32 ) {
	    r0 = _mm_loadu_ps( src + i );
	    r1 = _mm_loadu_ps( src + i + 4 );
	    r0 = _mm_and_ps(r0, smask);
	    r1 = _mm_and_ps(r1, smask);
	    _mm_storeu_ps(dst + i, r0);
	    _mm_storeu_ps(dst + i + 4, r1);
	}
    }
#endif
    for ( ; i < n ; i++ ) {
	_dst[i] = _src[i] & sign_mask.i;
    }
}
