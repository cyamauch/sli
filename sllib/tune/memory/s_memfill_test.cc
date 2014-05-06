/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2012-12-12 17:44:54 cyamauch> */

/*

  Performance test of s_memfill()
  
  g++ -Wall -O2 -msse2 s_memfill_test.cc -o s_memfill_test -DUSE_SIMD

 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

/* */

#include "../../src/private/memfill.h"
#include "../../src/private/s_memfill.h"

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

    const size_t wlen = 8;

    tm0 = clock() / (double)CLOCKS_PER_SEC;
    //               0123456789012345678901234567890123456789012345678901
    s_memfill(p0, 
      (const void *)"_123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ9876543210zyxwvutsrqponmlkjihgfedcbaZYXWVUTSRQPONMLKJIHGFEDCBA",
	    wlen, buf_len / wlen, 0);
    tm1 = clock() / (double)CLOCKS_PER_SEC;
    printf("s_memfill() etime: %g\n",tm1 - tm0);

    for ( i=0 ; i < 80*5 ; i++ ) {
	printf("%c",p0[i]);
    }
    printf("\n");

    memset(p0, 1, buf_len);

    tm0 = clock() / (double)CLOCKS_PER_SEC;
    //               0123456789012345678901234567890123456789012345678901
    memfill(p0, 
      (const void *)"_123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ9876543210zyxwvutsrqponmlkjihgfedcbaZYXWVUTSRQPONMLKJIHGFEDCBA",
	    wlen, buf_len / wlen);
    tm1 = clock() / (double)CLOCKS_PER_SEC;
    printf("memfill() etime: %g\n",tm1 - tm0);

    for ( i=0 ; i < 80*5 ; i++ ) {
	printf("%c",p0[i]);
    }
    printf("\n");

    if ( buf != NULL ) free(buf);
    return 0;
}

#include "../../src/private/memfill.cc"
#include "../../src/private/s_memfill.cc"
