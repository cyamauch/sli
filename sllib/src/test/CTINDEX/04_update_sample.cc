/**
  example code
  ctindex�N���X:int update( const char *key, size_t current_index, size_t new_index )
                �C���f�b�N�X�̍X�V
*/
#include <sli/stdstreamio.h>
#include <sli/ctindex.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;

    ctindex my_idx;

    my_idx.append("Pansy",2);
    my_idx.append("Violet",4);
    sio.printf("my_idx.index... [%zd]\n", my_idx.index("Pansy"));
    sio.printf("my_idx.index... [%zd]\n", my_idx.index("Violet"));

    //my_idx��"Pansy"�̃C���f�b�N�X��2����1�֍X�V
    my_idx.update("Pansy", 2, 1);
    sio.printf("my_idx.index... [%zd]\n", my_idx.index("Pansy"));
    sio.printf("my_idx.index... [%zd]\n", my_idx.index("Violet"));

    //�G���[�m�F�p�R�[�h�F�L�[��NULL�̏ꍇ�́A-1���Ԃ�܂�
    int ret = my_idx.update("", 2, 1);
    sio.printf("ret... [%d]\n", ret);
    return 0;
}

