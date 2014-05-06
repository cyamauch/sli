/**
  example code
  mdarray���饹: long lvalue( ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF, 
		              ssize_t idx2 = MDARRAY_INDEF ) const
                 long long llvalue( ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF, 
			            ssize_t idx2 = MDARRAY_INDEF ) const
                 long������long long�����Ѵ��������Ǥ���
*/
#include <sli/stdstreamio.h>
#include <sli/mdarray.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;
    mdarray my_mdarry(FLOAT_ZT, true);

    sio.printf("start\n");

    my_mdarry.f(0) = 123.556;
    //float���ǽ���
    sio.printf("my_mdarry f... [%g]\n", my_mdarry.f(0));
    //long���ǽ��� �������ʲ��ڤ�Τ�
    sio.printf("my_mdarry lvalue... [%ld]\n", my_mdarry.lvalue(0));
    //long long���ǽ��� �������ʲ��ڤ�Τ�
    sio.printf("my_mdarry llvalue... [%lld]\n", my_mdarry.llvalue(0));

    //�ͼθ����Ĥ�����
    my_mdarry.set_rounding(true);
    //long���ǽ��� �������ʲ��ͼθ���
    sio.printf("my_mdarry lvalue... [%ld]\n", my_mdarry.lvalue(0));
    //long long���ǽ��� �������ʲ��ͼθ���
    sio.printf("my_mdarry llvalue... [%lld]\n", my_mdarry.llvalue(0));

    sio.printf("end\n");

    return 0;
}
