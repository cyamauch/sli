/**
  example code
  tarray_tstring�N���X:tarray_tstring &assign( const char *el0, const char *el1, ... )
		       tarray_tstring &vassign( const char *el0, const char *el1, va_list ap )
		       tarray_tstring &assign( const char *const *elements )
		       tarray_tstring &assign( const char *const *elements, size_t n )
		       tarray_tstring &assign( const tarray_tstring &src, size_t idx2 = 0 )
		       tarray_tstring &assign( const tarray_tstring &src, size_t idx2, size_t n2 )
                       �I�u�W�F�N�g�̏�����
*/
#include <sli/stdstreamio.h>
#include <sli/tarray_tstring.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;

    tarray_tstring my_arr; 

    const char *tree[] = {"fir", "magnolia", "dogwood", NULL};
    const tarray_tstring myTree(tree);

    /* �z��myTree��1�Ԗڂ���2��my_arr�������� */
    my_arr.assign(myTree,1,2);
    sio.printf("*** my_arr length = %zu\n", my_arr.length());
    for (size_t i = 0; i < my_arr.length(); i++){
        sio.printf("*** my_arr[%zu] = %s\n", i, my_arr.cstr(i));
    }

    //���̃����o�֐��̃T���v��
    tarray_tstring hogeArr; 
    tarray_tstring piyoArr;

    //#1 &assign( const char *el0, const char *el1, ... ),�����Ŏw�肵�������̔z����쐬���A�擪��菇�Ɉ���������Ŗ��߂�B
    sio.printf("*** &assign( const char *el0, const char *el1, ... ) sample ***\n");
    hogeArr.assign( "hoga", "huga", "hega", "hoge", NULL);
    sio.printf("hogeArr length = %zu\n", hogeArr.length());
    for (size_t i = 0; i < hogeArr.length(); i++){
        sio.printf("hogeArr[%zu] = %s\n",i, hogeArr.cstr(i));
    }

    //#3 &assign( const char *const *elements ),elements�Œ�`�����v�f���̔z����쐬����B
    sio.printf("*** &assign( const char *const *elements ) sample ***\n");
    const char *piyoElement[] = {"Piyo", "piyo", "Nyan","Goo", "Pau","Pou",NULL};
    hogeArr.assign(piyoElement);
    sio.printf("hogeArr length = %zu\n", hogeArr.length());
    for (size_t i = 0; i < hogeArr.length(); i++){
        sio.printf("hogeArr[%zu] = %s\n",i, hogeArr.cstr(i));
    }

    //#4 &assign( const char *const *elements, size_t n ),n�̔z����쐬��elements�Œ�`�����v�f�ŏ���������B
    //const char *piyoElement[] = {"Piyo", "piyo", "Nyan","Goo", "Pau","Pou",NULL};
    //��`����菭�Ȃ��ꍇ
    hogeArr.assign(piyoElement,3);
    sio.printf("��`����菭�Ȃ�**** hogeArr length = %zu\n", hogeArr.length());
    for (size_t i = 0; i < hogeArr.length(); i++){
        sio.printf("hogeArr[%zu] = %s\n",i, hogeArr.cstr(i));
    }

    //#5 &assign( const tarray_tstring &src, size_t idx2 = 0 ),�@n�̔z����쐬��elements�Œ�`�����v�f�ŏ���������B
    sio.printf("*** &assign( const tarray_tstring &src, size_t idx2 = 0 ) sample ***\n");
    const char *penElement[] = {"penkocyan", "penokun", "pentakun","penpen", "penbou",NULL};
    tarray_tstring penArr(penElement);
    for (size_t i = 0; i < penArr.length(); i++){
        sio.printf("penArr[%zu] = %s\n",i, penArr.cstr(i));
    }
    const char *pen2Element[] = {"penkichi","penmicyan",NULL};
    //NULL���Ȃ��ƁA"2 [main] assign_sample 1684 _cygtls::handle_exceptions: Error while dumping state (probably co
    //rrupted stack)"
    //const char *pen2Element[] = {"penkichi","penmicyan"};
    sio.printf("*** &assign( const tarray_tstring &src, size_t idx2, size_t n2  ) sample ***\n");
    const tarray_tstring penArr2(pen2Element);
    for (size_t i = 0; i < penArr2.length(); i++){
        sio.printf("penArr2[%zu] = %s\n",i, penArr2.cstr(i));
    }

    //#6 &assign( const tarray_tstring &src, size_t idx2, size_t n2)
    penArr.assign(penArr2,0,2);
    sio.printf("*** penArr length = %zu\n", penArr.length());
    for (size_t i = 0; i < penArr.length(); i++){
        sio.printf("*** penArr[%zu] = %s\n",i, penArr.cstr(i));
    }

    return 0;

}

