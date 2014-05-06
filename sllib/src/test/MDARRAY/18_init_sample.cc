/**
  example code
  mdarrayクラス: mdarray &init()
                 mdarray &init( ssize_t sz_type )
                 mdarray &init( ssize_t sz_type, const size_t naxisx[],
			        size_t ndim )
                 mdarray &init( ssize_t sz_type, size_t naxis0 )
                 mdarray &init( ssize_t sz_type, size_t naxis0, size_t naxis1 )
                 mdarray &init( ssize_t sz_type, size_t naxis0, size_t naxis1,
			        size_t naxis2 )
                 mdarray &init( const mdarray &obj );mdarray &operator=(double v)
                 配列の初期化
*/
#include <sli/stdstreamio.h>
#include <sli/mdarray.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;
    mdarray my_mdarr;
    size_t nx[] = {2, 3};

    sio.printf("start\n");

    my_mdarr.init(INT_ZT, true, nx, 2, true);

    /* 初期化後のサイズを出力 */
    sio.printf("*** my_mdarr 0 dim length ====> [%zu] *** \n",
	       my_mdarr.length(0));
    sio.printf("*** my_mdarr col_length ====> [%zu] *** \n",
	       my_mdarr.col_length());
    sio.printf("*** my_mdarr 1st dim length ====> [%zu] *** \n",
	       my_mdarr.length(1));
    sio.printf("*** my_mdarr row_length ====> [%zu] *** \n",
	       my_mdarr.row_length());

    return 0;
}
