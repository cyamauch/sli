/**
  example code
  tarray_tstring�N���X:tarray_tstring &swap( tarray_tstring &sobj )
                       ������z��I�u�W�F�N�g�̓��ւ�
*/
#include <sli/stdstreamio.h>
#include <sli/tarray_tstring.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;

    const char *menu[] = {"rice ball", "sushi", "tofu", NULL};
    tarray_tstring myMenu_arr = menu;

    const char *tree[] = {"Pine", "Ginkgo", "Magnolia", NULL};
    tarray_tstring myTree_arr = tree;

    //myMenu_arr��myTree_arr�����ւ�
    myMenu_arr.swap(myTree_arr);
    for (size_t i = 0; i < myMenu_arr.length(); i++){
        sio.printf("myMenu_arr[%zu] = %s\n", i, myMenu_arr.cstr(i));
    }

    sio.printf("\n");
    for (size_t i = 0; i < myTree_arr.length(); i++){
        sio.printf("myTree_arr[%zu] = %s\n", i, myTree_arr.cstr(i));
    }


    //old
    tarray_tstring hogeArr;
    tarray_tstring piyoArr;
    const char *hogeElement[] = {"Hello", "Hoge", "World","Huge", "People",NULL};
    const char *piyoElement[] = {"Piyo", "piyo", "Nyan","Goo",NULL};

    hogeArr.init(hogeElement);
    piyoArr.init(piyoElement);
    sio.printf("init\n");
    for (size_t i = 0; i < hogeArr.length(); i++){
        sio.printf("hogeArr[%zu] = %s\n",i, hogeArr.cstr(i));
    }
    sio.printf("********************\n");
    for (size_t i = 0; i < piyoArr.length(); i++){
        sio.printf("piyoArr[%zu] = %s\n",i, piyoArr.cstr(i));
    }

    sio.printf("after swap\n");
    //�v�f���̈قȂ����ւ��̏ꍇ�A����ւ���v�f���ƂȂ�B
    hogeArr.swap(piyoArr);
    for (size_t i = 0; i < hogeArr.length(); i++){
        sio.printf("hogeArr[%zu] = %s\n",i, hogeArr.cstr(i));
    }
    sio.printf("********************\n");
    for (size_t i = 0; i < piyoArr.length(); i++){
        sio.printf("piyoArr[%zu] = %s\n",i, piyoArr.cstr(i));
    }
    return 0;

/* �v�f�����قȂ��Ă��n�j�B
*/
}

