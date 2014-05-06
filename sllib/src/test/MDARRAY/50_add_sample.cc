/**
  example code
  mdarrayクラス: mdarray &add( double value, 
		 	       ssize_t col_index = 0, size_t col_size = MDARRAY_ALL,
		               ssize_t row_index = 0, size_t row_size = MDARRAY_ALL,
		               ssize_t layer_index = 0, size_t layer_size = MDARRAY_ALL )
                 値の加算
*/
#include <sli/stdstreamio.h>
#include <sli/mdarray.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;
    size_t nx[] = {4, 3};
    mdarray my_smdarr(SHORT_ZT, true);
    short my_short[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
    size_t i, j;

    sio.printf("*** BEGIN: 50_add_sample ***\n");

    my_smdarr.resize(nx,2,true);
    my_smdarr.putdata((const void *)my_short, sizeof(my_short));

    sio.printf("before:\n");
    for ( j = 0 ; j < my_smdarr.length(1) ; j++ ) {
	for ( i = 0 ; i < my_smdarr.length(0) ; i++ ) {
	    sio.printf("[%02hd]", my_smdarr.s(i, j));
	}
	sio.printf("\n");
    }

    my_smdarr.set_rounding(false);
    my_smdarr.add(1.5);

    my_smdarr.set_rounding(true);
    my_smdarr.addf(9.5, "0:1, 1:2");    // same as my_smdarr.add(9.5, 0,2,1,2);

    sio.printf("after:\n");
    for ( j = 0 ; j < my_smdarr.length(1) ; j++ ) {
	for ( i = 0 ; i < my_smdarr.length(0) ; i++ ) {
	    sio.printf("[%02hd]", my_smdarr.s(i, j));
	}
	sio.printf("\n");
    }

    sio.printf("*** END: 50_add_sample ***\n");

    return 0;
}
