#include <sli/stdstreamio.h>
#include <sli/digeststreamio.h>
#include <sli/tstring.h>
using namespace sli;

/**
 * @file   verbose_grep.cc
 * @brief  ネットワークに対応した冗長な grep コマンド
 */

int main( int argc, char *argv[] )
{
    stdstreamio sio;
    digeststreamio sin;
    tstring buf;

    if ( 1 < argc ) {
	if ( 2 < argc ) {
	    int s = sin.open("r",argv[2]);
	    if ( s ) {
		sio.eprintf("[ERROR] obj.read_stream() failed\n");
		return -1;
	    }
	}
	else {
	    int s = sin.open("r");
	    if ( s ) {
		sio.eprintf("[ERROR] obj.read_stream() failed\n");
		return -1;
	    }
	}
	while ( (buf = sin.getline()) != NULL ) {
	    size_t len;
	    ssize_t pos = buf.chomp().expand_tabs().regmatch(0,argv[1],&len);
	    if ( 0 <= pos ) {
		tstring uline;
		sio.putstr(buf.cstr());
		sio.putstr("\n");
		uline.assign(' ',pos);
		uline.append('~',len);
		sio.printf("%s[%d,%d]\n",uline.cstr(),(int)pos,(int)len);
	    }
	}
	sio.printf("pattern = '%s'\n",argv[1]);
	sin.close();
    }

    return 0;
}
