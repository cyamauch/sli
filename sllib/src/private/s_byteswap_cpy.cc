#include <string.h>
#include <unistd.h>

#define USE_OPTIMIZED_BYTE_SWAP 1

#include "_test_sse2.h"

#ifdef _SSE2_IS_OK
#include <emmintrin.h>
#include "_tune_sse2.h"
#endif

/**
 * @brief  SIMD̿��(SSE2)��Ȥä�2�Х����ͤΥХ��ȥ������ι�®�Ѵ��ȥ��ԡ�
 */
inline static void s_byteswap2_cpy( const void *buf_in, size_t len_elements,
				    void *buf_out )
{
    const unsigned char *s_ptr = (const unsigned char *)buf_in;
    unsigned char *d_ptr = (unsigned char *)buf_out;
    bool dest_is_aligned = ( ((size_t)buf_out & 0x01) == 0 );

#ifdef USE_OPTIMIZED_BYTE_SWAP
#ifdef _SSE2_IS_OK
    if ( _SSE2_MIN_NBYTES <= 2 * len_elements ) {
	if ( dest_is_aligned == true ) {		/* align if possible */
	    while ( ((size_t)d_ptr & 0x0f) != 0 ) {
		len_elements --;
		d_ptr[0] = s_ptr[1];  d_ptr[1] = s_ptr[0];
		d_ptr += 2;  s_ptr += 2;
	    }
	}
	while ( 8 <= len_elements ) {
	    len_elements -= 8;
	    __m128i r0 = _mm_loadu_si128((__m128i *)s_ptr);
	    __m128i r1 = r0;
	    /* 1-byte shift */
	    r0 = _mm_srli_epi16(r0, 8);
	    r1 = _mm_slli_epi16(r1, 8);
	    /* or */
	    r0 = _mm_or_si128(r0, r1);
	    _mm_storeu_si128((__m128i *)d_ptr, r0);
	    d_ptr += 2 * 8;  s_ptr += 2 * 8;
	}
    }
#endif	/* _SSE2_IS_OK */

    /* ���饤���Ȥ�OK�ʾ��� swab() ��Ȥ�                          */
    /* swab() �� htons()���ϥХ��ȶ�����·�äƤ��ʤ����٤��ʤ�������� */
    if ( dest_is_aligned == true && ((size_t)(s_ptr) & 0x01) == 0 ) {
	/* Linux �� swab() �Ϥ��ä���®�� */
	swab(s_ptr, d_ptr, 2 * len_elements);
	len_elements = 0;
	/*
	 * �¸��Ǥ� swab() ��꼡�Υ����ɤ�®���ä������ѥե����ޥ󥹤�
	 * htons() �μ����˰�¸����ΤǺ��Ѥϸ����ꡥ
	 *  
	 *  uint16_t *d_ptr_s = (uint16_t *)d_ptr;
	 *  const uint16_t *s_ptr_s = (const uint16_t *)s_ptr;
	 *  for ( i=0 ; i < len_elements ; i++ ) {
	 *      *d_ptr_s = htons(*s_ptr_s);
	 *      d_ptr_s ++;  s_ptr_s ++;
	 *  }
	 */
    }
    else {
	unsigned char tmp;
	/* swab() ���1�䤯�餤�٤������饤���ȴط��ʤ���ư�������� */
	/* (�����󡤥��饤���Ȥ����פ��Ƥʤ����Ϥ�ä��٤��ʤ�) */
	while ( 8 <= len_elements ) {
	    len_elements -= 8;
	    /* �֥�å�Ĺ�� 8,16,32 �Ȼ������16���Ǥ��ɤ��褦�� */
	    memcpy(d_ptr, s_ptr, 2 * 8);  /* memcpy() �ΰ��Ϥ�ڤ�� */
	    tmp = d_ptr[0];  d_ptr[0] = d_ptr[1];  d_ptr[1] = tmp;
	    tmp = d_ptr[2];  d_ptr[2] = d_ptr[3];  d_ptr[3] = tmp;
	    tmp = d_ptr[4];  d_ptr[4] = d_ptr[5];  d_ptr[5] = tmp;
	    tmp = d_ptr[6];  d_ptr[6] = d_ptr[7];  d_ptr[7] = tmp;
	    tmp = d_ptr[8];  d_ptr[8] = d_ptr[9];  d_ptr[9] = tmp;
	    tmp = d_ptr[10];  d_ptr[10] = d_ptr[11];  d_ptr[11] = tmp;
	    tmp = d_ptr[12];  d_ptr[12] = d_ptr[13];  d_ptr[13] = tmp;
	    tmp = d_ptr[14];  d_ptr[14] = d_ptr[15];  d_ptr[15] = tmp;
	    d_ptr += 2 * 8;  s_ptr += 2 * 8;
	}
    }
#endif	/* USE_OPTIMIZED_BYTE_SWAP */

    while ( 0 < len_elements ) {
	len_elements --;
	d_ptr[0] = s_ptr[1];  d_ptr[1] = s_ptr[0];
	d_ptr += 2;  s_ptr += 2;
    }

    return;
}

/**
 * @brief  SIMD̿��(SSE2)��Ȥä�4�Х����ͤΥХ��ȥ������ι�®�Ѵ��ȥ��ԡ�
 */
inline static void s_byteswap4_cpy( const void *buf_in, size_t len_elements,
				    void *buf_out )
{
     const unsigned char *s_ptr = (const unsigned char *)buf_in;
     unsigned char *d_ptr = (unsigned char *)buf_out;

#ifdef USE_OPTIMIZED_BYTE_SWAP
#ifdef _SSE2_IS_OK
    if ( _SSE2_MIN_NBYTES <= 4 * len_elements ) {
	bool is_aligned = ( ((size_t)d_ptr & 0x03) == 0 );
	if ( is_aligned == true ) {			/* align if possible */
	    while ( ((size_t)d_ptr & 0x0f) != 0 ) {
		len_elements --;
		d_ptr[0] = s_ptr[3];
		d_ptr[1] = s_ptr[2];
		d_ptr[2] = s_ptr[1];
		d_ptr[3] = s_ptr[0];
		d_ptr += 4;  s_ptr += 4;
	    }
	}
	while ( 4 <= len_elements ) {
	    len_elements -= 4;
	    __m128i r0 = _mm_loadu_si128((__m128i *)s_ptr);
	    /* 0123 => 2301 */
	    r0 = _mm_shufflelo_epi16(r0, _MM_SHUFFLE(2,3,0,1));
	    r0 = _mm_shufflehi_epi16(r0, _MM_SHUFFLE(2,3,0,1));
	    __m128i r1 = r0;
	    /* 1-byte shift */
	    r0 = _mm_srli_epi16(r0, 8);
	    r1 = _mm_slli_epi16(r1, 8);
	    /* or */
	    r0 = _mm_or_si128(r0, r1);
	    _mm_storeu_si128((__m128i *)d_ptr, r0);
	    d_ptr += 4 * 4;  s_ptr += 4 * 4;
	}
    }
#endif	/* _SSE2_IS_OK */

     /*
      * ���Υ֥�å��Υ����ɤϡ�htonl() ��Ȥä����Υ�����
      * {
      *     uint32_t *d_ptr_l = (uint32_t *)d_ptr;
      *     const uint32_t *s_ptr_l = (const uint32_t *)s_ptr;
      *     for ( i=0 ; i < len_elements ; i++ ) {
      *         *d_ptr_l = htonl(*s_ptr_l);
      *         d_ptr_l ++;  s_ptr_l ++;
      *     }
      * }
      * ��Ʊ����ǽ���Ф롥
      */

    if ( 4 <= len_elements ) {
	unsigned char tmp;
	len_elements -= 4;
	memcpy(d_ptr, s_ptr, 4 * 4);
	tmp = d_ptr[0];  d_ptr[0] = d_ptr[3];  d_ptr[3] = tmp;
	tmp = d_ptr[1];  d_ptr[1] = d_ptr[2];  d_ptr[2] = tmp;
	tmp = d_ptr[4];  d_ptr[4] = d_ptr[7];  d_ptr[7] = tmp;
	tmp = d_ptr[5];  d_ptr[5] = d_ptr[6];  d_ptr[6] = tmp;
	tmp = d_ptr[8];  d_ptr[8] = d_ptr[11];  d_ptr[11] = tmp;
	tmp = d_ptr[9];  d_ptr[9] = d_ptr[10];  d_ptr[10] = tmp;
	tmp = d_ptr[12];  d_ptr[12] = d_ptr[15];  d_ptr[15] = tmp;
	tmp = d_ptr[13];  d_ptr[13] = d_ptr[14];  d_ptr[14] = tmp;
	d_ptr += 4 * 4;  s_ptr += 4 * 4;
    }
#endif	/* USE_OPTIMIZED_BYTE_SWAP */

    while ( 0 < len_elements ) {
	len_elements --;
	d_ptr[0] = s_ptr[3];
	d_ptr[1] = s_ptr[2];
	d_ptr[2] = s_ptr[1];
	d_ptr[3] = s_ptr[0];
	d_ptr += 4;  s_ptr += 4;
    }

    return;
}

/**
 * @brief  SIMD̿��(SSE2)��Ȥä�8�Х����ͤΥХ��ȥ������ι�®�Ѵ��ȥ��ԡ�
 */
inline static void s_byteswap8_cpy( const void *buf_in, size_t len_elements,
				    void *buf_out )
{
    const unsigned char *s_ptr = (const unsigned char *)buf_in;
    unsigned char *d_ptr = (unsigned char *)buf_out;

#ifdef USE_OPTIMIZED_BYTE_SWAP
#ifdef _SSE2_IS_OK
    if ( _SSE2_MIN_NBYTES <= 8 * len_elements ) {
	bool is_aligned = ( ((size_t)d_ptr & 0x07) == 0 );
	if ( is_aligned == true ) {			/* align if possible */
	    while ( ((size_t)d_ptr & 0x0f) != 0 ) {
		len_elements --;
		d_ptr[0] = s_ptr[7];
		d_ptr[1] = s_ptr[6];
		d_ptr[2] = s_ptr[5];
		d_ptr[3] = s_ptr[4];
		d_ptr[4] = s_ptr[3];
		d_ptr[5] = s_ptr[2];
		d_ptr[6] = s_ptr[1];
		d_ptr[7] = s_ptr[0];
		d_ptr += 8;  s_ptr += 8;
	    }
	}
	while ( 2 <= len_elements ) {
	    len_elements -= 2;
	    __m128i r0 = _mm_loadu_si128((__m128i *)s_ptr);
	    /* 01234567 => 67452301 */
	    r0 = _mm_shufflelo_epi16(r0, _MM_SHUFFLE(0,1,2,3));
	    r0 = _mm_shufflehi_epi16(r0, _MM_SHUFFLE(0,1,2,3));
	    __m128i r1 = r0;
	    /* 1-byte shift */
	    r0 = _mm_srli_epi16(r0, 8);
	    r1 = _mm_slli_epi16(r1, 8);
	    /* or */
	    r0 = _mm_or_si128(r0, r1);
	    _mm_storeu_si128((__m128i *)d_ptr, r0);
	    d_ptr += 8 * 2;  s_ptr += 8 * 2;
	}
    }
#endif	/* _SSE2_IS_OK */
#endif	/* USE_OPTIMIZED_BYTE_SWAP */

    /* 8�Х��Ȥξ��� memcpy() ��Ȥ���ꤳ�������㴳®�� */
    /* memcpy() ��Ȥ����ɤ����ζ����� 8�Х��Ȥˤ���餷�� */
    /* �ʤ���*d_ptr++ �ϻȤ��Ȥ���٤��ʤ�                 */
    while ( 0 < len_elements ) {
	len_elements --;
	d_ptr[0] = s_ptr[7];
	d_ptr[1] = s_ptr[6];
	d_ptr[2] = s_ptr[5];
	d_ptr[3] = s_ptr[4];
	d_ptr[4] = s_ptr[3];
	d_ptr[5] = s_ptr[2];
	d_ptr[6] = s_ptr[1];
	d_ptr[7] = s_ptr[0];
	d_ptr += 8;  s_ptr += 8;
    }

    return;
}

/**
 * @brief  SIMD̿��(SSE2)��Ȥä��Х��ȥ������ι�®�Ѵ��ȥ��ԡ�
 *
 * Ǥ�դΥХ���Ĺ������(���饤���ȵ���ʤ�)�ΥХ��ȥ��������Ѵ����ʤ����̤�
 * �Хåե�������򥳥ԡ����ޤ�(SSE2�б���)
 *
 * @param      buf_in �ɤ߼���������Ƭ���ɥ쥹
 * @param      word_bytes 1���ǤΥХ���Ĺ
 * @param      len_elements ��������ǤθĿ�
 * @param      buf_out ������ΥХåե�����Ƭ���ɥ쥹
 * @note       �Х���Ĺ��2,4,8�Х��Ȥξ������̤ʥ����ɤ�ư����ä˹�®
 *             �Ǥ�������¾�ΥХ���Ĺ�Ǥ����ѥ����ɤ�ư��ޤ�����8�Х��Ȥ�
 *             �ۤ�����ǤϤ�������®��ư���褦��Ĵ�����Ƥ���ޤ���<br>
 *             �ƥ��ȴĶ��� GCC 3.3.2 + PentiumM�Τ��Ť���åץȥåפ�
 *             64-bit Linux��<br>
 *             ɸ��饤�֥��� htonl(), htons(), ntohl(), ntohs(), swab()
 *             �⻲�ȤΤ��ȡ�
 */
inline static void s_byteswap_cpy( const void *buf_in, 
			size_t word_bytes, size_t len_elements, void *buf_out )
{
    /*
     * ���å���ñ��ʥ����ɤǤ�ɽ��:
     *	 const unsigned char *s_ptr = (const unsigned char *)buf_in;
     *	 unsigned char *d_ptr = (unsigned char *)buf_out;
     *   for ( i=0 ; i < len_elements ; i++ ) {
     *       for ( j=0 ; j < word_bytes ; j++ ) {
     *	         d_ptr[j] = s_ptr[word_bytes-1-j];
     *       }
     *       d_ptr += word_bytes;
     *       s_ptr += word_bytes;
     *   }
     */

    /* fast vesion for 2,4 and 8 bytes */
    if ( word_bytes == 2 ) s_byteswap2_cpy( buf_in, len_elements, buf_out);
    else if (word_bytes == 4) s_byteswap4_cpy( buf_in, len_elements, buf_out);
    else if (word_bytes == 8) s_byteswap8_cpy( buf_in, len_elements, buf_out);
#ifdef USE_OPTIMIZED_BYTE_SWAP
#ifdef _SSE2_IS_OK
    else if ( 16 <= word_bytes && _SSE2_MIN_NBYTES <= word_bytes * len_elements ) {
	const unsigned char *s_ptr = (const unsigned char *)buf_in;
	unsigned char *d_ptr = (unsigned char *)buf_out;
	const unsigned char *s_ptr_x;
	size_t i, j;
	for ( i=0 ; i < len_elements ; i++ ) {
	    s_ptr += word_bytes;
	    s_ptr_x = s_ptr;
	    j = word_bytes;
	    while ( 16 <= j ) {
		s_ptr_x -= 16;
		size_t j1 = j - 16;
		if ( 0 < j1 && j1 < 16 ) {			/* overwrap */
		    /*              0123456789abcdefgh  */
		    /*  r0:           ^^^^^^^^^^^^^^^^  */
		    /*  r1:         ^^^^^^^^^^^^^^^^    */
		    /*  swapped r0:   hgfedcba98765432  */
		    /*  swapped r1: fedcba9876543210    */
		    /*  result:     hgfedcba9876543210  */
		    __m128i r0 = _mm_loadu_si128((__m128i *)s_ptr_x);
		    __m128i r1 = _mm_loadu_si128((__m128i *)(s_ptr_x - j1));
		    /* 01234567 89abcdef => 89abcdef 01234567 */
		    r0 = _mm_shuffle_epi32(r0, _MM_SHUFFLE(1,0,3,2));
		    r1 = _mm_shuffle_epi32(r1, _MM_SHUFFLE(1,0,3,2));
		    /* 01234567 => 67452301 */
		    r0 = _mm_shufflelo_epi16(r0, _MM_SHUFFLE(0,1,2,3));
		    r1 = _mm_shufflelo_epi16(r1, _MM_SHUFFLE(0,1,2,3));
		    r0 = _mm_shufflehi_epi16(r0, _MM_SHUFFLE(0,1,2,3));
		    r1 = _mm_shufflehi_epi16(r1, _MM_SHUFFLE(0,1,2,3));
		    __m128i r0a = r0;
		    __m128i r1a = r1;
		    /* 1-byte shift */
		    r0 = _mm_srli_epi16(r0, 8);
		    r1 = _mm_srli_epi16(r1, 8);
		    r0a = _mm_slli_epi16(r0a, 8);
		    r1a = _mm_slli_epi16(r1a, 8);
		    /* or */
		    r0 = _mm_or_si128(r0, r0a);
		    r1 = _mm_or_si128(r1, r1a);
		    _mm_storeu_si128((__m128i *)d_ptr, r0);
		    _mm_storeu_si128((__m128i *)(d_ptr + j1), r1);
		    d_ptr += j;
		    j = 0;					/* finished */
		}
		else {
		    __m128i r0 = _mm_loadu_si128((__m128i *)s_ptr_x);
		    /* 01234567 89abcdef => 89abcdef 01234567 */
		    r0 = _mm_shuffle_epi32(r0, _MM_SHUFFLE(1,0,3,2));
		    /* 01234567 => 67452301 */
		    r0 = _mm_shufflelo_epi16(r0, _MM_SHUFFLE(0,1,2,3));
		    r0 = _mm_shufflehi_epi16(r0, _MM_SHUFFLE(0,1,2,3));
		    __m128i r0a = r0;
		    /* 1-byte shift */
		    r0 = _mm_srli_epi16(r0, 8);
		    r0a = _mm_slli_epi16(r0a, 8);
		    /* or */
		    r0 = _mm_or_si128(r0, r0a);
		    _mm_storeu_si128((__m128i *)d_ptr, r0);
		    d_ptr += 16;
		    j -= 16;
		}
	    }
	}
    }
#endif	/* _SSE2_IS_OK */
#endif	/* USE_OPTIMIZED_BYTE_SWAP */
    /* fallback                                                    */
    /* word_bytes�������������٤�����8�Х��Ȥ�ۤ������ǽ���Ф� */
    else {
	const unsigned char *s_ptr = (const unsigned char *)buf_in;
	unsigned char *d_ptr = (unsigned char *)buf_out;
	const unsigned char *s_ptr_x;
	size_t i, j;
	for ( i=0 ; i < len_elements ; i++ ) {
	    s_ptr += word_bytes;
	    s_ptr_x = s_ptr;
	    j = word_bytes;
#ifdef USE_OPTIMIZED_BYTE_SWAP
	    while ( 8 <= j ) {
		s_ptr_x -= 8;
		d_ptr[0] = s_ptr_x[7];
		d_ptr[1] = s_ptr_x[6];
		d_ptr[2] = s_ptr_x[5];
		d_ptr[3] = s_ptr_x[4];
		d_ptr[4] = s_ptr_x[3];
		d_ptr[5] = s_ptr_x[2];
		d_ptr[6] = s_ptr_x[1];
		d_ptr[7] = s_ptr_x[0];
		d_ptr += 8;
		j -= 8;
	    }
#endif	/* USE_OPTIMIZED_BYTE_SWAP */
	    s_ptr_x -= j;
	    while ( 0 < j ) {
		j--;
		*d_ptr = s_ptr_x[j];
		d_ptr++;
	    }
	}
    }

    return;
}

#ifdef USE_OPTIMIZED_BYTE_SWAP
#undef USE_OPTIMIZED_BYTE_SWAP
#endif
