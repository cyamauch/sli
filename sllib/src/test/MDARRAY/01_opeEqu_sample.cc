/**
  example code
  mdarrayクラス: mdarray &operator=(const mdarray &obj)
                 mdarrayオブジェクトの初期化
*/
#include <sli/stdstreamio.h>
#include <sli/mdarray.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;
    mdarray my_mdarr;
    mdarray area_mdarr(LLONG_ZT, true);
    size_t i;

    /* area_mdarrに値をセット */
    area_mdarr.ll(0) = 17090000;
    area_mdarr.ll(1) = 9980000;
    area_mdarr.ll(2) = 9620000;

    /* 確認 */
    for ( i=0 ; i < area_mdarr.length() ; i++ ) {
        sio.printf("area_mdarr value(%zu)... [%lld]\n", 
		   i, area_mdarr.ll_cs(i));
    }

    /* my_mdarr に area_mdarrをセット */
    my_mdarr = area_mdarr;

    /* 確認 */
    for ( i=0 ; i < my_mdarr.length() ; i++ ) {
        sio.printf("my_mdarr value(%zu)... [%lld]\n", i, my_mdarr.ll_cs(i));
    }

    sio.printf("area_mdarr size_type ... [%zd]\n",  area_mdarr.size_type());

    return 0;
}
