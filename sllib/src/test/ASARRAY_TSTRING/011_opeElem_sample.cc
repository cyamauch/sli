/**
  example code
  asarray_tstring�N���X:tstring &operator[]( const char *key )
                        �L�[�ɑΉ�����A�z�z��̒l���Q��
*/
#include <sli/stdstreamio.h>
#include <sli/asarray_tstring.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;

    asarray_tstring my_asarr;
    //my_asarr�ɒl��ݒ�
    my_asarr["google"]  = "Larry Page";
    my_asarr["YouTube"]   = "Steve Chen";

    sio.printf("YouTube ... [%s]\n", my_asarr["YouTube"].cstr());

    //�l�Ȃ��ŃL�[�̂ݐݒ肷��ƁA�l��""�ŕԂ�
    my_asarr["Yahoo"];
    sio.printf("Yahoo ... [%s]\n", my_asarr["Yahoo"].cstr());

    return 0;
}

