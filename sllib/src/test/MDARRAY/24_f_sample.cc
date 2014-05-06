/**
  example code
  mdarrayクラス: float &f( ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF, ssize_t idx2 = MDARRAY_INDEF )
                 double &d( ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF, ssize_t idx2 = MDARRAY_INDEF )
		 long double &ld(ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF, ssize_t idx2 = MDARRAY_INDEF)
		 unsigned char &c(ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF, ssize_t idx2 = MDARRAY_INDEF)
		 short &s( ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF, ssize_t idx2 = MDARRAY_INDEF )
		 int &i( ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF, ssize_t idx2 = MDARRAY_INDEF )
		 long &l( ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF, ssize_t idx2 = MDARRAY_INDEF )
		 long long &ll( ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF, ssize_t idx2 = MDARRAY_INDEF )
		 int16_t &i16( ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF, ssize_t idx2 = MDARRAY_INDEF )
		 int32_t &i32( ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF, ssize_t idx2 = MDARRAY_INDEF )
		 int64_t &i64( ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF, ssize_t idx2 = MDARRAY_INDEF )
		 size_t &z( ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF, ssize_t idx2 = MDARRAY_INDEF )
		 ssize_t &sz( ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF, ssize_t idx2 = MDARRAY_INDEF )
		 bool &b( ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF, ssize_t idx2 = MDARRAY_INDEF )
		 uintptr_t &p( ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF, ssize_t idx2 = MDARRAY_INDEF )
                 各要素の値を設定・取得
*/
#include <sli/stdstreamio.h>
#include <sli/mdarray.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;

    sio.printf("start\n");

    mdarray my_fmdarr(FLOAT_ZT, true);

    //float型の値を設定して、確認の為出力
    my_fmdarr.f(0) = 1000.1;
    my_fmdarr.f(1) = 2000.2;
    for (size_t i = 0; i < my_fmdarr.length(); i++){
        sio.printf("my_fmdarr value(%zu)... [%5.1f]\n", i, my_fmdarr.f(i));
    }

    //double型の値を設定して、確認の為出力
    mdarray mdarr2(DOUBLE_ZT, true);
    mdarr2.d(0) = 3.0;
    mdarr2.d(1) = 4.0;
    sio.printf("type:double\n");
    for (int i = 0; i < 2; i++){
        sio.printf("mdarr2 lvalue(%d)... [%ld]\n", i, mdarr2.lvalue(i));
    }

    //char型の値を設定して、確認の為出力
    mdarray mdarrchar(UCHAR_ZT, true);
    mdarrchar.c(0) = 1;
    mdarrchar.c(1) = 10;
    sio.printf("type:char\n");
    for (int i = 0; i < 2; i++){
        sio.printf("mdarrchar value(%d)... [%d]\n", i, (int)mdarrchar.c_cs(i));
    }

    //short型の値を設定して、確認の為出力
    mdarray mdarrshort(SHORT_ZT, true);
    mdarrshort.s(0) = 20;
    mdarrshort.s(1) = 300;
    sio.printf("type:short\n");
    for (int i = 0; i < 2; i++){
        sio.printf("mdarrshort value(%d)... [%hd]\n", i, mdarrshort.s_cs(i));
    }

    //int型の値を設定して、確認の為出力
    mdarray mdarrint(INT_ZT, true);
    mdarrint.i(0) = 400;
    mdarrint.i(1) = 5000;
    sio.printf("type:int\n");
    for (int i = 0; i < 2; i++){
        sio.printf("mdarrint value(%d)... [%d]\n", i, mdarrint.i_cs(i));
    }

    //long型の値を設定して、確認の為出力
    mdarray mdarrlong(LONG_ZT, true);
    mdarrlong.l(0) = 5000;
    mdarrlong.l(1) = 60000;
    sio.printf("type:long\n");
    for (int i = 0; i < 2; i++){
        sio.printf("mdarrlong value(%d)... [%ld]\n", i, mdarrlong.l_cs(i));
    }

    //long long型の値を設定して、確認の為出力
    mdarray mdarrlonglong(LLONG_ZT, true);
    mdarrlonglong.ll(0) = 60000;
    mdarrlonglong.ll(1) = 70000000;
    sio.printf("type:longlong\n");
    for (int i = 0; i < 2; i++){
        sio.printf("mdarrlonglong value(%d)... [%lld]\n", i, mdarrlonglong.ll_cs(i));
    }

    //i16型の値を設定して、確認の為出力
    mdarray mdarri16(INT16_ZT, true);
    mdarri16.i16(0) = 120;
    mdarri16.i16(1) = -120;
    sio.printf("type:i16\n");
    for (int i = 0; i < 2; i++){
        sio.printf("mdarri16 value(%d)... [%d]\n", i, (int)mdarri16.i16_cs(i));
    }

    //i32型の値を設定して、確認の為出力
    mdarray mdarri32(INT32_ZT, true);
    mdarri32.i32(0) = 65535;
    mdarri32.i32(1) = -65535;
    sio.printf("type:i32\n");
    for (int i = 0; i < 2; i++){
        sio.printf("mdarri32 value(%d)... [%d]\n", i, (int)mdarri32.i32_cs(i));
    }

    //bool型の値を設定して、確認の為出力
    mdarray mdarrb(BOOL_ZT, true);
    mdarrb.b(0) = 1;
    mdarrb.b(1) = 0;
    mdarrb.b(2) = true;
    mdarrb.b(3) = false;
    sio.printf("type:bool\n");
    for (int i = 0; i < 4; i++){
        sio.printf("mdarrb value(%d)... [%d]\n", i, (int)mdarrb.b_cs(i));
    }

    sio.printf("end\n");

    return 0;
}
