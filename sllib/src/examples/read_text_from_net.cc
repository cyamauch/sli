#include <sli/stdstreamio.h>
#include <sli/digeststreamio.h>
#include <sli/tstring.h>
using namespace sli;

/**
 * @file   read_text_from_net.cc
 * @brief  httpサーバやftpサーバ上にあるテキストファイルを1行ずつ読み，出力する
 */

/*
 * An example code for reading a text file from network
 */

int main( int argc, char *argv[] )
{
    int ret_status = -1;
    stdstreamio sio;				/* stdin, stdout and stderr */
    digeststreamio f_in;			/* for local file */

    if ( 1 < argc ) {

	const char *filename = argv[1];
	tstring line;				/* buffer for a line */
	
	/*
	 * open a stream: 
	 * "http://...", "ftp://..." and compressed stream are also OK.
	 */
	if ( f_in.open("r", filename) < 0 ) {
	    sio.eprintf("[ERROR] cannot open: %s\n",filename);
	    goto quit;
	}

	/*
	 * read text line by line
	 */
	while ( (line=f_in.getline()) != NULL ) {
	    line.chomp();				/* erase CR and LF */
	    sio.printf("%s\n",line.cstr());
	}

	/*
	 * close stream
	 */
	f_in.close();

    }

    ret_status = 0;
 quit:
    return ret_status;
}
