/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2013-05-14 17:25:27 cyamauch> */

#ifndef _SLI__TARRAY_H
#define _SLI__TARRAY_H 1

/**
 * @file   tarray.h
 * @brief  任意クラスのオブジェクト配列を扱う tarray クラスのコード
 */

#include <stddef.h>
#include <sys/types.h>

#include "sli_config.h"
#include "stdstreamio.h"
#include "mdarray.h"

#include "slierr.h"

namespace sli
{

/*
 * tarray template class can handle arrays of arbitrary data types or classes.
 * Its function is almost the same as that of tarray_tstring,  except that this
 * class does not include member functions specializing in string.
 */

/**
 * @class  sli::tarray
 * @brief  任意クラスのオブジェクト配列を扱うためのテンプレートクラス
 *
 *   tarray テンプレートクラスは，任意クラスのオブジェクト配列を扱う事ができま
 *   す．<br>
 *   内部実装において，ある程度の規模のクラスを仮定しており，このテンプレート
 *   クラスでプリミティブ型を扱うメリットはありません．プリミティブ型の配列には
 *   mdarray，mdarray_int，mdarray_float などをご利用ください．
 *
 * @author Chisato YAMAUCHI
 * @date 2013-04-01 00:00:00
 */

  template <class datatype> class tarray
  {

  public:
    /* constructor and destructor */
    tarray();
    tarray(const tarray &obj);
    virtual ~tarray();

    /*
     * operator
     */
    /* same as .init(sobj) */
    virtual tarray &operator=(const tarray &obj);

    /* same as .append(sobj,0) */
    virtual tarray &operator+=(const tarray &obj);

    /* same as .append(sobj,1) */
    virtual tarray &operator+=(const datatype &one);

    /* same as .at(key) */
    virtual datatype &operator[]( size_t index );
    virtual const datatype &operator[]( size_t index ) const;

    /*
     * member functions
     */
    /* complete initialization of objects */
    virtual tarray &init();
    virtual tarray &init(const tarray &obj);

    /* Padding of the existing values in an array by arbitrary ones */
    virtual tarray &clean(const datatype &one);

    /* substitute the object */
    virtual tarray &assign( const datatype &one, size_t n );
    virtual tarray &assign( const tarray &src, size_t idx2 = 0 );
    virtual tarray &assign( const tarray &src, 
			    size_t idx2, size_t n2 );

    /* append an element */
    virtual tarray &append( const datatype &one, size_t n );
    virtual tarray &append( const tarray &src, size_t idx2 = 0 );
    virtual tarray &append( const tarray &src, size_t idx2, size_t n2 );

    /* insert an element */
    virtual tarray &insert(size_t index, const datatype &one, size_t n);
    virtual tarray &insert( size_t index, 
			    const tarray &src, size_t idx2 = 0 );
    virtual tarray &insert( size_t index, 
			    const tarray &src, size_t idx2, size_t n2 );

    /* replace an element */
    virtual tarray &replace( size_t idx1, size_t n1, 
			     const datatype &one, size_t n2 );
    virtual tarray &replace( size_t idx1, size_t n1,
			     const tarray &src, size_t idx2 = 0 );
    virtual tarray &replace( size_t idx1, size_t n1,
			     const tarray &src, size_t idx2, size_t n2 );

    /* put a value to an arbitrary element's point */
    virtual tarray &put( size_t index, const datatype &one, size_t n );
    virtual tarray &put( size_t index, 
			 const tarray &src, size_t idx2 = 0);
    virtual tarray &put( size_t index, 
			 const tarray &src, size_t idx2, size_t n2 );

    /* change the length of the array */
    virtual tarray &resize( size_t new_num_elements );

    /* change the length of the array relatively */
    virtual tarray &resizeby( ssize_t len );

    /* cutout elements */
    virtual tarray &crop( size_t idx, size_t len );
    virtual tarray &crop( size_t idx );

    /* erase element(s) */
    virtual tarray &erase();
    virtual tarray &erase( size_t index, size_t num_elements = 1 );

    /* copy all or some elements to another object */
    virtual ssize_t copy( size_t index, size_t n, tarray *dest ) const;
    virtual ssize_t copy( size_t index, tarray *dest ) const;
    virtual ssize_t copy( tarray *dest ) const;
    /* not recommended */
    virtual ssize_t copy( size_t index, size_t n, tarray &dest ) const;
    /* not recommended */
    virtual ssize_t copy( size_t index, tarray &dest ) const;
    /* not recommended */
    virtual ssize_t copy( tarray &dest ) const;

    /* swap self and another */
    virtual tarray &swap( tarray &sobj );

    /* a reference to the specified element */
    virtual datatype &at( size_t index );
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const datatype &at( size_t index ) const;
#endif
    virtual const datatype &at_cs( size_t index ) const;

    /* length of the array (the number of elements) */
    virtual size_t length() const;
    /* not recommended */
    virtual size_t size() const;

    /* register a function to create object of datatype class */
    virtual void register_creator( datatype *(*func)(void *), void *user_ptr );
    virtual const datatype &initial_one() const;

    /* 一時オブジェクトの return の直前に使い，shallow copy を許可する場合に */
    /* 使う (未実装)                                                         */
    virtual void set_scopy_flag();

  private:
    bool request_shallow_copy( datatype *from_obj ) const;

    /* エラーハンドリング */
    void err_throw( const char *func0, const char *level0, 
		    const char *message0 ) const;

  private:
    mdarray arrs_rec;
    /* 初期値をキープする(要delete) */
    datatype *initial_one_rec;
    /* datatypeクラスのオブジェクトを作る関数 */
    datatype *(*creator)(void *);
    void *creator_arg;

    /* shallow copy のためのフラグ．= 等ではコピーされない */
    /* コピーの src 側のフラグ */
    bool shallow_copy_ok;		/* set_scopy_flag() でセットされる */
    datatype *shallow_copy_dest_obj;	/* コピー先: 有効なら non-NULL */

    /* コピーの dest 側のフラグ(コピー元から書き込まれる) */
    datatype *shallow_copy_src_obj;	/* コピー元: 生きていれば non-NULL */

    /* __shallow_init(), __deep_init() を安全に行なうためのフラグ */
    bool __copying;

    /*
      gcc でエラーを出すための，ダミーのメンバ関数． 

      参照と整数とのオーバーロードはかなりヤバいようだ…
      というわけで，private に退避．．．
     */
    tarray &assign( size_t n );
    tarray &append( size_t n );
    tarray &insert( size_t index, size_t n );
    tarray &replace( size_t idx1, size_t n1, size_t n2 );
    tarray &put( size_t index, size_t n );

  };
}

/* SOURCE */

namespace sli
{

/* constructor */
template <class datatype>
/**
 * @brief  コンストラクタ
 *
 * @throw  内部バッファの確保に失敗した場合
 * @throw  メモリ破壊を起こしたとき
 */
tarray<datatype>::tarray()
{
    static datatype one;
    try {
	this->initial_one_rec = new datatype;
	*(this->initial_one_rec) = one;
    }
    catch (...) {
	this->err_throw(__FUNCTION__,"FATAL","new failed");
    }

    this->creator = NULL;
    this->creator_arg = NULL;

    this->arrs_rec.init(sizeof(datatype *), true);

    this->shallow_copy_ok = false;
    this->shallow_copy_dest_obj = NULL;
    this->shallow_copy_src_obj = NULL;
    this->__copying = false;

    return;
}

/* copy constructor */
template <class datatype>
/**
 * @brief  コピーコンストラクタ
 *
 *  自身をobjの内容で初期化します
 *
 * @param  obj tarray<datatype>オブジェクト
 * @throw  内部バッファの確保に失敗した場合
 * @throw  メモリ破壊を起こしたとき
 */
tarray<datatype>::tarray(const tarray<datatype> &obj)
{
    static datatype one;
    try {
	this->initial_one_rec = new datatype;
	*(this->initial_one_rec) = one;
    }
    catch (...) {
	this->err_throw(__FUNCTION__,"FATAL","new failed");
    }

    this->creator = NULL;
    this->creator_arg = NULL;

    this->arrs_rec.init(sizeof(datatype *), true);

    this->shallow_copy_ok = false;
    this->shallow_copy_dest_obj = NULL;
    this->shallow_copy_src_obj = NULL;
    this->__copying = false;

    this->init(obj);

    return;
}

/* destructor */
template <class datatype>
/**
 * @brief  デストラクタ
 *
 */
tarray<datatype>::~tarray()
{
    size_t i;
    for ( i=0 ; i < this->arrs_rec.length() ; i++ ) {
	datatype *objp = (datatype *)(this->arrs_rec.p(i));
	if ( objp != NULL ) {
	    delete objp;
	}
    }
    if ( this->initial_one_rec != NULL ) {
	delete this->initial_one_rec;
    }
    return;
}

template <class datatype>
/**
 * @brief  オブジェクトのコピー
 *
 *  演算子の右側(引数) で指定された同じ型・クラスを扱うオブジェクトを自身に
 *  コピーします．
 *
 * @param     obj 同じ型・クラスを扱う配列を持つオブジェクト
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合
 * @throw     メモリ破壊を起こした場合
 */
tarray<datatype> &tarray<datatype>::operator=(const tarray<datatype> &obj)
{
    this->init(obj);
    return *this;
}


template <class datatype>
/**
 * @brief  自身の配列の最後に，配列を追加
 *
 *  自身の配列に，演算子の右側(引数) で指定された配列の追加を行います.
 *
 * @param     obj 同じ型・クラスを扱う配列を持つオブジェクト
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合
 * @throw     メモリ破壊を起こした場合
 */
tarray<datatype> &tarray<datatype>::operator+=(const tarray<datatype> &obj)
{
    this->append(obj,0);
    return *this;
}

template <class datatype>
/**
 * @brief  自身の配列の最後に，1 要素を追加
 *
 *  自身の配列に，演算子の右側(引数) で指定された1 要素を追加します.
 *
 * @param     one 追加する要素
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合
 */
tarray<datatype> &tarray<datatype>::operator+=(const datatype &one)
{
    this->append(one,1);
    return *this;
}

template <class datatype>
/**
 * @brief  [] で指定された要素値の参照を返す
 *
 *  添え字で指定された配列要素の参照を返します．<br>
 *  "[]"の直後に "." で接続し，type クラスのメンバ関数を使う事ができます．<br>
 *  配列長以上の index が指定された場合は，新しい配列要素が作られます．
 *  バッファの確保に失敗した場合を除き，例外は発生しません．
 *
 * @param     index 0 から始まる要素番号
 * @return    添え字で指定された配列要素の参照
 * @throw     内部バッファの確保に失敗した場合
 */
inline datatype &tarray<datatype>::operator[]( size_t index )
{
    return this->at(index);
}

template <class datatype>
/**
 * @brief  [] で指定された要素値の参照を返す (読取専用)
 *
 *  添え字で指定された配列要素の参照を返します．<br>
 *  "[]"の直後に "." で接続し，type クラスのメンバ関数を使う事ができます．<br>
 *  読み取り専用で，index に配列の長さ以上の値が指定された場合，例外が発生
 *  します．
 *
 * @param     index 0 から始まる要素番号
 * @return    添え字で指定された配列要素の参照
 * @throw     index に配列長以上の値が指定された場合
 */
inline const datatype &tarray<datatype>::operator[]( size_t index ) const
{
    return this->at_cs(index);
}

/*
 * public member functions
 */

template <class datatype>
/**
 * @brief  オブジェクトの初期化
 *
 *  オブジェクトを初期化し，配列バッファに割り当てられたメモリ領域は開放され
 *  ます．
 *
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合
 */
tarray<datatype> &tarray<datatype>::init()
{
    size_t i;
    for ( i=0 ; i < this->arrs_rec.length() ; i++ ) {
	datatype *objp = (datatype *)(this->arrs_rec.p(i));
	if ( objp != NULL ) {
	    delete objp;
	}
    }
    this->arrs_rec.init(sizeof(datatype *), true);
    this->register_creator(NULL,NULL);
    return *this;
}

template <class datatype>
/**
 * @brief  オブジェクトのコピー
 *
 *  指定されたオブジェクト obj の内容で初期化します (obj の内容すべてを自身に
 *  コピーします)．
 *
 * @param     obj tarray クラスのオブジェクト(コピー元)
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合
 * @throw     メモリ破壊を起こした場合
 */
tarray<datatype> &tarray<datatype>::init(const tarray<datatype> &obj)
{
    if ( &obj == this ) return *this;
    tarray<datatype>::init();
    this->register_creator(obj.creator, obj.creator_arg);

    return this->append(obj,0);
}

template <class datatype>
/**
 * @brief  既存の配列全体を指定された値でパディング
 *
 *  自身が持つ配列の全要素を値 one でパディングします．
 *  clean() を実行しても配列長は変化しません．
 *
 * @param     one 配列をパディングするための値
 * @return    自身の参照
 * @throw     要素内バッファの確保に失敗した場合
 */
tarray<datatype> &tarray<datatype>::clean(const datatype &one)
{
    size_t i;
    for ( i=0 ; i < this->length() ; i++ ) {
	datatype *objp = (datatype *)(this->arrs_rec.p(i));
	*(objp) = one;
    }
    return *this;
}

/**
 * @brief  コンパイル時にエラーを出力させるためのダミーのメンバ関数
 * @deprecated  使用不可．
 * @note	このメンバ関数はprivateです
 */
template <class datatype>
tarray<datatype> &tarray<datatype>::assign( size_t n )
{
    return this->replace(0, this->length(), n);
}

template <class datatype>
/**
 * @brief  指定された値をn個 自身に代入
 *
 *  自身の配列要素をn個とし，すべての要素に指定された値を代入します．
 *
 * @param     one 源泉となる値
 * @param     n 値one を書き込む個数
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合
 */
tarray<datatype> &tarray<datatype>::assign( const datatype &one, size_t n )
{
    return this->replace(0, this->length(), one, n);
}


template <class datatype>
/**
 * @brief  指定された配列の(一部)要素を自身に代入
 *
 *  srcで指定された配列の全部または一部を，自身に代入します．
 *
 * @param     src 源泉となる配列要素を持つオブジェクト
 * @param     idx2 src の要素の開始番号
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合
 */
tarray<datatype> &tarray<datatype>::assign(const tarray<datatype> &src,
					   size_t idx2)
{
    return this->replace(0, this->length(), src, idx2, src.length());
}

template <class datatype>
/**
 * @brief  指定された配列の(一部)要素を自身に代入
 *
 *  srcで指定された配列の全部または一部を，自身に代入します．
 *
 * @param     src 源泉となる配列要素を持つオブジェクト
 * @param     idx2 src の要素の開始番号
 * @param     n2 src から取り出す要素の個数
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合
 */
tarray<datatype> &tarray<datatype>::assign( const tarray<datatype> &src, 
					    size_t idx2, size_t n2 )
{
    return this->replace(0, this->length(), src, idx2, n2);
}

/**
 * @brief  コンパイル時にエラーを出力させるためのダミーのメンバ関数
 * @deprecated  使用不可．
 * @note	このメンバ関数はprivateです
 */
template <class datatype>
tarray<datatype> &tarray<datatype>::append( size_t n )
{
    return this->replace(this->length(), 0, n);
}

template <class datatype>
/**
 * @brief  自身の配列の最後に，指定された値をn個追加
 *
 *  自身の配列の最後に，指定された値を持つ要素n個を追加します．
 *
 * @param     one 源泉となる値
 * @param     n 値one を書き込む個数
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合
 */
tarray<datatype> &tarray<datatype>::append( const datatype &one, size_t n )
{
    return this->replace(this->length(), 0, one, n);
}

template <class datatype>
/**
 * @brief  配列の(一部)要素を，自身の配列の最後に追加
 *
 *  srcで指定された配列を，自身の配列の最後尾以降に追加します．
 *
 * @param     src 源泉となる配列要素を持つオブジェクト
 * @param     idx2 src の要素の開始番号
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合
 */
tarray<datatype> &tarray<datatype>::append(const tarray<datatype> &src,
					   size_t idx2)
{
    return this->replace(this->length(), 0, src, idx2, src.length());
}

template <class datatype>
/**
 * @brief  配列の(一部)要素を，自身の配列の最後に追加
 *
 *  srcで指定された配列を，自身の配列の最後尾以降に追加します．
 *
 * @param     src 源泉となる配列要素を持つオブジェクト
 * @param     idx2 src の要素の開始番号
 * @param     n2 src から取り出す要素の個数
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合
 */
tarray<datatype> &tarray<datatype>::append( const tarray<datatype> &src, 
					    size_t idx2, size_t n2 )
{
    return this->replace(this->length(), 0, src, idx2, n2);
}

/**
 * @brief  コンパイル時にエラーを出力させるためのダミーのメンバ関数
 * @deprecated  使用不可．
 * @note	このメンバ関数はprivateです
 */
template <class datatype>
tarray<datatype> &tarray<datatype>::insert( size_t index, size_t n )
{
    return this->replace(index,0, n);
}

template <class datatype>
/**
 * @brief  自身の配列の指定位置に，指定された値をn個挿入
 *
 *  自身の配列の要素番号indexの位置に，指定された値を持つ要素n個を挿入
 *  します．<br>
 *  index に配列の長さ以上の値を指定する場合，index に自身の配列の長さ
 *  を与えたものとみなします．
 *
 * @param     index 自身の配列の挿入位置
 * @param     one 源泉となる値
 * @param     n 値one を書き込む個数
 * @return    自身の参照
 */
tarray<datatype> &tarray<datatype>::insert( size_t index, 
					    const datatype &one, size_t n )
{
    return this->replace(index,0, one,n);
}

template <class datatype>
/**
 * @brief  配列の(一部)要素を，自身の配列の指定位置に挿入
 *
 *  srcで指定された配列を，自身の配列の指定位置indexに挿入します．<br>
 *  index に配列の長さ以上の値を指定する場合，index に自身の配列の長さを
 *  与えたものとみなします．
 *
 * @param     index 自身の配列の挿入位置
 * @param     src 源泉となる配列要素を持つオブジェクト
 * @param     idx2 src の要素の開始番号
 * @return    自身の参照
 */
tarray<datatype> &tarray<datatype>::insert( size_t index,
				      const tarray<datatype> &src, size_t idx2)
{
    return this->replace(index,0, src,idx2,src.length());
}

template <class datatype>
/**
 * @brief  配列の(一部)要素を，自身の配列の指定位置に挿入
 *
 *  srcで指定された配列を，自身の配列の指定位置indexに挿入します．<br>
 *  index に配列の長さ以上の値を指定する場合，index に自身の配列の長さを
 *  与えたものとみなします．
 *
 * @param     index 自身の配列の挿入位置
 * @param     src 源泉となる配列要素を持つオブジェクト
 * @param     idx2 src の要素の開始番号
 * @param     n2 src から取り出す要素の個数
 * @return    自身の参照
 */
tarray<datatype> &tarray<datatype>::insert( size_t index, 
					    const tarray<datatype> &src, 
					    size_t idx2, size_t n2 )
{
    return this->replace(index,0, src,idx2,n2);
}

template <class datatype>
/**
 * @brief  配列の一部または全部を，指定された値で置換
 *
 *  自身の配列の要素の位置idx1からn1個の要素を，指定された値を持つ要素
 *  n2個で置換します．<br>
 *  idx1 に配列要素数以上の値が指定された場合，append() メンバ関数と同様の
 *  処理を行います．<br>
 *  idx1 とn1 の和が配列の要素数よりも大きい場合や，またn1，n2 の大小関係に
 *  より配列の拡張，収縮が必要な場合は要素数を自動的に調整します．
 *
 * @param     idx1 自身の配列の開始位置
 * @param     n1 置換される要素数
 * @param     one 源泉となる値
 * @param     n 値one を書き込む個数
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合
 */
tarray<datatype> &tarray<datatype>::replace( size_t idx1, size_t n1,
					     const datatype &one, size_t n2 )
{
    size_t new_length, crt_length = this->length();
    size_t i;

    /* メモ */
    /* 引数 src に自身を指定されても良いように，領域が小さくなる時は */
    /* 先に代入するようにしている */

    if ( crt_length < idx1 ) idx1 = crt_length;
    if ( crt_length < idx1 + n1 ) n1 = crt_length - idx1;

    if ( n1 < n2 ) {
	new_length = crt_length + (n2 - n1);
    }
    else {	/* (n2 <= n1)	to be small or no-sizechange */
	/* 先に代入してしまう  */
	try {
	    datatype *objp_dst;
	    for ( i=0 ; i < n2 ; i++ ) {
		objp_dst = (datatype *)(this->arrs_rec.p(idx1 + i));
		*(objp_dst) = one;
	    }
	}
	catch (...) {
	    this->err_throw(__FUNCTION__,"FATAL","*(objp_dst) = one; failed");
	}
	new_length = crt_length - (n1 - n2);
	for ( i=0 ; i < (n1 - n2) ; i++ ) {
	    datatype *objp = (datatype *)(this->arrs_rec.p((idx1 + n2) + i));
	    delete objp;
	}
	if ( n2 < n1 ) {
	    this->arrs_rec.move(idx1 + n1, crt_length - idx1 - n1, idx1 + n2,
				false);
	}
    }

    if ( new_length != crt_length ) {
	this->arrs_rec.resize(new_length);
    }
    if ( n1 < n2 ) {		/* to be large */
	ssize_t src_p = idx1 + n1;
	ssize_t dest_p = idx1 + n2;
	size_t sz = crt_length - idx1 - n1;
	/* アドレステーブルの部分的移動 */
	this->arrs_rec.move(src_p, sz, dest_p, true);
	/* 新規オブジェクト生成 */
	try {
	    datatype *objp;
	    for ( i=0 ; i < (n2 - n1) ; i++ ) {
		if ( this->creator != NULL ) 
		    objp = (*(this->creator))(this->creator_arg);
		else objp = new datatype;
		this->arrs_rec.p((idx1 + n1) + i) = (uintptr_t)objp;
	    }
	}
	catch (...) {
	    for ( i=0 ; i < (n2 - n1) ; i++ ) {
	      datatype *objp = (datatype *)(this->arrs_rec.p((idx1 + n1) + i));
	      if ( objp != NULL ) {
		  delete objp;
	      }
	    }
	    this->arrs_rec.move(dest_p, sz, src_p, true);
	    this->err_throw(__FUNCTION__,"FATAL","new failed");
	}
	/* 代入 */
	try {
	    datatype *objp_dst;
	    for ( i=0 ; i < n2 ; i++ ) {
		objp_dst = (datatype *)(this->arrs_rec.p(idx1 + i));
		*(objp_dst) = one;
	    }
	}
	catch (...) {
	    this->err_throw(__FUNCTION__,"FATAL","*(objp_dst) = one; failed");
	}
    }

    return *this;
}

template <class datatype>
/**
 * @brief  配列の一部または全部を，指定された配列で置換
 *
 *  自身の配列の要素の位置idx1からn1個の要素を，srcで指定された配列で
 *  置換します．<br>
 *  idx1 に配列要素数以上の値が指定された場合，append() メンバ関数と同様の
 *  処理を行います．<br>
 *  idx1 とn1 の和が配列の要素数よりも大きい場合や，またn1，n2 の大小関係に
 *  より配列の拡張，収縮が必要な場合は要素数を自動的に調整します．
 *
 * @param     idx1 自身の配列の開始位置
 * @param     n1 置換される要素数
 * @param     src 源泉となる配列要素を持つオブジェクト
 * @param     idx2 src の要素の開始番号
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合
 */
tarray<datatype> &tarray<datatype>::replace( size_t idx1, size_t n1,
					     const tarray<datatype> &src,
					     size_t idx2 )
{
    return this->replace(idx1, n1, src, idx2, src.length());
}

template <class datatype>
/**
 * @brief  配列の一部または全部を，指定された配列で置換
 *
 *  自身の配列の要素の位置idx1からn1個の要素を，srcで指定された配列で
 *  置換します．<br>
 *  idx1 に配列要素数以上の値が指定された場合，append() メンバ関数と同様の
 *  処理を行います．<br>
 *  idx1 とn1 の和が配列の要素数よりも大きい場合や，またn1，n2 の大小関係に
 *  より配列の拡張，収縮が必要な場合は要素数を自動的に調整します．
 *
 * @param     idx1 自身の配列の開始位置
 * @param     n1 置換される要素数
 * @param     src 源泉となる配列要素を持つオブジェクト
 * @param     n2 src から取り出す要素の個数
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合
 */
tarray<datatype> &tarray<datatype>::replace( size_t idx1, size_t n1,
					     const tarray<datatype> &src, 
					     size_t idx2, size_t n2 )
{
    size_t new_length, crt_length = this->length();
    size_t i;

    /* メモ */
    /* 引数 src に自身を指定されても良いように，いろいろと工夫している */
    /* - 領域が小さくなる場合は，先に代入 */
    /* - 領域が大きくなる場合は，アドレステーブルリサイズ後，*/
    /*   アドレステーブル上のアドレスを保存してから，NULLなアドレステーブル */
    /*   を new で埋め，最後に保存したアドレス上のオブジェクトをコピー */

    if ( src.length() < idx2 ) idx2 = src.length();
    if ( src.length() < idx2 + n2 ) n2 = src.length() - idx2;

    if ( crt_length < idx1 ) idx1 = crt_length;
    if ( crt_length < idx1 + n1 ) n1 = crt_length - idx1;

    if ( n1 < n2 ) {
	new_length = crt_length + (n2 - n1);
    }
    else {	/* (n2 <= n1)	to be small or no-sizechange */
	/* 先に代入してしまう  */
	try {
	    const datatype *objp_src;
	    datatype *objp_dst;
	    size_t j;
	    for ( i=0 ; i < n2 ; i++ ) {
		if ( idx1 <= idx2 ) j = i;
		else j = n2 - i - 1;
		objp_dst = (datatype *)(this->arrs_rec.p(idx1 + j));
		objp_src = (const datatype *)(src.arrs_rec.p_cs(idx2 + j));
		*(objp_dst) = *(objp_src);
	    }
	}
	catch (...) {
	    this->err_throw(__FUNCTION__,"FATAL",
			    "*(objp_dst) = *(objp_src) failed");
	}
	new_length = crt_length - (n1 - n2);
	for ( i=0 ; i < (n1 - n2) ; i++ ) {
	    datatype *objp = (datatype *)(this->arrs_rec.p((idx1 + n2) + i));
	    delete objp;
	}
	if ( n2 < n1 ) {
	    this->arrs_rec.move(idx1 + n1, crt_length - idx1 - n1, idx1 + n2,
				false);
	}
    }

    if ( new_length != crt_length ) {
	this->arrs_rec.resize(new_length);
    }

    if ( n1 < n2 ) {		/* to be large */
	ssize_t src_p = idx1 + n1;
	ssize_t dest_p = idx1 + n2;
	size_t sz = crt_length - idx1 - n1;
	const size_t nx[] = {n2};
	mdarray adr_save(sizeof(datatype *), true, nx, 1, false);
	/* src に自身が指定された時，次の .move() でアドレスがズレるので保存 */
	for ( i=0 ; i < n2 ; i++ ) {
	    adr_save.p(i) = src.arrs_rec.p_cs(idx2 + i);
	}
	/* アドレステーブルの部分的移動 */
	this->arrs_rec.move(src_p, sz, dest_p, true);
	/* 新規オブジェクト生成 */
	try {
	    datatype *objp;
	    for ( i=0 ; i < (n2 - n1) ; i++ ) {
		if ( this->creator != NULL ) 
		    objp = (*(this->creator))(this->creator_arg);
		else objp = new datatype;
		this->arrs_rec.p((idx1 + n1) + i) = (uintptr_t)objp;
	    }
	}
	catch (...) {
	    for ( i=0 ; i < (n2 - n1) ; i++ ) {
	      datatype *objp = (datatype *)(this->arrs_rec.p((idx1 + n1) + i));
	      if ( objp != NULL ) {
		  delete objp;
	      }
	    }
	    this->arrs_rec.move(dest_p, sz, src_p, true);
	    this->err_throw(__FUNCTION__,"FATAL","new failed");
	}
	/* adr_save に保存したインスタンスのアドレスを使って，代入 */
	try {
	    const datatype *objp_src;
	    datatype *objp_dst;
	    size_t j;
	    for ( i=0 ; i < n2 ; i++ ) {
		if ( idx1 <= idx2 ) j = i;
		else j = n2 - i - 1;
		objp_dst = (datatype *)(this->arrs_rec.p(idx1 + j));
		objp_src = (const datatype *)(adr_save.p(j));
		*(objp_dst) = *(objp_src);
	    }
	}
	catch (...) {
	    this->err_throw(__FUNCTION__,"FATAL",
			    "*(objp_dst) = *(objp_src) failed");
	}
    }

    return *this;
}

/**
 * @brief  コンパイル時にエラーを出力させるためのダミーのメンバ関数
 * @deprecated  使用不可．
 * @note	このメンバ関数はprivateです
 */
template <class datatype>
tarray<datatype> &tarray<datatype>::replace( size_t idx1, size_t n1, 
					     size_t n2 )
{
    return this->replace(idx1,n1,this->initial_one(),n2);
}

/**
 * @brief  コンパイル時にエラーを出力させるためのダミーのメンバ関数
 * @deprecated  使用不可．
 * @note	このメンバ関数はprivateです
 */
template <class datatype>
tarray<datatype> &tarray<datatype>::put( size_t index, size_t n )
{
    if ( this->length() < index + n ) {
	this->replace(this->length(), 0, 
		      this->initial_one(), (index + n) - this->length());
    }
    return this->replace(index, n, n);
}

template <class datatype>
/**
 * @brief  指定された値で，自身の配列の指定位置からn個上書き
 *
 *  自身の配列の要素番号indexの位置から，指定された値を持つ要素n個で
 *  上書きします．<br>
 *  index は任意の値を取る事ができます．引数の指定に対して配列内の要素数が
 *  不足している場合は，自動的にサイズを拡張します．<br>
 *  要素が空の状態において，例えば，my_arr.put(0,value,6) と 
 *  my_arr.put(2,value,4) の結果は同じです．<br>
 *  さらに，要素数 4 個の配列に対して my_arr.put(2,value,4) とすると，要素数は
 *  6 個となり，要素番号 2 以降の要素は value で指定されたものとなります．
 *
 * @param     index 自身の配列の書き込み位置
 * @param     one 源泉となる値
 * @param     n 値one を書き込む個数
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合
 */
tarray<datatype> &tarray<datatype>::put( size_t index, 
					 const datatype &one, size_t n )
{
    if ( this->length() < index + n ) {
	this->replace(this->length(), 0, 
		      this->initial_one(), (index + n) - this->length());
    }
    return this->replace(index,n, one,n);
}

/**
 * @brief  指定された配列を，自身の配列の指定位置に上書き
 *
 *  srcで指定された配列を，自身の配列の要素位置indexから上書きします．
 *  index は任意の値を取る事ができます．引数の指定に対して配列内の要素数が
 *  不足している場合は，自動的にサイズを拡張します．<br>
 *  要素が空の状態において，例えば，my_arr.put(0,value,6) と 
 *  my_arr.put(2,value,4) の結果は同じです．<br>
 *  さらに，要素数 4 個の配列に対して my_arr.put(2,value,4) とすると，要素数は
 *  6 個となり，要素番号 2 以降の要素は value で指定されたものとなります．
 *
 * @param     index 自身の配列の書き込み位置
 * @param     src 源泉となる配列要素を持つオブジェクト
 * @param     idx2 src の要素の開始番号
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合
 */
template <class datatype>
tarray<datatype> &tarray<datatype>::put( size_t index,
					 const tarray<datatype> &src,
					 size_t idx2)
{
    return this->put(index, src, idx2, src.length());
}

template <class datatype>
/**
 * @brief  指定された配列を，自身の配列の指定位置に上書き
 *
 *  srcで指定された配列を，自身の配列の要素位置indexから上書きします．
 *  index は任意の値を取る事ができます．引数の指定に対して配列内の要素数が
 *  不足している場合は，自動的にサイズを拡張します．<br>
 *  要素が空の状態において，例えば，my_arr.put(0,value,6) と 
 *  my_arr.put(2,value,4) の結果は同じです．<br>
 *  さらに，要素数 4 個の配列に対して my_arr.put(2,value,4) とすると，要素数は
 *  6 個となり，要素番号 2 以降の要素は value で指定されたものとなります．
 *
 * @param     index 自身の配列の書き込み位置
 * @param     src 源泉となる配列要素を持つオブジェクト
 * @param     idx2 src の要素の開始番号
 * @param     n2 src から取り出す要素の個数
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合
 */
tarray<datatype> &tarray<datatype>::put( size_t index, 
					 const tarray<datatype> &src, 
					 size_t idx2, size_t n2 )
{
    size_t len = src.length();
    if ( idx2 <= len ) {
	size_t max = len - idx2;
	if ( max < n2 ) n2 = max;
	if ( this->length() < index + n2 ) {
	    this->replace(this->length(), 0, 
			  this->initial_one(), (index + n2) - this->length());
	}
	return this->replace(index,n2, src,idx2,n2);
    }
    return *this;
}

template <class datatype>
/**
 * @brief  配列の不要部分の消去
 *
 *  自身の配列を要素番号idx からlen 個の要素だけにします．
 *
 * @param     idx 切り出し要素の開始位置
 * @param     len 要素の個数
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合
 */
tarray<datatype> &tarray<datatype>::crop( size_t idx, size_t len )
{
    size_t max_n;
    if ( this->length() < idx ) idx = this->length();
    max_n = this->length() - idx;
    if ( max_n < len ) len = max_n;
    this->erase(0, idx);
    this->erase(len, this->length() - len);
    return *this;
}

template <class datatype>
/**
 * @brief  配列の不要部分の消去
 *
 *  自身の配列を，要素番号idx 以降の配列だけにします．
 *
 * @param     idx 切り出し要素の開始位置
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合
 */
tarray<datatype> &tarray<datatype>::crop( size_t idx )
{
    if ( this->length() < idx ) idx = this->length();
    return this->crop(idx, this->length() - idx);
}

template <class datatype>
/**
 * @brief  全配列要素の削除
 *
 *  自身が持つ配列の全要素を削除します(配列長はゼロになります)．
 *
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合
 */
tarray<datatype> &tarray<datatype>::erase()
{
    return this->erase(0,this->length());
}

template <class datatype>
/**
 * @brief  配列要素の削除
 *
 *  要素番号 index の要素から num_el 個の要素を削除します．<br>
 *  num_el が指定されない場合は，1 つの要素を削除します．<br>
 *  index に配列の長さ以上の値が指定された場合，無視されます．
 *
 * @param     index 要素番号
 * @param     num_elements 要素の個数
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合
 */
tarray<datatype> &tarray<datatype>::erase( size_t index, size_t num_el )
{
    return this->replace(index,num_el, this->initial_one(),(size_t)0);
}

template <class datatype>
/**
 * @brief  配列長の変更
 *
 *  自身の配列の長さを new_num_elements に変更します．配列長を収縮する場合，
 *  new_num_elements 以降の要素は削除されます．
 *
 * @param     new_num_elements 変更後の配列長
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合
 */
tarray<datatype> &tarray<datatype>::resize( size_t new_num_elements )
{
    if ( new_num_elements < this->length() ) {
        this->replace( new_num_elements, this->length() - new_num_elements,
		       this->initial_one(), (size_t)0 );
    }
    else {
	this->replace( this->length(), 0,
		       this->initial_one(), new_num_elements - this->length() );
    }
    return *this;
}

template <class datatype>
/**
 * @brief  配列長の相対的な変更
 *
 *  自身の配列の長さを len の長さ分だけ変更します．<br>
 *  配列長を収縮する場合，最後の abs(len) 個の要素は削除されます．
 *
 * @param     len 配列長の増分・減分
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合
 */
tarray<datatype> &tarray<datatype>::resizeby( ssize_t len )
{
    size_t new_len, a_len;
    a_len = ((len < 0) ? -len : len);
    if ( len < 0 ) {
	if ( a_len < this->length() ) 
	    new_len = this->length() - a_len;
	else
	    new_len = 0;
    }
    else {
	new_len = this->length() + a_len;
    }

    return this->resize(new_len);
}

template <class datatype> 
/**
 * @brief  一部要素を別オブジェクトへコピー
 *
 *  自身の配列のすべてまたは一部を，dest で指定されたオブジェクトにコピー
 *  します．<br>
 *  index + n の値がコピー元の要素数を超える場合，要素番号index から最後の要素
 *  までコピーされます．<br>
 *  index の値がコピー元の要素数を超える場合，dest の内容は消去され，返り値が
 *  -1となります．
 *
 * @param     index コピー元オブジェクトの配列の開始要素番号
 * @param     n コピーする要素数
 * @param     dest コピー先のtarray クラスのオブジェクト
 * @return    非負の値: コピーした要素数<br>
 *            負の値(エラー) : index に配列長以上の値が指定された場合
 * @throw     内部バッファの確保に失敗した場合
 */
ssize_t tarray<datatype>::copy( size_t index, size_t n,
				tarray<datatype> *dest ) const
{
    if ( dest == NULL ) return -1;

    size_t len = this->length();

    if ( len < index ) {
	dest->erase();
	return -1;
    }
    if ( len == 0 ) {
	dest->erase();
	return 0;
    }

    if ( len - index < n ) n = len - index;

    dest->replace(0, dest->length(), *this, index, n);

    return n;
}

template <class datatype>
/**
 * @brief  一部要素を別オブジェクトへコピー
 *
 *  自身の配列のすべてまたは一部を，dest で指定されたオブジェクトにコピー
 *  します．<br>
 *  index + n の値がコピー元の要素数を超える場合，要素番号index から最後の要素
 *  までコピーされます．<br>
 *  index の値がコピー元の要素数を超える場合，dest の内容は消去され，返り値が
 *  -1となります．
 *
 * @param     index コピー元オブジェクトの配列の開始要素番号
 * @param     dest コピー先のtarray クラスのオブジェクト
 * @return    非負の値: コピーした要素数<br>
 *            負の値(エラー) : index に配列長以上の値が指定された場合
 * @throw     内部バッファの確保に失敗した場合
 */
ssize_t tarray<datatype>::copy( size_t index, tarray<datatype> *dest ) const
{
    if ( dest == NULL ) return -1;

    return this->copy(index, this->length(), dest);
}

template <class datatype> 
/**
 * @brief  自身の内容を別オブジェクトへコピー
 *
 *  自身の配列のすべてを，dest で指定されたオブジェクトにコピーします．
 *
 * @param     dest コピー先のtarray クラスのオブジェクト
 * @return    非負の値: コピーした要素数<br>
 *            負の値(エラー) : index に配列長以上の値が指定された場合
 * @throw     内部バッファの確保に失敗した場合
 */
ssize_t tarray<datatype>::copy( tarray<datatype> *dest ) const
{
    if ( dest == NULL ) return -1;

    return this->copy(0, this->length(), dest);
}

template <class datatype>
/**
 * @brief  一部要素を別オブジェクトへコピー (非推奨)
 * @deprecated 非推奨．
 */
ssize_t tarray<datatype>::copy( size_t index, size_t n, 
				tarray<datatype> &dest ) const
{
    return this->copy(index, n, &dest);
}

template <class datatype>
/**
 * @brief  一部要素を別オブジェクトへコピー (非推奨)
 * @deprecated 非推奨．
 */
ssize_t tarray<datatype>::copy( size_t index, tarray<datatype> &dest ) const
{
    return this->copy(index, &dest);
}

template <class datatype>
/**
 * @brief  自身の内容を別オブジェクトへコピー (非推奨)
 * @deprecated 非推奨．
 */
ssize_t tarray<datatype>::copy( tarray<datatype> &dest ) const
{
    return this->copy(&dest);
}

template <class datatype>
/**
 * @brief  2つのオブジェクト間での内容のスワップ
 *
 *  オブジェクトsobj の内容と自身の内容とを入れ替えます．
 *
 * @param     sobj 内容を入れ替えるtarray クラスのオブジェクト
 * @return    自身の参照
 */
tarray<datatype> &tarray<datatype>::swap( tarray<datatype> &sobj )
{
    datatype *tmp__initial_one_rec;
    datatype *(*tmp__creator)(void *);
    void *tmp__creator_arg;

    if ( &sobj == this ) return *this;

    this->arrs_rec.swap(sobj.arrs_rec);

    tmp__initial_one_rec = this->initial_one_rec;
    this->initial_one_rec = sobj.initial_one_rec;
    sobj.initial_one_rec = tmp__initial_one_rec;

    tmp__creator = this->creator;
    this->creator = sobj.creator;
    sobj.creator = tmp__creator;

    tmp__creator_arg = this->creator_arg;
    this->creator_arg = sobj.creator_arg;
    sobj.creator_arg = tmp__creator_arg;

    return *this;
}

template <class datatype>
/**
 * @brief  指定された配列要素の参照を返す
 *
 *  index で指定された配列要素の参照を返します．<br>
 *  これらメンバ関数の直後に "." で接続し，typeクラスのメンバ関数を使う事が
 *  できます<br>
 *  配列長以上のindex が指定された場合は，新しい配列要素が作られます．
 *  バッファの確保に失敗した場合を除き，例外は発生しません．
 *
 * @param     index 要素番号
 * @return    指定された要素番号に該当する値またはオブジェクトの参照
 * @throw     内部バッファの確保に失敗した場合
 */
inline datatype &tarray<datatype>::at( size_t index )
{
    datatype *objp = NULL;
#if 1
    if ( this->length() <= index ) {
	this->resize(index + 1);
    }
#endif
    if ( index < 0 || this->length() <= index ) {
        this->err_throw(__FUNCTION__,"ERROR","Invalid index");
    }
    else {
	objp = (datatype *)(this->arrs_rec.p(index));
    }
    return *(objp);
}

#ifdef SLI__OVERLOAD_CONST_AT
template <class datatype>
/**
 * @brief  指定された配列要素の参照を返す (読取専用)
 *
 *  index で指定された配列要素の参照を返します．<br>
 *  これらメンバ関数の直後に "." で接続し，typeクラスのメンバ関数を使う事が
 *  できます．<br>
 *  読み取り専用で，配列長以上の index が指定された場合は，例外が発生します．
 *
 * @param     index 要素番号
 * @return    指定された要素番号に該当する値またはオブジェクトの参照
 * @throw     配列長以上のindex が指定された場合
 */
inline const datatype &tarray<datatype>::at( size_t index ) const
{
    return this->at_cs(index);
}
#endif

template <class datatype>
/**
 * @brief  指定された配列要素の参照を返す (読取専用)
 *
 *  index で指定された配列要素の参照を返します．<br>
 *  これらメンバ関数の直後に "." で接続し，typeクラスのメンバ関数を使う事が
 *  できます．<br>
 *  読み取り専用で，配列長以上の index が指定された場合は，例外が発生します．
 *
 * @param     index 要素番号
 * @return    指定された要素番号に該当する値またはオブジェクトの参照
 * @throw     配列長以上のindex が指定された場合
 */
inline const datatype &tarray<datatype>::at_cs( size_t index ) const
{
    const datatype *objp = NULL;
    if ( index < 0 || this->length() <= index ) {
        this->err_throw(__FUNCTION__,"ERROR","Invalid index");
    }
    else {
	objp = (const datatype *)(this->arrs_rec.p_cs(index));
    }
    return *(objp);
}

template <class datatype>
/**
 * @brief  配列の長さ(個数)を取得
 *
 * @return    配列数
 */
inline size_t tarray<datatype>::length() const
{
    return this->arrs_rec.length();
}

/**
 * @brief  配列の長さ(個数)を取得
 *
 * @return    配列数
 */
template <class datatype>
inline size_t tarray<datatype>::size() const
{
    return this->arrs_rec.length();
}

/* new datatype する時に，コンストラクタに引数を与えたい場合に使う */
template <class datatype>
/**
 * @brief  datatypeクラスのオブジェクトを作る関数の登録
 *
 *  tarray クラス内部で「new datatype」する時に，コンストラクタに引数を与えたい
 *  場合に使います.
 *
 * @param     func ユーザ関数のアドレス
 * @param     user_ptr ユーザ関数の最後に与えられるユーザのポインタ
 * @throw     内部バッファの確保に失敗した場合
 */
void tarray<datatype>::register_creator( datatype *(*func)(void *), 
					 void *user_ptr )
{
    static datatype one;
    if ( this->initial_one_rec != NULL ) {
	delete this->initial_one_rec;
	this->initial_one_rec = NULL;
    }
    try {
	if ( func == NULL ) {
	    this->initial_one_rec = new datatype;
	    *(this->initial_one_rec) = one;
	}
	else {
	    this->initial_one_rec = (*func)(user_ptr);	/* new */
	}
    }
    catch (...) {
	this->err_throw(__FUNCTION__,"FATAL","new failed");
    }
    this->creator = func;
    this->creator_arg = user_ptr;
    return;
}

template <class datatype>
/**
 * @brief  initial_one_recの参照(読取専用)を返す
 *
 * @return    initial_one_recの参照
 */
const datatype &tarray<datatype>::initial_one() const
{
    return *(this->initial_one_rec);
}

/**
 * @brief  テンポラリオブジェクトのためのshallow copy属性の付与 (未実装)
 * @deprecated  未実装
 */
/* 注意: 参照を返したいところだが，return 文でそれをやるとインスタンス */
/*       を2回コピーされるというおかしな事になるので void になっている */
template <class datatype>
void tarray<datatype>::set_scopy_flag()
{
    this->shallow_copy_ok = true;
    return;
}


/* private member functions */

/**
 * @brief  shallow copy が可能かを取得 (未実装)
 * 
 *  src側で実行する．<br>
 *  SLLIBの実装では，shallow copy は特定の相手とのみ可．
 * 
 * @deprecated  未実装
 * @param   from_obj リクエストを送信する側のオブジェクトのアドレス
 * @return  shallow copyが可能なら真<br>
 *          それ以外の時は偽
 * @note このメンバ関数は private です
 */
template <class datatype>
bool tarray<datatype>::request_shallow_copy( datatype *from_obj ) const
{
    return false;
}

/**
 * @brief  エラー，例外発生時に err_rec を throw する
 *
 *  tarrayクラスの関数内でエラー，例外発生時に呼び出されます．
 *  標準エラー出力にエラー情報を表示し，err_rec を throwします．
 * 
 * @param	func0 呼び出し元関数名
 * @param	level0 エラー種別<br>
 *			例外発生時:"FATAL"<BR>
 *			エラー時:"ERROR"
 * @param	message0 エラーメッセージ
 * @throw	err_rec:エラー情報を持つ構造体
 * @note	このメンバ関数はprivateです
 */

template <class datatype>
void tarray<datatype>::err_throw( const char *func0, const char *level0, 
				  const char *message0 ) const
{
    static stdstreamio sio;
    const char *classname = "tarray";
    sio.eprintf("%s::%s(): [%s] ",classname,func0,level0);
    sio.eprintf("%s\n", message0);
    {
	size_t i, len;
	const char *src;
	char *dst;
	sli::err_rec st;
	/* */
	src = classname;  dst = st.class_name;  len = 64;
	for ( i=0 ; i < len && src[i] != '\0' ; i++ ) dst[i] = src[i];
	dst[i] = '\0';
	/* */
	src = func0;     dst = st.func_name;    len = 64;
	for ( i=0 ; i < len && src[i] != '\0' ; i++ ) dst[i] = src[i];
	dst[i] = '\0';
	/* */
	src = level0;    dst = st.level;        len = 64;
	for ( i=0 ; i < len && src[i] != '\0' ; i++ ) dst[i] = src[i];
	dst[i] = '\0';
	/* */
	src = message0;  dst = st.message;      len = 128;
	for ( i=0 ; i < len && src[i] != '\0' ; i++ ) dst[i] = src[i];
	dst[i] = '\0';
	/* */
	throw st;
    }
}

}	/* namespace sli */

#endif	/* _SLI__TARRAY_H */
