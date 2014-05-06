/**
  example code
  mdarrayクラス: virtual mdarray &increase_dim()
                 配列オブジェクトの次元数の拡張
*/
#include <sli/stdstreamio.h>
#include <sli/mdarray.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;
    size_t nx[] = {1, 2, 3};
    mdarray my_mdarr(UCHAR_ZT, true);

    my_mdarr.resize(nx,3,true);
    sio.printf("my_mdarr dim... [%zu]\n",  my_mdarr.dim_length());

    my_mdarr.increase_dim();
    //次元数を出力
    sio.printf("=> my_mdarr dim... [%zu]\n",  my_mdarr.dim_length());

    return 0;
}
