/**
  example code
  mdarrayクラス: mdarray &cut( mdarray &dest )
                 配列オブジェクトから値の取り出し取得
*/
#include <sli/stdstreamio.h>
#include <sli/mdarray.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;
    mdarray my_mdarr;

    size_t nx[] = {2, 2};
    mdarray my_cmdarr(UCHAR_ZT, true);
    my_cmdarr.resize(nx,2,true);
    unsigned char my_char[] = {51, 52, 101, 102};
    my_cmdarr.putdata((const void *)my_char, sizeof(my_char));

    sio.printf("my_cmdarr length()... [%zu]\n", my_cmdarr.length());
    sio.printf("my_mdarr length()... [%zu]\n", my_mdarr.length());

    my_cmdarr.cut( &my_mdarr );

    sio.printf("=> my_cmdarr length()... [%zu]\n", my_cmdarr.length());
    sio.printf("=> my_mdarr length()... [%zu]\n", my_mdarr.length());

    return 0;
}
