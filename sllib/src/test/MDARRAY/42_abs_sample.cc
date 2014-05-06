/**
  example code
  mdarrayクラス: mdarray &abs()
                 要素の値の絶対値
*/
#include <sli/stdstreamio.h>
#include <sli/mdarray.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;
    mdarray my_fmdarr(FLOAT_ZT, true);

    my_fmdarr.f(0) = -1000.1;
    my_fmdarr.f(1) = -2000.6;

    for (size_t i = 0; i < my_fmdarr.length(0); i++){
        sio.printf("my_fmdarr value(%zu)... [%5.1f]\n", i, my_fmdarr.f(i));
    }
    my_fmdarr.abs();
    //絶対値を出力
    for (size_t i = 0; i < my_fmdarr.length(0); i++){
        sio.printf("=> my_fmdarr value(%zu)... [%5.1f]\n", i, my_fmdarr.f(i));
    }

    return 0;
}
