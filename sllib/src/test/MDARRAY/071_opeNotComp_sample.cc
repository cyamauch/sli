/**
  example code
  mdarray¥¯¥é¥¹: bool operator!=(const mdarray &obj) const
                 Èæ³Ó(ÈÝÄê·Á)
*/
#include <sli/stdstreamio.h>
#include <sli/mdarray.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;
    mdarray my_mdarr(UCHAR_ZT, true);
    mdarray comp_mdarr(LONG_ZT, true);

    sio.printf("start\n");

    my_mdarr.resize(3);
    my_mdarr = 20;

    comp_mdarr = 20;

    if ( my_mdarr != comp_mdarr ) {
        sio.printf("true\n");
    } else {
        sio.printf("false\n");
    }

    sio.printf("end\n");

    return 0;
}
