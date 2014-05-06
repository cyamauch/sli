/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2012-11-25 18:35:26 cyamauch> */

/*

  Performance test of s_memswap()
  
  g++ -Wall -O2 -msse2 s_memswap.cc -o s_memswap -DUSE_SIMD

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
 * 2つのバッファの内容(アライメント規定なし)を入れ替えます(SSE2対応版)．
 *
 * @param      buf0 1つめの配列の先頭アドレス
 * @param      buf1 2つめの配列の先頭アドレス
 * @param      len 配列の長さ
 * @note       SSE2, memcpy() が効くので速く動きます．<br>
 *             テスト環境は GCC 3.3.2 + PentiumMのやや古いラップトップと
 *             64-bit Linux．
 */
inline static void s_memswap( void *buf0, void *buf1, size_t len )
{
    const size_t op_bytes = sizeof(size_t) * 2;
    unsigned char *s_ptr = (unsigned char *)buf0;
    unsigned char *d_ptr = (unsigned char *)buf1;
    unsigned char tmpbuf[op_bytes];
    unsigned char tmp;
    size_t i;

    i = len;

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

/* */

int main()
{
    void *buf0 = NULL;
    void *buf1 = NULL;
    const size_t buf_len = 1024L*1024L*1024L;
    double tm0, tm1;
    int i;

    buf0 = malloc(buf_len + 256);
    if ( buf0 == NULL ) {
	fprintf(stderr,"[ERROR] malloc() failed\n");
	return 0;
    }
    buf1 = malloc(buf_len + 256);
    if ( buf1 == NULL ) {
	fprintf(stderr,"[ERROR] malloc() failed\n");
	return 0;
    }

    unsigned char *p0 = (unsigned char *)buf0;
    unsigned char *p1 = (unsigned char *)buf1;

    memset(p0, 0, buf_len);
    memset(p1, 0, buf_len);

    tm0 = clock() / (double)CLOCKS_PER_SEC;
    memset(p1, 1, buf_len);
    tm1 = clock() / (double)CLOCKS_PER_SEC;
    printf("memset() etime: %g\n",tm1 - tm0);

    for ( i=0 ; i < 256 ; i++ ) p0[i] = i;

    printf("p0: ");
    for ( i=0 ; i < 100 ; i++ ) {
	printf("[%d]",p0[i]);
    }
    printf("\n");
    printf("p1: ");
    for ( i=0 ; i < 100 ; i++ ) {
	printf("[%d]",p1[i]);
    }
    printf("\n");

    tm0 = clock() / (double)CLOCKS_PER_SEC;
    s_memswap(p1, p0, buf_len);
    tm1 = clock() / (double)CLOCKS_PER_SEC;
    printf("s_memswap() etime: %g\n",tm1 - tm0);

    printf("p0: ");
    for ( i=0 ; i < 100 ; i++ ) {
	printf("[%d]",p0[i]);
    }
    printf("\n");
    printf("p1: ");
    for ( i=0 ; i < 100 ; i++ ) {
	printf("[%d]",p1[i]);
    }
    printf("\n");


    if ( buf0 != NULL ) free(buf0);
    if ( buf1 != NULL ) free(buf1);
    return 0;
}
