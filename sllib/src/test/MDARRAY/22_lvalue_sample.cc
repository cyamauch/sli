/**
  example code
  mdarrayクラス: long lvalue( ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF, 
		              ssize_t idx2 = MDARRAY_INDEF ) const
                 long long llvalue( ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF, 
			            ssize_t idx2 = MDARRAY_INDEF ) const
                 long型又はlong long型に変換した要素の値
*/
#include <sli/stdstreamio.h>
#include <sli/mdarray.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;
    mdarray my_mdarry(FLOAT_ZT, true);

    sio.printf("start\n");

    my_mdarry.f(0) = 123.556;
    //float型で出力
    sio.printf("my_mdarry f... [%g]\n", my_mdarry.f(0));
    //long型で出力 小数点以下切り捨て
    sio.printf("my_mdarry lvalue... [%ld]\n", my_mdarry.lvalue(0));
    //long long型で出力 小数点以下切り捨て
    sio.printf("my_mdarry llvalue... [%lld]\n", my_mdarry.llvalue(0));

    //四捨五入可に設定
    my_mdarry.set_rounding(true);
    //long型で出力 小数点以下四捨五入
    sio.printf("my_mdarry lvalue... [%ld]\n", my_mdarry.lvalue(0));
    //long long型で出力 小数点以下四捨五入
    sio.printf("my_mdarry llvalue... [%lld]\n", my_mdarry.llvalue(0));

    sio.printf("end\n");

    return 0;
}
