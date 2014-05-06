#include <string.h>

#include "_test_sse2.h"

#ifdef _SSE2_IS_OK
#include <emmintrin.h>
#include "_tune_sse2.h"
#endif

/**
 * @brief  SIMD命令(SSE2)を使った高速版 memmove() 関数
 *
 * @param  dest 書き込み先バッファのアドレス
 * @param  src 読み込み元バッファのアドレス
 * @param  n コピーを行なうバイト数
 */
inline static void *s_memmove( void *dest, const void *src, size_t n )
{
#ifdef _SSE2_IS_OK
    unsigned char *d_ptr = (unsigned char *)dest;
    const unsigned char *s_ptr = (const unsigned char *)src;

    if ( _SSE2_MIN_NBYTES <= n ) {	/* use SSE2 if n is large enough */
	if ( src < dest ) {
	    size_t mm;
	    d_ptr += n;
	    s_ptr += n;
	    mm = ((size_t)d_ptr & 0x0f);
	    if ( 0 < mm ) {		/* align with 16-byte for dest */
		n -= mm;
		d_ptr -= mm;
		s_ptr -= mm;
		memmove(d_ptr, s_ptr, mm);
	    }
	    while ( 16 <= n ) {
		__m128i r0;
		n -= 16;
		d_ptr -= 16;
		s_ptr -= 16;
		r0 = _mm_loadu_si128((__m128i *)s_ptr);
		_mm_store_si128((__m128i *)d_ptr, r0);
	    }
	    memmove(dest, src, n);	/* NOTE: don't use d_ptr, s_ptr here */
	}
	else if ( dest < src ) {
	    size_t mm;
	    mm = ((size_t)d_ptr & 0x0f);
	    if ( 0 < mm ) {		/* align with 16-byte for dest */
		mm = (16 - mm);
		n -= mm;
		memmove(d_ptr, s_ptr, mm);
		d_ptr += mm;
		s_ptr += mm;
	    }
	    while ( 16 <= n ) {
		__m128i r0;
		n -= 16;
		r0 = _mm_loadu_si128((__m128i *)s_ptr);
		_mm_store_si128((__m128i *)d_ptr, r0);
		d_ptr += 16;
		s_ptr += 16;
	    }
	    memmove(d_ptr, s_ptr, n);
	}
	else {
	    /* do nothing */
	}
    }
    else {
	memmove(dest, src, n);
    }
#else

    memmove(dest, src, n);

#endif	/* _SSE2_IS_OK */

    return dest;
}
