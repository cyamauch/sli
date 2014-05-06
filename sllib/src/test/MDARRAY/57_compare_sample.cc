/**
  example code
  mdarrayクラス: bool compare(const mdarray &obj) const
                 配列オブジェクトの比較
*/
#include <sli/stdstreamio.h>
#include <sli/mdarray.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;
    size_t nx[] = {2, 2};
    mdarray my_fmdarr(FLOAT_ZT, true);
    my_fmdarr.resize(nx,2,true);
    my_fmdarr.f(0,0) = 1000;

    mdarray my_i64mdarr(INT64_ZT, true);
    my_i64mdarr.resize(nx,2,true);
    my_i64mdarr.i64(0,0) = 1000;

    sio.printf("*** my_fmdarr compare [%d] *** \n", (int)my_fmdarr.compare(my_i64mdarr));

    return 0;
}
