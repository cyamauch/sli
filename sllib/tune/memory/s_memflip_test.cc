/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2012-11-30 22:26:24 cyamauch> */

/*

  Performance test of s_memflip()
  
  g++ -Wall -O2 -msse2 s_memflip_test.cc -o s_memflip_test -DUSE_SIMD

 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

/* */

#include "../../src/private/memflip.h"
#include "../../src/private/s_memflip.h"

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

    tm0 = clock() / (double)CLOCKS_PER_SEC;
    memflip((char *)p0, wlen, buf_len / wlen);
    tm1 = clock() / (double)CLOCKS_PER_SEC;
    printf("memflip() etime: %g\n",tm1 - tm0);

    if ( buf != NULL ) free(buf);
    return 0;
}

#include "../../src/private/memflip.cc"
#include "../../src/private/s_memflip.cc"
