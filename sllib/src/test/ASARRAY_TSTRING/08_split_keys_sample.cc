/**
  example code
  asarray_tstringクラス:asarray_tstring &split_keys( const char *line, const char *delims, 
 	                			     bool zero_str, const char *quotations, int escape )
			asarray_tstring &split_keys( const char *line, const char *delims,
						     bool zero_str = false )
                        文字列を分割しキーに設定する

  			asarray_tstring &split_values( const char *line, const char *delims, 
	                    			       bool zero_str, const char *quotations, int escape )
		    	asarray_tstring &split_values( const char *line, const char *delims,
						       bool zero_str = false )
                        文字列を分割し値に設定する

*/
#include <sli/stdstreamio.h>
#include <sli/asarray_tstring.h>
#include <sli/tarray_tstring.h>
#include <sli/ctindex.h>
#include <sli/digeststreamio.h>
#include <sli/tstring.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;

    const char *line = "'Camellia sasanqua' 'Chrysanthemum morifolium' 'Cyclamen persicum'";
    asarray_tstring my_arr;
    //lineの文字列を分割してmy_arrのキーに設定
    my_arr.split_keys(line, " ", false, "'", 0);
    //キーの設定を確認
    for ( size_t i=0 ; i < my_arr.length() ; i++ ) {
        const char *key = my_arr.key(i);
        sio.printf("%s ... [%s]\n", key, my_arr.cstr(key));
    }

    sio.printf("******************************************\n");

    const char *val = "'Camellia,pink','Chrysanthemum,yellow','Cyclamen,pink'";
    //valの文字列を分割してmy_arrの値に設定
    my_arr.split_values(val, ",", false, "'", 0);
    //キーと値の設定を確認
    for ( size_t i=0 ; i < my_arr.length() ; i++ ) {
        const char *key = my_arr.key(i);
        sio.printf("%s ... [%s]\n", key, my_arr.cstr(key));
    }

    //ファイルからの読み込んだ文字列を設定する例
    stdstreamio fin;
    tstring line_buf;
    asarray_tstring academy_awards_asarr;

    //ファイルのオープン
    if (fin.open("r","split_key_value.csv") < 0){
      //エラー
    }

    //ファイルの読み込み
    if ((line_buf = fin.getline()) == NULL){
      //エラー
    }

    //行末の改行文字を読み込むようなので、削除する
    line_buf.strtrim();
    sio.printf("line_buf ... [%s]\n", line_buf.cstr());
    //1行目の文字列を分割してmy_arrのキーに設定
    academy_awards_asarr.split_keys(line_buf.cstr(), ",", false);
    for ( size_t i=0 ; i < academy_awards_asarr.length() ; i++ ) {
        const char *key = academy_awards_asarr.key(i);
        sio.printf("%s ... [%s]\n", key, academy_awards_asarr.cstr(key));
    }

    //念の為、初期化
    line_buf.init();
    //次の行の読み込み
    if ((line_buf = fin.getline()) == NULL){
      //エラー
    }
    line_buf.strtrim();
    sio.printf("line_buf ... [%s]\n", line_buf.cstr());
    //2行目の文字列を分割してmy_arrの値に設定
    academy_awards_asarr.split_values(line_buf.cstr(), ",", false);
    for ( size_t i=0 ; i < academy_awards_asarr.length() ; i++ ) {
        const char *key = academy_awards_asarr.key(i);
        sio.printf("%s ... [%s]\n", key, academy_awards_asarr.cstr(key));
    }

    return 0;
}

