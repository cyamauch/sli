/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2012-11-26 16:18:55 cyamauch> */

/*

  Performance test of 4-byte byte swap
  
  g++ -Wall -O2 -msse2 s_cvt_i16tod.cc -o s_cvt_i16tod -DUSE_SIMD

 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* */

#include <stdint.h>
#include "_test_sse2.h"

#ifdef _SSE2_IS_OK
#include <emmintrin.h>
#include "_tune_sse2.h"
#endif

inline static void s_cvt_i16tod( const void *src, void *dest, 
				 size_t len_elements )
{
    const unsigned char *s_ptr = (const unsigned char *)src;
    unsigned char *d_ptr = (unsigned char *)dest;
    s_ptr += 2 * len_elements;	/* for int16_t */
    d_ptr += 8 * len_elements;	/* for double */

#ifdef _SSE2_IS_OK
    if ( _SSE2_MIN_NBYTES <= 8 * len_elements ) {
	bool is_aligned = ( ((size_t)d_ptr & 0x07) == 0 );
	if ( is_aligned == true ) {			/* align if possible */
	    while ( ((size_t)d_ptr & 0x0f) != 0 ) {
		len_elements --;
		s_ptr -= 2;
		d_ptr -= 8;
		*((double *)d_ptr) = *((int16_t *)s_ptr);
	    }
	}
	__m128i r0 = _mm_setzero_si128();
	while ( 8 <= len_elements ) {
	    __m128i r1, r2;
	    __m128d r3;
	    len_elements -= 8;
	    s_ptr -= 16;
	    r1 = _mm_loadu_si128((__m128i *)s_ptr);
	    /* convert right 4 int16  */
	    r2 = _mm_unpackhi_epi16(r0, r1);	/* interleave */
	    r2 = _mm_srai_epi32(r2, 16);	/* shift 16-bit */
	    r3 = _mm_cvtepi32_pd(r2);		/* convert int32 => double */
	    d_ptr -= 32;
	    _mm_storeu_pd((double *)d_ptr, r3);
	    r2 = _mm_srli_si128(r2, 8);		/* shift 8-byte */
	    r3 = _mm_cvtepi32_pd(r2);		/* convert int32 => double */
	    _mm_storeu_pd((double *)(d_ptr+16), r3);
	    /* convert left 4 int16  */
	    r2 = _mm_unpacklo_epi16(r0, r1);
	    r2 = _mm_srai_epi32(r2, 16);
	    r3 = _mm_cvtepi32_pd(r2);
	    d_ptr -= 32;
	    _mm_storeu_pd((double *)d_ptr, r3);
	    r2 = _mm_srli_si128(r2, 8);
	    r3 = _mm_cvtepi32_pd(r2);
	    _mm_storeu_pd((double *)(d_ptr+16), r3);
	}
    }
#endif	/* _SSE2_IS_OK */
    while ( 0 < len_elements ) {
	len_elements --;
	s_ptr -= 2;
	d_ptr -= 8;
	*((double *)d_ptr) = *((int16_t *)s_ptr);
    }

    return;
}

/* */

int main()
{
    void *buf0 = NULL;
    void *buf1 = NULL;
    const size_t buf_len = 1024L*1024L*2048L;
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
    memset(buf0, 0, buf_len + 256);
    memset(buf1, 0, buf_len + 256);

    char *p0 = (char *)buf0;
    short *p0s = (short *)p0;
    double *p0d = (double *)p0;
    
    tm0 = clock() / (double)CLOCKS_PER_SEC;
    memset(p0, 1, buf_len);
    tm1 = clock() / (double)CLOCKS_PER_SEC;
    printf("memset() etime: %g\n",tm1 - tm0);

    //for ( i=0 ; i < 256 ; i++ ) p0s[i] = 2000 + i;
    for ( i=0 ; i < 256 ; i++ ) p0s[i] = -32000 + i;
    //for ( i=0 ; i < 256 ; i++ ) {
    //	if ( (i % 2) == 0 ) p0[i] = i;
    //	else p0[i] = 255 - i;
    //}

    printf("convert and store to another buffer:\n");
    tm0 = clock() / (double)CLOCKS_PER_SEC;
    s_cvt_i16tod(p0, buf1, buf_len / 8);
    tm1 = clock() / (double)CLOCKS_PER_SEC;
    printf("s_cvt_i16tod() etime: %g\n",tm1 - tm0);
    printf("\n");

    printf("before:\n");
    for ( i=0 ; i < 128 ; i++ ) {
	printf("[%02hhx]",p0[i]);
	if ( (i % 16) == 15 ) printf("\n");
    }
    printf("\n");

    printf("convert at the same buffer:\n");
    tm0 = clock() / (double)CLOCKS_PER_SEC;
    s_cvt_i16tod(p0, p0, buf_len / 8);
    tm1 = clock() / (double)CLOCKS_PER_SEC;
    printf("s_cvt_i16tod() etime: %g\n",tm1 - tm0);
    printf("\n");

    printf("after:\n");
    for ( i=0 ; i < 128 ; i++ ) {
	printf("[%02hhx]",p0[i]);
	if ( (i % 16) == 15 ) printf("\n");
    }
    printf("\n");

    printf("converted double values:\n");
    for ( i=0 ; i < 128 ; i++ ) {
	printf("[%.15g]",p0d[i]);
	if ( (i % 16) == 15 ) printf("\n");
    }
    printf("\n");

    if ( buf0 != NULL ) free(buf0);
    return 0;
}
