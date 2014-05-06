/**
  example code
  ctindexクラス:int append( const char *key, size_t index )
                一対のキーとインデックスの追加
*/
#include <sli/stdstreamio.h>
#include <sli/ctindex.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;

    ctindex my_idx;

    //my_idxにキーとインデックスを追加
    my_idx.append("Pansy",2);
    my_idx.append("Violet",4);
    sio.printf("my_idx.index... [%zd]\n", my_idx.index("Pansy"));
    sio.printf("my_idx.index... [%zd]\n", my_idx.index("Violet"));

    //エラー確認用コード：同一組み合わせはエラーとなる
    /*
    int ret = my_idx.append("Violet",4);
    sio.printf("ret... [%d]\n", ret);
    */

    return 0;
}

