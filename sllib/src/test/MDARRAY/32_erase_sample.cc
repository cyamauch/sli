/**
  example code
  mdarray���饹: mdarray &erase( ssize_t idx, size_t len )
		 mdarray &erase( size_t dim_index, ssize_t idx, size_t len )
                 ���󥪥֥������Ȥ����Ǥκ��

*/
#include <sli/stdstreamio.h>
#include <sli/mdarray.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;
    mdarray my_mdarr(LLONG_ZT, true);

    my_mdarr.resize(3);

    my_mdarr.ll(0) = 0;
    my_mdarr.ll(1) = 2147483646;
    my_mdarr.ll(2) = 2147483647;

    for (size_t i = 0; i < my_mdarr.length(); i++){
        sio.printf("my_mdarr value(%zu)... [%lld]\n", i, my_mdarr.ll(i));
    }

    my_mdarr.erase( 1, 1 );
    //erase����ͤ����
    for (size_t i = 0; i < my_mdarr.length(); i++){
        sio.printf("=> my_mdarr value(%zu)... [%lld]\n", i, (long long)my_mdarr.i64_cs(i));
    }

    return 0;
}
