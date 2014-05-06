#include <sli/stdstreamio.h>
#include <sli/tstring.h>
#include <sli/tarray_tstring.h>
using namespace sli;

/*
 * main function
 */
int main( int argc, char *argv[] )
{
    stdstreamio sio;
    size_t i;
    tstring target = 
    "winnt,Program\\ Files, 'My Music' (Documents,'(' (Settings )),Recycled,,";
    tarray_tstring arr;

    sio.printf("Target string:\n");
    sio.printf("  [%s]\n", target.cstr());
    sio.printf("\n");

    arr.explode(target, ", ", true);
    sio.printf("Result of explode() with \"%s\"\n", ", ");
    for ( i=0 ; i < arr.length() ; i++ ) {
	sio.printf("  [%s]\n", arr.cstr(i));
    }
    sio.printf("\n");

    arr.explode(target, ",", true);
    sio.printf("Result of explode() with \"%s\" zero_str=true\n", ",");
    for ( i=0 ; i < arr.length() ; i++ ) {
        sio.printf("  [%s]\n", arr.cstr(i));
    }
    sio.printf("\n");

    arr.explode(target, ",", false);
    sio.printf("Result of explode() with \"%s\" zero_str=false\n", ",");
    for ( i=0 ; i < arr.length() ; i++ ) {
        sio.printf("  [%s]\n", arr.cstr(i));
    }
    sio.printf("\n");

    arr.split(target, ", ", false);
    sio.printf("Result of simple split() with \"%s\"\n", ", ");
    for ( i=0 ; i < arr.length() ; i++ ) {
	sio.printf("  [%s]\n", arr.cstr(i));
    }
    sio.printf("\n");

    arr.split(target, ", ", false, "`()[]<>{}'", '\\', true);
    sio.printf("Result of advanced split()\n");
    for ( i=0 ; i < arr.length() ; i++ ) {
	sio.printf("  [%s]\n", arr.cstr(i));
    }
    sio.printf("\n");

    arr.split(target, ",", true);
    sio.printf("Result of split() with simple CSV mode\n");
    for ( i=0 ; i < arr.length() ; i++ ) {
	sio.printf("  [%s]\n", arr.cstr(i));
    }
    sio.printf("\n");

    arr.split(target, "[^a-zA-Z]", false);
    sio.printf("Result of split() with [^a-zA-Z] expression\n");
    for ( i=0 ; i < arr.length() ; i++ ) {
	sio.printf("  [%s]\n", arr.cstr(i));
    }
    sio.printf("\n");

    /* more tests */
#if 0
    arr.split(target, "[a-zA-Z ]", false);
    sio.printf("Result: split: [a-zA-Z ]\n");
    for ( i=0 ; i < arr.length() ; i++ ) {
	sio.printf("  [%s]\n", arr.cstr(i));
    }
    sio.printf("\n");

    arr.split(target, "[a-zA-Z ]", false, "", '\\', true);
    sio.printf("Result: split: [a-zA-Z ] with rm ESC\n");
    for ( i=0 ; i < arr.length() ; i++ ) {
	sio.printf("  [%s]\n", arr.cstr(i));
    }
    sio.printf("\n");

    arr.split(target, "", false, "", '\\', true);
    sio.printf("Result: split: \"\" with rm ESC\n");
    for ( i=0 ; i < arr.length() ; i++ ) {
	sio.printf("  [%s]\n", arr.cstr(i));
    }
    sio.printf("\n");

    arr.split(target, "", false);
    sio.printf("Result: split: \"\"\n");
    for ( i=0 ; i < arr.length() ; i++ ) {
	sio.printf("  [%s]\n", arr.cstr(i));
    }
    sio.printf("\n");

    arr.explode(target, "", true);
    sio.printf("Result: explode: \"\"\n");
    for ( i=0 ; i < arr.length() ; i++ ) {
	sio.printf("  [%s]\n", arr.cstr(i));
    }
    sio.printf("\n");
#endif

#if 0	/* New API */
    arr.explode(target, ",", true, "`()[]<>{}'", '\\', true);
    sio.printf("Result of advanced explode() zero_str=true\n");
    for ( i=0 ; i < arr.length() ; i++ ) {
	sio.printf("  [%s]\n", arr.cstr(i));
    }
    sio.printf("\n");

    arr.explode(target, ",", false, "`()[]<>{}'", '\\', true);
    sio.printf("Result of advanced explode() zero_str=false\n");
    for ( i=0 ; i < arr.length() ; i++ ) {
        sio.printf("  [%s]\n", arr.cstr(i));
    }
    sio.printf("\n");
#endif

    return 0;
}

