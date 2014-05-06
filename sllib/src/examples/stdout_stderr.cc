#include <sli/stdstreamio.h>
using namespace sli;

/**
 * @file   stdout_stderr.cc
 * @brief  ɸ����ϡ�ɸ�२�顼���Ϥؽ��Ϥ��뤿��Υ�����
 */

/*
 * Basic examples for STDOUT and STDERR
 */

int main( int argc, char *argv[] )
{
    stdstreamio sio;				/* object for stdin, stdout */

    sio.printf("This is stdout\n");		/* to stdout */
    sio.eprintf("This is stderr\n");		/* to stderr */

    return 0;
}
