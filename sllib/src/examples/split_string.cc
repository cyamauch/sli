#include <sli/stdstreamio.h>
#include <sli/tarray_tstring.h>
using namespace sli;

/**
 * @file   split_string.cc
 * @brief  文字列のデリミタでの分割を行なう例(csvを分割，ssvを分割，高度な分割)
 */

/*
 * Example of splitting strings of csv, ssv, etc.
 */

int main( int argc, char *argv[] )
{
    stdstreamio sio;
    tarray_tstring my_arr;
    size_t i;


    /*
     * SSV format
     */

    const char *line_ssv = "abc def  wxyz ";

    sio.printf("target string = '%s'\n", line_ssv);

    /* split string of SSV format */
    my_arr.split(line_ssv, " ", false);		/* to "abc","def","wxyz" */

    for ( i=0 ; i < my_arr.length() ; i++ ) {
	sio.printf("my_arr[%zu] = '%s'\n", i, my_arr.cstr(i));
    }

    my_arr.dprint();

    sio.printf("\n");


    /*
     * CSV format
     */

    const char *line_csv = " Z-80,,  8086 , 6800";

    sio.printf("target string = '%s'\n", line_csv);

    /* split string of CSV format */
    my_arr.split(line_csv, ",", true);

    for ( i=0 ; i < my_arr.length() ; i++ ) {
	sio.printf("my_arr[%zu] = '%s'\n", i, my_arr[i].cstr());
    }

    my_arr.dprint();

    sio.eprintf("after trim():\n");
    my_arr.trim();
    my_arr.dprint();

    sio.printf("\n");


    /*
     * Advanced
     */

    const char *line_advanced = "winnt( ) program\\ files 'mary\\'s music'";

    sio.printf("target string = '%s'\n", line_advanced);

    /* to "winnt( )", "program\\ files", "'mary\\'s music'" */
    my_arr.split(line_advanced, " ", false, "'()", '\\', false);

    my_arr.dprint();


    return 0;
}
