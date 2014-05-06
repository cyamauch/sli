#include <sli/stdstreamio.h>
#include <sli/inetstreamio.h>
using namespace sli;

/**
 * @file   http_client.cc
 * @brief  HTTP-1.0 クライアントの簡単な例
 */

/*
 *  Example of simple HTTP-1.0 client
 */

int main( int argc, char *argv[] )
{
    int ret_status = -1;
    stdstreamio sio;
    inetstreamio netio;
    const char *base_url;
    const char *path;
    const char *line;

    /* connect */

    base_url = "http://www.ir.isas.jaxa.jp/";

    if ( netio.open("r+", base_url) < 0 ) {
        sio.eprintf("[ERROR] cannot open");
        goto quit;
    }


    /* send GET request */

    path = "/";

    netio.printf("GET %s HTTP/1.0\r\n", path);
    netio.printf("Connection: close\r\n");
    netio.printf("\r\n");

    netio.flush();


    /* recieve contents */

    while ( (line=netio.getline()) != NULL ) {
        sio.printf("%s", line);
    }

    netio.close();


    ret_status = 0;
 quit:
    return ret_status;
}

