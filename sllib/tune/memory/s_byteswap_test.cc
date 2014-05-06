/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2012-11-30 12:34:51 cyamauch> */

/*

  Performance test of s_byteswap()
  
  g++ -Wall -O2 -msse2 s_byteswap_test.cc -o s_byteswap_test -DUSE_SIMD

 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* */

#include "../../src/private/s_byteswap.h"

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

    //               ----____----____----____----____----____----____----
    snprintf(p0,256,"ABCDEFGHIJKLMNOPQRSTUVWXYZ0123abcdefghijklmnopqrstuvwxyz0123ABCDEFGHIJKLMNOPQRSTUVWXYZ0123abcdefghijklmnopqrstuvwxyz0123");

    const size_t wlen = 8;

    tm0 = clock() / (double)CLOCKS_PER_SEC;
    s_byteswap((char *)p0, wlen, buf_len / wlen);
    tm1 = clock() / (double)CLOCKS_PER_SEC;
    printf("s_byteswap() etime: %g\n",tm1 - tm0);

    printf("0123456789012345678901234567890123456789012345678901234567890\n");
    for ( i=0 ; i < 4 * 14 ; i++ ) {
	printf("%c",p0[i]);
    }
    printf("\n");

    if ( buf != NULL ) free(buf);
    return 0;
}

#include "../../src/private/s_byteswap.cc"
