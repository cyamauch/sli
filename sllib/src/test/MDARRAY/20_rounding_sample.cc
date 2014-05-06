/**
  example code
  mdarrayクラス: bool rounding() const
                 四捨五入の可否の取得・設定
*/
#include <sli/stdstreamio.h>
#include <sli/mdarray.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;
    mdarray my_imdarr(LLONG_ZT, true);

    my_imdarr.assign(1.618, 0);
    /* 初期状態は、四捨五入しないことを確認 */
    sio.printf("my_imdarr value(0)... [%lld]\n", my_imdarr.ll(0));

    /* 四捨五入を行うよう設定 */
    my_imdarr.set_rounding(true);
    my_imdarr.assign(1.618, 1);
    /* 四捨五入が行われるか確認 */
    sio.printf("my_imdarr value(1)... [%lld]\n", my_imdarr.ll(1));

    mdarray my_mdarr;
    my_mdarr.init(my_imdarr);
    /* 初期状態の四捨五入の設定を出力 */
    sio.printf("my_mdarr rounding = [%d]\n", (int)my_mdarr.rounding());

    return 0;
}
