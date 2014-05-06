/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2013-05-07 00:24:49 cyamauch> */

/**
 * @file   ctindex.cc
 * @brief  キー文字列と番号との関係を管理するクラス ctindex のコード
 */

#define CLASS_NAME "ctindex"

#include "config.h"

#include "ctindex.h"

#include <stdlib.h>
#include <math.h>

#include "private/err_report.h"

#include "private/c_memmove.h"


namespace sli
{

/* constructor */

/**
 * @brief  コンストラクタ
 *
 */
ctindex::ctindex()
{
    this->_index_rec = NULL;
    this->index_alloc_blen_rec = 0;
    this->index_rec_size = 0;
    this->ctable_begin_ch = 0;
    this->ctable_size = 0;
    this->_ctable_rec = NULL;
    this->ctable_alloc_blen_rec = 0;

    this->shallow_copy_ok = false;
    this->shallow_copy_dest_obj = NULL;
    this->shallow_copy_src_obj = NULL;
    this->__copying = false;

    return;
}

/* copy constructor */

/**
 * @brief  コピーコンストラクタ
 *
 *  obj で自身を初期化します．
 * 
 * @param  obj ctindexオブジェクト
 * @throw  内部バッファの確保に失敗した場合
 * @throw  メモリ破壊を起こしたとき
 */
ctindex::ctindex(const ctindex &obj)
{
    this->_index_rec = NULL;
    this->index_alloc_blen_rec = 0;
    this->index_rec_size = 0;
    this->ctable_begin_ch = 0;
    this->ctable_size = 0;
    this->_ctable_rec = NULL;
    this->ctable_alloc_blen_rec = 0;

    this->shallow_copy_ok = false;
    this->shallow_copy_dest_obj = NULL;
    this->shallow_copy_src_obj = NULL;
    this->__copying = false;

    this->init(obj);

    return;
}

/* destructor */
/**
 * @brief  デストラクタ
 *
 */
ctindex::~ctindex()
{
    if ( this->_index_rec != NULL ) {
	free(this->_index_rec);
    }
    if ( this->_ctable_rec != NULL ) {
	int i;
	for ( i=0 ; i < this->ctable_size ; i++ ) {
	    if ( this->_ctable_rec[i] != NULL ) {
		delete this->_ctable_rec[i];
	    }
	}
	free(this->_ctable_rec);
    }

    return;
}

/**
 * @brief  オブジェクトのコピー
 *
 *  演算子の右側(引数) で指定された ctindex オブジェクトを自身にコピーします．
 *
 * @param   obj ctindexクラスのオブジェクト
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合<br>
 *          メモリ破壊を起こした場合
 */
ctindex &ctindex::operator=(const ctindex &obj)
{
    this->init(obj);
    return *this;
}

/*
 * public member functions
 */

/**
 * @brief  オブジェクトの初期化
 *
 *  オブジェクトの初期化を行います．
 *
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
ctindex &ctindex::init()
{
    this->free_index_rec();
    if ( this->_ctable_rec != NULL ) {
	int i;
	for ( i=0 ; i < this->ctable_size ; i++ ) {
	    if ( this->_ctable_rec[i] != NULL ) {
		delete this->_ctable_rec[i];
	    }
	}
	this->free_ctable_rec();
    }
    this->index_rec_size = 0;
    this->ctable_begin_ch = 0;
    this->ctable_size = 0;
    return *this;
}

/**
 * @brief  オブジェクトのコピー
 *
 *  引数で指定された ctindex オブジェクトを自身にコピーします．
 *
 * @param   obj ctindexクラスのオブジェクト
 * @return  自身の参照
 * @throw   メモリ破壊を起こした場合
 */
ctindex &ctindex::init(const ctindex &obj)
{
    if ( &obj == this ) return *this;

    this->ctindex::init();

    if ( obj._index_rec != NULL ) {
	int i;
	if ( this->realloc_index_rec(obj.index_rec_size) < 0 ) {
            err_throw(__FUNCTION__,"FATAL","realloc() failed");
	}
	for ( i=0 ; i < obj.index_rec_size ; i++ ) {
	    this->_index_rec[i] = obj._index_rec[i];
	}
    }
    if ( obj._ctable_rec != NULL ) {
	int i;
	if ( this->realloc_ctable_rec(obj.ctable_size) < 0 ) {
	    this->free_index_rec();
            err_throw(__FUNCTION__,"FATAL","realloc() failed");
	}
	for ( i=0 ; i < obj.ctable_size ; i++ ) this->_ctable_rec[i] = NULL;

	for ( i=0 ; i < obj.ctable_size ; i++ ) {
	    if ( obj._ctable_rec[i] != NULL ) {
		try {
		    this->_ctable_rec[i] = new ctindex;
		    *(this->_ctable_rec[i]) = *(obj._ctable_rec[i]);
		}
		catch (...) {
		    for ( i=0 ; i < obj.ctable_size ; i++ ) {
			if ( this->_ctable_rec[i] != NULL ) {
			    delete this->_ctable_rec[i];
			}
		    }
		    this->free_ctable_rec();
		    this->free_index_rec();
		    err_throw(__FUNCTION__,"FATAL","'new' of '=' failed");
		}
	    }
	}
    }
    this->index_rec_size = obj.index_rec_size;
    this->ctable_begin_ch = obj.ctable_begin_ch;
    this->ctable_size = obj.ctable_size;

    return *this;
}


/**
 * @brief  インデックス値を追加
 *
 *  ctindex オブジェクトに，キー文字列 key とそれに対応するインデックス値 index
 *  を追加します．
 *
 * @param   key キーとなる文字列
 * @param   index キーに対応する任意のインデックス値
 * @return  成功した場合は0<br>
 *          失敗した場合は負値
 * @throw   内部バッファの確保に失敗した場合
 */
int ctindex::append( const char *key, size_t index )
{
    int return_status = -1;

    if ( key == NULL ) goto quit;

    //fprintf(stderr,"debug: append: key = [%s]\n",key);

    if ( key[0] == '\0' ) {
	int i;
	for ( i=0 ; i < this->index_rec_size ; i++ ) {
	    if ( this->_index_rec[i] == index ) {
		/* already registered */
		goto quit;
	    };
	}
	if ( this->realloc_index_rec(this->index_rec_size + 1) < 0 ) {
            err_throw(__FUNCTION__,"FATAL","realloc() failed");
	}
	this->_index_rec[this->index_rec_size] = index;
	this->index_rec_size ++;
    }
    else {
	int key0 = ((unsigned char *)key)[0];
	ctindex *next = this->configure_ctable(key0);
	if ( next == NULL ) {
            err_report(__FUNCTION__,"ERROR","this->configure_ctable() failed");
            goto quit;
	}
	if ( next->append(key + 1,index) < 0 ) {
            err_report(__FUNCTION__,"ERROR","next->append() failed");
            goto quit;
	}
    }

    return_status = 0;
 quit:
    return return_status;
}

/**
 * @brief  インデックス値の変更
 *
 *  キー文字列 key に対応するインデックス値を変更します．
 *
 * @param   key キーとなる文字列
 * @param   current_index 現在のインデックス値
 * @param   new_index 変更後のインデックス値
 * @return  成功した場合は0<br>
 *          失敗した場合は負値
 * @note  1つのキーに複数のインデックスを持たせる事もできるため，current_index
 *        が必要になっています．
 */
int ctindex::update( const char *key, size_t current_index, size_t new_index )
{
    int return_val = -1;

    if ( key == NULL ) goto quit;

    if ( key[0] == '\0' ) {
	int i;
	for ( i=0 ; i < this->index_rec_size ; i++ ) {
	    if ( this->_index_rec[i] == current_index ) {
		this->_index_rec[i] = new_index;
		return_val = 0;
		break;
	    }
	}
    }
    else {
	int key0 = ((unsigned char *)key)[0];
	if ( this->ctable_begin_ch <= key0 &&
	     key0 < this->ctable_begin_ch + this->ctable_size ) {
	    int next_idx = key0 - this->ctable_begin_ch;
	    if ( this->_ctable_rec[next_idx] != NULL ) {
		return_val = this->_ctable_rec[next_idx]->update(key + 1,
							    current_index,
							    new_index);
	    }
	}
    }
 quit:
    return return_val;
}

/**
 * @brief  インデックス値の削除
 *
 *  キー文字列 key に対応するインデックス値 index を削除します．
 *
 * @param   key キーとなる文字列
 * @param   index インデックス値
 * @return  成功した場合は0以上の値<br>
 *          失敗した場合は負値
 * @throw   内部バッファの確保に失敗した場合
 */
int ctindex::erase( const char *key, size_t index )
{
    int return_val = -1;

    //fprintf(stderr,"debug: erase: key = [%s]\n",key);

    if ( key == NULL ) goto quit;

    if ( key[0] == '\0' ) {
	int i;
	if ( this->index_rec_size == 0 ) {
	    /* not found */
	    goto quit;
	}
	for ( i=0 ; i < this->index_rec_size ; i++ ) {
	    if ( this->_index_rec[i] == index ) break;
	}
	for ( ; i < this->index_rec_size - 1 ; i++ ) {
	    this->_index_rec[i] = this->_index_rec[i+1];
	}
	if ( i < this->index_rec_size ) {
	    if ( this->index_rec_size == 1 ) {
		this->free_index_rec();
		this->index_rec_size = 0;
	    }
	    else {
		if ( this->realloc_index_rec(this->index_rec_size - 1) < 0 ) {
		    this->index_rec_size --;
		    err_throw(__FUNCTION__,"FATAL","realloc() failed");
		}
		this->index_rec_size --;
	    }
	}
	else {
	    /* not found */
	    goto quit;
	}
	//this->index_rec = -1;
	//fprintf(stderr,"debug: erase: index_rec = [%d]\n",this->index_rec);
    }
    else {
	int key0 = ((unsigned char *)key)[0];
	if ( this->ctable_begin_ch <= key0 &&
	     key0 < this->ctable_begin_ch + this->ctable_size ) {
	    int next_idx = key0 - this->ctable_begin_ch;

	    if ( this->_ctable_rec[next_idx] == NULL ) {
		/* invalid key! */
		goto quit;
	    }
	    else {
		/* 消しても OK なら next_rq が 0 になる */
		int next_rq = this->_ctable_rec[next_idx]->erase(key + 1,index);
		if ( next_rq < 0 ) {
		    /* invalid key! */
		    goto quit;
		}
		else if ( next_rq == 0 ) {
		    delete this->_ctable_rec[next_idx];
		    this->_ctable_rec[next_idx] = NULL;
		    if ( next_idx == 0 ) {
			int i;
			/* 前から順に残りものを探す */
			for ( i=0 ; i < this->ctable_size ; i++ ) {
			    if ( this->_ctable_rec[i] != NULL ) break;
			}
			if ( i == this->ctable_size ) {	/* 1つも無い場合 */
			    this->free_ctable_rec();
			    this->ctable_size = 0;
			    this->ctable_begin_ch = 0;
			}
			else {
			    int new_size = this->ctable_size - i;
			    c_memmove(this->_ctable_rec, this->_ctable_rec + i,
				      sizeof(*(this->_ctable_rec)) * new_size);
			    this->ctable_size = new_size;
			    this->ctable_begin_ch += i;

			    if ( this->realloc_ctable_rec(new_size) < 0 ) {
				err_throw(__FUNCTION__,"FATAL",
					  "realloc() failed");
			    }
			}
		    }
		    else if ( next_idx == this->ctable_size -1 ) {
			int i;
			for ( i=this->ctable_size ; 0 < i ; i-- ) {
			    if ( this->_ctable_rec[i-1] != NULL ) break;
			}
			if ( i == 0 ) {
			    this->free_ctable_rec();
			    this->ctable_size = 0;
			    this->ctable_begin_ch = 0;
			}
			else {
			    int new_size = i;

			    this->ctable_size = new_size;

			    if ( this->realloc_ctable_rec(new_size) < 0 ) {
				err_throw(__FUNCTION__,"FATAL",
					  "realloc() failed");
			    }
			}
		    }
		}
	    }
	}
	else {
	    /* invalid key! */
	    goto quit;
	}
    }

    if ( 0 < this->ctable_size || this->_index_rec != NULL ) return_val = 1;
    else return_val = 0;
 quit:
    //fprintf(stderr,"debug: erase: [char='%c'][ret=%d][sz=%d]\n",
    //	    key[0],return_val,this->ctable_size);
    return return_val;
}

/**
 * @brief  インデックス値の取得
 *
 *  キー文字列 key に対応するインデックス値を取得します．
 *
 * @param   key キーとなる文字列
 * @param   index_of_index インデックスの番号．1つのキーに複数のインデックス値
 *          が登録されている場合に指定．
 * @return  成功した場合は正値<br>
 *          失敗した場合は負値
 */
ssize_t ctindex::index( const char *key, int index_of_index ) const
{
    ssize_t return_val = -1;

    if ( key == NULL ) goto quit;

    if ( key[0] == '\0' ) {
	if ( 0 <= index_of_index && index_of_index < this->index_rec_size ) {
	    return_val = this->_index_rec[index_of_index];
	}
    }
    else {
	int key0 = ((unsigned char *)key)[0];
	if ( this->ctable_begin_ch <= key0 &&
	     key0 < this->ctable_begin_ch + this->ctable_size ) {
	    int next_idx = key0 - this->ctable_begin_ch;
	    if ( this->_ctable_rec[next_idx] != NULL ) {
		return_val = this->_ctable_rec[next_idx]->index(key + 1,
							   index_of_index);
	    }
	}
    }
 quit:
    return return_val;
}

/**
 * @brief  2つのオブジェクト間での内容のスワップ
 *
 *  自身の内容と sobj の内容とを入れ替えます．
 * 
 * @param   sobj ctindex オブジェクト
 * @return  自身の参照 
 */
ctindex &ctindex::swap( ctindex &sobj )
{
    if ( &sobj == this ) return *this;

    size_t *tmp__index_rec;
    size_t tmp__index_alloc_blen_rec;
    int tmp__index_rec_size;
    int tmp__ctable_begin_ch;
    int tmp__ctable_size;
    ctindex **tmp__ctable_rec;
    size_t tmp__ctable_alloc_blen_rec;

    tmp__index_rec       = sobj._index_rec;
    tmp__index_alloc_blen_rec = sobj.index_alloc_blen_rec;
    tmp__index_rec_size  = sobj.index_rec_size;
    tmp__ctable_begin_ch = sobj.ctable_begin_ch;
    tmp__ctable_size     = sobj.ctable_size;
    tmp__ctable_rec      = sobj._ctable_rec;
    tmp__ctable_alloc_blen_rec = sobj.ctable_alloc_blen_rec;

    sobj._index_rec       = this->_index_rec;
    sobj.index_alloc_blen_rec = this->index_alloc_blen_rec;
    sobj.index_rec_size  = this->index_rec_size;
    sobj.ctable_begin_ch = this->ctable_begin_ch;
    sobj.ctable_size     = this->ctable_size;
    sobj._ctable_rec     = this->_ctable_rec;
    sobj.ctable_alloc_blen_rec = this->ctable_alloc_blen_rec;

    this->_index_rec       = tmp__index_rec;
    this->index_alloc_blen_rec = tmp__index_alloc_blen_rec;
    this->index_rec_size  = tmp__index_rec_size;
    this->ctable_begin_ch = tmp__ctable_begin_ch;
    this->ctable_size     = tmp__ctable_size;
    this->_ctable_rec     = tmp__ctable_rec;
    this->ctable_alloc_blen_rec = tmp__ctable_alloc_blen_rec;

    return *this;
}

/**
 * @brief  テンポラリオブジェクトのためのshallow copy属性の付与 (未実装)
 * @deprecated  未実装
 *
 */
/* 注意: 参照を返したいところだが，return 文でそれをやるとインスタンス */
/*       を2回コピーされるというおかしな事になるので void になっている */
void ctindex::set_scopy_flag()
{
    this->shallow_copy_ok = true;
    return;
}


/*
 * private member functions
 */

/**
 * @brief  必要に応じて，木の枝を追加
 *
 *  キーとなる文字chを自身が保持しているかを確認し，必要に応じて木の枝を追加
 *  します．
 *
 * @param   ch キー文字列の最初あるいは途中の1文字
 * @return  成功した場合はctindexオブジェクトの間接参照<br>
 *          失敗した場合はNULL
 * @throw   内部バッファの確保に失敗した場合
 * @note    このメンバ関数は private です．
 */
ctindex *ctindex::configure_ctable( int ch )
{
    ctindex *return_ptr = NULL;
    if ( ch <= 0 ) {
	err_report(__FUNCTION__,"ERROR","invalid ch arg");
	goto quit;
    }

    if ( this->_ctable_rec == NULL ) {
	ctindex *tmp_obj = NULL;
	try {
	    tmp_obj = new ctindex;
	}
	catch (...) {
            err_throw(__FUNCTION__,"FATAL","'new' failed");
	}
	if ( this->realloc_ctable_rec(1) < 0 ) {
	    delete tmp_obj;
            err_throw(__FUNCTION__,"FATAL","realloc() failed");
	}
	this->_ctable_rec[0] = tmp_obj;
	this->ctable_begin_ch = ch;
	this->ctable_size = 1;
	return_ptr = tmp_obj;
    }
    else {
	if ( this->ctable_begin_ch <= ch && 
	     ch < this->ctable_begin_ch + this->ctable_size ) {
	    int i = ch - this->ctable_begin_ch;
	    if ( this->_ctable_rec[i] == NULL ) {
		ctindex *tmp_obj = NULL;
		try {
		    tmp_obj = new ctindex;
		}
		catch (...) {
		    err_throw(__FUNCTION__,"FATAL","'new' failed");
		}
		this->_ctable_rec[i] = tmp_obj;
		return_ptr = tmp_obj;
	    }
	    else return_ptr = this->_ctable_rec[i];
	}
	else if ( ch < this->ctable_begin_ch ) {
	    int tdiff = this->ctable_begin_ch - ch;	/* 増やす個数 */
	    int new_size = tdiff + this->ctable_size;	/* 新規サイズ */
	    ctindex *tmp_obj = NULL;
	    int i;

	    try {
		tmp_obj = new ctindex;
	    }
	    catch (...) {
		err_throw(__FUNCTION__,"FATAL","'new' failed");
	    }

	    if ( this->realloc_ctable_rec(new_size) < 0 ) {
		delete tmp_obj;
		err_throw(__FUNCTION__,"FATAL","realloc() failed");
	    }
	    c_memmove(this->_ctable_rec + tdiff, this->_ctable_rec, 
		      sizeof(*(this->_ctable_rec)) * this->ctable_size);
	    this->_ctable_rec[0] = tmp_obj;

	    for ( i=1 ; i < tdiff ; i++ ) this->_ctable_rec[i] = NULL;
	    this->ctable_begin_ch = ch;
	    this->ctable_size = new_size;
	    return_ptr = tmp_obj;
	}
	else {	/* this->ctable_begin_ch + this->ctable_size <= ch */
	    int tdiff = 1 + ch - 
	      (this->ctable_begin_ch + this->ctable_size);  /* 増やす個数 */
	    int new_size = tdiff + this->ctable_size;	    /* 新規サイズ */
	    ctindex *tmp_obj = NULL;
	    int i;

	    try {
		tmp_obj = new ctindex;
	    }
	    catch (...) {
		err_throw(__FUNCTION__,"FATAL","'new' failed");
	    }

	    if ( this->realloc_ctable_rec(new_size) < 0 ) {
		delete tmp_obj;
		err_throw(__FUNCTION__,"FATAL","realloc() failed");
	    }

	    for ( i=0 ; i < tdiff - 1 ; i++ ) 
		this->_ctable_rec[this->ctable_size + i] = NULL;
	    this->_ctable_rec[this->ctable_size + i] = tmp_obj;
	    this->ctable_size = new_size;
	    return_ptr = tmp_obj;
	}
    }
    
 quit:
    return return_ptr;
}

/**
 * @brief  メモリ確保用メンバ関数
 *
 * @note   このメンバ関数は private です．
 */
int ctindex::realloc_index_rec( size_t len_elements )
{
    const size_t len_bytes = sizeof(*(this->_index_rec)) * len_elements;

    size_t len_alloc;

    if ( this->_index_rec == NULL && len_bytes == 0 ) return 0;

    if ( 1 ) {
	if ( len_bytes <= 32 ) {		/* 要求が小さい場合 */
	    len_alloc = 32;
	}
	else {					/* 2^n でとるように手配 */
	    const double base = 2.0;
	    size_t nn = (size_t)ceil( log((double)len_bytes) / log(base) );
	    size_t len = (size_t)pow(base, (double)nn);
	    /* 念のためチェック */
	    if ( len < len_bytes ) {
		len = (size_t)pow(base, (double)(nn + 1));
		if ( len < len_bytes ) {
		    err_throw(__FUNCTION__,"FATAL","internal error");
		}
	    }
	    /* */
	    len_alloc = len;
	}
    }
    else {
	len_alloc = len_bytes;
    }

    if ( this->index_alloc_blen_rec != len_alloc ) {

	void *tmp_ptr;

	tmp_ptr = realloc(this->_index_rec, len_alloc);
	if ( tmp_ptr == NULL && 0 < len_alloc ) return -1;
	else {
	    this->_index_rec = (size_t *)tmp_ptr;
	    this->index_alloc_blen_rec = len_alloc;
	    return 0;
	}

    }
    else {

	return 0;

    }

}

/**
 * @brief  メモリ開放用メンバ関数
 *
 * @note   このメンバ関数は private です．
 */
void ctindex::free_index_rec()
{
    if ( this->_index_rec != NULL ) {
	free(this->_index_rec);
	this->_index_rec = NULL;
	this->index_alloc_blen_rec = 0;
    }
    return;
}

/**
 * @brief  メモリ確保用メンバ関数
 *
 * @note   このメンバ関数は private です．
 */
int ctindex::realloc_ctable_rec( size_t len_elements )
{
    const size_t len_bytes = sizeof(*(this->_ctable_rec)) * len_elements;

    size_t len_alloc;

    if ( this->_ctable_rec == NULL && len_bytes == 0 ) return 0;

    if ( 1 ) {
	if ( len_bytes <= 32 ) {		/* 要求が小さい場合 */
	    len_alloc = 32;
	}
	else {					/* 2^n でとるように手配 */
	    const double base = 2.0;
	    size_t nn = (size_t)ceil( log((double)len_bytes) / log(base) );
	    size_t len = (size_t)pow(base, (double)nn);
	    /* 念のためチェック */
	    if ( len < len_bytes ) {
		len = (size_t)pow(base, (double)(nn + 1));
		if ( len < len_bytes ) {
		    err_throw(__FUNCTION__,"FATAL","internal error");
		}
	    }
	    /* */
	    len_alloc = len;
	}
    }
    else {
	len_alloc = len_bytes;
    }

    if ( this->ctable_alloc_blen_rec != len_alloc ) {

	void *tmp_ptr;

	tmp_ptr = realloc(this->_ctable_rec, len_alloc);
	if ( tmp_ptr == NULL && 0 < len_alloc ) return -1;
	else {
	    this->_ctable_rec = (ctindex **)tmp_ptr;
	    this->ctable_alloc_blen_rec = len_alloc;
	    return 0;
	}

    }
    else {

	return 0;

    }

}

/**
 * @brief  メモリ開放用メンバ関数
 *
 * @note   このメンバ関数は private です．
 */
void ctindex::free_ctable_rec()
{
    if ( this->_ctable_rec != NULL ) {
	free(this->_ctable_rec);
	this->_ctable_rec = NULL;
	this->ctable_alloc_blen_rec = 0;
    }
    return;
}

/**
 * @brief  shallow copy が可能かを返す (未実装)
 * 
 * @return  shallow copyが可能なら真<br>
 *          それ以外の時は偽
 * @note    このメンバ関数は private です．
 */
bool ctindex::request_shallow_copy( ctindex *from_obj ) const
{
    return false;
}


}	/* namespace sli */


#include "private/c_memmove.cc"
