/**
  example code
  mdarrayクラス: ssize_t putdata( const void *src_buf, size_t buf_size,
		 		  ssize_t idx0 = 0,
		  		  ssize_t idx1 = MDARRAY_INDEF, ssize_t idx2 = MDARRAY_INDEF )
                 ユーザバッファから配列オブジェクトへのコピー
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

    float my_float[] = {1000.3, 2000.2, 3000.6, 4000.5};
    my_fmdarr.putdata((const void *)my_float, sizeof(my_float));

    for (size_t j = 0; j < my_fmdarr.length(1); j++){
      for (size_t i = 0; i < my_fmdarr.length(0); i++){
        sio.printf("my_fmdarr value(%zu,%zu)... [%g]\n", i, j, my_fmdarr.f(i, j));
      }
    }

    return 0;
}
