/**
  example code
  ctindexクラス:ctindex &operator=(const ctindex &obj)
                ctindexオブジェクトを代入
*/

#include <sli/stdstreamio.h>
#include <sli/ctindex.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;

    ctindex my_idx;
    ctindex my_idx1;

    my_idx.append("rose",0);
    my_idx.append("cosmos",1);

    //my_idx1にmy_idxを代入
    my_idx1=my_idx;
    sio.printf("my_idx1.index... [%zd]\n", my_idx1.index("rose"));
    sio.printf("my_idx1.index... [%zd]\n", my_idx1.index("cosmos"));

    sio.printf("my_idx.index... [%zd]\n", my_idx.index("rose"));
    sio.printf("my_idx.index... [%zd]\n", my_idx.index("cosmos"));

    return 0;
}

