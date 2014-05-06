/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2013-05-14 17:50:41 cyamauch> */

/**
 * @file   mdarray_uintptr.cc
 * @brief  uintptr_t型の多次元配列を扱う mdarray_uintptr クラスのコード
 */

#define CLASS_NAME "mdarray_uintptr"

#include "config.h"

#include "mdarray_uintptr.h"
#include "tarray_tstring.h"

#include <stdarg.h>

#include "sli_funcs.h"

#include "private/s_memset.h"
#include "private/s_memmove.h"
#include "private/err_report.h"

#ifndef debug_report
//#define debug_report(xx) err_report(__FUNCTION__,"DEBUG",xx)
#define debug_report(xx)
#endif

namespace sli
{

/**
 * @brief  コンストラクタ
 *
 *  自身を自動リサイズモードで初期化します．
 *
 * @throw  内部バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 */
mdarray_uintptr::mdarray_uintptr()
{
    this->class_level_rec ++;
    this->mdarray::init((ssize_t)UINTPTR_ZT, true);
    return;
}

/**
 * @brief  コンストラクタ
 * 
 * @param  auto_r 自動リサイズモードで初期化を行う場合はtrue.
 * @throw  内部バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 */
mdarray_uintptr::mdarray_uintptr( bool auto_r )
{
    this->class_level_rec ++;
    this->mdarray::init((ssize_t)UINTPTR_ZT, auto_r);
    return;
}

/**
 * @brief  コンストラクタ
 *
 * @param      auto_r 自動リサイズモードで初期化を行う場合はtrue.
 * @param      extptr_address ユーザのポインタ変数のアドレス
 * @throw      内部バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 * @attention  ユーザのポインタ変数を登録すると shallow copy が機能しなくなり
 *             ます．
 */
mdarray_uintptr::mdarray_uintptr( bool auto_r, uintptr_t **extptr_address )
{
    this->class_level_rec ++;
    this->mdarray::init((ssize_t)UINTPTR_ZT, auto_r);
    this->mdarray::register_extptr((void *)extptr_address);
    return;
}

/**
 * @brief  コンストラクタ
 *
 * @param      auto_r 自動リサイズモードで初期化を行う場合はtrue.
 * @param      extptr2d_address ユーザのポインタ変数(2d用)のアドレス
 * @throw      内部バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 * @attention  ユーザのポインタ変数を登録すると shallow copy が機能しなくなり
 *             ます．
 */
mdarray_uintptr::mdarray_uintptr( bool auto_r, uintptr_t *const **extptr2d_address )
{
    this->class_level_rec ++;
    this->mdarray::init((ssize_t)UINTPTR_ZT, auto_r);
    this->mdarray::register_extptr_2d((void *)extptr2d_address);
    return;
}

/**
 * @brief  コンストラクタ
 *
 * @param      auto_r 自動リサイズモードで初期化を行う場合はtrue.
 * @param      extptr3d_address ユーザのポインタ変数(3d用)のアドレス
 * @throw      内部バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 * @attention  ユーザのポインタ変数を登録すると shallow copy が機能しなくなり
 *             ます．
 */
mdarray_uintptr::mdarray_uintptr( bool auto_r, uintptr_t *const *const **extptr3d_address )
{
    this->class_level_rec ++;
    this->mdarray::init((ssize_t)UINTPTR_ZT, auto_r);
    this->mdarray::register_extptr_3d((void *)extptr3d_address);
    return;
}

/**
 * @brief  コンストラクタ
 *
 *  ndimで次元数を，naxisx[]で各次元の要素数を指定して初期化を行います．
 *
 * @param  auto_r 自動リサイズモードで初期化を行う場合はtrue
 * @param  naxisx[] 各次元の要素数
 * @param  ndim 次元数
 * @throw  内部バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 */
mdarray_uintptr::mdarray_uintptr( bool auto_r, const size_t naxisx[], size_t ndim, bool init_buf )
{
    this->class_level_rec ++;
    this->mdarray::init((ssize_t)UINTPTR_ZT, auto_r, naxisx, ndim, init_buf);
    return;
}

/**
 * @brief  コンストラクタ
 *
 *  naxis0で1次元目の要素数を指定して初期化を行います．
 *
 * @param  auto_r 自動リサイズモードで初期化を行う場合はtrue
 * @param  naxis0 次元番号0（1次元目)の次元数
 * @throw  内部バッファの確保に失敗した場合．メモリ破壊を起こした場合．
*/
mdarray_uintptr::mdarray_uintptr( bool auto_r, size_t naxis0 )
{
    const size_t naxisx[] = {naxis0};
    this->class_level_rec ++;
    this->mdarray::init((ssize_t)UINTPTR_ZT, auto_r, naxisx, 1, true);
    return;
}

/**
 * @brief  コンストラクタ
 *
 *  naxis0で１次元目の要素数を，naxis1で2次元目の要素数を指定して初期化を
 *  行います．
 * 
 * @param  auto_r 自動リサイズモードで初期化を行う場合はtrue
 * @param  naxis0 次元番号0（1次元目)の要素数
 * @param  naxis1 次元番号1 （2次元目)の要素数
 * @throw  内部バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 */
mdarray_uintptr::mdarray_uintptr( bool auto_r, size_t naxis0, size_t naxis1 )
{
    const size_t naxisx[] = {naxis0, naxis1};
    this->class_level_rec ++;
    this->mdarray::init((ssize_t)UINTPTR_ZT, auto_r, naxisx, 2, true);
    return;
}

/**
 * @brief  コンストラクタ
 *
 *  naxis0で１次元目の要素数を，naxis1で2次元目の要素数を，naxis2で3次元目の
 *  要素数を指定して初期化を行います．
 * 
 * @param  auto_r 自動リサイズモードで初期化を行う場合はtrue
 * @param  naxis0 次元番号0(1次元目)の要素数
 * @param  naxis1 次元番号1(2次元目)の要素数
 * @param  naxis2 次元番号2(3次元目)の要素数
 * @throw  内部バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 */
mdarray_uintptr::mdarray_uintptr( bool auto_r, size_t naxis0, size_t naxis1, size_t naxis2 )
{
    const size_t naxisx[] = {naxis0, naxis1, naxis2};
    this->class_level_rec ++;
    this->mdarray::init((ssize_t)UINTPTR_ZT, auto_r, naxisx, 3, true);
    return;
}

/**
 * @brief  コンストラクタ
 *
 *  naxis0で1次元目の要素数を指定し，valsの内容で初期化を行います．
 * 
 * @param  auto_r 自動リサイズモードで初期化を行う場合はtrue
 * @param  naxis0 次元番号0(1次元目)の要素数
 * @param  vals[] コピー元となる配列のアドレス
 * @throw  内部バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 */
mdarray_uintptr::mdarray_uintptr( bool auto_r, size_t naxis0, 
			      const uintptr_t vals[] )
{
    this->class_level_rec ++;
    this->init(auto_r, naxis0, vals);
    return;
}

/**
 * @brief  コンストラクタ
 *
 *  naxis0で1次元目の要素数を，naxis1で2次元目の要素数を指定し，
 *  valsの内容で初期化を行います.
 * 
 * @param  auto_r 自動リサイズモードで初期化を行う場合はtrue
 * @param  naxis0 次元番号0(1次元目)の要素数
 * @param  naxis1 次元番号1(2次元目)の要素数
 * @param  vals[] コピー元となる配列のアドレス
 * @throw  内部バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 */
mdarray_uintptr::mdarray_uintptr( bool auto_r, size_t naxis0, size_t naxis1,
			      const uintptr_t vals[] )
{
    this->class_level_rec ++;
    this->init(auto_r, naxis0, naxis1, vals);
    return;
}

/**
 * @brief  コンストラクタ
 *
 *  naxis0で1次元目の要素数を，naxis1で2次元目の要素数を指定し，
 *  valsの内容で初期化を行います．
 * 
 * @param  auto_r 自動リサイズモードで初期化を行う場合はtrue
 * @param  naxis0 次元番号0(1次元目)の要素数
 * @param  naxis1 次元番号1(2次元目)の要素数
 * @param  vals[] コピー元となる配列のアドレス
 * @throw  内部バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 */
mdarray_uintptr::mdarray_uintptr( bool auto_r, size_t naxis0, size_t naxis1,
			      const uintptr_t *const vals[] )
{
    this->class_level_rec ++;
    this->init(auto_r, naxis0, naxis1, vals);
    return;
}

/**
 * @brief  コンストラクタ
 *
 *  naxis0で1次元目の要素数を，naxis2で2次元目の要素数を，naxis2で三次元目の
 *  要素数を指定し，valsの内容で初期化を行います．
 * 
 * @param  auto_r 自動リサイズモードで初期化を行う場合はtrue
 * @param  naxis0 次元番号0(1次元目)の要素数
 * @param  naxis1 次元番号1(2次元目)の要素数
 * @param  naxis2 次元番号2(3次元目)の要素数
 * @param  vals[] コピー元となる配列のアドレス
 * @throw  内部バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 */
mdarray_uintptr::mdarray_uintptr( bool auto_r, size_t naxis0, size_t naxis1, size_t naxis2,
			      const uintptr_t vals[] )
{
    this->class_level_rec ++;
    this->init(auto_r, naxis0, naxis1, naxis2, vals);
    return;
}

/**
 * @brief  コンストラクタ
 *
 *  naxis0で1次元目の要素数を，naxis2で2次元目の要素数を，naxis2で三次元目の
 *  要素数を指定し，valsの内容で初期化を行います．
 * 
 * @param  auto_r 自動リサイズモードで初期化を行う場合はtrue
 * @param  naxis0 次元番号0(1次元目)の要素数
 * @param  naxis1 次元番号1(2次元目)の要素数
 * @param  naxis2 次元番号2(3次元目)の要素数
 * @param  vals[] コピー元となる配列のアドレス
 * @throw  内部バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 */
mdarray_uintptr::mdarray_uintptr( bool auto_r, size_t naxis0, size_t naxis1, size_t naxis2,
			      const uintptr_t *const *const vals[] )
{
    this->class_level_rec ++;
    this->init(auto_r, naxis0, naxis1, naxis2, vals);
    return;
}

#if 0

/**
 * @brief  コンストラクタ
 *
 *  自身を引数として与えられた各要素データで初期化します．
 * 
 * @deprecated  使用不可．
 * @param  auto_r 自動リサイズモードで初期化を行う場合はtrue
 * @param  dim_info  各次元の要素数
 * @param  v0 値データ
 * @param  ... 値データの可変長引数リスト
 * @throw  内部バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 */
mdarray_uintptr::mdarray_uintptr( bool auto_r, const char *dim_info,
			      uintptr_t v0, ... )
{
    va_list ap;

    this->class_level_rec ++;

    va_start(ap, v0);
    try {
	this->vinit(auto_r,dim_info,v0,ap);
    }
    catch (...) {
	va_end(ap);
 	err_throw(__FUNCTION__,"FATAL","this->vinit()");
    }
    va_end(ap);

    return;
}
#endif

/**
 * @brief  コピーコンストラクタ
 *
 *  自身にobjの内容をコピーします．
 * 
 * @param  obj コピー元のオブジェクト
 * @throw  内部バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 */
mdarray_uintptr::mdarray_uintptr( const mdarray_uintptr &obj )
{
    this->class_level_rec ++;
    this->mdarray::init(obj);
    return;
}

/**
 * @brief  オブジェクトのコピーまたは代入
 *
 *  オブジェクトを初期化し，obj の内容をすべて(配列長や各種属性)を 自身にコピー
 *  します．obj が関数によって返されるテンポラリオブジェクトの場合，許可されて
 *  いれば shallow  copy が行なわれます．<br>
 *  objの型が自身の型とは異なる場合，型以外のすべて(配列長や各種属性)をコピー
 *  し，要素値のコピーはスカラー値の「=」演算子と同様の規則で行なわれます．
 *
 * @param      obj mdarray(継承) クラスのオブジェクト
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 *
 */
mdarray_uintptr &mdarray_uintptr::operator=(const mdarray &obj)
{
    debug_report("(const mdarray &obj)");
    if ( &obj == this ) return *this;

    if ( obj.size_type() == (ssize_t)UINTPTR_ZT ) {
	this->mdarray::init(obj);
    }
    else {

	if ( obj.dim_length() == 0 ) this->init();
	else {
	    this->init(true, obj.cdimarray(), obj.dim_length(), true);
	    this->ope_plus_equal(obj);
	}
	this->init_properties(obj);

    }

    return *this;
}

/**
 * @brief  自身の配列に，指定された配列を加算
 *
 *  演算子の右側(引数) で指定されたmdarray(継承) クラスのオブジェクトの配列を
 *  自身に加算します．<br>
 *  自身とは異なる型が設定されたオブジェクトが指定された場合，通常のスカラー
 *  演算の場合と同様の型変換処理が行なわれます．<br>
 * 「自動リサイズモード」が設定されている場合，各次元サイズに関して自身よりも
 *  obj の方が大きい場合，自動的にリサイズします．
 *
 * @param      obj mdarray(継承) クラスのオブジェクト
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 *
 */
mdarray_uintptr &mdarray_uintptr::operator+=(const mdarray &obj)
{
    debug_report("(const mdarray &obj)");
    this->ope_plus_equal(obj);
    return *this;
}

/**
 * @brief  自身の配列を，指定された配列で減算
 *
 *  演算子の右側(引数) で指定されたmdarray(継承) クラスのオブジェクトの配列を
 *  自身から減算します．<br>
 *  自身とは異なる型が設定されたオブジェクトが指定された場合，通常のスカラー
 *  演算の場合と同様の型変換処理が行なわれます．<br>
 *  「自動リサイズモード」が設定されている場合，各次元サイズに関して自身よりも
 *  obj の方が大きい場合，自動的にリサイズします．
 *
 * @param      obj mdarray(継承) クラスのオブジェクト
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 *
 */
mdarray_uintptr &mdarray_uintptr::operator-=(const mdarray &obj)
{
    debug_report("(const mdarray &obj)");
    this->ope_minus_equal(obj);
    return *this;
}

/**
 * @brief  自身の配列に，指定された配列を乗算
 *
 *  演算子の右側(引数) で指定されたmdarray(継承) クラスのオブジェクトの配列を
 *  自身に乗算します．<br>
 *  自身とは異なる型が設定されたオブジェクトが指定された場合，通常のスカラー
 *  演算の場合と同様の型変換処理が行なわれます．<br>
 *  「自動リサイズモード」が設定されている場合，各次元サイズに関して自身よりも
 *  obj の方が大きい場合，自動的にリサイズします．
 *
 * @param      obj mdarray(継承) クラスのオブジェクト
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 *
 */
mdarray_uintptr &mdarray_uintptr::operator*=(const mdarray &obj)
{
    debug_report("(const mdarray &obj)");
    this->ope_star_equal(obj);
    return *this;
}

/**
 * @brief  自身の配列を，指定された配列で除算
 *
 *  演算子の右側(引数) で指定されたmdarray(継承) クラスのオブジェクトの配列を
 *  自身から除算します．<br>
 *  自身とは異なる型が設定されたオブジェクトが指定された場合，通常のスカラー
 *  演算の場合と同様の型変換処理が行なわれます．<br>
 *  「自動リサイズモード」が設定されている場合，各次元サイズに関して自身よりも
 *  obj の方が大きい場合，自動的にリサイズします．
 *
 * @param      obj mdarray(継承) クラスのオブジェクト
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 *
 */
mdarray_uintptr &mdarray_uintptr::operator/=(const mdarray &obj)
{
    debug_report("(const mdarray &obj)");
    this->ope_slash_equal(obj);
    return *this;
}

/**
 * @brief  自身の配列に，スカラー値を代入
 *
 *  演算子の右側(引数) で指定された数値(スカラー値) を代入をします.
 *
 * @param      v スカラー値
 * @return     自身の参照
 *
 */
mdarray_uintptr &mdarray_uintptr::operator=(dcomplex v)
{
    debug_report("(dcomplex v)");
    this->ope_equal(DCOMPLEX_ZT,(const void *)&v);
    return *this;
}

/**
 * @brief  自身の配列に，スカラー値を代入
 *
 *  演算子の右側(引数) で指定された数値(スカラー値) を代入をします.
 *
 * @param      v スカラー値
 * @return     自身の参照
 *
 */
mdarray_uintptr &mdarray_uintptr::operator=(double v)
{
    debug_report("(double v)");
    this->ope_equal(DOUBLE_ZT,(const void *)&v);
    return *this;
}

/**
 * @brief  自身の配列に，スカラー値を代入
 *
 *  演算子の右側(引数) で指定された数値(スカラー値) を代入をします.
 *
 * @param      v スカラー値
 * @return     自身の参照
 *
 */
mdarray_uintptr &mdarray_uintptr::operator=(long long v)
{
    debug_report("(long long v)");
    this->ope_equal(LLONG_ZT,(const void *)&v);
    return *this;
}

/**
 * @brief  自身の配列に，スカラー値を代入
 *
 *  演算子の右側(引数) で指定された数値(スカラー値) を代入をします.
 *
 * @param      v スカラー値
 * @return     自身の参照
 *
 */
mdarray_uintptr &mdarray_uintptr::operator=(long v)
{
    debug_report("(long v)");
    this->ope_equal(LONG_ZT,(const void *)&v);
    return *this;
}

/**
 * @brief  自身の配列に，スカラー値を代入
 *
 *  演算子の右側(引数) で指定された数値(スカラー値) を代入をします.
 *
 * @param      v スカラー値
 * @return     自身の参照
 *
 */
mdarray_uintptr &mdarray_uintptr::operator=(int v)
{
    debug_report("(int v)");
    this->ope_equal(INT_ZT,(const void *)&v);
    return *this;
}

/**
 * @brief  自身の配列を，スカラー値で加算
 *
 *  演算子の右側(引数) で指定されたスカラー値を自身の要素すべてに対して加算
 *  します．<br>
 *  自身とは型が異なる引数の場合，通常のスカラー演算の場合と同様の型変換処理が
 *  行なわれます．
 *
 * @param      v スカラー値
 * @return     自身の参照
 *
 */
mdarray_uintptr &mdarray_uintptr::operator+=(dcomplex v)
{
    debug_report("(dcomplex v)");
    this->ope_plus_equal(DCOMPLEX_ZT,(const void *)&v);
    return *this;
}

/**
 * @brief  自身の配列を，スカラー値で加算
 *
 *  演算子の右側(引数) で指定されたスカラー値を自身の要素すべてに対して加算
 *  します．<br>
 *  自身とは型が異なる引数の場合，通常のスカラー演算の場合と同様の型変換処理が
 *  行なわれます．
 *
 * @param      v スカラー値
 * @return     自身の参照
 *
 */
mdarray_uintptr &mdarray_uintptr::operator+=(double v)
{
    debug_report("(double v)");
    this->ope_plus_equal(DOUBLE_ZT,(const void *)&v);
    return *this;
}

/**
 * @brief  自身の配列を，スカラー値で加算
 *
 *  演算子の右側(引数) で指定されたスカラー値を自身の要素すべてに対して加算
 *  します．<br>
 *  自身とは型が異なる引数の場合，通常のスカラー演算の場合と同様の型変換処理が
 *  行なわれます．
 *
 * @param      v スカラー値
 * @return     自身の参照
 *
 */
mdarray_uintptr &mdarray_uintptr::operator+=(long long v)
{
    debug_report("(long long v)");
    this->ope_plus_equal(LLONG_ZT,(const void *)&v);
    return *this;
}

/**
 * @brief  自身の配列を，スカラー値で加算
 *
 *  演算子の右側(引数) で指定されたスカラー値を自身の要素すべてに対して加算
 *  します．<br>
 *  自身とは型が異なる引数の場合，通常のスカラー演算の場合と同様の型変換処理が
 *  行なわれます．
 *
 * @param      v スカラー値
 * @return     自身の参照
 *
 */
mdarray_uintptr &mdarray_uintptr::operator+=(long v)
{
    debug_report("(long v)");
    this->ope_plus_equal(LONG_ZT,(const void *)&v);
    return *this;
}

/**
 * @brief  自身の配列を，スカラー値で加算
 *
 *  演算子の右側(引数) で指定されたスカラー値を自身の要素すべてに対して加算
 *  します．<br>
 *  自身とは型が異なる引数の場合，通常のスカラー演算の場合と同様の型変換処理が
 *  行なわれます．
 *
 * @param      v スカラー値
 * @return     自身の参照
 *
 */
mdarray_uintptr &mdarray_uintptr::operator+=(int v)
{
    debug_report("(int v)");
    this->ope_plus_equal(INT_ZT,(const void *)&v);
    return *this;
}

/**
 * @brief  自身の配列を，スカラー値で減算
 *
 *  自身の要素すべてに対して，演算子の右側(引数) で指定されたスカラー値で減算
 *  します．<br>
 *  自身とは型が異なる引数の場合，通常のスカラー演算の場合と同様の型変換処理が
 *  行なわれます．
 *
 * @param      v スカラー値
 * @return     自身の参照
 *
 */
mdarray_uintptr &mdarray_uintptr::operator-=(dcomplex v)
{
    debug_report("(dcomplex v)");
    this->ope_minus_equal(DCOMPLEX_ZT,(const void *)&v);
    return *this;
}

/**
 * @brief  自身の配列を，スカラー値で減算
 *
 *  自身の要素すべてに対して，演算子の右側(引数) で指定されたスカラー値で減算
 *  します．<br>
 *  自身とは型が異なる引数の場合，通常のスカラー演算の場合と同様の型変換処理が
 *  行なわれます．
 *
 * @param      v スカラー値
 * @return     自身の参照
 *
 */
mdarray_uintptr &mdarray_uintptr::operator-=(double v)
{
    debug_report("(double v)");
    this->ope_minus_equal(DOUBLE_ZT,(const void *)&v);
    return *this;
}

/**
 * @brief  自身の配列を，スカラー値で減算
 *
 *  自身の要素すべてに対して，演算子の右側(引数) で指定されたスカラー値で減算
 *  します．<br>
 *  自身とは型が異なる引数の場合，通常のスカラー演算の場合と同様の型変換処理が
 *  行なわれます．
 *
 * @param      v スカラー値
 * @return     自身の参照
 *
 */
mdarray_uintptr &mdarray_uintptr::operator-=(long long v)
{
    debug_report("(long long v)");
    this->ope_minus_equal(LLONG_ZT,(const void *)&v);
    return *this;
}

/**
 * @brief  自身の配列を，スカラー値で減算
 *
 *  自身の要素すべてに対して，演算子の右側(引数) で指定されたスカラー値で減算
 *  します．<br>
 *  自身とは型が異なる引数の場合，通常のスカラー演算の場合と同様の型変換処理が
 *  行なわれます．
 *
 * @param      v スカラー値
 * @return     自身の参照
 *
 */
mdarray_uintptr &mdarray_uintptr::operator-=(long v)
{
    debug_report("(long v)");
    this->ope_minus_equal(LONG_ZT,(const void *)&v);
    return *this;
}

/**
 * @brief  自身の配列を，スカラー値で減算
 *
 *  自身の要素すべてに対して，演算子の右側(引数) で指定されたスカラー値で減算
 *  します．<br>
 *  自身とは型が異なる引数の場合，通常のスカラー演算の場合と同様の型変換処理が
 *  行なわれます．
 *
 * @param      v スカラー値
 * @return     自身の参照
 *
 */
mdarray_uintptr &mdarray_uintptr::operator-=(int v)
{
    debug_report("(int v)");
    this->ope_minus_equal(INT_ZT,(const void *)&v);
    return *this;
}

/**
 * @brief  自身の配列に，スカラー値を乗算
 *
 *  演算子の右側(引数) で指定されたスカラー値を自身の要素すべてに対して乗算
 *  します．<br>
 *  自身とは型が異なる引数の場合，通常のスカラー演算の場合と同様の型変換処理が
 *  行なわれます．
 *
 * @param      v スカラー値
 * @return     自身の参照
 *
 */
mdarray_uintptr &mdarray_uintptr::operator*=(dcomplex v)
{
    debug_report("(dcomplex v)");
    this->ope_star_equal(DCOMPLEX_ZT,(const void *)&v);
    return *this;
}

/**
 * @brief  自身の配列に，スカラー値を乗算
 *
 *  演算子の右側(引数) で指定されたスカラー値を自身の要素すべてに対して乗算
 *  します．<br>
 *  自身とは型が異なる引数の場合，通常のスカラー演算の場合と同様の型変換処理が
 *  行なわれます．
 *
 * @param      v スカラー値
 * @return     自身の参照
 *
 */
mdarray_uintptr &mdarray_uintptr::operator*=(double v)
{
    debug_report("(double v)");
    this->ope_star_equal(DOUBLE_ZT,(const void *)&v);
    return *this;
}

/**
 * @brief  自身の配列に，スカラー値を乗算
 *
 *  演算子の右側(引数) で指定されたスカラー値を自身の要素すべてに対して乗算
 *  します．<br>
 *  自身とは型が異なる引数の場合，通常のスカラー演算の場合と同様の型変換処理が
 *  行なわれます．
 *
 * @param      v スカラー値
 * @return     自身の参照
 *
 */
mdarray_uintptr &mdarray_uintptr::operator*=(long long v)
{
    debug_report("(long long v)");
    this->ope_star_equal(LLONG_ZT,(const void *)&v);
    return *this;
}

/**
 * @brief  自身の配列に，スカラー値を乗算
 *
 *  演算子の右側(引数) で指定されたスカラー値を自身の要素すべてに対して乗算
 *  します．<br>
 *  自身とは型が異なる引数の場合，通常のスカラー演算の場合と同様の型変換処理が
 *  行なわれます．
 *
 * @param      v スカラー値
 * @return     自身の参照
 *
 */
mdarray_uintptr &mdarray_uintptr::operator*=(long v)
{
    debug_report("(long v)");
    this->ope_star_equal(LONG_ZT,(const void *)&v);
    return *this;
}

/**
 * @brief  自身の配列に，スカラー値を乗算
 *
 *  演算子の右側(引数) で指定されたスカラー値を自身の要素すべてに対して乗算
 *  します．<br>
 *  自身とは型が異なる引数の場合，通常のスカラー演算の場合と同様の型変換処理が
 *  行なわれます．
 *
 * @param      v スカラー値
 * @return     自身の参照
 *
 */
mdarray_uintptr &mdarray_uintptr::operator*=(int v)
{
    debug_report("(int v)");
    this->ope_star_equal(INT_ZT,(const void *)&v);
    return *this;
}

/**
 * @brief  自身の配列を，スカラー値で除算
 *
 *  自身の要素すべてに対して，演算子の右側(引数) で指定されたスカラー値で除算
 *  します．<br>
 *  自身とは型が異なる引数の場合，通常のスカラー演算の場合と同様の型変換処理が
 *  行なわれます．
 *
 * @param      v スカラー値
 * @return     自身の参照
 *
 */
mdarray_uintptr &mdarray_uintptr::operator/=(dcomplex v)
{
    debug_report("(dcomplex v)");
    this->ope_slash_equal(DCOMPLEX_ZT,(const void *)&v);
    return *this;
}

/**
 * @brief  自身の配列を，スカラー値で除算
 *
 *  自身の要素すべてに対して，演算子の右側(引数) で指定されたスカラー値で除算
 *  します．<br>
 *  自身とは型が異なる引数の場合，通常のスカラー演算の場合と同様の型変換処理が
 *  行なわれます．
 *
 * @param      v スカラー値
 * @return     自身の参照
 *
 */
mdarray_uintptr &mdarray_uintptr::operator/=(double v)
{
    debug_report("(double v)");
    this->ope_slash_equal(DOUBLE_ZT,(const void *)&v);
    return *this;
}

/**
 * @brief  自身の配列を，スカラー値で除算
 *
 *  自身の要素すべてに対して，演算子の右側(引数) で指定されたスカラー値で除算
 *  します．<br>
 *  自身とは型が異なる引数の場合，通常のスカラー演算の場合と同様の型変換処理が
 *  行なわれます．
 *
 * @param      v スカラー値
 * @return     自身の参照
 *
 */
mdarray_uintptr &mdarray_uintptr::operator/=(long long v)
{
    debug_report("(long long v)");
    this->ope_slash_equal(LLONG_ZT,(const void *)&v);
    return *this;
}

/**
 * @brief  自身の配列を，スカラー値で除算
 *
 *  自身の要素すべてに対して，演算子の右側(引数) で指定されたスカラー値で除算
 *  します．<br>
 *  自身とは型が異なる引数の場合，通常のスカラー演算の場合と同様の型変換処理が
 *  行なわれます．
 *
 * @param      v スカラー値
 * @return     自身の参照
 *
 */
mdarray_uintptr &mdarray_uintptr::operator/=(long v)
{
    debug_report("(long v)");
    this->ope_slash_equal(LONG_ZT,(const void *)&v);
    return *this;
}

/**
 * @brief  自身の配列を，スカラー値で除算
 *
 *  自身の要素すべてに対して，演算子の右側(引数) で指定されたスカラー値で除算
 *  します．<br>
 *  自身とは型が異なる引数の場合，通常のスカラー演算の場合と同様の型変換処理が
 *  行なわれます．
 *
 * @param      v スカラー値
 * @return     自身の参照
 *
 */
mdarray_uintptr &mdarray_uintptr::operator/=(int v)
{
    debug_report("(int v)");
    this->ope_slash_equal(INT_ZT,(const void *)&v);
    return *this;
}

/**
 * @brief  オブジェクトの初期化
 *
 *  自身の配列・属性を初期化します．<br>
 *  配列サイズ 0 としてオブジェクトを初期化します．
 *
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 *
 */
mdarray_uintptr &mdarray_uintptr::init()
{
    debug_report("()");
    this->mdarray::init();
    return *this;
}

/**
 * @brief  オブジェクトの初期化
 *
 *  自身の配列・属性を初期化します．
 *
 * @param      auto_resize 自動リサイズモードに設定する場合は true
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 *
 */
mdarray_uintptr &mdarray_uintptr::init( bool auto_resize )
{
    this->mdarray::init();
    this->mdarray::set_auto_resize(auto_resize);
    return *this;
}

/**
 * @brief  オブジェクトの初期化 (n次元)
 *
 *  自身の配列・属性を初期化します．<br>
 *  ndim で次元数，naxisx[] で各次元の要素数を指定します．
 *
 * @param      auto_resize 自動リサイズモードに設定する場合は true
 * @param      ndim 配列次元数
 * @param      naxisx[] 各次元の要素数
 * @param      init_buf 要素値をデフォルト値で埋めるなら true
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 *
 */
mdarray_uintptr &mdarray_uintptr::init( bool auto_resize, const size_t naxisx[], size_t ndim, bool init_buf )
{
    this->mdarray::init((ssize_t)UINTPTR_ZT, auto_resize, naxisx, ndim, init_buf);
    return *this;
}

/**
 * @brief  配列の初期化 (1次元)
 *
 *  自身の配列を初期化します．<br>
 *  naxis0 で 1 次元目の要素数を指定し，1 次元の配列を持つオブジェクトを
 *  作成します．
 *
 * @param      auto_resize 自動リサイズモードに設定する場合は true
 * @param      naxis0 次元番号0 の次元(1 次元目) の要素数
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 *
 */
mdarray_uintptr &mdarray_uintptr::init( bool auto_resize, size_t naxis0 )
{
    const size_t naxisx[] = {naxis0};
    this->mdarray::init((ssize_t)UINTPTR_ZT, auto_resize, naxisx, 1, true);
    return *this;
}

/**
 * @brief  配列の初期化 (2次元)
 *
 *  自身の配列を初期化します．<br>
 *  naxis0 で 1 次元目の要素数，naxis1 で 2 次元目の要素数指定し，2 次元の配列
 *  を持つオブジェクトを作成します．
 *
 * @param      auto_resize 自動リサイズモードに設定する場合は true
 * @param      naxis0 次元番号0 の次元(1 次元目) の要素数
 * @param      naxis1 次元番号1 の次元(2 次元目) の要素数
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 *
 */
mdarray_uintptr &mdarray_uintptr::init( bool auto_resize, size_t naxis0, size_t naxis1 )
{
    const size_t naxisx[] = {naxis0, naxis1};
    this->mdarray::init((ssize_t)UINTPTR_ZT, auto_resize, naxisx, 2, true);
    return *this;
}

/**
 * @brief  配列の初期化 (3次元)
 *
 *  自身の配列を初期化します．<br>
 *  naxis0 で 1 次元目の要素数，naxis1 で 2 次元目の要素数，naxis2 で 3 次元目
 *  の要素数を指定し，3 次元の配列を持つオブジェクトを作成します．
 *
 * @param      auto_resize 自動リサイズモードに設定する場合は true
 * @param      naxis0 次元番号0 の次元(1 次元目) の要素数
 * @param      naxis1 次元番号1 の次元(2 次元目) の要素数
 * @param      naxis2 次元番号2 の次元(3 次元目) の要素数
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 *
 */
mdarray_uintptr &mdarray_uintptr::init( bool auto_resize, size_t naxis0, size_t naxis1, size_t naxis2 )
{
    const size_t naxisx[] = {naxis0, naxis1, naxis2};
    this->mdarray::init((ssize_t)UINTPTR_ZT, auto_resize, naxisx, 3, true);
    return *this;
}

/**
 * @brief  コンストラクタ，あるいは init() の引数の次元情報を parse (未使用)
 * 
 * @deprecated  未使用
 * @param   dim_info 各次元の要素数の情報
 * @return  設定された次元の数
 * @throw   内部バッファの確保に失敗した場合
 * @note    このメンバ関数はprotectedです．<br>
 *          3次元までしかサポートしていません.
 */
size_t mdarray_uintptr::parse_dim_info( const char *dim_info )
{
    size_t i;
    tarray_tstring arr0;
    this->dinfo_dlen[0] = 0;
    this->dinfo_dlen[1] = 0;
    this->dinfo_dlen[2] = 0;
    arr0.split(dim_info,", ",false);
    /* 3次元までしかサポートしない */
    for ( i=0 ; i < arr0.length() && i < 3 ; i++ ) {
	this->dinfo_dlen[i] = arr0[i].atoll();
	if ( this->dinfo_dlen[i] == 0 ) break;
    }
    return i;
}

/**
 * @brief  配列の初期化
 *
 *  naxis0で1次元目の要素数を指定し，valsの内容で初期化を行います．
 *
 * @param      auto_resize 自動リサイズモードに設定する場合は true
 * @param      naxis0 次元番号0 の次元(1 次元目) の要素数
 * @param      vals コピー元となる配列のアドレス
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 *
 */
mdarray_uintptr &mdarray_uintptr::init( bool auto_resize, size_t naxis0, 
				    const uintptr_t vals[] )
{
    const size_t naxisx[] = {naxis0};
    this->mdarray::init((ssize_t)UINTPTR_ZT, auto_resize, naxisx, 1, false);
    this->mdarray::putdata((const void *)vals, this->byte_length());
    return *this;
}

/**
 * @brief  配列の初期化
 *
 *  naxis0で1次元目の要素数を，naxis1で2次元目の要素数を指定し，
 *  valsの内容で初期化を行います.
 *
 * @param      auto_resize 自動リサイズモードに設定する場合は true
 * @param      naxis0 次元番号0 の次元(1 次元目) の要素数
 * @param      naxis1 次元番号1 の次元(2 次元目) の要素数
 * @param      vals コピー元となる配列のアドレス
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 *
 */
mdarray_uintptr &mdarray_uintptr::init( bool auto_resize, size_t naxis0, size_t naxis1, 
				    const uintptr_t vals[] )
{
    const size_t naxisx[] = {naxis0, naxis1};
    this->mdarray::init((ssize_t)UINTPTR_ZT, auto_resize, naxisx, 2, false);
    this->mdarray::putdata((const void *)vals, this->byte_length());
    return *this;
}

/**
 * @brief  配列の初期化
 *
 *  naxis0で1次元目の要素数を，naxis1で2次元目の要素数を指定し，
 *  valsの内容で初期化を行います．
 *
 * @param      auto_resize 自動リサイズモードに設定する場合は true
 * @param      naxis0 次元番号0 の次元(1 次元目) の要素数
 * @param      naxis1 次元番号1 の次元(2 次元目) の要素数
 * @param      vals コピー元となるポインタ配列のアドレス
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 *
 */
mdarray_uintptr &mdarray_uintptr::init( bool auto_resize, size_t naxis0, size_t naxis1, 
				    const uintptr_t *const vals[] )
{
    const size_t naxisx[] = {naxis0, naxis1};
    size_t blen0, i;
    char *p_dst;
    /* */
    this->mdarray::init((ssize_t)UINTPTR_ZT, auto_resize, naxisx, 2, false);
    /* */
    blen0 = this->byte_length(0);
    p_dst = (char *)(this->mdarray::data_ptr());
    for ( i=0 ; i < naxis1 ; i++ ) {
	const void *p_src = (const void *)(vals[i]);
	if ( p_src == NULL ) break;
	s_memmove(p_dst, p_src, blen0);
	p_dst += blen0;
    }
    if ( i < naxis1 ) s_memset(p_dst, 0, blen0 * (naxis1 - i), 0);

    return *this;
}

/**
 * @brief  配列の初期化
 *
 *  naxis0で1次元目の要素数を，naxis2で2次元目の要素数を，naxis2で三次元目の
 *  要素数を指定し，valsの内容で初期化を行います．
 *
 * @param      auto_resize 自動リサイズモードに設定する場合は true
 * @param      naxis0 次元番号0 の次元(1 次元目) の要素数
 * @param      naxis1 次元番号1 の次元(2 次元目) の要素数
 * @param      naxis2 次元番号2 の次元(3 次元目) の要素数
 * @param      vals コピー元となる配列のアドレス
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 *
 */
mdarray_uintptr &mdarray_uintptr::init( bool auto_resize, size_t naxis0, size_t naxis1, size_t naxis2, 
				    const uintptr_t vals[] )
{
    const size_t naxisx[] = {naxis0, naxis1, naxis2};
    this->mdarray::init((ssize_t)UINTPTR_ZT, auto_resize, naxisx, 3, false);
    this->mdarray::putdata((const void *)vals, this->byte_length());
    return *this;
}

/**
 * @brief  配列の初期化
 *
 *  naxis0で1次元目の要素数を，naxis2で2次元目の要素数を，naxis2で三次元目の
 *  要素数を指定し，valsの内容で初期化を行います．
 *
 * @param      auto_resize 自動リサイズモードに設定する場合は true
 * @param      naxis0 次元番号0 の次元(1 次元目) の要素数
 * @param      naxis1 次元番号1 の次元(2 次元目) の要素数
 * @param      naxis2 次元番号2 の次元(3 次元目) の要素数
 * @param      vals コピー元となるポインタ配列のアドレス
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 *
 */
mdarray_uintptr &mdarray_uintptr::init( bool auto_resize, size_t naxis0, size_t naxis1, size_t naxis2, 
				    const uintptr_t *const *const vals[] )
{
    const size_t naxisx[] = {naxis0, naxis1, naxis2};
    size_t blen0, i, j;
    char *p_dst;
    /* */
    this->mdarray::init((ssize_t)UINTPTR_ZT, auto_resize, naxisx, 3, false);
    /* */
    blen0 = this->byte_length(0);
    p_dst = (char *)(this->mdarray::data_ptr());
    for ( i=0 ; i < naxis2 ; i++ ) {		/* layer */
	j = 0;					/* row */
	if ( vals[i] != NULL ) {
	    for ( ; j < naxis1 ; j++ ) {
		const void *p_src = (const void *)(vals[i][j]);
		if ( p_src == NULL ) break;
		s_memmove(p_dst, p_src, blen0);
		p_dst += blen0;
	    }
	}
	if ( j < naxis1 ) {
	    s_memset(p_dst, 0, blen0 * (naxis1 - j), 0);
	    p_dst += blen0 * (naxis1 - j);
	}
    }

    return *this;
}

#if 0

/**
 * @brief  配列の初期化
 *
 *  自身を引数として与えられた各要素データで初期化します.
 * 
 * @deprecated  未使用
 * @param   auto_resize 自動リサイズモードで初期化を行う場合はtrue
 * @param   dim_info 各次元の要素数
 * @param   v0 値データ
 * @param   ... 値データの各要素値
 * 
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 */
mdarray_uintptr &mdarray_uintptr::init( bool auto_resize, const char *dim_info, uintptr_t v0, ... )
{
    va_list ap;
    va_start(ap, v0);
    try {
	this->vinit(auto_resize, dim_info, v0, ap);
    }
    catch (...) {
	va_end(ap);
 	err_throw(__FUNCTION__,"FATAL","failed this->init()");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  配列の初期化
 *
 *  自身を引数として与えられた各要素データで初期化します.
 * 
 * @deprecated  未使用
 * @param   auto_resize 自動リサイズモードで初期化を行う場合はtrue
 * @param   dim_info 各次元の要素数
 * @param   v0 値データ
 * @param   ap 値データの可変長引数リスト
 * 
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 */
mdarray_uintptr &mdarray_uintptr::vinit( bool auto_resize, const char *dim_info, uintptr_t v0, va_list ap )
{
    size_t ndim = this->parse_dim_info(dim_info);
    if ( ndim < 1 ) {
	this->mdarray::init((ssize_t)UINTPTR_ZT, auto_resize);
    }
    else {
	size_t i;
	this->mdarray::init((ssize_t)UINTPTR_ZT, auto_resize, this->dinfo_dlen, ndim, true);
	i = 0;
	if ( i < this->length() ) this->p(i) = v0;
	i++;
	for ( ; i < this->length() ; i++ ) {
	    this->p(i) = va_arg(ap,uintptr_t);
	}
    }
    return *this;
}
#endif

/**
 * @brief  オブジェクトのコピー
 *
 *  obj の配列の内容や属性等すべてを自身にコピーします．自身の配列の型と obj
 *  の配列の型は一致している必要があります(型が異なる場合は「=」演算子を使用
 *  します)．<br>
 *  obj が関数によって返されたテンポラリオブジェクトの場合，許可されていれば
 *  shallow copy が行なわれます．
 *
 * @param      obj コピー元となるオブジェクト (型は自身と同一)
 * @return     自身の参照
 * @throw      型が異なる配列をコピーしようとした時
 * @throw      内部バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 *
 */
mdarray_uintptr &mdarray_uintptr::init( const mdarray &obj )
{
    debug_report("( const mdarray &obj )");
    if ( obj.size_type() == this->size_type() ) {
	this->mdarray::init(obj);
    }
    else {
 	err_throw(__FUNCTION__,"FATAL",
		  "cannot use init() between different size_type");
    }
    return *this;
}

/* copy properties (auto_resize, auto_init, and rounding) */
/**
 * @brief  自動リサイズ，自動初期化，四捨五入，メモリ確保方法の設定をコピー
 *
 *  src_obj の auto_resize, auto_init, rounding, alloc_strategy の設定を
 *  コピーします．
 *
 * @param   src_obj コピーされるオブジェクト
 * @return  自身の参照
 *            
 */
mdarray_uintptr &mdarray_uintptr::init_properties( const mdarray &src_obj )
{
    this->mdarray::init_properties(src_obj);
    return *this;
}

/**
 * @brief  現在の自動リサイズの可否の設定を変更
 *
 *  リサイズモードを真(true)，または偽(false) で設定します．<br>
 *  動作モードが「自動リサイズモード」の場合は true，
 *  動作モードが「手動リサイズモード」の場合は false です．
 *
 * @param     tf リサイズモード
 * @return    自身の参照
 *
 */
mdarray_uintptr &mdarray_uintptr::set_auto_resize( bool tf )
{
    this->mdarray::set_auto_resize(tf);
    return *this;
}

/**
 * @brief  現在の自動初期化の可否の設定を変更
 *
 *  自動初期化モードを真(true)，または偽(false) で設定します．
 *
 * @param     tf 自動初期化モード
 * @return    自身の参照
 *
 */
mdarray_uintptr &mdarray_uintptr::set_auto_init( bool tf )
{
    this->mdarray::set_auto_init(tf);
    return *this;
}

/**
 * @brief  現在の四捨五入の可否の設定を変更
 *
 *  いくつかの高レベルメンバ関数において，浮動小数点数を整数に変換する時に，
 *  四捨五入を行うか否を設定します．<br>
 *  四捨五入するように設定されている場合は真(true)，
 *  四捨五入しないように設定されている場合は偽(false) を指定します．
 *
 * @param   tf 四捨五入の設定
 * @return  自身の参照
 * @note    四捨五入の属性が機能するメンバ関数は，
 *          lvalue()，llvalue() メンバ関数，assign default() メンバ関数，
 *          assign() メンバ関数，convert() メンバ関数，画像向きメンバ関数全般．
 *
 */
mdarray_uintptr &mdarray_uintptr::set_rounding( bool tf )
{
    this->mdarray::set_rounding(tf);
    return *this;
}

/**
 * @brief  メモリ確保方法の設定を変更
 *
 *  配列用メモリを確保する時に，どのような方法で行なうかを決定します．
 *  次の 3 つから選択します．<br>
 *    "min"  ... 最小限を確保．リサイズ時に必ず realloc() が呼ばれます．<br>
 *    "pow"  ... 2のn乗で確保．<br>
 *    "auto" ... 自動リサイズモードの時には "pow"，そうでない場合は "min" で
 *               リサイズ処理を実行します．<br>
 *    NULL，他 ... 現在の方法を維持します．
 *
 * @param   strategy メモリ確保方法の設定
 * @return  自身の参照
 *
 */
mdarray_uintptr &mdarray_uintptr::set_alloc_strategy( const char *strategy )
{
    this->mdarray::set_alloc_strategy(strategy);
    return *this;
}

/*
static void fnc_for_convert( const void *v0p, void *v1p, void *p )
{
    mdarray_uintptr *this_p = (mdarray_uintptr *)p;
    uintptr_t v0 = *((const uintptr_t *)v0p);
    *((uintptr_t *)v1p) = (*(this_p->convert_func_rec))(v0, this_p->convert_user_ptr_rec);
}
*/

/*
mdarray_uintptr &mdarray_uintptr::convert( uintptr_t (*func)(uintptr_t,void *),
				       void *user_ptr )
{
    this->convert_func_rec = func;
    this->convert_user_ptr_rec = user_ptr;
    this->mdarray::convert((ssize_t)UINTPTR_ZT, &fnc_for_convert, (void *)this);
    return *this;
}
*/

/**
 * @brief  配列の全要素の値の書き換え (ユーザ関数経由)
 *
 *  自身の全要素の値を ユーザ定義関数 func 経由で書き換えます．<br>
 *  ユーザ定義関数の第1引数には配列の元の各要素アドレスが，第2引数には変換後の
 *  各要素のアドレスが，第3引数には第1・第2引数に与えられた要素の個数が，
 *  第4引数には変換の方向(前からなら正，後ろからなら負)，第5引数には user_ptr 
 *  が与えられます．
 *
 * @param      func     ユーザ関数のアドレス
 * @param      user_ptr ユーザ関数の最後に与えられるユーザのポインタ
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 */
mdarray_uintptr &mdarray_uintptr::convert_via_udf( void (*func)(const uintptr_t *,uintptr_t *,size_t,int,void *),
				       void *user_ptr )
{
    this->mdarray::convert_via_udf((ssize_t)UINTPTR_ZT,
		   (void (*)(const void *,void *,size_t,int,void *))func,
		   user_ptr);
    return *this;
}

/**
 * @brief  2つのオブジェクト間での内容のスワップ
 *
 *  指定されたオブジェクトsobj の内容と自身の内容を入れ替えます．
 *  配列の内容，配列のサイズ，属性等すべての状態が入れ替わります．<br>
 *  sobj の型は自身の型と同一である必要があります．
 *
 * @param     sobj 自身と同じ型を持つ mdarray(継承) クラスのオブジェクト
 * @return    自身の参照    
 * @throw     sobj の型が自身の型と異なる場合
 */
mdarray_uintptr &mdarray_uintptr::swap( mdarray &sobj )
{
    if ( sobj.size_type() == this->size_type() ) {
	this->mdarray::swap(sobj);
    }
    else {
 	err_throw(__FUNCTION__,"FATAL",
		  "cannot swap objects between different size_type");
    }
    return *this;
}

/**
 * @brief  自身の内容を指定オブジェクトへコピー
 *
 *  自身の全ての内容を指定されたオブジェクト dest へコピーします．
 *  型，配列長，値等全ての属性がコピーされます．自身(コピー元) の配列は改変され
 *  ません．<br>
 *  型と全配列長が dest と自身とで等しい場合は，配列用バッファの再確保は
 *  行なわれず，既存のバッファに配列の内容がコピーされます．<br>
 *  「=」演算子や .init(obj) とは異なり，常に deep copy が実行されます．<br>
 *  dest には，mdarray_uintptr クラスのオブジェクトあるいは mdarray クラスの
 *  オブジェクトが指定されなければなりません．
 *
 * @param     dest コピー先のオブジェクトのアドレス
 * @return    コピーした要素数(列数×行数×レイヤ数)．<br>
 *            引数が不正な場合は負の値
 * @throw     dest で指定されたオブジェクトが不正な場合
 * @throw     バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 *
 */
ssize_t mdarray_uintptr::copy( mdarray *dest ) const
{
    debug_report("( mdarray *dest )");
    return this->mdarray::copy(dest);
}

/**
 * @brief  自身の内容を指定オブジェクトへコピー (非推奨)
 * @deprecated 非推奨．<br>
 * ssize_t mdarray_uintptr::copy( mdarray *dest ) const <br>
 * をお使いください．
 */
ssize_t mdarray_uintptr::copy( mdarray &dest ) const
{
    debug_report("( mdarray &dest )");
    return this->mdarray::copy(dest);
}

/**
 * @brief  自身の内容を指定オブジェクトへ移管
 *
 *  自身の配列の内容を，dest により指定されたオブジェクトへ「移管」します
 *  (型，配列長，各種属性が指定されたオブジェクトにコピーされます)．移管の
 *  結果，自身の配列長はゼロになります．<br>
 *  dest についての配列用バッファの再確保は行なわれず，自身の配列用バッファ
 *  についての管理権限を dest に譲渡する実装になっており，高速に動作します．
 *  dest には，mdarray_uintptr クラスのオブジェクトあるいは mdarray クラスの
 *  オブジェクトが指定されなければなりません．
 *
 * @param     dest 移管・コピー先のオブジェクトのアドレス
 * @return    自身の参照
 * @throw     dest で指定されたオブジェクトが不正な場合
 * @throw     バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 */
mdarray_uintptr &mdarray_uintptr::cut( mdarray *dest )
{
    debug_report("( mdarray *dest )");
    this->mdarray::cut(dest);
    return *this;
}

/* interchange rows and columns */
/**
 * @brief  自身の配列の (x,y) でのトランスポーズ
 *
 *  自身の配列のカラムとロウとを入れ替えます．
 * 
 * @note  高速なアルゴリズムを使用しています．
 */
mdarray_uintptr &mdarray_uintptr::transpose_xy()
{
    this->mdarray::transpose_xy();
    return *this;
}

/* interchange xyz to zxy */
/**
 * @brief  自身の配列の (x,y,z)→(z,x,y) のトランスポーズ
 *
 *  自身の配列の軸 (x,y,z) を (z,x,y) へ変換します．
 * 
 * @note  高速なアルゴリズムを使用しています．
 */
mdarray_uintptr &mdarray_uintptr::transpose_xyz2zxy()
{
    this->mdarray::transpose_xyz2zxy();
    return *this;
}

/* rotate image                                              */
/*   angle: 90,-90, or 180                                   */
/*          (anticlockwise when image is bottom-left origin) */
/**
 * @brief  自身の配列の (x,y) での回転 (90度単位)
 *
 *  自身の配列の (x,y) 面についての回転(90度単位)を行ないます．<br>
 *  左下を原点とした場合，反時計まわりで指定します．
 *
 * @param  angle 90,-90, 180 のいずれかを指定
 * 
 */
mdarray_uintptr &mdarray_uintptr::rotate_xy( int angle )
{
    this->mdarray::rotate_xy( angle );
    return *this;
}


/**
 * @brief  自身の配列の内容をユーザ・バッファへコピー (要素数での指定)
 *
 *  自身の配列の内容を dest_bufで指定されたユーザ・バッファへコピーします．<br>
 *  バッファの大きさ elem_size は要素の個数で与えます．idx0, idx1, idx2 で
 *  自身の配列での開始点を指定します．<br>
 *  引数に，MDARRAY_INDEF を明示的に与えないでください．
 *
 * @param      dest_buf ユーザ・バッファのアドレス (コピー先)
 * @param      elem_size コピーする要素の個数
 * @param      idx0 次元番号0 の次元(1 次元目) の要素番号(コピー元，省略可)
 * @param      idx1 次元番号1 の次元(2 次元目) の要素番号(コピー元，省略可)
 * @param      idx2 次元番号2 の次元(3 次元目) の要素番号(コピー元，省略可)
 * @return     ユーザのバッファ長が十分な場合にコピーされる要素数
 * @throw      メモリ破壊を起こした場合
 *
 */
ssize_t mdarray_uintptr::get_elements( uintptr_t *dest_buf, size_t elem_size, 
				     ssize_t idx0, ssize_t idx1, ssize_t idx2 ) const
{
    return this->mdarray::get_elements((void *)dest_buf,
				       elem_size, idx0,idx1,idx2);
}

/**
 * @brief  ユーザ・バッファの内容を自身の配列へコピー (要素数での指定)
 *
 *  src_buf で指定されたユーザ・バッファの内容を自身の配列へコピーします．<br>
 *  バッファの大きさ elem_size は，要素の個数で与えます．idx0, idx1, idx2 で
 *  自身の配列での開始点を指定します．<br>
 *  引数に，MDARRAY_INDEF を明示的に与えないでください．
 *
 * @param      src_buf ユーザ・バッファのアドレス (コピー元)
 * @param      elem_size コピーする要素の個数
 * @param      idx0 次元番号0 の次元(1 次元目) の要素番号(コピー先，省略可)
 * @param      idx1 次元番号1 の次元(2 次元目) の要素番号(コピー先，省略可)
 * @param      idx2 次元番号2 の次元(3 次元目) の要素番号(コピー先，省略可)
 * @return     ユーザのバッファ長が十分な場合にコピーされる要素数
 * @throw      メモリ破壊を起こした場合
 *
 */
ssize_t mdarray_uintptr::put_elements( const uintptr_t *src_buf, size_t elem_size,
				     ssize_t idx0, ssize_t idx1, ssize_t idx2 )
{
    return this->mdarray::put_elements((const void *)src_buf, 
				       elem_size, idx0,idx1,idx2);
}

/**
 * @brief  次元数を1つ拡張
 *
 *  自身が持つ配列の次元数を1 つ拡張します．
 *
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 *
 */
mdarray_uintptr &mdarray_uintptr::increase_dim()
{
    this->mdarray::increase_dim();
    return *this;
}

/**
 * @brief  次元数を1つ縮小
 *
 *  自身が持つ配列の次元を1 つ縮小します．
 *
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 *
 */
mdarray_uintptr &mdarray_uintptr::decrease_dim()
{
    this->mdarray::decrease_dim();
    return *this;
}

/**
 * @brief  最初の次元についての配列長の変更
 *
 *  自身が持つ配列の長さを変更します．<br>
 *  配列長を拡張する場合，要素の値はデフォルト値で埋められます．<br>
 *  配列長を収縮する場合，len以降の要素は削除されます．<br>
 *  常に次元番号 0 の次元(1次元目)を処理対象とします．
 *
 * @param      len 要素の個数
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 *
 */
mdarray_uintptr &mdarray_uintptr::resize( size_t len )
{
    this->mdarray::resize(len);
    return *this;
}

/**
 * @brief  任意の1つの次元についての配列長の変更
 *
 *  自身が持つ配列の長さを変更します．<br>
 *  配列長を拡張する場合，要素の値はデフォルト値で埋められます．<br>
 *  配列長を収縮する場合，len以降の要素は削除されます．<br>
 *  次元番号 dim_index で処理対象とする次元を指定できます．
 *
 * @param      dim_index 次元番号
 * @param      len 要素の個数
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 *
 */
mdarray_uintptr &mdarray_uintptr::resize( size_t dim_index, size_t len )
{
    this->mdarray::resize(dim_index,len);
    return *this;
}

/**
 * @brief  配列長の変更 (他オブジェクトからコピー)
 *
 *  自身の次元数と配列長を，オブジェクトsrc が持つものと同じ大きさにします．
 *  配列長を拡張する場合，要素の値はデフォルト値で埋められます．<br>
 *  配列長を収縮する場合，配列長に満たない部分の要素は削除されます．<br>
 *
 * @param     src 配列長のコピー元
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合
 *
 */
mdarray_uintptr &mdarray_uintptr::resize( const mdarray &src )
{
    debug_report("(const mdarray &src)");
    this->mdarray::resize(src);
    return *this;
}

/* change the length of the 1-d array */
/**
 * @brief  配列長の変更 (1次元)
 *
 *  自身が持つ配列の長さを変更し，配列長 x_len の1次元配列とします．
 */
mdarray_uintptr &mdarray_uintptr::resize_1d( size_t x_len )
{
    this->mdarray::resize_1d(x_len);
    return *this;
}

/* change the length of the 2-d array */
/**
 * @brief  配列長の変更 (2次元)
 *
 *  自身が持つ配列の長さを変更し，配列長 (x_len, y_len) の2次元配列とします．
 */
mdarray_uintptr &mdarray_uintptr::resize_2d( size_t x_len, size_t y_len )
{
    this->mdarray::resize_2d(x_len, y_len);
    return *this;
}

/* change the length of the 3-d array */
/**
 * @brief  配列長の変更 (3次元)
 *
 *  自身が持つ配列の長さを変更し，配列長 (x_len, y_len, z_len) の3次元配列と
 *  します．
 */
mdarray_uintptr &mdarray_uintptr::resize_3d( size_t x_len, size_t y_len, size_t z_len )
{
    this->mdarray::resize_3d(x_len, y_len, z_len);
    return *this;
}

/**
 * @brief  配列長の変更 (複数の次元を指定可)
 *
 *  自身が持つ配列の長さを変更します．<br>
 *  配列長を拡張する場合，要素の値をデフォルト値で埋るかどうかは init_buf で
 *  設定できます．<br>
 *  配列長を収縮する場合，配列長に満たない部分の要素は削除されます．<br>
 *
 * @param     naxisx[] 各次元の要素数
 * @param     ndim 配列次元数
 * @param     init_buf 配列長の拡張時，要素値をデフォルト値で埋めるなら true
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合
 * @note      2次元以上のリサイズの場合，低コストで行なえます．1次元だけリサイ
 *            ズするなら，resize(dim_index, ...) の方が低コストです．
 */
mdarray_uintptr &mdarray_uintptr::resize( const size_t naxisx[], size_t ndim, bool init_buf )
{
    this->mdarray::resize(naxisx,ndim,init_buf);
    return *this;
}

/**
 * @brief  配列長の変更 (IDLの記法)
 *
 *  自身が持つ配列の長さを変更します．長さの指定は，printf() 形式の可変長引数で
 *  指定可能なカンマ区切りの文字列(例えば"10,5")によって行ないます．<br>
 *  指定された文字列中の要素が数字ではない場合，その次元は長さを変更しません．
 *  例えば， ",2" が指定された場合，最初の次元の長さは変更されません．
 *
 * @param  exp_fmt csv形式の長さ指定文字列のためのフォーマット指定
 * @param  ...     formatに対応した可変長引数の各要素データ
 * @throw  内部バッファの確保に失敗した場合
 */
mdarray_uintptr &mdarray_uintptr::resizef( const char *exp_fmt, ... )
{
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	this->mdarray::vresizef(exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vresizef() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  配列長の変更 (IDLの記法)
 *
 *  自身が持つ配列の長さを変更します．長さの指定は，printf() 形式の可変長引数で
 *  指定可能なカンマ区切りの文字列(例えば"10,5")によって行ないます．<br>
 *  指定された文字列中の要素が数字ではない場合，その次元は長さを変更しません．
 *  例えば， ",2" が指定された場合，最初の次元の長さは変更されません．
 *
 * @param  exp_fmt csv形式の長さ指定文字列のためのフォーマット指定
 * @param  ap      formatに対応した可変長引数のリスト
 * @throw  内部バッファの確保に失敗した場合
 */
mdarray_uintptr &mdarray_uintptr::vresizef( const char *exp_fmt, va_list ap )
{
    this->mdarray::vresizef(exp_fmt,ap);
    return *this;
}

/**
 * @brief  最初の次元について配列長の相対的な変更
 *
 *  自身が持つ配列の長さを len の指定分，拡張・縮小します．<br>
 *  resizeby() 後の配列サイズは，元の配列の長さに len を加えたものとなります．
 *  サイズの縮小は，len にマイナス値を指定することによって行います．<br>
 *  常に次元番号 0 の次元(1次元目)を処理対象とします．
 *
 * @param      len 要素個数の増分・減分
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 *
 */
mdarray_uintptr &mdarray_uintptr::resizeby( ssize_t len )
{
    this->mdarray::resizeby(len);
    return *this;
}

/**
 * @brief  任意の1つの次元について配列長の相対的な変更
 *
 *  自身が持つ配列の長さを len の指定分，拡張・縮小します．<br>
 *  resizeby() 後の配列サイズは，元の配列の長さに len を加えたものとなります．
 *  サイズの縮小は，len にマイナス値を指定することによって行います．<br>
 *  次元番号 dim_index で処理対象とする次元を指定できます．
 *
 * @param      dim_index 次元番号
 * @param      len 要素個数の増分・減分
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 *
 */
mdarray_uintptr &mdarray_uintptr::resizeby( size_t dim_index, ssize_t len )
{
    this->mdarray::resizeby(dim_index,len);
    return *this;
}

/* change the length of the 1-d array relatively */
/**
 * @brief  配列長の相対的な変更 (1次元用)
 */
mdarray_uintptr &mdarray_uintptr::resizeby_1d( ssize_t x_len )
{
    this->mdarray::resizeby_1d(x_len);
    return *this;
}

/* change the length of the 2-d array relatively */
/**
 * @brief  配列長の相対的な変更 (2次元用)
 */
mdarray_uintptr &mdarray_uintptr::resizeby_2d( ssize_t x_len, ssize_t y_len )
{
    this->mdarray::resizeby_2d(x_len, y_len);
    return *this;
}

/* change the length of the 3-d array relatively */
/**
 * @brief  配列長の相対的な変更 (3次元用)
 */
mdarray_uintptr &mdarray_uintptr::resizeby_3d(ssize_t x_len, ssize_t y_len, ssize_t z_len)
{
    this->mdarray::resizeby_3d(x_len, y_len, z_len);
    return *this;
}

/**
 * @brief  配列長の相対的な変更 (複数の次元を指定可)
 *
 *  自身が持つ配列の長さを naxisx_rel[] の指定分，拡張・縮小します．<br>
 *  resizeby() 後の配列サイズは，元の配列の長さに naxisx_rel[] を加えたもの
 *  となります．サイズの縮小は、naxisx_rel[] にマイナス値を指定することによって
 *  行います．<br>
 *  init_buf でサイズ拡張時に新規に作られる配列要素の初期化をするかどうかを
 *  指定できます．
 *
 * @param     naxisx_rel 要素個数の増分・減分
 * @param     ndim naxisx_rel[] の個数
 * @param     init_buf 新規に作られる配列要素の初期化を行なう場合は true
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合
 */
mdarray_uintptr &mdarray_uintptr::resizeby( const ssize_t naxisx_rel[], size_t ndim, 
			    bool init_buf )
{
    this->mdarray::resizeby(naxisx_rel,ndim,init_buf);
    return *this;
}

/**
 * @brief  配列長の相対的な変更 (IDLの記法)
 *
 *  自身が持つ配列の長さを指定分，拡張・縮小します．長さの指定は，printf() 形式
 *  の可変長引数で指定可能なカンマ区切りの文字列(例えば"10,-5")によって行ない
 *  ます．<br>
 *  resizebyf()後の配列サイズは，元の配列の長さに指定分を加えたものとなります．
 *  サイズの縮小は、マイナス値を指定することによって行います．<br>
 *  指定された文字列中の要素が数字ではない場合，その次元は長さを変更しません．
 *  例えば， ",2" が指定された場合，最初の次元の長さは変更されません．
 *
 * @param  exp_fmt csv形式の長さ指定文字列のためのフォーマット指定
 * @param  ...     formatに対応した可変長引数の各要素データ
 * @throw  内部バッファの確保に失敗した場合
 */
mdarray_uintptr &mdarray_uintptr::resizebyf( const char *exp_fmt, ... )
{
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	this->mdarray::vresizebyf(exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vresizebyf() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  配列長の相対的な変更 (IDLの記法)
 *
 *  自身が持つ配列の長さを指定分，拡張・縮小します．長さの指定は，printf() 形式
 *  の可変長引数で指定可能なカンマ区切りの文字列(例えば"10,-5")によって行ない
 *  ます．<br>
 *  resizebyf()後の配列サイズは，元の配列の長さに指定分を加えたものとなります．
 *  サイズの縮小は、マイナス値を指定することによって行います．<br>
 *  指定された文字列中の要素が数字ではない場合，その次元は長さを変更しません．
 *  例えば， ",2" が指定された場合，最初の次元の長さは変更されません．
 *
 * @param  exp_fmt csv形式の長さ指定文字列のためのフォーマット指定
 * @param  ap      formatに対応した可変長引数のリスト
 * @throw  内部バッファの確保に失敗した場合
 */
mdarray_uintptr &mdarray_uintptr::vresizebyf( const char *exp_fmt, va_list ap )
{
    this->mdarray::vresizebyf(exp_fmt,ap);
    return *this;
}

/* change length of array without adjusting buffer contents */
/**
 * @brief  配列のバッファ内位置の調整をせずに，配列用バッファを再確保
 *
 *  配列のバッファ内位置に調整をせずに，自身が持つ次元の大きさと配列の長さを
 *  変更します．つまり，配列用バッファに対しては単純にrealloc()を呼ぶだけの処理
 *  を行ないます．
 *
 * @param     naxisx[] 各次元の要素数
 * @param     ndim 配列次元数
 * @param    init_buf 新たに確保した部分の要素値をデフォルト値で埋めるなら true
 * @throw     内部バッファの確保に失敗した場合
 */
mdarray_uintptr &mdarray_uintptr::reallocate( const size_t naxisx[], size_t ndim, bool init_buf )
{
    this->mdarray::reallocate(naxisx,ndim,init_buf);
    return *this;
}

/* free current buffer and alloc new memory */
/**
 * @brief  配列用バッファを一旦開放し，新規に確保
 *
 *  配列の内容を一旦破棄し，配列の長さを変更します．つまり，配列用バッファに
 *  対して free()，malloc() を呼ぶだけの処理を行ないます．
 *
 * @param    naxisx[] 各次元の要素数
 * @param    ndim 配列次元数
 * @param    init_buf 新たに確保した部分の要素値をデフォルト値で埋めるなら true
 * @throw    内部バッファの確保に失敗した場合
 */
mdarray_uintptr &mdarray_uintptr::allocate( const size_t naxisx[], size_t ndim, bool init_buf )
{
    this->mdarray::allocate(naxisx,ndim,init_buf);
    return *this;
}

/**
 * @brief  サイズ拡張時の初期値の設定
 *
 *  配列サイズ拡張時の初期値を設定します．設定された値は既存の要素には
 *  作用せず，サイズ拡張時に有効となります．
 *
 * @param     value 配列サイズ拡張時の初期値
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合
 *
 */
mdarray_uintptr &mdarray_uintptr::assign_default( uintptr_t value )
{
    this->mdarray::assign_default((const void *)&value);
    return *this;
}

/**
 * @brief  最初の次元について新規配列要素の挿入
 *
 *  自身の配列の要素位置 idx に，len 個分の要素を挿入します．なお，挿入される
 *  要素の値はデフォルト値です．<br>
 *  常に次元番号 0 の次元(1次元目)を処理対象とします．
 *
 * @param      idx 挿入位置の要素番号
 * @param      len 要素の個数
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 *
 */
mdarray_uintptr &mdarray_uintptr::insert( ssize_t idx, size_t len )
{
    this->mdarray::insert(idx,len);
    return *this;
}

/**
 * @brief  任意の1つの次元について新規配列要素の挿入
 *
 *  自身の配列の要素位置 idx に，len 個分の要素を挿入します．なお，挿入される
 *  要素の値はデフォルト値です．<br>
 *  次元番号 dim_index で処理対象とする次元を指定できます．
 *
 * @param      dim_index 次元番号
 * @param      idx 挿入位置の要素番号
 * @param      len 要素の個数
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 *
 */
mdarray_uintptr &mdarray_uintptr::insert( size_t dim_index, ssize_t idx, size_t len )
{
    this->mdarray::insert(dim_index,idx,len);
    return *this;
}

/**
 * @brief  最初の次元について配列要素の削除
 *
 *  自身の配列から指定された部分の要素を削除します．削除した分，長さは短くなり
 *  ます．<br>
 *  常に次元番号 0 の次元(1次元目)を処理対象とします.
 *
 * @param      idx 開始位置の要素番号
 * @param      len 要素の個数
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 *
 */
mdarray_uintptr &mdarray_uintptr::erase( ssize_t idx, size_t len )
{
    this->mdarray::erase(idx,len);
    return *this;
}

/**
 * @brief  任意の1つの次元について配列要素の削除
 *
 *  自身の配列から指定された部分の要素を削除します．削除した分，長さは短くなり
 *  ます．<br>
 *  次元番号 dim_index で処理対象とする次元を指定できます．
 *
 * @param      dim_index 次元番号
 * @param      idx 開始位置の要素番号
 * @param      len 要素の個数
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 *
 */
mdarray_uintptr &mdarray_uintptr::erase( size_t dim_index, ssize_t idx, size_t len )
{
    this->mdarray::erase(dim_index,idx,len);
    return *this;
}

/**
 * @brief  最初の次元について配列要素間での値のコピー
 *
 * 自身の配列要素間で値をコピーします．<br>
 * 引数 clr に false が指定された場合はコピー元の値は残りますが，true が指定
 * された場合はコピー元の値は残らずデフォルト値で埋められます．<br>
 * idx_dst に既存の配列長より大きな値を設定しても，配列サイズは変わりません．
 * この点が次の cpy() メンバ関数とは異なります．<br>
 * 常に次元番号 0 の次元(1次元目)を処理対象とします．
 *
 * @param      idx_src コピー元の要素番号
 * @param      len コピー元の要素の長さ
 * @param      idx_dst コピー先の要素番号
 * @param      clr コピー元の値のクリア可否
 * @return     自身の参照
 *
 */
mdarray_uintptr &mdarray_uintptr::move( ssize_t idx_src, size_t len, ssize_t idx_dst,
			   bool clr )
{
    this->mdarray::move(idx_src,len,idx_dst,clr);
    return *this;
}

/**
 * @brief  任意の1つの次元について配列要素間での値のコピー
 *
 *  自身の配列要素間で値をコピーします．<br>
 *  引数 clr に false が指定された場合はコピー元の値は残りますが，true が指定
 *  された場合はコピー元の値は残らずデフォルト値で埋められます．<br>
 *  idx_dst に既存の配列長より大きな値を設定しても，配列サイズは変わりません．
 *  この点が次の cpy() メンバ関数とは異なります．<br>
 *  次元番号 dim_index で処理対象とする次元を指定できます．
 *
 * @param      dim_index 次元番号
 * @param      idx_src コピー元の要素番号
 * @param      len コピー元の要素の長さ
 * @param      idx_dst コピー先の要素番号
 * @param      clr コピー元の値のクリア可否
 * @return     自身の参照
 *
 */
mdarray_uintptr &mdarray_uintptr::move( size_t dim_index, 
			   ssize_t idx_src, size_t len, ssize_t idx_dst,
			   bool clr )
{
    this->mdarray::move(dim_index,idx_src,len,idx_dst,clr);
    return *this;
}

/**
 * @brief  最初の次元について配列要素間での値のコピー (配列数は自動拡張)
 *
 *  自身の配列要素間で値をコピーします．<br>
 *  引数 clr に false が指定された場合はコピー元の値は残りますが，true が指定
 *  された場合はコピー元の値は残らずデフォルト値で埋められます．<br>
 *  idx_dst + len が既存の配列長より大きい場合，配列サイズは自動拡張されま
 *  す．<br>
 *  常に次元番号 0 の次元(1次元目)を処理対象とします．
 *
 * @param      idx_src コピー元の要素番号
 * @param      len コピー元の要素の長さ
 * @param      idx_dst コピー先の要素番号
 * @param      clr コピー元の値のクリア可否
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 *
 */
mdarray_uintptr &mdarray_uintptr::cpy( ssize_t idx_src, size_t len, ssize_t idx_dst,
			  bool clr )
{
    this->mdarray::cpy(idx_src,len,idx_dst,clr);
    return *this;
}

/**
 * @brief  任意の1つの次元について配列要素間での値のコピー (配列数は自動拡張)
 *
 *  自身の配列要素間で値をコピーします．<br>
 *  引数 clr に false が指定された場合はコピー元の値は残りますが，true が指定
 *  された場合はコピー元の値は残らずデフォルト値で埋められます．<br>
 *  idx_dst + len が既存の配列長より大きい場合，配列サイズは自動拡張されま
 *  す．<br>
 *  次元番号 dim_index で処理対象とする次元を指定できます．
 *
 * @param      dim_index 次元番号
 * @param      idx_src コピー元の要素番号
 * @param      len コピー元の要素の長さ
 * @param      idx_dst コピー先の要素番号
 * @param      clr コピー元の値のクリア可否
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 *
 */
mdarray_uintptr &mdarray_uintptr::cpy( size_t dim_index, 
			  ssize_t idx_src, size_t len, ssize_t idx_dst,
			  bool clr )
{
    this->mdarray::cpy(dim_index,idx_src,len,idx_dst,clr);
    return *this;
}

/**
 * @brief  最初の次元について配列要素間での値の入れ換え
 *
 *  自身の配列要素間で値を入れ替えます．<br>
 *  要素番号 idx_src から len 個分の要素を，要素番号 idx_dst から len 個分の要
 *  素と入れ替えます．<br>
 *  idx_dst + len が配列サイズを超える場合は，配列サイズまでの処理が行われま
 *  す．入れ替える領域が重なった場合，重なっていない src の領域に対してのみ入
 *  れ替え処理が行われます．
 *
 * @param      idx_src 入れ替え元の要素番号
 * @param      len 入れ替え元の要素の長さ
 * @param      idx_dst 入れ替え先の要素番号
 * @return     自身の参照
 *
 */
mdarray_uintptr &mdarray_uintptr::swap( ssize_t idx_src, size_t len, ssize_t idx_dst )
{

    this->mdarray::swap(idx_src,len,idx_dst);
    return *this;
}

/**
 * @brief  任意の1つの次元について配列要素間での値の入れ換え
 *
 *  自身の配列要素間で値を入れ替えます．<br>
 *  次元番号 dim_index の要素番号 idx_src から len 個分の要素を，要素番号 
 *  idx_dst から len 個分の要素と入れ替えます．<br>
 *  idx_dst + len が配列サイズを超える場合は，配列サイズまでの処理が行われま
 *  す．入れ替える領域が重なった場合，重なっていない src の領域に対してのみ入
 *  れ替え処理が行われます．
 *
 * @param      dim_index 次元番号
 * @param      idx_src 入れ替え元の要素番号
 * @param      len 入れ替え元の要素の長さ
 * @param      idx_dst 入れ替え先の要素番号
 * @return     自身の参照
 *
 */
mdarray_uintptr &mdarray_uintptr::swap( size_t dim_index, 
		      ssize_t idx_src, size_t len, ssize_t idx_dst )
{
    this->mdarray::swap(dim_index,idx_src,len,idx_dst);
    return *this;
}

/**
 * @brief  最初の次元について配列の不要部分の消去
 *
 *  自身の配列を，要素位置 idx から len 個の要素だけにします．<br>
 *  常に次元番号 0 の次元(1次元目)を処理対象とします．
 *
 * @param      idx 切り出し開始位置の要素番号
 * @param      len 要素の個数
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 *
 */
mdarray_uintptr &mdarray_uintptr::crop( ssize_t idx, size_t len )
{
    this->mdarray::crop(idx,len);
    return *this;
}

/**
 * @brief  任意の1つの次元について配列の不要部分の消去
 *
 *  自身の配列を，要素位置 idx から len 個の要素だけにします．<br>
 *  次元番号 dim_index で処理対象とする次元を指定できます．
 *
 * @param      dim_index 次元番号
 * @param      idx 切り出し開始位置の要素番号
 * @param      len 要素の個数
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 *
 */
mdarray_uintptr &mdarray_uintptr::crop( size_t dim_index, ssize_t idx, size_t len )
{
    this->mdarray::crop(dim_index,idx,len);
    return *this;
}

#if 0
/* crop(), vcrop() : 未実装 */
/**
 *
 * @deprecated 未実装です.
 */
mdarray_uintptr &mdarray_uintptr::cropf( const char *exp_fmt, ... )
{
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	this->mdarray::vcropf(exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vcropf() failed");
    }
    va_end(ap);
    return *this;
}

/**
 *
 * @deprecated 未実装です.
 */
mdarray_uintptr &mdarray_uintptr::vcropf( const char *exp_fmt, va_list ap )
{
    this->mdarray::vcropf(exp_fmt,ap);
    return *this;
}
#endif	/* 0 */

/**
 * @brief  最初の次元について配列の並びを反転
 *
 *  自身の配列の要素位置 idx から len 個の要素を反転させます．<br>
 *  常に次元番号 0 の次元(1次元目)を処理対象とします．
 *
 * @param     idx 開始位置の要素番号
 * @param     len 要素の個数
 * @return    自身の参照
 */
mdarray_uintptr &mdarray_uintptr::flip( ssize_t idx, size_t len )
{
    this->mdarray::flip(idx,len);
    return *this;
}

/**
 * @brief  任意の1つの次元について配列の並びを反転
 *
 *  自身の配列の要素位置 idx から len 個の要素を反転させます．<br>
 *  次元番号 dim_index で処理対象とする次元を指定できます．
 *
 * @param     dim_index 次元番号
 * @param     idx 開始位置の要素番号
 * @param     len 要素の個数
 * @return    自身の参照
 */
mdarray_uintptr &mdarray_uintptr::flip( size_t dim_index, ssize_t idx, size_t len )
{
    this->mdarray::flip(dim_index,idx,len);
    return *this;
}


/* 
 * member functions for image processing using IDL style argument 
 * such as "0:99,*".  The expression is set to exp_fmt argument in 
 * member functions.
 *
 * Number of dimension in the expression is unlimited.
 * Note that integer numbers in the string is 0-indexed. 
 *
 * [example]
 *   array1 = array0.sectionf("0:99,*");
 *   array1 = array0.sectionf("%ld:%ld,*", begin, end);
 *
 * Flipping elements in dimensions is supported in sectionf(), copyf(),
 * trimf(), and flipf().  Here is an example to perform flipping elements
 * in first 2 dimensions:
 *   array1 = array0.sectionf("99:0,-*");
 * 
 */

/**
 * @brief  一部要素を別オブジェクトへコピー (IDLの記法)
 *
 *  画像データ向きのメンバ関数で，自身の配列の内容の一部分を指定されたオブジェ
 *  クト dest_obj へコピーします．<br>
 *  コピーされる範囲は任意の次元まで指定可能で "0:99,*" のように IDL や IRAF と
 *  同様の文字列で指定します．この文字列中の要素番号は 0-indexed です．<br>
 *  コピー先へは，配列の要素値とコピー元の型，デフォルト値，丸めフラグの属性な
 *  どすべてをコピーします．自身(コピー元) の配列は改変されません．<br>
 *  dest_obj に自身を与えた場合，trimf()を実行した場合と同じ結果になります．
 *
 * @param  dest_obj コピー先のオブジェクト
 * @param  exp_fmt  csv形式の範囲指定文字列のためのフォーマット指定
 * @param  ...      exp_fmtに対応した可変長引数の各要素データ
 * @return    コピーした要素数(列数 × 行数 × レイヤ数 × ...)
 * @throw     バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 */
ssize_t mdarray_uintptr::copyf( mdarray *dest_obj, const char *exp_fmt, ... ) const
{
    ssize_t ret;
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	ret = this->mdarray::vcopyf(dest_obj, exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vcopyf() failed");
    }
    va_end(ap);
    return ret;
}

/**
 * @brief  一部要素を別オブジェクトへコピー (IDLの記法)
 *
 *  画像データ向きのメンバ関数で，自身の配列の内容の一部分を指定されたオブジェ
 *  クト dest_obj へコピーします．<br>
 *  コピーされる範囲は任意の次元まで指定可能で "0:99,*" のように IDL や IRAF と
 *  同様の文字列で指定します．この文字列中の要素番号は 0-indexed です．<br>
 *  コピー先へは，配列の要素値とコピー元の型，デフォルト値，丸めフラグの属性な
 *  どすべてをコピーします．自身(コピー元) の配列は改変されません．<br>
 *  dest_obj に自身を与えた場合，trimf()を実行した場合と同じ結果になります．
 *
 * @param  dest_obj コピー先のオブジェクト
 * @param  exp_fmt  csv形式の範囲指定文字列のためのフォーマット指定
 * @param  ap       exp_fmtに対応した可変長引数のリスト
 * @return    コピーした要素数(列数 × 行数 × レイヤ数 × ...)
 * @throw     バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 */
ssize_t mdarray_uintptr::vcopyf( mdarray *dest_obj, const char *exp_fmt, va_list ap ) const
{
    return this->mdarray::vcopyf(dest_obj, exp_fmt, ap);
}

/* copy an convet all or a section to another mdarray object */
/* Flipping elements is supported                            */
/* [not implemented] */
/**
 * @brief  自身の一部要素を型変換した配列を返す (IDLの記法)
 * @deprecated  未実装
 */
ssize_t mdarray_uintptr::convertf_copy( mdarray *dest_obj, const char *exp_fmt, ... ) const
{
    ssize_t ret;
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	ret = this->mdarray::vconvertf_copy(dest_obj, exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vconvertf_copy() failed");
    }
    va_end(ap);
    return ret;
}

/**
 * @brief  自身の一部要素を型変換した配列を返す (IDLの記法)
 * @deprecated  未実装
 */
ssize_t mdarray_uintptr::vconvertf_copy( mdarray *dest_obj, const char *exp_fmt, va_list ap ) const
{
    return this->mdarray::vconvertf_copy(dest_obj, exp_fmt, ap);
}

/**
 * @brief  指定部分以外の配列要素の消去 (画像データのトリミング・IDLの記法)
 *
 *  画像データ向きのメンバ関数で，自身の配列のうち指定された部分だけを残し，
 *  指定外の部分を消去します．<br>
 *  切り出されるされる範囲は任意の次元まで指定可能で "0:99,*" のように IDL や
 *  IRAF と同様の文字列で指定します．この文字列中の要素番号は 0-indexed です．
 *
 * @param  exp_fmt  csv形式の範囲指定文字列のためのフォーマット指定
 * @param  ...      exp_fmtに対応した可変長引数の各要素データ
 * @return    自身の参照
 * @throw     バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 */
mdarray_uintptr &mdarray_uintptr::trimf( const char *exp_fmt, ... )
{
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	this->mdarray::vtrimf(exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vtrimf() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  指定部分以外の配列要素の消去 (画像データのトリミング・IDLの記法)
 *
 *  画像データ向きのメンバ関数で，自身の配列のうち指定された部分だけを残し，
 *  指定外の部分を消去します．<br>
 *  切り出されるされる範囲は任意の次元まで指定可能で "0:99,*" のように IDL や
 *  IRAF と同様の文字列で指定します．この文字列中の要素番号は 0-indexed です．
 *
 * @param  exp_fmt  csv形式の範囲指定文字列のためのフォーマット指定
 * @param  ap       exp_fmtに対応した可変長引数のリスト
 * @return    自身の参照
 * @throw     バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 */
mdarray_uintptr &mdarray_uintptr::vtrimf( const char *exp_fmt, va_list ap )
{
    this->mdarray::vtrimf(exp_fmt, ap);
    return *this;
}

/* flip elements in a section specified by IDL-style expression */
/**
 * @brief  任意の次元(複数も可)で配列の並びを反転 (IDLの記法)
 */
mdarray_uintptr &mdarray_uintptr::flipf( const char *exp_fmt, ... )
{
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	this->mdarray::vflipf(exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vflipf() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  任意の次元(複数も可)で配列の並びを反転 (IDLの記法)
 */
mdarray_uintptr &mdarray_uintptr::vflipf( const char *exp_fmt, va_list ap )
{
    this->mdarray::vflipf(exp_fmt, ap);
    return *this;
}

/**
 * @brief  (x,y)のトランスポーズを行なった配列を取得 (IDLの記法)
 */
ssize_t mdarray_uintptr::transposef_xy_copy( mdarray *dest_obj, const char *exp_fmt, ... ) const
{
    ssize_t ret;
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	ret = this->mdarray::vtransposef_xy_copy(dest_obj, exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vtransposef_xy_copy() failed");
    }
    va_end(ap);
    return ret;
}

/**
 * @brief  (x,y)のトランスポーズを行なった配列を取得 (IDLの記法)
 */
ssize_t mdarray_uintptr::vtransposef_xy_copy( mdarray *dest_obj, const char *exp_fmt, va_list ap ) const
{
    return this->mdarray::vtransposef_xy_copy(dest_obj, exp_fmt, ap);
}

/**
 * @brief  (x,y,z)→(z,x,y)のトランスポーズを行なった配列を取得 (IDLの記法)
 */
ssize_t mdarray_uintptr::transposef_xyz2zxy_copy( mdarray *dest_obj, const char *exp_fmt, ... ) const
{
    ssize_t ret;
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	ret = this->mdarray::vtransposef_xyz2zxy_copy(dest_obj, exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vtransposef_xyz2zxy_copy() failed");
    }
    va_end(ap);
    return ret;
}

/**
 * @brief  (x,y,z)→(z,x,y)のトランスポーズを行なった配列を取得 (IDLの記法)
 */
ssize_t mdarray_uintptr::vtransposef_xyz2zxy_copy( mdarray *dest_obj, const char *exp_fmt, va_list ap ) const
{
    return this->mdarray::vtransposef_xyz2zxy_copy(dest_obj, exp_fmt, ap);
}

/* padding existing values in an array */
/**
 * @brief  自身の配列をデフォルト値でパディング (IDLの記法)
 */
mdarray_uintptr &mdarray_uintptr::cleanf( const char *exp_fmt, ... )
{
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	this->mdarray::vcleanf(exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vcleanf() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  自身の配列をデフォルト値でパディング (IDLの記法)
 */
mdarray_uintptr &mdarray_uintptr::vcleanf( const char *exp_fmt, va_list ap )
{
    this->mdarray::vcleanf(exp_fmt, ap);
    return *this;
}

/* rewrite element values with a value in a section */
/**
 * @brief  自身の配列を指定されたスカラー値で書き換え (IDLの記法)
 */
mdarray_uintptr &mdarray_uintptr::fillf( double value, const char *exp_fmt, ... )
{
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	this->mdarray::vfillf(value, exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vfillf() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  自身の配列を指定されたスカラー値で書き換え (IDLの記法)
 */
mdarray_uintptr &mdarray_uintptr::vfillf( double value, const char *exp_fmt, va_list ap )
{
    this->mdarray::vfillf(value, exp_fmt, ap);
    return *this;
}

/* rewrite element values with a value in a section */
/**
 * @brief  自身の配列をスカラー値で書き換え (ユーザ関数経由・IDLの記法)
 */
mdarray_uintptr &mdarray_uintptr::fillf_via_udf( double value, 
	void (*func)(double [],double,size_t, ssize_t,ssize_t,ssize_t,mdarray_uintptr *,void *),
        void *user_ptr, const char *exp_fmt, ... )
{
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	this->mdarray::vfillf_via_udf(value, 
		(void (*)(double [],double,size_t, ssize_t,ssize_t,ssize_t,mdarray *,void *))func,
		user_ptr, exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vfillf() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  自身の配列をスカラー値で書き換え (ユーザ関数経由・IDLの記法)
 */
mdarray_uintptr &mdarray_uintptr::vfillf_via_udf( double value, 
	void (*func)(double [],double,size_t, ssize_t,ssize_t,ssize_t,mdarray_uintptr *,void *),
        void *user_ptr, const char *exp_fmt, va_list ap )
{
    this->mdarray::vfillf_via_udf(value, 
	(void (*)(double [],double,size_t, ssize_t,ssize_t,ssize_t,mdarray *,void *))func,
	user_ptr, exp_fmt, ap);
    return *this;
}

/**
 * @brief  自身の配列に，指定されたスカラー値で加算 (IDLの記法)
 */
mdarray_uintptr &mdarray_uintptr::addf( double value, const char *exp_fmt, ... )
{
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	this->mdarray::vaddf(value, exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vaddf() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  自身の配列に，指定されたスカラー値で加算 (IDLの記法)
 */
mdarray_uintptr &mdarray_uintptr::vaddf( double value, const char *exp_fmt, va_list ap )
{
    this->mdarray::vaddf(value, exp_fmt, ap);
    return *this;
}

/**
 * @brief  自身の配列を，指定されたスカラー値で減算 (IDLの記法)
 */
mdarray_uintptr &mdarray_uintptr::subtractf( double value, const char *exp_fmt, ... )
{
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	this->mdarray::vsubtractf(value, exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vsubtractf() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  自身の配列を，指定されたスカラー値で減算 (IDLの記法)
 */
mdarray_uintptr &mdarray_uintptr::vsubtractf( double value, const char *exp_fmt, va_list ap )
{
    this->mdarray::vsubtractf(value, exp_fmt, ap);
    return *this;
}

/**
 * @brief  自身の配列に，指定されたスカラー値を乗算 (IDLの記法)
 */
mdarray_uintptr &mdarray_uintptr::multiplyf( double value, const char *exp_fmt, ... )
{
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	this->mdarray::vmultiplyf(value, exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vmultiplyf() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  自身の配列に，指定されたスカラー値を乗算 (IDLの記法)
 */
mdarray_uintptr &mdarray_uintptr::vmultiplyf( double value, const char *exp_fmt, va_list ap )
{
    this->mdarray::vmultiplyf(value, exp_fmt, ap);
    return *this;
}

/**
 * @brief  自身の配列を，指定されたスカラー値で除算 (IDLの記法)
 */
mdarray_uintptr &mdarray_uintptr::dividef( double value, const char *exp_fmt, ... )
{
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	this->mdarray::vdividef(value, exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vdividef() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  自身の配列を，指定されたスカラー値で除算 (IDLの記法)
 */
mdarray_uintptr &mdarray_uintptr::vdividef( double value, const char *exp_fmt, va_list ap )
{
    this->mdarray::vdividef(value, exp_fmt, ap);
    return *this;
}

/**
 * @brief  自身の配列に指定されたオブジェクトの配列を貼り付け (IDLの記法)
 */
mdarray_uintptr &mdarray_uintptr::pastef( const mdarray &src, const char *exp_fmt, ... )
{
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	this->mdarray::vpastef(src, exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vpastef() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  自身の配列に指定されたオブジェクトの配列を貼り付け (IDLの記法)
 */
mdarray_uintptr &mdarray_uintptr::vpastef( const mdarray &src, const char *exp_fmt, va_list ap )
{
    this->mdarray::vpastef(src, exp_fmt, ap);
    return *this;
}

/**
 * @brief  自身の配列に指定された配列を貼り付け (ユーザ関数経由・IDLの記法)
 */
mdarray_uintptr &mdarray_uintptr::pastef_via_udf( const mdarray &src,
	void (*func)(double [],double [],size_t, ssize_t,ssize_t,ssize_t,mdarray_uintptr *,void *),
	void *user_ptr,
        const char *exp_fmt, ... )
{
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	this->mdarray::vpastef_via_udf(src, 
	       (void (*)(double [],double [],size_t, ssize_t,ssize_t,ssize_t,mdarray *,void *))func, user_ptr, 
		exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vpastef() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  自身の配列に指定された配列を貼り付け (ユーザ関数経由・IDLの記法)
 */
mdarray_uintptr &mdarray_uintptr::vpastef_via_udf( const mdarray &src,
	void (*func)(double [],double [],size_t, ssize_t,ssize_t,ssize_t,mdarray_uintptr *,void *),
	void *user_ptr,
        const char *exp_fmt, va_list ap )
{
    this->mdarray::vpastef_via_udf(src, 
	(void (*)(double [],double [],size_t, ssize_t,ssize_t,ssize_t,mdarray *,void *))func, user_ptr, 
	exp_fmt, ap);
    return *this;
}

/* add an array object */
/**
 * @brief  自身の配列に，指定されたオブジェクトの配列を加算 (IDLの記法)
 */
mdarray_uintptr &mdarray_uintptr::addf( const mdarray &src, const char *exp_fmt, ... )
{
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	this->mdarray::vaddf(src, exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vaddf() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  自身の配列に，指定されたオブジェクトの配列を加算 (IDLの記法)
 */
mdarray_uintptr &mdarray_uintptr::vaddf( const mdarray &src, const char *exp_fmt, va_list ap )
{
    this->mdarray::vaddf(src, exp_fmt, ap);
    return *this;
}

/* subtract an array object */
/**
 * @brief 自身の配列を，指定されたオブジェクトの配列で減算 (IDLの記法)
 */
mdarray_uintptr &mdarray_uintptr::subtractf( const mdarray &src, const char *exp_fmt, ... )
{
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	this->mdarray::vsubtractf(src, exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vsubtractf() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief 自身の配列を，指定されたオブジェクトの配列で減算 (IDLの記法)
 */
mdarray_uintptr &mdarray_uintptr::vsubtractf( const mdarray &src, const char *exp_fmt, va_list ap )
{
    this->mdarray::vsubtractf(src, exp_fmt, ap);
    return *this;
}

/* multiply an array object */
/**
 * @brief  自身の配列に，指定されたオブジェクトの配列を乗算 (IDLの記法)
 */
mdarray_uintptr &mdarray_uintptr::multiplyf( const mdarray &src, const char *exp_fmt, ... )
{
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	this->mdarray::vmultiplyf(src, exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vmultiplyf() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  自身の配列に，指定されたオブジェクトの配列を乗算 (IDLの記法)
 */
mdarray_uintptr &mdarray_uintptr::vmultiplyf( const mdarray &src, const char *exp_fmt, va_list ap )
{
    this->mdarray::vmultiplyf(src, exp_fmt, ap);
    return *this;
}

/* divide an array object */
/**
 * @brief  自身の配列を，指定されたオブジェクトの配列で除算 (IDLの記法)
 */
mdarray_uintptr &mdarray_uintptr::dividef( const mdarray &src, const char *exp_fmt, ... )
{
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	this->mdarray::vdividef(src, exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vdividef() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  自身の配列を，指定されたオブジェクトの配列で除算 (IDLの記法)
 */
mdarray_uintptr &mdarray_uintptr::vdividef( const mdarray &src, const char *exp_fmt, va_list ap )
{
    this->mdarray::vdividef(src, exp_fmt, ap);
    return *this;
}


/* 
 * member functions for image processing
 */


/**
 * @brief  一部要素を別オブジェクトへコピー
 *
 *  画像データ向きのメンバ関数で，自身の配列の内容の一部分を指定されたオブジェ
 *  クト dest へコピーします．<br>
 *  コピー先へは，配列の要素値とコピー元の型，デフォルト値，丸めフラグの属性な
 *  どすべてをコピーします．自身(コピー元) の配列は改変されません．<br>
 *  dest_obj に自身を与えた場合，trim()を実行した場合と同じ結果になります．
 *
 * @param      dest コピー先のオブジェクト
 * @param      col_idx コピー元の列位置
 * @param      col_len コピー元の列サイズ
 * @param      row_idx コピー元の行位置
 * @param      row_len コピー元の行サイズ
 * @param      layer_idx コピー元のレイヤ位置
 * @param      layer_len コピー元のレイヤサイズ
 * @return     コピーした要素数
 * @throw      バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 *
 */
ssize_t mdarray_uintptr::copy( mdarray *dest,
			     ssize_t col_idx, size_t col_len,
			     ssize_t row_idx, size_t row_len, 
			     ssize_t layer_idx, size_t layer_len ) const
{
    debug_report("( mdarray *dest, ssize_t, ... )");
    return this->mdarray::copy(dest, col_idx,col_len,
			       row_idx, row_len, 
			       layer_idx, layer_len);
}

/* convert and copy the value of selected array element. */
/* [not implemented] */
/**
 * @brief  自身の一部要素を型変換した配列を返す
 * @deprecated  未実装
 */
ssize_t mdarray_uintptr::convert_copy( mdarray *dest,
				    ssize_t col_idx, size_t col_len,
				    ssize_t row_idx, size_t row_len, 
				    ssize_t layer_idx, size_t layer_len ) const
{
    debug_report("( mdarray *dest )");
    return this->mdarray::convert_copy(dest, col_idx, col_len,
				       row_idx, row_len, layer_idx, layer_len);
}

/* set a section to be copied by move_to() */
/**
 * @brief  次回の move_to() により移動させる領域の指定
 */
mdarray_uintptr &mdarray_uintptr::move_from(ssize_t col_idx, size_t col_len,
					  ssize_t row_idx, size_t row_len, 
					  ssize_t layer_idx, size_t layer_len)
{
    this->mdarray::move_from(col_idx,col_len, row_idx,row_len,
			     layer_idx,layer_len);
    return *this;
}

/* copy a section specified by move_from() */
/**
 * @brief  move_from() で指定された領域の移動
 */
mdarray_uintptr &mdarray_uintptr::move_to( 
		       ssize_t dest_col, ssize_t dest_row, ssize_t dest_layer )
{
    this->mdarray::move_to(dest_col, dest_row, dest_layer);
    return *this;
}

/* trim a section */
/**
 * @brief  指定部分以外の配列要素の消去 (画像データのトリミング)
 *
 *  画像データ向きのメンバ関数で，自身の配列のうち指定された部分だけを残し，
 *  指定外の部分を消去します．<br>
 *
 * @param     col_idx 列位置
 * @param     col_len 列サイズ
 * @param     row_idx 行位置
 * @param     row_len 行サイズ
 * @param     layer_idx レイヤ位置
 * @param     layer_len レイヤサイズ
 * @return    自身の参照
 * @throw     バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 */
mdarray_uintptr &mdarray_uintptr::trim( ssize_t col_idx, size_t col_len,
				      ssize_t row_idx, size_t row_len, 
				      ssize_t layer_idx, size_t layer_len )
{
    this->mdarray::trim(col_idx,col_len, row_idx,row_len, layer_idx,layer_len);
    return *this;
}


/* flip horizontal within a rectangular section */
/**
 * @brief  配列の水平方向での反転
 *
 * 自身の配列の指定された範囲の要素について，その内容を水平方向に反転させます．
 *
 * @param     col_idx 列位置
 * @param     col_len 列サイズ
 * @param     row_idx 行位置
 * @param     row_len 行サイズ
 * @param     layer_idx レイヤ位置
 * @param     layer_len レイヤサイズ
 * @return    自身の参照
 */
mdarray_uintptr &mdarray_uintptr::flip_cols( ssize_t col_idx, size_t col_len,
					  ssize_t row_idx, size_t row_len, 
					  ssize_t layer_idx, size_t layer_len )
{
    this->mdarray::flip_cols(col_idx,col_len, row_idx,row_len, 
			     layer_idx,layer_len);
    return *this;
}

/* flip vertical within a rectangular section */
/**
 * @brief  配列の垂直方向での反転
 *
 * 自身の配列の指定された範囲の要素について，その内容を垂直方向に反転させます．
 *
 * @param     col_idx 列位置
 * @param     col_len 列サイズ
 * @param     row_idx 行位置
 * @param     row_len 行サイズ
 * @param     layer_idx レイヤ位置
 * @param     layer_len レイヤサイズ
 * @return    自身の参照
 */
mdarray_uintptr &mdarray_uintptr::flip_rows( ssize_t col_idx, size_t col_len,
					  ssize_t row_idx, size_t row_len, 
					  ssize_t layer_idx, size_t layer_len )
{
    this->mdarray::flip_rows(col_idx,col_len, row_idx,row_len, 
			     layer_idx,layer_len);
    return *this;
}

/* interchange rows and columns and copy */
/**
 * @brief  (x,y)のトランスポーズを行なった配列を取得
 */
ssize_t mdarray_uintptr::transpose_xy_copy( mdarray *dest,
				    ssize_t col_idx, size_t col_len,
				    ssize_t row_idx, size_t row_len, 
				    ssize_t layer_idx, size_t layer_len ) const
{
    return this->mdarray::transpose_xy_copy( (mdarray *)dest, 
				       col_idx, col_len, row_idx, row_len, 
				       layer_idx, layer_len );
}

/* interchange x and z and copy */
/**
 * @brief  (x,y,z)→(z,x,y)のトランスポーズを行なった配列を取得
 */
ssize_t mdarray_uintptr::transpose_xyz2zxy_copy( mdarray *dest,
				    ssize_t col_idx, size_t col_len,
				    ssize_t row_idx, size_t row_len, 
				    ssize_t layer_idx, size_t layer_len ) const
{
    return this->mdarray::transpose_xyz2zxy_copy( (mdarray *)dest, 
				       col_idx, col_len, row_idx, row_len, 
				       layer_idx, layer_len );
}

/* rotate and copy a section to another mdarray object       */
/*   angle: 90,-90, or 180                                   */
/*          (anticlockwise when image is bottom-left origin) */
/**
 * @brief  (x,y)で回転させた配列を取得 (回転は90度単位)
 */
ssize_t mdarray_uintptr::rotate_xy_copy( mdarray *dest, int angle,
				    ssize_t col_idx, size_t col_len,
				    ssize_t row_idx, size_t row_len, 
				    ssize_t layer_idx, size_t layer_len ) const
{
    return this->mdarray::rotate_xy_copy( (mdarray *)dest, angle, 
					  col_idx, col_len, row_idx, row_len, 
					  layer_idx, layer_len);
}


/**
 * @brief  自身の配列をデフォルト値でパディング
 *
 *  自身の配列の要素をデフォルト値でパディングします．<br>
 *  引数は指定しなくても使用できます．その場合は，全要素が処理対象です．
 *  clean() を実行しても配列長は変化しません．<br>
 *  画像データ向きのメンバ関数です．<br>
 *
 * @param      col_index 列位置
 * @param      col_size 列サイズ
 * @param      row_index 行位置
 * @param      row_size 行サイズ
 * @param      layer_index レイヤ位置
 * @param      layer_size レイヤサイズ
 * @return     自身の参照
 *
 */
mdarray_uintptr &mdarray_uintptr::clean( ssize_t col_index, size_t col_size,
				       ssize_t row_index, size_t row_size,
				       ssize_t layer_index, size_t layer_size )
{
    this->mdarray::clean(col_index,col_size,row_index,row_size,
			 layer_index,layer_size);
    return *this;
}


/**
 * @brief  自身の配列を指定されたスカラー値で書き換え
 *
 *  自身の配列の指定された範囲の要素を，指定された値で書換えます．<br>
 *  画像データ向きのメンバ関数です．
 *
 * @param      value 書き込む値
 * @param      col_index 列位置
 * @param      col_size 列サイズ
 * @param      row_index 行位置
 * @param      row_size 行サイズ
 * @param      layer_index レイヤ位置
 * @param      layer_size レイヤサイズ
 * @param      func 値変換の為のユーザ関数のアドレス
 * @return     自身の参照
 *
 */
mdarray_uintptr &mdarray_uintptr::fill( double value, 
			 ssize_t col_index, size_t col_size,
			 ssize_t row_index, size_t row_size,
			 ssize_t layer_index, size_t layer_size )
{
    debug_report("( double value, ssize_t ... )");
    this->mdarray::fill(value, col_index, col_size, 
			row_index, row_size, layer_index, layer_size);
    return *this;
}

/**
 * @brief  自身の配列を指定されたスカラー値で書き換え (ユーザ関数経由)
 *
 *  自身の配列の指定された範囲の要素を，指定された値とユーザ定義関数経由で書換
 *  えます．<br>
 *  ユーザ定義関数 func の引数には順に，自身の要素値(配列)，value で与えられた
 *  値，配列(最初の引数)の個数，列位置，行位置，レイヤ位置，自身のオブジェクト
 *  のアドレス，ユーザポインタ user_ptr の値，が与えられます．<br>
 *  画像データ向きのメンバ関数です．
 *
 * @param      value 書き込む値
 * @param      func 値変換の為のユーザ関数のアドレス
 * @param      user_ptr func の最後に与えられるユーザのポインタ
 * @param      col_index 列位置
 * @param      col_size 列サイズ
 * @param      row_index 行位置
 * @param      row_size 行サイズ
 * @param      layer_index レイヤ位置
 * @param      layer_size レイヤサイズ
 * @return     自身の参照
 *
 */
mdarray_uintptr &mdarray_uintptr::fill_via_udf( double value, 
	void (*func)(double [],double,size_t, ssize_t,ssize_t,ssize_t,mdarray_uintptr *,void *),
	void *user_ptr,
	ssize_t col_index, size_t col_size,
	ssize_t row_index, size_t row_size,
	ssize_t layer_index, size_t layer_size )
{
    debug_report("( double value, double (*func)() ... )");
    this->mdarray::fill_via_udf(value, 
			(void (*)(double [],double,size_t, ssize_t,ssize_t,ssize_t,mdarray *,void *))func,
			user_ptr, col_index, col_size,
			row_index, row_size,
			layer_index, layer_size);
    return *this;
}

/**
 * @brief  自身の配列に，指定されたスカラー値で加算
 *
 *  自身の配列の指定された範囲の要素に value を加算します．
 *  画像データ向きのメンバ関数です．
 *
 * @param      value 加算する値
 * @param      col_index 列位置
 * @param      col_size 列サイズ
 * @param      row_index 行位置
 * @param      row_size 行サイズ
 * @param      layer_index レイヤ位置
 * @param      layer_size レイヤサイズ
 * @return     自身の参照
 *
 */
mdarray_uintptr &mdarray_uintptr::add( double value, 
		    ssize_t col_index, size_t col_size,
		    ssize_t row_index, size_t row_size,
		    ssize_t layer_index, size_t layer_size )
{
    this->mdarray::add(value,col_index,col_size,row_index,row_size,
		       layer_index,layer_size);
    return *this;
}

/**
 * @brief  自身の配列を，指定されたスカラー値で減算
 *
 *  自身の配列の指定された範囲の要素から value を減算します．
 *  画像データ向きのメンバ関数です．
 *
 * @param      value 減算する値
 * @param      col_index 列位置
 * @param      col_size 列サイズ
 * @param      row_index 行位置
 * @param      row_size 行サイズ
 * @param      layer_index レイヤ位置
 * @param      layer_size レイヤサイズ
 * @return     自身の参照
 *
 */
mdarray_uintptr &mdarray_uintptr::subtract( double value, 
				       ssize_t col_index, size_t col_size,
				       ssize_t row_index, size_t row_size,
				       ssize_t layer_index, size_t layer_size )
{
    this->mdarray::subtract(value,col_index,col_size,row_index,row_size,
			    layer_index,layer_size);
    return *this;
}

/**
 * @brief  自身の配列に，指定されたスカラー値を乗算
 *
 * 自身の配列の指定された範囲の要素の値にvalue を乗算します．
 * 画像データ向きのメンバ関数です．
 *
 * @param      value 乗算する値
 * @param      col_index 列位置
 * @param      col_size 列サイズ
 * @param      row_index 行位置
 * @param      row_size 行サイズ
 * @param      layer_index レイヤ位置
 * @param      layer_size レイヤサイズ
 * @return     自身の参照
 *
 */
mdarray_uintptr &mdarray_uintptr::multiply( double value, 
		    ssize_t col_index, size_t col_size,
		    ssize_t row_index, size_t row_size,
		    ssize_t layer_index, size_t layer_size )
{
    this->mdarray::multiply(value,col_index,col_size,row_index,row_size,
			    layer_index,layer_size);
    return *this;
}

/**
 * @brief  自身の配列を，指定されたスカラー値で除算
 *
 *  自身の配列の指定された範囲の要素について value で除算します．
 *  画像データ向きのメンバ関数です．
 *
 * @param      value 除算する値
 * @param      col_index 列位置
 * @param      col_size 列サイズ
 * @param      row_index 行位置
 * @param      row_size 行サイズ
 * @param      layer_index レイヤ位置
 * @param      layer_size レイヤサイズ
 * @return     自身の参照
 *
 */
mdarray_uintptr &mdarray_uintptr::divide( double value, 
				       ssize_t col_index, size_t col_size,
				       ssize_t row_index, size_t row_size,
				       ssize_t layer_index, size_t layer_size )
{
    this->mdarray::divide(value,col_index,col_size,row_index,row_size,
			  layer_index,layer_size);
    return *this;
}

/**
 * @brief  自身の配列に指定されたオブジェクトの配列を貼り付け
 *
 *  自身の配列の指定された範囲の要素値に，src で指定されたオブジェクトの
 *  各要素値を貼り付けます．<br>
 *  画像データ向きのメンバ関数です．
 *
 * @param      src 源泉となる配列を持つオブジェクト
 * @param      dest_col 列位置
 * @param      dest_row 行位置
 * @param      dest_layer レイヤ位置
 * @return     自身の参照
 *
 */
mdarray_uintptr &mdarray_uintptr::paste( const mdarray &src,
	ssize_t dest_col, ssize_t dest_row, ssize_t dest_layer )
{
    debug_report("( const mdarray &src, ssize_t ... )");
    this->mdarray::paste(src,dest_col,dest_row,dest_layer);
    return *this;
}

/**
 * @brief  自身の配列に指定されたオブジェクトの配列を貼り付け (ユーザ関数経由)
 *
 *  自身の配列の指定された範囲の要素値に，src で指定されたオブジェクトの各要素
 *  値をユーザ定義関数経由で貼り付けます．ユーザ定義関数を与えて貼り付け時の
 *  挙動を変えることができます．<br>
 *  ユーザ定義関数 func の引数には順に，自身の要素値(配列)，オブジェクト src の
 *  要素値(配列)，最初の2つの引数の配列の個数，列位置，行位置，レイヤ位置，自身
 *  のオブジェクトのアドレス，ユーザポインタ user_ptr の値 が与えられます．<br>
 *  画像データ向きのメンバ関数です．
 *
 * @param      src 源泉となる配列を持つオブジェクト
 * @param      func 値変換のためのユーザ関数のアドレス
 * @param      user_ptr func の最後に与えられるユーザのポインタ
 * @param      dest_col 列位置
 * @param      dest_row 行位置
 * @param      dest_layer レイヤ位置
 * @return     自身の参照
 *
 */
mdarray_uintptr &mdarray_uintptr::paste_via_udf( const mdarray &src,
	void (*func)(double [],double [],size_t,ssize_t,ssize_t,ssize_t,mdarray_uintptr *,void *),
	void *user_ptr,
	ssize_t dest_col, ssize_t dest_row, ssize_t dest_layer )
{
    debug_report("( const mdarray &src, double (*func)() ... )");
    this->mdarray::paste_via_udf(src,
		(void (*)(double [],double [],size_t,ssize_t,ssize_t,ssize_t,mdarray *,void *))func,
		user_ptr,
		dest_col, dest_row, dest_layer);
    return *this;
}


/**
 * @brief  自身の配列に，指定されたオブジェクトの配列を加算
 *
 *  自身の要素にオブジェクト src_img が持つ配列を加算します．
 *  列・行・レイヤについてそれぞれの加算適用開始位置を指定できます．<br>
 *  画像データ向きのメンバ関数です．
 *
 * @param      src_img 演算に使う配列を持つオブジェクト
 * @param      dest_col 加算開始位置(列)
 * @param      dest_row 加算開始位置(行)
 * @param      dest_layer 加算開始位置(レイヤ)
 * @return     自身の参照
 *
 */
mdarray_uintptr &mdarray_uintptr::add( const mdarray &src_img,
	  ssize_t dest_col, ssize_t dest_row, ssize_t dest_layer )
{
    this->mdarray::add(src_img,dest_col,dest_row,dest_layer);
    return *this;
}

/**
 * @brief  自身の配列を，指定されたオブジェクトの配列で減算
 *
 *  自身の配列の要素値からオブジェクト src_img が持つ配列の要素値を減算します．
 *  列・行・レイヤについてそれぞれの減算適用開始位置を指定できます．<br>
 *  画像データ向きのメンバ関数です．
 *
 * @param      src_img 演算に使う配列を持つオブジェクト
 * @param      dest_col 減算開始位置(列)
 * @param      dest_row 減算開始位置(行)
 * @param      dest_layer 減算開始位置(レイヤ)
 * @return     自身の参照
 *
 */
mdarray_uintptr &mdarray_uintptr::subtract( const mdarray &src_img,
	  ssize_t dest_col, ssize_t dest_row, ssize_t dest_layer )
{
    this->mdarray::subtract(src_img,dest_col,dest_row,dest_layer);
    return *this;
}

/**
 * @brief  自身の配列に，指定されたオブジェクトの配列を乗算
 *
 *  自身の配列の要素値にオブジェクト src_img が持つ配列を乗算します．
 *  列・行・レイヤについてそれぞれの乗算適用開始位置を指定できます．<br>
 *  画像データ向きのメンバ関数です．
 *
 * @param      src_img 演算に使う配列を持つオブジェクト
 * @param      dest_col 乗算開始位置(列)
 * @param      dest_row 乗算開始位置(行)
 * @param      dest_layer 乗算開始位置(レイヤ)
 * @return     自身の参照
 *
 */
mdarray_uintptr &mdarray_uintptr::multiply( const mdarray &src_img,
	  ssize_t dest_col, ssize_t dest_row, ssize_t dest_layer )
{
    this->mdarray::multiply(src_img,dest_col,dest_row,dest_layer);
    return *this;
}

/**
 * @brief  自身の配列を，指定されたオブジェクトの配列で除算
 *
 *  自身の配列の要素値からオブジェクト src_img が持つ配列を除算します．
 *  列・行・レイヤについてそれぞれの除算適用開始位置を指定できます．<br>
 *  画像データ向きのメンバ関数です．
 *
 * @param      src_img 演算に使う配列を持つオブジェクト
 * @param      dest_col 除算開始位置(列)
 * @param      dest_row 除算開始位置(行)
 * @param      dest_layer 除算開始位置(レイヤ)
 * @return     自身の参照
 *
 */
mdarray_uintptr &mdarray_uintptr::divide( const mdarray &src_img,
	  ssize_t dest_col, ssize_t dest_row, ssize_t dest_layer )
{
    this->mdarray::divide(src_img,dest_col,dest_row,dest_layer);
    return *this;
}


/**
 * @brief  ユーザのポインタ変数の登録
 *
 *  ユーザのポインタ変数をオブジェクトに登録します．<br>
 *  このメンバ関数を使ってユーザのポインタ変数のアドレスを登録すれば，オブジェ
 *  クトが管理するバッファの先頭アドレスを常にユーザのポインタ変数に保持させて
 *  おく事ができます．
 *
 * @param   extptr_address ユーザのポインタ変数のアドレス
 * @return  自身の参照
 * @note    引数を与える時に，ポインタ変数に「&」をつけるのを忘れないように
 *          しましょう
 * @attention  ユーザのポインタ変数を登録すると shallow copy が機能しなくなり
 *             ます．
 */
mdarray_uintptr &mdarray_uintptr::register_extptr( uintptr_t **extptr_address )
{
    this->mdarray::register_extptr((void *)extptr_address);
    return *this;
}

/**
 * @brief  ユーザのポインタ変数(2d用)の登録
 *
 *  ユーザのポインタ変数(2d用)をオブジェクトに登録します．<br>
 *  このメンバ関数を使ってユーザのポインタ変数のアドレスを登録すれば，オブジェ
 *  クトが管理するアドレステーブルの先頭アドレスを常にユーザのポインタ変数にさ
 *  保持せておく事ができます．
 *
 * @param   extptr2d_address ユーザのポインタ変数のアドレス
 * @return  自身の参照
 * @note    引数を与える時に，ポインタ変数に「&」をつけるのを忘れないように
 *          しましょう
 * @attention  ユーザのポインタ変数を登録すると shallow copy が機能しなくなり
 *             ます．
 */
mdarray_uintptr &mdarray_uintptr::register_extptr_2d( uintptr_t *const **extptr2d_address )
{
    this->mdarray::register_extptr_2d((void *)extptr2d_address);
    return *this;
}

/**
 * @brief  ユーザのポインタ変数(3d用)の登録
 *
 *  ユーザのポインタ変数(3d用)をオブジェクトに登録します．<br>
 *  このメンバ関数を使ってユーザのポインタ変数のアドレスを登録すれば，オブジェ
 *  クトが管理するアドレステーブルの先頭アドレスを常にユーザのポインタ変数にさ
 *  保持せておく事ができます．
 *
 * @param   extptr3d_address ユーザのポインタ変数のアドレス
 * @return  自身の参照
 * @note    引数を与える時に，ポインタ変数に「&」をつけるのを忘れないように
 *          しましょう
 * @attention  ユーザのポインタ変数を登録すると shallow copy が機能しなくなり
 *             ます．
 */
mdarray_uintptr &mdarray_uintptr::register_extptr_3d( uintptr_t *const *const **extptr3d_address )
{
    this->mdarray::register_extptr_3d((void *)extptr3d_address);
    return *this;
}

/**
 * @brief  バイトオーダの調整
 *
 *  このメンバ関数は，自身の配列をバイナリデータとしてファイルに保存したい時，
 *  あるいはファイルのバイナリデータを自身の配列に取り込みたい時に使います．
 *  (詳細は reverse_endian( bool, size_t, size_t ) を参照)
 *
 * @param     is_little_endian 1 回目の処理後のメモリ上のエンディアン
 * @return    自身の参照
 * @note      高速化のため，SIMD命令を使っています．
 *
 */
mdarray_uintptr &mdarray_uintptr::reverse_endian( bool is_little_endian )
{
    this->mdarray::reverse_byte_order(is_little_endian, 0, 0, this->length());
    return *this;
}

/**
 * @brief  バイトオーダの調整
 *
 * このメンバ関数は，自身の配列をバイナリデータとしてファイルに保存したい時，
 * あるいはファイルのバイナリデータを自身の配列に取り込みたい時に使います．<br>
 * ファイルにデータを保存したい時は，このメンバ関数を呼び出してファイル保存に適
 * したエンディアンに変換し，array_ptr() メンバ関数などで取得したアドレスをスト
 * リーム書き込み用の関数に与えて内容を書き込んだ後，再度このメンバ関数を呼び出
 * して，エンディアンを元に戻します．<br>
 * ファイルからデータを読み込みたい時は，array_ptr() メンバ関数などで取得したア
 * ドレスをストリーム読み取り用の関数に与えて内容を読み込んだ後，このメンバ関数
 * を呼び出して処理系に適したエンディアンに変換します．<br>
 * 上記のいずれの場合も，ファイルに保存されるべきデータがビッグエンディアンなら
 * ば，第1引数に false をセットしまず(リトルエンディアンなら true です)．<br>
 * このメンバ関数は，処理系によって使い分けが必要とならないように作られていま
 * す．例えば，ファイルにビッグエンディアンのデータを保存したいので，
 * is_little_endian に false を指定し，このメンバ関数を呼び出したとします．
 * この時，マシンがビッグエンディアンであれば，実際には反転処理は行われません
 * (マシンがリトルエンディアンであれば，反転処理が行われます)．
 * 次に，オブジェクト内のバイナリデータをそのままファイルに保存すれば，
 * 指定されたバイトオーダーのバイナリファイルができます．
 * その後，再度同じ引数でこのメンバ関数を呼び出して，エンディアンが反転されてい
 * る場合は元に戻す処理を行います．<br>
 * 従って，ファイルへの保存に際しては，このメンバ関数は同じ引数で２回呼び出す事
 * が前提です．
 *
 * @param     is_little_endian 1 回目の処理後のメモリ上のエンディアン
 * @param     begin 処理を開始する要素番号
 * @param     length 処理対象となる長さ
 * @return    自身の参照
 * @note      高速化のため，SIMD命令を使っています．
 *
 */
mdarray_uintptr &mdarray_uintptr::reverse_endian( bool is_little_endian, 
						size_t begin, size_t length )
{
    this->mdarray::reverse_byte_order(is_little_endian, 0, begin, length);
    return *this;
}

/* protected */

/**
 * @brief  継承クラスにおける，デフォルト型の設定
 *
 *  自身が扱うべき型を基底クラス(mdarray)のメンバ関数に通知します．
 * 
 * @return  型種別 (固定値: uintptr_t)
 * @note    このメンバ関数はprotectedです．
 */
ssize_t mdarray_uintptr::default_size_type()
{
    return (ssize_t)UINTPTR_ZT;
}

/**
 * @brief  継承クラスにおける，受け入れ可能な型の設定
 *
 *  受け入れる型かどうかを基底クラス(mdarray)のメンバ関数に通知します．
 * 
 * @param   sz_type 型種別
 * @return  受け入れ可能な型(uintptr_t)なら真
 * @note    このメンバ関数はprotectedです．
 */
bool mdarray_uintptr::is_acceptable_size_type( ssize_t sz_type )
{
    if ( sz_type == (ssize_t)UINTPTR_ZT ) return true;
    else return false;
}

}	/* namespace */

#include "private/s_memset.cc"
#include "private/s_memmove.cc"

#undef CLASS_NAME

