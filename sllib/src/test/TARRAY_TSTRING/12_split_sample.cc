/**
  example code
  tarray_tstring�N���X:tarray_tstring &split( const char *line, const char *delims,
				      bool zero_str,const char *quotations, int escape )
		       tarray_tstring &split( const char *line, const char *delims, bool zero_str = false )
                       ������𕪊���������z��֑��
*/
#include <sli/stdstreamio.h>
#include <sli/tarray_tstring.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;

    const char *line = "Fragrant olive is 'KINMOKUSEI'. It is good smell.";
    tarray_tstring my_arr;
    //line�̕�������󔒕����ŕ���
    my_arr.split(line, " ", false);
    for ( size_t i=0 ; i < my_arr.length() ; i++ ) {
        sio.printf("my_arr[%zu] ===> %s\n", i, my_arr.cstr(i));
    }

    return 0;
}

