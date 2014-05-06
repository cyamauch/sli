/**
  example code
  mdarrayクラス: ssize_t size_type() const
                 型を表す数値
*/
#include <sli/stdstreamio.h>
#include <sli/mdarray.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;
    mdarray my_mdarr(INT32_ZT, true);

    sio.printf("start\n");

    /* 型を表す数値を出力 */
    sio.printf("*** my_mdarr size_type... [%zd]\n", my_mdarr.size_type());

    sio.printf("end\n");

    return 0;
}

