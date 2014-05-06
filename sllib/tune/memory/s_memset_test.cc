/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2012-12-12 17:44:07 cyamauch> */

/*

  Performance test of s_memset()
  
  g++ -Wall -O2 -msse2 s_memset_test.cc -o s_memset_test -DUSE_SIMD

 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

/* */

#include "../../src/private/s_memset.h"

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
    s_memset((char *)p0, 200, buf_len, 0);
    tm1 = clock() / (double)CLOCKS_PER_SEC;
    printf("s_memset() etime: %g\n",tm1 - tm0);

    for ( i=0 ; i < 100 ; i++ ) {
	printf("[%d]",p0[i]);
    }
    printf("\n");

    if ( buf != NULL ) free(buf);
    return 0;
}

#include "../../src/private/s_memset.cc"
