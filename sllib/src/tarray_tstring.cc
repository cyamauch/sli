/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2013-05-14 00:14:25 cyamauch> */

/**
 * @file   tarray_tstring.cc
 * @brief  文字列配列を扱うtarray_tstringクラスのコード
 */

#define CLASS_NAME "tarray_tstring"

#include "config.h"

#include "tarray_tstring.h"
#include "heap_mem.h"

#include <stdlib.h>

#include "private/err_report.h"

#include "private/c_memcpy.h"
#include "private/c_strncmp.h"
#include "private/c_isalpha.h"
#include "private/c_regsearchx.h"
#include "private/c_regfatal.h"

/* #define TARRAY_TSTRING__REMOVE_ESCAPE 1 */

/* 正規表現の検索の時，内部キャッシュを使う */
#define REGFUNCS_USING_CACHE 1

namespace sli
{

/* make_accepts_regexp() and is_found() */
#include "private/tstring_regexp_cset.cc"

/*
 * オブジェクト生成時は，ヒープに領域を確保しない．
 * したがって，何もしないと obj.elements() は NULL 
 * が返る．
 *
 * const char tbl[] = {NULL};
 * hoge.init(tbl) や hoge.append(tbl)
 * に対しては，this->cstr_ptrs[0] = NULL を作る．
 *
 * hoge.init((char **)NULL)
 * だと初期化
 * 
 * hoge.append((char **)NULL) に対しては，無反応．
 *
 * オブジェクトを初期状態に戻したい(ヒープを開放したい)場合は，
 * obj = NULL;
 * とする．
 */

/**
 * @brief  tarray 内部で新規オブジェクトを作る時に呼ばれる関数
 *
 * @note   private な関数です．
 */
static tstring *tstring_creator(void *ptr)
{
    tstring *p;
    p = new tstring(true);
    return p;
}


/* constructor */

/**
 * @brief  コンストラクタ
 *
 * @throw  内部バッファの確保に失敗した場合
 * @throw  メモリ破壊を起こしたとき
 */
tarray_tstring::tarray_tstring()
{
    this->arrs_rec.register_creator(&tstring_creator, NULL);
    this->cstr_ptrs_rec.init(sizeof(char *), true);
    this->reg_pos_rec.init(sizeof(size_t), true);
    this->reg_length_rec.init(sizeof(size_t), true);
    return;
}

/* constructor */

/**
 * @brief  コンストラクタ
 *
 *  自身の配列を，文字列配列 elements の内容で初期化します．<br>
 *  elements の終端はNULLでなければなりません．
 *
 * @param  elements 文字列配列
 * @throw  内部バッファの確保に失敗した場合
 * @throw  メモリ破壊を起こしたとき
 */
tarray_tstring::tarray_tstring(const char *const *elements)
{
    this->arrs_rec.register_creator(&tstring_creator, NULL);
    this->cstr_ptrs_rec.init(sizeof(char *), true);
    this->reg_pos_rec.init(sizeof(size_t), true);
    this->reg_length_rec.init(sizeof(size_t), true);
    if ( elements != NULL ) this->assign(elements);
    return;
}

/* constructor */

/**
 * @brief  コンストラクタ
 *
 *  自身を与えられた文字列で初期化します．<br>
 *  引数の最後は必ず NULL を与えます．
 * 
 * @param  el0 文字列
 * @param  el1 文字列
 * @param  ... 文字列の各要素.
 * @throw  内部バッファの確保に失敗した場合
 * @throw  メモリ破壊を起こしたとき
 */
tarray_tstring::tarray_tstring(const char *el0, const char *el1, ...)
{
    va_list ap;

    this->arrs_rec.register_creator(&tstring_creator, NULL);
    this->cstr_ptrs_rec.init(sizeof(char *), true);
    this->reg_pos_rec.init(sizeof(size_t), true);
    this->reg_length_rec.init(sizeof(size_t), true);

    va_start(ap, el1);
    try {
	this->vassign( el0, el1, ap );
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
 *  自身を obj の内容で初期化します．
 * 
 * @param  obj tarray_tstringのオブジェクト
 * @throw  内部バッファの確保に失敗した場合
 * @throw  メモリ破壊を起こしたとき
 */
tarray_tstring::tarray_tstring(const tarray_tstring &obj)
{
    this->arrs_rec.register_creator(&tstring_creator, NULL);
    this->cstr_ptrs_rec.init(sizeof(char *), true);
    this->reg_pos_rec.init(sizeof(size_t), true);
    this->reg_length_rec.init(sizeof(size_t), true);
    this->init(obj);
    return;
}

/* destructor */

/**
 * @brief  デストラクタ
 */
tarray_tstring::~tarray_tstring()
{
    return;
}

/**
 * @brief  オブジェクトのコピー
 *
 *  演算子の右側(引数) で指定されたオブジェクトの内容または文字列配列を
 *  自身に代入します．
 *
 * @param      obj tarray_tstringクラスのオブジェクト
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 * @throw      メモリ破壊を起こした場合
 *
 */
tarray_tstring &tarray_tstring::operator=(const tarray_tstring &obj)
{
    this->init(obj);
    return *this;
}

/**
 * @brief  自身の配列の最後に，文字列配列(tarray_tstring)を追加
 *
 *  自身の配列に，演算子の右側(引数) で指定された文字列配列の追加を行います．
 *
 * @param      obj tarray_tstringクラスのオブジェクト
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 *
 */
tarray_tstring &tarray_tstring::operator+=(const tarray_tstring &obj)
{
    this->append(obj);
    return *this;
}

/**
 * @brief  自身の配列に，指定された文字列配列(const char *[])を代入
 *
 *  演算子の右側(引数) で指定されたオブジェクトの内容または文字列配列を
 *  自身に代入します．
 *
 * @param      elements 文字列のポインタ配列のアドレス(NULL 終端)
 * @return     内部文字列バッファへのポインタ配列
 * @throw      内部バッファの確保に失敗した場合
 * 
 */
const char *const *tarray_tstring::operator=(const char *const *elements)
{
    if ( elements == NULL ) {	/* NULLの時はからっぽにする */
        this->init();
    }
    else {
        this->assign(elements);
    }
    return this->cstrarray();
}

/**
 * @brief  自身の配列の最後に，指定された文字列配列(const char *[])を追加
 *
 *  自身の配列に，演算子の右側(引数) で指定された文字列配列の追加を行います．
 *
 * @param      elements 文字列のポインタ配列のアドレス(NULL 終端)
 * @return     内部文字列バッファへのポインタ配列
 * @throw      内部バッファの確保に失敗した場合
 *
 */
const char *const *tarray_tstring::operator+=(const char *const *elements)
{
    if ( elements != NULL ) this->append(elements);
    return this->cstrarray();
}

/**
 * @brief  自身の配列の最後に，指定された文字列要素(const char *)を追加
 *
 *  自身の配列に，演算子の右側(引数) で指定された文字列を追加します．
 *
 * @param   str 文字列のアドレス
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 *
 */
tarray_tstring &tarray_tstring::operator+=(const char *str)
{
    if ( str != NULL ) {
        this->append(str,1);
    }
    return *this;
}

/**
 * @brief  自身の配列の最後に，指定された文字列要素(tstring)を追加
 *
 *  自身の配列に，演算子の右側(引数) で指定された文字列を追加します．
 *
 * @param   one 文字列のアドレス(マニュアルではstr)
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 *
 */
tarray_tstring &tarray_tstring::operator+=(const tstring &one)
{
    if ( one.cstr() != NULL ) {
        this->append(one,1);
    }
    return *this;
}

/*
 * public member functions
 *
 * 基本方針として，引数の文字列,ポインタ配列は object 内部で管理されている
 * 領域が与えられても動作する仕様とする．
 *
 */

/**
 * @brief  オブジェクトの初期化
 *
 *  自身の文字列配列を消去し，オブジェクトの初期化を行います．
 *
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 *
 */
tarray_tstring &tarray_tstring::init()
{
    this->arrs_rec.init();
    this->arrs_rec.register_creator(&tstring_creator, NULL);
    this->cstr_ptrs_rec.init(sizeof(char *), true);
    this->reg_pos_rec.init(sizeof(size_t), true);
    this->reg_length_rec.init(sizeof(size_t), true);
    this->regex_rec.init();

    return *this;
}

/**
 * @brief  オブジェクトのコピー
 *
 *  指定されたオブジェクト obj の内容を自身にコピーします．
 *
 * @param      obj tarray_tstring クラスのオブジェクト
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 * @throw      メモリ破壊を起こした場合
 *
 */
tarray_tstring &tarray_tstring::init(const tarray_tstring &obj)
{
    if ( &obj == this ) return *this;
    this->arrs_rec.init();
    this->arrs_rec.register_creator(&tstring_creator, NULL);
    this->cstr_ptrs_rec.init(sizeof(char *), true);
    this->reg_pos_rec.init(obj.reg_pos_rec);
    this->reg_length_rec.init(obj.reg_length_rec);
    /* this->regex_rec.init(obj.regex_rec); */
    this->regex_rec.init();
    this->append(obj);

    return *this;
}

/**
 * @brief  配列全体を指定された文字列(const char *)でパディング
 *
 *  自身が持つ配列の全要素を文字列 str でパディングします．
 *
 * @param      str 文字列配列をパディングするための文字列
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 *
 */
tarray_tstring &tarray_tstring::clean(const char *str)
{
    if ( str != NULL ) {
	tstring one = str;
	this->arrs_rec.clean(one);
    }
    return *this;
}

/**
 * @brief  配列全体を指定された文字列(tstring)でパディング
 *
 *  自身が持つ配列の全要素を文字列 str でパディングします．
 *
 * @param      one 文字列配列をパディングするための文字列(マニュアルではstr)
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 *
 */
tarray_tstring &tarray_tstring::clean(const tstring &one)
{
    if ( one.cstr() != NULL ) {
	this->arrs_rec.clean(one);
    }
    return *this;
}

/**
 * @brief  コンパイル時にエラーを出力させるためのダミーのメンバ関数
 * @deprecated  使用不可．
 * @note	このメンバ関数はprivateです
 */
tarray_tstring &tarray_tstring::assign( size_t n )
{
    return this->replace(0, this->length(), n);
}

/**
 * @brief  指定された文字列配列(const char *[])を自身に代入
 *
 *  elementsで指定された文字列配列を，自身に代入します．
 *
 * @param      elements 要素に入る文字列のポインタ配列(NULL で終端)
 * @return     自身の参照
 *
 */
tarray_tstring &tarray_tstring::assign( const char *const *elements )
{
    size_t i;
    if ( elements == NULL ) return this->assign(elements, 0);
    for ( i=0 ; elements[i] != NULL ; i++ );
    return this->assign(elements,i);
}

/**
 * @brief  指定された文字列配列(const char *[])のn要素を自身に代入
 *
 *  elementsで指定された文字列配列の n 個を，自身に代入します．
 *
 * @param      elements 要素に入る文字列のポインタ配列
 * @param      n 配列elements の要素数
 * @return     自身の参照
 *
 */
tarray_tstring &tarray_tstring::assign( const char *const *elements, size_t n )
{
    return this->replace(0, this->length(), elements, n);
}

/**
 * @brief  指定された文字列配列(tarray_tstring)の(一部)要素を自身に代入
 *
 *  srcで指定された文字列配列の全部または一部を，自身に代入します．
 *
 * @param    src 源泉となる文字列配列を持つtarray_tstring クラスのオブジェクト
 * @param    idx2 src中の要素の開始位置(src の部分配列を代入する場合)
 * @return   自身の参照
 *
 */
tarray_tstring &tarray_tstring::assign(const tarray_tstring &src, size_t idx2)
{
    return this->assign(src, idx2, src.length());
}

/**
 * @brief  指定された文字列配列(tarray_tstring)の(一部)要素を自身に代入
 *
 *  srcで指定された文字列配列の全部または一部を，自身に代入します．
 *
 * @param    src 源泉となる文字列配列を持つtarray_tstring クラスのオブジェクト
 * @param    idx2 src中の要素の開始位置(src の部分配列を代入する場合)
 * @param    n2 src 中の要素の個数(src の部分配列を代入する場合)
 * @return   自身の参照
 *
 */
tarray_tstring &tarray_tstring::assign( const tarray_tstring &src, 
					size_t idx2, size_t n2 )
{
    return this->replace(0, this->length(), src, idx2, n2);
}

/**
 * @brief  指定された文字列(const char *)をn個 自身に代入
 *
 *  自身の配列要素をn個とし，すべての要素に指定された文字列を代入します．
 *
 * @param      str 源泉となる文字列
 * @param      n 配列の要素数
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 *
 */
tarray_tstring &tarray_tstring::assign( const char *str, size_t n )
{
    return this->replace(0, this->length(), str, n);
}

/**
 * @brief  指定された文字列(tstring)をn個 自身に代入
 *
 *  自身の配列要素をn個とし，すべての要素に指定された文字列を代入します．
 *
 * @param      one 源泉となる文字列(マニュアルではstr)
 * @param      n 配列の要素数
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 *
 */
tarray_tstring &tarray_tstring::assign( const tstring &one, size_t n )
{
    return this->replace(0, this->length(), one, n);
}

/**
 * @brief  指定された文字列(printf()の記法で指定)をn個 自身に代入
 *
 *  自身の配列要素をn個とし，すべての要素に指定された文字列を代入します．
 *
 * @param   n 配列の要素数
 * @param   format 源泉となる文字列のためのフォーマット指定
 * @param   ... formatに対応した可変長引数の各要素データ
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 * @throw   可変長引数の各要素データが指定された変換フォーマットで
 *          変換できない値の場合
 *
 */
tarray_tstring &tarray_tstring::assignf( size_t n, const char *format, ... )
{
    va_list ap;
    va_start(ap,format);
    try {
	this->vassignf(n,format,ap);
    }
    catch (...) {
	va_end(ap);
        err_throw(__FUNCTION__,"FATAL","this->vassignf() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  指定された文字列(printf()の記法で指定)をn個 自身に代入
 *
 *  自身の配列要素をn個とし，すべての要素に指定された文字列を代入します．
 *
 * @param      n 配列の要素数
 * @param      format 源泉となる文字列のためのフォーマット指定
 * @param      ap formatに対応した可変長引数のリスト
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 * @throw      可変長引数の各要素データが指定された変換フォーマットで
 *             変換できない値の場合
 *
 */
tarray_tstring &tarray_tstring::vassignf(size_t n, const char *format, va_list ap)
{
    tstring one;

    if ( format == NULL ) return this->assign(format,n);

    try {
	one.vprintf(format,ap);
    }
    catch (...) {
	err_throw(__FUNCTION__,"FATAL","one.vprintf() failed");
    }
    this->assign(one,n);

    return *this;
}

/**
 * @brief  指定された複数の文字列を自身に代入
 *
 *  可変引数で指定された複数の文字列を，文字列配列として自身に代入します．
 *
 * @param      el0 要素に入る文字列(0 番目)
 * @param      el1 要素に入る文字列(1 番目)
 * @param      ... 要素に入る文字列(2 番目以降; 要NULL 終端)
 * @return     自身の参照
 *
 */
tarray_tstring &tarray_tstring::assign( const char *el0, const char *el1, ... )
{
    va_list ap;
    va_start(ap, el1);
    try {
	this->vassign( el0, el1, ap );
    }
    catch (...) {
	va_end(ap);
        err_throw(__FUNCTION__,"FATAL","this->vassign() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  指定された複数の文字列を自身に代入
 *
 *  可変引数で指定された複数の文字列を，文字列配列として自身に代入します．
 *
 * @param      el0 要素に入る文字列(0 番目)
 * @param      el1 要素に入る文字列(1 番目)
 * @param      ap 要素に入る文字列の可変長引数のリスト(2 番目以降; NULL 終端)
 * @return     自身の参照
 *
 */
tarray_tstring &tarray_tstring::vassign( const char *el0, const char *el1, 
					 va_list ap )
{
    heap_mem<const char *> tmp_ptbl;
    size_t element_count, i;
    va_list aq;

    va_copy(aq, ap);

    element_count = 0;
    if ( el0 != NULL ) {
        element_count++;
	if ( el1 != NULL ) {
	    element_count++;
	    while ( 1 ) {
	        const char *elem = va_arg(aq,char *);
		if ( elem == NULL ) break;
		element_count++;
	    }
	}
    }
    va_end(aq);

    if ( tmp_ptbl.allocate(element_count + 1) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","malloc() failed");
    }

    i = 0;
    if ( el0 != NULL ) {
        tmp_ptbl[i] = el0;
        i++;
	if ( el1 != NULL ) {
	    tmp_ptbl[i] = el1;
	    i++;
	    while ( 1 ) {
	        const char *elem = va_arg(ap,char *);
		if ( elem == NULL ) break;
		tmp_ptbl[i] = elem;
		i++;
	    }
	}
    }
    tmp_ptbl[i] = NULL;

    this->assign(tmp_ptbl.ptr());

    return *this;
}


#define LEN_RET_S_STK 512
#define LEN_RET_P_STK 128

/* len_src_str は不明な場合は 0 にしても良い */
inline void explode_php( const char *src_str, size_t len_src_str, 
		   const char *delim, bool zero_str, tarray_tstring *retobjp )
{
    /* 一時バッファ: */
    /* 基本的には最初はスタックを使い，足りなくなったらヒープに切り替える */
    char ret_s_stk[LEN_RET_S_STK];		/* result for temp string */
    char *ret_s_heap = NULL;
    size_t len_ret_s_buf = LEN_RET_S_STK;
    char *ret_s = ret_s_stk;
    size_t ix_ret_s = 0;			/* current index of ret_s */
    size_t ix0_ret_s = 0;			/* first pos of current elem */

    union _ret_p {
	char *ptr;
	size_t offset;
    };
    union _ret_p ret_p_stk[LEN_RET_P_STK];	/* result for temp str ptrs */
    union _ret_p *ret_p_heap = NULL;
    size_t len_ret_p_buf = LEN_RET_P_STK;
    union _ret_p *ret_p = ret_p_stk;
    size_t ix_ret_p = 0;			/* current index of ret_p */

    size_t i;

    if ( src_str == NULL ) {
	retobjp->assign(src_str, 1);
	goto quit;
    }
    if ( delim == NULL ) delim = "";

    /* len_src_str が 0 でない場合，明らかに足りない場合はヒープに切り替え */
    if ( LEN_RET_S_STK < len_src_str + 1 ) {
	ret_s_heap = (char *)malloc(len_src_str + 1);
	if ( ret_s_heap == NULL ) {
	    err_throw(__FUNCTION__,"FATAL","malloc() failed");
	}
	len_ret_s_buf = len_src_str + 1;
	ret_s = ret_s_heap;
    }

    /*
     * parse src_str
     */

    try {
	i = 0;
	while ( src_str[i] != '\0' ) {
	    char ch = src_str[i];
	    size_t i_step = 1;
	    size_t j;
	    for ( j=0 ; delim[j] != '\0' ; j++ ) {
		if ( src_str[i+j] != delim[j] ) break;
	    }
	    if ( 0 < j && delim[j] == '\0' ) {
		ch = '\0';
		i_step = j;
	    }

	    /* 一時バッファに格納 */
	    if ( len_ret_s_buf <= ix_ret_s + 1 ) {
		size_t ii, len_realloc;
		if ( len_src_str == 0 ) {
		    /* src_strの長さが不明の場合は調べてしまう */
		    for ( ii=0 ; src_str[i + ii] != '\0' ; ii++ );
		    len_realloc = i + ii + 1;
		} else len_realloc = len_src_str + 1;

		if ( ret_s_heap == NULL ) {
		    ret_s_heap = (char *)malloc(len_realloc);
		    if ( ret_s_heap == NULL ) {
			err_throw(__FUNCTION__,"FATAL","malloc() failed");
		    }
		    c_memcpy(ret_s_heap, ret_s, len_ret_s_buf);
		}
		else {
		    void *tmp_ptr = realloc(ret_s_heap, len_realloc);
		    if ( tmp_ptr == NULL ) {
			err_throw(__FUNCTION__,"FATAL","realloc() failed");
		    }
		    ret_s_heap = (char *)tmp_ptr;
		}
		ret_s = ret_s_heap;
		len_ret_s_buf = len_realloc;
	    }
	    ret_s[ix_ret_s] = ch;
	    ix_ret_s ++;
	    
	    /* 区切り文字列だった場合の処理: アドレステーブルに登録 */
	    if ( ch == '\0' ) {
		if ( zero_str == true || ix0_ret_s + 1 < ix_ret_s ) {
		    if ( len_ret_p_buf <= ix_ret_p + 2 ) {
			size_t len_realloc = len_ret_p_buf * 2;
			if ( ret_p_heap == NULL ) {
			    ret_p_heap = (union _ret_p *)malloc(sizeof(*ret_p_heap)*len_realloc);
			    if ( ret_p_heap == NULL ) {
				err_throw(__FUNCTION__,"FATAL","malloc() failed");
			    }
			    c_memcpy(ret_p_heap, ret_p, sizeof(*ret_p_heap)*len_ret_p_buf);
			}
			else {
			    void *tmp_ptr = realloc(ret_p_heap, sizeof(*ret_p_heap)*len_realloc);
			    if ( tmp_ptr == NULL ) {
				err_throw(__FUNCTION__,"FATAL","realloc() failed");
			    }
			    ret_p_heap = (union _ret_p *)tmp_ptr;
			}
			ret_p = ret_p_heap;
			len_ret_p_buf = len_realloc;
		    }
		    ret_p[ix_ret_p].offset = ix0_ret_s;
		    ix_ret_p ++;
		}
		ix0_ret_s = ix_ret_s;
	    }
	    i += i_step;
	}
    }
    catch (...) {
	if ( ret_p_heap != NULL ) {
	    free(ret_p_heap);
	    ret_p_heap = NULL;
	}
	if ( ret_s_heap != NULL ) {
	    free(ret_s_heap);
	    ret_s_heap = NULL;
	}
	err_throw(__FUNCTION__,"FATAL","caught exception");
    }
    ret_s[ix_ret_s] = '\0';
    ix_ret_s ++;
    if ( zero_str == true || ix0_ret_s + 1 < ix_ret_s ) {
	ret_p[ix_ret_p].offset = ix0_ret_s;
	ix_ret_p ++;
    }
    ret_p[ix_ret_p].ptr = NULL;
    for ( i=0 ; i < ix_ret_p ; i++ ) {
	size_t off = ret_p[i].offset;
	ret_p[i].ptr = ret_s + off;
    }

    try {
	retobjp->erase();
	retobjp->append((char **)ret_p);
    }
    catch (...) {
	if ( ret_p_heap != NULL ) {
	    free(ret_p_heap);
	    ret_p_heap = NULL;
	}
	if ( ret_s_heap != NULL ) {
	    free(ret_s_heap);
	    ret_s_heap = NULL;
	}
	err_throw(__FUNCTION__,"FATAL","this->append() failed");
    }


 quit:
    if ( ret_p_heap != NULL ) free(ret_p_heap);
    if ( ret_s_heap != NULL ) free(ret_s_heap);
    return;
}


/* len_src_str は不明な場合は 0 にしても良い */
inline void explode_advanced( const char *src_str, size_t len_src_str, 
			      const char *delim, bool zero_str, 
			      const char *quot_bkt, 
			      int escape, bool rm_escape,
			      tarray_tstring *retobjp )
{
    char quot[96];				/* list of quotations */
    char bkt_l[5];				/* list of brackets (left) */
    char bkt_r[5];				/* list of brackets (right) */

    bool prev_escape = false;
    char quot_to_find = '\0';			/* quotation to be found */
    tstring bkt_to_find;			/* 見つけるべきペアの括弧 */
    size_t depth_bkt = 0;			/* 括弧の深さ */

    /* 一時バッファ: */
    /* 基本的には最初はスタックを使い，足りなくなったらヒープに切り替える */
    char ret_s_stk[LEN_RET_S_STK];		/* result for temp string */
    char *ret_s_heap = NULL;
    size_t len_ret_s_buf = LEN_RET_S_STK;
    char *ret_s = ret_s_stk;
    size_t ix_ret_s = 0;			/* current index of ret_s */
    size_t ix0_ret_s = 0;			/* first pos of current elem */

    union _ret_p {
	char *ptr;
	size_t offset;
    };
    union _ret_p ret_p_stk[LEN_RET_P_STK];	/* result for temp str ptrs */
    union _ret_p *ret_p_heap = NULL;
    size_t len_ret_p_buf = LEN_RET_P_STK;
    union _ret_p *ret_p = ret_p_stk;
    size_t ix_ret_p = 0;			/* current index of ret_p */

    size_t i;

    if ( src_str == NULL ) {
	retobjp->assign(src_str, 1);
	goto quit;
    }
    if ( delim == NULL ) delim = "";

    /* len_src_str が 0 でない場合，明らかに足りない場合はヒープに切り替え */
    if ( LEN_RET_S_STK < len_src_str + 1 ) {
	ret_s_heap = (char *)malloc(len_src_str + 1);
	if ( ret_s_heap == NULL ) {
	    err_throw(__FUNCTION__,"FATAL","malloc() failed");
	}
	len_ret_s_buf = len_src_str + 1;
	ret_s = ret_s_heap;
    }

    /*
     * setup quotations and bracket info: quot[] bkt_l[] bkt_r[]
     */
    if ( quot_bkt != NULL ) {
	bool b0=false, b1=false, b2=false, b3=false;
	size_t j=0, k=0;
	for ( i=0 ; quot_bkt[i] != '\0' ; i++ ) {
	    const char ch = quot_bkt[i];
	    if ( ch == '[' || ch == ']' ) {
		if ( b0 == false ) {
		    bkt_l[k] = '[';  bkt_r[k] = ']';
		    k++;  b0 = true;
		}
	    }
	    else if ( ch == '{' || ch == '}' ) {
		if ( b1 == false ) {
		    bkt_l[k] = '{';  bkt_r[k] = '}';
		    k++;  b1 = true;
		}
	    }
	    else if ( ch == '(' || ch == ')' ) {
		if ( b2 == false ) {
		    bkt_l[k] = '(';  bkt_r[k] = ')';
		    k++;  b2 = true;
		}
	    }
	    else if ( ch == '<' || ch == '>' ) {
		if ( b3 == false ) {
		    bkt_l[k] = '<';  bkt_r[k] = '>';
		    k++;  b3 = true;
		}
	    }
	    else {
		quot[j] = ch;
		if ( j + 1 < 96 ) j++;
	    }
	}
	quot[j] = '\0';
	bkt_l[k] = '\0';
	bkt_r[k] = '\0';
    }
    else {
	quot[0] = '\0';
	bkt_l[0] = '\0';
	bkt_r[0] = '\0';
    }

    /*
     * parse src_str
     */

    try {
      i = 0;
      while ( src_str[i] != '\0' ) {
	char ch = src_str[i];
	bool cpy_ok = true;
	size_t i_step = 1;
	if ( prev_escape == true ) {		/* 前のchrがescape文字なら */
	    prev_escape = false;
	}
	else if ( ch == escape ) {		/* 今のchrがescape文字なら */
	    if ( rm_escape == true ) cpy_ok = false;
	    prev_escape = true;
	}
	else if ( quot_to_find != '\0' ) {	/* ペアの quot を見つける */
	    if ( ch == quot_to_find ) quot_to_find = '\0';
	}
	else {
	    size_t j;
	    /* quot の開始を見つける */
	    for ( j=0 ; quot[j] != '\0' ; j++ ) {
		if ( ch == quot[j] ) {
		    quot_to_find = ch;
		    break;
		}
	    }
	    if ( quot[j] == '\0' ) {
		/* 新規の括弧を見つける */
		for ( j=0 ; bkt_l[j] != '\0' ; j++ ) {
		    if ( ch == bkt_l[j] ) {
			bkt_to_find.at(depth_bkt) = bkt_r[j];
			depth_bkt ++;
			break;
		    }
		}
	    }
	    if ( bkt_l[j] == '\0' ) {
		/* ペアの括弧を見つける */
		if ( 0 < depth_bkt ) {
		    if ( ch == bkt_to_find.at_cs(depth_bkt-1) ) {
			depth_bkt --;
		    }
		}
		/* ようやくここで区切り文字の判定 */
		else {
		    for ( j=0 ; delim[j] != '\0' ; j++ ) {
			if ( src_str[i+j] != delim[j] ) break;
		    }
		    if ( 0 < j && delim[j] == '\0' ) {
			ch = '\0';
			i_step = j;
		    }
		}
	    }
	}
	if ( cpy_ok == true ) {

	    /* 一時バッファに格納 */
	    if ( len_ret_s_buf <= ix_ret_s + 1 ) {
		size_t ii, len_realloc;
		if ( len_src_str == 0 ) {
		    /* src_strの長さが不明の場合は調べてしまう */
		    for ( ii=0 ; src_str[i + ii] != '\0' ; ii++ );
		    len_realloc = i + ii + 1;
		} else len_realloc = len_src_str + 1;

		if ( ret_s_heap == NULL ) {
		    ret_s_heap = (char *)malloc(len_realloc);
		    if ( ret_s_heap == NULL ) {
			err_throw(__FUNCTION__,"FATAL","malloc() failed");
		    }
		    c_memcpy(ret_s_heap, ret_s, len_ret_s_buf);
		}
		else {
		    void *tmp_ptr = realloc(ret_s_heap, len_realloc);
		    if ( tmp_ptr == NULL ) {
			err_throw(__FUNCTION__,"FATAL","realloc() failed");
		    }
		    ret_s_heap = (char *)tmp_ptr;
		}
		ret_s = ret_s_heap;
		len_ret_s_buf = len_realloc;
	    }
	    ret_s[ix_ret_s] = ch;
	    ix_ret_s ++;

	    /* 区切り文字列だった場合の処理: アドレステーブルに登録 */
	    if ( ch == '\0' ) {
		if ( zero_str == true || ix0_ret_s + 1 < ix_ret_s ) {
		    if ( len_ret_p_buf <= ix_ret_p + 2 ) {
			size_t len_realloc = len_ret_p_buf * 2;
			if ( ret_p_heap == NULL ) {
			    ret_p_heap = (union _ret_p *)malloc(sizeof(*ret_p_heap)*len_realloc);
			    if ( ret_p_heap == NULL ) {
				err_throw(__FUNCTION__,"FATAL","malloc() failed");
			    }
			    c_memcpy(ret_p_heap, ret_p, sizeof(*ret_p_heap)*len_ret_p_buf);
			}
			else {
			    void *tmp_ptr = realloc(ret_p_heap, sizeof(*ret_p_heap)*len_realloc);
			    if ( tmp_ptr == NULL ) {
				err_throw(__FUNCTION__,"FATAL","realloc() failed");
			    }
			    ret_p_heap = (union _ret_p *)tmp_ptr;
			}
			ret_p = ret_p_heap;
			len_ret_p_buf = len_realloc;
		    }
		    ret_p[ix_ret_p].offset = ix0_ret_s;
		    ix_ret_p ++;
		}
		ix0_ret_s = ix_ret_s;
	    }
	}
	i += i_step;
      }
    }
    catch (...) {
	if ( ret_p_heap != NULL ) {
	    free(ret_p_heap);
	    ret_p_heap = NULL;
	}
	if ( ret_s_heap != NULL ) {
	    free(ret_s_heap);
	    ret_s_heap = NULL;
	}
	err_throw(__FUNCTION__,"FATAL","caught exception");
    }
    ret_s[ix_ret_s] = '\0';
    ix_ret_s ++;
    if ( zero_str == true || ix0_ret_s + 1 < ix_ret_s ) {
	ret_p[ix_ret_p].offset = ix0_ret_s;
	ix_ret_p ++;
    }
    ret_p[ix_ret_p].ptr = NULL;
    for ( i=0 ; i < ix_ret_p ; i++ ) {
	size_t off = ret_p[i].offset;
	ret_p[i].ptr = ret_s + off;
    }

    try {
	retobjp->erase();
	retobjp->append((char **)ret_p);
    }
    catch (...) {
	if ( ret_p_heap != NULL ) {
	    free(ret_p_heap);
	    ret_p_heap = NULL;
	}
	if ( ret_s_heap != NULL ) {
	    free(ret_s_heap);
	    ret_s_heap = NULL;
	}
	err_throw(__FUNCTION__,"FATAL","this->append() failed");
    }


 quit:
    if ( ret_p_heap != NULL ) free(ret_p_heap);
    if ( ret_s_heap != NULL ) free(ret_s_heap);
    return;
}


/* len_src_str は不明な場合は 0 にしても良い */
inline void split_simple( const char *src_str, size_t len_src_str, 
		   const char *delims, bool zero_str, tarray_tstring *retobjp )
{
    /* 一時バッファ: */
    /* 基本的には最初はスタックを使い，足りなくなったらヒープに切り替える */
    char ret_s_stk[LEN_RET_S_STK];		/* result for temp string */
    char *ret_s_heap = NULL;
    size_t len_ret_s_buf = LEN_RET_S_STK;
    char *ret_s = ret_s_stk;
    size_t ix_ret_s = 0;			/* current index of ret_s */
    size_t ix0_ret_s = 0;			/* first pos of current elem */

    union _ret_p {
	char *ptr;
	size_t offset;
    };
    union _ret_p ret_p_stk[LEN_RET_P_STK];	/* result for temp str ptrs */
    union _ret_p *ret_p_heap = NULL;
    size_t len_ret_p_buf = LEN_RET_P_STK;
    union _ret_p *ret_p = ret_p_stk;
    size_t ix_ret_p = 0;			/* current index of ret_p */

    bool reg_delims = false;
    tstring accepts;
    bool ac_bl;
    int ac_flg = 0;
    size_t i;

    if ( src_str == NULL ) {
	retobjp->assign(src_str, 1);
	goto quit;
    }
    if ( delims == NULL ) delims = "";

    /* check [A-Z] style */
    if ( delims[0] == '[' ) {
	for ( i=0 ; delims[i] != '\0' ; i++ );
	if ( delims[i-1] == ']' ) {
	    size_t p0 = 0;
	    p0 += make_accepts_regexp(delims, true, accepts, &ac_flg, &ac_bl);
	    if ( p0 != 0 ) reg_delims = true;
	}
    }

    /* len_src_str が 0 でない場合，明らかに足りない場合はヒープに切り替え */
    if ( LEN_RET_S_STK < len_src_str + 1 ) {
	ret_s_heap = (char *)malloc(len_src_str + 1);
	if ( ret_s_heap == NULL ) {
	    err_throw(__FUNCTION__,"FATAL","malloc() failed");
	}
	len_ret_s_buf = len_src_str + 1;
	ret_s = ret_s_heap;
    }

    /*
     * parse src_str
     */

    try {
	for ( i=0 ; src_str[i] != '\0' ; i++ ) {
	    char ch = src_str[i];
	    size_t j;
	    if ( reg_delims == true ) {
		if ( is_found(ch, accepts.cstr(), ac_flg) == ac_bl ) {
		    ch = '\0';
		}
	    }
	    else {
		for ( j=0 ; delims[j] != '\0' ; j++ ) {
		    if ( ch == delims[j] ) {
			ch = '\0';
			break;
		    }
		}
	    }
	    /* 一時バッファに格納 */
	    if ( len_ret_s_buf <= ix_ret_s + 1 ) {
		size_t ii, len_realloc;
		if ( len_src_str == 0 ) {
		    /* src_strの長さが不明の場合は調べてしまう */
		    for ( ii=0 ; src_str[i + ii] != '\0' ; ii++ );
		    len_realloc = i + ii + 1;
		} else len_realloc = len_src_str + 1;

		if ( ret_s_heap == NULL ) {
		    ret_s_heap = (char *)malloc(len_realloc);
		    if ( ret_s_heap == NULL ) {
			err_throw(__FUNCTION__,"FATAL","malloc() failed");
		    }
		    c_memcpy(ret_s_heap, ret_s, len_ret_s_buf);
		}
		else {
		    void *tmp_ptr = realloc(ret_s_heap, len_realloc);
		    if ( tmp_ptr == NULL ) {
			err_throw(__FUNCTION__,"FATAL","realloc() failed");
		    }
		    ret_s_heap = (char *)tmp_ptr;
		}
		ret_s = ret_s_heap;
		len_ret_s_buf = len_realloc;
	    }
	    ret_s[ix_ret_s] = ch;
	    ix_ret_s ++;
	    
	    /* 区切り文字だった場合の処理: アドレステーブルに登録 */
	    if ( ch == '\0' ) {
		if ( zero_str == true || ix0_ret_s + 1 < ix_ret_s ) {
		    if ( len_ret_p_buf <= ix_ret_p + 2 ) {
			size_t len_realloc = len_ret_p_buf * 2;
			if ( ret_p_heap == NULL ) {
			    ret_p_heap = (union _ret_p *)malloc(sizeof(*ret_p_heap)*len_realloc);
			    if ( ret_p_heap == NULL ) {
				err_throw(__FUNCTION__,"FATAL","malloc() failed");
			    }
			    c_memcpy(ret_p_heap, ret_p, sizeof(*ret_p_heap)*len_ret_p_buf);
			}
			else {
			    void *tmp_ptr = realloc(ret_p_heap, sizeof(*ret_p_heap)*len_realloc);
			    if ( tmp_ptr == NULL ) {
				err_throw(__FUNCTION__,"FATAL","realloc() failed");
			    }
			    ret_p_heap = (union _ret_p *)tmp_ptr;
			}
			ret_p = ret_p_heap;
			len_ret_p_buf = len_realloc;
		    }
		    ret_p[ix_ret_p].offset = ix0_ret_s;
		    ix_ret_p ++;
		}
		ix0_ret_s = ix_ret_s;
	    }
	}
    }
    catch (...) {
	if ( ret_p_heap != NULL ) {
	    free(ret_p_heap);
	    ret_p_heap = NULL;
	}
	if ( ret_s_heap != NULL ) {
	    free(ret_s_heap);
	    ret_s_heap = NULL;
	}
	err_throw(__FUNCTION__,"FATAL","caught exception");
    }
    ret_s[ix_ret_s] = '\0';
    ix_ret_s ++;
    if ( zero_str == true || ix0_ret_s + 1 < ix_ret_s ) {
	ret_p[ix_ret_p].offset = ix0_ret_s;
	ix_ret_p ++;
    }
    ret_p[ix_ret_p].ptr = NULL;
    for ( i=0 ; i < ix_ret_p ; i++ ) {
	size_t off = ret_p[i].offset;
	ret_p[i].ptr = ret_s + off;
    }

    try {
	retobjp->erase();
	retobjp->append((char **)ret_p);
    }
    catch (...) {
	if ( ret_p_heap != NULL ) {
	    free(ret_p_heap);
	    ret_p_heap = NULL;
	}
	if ( ret_s_heap != NULL ) {
	    free(ret_s_heap);
	    ret_s_heap = NULL;
	}
	err_throw(__FUNCTION__,"FATAL","this->append() failed");
    }


 quit:
    if ( ret_p_heap != NULL ) free(ret_p_heap);
    if ( ret_s_heap != NULL ) free(ret_s_heap);
    return;
}

/* len_src_str は不明な場合は 0 にしても良い */
inline void split_advanced( const char *src_str, size_t len_src_str, 
		     const char *delims, bool zero_str, const char *quot_bkt,
		     int escape, bool rm_escape, tarray_tstring *retobjp )
{
    char quot[96];				/* list of quotations */
    char bkt_l[5];				/* list of brackets (left) */
    char bkt_r[5];				/* list of brackets (right) */

    bool prev_escape = false;
    char quot_to_find = '\0';			/* quotation to be found */
    tstring bkt_to_find;			/* 見つけるべきペアの括弧 */
    size_t depth_bkt = 0;			/* 括弧の深さ */

    /* 一時バッファ: */
    /* 基本的には最初はスタックを使い，足りなくなったらヒープに切り替える */
    char ret_s_stk[LEN_RET_S_STK];		/* result for temp string */
    char *ret_s_heap = NULL;
    size_t len_ret_s_buf = LEN_RET_S_STK;
    char *ret_s = ret_s_stk;
    size_t ix_ret_s = 0;			/* current index of ret_s */
    size_t ix0_ret_s = 0;			/* first pos of current elem */

    union _ret_p {
	char *ptr;
	size_t offset;
    };
    union _ret_p ret_p_stk[LEN_RET_P_STK];	/* result for temp str ptrs */
    union _ret_p *ret_p_heap = NULL;
    size_t len_ret_p_buf = LEN_RET_P_STK;
    union _ret_p *ret_p = ret_p_stk;
    size_t ix_ret_p = 0;			/* current index of ret_p */

    bool reg_delims = false;
    tstring accepts;
    bool ac_bl;
    int ac_flg = 0;
    size_t i;

    if ( src_str == NULL ) {
	retobjp->assign(src_str, 1);
	goto quit;
    }
    if ( delims == NULL ) delims = "";

    /* check [A-Z] style */
    if ( delims[0] == '[' ) {
	for ( i=0 ; delims[i] != '\0' ; i++ );
	if ( delims[i-1] == ']' ) {
	    size_t p0 = 0;
	    p0 += make_accepts_regexp(delims, true, accepts, &ac_flg, &ac_bl);
	    if ( p0 != 0 ) reg_delims = true;
	}
    }

    /* len_src_str が 0 でない場合，明らかに足りない場合はヒープに切り替え */
    if ( LEN_RET_S_STK < len_src_str + 1 ) {
	ret_s_heap = (char *)malloc(len_src_str + 1);
	if ( ret_s_heap == NULL ) {
	    err_throw(__FUNCTION__,"FATAL","malloc() failed");
	}
	len_ret_s_buf = len_src_str + 1;
	ret_s = ret_s_heap;
    }

    /*
     * setup quotations and bracket info: quot[] bkt_l[] bkt_r[]
     */
    if ( quot_bkt != NULL ) {
	bool b0=false, b1=false, b2=false, b3=false;
	size_t j=0, k=0;
	for ( i=0 ; quot_bkt[i] != '\0' ; i++ ) {
	    const char ch = quot_bkt[i];
	    if ( ch == '[' || ch == ']' ) {
		if ( b0 == false ) {
		    bkt_l[k] = '[';  bkt_r[k] = ']';
		    k++;  b0 = true;
		}
	    }
	    else if ( ch == '{' || ch == '}' ) {
		if ( b1 == false ) {
		    bkt_l[k] = '{';  bkt_r[k] = '}';
		    k++;  b1 = true;
		}
	    }
	    else if ( ch == '(' || ch == ')' ) {
		if ( b2 == false ) {
		    bkt_l[k] = '(';  bkt_r[k] = ')';
		    k++;  b2 = true;
		}
	    }
	    else if ( ch == '<' || ch == '>' ) {
		if ( b3 == false ) {
		    bkt_l[k] = '<';  bkt_r[k] = '>';
		    k++;  b3 = true;
		}
	    }
	    else {
		quot[j] = ch;
		if ( j + 1 < 96 ) j++;
	    }
	}
	quot[j] = '\0';
	bkt_l[k] = '\0';
	bkt_r[k] = '\0';
    }
    else {
	quot[0] = '\0';
	bkt_l[0] = '\0';
	bkt_r[0] = '\0';
    }

    /*
     * parse src_str
     */

    try {
      for ( i=0 ; src_str[i] != '\0' ; i++ ) {
	char ch = src_str[i];
	bool cpy_ok = true;
	if ( prev_escape == true ) {		/* 前のchrがescape文字なら */
	    prev_escape = false;
	}
	else if ( ch == escape ) {		/* 今のchrがescape文字なら */
	    if ( rm_escape == true ) cpy_ok = false;
	    prev_escape = true;
	}
	else if ( quot_to_find != '\0' ) {	/* ペアの quot を見つける */
	    if ( ch == quot_to_find ) quot_to_find = '\0';
	}
	else {
	    size_t j;
	    /* quot の開始を見つける */
	    for ( j=0 ; quot[j] != '\0' ; j++ ) {
		if ( ch == quot[j] ) {
		    quot_to_find = ch;
		    break;
		}
	    }
	    if ( quot[j] == '\0' ) {
		/* 新規の括弧を見つける */
		for ( j=0 ; bkt_l[j] != '\0' ; j++ ) {
		    if ( ch == bkt_l[j] ) {
			bkt_to_find.at(depth_bkt) = bkt_r[j];
			depth_bkt ++;
			break;
		    }
		}
	    }
	    if ( bkt_l[j] == '\0' ) {
		/* ペアの括弧を見つける */
		if ( 0 < depth_bkt ) {
		    if ( ch == bkt_to_find.at_cs(depth_bkt-1) ) {
			depth_bkt --;
		    }
		}
		/* ようやくここで区切り文字の判定 */
		else {
		    if ( reg_delims == true ) {
			if ( is_found(ch, accepts.cstr(), ac_flg) == ac_bl ) {
			    ch = '\0';
			}
		    }
		    else {
			for ( j=0 ; delims[j] != '\0' ; j++ ) {
			    if ( ch == delims[j] ) {
				ch = '\0';
				break;
			    }
			}
		    }
		}
	    }
	}
	if ( cpy_ok == true ) {

	    /* 一時バッファに格納 */
	    if ( len_ret_s_buf <= ix_ret_s + 1 ) {
		size_t ii, len_realloc;
		if ( len_src_str == 0 ) {
		    /* src_strの長さが不明の場合は調べてしまう */
		    for ( ii=0 ; src_str[i + ii] != '\0' ; ii++ );
		    len_realloc = i + ii + 1;
		} else len_realloc = len_src_str + 1;

		if ( ret_s_heap == NULL ) {
		    ret_s_heap = (char *)malloc(len_realloc);
		    if ( ret_s_heap == NULL ) {
			err_throw(__FUNCTION__,"FATAL","malloc() failed");
		    }
		    c_memcpy(ret_s_heap, ret_s, len_ret_s_buf);
		}
		else {
		    void *tmp_ptr = realloc(ret_s_heap, len_realloc);
		    if ( tmp_ptr == NULL ) {
			err_throw(__FUNCTION__,"FATAL","realloc() failed");
		    }
		    ret_s_heap = (char *)tmp_ptr;
		}
		ret_s = ret_s_heap;
		len_ret_s_buf = len_realloc;
	    }
	    ret_s[ix_ret_s] = ch;
	    ix_ret_s ++;

	    /* 区切り文字だった場合の処理: アドレステーブルに登録 */
	    if ( ch == '\0' ) {
		if ( zero_str == true || ix0_ret_s + 1 < ix_ret_s ) {
		    if ( len_ret_p_buf <= ix_ret_p + 2 ) {
			size_t len_realloc = len_ret_p_buf * 2;
			if ( ret_p_heap == NULL ) {
			    ret_p_heap = (union _ret_p *)malloc(sizeof(*ret_p_heap)*len_realloc);
			    if ( ret_p_heap == NULL ) {
				err_throw(__FUNCTION__,"FATAL","malloc() failed");
			    }
			    c_memcpy(ret_p_heap, ret_p, sizeof(*ret_p_heap)*len_ret_p_buf);
			}
			else {
			    void *tmp_ptr = realloc(ret_p_heap, sizeof(*ret_p_heap)*len_realloc);
			    if ( tmp_ptr == NULL ) {
				err_throw(__FUNCTION__,"FATAL","realloc() failed");
			    }
			    ret_p_heap = (union _ret_p *)tmp_ptr;
			}
			ret_p = ret_p_heap;
			len_ret_p_buf = len_realloc;
		    }
		    ret_p[ix_ret_p].offset = ix0_ret_s;
		    ix_ret_p ++;
		}
		ix0_ret_s = ix_ret_s;
	    }
	}
      }
    }
    catch (...) {
	if ( ret_p_heap != NULL ) {
	    free(ret_p_heap);
	    ret_p_heap = NULL;
	}
	if ( ret_s_heap != NULL ) {
	    free(ret_s_heap);
	    ret_s_heap = NULL;
	}
	err_throw(__FUNCTION__,"FATAL","caught exception");
    }
    ret_s[ix_ret_s] = '\0';
    ix_ret_s ++;
    if ( zero_str == true || ix0_ret_s + 1 < ix_ret_s ) {
	ret_p[ix_ret_p].offset = ix0_ret_s;
	ix_ret_p ++;
    }
    ret_p[ix_ret_p].ptr = NULL;
    for ( i=0 ; i < ix_ret_p ; i++ ) {
	size_t off = ret_p[i].offset;
	ret_p[i].ptr = ret_s + off;
    }

    try {
	retobjp->erase();
	retobjp->append((char **)ret_p);
    }
    catch (...) {
	if ( ret_p_heap != NULL ) {
	    free(ret_p_heap);
	    ret_p_heap = NULL;
	}
	if ( ret_s_heap != NULL ) {
	    free(ret_s_heap);
	    ret_s_heap = NULL;
	}
	err_throw(__FUNCTION__,"FATAL","this->append() failed");
    }


 quit:
    if ( ret_p_heap != NULL ) free(ret_p_heap);
    if ( ret_s_heap != NULL ) free(ret_s_heap);
    return;
}

/* 簡易版explode (高速版) */

/**
 * @brief  指定された文字列を区切り文字列で分割し，その結果を自身の配列に代入
 *
 *  文字列src_strを区切り文字列delimで分割し，その結果を文字列配列として自身へ
 *  代入します．文字列長ゼロの要素も許します(csv形式の分割に使えます)．<br>
 *  基本的には「::」のような2文字以上の区切り文字列の時に使うものです．
 *
 * @param      src_str 分割対象の文字列
 * @param      delim 区切り文字列
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 * @note       処理内容を限定しているため，相対的に高速に動作します．
 */
tarray_tstring &tarray_tstring::explode(const char *src_str, const char *delim,
					bool zero_str )
{
    explode_php(src_str, 0, delim, zero_str, this);
    return *this;
}

/**
 * @brief  指定された文字列を区切り文字列で分割し，その結果を自身の配列に代入
 *
 *  文字列src_strを区切り文字列delimで分割し，その結果を文字列配列として自身へ
 *  代入します．文字列長ゼロの要素も許します(csv形式の分割に使えます)．<br>
 *  基本的には「::」のような2文字以上の区切り文字列の時に使うものです．
 *
 * @param      src_str 分割対象の文字列
 * @param      delim 区切り文字列
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 * @note       処理内容を限定しているため，相対的に高速に動作します．
 */
tarray_tstring &tarray_tstring::explode(const tstring &src_str, const char *delim,
					bool zero_str )
{
    explode_php(src_str.cstr(), src_str.length(), delim, zero_str, this);
    return *this;
}

/* 多機能版explode */

/*
 * New member functions of 1.3.0
 */

/**
 * @brief  文字列を区切り文字列で分割し，その結果を自身の配列に代入(多機能版)
 *
 *  文字列src_strを区切り文字列delimで分割し，その結果を文字列配列として自身へ
 *  代入します．文字列長ゼロの要素も許します(csv形式の分割に使えます)．<br>
 *  基本的には「::」のような2文字以上の区切り文字列の時に使うものです．<br>
 *  クォーテーションや括弧の「特定の文字」で囲まれた文字列は分割しない場合，
 *  quot_bkt に「特定の文字」を指定します．
 *
 * @param      src_str 分割対象の文字列
 * @param      delim 区切り文字列
 * @param      quot_bkt クォーテーション・ブラケット文字を含む文字列
 * @param      escape エスケープ文字
 * @param      rm_escape エスケープ文字を削除するかどうかのフラグ(true/false)
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 */
tarray_tstring &tarray_tstring::explode(const char *src_str, const char *delim,
	      bool zero_str, const char *quot_bkt, int escape, bool rm_escape )
{
    explode_advanced(src_str, 0, delim, zero_str,
		     quot_bkt, escape, rm_escape, this);
    return *this;
}

/**
 * @brief  文字列を区切り文字列で分割し，その結果を自身の配列に代入(多機能版)
 *
 *  文字列src_strを区切り文字列delimで分割し，その結果を文字列配列として自身へ
 *  代入します．文字列長ゼロの要素も許します(csv形式の分割に使えます)．<br>
 *  基本的には「::」のような2文字以上の区切り文字列の時に使うものです．<br>
 *  クォーテーションや括弧の「特定の文字」で囲まれた文字列は分割しない場合，
 *  quot_bkt に「特定の文字」を指定します．
 *
 * @param      src_str 分割対象の文字列
 * @param      delim 区切り文字列
 * @param      quot_bkt クォーテーション・ブラケット文字を含む文字列
 * @param      escape エスケープ文字
 * @param      rm_escape エスケープ文字を削除するかどうかのフラグ(true/false)
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 */
tarray_tstring &tarray_tstring::explode(const tstring &src_str, const char *delim,
		bool zero_str, const char *quot_bkt, int escape, bool rm_escape )
{
    explode_advanced(src_str.cstr(), src_str.length(), delim, 
		     zero_str, quot_bkt, escape, rm_escape, this);
    return *this;
}


/* 簡易版split (高速版) */

/**
 * @brief  指定された文字列を区切り文字で分割し，その結果を自身の配列に代入
 *
 *  文字列 src_str を区切り文字 delims で分割し，その結果を文字列配列として
 *  自身へ代入します．
 *
 * @param      src_str 分割対象の文字列
 * @param      delims 区切り文字列を含む文字列．[A-Z] といった表現も使用可能．
 * @param      zero_str 分割後に長さ0 の文字列要素を許すか否か(true/false)
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 * @note       処理内容を限定しているため，相対的に高速に動作します．
 *
 */
tarray_tstring &tarray_tstring::split( const char *src_str,
				       const char *delims, bool zero_str )
{
    split_simple( src_str, 0, delims, zero_str, this );
    return *this;
}

/**
 * @brief  指定された文字列を区切り文字で分割し，その結果を自身の配列に代入
 *
 *  文字列 src_str を区切り文字 delims で分割し，その結果を文字列配列として
 *  自身へ代入します．
 *
 * @param      src_str 分割対象の文字列
 * @param      delims 区切り文字列を含む文字列．[A-Z] といった表現も使用可能．
 * @param      zero_str 分割後に長さ0 の文字列要素を許すか否か(true/false)
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 * @note       処理内容を限定しているため，相対的に高速に動作します．
 *
 */
tarray_tstring &tarray_tstring::split( const tstring &src_str,
				       const char *delims, bool zero_str )
{
    split_simple( src_str.cstr(), src_str.length(), delims, zero_str, this );
    return *this;
}


/* 多機能版split */

/*
 * 文字列 line を区切り文字 delims で分割する．[A-Z] といった表現も OK．
 * [...]の表現については，tstring クラスの strpbrk() を参照のこと．
 *
 * クォーテーションとエスケープ文字は quot_bkt と escape で設定する．
 *
 * [分割の例]
 *   foo name="bar \"hoge\" <xyz>"
 *   → 「foo」「name="bar \"hoge\" <xyz>"」 に分割
 *
 * zero_str は length=0 の文字列を許すかどうか．true の場合，"a,,c" の場合，
 * "a", "", "c" のように分割される．
 *
 * quot_bkt は Version 1.3.0 からブラケットにも対応した．このような分割もOK
 *   1,2,{3,4,5}
 *   → 「1」「2」「{3,4,5}」
 * ブラケットの種類は，[] {} () <> の4種．引数 quot_bkt に "[]{}" のように指定
 * する．普通のクォーテーションと混ぜて，"'\"[]{}" のようにしても良い．この
 * 場合は，クォーテーションの優先度が高いものとして扱われる．
 *
 * split() の最初の引数は，NULL でも良い．その場合，el=cstringarray() とした
 * 場合，el[0] が "" になる．
 *
 * [使用例1]
 *   tarray_tstring sobj;
 *   sobj.split(str, " \t", false, "\"", '\\');
 *   for ( i=0 ; i < sobj.length() ; i++ ) {
 *       printf("%d: [%s]\n", i, sobj.cstr(i));
 *   }
 */

/**
 * @brief  文字列を区切り文字で分割し，その結果を自身の配列に代入(多機能版)
 *
 *  文字列 src_str を区切り文字 delims で分割し，その結果を文字列配列として
 *  自身へ代入します．<br>
 *  クォーテーションや括弧の「特定の文字」で囲まれた文字列は分割しない場合，
 *  quot_bkt に「特定の文字」を指定します．
 *
 * @param      src_str 分割対象の文字列
 * @param      delims 区切り文字列を含む文字列．[A-Z] といった表現も使用可能．
 * @param      zero_str 分割後に長さ0 の文字列要素を許すか否か(true/false)
 * @param      quot_bkt クォーテーション・ブラケット文字を含む文字列
 * @param      escape エスケープ文字
 * @param      rm_escape エスケープ文字を削除するかどうかのフラグ(true/false)
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 *
 */
tarray_tstring &tarray_tstring::split( const char *src_str, const char *delims,
				       bool zero_str, const char *quot_bkt,
				       int escape, bool rm_escape )
{
    split_advanced( src_str, 0, delims, zero_str, 
		    quot_bkt, escape, rm_escape, this );
    return *this;
}

/**
 * @brief  文字列を区切り文字で分割し，その結果を自身の配列に代入(多機能版)
 *
 *  文字列 src_str を区切り文字 delims で分割し，その結果を文字列配列として
 *  自身へ代入します．<br>
 *  クォーテーションや括弧の「特定の文字」で囲まれた文字列は分割しない場合，
 *  quot_bkt に「特定の文字」を指定します．
 *
 * @param      src_str 分割対象の文字列
 * @param      delims 区切り文字列を含む文字列．[A-Z] といった表現も使用可能．
 * @param      zero_str 分割後に長さ0 の文字列要素を許すか否か(true/false)
 * @param      quot_bkt クォーテーション・ブラケット文字を含む文字列
 * @param      escape エスケープ文字
 * @param      rm_escape エスケープ文字を削除するかどうかのフラグ(true/false)
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 *
 */
tarray_tstring &tarray_tstring::split( const tstring &src_str, 
				       const char *delims,
				       bool zero_str, const char *quot_bkt,
				       int escape, bool rm_escape )
{
    split_advanced( src_str.cstr(), src_str.length(), delims, zero_str, 
		    quot_bkt, escape, rm_escape, this );
    return *this;
}


/* regassign */

/**
 * @brief  指定文字列に正規表現マッチを試行し，後方参照を含む結果を自身に格納
 *
 *  文字列src_strに対し，patで指定されたPOSIX 拡張正規表現(以下，正規表現)
 *  による文字列マッチを試行し，文字列配列として自身に格納します．
 *
 * @param      src_str マッチング対象の文字列
 * @param      pat 文字パターン(正規表現)
 * @return     自身の参照
 * @throw      regexルーチンがメモリを使い果たしている場合
 * @throw      内部バッファの確保に失敗した場合
 * @throw      メモリ破壊を起こした場合
 *
 */
tarray_tstring &tarray_tstring::regassign( const char *src_str, const char *pat )
{
    tstring one = src_str;
    if ( this->regex_rec.compile(pat) < 0 ) {
	err_report1(__FUNCTION__,"ERROR","%s",this->regex_rec.cerrstr());
    }
    this->regexp_match_advanced(one, 0, this->regex_rec, NULL);
    return *this;
}    

/**
 * @brief  指定文字列に正規表現マッチを試行し，後方参照を含む結果を自身に格納
 *
 *  文字列src_strに対し，patで指定されたPOSIX 拡張正規表現(以下，正規表現)
 *  による文字列マッチを試行し，文字列配列として自身に格納します．
 *
 * @param      src_str マッチング対象の文字列
 * @param      pos 文字列マッチの開始位置
 * @param      pat 文字パターン(正規表現)
 * @return     自身の参照
 * @throw      regexルーチンがメモリを使い果たしている場合
 * @throw      内部バッファの確保に失敗した場合
 * @throw      メモリ破壊を起こした場合
 *
 */
tarray_tstring &tarray_tstring::regassign( const char *src_str, size_t pos,
					   const char *pat )
{
    tstring one = src_str;
    if ( this->regex_rec.compile(pat) < 0 ) {
	err_report1(__FUNCTION__,"ERROR","%s",this->regex_rec.cerrstr());
    }
    this->regexp_match_advanced(one, pos, this->regex_rec, NULL);
    return *this;
}

/**
 * @brief  指定文字列に正規表現マッチを試行し，後方参照を含む結果を自身に格納
 *
 *  文字列src_strに対し，patで指定されたPOSIX 拡張正規表現(以下，正規表現)
 *  による文字列マッチを試行し，文字列配列として自身に格納します．
 *
 * @param      src_str マッチング対象の文字列
 * @param      pos 文字列マッチの開始位置
 * @param      pat 文字パターン(正規表現)
 * @param      nextpos 次回のpos
 * @return     自身の参照
 * @throw      regexルーチンがメモリを使い果たしている場合
 * @throw      内部バッファの確保に失敗した場合
 * @throw      メモリ破壊を起こした場合
 *
 */
tarray_tstring &tarray_tstring::regassign( const char *src_str, size_t pos,
					   const char *pat, size_t *nextpos )
{
    tstring one = src_str;
    if ( this->regex_rec.compile(pat) < 0 ) {
	err_report1(__FUNCTION__,"ERROR","%s",this->regex_rec.cerrstr());
    }
    this->regexp_match_advanced(one, pos, this->regex_rec, nextpos);
    return *this;
}

/**
 * @brief  指定文字列に正規表現マッチを試行し，後方参照を含む結果を自身に格納
 *
 *  文字列src_strに対し，patで指定されたPOSIX 拡張正規表現(以下，正規表現)
 *  による文字列マッチを試行し，文字列配列として自身に格納します．
 *
 * @param      src_str マッチング対象の文字列
 * @param      pat 文字パターン(正規表現)
 * @return     自身の参照
 * @throw      regexルーチンがメモリを使い果たしている場合
 * @throw      内部バッファの確保に失敗した場合
 * @throw      メモリ破壊を起こした場合
 *
 */
tarray_tstring &tarray_tstring::regassign(const tstring &src_str, const char *pat)
{
    if ( this->regex_rec.compile(pat) < 0 ) {
	err_report1(__FUNCTION__,"ERROR","%s",this->regex_rec.cerrstr());
    }
    this->regexp_match_advanced(src_str, 0, this->regex_rec, NULL);
    return *this;
}

/**
 * @brief  指定文字列に正規表現マッチを試行し，後方参照を含む結果を自身に格納
 *
 *  文字列src_strに対し，patで指定されたPOSIX 拡張正規表現(以下，正規表現)
 *  による文字列マッチを試行し，文字列配列として自身に格納します．
 *
 * @param      src_str マッチング対象の文字列
 * @param      pos 文字列マッチの開始位置
 * @param      pat 文字パターン(正規表現)
 * @return     自身の参照
 * @throw      regexルーチンがメモリを使い果たしている場合
 * @throw      内部バッファの確保に失敗した場合
 * @throw      メモリ破壊を起こした場合
 *
 */
tarray_tstring &tarray_tstring::regassign( const tstring &src_str, size_t pos,
					   const char *pat )
{
    if ( this->regex_rec.compile(pat) < 0 ) {
	err_report1(__FUNCTION__,"ERROR","%s",this->regex_rec.cerrstr());
    }
    this->regexp_match_advanced(src_str, pos, this->regex_rec, NULL);
    return *this;
}

/**
 * @brief  指定文字列に正規表現マッチを試行し，後方参照を含む結果を自身に格納
 *
 *  文字列src_strに対し，patで指定されたPOSIX 拡張正規表現(以下，正規表現)
 *  による文字列マッチを試行し，文字列配列として自身に格納します．
 *
 * @param      src_str マッチング対象の文字列
 * @param      pos 文字列マッチの開始位置
 * @param      pat 文字パターン(正規表現)
 * @param      nextpos 次回のpos
 * @return     自身の参照
 * @throw      regexルーチンがメモリを使い果たしている場合
 * @throw      内部バッファの確保に失敗した場合
 * @throw      メモリ破壊を起こした場合
 *
 */
tarray_tstring &tarray_tstring::regassign( const tstring &src_str, size_t pos,
					   const char *pat, size_t *nextpos )
{
    if ( this->regex_rec.compile(pat) < 0 ) {
	err_report1(__FUNCTION__,"ERROR","%s",this->regex_rec.cerrstr());
    }
    this->regexp_match_advanced(src_str, pos, this->regex_rec, nextpos);
    return *this;
}

/**
 * @brief  指定文字列に正規表現マッチを試行し，後方参照を含む結果を自身に格納
 *
 *  文字列src_strに対し，patで指定されたPOSIX 拡張正規表現(以下，正規表現)
 *  による文字列マッチを試行し，文字列配列として自身に格納します．
 *
 * @param      src_str マッチング対象の文字列
 * @param      pat 文字パターン(正規表現)
 * @return     自身の参照
 * @throw      regexルーチンがメモリを使い果たしている場合
 * @throw      内部バッファの確保に失敗した場合
 * @throw      メモリ破壊を起こした場合
 *
 */
tarray_tstring &tarray_tstring::regassign(const char *src_str, const tstring &pat)
{
    tstring one = src_str;
    if ( this->regex_rec.compile(pat.cstr()) < 0 ) {
	err_report1(__FUNCTION__,"ERROR","%s",this->regex_rec.cerrstr());
    }
    this->regexp_match_advanced(one, 0, this->regex_rec, NULL);
    return *this;
}

/**
 * @brief  指定文字列に正規表現マッチを試行し，後方参照を含む結果を自身に格納
 *
 *  文字列src_strに対し，patで指定されたPOSIX 拡張正規表現(以下，正規表現)
 *  による文字列マッチを試行し，文字列配列として自身に格納します．
 *
 * @param      src_str マッチング対象の文字列
 * @param      pos 文字列マッチの開始位置
 * @param      pat 文字パターン(正規表現)
 * @return     自身の参照
 * @throw      regexルーチンがメモリを使い果たしている場合
 * @throw      内部バッファの確保に失敗した場合
 * @throw      メモリ破壊を起こした場合
 *
 */
tarray_tstring &tarray_tstring::regassign( const char *src_str, size_t pos,
					   const tstring &pat )
{
    tstring one = src_str;
    if ( this->regex_rec.compile(pat.cstr()) < 0 ) {
	err_report1(__FUNCTION__,"ERROR","%s",this->regex_rec.cerrstr());
    }
    this->regexp_match_advanced(one, pos, this->regex_rec, NULL);
    return *this;
}

/**
 * @brief  指定文字列に正規表現マッチを試行し，後方参照を含む結果を自身に格納
 *
 *  文字列src_strに対し，patで指定されたPOSIX 拡張正規表現(以下，正規表現)
 *  による文字列マッチを試行し，文字列配列として自身に格納します．
 *
 * @param      src_str マッチング対象の文字列
 * @param      pos 文字列マッチの開始位置
 * @param      pat 文字パターン(正規表現)
 * @param      nextpos 次回のpos
 * @return     自身の参照
 * @throw      regexルーチンがメモリを使い果たしている場合
 * @throw      内部バッファの確保に失敗した場合
 * @throw      メモリ破壊を起こした場合
 *
 */
tarray_tstring &tarray_tstring::regassign( const char *src_str, size_t pos,
					  const tstring &pat, size_t *nextpos )
{
    tstring one = src_str;
    if ( this->regex_rec.compile(pat.cstr()) < 0 ) {
	err_report1(__FUNCTION__,"ERROR","%s",this->regex_rec.cerrstr());
    }
    this->regexp_match_advanced(one, pos, this->regex_rec, nextpos);
    return *this;
}

/**
 * @brief  指定文字列に正規表現マッチを試行し，後方参照を含む結果を自身に格納
 *
 *  文字列src_strに対し，patで指定されたPOSIX 拡張正規表現(以下，正規表現)
 *  による文字列マッチを試行し，文字列配列として自身に格納します．
 *
 * @param      src_str マッチング対象の文字列
 * @param      pat 文字パターン(正規表現)
 * @return     自身の参照
 * @throw      regexルーチンがメモリを使い果たしている場合
 * @throw      内部バッファの確保に失敗した場合
 * @throw      メモリ破壊を起こした場合
 *
 */
tarray_tstring &tarray_tstring::regassign( const tstring &src_str, 
					   const tstring &pat)
{
    if ( this->regex_rec.compile(pat.cstr()) < 0 ) {
	err_report1(__FUNCTION__,"ERROR","%s",this->regex_rec.cerrstr());
    }
    this->regexp_match_advanced(src_str, 0, this->regex_rec, NULL);
    return *this;
}

/**
 * @brief  指定文字列に正規表現マッチを試行し，後方参照を含む結果を自身に格納
 *
 *  文字列src_strに対し，patで指定されたPOSIX 拡張正規表現(以下，正規表現)
 *  による文字列マッチを試行し，文字列配列として自身に格納します．
 *
 * @param      src_str マッチング対象の文字列
 * @param      pos 文字列マッチの開始位置
 * @param      pat 文字パターン(正規表現)
 * @return     自身の参照
 * @throw      regexルーチンがメモリを使い果たしている場合
 * @throw      内部バッファの確保に失敗した場合
 * @throw      メモリ破壊を起こした場合
 *
 */
tarray_tstring &tarray_tstring::regassign( const tstring &src_str, size_t pos,
					   const tstring &pat )
{
    if ( this->regex_rec.compile(pat.cstr()) < 0 ) {
	err_report1(__FUNCTION__,"ERROR","%s",this->regex_rec.cerrstr());
    }
    this->regexp_match_advanced(src_str, pos, this->regex_rec, NULL);
    return *this;
}

/**
 * @brief  指定文字列に正規表現マッチを試行し，後方参照を含む結果を自身に格納
 *
 *  文字列src_strに対し，patで指定されたPOSIX 拡張正規表現(以下，正規表現)
 *  による文字列マッチを試行し，文字列配列として自身に格納します．
 *
 * @param      src_str マッチング対象の文字列
 * @param      pos 文字列マッチの開始位置
 * @param      pat 文字パターン(正規表現)
 * @param      nextpos 次回のpos
 * @return     自身の参照
 * @throw      regexルーチンがメモリを使い果たしている場合
 * @throw      内部バッファの確保に失敗した場合
 * @throw      メモリ破壊を起こした場合
 *
 */
tarray_tstring &tarray_tstring::regassign( const tstring &src_str, size_t pos,
					  const tstring &pat, size_t *nextpos )
{
    if ( this->regex_rec.compile(pat.cstr()) < 0 ) {
	err_report1(__FUNCTION__,"ERROR","%s",this->regex_rec.cerrstr());
    }
    this->regexp_match_advanced(src_str, pos, this->regex_rec, nextpos);
    return *this;
}

/**
 * @brief  指定文字列に正規表現マッチを試行し，後方参照を含む結果を自身に格納
 *
 *  文字列src_strに対し，patで指定されたPOSIX 拡張正規表現(以下，正規表現)
 *  による文字列マッチを試行し，文字列配列として自身に格納します．
 *
 * @param      src_str マッチング対象の文字列
 * @param      tregex クラスの正規表現コンパイル済オブジェクト
 * @return     自身の参照
 * @throw      regexルーチンがメモリを使い果たしている場合
 * @throw      内部バッファの確保に失敗した場合
 * @throw      メモリ破壊を起こした場合
 *
 */
tarray_tstring &tarray_tstring::regassign(const char *src_str, const tregex &pat)
{
    tstring one = src_str;
    this->regexp_match_advanced(one, 0, pat, NULL);
    return *this;
}

/**
 * @brief  指定文字列に正規表現マッチを試行し，後方参照を含む結果を自身に格納
 *
 *  文字列src_strに対し，patで指定されたPOSIX 拡張正規表現(以下，正規表現)
 *  による文字列マッチを試行し，文字列配列として自身に格納します．
 *
 * @param      src_str マッチング対象の文字列
 * @param      pos 文字列マッチの開始位置
 * @param      tregex クラスの正規表現コンパイル済オブジェクト
 * @return     自身の参照
 * @throw      regexルーチンがメモリを使い果たしている場合
 * @throw      内部バッファの確保に失敗した場合
 * @throw      メモリ破壊を起こした場合
 *
 */
tarray_tstring &tarray_tstring::regassign( const char *src_str, size_t pos,
					   const tregex &pat )
{
    tstring one = src_str;
    this->regexp_match_advanced(one, pos, pat, NULL);
    return *this;
}

/**
 * @brief  指定文字列に正規表現マッチを試行し，後方参照を含む結果を自身に格納
 *
 *  文字列src_strに対し，patで指定されたPOSIX 拡張正規表現(以下，正規表現)
 *  による文字列マッチを試行し，文字列配列として自身に格納します．
 *
 * @param      src_str マッチング対象の文字列
 * @param      pos 文字列マッチの開始位置
 * @param      tregex クラスの正規表現コンパイル済オブジェクト
 * @param      nextpos 次回のpos
 * @return     自身の参照
 * @throw      regexルーチンがメモリを使い果たしている場合
 * @throw      内部バッファの確保に失敗した場合
 * @throw      メモリ破壊を起こした場合
 *
 */
tarray_tstring &tarray_tstring::regassign( const char *src_str, size_t pos,
					   const tregex &pat, size_t *nextpos )
{
    tstring one = src_str;
    this->regexp_match_advanced(one, pos, pat, nextpos);
    return *this;
}

/**
 * @brief  指定文字列に正規表現マッチを試行し，後方参照を含む結果を自身に格納
 *
 *  文字列src_strに対し，patで指定されたPOSIX 拡張正規表現(以下，正規表現)
 *  による文字列マッチを試行し，文字列配列として自身に格納します．
 *
 * @param      src_str マッチング対象の文字列
 * @param      tregex クラスの正規表現コンパイル済オブジェクト
 * @return     自身の参照
 * @throw      regexルーチンがメモリを使い果たしている場合
 * @throw      内部バッファの確保に失敗した場合
 * @throw      メモリ破壊を起こした場合
 *
 */
tarray_tstring &tarray_tstring::regassign( const tstring &src_str,
					   const tregex &pat )
{
    this->regexp_match_advanced(src_str, 0, pat, NULL);
    return *this;
}

/**
 * @brief  指定文字列に正規表現マッチを試行し，後方参照を含む結果を自身に格納
 *
 *  文字列src_strに対し，patで指定されたPOSIX 拡張正規表現(以下，正規表現)
 *  による文字列マッチを試行し，文字列配列として自身に格納します．
 *
 * @param      src_str マッチング対象の文字列
 * @param      pos 文字列マッチの開始位置
 * @param      tregex クラスの正規表現コンパイル済オブジェクト
 * @return     自身の参照
 * @throw      regexルーチンがメモリを使い果たしている場合
 * @throw      内部バッファの確保に失敗した場合
 * @throw      メモリ破壊を起こした場合
 *
 */
tarray_tstring &tarray_tstring::regassign( const tstring &src_str, size_t pos,
					   const tregex &pat )
{
    this->regexp_match_advanced(src_str, pos, pat, NULL);
    return *this;
}

/**
 * @brief  指定文字列に正規表現マッチを試行し，後方参照を含む結果を自身に格納
 *
 *  文字列src_strに対し，patで指定されたPOSIX 拡張正規表現(以下，正規表現)
 *  による文字列マッチを試行し，文字列配列として自身に格納します．
 *
 * @param      src_str マッチング対象の文字列
 * @param      pos 文字列マッチの開始位置
 * @param      tregex クラスの正規表現コンパイル済オブジェクト
 * @param      nextpos 次回のpos
 * @return     自身の参照
 * @throw      regexルーチンがメモリを使い果たしている場合
 * @throw      内部バッファの確保に失敗した場合
 * @throw      メモリ破壊を起こした場合
 *
 */
tarray_tstring &tarray_tstring::regassign( const tstring &src_str, size_t pos,
					   const tregex &pat, size_t *nextpos )
{
    this->regexp_match_advanced(src_str, pos, pat, nextpos);
    return *this;
}

/**
 * @brief  コンパイル時にエラーを出力させるためのダミーのメンバ関数
 * @deprecated  使用不可．
 * @note	このメンバ関数はprivateです
 */
tarray_tstring &tarray_tstring::append( size_t n )
{
    return this->replace(this->length(),0, n);
}

/**
 * @brief  指定された文字列配列(const char *[])を，自身の配列の最後に追加
 *
 *  elementsで指定された文字列配列を，自身の配列の最後尾以降に追加します．
 *
 * @param      elements 要素に入る文字列のポインタ配列(NULL で終端)
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 *
 */
tarray_tstring &tarray_tstring::append( const char *const *elements )
{
    size_t i;
    if ( elements == NULL ) return this->append(elements, 0);
    for ( i=0 ; elements[i] != NULL ; i++ );
    return this->append(elements,i);
}

/**
 * @brief  文字列配列(const char *[])のn要素を，自身の配列の最後に追加
 *
 *  elementsで指定された文字列配列を，自身の配列の最後尾以降に追加します．
 *
 * @param      elements 要素に入る文字列のポインタ配列
 * @param      n 配列elements の要素数
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 *
 */
tarray_tstring &tarray_tstring::append( const char *const *elements, size_t n )
{
    return this->replace(this->length(), 0, elements, n);
}

/**
 * @brief  文字列配列(tarray_tstring)の(一部)要素を，自身の配列の最後に追加
 *
 *  srcで指定された文字列配列を，自身の配列の最後尾以降に追加します．
 *
 * @param     src 源泉となる文字列配列を持つtarray_tstring クラスのオブジェクト
 * @param     idx2 src 中の要素の開始位置
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合
 *
 */
tarray_tstring &tarray_tstring::append(const tarray_tstring &src, size_t idx2)
{
    return this->append(src, idx2, src.length());
}

/**
 * @brief  文字列配列(tarray_tstring)の(一部)要素を，自身の配列の最後に追加
 *
 *  srcで指定された文字列配列を，自身の配列の最後尾以降に追加します．
 *
 * @param    src 源泉となる文字列配列を持つtarray_tstring クラスのオブジェクト
 * @param    idx2 src中の要素の開始位置
 * @param    n2 src 中の要素の個数
 * @return   自身の参照
 * @throw    内部バッファの確保に失敗した場合
 *
 */
tarray_tstring &tarray_tstring::append( const tarray_tstring &src, 
					size_t idx2, size_t n2 )
{
    return this->replace(this->length(), 0, src, idx2, n2);
}

/**
 * @brief  自身の配列の最後に，指定された複数の文字列を追加
 *
 *  可変引数で指定された複数の文字列を，自身の配列の最後尾以降に追加します．
 *
 * @param      el0 源泉となる文字列(0 番目)
 * @param      el1 源泉となる文字列(1 番目)
 * @param      ... 源泉となる文字列(2 番目以降; 要NULL 終端)
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 *
 */
tarray_tstring &tarray_tstring::append( const char *el0, const char *el1, ... )
{
    va_list ap;
    va_start(ap, el1);
    try {
	this->vappend( el0, el1, ap );
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","this->vappend() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  自身の配列の最後に，指定された複数の文字列を追加
 *
 *  可変引数で指定された複数の文字列を，自身の配列の最後尾以降に追加します．
 *
 * @param      el0 源泉となる文字列(0 番目)
 * @param      el1 源泉となる文字列(1 番目)
 * @param      ap 源泉となる文字列の可変長引数のリスト(2 番目以降; 要NULL 終端)
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 *
 */
tarray_tstring &tarray_tstring::vappend( const char *el0, const char *el1,
					 va_list ap )
{
    heap_mem<const char *> tmp_ptbl;
    size_t element_count, i;
    va_list aq;

    va_copy(aq, ap);

    element_count = 0;
    if ( el0 != NULL ) {
        element_count++;
	if ( el1 != NULL ) {
	    element_count++;
	    while ( 1 ) {
	        const char *elem = va_arg(aq,char *);
		if ( elem == NULL ) break;
		element_count++;
	    }
	}
    }
    va_end(aq);

    if ( tmp_ptbl.allocate(element_count + 1) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","malloc() failed");
    }

    i = 0;
    if ( el0 != NULL ) {
        tmp_ptbl[i] = el0;
        i++;
	if ( el1 != NULL ) {
	    tmp_ptbl[i] = el1;
	    i++;
	    while ( 1 ) {
	        const char *elem = va_arg(ap,char *);
		if ( elem == NULL ) break;
		tmp_ptbl[i] = elem;
		i++;
	    }
	}
    }
    tmp_ptbl[i] = NULL;

    this->append(tmp_ptbl.ptr());

    return *this;
}

/**
 * @brief  自身の配列の最後に，指定された文字列(const char *)をn個追加
 *
 *  自身の配列の最後に，指定された文字列を持つ要素n個を追加します．
 *
 * @param      str 源泉となる文字列
 * @param      n 追加要素の個数
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 *
 */
tarray_tstring &tarray_tstring::append( const char *str, size_t n )
{
    return this->replace(this->length(), 0, str, n);
}

/**
 * @brief  自身の配列の最後に，指定された文字列(tstring)をn個追加
 *
 *  自身の配列の最後に，指定された文字列を持つ要素n個を追加します．
 *
 * @param      one 源泉となる文字列(マニュアルではstr)
 * @param      n 追加要素の個数
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 *
 */
tarray_tstring &tarray_tstring::append( const tstring &one, size_t n )
{
    return this->replace(this->length(), 0, one, n);
}

/**
 * @brief  自身の配列の最後に，指定された文字列(printf()の記法で指定)をn個追加
 *
 *  自身の配列の最後に，指定された文字列を持つ要素n個を追加します．
 *
 * @param      n 追加要素の個数
 * @param      format 源泉となる文字列のためのフォーマット指定
 * @param      ... formatに対応した可変長引数の各要素データ
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 * @throw      可変長引数の各要素データが指定された変換フォーマットで変換
 *             できない値の場合
 *
 */
tarray_tstring &tarray_tstring::appendf( size_t n, const char *format, ... )
{
    va_list ap;
    va_start(ap,format);
    try {
	this->vappendf(n,format,ap);
    }
    catch (...) {
	va_end(ap);
        err_throw(__FUNCTION__,"FATAL","this->vappendf() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  自身の配列の最後に，指定された文字列(printf()の記法で指定)をn個追加
 *
 *  自身の配列の最後に，指定された文字列を持つ要素n個を追加します．
 *
 * @param      n 追加要素の個数
 * @param      format 源泉となる文字列のためのフォーマット指定
 * @param      ... formatに対応した可変長引数の各要素データ
 * @param      ap formatに対応した可変長引数のリスト
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 * @throw      可変長引数の各要素データが指定された変換フォーマットで
 *             変換できない値の場合
 *
 */
tarray_tstring &tarray_tstring::vappendf(size_t n, const char *format, va_list ap)
{
    tstring one;

    if ( format == NULL ) return this->append(format,n);

    try {
	one.vprintf(format,ap);
    }
    catch (...) {
	err_throw(__FUNCTION__,"FATAL","one.vprintf() failed");
    }
    this->append(one,n);

    return *this;
}

/**
 * @brief  コンパイル時にエラーを出力させるためのダミーのメンバ関数
 * @deprecated  使用不可．
 * @note	このメンバ関数はprivateです
 */
tarray_tstring &tarray_tstring::insert( size_t index, size_t n )
{
    return this->replace(index,0, n);
}

/**
 * @brief  指定された文字列配列(const char *[])を，自身の配列の指定位置に挿入
 *
 *  elementsで指定された文字列配列を，自身の文字列配列の指定位置indexに
 *  挿入します．
 *
 * @param      index 自身の配列の挿入位置
 * @param      elements 源泉となる文字列のポインタ配列(NULL で終端)
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 *
 */
tarray_tstring &tarray_tstring::insert( size_t index, 
					const char *const *elements )
{
    size_t i;
    if ( elements == NULL ) return this->insert(index,elements, 0);
    for ( i=0 ; elements[i] != NULL ; i++ );
    return this->insert(index,elements,i);
}

/**
 * @brief  文字列配列(const char *[])のn要素を，自身の配列の指定位置に挿入
 *
 *  elementsで指定された文字列配列を，自身の文字列配列の指定位置indexに
 *  挿入します．
 *
 * @param      index 自身の配列の挿入位置
 * @param      elements 源泉となる文字列のポインタ配列
 * @param      n 配列elements の要素数
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 *
 */
tarray_tstring &tarray_tstring::insert( size_t index, 
					const char *const *elements, size_t n )
{
    return this->replace(index, 0, elements, n);
}

/**
 * @brief  文字列配列(tarray_tstring)の(一部)要素を，自身の配列の指定位置に挿入
 *
 *  srcで指定された文字列配列を，自身の文字列配列の指定位置indexに挿入します．
 *
 * @param     index 自身の配列の挿入位置
 * @param     src 源泉となる文字列配列を持つtarray_tstring クラスのオブジェクト
 * @param     idx2 src 中の要素の開始位置
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合
 *
 */
tarray_tstring &tarray_tstring::insert( size_t index,
					const tarray_tstring &src, size_t idx2)
{
    return this->insert(index, src, idx2, src.length());
}

/**
 * @brief  文字列配列(tarray_tstring)の(一部)要素を，自身の配列の指定位置に挿入
 *
 *  srcで指定された文字列配列を，自身の文字列配列の指定位置indexに挿入します．
 *
 * @param     index 自身の配列の挿入位置
 * @param     src 源泉となる文字列配列を持つtarray_tstring クラスのオブジェクト
 * @param     idx2 src 中の要素の開始位置
 * @param     n2 src 中の要素の個数
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合
 *
 */
tarray_tstring &tarray_tstring::insert( size_t index, 
					const tarray_tstring &src, 
					size_t idx2, size_t n2 )
{
    return this->replace(index, 0, src, idx2, n2);
}

/**
 * @brief  指定された複数の文字列を，自身の配列の指定位置に挿入
 *
 *  可変引数で指定された文字列配列を，自身の文字列配列の指定位置indexに
 *  挿入します．
 *
 * @param      index 自身の配列の挿入位置
 * @param      el0 源泉となる文字列(0 番目)
 * @param      el1 源泉となる文字列(1 番目)
 * @param      ... 源泉となる文字列(2 番目以降; 要NULL 終端)
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 *
 */
tarray_tstring &tarray_tstring::insert( size_t index, 
					const char *el0, const char *el1, ... )
{
    va_list ap;
    va_start(ap, el1);
    try {
	this->vinsert( index, el0, el1, ap );
    }
    catch (...) {
	va_end(ap);
        err_throw(__FUNCTION__,"FATAL","this->vinsert() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  指定された複数の文字列を，自身の配列の指定位置に挿入
 *
 *  可変引数で指定された文字列配列を，自身の文字列配列の指定位置indexに
 *  挿入します．
 *
 * @param      index 自身の配列の挿入位置
 * @param      el0 源泉となる文字列(0 番目)
 * @param      el1 源泉となる文字列(1 番目)
 * @param      ap 源泉となる文字列の可変長引数のリスト(2 番目以降; 要NULL 終端)
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 *
 */
tarray_tstring &tarray_tstring::vinsert( size_t index, 
				 const char *el0, const char *el1, va_list ap )
{
    heap_mem<const char *> tmp_ptbl;
    size_t element_count, i;
    va_list aq;

    va_copy(aq, ap);

    element_count = 0;
    if ( el0 != NULL ) {
        element_count++;
	if ( el1 != NULL ) {
	    element_count++;
	    while ( 1 ) {
	        const char *elem = va_arg(aq,char *);
		if ( elem == NULL ) break;
		element_count++;
	    }
	}
    }
    va_end(aq);

    if ( tmp_ptbl.allocate(element_count + 1) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","malloc() failed");
    }

    i = 0;
    if ( el0 != NULL ) {
        tmp_ptbl[i] = el0;
        i++;
	if ( el1 != NULL ) {
	    tmp_ptbl[i] = el1;
	    i++;
	    while ( 1 ) {
	        const char *elem = va_arg(ap,char *);
		if ( elem == NULL ) break;
		tmp_ptbl[i] = elem;
		i++;
	    }
	}
    }
    tmp_ptbl[i] = NULL;

    this->insert(index, tmp_ptbl.ptr());

    return *this;
}

/**
 * @brief  自身の配列の指定位置に，指定された文字列(const char *)をn個挿入
 *
 *  自身の文字列配列の要素番号indexの位置に，指定された文字列を持つ要素n個を
 *  挿入します．
 *
 * @param      index 自身の配列の挿入位置
 * @param      str 源泉となる文字列
 * @param      n 追加要素の個数
 * @return     自身の参照
 *
 */
tarray_tstring &tarray_tstring::insert( size_t index, 
					const char *str, size_t n )
{
    return this->replace(index, 0, str, n);
}

/**
 * @brief  自身の配列の指定位置に，指定された文字列(tstring)をn個挿入
 *
 *  自身の文字列配列の要素番号indexの位置に，指定された文字列を持つ要素n個を
 *  挿入します．
 *
 * @param      index 自身の配列の挿入位置
 * @param      one 源泉となる文字列(マニュアルではstr)
 * @param      n 追加要素の個数
 * @return     自身の参照
 *
 */
tarray_tstring &tarray_tstring::insert( size_t index, 
					const tstring &one, size_t n )
{
    return this->replace(index, 0, one, n);
}

/**
 * @brief  自身の配列の指定位置に，指定された文字列(printf()の記法)をn個挿入
 *
 *  自身の文字列配列の要素番号indexの位置に，指定された文字列を持つ要素n個を
 *  挿入します．
 *
 * @param      index 自身の配列の挿入位置
 * @param      n 追加要素の個数
 * @param      format 源泉となる文字列のためのフォーマット指定
 * @param      ... format に対応した可変長引数の各要素データ
 * @return     自身の参照
 *
 */
tarray_tstring &tarray_tstring::insertf( size_t index, 
					 size_t n, const char *format, ...)
{
    va_list ap;
    va_start(ap,format);
    try {
	this->vinsertf(index,n,format,ap);
    }
    catch (...) {
	va_end(ap);
        err_throw(__FUNCTION__,"FATAL","this->vinsertf() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  自身の配列の指定位置に，指定された文字列(printf()の記法)をn個挿入
 *
 *  自身の文字列配列の要素番号indexの位置に，指定された文字列を持つ要素n個を
 *  挿入します．
 *
 * @param      index 自身の配列の挿入位置
 * @param      n 追加要素の個数
 * @param      format 源泉となる文字列のためのフォーマット指定
 * @param      ap format に対応した可変長引数のリスト
 * @return     自身の参照
 *
 */
tarray_tstring &tarray_tstring::vinsertf( size_t index, 
					size_t n, const char *format, va_list ap )
{
    tstring one;

    if ( format == NULL ) return this->insert(index,format,n);

    try {
	one.vprintf(format,ap);
    }
    catch (...) {
	err_throw(__FUNCTION__,"FATAL","one.vprintf() failed");
    }
    this->insert(index,one,n);

    return *this;
}

/**
 * @brief  コンパイル時にエラーを出力させるためのダミーのメンバ関数
 * @deprecated  使用不可．
 * @note	このメンバ関数はprivateです 
 */
tarray_tstring &tarray_tstring::replace( size_t idx1, size_t n1, size_t n2 )
{
    static tstring one(true);

    this->arrs_rec.replace(idx1,n1, one,n2);

    this->update_cstr_ptrs_rec();

    return *this;
}

/**
 * @brief  配列の一部または全部を，指定された文字列配列(const char *[])で置換
 *
 *  自身の文字列配列の要素番号idx1からn1個の要素を，elementsで指定された
 *  文字列配列で置換します．
 *
 * @param      idx1 自身の配列の開始位置
 * @param      n1 置換される要素数
 * @param      elements 源泉となる文字列のポインタ配列(NULL で終端)
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 *
 */
tarray_tstring &tarray_tstring::replace( size_t idx1, size_t n1,
					 const char *const *elements )
{
    size_t i;
    if ( elements == NULL ) return this->replace(idx1,n1, elements,0);
    for ( i=0 ; elements[i] != NULL ; i++ );
    return this->replace(idx1,n1, elements,i);
}

/**
 * @brief  配列の一部または全部を，指定された文字列配列(const char *[])で置換
 *
 *  自身の文字列配列の要素番号idx1からn1個の要素を，elementsで指定された
 *  文字列配列で置換します．
 *
 * @param      idx1 自身の配列の開始位置
 * @param      n1 置換される要素数
 * @param      elements 源泉となる文字列のポインタ配列(NULL で終端)
 * @param      n2 配列elements の要素数，またはsrc 中の要素の個数
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 *
 */
tarray_tstring &tarray_tstring::replace( size_t idx1, size_t n1,
				       const char *const *elements, size_t n2 )
{
    size_t i;
    tarray<tstring> tmp_src;

    if ( 0 < n2 && elements == NULL ) return *this;

    /* n2 より小さい場合の対応 */
    for ( i=0 ; i < n2 && elements[i] != NULL ; i++ );
    n2 = i;

    /* elements 上の文字列に自身が管理している文字列が入っているかどうかは
       this->is_my_buffer(elements) では完全に調べられないので全部コピーする */
    if ( 0 < n2 ) {
	tmp_src.resize(n2);
	for ( i=0 ; i < n2 ; i++ ) tmp_src[i] = elements[i];
    }

    this->arrs_rec.replace(idx1,n1, tmp_src,0,n2);

    this->update_cstr_ptrs_rec();

    return *this;
}

/**
 * @brief  配列の一部または全部を，指定された文字列配列(tarray_tstring)で置換
 *
 *  自身の配列の要素の位置idx1からn1個の要素を，srcで指定された文字列配列で
 *  置換します．
 *
 * @param     idx1 自身の配列の開始位置
 * @param     n1 置換される要素数
 * @param     src 源泉となる文字列配列を持つtarray_tstring クラスのオブジェクト
 * @param     idx2 src 中の要素の開始位置
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合
 *
 */
tarray_tstring &tarray_tstring::replace( size_t idx1, size_t n1,
				       const tarray_tstring &src, size_t idx2 )
{
    return this->replace(idx1, n1, src, idx2, src.length());
}

/**
 * @brief  配列の一部または全部を，指定された文字列配列(tarray_tstring)で置換
 *
 *  自身の配列の要素の位置idx1からn1個の要素を，srcで指定された文字列配列で
 *  置換します．
 *
 * @param     idx1 自身の配列の開始位置
 * @param     n1 置換される要素数
 * @param     src 源泉となる文字列配列を持つtarray_tstring クラスのオブジェクト
 * @param     idx2 src中の要素の開始位置
 * @param     n2 src中の要素の個数
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合
 *
 */
tarray_tstring &tarray_tstring::replace( size_t idx1, size_t n1,
					 const tarray_tstring &src, 
					 size_t idx2, size_t n2 )
{
    this->arrs_rec.replace(idx1,n1, src.arrs_rec,idx2,n2);

    this->update_cstr_ptrs_rec();

    return *this;
}

/**
 * @brief  配列の一部または全部を，指定された複数の文字列で置換
 *
 *  自身の配列の要素の位置idx1からn1個の要素を，可変引数で指定された
 *  複数の文字列で指定された文字列配列で置換します．
 *
 * @param      idx1 自身の配列の開始位置
 * @param      n1 置換される要素数
 * @param      el0 源泉となる文字列(0 番目)
 * @param      el1 源泉となる文字列(1 番目)
 * @param      ... 源泉となる文字列(2 番目以降; 要NULL 終端)
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 *
 */
tarray_tstring &tarray_tstring::replace( size_t idx1, size_t n1, 
					const char *el0, const char *el1, ... )
{
    va_list ap;
    va_start(ap, el1);
    try {
	this->vreplace( idx1, n1, el0, el1, ap );
    }
    catch (...) {
	va_end(ap);
        err_throw(__FUNCTION__,"FATAL","this->vreplace() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  配列の一部または全部を，指定された複数の文字列で置換
 *
 *  自身の配列の要素の位置idx1からn1個の要素を，可変引数で指定された
 *  複数の文字列で指定された文字列配列で置換します．
 *
 * @param      idx1 自身の配列の開始位置
 * @param      n1 置換される要素数
 * @param      el0 源泉となる文字列(0 番目)
 * @param      el1 源泉となる文字列(1 番目)
 * @param      ap 源泉となる文字列の可変長引数のリスト(2 番目以降; 要NULL 終端)
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 *
 */
tarray_tstring &tarray_tstring::vreplace( size_t idx1, size_t n1, 
				 const char *el0, const char *el1, va_list ap )
{
    heap_mem<const char *> tmp_ptbl;
    size_t element_count, i;
    va_list aq;

    va_copy(aq, ap);

    element_count = 0;
    if ( el0 != NULL ) {
        element_count++;
	if ( el1 != NULL ) {
	    element_count++;
	    while ( 1 ) {
	        const char *elem = va_arg(aq,char *);
		if ( elem == NULL ) break;
		element_count++;
	    }
	}
    }
    va_end(aq);

    if ( tmp_ptbl.allocate(element_count + 1) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","malloc() failed");
    }

    i = 0;
    if ( el0 != NULL ) {
        tmp_ptbl[i] = el0;
        i++;
	if ( el1 != NULL ) {
	    tmp_ptbl[i] = el1;
	    i++;
	    while ( 1 ) {
	        const char *elem = va_arg(ap,char *);
		if ( elem == NULL ) break;
		tmp_ptbl[i] = elem;
		i++;
	    }
	}
    }
    tmp_ptbl[i] = NULL;

    this->replace(idx1,n1, tmp_ptbl.ptr());

    return *this;
}

/**
 * @brief  配列の一部または全部を，指定された文字列(const char *)で置換
 *
 *  自身の配列の要素の位置idx1からn1個の要素を，指定された文字列を持つ要素
 *  n2個で置換します．
 *
 * @param      idx1 自身の配列の開始位置
 * @param      n1 置換される要素数
 * @param      str 源泉となる文字列
 * @param      n2 指定された文字列が代入される要素の個数
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 *
 */
tarray_tstring &tarray_tstring::replace( size_t idx1, size_t n1, 
					 const char *str, size_t n2 )
{
    tstring one = str;
    return this->replace(idx1,n1, one,n2);
}

/**
 * @brief  配列の一部または全部を，指定された文字列(tstring)で置換
 *
 *  自身の配列の要素の位置idx1からn1個の要素を，指定された文字列を持つ要素
 *  n2個で置換します．
 *
 * @param      idx1 自身の配列の開始位置
 * @param      n1 置換される要素数
 * @param      one 源泉となる文字列(マニュアルではstr)
 * @param      n2 指定された文字列が代入される要素の個数
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 *
 */
tarray_tstring &tarray_tstring::replace( size_t idx1, size_t n1, 
					 const tstring &one, size_t n2 )
{
    if ( one.cstr() != NULL ) {
	this->arrs_rec.replace(idx1,n1, one,n2);

	this->update_cstr_ptrs_rec();
    }
    return *this;
}

/**
 * @brief  配列の一部または全部を，指定された文字列(printf()の記法)で置換
 *
 *  自身の配列の要素の位置idx1からn1個の要素を，指定された文字列を持つ要素
 *  n2個で置換します．
 *
 * @param      idx1 自身の配列の開始位置
 * @param      n1 置換される要素数
 * @param      n2 指定された文字列が代入される要素の個数
 * @param      format 源泉となる文字列のためのフォーマット指定
 * @param      ... formatに対応した可変長引数の各要素データ
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 * @throw      可変長引数の各要素データが，指定された変換フォーマットで
 *             変換できない値の場合
 *
 */
tarray_tstring &tarray_tstring::replacef( size_t idx1, size_t n1, 
					  size_t n2, const char *format, ...)
{
    va_list ap;
    va_start(ap,format);
    try {
	this->vreplacef(idx1,n1,n2,format,ap);
    }
    catch (...) {
	va_end(ap);
        err_throw(__FUNCTION__,"FATAL","this->vreplacef() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  配列の一部または全部を，指定された文字列(printf()の記法)で置換
 *
 *  自身の配列の要素の位置idx1からn1個の要素を，指定された文字列を持つ要素
 *  n2個で置換します．
 *
 * @param      idx1 自身の配列の開始位置
 * @param      n1 置換される要素数
 * @param      n2 指定された文字列が代入される要素の個数
 * @param      format 源泉となる文字列のためのフォーマット指定
 * @param      ap formatに対応した可変長引数のリスト
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 * @throw      可変長引数の各要素データが，指定された変換フォーマットで
 *             変換できない値の場合
 *
 */
tarray_tstring &tarray_tstring::vreplacef( size_t idx1, size_t n1, size_t n2,
					   const char *format, va_list ap )
{
    tstring one;

    if ( format == NULL ) return this->replace(idx1,n1,format,n2);

    try {
	one.vprintf(format,ap);
    }
    catch (...) {
	err_throw(__FUNCTION__,"FATAL","one.vprintf() failed");
    }
    this->replace(idx1,n1,one,n2);

    return *this;
}

/**
 * @brief  コンパイル時にエラーを出力させるためのダミーのメンバ関数
 * @deprecated  使用不可．
 * @note	このメンバ関数はprivateです
 */
tarray_tstring &tarray_tstring::put( size_t index, size_t n )
{
    static tstring one(true);

    this->arrs_rec.put(index, one,n);

    this->update_cstr_ptrs_rec();

    return *this;
}

/**
 * @brief  指定された文字列配列(const char *[])を，自身の配列の指定位置に上書き
 *
 *  elementsで指定された文字列配列を，自身の配列の要素位置indexから上書き
 *  します．
 *
 * @param      index 自身の配列の書き込み位置
 * @param      elements 源泉となる文字列のポインタ配列(NULLで終端)
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 * 
 */
tarray_tstring &tarray_tstring::put( size_t index, 
				     const char *const *elements )
{
    size_t i;
    if ( elements == NULL ) return this->put(index,elements, 0);
    for ( i=0 ; elements[i] != NULL ; i++ );
    return this->put(index,elements,i);
}

/**
 * @brief  指定された文字列配列(const char *[])を，自身の配列の指定位置に上書き
 *
 *  elementsで指定された文字列配列を，自身の配列の要素位置indexから上書き
 *  します．
 *
 * @param      index 自身の配列の書き込み位置
 * @param      elements 源泉となる文字列のポインタ配列
 * @param      n 配列elements の要素数
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 * 
 */
tarray_tstring &tarray_tstring::put( size_t index, 
				     const char *const *elements, size_t n )
{
    heap_mem<const char *> tmp_ptbl;
    size_t i;

    if ( 0 < n && elements == NULL ) return *this;

    /* n より小さい場合の対応 */
    for ( i=0 ; i < n && elements[i] != NULL ; i++ );
    n = i;

    if ( 0 < n && this->is_my_buffer(elements) == true ) {
	/* elements が this->cstr_ptrs_rec 由来の場合は
	   アドレステーブルをコピーする (step1 でのリサイズのため) */
	if ( tmp_ptbl.allocate(n) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","malloc() failed");
	}
	tmp_ptbl.copyfrom(elements, n);
	elements = tmp_ptbl.ptr();
    }

    if ( this->length() < index + n ) {
	this->replace(this->length(),0, (index + n) - this->length());
    }
    this->replace(index, n, elements, n);

    return *this;
}

/**
 * @brief  指定された文字列配列(tarray_tstring)を，自身の配列の指定位置に上書き
 *
 *  srcで指定された文字列配列を，自身の配列の要素位置indexから上書きします．
 *
 * @param    index 自身の配列の書き込み位置
 * @param    src 源泉となる文字列配列を持つtarray_tstring クラスのオブジェクト
 * @param    idx2 src中の要素の開始位置
 * @return   自身の参照
 * @throw    内部バッファの確保に失敗した場合
 * 
 */
tarray_tstring &tarray_tstring::put( size_t index,
				     const tarray_tstring &src, size_t idx2)
{
    return this->put(index, src, idx2, src.length());
}

/**
 * @brief  指定された文字列配列(tarray_tstring)を，自身の配列の指定位置に上書き
 *
 *  srcで指定された文字列配列を，自身の配列の要素位置indexから上書きします．
 *
 * @param    index 自身の配列の書き込み位置
 * @param    src 源泉となる文字列配列を持つtarray_tstring クラスのオブジェクト
 * @param    idx2 src中の要素の開始位置
 * @param    n2 src 中の要素の個数
 * @return   自身の参照
 * @throw    内部バッファの確保に失敗した場合
 * 
 */
tarray_tstring &tarray_tstring::put( size_t index, 
				     const tarray_tstring &src, 
				     size_t idx2, size_t n2 )
{
    this->arrs_rec.put(index, src.arrs_rec,idx2,n2);

    this->update_cstr_ptrs_rec();

    return *this;
}

/**
 * @brief  指定された複数の文字列を，自身の配列の指定位置に上書き
 *
 *  可変引数で指定された複数の文字列を，自身の配列の要素位置 index から
 *  上書きします．
 *
 * @param      idx1 自身の配列の書き込み位置
 * @param      el0 源泉となる文字列(0 番目)
 * @param      el1 源泉となる文字列(1 番目)
 * @param      ... 源泉となる文字列(2 番目以降; 要NULL 終端)
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 *
 */
tarray_tstring &tarray_tstring::put( size_t index, 
				     const char *el0, const char *el1, ... )
{
    va_list ap;
    va_start(ap, el1);
    try {
	this->vput( index, el0, el1, ap );
    }
    catch (...) {
	va_end(ap);
        err_throw(__FUNCTION__,"FATAL","this->vput() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  指定された複数の文字列を，自身の配列の指定位置に上書き
 *
 *  可変引数で指定された複数の文字列を，自身の配列の要素位置 index から
 *  上書きします．
 *
 * @param      idx1 自身の配列の書き込み位置
 * @param      el0 源泉となる文字列(0 番目)
 * @param      el1 源泉となる文字列(1 番目)
 * @param      ap 源泉となる文字列の可変長引数のリスト(2 番目以降; 要NULL 終端)
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 *
 */
tarray_tstring &tarray_tstring::vput( size_t index, 
				 const char *el0, const char *el1, va_list ap )
{
    heap_mem<const char *> tmp_ptbl;
    size_t element_count, i;
    va_list aq;

    va_copy(aq, ap);

    element_count = 0;
    if ( el0 != NULL ) {
        element_count++;
	if ( el1 != NULL ) {
	    element_count++;
	    while ( 1 ) {
	        const char *elem = va_arg(aq,char *);
		if ( elem == NULL ) break;
		element_count++;
	    }
	}
    }
    va_end(aq);

    if ( tmp_ptbl.allocate(element_count + 1) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","malloc() failed");
    }

    i = 0;
    if ( el0 != NULL ) {
        tmp_ptbl[i] = el0;
        i++;
	if ( el1 != NULL ) {
	    tmp_ptbl[i] = el1;
	    i++;
	    while ( 1 ) {
	        const char *elem = va_arg(ap,char *);
		if ( elem == NULL ) break;
		tmp_ptbl[i] = elem;
		i++;
	    }
	}
    }
    tmp_ptbl[i] = NULL;

    this->put(index, tmp_ptbl.ptr());

    return *this;
}

/**
 * @brief  指定された文字列(const char *)で，自身の配列の指定位置からn個上書き
 *
 *  自身の配列の要素番号indexの位置から，指定された文字列を持つ要素n個で
 *  上書きします．
 *
 * @param      index 自身の配列の書き込み位置
 * @param      str 源泉となる文字列
 * @param      n 要素の個数
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 *
 */
tarray_tstring &tarray_tstring::put( size_t index, const char *str, size_t n )
{
    tstring one = str;
    return this->put(index, one, n);
}

/**
 * @brief  指定された文字列(tstring)で，自身の配列の指定位置からn個上書き
 *
 *  自身の配列の要素番号indexの位置から，指定された文字列を持つ要素n個で
 *  上書きします．
 *
 * @param      index 自身の配列の書き込み位置
 * @param      one 源泉となる文字列(マニュアルではstr)
 * @param      n 要素の個数
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 *
 */
tarray_tstring &tarray_tstring::put(size_t index, const tstring &one, size_t n)
{
    if ( one.cstr() != NULL ) {
	this->arrs_rec.put(index, one,n);

	this->update_cstr_ptrs_rec();
    }
    return *this;
}

/**
 * @brief  指定された文字列(printf()の記法)で，配列の指定位置からn個上書き
 *
 *  自身の配列の要素番号indexの位置から，指定された文字列を持つ要素n個で
 *  上書きします．
 *
 * @param      index 自身の配列の書き込み位置
 * @param      n 要素の個数
 * @param      format 源泉となる文字列のためのフォーマット指定
 * @param      ... formatに対応した可変長引数の各要素データ
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 * @throw      可変長引数の各要素データが，指定された変換フォーマットで変換
 *             できない値の場合
 *
 */
tarray_tstring &tarray_tstring::putf( size_t index, 
				      size_t n, const char *format, ...)
{
    va_list ap;
    va_start(ap,format);
    try {
	this->vputf(index,n,format,ap);
    }
    catch (...) {
	va_end(ap);
        err_throw(__FUNCTION__,"FATAL","this->vputf() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  指定された文字列(printf()の記法)で，配列の指定位置からn個上書き
 *
 *  自身の配列の要素番号indexの位置から，指定された文字列を持つ要素n個で
 *  上書きします．
 *
 * @param      index 自身の配列の書き込み位置
 * @param      n 要素の個数
 * @param      format 源泉となる文字列のためのフォーマット指定
 * @param      ap format に対応した可変長引数のリスト
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 * @throw      可変長引数の各要素データが，指定された変換フォーマットで変換
 *             できない値の場合
 *
 */
tarray_tstring &tarray_tstring::vputf( size_t index, size_t n,
				       const char *format, va_list ap )
{
    tstring one;

    if ( format == NULL ) return this->put(index,format,n);

    try {
	one.vprintf(format,ap);
    }
    catch (...) {
	err_throw(__FUNCTION__,"FATAL","one.vprintf() failed");
    }
    this->put(index,one,n);

    return *this;
}

/**
 * @brief  配列の不要部分の消去
 *
 *  自身の配列を，要素番号 idx から len 個の要素だけにします．
 *
 * @param      idx 切り出し要素の開始位置
 * @param      len 要素の個数
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 * 
 */
tarray_tstring &tarray_tstring::crop( size_t idx, size_t len )
{
    size_t max_n;
    if ( this->length() < idx ) idx = this->length();
    max_n = this->length() - idx;
    if ( max_n < len ) len = max_n;
    this->erase(0, idx);
    this->erase(len, this->length() - len);
    return *this;
}

/**
 * @brief  配列の不要部分の消去
 *
 *  自身の配列を，要素番号idx 以降の配列だけにします．
 *
 * @param      idx 切り出し要素の開始位置
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 * 
 */
tarray_tstring &tarray_tstring::crop( size_t idx )
{
    if ( this->length() < idx ) idx = this->length();
    return this->crop(idx, this->length() - idx);
}

/**
 * @brief  全配列要素の削除
 *
 *  自身が持つ文字列配列の全要素を削除します．
 *
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 * 
 */
tarray_tstring &tarray_tstring::erase()
{
    return this->erase(0,this->length());
}

/**
 * @brief  配列要素の削除
 *
 *  自身が持つ文字列配列の要素を削除します．
 *
 * @param      index 要素番号
 * @param      num_elements 要素の個数
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 * 
 */
tarray_tstring &tarray_tstring::erase( size_t index, size_t num_elements )
{
    return this->replace(index,num_elements, (size_t)0);
}

/**
 * @brief  配列長の変更
 *
 *  自身の文字列配列の長さを new_num_elements に変更します．
 *
 * @param      new_num_elements 変更後の文字列配列長
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 * 
 */
tarray_tstring &tarray_tstring::resize( size_t new_num_elements )
{
    if ( new_num_elements < this->length() ) {
        this->replace( new_num_elements, 
		       this->length() - new_num_elements,
		       (size_t)0 );
    }
    else {
	this->replace( this->length(), (size_t)0,
		       new_num_elements - this->length() );
    }
    return *this;
}

/**
 * @brief  配列長の相対的な変更
 *
 *  自身の文字列配列の長さを len の長さ分だけ変更します．
 *
 * @param      len 配列長の増分・減分
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 * 
 */
tarray_tstring &tarray_tstring::resizeby( ssize_t len )
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

/**
 * @brief  一部要素を別オブジェクトへコピー
 *
 *  自身の文字列配列のすべてまたは一部を，dest で指定されたオブジェクトに
 *  コピーします．
 *
 * @param      index コピー元(自身)の配列要素の位置
 * @param      n コピーする要素数
 * @param      dest コピー先のtarray_tstring クラスのオブジェクト
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 * @throw      内部バッファの確保に失敗した場合
 *
 */
ssize_t tarray_tstring::copy( size_t index, size_t n,
			      tarray_tstring *dest ) const
{
    if ( dest == NULL ) return -1;

    size_t len = this->length();

    if ( len < index ) {
	dest->erase();
	return -1;
    }
    if ( this->cstrarray() == NULL ) {
	dest->erase();
	return 0;
    }

    if ( len - index < n ) n = len - index;

    dest->replace(0, dest->length(), *this, index, n);

    return n;
}

/**
 * @brief  一部要素を別オブジェクトへコピー
 *
 *  自身の文字列配列のすべてまたは一部を，dest で指定されたオブジェクトに
 *  コピーします．
 *
 * @param      index コピー元(自身)の配列要素の位置
 * @param      dest コピー先のtarray_tstring クラスのオブジェクト
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 * @throw      内部バッファの確保に失敗した場合
 *
 */
ssize_t tarray_tstring::copy( size_t index, tarray_tstring *dest ) const
{
    if ( dest == NULL ) return -1;
    return this->copy(index, this->length(), dest);
}

/**
 * @brief  自身の内容を別オブジェクトへコピー
 *
 *  自身の文字列配列のすべてを，dest で指定されたオブジェクトにコピーします．
 *
 * @param      dest コピー先のtarray_tstring クラスのオブジェクト
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 * @throw      内部バッファの確保に失敗した場合
 *
 */
ssize_t tarray_tstring::copy( tarray_tstring *dest ) const
{
    if ( dest == NULL ) return -1;
    return this->copy(0, this->length(), dest);
}

/**
 * @brief  一部要素を別オブジェクトへコピー (非推奨)
 * @deprecated 非推奨．<br>
 * ssize_t tarray_tstring::copy(size_t, size_t, tarray_tstring *) const <br>
 * をお使いください．
 */
ssize_t tarray_tstring::copy( size_t index, size_t n, 
			      tarray_tstring &dest ) const
{
    return this->copy(index, n, &dest);
}

/**
 * @brief  一部要素を別オブジェクトへコピー (非推奨)
 * @deprecated 非推奨．<br>
 * ssize_t tarray_tstring::copy(size_t, tarray_tstring *) const <br>
 * をお使いください．
 */
ssize_t tarray_tstring::copy( size_t index, tarray_tstring &dest ) const
{
    return this->copy(index, this->length(), &dest);
}

/**
 * @brief  自身の内容を別オブジェクトへコピー (非推奨)
 * @deprecated 非推奨．<br>
 * ssize_t tarray_tstring::copy(tarray_tstring *) const <br>
 * をお使いください．
 */
ssize_t tarray_tstring::copy( tarray_tstring &dest ) const
{
    return this->copy(0, this->length(), &dest);
}

/**
 * @brief  2つのオブジェクト間での内容のスワップ
 *
 *  オブジェクトsobj の内容と自身の内容とを入れ替えます．
 *
 * @param      sobj 内容を入れ替える tarray_tstring クラスのオブジェクト
 * @return     自身の参照
 * 
 */
tarray_tstring &tarray_tstring::swap( tarray_tstring &sobj )
{
    if ( &sobj == this ) return *this;

    this->arrs_rec.swap(sobj.arrs_rec);
    this->cstr_ptrs_rec.swap(sobj.cstr_ptrs_rec);
    this->reg_pos_rec.swap(sobj.reg_pos_rec);
    this->reg_length_rec.swap(sobj.reg_length_rec);
    this->regex_rec.swap(sobj.regex_rec);

    return *this;
}

/**
 * @brief  配列の全要素についての単純な文字列検索と置換
 *
 *  自身の文字列配列の全要素について，文字列の左側から文字列 org_str を検索し，
 *  見つかった場合は文字列 new_str で置き換えます．
 *
 * @param      org_str 検出する文字列
 * @param      new_str 置換の源泉となる文字列
 * @param      all 全置換のフラグ
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 *
 */
tarray_tstring &tarray_tstring::strreplace( const char *org_str,
					    const char *new_str, bool all )
{
    size_t i;
    for ( i=0 ; i < this->length() ; i++ ) {
	this->at(i).strreplace(org_str,new_str,all);
    }
    return *this;
}

/**
 * @brief  配列の全要素についての単純な文字列検索と置換
 *
 *  自身の文字列配列の全要素について，文字列の左側から文字列 org_str を検索し，
 *  見つかった場合は文字列 new_str で置き換えます．
 *
 * @param      org_str 検出する文字列
 * @param      new_str 置換の源泉となる文字列
 * @param      all 全置換のフラグ
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 *
 */
tarray_tstring &tarray_tstring::strreplace( const tstring &org_str,
					    const char *new_str, bool all )
{
    return this->strreplace(org_str.cstr(), new_str, all);
}

/**
 * @brief  配列の全要素についての単純な文字列検索と置換
 *
 *  自身の文字列配列の全要素について，文字列の左側から文字列 org_str を検索し，
 *  見つかった場合は文字列 new_str で置き換えます．
 *
 * @param      org_str 検出する文字列
 * @param      new_str 置換の源泉となる文字列
 * @param      all 全置換のフラグ
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 *
 */
tarray_tstring &tarray_tstring::strreplace( const char *org_str,
					    const tstring &new_str, bool all )
{
    return this->strreplace(org_str, new_str.cstr(), all);
}

/**
 * @brief  配列の全要素についての単純な文字列検索と置換
 *
 *  自身の文字列配列の全要素について，文字列の左側から文字列 org_str を検索し，
 *  見つかった場合は文字列 new_str で置き換えます．
 *
 * @param      org_str 検出する文字列
 * @param      new_str 置換の源泉となる文字列
 * @param      all 全置換のフラグ
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 *
 */
tarray_tstring &tarray_tstring::strreplace( const tstring &org_str,
					    const tstring &new_str, bool all )
{
    return this->strreplace(org_str.cstr(), new_str.cstr(), all);
}

/**
 * @brief  配列の全要素についての末尾文字の除去
 *
 *  自身が持つ全要素の末尾の文字列を除去します．
 * 
 * @return 自身の参照
 * @throw 内部バッファの確保に失敗した場合
 */
tarray_tstring &tarray_tstring::chop()
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
 *  自身の文字列配列の全要素の右端の改行文字を除去します．
 *
 * @param      rs 改行文字列 (省略可)
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 *
 */
tarray_tstring &tarray_tstring::chomp( const char *rs )
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
 *  自身の文字列配列の全要素の右端の改行文字を除去します．
 *
 * @param      rs 改行文字列
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 *
 */
tarray_tstring &tarray_tstring::chomp( const tstring &rs )
{
    return this->chomp(rs.cstr());
}

/**
 * @brief  配列の全要素についての文字列両端の不要文字の除去
 *
 *  自身の文字列配列の全要素について，文字列両端にある任意文字を除去します．
 *
 * @param      side_spaces 除去対象の文字セット．[A-Z] といった表現も使用可能．
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 *
 */
tarray_tstring &tarray_tstring::trim( const char *side_spaces )
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
 *  自身の文字列配列の全要素について，文字列両端にある任意文字を除去します．
 *
 * @param      side_spaces 除去対象の文字セット．[A-Z] といった表現も使用可能．
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 *
 */
tarray_tstring &tarray_tstring::trim( const tstring &side_spaces )
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
 *  自身の文字列配列の全要素について，文字列両端にある任意文字を除去します．
 *
 * @param      side_space 除去対象の文字
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 *
 */
tarray_tstring &tarray_tstring::trim( int side_space )
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
 *  自身の文字列配列の全要素について，文字列左端にある任意文字を除去します．
 *
 * @param      side_spaces 除去対象の文字セット．[A-Z] といった表現も使用可能．
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 *
 */
tarray_tstring &tarray_tstring::ltrim( const char *side_spaces )
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
 *  自身の文字列配列の全要素について，文字列左端にある任意文字を除去します．
 *
 * @param      side_spaces 除去対象の文字セット．[A-Z] といった表現も使用可能．
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 *
 */
tarray_tstring &tarray_tstring::ltrim( const tstring &side_spaces )
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
 *  自身の文字列配列の全要素について，文字列左端にある任意文字を除去します．
 *
 * @param      side_space 除去対象の文字
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 *
 */
tarray_tstring &tarray_tstring::ltrim( int side_space )
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
 *  自身の文字列配列の全要素について，文字列右端にある任意文字を除去します．
 *
 * @param      side_spaces 除去対象の文字セット．[A-Z] といった表現も使用可能．
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 *
 */
tarray_tstring &tarray_tstring::rtrim( const char *side_spaces )
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
 *  自身の文字列配列の全要素について，文字列右端にある任意文字を除去します．
 *
 * @param      side_spaces 除去対象の文字セット．[A-Z] といった表現も使用可能．
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 *
 */
tarray_tstring &tarray_tstring::rtrim( const tstring &side_spaces )
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
 *  自身の文字列配列の全要素について，文字列右端にある任意文字を除去します．
 *
 * @param      side_space 除去対象の文字
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 *
 */
tarray_tstring &tarray_tstring::rtrim( int side_space )
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
 *  自身の文字列配列の全要素について，文字列両端にある任意文字を除去します．
 *
 * @param      side_spaces 除去対象の文字セット．[A-Z] といった表現も使用可能．
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 *
 */
tarray_tstring &tarray_tstring::strtrim( const char *side_spaces )
{
    return this->trim(side_spaces);
}

/**
 * @brief  配列の全要素についての文字列両端の不要文字の除去
 *
 *  自身の文字列配列の全要素について，文字列両端にある任意文字を除去します．
 *
 * @param      side_spaces 除去対象の文字セット．[A-Z] といった表現も使用可能．
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 *
 */
tarray_tstring &tarray_tstring::strtrim( const tstring &side_spaces )
{
    return this->trim(side_spaces);
}

/**
 * @brief  配列の全要素についての文字列両端の不要文字の除去
 *
 *  自身の文字列配列の全要素について，文字列両端にある任意文字を除去します．
 *
 * @param      side_space 除去対象の文字
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 *
 */
tarray_tstring &tarray_tstring::strtrim( int side_space )
{
    return this->trim(side_space);
}

/**
 * @brief  配列の全要素について大文字を小文字に変換
 *
 *  自身の文字列配列の全要素のアルファベットの大文字を小文字に変換します．
 *
 * @return     自身の参照
 *
 */
tarray_tstring &tarray_tstring::tolower()
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
 *  自身の文字列配列の全要素のアルファベットの小文字を大文字に変換します．
 *
 * @return     自身の参照
 *
 */
tarray_tstring &tarray_tstring::toupper()
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
 *  自身の文字列配列の全要素について，水平タブ文字 '\t' を，tab_width の値に
 *  桁揃えをして空白文字に置換します．
 *
 * @param      tab_width タブ幅．省略時は8
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 *
 */
tarray_tstring &tarray_tstring::expand_tabs( size_t tab_width )
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
 * @param      tab_width タブ幅．省略時は8
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 *
 */
tarray_tstring &tarray_tstring::contract_spaces( size_t tab_width )
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
 *  自身の文字列配列の全要素について，pat で指定された POSIX 拡張正規表現
 *  (以下，正規表現) でマッチした部分を文字列 new_str で置き換えます．
 *
 * @param      pat 文字パターン(正規表現)
 * @param      new_str 置換後の文字列
 * @param      all 全置換のフラグ
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 *
 */
tarray_tstring &tarray_tstring::regreplace( const char *pat, 
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
 *  自身の文字列配列の全要素について，pat で指定された POSIX 拡張正規表現
 *  (以下，正規表現) でマッチした部分を文字列 new_str で置き換えます．
 *
 * @param      pat 文字パターン(正規表現)
 * @param      new_str 置換後の文字列
 * @param      all 全置換のフラグ
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 *
 */
tarray_tstring &tarray_tstring::regreplace( const tstring &pat, 
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
 *  自身の文字列配列の全要素について，pat で指定された POSIX 拡張正規表現
 *  (以下，正規表現) でマッチした部分を文字列 new_str で置き換えます．
 *
 * @param      pat tregex クラスのコンパイル済オブジェクト
 * @param      new_str 置換後の文字列
 * @param      all 全置換のフラグ
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 *
 */
tarray_tstring &tarray_tstring::regreplace( const tregex &pat, 
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
 *  自身の文字列配列の全要素について，pat で指定された POSIX 拡張正規表現
 *  (以下，正規表現) でマッチした部分を文字列 new_str で置き換えます．
 *
 * @param      pat 文字パターン(正規表現)
 * @param      new_str 置換後の文字列
 * @param      all 全置換のフラグ
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 *
 */
tarray_tstring &tarray_tstring::regreplace( const char *pat, 
					    const tstring &new_str, bool all )
{
    return this->regreplace(pat, new_str.cstr(), all);
}

/**
 * @brief  配列の全要素についての正規表現による置換
 *
 *  自身の文字列配列の全要素について，pat で指定された POSIX 拡張正規表現
 *  (以下，正規表現) でマッチした部分を文字列 new_str で置き換えます．
 *
 * @param      pat 文字パターン(正規表現)
 * @param      new_str 置換後の文字列
 * @param      all 全置換のフラグ
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 *
 */
tarray_tstring &tarray_tstring::regreplace( const tstring &pat, 
					    const tstring &new_str, bool all )
{
    return this->regreplace(pat, new_str.cstr(), all);
}

/**
 * @brief  配列の全要素についての正規表現による置換
 *
 *  自身の文字列配列の全要素について，pat で指定された POSIX 拡張正規表現
 *  (以下，正規表現) でマッチした部分を文字列 new_str で置き換えます．
 *
 * @param      pat tregex クラスのコンパイル済オブジェクト
 * @param      new_str 置換後の文字列
 * @param      all 全置換のフラグ
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 *
 */
tarray_tstring &tarray_tstring::regreplace( const tregex &pat, 
					    const tstring &new_str, bool all )
{
    return this->regreplace(pat, new_str.cstr(), all);
}

/**
 * @brief  左側からの配列要素の完全一致検索
 *
 *  自身の配列要素の左側から文字列 str に完全に一致する要素を検索し，
 *  見つかった場合はその要素番号を返し，見つからなければ負数を返します．
 *
 * @param      str 検出する要素値に一致する文字列
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tarray_tstring::find_elem( const char *str ) const
{
    return this->find_elem((size_t)0, str, (size_t *)NULL);
}

/**
 * @brief  左側からの配列要素の完全一致検索
 *
 *  自身の配列要素の左側から文字列 str に完全に一致する要素を検索し，
 *  見つかった場合はその要素番号を返し，見つからなければ負数を返します．
 *
 * @param      idx 配列要素の検索開始位置
 * @param      str 検出する要素値に一致する文字列
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tarray_tstring::find_elem( size_t idx, const char *str ) const
{
    return this->find_elem(idx, str, (size_t *)NULL);
}

/**
 * @brief  連続的な左側からの配列要素の完全一致検索
 *
 *  自身の配列要素の左側から文字列 str に完全に一致する要素を検索し，
 *  見つかった場合はその要素番号を返し，見つからなければ負数を返します．
 *
 * @param      idx 配列要素の検索開始位置
 * @param      str 検出する要素値に一致する文字列
 * @param      nextidx 次回のidx
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tarray_tstring::find_elem( size_t idx, const char *str, 
				   size_t *nextidx ) const
{
    if ( idx < this->length() && str != NULL ) {
	size_t i;
	for ( i=idx ; i < this->length() ; i++ ) {
	    if ( this->at_cs(i).compare(str) == 0 ) {
		if ( nextidx != NULL ) {
		    *nextidx = i + 1;
		}
		return i;
	    }
	}
    }
    /* */
    if ( nextidx != NULL ) *nextidx = this->length();
    return -1;
}

/**
 * @brief  左側からの配列要素の完全一致検索
 *
 *  自身の配列要素の左側から文字列 str に完全に一致する要素を検索し，
 *  見つかった場合はその要素番号を返し，見つからなければ負数を返します．
 *
 * @param      str 検出する要素値に一致する文字列
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tarray_tstring::find_elem( const tstring &str ) const
{
    return this->find_elem(str.cstr());
}

/**
 * @brief  左側からの配列要素の完全一致検索
 *
 *  自身の配列要素の左側から文字列 str に完全に一致する要素を検索し，
 *  見つかった場合はその要素番号を返し，見つからなければ負数を返します．
 *
 * @param      idx 配列要素の検索開始位置
 * @param      str 検出する要素値に一致する文字列
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tarray_tstring::find_elem( size_t idx, const tstring &str ) const
{
    return this->find_elem(idx, str.cstr());
}

/**
 * @brief  連続的な左側からの配列要素の完全一致検索
 *
 *  自身の配列要素の左側から文字列 str に完全に一致する要素を検索し，
 *  見つかった場合はその要素番号を返し，見つからなければ負数を返します．
 *
 * @param      idx 配列要素の検索開始位置
 * @param      str 検出する要素値に一致する文字列
 * @param      nextidx 次回のidx
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tarray_tstring::find_elem( size_t idx, const tstring &str,
				   size_t *nextidx ) const
{
    return this->find_elem(idx, str.cstr(), nextidx);
}

/**
 * @brief  右側からの配列要素の完全一致検索
 *
 *  自身の配列要素の右側から文字列 str に完全に一致する要素を検索し，
 *  見つかった場合はその要素番号を返し，見つからなければ負数を返します．
 *
 * @param      str 検出する要素値に一致する文字列
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tarray_tstring::rfind_elem( const char *str ) const
{
    if ( this->length() < 1 ) return -1;
    return this->rfind_elem(this->length() - 1, str, (size_t *)NULL);
}

/**
 * @brief  右側からの配列要素の完全一致検索
 *
 *  自身の配列要素の右側から文字列 str に完全に一致する要素を検索し，
 *  見つかった場合はその要素番号を返し，見つからなければ負数を返します．
 *
 * @param      idx 配列要素の検索開始位置
 * @param      str 検出する要素値に一致する文字列
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tarray_tstring::rfind_elem( size_t idx, const char *str ) const
{
    return this->rfind_elem(idx, str, (size_t *)NULL);
}

/**
 * @brief  連続的な右側からの配列要素の完全一致検索
 *
 *  自身の配列要素の右側から文字列 str に完全に一致する要素を検索し，
 *  見つかった場合はその要素番号を返し，見つからなければ負数を返します．
 *
 * @param      idx 配列要素の検索開始位置
 * @param      str 検出する要素値に一致する文字列
 * @param      nextidx 次回のidx
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tarray_tstring::rfind_elem( size_t idx, const char *str,
				    size_t *nextidx ) const
{
    if ( idx < this->length() && str != NULL ) {
	size_t i;
	for ( i=idx+1 ; 0 < i ; ) {
	    i--;
	    if ( this->at_cs(i).compare(str) == 0 ) {
		if ( nextidx != NULL ) {
		    if ( 0 < i ) *nextidx = i - 1;
		    else *nextidx = this->length();
		}
		return i;
	    }
	}
    }
    /* */
    if ( nextidx != NULL ) *nextidx = this->length();
    return -1;
}

/**
 * @brief  右側からの配列要素の完全一致検索
 *
 *  自身の配列要素の右側から文字列 str に完全に一致する要素を検索し，
 *  見つかった場合はその要素番号を返し，見つからなければ負数を返します．
 *
 * @param      str 検出する要素値に一致する文字列
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tarray_tstring::rfind_elem( const tstring &str ) const
{
    return this->rfind_elem(str.cstr());
}

/**
 * @brief  右側からの配列要素の完全一致検索
 *
 *  自身の配列要素の右側から文字列 str に完全に一致する要素を検索し，
 *  見つかった場合はその要素番号を返し，見つからなければ負数を返します．
 *
 * @param      idx 配列要素の検索開始位置
 * @param      str 検出する要素値に一致する文字列
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tarray_tstring::rfind_elem( size_t idx, const tstring &str ) const
{
    return this->rfind_elem(idx, str.cstr());
}

/**
 * @brief  連続的な右側からの配列要素の完全一致検索
 *
 *  自身の配列要素の右側から文字列 str に完全に一致する要素を検索し，
 *  見つかった場合はその要素番号を返し，見つからなければ負数を返します．
 *
 * @param      idx 配列要素の検索開始位置
 * @param      str 検出する要素値に一致する文字列
 * @param      nextidx 次回のidx
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tarray_tstring::rfind_elem( size_t idx, const tstring &str,
				    size_t *nextidx ) const
{
    return this->rfind_elem(idx, str.cstr(), nextidx);
}

/**
 * @brief  左側からの配列要素の部分一致検索
 *
 *  自身の配列要素の左側から文字列 str を含む要素を検索し，メンバ関数の返り値と
 *  して，見つかった場合はその要素番号を返し，見つからなければ負数を返します．
 *  同時に，見つかった場合は pos_r が指す変数に，その要素における文字列の位置
 *  も返します．
 *
 * @param      str 検出する文字列
 * @param      pos_r 見つかった場合，その要素における文字列の位置
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tarray_tstring::find( const char *str, ssize_t *pos_r ) const
{
    return this->find((size_t)0, (size_t)0, str, 
		      pos_r, (size_t *)NULL, (size_t *)NULL);
}

/**
 * @brief  左側からの配列要素の部分一致検索
 *
 *  自身の配列要素の左側から文字列 str を含む要素を検索し，メンバ関数の返り値と
 *  して，見つかった場合はその要素番号を返し，見つからなければ負数を返します．
 *  同時に，見つかった場合は pos_r が指す変数に，その要素における文字列の位置
 *  も返します．
 *
 * @param      idx 配列要素の検索開始位置
 * @param      pos 文字列の検索開始位置
 * @param      str 検出する文字列
 * @param      pos_r 見つかった場合，その要素における文字列の位置
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tarray_tstring::find( size_t idx, size_t pos, const char *str,
			      ssize_t *pos_r ) const
{
    return this->find(idx, pos, str, pos_r, (size_t *)NULL, (size_t *)NULL);
}

/**
 * @brief  連続的な左側からの配列要素の部分一致検索
 *
 *  自身の配列要素の左側から文字列 str を含む要素を検索し，メンバ関数の返り値と
 *  して，見つかった場合はその要素番号を返し，見つからなければ負数を返します．
 *  同時に，見つかった場合は pos_r が指す変数に，その要素における文字列の位置
 *  も返します．
 *
 * @param      idx 配列要素の検索開始位置
 * @param      pos 文字列の検索開始位置
 * @param      str 検出する文字列
 * @param      pos_r 見つかった場合，その要素における文字列の位置
 * @param      nextidx 次回のidx
 * @param      nextpos 次回のpos
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tarray_tstring::find( size_t idx, size_t pos, const char *str,
			      ssize_t *pos_r,
			      size_t *nextidx, size_t *nextpos ) const
{
    ssize_t f_pos = -1;
    size_t a_length = this->length();
    if ( idx < a_length && pos <= this->at_cs(idx).length() && str != NULL) {
	size_t i;
	for ( i=idx ; i < a_length ; i++ ) {
	    if ( 0 <= (f_pos=this->at_cs(i).find(pos, str, &pos)) ) {
		if ( pos_r != NULL ) *pos_r = f_pos;
		if ( nextidx != NULL ) {
		    if ( this->at_cs(i).length() < pos ) *nextidx = i + 1;
		    else *nextidx = i;		/* keep */
		}
		if ( nextpos != NULL ) {
		    if ( i + 1 < a_length ) {	/* not last */
			if ( this->at_cs(i).length() < pos ) *nextpos = 0;
			else *nextpos = pos;
		    }
		    else *nextpos = pos;
		}
		return i;
	    }
	    pos = 0;	/* reset */
	}
    }
    /* not found */
    if ( pos_r != NULL ) *pos_r = f_pos;
    if ( nextidx != NULL ) *nextidx = a_length;
    if ( nextpos != NULL ) {
	*nextpos = (0 < a_length) ? this->at_cs(a_length - 1).length() + 1 : 1;
    }
    return -1;
}

/**
 * @brief  左側からの配列要素の部分一致検索
 *
 *  自身の配列要素の左側から文字列 str を含む要素を検索し，メンバ関数の返り値と
 *  して，見つかった場合はその要素番号を返し，見つからなければ負数を返します．
 *  同時に，見つかった場合は pos_r が指す変数に，その要素における文字列の位置
 *  も返します．
 *
 * @param      str 検出する文字列
 * @param      pos_r 見つかった場合，その要素における文字列の位置
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tarray_tstring::find( const tstring &str, ssize_t *pos_r ) const
{
    return this->find(str.cstr(), pos_r);
}

/**
 * @brief  左側からの配列要素の部分一致検索
 *
 *  自身の配列要素の左側から文字列 str を含む要素を検索し，メンバ関数の返り値と
 *  して，見つかった場合はその要素番号を返し，見つからなければ負数を返します．
 *  同時に，見つかった場合は pos_r が指す変数に，その要素における文字列の位置
 *  も返します．
 *
 * @param      idx 配列要素の検索開始位置
 * @param      pos 文字列の検索開始位置
 * @param      str 検出する文字列
 * @param      pos_r 見つかった場合，その要素における文字列の位置
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tarray_tstring::find( size_t idx, size_t pos, const tstring &str,
			      ssize_t *pos_r) const
{
    return this->find(idx, pos, str.cstr(), pos_r);
}

/**
 * @brief  連続的な左側からの配列要素の部分一致検索
 *
 *  自身の配列要素の左側から文字列 str を含む要素を検索し，メンバ関数の返り値と
 *  して，見つかった場合はその要素番号を返し，見つからなければ負数を返します．
 *  同時に，見つかった場合は pos_r が指す変数に，その要素における文字列の位置
 *  も返します．
 *
 * @param      idx 配列要素の検索開始位置
 * @param      pos 文字列の検索開始位置
 * @param      str 検出する文字列
 * @param      pos_r 見つかった場合，その要素における文字列の位置
 * @param      nextidx 次回のidx
 * @param      nextpos 次回のpos
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tarray_tstring::find( size_t idx, size_t pos, const tstring &str,
			      ssize_t *pos_r, 
			      size_t *nextidx, size_t *nextpos) const
{
    return this->find(idx, pos, str.cstr(), pos_r, nextidx, nextpos);
}

/**
 * @brief  右側からの配列要素の部分一致検索
 *
 *  自身の配列要素の右側から文字列 str を含む要素を検索し，メンバ関数の返り値と
 *  して，見つかった場合はその要素番号を返し，見つからなければ負数を返します．
 *  同時に，見つかった場合は pos_r が指す変数に，その要素における文字列の位置
 *  も返します．
 *
 * @param      str 検出する文字列
 * @param      pos_r 見つかった場合，その要素における文字列の位置
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tarray_tstring::rfind( const char *str, ssize_t *pos_r ) const
{
    size_t a_length = this->length();
    if ( a_length < 1 ) return -1;
    return this->rfind(a_length - 1, this->at_cs(a_length - 1).length(), str, 
		       pos_r, (size_t *)NULL, (size_t *)NULL);
}

/**
 * @brief  右側からの配列要素の部分一致検索
 *
 *  自身の配列要素の右側から文字列 str を含む要素を検索し，メンバ関数の返り値と
 *  して，見つかった場合はその要素番号を返し，見つからなければ負数を返します．
 *  同時に，見つかった場合は pos_r が指す変数に，その要素における文字列の位置
 *  も返します．
 *
 * @param      idx 配列要素の検索開始位置
 * @param      pos 文字列の検索開始位置
 * @param      str 検出する文字列
 * @param      pos_r 見つかった場合，その要素における文字列の位置
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tarray_tstring::rfind( size_t idx, size_t pos, const char *str,
			       ssize_t *pos_r ) const
{
    return this->rfind(idx, pos, str, pos_r, (size_t *)NULL, (size_t *)NULL);
}

/**
 * @brief  連続的な右側からの配列要素の部分一致検索
 *
 *  自身の配列要素の右側から文字列 str を含む要素を検索し，メンバ関数の返り値と
 *  して，見つかった場合はその要素番号を返し，見つからなければ負数を返します．
 *  同時に，見つかった場合は pos_r が指す変数に，その要素における文字列の位置
 *  も返します．
 *
 * @param      idx 配列要素の検索開始位置
 * @param      pos 文字列の検索開始位置
 * @param      str 検出する文字列
 * @param      pos_r 見つかった場合，その要素における文字列の位置
 * @param      nextidx 次回のidx
 * @param      nextpos 次回のpos
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tarray_tstring::rfind( size_t idx, size_t pos, const char *str,
			       ssize_t *pos_r,
			       size_t *nextidx, size_t *nextpos ) const
{
    ssize_t f_pos = -1;
    size_t a_length = this->length();
    if ( idx < a_length && pos <= this->at_cs(idx).length() && str != NULL) {
	size_t i;
	for ( i=idx+1 ; 0 < i ; ) {
	    i--;
	    if ( 0 <= (f_pos=this->at_cs(i).rfind(pos, str, &pos)) ) {
		if ( pos_r != NULL ) *pos_r = f_pos;
		if ( nextidx != NULL ) {
		    if ( this->at_cs(i).length() < pos ) {
			if ( 0 < i ) *nextidx = i - 1;
			else *nextidx = a_length;
		    }
		    else *nextidx = i;		/* keep */
		}
		if ( nextpos != NULL ) {
		    if ( 0 < i ) {	/* not last */
			if ( this->at_cs(i).length() < pos ) {
			    *nextpos = this->at_cs(i-1).length();
			}
			else *nextpos = pos;
		    }
		    else *nextpos = pos;
		}
		return i;
	    }
	    if ( 0 < i ) pos = this->at_cs(i-1).length();	/* reset */
	}
    }
    /* not found */
    if ( pos_r != NULL ) *pos_r = f_pos;
    if ( nextidx != NULL ) *nextidx = a_length;
    if ( nextpos != NULL ) {
	*nextpos = (0 < a_length) ? this->at_cs((size_t)0).length() + 1 : 1;
    }
    return -1;
}

/**
 * @brief  右側からの配列要素の部分一致検索
 *
 *  自身の配列要素の右側から文字列 str を含む要素を検索し，メンバ関数の返り値と
 *  して，見つかった場合はその要素番号を返し，見つからなければ負数を返します．
 *  同時に，見つかった場合は pos_r が指す変数に，その要素における文字列の位置
 *  も返します．
 *
 * @param      str 検出する文字列
 * @param      pos_r 見つかった場合，その要素における文字列の位置
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tarray_tstring::rfind( const tstring &str, ssize_t *pos_r ) const
{
    return this->rfind(str.cstr(), pos_r);
}

/**
 * @brief  右側からの配列要素の部分一致検索
 *
 *  自身の配列要素の右側から文字列 str を含む要素を検索し，メンバ関数の返り値と
 *  して，見つかった場合はその要素番号を返し，見つからなければ負数を返します．
 *  同時に，見つかった場合は pos_r が指す変数に，その要素における文字列の位置
 *  も返します．
 *
 * @param      idx 配列要素の検索開始位置
 * @param      pos 文字列の検索開始位置
 * @param      str 検出する文字列
 * @param      pos_r 見つかった場合，その要素における文字列の位置
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tarray_tstring::rfind( size_t idx, size_t pos, const tstring &str,
			       ssize_t *pos_r) const
{
    return this->rfind(idx, pos, str.cstr(), pos_r);
}

/**
 * @brief  連続的な右側からの配列要素の部分一致検索
 *
 *  自身の配列要素の右側から文字列 str を含む要素を検索し，メンバ関数の返り値と
 *  して，見つかった場合はその要素番号を返し，見つからなければ負数を返します．
 *  同時に，見つかった場合は pos_r が指す変数に，その要素における文字列の位置
 *  も返します．
 *
 * @param      idx 配列要素の検索開始位置
 * @param      pos 文字列の検索開始位置
 * @param      str 検出する文字列
 * @param      pos_r 見つかった場合，その要素における文字列の位置
 * @param      nextidx 次回のidx
 * @param      nextpos 次回のpos
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tarray_tstring::rfind( size_t idx, size_t pos, const tstring &str,
			       ssize_t *pos_r, 
			       size_t *nextidx, size_t *nextpos) const
{
    return this->rfind(idx, pos, str.cstr(), pos_r, nextidx, nextpos);
}

/**
 * @brief  シェルのワイルドカードパターンを用いた文字列マッチを試行
 *
 *  自身の配列要素の左側から順に，シェルのワイルドカードパターンを用いた文字列
 *  マッチを試行し，マッチした場合は要素番号を返します．
 *
 * @param      str 検出する要素値に一致する文字列(マニュアルではpat)
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tarray_tstring::find_matched_str( const char *str ) const
{
    return this->find_matched_str((size_t)0, str, (size_t *)NULL);
}

/**
 * @brief  シェルのワイルドカードパターンを用いた文字列マッチを試行
 *
 *  自身の配列要素の左側から順に，シェルのワイルドカードパターンを用いた文字列
 *  マッチを試行し，マッチした場合は要素番号を返します．
 *
 * @param      idx 配列要素の検索開始位置
 * @param      str 検出する要素値に一致する文字列(マニュアルではpat)
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tarray_tstring::find_matched_str( size_t idx, const char *str ) const
{
    return this->find_matched_str(idx, str, (size_t *)NULL);
}

/**
 * @brief  シェルのワイルドカードパターンを用いた文字列マッチを試行
 *
 *  自身の配列要素の左側から順に，シェルのワイルドカードパターンを用いた文字列
 *  マッチを試行し，マッチした場合は要素番号を返します．
 *
 * @param      idx 配列要素の検索開始位置
 * @param      str 検出する要素値に一致する文字列(マニュアルではpat)
 * @param      nextidx 次回のidx
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tarray_tstring::find_matched_str( size_t idx, const char *str, 
					  size_t *nextidx ) const
{
    if ( idx < this->length() && str != NULL ) {
	size_t i;
	for ( i=idx ; i < this->length() ; i++ ) {
	    if ( this->at_cs(i).strmatch(str) == 0 ) {
		if ( nextidx != NULL ) {
		    *nextidx = i + 1;
		}
		return i;
	    }
	}
    }
    /* */
    if ( nextidx != NULL ) *nextidx = this->length();
    return -1;
}

/**
 * @brief  シェルのワイルドカードパターンを用いた文字列マッチを試行
 *
 *  自身の配列要素の左側から順に，シェルのワイルドカードパターンを用いた文字列
 *  マッチを試行し，マッチした場合は要素番号を返します．
 *
 * @param      str 検出する要素値に一致する文字列(マニュアルではpat)
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tarray_tstring::find_matched_str( const tstring &str ) const
{
    return this->find_matched_str(str.cstr());
}

/**
 * @brief  シェルのワイルドカードパターンを用いた文字列マッチを試行
 *
 *  自身の配列要素の左側から順に，シェルのワイルドカードパターンを用いた文字列
 *  マッチを試行し，マッチした場合は要素番号を返します．
 *
 * @param      idx 配列要素の検索開始位置
 * @param      str 検出する要素値に一致する文字列(マニュアルではpat)
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tarray_tstring::find_matched_str(size_t idx, const tstring &str) const
{
    return this->find_matched_str(idx, str.cstr());
}

/**
 * @brief  シェルのワイルドカードパターンを用いた文字列マッチを試行
 *
 *  自身の配列要素の左側から順に，シェルのワイルドカードパターンを用いた文字列
 *  マッチを試行し，マッチした場合は要素番号を返します．
 *
 * @param      idx 配列要素の検索開始位置
 * @param      str 検出する要素値に一致する文字列(マニュアルではpat)
 * @param      nextidx 次回のidx
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tarray_tstring::find_matched_str( size_t idx, const tstring &str,
					  size_t *nextidx ) const
{
    return this->find_matched_str(idx, str.cstr(), nextidx);
}

/**
 * @brief  シェルのワイルドカードパターンを用いた文字列マッチ(ファイル名向き)
 *
 *  自身の配列要素の左側から順に，シェルのワイルドカードパターンを用いた文字列
 *  マッチを試行し，マッチした場合は要素番号を返します．
 *
 * @param      str 検出する要素値に一致する文字列(マニュアルではpat)
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tarray_tstring::find_matched_fn( const char *str ) const
{
    return this->find_matched_fn((size_t)0, str, (size_t *)NULL);
}

/**
 * @brief  シェルのワイルドカードパターンを用いた文字列マッチ(ファイル名向き)
 *
 *  自身の配列要素の左側から順に，シェルのワイルドカードパターンを用いた文字列
 *  マッチを試行し，マッチした場合は要素番号を返します．
 *
 * @param      idx 配列要素の検索開始位置
 * @param      str 検出する要素値に一致する文字列(マニュアルではpat)
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tarray_tstring::find_matched_fn( size_t idx, const char *str ) const
{
    return this->find_matched_fn(idx, str, (size_t *)NULL);
}

/**
 * @brief  シェルのワイルドカードパターンを用いた文字列マッチ(ファイル名向き)
 *
 *  自身の配列要素の左側から順に，シェルのワイルドカードパターンを用いた文字列
 *  マッチを試行し，マッチした場合は要素番号を返します．
 *
 * @param      idx 配列要素の検索開始位置
 * @param      str 検出する要素値に一致する文字列(マニュアルではpat)
 * @param      nextidx 次回のidx
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tarray_tstring::find_matched_fn( size_t idx, const char *str, 
					  size_t *nextidx ) const
{
    if ( idx < this->length() && str != NULL ) {
	size_t i;
	for ( i=idx ; i < this->length() ; i++ ) {
	    if ( this->at_cs(i).fnmatch(str) == 0 ) {
		if ( nextidx != NULL ) {
		    *nextidx = i + 1;
		}
		return i;
	    }
	}
    }
    /* */
    if ( nextidx != NULL ) *nextidx = this->length();
    return -1;
}

/**
 * @brief  シェルのワイルドカードパターンを用いた文字列マッチ(ファイル名向き)
 *
 *  自身の配列要素の左側から順に，シェルのワイルドカードパターンを用いた文字列
 *  マッチを試行し，マッチした場合は要素番号を返します．
 *
 * @param      str 検出する要素値に一致する文字列(マニュアルではpat)
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tarray_tstring::find_matched_fn( const tstring &str ) const
{
    return this->find_matched_fn(str.cstr());
}

/**
 * @brief  シェルのワイルドカードパターンを用いた文字列マッチ(ファイル名向き)
 *
 *  自身の配列要素の左側から順に，シェルのワイルドカードパターンを用いた文字列
 *  マッチを試行し，マッチした場合は要素番号を返します．
 *
 * @param      idx 配列要素の検索開始位置
 * @param      str 検出する要素値に一致する文字列(マニュアルではpat)
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tarray_tstring::find_matched_fn(size_t idx, const tstring &str) const
{
    return this->find_matched_fn(idx, str.cstr());
}

/**
 * @brief  シェルのワイルドカードパターンを用いた文字列マッチ(ファイル名向き)
 *
 *  自身の配列要素の左側から順に，シェルのワイルドカードパターンを用いた文字列
 *  マッチを試行し，マッチした場合は要素番号を返します．
 *
 * @param      idx 配列要素の検索開始位置
 * @param      str 検出する要素値に一致する文字列(マニュアルではpat)
 * @param      nextidx 次回のidx
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tarray_tstring::find_matched_fn( size_t idx, const tstring &str,
					  size_t *nextidx ) const
{
    return this->find_matched_fn(idx, str.cstr(), nextidx);
}

/**
 * @brief  シェルのワイルドカードパターンを用いた文字列マッチ(パス名向き)
 *
 *  自身の配列要素の左側から順に，シェルのワイルドカードパターンを用いた文字列
 *  マッチを試行し，マッチした場合は要素番号を返します．
 *
 * @param      str 検出する要素値に一致する文字列(マニュアルではpat)
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tarray_tstring::find_matched_pn( const char *str ) const
{
    return this->find_matched_pn((size_t)0, str, (size_t *)NULL);
}

/**
 * @brief  シェルのワイルドカードパターンを用いた文字列マッチ(パス名向き)
 *
 *  自身の配列要素の左側から順に，シェルのワイルドカードパターンを用いた文字列
 *  マッチを試行し，マッチした場合は要素番号を返します．
 *
 * @param      idx 配列要素の検索開始位置
 * @param      str 検出する要素値に一致する文字列(マニュアルではpat)
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tarray_tstring::find_matched_pn( size_t idx, const char *str ) const
{
    return this->find_matched_pn(idx, str, (size_t *)NULL);
}

/**
 * @brief  シェルのワイルドカードパターンを用いた文字列マッチ(パス名向き)
 *
 *  自身の配列要素の左側から順に，シェルのワイルドカードパターンを用いた文字列
 *  マッチを試行し，マッチした場合は要素番号を返します．
 *
 * @param      idx 配列要素の検索開始位置
 * @param      str 検出する要素値に一致する文字列(マニュアルではpat)
 * @param      nextidx 次回のidx
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tarray_tstring::find_matched_pn( size_t idx, const char *str, 
					  size_t *nextidx ) const
{
    if ( idx < this->length() && str != NULL ) {
	size_t i;
	for ( i=idx ; i < this->length() ; i++ ) {
	    if ( this->at_cs(i).pnmatch(str) == 0 ) {
		if ( nextidx != NULL ) {
		    *nextidx = i + 1;
		}
		return i;
	    }
	}
    }
    /* */
    if ( nextidx != NULL ) *nextidx = this->length();
    return -1;
}

/**
 * @brief  シェルのワイルドカードパターンを用いた文字列マッチ(パス名向き)
 *
 *  自身の配列要素の左側から順に，シェルのワイルドカードパターンを用いた文字列
 *  マッチを試行し，マッチした場合は要素番号を返します．
 *
 * @param      str 検出する要素値に一致する文字列(マニュアルではpat)
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tarray_tstring::find_matched_pn( const tstring &str ) const
{
    return this->find_matched_pn(str.cstr());
}

/**
 * @brief  シェルのワイルドカードパターンを用いた文字列マッチ(パス名向き)
 *
 *  自身の配列要素の左側から順に，シェルのワイルドカードパターンを用いた文字列
 *  マッチを試行し，マッチした場合は要素番号を返します．
 *
 * @param      idx 配列要素の検索開始位置
 * @param      str 検出する要素値に一致する文字列(マニュアルではpat)
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tarray_tstring::find_matched_pn(size_t idx, const tstring &str) const
{
    return this->find_matched_pn(idx, str.cstr());
}

/**
 * @brief  シェルのワイルドカードパターンを用いた文字列マッチ(パス名向き)
 *
 *  自身の配列要素の左側から順に，シェルのワイルドカードパターンを用いた文字列
 *  マッチを試行し，マッチした場合は要素番号を返します．
 *
 * @param      idx 配列要素の検索開始位置
 * @param      str 検出する要素値に一致する文字列(マニュアルではpat)
 * @param      nextidx 次回のidx
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tarray_tstring::find_matched_pn( size_t idx, const tstring &str,
					  size_t *nextidx ) const
{
    return this->find_matched_pn(idx, str.cstr(), nextidx);
}

#ifdef REGFUNCS_USING_CACHE
/* 強引にキャストする関数 */
static tregex *coerce_into_casting( const tregex *in )
{ 
    return (tregex *)in;
} 
#endif

/**
 * @brief  配列要素に対する正規表現マッチを試行
 *
 *  自身の配列要素の左側から，pat で指定された POSIX 拡張正規表現(以下，正規
 *  表現)にマッチする部分を含む要素を検索し，メンバ関数の返り値として，マッチ
 *  した場合はその要素番号を返し，マッチしなければ負数を返します．同時に，
 *  マッチした場合は pos_r，span_r が指す変数に，その要素におけるマッチした
 *  部分の文字位置と長さも返します．
 *
 * @param      pat 検索に使う正規表現
 * @param      pos_r マッチした場合，その要素におけるマッチした部分の文字位置
 * @param      span_r マッチした場合，その要素におけるマッチした部分の文字列長
 * @return     成功した場合は配列の要素番号<br>
 *             失敗した場合は負値
 *
 */
ssize_t tarray_tstring::regmatch( const char *pat,
				  ssize_t *pos_r, size_t *span_r ) const
{
    return this->regmatch((size_t)0, (size_t)0, pat, pos_r, span_r,
			  (size_t *)NULL, (size_t *)NULL);
}

/**
 * @brief  配列要素に対する正規表現マッチを試行
 *
 *  自身の配列要素の左側から，pat で指定された POSIX 拡張正規表現(以下，正規
 *  表現)にマッチする部分を含む要素を検索し，メンバ関数の返り値として，マッチ
 *  した場合はその要素番号を返し，マッチしなければ負数を返します．同時に，
 *  マッチした場合は pos_r，span_r が指す変数に，その要素におけるマッチした
 *  部分の文字位置と長さも返します．
 *
 * @param      idx 配列要素の検索開始位置
 * @param      pos 文字列の検索開始位置
 * @param      pat 検索に使う正規表現
 * @param      pos_r マッチした場合，その要素におけるマッチした部分の文字位置
 * @param      span_r マッチした場合，その要素におけるマッチした部分の文字列長
 * @return     成功した場合は配列の要素番号<br>
 *             失敗した場合は負値
 *
 */
ssize_t tarray_tstring::regmatch( size_t idx, size_t pos, const char *pat,
				  ssize_t *pos_r, size_t *span_r ) const
{
    return this->regmatch(idx, pos, pat, pos_r, span_r,
			  (size_t *)NULL, (size_t *)NULL);
}

/**
 * @brief  連続的に配列要素に対する正規表現マッチを試行
 *
 *  自身の配列要素の左側から，pat で指定された POSIX 拡張正規表現(以下，正規
 *  表現)にマッチする部分を含む要素を検索し，メンバ関数の返り値として，マッチ
 *  した場合はその要素番号を返し，マッチしなければ負数を返します．同時に，
 *  マッチした場合は pos_r，span_r が指す変数に，その要素におけるマッチした
 *  部分の文字位置と長さも返します．
 *
 * @param      idx 配列要素の検索開始位置
 * @param      pos 文字列の検索開始位置
 * @param      pat 検索に使う正規表現
 * @param      pos_r マッチした場合，その要素におけるマッチした部分の文字位置
 * @param      span_r マッチした場合，その要素におけるマッチした部分の文字列長
 * @param      nextidx 次回のidx
 * @param      nextpos 次回のpos
 * @return     成功した場合は配列の要素番号<br>
 *             失敗した場合は負値
 *
 */
ssize_t tarray_tstring::regmatch( size_t idx, size_t pos, const char *pat,
				  ssize_t *pos_r, size_t *span_r,
				  size_t *nextidx, size_t *nextpos ) const
{
    tregex *epat_p;
#ifdef REGFUNCS_USING_CACHE
    epat_p = coerce_into_casting(&(this->regex_rec));
#else
    tregex epat;
    epat_p = &epat;
#endif
    if ( epat_p->compile(pat) < 0 ) {
	err_report1(__FUNCTION__,"ERROR","%s",epat_p->cerrstr());
    }
    return this->regmatch(idx, pos, *epat_p, pos_r, span_r,
			  nextidx, nextpos);
}

/**
 * @brief  配列要素に対する正規表現マッチを試行
 *
 *  自身の配列要素の左側から，pat で指定された POSIX 拡張正規表現(以下，正規
 *  表現)にマッチする部分を含む要素を検索し，メンバ関数の返り値として，マッチ
 *  した場合はその要素番号を返し，マッチしなければ負数を返します．同時に，
 *  マッチした場合は pos_r，span_r が指す変数に，その要素におけるマッチした
 *  部分の文字位置と長さも返します．
 *
 * @param      pat 検索に使う正規表現
 * @param      pos_r マッチした場合，その要素におけるマッチした部分の文字位置
 * @param      span_r マッチした場合，その要素におけるマッチした部分の文字列長
 * @return     成功した場合は配列の要素番号<br>
 *             失敗した場合は負値
 *
 */
ssize_t tarray_tstring::regmatch( const tstring &pat,
				  ssize_t *pos_r, size_t *span_r ) const
{
    return this->regmatch(0, 0, pat, pos_r, span_r,
			  (size_t *)NULL, (size_t *)NULL);
}

/**
 * @brief  配列要素に対する正規表現マッチを試行
 *
 *  自身の配列要素の左側から，pat で指定された POSIX 拡張正規表現(以下，正規
 *  表現)にマッチする部分を含む要素を検索し，メンバ関数の返り値として，マッチ
 *  した場合はその要素番号を返し，マッチしなければ負数を返します．同時に，
 *  マッチした場合は pos_r，span_r が指す変数に，その要素におけるマッチした
 *  部分の文字位置と長さも返します．
 *
 * @param      idx 配列要素の検索開始位置
 * @param      pos 文字列の検索開始位置
 * @param      pat 検索に使う正規表現
 * @param      pos_r マッチした場合，その要素におけるマッチした部分の文字位置
 * @param      span_r マッチした場合，その要素におけるマッチした部分の文字列長
 * @return     成功した場合は配列の要素番号<br>
 *             失敗した場合は負値
 *
 */
ssize_t tarray_tstring::regmatch( size_t idx, size_t pos, const tstring &pat,
				  ssize_t *pos_r, size_t *span_r ) const
{
    return this->regmatch(idx, pos, pat, pos_r, span_r,
			  (size_t *)NULL, (size_t *)NULL);
}

/**
 * @brief  連続的に配列要素に対する正規表現マッチを試行
 *
 *  自身の配列要素の左側から，pat で指定された POSIX 拡張正規表現(以下，正規
 *  表現)にマッチする部分を含む要素を検索し，メンバ関数の返り値として，マッチ
 *  した場合はその要素番号を返し，マッチしなければ負数を返します．同時に，
 *  マッチした場合は pos_r，span_r が指す変数に，その要素におけるマッチした
 *  部分の文字位置と長さも返します．
 *
 * @param      idx 配列要素の検索開始位置
 * @param      pos 文字列の検索開始位置
 * @param      pat 検索に使う正規表現
 * @param      pos_r マッチした場合，その要素におけるマッチした部分の文字位置
 * @param      span_r マッチした場合，その要素におけるマッチした部分の文字列長
 * @param      nextidx 次回のidx
 * @param      nextpos 次回のpos
 * @return     成功した場合は配列の要素番号<br>
 *             失敗した場合は負値
 *
 */
ssize_t tarray_tstring::regmatch( size_t idx, size_t pos, const tstring &pat,
				  ssize_t *pos_r, size_t *span_r,
				  size_t *nextidx, size_t *nextpos ) const
{
    tregex *epat_p;
#ifdef REGFUNCS_USING_CACHE
    epat_p = coerce_into_casting(&(this->regex_rec));
#else
    tregex epat;
    epat_p = &epat;
#endif
    if ( epat_p->compile(pat.cstr()) < 0 ) {
	err_report1(__FUNCTION__,"ERROR","%s",epat_p->cerrstr());
    }
    return this->regmatch(idx, pos, *epat_p, pos_r, span_r,
			  nextidx, nextpos);
}

/**
 * @brief  配列要素に対する正規表現マッチを試行
 *
 *  自身の配列要素の左側から，pat で指定された POSIX 拡張正規表現(以下，正規
 *  表現)にマッチする部分を含む要素を検索し，メンバ関数の返り値として，マッチ
 *  した場合はその要素番号を返し，マッチしなければ負数を返します．同時に，
 *  マッチした場合は pos_r，span_r が指す変数に，その要素におけるマッチした
 *  部分の文字位置と長さも返します．
 *
 * @param      pat 検索に使う正規表現
 * @param      pos_r マッチした場合，その要素におけるマッチした部分の文字位置
 * @param      span_r マッチした場合，その要素におけるマッチした部分の文字列長
 * @return     成功した場合は配列の要素番号<br>
 *             失敗した場合は負値
 *
 */
ssize_t tarray_tstring::regmatch( const tregex &pat,
				  ssize_t *pos_r, size_t *span_r ) const
{
    return this->regmatch(0, 0, pat, pos_r, span_r, NULL, NULL);
}

/**
 * @brief  配列要素に対する正規表現マッチを試行
 *
 *  自身の配列要素の左側から，pat で指定された POSIX 拡張正規表現(以下，正規
 *  表現)にマッチする部分を含む要素を検索し，メンバ関数の返り値として，マッチ
 *  した場合はその要素番号を返し，マッチしなければ負数を返します．同時に，
 *  マッチした場合は pos_r，span_r が指す変数に，その要素におけるマッチした
 *  部分の文字位置と長さも返します．
 *
 * @param      idx 配列要素の検索開始位置
 * @param      pos 文字列の検索開始位置
 * @param      pat 検索に使う正規表現
 * @param      pos_r マッチした場合，その要素におけるマッチした部分の文字位置
 * @param      span_r マッチした場合，その要素におけるマッチした部分の文字列長
 * @return     成功した場合は配列の要素番号<br>
 *             失敗した場合は負値
 *
 */
ssize_t tarray_tstring::regmatch( size_t idx, size_t pos, const tregex &pat,
				  ssize_t *pos_r, size_t *span_r ) const
{
    return this->regmatch(idx, pos, pat, pos_r, span_r, NULL, NULL);
}

/**
 * @brief  連続的に配列要素に対する正規表現マッチを試行
 *
 *  自身の配列要素の左側から，pat で指定された POSIX 拡張正規表現(以下，正規
 *  表現)にマッチする部分を含む要素を検索し，メンバ関数の返り値として，マッチ
 *  した場合はその要素番号を返し，マッチしなければ負数を返します．同時に，
 *  マッチした場合は pos_r，span_r が指す変数に，その要素におけるマッチした
 *  部分の文字位置と長さも返します．
 *
 * @param      idx 配列要素の検索開始位置
 * @param      pos 文字列の検索開始位置
 * @param      pat 検索に使う正規表現
 * @param      pos_r マッチした場合，その要素におけるマッチした部分の文字位置
 * @param      span_r マッチした場合，その要素におけるマッチした部分の文字列長
 * @param      nextidx 次回のidx
 * @param      nextpos 次回のpos
 * @return     成功した場合は配列の要素番号<br>
 *             失敗した場合は負値
 *
 */
ssize_t tarray_tstring::regmatch( size_t idx, size_t pos, const tregex &pat,
				  ssize_t *pos_r, size_t *span_r,
				  size_t *nextidx, size_t *nextpos ) const
{
    ssize_t f_pos = -1;
    size_t a_length = this->length();
    if ( idx < a_length && pos <= this->at_cs(idx).length() ) {
	size_t i;
	for ( i=idx ; i < a_length ; i++ ) {
	    if (0 <= (f_pos=this->at_cs(i).regmatch(pos, pat, span_r, &pos))) {
		if ( pos_r != NULL ) *pos_r = f_pos;
		if ( nextidx != NULL ) {
		    if ( this->at_cs(i).length() < pos ) *nextidx = i + 1;
		    else *nextidx = i;		/* keep */
		}
		if ( nextpos != NULL ) {
		    if ( i + 1 < a_length ) {	/* not last */
			if ( this->at_cs(i).length() < pos ) *nextpos = 0;
			else *nextpos = pos;
		    }
		    else *nextpos = pos;
		}
		return i;
	    }
	    pos = 0;	/* reset */
	}
    }
    /* not found */
    if ( pos_r != NULL ) *pos_r = f_pos;
    if ( nextidx != NULL ) *nextidx = a_length;
    if ( nextpos != NULL ) {
	*nextpos = (0 < a_length) ? this->at_cs(a_length - 1).length() + 1 : 1;
    }
    return -1;
}

/**
 * @brief  配列要素に対する正規表現マッチを試行し，後方参照を含む情報を取得
 *
 *  自身の配列要素の左側から，pat で指定された POSIX 拡張正規表現(以下，正規
 *  表現)にマッチする部分を含む要素を検索し，メンバ関数の返り値として，マッチ
 *  した場合はその要素番号を返し，マッチしなければ負数を返します．同時に，
 *  マッチした場合は文字列配列オブジェクト result に，その要素におけるマッチ
 *  した部分の後方参照を含む情報も返します．
 *
 * @param      pat 検索に使う正規表現
 * @param      result マッチした場合，その要素におけるマッチした部分の結果情報
 * @return     成功した場合は配列の要素番号<br>
 *             失敗した場合は負値
 *
 */
ssize_t tarray_tstring::regmatch( const char *pat,
				  tarray_tstring *result )
{
    if ( result == NULL ) return -1;
    if ( this->regex_rec.compile(pat) < 0 ) {
	err_report1(__FUNCTION__,"ERROR","%s",this->regex_rec.cerrstr());
    }
    return this->regmatch(0, 0, this->regex_rec, result,
			  (size_t *)NULL, (size_t *)NULL);
}

/**
 * @brief  配列要素に対する正規表現マッチを試行し，後方参照を含む情報を取得
 *
 *  自身の配列要素の左側から，pat で指定された POSIX 拡張正規表現(以下，正規
 *  表現)にマッチする部分を含む要素を検索し，メンバ関数の返り値として，マッチ
 *  した場合はその要素番号を返し，マッチしなければ負数を返します．同時に，
 *  マッチした場合は文字列配列オブジェクト result に，その要素におけるマッチ
 *  した部分の後方参照を含む情報も返します．
 *
 * @param      idx 配列要素の検索開始位置
 * @param      pos 文字列の検索開始位置
 * @param      pat 検索に使う正規表現
 * @param      result マッチした場合，その要素におけるマッチした部分の結果情報
 * @return     成功した場合は配列の要素番号<br>
 *             失敗した場合は負値
 *
 */
ssize_t tarray_tstring::regmatch( size_t idx, size_t pos, const char *pat,
				  tarray_tstring *result )
{
    if ( result == NULL ) return -1;
    if ( this->regex_rec.compile(pat) < 0 ) {
	err_report1(__FUNCTION__,"ERROR","%s",this->regex_rec.cerrstr());
    }
    return this->regmatch(idx, pos, this->regex_rec, result,
			  (size_t *)NULL, (size_t *)NULL);
}

/**
 * @brief  連続的に配列要素への正規表現マッチを試行し，後方参照を含む情報を取得
 *
 *  自身の配列要素の左側から，pat で指定された POSIX 拡張正規表現(以下，正規
 *  表現)にマッチする部分を含む要素を検索し，メンバ関数の返り値として，マッチ
 *  した場合はその要素番号を返し，マッチしなければ負数を返します．同時に，
 *  マッチした場合は文字列配列オブジェクト result に，その要素におけるマッチ
 *  した部分の後方参照を含む情報も返します．
 *
 * @param      idx 配列要素の検索開始位置
 * @param      pos 文字列の検索開始位置
 * @param      pat 検索に使う正規表現
 * @param      result マッチした場合，その要素におけるマッチした部分の結果情報
 * @param      nextidx 次回のidx
 * @param      nextpos 次回のpos
 * @return     成功した場合は配列の要素番号<br>
 *             失敗した場合は負値
 *
 */
ssize_t tarray_tstring::regmatch( size_t idx, size_t pos, const char *pat,
				  tarray_tstring *result,
				  size_t *nextidx, size_t *nextpos )
{
    if ( result == NULL ) return -1;
    if ( this->regex_rec.compile(pat) < 0 ) {
	err_report1(__FUNCTION__,"ERROR","%s",this->regex_rec.cerrstr());
    }
    return this->regmatch(idx, pos, this->regex_rec, result, nextidx, nextpos);
}

/**
 * @brief  配列要素への正規表現マッチを試行し，後方参照を含む情報を取得
 *
 *  自身の配列要素の左側から，pat で指定された POSIX 拡張正規表現(以下，正規
 *  表現)にマッチする部分を含む要素を検索し，メンバ関数の返り値として，マッチ
 *  した場合はその要素番号を返し，マッチしなければ負数を返します．同時に，
 *  マッチした場合は文字列配列オブジェクト result に，その要素におけるマッチ
 *  した部分の後方参照を含む情報も返します．
 *
 * @param      pat 検索に使う正規表現
 * @param      result マッチした場合，その要素におけるマッチした部分の結果情報
 * @return     成功した場合は配列の要素番号<br>
 *             失敗した場合は負値
 *
 */
ssize_t tarray_tstring::regmatch( const tstring &pat,
				  tarray_tstring *result )
{
    if ( result == NULL ) return -1;
    if ( this->regex_rec.compile(pat.cstr()) < 0 ) {
	err_report1(__FUNCTION__,"ERROR","%s",this->regex_rec.cerrstr());
    }
    return this->regmatch(0, 0, this->regex_rec, result,
			  (size_t *)NULL, (size_t *)NULL);
}

/**
 * @brief  配列要素への正規表現マッチを試行し，後方参照を含む情報を取得
 *
 *  自身の配列要素の左側から，pat で指定された POSIX 拡張正規表現(以下，正規
 *  表現)にマッチする部分を含む要素を検索し，メンバ関数の返り値として，マッチ
 *  した場合はその要素番号を返し，マッチしなければ負数を返します．同時に，
 *  マッチした場合は文字列配列オブジェクト result に，その要素におけるマッチ
 *  した部分の後方参照を含む情報も返します．
 *
 * @param      idx 配列要素の検索開始位置
 * @param      pos 文字列の検索開始位置
 * @param      pat 検索に使う正規表現
 * @param      result マッチした場合，その要素におけるマッチした部分の結果情報
 * @return     成功した場合は配列の要素番号<br>
 *             失敗した場合は負値
 *
 */
ssize_t tarray_tstring::regmatch( size_t idx, size_t pos, const tstring &pat,
				  tarray_tstring *result )
{
    if ( result == NULL ) return -1;
    if ( this->regex_rec.compile(pat.cstr()) < 0 ) {
	err_report1(__FUNCTION__,"ERROR","%s",this->regex_rec.cerrstr());
    }
    return this->regmatch(idx, pos, this->regex_rec, result,
			  (size_t *)NULL, (size_t *)NULL);
}

/**
 * @brief  連続的に配列要素への正規表現マッチを試行し，後方参照を含む情報を取得
 *
 *  自身の配列要素の左側から，pat で指定された POSIX 拡張正規表現(以下，正規
 *  表現)にマッチする部分を含む要素を検索し，メンバ関数の返り値として，マッチ
 *  した場合はその要素番号を返し，マッチしなければ負数を返します．同時に，
 *  マッチした場合は文字列配列オブジェクト result に，その要素におけるマッチ
 *  した部分の後方参照を含む情報も返します．
 *
 * @param      idx 配列要素の検索開始位置
 * @param      pos 文字列の検索開始位置
 * @param      pat 検索に使う正規表現
 * @param      result マッチした場合，その要素におけるマッチした部分の結果情報
 * @param      nextidx 次回のidx
 * @param      nextpos 次回のpos
 * @return     成功した場合は配列の要素番号<br>
 *             失敗した場合は負値
 *
 */
ssize_t tarray_tstring::regmatch( size_t idx, size_t pos, const tstring &pat,
				  tarray_tstring *result,
				  size_t *nextidx, size_t *nextpos )
{
    if ( result == NULL ) return -1;
    if ( this->regex_rec.compile(pat.cstr()) < 0 ) {
	err_report1(__FUNCTION__,"ERROR","%s",this->regex_rec.cerrstr());
    }
    return this->regmatch(idx, pos, this->regex_rec, result, nextidx, nextpos);
}

/**
 * @brief  配列要素への正規表現マッチを試行し，後方参照を含む情報を取得
 *
 *  自身の配列要素の左側から，pat で指定された POSIX 拡張正規表現(以下，正規
 *  表現)にマッチする部分を含む要素を検索し，メンバ関数の返り値として，マッチ
 *  した場合はその要素番号を返し，マッチしなければ負数を返します．同時に，
 *  マッチした場合は文字列配列オブジェクト result に，その要素におけるマッチ
 *  した部分の後方参照を含む情報も返します．
 *
 * @param      pat 検索に使う正規表現
 * @param      result マッチした場合，その要素におけるマッチした部分の結果情報
 * @return     成功した場合は配列の要素番号<br>
 *             失敗した場合は負値
 *
 */
ssize_t tarray_tstring::regmatch( const tregex &pat,
				  tarray_tstring *result ) const
{
    if ( result == NULL ) return -1;
    return this->regmatch(0, 0, pat, result,
			  (size_t *)NULL, (size_t *)NULL);
}

/**
 * @brief  配列要素への正規表現マッチを試行し，後方参照を含む情報を取得
 *
 *  自身の配列要素の左側から，pat で指定された POSIX 拡張正規表現(以下，正規
 *  表現)にマッチする部分を含む要素を検索し，メンバ関数の返り値として，マッチ
 *  した場合はその要素番号を返し，マッチしなければ負数を返します．同時に，
 *  マッチした場合は文字列配列オブジェクト result に，その要素におけるマッチ
 *  した部分の後方参照を含む情報も返します．
 *
 * @param      idx 配列要素の検索開始位置
 * @param      pos 文字列の検索開始位置
 * @param      pat 検索に使う正規表現
 * @param      result マッチした場合，その要素におけるマッチした部分の結果情報
 * @return     成功した場合は配列の要素番号<br>
 *             失敗した場合は負値
 *
 */
ssize_t tarray_tstring::regmatch( size_t idx, size_t pos, const tregex &pat,
				  tarray_tstring *result ) const
{
    if ( result == NULL ) return -1;
    return this->regmatch(idx, pos, pat, result,
			  (size_t *)NULL, (size_t *)NULL);
}

/**
 * @brief  連続的に配列要素への正規表現マッチを試行し，後方参照を含む情報を取得
 *
 *  自身の配列要素の左側から，pat で指定された POSIX 拡張正規表現(以下，正規
 *  表現)にマッチする部分を含む要素を検索し，メンバ関数の返り値として，マッチ
 *  した場合はその要素番号を返し，マッチしなければ負数を返します．同時に，
 *  マッチした場合は文字列配列オブジェクト result に，その要素におけるマッチ
 *  した部分の後方参照を含む情報も返します．
 *
 * @param      idx 配列要素の検索開始位置
 * @param      pos 文字列の検索開始位置
 * @param      pat 検索に使う正規表現
 * @param      result マッチした場合，その要素におけるマッチした部分の結果情報
 * @param      nextidx 次回のidx
 * @param      nextpos 次回のpos
 * @return     成功した場合は配列の要素番号<br>
 *             失敗した場合は負値
 *
 */
ssize_t tarray_tstring::regmatch( size_t idx, size_t pos, const tregex &pat,
				  tarray_tstring *result,
				  size_t *nextidx, size_t *nextpos ) const
{
    if ( result == NULL ) return -1;
    size_t a_length = this->length();
    if ( result != this && idx < a_length && pos <= this->at_cs(idx).length() ) {
	size_t i;
	for ( i=idx ; i < a_length ; i++ ) {
	    result->regassign(this->at_cs(i), pos, pat, &pos);
	    if ( 0 < result->length() ) {
		if ( nextidx != NULL ) {
		    if ( this->at_cs(i).length() < pos ) *nextidx = i + 1;
		    else *nextidx = i;		/* keep */
		}
		if ( nextpos != NULL ) {
		    if ( i + 1 < a_length ) {	/* not last */
			if ( this->at_cs(i).length() < pos ) *nextpos = 0;
			else *nextpos = pos;
		    }
		    else *nextpos = pos;
		}
		return i;
	    }
	    pos = 0;	/* reset */
	}
    }
    /* not found */
    if ( nextidx != NULL ) *nextidx = a_length;
    if ( nextpos != NULL ) {
	*nextpos = (0 < a_length) ? this->at_cs(a_length - 1).length() + 1 : 1;
    }
    return -1;
}

/**
 * @brief  配列要素への正規表現マッチを試行し，後方参照を含む情報を取得(非推奨)
 * @deprecated  非推奨
 */
ssize_t tarray_tstring::regmatch( const char *pat,
				  tarray_tstring &result )
{
    return this->regmatch(pat, &result);
}

/**
 * @brief  配列要素への正規表現マッチを試行し，後方参照を含む情報を取得(非推奨)
 * @deprecated  非推奨
 */
ssize_t tarray_tstring::regmatch( size_t idx, size_t pos, const char *pat,
				  tarray_tstring &result )
{
    return this->regmatch(idx, pos, pat, &result);
}

/**
 * @brief  配列要素への正規表現マッチを試行し，後方参照を含む情報を取得(非推奨)
 * @deprecated  非推奨
 */
ssize_t tarray_tstring::regmatch( size_t idx, size_t pos, const char *pat,
				  tarray_tstring &result,
				  size_t *nextidx, size_t *nextpos )
{
    return this->regmatch(idx, pos, pat, &result, nextidx, nextpos);
}

/**
 * @brief  配列要素への正規表現マッチを試行し，後方参照を含む情報を取得(非推奨)
 * @deprecated  非推奨
 */
ssize_t tarray_tstring::regmatch( const tstring &pat,
				  tarray_tstring &result )
{
    return this->regmatch(pat, &result);
}

/**
 * @brief  配列要素への正規表現マッチを試行し，後方参照を含む情報を取得(非推奨)
 * @deprecated  非推奨
 */
ssize_t tarray_tstring::regmatch( size_t idx, size_t pos, const tstring &pat,
				  tarray_tstring &result )
{
    return this->regmatch(idx, pos, pat, &result);
}

/**
 * @brief  配列要素への正規表現マッチを試行し，後方参照を含む情報を取得(非推奨)
 * @deprecated  非推奨
 */
ssize_t tarray_tstring::regmatch( size_t idx, size_t pos, const tstring &pat,
				  tarray_tstring &result,
				  size_t *nextidx, size_t *nextpos )
{
    return this->regmatch(idx, pos, pat, &result, nextidx, nextpos);
}

/**
 * @brief  配列要素への正規表現マッチを試行し，後方参照を含む情報を取得(非推奨)
 * @deprecated  非推奨
 */
ssize_t tarray_tstring::regmatch( const tregex &pat,
				  tarray_tstring &result ) const
{
    return this->regmatch(pat, &result);
}

/**
 * @brief  配列要素への正規表現マッチを試行し，後方参照を含む情報を取得(非推奨)
 * @deprecated  非推奨
 */
ssize_t tarray_tstring::regmatch( size_t idx, size_t pos, const tregex &pat,
				  tarray_tstring &result ) const
{
    return this->regmatch(idx, pos, pat, &result);
}

/**
 * @brief  配列要素への正規表現マッチを試行し，後方参照を含む情報を取得(非推奨)
 * @deprecated  非推奨
 */
ssize_t tarray_tstring::regmatch( size_t idx, size_t pos, const tregex &pat,
				  tarray_tstring &result,
				  size_t *nextidx, size_t *nextpos ) const
{
    return this->regmatch(idx, pos, pat, &result, nextidx, nextpos);
}

/**
 * @brief  要素文字列の外部バッファへのコピー
 *
 *  indexで指定された要素の文字列を，外部バッファにコピーします．
 * 
 * @param   index 要素番号
 * @param   dest_buf 外部バッファのアドレス
 * @param   buf_size 外部バッファのサイズ
 * 
 * @return  非負の値: バッファ長が十分な場合に,コピーできる文字数．<br>
 *          負の値(エラー): dest_strにNULLを設定し，buf_sizeに0以外の値を設定
 *          した場合.
 * @throw   配列長以上のIndexが指定された場合
 */
ssize_t tarray_tstring::getstr( size_t index, 
				char *dest_buf, size_t buf_size ) const
{
    if ( index < 0 || this->length() <= index ) {
	return -1;
    }
    else {
        return this->arrs_rec.at_cs(index).getstr(dest_buf,buf_size);
    }
}

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
 * @brief  正規表現マッチ結果のマッチ位置を取得
 *
 *  保存された正規表現マッチ結果の，マッチした部分の位置を返します．
 * 
 * @param  idx 0: マッチした文字列全体の情報<br>
 *             1以降: 正規表現それぞれにマッチした部分文字列の情報
 * 
 * @return 保存されたマッチした部分の位置
 */
size_t tarray_tstring::reg_pos( size_t idx ) const
{
    if ( idx < this->reg_pos_rec.length() )
	return this->reg_pos_rec.z_cs(idx);
    else 
	return 0;
}

/**
 * @brief  テンポラリオブジェクトのためのshallow copy属性の付与 (未実装)
 * @deprecated  未実装
 */
/* 注意: 参照を返したいところだが，return 文でそれをやるとインスタンス */
/*       を2回コピーされるというおかしな事になるので void になっている */
void tarray_tstring::set_scopy_flag()
{
    return;
}

/**
 * @brief  オブジェクト情報を標準エラー出力へ出力
 *
 *  自身のオブジェクト情報を標準エラー出力へ出力します．
 *
 */
void tarray_tstring::dprint( const char *msg ) const
{
    size_t i;
    if ( msg != NULL ) {
	sli__eprintf("%s sli::%s[obj=0x%zx] = {",
		     msg, CLASS_NAME, (const size_t)this);
    }
    else {
	sli__eprintf("sli::%s[obj=0x%zx] = {",CLASS_NAME,(const size_t)this);
    }
    for ( i=0 ; i < this->length() ; i++ ) {
	if ( i != 0 ) sli__eprintf(", ");
	dump_tstring(this->at_cs(i));
    }
    sli__eprintf("}\n");
    return;
}


/*
 * private member functions
 */
 
/**
 * @brief   cstr_ptrs_rec を更新
 * 
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 * @note    このメンバ関数はprivateです 
 */
 tarray_tstring &tarray_tstring::update_cstr_ptrs_rec()
{
    size_t i;
    const char **ptrs;
    this->cstr_ptrs_rec.resize(this->arrs_rec.length() + 1);
    ptrs = (const char **)(this->cstr_ptrs_rec.data_ptr());
    for ( i=0 ; i < this->arrs_rec.length() ; i++ ) {
	this->arrs_rec[i].register_extptr(ptrs + i);
	ptrs[i] = this->arrs_rec[i].cstr();
    }
    ptrs[i] = NULL;

    return *this;
}

/**
 * @brief   ptrが指し示すアドレスが自身の文字列要素かをチェック
 * 
 * @param   ptr アドレス
 * @return  自身の領域内の時は真<br>
 *          それ以外の時は偽
 * @note    このメンバ関数はprivateです 
 */
bool tarray_tstring::is_my_buffer( const char *const *ptr ) const
{
    const char *const *cstr_ptrs = this->cstrarray();
    if ( ptr == NULL || cstr_ptrs == NULL ) return false;
    if ( cstr_ptrs <= ptr && ptr < cstr_ptrs + this->length() + 1 )
	return true;
    else
	return false;
}

/**
 * @brief  正規表現によるマッチを試行(private)
 *
 *  文字列 target に対し，regex_ref で指定された POSIX 拡張正規表現
 *  による文字列マッチを試行し，文字列配列として自身に格納します．
 * 
 * @param   target マッチング対象のtstringオブジェクト
 * @param   pos 文字列マッチの開始位置
 * @param   regex_ref tregex クラスのコンパイル済オブジェクト
 * @param   nextpos 次回のpos
 * @return  最初にマッチした位置<br>
 *          失敗した場合は負値
 * @throw   regexルーチンがメモリを使い果たしている場合
 * @throw   内部バッファの確保に失敗した場合
 * @throw   メモリ破壊を起こした場合
 * @note    このメンバ関数はprivateです
 */
ssize_t tarray_tstring::regexp_match_advanced( const tstring &target,
					       size_t pos,
					       const tregex &regex_ref,
					       size_t *nextpos )
{
    int status;
    size_t to_append, n_result, i, len_target_str = 0;
    const char *str_ptr;
    const char *target_str;
    tstring bak_target;

    /* 自身が保持する文字列の場合はコピーしたものを対象にする */
    for ( i=0 ; i < this->length() ; i++ ) {
	if ( &(this->at_cs(i)) == &target ) break;
    }
    if ( i < this->length() ) {	/* 見つかった場合 */
	bak_target.init(target);
	target_str = bak_target.cstr();
	len_target_str = bak_target.length();
    }
    else {
	target_str = target.cstr();
	len_target_str = target.length();
    }

    this->erase();

    if ( target_str == NULL ) goto quit;
    if ( len_target_str < pos ) goto quit;
    if ( regex_ref.cregex() == NULL ) goto quit;

    to_append = 1;
    str_ptr = regex_ref.cstr();
    /* '(' の個数を数え，それを追加分にする．'\\(' もカウントされるがOK */
    if ( str_ptr != NULL ) {
	for ( i=0 ; str_ptr[i] != '\0' ; i++ ) {
	    if ( str_ptr[i] == '(' ) to_append++;
	}
    }

    this->reg_pos_rec.resize(1);
    this->reg_length_rec.resize(1);
    do {
	this->reg_pos_rec.resizeby(to_append);
	this->reg_length_rec.resizeby(to_append);
	/* */
	status = c_regsearchx(regex_ref.cregex(), target_str + pos, 
			      (0 < pos), false, 
			      this->reg_pos_rec.length(),
			      (size_t *)(this->reg_pos_rec.data_ptr()),
			      (size_t *)(this->reg_length_rec.data_ptr()),
			      &n_result);
	if ( status != 0 ) {
	   this->init_reg_results();
	   if ( c_regfatal(status) ) {
	     err_throw(__FUNCTION__,"FATAL","Out of memory in c_regsearchx()");
	   }
	   goto quit;
	}
	if ( n_result == 0 ) {	/* never */
	    goto quit;
	}
    } while ( this->reg_pos_rec.length() <= n_result );

    this->reg_pos_rec.resize(n_result);
    this->reg_length_rec.resize(n_result);
    this->resize(n_result);

    for ( i=0 ; i < n_result ; i++ ) {
	this->reg_pos_rec.z(i) += pos;
	this->at(i).assign(target_str + this->reg_pos_rec.z(i),
			   this->reg_length_rec.z(i));
    }

    if ( 0 < n_result ) {
	ssize_t ret = this->reg_pos_rec.z(0);
	size_t span = this->reg_length_rec.z(0);
	if ( nextpos != NULL ) {
	    size_t add_span = ((span == 0) ? 1 : span);
	    if ( 0 <= ret ) *nextpos = ret + add_span;
	    else *nextpos = len_target_str + 1;
	}
	return ret;
    }

 quit:
    if ( nextpos != NULL ) *nextpos = len_target_str + 1;
    this->init_reg_results();
    return -1;
}

/**
 * @brief  正規表現の結果の格納先を初期化
 *
 * @note   このメンバ関数はprivateです 
 */
void tarray_tstring::init_reg_results()
{
    this->reg_pos_rec.init(sizeof(size_t), true);
    this->reg_length_rec.init(sizeof(size_t), true);
    return;
}

static const tstring junk;

/**
 * @brief  at() 専用のメンバ関数 (private)
 *
 * @note   このメンバ関数はprivateです 
 */
tstring &tarray_tstring::tstring_ref_after_resize( size_t index )
{
    this->resize(index + 1);
    return this->arrs_rec.at(index);
}

/**
 * @brief  at() 専用のメンバ関数 (private)
 *
 * @note   このメンバ関数はprivateです 
 */
const tstring &tarray_tstring::err_throw_const_tstring_ref( const char *fnc,
					const char *lv, const char *mes ) const
{
    err_throw(fnc,lv,mes);
    return junk;		/* never */
}

}	/* namespace sli */


#include "private/c_memcpy.cc"
#include "private/c_strncmp.cc"
#include "private/c_isalpha.cc"
#include "private/c_regsearchx.c"
#include "private/c_regfatal.c"

