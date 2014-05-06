/**
  example code
  mdarray���饹: mdarray &trunc()
                 ��ư���������ͤ��ڤ�Τ�
*/
#include <sli/stdstreamio.h>
#include <sli/mdarray.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;
    mdarray my_fmdarr(FLOAT_ZT, true);

    my_fmdarr.f(0) = 1.7;
    my_fmdarr.f(1) = -1.7;

    for (size_t i = 0; i < my_fmdarr.length(); i++){
        sio.printf("my_fmdarr value(%zu)... [%g]\n", i, my_fmdarr.f(i));
    }
    my_fmdarr.trunc();
    //�ڤ�ΤƸ���ͤ����
    for (size_t i = 0; i < my_fmdarr.length(); i++){
        sio.printf("=> my_fmdarr value(%zu)... [%g]\n", i, my_fmdarr.f(i));
    }

    return 0;
}
