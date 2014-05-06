/**
  example code
  mdarrayクラス: mdarray &subtract( const mdarray &src_img,
	  			    ssize_t dest_col = 0, ssize_t dest_row = 0, ssize_t dest_layer = 0 );
                 配列オブジェクトの減算
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
    mdarray mysubtract_mdarr(FLOAT_ZT, true);
    mysubtract_mdarr.resize(nx,2,true);

    float my_float[] = {1000, 2000, 3000, 4000};
    my_fmdarr.putdata((const void *)my_float, sizeof(my_float));
    float mysubt_float[] = {100, 200, 300, 400};
    mysubtract_mdarr.putdata((const void *)mysubt_float, sizeof(mysubt_float));

    for (size_t j = 0; j < my_fmdarr.length(1); j++){
      for (size_t i = 0; i < my_fmdarr.length(0); i++){
        sio.printf("my_fmdarr value(%zu,%zu)... [%g]\n", i, j, my_fmdarr.f(i, j));
      }
    }
    my_fmdarr.subtract(mysubtract_mdarr);
    for (size_t j = 0; j < my_fmdarr.length(1); j++){
      for (size_t i = 0; i < my_fmdarr.length(0); i++){
        sio.printf("=> my_fmdarr value(%zu,%zu)... [%g]\n", i, j, my_fmdarr.f(i, j));
      }
    }

    return 0;
}
