/**
  example code
  tarray_tstring�N���X:tarray_tstring &append( const char *str, size_t n )
	               tarray_tstring &append( const tstring &str, size_t n )
		       tarray_tstring &appendf( size_t n, const char *format, ... )
		       tarray_tstring &vappendf( size_t n, const char *format, va_list ap )
                       �v�f�̒ǉ�
*/
#include <sli/stdstreamio.h>
#include <sli/tarray_tstring.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;

    const char *mytree[] = {"maple", "larch", NULL};
    tarray_tstring my_arr(mytree);

    const tstring mytrr = "gardenia";
    //my_arr��mytrr��2�ǉ�
    my_arr.append(mytrr,2);
    for (size_t i = 0; i < my_arr.length(); i++){
        sio.printf("my_arr[%zu] = %s\n", i, my_arr.cstr(i));
    }


    //���̃����o�֐��̃T���v��
    tarray_tstring tarr; 
    tarray_tstring tarr1;

    const char *hogeElement[] = {"HOGE", "hoge", "is", "Hoge",NULL};

    const tarray_tstring hogeArr(hogeElement);

    const char *element[] = {"abc", "def", NULL};
    tarr.append(element);
    sio.printf("tarr length = %zu\n", tarr.length());
    for (size_t i = 0; i < tarr.length(); i++){
        sio.printf("tarr[%zu] = %s\n",i, tarr.cstr(i));
    }

    //#1 &append( const char *str, size_t n  ),str��n�ǉ�����B
    tarr.append("anyway", 2);
    sio.printf("tarr length = %zu\n", tarr.length());
    for (size_t i = 0; i < tarr.length(); i++){
        sio.printf("tarr[%zu] = %s\n",i, tarr.cstr(i));
    }

    //#2 &append( const tstring &str, size_t n   ),tstring�^str��n�ǉ�����B
    const tstring hogeStr = "hoge";

    tarr.append(hogeStr, 2);
    sio.printf("tarr length = %zu\n", tarr.length());
    for (size_t i = 0; i < tarr.length(); i++){
        sio.printf("tarr[%zu] = %s\n",i, tarr.cstr(i));
    }

    //#3 &appendf( size_t n, const char *format, ...  ),...�Ŏw�肷�镶�����format�̏����ɂ��Ă͂߂čŌ��n�ǉ�����B
    tarr.appendf(2, "*** %s %s ***", "is", "grate");
    sio.printf("tarr length = %zu\n", tarr.length());
    for (size_t i = 0; i < tarr.length(); i++){
        sio.printf("tarr[%zu] = %s\n",i, tarr.cstr(i));
    }


    return 0;

}

