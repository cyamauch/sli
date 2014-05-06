/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2013-04-27 19:48:24 cyamauch> */

#ifndef _SLI__CTINDEX_H
#define _SLI__CTINDEX_H 1

/**
 * @file   ctindex.h
 * @brief  キー文字列と番号との関係を管理するクラス ctindex のヘッダファイル
 */

#include <stddef.h>
#include <sys/types.h>

#include "slierr.h"

/*
 * Character Tree Index
 *
 * 文字列 → index(size_t) の索引を，Character Tree を使って作る．
 *
 */

namespace sli
{

/*
 * sli::ctindex class manages the relationship between the key string and the
 * index.  Once the relationship between a string (key) and a number (index) is
 * registered, the index can be retrieved quickly by using key string.  Its
 * behavior is similar to that of hash, but no collision occurs because this 
 * class manages the relationship between the key string and the index with a 
 * dictionary.
 */

/**
 * @class  sli::ctindex
 * @brief  キー文字列とインデックス(符号なし整数)との関係を管理するクラス
 *
 *   ctindex クラスは，キー文字列とインデックス(符号なし整数)との関係を，
 *   「木」構造で記録し，高速な検索(キー文字列から配列番号への変換)が可能です．
 *   同一のキー文字列に対し，複数のインデックスを登録する事もできます．<br>
 *   検索にかかる時間は，登録されたキー文字列の数ではなく，各々のキー文字列
 *   の長さに比例します(キー文字列の長さが小さいほど速い)．ただし，「木」構造
 *   ですので，更新にはやや時間がかかります．
 *   
 * @author  Chisato YAMAUCHI
 * @date  2013-04-01 00:00:00
 */

  class ctindex
  {

  public:
    /* constructor and destructor */
    ctindex();
    ctindex(const ctindex &obj);
    virtual ~ctindex();

    /*
     * operator
     */
    virtual ctindex &operator=(const ctindex &obj);

    /*
     * member functions
     */
    /* complete initialization of objects */
    virtual ctindex &init();
    virtual ctindex &init(const ctindex &obj);

    /* append an index */
    virtual int append( const char *key, size_t index );

    /* update an index associate by a key */
    virtual int update( const char *key, 
			size_t current_index, size_t new_index );

    /* erase an index */
    virtual int erase( const char *key, size_t index );

    /* returns an index */
    virtual ssize_t index( const char *key, int index_of_index = 0 ) const;

    /* swap self and another */
    virtual ctindex &swap( ctindex &sobj );

    /* shallow copy を許可する場合に使う (未実装) */
    virtual void set_scopy_flag();

  private:
    ctindex *configure_ctable( int ch );
    int realloc_index_rec( size_t len_elements );
    void free_index_rec();
    int realloc_ctable_rec( size_t len_elements );
    void free_ctable_rec();
    bool request_shallow_copy( ctindex *from_obj ) const;

  private:
    size_t *_index_rec;		/* 目的の index */
    size_t index_alloc_blen_rec;
    int index_rec_size;		/* index_rec の数 */
    int ctable_begin_ch;	/* ctable の開始キャラクタ('A'とか) */
    int ctable_size;		/* ctable のサイズ('A' から 'E' なら 5) */
    ctindex **_ctable_rec;
    size_t ctable_alloc_blen_rec;

    /* shallow copy のためのフラグ．= 等ではコピーされない */
    /* コピーの src 側のフラグ */
    bool shallow_copy_ok;		/* set_scopy_flag() でセットされる */
    ctindex *shallow_copy_dest_obj;	/* コピー先: 有効なら non-NULL */

    /* コピーの dest 側のフラグ(コピー元から書き込まれる) */
    ctindex *shallow_copy_src_obj;	/* コピー元: 生きていれば non-NULL */

    /* __shallow_init(), __deep_init() を安全に行なうためのフラグ */
    bool __copying;

  };
}

#endif  /* _SLI__CTINDEX_H */
