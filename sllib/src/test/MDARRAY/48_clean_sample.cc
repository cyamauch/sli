/**
  example code
  mdarrayクラス: mdarray &clean( ssize_t col_index = 0, size_t col_size = MDARRAY_ALL,
		                 ssize_t row_index = 0, size_t row_size = MDARRAY_ALL,
		                 ssize_t layer_index = 0, size_t layer_size = MDARRAY_ALL );
                 配列要素をデフォルト値で埋める(画像向き)
*/
#include <sli/stdstreamio.h>
#include <sli/mdarray.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;
    size_t nx[] = {2, 2};
    mdarray my_smdarr(SHORT_ZT, true);
    size_t i, j;

    my_smdarr.resize(nx,2,true);

    my_smdarr.s(0,0) = 1;
    my_smdarr.s(1,0) = 3;
    my_smdarr.s(0,1) = 2;
    my_smdarr.s(1,1) = 4;

    my_smdarr.clean(1,1,1,1);
    for ( j = 0 ; j < my_smdarr.length(1) ; j++ ) {
	for ( i = 0 ; i < my_smdarr.length(0) ; i++ ) {
	    sio.printf("my_smdarr value(%zu,%zu)... [%hd]\n", 
		       i, j, my_smdarr.s(i, j));
	}
    }

    return 0;
}
