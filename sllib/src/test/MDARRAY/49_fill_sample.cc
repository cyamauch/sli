/**
  example code
  mdarrayクラス: mdarray &fill_via_udf( double value, 
				void (*func_dest2d)(const void *,void *,void *), void *user_ptr_dest2d,
				void (*func_d2dest)(const void *,void *,void *), void *user_ptr_d2dest,
				double (*func)(double,double,ssize_t,ssize_t,ssize_t,mdarray *,void *),
			        void *user_ptr_func,
				ssize_t col_index = 0, size_t col_size = MDARRAY_ALL,
				ssize_t row_index = 0, size_t row_size = MDARRAY_ALL,
				ssize_t layer_index = 0, size_t layer_size = MDARRAY_ALL )
    		 mdarray &fill_via_udf( double value, 
				double (*func)(double,double,ssize_t,ssize_t,ssize_t,mdarray *,void *),
				void *user_ptr,
				ssize_t col_index = 0, size_t col_size = MDARRAY_ALL,
				ssize_t row_index = 0, size_t row_size = MDARRAY_ALL,
				ssize_t layer_index = 0, size_t layer_size = MDARRAY_ALL )
    		 darray &fill( double value, 
				ssize_t col_index = 0, size_t col_size = MDARRAY_ALL,
				ssize_t row_index = 0, size_t row_size = MDARRAY_ALL,
				 ssize_t layer_index = 0, size_t layer_size = MDARRAY_ALL )
                 値の書き換え(画像向き)
*/
#include <sli/stdstreamio.h>
#include <sli/mdarray.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;
    size_t nx[] = {2,2};
    mdarray my_smdarr(SHORT_ZT, true);
    size_t i, j;

    my_smdarr.resize(nx,2,true);
    my_smdarr.fill(100);

    for ( j=0 ; j < my_smdarr.length(1) ; j++ ) {
	for ( i=0 ; i < my_smdarr.length(0) ; i++ ) {
	    sio.printf("my_smdarr value(%zu,%zu).. [%hd]\n", 
		       i, j, my_smdarr.s(i, j));
	}
    }

    return 0;
}
