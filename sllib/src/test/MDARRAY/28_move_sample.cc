/**
  example code
  mdarrayクラス: mdarray &move( ssize_t idx_src, size_t len, ssize_t idx_dst,
			   bool clr )
                 mdarray &move( size_t dim_index, ssize_t idx_src, size_t len,
				ssize_t idx_dst, bool clr )
                 配列オブジェクトの要素間の値のコピー
*/
#include <sli/stdstreamio.h>
#include <sli/mdarray.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;
    mdarray my_cmdarr(UCHAR_ZT, true);
    my_cmdarr.resize(3);
    my_cmdarr.c(0) = 99;
    my_cmdarr.c(1) = 98;
    my_cmdarr.c(2) = 97;
    my_cmdarr.move( 2, 1, 0, true );
    //move後の値を出力
    for (size_t i = 0; i < my_cmdarr.length(); i++){
	sio.printf("my_cmdarr value(%zu)... [%hhu]\n", i, my_cmdarr.c(i));
    }

    return 0;
}
