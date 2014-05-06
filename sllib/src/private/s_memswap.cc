#include <string.h>

#include "_test_sse2.h"

#ifdef _SSE2_IS_OK
#include <emmintrin.h>
#include "_tune_sse2.h"
#endif

/**  
 * @brief  SIMD̿��(SSE2)��Ȥä�2�Хåե��֤ι�®�����������ؤ�
 *
 *  2�ĤΥХåե�������(���饤���ȵ���ʤ�)�������ؤ��ޤ�(SSE2�б���)��
 *
 * @param      buf0 1�Ĥ���������Ƭ���ɥ쥹
 * @param      buf1 2�Ĥ���������Ƭ���ɥ쥹
 * @param      n ����ΥХ���Ĺ
 * @note       SSE2, memcpy() �������Τ�®��ư���ޤ���<br>
 *             �ƥ��ȴĶ��� GCC 3.3.2 + PentiumM�Τ��Ť���åץȥåפ�
 *             64-bit Linux��
 */
inline static void s_memswap( void *buf0, void *buf1, size_t n )
{
    const size_t op_bytes = sizeof(size_t) * 2;
    unsigned char *s_ptr = (unsigned char *)buf0;
    unsigned char *d_ptr = (unsigned char *)buf1;
    unsigned char tmpbuf[op_bytes];
    unsigned char tmp;
    size_t i;

    i = n;

#ifdef _SSE2_IS_OK
    if ( _SSE2_MIN_NBYTES <= i ) {	/* use SSE2 if i is large enough */
	while ( 16 <= i ) {
	    __m128i rs0, rd0;
	    i -= 16;
	    rs0 = _mm_loadu_si128((__m128i *)s_ptr);
	    rd0 = _mm_loadu_si128((__m128i *)d_ptr);
	    _mm_storeu_si128((__m128i *)d_ptr, rs0);
	    _mm_storeu_si128((__m128i *)s_ptr, rd0);
	    s_ptr += 16;  d_ptr += 16;
	}
    }
#endif	/* _SSE2_IS_OK */

    while ( op_bytes <= i ) {
	i -= op_bytes;
	memcpy(tmpbuf,s_ptr,op_bytes);
	memcpy(s_ptr,d_ptr,op_bytes);
	memcpy(d_ptr,tmpbuf,op_bytes);
	s_ptr += op_bytes;  d_ptr += op_bytes;
    }

    while ( 0 < i ) {
	i--;
	tmp = *s_ptr;  *s_ptr = *d_ptr;  *d_ptr = tmp;
	s_ptr ++;  d_ptr ++;
    }

    return;
}
