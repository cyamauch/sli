#include <sli/stdstreamio.h>
#include <sli/mdarray.h>
using namespace sli;

/**
 * @file   array_basic.cc
 * @brief  多次元配列の扱い方の基本を示したコード
 */

/*
 * Basic example for handling array objects
 */

int main( int argc, char *argv[] )
{
    stdstreamio sio;
    size_t i, j;

    mdarray_double arr;				/* array of double type */

    arr[0] = 1.57079632679489661923;		/* auto resizing */
    arr[1] = 3.14159265358979323846;

    /* display */
    for ( i=0 ; i < arr.length() ; i++ ) {
        sio.printf("[%f]\n", arr[i]);
    }

    sio.printf("\n");


    /* false is set to disable auto resizing when assign(), etc. are called */
    mdarray_float arr0(false);			/* array of float type */

    /* set length of 2-d array */
    arr0.resize_2d(8,4);

    arr0 = 500.0;
    arr0 *= 2;

    /* display */
    for ( i=0 ; i < arr0.length(1) ; i++ ) {			/* Y */
	for ( j=0 ; j < arr0.length(0) ; j++ ) {		/* X */
	    sio.printf("[%g]", arr0(j,i));
	}
	sio.printf("\n");
    }

    return 0;
}
