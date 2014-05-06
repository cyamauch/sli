#include <string.h>

#include "_test_sse2.h"

#ifdef _SSE2_IS_OK
#include <emmintrin.h>
#include "_tune_sse2.h"
#endif

/**  
 * @brief  SIMD命令(SSE2)を使った2バッファ間の高速な内容入れ替え
 *
 *  2つのバッファの内容(アライメント規定なし)を入れ替えます(SSE2対応版)．
 *
 * @param      buf0 1つめの配列の先頭アドレス
 * @param      buf1 2つめの配列の先頭アドレス
 * @param      n 配列のバイト長
 * @note       SSE2, memcpy() が効くので速く動きます．<br>
 *             テスト環境は GCC 3.3.2 + PentiumMのやや古いラップトップと
 *             64-bit Linux．
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
