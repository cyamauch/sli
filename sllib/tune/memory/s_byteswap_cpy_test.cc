/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2012-11-29 13:39:42 cyamauch> */

/*

  Performance test of s_byteswap_cpy()
  
  g++ -Wall -O2 -msse2 s_byteswap_cpy_test.cc -o s_byteswap_cpy_test -DUSE_SIMD

 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* */

#include "../../src/private/s_byteswap_cpy.h"

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

    char *p0 = (char *)buf0;
    char *p1 = (char *)buf1;

    memset(p0, 0, buf_len);

    tm0 = clock() / (double)CLOCKS_PER_SEC;
    memset(p0, 1, buf_len);
    tm1 = clock() / (double)CLOCKS_PER_SEC;
    printf("memset() etime: %g\n",tm1 - tm0);

    //               ----____----____----____----____----____----____----
    snprintf(p0,256,"ABCDEFGHIJKLMNOPQRSTUVWXYZ0123abcdefghijklmnopqrstuvwxyz0123ABCDEFGHIJKLMNOPQRSTUVWXYZ0123abcdefghijklmnopqrstuvwxyz0123");

    const size_t wlen = 4;

    tm0 = clock() / (double)CLOCKS_PER_SEC;
    s_byteswap_cpy(p0, wlen, buf_len / wlen, p1);
    tm1 = clock() / (double)CLOCKS_PER_SEC;
    printf("s_byteswap_cpy() etime: %g\n",tm1 - tm0);

    printf("012345678901234567890123456789012345678901234567890123456789\n");
    for ( i=0 ; i < 4 * 14 ; i++ ) {
	printf("%c",p1[i]);
    }
    printf("\n");

    if ( buf0 != NULL ) free(buf0);
    if ( buf1 != NULL ) free(buf1);

    return 0;
}

#include "../../src/private/s_byteswap_cpy.cc"
