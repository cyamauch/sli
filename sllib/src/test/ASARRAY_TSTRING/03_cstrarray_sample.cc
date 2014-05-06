/**
  example code
  asarray_tstring�N���X:const char *const *cstrarray() const
                        �A�z�z��̒l�̃|�C���^�z��
*/
#include <sli/stdstreamio.h>
#include <sli/asarray_tstring.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;

    asarray_tstring my_asarr;
    my_asarr["MIT"] = "cambridge";
    my_asarr["Princeton"] = "New Jersey";
    my_asarr["Berkeley"] = "California";

    const char *const *my_ptr;
    //my_asarr�̒l�̃|�C���^�z����擾
    my_ptr = my_asarr.cstrarray();
    //�|�C���^�z��̒l���o��
    if (my_ptr != NULL){
      for (int i = 0; my_ptr[i] != NULL; i++){
        sio.printf("%d ... [%s]\n", i, my_ptr[i]);
      }
    }

    return 0;
}

