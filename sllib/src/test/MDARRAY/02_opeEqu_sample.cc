/**
  example code
  mdarrayクラス: mdarray &operator=(double v)
                 mdarrayオブジェクトへの代入
*/
#include <sli/stdstreamio.h>
#include <sli/mdarray.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;
    mdarray my_mdarr(UCHAR_ZT, true);
    size_t i;

    my_mdarr.resize(5);

    /* 値を代入 */
    my_mdarr = 125;

    /* 確認 */
    for ( i=0 ; i < my_mdarr.length() ; i++ ) {
        sio.printf("my_mdarr value(%zu)... [%hhu]\n", i, my_mdarr.c(i));
    }
    sio.printf("end\n");

    return 0;
}
