/**
  example code
  mdarrayクラス: mdarray &multiply( const mdarray &src_img,
	  		         　ssize_t dest_col = 0, ssize_t dest_row = 0, ssize_t dest_layer = 0 )
                 配列オブジェクトの乗算(画像向き)
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
    float my_float[] = {1, 2, 3, 4};
    my_fmdarr.putdata((const void *)my_float, sizeof(my_float));

    mdarray mymulti_fmdarr(FLOAT_ZT, true);
    mymulti_fmdarr.resize(nx,2,true);
    float mymulti_float[] = {10, 20, 30, 40};
    mymulti_fmdarr.putdata((const void *)mymulti_float, sizeof(mymulti_float));

    for (size_t j = 0; j < my_fmdarr.length(1); j++) {
      for (size_t i = 0; i < my_fmdarr.length(0); i++) {
        sio.printf("my_fmdarr value(%zu,%zu)... [%g]\n", i, j, my_fmdarr.f(i, j));
      }
    }
    my_fmdarr.multiply(mymulti_fmdarr);
    for (size_t j = 0; j < my_fmdarr.length(1); j++) {
      for (size_t i = 0; i < my_fmdarr.length(0); i++) {
        sio.printf("=> my_fmdarr value(%zu,%zu)... [%g]\n", i, j, my_fmdarr.f(i, j));
      }
    }

    return 0;
}
