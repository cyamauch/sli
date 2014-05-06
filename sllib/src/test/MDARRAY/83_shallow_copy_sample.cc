/*
 * shallow copy, deep copy のテストのためのコード
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

    /* これは shallow copy になる */
    my_arr0 = obj;
    sio.printf("func0: my_arr0 total length=%zu (1)\n", my_arr0.length());

    /* shallow copy がキャンセルになり，再度 shallow copy が走る */
    my_arr0 = obj;
    sio.printf("func0: my_arr0 total length=%zu (2)\n", my_arr0.length());

    /* ここで obj から読み取りのためのアドレスをゲットしても */
    /* shallow copy がキャンセルになり，deep copy が走る */
    obj.data_ptr_cs();
    //my_arr0.data_ptr_cs();

    /* これは shallow copy になる */
    my_arr1 = obj;
    sio.printf("func0: my_arr1 total length=%zu (3)\n", my_arr1.length());

    /* これは deep copy になる */
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

    /* func4 が呼ばれた時点でコピー処理されそうな気がするが， */
    /* ここではじめてコピー処理される */
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

    /* func4 と同様 */
    /* ここではじめてコピー処理される */
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

    /* これはコンパイルが通らない */
    /* テンポラリオブジェクトは non-const では受けられない */
    // func1(my_arr0.sectionf("*,0:1"));

    /* これはコンパイルできるが警告がでる．やるべきでない */
    //func2(&(my_arr0.sectionf("*,0:1")));

    /* これはコンパイルできるが警告がでる．やるべきでない */
    //func3(&(my_arr0.sectionf("*,0:1")));

    func4(my_arr0.sectionf("*,0:1"));

    func5(my_arr0.sectionf("*,0:1"));

    sio.printf("*** 83_shallow_copy_sample end ***\n");
    sio.printf("\n");

    return 0;
}
