/**
  example code
  mdarrayクラス: mdarray &cpy( ssize_t idx_src, size_t len,
			       ssize_t idx_dst,	bool clr )
    		 mdarray &cpy( size_t dim_index, ssize_t idx_src,
                               size_t len, ssize_t idx_dst, bool clr )
                 配列オブジェクトの要素間の値のコピー
*/
#include <sli/stdstreamio.h>
#include <sli/mdarray.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;
    mdarray my_lmdarr(LLONG_ZT, true);
    my_lmdarr.ll(0) = -2147483646;
    my_lmdarr.ll(1) = 2147483647;
    my_lmdarr.cpy(1, 1, 2, false);
    //コピー後の値を出力
    for (size_t i = 0; i < my_lmdarr.length(0); i++) {
       sio.printf("my_lmdarr value(%zu)... [%lld]\n", i, my_lmdarr.ll(i));
    }

    sio.printf("end\n");

    return 0;
}
