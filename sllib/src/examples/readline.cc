#include <sli/termlineio.h>
#include <sli/stdstreamio.h>
#include <sli/tstring.h>
using namespace sli;

/**
 * @file   readline.cc
 * @brief  GNU readline を使ってターミナルからコマンド入力を受け付ける例
 */

/*
 * Example for reading commands on a terminal (using GNU readline)
 */

int main()
{
    stdstreamio sio;
    termlineio tin;
    tstring line;
    int cnt = 0;

    sio.printf("Type [Ctrl + D] to end.\n");
    tin.open("r");

    while ( (line=tin.setf_prompt("prompt[%d]> ",cnt).getline()) != NULL ) {
        line.rtrim("\n");                               /* remove '\n' */
        sio.printf("command = [%s]\n",line.cstr());
        cnt++;
    }

    //sio.printf("[in]  eof flag     : %d\n",tin.eof());
    //sio.printf("[in]  error flag   : %d\n",tin.error());
    //sio.printf("[in]  close status : %d\n",tin.close());

    return 0;
}
