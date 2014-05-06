/**
  example code
  tarray_tstring�N���X:tstring &at( size_t index )
		       const tstring &at_cs( size_t index ) const
                       �w��v�f�ʒu�̕�����Q��
*/
#include <sli/stdstreamio.h>
#include <sli/tarray_tstring.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;

    tarray_tstring my_arr;
    //my_arr[0] = "Hello"; �Ɠ���
    my_arr.at(0) = "Hello";
    sio.printf("my_arr[0] = %s\n",my_arr.at(0).cstr());
    //�����I�ɑ�����Ă��Ȃ��ꍇ�̊m�F
    sio.printf("my_arr[1] = %s\n",my_arr.at(1).cstr());

    sio.printf("***********************\n");
    //�G���[�m�F�p�R�[�h�Fat_cs()�ł̔z�񒷂𒴂��Ă̎Q�Ƃ̓G���[
    /*
    try{
       sio.printf("my_arr[2] = %s\n",my_arr.at_cs(2).cstr());
    }catch ( err_rec msg ) {
      sio.eprintf("[EXCEPTION] function=[%s::%s] message=[%s]\n",
                  msg.class_name, msg.func_name, msg.message);
      return -1;
    }
    */

    return 0;
}

