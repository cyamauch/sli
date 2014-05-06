/*
 * shallow copy, deep copy �Υƥ��ȤΤ���Υ�����
 */
#include <sli/stdstreamio.h>
#include <sli/mdarray.h>

using namespace sli;

static void func0( const mdarray &obj )
{
    stdstreamio sio;
    mdarray my_arr0;
    mdarray my_arr1;
    mdarray my_arr2;

    sio.printf("func0: start\n");

    /* ����� shallow copy �ˤʤ� */
    my_arr0 = obj;
    sio.printf("func0: my_arr0 total length=%zu (1)\n", my_arr0.length());

    /* shallow copy ������󥻥�ˤʤꡤ���� shallow copy ������ */
    my_arr0 = obj;
    sio.printf("func0: my_arr0 total length=%zu (2)\n", my_arr0.length());

    /* ������ obj �����ɤ߼��Τ���Υ��ɥ쥹�򥲥åȤ��Ƥ� */
    /* shallow copy ������󥻥�ˤʤꡤdeep copy ������ */
    obj.data_ptr_cs();
    //my_arr0.data_ptr_cs();

    /* ����� shallow copy �ˤʤ� */
    my_arr1 = obj;
    sio.printf("func0: my_arr1 total length=%zu (3)\n", my_arr1.length());

    /* ����� deep copy �ˤʤ� */
    my_arr2 = obj;
    sio.printf("func0: my_arr1 total length=%zu (4)\n", my_arr2.length());

    obj.dprint();
    my_arr0.dprint();
    my_arr1.dprint();
    my_arr2.dprint();

    sio.printf("func0: end\n");
    sio.printf("\n");

    return;
}

static void func1( mdarray &obj )
{
    stdstreamio sio;
    mdarray my_arr;

    sio.printf("func1: start\n");

    my_arr = obj;
    sio.printf("func1: my_arr total length=%zu\n", my_arr.length());

    sio.printf("func1: end\n");
    sio.printf("\n");

    return;
}

static void func2( const mdarray *obj )
{
    stdstreamio sio;
    mdarray my_arr;

    sio.printf("func2: start\n");

    my_arr = *obj;
    sio.printf("func2: my_arr total length=%zu\n", my_arr.length());

    sio.printf("func2: end\n");
    sio.printf("\n");

    return;
}

static void func3( mdarray *obj )
{
    stdstreamio sio;
    mdarray my_arr;

    sio.printf("func3: start\n");

    my_arr = *obj;
    sio.printf("func3: my_arr total length=%zu\n", my_arr.length());

    sio.printf("func3: end\n");
    sio.printf("\n");

    return;
}

static void func4( const mdarray obj )
{
    stdstreamio sio;
    mdarray my_arr;

    sio.printf("func4: start\n");

    /* func4 ���ƤФ줿�����ǥ��ԡ��������줽���ʵ������뤬�� */
    /* �����ǤϤ���ƥ��ԡ���������� */
    my_arr = obj;
    sio.printf("func4: my_arr total length=%zu\n", my_arr.length());

    sio.printf("func4: end\n");
    sio.printf("\n");

    return;
}

static void func5( mdarray obj )
{
    stdstreamio sio;
    mdarray my_arr;

    sio.printf("func5: start\n");

    /* func4 ��Ʊ�� */
    /* �����ǤϤ���ƥ��ԡ���������� */
    my_arr = obj;
    sio.printf("func5: my_arr total length=%zu\n", my_arr.length());

    sio.printf("func5: end\n");
    sio.printf("\n");

    return;
}

int main( int argc, char *argv[] )
{
    stdstreamio sio;
    mdarray my_arr0(INT_ZT,true);
    mdarray my_arr1;

    sio.printf("*** 83_shallow_copy_sample start ***\n");
    sio.printf("func0: start\n");

    my_arr0.resize_2d(8,4);

    /* deep copy */
    my_arr1 = my_arr0;

    /* shallow copy */
    my_arr1 = my_arr0.sectionf("*,0:1");

    sio.printf("\n");

    func0(my_arr0.sectionf("*,0:1"));

    /* ����ϥ���ѥ��뤬�̤�ʤ� */
    /* �ƥ�ݥ�ꥪ�֥������Ȥ� non-const �Ǥϼ������ʤ� */
    // func1(my_arr0.sectionf("*,0:1"));

    /* ����ϥ���ѥ���Ǥ��뤬�ٹ𤬤Ǥ롥���٤��Ǥʤ� */
    //func2(&(my_arr0.sectionf("*,0:1")));

    /* ����ϥ���ѥ���Ǥ��뤬�ٹ𤬤Ǥ롥���٤��Ǥʤ� */
    //func3(&(my_arr0.sectionf("*,0:1")));

    func4(my_arr0.sectionf("*,0:1"));

    func5(my_arr0.sectionf("*,0:1"));

    sio.printf("*** 83_shallow_copy_sample end ***\n");
    sio.printf("\n");

    return 0;
}
