/**
  example code
  mdarrayクラス: ssize_t copy( mdarray &dest,
		 	       ssize_t col_idx, size_t col_len=MDARRAY_ALL,
		     	       ssize_t row_idx=0, size_t row_len=MDARRAY_ALL, 
		     	       ssize_t layer_idx=0, size_t layer_len=MDARRAY_ALL ) const;
                 配列オブジェクトのコピー
*/
#include <sli/stdstreamio.h>
#include <sli/mdarray.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;
    size_t nx_2d[] = {2,2};
    size_t nx_3d[] = {4,2,3};

    mdarray my_cmdarr(UCHAR_ZT, true);
    my_cmdarr.resize(nx_2d,2,true);
    unsigned char my_char[] = {98, 99, 101, 102};
    my_cmdarr.putdata((const void *)my_char, sizeof(my_char));

    mdarray my_dmdarr(DOUBLE_ZT, true);
    my_dmdarr.resize(nx_3d,3,true);
    double my_d[] = {-111, 112, -113, 114, 
		     -121, 122, -123, 124,
		     -211, 212, -212, 214,
		     -221, 222, -222, 224,
		     -311, 312, -312, 314,
		     -321, 322, -322, 324,
    };
    my_dmdarr.putdata((const void *)my_d, sizeof(my_d));

    //コピー元の値を出力
    for (size_t j = 0; j < my_cmdarr.length(1); j++){
	for (size_t i = 0; i < my_cmdarr.length(0); i++){
            sio.printf("my_cmdarr value(%zu,%zu)... [%hhu]\n", i, j, my_cmdarr.c(i, j));
        }
    }

    sio.printf("********************************\n");

    sio.printf("size_type... [%zd]\n", my_dmdarr.size_type());
    ssize_t ret_size = my_cmdarr.copy( &my_dmdarr, 0, 2, 1, 1 );
    sio.printf("=> size_type... [%zd]\n", my_dmdarr.size_type());
    sio.printf("=> ret_size.... [%zd]\n", ret_size);

    sio.printf("** copy() **********************\n");

    //コピー後のコピー先の値を出力
    for (size_t j = 0; j < my_dmdarr.length(1); j++){
        for (size_t i = 0; i < my_dmdarr.length(0); i++){
            //copyによりmy_dmdarrの型はunsigned charになります。
            sio.printf("my_dmdarr value(%zu,%zu)... [%hhu]\n", i, j, my_dmdarr.c(i, j));
        }
    }

    sio.printf("********************************\n");

    /* 再度初期化 */
    my_dmdarr.init(DOUBLE_ZT, true);
    my_dmdarr.resize(nx_3d,3,true);
    my_dmdarr.putdata((const void *)my_d, sizeof(my_d));

    for (size_t k = 0; k < my_dmdarr.layer_length(); k++) {
	for (size_t j = 0; j < my_dmdarr.row_length(); j++) {
	    sio.printf("my_dmdarr value(?,%zu,%zu) ... ", j, k);
	    for (size_t i = 0; i < my_dmdarr.col_length(); i++) {
		sio.printf("[%g]",my_dmdarr.dvalue(i, j, k));
	    }
	    sio.printf("\n");
	}
    }

    sio.printf("** copyf() *********************\n");
    my_dmdarr.copyf(&my_cmdarr, "2:1,*,1");

    //sio.printf("** sectionf() *********************\n");
    //my_cmdarr = my_dmdarr.sectionf("2:1,*,1");

    //my_cmdarr = my_dmdarr;  my_cmdarr.trimf("2:1,*,1");

    for (size_t k = 0; k < my_cmdarr.layer_length(); k++) {
	for (size_t j = 0; j < my_cmdarr.row_length(); j++) {
	    sio.printf("my_cmdarr value(?,%zu,%zu) ... ", j, k);
	    for (size_t i = 0; i < my_cmdarr.col_length(); i++) {
		sio.printf("[%g]",my_cmdarr.dvalue(i, j, k));
	    }
	    sio.printf("\n");
	}
    }

    return 0;
}
