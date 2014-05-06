/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2012-11-30 13:31:49 cyamauch> */

/*

  Performance test of s_memset()
  
  g++ -Wall -O2 -msse2 s_memset.cc -o s_memset -DUSE_SIMD

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

inline static void *s_memset( void *s, int c, size_t n )
{
    unsigned char *d_ptr = (unsigned char *)s;

#ifdef _SSE2_IS_OK
    if ( _SSE2_MIN_NBYTES <= n ) {	/* use SSE2 if n is large enough */
	const unsigned char src[16] __attribute__((aligned(16))) = 
					  {c,c,c,c, c,c,c,c, c,c,c,c, c,c,c,c};
	size_t mm = ((size_t)d_ptr & 0x0f);
	if ( 0 < mm ) {			/* align with 16-byte */
	    mm = 16 - mm;
	    n -= mm;
	    memset(d_ptr, c, mm);
	    d_ptr += mm;
	}
	__m128i r0 = _mm_load_si128((__m128i *)src);
	while ( 16 <= n ) {
	    n -= 16;
	    /* without polluting the cache */
	    _mm_stream_si128((__m128i *)d_ptr, r0);
	    d_ptr += 16;
	}
    }
#endif	/* _SSE2_IS_OK */

    memset(d_ptr, c, n);

    return s;
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
    s_memset((char *)p0, 200, buf_len);
    tm1 = clock() / (double)CLOCKS_PER_SEC;
    printf("s_memset() etime: %g\n",tm1 - tm0);

    for ( i=0 ; i < 100 ; i++ ) {
	printf("[%d]",p0[i]);
    }
    printf("\n");

    if ( buf != NULL ) free(buf);
    return 0;
}
