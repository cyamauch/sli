/**
  example code
  asarray_tstringクラス:const char *const *cstrarray() const
                        連想配列の値のポインタ配列
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
    //my_asarrの値のポインタ配列を取得
    my_ptr = my_asarr.cstrarray();
    //ポインタ配列の値を出力
    if (my_ptr != NULL){
      for (int i = 0; my_ptr[i] != NULL; i++){
        sio.printf("%d ... [%s]\n", i, my_ptr[i]);
      }
    }

    return 0;
}

