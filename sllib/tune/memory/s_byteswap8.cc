/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2012-11-25 17:16:53 cyamauch> */

/*

  Performance test of 8-byte byte swap
  
  g++ -Wall -O2 -msse2 s_byteswap8.cc -o s_byteswap8 -DUSE_SIMD

 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* */

#define USE_OPTIMIZED_BYTE_SWAP 1

#include "_test_sse2.h"

#ifdef _SSE2_IS_OK
#include <emmintrin.h>
#include "_tune_sse2.h"
#endif

inline static void s_byteswap8( void *buf, size_t len_elements )
{
    unsigned char *d_ptr = (unsigned char *)buf;
    unsigned char tmp;

#ifdef USE_OPTIMIZED_BYTE_SWAP
#ifdef _SSE2_IS_OK
    if ( _SSE2_MIN_NBYTES <= 8 * len_elements ) {
	bool is_aligned = ( ((size_t)d_ptr & 0x07) == 0 );
	if ( is_aligned == true ) {			/* align if possible */
	    while ( ((size_t)d_ptr & 0x0f) != 0 ) {
		len_elements --;
		tmp = d_ptr[0];  d_ptr[0] = d_ptr[7];  d_ptr[7] = tmp;
		tmp = d_ptr[1];  d_ptr[1] = d_ptr[6];  d_ptr[6] = tmp;
		tmp = d_ptr[2];  d_ptr[2] = d_ptr[5];  d_ptr[5] = tmp;
		tmp = d_ptr[3];  d_ptr[3] = d_ptr[4];  d_ptr[4] = tmp;
		d_ptr += 8;
	    }
	}
	/* NOTE: Do not use _mm_stream...() that causes slow down.      */
	/*       Using load+store improves slightly, but u+u is enough. */
	while ( 2 <= len_elements ) {
	    len_elements -= 2;
	    __m128i r0 = _mm_loadu_si128((__m128i *)d_ptr);
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
	    d_ptr += 8 * 2;
	}
    }
#else
    while ( 2 <= len_elements ) {
	len_elements -= 2;
	tmp = d_ptr[0];  d_ptr[0] = d_ptr[7];  d_ptr[7] = tmp;
	tmp = d_ptr[1];  d_ptr[1] = d_ptr[6];  d_ptr[6] = tmp;
	tmp = d_ptr[2];  d_ptr[2] = d_ptr[5];  d_ptr[5] = tmp;
	tmp = d_ptr[3];  d_ptr[3] = d_ptr[4];  d_ptr[4] = tmp;
	tmp = d_ptr[8];  d_ptr[8] = d_ptr[15];  d_ptr[15] = tmp;
	tmp = d_ptr[9];  d_ptr[9] = d_ptr[14];  d_ptr[14] = tmp;
	tmp = d_ptr[10];  d_ptr[10] = d_ptr[13];  d_ptr[13] = tmp;
	tmp = d_ptr[11];  d_ptr[11] = d_ptr[12];  d_ptr[12] = tmp;
	d_ptr += 8 * 2;
    }
#endif	/* _SSE2_IS_OK */
#endif	/* USE_OPTIMIZED_BYTE_SWAP */

    while ( 0 < len_elements ) {
	len_elements --;
	tmp = d_ptr[0];  d_ptr[0] = d_ptr[7];  d_ptr[7] = tmp;
	tmp = d_ptr[1];  d_ptr[1] = d_ptr[6];  d_ptr[6] = tmp;
	tmp = d_ptr[2];  d_ptr[2] = d_ptr[5];  d_ptr[5] = tmp;
	tmp = d_ptr[3];  d_ptr[3] = d_ptr[4];  d_ptr[4] = tmp;
	d_ptr += 8;
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

    char *p0 = (char *)buf;

    memset(p0, 0, buf_len);

    tm0 = clock() / (double)CLOCKS_PER_SEC;
    memset(p0, 1, buf_len);
    tm1 = clock() / (double)CLOCKS_PER_SEC;
    printf("memset() etime: %g\n",tm1 - tm0);

    //               ----____----____----____----____----____----____----____
    snprintf(p0,256,"ABCDEFGHijklmnopQRSTUVWX0123abcdEFGHIJKLmnopqrstUVWX0123");

    tm0 = clock() / (double)CLOCKS_PER_SEC;
    s_byteswap8((char *)p0, buf_len/8);
    tm1 = clock() / (double)CLOCKS_PER_SEC;
    printf("s_byteswap8() etime: %g\n",tm1 - tm0);

    for ( i=0 ; i < 8 * 7 ; i++ ) {
	printf("%c",p0[i]);
    }
    printf("\n");

    if ( buf != NULL ) free(buf);
    return 0;
}
