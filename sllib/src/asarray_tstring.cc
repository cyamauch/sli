/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2013-05-14 17:15:47 cyamauch> */

/**
 * @file   asarray_tstring.cc
 * @brief  文字列の連想配列を扱う asarray_tstring クラスのコード
 */

#define CLASS_NAME "asarray_tstring"

#include "config.h"

#include "asarray_tstring.h"

#include <stdlib.h>

#include "private/err_report.h"


namespace sli
{

/* constructor */

/**
 * @brief  コンストラクタ
 *
 */
asarray_tstring::asarray_tstring()
{
    return;
}

/**
 * @brief  コンストラクタ
 *
 *  自身を elements[] を初期値として初期化します．<br>
 *  構造体の配列の最終要素には必ず {NULL,NULL} を与えてください．
 * 
 * @param  elements asarrdef_tstring構造体の配列
 * @throw  内部バッファの確保に失敗した場合
 */
asarray_tstring::asarray_tstring(const asarrdef_tstring elements[])
{
    this->assign(elements);
    return;
}

/**
 * @brief  コンストラクタ
 *
 *  与えられたキー文字列と値文字列で自身を初期化します．<br>
 *  引数の最後には必ず NULL を与えてください．
 *  
 * @param  key0 キー文字列
 * @param  val0 値文字列
 * @param  key1 キー文字列
 * @param  ... キー，値となる文字列の可変長引数の各要素データ
 * @throw  内部バッファの確保に失敗した場合
 */
asarray_tstring::asarray_tstring(const char *key0, const char *val0,
				 const char *key1, ...)
{
    va_list ap;
    va_start(ap,key1);
    try {
	this->vassign(key0,val0,key1,ap);
    }
    catch (...) {
	va_end(ap);
        err_throw(__FUNCTION__,"FATAL","this->vassign() failed");
    }
    va_end(ap);
    return;
}

/* copy constructor */

/**
 * @brief  コピーコンストラクタ
 *
 *  自身をobjの内容で初期化します．
 * 
 * @param  obj asarray_tstringオブジェクト
 * @throw  内部バッファの確保に失敗した場合
 * @throw  メモリ破壊を起こしたとき
 */
asarray_tstring::asarray_tstring(const asarray_tstring &obj)
{
    this->init(obj);
    return;
}

/* destructor */

/**
 * @brief  デストラクタ
 */
asarray_tstring::~asarray_tstring()
{
    return;
}

/**
 * @brief  オブジェクトのコピー
 *
 *  演算子の右側で指定された asarray_tstring クラスのオブジェクトを自身にコピー
 *  します．
 * 
 * @param   obj asarray_tstringのオブジェクト
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合 
 * @throw   メモリ破壊を起こしたとき
 */
asarray_tstring &asarray_tstring::operator=(const asarray_tstring &obj)
{
    this->init(obj);
    return *this;
}

/**
 * @brief  [] で指定された要素値の参照(tstring)を返す
 *
 *  キーに対応する連想配列の要素値の参照を返します．
 * 
 * @param   key 連想配列のキー文字列
 * @return  キーに対応する連想配列の要素値の参照
 * @throw   指定されたキー文字列がNULLの場合
 * @throw   内部バッファの確保に失敗した場合
 */
tstring &asarray_tstring::operator[]( const char *key )
{
    return this->at(key);
}

/**
 * @brief  [] で指定された要素値の参照(tstring)を返す (読取専用)
 *
 *  キーに対応する連想配列の要素値の参照を返します．
 * 
 * @param   key 連想配列のキー文字列 
 * @return  キーに対応する連想配列の要素値
 * @throw   指定されたキー文字列がNULLの場合
 * @throw   存在しないキー文字列が指定された場合
 */
const tstring &asarray_tstring::operator[]( const char *key ) const
{
    return this->at_cs(key);
}

/*
 * public member functions
 */

/**
 * @brief  オブジェクトの初期化
 *
 *  自身の連想配列を消去し，オブジェクトの初期化を行います．
 * 
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
asarray_tstring &asarray_tstring::init()
{
    this->index_rec.init();
    this->key_rec.init();
    this->value_rec.init();
    this->regex_rec.init();
    return *this;
}

/**
 * @brief  オブジェクトのコピー
 * 
 *  指定されたオブジェクト obj の内容を自身にコピーします．
 *
 * @param   obj asarray_tstringのオブジェクト
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 * @throw   メモリ破壊を起こした場合
 */
asarray_tstring &asarray_tstring::init(const asarray_tstring &obj)
{
    if ( &obj == this ) return *this;
    this->value_rec.init(obj.value_rec);
    this->key_rec.init(obj.key_rec);
    this->index_rec.init(obj.index_rec);
    /* this->regex_rec.init(obj.regex_rec); */
    this->regex_rec.init();
    return *this;
}

/**
 * @brief  既存の要素値すべてを指定された文字列(const char *)でパディング
 *
 *  自身の連想配列の要素値すべてを，文字列 str でパディングします．<br>
 *  str を省略した場合，長さ0の文字列が使われます．
 * 
 * @param   str 連想配列の値をパディングする文字列
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
asarray_tstring &asarray_tstring::clean(const char *str)
{
    this->value_rec.clean(str);
    return *this;
}

/**
 * @brief  既存の要素値すべてを指定された文字列(tstring)でパディング
 *
 *  自身の連想配列の要素値すべてを，文字列 str でパディングします．<br>
 * 
 * @param   str 連想配列の値をパディングする文字列
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
asarray_tstring &asarray_tstring::clean(const tstring &str)
{
    this->value_rec.clean(str);
    return *this;
}

/**
 * @brief  指定された連想配列(asarray_tstring)を自身に代入
 *
 *  srcで指定された文字列連想配列の全部を，自身に代入します．
 * 
 * @param   src asarray_tstringオブジェクト
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 * 
 */
asarray_tstring &asarray_tstring::assign(const asarray_tstring &src)
{
    if ( &src == this ) return *this;
    this->erase();
    return this->append(src);
}

/**
 * @brief  指定された連想配列(asarrdef_tstring [])を自身に代入
 *
 *  elements で指定された文字列連想配列の全部を，自身に代入します．<br>
 *  elements は {NULL,NULL} で終端している必要があります．
 * 
 * @param   elements[] asarrdef_tstring型の配列
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
asarray_tstring &asarray_tstring::assign( const asarrdef_tstring elements[] )
{
    if ( elements != NULL ) {
	size_t i;
	for ( i=0 ; elements[i].key != NULL ; i++ );
	this->assign(elements, i);
    }
    else {
	this->erase();
    }
    return *this;
}

/**
 * @brief  指定された連想配列(asarrdef_tstring [])のn個を自身に代入
 *
 *  elements で指定された文字列連想配列の先頭から n 個分の内容を，自身に代入
 *  します．
 * 
 * @param   elements asarray_tstringオブジェクト
 * @param   n 配列elementsの数
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
asarray_tstring &asarray_tstring::assign( const asarrdef_tstring elements[],
					  size_t n )
{
    /* 自身が管理している key, val が来てもいいようにする */
    if ( 0 < n && elements != NULL ) { 
	size_t i, len_elem, org_len = this->length();
	for ( i=0 ; i < n && elements[i].key != NULL ; i++ );
	len_elem = i;
	/* まず，最後に追加する */
	for ( i=0 ; i < len_elem ; i++ ) {
	    const char *val = elements[i].value;
	    if ( val == NULL ) val = "";
	    this->value_rec.append(val, 1);
	    this->key_rec.append(elements[i].key, 1);
	} 
	/* 元のやつを消去する */
	this->value_rec.erase(0, org_len);
	this->key_rec.erase(0, org_len);
	/* インデックスをはりはおす */
	this->index_rec.init();
	for ( i=0 ; i < this->key_rec.length() ; ) {
	    if ( this->index_rec.index(this->key_rec.cstr(i),0) < 0 ) {
		this->index_rec.append(this->key_rec.cstr(i),i);
		i++;
	    }
	    else {
		err_report1(__FUNCTION__,"WARNING","cannot append key '%s'",
			    this->key_rec.cstr(i));
		this->value_rec.erase(i, 1);
		this->key_rec.erase(i, 1);
	    }
	} 
    } 
    else {
	this->erase();
    } 
    return *this;
}

/**
 * @brief  指定された要素(key,value) 1個のみの連想配列とする
 *
 *  自身を，指定された 1つの要素だけを持つ連想配列にします．
 *  
 * @param   key 連想配列に設定するキー文字列
 * @param   val 連想配列に設定する値文字列 
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
asarray_tstring &asarray_tstring::assign( const char *key, const char *val )
{
    asarrdef_tstring el[] = { {key,val},{NULL,NULL} };
    return this->assign(el);
}

/**
 * @brief  指定された要素(key,value) 1個のみの連想配列とする
 *
 *  自身を，指定された 1つの要素だけを持つ連想配列にします．
 *  
 * @param   key 連想配列に設定するキー文字列
 * @param   val 連想配列に設定する値文字列 
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
asarray_tstring &asarray_tstring::assign( const char *key, const tstring &val )
{
    asarrdef_tstring el[] = { {key,val.cstr()},{NULL,NULL} };
    return this->assign(el);
}

/**
 * @brief  指定された要素(key,value) 1個のみの連想配列とする
 *
 *  自身を，指定された 1つの要素だけを持つ連想配列にします．<br>
 *  このメンバ関数では，指定したい値文字列をprintf()関数と同様のフォーマットと
 *  可変引数でセットできます.
 * 
 * @param   key 連想配列に設定するキー文字列
 * @param   fmt 設定する値文字列のためのフォーマット指定
 * @param   ... fmtに対応した可変長引数の各要素データ
 * 
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
asarray_tstring &asarray_tstring::assignf( const char *key, 
					   const char *fmt, ... )
{
    va_list ap;
    va_start(ap,fmt);
    try {
	this->vassignf(key,fmt,ap);
    }
    catch (...) {
	va_end(ap);
        err_throw(__FUNCTION__,"FATAL","this->vassignf() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  指定された要素(key,value) 1個のみの連想配列とする
 *
 *  自身を，指定された 1つの要素だけを持つ連想配列にします．<br>
 *  このメンバ関数では，指定したい値文字列をprintf()関数と同様のフォーマットと
 *  可変引数でセットできます.
 *
 * @param   key 連想配列に設定するキー文字列
 * @param   fmt 設定する値文字列のためのフォーマット指定
 * @param   ap fmtに対応した可変長引数のリスト
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
asarray_tstring &asarray_tstring::vassignf( const char *key,
					    const char *fmt, va_list ap )
{
    tstring buf;

    if ( fmt == NULL ) return this->assign(key,fmt);

    buf.vprintf(fmt,ap);
    this->assign(key,buf.cstr());

    return *this;
}

/**
 * @brief  指定された複数の連想配列要素を自身に代入
 *
 *  指定された複数の連想配列要素を自身に代入します．<br>
 *  可変長引数の終端は NULL で終端している必要があります．
 *
 * @param   key0 連想配列に設定するキー文字列
 * @param   key1 連想配列に設定するキー文字列
 * @param   val0 連想配列に設定するキー文字列
 * @param   ... キー,値となる文字列の可変長引数の各要素データ
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
asarray_tstring &asarray_tstring::assign( const char *key0, const char *val0,
					  const char *key1, ... )
{
    va_list ap;
    va_start(ap,key1);
    try {
	this->vassign(key0,val0,key1,ap);
    }
    catch (...) {
	va_end(ap);
        err_throw(__FUNCTION__,"FATAL","this->vassign() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  指定された複数の連想配列要素を自身に代入
 *
 *  指定された複数の連想配列要素を自身に代入します．<br>
 *  可変長引数の終端は NULL で終端している必要があります．
 *
 * @param   key0 連想配列に設定するキー文字列
 * @param   val0 連想配列に設定するキー文字列
 * @param   ap キー,値となる文字列の可変長引数の各要素データ
 * 
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
asarray_tstring &asarray_tstring::vassign( const char *key0, const char *val0,
					   const char *key1, va_list ap )
{
    /* 自身が管理している key, val が来てもいいようにする */
    if ( key0 != NULL ) { 
	size_t i, org_len = this->length();
	/* まず，最後に追加する */
	this->value_rec.append((val0 == NULL) ? "" : val0, 1);
	this->key_rec.append(key0, 1);
	if ( key1 != NULL ) {
	    const char *val1 = va_arg(ap,char *);
	    this->value_rec.append((val1 == NULL) ? "" : val1, 1);
	    this->key_rec.append(key1, 1);
	    while ( 1 ) {
		const char *keyx = va_arg(ap,char *);
		const char *valx;
		if ( keyx == NULL ) break;
		valx = va_arg(ap,char *);
		this->value_rec.append((valx == NULL) ? "" : valx, 1);
		this->key_rec.append(keyx, 1);
	    }
	} 
	/* 元のやつを消去する */
	this->value_rec.erase(0, org_len);
	this->key_rec.erase(0, org_len);
	/* インデックスをはりはおす */
	this->index_rec.init();
	for ( i=0 ; i < this->key_rec.length() ; ) {
	    if ( this->index_rec.index(this->key_rec.cstr(i),0) < 0 ) {
		this->index_rec.append(this->key_rec.cstr(i),i);
		i++;
	    }
	    else {
		err_report1(__FUNCTION__,"WARNING","cannot append key '%s'",
			    this->key_rec.cstr(i));
		this->value_rec.erase(i, 1);
		this->key_rec.erase(i, 1);
	    }
	} 
    } 
    else {
	this->erase();
    } 
    return *this;
}

/**
 * @brief  指定された複数の文字列を，連想配列のキーとして自身に設定
 *
 *  指定された複数の文字列を自身の連想配列のキーに設定します．<br>
 *  可変引数の終端は NULL でなければなりません．
 * 
 * @param   key0 キー文字列
 * @param   ... キー文字列の可変長引数の各要素データ
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
asarray_tstring &asarray_tstring::assign_keys( const char *key0, ... )
{
    va_list ap;
    va_start(ap,key0);
    try {
	this->vassign_keys(key0,ap);
    }
    catch (...) {
	va_end(ap);
        err_throw(__FUNCTION__,"FATAL","this->vassign_keys() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  指定された複数の文字列を，連想配列のキーとして自身に設定
 *
 *  指定された複数の文字列を自身の連想配列のキーに設定します．<br>
 *  可変引数の終端は NULL でなければなりません．
 * 
 * @param   key0 キー文字列
 * @param   ap キー文字列の可変長引数のリスト
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
asarray_tstring &asarray_tstring::vassign_keys( const char *key0, va_list ap )
{
    tarray_tstring tmp_keys;
    if ( key0 != NULL ) {
	tmp_keys.append(key0,1);
	while ( 1 ) {
	    const char *keyx = va_arg(ap,char *);
	    if ( keyx == NULL ) break;
	    tmp_keys.append(keyx,1);
	}
    }
    this->assign_keys(tmp_keys);
    return *this;
}

/**
 * @brief  指定された文字列配列を，連想配列のキーとして自身に設定
 *
 *  指定された複数の文字列配列 keys を自身の連想配列のキーに設定します．<br>
 *  配列の終端は NULL でなければなりません．
 * 
 * @param   keys キー文字列に設定する文字列配列
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
asarray_tstring &asarray_tstring::assign_keys( const char *const *keys )
{
    tarray_tstring tmp_keys;
    if ( this->key_rec.cstrarray() == keys ) return *this;
    tmp_keys.assign(keys);
    this->assign_keys(tmp_keys);
    return *this;
}

/**
 * @brief  指定された文字列配列を，連想配列のキーとして自身に設定
 *
 *  指定された複数の文字列配列 keys を自身の連想配列のキーに設定します．<br>
 *
 * @param   keys キー文字列に設定する文字列配列
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
asarray_tstring &asarray_tstring::assign_keys( const tarray_tstring &keys )
{
    size_t i;

    if ( &(this->key_rec) == &keys ) return *this;

    this->key_rec.erase();
    this->index_rec.init();
    for ( i=0 ; i < keys.length() ; i++ ) {
	/* 同じキー名がきた場合，name, name(1), name(2), ... のようにする */
	int ix=0;
	tstring r_key;
	r_key.assign(keys.cstr(i));
	while ( 0 <= this->index_rec.index(r_key.cstr(),0) ) {
	    ix ++;
	    r_key.printf("%s(%d)",keys.cstr(i),ix);
	}
	this->key_rec.append(r_key.cstr(), 1);
	this->index_rec.append(r_key.cstr(), i);
    }
    this->value_rec.resize(this->key_rec.length());

    return *this;
}

/**
 * @brief  文字列を分割して，キーに設定 (多機能版)
 *
 *  文字列を分割して，キーに設定します．<br>
 *  delims には，" \t" のような単純な文字リストに加え，正規表現で用いられる
 *  "[A-Z]" あるいは "[^A-Z]" のような指定が可能です．<br>
 *  クォーテーションや括弧の「特定の文字」で囲まれた文字列は分割しない場合，
 *  quotations に「特定の文字」を指定します．
 * 
 * @param   src_str 分割対象の文字列
 * @param   delims 区切り文字を含む文字列
 * @param   zero_str 長さ0の結果の文字列を許すかどうかのフラグ
 * @param   quotations クォーテーション・ブラケット文字を含む文字列
 * @param   escape エスケープ文字
 * @param   rm_escape エスケープ文字を削除するかどうかのフラグ
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
asarray_tstring &asarray_tstring::split_keys( const char *src_str, 
					 const char *delims, 
					 bool zero_str, const char *quotations,
					 int escape, bool rm_escape )
{
    tarray_tstring tmp_keys;

    tmp_keys.split(src_str,delims,zero_str,quotations,escape,rm_escape);
    this->assign_keys(tmp_keys);

    return *this;
}

/**
 * @brief  文字列を分割して，キーに設定
 *
 *  文字列を分割して，キーに設定します．<br>
 *  delims には，" \t" のような単純な文字リストに加え，正規表現で用いられる
 *  "[A-Z]" あるいは "[^A-Z]" のような指定が可能です．
 *
 * @param   src_str 分割対象の文字列
 * @param   delims 区切り文字を含む文字列
 * @param   zero_str 長さ0の結果の文字列を許すかどうかのフラグ
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
asarray_tstring &asarray_tstring::split_keys( const char *src_str, 
					      const char *delims,
					      bool zero_str )
{
    return this->split_keys(src_str,delims,zero_str,NULL,'\0',false);
}


/**
 * @brief  文字列を分割して，キーに設定 (多機能版)
 *
 *  文字列を分割して，キーに設定します．<br>
 *  delims には，" \t" のような単純な文字リストに加え，正規表現で用いられる
 *  "[A-Z]" あるいは "[^A-Z]" のような指定が可能です．<br>
 *  クォーテーションや括弧の「特定の文字」で囲まれた文字列は分割しない場合，
 *  quotations に「特定の文字」を指定します．
 * 
 * @param   src_str 分割対象の文字列
 * @param   delims 区切り文字を含む文字列
 * @param   zero_str 長さ0の結果の文字列を許すかどうかのフラグ
 * @param   quotations クォーテーション・ブラケット文字を含む文字列
 * @param   escape エスケープ文字
 * @param   rm_escape エスケープ文字を削除するかどうかのフラグ
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
asarray_tstring &asarray_tstring::split_keys( const tstring &src_str, 
					 const char *delims, 
					 bool zero_str, const char *quotations,
					 int escape, bool rm_escape )
{
    tarray_tstring tmp_keys;

    tmp_keys.split(src_str,delims,zero_str,quotations,escape,rm_escape);
    this->assign_keys(tmp_keys);

    return *this;
}

/**
 * @brief  文字列を分割して，キーに設定
 *
 *  文字列を分割して，キーに設定します．
 *  delims には，" \t" のような単純な文字リストに加え，正規表現で用いられる 
 *  "[A-Z]" あるいは "[^A-Z]" のような指定が可能です．
 * 
 * @param   src_str 分割対象の文字列
 * @param   delims 区切り文字を含む文字列
 * @param   zero_str 長さ0の結果の文字列を許すかどうかのフラグ
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
asarray_tstring &asarray_tstring::split_keys( const tstring &src_str, 
					      const char *delims,
					      bool zero_str )
{
    return this->split_keys(src_str,delims,zero_str,NULL,'\0',false);
}

/**
 * @brief  指定された複数の文字列を，連想配列の値文字列として自身に設定
 *
 *  指定された複数の文字列を，自身の連想配列の値に設定します．<br>
 *  可変引数の終端は NULL でなければなりません．
 *
 * @param   value0 値文字列
 * @param   ... 値文字列の可変長引数の各要素データ 
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
asarray_tstring &asarray_tstring::assign_values( const char *value0, ... )
{
    va_list ap;
    va_start(ap,value0);
    try {
	this->vassign_values(value0,ap);
    }
    catch (...) {
	va_end(ap);
        err_throw(__FUNCTION__,"FATAL","this->vassign_values() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  指定された複数の文字列を，連想配列の値文字列として自身に設定
 *
 *  指定された複数の文字列を,自身の連想配列の値に設定します．<br>
 *  可変引数の終端は NULL でなければなりません．
 *
 * @param   value0 値文字列
 * @param   ap 値文字列の可変長引数のリスト
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
asarray_tstring &asarray_tstring::vassign_values( const char *value0, va_list ap )
{
    tarray_tstring tmp_values;
    if ( value0 != NULL ) {
	tmp_values.append(value0,1);
	while ( 1 ) {
	    const char *valuex = va_arg(ap,char *);
	    if ( valuex == NULL ) break;
	    tmp_values.append(valuex,1);
	}
    }
    this->assign_values(tmp_values);
    return *this;
}

/**
 * @brief  指定された文字列配列を，連想配列の値文字列として自身に設定
 *
 *  指定された複数の文字列を，自身の連想配列の値に設定します．<br>
 *  配列の終端は NULL でなければなりません．
 *
 * @param   values 値文字列に設定する文字列配列
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
asarray_tstring &asarray_tstring::assign_values( const char *const *values )
{
    if ( this->value_rec.cstrarray() == values ) return *this;
    this->value_rec.assign(values);
    this->value_rec.resize(this->key_rec.length());
    return *this;
}

/**
 * @brief  指定された文字列配列を，連想配列の値文字列として自身に設定
 *
 *  指定された複数の文字列を，自身の連想配列の値に設定します．
 *
 * @param   values 値文字列に設定する文字列配列
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
asarray_tstring &asarray_tstring::assign_values( const tarray_tstring &values )
{
    if ( &(this->value_rec) == &values ) return *this;
    this->value_rec.assign(values);
    this->value_rec.resize(this->key_rec.length());
    return *this;
}

/**
 * @brief  文字列を分割して，値に設定 (多機能版)
 *
 *  文字列を分割して，値に設定します．
 *  delims には，" \t" のような単純な文字リストに加え，正規表現で用いられる
 *  "[A-Z]" あるいは "[^A-Z]" のような指定が可能です．<br>
 *  クォーテーションや括弧の「特定の文字」で囲まれた文字列は分割しない場合，
 *  quotations に「特定の文字」を指定します．
 *
 * @param   src_str 分割対照の文字列
 * @param   delims 区切り文字を含む文字列
 * @param   zero_str 長さ0の区切り結果の文字列を許すかどうかのフラグ
 * @param   quotations クォーテーション・ブラケット文字を含む文字列
 * @param   escape エスケープ文字
 * @param   rm_escape エスケープ文字を削除するかどうかのフラグ
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
asarray_tstring &asarray_tstring::split_values( const char *src_str, 
					 const char *delims, 
					 bool zero_str, const char *quotations,
					 int escape, bool rm_escape )
{
    this->value_rec.split(src_str,delims,zero_str,quotations,escape,rm_escape);
    this->value_rec.resize(this->key_rec.length());
    return *this;
}

/**
 * @brief  文字列を分割して，値に設定
 *
 *  文字列を分割して，値に設定します．
 *  delims には，" \t" のような単純な文字リストに加え，正規表現で用いられる
 *  "[A-Z]" あるいは "[^A-Z]" のような指定が可能です．
 * 
 * @param   src_str 分割対照の文字列
 * @param   delims 区切り文字を含む文字列
 * @param   zero_str 長さ0の区切り結果の文字列を許すかどうかのフラグ
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
asarray_tstring &asarray_tstring::split_values( const char *src_str, 
						const char *delims,
						bool zero_str )
{
    return this->split_values(src_str,delims,zero_str,NULL,'\0',false);
}

/**
 * @brief  文字列を分割して，値に設定 (多機能版)
 *
 *  文字列を分割して，値に設定します．
 *  delims には，" \t" のような単純な文字リストに加え，正規表現で用いられる
 *  "[A-Z]" あるいは "[^A-Z]" のような指定が可能です．<br>
 *  クォーテーションや括弧の「特定の文字」で囲まれた文字列は分割しない場合，
 *  quotations に「特定の文字」を指定します．
 * 
 * @param   src_str 分割対照の文字列
 * @param   delims 区切り文字を含む文字列
 * @param   zero_str 長さ0の区切り結果の文字列を許すかどうかのフラグ
 * @param   quotations クォーテーション・ブラケット文字を含む文字列
 * @param   escape エスケープ文字
 * @param   rm_escape エスケープ文字を削除するかどうかのフラグ
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
asarray_tstring &asarray_tstring::split_values( const tstring &src_str, 
					 const char *delims, 
					 bool zero_str, const char *quotations,
					 int escape, bool rm_escape )
{
    this->value_rec.split(src_str,delims,zero_str,quotations,escape,rm_escape);
    this->value_rec.resize(this->key_rec.length());
    return *this;
}

/**
 * @brief  文字列を分割して，値に設定
 *
 *  文字列を分割して,値に設定します.
 *  delims には，" \t" のような単純な文字リストに加え，正規表現で用いられる
 *  "[A-Z]" あるいは "[^A-Z]" のような指定が可能です．
 * 
 * @param   src_str 分割対照の文字列
 * @param   delims 区切り文字を含む文字列
 * @param   zero_str 長さ0の区切り結果の文字列を許すかどうかのフラグ
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
asarray_tstring &asarray_tstring::split_values( const tstring &src_str, 
						const char *delims,
						bool zero_str )
{
    return this->split_values(src_str,delims,zero_str,NULL,'\0',false);
}

/**
 * @brief  指定された連想配列(asarray_tstring)を，自身の連想配列に追加
 *
 *  自身の連想配列に，指定された複数の要素を追加します．<br>
 *  キーが重複した場合，実行時に警告が出力され，処理が行われません．
 * 
 * @param   src 源泉となる要素を持つasarray_tstringのオブジェクト
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
asarray_tstring &asarray_tstring::append( const asarray_tstring &src )
{
    size_t i;
    if ( &src == this ) {
	/* キーが重複するので自身を追加することはできない */
	err_report(__FUNCTION__,"WARNING", "cannot append self-object");
	goto quit;
    }
    for ( i=0 ; i < src.length() ; i++ ) {
	this->append(src.key(i),src.cstr(i));
    }
 quit:
    return *this;
}

/**
 * @brief  指定された連想配列(asarrdef_tstring)を，自身の連想配列に追加
 *
 *  自身の連想配列に，指定された複数の要素を追加します．<br>
 *  elements は {NULL,NULL} で終端している必要があります．<br>
 *  キーが重複した場合，実行時に警告が出力され，処理が行われません．
 * 
 * @param   elements[] 源泉となるasarrdef_tstring型の配列
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
asarray_tstring &asarray_tstring::append( const asarrdef_tstring elements[] )
{
    if ( elements != NULL ) {
	size_t i;
	for ( i=0 ; elements[i].key != NULL ; i++ ) {
	    this->append(elements[i].key,elements[i].value);
	}
    }
    return *this;
}

/**
 * @brief  指定された連想配列(asarrdef_tstring)のn個を，自身の連想配列に追加
 *
 *  自身の連想配列に，elements の先頭から n個の要素を追加します．<br>
 *  キーが重複した場合，実行時に警告が出力され，処理が行われません．
 * 
 * @param   elements[] 源泉となるasarrdef_tstring型の配列
 * @param   n 配列elementsの個数
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
asarray_tstring &asarray_tstring::append( const asarrdef_tstring elements[],
					  size_t n )
{
    if ( 0 < n && elements != NULL ) {
	size_t i;
	for ( i=0 ; i < n && elements[i].key != NULL ; i++ ) {
	    this->append(elements[i].key,elements[i].value);
	}
    }
    return *this;
}

/**
 * @brief  指定された1要素(const char *)を，自身の連想配列に追加
 *
 *  自身の連想配列に，指定された 1つの要素を追加します．<br>
 *  キーが重複した場合，実行時に警告が出力され，処理が行われません．
 * 
 * @param   key 連想配列に追加するキー文字列
 * @param   val 連想配列に追加する値文字列
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
asarray_tstring &asarray_tstring::append( const char *key, const char *val )
{
    if ( key != NULL ) {
	if ( this->index_rec.index(key,0) < 0 ) {
	    if ( val == NULL ) val = "";
	    this->value_rec.append(val,1);
	    this->key_rec.append(key,1);
	    this->index_rec.append(key,this->key_rec.length()-1);
	}
	else {
	    err_report1(__FUNCTION__,"WARNING","cannot append key '%s'",key);
	}
    }
    return *this;
}

/**
 * @brief  指定された1要素(tstring)を，自身の連想配列に追加
 *
 *  自身の連想配列に，指定された 1つの要素を追加します．<br>
 *  キーが重複した場合，実行時に警告が出力され，処理が行われません．
 * 
 * @param   key 連想配列に追加するキー文字列
 * @param   val 連想配列に追加する値文字列
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
asarray_tstring &asarray_tstring::append( const char *key, const tstring &val )
{
    return this->append(key, val.cstr());
}

/**
 * @brief  指定された1要素(printf()の記法で指定)を，自身の連想配列に追加
 *
 *  自身の連想配列に，指定された1つの要素を追加します．<br>
 *  このメンバ関数では，指定したい値文字列を printf() 関数と同様のフォーマット
 *  と可変引数でセットできます．<br>
 *  キーが重複した場合，実行時に警告が出力され，処理が行われません．
 * 
 * @param   key 連想配列に追加するキー文字列
 * @param   fmt 追加する値文字列のためのフォーマット指定
 * @param   ... fmtに対応した可変引数の各要素データ
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
asarray_tstring &asarray_tstring::appendf( const char *key, 
					   const char *fmt, ... )
{
    va_list ap;
    va_start(ap,fmt);
    try {
	this->vappendf(key,fmt,ap);
    }
    catch (...) {
	va_end(ap);
        err_throw(__FUNCTION__,"FATAL","this->vappendf() failed");
    }
    va_end(ap);
    return *this;
}


/**
 * @brief  指定された1要素(printf()の記法で指定)を，自身の連想配列に追加
 *
 *  自身の連想配列に，指定された1つの要素を追加します．<br>
 *  このメンバ関数では，指定したい値文字列を printf() 関数と同様のフォーマット
 *  と可変引数でセットできます．<br>
 *  キーが重複した場合，実行時に警告が出力され，処理が行われません．
 * 
 * @param   key 連想配列に追加するキー文字列
 * @param   fmt 追加する値文字列のためのフォーマット指定
 * @param   ap fmtに対応した可変引数のリスト
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
asarray_tstring &asarray_tstring::vappendf( const char *key,
					    const char *fmt, va_list ap )
{
    tstring buf;

    if ( fmt == NULL ) return this->append(key,fmt);

    buf.vprintf(fmt,ap);
    this->append(key,buf.cstr());

    return *this;
}

/**
 * @brief  指定された複数の要素を，自身の連想配列に追加
 *
 *  自身の連想配列に，指定された複数の要素を追加します．<br>
 *  可変長引数の要素データの最後は NULL である必要があります．<br>
 *  キーが重複した場合，実行時に警告が出力され，処理が行われません．
 *
 * @param   key0 連想配列に追加するキー文字列
 * @param   val0 連想配列に追加する値文字列
 * @param   key1 連想配列に追加するキー文字列
 * @param   ... キー,値となる文字列の可変長引数の各要素データ
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
asarray_tstring &asarray_tstring::append( const char *key0, const char *val0,
					  const char *key1, ... )
{
    va_list ap;
    va_start(ap,key1);
    try {
	this->vappend(key0,val0,key1,ap);
    }
    catch (...) {
	va_end(ap);
        err_throw(__FUNCTION__,"FATAL","this->vappend() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  指定された複数の要素を，自身の連想配列に追加
 *
 *  自身の連想配列に，指定された複数の要素を追加します．<br>
 *  可変長引数のリストの終端のデータはNULLである必要があります．<br>
 *  キーが重複した場合，実行時に警告が出力され，処理が行われません．
 *
 * @param   key0 連想配列に追加するキー文字列
 * @param   val0 連想配列に追加する値文字列
 * @param   key1 連想配列に追加するキー文字列
 * @param   ap キー,値となる文字列の可変長引数のリスト
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
asarray_tstring &asarray_tstring::vappend( const char *key0, const char *val0,
					   const char *key1, va_list ap )
{
    this->append(key0,val0);
    if ( key0 != NULL && key1 != NULL ) {
	const char *val1 = va_arg(ap,char *);
	this->append(key1,val1);
	while ( 1 ) {
	    const char *keyx = va_arg(ap,char *);
	    const char *valx;
	    if ( keyx == NULL ) break;
	    valx = va_arg(ap,const char *);
	    this->append(keyx,valx);
	}
    }
    return *this;
}

/**
 * @brief  指定された連想配列(asarray_tstring)を，自身の連想配列に挿入
 *
 *  自身の連想配列のキー key の要素位置の前に，指定された複数の要素を挿入
 *  します．<br>
 *  キーが重複した場合，実行時に警告が出力され，処理が行われません．
 * 
 * @param   key 挿入位置にある連想配列のキー文字列
 * @param   src 源泉となる要素を持つasarray_tstringクラスのオブジェクト
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
asarray_tstring &asarray_tstring::insert( const char *key,
					  const asarray_tstring &src )
{
    size_t i;
    if ( &src == this ) {
	/* キーが重複するので自身をインサートすることはできない */
	err_report(__FUNCTION__,"WARNING", "cannot insert self-object");
	goto quit;
    }
    for ( i=0 ; i < src.length() ; i++ ) {
	this->insert(key,src.key(i),src.cstr(i));
    }
 quit:
    return *this;
}


/**
 * @brief  指定された連想配列(asarrdef_tstring)を，自身の連想配列に挿入
 *
 *  自身の連想配列のキー key の要素位置の前に，指定された複数の要素を挿入
 *  します．<br>
 *  elements の最後の要素は，{NULL,NULL} である必要があります．<br>
 *  キーが重複した場合，実行時に警告が出力され，処理が行われません．
 *
 * @param   key 挿入位置にある連想配列のキー文字列
 * @param   elements[] 源泉となる要素を持つasarrdef_tstring型の配列
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
asarray_tstring &asarray_tstring::insert( const char *key, 
					  const asarrdef_tstring elements[] )
{
    if ( key != NULL && elements != NULL ) {
	size_t i;
	if ( this->index_rec.index(key,0) < 0 ) {
	    err_report1(__FUNCTION__,"WARNING","unknown key '%s'",key);
	    goto quit;
	}
	for ( i=0 ; elements[i].key != NULL ; i++ ) {
	    this->insert(key,elements[i].key,elements[i].value);
	}
    }
 quit:
    return *this;
}

/**
 * @brief  指定された連想配列(asarrdef_tstring)のn個を，自身の連想配列に挿入
 *
 *  自身の連想配列のキー key の要素位置の前に，elements の先頭から n個の要素を
 *  挿入します．<br>
 *  キーが重複した場合，実行時に警告が出力され，処理が行われません．
 * 
 * @param   key 挿入位置にある連想配列のキー文字列
 * @param   elements[] 源泉となる要素を持つasarrdef_tstring型の配列
 * @param   n 配列elementsの個数
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
asarray_tstring &asarray_tstring::insert( const char *key, 
					  const asarrdef_tstring elements[],
					  size_t n )
{
    if ( key != NULL && 0 < n && elements != NULL ) {
	size_t i;
	if ( this->index_rec.index(key,0) < 0 ) {
	    err_report1(__FUNCTION__,"WARNING","unknown key '%s'",key);
	    goto quit;
	}
	for ( i=0 ; i < n && elements[i].key != NULL ; i++ ) {
	    this->insert(key,elements[i].key,elements[i].value);
	}
    }
 quit:
    return *this;
}

/**
 * @brief  指定された1要素(const char *)を，自身の連想配列に挿入
 *
 *  自身の連想配列のキー key の要素位置の前に，指定された 1つの要素を挿入
 *  します．<br>
 *  キーが重複した場合，実行時に警告が出力され，処理が行われません．
 * 
 * @param   key 挿入位置にある地震の連想配列のキー文字列
 * @param   newkey 連想配列に挿入するキー文字列
 * @param   newval 連想配列に挿入する値文字列
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
asarray_tstring &asarray_tstring::insert( const char *key,
				       const char *newkey, const char *newval )
{
    if ( key != NULL && newkey != NULL ) {
	size_t i;
	ssize_t idx = this->index_rec.index(key,0);
	if ( idx < 0 ) {
	    err_report1(__FUNCTION__,"WARNING","unknown key '%s'",key);
	    goto quit;
	}
	if ( 0 <= this->index_rec.index(newkey,0) ) {
	    err_report1(__FUNCTION__,"WARNING","cannot insert key '%s'",newkey);
	    goto quit;
	}
	if ( newval == NULL ) newval = "";
	this->value_rec.insert(idx,newval,1);
	this->key_rec.insert(idx,newkey,1);
	this->index_rec.append(newkey,idx);
	for ( i=idx+1 ; i < this->key_rec.length() ; i++ ) {
	    this->index_rec.update(this->key_rec.cstr(i),i-1,i);
	}
    }
 quit:
    return *this;
}

/**
 * @brief  指定された1要素(tstring)を，自身の連想配列に挿入
 *
 *  自身の連想配列のキー key の要素位置の前に，指定された 1つの要素を挿入
 *  します．<br>
 *  キーが重複した場合，実行時に警告が出力され，処理が行われません．
 * 
 * @param   key 挿入位置にある地震の連想配列のキー文字列
 * @param   newkey 連想配列に挿入するキー文字列
 * @param   newval 連想配列に挿入する値文字列
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
asarray_tstring &asarray_tstring::insert( const char *key,
				    const char *newkey, const tstring &newval )
{
    return this->insert(key, newkey, newval.cstr());
}

/**
 * @brief  指定された1要素(printf()の記法で指定)を，自身の連想配列に挿入
 *
 *  自身の連想配列のキー key の要素位置の前に，指定された 1つの要素を挿入
 *  します．<br>
 *  このメンバ関数では，指定したい値文字列を printf() 関数と同様のフォーマット
 *  と可変引数でセットできます．<br>
 *  キーが重複した場合，実行時に警告が出力され，処理が行われません．
 * 
 * @param   key 挿入位置にある地震の連想配列のキーの文字列
 * @param   newkey 連想配列に挿入するキー文字列
 * @param   fmt 挿入する値文字列のためのフォーマット指定
 * @param   ... fmtに対応した可変長引数の各要素データ
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
asarray_tstring &asarray_tstring::insertf( const char *key,
					   const char *newkey, 
					   const char *fmt, ... )
{
    va_list ap;
    va_start(ap,fmt);
    try {
	this->vinsertf(key,newkey,fmt,ap);
    }
    catch (...) {
	va_end(ap);
        err_throw(__FUNCTION__,"FATAL","this->vinsertf() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  指定された1要素(printf()の記法で指定)を，自身の連想配列に挿入
 *
 *  自身の連想配列のキー key の要素位置の前に，指定された 1つの要素を挿入
 *  します．<br>
 *  このメンバ関数では，指定したい値文字列を printf() 関数と同様のフォーマット
 *  と可変引数でセットできます．<br>
 *  キーが重複した場合，実行時に警告が出力され，処理が行われません．
 * 
 * @param   key 挿入位置にある自身の連想配列のキーの文字列
 * @param   newkey 連想配列に挿入するキー文字列
 * @param   fmt 挿入する値文字列のためのフォーマット指定
 * @param   ap fmtに対応した可変長引数の各要素データ
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
asarray_tstring &asarray_tstring::vinsertf( const char *key,
					    const char *newkey,
					    const char *fmt, va_list ap )
{
    tstring buf;

    if ( fmt == NULL ) return this->insert(key,newkey,fmt);

    buf.vprintf(fmt,ap);
    this->insert(key,newkey,buf.cstr());

    return *this;
}

/**
 * @brief  指定された複数の要素を，自身の連想配列に挿入
 *
 *  自身の連想配列のキー key の要素位置の前に，指定された複数の要素を挿入
 *  します．<br>
 *  可変長引数の最後の要素は NULL である必要があります．<br>
 *  キーが重複した場合，実行時に警告が出力され，処理が行われません．
 *
 * @param   key 挿入位置にある連想配列のキー文字列
 * @param   key0 連想配列に挿入するキー文字列
 * @param   val0 連想配列に挿入する値文字
 * @param   key1 連想配列に挿入するキー文字列
 * @param   ... キー,値となる文字列の可変長引数の各要素データ
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
asarray_tstring &asarray_tstring::insert( const char *key,
					  const char *key0, const char *val0,
					  const char *key1, ... )
{
    va_list ap;
    va_start(ap,key1);
    try {
	this->vinsert(key,key0,val0,key1,ap);
    }
    catch (...) {
	va_end(ap);
        err_throw(__FUNCTION__,"FATAL","this->vinsert() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  指定された複数の要素を，自身の連想配列に挿入
 *
 *  自身の連想配列のキー key の要素位置の前に，指定された複数の要素を挿入
 *  します．<br>
 *  可変長引数の最後の要素は NULL である必要があります．<br>
 *  キーが重複した場合，実行時に警告が出力され，処理が行われません．
 *
 * @param   key 挿入位置にある連想配列のキー文字列
 * @param   key0 連想配列に挿入するキー文字列
 * @param   val0 連想配列に挿入する値文字
 * @param   key1 連想配列に挿入するキー文字列
 * @param   ap キー,値となる文字列の可変長引数のリスト
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
asarray_tstring &asarray_tstring::vinsert( const char *key,
					   const char *key0, const char *val0,
					   const char *key1, va_list ap )
{
    if ( key == NULL ) goto quit;
    if ( this->index_rec.index(key,0) < 0 ) {
	err_report1(__FUNCTION__,"WARNING","unknown key '%s'",key);
	goto quit;
    }
    this->insert(key,key0,val0);
    if ( key0 != NULL && key1 != NULL ) {
	const char *val1 = va_arg(ap,char *);
	this->insert(key,key1,val1);
	while ( 1 ) {
	    const char *keyx = va_arg(ap,char *);
	    const char *valx;
	    if ( keyx == NULL ) break;
	    valx = va_arg(ap,const char *);
	    this->insert(key,keyx,valx);
	}
    }
 quit:
    return *this;
}

/**
 * @brief  全配列要素の削除
 *
 *  自身の連想配列のすべての要素を削除します．
 * 
 * @return  自身の参照
 * @throw   内部バッファの確保し失敗した場合
 */
asarray_tstring &asarray_tstring::erase()
{
    this->index_rec.init();
    this->key_rec.erase();
    this->value_rec.erase();
    return *this;
}

/**
 * @brief  配列要素の削除
 *
 *  自身の連想配列の key で指定されたキーに対応する要素から num_elements 個の
 *  要素を消去します．<br>
 *  消去された分だけ配列長は短くなります．
 * 
 * @param   key キー文字列
 * @param   num_elements 削除する要素の個数 (省略時 1)
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
asarray_tstring &asarray_tstring::erase( const char *key, size_t num_elements )
{
    size_t i, maxels;
    ssize_t idx;
    if ( key == NULL ) goto quit;
    idx = this->index_rec.index(key,0);
    if ( idx < 0 ) {
	err_report1(__FUNCTION__,"WARNING","unknown key '%s'",key);
	goto quit;
    }
    maxels = this->key_rec.length() - idx;
    if ( maxels < num_elements ) num_elements = maxels;

    for ( i=idx ; i < idx + num_elements ; i++ ) {
	this->index_rec.erase(this->key_rec.cstr(i),i);
    }
    for ( ; i < this->key_rec.length() ; i++ ) {
	this->index_rec.update(this->key_rec.cstr(i),i,i-num_elements);
    }

    this->key_rec.erase(idx,num_elements);
    this->value_rec.erase(idx,num_elements);

 quit:
    return *this;
}

/**
 * @brief  2つのオブジェクト間での内容のスワップ
 *
 *  オブジェクトsobj の内容と自身の内容とを入れ替えます．
 * 
 * @param   sobj 内容を入れ替える asarray_tstring クラスのオブジェクト
 * @return  自身の参照
 */
asarray_tstring &asarray_tstring::swap( asarray_tstring &sobj )
{
    if ( &sobj == this ) return *this;
    this->key_rec.swap( sobj.key_rec );
    this->value_rec.swap( sobj.value_rec );
    this->index_rec.swap( sobj.index_rec );
    this->regex_rec.swap( sobj.regex_rec );
    return *this;
}

/**
 * @brief  キー文字列の変更
 *
 *  自身の連想配列のキー文字列 org_key を new_key で指定された文字列に変更
 *  します．<br>
 *  org_key が存在しない場合や，new_key が重複するキー文字列だった場合は警告が
 *  出力されます．
 * 
 * @param   org_key 元のキー文字列
 * @param   new_key 変更後のキー文字列
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
asarray_tstring &asarray_tstring::rename_a_key( const char *org_key, 
						const char *new_key )
{
    ssize_t idx;
    idx = this->index_rec.index(org_key,0);
    if ( idx < 0 ) {
	err_report1(__FUNCTION__,"WARNING","unknown key '%s'",org_key);
	goto quit;
    }
    if ( new_key == NULL || this->key_rec.at(idx).compare(new_key) == 0 )
	goto quit;
    if ( 0 <= this->index_rec.index(new_key,0) ) {
	err_report1(__FUNCTION__,"WARNING",
		    "already used: key '%s'. Not renamed.",new_key);
	goto quit;
    }
    this->index_rec.erase(org_key, idx);
    this->index_rec.append(new_key, idx);
    this->key_rec.at(idx).assign(new_key);
 quit:
    return *this;
}

/* */

/**
 * @brief  配列の全要素についての単純な文字列検索と置換
 *
 *  自身の連想配列の全要素の値文字列について，文字列の左側から文字列 org_str を
 *  検索し，見つかった場合は文字列 new_str で置き換えます．
 * 
 * @param   org_str 検出する文字列
 * @param   new_str 置換の源泉となる文字列
 * @param   all 全置換のフラグ(省略時false)
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
asarray_tstring &asarray_tstring::strreplace( const char *org_str,
					      const char *new_str, bool all )
{
    size_t i;
    for ( i=0 ; i < this->length() ; i++ ) {
	this->at(i).strreplace(org_str, new_str, all);
    }
    return *this;
}

/**
 * @brief  配列の全要素についての単純な文字列検索と置換
 *
 *  自身の連想配列の全要素の値文字列について，文字列の左側から文字列 org_str を
 *  検索し，見つかった場合は文字列 new_str で置き換えます．
 * 
 * @param   org_str 検出する文字列
 * @param   new_str 置換の源泉となる文字列
 * @param   all 全置換のフラグ(省略時false)
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
asarray_tstring &asarray_tstring::strreplace( const tstring &org_str,
					      const char *new_str, bool all )
{
    return this->strreplace(org_str.cstr(), new_str, all);
}

/**
 * @brief  配列の全要素についての単純な文字列検索と置換
 *
 *  自身の連想配列の全要素の値文字列について，文字列の左側から文字列 org_str を
 *  検索し，見つかった場合は文字列 new_str で置き換えます．
 * 
 * @param   org_str 検出する文字列
 * @param   new_str 置換の源泉となる文字列
 * @param   all 全置換のフラグ(省略時false)
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
asarray_tstring &asarray_tstring::strreplace( const char *org_str,
					     const tstring &new_str, bool all )
{
    return this->strreplace(org_str, new_str.cstr(), all);
}

/**
 * @brief  配列の全要素についての単純な文字列検索と置換
 *
 *  自身の連想配列の全要素の値文字列について，文字列の左側から文字列 org_str を
 *  検索し，見つかった場合は文字列 new_str で置き換えます．
 * 
 * @param   org_str 検出する文字列
 * @param   new_str 置換の源泉となる文字列
 * @param   all 全置換のフラグ(省略時false)
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
asarray_tstring &asarray_tstring::strreplace( const tstring &org_str,
					     const tstring &new_str, bool all )
{
    return this->strreplace(org_str.cstr(), new_str.cstr(), all);
}

/**
 * @brief  配列の全要素についての末尾文字の除去
 *
 *  自身の全要素の値文字列の末尾の文字を削除します．
 * 
 * @return  自身の参照
 * @throw   指定された要素番号が不正な場合
 * @throw   内部バッファの確保に失敗した場合
 */
asarray_tstring &asarray_tstring::chop()
{
    size_t i;
    for ( i=0 ; i < this->length() ; i++ ) {
	this->at(i).chop();
    }
    return *this;
}

/**
 * @brief  配列の全要素についての右端の改行文字の除去
 *
 *  自身の連想配列の全要素の値文字列の右端の改行文字を除去します．
 * 
 * @param   rs 改行文字列 (省略可)
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
asarray_tstring &asarray_tstring::chomp( const char *rs )
{
    size_t i;
    for ( i=0 ; i < this->length() ; i++ ) {
	this->at(i).chomp(rs);
    }
    return *this;
}

/**
 * @brief  配列の全要素についての右端の改行文字の除去
 *
 *  自身の連想配列の全要素の値文字列の右端の改行文字を除去します．
 * 
 * @param   rs 改行文字列 (省略可)
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
asarray_tstring &asarray_tstring::chomp( const tstring &rs )
{
    return this->chomp(rs.cstr());
}

/**
 * @brief  配列の全要素についての文字列両端の不要文字の除去
 *
 *  自身の文字列配列の全要素について，文字列両端にある任意文字(side_spaces
 *  で指定)を除去します．<br>
 *  side_spacesを省略した場合，" \t\n\r\f\v" を指定したものとみなします．
 * 
 * @param   side_spaces 除去対象の文字セット．[A-Z] といった表現も使用可能．
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
asarray_tstring &asarray_tstring::trim( const char *side_spaces )
{
    size_t i;
    for ( i=0 ; i < this->length() ; i++ ) {
	this->at(i).trim(side_spaces);
    }
    return *this;
}

/**
 * @brief  配列の全要素についての文字列両端の不要文字の除去
 *
 *  自身の文字列配列の全要素について，文字列両端にある任意文字(side_spaces
 *  で指定)を除去します．
 * 
 * @param   side_spaces 除去対象の文字セット．[A-Z] といった表現も使用可能．
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
asarray_tstring &asarray_tstring::trim( const tstring &side_spaces )
{
    size_t i;
    for ( i=0 ; i < this->length() ; i++ ) {
	this->at(i).trim(side_spaces);
    }
    return *this;
}

/**
 * @brief  配列の全要素についての文字列両端の不要文字の除去
 *
 *  自身の全ての要素の値文字列の両端にある任意文字を除去します．
 * 
 * @param   side_space 除去対象の文字
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
asarray_tstring &asarray_tstring::trim( int side_space )
{
    size_t i;
    for ( i=0 ; i < this->length() ; i++ ) {
	this->at(i).trim(side_space);
    }
    return *this;
}

/**
 * @brief  配列の全要素についての文字列左端の不要文字の除去
 *
 *  自身の文字列配列の全要素について，文字列左端にある任意文字(side_spaces
 *  で指定)を除去します．<br>
 *  side_spacesを省略した場合，" \t\n\r\f\v" を指定したものとみなします．
 *
 * @param   side_spaces 除去対象の文字セット．[A-Z] といった表現も使用可能．
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
asarray_tstring &asarray_tstring::ltrim( const char *side_spaces )
{
    size_t i;
    for ( i=0 ; i < this->length() ; i++ ) {
	this->at(i).ltrim(side_spaces);
    }
    return *this;
}

/**
 * @brief  配列の全要素についての文字列左端の不要文字の除去
 *
 *  自身の文字列配列の全要素について，文字列左端にある任意文字(side_spaces
 *  で指定)を除去します．<br>
 *
 * @param   side_spaces 除去対象の文字セット．[A-Z] といった表現も使用可能．
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
asarray_tstring &asarray_tstring::ltrim( const tstring &side_spaces )
{
    size_t i;
    for ( i=0 ; i < this->length() ; i++ ) {
	this->at(i).ltrim(side_spaces);
    }
    return *this;
}

/**
 * @brief  配列の全要素についての文字列左端の不要文字の除去
 *
 *  自身の全ての要素の値文字列の左端にある任意文字を除去します．
 *
 * @param   side_space 除去対象の文字
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
asarray_tstring &asarray_tstring::ltrim( int side_space )
{
    size_t i;
    for ( i=0 ; i < this->length() ; i++ ) {
	this->at(i).ltrim(side_space);
    }
    return *this;
}

/**
 * @brief  配列の全要素についての文字列右端の不要文字の除去
 *
 *  自身の文字列配列の全要素について，文字列右端にある任意文字(side_spaces
 *  で指定)を除去します．<br>
 *  side_spacesを省略した場合，" \t\n\r\f\v" を指定したものとみなします．
 *
 * @param   side_spaces 除去対象の文字セット．[A-Z] といった表現も使用可能．
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
asarray_tstring &asarray_tstring::rtrim( const char *side_spaces )
{
    size_t i;
    for ( i=0 ; i < this->length() ; i++ ) {
	this->at(i).rtrim(side_spaces);
    }
    return *this;
}

/**
 * @brief  配列の全要素についての文字列右端の不要文字の除去
 *
 *  自身の文字列配列の全要素について，文字列右端にある任意文字(side_spaces
 *  で指定)を除去します．
 *
 * @param   side_spaces 除去対象の文字セット．[A-Z] といった表現も使用可能．
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
asarray_tstring &asarray_tstring::rtrim( const tstring &side_spaces )
{
    size_t i;
    for ( i=0 ; i < this->length() ; i++ ) {
	this->at(i).rtrim(side_spaces);
    }
    return *this;
}

/**
 * @brief  配列の全要素についての文字列右端の不要文字の除去
 *
 *  自身の全ての要素の値文字列の右端にある任意文字を除去します．
 *
 * @param   side_space 除去対象の文字
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
asarray_tstring &asarray_tstring::rtrim( int side_space )
{
    size_t i;
    for ( i=0 ; i < this->length() ; i++ ) {
	this->at(i).rtrim(side_space);
    }
    return *this;
}

/**
 * @brief  配列の全要素についての文字列両端の不要文字の除去
 *
 *  自身の文字列配列の全要素について，文字列両端にある任意文字(side_spaces
 *  で指定)を除去します．<br>
 *  side_spacesを省略した場合，" \t\n\r\f\v" を指定したものとみなします．
 * 
 * @param   side_spaces 除去対象の文字セット．[A-Z] といった表現も使用可能．
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
asarray_tstring &asarray_tstring::strtrim( const char *side_spaces )
{
    return this->trim(side_spaces);
}

/**
 * @brief  配列の全要素についての文字列両端の不要文字の除去
 *
 *  自身の文字列配列の全要素について，文字列両端にある任意文字(side_spaces
 *  で指定)を除去します．
 * 
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
asarray_tstring &asarray_tstring::strtrim( const tstring &side_spaces )
{
    return this->trim(side_spaces);
}

/**
 * @brief  配列の全要素についての文字列両端の不要文字の除去
 *
 *  自身の全ての要素の値文字列の両端にある任意文字を除去します．
 * 
 * @param   side_space 除去対象の文字
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
asarray_tstring &asarray_tstring::strtrim( int side_space )
{
    return this->trim(side_space);
}

/**
 * @brief  配列の全要素について大文字を小文字に変換
 *
 *  自身の連想配列の全要素のアルファベットの大文字を小文字に変換します．
 * 
 * @return  自身の参照
 */
asarray_tstring &asarray_tstring::tolower()
{
    size_t i;
    for ( i=0 ; i < this->length() ; i++ ) {
	this->at(i).tolower();
    }
    return *this;
}

/**
 * @brief  配列の全要素について小文字を大文字に変換
 *
 *  自身の連想配列の全要素のアルファベットの小文字を大文字に変換します．
 * 
 * @return  自身の参照
 */
asarray_tstring &asarray_tstring::toupper()
{
    size_t i;
    for ( i=0 ; i < this->length() ; i++ ) {
	this->at(i).toupper();
    }
    return *this;
}

/**
 * @brief  配列の全要素についてタブ文字を桁揃えして空白文字に置換
 *
 *  自身の連想配列の全要素について，水平タブ文字 '\t' を，tab_width の値に
 *  桁揃えをして空白文字に置換します．
 * 
 * @param   tab_width タブ幅．省略時は8
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
asarray_tstring &asarray_tstring::expand_tabs( size_t tab_width )
{
    size_t i;
    for ( i=0 ; i < this->length() ; i++ ) {
	this->at(i).expand_tabs(tab_width);
    }
    return *this;
}

/**
 * @brief  配列の全要素について空白文字を桁揃えしてタブ文字に置換
 *
 *  自身の文字列配列の全要素について，2文字以上連続した空白文字 ' ' すべてを
 *  対象にし，指定されたタブ幅 tab_width で桁揃えして '\t' で置換します．
 * 
 * @param   tab_width タブ幅．省略時は8
 * @return  自身の参照
 * @throws  内部バッファの確保に失敗した場合
 */
asarray_tstring &asarray_tstring::contract_spaces( size_t tab_width )
{
    size_t i;
    for ( i=0 ; i < this->length() ; i++ ) {
	this->at(i).contract_spaces(tab_width);
    }
    return *this;
}

/**
 * @brief  配列の全要素についての正規表現による置換
 *
 *  自身の連想配列の全要素の値文字列を，pat で指定された POSIX 拡張正規表現で
 *  マッチした部分を文字列 new_str で置き換えます.
 * 
 * @param   pat 文字列パターン(正規表現)
 * @param   new_str 置換後の文字列
 * @param   all 全置換のフラグ
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
asarray_tstring &asarray_tstring::regreplace( const char *pat, 
					      const char *new_str, bool all )
{
    size_t i;
    if ( this->regex_rec.compile(pat) < 0 ) {
	err_report1(__FUNCTION__,"ERROR","%s",this->regex_rec.cerrstr());
    }
    for ( i=0 ; i < this->length() ; i++ ) {
	this->at(i).regreplace(this->regex_rec, new_str, all);
    }
    return *this;
}

/**
 * @brief  配列の全要素についての正規表現による置換
 *
 *  自身の連想配列の全要素の値文字列を，pat で指定された POSIX 拡張正規表現で
 *  マッチした部分を文字列 new_str で置き換えます.
 * 
 * @param   pat 文字列パターン(正規表現)
 * @param   new_str 置換後の文字列
 * @param   all 全置換のフラグ
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
asarray_tstring &asarray_tstring::regreplace( const tstring &pat, 
					      const char *new_str, bool all )
{
    size_t i;
    if ( this->regex_rec.compile(pat.cstr()) < 0 ) {
	err_report1(__FUNCTION__,"ERROR","%s",this->regex_rec.cerrstr());
    }
    for ( i=0 ; i < this->length() ; i++ ) {
	this->at(i).regreplace(this->regex_rec, new_str, all);
    }
    return *this;
}

/**
 * @brief  配列の全要素についての正規表現による置換
 *
 *  自身の連想配列の全要素の値文字列を，pat で指定された POSIX 拡張正規表現で
 *  マッチした部分を文字列 new_str で置き換えます.
 * 
 * @param   pat tregexのコンパイル済みオブジェクト
 * @param   new_str 置換後の文字列
 * @param   all 全置換のフラグ
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
asarray_tstring &asarray_tstring::regreplace( const tregex &pat, 
					      const char *new_str, bool all )
{
    size_t i;
    for ( i=0 ; i < this->length() ; i++ ) {
	this->at(i).regreplace(pat,new_str,all);
    }
    return *this;
}

/**
 * @brief  配列の全要素についての正規表現による置換
 *
 *  自身の連想配列の全要素の値文字列を，pat で指定された POSIX 拡張正規表現で
 *  マッチした部分を文字列 new_str で置き換えます.
 * 
 * @param   pat 文字列パターン
 * @param   new_str 置換後の文字列
 * @param   all 全置換のフラグ
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
asarray_tstring &asarray_tstring::regreplace( const char *pat, 
					     const tstring &new_str, bool all )
{
    return this->regreplace(pat, new_str.cstr(), all);
}

/**
 * @brief  配列の全要素についての正規表現による置換
 *
 *  自身の連想配列の全要素の値文字列を，pat で指定された POSIX 拡張正規表現で
 *  マッチした部分を文字列 new_str で置き換えます.
 * 
 * @param   pat 文字列パターン
 * @param   new_str 置換後の文字列
 * @param   all 全置換のフラグ
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
asarray_tstring &asarray_tstring::regreplace( const tstring &pat, 
					     const tstring &new_str, bool all )
{
    return this->regreplace(pat, new_str.cstr(), all);
}

/**
 * @brief  配列の全要素についての正規表現による置換
 *
 *  自身の連想配列の全要素の値文字列を，pat で指定された POSIX 拡張正規表現で
 *  マッチした部分を文字列 new_str で置き換えます.
 * 
 * @param   pat tregexクラスのコンパイル済みオブジェクト
 * @param   new_str 置換後の文字列
 * @param   all 全置換のフラグ
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
asarray_tstring &asarray_tstring::regreplace( const tregex &pat, 
					     const tstring &new_str, bool all )
{
    return this->regreplace(pat, new_str.cstr(), all);
}

/* */

/**
 * @brief  連想配列の値文字列に対するポインタ配列を取得 (読取専用)
 *
 *  自身が持つ連想配列の値文字列のポインタ配列を返します．<br>
 *  ポインタ配列は，必ず NULL で終端しています．
 * 
 * @return  連想配列の値文字列へのポインタ配列
 */
const char *const *asarray_tstring::cstrarray() const
{
    return this->value_rec.cstrarray();
}

/**
 * @brief  指定されたキーに対応する値文字列を返す (読取専用)
 *
 *  指定されたキーに対応する連想配列要素の文字列のアドレスを返します．
 * 
 * @param   key 連想配列のためのキー文字列
 * @return  正常終了時: 指定されたキーに対応する文字列のアドレス．<br>
 *          キー値が不正な場合: NULL．
 * @note    asarray_tstring::cstr() との違いはありません．
 */
const char *asarray_tstring::c_str( const char *key ) const
{
    ssize_t idx;
    if ( key == NULL ) return NULL;
    idx = this->index_rec.index(key,0);
    if ( idx < 0 ) return NULL;
    return this->value_rec.c_str(idx);
}

/**
 * @brief  指定されたキーに対応する値文字列を返す (読取専用)
 *
 *  指定されたキーに対応する連想配列要素の文字列のアドレスを返します．
 * 
 * @param   key 連想配列のためのキー文字列
 * @return  正常終了時: 指定されたキーに対応する文字列のアドレス.<br>
 *          キー値が不正な場合: NULL．
 * @note    asarray_tstring::c_str() との違いはありません．
 */
const char *asarray_tstring::cstr( const char *key ) const
{
    ssize_t idx;
    if ( key == NULL ) return NULL;
    idx = this->index_rec.index(key,0);
    if ( idx < 0 ) return NULL;
    return this->value_rec.cstr(idx);
}

/**
 * @brief  指定されたキーに対応する値文字列を返す (読取専用・printf()の記法)
 *
 *  指定されたキーに対応する連想配列要素の文字列のアドレスを返します．<br>
 *  このメンバ関数は指定したいキー文字列を printf() 関数と同様のフォーマットと
 *  可変引数でセットできます．
 * 
 * @param   fmt キー文字列のためのフォーマット文字列
 * @param   ... fmtに対応した可変長引数の各要素データ
 * @return  正常終了時: 指定されたキーに対応する文字列のアドレス．<br>
 *          キー値が不正な場合: NULL．
 */
const char *asarray_tstring::cstrf( const char *fmt, ... ) const
{
    const char *ret;
    va_list ap;
    va_start(ap,fmt);
    try {
	ret = this->vcstrf(fmt,ap);
    }
    catch (...) {
	va_end(ap);
        err_throw(__FUNCTION__,"FATAL","this->vcstrf() failed");
    }
    va_end(ap);
    return ret;
}

/**
 * @brief  指定されたキーに対応する値文字列を返す (読取専用・printf()の記法)
 *
 *  指定されたキーに対応する連想配列要素の文字列のアドレスを返します．<br>
 *  このメンバ関数は指定したいキー文字列を printf() 関数と同様のフォーマットと
 *  可変引数でセットできます．
 * 
 * @param   fmt キー文字列のためのフォーマット文字列
 * @param   ap fmtに対応した可変長引数のリスト
 * @return  正常終了時: 指定されたキーに対応する文字列のアドレス．<br>
 *          キー値が不正な場合: NULL．
 */
const char *asarray_tstring::vcstrf( const char *fmt, va_list ap ) const
{
    tstring buf;

    if ( fmt == NULL ) return this->cstr(fmt);

    buf.vprintf(fmt,ap);
    return this->cstr(buf.cstr());
}

/**
 * @brief  指定されたキー文字列に対応する配列要素の参照(tstring)を返す
 *
 * @param   key 連想配列のためのキー文字列
 * @return  指定されたキーに対応する要素値の参照
 * @throw   指定されたキー文字列がNULLの場合
 * @throw   内部バッファの確保に失敗した場合
 * @note    asarray_tstring::at() との違いはありません．
 */
tstring &asarray_tstring::element( const char *key )
{
    return this->at(key);
}

/**
 * @brief  指定されたキー文字列に対応する配列要素の参照(tstring)を返す
 *
 * @param   key 連想配列のためのキー文字列
 * @return  指定されたキーに対応する要素値の参照
 * @throw   指定されたキー文字列がNULLの場合
 * @throw   内部バッファの確保に失敗した場合
 * @note    asarray_tstring::element() との違いはありません．
 */
tstring &asarray_tstring::at( const char *key )
{
    ssize_t idx;
    if ( key == NULL ) {
        err_throw(__FUNCTION__,"ERROR","NULL key...?");
    }
    idx = this->index_rec.index(key,0);
    if ( idx < 0 ) {
	this->append(key,"");
	idx = this->index_rec.index(key,0);
    }
    if ( idx < 0 ) {
        err_throw1(__FUNCTION__,"ERROR","not found key '%s'",key);
    }
    return this->value_rec.at(idx);
}

/**
 * @brief  キー文字列に対応する配列要素の参照(tstring)を返す (printf()の記法)
 *
 *  引数のキー文字列に対応する要素値の参照を返します．<br>
 *  このメンバ関数は指定したいキー文字列を printf() 関数と同様のフォーマットと
 *  可変引数でセットできます．
 * 
 * @param   fmt キー文字列のためのフォーマット指定
 * @param   ... fmtに対応した可変長引数の各要素データ
 * @return  指定されたキーに対応する要素値の参照
 * @throw   指定されたキー文字列がNULLの場合
 * @throw   内部バッファの確保に失敗した場合
 */
tstring &asarray_tstring::atf( const char *fmt, ... )
{
    va_list ap;
    va_start(ap,fmt);
    try {
	tstring &ret = this->vatf(fmt,ap);
	va_end(ap);
	return ret;
    }
    catch (...) {
	va_end(ap);
        err_throw(__FUNCTION__,"FATAL","this->vatf() failed");
    }
}

/**
 * @brief  キー文字列に対応する配列要素の参照(tstring)を返す (printf()の記法)
 *
 *  引数のキー文字列に対応する要素値の参照を返します．<br>
 *  このメンバ関数は指定したいキー文字列を printf() 関数と同様のフォーマットと
 *  可変引数でセットできます．
 * 
 * @param   fmt キー文字列のためのフォーマット指定
 * @param   ap fmtに対応した可変長引数の各要素データ
 * @return  指定されたキーに対応する要素値の参照
 * @throw   指定されたキー文字列がNULLの場合
 * @throw   内部バッファの確保に失敗した場合
 */
tstring &asarray_tstring::vatf( const char *fmt, va_list ap )
{
    tstring buf;

    if ( fmt == NULL ) return this->at(fmt);

    buf.vprintf(fmt,ap);
    return this->at(buf.cstr());
}

#ifdef SLI__OVERLOAD_CONST_AT

/**
 * @brief  キー文字列に対応する配列要素の参照(tstring)を返す (読取専用)
 *
 * @param   key 連想配列のキー文字列
 * @return  指定されたキーに対応する要素値の参照
 * @throw   指定されたキー文字列がNULL又は不正な場合
 * @throw   内部バッファの確保に失敗した場合
 */
const tstring &asarray_tstring::at( const char *key ) const
{
    return this->at_cs(key);
}

/**
 * @brief  キー文字列に対応する配列要素の参照を返す (読取専用・printf()の記法)
 *
 *  引数のキー文字列に対応する要素値の参照(読み取り専用)を返します．<br>
 *  このメンバ関数は指定したいキー文字列を printf() 関数と同様のフォーマットと
 *  可変引数でセットできます．
 * 
 * @param   fmt キー文字列のためのフォーマット指定
 * @param   ... fmtに対応した可変長引数の各要素データ
 * @return  指定されたキーに対応する要素値の参照
 * @throw   指定されたキー文字列がNULL又は存在しない場合
 * @throw   内部バッファの確保に失敗した場合
 */
const tstring &asarray_tstring::atf( const char *fmt, ... ) const
{
    va_list ap;
    va_start(ap,fmt);
    try {
	const tstring &ret = this->vatf_cs(fmt,ap);
	va_end(ap);
	return ret;
    }
    catch (...) {
	va_end(ap);
        err_throw(__FUNCTION__,"FATAL","this->vatf_cs() failed");
    }
}

/**
 * @brief  キー文字列に対応する配列要素の参照を返す (読取専用・printf()の記法)
 *
 *  引数のキー文字列に対応する要素値の参照(読み取り専用)を返します．<br>
 *  このメンバ関数は指定したいキー文字列を printf() 関数と同様のフォーマットと
 *  可変引数でセットできます．
 * 
 * @param   fmt キー文字列のためのフォーマット指定
 * @param   ap fmtに対応した可変長引数のリスト
 * @return  指定されたキーに対応する要素値の参照
 * @throw   指定されたキー文字列がNULL又は存在しない場合
 * @throw   内部バッファの確保に失敗した場合
 */
const tstring &asarray_tstring::vatf( const char *fmt, va_list ap ) const
{
    return this->vatf_cs(fmt, ap);
}
#endif

/**
 * @brief  キー文字列に対応する配列要素の参照(tstring)を返す (読取専用)
 *
 * @param   key 連想配列のキー文字列
 * @return  指定されたキーに対応する要素値の参照
 * @throw   指定されたキー文字列がNULL又は存在しない場合
 */
const tstring &asarray_tstring::at_cs( const char *key ) const
{
    ssize_t idx;
    if ( key == NULL ) {
        err_throw(__FUNCTION__,"ERROR","NULL key...?");
    }
    idx = this->index_rec.index(key,0);
    if ( idx < 0 ) {
        err_throw1(__FUNCTION__,"ERROR","not found key '%s'",key);
    }
    return this->value_rec.at_cs(idx);
}

/**
 * @brief  キー文字列に対応する配列要素の参照を返す (読取専用・printf()の記法)
 *
 *  引数のキー文字列に対応する要素値の参照(読み取り専用)を返します．<br>
 *  このメンバ関数は指定したいキー文字列を printf() 関数と同様のフォーマットと
 *  可変引数でセットできます．
 *
 * @param   fmt キー文字列のためのフォーマット指定
 * @param   ... fmtに対応した可変長引数のリスト
 * @return  指定されたキーに対応する要素値の参照
 * @throw   指定されたキー文字列がNULL又は存在しない場合.
 * @throw   内部バッファの確保に失敗した場合
 */
const tstring &asarray_tstring::atf_cs( const char *fmt, ... ) const
{
    va_list ap;
    va_start(ap,fmt);
    try {
	const tstring &ret = this->vatf_cs(fmt,ap);
	va_end(ap);
	return ret;
    }
    catch (...) {
	va_end(ap);
        err_throw(__FUNCTION__,"FATAL","this->vatf_cs() failed");
    }
}

/**
 * @brief  キー文字列に対応する配列要素の参照を返す (読取専用・printf()の記法)
 *
 *  引数のキー文字列に対応する要素値の参照(読み取り専用)を返します．<br>
 *  このメンバ関数は指定したいキー文字列を printf() 関数と同様のフォーマットと
 *  可変引数でセットできます．
 *
 * @param   fmt キー文字列のためのフォーマット指定
 * @param   ap fmtに対応した可変長引数のリスト
 * @return  指定されたキーに対応する要素値の参照
 * @throw   指定されたキー文字列がNULL又は存在しない場合.
 * @throw   内部バッファの確保に失敗した場合
 */
const tstring &asarray_tstring::vatf_cs( const char *fmt, va_list ap ) const
{
    tstring buf;

    if ( fmt == NULL ) return this->at_cs(fmt);

    buf.vprintf(fmt,ap);
    return this->at_cs(buf.cstr());
}

/**
 * @brief  キー文字列に対応する要素文字列の外部バッファへのコピー
 *
 *  引数で指定されたキーに対応する要素値の文字列を，外部バッファにコピー
 *  します．
 *
 * @param   key キー文字列
 * @param   dest_buf コピー先の外部バッファアドレス
 * @param   buf_size 外部バッファのサイズ
 * @return  非負の値: バッファ長が十分な場合にコピーできる文字数．<br>
 *          負の値: dest_bufにNULLを設定し，buf_sizeに0以外の値を設定した場合．
 */
ssize_t asarray_tstring::getstr( const char *key, 
				 char *dest_buf, size_t buf_size ) const
{
    ssize_t idx;
    if ( key == NULL ) return -1;
    idx = this->index_rec.index(key,0);
    if ( idx < 0 ) return -1;
    return this->value_rec.getstr(idx,dest_buf,buf_size);
}

/**
 * @brief  連想配列の長さ(個数)を取得
 *
 * @return  自身の連想配列の要素数
 */
size_t asarray_tstring::length() const
{
    return this->key_rec.length();
}

/**
 * @brief  連想配列の長さ(個数)を取得
 * 
 * @return 自身の連想配列の要素数
 * @note   asarray_tstring::length() との違いはありません．
 */
size_t asarray_tstring::size() const
{
    return this->key_rec.size();
}

/**
 * @brief  指定されたキー文字列に対応する要素の文字列長を返す
 *
 * @param   key 連想配列のキー文字列
 * @return  指定されたキーに対応する値の文字列長さ
 */
size_t asarray_tstring::length( const char *key ) const
{
    ssize_t idx;
    if ( key == NULL ) return 0;
    idx = this->index_rec.index(key,0);
    if ( idx < 0 ) return 0;
    return this->value_rec.length(idx);
}

/**
 * @brief  指定されたキー文字列に対応する要素の文字列長を返す
 *
 * @param   key 連想配列のキー文字列
 * @return  指定されたキーに対応する値の文字列長さ
 */
size_t asarray_tstring::size( const char *key ) const
{
    ssize_t idx;
    if ( key == NULL ) return 0;
    idx = this->index_rec.index(key,0);
    if ( idx < 0 ) return 0;
    return this->value_rec.size(idx);
}

/**
 * @brief  指定されたキー文字列に対応する要素文字列の位置posにある文字を返す
 *
 *  引数で指定されたキー文字列に対応する要素値の，位置posにある文字を返します．
 * 
 * @param   key 連想配列のキー文字列
 * @param   pos 文字列の位置
 * @return  非負の値: 指定した位置にある文字．<br>
 *          負の値: エラー．
 */
int asarray_tstring::cchr( const char *key, size_t pos ) const
{
    ssize_t idx;
    if ( key == NULL ) return -1;
    idx = this->index_rec.index(key,0);
    if ( idx < 0 ) return -1;
    return this->value_rec.cchr(idx,pos);
}

/**
 * @brief  指定されたキー文字列に対応する要素番号を返す
 * 
 * @param   key キー文字列
 * @return  非負の値: 指定されたキー文字列が見つかった場合．<br>
 *          負の値: 見つからなかった場合．
 */
ssize_t asarray_tstring::index( const char *key ) const
{
    if ( key == NULL ) return -1;
    return this->index_rec.index(key,0);
}

/**
 * @brief  指定されたキー文字列に対応する要素番号を返す (printf()の記法)
 *
 *  キー文字列に対応する要素番号を返します．<br>
 *  このメンバ関数では printf() 関数と同様のフォーマットと可変引数で，キー
 *  文字列をセットできます．
 * 
 * @param   fmt キー文字列のためのフォーマット指定
 * @param   ... fmtに対応した可変長引数の各要素データ
 * @return  非負の値: 指定されたキー文字列が見つかった場合．<br>
 *          負の値: 見つからなかった場合．
 * @throw   内部バッファの確保に失敗した場合
 */
ssize_t asarray_tstring::indexf( const char *fmt, ... ) const
{
    ssize_t ret;
    va_list ap;
    va_start(ap,fmt);
    try {
	ret = this->vindexf(fmt,ap);
    }
    catch (...) {
	va_end(ap);
        err_throw(__FUNCTION__,"FATAL","this->vindexf() failed");
    }
    va_end(ap);
    return ret;
}

/**
 * @brief  指定されたキー文字列に対応する要素番号を返す (printf()の記法)
 *
 *  キー文字列に対応する要素番号を返します．<br>
 *  このメンバ関数では printf() 関数と同様のフォーマットと可変引数で，キー
 *  文字列をセットできます．
 * 
 * @param   fmt キー文字列のためのフォーマット指定
 * @param   ap fmtに対応した可変長引数のリスト
 * @return  非負の値: 指定されたキー文字列が見つかった場合．<br>
 *          負の値: 見つからなかった場合．
 * @throw   内部バッファの確保に失敗した場合
 */
ssize_t asarray_tstring::vindexf( const char *fmt, va_list ap ) const
{
    tstring buf;

    if ( fmt == NULL ) return this->index(fmt);

    buf.vprintf(fmt,ap);
    return this->index(buf.cstr());
}

/**
 * @brief  指定された要素番号に該当するキー文字列を返す
 * 
 *  指定された要素番号に該当するキー文字列のアドレスを返します．
 * 
 * @param   index 要素番号
 * @return   キー文字列の内部バッファのアドレス．<br>
 *           配列の長さ以上の要素番号が指定された場合はNULL
 */
const char *asarray_tstring::key( size_t index ) const
{
    if ( index < 0 || this->length() <= index ) return NULL;
    else return this->key_rec.cstr(index);
}

/**
 * @brief  指定された要素番号に該当する値文字列を返す (読取専用)
 *
 *  指定された要素番号に該当する連想配列の値文字列のアドレスを返します．
 * 
 * @param   index 要素番号
 * @return  要素番号に該当する文字列のアドレス．<br>
 *          要素番号が不正な場合NULL．
 */
const char *asarray_tstring::cstr( size_t index ) const
{
    if ( index < 0 || this->length() <= index ) return NULL;
    else return this->value_rec.cstr(index);
}

/**
 * @brief  指定された要素番号に該当する要素値の参照(tstring)を返す
 *
 * @param   index 要素番号
 * @return  指定された要素番号に該当する要素値の参照
 * @throw   指定された要素番号が不正な場合
 */
tstring &asarray_tstring::at( size_t index )
{
    if ( index < 0 || this->length() <= index ) {
        err_throw1(__FUNCTION__,"ERROR","invalid index: %d",(int)index);
    }
    return this->value_rec.at(index);
}

#ifdef SLI__OVERLOAD_CONST_AT

/**
 * @brief  指定された要素番号に該当する要素値の参照(tstring)を返す (読取専用)
 * 
 * @param   index 要素番号
 * @return  指定された要素番号に該当する要素値の参照
 * @throw   指定された要素番号が不正な場合
 */
const tstring &asarray_tstring::at( size_t index ) const
{
    if ( index < 0 || this->length() <= index ) {
        err_throw1(__FUNCTION__,"ERROR","invalid index: %d",(int)index);
    }
    return this->value_rec.at_cs(index);
}
#endif

/**
 * @brief  指定された要素番号に該当する要素値の参照(tstring)を返す
 * 
 * @param   index 要素番号
 * @return  指定された要素番号に該当する要素値の参照
 * @throw   指定された要素番号が不正な場合
 */
tstring &asarray_tstring::element( size_t index )
{
    if ( index < 0 || this->length() <= index ) {
        err_throw1(__FUNCTION__,"ERROR","invalid index: %d",(int)index);
    }
    return this->value_rec.at(index);
}

/**
 * @brief  指定された要素番号に該当する要素値の参照(tstring)を返す (読取専用)
 * 
 * @param   index 要素番号
 * @return  指定された要素番号に該当する要素値の参照
 * @throw   指定された要素番号が不正な場合
 */
const tstring &asarray_tstring::at_cs( size_t index ) const
{
    if ( index < 0 || this->length() <= index ) {
        err_throw1(__FUNCTION__,"ERROR","invalid index: %d",(int)index);
    }
    return this->value_rec.at_cs(index);
}

/**
 * @brief  キー文字列の配列オブジェクトの参照を取得 (読取専用)
 *
 *  オブジェクト内で管理しているキー文字列の配列オブジェクトの参照(読み取り
 *  専用)を返します．
 * 
 * @return  キー文字列の配列オブジェクトの参照
 */
const tarray_tstring &asarray_tstring::keys() const
{
    return this->key_rec;
}

/**
 * @brief  値文字列の配列オブジェクトの参照を取得 (読取専用)
 *
 *  オブジェクト内で管理している値文字列の配列オブジェクトの参照(読み取り専用)
 *  を返します．
 * 
 * @return  値文字列の配列オブジェクトの参照
 */
const tarray_tstring &asarray_tstring::values() const
{
    return this->value_rec;
}

/**
 * @brief  標準エラー出力へsrcを出力 (内部用)
 * 
 * @param  src tstringオブジェクト
 * @note   private な関数です．
 */
static void dump_tstring( const tstring &src )
{
    if ( 0 <= src.strchr('"') ) {
	size_t j;
	sli__eprintf("\"");
	for ( j=0 ; j < src.length() ; j++ ) {
	    int ch = src.cchr(j);
	    if ( ch == '"' ) sli__eprintf("\\\"");
	    else sli__eprintf("%c", ch);
	}
	sli__eprintf("\"");
    }
    else {
	sli__eprintf("\"%s\"", src.cstr());
    }
    return;
}

/**
 * @brief  テンポラリオブジェクトのためのshallow copy属性の付与 (未実装)
 * @deprecated  未実装
 *
 */
/* 注意: 参照を返したいところだが，return 文でそれをやるとインスタンス */
/*       を2回コピーされるというおかしな事になるので void になっている */
void asarray_tstring::set_scopy_flag()
{
    return;
}

/**
 * @brief  オブジェクト情報を標準エラー出力へ出力
 *
 *  オブジェクト情報を標準エラー出力へ出力します．
 */
void asarray_tstring::dprint( const char *msg ) const
{
    size_t i;
    if ( msg != NULL ) {
	sli__eprintf("%s sli::%s[obj=0x%zx] = { ",
		     msg, CLASS_NAME, (const size_t)this);
    }
    else {
	sli__eprintf("sli::%s[obj=0x%zx] = { ",CLASS_NAME,(const size_t)this);
    }
    for ( i=0 ; i < this->length() ; i++ ) {
	if ( i != 0 ) sli__eprintf(", ");
	sli__eprintf("{");
	dump_tstring(this->key_rec.at_cs(i));
	sli__eprintf(", ");
	dump_tstring(this->value_rec.at_cs(i));
	sli__eprintf("}");
    }
    sli__eprintf(" }\n");
    return;
}

}	/* namespace sli */
