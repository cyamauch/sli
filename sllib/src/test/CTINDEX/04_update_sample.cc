/**
  example code
  ctindexクラス:int update( const char *key, size_t current_index, size_t new_index )
                インデックスの更新
*/
#include <sli/stdstreamio.h>
#include <sli/ctindex.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;

    ctindex my_idx;

    my_idx.append("Pansy",2);
    my_idx.append("Violet",4);
    sio.printf("my_idx.index... [%zd]\n", my_idx.index("Pansy"));
    sio.printf("my_idx.index... [%zd]\n", my_idx.index("Violet"));

    //my_idxの"Pansy"のインデックスを2から1へ更新
    my_idx.update("Pansy", 2, 1);
    sio.printf("my_idx.index... [%zd]\n", my_idx.index("Pansy"));
    sio.printf("my_idx.index... [%zd]\n", my_idx.index("Violet"));

    //エラー確認用コード：キーがNULLの場合は、-1が返ります
    int ret = my_idx.update("", 2, 1);
    sio.printf("ret... [%d]\n", ret);
    return 0;
}

