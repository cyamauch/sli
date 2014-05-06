/**
  example code
  mdarrayクラス: mdarray &assign_default( const void *value_ptr )
                 mdarray &assign_default( double value )
                 サイズ拡張時の初期値
*/
#include <sli/stdstreamio.h>
#include <sli/mdarray.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;
    size_t nx[] = {3,2,2};
    mdarray my_mdarr(SHORT_ZT, true);
    size_t i,j,k;

    my_mdarr.resize(nx,3,true);

    /* 初期値設定 */
    my_mdarr.assign_default(50);
    for ( k=0 ; k < my_mdarr.layer_length() ; k++ ) {
	for ( j=0 ; j < my_mdarr.row_length() ; j++ ) {
	    for ( i=0 ; i < my_mdarr.col_length() ; i++ ) {
		sio.printf("my_mdarr value(%zu,%zu,%zu)... [%hd]\n",
			   i,j,k, my_mdarr.s(i,j,k));
	    }
	}
    }

    /* サイズ拡張 */
    my_mdarr.resize(1,4);

    /* 値を出力 */
    for ( k=0 ; k < my_mdarr.layer_length() ; k++ ) {
	for ( j=0 ; j < my_mdarr.row_length() ; j++ ) {
	    for ( i=0 ; i < my_mdarr.col_length() ; i++ ) {
		sio.printf("=> my_mdarr value(%zu,%zu,%zu)... [%hd]\n",
			   i,j,k, my_mdarr.s(i,j,k));
	    }
	}
    }

    sio.printf("end\n");

    return 0;
}
