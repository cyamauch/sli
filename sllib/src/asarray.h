/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2013-05-14 17:23:23 cyamauch> */

#ifndef _SLI__ASARRAY_H
#define _SLI__ASARRAY_H 1

/**
 * @file   asarray.h
 * @brief  任意クラスのオブジェクト連想配列を扱う asarray クラスのコード
 */

#include "sli_config.h"
#include "tarray_tstring.h"
#include "tarray.h"
#include "ctindex.h"

namespace sli
{

/*
 * sli::asarray template class can handle the associative arrays of arbitrary
 * data types or classes.  Its function is almost the same as that of 
 * asarray_tstring, except that this class does not include member functions
 * specializing in string.
 */

/**
 * @class  sli::asarray
 * @brief  任意クラスのオブジェクト連想配列を扱うためのテンプレートクラス
 *
 *   asarray テンプレートクラスは，任意クラスのオブジェクト連想配列を扱う事が
 *   できます．
 *
 * @note  asarray オブジェクト内部では，値としてのオブジェクトは単純な配列とし
 *        て保存されます．同時に，キー文字列と配列番号との関係も保存され，それ
 *        には ctindex クラスを用いています．ctindex クラスはキー文字列と配列番
 *        号との関係を，「木」構造で記録し，高速な検索(キー文字列から配列番号へ
 *        の変換)を可能にしています．ただし，「木」構造ですので，更新にはやや時
 *        間がかかります．
 * @author Chisato YAMAUCHI
 * @date 2013-04-01 00:00:00
 */

  template <class datatype> class asarray
  {

  public:
    /* constructor and destructor */
    asarray();
    asarray(const asarray &obj);
    virtual ~asarray();

    /*
     * operator
     */
    /* same as .init(sobj) */
    virtual asarray &operator=(const asarray &obj);

    /* same as .at(key) */
    virtual datatype &operator[]( const char *key );
    virtual const datatype &operator[]( const char *key ) const;

    /*
     * member functions
     */
    /* complete initialization of objects */
    virtual asarray &init();
    virtual asarray &init(const asarray &obj);

    /* padding of the total existing values in an a ssociated array */
    virtual asarray &clean(const datatype &one);

    /* set an associative array */
    virtual asarray &assign( const asarray &src );
    virtual asarray &assign( const char *key, const datatype &val );

    /* assign multiple strings or arrays of strings to the key */
    virtual asarray &assign_keys( const char *key0, ... );
    virtual asarray &vassign_keys( const char *key0, va_list ap );
    virtual asarray &assign_keys( const char *const *keys );
    virtual asarray &assign_keys( const tarray_tstring &keys );

    /* split a string and assign to the key */
    virtual asarray &split_keys( const char *src_str, const char *delims, 
				 bool zero_str, const char *quotations,
				 int escape, bool rm_escape );
    virtual asarray &split_keys( const char *src_str, const char *delims,
				 bool zero_str = false );
    virtual asarray &split_keys( const tstring &src_str, const char *delims, 
				 bool zero_str, const char *quotations,
				 int escape, bool rm_escape );
    virtual asarray &split_keys( const tstring &src_str, const char *delims,
				 bool zero_str = false );

    /* append an element */
    virtual asarray &append( const asarray &src );
    virtual asarray &append( const char *key, const datatype &val );

    /* insert an element */
    virtual asarray &insert( const char *key, const asarray &src );
    virtual asarray &insert( const char *key,
			     const char *newkey, const datatype &newval );

    /* erase element(s) */
    virtual asarray &erase();
    virtual asarray &erase( const char *key, size_t num_elements = 1 );

    /* swap self and another */
    virtual asarray &swap( asarray &sobj );

    /* change a key string */
    virtual asarray &rename_a_key( const char *org_key, const char *new_key );

    /* a reference to the element value corresponding to specified key or */
    /* element number                                                     */
    virtual datatype &at( const char *key );
    virtual datatype &atf( const char *fmt, ... );
    virtual datatype &vatf( const char *fmt, va_list ap );
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const datatype &at( const char *key ) const;
    virtual const datatype &atf( const char *fmt, ... ) const;
    virtual const datatype &vatf( const char *fmt, va_list ap ) const;
#endif
    virtual const datatype &at_cs( const char *key ) const;
    virtual const datatype &atf_cs( const char *fmt, ... ) const;
    virtual const datatype &vatf_cs( const char *fmt, va_list ap ) const;

    /* length of an associated array */
    virtual size_t length() const;
    /* not recommended */
    virtual size_t size() const;

    /* acquire the element number corresponding to the key string */
    virtual ssize_t index( const char *key ) const;
    virtual ssize_t indexf( const char *fmt, ... ) const;
    virtual ssize_t vindexf( const char *fmt, va_list ap ) const;

    /* acquire the key string corresponding to the element number */
    virtual const char *key( size_t index ) const;

    /* a reference to the element value corresponding to specified key or */
    /* element number                                                     */
    virtual datatype &at( size_t index );
    virtual const datatype &at_cs( size_t index ) const;

    /* a reference to the array object of the key string (read only) */
    virtual const tarray_tstring &keys() const;

    /* a reference to the array object of the value (read only) */
    virtual const tarray<datatype> &values() const;

    /* register a function to create object of datatype class */
    virtual void register_creator( datatype *(*func)(void *), void *user_ptr );

    /* 一時オブジェクトの return の直前に使い，shallow copy を許可する場合に */
    /* 使う (未実装)                                                         */
    virtual void set_scopy_flag();

#if 0	/* こういう使い方はできないらしい…orz */
    virtual asarray &assign( const char *key0, const datatype &val0,
			     const char *key1, ... );
    virtual asarray &vassign( const char *key0, const datatype &val0,
			      const char *key1, va_list ap );
    virtual asarray &append( const char *key0, const datatype &val0, 
			     const char *key1, ... );
    virtual asarray &vappend( const char *key0, const datatype &val0, 
			      const char *key1, va_list ap );
    virtual asarray &insert( const char *key, 
			     const char *key0, const datatype &val0, 
			     const char *key1, ... );
    virtual asarray &vinsert( const char *key,
			      const char *key0, const datatype &val0, 
			      const char *key1, va_list ap );
#endif

  private:
    tarray_tstring key_rec;
    tarray<datatype> value_rec;
    ctindex index_rec;

    /* for error handling */
    void err_throw1( const char *func0, const char *level0, 
		     const char *message0, const char *arg ) const;
    void err_report1( const char *func0, const char *level0, 
		      const char *message0, const char *arg ) const;

  };
}

namespace sli
{

/* constructor */
/**
 * @brief  コンストラクタ
 */
template <class datatype>
asarray<datatype>::asarray()
{
    return;
}

/* copy constructor */

/**
 * @brief  コピーコンストラクタ
 *
 *  自身をobjの内容で初期化します
 * 
 * @param  obj asarrayクラスのオブジェクト
 * @throw  内部バッファの確保に失敗した場合
 * @throw  メモリ破壊を起こしたとき
 */
template <class datatype>
asarray<datatype>::asarray(const asarray<datatype> &obj)
{
    this->init(obj);
    return;
}

/* destructor */

/**
 * @brief  デストラクタ
 *
 */
template <class datatype>
asarray<datatype>::~asarray()
{
    return;
}

template <class datatype>
/**
 * @brief  オブジェクトのコピー
 *
 *  演算子の右側(引数) で指定された同じ型・クラスを扱うオブジェクトを自身に
 *  コピーします．
 * 
 * @param   obj asarrayクラスのオブジェクト
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 * @throw   メモリ破壊を起こした場合
 */
asarray<datatype> &asarray<datatype>::operator=(const asarray<datatype> &obj)
{
    this->init(obj);
    return *this;
}

template <class datatype>
/**
 * @brief  [] で指定された要素値の参照を返す
 *
 *  指定されたキーに対応する連想配列の要素値の参照を返します．
 * 
 * @param   key 連想配列のキー文字列
 * @return  キーに対応する連想配列の要素値の参照
 * @throw   指定されたキー文字列がNULLの場合
 * @throw   内部バッファの確保に失敗した場合
 */
inline datatype &asarray<datatype>::operator[]( const char *key )
{
    return this->at(key);
}

template <class datatype>
/**
 * @brief  [] で指定された要素値の参照を返す (読取専用)
 *
 *  指定されたキーに対応する連想配列の要素値の参照を返します．
 * 
 * @param   key 連想配列のキー文字列
 * @return  キーに対応する連想配列の要素値の参照
 * @throw   指定されたキー文字列がNULLの場合
 * @throw   存在しないキー文字列が指定された場合
 */
inline const datatype &asarray<datatype>::operator[]( const char *key ) const
{
    return this->at_cs(key);
}

/*
 * public member functions
 */

template <class datatype>
/**
 * @brief  オブジェクトの初期化
 *
 *  自身の連想配列を消去し，オブジェクトの初期化を行います．
 * 
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
asarray<datatype> &asarray<datatype>::init()
{
    this->index_rec.init();
    this->key_rec.init();
    this->value_rec.init();
    return *this;
}

template <class datatype>
/**
 * @brief  オブジェクトのコピー
 * 
 *  指定されたオブジェクト obj の内容を自身にコピーします．
 * 
 * @param   obj asarrayクラスのオブジェクト
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 * @throw   メモリ破壊を起こした場合
 */
asarray<datatype> &asarray<datatype>::init(const asarray<datatype> &obj)
{
    if ( this == &obj ) return *this;
    this->value_rec.init(obj.value_rec);
    this->key_rec.init(obj.key_rec);
    this->index_rec.init(obj.index_rec);
    return *this;
}

template <class datatype>
/**
 * @brief  既存の要素値すべてを指定された値でパディング
 *
 *  自身の連想配列の要素値全てを任意の値でパディングします．
 * 
 * @param   one 連想配列の全要素をパディングする値
 * @return  自身の参照
 * @throw   要素のバッファの確保に失敗した場合
 */
asarray<datatype> &asarray<datatype>::clean(const datatype &one)
{
    this->value_rec.clean(one);
    return *this;
}

template <class datatype>
/**
 * @brief  指定された連想配列を自身に代入
 *
 *  srcで指定された連想配列の全部を，自身に代入します．
 * 
 * @param   src asarrayクラスのオブジェクト
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
asarray<datatype> &asarray<datatype>::assign(const asarray<datatype> &src)
{
    if ( this == &src ) return *this;
    this->erase();
    return this->append(src);
}

template <class datatype>
/**
 * @brief  指定された要素(key,value) 1個のみの連想配列とする
 *
 *  自身を，指定された 1つの要素だけを持つ連想配列にします．
 * 
 * @param   key 連想配列に設定するキー文字列
 * @param   val 連想配列に設定する値
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
asarray<datatype> &asarray<datatype>::assign( const char *key, 
					      const datatype &val )
{
    /* 自身が管理している key, val が来てもいいようにする */
    if ( key != NULL ) {
	size_t org_len = this->length();
	/* まず，最後に追加する */
	this->value_rec.append(val,1);
	this->key_rec.append(key,1);
	/* 元のやつを消去する */
	this->value_rec.erase(0, org_len);
	this->key_rec.erase(0, org_len);
	/* */
	this->index_rec.init();
	this->index_rec.append(this->key_rec.cstr(0), 0);
    }
    else {
	this->erase();
    }
    return *this;
}

template <class datatype>
/**
 * @brief  指定された複数の文字列を，連想配列のキーとして自身に設定
 *
 *  指定された複数の文字列を自身の連想配列のキーに設定します．<br>
 *  可変引数の終端は NULL でなければなりません．
 * 
 * @param   key0 キー文字列
 * @param   ... キー文字列の可変長引数のリスト (NULL終端)
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
asarray<datatype> &asarray<datatype>::assign_keys( const char *key0, ... )
{
    va_list ap;
    va_start(ap,key0);
    try {
	this->vassign_keys(key0,ap);
    }
    catch (...) {
	va_end(ap);
        this->err_throw1(__FUNCTION__,"FATAL","%s","this->vassign_keys() failed");
    }
    va_end(ap);
    return *this;
}

template <class datatype>
/**
 * @brief  指定された複数の文字列を，連想配列のキーとして自身に設定
 *
 *  指定された複数の文字列を自身の連想配列のキーに設定します．<br>
 *  可変引数の終端は NULL でなければなりません．
 * 
 * @param   key0 キー文字列
 * @param   ap キー文字列の可変長引数のリスト (NULL終端)
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
asarray<datatype> &asarray<datatype>::vassign_keys( const char *key0, va_list ap )
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

template <class datatype>
/**
 * @brief  指定された文字列配列を，連想配列のキーとして自身に設定
 *
 *  指定された複数の文字列配列 keys を自身の連想配列のキーに設定します．<br>
 *  配列の終端は NULL でなければなりません．
 * 
 * @param   keys キー文字列に設定する文字列配列 (NULL終端)
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
asarray<datatype> &asarray<datatype>::assign_keys( const char *const *keys )
{
    tarray_tstring tmp_keys;
    if ( this->key_rec.cstrarray() == keys ) return *this;
    tmp_keys.assign(keys);
    this->assign_keys(tmp_keys);
    return *this;
}

template <class datatype>
/**
 * @brief  指定された文字列配列を，連想配列のキーとして自身に設定
 *
 *  指定された複数の文字列配列 keys を自身の連想配列のキーに設定します．<br>
 * 
 * @param   keys キー文字列に設定する文字列配列
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
asarray<datatype> &asarray<datatype>::assign_keys( const tarray_tstring &keys )
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

template <class datatype>
/**
 * @brief  文字列を分割して，キーに設定 (多機能版)
 *
 *  文字列を分割して,キーに設定します．文字列の分割後に得られたキーの個数を
 *  超える連想配列要素は削除されます. <br>
 *  delims には，" \t" のような単純な文字リストに加え，正規表現で用いられる
 *  "[A-Z]" あるいは "[^A-Z]" のような指定が可能です．<br>
 *  クォーテーションや括弧の「特定の文字」で囲まれた文字列は分割しない場合，
 *  quotations に「特定の文字」を指定します．
 * 
 * @param   src_str 分割対象の文字列
 * @param   delims 区切り文字を含む文字列
 * @param   zero_str 長さ0の区切り結果の文字列を許すかどうかのフラグ
 * @param   quotations クォーテーション・ブラケット文字を含む文字列
 * @param   escape エスケープ文字
 * @param   rm_escape エスケープ文字を削除するかどうかのフラグ
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
asarray<datatype> &asarray<datatype>::split_keys( const char *src_str, 
					 const char *delims, 
					 bool zero_str, const char *quotations,
					 int escape, bool rm_escape )
{
    tarray_tstring tmp_keys;

    tmp_keys.split(src_str,delims,zero_str,quotations,escape,rm_escape);
    this->assign_keys(tmp_keys);

    return *this;
}

template <class datatype>
/**
 * @brief  文字列を分割して，キーに設定
 *
 *  文字列を分割して,キーに設定します．文字列の分割後に得られたキーの個数を
 *  超える連想配列要素は削除されます. <br>
 *  delims には，" \t"のような単純な文字リストに加え，正規表現で用いられる
 *  "[A-Z]" あるいは "[^A-Z]" のような指定が可能です．
 * 
 * @param   src_str 分割対象の文字列
 * @param   delims 区切り文字を含む文字列
 * @param   zero_str 長さ0の区切り結果の文字列を許すかどうかのフラグ
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
asarray<datatype> &asarray<datatype>::split_keys( const char *src_str, 
						  const char *delims,
						  bool zero_str )
{
    return this->split_keys(src_str,delims,zero_str,NULL,'\0',false);
}

template <class datatype>
/**
 * @brief  文字列を分割して，キーに設定 (多機能版)
 *
 *  文字列を分割して，キーに設定します．文字列の分割後に得られたキーの個数を
 *  超える連想配列要素は削除されます．<br>
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
asarray<datatype> &asarray<datatype>::split_keys( const tstring &src_str, 
					 const char *delims, 
					 bool zero_str, const char *quotations,
					 int escape, bool rm_escape )
{
    tarray_tstring tmp_keys;

    tmp_keys.split(src_str,delims,zero_str,quotations,escape,rm_escape);
    this->assign_keys(tmp_keys);

    return *this;
}

template <class datatype>
/**
 * @brief  文字列を分割して，キーに設定
 *
 *  文字列を分割して，キーに設定します．文字列の分割後に得られたキーの個数を
 *  超える連想配列要素は削除されます. <br>
 *  delims には，" \t"のような単純な文字リストに加え，正規表現で用いられる
 *  "[A-Z]" あるいは "[^A-Z]" のような指定が可能です．
 * 
 * @param   src_str 分割対象の文字列
 * @param   delims 区切り文字を含む文字列
 * @param   zero_str 長さ0の区切り結果の文字列を許すかどうかのフラグ
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
asarray<datatype> &asarray<datatype>::split_keys( const tstring &src_str, 
						  const char *delims,
						  bool zero_str )
{
    return this->split_keys(src_str,delims,zero_str,NULL,'\0',false);
}

template <class datatype>
/**
 * @brief  指定された連想配列を，自身の連想配列に追加
 *
 *  自身の連想配列に，指定された複数の要素を追加します．<br>
 *  キーが重複した場合，実行時に警告が出力され，処理が行われません．
 * 
 * @param   src 源泉となる要素を持つasararyクラスのオブジェクト
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
asarray<datatype> &asarray<datatype>::append( const asarray<datatype> &src )
{
    size_t i;
    if ( this == &src ) {
	/* キーが重複するので自身を追加することはできない */
	this->err_report1(__FUNCTION__,"WARNING", 
			  "%s", "cannot append self-object");
	goto quit;
    }
    for ( i=0 ; i < src.length() ; i++ ) {
	this->append(src.key(i),src.at_cs(i));
    }
 quit:
    return *this;
}

template <class datatype>
/**
 * @brief  指定された1要素を，自身の連想配列に追加
 *
 *  自身の連想配列に，指定された 1つの要素を追加します．<br>
 *  キーが重複した場合，実行時に警告が出力され，処理が行われません．
 * 
 * @param   key 連想配列に追加するキー文字列
 * @param   val 連想配列に追加する値
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
asarray<datatype> &asarray<datatype>::append( const char *key, 
					      const datatype &val )
{
    if ( key != NULL ) {
	if ( this->index_rec.index(key,0) < 0 ) {
	    this->value_rec.append(val,1);
	    this->key_rec.append(key,1);
	    this->index_rec.append(key,this->key_rec.length()-1);
	}
	else {
	    this->err_report1(__FUNCTION__,"WARNING",
			      "cannot append key '%s'",key);
	}
    }
    return *this;
}

template <class datatype>
/**
 * @brief  指定された連想配列を，自身の連想配列に挿入
 *
 *  自身の連想配列のキー key の要素位置の前に，指定された複数の要素を挿入
 *  します．<br>
 *  キーが重複した場合，実行時に警告が出力され，処理が行われません．
 * 
 * @param   key 挿入位置にある自身の連想配列のキー文字列
 * @param   src 源泉となる要素を持つasararyクラスのオブジェクト
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
asarray<datatype> &asarray<datatype>::insert( const char *key,
					      const asarray<datatype> &src )
{
    size_t i;
    if ( this == &src ) {
	/* キーが重複するので自身をインサートすることはできない */
	this->err_report1(__FUNCTION__,"WARNING", 
			  "%s", "cannot insert self-object");
	goto quit;
    }
    for ( i=0 ; i < src.length() ; i++ ) {
	this->insert(key,src.key(i),src.at_cs(i));
    }
 quit:
    return *this;
}

template <class datatype>
/**
 * @brief  指定された1要素を，自身の連想配列に挿入
 *
 *  自身の連想配列のキー key の要素位置の前に，指定された 1つの要素を挿入
 *  します．<br>
 *  キーが重複した場合，実行時に警告が出力され，処理が行われません．
 * 
 * @param   key 挿入位置にある自身の連想配列のキー文字列
 * @param   newkey 連想配列に挿入するキー文字列
 * @param   newval 連想配列に挿入する値
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
asarray<datatype> &asarray<datatype>::insert( const char *key,
				   const char *newkey, const datatype &newval )
{
    if ( key != NULL && newkey != NULL ) {
	size_t i;
	ssize_t idx = this->index_rec.index(key,0);
	if ( idx < 0 ) {
	    this->err_report1(__FUNCTION__,"WARNING","unknown key '%s'",key);
	    goto quit;
	}
	if ( 0 <= this->index_rec.index(newkey,0) ) {
	    this->err_report1(__FUNCTION__,"WARNING",
			      "cannot insert key '%s'",newkey);
	    goto quit;
	}
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

template <class datatype>
/**
 * @brief  全配列要素の削除
 *
 *  自身の連想配列の全ての要素を消去します．
 * 
 * @return  自身の参照
 * @hrow    内部バッファの確保に失敗した場合
 */
asarray<datatype> &asarray<datatype>::erase()
{
    this->index_rec.init();
    this->key_rec.erase();
    this->value_rec.erase();
    return *this;
}

template <class datatype>
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
asarray<datatype> &asarray<datatype>::erase( const char *key, 
					     size_t num_elements )
{
    size_t i, maxels;
    ssize_t idx;
    if ( key == NULL ) goto quit;
    idx = this->index_rec.index(key,0);
    if ( idx < 0 ) {
	this->err_report1(__FUNCTION__,"WARNING","unknown key '%s'",key);
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

template <class datatype>
/**
 * @brief  2つのオブジェクト間での内容のスワップ
 *
 *  オブジェクトsobj の内容と自身の内容とを入れ替えます．
 * 
 * @param   sobj 内容を入れ替えるasarrayオブジェクト
 * @return  自身の参照
 */
asarray<datatype> &asarray<datatype>::swap( asarray<datatype> &sobj )
{
    if ( this == &sobj ) return *this;
    this->key_rec.swap( sobj.key_rec );
    this->value_rec.swap( sobj.value_rec );
    this->index_rec.swap( sobj.index_rec );
    return *this;
}

template <class datatype>
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
asarray<datatype> &asarray<datatype>::rename_a_key( const char *org_key, 
						    const char *new_key )
{
    ssize_t idx;
    idx = this->index_rec.index(org_key,0);
    if ( idx < 0 ) {
	this->err_report1(__FUNCTION__,"WARNING","unknown key '%s'",org_key);
	goto quit;
    }
    if ( new_key == NULL || this->key_rec.at(idx).compare(new_key) == 0 )
	goto quit;
    if ( 0 <= this->index_rec.index(new_key,0) ) {
	this->err_report1(__FUNCTION__,"WARNING",
			  "already used: key '%s'. Not renamed.",new_key);
	goto quit;
    }
    this->index_rec.erase(org_key, idx);
    this->index_rec.append(new_key, idx);
    this->key_rec.at(idx).assign(new_key);
 quit:
    return *this;
}

template <class datatype>
/**
 * @brief  指定されたキー文字列に対応する配列要素の参照を返す
 * 
 * @param   key 連想配列のキー文字列
 * @return  指定されたキーに該当する要素値の参照
 * @throw   指定されたキー文字列がNULLの場合
 * @throw   内部バッファの確保に失敗した場合
 */
inline datatype &asarray<datatype>::at( const char *key )
{
    ssize_t idx;
    if ( key == NULL ) {
        this->err_throw1(__FUNCTION__,"ERROR","NULL key...?","");
    }
    idx = this->index_rec.index(key,0);
    if ( idx < 0 ) {
	this->append(key,this->value_rec.initial_one());
	idx = this->index_rec.index(key,0);
    }
    if ( idx < 0 ) {
        this->err_throw1(__FUNCTION__,"ERROR","not found key: %s",key);
    }
    return this->value_rec.at(idx);
}

template <class datatype>
/**
 * @brief  キー文字列に対応する配列要素の参照を返す (printf()の記法)
 *
 *  引数のキー文字列に対応する連想配列の要素値の参照を返します．<br>
 *  このメンバ関数では指定したいキー文字列を printf() 関数と同様のフォーマット
 *  と可変引数でセットできます.
 * 
 * @param   fmt キー文字列のためのフォーマット指定
 * @param   ... fmtに対応した可変長引数の各要素データ
 * @return  指定されたキーに該当する要素値の参照
 * @throw   指定されたキー文字列がNULLの場合
 * @throw   内部バッファの確保に失敗した場合
 */
datatype &asarray<datatype>::atf( const char *fmt, ... )
{
    datatype *ret_p = NULL;
    va_list ap;
    va_start(ap,fmt);
    try {
	datatype &ret = this->vatf(fmt,ap);
	va_end(ap);
	ret_p = &ret;
    }
    catch (...) {
	va_end(ap);
        this->err_throw1(__FUNCTION__,"FATAL","this->vatf() failed","");
    }
    return *(ret_p);
}

template <class datatype>
/**
 * @brief  キー文字列に対応する配列要素の参照を返す (printf()の記法)
 *
 *  引数のキー文字列に対応する連想配列の要素値の参照を返します.<br>
 *  このメンバ関数では指定したいキー文字列を printf() 関数と同様のフォーマット
 *  と可変引数でセットできます.
 * 
 * @param   fmt キー文字列のためのフォーマット指定
 * @param   ap fmtに対応した可変長引数のリスト
 * @return  指定されたキーに該当する要素値の参照
 * @throw   指定されたキー文字列がNULLの場合
 * @throw   内部バッファの確保に失敗した場合
 */
datatype &asarray<datatype>::vatf( const char *fmt, va_list ap )
{
    tstring buf;

    if ( fmt == NULL ) return this->at(fmt);

    buf.vprintf(fmt,ap);
    return this->at(buf.cstr());
}

#ifdef SLI__OVERLOAD_CONST_AT
template <class datatype>
/**
 * @brief  キー文字列に対応する配列要素の参照を返す (読取専用)
 *
 * @param   key 連想配列のキー文字列
 * @return  指定されたキーに該当する要素値の参照
 * @throw   指定されたキー文字列がNULLの場合
 */
inline const datatype &asarray<datatype>::at( const char *key ) const
{
    return this->at_cs(key);
}

template <class datatype>
/**
 * @brief  キー文字列に対応する配列要素の参照を返す (読取専用・printf()の記法)
 *
 *  引数のキー文字列に対応する要素値の参照(読み取り専用)を返します．<br>
 *  このメンバ関数は指定したいキー文字列を printf() 関数と同様のフォーマットと
 *  可変引数でセットできます．
 * 
 * @param   fmt キー文字列のためのフォーマット指定
 * @param   ... fmtに対応した可変長引数の各要素データ
 * @return  指定されたキーに該当する要素値の参照
 * @throw   指定されたキー文字列がNULLの場合
 * @throw   内部バッファの確保に失敗した場合
 */
const datatype &asarray<datatype>::atf( const char *fmt, ... ) const
{
    const datatype *ret_p = NULL;
    va_list ap;
    va_start(ap,fmt);
    try {
	const datatype &ret = this->vatf_cs(fmt,ap);
	va_end(ap);
	ret_p = &ret;
    }
    catch (...) {
	va_end(ap);
        this->err_throw1(__FUNCTION__,"FATAL","this->vatf_cs() failed","");
    }
    return *(ret_p);
}
template <class datatype>
/**
 * @brief  キー文字列に対応する配列要素の参照を返す (読取専用・printf()の記法)
 *
 *  引数のキー文字列に対応する要素値の参照(読み取り専用)を返します．<br>
 *  このメンバ関数は指定したいキー文字列を printf() 関数と同様のフォーマットと
 *  可変引数でセットできます．
 * 
 * @param   fmt キー文字列のためのフォーマット指定
 * @param   ap fmtに対応した可変長引数のリスト
 * @return  指定されたキーに該当する要素値の参照
 * @throw   指定されたキー文字列がNULLの場合
 * @throw   内部バッファの確保に失敗した場合
 */
const datatype &asarray<datatype>::vatf( const char *fmt, va_list ap ) const
{
    return this->vatf_cs(fmt, ap);
}
#endif

template <class datatype>
/**
 * @brief  キー文字列に対応する配列要素の参照を返す (読取専用)
 * 
 * @param   key 連想配列のキー文字列
 * @return  指定されたキーに該当する要素値の参照
 * @throw   存在しないキー文字列が指定された場合
 * @throw   指定されたキー文字列がNULLの場合
 */
inline const datatype &asarray<datatype>::at_cs( const char *key ) const
{
    ssize_t idx;
    if ( key == NULL ) {
        this->err_throw1(__FUNCTION__,"ERROR","NULL key...?","");
    }
    idx = this->index_rec.index(key,0);
    if ( idx < 0 ) {
        this->err_throw1(__FUNCTION__,"ERROR","not found key '%s'",key);
    }
    return this->value_rec.at_cs(idx);
}

template <class datatype>
/**
 * @brief  キー文字列に対応する配列要素の参照を返す (読取専用・printf()の記法)
 *
 *  引数のキー文字列に対応する要素値の参照(読み取り専用)を返します．<br>
 *  このメンバ関数は指定したいキー文字列を printf() 関数と同様のフォーマットと
 *  可変引数でセットできます．
 * 
 * @param   fmt キー文字列のためのフォーマット指定
 * @param   ... fmtに対応した可変長引数の各要素データ
 * @return  指定されたキーに該当する要素値の参照
 * @throw   存在しないキー文字列が指定された場合
 * @throw   指定されたキー文字列がNULLの場合
 * @throw   内部バッファの確保に失敗した場合
 */
const datatype &asarray<datatype>::atf_cs( const char *fmt, ... ) const
{
    const datatype *ret_p = NULL;
    va_list ap;
    va_start(ap,fmt);
    try {
	const datatype &ret = this->vatf_cs(fmt,ap);
	va_end(ap);
	ret_p = &ret;
    }
    catch (...) {
	va_end(ap);
        this->err_throw1(__FUNCTION__,"FATAL","this->vatf_cs() failed","");
    }
    return *(ret_p);
}

template <class datatype>
/**
 * @brief  キー文字列に対応する配列要素の参照を返す (読取専用・printf()の記法)
 *
 *  引数のキー文字列に対応する要素値の参照(読み取り専用)を返します．<br>
 *  このメンバ関数は指定したいキー文字列を printf() 関数と同様のフォーマットと
 *  可変引数でセットできます．
 * 
 * @param   fmt キー文字列のためのフォーマット指定
 * @param   ap fmtに対応した可変長引数のリスト
 * @return  指定されたキーに該当する要素値の参照
 * @throw   存在しないキー文字列が指定された場合
 * @throw   指定されたキー文字列がNULLの場合
 * @throw   内部バッファの確保に失敗した場合
 */
const datatype &asarray<datatype>::vatf_cs( const char *fmt, va_list ap ) const
{
    tstring buf;

    if ( fmt == NULL ) return this->at_cs(fmt);

    buf.vprintf(fmt,ap);
    return this->at_cs(buf.cstr());
}

template <class datatype>
/**
 * @brief  キー文字列の配列オブジェクトの参照を取得 (読取専用)
 *
 *  オブジェクト内で管理しているキー文字列の配列オブジェクトの参照(読み取り
 *  専用)を返します．
 * 
 * @return  キー文字列の配列オブジェクト
 */
inline const tarray_tstring &asarray<datatype>::keys() const
{
    return this->key_rec;
}

template <class datatype>
/**
 * @brief  値の配列オブジェクトの参照を取得 (読取専用)
 *
 *  オブジェクト内で管理している値の配列オブジェクトの参照(読み取り専用)
 *  を返します．
 * 
 * @return  値の配列オブジェクトの参照
 */
inline const tarray<datatype> &asarray<datatype>::values() const
{
    return this->value_rec;
}

template <class datatype>
/**
 * @brief  連想配列の長さ(個数)を取得
 * 
 * @return  自身の連想配列の個数
 */
inline size_t asarray<datatype>::length() const
{
    return this->key_rec.length();
}

template <class datatype>
/**
 * @brief  連想配列の長さ(個数)を取得
 * 
 * @return  自身の連想配列の個数
 * @note    asarray<datatype>::length() との違いはありません．
 */
inline size_t asarray<datatype>::size() const
{
    return this->key_rec.size();
}

template <class datatype>
/**
 * @brief  指定されたキー文字列に対応する要素番号を返す
 * 
 * @param   key キー文字列
 * @return  非負の値: 指定されたキー文字列が見つかった場合．<br>
 *          負の値: 見つからなかった場合．
 */
ssize_t asarray<datatype>::index( const char *key ) const
{
    if ( key == NULL ) return -1;
    return this->index_rec.index(key,0);
}

template <class datatype>
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
ssize_t asarray<datatype>::indexf( const char *fmt, ... ) const
{
    ssize_t ret = -1;
    va_list ap;
    va_start(ap,fmt);
    try {
	ret = this->vindexf(fmt,ap);
    }
    catch (...) {
	va_end(ap);
        this->err_throw1(__FUNCTION__,"FATAL","this->vindexf() failed","");
    }
    va_end(ap);
    return ret;
}

template <class datatype>
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
ssize_t asarray<datatype>::vindexf( const char *fmt, va_list ap ) const
{
    tstring buf;

    if ( fmt == NULL ) return this->index(fmt);

    buf.vprintf(fmt,ap);
    return this->index(buf.cstr());
}

template <class datatype>
/**
 * @brief  指定された要素番号に該当するキー文字列を返す
 * 
 *  指定された要素番号に該当するキー文字列のアドレスを返します．
 * 
 * @param   index 要素番号
 * @return  キー文字列の内部バッファのアドレス．<br>
 *          配列の長さ以上の要素番号が指定された場合はNULL
 */
const char *asarray<datatype>::key( size_t index ) const
{
    if ( index < 0 || this->length() <= index ) return NULL;
    else return this->key_rec.cstr(index);
}

template <class datatype>
/**
 * @brief  指定された要素番号に該当する要素値の参照を返す
 * 
 * @param   index 要素番号
 * @return  指定された要素番号に該当する要素値の参照
 * @throw   指定された要素番号が不正な場合
 */
inline datatype &asarray<datatype>::at( size_t index )
{
    static tstring buf(32);
    if ( index < 0 || this->length() <= index ) {
	buf.printf("%d",(int)index);
        this->err_throw1(__FUNCTION__,"ERROR","invalid index: %s",buf.cstr());
    }
    return this->value_rec.at(index);
}


template <class datatype>
/**
 * @brief  指定された要素番号に該当する要素値の参照(tstring)を返す (読取専用)
 * 
 * @param   index 要素番号
 * @return  指定された要素番号に該当する要素値の参照
 * @throw   指定された要素番号が不正な場合
 */
inline const datatype &asarray<datatype>::at_cs( size_t index ) const
{
    static tstring buf(32);
    if ( index < 0 || this->length() <= index ) {
	buf.printf("%d",(int)index);
        this->err_throw1(__FUNCTION__,"ERROR","invalid index: %s",buf.cstr());
    }
    return this->value_rec.at_cs(index);
}

/* new datatype する時に，コンストラクタに引数を与えたい場合に使う */

/**
 * @brief  datatypeクラスのオブジェクトを作る関数の登録
 *
 *  asarray クラス内部で「new datatype」する時に，コンストラクタに引数を
 *  与えたい場合に使います.
 *
 * @param     func ユーザ関数のアドレス
 * @param     user_ptr ユーザ関数の最後に与えられるユーザのポインタ
 * @throw     内部バッファの確保に失敗した場合
 */
template <class datatype>
void asarray<datatype>::register_creator( datatype *(*func)(void *), 
					  void *user_ptr )
{
    this->value_rec.register_creator(func, user_ptr);
    return;
}

/**
 * @brief  テンポラリオブジェクトのためのshallow copy属性の付与 (未実装)
 * @deprecated  未実装
 */
/* 注意: 参照を返したいところだが，return 文でそれをやるとインスタンス */
/*       を2回コピーされるというおかしな事になるので void になっている */
template <class datatype>
void asarray<datatype>::set_scopy_flag()
{
    return;
}


/* うーん，こういう使い方はできないらしい…orz */
#if 0
template <class datatype>
asarray<datatype> &asarray<datatype>::assign( const char *key0, 
					      const datatype &val0,
					      const char *key1, ... )
{
    va_list ap;
    va_start(ap,key1);
    try {
	this->vassign(key0,val0,key1,ap);
    }
    catch (...) {
	va_end(ap);
        this->err_throw1(__FUNCTION__,"FATAL","this->vassign() failed","");
    }
    va_end(ap);
    return *this;
}
template <class datatype>
asarray<datatype> &asarray<datatype>::vassign( const char *key0, 
					       const datatype &val0,
					       const char *key1, va_list ap )
{
    this->assign(key0,val0);
    if ( key0 != NULL && key1 != NULL ) {
	const datatype &val1 = va_arg(ap,const datatype &);
	this->append(key1,val1);
	while ( 1 ) {
	    const char *keyx = va_arg(ap,const char *);
	    if ( keyx == NULL ) break;
	    const datatype &valx = va_arg(ap,const datatype &);
	    this->append(keyx,valx);
	}
    }
    return *this;
}
template <class datatype>
asarray<datatype> &asarray<datatype>::append( const char *key0, 
					      const datatype &val0,
					      const char *key1, ... )
{
    va_list ap;
    va_start(ap,key1);
    try {
	this->vappend(key0,val0,key1,ap);
    }
    catch (...) {
	va_end(ap);
        this->err_throw1(__FUNCTION__,"FATAL","this->vappend() failed","");
    }
    va_end(ap);
    return *this;
}
template <class datatype>
asarray<datatype> &asarray<datatype>::vappend( const char *key0, 
					       const datatype &val0,
					       const char *key1, va_list ap )
{
    this->append(key0,val0);
    if ( key0 != NULL && key1 != NULL ) {
	const datatype &val1 = va_arg(ap,const datatype &);
	this->append(key1,val1);
	while ( 1 ) {
	    const char *keyx = va_arg(ap,char *);
	    if ( keyx == NULL ) break;
	    const datatype &valx = va_arg(ap,const datatype &);
	    this->append(keyx,valx);
	}
    }
    return *this;
}
template <class datatype>
asarray<datatype> &asarray<datatype>::insert( const char *key,
					const char *key0, const datatype &val0,
					const char *key1, ... )
{
    va_list ap;
    va_start(ap,key1);
    try {
	this->vinsert(key,key0,val0,key1,ap);
    }
    catch (...) {
	va_end(ap);
        this->err_throw1(__FUNCTION__,"FATAL","this->vinsert() failed","");
    }
    va_end(ap);
    return *this;
}
template <class datatype>
asarray<datatype> &asarray<datatype>::vinsert( const char *key,
					const char *key0, const datatype &val0,
					const char *key1, va_list ap )
{
    if ( key == NULL ) goto quit;
    if ( this->index_rec.index(key,0) < 0 ) {
	this->err_report1(__FUNCTION__,"WARNING","unknown key '%s'",key);
	goto quit;
    }
    this->insert(key,key0,val0);
    if ( key0 != NULL && key1 != NULL ) {
	const datatype &val1 = va_arg(ap,const datatype &);
	this->insert(key,key1,val1);
	while ( 1 ) {
	    const char *keyx = va_arg(ap,char *);
	    if ( keyx == NULL ) break;
	    const datatype &valx = va_arg(ap,const datatype &);
	    this->insert(key,keyx,valx);
	}
    }
 quit:
    return *this;
}
#endif


/* private member functions */

/**
 * @brief  エラー，例外発生時に err_rec を throw する
 *
 *  asarrayクラスの関数内でエラー，例外発生時に呼び出されます．
 *  標準エラー出力にエラー情報を表示し，err_rec を throwします．
 *
 * @param	func0 呼び出し元関数名
 * @param	level0 エラー種別<BR>
 *			例外発生時:"FATAL"<BR>
 *			エラー時:"ERROR"
 * @param	message0 エラーメッセージ書式
 * @param	arg エラーメッセージ要素データ
 * @throw	err_rec:エラー情報を持つ構造体.
 * @note	このメンバ関数はprivateです
 */
template <class datatype>
void asarray<datatype>::err_throw1( const char *func0, const char *level0, 
				  const char *message0, const char *arg ) const
{
    static stdstreamio sio;
    static tstring mes(128);
    const char *classname = "asarray";
    sio.eprintf("%s::%s(): [%s] ",classname,func0,level0);
    mes.printf(message0, arg);
    sio.eprintf("%s\n", mes.cstr());
    {
	size_t i, len;
	const char *src;
	char *dst;
	sli::err_rec st;
	/* */
	src = classname;  dst = st.class_name;   len = 64;
	for ( i=0 ; i < len && src[i] != '\0' ; i++ ) dst[i] = src[i];
	dst[i] = '\0';
	/* */
	src = func0;      dst = st.func_name;    len = 64;
	for ( i=0 ; i < len && src[i] != '\0' ; i++ ) dst[i] = src[i];
	dst[i] = '\0';
	/* */
	src = level0;     dst = st.level;        len = 64;
	for ( i=0 ; i < len && src[i] != '\0' ; i++ ) dst[i] = src[i];
	dst[i] = '\0';
	/* */
	src = mes.cstr(); dst = st.message;      len = 128;
	for ( i=0 ; i < len && src[i] != '\0' ; i++ ) dst[i] = src[i];
	dst[i] = '\0';
	/* */
	throw st;
    }
}


/**
 * @brief  警告を出力させるためのメンバ関数
 *
 *  asarrayクラスのメンバ関数内で警告を発生させるときに呼び出されます．
 *  標準エラー出力にエラーメッセージを出力します．
 *
 * @param	func0 呼び出し元メンバ関数名
 * @param	level0 警告:"WARNING"
 * @param	message0 エラーメッセージの書式
 * @param	arg	エラーメッセージ要素データ
 * @note	このメンバ関数はprivateです
 */
template <class datatype>
void asarray<datatype>::err_report1( const char *func0, const char *level0, 
				  const char *message0, const char *arg ) const
{
    static stdstreamio sio;
    const char *classname = "asarray";
    sio.eprintf("%s::%s(): [%s] ",classname,func0,level0);
    sio.eprintf(message0, arg);
    sio.eprintf("\n");
}

}	/* namespace sli */

#endif	/* _SLI__ASARRAY_H */
