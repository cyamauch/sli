/**
  example code
  ctindexクラス:ssize_t index( const char *key, int index_of_index = 0 ) const
                インデックスの取得
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
    my_idx.append("Pansy",6);

    int i = 0 ;
    //"Pansy"のキー文字列が存在する間、インデックスを出力する
    while (my_idx.index("Pansy",i) > 0){
      sio.printf("my_idx.index[Pansy]... [%zd]\n",  my_idx.index("Pansy",i));
      i++;
    }

    //上のwhile文と同じ結果を得る
    sio.printf("my_idx.index... [%zd]\n", my_idx.index("Pansy"));
    sio.printf("my_idx.index... [%zd]\n", my_idx.index("Pansy",1));
    return 0;
}

