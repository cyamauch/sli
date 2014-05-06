/**
  example code
  ctindex�N���X:int append( const char *key, size_t index )
                ��΂̃L�[�ƃC���f�b�N�X�̒ǉ�
*/
#include <sli/stdstreamio.h>
#include <sli/ctindex.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;

    ctindex my_idx;

    //my_idx�ɃL�[�ƃC���f�b�N�X��ǉ�
    my_idx.append("Pansy",2);
    my_idx.append("Violet",4);
    sio.printf("my_idx.index... [%zd]\n", my_idx.index("Pansy"));
    sio.printf("my_idx.index... [%zd]\n", my_idx.index("Violet"));

    //�G���[�m�F�p�R�[�h�F����g�ݍ��킹�̓G���[�ƂȂ�
    /*
    int ret = my_idx.append("Violet",4);
    sio.printf("ret... [%d]\n", ret);
    */

    return 0;
}

