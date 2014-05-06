/**
  example code
  mdarrayクラス: mdarray &operator*=(const mdarray &obj)
                 mdarray &operator*=(double v)
                 mdarrayオブジェクトへの乗算
*/
#include <sli/stdstreamio.h>
#include <sli/mdarray.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;
    mdarray my_mdarr0(INT32_ZT, true);
    mdarray my_mdarr1(INT32_ZT, true);
    mdarray my_mdarr2;
    size_t i;

    sio.printf("*** BEGIN: 05_opeMulti_sample ***\n");

    my_mdarr0.set_auto_resize(false).resize(2);
    my_mdarr0 = 50;

    my_mdarr1.i32(0) = 10;
    my_mdarr1.i32(1) = 20;
    my_mdarr1.i32(2) = 30;

    sio.printf("obj0 *= obj1 :\n");
    my_mdarr0 *= my_mdarr1;
    sio.printf(" result type = %zd\n", my_mdarr0.size_type());
    for ( i=0 ; i < my_mdarr0.length() ; i++ ) {
        sio.printf(" my_mdarr0 value(%zu)... [%g]\n", i, my_mdarr0.dvalue(i));
    }

    sio.printf("obj0 = obj0 * double :\n");
    my_mdarr0 = my_mdarr0 * 0.5;
    sio.printf(" result type = %zd\n", my_mdarr0.size_type());
    for ( i=0 ; i < my_mdarr0.length() ; i++ ) {
        sio.printf(" my_mdarr0 value(%zu)... [%g]\n", i, my_mdarr0.dvalue(i));
    }

    sio.printf("obj0 = obj0 * int :\n");
    my_mdarr0 = my_mdarr0 * 2;
    sio.printf(" result type = %zd\n", my_mdarr0.size_type());
    for ( i=0 ; i < my_mdarr0.length() ; i++ ) {
        sio.printf(" my_mdarr0 value(%zu)... [%g]\n", i, my_mdarr0.dvalue(i));
    }

    /* --------------------------------------------------------------------- */

    my_mdarr0.init(DOUBLE_ZT, true);
    my_mdarr1.init(INT32_ZT, true);

    my_mdarr0.resize_2d(8,5);
    my_mdarr1.resize_2d(4,9);

    my_mdarr0 = 10;
    my_mdarr1 = 4;

    sio.printf("using arr * arr [N=N*O]:\n");

    my_mdarr2 = my_mdarr0 * my_mdarr1;

    sio.printf(" result type = %zd\n", my_mdarr2.size_type());
    for ( i=0 ; i < my_mdarr2.row_length() ; i++ ) {
	size_t j;
	sio.printf(" ");
	for ( j=0 ; j < my_mdarr2.col_length() ; j++ ) {
	    sio.printf("[%g]", my_mdarr2.dvalue(j,i));
	}
	sio.printf("\n");
    }

    sio.printf("using arr * arr [N=O*N]:\n");

    my_mdarr2 = my_mdarr1 * my_mdarr0;

    sio.printf(" result type = %zd\n", my_mdarr2.size_type());
    for ( i=0 ; i < my_mdarr2.row_length() ; i++ ) {
	size_t j;
	sio.printf(" ");
	for ( j=0 ; j < my_mdarr2.col_length() ; j++ ) {
	    sio.printf("[%g]", my_mdarr2.dvalue(j,i));
	}
	sio.printf("\n");
    }

    sio.printf("*** END: 05_opeMulti_sample ***\n");
    sio.printf("\n");

    return 0;
}
