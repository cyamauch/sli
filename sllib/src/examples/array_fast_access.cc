#include <sli/stdstreamio.h>
#include <sli/mdarray.h>
using namespace sli;

/**
 * @file   array_fast_access.cc
 * @brief  3次元配列までの要素にポインタ変数で高速にアクセスする例
 */

/*
 * Example for fast access of array objects
 */

int main( int argc, char *argv[] )
{
    stdstreamio sio;
    size_t i, j, k;

    /* false is set to disable auto resizing when assign(), etc. are called */
    mdarray_float arr0(false);                  /* array of float type */
    mdarray_double arr1(false);                 /* array of double type */

    /* set length of 2-d array (x,y) */
    arr0.resize_2d(8,4);

    /* 1-d ptr access */
    float *arr0_ptr = arr0.array_ptr();
    for ( i=0 ; i < arr0.length() ; i++ ) {
        arr0_ptr[i] = 100 + i;
    }
    arr0.dprint();

    /* 2-d ptr access */
    float *const *arr0_2d_ptr = arr0.array_ptr_2d(true);
    for ( i=0 ; i < arr0.row_length() ; i++ ) {                 /* Y */
        for ( j=0 ; j < arr0.col_length() ; j++ ) {             /* X */
            arr0_2d_ptr[i][j] = 100 + 10*i + j;
        }
    }
    arr0.dprint();


    /* set length of 3-d array (x,y,z) */
    arr1.resize_3d(6,4,2);

    /* 3-d ptr access */
    double *const *const *arr1_3d_ptr = arr1.array_ptr_3d(true);
    for ( i=0 ; i < arr1.layer_length() ; i++ ) {               /* Z */
        for ( j=0 ; j < arr1.row_length() ; j++ ) {             /* Y */
            for ( k=0 ; k < arr1.col_length() ; k++ ) {         /* X */
                arr1_3d_ptr[i][j][k] = 1000 + 100*i + 10*j + k;
            }
        }
    }
    arr1.dprint();

    return 0;
}
