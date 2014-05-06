/**
  example code
  mdarrayクラス: mdarray &operator+=(const mdarray &obj)
                 mdarray &operator+=(double v)
                 mdarrayオブジェクトへの加算
*/
#include <sli/stdstreamio.h>
#include <sli/mdarray.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;
    mdarray my_mdarr0(UCHAR_ZT, true);
    mdarray my_mdarr1;
    mdarray my_mdarr2;
    size_t i;

    my_mdarr0.resize(5);

    sio.printf("*** BEGIN: 03_opePlus_sample ***\n");

    sio.printf("using '+=':\n");
    my_mdarr0 = 25;

    my_mdarr0 += 50;
    sio.printf(" result type = %zd\n", my_mdarr0.size_type());
    for ( i=0 ; i < my_mdarr0.length() ; i++ ) {
        sio.printf(" my_mdarr0 value(%zu)... [%g]\n", i, my_mdarr0.dvalue(i));
    }

    sio.printf("using '+' [N=O+N]:\n");
    my_mdarr0 = 25;

    my_mdarr0 = my_mdarr0 + 50;
    sio.printf(" result type = %zd\n", my_mdarr0.size_type());
    for ( i=0 ; i < my_mdarr0.length() ; i++ ) {
        sio.printf(" my_mdarr0 value(%zu)... [%g]\n", i, my_mdarr0.dvalue(i));
    }

    my_mdarr0.init(DOUBLE_ZT, true);
    my_mdarr0.resize(5);

    sio.printf("using '+' [N=N+O]:\n");
    my_mdarr0 = 25;

    my_mdarr0 = my_mdarr0 + 50;
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

    sio.printf("using arr + arr [N=N+O]:\n");

    my_mdarr2 = my_mdarr0 + my_mdarr1;

    sio.printf(" result type = %zd\n", my_mdarr2.size_type());
    for ( i=0 ; i < my_mdarr2.row_length() ; i++ ) {
	size_t j;
	sio.printf(" ");
	for ( j=0 ; j < my_mdarr2.col_length() ; j++ ) {
	    sio.printf("[%g]", my_mdarr2.dvalue(j,i));
	}
	sio.printf("\n");
    }

    sio.printf("using arr + arr [N=O+N]:\n");

    my_mdarr2 = my_mdarr1 + my_mdarr0;

    sio.printf(" result type = %zd\n", my_mdarr2.size_type());
    for ( i=0 ; i < my_mdarr2.row_length() ; i++ ) {
	size_t j;
	sio.printf(" ");
	for ( j=0 ; j < my_mdarr2.col_length() ; j++ ) {
	    sio.printf("[%g]", my_mdarr2.dvalue(j,i));
	}
	sio.printf("\n");
    }

    sio.printf("*** END: 03_opePlus_sample ***\n");
    sio.printf("\n");

    return 0;
}
