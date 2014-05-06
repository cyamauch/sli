/**
  example code
  mdarrayクラス: mdarray &crop( ssize_t idx, size_t len )
    		 mdarray &crop( size_t dim_index, ssize_t idx, size_t len )
                 配列オブジェクトから要素の切り出し
*/
#include <sli/stdstreamio.h>
#include <sli/mdarray.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;
    size_t nx[] = {2, 3};
    mdarray my_cmdarr(UCHAR_ZT, true);
    my_cmdarr.resize(nx,2,true);
    my_cmdarr.c(0,0) = 124;
    my_cmdarr.c(1,0) = 125;
    my_cmdarr.c(0,1) = 126;
    my_cmdarr.c(1,1) = 127;
    my_cmdarr.crop( 0, 1, 1 );
    //crop後の値を出力
    for (size_t j = 0; j < my_cmdarr.length(1); j++){
        for (size_t i = 0; i < my_cmdarr.length(0); i++){
            sio.printf("my_cmdarr value(%zu,%zu)... [%hhu]\n", i, j, my_cmdarr.c(i, j));
        }
    }

    return 0;
}
