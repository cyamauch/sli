/**
  example code
  mdarrayクラス: bool auto_resize() const
                 リサイズモードの取得・設定
*/
#include <sli/stdstreamio.h>
#include <sli/mdarray.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;

    mdarray my_mdarr0dim(DOUBLE_ZT, true);

    /* リサイズモードを取得して表示 */
    sio.printf("*** my_mdarr0dim auto_resize... [%d]\n",
	       (int)(my_mdarr0dim.auto_resize()));

    /* リサイズのテスト */
    sio.printf("** before ** dim_length: %zd  col: %zd  row: %zd  layer: %zd\n",
	       my_mdarr0dim.dim_length(), my_mdarr0dim.col_length(),
	       my_mdarr0dim.row_length(), my_mdarr0dim.layer_length());
    my_mdarr0dim.d(4) = 1;
    sio.printf("** after1 ** dim_length: %zd  col: %zd  row: %zd  layer: %zd\n",
	       my_mdarr0dim.dim_length(), my_mdarr0dim.col_length(),
	       my_mdarr0dim.row_length(), my_mdarr0dim.layer_length());
    my_mdarr0dim.d(0,2) = 2;
    sio.printf("** after2 ** dim_length: %zd  col: %zd  row: %zd  layer: %zd\n",
	       my_mdarr0dim.dim_length(), my_mdarr0dim.col_length(),
	       my_mdarr0dim.row_length(), my_mdarr0dim.layer_length());
    my_mdarr0dim.d(14) = 3;
    sio.printf("** after3 ** dim_length: %zd  col: %zd  row: %zd  layer: %zd\n",
	       my_mdarr0dim.dim_length(), my_mdarr0dim.col_length(),
	       my_mdarr0dim.row_length(), my_mdarr0dim.layer_length());
    my_mdarr0dim.d(15) = 4;
    sio.printf("** after4 ** dim_length: %zd  col: %zd  row: %zd  layer: %zd\n",
	       my_mdarr0dim.dim_length(), my_mdarr0dim.col_length(),
	       my_mdarr0dim.row_length(), my_mdarr0dim.layer_length());
    my_mdarr0dim.d(4,3,2) = 5;
    sio.printf("** after5 ** dim_length: %zd  col: %zd  row: %zd  layer: %zd\n",
	       my_mdarr0dim.dim_length(), my_mdarr0dim.col_length(),
	       my_mdarr0dim.row_length(), my_mdarr0dim.layer_length());
    my_mdarr0dim.d(0,11) = 6;
    sio.printf("** after6 ** dim_length: %zd  col: %zd  row: %zd  layer: %zd\n",
	       my_mdarr0dim.dim_length(), my_mdarr0dim.col_length(),
	       my_mdarr0dim.row_length(), my_mdarr0dim.layer_length());
    my_mdarr0dim.d(0,12) = 7;
    sio.printf("** after8 ** dim_length: %zd  col: %zd  row: %zd  layer: %zd\n",
	       my_mdarr0dim.dim_length(), my_mdarr0dim.col_length(),
	       my_mdarr0dim.row_length(), my_mdarr0dim.layer_length());
    //my_mdarr0dim.dprint();

    size_t nx[] = {3, 4, 5};
    mdarray my_mdarr3dim(FLOAT_ZT, true);
    my_mdarr3dim.set_auto_resize(false).resize(nx,3,true);
    sio.printf("*** my_mdarr3dim auto_resize... [%d]\n",
	       (int)(my_mdarr3dim.auto_resize()));

    sio.printf("end\n");

    return 0;
}
