/**
  example code
  ctindexクラス:ctindex &init()
                ctindex &init(const ctindex &obj)
                オブジェクトの完全初期化
*/
#include <sli/stdstreamio.h>
#include <sli/ctindex.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;

    ctindex my_idx;
    ctindex my_idx1;

    my_idx.append("Morning glory",0);
    my_idx.append("Gentiana",1);

    //my_idx1をmy_idxで初期化
    my_idx1.init(my_idx);
    sio.printf("my_idx1.index... [%zd]\n", my_idx1.index("Morning glory"));
    sio.printf("my_idx1.index... [%zd]\n", my_idx1.index("Gentiana"));

    sio.printf("my_idx.index... [%zd]\n", my_idx.index("Morning glory"));
    sio.printf("my_idx.index... [%zd]\n", my_idx.index("Gentiana"));

    return 0;
}

