/**
  example code
  mdarray���饹: virtual mdarray &increase_dim()
                 ���󥪥֥������Ȥμ������γ�ĥ
*/
#include <sli/stdstreamio.h>
#include <sli/mdarray.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;
    size_t nx[] = {1, 2, 3};
    mdarray my_mdarr(UCHAR_ZT, true);

    my_mdarr.resize(nx,3,true);
    sio.printf("my_mdarr dim... [%zu]\n",  my_mdarr.dim_length());

    my_mdarr.increase_dim();
    //�����������
    sio.printf("=> my_mdarr dim... [%zu]\n",  my_mdarr.dim_length());

    return 0;
}
