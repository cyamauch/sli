#include <sli/stdstreamio.h>
#include <sli/tstring.h>
using namespace sli;

/*
 * main function
 */
int main( int argc, char *argv[] )
{
    stdstreamio sio;
    tstring mystr, ggstr;
    size_t c_pos, len;
    ssize_t r_pos;

    ggstr.assign("01234567890123456789012345678901234567890123456789012");
    mystr.assign("winnt 'program fils' [my 'document']  (my (mu\\'sic))");

    sio.printf("%s\n",ggstr.cstr());
    sio.printf("%s\n",mystr.cstr());
    
    c_pos = 0;
    while ( 0 <= (r_pos=mystr.find_quoted(c_pos, "'[]()", '\\', &len, &c_pos)) ) {
	tstring spc;
	spc.assign(' ',r_pos).append('~',len);
	sio.printf("%spos=%zd len=%zd\n",spc.cstr(),r_pos,len);
    }
    sio.printf("out of loop: pos=%zd\n",r_pos);
    sio.printf("\n");

    r_pos = 0;
    while ( 0 <= (r_pos=mystr.erase_quotes(r_pos, "'[]()", '\\', true, &len)) ) {
	sio.printf("pos=%zd newspn=%zd length()=%zd\n",
		   r_pos,len,mystr.length());
	sio.printf("%s\n",ggstr.cstr());
	sio.printf("%s\n",mystr.cstr());
    }
    sio.printf("out of loop: pos=%zd\n",r_pos);
    sio.printf("\n");

    mystr.assign("winnt 'program fils' [my 'document']  (my (mu\\'sic))");
    mystr.erase_quotes("'[]()", '\\', true, &len, true);
    sio.printf("%s\n",ggstr.cstr());
    sio.printf("%s\n",mystr.cstr());
    sio.printf("length()=%zd\n",mystr.length());
    sio.printf("\n");

    mystr.assign("erase \\! \\\\! \\\\\\! ESCAPE chars only");
    sio.printf("%s\n",ggstr.cstr());
    sio.printf("%s\n",mystr.cstr());
    sio.printf("length()=%zd\n",mystr.length());
    mystr.erase_quotes("", '\\', true, &len, true);
    sio.printf("%s\n",mystr.cstr());
    sio.printf("length()=%zd\n",mystr.length());
    sio.printf("\n");

    return 0;
}

