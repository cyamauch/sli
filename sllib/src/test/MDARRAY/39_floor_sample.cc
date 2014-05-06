/**
  example code
  mdarrayクラス: mdarray &floor()
                 浮動小数点の値を切り下げ
*/
#include <sli/stdstreamio.h>
#include <sli/mdarray.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;
    mdarray my_fmdarr(FLOAT_ZT, true);

    my_fmdarr.f(0) = 1000.1;
    my_fmdarr.f(1) = 2000.9;

    for (size_t i = 0; i < my_fmdarr.length(0); i++){
        sio.printf("my_fmdarr value(%zu)... [%g]\n", i, my_fmdarr.f(i));
    }
    my_fmdarr.floor();
    //切り下げ後の値を出力
    for (size_t i = 0; i < my_fmdarr.length(0); i++){
        sio.printf("=> my_fmdarr value(%zu)... [%g]\n", i, my_fmdarr.f(i));
    }

    return 0;
}
