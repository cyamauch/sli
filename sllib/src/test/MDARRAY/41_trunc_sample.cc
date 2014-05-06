/**
  example code
  mdarrayクラス: mdarray &trunc()
                 浮動小数点の値を切り捨て
*/
#include <sli/stdstreamio.h>
#include <sli/mdarray.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;
    mdarray my_fmdarr(FLOAT_ZT, true);

    my_fmdarr.f(0) = 1.7;
    my_fmdarr.f(1) = -1.7;

    for (size_t i = 0; i < my_fmdarr.length(); i++){
        sio.printf("my_fmdarr value(%zu)... [%g]\n", i, my_fmdarr.f(i));
    }
    my_fmdarr.trunc();
    //切り捨て後の値を出力
    for (size_t i = 0; i < my_fmdarr.length(); i++){
        sio.printf("=> my_fmdarr value(%zu)... [%g]\n", i, my_fmdarr.f(i));
    }

    return 0;
}
