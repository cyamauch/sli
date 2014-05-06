/**
  example code
  tarray_tstring�N���X:ssize_t copy( tarray_tstring &dest ) const
		       ssize_t copy( size_t index, tarray_tstring &dest ) const
    		       ssize_t copy( size_t index, size_t n, tarray_tstring &dest ) const
                       ������z���ʂ̕�����z��ɃR�s�[
*/
#include <sli/stdstreamio.h>
#include <sli/tarray_tstring.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;
    //new
    const char *menu[] = {"pickles", "natto", "tempura", "sukiyaki", NULL};
    tarray_tstring my_arr = menu;
    tarray_tstring dest_arr;

    //my_arr��2�Ԗڂ���2�̗v�f��dest_arr�ɃR�s�[
    my_arr.copy(2, 2, dest_arr);
    for (size_t i = 0; i < dest_arr.length(); i++){
        sio.printf("dest_arr[%zu] = %s\n", i, dest_arr.cstr(i));
    }

    sio.printf("*****************************\n");


    //�����ȃR�s�[��������
    //�S���R�s�[
    my_arr.copy(dest_arr);
    for (size_t i = 0; i < dest_arr.length(); i++){
        sio.printf("dest_arr[%zu] = %s\n", i, dest_arr.cstr(i));
    }
    sio.printf("*****************************\n");

    //ret = 1  �R�s�[���̗v�f���𒴂����w��(��)
    sio.printf("dest_arr = %zd\n", my_arr.copy(3, 2, dest_arr));
    for (size_t i = 0; i < dest_arr.length(); i++){
        sio.printf("dest_arr[%d] = %s\n", i, dest_arr.cstr(i));
    }

    sio.printf("*****************************\n");
    //ret = -1 �R�s�[���̗v�f���𒴂����w��(�J�n�ʒu)
    sio.printf("dest_arr = %zd\n", my_arr.copy(5, 1, dest_arr));
    for (size_t i = 0; i < dest_arr.length(); i++){
        sio.printf("dest_arr[%zu] = %s\n", i, dest_arr.cstr(i));
    }


    tarray_tstring hogeArr;
    tarray_tstring piyoArr;
    const char *hogeElement[] = {"Hello", "Hoge", "World","Huge", "People",NULL};
    const char *piyoElement[] = {"Piyo", "piyo", "Nyan","Goo", "Pau","Pou",NULL};
    hogeArr.init(hogeElement);
    piyoArr.init(piyoElement);
    sio.printf("hogeArr init\n");
    for (size_t i = 0; i < hogeArr.length(); i++){
        sio.printf("hogeArr[%zu] = %s\n",i, hogeArr.cstr(i));
    }

    int ret = 0;

    if ((ret = piyoArr.copy(hogeArr)) < 0){
    }else {
      sio.printf("**********\nret = %d\n",ret);
    }
    
    for (size_t i = 0; i < hogeArr.length(); i++){
        sio.printf("hogeArr[%zu] = %s\n",i, hogeArr.cstr(i));
    }

    if ((ret = piyoArr.copy(2,hogeArr)) < 0){
    }else {
      sio.printf("**********\nret = %d\n",ret);
    }
    
    for (size_t i = 0; i < hogeArr.length(); i++){
        sio.printf("hogeArr[%zu] = %s\n",i, hogeArr.cstr(i));
    }

    if ((ret = piyoArr.copy(2,1,hogeArr)) < 0){
    }else {
      sio.printf("**********\nret = %d\n",ret);
    }
    
    for (size_t i = 0; i < hogeArr.length(); i++){
        sio.printf("hogeArr[%zu] = %s\n",i, hogeArr.cstr(i));
    }

    if ((ret = hogeArr.copy(0,3,piyoArr)) < 0){
    }else {
      sio.printf("**********\nret = %d\n",ret);
    }
    
    for (size_t i = 0; i < piyoArr.length(); i++){
        sio.printf("piyoArr[%zu] = %s\n",i, piyoArr.cstr(i));
    }

}

