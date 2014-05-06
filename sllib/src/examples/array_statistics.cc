#include <sli/stdstreamio.h>
#include <sli/mdarray.h>
#include <sli/mdarray_math.h>
#include <sli/mdarray_statistics.h>
using namespace sli;

/**
 * @file   array_statistics.cc
 * @brief  多次元配列に対する統計用関数を使った例
 */

/*
 * Example for statistics of array object
 */

int main( int argc, char *argv[] )
{
    stdstreamio sio;
    size_t i, j;

    /* false is set to disable auto resizing when assign(), etc. are called */
    mdarray_float arr0(false);                  /* array of float type */
    mdarray_float arr1(false);

    arr0.resize_2d(8,5);

    for ( i=0 ; i < arr0.row_length() ; i++ ) {       /* Y */
        for ( j=0 ; j < arr0.col_length() ; j++ ) {   /* X */
            arr0(j,i) = 100 + 10*i + j;
        }
    }
    arr0 = pow(arr0, 1.5);

    arr0.dprint();

    sio.eprintf("\n");


    /* get moment */

    double mdev, sdev;
    mdarray_double mm = md_moment(arr0, false, &mdev, &sdev);

    sio.printf("mean = %g\n", mm[0]);
    sio.printf("variance = %g\n", mm[1]);
    sio.printf("skewness = %g\n", mm[2]);
    sio.printf("kurtosis = %g\n", mm[3]);
    sio.printf("meanabsdev = %g\n", mdev);
    sio.printf("stddev = %g\n", sdev);

    sio.printf("\n");


    /* get mean with x */

    sio.eprintf("mean of x\n");
    arr1 = md_mean_x(arr0);
    arr1.dprint();

    sio.eprintf("\n");


    /* get median with y */

    sio.eprintf("median of y\n");
    arr1 = md_median_y(arr0);
    arr1.dprint();


    return 0;
}
