#include <string.h>

#include "_test_sse2.h"

#ifdef _SSE2_IS_OK
#include <emmintrin.h>
#include "_tune_sse2.h"
#endif

/**
 * @brief  SIMD命令(SSE2)を使った高速版 memcpy() 関数
 *
 *   引数 total_n は連続して s_memcpy() を連続して使う時，トータルの転送バイト
 *   長を指定する．SSE2 が有効な場合のみ内部動作の切り替えに使用される．
 *   total_n が _SSE2_CPU_CACHE_SIZE 以上の場合，書き込み側では CPU キャッシュ
 *   を使用しない高速転送モードに切り替わる．一回きりの呼び出しの場合は total_n
 *   に 0 を指定しておけば良い．
 *
 * @param  dest 書き込み先バッファのアドレス
 * @param  src 読み込み元バッファのアドレス
 * @param  n コピーを行なうバイト数
 * @param  total_n s_memcpy()が連続して呼び出される場合のトータルのバイト数
 * @note  4つめの引数を呼び出し回数にする事も考えたが，n が変化するような場合
 *        に困るので total_n とした．
 */
inline static void *s_memcpy( void *dest, const void *src, size_t n,
			      size_t total_n )
{
    unsigned char *d_ptr = (unsigned char *)dest;
    const unsigned char *s_ptr = (const unsigned char *)src;

#ifdef _SSE2_IS_OK
    /* use SSE2 if n is large enough */
    if ( _SSE2_MIN_NBYTES <= n ) {
	const size_t n_bytes = n;
	const bool total_large = (_SSE2_CPU_CACHE_SIZE <= total_n);
	size_t mm;
	mm = ((size_t)d_ptr & 0x0f);
	if ( 0 < mm ) {		/* align with 16-byte for dest */
	    mm = (16 - mm);
	    n -= mm;
	    memcpy(d_ptr, s_ptr, mm);
	    d_ptr += mm;
	    s_ptr += mm;
	}
	if ( _SSE2_CPU_CACHE_SIZE <= n_bytes || total_large == true ) {
	    while ( 32 <= n ) {
		__m128i r0, r1;
		n -= 32;
		/* */
		r0 = _mm_loadu_si128((__m128i *)s_ptr);
		s_ptr += 16;
		r1 = _mm_loadu_si128((__m128i *)s_ptr);
		s_ptr += 16;
		/* without polluting the cache */
		_mm_stream_si128((__m128i *)d_ptr, r0);
		d_ptr += 16;
		_mm_stream_si128((__m128i *)d_ptr, r1);
		d_ptr += 16;
	    }
	}
	else {
	    while ( 32 <= n ) {
		__m128i r0, r1;
		n -= 32;
		/* */
		r0 = _mm_loadu_si128((__m128i *)s_ptr);
		s_ptr += 16;
		r1 = _mm_loadu_si128((__m128i *)s_ptr);
		s_ptr += 16;
		/* */
		_mm_store_si128((__m128i *)d_ptr, r0);
		d_ptr += 16;
		_mm_store_si128((__m128i *)d_ptr, r1);
		d_ptr += 16;
	    }
	}
    }
#endif	/* _SSE2_IS_OK */

    memcpy(d_ptr, s_ptr, n);

    return dest;
}
