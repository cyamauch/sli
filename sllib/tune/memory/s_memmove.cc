/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2012-11-30 14:53:46 cyamauch> */

/*

  Performance test of s_memmove()
  
  g++ -Wall -O2 -msse2 s_memmove.cc -o s_memmove -DUSE_SIMD

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

/*
 * SSE2対応版 memmove．
 */
inline static void *s_memmove( void *dest, const void *src, size_t n )
{
#ifdef _SSE2_IS_OK
    if ( _SSE2_MIN_NBYTES <= n ) {	/* use SSE2 if n is large enough */
	if ( src < dest ) {
	    unsigned char *d_ptr = (unsigned char *)dest;
	    const unsigned char *s_ptr = (const unsigned char *)src;
	    size_t mm;
	    d_ptr += n;
	    s_ptr += n;
	    mm = ((size_t)d_ptr & 0x0f);
	    if ( 0 < mm ) {		/* align with 16-byte for dest */
		n -= mm;
		d_ptr -= mm;
		s_ptr -= mm;
		memmove(d_ptr, s_ptr, mm);
	    }
	    while ( 16 <= n ) {
		__m128i r0;
		n -= 16;
		d_ptr -= 16;
		s_ptr -= 16;
		r0 = _mm_loadu_si128((__m128i *)s_ptr);
		_mm_store_si128((__m128i *)d_ptr, r0);
	    }
	    memmove(dest, src, n);	/* NOTE: don't use d_ptr, s_ptr here */
	}
	else if ( dest < src ) {
	    unsigned char *d_ptr = (unsigned char *)dest;
	    const unsigned char *s_ptr = (const unsigned char *)src;
	    size_t mm;
	    mm = ((size_t)d_ptr & 0x0f);
	    if ( 0 < mm ) {		/* align with 16-byte for dest */
		mm = (16 - mm);
		n -= mm;
		memmove(d_ptr, s_ptr, mm);
		d_ptr += mm;
		s_ptr += mm;
	    }
	    while ( 16 <= n ) {
		__m128i r0;
		n -= 16;
		r0 = _mm_loadu_si128((__m128i *)s_ptr);
		_mm_store_si128((__m128i *)d_ptr, r0);
		d_ptr += 16;
		s_ptr += 16;
	    }
	    memmove(d_ptr, s_ptr, n);
	}
	else {
	    /* do nothing */
	}
    }
    else {
	memmove(dest, src, n);
    }
#else

    memmove(dest, src, n);

#endif	/* _SSE2_IS_OK */

    return dest;
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
    memset(p0, 1, buf_len);
    tm1 = clock() / (double)CLOCKS_PER_SEC;
    printf("memset() etime: %g\n",tm1 - tm0);

    for ( i=0 ; i < 256 ; i++ ) p0[i] = i;

    tm0 = clock() / (double)CLOCKS_PER_SEC;
    s_memmove(p1, p0, buf_len);
    tm1 = clock() / (double)CLOCKS_PER_SEC;
    printf("s_memmove() etime: %g\n",tm1 - tm0);

    for ( i=0 ; i < 100 ; i++ ) {
	printf("[%d]",p1[i]);
    }
    printf("\n");

    memset(p0, 0, buf_len);

    tm0 = clock() / (double)CLOCKS_PER_SEC;
    s_memmove(p0, p1, buf_len);
    tm1 = clock() / (double)CLOCKS_PER_SEC;
    printf("s_memmove() etime: %g\n",tm1 - tm0);

    for ( i=0 ; i < 100 ; i++ ) {
	printf("[%d]",p0[i]);
    }
    printf("\n");

    memset(p0, 0, buf_len);

    tm0 = clock() / (double)CLOCKS_PER_SEC;
    memcpy(p0, p1, buf_len);
    tm1 = clock() / (double)CLOCKS_PER_SEC;
    printf("memcpy() etime: %g\n",tm1 - tm0);

    memset(p0, 0, buf_len);

    tm0 = clock() / (double)CLOCKS_PER_SEC;
    memmove(p0, p1, buf_len);
    tm1 = clock() / (double)CLOCKS_PER_SEC;
    printf("memmove() etime: %g\n",tm1 - tm0);

    if ( buf0 != NULL ) free(buf0);
    if ( buf1 != NULL ) free(buf1);
    return 0;
}
