/**
  example code
  mdarrayクラス: mdarray &operator=(double v)
                 各要素の値を取得
*/
#include <sli/stdstreamio.h>
#include <sli/mdarray.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;

    sio.printf("start\n");
    //long long型の値を取得し出力
    sio.printf("type:long long\n");
    mdarray my_llmdarr(LLONG_ZT, true);
    my_llmdarr.ll(0) = 60000;
    my_llmdarr.ll(1) = 70000000;
    for (size_t i = 0; i < my_llmdarr.length(); i++){
        sio.printf("my_llmdarr value(%zu)... [%lld]\n", i, my_llmdarr.ll_cs(i));
    }

    //float型の値を取得し出力
    sio.printf("type:float\n");
    mdarray mdarrf(FLOAT_ZT, true);
    mdarrf.f(0) = 1000.0;
    mdarrf.f(1) = 2000.0;
    for (int i = 0; i < 2; i++){
        sio.printf("mdarrf f(%d)... [%g]\n", i, mdarrf.f(i));
    }


    //double型の値を取得し出力
    sio.printf("type:double\n");
    mdarray mdarr2(DOUBLE_ZT, true);
    mdarr2.d(0) = 3.0;
    mdarr2.d(1) = 4.0;
    for (int i = 0; i < 2; i++){
        sio.printf("mdarr2 lvalue(%d)... [%ld]\n", i, mdarr2.lvalue(i));
    }

    //char型の値を取得し出力
    sio.printf("type:char\n");
    mdarray mdarrchar(UCHAR_ZT, true);
    mdarrchar.c(0) = 1;
    mdarrchar.c(1) = 10;
    for (int i = 0; i < 2; i++){
        sio.printf("mdarrchar value(%d)... [%d]\n", i, (int)mdarrchar.c_cs(i));
    }

    //short型の値を取得し出力
    sio.printf("type:short\n");
    mdarray mdarrshort(SHORT_ZT, true);
    mdarrshort.s(0) = 20;
    mdarrshort.s(1) = 300;
    for (int i = 0; i < 2; i++){
        sio.printf("mdarrshort value(%d)... [%hd]\n", i, mdarrshort.s_cs(i));
    }

    //int型の値を取得し出力
    sio.printf("type:int\n");
    mdarray mdarrint(INT_ZT, true);
    mdarrint.i(0) = 400;
    mdarrint.i(1) = 5000;
    for (int i = 0; i < 2; i++){
        sio.printf("mdarrint value(%d)... [%d]\n", i, mdarrint.i_cs(i));
    }

    //long型の値を取得し出力
    sio.printf("type:long\n");
    mdarray mdarrlong(LONG_ZT, true);
    mdarrlong.l(0) = 5000;
    mdarrlong.l(1) = 60000;
    for (int i = 0; i < 2; i++){
        sio.printf("mdarrlong value(%d)... [%ld]\n", i, mdarrlong.l_cs(i));
    }

    //long double型の値を取得し出力
    sio.printf("type:long double\n");
    mdarray mdarrlonglong(LDOUBLE_ZT, true);
    mdarrlonglong.ld(0) = 60000;
    mdarrlonglong.ld(1) = 70000000;
    for (int i = 0; i < 2; i++){
        sio.printf("darrlonglong value(%d)... [%Lg]\n", i, mdarrlonglong.ld_cs(i));
    }

    //i16型の値を取得し出力
    sio.printf("type:i16\n");
    mdarray mdarri16(INT16_ZT, true);
    mdarri16.i16(0) = 120;
    mdarri16.i16(1) = -120;
    for (int i = 0; i < 2; i++){
        sio.printf("mdarri16 value(%d)... [%d]\n", i, (int)mdarri16.i16_cs(i));
    }

    //i32型の値を取得し出力
    sio.printf("type:i32\n");
    mdarray mdarri32(INT32_ZT, true);
    mdarri32.i32(0) = 65535;
    mdarri32.i32(1) = -65535;
    for (int i = 0; i < 2; i++){
        sio.printf("mdarri32 value(%d)... [%d]\n", i, (int)mdarri32.i32_cs(i));
    }

    //i64型の値を取得し出力
    sio.printf("type:i64\n");
    mdarray mdarri64(INT64_ZT, true);
    mdarri64.i64(0) = 429496729;
    mdarri64.i64(1) = -429496729;
    for (int i = 0; i < 2; i++){
        sio.printf("mdarri64 value(%d)... [%lld]\n", i, (long long)mdarri64.i64_cs(i));
    }

    //size_t型の値を取得し出力
    sio.printf("type:size_t\n");
    mdarray mdarrz(SIZE_ZT, true);
    mdarrz.z(0) = 48;
    for (int i = 0; i < 2; i++){
        sio.printf("mdarrz value(%d)... [%zu]\n", i, mdarrz.z_cs(i));
    }

    //ssize_t型の値を取得し出力
    sio.printf("type:ssize_t\n");
    mdarray mdarrsz(SSIZE_ZT, true);
    mdarrsz.sz(0) = 120;
    mdarrsz.sz(1) = -120;
    for (int i = 0; i < 2; i++){
        sio.printf("mdarrsz value(%d)... [%zd]\n", i, mdarrsz.sz_cs(i));
    }

    //bool型の値を取得し出力
    sio.printf("type:bool\n");
    mdarray mdarrb(BOOL_ZT, true);
    mdarrb.b(0) = 1;
    mdarrb.b(1) = 0;
    mdarrb.b(2) = true;
    mdarrb.b(3) = false;
    for (int i = 0; i < 4; i++){
        sio.printf("mdarrb value(%d)... [%d]\n", i, (int)mdarrb.b_cs(i));
    }

    sio.printf("end\n");

    return 0;
}
