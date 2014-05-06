/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2012-11-30 22:20:12 cyamauch> */

/*

  Performance test of s_memswap()
  
  g++ -Wall -O2 -msse2 s_memswap_test.cc -o s_memswap_test -DUSE_SIMD

 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

/* */

#include "../../src/private/memswap.h"
#include "../../src/private/s_memswap.h"

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

    tm0 = clock() / (double)CLOCKS_PER_SEC;
    memswap(p1, p0, buf_len);
    tm1 = clock() / (double)CLOCKS_PER_SEC;
    printf("memswap() etime: %g\n",tm1 - tm0);

    if ( buf0 != NULL ) free(buf0);
    if ( buf1 != NULL ) free(buf1);
    return 0;
}

#include "../../src/private/memswap.cc"
#include "../../src/private/s_memswap.cc"
