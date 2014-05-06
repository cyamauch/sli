/**
  example code
  mdarray���饹:  mdarray &round()
                  ��ư���������ͤ�ͼθ���
*/
#include <sli/stdstreamio.h>
#include <sli/mdarray.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;
    mdarray my_fmdarr(FLOAT_ZT, true);

    my_fmdarr.f(0) = 1000.5;
    my_fmdarr.f(1) = -1000.5;

    for (size_t i = 0; i < my_fmdarr.length(0); i++){
        sio.printf("my_fmdarr value[%zu]... [%g]\n", i, my_fmdarr.f(i));
    }
    my_fmdarr.round();
    //�ͼθ�������ͤ����
    for (size_t i = 0; i < my_fmdarr.length(0); i++){
        sio.printf("=> my_fmdarr value[%zu]... [%g]\n", i, my_fmdarr.f(i));
    }

   return 0;
}
