/**
  example code
  mdarrayクラス: mdarray &add( const mdarray &src_img,
	  		       ssize_t dest_col = 0, ssize_t dest_row = 0, ssize_t dest_layer = 0 );
                 配列オブジェクトの加算(画像向き)
*/
#include <sli/stdstreamio.h>
#include <sli/mdarray.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;
    size_t nx[] = {2, 2};
    mdarray my_smdarr(SHORT_ZT, true);
    my_smdarr.resize(nx,2,true);
    short my_short[] = {1, 2, 3, 4};
    my_smdarr.putdata((const void *)my_short, sizeof(my_short));

    mdarray myadd_smdarr(SHORT_ZT, true);
    myadd_smdarr.resize(nx,2,true);
    short myadd_short[] = {9, 8, 7, 6};
    myadd_smdarr.putdata((const void *)myadd_short, sizeof(myadd_short));

    for (size_t j = 0; j < my_smdarr.length(1); j++){
      for (size_t i = 0; i < my_smdarr.length(0); i++){
        sio.printf("my_smdarr value(%zu,%zu)... [%hd]\n", i, j, my_smdarr.s(i, j));
      }
    }
    my_smdarr.add(myadd_smdarr);
    for (size_t j = 0; j < my_smdarr.length(1); j++){
      for (size_t i = 0; i < my_smdarr.length(0); i++){
        sio.printf("=> my_smdarr value(%zu,%zu)... [%hd]\n", i, j, my_smdarr.s(i, j));
      }
    }
    my_smdarr.addf(myadd_smdarr,"*");
    for (size_t j = 0; j < my_smdarr.length(1); j++){
      for (size_t i = 0; i < my_smdarr.length(0); i++){
        sio.printf("==> my_smdarr value(%zu,%zu)... [%hd]\n", i, j, my_smdarr.s(i, j));
      }
    }

    return 0;
}
