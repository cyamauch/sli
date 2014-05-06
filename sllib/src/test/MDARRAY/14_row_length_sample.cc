/**
  example code
  mdarrayクラス: size_t row_length() const
                 配列の行の長さ
*/
#include <sli/stdstreamio.h>
#include <sli/mdarray.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;
    size_t naxixs[4] = {6,5,4,3};
    mdarray my_mdarr3dim(SHORT_ZT, true, naxixs, 4, true);

    sio.printf("start\n");

    sio.printf("*** my_mdarr3dim dim... [%zu]\n", my_mdarr3dim.dim_length());
    sio.printf("*** my_mdarr3dim length... [%zu]\n", my_mdarr3dim.length());
    sio.printf("*** my_mdarr3dim byte_length... [%zu]\n", my_mdarr3dim.byte_length());
    sio.printf("*** my_mdarr3dim byte_length 1dim... [%zu]\n", my_mdarr3dim.byte_length(0));
    sio.printf("*** my_mdarr3dim byte_length 2dim... [%zu]\n", my_mdarr3dim.byte_length(1));
    sio.printf("*** my_mdarr3dim byte_length 3dim... [%zu]\n", my_mdarr3dim.byte_length(2));
    sio.printf("*** my_mdarr3dim byte_length 4dim... [%zu]\n", my_mdarr3dim.byte_length(3));
    sio.printf("*** my_mdarr3dim byte_length 5dim... [%zu]\n", my_mdarr3dim.byte_length(4));

    sio.printf("*** my_mdarr3dim size_type... [%zd]\n", my_mdarr3dim.size_type());
    sio.printf("*** my_mdarr3dim col... [%zu]\n", my_mdarr3dim.col_length());
    sio.printf("*** my_mdarr3dim row... [%zu]\n", my_mdarr3dim.row_length());
    sio.printf("*** my_mdarr3dim layer... [%zu]\n", my_mdarr3dim.layer_length());

    sio.printf("end\n");

    return 0;
}
