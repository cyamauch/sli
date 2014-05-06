/**
  example code
  mdarrayクラス: mdarray &swap( mdarray &sobj )
                 別オブジェクトの内容の入れ替え
*/
#include <sli/stdstreamio.h>
#include <sli/mdarray.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;

    mdarray my_fmdarr(FLOAT_ZT,true);
    my_fmdarr.resize(2);
    my_fmdarr.f(0) = 1000;
    my_fmdarr.f(1) = 2000;

    mdarray swapf_mdarr(DOUBLE_ZT,true);
    swapf_mdarr.resize(2);
    swapf_mdarr.d(0) = 100;
    swapf_mdarr.d(1) = 200;

    for (size_t i = 0; i < my_fmdarr.length(0); i++){
      sio.printf("my_fmdarr value(%zu)... [%g]\n", i, my_fmdarr.dvalue(i));
    }
    my_fmdarr.swap(swapf_mdarr);
    //swap後の値を出力
    for (size_t i = 0; i < my_fmdarr.length(0); i++){
      sio.printf("=> my_fmdarr value(%zu)... [%g]\n", i, my_fmdarr.dvalue(i));
    }

    return 0;
}
