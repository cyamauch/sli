#include <string.h>

#include "_test_sse2.h"

#ifdef _SSE2_IS_OK
#include <emmintrin.h>
#include "_tune_sse2.h"
#endif

/**
 * @brief  SIMD̿��(SSE2)��Ȥä���®�� memcpy() �ؿ�
 *
 *   ���� total_n ��Ϣ³���� s_memcpy() ��Ϣ³���ƻȤ������ȡ������ž���Х���
 *   Ĺ����ꤹ�롥SSE2 ��ͭ���ʾ��Τ�����ư����ڤ��ؤ��˻��Ѥ���롥
 *   total_n �� _SSE2_CPU_CACHE_SIZE �ʾ�ξ�硤�񤭹���¦�Ǥ� CPU ����å���
 *   ����Ѥ��ʤ���®ž���⡼�ɤ��ڤ��ؤ�롥��󤭤�θƤӽФ��ξ��� total_n
 *   �� 0 ����ꤷ�Ƥ������ɤ���
 *
 * @param  dest �񤭹�����Хåե��Υ��ɥ쥹
 * @param  src �ɤ߹��߸��Хåե��Υ��ɥ쥹
 * @param  n ���ԡ���Ԥʤ��Х��ȿ�
 * @param  total_n s_memcpy()��Ϣ³���ƸƤӽФ������Υȡ�����ΥХ��ȿ�
 * @note  4�Ĥ�ΰ�����ƤӽФ�����ˤ������ͤ�������n ���Ѳ�����褦�ʾ��
 *        �˺���Τ� total_n �Ȥ�����
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
