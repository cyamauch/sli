/**
  example code
  asarray_tstringクラス:size_t length() const
                        size_t length( const char *key ) const
                        連想配列の長さ(個数)，文字列長の長さ
*/
#include <sli/stdstreamio.h>
#include <sli/asarray_tstring.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;
    asarray_tstring my_asarr;
    my_asarr["linux"]   = "Linus Torvalds";
    my_asarr["google"]  = "Larry Page";
    my_asarr["mac"]     = "Steve Jobs";

    //配列長を取得
    sio.printf("my_asarr total length   ... [%zu]\n", my_asarr.length());
    //'google'に対応する値の文字列長を取得
    sio.printf("my_asarr key='google' length ... [%zu]\n", my_asarr.length("google"));


    //asarrについて同様に処理する
    asarray_tstring asarr;
    asarr["a"] = "linux";
    asarr["b"] = "solaris";
    asarr["c"] = "windows";
    asarr["d"] = "mac";
    asarr["e"] = "lindows";

    //配列長を取得
    sio.printf("asarr total length ... [%zu]\n", asarr.length());
    //'c'に対応する値の文字列長を取得
    sio.printf("asarr key='c' length ... [%zu]\n", asarr.length("c"));
    /* すべての要素を表示 */
    for ( size_t i=0 ; i < asarr.length() ; i++ ) {
        const char *key = asarr.key(i);
        sio.printf("%s ... [%s]\n", key, asarr.cstr(key));
    }

    return 0;
}

