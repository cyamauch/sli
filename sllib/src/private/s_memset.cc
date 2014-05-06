#include <string.h>

#include "_test_sse2.h"

#ifdef _SSE2_IS_OK
#include <emmintrin.h>
#include "_tune_sse2.h"
#endif

/**
 * @brief  SIMD命令(SSE2)を使った高速版 memset() 関数
 *
 * @param  s 書き込み先バッファのアドレス
 * @param  c 書き込まれる値
 * @param  n 書き込むバイト数
 * @param  total_n s_memset()が連続して呼び出される場合のトータルのバイト数
 */
inline static void *s_memset( void *s, int c, size_t n, size_t total_n )
{
    unsigned char *d_ptr = (unsigned char *)s;

#ifdef _SSE2_IS_OK
    if ( _SSE2_MIN_NBYTES <= n ) {	/* use SSE2 if n is large enough */
	const unsigned char src[16] __attribute__((aligned(16))) = 
					  {c,c,c,c, c,c,c,c, c,c,c,c, c,c,c,c};
	size_t mm = ((size_t)d_ptr & 0x0f);
	bool use_mm_stream;
	if ( _SSE2_CPU_CACHE_SIZE <= n || _SSE2_CPU_CACHE_SIZE <= total_n ) {
	    use_mm_stream = true;
	}
	else use_mm_stream = false;
	/* set start */
	if ( 0 < mm ) {			/* align with 16-byte */
	    mm = 16 - mm;
	    n -= mm;
	    memset(d_ptr, c, mm);
	    d_ptr += mm;
	}
	__m128i r0 = _mm_load_si128((__m128i *)src);
	if ( use_mm_stream == true ) {
	    while ( 16 <= n ) {
		n -= 16;
		/* without polluting the cache */
		_mm_stream_si128((__m128i *)d_ptr, r0);
		d_ptr += 16;
	    }
	}
	else {
	    while ( 16 <= n ) {
		n -= 16;
		_mm_store_si128((__m128i *)d_ptr, r0);
		d_ptr += 16;
	    }
	}
    }
#endif	/* _SSE2_IS_OK */

    memset(d_ptr, c, n);

    return s;
}
