/**
  example code
  ctindex�N���X:ctindex &init()
                ctindex &init(const ctindex &obj)
                �I�u�W�F�N�g�̊��S������
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

    //my_idx1��my_idx�ŏ�����
    my_idx1.init(my_idx);
    sio.printf("my_idx1.index... [%zd]\n", my_idx1.index("Morning glory"));
    sio.printf("my_idx1.index... [%zd]\n", my_idx1.index("Gentiana"));

    sio.printf("my_idx.index... [%zd]\n", my_idx.index("Morning glory"));
    sio.printf("my_idx.index... [%zd]\n", my_idx.index("Gentiana"));

    return 0;
}

