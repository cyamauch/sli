/**
  example code
  mdarrayクラス: mdarray &ceil();
                 浮動小数点の値を切り上げ
*/
#include <sli/stdstreamio.h>
#include <sli/mdarray.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;
    mdarray my_fmdarr(FLOAT_ZT, true);

    my_fmdarr.f(0) = 1000.1;
    my_fmdarr.f(1) = 2000.6;

    for (size_t i = 0; i < my_fmdarr.length(); i++){
        sio.printf("my_fmdarr value(%zu)... [%g]\n", i, my_fmdarr.f(i));
    }
    my_fmdarr.ceil();
    //切り上げ後の値を出力
    for (size_t i = 0; i < my_fmdarr.length(); i++){
        sio.printf("=> my_fmdarr value(%zu)... [%g]\n", i, my_fmdarr.f(i));
    }

    return 0;
}
