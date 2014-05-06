/**
  example code
  mdarrayクラス: mdarray &reverse_endian( bool is_little_endian, 
					  ssize_t sz_type = 0 )
                 必要に応じたエンディアンの変換
*/
#include<stdio.h>
#include <sli/stdstreamio.h>
#include <sli/mdarray.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;
    stdstreamio fio;

    sio.printf("start\n");

    //現状確認
    int i = 1;
    if ( *((char *)&i) ) 
	sio.eprintf("This machine is little-endian\n");
    else if ( *((char *)&i + (sizeof(int) - 1)) ) 
	sio.eprintf("This machine is big-endian\n");
    else 
	sio.eprintf("This machine is unknown endian\n"); 

    size_t nx[] = {2, 2};
    mdarray my_mdarr(sizeof(int), true);
    my_mdarr.resize(nx,2,true);

    my_mdarr.i(0,0) = 0x00a;
    my_mdarr.i(1,0) = 0x014;
    my_mdarr.i(0,1) = 0x01e;
    my_mdarr.i(1,1) = 0x028;

    my_mdarr.reverse_endian(false);
    const void *mycarray_ptr = my_mdarr.data_ptr();

    sio.printf("*** output big-endian data ***\n"); 

    if ( fio.openf("w", "%s", "62_binary.dat") < 0){
     //エラー処理
    }

    if ( fio.write(mycarray_ptr, my_mdarr.byte_length()) < 0){
     //エラー処理
    }

    my_mdarr.reverse_endian(false);

    fio.close();

    return 0;
}
