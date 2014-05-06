/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2013-05-12 00:00:46 cyamauch> */

/**
 * @file   tregex.cc
 * @brief  POSIX拡張正規表現を扱うための基本的なクラス tregex のコード
 */

#define CLASS_NAME "tregex"

#include "config.h"

#include "tregex.h"

#include <stdlib.h>

#include "private/err_report.h"

#include "private/c_strcmp.h"
#include "private/c_strdup.h"
#include "private/c_regex.h"
#include "private/c_regfatal.h"
#include "private/c_regsearchx.h"
#include "private/memswap.h"

namespace sli
{

/**
 * @brief  コンストラクタ
 *
 */
tregex::tregex()
{
    this->regstr_rec = NULL;
    this->preg_rec = NULL;
    this->errstr_rec[0] = '\0';

    this->shallow_copy_ok = false;
    this->shallow_copy_dest_obj = NULL;
    this->shallow_copy_src_obj = NULL;
    this->__copying = false;

    return;
}

/**
 * @brief  コンストラクタ
 *
 *  指定された正規表現をコンパイルし，その結果を自身に格納します．
 * 
 * @param  regstr コンパイルされる正規表現文字列
 * @throw  メモリが不足している場合．内部バッファの確保に失敗した場合．
 * @note   コンパイルエラーを検出したい場合は，.compile() を使うと便利です．
 */
tregex::tregex(const char *regstr)
{
    this->regstr_rec = NULL;
    this->preg_rec = NULL;
    this->errstr_rec[0] = '\0';

    this->shallow_copy_ok = false;
    this->shallow_copy_dest_obj = NULL;
    this->shallow_copy_src_obj = NULL;
    this->__copying = false;

    if ( this->compile(regstr) < 0 ) {
	//err_report(__FUNCTION__,"WARNING","this->compile() reports error");
    }

    return;
}

/**
 * @brief  コピーコンストラクタ
 *
 *  objの内容で自身を初期化します．
 *
 * @param  obj 源泉となるオブジェクト
 */
tregex::tregex(const tregex &obj)
{
    this->regstr_rec = NULL;
    this->preg_rec = NULL;
    this->errstr_rec[0] = '\0';

    this->shallow_copy_ok = false;
    this->shallow_copy_dest_obj = NULL;
    this->shallow_copy_src_obj = NULL;
    this->__copying = false;

    this->init(obj);

    return;
}

/**
 * @brief  デストラクタ
 * 
 */
tregex::~tregex()
{
    if ( this->regstr_rec != NULL ) {
	free(this->regstr_rec);
    }
    if ( this->preg_rec != NULL ) {
	c_regfree(this->preg_rec);
    }
    return;
}

/**
 * @brief  オブジェクトのコピー
 *
 *  演算子の右側(引数) で指定された tregex オブジェクトを自身にコピーします．
 *
 * @param   obj 源泉となるオブジェクト
 * @return  自身の参照
 */
tregex &tregex::operator=(const tregex &obj)
{
    this->init(obj);
    return *this;
}


/**
 * @brief  指定された正規表現をコンパイルし自身に格納
 *
 *  演算子の右側(引数) で指定された正規表現をコンパイルし，その結果を自身に
 *  格納します．
 *
 * @param  regstr コンパイルされる正規表現文字列
 * @throw  メモリが不足している場合．内部バッファの確保に失敗した場合．
 * @note   コンパイルエラーを検出したい場合は，.compile() を使うと便利です．
 */
tregex &tregex::operator=(const char *regstr)
{
    if ( this->compile(regstr) < 0 ) {
	//err_report(__FUNCTION__,"WARNING","this->compile() reports error");
    }
    return *this;
}

/**
 * @brief   オブジェクトの初期化
 * 
 * @return  自身の参照
 */
tregex &tregex::init()
{
    if ( this->regstr_rec != NULL ) {
	free(this->regstr_rec);
	this->regstr_rec = NULL;
    }
    if ( this->preg_rec != NULL ) {
	c_regfree(this->preg_rec);
	this->preg_rec = NULL;
    }
    this->errstr_rec[0] = '\0';
    return *this;
}

/**
 * @brief  オブジェクトのコピー
 *
 *  obj の内容を自身にコピーします．
 *
 * @param   obj tregexオブジェクト
 * @return  自身の参照
 * @throw   メモリが不足している場合．内部バッファの確保に失敗した場合．
 */
tregex &tregex::init(const tregex &obj)
{
    if ( &obj == this ) return *this;
    this->tregex::init();
    if ( obj.cstr() != NULL ) this->tregex::compile(obj.cstr());
    return *this;
}

/* エラーチェックは cregex() でも可能．cregex() の返り値が NULL なら */
/* コンパイルに失敗している．                                        */

/**
 * @brief  指定された正規表現をコンパイルし 結果を自身に格納 (返り値はstatus)
 *
 *  正規表現 regstr をコンパイルし，正規表現とコンパイル結果とをオブジェクトに
 *  格納します．
 * 
 * @param   regstr コンパイルされる正規表現文字列
 * @return  0: 正常終了 <br>
 *          負の値: 正規表現文字列が不正な場合
 * @throw   メモリが不足している場合．内部バッファの確保に失敗した場合．
 */
int tregex::compile(const char *regstr)
{
    int status;
    void *tmp_preg;

    if ( this->regstr_rec != NULL && regstr != NULL &&
	 c_strcmp(this->regstr_rec,regstr) == 0 ) return 0;	/* OK */

    if ( this->regstr_rec != NULL ) {
	free(this->regstr_rec);
	this->regstr_rec = NULL;
    }
    if ( this->preg_rec != NULL ) {
	c_regfree(this->preg_rec);
	this->preg_rec = NULL;
    }
    this->errstr_rec[0] = '\0';

    if ( regstr == NULL ) return -1;				/* Invalid */

    status = c_regcomp(&tmp_preg, regstr);
    if ( status != 0 ) {
	if ( c_regfatal(status) ) {
	    c_regfree(tmp_preg);
	    err_throw(__FUNCTION__,"FATAL","Out of memory in c_regcomp()");
	}
	c_regerror(status,tmp_preg,this->errstr_rec,192);
	c_regfree(tmp_preg);
	return -1;						/* Error */
    }

    /* register */
    this->preg_rec = tmp_preg;
    this->regstr_rec = c_strdup(regstr);
    if ( this->regstr_rec == NULL ) {
	c_regfree(this->preg_rec);
	this->preg_rec = NULL;
	err_throw(__FUNCTION__,"FATAL","c_strdup() failed");
    }

    return 0;
}

/**
 * @brief  2つのオブジェクト間での内容のスワップ
 *
 *  自身の内容と sobj の内容とを入れ替えます．
 *
 * @param   sobj tregex オブジェクト
 * @return  自身の参照
 */
tregex &tregex::swap(tregex &sobj)
{
    if ( &sobj == this ) return *this;

    char *tmp__regstr_rec;
    void *tmp__preg_rec;

    tmp__regstr_rec = sobj.regstr_rec;
    tmp__preg_rec  = sobj.preg_rec;
    sobj.regstr_rec = this->regstr_rec;
    sobj.preg_rec  = this->preg_rec;
    this->regstr_rec = tmp__regstr_rec;
    this->preg_rec  = tmp__preg_rec;

    memswap(this->errstr_rec, sobj.errstr_rec, 192);

    return *this;
}

/**
 * @brief  自身に格納されている正規表現文字列を返す
 *
 * @return  格納されている正規表現文字列
 */
const char *tregex::cstr() const
{
    return this->regstr_rec;
}

/**
 * @brief  自身に格納されている regex_t オブジェクトのアドレスを返す
 * 
 *  自身に格納されている regex_t オブジェクトのアドレスを返します．<br>
 *  このメンバ関数が NULL を返す場合，compile() に失敗している事を示します．
 *
 * @return  自身に格納されている regex_t オブジェクトのアドレス
 */
const void *tregex::cregex() const
{
    return this->preg_rec;
}

/**
 * @brief  指定された文字列について，正規表現によりマッチングを試行
 *
 *  文字列 str について，自身に格納された正規表現によりマッチングを行います．
 * 
 * @param  str 検索対象文字列
 * @param  notbol true の場合，行頭マッチのオペレータを必ず失敗させる
 * @param  noteol true の場合，行末マッチのオペレータを必ず失敗させる
 * @param  max_nelem 検索上限の個数
 * @param  pos_r[] マッチした位置が格納される
 * @param  len_r[] マッチした文字列の要素数が格納される
 * @param  nelem_r マッチした個数が格納される
 * @return  0: マッチング成功 <br>
 *          0以外: マッチング失敗
 */
int tregex::exec( const char *str, bool notbol, bool noteol,
		  size_t max_nelem, size_t pos_r[], size_t len_r[],
		  size_t *nelem_r ) const
{
    return c_regsearchx(this->cregex(), str, notbol, noteol,
			max_nelem, pos_r, len_r, nelem_r);
}

/**
 * @brief  コンパイル時のエラーメッセージを取得
 *
 *  自身に格納されている コンパイル時のエラーメッセージを返します．<br>
 *  これが NULL を返す場合，compile() に成功している事を示します．
 *
 * @return  格納されているエラーメッセージ: コンパイルに失敗した時 <br>
 *          NULL: コンパイルに成功した時
 */
const char *tregex::cerrstr() const
{
    if ( this->preg_rec != NULL ) return NULL;
    else return this->errstr_rec;
}

/**
 * @brief  テンポラリオブジェクトのためのshallow copy属性の付与 (未実装)
 * @deprecated  未実装
 *
 */
/* 注意: 参照を返したいところだが，return 文でそれをやるとインスタンス */
/*       を2回コピーされるというおかしな事になるので void になっている */
void tregex::set_scopy_flag()
{
    this->shallow_copy_ok = true;
    return;
}

/**
 * @brief  shallow copy が可能かを返す (未実装)
 * 
 * @return  shallow copyが可能なら真<br>
 *          それ以外の時は偽
 * @note    このメンバ関数は private です．
 */
bool tregex::request_shallow_copy( tregex *from_obj ) const
{
    return false;
}


}

#include "private/c_strcmp.cc"
#include "private/c_strdup.c"
#include "private/c_regex.c"
#include "private/c_regfatal.c"
#include "private/c_regsearchx.c"
#include "private/memswap.cc"

