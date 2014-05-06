#include <sli/stdstreamio.h>
using namespace sli;

/**
 * @file   read_local_text_file.cc
 * @brief  ローカルにあるテキストファイルを1行ずつ読み，出力するコード
 */

/*
 * An example code for reading a local text file
 */

int main( int argc, char *argv[] )
{
    int ret_status = -1;
    stdstreamio sio;				/* stdin, stdout and stderr */
    stdstreamio f_in;				/* for local file */

    if ( 1 < argc ) {

	const char *filename = argv[1];
	const char *line;
	
	/*
	 * open a file
	 */
	if ( f_in.open("r", filename) < 0 ) {
	    sio.eprintf("[ERROR] cannot open: %s\n",filename);
	    goto quit;
	}

	/*
	 * read text line by line
	 */
	while ( (line=f_in.getline()) != NULL ) {
	    sio.printf("%s",line);
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
