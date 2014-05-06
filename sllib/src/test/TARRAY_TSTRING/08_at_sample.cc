/**
  example code
  tarray_tstringクラス:tstring &at( size_t index )
		       const tstring &at_cs( size_t index ) const
                       指定要素位置の文字列参照
*/
#include <sli/stdstreamio.h>
#include <sli/tarray_tstring.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;

    tarray_tstring my_arr;
    //my_arr[0] = "Hello"; と同じ
    my_arr.at(0) = "Hello";
    sio.printf("my_arr[0] = %s\n",my_arr.at(0).cstr());
    //明示的に代入していない場合の確認
    sio.printf("my_arr[1] = %s\n",my_arr.at(1).cstr());

    sio.printf("***********************\n");
    //エラー確認用コード：at_cs()での配列長を超えての参照はエラー
    /*
    try{
       sio.printf("my_arr[2] = %s\n",my_arr.at_cs(2).cstr());
    }catch ( err_rec msg ) {
      sio.eprintf("[EXCEPTION] function=[%s::%s] message=[%s]\n",
                  msg.class_name, msg.func_name, msg.message);
      return -1;
    }
    */

    return 0;
}

