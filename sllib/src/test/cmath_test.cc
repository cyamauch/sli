#define SLI__USE_CMATH

#include <cmath>
#include <sli/stdstreamio.h>
#include <sli/mdarray.h>
#include <sli/mdarray_math.h>
#include <sli/mdarray_statistics.h>
#include <sli/complex.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;
    mdarray_double my_arr;
    size_t i, j;

    my_arr.resize_2d(8,4);

    for ( i=0 ; i < my_arr.row_length() ; i++ ) {
	for ( j=0 ; j < my_arr.col_length() ; j++ ) {
	    my_arr.assign(100 + 10 * i + j, j,i);
	}
    }

    for ( i=0 ; i < my_arr.row_length() ; i++ ) {
	for ( j=0 ; j < my_arr.col_length() ; j++ ) {
	    sio.printf("%g ",my_arr.dvalue(j,i));
	}
	sio.printf("\n");
    }

    my_arr.dprint();

    my_arr = log(my_arr);
    my_arr.dprint();

    sio.printf("median = %g\n", md_median(my_arr));

    return 0;
}
