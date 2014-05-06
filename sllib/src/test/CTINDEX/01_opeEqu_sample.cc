/**
  example code
  ctindex�N���X:ctindex &operator=(const ctindex &obj)
                ctindex�I�u�W�F�N�g����
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

    //my_idx1��my_idx����
    my_idx1=my_idx;
    sio.printf("my_idx1.index... [%zd]\n", my_idx1.index("rose"));
    sio.printf("my_idx1.index... [%zd]\n", my_idx1.index("cosmos"));

    sio.printf("my_idx.index... [%zd]\n", my_idx.index("rose"));
    sio.printf("my_idx.index... [%zd]\n", my_idx.index("cosmos"));

    return 0;
}

