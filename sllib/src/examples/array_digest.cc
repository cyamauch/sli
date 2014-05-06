#include <sli/stdstreamio.h>
#include <sli/mdarray.h>
#include <sli/mdarray_math.h>
using namespace sli;

/**
 * @file   array_digest.cc
 * @brief  多次元配列の様々な機能のデモ・コード
 */

/*
 * A first example for handling array objects
 */

int main( int argc, char *argv[] )
{
    stdstreamio sio;
    const double arr0_src[] = {0.02, 0.2, 2.0,  20.0, 200.0, 2000.0,  1, 2, 3};
    mdarray_double arr0;				/* array object */
    mdarray_double arr1;				/* array object */
    size_t i, j;

    /* set dimension and length of array (x,y) */
    arr0.resize_2d(3,3);

    /* set initial values */
    arr0.put_elements(arr0_src, 3*3);

    /* select first 2 rows of arr0 */
    arr1 = arr0.sectionf("*, 0:1");

    /* operation for array and scalar */
    arr1 = log10(arr1 / 2);

    /* display result */
    for ( j=0 ; j < arr1.length(1) ; j++ ) {
        for ( i=0 ; i < arr1.length(0) ; i++ ) sio.printf("[%g]", arr1(i,j));
        sio.printf("\n");
    }

    return 0;
}
