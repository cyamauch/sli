/**
  example code
  mdarrayクラス: mdarray &insert( ssize_t idx, size_t len )
    		 mdarray &insert( size_t dim_index, ssize_t idx, size_t len )
                 要素の挿入
*/
#include <sli/stdstreamio.h>
#include <sli/mdarray.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;
    mdarray my_mdarr(LONG_ZT, true);
    size_t i;

    my_mdarr.resize(2);
    my_mdarr.l(0) = -2147483646;
    my_mdarr.l(1) = 2147483647;
    my_mdarr.insert( 1, 2 );

    /* insert後の値を出力 */
    for ( i = 0 ; i < my_mdarr.length(0) ; i++ ) {
        sio.printf("my_mdarr value(%zu)... [%ld]\n", i, my_mdarr.l(i));
    }

    return 0;
}
