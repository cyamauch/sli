#include <sli/stdstreamio.h>
#include <sli/digeststreamio.h>
using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;
    digeststreamio sin, sout;
    char buf[65536];
    ssize_t n;

    //pigz_disabled();
    //lbzip2_disabled();

    if ( argc < 3 ) {
	sio.eprintf("[USAGE]\n");
	sio.eprintf("$ %s foo foo.gz\n",argv[0]);
	sio.eprintf("$ %s foo.bz2 foo.gz\n",argv[0]);
	sio.eprintf("$ %s foo.bz foo\n",argv[0]);
	return 1;
    }
    
    if ( sin.open("r",argv[1]) < 0 ) {
	sio.eprintf("[ERROR] sin.open() failed\n");
	return -1;
    }
    if ( sout.open("w",argv[2]) < 0 ) {
	sio.eprintf("[ERROR] sout.open() failed\n");
	return -1;
    }

    while ( 0 < (n=sin.read(buf,65536)) ) sout.write(buf,n);

    //sio.printf("[in]  eof flag     : %d\n",sin.eof());
    //sio.printf("[in]  error flag   : %d\n",sin.error());
    //sio.printf("[in]  close status : %d\n",sin.close());
    //sio.printf("[out] close status : %d\n",sout.close());

    return 0;
}
