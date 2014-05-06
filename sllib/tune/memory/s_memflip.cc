/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2012-11-26 15:50:15 cyamauch> */

/*

  Performance test of s_memflip()
  
  g++ -Wall -O2 -msse2 s_memflip.cc -o s_memflip -DUSE_SIMD

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
 * 任意のバイト長の配列(アライメント規定なし)の前後を反転させます(SSE2対応版)．
 *
 * @param      buf 配列の先頭アドレス
 * @param      sz_bytes 1要素のバイト長
 * @param      len 配列の長さ
 * @note       バイト長が1,2,4,8バイトの場合は特別なコードで動作し，高速です．
 *             その他のバイト長では汎用コードで動作しますが，マシンバイト長
 *             以上では memcpy() が効くので速く動きます．<br>
 *             テスト環境は GCC 3.3.2 + PentiumMのやや古いラップトップと
 *             64-bit Linux．
 */
inline static void s_memflip( void *buf, size_t sz_bytes, size_t len )
{
    const size_t ma_bytes = sizeof(size_t);
    size_t len_loop = len / 2;
    unsigned char *s_ptr0 = (unsigned char *)(buf);	/* first    */
    unsigned char *s_ptr1 = s_ptr0 + (sz_bytes * len);	/* last + 1 */

    if ( sz_bytes == 1 ) {
	unsigned char tmp;
#ifdef _SSE2_IS_OK
	if ( _SSE2_MIN_NBYTES <= sz_bytes * len_loop ) {
	    while ( 16 <= len_loop ) {
		__m128i r0, r1, r0t, r1t;
		len_loop -= 16;
		s_ptr1 -= 16;
		/* load */
		r0 = _mm_loadu_si128((__m128i *)s_ptr0);
		r1 = _mm_loadu_si128((__m128i *)s_ptr1);
		/* 01234567 89abcdef => 89abcdef 01234567 */
		r0 = _mm_shuffle_epi32(r0, _MM_SHUFFLE(1,0,3,2));
		r1 = _mm_shuffle_epi32(r1, _MM_SHUFFLE(1,0,3,2));
		/* 01234567 => 67452301 */
		r0 = _mm_shufflelo_epi16(r0, _MM_SHUFFLE(0,1,2,3));
		r1 = _mm_shufflelo_epi16(r1, _MM_SHUFFLE(0,1,2,3));
		r0 = _mm_shufflehi_epi16(r0, _MM_SHUFFLE(0,1,2,3));
		r1 = _mm_shufflehi_epi16(r1, _MM_SHUFFLE(0,1,2,3));
		r0t = r0;
		r1t = r1;
		/* 1-byte shift */
		r0  = _mm_srli_epi16(r0, 8);
		r1  = _mm_srli_epi16(r1, 8);
		r0t = _mm_slli_epi16(r0t, 8);
		r1t = _mm_slli_epi16(r1t, 8);
		/* or */
		r0 = _mm_or_si128(r0, r0t);
		r1 = _mm_or_si128(r1, r1t);
		/* store */
		_mm_storeu_si128((__m128i *)s_ptr1, r0);
		_mm_storeu_si128((__m128i *)s_ptr0, r1);
		/* */
		s_ptr0 += 16;
	    }
	    
	}
#endif	/* _SSE2_IS_OK */
	while ( 0 < len_loop ) {
	    len_loop --;
	    s_ptr1 --;
	    tmp = *s_ptr0;  *s_ptr0 = *s_ptr1;  *s_ptr1 = tmp;
	    s_ptr0 ++;
	}
    }
    else if ( sz_bytes == 2 ) {
	unsigned char tmpbuf[2];
#ifdef _SSE2_IS_OK
	if ( _SSE2_MIN_NBYTES <= sz_bytes * len_loop ) {
	    while ( 8 <= len_loop ) {
		__m128i r0, r1;
		len_loop -= 8;
		s_ptr1 -= 16;
		/* load */
		r0 = _mm_loadu_si128((__m128i *)s_ptr0);
		r1 = _mm_loadu_si128((__m128i *)s_ptr1);
		/* 01234567 89abcdef => 89abcdef 01234567 */
		r0 = _mm_shuffle_epi32(r0, _MM_SHUFFLE(1,0,3,2));
		r1 = _mm_shuffle_epi32(r1, _MM_SHUFFLE(1,0,3,2));
		/* 01234567 => 67452301 */
		r0 = _mm_shufflelo_epi16(r0, _MM_SHUFFLE(0,1,2,3));
		r1 = _mm_shufflelo_epi16(r1, _MM_SHUFFLE(0,1,2,3));
		r0 = _mm_shufflehi_epi16(r0, _MM_SHUFFLE(0,1,2,3));
		r1 = _mm_shufflehi_epi16(r1, _MM_SHUFFLE(0,1,2,3));
		/* store */
		_mm_storeu_si128((__m128i *)s_ptr1, r0);
		_mm_storeu_si128((__m128i *)s_ptr0, r1);
		/* */
		s_ptr0 += 16;
	    }
	    
	}
#endif	/* _SSE2_IS_OK */
	while ( 0 < len_loop ) {
	    len_loop --;
	    s_ptr1 -= 2;
	    memcpy(tmpbuf,s_ptr0,2);
	    memcpy(s_ptr0,s_ptr1,2);
	    memcpy(s_ptr1,tmpbuf,2);
	    s_ptr0 += 2;
	}
    }
    else if ( sz_bytes == 4 ) {
	unsigned char tmpbuf[4];
#ifdef _SSE2_IS_OK
	if ( _SSE2_MIN_NBYTES <= sz_bytes * len_loop ) {
	    while ( 4 <= len_loop ) {
		__m128i r0, r1;
		len_loop -= 4;
		s_ptr1 -= 16;
		/* load */
		r0 = _mm_loadu_si128((__m128i *)s_ptr0);
		r1 = _mm_loadu_si128((__m128i *)s_ptr1);
		/* 01234567 89abcdef => cdef89ab 45670123 */
		r0 = _mm_shuffle_epi32(r0, _MM_SHUFFLE(0,1,2,3));
		r1 = _mm_shuffle_epi32(r1, _MM_SHUFFLE(0,1,2,3));
		/* store */
		_mm_storeu_si128((__m128i *)s_ptr1, r0);
		_mm_storeu_si128((__m128i *)s_ptr0, r1);
		/* */
		s_ptr0 += 16;
	    }
	}
#endif	/* _SSE2_IS_OK */
	while ( 0 < len_loop ) {
	    len_loop --;
	    s_ptr1 -= 4;
	    memcpy(tmpbuf,s_ptr0,4);
	    memcpy(s_ptr0,s_ptr1,4);
	    memcpy(s_ptr1,tmpbuf,4);
	    s_ptr0 += 4;
	}
    }
    else if ( sz_bytes == 8 ) {
	unsigned char tmpbuf[8];
#ifdef _SSE2_IS_OK
	if ( _SSE2_MIN_NBYTES <= sz_bytes * len_loop ) {
	    while ( 2 <= len_loop ) {
		__m128i r0, r1;
		len_loop -= 2;
		s_ptr1 -= 16;
		/* load */
		r0 = _mm_loadu_si128((__m128i *)s_ptr0);
		r1 = _mm_loadu_si128((__m128i *)s_ptr1);
		/* 01234567 89abcdef => 89abcdef 01234567 */
		r0 = _mm_shuffle_epi32(r0, _MM_SHUFFLE(1,0,3,2));
		r1 = _mm_shuffle_epi32(r1, _MM_SHUFFLE(1,0,3,2));
		/* store */
		_mm_storeu_si128((__m128i *)s_ptr1, r0);
		_mm_storeu_si128((__m128i *)s_ptr0, r1);
		/* */
		s_ptr0 += 16;
	    }
	}
#endif	/* _SSE2_IS_OK */
	/* 32-bit機では 4バイトずつやるのが正解? */
	while ( 0 < len_loop ) {
	    len_loop --;
	    s_ptr1 -= 8;
	    memcpy(tmpbuf,s_ptr0,8);
	    memcpy(s_ptr0,s_ptr1,8);
	    memcpy(s_ptr1,tmpbuf,8);
	    s_ptr0 += 8;
	}
    }
#ifdef _SSE2_IS_OK
    else if ( sz_bytes == 16 ) {
	unsigned char tmpbuf[16];
	if ( _SSE2_MIN_NBYTES <= sz_bytes * len_loop ) {
	    while ( 1 <= len_loop ) {
		__m128i r0, r1;
		len_loop -= 1;
		s_ptr1 -= 16;
		/* load */
		r0 = _mm_loadu_si128((__m128i *)s_ptr0);
		r1 = _mm_loadu_si128((__m128i *)s_ptr1);
		/* store */
		_mm_storeu_si128((__m128i *)s_ptr1, r0);
		_mm_storeu_si128((__m128i *)s_ptr0, r1);
		/* */
		s_ptr0 += 16;
	    }
	}
	while ( 0 < len_loop ) {
	    len_loop --;
	    s_ptr1 -= 16;
	    memcpy(tmpbuf,s_ptr0,16);
	    memcpy(s_ptr0,s_ptr1,16);
	    memcpy(s_ptr1,tmpbuf,16);
	    s_ptr0 += 16;
	}
    }
#endif	/* _SSE2_IS_OK */
    else if ( ma_bytes <= sz_bytes ) {
	unsigned char tmpbuf[ma_bytes];
	unsigned char tmp;
	size_t i,j;
	const size_t n_j0 = sz_bytes / ma_bytes;
	const size_t n_j1 = sz_bytes % ma_bytes;
	s_ptr1 += ma_bytes * n_j0;			/* offset */
	for ( i=0 ; i < len_loop ; i++ ) {
	    s_ptr1 -= (ma_bytes * n_j0 + sz_bytes);
	    for ( j=0 ; j < n_j0 ; j++ ) {
		memcpy(tmpbuf,s_ptr0,ma_bytes);
		memcpy(s_ptr0,s_ptr1,ma_bytes);
		memcpy(s_ptr1,tmpbuf,ma_bytes);
		s_ptr0 += ma_bytes;
		s_ptr1 += ma_bytes;
	    }
	    for ( j=0 ; j < n_j1 ; j++ ) {
		tmp = s_ptr0[j];  s_ptr0[j] = s_ptr1[j];  s_ptr1[j] = tmp;
	    }
	    s_ptr0 += n_j1;
	}
    }
    else {
	unsigned char tmp;
	size_t i,j;
	for ( i=0 ; i < len_loop ; i++ ) {
	    s_ptr1 -= sz_bytes;
	    for ( j=0 ; j < sz_bytes ; j++ ) {
		tmp = s_ptr0[j];  s_ptr0[j] = s_ptr1[j];  s_ptr1[j] = tmp;
	    }
	    s_ptr0 += sz_bytes;
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

    for ( i=0 ; i < 256 ; i++ ) p0[i] = i;

    printf("before: \n");
    for ( i=0 ; i < 256 ; i++ ) {
	printf("[%d]",p0[i]);
    }
    printf("\n");

    const size_t wlen = 16;

    tm0 = clock() / (double)CLOCKS_PER_SEC;
    s_memflip((char *)p0, wlen, buf_len / wlen);
    //s_memflip((char *)p0, wlen, 256 / wlen);
    tm1 = clock() / (double)CLOCKS_PER_SEC;
    printf("s_memflip() etime: %g\n",tm1 - tm0);

    printf("after: \n");
    for ( i=0 ; i < 256 ; i++ ) {
	printf("[%d]",p0[i]);
    }
    printf("\n");

    if ( buf != NULL ) free(buf);
    return 0;
}
