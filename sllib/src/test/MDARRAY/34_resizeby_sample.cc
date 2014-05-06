/**
  example code
  mdarrayクラス: mdarray &resizeby( ssize_t len )
    		 mdarray &resizeby( size_t dim_index, ssize_t len )
                 配列の長さを変更
*/
#include <sli/stdstreamio.h>
#include <sli/mdarray.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;
    mdarray my_cmdarr(UCHAR_ZT, true);
    my_cmdarr.resize(3);

    for (size_t i = 0; i < my_cmdarr.length(0); i++){
        sio.printf("my_cmdarr value(%zu)... [%hhu]\n", i, my_cmdarr.c(i));
    }

    sio.printf("*** my_cmdarr resizeby: -2 *** \n");
    my_cmdarr.resizebyf("%d",-2);
    //resizeby(減少)後の値を出力
    for (size_t i = 0; i < my_cmdarr.length(0); i++){
        sio.printf("=> my_cmdarr value(%zu)... [%hhu]\n", i, my_cmdarr.c(i));
    }

    sio.printf("*** my_cmdarr resizeby: +1 *** \n");
    my_cmdarr.resizeby( 1 );
    //resizeby(増加)後の値を出力
    for (size_t i = 0; i < my_cmdarr.length(0); i++){
        sio.printf("==> my_cmdarr value(%zu)... [%hhd]\n", i, my_cmdarr.c_cs(i));
    }

    return 0;
}
