#include <sli/stdstreamio.h>
#include <sli/tarray_tstring.h>
using namespace sli;

/**
 * @file   regexp_back_reference.cc
 * @brief  POSIX拡張正規表現マッチを行ない 後方参照情報を含んだ文字列配列を取得
 */

/*
 * Trying match using regular expressions, and store matched strings including
 * back reference parts.
 */

int main( int argc, char *argv[] )
{
    stdstreamio sio;
    tstring my_str = "OS = linux";         /* target string */
    tarray_tstring my_elms;                /* result is stored to this obj */

    sio.printf("target string = '%s'\n", my_str.cstr());

    /*
     * split "OS = linux" using regular expression
     */
    my_elms.regassign(my_str, "([^ ]+)([ ]*=[ ]*)([^ ]+)");

    if ( my_elms.length() == 4 ) {
        sio.printf("keyword='%s' value='%s'\n", 
                    my_elms[1].cstr(), my_elms[3].cstr());
    }

    my_elms.dprint();

    return 0;
}
