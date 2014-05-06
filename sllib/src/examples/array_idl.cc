#include <sli/stdstreamio.h>
#include <sli/mdarray.h>
using namespace sli;

/**
 * @file   array_idl.cc
 * @brief  多次元配列を IDL 風の記法で部分選択や演算を行なう例
 */

/*
 * Example for handling array object using IDL-like expressions
 */

int main( int argc, char *argv[] )
{
    stdstreamio sio;
    size_t i, j;

    /* false is set to disable auto resizing when assign(), etc. are called */
    mdarray_float arr0(false);			/* array of float type */

    arr0.resize_2d(8,4);

    for ( i=0 ; i < arr0.row_length() ; i++ ) {       /* Y */
	for ( j=0 ; j < arr0.col_length() ; j++ ) {   /* X */
	    arr0(j,i) = 100 + 10*i + j;
	}
    }
    arr0.dprint();

    sio.eprintf("\n");


    /* select a part of array elements */

    sio.eprintf("after arr1 = arr0.sectionf(\"1:4, *\")\n");

    mdarray_double arr1(false);
    arr1 = arr0.sectionf("1:4, *");

    arr1.dprint();

    sio.eprintf("\n");


    /* paste arr1 to arr0 */

    sio.eprintf("after arr0.pastef(arr1, \"*, 2:3\")\n");

    arr0.pastef(arr1, "*, 2:3");
    arr0.dprint();

    sio.eprintf("\n");


    /* arr0 / arr1 */

    sio.eprintf("after arr0.dividef(arr1, \"*, 2:3\")\n");

    arr0.dividef(arr1, "*, 2:3");
    arr0.dprint();

    sio.eprintf("\n");


    return 0;
}
