/**
  example code
  mdarrayクラス: ssize_t copy( mdarray &dest ) const
                 別オブジェクトへのコピー
*/
#include <sli/stdstreamio.h>
#include <sli/mdarray.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;
    mdarray my_fmdarr(FLOAT_ZT, true);

    size_t nx[] = {2, 2};
    mdarray my_cmdarr(UCHAR_ZT, true);
    my_cmdarr.resize(nx,2,true);

    unsigned char my_char[] = {98, 99, 101, 102};
    my_cmdarr.putdata((const void *)my_char, sizeof(my_char));

    my_cmdarr.copy( &my_fmdarr );
    sio.printf("size... [%zd]\n", my_fmdarr.size_type());
    //copy後の値を出力

    for (size_t j = 0; j < my_fmdarr.length(1); j++){
        for (size_t i = 0; i < my_fmdarr.length(0); i++){
            sio.printf("my_fmdarr value(%zu,%zu)... [%hhu]\n", i, j, my_fmdarr.c(i, j));
        }
    }

    return 0;
}
