/**
  example code
  mdarrayクラス: mdarray &register_extptr(void **extptr_ptr)
                 ユーザポインタ変数の登録
*/
#include<stdio.h>
#include <sli/stdstreamio.h>
#include <sli/mdarray.h>

using namespace sli;
stdstreamio sio;

int main(void)
{
    struct tokyoStock {
        short id;
        long stock;
    };

    struct tokyoStock   *tokyoStock_ptr;
    mdarray tokyoStock_buf(sizeof(struct tokyoStock), true);

    tokyoStock_buf.register_extptr((void **)&tokyoStock_ptr);
    tokyoStock_buf.resize(6);
    tokyoStock_ptr[0].id = 3407;
    tokyoStock_ptr[0].stock = 438;

    tokyoStock_ptr[1].id = 4951;
    tokyoStock_ptr[1].stock = 1058;

    tokyoStock_ptr[2].id = 4912;
    tokyoStock_ptr[2].stock = 449;

    sio.printf("tokyoStock_buf length[%zu]\n", tokyoStock_buf.length());

    //確認の為、要素の値を出力
    for (size_t i = 0; i < tokyoStock_buf.length(0); i++) {
      sio.printf("tokyoStock_buf id[%zu]... [%hd]\n", i, tokyoStock_ptr[i].id);
      sio.printf("tokyoStock_buf stock[%zu]... [%ld]\n", i, tokyoStock_ptr[i].stock);
    }

    return 0;
}

