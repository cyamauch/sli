/**
  example code
  ctindexクラス:int erase( const char *key, size_t index )
                一対のキーとインデックスの削除
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
    sio.printf("my_idx.index... [%zd]\n", my_idx.index("Pansy"));
    sio.printf("my_idx.index... [%zd]\n", my_idx.index("Violet"));

    //my_idxの"Pansy"と2の組を削除
    int ret = my_idx.erase("Pansy", 2);
    sio.printf("my_idx.index... [%zd]\n", my_idx.index("Pansy"));
    sio.printf("ret... [%d]\n", ret);

    ret = my_idx.erase("Violet",4);
    sio.printf("ret... [%d]\n", ret);

    //削除後、my_idxは、組が存在しないので、retは0となります
    ret = my_idx.erase("Pansy",6);
    sio.printf("ret... [%d]\n", ret);

    return 0;
}

