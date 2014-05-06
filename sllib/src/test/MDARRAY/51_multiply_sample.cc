/**
  example code
  mdarrayクラス: mdarray &multiply( double value, 
		 		    ssize_t col_index = 0, size_t col_size = MDARRAY_ALL,
				    ssize_t row_index = 0, size_t row_size = MDARRAY_ALL,
				    ssize_t layer_index = 0, size_t layer_size = MDARRAY_ALL );
                 値の乗算
*/
#include <sli/stdstreamio.h>
#include <sli/mdarray.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;
    size_t nx[] = {2, 2};
    mdarray my_fmdarr(FLOAT_ZT, true);
    my_fmdarr.resize(nx,2,true);
    float my_float[] = {1, 3, 2, 4};
    my_fmdarr.putdata((const void *)my_float, sizeof(my_float));

    my_fmdarr.multiply(50);
    for (size_t j = 0; j < my_fmdarr.length(1); j++){
      for (size_t i = 0; i < my_fmdarr.length(0); i++){
        sio.printf("my_fmdarr value(%zu,%zu)... [%g]\n", i, j, my_fmdarr.f(i, j));
      }
    }

    return 0;
}
