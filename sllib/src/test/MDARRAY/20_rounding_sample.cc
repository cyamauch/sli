/**
  example code
  mdarray���饹: bool rounding() const
                 �ͼθ����β��ݤμ���������
*/
#include <sli/stdstreamio.h>
#include <sli/mdarray.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;
    mdarray my_imdarr(LLONG_ZT, true);

    my_imdarr.assign(1.618, 0);
    /* ������֤ϡ��ͼθ������ʤ����Ȥ��ǧ */
    sio.printf("my_imdarr value(0)... [%lld]\n", my_imdarr.ll(0));

    /* �ͼθ�����Ԥ��褦���� */
    my_imdarr.set_rounding(true);
    my_imdarr.assign(1.618, 1);
    /* �ͼθ������Ԥ��뤫��ǧ */
    sio.printf("my_imdarr value(1)... [%lld]\n", my_imdarr.ll(1));

    mdarray my_mdarr;
    my_mdarr.init(my_imdarr);
    /* ������֤λͼθ������������� */
    sio.printf("my_mdarr rounding = [%d]\n", (int)my_mdarr.rounding());

    return 0;
}
