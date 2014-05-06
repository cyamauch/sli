/**
  example code
  mdarray���饹: mdarray &ceil();
                 ��ư���������ͤ��ڤ�夲
*/
#include <sli/stdstreamio.h>
#include <sli/mdarray.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;
    mdarray my_fmdarr(FLOAT_ZT, true);

    my_fmdarr.f(0) = 1000.1;
    my_fmdarr.f(1) = 2000.6;

    for (size_t i = 0; i < my_fmdarr.length(); i++){
        sio.printf("my_fmdarr value(%zu)... [%g]\n", i, my_fmdarr.f(i));
    }
    my_fmdarr.ceil();
    //�ڤ�夲����ͤ����
    for (size_t i = 0; i < my_fmdarr.length(); i++){
        sio.printf("=> my_fmdarr value(%zu)... [%g]\n", i, my_fmdarr.f(i));
    }

    return 0;
}
