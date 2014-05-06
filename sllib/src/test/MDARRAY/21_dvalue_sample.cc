/**
  example code
  mdarray���饹: double dvalue( ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF, 
			        ssize_t idx2 = MDARRAY_INDEF ) const
                 double�����Ѵ��������Ǥ���
*/
#include <sli/stdstreamio.h>
#include <sli/mdarray.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;
    mdarray my_mdarry(FLOAT_ZT,true);

    sio.printf("start\n");

    my_mdarry.f(0) = 123.456;

    //[%f] �ȷ������ꤷ�ʤ���硢123.456001 �Ƚ��Ϥ���롣
    //sio.printf("my_mdarry f... [%f]\n", my_mdarry.f(0));
    sio.printf("my_mdarry dvalue... [%6.3f]\n", my_mdarry.dvalue(0));
    //���󥵥�����Ķ������ֵ��ͤ�NAN
    sio.printf("my_mdarry dvalue... [%6.3f]\n", my_mdarry.dvalue(2));

    return 0;
}
