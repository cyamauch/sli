/**
  example code
  tarray_tstring�N���X:tarray_tstring &tarray_tstring &replace( size_t idx1,size_t n1,
								const char *str, size_t n2 )
		       tarray_tstring &tarray_tstring &replace( size_t idx1, size_t n1,
								const tstring &str, size_t n2 )
    		       tarray_tstring &replacef( size_t idx1, size_t n1,
						 size_t n2, const char *format, ... )
		       tarray_tstring &tarray_tstring &vreplacef( size_t idx1, size_t n1,
						      size_t n2, const char *format, va_list ap )
                       �v�f�̒u��
*/
#include <sli/stdstreamio.h>
#include <sli/tarray_tstring.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;

    const char *tree[] = {"willow", "pine", "fir", NULL};
    const char *addTree[] = {"linden", "beech", "holly", NULL};
    tarray_tstring my_arr = tree;

    //my_arr��1�Ԗڂ���1�̗v�f��"linden"�Œu��
    my_arr.replace(1, 1, "linden", 1);
    for (size_t i = 0; i < my_arr.length(); i++){
        sio.printf("my_arr[%zu] = %s\n", i, my_arr.cstr(i));
    }

    //replace()�ɂ��my_arr�̃T�C�Y�����g��
    my_arr.replace(4, 0, addTree, 2);
    for (size_t i = 0; i < my_arr.length(); i++){
        sio.printf("my_arr[%zu] = %s\n", i, my_arr.cstr(i));
    }


    //���̃����o�֐��̃T���v��
    tarray_tstring tarr; 

    const char *hogeElement[] = {"HOGE", "hoge", "is", "Hoge",NULL};

    const tarray_tstring hogeArr(hogeElement);
    tarr.assign(hogeArr);

    for (size_t i = 0; i < tarr.length(); i++){
        sio.printf("tarr[%zu] = %s\n",i, tarr.cstr(i));
    }

    //#2 &replace( size_t idx1, size_t n1, const tstring &str, size_t n2 ),idx1�Ԗڂ���n1�̗v�f��el�ƒu������B
    const tstring str = "niginigi";
    tarr.replace(1, 3, str,1);
    sio.printf("tarr length = %zu\n", tarr.length());
    for (size_t i = 0; i < tarr.length(); i++){
        sio.printf("tarr[%zu] = %s\n",i, tarr.cstr(i));
    }

    return 0;
}

