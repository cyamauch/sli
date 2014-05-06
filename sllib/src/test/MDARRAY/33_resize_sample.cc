/**
  example code
  mdarrayクラス: mdarray &resize( size_t len )
                 mdarray &resize( size_t dim_index, size_t len )
                 配列の長さを変更
*/
#include <sli/stdstreamio.h>
#include <sli/mdarray.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;
    size_t nx[] = {2, 2};
    mdarray my_cmdarr(UCHAR_ZT, true);

    my_cmdarr.resize(nx, 2, true);
    my_cmdarr.c(0,0) = 70;
    my_cmdarr.c(1,0) = 71;
    my_cmdarr.c(0,1) = 36;
    my_cmdarr.c(1,1) = 37;
    for (size_t j = 0; j < my_cmdarr.length(1); j++){
        for (size_t i = 0; i < my_cmdarr.length(0); i++){
            sio.printf("my_cmdarr value(%zu,%zu)... [%hhu]\n", i, j, my_cmdarr.c(i, j));
        }
    }

    my_cmdarr.resizef(",%d",3);
    //resize後の値を出力
    for (size_t j = 0; j < my_cmdarr.length(1); j++){
        for (size_t i = 0; i < my_cmdarr.length(0); i++){
            sio.printf("=> my_cmdarr value(%zu,%zu)... [%hhu]\n", i, j, my_cmdarr.c(i, j));
        }
    }

    return 0;
}
