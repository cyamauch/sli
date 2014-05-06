#include <string.h>

#include "_test_sse2.h"

#ifdef _SSE2_IS_OK
#include <emmintrin.h>
#include "_tune_sse2.h"
#endif

/**  
 * @brief  SIMD̿��(SSE2)��Ȥä���®���ΰ��ɤ��٤� (n�Х��ȤΥѥ�����)
 *
 *  Ǥ�դΥХ���Ĺ�ΥХ��ȥѥ�����(���饤���ȵ���ʤ�)�η����֤��ǥХåե�
 *  �ΰ���ɤ��٤��ޤ�(SSE2�б���)��
 *
 * @param      dest ������ΥХåե�����Ƭ���ɥ쥹
 * @param      src_pattern �Х��ȥѥ�����
 * @param      word_bytes �Х��ȥѥ�����ΥХ���Ĺ
 * @param      len_elements �ɤ��٤��٤��Х��ȥѥ�����θĿ�
 * @param      total_len_elements �����֤��ƤӽФ����ΥХ��ȥѥ���������
 * @note       �ѥ����󤪤�ӽ�����Хåե����礭���˱����Ƥ��������ɤ���ǽ��
 *             �Ф�褦��Ĵ�����Ƥ���ޤ���<br>
 *             �ƥ��ȴĶ��� GCC 3.3.2 + PentiumM�Τ��Ť���åץȥåפ�
 *             GCC-4 + 64-bit Linux��<br>
 *             ɸ��饤�֥��� wmemset() �⻲�ȡ�<br>
 *             ���: ������Ȼ����̡�32-bit�Ķ��Ǥ� n = 8�Х��ȸ����
 *                   memcpy()�Υ롼�פ���®�Ȥ��������ˤʤä���
 */
inline static void s_memfill( void *dest, 
	      const void *src_pattern, size_t word_bytes, size_t len_elements,
	      size_t total_len_elements )
{
#ifdef _SSE2_IS_OK
    const size_t op_bytes = 16;			/* for SSE2 */
#else
    const size_t op_bytes = 2 * sizeof(size_t);	/* good bytes for memcpy() */
#endif
    const unsigned char *src_pat = (const unsigned char *)src_pattern;
    unsigned char *d_ptr = (unsigned char *)dest;
    size_t i, j;

    /*
     * ���å���ñ��ʥ����ɤ�ɽ���������:
     *
     *	    for ( i=0 ; i < len ; i++ ) {
     *		for ( j=0 ; j < word_bytes ; j++ ) {
     *		    d_ptr[j] = src_pat[j];
     *		}
     *		d_ptr += word_bytes;
     *	    }
     */

    /* BLOCK-A �� BLOCK-B �Ȥ�ʬ����� */
    const size_t len_blk = 4 * op_bytes;	/* �� ���� 4 �ϼ�¬��Ĵ�� */

#ifdef _SSE2_IS_OK
    size_t blen_all = word_bytes * len_elements;
    /*                                                        */
    /* BLOCK-S : special code for word_bytes = 2, 4, 8 and 16 */
    /*                                                        */
    if ( _SSE2_MIN_NBYTES <= blen_all && 
	 word_bytes <= 16 && (16 % word_bytes) == 0 ) {
        const size_t nn = 16 / word_bytes;
	const size_t mm = ((size_t)d_ptr & (word_bytes - 1));
	unsigned char src_al[16] __attribute__((aligned(16)));
	size_t ix;
	bool use_mm_stream;
	if ( _SSE2_CPU_CACHE_SIZE <= blen_all || 
	     _SSE2_CPU_CACHE_SIZE <= word_bytes * total_len_elements ) {
	    use_mm_stream = true;
	}
	else use_mm_stream = false;
	/* create shifted pattern */
	ix = mm;
	for ( i=0 ; i < nn ; i++ ) {
	    for ( j=0 ; j < word_bytes ; j++ ) {
		src_al[ix] = src_pat[j];
		ix++;
		if ( 16 <= ix ) ix = 0;
	    }
	}
	__m128i r0 = _mm_load_si128((__m128i *)src_al);
	/* not aligned part */
	j = mm;
	while ( ((size_t)d_ptr & 0x0f) != 0 ) {
	    blen_all --;
	    *d_ptr = src_al[j];
	    d_ptr ++;  j++;
	}
	/* aligned part */
	if ( use_mm_stream == true ) {
	    while ( 16 <= blen_all ) {
		blen_all -= 16;
		/* without polluting the cache */
		_mm_stream_si128((__m128i *)d_ptr, r0);
		d_ptr += 16;
	    }
	}
	else {
	    while ( 16 <= blen_all ) {
		blen_all -= 16;
		_mm_store_si128((__m128i *)d_ptr, r0);
		d_ptr += 16;
	    }
	}
	/* not aligned part */
	j = 0;
	while ( 0 < blen_all ) {
	    blen_all --;
	    *d_ptr = src_al[j];
	    d_ptr ++;  j++;
	}
    }
    else
#endif	/* _SSE2_IS_OK */
    /*                                    */
    /* BLOCK-A : when word_bytes is small */
    /*                                    */
    if ( word_bytes < len_blk ) {
        /* factor_nn ������Ϥ��ʤ��񤷤������ʤ��Ȥ�2�ʾ�����ꤷ�ʤ��ȡ� */
        /* �㤨�� word_bytes = 3 �ξ��˥ѥե����ޥ󥹤��Фʤ��ʤ�        */
        const size_t factor_nn = 2;
	const size_t nn = factor_nn * (len_blk / word_bytes);
	const unsigned char *s_ptr0 = src_pat;		/* ���ԡ������ɥ쥹 */
	if ( nn < len_elements ) {
	    s_ptr0 = d_ptr;	  /* ���ԡ������ɥ쥹���ѹ�(��Ƭ���Ϥ�Ͽ) */
	    /* �ޤ���Ƭ�� nn �Ĥ����� */
	    len_elements -= nn;
	    for ( i=0 ; i < nn ; i++ ) {
		for ( j=0 ; j < word_bytes ; j++ ) d_ptr[j] = src_pat[j];
		d_ptr += word_bytes;
	    }
	    /* ³����ʬ�� nn �Ĥ��Ĺ�®�ʥ֥�å�ž�������� */
	    while ( nn <= len_elements ) {
		const unsigned char *s_ptr = s_ptr0;
		len_elements -= nn;
		j = nn * word_bytes;
#ifdef _SSE2_IS_OK
		while ( op_bytes <= j ) {
		    j -= op_bytes;
		    __m128i r0 = _mm_loadu_si128((__m128i *)s_ptr);
		    _mm_storeu_si128((__m128i *)d_ptr, r0);
		    s_ptr += op_bytes;  d_ptr += op_bytes;
		}
#else
		while ( op_bytes <= j ) {
		    j -= op_bytes;
		    memcpy(d_ptr, s_ptr, op_bytes);
		    s_ptr += op_bytes;  d_ptr += op_bytes;
		}
#endif	/* _SSE2_IS_OK */
		while ( 0 < j ) {
		    j--;
		    *d_ptr = *s_ptr;
		    s_ptr ++;  d_ptr ++;
		}
	    }
	}
	/* �Ĥ����ʬ������ */
	while ( 0 < len_elements ) {
	    len_elements --;
	    for ( j=0 ; j < word_bytes ; j++ ) d_ptr[j] = s_ptr0[j];
	    d_ptr += word_bytes;
	}
    }
    /*                                    */
    /* BLOCK-B : when word_bytes is large */
    /*                                    */
    else {
	for ( i=0 ; i < len_elements ; i++ ) {
	    const unsigned char *s_ptr = src_pat;
	    j = word_bytes;
#ifdef _SSE2_IS_OK
	    while ( op_bytes <= j ) {
	        j -= op_bytes;
		__m128i r0 = _mm_loadu_si128((__m128i *)s_ptr);
		_mm_storeu_si128((__m128i *)d_ptr, r0);
		s_ptr += op_bytes;  d_ptr += op_bytes;
	    }
#else
	    while ( op_bytes <= j ) {
		j -= op_bytes;
		memcpy(d_ptr, s_ptr, op_bytes);
		s_ptr += op_bytes;  d_ptr += op_bytes;
	    }
#endif	/* _SSE2_IS_OK */
	    while ( 0 < j ) {
		j--;
		*d_ptr = *s_ptr;
		s_ptr ++;  d_ptr ++;
	    }
	}
    }

    return;
}
