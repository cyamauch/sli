/**
  example code
  mdarrayクラス: mdarray &operator=(double v)
                 1要素のバイト数
*/
#include <sli/stdstreamio.h>
#include <sli/mdarray.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;
    mdarray my_mdarr(DOUBLE_ZT, true);
    mdarray mdarrld(LDOUBLE_ZT, true);

    sio.printf("start\n");

    sio.printf("*** my_mdarr bytes... [%zu]\n", my_mdarr.bytes());

    if ( mdarrld.bytes() == sizeof(long double) ) {
	sio.printf("*** mdarrld bytes... OK.\n");
    } else {
	sio.printf("*** mdarrld bytes... NG!\n");
    }

    sio.printf("end\n");

    return 0;
}
