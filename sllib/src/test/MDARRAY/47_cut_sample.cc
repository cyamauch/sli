/**
  example code
  mdarrayクラス: mdarray &cut( mdarray &dest, 
		 	       ssize_t col_idx, size_t col_len=MDARRAY_ALL,
			       ssize_t row_idx=0, size_t row_len=MDARRAY_ALL, 
			       ssize_t layer_idx=0, size_t layer_len=MDARRAY_ALL )
                 配列オブジェクトから値の取り出し(画像向き)
*/
#include <sli/stdstreamio.h>
#include <sli/mdarray.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;
    mdarray my_mdarr;

    size_t nx[] = {2, 2};
    mdarray my_cmdarr(UCHAR_ZT);
    my_cmdarr.resize(nx,2,true);
    unsigned char my_char[] = {51, 101, 52, 102};
    my_cmdarr.putdata((const void *)my_char, sizeof(my_char));

    my_cmdarr.cut( my_mdarr, 0, 1 );

    for (size_t j = 0; j < my_cmdarr.length(1); j++){
        for (size_t i = 0; i < my_cmdarr.length(0); i++){
            sio.printf("my_cmdarr value(%zu,%zu)... [%hhu]\n", i, j, my_cmdarr.c(i, j));
        }
    }

    return 0;
}

