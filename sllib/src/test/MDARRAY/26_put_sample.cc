/**
  example code
  mdarrayクラス: mdarray &put( const void *value_ptr, ssize_t idx, size_t len )
		 mdarray &put( const void *value_ptr,
			  	size_t dim_index, ssize_t idx, size_t len )
                 任意の要素位置へ値をセット
*/
#include <sli/stdstreamio.h>
#include <sli/mdarray.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;
    short s12 = 12;
    const void *ptr_s12 = &s12;
    mdarray my_smdarr(SHORT_ZT, true);

    my_smdarr.resize(3);
    my_smdarr.put(ptr_s12,1,2);

    /* put後の値を出力 */
    for (size_t i = 0; i < my_smdarr.length(); i++){
        sio.printf("my_smdarr value(%zu)... [%hd]\n", i, my_smdarr.s(i));
    }

    return 0;
}
