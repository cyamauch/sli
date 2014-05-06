/**
  example code
  mdarray���饹: ssize_t size_type() const
                 ����ɽ������
*/
#include <sli/stdstreamio.h>
#include <sli/mdarray.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;
    mdarray my_mdarr(INT32_ZT, true);

    sio.printf("start\n");

    /* ����ɽ�����ͤ���� */
    sio.printf("*** my_mdarr size_type... [%zd]\n", my_mdarr.size_type());

    sio.printf("end\n");

    return 0;
}

