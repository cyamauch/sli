#include <sli/stdstreamio.h>
#include <sli/digeststreamio.h>
#include <unistd.h>
using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;
    digeststreamio sin;
    char buf[65536];
    ssize_t n;
    int ch, tty_in = 0;

    if ( 1 < argc ) {
	int s = sin.open("r",argv[1]);
	if ( s ) {
	    sio.eprintf("[ERROR] sin.open() failed\n");
	    return -1;
	}
    }
    else {
	tty_in = isatty(0);
	int s = sin.open("r");
	if ( s ) {
	    sio.eprintf("[ERROR] sin.open() failed\n");
	    return -1;
	}
    }
    if ( tty_in == 0 ) {
	while ( 0 < (n=sin.read(buf,65536)) ) sio.write(buf,n);
    }
    else {
	while ( (ch=sin.getchr()) != EOF ) sio.putchr(ch);
    }
    sin.close();

    return 0;
}
