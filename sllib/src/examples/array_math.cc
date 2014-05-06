#include <sli/stdstreamio.h>
#include <sli/mdarray.h>
#include <sli/mdarray_math.h>
using namespace sli;

/**
 * @file   array_math.cc
 * @brief  多次元配列に対する数学関数を使った例
 */

/*
 * Example for using math functions to array object
 */

int main( int argc, char *argv[] )
{
    stdstreamio sio;
    size_t i, j;

    /* false is set to disable auto resizing when assign(), etc. are called */
    mdarray_double arr0(false);			/* array of double type */

    arr0.resize_2d(8,4);

    for ( i=0 ; i < arr0.row_length() ; i++ ) {       /* Y */
	for ( j=0 ; j < arr0.col_length() ; j++ ) {   /* X */
	    arr0(j,i) = 100 + 10*i + j;
	}
    }
    arr0.dprint();

    sio.eprintf("\n");


    /* pow */

    sio.eprintf("after arr0 = pow(arr0,2)\n");

    arr0 = pow(arr0,2);
    arr0.dprint();

    sio.eprintf("\n");


    /* log10 */

    sio.eprintf("after arr0 = log10(arr0)\n");

    arr0 = log10(arr0);
    arr0.dprint();

    sio.eprintf("\n");


    /* sqrt */

    sio.eprintf("after arr0 = sqrt(arr0)\n");

    arr0 = sqrt(arr0);
    arr0.dprint();


    return 0;
}
