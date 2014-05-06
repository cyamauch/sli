/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2012-11-26 15:43:31 cyamauch> */

/*

  Performance test of s_memfill()
  
  g++ -Wall -O2 -msse2 s_memfill.cc -o s_memfill -DUSE_SIMD

 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* */

#include <string.h>

#include "_test_sse2.h"

#ifdef _SSE2_IS_OK
#include <emmintrin.h>
#include "_tune_sse2.h"
#endif

/**  
 * Ǥ�դΥХ���Ĺ�ΥХ��ȥѥ�����(���饤���ȵ���ʤ�)�η����֤��ǥХåե��ΰ�
 * ���ɤ��٤��ޤ�(SSE2�б���)��
 *
 * @param      dest ������ΥХåե�����Ƭ���ɥ쥹
 * @param      src_pattern �Х��ȥѥ�����
 * @param      sz_bytes �Х��ȥѥ�����ΥХ���Ĺ
 * @param      len �ɤ��٤��٤��Х��ȥѥ�����θĿ�
 * @note       �ѥ����󤪤�ӽ�����Хåե����礭���˱����Ƥ��������ɤ���ǽ��
 *             �Ф�褦��Ĵ�����Ƥ���ޤ���<br>
 *             �ƥ��ȴĶ��� GCC 3.3.2 + PentiumM�Τ��Ť���åץȥåפ�
 *             GCC-4 + 64-bit Linux��<br>
 *             ɸ��饤�֥��� wmemset() �⻲�ȡ�<br>
 *             ���: ������Ȼ����̡�32-bit�Ķ��Ǥ� n = 8�Х��ȸ����
 *                   memcpy()�Υ롼�פ���®�Ȥ��������ˤʤä���
 */
inline static void s_memfill( void *dest, 
			 const void *src_pattern, size_t sz_bytes, size_t len )
{
#ifdef _SSE2_IS_OK
    const size_t op_bytes = 16;			/* for SSE2 */
#else
    const size_t op_bytes = 2 * sizeof(size_t);	/* good bytes for memcpy() */
#endif
    const unsigned char *src_pat = (const unsigned char *)src_pattern;
    unsigned char *d_ptr = (unsigned char *)dest;
    size_t len_all = len;
    size_t i, j;

    /*
     * ���å���ñ��ʥ����ɤ�ɽ���������:
     *
     *	    for ( i=0 ; i < len ; i++ ) {
     *		for ( j=0 ; j < sz_bytes ; j++ ) {
     *		    d_ptr[j] = src_pat[j];
     *		}
     *		d_ptr += sz_bytes;
     *	    }
     */

    /* BLOCK-A �� BLOCK-B �Ȥ�ʬ����� */
    const size_t len_blk = 4 * op_bytes;	/* �� ���� 4 �ϼ�¬��Ĵ�� */

#ifdef _SSE2_IS_OK
    size_t blen_all = sz_bytes * len_all;
    /*                                                      */
    /* BLOCK-S : special code for sz_bytes = 2, 4, 8 and 16 */
    /*                                                      */
    if ( _SSE2_MIN_NBYTES <= blen_all && 
	 sz_bytes <= 16 && (16 % sz_bytes) == 0 ) {
        const size_t nn = 16 / sz_bytes;
	const size_t mm = ((size_t)d_ptr & (sz_bytes - 1));
	unsigned char src_al[16] __attribute__((aligned(16)));
	size_t ix;
	/* create shifted pattern */
	ix = mm;
	for ( i=0 ; i < nn ; i++ ) {
	    for ( j=0 ; j < sz_bytes ; j++ ) {
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
	while ( 16 <= blen_all ) {
	    blen_all -= 16;
	    /* without polluting the cache */
	    _mm_stream_si128((__m128i *)d_ptr, r0);
	    d_ptr += 16;
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
    /*                                  */
    /* BLOCK-A : when sz_bytes is small */
    /*                                  */
    if ( sz_bytes < len_blk ) {
        /* factor_nn ������Ϥ��ʤ��񤷤������ʤ��Ȥ�2�ʾ�����ꤷ�ʤ��ȡ� */
        /* �㤨�� sz_bytes = 3 �ξ��˥ѥե����ޥ󥹤��Фʤ��ʤ�          */
        const size_t factor_nn = 2;
	const size_t nn = factor_nn * (len_blk / sz_bytes);
	const unsigned char *s_ptr0 = src_pat;		/* ���ԡ������ɥ쥹 */
	if ( nn < len_all ) {
	    s_ptr0 = d_ptr;	  /* ���ԡ������ɥ쥹���ѹ�(��Ƭ���Ϥ�Ͽ) */
	    /* �ޤ���Ƭ�� nn �Ĥ����� */
	    len_all -= nn;
	    for ( i=0 ; i < nn ; i++ ) {
		for ( j=0 ; j < sz_bytes ; j++ ) d_ptr[j] = src_pat[j];
		d_ptr += sz_bytes;
	    }
	    /* ³����ʬ�� nn �Ĥ��Ĺ�®�ʥ֥�å�ž�������� */
	    while ( nn <= len_all ) {
		const unsigned char *s_ptr = s_ptr0;
		len_all -= nn;
		j = nn * sz_bytes;
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
	while ( 0 < len_all ) {
	    len_all --;
	    for ( j=0 ; j < sz_bytes ; j++ ) d_ptr[j] = s_ptr0[j];
	    d_ptr += sz_bytes;
	}
    }
    /*                                  */
    /* BLOCK-B : when sz_bytes is large */
    /*                                  */
    else {
	for ( i=0 ; i < len_all ; i++ ) {
	    const unsigned char *s_ptr = src_pat;
	    j = sz_bytes;
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

/* */

int main()
{
    void *buf = NULL;
    const size_t buf_len = 1024L*1024L*2048L;
    double tm0, tm1;
    int i;

    buf = malloc(buf_len + 256);
    if ( buf == NULL ) {
	fprintf(stderr,"[ERROR] malloc() failed\n");
	return 0;
    }

    unsigned char *p0 = (unsigned char *)buf;

    memset(p0, 0, buf_len);

    tm0 = clock() / (double)CLOCKS_PER_SEC;
    memset(p0, 1, buf_len);
    tm1 = clock() / (double)CLOCKS_PER_SEC;
    printf("memset() etime: %g\n",tm1 - tm0);

    tm0 = clock() / (double)CLOCKS_PER_SEC;
    const size_t wlen = 16;
    //               0123456789012345678901234567890123456789012345678901
    s_memfill(p0, 
      (const void *)"_123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ9876543210zyxwvutsrqponmlkjihgfedcbaZYXWVUTSRQPONMLKJIHGFEDCBA",
	      wlen, buf_len / wlen);
    tm1 = clock() / (double)CLOCKS_PER_SEC;
    printf("s_memfill() etime: %g\n",tm1 - tm0);

    for ( i=0 ; i < 80*5 ; i++ ) {
	printf("%c",p0[i]);
    }
    printf("\n");

    if ( buf != NULL ) free(buf);
    return 0;
}
