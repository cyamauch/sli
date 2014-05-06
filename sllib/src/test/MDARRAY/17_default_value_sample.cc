/**
  example code
  mdarray���饹: const void *default_value_ptr() const
                 ��������ĥ���ν���ͤμ���
*/
#include <sli/stdstreamio.h>
#include <sli/mdarray.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;
    mdarray my_mdarr;
    my_mdarr.init(FLOAT_ZT, true);

    sio.printf("start\n");

    my_mdarr.assign_default(50);

    /* default_value()��default_value_ptr()���ѹ� 091127 */
    /*
    sio.printf("*** my_mdarr defval... [%f]\n", 
	       *((const float *)my_mdarr.default_value()));
     */
    sio.printf("*** my_mdarr defval... [%g]\n", 
	       *((const float *)my_mdarr.default_value_ptr()));

    my_mdarr.resize(2);

    sio.printf("end\n");

    return 0;
}
