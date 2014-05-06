/**
  example code
  mdarrayクラス: size_t dim_length() const
                 配列の次元数
*/
#include <sli/stdstreamio.h>
#include <sli/mdarray.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;
    mdarray my_mdarr3dim(FLOAT_ZT, true);
    size_t nx[] = {3, 4, 5};

    my_mdarr3dim.resize(nx, 3, true);

    sio.printf("start\n");

    sio.printf("*** my_mdarr3dim dim... [%zu]\n", my_mdarr3dim.dim_length());
    sio.printf("*** my_mdarr3dim length... [%zu]\n", my_mdarr3dim.length());

    sio.printf("*** my_mdarr3dim size_type... [%zd]\n", my_mdarr3dim.size_type());
    sio.printf("*** my_mdarr3dim col... [%zu]\n", my_mdarr3dim.col_length());
    sio.printf("*** my_mdarr3dim row... [%zu]\n", my_mdarr3dim.row_length());
    sio.printf("*** my_mdarr3dim layer... [%zu]\n", my_mdarr3dim.layer_length());

    sio.printf("end\n");

    return 0;
}
