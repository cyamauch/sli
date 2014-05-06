/**
  example code
  mdarrayクラス: size_t layer_length() const
                 配列のレイヤ数
*/
#include <sli/stdstreamio.h>
#include <sli/mdarray.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;
    size_t nx3[] = {5, 4, 3};
    mdarray my_mdarr3dim(FLOAT_ZT, true);

    sio.printf("start\n");

    my_mdarr3dim.resize(nx3,3,true);

    sio.printf("*** my_mdarr3dim dim... [%zu]\n", my_mdarr3dim.dim_length());
    sio.printf("*** my_mdarr3dim byte_length... [%zu]\n", my_mdarr3dim.byte_length());
    sio.printf("*** my_mdarr3dim byte_length 1dim... [%zu]\n", my_mdarr3dim.byte_length(0));
    sio.printf("*** my_mdarr3dim byte_length 2dim... [%zu]\n", my_mdarr3dim.byte_length(1));
    sio.printf("*** my_mdarr3dim byte_length 3dim... [%zu]\n", my_mdarr3dim.byte_length(2));
    sio.printf("*** my_mdarr3dim byte_length 4dim... [%zu]\n", my_mdarr3dim.byte_length(3));
    sio.printf("*** my_mdarr3dim byte_length 5dim... [%zu]\n", my_mdarr3dim.byte_length(4));

    sio.printf("*** my_mdarr3dim size_type... [%zd]\n", my_mdarr3dim.size_type());

    /* レイヤ数を取得して、表示する */
    sio.printf("*** my_mdarr3dim layer... [%zu]\n", my_mdarr3dim.layer_length());

    sio.printf("*** my_mdarr3dim row... [%zu]\n", my_mdarr3dim.row_length());
    sio.printf("*** my_mdarr3dim col... [%zu]\n", my_mdarr3dim.col_length());

    size_t nx2[] = {3, 4};
    mdarray mdarr2dim(FLOAT_ZT, true);
    mdarr2dim.resize(nx2,2,true);
    sio.printf("*** mdarr2dim 2layer... [%zu]\n", mdarr2dim.layer_length());

    mdarray mdarr1dim(FLOAT_ZT, true);
    mdarr1dim.resize(3);
    sio.printf("*** mdarr1dim 1layer... [%zu]\n", mdarr1dim.layer_length());

    sio.printf("end\n");

    return 0;
}
