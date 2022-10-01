/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2022-10-01 00:00:00 cyamauch> */

/**
 * @file   fits_table_col.cc
 * @brief  ASCII or Binary Tableの1カラムを表現するクラスfits_table_colのコード
 */

#define CLASS_NAME "fits_table_col"

#include "config.h"

#include "fits_table_col.h"
#include "fits_table.h"

#include <math.h>
#include <stdlib.h>
#include <sli/numeric_minmax.h>
#include <sli/heap_mem.h>

#include "private/err_report.h"

#include "private/c_memset.h"


namespace sli
{

static const char *const Bit_str =
    "00000000\0" "00000001\0" "00000010\0" "00000011\0"
    "00000100\0" "00000101\0" "00000110\0" "00000111\0"
    "00001000\0" "00001001\0" "00001010\0" "00001011\0"
    "00001100\0" "00001101\0" "00001110\0" "00001111\0"
    "00010000\0" "00010001\0" "00010010\0" "00010011\0"
    "00010100\0" "00010101\0" "00010110\0" "00010111\0"
    "00011000\0" "00011001\0" "00011010\0" "00011011\0"
    "00011100\0" "00011101\0" "00011110\0" "00011111\0"
    "00100000\0" "00100001\0" "00100010\0" "00100011\0"
    "00100100\0" "00100101\0" "00100110\0" "00100111\0"
    "00101000\0" "00101001\0" "00101010\0" "00101011\0"
    "00101100\0" "00101101\0" "00101110\0" "00101111\0"
    "00110000\0" "00110001\0" "00110010\0" "00110011\0"
    "00110100\0" "00110101\0" "00110110\0" "00110111\0"
    "00111000\0" "00111001\0" "00111010\0" "00111011\0"
    "00111100\0" "00111101\0" "00111110\0" "00111111\0"
    "01000000\0" "01000001\0" "01000010\0" "01000011\0"
    "01000100\0" "01000101\0" "01000110\0" "01000111\0"
    "01001000\0" "01001001\0" "01001010\0" "01001011\0"
    "01001100\0" "01001101\0" "01001110\0" "01001111\0"
    "01010000\0" "01010001\0" "01010010\0" "01010011\0"
    "01010100\0" "01010101\0" "01010110\0" "01010111\0"
    "01011000\0" "01011001\0" "01011010\0" "01011011\0"
    "01011100\0" "01011101\0" "01011110\0" "01011111\0"
    "01100000\0" "01100001\0" "01100010\0" "01100011\0"
    "01100100\0" "01100101\0" "01100110\0" "01100111\0"
    "01101000\0" "01101001\0" "01101010\0" "01101011\0"
    "01101100\0" "01101101\0" "01101110\0" "01101111\0"
    "01110000\0" "01110001\0" "01110010\0" "01110011\0"
    "01110100\0" "01110101\0" "01110110\0" "01110111\0"
    "01111000\0" "01111001\0" "01111010\0" "01111011\0"
    "01111100\0" "01111101\0" "01111110\0" "01111111\0"
    "10000000\0" "10000001\0" "10000010\0" "10000011\0"
    "10000100\0" "10000101\0" "10000110\0" "10000111\0"
    "10001000\0" "10001001\0" "10001010\0" "10001011\0"
    "10001100\0" "10001101\0" "10001110\0" "10001111\0"
    "10010000\0" "10010001\0" "10010010\0" "10010011\0"
    "10010100\0" "10010101\0" "10010110\0" "10010111\0"
    "10011000\0" "10011001\0" "10011010\0" "10011011\0"
    "10011100\0" "10011101\0" "10011110\0" "10011111\0"
    "10100000\0" "10100001\0" "10100010\0" "10100011\0"
    "10100100\0" "10100101\0" "10100110\0" "10100111\0"
    "10101000\0" "10101001\0" "10101010\0" "10101011\0"
    "10101100\0" "10101101\0" "10101110\0" "10101111\0"
    "10110000\0" "10110001\0" "10110010\0" "10110011\0"
    "10110100\0" "10110101\0" "10110110\0" "10110111\0"
    "10111000\0" "10111001\0" "10111010\0" "10111011\0"
    "10111100\0" "10111101\0" "10111110\0" "10111111\0"
    "11000000\0" "11000001\0" "11000010\0" "11000011\0"
    "11000100\0" "11000101\0" "11000110\0" "11000111\0"
    "11001000\0" "11001001\0" "11001010\0" "11001011\0"
    "11001100\0" "11001101\0" "11001110\0" "11001111\0"
    "11010000\0" "11010001\0" "11010010\0" "11010011\0"
    "11010100\0" "11010101\0" "11010110\0" "11010111\0"
    "11011000\0" "11011001\0" "11011010\0" "11011011\0"
    "11011100\0" "11011101\0" "11011110\0" "11011111\0"
    "11100000\0" "11100001\0" "11100010\0" "11100011\0"
    "11100100\0" "11100101\0" "11100110\0" "11100111\0"
    "11101000\0" "11101001\0" "11101010\0" "11101011\0"
    "11101100\0" "11101101\0" "11101110\0" "11101111\0"
    "11110000\0" "11110001\0" "11110010\0" "11110011\0"
    "11110100\0" "11110101\0" "11110110\0" "11110111\0"
    "11111000\0" "11111001\0" "11111010\0" "11111011\0"
    "11111100\0" "11111101\0" "11111110\0" "11111111";

/* this->tany array の番号 */
static const int TTYPE_IDX         = 0;
static const int TTYPE_COMMENT_IDX = 1;
static const int TALAS_IDX         = 2;
static const int TALAS_COMMENT_IDX = 3;
static const int TELEM_IDX         = 4;
static const int TELEM_COMMENT_IDX = 5;
static const int TUNIT_IDX         = 6;
static const int TUNIT_COMMENT_IDX = 7;
static const int TDISP_IDX         = 8;
static const int TDISP_COMMENT_IDX = 9;
static const int TFORM_IDX         = 10;
static const int TFORM_COMMENT_IDX = 11;
static const int TDIM_IDX          = 12;
static const int TDIM_COMMENT_IDX  = 13;
static const int TNULL_IDX         = 14;
static const int TNULL_COMMENT_IDX = 15;
static const int TZERO_IDX         = 16;
static const int TZERO_COMMENT_IDX = 17;
static const int TSCAL_IDX         = 18;
static const int TSCAL_COMMENT_IDX = 19;

static const int TANY_ARR_SIZE     = 20;

/* */
inline static size_t abs_sz2z( ssize_t val )
{
    ssize_t ret;
    if ( val < 0 ) ret = -val;
    else ret = val;
    return (size_t)ret;
}

/* */
inline static int32_t round_f2i32( float v )
{
    if ( v < 0 ) return (int32_t)(v-0.5);
    else return (int32_t)(v+0.5);
}
inline static int64_t round_f2i64( float v )
{
    if ( v < 0 ) return (int64_t)(v-0.5);
    else return (int64_t)(v+0.5);
}
inline static int32_t round_d2i32( double v )
{
    if ( v < 0 ) return (int32_t)(v-0.5);
    else return (int32_t)(v+0.5);
}
inline static int64_t round_d2i64( double v )
{
    if ( v < 0 ) return (int64_t)(v-0.5);
    else return (int64_t)(v+0.5);
}

inline static long round_f2l( float v )
{
    if ( v < 0 ) return (long)(v-0.5);
    else return (long)(v+0.5);
}
inline static long long round_f2ll( float v )
{
    if ( v < 0 ) return (long long)(v-0.5);
    else return (long long)(v+0.5);
}
inline static long round_d2l( double v )
{
    if ( v < 0 ) return (long)(v-0.5);
    else return (long)(v+0.5);
}
inline static long long round_d2ll( double v )
{
    if ( v < 0 ) return (long long)(v-0.5);
    else return (long long)(v+0.5);
}

/* */

inline static double get_min_for_type( int type )
{
    if ( type == FITS::BYTE_T ) return MIN_DOUBLE_ROUND_UCHAR;
    else if ( type == FITS::SHORT_T ) return MIN_DOUBLE_ROUND_INT16;
    else if ( type == FITS::LONG_T ) return MIN_DOUBLE_ROUND_INT32;
    else if ( type == FITS::LONGLONG_T ) return MIN_DOUBLE_ROUND_INT64;
    else if ( type == FITS::LOGICAL_T ) return MIN_DOUBLE_ROUND_UCHAR;
    else return MIN_DOUBLE_ROUND_LLONG;
}

inline static double get_max_for_type( int type )
{
    if ( type == FITS::BYTE_T ) return MAX_DOUBLE_ROUND_UCHAR;
    else if ( type == FITS::SHORT_T ) return MAX_DOUBLE_ROUND_INT16;
    else if ( type == FITS::LONG_T ) return MAX_DOUBLE_ROUND_INT32;
    else if ( type == FITS::LONGLONG_T ) return MAX_DOUBLE_ROUND_INT64;
    else if ( type == FITS::LOGICAL_T ) return MAX_DOUBLE_ROUND_UCHAR;
    else return MAX_DOUBLE_ROUND_LLONG;
}

/**
 * @brief  未定義値を返す (内部用)
 *
 * @note    private な関数です．
 */
inline static long long get_indef_for_type( int type )
{
    if ( type == FITS::BYTE_T ) return INDEF_UCHAR;
    else if ( type == FITS::SHORT_T ) return INDEF_INT16;
    else if ( type == FITS::LONG_T ) return INDEF_INT32;
    else if ( type == FITS::LONGLONG_T ) return INDEF_INT64;
    else if ( type == FITS::BIT_T ) return 0;
    else if ( type == FITS::LOGICAL_T ) return '\0';
    else return INDEF_LLONG;
}

/**
 * @brief  書き出される時の tnull 値を返す (内部用)
 *
 * @note    private な関数です．
 */
inline static long long get_tnull_for_type( int type, long long tnull )
{
    if ( type == FITS::BYTE_T ) {
	if ( MIN_UCHAR <= tnull && tnull <= MAX_UCHAR ) return tnull;
	else return INDEF_UCHAR;
    }
    else if ( type == FITS::SHORT_T ) {
	if ( MIN_INT16 <= tnull && tnull <= MAX_INT16 ) return tnull;
	else return INDEF_INT16;
    }
    else if ( type == FITS::LONG_T ) {
	if ( MIN_INT32 <= tnull && tnull <= MAX_INT32 ) return tnull;
	else return INDEF_INT32;
    }
    else if ( type == FITS::LONGLONG_T ) {
	if ( MIN_INT64 <= tnull && tnull <= MAX_INT64 ) return tnull;
	else return INDEF_INT64;
    }
    else if ( type == FITS::BIT_T ) {
	return 0;
    }
    else if ( type == FITS::LOGICAL_T ) {
	if ( MIN_UCHAR <= tnull && tnull <= MAX_UCHAR ) return tnull;
	else return '\0';
    }
    else return tnull;
}

/* constructor */
/**
 * @brief  コンストラクタ
 */
fits_table_col::fits_table_col()
{
    c_memset(&(this->def_all_rec),0,sizeof(this->def_all_rec));
    c_memset(&(this->def_rec),0,sizeof(this->def_rec));

    this->type_rec = FITS::ANY_T;
    this->bytes_rec = 0;
    this->elem_size_rec = 0;
    this->dcol_size_rec = 0;
    this->full_bytes_rec = 0;
    this->heap_type_rec = FITS::ANY_T;
    this->vl_max_length_rec = -1;
    this->tzero_rec = 0;
    this->tscal_rec = 1;
    this->tnull_is_set_rec = false;
    this->set_tnull(NULL);

    this->protected_rec = false;
    this->manager = NULL;

    this->row_size_rec = 0;

    this->data_rec.init(1,false);
    this->data_rec.set_auto_resize(false);
    this->data_rec.register_extptr(&this->data_ptr_rec);	/* [void **] */

    this->heap_rec.init(1,false);
    this->heap_rec.set_auto_resize(false);
    this->heap_rec.register_extptr(&this->heap_ptr_rec);	/* [void **] */

    this->bit_size_telem.init(sizeof(int), true);
    this->str_buf = NULL;
    this->tmp_str_buf = NULL;
    this->fmt_str = NULL;
    this->fmt_nullstr = NULL;
    this->null_svalue_rec = "NULL";

    this->shallow_copy_ok = false;
    this->shallow_copy_dest_obj = NULL;
    this->shallow_copy_src_obj = NULL;
    this->__copying = false;

    return;
}

/* copy constructor */
/**
 * @brief  コピーコンストラクタ
 */
fits_table_col::fits_table_col(const fits_table_col &obj)
{
    c_memset(&(this->def_all_rec),0,sizeof(this->def_all_rec));
    c_memset(&(this->def_rec),0,sizeof(this->def_rec));

    this->type_rec = FITS::ANY_T;
    this->bytes_rec = 0;
    this->elem_size_rec = 0;
    this->dcol_size_rec = 0;
    this->full_bytes_rec = 0;
    this->heap_type_rec = FITS::ANY_T;
    this->vl_max_length_rec = -1;
    this->tzero_rec = 0;
    this->tscal_rec = 1;
    this->tnull_is_set_rec = false;
    this->set_tnull(NULL);

    this->protected_rec = false;
    this->manager = NULL;

    this->row_size_rec = 0;

    this->data_rec.init(1,false);
    this->data_rec.set_auto_resize(false);
    this->data_rec.register_extptr(&this->data_ptr_rec);	/* [void **] */

    this->heap_rec.init(1,false);
    this->heap_rec.set_auto_resize(false);
    this->heap_rec.register_extptr(&this->heap_ptr_rec);	/* [void **] */

    this->bit_size_telem.init(sizeof(int), true);
    this->str_buf = NULL;
    this->tmp_str_buf = NULL;
    this->fmt_str = NULL;
    this->fmt_nullstr = NULL;
    this->null_svalue_rec = "NULL";

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
 */
fits_table_col::~fits_table_col()
{
    if ( this->str_buf != NULL ) {
	delete this->str_buf;
    }
    if ( this->tmp_str_buf != NULL ) {
	delete this->tmp_str_buf;
    }
    return;
}

/*
 * public
 */

/* initialize */
/**
 * @brief  オブジェクトのコピー
 *
 * @attention  可変長配列の場合，配列記述子に対する操作となります．
 */
fits_table_col &fits_table_col::operator=(const fits_table_col &obj)
{
    this->init(obj);
    return *this;
}

/**
 * @brief  オブジェクトの初期化
 *
 * @attention  可変長配列の場合，配列記述子に対する操作となります．
 */
fits_table_col &fits_table_col::init()
{
    if ( this->manager != NULL ) {
	//const char *none[] = {NULL};
	//fits::table_def def = { NULL,"",none,none,
	//			"","","","1A", "","","","" };
	fits::table_def def = { NULL,"","","",
				"","","","1A", "","","","" };
	/* manager 経由で _define を呼ぶ */
	this->define(def);
	/* きれいさっぱりにする */
	this->clean(0, this->length());
	/* svalue() の NULL 値をリセット */
	this->assign_null_svalue(this->manager->default_null_svalue());
	return *this;
    }

    c_memset(&(this->def_all_rec),0,sizeof(this->def_all_rec));
    c_memset(&(this->def_rec),0,sizeof(this->def_rec));

    this->tany = NULL;
    this->talas = NULL;
    this->telem = NULL;
    this->telem_def = NULL;

    this->elem_index_rec.init();

    this->type_rec = FITS::ANY_T;
    this->bytes_rec = 0;
    this->elem_size_rec = 0;
    this->dcol_size_rec = 0;
    this->full_bytes_rec = 0;
    this->heap_type_rec = FITS::ANY_T;
    this->vl_max_length_rec = -1;
    this->tzero_rec = 0;
    this->tscal_rec = 1;
    this->tnull_is_set_rec = false;
    this->set_tnull(NULL);

    if ( this->manager == NULL ) {
	this->protected_rec = false;
    }

    this->row_size_rec = 0;

    this->bit_size_telem.init(sizeof(int), true);

    if ( this->str_buf != NULL ) {
	delete this->str_buf;
	this->str_buf = NULL;
    }
    if ( this->tmp_str_buf != NULL ) {
	delete this->tmp_str_buf;
	this->tmp_str_buf = NULL;
    }
    this->fmt_str = NULL;
    this->fmt_nullstr = NULL;
    this->null_svalue_rec = "NULL";

    this->data_rec.init(1,false);
    this->data_rec.set_auto_resize(false);

    this->heap_rec.init(1,false);
    this->heap_rec.set_auto_resize(false);

    return *this;
}

/**
 * @brief  オブジェクトのコピー
 *
 * @attention  可変長配列の場合，配列記述子に対する操作となります．
 */
fits_table_col &fits_table_col::init(const fits_table_col &obj)
{
    long obj_full_bytes;
    long org_nrows = this->length();

    if ( &obj == this ) return *this;

    if ( this->manager == NULL ) {
	this->fits_table_col::init();
    }
    else {
	/* カラム名，TALAS などの処理を manager にさせる */
	/* manager 系由で _define() を呼ぶ */
	this->define(obj.definition());
    }

    obj_full_bytes = obj.full_bytes_rec;

    try {
	this->data_rec.init(obj.data_rec);
    }
    catch (...) {
	err_throw(__FUNCTION__,"FATAL","this->data_rec.init() failed");
    }

    try {
	this->heap_rec.init(obj.heap_rec);
    }
    catch (...) {
	this->data_rec.init(1,false);
	err_throw(__FUNCTION__,"FATAL","this->heap_rec.init() failed");
    }

    if ( obj.str_buf != NULL ) {
	try {
	    this->str_buf = new tstring(obj.str_buf->max_length());
	}
	catch (...) {
	    this->data_rec.init(1,false);
	    this->heap_rec.init(1,false);
	    err_throw(__FUNCTION__,"FATAL","new failed");
	}
	try {
	    this->tmp_str_buf = new tstring(obj.tmp_str_buf->max_length());
	}
	catch (...) {
	    if ( this->str_buf != NULL ) delete this->str_buf;
	    this->str_buf = NULL;
	    this->data_rec.init(1,false);
	    this->heap_rec.init(1,false);
	    err_throw(__FUNCTION__,"FATAL","new failed");
	}
	this->str_buf->assign(obj.str_buf->cstr());
    }

    if ( 0 < obj.bit_size_telem.length() ) {
	try {
	    this->bit_size_telem.init(obj.bit_size_telem);
	}
	catch (...) {
	    if ( this->tmp_str_buf != NULL ) delete this->tmp_str_buf;
	    this->tmp_str_buf = NULL;
	    if ( this->str_buf != NULL ) delete this->str_buf;
	    this->str_buf = NULL;
	    this->data_rec.init(1,false);
	    this->heap_rec.init(1,false);
	    err_throw(__FUNCTION__,"FATAL","bit_size_telem.init() failed");
	}
    }

    try {
	this->fmt_str = obj.fmt_str;
	this->fmt_nullstr = obj.fmt_nullstr;
	this->null_svalue_rec = obj.null_svalue_rec;
	this->tany = obj.tany;
	this->talas = obj.talas;
	this->telem = obj.telem;
	this->telem_def = obj.telem_def;
	this->elem_index_rec = obj.elem_index_rec;
    }
    catch (...) {
	this->elem_index_rec.init();
	this->telem = NULL;
	this->telem_def = NULL;
	this->talas = NULL;
	this->tany = NULL;
	this->bit_size_telem.init(sizeof(int), true);
	if ( this->str_buf != NULL ) delete this->str_buf;
	this->str_buf = NULL;
	if ( this->tmp_str_buf != NULL ) delete this->tmp_str_buf;
	this->tmp_str_buf = NULL;
	this->data_rec.init(1,false);
	this->heap_rec.init(1,false);
	err_throw(__FUNCTION__,"FATAL","'=' failed");
    }

    this->update_def_rec_ptr();

    this->type_rec = obj.type_rec;
    this->bytes_rec = obj.bytes_rec;
    this->elem_size_rec = obj.elem_size_rec;
    this->dcol_size_rec = obj.dcol_size_rec;
    this->full_bytes_rec = obj.full_bytes_rec;
    this->heap_type_rec = obj.heap_type_rec;
    this->vl_max_length_rec = obj.vl_max_length_rec;
    this->tzero_rec = obj.tzero_rec;
    this->tscal_rec = obj.tscal_rec;
    this->tnull_is_set_rec = obj.tnull_is_set_rec;
    this->tnull_r_rec = obj.tnull_r_rec;
    this->tnull_w_rec = obj.tnull_w_rec;
    this->tnull_longlong_rec = obj.tnull_longlong_rec;
    this->tnull_long_rec = obj.tnull_long_rec;
    this->tnull_short_rec = obj.tnull_short_rec;
    this->tnull_byte_rec = obj.tnull_byte_rec;

    if ( this->manager == NULL ) {
	this->protected_rec = obj.protected_rec;
    }

    this->row_size_rec = obj.row_size_rec;

    if ( this->manager != NULL ) {	/* fits_table の管理下の場合 */
	/* 元のロウの大きさに戻す */
	this->_resize(org_nrows);
	/* 間接的に _resize() を呼ぶ(これだとテーブルの全部がリサイズ) */
	//this->manager->resize_rows(obj.row_size_rec);
    }

    return *this;
}

/* swap internal data of two objects */
/**
 * @brief  2つのオブジェクト間での内容のスワップ
 *
 *  指定されたオブジェクトobj の内容と自身の内容を入れ替えます．
 *  データ配列・ヘッダの内容，属性等すべての状態が入れ替わります．
 *
 * @param   obj fits_table_col クラスのオブジェクト
 * @return  自身の参照    
 * @attention  可変長配列の場合，配列記述子に対する操作となります．
 */
fits_table_col &fits_table_col::swap(fits_table_col &obj)
{
    fits::table_def_all def;
    tstring ttype1;
    //tarray_tstring talas1;
    tstring talas1;
    tstring ttype2;
    //tarray_tstring talas2;
    tstring talas2;

    if ( &obj == this ) return *this;

    c_memset(&def,0,sizeof(def));

    if ( this->manager != NULL ) {
	ttype2 = obj.tany.at(TTYPE_IDX);
	//talas2 = obj.talas;
	talas2 = obj.tany.cstr(TALAS_IDX);
    }
    if ( obj.manager != NULL ) {
	ttype1 = this->tany.at(TTYPE_IDX);
	//talas1 = this->talas;
	talas1 = this->tany.cstr(TALAS_IDX);
    }

    /* まず manager 側を更新する */
    if ( this->manager != NULL ) {
	def.ttype = ttype2.cstr();
	//def.talas = talas2.cstrarray();
	def.talas = talas2.cstr();
	/* manager 経由で _define を呼ぶ */
	this->define(def);
    }
    if ( obj.manager != NULL ) {
	def.ttype = ttype1.cstr();
	//def.talas = talas1.cstrarray();
	def.talas = talas1.cstr();
	/* manager 経由で _define を呼ぶ */
	obj.define(def);
    }

    this->tany.swap(obj.tany);
    this->talas.swap(obj.talas);
    this->telem.swap(obj.telem);
    this->telem_def.swap(obj.telem_def);
    this->elem_index_rec.swap(obj.elem_index_rec);

    tstring *tmp__str_buf;
    tstring *tmp__tmp_str_buf;

    this->bit_size_telem.swap(obj.bit_size_telem);

    tmp__str_buf = this->str_buf;
    this->str_buf = obj.str_buf;
    obj.str_buf = tmp__str_buf;

    tmp__tmp_str_buf = this->tmp_str_buf;
    this->tmp_str_buf = obj.tmp_str_buf;
    obj.tmp_str_buf = tmp__tmp_str_buf;

    this->fmt_str.swap(obj.fmt_str);
    this->fmt_nullstr.swap(obj.fmt_nullstr);
    this->null_svalue_rec.swap(obj.null_svalue_rec);

    int tmp__type_rec;
    long tmp__bytes_rec;
    long tmp__elem_size_rec;
    long tmp__dcol_size_rec;
    long tmp__full_bytes_rec;
    int tmp__heap_type_rec;
    long tmp__vl_max_length_rec;
    double tmp__tzero_rec;
    double tmp__tscal_rec;
    bool tmp__tnull_is_set_rec;
    long long tmp__tnull_r_rec;
    long long tmp__tnull_w_rec;
    long long tmp__tnull_longlong_rec;
    long tmp__tnull_long_rec;
    short tmp__tnull_short_rec;
    unsigned char tmp__tnull_byte_rec;
    long tmp__row_size_rec;

    tmp__type_rec = this->type_rec;
    this->type_rec = obj.type_rec;
    obj.type_rec = tmp__type_rec;

    tmp__bytes_rec = this->bytes_rec;
    this->bytes_rec = obj.bytes_rec;
    obj.bytes_rec = tmp__bytes_rec;

    tmp__elem_size_rec = this->elem_size_rec;
    this->elem_size_rec = obj.elem_size_rec;
    obj.elem_size_rec = tmp__elem_size_rec;

    tmp__dcol_size_rec = this->dcol_size_rec;
    this->dcol_size_rec = obj.dcol_size_rec;
    obj.dcol_size_rec = tmp__dcol_size_rec;

    tmp__full_bytes_rec = this->full_bytes_rec;
    this->full_bytes_rec = obj.full_bytes_rec;
    obj.full_bytes_rec = tmp__full_bytes_rec;

    tmp__heap_type_rec = this->heap_type_rec;
    this->heap_type_rec = obj.heap_type_rec;
    obj.heap_type_rec = tmp__heap_type_rec;

    tmp__vl_max_length_rec = this->vl_max_length_rec;
    this->vl_max_length_rec = obj.vl_max_length_rec;
    obj.vl_max_length_rec = tmp__vl_max_length_rec;

    tmp__tzero_rec = this->tzero_rec;
    this->tzero_rec = obj.tzero_rec;
    obj.tzero_rec = tmp__tzero_rec;

    tmp__tscal_rec = this->tscal_rec;
    this->tscal_rec = obj.tscal_rec;
    obj.tscal_rec = tmp__tscal_rec;

    tmp__tnull_is_set_rec = this->tnull_is_set_rec;
    this->tnull_is_set_rec = obj.tnull_is_set_rec;
    obj.tnull_is_set_rec = tmp__tnull_is_set_rec;

    tmp__tnull_r_rec = this->tnull_r_rec;
    this->tnull_r_rec = obj.tnull_r_rec;
    obj.tnull_r_rec = tmp__tnull_r_rec;

    tmp__tnull_w_rec = this->tnull_w_rec;
    this->tnull_w_rec = obj.tnull_w_rec;
    obj.tnull_w_rec = tmp__tnull_w_rec;

    tmp__tnull_longlong_rec = this->tnull_longlong_rec;
    this->tnull_longlong_rec = obj.tnull_longlong_rec;
    obj.tnull_longlong_rec = tmp__tnull_longlong_rec;

    tmp__tnull_long_rec = this->tnull_long_rec;
    this->tnull_long_rec = obj.tnull_long_rec;
    obj.tnull_long_rec = tmp__tnull_long_rec;

    tmp__tnull_short_rec = this->tnull_short_rec;
    this->tnull_short_rec = obj.tnull_short_rec;
    obj.tnull_short_rec = tmp__tnull_short_rec;

    tmp__tnull_byte_rec = this->tnull_byte_rec;
    this->tnull_byte_rec = obj.tnull_byte_rec;
    obj.tnull_byte_rec = tmp__tnull_byte_rec;

    tmp__row_size_rec = this->row_size_rec;
    this->row_size_rec = obj.row_size_rec;
    obj.row_size_rec = tmp__row_size_rec;

    this->data_rec.swap(obj.data_rec);

    this->heap_rec.swap(obj.heap_rec);

    this->update_def_rec_ptr();
    obj.update_def_rec_ptr();

    bool tmp__protected_rec;
    tmp__protected_rec = this->protected_rec;
    if ( this->manager == NULL ) {
	this->protected_rec = obj.protected_rec;
    }
    if ( obj.manager == NULL ) {
	obj.protected_rec = tmp__protected_rec;
    }

    if ( this->manager != NULL ) {
	def.ttype = ttype2.cstr();
	//def.talas = talas2.cstrarray();
	def.talas = talas2.cstr();
	this->_define(def);
    }
    if ( obj.manager != NULL ) {
	def.ttype = ttype1.cstr();
	//def.talas = talas1.cstrarray();
	def.talas = talas1.cstr();
	obj._define(def);
    }

    return *this;
}

/* 
 * Functions to obtain column information
 */

/**
 * @brief  保護属性の有無を返す
 *
 * @note   ユーザの拡張クラスで使用を想定．
 */
bool fits_table_col::is_protected() const
{
    return this->protected_rec;
}

/* to obtain all definition of a column */
/**
 * @brief  カラムの定義を fits::table_def 構造体で取得
 */
const fits::table_def &fits_table_col::definition() const
{
    return this->def_rec;
}

/**
 * @brief  カラムの定義を fits::table_def_all 構造体で取得
 */
const fits::table_def_all &fits_table_col::definition_all() const
{
    return this->def_all_rec;
}

/* get/set column name */
/**
 * @brief  カラム名を取得
 */
const char *fits_table_col::name() const
{
    return this->tany.cstr(TTYPE_IDX);
}

/**
 * @brief  カラム名を設定
 */
fits_table_col &fits_table_col::assign_name( const char *new_name )
{
    fits::table_def_all def = {NULL};
    def.ttype = new_name;
    this->define(def);
    return *this;
}

/**
 * @brief  固定長配列または可変長配列の要素数を取得
 *
 *  固定長配列の場合，引数の値に関係なく fits_table_col::elem_length() の返り値
 *  と同じ値が返されます．<br>
 *  elem_idx は，可変長配列の配列記述子が複数存在する場合に指定します．
 *  例えば TFORM の定義が '4PE(999)' の場合 elem_idx は 0 〜 3 の値をとる事が
 *  できます．
 *
 * @param   row_idx 行インデックス
 * @param   elem_idx 可変長配列の配列記述子の要素番号 (省略時は0)
 * @return  配列の要素数
 */
long fits_table_col::array_length( long row_idx, long elem_idx ) const
{
    long elem_len = this->elem_size_rec;
    if ( this->type_rec == FITS::LONGARRDESC_T ) {
      if ( this->data_ptr() == NULL ) return -1;
      else {
	  long ix = (elem_len * row_idx) + (elem_idx % elem_len);
	  return 
	      ((const fits::longarrdesc_t *)(this->data_ptr()))[ix].length;
      }
    }
    else if ( this->type_rec == FITS::LLONGARRDESC_T ) {
      if ( this->data_ptr() == NULL ) return -1;
      else {
	  long ix = (elem_len * row_idx) + (elem_idx % elem_len);
	  return 
	      ((const fits::llongarrdesc_t *)(this->data_ptr()))[ix].length;
      }
    }
    else return elem_len;
}

/**
 * @brief  可変長配列のヒープオフセットを取得
 *
 *  elem_idx は，可変長配列の配列記述子が複数存在する場合に指定します．
 *  例えば TFORM の定義が '4PE(999)' の場合 elem_idx は 0 〜 3 の値をとる事が
 *  できます．
 *
 * @param   row_idx 行インデックス
 * @param   elem_idx 可変長配列の配列記述子の要素番号 (省略時は0)
 * @return  ヒープオフセット (バイト単位)
 */
long fits_table_col::array_heap_offset( long row_idx, long elem_idx ) const
{
    long elem_len = this->elem_size_rec;
    if ( this->type_rec == FITS::LONGARRDESC_T ) {
      if ( this->data_ptr() == NULL ) return -1;
      else {
	  long ix = (elem_len * row_idx) + (elem_idx % elem_len);
	  return 
	      ((const fits::longarrdesc_t *)(this->data_ptr()))[ix].offset;
      }
    }
    else if ( this->type_rec == FITS::LLONGARRDESC_T ) {
      if ( this->data_ptr() == NULL ) return -1;
      else {
	  long ix = (elem_len * row_idx) + (elem_idx % elem_len);
	  return 
	      ((const fits::llongarrdesc_t *)(this->data_ptr()))[ix].offset;
      }
    }
    else return -1;
}


/*
 * NULL value to I/O of high-level APIs.  Default is "NULL".
 */

/**
 * @brief  NULL文字列の設定
 *
 *  fits_table_col::svalue()，fits_table_col::assign( const char *, ... ) で
 *  使用する NULL 文字列値を設定します．<br>
 *  NULL文字列の初期設定値は "NULL" です．
 *
 * @param   snull NULL文字列
 * @return  自身の参照
 */
fits_table_col &fits_table_col::assign_null_svalue( const char *snull )
{
    if ( snull != NULL ) {
	this->null_svalue_rec.assign(snull);
	/* */
	if ( this->str_buf != NULL ) {
	    size_t org_len = this->str_buf->max_length();
	    if ( org_len < this->null_svalue_rec.length() ) {
		size_t len_alc = this->null_svalue_rec.length();
		delete this->str_buf;
		this->str_buf = NULL;
		try {
		    this->str_buf = new tstring(len_alc);
		}
		catch (...) {
		    err_throw(__FUNCTION__,"FATAL","new failed");
		}
	    }
	}
	if ( this->tmp_str_buf != NULL ) {
	    size_t org_len = this->tmp_str_buf->max_length();
	    if ( org_len < this->null_svalue_rec.length() ) {
		size_t len_alc = this->null_svalue_rec.length();
		delete this->tmp_str_buf;
		this->tmp_str_buf = NULL;
		try {
		    this->tmp_str_buf = new tstring(len_alc);
		}
		catch (...) {
		    err_throw(__FUNCTION__,"FATAL","new failed");
		}
	    }
	}
    }
    return *this;
}

/**
 * @brief  NULL文字列の取得
 *
 *  fits_table_col::svalue()，fits_table_col::assign( const char *, ... ) で
 *  使用する NULL 文字列値を取得します．
 *
 * @return  NULL文字列
 */
const char *fits_table_col::null_svalue() const
{
    return this->null_svalue_rec.cstr();
}

/*
 * Set default value when resizing
 * Note: default value will be resetted when changing attributes of column
 */

/* high-level */
/**
 * @brief  セルのデフォルト値を double 型で設定
 *
 *  fits_table::resize_rows() 等でテーブルの行の長さを大きくした場合の新規セル
 *  の値を設定します．このメンバ関数ではヘッダの TZEROn，TSCALEn，TNULLn の値が
 *  反映されます．<br>
 *  デフォルト値の初期設定値は，論理型の場合は 'F'，文字列型の場合は ' ' から
 *  なる文字列，その他の型の場合は 0 です．<br>
 *  デフォルト値として NULL値をセットしたい場合は NAN をセットします．
 *
 * @param   value 初期値
 * @return  自身の参照
 * @attention  可変長配列はサポートされていません．
 */
fits_table_col &fits_table_col::assign_default( double value )
{
    fits_table_col dummy;
    if ( this->heap_is_used() == true ) goto quit;

    if ( this->data_ptr() == NULL ) {
	dummy.define(this->definition());
	dummy.resize(1);
	dummy.assign(value, 0);
	this->data_rec.assign_default((const void *)(dummy.data_ptr_cs()));
    }
    else {	/* Improved code by K. Matsuzaki (ISAS/JAXA). 2015.06.16 */
	heap_mem<char> saved_area;
	size_t n = this->data_rec.bytes();
	char *arr_area = (char *)this->data_ptr();

	saved_area.allocate(n);
	saved_area.copyfrom(arr_area, n);
	this->assign(value, 0);
	this->data_rec.assign_default((const void *)arr_area);
	saved_area.copyto(arr_area, n);
    }

 quit:
    return *this;
}

/**
 * @brief  セルのデフォルト値を float 型で設定
 *
 *  fits_table::resize_rows() 等でテーブルの行の長さを大きくした場合の新規セル
 *  の値を設定します．このメンバ関数ではヘッダの TZEROn，TSCALEn，TNULLn の値が
 *  反映されます．<br>
 *  デフォルト値の初期設定値は，論理型の場合は 'F'，文字列型の場合は ' ' から
 *  なる文字列，その他の型の場合は 0 です．<br>
 *  デフォルト値として NULL値をセットしたい場合は NAN をセットします．
 *
 * @param   value 初期値
 * @return  自身の参照
 * @attention  可変長配列はサポートされていません．
 */
fits_table_col &fits_table_col::assign_default( float value )
{
    fits_table_col dummy;
    if ( this->heap_is_used() == true ) goto quit;

    if ( this->data_ptr() == NULL ) {
	dummy.define(this->definition());
	dummy.resize(1);
	dummy.assign(value, 0);
	this->data_rec.assign_default((const void *)(dummy.data_ptr_cs()));
    }
    else {	/* Improved code by K. Matsuzaki (ISAS/JAXA). 2015.06.16 */
	heap_mem<char> saved_area;
	size_t n = this->data_rec.bytes();
	char *arr_area = (char *)this->data_ptr();

	saved_area.allocate(n);
	saved_area.copyfrom(arr_area, n);
	this->assign(value, 0);
	this->data_rec.assign_default((const void *)arr_area);
	saved_area.copyto(arr_area, n);
    }

 quit:
    return *this;
}

/**
 * @brief  セルのデフォルト値を long long 型で設定
 *
 *  fits_table::resize_rows() 等でテーブルの行の長さを大きくした場合の新規セル
 *  の値を設定します．このメンバ関数ではヘッダの TZEROn，TSCALEn，TNULLn の値が
 *  反映されます．<br>
 *  デフォルト値の初期設定値は，論理型の場合は 'F'，文字列型の場合は ' ' から
 *  なる文字列，その他の型の場合は 0 です．<br>
 *  デフォルト値として NULL値をセットしたい場合は 
 *  fits_table_col::assign_default(double) で NAN をセットします．
 *
 * @param   value 初期値
 * @return  自身の参照
 * @attention  可変長配列はサポートされていません．
 */
fits_table_col &fits_table_col::assign_default( long long value )
{
    fits_table_col dummy;
    if ( this->heap_is_used() == true ) goto quit;

    if ( this->data_ptr() == NULL ) {
	dummy.define(this->definition());
	dummy.resize(1);
	dummy.assign(value, 0);
	this->data_rec.assign_default((const void *)(dummy.data_ptr_cs()));
    }
    else {	/* Improved code by K. Matsuzaki (ISAS/JAXA). 2015.06.16 */
	heap_mem<char> saved_area;
	size_t n = this->data_rec.bytes();
	char *arr_area = (char *)this->data_ptr();

	saved_area.allocate(n);
	saved_area.copyfrom(arr_area, n);
	this->assign(value, 0);
	this->data_rec.assign_default((const void *)arr_area);
	saved_area.copyto(arr_area, n);
    }

 quit:
    return *this;
}

/**
 * @brief  セルのデフォルト値を long 型で設定
 *
 *  fits_table::resize_rows() 等でテーブルの行の長さを大きくした場合の新規セル
 *  の値を設定します．このメンバ関数ではヘッダの TZEROn，TSCALEn，TNULLn の値が
 *  反映されます．<br>
 *  デフォルト値の初期設定値は，論理型の場合は 'F'，文字列型の場合は ' ' から
 *  なる文字列，その他の型の場合は 0 です．<br>
 *  デフォルト値として NULL値をセットしたい場合は 
 *  fits_table_col::assign_default(double) で NAN をセットします．
 *
 * @param   value 初期値
 * @return  自身の参照
 * @attention  可変長配列はサポートされていません．
 */
fits_table_col &fits_table_col::assign_default( long value )
{
    fits_table_col dummy;
    if ( this->heap_is_used() == true ) goto quit;

    if ( this->data_ptr() == NULL ) {
	dummy.define(this->definition());
	dummy.resize(1);
	dummy.assign(value, 0);
	this->data_rec.assign_default((const void *)(dummy.data_ptr_cs()));
    }
    else {	/* Improved code by K. Matsuzaki (ISAS/JAXA). 2015.06.16 */
	heap_mem<char> saved_area;
	size_t n = this->data_rec.bytes();
	char *arr_area = (char *)this->data_ptr();

	saved_area.allocate(n);
	saved_area.copyfrom(arr_area, n);
	this->assign(value, 0);
	this->data_rec.assign_default((const void *)arr_area);
	saved_area.copyto(arr_area, n);
    }

 quit:
    return *this;
}

/**
 * @brief  セルのデフォルト値を int 型で設定
 *
 *  fits_table::resize_rows() 等でテーブルの行の長さを大きくした場合の新規セル
 *  の値を設定します．このメンバ関数ではヘッダの TZEROn，TSCALEn，TNULLn の値が
 *  反映されます．<br>
 *  デフォルト値の初期設定値は，論理型の場合は 'F'，文字列型の場合は ' ' から
 *  なる文字列，その他の型の場合は 0 です．<br>
 *  デフォルト値として NULL値をセットしたい場合は 
 *  fits_table_col::assign_default(double) で NAN をセットします．
 *
 * @param   value 初期値
 * @return  自身の参照
 * @attention  可変長配列はサポートされていません．
 */
fits_table_col &fits_table_col::assign_default( int value )
{
    fits_table_col dummy;
    if ( this->heap_is_used() == true ) goto quit;

    if ( this->data_ptr() == NULL ) {
	dummy.define(this->definition());
	dummy.resize(1);
	dummy.assign(value, 0);
	this->data_rec.assign_default((const void *)(dummy.data_ptr_cs()));
    }
    else {	/* Improved code by K. Matsuzaki (ISAS/JAXA). 2015.06.16 */
	heap_mem<char> saved_area;
	size_t n = this->data_rec.bytes();
	char *arr_area = (char *)this->data_ptr();

	saved_area.allocate(n);
	saved_area.copyfrom(arr_area, n);
	this->assign(value, 0);
	this->data_rec.assign_default((const void *)arr_area);
	saved_area.copyto(arr_area, n);
    }

 quit:
    return *this;
}

/**
 * @brief  セルのデフォルト値を char * 型で設定
 *
 *  fits_table::resize_rows() 等でテーブルの行の長さを大きくした場合の新規セル
 *  の値を設定します．このメンバ関数ではヘッダの TZEROn，TSCALEn，TNULLn の値が
 *  反映されます．<br>
 *  デフォルト値の初期設定値は，論理型の場合は 'F'，文字列型の場合は ' ' から
 *  なる文字列，その他の型の場合は 0 です．<br>
 *  デフォルト値として NULL値をセットしたい場合は 
 *  fits_table_col::assign_default(double) で NAN をセットします．
 *
 * @param   value 初期値
 * @return  自身の参照
 * @attention  可変長配列はサポートされていません．
 */
fits_table_col &fits_table_col::assign_default( const char *value )
{
    fits_table_col dummy;
    if ( this->heap_is_used() == true ) goto quit;

    if ( this->data_ptr() == NULL ) {
	dummy.define(this->definition());
	dummy.resize(1);
	dummy.assign(value, 0);
	this->data_rec.assign_default((const void *)(dummy.data_ptr_cs()));
    }
    else {	/* Improved code by K. Matsuzaki (ISAS/JAXA). 2015.06.16 */
	heap_mem<char> saved_area;
	size_t n = this->data_rec.bytes();
	char *arr_area = (char *)this->data_ptr();

	saved_area.allocate(n);
	saved_area.copyfrom(arr_area, n);
	this->assign(value, 0);
	this->data_rec.assign_default((const void *)arr_area);
	saved_area.copyto(arr_area, n);
    }

 quit:
    return *this;
}

/* low-level */
/**
 * @brief  セルのデフォルト値を設定 (低レベル)
 *
 *  fits_table::resize_rows() 等でテーブルの行の長さを大きくした場合の新規セル
 *  の値を設定します．低レベルなメンバ関数ですので，ヘッダの TZEROn，TSCALEn，
 *  TNULLn の値は考慮されません．<br>
 *  デフォルト値の初期設定値は，論理型の場合は 'F'，文字列型の場合は ' ' から
 *  なる文字列，その他の型の場合は 0 です．
 *
 * @param   value 初期値のアドレス
 * @note    value が指す内容のバイト数は，カラムの型のバイト数，すなわち
 *          「fits_table_col::bytes() が返す値」に一致している必要があります．
 * @return  自身の参照
 * @attention  可変長配列はサポートされていません．
 */
fits_table_col &fits_table_col::assign_default_value( const void *value )
{
    if ( this->heap_is_used() == true ) goto quit;

    this->data_rec.assign_default(value);

 quit:
    return *this;
}

/* */

/**
 * @deprecated  将来の実装のためのメンバ関数．使用しないでください．
 */
void *fits_table_col::heap_ptr()
{
    if ( this->manager != NULL ) return NULL;		      /* not enabled */

    return this->heap_ptr_rec;
}

#ifdef SLI__OVERLOAD_CONST_AT
/**
 * @deprecated  将来の実装のためのメンバ関数．使用しないでください．
 */
const void *fits_table_col::heap_ptr() const
{
    if ( this->manager != NULL ) return NULL;		      /* not enabled */

    return this->heap_ptr_rec;
}
#endif

/**
 * @deprecated  将来の実装のためのメンバ関数．使用しないでください．
 */
const void *fits_table_col::heap_ptr_cs() const
{
    if ( this->manager != NULL ) return NULL;		      /* not enabled */

    return this->heap_ptr_rec;
}

/*
 * Get or put raw data using C-like I/O args
 */

/**
 * @brief  データを外部バッファへコピー
 *
 * @param   dest_buf コピー先バッファのアドレス
 * @param   buf_size dest_buf のバイトサイズ
 * @return  非負の値: バッファ長が十分な場合にコピーできるバイト数 <br>
 *          負の値(エラー): 引数が不正でコピーされなかった場合
 * @attention  可変長配列はサポートされていません．
 */
ssize_t fits_table_col::get_data( void *dest_buf, size_t buf_size ) const
{
    return this->get_data(0,dest_buf,buf_size);
}

/**
 * @brief  データを外部バッファへコピー
 *
 * @param   row_index  コピー元(内部バッファ)の行インデックス
 * @param   dest_buf コピー先バッファのアドレス
 * @param   buf_size dest_buf のバイトサイズ
 * @return  非負の値: バッファ長が十分な場合にコピーできるバイト数 <br>
 *          負の値(エラー): 引数が不正でコピーされなかった場合
 * @attention  可変長配列はサポートされていません．
 */
ssize_t fits_table_col::get_data( long row_index, 
				  void *dest_buf, size_t buf_size ) const
{
    if ( this->full_bytes_rec == 0 ) return -1;		/* not defined */

    return this->data_rec.getdata(dest_buf, buf_size, 0, row_index);
}

/**
 * @brief  外部バッファのデータを取り込む
 *
 * @param   src_buf コピー元バッファのアドレス
 * @param   buf_size src_buf のバイトサイズ
 * @return  非負の値: src_bufのバッファ長が十分な場合にコピーできるバイト数<br>
 *          負の値: 引数不正でコピーされなかった場合
 * @attention  可変長配列はサポートされていません．
 */
ssize_t fits_table_col::put_data( const void *src_buf, size_t buf_size )
{
    return this->put_data(0,src_buf,buf_size);
}

/**
 * @brief  外部バッファのデータを取り込む
 *
 * @param   row_index コピー先(内部バッファ)の行インデックス
 * @param   src_buf コピー元バッファのアドレス
 * @param   buf_size src_buf のバイトサイズ
 * @return  非負の値: src_bufのバッファ長が十分な場合にコピーできるバイト数<br>
 *          負の値: 引数不正でコピーされなかった場合
 * @attention  可変長配列はサポートされていません．
 */
ssize_t fits_table_col::put_data( long row_index, 
				  const void *src_buf, size_t buf_size )
{
    if ( this->full_bytes_rec == 0 ) return -1;		/* not defined */

    return this->data_rec.putdata(src_buf, buf_size, 0, row_index);
}

/**
 * @brief  カラムデータを管理している内部オブジェクトの参照を返す (読取専用)
 */
const mdarray &fits_table_col::data_array_cs() const
{
    return this->data_rec;
}

/**
 * @deprecated  将来の実装のためのメンバ関数．使用しないでください．
 */
ssize_t fits_table_col::get_heap( void *dest_buf, size_t buf_size ) const
{
    if ( this->manager != NULL ) return -1;		      /* not enabled */

    return this->get_heap(0,dest_buf,buf_size);
}

/**
 * @deprecated  将来の実装のためのメンバ関数．使用しないでください．
 */
ssize_t fits_table_col::get_heap( long offset, 
				  void *dest_buf, size_t buf_size ) const
{
    if ( this->manager != NULL ) return -1;		      /* not enabled */

    return this->heap_rec.getdata(dest_buf, buf_size, offset);
}

/**
 * @deprecated  将来の実装のためのメンバ関数．使用しないでください．
 */
ssize_t fits_table_col::put_heap( const void *buf, size_t buf_size )
{
    if ( this->manager != NULL ) return -1;		      /* not enabled */

    return this->put_heap(0,buf,buf_size);
}

/**
 * @deprecated  将来の実装のためのメンバ関数．使用しないでください．
 */
ssize_t fits_table_col::put_heap( long offset, 
				  const void *buf, size_t buf_size )
{
    if ( this->manager != NULL ) return -1;		      /* not enabled */

    return this->heap_rec.putdata(buf, buf_size, offset);
}

/**
 * @deprecated  将来の実装のためのメンバ関数．使用しないでください．
 */
const mdarray &fits_table_col::heap_array_cs() const
{
    return this->heap_rec;
}

/**
 * @deprecated  将来の実装のためのメンバ関数．使用しないでください．
 */
fits_table_col &fits_table_col::resize_heap( size_t sz )
{
    this->heap_rec.resize(sz);
    return *this;
}

/**
 * @deprecated  将来の実装のためのメンバ関数．使用しないでください．
 */
size_t fits_table_col::heap_length()
{
    return this->heap_rec.length();
}

/**
 * @brief  TFORMn，TDIMn をパースし，カラムの各種情報を返す (内部用)
 *
 * @note    private な関数です．
 */
static int parse_tform_and_tdim( const char *tform_in, const char *tdim_in,
				 int *ret_type, long *ret_sz_bytes, 
				 long *ret_elem_size, 
				 long *ret_dcol_size,
				 long *ret_full_bytes,
				 int *ret_heap_type, long *ret_heap_sz_bytes,
				 long *ret_vl_max_length )
{
    int status = 0;
    ssize_t p0, p1, p2;
    size_t l0, l1, l2;
    long new_elem_size;
    tstring tform;
    tstring tdim;
    int *tgt_type;
    long *tgt_sz_bytes;
    int tgt_ch;

    if ( tform_in == NULL ) {
	status = -1;					/* for ERROR */
	goto quit;
    }

    if ( tdim_in != NULL ) tdim.assign(tdim_in);
    tform.assign(tform_in);

    /* find starting position */
    p0 = tform.strpbrk("0123456789LXIJKAEDBPQCM");
    if ( p0 != 0 ) status = 1;				/* for WARNING */
    else if ( p0 < 0 ) {
	/* unsupported type */
	status = -1;					/* for ERROR */
	goto quit;
    }

    /* get element length */
    l0 = tform.strspn(p0, "0123456789");
    if ( l0 == 0 ) {
	new_elem_size = 1;
    }
    else {
	size_t endpos;
	new_elem_size = tform.strtol(p0,10,&endpos);
	if ( new_elem_size < 0 ) {
	    new_elem_size = 1;
	    status = 1;					/* for WARNING */
	}
	else if ( new_elem_size == 0 ) {
	    //status = 1;				/* for WARNING */
	}
    }

    /* get type */
    p1 = p0 + l0;
    l1 = tform.strspn(p1, "LXIJKAEDBPQCM");
    if ( l1 == 0 ) {
	/* unsupported type */
	status = -1;					/* for ERROR */
	goto quit;
    }
    else l1 = 1;

    /* using heap */
    if ( tform.cchr(p1) == 'P' || tform.cchr(p1) == 'Q' ) {
	/* とりあえず，文字列とビット型はサポートせず，という事で */
	p2 = p1 + l1;
	l2 = tform.strspn(p2, "LIJKEDBCM");

	if ( l2 == 0 ) {
	    /* unsupported type */
	    status = -1;				/* for ERROR */
	    goto quit;
	}

	if ( tform.cchr(p1) == 'P' ) {
	    *ret_type  = FITS::LONGARRDESC_T;
	    *ret_sz_bytes = sizeof(fits::longarrdesc_t);
	}
	else if ( tform.cchr(p1) == 'Q' ) {
	    *ret_type  = FITS::LLONGARRDESC_T;
	    *ret_sz_bytes = sizeof(fits::llongarrdesc_t);
	}

	/* change target for next switch() statement */
	tgt_type = ret_heap_type;
	tgt_sz_bytes = ret_heap_sz_bytes;
	tgt_ch = tform.cchr(p2);

	if ( tform.cchr(p2 + 1) == '(' ) {
	    *ret_vl_max_length = tform.atol(p2 + 2);
	}
	else {
	    *ret_vl_max_length = -1;
	}
    }
    /* not using heap */
    else {
	*ret_heap_type = FITS::ANY_T;
	*ret_heap_sz_bytes = 0;
	*ret_vl_max_length = -1;
	/* change target for next switch() statement */
	tgt_type = ret_type;
	tgt_sz_bytes = ret_sz_bytes;
	tgt_ch = tform.cchr(p1);
    }

    /* set *ret_type or *ret_heap_type */
    switch ( tgt_ch ) {
    case 'L':
	*tgt_type  = FITS::LOGICAL_T;
	*tgt_sz_bytes = sizeof(fits::logical_t);
	break;
    case 'X':
	*tgt_type  = FITS::BIT_T;
	*tgt_sz_bytes = sizeof(fits::bit_t);
	break;
    case 'I':
	*tgt_type  = FITS::SHORT_T;
	*tgt_sz_bytes = sizeof(fits::short_t);
	break;
    case 'J':
	*tgt_type  = FITS::LONG_T;
	*tgt_sz_bytes = sizeof(fits::long_t);
	break;
    case 'K':
	*tgt_type  = FITS::LONGLONG_T;
	*tgt_sz_bytes = sizeof(fits::longlong_t);
	break;
    case 'A':
	*tgt_type  = FITS::STRING_T;
	*tgt_sz_bytes = sizeof(fits::byte_t);	/* for now */
	break;
    case 'E':
	*tgt_type  = FITS::FLOAT_T;
	*tgt_sz_bytes = -sizeof(fits::float_t);
	break;
    case 'D':
	*tgt_type  = FITS::DOUBLE_T;
	*tgt_sz_bytes = -sizeof(fits::double_t);
	break;
    case 'B':
	*tgt_type  = FITS::BYTE_T;
	*tgt_sz_bytes = sizeof(fits::byte_t);
	break;
    case 'P':
	*tgt_type  = FITS::LONGARRDESC_T;
	*tgt_sz_bytes = sizeof(fits::longarrdesc_t);
	break;
    case 'Q':
	*tgt_type  = FITS::LLONGARRDESC_T;
	*tgt_sz_bytes = sizeof(fits::llongarrdesc_t);
	break;
    case 'C':
	*tgt_type  = FITS::COMPLEX_T;
	*tgt_sz_bytes = sizeof(fits::complex_t);
	break;
    case 'M':
	*tgt_type  = FITS::DOUBLECOMPLEX_T;
	*tgt_sz_bytes = sizeof(fits::doublecomplex_t);
	break;
    }

    *ret_elem_size = new_elem_size;
    *ret_dcol_size = new_elem_size;

    if ( *ret_type == FITS::STRING_T ) {
	ssize_t t_pos, d_pos = -1;
	size_t t_endpos, d_endpos;
	long sz, dx, d0=new_elem_size, d1=1, d2=1;
	if ( new_elem_size == 0 ) {
	    d1 = 0;
	    d2 = 0;
	}
	t_pos = tform.strpbrk(p1+1,"[0-9]");	/* e.g., 120A10 */
	if ( 0 <= t_pos ) {
	    dx = tform.strtol(t_pos,10,&t_endpos);
	    if ( d0 < dx ) {
		status = 1;			/* for WARNING */
	    }
	    else {
		if ( 0 < dx ) {			/* 通常はここ */
		    d1 = d0 / dx;
		    d0 = dx;
		}
		else if ( dx == 0 ) {
		    if ( d0 == 0 ) {
			d1 = 0;
		    }
		    else {
			status = 1;		/* for WARNING */
		    }
		}
	    }
	    if ( 0 < tdim.length() ) {
		d_pos = tdim.strpbrk("[0-9]");
		if ( d_pos < 0 ) {
		    /* not found dim:0 */
		    status = 1;			/* for WARNING */
		}
	    }
	    else {
		d_pos = -1;
	    }
	}
	else if ( 0 < tdim.length() ) {		/* TDIMによる文字列長の指定 */
	    d_pos = tdim.strpbrk("[0-9]");
	    if ( 0 <= d_pos ) {
		dx = tdim.strtol(d_pos,10,&d_endpos);
		if ( d0 < dx ) {
		    status = 1;			/* for WARNING */
		}
		else {
		    if ( 0 < dx ) {		/* 通常はここ */
			d1 = d0 / dx;
			d0 = dx;
		    }
		    else if ( dx == 0 ) {
			if ( d0 == 0 ) {
			    d1 = 0;
			}
			else {
			    status = 1;		/* for WARNING */
			}
		    }
		}
		d_pos = tdim.strpbrk(d_endpos,"[0-9]");
	    }
	    else {
		/* not found dim:0 */
		status = 1;			/* for WARNING */
	    }
	}
	*ret_sz_bytes = sizeof(fits::byte_t) * d0;
	*ret_dcol_size = d1;
	*ret_elem_size = d1;
	if ( 0 <= d_pos ) {
	    dx = tdim.strtol(d_pos,10,&d_endpos);
	    if ( 0 < dx ) {			/* 通常はここ */
		d2 = d1 / dx;
		d1 = dx;
	    }
	    else if ( dx == 0 ) {
		if ( d1 == 0 ) {
		    d2 = 0;
		}
		else {
		    status = 1;			/* for WARNING */
		}
	    }
	    *ret_dcol_size = d1;
	    d_pos = tdim.strpbrk(d_endpos,"[0-9]");
	    if ( 0 <= d_pos ) {
		dx = tdim.strtol(d_pos,10,&d_endpos);
		if ( dx != d2 ) {
		    status = 1;			/* for WARNING */
		}
	    }
	}
	/* 正しいかチェックする */
	if ( *ret_elem_size < *ret_dcol_size ||
	     (*ret_dcol_size != 0 &&
	      *ret_elem_size % *ret_dcol_size != 0) ) {
	    *ret_dcol_size = *ret_elem_size;
	    status = 1;				/* for WARNING */
	}
	sz = sizeof(fits::byte_t) * new_elem_size; 
	if ( (*ret_sz_bytes) * (*ret_elem_size) != sz ) {
	    *ret_sz_bytes = sizeof(fits::byte_t) * new_elem_size;
	    *ret_elem_size = 1;
	    *ret_dcol_size = 1;
	    status = 1;				/* for WARNING */
	}
    }
    else {	/* STRING ではない場合 */
	if ( 0 < tdim.length() ) {
	    ssize_t d_pos;
	    size_t d_endpos;
	    d_pos = tdim.strpbrk("[0-9]");
	    if ( 0 <= d_pos ) {
		long dx, d0=new_elem_size, d1=1;
		if ( new_elem_size == 0 ) {
		    d1 = 0;
		}
		dx = tdim.strtol(d_pos,10,&d_endpos);
		if ( 0 < dx ) {			/* 通常はここ */
		    d1 = d0 / dx;
		    d0 = dx;
		}
		else if ( dx == 0 ) {
		    if ( d0 == 0 ) {
			d1 = 0;
		    }
		    else {
			status = 1;		/* for WARNING */
		    }
		}
		*ret_dcol_size = d0;
		d_pos = tdim.strpbrk(d_endpos,"[0-9]");
		if ( 0 <= d_pos ) {
		    dx = tdim.strtol(d_pos,10,&d_endpos);
		    if ( dx != d1 ) {
			status = 1;		/* for WARNING */
		    }
		}
	    }
	    else {
		/* not found dim:0 */
		status = 1;			/* for WARNING */
	    }
	    /* 正しいかチェックする */
	    if ( *ret_elem_size < *ret_dcol_size ||
		 (*ret_dcol_size != 0 &&
		  *ret_elem_size % *ret_dcol_size != 0) ) {
		*ret_dcol_size = *ret_elem_size;
		status = 1;			/* for WARNING */
	    }
	}
    }

    if ( *ret_type == FITS::BIT_T ) {
	if ( *ret_elem_size == 0 ) *ret_full_bytes = 0;
	else *ret_full_bytes = 1 + (*ret_elem_size - 1) / 8;
    }
    else {
	*ret_full_bytes = abs_sz2z(*ret_sz_bytes) * (*ret_elem_size);
    }

    /* debug */
#if 0
    err_report1(__FUNCTION__,"DEBUG","*ret_type = %d",*ret_type);
    err_report1(__FUNCTION__,"DEBUG","*ret_sz_bytes = %ld",*ret_sz_bytes);
    err_report1(__FUNCTION__,"DEBUG","*ret_elem_size = %ld",*ret_elem_size);
    err_report1(__FUNCTION__,"DEBUG","*ret_dcol_size = %ld",*ret_dcol_size);
    err_report1(__FUNCTION__,"DEBUG","*ret_full_bytes = %ld",*ret_full_bytes);
    err_report1(__FUNCTION__,"DEBUG","*ret_heap_type = %d",*ret_heap_type);
    err_report1(__FUNCTION__,"DEBUG","*ret_heap_sz_bytes = %ld",
		*ret_heap_sz_bytes);
#endif

 quit:
    return status;
}

/* fits_table からは，このメンバ関数を使わないこと．*/
/* change the definition of a column */
/**
 * @brief  カラム定義の設定 (fits::table_def 構造体を指定)
 *
 *  このメンバ関数は，新規カラムの定義の設定と，カラム定義の変更を兼ねていま
 *  す．def.xxx == NULL の場合は，その項目は変更しません．
 *
 * @param   def カラム定義
 * @return  自身の参照
 */
fits_table_col &fits_table_col::define( const fits::table_def &def )
{
    fits::table_def_all def_all;

    def_all.ttype         = def.ttype;
    def_all.ttype_comment = def.ttype_comment;
    def_all.talas         = def.talas;
    def_all.talas_comment = NULL;
    def_all.telem         = def.telem;
    def_all.telem_comment = NULL;
    def_all.tunit         = def.tunit;
    def_all.tunit_comment = def.tunit_comment;
    def_all.tdisp         = def.tdisp;
    def_all.tdisp_comment = NULL;
    def_all.tform         = def.tform;
    def_all.tform_comment = NULL;
    def_all.tdim          = def.tdim;
    def_all.tdim_comment  = NULL;
    def_all.tnull         = def.tnull;
    def_all.tnull_comment = NULL;
    def_all.tzero         = def.tzero;
    def_all.tzero_comment = NULL;
    def_all.tscal         = def.tscal;
    def_all.tscal_comment = NULL;

    return this->define(def_all);
}

/* fits_table からは，このメンバ関数を使わないこと．*/
/**
 * @brief  カラム定義の設定 (fits::table_def_all 構造体を指定)
 *
 *  このメンバ関数は，新規カラムの定義の設定と，カラム定義の変更を兼ねていま
 *  す．def.xxx == NULL の場合は，その項目は変更しません．
 *
 * @param   def カラム定義
 * @return  自身の参照
 */
fits_table_col &fits_table_col::define( const fits::table_def_all &def )
{
    if ( this->manager == NULL ) {	/* fits_table の管理下でない場合 */
	return this->_define(def);
    }
    else {				/* fits_table の管理下の場合 */
	long i;
	for ( i=0 ; i < this->manager->col_length() ; i++ ) {
	    if ( &(this->manager->col(i)) == this ) {
		/* 間接的に _define() を呼ぶ */
		this->manager->define_a_col(i, def);
		break;
	    }
	}
	if ( i == this->manager->col_length() ) {
	    err_throw(__FUNCTION__,"FATAL",
		      "internal error; fits_table does not manage me...");
	}
    }
    return *this;
}

/**
 * @brief  カラム定義の設定 (低レベル)
 *
 *  このメンバ関数は，新規カラムの定義の設定と，カラム定義の変更を兼ねていま
 *  す．def.xxx == NULL の場合は，その項目は変更しません．
 *
 * @param   def カラム定義
 * @return  自身の参照
 * @note  fits_table からは，このメンバ関数を使うこと．<br>
 *        このメンバ関数は protected です．
 */
fits_table_col &fits_table_col::_define( const fits::table_def_all &def )
{
    tstring tmp_str;
    int status;

    if ( this->is_protected() != false ) {
	err_report1(__FUNCTION__,"WARNING",
		    "This column [%s] is protected, cannot modify definitions",
		    this->tany.cstr(TTYPE_IDX));
	goto quit;
    }

    if ( def.tzero != NULL ) {
	tmp_str.assign(def.tzero).strtrim();
	this->tany.put(TZERO_IDX,tmp_str.cstr(),1);
	if ( 0 < tmp_str.length() ) this->tzero_rec = tmp_str.atof();
	else this->tzero_rec = 0;
	this->update_zero_of_data_rec();
    }

    if ( def.tzero_comment != NULL ) {
        tmp_str.assign(def.tzero_comment);
	this->tany.put(TZERO_COMMENT_IDX,tmp_str.cstr(),1);
    }

    if ( def.tscal != NULL ) {
	tmp_str.assign(def.tscal).strtrim();
	if ( 0 < tmp_str.length() ) {
	    if ( tmp_str.atof() == 0 ) {
		err_report(__FUNCTION__,"WARNING",
			   "ZERO TSCAL cannot be set, ignored.");
	    }
	    else {
		this->tany.put(TSCAL_IDX,tmp_str.cstr(),1);
		this->tscal_rec = tmp_str.atof();
	    }
	}
	else {
	    this->tany.put(TSCAL_IDX,"",1);
	    this->tscal_rec = 1;
	}
	this->update_zero_of_data_rec();
    }

    if ( def.tscal_comment != NULL ) {
        tmp_str.assign(def.tscal_comment);
	this->tany.put(TSCAL_COMMENT_IDX,tmp_str.cstr(),1);
    }

    if ( def.tform != NULL || def.tdim != NULL ) {
	int new_type = 0;
	long new_sz_bytes = 0;
	long new_elem_size = 0;
	long new_dcol_size = 0;
	long new_full_bytes = 0;
	int new_heap_type = 0;
	long new_heap_sz_bytes = 0;
	long new_vl_max_length = -1;
	tstring tmp_tform;
	tstring tmp_tdim;
	bool do_tnull_reset;
	if ( def.tform != NULL ) {
	    tmp_tform.assign(def.tform).strtrim();
	}
	else {
	    tmp_tform.assign(this->tany.cstr(TFORM_IDX));
	}

	if ( def.tdim != NULL ) {
	    tmp_tdim.assign(def.tdim).strtrim();
	}
	else {
	    tmp_tdim.assign(this->tany.cstr(TDIM_IDX));
	}
	status = parse_tform_and_tdim( tmp_tform.cstr(), tmp_tdim.cstr(),
				       &new_type, &new_sz_bytes, 
				       &new_elem_size, 
				       &new_dcol_size,
				       &new_full_bytes,
				       &new_heap_type, &new_heap_sz_bytes,
				       &new_vl_max_length );
	if ( status < 0 ) {
	    err_throw1(__FUNCTION__,"ERROR","unsupported TFORM [%s]",
		       tmp_tform.cstr());
	}
	if ( status != 0 ) {
	    err_report1(__FUNCTION__,"WARNING","TFORM = [%s]",
			tmp_tform.cstr());
	    err_report1(__FUNCTION__,"WARNING","TDIM  = [%s]",
			tmp_tdim.cstr());
	    err_report1(__FUNCTION__,"WARNING","new_sz_bytes = [%ld]",
			new_sz_bytes);
	    err_report1(__FUNCTION__,"WARNING","new_elem_size = [%ld]",
			new_elem_size);
	    err_report1(__FUNCTION__,"WARNING","new_dcol_size = [%ld]",
			new_dcol_size);
	    err_report1(__FUNCTION__,"WARNING","new_full_bytes = [%ld]",
			new_full_bytes);
	    err_report(__FUNCTION__,"WARNING",
		       "TFORM or TDIM includes non-standard definitions");
	}

	if ( this->type_rec != new_type || 
	     this->full_bytes_rec != new_full_bytes ||
	     this->heap_type_rec != new_heap_type ) {
	    /* initialize internal memory buffer */
	    try {
		/* heap を使う場合 */
		if ( new_type == FITS::LONGARRDESC_T || 
		     new_type == FITS::LLONGARRDESC_T ) {
		    const size_t nx[] = {new_elem_size, this->row_size_rec};
		    this->data_rec.init(new_sz_bytes, false, nx, 2, true);
		    this->heap_rec.init(new_heap_sz_bytes, false);
		}
		/* BIT_T の場合は特別 */
		else if ( new_type == FITS::BIT_T ) {
		    const size_t nx[] = {new_full_bytes, this->row_size_rec};
		    this->data_rec.init(1, false, nx, 2, true);
		    this->heap_rec.init(1, false);
		}
		/* その他の場合(STRING_Tも含む) */
		else {
		    const size_t nx[] = {new_elem_size,	this->row_size_rec};
		    this->data_rec.init(new_sz_bytes, false, nx, 2, true);
		    this->heap_rec.init(1, false);
		}
		/* set default value of internal memory buffer */
		if ( new_type == FITS::LONGARRDESC_T || 
		     new_type == FITS::LLONGARRDESC_T ) {
		    this->update_zero_of_data_rec( new_heap_type, true );
		}
		else {
		    this->update_zero_of_data_rec( new_type, false );
		}
		/* clean up */
		this->data_rec.clean();
		this->heap_rec.clean();
	    }
	    catch (...) {
		err_throw(__FUNCTION__,"FATAL",
			  "data_rec.init() or heap_rec.init() failed");
	    }
	}

	if ( this->vl_max_length_rec != new_vl_max_length ) {
	    this->vl_max_length_rec = new_vl_max_length;
	}

	if ( this->dcol_size_rec != new_dcol_size ) {
	    const int c_1 = 1;
	    try {
		this->bit_size_telem.assign_default((const void *)&c_1);
		this->bit_size_telem.resize(new_dcol_size).clean();
	    }
	    catch (...) {
		err_throw(__FUNCTION__,"FATAL",
			  "bit_size_telem.resize() failed");
	    }
	}

	if ( new_type == FITS::STRING_T ) {
	    size_t len_alc;
	    if ( (long)(this->null_svalue_rec.length()) < new_sz_bytes )
		len_alc = new_sz_bytes;
	    else 		/* "NULL" 表示のため */
		len_alc = this->null_svalue_rec.length();
	    /* */
	    if ( this->str_buf != NULL ) {
		delete this->str_buf;
		this->str_buf = NULL;
	    }
	    if ( this->tmp_str_buf != NULL ) {
		delete this->tmp_str_buf;
		this->tmp_str_buf = NULL;
	    }
	    try {
		this->str_buf = new tstring(len_alc);
		this->tmp_str_buf = new tstring(len_alc);
	    }
	    catch (...) {
		err_throw(__FUNCTION__,"FATAL","new failed");
	    }
	}
	else if (this->str_buf == NULL || this->str_buf->max_length() != 64) {
	    size_t len_alc = 64;
	    if ( this->str_buf != NULL ) {
		delete this->str_buf;
		this->str_buf = NULL;
	    }
	    if ( this->tmp_str_buf != NULL ) {
		delete this->tmp_str_buf;
		this->tmp_str_buf = NULL;
	    }
	    try {
		this->str_buf = new tstring(len_alc);
		this->tmp_str_buf = new tstring(len_alc);
	    }
	    catch (...) {
		err_throw(__FUNCTION__,"FATAL","new failed");
	    }
	}

	/* ヌル値のリセットを忘れていたので追加 2011/12/2 */
	if ( (this->type_rec != new_type || 
	      this->heap_type_rec != new_heap_type)
	     && this->tany.length(TNULL_IDX) <= 0 )
	    do_tnull_reset = true;
	else do_tnull_reset = false;

	this->type_rec = new_type;
	this->bytes_rec = abs_sz2z(new_sz_bytes);
	this->elem_size_rec = new_elem_size;
	this->dcol_size_rec = new_dcol_size;
	this->full_bytes_rec = new_full_bytes;
	this->heap_type_rec = new_heap_type;

	/* ヌル値のリセットを忘れていたので追加 2011/12/2 */
	if ( do_tnull_reset == true ) this->set_tnull(NULL);	/* リセット */

    }

    if ( def.talas != NULL ) {
	if ( def.talas[0] == '\0' /* NULL */ ) {
	    this->talas.resize(0);
	}
	else {
	    size_t i;
	    tarray_tstring tmp_arr;
	    tmp_arr.explode(def.talas,",",true);
	    this->talas.resize(0);
	    //for ( i=0 ; def.talas[i] != NULL ; i++ ) {
	    //    tmp_str.assign(def.talas[i]).strtrim();
	    //	this->talas.put(i,tmp_str.cstr(),1);
	    //}
	    for ( i=0 ; i < tmp_arr.length() ; i++ ) {
		tmp_arr[i].trim();
		this->talas.put(i,tmp_arr[i].cstr(),1);
	    }
	}
	tmp_str.assign(def.talas).trim();
	this->tany.put(TALAS_IDX,tmp_str.cstr(),1);
    }
    else {
	if ( this->talas.cstrarray() == NULL ) {
	    this->talas.resize(0);
	}
    }

    if ( def.talas_comment != NULL ) {
        tmp_str.assign(def.talas_comment);
	this->tany.put(TALAS_COMMENT_IDX,tmp_str.cstr(),1);
    }
    
    if ( def.telem != NULL ) {
	if ( def.telem[0] == '\0' /* NULL */ ) {
	    this->telem.resize(0);
	    this->telem_def.resize(0);
	    this->elem_index_rec.init();
	}
	else {
	    size_t i, j;
	    tarray_tstring tmp_arr;
	    tmp_arr.explode(def.telem,",",true);
	    this->telem.resize(0);
	    /* this->telem をなんとかする */
	    for ( i=0,j=0 ; i < tmp_arr.length() /* def.telem[i] != NULL */ ; i++ ) {
		ssize_t c_pos;
	        //tmp_str.assign(def.telem[i]).strtrim();
	        tmp_str.assign(tmp_arr[i]).trim();
		/* ビットフィールトを扱う */
		c_pos = tmp_str.strrchr(':');
		if ( 0 <= c_pos ) {
		    ssize_t k, n_bit = tmp_str.atoi(c_pos+1);
		    if ( 0 < n_bit ) {
			/* ':' より右側を消去 */
			tmp_str.erase(c_pos, tmp_str.length() - c_pos);
			for ( k=0 ; k < n_bit ; k++ ) {
			    this->telem.put(j,tmp_str.cstr(),1);
			    j++;
			}
		    }
		    else {
			err_report1(__FUNCTION__,"WARNING",
			       "Ignored invalid bitfield: %s", tmp_str.cstr());
			this->telem.put(j,tmp_str.cstr(),1);
			j++;
		    }
		}
		else {
		    this->telem.put(j,tmp_str.cstr(),1);
		    j++;
		}
	    }
	    /* this->telem_def をなんとかする */
	    this->telem_def.resize(0);
	    for ( i=0,j=0 ; i < this->telem.length() ; ) {
		size_t k, n_bit = 1;
		const tstring el_ref = this->telem.at_cs(i);
		if ( 0 < el_ref.length() ) {
		   for ( k=1 ; i+k < this->telem.length() ; k++ ) {
		      if ( el_ref.strcmp(this->telem.cstr(i+k)) == 0 ) n_bit++;
		      else break;
		   }
		}
		this->telem_def.put(j, this->telem.cstr(i),1);
		if ( 1 < n_bit ) {
		    this->telem_def.at(j).appendf(":%zu",n_bit);
		}
		j++;
		i += n_bit;
	    }
	    /* インデックスを張る */
	    this->elem_index_rec.init();
	    for ( i=0 ; i < this->telem.length() ; i++ ) {
		status = this->elem_index_rec.append(this->telem.cstr(i),i);
		if ( status < 0 ) {
		    err_throw(__FUNCTION__,"FATAL",
			       "this->elem_index_rec.append() failed");
		}
	    }
	}
	tmp_str.assign(def.telem).trim();
	this->tany.put(TELEM_IDX,tmp_str.cstr(),1);
    }
    else {
	if ( this->telem.cstrarray() == NULL ) {
	    this->telem.resize(0);
	}
	if ( this->telem_def.cstrarray() == NULL ) {
	    this->telem_def.resize(0);
	}
    }

    if ( def.telem_comment != NULL ) {
        tmp_str.assign(def.telem_comment);
	this->tany.put(TELEM_COMMENT_IDX,tmp_str.cstr(),1);
    }

    if ( def.tform != NULL || def.tdim != NULL || def.telem != NULL ) {
	long i;
	/* ビットの数を数える */
	for ( i=0 ; i < this->dcol_size_rec ; i++ ) {
	    int j;
	    const char *e0 = this->telem.cstr(i);
	    if ( e0 == NULL ) {
		this->bit_size_telem.i(i) = 1;	/* default */
	    }
	    else {
		for ( j=0 ; ; j++ ) {
		    const char *e1 = this->telem.cstr(i+j);
		    if ( e1 == NULL ) break;
		    if ( this->telem.at(i).strcmp(e1) != 0 ) break;
		    //if ( c_strcmp(e0,e1) != 0 ) break;
		}
		this->bit_size_telem.i(i) = j;
	    }
	}
    }

    if ( def.ttype != NULL ) {
        tmp_str.assign(def.ttype).strtrim();
	this->tany.put(TTYPE_IDX,tmp_str.cstr(),1);
    }

    if ( def.ttype_comment != NULL ) {
        tmp_str.assign(def.ttype_comment);
	this->tany.put(TTYPE_COMMENT_IDX,tmp_str.cstr(),1);
    }

    if ( def.tunit != NULL ) {
        tmp_str.assign(def.tunit).strtrim();
	this->tany.put(TUNIT_IDX,tmp_str.cstr(),1);
    }

    if ( def.tunit_comment != NULL ) {
        tmp_str.assign(def.tunit_comment);
	this->tany.put(TUNIT_COMMENT_IDX,tmp_str.cstr(),1);
    }

    if ( def.tdisp != NULL ) {
	bool a_ok = true;
	bool boz_ok = true;
	bool fegd_ok = true;
        tmp_str.assign(def.tdisp).strtrim();
	this->tany.put(TDISP_IDX,tmp_str.cstr(),1);
	/* */
	this->fmt_str = NULL;
	this->fmt_nullstr = NULL;
	/* 各データ型でどの TDISP のタイプを許可するかを設定 */
	if ( this->type_rec != FITS::STRING_T &&
	     this->type_rec != FITS::LOGICAL_T ) a_ok = false;
	if ( this->type_rec == FITS::STRING_T ) boz_ok = false;
	if ( this->type_rec == FITS::LOGICAL_T ) boz_ok = false;
	if ( this->type_rec == FITS::LOGICAL_T ) fegd_ok = false;
	if ( this->type_rec == FITS::BIT_T ) fegd_ok = false;
	if ( 0 < tmp_str.length() ) {
	    ssize_t pos = tmp_str.strpbrk("[-0-9]");
	    int len0 = 0;
	    if ( 0 <= pos ) len0 = tmp_str.atoi(pos);
	    if ( (tmp_str.cchr(0) == 'A' || tmp_str.cchr(0) == 'L') && 
		 a_ok == true ) {
		if ( len0 != 0 ) {
		    if ( tmp_str.cchr(0) == 'A' ) {
			/* 左寄せ */
			this->fmt_str.printf("%%%ds",-len0);
			this->fmt_nullstr.printf("%%%ds",-len0);
		    }
		    else {	/* for 'L' tdisp  */
			/* 右寄せ */
			this->fmt_str.printf("%%%ds",len0);
			this->fmt_nullstr.printf("%%%ds",len0);
		    }
		}
		else {
		    this->fmt_str.printf("%%s");
		    this->fmt_nullstr.printf("%%s");
		}
		if ( this->str_buf == NULL || 
		     this->str_buf->max_length() < (size_t)len0 ) {
		    size_t len_alc;
		    if ( (int)(this->null_svalue_rec.length()) < len0 )
			len_alc = len0;
		    else	/* "NULL" 表示のため */
			len_alc = this->null_svalue_rec.length();
		    /* */
		    if ( this->str_buf != NULL ) {
			delete this->str_buf;
			this->str_buf = NULL;
		    }
		    if ( this->tmp_str_buf != NULL ) {
			delete this->tmp_str_buf;
			this->tmp_str_buf = NULL;
		    }
		    try {
			this->str_buf = new tstring(len_alc);
			this->tmp_str_buf = new tstring(len_alc);
		    }
		    catch (...) {
			err_throw(__FUNCTION__,"FATAL","new failed");
		    }
		}
	    }
	    else if ( tmp_str.cchr(0) == 'B' && boz_ok == true ) {
		if ( 0 < len0 ) {
		    this->fmt_str.printf("%%%ds",len0+2);
		    this->fmt_nullstr.printf("%%%ds",len0+2);
		}
		else {
		    this->fmt_str.printf("%%s");
		    this->fmt_nullstr.printf("%%s");
		}
	    }
	    else if ( tmp_str.cchr(0) == 'O' && boz_ok == true ) {
		if ( 0 < len0 ) {
		    this->fmt_str.printf("%%#%dllo",len0+1);
		    this->fmt_nullstr.printf("%%%ds",len0+1);
		}
		else {
		    this->fmt_str.printf("%%#llo");
		    this->fmt_nullstr.printf("%%s");
		}
	    }
	    else if ( tmp_str.cchr(0) == 'Z' && boz_ok == true ) {
		if ( 0 < len0 ) {
		    this->fmt_str.printf("%%#%dllx",len0+2);
		    this->fmt_nullstr.printf("%%%ds",len0+2);
		}
		else {
		    this->fmt_str.printf("%%#llx");
		    this->fmt_nullstr.printf("%%s");
		}
	    }
	    else if ( tmp_str.cchr(0) == 'I' ) {
		if ( 0 < len0 ) {
		    this->fmt_str.printf("%%%dlld",len0);
		    this->fmt_nullstr.printf("%%%ds",len0);
		}
		else {
		    this->fmt_str.printf("%%lld");
		    this->fmt_nullstr.printf("%%s");
		}
	    }
	    else if ( (tmp_str.cchr(0) == 'F' || tmp_str.cchr(0) == 'E' ||
		       tmp_str.cchr(0) == 'G' || tmp_str.cchr(0) == 'D') &&
		      fegd_ok == true ) {
		ssize_t pos0 = tmp_str.strpbrk("[0-9]");
		ssize_t pos1 = tmp_str.strchr('.');
		int len0 = 0;
		int len1 = -1;
		if ( 0 <= pos0 ) {
		    if ( 0 <= pos1 ) {
			if ( pos0 < pos1 ) len0 = tmp_str.atoi(pos0);
		    }
		    else len0 = tmp_str.atoi(pos0);
		}
		if ( 0 <= pos1 ) {
		    pos1 = tmp_str.strpbrk(pos1,"[0-9]");
		    if ( 0 <= pos1 ) {
			len1 = tmp_str.atoi(pos1);
		    }
		}
		if ( tmp_str.cchr(0) == 'E' ) {
		    if ( 0 < len0 ) {
			if (0 <= len1) {
			    this->fmt_str.printf("%%%d.%dE",len0,len1);
			    this->fmt_nullstr.printf("%%%ds",len0);
			}
			else {
			    this->fmt_str.printf("%%%dE",len0);
			    this->fmt_nullstr.printf("%%%ds",len0);
			}
		    }
		    else {
			if (0 <= len1) {
			    this->fmt_str.printf("%%.%dE",len1);
			    this->fmt_nullstr.printf("%%s");
			}
			else {
			    this->fmt_str.printf("%%E");
			    this->fmt_nullstr.printf("%%s");
			}
		    }
		}
		else if ( tmp_str.cchr(0) == 'G' ) {
		    if ( 0 < len0 ) {
			if (0 <= len1) {
			    this->fmt_str.printf("%%%d.%dG",len0,len1);
			    this->fmt_nullstr.printf("%%%ds",len0);
			}
			else {
			    this->fmt_str.printf("%%%dG",len0);
			    this->fmt_nullstr.printf("%%%ds",len0);
			}
		    }
		    else {
			if (0 <= len1) {
			    this->fmt_str.printf("%%.%dG",len1);
			    this->fmt_nullstr.printf("%%s");
			}
			else {
			    this->fmt_str.printf("%%G");
			    this->fmt_nullstr.printf("%%s");
			}
		    }
		}
		/* 'D' の扱いはこれで正しい? */
		/* (fv の動作では %f だが，FORTRAN的には %E で後で D に変更) */
		else if ( tmp_str.cchr(0) == 'D' ) {
		    if ( 0 < len0 ) {
			if (0 <= len1) {
			    this->fmt_str.printf("%%%d.%dE",len0,len1);
			    this->fmt_nullstr.printf("%%%ds",len0);
			}
			else {
			    this->fmt_str.printf("%%%d.15E",len0);
			    this->fmt_nullstr.printf("%%%ds",len0);
			}
		    }
		    else {
			if (0 <= len1) {
			    this->fmt_str.printf("%%.%dE",len1);
			    this->fmt_nullstr.printf("%%s");
			}
			else {
			    this->fmt_str.printf("%%.15E");
			    this->fmt_nullstr.printf("%%s");
			}
		    }
		}
		/* 'F' の場合 */
		else {
		    if ( 0 < len0 ) {
			if (0 <= len1) {
			    this->fmt_str.printf("%%%d.%df",len0,len1);
			    this->fmt_nullstr.printf("%%%ds",len0);
			}
			else {
			    this->fmt_str.printf("%%%df",len0);
			    this->fmt_nullstr.printf("%%%ds",len0);
			}
		    }
		    else {
			if (0 <= len1) {
			    this->fmt_str.printf("%%.%df",len1);
			    this->fmt_nullstr.printf("%%s");
			}
			else {
			    this->fmt_str.printf("%%f");
			    this->fmt_nullstr.printf("%%s");
			}
		    }
		}
	    }
	}
    }
	
    if ( def.tdisp_comment != NULL ) {
        tmp_str.assign(def.tdisp_comment);
	this->tany.put(TDISP_COMMENT_IDX,tmp_str.cstr(),1);
    }

    if ( def.tform != NULL ) {
        tmp_str.assign(def.tform).strtrim();
	this->tany.put(TFORM_IDX,tmp_str.cstr(),1);
    }

    if ( def.tform_comment != NULL ) {
        tmp_str.assign(def.tform_comment);
	this->tany.put(TFORM_COMMENT_IDX,tmp_str.cstr(),1);
    }

    if ( def.tdim != NULL ) {
        tmp_str.assign(def.tdim).strtrim();
	this->tany.put(TDIM_IDX,tmp_str.cstr(),1);
    }

    if ( def.tdim_comment != NULL ) {
        tmp_str.assign(def.tdim_comment);
	this->tany.put(TDIM_COMMENT_IDX,tmp_str.cstr(),1);
    }

    /*
     * TNULL の処理は混乱しやすいので注意
     *  def.tnull = NULL   -> 何もしない
     *  def.tnull = ""     -> 定義を消去
     *  def.tnull = " "    -> this->bytes() 個の空白を NULL 値とする
     */
    if ( def.tnull != NULL ) {
	if ( this->type_rec != FITS::STRING_T ) {
	    tmp_str.assign(def.tnull).strtrim();
	    if ( 0 < tmp_str.length() ) {
		long long tnl = tmp_str.atoll();
		this->set_tnull(&tnl);			/* セット */
	    }
	    else this->set_tnull(NULL);			/* リセット */
	}
	else {
	    tmp_str.assign(def.tnull);
	    if ( 0 < tmp_str.length() ) {
		/* Changed 2011/12/1 */
		//tmp_str.resize(this->full_bytes_rec);
		if ( (long)(tmp_str.length()) <= this->bytes() ) {
		    tmp_str.resize(this->bytes());
		}
		/* NULL 値の長さが大きすぎる場合，*/
		/* ここで警告を出しても良いが，ウザいから黙っておく */
	    }
	    /* これは整数の NULL 設定をリセットしている */
	    this->set_tnull(NULL);
	}
	this->tany.put(TNULL_IDX,tmp_str.cstr(),1);
	if ( 0 < this->tany.length(TNULL_IDX) ) {
	    this->tnull_is_set_rec = true;
	}
	else {
	    this->tnull_is_set_rec = false;
	}
    }

    if ( def.tnull_comment != NULL ) {
        tmp_str.assign(def.tnull_comment);
	this->tany.put(TNULL_COMMENT_IDX,tmp_str.cstr(),1);
    }

    /* ヌルポはないようにする */
    this->tany.resize(TANY_ARR_SIZE);

    this->update_def_rec_ptr();

 quit:
    return *this;
}

/*
 * Functions to convert numeric columns
 */

/**
 * @brief  データ型の変換・変更
 *
 *  可変長配列ではない整数型あるいは実数型のカラム(TFORMn の指定が 'B'，'I'，
 *  'J'，'K'，'E'，'D'を含む場合)の型を new_type に変更します．
 *  必要に応じて，内部バッファのサイズも変更します．<br>
 *  new_type に指定できる値は，FITS::DOUBLE_T，FITS::FLOAT_T，
 *  FITS::LONGLONG_T，FITS::LONG_T，FITS::SHORT_T，FITS::BYTE_T のいずれか
 *  です．<br>
 *  new_zero，new_scale，new_null が指定された場合は，ヘッダの TZEROn，
 *  TSCALn，TNULLn も変更し，それらの値を反映したデータに変換します．<br>
 *  new_null が有効なのは，new_type が整数型の場合のみです．<br>
 *  文字列型や論理型のカラムはこのメンバ関数では変換できません．
 *
 * @param  new_type 変換後のデータ型
 * @return  自身の参照
 * @attention  可変長配列はサポートされていません．
 */
fits_table_col &fits_table_col::convert_type( int new_type )
{
    return this->convert_col_type( new_type, NULL, NULL, NULL );
}

/**
 * @brief  データ型の変換・変更
 *
 *  可変長配列ではない整数型あるいは実数型のカラム(TFORMn の指定が 'B'，'I'，
 *  'J'，'K'，'E'，'D'を含む場合)の型を new_type に変更します．
 *  必要に応じて，内部バッファのサイズも変更します．<br>
 *  new_type に指定できる値は，FITS::DOUBLE_T，FITS::FLOAT_T，
 *  FITS::LONGLONG_T，FITS::LONG_T，FITS::SHORT_T，FITS::BYTE_T のいずれか
 *  です．<br>
 *  ヘッダの TZEROn，も変更し，それらの値を反映したデータに変換します．<br>
 *  文字列型や論理型のカラムはこのメンバ関数では変換できません．
 *
 * @param  new_type 変換後のデータ型
 * @param  new_zero 変換後のTZEROn値
 * @return  自身の参照
 * @attention  可変長配列はサポートされていません．
 */
fits_table_col &fits_table_col::convert_type( int new_type, double new_zero )
{
    return this->convert_col_type( new_type, &new_zero, NULL, NULL );
}

/**
 * @brief  データ型の変換・変更
 *
 *  可変長配列ではない整数型あるいは実数型のカラム(TFORMn の指定が 'B'，'I'，
 *  'J'，'K'，'E'，'D'を含む場合)の型を new_type に変更します．
 *  必要に応じて，内部バッファのサイズも変更します．<br>
 *  new_type に指定できる値は，FITS::DOUBLE_T，FITS::FLOAT_T，
 *  FITS::LONGLONG_T，FITS::LONG_T，FITS::SHORT_T，FITS::BYTE_T のいずれか
 *  です．<br>
 *  ヘッダの TZEROn，TSCALn，も変更し，それらの値を反映したデータに変換しま
 *  す．<br>
 *  文字列型や論理型のカラムはこのメンバ関数では変換できません．
 *
 * @param  new_type 変換後のデータ型
 * @param  new_zero 変換後のTZEROn値
 * @param  new_scale 変換後のTSCALn値
 * @return  自身の参照
 * @attention  可変長配列はサポートされていません．
 */
fits_table_col &fits_table_col::convert_type( int new_type, double new_zero,
					      double new_scale )
{
    return this->convert_col_type( new_type, &new_zero, &new_scale, NULL );
}

/**
 * @brief  データ型の変換・変更
 *
 *  可変長配列ではない整数型あるいは実数型のカラム(TFORMn の指定が 'B'，'I'，
 *  'J'，'K'，'E'，'D'を含む場合)の型を new_type に変更します．
 *  必要に応じて，内部バッファのサイズも変更します．<br>
 *  new_type に指定できる値は，FITS::DOUBLE_T，FITS::FLOAT_T，
 *  FITS::LONGLONG_T，FITS::LONG_T，FITS::SHORT_T，FITS::BYTE_T のいずれか
 *  です．<br>
 *  ヘッダの TZEROn，TSCALn，TNULLn も変更し，それらの値を反映したデータに
 *  変換します．<br>
 *  new_null が有効なのは，new_type が整数型の場合のみです．<br>
 *  文字列型や論理型のカラムはこのメンバ関数では変換できません．
 *
 * @param  new_type 変換後のデータ型
 * @param  new_zero 変換後のTZEROn値
 * @param  new_scale 変換後のTSCALn値
 * @param  new_null 変換後のTNULLn値 
 * @return  自身の参照
 * @attention  可変長配列はサポートされていません．
 */
fits_table_col &fits_table_col::convert_type( int new_type, double new_zero, 
					      double new_scale,
					      long long new_null )
{
    return this->convert_col_type( new_type, &new_zero, &new_scale, &new_null);
}

/*
 * Functions to set TNULL and scaling parameters
 */

/**
 * @brief  物理単位(TUNITn の値)の設定
 */
fits_table_col &fits_table_col::assign_tunit( const char *unit )
{
    fits::table_def_all def = {NULL};
    def.tunit = unit;
    return this->_define(def);
}

/**
 * @brief  物理単位(TUNITn)の設定の消去
 *
 * @return  自身の参照
 */
fits_table_col &fits_table_col::erase_tunit()
{
    fits::table_def_all def = {NULL};
    def.tunit = "";
    return this->_define(def);
}

/**
 * @brief  整数型のカラムに対する NULL値(TNULLn の値)を long long 型で設定
 */
fits_table_col &fits_table_col::assign_tnull( long long null )
{
    tstring sval;
    fits::table_def_all def = {NULL};
    sval.printf("%lld",null);
    def.tnull = sval.cstr();
    return this->_define(def);
}

/**
 * @brief  整数型のカラムに対する NULL値(TNULLn の値)を文字列で設定
 */
fits_table_col &fits_table_col::assign_tnull( const char *null )
{
    if ( null != NULL ) {
	fits::table_def_all def = {NULL};
	def.tnull = null;
	return this->_define(def);
    }
    else return *this;
}

/**
 * @brief  整数型のカラムに対する NULL値(TNULLn)の設定の消去
 *
 * @return  自身の参照
 */
fits_table_col &fits_table_col::erase_tnull()
{
    fits::table_def_all def = {NULL};
    def.tnull = "";
    return this->_define(def);
}

/**
 * @brief  ゼロ点(TZEROn の値)の設定
 *
 * @param   zero TZEROn の値
 * @param   prec 精度(桁数)．省略時は15．
 * @return  自身の参照
 */
fits_table_col &fits_table_col::assign_tzero( double zero, int prec )
{
    tstring val, fmt;
    fits::table_def_all def = {NULL};
    ssize_t ep;
    fmt.printf("%%.%dG",prec);
    val.printf(fmt.cstr(),zero);
    ep = val.strchr('E');
    if ( ep < 0 && val.strchr('.') < 0 ) val.append(".");
    if ( 0 <= ep ) val.at(ep) = 'D';
    def.tzero = val.cstr();
    return this->_define(def);
}

/**
 * @brief  ゼロ点(TZEROn)の設定の消去
 *
 * @return  自身の参照
 */
fits_table_col &fits_table_col::erase_tzero()
{
    fits::table_def_all def = {NULL};
    def.tzero = "";
    return this->_define(def);
}

/**
 * @brief  スケーリングファクター(TSCALn の値)の設定
 *
 * @param   scal TSCALn の値
 * @param   prec 精度(桁数)．省略時は15．
 * @return  自身の参照
 */
fits_table_col &fits_table_col::assign_tscal( double scal, int prec )
{
    tstring val, fmt;
    fits::table_def_all def = {NULL};
    ssize_t ep;
    fmt.printf("%%.%dG",prec);
    val.printf(fmt.cstr(),scal);
    ep = val.strchr('E');
    if ( ep < 0 && val.strchr('.') < 0 ) val.append(".");
    if ( 0 <= ep ) val.at(ep) = 'D';
    def.tscal = val.cstr();
    return this->_define(def);
}

/**
 * @brief  スケーリングファクター(TSCALn)の設定の消去
 *
 * @return  自身の参照
 */
fits_table_col &fits_table_col::erase_tscal()
{
    fits::table_def_all def = {NULL};
    def.tscal = "";
    return this->_define(def);
}

/*
 * Functions to resize, erase and insert rows.
 */

/* fits_table_col, fits_table からは，このメンバ関数を使わないこと */
/**
 * @brief  行数の変更
 *
 *  新しいセルには，デフォルト値がセットされます．デフォルト値の初期設定値は，
 *  論理型の場合は 'F'，文字列型の場合は ' ' からなる文字列，その他の型の場合は
 *  0 です．デフォルト値は fits_table_col::assign_default() メンバ関数で変更で
 *  きます．<br>
 *  自身が fits_table オブジェクトの管理下にある場合，他のカラムも同時に
 *  処理が実行されます．
 *
 * @param   num_rows 変更後の行数
 * @return  自身の参照
 * @attention  可変長配列の場合，配列記述子に対する操作となります．
 */
fits_table_col &fits_table_col::resize( long num_rows )
{
    if ( this->manager == NULL ) {	/* fits_table の管理下でない場合 */
	return this->_resize(num_rows);
    }
    else {				/* fits_table の管理下の場合 */
	/* 間接的に _resize() を呼ぶ */
	this->manager->resize_rows(num_rows);
	return *this;
    }
}

/**
 * @brief  行数の変更 (低レベル)
 *
 * @param   num_rows 変更後の行数
 * @return  自身の参照
 * @note    fits_table からは，このメンバ関数を使う事．<br>
 *          このメンバ関数は protected です．
 */
fits_table_col &fits_table_col::_resize( long num_rows )
{
    if ( this->full_bytes_rec == 0 ) return *this;	/* not defined */
    if ( num_rows < 0 ) return *this;			/* invalid */

    this->data_rec.resize(1, num_rows);
    this->row_size_rec = this->data_rec.row_length();

    return *this;
}

/*
 * Functions to edit rows
 */

/**
 * @brief  行から行へのコピー
 *
 * @param  src_index コピー元を示す行インデックス
 * @param  num_rows コピーを行う行数
 * @param  dest_index コピー先を示す行インデックス
 * @return  自身の参照
 * @attention  可変長配列の場合，配列記述子に対する操作となります．
 */
fits_table_col &fits_table_col::move( long src_index, long num_rows, 
				      long dest_index )
{
    if ( this->full_bytes_rec == 0 ) return *this;	/* not defined */
    if ( num_rows < 0 ) return *this;			/* invalid */

    this->data_rec.move(1, src_index, num_rows, dest_index, false);

    return *this;
}

/**
 * @brief  行と行との入れ替え
 *
 *  index0 で指定された行から num_rows 個の行を，index1 で指定された行から
 *  num_cols 個の行と入れ替えます．<br>
 *  num_cols によって，重なる行がある場合は，num_cols の値を減らして入れ替えを
 *  行ないます．
 *
 * @param  index0 入れ替え元を示す行インデックス
 * @param  num_rows 入れ替えを行う行数
 * @param  index1 入れ替え先を示す行インデックス
 * @return  自身の参照
 * @attention  可変長配列の場合，配列記述子に対する操作となります．
 */
fits_table_col &fits_table_col::swap( long index0, long num_rows, 
				      long index1 )
{
    if ( this->full_bytes_rec == 0 ) return *this;	/* not defined */
    if ( num_rows < 0 ) return *this;			/* invalid */

    this->data_rec.swap(1, index0, num_rows, index1);

    return *this;
}

/**
 * @brief  行の順序の反転
 *
 * @param   index 反転開始位置を示す行インデックス
 * @param   num_rows 反転される行数
 * @return  自身の参照
 * @attention  可変長配列の場合，配列記述子に対する操作となります．
 */
fits_table_col &fits_table_col::flip( long index, long num_rows )
{
    if ( this->full_bytes_rec == 0 ) return *this;	/* not defined */
    if ( num_rows < 0 ) return *this;			/* invalid */

    this->data_rec.flip(1, index, num_rows);

    return *this;
}

/* fits_table_col, fits_table からは，このメンバ関数を使わないこと */
/**
 * @brief  行の消去
 *
 *  自身が fits_table オブジェクトの管理下にある場合，他のカラムも同時に
 *  処理が実行されます．
 *
 * @param   index 削除開始位置を示す行インデックス
 * @param   num_rows 削除される行数
 * @return  自身の参照
 * @attention  テーブルの横方向のバイト数と行数が大きく，かつ index が小さい
 *             ほど，メモリのコピー処理の実行コストが増大します．
 * @attention  可変長配列の場合，配列記述子に対する操作となります．
 */
fits_table_col &fits_table_col::erase( long index, long num_rows )
{
    if ( this->manager == NULL ) {	/* fits_table の管理下でない場合 */
	return this->_erase(index, num_rows);
    }
    else {				/* fits_table の管理下の場合 */
	/* 間接的に _erase() を呼ぶ */
	this->manager->erase_rows(index, num_rows);
	return *this;
    }
}

/**
 * @brief  行の消去 (低レベル)
 *
 * @param   index 削除開始位置を示す行インデックス
 * @param   num_rows 削除される行数
 * @return  自身の参照
 * @note    fits_table からは，このメンバ関数を使う事．
 *          このメンバ関数は protected です．
 */
fits_table_col &fits_table_col::_erase( long index, long num_rows )
{
    if ( this->full_bytes_rec == 0 ) return *this;	/* not defined */
    if ( num_rows < 0 ) return *this;			/* invalid */

    this->data_rec.erase(1, index, num_rows);
    this->row_size_rec = this->data_rec.row_length();

    return *this;
}

/* fits_table_col, fits_table からは，このメンバ関数を使わないこと */
/**
 * @brief  新しい行の挿入
 *
 *  新しいセルには，デフォルト値がセットされます．デフォルト値の初期設定値は，
 *  論理型の場合は 'F'，文字列型の場合は ' ' からなる文字列，その他の型の場合は
 *  0 です．デフォルト値は fits_table_col::assign_default() メンバ関数で変更で
 *  きます．<br>
 *  自身が fits_table オブジェクトの管理下にある場合，他のカラムも同時に
 *  処理が実行されます．
 *
 * @param   index 挿入位置を示す行インデックス
 * @param   num_rows 挿入される行数
 * @return  自身の参照
 * @attention  テーブルの横方向のバイト数と行数が大きく，かつ index が小さい
 *             ほど，メモリのコピー処理の実行コストが増大します．
 * @attention  可変長配列の場合，配列記述子に対する操作となります．
 */
fits_table_col &fits_table_col::insert( long index, long num_rows )
{
    if ( this->manager == NULL ) {	/* fits_table の管理下でない場合 */
	return this->_insert(index,num_rows);
    }
    else {				/* fits_table の管理下の場合 */
	/* 間接的に _insert() を呼ぶ */
	this->manager->insert_rows(index,num_rows);
	return *this;
    }
}

/**
 * @brief  新しい行の挿入 (低レベル)
 *
 * @return  自身の参照
 * @note    fits_table からは，このメンバ関数を使う事．
 *          このメンバ関数は protected です．
 */
fits_table_col &fits_table_col::_insert( long index, long num_rows )
{
    if ( this->full_bytes_rec == 0 ) return *this;	/* not defined */
    if ( num_rows < 0 ) return *this;			/* invalid */

    this->data_rec.insert(1, index, num_rows);
    this->row_size_rec = this->data_rec.row_length();

    return *this;
}

/**
 * @brief  将来の実装のためのメンバ関数
 *
 * @note   このメンバ関数は protected です．
 */
void **fits_table_col::data_ptr_mng_ptr()
{
    return &this->data_ptr_rec;
}

/**
 * @brief  将来の実装のためのメンバ関数
 *
 * @note   このメンバ関数は protected です．
 */
void **fits_table_col::heap_ptr_mng_ptr()
{
    return &this->heap_ptr_rec;
}

/**
 * @brief  外部カラムオブジェクトからのインポート
 *
 *  fits_table_col オブジェクト from の idx_begin から num_rows 個の行を，
 *  自身の dest_index で指定された行から num_rows個の行へインポートします．<br>
 *  from の持つカラムと当該オブジェクトのカラムの型は，一致している必要は
 *  ありません．一致しない場合は，値を変換してインポートします．
 *
 * @param  dest_index インポート先の行インデックス
 * @param  from インポート元のテーブルオブジェクト
 * @param  idx_begin インポート元の行インデックス
 * @param  num_rows インポートを行う行数
 * @return  自身の参照
 * @note  インポート先(自身)の行数の変更は行ないません．行の欠損なくインポート
 *        するには，あらかじめ十分な行数を確保しておく必要があります．
 * @attention  可変長配列はサポートされていません．
 */
fits_table_col &fits_table_col::import( long dest_index, 
				    const fits_table_col &from, long idx_begin,
				    long num_rows )
{
    if ( this->full_bytes_rec == 0 ) return *this;	/* not defined */
    if ( from.full_bytes_rec == 0 ) return *this;	/* not defined */

    /* reject variable length array */
    if ( this->heap_is_used() == true ) {
	err_report1(__FUNCTION__,"WARNING", 
		    "cannot use this function for TFORM = %s",
		    this->tany.cstr(TFORM_IDX));
	return *this;
    }
    if ( from.heap_is_used() == true ) {
	err_report1(__FUNCTION__,"WARNING", 
		    "cannot import TFORM = %s",from.tany.cstr(TFORM_IDX));
	return *this;
    }

    /* check args */
    if ( num_rows == FITS::ALL )
	num_rows = from.length() - idx_begin;

    if ( dest_index < 0 || this->row_size_rec <= dest_index || 
	 num_rows < 0 || idx_begin < 0 ) {
	goto quit;	/* invalid */
    }
    if ( from.length() <= idx_begin ) {
	goto quit;	/* invalid */
    }
    if ( num_rows == 0 ) {
	goto quit;
    }
    
    if ( this->row_size_rec < dest_index + num_rows ) {
	num_rows = this->row_size_rec - dest_index;
    }
    if ( from.length() < idx_begin + num_rows ) {
	num_rows = from.length() - idx_begin;
    }

    /* import */
    if ( this->type() == from.type() &&
	 this->bytes() == from.bytes() &&
	 this->elem_length() == from.elem_length() &&
	 this->tzero() == from.tzero() &&
	 this->tscal() == from.tscal() &&
	 this->tnull_is_set() == from.tnull_is_set() &&
	 this->tnull() == from.tnull() ) {
	long full_bytes = this->full_bytes_rec;
	this->data_rec.putdata(
		(const char *)from.data_ptr_cs() + full_bytes * idx_begin,
		full_bytes * num_rows,
		0, dest_index);
    }
    else {
	long i,j,ix,id;
	/* &from == this でも良いように */
	if ( dest_index <= idx_begin ) {
	    ix = 0;
	    id = 1;
	}
	else {
	    ix = num_rows - 1;
	    id = -1;
	}
	if ( this->type_rec == FITS::STRING_T ) {
	    tstring sval;
	    for ( i=0 ; i < num_rows ; i++, ix += id ) {
		for ( j=0 ; j < this->elem_size_rec ; j++ ) {
		    const char *v = from.get_svalue(idx_begin+ix,j,sval);
		    this->assign(v,dest_index+ix,j);
		}
	    }
	}
	//else if ( this->type_rec == FITS::LOGICAL_T ) {
	//    if ( from.type() == FITS::STRING_T ||
	//	 from.type() == FITS::LOGICAL_T ) {
	//	for ( i=0 ; i < num_rows ; i++, ix += id ) {
	//	    for ( j=0 ; j < this->elem_size_rec ; j++ ) {
	//		int v = from.logical_value(idx_begin+ix,j);
	//		this->assign_logical(v,dest_index+ix,j);
	//	    }
	//	}
	//   }
	//   else {
	//	for ( i=0 ; i < num_rows ; i++, ix += id ) {
	//	    for ( j=0 ; j < this->elem_size_rec ; j++ ) {
	//		bool v = from.bvalue(idx_begin+ix,j);
	//		this->assign(v,dest_index+ix,j);
	//	    }
	//	}
	//    }
	//}
	if ( this->type_rec == FITS::LONGLONG_T ) {
	    for ( i=0 ; i < num_rows ; i++, ix += id ) {
		for ( j=0 ; j < this->elem_size_rec ; j++ ) {
		    long long v = from.llvalue(idx_begin+ix,j);
		    this->assign(v,dest_index+ix,j);
		}
	    }
	}
	else {
	    for ( i=0 ; i < num_rows ; i++, ix += id ) {
		for ( j=0 ; j < this->elem_size_rec ; j++ ) {
		    double v = from.dvalue(idx_begin+ix,j);
		    this->assign(v,dest_index+ix,j);
		}
	    }
	}
    }

 quit:
    return *this;
}

/* fill all or part of rows using default value */
/**
 * @brief  一部の行のセルの値を初期化(デフォルト値をセット)
 *
 *  デフォルト値の初期設定値は，論理型の場合は 'F'，文字列型の場合は ' ' から
 *  なる文字列，その他の型の場合は 0 です．デフォルト値は 
 *  fits_table_col::assign_default() メンバ関数で変更できます．
 *
 * @param   index 初期化開始位置を示す行インデックス
 * @param   num_rows 初期化される行数
 * @return  自身の参照
 * @attention  可変長配列の場合，配列記述子に対する操作となります．
 */
fits_table_col &fits_table_col::clean( long index, long num_rows )
{
    if ( this->full_bytes_rec == 0 ) return *this;	/* not defined */
    if ( num_rows < 0 ) return *this;			/* invalid */

    this->data_rec.clean( 0, this->data_rec.col_length(),
			  index, num_rows );

    return *this;
}

/**
 * @brief  全ての行のセルの値を初期化(デフォルト値をセット)
 *
 *  デフォルト値の初期設定値は，論理型の場合は 'F'，文字列型の場合は ' ' から
 *  なる文字列，その他の型の場合は 0 です．デフォルト値は 
 *  fits_table_col::assign_default() メンバ関数で変更できます．
 *
 * @return  自身の参照
 * @attention  可変長配列の場合，配列記述子に対する操作となります．
 */
fits_table_col &fits_table_col::clean()
{
    return this->clean( 0, this->row_size_rec );
}

#define REMOVE_SPC(ts)	{ \
    ssize_t i_s; \
    ts.strtrim(' '); \
    for ( i_s=0 ; 0 <= (i_s=ts.strchr(i_s,' ')) ; i_s++ ) { \
        ts.erase(i_s,ts.strspn(i_s,' ')); \
    } \
}

/*
 * high-level APIs to read a cell.
 *  - TSCAL and TZERO are applied.
 *  - NULL value is handled.
 */

/**
 * @brief  セルの値を実数値で取得 (高レベル)
 *
 *  セルの値に，ヘッダの TZEROn と TSCALn を反映した実数値を取得します．<br>
 *  NULL値の場合は，NANを返します．バイナリテーブルの整数型カラムやアスキー
 *  テーブルの TNULLn の値に，セルの値が一致した場合もNULL値とします．<br>
 *  論理型のカラムの場合は，値が 'T' なら 1 を，値が 'F' なら 0 を，それ以外の
 *  場合は NAN を返します．<br>
 *  バイナリテーブルの文字列型のカラムの場合と，アスキーテーブルの TFORMn
 *  が数値表現を示さない場合には，セルの文字列を実数値に変換した値をそのまま
 *  返します．<br>
 *  引数のインデックスは，0から始まる数字です．
 *
 * @param   row_index 行インデックス
 * @return  セルの値: 正常終了．NULL値の場合はNAN <br>
 *          NAN: 引数が不正な場合
 * @note  バイナリテーブルの文字列型のカラムでも TNULLn の設定が可能です．これ
 *        は SFITSIO の独自拡張です．
 * @attention  可変長配列はサポートされていません．
 */
double fits_table_col::dvalue( long row_index ) const
{
    double v;
    if ( this->type_rec == FITS::BIT_T || 
	 this->type_rec == FITS::LOGICAL_T )
	v = this->double_value(row_index);
    else if ( this->type_rec == FITS::STRING_T ) {
	if ( row_index < 0 || this->row_size_rec <= row_index ) return NAN;
	tstring sval;
	this->get_string_value(row_index, &sval);
	if ( sval.length() == 0 && this->tnull_is_set_rec == false ) return NAN;
	else if ( this->tnull_is_set_rec == true &&
		  this->tany.at_cs(TNULL_IDX).strcmp(sval.cstr()) == 0 )
	    return NAN;
	else {
	    REMOVE_SPC(sval);
	    v = sval.atof();
	    if ( this->tany.at_cs(TDISP_IDX).strpbrk("FEGDI") == 0 ) {
		v = this->tzero_rec + v * this->tscal_rec;
	    }
	}
    }
    else if ( this->tnull_is_set_rec == true ) {
	if ( row_index < 0 || this->row_size_rec <= row_index ) return NAN;
	if ( this->type_rec == FITS::LONG_T ||
	     this->type_rec == FITS::SHORT_T ||
	     this->type_rec == FITS::BYTE_T ||
	     this->type_rec == FITS::LONGLONG_T ) {
	    long long llv = this->longlong_value(row_index);
	    if ( llv == this->tnull_r_rec ) return NAN;
	    else v = this->tzero_rec + llv * this->tscal_rec;
	}
	else {
	    v = this->tzero_rec + 
		this->double_value(row_index) * this->tscal_rec;
	}
    }
    else
	v = this->tzero_rec + this->double_value(row_index) * this->tscal_rec;
    return v;
}

/**
 * @brief  セルの値を実数値で取得 (高レベル)
 *
 *  セルの値に，ヘッダの TZEROn と TSCALn を反映した実数値を取得します．<br>
 *  NULL値の場合は，NANを返します．バイナリテーブルの整数型カラムやアスキー
 *  テーブルの TNULLn の値に，セルの値が一致した場合もNULL値とします．<br>
 *  論理型のカラムの場合は，値が 'T' なら 1 を，値が 'F' なら 0 を，それ以外の
 *  場合は NAN を返します．<br>
 *  バイナリテーブルの文字列型のカラムの場合と，アスキーテーブルの TFORMn
 *  が数値表現を示さない場合には，セルの文字列を実数値に変換した値をそのまま
 *  返します．<br>
 *  引数のインデックスは，0から始まる数字です．
 *
 * @param   row_index 行インデックス
 * @param   elem_name 要素名 (TELEMn に存在する名前)
 * @param   repetition_idx  TDIMn の2次元目のインデックス
 * @return  セルの値: 正常終了．NULL値の場合はNAN <br>
 *          NAN: 引数が不正な場合
 * @note  バイナリテーブルの文字列型のカラムでも TNULLn の設定が可能です．これ
 *        は SFITSIO の独自拡張です．
 * @attention  可変長配列はサポートされていません．
 */
double fits_table_col::dvalue( long row_index,
			     const char *elem_name, long repetition_idx ) const
{
    double v;
    if ( this->type_rec == FITS::BIT_T || 
	 this->type_rec == FITS::LOGICAL_T )
	v = this->double_value(row_index,elem_name,repetition_idx);
    else if ( this->type_rec == FITS::STRING_T ) {
	long elem_index = this->elem_index_rec.index(elem_name,0);
	if ( row_index < 0 || this->row_size_rec <= row_index ) return NAN;
	if ( elem_index < 0 || repetition_idx < 0 ) return NAN;
	tstring sval;
	this->get_string_value(row_index,elem_index,repetition_idx, &sval);
	if ( sval.length() == 0 && this->tnull_is_set_rec == false ) return NAN;
	else if ( this->tnull_is_set_rec == true &&
		  this->tany.at_cs(TNULL_IDX).strcmp(sval.cstr()) == 0 )
	    return NAN;
	else {
	    REMOVE_SPC(sval);
	    v = sval.atof();
	    if ( this->tany.at_cs(TDISP_IDX).strpbrk("FEGDI") == 0 ) {
		v = this->tzero_rec + v * this->tscal_rec;
	    }
	}
    }
    else if ( this->tnull_is_set_rec == true ) {
	long elem_index = this->elem_index_rec.index(elem_name,0);
	if ( row_index < 0 || this->row_size_rec <= row_index ) return NAN;
	if ( elem_index < 0 || repetition_idx < 0 ) return NAN;
	if ( this->type_rec == FITS::LONG_T ||
	     this->type_rec == FITS::SHORT_T ||
	     this->type_rec == FITS::BYTE_T ||
	     this->type_rec == FITS::LONGLONG_T ) {
	    long long llv = this->longlong_value(row_index,
						 elem_index,repetition_idx);
	    if ( llv == this->tnull_r_rec ) return NAN;
	    else v = this->tzero_rec + llv * this->tscal_rec;
	}
	else {
	    v = this->tzero_rec + 
      this->double_value(row_index,elem_name,repetition_idx) * this->tscal_rec;
	}
    }
    else
	v = this->tzero_rec + 
      this->double_value(row_index,elem_name,repetition_idx) * this->tscal_rec;
    return v;
}

/**
 * @brief  セルの値を実数値で取得 (高レベル)
 *
 *  セルの値に，ヘッダの TZEROn と TSCALn を反映した実数値を取得します．<br>
 *  NULL値の場合は，NANを返します．バイナリテーブルの整数型カラムやアスキー
 *  テーブルの TNULLn の値に，セルの値が一致した場合もNULL値とします．<br>
 *  論理型のカラムの場合は，値が 'T' なら 1 を，値が 'F' なら 0 を，それ以外の
 *  場合は NAN を返します．<br>
 *  バイナリテーブルの文字列型のカラムの場合と，アスキーテーブルの TFORMn
 *  が数値表現を示さない場合には，セルの文字列を実数値に変換した値をそのまま
 *  返します．<br>
 *  引数のインデックスは，0から始まる数字です．
 *
 * @param   row_index 行インデックス
 * @param   elem_index 要素インデックス (TDIMn の1次元目のインデックス)
 * @param   repetition_idx TDIMn の2次元目のインデックス
 * @return  セルの値: 正常終了．NULL値の場合はNAN <br>
 *          NAN: 引数が不正な場合
 * @note  バイナリテーブルの文字列型のカラムでも TNULLn の設定が可能です．これ
 *        は SFITSIO の独自拡張です．
 * @attention  可変長配列はサポートされていません．
 */
double fits_table_col::dvalue( long row_index,
			      long elem_index, long repetition_idx ) const
{
    double v;
    if ( this->type_rec == FITS::BIT_T || 
	 this->type_rec == FITS::LOGICAL_T )
	v = this->double_value(row_index,elem_index,repetition_idx);
    else if ( this->type_rec == FITS::STRING_T ) {
	if ( row_index < 0 || this->row_size_rec <= row_index ) return NAN;
	if ( elem_index < 0 || repetition_idx < 0 ) return NAN;
	tstring sval;
	this->get_string_value(row_index,elem_index,repetition_idx, &sval);
	if ( sval.length() == 0 && this->tnull_is_set_rec == false ) return NAN;
	else if ( this->tnull_is_set_rec == true &&
		  this->tany.at_cs(TNULL_IDX).strcmp(sval.cstr()) == 0 )
	    return NAN;
	else {
	    REMOVE_SPC(sval);
	    v = sval.atof();
	    if ( this->tany.at_cs(TDISP_IDX).strpbrk("FEGDI") == 0 ) {
		v = this->tzero_rec + v * this->tscal_rec;
	    }
	}
    }
    else if ( this->tnull_is_set_rec == true ) {
	if ( row_index < 0 || this->row_size_rec <= row_index ) return NAN;
	if ( elem_index < 0 || repetition_idx < 0 ) return NAN;
	if ( this->type_rec == FITS::LONG_T ||
	     this->type_rec == FITS::SHORT_T ||
	     this->type_rec == FITS::BYTE_T ||
	     this->type_rec == FITS::LONGLONG_T ) {
	    long long llv = this->longlong_value(row_index,
						 elem_index,repetition_idx);
	    if ( llv == this->tnull_r_rec ) return NAN;
	    else v = this->tzero_rec + llv * this->tscal_rec;
	}
	else {
	    v = this->tzero_rec + 
     this->double_value(row_index,elem_index,repetition_idx) * this->tscal_rec;
	}
    }
    else
	v = this->tzero_rec + 
     this->double_value(row_index,elem_index,repetition_idx) * this->tscal_rec;
    return v;
}

/**
 * @brief  セルの値を整数値で取得 (高レベル)
 *
 *  セルの値に，ヘッダの TZEROn と TSCALn を反映した実数値に最も近い整数値を
 *  返します．この整数値が long 型で表現できる範囲に入らない場合，INDEF_LONG
 *  を返します．<br>
 *  NULL値の場合は，INDEF_LONG を返します．バイナリテーブルの整数型カラムや
 *  アスキーテーブルの TNULLn の値に，セルの値が一致した場合もNULL値としま
 *  す．<br>
 *  論理型のカラムの場合は，値が 'T' なら 1 を，値が 'F' なら 0 を，それ以外の
 *  場合は INDEF_LONG を返します．<br>
 *  バイナリテーブルの文字列型のカラムの場合と，アスキーテーブルの当該カラムの
 *  TFORMn が数値表現を示さない場合には，セルの文字列を実数値に変換し，その値に
 *  最も近い整数値を返します．
 *  引数のインデックスは，0から始まる数字です．
 *
 * @param   row_index 行インデックス
 * @return  セルの値．NULL値の場合はINDEF_LONG <br>
 *          INDEF_LONG: 引数が不正な場合
 * @note  バイナリテーブルの文字列型のカラムでも TNULLn の設定が可能です．これ
 *        は SFITSIO の独自拡張です．
 * @attention  可変長配列はサポートされていません．
 */
long fits_table_col::lvalue( long row_index ) const
{
    double dv = this->dvalue(row_index);
    if ( isfinite(dv) && MIN_DOUBLE_ROUND_LONG <= dv && dv <= MAX_DOUBLE_ROUND_LONG )
	return round_d2l(dv);
    else return INDEF_LONG;
}

/**
 * @brief  セルの値を整数値で取得 (高レベル)
 *
 *  セルの値に，ヘッダの TZEROn と TSCALn を反映した実数値に最も近い整数値を
 *  返します．この整数値が long 型で表現できる範囲に入らない場合，INDEF_LONG
 *  を返します．<br>
 *  NULL値の場合は，INDEF_LONG を返します．バイナリテーブルの整数型カラムや
 *  アスキーテーブルの TNULLn の値に，セルの値が一致した場合もNULL値としま
 *  す．<br>
 *  論理型のカラムの場合は，値が 'T' なら 1 を，値が 'F' なら 0 を，それ以外の
 *  場合は INDEF_LONG を返します．<br>
 *  バイナリテーブルの文字列型のカラムの場合と，アスキーテーブルの当該カラムの
 *  TFORMn が数値表現を示さない場合には，セルの文字列を実数値に変換し，その値に
 *  最も近い整数値を返します．
 *  引数のインデックスは，0から始まる数字です．
 *
 * @param   row_index 行インデックス
 * @param   elem_name 要素名 (TELEMn に存在する名前)
 * @param   repetition_idx TDIMn の2次元目のインデックス
 * @return  セルの値．NULL値の場合はINDEF_LONG <br>
 *          INDEF_LONG: 引数が不正な場合
 * @note  バイナリテーブルの文字列型のカラムでも TNULLn の設定が可能です．これ
 *        は SFITSIO の独自拡張です．
 * @attention  可変長配列はサポートされていません．
 */
long fits_table_col::lvalue( long row_index,
			     const char *elem_name, long repetition_idx ) const
{
    double dv = this->dvalue(row_index,elem_name,repetition_idx);
    if ( isfinite(dv) && MIN_DOUBLE_ROUND_LONG <= dv && dv <= MAX_DOUBLE_ROUND_LONG )
	return round_d2l(dv);
    else return INDEF_LONG;
}

/**
 * @brief  セルの値を整数値で取得 (高レベル)
 *
 *  セルの値に，ヘッダの TZEROn と TSCALn を反映した実数値に最も近い整数値を
 *  返します．この整数値が long 型で表現できる範囲に入らない場合，INDEF_LONG
 *  を返します．<br>
 *  NULL値の場合は，INDEF_LONG を返します．バイナリテーブルの整数型カラムや
 *  アスキーテーブルの TNULLn の値に，セルの値が一致した場合もNULL値としま
 *  す．<br>
 *  論理型のカラムの場合は，値が 'T' なら 1 を，値が 'F' なら 0 を，それ以外の
 *  場合は INDEF_LONG を返します．<br>
 *  バイナリテーブルの文字列型のカラムの場合と，アスキーテーブルの当該カラムの
 *  TFORMn が数値表現を示さない場合には，セルの文字列を実数値に変換し，その値に
 *  最も近い整数値を返します．
 *  引数のインデックスは，0から始まる数字です．
 *
 * @param   row_index 行インデックス
 * @param   elem_index 要素インデックス (TDIMn の1次元目のインデックス)
 * @param   repetition_idx TDIMn の2次元目のインデックス
 * @return  セルの値．NULL値の場合はINDEF_LONG <br>
 *          INDEF_LONG: 引数が不正な場合
 * @note  バイナリテーブルの文字列型のカラムでも TNULLn の設定が可能です．これ
 *        は SFITSIO の独自拡張です．
 * @attention  可変長配列はサポートされていません．
 */
long fits_table_col::lvalue( long row_index,
			     long elem_index, long repetition_idx ) const
{
    double dv = this->dvalue(row_index,elem_index,repetition_idx);
    if ( isfinite(dv) && MIN_DOUBLE_ROUND_LONG <= dv && dv <= MAX_DOUBLE_ROUND_LONG )
	return round_d2l(dv);
    else return INDEF_LONG;
}

/**
 * @brief  セルの値を整数値で取得 (高レベル)
 *
 *  セルの値に，ヘッダの TZEROn と TSCALn を反映した実数値に最も近い整数値を
 *  返します．この整数値が long long 型で表現できる範囲に入らない場合，
 *  INDEF_LLONG を返します．<br>
 *  NULL値の場合は，INDEF_LLONG を返します．バイナリテーブルの整数型カラムや
 *  アスキーテーブルの TNULLn の値に，セルの値が一致した場合もNULL値としま
 *  す．<br>
 *  論理型のカラムの場合は，値が 'T' なら 1 を，値が 'F' なら 0 を，それ以外の
 *  場合は INDEF_LLONG を返します．<br>
 *  バイナリテーブルの文字列型のカラムの場合と，アスキーテーブルの当該カラムの
 *  TFORMn が数値表現を示さない場合には，セルの文字列を実数値に変換し，その値に
 *  最も近い整数値を返します．
 *  引数のインデックスは，0から始まる数字です．
 *
 * @param   row_index 行インデックス
 * @return  セルの値．NULL値の場合はINDEF_LLONG <br>
 *          INDEF_LLONG: 引数が不正な場合
 * @note  バイナリテーブルの文字列型のカラムでも TNULLn の設定が可能です．これ
 *        は SFITSIO の独自拡張です．
 * @attention  可変長配列はサポートされていません．
 */
long long fits_table_col::llvalue( long row_index ) const
{
    if ( this->type_rec == FITS::LONGLONG_T ) {
	if ( row_index < 0 || this->row_size_rec <= row_index )
	    return INDEF_LLONG;
	long long v = this->longlong_value(row_index);
	if ( this->tnull_is_set_rec == true && this->tnull_r_rec == v ) {
	    return INDEF_LLONG;
	}
	else if ( this->tscal_rec == 1.0 && this->tzero_rec == 0.0 ) {
	    return v;
	}
	else {
	    double dv = this->tzero_rec + v * this->tscal_rec;
	    if ( isfinite(dv) && MIN_DOUBLE_ROUND_LLONG <= dv && dv <= MAX_DOUBLE_ROUND_LLONG )
		return round_d2ll(dv);
	    else return INDEF_LLONG;
	}
    }
    else {
	double dv = this->dvalue(row_index);
	if ( isfinite(dv) && MIN_DOUBLE_ROUND_LLONG <= dv && dv <= MAX_DOUBLE_ROUND_LLONG )
	    return round_d2ll(dv);
	else return INDEF_LLONG;
    }
}

/**
 * @brief  セルの値を整数値で取得 (高レベル)
 *
 *  セルの値に，ヘッダの TZEROn と TSCALn を反映した実数値に最も近い整数値を
 *  返します．この整数値が long long 型で表現できる範囲に入らない場合，
 *  INDEF_LLONG を返します．<br>
 *  NULL値の場合は，INDEF_LLONG を返します．バイナリテーブルの整数型カラムや
 *  アスキーテーブルの TNULLn の値に，セルの値が一致した場合もNULL値としま
 *  す．<br>
 *  論理型のカラムの場合は，値が 'T' なら 1 を，値が 'F' なら 0 を，それ以外の
 *  場合は INDEF_LLONG を返します．<br>
 *  バイナリテーブルの文字列型のカラムの場合と，アスキーテーブルの当該カラムの
 *  TFORMn が数値表現を示さない場合には，セルの文字列を実数値に変換し，その値に
 *  最も近い整数値を返します．
 *  引数のインデックスは，0から始まる数字です．
 *
 * @param   row_index 行インデックス
 * @param   elem_name 要素名 (TELEMn に存在する名前)
 * @param   repetition_idx TDIMn の2次元目のインデックス
 * @return  セルの値．NULL値の場合はINDEF_LLONG <br>
 *          INDEF_LLONG: 引数が不正な場合
 * @note  バイナリテーブルの文字列型のカラムでも TNULLn の設定が可能です．これ
 *        は SFITSIO の独自拡張です．
 * @attention  可変長配列はサポートされていません．
 */
long long fits_table_col::llvalue( long row_index,
			     const char *elem_name, long repetition_idx ) const
{
    if ( this->type_rec == FITS::LONGLONG_T ) {
	long elem_index = this->elem_index_rec.index(elem_name,0);
	if ( row_index < 0 || this->row_size_rec <= row_index )
	    return INDEF_LLONG;
	if ( elem_index < 0 || repetition_idx < 0 ) return INDEF_LLONG;
	long long v = this->longlong_value(row_index,elem_index,repetition_idx);
	if ( this->tnull_is_set_rec == true && this->tnull_r_rec == v ) {
	    return INDEF_LLONG;
	}
	else if ( this->tscal_rec == 1.0 && this->tzero_rec == 0.0 ) {
	    return v;
	}
	else {
	    double dv = this->tzero_rec + v * this->tscal_rec;
	    if ( isfinite(dv) && MIN_DOUBLE_ROUND_LLONG <= dv && dv <= MAX_DOUBLE_ROUND_LLONG )
		return round_d2ll(dv);
	    else return INDEF_LLONG;
	}
    }
    else {
	double dv = this->dvalue(row_index,elem_name,repetition_idx);
	if ( isfinite(dv) && MIN_DOUBLE_ROUND_LLONG <= dv && dv <= MAX_DOUBLE_ROUND_LLONG )
	    return round_d2ll(dv);
	else return INDEF_LLONG;
    }
}

/**
 * @brief  セルの値を整数値で取得 (高レベル)
 *
 *  セルの値に，ヘッダの TZEROn と TSCALn を反映した実数値に最も近い整数値を
 *  返します．この整数値が long long 型で表現できる範囲に入らない場合，
 *  INDEF_LLONG を返します．<br>
 *  NULL値の場合は，INDEF_LLONG を返します．バイナリテーブルの整数型カラムや
 *  アスキーテーブルの TNULLn の値に，セルの値が一致した場合もNULL値としま
 *  す．<br>
 *  論理型のカラムの場合は，値が 'T' なら 1 を，値が 'F' なら 0 を，それ以外の
 *  場合は INDEF_LLONG を返します．<br>
 *  バイナリテーブルの文字列型のカラムの場合と，アスキーテーブルの当該カラムの
 *  TFORMn が数値表現を示さない場合には，セルの文字列を実数値に変換し，その値に
 *  最も近い整数値を返します．
 *  引数のインデックスは，0から始まる数字です．
 *
 * @param   row_index 行インデックス
 * @param   elem_index 要素インデックス (TDIMn の1次元目のインデックス)
 * @param   repetition_idx TDIMn の2次元目のインデックス
 * @return  セルの値．NULL値の場合はINDEF_LLONG <br>
 *          INDEF_LLONG: 引数が不正な場合
 * @note  バイナリテーブルの文字列型のカラムでも TNULLn の設定が可能です．これ
 *        は SFITSIO の独自拡張です．
 * @attention  可変長配列はサポートされていません．
 */
long long fits_table_col::llvalue( long row_index,
				   long elem_index, long repetition_idx ) const
{
    if ( this->type_rec == FITS::LONGLONG_T ) {
	if ( row_index < 0 || this->row_size_rec <= row_index )
	    return INDEF_LLONG;
	if ( elem_index < 0 || repetition_idx < 0 ) return INDEF_LLONG;
	long long v = this->longlong_value(row_index,elem_index,repetition_idx);
	if ( this->tnull_is_set_rec == true && this->tnull_r_rec == v ) {
	    return INDEF_LLONG;
	}
	else if ( this->tscal_rec == 1.0 && this->tzero_rec == 0.0 ) {
	    return v;
	}
	else {
	    double dv = this->tzero_rec + v * this->tscal_rec;
	    if ( isfinite(dv) && MIN_DOUBLE_ROUND_LLONG <= dv && dv <= MAX_DOUBLE_ROUND_LLONG )
		return round_d2ll(dv);
	    else return INDEF_LLONG;
	}
    }
    else {
	double dv = this->dvalue(row_index,elem_index,repetition_idx);
	if ( isfinite(dv) && MIN_DOUBLE_ROUND_LLONG <= dv && dv <= MAX_DOUBLE_ROUND_LLONG )
	    return round_d2ll(dv);
	else return INDEF_LLONG;
    }
}

/**
 * @brief  セルの値を論理値で取得 (高レベル)
 *
 *  セルの値を論理値で返します．返り値は，true か false で，後者は NULL値の場合
 *  を含みます．バイナリテーブルの整数型カラムやアスキーテーブルの TNULLn の値
 *  に，セルの値が一致した場合もNULL値とします．<br>
 *  'T'，'F'，'\0' の3種類の値を必要とする場合は，
 *  fits_table_col::logical_value() メンバ関数を使ってください．<br>
 *  論理型のカラムの場合，値が 'T' なら true を，そうでないなら false を返し
 *  ます．<br>
 *  整数型や実数型のカラムの場合，セルの値に，ヘッダの TZEROn と TSCAL n を
 *  反映した実数値に最も近い整数値が 0 なら false を，0 でないなら true を
 *  返します．<br>
 *  バイナリテーブルの文字列型のカラムの場合と，アスキーテーブルの TFORMn が
 *  数値表現を示さない場合には，セルの文字列の実数値への変換を試行し，変換可能
 *  な場合はその実数値に最も近い整数値が 0 なら false を，0 でないなら true を
 *  返します．変換不可能な場合は，先頭文字(空白は除く)が 'T' または 't' の場合
 *  には true を，そうでないなら false を返します．<br>
 *  引数のインデックスは，0から始まる数字です．
 *
 * @param   row_index 行インデックス
 * @return  セルの値．NULL値の場合はfalse <br>
 *          false: 引数が不正な場合
 * @note  バイナリテーブルの文字列型のカラムでも TNULLn の設定が可能です．これ
 *        は SFITSIO の独自拡張です．
 * @attention  可変長配列はサポートされていません．
 */
bool fits_table_col::bvalue( long row_index ) const
{
    if ( this->type_rec == FITS::LOGICAL_T ||
	 this->type_rec == FITS::BIT_T )
	return (this->logical_value(row_index) == 'T') ? true : false;
    else if ( this->type_rec == FITS::STRING_T ) {
	if ( row_index < 0 || this->row_size_rec <= row_index ) return false;
	tstring sval;
	double dval;
	size_t epos;
	this->get_string_value(row_index, &sval);
	if (sval.length() == 0 && this->tnull_is_set_rec == false) return false;
	else if ( this->tnull_is_set_rec == true &&
		  this->tany.at_cs(TNULL_IDX).strcmp(sval.cstr()) == 0 ) {
	    return false;
	}
	REMOVE_SPC(sval);
	dval = sval.strtod(&epos);
	if ( epos == 0 ) {
	    if ( sval.cchr(0) == 'T' || sval.cchr(0) == 't' ) return true;
	    else return false;
	}
	else {
	    if ( !isfinite(dval) ) return false;
	    if ( this->tany.at_cs(TDISP_IDX).strpbrk("FEGDI") == 0 ) {
		dval = this->tzero_rec + dval * this->tscal_rec;
	    }
	    return (round(dval) != 0.0) ? true : false;
	}
    }
    else {
	double v = this->dvalue(row_index);
	if ( !isfinite(v) ) return false;
	else if ( round(v) != 0.0 ) return true;
	else return false;
    }
}

/**
 * @brief  セルの値を論理値で取得 (高レベル)
 *
 *  セルの値を論理値で返します．返り値は，true か false で，後者は NULL値の場合
 *  を含みます．バイナリテーブルの整数型カラムやアスキーテーブルの TNULLn の値
 *  に，セルの値が一致した場合もNULL値とします．<br>
 *  'T'，'F'，'\0' の3種類の値を必要とする場合は，
 *  fits_table_col::logical_value() メンバ関数を使ってください．<br>
 *  論理型のカラムの場合，値が 'T' なら true を，そうでないなら false を返し
 *  ます．<br>
 *  整数型や実数型のカラムの場合，セルの値に，ヘッダの TZEROn と TSCAL n を
 *  反映した実数値に最も近い整数値が 0 なら false を，0 でないなら true を
 *  返します．<br>
 *  バイナリテーブルの文字列型のカラムの場合と，アスキーテーブルの TFORMn が
 *  数値表現を示さない場合には，セルの文字列の実数値への変換を試行し，変換可能
 *  な場合はその実数値に最も近い整数値が 0 なら false を，0 でないなら true を
 *  返します．変換不可能な場合は，先頭文字(空白は除く)が 'T' または 't' の場合
 *  には true を，そうでないなら false を返します．<br>
 *  引数のインデックスは，0から始まる数字です．
 *
 * @param   row_index 行インデックス
 * @param   elem_name 要素名 (TELEMn に存在する名前)
 * @param   repetition_idx TDIMn の2次元目のインデックス
 * @return  セルの値．NULL値の場合はfalse <br>
 *          false: 引数が不正な場合
 * @note  バイナリテーブルの文字列型のカラムでも TNULLn の設定が可能です．これ
 *        は SFITSIO の独自拡張です．
 * @attention  可変長配列はサポートされていません．
 */
bool fits_table_col::bvalue( long row_index,
			     const char *elem_name, long repetition_idx ) const
{
    if ( this->type_rec == FITS::LOGICAL_T ||
	 this->type_rec == FITS::BIT_T )
	return (this->logical_value(row_index,elem_name,repetition_idx) == 'T') ? true : false;
    else if ( this->type_rec == FITS::STRING_T ) {
	long elem_index = this->elem_index_rec.index(elem_name,0);
	if ( row_index < 0 || this->row_size_rec <= row_index ) return false;
	if ( elem_index < 0 || repetition_idx < 0 ) return false;
	tstring sval;
	double dval;
	size_t epos;
	this->get_string_value(row_index,elem_index,repetition_idx, &sval);
	if (sval.length() == 0 && this->tnull_is_set_rec == false) return false;
	else if ( this->tnull_is_set_rec == true &&
		  this->tany.at_cs(TNULL_IDX).strcmp(sval.cstr()) == 0 ) {
	    return false;
	}
	REMOVE_SPC(sval);
	dval = sval.strtod(&epos);
	if ( epos == 0 ) {
	    if ( sval.cchr(0) == 'T' || sval.cchr(0) == 't' ) return true;
	    else return false;
	}
	else {
	    if ( !isfinite(dval) ) return false;
	    if ( this->tany.at_cs(TDISP_IDX).strpbrk("FEGDI") == 0 ) {
		dval = this->tzero_rec + dval * this->tscal_rec;
	    }
	    return (round(dval) != 0.0) ? true : false;
	}
    }
    else {
	double v = this->dvalue(row_index,elem_name,repetition_idx);
	if ( !isfinite(v) ) return false;
	else if ( round(v) != 0.0 ) return true;
	else return false;
    }
}

/**
 * @brief  セルの値を論理値で取得 (高レベル)
 *
 *  セルの値を論理値で返します．返り値は，true か false で，後者は NULL値の場合
 *  を含みます．バイナリテーブルの整数型カラムやアスキーテーブルの TNULLn の値
 *  に，セルの値が一致した場合もNULL値とします．<br>
 *  'T'，'F'，'\0' の3種類の値を必要とする場合は，
 *  fits_table_col::logical_value() メンバ関数を使ってください．<br>
 *  論理型のカラムの場合，値が 'T' なら true を，そうでないなら false を返し
 *  ます．<br>
 *  整数型や実数型のカラムの場合，セルの値に，ヘッダの TZEROn と TSCAL n を
 *  反映した実数値に最も近い整数値が 0 なら false を，0 でないなら true を
 *  返します．<br>
 *  バイナリテーブルの文字列型のカラムの場合と，アスキーテーブルの TFORMn が
 *  数値表現を示さない場合には，セルの文字列の実数値への変換を試行し，変換可能
 *  な場合はその実数値に最も近い整数値が 0 なら false を，0 でないなら true を
 *  返します．変換不可能な場合は，先頭文字(空白は除く)が 'T' または 't' の場合
 *  には true を，そうでないなら false を返します．<br>
 *  引数のインデックスは，0から始まる数字です．
 *
 * @param   row_index 行インデックス
 * @param   elem_index 要素インデックス (TDIMn の1次元目のインデックス)
 * @param   repetition_idx TDIMn の2次元目のインデックス
 * @return  セルの値．NULL値の場合はfalse <br>
 *          false: 引数が不正な場合
 * @note  バイナリテーブルの文字列型のカラムでも TNULLn の設定が可能です．これ
 *        は SFITSIO の独自拡張です．
 * @attention  可変長配列はサポートされていません．
 */
bool fits_table_col::bvalue( long row_index,
			     long elem_index, long repetition_idx ) const
{
    if ( this->type_rec == FITS::LOGICAL_T ||
	 this->type_rec == FITS::BIT_T )
	return (this->logical_value(row_index,elem_index,repetition_idx) == 'T') ? true : false;
    else if ( this->type_rec == FITS::STRING_T ) {
	if ( row_index < 0 || this->row_size_rec <= row_index ) return false;
	if ( elem_index < 0 || repetition_idx < 0 ) return false;
	tstring sval;
	double dval;
	size_t epos;
	this->get_string_value(row_index,elem_index,repetition_idx, &sval);
	if (sval.length() == 0 && this->tnull_is_set_rec == false) return false;
	else if ( this->tnull_is_set_rec == true &&
		  this->tany.at_cs(TNULL_IDX).strcmp(sval.cstr()) == 0 ) {
	    return false;
	}
	REMOVE_SPC(sval);
	dval = sval.strtod(&epos);
	if ( epos == 0 ) {
	    if ( sval.cchr(0) == 'T' || sval.cchr(0) == 't' ) return true;
	    else return false;
	}
	else {
	    if ( !isfinite(dval) ) return false;
	    if ( this->tany.at_cs(TDISP_IDX).strpbrk("FEGDI") == 0 ) {
		dval = this->tzero_rec + dval * this->tscal_rec;
	    }
	    return (round(dval) != 0.0) ? true : false;
	}
    }
    else {
	double v = this->dvalue(row_index,elem_index,repetition_idx);
	if ( !isfinite(v) ) return false;
	else if ( round(v) != 0.0 ) return true;
	else return false;
    }
}


#define DO_LL(ll_v) { \
	    if ( this->tany.cchr(TDISP_IDX,0) == 'B' ) {	/* Binary */ \
		const char *bit_str_p[8]; \
		size_t p_cnt = 0; \
		uint64_t ll_one, ll_tmp = (uint64_t)ll_v; \
		while ( ll_tmp != (uint64_t)0 ) { \
		    ll_one = (ll_tmp & (uint64_t)0x0ff); \
		    bit_str_p[p_cnt] = Bit_str + (9 * ll_one);	\
		    ll_tmp >>= 8; \
		    p_cnt ++; \
		} \
		this->tmp_str_buf->assign("0b"); \
		while ( 0 < p_cnt ) { \
		    p_cnt--; \
		    this->tmp_str_buf->append(bit_str_p[p_cnt]); \
		} \
		p_cnt = this->tmp_str_buf->strspn(2,'0'); \
		this->tmp_str_buf->put(0 + p_cnt, "0b"); \
		this->str_buf->printf(this->fmt_str.cstr(),this->tmp_str_buf->cstr() + p_cnt); \
	    } \
	    else { \
		this->str_buf->printf(this->fmt_str.cstr(),ll_v); \
	    } \
}

#define DO_LL_TS(ll_v,ts) { \
	    if ( this->tany.cchr(TDISP_IDX,0) == 'B' ) {	/* Binary */ \
		const char *bit_str_p[8]; \
		size_t p_cnt = 0; \
		uint64_t ll_one, ll_tmp = (uint64_t)ll_v; \
		while ( ll_tmp != (uint64_t)0 ) { \
		    ll_one = (ll_tmp & (uint64_t)0x0ff); \
		    bit_str_p[p_cnt] = Bit_str + (9 * ll_one);	\
		    ll_tmp >>= 8; \
		    p_cnt ++; \
		} \
		this->tmp_str_buf->assign("0b"); \
		while ( 0 < p_cnt ) { \
		    p_cnt--; \
		    this->tmp_str_buf->append(bit_str_p[p_cnt]); \
		} \
		p_cnt = this->tmp_str_buf->strspn(2,'0'); \
		this->tmp_str_buf->put(0 + p_cnt, "0b"); \
		ts.printf(this->fmt_str.cstr(),this->tmp_str_buf->cstr() + p_cnt); \
	    } \
	    else { \
		ts.printf(this->fmt_str.cstr(),ll_v); \
	    } \
}

#define DO_IBOZFEGD(val) { \
	if ( this->tany.cchr(TDISP_IDX,0) == 'I' || \
	     this->tany.cchr(TDISP_IDX,0) == 'B' || \
	     this->tany.cchr(TDISP_IDX,0) == 'Z' || \
	     this->tany.cchr(TDISP_IDX,0) == 'O' ) { \
	    double dv = this->tzero_rec + val * this->tscal_rec; \
	    if ( isfinite(dv) && MIN_DOUBLE_ROUND_LLONG <= dv && dv <= MAX_DOUBLE_ROUND_LLONG ) { \
	        long long ll_v = round_d2ll(dv); \
	        DO_LL(ll_v); \
	    } \
	    else { \
		goto ret_null; \
	    } \
	} \
	else {	/* 'F','E','G' and 'D' */ \
	    double v = this->tzero_rec + val * this->tscal_rec; \
	    if ( isfinite(v) ) { \
		this->str_buf->printf(this->fmt_str.cstr(),v); \
		if ( this->tany.cchr(TDISP_IDX,0) == 'D' ) { \
		    ssize_t ep = this->str_buf->strchr('E'); \
		    if ( 0 <= ep ) this->str_buf->at(ep) = 'D'; \
		} \
	    } \
	    else { \
		goto ret_null; \
	    } \
	} \
}

#define DO_IBOZFEGD_TS(val,ts) { \
	if ( this->tany.cchr(TDISP_IDX,0) == 'I' || \
	     this->tany.cchr(TDISP_IDX,0) == 'B' || \
	     this->tany.cchr(TDISP_IDX,0) == 'Z' || \
	     this->tany.cchr(TDISP_IDX,0) == 'O' ) { \
	    double dv = this->tzero_rec + val * this->tscal_rec; \
	    if ( isfinite(dv) && MIN_DOUBLE_ROUND_LLONG <= dv && dv <= MAX_DOUBLE_ROUND_LLONG ) { \
		long long ll_v = round_d2ll(dv); \
		DO_LL_TS(ll_v,ts); \
	    } \
	    else { \
		goto ret_null; \
	    } \
	} \
	else {	/* 'F','E','G' and 'D' */ \
	    double v = this->tzero_rec + val * this->tscal_rec; \
	    if ( isfinite(v) ) { \
		ts.printf(this->fmt_str.cstr(),v); \
		if ( this->tany.cchr(TDISP_IDX,0) == 'D' ) { \
		    ssize_t ep = ts.strchr('E'); \
		    if ( 0 <= ep ) ts.at(ep) = 'D'; \
		} \
	    } \
	    else { \
		goto ret_null; \
	    } \
	} \
}


/* string */
/**
 * @brief  セルの値を文字列値で取得 (高レベル)
 *
 *  FITS規約に基づく各種変換処理を行なったセル値を，文字列値で返します．<br>
 *  NULL値の場合，fits_table_col::assign_null_svalue() で指定された NULL 文字列
 *  が返されます(NULL文字列の初期設定値は "NULL" です)．バイナリテーブルの整
 *  数型カラムやアスキーテーブルの TNULLn の値に，セルの値が一致した場合も
 *  NULL値とします．<br>
 *  ・バイナリテーブルの場合 <br>
 *  整数型や実数型のカラムの場合，セルの値をヘッダの TZEROn と TSCALn の値で
 *  変換し，さらに文字列に変換した値を返します．<br>
 *  TDISPn の指定がある場合は，TDISPn の指定に従ってフォーマットされた文字列を
 *  返します．<br>
 *  TDISPn の指定が無い場合，実数型のカラムと TZEROn，TSCALn が設定されている
 *  整数型のカラムでは，以下のprintf()関数のフォーマットに従って変換した文字列
 *  を返します．<br>
 *  FITS::FLOAT_T                    ... "%.7G" <br>
 *  FITS::DOUBLE_T, FITS::LONGLONG_T ... "%.15G" <br>
 *  FITS::LONG_T                     ... "%.10G" <br>
 *  上記以外のカラムタイプ           ... "%.7G" <br>
 *  ・アスキーテーブルの場合 <br>
 *  TFORMn が数値表現を示さない場合には，セルの文字列を TFORMn でフォーマット
 *  した文字列を返します(TFORMn の指定が無い場合は，セルの文字列をそのまま返し
 *  ます)．<br>
 *  TFORMn が数値表現を示す場合には，セルの文字列を実数値に変換し，その値を
 *  TZEROn と TSCALn で変換し，その値を TFORMn でフォーマットした文字列を返し
 *  ます．<br>
 *
 * @param   row_index 行インデックス
 * @return  セルの値: 正常終了．NULL値の場合は NULL文字列 <br>
 *          NULL文字列: 引数が不正な場合
 * @note  バイナリテーブルの文字列型のカラムでも TNULLn の設定が可能です．これ
 *        は SFITSIO の独自拡張です．
 * @note  SFITSIO では，カラム型が FITS::ASCII_T (TFORMn の指定に 'A' を含む) 
 *        の場合，TNULLn の設定がなければ，先頭文字が '\0' のセルは NULL値とし
 *        ます．これを「文字列の低レベルなNULL値」と呼んでいます．
 * @attention  可変長配列はサポートされていません．
 */
const char *fits_table_col::svalue( long row_index )
{
    if ( row_index < 0 || this->row_size_rec <= row_index ) goto ret_null;

    if ( this->type_rec == FITS::STRING_T ) {
	const char *d_ptr = (const char *)this->data_ptr();
	if ( d_ptr == NULL ) goto ret_null;
	this->tmp_str_buf->assign(d_ptr +
		this->bytes_rec * this->elem_size_rec * row_index,
		this->bytes_rec);
	/* */
	if ( (this->tmp_str_buf->cchr(0) == '\0' && this->tnull_is_set_rec == false) ||
	     (this->tnull_is_set_rec == true &&
	      this->tany.at_cs(TNULL_IDX).strcmp(this->tmp_str_buf->cstr()) == 0) ) {
	    goto ret_null;
	}
	else if ( 0 < this->fmt_str.length() ) {
	    if ( this->tany.cchr(TDISP_IDX,0) == 'A' ) {
		this->str_buf->printf(this->fmt_str.cstr(),
				      this->tmp_str_buf->cstr());
	    }
	    else if ( this->tany.at_cs(TDISP_IDX).strpbrk("FEGD") == 0 ) {
		double v;
		tstring sv;
		sv.assign(this->tmp_str_buf->cstr());
		REMOVE_SPC(sv);
		v = this->tzero_rec + sv.atof() * this->tscal_rec;
		if ( isfinite(v) ) {
		    this->str_buf->printf(this->fmt_str.cstr(),v);
		    if ( this->tany.cchr(TDISP_IDX,0) == 'D' ) {
			ssize_t ep = this->str_buf->strchr('E');
			if ( 0 <= ep ) this->str_buf->at(ep) = 'D';
		    }
		}
		else {
		    goto ret_null;
		}
	    }
	    else if ( this->tany.at_cs(TDISP_IDX).strpbrk("I") == 0 ) {
		double v;
		tstring sv;
		sv.assign(this->tmp_str_buf->cstr());
		REMOVE_SPC(sv);
		v = this->tzero_rec + sv.atof() * this->tscal_rec;
		if ( isfinite(v) && MIN_DOUBLE_ROUND_LLONG <= v && v <= MAX_DOUBLE_ROUND_LLONG ) {
		    this->str_buf->printf(this->fmt_str.cstr(),round_d2ll(v));
		}
		else {
		    goto ret_null;
		}
	    }
	    else if ( this->tany.cchr(TDISP_IDX,0) == 'L' ) {
		this->str_buf->printf(this->fmt_str.cstr(),
				      this->tmp_str_buf->cstr());
	    }
	    else {
		this->str_buf->assign(this->tmp_str_buf->cstr());
	    }
	}
	else {
	    this->str_buf->assign(this->tmp_str_buf->cstr());
	}
	return this->str_buf->cstr();
    }
    else if ( this->type_rec == FITS::LONG_T ) {
	long val = this->long_value(row_index);
	if ( this->tnull_is_set_rec == true && val == this->tnull_r_rec ) {
	    goto ret_null;
	}
	else if ( 0 < this->fmt_str.length() ) {
	    DO_IBOZFEGD(val);
	}
	else {
	    if ( this->tscal_rec == 1.0 && this->tzero_rec == 0.0 ) {
		this->str_buf->printf("%ld",val);
	    }
	    else {
		double v = this->tzero_rec + val * this->tscal_rec;
		if ( isfinite(v) ) this->str_buf->printf("%.10G",v);
		else goto ret_null;
	    }
	}
	return this->str_buf->cstr();
    }
    else if ( this->type_rec == FITS::LONGLONG_T ) {
	long long val = this->longlong_value(row_index);
	if ( this->tnull_is_set_rec == true && val == this->tnull_r_rec ) {
	    goto ret_null;
	}
	else if ( 0 < this->fmt_str.length() ) {
	    DO_IBOZFEGD(val);
	}
	else {
	    if ( this->tscal_rec == 1.0 && this->tzero_rec == 0.0 ) {
		this->str_buf->printf("%lld",val);
	    }
	    else {
		double v = this->tzero_rec + val * this->tscal_rec;
		if ( isfinite(v) ) this->str_buf->printf("%.15G",v);
		else goto ret_null;
	    }
	}
	return this->str_buf->cstr();
    }
    else if ( this->type_rec == FITS::SHORT_T ) {
	short val = this->short_value(row_index);
	if ( this->tnull_is_set_rec == true && val == this->tnull_r_rec ) {
	    goto ret_null;
	}
	else if ( 0 < this->fmt_str.length() ) {
	    DO_IBOZFEGD(val);
	}
	else {
	    if ( this->tscal_rec == 1.0 && this->tzero_rec == 0.0 ) {
		this->str_buf->printf("%hd",val);
	    }
	    else {
		double v = this->tzero_rec + val * this->tscal_rec;
		if ( isfinite(v) ) this->str_buf->printf("%.7G",v);
		else goto ret_null;
	    }
	}
	return this->str_buf->cstr();
    }
    else if ( this->type_rec == FITS::DOUBLE_T ) {
	double val = this->double_value(row_index);
	if ( !isfinite(val) ) {
	    goto ret_null;
	}
	else if ( 0 < this->fmt_str.length() ) {
	    DO_IBOZFEGD(val);
	}
	else {
	    double v = this->tzero_rec + val * this->tscal_rec;
	    if ( isfinite(v) ) this->str_buf->printf("%.15G",v);
	    else goto ret_null;
	}
	return this->str_buf->cstr();
    }
    else if ( this->type_rec == FITS::FLOAT_T ) {
	float val = this->float_value(row_index);
	if ( !isfinite(val) ) {
	    goto ret_null;
	}
	else if ( 0 < this->fmt_str.length() ) {
	    DO_IBOZFEGD(val);
	}
	else {
	    if ( this->tscal_rec == 1.0 && this->tzero_rec == 0.0 ) {
		this->str_buf->printf("%.7G",(double)val);
	    }
	    else {
		double v = this->tzero_rec + val * this->tscal_rec;
		if ( isfinite(v) ) this->str_buf->printf("%.7G",v);
		else goto ret_null;
	    }
	}
	return this->str_buf->cstr();
    }
    else if ( this->type_rec == FITS::BYTE_T ) {
	unsigned char val = this->byte_value(row_index);
	if ( this->tnull_is_set_rec == true && val == this->tnull_r_rec ) {
	    goto ret_null;
	}
	else if ( 0 < this->fmt_str.length() ) {
	    DO_IBOZFEGD(val);
	}
	else {
	    if ( this->tscal_rec == 1.0 && this->tzero_rec == 0.0 ) {
		this->str_buf->printf("%hhu",val);
	    }
	    else {
		double v = this->tzero_rec + val * this->tscal_rec;
		if ( isfinite(v) ) this->str_buf->printf("%.7G",v);
		else goto ret_null;
	    }
	}
	return this->str_buf->cstr();
    }
    else if ( this->type_rec == FITS::LOGICAL_T ) {
	int v = this->logical_value(row_index);
	if ( 0 < this->fmt_str.length() ) {
	    if ( v == 'T' ) {
		this->str_buf->printf(this->fmt_str.cstr(),"T");
	    }
	    else if ( v == 'F' ) {
		this->str_buf->printf(this->fmt_str.cstr(),"F");
	    }
	    else {
		this->str_buf->printf(this->fmt_nullstr.cstr(),
				      this->null_svalue());
	    }
	}
	else {
	    if (v == 'T') this->str_buf->assign("T");
	    else if (v == 'F') this->str_buf->assign("F");
	    else this->str_buf->assign(this->null_svalue());
	}
	return this->str_buf->cstr();
    }
    else if ( this->type_rec == FITS::BIT_T ) {
	long long v = this->bit_value(row_index);
	if ( 0 < this->fmt_str.length() ) {
	    long long ll_v=v; DO_LL(ll_v);
	}
	else {
	    this->str_buf->printf("%lld",v);
	}
	return this->str_buf->cstr();
    }
 ret_null:
    if ( 0 < this->fmt_nullstr.length() ) 
	this->str_buf->printf(this->fmt_nullstr.cstr(), this->null_svalue());
    else
	this->str_buf->assign(this->null_svalue());
    return this->str_buf->cstr();
}

/**
 * @brief  セルの値を文字列値で取得 (高レベル)
 *
 *  FITS規約に基づく各種変換処理を行なったセル値を，文字列値で返します．<br>
 *  NULL値の場合，fits_table_col::assign_null_svalue() で指定された NULL 文字列
 *  が返されます(NULL文字列の初期設定値は "NULL" です)．バイナリテーブルの整
 *  数型カラムやアスキーテーブルの TNULLn の値に，セルの値が一致した場合も
 *  NULL値とします．<br>
 *  ・バイナリテーブルの場合 <br>
 *  整数型や実数型のカラムの場合，セルの値をヘッダの TZEROn と TSCALn の値で
 *  変換し，さらに文字列に変換した値を返します．<br>
 *  TDISPn の指定がある場合は，TDISPn の指定に従ってフォーマットされた文字列を
 *  返します．<br>
 *  TDISPn の指定が無い場合，実数型のカラムと TZEROn，TSCALn が設定されている
 *  整数型のカラムでは，以下のprintf()関数のフォーマットに従って変換した文字列
 *  を返します．<br>
 *  FITS::FLOAT_T                    ... "%.7G" <br>
 *  FITS::DOUBLE_T, FITS::LONGLONG_T ... "%.15G" <br>
 *  FITS::LONG_T                     ... "%.10G" <br>
 *  上記以外のカラムタイプ           ... "%.7G" <br>
 *  ・アスキーテーブルの場合 <br>
 *  TFORMn が数値表現を示さない場合には，セルの文字列を TFORMn でフォーマット
 *  した文字列を返します(TFORMn の指定が無い場合は，セルの文字列をそのまま返し
 *  ます)．<br>
 *  TFORMn が数値表現を示す場合には，セルの文字列を実数値に変換し，その値を
 *  TZEROn と TSCALn で変換し，その値を TFORMn でフォーマットした文字列を返し
 *  ます．<br>
 *
 * @param   row_index 行インデックス
 * @param   elem_name 要素名 (TELEMn に存在する名前)
 * @param   repetition_idx TDIMn の2次元目のインデックス
 * @return  セルの値: 正常終了．NULL値の場合は NULL文字列 <br>
 *          NULL文字列: 引数が不正な場合
 * @note  バイナリテーブルの文字列型のカラムでも TNULLn の設定が可能です．これ
 *        は SFITSIO の独自拡張です．
 * @note  SFITSIO では，カラム型が FITS::ASCII_T (TFORMn の指定に 'A' を含む) 
 *        の場合，TNULLn の設定がなければ，先頭文字が '\0' のセルは NULL値とし
 *        ます．これを「文字列の低レベルなNULL値」と呼んでいます．
 * @attention  可変長配列はサポートされていません．
 */
const char *fits_table_col::svalue( long row_index,
				    const char *elem_name, 
				    long repetition_idx )
{
    long elem_index = this->elem_index_rec.index(elem_name,0);
    if ( this->type_rec == FITS::BIT_T ) {
	long long v;
	if ( row_index < 0 || this->row_size_rec <= row_index ) goto ret_null;
	if ( elem_index < 0 || repetition_idx < 0 ) goto ret_null;
	v = this->bit_value( row_index, elem_index, repetition_idx, 0 );
	if ( 0 < this->fmt_str.length() ) {
	    long long ll_v=v;
	    DO_LL(ll_v);
	}
	else {
	    this->str_buf->printf("%lld",v);
	}
	return this->str_buf->cstr();
    }
    else {
	return this->svalue( row_index, elem_index, repetition_idx );
    }
 ret_null:
    if ( 0 < this->fmt_nullstr.length() ) 
	this->str_buf->printf(this->fmt_nullstr.cstr(), this->null_svalue());
    else
	this->str_buf->assign(this->null_svalue());
    return this->str_buf->cstr();
}

/**
 * @brief  セルの値を文字列値で取得 (高レベル)
 *
 *  FITS規約に基づく各種変換処理を行なったセル値を，文字列値で返します．<br>
 *  NULL値の場合，fits_table_col::assign_null_svalue() で指定された NULL 文字列
 *  が返されます(NULL文字列の初期設定値は "NULL" です)．バイナリテーブルの整
 *  数型カラムやアスキーテーブルの TNULLn の値に，セルの値が一致した場合も
 *  NULL値とします．<br>
 *  ・バイナリテーブルの場合 <br>
 *  整数型や実数型のカラムの場合，セルの値をヘッダの TZEROn と TSCALn の値で
 *  変換し，さらに文字列に変換した値を返します．<br>
 *  TDISPn の指定がある場合は，TDISPn の指定に従ってフォーマットされた文字列を
 *  返します．<br>
 *  TDISPn の指定が無い場合，実数型のカラムと TZEROn，TSCALn が設定されている
 *  整数型のカラムでは，以下のprintf()関数のフォーマットに従って変換した文字列
 *  を返します．<br>
 *  FITS::FLOAT_T                    ... "%.7G" <br>
 *  FITS::DOUBLE_T, FITS::LONGLONG_T ... "%.15G" <br>
 *  FITS::LONG_T                     ... "%.10G" <br>
 *  上記以外のカラムタイプ           ... "%.7G" <br>
 *  ・アスキーテーブルの場合 <br>
 *  TFORMn が数値表現を示さない場合には，セルの文字列を TFORMn でフォーマット
 *  した文字列を返します(TFORMn の指定が無い場合は，セルの文字列をそのまま返し
 *  ます)．<br>
 *  TFORMn が数値表現を示す場合には，セルの文字列を実数値に変換し，その値を
 *  TZEROn と TSCALn で変換し，その値を TFORMn でフォーマットした文字列を返し
 *  ます．<br>
 *
 * @param   row_index 行インデックス
 * @param   elem_index 要素インデックス (TDIMn の1次元目のインデックス)
 * @param   repetition_idx TDIMn の2次元目のインデックス
 * @return  セルの値: 正常終了．NULL値の場合は NULL文字列 <br>
 *          NULL文字列: 引数が不正な場合
 * @note  バイナリテーブルの文字列型のカラムでも TNULLn の設定が可能です．これ
 *        は SFITSIO の独自拡張です．
 * @note  SFITSIO では，カラム型が FITS::ASCII_T (TFORMn の指定に 'A' を含む) 
 *        の場合，TNULLn の設定がなければ，先頭文字が '\0' のセルは NULL値とし
 *        ます．これを「文字列の低レベルなNULL値」と呼んでいます．
 * @attention  可変長配列はサポートされていません．
 */
const char *fits_table_col::svalue( long row_index,
				    long elem_index, long repetition_idx )
{
    if ( row_index < 0 || this->row_size_rec <= row_index ) goto ret_null;
    if ( elem_index < 0 || repetition_idx < 0 ) goto ret_null;

    if ( this->type_rec == FITS::STRING_T ) {
	const char *d_ptr = (const char *)this->data_ptr();
	long e_idx;
	if ( d_ptr == NULL ) goto ret_null;
	e_idx = this->dcol_size_rec * repetition_idx 
	    + elem_index;
	if ( this->elem_size_rec <= e_idx ) 
	    e_idx = this->elem_size_rec - this->dcol_size_rec 
		+ elem_index % this->dcol_size_rec;
	this->tmp_str_buf->assign(d_ptr + 
	      this->bytes_rec * (this->elem_size_rec * row_index + e_idx),
	      this->bytes_rec);
	/* */
	if ( (this->tmp_str_buf->cchr(0) == '\0' && this->tnull_is_set_rec == false) ||
	     (this->tnull_is_set_rec == true &&
	      this->tany.at_cs(TNULL_IDX).strcmp(this->tmp_str_buf->cstr()) == 0) ) {
	    goto ret_null;
	}
	else if ( 0 < this->fmt_str.length() ) {
	    if ( this->tany.cchr(TDISP_IDX,0) == 'A' ) {
		this->str_buf->printf(this->fmt_str.cstr(),
				      this->tmp_str_buf->cstr());
	    }
	    else if ( this->tany.at_cs(TDISP_IDX).strpbrk("FEGD") == 0 ) {
		double v;
		tstring sv;
		sv.assign(this->tmp_str_buf->cstr());
		REMOVE_SPC(sv);
		v = this->tzero_rec + sv.atof() * this->tscal_rec;
		if ( isfinite(v) ) {
		    this->str_buf->printf(this->fmt_str.cstr(),v);
		    if ( this->tany.cchr(TDISP_IDX,0) == 'D' ) {
			ssize_t ep = this->str_buf->strchr('E');
			if ( 0 <= ep ) this->str_buf->at(ep) = 'D';
		    }
		}
		else {
		    goto ret_null;
		}
	    }
	    else if ( this->tany.at_cs(TDISP_IDX).strpbrk("I") == 0 ) {
		double v;
		tstring sv;
		sv.assign(this->tmp_str_buf->cstr());
		REMOVE_SPC(sv);
		v = this->tzero_rec + sv.atof() * this->tscal_rec;
		if ( isfinite(v) && MIN_DOUBLE_ROUND_LLONG <= v && v <= MAX_DOUBLE_ROUND_LLONG ) {
		    this->str_buf->printf(this->fmt_str.cstr(),round_d2ll(v));
		}
		else {
		    goto ret_null;
		}
	    }
	    else if ( this->tany.cchr(TDISP_IDX,0) == 'L' ) {
		this->str_buf->printf(this->fmt_str.cstr(),
				      this->tmp_str_buf->cstr());
	    }
	    else {
		this->str_buf->assign(this->tmp_str_buf->cstr());
	    }
	}
	else {
	    this->str_buf->assign(this->tmp_str_buf->cstr());
	}
	return this->str_buf->cstr();
    }
    else if ( this->type_rec == FITS::LONG_T ) {
	long val = this->long_value(row_index,elem_index,repetition_idx);
	if ( this->tnull_is_set_rec == true && val == this->tnull_r_rec ) {
	    goto ret_null;
	}
	else if ( 0 < this->fmt_str.length() ) {
	    DO_IBOZFEGD(val);
	}
	else {
	    if ( this->tscal_rec == 1.0 && this->tzero_rec == 0.0 ) {
		this->str_buf->printf("%ld",val);
	    }
	    else {
		double v = this->tzero_rec + val * this->tscal_rec;
		if ( isfinite(v) ) this->str_buf->printf("%.10G",v);
		else goto ret_null;
	    }
	}
	return this->str_buf->cstr();
    }
    else if ( this->type_rec == FITS::LONGLONG_T ) {
	long long val = 
	    this->longlong_value(row_index,elem_index,repetition_idx);
	if ( this->tnull_is_set_rec == true && val == this->tnull_r_rec ) {
	    goto ret_null;
	}
	else if ( 0 < this->fmt_str.length() ) {
	    DO_IBOZFEGD(val);
	}
	else {
	    if ( this->tscal_rec == 1.0 && this->tzero_rec == 0.0 ) {
		this->str_buf->printf("%lld",val);
	    }
	    else {
		double v = this->tzero_rec + val * this->tscal_rec;
		if ( isfinite(v) ) this->str_buf->printf("%.15G",v);
		else goto ret_null;
	    }
	}
	return this->str_buf->cstr();
    }
    else if ( this->type_rec == FITS::SHORT_T ) {
	short val = this->short_value(row_index,elem_index,repetition_idx);
	if ( this->tnull_is_set_rec == true && val == this->tnull_r_rec ) {
	    goto ret_null;
	}
	else if ( 0 < this->fmt_str.length() ) {
	    DO_IBOZFEGD(val);
	}
	else {
	    if ( this->tscal_rec == 1.0 && this->tzero_rec == 0.0 ) {
		this->str_buf->printf("%hd",val);
	    }
	    else {
		double v = this->tzero_rec + val * this->tscal_rec;
		if ( isfinite(v) ) this->str_buf->printf("%.7G",v);
		else goto ret_null;
	    }
	}
	return this->str_buf->cstr();
    }
    else if ( this->type_rec == FITS::DOUBLE_T ) {
	double val = this->double_value(row_index,elem_index,repetition_idx);
	if ( !isfinite(val) ) {
	    goto ret_null;
	}
	else if ( 0 < this->fmt_str.length() ) {
	    DO_IBOZFEGD(val);
	}
	else {
	    double v = this->tzero_rec + val * this->tscal_rec;
	    if ( isfinite(v) ) this->str_buf->printf("%.15G",v);
	    else goto ret_null;
	}
	return this->str_buf->cstr();
    }
    else if ( this->type_rec == FITS::FLOAT_T ) {
	float val = this->float_value(row_index,elem_index,repetition_idx);
	if ( !isfinite(val) ) {
	    goto ret_null;
	}
	else if ( 0 < this->fmt_str.length() ) {
	    DO_IBOZFEGD(val);
	}
	else {
	    if ( this->tscal_rec == 1.0 && this->tzero_rec == 0.0 ) {
		this->str_buf->printf("%.7G",(double)val);
	    }
	    else {
		double v = this->tzero_rec + val * this->tscal_rec;
		if ( isfinite(v) ) this->str_buf->printf("%.7G",v);
		else goto ret_null;
	    }
	}
	return this->str_buf->cstr();
    }
    else if ( this->type_rec == FITS::BYTE_T ) {
	unsigned char val = 
	    this->byte_value(row_index,elem_index,repetition_idx);
	if ( this->tnull_is_set_rec == true && val == this->tnull_r_rec ) {
	    goto ret_null;
	}
	else if ( 0 < this->fmt_str.length() ) {
	    DO_IBOZFEGD(val);
	}
	else {
	    if ( this->tscal_rec == 1.0 && this->tzero_rec == 0.0 ) {
		this->str_buf->printf("%hhu",val);
	    }
	    else {
		double v = this->tzero_rec + val * this->tscal_rec;
		if ( isfinite(v) ) this->str_buf->printf("%.7G",v);
		else goto ret_null;
	    }
	}
	return this->str_buf->cstr();
    }
    else if ( this->type_rec == FITS::LOGICAL_T ) {
	int v = this->logical_value(row_index,elem_index,repetition_idx);
	if ( 0 < this->fmt_str.length() ) {
	    if ( v == 'T' ) {
		this->str_buf->printf(this->fmt_str.cstr(),"T");
	    }
	    else if ( v == 'F' ) {
		this->str_buf->printf(this->fmt_str.cstr(),"F");
	    }
	    else {
		this->str_buf->printf(this->fmt_nullstr.cstr(),
				      this->null_svalue());
	    }
	}
	else {
	    if (v == 'T') this->str_buf->assign("T");
	    else if (v == 'F') this->str_buf->assign("F");
	    else this->str_buf->assign(this->null_svalue());
	}
	return this->str_buf->cstr();
    }
    else if ( this->type_rec == FITS::BIT_T ) {
	long long v = this->bit_value(row_index,elem_index,repetition_idx);
	if ( 0 < this->fmt_str.length() ) {
	    long long ll_v=v; DO_LL(ll_v);
	}
	else {
	    this->str_buf->printf("%lld",v);
	}
	return this->str_buf->cstr();
    }
 ret_null:
    if ( 0 < this->fmt_nullstr.length() ) 
	this->str_buf->printf(this->fmt_nullstr.cstr(), this->null_svalue());
    else
	this->str_buf->assign(this->null_svalue());
    return this->str_buf->cstr();
}

/* */

/* get string */

/**
 * @brief  セルの値を文字列値で取得 (高レベル・tstringで取得)
 *
 *  FITS規約に基づく各種変換処理を行なったセル値を，文字列値で返します．<br>
 *  NULL値の場合，fits_table_col::assign_null_svalue() で指定された NULL 文字列
 *  が返されます(NULL文字列の初期設定値は "NULL" です)．バイナリテーブルの整
 *  数型カラムやアスキーテーブルの TNULLn の値に，セルの値が一致した場合も
 *  NULL値とします．<br>
 *  ・バイナリテーブルの場合 <br>
 *  整数型や実数型のカラムの場合，セルの値をヘッダの TZEROn と TSCALn の値で
 *  変換し，さらに文字列に変換した値を返します．<br>
 *  TDISPn の指定がある場合は，TDISPn の指定に従ってフォーマットされた文字列を
 *  返します．<br>
 *  TDISPn の指定が無い場合，実数型のカラムと TZEROn，TSCALn が設定されている
 *  整数型のカラムでは，以下のprintf()関数のフォーマットに従って変換した文字列
 *  を返します．<br>
 *  FITS::FLOAT_T                    ... "%.7G" <br>
 *  FITS::DOUBLE_T, FITS::LONGLONG_T ... "%.15G" <br>
 *  FITS::LONG_T                     ... "%.10G" <br>
 *  上記以外のカラムタイプ           ... "%.7G" <br>
 *  ・アスキーテーブルの場合 <br>
 *  TFORMn が数値表現を示さない場合には，セルの文字列を TFORMn でフォーマット
 *  した文字列を返します(TFORMn の指定が無い場合は，セルの文字列をそのまま返し
 *  ます)．<br>
 *  TFORMn が数値表現を示す場合には，セルの文字列を実数値に変換し，その値を
 *  TZEROn と TSCALn で変換し，その値を TFORMn でフォーマットした文字列を返し
 *  ます．<br>
 *
 * @param   row_index 行インデックス
 * @param   dest セル値の格納先 (return)
 * @return  セルの値: 正常終了．NULL値の場合は NULL文字列 <br>
 *          NULL文字列: 引数が不正な場合 <br>
 *          NULL: dest が NULL の場合
 * @note  バイナリテーブルの文字列型のカラムでも TNULLn の設定が可能です．これ
 *        は SFITSIO の独自拡張です．
 * @attention  可変長配列はサポートされていません．
 */
const char *fits_table_col::get_svalue( long row_index,
					tstring *dest ) const
{
    if ( dest == NULL ) return NULL;

    if ( row_index < 0 || this->row_size_rec <= row_index ) goto ret_null;

    if ( this->type_rec == FITS::STRING_T ) {
	const char *d_ptr = (const char *)this->data_ptr();
	if ( d_ptr == NULL ) goto ret_null;
	dest->assign(d_ptr + 
		    this->bytes_rec * this->elem_size_rec * row_index,
		    this->bytes_rec);
	if ( dest->length() == 0 && this->tnull_is_set_rec == false ) {
	    goto ret_null;
	}
	else if ( this->tnull_is_set_rec == true &&
		  this->tany.at_cs(TNULL_IDX).strcmp(dest->cstr()) == 0 ) {
	    goto ret_null;
	}
	else if ( 0 < this->fmt_str.length() ) {
	    if ( this->tany.cchr(TDISP_IDX,0) == 'A' ) {
		dest->assignf(this->fmt_str.cstr(),dest->cstr());
	    }
	    else if ( this->tany.at_cs(TDISP_IDX).strpbrk("FEGD") == 0 ) {
		double v;
		REMOVE_SPC((*dest));
		v = this->tzero_rec + dest->atof() * this->tscal_rec;
		if ( isfinite(v) ) {
		    dest->assignf(this->fmt_str.cstr(),v);
		    if ( this->tany.cchr(TDISP_IDX,0) == 'D' ) {
			ssize_t ep = dest->strchr('E');
			if ( 0 <= ep ) dest->at(ep) = 'D';
		    }
		}
		else {
		    goto ret_null;
		}
	    }
	    else if ( this->tany.at_cs(TDISP_IDX).strpbrk("I") == 0 ) {
		double v;
		REMOVE_SPC((*dest));
		v = this->tzero_rec + dest->atof() * this->tscal_rec;
		if ( isfinite(v) && MIN_DOUBLE_ROUND_LLONG <= v && v <= MAX_DOUBLE_ROUND_LLONG ) {
		    dest->assignf(this->fmt_str.cstr(),round_d2ll(v));
		}
		else {
		    goto ret_null;
		}
	    }
	    else if ( this->tany.cchr(TDISP_IDX,0) == 'L' ) {
		dest->assignf(this->fmt_str.cstr(),dest->cstr());
	    }
	}
	return dest->cstr();
    }
    else if ( this->type_rec == FITS::LONG_T ) {
	long val = this->long_value(row_index);
	if ( this->tnull_is_set_rec == true && val == this->tnull_r_rec ) {
	    goto ret_null;
	}
	else if ( 0 < this->fmt_str.length() ) {
	    DO_IBOZFEGD_TS(val,(*dest));
	}
	else {
	    if ( this->tscal_rec == 1.0 && this->tzero_rec == 0.0 ) {
		dest->printf("%ld",val);
	    }
	    else {
		double v = this->tzero_rec + val * this->tscal_rec;
		if ( isfinite(v) ) dest->printf("%.10G",v);
		else goto ret_null;
	    }
	}
	return dest->cstr();
    }
    else if ( this->type_rec == FITS::LONGLONG_T ) {
	long long val = this->longlong_value(row_index);
	if ( this->tnull_is_set_rec == true && val == this->tnull_r_rec ) {
	    goto ret_null;
	}
	else if ( 0 < this->fmt_str.length() ) {
	    DO_IBOZFEGD_TS(val,(*dest));
	}
	else {
	    if ( this->tscal_rec == 1.0 && this->tzero_rec == 0.0 ) {
		dest->printf("%lld",val);
	    }
	    else {
		double v = this->tzero_rec + val * this->tscal_rec;
		if ( isfinite(v) ) dest->printf("%.15G",v);
		else goto ret_null;
	    }
	}
	return dest->cstr();
    }
    else if ( this->type_rec == FITS::SHORT_T ) {
	short val = this->short_value(row_index);
	if ( this->tnull_is_set_rec == true && val == this->tnull_r_rec ) {
	    goto ret_null;
	}
	else if ( 0 < this->fmt_str.length() ) {
	    DO_IBOZFEGD_TS(val,(*dest));
	}
	else {
	    if ( this->tscal_rec == 1.0 && this->tzero_rec == 0.0 ) {
		dest->printf("%hd",val);
	    }
	    else {
		double v = this->tzero_rec + val * this->tscal_rec;
		if ( isfinite(v) ) dest->printf("%.7G",v);
		else goto ret_null;
	    }
	}
	return dest->cstr();
    }
    else if ( this->type_rec == FITS::DOUBLE_T ) {
	double val = this->double_value(row_index);
	if ( !isfinite(val) ) {
	    goto ret_null;
	}
	else if ( 0 < this->fmt_str.length() ) {
	    DO_IBOZFEGD_TS(val,(*dest));
	}
	else {
	    double v = this->tzero_rec + val * this->tscal_rec;
	    if ( isfinite(v) ) dest->printf("%.15G",v);
	    else goto ret_null;
	}
	return dest->cstr();
    }
    else if ( this->type_rec == FITS::FLOAT_T ) {
	float val = this->float_value(row_index);
	if ( !isfinite(val) ) {
	    goto ret_null;
	}
	else if ( 0 < this->fmt_str.length() ) {
	    DO_IBOZFEGD_TS(val,(*dest));
	}
	else {
	    if ( this->tscal_rec == 1.0 && this->tzero_rec == 0.0 ) {
		dest->printf("%.7G",(double)val);
	    }
	    else {
		double v = this->tzero_rec + val * this->tscal_rec;
		if ( isfinite(v) ) dest->printf("%.7G",v);
		else goto ret_null;
	    }
	}
	return dest->cstr();
    }
    else if ( this->type_rec == FITS::BYTE_T ) {
	unsigned char val = this->byte_value(row_index);
	if ( this->tnull_is_set_rec == true && val == this->tnull_r_rec ) {
	    goto ret_null;
	}
	else if ( 0 < this->fmt_str.length() ) {
	    DO_IBOZFEGD_TS(val,(*dest));
	}
	else {
	    if ( this->tscal_rec == 1.0 && this->tzero_rec == 0.0 ) {
		dest->printf("%hhu",val);
	    }
	    else {
		double v = this->tzero_rec + val * this->tscal_rec;
		if ( isfinite(v) ) dest->printf("%.7G",v);
		else goto ret_null;
	    }
	}
	return dest->cstr();
    }
    else if ( this->type_rec == FITS::LOGICAL_T ) {
	int v = this->logical_value(row_index);
	if ( 0 < this->fmt_str.length() ) {
	    if ( v == 'T' ) {
		dest->printf(this->fmt_str.cstr(),"T");
	    }
	    else if ( v == 'F' ) {
		dest->printf(this->fmt_str.cstr(),"F");
	    }
	    else {
		dest->printf(this->fmt_nullstr.cstr(), this->null_svalue());
	    }
	}
	else {
	    if (v == 'T') dest->assign("T");
	    else if (v == 'F') dest->assign("F");
	    else dest->assign(this->null_svalue());
	}
	return dest->cstr();
    }
    else if ( this->type_rec == FITS::BIT_T ) {
	long long v = this->bit_value(row_index);
	if ( 0 < this->fmt_str.length() ) {
	    long long ll_v=v; DO_LL_TS(ll_v,(*dest));
	}
	else {
	    dest->printf("%lld",v);
	}
	return dest->cstr();
    }
 ret_null:
    if ( 0 < this->fmt_nullstr.length() ) 
	dest->assignf(this->fmt_nullstr.cstr(), this->null_svalue());
    else
	dest->assign(this->null_svalue());
    return dest->cstr();
}

/**
 * @brief  セルの値を文字列値で取得 (高レベル・tstringで取得)
 *
 *  FITS規約に基づく各種変換処理を行なったセル値を，文字列値で返します．<br>
 *  NULL値の場合，fits_table_col::assign_null_svalue() で指定された NULL 文字列
 *  が返されます(NULL文字列の初期設定値は "NULL" です)．バイナリテーブルの整
 *  数型カラムやアスキーテーブルの TNULLn の値に，セルの値が一致した場合も
 *  NULL値とします．<br>
 *  ・バイナリテーブルの場合 <br>
 *  整数型や実数型のカラムの場合，セルの値をヘッダの TZEROn と TSCALn の値で
 *  変換し，さらに文字列に変換した値を返します．<br>
 *  TDISPn の指定がある場合は，TDISPn の指定に従ってフォーマットされた文字列を
 *  返します．<br>
 *  TDISPn の指定が無い場合，実数型のカラムと TZEROn，TSCALn が設定されている
 *  整数型のカラムでは，以下のprintf()関数のフォーマットに従って変換した文字列
 *  を返します．<br>
 *  FITS::FLOAT_T                    ... "%.7G" <br>
 *  FITS::DOUBLE_T, FITS::LONGLONG_T ... "%.15G" <br>
 *  FITS::LONG_T                     ... "%.10G" <br>
 *  上記以外のカラムタイプ           ... "%.7G" <br>
 *  ・アスキーテーブルの場合 <br>
 *  TFORMn が数値表現を示さない場合には，セルの文字列を TFORMn でフォーマット
 *  した文字列を返します(TFORMn の指定が無い場合は，セルの文字列をそのまま返し
 *  ます)．<br>
 *  TFORMn が数値表現を示す場合には，セルの文字列を実数値に変換し，その値を
 *  TZEROn と TSCALn で変換し，その値を TFORMn でフォーマットした文字列を返し
 *  ます．<br>
 *
 * @param   row_index 行インデックス
 * @param   elem_name 要素名 (TELEMn に存在する名前)
 * @param   dest セル値の格納先 (return)
 * @return  セルの値: 正常終了．NULL値の場合は NULL文字列 <br>
 *          NULL文字列: 引数が不正な場合 <br>
 *          NULL: dest が NULL の場合
 * @note  バイナリテーブルの文字列型のカラムでも TNULLn の設定が可能です．これ
 *        は SFITSIO の独自拡張です．
 * @attention  可変長配列はサポートされていません．
 */
const char *fits_table_col::get_svalue( long row_index,
					const char *elem_name, 
					tstring *dest ) const
{
    if ( dest == NULL ) return NULL;

    return this->get_svalue( row_index, elem_name, 0, dest );
}

/**
 * @brief  セルの値を文字列値で取得 (高レベル・tstringで取得)
 *
 *  FITS規約に基づく各種変換処理を行なったセル値を，文字列値で返します．<br>
 *  NULL値の場合，fits_table_col::assign_null_svalue() で指定された NULL 文字列
 *  が返されます(NULL文字列の初期設定値は "NULL" です)．バイナリテーブルの整
 *  数型カラムやアスキーテーブルの TNULLn の値に，セルの値が一致した場合も
 *  NULL値とします．<br>
 *  ・バイナリテーブルの場合 <br>
 *  整数型や実数型のカラムの場合，セルの値をヘッダの TZEROn と TSCALn の値で
 *  変換し，さらに文字列に変換した値を返します．<br>
 *  TDISPn の指定がある場合は，TDISPn の指定に従ってフォーマットされた文字列を
 *  返します．<br>
 *  TDISPn の指定が無い場合，実数型のカラムと TZEROn，TSCALn が設定されている
 *  整数型のカラムでは，以下のprintf()関数のフォーマットに従って変換した文字列
 *  を返します．<br>
 *  FITS::FLOAT_T                    ... "%.7G" <br>
 *  FITS::DOUBLE_T, FITS::LONGLONG_T ... "%.15G" <br>
 *  FITS::LONG_T                     ... "%.10G" <br>
 *  上記以外のカラムタイプ           ... "%.7G" <br>
 *  ・アスキーテーブルの場合 <br>
 *  TFORMn が数値表現を示さない場合には，セルの文字列を TFORMn でフォーマット
 *  した文字列を返します(TFORMn の指定が無い場合は，セルの文字列をそのまま返し
 *  ます)．<br>
 *  TFORMn が数値表現を示す場合には，セルの文字列を実数値に変換し，その値を
 *  TZEROn と TSCALn で変換し，その値を TFORMn でフォーマットした文字列を返し
 *  ます．<br>
 *
 * @param   row_index 行インデックス
 * @param   elem_name 要素名 (TELEMn に存在する名前)
 * @param   repetition_idx TDIMn の2次元目のインデックス
 * @param   dest セル値の格納先 (return)
 * @return  セルの値: 正常終了．NULL値の場合は NULL文字列 <br>
 *          NULL文字列: 引数が不正な場合 <br>
 *          NULL: dest が NULL の場合
 * @note  バイナリテーブルの文字列型のカラムでも TNULLn の設定が可能です．これ
 *        は SFITSIO の独自拡張です．
 * @attention  可変長配列はサポートされていません．
 */
const char *fits_table_col::get_svalue( long row_index,
					const char *elem_name, 
					long repetition_idx,
					tstring *dest ) const
{
    if ( dest == NULL ) return NULL;

    long elem_index = this->elem_index_rec.index(elem_name,0);
    if ( this->type_rec == FITS::BIT_T ) {
	long long v;
	if ( row_index < 0 || this->row_size_rec <= row_index ) goto ret_null;
	if ( elem_index < 0 || repetition_idx < 0 ) goto ret_null;
	v = this->bit_value( row_index, elem_index, repetition_idx, 0 );
	if ( 0 < this->fmt_str.length() ) {
	    long long ll_v=v;
	    DO_LL_TS(ll_v,(*dest));
	}
	else {
	    dest->printf("%lld",v);
	}
	return dest->cstr();
    }
    else {
	return this->get_svalue( row_index, elem_index, repetition_idx, dest );
    }
 ret_null:
    if ( 0 < this->fmt_nullstr.length() ) 
	dest->assignf(this->fmt_nullstr.cstr(), this->null_svalue());
    else
	dest->assign(this->null_svalue());
    return dest->cstr();
}

/**
 * @brief  セルの値を文字列値で取得 (高レベル・tstringで取得)
 *
 *  FITS規約に基づく各種変換処理を行なったセル値を，文字列値で返します．<br>
 *  NULL値の場合，fits_table_col::assign_null_svalue() で指定された NULL 文字列
 *  が返されます(NULL文字列の初期設定値は "NULL" です)．バイナリテーブルの整
 *  数型カラムやアスキーテーブルの TNULLn の値に，セルの値が一致した場合も
 *  NULL値とします．<br>
 *  ・バイナリテーブルの場合 <br>
 *  整数型や実数型のカラムの場合，セルの値をヘッダの TZEROn と TSCALn の値で
 *  変換し，さらに文字列に変換した値を返します．<br>
 *  TDISPn の指定がある場合は，TDISPn の指定に従ってフォーマットされた文字列を
 *  返します．<br>
 *  TDISPn の指定が無い場合，実数型のカラムと TZEROn，TSCALn が設定されている
 *  整数型のカラムでは，以下のprintf()関数のフォーマットに従って変換した文字列
 *  を返します．<br>
 *  FITS::FLOAT_T                    ... "%.7G" <br>
 *  FITS::DOUBLE_T, FITS::LONGLONG_T ... "%.15G" <br>
 *  FITS::LONG_T                     ... "%.10G" <br>
 *  上記以外のカラムタイプ           ... "%.7G" <br>
 *  ・アスキーテーブルの場合 <br>
 *  TFORMn が数値表現を示さない場合には，セルの文字列を TFORMn でフォーマット
 *  した文字列を返します(TFORMn の指定が無い場合は，セルの文字列をそのまま返し
 *  ます)．<br>
 *  TFORMn が数値表現を示す場合には，セルの文字列を実数値に変換し，その値を
 *  TZEROn と TSCALn で変換し，その値を TFORMn でフォーマットした文字列を返し
 *  ます．<br>
 *
 * @param   row_index 行インデックス
 * @param   elem_index 要素インデックス (TDIMn の1次元目のインデックス)
 * @param   dest セル値の格納先 (return)
 * @return  セルの値: 正常終了．NULL値の場合は NULL文字列 <br>
 *          NULL文字列: 引数が不正な場合 <br>
 *          NULL: dest が NULL の場合
 * @note  バイナリテーブルの文字列型のカラムでも TNULLn の設定が可能です．これ
 *        は SFITSIO の独自拡張です．
 * @attention  可変長配列はサポートされていません．
 */
const char *fits_table_col::get_svalue( long row_index,
					long elem_index, tstring *dest ) const
{
    if ( dest == NULL ) return NULL;

    return this->get_svalue( row_index, elem_index, 0, dest );
}

/**
 * @brief  セルの値を文字列値で取得 (高レベル・tstringで取得)
 *
 *  FITS規約に基づく各種変換処理を行なったセル値を，文字列値で返します．<br>
 *  NULL値の場合，fits_table_col::assign_null_svalue() で指定された NULL 文字列
 *  が返されます(NULL文字列の初期設定値は "NULL" です)．バイナリテーブルの整
 *  数型カラムやアスキーテーブルの TNULLn の値に，セルの値が一致した場合も
 *  NULL値とします．<br>
 *  ・バイナリテーブルの場合 <br>
 *  整数型や実数型のカラムの場合，セルの値をヘッダの TZEROn と TSCALn の値で
 *  変換し，さらに文字列に変換した値を返します．<br>
 *  TDISPn の指定がある場合は，TDISPn の指定に従ってフォーマットされた文字列を
 *  返します．<br>
 *  TDISPn の指定が無い場合，実数型のカラムと TZEROn，TSCALn が設定されている
 *  整数型のカラムでは，以下のprintf()関数のフォーマットに従って変換した文字列
 *  を返します．<br>
 *  FITS::FLOAT_T                    ... "%.7G" <br>
 *  FITS::DOUBLE_T, FITS::LONGLONG_T ... "%.15G" <br>
 *  FITS::LONG_T                     ... "%.10G" <br>
 *  上記以外のカラムタイプ           ... "%.7G" <br>
 *  ・アスキーテーブルの場合 <br>
 *  TFORMn が数値表現を示さない場合には，セルの文字列を TFORMn でフォーマット
 *  した文字列を返します(TFORMn の指定が無い場合は，セルの文字列をそのまま返し
 *  ます)．<br>
 *  TFORMn が数値表現を示す場合には，セルの文字列を実数値に変換し，その値を
 *  TZEROn と TSCALn で変換し，その値を TFORMn でフォーマットした文字列を返し
 *  ます．<br>
 *
 * @param   row_index 行インデックス
 * @param   elem_index 要素インデックス (TDIMn の1次元目のインデックス)
 * @param   repetition_idx TDIMn の2次元目のインデックス
 * @param   dest セル値の格納先 (return)
 * @return  セルの値: 正常終了．NULL値の場合は NULL文字列 <br>
 *          NULL文字列: 引数が不正な場合 <br>
 *          NULL: dest が NULL の場合
 * @note  バイナリテーブルの文字列型のカラムでも TNULLn の設定が可能です．これ
 *        は SFITSIO の独自拡張です．
 * @attention  可変長配列はサポートされていません．
 */
const char *fits_table_col::get_svalue( long row_index,
					long elem_index, 
					long repetition_idx,
					tstring *dest ) const
{
    if ( dest == NULL ) return NULL;

    if ( row_index < 0 || this->row_size_rec <= row_index ) goto ret_null;
    if ( elem_index < 0 || repetition_idx < 0 ) goto ret_null;

    if ( this->type_rec == FITS::STRING_T ) {
	const char *d_ptr = (const char *)this->data_ptr();
	long e_idx;
	if ( d_ptr == NULL ) goto ret_null;
	e_idx = this->dcol_size_rec * repetition_idx 
	    + elem_index;
	if ( this->elem_size_rec <= e_idx ) 
	    e_idx = this->elem_size_rec - this->dcol_size_rec 
		+ elem_index % this->dcol_size_rec;
	dest->assign(d_ptr + 
	      this->bytes_rec * (this->elem_size_rec * row_index + e_idx),
	      this->bytes_rec);
	if ( dest->length() == 0 && this->tnull_is_set_rec == false ) {
	    goto ret_null;
	}
	else if ( this->tnull_is_set_rec == true &&
		  this->tany.at_cs(TNULL_IDX).strcmp(dest->cstr()) == 0 ) {
	    goto ret_null;
	}
	else if ( 0 < this->fmt_str.length() ) {
	    if ( this->tany.cchr(TDISP_IDX,0) == 'A' ) {
		dest->assignf(this->fmt_str.cstr(),dest->cstr());
	    }
	    else if ( this->tany.at_cs(TDISP_IDX).strpbrk("FEGD") == 0 ) {
		double v;
		REMOVE_SPC((*dest));
		v = this->tzero_rec + dest->atof() * this->tscal_rec;
		if ( isfinite(v) ) {
		    dest->assignf(this->fmt_str.cstr(),v);
		    if ( this->tany.cchr(TDISP_IDX,0) == 'D' ) {
			ssize_t ep = dest->strchr('E');
			if ( 0 <= ep ) dest->at(ep) = 'D';
		    }
		}
		else {
		    goto ret_null;
		}
	    }
	    else if ( this->tany.at_cs(TDISP_IDX).strpbrk("I") == 0 ) {
		double v;
		REMOVE_SPC((*dest));
		v = this->tzero_rec + dest->atof() * this->tscal_rec;
		if ( isfinite(v) && MIN_DOUBLE_ROUND_LLONG <= v && v <= MAX_DOUBLE_ROUND_LLONG ) {
		    dest->assignf(this->fmt_str.cstr(),round_d2ll(v));
		}
		else {
		    goto ret_null;
		}
	    }
	    else if ( this->tany.cchr(TDISP_IDX,0) == 'L' ) {
		dest->assignf(this->fmt_str.cstr(),dest->cstr());
	    }
	}
	return dest->cstr();
    }
    else if ( this->type_rec == FITS::LONG_T ) {
	long val = this->long_value(row_index,elem_index,repetition_idx);
	if ( this->tnull_is_set_rec == true && val == this->tnull_r_rec ) {
	    goto ret_null;
	}
	else if ( 0 < this->fmt_str.length() ) {
	    DO_IBOZFEGD_TS(val,(*dest));
	}
	else {
	    if ( this->tscal_rec == 1.0 && this->tzero_rec == 0.0 ) {
		dest->printf("%ld",val);
	    }
	    else {
		double v = this->tzero_rec + val * this->tscal_rec;
		if ( isfinite(v) ) dest->printf("%.10G",v);
		else goto ret_null;
	    }
	}
	return dest->cstr();
    }
    else if ( this->type_rec == FITS::LONGLONG_T ) {
	long long val = 
	    this->longlong_value(row_index,elem_index,repetition_idx);
	if ( this->tnull_is_set_rec == true && val == this->tnull_r_rec ) {
	    goto ret_null;
	}
	else if ( 0 < this->fmt_str.length() ) {
	    DO_IBOZFEGD_TS(val,(*dest));
	}
	else {
	    if ( this->tscal_rec == 1.0 && this->tzero_rec == 0.0 ) {
		dest->printf("%lld",val);
	    }
	    else {
		double v = this->tzero_rec + val * this->tscal_rec;
		if ( isfinite(v) ) dest->printf("%.15G",v);
		else goto ret_null;
	    }
	}
	return dest->cstr();
    }
    else if ( this->type_rec == FITS::SHORT_T ) {
	short val = 
	    this->short_value(row_index,elem_index,repetition_idx);
	if ( this->tnull_is_set_rec == true && val == this->tnull_r_rec ) {
	    goto ret_null;
	}
	else if ( 0 < this->fmt_str.length() ) {
	    DO_IBOZFEGD_TS(val,(*dest));
	}
	else {
	    if ( this->tscal_rec == 1.0 && this->tzero_rec == 0.0 ) {
		dest->printf("%hd",val);
	    }
	    else {
		double v = this->tzero_rec + val * this->tscal_rec;
		if ( isfinite(v) ) dest->printf("%.7G",v);
		else goto ret_null;
	    }
	}
	return dest->cstr();
    }
    else if ( this->type_rec == FITS::DOUBLE_T ) {
	double val = this->double_value(row_index,elem_index,repetition_idx);
	if ( !isfinite(val) ) {
	    goto ret_null;
	}
	else if ( 0 < this->fmt_str.length() ) {
	    DO_IBOZFEGD_TS(val,(*dest));
	}
	else {
	    double v = this->tzero_rec + val * this->tscal_rec;
	    if ( isfinite(v) ) dest->printf("%.15G",v);
	    else goto ret_null;
	}
	return dest->cstr();
    }
    else if ( this->type_rec == FITS::FLOAT_T ) {
	float val = this->float_value(row_index,elem_index,repetition_idx);
	if ( !isfinite(val) ) {
	    goto ret_null;
	}
	else if ( 0 < this->fmt_str.length() ) {
	    DO_IBOZFEGD_TS(val,(*dest));
	}
	else {
	    if ( this->tscal_rec == 1.0 && this->tzero_rec == 0.0 ) {
		dest->printf("%.7G",(double)val);
	    }
	    else {
		double v = this->tzero_rec + val * this->tscal_rec;
		if ( isfinite(v) ) dest->printf("%.7G",v);
		else goto ret_null;
	    }
	}
	return dest->cstr();
    }
    else if ( this->type_rec == FITS::BYTE_T ) {
	unsigned char val = 
	    this->byte_value(row_index,elem_index,repetition_idx);
	if ( this->tnull_is_set_rec == true && val == this->tnull_r_rec ) {
	    goto ret_null;
	}
	else if ( 0 < this->fmt_str.length() ) {
	    DO_IBOZFEGD_TS(val,(*dest));
	}
	else {
	    if ( this->tscal_rec == 1.0 && this->tzero_rec == 0.0 ) {
		dest->printf("%hhu",val);
	    }
	    else {
		double v = this->tzero_rec + val * this->tscal_rec;
		if ( isfinite(v) ) dest->printf("%.7G",v);
		else goto ret_null;
	    }
	}
	return dest->cstr();
    }
    else if ( this->type_rec == FITS::LOGICAL_T ) {
	int v = this->logical_value(row_index,elem_index,repetition_idx);
	if ( 0 < this->fmt_str.length() ) {
	    if ( v == 'T' ) {
		dest->printf(this->fmt_str.cstr(),"T");
	    }
	    else if ( v == 'F' ) {
		dest->printf(this->fmt_str.cstr(),"F");
	    }
	    else {
		dest->printf(this->fmt_nullstr.cstr(), this->null_svalue());
	    }
	}
	else {
	    if (v == 'T') dest->assign("T");
	    else if (v == 'F') dest->assign("F");
	    else dest->assign(this->null_svalue());
	}
	return dest->cstr();
    }
    else if ( this->type_rec == FITS::BIT_T ) {
	long long v = this->bit_value(row_index,elem_index,repetition_idx);
	if ( 0 < this->fmt_str.length() ) {
	    long long ll_v=v; DO_LL_TS(ll_v,(*dest));
	}
	else {
	    dest->printf("%lld",v);
	}
	return dest->cstr();
    }
 ret_null:
    if ( 0 < this->fmt_nullstr.length() ) 
	dest->assignf(this->fmt_nullstr.cstr(), this->null_svalue());
    else
	dest->assign(this->null_svalue());
    return dest->cstr();
}

/**
 * @deprecated  非推奨．<br>
 *              fits_table_col::get_svalue( long, tstring * ) <br>
 *              をお使いください．
 */
const char *fits_table_col::get_svalue( long row_index, 
					tstring &dest ) const
{
    return this->get_svalue(row_index, &dest);
}

/**
 * @deprecated  非推奨．<br>
 *             fits_table_col::get_svalue( long, const char *, tstring * ) <br>
 *             をお使いください．
 */
const char *fits_table_col::get_svalue( long row_index,
					const char *elem_name, 
					tstring &dest ) const
{
    return this->get_svalue(row_index, elem_name, 0, &dest);
}

/**
 * @deprecated  非推奨．<br>
 *       fits_table_col::get_svalue( long, const char *, long, tstring * ) <br>
 *       をお使いください．
 */
const char *fits_table_col::get_svalue( long row_index,
					const char *elem_name, 
					long repetition_idx,
					tstring &dest ) const
{
    return this->get_svalue(row_index, elem_name, repetition_idx, &dest);
}

/**
 * @deprecated  非推奨．<br>
 *              fits_table_col::get_svalue( long, long, tstring * ) <br>
 *              をお使いください．
 */
const char *fits_table_col::get_svalue( long row_index,
					long elem_index, tstring &dest ) const
{
    return this->get_svalue(row_index, elem_index, 0, &dest);
}

/**
 * @deprecated  非推奨．<br>
 *              fits_table_col::get_svalue( long, long, long, tstring * ) <br>
 *              をお使いください．
 */
const char *fits_table_col::get_svalue( long row_index,
					long elem_index, 
					long repetition_idx,
					tstring &dest ) const
{
    return this->get_svalue(row_index, elem_index, repetition_idx, &dest);
}

#undef DO_IBOZFEGD_TS
#undef DO_IBOZFEGD
#undef DO_LL_TS
#undef DO_LL

/**
 * @brief  セルの値を文字列値で取得 (高レベル・ユーザバッファに取得)
 *
 *  FITS規約に基づく各種変換処理を行なったセル値を，文字列値で返します．<br>
 *  NULL値の場合，fits_table_col::assign_null_svalue() で指定された NULL 文字列
 *  が返されます(NULL文字列の初期設定値は "NULL" です)．バイナリテーブルの整
 *  数型カラムやアスキーテーブルの TNULLn の値に，セルの値が一致した場合も
 *  NULL値とします．<br>
 *  ・バイナリテーブルの場合 <br>
 *  整数型や実数型のカラムの場合，セルの値をヘッダの TZEROn と TSCALn の値で
 *  変換し，さらに文字列に変換した値を返します．<br>
 *  TDISPn の指定がある場合は，TDISPn の指定に従ってフォーマットされた文字列を
 *  返します．<br>
 *  TDISPn の指定が無い場合，実数型のカラムと TZEROn，TSCALn が設定されている
 *  整数型のカラムでは，以下のprintf()関数のフォーマットに従って変換した文字列
 *  を返します．<br>
 *  FITS::FLOAT_T                    ... "%.7G" <br>
 *  FITS::DOUBLE_T, FITS::LONGLONG_T ... "%.15G" <br>
 *  FITS::LONG_T                     ... "%.10G" <br>
 *  上記以外のカラムタイプ           ... "%.7G" <br>
 *  ・アスキーテーブルの場合 <br>
 *  TFORMn が数値表現を示さない場合には，セルの文字列を TFORMn でフォーマット
 *  した文字列を返します(TFORMn の指定が無い場合は，セルの文字列をそのまま返し
 *  ます)．<br>
 *  TFORMn が数値表現を示す場合には，セルの文字列を実数値に変換し，その値を
 *  TZEROn と TSCALn で変換し，その値を TFORMn でフォーマットした文字列を返し
 *  ます．<br>
 *
 * @param   row_index 行インデックス
 * @param   dest_buf セル値の格納先 (return)
 * @param   buf_size dest_buf のバッファサイズ
 * @return  非負の値: バッファ長が十分な場合にコピーできる文字数
 *                    ('\0'は含まない) <br>
 *          負の値(エラー): 引数が不正でコピーされなかった場合
 * @note  バイナリテーブルの文字列型のカラムでも TNULLn の設定が可能です．これ
 *        は SFITSIO の独自拡張です．
 * @attention  可変長配列はサポートされていません．
 */
ssize_t fits_table_col::get_svalue( long row_index, 
				    char *dest_buf, size_t buf_size ) const
{
    tstring sval;
    this->get_svalue( row_index, &sval );
    return sval.getstr(dest_buf,buf_size);
}

/**
 * @brief  セルの値を文字列値で取得 (高レベル・ユーザバッファに取得)
 *
 *  FITS規約に基づく各種変換処理を行なったセル値を，文字列値で返します．<br>
 *  NULL値の場合，fits_table_col::assign_null_svalue() で指定された NULL 文字列
 *  が返されます(NULL文字列の初期設定値は "NULL" です)．バイナリテーブルの整
 *  数型カラムやアスキーテーブルの TNULLn の値に，セルの値が一致した場合も
 *  NULL値とします．<br>
 *  ・バイナリテーブルの場合 <br>
 *  整数型や実数型のカラムの場合，セルの値をヘッダの TZEROn と TSCALn の値で
 *  変換し，さらに文字列に変換した値を返します．<br>
 *  TDISPn の指定がある場合は，TDISPn の指定に従ってフォーマットされた文字列を
 *  返します．<br>
 *  TDISPn の指定が無い場合，実数型のカラムと TZEROn，TSCALn が設定されている
 *  整数型のカラムでは，以下のprintf()関数のフォーマットに従って変換した文字列
 *  を返します．<br>
 *  FITS::FLOAT_T                    ... "%.7G" <br>
 *  FITS::DOUBLE_T, FITS::LONGLONG_T ... "%.15G" <br>
 *  FITS::LONG_T                     ... "%.10G" <br>
 *  上記以外のカラムタイプ           ... "%.7G" <br>
 *  ・アスキーテーブルの場合 <br>
 *  TFORMn が数値表現を示さない場合には，セルの文字列を TFORMn でフォーマット
 *  した文字列を返します(TFORMn の指定が無い場合は，セルの文字列をそのまま返し
 *  ます)．<br>
 *  TFORMn が数値表現を示す場合には，セルの文字列を実数値に変換し，その値を
 *  TZEROn と TSCALn で変換し，その値を TFORMn でフォーマットした文字列を返し
 *  ます．<br>
 *
 * @param   row_index 行インデックス
 * @param   elem_name 要素名 (TELEMn に存在する名前)
 * @param   dest_buf セル値の格納先 (return)
 * @param   buf_size dest_buf のバッファサイズ
 * @return  非負の値: バッファ長が十分な場合にコピーできる文字数
 *                    ('\0'は含まない) <br>
 *          負の値(エラー): 引数が不正でコピーされなかった場合
 * @note  バイナリテーブルの文字列型のカラムでも TNULLn の設定が可能です．これ
 *        は SFITSIO の独自拡張です．
 * @attention  可変長配列はサポートされていません．
 */
ssize_t fits_table_col::get_svalue( long row_index,
				    const char *elem_name, 
				    char *dest_buf, size_t buf_size ) const
{
    tstring sval;
    this->get_svalue( row_index, elem_name, 0, &sval );
    return sval.getstr(dest_buf,buf_size);
}

/**
 * @brief  セルの値を文字列値で取得 (高レベル・ユーザバッファに取得)
 *
 *  FITS規約に基づく各種変換処理を行なったセル値を，文字列値で返します．<br>
 *  NULL値の場合，fits_table_col::assign_null_svalue() で指定された NULL 文字列
 *  が返されます(NULL文字列の初期設定値は "NULL" です)．バイナリテーブルの整
 *  数型カラムやアスキーテーブルの TNULLn の値に，セルの値が一致した場合も
 *  NULL値とします．<br>
 *  ・バイナリテーブルの場合 <br>
 *  整数型や実数型のカラムの場合，セルの値をヘッダの TZEROn と TSCALn の値で
 *  変換し，さらに文字列に変換した値を返します．<br>
 *  TDISPn の指定がある場合は，TDISPn の指定に従ってフォーマットされた文字列を
 *  返します．<br>
 *  TDISPn の指定が無い場合，実数型のカラムと TZEROn，TSCALn が設定されている
 *  整数型のカラムでは，以下のprintf()関数のフォーマットに従って変換した文字列
 *  を返します．<br>
 *  FITS::FLOAT_T                    ... "%.7G" <br>
 *  FITS::DOUBLE_T, FITS::LONGLONG_T ... "%.15G" <br>
 *  FITS::LONG_T                     ... "%.10G" <br>
 *  上記以外のカラムタイプ           ... "%.7G" <br>
 *  ・アスキーテーブルの場合 <br>
 *  TFORMn が数値表現を示さない場合には，セルの文字列を TFORMn でフォーマット
 *  した文字列を返します(TFORMn の指定が無い場合は，セルの文字列をそのまま返し
 *  ます)．<br>
 *  TFORMn が数値表現を示す場合には，セルの文字列を実数値に変換し，その値を
 *  TZEROn と TSCALn で変換し，その値を TFORMn でフォーマットした文字列を返し
 *  ます．<br>
 *
 * @param   row_index 行インデックス
 * @param   elem_name 要素名 (TELEMn に存在する名前)
 * @param   repetition_idx TDIMn の2次元目のインデックス
 * @param   dest_buf セル値の格納先 (return)
 * @param   buf_size dest_buf のバッファサイズ
 * @return  非負の値: バッファ長が十分な場合にコピーできる文字数
 *                    ('\0'は含まない) <br>
 *          負の値(エラー): 引数が不正でコピーされなかった場合
 * @note  バイナリテーブルの文字列型のカラムでも TNULLn の設定が可能です．これ
 *        は SFITSIO の独自拡張です．
 * @attention  可変長配列はサポートされていません．
 */
ssize_t fits_table_col::get_svalue( long row_index,
				    const char *elem_name, long repetition_idx,
				    char *dest_buf, size_t buf_size ) const
{
    tstring sval;
    this->get_svalue( row_index, elem_name, repetition_idx, &sval );
    return sval.getstr(dest_buf,buf_size);
}

/**
 * @brief  セルの値を文字列値で取得 (高レベル・ユーザバッファに取得)
 *
 *  FITS規約に基づく各種変換処理を行なったセル値を，文字列値で返します．<br>
 *  NULL値の場合，fits_table_col::assign_null_svalue() で指定された NULL 文字列
 *  が返されます(NULL文字列の初期設定値は "NULL" です)．バイナリテーブルの整
 *  数型カラムやアスキーテーブルの TNULLn の値に，セルの値が一致した場合も
 *  NULL値とします．<br>
 *  ・バイナリテーブルの場合 <br>
 *  整数型や実数型のカラムの場合，セルの値をヘッダの TZEROn と TSCALn の値で
 *  変換し，さらに文字列に変換した値を返します．<br>
 *  TDISPn の指定がある場合は，TDISPn の指定に従ってフォーマットされた文字列を
 *  返します．<br>
 *  TDISPn の指定が無い場合，実数型のカラムと TZEROn，TSCALn が設定されている
 *  整数型のカラムでは，以下のprintf()関数のフォーマットに従って変換した文字列
 *  を返します．<br>
 *  FITS::FLOAT_T                    ... "%.7G" <br>
 *  FITS::DOUBLE_T, FITS::LONGLONG_T ... "%.15G" <br>
 *  FITS::LONG_T                     ... "%.10G" <br>
 *  上記以外のカラムタイプ           ... "%.7G" <br>
 *  ・アスキーテーブルの場合 <br>
 *  TFORMn が数値表現を示さない場合には，セルの文字列を TFORMn でフォーマット
 *  した文字列を返します(TFORMn の指定が無い場合は，セルの文字列をそのまま返し
 *  ます)．<br>
 *  TFORMn が数値表現を示す場合には，セルの文字列を実数値に変換し，その値を
 *  TZEROn と TSCALn で変換し，その値を TFORMn でフォーマットした文字列を返し
 *  ます．<br>
 *
 * @param   row_index 行インデックス
 * @param   elem_index 要素インデックス (TDIMn の1次元目のインデックス)
 * @param   dest_buf セル値の格納先 (return)
 * @param   buf_size dest_buf のバッファサイズ
 * @return  非負の値: バッファ長が十分な場合にコピーできる文字数
 *                    ('\0'は含まない) <br>
 *          負の値(エラー): 引数が不正でコピーされなかった場合
 * @note  バイナリテーブルの文字列型のカラムでも TNULLn の設定が可能です．これ
 *        は SFITSIO の独自拡張です．
 * @attention  可変長配列はサポートされていません．
 */
ssize_t fits_table_col::get_svalue( long row_index, long elem_index, 
				    char *dest_buf, size_t buf_size ) const
{
    tstring sval;
    this->get_svalue( row_index, elem_index, 0, &sval );
    return sval.getstr(dest_buf,buf_size);
}

/**
 * @brief  セルの値を文字列値で取得 (高レベル・ユーザバッファに取得)
 *
 *  FITS規約に基づく各種変換処理を行なったセル値を，文字列値で返します．<br>
 *  NULL値の場合，fits_table_col::assign_null_svalue() で指定された NULL 文字列
 *  が返されます(NULL文字列の初期設定値は "NULL" です)．バイナリテーブルの整
 *  数型カラムやアスキーテーブルの TNULLn の値に，セルの値が一致した場合も
 *  NULL値とします．<br>
 *  ・バイナリテーブルの場合 <br>
 *  整数型や実数型のカラムの場合，セルの値をヘッダの TZEROn と TSCALn の値で
 *  変換し，さらに文字列に変換した値を返します．<br>
 *  TDISPn の指定がある場合は，TDISPn の指定に従ってフォーマットされた文字列を
 *  返します．<br>
 *  TDISPn の指定が無い場合，実数型のカラムと TZEROn，TSCALn が設定されている
 *  整数型のカラムでは，以下のprintf()関数のフォーマットに従って変換した文字列
 *  を返します．<br>
 *  FITS::FLOAT_T                    ... "%.7G" <br>
 *  FITS::DOUBLE_T, FITS::LONGLONG_T ... "%.15G" <br>
 *  FITS::LONG_T                     ... "%.10G" <br>
 *  上記以外のカラムタイプ           ... "%.7G" <br>
 *  ・アスキーテーブルの場合 <br>
 *  TFORMn が数値表現を示さない場合には，セルの文字列を TFORMn でフォーマット
 *  した文字列を返します(TFORMn の指定が無い場合は，セルの文字列をそのまま返し
 *  ます)．<br>
 *  TFORMn が数値表現を示す場合には，セルの文字列を実数値に変換し，その値を
 *  TZEROn と TSCALn で変換し，その値を TFORMn でフォーマットした文字列を返し
 *  ます．<br>
 *
 * @param   row_index 行インデックス
 * @param   elem_index 要素インデックス (TDIMn の1次元目のインデックス)
 * @param   repetition_idx TDIMn の2次元目のインデックス
 * @param   dest_buf セル値の格納先 (return)
 * @param   buf_size dest_buf のバッファサイズ
 * @return  非負の値: バッファ長が十分な場合にコピーできる文字数
 *                    ('\0'は含まない) <br>
 *          負の値(エラー): 引数が不正でコピーされなかった場合
 * @note  バイナリテーブルの文字列型のカラムでも TNULLn の設定が可能です．これ
 *        は SFITSIO の独自拡張です．
 * @attention  可変長配列はサポートされていません．
 */
ssize_t fits_table_col::get_svalue( long row_index,
				    long elem_index, long repetition_idx,
				    char *dest_buf, size_t buf_size ) const
{
    tstring sval;
    this->get_svalue( row_index, elem_index, repetition_idx, &sval );
    return sval.getstr(dest_buf,buf_size);
}

/*
 * low-level APIs to read a cell.
 */

/* logical */
/**
 * @brief  セルの生の値を論理値で取得 (低レベル)
 *
 *  返り値は，'T'，'F'，'\0' (NULL値) の3種類です．<br>
 *  カラムの型が FITS::LOGICAL_T (TFORMn の指定に L を含む) の場合に最速で
 *  アクセスできます．
 *
 * @param   row_index 行インデックス
 * @return  セルの値: 正常終了．<br>
 *          NULL値: 引数が不正な場合
 * @attention  可変長配列はサポートされていません．
 */
int fits_table_col::logical_value( long row_index ) const
{
    if ( row_index < 0 || this->row_size_rec <= row_index ) return '\0';

    if ( this->type_rec == FITS::LOGICAL_T ) {
	fits::logical_t v;
	const fits::logical_t *d_ptr = (fits::logical_t *)this->data_ptr();
	if ( d_ptr == NULL ) return '\0';
	v = d_ptr[this->elem_size_rec * row_index];
	if ( v == 'T' ) return 'T';
	else if ( v == 'F' ) return 'F';
	else return '\0';
    }
    else if ( this->type_rec == FITS::LONG_T ) {
	long val = this->long_value(row_index);
	if ( this->tnull_is_set_rec == true && val == this->tnull_r_rec )
	    return '\0';
	return (val != 0) ? 'T' : 'F';
    }
    else if ( this->type_rec == FITS::SHORT_T ) {
	short val = this->short_value(row_index);
	if ( this->tnull_is_set_rec == true && val == this->tnull_r_rec )
	    return '\0';
	return (val != 0) ? 'T' : 'F';
    }
    else if ( this->type_rec == FITS::LONGLONG_T ) {
	long long val = this->longlong_value(row_index);
	if ( this->tnull_is_set_rec == true && val == this->tnull_r_rec )
	    return '\0';
	return (val != 0) ? 'T' : 'F';
    }
    else if ( this->type_rec == FITS::BYTE_T ) {
	unsigned char val = this->byte_value(row_index);
	if ( this->tnull_is_set_rec == true && val == this->tnull_r_rec )
	    return '\0';
	return (val != 0) ? 'T' : 'F';
    }
    else if ( this->type_rec == FITS::DOUBLE_T ) {
	double v = this->double_value(row_index);
	if ( !isfinite(v) ) return '\0';
	else return (round(v) != 0.0) ? 'T' : 'F';
    }
    else if ( this->type_rec == FITS::FLOAT_T ) {
	float v = this->float_value(row_index);
	if ( !isfinite(v) ) return '\0';
	else return (round(v) != 0.0) ? 'T' : 'F';
    }
    else if ( this->type_rec == FITS::BIT_T ) {
	return (this->bit_value(row_index) != 0) ? 'T' : 'F';
    }
    else if ( this->type_rec == FITS::STRING_T ) {
	tstring sval;
	double dval;
	size_t epos;
	this->get_string_value(row_index, &sval);
	if ( sval.length() == 0 && this->tnull_is_set_rec == false ) {
	    return '\0';
	}
	else if ( this->tnull_is_set_rec == true &&
		  this->tany.at_cs(TNULL_IDX).strcmp(sval.cstr()) == 0 ) {
	    return '\0';
	}
	REMOVE_SPC(sval);
	dval = sval.strtod(&epos);
	if ( epos == 0 ) {
	    if ( sval.cchr(0) == 'T' || sval.cchr(0) == 't' ) return 'T';
	    else if ( sval.cchr(0) == 'F' || sval.cchr(0) == 'f' ) return 'F';
	    else return '\0';
	}
	else {
	    if ( !isfinite(dval) ) return '\0';
	    else return (round(dval) != 0.0) ? 'T' : 'F';
	}
    }
    else return '\0';
}

/**
 * @brief  セルの生の値を論理値で取得 (低レベル)
 *
 *  返り値は，'T'，'F'，'\0' (NULL値) の3種類です．<br>
 *  カラムの型が FITS::LOGICAL_T (TFORMn の指定に L を含む) の場合に最速で
 *  アクセスできます．
 *
 * @param   row_index 行インデックス
 * @param   elem_name 要素名 (TELEMn に存在する名前)
 * @param   repetition_idx  TDIMn の2次元目のインデックス
 * @return  セルの値: 正常終了．<br>
 *          NULL値: 引数が不正な場合
 * @attention  可変長配列はサポートされていません．
 */
int fits_table_col::logical_value( long row_index,
			     const char *elem_name, long repetition_idx ) const
{
    long elem_index = this->elem_index_rec.index(elem_name,0);
    if ( this->type_rec == FITS::BIT_T ) {
	if ( row_index < 0 || this->row_size_rec <= row_index ) return '\0';
	if ( elem_index < 0 || repetition_idx < 0 ) return '\0';
	return (this->bit_value(row_index,elem_index,repetition_idx,0) != 0) ? 'T' : 'F';
    }
    else {
	return this->logical_value( row_index, elem_index, repetition_idx );
    }
}

/**
 * @brief  セルの生の値を論理値で取得 (低レベル)
 *
 *  返り値は，'T'，'F'，'\0' (NULL値) の3種類です．<br>
 *  カラムの型が FITS::LOGICAL_T (TFORMn の指定に L を含む) の場合に最速で
 *  アクセスできます．
 *
 * @param   row_index 行インデックス
 * @param   elem_index 要素インデックス (TDIMn の1次元目のインデックス)
 * @param   repetition_idx  TDIMn の2次元目のインデックス
 * @return  セルの値: 正常終了．<br>
 *          NULL値: 引数が不正な場合
 * @attention  可変長配列はサポートされていません．
 */
int fits_table_col::logical_value( long row_index,
				   long elem_index, long repetition_idx ) const
{
    if ( row_index < 0 || this->row_size_rec <= row_index ) return '\0';
    if ( elem_index < 0 || repetition_idx < 0 ) return '\0';

    if ( this->type_rec == FITS::LOGICAL_T ) {
	fits::logical_t v;
	const fits::logical_t *d_ptr = (fits::logical_t *)this->data_ptr();
	long e_idx;
	if ( d_ptr == NULL ) return '\0';
	e_idx = this->dcol_size_rec * repetition_idx + elem_index;
	if ( this->elem_size_rec <= e_idx ) 
	    e_idx = this->elem_size_rec - this->dcol_size_rec 
		+ elem_index % this->dcol_size_rec;
	v = d_ptr[this->elem_size_rec * row_index + e_idx];
	if ( v == 'T' ) return 'T';
	else if ( v == 'F' ) return 'F';
	else return '\0';
    }
    else if ( this->type_rec == FITS::LONG_T ) {
	long val = this->long_value(row_index,elem_index,repetition_idx);
	if ( this->tnull_is_set_rec == true && val == this->tnull_r_rec )
	    return '\0';
	return (val != 0) ? 'T' : 'F';
    }
    else if ( this->type_rec == FITS::SHORT_T ) {
	short val = this->short_value(row_index,elem_index,repetition_idx);
	if ( this->tnull_is_set_rec == true && val == this->tnull_r_rec )
	    return '\0';
	return (val != 0) ? 'T' : 'F';
    }
    else if ( this->type_rec == FITS::LONGLONG_T ) {
	long long val = this->longlong_value(row_index,elem_index,repetition_idx);
	if ( this->tnull_is_set_rec == true && val == this->tnull_r_rec )
	    return '\0';
	return (val != 0) ? 'T' : 'F';
    }
    else if ( this->type_rec == FITS::BYTE_T ) {
	unsigned char val = this->byte_value(row_index,elem_index,repetition_idx);
	if ( this->tnull_is_set_rec == true && val == this->tnull_r_rec )
	    return '\0';
	return (val != 0) ? 'T' : 'F';
    }
    else if ( this->type_rec == FITS::DOUBLE_T ) {
	double v = this->double_value(row_index,elem_index,repetition_idx);
	if ( !isfinite(v) ) return '\0';
	else return (round(v) != 0.0) ? 'T' : 'F';
    }
    else if ( this->type_rec == FITS::FLOAT_T ) {
	float v = this->float_value(row_index,elem_index,repetition_idx);
	if ( !isfinite(v) ) return '\0';
	else return (round(v) != 0.0) ? 'T' : 'F';
    }
    else if ( this->type_rec == FITS::BIT_T ) {
	return (this->bit_value(row_index,elem_index,repetition_idx) != 0) ? 'T' : 'F';
    }
    else if ( this->type_rec == FITS::STRING_T ) {
	tstring sval;
	double dval;
	size_t epos;
	this->get_string_value(row_index,elem_index,repetition_idx, &sval);
	if ( sval.length() == 0 && this->tnull_is_set_rec == false ) {
	    return '\0';
	}
	else if ( this->tnull_is_set_rec == true &&
		  this->tany.at_cs(TNULL_IDX).strcmp(sval.cstr()) == 0 ) {
	    return '\0';
	}
	REMOVE_SPC(sval);
	dval = sval.strtod(&epos);
	if ( epos == 0 ) {
	    if ( sval.cchr(0) == 'T' || sval.cchr(0) == 't' ) return 'T';
	    else if ( sval.cchr(0) == 'F' || sval.cchr(0) == 'f' ) return 'F';
	    else return '\0';
	}
	else {
	    if ( !isfinite(dval) ) return '\0';
	    else return (round(dval) != 0.0) ? 'T' : 'F';
	}
    }
    else return '\0';
}


/* short */
/**
 * @brief  セルの生の値を整数値(short型)で取得 (低レベル)
 *
 *  セルの生の値(ヘッダの TZEROn と TSCALn の値を反映していない値)を整数値
 *  (short型)で返します．<br>
 *  カラムの型が FITS::SHORT_T (TFORMn の指定に 'I' を含む) の場合に最速で
 *  アクセスできます．<br>
 *  返される NULL値は，ヘッダの TNULLn の値が存在する場合はその値，存在しない
 *  場合は INDEF_SHORT です．
 *
 * @param   row_index 行インデックス
 * @return  セルの値: 正常終了．<br>
 *          NULL値: 引数が不正な場合
 * @attention  可変長配列はサポートされていません．
 */
short fits_table_col::short_value( long row_index ) const
{
    if ( row_index < 0 || this->row_size_rec <= row_index ) goto invalid;

    if ( this->type_rec == FITS::SHORT_T ) {
	const fits::short_t *d_ptr = (fits::short_t *)this->data_ptr();
	if ( d_ptr == NULL ) goto invalid;
	return d_ptr[this->elem_size_rec * row_index];
    }
    else if ( this->type_rec == FITS::LONG_T ) {
	long v0 = this->long_value(row_index);
	if ( MIN_SHORT <= v0 && v0 <= MAX_SHORT ) return v0;
	else return this->tnull_short_rec;
    }
    else if ( this->type_rec == FITS::LONGLONG_T ) {
	long long v0 = this->longlong_value(row_index);
	if ( MIN_SHORT <= v0 && v0 <= MAX_SHORT ) return v0;
	else return this->tnull_short_rec;
    }
    else if ( this->type_rec == FITS::BYTE_T ) {
	return this->byte_value(row_index);
    }
    else if ( this->type_rec == FITS::LOGICAL_T ) {
	int v = this->logical_value(row_index);
	if ( v == 'T' ) return 1;
	else if ( v == 'F' ) return 0;
	else return this->tnull_short_rec;
    }
    else if ( this->type_rec == FITS::DOUBLE_T ) {
	double v0 = this->double_value(row_index);
	if ( isfinite(v0) && MIN_DOUBLE_ROUND_SHORT <= v0 && v0 <= MAX_DOUBLE_ROUND_SHORT )
	    return (short)round_d2l(v0);
	else return this->tnull_short_rec;
    }
    else if ( this->type_rec == FITS::FLOAT_T ) {
	float v0 = this->float_value(row_index);
	if ( isfinite(v0) && MIN_FLOAT_ROUND_SHORT <= v0 && v0 <= MAX_FLOAT_ROUND_SHORT )
	    return (short)round_f2l(v0);
	else return this->tnull_short_rec;
    }
    else if ( this->type_rec == FITS::BIT_T ) {
	long long v0 = this->bit_value(row_index);
	if ( MIN_SHORT <= v0 && v0 <= MAX_SHORT ) return v0;
	else return this->tnull_short_rec;
    }
    else if ( this->type_rec == FITS::STRING_T ) {
	tstring sval;
	double v0;
	this->get_string_value(row_index, &sval);
	if ( sval.length() == 0 && this->tnull_is_set_rec == false ) {
	    return this->tnull_short_rec;
	}
	else if ( this->tnull_is_set_rec == true &&
		  this->tany.at_cs(TNULL_IDX).strcmp(sval.cstr()) == 0 ) {
	    return this->tnull_short_rec;
	}
	REMOVE_SPC(sval);
	v0 = sval.atof();
	if ( isfinite(v0) && MIN_DOUBLE_ROUND_SHORT <= v0 && v0 <= MAX_DOUBLE_ROUND_SHORT )
	    return (short)round_d2l(v0);
	else return this->tnull_short_rec;
    }
 invalid:
    return this->tnull_short_rec;
}

/**
 * @brief  セルの生の値を整数値(short型)で取得 (低レベル)
 *
 *  セルの生の値(ヘッダの TZEROn と TSCALn の値を反映していない値)を整数値
 *  (short型)で返します．<br>
 *  カラムの型が FITS::SHORT_T (TFORMn の指定に 'I' を含む) の場合に最速で
 *  アクセスできます．<br>
 *  返される NULL値は，ヘッダの TNULLn の値が存在する場合はその値，存在しない
 *  場合は INDEF_SHORT です．
 *
 * @param   row_index 行インデックス
 * @param   elem_name 要素名 (TELEMn に存在する名前)
 * @param   repetition_idx  TDIMn の2次元目のインデックス
 * @return  セルの値: 正常終了．<br>
 *          NULL値: 引数が不正な場合
 * @attention  可変長配列はサポートされていません．
 */
short fits_table_col::short_value( long row_index,
			     const char *elem_name, long repetition_idx ) const
{
    long elem_index = this->elem_index_rec.index(elem_name,0);
    if ( this->type_rec == FITS::BIT_T ) {
	long long v0 = this->bit_value(row_index,elem_index,repetition_idx,0);
	if ( MIN_SHORT <= v0 && v0 <= MAX_SHORT ) return v0;
	else return this->tnull_short_rec;
    }
    else {
	return this->short_value( row_index, elem_index, repetition_idx );
    }
}

/**
 * @brief  セルの生の値を整数値(short型)で取得 (低レベル)
 *
 *  セルの生の値(ヘッダの TZEROn と TSCALn の値を反映していない値)を整数値
 *  (short型)で返します．<br>
 *  カラムの型が FITS::SHORT_T (TFORMn の指定に 'I' を含む) の場合に最速で
 *  アクセスできます．<br>
 *  返される NULL値は，ヘッダの TNULLn の値が存在する場合はその値，存在しない
 *  場合は INDEF_SHORT です．
 *
 * @param   row_index 行インデックス
 * @param   elem_index 要素インデックス (TDIMn の1次元目のインデックス)
 * @param   repetition_idx  TDIMn の2次元目のインデックス
 * @return  セルの値: 正常終了．<br>
 *          NULL値: 引数が不正な場合
 * @attention  可変長配列はサポートされていません．
 */
short fits_table_col::short_value( long row_index,
				   long elem_index, long repetition_idx ) const
{
    if ( row_index < 0 || this->row_size_rec <= row_index ) goto invalid;
    if ( elem_index < 0 || repetition_idx < 0 ) goto invalid;

    if ( this->type_rec == FITS::SHORT_T ) {
	const fits::short_t *d_ptr = (fits::short_t *)this->data_ptr();
	long e_idx;
	if ( d_ptr == NULL ) goto invalid;
	e_idx = this->dcol_size_rec * repetition_idx 
	    + elem_index;
	if ( this->elem_size_rec <= e_idx ) 
	    e_idx = this->elem_size_rec - this->dcol_size_rec 
		+ elem_index % this->dcol_size_rec;
	return d_ptr[this->elem_size_rec * row_index + e_idx];
    }
    else if ( this->type_rec == FITS::LONG_T ) {
	long v0 = this->long_value(row_index,elem_index,repetition_idx);
	if ( MIN_SHORT <= v0 && v0 <= MAX_SHORT ) return v0;
	else return this->tnull_short_rec;
    }
    else if ( this->type_rec == FITS::LONGLONG_T ) {
	long long v0 = this->longlong_value(row_index,elem_index,repetition_idx);
	if ( MIN_SHORT <= v0 && v0 <= MAX_SHORT ) return v0;
	else return this->tnull_short_rec;
    }
    else if ( this->type_rec == FITS::BYTE_T ) {
	return this->byte_value(row_index,elem_index,repetition_idx);
    }
    else if ( this->type_rec == FITS::LOGICAL_T ) {
	int v = this->logical_value(row_index,elem_index,repetition_idx);
	if ( v == 'T' ) return 1;
	else if ( v == 'F' ) return 0;
	else return this->tnull_short_rec;
    }
    else if ( this->type_rec == FITS::DOUBLE_T ) {
	double v0 = this->double_value(row_index,elem_index,repetition_idx);
	if ( isfinite(v0) && MIN_DOUBLE_ROUND_SHORT <= v0 && v0 <= MAX_DOUBLE_ROUND_SHORT )
	    return (short)round_d2l(v0);
	else return this->tnull_short_rec;
    }
    else if ( this->type_rec == FITS::FLOAT_T ) {
	float v0 = this->float_value(row_index,elem_index,repetition_idx);
	if ( isfinite(v0) && MIN_FLOAT_ROUND_SHORT <= v0 && v0 <= MAX_FLOAT_ROUND_SHORT )
	    return (short)round_f2l(v0);
	else return this->tnull_short_rec;
    }
    else if ( this->type_rec == FITS::BIT_T ) {
	long long v0 = this->bit_value(row_index,elem_index,repetition_idx);
	if ( MIN_SHORT <= v0 && v0 <= MAX_SHORT ) return v0;
	else return this->tnull_short_rec;
    }
    else if ( this->type_rec == FITS::STRING_T ) {
	tstring sval;
	double v0;
	this->get_string_value(row_index,elem_index,repetition_idx, &sval);
	if ( sval.length() == 0 && this->tnull_is_set_rec == false ) {
	    return this->tnull_short_rec;
	}
	else if ( this->tnull_is_set_rec == true &&
		  this->tany.at_cs(TNULL_IDX).strcmp(sval.cstr()) == 0 ) {
	    return this->tnull_short_rec;
	}
	REMOVE_SPC(sval);
	v0 = sval.atof();
	if ( isfinite(v0) && MIN_DOUBLE_ROUND_SHORT <= v0 && v0 <= MAX_DOUBLE_ROUND_SHORT )
	    return (short)round_d2l(v0);
	else return this->tnull_short_rec;
    }
 invalid:
    return this->tnull_short_rec;
}


/* long */
/**
 * @brief  セルの生の値を整数値(long型)で取得 (低レベル)
 *
 *  セルの生の値(ヘッダの TZEROn と TSCALn の値を反映していない値)を整数値
 *  (long型)で返します．<br>
 *  カラムの型が FITS::LONG_T (TFORMn の指定に 'J' を含む) の場合に最速で
 *  アクセスできます．<br>
 *  返される NULL値は，ヘッダの TNULLn の値が存在する場合はその値，存在しない
 *  場合は INDEF_LONG です．
 *
 * @param   row_index 行インデックス
 * @return  セルの値: 正常終了．<br>
 *          NULL値: 引数が不正な場合
 * @attention  可変長配列はサポートされていません．
 * @note  long型は 32-bit OS では 32-bit，64-bit OS では 64-bit の幅を持つ事が
 *        ほとんどです．
 */
long fits_table_col::long_value( long row_index ) const
{
    if ( row_index < 0 || this->row_size_rec <= row_index ) goto invalid;

    if ( this->type_rec == FITS::LONG_T ) {
	const fits::long_t *d_ptr = (fits::long_t *)this->data_ptr();
	if ( d_ptr == NULL ) goto invalid;
	return d_ptr[this->elem_size_rec * row_index];
    }
    else if ( this->type_rec == FITS::SHORT_T ) {
	return this->short_value(row_index);
    }
    else if ( this->type_rec == FITS::BYTE_T ) {
	return this->byte_value(row_index);
    }
    else if ( this->type_rec == FITS::LONGLONG_T ) {
	long long v0 = this->longlong_value(row_index);
	if ( MIN_LONG <= v0 && v0 <= MAX_LONG ) return v0;
	else return this->tnull_long_rec;
    }
    else if ( this->type_rec == FITS::LOGICAL_T ) {
	int v = this->logical_value(row_index);
	if ( v == 'T' ) return 1;
	else if ( v == 'F' ) return 0;
	else return this->tnull_long_rec;
    }
    else if ( this->type_rec == FITS::DOUBLE_T ) {
	double v0 = this->double_value(row_index);
	if ( isfinite(v0) && MIN_DOUBLE_ROUND_LONG <= v0 && v0 <= MAX_DOUBLE_ROUND_LONG )
	    return round_d2l(v0);
	else return this->tnull_long_rec;
    }
    else if ( this->type_rec == FITS::FLOAT_T ) {
	float v0 = this->float_value(row_index);
	if ( isfinite(v0) && MIN_FLOAT_ROUND_LONG <= v0 && v0 <= MAX_FLOAT_ROUND_LONG )
	    return round_f2l(v0);
	else return this->tnull_long_rec;
    }
    else if ( this->type_rec == FITS::BIT_T ) {
	long long v0 = this->bit_value(row_index);
	if ( MIN_LONG <= v0 && v0 <= MAX_LONG ) return v0;
	else return this->tnull_long_rec;
    }
    else if ( this->type_rec == FITS::STRING_T ) {
	tstring sval;
	double v0;
	this->get_string_value(row_index, &sval);
	if ( sval.length() == 0 && this->tnull_is_set_rec == false ) {
	    return this->tnull_long_rec;
	}
	else if ( this->tnull_is_set_rec == true &&
		  this->tany.at_cs(TNULL_IDX).strcmp(sval.cstr()) == 0 ) {
	    return this->tnull_long_rec;
	}
	REMOVE_SPC(sval);
	v0 = sval.atof();
	if ( isfinite(v0) && MIN_DOUBLE_ROUND_LONG <= v0 && v0 <= MAX_DOUBLE_ROUND_LONG )
	    return round_d2l(v0);
	else return this->tnull_long_rec;
    }
 invalid:
    return this->tnull_long_rec;
}

/**
 * @brief  セルの生の値を整数値(long型)で取得 (低レベル)
 *
 *  セルの生の値(ヘッダの TZEROn と TSCALn の値を反映していない値)を整数値
 *  (long型)で返します．<br>
 *  カラムの型が FITS::LONG_T (TFORMn の指定に 'J' を含む) の場合に最速で
 *  アクセスできます．<br>
 *  返される NULL値は，ヘッダの TNULLn の値が存在する場合はその値，存在しない
 *  場合は INDEF_LONG です．
 *
 * @param   row_index 行インデックス
 * @param   elem_name 要素名 (TELEMn に存在する名前)
 * @param   repetition_idx  TDIMn の2次元目のインデックス
 * @return  セルの値: 正常終了．<br>
 *          NULL値: 引数が不正な場合
 * @attention  可変長配列はサポートされていません．
 * @note  long型は 32-bit OS では 32-bit，64-bit OS では 64-bit の幅を持つ事が
 *        ほとんどです．
 */
long fits_table_col::long_value( long row_index,
			     const char *elem_name, long repetition_idx ) const
{
    long elem_index = this->elem_index_rec.index(elem_name,0);
    if ( this->type_rec == FITS::BIT_T ) {
	long long v0 = this->bit_value(row_index,elem_index,repetition_idx,0);
	if ( MIN_LONG <= v0 && v0 <= MAX_LONG ) return v0;
	else return this->tnull_long_rec;
    }
    else {
	return this->long_value( row_index, elem_index, repetition_idx );
    }
}

/**
 * @brief  セルの生の値を整数値(long型)で取得 (低レベル)
 *
 *  セルの生の値(ヘッダの TZEROn と TSCALn の値を反映していない値)を整数値
 *  (long型)で返します．<br>
 *  カラムの型が FITS::LONG_T (TFORMn の指定に 'J' を含む) の場合に最速で
 *  アクセスできます．<br>
 *  返される NULL値は，ヘッダの TNULLn の値が存在する場合はその値，存在しない
 *  場合は INDEF_LONG です．
 *
 * @param   row_index 行インデックス
 * @param   elem_index 要素インデックス (TDIMn の1次元目のインデックス)
 * @param   repetition_idx  TDIMn の2次元目のインデックス
 * @return  セルの値: 正常終了．<br>
 *          NULL値: 引数が不正な場合
 * @attention  可変長配列はサポートされていません．
 * @note  long型は 32-bit OS では 32-bit，64-bit OS では 64-bit の幅を持つ事が
 *        ほとんどです．
 */
long fits_table_col::long_value( long row_index,
				 long elem_index, long repetition_idx ) const
{
    if ( row_index < 0 || this->row_size_rec <= row_index ) goto invalid;
    if ( elem_index < 0 || repetition_idx < 0 ) goto invalid;

    if ( this->type_rec == FITS::LONG_T ) {
	const fits::long_t *d_ptr = (fits::long_t *)this->data_ptr();
	long e_idx;
	if ( d_ptr == NULL ) goto invalid;
	e_idx = this->dcol_size_rec * repetition_idx 
	    + elem_index;
	if ( this->elem_size_rec <= e_idx ) 
	    e_idx = this->elem_size_rec - this->dcol_size_rec 
		+ elem_index % this->dcol_size_rec;
	return d_ptr[this->elem_size_rec * row_index + e_idx];
    }
    else if ( this->type_rec == FITS::SHORT_T ) {
	return this->short_value(row_index,elem_index,repetition_idx);
    }
    else if ( this->type_rec == FITS::BYTE_T ) {
	return this->byte_value(row_index,elem_index,repetition_idx);
    }
    else if ( this->type_rec == FITS::LONGLONG_T ) {
	long long v0 = this->longlong_value(row_index,elem_index,repetition_idx);
	if ( MIN_LONG <= v0 && v0 <= MAX_LONG ) return v0;
	else return this->tnull_long_rec;
    }
    else if ( this->type_rec == FITS::LOGICAL_T ) {
	int v = this->logical_value(row_index,elem_index,repetition_idx);
	if ( v == 'T' ) return 1;
	else if ( v == 'F' ) return 0;
	else return this->tnull_long_rec;
    }
    else if ( this->type_rec == FITS::DOUBLE_T ) {
	double v0 = this->double_value(row_index,elem_index,repetition_idx);
	if ( isfinite(v0) && MIN_DOUBLE_ROUND_LONG <= v0 && v0 <= MAX_DOUBLE_ROUND_LONG )
	    return round_d2l(v0);
	else return this->tnull_long_rec;
    }
    else if ( this->type_rec == FITS::FLOAT_T ) {
	float v0 = this->float_value(row_index,elem_index,repetition_idx);
	if ( isfinite(v0) && MIN_FLOAT_ROUND_LONG <= v0 && v0 <= MAX_FLOAT_ROUND_LONG )
	    return round_f2l(v0);
	else return this->tnull_long_rec;
    }
    else if ( this->type_rec == FITS::BIT_T ) {
	long long v0 = this->bit_value(row_index,elem_index,repetition_idx);
	if ( MIN_LONG <= v0 && v0 <= MAX_LONG ) return v0;
	else return this->tnull_long_rec;
    }
    else if ( this->type_rec == FITS::STRING_T ) {
	tstring sval;
	double v0;
	this->get_string_value(row_index,elem_index,repetition_idx, &sval);
	if ( sval.length() == 0 && this->tnull_is_set_rec == false ) {
	    return this->tnull_long_rec;
	}
	else if ( this->tnull_is_set_rec == true &&
		  this->tany.at_cs(TNULL_IDX).strcmp(sval.cstr()) == 0 ) {
	    return this->tnull_long_rec;
	}
	REMOVE_SPC(sval);
	v0 = sval.atof();
	if ( isfinite(v0) && MIN_DOUBLE_ROUND_LONG <= v0 && v0 <= MAX_DOUBLE_ROUND_LONG )
	    return round_d2l(v0);
	else return this->tnull_long_rec;
    }
 invalid:
    return this->tnull_long_rec;
}


/* longlong */
/**
 * @brief  セルの生の値を整数値(long long型)で取得 (低レベル)
 *
 *  セルの生の値(ヘッダの TZEROn と TSCALn の値を反映していない値)を整数値
 *  (long long型)で返します．<br>
 *  カラムの型が FITS::LONGLONG_T (TFORMn の指定に 'K' を含む) の場合に最速で
 *  アクセスできます．<br>
 *  返される NULL値は，ヘッダの TNULLn の値が存在する場合はその値，存在しない
 *  場合は INDEF_LLONG です．
 *
 * @param   row_index 行インデックス
 * @return  セルの値: 正常終了．<br>
 *          NULL値: 引数が不正な場合
 * @attention  可変長配列はサポートされていません．
 */
long long fits_table_col::longlong_value( long row_index ) const
{
    if ( row_index < 0 || this->row_size_rec <= row_index ) goto invalid;

    if ( this->type_rec == FITS::LONGLONG_T ) {
	const fits::longlong_t *d_ptr = (fits::longlong_t *)this->data_ptr();
	if ( d_ptr == NULL ) goto invalid;
	return d_ptr[this->elem_size_rec * row_index];
    }
    else if ( this->type_rec == FITS::LONG_T ) {
	return this->long_value(row_index);
    }
    else if ( this->type_rec == FITS::SHORT_T ) {
	return this->short_value(row_index);
    }
    else if ( this->type_rec == FITS::BYTE_T ) {
	return this->byte_value(row_index);
    }
    else if ( this->type_rec == FITS::LOGICAL_T ) {
	int v = this->logical_value(row_index);
	if ( v == 'T' ) return 1;
	else if ( v == 'F' ) return 0;
	else return this->tnull_longlong_rec;
    }
    else if ( this->type_rec == FITS::DOUBLE_T ) {
	double v0 = this->double_value(row_index);
	if ( isfinite(v0) && MIN_DOUBLE_ROUND_LLONG <= v0 && v0 <= MAX_DOUBLE_ROUND_LLONG )
	    return round_d2ll(v0);
	else return this->tnull_longlong_rec;
    }
    else if ( this->type_rec == FITS::FLOAT_T ) {
	float v0 = this->float_value(row_index);
	if ( isfinite(v0) && MIN_FLOAT_ROUND_LLONG <= v0 && v0 <= MAX_FLOAT_ROUND_LLONG )
	    return round_f2ll(v0);
	else return this->tnull_longlong_rec;
    }
    else if ( this->type_rec == FITS::BIT_T ) {
	return this->bit_value(row_index);
    }
    else if ( this->type_rec == FITS::STRING_T ) {
	tstring sval;
	double v0;
	this->get_string_value(row_index, &sval);
	if ( sval.length() == 0 && this->tnull_is_set_rec == false ) {
	    return this->tnull_longlong_rec;
	}
	else if ( this->tnull_is_set_rec == true &&
		  this->tany.at_cs(TNULL_IDX).strcmp(sval.cstr()) == 0 ) {
	    return this->tnull_longlong_rec;
	}
	REMOVE_SPC(sval);
	v0 = sval.atof();
	if ( isfinite(v0) && MIN_DOUBLE_ROUND_LLONG <= v0 && v0 <= MAX_DOUBLE_ROUND_LLONG )
	    return round_d2ll(v0);
	else return this->tnull_longlong_rec;
    }
 invalid:
    return this->tnull_longlong_rec;
}

/**
 * @brief  セルの生の値を整数値(long long型)で取得 (低レベル)
 *
 *  セルの生の値(ヘッダの TZEROn と TSCALn の値を反映していない値)を整数値
 *  (long long型)で返します．<br>
 *  カラムの型が FITS::LONGLONG_T (TFORMn の指定に 'K' を含む) の場合に最速で
 *  アクセスできます．<br>
 *  返される NULL値は，ヘッダの TNULLn の値が存在する場合はその値，存在しない
 *  場合は INDEF_LLONG です．
 *
 * @param   row_index 行インデックス
 * @param   elem_name 要素名 (TELEMn に存在する名前)
 * @param   repetition_idx  TDIMn の2次元目のインデックス
 * @return  セルの値: 正常終了．<br>
 *          NULL値: 引数が不正な場合
 * @attention  可変長配列はサポートされていません．
 */
long long fits_table_col::longlong_value( long row_index,
					  const char *elem_name, 
					  long repetition_idx ) const
{
    long elem_index = this->elem_index_rec.index(elem_name,0);
    if ( this->type_rec == FITS::BIT_T ) {
	return this->bit_value(row_index,elem_index,repetition_idx,0);
    }
    else {
	return this->longlong_value( row_index, elem_index, repetition_idx );
    }
}

/**
 * @brief  セルの生の値を整数値(long long型)で取得 (低レベル)
 *
 *  セルの生の値(ヘッダの TZEROn と TSCALn の値を反映していない値)を整数値
 *  (long long型)で返します．<br>
 *  カラムの型が FITS::LONGLONG_T (TFORMn の指定に 'K' を含む) の場合に最速で
 *  アクセスできます．<br>
 *  返される NULL値は，ヘッダの TNULLn の値が存在する場合はその値，存在しない
 *  場合は INDEF_LLONG です．
 *
 * @param   row_index 行インデックス
 * @param   elem_index 要素インデックス (TDIMn の1次元目のインデックス)
 * @param   repetition_idx  TDIMn の2次元目のインデックス
 * @return  セルの値: 正常終了．<br>
 *          NULL値: 引数が不正な場合
 * @attention  可変長配列はサポートされていません．
 */
long long fits_table_col::longlong_value( long row_index,
				   long elem_index, long repetition_idx ) const
{
    if ( row_index < 0 || this->row_size_rec <= row_index ) goto invalid;
    if ( elem_index < 0 || repetition_idx < 0 ) goto invalid;

    if ( this->type_rec == FITS::LONGLONG_T ) {
	const fits::longlong_t *d_ptr = (fits::longlong_t *)this->data_ptr();
	long e_idx;
	if ( d_ptr == NULL ) goto invalid;
	e_idx = this->dcol_size_rec * repetition_idx 
	    + elem_index;
	if ( this->elem_size_rec <= e_idx ) 
	    e_idx = this->elem_size_rec - this->dcol_size_rec 
		+ elem_index % this->dcol_size_rec;
	return d_ptr[this->elem_size_rec * row_index + e_idx];
    }
    else if ( this->type_rec == FITS::LONG_T ) {
	return this->long_value(row_index,elem_index,repetition_idx);
    }
    else if ( this->type_rec == FITS::SHORT_T ) {
	return this->short_value(row_index,elem_index,repetition_idx);
    }
    else if ( this->type_rec == FITS::BYTE_T ) {
	return this->byte_value(row_index,elem_index,repetition_idx);
    }
    else if ( this->type_rec == FITS::LOGICAL_T ) {
	int v = this->logical_value(row_index,elem_index,repetition_idx);
	if ( v == 'T' ) return 1;
	else if ( v == 'F' ) return 0;
	else return this->tnull_longlong_rec;
    }
    else if ( this->type_rec == FITS::DOUBLE_T ) {
	double v0 = this->double_value(row_index,elem_index,repetition_idx);
	if ( isfinite(v0) && MIN_DOUBLE_ROUND_LLONG <= v0 && v0 <= MAX_DOUBLE_ROUND_LLONG )
	    return round_d2ll(v0);
	else return this->tnull_longlong_rec;
    }
    else if ( this->type_rec == FITS::FLOAT_T ) {
	float v0 = this->float_value(row_index,elem_index,repetition_idx);
	if ( isfinite(v0) && MIN_FLOAT_ROUND_LLONG <= v0 && v0 <= MAX_FLOAT_ROUND_LLONG )
	    return round_f2ll(v0);
	else return this->tnull_longlong_rec;
    }
    else if ( this->type_rec == FITS::BIT_T ) {
	return this->bit_value(row_index,elem_index,repetition_idx);
    }
    else if ( this->type_rec == FITS::STRING_T ) {
	tstring sval;
	double v0;
	this->get_string_value(row_index,elem_index,repetition_idx, &sval);
	if ( sval.length() == 0 && this->tnull_is_set_rec == false ) {
	    return this->tnull_longlong_rec;
	}
	else if ( this->tnull_is_set_rec == true &&
		  this->tany.at_cs(TNULL_IDX).strcmp(sval.cstr()) == 0 ) {
	    return this->tnull_longlong_rec;
	}
	REMOVE_SPC(sval);
	v0 = sval.atof();
	if ( isfinite(v0) && MIN_DOUBLE_ROUND_LLONG <= v0 && v0 <= MAX_DOUBLE_ROUND_LLONG )
	    return round_d2ll(v0);
	else return this->tnull_longlong_rec;
    }
 invalid:
    return this->tnull_longlong_rec;
}


/* float */
/**
 * @brief  セルの生の値を実数値(float型)で取得 (低レベル)
 *
 *  セルの生の値(ヘッダの TZEROn と TSCALn の値を反映していない値)を実数値
 *  (float型)で返します．<br>
 *  カラムの型が FITS::FLOAT_T (TFORMn の指定に 'E' を含む) の場合に最速で
 *  アクセスできます．<br>
 *  NULL値の場合は，NAN を返します．
 *
 * @param   row_index 行インデックス
 * @return  セルの値: 正常終了．<br>
 *          NAN: 引数が不正な場合
 * @attention  可変長配列はサポートされていません．
 */
float fits_table_col::float_value( long row_index ) const
{
    if ( row_index < 0 || this->row_size_rec <= row_index ) goto invalid;

    if ( this->type_rec == FITS::FLOAT_T ) {
	const fits::float_t *d_ptr = (fits::float_t *)this->data_ptr();
	if ( d_ptr == NULL ) goto invalid;
	return d_ptr[this->elem_size_rec * row_index];
    }
    else if ( this->type_rec == FITS::DOUBLE_T ) {
	return this->double_value(row_index);
    }
    else if ( this->type_rec == FITS::LONG_T ) {
	long v0 = this->long_value(row_index);
	if ( this->tnull_is_set_rec == true && this->tnull_r_rec == v0 )
	    return NAN;
	else return v0;
    }
    else if ( this->type_rec == FITS::LONGLONG_T ) {
	long long v0 = this->longlong_value(row_index);
	if ( this->tnull_is_set_rec == true && this->tnull_r_rec == v0 )
	    return NAN;
	else return v0;
    }
    else if ( this->type_rec == FITS::SHORT_T ) {
	short v0 = this->short_value(row_index);
	if ( this->tnull_is_set_rec == true && this->tnull_r_rec == v0 )
	    return NAN;
	else return v0;
    }
    else if ( this->type_rec == FITS::BYTE_T ) {
	unsigned char v0 = this->byte_value(row_index);
	if ( this->tnull_is_set_rec == true && this->tnull_r_rec == v0 )
	    return NAN;
	else return v0;
    }
    else if ( this->type_rec == FITS::LOGICAL_T ) {
	int v = this->logical_value(row_index);
	if ( v == 'T' ) return 1;
	else if ( v == 'F' ) return 0;
	else return NAN;
    }
    else if ( this->type_rec == FITS::BIT_T ) {
	return this->bit_value(row_index);
    }
    else if ( this->type_rec == FITS::STRING_T ) {
	tstring sval;
	this->get_string_value(row_index, &sval);
	if ( sval.length() == 0 && this->tnull_is_set_rec == false ) {
	    return NAN;
	}
	else if ( this->tnull_is_set_rec == true &&
		  this->tany.at_cs(TNULL_IDX).strcmp(sval.cstr()) == 0 ) {
	    return NAN;
	}
	REMOVE_SPC(sval);
	return (float)(sval.atof());
    }
 invalid:
    return NAN;
}

/**
 * @brief  セルの生の値を実数値(float型)で取得 (低レベル)
 *
 *  セルの生の値(ヘッダの TZEROn と TSCALn の値を反映していない値)を実数値
 *  (float型)で返します．<br>
 *  カラムの型が FITS::FLOAT_T (TFORMn の指定に 'E' を含む) の場合に最速で
 *  アクセスできます．<br>
 *  NULL値の場合は，NAN を返します．
 *
 * @param   row_index 行インデックス
 * @param   elem_name 要素名 (TELEMn に存在する名前)
 * @param   repetition_idx  TDIMn の2次元目のインデックス
 * @return  セルの値: 正常終了．<br>
 *          NAN: 引数が不正な場合
 * @attention  可変長配列はサポートされていません．
 */
float fits_table_col::float_value( long row_index,
			     const char *elem_name, long repetition_idx ) const
{
    long elem_index = this->elem_index_rec.index(elem_name,0);
    if ( this->type_rec == FITS::BIT_T ) {
	return this->bit_value(row_index,elem_index,repetition_idx,0);
    }
    else {
	return this->float_value( row_index, elem_index, repetition_idx );
    }
}

/**
 * @brief  セルの生の値を実数値(float型)で取得 (低レベル)
 *
 *  セルの生の値(ヘッダの TZEROn と TSCALn の値を反映していない値)を実数値
 *  (float型)で返します．<br>
 *  カラムの型が FITS::FLOAT_T (TFORMn の指定に 'E' を含む) の場合に最速で
 *  アクセスできます．<br>
 *  NULL値の場合は，NAN を返します．
 *
 * @param   row_index 行インデックス
 * @param   elem_index 要素インデックス (TDIMn の1次元目のインデックス)
 * @param   repetition_idx  TDIMn の2次元目のインデックス
 * @return  セルの値: 正常終了．<br>
 *          NAN: 引数が不正な場合
 * @attention  可変長配列はサポートされていません．
 */
float fits_table_col::float_value( long row_index,
				   long elem_index, long repetition_idx ) const
{
    if ( row_index < 0 || this->row_size_rec <= row_index ) goto invalid;
    if ( elem_index < 0 || repetition_idx < 0 ) goto invalid;

    if ( this->type_rec == FITS::FLOAT_T ) {
	const fits::float_t *d_ptr = (fits::float_t *)this->data_ptr();
	long e_idx;
	if ( d_ptr == NULL ) goto invalid;
	e_idx = this->dcol_size_rec * repetition_idx 
	    + elem_index;
	if ( this->elem_size_rec <= e_idx ) 
	    e_idx = this->elem_size_rec - this->dcol_size_rec 
		+ elem_index % this->dcol_size_rec;
	return d_ptr[this->elem_size_rec * row_index + e_idx];
    }
    else if ( this->type_rec == FITS::DOUBLE_T ) {
	return this->double_value(row_index,elem_index,repetition_idx);
    }
    else if ( this->type_rec == FITS::LONG_T ) {
	long v0 = this->long_value(row_index,elem_index,repetition_idx);
	if ( this->tnull_is_set_rec == true && this->tnull_r_rec == v0 )
	    return NAN;
	else return v0;
    }
    else if ( this->type_rec == FITS::LONGLONG_T ) {
	long long v0 = this->longlong_value(row_index,elem_index,repetition_idx);
	if ( this->tnull_is_set_rec == true && this->tnull_r_rec == v0 )
	    return NAN;
	else return v0;
    }
    else if ( this->type_rec == FITS::SHORT_T ) {
	short v0 = this->short_value(row_index,elem_index,repetition_idx);
	if ( this->tnull_is_set_rec == true && this->tnull_r_rec == v0 )
	    return NAN;
	else return v0;
    }
    else if ( this->type_rec == FITS::BYTE_T ) {
	unsigned char v0 = this->byte_value(row_index,elem_index,repetition_idx);
	if ( this->tnull_is_set_rec == true && this->tnull_r_rec == v0 )
	    return NAN;
	else return v0;
    }
    else if ( this->type_rec == FITS::LOGICAL_T ) {
	int v = this->logical_value(row_index,elem_index,repetition_idx);
	if ( v == 'T' ) return 1;
	else if ( v == 'F' ) return 0;
	else return NAN;
    }
    else if ( this->type_rec == FITS::BIT_T ) {
	return this->bit_value(row_index,elem_index,repetition_idx);
    }
    else if ( this->type_rec == FITS::STRING_T ) {
	tstring sval;
	this->get_string_value(row_index,elem_index,repetition_idx, &sval);
	if ( sval.length() == 0 && this->tnull_is_set_rec == false ) {
	    return NAN;
	}
	else if ( this->tnull_is_set_rec == true &&
		  this->tany.at_cs(TNULL_IDX).strcmp(sval.cstr()) == 0 ) {
	    return NAN;
	}
	REMOVE_SPC(sval);
	return (float)(sval.atof());
    }
 invalid:
    return NAN;
}


/* double */
/**
 * @brief  セルの生の値を実数値(double型)で取得 (低レベル)
 *
 *  セルの生の値(ヘッダの TZEROn と TSCALn の値を反映していない値)を実数値
 *  (double型)で返します．<br>
 *  カラムの型が FITS::DOUBLE_T (TFORMn の指定に 'D' を含む) の場合に最速で
 *  アクセスできます．<br>
 *  NULL値の場合は，NAN を返します．
 *
 * @param   row_index 行インデックス
 * @return  セルの値: 正常終了．<br>
 *          NAN: 引数が不正な場合
 * @attention  可変長配列はサポートされていません．
 */
double fits_table_col::double_value( long row_index ) const
{
    if ( row_index < 0 || this->row_size_rec <= row_index ) goto invalid;

    if ( this->type_rec == FITS::DOUBLE_T ) {
	const fits::double_t *d_ptr = (fits::double_t *)this->data_ptr();
	if ( d_ptr == NULL ) goto invalid;
	return d_ptr[this->elem_size_rec * row_index];
    }
    else if ( this->type_rec == FITS::FLOAT_T ) {
	return this->float_value(row_index);
    }
    else if ( this->type_rec == FITS::LONG_T ) {
	long v0 = this->long_value(row_index);
	if ( this->tnull_is_set_rec == true && this->tnull_r_rec == v0 )
	    return NAN;
	else return v0;
    }
    else if ( this->type_rec == FITS::LONGLONG_T ) {
	long long v0 = this->longlong_value(row_index);
	if ( this->tnull_is_set_rec == true && this->tnull_r_rec == v0 )
	    return NAN;
	else return v0;
    }
    else if ( this->type_rec == FITS::SHORT_T ) {
	short v0 = this->short_value(row_index);
	if ( this->tnull_is_set_rec == true && this->tnull_r_rec == v0 )
	    return NAN;
	else return v0;
    }
    else if ( this->type_rec == FITS::BYTE_T ) {
	unsigned char v0 = this->byte_value(row_index);
	if ( this->tnull_is_set_rec == true && this->tnull_r_rec == v0 )
	    return NAN;
	else return v0;
    }
    else if ( this->type_rec == FITS::LOGICAL_T ) {
	int v = this->logical_value(row_index);
	if ( v == 'T' ) return 1;
	else if ( v == 'F' ) return 0;
	else return NAN;
    }
    else if ( this->type_rec == FITS::BIT_T ) {
	return this->bit_value(row_index);
    }
    else if ( this->type_rec == FITS::STRING_T ) {
	tstring sval;
	this->get_string_value(row_index, &sval);
	if ( sval.length() == 0 && this->tnull_is_set_rec == false ) {
	    return NAN;
	}
	else if ( this->tnull_is_set_rec == true &&
		  this->tany.at_cs(TNULL_IDX).strcmp(sval.cstr()) == 0 ) {
	    return NAN;
	}
	REMOVE_SPC(sval);
	return sval.atof();
    }
 invalid:
    return NAN;
}

/**
 * @brief  セルの生の値を実数値(double型)で取得 (低レベル)
 *
 *  セルの生の値(ヘッダの TZEROn と TSCALn の値を反映していない値)を実数値
 *  (double型)で返します．<br>
 *  カラムの型が FITS::DOUBLE_T (TFORMn の指定に 'D' を含む) の場合に最速で
 *  アクセスできます．<br>
 *  NULL値の場合は，NAN を返します．
 *
 * @param   row_index 行インデックス
 * @param   elem_name 要素名 (TELEMn に存在する名前)
 * @param   repetition_idx  TDIMn の2次元目のインデックス
 * @return  セルの値: 正常終了．<br>
 *          NAN: 引数が不正な場合
 * @attention  可変長配列はサポートされていません．
 */
double fits_table_col::double_value( long row_index,
			     const char *elem_name, long repetition_idx ) const
{
    long elem_index = this->elem_index_rec.index(elem_name,0);
    if ( this->type_rec == FITS::BIT_T ) {
	return this->bit_value(row_index,elem_index,repetition_idx,0);
    }
    else {
	return this->double_value( row_index, elem_index, repetition_idx );
    }
}

/**
 * @brief  セルの生の値を実数値(double型)で取得 (低レベル)
 *
 *  セルの生の値(ヘッダの TZEROn と TSCALn の値を反映していない値)を実数値
 *  (double型)で返します．<br>
 *  カラムの型が FITS::DOUBLE_T (TFORMn の指定に 'D' を含む) の場合に最速で
 *  アクセスできます．<br>
 *  NULL値の場合は，NAN を返します．
 *
 * @param   row_index 行インデックス
 * @param   elem_index 要素インデックス (TDIMn の1次元目のインデックス)
 * @param   repetition_idx  TDIMn の2次元目のインデックス
 * @return  セルの値: 正常終了．<br>
 *          NAN: 引数が不正な場合
 * @attention  可変長配列はサポートされていません．
 */
double fits_table_col::double_value( long row_index,
				   long elem_index, long repetition_idx ) const
{
    if ( row_index < 0 || this->row_size_rec <= row_index ) goto invalid;
    if ( elem_index < 0 || repetition_idx < 0 ) goto invalid;

    if ( this->type_rec == FITS::DOUBLE_T ) {
	const fits::double_t *d_ptr = (fits::double_t *)this->data_ptr();
	long e_idx;
	if ( d_ptr == NULL ) goto invalid;
	e_idx = this->dcol_size_rec * repetition_idx 
	    + elem_index;
	if ( this->elem_size_rec <= e_idx ) 
	    e_idx = this->elem_size_rec - this->dcol_size_rec 
		+ elem_index % this->dcol_size_rec;
	return d_ptr[this->elem_size_rec * row_index + e_idx];
    }
    else if ( this->type_rec == FITS::FLOAT_T ) {
	return this->float_value(row_index,elem_index,repetition_idx);
    }
    else if ( this->type_rec == FITS::LONG_T ) {
	long v0 = this->long_value(row_index,elem_index,repetition_idx);
	if ( this->tnull_is_set_rec == true && this->tnull_r_rec == v0 )
	    return NAN;
	else return v0;
    }
    else if ( this->type_rec == FITS::LONGLONG_T ) {
	long long v0 = this->longlong_value(row_index,elem_index,repetition_idx);
	if ( this->tnull_is_set_rec == true && this->tnull_r_rec == v0 )
	    return NAN;
	else return v0;
    }
    else if ( this->type_rec == FITS::SHORT_T ) {
	short v0 = this->short_value(row_index,elem_index,repetition_idx);
	if ( this->tnull_is_set_rec == true && this->tnull_r_rec == v0 )
	    return NAN;
	else return v0;
    }
    else if ( this->type_rec == FITS::BYTE_T ) {
	unsigned char v0 = this->byte_value(row_index,elem_index,repetition_idx);
	if ( this->tnull_is_set_rec == true && this->tnull_r_rec == v0 )
	    return NAN;
	else return v0;
    }
    else if ( this->type_rec == FITS::LOGICAL_T ) {
	int v = this->logical_value(row_index,elem_index,repetition_idx);
	if ( v == 'T' ) return 1;
	else if ( v == 'F' ) return 0;
	else return NAN;
    }
    else if ( this->type_rec == FITS::BIT_T ) {
	return this->bit_value(row_index,elem_index,repetition_idx);
    }
    else if ( this->type_rec == FITS::STRING_T ) {
	tstring sval;
	this->get_string_value(row_index,elem_index,repetition_idx, &sval);
	if ( sval.length() == 0 && this->tnull_is_set_rec == false ) {
	    return NAN;
	}
	else if ( this->tnull_is_set_rec == true &&
		  this->tany.at_cs(TNULL_IDX).strcmp(sval.cstr()) == 0 ) {
	    return NAN;
	}
	REMOVE_SPC(sval);
	return sval.atof();
    }
 invalid:
    return NAN;
}


/* byte */
/**
 * @brief  セルの生の値を整数値(byte型)で取得 (低レベル)
 *
 *  セルの生の値(ヘッダの TZEROn と TSCALn の値を反映していない値)を整数値
 *  (unsigned char型)で返します．<br>
 *  カラムの型が FITS::BYTE_T (TFORMn の指定に 'B' を含む) の場合に最速で
 *  アクセスできます．<br>
 *  返される NULL値は，ヘッダの TNULLn の値が存在する場合はその値，存在しない
 *  場合は INDEF_UCHAR です．
 *
 * @param   row_index 行インデックス
 * @return  セルの値: 正常終了．<br>
 *          NULL値: 引数が不正な場合
 * @attention  可変長配列はサポートされていません．
 */
unsigned char fits_table_col::byte_value( long row_index ) const
{
    if ( row_index < 0 || this->row_size_rec <= row_index ) goto invalid;

    if ( this->type_rec == FITS::BYTE_T ) {
	const fits::byte_t *d_ptr = (fits::byte_t *)this->data_ptr();
	if ( d_ptr == NULL ) goto invalid;
	return d_ptr[this->elem_size_rec * row_index];
    }
    else if ( this->type_rec == FITS::LOGICAL_T ) {
	int v = this->logical_value(row_index);
	if ( v == 'T' ) return 1;
	else if ( v == 'F' ) return 0;
	else return this->tnull_byte_rec;
    }
    else if ( this->type_rec == FITS::SHORT_T ) {
	short v0 = this->short_value(row_index);
	if ( MIN_UCHAR <= v0 && v0 <= MAX_UCHAR ) return v0;
	else return this->tnull_byte_rec;
    }
    else if ( this->type_rec == FITS::LONG_T ) {
	long v0 = this->long_value(row_index);
	if ( MIN_UCHAR <= v0 && v0 <= MAX_UCHAR ) return v0;
	else return this->tnull_byte_rec;
    }
    else if ( this->type_rec == FITS::LONGLONG_T ) {
	long long v0 = this->longlong_value(row_index);
	if ( MIN_UCHAR <= v0 && v0 <= MAX_UCHAR ) return v0;
	else return this->tnull_byte_rec;
    }
    else if ( this->type_rec == FITS::FLOAT_T ) {
	float v0 = this->float_value(row_index);
	if ( isfinite(v0) && MIN_FLOAT_ROUND_UCHAR <= v0 && v0 <= MAX_FLOAT_ROUND_UCHAR )
	    return round_f2l(v0);
	else return this->tnull_byte_rec;
    }
    else if ( this->type_rec == FITS::DOUBLE_T ) {
	double v0 = this->double_value(row_index);
	if ( isfinite(v0) && MIN_DOUBLE_ROUND_UCHAR <= v0 && v0 <= MAX_DOUBLE_ROUND_UCHAR )
	    return round_d2l(v0);
	else return this->tnull_byte_rec;
    }
    else if ( this->type_rec == FITS::BIT_T ) {
	long long v0 = this->bit_value(row_index);
	if ( MIN_UCHAR <= v0 && v0 <= MAX_UCHAR ) return v0;
	else return this->tnull_byte_rec;
    }
    else if ( this->type_rec == FITS::STRING_T ) {
	tstring sval;
	double v0;
	this->get_string_value(row_index, &sval);
	if ( sval.length() == 0 && this->tnull_is_set_rec == false ) {
	    return this->tnull_byte_rec;
	}
	else if ( this->tnull_is_set_rec == true &&
		  this->tany.at_cs(TNULL_IDX).strcmp(sval.cstr()) == 0 ) {
	    return this->tnull_byte_rec;
	}
	REMOVE_SPC(sval);
	v0 = sval.atof();
	if ( isfinite(v0) && MIN_DOUBLE_ROUND_UCHAR <= v0 && v0 <= MAX_DOUBLE_ROUND_UCHAR )
	    return round_d2l(v0);
	else return this->tnull_byte_rec;
    }
 invalid:
    return this->tnull_byte_rec;
}

/**
 * @brief  セルの生の値を整数値(byte型)で取得 (低レベル)
 *
 *  セルの生の値(ヘッダの TZEROn と TSCALn の値を反映していない値)を整数値
 *  (unsigned char型)で返します．<br>
 *  カラムの型が FITS::BYTE_T (TFORMn の指定に 'B' を含む) の場合に最速で
 *  アクセスできます．<br>
 *  返される NULL値は，ヘッダの TNULLn の値が存在する場合はその値，存在しない
 *  場合は INDEF_UCHAR です．
 *
 * @param   row_index 行インデックス
 * @param   elem_name 要素名 (TELEMn に存在する名前)
 * @param   repetition_idx  TDIMn の2次元目のインデックス
 * @return  セルの値: 正常終了．<br>
 *          NULL値: 引数が不正な場合
 * @attention  可変長配列はサポートされていません．
 */
unsigned char fits_table_col::byte_value( long row_index, 
					  const char *elem_name, 
					  long repetition_idx ) const
{
    long elem_index = this->elem_index_rec.index(elem_name,0);
    if ( this->type_rec == FITS::BIT_T ) {
	long long v0 = this->bit_value(row_index,elem_index,repetition_idx,0);
	if ( MIN_UCHAR <= v0 && v0 <= MAX_UCHAR ) return v0;
	else return this->tnull_byte_rec;
    }
    else {
	return this->byte_value( row_index, elem_index, repetition_idx );
    }
}

/**
 * @brief  セルの生の値を整数値(byte型)で取得 (低レベル)
 *
 *  セルの生の値(ヘッダの TZEROn と TSCALn の値を反映していない値)を整数値
 *  (unsigned char型)で返します．<br>
 *  カラムの型が FITS::BYTE_T (TFORMn の指定に 'B' を含む) の場合に最速で
 *  アクセスできます．<br>
 *  返される NULL値は，ヘッダの TNULLn の値が存在する場合はその値，存在しない
 *  場合は INDEF_UCHAR です．
 *
 * @param   row_index 行インデックス
 * @param   elem_index 要素インデックス (TDIMn の1次元目のインデックス)
 * @param   repetition_idx  TDIMn の2次元目のインデックス
 * @return  セルの値: 正常終了．<br>
 *          NULL値: 引数が不正な場合
 * @attention  可変長配列はサポートされていません．
 */
unsigned char fits_table_col::byte_value( long row_index,
				   long elem_index, long repetition_idx ) const
{
    if ( row_index < 0 || this->row_size_rec <= row_index ) goto invalid;
    if ( elem_index < 0 || repetition_idx < 0 ) goto invalid;

    if ( this->type_rec == FITS::BYTE_T ) {
	const fits::byte_t *d_ptr = (fits::byte_t *)this->data_ptr();
	long e_idx;
	if ( d_ptr == NULL ) goto invalid;
	e_idx = this->dcol_size_rec * repetition_idx 
	    + elem_index;
	if ( this->elem_size_rec <= e_idx ) 
	    e_idx = this->elem_size_rec - this->dcol_size_rec 
		+ elem_index % this->dcol_size_rec;
	return d_ptr[this->elem_size_rec * row_index + e_idx];
    }
    else if ( this->type_rec == FITS::LOGICAL_T ) {
	int v = this->logical_value(row_index,elem_index,repetition_idx);
	if ( v == 'T' ) return 1;
	else if ( v == 'F' ) return 0;
	else return this->tnull_byte_rec;
    }
    else if ( this->type_rec == FITS::SHORT_T ) {
	short v0 = this->short_value(row_index,elem_index,repetition_idx);
	if ( MIN_UCHAR <= v0 && v0 <= MAX_UCHAR ) return v0;
	else return this->tnull_byte_rec;
    }
    else if ( this->type_rec == FITS::LONG_T ) {
	long v0 = this->long_value(row_index,elem_index,repetition_idx);
	if ( MIN_UCHAR <= v0 && v0 <= MAX_UCHAR ) return v0;
	else return this->tnull_byte_rec;
    }
    else if ( this->type_rec == FITS::LONGLONG_T ) {
	long long v0 = this->longlong_value(row_index,elem_index,repetition_idx);
	if ( MIN_UCHAR <= v0 && v0 <= MAX_UCHAR ) return v0;
	else return this->tnull_byte_rec;
    }
    else if ( this->type_rec == FITS::FLOAT_T ) {
	float v0 = this->float_value(row_index,elem_index,repetition_idx);
	if ( isfinite(v0) && MIN_FLOAT_ROUND_UCHAR <= v0 && v0 <= MAX_FLOAT_ROUND_UCHAR )
	    return round_f2l(v0);
	else return this->tnull_byte_rec;
    }
    else if ( this->type_rec == FITS::DOUBLE_T ) {
	double v0 = this->double_value(row_index,elem_index,repetition_idx);
	if ( isfinite(v0) && MIN_DOUBLE_ROUND_UCHAR <= v0 && v0 <= MAX_DOUBLE_ROUND_UCHAR )
	    return round_d2l(v0);
	else return this->tnull_byte_rec;
    }
    else if ( this->type_rec == FITS::BIT_T ) {
	long long v0 = this->bit_value(row_index,elem_index,repetition_idx);
	if ( MIN_UCHAR <= v0 && v0 <= MAX_UCHAR ) return v0;
	else return this->tnull_byte_rec;
    }
    else if ( this->type_rec == FITS::STRING_T ) {
	tstring sval;
	double v0;
	this->get_string_value(row_index,elem_index,repetition_idx, &sval);
	if ( sval.length() == 0 && this->tnull_is_set_rec == false ) {
	    return this->tnull_byte_rec;
	}
	else if ( this->tnull_is_set_rec == true &&
		  this->tany.at_cs(TNULL_IDX).strcmp(sval.cstr()) == 0 ) {
	    return this->tnull_byte_rec;
	}
	REMOVE_SPC(sval);
	v0 = sval.atof();
	if ( isfinite(v0) && MIN_DOUBLE_ROUND_UCHAR <= v0 && v0 <= MAX_DOUBLE_ROUND_UCHAR )
	    return round_d2l(v0);
	else return this->tnull_byte_rec;
    }
 invalid:
    return this->tnull_byte_rec;
}


/* bit */
/**
 * @brief  セルの生の値を整数値(bit型)で返す (低レベル)
 *
 *  セルの生の値を整数値(bit型)で返します．<br>
 *  カラムの型が FITS::BIT_T (TFORMn の指定に 'X' を含む) の場合に最速で
 *  アクセスでき，セルのバイト要素の左端のビットの値を返します．<br>
 *  カラムの型が FITS::BIT_T 以外の場合に返される NULL値は，ヘッダの TNULLn の
 *  値が存在する場合はその値，存在しない場合は INDEF_LONG です．
 *
 * @param   row_index 行インデックス
 * @return  セルの値: 正常終了．<br>
 *          NULL値: 引数が不正な場合
 * @attention  可変長配列はサポートされていません．
 */
long long fits_table_col::bit_value( long row_index ) const
{
    if ( row_index < 0 || this->row_size_rec <= row_index ) goto invalid;

    if ( this->type_rec == FITS::BIT_T ) {
	/* MSBを見る事 */
	const unsigned char *d_ptr = (const unsigned char *)this->data_ptr();
	unsigned char v;
	if ( d_ptr == NULL ) goto invalid;
	v = d_ptr[this->full_bytes_rec * row_index];
	if ( (v & 0x080) != 0 ) return 1;
	else return 0;
    }
    else if ( this->type_rec == FITS::BYTE_T ) {
	return this->byte_value(row_index);
    }
    else if ( this->type_rec == FITS::LOGICAL_T ) {
	int v = this->logical_value(row_index);
	if ( v == 'T' ) return 1;
	else if ( v == 'F' ) return 0;
	else return this->tnull_longlong_rec;
    }
    else if ( this->type_rec == FITS::SHORT_T ) {
	return this->short_value(row_index);
    }
    else if ( this->type_rec == FITS::LONG_T ) {
	return this->long_value(row_index);
    }
    else if ( this->type_rec == FITS::LONGLONG_T ) {
	return this->longlong_value(row_index);
    }
    else if ( this->type_rec == FITS::FLOAT_T ) {
	float v0 = this->float_value(row_index);
	if ( isfinite(v0) && MIN_FLOAT_ROUND_LLONG <= v0 && v0 <= MAX_FLOAT_ROUND_LLONG )
	    return round_f2ll(v0);
	else return this->tnull_longlong_rec;
    }
    else if ( this->type_rec == FITS::DOUBLE_T ) {
	double v0 = this->double_value(row_index);
	if ( isfinite(v0) && MIN_DOUBLE_ROUND_LLONG <= v0 && v0 <= MAX_DOUBLE_ROUND_LLONG )
	    return round_d2ll(v0);
	else return this->tnull_longlong_rec;
    }
    else if ( this->type_rec == FITS::STRING_T ) {
	tstring sval;
	double v0;
	this->get_string_value(row_index, &sval);
	if ( sval.length() == 0 && this->tnull_is_set_rec == false ) {
	    return this->tnull_longlong_rec;
	}
	else if ( this->tnull_is_set_rec == true &&
		  this->tany.at_cs(TNULL_IDX).strcmp(sval.cstr()) == 0 ) {
	    return this->tnull_longlong_rec;
	}
	REMOVE_SPC(sval);
	v0 = sval.atof();
	if ( isfinite(v0) && MIN_DOUBLE_ROUND_LLONG <= v0 && v0 <= MAX_DOUBLE_ROUND_LLONG )
	    return round_d2ll(v0);
	else return this->tnull_longlong_rec;
    }
 invalid:
    return this->tnull_longlong_rec;
}

/**
 * @brief  セルの生の値を整数値(bit型)で返す (低レベル)
 *
 *  セルの生の値を整数値(bit型)で返します．<br>
 *  カラムの型が FITS::BIT_T (TFORMn の指定に 'X' を含む) の場合に使用する
 *  メンバ関数で，任意のビット要素から右方向に任意のビット数の整数値を取り出せ
 *  ます．<br>
 *  カラムの型が FITS::BIT_T 以外の場合に返される NULL値は，ヘッダの TNULLn の
 *  値が存在する場合はその値，存在しない場合は INDEF_LONG です．
 *
 * @param   row_index 行インデックス
 * @param   elem_name 要素名 (TELEMn に存在する名前)
 * @param   repetition_idx  TDIMn の2次元目のインデックス
 * @param   nbit ビット幅 (省略時は TELEMn の値のビットフィールド指定を適用)
 * @return  セルの値: 正常終了．<br>
 *          NULL値: 引数が不正な場合
 * @attention  可変長配列はサポートされていません．
 * @note  ビット演算を行なうため，実行コストが大きいかもしれません．
 */
long long fits_table_col::bit_value( long row_index, const char *elem_name,
				     long repetition_idx, int nbit ) const
{
    long elem_index = this->elem_index_rec.index(elem_name,0);
    return this->bit_value( row_index, elem_index, repetition_idx, nbit );
}

/**
 * @brief  セルの生の値を整数値(bit型)で返す (低レベル)
 *
 *  セルの生の値を整数値(bit型)で返します．<br>
 *  カラムの型が FITS::BIT_T (TFORMn の指定に 'X' を含む) の場合に使用する
 *  メンバ関数で，任意のビット要素から右方向に任意のビット数の整数値を取り出せ
 *  ます．<br>
 *  カラムの型が FITS::BIT_T 以外の場合に返される NULL値は，ヘッダの TNULLn の
 *  値が存在する場合はその値，存在しない場合は INDEF_LONG です．
 *
 * @param   row_index 行インデックス
 * @param   elem_index 要素インデックス (TDIMn の1次元目のインデックス)
 * @param   repetition_idx  TDIMn の2次元目のインデックス
 * @param   nbit ビット幅 (省略時は 1 を適用)
 * @return  セルの値: 正常終了．<br>
 *          NULL値: 引数が不正な場合
 * @attention  可変長配列はサポートされていません．
 * @note  ビット演算を行なうため，実行コストが大きいかもしれません．
 */
long long fits_table_col::bit_value( long row_index,
				     long elem_index, long repetition_idx,
				     int nbit ) const
{
    if ( row_index < 0 || this->row_size_rec <= row_index ) goto invalid;
    if ( elem_index < 0 || repetition_idx < 0 ) goto invalid;

    if ( this->type_rec == FITS::BIT_T ) {
	const unsigned char *d_ptr = (const unsigned char *)this->data_ptr();
	long e_idx;
	int n_bit;
	long e_mod, e_div, e_align;
	long long ret_val = 0;
	int i;
	if ( d_ptr == NULL ) goto invalid;
	e_idx = this->dcol_size_rec * repetition_idx 
	    + elem_index;
	if ( nbit < 1 ) 
	    n_bit = this->bit_size_telem.i_cs(elem_index % this->dcol_size_rec);
	else
	    n_bit = nbit;
	if ( this->elem_size_rec <= e_idx ) {
	    e_idx = this->elem_size_rec - this->dcol_size_rec 
		+ elem_index % this->dcol_size_rec;
	}
	d_ptr += this->full_bytes_rec * row_index;
	e_align = (n_bit-1) % 8;
	e_mod = e_idx % 8;
	e_div = e_idx / 8;
	    /*  
	     *   [case A]
	     *
	     *   01234567 01234567
	     *   xxxXXXXx xxxxxxxx  e_mod:3  n_bit:4
	     *      3210            e_align:3
	     *
	     *   01234567 01234567
	     *   xxxXYYYY YYYYxxxx  e_mod:3  n_bit:9
	     *      07654 3210      e_align:0 n_shift:4
	     *
	     *   01234567 01234567
	     *   xxxxXXYY YYYYYYxx  e_mod:4  n_bit:10
	     *       1076 543210    e_align:1 n_shift:2
	     *
	     *   [case B]
	     *
	     *   01234567 01234567
	     *   xxxxxxxY Yxxxxxxx  e_mod:7  n_bit:2
	     *          1 0         e_align:1
	     *
	     *   01234567 01234567
	     *   xxxxxYYY YYYYxxxx  e_mod:5  n_bit:7
	     *        654 3210      e_align:6 n_shift:4
	     *
	     *   01234567 01234567
	     *   xxxxxYYY YYYYYxxx  e_mod:5  n_bit:8
	     *        765 43210     e_align:7 n_shift:5
	     */
	if ( e_mod + e_align < 8 ) {		/* case A : 右へシフトする */
	    int n_shift = 7 - (e_mod + e_align);	/* シフトするbit数 */
	    unsigned char mask;
	    i = n_bit;					/* 未登録bit数 */
	    mask = (unsigned char)0xff >> e_mod;
	    ret_val = (d_ptr[e_div] & mask) >> n_shift;	i -= 1 + e_align;
	    while ( 0 < i ) {
		ret_val <<= 8;
		ret_val |= d_ptr[e_div] << (8-n_shift);	i -= n_shift;
		e_div ++;
		ret_val |= d_ptr[e_div] >> n_shift;	i -= 8 - n_shift;
	    }
	}
	else {					/* case B : 左へシフトする */
	    int n_shift = e_mod + e_align - 7;		/* シフトするbit数 */
	    unsigned char mask;
	    i = n_bit;
	    mask = (unsigned char)0xff >> e_mod;
	    ret_val= (d_ptr[e_div] & mask) << n_shift;	i -= 8 - e_mod;
	    e_div ++;
	    ret_val |= d_ptr[e_div] >> (8-n_shift);	i -= n_shift;
	    while ( 0 < i ) {
		ret_val <<= 8;
		ret_val |= d_ptr[e_div] << n_shift;	i -= 8 - n_shift;
		e_div ++;
		ret_val |= d_ptr[e_div] >> (8-n_shift);	i -= n_shift;
	    }
	}
	return ret_val;
    }
    else if ( this->type_rec == FITS::BYTE_T ) {
	return this->byte_value(row_index,elem_index,repetition_idx);
    }
    else if ( this->type_rec == FITS::LOGICAL_T ) {
	int v = this->logical_value(row_index,elem_index,repetition_idx);
	if ( v == 'T' ) return 1;
	else if ( v == 'F' ) return 0;
	else return this->tnull_longlong_rec;
    }
    else if ( this->type_rec == FITS::SHORT_T ) {
	return this->short_value(row_index,elem_index,repetition_idx);
    }
    else if ( this->type_rec == FITS::LONG_T ) {
	return this->long_value(row_index,elem_index,repetition_idx);
    }
    else if ( this->type_rec == FITS::LONGLONG_T ) {
	return this->longlong_value(row_index,elem_index,repetition_idx);
    }
    else if ( this->type_rec == FITS::FLOAT_T ) {
	float v0 = this->float_value(row_index,elem_index,repetition_idx);
	if ( isfinite(v0) && MIN_FLOAT_ROUND_LLONG <= v0 && v0 <= MAX_FLOAT_ROUND_LLONG )
	    return round_f2ll(v0);
	else return this->tnull_longlong_rec;
    }
    else if ( this->type_rec == FITS::DOUBLE_T ) {
	double v0 = this->double_value(row_index,elem_index,repetition_idx);
	if ( isfinite(v0) && MIN_DOUBLE_ROUND_LLONG <= v0 && v0 <= MAX_DOUBLE_ROUND_LLONG )
	    return round_d2ll(v0);
	else return this->tnull_longlong_rec;
    }
    else if ( this->type_rec == FITS::STRING_T ) {
	tstring sval;
	double v0;
	this->get_string_value(row_index,elem_index,repetition_idx, &sval);
	if ( sval.length() == 0 && this->tnull_is_set_rec == false ) {
	    return this->tnull_longlong_rec;
	}
	else if ( this->tnull_is_set_rec == true &&
		  this->tany.at_cs(TNULL_IDX).strcmp(sval.cstr()) == 0 ) {
	    return this->tnull_longlong_rec;
	}
	REMOVE_SPC(sval);
	v0 = sval.atof();
	if ( isfinite(v0) && MIN_DOUBLE_ROUND_LLONG <= v0 && v0 <= MAX_DOUBLE_ROUND_LLONG )
	    return round_d2ll(v0);
	else return this->tnull_longlong_rec;
    }
 invalid:
    return this->tnull_longlong_rec;
}


/* string */
/**
 * @brief  セルの生の値を文字列値で取得 (低レベル)
 *
 *  セルの生の値を文字列値で返します．<br>
 *  カラムの型が FITS::ASCII_T (TFORMn の指定に 'A' を含む) の場合に最速で
 *  アクセスできます．<br>
 *  カラムの型が FITS::ASCII_T の場合，セルが「文字列の低レベルなNULL値」なら
 *  このメンバ関数のNULL値として "" を返します(TNULLn との比較は行ないません)．
 *  FITS::ASCII_T 以外の場合，セルがFITS規約でのNULL値であれば，このメンバ関数
 *  の NULL値として "" を返します．
 *
 * @param   row_index 行インデックス
 * @return  セルの値: 正常終了．<br>
 *          NULL値: 引数が不正な場合
 * @attention  可変長配列はサポートされていません．
 * @note  SFITSIO では，カラム型が FITS::ASCII_T (TFORMn の指定に 'A' を含む) 
 *        の場合，TNULLn の設定がなければ，先頭文字が '\0' のセルを NULL値とし
 *        ます．これを「文字列の低レベルなNULL値」と呼んでいます．
 */
const char *fits_table_col::string_value( long row_index )
{
    if ( row_index < 0 || this->row_size_rec <= row_index ) goto invalid;

    if ( this->type_rec == FITS::STRING_T ) {
	const char *d_ptr = (const char *)this->data_ptr();
	if ( d_ptr == NULL ) goto invalid;
	this->str_buf->assign(d_ptr +
		this->bytes_rec * this->elem_size_rec * row_index,
		this->bytes_rec);
	return this->str_buf->cstr();
    }
    else if ( this->type_rec == FITS::LOGICAL_T ) {
	int v = this->logical_value(row_index);
	if ( v == 'T' ) this->str_buf->assign("T");
	else if ( v == 'F' ) this->str_buf->assign("F");
	else this->str_buf->assign("");
	return this->str_buf->cstr();
    }
    else if ( this->type_rec == FITS::LONG_T ) {
	long v = this->long_value(row_index);
	if ( this->tnull_is_set_rec == true && this->tnull_r_rec == v )
	    this->str_buf->assign("");
	else this->str_buf->printf("%ld",v);
	return this->str_buf->cstr();
    }
    else if ( this->type_rec == FITS::LONGLONG_T ) {
	long long v = this->longlong_value(row_index);
	if ( this->tnull_is_set_rec == true && this->tnull_r_rec == v )
	    this->str_buf->assign("");
	else this->str_buf->printf("%lld",v);
	return this->str_buf->cstr();
    }
    else if ( this->type_rec == FITS::SHORT_T ) {
	short v = this->short_value(row_index);
	if ( this->tnull_is_set_rec == true && this->tnull_r_rec == v )
	    this->str_buf->assign("");
	else this->str_buf->printf("%hd",v);
	return this->str_buf->cstr();
    }
    else if ( this->type_rec == FITS::DOUBLE_T ) {
	double v = this->double_value(row_index);
	if ( !isfinite(v) ) this->str_buf->assign("");
	else this->str_buf->printf("%.15G",v);
	return this->str_buf->cstr();
    }
    else if ( this->type_rec == FITS::FLOAT_T ) {
	float v = this->float_value(row_index);
	if ( !isfinite(v) ) this->str_buf->assign("");
	else this->str_buf->printf("%.7G",(double)v);
	return this->str_buf->cstr();
    }
    else if ( this->type_rec == FITS::BYTE_T ) {
	unsigned char v = this->byte_value(row_index);
	if ( this->tnull_is_set_rec == true && this->tnull_r_rec == v )
	    this->str_buf->assign("");
	else this->str_buf->printf("%hhu",v);
	return this->str_buf->cstr();
    }
    else if ( this->type_rec == FITS::BIT_T ) {
	long long v = this->bit_value(row_index);
	this->str_buf->printf("%lld",v);
	return this->str_buf->cstr();
    }
 invalid:
    return "";
}

/**
 * @brief  セルの生の値を文字列値で取得 (低レベル)
 *
 *  セルの生の値を文字列値で返します．<br>
 *  カラムの型が FITS::ASCII_T (TFORMn の指定に 'A' を含む) の場合に最速で
 *  アクセスできます．<br>
 *  カラムの型が FITS::ASCII_T の場合，セルが「文字列の低レベルなNULL値」なら
 *  このメンバ関数のNULL値として "" を返します(TNULLn との比較は行ないません)．
 *  FITS::ASCII_T 以外の場合，セルがFITS規約でのNULL値であれば，このメンバ関数
 *  の NULL値として "" を返します．
 *
 * @param   row_index 行インデックス
 * @param   elem_name 要素名 (TELEMn に存在する名前)
 * @param   repetition_idx  TDIMn の2次元目のインデックス
 * @return  セルの値: 正常終了．<br>
 *          NULL値: 引数が不正な場合
 * @attention  可変長配列はサポートされていません．
 * @note  SFITSIO では，カラム型が FITS::ASCII_T (TFORMn の指定に 'A' を含む) 
 *        の場合，TNULLn の設定がなければ，先頭文字が '\0' のセルを NULL値とし
 *        ます．これを「文字列の低レベルなNULL値」と呼んでいます．
 */
const char *fits_table_col::string_value( long row_index,
					  const char *elem_name, 
					  long repetition_idx )
{
    long elem_index = this->elem_index_rec.index(elem_name,0);
    if ( this->type_rec == FITS::BIT_T ) {
	long long v;
	if ( row_index < 0 || this->row_size_rec <= row_index ) return "";
	if ( elem_index < 0 || repetition_idx < 0 ) return "";
	v = this->bit_value(row_index,elem_index,repetition_idx,0);
	this->str_buf->printf("%lld",v);
	return this->str_buf->cstr();
    }
    else {
	return this->string_value( row_index, elem_index, repetition_idx );
    }
}

/**
 * @brief  セルの生の値を文字列値で取得 (低レベル)
 *
 *  セルの生の値を文字列値で返します．<br>
 *  カラムの型が FITS::ASCII_T (TFORMn の指定に 'A' を含む) の場合に最速で
 *  アクセスできます．<br>
 *  カラムの型が FITS::ASCII_T の場合，セルが「文字列の低レベルなNULL値」なら
 *  このメンバ関数のNULL値として "" を返します(TNULLn との比較は行ないません)．
 *  FITS::ASCII_T 以外の場合，セルがFITS規約でのNULL値であれば，このメンバ関数
 *  の NULL値として "" を返します．
 *
 * @param   row_index 行インデックス
 * @param   elem_index 要素インデックス (TDIMn の1次元目のインデックス)
 * @param   repetition_idx  TDIMn の2次元目のインデックス
 * @return  セルの値: 正常終了．<br>
 *          NULL値: 引数が不正な場合
 * @attention  可変長配列はサポートされていません．
 * @note  SFITSIO では，カラム型が FITS::ASCII_T (TFORMn の指定に 'A' を含む) 
 *        の場合，TNULLn の設定がなければ，先頭文字が '\0' のセルを NULL値とし
 *        ます．これを「文字列の低レベルなNULL値」と呼んでいます．
 */
const char *fits_table_col::string_value( long row_index,
					 long elem_index, long repetition_idx )
{
    if ( row_index < 0 || this->row_size_rec <= row_index ) goto invalid;
    if ( elem_index < 0 || repetition_idx < 0 ) goto invalid;

    if ( this->type_rec == FITS::STRING_T ) {
	const char *d_ptr = (const char *)this->data_ptr();
	long e_idx;
	if ( d_ptr == NULL ) goto invalid;
	e_idx = this->dcol_size_rec * repetition_idx 
	    + elem_index;
	if ( this->elem_size_rec <= e_idx ) 
	    e_idx = this->elem_size_rec - this->dcol_size_rec 
		+ elem_index % this->dcol_size_rec;
	this->str_buf->assign(d_ptr + 
	      this->bytes_rec * (this->elem_size_rec * row_index +e_idx ),
	      this->bytes_rec);
	return this->str_buf->cstr();
    }
    else if ( this->type_rec == FITS::LOGICAL_T ) {
	int v = this->logical_value(row_index,elem_index,repetition_idx);
	if ( v == 'T' ) this->str_buf->assign("T");
	else if ( v == 'F' ) this->str_buf->assign("F");
	else this->str_buf->assign("");
	return this->str_buf->cstr();
    }
    else if ( this->type_rec == FITS::LONG_T ) {
	long v = this->long_value(row_index,elem_index,repetition_idx);
	if ( this->tnull_is_set_rec == true && this->tnull_r_rec == v )
	    this->str_buf->assign("");
	else this->str_buf->printf("%ld",v);
	return this->str_buf->cstr();
    }
    else if ( this->type_rec == FITS::LONGLONG_T ) {
	long long v=this->longlong_value(row_index,elem_index,repetition_idx);
	if ( this->tnull_is_set_rec == true && this->tnull_r_rec == v )
	    this->str_buf->assign("");
	else this->str_buf->printf("%lld",v);
	return this->str_buf->cstr();
    }
    else if ( this->type_rec == FITS::SHORT_T ) {
	short v = this->short_value(row_index,elem_index,repetition_idx);
	if ( this->tnull_is_set_rec == true && this->tnull_r_rec == v )
	    this->str_buf->assign("");
	else this->str_buf->printf("%hd",v);
	return this->str_buf->cstr();
    }
    else if ( this->type_rec == FITS::DOUBLE_T ) {
	double v = this->double_value(row_index,elem_index,repetition_idx);
	if ( !isfinite(v) ) this->str_buf->assign("");
	else this->str_buf->printf("%.15G",v);
	return this->str_buf->cstr();
    }
    else if ( this->type_rec == FITS::FLOAT_T ) {
	float v = this->float_value(row_index,elem_index,repetition_idx);
	if ( !isfinite(v) ) this->str_buf->assign("");
	else this->str_buf->printf("%.7G",(double)v);
	return this->str_buf->cstr();
    }
    else if ( this->type_rec == FITS::BYTE_T ) {
	unsigned char v =this->byte_value(row_index,elem_index,repetition_idx);
	if ( this->tnull_is_set_rec == true && this->tnull_r_rec == v )
	    this->str_buf->assign("");
	else this->str_buf->printf("%hhu",v);
	return this->str_buf->cstr();
    }
    else if ( this->type_rec == FITS::BIT_T ) {
	long long v = this->bit_value(row_index,elem_index,repetition_idx);
	this->str_buf->printf("%lld",v);
	return this->str_buf->cstr();
    }
 invalid:
    return "";
}

/**
 * @brief  セルの生の値を文字列値で取得 (低レベル・tstringで取得)
 *
 *  セルの生の値を文字列値で返します．<br>
 *  カラムの型が FITS::ASCII_T (TFORMn の指定に 'A' を含む) の場合に最速で
 *  アクセスできます．<br>
 *  カラムの型が FITS::ASCII_T の場合，セルが「文字列の低レベルなNULL値」なら
 *  このメンバ関数のNULL値として "" を返します(TNULLn との比較は行ないません)．
 *  FITS::ASCII_T 以外の場合，セルがFITS規約でのNULL値であれば，このメンバ関数
 *  の NULL値として "" を返します．
 *
 * @param   row_index 行インデックス
 * @param   dest セル値の格納先 (return)
 * @return  セルの値: 正常終了．<br>
 *          NULL値: 引数が不正な場合．<br>
 *          NULL: dest が NULL の場合
 * @attention  可変長配列はサポートされていません．
 * @note  SFITSIO では，カラム型が FITS::ASCII_T (TFORMn の指定に 'A' を含む) 
 *        の場合，TNULLn の設定がなければ，先頭文字が '\0' のセルを NULL値とし
 *        ます．これを「文字列の低レベルなNULL値」と呼んでいます．
 */
const char *fits_table_col::get_string_value( long row_index,
					      tstring *dest ) const
{
    if ( dest == NULL ) return NULL;

    if ( row_index < 0 || this->row_size_rec <= row_index ) goto invalid;

    if ( this->type_rec == FITS::STRING_T ) {
	const char *d_ptr = (const char *)this->data_ptr();
	if ( d_ptr == NULL ) goto invalid;
	dest->assign(d_ptr + 
		    this->bytes_rec * this->elem_size_rec * row_index,
		    this->bytes_rec);
	return dest->cstr();
    }
    else if ( this->type_rec == FITS::LOGICAL_T ) {
	int v = this->logical_value(row_index);
	if ( v == 'T' ) dest->assign("T");
	else if ( v == 'F' ) dest->assign("F");
	else dest->assign("");
	return dest->cstr();
    }
    else if ( this->type_rec == FITS::LONG_T ) {
	long v = this->long_value(row_index);
	if ( this->tnull_is_set_rec == true && this->tnull_r_rec == v )
	    dest->assign("");
	else dest->printf("%ld",v);
	return dest->cstr();
    }
    else if ( this->type_rec == FITS::LONGLONG_T ) {
	long long v = this->longlong_value(row_index);
	if ( this->tnull_is_set_rec == true && this->tnull_r_rec == v )
	    dest->assign("");
	else dest->printf("%lld",v);
	return dest->cstr();
    }
    else if ( this->type_rec == FITS::SHORT_T ) {
	short v = this->short_value(row_index);
	if ( this->tnull_is_set_rec == true && this->tnull_r_rec == v )
	    dest->assign("");
	else dest->printf("%hd",v);
	return dest->cstr();
    }
    else if ( this->type_rec == FITS::DOUBLE_T ) {
	double v = this->double_value(row_index);
	if ( !isfinite(v) ) dest->assign("");
	else dest->printf("%.15G",v);
	return dest->cstr();
    }
    else if ( this->type_rec == FITS::FLOAT_T ) {
	float v = this->float_value(row_index);
	if ( !isfinite(v) ) dest->assign("");
	else dest->printf("%.7G",(double)v);
	return dest->cstr();
    }
    else if ( this->type_rec == FITS::BYTE_T ) {
	unsigned char v = this->byte_value(row_index);
	if ( this->tnull_is_set_rec == true && this->tnull_r_rec == v )
	    dest->assign("");
	else dest->printf("%hhu",v);
	return dest->cstr();
    }
    else if ( this->type_rec == FITS::BIT_T ) {
	long long v = this->bit_value(row_index);
	dest->printf("%lld",v);
	return dest->cstr();
    }
 invalid:
    dest->assign("");
    return dest->cstr();
}

/**
 * @brief  セルの生の値を文字列値で取得 (低レベル・tstringで取得)
 *
 *  セルの生の値を文字列値で返します．<br>
 *  カラムの型が FITS::ASCII_T (TFORMn の指定に 'A' を含む) の場合に最速で
 *  アクセスできます．<br>
 *  カラムの型が FITS::ASCII_T の場合，セルが「文字列の低レベルなNULL値」なら
 *  このメンバ関数のNULL値として "" を返します(TNULLn との比較は行ないません)．
 *  FITS::ASCII_T 以外の場合，セルがFITS規約でのNULL値であれば，このメンバ関数
 *  の NULL値として "" を返します．
 *
 * @param   row_index 行インデックス
 * @param   elem_index 要素インデックス (TDIMn の1次元目のインデックス)
 * @param   dest セル値の格納先 (return)
 * @return  セルの値: 正常終了．<br>
 *          NULL値: 引数が不正な場合．<br>
 *          NULL: dest が NULL の場合
 * @attention  可変長配列はサポートされていません．
 * @note  SFITSIO では，カラム型が FITS::ASCII_T (TFORMn の指定に 'A' を含む) 
 *        の場合，TNULLn の設定がなければ，先頭文字が '\0' のセルを NULL値とし
 *        ます．これを「文字列の低レベルなNULL値」と呼んでいます．
 */
const char *fits_table_col::get_string_value( long row_index,
					 long elem_index, tstring *dest ) const
{
    if ( dest == NULL ) return NULL;

    return this->get_string_value( row_index, elem_index, 0, dest );
}

/**
 * @brief  セルの生の値を文字列値で取得 (低レベル・tstringで取得)
 *
 *  セルの生の値を文字列値で返します．<br>
 *  カラムの型が FITS::ASCII_T (TFORMn の指定に 'A' を含む) の場合に最速で
 *  アクセスできます．<br>
 *  カラムの型が FITS::ASCII_T の場合，セルが「文字列の低レベルなNULL値」なら
 *  このメンバ関数のNULL値として "" を返します(TNULLn との比較は行ないません)．
 *  FITS::ASCII_T 以外の場合，セルがFITS規約でのNULL値であれば，このメンバ関数
 *  の NULL値として "" を返します．
 *
 * @param   row_index 行インデックス
 * @param   elem_index 要素インデックス (TDIMn の1次元目のインデックス)
 * @param   repetition_idx  TDIMn の2次元目のインデックス
 * @param   dest セル値の格納先 (return)
 * @return  セルの値: 正常終了．<br>
 *          NULL値: 引数が不正な場合．<br>
 *          NULL: dest が NULL の場合
 * @attention  可変長配列はサポートされていません．
 * @note  SFITSIO では，カラム型が FITS::ASCII_T (TFORMn の指定に 'A' を含む) 
 *        の場合，TNULLn の設定がなければ，先頭文字が '\0' のセルを NULL値とし
 *        ます．これを「文字列の低レベルなNULL値」と呼んでいます．
 */
const char *fits_table_col::get_string_value( long row_index,
		    long elem_index, long repetition_idx, tstring *dest ) const
{
    if ( dest == NULL ) return NULL;

    if ( row_index < 0 || this->row_size_rec <= row_index ) goto invalid;
    if ( elem_index < 0 || repetition_idx < 0 ) goto invalid;

    if ( this->type_rec == FITS::STRING_T ) {
	const char *d_ptr = (const char *)this->data_ptr();
	long e_idx;
	if ( d_ptr == NULL ) goto invalid;
	e_idx = this->dcol_size_rec * repetition_idx 
	    + elem_index;
	if ( this->elem_size_rec <= e_idx ) 
	    e_idx = this->elem_size_rec - this->dcol_size_rec 
		+ elem_index % this->dcol_size_rec;
	dest->assign(d_ptr + 
	     this->bytes_rec * (this->elem_size_rec * row_index + e_idx ),
	     this->bytes_rec);
	return dest->cstr();
    }
    else if ( this->type_rec == FITS::LOGICAL_T ) {
	int v = this->logical_value(row_index,elem_index,repetition_idx);
	if ( v == 'T' ) dest->assign("T");
	else if ( v == 'F' ) dest->assign("F");
	else dest->assign("");
	return dest->cstr();
    }
    else if ( this->type_rec == FITS::LONG_T ) {
	long v = this->long_value(row_index,elem_index,repetition_idx);
	if ( this->tnull_is_set_rec == true && this->tnull_r_rec == v )
	    dest->assign("");
	else dest->printf("%ld",v);
	return dest->cstr();
    }
    else if ( this->type_rec == FITS::LONGLONG_T ) {
	long long v=this->longlong_value(row_index,elem_index,repetition_idx);
	if ( this->tnull_is_set_rec == true && this->tnull_r_rec == v )
	    dest->assign("");
	else dest->printf("%lld",v);
	return dest->cstr();
    }
    else if ( this->type_rec == FITS::SHORT_T ) {
	short v = this->short_value(row_index,elem_index,repetition_idx);
	if ( this->tnull_is_set_rec == true && this->tnull_r_rec == v )
	    dest->assign("");
	else dest->printf("%hd",v);
	return dest->cstr();
    }
    else if ( this->type_rec == FITS::DOUBLE_T ) {
	double v = this->double_value(row_index,elem_index,repetition_idx);
	if ( !isfinite(v) ) dest->assign("");
	else dest->printf("%.15G",v);
	return dest->cstr();
    }
    else if ( this->type_rec == FITS::FLOAT_T ) {
	float v = this->float_value(row_index,elem_index,repetition_idx);
	if ( !isfinite(v) ) dest->assign("");
	else dest->printf("%.7G",(double)v);
	return dest->cstr();
    }
    else if ( this->type_rec == FITS::BYTE_T ) {
	unsigned char v =this->byte_value(row_index,elem_index,repetition_idx);
	if ( this->tnull_is_set_rec == true && this->tnull_r_rec == v )
	    dest->assign("");
	else dest->printf("%hhu",v);
	return dest->cstr();
    }
    else if ( this->type_rec == FITS::BIT_T ) {
	long long v = this->bit_value(row_index,elem_index,repetition_idx);
	dest->printf("%lld",v);
	return dest->cstr();
    }
 invalid:
    dest->assign("");
    return dest->cstr();
}

/**
 * @brief  セルの生の値を文字列値で取得 (低レベル・tstringで取得)
 *
 *  セルの生の値を文字列値で返します．<br>
 *  カラムの型が FITS::ASCII_T (TFORMn の指定に 'A' を含む) の場合に最速で
 *  アクセスできます．<br>
 *  カラムの型が FITS::ASCII_T の場合，セルが「文字列の低レベルなNULL値」なら
 *  このメンバ関数のNULL値として "" を返します(TNULLn との比較は行ないません)．
 *  FITS::ASCII_T 以外の場合，セルがFITS規約でのNULL値であれば，このメンバ関数
 *  の NULL値として "" を返します．
 *
 * @param   row_index 行インデックス
 * @param   elem_name 要素名 (TELEMn に存在する名前)
 * @param   dest セル値の格納先 (return)
 * @return  セルの値: 正常終了．<br>
 *          NULL値: 引数が不正な場合．<br>
 *          NULL: dest が NULL の場合
 * @attention  可変長配列はサポートされていません．
 * @note  SFITSIO では，カラム型が FITS::ASCII_T (TFORMn の指定に 'A' を含む) 
 *        の場合，TNULLn の設定がなければ，先頭文字が '\0' のセルを NULL値とし
 *        ます．これを「文字列の低レベルなNULL値」と呼んでいます．
 */
const char *fits_table_col::get_string_value( long row_index,
					      const char *elem_name, 
					      tstring *dest ) const
{
    if ( dest == NULL ) return NULL;

    return this->get_string_value( row_index, elem_name, 0, dest );
}

/**
 * @brief  セルの生の値を文字列値で取得 (低レベル・tstringで取得)
 *
 *  セルの生の値を文字列値で返します．<br>
 *  カラムの型が FITS::ASCII_T (TFORMn の指定に 'A' を含む) の場合に最速で
 *  アクセスできます．<br>
 *  カラムの型が FITS::ASCII_T の場合，セルが「文字列の低レベルなNULL値」なら
 *  このメンバ関数のNULL値として "" を返します(TNULLn との比較は行ないません)．
 *  FITS::ASCII_T 以外の場合，セルがFITS規約でのNULL値であれば，このメンバ関数
 *  の NULL値として "" を返します．
 *
 * @param   row_index 行インデックス
 * @param   elem_name 要素名 (TELEMn に存在する名前)
 * @param   repetition_idx  TDIMn の2次元目のインデックス
 * @param   dest セル値の格納先 (return)
 * @return  セルの値: 正常終了．<br>
 *          NULL値: 引数が不正な場合．<br>
 *          NULL: dest が NULL の場合
 * @attention  可変長配列はサポートされていません．
 * @note  SFITSIO では，カラム型が FITS::ASCII_T (TFORMn の指定に 'A' を含む) 
 *        の場合，TNULLn の設定がなければ，先頭文字が '\0' のセルを NULL値とし
 *        ます．これを「文字列の低レベルなNULL値」と呼んでいます．
 */
const char *fits_table_col::get_string_value( long row_index,
					      const char *elem_name, 
					      long repetition_idx,
					      tstring *dest ) const
{
    if ( dest == NULL ) return NULL;

    long elem_index = this->elem_index_rec.index(elem_name,0);
    if ( this->type_rec == FITS::BIT_T ) {
	long long v;
	if ( row_index < 0 || this->row_size_rec <= row_index ) {
	    dest->assign("");
	    return dest->cstr();
	}
	if ( elem_index < 0 || repetition_idx < 0 ) {
	    dest->assign("");
	    return dest->cstr();
	}
	v = this->bit_value(row_index,elem_index,repetition_idx,0);
	dest->printf("%lld",v);
	return dest->cstr();
    }
    else {
	return this->get_string_value( row_index, elem_index, repetition_idx,
				       dest );
    }
}

/**
 * @deprecated  非推奨．<br>
 *              fits_table_col::get_string_value( long, tstring * ) <br>
 *              をお使いください．
 */
const char *fits_table_col::get_string_value( long row_index, 
					      tstring &dest ) const
{
    return this->get_string_value(row_index, &dest);
}

/**
 * @deprecated  非推奨．<br>
 *              fits_table_col::get_string_value( long, long, tstring * ) <br>
 *              をお使いください．
 */
const char *fits_table_col::get_string_value( long row_index,
					 long elem_index, tstring &dest ) const
{
    return this->get_string_value(row_index, elem_index, 0, &dest);
}

/**
 * @deprecated  非推奨．<br>
 *         fits_table_col::get_string_value( long, long, long, tstring * ) <br>
 *         をお使いください．
 */
const char *fits_table_col::get_string_value( long row_index,
		    long elem_index, long repetition_idx, tstring &dest ) const
{
    return 
	this->get_string_value(row_index, elem_index, repetition_idx, &dest);
}

/**
 * @deprecated  非推奨．<br>
 *       fits_table_col::get_string_value( long, const char *, tstring * ) <br>
 *       をお使いください．
 */
const char *fits_table_col::get_string_value( long row_index,
					      const char *elem_name, 
					      tstring &dest ) const
{
    return this->get_string_value(row_index, elem_name, 0, &dest);
}

/**
 * @deprecated  非推奨．<br>
 * fits_table_col::get_string_value( long, const char *, long, tstring * ) <br>
 * をお使いください．
 */
const char *fits_table_col::get_string_value( long row_index,
					      const char *elem_name, 
					      long repetition_idx,
					      tstring &dest ) const
{
    return this->get_string_value(row_index, elem_name, repetition_idx, &dest);
}

/**
 * @brief  セルの生の値を文字列値で取得 (低レベル・ユーザバッファに取得)
 *
 *  セルの生の値を文字列値で返します．<br>
 *  カラムの型が FITS::ASCII_T (TFORMn の指定に 'A' を含む) の場合に最速で
 *  アクセスできます．<br>
 *  カラムの型が FITS::ASCII_T 以外の場合に返される NULL値は，"" です．
 *
 * @param   row_index 行インデックス
 * @param   dest_buf セル値の格納先 (return)
 * @param   buf_size dest_buf のバッファサイズ
 * @return  非負の値: バッファ長が十分な場合にコピーできる文字数
 *                    ('\0'は含まない) <br>
 *          負の値(エラー): 引数が不正でコピーされなかった場合
 * @attention  可変長配列はサポートされていません．
 * @note  SFITSIO では，カラム型が FITS::ASCII_T (TFORMn の指定に 'A' を含む) 
 *        の場合，TNULLn の設定がなければ，先頭文字が '\0' のセルを NULL値とし
 *        ます．これを「文字列の低レベルなNULL値」と呼んでいます．
 */
ssize_t fits_table_col::get_string_value( long row_index, 
					char *dest_buf, size_t buf_size ) const
{
    tstring sval;
    this->get_string_value( row_index, &sval );
    return sval.getstr(dest_buf,buf_size);
}

/**
 * @brief  セルの生の値を文字列値で取得 (低レベル・ユーザバッファに取得)
 *
 *  セルの生の値を文字列値で返します．<br>
 *  カラムの型が FITS::ASCII_T (TFORMn の指定に 'A' を含む) の場合に最速で
 *  アクセスできます．<br>
 *  カラムの型が FITS::ASCII_T の場合，セルが「文字列の低レベルなNULL値」なら
 *  このメンバ関数のNULL値として "" を返します(TNULLn との比較は行ないません)．
 *  FITS::ASCII_T 以外の場合，セルがFITS規約でのNULL値であれば，このメンバ関数
 *  の NULL値として "" を返します．
 *
 * @param   row_index 行インデックス
 * @param   elem_name 要素名 (TELEMn に存在する名前)
 * @param   dest_buf セル値の格納先 (return)
 * @param   buf_size dest_buf のバッファサイズ
 * @return  非負の値: バッファ長が十分な場合にコピーできる文字数
 *                    ('\0'は含まない) <br>
 *          負の値(エラー): 引数が不正でコピーされなかった場合
 * @attention  可変長配列はサポートされていません．
 * @note  SFITSIO では，カラム型が FITS::ASCII_T (TFORMn の指定に 'A' を含む) 
 *        の場合，TNULLn の設定がなければ，先頭文字が '\0' のセルを NULL値とし
 *        ます．これを「文字列の低レベルなNULL値」と呼んでいます．
 */
ssize_t fits_table_col::get_string_value( long row_index,
					const char *elem_name, 
					char *dest_buf, size_t buf_size ) const
{
    tstring sval;
    this->get_string_value( row_index, elem_name, 0, &sval );
    return sval.getstr(dest_buf,buf_size);
}

/**
 * @brief  セルの生の値を文字列値で取得 (低レベル・ユーザバッファに取得)
 *
 *  セルの生の値を文字列値で返します．<br>
 *  カラムの型が FITS::ASCII_T (TFORMn の指定に 'A' を含む) の場合に最速で
 *  アクセスできます．<br>
 *  カラムの型が FITS::ASCII_T の場合，セルが「文字列の低レベルなNULL値」なら
 *  このメンバ関数のNULL値として "" を返します(TNULLn との比較は行ないません)．
 *  FITS::ASCII_T 以外の場合，セルがFITS規約でのNULL値であれば，このメンバ関数
 *  の NULL値として "" を返します．
 *
 * @param   row_index 行インデックス
 * @param   elem_name 要素名 (TELEMn に存在する名前)
 * @param   repetition_idx  TDIMn の2次元目のインデックス
 * @param   dest_buf セル値の格納先 (return)
 * @param   buf_size dest_buf のバッファサイズ
 * @return  非負の値: バッファ長が十分な場合にコピーできる文字数
 *                    ('\0'は含まない) <br>
 *          負の値(エラー): 引数が不正でコピーされなかった場合
 * @attention  可変長配列はサポートされていません．
 * @note  SFITSIO では，カラム型が FITS::ASCII_T (TFORMn の指定に 'A' を含む) 
 *        の場合，TNULLn の設定がなければ，先頭文字が '\0' のセルを NULL値とし
 *        ます．これを「文字列の低レベルなNULL値」と呼んでいます．
 */
ssize_t fits_table_col::get_string_value( long row_index,
				    const char *elem_name, long repetition_idx,
				    char *dest_buf, size_t buf_size ) const
{
    tstring sval;
    this->get_string_value( row_index, elem_name, repetition_idx, &sval );
    return sval.getstr(dest_buf,buf_size);
}

/**
 * @brief  セルの生の値を文字列値で取得 (低レベル・ユーザバッファに取得)
 *
 *  セルの生の値を文字列値で返します．<br>
 *  カラムの型が FITS::ASCII_T (TFORMn の指定に 'A' を含む) の場合に最速で
 *  アクセスできます．<br>
 *  カラムの型が FITS::ASCII_T の場合，セルが「文字列の低レベルなNULL値」なら
 *  このメンバ関数のNULL値として "" を返します(TNULLn との比較は行ないません)．
 *  FITS::ASCII_T 以外の場合，セルがFITS規約でのNULL値であれば，このメンバ関数
 *  の NULL値として "" を返します．
 *
 * @param   row_index 行インデックス
 * @param   elem_index 要素インデックス (TDIMn の1次元目のインデックス)
 * @param   dest_buf セル値の格納先 (return)
 * @param   buf_size dest_buf のバッファサイズ
 * @return  非負の値: バッファ長が十分な場合にコピーできる文字数
 *                    ('\0'は含まない) <br>
 *          負の値(エラー): 引数が不正でコピーされなかった場合
 * @attention  可変長配列はサポートされていません．
 * @note  SFITSIO では，カラム型が FITS::ASCII_T (TFORMn の指定に 'A' を含む) 
 *        の場合，TNULLn の設定がなければ，先頭文字が '\0' のセルを NULL値とし
 *        ます．これを「文字列の低レベルなNULL値」と呼んでいます．
 */
ssize_t fits_table_col::get_string_value( long row_index, long elem_index, 
					char *dest_buf, size_t buf_size ) const
{
    tstring sval;
    this->get_string_value( row_index, elem_index, 0, &sval );
    return sval.getstr(dest_buf,buf_size);
}

/**
 * @brief  セルの生の値を文字列値で取得 (低レベル・ユーザバッファに取得)
 *
 *  セルの生の値を文字列値で返します．<br>
 *  カラムの型が FITS::ASCII_T (TFORMn の指定に 'A' を含む) の場合に最速で
 *  アクセスできます．<br>
 *  カラムの型が FITS::ASCII_T の場合，セルが「文字列の低レベルなNULL値」なら
 *  このメンバ関数のNULL値として "" を返します(TNULLn との比較は行ないません)．
 *  FITS::ASCII_T 以外の場合，セルがFITS規約でのNULL値であれば，このメンバ関数
 *  の NULL値として "" を返します．
 *
 * @param   row_index 行インデックス
 * @param   elem_index 要素インデックス (TDIMn の1次元目のインデックス)
 * @param   repetition_idx  TDIMn の2次元目のインデックス
 * @param   dest_buf セル値の格納先 (return)
 * @param   buf_size dest_buf のバッファサイズ
 * @return  非負の値: バッファ長が十分な場合にコピーできる文字数
 *                    ('\0'は含まない) <br>
 *          負の値(エラー): 引数が不正でコピーされなかった場合
 * @attention  可変長配列はサポートされていません．
 * @note  SFITSIO では，カラム型が FITS::ASCII_T (TFORMn の指定に 'A' を含む) 
 *        の場合，TNULLn の設定がなければ，先頭文字が '\0' のセルを NULL値とし
 *        ます．これを「文字列の低レベルなNULL値」と呼んでいます．
 */
ssize_t fits_table_col::get_string_value( long row_index,
					long elem_index, long repetition_idx,
					char *dest_buf, size_t buf_size ) const
{
    tstring sval;
    this->get_string_value( row_index, elem_index, repetition_idx, &sval );
    return sval.getstr(dest_buf,buf_size);
}


/*
 * high-level APIs to write a cell.
 *  - TSCAL and TZERO are applied.
 *  - NULL value is handled.
 */

/* double arg */

/**
 * @brief  実数値(double型)でセルに値を代入 (高レベル)
 *
 *  実数 value から，ヘッダの TZEROn と TSCALn を反映した実数値を求め，適切な
 *  変換(整数型のカラムの場合は丸め)を行なってセルに代入します．<br>
 *  value に NAN を与えた場合は，NULL値が与えられたものとします．この時，バイ
 *  ナリテーブルの整数型カラムやアスキーテーブルの場合に TNULLn の値があれば
 *  その値をセルに代入します．
 *
 * @param   value 代入する値
 * @param   row_index 行インデックス
 * @return  自身の参照
 * @note  バイナリテーブルの文字列型のカラムでも TNULLn の設定が可能です．これ
 *        は SFITSIO の独自拡張です．
 * @attention  可変長配列はサポートされていません．
 */
fits_table_col &fits_table_col::assign( double value, long row_index )
{
    if ( this->type_rec == FITS::STRING_T ) {
	if ( isfinite(value) ) {
	    if ( 0 < this->fmt_str.length() ) {
		if ( this->tany.at(TDISP_IDX).strpbrk("A") == 0 ) {
		    tstring buf(64);
		    buf.printf("%.15G",value);
		    this->tmp_str_buf->printf(this->fmt_str.cstr(), buf.cstr());
		    return this->assign_string(this->tmp_str_buf->cstr(),
					       row_index);
		}
		double dvalue = value;
		dvalue -= this->tzero_rec;
		dvalue /= this->tscal_rec;
		if ( !isfinite(dvalue) ) goto set_null_str;
		if ( this->tany.at(TDISP_IDX).strpbrk("FEGD") == 0 ) {
		    this->tmp_str_buf->printf(this->fmt_str.cstr(),dvalue);
		    if ( this->tany.cchr(TDISP_IDX,0) == 'D' ) {
			ssize_t ep = this->tmp_str_buf->strchr('E');
			if ( 0 <= ep ) this->tmp_str_buf->at(ep) = 'D';
		    }
		    return this->assign_string(this->tmp_str_buf->cstr(),
					       row_index);
		}
		else if ( this->tany.at(TDISP_IDX).strpbrk("I") == 0 ) {
		    if ( MIN_DOUBLE_ROUND_LLONG <= dvalue && dvalue <= MAX_DOUBLE_ROUND_LLONG ) {
			this->tmp_str_buf->printf(this->fmt_str.cstr(),
						  round_d2ll(dvalue));
			return this->assign_string(this->tmp_str_buf->cstr(),
						   row_index);
		    }
		    else goto set_null_str;
		}
	    }
	}
	else {
	set_null_str:
	    if ( this->tnull_is_set_rec == true ) {
		return this->assign_string(this->tany.cstr(TNULL_IDX),
					   row_index);
	    }
	    else return this->assign_string(NULL, row_index);
	}
	return this->assign_double(value, row_index);
    }
    else if ( this->type_rec == FITS::LOGICAL_T ||
	      this->type_rec == FITS::BIT_T ) {
	return this->assign_double(value, row_index);
    }
    else if ( !isfinite(value) &&
	      ( this->type_rec == FITS::LONG_T ||
		this->type_rec == FITS::SHORT_T ||
		this->type_rec == FITS::BYTE_T ||
		this->type_rec == FITS::LONGLONG_T ) ) {
	return this->assign_longlong(this->tnull_w_rec, row_index);
    }
    else {
	value -= this->tzero_rec;
	value /= this->tscal_rec;
	return this->assign_double(value, row_index);
    }
}

/**
 * @brief  実数値(double型)でセルに値を代入 (高レベル)
 *
 *  実数 value から，ヘッダの TZEROn と TSCALn を反映した実数値を求め，適切な
 *  変換(整数型のカラムの場合は丸め)を行なってセルに代入します．<br>
 *  value に NAN を与えた場合は，NULL値が与えられたものとします．この時，バイ
 *  ナリテーブルの整数型カラムやアスキーテーブルの場合に TNULLn の値があれば
 *  その値をセルに代入します．
 *
 * @param   value 代入する値
 * @param   row_index 行インデックス
 * @param   elem_name 要素名 (TELEMn に存在する名前)
 * @param   repetition_idx  TDIMn の2次元目のインデックス
 * @return  自身の参照
 * @note  バイナリテーブルの文字列型のカラムでも TNULLn の設定が可能です．これ
 *        は SFITSIO の独自拡張です．
 * @attention  可変長配列はサポートされていません．
 */
fits_table_col &fits_table_col::assign( double value, long row_index,
				   const char *elem_name, long repetition_idx )
{
    if ( this->type_rec == FITS::STRING_T ) {
	if ( isfinite(value) ) {
	    if ( 0 < this->fmt_str.length() ) {
		if ( this->tany.at(TDISP_IDX).strpbrk("A") == 0 ) {
		    tstring buf(64);
		    buf.printf("%.15G",value);
		    this->tmp_str_buf->printf(this->fmt_str.cstr(), buf.cstr());
		    return this->assign_string(this->tmp_str_buf->cstr(),
					  row_index, elem_name, repetition_idx);
		}
		double dvalue = value;
		dvalue -= this->tzero_rec;
		dvalue /= this->tscal_rec;
		if ( !isfinite(dvalue) ) goto set_null_str;
		if ( this->tany.at(TDISP_IDX).strpbrk("FEGD") == 0 ) {
		    this->tmp_str_buf->printf(this->fmt_str.cstr(),dvalue);
		    if ( this->tany.cchr(TDISP_IDX,0) == 'D' ) {
			ssize_t ep = this->tmp_str_buf->strchr('E');
			if ( 0 <= ep ) this->tmp_str_buf->at(ep) = 'D';
		    }
		    return this->assign_string(this->tmp_str_buf->cstr(),
				   row_index, elem_name, repetition_idx);
		}
		else if ( this->tany.at(TDISP_IDX).strpbrk("I") == 0 ) {
		    if ( MIN_DOUBLE_ROUND_LLONG <= dvalue && dvalue <= MAX_DOUBLE_ROUND_LLONG ) {
			this->tmp_str_buf->printf(this->fmt_str.cstr(),
						  round_d2ll(dvalue));
			return this->assign_string(this->tmp_str_buf->cstr(),
				       row_index, elem_name, repetition_idx);
		    }
		    else goto set_null_str;
		}
	    }
	}
	else {
	set_null_str:
	    if ( this->tnull_is_set_rec == true ) {
		return this->assign_string(this->tany.cstr(TNULL_IDX),
				row_index, elem_name, repetition_idx);
	    }
	    else return this->assign_string(NULL, row_index,
					    elem_name, repetition_idx);
	}
	return this->assign_double(value, row_index,
				   elem_name, repetition_idx);
    }
    else if ( this->type_rec == FITS::LOGICAL_T ||
	      this->type_rec == FITS::BIT_T ) {
	return this->assign_double(value, row_index,
				   elem_name, repetition_idx);
    }
    else if ( !isfinite(value) &&
	      ( this->type_rec == FITS::LONG_T ||
		this->type_rec == FITS::SHORT_T ||
		this->type_rec == FITS::BYTE_T ||
		this->type_rec == FITS::LONGLONG_T ) ) {
	return this->assign_longlong(this->tnull_w_rec, row_index,
				     elem_name, repetition_idx);
    }
    else {
	value -= this->tzero_rec;
	value /= this->tscal_rec;
	return this->assign_double(value, row_index,
				   elem_name, repetition_idx);
    }
}

/**
 * @brief  実数値(double型)でセルに値を代入 (高レベル)
 *
 *  実数 value から，ヘッダの TZEROn と TSCALn を反映した実数値を求め，適切な
 *  変換(整数型のカラムの場合は丸め)を行なってセルに代入します．<br>
 *  value に NAN を与えた場合は，NULL値が与えられたものとします．この時，バイ
 *  ナリテーブルの整数型カラムやアスキーテーブルの場合に TNULLn の値があれば
 *  その値をセルに代入します．
 *
 * @param   value 代入する値
 * @param   row_index 行インデックス
 * @param   elem_index 要素インデックス (TDIMn の1次元目のインデックス)
 * @param   repetition_idx  TDIMn の2次元目のインデックス
 * @return  自身の参照
 * @note  バイナリテーブルの文字列型のカラムでも TNULLn の設定が可能です．これ
 *        は SFITSIO の独自拡張です．
 * @attention  可変長配列はサポートされていません．
 */
fits_table_col &fits_table_col::assign( double value, long row_index,
					long elem_index, long repetition_idx )
{
    if ( this->type_rec == FITS::STRING_T ) {
	if ( isfinite(value) ) {
	    if ( 0 < this->fmt_str.length() ) {
		if ( this->tany.at(TDISP_IDX).strpbrk("A") == 0 ) {
		    tstring buf(64);
		    buf.printf("%.15G",value);
		    this->tmp_str_buf->printf(this->fmt_str.cstr(), buf.cstr());
		    return this->assign_string(this->tmp_str_buf->cstr(),
					 row_index, elem_index, repetition_idx);
		}
		double dvalue = value;
		dvalue -= this->tzero_rec;
		dvalue /= this->tscal_rec;
		if ( !isfinite(dvalue) ) goto set_null_str;
		if ( this->tany.at(TDISP_IDX).strpbrk("FEGD") == 0 ) {
		    this->tmp_str_buf->printf(this->fmt_str.cstr(),dvalue);
		    if ( this->tany.cchr(TDISP_IDX,0) == 'D' ) {
			ssize_t ep = this->tmp_str_buf->strchr('E');
			if ( 0 <= ep ) this->tmp_str_buf->at(ep) = 'D';
		    }
		    return this->assign_string(this->tmp_str_buf->cstr(),
				  row_index, elem_index, repetition_idx);
		}
		else if ( this->tany.at(TDISP_IDX).strpbrk("I") == 0 ) {
		    if ( MIN_DOUBLE_ROUND_LLONG <= dvalue && dvalue <= MAX_DOUBLE_ROUND_LLONG ) {
			this->tmp_str_buf->printf(this->fmt_str.cstr(),
						  round_d2ll(dvalue));
			return this->assign_string(this->tmp_str_buf->cstr(),
				      row_index, elem_index, repetition_idx);
		    }
		    else goto set_null_str;
		}
	    }
	}
	else {
	set_null_str:
	    if ( this->tnull_is_set_rec == true ) {
		return this->assign_string(this->tany.cstr(TNULL_IDX),
			       row_index, elem_index, repetition_idx);
	    }
	    else return this->assign_string(NULL, row_index,
					    elem_index, repetition_idx);
	}
	return this->assign_double(value, row_index,
				   elem_index, repetition_idx);
    }
    else if ( this->type_rec == FITS::LOGICAL_T ||
	      this->type_rec == FITS::BIT_T ) {
	return this->assign_double(value, row_index,
				   elem_index, repetition_idx);
    }
    else if ( !isfinite(value) &&
	      ( this->type_rec == FITS::LONG_T ||
		this->type_rec == FITS::SHORT_T ||
		this->type_rec == FITS::BYTE_T ||
		this->type_rec == FITS::LONGLONG_T ) ) {
	return this->assign_longlong(this->tnull_w_rec, row_index,
				     elem_index, repetition_idx);
    }
    else {
	value -= this->tzero_rec;
	value /= this->tscal_rec;
	return this->assign_double(value, row_index,
				   elem_index, repetition_idx);
    }
}

/* float arg */

/**
 * @brief  実数値(float型)でセルに値を代入 (高レベル)
 *
 *  実数 value から，ヘッダの TZEROn と TSCALn を反映した実数値を求め，適切な
 *  変換(整数型のカラムの場合は丸め)を行なってセルに代入します．<br>
 *  value に NAN を与えた場合は，NULL値が与えられたものとします．この時，バイ
 *  ナリテーブルの整数型カラムやアスキーテーブルの場合に TNULLn の値があれば
 *  その値をセルに代入します．
 *
 * @param   value 代入する値
 * @param   row_index 行インデックス
 * @return  自身の参照
 * @note  バイナリテーブルの文字列型のカラムでも TNULLn の設定が可能です．これ
 *        は SFITSIO の独自拡張です．
 * @attention  可変長配列はサポートされていません．
 */
fits_table_col &fits_table_col::assign( float value, long row_index )
{
    if ( this->type_rec == FITS::STRING_T ) {
	if ( isfinite(value) ) {
	    if ( 0 < this->fmt_str.length() ) {
		if ( this->tany.at(TDISP_IDX).strpbrk("A") == 0 ) {
		    tstring buf(64);
		    buf.printf("%.7G",(double)value);
		    this->tmp_str_buf->printf(this->fmt_str.cstr(), buf.cstr());
		    return this->assign_string(this->tmp_str_buf->cstr(),
					       row_index);
		}
		double dvalue = value;
		dvalue -= this->tzero_rec;
		dvalue /= this->tscal_rec;
		if ( !isfinite(dvalue) ) goto set_null_str;
		if ( this->tany.at(TDISP_IDX).strpbrk("FEGD") == 0 ) {
		    this->tmp_str_buf->printf(this->fmt_str.cstr(),dvalue);
		    if ( this->tany.cchr(TDISP_IDX,0) == 'D' ) {
			ssize_t ep = this->tmp_str_buf->strchr('E');
			if ( 0 <= ep ) this->tmp_str_buf->at(ep) = 'D';
		    }
		    return this->assign_string(this->tmp_str_buf->cstr(),
					       row_index);
		}
		else if ( this->tany.at(TDISP_IDX).strpbrk("I") == 0 ) {
		    if ( MIN_DOUBLE_ROUND_LLONG <= dvalue && dvalue <= MAX_DOUBLE_ROUND_LLONG ) {
			this->tmp_str_buf->printf(this->fmt_str.cstr(),
						  round_d2ll(dvalue));
			return this->assign_string(this->tmp_str_buf->cstr(),
						   row_index);
		    }
		    else goto set_null_str;
		}
	    }
	}
	else {
	set_null_str:
	    if ( this->tnull_is_set_rec == true ) {
		return this->assign_string(this->tany.cstr(TNULL_IDX),
					   row_index);
	    }
	    else return this->assign_string(NULL, row_index);
	}
	return this->assign_float(value, row_index);
    }
    else if ( this->type_rec == FITS::LOGICAL_T ||
	      this->type_rec == FITS::BIT_T ) {
	return this->assign_double(value, row_index);
    }
    else if ( !isfinite(value) &&
	      ( this->type_rec == FITS::LONG_T ||
		this->type_rec == FITS::SHORT_T ||
		this->type_rec == FITS::BYTE_T ||
		this->type_rec == FITS::LONGLONG_T ) ) {
	return this->assign_longlong(this->tnull_w_rec, row_index);
    }
    else {
	double dvalue = value;
	dvalue -= this->tzero_rec;
	dvalue /= this->tscal_rec;
	return this->assign_double(dvalue, row_index);
    }
}

/**
 * @brief  実数値(float型)でセルに値を代入 (高レベル)
 *
 *  実数 value から，ヘッダの TZEROn と TSCALn を反映した実数値を求め，適切な
 *  変換(整数型のカラムの場合は丸め)を行なってセルに代入します．<br>
 *  value に NAN を与えた場合は，NULL値が与えられたものとします．この時，バイ
 *  ナリテーブルの整数型カラムやアスキーテーブルの場合に TNULLn の値があれば
 *  その値をセルに代入します．
 *
 * @param   value 代入する値
 * @param   row_index 行インデックス
 * @param   elem_name 要素名 (TELEMn に存在する名前)
 * @param   repetition_idx  TDIMn の2次元目のインデックス
 * @return  自身の参照
 * @note  バイナリテーブルの文字列型のカラムでも TNULLn の設定が可能です．これ
 *        は SFITSIO の独自拡張です．
 * @attention  可変長配列はサポートされていません．
 */
fits_table_col &fits_table_col::assign( float value, long row_index,
				   const char *elem_name, long repetition_idx )
{
    if ( this->type_rec == FITS::STRING_T ) {
	if ( isfinite(value) ) {
	    if ( 0 < this->fmt_str.length() ) {
		if ( this->tany.at(TDISP_IDX).strpbrk("A") == 0 ) {
		    tstring buf(64);
		    buf.printf("%.7G",(double)value);
		    this->tmp_str_buf->printf(this->fmt_str.cstr(), buf.cstr());
		    return this->assign_string(this->tmp_str_buf->cstr(),
				          row_index, elem_name, repetition_idx);
		}
		double dvalue = value;
		dvalue -= this->tzero_rec;
		dvalue /= this->tscal_rec;
		if ( !isfinite(dvalue) ) goto set_null_str;
		if ( this->tany.at(TDISP_IDX).strpbrk("FEGD") == 0 ) {
		    this->tmp_str_buf->printf(this->fmt_str.cstr(),dvalue);
		    if ( this->tany.cchr(TDISP_IDX,0) == 'D' ) {
			ssize_t ep = this->tmp_str_buf->strchr('E');
			if ( 0 <= ep ) this->tmp_str_buf->at(ep) = 'D';
		    }
		    return this->assign_string(this->tmp_str_buf->cstr(),
				   row_index, elem_name, repetition_idx);
		}
		else if ( this->tany.at(TDISP_IDX).strpbrk("I") == 0 ) {
		    if ( MIN_DOUBLE_ROUND_LLONG <= dvalue && dvalue <= MAX_DOUBLE_ROUND_LLONG ) {
			this->tmp_str_buf->printf(this->fmt_str.cstr(),
						  round_d2ll(dvalue));
			return this->assign_string(this->tmp_str_buf->cstr(),
				       row_index, elem_name, repetition_idx);
		    }
		    else goto set_null_str;
		}
	    }
	}
	else {
	set_null_str:
	    if ( this->tnull_is_set_rec == true ) {
		return this->assign_string(this->tany.cstr(TNULL_IDX),
				row_index, elem_name, repetition_idx);
	    }
	    else return this->assign_string(NULL, row_index,
					    elem_name, repetition_idx);
	}
	return this->assign_float(value, row_index,
				  elem_name, repetition_idx);
    }
    else if ( this->type_rec == FITS::LOGICAL_T ||
	      this->type_rec == FITS::BIT_T ) {
	return this->assign_double(value, row_index,
				   elem_name, repetition_idx);
    }
    else if ( !isfinite(value) &&
	      ( this->type_rec == FITS::LONG_T ||
		this->type_rec == FITS::SHORT_T ||
		this->type_rec == FITS::BYTE_T ||
		this->type_rec == FITS::LONGLONG_T ) ) {
	return this->assign_longlong(this->tnull_w_rec, row_index,
				     elem_name, repetition_idx);
    }
    else {
	double dvalue = value;
	dvalue -= this->tzero_rec;
	dvalue /= this->tscal_rec;
	return this->assign_double(dvalue, row_index,
				   elem_name, repetition_idx);
    }
}

/**
 * @brief  実数値(double型)でセルに値を代入 (高レベル)
 *
 *  実数 value から，ヘッダの TZEROn と TSCALn を反映した実数値を求め，適切な
 *  変換(整数型のカラムの場合は丸め)を行なってセルに代入します．<br>
 *  value に NAN を与えた場合は，NULL値が与えられたものとします．この時，バイ
 *  ナリテーブルの整数型カラムやアスキーテーブルの場合に TNULLn の値があれば
 *  その値をセルに代入します．
 *
 * @param   value 代入する値
 * @param   row_index 行インデックス
 * @param   elem_index 要素インデックス (TDIMn の1次元目のインデックス)
 * @param   repetition_idx  TDIMn の2次元目のインデックス
 * @return  自身の参照
 * @note  バイナリテーブルの文字列型のカラムでも TNULLn の設定が可能です．これ
 *        は SFITSIO の独自拡張です．
 * @attention  可変長配列はサポートされていません．
 */
fits_table_col &fits_table_col::assign( float value, long row_index,
					long elem_index, long repetition_idx )
{
    if ( this->type_rec == FITS::STRING_T ) {
	if ( isfinite(value) ) {
	    if ( 0 < this->fmt_str.length() ) {
		if ( this->tany.at(TDISP_IDX).strpbrk("A") == 0 ) {
		    tstring buf(64);
		    buf.printf("%.7G",(double)value);
		    this->tmp_str_buf->printf(this->fmt_str.cstr(), buf.cstr());
		    return this->assign_string(this->tmp_str_buf->cstr(),
				         row_index, elem_index, repetition_idx);
		}
		double dvalue = value;
		dvalue -= this->tzero_rec;
		dvalue /= this->tscal_rec;
		if ( !isfinite(dvalue) ) goto set_null_str;
		if ( this->tany.at(TDISP_IDX).strpbrk("FEGD") == 0 ) {
		    this->tmp_str_buf->printf(this->fmt_str.cstr(),dvalue);
		    if ( this->tany.cchr(TDISP_IDX,0) == 'D' ) {
			ssize_t ep = this->tmp_str_buf->strchr('E');
			if ( 0 <= ep ) this->tmp_str_buf->at(ep) = 'D';
		    }
		    return this->assign_string(this->tmp_str_buf->cstr(),
				  row_index, elem_index, repetition_idx);
		}
		else if ( this->tany.at(TDISP_IDX).strpbrk("I") == 0 ) {
		    if ( MIN_DOUBLE_ROUND_LLONG <= dvalue && dvalue <= MAX_DOUBLE_ROUND_LLONG ) {
			this->tmp_str_buf->printf(this->fmt_str.cstr(),
						  round_d2ll(dvalue));
			return this->assign_string(this->tmp_str_buf->cstr(),
				      row_index, elem_index, repetition_idx);
		    }
		    else goto set_null_str;
		}
	    }
	}
	else {
	set_null_str:
	    if ( this->tnull_is_set_rec == true ) {
		return this->assign_string(this->tany.cstr(TNULL_IDX),
			       row_index, elem_index, repetition_idx);
	    }
	    else return this->assign_string(NULL, row_index,
					    elem_index, repetition_idx);
	}
	return this->assign_float(value, row_index,
				  elem_index, repetition_idx);
    }
    else if ( this->type_rec == FITS::LOGICAL_T ||
	      this->type_rec == FITS::BIT_T ) {
	return this->assign_double(value, row_index,
				   elem_index, repetition_idx);
    }
    else if ( !isfinite(value) &&
	      ( this->type_rec == FITS::LONG_T ||
		this->type_rec == FITS::SHORT_T ||
		this->type_rec == FITS::BYTE_T ||
		this->type_rec == FITS::LONGLONG_T ) ) {
	return this->assign_longlong(this->tnull_w_rec, row_index,
				     elem_index, repetition_idx);
    }
    else {
	double dvalue = value;
	dvalue -= this->tzero_rec;
	dvalue /= this->tscal_rec;
	return this->assign_double(dvalue, row_index,
				   elem_index, repetition_idx);
    }
}

/* long long arg */

/**
 * @brief  整数値(long long型)でセルに値を代入 (高レベル)
 *
 *  整数 value から，ヘッダの TZEROn と TSCALn を反映した実数値を求め，適切な
 *  変換(整数型のカラムの場合は丸め)を行なってセルに代入します．<br>
 *  このメンバ関数では NULL値を与える事はできません．NULL値を与える場合は，
 *  fits_table_col::assign(double, long) を使用してください．
 *
 * @param   value 代入する値
 * @param   row_index 行インデックス
 * @return  自身の参照
 * @note  バイナリテーブルの文字列型のカラムでも TNULLn の設定が可能です．これ
 *        は SFITSIO の独自拡張です．
 * @attention  可変長配列はサポートされていません．
 */
fits_table_col &fits_table_col::assign( long long value, long row_index )
{
    if ( this->type_rec == FITS::STRING_T ) {
	if ( 0 < this->fmt_str.length() ) {
	    if ( this->tany.at(TDISP_IDX).strpbrk("A") == 0 ) {
		tstring buf(64);
		buf.printf("%lld",value);
		this->tmp_str_buf->printf(this->fmt_str.cstr(), buf.cstr());
		return this->assign_string(this->tmp_str_buf->cstr(),
					   row_index);
	    }
	    double dval = value;
	    dval -= this->tzero_rec;
	    dval /= this->tscal_rec;
	    if ( isfinite(dval) ) {
		if ( this->tany.at(TDISP_IDX).strpbrk("FEGD") == 0 ) {
		    this->tmp_str_buf->printf(this->fmt_str.cstr(),dval);
		    if ( this->tany.cchr(TDISP_IDX,0) == 'D' ) {
			ssize_t ep = this->tmp_str_buf->strchr('E');
			if ( 0 <= ep ) this->tmp_str_buf->at(ep) = 'D';
		    }
		    return this->assign_string(this->tmp_str_buf->cstr(),
					       row_index);
		}
		else if ( this->tany.at(TDISP_IDX).strpbrk("I") == 0 ) {
		    if ( this->tscal_rec != 1.0 || this->tzero_rec != 0.0 ) {
			if ( MIN_DOUBLE_ROUND_LLONG <= dval && dval <= MAX_DOUBLE_ROUND_LLONG ) {
			    value = round_d2ll(dval);
			}
			else goto set_null_str;
		    }
		    this->tmp_str_buf->printf(this->fmt_str.cstr(),value);
		    return this->assign_string(this->tmp_str_buf->cstr(),
					       row_index);
		}
	    }
	    else {
	    set_null_str:
		if ( this->tnull_is_set_rec == true ) {
		    return this->assign_string(this->tany.cstr(TNULL_IDX),
					       row_index);
		}
		else return this->assign_string(NULL, row_index);
	    }
	}
	return this->assign_longlong(value, row_index);
    }
    else if ( this->type_rec == FITS::LOGICAL_T ||
	      this->type_rec == FITS::BIT_T ) {
	return this->assign_longlong(value, row_index);
    }
    else {
	if ( this->tscal_rec == 1.0 && this->tzero_rec == 0.0 ) {
	    return this->assign_longlong(value, row_index);
	}
	else {
	    double dval = value;
	    dval -= this->tzero_rec;
	    dval /= this->tscal_rec;
	    return this->assign_double(dval, row_index);
	}
    }
}

/**
 * @brief  整数値(long long型)でセルに値を代入 (高レベル)
 *
 *  整数 value から，ヘッダの TZEROn と TSCALn を反映した実数値を求め，適切な
 *  変換(整数型のカラムの場合は丸め)を行なってセルに代入します．<br>
 *  このメンバ関数では NULL値を与える事はできません．NULL値を与える場合は，
 *  fits_table_col::assign(double, long, const char *, long) を使用してくだ
 *  さい．
 *
 * @param   value 代入する値
 * @param   row_index 行インデックス
 * @param   elem_name 要素名 (TELEMn に存在する名前)
 * @param   repetition_idx  TDIMn の2次元目のインデックス
 * @return  自身の参照
 * @note  バイナリテーブルの文字列型のカラムでも TNULLn の設定が可能です．これ
 *        は SFITSIO の独自拡張です．
 * @attention  可変長配列はサポートされていません．
 */
fits_table_col &fits_table_col::assign( long long value, long row_index,
				   const char *elem_name, long repetition_idx )
{
    if ( this->type_rec == FITS::STRING_T ) {
	if ( 0 < this->fmt_str.length() ) {
	    if ( this->tany.at(TDISP_IDX).strpbrk("A") == 0 ) {
		tstring buf(64);
		buf.printf("%lld",value);
		this->tmp_str_buf->printf(this->fmt_str.cstr(), buf.cstr());
		return this->assign_string(this->tmp_str_buf->cstr(),
				      row_index, elem_name, repetition_idx);
	    }
	    double dval = value;
	    dval -= this->tzero_rec;
	    dval /= this->tscal_rec;
	    if ( isfinite(dval) ) {
		if ( this->tany.at(TDISP_IDX).strpbrk("FEGD") == 0 ) {
		    this->tmp_str_buf->printf(this->fmt_str.cstr(),dval);
		    if ( this->tany.cchr(TDISP_IDX,0) == 'D' ) {
			ssize_t ep = this->tmp_str_buf->strchr('E');
			if ( 0 <= ep ) this->tmp_str_buf->at(ep) = 'D';
		    }
		    return this->assign_string(this->tmp_str_buf->cstr(),
				   row_index, elem_name, repetition_idx);
		}
		else if ( this->tany.at(TDISP_IDX).strpbrk("I") == 0 ) {
		    if ( this->tscal_rec != 1.0 || this->tzero_rec != 0.0 ) {
			if ( MIN_DOUBLE_ROUND_LLONG <= dval && dval <= MAX_DOUBLE_ROUND_LLONG ) {
			    value = round_d2ll(dval);
			}
			else goto set_null_str;
		    }
		    this->tmp_str_buf->printf(this->fmt_str.cstr(),value);
		    return this->assign_string(this->tmp_str_buf->cstr(),
				   row_index, elem_name, repetition_idx);
		}
	    }
	    else {
	    set_null_str:
		if ( this->tnull_is_set_rec == true ) {
		    return this->assign_string(this->tany.cstr(TNULL_IDX),
				    row_index, elem_name, repetition_idx);
		}
		else return this->assign_string(NULL, row_index,
						elem_name, repetition_idx);
	    }
	}
	return this->assign_longlong(value, row_index,
				     elem_name, repetition_idx);
    }
    else if ( this->type_rec == FITS::LOGICAL_T ||
	      this->type_rec == FITS::BIT_T ) {
	return this->assign_longlong(value, row_index,
				     elem_name, repetition_idx);
    }
    else {
	if ( this->tscal_rec == 1.0 && this->tzero_rec == 0.0 ) {
	    return this->assign_longlong(value, row_index,
					 elem_name, repetition_idx);
	}
	else {
	    double dval = value;
	    dval -= this->tzero_rec;
	    dval /= this->tscal_rec;
	    return this->assign_double(dval, row_index,
				       elem_name, repetition_idx);
	}
    }
}

/**
 * @brief  整数値(long long型)でセルに値を代入 (高レベル)
 *
 *  整数 value から，ヘッダの TZEROn と TSCALn を反映した実数値を求め，適切な
 *  変換(整数型のカラムの場合は丸め)を行なってセルに代入します．<br>
 *  このメンバ関数では NULL値を与える事はできません．NULL値を与える場合は，
 *  fits_table_col::assign(double, long, long, long) を使用してください．
 *
 * @param   value 代入する値
 * @param   row_index 行インデックス
 * @param   elem_index 要素インデックス (TDIMn の1次元目のインデックス)
 * @param   repetition_idx  TDIMn の2次元目のインデックス
 * @return  自身の参照
 * @note  バイナリテーブルの文字列型のカラムでも TNULLn の設定が可能です．これ
 *        は SFITSIO の独自拡張です．
 * @attention  可変長配列はサポートされていません．
 */
fits_table_col &fits_table_col::assign( long long value, long row_index,
					long elem_index, long repetition_idx )
{
    if ( this->type_rec == FITS::STRING_T ) {
	if ( 0 < this->fmt_str.length() ) {
	    if ( this->tany.at(TDISP_IDX).strpbrk("A") == 0 ) {
		tstring buf(64);
		buf.printf("%lld",value);
		this->tmp_str_buf->printf(this->fmt_str.cstr(), buf.cstr());
		return this->assign_string(this->tmp_str_buf->cstr(),
				     row_index, elem_index, repetition_idx);
	    }
	    double dval = value;
	    dval -= this->tzero_rec;
	    dval /= this->tscal_rec;
	    if ( isfinite(dval) ) {
		if ( this->tany.at(TDISP_IDX).strpbrk("FEGD") == 0 ) {
		    this->tmp_str_buf->printf(this->fmt_str.cstr(),dval);
		    if ( this->tany.cchr(TDISP_IDX,0) == 'D' ) {
			ssize_t ep = this->tmp_str_buf->strchr('E');
			if ( 0 <= ep ) this->tmp_str_buf->at(ep) = 'D';
		    }
		    return this->assign_string(this->tmp_str_buf->cstr(),
				  row_index, elem_index, repetition_idx);
		}
		else if ( this->tany.at(TDISP_IDX).strpbrk("I") == 0 ) {
		    if ( this->tscal_rec != 1.0 || this->tzero_rec != 0.0 ) {
			if ( MIN_DOUBLE_ROUND_LLONG <= dval && dval <= MAX_DOUBLE_ROUND_LLONG ) {
			    value = round_d2ll(dval);
			}
			else goto set_null_str;
		    }
		    this->tmp_str_buf->printf(this->fmt_str.cstr(),value);
		    return this->assign_string(this->tmp_str_buf->cstr(),
				  row_index, elem_index, repetition_idx);
		}
	    }
	    else {
	    set_null_str:
		if ( this->tnull_is_set_rec == true ) {
		    return this->assign_string(this->tany.cstr(TNULL_IDX),
				   row_index, elem_index, repetition_idx);
		}
		else return this->assign_string(NULL, row_index,
						elem_index, repetition_idx);
	    }
	}
	return this->assign_longlong(value, row_index,
				     elem_index, repetition_idx);
    }
    else if ( this->type_rec == FITS::LOGICAL_T ||
	      this->type_rec == FITS::BIT_T ) {
	return this->assign_longlong(value, row_index,
				     elem_index, repetition_idx);
    }
    else {
	if ( this->tscal_rec == 1.0 && this->tzero_rec == 0.0 ) {
	    return this->assign_longlong(value, row_index,
					 elem_index, repetition_idx);
	}
	else {
	    double dval = value;
	    dval -= this->tzero_rec;
	    dval /= this->tscal_rec;
	    return this->assign_double(dval, row_index,
				       elem_index, repetition_idx);
	}
    }
}

/* long arg */

/**
 * @brief  整数値(long型)でセルに値を代入 (高レベル)
 *
 *  整数 value から，ヘッダの TZEROn と TSCALn を反映した実数値を求め，適切な
 *  変換(整数型のカラムの場合は丸め)を行なってセルに代入します．<br>
 *  このメンバ関数では NULL値を与える事はできません．NULL値を与える場合は，
 *  fits_table_col::assign(double, long) を使用してください．
 *
 * @param   value 代入する値
 * @param   row_index 行インデックス
 * @return  自身の参照
 * @note  バイナリテーブルの文字列型のカラムでも TNULLn の設定が可能です．これ
 *        は SFITSIO の独自拡張です．
 * @attention  可変長配列はサポートされていません．
 */
fits_table_col &fits_table_col::assign( long value, long row_index )
{
    if ( this->type_rec == FITS::STRING_T ) {
	if ( 0 < this->fmt_str.length() ) {
	    if ( this->tany.at(TDISP_IDX).strpbrk("A") == 0 ) {
		tstring buf(64);
		buf.printf("%ld",value);
		this->tmp_str_buf->printf(this->fmt_str.cstr(), buf.cstr());
		return this->assign_string(this->tmp_str_buf->cstr(),
					   row_index);
	    }
	    double dval = value;
	    dval -= this->tzero_rec;
	    dval /= this->tscal_rec;
	    if ( isfinite(dval) ) {
		if ( this->tany.at(TDISP_IDX).strpbrk("FEGD") == 0 ) {
		    this->tmp_str_buf->printf(this->fmt_str.cstr(),dval);
		    if ( this->tany.cchr(TDISP_IDX,0) == 'D' ) {
			ssize_t ep = this->tmp_str_buf->strchr('E');
			if ( 0 <= ep ) this->tmp_str_buf->at(ep) = 'D';
		    }
		    return this->assign_string(this->tmp_str_buf->cstr(),
					       row_index);
		}
		else if ( this->tany.at(TDISP_IDX).strpbrk("I") == 0 ) {
		    long long llv = value;
		    if ( this->tscal_rec != 1.0 || this->tzero_rec != 0.0 ) {
			if ( MIN_DOUBLE_ROUND_LLONG <= dval && dval <= MAX_DOUBLE_ROUND_LLONG ) {
			    llv = round_d2ll(dval);
			}
			else goto set_null_str;
		    }
		    this->tmp_str_buf->printf(this->fmt_str.cstr(),llv);
		    return this->assign_string(this->tmp_str_buf->cstr(),
					       row_index);
		}
	    }
	    else {
	    set_null_str:
		if ( this->tnull_is_set_rec == true ) {
		    return this->assign_string(this->tany.cstr(TNULL_IDX),
					       row_index);
		}
		else return this->assign_string(NULL, row_index);
	    }
	}
	return this->assign_long(value, row_index);
    }
    else if ( this->type_rec == FITS::LOGICAL_T ||
	      this->type_rec == FITS::BIT_T ) {
	return this->assign_long(value, row_index);
    }
    else {
	if ( this->tscal_rec == 1.0 && this->tzero_rec == 0.0 ) {
	    return this->assign_long(value, row_index);
	}
	else {
	    double dval = value;
	    dval -= this->tzero_rec;
	    dval /= this->tscal_rec;
	    return this->assign_double(dval, row_index);
	}
    }
}

/**
 * @brief  整数値(long型)でセルに値を代入 (高レベル)
 *
 *  整数 value から，ヘッダの TZEROn と TSCALn を反映した実数値を求め，適切な
 *  変換(整数型のカラムの場合は丸め)を行なってセルに代入します．<br>
 *  このメンバ関数では NULL値を与える事はできません．NULL値を与える場合は，
 *  fits_table_col::assign(double, long, const char *, long) を使用してくだ
 *  さい．
 *
 * @param   value 代入する値
 * @param   row_index 行インデックス
 * @param   elem_name 要素名 (TELEMn に存在する名前)
 * @param   repetition_idx  TDIMn の2次元目のインデックス
 * @return  自身の参照
 * @note  バイナリテーブルの文字列型のカラムでも TNULLn の設定が可能です．これ
 *        は SFITSIO の独自拡張です．
 * @attention  可変長配列はサポートされていません．
 */
fits_table_col &fits_table_col::assign( long value, long row_index,
				   const char *elem_name, long repetition_idx )
{
    if ( this->type_rec == FITS::STRING_T ) {
	if ( 0 < this->fmt_str.length() ) {
	    if ( this->tany.at(TDISP_IDX).strpbrk("A") == 0 ) {
		tstring buf(64);
		buf.printf("%ld",value);
		this->tmp_str_buf->printf(this->fmt_str.cstr(), buf.cstr());
		return this->assign_string(this->tmp_str_buf->cstr(),
				      row_index, elem_name, repetition_idx);
	    }
	    double dval = value;
	    dval -= this->tzero_rec;
	    dval /= this->tscal_rec;
	    if ( isfinite(dval) ) {
		if ( this->tany.at(TDISP_IDX).strpbrk("FEGD") == 0 ) {
		    this->tmp_str_buf->printf(this->fmt_str.cstr(),dval);
		    if ( this->tany.cchr(TDISP_IDX,0) == 'D' ) {
			ssize_t ep = this->tmp_str_buf->strchr('E');
			if ( 0 <= ep ) this->tmp_str_buf->at(ep) = 'D';
		    }
		    return this->assign_string(this->tmp_str_buf->cstr(),
				   row_index, elem_name, repetition_idx);
		}
		else if ( this->tany.at(TDISP_IDX).strpbrk("I") == 0 ) {
		    long long llv = value;
		    if ( this->tscal_rec != 1.0 || this->tzero_rec != 0.0 ) {
			if ( MIN_DOUBLE_ROUND_LLONG <= dval && dval <= MAX_DOUBLE_ROUND_LLONG ) {
			    llv = round_d2ll(dval);
			}
			else goto set_null_str;
		    }
		    this->tmp_str_buf->printf(this->fmt_str.cstr(),llv);
		    return this->assign_string(this->tmp_str_buf->cstr(),
				   row_index, elem_name, repetition_idx);
		}
	    }
	    else {
	    set_null_str:
		if ( this->tnull_is_set_rec == true ) {
		    return this->assign_string(this->tany.cstr(TNULL_IDX),
				    row_index, elem_name, repetition_idx);
		}
		else return this->assign_string(NULL, row_index,
						elem_name, repetition_idx);
	    }
	}
	return this->assign_long(value, row_index,
				 elem_name, repetition_idx);
    }
    else if ( this->type_rec == FITS::LOGICAL_T ||
	      this->type_rec == FITS::BIT_T ) {
	return this->assign_long(value, row_index,
				 elem_name, repetition_idx);
    }
    else {
	if ( this->tscal_rec == 1.0 && this->tzero_rec == 0.0 ) {
	    return this->assign_long(value, row_index,
				     elem_name, repetition_idx);
	}
	else {
	    double dval = value;
	    dval -= this->tzero_rec;
	    dval /= this->tscal_rec;
	    return this->assign_double(dval, row_index,
				       elem_name, repetition_idx);
	}
    }
}

/**
 * @brief  整数値(long型)でセルに値を代入 (高レベル)
 *
 *  整数 value から，ヘッダの TZEROn と TSCALn を反映した実数値を求め，適切な
 *  変換(整数型のカラムの場合は丸め)を行なってセルに代入します．<br>
 *  このメンバ関数では NULL値を与える事はできません．NULL値を与える場合は，
 *  fits_table_col::assign(double, long, long, long) を使用してください．
 *
 * @param   value 代入する値
 * @param   row_index 行インデックス
 * @param   elem_index 要素インデックス (TDIMn の1次元目のインデックス)
 * @param   repetition_idx  TDIMn の2次元目のインデックス
 * @return  自身の参照
 * @note  バイナリテーブルの文字列型のカラムでも TNULLn の設定が可能です．これ
 *        は SFITSIO の独自拡張です．
 * @attention  可変長配列はサポートされていません．
 */
fits_table_col &fits_table_col::assign( long value, long row_index,
					long elem_index, long repetition_idx )
{
    if ( this->type_rec == FITS::STRING_T ) {
	if ( 0 < this->fmt_str.length() ) {
	    if ( this->tany.at(TDISP_IDX).strpbrk("A") == 0 ) {
		tstring buf(64);
		buf.printf("%ld",value);
		this->tmp_str_buf->printf(this->fmt_str.cstr(), buf.cstr());
		return this->assign_string(this->tmp_str_buf->cstr(),
				     row_index, elem_index, repetition_idx);
	    }
	    double dval = value;
	    dval -= this->tzero_rec;
	    dval /= this->tscal_rec;
	    if ( isfinite(dval) ) {
		if ( this->tany.at(TDISP_IDX).strpbrk("FEGD") == 0 ) {
		    this->tmp_str_buf->printf(this->fmt_str.cstr(),dval);
		    if ( this->tany.cchr(TDISP_IDX,0) == 'D' ) {
			ssize_t ep = this->tmp_str_buf->strchr('E');
			if ( 0 <= ep ) this->tmp_str_buf->at(ep) = 'D';
		    }
		    return this->assign_string(this->tmp_str_buf->cstr(),
				  row_index, elem_index, repetition_idx);
		}
		else if ( this->tany.at(TDISP_IDX).strpbrk("I") == 0 ) {
		    long long llv = value;
		    if ( this->tscal_rec != 1.0 || this->tzero_rec != 0.0 ) {
			if ( MIN_DOUBLE_ROUND_LLONG <= dval && dval <= MAX_DOUBLE_ROUND_LLONG ) {
			    llv = round_d2ll(dval);
			}
			else goto set_null_str;
		    }
		    this->tmp_str_buf->printf(this->fmt_str.cstr(),llv);
		    return this->assign_string(this->tmp_str_buf->cstr(),
				  row_index, elem_index, repetition_idx);
		}
	    }
	    else {
	    set_null_str:
		if ( this->tnull_is_set_rec == true ) {
		    return this->assign_string(this->tany.cstr(TNULL_IDX),
				   row_index, elem_index, repetition_idx);
		}
		else return this->assign_string(NULL, row_index,
						elem_index, repetition_idx);
	    }
	}
	return this->assign_long(value, row_index,
				 elem_index, repetition_idx);
    }
    else if ( this->type_rec == FITS::LOGICAL_T ||
	      this->type_rec == FITS::BIT_T ) {
	return this->assign_long(value, row_index,
				 elem_index, repetition_idx);
    }
    else {
	if ( this->tscal_rec == 1.0 && this->tzero_rec == 0.0 ) {
	    return this->assign_long(value, row_index,
				     elem_index, repetition_idx);
	}
	else {
	    double dval = value;
	    dval -= this->tzero_rec;
	    dval /= this->tscal_rec;
	    return this->assign_double(dval, row_index,
				       elem_index, repetition_idx);
	}
    }
}

/* int arg */

/**
 * @brief  整数値(int型)でセルに値を代入 (高レベル)
 *
 *  整数 value から，ヘッダの TZEROn と TSCALn を反映した実数値を求め，適切な
 *  変換(整数型のカラムの場合は丸め)を行なってセルに代入します．<br>
 *  このメンバ関数では NULL値を与える事はできません．NULL値を与える場合は，
 *  fits_table_col::assign(double, long) を使用してください．
 *
 * @param   value 代入する値
 * @param   row_index 行インデックス
 * @return  自身の参照
 * @note  バイナリテーブルの文字列型のカラムでも TNULLn の設定が可能です．これ
 *        は SFITSIO の独自拡張です．
 * @attention  可変長配列はサポートされていません．
 */
fits_table_col &fits_table_col::assign( int value, long row_index )
{
    return this->assign((long)value, row_index);
}

/**
 * @brief  整数値(int型)でセルに値を代入 (高レベル)
 *
 *  整数 value から，ヘッダの TZEROn と TSCALn を反映した実数値を求め，適切な
 *  変換(整数型のカラムの場合は丸め)を行なってセルに代入します．<br>
 *  このメンバ関数では NULL値を与える事はできません．NULL値を与える場合は，
 *  fits_table_col::assign(double, long, const char *, long) を使用してくだ
 *  さい．
 *
 * @param   value 代入する値
 * @param   row_index 行インデックス
 * @param   elem_name 要素名 (TELEMn に存在する名前)
 * @param   repetition_idx  TDIMn の2次元目のインデックス
 * @return  自身の参照
 * @note  バイナリテーブルの文字列型のカラムでも TNULLn の設定が可能です．これ
 *        は SFITSIO の独自拡張です．
 * @attention  可変長配列はサポートされていません．
 */
fits_table_col &fits_table_col::assign( int value, long row_index,
				   const char *elem_name, long repetition_idx )
{
    return this->assign((long)value, row_index, 
			elem_name, repetition_idx);
}

/**
 * @brief  整数値(int型)でセルに値を代入 (高レベル)
 *
 *  整数 value から，ヘッダの TZEROn と TSCALn を反映した実数値を求め，適切な
 *  変換(整数型のカラムの場合は丸め)を行なってセルに代入します．<br>
 *  このメンバ関数では NULL値を与える事はできません．NULL値を与える場合は，
 *  fits_table_col::assign(double, long, long, long) を使用してください．
 *
 * @param   value 代入する値
 * @param   row_index 行インデックス
 * @param   elem_index 要素インデックス (TDIMn の1次元目のインデックス)
 * @param   repetition_idx  TDIMn の2次元目のインデックス
 * @return  自身の参照
 * @note  バイナリテーブルの文字列型のカラムでも TNULLn の設定が可能です．これ
 *        は SFITSIO の独自拡張です．
 * @attention  可変長配列はサポートされていません．
 */
fits_table_col &fits_table_col::assign( int value, long row_index,
					long elem_index, long repetition_idx )
{
    return this->assign((long)value, row_index, 
			elem_index, repetition_idx);
}

#if 0
/* short arg */

fits_table_col &fits_table_col::assign( short value, long row_index )
{
    if ( this->type_rec == FITS::STRING_T ) {
	if ( 0 < this->fmt_str.length() ) {
	    if ( this->tany.at(TDISP_IDX).strpbrk("A") == 0 ) {
		tstring buf(64);
		buf.printf("%hd",value);
		this->tmp_str_buf->printf(this->fmt_str.cstr(), buf.cstr());
		return this->assign_string(this->tmp_str_buf->cstr(),
					   row_index);
	    }
	    double dval = value;
	    dval -= this->tzero_rec;
	    dval /= this->tscal_rec;
	    if ( isfinite(dval) ) {
		if ( this->tany.at(TDISP_IDX).strpbrk("FEGD") == 0 ) {
		    this->tmp_str_buf->printf(this->fmt_str.cstr(),dval);
		    if ( this->tany.cchr(TDISP_IDX,0) == 'D' ) {
			ssize_t ep = this->tmp_str_buf->strchr('E');
			if ( 0 <= ep ) this->tmp_str_buf->at(ep) = 'D';
		    }
		    return this->assign_string(this->tmp_str_buf->cstr(),
					       row_index);
		}
		else if ( this->tany.at(TDISP_IDX).strpbrk("I") == 0 ) {
		    long long llv = value;
		    if ( this->tscal_rec != 1.0 || this->tzero_rec != 0.0 ) {
			if ( MIN_DOUBLE_ROUND_LLONG <= dval && dval <= MAX_DOUBLE_ROUND_LLONG ) {
			    llv = round_d2ll(dval);
			}
			else goto set_null_str;
		    }
		    this->tmp_str_buf->printf(this->fmt_str.cstr(),llv);
		    return this->assign_string(this->tmp_str_buf->cstr(),
					       row_index);
		}
	    }
	    else {
	    set_null_str:
		if ( this->tnull_is_set_rec == true ) {
		    return this->assign_string(this->tany.cstr(TNULL_IDX),
					       row_index);
		}
		else return this->assign_string(NULL, row_index);
	    }
	}
	return this->assign_short(value, row_index);
    }
    else if ( this->type_rec == FITS::LOGICAL_T ||
	      this->type_rec == FITS::BIT_T ) {
	return this->assign_short(value, row_index);
    }
    else {
	if ( this->tscal_rec == 1.0 && this->tzero_rec == 0.0 ) {
	    return this->assign_short(value, row_index);
	}
	else {
	    double dval = value;
	    dval -= this->tzero_rec;
	    dval /= this->tscal_rec;
	    return this->assign_double(dval, row_index);
	}
    }
}

fits_table_col &fits_table_col::assign( short value, long row_index,
				   const char *elem_name, long repetition_idx )
{
    if ( this->type_rec == FITS::STRING_T ) {
	if ( 0 < this->fmt_str.length() ) {
	    if ( this->tany.at(TDISP_IDX).strpbrk("A") == 0 ) {
		tstring buf(64);
		buf.printf("%hd",value);
		this->tmp_str_buf->printf(this->fmt_str.cstr(), buf.cstr());
		return this->assign_string(this->tmp_str_buf->cstr(),
				      row_index, elem_name, repetition_idx);
	    }
	    double dval = value;
	    dval -= this->tzero_rec;
	    dval /= this->tscal_rec;
	    if ( isfinite(dval) ) {
		if ( this->tany.at(TDISP_IDX).strpbrk("FEGD") == 0 ) {
		    this->tmp_str_buf->printf(this->fmt_str.cstr(),dval);
		    if ( this->tany.cchr(TDISP_IDX,0) == 'D' ) {
			ssize_t ep = this->tmp_str_buf->strchr('E');
			if ( 0 <= ep ) this->tmp_str_buf->at(ep) = 'D';
		    }
		    return this->assign_string(this->tmp_str_buf->cstr(),
				   row_index, elem_name, repetition_idx);
		}
		else if ( this->tany.at(TDISP_IDX).strpbrk("I") == 0 ) {
		    long long llv = value;
		    if ( this->tscal_rec != 1.0 || this->tzero_rec != 0.0 ) {
			if ( MIN_DOUBLE_ROUND_LLONG <= dval && dval <= MAX_DOUBLE_ROUND_LLONG ) {
			    llv = round_d2ll(dval);
			}
			else goto set_null_str;
		    }
		    this->tmp_str_buf->printf(this->fmt_str.cstr(),llv);
		    return this->assign_string(this->tmp_str_buf->cstr(),
				   row_index, elem_name, repetition_idx);
		}
	    }
	    else {
	    set_null_str:
		if ( this->tnull_is_set_rec == true ) {
		    return this->assign_string(this->tany.cstr(TNULL_IDX),
				    row_index, elem_name, repetition_idx);
		}
		else return this->assign_string(NULL, row_index,
						elem_name, repetition_idx);
	    }
	}
	return this->assign_short(value, row_index,
				  elem_name, repetition_idx);
    }
    else if ( this->type_rec == FITS::LOGICAL_T ||
	      this->type_rec == FITS::BIT_T ) {
	return this->assign_short(value, row_index,
				  elem_name, repetition_idx);
    }
    else {
	if ( this->tscal_rec == 1.0 && this->tzero_rec == 0.0 ) {
	    return this->assign_short(value, row_index,
				      elem_name, repetition_idx);
	}
	else {
	    double dval = value;
	    dval -= this->tzero_rec;
	    dval /= this->tscal_rec;
	    return this->assign_double(dval, row_index,
				       elem_name, repetition_idx);
	}
    }
}

fits_table_col &fits_table_col::assign( short value, long row_index,
					long elem_index, long repetition_idx )
{
    if ( this->type_rec == FITS::STRING_T ) {
	if ( 0 < this->fmt_str.length() ) {
	    if ( this->tany.at(TDISP_IDX).strpbrk("A") == 0 ) {
		tstring buf(64);
		buf.printf("%hd",value);
		this->tmp_str_buf->printf(this->fmt_str.cstr(), buf.cstr());
		return this->assign_string(this->tmp_str_buf->cstr(),
				     row_index, elem_index, repetition_idx);
	    }
	    double dval = value;
	    dval -= this->tzero_rec;
	    dval /= this->tscal_rec;
	    if ( isfinite(dval) ) {
		if ( this->tany.at(TDISP_IDX).strpbrk("FEGD") == 0 ) {
		    this->tmp_str_buf->printf(this->fmt_str.cstr(),dval);
		    if ( this->tany.cchr(TDISP_IDX,0) == 'D' ) {
			ssize_t ep = this->tmp_str_buf->strchr('E');
			if ( 0 <= ep ) this->tmp_str_buf->at(ep) = 'D';
		    }
		    return this->assign_string(this->tmp_str_buf->cstr(),
				  row_index, elem_index, repetition_idx);
		}
		else if ( this->tany.at(TDISP_IDX).strpbrk("I") == 0 ) {
		    long long llv = value;
		    if ( this->tscal_rec != 1.0 || this->tzero_rec != 0.0 ) {
			if ( MIN_DOUBLE_ROUND_LLONG <= dval && dval <= MAX_DOUBLE_ROUND_LLONG ) {
			    llv = round_d2ll(dval);
			}
			else goto set_null_str;
		    }
		    this->tmp_str_buf->printf(this->fmt_str.cstr(),llv);
		    return this->assign_string(this->tmp_str_buf->cstr(),
				  row_index, elem_index, repetition_idx);
		}
	    }
	    else {
	    set_null_str:
		if ( this->tnull_is_set_rec == true ) {
		    return this->assign_string(this->tany.cstr(TNULL_IDX),
				   row_index, elem_index, repetition_idx);
		}
		else return this->assign_string(NULL, row_index,
						elem_index, repetition_idx);
	    }
	}
	return this->assign_short(value, row_index,
				  elem_index, repetition_idx);
    }
    else if ( this->type_rec == FITS::LOGICAL_T ||
	      this->type_rec == FITS::BIT_T ) {
	return this->assign_short(value, row_index,
				  elem_index, repetition_idx);
    }
    else {
	if ( this->tscal_rec == 1.0 && this->tzero_rec == 0.0 ) {
	    return this->assign_short(value, row_index,
				      elem_index, repetition_idx);
	}
	else {
	    double dval = value;
	    dval -= this->tzero_rec;
	    dval /= this->tscal_rec;
	    return this->assign_double(dval, row_index,
				       elem_index, repetition_idx);
	}
    }
}

#endif	/* 0 */

/* const char * arg */

/**
 * @brief  文字列値でセルに値を代入 (高レベル)
 *
 *  文字列値を適切に変換し，セルに代入します．<br>
 *  value が NULL文字列の場合は，NULL値が与えられたものとします．この時，バイ
 *  ナリテーブルの整数型カラムやアスキーテーブルの場合に TNULLn の値があれば
 *  その値をセルに代入します．NULL文字列の初期設定値は "NULL" で，この値は
 *  fits_table_col::assign_null_svalue() で変更する事ができます．<br>
 *  整数型や実数型のカラムの場合，文字列 value を実数値に変換し，さらにヘッダ
 *  の TZEROn と TSCALn の値で変換した実数値をセルに代入します(カラムが整数型
 *  の場合は，最も近い整数が代入されます)．
 *
 * @param   value 代入する値
 * @param   row_index 行インデックス
 * @return  自身の参照
 * @note  バイナリテーブルの文字列型のカラムでも TNULLn の設定が可能です．これ
 *        は SFITSIO の独自拡張です．
 * @attention  可変長配列はサポートされていません．
 * @note  SFITSIO では，カラム型が FITS::ASCII_T (TFORMn の指定に 'A' を含む) 
 *        の場合，TNULLn の設定がなければ，先頭文字が '\0' のセルを NULL値とし
 *        ます．これを「文字列の低レベルなNULL値」と呼んでいます．
 */
fits_table_col &fits_table_col::assign( const char *value, long row_index )
{
    tstring sval;
    if ( value == NULL ) sval.assign(this->null_svalue());
    else {
	if ( sval.assign(value).strtrim(' ').strcmp(this->null_svalue()) != 0 )
	    sval.assign(value);
    }
    if ( this->type_rec == FITS::STRING_T ) {
	if ( sval.strcmp(this->null_svalue()) == 0 ) {
	    if ( this->tnull_is_set_rec == true ) {
		sval.assign(this->tany.cstr(TNULL_IDX));
	    }
	    else {
		sval = NULL;
	    }
	}
	else if ( 0 < this->fmt_str.length() ) {
	    if ( this->tany.at(TDISP_IDX).strpbrk("A") == 0 ) {
		sval.assignf(this->fmt_str.cstr(),sval.cstr());
	    }
	    else if ( this->tany.at(TDISP_IDX).strpbrk("FEGD") == 0 ) {
		REMOVE_SPC(sval);
		double dval = sval.atof();
		dval -= this->tzero_rec;
		dval /= this->tscal_rec;
		if ( isfinite(dval) ) {
		    sval.assignf(this->fmt_str.cstr(),dval);
		    if ( this->tany.cchr(TDISP_IDX,0) == 'D' ) {
			ssize_t ep = sval.strchr('E');
			if ( 0 <= ep ) sval.at(ep) = 'D';
		    }
		}
		else {
		    if ( this->tnull_is_set_rec == true ) {
			sval.assign(this->tany.cstr(TNULL_IDX));
		    }
		    else {
			sval = NULL;
		    }
		}
	    }
	    else if ( this->tany.at(TDISP_IDX).strpbrk("I") == 0 ) {
		REMOVE_SPC(sval);
		double dval = sval.atof();
		dval -= this->tzero_rec;
		dval /= this->tscal_rec;
		if ( isfinite(dval) && MIN_DOUBLE_ROUND_LLONG <= dval && dval <= MAX_DOUBLE_ROUND_LLONG ) {
		    sval.assignf(this->fmt_str.cstr(),round_d2ll(dval));
		}
		else {
		    if ( this->tnull_is_set_rec == true ) {
			sval.assign(this->tany.cstr(TNULL_IDX));
		    }
		    else {
			sval = NULL;
		    }
		}
	    }
	}
	return this->assign_string(sval.cstr(), row_index);
    }
    else if ( this->type_rec == FITS::LOGICAL_T ) {
	const char *v;
	v = sval.strtrim(' ').cstr();
	if ( sval.strcmp(this->null_svalue()) == 0 ) v = NULL;
	return this->assign_string(v, row_index);
    }
    else if ( this->type_rec == FITS::BIT_T ) {
	REMOVE_SPC(sval);
	double dval = sval.atof();
	long long v;
	if ( isfinite(dval) && MIN_DOUBLE_ROUND_LLONG <= dval && dval <= MAX_DOUBLE_ROUND_LLONG )
	    v = round_d2ll(dval);
	else v = this->tnull_w_rec;
	return this->assign_bit(v, row_index);
    }
    else if ( (sval.strcmp(this->null_svalue()) == 0)
	      && (this->type_rec == FITS::LONG_T ||
		  this->type_rec == FITS::SHORT_T ||
		  this->type_rec == FITS::BYTE_T ||
		  this->type_rec == FITS::LONGLONG_T) ) {
	return this->assign_longlong(this->tnull_w_rec, row_index);
    }
    else if ( sval.strcmp(this->null_svalue()) == 0 ) {
	return this->assign_double(NAN, row_index);
    }
    else {
	REMOVE_SPC(sval);
	double v = sval.atof();
	v -= this->tzero_rec;
	v /= this->tscal_rec;
	return this->assign_double(v, row_index);
    }
}

/**
 * @brief  文字列値でセルに値を代入 (高レベル)
 *
 *  文字列値を適切に変換し，セルに代入します．<br>
 *  value が NULL文字列の場合は，NULL値が与えられたものとします．この時，バイ
 *  ナリテーブルの整数型カラムやアスキーテーブルの場合に TNULLn の値があれば
 *  その値をセルに代入します．NULL文字列の初期設定値は "NULL" で，この値は
 *  fits_table_col::assign_null_svalue() で変更する事ができます．<br>
 *  整数型や実数型のカラムの場合，文字列 value を実数値に変換し，さらにヘッダ
 *  の TZEROn と TSCALn の値で変換した実数値をセルに代入します(カラムが整数型
 *  の場合は，最も近い整数が代入されます)．
 *
 * @param   value 代入する値
 * @param   row_index 行インデックス
 * @param   elem_name 要素名 (TELEMn に存在する名前)
 * @param   repetition_idx  TDIMn の2次元目のインデックス
 * @return  自身の参照
 * @note  バイナリテーブルの文字列型のカラムでも TNULLn の設定が可能です．これ
 *        は SFITSIO の独自拡張です．
 * @attention  可変長配列はサポートされていません．
 * @note  SFITSIO では，カラム型が FITS::ASCII_T (TFORMn の指定に 'A' を含む) 
 *        の場合，TNULLn の設定がなければ，先頭文字が '\0' のセルを NULL値とし
 *        ます．これを「文字列の低レベルなNULL値」と呼んでいます．
 */
fits_table_col &fits_table_col::assign( const char *value, long row_index,
				   const char *elem_name, long repetition_idx )
{
    tstring sval;
    if ( value == NULL ) sval.assign(this->null_svalue());
    else {
	if ( sval.assign(value).strtrim(' ').strcmp(this->null_svalue()) != 0 )
	    sval.assign(value);
    }
    if ( this->type_rec == FITS::STRING_T ) {
	if ( sval.strcmp(this->null_svalue()) == 0 ) {
	    if ( this->tnull_is_set_rec == true ) {
		sval.assign(this->tany.cstr(TNULL_IDX));
	    }
	    else {
		sval = NULL;
	    }
	}
	else if ( 0 < this->fmt_str.length() ) {
	    if ( this->tany.at(TDISP_IDX).strpbrk("A") == 0 ) {
		sval.assignf(this->fmt_str.cstr(),sval.cstr());
	    }
	    else if ( this->tany.at(TDISP_IDX).strpbrk("FEGD") == 0 ) {
		REMOVE_SPC(sval);
		double dval = sval.atof();
		dval -= this->tzero_rec;
		dval /= this->tscal_rec;
		if ( isfinite(dval) ) {
		    sval.assignf(this->fmt_str.cstr(),dval);
		    if ( this->tany.cchr(TDISP_IDX,0) == 'D' ) {
			ssize_t ep = sval.strchr('E');
			if ( 0 <= ep ) sval.at(ep) = 'D';
		    }
		}
		else {
		    if ( this->tnull_is_set_rec == true ) {
			sval.assign(this->tany.cstr(TNULL_IDX));
		    }
		    else {
			sval = NULL;
		    }
		}
	    }
	    else if ( this->tany.at(TDISP_IDX).strpbrk("I") == 0 ) {
		REMOVE_SPC(sval);
		double dval = sval.atof();
		dval -= this->tzero_rec;
		dval /= this->tscal_rec;
		if ( isfinite(dval) && MIN_DOUBLE_ROUND_LLONG <= dval && dval <= MAX_DOUBLE_ROUND_LLONG ) {
		    sval.assignf(this->fmt_str.cstr(),round_d2ll(dval));
		}
		else {
		    if ( this->tnull_is_set_rec == true ) {
			sval.assign(this->tany.cstr(TNULL_IDX));
		    }
		    else {
			sval = NULL;
		    }
		}
	    }
	}
	return this->assign_string(sval.cstr(), row_index,
				   elem_name, repetition_idx);
    }
    else if ( this->type_rec == FITS::LOGICAL_T ) {
	const char *v;
	v = sval.strtrim(' ').cstr();
	if ( sval.strcmp(this->null_svalue()) == 0 ) v = NULL;
	return this->assign_string(v, row_index, elem_name, repetition_idx);
    }
    else if ( this->type_rec == FITS::BIT_T ) {
	REMOVE_SPC(sval);
	double dval = sval.atof();
	long long v;
	if ( isfinite(dval) && MIN_DOUBLE_ROUND_LLONG <= dval && dval <= MAX_DOUBLE_ROUND_LLONG )
	    v = round_d2ll(dval);
	else v = this->tnull_w_rec;
	return this->assign_bit(v, row_index,
				elem_name, repetition_idx);
    }
    else if ( (sval.strcmp(this->null_svalue()) == 0)
	      && (this->type_rec == FITS::LONG_T ||
		  this->type_rec == FITS::SHORT_T ||
		  this->type_rec == FITS::BYTE_T ||
		  this->type_rec == FITS::LONGLONG_T) ) {
	return this->assign_longlong(this->tnull_w_rec, row_index,
				     elem_name, repetition_idx);
    }
    else if ( sval.strcmp(this->null_svalue()) == 0 ) {
	return this->assign_double(NAN, row_index, elem_name, repetition_idx);
    }
    else {
	REMOVE_SPC(sval);
	double v = sval.atof();
	v -= this->tzero_rec;
	v /= this->tscal_rec;
	return this->assign_double(v, row_index, elem_name, repetition_idx);
    }
}

/**
 * @brief  文字列値でセルに値を代入 (高レベル)
 *
 *  文字列値を適切に変換し，セルに代入します．<br>
 *  value が NULL文字列の場合は，NULL値が与えられたものとします．この時，バイ
 *  ナリテーブルの整数型カラムやアスキーテーブルの場合に TNULLn の値があれば
 *  その値をセルに代入します．NULL文字列の初期設定値は "NULL" で，この値は
 *  fits_table_col::assign_null_svalue() で変更する事ができます．<br>
 *  整数型や実数型のカラムの場合，文字列 value を実数値に変換し，さらにヘッダ
 *  の TZEROn と TSCALn の値で変換した実数値をセルに代入します(カラムが整数型
 *  の場合は，最も近い整数が代入されます)．
 *
 * @param   value 代入する値
 * @param   row_index 行インデックス
 * @param   elem_index 要素インデックス (TDIMn の1次元目のインデックス)
 * @param   repetition_idx  TDIMn の2次元目のインデックス
 * @return  自身の参照
 * @note  バイナリテーブルの文字列型のカラムでも TNULLn の設定が可能です．これ
 *        は SFITSIO の独自拡張です．
 * @attention  可変長配列はサポートされていません．
 * @note  SFITSIO では，カラム型が FITS::ASCII_T (TFORMn の指定に 'A' を含む) 
 *        の場合，TNULLn の設定がなければ，先頭文字が '\0' のセルを NULL値とし
 *        ます．これを「文字列の低レベルなNULL値」と呼んでいます．
 */
fits_table_col &fits_table_col::assign( const char *value, long row_index,
					long elem_index, long repetition_idx )
{
    tstring sval;
    if ( value == NULL ) sval.assign(this->null_svalue());
    else {
	if ( sval.assign(value).strtrim(' ').strcmp(this->null_svalue()) != 0 )
	    sval.assign(value);
    }
    if ( this->type_rec == FITS::STRING_T ) {
	if ( sval.strcmp(this->null_svalue()) == 0 ) {
	    if ( this->tnull_is_set_rec == true ) {
		sval.assign(this->tany.cstr(TNULL_IDX));
	    }
	    else {
		sval = NULL;
	    }
	}
	else if ( 0 < this->fmt_str.length() ) {
	    if ( this->tany.at(TDISP_IDX).strpbrk("A") == 0 ) {
		sval.assignf(this->fmt_str.cstr(),sval.cstr());
	    }
	    else if ( this->tany.at(TDISP_IDX).strpbrk("FEGD") == 0 ) {
		REMOVE_SPC(sval);
		double dval = sval.atof();
		dval -= this->tzero_rec;
		dval /= this->tscal_rec;
		if ( isfinite(dval) ) {
		    sval.assignf(this->fmt_str.cstr(),dval);
		    if ( this->tany.cchr(TDISP_IDX,0) == 'D' ) {
			ssize_t ep = sval.strchr('E');
			if ( 0 <= ep ) sval.at(ep) = 'D';
		    }
		}
		else {
		    if ( this->tnull_is_set_rec == true ) {
			sval.assign(this->tany.cstr(TNULL_IDX));
		    }
		    else {
			sval = NULL;
		    }
		}
	    }
	    else if ( this->tany.at(TDISP_IDX).strpbrk("I") == 0 ) {
		REMOVE_SPC(sval);
		double dval = sval.atof();
		dval -= this->tzero_rec;
		dval /= this->tscal_rec;
		if ( isfinite(dval) && MIN_DOUBLE_ROUND_LLONG <= dval && dval <= MAX_DOUBLE_ROUND_LLONG ) {
		    sval.assignf(this->fmt_str.cstr(),round_d2ll(dval));
		}
		else {
		    if ( this->tnull_is_set_rec == true ) {
			sval.assign(this->tany.cstr(TNULL_IDX));
		    }
		    else {
			sval = NULL;
		    }
		}
	    }
	}
	return this->assign_string(sval.cstr(), row_index,
				   elem_index, repetition_idx);
    }
    else if ( this->type_rec == FITS::LOGICAL_T ) {
	const char *v;
	v = sval.strtrim(' ').cstr();
	if ( sval.strcmp(this->null_svalue()) == 0 ) v = NULL;
	return this->assign_string(v, row_index, elem_index, repetition_idx);
    }
    else if ( this->type_rec == FITS::BIT_T ) {
	REMOVE_SPC(sval);
	double dval = sval.atof();
	long long v;
	if ( isfinite(dval) && MIN_DOUBLE_ROUND_LLONG <= dval && dval <= MAX_DOUBLE_ROUND_LLONG )
	    v = round_d2ll(dval);
	else v = this->tnull_w_rec;
	return this->assign_bit(v, row_index,
				elem_index, repetition_idx);
    }
    else if ( (sval.strcmp(this->null_svalue()) == 0)
	      && (this->type_rec == FITS::LONG_T ||
		  this->type_rec == FITS::SHORT_T ||
		  this->type_rec == FITS::BYTE_T ||
		  this->type_rec == FITS::LONGLONG_T) ) {
	return this->assign_longlong(this->tnull_w_rec, row_index,
				     elem_index, repetition_idx);
    }
    else if ( sval.strcmp(this->null_svalue()) == 0 ) {
	return this->assign_double(NAN, row_index, elem_index, repetition_idx);
    }
    else {
	REMOVE_SPC(sval);
	double v = sval.atof();
	v -= this->tzero_rec;
	v /= this->tscal_rec;
	return this->assign_double(v, row_index, elem_index, repetition_idx);
    }
}

/* low-level member functions */

/* logical */
/**
 * @brief  論理値でセルに値をそのまま代入 (低レベル)
 *
 *  論理値でセルに値をそのまま代入します．<br>
 *  カラムの型が FITS::LOGICAL_T (TFORMn の指定に 'L' を含む) の場合に最速で
 *  アクセスできます．<br>
 *  value には，真の値を示す 'T'，偽の値を示す 'F'，NULL値 を示すその他の値
 *  (例えば 0) を与えます．
 *
 * @param   value 代入する値
 * @param   row_index 行インデックス
 * @return  自身の参照
 * @attention  可変長配列はサポートされていません．
 */
fits_table_col &fits_table_col::assign_logical( int value, long row_index )
{
    if ( row_index < 0 || this->row_size_rec <= row_index ) {
	return *this;	/* invalid */
    }

    if ( this->type_rec == FITS::LOGICAL_T ) {
	fits::logical_t v;
	fits::logical_t *d_ptr = (fits::logical_t *)this->data_ptr();
	if ( d_ptr == NULL ) return *this;	/* invalid */
	if ( value == 'T' ) v = 'T';
	else if ( value == 'F' ) v = 'F';
	else v = '\0';
	d_ptr[this->elem_size_rec * row_index] = v;
	return *this;
    }
    else if ( this->type_rec == FITS::LONG_T ) {
	long v;
	if ( value == 'T' ) v = 1;
	else if ( value == 'F' ) v = 0;
	else v = this->tnull_w_rec;
	return this->assign_long(v, row_index);
    }
    else if ( this->type_rec == FITS::SHORT_T ) {
	short v;
	if ( value == 'T' ) v = 1;
	else if ( value == 'F' ) v = 0;
	else v = this->tnull_w_rec;
	return this->assign_short(v, row_index);
    }
    else if ( this->type_rec == FITS::LONGLONG_T ) {
	long long v;
	if ( value == 'T' ) v = 1;
	else if ( value == 'F' ) v = 0;
	else v = this->tnull_w_rec;
	return this->assign_longlong(v, row_index);
    }
    else if ( this->type_rec == FITS::BYTE_T ) {
	unsigned char v;
	if ( value == 'T' ) v = 1;
	else if ( value == 'F' ) v = 0;
	else v = this->tnull_w_rec;
	return this->assign_byte(v, row_index);
    }
    else if ( this->type_rec == FITS::DOUBLE_T ) {
	double v;
	if ( value == 'T' ) v = 1;
	else if ( value == 'F' ) v = 0;
	else v = NAN;
	return this->assign_double(v, row_index);
    }
    else if ( this->type_rec == FITS::FLOAT_T ) {
	float v;
	if ( value == 'T' ) v = 1;
	else if ( value == 'F' ) v = 0;
	else v = NAN;
	return this->assign_float(v, row_index);
    }
    else if ( this->type_rec == FITS::BIT_T ) {
	return this->assign_bit((value == 'T') ? 1 : 0, row_index);
    }
    else if ( this->type_rec == FITS::STRING_T ) {
	const char *v;
	if ( value == 'T' ) v = "T";
	else if ( value == 'F' ) v = "F";
	else v = NULL;
	return this->assign_string(v,row_index);
    }
    else return *this;	/* invalid */
}

/**
 * @brief  論理値でセルに値をそのまま代入 (低レベル)
 *
 *  論理値でセルに値をそのまま代入します．<br>
 *  カラムの型が FITS::LOGICAL_T (TFORMn の指定に 'L' を含む) の場合に最速で
 *  アクセスできます．<br>
 *  value には，真の値を示す 'T'，偽の値を示す 'F'，NULL値 を示すその他の値
 *  (例えば 0) を与えます．
 *
 * @param   value 代入する値
 * @param   row_index 行インデックス
 * @param   elem_name 要素名 (TELEMn に存在する名前)
 * @param   repetition_idx  TDIMn の2次元目のインデックス
 * @return  自身の参照
 * @attention  可変長配列はサポートされていません．
 */
fits_table_col &fits_table_col::assign_logical( int value, long row_index,
				   const char *elem_name, long repetition_idx )
{
    long elem_index = this->elem_index_rec.index(elem_name,0);
    if ( this->type_rec == FITS::BIT_T ) {
	return this->assign_bit((value == 'T') ? 1 : 0,
				row_index,elem_index,repetition_idx,0);
    }
    else {
	return this->assign_logical( value, 
				     row_index, elem_index, repetition_idx );
    }
}

/**
 * @brief  論理値でセルに値をそのまま代入 (低レベル)
 *
 *  論理値でセルに値をそのまま代入します．<br>
 *  カラムの型が FITS::LOGICAL_T (TFORMn の指定に 'L' を含む) の場合に最速で
 *  アクセスできます．<br>
 *  value には，真の値を示す 'T'，偽の値を示す 'F'，NULL値 を示すその他の値
 *  (例えば 0) を与えます．
 *
 * @param   value 代入する値
 * @param   row_index 行インデックス
 * @param   elem_index 要素インデックス (TDIMn の1次元目のインデックス)
 * @param   repetition_idx  TDIMn の2次元目のインデックス
 * @return  自身の参照
 * @attention  可変長配列はサポートされていません．
 */
fits_table_col &fits_table_col::assign_logical( int value, long row_index,
					 long elem_index, long repetition_idx )
{
    if ( row_index < 0 || this->row_size_rec <= row_index ) {
	return *this;	/* invalid */
    }
    if ( elem_index < 0 || repetition_idx < 0 ) return *this;	/*invalid */

    if ( this->type_rec == FITS::LOGICAL_T ) {
	fits::logical_t v;
	fits::logical_t *d_ptr = (fits::logical_t *)this->data_ptr();
	long e_idx;
	if ( d_ptr == NULL ) return *this;	/* invalid */
	e_idx = this->dcol_size_rec * repetition_idx + elem_index;
	if ( this->elem_size_rec <= e_idx ) 
	    e_idx = this->elem_size_rec - this->dcol_size_rec 
		+ elem_index % this->dcol_size_rec;
	if ( value == 'T' ) v = 'T';
	else if ( value == 'F' ) v = 'F';
	else v = '\0';
	d_ptr[this->elem_size_rec * row_index + e_idx] = v;
	return *this;
    }
    else if ( this->type_rec == FITS::LONG_T ) {
	long v;
	if ( value == 'T' ) v = 1;
	else if ( value == 'F' ) v = 0;
	else v = this->tnull_w_rec;
	return this->assign_long(v,row_index,elem_index,repetition_idx);
    }
    else if ( this->type_rec == FITS::SHORT_T ) {
	short v;
	if ( value == 'T' ) v = 1;
	else if ( value == 'F' ) v = 0;
	else v = this->tnull_w_rec;
	return this->assign_short(v,row_index,elem_index,repetition_idx);
    }
    else if ( this->type_rec == FITS::LONGLONG_T ) {
	long long v;
	if ( value == 'T' ) v = 1;
	else if ( value == 'F' ) v = 0;
	else v = this->tnull_w_rec;
	return this->assign_longlong(v,row_index,elem_index,repetition_idx);
    }
    else if ( this->type_rec == FITS::BYTE_T ) {
	unsigned char v;
	if ( value == 'T' ) v = 1;
	else if ( value == 'F' ) v = 0;
	else v = this->tnull_w_rec;
	return this->assign_byte(v,row_index,elem_index,repetition_idx);
    }
    else if ( this->type_rec == FITS::DOUBLE_T ) {
	double v;
	if ( value == 'T' ) v = 1;
	else if ( value == 'F' ) v = 0;
	else v = NAN;
	return this->assign_double(v,row_index,elem_index,repetition_idx);
    }
    else if ( this->type_rec == FITS::FLOAT_T ) {
	float v;
	if ( value == 'T' ) v = 1;
	else if ( value == 'F' ) v = 0;
	else v = NAN;
	return this->assign_float(v,row_index,elem_index,repetition_idx);
    }
    else if ( this->type_rec == FITS::BIT_T ) {
	return this->assign_bit((value == 'T') ? 1 : 0,
				row_index,elem_index,repetition_idx);
    }
    else if ( this->type_rec == FITS::STRING_T ) {
	const char *v;
	if ( value == 'T' ) v = "T";
	else if ( value == 'F' ) v = "F";
	else v = NULL;
	return this->assign_string(v,row_index,elem_index,repetition_idx);
    }
    else return *this;	/* invalid */
}


/* short */
/**
 * @brief  整数値(short型)でセルに値をそのまま代入 (低レベル)
 *
 *  整数値(short型)でセルに値をそのまま代入します．TZEROn と TSCALn の値による
 *  変換は行ないません．<br>
 *  カラムの型が FITS::SHORT_T (TFORMn の指定に 'I' を含む) の場合に最速で
 *  アクセスできます．
 *
 * @param   value 代入する値
 * @param   row_index 行インデックス
 * @return  自身の参照
 * @attention  可変長配列はサポートされていません．
 */
fits_table_col &fits_table_col::assign_short( short value, long row_index )
{
    if ( row_index < 0 || this->row_size_rec <= row_index ) {
	return *this;	/* invalid */
    }

    if ( this->type_rec == FITS::SHORT_T ) {
	fits::short_t *d_ptr = (fits::short_t *)this->data_ptr();
	if ( d_ptr == NULL ) return *this;	/* invalid */
	d_ptr[this->elem_size_rec * row_index] = value;
	return *this;
    }
    else if ( this->type_rec == FITS::LONG_T ) {
	return this->assign_long(value,row_index);
    }
    else if ( this->type_rec == FITS::LONGLONG_T ) {
	return this->assign_longlong(value,row_index);
    }
    else if ( this->type_rec == FITS::BYTE_T ) {
	unsigned char v;
	if ( MIN_UCHAR <= value && value <= MAX_UCHAR ) v = value;
	else v = this->tnull_w_rec;
	return this->assign_byte(v,row_index);
    }
    else if ( this->type_rec == FITS::DOUBLE_T ) {
	double v;
	if ( this->tnull_is_set_rec == true && this->tnull_r_rec == value ) 
	    v = NAN;
	else v = value;
	return this->assign_double(v,row_index);
    }
    else if ( this->type_rec == FITS::FLOAT_T ) {
	float v;
	if ( this->tnull_is_set_rec == true && this->tnull_r_rec == value ) 
	    v = NAN;
	else v = value;
	return this->assign_float(v,row_index);
    }
    else if ( this->type_rec == FITS::LOGICAL_T ) {
	return this->assign_logical((value != 0) ? 'T' : 'F', row_index);
    }
    else if ( this->type_rec == FITS::BIT_T ) {
	return this->assign_bit(value,row_index);
    }
    else if ( this->type_rec == FITS::STRING_T ) {
	tstring buf(64);
	buf.printf("%hd",value);
	return this->assign_string(buf.cstr(), row_index);
    }
    else return *this;	/* invalid */
}

/**
 * @brief  整数値(short型)でセルに値をそのまま代入 (低レベル)
 *
 *  整数値(short型)でセルに値をそのまま代入します．TZEROn と TSCALn の値による
 *  変換は行ないません．<br>
 *  カラムの型が FITS::SHORT_T (TFORMn の指定に 'I' を含む) の場合に最速で
 *  アクセスできます．
 *
 * @param   value 代入する値
 * @param   row_index 行インデックス
 * @param   elem_name 要素名 (TELEMn に存在する名前)
 * @param   repetition_idx  TDIMn の2次元目のインデックス
 * @return  自身の参照
 * @attention  可変長配列はサポートされていません．
 */
fits_table_col &fits_table_col::assign_short( short value, long row_index,
				   const char *elem_name, long repetition_idx )
{
    long elem_index = this->elem_index_rec.index(elem_name,0);
    if ( this->type_rec == FITS::BIT_T ) {
	return this->assign_bit(value,row_index,elem_index,repetition_idx,0);
    }
    else {
	return this->assign_short( value,
				   row_index, elem_index, repetition_idx );
    }
}

/**
 * @brief  整数値(short型)でセルに値をそのまま代入 (低レベル)
 *
 *  整数値(short型)でセルに値をそのまま代入します．TZEROn と TSCALn の値による
 *  変換は行ないません．<br>
 *  カラムの型が FITS::SHORT_T (TFORMn の指定に 'I' を含む) の場合に最速で
 *  アクセスできます．
 *
 * @param   value 代入する値
 * @param   row_index 行インデックス
 * @param   elem_index 要素インデックス (TDIMn の1次元目のインデックス)
 * @param   repetition_idx  TDIMn の2次元目のインデックス
 * @return  自身の参照
 * @attention  可変長配列はサポートされていません．
 */
fits_table_col &fits_table_col::assign_short( short value, long row_index,
					 long elem_index, long repetition_idx )
{
    if ( row_index < 0 || this->row_size_rec <= row_index ) {
	return *this;	/* invalid */
    }
    if ( elem_index < 0 || repetition_idx < 0 ) return *this;	/* invalid */

    if ( this->type_rec == FITS::SHORT_T ) {
	fits::short_t *d_ptr = (fits::short_t *)this->data_ptr();
	long e_idx;
	if ( d_ptr == NULL ) return *this;	/* invalid */
	e_idx = this->dcol_size_rec * repetition_idx
	    + elem_index;
	if ( this->elem_size_rec <= e_idx ) 
	    e_idx = this->elem_size_rec - this->dcol_size_rec 
		+ elem_index % this->dcol_size_rec;
	d_ptr[this->elem_size_rec * row_index + e_idx] = value;
	return *this;
    }
    else if ( this->type_rec == FITS::LONG_T ) {
	return this->assign_long(value,row_index,elem_index,repetition_idx);
    }
    else if ( this->type_rec == FITS::LONGLONG_T ) {
	return this->assign_longlong(value,
				     row_index,elem_index,repetition_idx);
    }
    else if ( this->type_rec == FITS::BYTE_T ) {
	unsigned char v;
	if ( MIN_UCHAR <= value && value <= MAX_UCHAR ) v = value;
	else v = this->tnull_w_rec;
	return this->assign_byte(v,row_index,elem_index,repetition_idx);
    }
    else if ( this->type_rec == FITS::DOUBLE_T ) {
	double v;
	if ( this->tnull_is_set_rec == true && this->tnull_r_rec == value ) 
	    v = NAN;
	else v = value;
	return this->assign_double(v,row_index,elem_index,repetition_idx);
    }
    else if ( this->type_rec == FITS::FLOAT_T ) {
	float v;
	if ( this->tnull_is_set_rec == true && this->tnull_r_rec == value ) 
	    v = NAN;
	else v = value;
	return this->assign_float(v,row_index,elem_index,repetition_idx);
    }
    else if ( this->type_rec == FITS::LOGICAL_T ) {
	return this->assign_logical((value != 0) ? 'T' : 'F',
				    row_index,elem_index,repetition_idx);
    }
    else if ( this->type_rec == FITS::BIT_T ) {
	return this->assign_bit(value,row_index,elem_index,repetition_idx);
    }
    else if ( this->type_rec == FITS::STRING_T ) {
	tstring buf(64);
	buf.printf("%hd",value);
	return this->assign_string(buf.cstr(), row_index, elem_index,
				   repetition_idx);
    }
    else return *this;	/* invalid */
}


/* long */
/**
 * @brief  整数値(long型)でセルに値をそのまま代入 (低レベル)
 *
 *  整数値(long型)でセルに値をそのまま代入します．TZEROn と TSCALn の値による
 *  変換は行ないません．<br>
 *  カラムの型が FITS::LONG_T (TFORMn の指定に 'J' を含む) の場合に最速で
 *  アクセスできます．
 *
 * @param   value 代入する値
 * @param   row_index 行インデックス
 * @return  自身の参照
 * @attention  可変長配列はサポートされていません．
 * @note  long型は 32-bit OS では 32-bit，64-bit OS では 64-bit の幅を持つ事が
 *        ほとんどです．
 */
fits_table_col &fits_table_col::assign_long( long value, long row_index )
{
    if ( row_index < 0 || this->row_size_rec <= row_index ) {
	return *this;	/* invalid */
    }

    if ( this->type_rec == FITS::LONG_T ) {
	fits::long_t *d_ptr = (fits::long_t *)this->data_ptr();
	if ( d_ptr == NULL ) return *this;	/* invalid */
	d_ptr += this->elem_size_rec * row_index;
	if ( MIN_INT32 <= value && value <= MAX_INT32 )
	    *d_ptr = value;
	else
	    *d_ptr = this->tnull_w_rec;
	return *this;
    }
    else if ( this->type_rec == FITS::LONGLONG_T ) {
	return this->assign_longlong(value,row_index);
    }
    else if ( this->type_rec == FITS::SHORT_T ) {
	short v;
	if ( MIN_SHORT <= value && value <= MAX_SHORT ) v = value;
	else v = this->tnull_w_rec;
	return this->assign_short(v,row_index);
    }
    else if ( this->type_rec == FITS::BYTE_T ) {
	unsigned char v;
	if ( MIN_UCHAR <= value && value <= MAX_UCHAR ) v = value;
	else v = this->tnull_w_rec;
	return this->assign_byte(v,row_index);
    }
    else if ( this->type_rec == FITS::DOUBLE_T ) {
	double v;
	if ( this->tnull_is_set_rec == true && this->tnull_r_rec == value ) 
	    v = NAN;
	else v = value;
	return this->assign_double(v,row_index);
    }
    else if ( this->type_rec == FITS::FLOAT_T ) {
	float v;
	if ( this->tnull_is_set_rec == true && this->tnull_r_rec == value ) 
	    v = NAN;
	else v = value;
	return this->assign_float(v,row_index);
    }
    else if ( this->type_rec == FITS::LOGICAL_T ) {
	return this->assign_logical((value != 0) ? 'T' : 'F',
				    row_index);
    }
    else if ( this->type_rec == FITS::BIT_T ) {
	return this->assign_bit(value,row_index);
    }
    else if ( this->type_rec == FITS::STRING_T ) {
	tstring buf(64);
	buf.printf("%ld",value);
	return this->assign_string(buf.cstr(), row_index);
    }
    else return *this;	/* invalid */
}

/**
 * @brief  整数値(long型)でセルに値をそのまま代入 (低レベル)
 *
 *  整数値(long型)でセルに値をそのまま代入します．TZEROn と TSCALn の値による
 *  変換は行ないません．<br>
 *  カラムの型が FITS::LONG_T (TFORMn の指定に 'J' を含む) の場合に最速で
 *  アクセスできます．
 *
 * @param   value 代入する値
 * @param   row_index 行インデックス
 * @param   elem_name 要素名 (TELEMn に存在する名前)
 * @param   repetition_idx  TDIMn の2次元目のインデックス
 * @return  自身の参照
 * @attention  可変長配列はサポートされていません．
 * @note  long型は 32-bit OS では 32-bit，64-bit OS では 64-bit の幅を持つ事が
 *        ほとんどです．
 */
fits_table_col &fits_table_col::assign_long( long value, long row_index,
				   const char *elem_name, long repetition_idx )
{
    long elem_index = this->elem_index_rec.index(elem_name,0);
    if ( this->type_rec == FITS::BIT_T ) {
	return this->assign_bit(value,row_index,elem_index,repetition_idx,0);
    }
    else {
	return this->assign_long( value,
				  row_index, elem_index, repetition_idx );
    }
}

/**
 * @brief  整数値(long型)でセルに値をそのまま代入 (低レベル)
 *
 *  整数値(long型)でセルに値をそのまま代入します．TZEROn と TSCALn の値による
 *  変換は行ないません．<br>
 *  カラムの型が FITS::LONG_T (TFORMn の指定に 'J' を含む) の場合に最速で
 *  アクセスできます．
 *
 * @param   value 代入する値
 * @param   row_index 行インデックス
 * @param   elem_index 要素インデックス (TDIMn の1次元目のインデックス)
 * @param   repetition_idx  TDIMn の2次元目のインデックス
 * @return  自身の参照
 * @attention  可変長配列はサポートされていません．
 * @note  long型は 32-bit OS では 32-bit，64-bit OS では 64-bit の幅を持つ事が
 *        ほとんどです．
 */
fits_table_col &fits_table_col::assign_long( long value, long row_index,
					 long elem_index, long repetition_idx )
{
    if ( row_index < 0 || this->row_size_rec <= row_index ) {
	return *this;	/* invalid */
    }
    if ( elem_index < 0 || repetition_idx < 0 ) return *this;	/* invalid */

    if ( this->type_rec == FITS::LONG_T ) {
	fits::long_t *d_ptr = (fits::long_t *)this->data_ptr();
	long e_idx;
	if ( d_ptr == NULL ) return *this;	/* invalid */
	e_idx = this->dcol_size_rec * repetition_idx
	    + elem_index;
	if ( this->elem_size_rec <= e_idx ) 
	    e_idx = this->elem_size_rec - this->dcol_size_rec 
		+ elem_index % this->dcol_size_rec;
	d_ptr += this->elem_size_rec * row_index + e_idx;
	if ( MIN_INT32 <= value && value <= MAX_INT32 )
	    *d_ptr = value;
	else
	    *d_ptr = this->tnull_w_rec;
	return *this;
    }
    else if ( this->type_rec == FITS::LONGLONG_T ) {
	return this->assign_longlong(value,row_index,elem_index,repetition_idx);
    }
    else if ( this->type_rec == FITS::SHORT_T ) {
	short v;
	if ( MIN_SHORT <= value && value <= MAX_SHORT ) v = value;
	else v = this->tnull_w_rec;
	return this->assign_short(v,row_index,elem_index,repetition_idx);
    }
    else if ( this->type_rec == FITS::BYTE_T ) {
	unsigned char v;
	if ( MIN_UCHAR <= value && value <= MAX_UCHAR ) v = value;
	else v = this->tnull_w_rec;
	return this->assign_byte(v,row_index,elem_index,repetition_idx);
    }
    else if ( this->type_rec == FITS::DOUBLE_T ) {
	double v;
	if ( this->tnull_is_set_rec == true && this->tnull_r_rec == value ) 
	    v = NAN;
	else v = value;
	return this->assign_double(v,row_index,elem_index,repetition_idx);
    }
    else if ( this->type_rec == FITS::FLOAT_T ) {
	float v;
	if ( this->tnull_is_set_rec == true && this->tnull_r_rec == value ) 
	    v = NAN;
	else v = value;
	return this->assign_float(v,row_index,elem_index,repetition_idx);
    }
    else if ( this->type_rec == FITS::LOGICAL_T ) {
	return this->assign_logical((value != 0) ? 'T' : 'F',
				    row_index,elem_index,repetition_idx);
    }
    else if ( this->type_rec == FITS::BIT_T ) {
	return this->assign_bit(value,row_index,elem_index,repetition_idx);
    }
    else if ( this->type_rec == FITS::STRING_T ) {
	tstring buf(64);
	buf.printf("%ld",value);
	return this->assign_string(buf.cstr(), row_index, elem_index,
				   repetition_idx);
    }
    else return *this;	/* invalid */
}


/* long long */
/**
 * @brief  整数値(long long型)でセルに値をそのまま代入 (低レベル)
 *
 *  整数値(long long型)でセルに値をそのまま代入します．TZEROn と TSCALn の値に
 *  よる変換は行ないません．<br>
 *  カラムの型が FITS::LONGLONG_T (TFORMn の指定に 'K' を含む) の場合に最速で
 *  アクセスできます．
 *
 * @param   value 代入する値
 * @param   row_index 行インデックス
 * @return  自身の参照
 * @attention  可変長配列はサポートされていません．
 */
fits_table_col &fits_table_col::assign_longlong( long long value, 
						 long row_index )
{
    if ( row_index < 0 || this->row_size_rec <= row_index ) {
	return *this;	/* invalid */
    }

    if ( this->type_rec == FITS::LONGLONG_T ) {
	fits::longlong_t *d_ptr = (fits::longlong_t *)this->data_ptr();
	if ( d_ptr == NULL ) return *this;	/* invalid */
	d_ptr += this->elem_size_rec * row_index;
	if ( MIN_INT64 <= value && value <= MAX_INT64 )
	    *d_ptr = value;
	else
	    *d_ptr = this->tnull_w_rec;
	return *this;
    }
    else if ( this->type_rec == FITS::LONG_T ) {
	long v;
	if ( MIN_LONG <= value && value <= MAX_LONG ) v = value;
	else v = this->tnull_w_rec;
	return this->assign_long(v,row_index);
    }
    else if ( this->type_rec == FITS::SHORT_T ) {
	short v;
	if ( MIN_SHORT <= value && value <= MAX_SHORT ) v = value;
	else v = this->tnull_w_rec;
	return this->assign_short(v,row_index);
    }
    else if ( this->type_rec == FITS::BYTE_T ) {
	unsigned char v;
	if ( MIN_UCHAR <= value && value <= MAX_UCHAR ) v = value;
	else v = this->tnull_w_rec;
	return this->assign_byte(v,row_index);
    }
    else if ( this->type_rec == FITS::DOUBLE_T ) {
	double v;
	if ( this->tnull_is_set_rec == true && this->tnull_r_rec == value ) 
	    v = NAN;
	else v = value;
	return this->assign_double(v,row_index);
    }
    else if ( this->type_rec == FITS::FLOAT_T ) {
	float v;
	if ( this->tnull_is_set_rec == true && this->tnull_r_rec == value ) 
	    v = NAN;
	else v = value;
	return this->assign_float(v,row_index);
    }
    else if ( this->type_rec == FITS::LOGICAL_T ) {
	return this->assign_logical((value != 0) ? 'T' : 'F',
				    row_index);
    }
    else if ( this->type_rec == FITS::BIT_T ) {
	return this->assign_bit(value,row_index);
    }
    else if ( this->type_rec == FITS::STRING_T ) {
	tstring buf(64);
	buf.printf("%lld",value);
	return this->assign_string(buf.cstr(), row_index);
    }
    else return *this;	/* invalid */
}

/**
 * @brief  整数値(long long型)でセルに値をそのまま代入 (低レベル)
 *
 *  整数値(long long型)でセルに値をそのまま代入します．TZEROn と TSCALn の値に
 *  よる変換は行ないません．<br>
 *  カラムの型が FITS::LONGLONG_T (TFORMn の指定に 'K' を含む) の場合に最速で
 *  アクセスできます．
 *
 * @param   value 代入する値
 * @param   row_index 行インデックス
 * @param   elem_name 要素名 (TELEMn に存在する名前)
 * @param   repetition_idx  TDIMn の2次元目のインデックス
 * @return  自身の参照
 * @attention  可変長配列はサポートされていません．
 */
fits_table_col &fits_table_col::assign_longlong( long long value, 
				   long row_index,
				   const char *elem_name, long repetition_idx )
{
    long elem_index = this->elem_index_rec.index(elem_name,0);
    if ( this->type_rec == FITS::BIT_T ) {
	return this->assign_bit(value,row_index,elem_index,repetition_idx,0);
    }
    else {
	return this->assign_longlong( value, 
				      row_index, elem_index, repetition_idx );
    }
}

/**
 * @brief  整数値(long long型)でセルに値をそのまま代入 (低レベル)
 *
 *  整数値(long long型)でセルに値をそのまま代入します．TZEROn と TSCALn の値に
 *  よる変換は行ないません．<br>
 *  カラムの型が FITS::LONGLONG_T (TFORMn の指定に 'K' を含む) の場合に最速で
 *  アクセスできます．
 *
 * @param   value 代入する値
 * @param   row_index 行インデックス
 * @param   elem_index 要素インデックス (TDIMn の1次元目のインデックス)
 * @param   repetition_idx  TDIMn の2次元目のインデックス
 * @return  自身の参照
 * @attention  可変長配列はサポートされていません．
 */
fits_table_col &fits_table_col::assign_longlong( long long value, 
				         long row_index,
					 long elem_index, long repetition_idx )
{
    if ( row_index < 0 || this->row_size_rec <= row_index ) {
	return *this;	/* invalid */
    }
    if ( elem_index < 0 || repetition_idx < 0 ) return *this;	/* invalid */

    if ( this->type_rec == FITS::LONGLONG_T ) {
	fits::longlong_t *d_ptr = (fits::longlong_t *)this->data_ptr();
	long e_idx;
	if ( d_ptr == NULL ) return *this;	/* invalid */
	e_idx = this->dcol_size_rec * repetition_idx
	    + elem_index;
	if ( this->elem_size_rec <= e_idx ) 
	    e_idx = this->elem_size_rec - this->dcol_size_rec 
		+ elem_index % this->dcol_size_rec;
	d_ptr += this->elem_size_rec * row_index + e_idx;
	if ( MIN_INT64 <= value && value <= MAX_INT64 )
	    *d_ptr = value;
	else
	    *d_ptr = this->tnull_w_rec;
	return *this;
    }
    else if ( this->type_rec == FITS::LONG_T ) {
	long v;
	if ( MIN_LONG <= value && value <= MAX_LONG ) v = value;
	else v = this->tnull_w_rec;
	return this->assign_long(v,row_index,elem_index,repetition_idx);
    }
    else if ( this->type_rec == FITS::SHORT_T ) {
	short v;
	if ( MIN_SHORT <= value && value <= MAX_SHORT ) v = value;
	else v = this->tnull_w_rec;
	return this->assign_short(v,row_index,elem_index,repetition_idx);
    }
    else if ( this->type_rec == FITS::BYTE_T ) {
	unsigned char v;
	if ( MIN_UCHAR <= value && value <= MAX_UCHAR ) v = value;
	else v = this->tnull_w_rec;
	return this->assign_byte(v,row_index,elem_index,repetition_idx);
    }
    else if ( this->type_rec == FITS::DOUBLE_T ) {
	double v;
	if ( this->tnull_is_set_rec == true && this->tnull_r_rec == value ) 
	    v = NAN;
	else v = value;
	return this->assign_double(v,row_index,elem_index,repetition_idx);
    }
    else if ( this->type_rec == FITS::FLOAT_T ) {
	float v;
	if ( this->tnull_is_set_rec == true && this->tnull_r_rec == value ) 
	    v = NAN;
	else v = value;
	return this->assign_float(v,row_index,elem_index,repetition_idx);
    }
    else if ( this->type_rec == FITS::LOGICAL_T ) {
	return this->assign_logical((value != 0) ? 'T' : 'F',
				    row_index,elem_index,repetition_idx);
    }
    else if ( this->type_rec == FITS::BIT_T ) {
	return this->assign_bit(value,row_index,elem_index,repetition_idx);
    }
    else if ( this->type_rec == FITS::STRING_T ) {
	tstring buf(64);
	buf.printf("%lld",value);
	return this->assign_string(buf.cstr(), row_index, elem_index,
				   repetition_idx);
    }
    else return *this;	/* invalid */
}


/* byte */
/**
 * @brief  整数値(byte型)でセルに値をそのまま代入 (低レベル)
 *
 *  整数値(unsigned char型)でセルに値をそのまま代入します．TZEROn と TSCALn の
 *  値による変換は行ないません．<br>
 *  カラムの型が FITS::BYTE_T (TFORMn の指定に 'B' を含む) の場合に最速で
 *  アクセスできます．
 *
 * @param   value 代入する値
 * @param   row_index 行インデックス
 * @return  自身の参照
 * @attention  可変長配列はサポートされていません．
 */
fits_table_col &fits_table_col::assign_byte( unsigned char value, 
					     long row_index )
{
    if ( row_index < 0 || this->row_size_rec <= row_index ) {
	return *this;	/* invalid */
    }

    if ( this->type_rec == FITS::BYTE_T ) {
	fits::byte_t *d_ptr = (fits::byte_t *)this->data_ptr();
	if ( d_ptr == NULL ) return *this;	/* invalid */
	d_ptr += this->elem_size_rec * row_index;
	if ( MIN_UCHAR <= value && value <= MAX_UCHAR )
	    *d_ptr = value;
	else
	    *d_ptr = this->tnull_w_rec;
	return *this;
    }
    else if ( this->type_rec == FITS::SHORT_T ) {
	return this->assign_short(value,row_index);
    }
    else if ( this->type_rec == FITS::LONG_T ) {
	return this->assign_long(value,row_index);
    }
    else if ( this->type_rec == FITS::LONGLONG_T ) {
	return this->assign_longlong(value,row_index);
    }
    else if ( this->type_rec == FITS::DOUBLE_T ) {
	double v;
	if ( this->tnull_is_set_rec == true && this->tnull_r_rec == value ) 
	    v = NAN;
	else v = value;
	return this->assign_double(v,row_index);
    }
    else if ( this->type_rec == FITS::FLOAT_T ) {
	float v;
	if ( this->tnull_is_set_rec == true && this->tnull_r_rec == value ) 
	    v = NAN;
	else v = value;
	return this->assign_float(v,row_index);
    }
    else if ( this->type_rec == FITS::LOGICAL_T ) {
	return this->assign_logical((value != 0) ? 'T' : 'F',
				    row_index);
    }
    else if ( this->type_rec == FITS::BIT_T ) {
	return this->assign_bit(value,row_index);
    }
    else if ( this->type_rec == FITS::STRING_T ) {
	tstring buf(64);
	buf.printf("%hhu",value);
	return this->assign_string(buf.cstr(), row_index);
    }
    else return *this;	/* invalid */
}

/**
 * @brief  整数値(byte型)でセルに値をそのまま代入 (低レベル)
 *
 *  整数値(unsigned char型)でセルに値をそのまま代入します．TZEROn と TSCALn の
 *  値による変換は行ないません．<br>
 *  カラムの型が FITS::BYTE_T (TFORMn の指定に 'B' を含む) の場合に最速で
 *  アクセスできます．
 *
 * @param   value 代入する値
 * @param   row_index 行インデックス
 * @param   elem_name 要素名 (TELEMn に存在する名前)
 * @param   repetition_idx  TDIMn の2次元目のインデックス
 * @return  自身の参照
 * @attention  可変長配列はサポートされていません．
 */
fits_table_col &fits_table_col::assign_byte( unsigned char value, 
				   long row_index,
				   const char *elem_name, long repetition_idx )
{
    long elem_index = this->elem_index_rec.index(elem_name,0);
    if ( this->type_rec == FITS::BIT_T ) {
	return this->assign_bit(value,row_index,elem_index,repetition_idx,0);
    }
    else {
	return this->assign_byte( value, 
				  row_index, elem_index, repetition_idx );
    }
}

/**
 * @brief  整数値(byte型)でセルに値をそのまま代入 (低レベル)
 *
 *  整数値(unsigned char型)でセルに値をそのまま代入します．TZEROn と TSCALn の
 *  値による変換は行ないません．<br>
 *  カラムの型が FITS::BYTE_T (TFORMn の指定に 'B' を含む) の場合に最速で
 *  アクセスできます．
 *
 * @param   value 代入する値
 * @param   row_index 行インデックス
 * @param   elem_index 要素インデックス (TDIMn の1次元目のインデックス)
 * @param   repetition_idx  TDIMn の2次元目のインデックス
 * @return  自身の参照
 * @attention  可変長配列はサポートされていません．
 */
fits_table_col &fits_table_col::assign_byte( unsigned char value, 
					 long row_index,
					 long elem_index, long repetition_idx )
{
    if ( row_index < 0 || this->row_size_rec <= row_index ) {
	return *this;	/* invalid */
    }
    if ( elem_index < 0 || repetition_idx < 0 ) return *this;	/* invalid */

    if ( this->type_rec == FITS::BYTE_T ) {
	fits::byte_t *d_ptr = (fits::byte_t *)this->data_ptr();
	long e_idx;
	if ( d_ptr == NULL ) return *this;	/* invalid */
	e_idx = this->dcol_size_rec * repetition_idx
	    + elem_index;
	if ( this->elem_size_rec <= e_idx ) 
	    e_idx = this->elem_size_rec - this->dcol_size_rec 
		+ elem_index % this->dcol_size_rec;
	d_ptr += this->elem_size_rec * row_index + e_idx;
	if ( MIN_UCHAR <= value && value <= MAX_UCHAR )
	    *d_ptr = value;
	else
	    *d_ptr = this->tnull_w_rec;
	return *this;
    }
    else if ( this->type_rec == FITS::SHORT_T ) {
	return this->assign_short(value,row_index,elem_index,repetition_idx);
    }
    else if ( this->type_rec == FITS::LONG_T ) {
	return this->assign_long(value,row_index,elem_index,repetition_idx);
    }
    else if ( this->type_rec == FITS::LONGLONG_T ) {
	return this->assign_longlong(value,row_index,elem_index,repetition_idx);
    }
    else if ( this->type_rec == FITS::DOUBLE_T ) {
	double v;
	if ( this->tnull_is_set_rec == true && this->tnull_r_rec == value ) 
	    v = NAN;
	else v = value;
	return this->assign_double(v,row_index,elem_index,repetition_idx);
    }
    else if ( this->type_rec == FITS::FLOAT_T ) {
	double v;
	if ( this->tnull_is_set_rec == true && this->tnull_r_rec == value ) 
	    v = NAN;
	else v = value;
	return this->assign_float(v,row_index,elem_index,repetition_idx);
    }
    else if ( this->type_rec == FITS::LOGICAL_T ) {
	return this->assign_logical((value != 0) ? 'T' : 'F',
				    row_index,elem_index,repetition_idx);
    }
    else if ( this->type_rec == FITS::BIT_T ) {
	return this->assign_bit(value,row_index,elem_index,repetition_idx);
    }
    else if ( this->type_rec == FITS::STRING_T ) {
	tstring buf(64);
	buf.printf("%hhu",value);
	return this->assign_string(buf.cstr(), row_index, elem_index,
				   repetition_idx);
    }
    else return *this;	/* invalid */
}


/* float */
/**
 * @brief  整数値(float型)でセルに値をそのまま代入 (低レベル)
 *
 *  実数値(float型)でセルに値をそのまま代入します．TZEROn と TSCALn の値による
 *  変換は行ないません．<br>
 *  カラムの型が FITS::FLOAT_T (TFORMn の指定に 'E' を含む) の場合に最速で
 *  アクセスできます．
 *
 * @param   value 代入する値
 * @param   row_index 行インデックス
 * @return  自身の参照
 * @attention  可変長配列はサポートされていません．
 */
fits_table_col &fits_table_col::assign_float( float value, long row_index )
{
    if ( row_index < 0 || this->row_size_rec <= row_index ) {
	return *this;	/* invalid */
    }

    if ( this->type_rec == FITS::FLOAT_T ) {
	fits::float_t *d_ptr = (fits::float_t *)this->data_ptr();
	if ( d_ptr == NULL ) return *this;	/* invalid */
	d_ptr[this->elem_size_rec * row_index] = value;
	return *this;
    }
    else if ( this->type_rec == FITS::DOUBLE_T ) {
	return this->assign_double((double)value,row_index);
    }
    else if ( this->type_rec == FITS::LONG_T ) {
	long v;
	if ( isfinite(value) && MIN_FLOAT_ROUND_LONG <= value && value <= MAX_FLOAT_ROUND_LONG )
	    v = round_f2l(value);
	else v = this->tnull_w_rec;
	return this->assign_long(v,row_index);
    }
    else if ( this->type_rec == FITS::LONGLONG_T ) {
	long long v;
	if ( isfinite(value) && MIN_FLOAT_ROUND_LLONG <= value && value <= MAX_FLOAT_ROUND_LLONG )
	    v = round_f2ll(value);
	else v = this->tnull_w_rec;
	return this->assign_longlong(v,row_index);
    }
    else if ( this->type_rec == FITS::SHORT_T ) {
	short v;
	if ( isfinite(value) && MIN_FLOAT_ROUND_SHORT <= value && value <= MAX_FLOAT_ROUND_SHORT )
	    v = round_f2l(value);
	else v = this->tnull_w_rec;
	return this->assign_short(v,row_index);
    }
    else if ( this->type_rec == FITS::BYTE_T ) {
	unsigned char v;
	if ( isfinite(value) && MIN_FLOAT_ROUND_UCHAR <= value && value <= MAX_FLOAT_ROUND_UCHAR )
	    v = round_f2l(value);
	else v = this->tnull_w_rec;
	return this->assign_byte(v,row_index);
    }
    else if ( this->type_rec == FITS::LOGICAL_T ) {
	int v;
	if ( !isfinite(value) ) v = '\0';
	else v = (round(value) != 0.0) ? 'T' : 'F';
	return this->assign_logical(v, row_index);
    }
    else if ( this->type_rec == FITS::BIT_T ) {
	long long v;
	if ( isfinite(value) && MIN_FLOAT_ROUND_LLONG <= value && value <= MAX_FLOAT_ROUND_LLONG )
	    v = round_f2ll(value);
	else v = this->tnull_w_rec;
	return this->assign_bit(v,row_index);
    }
    else if ( this->type_rec == FITS::STRING_T ) {
	tstring buf(64);
	if ( !isfinite(value) ) {
	    return this->assign_string(NULL, row_index);
	}
	buf.printf("%.7G",(double)value);
	return this->assign_string(buf.cstr(), row_index);
    }
    else return *this;	/* invalid */
}

/**
 * @brief  整数値(float型)でセルに値をそのまま代入 (低レベル)
 *
 *  実数値(float型)でセルに値をそのまま代入します．TZEROn と TSCALn の値による
 *  変換は行ないません．<br>
 *  カラムの型が FITS::FLOAT_T (TFORMn の指定に 'E' を含む) の場合に最速で
 *  アクセスできます．
 *
 * @param   value 代入する値
 * @param   row_index 行インデックス
 * @param   elem_name 要素名 (TELEMn に存在する名前)
 * @param   repetition_idx  TDIMn の2次元目のインデックス
 * @return  自身の参照
 * @attention  可変長配列はサポートされていません．
 */
fits_table_col &fits_table_col::assign_float( float value, long row_index, 
					      const char *elem_name, 
					      long repetition_idx )
{
    long elem_index = this->elem_index_rec.index(elem_name,0);
    if ( this->type_rec == FITS::BIT_T ) {
	long long v;
	if ( isfinite(value) && MIN_FLOAT_ROUND_LLONG <= value && value <= MAX_FLOAT_ROUND_LLONG )
	    v = round_f2ll(value);
	else v = this->tnull_w_rec;
	return this->assign_bit(v,row_index,elem_index,repetition_idx,0);
    }
    else {
	return this->assign_float( value, 
				   row_index, elem_index, repetition_idx );
    }
}

/**
 * @brief  整数値(float型)でセルに値をそのまま代入 (低レベル)
 *
 *  実数値(float型)でセルに値をそのまま代入します．TZEROn と TSCALn の値による
 *  変換は行ないません．<br>
 *  カラムの型が FITS::FLOAT_T (TFORMn の指定に 'E' を含む) の場合に最速で
 *  アクセスできます．
 *
 * @param   value 代入する値
 * @param   row_index 行インデックス
 * @param   elem_index 要素インデックス (TDIMn の1次元目のインデックス)
 * @param   repetition_idx  TDIMn の2次元目のインデックス
 * @return  自身の参照
 * @attention  可変長配列はサポートされていません．
 */
fits_table_col &fits_table_col::assign_float( float value, long row_index,
					 long elem_index, long repetition_idx )
{
    if ( row_index < 0 || this->row_size_rec <= row_index ) {
	return *this;	/* invalid */
    }
    if ( elem_index < 0 || repetition_idx < 0 ) return *this;	/* invalid */

    if ( this->type_rec == FITS::FLOAT_T ) {
	fits::float_t *d_ptr = (fits::float_t *)this->data_ptr();
	long e_idx;
	if ( d_ptr == NULL ) return *this;	/* invalid */
	e_idx = this->dcol_size_rec * repetition_idx
	    + elem_index;
	if ( this->elem_size_rec <= e_idx ) 
	    e_idx = this->elem_size_rec - this->dcol_size_rec 
		+ elem_index % this->dcol_size_rec;
	d_ptr[this->elem_size_rec * row_index + e_idx] = value;
	return *this;
    }
    else if ( this->type_rec == FITS::DOUBLE_T ) {
	return this->assign_double((double)value,
				   row_index,elem_index,repetition_idx);
    }
    else if ( this->type_rec == FITS::LONG_T ) {
	long v;
	if ( isfinite(value) && MIN_FLOAT_ROUND_LONG <= value && value <= MAX_FLOAT_ROUND_LONG )
	    v = round_f2l(value);
	else v = this->tnull_w_rec;
	return this->assign_long(v,row_index,elem_index,repetition_idx);
    }
    else if ( this->type_rec == FITS::LONGLONG_T ) {
	long long v;
	if ( isfinite(value) && MIN_FLOAT_ROUND_LLONG <= value && value <= MAX_FLOAT_ROUND_LLONG )
	    v = round_f2ll(value);
	else v = this->tnull_w_rec;
	return this->assign_longlong(v,row_index,elem_index,repetition_idx);
    }
    else if ( this->type_rec == FITS::SHORT_T ) {
	short v;
	if ( isfinite(value) && MIN_FLOAT_ROUND_SHORT <= value && value <= MAX_FLOAT_ROUND_SHORT )
	    v = round_f2l(value);
	else v = this->tnull_w_rec;
	return this->assign_short(v,row_index,elem_index,repetition_idx);
    }
    else if ( this->type_rec == FITS::BYTE_T ) {
	unsigned char v;
	if ( isfinite(value) && MIN_FLOAT_ROUND_UCHAR <= value && value <= MAX_FLOAT_ROUND_UCHAR )
	    v = round_f2l(value);
	else v = this->tnull_w_rec;
	return this->assign_byte(v,row_index,elem_index,repetition_idx);
    }
    else if ( this->type_rec == FITS::LOGICAL_T ) {
	int v;
	if ( !isfinite(value) ) v = '\0';
	else v = (round(value) != 0.0) ? 'T' : 'F';
	return this->assign_logical(v,row_index,elem_index,repetition_idx);
    }
    else if ( this->type_rec == FITS::BIT_T ) {
	long long v;
	if ( isfinite(value) && MIN_FLOAT_ROUND_LLONG <= value && value <= MAX_FLOAT_ROUND_LLONG )
	    v = round_f2ll(value);
	else v = this->tnull_w_rec;
	return this->assign_bit(v,row_index,elem_index,repetition_idx);
    }
    else if ( this->type_rec == FITS::STRING_T ) {
	tstring buf(64);
	if ( !isfinite(value) ) {
	    return this->assign_string(NULL, row_index,
				       elem_index, repetition_idx);
	}
	buf.printf("%.7G",(double)value);
	return this->assign_string(buf.cstr(), row_index, elem_index,
				   repetition_idx);
    }
    else return *this;	/* invalid */
}


/* double */
/**
 * @brief  整数値(double型)でセルに値をそのまま代入 (低レベル)
 *
 *  実数値(double型)でセルに値をそのまま代入します．TZEROn と TSCALn の値による
 *  変換は行ないません．<br>
 *  カラムの型が FITS::DOUBLE_T (TFORMn の指定に 'D' を含む) の場合に最速で
 *  アクセスできます．
 *
 * @param   value 代入する値
 * @param   row_index 行インデックス
 * @return  自身の参照
 * @attention  可変長配列はサポートされていません．
 */
fits_table_col &fits_table_col::assign_double( double value, long row_index )
{
    if ( row_index < 0 || this->row_size_rec <= row_index ) {
	return *this;	/* invalid */
    }

    if ( this->type_rec == FITS::DOUBLE_T ) {
	fits::double_t *d_ptr = (fits::double_t *)this->data_ptr();
	if ( d_ptr == NULL ) return *this;	/* invalid */
	d_ptr[this->elem_size_rec * row_index] = value;
	return *this;
    }
    else if ( this->type_rec == FITS::FLOAT_T ) {
	return this->assign_float((float)value,row_index);
    }
    else if ( this->type_rec == FITS::LONG_T ) {
	long v;
	if ( isfinite(value) && MIN_DOUBLE_ROUND_LONG <= value && value <= MAX_DOUBLE_ROUND_LONG )
	    v = round_d2l(value);
	else v = this->tnull_w_rec;
	return this->assign_long(v,row_index);
    }
    else if ( this->type_rec == FITS::LONGLONG_T ) {
	long long v;
	if ( isfinite(value) && MIN_DOUBLE_ROUND_LLONG <= value && value <= MAX_DOUBLE_ROUND_LLONG )
	    v = round_d2ll(value);
	else v = this->tnull_w_rec;
	return this->assign_longlong(v,row_index);
    }
    else if ( this->type_rec == FITS::SHORT_T ) {
	short v;
	if ( isfinite(value) && MIN_DOUBLE_ROUND_SHORT <= value && value <= MAX_DOUBLE_ROUND_SHORT )
	    v = round_d2l(value);
	else v = this->tnull_w_rec;
	return this->assign_short(v,row_index);
    }
    else if ( this->type_rec == FITS::BYTE_T ) {
	unsigned char v;
	if ( isfinite(value) && MIN_DOUBLE_ROUND_UCHAR <= value && value <= MAX_DOUBLE_ROUND_UCHAR )
	    v = round_d2l(value);
	else v = this->tnull_w_rec;
	return this->assign_byte(v,row_index);
    }
    else if ( this->type_rec == FITS::LOGICAL_T ) {
	int v;
	if ( !isfinite(value) ) v = '\0';
	else v = (round(value) != 0.0) ? 'T' : 'F';
	return this->assign_logical(v,row_index);
    }
    else if ( this->type_rec == FITS::BIT_T ) {
	long long v;
	if ( isfinite(value) && MIN_DOUBLE_ROUND_LLONG <= value && value <= MAX_DOUBLE_ROUND_LLONG )
	    v = round_d2ll(value);
	else v = this->tnull_w_rec;
	return this->assign_bit(v,row_index);
    }
    else if ( this->type_rec == FITS::STRING_T ) {
	tstring buf(64);
	if ( !isfinite(value) ) {
	    return this->assign_string(NULL, row_index);
	}
	buf.printf("%.15G",value);
	return this->assign_string(buf.cstr(), row_index);
    }
    else return *this;	/* invalid */
}

/**
 * @brief  整数値(double型)でセルに値をそのまま代入 (低レベル)
 *
 *  実数値(double型)でセルに値をそのまま代入します．TZEROn と TSCALn の値による
 *  変換は行ないません．<br>
 *  カラムの型が FITS::DOUBLE_T (TFORMn の指定に 'D' を含む) の場合に最速で
 *  アクセスできます．
 *
 * @param   value 代入する値
 * @param   row_index 行インデックス
 * @param   elem_name 要素名 (TELEMn に存在する名前)
 * @param   repetition_idx  TDIMn の2次元目のインデックス
 * @return  自身の参照
 * @attention  可変長配列はサポートされていません．
 */
fits_table_col &fits_table_col::assign_double( double value, long row_index,
				   const char *elem_name, long repetition_idx )
{
    long elem_index = this->elem_index_rec.index(elem_name,0);
    if ( this->type_rec == FITS::BIT_T ) {
	long long v;
	if ( isfinite(value) && MIN_DOUBLE_ROUND_LLONG <= value && value <= MAX_DOUBLE_ROUND_LLONG )
	    v = round_d2ll(value);
	else v = this->tnull_w_rec;
	return this->assign_bit(v,row_index,elem_index,repetition_idx,0);
    }
    else {
	return this->assign_double( value, 
				    row_index, elem_index, repetition_idx );
    }
}

/**
 * @brief  整数値(double型)でセルに値をそのまま代入 (低レベル)
 *
 *  実数値(double型)でセルに値をそのまま代入します．TZEROn と TSCALn の値による
 *  変換は行ないません．<br>
 *  カラムの型が FITS::DOUBLE_T (TFORMn の指定に 'D' を含む) の場合に最速で
 *  アクセスできます．
 *
 * @param   value 代入する値
 * @param   row_index 行インデックス
 * @param   elem_index 要素インデックス (TDIMn の1次元目のインデックス)
 * @param   repetition_idx  TDIMn の2次元目のインデックス
 * @return  自身の参照
 * @attention  可変長配列はサポートされていません．
 */
fits_table_col &fits_table_col::assign_double( double value, long row_index,
					 long elem_index, long repetition_idx )
{
    if ( row_index < 0 || this->row_size_rec <= row_index ) {
	return *this;	/* invalid */
    }
    if ( elem_index < 0 || repetition_idx < 0 ) return *this;	/* invalid */

    if ( this->type_rec == FITS::DOUBLE_T ) {
	fits::double_t *d_ptr = (fits::double_t *)this->data_ptr();
	long e_idx;
	if ( d_ptr == NULL ) return *this;	/* invalid */
	e_idx = this->dcol_size_rec * repetition_idx
	    + elem_index;
	if ( this->elem_size_rec <= e_idx ) 
	    e_idx = this->elem_size_rec - this->dcol_size_rec 
		+ elem_index % this->dcol_size_rec;
	d_ptr[this->elem_size_rec * row_index + e_idx] = value;
	return *this;
    }
    else if ( this->type_rec == FITS::FLOAT_T ) {
	return this->assign_float((float)value,
				  row_index,elem_index,repetition_idx);
    }
    else if ( this->type_rec == FITS::LONG_T ) {
	long v;
	if ( isfinite(value) && MIN_DOUBLE_ROUND_LONG <= value && value <= MAX_DOUBLE_ROUND_LONG )
	    v = round_d2l(value);
	else v = this->tnull_w_rec;
	return this->assign_long(v,row_index,elem_index,repetition_idx);
    }
    else if ( this->type_rec == FITS::LONGLONG_T ) {
	long long v;
	if ( isfinite(value) && MIN_DOUBLE_ROUND_LLONG <= value && value <= MAX_DOUBLE_ROUND_LLONG )
	    v = round_d2ll(value);
	else v = this->tnull_w_rec;
	return this->assign_longlong(v,row_index,elem_index,repetition_idx);
    }
    else if ( this->type_rec == FITS::SHORT_T ) {
	short v;
	if ( isfinite(value) && MIN_DOUBLE_ROUND_SHORT <= value && value <= MAX_DOUBLE_ROUND_SHORT )
	    v = round_d2l(value);
	else v = this->tnull_w_rec;
	return this->assign_short(v,row_index,elem_index,repetition_idx);
    }
    else if ( this->type_rec == FITS::BYTE_T ) {
	unsigned char v;
	if ( isfinite(value) && MIN_DOUBLE_ROUND_UCHAR <= value && value <= MAX_DOUBLE_ROUND_UCHAR )
	    v = round_d2l(value);
	else v = this->tnull_w_rec;
	return this->assign_byte(v,row_index,elem_index,repetition_idx);
    }
    else if ( this->type_rec == FITS::LOGICAL_T ) {
	int v;
	if ( !isfinite(value) ) v = '\0';
	else v = (round(value) != 0.0) ? 'T' : 'F';
	return this->assign_logical(v,row_index,elem_index,repetition_idx);
    }
    else if ( this->type_rec == FITS::BIT_T ) {
	long long v;
	if ( isfinite(value) && MIN_DOUBLE_ROUND_LLONG <= value && value <= MAX_DOUBLE_ROUND_LLONG )
	    v = round_d2ll(value);
	else v = this->tnull_w_rec;
	return this->assign_bit(v,row_index,elem_index,repetition_idx);
    }
    else if ( this->type_rec == FITS::STRING_T ) {
	tstring buf(64);
	if ( !isfinite(value) ) {
	    return this->assign_string(NULL, row_index,
				       elem_index, repetition_idx);
	}
	buf.printf("%.15G",value);
	return this->assign_string(buf.cstr(), row_index, elem_index,
				   repetition_idx);
    }
    else return *this;	/* invalid */
}


/* string */
/**
 * @brief  文字列値でセルに値をそのまま代入 (低レベル)
 *
 *  文字列値でセルに値をそのまま代入します．<br>
 *  バイナリテーブルのカラムの型が FITS::ASCII_T (TFORMn の指定に 'A' を含む) 
 *  の場合，またはアスキーテーブルの場合に最速でアクセスできます．<br>
 *  カラムの型が FITS::ASCII_T の場合，value に NULL を与えると「文字列の低レ
 *  ベルなNULL値」をセットします(TNULLn との比較は行ないません)．FITS::ASCII_T
 *  以外の場合は，value に NULL を与えると FITS 規約に従い，セルに NULL値を
 *  セットします．
 *
 * @param   value 代入する値．
 * @param   row_index 行インデックス
 * @return  自身の参照
 * @attention  可変長配列はサポートされていません．
 * @note  詳細な動作: 文字列型のカラムの場合，value が NULL の場合はセルが '\0'
 *        で埋められますが，value が "" の場合はセルが空白文字で埋められます．
 * @note  SFITSIO では，カラム型が FITS::ASCII_T (TFORMn の指定に 'A' を含む) 
 *        の場合，TNULLn の設定がなければ，先頭文字が '\0' のセルを NULL値とし
 *        ます．これを「文字列の低レベルなNULL値」と呼んでいます．
 */
fits_table_col &fits_table_col::assign_string( const char *value, 
					       long row_index )
{
    if ( row_index < 0 || this->row_size_rec <= row_index ) {
	return *this;	/* invalid */
    }
    //if ( value == NULL ) return *this;

    if ( this->type_rec == FITS::STRING_T ) {
	long i;
	char *d_ptr = (char *)this->data_ptr();
	if ( d_ptr == NULL ) return *this;	/* invalid */
	d_ptr += this->bytes_rec * this->elem_size_rec * row_index;
	if ( value == NULL ) {
	    for ( i=0 ; i < this->bytes_rec ; i++ ) d_ptr[i] = '\0';
	}
	else {
	    //for ( ; *value == ' ' ; value++ );
	    for ( i=0 ; i < this->bytes_rec && value[i] != '\0' ; i++ )
		d_ptr[i] = value[i];
	    for ( ; i < this->bytes_rec ; i++ )
		d_ptr[i] = ' ';
	}
	return *this;
    }
    else {
	tstring sval;
	sval.assign(value);
	REMOVE_SPC(sval);
	if ( this->type_rec == FITS::LOGICAL_T ) {
	    int v;
	    double dval;
	    size_t epos;
	    dval = sval.strtod(&epos);
	    if ( epos == 0 ) {
		if ( sval.cchr(0) == 'T' || sval.cchr(0) == 't' ) v = 'T';
		else if ( sval.cchr(0) == 'F' || sval.cchr(0) == 'f' ) v = 'F';
		else v = '\0';
	    }
	    else {
		if ( !isfinite(dval) ) v = '\0';
		else v = (round(dval) != 0.0) ? 'T' : 'F';
	    }
	    return this->assign_logical(v,row_index);
	}
	else if ( this->type_rec == FITS::BIT_T ) {
	    double dv = sval.atof();
	    long long v;
	    if ( isfinite(dv) && MIN_DOUBLE_ROUND_LLONG <= dv && dv <= MAX_DOUBLE_ROUND_LLONG )
		v = round_d2ll(dv);
	    else v = this->tnull_w_rec;
	    return this->assign_bit(v,row_index);
	}
	else if ( value == NULL ) {
	    return this->assign_double(NAN,row_index);
	}
	else {
	    return this->assign_double(sval.atof(),row_index);
	}
    }
}

/**
 * @brief  文字列値でセルに値をそのまま代入 (低レベル)
 *
 *  文字列値でセルに値をそのまま代入します．<br>
 *  バイナリテーブルのカラムの型が FITS::ASCII_T (TFORMn の指定に 'A' を含む) 
 *  の場合，またはアスキーテーブルの場合に最速でアクセスできます．<br>
 *  カラムの型が FITS::ASCII_T の場合，value に NULL を与えると「文字列の低レ
 *  ベルなNULL値」をセットします(TNULLn との比較は行ないません)．FITS::ASCII_T
 *  以外の場合は，value に NULL を与えると FITS 規約に従い，セルに NULL値を
 *  セットします．
 *
 * @param   value 代入する値．
 * @param   row_index 行インデックス
 * @param   elem_name 要素名 (TELEMn に存在する名前)
 * @param   repetition_idx  TDIMn の2次元目のインデックス
 * @return  自身の参照
 * @attention  可変長配列はサポートされていません．
 * @note  詳細な動作: 文字列型のカラムの場合，value が NULL の場合はセルが '\0'
 *        で埋められますが，value が "" の場合はセルが空白文字で埋められます．
 * @note  SFITSIO では，カラム型が FITS::ASCII_T (TFORMn の指定に 'A' を含む) 
 *        の場合，TNULLn の設定がなければ，先頭文字が '\0' のセルを NULL値とし
 *        ます．これを「文字列の低レベルなNULL値」と呼んでいます．
 */
fits_table_col &fits_table_col::assign_string( const char *value, 
				   long row_index,
				   const char *elem_name, long repetition_idx )
{
    long elem_index = this->elem_index_rec.index(elem_name,0);
    if ( this->type_rec == FITS::BIT_T ) {
	tstring sval;
	sval.assign(value);
	REMOVE_SPC(sval);
	double dv = sval.atof();
	long long v;
	if ( isfinite(dv) && MIN_DOUBLE_ROUND_LLONG <= dv && dv <= MAX_DOUBLE_ROUND_LLONG )
	    v = round_d2ll(dv);
	else v = this->tnull_w_rec;
	return this->assign_bit(v,row_index,elem_index,repetition_idx,0);
    }
    else {
	return this->assign_string( value, 
				    row_index, elem_index, repetition_idx );
    }
}

/**
 * @brief  文字列値でセルに値をそのまま代入 (低レベル)
 *
 *  文字列値でセルに値をそのまま代入します．<br>
 *  バイナリテーブルのカラムの型が FITS::ASCII_T (TFORMn の指定に 'A' を含む) 
 *  の場合，またはアスキーテーブルの場合に最速でアクセスできます．<br>
 *  カラムの型が FITS::ASCII_T の場合，value に NULL を与えると「文字列の低レ
 *  ベルなNULL値」をセットします(TNULLn との比較は行ないません)．FITS::ASCII_T
 *  以外の場合は，value に NULL を与えると FITS 規約に従い，セルに NULL値を
 *  セットします．
 *
 * @param   value 代入する値．
 * @param   row_index 行インデックス
 * @param   elem_index 要素インデックス (TDIMn の1次元目のインデックス)
 * @param   repetition_idx  TDIMn の2次元目のインデックス
 * @return  自身の参照
 * @attention  可変長配列はサポートされていません．
 * @note  詳細な動作: 文字列型のカラムの場合，value が NULL の場合はセルが '\0'
 *        で埋められますが，value が "" の場合はセルが空白文字で埋められます．
 * @note  SFITSIO では，カラム型が FITS::ASCII_T (TFORMn の指定に 'A' を含む) 
 *        の場合，TNULLn の設定がなければ，先頭文字が '\0' のセルを NULL値とし
 *        ます．これを「文字列の低レベルなNULL値」と呼んでいます．
 */
fits_table_col &fits_table_col::assign_string( const char *value, 
					 long row_index,
					 long elem_index, long repetition_idx )
{
    if ( row_index < 0 || this->row_size_rec <= row_index ) {
	return *this;	/* invalid */
    }
    if ( elem_index < 0 || repetition_idx < 0 ) return *this;	/* invalid */
    //if ( value == NULL ) return *this;

    if ( this->type_rec == FITS::STRING_T ) {
	long i;
	char *d_ptr = (char *)this->data_ptr();
	long e_idx;
	if ( d_ptr == NULL ) return *this;	/* invalid */
	e_idx = this->dcol_size_rec * repetition_idx
	    + elem_index;
	if ( this->elem_size_rec <= e_idx ) 
	    e_idx = this->elem_size_rec - this->dcol_size_rec 
		+ elem_index % this->dcol_size_rec;
	d_ptr += 
	    this->bytes_rec * (this->elem_size_rec * row_index + e_idx);
	if ( value == NULL ) {
	    for ( i=0 ; i < this->bytes_rec ; i++ ) d_ptr[i] = '\0';
	}
	else {
	    //for ( ; *value == ' ' ; value++ );
	    for ( i=0 ; i < this->bytes_rec && value[i] != '\0' ; i++ )
		d_ptr[i] = value[i];
	    for ( ; i < this->bytes_rec ; i++ )
		d_ptr[i] = ' ';
	}
	return *this;
    }
    else {
	tstring sval;
	sval.assign(value);
	REMOVE_SPC(sval);
	if ( this->type_rec == FITS::LOGICAL_T ) {
	    int v;
	    double dval;
	    size_t epos;
	    dval = sval.strtod(&epos);
	    if ( epos == 0 ) {
		if ( sval.cchr(0) == 'T' || sval.cchr(0) == 't' ) v = 'T';
		else if ( sval.cchr(0) == 'F' || sval.cchr(0) == 'f' ) v = 'F';
		else v = '\0';
	    }
	    else {
		if ( !isfinite(dval) ) v = '\0';
		else v = (round(dval) != 0.0) ? 'T' : 'F';
	    }
	    return this->assign_logical(v,row_index,elem_index,repetition_idx);
	}
	else if ( this->type_rec == FITS::BIT_T ) {
	    double dv = sval.atof();
	    long long v;
	    if ( isfinite(dv) && MIN_DOUBLE_ROUND_LLONG <= dv && dv <= MAX_DOUBLE_ROUND_LLONG )
		v = round_d2ll(dv);
	    else v = this->tnull_w_rec;
	    return this->assign_bit(v,row_index,elem_index,repetition_idx);
	}
	else if ( value == NULL ) {
	    return this->assign_double(NAN,row_index,elem_index,repetition_idx);
	}
	else {
	    return this->assign_double(sval.atof(),
				       row_index,elem_index,repetition_idx);
	}
    }
}


/* bit */
/**
 * @brief  整数値(bit型)でセルに値をそのまま代入 (低レベル)
 *
 *  整数値(bit型)でセルに値をそのまま代入します．<br>
 *  カラムの型が FITS::BIT_T (TFORMn の指定に 'X' を含む) の場合に最速で
 *  アクセスでき，セルのバイト要素の左端のビットの値をセットします．
 *
 * @param   value 代入する値．
 * @param   row_index 行インデックス
 * @return  自身の参照
 * @attention  可変長配列はサポートされていません．
 */
fits_table_col &fits_table_col::assign_bit( long long value, long row_index )
{
    if ( row_index < 0 || this->row_size_rec <= row_index ) {
	return *this;	/* invalid */
    }

    if ( this->type_rec == FITS::BIT_T ) {
	/* MSBに書く事 */
	unsigned char *d_ptr = (unsigned char *)this->data_ptr();
	if ( d_ptr == NULL ) return *this;	/* invalid */
	if ( value != 0 ) 
	    d_ptr[this->full_bytes_rec * row_index] |= 0x080;
	else 
	    d_ptr[this->full_bytes_rec * row_index] &= ~(0x080);
	return *this;
    }
    else if ( this->type_rec == FITS::LOGICAL_T ) {
	return this->assign_logical((value != 0) ? 'T' : 'F',
				    row_index);
    }
    else if ( this->type_rec == FITS::BYTE_T ) {
	unsigned char v;
	if ( MIN_UCHAR <= value && value <= MAX_UCHAR ) v = value;
	else v = this->tnull_w_rec;
	return this->assign_byte(v,row_index);
    }
    else if ( this->type_rec == FITS::SHORT_T ) {
	short v;
	if ( MIN_SHORT <= value && value <= MAX_SHORT ) v = value;
	else v = this->tnull_w_rec;
	return this->assign_short(v,row_index);
    }
    else if ( this->type_rec == FITS::LONG_T ) {
	long v;
	if ( MIN_LONG <= value && value <= MAX_LONG ) v = value;
	else v = this->tnull_w_rec;
	return this->assign_long(v,row_index);
    }
    else if ( this->type_rec == FITS::LONGLONG_T ) {
	return this->assign_longlong(value,row_index);
    }
    else if ( this->type_rec == FITS::FLOAT_T ) {
	return this->assign_float(value,row_index);
    }
    else if ( this->type_rec == FITS::DOUBLE_T ) {
	return this->assign_double(value,row_index);
    }
    else if ( this->type_rec == FITS::STRING_T ) {
	tstring buf(64);
	buf.printf("%lld",value);
	return this->assign_string(buf.cstr(), row_index);
    }
    else return *this;	/* invalid */
}

/**
 * @brief  整数値(bit型)でセルに値をそのまま代入 (低レベル)
 *
 *  整数値(bit型)でセルに値をそのまま代入します．<br>
 *  カラムの型が FITS::BIT_T (TFORMn の指定に 'X' を含む) の場合に使用する
 *  メンバ関数で，任意のビット要素から右方向に任意のビット数の整数値をセット
 *  できます．
 *
 * @param   value 代入する値．
 * @param   row_index 行インデックス
 * @param   elem_name 要素名 (TELEMn に存在する名前)
 * @param   repetition_idx  TDIMn の2次元目のインデックス
 * @param   nbit ビット幅 (省略時は TELEMn の値のビットフィールド指定を適用)
 * @return  自身の参照
 * @attention  可変長配列はサポートされていません．
 * @note  ビット演算を行なうため，実行コストが大きいかもしれません．
 */
fits_table_col &fits_table_col::assign_bit( long long value, long row_index,
			 const char *elem_name, long repetition_idx, int nbit )
{
    long elem_index = this->elem_index_rec.index(elem_name,0);
    return this->assign_bit( value, 
			     row_index, elem_index, repetition_idx, nbit );
}

/**
 * @brief  整数値(bit型)でセルに値をそのまま代入 (低レベル)
 *
 *  整数値(bit型)でセルに値をそのまま代入します．<br>
 *  カラムの型が FITS::BIT_T (TFORMn の指定に 'X' を含む) の場合に使用する
 *  メンバ関数で，任意のビット要素から右方向に任意のビット数の整数値をセット
 *  できます．
 *
 * @param   value 代入する値．
 * @param   row_index 行インデックス
 * @param   elem_index 要素インデックス (TDIMn の1次元目のインデックス)
 * @param   repetition_idx  TDIMn の2次元目のインデックス
 * @param   nbit ビット幅 (省略時は 1 を適用)
 * @return  自身の参照
 * @attention  可変長配列はサポートされていません．
 * @note  ビット演算を行なうため，実行コストが大きいかもしれません．
 */
fits_table_col &fits_table_col::assign_bit( long long value, long row_index,
			       long elem_index, long repetition_idx, int nbit )
{
    if ( row_index < 0 || this->row_size_rec <= row_index ) {
	return *this;	/* invalid */
    }
    if ( elem_index < 0 || repetition_idx < 0 ) return *this;	/* invalid */

    if ( this->type_rec == FITS::BIT_T ) {
	unsigned char *d_ptr = (unsigned char *)this->data_ptr();
	long e_idx;
	int n_bit;
	long e_mod, e_div, e_align;
	int i,j;
	if ( d_ptr == NULL ) return *this;	/* invalid */
	e_idx = this->dcol_size_rec * repetition_idx 
	    + elem_index;
	if ( nbit < 1 ) 
	    n_bit = this->bit_size_telem.i(elem_index % this->dcol_size_rec);
	else
	    n_bit = nbit;
	if ( this->elem_size_rec <= e_idx ) {
	    e_idx = this->elem_size_rec - this->dcol_size_rec 
		+ elem_index % this->dcol_size_rec;
	}
	d_ptr += this->full_bytes_rec * row_index;
	e_align = (n_bit-1) % 8;
	e_mod = e_idx % 8;
	e_div = e_idx / 8;
	/* */
	j = (n_bit - 1) / 8;
	//sli__eprintf("debug: e_align=%ld e_mod=%ld e_div=%ld j=%d n_bit=%d\n",
	//		       e_align,e_mod,e_div,j,n_bit);
	/* */
	if ( e_mod + e_align < 8 ) {		/* case A : 右へシフトする */
	    int n_shift = 7 - (e_mod + e_align);	/* シフトするbit数 */
	    unsigned char mask;
	    int sft;
	    i = n_bit;					/* 未処理bit数 */
	    mask = (unsigned char)0xff >> (e_mod + n_shift);
	    mask <<= n_shift;
	    d_ptr[e_div] &= ~mask;	/* clear */
	    sft = 8 * j - n_shift;
	    //sli__eprintf("debug0: n_shift=%d mask=[%hhx] sft=%d\n",n_shift,mask,sft);
	    //sli__eprintf("debug0a: i=%d\n",i);
	    if ( 0 <= sft )
		d_ptr[e_div] |= ((value >> sft) & mask);
	    else 
		d_ptr[e_div] |= ((value << -sft) & mask);
	    i -= 1 + e_align;
	    //sli__eprintf("debug0b: i=%d\n",i);
	    while ( 0 < i && 0 < j ) {
		j--;
		mask = (unsigned char)0xff >> (8 - n_shift);
		d_ptr[e_div] &= ~mask;	/* clear */
		//sli__eprintf("debug0L1: mask=[%hhx] sft=%d\n",mask,sft);
		if ( 0 <= sft )
		    d_ptr[e_div] |= ((value >> sft) & mask);
		else
		    d_ptr[e_div] |= ((value << -sft) & mask);
		i -= n_shift;
		e_div ++;
		mask = (unsigned char)0xff << n_shift;
		d_ptr[e_div] &= ~mask;	/* clear */
		sft = 8 * j - n_shift;
		//sli__eprintf("debug0L2:  mask=[%hhx] sft=%d\n",mask,sft);
		if ( 0 <= sft )
		    d_ptr[e_div] |= ((value >> sft) & mask);
		else
		    d_ptr[e_div] |= ((value << -sft) & mask);
		i -= 8 - n_shift;
	    }
	}
	else {					/* case B : 左へシフトする */
	    int n_shift = e_mod + e_align - 7;		/* シフトするbit数 */
	    unsigned char mask;
	    int sft;
	    i = n_bit;
	    mask = (unsigned char)0xff >> e_mod;
	    //sli__eprintf("debug1: mask = [%hhx]\n",mask);
	    d_ptr[e_div] &= ~mask;	/* clear */
	    sft = 8 * j + n_shift;
	    if ( 0 <= sft ) 
		d_ptr[e_div] |= ((value >> sft) & mask);
	    else 
		d_ptr[e_div] |= ((value << -sft) & mask);
	    i -= 8 - e_mod;
	    e_div ++;
	    mask = (unsigned char)0xff << (8 - n_shift);
	    d_ptr[e_div] &= ~mask;	/* clear */
	    sft = 8 * j - (8 - n_shift);
	    if ( 0 <= sft )
		d_ptr[e_div] |= ((value >> sft) & mask);
	    else
		d_ptr[e_div] |= ((value << -sft) & mask);
	    i -= n_shift;
	    while ( 0 < i && 0 < j ) {
		j--;
		mask = (unsigned char)0xff >> n_shift;
		d_ptr[e_div] &= ~mask;	/* clear */
		sft = 8 * j + n_shift;
		if ( 0 <= sft )
		    d_ptr[e_div] |= ((value >> sft) & mask);
		else
		    d_ptr[e_div] |= ((value << -sft) & mask);
		i -= 8 - n_shift;
		e_div ++;
		mask = (unsigned char)0xff << (8 - n_shift);
		d_ptr[e_div] &= ~mask;	/* clear */
		sft = 8 * j - (8 - n_shift);
		if ( 0 <= sft )
		    d_ptr[e_div] |= ((value >> sft) & mask);
		else
		    d_ptr[e_div] |= ((value << -sft) & mask);
		i -= n_shift;
	    }
	}
	return *this;
    }
    else if ( this->type_rec == FITS::LOGICAL_T ) {
	return this->assign_logical((value != 0) ? 'T' : 'F',
				    row_index,elem_index,repetition_idx);
    }
    else if ( this->type_rec == FITS::BYTE_T ) {
	unsigned char v;
	if ( MIN_UCHAR <= value && value <= MAX_UCHAR ) v = value;
	else v = this->tnull_w_rec;
	return this->assign_byte(v,row_index,elem_index,repetition_idx);
    }
    else if ( this->type_rec == FITS::SHORT_T ) {
	short v;
	if ( MIN_SHORT <= value && value <= MAX_SHORT ) v = value;
	else v = this->tnull_w_rec;
	return this->assign_short(v,row_index,elem_index,repetition_idx);
    }
    else if ( this->type_rec == FITS::LONG_T ) {
	long v;
	if ( MIN_LONG <= value && value <= MAX_LONG ) v = value;
	else v = this->tnull_w_rec;
	return this->assign_long(v,row_index,elem_index,repetition_idx);
    }
    else if ( this->type_rec == FITS::LONGLONG_T ) {
	return this->assign_longlong(value,row_index,elem_index,repetition_idx);
    }
    else if ( this->type_rec == FITS::FLOAT_T ) {
	return this->assign_float(value,row_index,elem_index,repetition_idx);
    }
    else if ( this->type_rec == FITS::DOUBLE_T ) {
	return this->assign_double(value,row_index,elem_index,repetition_idx);
    }
    else if ( this->type_rec == FITS::STRING_T ) {
	tstring buf(64);
	buf.printf("%lld",value);
	return this->assign_string(buf.cstr(), row_index, elem_index,
				   repetition_idx);
    }
    else return *this;	/* invalid */
}

/**
 * @brief  可変長配列の配列記述子をセット (低レベル)
 *
 *  elem_idx は，可変長配列の配列記述子が複数存在する場合に指定します．
 *  例えば TFORM の定義が '4PE(999)' の場合 elem_idx は 0 〜 3 の値をとる事が
 *  できます．
 *
 * @param   length 配列記述子: 要素数
 * @param   offset 配列記述子: バイトオフセット
 * @param   row_index 行インデックス
 * @param   elem_idx 配列記述子の要素番号 (省略時は0)
 * @return  自身の参照
 */
fits_table_col &fits_table_col::assign_arrdesc( long length, long offset,
						long row_index, long elem_idx )
{
    if ( row_index < 0 || this->row_size_rec <= row_index ||
	 elem_idx < 0 || this->elem_size_rec <= elem_idx ) {
	return *this;	/* invalid */
    }

    if ( this->type_rec == FITS::LONGARRDESC_T ) {
	fits::longarrdesc_t *d_ptr = (fits::longarrdesc_t *)this->data_ptr();
	if ( d_ptr == NULL ) return *this;	/* invalid */
	d_ptr[this->elem_size_rec * row_index + elem_idx].length = length;
	d_ptr[this->elem_size_rec * row_index + elem_idx].offset = offset;
	if ( this->vl_max_length_rec < length ) {
	    this->vl_max_length_rec = length;
	}
    }
    else if ( this->type_rec == FITS::LLONGARRDESC_T ) {
	fits::llongarrdesc_t *d_ptr = (fits::llongarrdesc_t *)this->data_ptr();
	if ( d_ptr == NULL ) return *this;	/* invalid */
	d_ptr[this->elem_size_rec * row_index + elem_idx].length = length;
	d_ptr[this->elem_size_rec * row_index + elem_idx].offset = offset;
	if ( this->vl_max_length_rec < length ) {
	    this->vl_max_length_rec = length;
	}
    }

    return *this;
}


/**
 * @brief  row用メモリ確保方法の設定を変更 (カラム毎の設定)
 *
 *  row用メモリを確保する時に，どのような方法で行なうかを決定します．
 *  次の 3 つから選択します．<br>
 *    "min", "auto"  ... 最小限を確保．リサイズ時に必ずrealloc()が呼ばれる<br>
 *    "pow"  ... 2のn乗で確保<br>
 *    NULL，他 ... 現在の方法を維持
 *
 * @param   strategy メモリ確保方法の設定
 * @return  自身の参照
 *
 */
fits_table_col &fits_table_col::set_alloc_strategy( const char *strategy )
{
    this->data_rec.set_alloc_strategy(strategy);
    return *this;
}


/* */


/**
 * @brief  shallow copy を許可する場合に使用 (未実装)
 * @note   一時オブジェクトの return の直前で使用する．
 */
/* 注意: 参照を返したいところだが，return 文でそれをやるとインスタンス */
/*       を2回コピーされるというおかしな事になるので void になっている */
void fits_table_col::set_scopy_flag()
{
    this->shallow_copy_ok = true;
    return;
}

/* protected */

/**
 * @brief  将来の実装のためのメンバ関数
 *
 * @note   このメンバ関数は protected です．
 */
fits_table_col &fits_table_col::set_protection( bool protect )
{
    this->protected_rec = protect;
    return *this;
}

/**
 * @brief  自身を管理する fits_table オブジェクトを登録
 *
 * @note   このメンバ関数は protected です．
 */
fits_table_col &fits_table_col::register_manager( fits_table *ptr )
{
    this->manager = ptr;
    return *this;
}

/* private */

/**
 * @brief  内部用 tnull 各値をセット or リセット (引数がNULLでリセット)
 *
 * @note   このメンバ関数は private です．
 */
void fits_table_col::set_tnull( const long long *new_tnull_ptr )
{
    int the_type = this->type_rec;

    if ( new_tnull_ptr == NULL ) {
      this->tnull_r_rec = INDEF_LLONG;
      this->tnull_w_rec = get_indef_for_type(the_type);
      this->tnull_longlong_rec = INDEF_LLONG;
      this->tnull_long_rec = INDEF_LONG;
      this->tnull_short_rec = INDEF_SHORT;
      this->tnull_byte_rec = INDEF_UCHAR;
    }
    else {
      this->tnull_r_rec = *new_tnull_ptr;
      this->tnull_w_rec = get_tnull_for_type(the_type, *new_tnull_ptr);
      if ( MIN_LLONG <= *new_tnull_ptr && *new_tnull_ptr <= MAX_LLONG )
	  this->tnull_longlong_rec = *new_tnull_ptr;
      else this->tnull_longlong_rec = INDEF_LLONG;
      if ( MIN_LONG <= *new_tnull_ptr && *new_tnull_ptr <= MAX_LONG ) 
	  this->tnull_long_rec = *new_tnull_ptr;
      else this->tnull_long_rec = INDEF_LONG;
      if ( MIN_SHORT <= *new_tnull_ptr && *new_tnull_ptr <= MAX_SHORT ) 
	  this->tnull_short_rec = *new_tnull_ptr;
      else this->tnull_short_rec = INDEF_SHORT;
      if ( MIN_UCHAR <= *new_tnull_ptr && *new_tnull_ptr <= MAX_UCHAR ) 
	  this->tnull_byte_rec = *new_tnull_ptr;
      else this->tnull_byte_rec = INDEF_UCHAR;
    }

    return;
}

/**
 * @brief  this->def_all_rec，this->def_rec を更新
 *
 * @note   このメンバ関数は private です．
 */
void fits_table_col::update_def_rec_ptr()
{
    /* all version */
    this->def_all_rec.ttype = this->tany.cstr(TTYPE_IDX);
    this->def_all_rec.ttype_comment = this->tany.cstr(TTYPE_COMMENT_IDX);
    this->def_all_rec.tunit = this->tany.cstr(TUNIT_IDX);
    this->def_all_rec.tunit_comment = this->tany.cstr(TUNIT_COMMENT_IDX);
    this->def_all_rec.tdisp = this->tany.cstr(TDISP_IDX);
    this->def_all_rec.tdisp_comment = this->tany.cstr(TDISP_COMMENT_IDX);
    this->def_all_rec.tform = this->tany.cstr(TFORM_IDX);
    this->def_all_rec.tform_comment = this->tany.cstr(TFORM_COMMENT_IDX);
    this->def_all_rec.tdim  = this->tany.cstr(TDIM_IDX);
    this->def_all_rec.tdim_comment  = this->tany.cstr(TDIM_COMMENT_IDX);
    this->def_all_rec.tnull = this->tany.cstr(TNULL_IDX);
    this->def_all_rec.tnull_comment = this->tany.cstr(TNULL_COMMENT_IDX);
    this->def_all_rec.tzero = this->tany.cstr(TZERO_IDX);
    this->def_all_rec.tzero_comment = this->tany.cstr(TZERO_COMMENT_IDX);
    this->def_all_rec.tscal = this->tany.cstr(TSCAL_IDX);
    this->def_all_rec.tscal_comment = this->tany.cstr(TSCAL_COMMENT_IDX);

    //this->def_all_rec.talas = this->talas.cstrarray();
    this->def_all_rec.talas = this->tany.cstr(TALAS_IDX);
    this->def_all_rec.talas_comment = this->tany.cstr(TALAS_COMMENT_IDX);
    //this->def_all_rec.telem = this->telem_def.cstrarray();
    this->def_all_rec.telem = this->tany.cstr(TELEM_IDX);
    this->def_all_rec.telem_comment = this->tany.cstr(TELEM_COMMENT_IDX);

    /* normal version */
    this->def_rec.ttype = this->tany.cstr(TTYPE_IDX);
    this->def_rec.ttype_comment = this->tany.cstr(TTYPE_COMMENT_IDX);
    this->def_rec.tunit = this->tany.cstr(TUNIT_IDX);
    this->def_rec.tunit_comment = this->tany.cstr(TUNIT_COMMENT_IDX);
    this->def_rec.tdisp = this->tany.cstr(TDISP_IDX);
    this->def_rec.tform = this->tany.cstr(TFORM_IDX);
    this->def_rec.tdim  = this->tany.cstr(TDIM_IDX);
    this->def_rec.tnull = this->tany.cstr(TNULL_IDX);
    this->def_rec.tzero = this->tany.cstr(TZERO_IDX);
    this->def_rec.tscal = this->tany.cstr(TSCAL_IDX);

    //this->def_rec.talas = this->talas.cstrarray();
    //this->def_rec.telem = this->telem_def.cstrarray();
    this->def_rec.talas = this->tany.cstr(TALAS_IDX);
    this->def_rec.telem = this->tany.cstr(TELEM_IDX);

    return;
}

/**
 * @brief  デフォルト値にゼロをセット
 *
 * @note   このメンバ関数は private です．
 */
void fits_table_col::update_zero_of_data_rec()
{
    if ( this->type_rec == FITS::LONGARRDESC_T ||
	 this->type_rec == FITS::LLONGARRDESC_T  ) {
	this->update_zero_of_data_rec( this->heap_type_rec, true );
    }
    else {
	this->update_zero_of_data_rec( this->type_rec, false );
    }
}

/* set default value of internal memory buffer */
/**
 * @brief  デフォルト値にゼロをセット
 *
 * @note   このメンバ関数は private です．
 */
void fits_table_col::update_zero_of_data_rec( int new_type, bool is_heap )
{
    double zval = 0;
    mdarray *tgt_buf_rec;

    zval -= this->tzero_rec;
    zval /= this->tscal_rec;

    if ( is_heap == true ) {
	fits::llongarrdesc_t v = {0,0};	      /* largest heap pointer type */
	tgt_buf_rec = &(this->heap_rec);
	this->data_rec.assign_default((const void *)&v);
    }
    else {
	fits::doublecomplex_t v = 0 + 0i;     /* largest data type: dcomplex */
	tgt_buf_rec = &(this->data_rec);
	this->heap_rec.assign_default((const void *)&v);
    }

    if ( new_type == FITS::DOUBLE_T ) {
	fits::double_t v = zval;
	tgt_buf_rec->assign_default((const void *)&v);
    }
    else if ( new_type == FITS::FLOAT_T ) {
	fits::float_t v = zval;
	tgt_buf_rec->assign_default((const void *)&v);
    }
    else if ( new_type == FITS::SHORT_T ) {
	if ( MIN_DOUBLE_ROUND_INT16 <= zval && zval <= MAX_DOUBLE_ROUND_INT16 ) {
	    fits::short_t v = round_d2i32(zval);
	    tgt_buf_rec->assign_default((const void *)&v);
	}
	else tgt_buf_rec->assign_default((const void *)&INDEF_INT16);
    }
    else if ( new_type == FITS::LONG_T ) {
	if ( MIN_DOUBLE_ROUND_INT32 <= zval && zval <= MAX_DOUBLE_ROUND_INT32 ) {
	    fits::long_t v = round_d2i32(zval);
	    tgt_buf_rec->assign_default((const void *)&v);
	}
	else tgt_buf_rec->assign_default((const void *)&INDEF_INT32);
    }
    else if ( new_type == FITS::BYTE_T ) {
	if ( MIN_DOUBLE_ROUND_UCHAR <= zval && zval <= MAX_DOUBLE_ROUND_UCHAR ) {
	    fits::byte_t v = round_d2i32(zval);
	    tgt_buf_rec->assign_default((const void *)&v);
	}
	else tgt_buf_rec->assign_default((const void *)&INDEF_UCHAR);
    }
    else if ( new_type == FITS::LONGLONG_T ) {
	if ( MIN_DOUBLE_ROUND_INT64 <= zval && zval <= MAX_DOUBLE_ROUND_INT64 ) {
	    fits::longlong_t v = round_d2i64(zval);
	    tgt_buf_rec->assign_default((const void *)&v);
	}
	else tgt_buf_rec->assign_default((const void *)&INDEF_INT64);
    }
    else if ( new_type == FITS::DOUBLECOMPLEX_T ) {
	fits::doublecomplex_t v = zval + 0.0i;
	tgt_buf_rec->assign_default((const void *)&v);
    }
    else if ( new_type == FITS::COMPLEX_T ) {
	fits::complex_t v = zval + 0.0i;
	tgt_buf_rec->assign_default((const void *)&v);
    }
    else if ( new_type == FITS::LOGICAL_T ) {
	fits::logical_t v = 'F';
	tgt_buf_rec->assign_default((const void *)&v);
    }
    else if ( new_type == FITS::STRING_T ) {
	/* fits::byte_t v = ' '; */
	tstring dummy;
	dummy.resize(this->data_rec.bytes()).clean(' ');
	tgt_buf_rec->assign_default((const void *)dummy.cstr());
    }
    else {	/* BIT_T */
	tgt_buf_rec->assign_default((const void *)NULL);
    }
}

/*
 * BEGIN : convert_col_type()
 *
 * 注意: fits_image には convert_image_type() が存在する．内容は全く同じ
 *       なので，コードを変更する場合は同時に行なう事．
 */

struct fits_table_col_cnv_prms {
    /* アライメントに注意(sizeofが長いものから順に!) */
    double new_min;
    double new_max;
    double old_zero;
    double new_zero;
    double old_scale;
    double new_scale;
    long long old_null;
    long long new_null;
    bool old_null_is_set;
};

namespace cnv
{
#define MAKE_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
static void fncname(const void *org_val_ptr, void *new_val_ptr, size_t n, int f_b, void *u_ptr) \
{ \
    const struct fits_table_col_cnv_prms *p = (const struct fits_table_col_cnv_prms *)u_ptr; \
    const org_type *org_t_ptr = (const org_type *)org_val_ptr; \
    new_type *new_t_ptr = (new_type *)new_val_ptr; \
    const double p_new_min = p->new_min; \
    const double p_new_max = p->new_max; \
    const double p_old_zero = p->old_zero; \
    const double p_new_zero = p->new_zero; \
    const double p_old_scale = p->old_scale; \
    const double p_new_scale = p->new_scale; \
    double val; \
    size_t i, n_v; \
    ssize_t d; \
    if ( f_b < 0 ) { \
	new_t_ptr += n; \
	new_t_ptr --; \
	org_t_ptr += n; \
	org_t_ptr --; \
	d = -1; \
	n_v = n; \
    } \
    else if ( 0 < f_b ) { \
	d = 1; \
	n_v = n; \
    } \
    else { \
	d = 0; \
	n_v = ((0 < n) ? 1 : 0); \
    } \
    \
    if ( org_sz_type < 0 /* org is float type */ ) { \
      if ( new_sz_type < 0 /* new is float type */ ) { \
	for ( i=0 ; i < n_v ; i++ ) { \
	  if ( !isfinite(*org_t_ptr) ) { \
	    *new_t_ptr = (new_type)(NAN); \
	  } \
	  else { \
	    val = *org_t_ptr; \
	    val *= p_old_scale; \
	    val += p_old_zero; \
	    val -= p_new_zero; \
	    val /= p_new_scale; \
	    *new_t_ptr = (new_type)val; \
	  } \
	  new_t_ptr += d; \
	  org_t_ptr += d; \
	} \
	for ( ; i < n ; i++ ) new_t_ptr[i] = new_t_ptr[0]; \
      } \
      else { /* new is not float type */ \
	for ( i=0 ; i < n_v ; i++ ) { \
	  if ( !isfinite(*org_t_ptr) ) { \
	    *new_t_ptr = p->new_null; \
	  } \
	  else { \
	    val = *org_t_ptr; \
	    val *= p_old_scale; \
	    val += p_old_zero; \
	    val -= p_new_zero; \
	    val /= p_new_scale; \
	    if ( isfinite(val) && p_new_min <= val && val <= p_new_max ) \
	      *new_t_ptr = (new_type)fnc(val); \
	    else *new_t_ptr = p->new_null; \
	  } \
	  new_t_ptr += d; \
	  org_t_ptr += d; \
	} \
	for ( ; i < n ; i++ ) new_t_ptr[i] = new_t_ptr[0]; \
      } \
    } \
    else { /* org is not float type */ \
      if ( new_sz_type < 0 /* new is float type */ ) { \
	for ( i=0 ; i < n_v ; i++ ) { \
	  if ( p->old_null_is_set == true && p->old_null == *org_t_ptr ) { \
	    *new_t_ptr = (new_type)(NAN); \
	  } \
	  else { \
	    val = *org_t_ptr; \
	    val *= p_old_scale; \
	    val += p_old_zero; \
	    val -= p_new_zero; \
	    val /= p_new_scale; \
	    *new_t_ptr = (new_type)val; \
	  } \
	  new_t_ptr += d; \
	  org_t_ptr += d; \
	} \
	for ( ; i < n ; i++ ) new_t_ptr[i] = new_t_ptr[0]; \
      } \
      else { /* new is not float type */ \
	for ( i=0 ; i < n_v ; i++ ) { \
	  if ( p->old_null_is_set == true && p->old_null == *org_t_ptr ) { \
	    *new_t_ptr = p->new_null; \
	  } \
	  else { \
	    val = *org_t_ptr; \
	    val *= p_old_scale; \
	    val += p_old_zero; \
	    val -= p_new_zero; \
	    val /= p_new_scale; \
	    if ( isfinite(val) && p_new_min <= val && val <= p_new_max ) \
	      *new_t_ptr = (new_type)fnc(val); \
	    else *new_t_ptr = p->new_null; \
	  } \
	  new_t_ptr += d; \
	  org_t_ptr += d; \
	} \
	for ( ; i < n ; i++ ) new_t_ptr[i] = new_t_ptr[0]; \
      } \
    } \
}
SLI__MDARRAY__DO_OPERATION_2TYPES(MAKE_FUNC,round_f2i32,round_f2i32,round_f2i32,round_f2i64,round_d2i32,round_d2i32,round_d2i32,round_d2i64,,,,,);
#undef MAKE_FUNC
}

/**
 * @brief  ピクセル値の変換時に mdarray の標準変換で良いかどうかを判定 (内部用)
 *
 * @note    private な関数です．
 */
static bool simple_conv_is_ok( const fits_table_col &src, 
			       int dst_type, long dst_bytes, 
			       double dst_tzero, double dst_tscal,
			       bool dst_tnull_is_set, long long dst_tnull )
{
    bool do_simple_conv = false;
    bool src_is_float_type = false;
    bool dst_is_float_type = false;

    if ( src.type() == FITS::FLOAT_T || src.type() == FITS::DOUBLE_T ) {
	src_is_float_type = true;
    }
    if ( dst_type == FITS::FLOAT_T || dst_type == FITS::DOUBLE_T ) {
	dst_is_float_type = true;
    }

    /* 両方とも浮動小数点数の場合 */
    if ( src_is_float_type == true && dst_is_float_type == true &&
	 src.tzero()  == dst_tzero && 
	 src.tscal() == dst_tscal ) {
	do_simple_conv = true;
    }
    /* 両方とも整数の場合 */
    else if ( src_is_float_type == false && 
	      dst_is_float_type == false &&
	      src.bytes() <= dst_bytes &&
	      src.tzero()        == dst_tzero &&
	      src.tscal()       == dst_tscal &&
	      ((src.tnull_is_set() == false && dst_tnull_is_set == false) ||
	       (src.tnull_is_set() == true && dst_tnull_is_set == true &&
		src.tnull() == dst_tnull)) ) {
	do_simple_conv = true;
    }
    /* 整数から float に変換される場合 */
    else if ( src_is_float_type == false && 
	      dst_is_float_type == true &&
	      src.tzero()        == dst_tzero &&
	      src.tscal()       == dst_tscal &&
	      src.tnull_is_set() == false ) {
	do_simple_conv = true;
    }

    return do_simple_conv;
}

/**
 * @brief  データ型の変換・変更 (低レベル)
 *
 * @note   このメンバ関数は private です．
 */
fits_table_col &fits_table_col::convert_col_type( int new_type, 
						const double *new_zero_ptr, 
						const double *new_scale_ptr,
						const long long *new_null_ptr )
{
    struct fits_table_col_cnv_prms prms;
    void (*func_cnv_ptr)(const void *,void *,size_t,int,void *);
    ssize_t org_size_type = this->data_rec.size_type();
    ssize_t new_size_type = org_size_type;
    ssize_t pos;
    int sym = -1;
    bool rounding_bak;

    if ( this->full_bytes_rec == 0 ) return *this;	/* not defined */

    pos = this->tany.at(TFORM_IDX).strpbrk("IJKEDB");
    if ( pos < 0 ) {
	err_report1(__FUNCTION__,"WARNING",
		 "cannot convert this TFORM : %s.",this->tany.cstr(TFORM_IDX));
	return *this;
    }

    prms.old_null = 0;
    prms.old_null_is_set = false;
    prms.new_zero = 0.0;
    prms.new_scale = 1.0;
    prms.new_null = 0;
    prms.new_min = 0.0;
    prms.new_max = 0.0;

    prms.old_zero = this->tzero();
    if ( new_zero_ptr != NULL ) prms.new_zero = *new_zero_ptr;

    prms.old_scale = this->tscal();
    if ( new_scale_ptr != NULL ) prms.new_scale = *new_scale_ptr;
    if ( prms.new_scale == 0 ) {
	err_report(__FUNCTION__,"WARNING",
		   "ZERO TSCAL cannot be set, 1.0 is used.");
	prms.new_scale = 1.0;
    }

    if ( this->type_rec == FITS::DOUBLE_T ||
	 this->type_rec == FITS::FLOAT_T ) {
    }
    else {
	prms.old_null = this->tnull();
	prms.old_null_is_set = this->tnull_is_set();
    }
    if ( new_type == FITS::DOUBLE_T ||
	 new_type == FITS::FLOAT_T ) {
	new_null_ptr = NULL;
    }
    else {
	if ( new_null_ptr != NULL ) {
	    prms.new_null = get_tnull_for_type(new_type, *(new_null_ptr));
	}
	else {
	    prms.new_null = get_indef_for_type(new_type);
	}
	prms.new_min = get_min_for_type(new_type);
	prms.new_max = get_max_for_type(new_type);
    }

    switch ( new_type ) {
    case FITS::DOUBLE_T:
	new_size_type = -sizeof(fits::double_t);
	sym = 'D';
	break;
    case FITS::FLOAT_T:
	new_size_type = -sizeof(fits::float_t);
	sym = 'E';
	break;
    case FITS::LONGLONG_T:
	new_size_type = sizeof(fits::longlong_t);
	sym = 'K';
	break;
    case FITS::LONG_T:
	new_size_type = sizeof(fits::long_t);
	sym = 'J';
	break;
    case FITS::SHORT_T:
	new_size_type = sizeof(fits::short_t);
	sym = 'I';
	break;
    case FITS::BYTE_T:
	new_size_type = sizeof(fits::byte_t);
	sym = 'B';
	break;
    default:
	err_report1(__FUNCTION__,"WARNING",
		    "unsuppoeted type %d; unchanged",new_type);
	new_type = this->type_rec;
	new_size_type = this->data_rec.size_type();
	break;
    }

    rounding_bak = this->data_rec.rounding();
    this->data_rec.set_rounding(true);

    try {

	if ( simple_conv_is_ok(*this, 
			   new_type, abs(new_size_type),
			   prms.new_zero, prms.new_scale,
			   (new_null_ptr != NULL), prms.new_null) == true ) {

	    this->data_rec.convert( new_size_type );

	}
	else {

	    func_cnv_ptr = NULL;

    /* 型変換のための関数を選択 */
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
    if ( org_size_type == org_sz_type && new_size_type == new_sz_type ) { \
	func_cnv_ptr = &cnv::fncname; \
    }
    SLI__MDARRAY__DO_OPERATION_2TYPES(SEL_FUNC,,,,,,,,,,,,,else);
#undef SEL_FUNC

	    if ( func_cnv_ptr == NULL ) {
		err_throw(__FUNCTION__,"FATAL",
			  "detected unexpected NULL pointer");
	    }

	    this->data_rec.convert_via_udf( new_size_type, 
					    func_cnv_ptr, (void *)&prms );

	}

    }
    catch (...) {
	this->data_rec.set_rounding(rounding_bak);
	err_throw(__FUNCTION__,"FATAL","caught exception");
    }

    this->data_rec.set_rounding(rounding_bak);

    this->type_rec = new_type;
    this->bytes_rec = this->data_rec.bytes();

    this->full_bytes_rec = this->data_rec.col_length() * this->bytes_rec;
    if ( 0 < sym ) this->tany.at(TFORM_IDX).at(pos) = sym;

    if ( new_zero_ptr != NULL ) {
	this->assign_tzero(prms.new_zero);
    }
    else {
	this->erase_tzero();
    }
    if ( new_scale_ptr != NULL ) {
	this->assign_tscal(prms.new_scale);
    }
    else {
	this->erase_tscal();
    }
    if ( new_null_ptr != NULL ) {
	this->assign_tnull(*new_null_ptr);
    }
    else {
	this->erase_tnull();
    }
    
    return *this;
}

/** 
 * @brief  shallow copy が可能かを返す (未実装)
 * 
 * @return  shallow copyが可能なら真<br>
 *          それ以外の時は偽
 * @note  このメンバ関数は private です
 */
bool fits_table_col::request_shallow_copy( fits_table_col *from_obj ) const
{
    return false;
}

/** 
 * @brief  shallow copy をキャンセルする (未実装)
 * 
 * @note  このメンバ関数は private です
 */
void fits_table_col::cancel_shallow_copy( fits_table_col *from_obj ) const
{
    return;
}

/**
 * @brief  自身について，shallow copy 関係のクリーンアップを行なう (未実装)
 * 
 * @note  このメンバ関数は private です
 */
void fits_table_col::cleanup_shallow_copy( bool do_deep_copy_for_this ) const
{
    return;
}


/**
 * @brief  inlineメンバ関数でthrowする時に使用
 *
 * @note   このメンバ関数は private です．
 */
void *fits_table_col::err_throw_void_p( const char *fnc, 
				    const char *lv, const char *mes)
{
    err_throw(fnc,lv,mes);
    return (void *)NULL;
}

/**
 * @brief  inlineメンバ関数でthrowする時に使用
 *
 * @note   このメンバ関数は private です．
 */
const void *fits_table_col::err_throw_const_void_p( const char *fnc, 
					 const char *lv, const char *mes) const
{
    err_throw(fnc,lv,mes);
    return (const void *)NULL;
}


}	/* namespace sli */

#include "private/c_memset.cc"

