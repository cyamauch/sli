/**
  example code
  mdarrayクラス: mdarray &divide( const mdarray &src_img,
	  			　ssize_t dest_col = 0, ssize_t dest_row = 0, ssize_t dest_layer = 0 )
                 配列オブジェクトの除算(画像向き)
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
    float my_float[] = {1000, 2000, 3000, 4000};
    my_fmdarr.putdata((const void *)my_float, sizeof(my_float));

    mdarray mydiv_mdarrf(FLOAT_ZT, true);
    mydiv_mdarrf.resize(nx,2,true);
    float mydiv_float[] = {2, 4, 3, 4};
    mydiv_mdarrf.putdata((const void *)mydiv_float, sizeof(mydiv_float));

    for (size_t j = 0; j < my_fmdarr.length(1); j++){
      for (size_t i = 0; i < my_fmdarr.length(0); i++){
        sio.printf("my_fmdarr value(%zu,%zu)... [%g]\n",
	    	    i, j, my_fmdarr.f(i, j));
      }
    }
    my_fmdarr.divide(mydiv_mdarrf);
    for (size_t j = 0; j < my_fmdarr.length(1); j++){
      for (size_t i = 0; i < my_fmdarr.length(0); i++){
        sio.printf("=> my_fmdarr value(%zu,%zu)... [%g]\n",
	    	    i, j, my_fmdarr.f(i, j));
      }
    }

    return 0;
}
