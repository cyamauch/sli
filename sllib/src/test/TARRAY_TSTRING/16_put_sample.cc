/**
  example code
  tarray_tstring�N���X:tarray_tstring &put( size_t index, const char *el0, const char *el1, ... )
		       tarray_tstring &vput( size_t index, const char *el0, const char *el1, va_list ap )
		       tarray_tstring &put( size_t index, const char *const *elements )
		       tarray_tstring &put( size_t index, const char *const *elements, size_t n )
		       tarray_tstring &put( size_t index, const tarray_tstring &src, size_t idx2 )
		       tarray_tstring &put( size_t index, const tarray_tstring &src, size_t idx2, size_t n2 )
                       �C�ӂ̗v�f�ʒu�֕�������Z�b�g
*/
#include <sli/stdstreamio.h>
#include <sli/tarray_tstring.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;

    tarray_tstring my_arr;

    const char *mytree[] = {"maple", "larch", "camphor", NULL};
    const tarray_tstring tree_arr(mytree);

    //tree_arr��1�ԖځA2�Ԗڂ�my_arr��2�Ԗڂ�3�ԖڂɃZ�b�g
    my_arr.put(2, tree_arr, 1, 2 );
    for (size_t i = 0; i < my_arr.length(); i++){
        sio.printf("my_arr[%zu] = %s\n", i, my_arr.cstr(i));
    }

    // ���̃����o�֐��̃T���v��
    tarray_tstring hogeArr; 
    tarray_tstring piyoArr;
    tarray_tstring penArr;

    //#1 &put( size_t index, const char *el0, const char *el1, ...  )�̃T���v��
    const char *el0 = "hogo";
    const char *el1 = "nyan";
    const char *el2 = NULL;
    hogeArr.put(0, el0, el1, el2);
    for (size_t i = 0; i < hogeArr.length(); i++){
        sio.printf("hogeArr[%zu] = %s\n",i, hogeArr.cstr(i));
    }

    //#3 &put( size_t index, const char *const *elements )�̃T���v��
    const char *piyoElem[] = {"Piyo", "piyo", NULL};
    hogeArr.put(0, piyoElem );
    sio.printf("put( size_t index, const char *const *elements )�T���v��\n hogeArr length = %zu\n", hogeArr.length());
    for (size_t i = 0; i < hogeArr.length(); i++){
        sio.printf("hogeArr[%zu] = %s\n", i, hogeArr.cstr(i));
    }

    //#4 &put( size_t index, const tarray_tstring &src, size_t idx2 )�̃T���v��
    //const char *fooElement[] = {"foo", "bar", "hoo","var", "foobar", "hoovar"};
    const char *penElement[] = {"penkocyan", "penokun", "pentakun","penpen", "penbou", NULL};
    const tarray_tstring penArr2(penElement);
    hogeArr.put(10, penArr2, 2 );
    sio.printf("put( size_t index, const tarray_tstring &src, size_t idx2 )�T���v��\n hogeArr length = %zu\n", hogeArr.length());
    for (size_t i = 0; i < hogeArr.length(); i++){
        sio.printf("hogeArr[%zu] = %s\n",i, hogeArr.cstr(i));
    }

    //#5 &put( size_t index, const tarray_tstring &src, size_t idx2, size_t n2 )�̃T���v��
    hogeArr.put(3, penArr2, 1, 2 );
    sio.printf("put( size_t index, const tarray_tstring &src, size_t idx2, size_t n2 )�T���v��\n hogeArr length = %zu\n", hogeArr.length());
    for (size_t i = 0; i < hogeArr.length(); i++){
        sio.printf("hogeArr[%zu] = %s\n",i, hogeArr.cstr(i));
    }

    return 0;

}

