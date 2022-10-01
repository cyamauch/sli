/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2022-10-01 00:00:00 cyamauch> */

/**
 * @file   fits_table_col.cc
 * @brief  ASCII or Binary Table��1������ɽ�����륯�饹fits_table_col�Υ�����
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

/* this->tany array ���ֹ� */
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
 * @brief  ̤����ͤ��֤� (������)
 *
 * @note    private �ʴؿ��Ǥ���
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
 * @brief  �񤭽Ф������� tnull �ͤ��֤� (������)
 *
 * @note    private �ʴؿ��Ǥ���
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
 * @brief  ���󥹥ȥ饯��
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
 * @brief  ���ԡ����󥹥ȥ饯��
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
 * @brief  �ǥ��ȥ饯��
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
 * @brief  ���֥������ȤΥ��ԡ�
 *
 * @attention  ����Ĺ����ξ�硤���󵭽һҤ��Ф������Ȥʤ�ޤ���
 */
fits_table_col &fits_table_col::operator=(const fits_table_col &obj)
{
    this->init(obj);
    return *this;
}

/**
 * @brief  ���֥������Ȥν����
 *
 * @attention  ����Ĺ����ξ�硤���󵭽һҤ��Ф������Ȥʤ�ޤ���
 */
fits_table_col &fits_table_col::init()
{
    if ( this->manager != NULL ) {
	//const char *none[] = {NULL};
	//fits::table_def def = { NULL,"",none,none,
	//			"","","","1A", "","","","" };
	fits::table_def def = { NULL,"","","",
				"","","","1A", "","","","" };
	/* manager ��ͳ�� _define ��Ƥ� */
	this->define(def);
	/* ���줤���äѤ�ˤ��� */
	this->clean(0, this->length());
	/* svalue() �� NULL �ͤ�ꥻ�å� */
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
 * @brief  ���֥������ȤΥ��ԡ�
 *
 * @attention  ����Ĺ����ξ�硤���󵭽һҤ��Ф������Ȥʤ�ޤ���
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
	/* �����̾��TALAS �ʤɤν����� manager �ˤ����� */
	/* manager ��ͳ�� _define() ��Ƥ� */
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

    if ( this->manager != NULL ) {	/* fits_table �δ������ξ�� */
	/* ���Υ����礭�����᤹ */
	this->_resize(org_nrows);
	/* ����Ū�� _resize() ��Ƥ�(������ȥơ��֥���������ꥵ����) */
	//this->manager->resize_rows(obj.row_size_rec);
    }

    return *this;
}

/* swap internal data of two objects */
/**
 * @brief  2�ĤΥ��֥������ȴ֤Ǥ����ƤΥ���å�
 *
 *  ���ꤵ�줿���֥�������obj �����Ƥȼ��Ȥ����Ƥ������ؤ��ޤ���
 *  �ǡ������󡦥إå������ơ�°�������٤Ƥξ��֤������ؤ��ޤ���
 *
 * @param   obj fits_table_col ���饹�Υ��֥�������
 * @return  ���Ȥλ���    
 * @attention  ����Ĺ����ξ�硤���󵭽һҤ��Ф������Ȥʤ�ޤ���
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

    /* �ޤ� manager ¦�򹹿����� */
    if ( this->manager != NULL ) {
	def.ttype = ttype2.cstr();
	//def.talas = talas2.cstrarray();
	def.talas = talas2.cstr();
	/* manager ��ͳ�� _define ��Ƥ� */
	this->define(def);
    }
    if ( obj.manager != NULL ) {
	def.ttype = ttype1.cstr();
	//def.talas = talas1.cstrarray();
	def.talas = talas1.cstr();
	/* manager ��ͳ�� _define ��Ƥ� */
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
 * @brief  �ݸ�°����̵ͭ���֤�
 *
 * @note   �桼���γ�ĥ���饹�ǻ��Ѥ����ꡥ
 */
bool fits_table_col::is_protected() const
{
    return this->protected_rec;
}

/* to obtain all definition of a column */
/**
 * @brief  ����������� fits::table_def ��¤�ΤǼ���
 */
const fits::table_def &fits_table_col::definition() const
{
    return this->def_rec;
}

/**
 * @brief  ����������� fits::table_def_all ��¤�ΤǼ���
 */
const fits::table_def_all &fits_table_col::definition_all() const
{
    return this->def_all_rec;
}

/* get/set column name */
/**
 * @brief  �����̾�����
 */
const char *fits_table_col::name() const
{
    return this->tany.cstr(TTYPE_IDX);
}

/**
 * @brief  �����̾������
 */
fits_table_col &fits_table_col::assign_name( const char *new_name )
{
    fits::table_def_all def = {NULL};
    def.ttype = new_name;
    this->define(def);
    return *this;
}

/**
 * @brief  ����Ĺ����ޤ��ϲ���Ĺ��������ǿ������
 *
 *  ����Ĺ����ξ�硤�������ͤ˴ط��ʤ� fits_table_col::elem_length() ���֤���
 *  ��Ʊ���ͤ��֤���ޤ���<br>
 *  elem_idx �ϡ�����Ĺ��������󵭽һҤ�ʣ��¸�ߤ�����˻��ꤷ�ޤ���
 *  �㤨�� TFORM ������� '4PE(999)' �ξ�� elem_idx �� 0 �� 3 ���ͤ�Ȥ����
 *  �Ǥ��ޤ���
 *
 * @param   row_idx �ԥ���ǥå���
 * @param   elem_idx ����Ĺ��������󵭽һҤ������ֹ� (��ά����0)
 * @return  ��������ǿ�
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
 * @brief  ����Ĺ����Υҡ��ץ��ե��åȤ����
 *
 *  elem_idx �ϡ�����Ĺ��������󵭽һҤ�ʣ��¸�ߤ�����˻��ꤷ�ޤ���
 *  �㤨�� TFORM ������� '4PE(999)' �ξ�� elem_idx �� 0 �� 3 ���ͤ�Ȥ����
 *  �Ǥ��ޤ���
 *
 * @param   row_idx �ԥ���ǥå���
 * @param   elem_idx ����Ĺ��������󵭽һҤ������ֹ� (��ά����0)
 * @return  �ҡ��ץ��ե��å� (�Х���ñ��)
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
 * @brief  NULLʸ���������
 *
 *  fits_table_col::svalue()��fits_table_col::assign( const char *, ... ) ��
 *  ���Ѥ��� NULL ʸ�����ͤ����ꤷ�ޤ���<br>
 *  NULLʸ����ν�������ͤ� "NULL" �Ǥ���
 *
 * @param   snull NULLʸ����
 * @return  ���Ȥλ���
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
 * @brief  NULLʸ����μ���
 *
 *  fits_table_col::svalue()��fits_table_col::assign( const char *, ... ) ��
 *  ���Ѥ��� NULL ʸ�����ͤ�������ޤ���
 *
 * @return  NULLʸ����
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
 * @brief  ����Υǥե�����ͤ� double ��������
 *
 *  fits_table::resize_rows() ���ǥơ��֥�ιԤ�Ĺ�����礭���������ο�������
 *  ���ͤ����ꤷ�ޤ������Υ��дؿ��Ǥϥإå��� TZEROn��TSCALEn��TNULLn ���ͤ�
 *  ȿ�Ǥ���ޤ���<br>
 *  �ǥե�����ͤν�������ͤϡ��������ξ��� 'F'��ʸ���󷿤ξ��� ' ' ����
 *  �ʤ�ʸ���󡤤���¾�η��ξ��� 0 �Ǥ���<br>
 *  �ǥե�����ͤȤ��� NULL�ͤ򥻥åȤ��������� NAN �򥻥åȤ��ޤ���
 *
 * @param   value �����
 * @return  ���Ȥλ���
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
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
 * @brief  ����Υǥե�����ͤ� float ��������
 *
 *  fits_table::resize_rows() ���ǥơ��֥�ιԤ�Ĺ�����礭���������ο�������
 *  ���ͤ����ꤷ�ޤ������Υ��дؿ��Ǥϥإå��� TZEROn��TSCALEn��TNULLn ���ͤ�
 *  ȿ�Ǥ���ޤ���<br>
 *  �ǥե�����ͤν�������ͤϡ��������ξ��� 'F'��ʸ���󷿤ξ��� ' ' ����
 *  �ʤ�ʸ���󡤤���¾�η��ξ��� 0 �Ǥ���<br>
 *  �ǥե�����ͤȤ��� NULL�ͤ򥻥åȤ��������� NAN �򥻥åȤ��ޤ���
 *
 * @param   value �����
 * @return  ���Ȥλ���
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
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
 * @brief  ����Υǥե�����ͤ� long long ��������
 *
 *  fits_table::resize_rows() ���ǥơ��֥�ιԤ�Ĺ�����礭���������ο�������
 *  ���ͤ����ꤷ�ޤ������Υ��дؿ��Ǥϥإå��� TZEROn��TSCALEn��TNULLn ���ͤ�
 *  ȿ�Ǥ���ޤ���<br>
 *  �ǥե�����ͤν�������ͤϡ��������ξ��� 'F'��ʸ���󷿤ξ��� ' ' ����
 *  �ʤ�ʸ���󡤤���¾�η��ξ��� 0 �Ǥ���<br>
 *  �ǥե�����ͤȤ��� NULL�ͤ򥻥åȤ��������� 
 *  fits_table_col::assign_default(double) �� NAN �򥻥åȤ��ޤ���
 *
 * @param   value �����
 * @return  ���Ȥλ���
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
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
 * @brief  ����Υǥե�����ͤ� long ��������
 *
 *  fits_table::resize_rows() ���ǥơ��֥�ιԤ�Ĺ�����礭���������ο�������
 *  ���ͤ����ꤷ�ޤ������Υ��дؿ��Ǥϥإå��� TZEROn��TSCALEn��TNULLn ���ͤ�
 *  ȿ�Ǥ���ޤ���<br>
 *  �ǥե�����ͤν�������ͤϡ��������ξ��� 'F'��ʸ���󷿤ξ��� ' ' ����
 *  �ʤ�ʸ���󡤤���¾�η��ξ��� 0 �Ǥ���<br>
 *  �ǥե�����ͤȤ��� NULL�ͤ򥻥åȤ��������� 
 *  fits_table_col::assign_default(double) �� NAN �򥻥åȤ��ޤ���
 *
 * @param   value �����
 * @return  ���Ȥλ���
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
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
 * @brief  ����Υǥե�����ͤ� int ��������
 *
 *  fits_table::resize_rows() ���ǥơ��֥�ιԤ�Ĺ�����礭���������ο�������
 *  ���ͤ����ꤷ�ޤ������Υ��дؿ��Ǥϥإå��� TZEROn��TSCALEn��TNULLn ���ͤ�
 *  ȿ�Ǥ���ޤ���<br>
 *  �ǥե�����ͤν�������ͤϡ��������ξ��� 'F'��ʸ���󷿤ξ��� ' ' ����
 *  �ʤ�ʸ���󡤤���¾�η��ξ��� 0 �Ǥ���<br>
 *  �ǥե�����ͤȤ��� NULL�ͤ򥻥åȤ��������� 
 *  fits_table_col::assign_default(double) �� NAN �򥻥åȤ��ޤ���
 *
 * @param   value �����
 * @return  ���Ȥλ���
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
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
 * @brief  ����Υǥե�����ͤ� char * ��������
 *
 *  fits_table::resize_rows() ���ǥơ��֥�ιԤ�Ĺ�����礭���������ο�������
 *  ���ͤ����ꤷ�ޤ������Υ��дؿ��Ǥϥإå��� TZEROn��TSCALEn��TNULLn ���ͤ�
 *  ȿ�Ǥ���ޤ���<br>
 *  �ǥե�����ͤν�������ͤϡ��������ξ��� 'F'��ʸ���󷿤ξ��� ' ' ����
 *  �ʤ�ʸ���󡤤���¾�η��ξ��� 0 �Ǥ���<br>
 *  �ǥե�����ͤȤ��� NULL�ͤ򥻥åȤ��������� 
 *  fits_table_col::assign_default(double) �� NAN �򥻥åȤ��ޤ���
 *
 * @param   value �����
 * @return  ���Ȥλ���
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
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
 * @brief  ����Υǥե�����ͤ����� (���٥�)
 *
 *  fits_table::resize_rows() ���ǥơ��֥�ιԤ�Ĺ�����礭���������ο�������
 *  ���ͤ����ꤷ�ޤ������٥�ʥ��дؿ��Ǥ��Τǡ��إå��� TZEROn��TSCALEn��
 *  TNULLn ���ͤϹ�θ����ޤ���<br>
 *  �ǥե�����ͤν�������ͤϡ��������ξ��� 'F'��ʸ���󷿤ξ��� ' ' ����
 *  �ʤ�ʸ���󡤤���¾�η��ξ��� 0 �Ǥ���
 *
 * @param   value ����ͤΥ��ɥ쥹
 * @note    value ���ؤ����ƤΥХ��ȿ��ϡ������η��ΥХ��ȿ������ʤ��
 *          ��fits_table_col::bytes() ���֤��͡פ˰��פ��Ƥ���ɬ�פ�����ޤ���
 * @return  ���Ȥλ���
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
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
 * @deprecated  ����μ����Τ���Υ��дؿ������Ѥ��ʤ��Ǥ���������
 */
void *fits_table_col::heap_ptr()
{
    if ( this->manager != NULL ) return NULL;		      /* not enabled */

    return this->heap_ptr_rec;
}

#ifdef SLI__OVERLOAD_CONST_AT
/**
 * @deprecated  ����μ����Τ���Υ��дؿ������Ѥ��ʤ��Ǥ���������
 */
const void *fits_table_col::heap_ptr() const
{
    if ( this->manager != NULL ) return NULL;		      /* not enabled */

    return this->heap_ptr_rec;
}
#endif

/**
 * @deprecated  ����μ����Τ���Υ��дؿ������Ѥ��ʤ��Ǥ���������
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
 * @brief  �ǡ��������Хåե��إ��ԡ�
 *
 * @param   dest_buf ���ԡ���Хåե��Υ��ɥ쥹
 * @param   buf_size dest_buf �ΥХ��ȥ�����
 * @return  �������: �Хåե�Ĺ����ʬ�ʾ��˥��ԡ��Ǥ���Х��ȿ� <br>
 *          �����(���顼): �����������ǥ��ԡ�����ʤ��ä����
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
 */
ssize_t fits_table_col::get_data( void *dest_buf, size_t buf_size ) const
{
    return this->get_data(0,dest_buf,buf_size);
}

/**
 * @brief  �ǡ��������Хåե��إ��ԡ�
 *
 * @param   row_index  ���ԡ���(�����Хåե�)�ιԥ���ǥå���
 * @param   dest_buf ���ԡ���Хåե��Υ��ɥ쥹
 * @param   buf_size dest_buf �ΥХ��ȥ�����
 * @return  �������: �Хåե�Ĺ����ʬ�ʾ��˥��ԡ��Ǥ���Х��ȿ� <br>
 *          �����(���顼): �����������ǥ��ԡ�����ʤ��ä����
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
 */
ssize_t fits_table_col::get_data( long row_index, 
				  void *dest_buf, size_t buf_size ) const
{
    if ( this->full_bytes_rec == 0 ) return -1;		/* not defined */

    return this->data_rec.getdata(dest_buf, buf_size, 0, row_index);
}

/**
 * @brief  �����Хåե��Υǡ����������
 *
 * @param   src_buf ���ԡ����Хåե��Υ��ɥ쥹
 * @param   buf_size src_buf �ΥХ��ȥ�����
 * @return  �������: src_buf�ΥХåե�Ĺ����ʬ�ʾ��˥��ԡ��Ǥ���Х��ȿ�<br>
 *          �����: ���������ǥ��ԡ�����ʤ��ä����
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
 */
ssize_t fits_table_col::put_data( const void *src_buf, size_t buf_size )
{
    return this->put_data(0,src_buf,buf_size);
}

/**
 * @brief  �����Хåե��Υǡ����������
 *
 * @param   row_index ���ԡ���(�����Хåե�)�ιԥ���ǥå���
 * @param   src_buf ���ԡ����Хåե��Υ��ɥ쥹
 * @param   buf_size src_buf �ΥХ��ȥ�����
 * @return  �������: src_buf�ΥХåե�Ĺ����ʬ�ʾ��˥��ԡ��Ǥ���Х��ȿ�<br>
 *          �����: ���������ǥ��ԡ�����ʤ��ä����
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
 */
ssize_t fits_table_col::put_data( long row_index, 
				  const void *src_buf, size_t buf_size )
{
    if ( this->full_bytes_rec == 0 ) return -1;		/* not defined */

    return this->data_rec.putdata(src_buf, buf_size, 0, row_index);
}

/**
 * @brief  �����ǡ�����������Ƥ����������֥������Ȥλ��Ȥ��֤� (�ɼ�����)
 */
const mdarray &fits_table_col::data_array_cs() const
{
    return this->data_rec;
}

/**
 * @deprecated  ����μ����Τ���Υ��дؿ������Ѥ��ʤ��Ǥ���������
 */
ssize_t fits_table_col::get_heap( void *dest_buf, size_t buf_size ) const
{
    if ( this->manager != NULL ) return -1;		      /* not enabled */

    return this->get_heap(0,dest_buf,buf_size);
}

/**
 * @deprecated  ����μ����Τ���Υ��дؿ������Ѥ��ʤ��Ǥ���������
 */
ssize_t fits_table_col::get_heap( long offset, 
				  void *dest_buf, size_t buf_size ) const
{
    if ( this->manager != NULL ) return -1;		      /* not enabled */

    return this->heap_rec.getdata(dest_buf, buf_size, offset);
}

/**
 * @deprecated  ����μ����Τ���Υ��дؿ������Ѥ��ʤ��Ǥ���������
 */
ssize_t fits_table_col::put_heap( const void *buf, size_t buf_size )
{
    if ( this->manager != NULL ) return -1;		      /* not enabled */

    return this->put_heap(0,buf,buf_size);
}

/**
 * @deprecated  ����μ����Τ���Υ��дؿ������Ѥ��ʤ��Ǥ���������
 */
ssize_t fits_table_col::put_heap( long offset, 
				  const void *buf, size_t buf_size )
{
    if ( this->manager != NULL ) return -1;		      /* not enabled */

    return this->heap_rec.putdata(buf, buf_size, offset);
}

/**
 * @deprecated  ����μ����Τ���Υ��дؿ������Ѥ��ʤ��Ǥ���������
 */
const mdarray &fits_table_col::heap_array_cs() const
{
    return this->heap_rec;
}

/**
 * @deprecated  ����μ����Τ���Υ��дؿ������Ѥ��ʤ��Ǥ���������
 */
fits_table_col &fits_table_col::resize_heap( size_t sz )
{
    this->heap_rec.resize(sz);
    return *this;
}

/**
 * @deprecated  ����μ����Τ���Υ��дؿ������Ѥ��ʤ��Ǥ���������
 */
size_t fits_table_col::heap_length()
{
    return this->heap_rec.length();
}

/**
 * @brief  TFORMn��TDIMn ��ѡ������������γƼ������֤� (������)
 *
 * @note    private �ʴؿ��Ǥ���
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
	/* �Ȥꤢ������ʸ����ȥӥåȷ��ϥ��ݡ��Ȥ������Ȥ������� */
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
		if ( 0 < dx ) {			/* �̾�Ϥ��� */
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
	else if ( 0 < tdim.length() ) {		/* TDIM�ˤ��ʸ����Ĺ�λ��� */
	    d_pos = tdim.strpbrk("[0-9]");
	    if ( 0 <= d_pos ) {
		dx = tdim.strtol(d_pos,10,&d_endpos);
		if ( d0 < dx ) {
		    status = 1;			/* for WARNING */
		}
		else {
		    if ( 0 < dx ) {		/* �̾�Ϥ��� */
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
	    if ( 0 < dx ) {			/* �̾�Ϥ��� */
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
	/* �������������å����� */
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
    else {	/* STRING �ǤϤʤ���� */
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
		if ( 0 < dx ) {			/* �̾�Ϥ��� */
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
	    /* �������������å����� */
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

/* fits_table ����ϡ����Υ��дؿ���Ȥ�ʤ����ȡ�*/
/* change the definition of a column */
/**
 * @brief  �������������� (fits::table_def ��¤�Τ����)
 *
 *  ���Υ��дؿ��ϡ��������������������ȡ������������ѹ����ͤƤ���
 *  ����def.xxx == NULL �ξ��ϡ����ι��ܤ��ѹ����ޤ���
 *
 * @param   def ��������
 * @return  ���Ȥλ���
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

/* fits_table ����ϡ����Υ��дؿ���Ȥ�ʤ����ȡ�*/
/**
 * @brief  �������������� (fits::table_def_all ��¤�Τ����)
 *
 *  ���Υ��дؿ��ϡ��������������������ȡ������������ѹ����ͤƤ���
 *  ����def.xxx == NULL �ξ��ϡ����ι��ܤ��ѹ����ޤ���
 *
 * @param   def ��������
 * @return  ���Ȥλ���
 */
fits_table_col &fits_table_col::define( const fits::table_def_all &def )
{
    if ( this->manager == NULL ) {	/* fits_table �δ������Ǥʤ���� */
	return this->_define(def);
    }
    else {				/* fits_table �δ������ξ�� */
	long i;
	for ( i=0 ; i < this->manager->col_length() ; i++ ) {
	    if ( &(this->manager->col(i)) == this ) {
		/* ����Ū�� _define() ��Ƥ� */
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
 * @brief  �������������� (���٥�)
 *
 *  ���Υ��дؿ��ϡ��������������������ȡ������������ѹ����ͤƤ���
 *  ����def.xxx == NULL �ξ��ϡ����ι��ܤ��ѹ����ޤ���
 *
 * @param   def ��������
 * @return  ���Ȥλ���
 * @note  fits_table ����ϡ����Υ��дؿ���Ȥ����ȡ�<br>
 *        ���Υ��дؿ��� protected �Ǥ���
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
		/* heap ��Ȥ���� */
		if ( new_type == FITS::LONGARRDESC_T || 
		     new_type == FITS::LLONGARRDESC_T ) {
		    const size_t nx[] = {new_elem_size, this->row_size_rec};
		    this->data_rec.init(new_sz_bytes, false, nx, 2, true);
		    this->heap_rec.init(new_heap_sz_bytes, false);
		}
		/* BIT_T �ξ������� */
		else if ( new_type == FITS::BIT_T ) {
		    const size_t nx[] = {new_full_bytes, this->row_size_rec};
		    this->data_rec.init(1, false, nx, 2, true);
		    this->heap_rec.init(1, false);
		}
		/* ����¾�ξ��(STRING_T��ޤ�) */
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
	    else 		/* "NULL" ɽ���Τ��� */
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

	/* �̥��ͤΥꥻ�åȤ�˺��Ƥ����Τ��ɲ� 2011/12/2 */
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

	/* �̥��ͤΥꥻ�åȤ�˺��Ƥ����Τ��ɲ� 2011/12/2 */
	if ( do_tnull_reset == true ) this->set_tnull(NULL);	/* �ꥻ�å� */

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
	    /* this->telem ��ʤ�Ȥ����� */
	    for ( i=0,j=0 ; i < tmp_arr.length() /* def.telem[i] != NULL */ ; i++ ) {
		ssize_t c_pos;
	        //tmp_str.assign(def.telem[i]).strtrim();
	        tmp_str.assign(tmp_arr[i]).trim();
		/* �ӥåȥե�����Ȥ򰷤� */
		c_pos = tmp_str.strrchr(':');
		if ( 0 <= c_pos ) {
		    ssize_t k, n_bit = tmp_str.atoi(c_pos+1);
		    if ( 0 < n_bit ) {
			/* ':' ��걦¦��õ� */
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
	    /* this->telem_def ��ʤ�Ȥ����� */
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
	    /* ����ǥå�����ĥ�� */
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
	/* �ӥåȤο�������� */
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
	/* �ƥǡ������Ǥɤ� TDISP �Υ����פ���Ĥ��뤫������ */
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
			/* ���� */
			this->fmt_str.printf("%%%ds",-len0);
			this->fmt_nullstr.printf("%%%ds",-len0);
		    }
		    else {	/* for 'L' tdisp  */
			/* ���� */
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
		    else	/* "NULL" ɽ���Τ��� */
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
		/* 'D' �ΰ����Ϥ����������? */
		/* (fv ��ư��Ǥ� %f ������FORTRANŪ�ˤ� %E �Ǹ�� D ���ѹ�) */
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
		/* 'F' �ξ�� */
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
     * TNULL �ν����Ϻ��𤷤䤹���Τ����
     *  def.tnull = NULL   -> ���⤷�ʤ�
     *  def.tnull = ""     -> �����õ�
     *  def.tnull = " "    -> this->bytes() �Ĥζ���� NULL �ͤȤ���
     */
    if ( def.tnull != NULL ) {
	if ( this->type_rec != FITS::STRING_T ) {
	    tmp_str.assign(def.tnull).strtrim();
	    if ( 0 < tmp_str.length() ) {
		long long tnl = tmp_str.atoll();
		this->set_tnull(&tnl);			/* ���å� */
	    }
	    else this->set_tnull(NULL);			/* �ꥻ�å� */
	}
	else {
	    tmp_str.assign(def.tnull);
	    if ( 0 < tmp_str.length() ) {
		/* Changed 2011/12/1 */
		//tmp_str.resize(this->full_bytes_rec);
		if ( (long)(tmp_str.length()) <= this->bytes() ) {
		    tmp_str.resize(this->bytes());
		}
		/* NULL �ͤ�Ĺ�����礭�������硤*/
		/* �����Ƿٹ��Ф��Ƥ��ɤ����������������ۤäƤ��� */
	    }
	    /* ����������� NULL �����ꥻ�åȤ��Ƥ��� */
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

    /* �̥�ݤϤʤ��褦�ˤ��� */
    this->tany.resize(TANY_ARR_SIZE);

    this->update_def_rec_ptr();

 quit:
    return *this;
}

/*
 * Functions to convert numeric columns
 */

/**
 * @brief  �ǡ��������Ѵ����ѹ�
 *
 *  ����Ĺ����ǤϤʤ����������뤤�ϼ¿����Υ����(TFORMn �λ��꤬ 'B'��'I'��
 *  'J'��'K'��'E'��'D'��ޤ���)�η��� new_type ���ѹ����ޤ���
 *  ɬ�פ˱����ơ������Хåե��Υ��������ѹ����ޤ���<br>
 *  new_type �˻���Ǥ����ͤϡ�FITS::DOUBLE_T��FITS::FLOAT_T��
 *  FITS::LONGLONG_T��FITS::LONG_T��FITS::SHORT_T��FITS::BYTE_T �Τ����줫
 *  �Ǥ���<br>
 *  new_zero��new_scale��new_null �����ꤵ�줿���ϡ��إå��� TZEROn��
 *  TSCALn��TNULLn ���ѹ������������ͤ�ȿ�Ǥ����ǡ������Ѵ����ޤ���<br>
 *  new_null ��ͭ���ʤΤϡ�new_type ���������ξ��ΤߤǤ���<br>
 *  ʸ���󷿤��������Υ����Ϥ��Υ��дؿ��Ǥ��Ѵ��Ǥ��ޤ���
 *
 * @param  new_type �Ѵ���Υǡ�����
 * @return  ���Ȥλ���
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
 */
fits_table_col &fits_table_col::convert_type( int new_type )
{
    return this->convert_col_type( new_type, NULL, NULL, NULL );
}

/**
 * @brief  �ǡ��������Ѵ����ѹ�
 *
 *  ����Ĺ����ǤϤʤ����������뤤�ϼ¿����Υ����(TFORMn �λ��꤬ 'B'��'I'��
 *  'J'��'K'��'E'��'D'��ޤ���)�η��� new_type ���ѹ����ޤ���
 *  ɬ�פ˱����ơ������Хåե��Υ��������ѹ����ޤ���<br>
 *  new_type �˻���Ǥ����ͤϡ�FITS::DOUBLE_T��FITS::FLOAT_T��
 *  FITS::LONGLONG_T��FITS::LONG_T��FITS::SHORT_T��FITS::BYTE_T �Τ����줫
 *  �Ǥ���<br>
 *  �إå��� TZEROn�����ѹ������������ͤ�ȿ�Ǥ����ǡ������Ѵ����ޤ���<br>
 *  ʸ���󷿤��������Υ����Ϥ��Υ��дؿ��Ǥ��Ѵ��Ǥ��ޤ���
 *
 * @param  new_type �Ѵ���Υǡ�����
 * @param  new_zero �Ѵ����TZEROn��
 * @return  ���Ȥλ���
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
 */
fits_table_col &fits_table_col::convert_type( int new_type, double new_zero )
{
    return this->convert_col_type( new_type, &new_zero, NULL, NULL );
}

/**
 * @brief  �ǡ��������Ѵ����ѹ�
 *
 *  ����Ĺ����ǤϤʤ����������뤤�ϼ¿����Υ����(TFORMn �λ��꤬ 'B'��'I'��
 *  'J'��'K'��'E'��'D'��ޤ���)�η��� new_type ���ѹ����ޤ���
 *  ɬ�פ˱����ơ������Хåե��Υ��������ѹ����ޤ���<br>
 *  new_type �˻���Ǥ����ͤϡ�FITS::DOUBLE_T��FITS::FLOAT_T��
 *  FITS::LONGLONG_T��FITS::LONG_T��FITS::SHORT_T��FITS::BYTE_T �Τ����줫
 *  �Ǥ���<br>
 *  �إå��� TZEROn��TSCALn�����ѹ������������ͤ�ȿ�Ǥ����ǡ������Ѵ�����
 *  ����<br>
 *  ʸ���󷿤��������Υ����Ϥ��Υ��дؿ��Ǥ��Ѵ��Ǥ��ޤ���
 *
 * @param  new_type �Ѵ���Υǡ�����
 * @param  new_zero �Ѵ����TZEROn��
 * @param  new_scale �Ѵ����TSCALn��
 * @return  ���Ȥλ���
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
 */
fits_table_col &fits_table_col::convert_type( int new_type, double new_zero,
					      double new_scale )
{
    return this->convert_col_type( new_type, &new_zero, &new_scale, NULL );
}

/**
 * @brief  �ǡ��������Ѵ����ѹ�
 *
 *  ����Ĺ����ǤϤʤ����������뤤�ϼ¿����Υ����(TFORMn �λ��꤬ 'B'��'I'��
 *  'J'��'K'��'E'��'D'��ޤ���)�η��� new_type ���ѹ����ޤ���
 *  ɬ�פ˱����ơ������Хåե��Υ��������ѹ����ޤ���<br>
 *  new_type �˻���Ǥ����ͤϡ�FITS::DOUBLE_T��FITS::FLOAT_T��
 *  FITS::LONGLONG_T��FITS::LONG_T��FITS::SHORT_T��FITS::BYTE_T �Τ����줫
 *  �Ǥ���<br>
 *  �إå��� TZEROn��TSCALn��TNULLn ���ѹ������������ͤ�ȿ�Ǥ����ǡ�����
 *  �Ѵ����ޤ���<br>
 *  new_null ��ͭ���ʤΤϡ�new_type ���������ξ��ΤߤǤ���<br>
 *  ʸ���󷿤��������Υ����Ϥ��Υ��дؿ��Ǥ��Ѵ��Ǥ��ޤ���
 *
 * @param  new_type �Ѵ���Υǡ�����
 * @param  new_zero �Ѵ����TZEROn��
 * @param  new_scale �Ѵ����TSCALn��
 * @param  new_null �Ѵ����TNULLn�� 
 * @return  ���Ȥλ���
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
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
 * @brief  ʪ��ñ��(TUNITn ����)������
 */
fits_table_col &fits_table_col::assign_tunit( const char *unit )
{
    fits::table_def_all def = {NULL};
    def.tunit = unit;
    return this->_define(def);
}

/**
 * @brief  ʪ��ñ��(TUNITn)������ξõ�
 *
 * @return  ���Ȥλ���
 */
fits_table_col &fits_table_col::erase_tunit()
{
    fits::table_def_all def = {NULL};
    def.tunit = "";
    return this->_define(def);
}

/**
 * @brief  �������Υ������Ф��� NULL��(TNULLn ����)�� long long ��������
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
 * @brief  �������Υ������Ф��� NULL��(TNULLn ����)��ʸ���������
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
 * @brief  �������Υ������Ф��� NULL��(TNULLn)������ξõ�
 *
 * @return  ���Ȥλ���
 */
fits_table_col &fits_table_col::erase_tnull()
{
    fits::table_def_all def = {NULL};
    def.tnull = "";
    return this->_define(def);
}

/**
 * @brief  ������(TZEROn ����)������
 *
 * @param   zero TZEROn ����
 * @param   prec ����(���)����ά����15��
 * @return  ���Ȥλ���
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
 * @brief  ������(TZEROn)������ξõ�
 *
 * @return  ���Ȥλ���
 */
fits_table_col &fits_table_col::erase_tzero()
{
    fits::table_def_all def = {NULL};
    def.tzero = "";
    return this->_define(def);
}

/**
 * @brief  ��������󥰥ե�������(TSCALn ����)������
 *
 * @param   scal TSCALn ����
 * @param   prec ����(���)����ά����15��
 * @return  ���Ȥλ���
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
 * @brief  ��������󥰥ե�������(TSCALn)������ξõ�
 *
 * @return  ���Ȥλ���
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

/* fits_table_col, fits_table ����ϡ����Υ��дؿ���Ȥ�ʤ����� */
/**
 * @brief  �Կ����ѹ�
 *
 *  ����������ˤϡ��ǥե�����ͤ����åȤ���ޤ����ǥե�����ͤν�������ͤϡ�
 *  �������ξ��� 'F'��ʸ���󷿤ξ��� ' ' ����ʤ�ʸ���󡤤���¾�η��ξ���
 *  0 �Ǥ����ǥե�����ͤ� fits_table_col::assign_default() ���дؿ����ѹ���
 *  ���ޤ���<br>
 *  ���Ȥ� fits_table ���֥������Ȥδ������ˤ����硤¾�Υ�����Ʊ����
 *  �������¹Ԥ���ޤ���
 *
 * @param   num_rows �ѹ���ιԿ�
 * @return  ���Ȥλ���
 * @attention  ����Ĺ����ξ�硤���󵭽һҤ��Ф������Ȥʤ�ޤ���
 */
fits_table_col &fits_table_col::resize( long num_rows )
{
    if ( this->manager == NULL ) {	/* fits_table �δ������Ǥʤ���� */
	return this->_resize(num_rows);
    }
    else {				/* fits_table �δ������ξ�� */
	/* ����Ū�� _resize() ��Ƥ� */
	this->manager->resize_rows(num_rows);
	return *this;
    }
}

/**
 * @brief  �Կ����ѹ� (���٥�)
 *
 * @param   num_rows �ѹ���ιԿ�
 * @return  ���Ȥλ���
 * @note    fits_table ����ϡ����Υ��дؿ���Ȥ�����<br>
 *          ���Υ��дؿ��� protected �Ǥ���
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
 * @brief  �Ԥ���ԤؤΥ��ԡ�
 *
 * @param  src_index ���ԡ����򼨤��ԥ���ǥå���
 * @param  num_rows ���ԡ���Ԥ��Կ�
 * @param  dest_index ���ԡ���򼨤��ԥ���ǥå���
 * @return  ���Ȥλ���
 * @attention  ����Ĺ����ξ�硤���󵭽һҤ��Ф������Ȥʤ�ޤ���
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
 * @brief  �ԤȹԤȤ������ؤ�
 *
 *  index0 �ǻ��ꤵ�줿�Ԥ��� num_rows �ĤιԤ�index1 �ǻ��ꤵ�줿�Ԥ���
 *  num_cols �ĤιԤ������ؤ��ޤ���<br>
 *  num_cols �ˤ�äơ��Ťʤ�Ԥ�������ϡ�num_cols ���ͤ򸺤餷�������ؤ���
 *  �Ԥʤ��ޤ���
 *
 * @param  index0 �����ؤ����򼨤��ԥ���ǥå���
 * @param  num_rows �����ؤ���Ԥ��Կ�
 * @param  index1 �����ؤ���򼨤��ԥ���ǥå���
 * @return  ���Ȥλ���
 * @attention  ����Ĺ����ξ�硤���󵭽һҤ��Ф������Ȥʤ�ޤ���
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
 * @brief  �Ԥν����ȿž
 *
 * @param   index ȿž���ϰ��֤򼨤��ԥ���ǥå���
 * @param   num_rows ȿž�����Կ�
 * @return  ���Ȥλ���
 * @attention  ����Ĺ����ξ�硤���󵭽һҤ��Ф������Ȥʤ�ޤ���
 */
fits_table_col &fits_table_col::flip( long index, long num_rows )
{
    if ( this->full_bytes_rec == 0 ) return *this;	/* not defined */
    if ( num_rows < 0 ) return *this;			/* invalid */

    this->data_rec.flip(1, index, num_rows);

    return *this;
}

/* fits_table_col, fits_table ����ϡ����Υ��дؿ���Ȥ�ʤ����� */
/**
 * @brief  �Ԥξõ�
 *
 *  ���Ȥ� fits_table ���֥������Ȥδ������ˤ����硤¾�Υ�����Ʊ����
 *  �������¹Ԥ���ޤ���
 *
 * @param   index ������ϰ��֤򼨤��ԥ���ǥå���
 * @param   num_rows ��������Կ�
 * @return  ���Ȥλ���
 * @attention  �ơ��֥�β������ΥХ��ȿ��ȹԿ����礭�������� index ��������
 *             �ۤɡ�����Υ��ԡ������μ¹ԥ����Ȥ����礷�ޤ���
 * @attention  ����Ĺ����ξ�硤���󵭽һҤ��Ф������Ȥʤ�ޤ���
 */
fits_table_col &fits_table_col::erase( long index, long num_rows )
{
    if ( this->manager == NULL ) {	/* fits_table �δ������Ǥʤ���� */
	return this->_erase(index, num_rows);
    }
    else {				/* fits_table �δ������ξ�� */
	/* ����Ū�� _erase() ��Ƥ� */
	this->manager->erase_rows(index, num_rows);
	return *this;
    }
}

/**
 * @brief  �Ԥξõ� (���٥�)
 *
 * @param   index ������ϰ��֤򼨤��ԥ���ǥå���
 * @param   num_rows ��������Կ�
 * @return  ���Ȥλ���
 * @note    fits_table ����ϡ����Υ��дؿ���Ȥ�����
 *          ���Υ��дؿ��� protected �Ǥ���
 */
fits_table_col &fits_table_col::_erase( long index, long num_rows )
{
    if ( this->full_bytes_rec == 0 ) return *this;	/* not defined */
    if ( num_rows < 0 ) return *this;			/* invalid */

    this->data_rec.erase(1, index, num_rows);
    this->row_size_rec = this->data_rec.row_length();

    return *this;
}

/* fits_table_col, fits_table ����ϡ����Υ��дؿ���Ȥ�ʤ����� */
/**
 * @brief  �������Ԥ�����
 *
 *  ����������ˤϡ��ǥե�����ͤ����åȤ���ޤ����ǥե�����ͤν�������ͤϡ�
 *  �������ξ��� 'F'��ʸ���󷿤ξ��� ' ' ����ʤ�ʸ���󡤤���¾�η��ξ���
 *  0 �Ǥ����ǥե�����ͤ� fits_table_col::assign_default() ���дؿ����ѹ���
 *  ���ޤ���<br>
 *  ���Ȥ� fits_table ���֥������Ȥδ������ˤ����硤¾�Υ�����Ʊ����
 *  �������¹Ԥ���ޤ���
 *
 * @param   index �������֤򼨤��ԥ���ǥå���
 * @param   num_rows ���������Կ�
 * @return  ���Ȥλ���
 * @attention  �ơ��֥�β������ΥХ��ȿ��ȹԿ����礭�������� index ��������
 *             �ۤɡ�����Υ��ԡ������μ¹ԥ����Ȥ����礷�ޤ���
 * @attention  ����Ĺ����ξ�硤���󵭽һҤ��Ф������Ȥʤ�ޤ���
 */
fits_table_col &fits_table_col::insert( long index, long num_rows )
{
    if ( this->manager == NULL ) {	/* fits_table �δ������Ǥʤ���� */
	return this->_insert(index,num_rows);
    }
    else {				/* fits_table �δ������ξ�� */
	/* ����Ū�� _insert() ��Ƥ� */
	this->manager->insert_rows(index,num_rows);
	return *this;
    }
}

/**
 * @brief  �������Ԥ����� (���٥�)
 *
 * @return  ���Ȥλ���
 * @note    fits_table ����ϡ����Υ��дؿ���Ȥ�����
 *          ���Υ��дؿ��� protected �Ǥ���
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
 * @brief  ����μ����Τ���Υ��дؿ�
 *
 * @note   ���Υ��дؿ��� protected �Ǥ���
 */
void **fits_table_col::data_ptr_mng_ptr()
{
    return &this->data_ptr_rec;
}

/**
 * @brief  ����μ����Τ���Υ��дؿ�
 *
 * @note   ���Υ��дؿ��� protected �Ǥ���
 */
void **fits_table_col::heap_ptr_mng_ptr()
{
    return &this->heap_ptr_rec;
}

/**
 * @brief  ��������४�֥������Ȥ���Υ���ݡ���
 *
 *  fits_table_col ���֥������� from �� idx_begin ���� num_rows �ĤιԤ�
 *  ���Ȥ� dest_index �ǻ��ꤵ�줿�Ԥ��� num_rows�ĤιԤإ���ݡ��Ȥ��ޤ���<br>
 *  from �λ��ĥ������������֥������ȤΥ����η��ϡ����פ��Ƥ���ɬ�פ�
 *  ����ޤ��󡥰��פ��ʤ����ϡ��ͤ��Ѵ����ƥ���ݡ��Ȥ��ޤ���
 *
 * @param  dest_index ����ݡ�����ιԥ���ǥå���
 * @param  from ����ݡ��ȸ��Υơ��֥륪�֥�������
 * @param  idx_begin ����ݡ��ȸ��ιԥ���ǥå���
 * @param  num_rows ����ݡ��Ȥ�Ԥ��Կ�
 * @return  ���Ȥλ���
 * @note  ����ݡ�����(����)�ιԿ����ѹ��ϹԤʤ��ޤ��󡥹Ԥη�»�ʤ�����ݡ���
 *        ����ˤϡ����餫���ώʬ�ʹԿ�����ݤ��Ƥ���ɬ�פ�����ޤ���
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
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
	/* &from == this �Ǥ��ɤ��褦�� */
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
 * @brief  �����ιԤΥ�����ͤ�����(�ǥե�����ͤ򥻥å�)
 *
 *  �ǥե�����ͤν�������ͤϡ��������ξ��� 'F'��ʸ���󷿤ξ��� ' ' ����
 *  �ʤ�ʸ���󡤤���¾�η��ξ��� 0 �Ǥ����ǥե�����ͤ� 
 *  fits_table_col::assign_default() ���дؿ����ѹ��Ǥ��ޤ���
 *
 * @param   index ��������ϰ��֤򼨤��ԥ���ǥå���
 * @param   num_rows ����������Կ�
 * @return  ���Ȥλ���
 * @attention  ����Ĺ����ξ�硤���󵭽һҤ��Ф������Ȥʤ�ޤ���
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
 * @brief  ���ƤιԤΥ�����ͤ�����(�ǥե�����ͤ򥻥å�)
 *
 *  �ǥե�����ͤν�������ͤϡ��������ξ��� 'F'��ʸ���󷿤ξ��� ' ' ����
 *  �ʤ�ʸ���󡤤���¾�η��ξ��� 0 �Ǥ����ǥե�����ͤ� 
 *  fits_table_col::assign_default() ���дؿ����ѹ��Ǥ��ޤ���
 *
 * @return  ���Ȥλ���
 * @attention  ����Ĺ����ξ�硤���󵭽һҤ��Ф������Ȥʤ�ޤ���
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
 * @brief  ������ͤ�¿��ͤǼ��� (���٥�)
 *
 *  ������ͤˡ��إå��� TZEROn �� TSCALn ��ȿ�Ǥ����¿��ͤ�������ޤ���<br>
 *  NULL�ͤξ��ϡ�NAN���֤��ޤ����Х��ʥ�ơ��֥�������������䥢������
 *  �ơ��֥�� TNULLn ���ͤˡ�������ͤ����פ�������NULL�ͤȤ��ޤ���<br>
 *  �������Υ����ξ��ϡ��ͤ� 'T' �ʤ� 1 ���ͤ� 'F' �ʤ� 0 �򡤤���ʳ���
 *  ���� NAN ���֤��ޤ���<br>
 *  �Х��ʥ�ơ��֥��ʸ���󷿤Υ����ξ��ȡ����������ơ��֥�� TFORMn
 *  ������ɽ���򼨤��ʤ����ˤϡ������ʸ�����¿��ͤ��Ѵ������ͤ򤽤Τޤ�
 *  �֤��ޤ���<br>
 *  �����Υ���ǥå����ϡ�0����Ϥޤ�����Ǥ���
 *
 * @param   row_index �ԥ���ǥå���
 * @return  �������: ���ｪλ��NULL�ͤξ���NAN <br>
 *          NAN: �����������ʾ��
 * @note  �Х��ʥ�ơ��֥��ʸ���󷿤Υ����Ǥ� TNULLn �����꤬��ǽ�Ǥ�������
 *        �� SFITSIO ���ȼ���ĥ�Ǥ���
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
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
 * @brief  ������ͤ�¿��ͤǼ��� (���٥�)
 *
 *  ������ͤˡ��إå��� TZEROn �� TSCALn ��ȿ�Ǥ����¿��ͤ�������ޤ���<br>
 *  NULL�ͤξ��ϡ�NAN���֤��ޤ����Х��ʥ�ơ��֥�������������䥢������
 *  �ơ��֥�� TNULLn ���ͤˡ�������ͤ����פ�������NULL�ͤȤ��ޤ���<br>
 *  �������Υ����ξ��ϡ��ͤ� 'T' �ʤ� 1 ���ͤ� 'F' �ʤ� 0 �򡤤���ʳ���
 *  ���� NAN ���֤��ޤ���<br>
 *  �Х��ʥ�ơ��֥��ʸ���󷿤Υ����ξ��ȡ����������ơ��֥�� TFORMn
 *  ������ɽ���򼨤��ʤ����ˤϡ������ʸ�����¿��ͤ��Ѵ������ͤ򤽤Τޤ�
 *  �֤��ޤ���<br>
 *  �����Υ���ǥå����ϡ�0����Ϥޤ�����Ǥ���
 *
 * @param   row_index �ԥ���ǥå���
 * @param   elem_name ����̾ (TELEMn ��¸�ߤ���̾��)
 * @param   repetition_idx  TDIMn ��2�����ܤΥ���ǥå���
 * @return  �������: ���ｪλ��NULL�ͤξ���NAN <br>
 *          NAN: �����������ʾ��
 * @note  �Х��ʥ�ơ��֥��ʸ���󷿤Υ����Ǥ� TNULLn �����꤬��ǽ�Ǥ�������
 *        �� SFITSIO ���ȼ���ĥ�Ǥ���
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
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
 * @brief  ������ͤ�¿��ͤǼ��� (���٥�)
 *
 *  ������ͤˡ��إå��� TZEROn �� TSCALn ��ȿ�Ǥ����¿��ͤ�������ޤ���<br>
 *  NULL�ͤξ��ϡ�NAN���֤��ޤ����Х��ʥ�ơ��֥�������������䥢������
 *  �ơ��֥�� TNULLn ���ͤˡ�������ͤ����פ�������NULL�ͤȤ��ޤ���<br>
 *  �������Υ����ξ��ϡ��ͤ� 'T' �ʤ� 1 ���ͤ� 'F' �ʤ� 0 �򡤤���ʳ���
 *  ���� NAN ���֤��ޤ���<br>
 *  �Х��ʥ�ơ��֥��ʸ���󷿤Υ����ξ��ȡ����������ơ��֥�� TFORMn
 *  ������ɽ���򼨤��ʤ����ˤϡ������ʸ�����¿��ͤ��Ѵ������ͤ򤽤Τޤ�
 *  �֤��ޤ���<br>
 *  �����Υ���ǥå����ϡ�0����Ϥޤ�����Ǥ���
 *
 * @param   row_index �ԥ���ǥå���
 * @param   elem_index ���ǥ���ǥå��� (TDIMn ��1�����ܤΥ���ǥå���)
 * @param   repetition_idx TDIMn ��2�����ܤΥ���ǥå���
 * @return  �������: ���ｪλ��NULL�ͤξ���NAN <br>
 *          NAN: �����������ʾ��
 * @note  �Х��ʥ�ơ��֥��ʸ���󷿤Υ����Ǥ� TNULLn �����꤬��ǽ�Ǥ�������
 *        �� SFITSIO ���ȼ���ĥ�Ǥ���
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
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
 * @brief  ������ͤ������ͤǼ��� (���٥�)
 *
 *  ������ͤˡ��إå��� TZEROn �� TSCALn ��ȿ�Ǥ����¿��ͤ˺Ǥ�ᤤ�����ͤ�
 *  �֤��ޤ������������ͤ� long ����ɽ���Ǥ����ϰϤ�����ʤ���硤INDEF_LONG
 *  ���֤��ޤ���<br>
 *  NULL�ͤξ��ϡ�INDEF_LONG ���֤��ޤ����Х��ʥ�ơ��֥��������������
 *  ���������ơ��֥�� TNULLn ���ͤˡ�������ͤ����פ�������NULL�ͤȤ���
 *  ����<br>
 *  �������Υ����ξ��ϡ��ͤ� 'T' �ʤ� 1 ���ͤ� 'F' �ʤ� 0 �򡤤���ʳ���
 *  ���� INDEF_LONG ���֤��ޤ���<br>
 *  �Х��ʥ�ơ��֥��ʸ���󷿤Υ����ξ��ȡ����������ơ��֥������������
 *  TFORMn ������ɽ���򼨤��ʤ����ˤϡ������ʸ�����¿��ͤ��Ѵ����������ͤ�
 *  �Ǥ�ᤤ�����ͤ��֤��ޤ���
 *  �����Υ���ǥå����ϡ�0����Ϥޤ�����Ǥ���
 *
 * @param   row_index �ԥ���ǥå���
 * @return  ������͡�NULL�ͤξ���INDEF_LONG <br>
 *          INDEF_LONG: �����������ʾ��
 * @note  �Х��ʥ�ơ��֥��ʸ���󷿤Υ����Ǥ� TNULLn �����꤬��ǽ�Ǥ�������
 *        �� SFITSIO ���ȼ���ĥ�Ǥ���
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
 */
long fits_table_col::lvalue( long row_index ) const
{
    double dv = this->dvalue(row_index);
    if ( isfinite(dv) && MIN_DOUBLE_ROUND_LONG <= dv && dv <= MAX_DOUBLE_ROUND_LONG )
	return round_d2l(dv);
    else return INDEF_LONG;
}

/**
 * @brief  ������ͤ������ͤǼ��� (���٥�)
 *
 *  ������ͤˡ��إå��� TZEROn �� TSCALn ��ȿ�Ǥ����¿��ͤ˺Ǥ�ᤤ�����ͤ�
 *  �֤��ޤ������������ͤ� long ����ɽ���Ǥ����ϰϤ�����ʤ���硤INDEF_LONG
 *  ���֤��ޤ���<br>
 *  NULL�ͤξ��ϡ�INDEF_LONG ���֤��ޤ����Х��ʥ�ơ��֥��������������
 *  ���������ơ��֥�� TNULLn ���ͤˡ�������ͤ����פ�������NULL�ͤȤ���
 *  ����<br>
 *  �������Υ����ξ��ϡ��ͤ� 'T' �ʤ� 1 ���ͤ� 'F' �ʤ� 0 �򡤤���ʳ���
 *  ���� INDEF_LONG ���֤��ޤ���<br>
 *  �Х��ʥ�ơ��֥��ʸ���󷿤Υ����ξ��ȡ����������ơ��֥������������
 *  TFORMn ������ɽ���򼨤��ʤ����ˤϡ������ʸ�����¿��ͤ��Ѵ����������ͤ�
 *  �Ǥ�ᤤ�����ͤ��֤��ޤ���
 *  �����Υ���ǥå����ϡ�0����Ϥޤ�����Ǥ���
 *
 * @param   row_index �ԥ���ǥå���
 * @param   elem_name ����̾ (TELEMn ��¸�ߤ���̾��)
 * @param   repetition_idx TDIMn ��2�����ܤΥ���ǥå���
 * @return  ������͡�NULL�ͤξ���INDEF_LONG <br>
 *          INDEF_LONG: �����������ʾ��
 * @note  �Х��ʥ�ơ��֥��ʸ���󷿤Υ����Ǥ� TNULLn �����꤬��ǽ�Ǥ�������
 *        �� SFITSIO ���ȼ���ĥ�Ǥ���
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
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
 * @brief  ������ͤ������ͤǼ��� (���٥�)
 *
 *  ������ͤˡ��إå��� TZEROn �� TSCALn ��ȿ�Ǥ����¿��ͤ˺Ǥ�ᤤ�����ͤ�
 *  �֤��ޤ������������ͤ� long ����ɽ���Ǥ����ϰϤ�����ʤ���硤INDEF_LONG
 *  ���֤��ޤ���<br>
 *  NULL�ͤξ��ϡ�INDEF_LONG ���֤��ޤ����Х��ʥ�ơ��֥��������������
 *  ���������ơ��֥�� TNULLn ���ͤˡ�������ͤ����פ�������NULL�ͤȤ���
 *  ����<br>
 *  �������Υ����ξ��ϡ��ͤ� 'T' �ʤ� 1 ���ͤ� 'F' �ʤ� 0 �򡤤���ʳ���
 *  ���� INDEF_LONG ���֤��ޤ���<br>
 *  �Х��ʥ�ơ��֥��ʸ���󷿤Υ����ξ��ȡ����������ơ��֥������������
 *  TFORMn ������ɽ���򼨤��ʤ����ˤϡ������ʸ�����¿��ͤ��Ѵ����������ͤ�
 *  �Ǥ�ᤤ�����ͤ��֤��ޤ���
 *  �����Υ���ǥå����ϡ�0����Ϥޤ�����Ǥ���
 *
 * @param   row_index �ԥ���ǥå���
 * @param   elem_index ���ǥ���ǥå��� (TDIMn ��1�����ܤΥ���ǥå���)
 * @param   repetition_idx TDIMn ��2�����ܤΥ���ǥå���
 * @return  ������͡�NULL�ͤξ���INDEF_LONG <br>
 *          INDEF_LONG: �����������ʾ��
 * @note  �Х��ʥ�ơ��֥��ʸ���󷿤Υ����Ǥ� TNULLn �����꤬��ǽ�Ǥ�������
 *        �� SFITSIO ���ȼ���ĥ�Ǥ���
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
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
 * @brief  ������ͤ������ͤǼ��� (���٥�)
 *
 *  ������ͤˡ��إå��� TZEROn �� TSCALn ��ȿ�Ǥ����¿��ͤ˺Ǥ�ᤤ�����ͤ�
 *  �֤��ޤ������������ͤ� long long ����ɽ���Ǥ����ϰϤ�����ʤ���硤
 *  INDEF_LLONG ���֤��ޤ���<br>
 *  NULL�ͤξ��ϡ�INDEF_LLONG ���֤��ޤ����Х��ʥ�ơ��֥��������������
 *  ���������ơ��֥�� TNULLn ���ͤˡ�������ͤ����פ�������NULL�ͤȤ���
 *  ����<br>
 *  �������Υ����ξ��ϡ��ͤ� 'T' �ʤ� 1 ���ͤ� 'F' �ʤ� 0 �򡤤���ʳ���
 *  ���� INDEF_LLONG ���֤��ޤ���<br>
 *  �Х��ʥ�ơ��֥��ʸ���󷿤Υ����ξ��ȡ����������ơ��֥������������
 *  TFORMn ������ɽ���򼨤��ʤ����ˤϡ������ʸ�����¿��ͤ��Ѵ����������ͤ�
 *  �Ǥ�ᤤ�����ͤ��֤��ޤ���
 *  �����Υ���ǥå����ϡ�0����Ϥޤ�����Ǥ���
 *
 * @param   row_index �ԥ���ǥå���
 * @return  ������͡�NULL�ͤξ���INDEF_LLONG <br>
 *          INDEF_LLONG: �����������ʾ��
 * @note  �Х��ʥ�ơ��֥��ʸ���󷿤Υ����Ǥ� TNULLn �����꤬��ǽ�Ǥ�������
 *        �� SFITSIO ���ȼ���ĥ�Ǥ���
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
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
 * @brief  ������ͤ������ͤǼ��� (���٥�)
 *
 *  ������ͤˡ��إå��� TZEROn �� TSCALn ��ȿ�Ǥ����¿��ͤ˺Ǥ�ᤤ�����ͤ�
 *  �֤��ޤ������������ͤ� long long ����ɽ���Ǥ����ϰϤ�����ʤ���硤
 *  INDEF_LLONG ���֤��ޤ���<br>
 *  NULL�ͤξ��ϡ�INDEF_LLONG ���֤��ޤ����Х��ʥ�ơ��֥��������������
 *  ���������ơ��֥�� TNULLn ���ͤˡ�������ͤ����פ�������NULL�ͤȤ���
 *  ����<br>
 *  �������Υ����ξ��ϡ��ͤ� 'T' �ʤ� 1 ���ͤ� 'F' �ʤ� 0 �򡤤���ʳ���
 *  ���� INDEF_LLONG ���֤��ޤ���<br>
 *  �Х��ʥ�ơ��֥��ʸ���󷿤Υ����ξ��ȡ����������ơ��֥������������
 *  TFORMn ������ɽ���򼨤��ʤ����ˤϡ������ʸ�����¿��ͤ��Ѵ����������ͤ�
 *  �Ǥ�ᤤ�����ͤ��֤��ޤ���
 *  �����Υ���ǥå����ϡ�0����Ϥޤ�����Ǥ���
 *
 * @param   row_index �ԥ���ǥå���
 * @param   elem_name ����̾ (TELEMn ��¸�ߤ���̾��)
 * @param   repetition_idx TDIMn ��2�����ܤΥ���ǥå���
 * @return  ������͡�NULL�ͤξ���INDEF_LLONG <br>
 *          INDEF_LLONG: �����������ʾ��
 * @note  �Х��ʥ�ơ��֥��ʸ���󷿤Υ����Ǥ� TNULLn �����꤬��ǽ�Ǥ�������
 *        �� SFITSIO ���ȼ���ĥ�Ǥ���
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
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
 * @brief  ������ͤ������ͤǼ��� (���٥�)
 *
 *  ������ͤˡ��إå��� TZEROn �� TSCALn ��ȿ�Ǥ����¿��ͤ˺Ǥ�ᤤ�����ͤ�
 *  �֤��ޤ������������ͤ� long long ����ɽ���Ǥ����ϰϤ�����ʤ���硤
 *  INDEF_LLONG ���֤��ޤ���<br>
 *  NULL�ͤξ��ϡ�INDEF_LLONG ���֤��ޤ����Х��ʥ�ơ��֥��������������
 *  ���������ơ��֥�� TNULLn ���ͤˡ�������ͤ����פ�������NULL�ͤȤ���
 *  ����<br>
 *  �������Υ����ξ��ϡ��ͤ� 'T' �ʤ� 1 ���ͤ� 'F' �ʤ� 0 �򡤤���ʳ���
 *  ���� INDEF_LLONG ���֤��ޤ���<br>
 *  �Х��ʥ�ơ��֥��ʸ���󷿤Υ����ξ��ȡ����������ơ��֥������������
 *  TFORMn ������ɽ���򼨤��ʤ����ˤϡ������ʸ�����¿��ͤ��Ѵ����������ͤ�
 *  �Ǥ�ᤤ�����ͤ��֤��ޤ���
 *  �����Υ���ǥå����ϡ�0����Ϥޤ�����Ǥ���
 *
 * @param   row_index �ԥ���ǥå���
 * @param   elem_index ���ǥ���ǥå��� (TDIMn ��1�����ܤΥ���ǥå���)
 * @param   repetition_idx TDIMn ��2�����ܤΥ���ǥå���
 * @return  ������͡�NULL�ͤξ���INDEF_LLONG <br>
 *          INDEF_LLONG: �����������ʾ��
 * @note  �Х��ʥ�ơ��֥��ʸ���󷿤Υ����Ǥ� TNULLn �����꤬��ǽ�Ǥ�������
 *        �� SFITSIO ���ȼ���ĥ�Ǥ���
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
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
 * @brief  ������ͤ������ͤǼ��� (���٥�)
 *
 *  ������ͤ������ͤ��֤��ޤ����֤��ͤϡ�true �� false �ǡ���Ԥ� NULL�ͤξ��
 *  ��ޤߤޤ����Х��ʥ�ơ��֥�������������䥢�������ơ��֥�� TNULLn ����
 *  �ˡ�������ͤ����פ�������NULL�ͤȤ��ޤ���<br>
 *  'T'��'F'��'\0' ��3������ͤ�ɬ�פȤ�����ϡ�
 *  fits_table_col::logical_value() ���дؿ���ȤäƤ���������<br>
 *  �������Υ����ξ�硤�ͤ� 'T' �ʤ� true �򡤤����Ǥʤ��ʤ� false ���֤�
 *  �ޤ���<br>
 *  ��������¿����Υ����ξ�硤������ͤˡ��إå��� TZEROn �� TSCAL n ��
 *  ȿ�Ǥ����¿��ͤ˺Ǥ�ᤤ�����ͤ� 0 �ʤ� false ��0 �Ǥʤ��ʤ� true ��
 *  �֤��ޤ���<br>
 *  �Х��ʥ�ơ��֥��ʸ���󷿤Υ����ξ��ȡ����������ơ��֥�� TFORMn ��
 *  ����ɽ���򼨤��ʤ����ˤϡ������ʸ����μ¿��ͤؤ��Ѵ����Ԥ����Ѵ���ǽ
 *  �ʾ��Ϥ��μ¿��ͤ˺Ǥ�ᤤ�����ͤ� 0 �ʤ� false ��0 �Ǥʤ��ʤ� true ��
 *  �֤��ޤ����Ѵ��Բ�ǽ�ʾ��ϡ���Ƭʸ��(����Ͻ���)�� 'T' �ޤ��� 't' �ξ��
 *  �ˤ� true �򡤤����Ǥʤ��ʤ� false ���֤��ޤ���<br>
 *  �����Υ���ǥå����ϡ�0����Ϥޤ�����Ǥ���
 *
 * @param   row_index �ԥ���ǥå���
 * @return  ������͡�NULL�ͤξ���false <br>
 *          false: �����������ʾ��
 * @note  �Х��ʥ�ơ��֥��ʸ���󷿤Υ����Ǥ� TNULLn �����꤬��ǽ�Ǥ�������
 *        �� SFITSIO ���ȼ���ĥ�Ǥ���
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
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
 * @brief  ������ͤ������ͤǼ��� (���٥�)
 *
 *  ������ͤ������ͤ��֤��ޤ����֤��ͤϡ�true �� false �ǡ���Ԥ� NULL�ͤξ��
 *  ��ޤߤޤ����Х��ʥ�ơ��֥�������������䥢�������ơ��֥�� TNULLn ����
 *  �ˡ�������ͤ����פ�������NULL�ͤȤ��ޤ���<br>
 *  'T'��'F'��'\0' ��3������ͤ�ɬ�פȤ�����ϡ�
 *  fits_table_col::logical_value() ���дؿ���ȤäƤ���������<br>
 *  �������Υ����ξ�硤�ͤ� 'T' �ʤ� true �򡤤����Ǥʤ��ʤ� false ���֤�
 *  �ޤ���<br>
 *  ��������¿����Υ����ξ�硤������ͤˡ��إå��� TZEROn �� TSCAL n ��
 *  ȿ�Ǥ����¿��ͤ˺Ǥ�ᤤ�����ͤ� 0 �ʤ� false ��0 �Ǥʤ��ʤ� true ��
 *  �֤��ޤ���<br>
 *  �Х��ʥ�ơ��֥��ʸ���󷿤Υ����ξ��ȡ����������ơ��֥�� TFORMn ��
 *  ����ɽ���򼨤��ʤ����ˤϡ������ʸ����μ¿��ͤؤ��Ѵ����Ԥ����Ѵ���ǽ
 *  �ʾ��Ϥ��μ¿��ͤ˺Ǥ�ᤤ�����ͤ� 0 �ʤ� false ��0 �Ǥʤ��ʤ� true ��
 *  �֤��ޤ����Ѵ��Բ�ǽ�ʾ��ϡ���Ƭʸ��(����Ͻ���)�� 'T' �ޤ��� 't' �ξ��
 *  �ˤ� true �򡤤����Ǥʤ��ʤ� false ���֤��ޤ���<br>
 *  �����Υ���ǥå����ϡ�0����Ϥޤ�����Ǥ���
 *
 * @param   row_index �ԥ���ǥå���
 * @param   elem_name ����̾ (TELEMn ��¸�ߤ���̾��)
 * @param   repetition_idx TDIMn ��2�����ܤΥ���ǥå���
 * @return  ������͡�NULL�ͤξ���false <br>
 *          false: �����������ʾ��
 * @note  �Х��ʥ�ơ��֥��ʸ���󷿤Υ����Ǥ� TNULLn �����꤬��ǽ�Ǥ�������
 *        �� SFITSIO ���ȼ���ĥ�Ǥ���
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
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
 * @brief  ������ͤ������ͤǼ��� (���٥�)
 *
 *  ������ͤ������ͤ��֤��ޤ����֤��ͤϡ�true �� false �ǡ���Ԥ� NULL�ͤξ��
 *  ��ޤߤޤ����Х��ʥ�ơ��֥�������������䥢�������ơ��֥�� TNULLn ����
 *  �ˡ�������ͤ����פ�������NULL�ͤȤ��ޤ���<br>
 *  'T'��'F'��'\0' ��3������ͤ�ɬ�פȤ�����ϡ�
 *  fits_table_col::logical_value() ���дؿ���ȤäƤ���������<br>
 *  �������Υ����ξ�硤�ͤ� 'T' �ʤ� true �򡤤����Ǥʤ��ʤ� false ���֤�
 *  �ޤ���<br>
 *  ��������¿����Υ����ξ�硤������ͤˡ��إå��� TZEROn �� TSCAL n ��
 *  ȿ�Ǥ����¿��ͤ˺Ǥ�ᤤ�����ͤ� 0 �ʤ� false ��0 �Ǥʤ��ʤ� true ��
 *  �֤��ޤ���<br>
 *  �Х��ʥ�ơ��֥��ʸ���󷿤Υ����ξ��ȡ����������ơ��֥�� TFORMn ��
 *  ����ɽ���򼨤��ʤ����ˤϡ������ʸ����μ¿��ͤؤ��Ѵ����Ԥ����Ѵ���ǽ
 *  �ʾ��Ϥ��μ¿��ͤ˺Ǥ�ᤤ�����ͤ� 0 �ʤ� false ��0 �Ǥʤ��ʤ� true ��
 *  �֤��ޤ����Ѵ��Բ�ǽ�ʾ��ϡ���Ƭʸ��(����Ͻ���)�� 'T' �ޤ��� 't' �ξ��
 *  �ˤ� true �򡤤����Ǥʤ��ʤ� false ���֤��ޤ���<br>
 *  �����Υ���ǥå����ϡ�0����Ϥޤ�����Ǥ���
 *
 * @param   row_index �ԥ���ǥå���
 * @param   elem_index ���ǥ���ǥå��� (TDIMn ��1�����ܤΥ���ǥå���)
 * @param   repetition_idx TDIMn ��2�����ܤΥ���ǥå���
 * @return  ������͡�NULL�ͤξ���false <br>
 *          false: �����������ʾ��
 * @note  �Х��ʥ�ơ��֥��ʸ���󷿤Υ����Ǥ� TNULLn �����꤬��ǽ�Ǥ�������
 *        �� SFITSIO ���ȼ���ĥ�Ǥ���
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
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
 * @brief  ������ͤ�ʸ�����ͤǼ��� (���٥�)
 *
 *  FITS����˴�Ť��Ƽ��Ѵ�������Ԥʤä������ͤ�ʸ�����ͤ��֤��ޤ���<br>
 *  NULL�ͤξ�硤fits_table_col::assign_null_svalue() �ǻ��ꤵ�줿 NULL ʸ����
 *  ���֤���ޤ�(NULLʸ����ν�������ͤ� "NULL" �Ǥ�)���Х��ʥ�ơ��֥����
 *  ���������䥢�������ơ��֥�� TNULLn ���ͤˡ�������ͤ����פ�������
 *  NULL�ͤȤ��ޤ���<br>
 *  ���Х��ʥ�ơ��֥�ξ�� <br>
 *  ��������¿����Υ����ξ�硤������ͤ�إå��� TZEROn �� TSCALn ���ͤ�
 *  �Ѵ����������ʸ������Ѵ������ͤ��֤��ޤ���<br>
 *  TDISPn �λ��꤬������ϡ�TDISPn �λ���˽��äƥե����ޥåȤ��줿ʸ�����
 *  �֤��ޤ���<br>
 *  TDISPn �λ��̵꤬����硤�¿����Υ����� TZEROn��TSCALn �����ꤵ��Ƥ���
 *  �������Υ����Ǥϡ��ʲ���printf()�ؿ��Υե����ޥåȤ˽��ä��Ѵ�����ʸ����
 *  ���֤��ޤ���<br>
 *  FITS::FLOAT_T                    ... "%.7G" <br>
 *  FITS::DOUBLE_T, FITS::LONGLONG_T ... "%.15G" <br>
 *  FITS::LONG_T                     ... "%.10G" <br>
 *  �嵭�ʳ��Υ���ॿ����           ... "%.7G" <br>
 *  �����������ơ��֥�ξ�� <br>
 *  TFORMn ������ɽ���򼨤��ʤ����ˤϡ������ʸ����� TFORMn �ǥե����ޥå�
 *  ����ʸ������֤��ޤ�(TFORMn �λ��̵꤬�����ϡ������ʸ����򤽤Τޤ��֤�
 *  �ޤ�)��<br>
 *  TFORMn ������ɽ���򼨤����ˤϡ������ʸ�����¿��ͤ��Ѵ����������ͤ�
 *  TZEROn �� TSCALn ���Ѵ����������ͤ� TFORMn �ǥե����ޥåȤ���ʸ������֤�
 *  �ޤ���<br>
 *
 * @param   row_index �ԥ���ǥå���
 * @return  �������: ���ｪλ��NULL�ͤξ��� NULLʸ���� <br>
 *          NULLʸ����: �����������ʾ��
 * @note  �Х��ʥ�ơ��֥��ʸ���󷿤Υ����Ǥ� TNULLn �����꤬��ǽ�Ǥ�������
 *        �� SFITSIO ���ȼ���ĥ�Ǥ���
 * @note  SFITSIO �Ǥϡ�����෿�� FITS::ASCII_T (TFORMn �λ���� 'A' ��ޤ�) 
 *        �ξ�硤TNULLn �����꤬�ʤ���С���Ƭʸ���� '\0' �Υ���� NULL�ͤȤ�
 *        �ޤ���������ʸ��������٥��NULL�͡פȸƤ�Ǥ��ޤ���
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
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
 * @brief  ������ͤ�ʸ�����ͤǼ��� (���٥�)
 *
 *  FITS����˴�Ť��Ƽ��Ѵ�������Ԥʤä������ͤ�ʸ�����ͤ��֤��ޤ���<br>
 *  NULL�ͤξ�硤fits_table_col::assign_null_svalue() �ǻ��ꤵ�줿 NULL ʸ����
 *  ���֤���ޤ�(NULLʸ����ν�������ͤ� "NULL" �Ǥ�)���Х��ʥ�ơ��֥����
 *  ���������䥢�������ơ��֥�� TNULLn ���ͤˡ�������ͤ����פ�������
 *  NULL�ͤȤ��ޤ���<br>
 *  ���Х��ʥ�ơ��֥�ξ�� <br>
 *  ��������¿����Υ����ξ�硤������ͤ�إå��� TZEROn �� TSCALn ���ͤ�
 *  �Ѵ����������ʸ������Ѵ������ͤ��֤��ޤ���<br>
 *  TDISPn �λ��꤬������ϡ�TDISPn �λ���˽��äƥե����ޥåȤ��줿ʸ�����
 *  �֤��ޤ���<br>
 *  TDISPn �λ��̵꤬����硤�¿����Υ����� TZEROn��TSCALn �����ꤵ��Ƥ���
 *  �������Υ����Ǥϡ��ʲ���printf()�ؿ��Υե����ޥåȤ˽��ä��Ѵ�����ʸ����
 *  ���֤��ޤ���<br>
 *  FITS::FLOAT_T                    ... "%.7G" <br>
 *  FITS::DOUBLE_T, FITS::LONGLONG_T ... "%.15G" <br>
 *  FITS::LONG_T                     ... "%.10G" <br>
 *  �嵭�ʳ��Υ���ॿ����           ... "%.7G" <br>
 *  �����������ơ��֥�ξ�� <br>
 *  TFORMn ������ɽ���򼨤��ʤ����ˤϡ������ʸ����� TFORMn �ǥե����ޥå�
 *  ����ʸ������֤��ޤ�(TFORMn �λ��̵꤬�����ϡ������ʸ����򤽤Τޤ��֤�
 *  �ޤ�)��<br>
 *  TFORMn ������ɽ���򼨤����ˤϡ������ʸ�����¿��ͤ��Ѵ����������ͤ�
 *  TZEROn �� TSCALn ���Ѵ����������ͤ� TFORMn �ǥե����ޥåȤ���ʸ������֤�
 *  �ޤ���<br>
 *
 * @param   row_index �ԥ���ǥå���
 * @param   elem_name ����̾ (TELEMn ��¸�ߤ���̾��)
 * @param   repetition_idx TDIMn ��2�����ܤΥ���ǥå���
 * @return  �������: ���ｪλ��NULL�ͤξ��� NULLʸ���� <br>
 *          NULLʸ����: �����������ʾ��
 * @note  �Х��ʥ�ơ��֥��ʸ���󷿤Υ����Ǥ� TNULLn �����꤬��ǽ�Ǥ�������
 *        �� SFITSIO ���ȼ���ĥ�Ǥ���
 * @note  SFITSIO �Ǥϡ�����෿�� FITS::ASCII_T (TFORMn �λ���� 'A' ��ޤ�) 
 *        �ξ�硤TNULLn �����꤬�ʤ���С���Ƭʸ���� '\0' �Υ���� NULL�ͤȤ�
 *        �ޤ���������ʸ��������٥��NULL�͡פȸƤ�Ǥ��ޤ���
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
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
 * @brief  ������ͤ�ʸ�����ͤǼ��� (���٥�)
 *
 *  FITS����˴�Ť��Ƽ��Ѵ�������Ԥʤä������ͤ�ʸ�����ͤ��֤��ޤ���<br>
 *  NULL�ͤξ�硤fits_table_col::assign_null_svalue() �ǻ��ꤵ�줿 NULL ʸ����
 *  ���֤���ޤ�(NULLʸ����ν�������ͤ� "NULL" �Ǥ�)���Х��ʥ�ơ��֥����
 *  ���������䥢�������ơ��֥�� TNULLn ���ͤˡ�������ͤ����פ�������
 *  NULL�ͤȤ��ޤ���<br>
 *  ���Х��ʥ�ơ��֥�ξ�� <br>
 *  ��������¿����Υ����ξ�硤������ͤ�إå��� TZEROn �� TSCALn ���ͤ�
 *  �Ѵ����������ʸ������Ѵ������ͤ��֤��ޤ���<br>
 *  TDISPn �λ��꤬������ϡ�TDISPn �λ���˽��äƥե����ޥåȤ��줿ʸ�����
 *  �֤��ޤ���<br>
 *  TDISPn �λ��̵꤬����硤�¿����Υ����� TZEROn��TSCALn �����ꤵ��Ƥ���
 *  �������Υ����Ǥϡ��ʲ���printf()�ؿ��Υե����ޥåȤ˽��ä��Ѵ�����ʸ����
 *  ���֤��ޤ���<br>
 *  FITS::FLOAT_T                    ... "%.7G" <br>
 *  FITS::DOUBLE_T, FITS::LONGLONG_T ... "%.15G" <br>
 *  FITS::LONG_T                     ... "%.10G" <br>
 *  �嵭�ʳ��Υ���ॿ����           ... "%.7G" <br>
 *  �����������ơ��֥�ξ�� <br>
 *  TFORMn ������ɽ���򼨤��ʤ����ˤϡ������ʸ����� TFORMn �ǥե����ޥå�
 *  ����ʸ������֤��ޤ�(TFORMn �λ��̵꤬�����ϡ������ʸ����򤽤Τޤ��֤�
 *  �ޤ�)��<br>
 *  TFORMn ������ɽ���򼨤����ˤϡ������ʸ�����¿��ͤ��Ѵ����������ͤ�
 *  TZEROn �� TSCALn ���Ѵ����������ͤ� TFORMn �ǥե����ޥåȤ���ʸ������֤�
 *  �ޤ���<br>
 *
 * @param   row_index �ԥ���ǥå���
 * @param   elem_index ���ǥ���ǥå��� (TDIMn ��1�����ܤΥ���ǥå���)
 * @param   repetition_idx TDIMn ��2�����ܤΥ���ǥå���
 * @return  �������: ���ｪλ��NULL�ͤξ��� NULLʸ���� <br>
 *          NULLʸ����: �����������ʾ��
 * @note  �Х��ʥ�ơ��֥��ʸ���󷿤Υ����Ǥ� TNULLn �����꤬��ǽ�Ǥ�������
 *        �� SFITSIO ���ȼ���ĥ�Ǥ���
 * @note  SFITSIO �Ǥϡ�����෿�� FITS::ASCII_T (TFORMn �λ���� 'A' ��ޤ�) 
 *        �ξ�硤TNULLn �����꤬�ʤ���С���Ƭʸ���� '\0' �Υ���� NULL�ͤȤ�
 *        �ޤ���������ʸ��������٥��NULL�͡פȸƤ�Ǥ��ޤ���
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
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
 * @brief  ������ͤ�ʸ�����ͤǼ��� (���٥롦tstring�Ǽ���)
 *
 *  FITS����˴�Ť��Ƽ��Ѵ�������Ԥʤä������ͤ�ʸ�����ͤ��֤��ޤ���<br>
 *  NULL�ͤξ�硤fits_table_col::assign_null_svalue() �ǻ��ꤵ�줿 NULL ʸ����
 *  ���֤���ޤ�(NULLʸ����ν�������ͤ� "NULL" �Ǥ�)���Х��ʥ�ơ��֥����
 *  ���������䥢�������ơ��֥�� TNULLn ���ͤˡ�������ͤ����פ�������
 *  NULL�ͤȤ��ޤ���<br>
 *  ���Х��ʥ�ơ��֥�ξ�� <br>
 *  ��������¿����Υ����ξ�硤������ͤ�إå��� TZEROn �� TSCALn ���ͤ�
 *  �Ѵ����������ʸ������Ѵ������ͤ��֤��ޤ���<br>
 *  TDISPn �λ��꤬������ϡ�TDISPn �λ���˽��äƥե����ޥåȤ��줿ʸ�����
 *  �֤��ޤ���<br>
 *  TDISPn �λ��̵꤬����硤�¿����Υ����� TZEROn��TSCALn �����ꤵ��Ƥ���
 *  �������Υ����Ǥϡ��ʲ���printf()�ؿ��Υե����ޥåȤ˽��ä��Ѵ�����ʸ����
 *  ���֤��ޤ���<br>
 *  FITS::FLOAT_T                    ... "%.7G" <br>
 *  FITS::DOUBLE_T, FITS::LONGLONG_T ... "%.15G" <br>
 *  FITS::LONG_T                     ... "%.10G" <br>
 *  �嵭�ʳ��Υ���ॿ����           ... "%.7G" <br>
 *  �����������ơ��֥�ξ�� <br>
 *  TFORMn ������ɽ���򼨤��ʤ����ˤϡ������ʸ����� TFORMn �ǥե����ޥå�
 *  ����ʸ������֤��ޤ�(TFORMn �λ��̵꤬�����ϡ������ʸ����򤽤Τޤ��֤�
 *  �ޤ�)��<br>
 *  TFORMn ������ɽ���򼨤����ˤϡ������ʸ�����¿��ͤ��Ѵ����������ͤ�
 *  TZEROn �� TSCALn ���Ѵ����������ͤ� TFORMn �ǥե����ޥåȤ���ʸ������֤�
 *  �ޤ���<br>
 *
 * @param   row_index �ԥ���ǥå���
 * @param   dest �����ͤγ�Ǽ�� (return)
 * @return  �������: ���ｪλ��NULL�ͤξ��� NULLʸ���� <br>
 *          NULLʸ����: �����������ʾ�� <br>
 *          NULL: dest �� NULL �ξ��
 * @note  �Х��ʥ�ơ��֥��ʸ���󷿤Υ����Ǥ� TNULLn �����꤬��ǽ�Ǥ�������
 *        �� SFITSIO ���ȼ���ĥ�Ǥ���
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
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
 * @brief  ������ͤ�ʸ�����ͤǼ��� (���٥롦tstring�Ǽ���)
 *
 *  FITS����˴�Ť��Ƽ��Ѵ�������Ԥʤä������ͤ�ʸ�����ͤ��֤��ޤ���<br>
 *  NULL�ͤξ�硤fits_table_col::assign_null_svalue() �ǻ��ꤵ�줿 NULL ʸ����
 *  ���֤���ޤ�(NULLʸ����ν�������ͤ� "NULL" �Ǥ�)���Х��ʥ�ơ��֥����
 *  ���������䥢�������ơ��֥�� TNULLn ���ͤˡ�������ͤ����פ�������
 *  NULL�ͤȤ��ޤ���<br>
 *  ���Х��ʥ�ơ��֥�ξ�� <br>
 *  ��������¿����Υ����ξ�硤������ͤ�إå��� TZEROn �� TSCALn ���ͤ�
 *  �Ѵ����������ʸ������Ѵ������ͤ��֤��ޤ���<br>
 *  TDISPn �λ��꤬������ϡ�TDISPn �λ���˽��äƥե����ޥåȤ��줿ʸ�����
 *  �֤��ޤ���<br>
 *  TDISPn �λ��̵꤬����硤�¿����Υ����� TZEROn��TSCALn �����ꤵ��Ƥ���
 *  �������Υ����Ǥϡ��ʲ���printf()�ؿ��Υե����ޥåȤ˽��ä��Ѵ�����ʸ����
 *  ���֤��ޤ���<br>
 *  FITS::FLOAT_T                    ... "%.7G" <br>
 *  FITS::DOUBLE_T, FITS::LONGLONG_T ... "%.15G" <br>
 *  FITS::LONG_T                     ... "%.10G" <br>
 *  �嵭�ʳ��Υ���ॿ����           ... "%.7G" <br>
 *  �����������ơ��֥�ξ�� <br>
 *  TFORMn ������ɽ���򼨤��ʤ����ˤϡ������ʸ����� TFORMn �ǥե����ޥå�
 *  ����ʸ������֤��ޤ�(TFORMn �λ��̵꤬�����ϡ������ʸ����򤽤Τޤ��֤�
 *  �ޤ�)��<br>
 *  TFORMn ������ɽ���򼨤����ˤϡ������ʸ�����¿��ͤ��Ѵ����������ͤ�
 *  TZEROn �� TSCALn ���Ѵ����������ͤ� TFORMn �ǥե����ޥåȤ���ʸ������֤�
 *  �ޤ���<br>
 *
 * @param   row_index �ԥ���ǥå���
 * @param   elem_name ����̾ (TELEMn ��¸�ߤ���̾��)
 * @param   dest �����ͤγ�Ǽ�� (return)
 * @return  �������: ���ｪλ��NULL�ͤξ��� NULLʸ���� <br>
 *          NULLʸ����: �����������ʾ�� <br>
 *          NULL: dest �� NULL �ξ��
 * @note  �Х��ʥ�ơ��֥��ʸ���󷿤Υ����Ǥ� TNULLn �����꤬��ǽ�Ǥ�������
 *        �� SFITSIO ���ȼ���ĥ�Ǥ���
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
 */
const char *fits_table_col::get_svalue( long row_index,
					const char *elem_name, 
					tstring *dest ) const
{
    if ( dest == NULL ) return NULL;

    return this->get_svalue( row_index, elem_name, 0, dest );
}

/**
 * @brief  ������ͤ�ʸ�����ͤǼ��� (���٥롦tstring�Ǽ���)
 *
 *  FITS����˴�Ť��Ƽ��Ѵ�������Ԥʤä������ͤ�ʸ�����ͤ��֤��ޤ���<br>
 *  NULL�ͤξ�硤fits_table_col::assign_null_svalue() �ǻ��ꤵ�줿 NULL ʸ����
 *  ���֤���ޤ�(NULLʸ����ν�������ͤ� "NULL" �Ǥ�)���Х��ʥ�ơ��֥����
 *  ���������䥢�������ơ��֥�� TNULLn ���ͤˡ�������ͤ����פ�������
 *  NULL�ͤȤ��ޤ���<br>
 *  ���Х��ʥ�ơ��֥�ξ�� <br>
 *  ��������¿����Υ����ξ�硤������ͤ�إå��� TZEROn �� TSCALn ���ͤ�
 *  �Ѵ����������ʸ������Ѵ������ͤ��֤��ޤ���<br>
 *  TDISPn �λ��꤬������ϡ�TDISPn �λ���˽��äƥե����ޥåȤ��줿ʸ�����
 *  �֤��ޤ���<br>
 *  TDISPn �λ��̵꤬����硤�¿����Υ����� TZEROn��TSCALn �����ꤵ��Ƥ���
 *  �������Υ����Ǥϡ��ʲ���printf()�ؿ��Υե����ޥåȤ˽��ä��Ѵ�����ʸ����
 *  ���֤��ޤ���<br>
 *  FITS::FLOAT_T                    ... "%.7G" <br>
 *  FITS::DOUBLE_T, FITS::LONGLONG_T ... "%.15G" <br>
 *  FITS::LONG_T                     ... "%.10G" <br>
 *  �嵭�ʳ��Υ���ॿ����           ... "%.7G" <br>
 *  �����������ơ��֥�ξ�� <br>
 *  TFORMn ������ɽ���򼨤��ʤ����ˤϡ������ʸ����� TFORMn �ǥե����ޥå�
 *  ����ʸ������֤��ޤ�(TFORMn �λ��̵꤬�����ϡ������ʸ����򤽤Τޤ��֤�
 *  �ޤ�)��<br>
 *  TFORMn ������ɽ���򼨤����ˤϡ������ʸ�����¿��ͤ��Ѵ����������ͤ�
 *  TZEROn �� TSCALn ���Ѵ����������ͤ� TFORMn �ǥե����ޥåȤ���ʸ������֤�
 *  �ޤ���<br>
 *
 * @param   row_index �ԥ���ǥå���
 * @param   elem_name ����̾ (TELEMn ��¸�ߤ���̾��)
 * @param   repetition_idx TDIMn ��2�����ܤΥ���ǥå���
 * @param   dest �����ͤγ�Ǽ�� (return)
 * @return  �������: ���ｪλ��NULL�ͤξ��� NULLʸ���� <br>
 *          NULLʸ����: �����������ʾ�� <br>
 *          NULL: dest �� NULL �ξ��
 * @note  �Х��ʥ�ơ��֥��ʸ���󷿤Υ����Ǥ� TNULLn �����꤬��ǽ�Ǥ�������
 *        �� SFITSIO ���ȼ���ĥ�Ǥ���
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
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
 * @brief  ������ͤ�ʸ�����ͤǼ��� (���٥롦tstring�Ǽ���)
 *
 *  FITS����˴�Ť��Ƽ��Ѵ�������Ԥʤä������ͤ�ʸ�����ͤ��֤��ޤ���<br>
 *  NULL�ͤξ�硤fits_table_col::assign_null_svalue() �ǻ��ꤵ�줿 NULL ʸ����
 *  ���֤���ޤ�(NULLʸ����ν�������ͤ� "NULL" �Ǥ�)���Х��ʥ�ơ��֥����
 *  ���������䥢�������ơ��֥�� TNULLn ���ͤˡ�������ͤ����פ�������
 *  NULL�ͤȤ��ޤ���<br>
 *  ���Х��ʥ�ơ��֥�ξ�� <br>
 *  ��������¿����Υ����ξ�硤������ͤ�إå��� TZEROn �� TSCALn ���ͤ�
 *  �Ѵ����������ʸ������Ѵ������ͤ��֤��ޤ���<br>
 *  TDISPn �λ��꤬������ϡ�TDISPn �λ���˽��äƥե����ޥåȤ��줿ʸ�����
 *  �֤��ޤ���<br>
 *  TDISPn �λ��̵꤬����硤�¿����Υ����� TZEROn��TSCALn �����ꤵ��Ƥ���
 *  �������Υ����Ǥϡ��ʲ���printf()�ؿ��Υե����ޥåȤ˽��ä��Ѵ�����ʸ����
 *  ���֤��ޤ���<br>
 *  FITS::FLOAT_T                    ... "%.7G" <br>
 *  FITS::DOUBLE_T, FITS::LONGLONG_T ... "%.15G" <br>
 *  FITS::LONG_T                     ... "%.10G" <br>
 *  �嵭�ʳ��Υ���ॿ����           ... "%.7G" <br>
 *  �����������ơ��֥�ξ�� <br>
 *  TFORMn ������ɽ���򼨤��ʤ����ˤϡ������ʸ����� TFORMn �ǥե����ޥå�
 *  ����ʸ������֤��ޤ�(TFORMn �λ��̵꤬�����ϡ������ʸ����򤽤Τޤ��֤�
 *  �ޤ�)��<br>
 *  TFORMn ������ɽ���򼨤����ˤϡ������ʸ�����¿��ͤ��Ѵ����������ͤ�
 *  TZEROn �� TSCALn ���Ѵ����������ͤ� TFORMn �ǥե����ޥåȤ���ʸ������֤�
 *  �ޤ���<br>
 *
 * @param   row_index �ԥ���ǥå���
 * @param   elem_index ���ǥ���ǥå��� (TDIMn ��1�����ܤΥ���ǥå���)
 * @param   dest �����ͤγ�Ǽ�� (return)
 * @return  �������: ���ｪλ��NULL�ͤξ��� NULLʸ���� <br>
 *          NULLʸ����: �����������ʾ�� <br>
 *          NULL: dest �� NULL �ξ��
 * @note  �Х��ʥ�ơ��֥��ʸ���󷿤Υ����Ǥ� TNULLn �����꤬��ǽ�Ǥ�������
 *        �� SFITSIO ���ȼ���ĥ�Ǥ���
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
 */
const char *fits_table_col::get_svalue( long row_index,
					long elem_index, tstring *dest ) const
{
    if ( dest == NULL ) return NULL;

    return this->get_svalue( row_index, elem_index, 0, dest );
}

/**
 * @brief  ������ͤ�ʸ�����ͤǼ��� (���٥롦tstring�Ǽ���)
 *
 *  FITS����˴�Ť��Ƽ��Ѵ�������Ԥʤä������ͤ�ʸ�����ͤ��֤��ޤ���<br>
 *  NULL�ͤξ�硤fits_table_col::assign_null_svalue() �ǻ��ꤵ�줿 NULL ʸ����
 *  ���֤���ޤ�(NULLʸ����ν�������ͤ� "NULL" �Ǥ�)���Х��ʥ�ơ��֥����
 *  ���������䥢�������ơ��֥�� TNULLn ���ͤˡ�������ͤ����פ�������
 *  NULL�ͤȤ��ޤ���<br>
 *  ���Х��ʥ�ơ��֥�ξ�� <br>
 *  ��������¿����Υ����ξ�硤������ͤ�إå��� TZEROn �� TSCALn ���ͤ�
 *  �Ѵ����������ʸ������Ѵ������ͤ��֤��ޤ���<br>
 *  TDISPn �λ��꤬������ϡ�TDISPn �λ���˽��äƥե����ޥåȤ��줿ʸ�����
 *  �֤��ޤ���<br>
 *  TDISPn �λ��̵꤬����硤�¿����Υ����� TZEROn��TSCALn �����ꤵ��Ƥ���
 *  �������Υ����Ǥϡ��ʲ���printf()�ؿ��Υե����ޥåȤ˽��ä��Ѵ�����ʸ����
 *  ���֤��ޤ���<br>
 *  FITS::FLOAT_T                    ... "%.7G" <br>
 *  FITS::DOUBLE_T, FITS::LONGLONG_T ... "%.15G" <br>
 *  FITS::LONG_T                     ... "%.10G" <br>
 *  �嵭�ʳ��Υ���ॿ����           ... "%.7G" <br>
 *  �����������ơ��֥�ξ�� <br>
 *  TFORMn ������ɽ���򼨤��ʤ����ˤϡ������ʸ����� TFORMn �ǥե����ޥå�
 *  ����ʸ������֤��ޤ�(TFORMn �λ��̵꤬�����ϡ������ʸ����򤽤Τޤ��֤�
 *  �ޤ�)��<br>
 *  TFORMn ������ɽ���򼨤����ˤϡ������ʸ�����¿��ͤ��Ѵ����������ͤ�
 *  TZEROn �� TSCALn ���Ѵ����������ͤ� TFORMn �ǥե����ޥåȤ���ʸ������֤�
 *  �ޤ���<br>
 *
 * @param   row_index �ԥ���ǥå���
 * @param   elem_index ���ǥ���ǥå��� (TDIMn ��1�����ܤΥ���ǥå���)
 * @param   repetition_idx TDIMn ��2�����ܤΥ���ǥå���
 * @param   dest �����ͤγ�Ǽ�� (return)
 * @return  �������: ���ｪλ��NULL�ͤξ��� NULLʸ���� <br>
 *          NULLʸ����: �����������ʾ�� <br>
 *          NULL: dest �� NULL �ξ��
 * @note  �Х��ʥ�ơ��֥��ʸ���󷿤Υ����Ǥ� TNULLn �����꤬��ǽ�Ǥ�������
 *        �� SFITSIO ���ȼ���ĥ�Ǥ���
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
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
 * @deprecated  ��侩��<br>
 *              fits_table_col::get_svalue( long, tstring * ) <br>
 *              �򤪻Ȥ�����������
 */
const char *fits_table_col::get_svalue( long row_index, 
					tstring &dest ) const
{
    return this->get_svalue(row_index, &dest);
}

/**
 * @deprecated  ��侩��<br>
 *             fits_table_col::get_svalue( long, const char *, tstring * ) <br>
 *             �򤪻Ȥ�����������
 */
const char *fits_table_col::get_svalue( long row_index,
					const char *elem_name, 
					tstring &dest ) const
{
    return this->get_svalue(row_index, elem_name, 0, &dest);
}

/**
 * @deprecated  ��侩��<br>
 *       fits_table_col::get_svalue( long, const char *, long, tstring * ) <br>
 *       �򤪻Ȥ�����������
 */
const char *fits_table_col::get_svalue( long row_index,
					const char *elem_name, 
					long repetition_idx,
					tstring &dest ) const
{
    return this->get_svalue(row_index, elem_name, repetition_idx, &dest);
}

/**
 * @deprecated  ��侩��<br>
 *              fits_table_col::get_svalue( long, long, tstring * ) <br>
 *              �򤪻Ȥ�����������
 */
const char *fits_table_col::get_svalue( long row_index,
					long elem_index, tstring &dest ) const
{
    return this->get_svalue(row_index, elem_index, 0, &dest);
}

/**
 * @deprecated  ��侩��<br>
 *              fits_table_col::get_svalue( long, long, long, tstring * ) <br>
 *              �򤪻Ȥ�����������
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
 * @brief  ������ͤ�ʸ�����ͤǼ��� (���٥롦�桼���Хåե��˼���)
 *
 *  FITS����˴�Ť��Ƽ��Ѵ�������Ԥʤä������ͤ�ʸ�����ͤ��֤��ޤ���<br>
 *  NULL�ͤξ�硤fits_table_col::assign_null_svalue() �ǻ��ꤵ�줿 NULL ʸ����
 *  ���֤���ޤ�(NULLʸ����ν�������ͤ� "NULL" �Ǥ�)���Х��ʥ�ơ��֥����
 *  ���������䥢�������ơ��֥�� TNULLn ���ͤˡ�������ͤ����פ�������
 *  NULL�ͤȤ��ޤ���<br>
 *  ���Х��ʥ�ơ��֥�ξ�� <br>
 *  ��������¿����Υ����ξ�硤������ͤ�إå��� TZEROn �� TSCALn ���ͤ�
 *  �Ѵ����������ʸ������Ѵ������ͤ��֤��ޤ���<br>
 *  TDISPn �λ��꤬������ϡ�TDISPn �λ���˽��äƥե����ޥåȤ��줿ʸ�����
 *  �֤��ޤ���<br>
 *  TDISPn �λ��̵꤬����硤�¿����Υ����� TZEROn��TSCALn �����ꤵ��Ƥ���
 *  �������Υ����Ǥϡ��ʲ���printf()�ؿ��Υե����ޥåȤ˽��ä��Ѵ�����ʸ����
 *  ���֤��ޤ���<br>
 *  FITS::FLOAT_T                    ... "%.7G" <br>
 *  FITS::DOUBLE_T, FITS::LONGLONG_T ... "%.15G" <br>
 *  FITS::LONG_T                     ... "%.10G" <br>
 *  �嵭�ʳ��Υ���ॿ����           ... "%.7G" <br>
 *  �����������ơ��֥�ξ�� <br>
 *  TFORMn ������ɽ���򼨤��ʤ����ˤϡ������ʸ����� TFORMn �ǥե����ޥå�
 *  ����ʸ������֤��ޤ�(TFORMn �λ��̵꤬�����ϡ������ʸ����򤽤Τޤ��֤�
 *  �ޤ�)��<br>
 *  TFORMn ������ɽ���򼨤����ˤϡ������ʸ�����¿��ͤ��Ѵ����������ͤ�
 *  TZEROn �� TSCALn ���Ѵ����������ͤ� TFORMn �ǥե����ޥåȤ���ʸ������֤�
 *  �ޤ���<br>
 *
 * @param   row_index �ԥ���ǥå���
 * @param   dest_buf �����ͤγ�Ǽ�� (return)
 * @param   buf_size dest_buf �ΥХåե�������
 * @return  �������: �Хåե�Ĺ����ʬ�ʾ��˥��ԡ��Ǥ���ʸ����
 *                    ('\0'�ϴޤޤʤ�) <br>
 *          �����(���顼): �����������ǥ��ԡ�����ʤ��ä����
 * @note  �Х��ʥ�ơ��֥��ʸ���󷿤Υ����Ǥ� TNULLn �����꤬��ǽ�Ǥ�������
 *        �� SFITSIO ���ȼ���ĥ�Ǥ���
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
 */
ssize_t fits_table_col::get_svalue( long row_index, 
				    char *dest_buf, size_t buf_size ) const
{
    tstring sval;
    this->get_svalue( row_index, &sval );
    return sval.getstr(dest_buf,buf_size);
}

/**
 * @brief  ������ͤ�ʸ�����ͤǼ��� (���٥롦�桼���Хåե��˼���)
 *
 *  FITS����˴�Ť��Ƽ��Ѵ�������Ԥʤä������ͤ�ʸ�����ͤ��֤��ޤ���<br>
 *  NULL�ͤξ�硤fits_table_col::assign_null_svalue() �ǻ��ꤵ�줿 NULL ʸ����
 *  ���֤���ޤ�(NULLʸ����ν�������ͤ� "NULL" �Ǥ�)���Х��ʥ�ơ��֥����
 *  ���������䥢�������ơ��֥�� TNULLn ���ͤˡ�������ͤ����פ�������
 *  NULL�ͤȤ��ޤ���<br>
 *  ���Х��ʥ�ơ��֥�ξ�� <br>
 *  ��������¿����Υ����ξ�硤������ͤ�إå��� TZEROn �� TSCALn ���ͤ�
 *  �Ѵ����������ʸ������Ѵ������ͤ��֤��ޤ���<br>
 *  TDISPn �λ��꤬������ϡ�TDISPn �λ���˽��äƥե����ޥåȤ��줿ʸ�����
 *  �֤��ޤ���<br>
 *  TDISPn �λ��̵꤬����硤�¿����Υ����� TZEROn��TSCALn �����ꤵ��Ƥ���
 *  �������Υ����Ǥϡ��ʲ���printf()�ؿ��Υե����ޥåȤ˽��ä��Ѵ�����ʸ����
 *  ���֤��ޤ���<br>
 *  FITS::FLOAT_T                    ... "%.7G" <br>
 *  FITS::DOUBLE_T, FITS::LONGLONG_T ... "%.15G" <br>
 *  FITS::LONG_T                     ... "%.10G" <br>
 *  �嵭�ʳ��Υ���ॿ����           ... "%.7G" <br>
 *  �����������ơ��֥�ξ�� <br>
 *  TFORMn ������ɽ���򼨤��ʤ����ˤϡ������ʸ����� TFORMn �ǥե����ޥå�
 *  ����ʸ������֤��ޤ�(TFORMn �λ��̵꤬�����ϡ������ʸ����򤽤Τޤ��֤�
 *  �ޤ�)��<br>
 *  TFORMn ������ɽ���򼨤����ˤϡ������ʸ�����¿��ͤ��Ѵ����������ͤ�
 *  TZEROn �� TSCALn ���Ѵ����������ͤ� TFORMn �ǥե����ޥåȤ���ʸ������֤�
 *  �ޤ���<br>
 *
 * @param   row_index �ԥ���ǥå���
 * @param   elem_name ����̾ (TELEMn ��¸�ߤ���̾��)
 * @param   dest_buf �����ͤγ�Ǽ�� (return)
 * @param   buf_size dest_buf �ΥХåե�������
 * @return  �������: �Хåե�Ĺ����ʬ�ʾ��˥��ԡ��Ǥ���ʸ����
 *                    ('\0'�ϴޤޤʤ�) <br>
 *          �����(���顼): �����������ǥ��ԡ�����ʤ��ä����
 * @note  �Х��ʥ�ơ��֥��ʸ���󷿤Υ����Ǥ� TNULLn �����꤬��ǽ�Ǥ�������
 *        �� SFITSIO ���ȼ���ĥ�Ǥ���
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
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
 * @brief  ������ͤ�ʸ�����ͤǼ��� (���٥롦�桼���Хåե��˼���)
 *
 *  FITS����˴�Ť��Ƽ��Ѵ�������Ԥʤä������ͤ�ʸ�����ͤ��֤��ޤ���<br>
 *  NULL�ͤξ�硤fits_table_col::assign_null_svalue() �ǻ��ꤵ�줿 NULL ʸ����
 *  ���֤���ޤ�(NULLʸ����ν�������ͤ� "NULL" �Ǥ�)���Х��ʥ�ơ��֥����
 *  ���������䥢�������ơ��֥�� TNULLn ���ͤˡ�������ͤ����פ�������
 *  NULL�ͤȤ��ޤ���<br>
 *  ���Х��ʥ�ơ��֥�ξ�� <br>
 *  ��������¿����Υ����ξ�硤������ͤ�إå��� TZEROn �� TSCALn ���ͤ�
 *  �Ѵ����������ʸ������Ѵ������ͤ��֤��ޤ���<br>
 *  TDISPn �λ��꤬������ϡ�TDISPn �λ���˽��äƥե����ޥåȤ��줿ʸ�����
 *  �֤��ޤ���<br>
 *  TDISPn �λ��̵꤬����硤�¿����Υ����� TZEROn��TSCALn �����ꤵ��Ƥ���
 *  �������Υ����Ǥϡ��ʲ���printf()�ؿ��Υե����ޥåȤ˽��ä��Ѵ�����ʸ����
 *  ���֤��ޤ���<br>
 *  FITS::FLOAT_T                    ... "%.7G" <br>
 *  FITS::DOUBLE_T, FITS::LONGLONG_T ... "%.15G" <br>
 *  FITS::LONG_T                     ... "%.10G" <br>
 *  �嵭�ʳ��Υ���ॿ����           ... "%.7G" <br>
 *  �����������ơ��֥�ξ�� <br>
 *  TFORMn ������ɽ���򼨤��ʤ����ˤϡ������ʸ����� TFORMn �ǥե����ޥå�
 *  ����ʸ������֤��ޤ�(TFORMn �λ��̵꤬�����ϡ������ʸ����򤽤Τޤ��֤�
 *  �ޤ�)��<br>
 *  TFORMn ������ɽ���򼨤����ˤϡ������ʸ�����¿��ͤ��Ѵ����������ͤ�
 *  TZEROn �� TSCALn ���Ѵ����������ͤ� TFORMn �ǥե����ޥåȤ���ʸ������֤�
 *  �ޤ���<br>
 *
 * @param   row_index �ԥ���ǥå���
 * @param   elem_name ����̾ (TELEMn ��¸�ߤ���̾��)
 * @param   repetition_idx TDIMn ��2�����ܤΥ���ǥå���
 * @param   dest_buf �����ͤγ�Ǽ�� (return)
 * @param   buf_size dest_buf �ΥХåե�������
 * @return  �������: �Хåե�Ĺ����ʬ�ʾ��˥��ԡ��Ǥ���ʸ����
 *                    ('\0'�ϴޤޤʤ�) <br>
 *          �����(���顼): �����������ǥ��ԡ�����ʤ��ä����
 * @note  �Х��ʥ�ơ��֥��ʸ���󷿤Υ����Ǥ� TNULLn �����꤬��ǽ�Ǥ�������
 *        �� SFITSIO ���ȼ���ĥ�Ǥ���
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
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
 * @brief  ������ͤ�ʸ�����ͤǼ��� (���٥롦�桼���Хåե��˼���)
 *
 *  FITS����˴�Ť��Ƽ��Ѵ�������Ԥʤä������ͤ�ʸ�����ͤ��֤��ޤ���<br>
 *  NULL�ͤξ�硤fits_table_col::assign_null_svalue() �ǻ��ꤵ�줿 NULL ʸ����
 *  ���֤���ޤ�(NULLʸ����ν�������ͤ� "NULL" �Ǥ�)���Х��ʥ�ơ��֥����
 *  ���������䥢�������ơ��֥�� TNULLn ���ͤˡ�������ͤ����פ�������
 *  NULL�ͤȤ��ޤ���<br>
 *  ���Х��ʥ�ơ��֥�ξ�� <br>
 *  ��������¿����Υ����ξ�硤������ͤ�إå��� TZEROn �� TSCALn ���ͤ�
 *  �Ѵ����������ʸ������Ѵ������ͤ��֤��ޤ���<br>
 *  TDISPn �λ��꤬������ϡ�TDISPn �λ���˽��äƥե����ޥåȤ��줿ʸ�����
 *  �֤��ޤ���<br>
 *  TDISPn �λ��̵꤬����硤�¿����Υ����� TZEROn��TSCALn �����ꤵ��Ƥ���
 *  �������Υ����Ǥϡ��ʲ���printf()�ؿ��Υե����ޥåȤ˽��ä��Ѵ�����ʸ����
 *  ���֤��ޤ���<br>
 *  FITS::FLOAT_T                    ... "%.7G" <br>
 *  FITS::DOUBLE_T, FITS::LONGLONG_T ... "%.15G" <br>
 *  FITS::LONG_T                     ... "%.10G" <br>
 *  �嵭�ʳ��Υ���ॿ����           ... "%.7G" <br>
 *  �����������ơ��֥�ξ�� <br>
 *  TFORMn ������ɽ���򼨤��ʤ����ˤϡ������ʸ����� TFORMn �ǥե����ޥå�
 *  ����ʸ������֤��ޤ�(TFORMn �λ��̵꤬�����ϡ������ʸ����򤽤Τޤ��֤�
 *  �ޤ�)��<br>
 *  TFORMn ������ɽ���򼨤����ˤϡ������ʸ�����¿��ͤ��Ѵ����������ͤ�
 *  TZEROn �� TSCALn ���Ѵ����������ͤ� TFORMn �ǥե����ޥåȤ���ʸ������֤�
 *  �ޤ���<br>
 *
 * @param   row_index �ԥ���ǥå���
 * @param   elem_index ���ǥ���ǥå��� (TDIMn ��1�����ܤΥ���ǥå���)
 * @param   dest_buf �����ͤγ�Ǽ�� (return)
 * @param   buf_size dest_buf �ΥХåե�������
 * @return  �������: �Хåե�Ĺ����ʬ�ʾ��˥��ԡ��Ǥ���ʸ����
 *                    ('\0'�ϴޤޤʤ�) <br>
 *          �����(���顼): �����������ǥ��ԡ�����ʤ��ä����
 * @note  �Х��ʥ�ơ��֥��ʸ���󷿤Υ����Ǥ� TNULLn �����꤬��ǽ�Ǥ�������
 *        �� SFITSIO ���ȼ���ĥ�Ǥ���
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
 */
ssize_t fits_table_col::get_svalue( long row_index, long elem_index, 
				    char *dest_buf, size_t buf_size ) const
{
    tstring sval;
    this->get_svalue( row_index, elem_index, 0, &sval );
    return sval.getstr(dest_buf,buf_size);
}

/**
 * @brief  ������ͤ�ʸ�����ͤǼ��� (���٥롦�桼���Хåե��˼���)
 *
 *  FITS����˴�Ť��Ƽ��Ѵ�������Ԥʤä������ͤ�ʸ�����ͤ��֤��ޤ���<br>
 *  NULL�ͤξ�硤fits_table_col::assign_null_svalue() �ǻ��ꤵ�줿 NULL ʸ����
 *  ���֤���ޤ�(NULLʸ����ν�������ͤ� "NULL" �Ǥ�)���Х��ʥ�ơ��֥����
 *  ���������䥢�������ơ��֥�� TNULLn ���ͤˡ�������ͤ����פ�������
 *  NULL�ͤȤ��ޤ���<br>
 *  ���Х��ʥ�ơ��֥�ξ�� <br>
 *  ��������¿����Υ����ξ�硤������ͤ�إå��� TZEROn �� TSCALn ���ͤ�
 *  �Ѵ����������ʸ������Ѵ������ͤ��֤��ޤ���<br>
 *  TDISPn �λ��꤬������ϡ�TDISPn �λ���˽��äƥե����ޥåȤ��줿ʸ�����
 *  �֤��ޤ���<br>
 *  TDISPn �λ��̵꤬����硤�¿����Υ����� TZEROn��TSCALn �����ꤵ��Ƥ���
 *  �������Υ����Ǥϡ��ʲ���printf()�ؿ��Υե����ޥåȤ˽��ä��Ѵ�����ʸ����
 *  ���֤��ޤ���<br>
 *  FITS::FLOAT_T                    ... "%.7G" <br>
 *  FITS::DOUBLE_T, FITS::LONGLONG_T ... "%.15G" <br>
 *  FITS::LONG_T                     ... "%.10G" <br>
 *  �嵭�ʳ��Υ���ॿ����           ... "%.7G" <br>
 *  �����������ơ��֥�ξ�� <br>
 *  TFORMn ������ɽ���򼨤��ʤ����ˤϡ������ʸ����� TFORMn �ǥե����ޥå�
 *  ����ʸ������֤��ޤ�(TFORMn �λ��̵꤬�����ϡ������ʸ����򤽤Τޤ��֤�
 *  �ޤ�)��<br>
 *  TFORMn ������ɽ���򼨤����ˤϡ������ʸ�����¿��ͤ��Ѵ����������ͤ�
 *  TZEROn �� TSCALn ���Ѵ����������ͤ� TFORMn �ǥե����ޥåȤ���ʸ������֤�
 *  �ޤ���<br>
 *
 * @param   row_index �ԥ���ǥå���
 * @param   elem_index ���ǥ���ǥå��� (TDIMn ��1�����ܤΥ���ǥå���)
 * @param   repetition_idx TDIMn ��2�����ܤΥ���ǥå���
 * @param   dest_buf �����ͤγ�Ǽ�� (return)
 * @param   buf_size dest_buf �ΥХåե�������
 * @return  �������: �Хåե�Ĺ����ʬ�ʾ��˥��ԡ��Ǥ���ʸ����
 *                    ('\0'�ϴޤޤʤ�) <br>
 *          �����(���顼): �����������ǥ��ԡ�����ʤ��ä����
 * @note  �Х��ʥ�ơ��֥��ʸ���󷿤Υ����Ǥ� TNULLn �����꤬��ǽ�Ǥ�������
 *        �� SFITSIO ���ȼ���ĥ�Ǥ���
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
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
 * @brief  ����������ͤ������ͤǼ��� (���٥�)
 *
 *  �֤��ͤϡ�'T'��'F'��'\0' (NULL��) ��3����Ǥ���<br>
 *  �����η��� FITS::LOGICAL_T (TFORMn �λ���� L ��ޤ�) �ξ��˺�®��
 *  ���������Ǥ��ޤ���
 *
 * @param   row_index �ԥ���ǥå���
 * @return  �������: ���ｪλ��<br>
 *          NULL��: �����������ʾ��
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
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
 * @brief  ����������ͤ������ͤǼ��� (���٥�)
 *
 *  �֤��ͤϡ�'T'��'F'��'\0' (NULL��) ��3����Ǥ���<br>
 *  �����η��� FITS::LOGICAL_T (TFORMn �λ���� L ��ޤ�) �ξ��˺�®��
 *  ���������Ǥ��ޤ���
 *
 * @param   row_index �ԥ���ǥå���
 * @param   elem_name ����̾ (TELEMn ��¸�ߤ���̾��)
 * @param   repetition_idx  TDIMn ��2�����ܤΥ���ǥå���
 * @return  �������: ���ｪλ��<br>
 *          NULL��: �����������ʾ��
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
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
 * @brief  ����������ͤ������ͤǼ��� (���٥�)
 *
 *  �֤��ͤϡ�'T'��'F'��'\0' (NULL��) ��3����Ǥ���<br>
 *  �����η��� FITS::LOGICAL_T (TFORMn �λ���� L ��ޤ�) �ξ��˺�®��
 *  ���������Ǥ��ޤ���
 *
 * @param   row_index �ԥ���ǥå���
 * @param   elem_index ���ǥ���ǥå��� (TDIMn ��1�����ܤΥ���ǥå���)
 * @param   repetition_idx  TDIMn ��2�����ܤΥ���ǥå���
 * @return  �������: ���ｪλ��<br>
 *          NULL��: �����������ʾ��
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
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
 * @brief  ����������ͤ�������(short��)�Ǽ��� (���٥�)
 *
 *  �����������(�إå��� TZEROn �� TSCALn ���ͤ�ȿ�Ǥ��Ƥ��ʤ���)��������
 *  (short��)���֤��ޤ���<br>
 *  �����η��� FITS::SHORT_T (TFORMn �λ���� 'I' ��ޤ�) �ξ��˺�®��
 *  ���������Ǥ��ޤ���<br>
 *  �֤���� NULL�ͤϡ��إå��� TNULLn ���ͤ�¸�ߤ�����Ϥ����͡�¸�ߤ��ʤ�
 *  ���� INDEF_SHORT �Ǥ���
 *
 * @param   row_index �ԥ���ǥå���
 * @return  �������: ���ｪλ��<br>
 *          NULL��: �����������ʾ��
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
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
 * @brief  ����������ͤ�������(short��)�Ǽ��� (���٥�)
 *
 *  �����������(�إå��� TZEROn �� TSCALn ���ͤ�ȿ�Ǥ��Ƥ��ʤ���)��������
 *  (short��)���֤��ޤ���<br>
 *  �����η��� FITS::SHORT_T (TFORMn �λ���� 'I' ��ޤ�) �ξ��˺�®��
 *  ���������Ǥ��ޤ���<br>
 *  �֤���� NULL�ͤϡ��إå��� TNULLn ���ͤ�¸�ߤ�����Ϥ����͡�¸�ߤ��ʤ�
 *  ���� INDEF_SHORT �Ǥ���
 *
 * @param   row_index �ԥ���ǥå���
 * @param   elem_name ����̾ (TELEMn ��¸�ߤ���̾��)
 * @param   repetition_idx  TDIMn ��2�����ܤΥ���ǥå���
 * @return  �������: ���ｪλ��<br>
 *          NULL��: �����������ʾ��
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
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
 * @brief  ����������ͤ�������(short��)�Ǽ��� (���٥�)
 *
 *  �����������(�إå��� TZEROn �� TSCALn ���ͤ�ȿ�Ǥ��Ƥ��ʤ���)��������
 *  (short��)���֤��ޤ���<br>
 *  �����η��� FITS::SHORT_T (TFORMn �λ���� 'I' ��ޤ�) �ξ��˺�®��
 *  ���������Ǥ��ޤ���<br>
 *  �֤���� NULL�ͤϡ��إå��� TNULLn ���ͤ�¸�ߤ�����Ϥ����͡�¸�ߤ��ʤ�
 *  ���� INDEF_SHORT �Ǥ���
 *
 * @param   row_index �ԥ���ǥå���
 * @param   elem_index ���ǥ���ǥå��� (TDIMn ��1�����ܤΥ���ǥå���)
 * @param   repetition_idx  TDIMn ��2�����ܤΥ���ǥå���
 * @return  �������: ���ｪλ��<br>
 *          NULL��: �����������ʾ��
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
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
 * @brief  ����������ͤ�������(long��)�Ǽ��� (���٥�)
 *
 *  �����������(�إå��� TZEROn �� TSCALn ���ͤ�ȿ�Ǥ��Ƥ��ʤ���)��������
 *  (long��)���֤��ޤ���<br>
 *  �����η��� FITS::LONG_T (TFORMn �λ���� 'J' ��ޤ�) �ξ��˺�®��
 *  ���������Ǥ��ޤ���<br>
 *  �֤���� NULL�ͤϡ��إå��� TNULLn ���ͤ�¸�ߤ�����Ϥ����͡�¸�ߤ��ʤ�
 *  ���� INDEF_LONG �Ǥ���
 *
 * @param   row_index �ԥ���ǥå���
 * @return  �������: ���ｪλ��<br>
 *          NULL��: �����������ʾ��
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
 * @note  long���� 32-bit OS �Ǥ� 32-bit��64-bit OS �Ǥ� 64-bit ��������Ļ���
 *        �ۤȤ�ɤǤ���
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
 * @brief  ����������ͤ�������(long��)�Ǽ��� (���٥�)
 *
 *  �����������(�إå��� TZEROn �� TSCALn ���ͤ�ȿ�Ǥ��Ƥ��ʤ���)��������
 *  (long��)���֤��ޤ���<br>
 *  �����η��� FITS::LONG_T (TFORMn �λ���� 'J' ��ޤ�) �ξ��˺�®��
 *  ���������Ǥ��ޤ���<br>
 *  �֤���� NULL�ͤϡ��إå��� TNULLn ���ͤ�¸�ߤ�����Ϥ����͡�¸�ߤ��ʤ�
 *  ���� INDEF_LONG �Ǥ���
 *
 * @param   row_index �ԥ���ǥå���
 * @param   elem_name ����̾ (TELEMn ��¸�ߤ���̾��)
 * @param   repetition_idx  TDIMn ��2�����ܤΥ���ǥå���
 * @return  �������: ���ｪλ��<br>
 *          NULL��: �����������ʾ��
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
 * @note  long���� 32-bit OS �Ǥ� 32-bit��64-bit OS �Ǥ� 64-bit ��������Ļ���
 *        �ۤȤ�ɤǤ���
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
 * @brief  ����������ͤ�������(long��)�Ǽ��� (���٥�)
 *
 *  �����������(�إå��� TZEROn �� TSCALn ���ͤ�ȿ�Ǥ��Ƥ��ʤ���)��������
 *  (long��)���֤��ޤ���<br>
 *  �����η��� FITS::LONG_T (TFORMn �λ���� 'J' ��ޤ�) �ξ��˺�®��
 *  ���������Ǥ��ޤ���<br>
 *  �֤���� NULL�ͤϡ��إå��� TNULLn ���ͤ�¸�ߤ�����Ϥ����͡�¸�ߤ��ʤ�
 *  ���� INDEF_LONG �Ǥ���
 *
 * @param   row_index �ԥ���ǥå���
 * @param   elem_index ���ǥ���ǥå��� (TDIMn ��1�����ܤΥ���ǥå���)
 * @param   repetition_idx  TDIMn ��2�����ܤΥ���ǥå���
 * @return  �������: ���ｪλ��<br>
 *          NULL��: �����������ʾ��
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
 * @note  long���� 32-bit OS �Ǥ� 32-bit��64-bit OS �Ǥ� 64-bit ��������Ļ���
 *        �ۤȤ�ɤǤ���
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
 * @brief  ����������ͤ�������(long long��)�Ǽ��� (���٥�)
 *
 *  �����������(�إå��� TZEROn �� TSCALn ���ͤ�ȿ�Ǥ��Ƥ��ʤ���)��������
 *  (long long��)���֤��ޤ���<br>
 *  �����η��� FITS::LONGLONG_T (TFORMn �λ���� 'K' ��ޤ�) �ξ��˺�®��
 *  ���������Ǥ��ޤ���<br>
 *  �֤���� NULL�ͤϡ��إå��� TNULLn ���ͤ�¸�ߤ�����Ϥ����͡�¸�ߤ��ʤ�
 *  ���� INDEF_LLONG �Ǥ���
 *
 * @param   row_index �ԥ���ǥå���
 * @return  �������: ���ｪλ��<br>
 *          NULL��: �����������ʾ��
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
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
 * @brief  ����������ͤ�������(long long��)�Ǽ��� (���٥�)
 *
 *  �����������(�إå��� TZEROn �� TSCALn ���ͤ�ȿ�Ǥ��Ƥ��ʤ���)��������
 *  (long long��)���֤��ޤ���<br>
 *  �����η��� FITS::LONGLONG_T (TFORMn �λ���� 'K' ��ޤ�) �ξ��˺�®��
 *  ���������Ǥ��ޤ���<br>
 *  �֤���� NULL�ͤϡ��إå��� TNULLn ���ͤ�¸�ߤ�����Ϥ����͡�¸�ߤ��ʤ�
 *  ���� INDEF_LLONG �Ǥ���
 *
 * @param   row_index �ԥ���ǥå���
 * @param   elem_name ����̾ (TELEMn ��¸�ߤ���̾��)
 * @param   repetition_idx  TDIMn ��2�����ܤΥ���ǥå���
 * @return  �������: ���ｪλ��<br>
 *          NULL��: �����������ʾ��
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
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
 * @brief  ����������ͤ�������(long long��)�Ǽ��� (���٥�)
 *
 *  �����������(�إå��� TZEROn �� TSCALn ���ͤ�ȿ�Ǥ��Ƥ��ʤ���)��������
 *  (long long��)���֤��ޤ���<br>
 *  �����η��� FITS::LONGLONG_T (TFORMn �λ���� 'K' ��ޤ�) �ξ��˺�®��
 *  ���������Ǥ��ޤ���<br>
 *  �֤���� NULL�ͤϡ��إå��� TNULLn ���ͤ�¸�ߤ�����Ϥ����͡�¸�ߤ��ʤ�
 *  ���� INDEF_LLONG �Ǥ���
 *
 * @param   row_index �ԥ���ǥå���
 * @param   elem_index ���ǥ���ǥå��� (TDIMn ��1�����ܤΥ���ǥå���)
 * @param   repetition_idx  TDIMn ��2�����ܤΥ���ǥå���
 * @return  �������: ���ｪλ��<br>
 *          NULL��: �����������ʾ��
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
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
 * @brief  ����������ͤ�¿���(float��)�Ǽ��� (���٥�)
 *
 *  �����������(�إå��� TZEROn �� TSCALn ���ͤ�ȿ�Ǥ��Ƥ��ʤ���)��¿���
 *  (float��)���֤��ޤ���<br>
 *  �����η��� FITS::FLOAT_T (TFORMn �λ���� 'E' ��ޤ�) �ξ��˺�®��
 *  ���������Ǥ��ޤ���<br>
 *  NULL�ͤξ��ϡ�NAN ���֤��ޤ���
 *
 * @param   row_index �ԥ���ǥå���
 * @return  �������: ���ｪλ��<br>
 *          NAN: �����������ʾ��
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
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
 * @brief  ����������ͤ�¿���(float��)�Ǽ��� (���٥�)
 *
 *  �����������(�إå��� TZEROn �� TSCALn ���ͤ�ȿ�Ǥ��Ƥ��ʤ���)��¿���
 *  (float��)���֤��ޤ���<br>
 *  �����η��� FITS::FLOAT_T (TFORMn �λ���� 'E' ��ޤ�) �ξ��˺�®��
 *  ���������Ǥ��ޤ���<br>
 *  NULL�ͤξ��ϡ�NAN ���֤��ޤ���
 *
 * @param   row_index �ԥ���ǥå���
 * @param   elem_name ����̾ (TELEMn ��¸�ߤ���̾��)
 * @param   repetition_idx  TDIMn ��2�����ܤΥ���ǥå���
 * @return  �������: ���ｪλ��<br>
 *          NAN: �����������ʾ��
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
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
 * @brief  ����������ͤ�¿���(float��)�Ǽ��� (���٥�)
 *
 *  �����������(�إå��� TZEROn �� TSCALn ���ͤ�ȿ�Ǥ��Ƥ��ʤ���)��¿���
 *  (float��)���֤��ޤ���<br>
 *  �����η��� FITS::FLOAT_T (TFORMn �λ���� 'E' ��ޤ�) �ξ��˺�®��
 *  ���������Ǥ��ޤ���<br>
 *  NULL�ͤξ��ϡ�NAN ���֤��ޤ���
 *
 * @param   row_index �ԥ���ǥå���
 * @param   elem_index ���ǥ���ǥå��� (TDIMn ��1�����ܤΥ���ǥå���)
 * @param   repetition_idx  TDIMn ��2�����ܤΥ���ǥå���
 * @return  �������: ���ｪλ��<br>
 *          NAN: �����������ʾ��
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
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
 * @brief  ����������ͤ�¿���(double��)�Ǽ��� (���٥�)
 *
 *  �����������(�إå��� TZEROn �� TSCALn ���ͤ�ȿ�Ǥ��Ƥ��ʤ���)��¿���
 *  (double��)���֤��ޤ���<br>
 *  �����η��� FITS::DOUBLE_T (TFORMn �λ���� 'D' ��ޤ�) �ξ��˺�®��
 *  ���������Ǥ��ޤ���<br>
 *  NULL�ͤξ��ϡ�NAN ���֤��ޤ���
 *
 * @param   row_index �ԥ���ǥå���
 * @return  �������: ���ｪλ��<br>
 *          NAN: �����������ʾ��
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
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
 * @brief  ����������ͤ�¿���(double��)�Ǽ��� (���٥�)
 *
 *  �����������(�إå��� TZEROn �� TSCALn ���ͤ�ȿ�Ǥ��Ƥ��ʤ���)��¿���
 *  (double��)���֤��ޤ���<br>
 *  �����η��� FITS::DOUBLE_T (TFORMn �λ���� 'D' ��ޤ�) �ξ��˺�®��
 *  ���������Ǥ��ޤ���<br>
 *  NULL�ͤξ��ϡ�NAN ���֤��ޤ���
 *
 * @param   row_index �ԥ���ǥå���
 * @param   elem_name ����̾ (TELEMn ��¸�ߤ���̾��)
 * @param   repetition_idx  TDIMn ��2�����ܤΥ���ǥå���
 * @return  �������: ���ｪλ��<br>
 *          NAN: �����������ʾ��
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
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
 * @brief  ����������ͤ�¿���(double��)�Ǽ��� (���٥�)
 *
 *  �����������(�إå��� TZEROn �� TSCALn ���ͤ�ȿ�Ǥ��Ƥ��ʤ���)��¿���
 *  (double��)���֤��ޤ���<br>
 *  �����η��� FITS::DOUBLE_T (TFORMn �λ���� 'D' ��ޤ�) �ξ��˺�®��
 *  ���������Ǥ��ޤ���<br>
 *  NULL�ͤξ��ϡ�NAN ���֤��ޤ���
 *
 * @param   row_index �ԥ���ǥå���
 * @param   elem_index ���ǥ���ǥå��� (TDIMn ��1�����ܤΥ���ǥå���)
 * @param   repetition_idx  TDIMn ��2�����ܤΥ���ǥå���
 * @return  �������: ���ｪλ��<br>
 *          NAN: �����������ʾ��
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
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
 * @brief  ����������ͤ�������(byte��)�Ǽ��� (���٥�)
 *
 *  �����������(�إå��� TZEROn �� TSCALn ���ͤ�ȿ�Ǥ��Ƥ��ʤ���)��������
 *  (unsigned char��)���֤��ޤ���<br>
 *  �����η��� FITS::BYTE_T (TFORMn �λ���� 'B' ��ޤ�) �ξ��˺�®��
 *  ���������Ǥ��ޤ���<br>
 *  �֤���� NULL�ͤϡ��إå��� TNULLn ���ͤ�¸�ߤ�����Ϥ����͡�¸�ߤ��ʤ�
 *  ���� INDEF_UCHAR �Ǥ���
 *
 * @param   row_index �ԥ���ǥå���
 * @return  �������: ���ｪλ��<br>
 *          NULL��: �����������ʾ��
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
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
 * @brief  ����������ͤ�������(byte��)�Ǽ��� (���٥�)
 *
 *  �����������(�إå��� TZEROn �� TSCALn ���ͤ�ȿ�Ǥ��Ƥ��ʤ���)��������
 *  (unsigned char��)���֤��ޤ���<br>
 *  �����η��� FITS::BYTE_T (TFORMn �λ���� 'B' ��ޤ�) �ξ��˺�®��
 *  ���������Ǥ��ޤ���<br>
 *  �֤���� NULL�ͤϡ��إå��� TNULLn ���ͤ�¸�ߤ�����Ϥ����͡�¸�ߤ��ʤ�
 *  ���� INDEF_UCHAR �Ǥ���
 *
 * @param   row_index �ԥ���ǥå���
 * @param   elem_name ����̾ (TELEMn ��¸�ߤ���̾��)
 * @param   repetition_idx  TDIMn ��2�����ܤΥ���ǥå���
 * @return  �������: ���ｪλ��<br>
 *          NULL��: �����������ʾ��
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
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
 * @brief  ����������ͤ�������(byte��)�Ǽ��� (���٥�)
 *
 *  �����������(�إå��� TZEROn �� TSCALn ���ͤ�ȿ�Ǥ��Ƥ��ʤ���)��������
 *  (unsigned char��)���֤��ޤ���<br>
 *  �����η��� FITS::BYTE_T (TFORMn �λ���� 'B' ��ޤ�) �ξ��˺�®��
 *  ���������Ǥ��ޤ���<br>
 *  �֤���� NULL�ͤϡ��إå��� TNULLn ���ͤ�¸�ߤ�����Ϥ����͡�¸�ߤ��ʤ�
 *  ���� INDEF_UCHAR �Ǥ���
 *
 * @param   row_index �ԥ���ǥå���
 * @param   elem_index ���ǥ���ǥå��� (TDIMn ��1�����ܤΥ���ǥå���)
 * @param   repetition_idx  TDIMn ��2�����ܤΥ���ǥå���
 * @return  �������: ���ｪλ��<br>
 *          NULL��: �����������ʾ��
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
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
 * @brief  ����������ͤ�������(bit��)���֤� (���٥�)
 *
 *  ����������ͤ�������(bit��)���֤��ޤ���<br>
 *  �����η��� FITS::BIT_T (TFORMn �λ���� 'X' ��ޤ�) �ξ��˺�®��
 *  ���������Ǥ�������ΥХ������Ǥκ�ü�ΥӥåȤ��ͤ��֤��ޤ���<br>
 *  �����η��� FITS::BIT_T �ʳ��ξ����֤���� NULL�ͤϡ��إå��� TNULLn ��
 *  �ͤ�¸�ߤ�����Ϥ����͡�¸�ߤ��ʤ����� INDEF_LONG �Ǥ���
 *
 * @param   row_index �ԥ���ǥå���
 * @return  �������: ���ｪλ��<br>
 *          NULL��: �����������ʾ��
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
 */
long long fits_table_col::bit_value( long row_index ) const
{
    if ( row_index < 0 || this->row_size_rec <= row_index ) goto invalid;

    if ( this->type_rec == FITS::BIT_T ) {
	/* MSB�򸫤�� */
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
 * @brief  ����������ͤ�������(bit��)���֤� (���٥�)
 *
 *  ����������ͤ�������(bit��)���֤��ޤ���<br>
 *  �����η��� FITS::BIT_T (TFORMn �λ���� 'X' ��ޤ�) �ξ��˻��Ѥ���
 *  ���дؿ��ǡ�Ǥ�դΥӥå����Ǥ��鱦������Ǥ�դΥӥåȿ��������ͤ���Ф�
 *  �ޤ���<br>
 *  �����η��� FITS::BIT_T �ʳ��ξ����֤���� NULL�ͤϡ��إå��� TNULLn ��
 *  �ͤ�¸�ߤ�����Ϥ����͡�¸�ߤ��ʤ����� INDEF_LONG �Ǥ���
 *
 * @param   row_index �ԥ���ǥå���
 * @param   elem_name ����̾ (TELEMn ��¸�ߤ���̾��)
 * @param   repetition_idx  TDIMn ��2�����ܤΥ���ǥå���
 * @param   nbit �ӥå��� (��ά���� TELEMn ���ͤΥӥåȥե�����ɻ����Ŭ��)
 * @return  �������: ���ｪλ��<br>
 *          NULL��: �����������ʾ��
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
 * @note  �ӥåȱ黻��Ԥʤ����ᡤ�¹ԥ����Ȥ��礭�����⤷��ޤ���
 */
long long fits_table_col::bit_value( long row_index, const char *elem_name,
				     long repetition_idx, int nbit ) const
{
    long elem_index = this->elem_index_rec.index(elem_name,0);
    return this->bit_value( row_index, elem_index, repetition_idx, nbit );
}

/**
 * @brief  ����������ͤ�������(bit��)���֤� (���٥�)
 *
 *  ����������ͤ�������(bit��)���֤��ޤ���<br>
 *  �����η��� FITS::BIT_T (TFORMn �λ���� 'X' ��ޤ�) �ξ��˻��Ѥ���
 *  ���дؿ��ǡ�Ǥ�դΥӥå����Ǥ��鱦������Ǥ�դΥӥåȿ��������ͤ���Ф�
 *  �ޤ���<br>
 *  �����η��� FITS::BIT_T �ʳ��ξ����֤���� NULL�ͤϡ��إå��� TNULLn ��
 *  �ͤ�¸�ߤ�����Ϥ����͡�¸�ߤ��ʤ����� INDEF_LONG �Ǥ���
 *
 * @param   row_index �ԥ���ǥå���
 * @param   elem_index ���ǥ���ǥå��� (TDIMn ��1�����ܤΥ���ǥå���)
 * @param   repetition_idx  TDIMn ��2�����ܤΥ���ǥå���
 * @param   nbit �ӥå��� (��ά���� 1 ��Ŭ��)
 * @return  �������: ���ｪλ��<br>
 *          NULL��: �����������ʾ��
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
 * @note  �ӥåȱ黻��Ԥʤ����ᡤ�¹ԥ����Ȥ��礭�����⤷��ޤ���
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
	if ( e_mod + e_align < 8 ) {		/* case A : ���إ��եȤ��� */
	    int n_shift = 7 - (e_mod + e_align);	/* ���եȤ���bit�� */
	    unsigned char mask;
	    i = n_bit;					/* ̤��Ͽbit�� */
	    mask = (unsigned char)0xff >> e_mod;
	    ret_val = (d_ptr[e_div] & mask) >> n_shift;	i -= 1 + e_align;
	    while ( 0 < i ) {
		ret_val <<= 8;
		ret_val |= d_ptr[e_div] << (8-n_shift);	i -= n_shift;
		e_div ++;
		ret_val |= d_ptr[e_div] >> n_shift;	i -= 8 - n_shift;
	    }
	}
	else {					/* case B : ���إ��եȤ��� */
	    int n_shift = e_mod + e_align - 7;		/* ���եȤ���bit�� */
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
 * @brief  ����������ͤ�ʸ�����ͤǼ��� (���٥�)
 *
 *  ����������ͤ�ʸ�����ͤ��֤��ޤ���<br>
 *  �����η��� FITS::ASCII_T (TFORMn �λ���� 'A' ��ޤ�) �ξ��˺�®��
 *  ���������Ǥ��ޤ���<br>
 *  �����η��� FITS::ASCII_T �ξ�硤���뤬��ʸ��������٥��NULL�͡פʤ�
 *  ���Υ��дؿ���NULL�ͤȤ��� "" ���֤��ޤ�(TNULLn �Ȥ���ӤϹԤʤ��ޤ���)��
 *  FITS::ASCII_T �ʳ��ξ�硤���뤬FITS����Ǥ�NULL�ͤǤ���С����Υ��дؿ�
 *  �� NULL�ͤȤ��� "" ���֤��ޤ���
 *
 * @param   row_index �ԥ���ǥå���
 * @return  �������: ���ｪλ��<br>
 *          NULL��: �����������ʾ��
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
 * @note  SFITSIO �Ǥϡ�����෿�� FITS::ASCII_T (TFORMn �λ���� 'A' ��ޤ�) 
 *        �ξ�硤TNULLn �����꤬�ʤ���С���Ƭʸ���� '\0' �Υ���� NULL�ͤȤ�
 *        �ޤ���������ʸ��������٥��NULL�͡פȸƤ�Ǥ��ޤ���
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
 * @brief  ����������ͤ�ʸ�����ͤǼ��� (���٥�)
 *
 *  ����������ͤ�ʸ�����ͤ��֤��ޤ���<br>
 *  �����η��� FITS::ASCII_T (TFORMn �λ���� 'A' ��ޤ�) �ξ��˺�®��
 *  ���������Ǥ��ޤ���<br>
 *  �����η��� FITS::ASCII_T �ξ�硤���뤬��ʸ��������٥��NULL�͡פʤ�
 *  ���Υ��дؿ���NULL�ͤȤ��� "" ���֤��ޤ�(TNULLn �Ȥ���ӤϹԤʤ��ޤ���)��
 *  FITS::ASCII_T �ʳ��ξ�硤���뤬FITS����Ǥ�NULL�ͤǤ���С����Υ��дؿ�
 *  �� NULL�ͤȤ��� "" ���֤��ޤ���
 *
 * @param   row_index �ԥ���ǥå���
 * @param   elem_name ����̾ (TELEMn ��¸�ߤ���̾��)
 * @param   repetition_idx  TDIMn ��2�����ܤΥ���ǥå���
 * @return  �������: ���ｪλ��<br>
 *          NULL��: �����������ʾ��
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
 * @note  SFITSIO �Ǥϡ�����෿�� FITS::ASCII_T (TFORMn �λ���� 'A' ��ޤ�) 
 *        �ξ�硤TNULLn �����꤬�ʤ���С���Ƭʸ���� '\0' �Υ���� NULL�ͤȤ�
 *        �ޤ���������ʸ��������٥��NULL�͡פȸƤ�Ǥ��ޤ���
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
 * @brief  ����������ͤ�ʸ�����ͤǼ��� (���٥�)
 *
 *  ����������ͤ�ʸ�����ͤ��֤��ޤ���<br>
 *  �����η��� FITS::ASCII_T (TFORMn �λ���� 'A' ��ޤ�) �ξ��˺�®��
 *  ���������Ǥ��ޤ���<br>
 *  �����η��� FITS::ASCII_T �ξ�硤���뤬��ʸ��������٥��NULL�͡פʤ�
 *  ���Υ��дؿ���NULL�ͤȤ��� "" ���֤��ޤ�(TNULLn �Ȥ���ӤϹԤʤ��ޤ���)��
 *  FITS::ASCII_T �ʳ��ξ�硤���뤬FITS����Ǥ�NULL�ͤǤ���С����Υ��дؿ�
 *  �� NULL�ͤȤ��� "" ���֤��ޤ���
 *
 * @param   row_index �ԥ���ǥå���
 * @param   elem_index ���ǥ���ǥå��� (TDIMn ��1�����ܤΥ���ǥå���)
 * @param   repetition_idx  TDIMn ��2�����ܤΥ���ǥå���
 * @return  �������: ���ｪλ��<br>
 *          NULL��: �����������ʾ��
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
 * @note  SFITSIO �Ǥϡ�����෿�� FITS::ASCII_T (TFORMn �λ���� 'A' ��ޤ�) 
 *        �ξ�硤TNULLn �����꤬�ʤ���С���Ƭʸ���� '\0' �Υ���� NULL�ͤȤ�
 *        �ޤ���������ʸ��������٥��NULL�͡פȸƤ�Ǥ��ޤ���
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
 * @brief  ����������ͤ�ʸ�����ͤǼ��� (���٥롦tstring�Ǽ���)
 *
 *  ����������ͤ�ʸ�����ͤ��֤��ޤ���<br>
 *  �����η��� FITS::ASCII_T (TFORMn �λ���� 'A' ��ޤ�) �ξ��˺�®��
 *  ���������Ǥ��ޤ���<br>
 *  �����η��� FITS::ASCII_T �ξ�硤���뤬��ʸ��������٥��NULL�͡פʤ�
 *  ���Υ��дؿ���NULL�ͤȤ��� "" ���֤��ޤ�(TNULLn �Ȥ���ӤϹԤʤ��ޤ���)��
 *  FITS::ASCII_T �ʳ��ξ�硤���뤬FITS����Ǥ�NULL�ͤǤ���С����Υ��дؿ�
 *  �� NULL�ͤȤ��� "" ���֤��ޤ���
 *
 * @param   row_index �ԥ���ǥå���
 * @param   dest �����ͤγ�Ǽ�� (return)
 * @return  �������: ���ｪλ��<br>
 *          NULL��: �����������ʾ�硥<br>
 *          NULL: dest �� NULL �ξ��
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
 * @note  SFITSIO �Ǥϡ�����෿�� FITS::ASCII_T (TFORMn �λ���� 'A' ��ޤ�) 
 *        �ξ�硤TNULLn �����꤬�ʤ���С���Ƭʸ���� '\0' �Υ���� NULL�ͤȤ�
 *        �ޤ���������ʸ��������٥��NULL�͡פȸƤ�Ǥ��ޤ���
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
 * @brief  ����������ͤ�ʸ�����ͤǼ��� (���٥롦tstring�Ǽ���)
 *
 *  ����������ͤ�ʸ�����ͤ��֤��ޤ���<br>
 *  �����η��� FITS::ASCII_T (TFORMn �λ���� 'A' ��ޤ�) �ξ��˺�®��
 *  ���������Ǥ��ޤ���<br>
 *  �����η��� FITS::ASCII_T �ξ�硤���뤬��ʸ��������٥��NULL�͡פʤ�
 *  ���Υ��дؿ���NULL�ͤȤ��� "" ���֤��ޤ�(TNULLn �Ȥ���ӤϹԤʤ��ޤ���)��
 *  FITS::ASCII_T �ʳ��ξ�硤���뤬FITS����Ǥ�NULL�ͤǤ���С����Υ��дؿ�
 *  �� NULL�ͤȤ��� "" ���֤��ޤ���
 *
 * @param   row_index �ԥ���ǥå���
 * @param   elem_index ���ǥ���ǥå��� (TDIMn ��1�����ܤΥ���ǥå���)
 * @param   dest �����ͤγ�Ǽ�� (return)
 * @return  �������: ���ｪλ��<br>
 *          NULL��: �����������ʾ�硥<br>
 *          NULL: dest �� NULL �ξ��
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
 * @note  SFITSIO �Ǥϡ�����෿�� FITS::ASCII_T (TFORMn �λ���� 'A' ��ޤ�) 
 *        �ξ�硤TNULLn �����꤬�ʤ���С���Ƭʸ���� '\0' �Υ���� NULL�ͤȤ�
 *        �ޤ���������ʸ��������٥��NULL�͡פȸƤ�Ǥ��ޤ���
 */
const char *fits_table_col::get_string_value( long row_index,
					 long elem_index, tstring *dest ) const
{
    if ( dest == NULL ) return NULL;

    return this->get_string_value( row_index, elem_index, 0, dest );
}

/**
 * @brief  ����������ͤ�ʸ�����ͤǼ��� (���٥롦tstring�Ǽ���)
 *
 *  ����������ͤ�ʸ�����ͤ��֤��ޤ���<br>
 *  �����η��� FITS::ASCII_T (TFORMn �λ���� 'A' ��ޤ�) �ξ��˺�®��
 *  ���������Ǥ��ޤ���<br>
 *  �����η��� FITS::ASCII_T �ξ�硤���뤬��ʸ��������٥��NULL�͡פʤ�
 *  ���Υ��дؿ���NULL�ͤȤ��� "" ���֤��ޤ�(TNULLn �Ȥ���ӤϹԤʤ��ޤ���)��
 *  FITS::ASCII_T �ʳ��ξ�硤���뤬FITS����Ǥ�NULL�ͤǤ���С����Υ��дؿ�
 *  �� NULL�ͤȤ��� "" ���֤��ޤ���
 *
 * @param   row_index �ԥ���ǥå���
 * @param   elem_index ���ǥ���ǥå��� (TDIMn ��1�����ܤΥ���ǥå���)
 * @param   repetition_idx  TDIMn ��2�����ܤΥ���ǥå���
 * @param   dest �����ͤγ�Ǽ�� (return)
 * @return  �������: ���ｪλ��<br>
 *          NULL��: �����������ʾ�硥<br>
 *          NULL: dest �� NULL �ξ��
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
 * @note  SFITSIO �Ǥϡ�����෿�� FITS::ASCII_T (TFORMn �λ���� 'A' ��ޤ�) 
 *        �ξ�硤TNULLn �����꤬�ʤ���С���Ƭʸ���� '\0' �Υ���� NULL�ͤȤ�
 *        �ޤ���������ʸ��������٥��NULL�͡פȸƤ�Ǥ��ޤ���
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
 * @brief  ����������ͤ�ʸ�����ͤǼ��� (���٥롦tstring�Ǽ���)
 *
 *  ����������ͤ�ʸ�����ͤ��֤��ޤ���<br>
 *  �����η��� FITS::ASCII_T (TFORMn �λ���� 'A' ��ޤ�) �ξ��˺�®��
 *  ���������Ǥ��ޤ���<br>
 *  �����η��� FITS::ASCII_T �ξ�硤���뤬��ʸ��������٥��NULL�͡פʤ�
 *  ���Υ��дؿ���NULL�ͤȤ��� "" ���֤��ޤ�(TNULLn �Ȥ���ӤϹԤʤ��ޤ���)��
 *  FITS::ASCII_T �ʳ��ξ�硤���뤬FITS����Ǥ�NULL�ͤǤ���С����Υ��дؿ�
 *  �� NULL�ͤȤ��� "" ���֤��ޤ���
 *
 * @param   row_index �ԥ���ǥå���
 * @param   elem_name ����̾ (TELEMn ��¸�ߤ���̾��)
 * @param   dest �����ͤγ�Ǽ�� (return)
 * @return  �������: ���ｪλ��<br>
 *          NULL��: �����������ʾ�硥<br>
 *          NULL: dest �� NULL �ξ��
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
 * @note  SFITSIO �Ǥϡ�����෿�� FITS::ASCII_T (TFORMn �λ���� 'A' ��ޤ�) 
 *        �ξ�硤TNULLn �����꤬�ʤ���С���Ƭʸ���� '\0' �Υ���� NULL�ͤȤ�
 *        �ޤ���������ʸ��������٥��NULL�͡פȸƤ�Ǥ��ޤ���
 */
const char *fits_table_col::get_string_value( long row_index,
					      const char *elem_name, 
					      tstring *dest ) const
{
    if ( dest == NULL ) return NULL;

    return this->get_string_value( row_index, elem_name, 0, dest );
}

/**
 * @brief  ����������ͤ�ʸ�����ͤǼ��� (���٥롦tstring�Ǽ���)
 *
 *  ����������ͤ�ʸ�����ͤ��֤��ޤ���<br>
 *  �����η��� FITS::ASCII_T (TFORMn �λ���� 'A' ��ޤ�) �ξ��˺�®��
 *  ���������Ǥ��ޤ���<br>
 *  �����η��� FITS::ASCII_T �ξ�硤���뤬��ʸ��������٥��NULL�͡פʤ�
 *  ���Υ��дؿ���NULL�ͤȤ��� "" ���֤��ޤ�(TNULLn �Ȥ���ӤϹԤʤ��ޤ���)��
 *  FITS::ASCII_T �ʳ��ξ�硤���뤬FITS����Ǥ�NULL�ͤǤ���С����Υ��дؿ�
 *  �� NULL�ͤȤ��� "" ���֤��ޤ���
 *
 * @param   row_index �ԥ���ǥå���
 * @param   elem_name ����̾ (TELEMn ��¸�ߤ���̾��)
 * @param   repetition_idx  TDIMn ��2�����ܤΥ���ǥå���
 * @param   dest �����ͤγ�Ǽ�� (return)
 * @return  �������: ���ｪλ��<br>
 *          NULL��: �����������ʾ�硥<br>
 *          NULL: dest �� NULL �ξ��
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
 * @note  SFITSIO �Ǥϡ�����෿�� FITS::ASCII_T (TFORMn �λ���� 'A' ��ޤ�) 
 *        �ξ�硤TNULLn �����꤬�ʤ���С���Ƭʸ���� '\0' �Υ���� NULL�ͤȤ�
 *        �ޤ���������ʸ��������٥��NULL�͡פȸƤ�Ǥ��ޤ���
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
 * @deprecated  ��侩��<br>
 *              fits_table_col::get_string_value( long, tstring * ) <br>
 *              �򤪻Ȥ�����������
 */
const char *fits_table_col::get_string_value( long row_index, 
					      tstring &dest ) const
{
    return this->get_string_value(row_index, &dest);
}

/**
 * @deprecated  ��侩��<br>
 *              fits_table_col::get_string_value( long, long, tstring * ) <br>
 *              �򤪻Ȥ�����������
 */
const char *fits_table_col::get_string_value( long row_index,
					 long elem_index, tstring &dest ) const
{
    return this->get_string_value(row_index, elem_index, 0, &dest);
}

/**
 * @deprecated  ��侩��<br>
 *         fits_table_col::get_string_value( long, long, long, tstring * ) <br>
 *         �򤪻Ȥ�����������
 */
const char *fits_table_col::get_string_value( long row_index,
		    long elem_index, long repetition_idx, tstring &dest ) const
{
    return 
	this->get_string_value(row_index, elem_index, repetition_idx, &dest);
}

/**
 * @deprecated  ��侩��<br>
 *       fits_table_col::get_string_value( long, const char *, tstring * ) <br>
 *       �򤪻Ȥ�����������
 */
const char *fits_table_col::get_string_value( long row_index,
					      const char *elem_name, 
					      tstring &dest ) const
{
    return this->get_string_value(row_index, elem_name, 0, &dest);
}

/**
 * @deprecated  ��侩��<br>
 * fits_table_col::get_string_value( long, const char *, long, tstring * ) <br>
 * �򤪻Ȥ�����������
 */
const char *fits_table_col::get_string_value( long row_index,
					      const char *elem_name, 
					      long repetition_idx,
					      tstring &dest ) const
{
    return this->get_string_value(row_index, elem_name, repetition_idx, &dest);
}

/**
 * @brief  ����������ͤ�ʸ�����ͤǼ��� (���٥롦�桼���Хåե��˼���)
 *
 *  ����������ͤ�ʸ�����ͤ��֤��ޤ���<br>
 *  �����η��� FITS::ASCII_T (TFORMn �λ���� 'A' ��ޤ�) �ξ��˺�®��
 *  ���������Ǥ��ޤ���<br>
 *  �����η��� FITS::ASCII_T �ʳ��ξ����֤���� NULL�ͤϡ�"" �Ǥ���
 *
 * @param   row_index �ԥ���ǥå���
 * @param   dest_buf �����ͤγ�Ǽ�� (return)
 * @param   buf_size dest_buf �ΥХåե�������
 * @return  �������: �Хåե�Ĺ����ʬ�ʾ��˥��ԡ��Ǥ���ʸ����
 *                    ('\0'�ϴޤޤʤ�) <br>
 *          �����(���顼): �����������ǥ��ԡ�����ʤ��ä����
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
 * @note  SFITSIO �Ǥϡ�����෿�� FITS::ASCII_T (TFORMn �λ���� 'A' ��ޤ�) 
 *        �ξ�硤TNULLn �����꤬�ʤ���С���Ƭʸ���� '\0' �Υ���� NULL�ͤȤ�
 *        �ޤ���������ʸ��������٥��NULL�͡פȸƤ�Ǥ��ޤ���
 */
ssize_t fits_table_col::get_string_value( long row_index, 
					char *dest_buf, size_t buf_size ) const
{
    tstring sval;
    this->get_string_value( row_index, &sval );
    return sval.getstr(dest_buf,buf_size);
}

/**
 * @brief  ����������ͤ�ʸ�����ͤǼ��� (���٥롦�桼���Хåե��˼���)
 *
 *  ����������ͤ�ʸ�����ͤ��֤��ޤ���<br>
 *  �����η��� FITS::ASCII_T (TFORMn �λ���� 'A' ��ޤ�) �ξ��˺�®��
 *  ���������Ǥ��ޤ���<br>
 *  �����η��� FITS::ASCII_T �ξ�硤���뤬��ʸ��������٥��NULL�͡פʤ�
 *  ���Υ��дؿ���NULL�ͤȤ��� "" ���֤��ޤ�(TNULLn �Ȥ���ӤϹԤʤ��ޤ���)��
 *  FITS::ASCII_T �ʳ��ξ�硤���뤬FITS����Ǥ�NULL�ͤǤ���С����Υ��дؿ�
 *  �� NULL�ͤȤ��� "" ���֤��ޤ���
 *
 * @param   row_index �ԥ���ǥå���
 * @param   elem_name ����̾ (TELEMn ��¸�ߤ���̾��)
 * @param   dest_buf �����ͤγ�Ǽ�� (return)
 * @param   buf_size dest_buf �ΥХåե�������
 * @return  �������: �Хåե�Ĺ����ʬ�ʾ��˥��ԡ��Ǥ���ʸ����
 *                    ('\0'�ϴޤޤʤ�) <br>
 *          �����(���顼): �����������ǥ��ԡ�����ʤ��ä����
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
 * @note  SFITSIO �Ǥϡ�����෿�� FITS::ASCII_T (TFORMn �λ���� 'A' ��ޤ�) 
 *        �ξ�硤TNULLn �����꤬�ʤ���С���Ƭʸ���� '\0' �Υ���� NULL�ͤȤ�
 *        �ޤ���������ʸ��������٥��NULL�͡פȸƤ�Ǥ��ޤ���
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
 * @brief  ����������ͤ�ʸ�����ͤǼ��� (���٥롦�桼���Хåե��˼���)
 *
 *  ����������ͤ�ʸ�����ͤ��֤��ޤ���<br>
 *  �����η��� FITS::ASCII_T (TFORMn �λ���� 'A' ��ޤ�) �ξ��˺�®��
 *  ���������Ǥ��ޤ���<br>
 *  �����η��� FITS::ASCII_T �ξ�硤���뤬��ʸ��������٥��NULL�͡פʤ�
 *  ���Υ��дؿ���NULL�ͤȤ��� "" ���֤��ޤ�(TNULLn �Ȥ���ӤϹԤʤ��ޤ���)��
 *  FITS::ASCII_T �ʳ��ξ�硤���뤬FITS����Ǥ�NULL�ͤǤ���С����Υ��дؿ�
 *  �� NULL�ͤȤ��� "" ���֤��ޤ���
 *
 * @param   row_index �ԥ���ǥå���
 * @param   elem_name ����̾ (TELEMn ��¸�ߤ���̾��)
 * @param   repetition_idx  TDIMn ��2�����ܤΥ���ǥå���
 * @param   dest_buf �����ͤγ�Ǽ�� (return)
 * @param   buf_size dest_buf �ΥХåե�������
 * @return  �������: �Хåե�Ĺ����ʬ�ʾ��˥��ԡ��Ǥ���ʸ����
 *                    ('\0'�ϴޤޤʤ�) <br>
 *          �����(���顼): �����������ǥ��ԡ�����ʤ��ä����
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
 * @note  SFITSIO �Ǥϡ�����෿�� FITS::ASCII_T (TFORMn �λ���� 'A' ��ޤ�) 
 *        �ξ�硤TNULLn �����꤬�ʤ���С���Ƭʸ���� '\0' �Υ���� NULL�ͤȤ�
 *        �ޤ���������ʸ��������٥��NULL�͡פȸƤ�Ǥ��ޤ���
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
 * @brief  ����������ͤ�ʸ�����ͤǼ��� (���٥롦�桼���Хåե��˼���)
 *
 *  ����������ͤ�ʸ�����ͤ��֤��ޤ���<br>
 *  �����η��� FITS::ASCII_T (TFORMn �λ���� 'A' ��ޤ�) �ξ��˺�®��
 *  ���������Ǥ��ޤ���<br>
 *  �����η��� FITS::ASCII_T �ξ�硤���뤬��ʸ��������٥��NULL�͡פʤ�
 *  ���Υ��дؿ���NULL�ͤȤ��� "" ���֤��ޤ�(TNULLn �Ȥ���ӤϹԤʤ��ޤ���)��
 *  FITS::ASCII_T �ʳ��ξ�硤���뤬FITS����Ǥ�NULL�ͤǤ���С����Υ��дؿ�
 *  �� NULL�ͤȤ��� "" ���֤��ޤ���
 *
 * @param   row_index �ԥ���ǥå���
 * @param   elem_index ���ǥ���ǥå��� (TDIMn ��1�����ܤΥ���ǥå���)
 * @param   dest_buf �����ͤγ�Ǽ�� (return)
 * @param   buf_size dest_buf �ΥХåե�������
 * @return  �������: �Хåե�Ĺ����ʬ�ʾ��˥��ԡ��Ǥ���ʸ����
 *                    ('\0'�ϴޤޤʤ�) <br>
 *          �����(���顼): �����������ǥ��ԡ�����ʤ��ä����
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
 * @note  SFITSIO �Ǥϡ�����෿�� FITS::ASCII_T (TFORMn �λ���� 'A' ��ޤ�) 
 *        �ξ�硤TNULLn �����꤬�ʤ���С���Ƭʸ���� '\0' �Υ���� NULL�ͤȤ�
 *        �ޤ���������ʸ��������٥��NULL�͡פȸƤ�Ǥ��ޤ���
 */
ssize_t fits_table_col::get_string_value( long row_index, long elem_index, 
					char *dest_buf, size_t buf_size ) const
{
    tstring sval;
    this->get_string_value( row_index, elem_index, 0, &sval );
    return sval.getstr(dest_buf,buf_size);
}

/**
 * @brief  ����������ͤ�ʸ�����ͤǼ��� (���٥롦�桼���Хåե��˼���)
 *
 *  ����������ͤ�ʸ�����ͤ��֤��ޤ���<br>
 *  �����η��� FITS::ASCII_T (TFORMn �λ���� 'A' ��ޤ�) �ξ��˺�®��
 *  ���������Ǥ��ޤ���<br>
 *  �����η��� FITS::ASCII_T �ξ�硤���뤬��ʸ��������٥��NULL�͡פʤ�
 *  ���Υ��дؿ���NULL�ͤȤ��� "" ���֤��ޤ�(TNULLn �Ȥ���ӤϹԤʤ��ޤ���)��
 *  FITS::ASCII_T �ʳ��ξ�硤���뤬FITS����Ǥ�NULL�ͤǤ���С����Υ��дؿ�
 *  �� NULL�ͤȤ��� "" ���֤��ޤ���
 *
 * @param   row_index �ԥ���ǥå���
 * @param   elem_index ���ǥ���ǥå��� (TDIMn ��1�����ܤΥ���ǥå���)
 * @param   repetition_idx  TDIMn ��2�����ܤΥ���ǥå���
 * @param   dest_buf �����ͤγ�Ǽ�� (return)
 * @param   buf_size dest_buf �ΥХåե�������
 * @return  �������: �Хåե�Ĺ����ʬ�ʾ��˥��ԡ��Ǥ���ʸ����
 *                    ('\0'�ϴޤޤʤ�) <br>
 *          �����(���顼): �����������ǥ��ԡ�����ʤ��ä����
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
 * @note  SFITSIO �Ǥϡ�����෿�� FITS::ASCII_T (TFORMn �λ���� 'A' ��ޤ�) 
 *        �ξ�硤TNULLn �����꤬�ʤ���С���Ƭʸ���� '\0' �Υ���� NULL�ͤȤ�
 *        �ޤ���������ʸ��������٥��NULL�͡פȸƤ�Ǥ��ޤ���
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
 * @brief  �¿���(double��)�ǥ�����ͤ����� (���٥�)
 *
 *  �¿� value ���顤�إå��� TZEROn �� TSCALn ��ȿ�Ǥ����¿��ͤ��ᡤŬ�ڤ�
 *  �Ѵ�(�������Υ����ξ��ϴݤ�)��Ԥʤäƥ�����������ޤ���<br>
 *  value �� NAN ��Ϳ�������ϡ�NULL�ͤ�Ϳ����줿��ΤȤ��ޤ������λ����Х�
 *  �ʥ�ơ��֥�������������䥢�������ơ��֥�ξ��� TNULLn ���ͤ������
 *  �����ͤ򥻥���������ޤ���
 *
 * @param   value ����������
 * @param   row_index �ԥ���ǥå���
 * @return  ���Ȥλ���
 * @note  �Х��ʥ�ơ��֥��ʸ���󷿤Υ����Ǥ� TNULLn �����꤬��ǽ�Ǥ�������
 *        �� SFITSIO ���ȼ���ĥ�Ǥ���
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
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
 * @brief  �¿���(double��)�ǥ�����ͤ����� (���٥�)
 *
 *  �¿� value ���顤�إå��� TZEROn �� TSCALn ��ȿ�Ǥ����¿��ͤ��ᡤŬ�ڤ�
 *  �Ѵ�(�������Υ����ξ��ϴݤ�)��Ԥʤäƥ�����������ޤ���<br>
 *  value �� NAN ��Ϳ�������ϡ�NULL�ͤ�Ϳ����줿��ΤȤ��ޤ������λ����Х�
 *  �ʥ�ơ��֥�������������䥢�������ơ��֥�ξ��� TNULLn ���ͤ������
 *  �����ͤ򥻥���������ޤ���
 *
 * @param   value ����������
 * @param   row_index �ԥ���ǥå���
 * @param   elem_name ����̾ (TELEMn ��¸�ߤ���̾��)
 * @param   repetition_idx  TDIMn ��2�����ܤΥ���ǥå���
 * @return  ���Ȥλ���
 * @note  �Х��ʥ�ơ��֥��ʸ���󷿤Υ����Ǥ� TNULLn �����꤬��ǽ�Ǥ�������
 *        �� SFITSIO ���ȼ���ĥ�Ǥ���
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
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
 * @brief  �¿���(double��)�ǥ�����ͤ����� (���٥�)
 *
 *  �¿� value ���顤�إå��� TZEROn �� TSCALn ��ȿ�Ǥ����¿��ͤ��ᡤŬ�ڤ�
 *  �Ѵ�(�������Υ����ξ��ϴݤ�)��Ԥʤäƥ�����������ޤ���<br>
 *  value �� NAN ��Ϳ�������ϡ�NULL�ͤ�Ϳ����줿��ΤȤ��ޤ������λ����Х�
 *  �ʥ�ơ��֥�������������䥢�������ơ��֥�ξ��� TNULLn ���ͤ������
 *  �����ͤ򥻥���������ޤ���
 *
 * @param   value ����������
 * @param   row_index �ԥ���ǥå���
 * @param   elem_index ���ǥ���ǥå��� (TDIMn ��1�����ܤΥ���ǥå���)
 * @param   repetition_idx  TDIMn ��2�����ܤΥ���ǥå���
 * @return  ���Ȥλ���
 * @note  �Х��ʥ�ơ��֥��ʸ���󷿤Υ����Ǥ� TNULLn �����꤬��ǽ�Ǥ�������
 *        �� SFITSIO ���ȼ���ĥ�Ǥ���
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
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
 * @brief  �¿���(float��)�ǥ�����ͤ����� (���٥�)
 *
 *  �¿� value ���顤�إå��� TZEROn �� TSCALn ��ȿ�Ǥ����¿��ͤ��ᡤŬ�ڤ�
 *  �Ѵ�(�������Υ����ξ��ϴݤ�)��Ԥʤäƥ�����������ޤ���<br>
 *  value �� NAN ��Ϳ�������ϡ�NULL�ͤ�Ϳ����줿��ΤȤ��ޤ������λ����Х�
 *  �ʥ�ơ��֥�������������䥢�������ơ��֥�ξ��� TNULLn ���ͤ������
 *  �����ͤ򥻥���������ޤ���
 *
 * @param   value ����������
 * @param   row_index �ԥ���ǥå���
 * @return  ���Ȥλ���
 * @note  �Х��ʥ�ơ��֥��ʸ���󷿤Υ����Ǥ� TNULLn �����꤬��ǽ�Ǥ�������
 *        �� SFITSIO ���ȼ���ĥ�Ǥ���
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
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
 * @brief  �¿���(float��)�ǥ�����ͤ����� (���٥�)
 *
 *  �¿� value ���顤�إå��� TZEROn �� TSCALn ��ȿ�Ǥ����¿��ͤ��ᡤŬ�ڤ�
 *  �Ѵ�(�������Υ����ξ��ϴݤ�)��Ԥʤäƥ�����������ޤ���<br>
 *  value �� NAN ��Ϳ�������ϡ�NULL�ͤ�Ϳ����줿��ΤȤ��ޤ������λ����Х�
 *  �ʥ�ơ��֥�������������䥢�������ơ��֥�ξ��� TNULLn ���ͤ������
 *  �����ͤ򥻥���������ޤ���
 *
 * @param   value ����������
 * @param   row_index �ԥ���ǥå���
 * @param   elem_name ����̾ (TELEMn ��¸�ߤ���̾��)
 * @param   repetition_idx  TDIMn ��2�����ܤΥ���ǥå���
 * @return  ���Ȥλ���
 * @note  �Х��ʥ�ơ��֥��ʸ���󷿤Υ����Ǥ� TNULLn �����꤬��ǽ�Ǥ�������
 *        �� SFITSIO ���ȼ���ĥ�Ǥ���
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
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
 * @brief  �¿���(double��)�ǥ�����ͤ����� (���٥�)
 *
 *  �¿� value ���顤�إå��� TZEROn �� TSCALn ��ȿ�Ǥ����¿��ͤ��ᡤŬ�ڤ�
 *  �Ѵ�(�������Υ����ξ��ϴݤ�)��Ԥʤäƥ�����������ޤ���<br>
 *  value �� NAN ��Ϳ�������ϡ�NULL�ͤ�Ϳ����줿��ΤȤ��ޤ������λ����Х�
 *  �ʥ�ơ��֥�������������䥢�������ơ��֥�ξ��� TNULLn ���ͤ������
 *  �����ͤ򥻥���������ޤ���
 *
 * @param   value ����������
 * @param   row_index �ԥ���ǥå���
 * @param   elem_index ���ǥ���ǥå��� (TDIMn ��1�����ܤΥ���ǥå���)
 * @param   repetition_idx  TDIMn ��2�����ܤΥ���ǥå���
 * @return  ���Ȥλ���
 * @note  �Х��ʥ�ơ��֥��ʸ���󷿤Υ����Ǥ� TNULLn �����꤬��ǽ�Ǥ�������
 *        �� SFITSIO ���ȼ���ĥ�Ǥ���
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
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
 * @brief  ������(long long��)�ǥ�����ͤ����� (���٥�)
 *
 *  ���� value ���顤�إå��� TZEROn �� TSCALn ��ȿ�Ǥ����¿��ͤ��ᡤŬ�ڤ�
 *  �Ѵ�(�������Υ����ξ��ϴݤ�)��Ԥʤäƥ�����������ޤ���<br>
 *  ���Υ��дؿ��Ǥ� NULL�ͤ�Ϳ������ϤǤ��ޤ���NULL�ͤ�Ϳ������ϡ�
 *  fits_table_col::assign(double, long) ����Ѥ��Ƥ���������
 *
 * @param   value ����������
 * @param   row_index �ԥ���ǥå���
 * @return  ���Ȥλ���
 * @note  �Х��ʥ�ơ��֥��ʸ���󷿤Υ����Ǥ� TNULLn �����꤬��ǽ�Ǥ�������
 *        �� SFITSIO ���ȼ���ĥ�Ǥ���
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
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
 * @brief  ������(long long��)�ǥ�����ͤ����� (���٥�)
 *
 *  ���� value ���顤�إå��� TZEROn �� TSCALn ��ȿ�Ǥ����¿��ͤ��ᡤŬ�ڤ�
 *  �Ѵ�(�������Υ����ξ��ϴݤ�)��Ԥʤäƥ�����������ޤ���<br>
 *  ���Υ��дؿ��Ǥ� NULL�ͤ�Ϳ������ϤǤ��ޤ���NULL�ͤ�Ϳ������ϡ�
 *  fits_table_col::assign(double, long, const char *, long) ����Ѥ��Ƥ���
 *  ������
 *
 * @param   value ����������
 * @param   row_index �ԥ���ǥå���
 * @param   elem_name ����̾ (TELEMn ��¸�ߤ���̾��)
 * @param   repetition_idx  TDIMn ��2�����ܤΥ���ǥå���
 * @return  ���Ȥλ���
 * @note  �Х��ʥ�ơ��֥��ʸ���󷿤Υ����Ǥ� TNULLn �����꤬��ǽ�Ǥ�������
 *        �� SFITSIO ���ȼ���ĥ�Ǥ���
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
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
 * @brief  ������(long long��)�ǥ�����ͤ����� (���٥�)
 *
 *  ���� value ���顤�إå��� TZEROn �� TSCALn ��ȿ�Ǥ����¿��ͤ��ᡤŬ�ڤ�
 *  �Ѵ�(�������Υ����ξ��ϴݤ�)��Ԥʤäƥ�����������ޤ���<br>
 *  ���Υ��дؿ��Ǥ� NULL�ͤ�Ϳ������ϤǤ��ޤ���NULL�ͤ�Ϳ������ϡ�
 *  fits_table_col::assign(double, long, long, long) ����Ѥ��Ƥ���������
 *
 * @param   value ����������
 * @param   row_index �ԥ���ǥå���
 * @param   elem_index ���ǥ���ǥå��� (TDIMn ��1�����ܤΥ���ǥå���)
 * @param   repetition_idx  TDIMn ��2�����ܤΥ���ǥå���
 * @return  ���Ȥλ���
 * @note  �Х��ʥ�ơ��֥��ʸ���󷿤Υ����Ǥ� TNULLn �����꤬��ǽ�Ǥ�������
 *        �� SFITSIO ���ȼ���ĥ�Ǥ���
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
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
 * @brief  ������(long��)�ǥ�����ͤ����� (���٥�)
 *
 *  ���� value ���顤�إå��� TZEROn �� TSCALn ��ȿ�Ǥ����¿��ͤ��ᡤŬ�ڤ�
 *  �Ѵ�(�������Υ����ξ��ϴݤ�)��Ԥʤäƥ�����������ޤ���<br>
 *  ���Υ��дؿ��Ǥ� NULL�ͤ�Ϳ������ϤǤ��ޤ���NULL�ͤ�Ϳ������ϡ�
 *  fits_table_col::assign(double, long) ����Ѥ��Ƥ���������
 *
 * @param   value ����������
 * @param   row_index �ԥ���ǥå���
 * @return  ���Ȥλ���
 * @note  �Х��ʥ�ơ��֥��ʸ���󷿤Υ����Ǥ� TNULLn �����꤬��ǽ�Ǥ�������
 *        �� SFITSIO ���ȼ���ĥ�Ǥ���
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
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
 * @brief  ������(long��)�ǥ�����ͤ����� (���٥�)
 *
 *  ���� value ���顤�إå��� TZEROn �� TSCALn ��ȿ�Ǥ����¿��ͤ��ᡤŬ�ڤ�
 *  �Ѵ�(�������Υ����ξ��ϴݤ�)��Ԥʤäƥ�����������ޤ���<br>
 *  ���Υ��дؿ��Ǥ� NULL�ͤ�Ϳ������ϤǤ��ޤ���NULL�ͤ�Ϳ������ϡ�
 *  fits_table_col::assign(double, long, const char *, long) ����Ѥ��Ƥ���
 *  ������
 *
 * @param   value ����������
 * @param   row_index �ԥ���ǥå���
 * @param   elem_name ����̾ (TELEMn ��¸�ߤ���̾��)
 * @param   repetition_idx  TDIMn ��2�����ܤΥ���ǥå���
 * @return  ���Ȥλ���
 * @note  �Х��ʥ�ơ��֥��ʸ���󷿤Υ����Ǥ� TNULLn �����꤬��ǽ�Ǥ�������
 *        �� SFITSIO ���ȼ���ĥ�Ǥ���
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
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
 * @brief  ������(long��)�ǥ�����ͤ����� (���٥�)
 *
 *  ���� value ���顤�إå��� TZEROn �� TSCALn ��ȿ�Ǥ����¿��ͤ��ᡤŬ�ڤ�
 *  �Ѵ�(�������Υ����ξ��ϴݤ�)��Ԥʤäƥ�����������ޤ���<br>
 *  ���Υ��дؿ��Ǥ� NULL�ͤ�Ϳ������ϤǤ��ޤ���NULL�ͤ�Ϳ������ϡ�
 *  fits_table_col::assign(double, long, long, long) ����Ѥ��Ƥ���������
 *
 * @param   value ����������
 * @param   row_index �ԥ���ǥå���
 * @param   elem_index ���ǥ���ǥå��� (TDIMn ��1�����ܤΥ���ǥå���)
 * @param   repetition_idx  TDIMn ��2�����ܤΥ���ǥå���
 * @return  ���Ȥλ���
 * @note  �Х��ʥ�ơ��֥��ʸ���󷿤Υ����Ǥ� TNULLn �����꤬��ǽ�Ǥ�������
 *        �� SFITSIO ���ȼ���ĥ�Ǥ���
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
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
 * @brief  ������(int��)�ǥ�����ͤ����� (���٥�)
 *
 *  ���� value ���顤�إå��� TZEROn �� TSCALn ��ȿ�Ǥ����¿��ͤ��ᡤŬ�ڤ�
 *  �Ѵ�(�������Υ����ξ��ϴݤ�)��Ԥʤäƥ�����������ޤ���<br>
 *  ���Υ��дؿ��Ǥ� NULL�ͤ�Ϳ������ϤǤ��ޤ���NULL�ͤ�Ϳ������ϡ�
 *  fits_table_col::assign(double, long) ����Ѥ��Ƥ���������
 *
 * @param   value ����������
 * @param   row_index �ԥ���ǥå���
 * @return  ���Ȥλ���
 * @note  �Х��ʥ�ơ��֥��ʸ���󷿤Υ����Ǥ� TNULLn �����꤬��ǽ�Ǥ�������
 *        �� SFITSIO ���ȼ���ĥ�Ǥ���
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
 */
fits_table_col &fits_table_col::assign( int value, long row_index )
{
    return this->assign((long)value, row_index);
}

/**
 * @brief  ������(int��)�ǥ�����ͤ����� (���٥�)
 *
 *  ���� value ���顤�إå��� TZEROn �� TSCALn ��ȿ�Ǥ����¿��ͤ��ᡤŬ�ڤ�
 *  �Ѵ�(�������Υ����ξ��ϴݤ�)��Ԥʤäƥ�����������ޤ���<br>
 *  ���Υ��дؿ��Ǥ� NULL�ͤ�Ϳ������ϤǤ��ޤ���NULL�ͤ�Ϳ������ϡ�
 *  fits_table_col::assign(double, long, const char *, long) ����Ѥ��Ƥ���
 *  ������
 *
 * @param   value ����������
 * @param   row_index �ԥ���ǥå���
 * @param   elem_name ����̾ (TELEMn ��¸�ߤ���̾��)
 * @param   repetition_idx  TDIMn ��2�����ܤΥ���ǥå���
 * @return  ���Ȥλ���
 * @note  �Х��ʥ�ơ��֥��ʸ���󷿤Υ����Ǥ� TNULLn �����꤬��ǽ�Ǥ�������
 *        �� SFITSIO ���ȼ���ĥ�Ǥ���
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
 */
fits_table_col &fits_table_col::assign( int value, long row_index,
				   const char *elem_name, long repetition_idx )
{
    return this->assign((long)value, row_index, 
			elem_name, repetition_idx);
}

/**
 * @brief  ������(int��)�ǥ�����ͤ����� (���٥�)
 *
 *  ���� value ���顤�إå��� TZEROn �� TSCALn ��ȿ�Ǥ����¿��ͤ��ᡤŬ�ڤ�
 *  �Ѵ�(�������Υ����ξ��ϴݤ�)��Ԥʤäƥ�����������ޤ���<br>
 *  ���Υ��дؿ��Ǥ� NULL�ͤ�Ϳ������ϤǤ��ޤ���NULL�ͤ�Ϳ������ϡ�
 *  fits_table_col::assign(double, long, long, long) ����Ѥ��Ƥ���������
 *
 * @param   value ����������
 * @param   row_index �ԥ���ǥå���
 * @param   elem_index ���ǥ���ǥå��� (TDIMn ��1�����ܤΥ���ǥå���)
 * @param   repetition_idx  TDIMn ��2�����ܤΥ���ǥå���
 * @return  ���Ȥλ���
 * @note  �Х��ʥ�ơ��֥��ʸ���󷿤Υ����Ǥ� TNULLn �����꤬��ǽ�Ǥ�������
 *        �� SFITSIO ���ȼ���ĥ�Ǥ���
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
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
 * @brief  ʸ�����ͤǥ�����ͤ����� (���٥�)
 *
 *  ʸ�����ͤ�Ŭ�ڤ��Ѵ�����������������ޤ���<br>
 *  value �� NULLʸ����ξ��ϡ�NULL�ͤ�Ϳ����줿��ΤȤ��ޤ������λ����Х�
 *  �ʥ�ơ��֥�������������䥢�������ơ��֥�ξ��� TNULLn ���ͤ������
 *  �����ͤ򥻥���������ޤ���NULLʸ����ν�������ͤ� "NULL" �ǡ������ͤ�
 *  fits_table_col::assign_null_svalue() ���ѹ���������Ǥ��ޤ���<br>
 *  ��������¿����Υ����ξ�硤ʸ���� value ��¿��ͤ��Ѵ���������˥إå�
 *  �� TZEROn �� TSCALn ���ͤ��Ѵ������¿��ͤ򥻥���������ޤ�(����ब������
 *  �ξ��ϡ��Ǥ�ᤤ��������������ޤ�)��
 *
 * @param   value ����������
 * @param   row_index �ԥ���ǥå���
 * @return  ���Ȥλ���
 * @note  �Х��ʥ�ơ��֥��ʸ���󷿤Υ����Ǥ� TNULLn �����꤬��ǽ�Ǥ�������
 *        �� SFITSIO ���ȼ���ĥ�Ǥ���
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
 * @note  SFITSIO �Ǥϡ�����෿�� FITS::ASCII_T (TFORMn �λ���� 'A' ��ޤ�) 
 *        �ξ�硤TNULLn �����꤬�ʤ���С���Ƭʸ���� '\0' �Υ���� NULL�ͤȤ�
 *        �ޤ���������ʸ��������٥��NULL�͡פȸƤ�Ǥ��ޤ���
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
 * @brief  ʸ�����ͤǥ�����ͤ����� (���٥�)
 *
 *  ʸ�����ͤ�Ŭ�ڤ��Ѵ�����������������ޤ���<br>
 *  value �� NULLʸ����ξ��ϡ�NULL�ͤ�Ϳ����줿��ΤȤ��ޤ������λ����Х�
 *  �ʥ�ơ��֥�������������䥢�������ơ��֥�ξ��� TNULLn ���ͤ������
 *  �����ͤ򥻥���������ޤ���NULLʸ����ν�������ͤ� "NULL" �ǡ������ͤ�
 *  fits_table_col::assign_null_svalue() ���ѹ���������Ǥ��ޤ���<br>
 *  ��������¿����Υ����ξ�硤ʸ���� value ��¿��ͤ��Ѵ���������˥إå�
 *  �� TZEROn �� TSCALn ���ͤ��Ѵ������¿��ͤ򥻥���������ޤ�(����ब������
 *  �ξ��ϡ��Ǥ�ᤤ��������������ޤ�)��
 *
 * @param   value ����������
 * @param   row_index �ԥ���ǥå���
 * @param   elem_name ����̾ (TELEMn ��¸�ߤ���̾��)
 * @param   repetition_idx  TDIMn ��2�����ܤΥ���ǥå���
 * @return  ���Ȥλ���
 * @note  �Х��ʥ�ơ��֥��ʸ���󷿤Υ����Ǥ� TNULLn �����꤬��ǽ�Ǥ�������
 *        �� SFITSIO ���ȼ���ĥ�Ǥ���
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
 * @note  SFITSIO �Ǥϡ�����෿�� FITS::ASCII_T (TFORMn �λ���� 'A' ��ޤ�) 
 *        �ξ�硤TNULLn �����꤬�ʤ���С���Ƭʸ���� '\0' �Υ���� NULL�ͤȤ�
 *        �ޤ���������ʸ��������٥��NULL�͡פȸƤ�Ǥ��ޤ���
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
 * @brief  ʸ�����ͤǥ�����ͤ����� (���٥�)
 *
 *  ʸ�����ͤ�Ŭ�ڤ��Ѵ�����������������ޤ���<br>
 *  value �� NULLʸ����ξ��ϡ�NULL�ͤ�Ϳ����줿��ΤȤ��ޤ������λ����Х�
 *  �ʥ�ơ��֥�������������䥢�������ơ��֥�ξ��� TNULLn ���ͤ������
 *  �����ͤ򥻥���������ޤ���NULLʸ����ν�������ͤ� "NULL" �ǡ������ͤ�
 *  fits_table_col::assign_null_svalue() ���ѹ���������Ǥ��ޤ���<br>
 *  ��������¿����Υ����ξ�硤ʸ���� value ��¿��ͤ��Ѵ���������˥إå�
 *  �� TZEROn �� TSCALn ���ͤ��Ѵ������¿��ͤ򥻥���������ޤ�(����ब������
 *  �ξ��ϡ��Ǥ�ᤤ��������������ޤ�)��
 *
 * @param   value ����������
 * @param   row_index �ԥ���ǥå���
 * @param   elem_index ���ǥ���ǥå��� (TDIMn ��1�����ܤΥ���ǥå���)
 * @param   repetition_idx  TDIMn ��2�����ܤΥ���ǥå���
 * @return  ���Ȥλ���
 * @note  �Х��ʥ�ơ��֥��ʸ���󷿤Υ����Ǥ� TNULLn �����꤬��ǽ�Ǥ�������
 *        �� SFITSIO ���ȼ���ĥ�Ǥ���
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
 * @note  SFITSIO �Ǥϡ�����෿�� FITS::ASCII_T (TFORMn �λ���� 'A' ��ޤ�) 
 *        �ξ�硤TNULLn �����꤬�ʤ���С���Ƭʸ���� '\0' �Υ���� NULL�ͤȤ�
 *        �ޤ���������ʸ��������٥��NULL�͡פȸƤ�Ǥ��ޤ���
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
 * @brief  �����ͤǥ�����ͤ򤽤Τޤ����� (���٥�)
 *
 *  �����ͤǥ�����ͤ򤽤Τޤ��������ޤ���<br>
 *  �����η��� FITS::LOGICAL_T (TFORMn �λ���� 'L' ��ޤ�) �ξ��˺�®��
 *  ���������Ǥ��ޤ���<br>
 *  value �ˤϡ������ͤ򼨤� 'T'�������ͤ򼨤� 'F'��NULL�� �򼨤�����¾����
 *  (�㤨�� 0) ��Ϳ���ޤ���
 *
 * @param   value ����������
 * @param   row_index �ԥ���ǥå���
 * @return  ���Ȥλ���
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
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
 * @brief  �����ͤǥ�����ͤ򤽤Τޤ����� (���٥�)
 *
 *  �����ͤǥ�����ͤ򤽤Τޤ��������ޤ���<br>
 *  �����η��� FITS::LOGICAL_T (TFORMn �λ���� 'L' ��ޤ�) �ξ��˺�®��
 *  ���������Ǥ��ޤ���<br>
 *  value �ˤϡ������ͤ򼨤� 'T'�������ͤ򼨤� 'F'��NULL�� �򼨤�����¾����
 *  (�㤨�� 0) ��Ϳ���ޤ���
 *
 * @param   value ����������
 * @param   row_index �ԥ���ǥå���
 * @param   elem_name ����̾ (TELEMn ��¸�ߤ���̾��)
 * @param   repetition_idx  TDIMn ��2�����ܤΥ���ǥå���
 * @return  ���Ȥλ���
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
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
 * @brief  �����ͤǥ�����ͤ򤽤Τޤ����� (���٥�)
 *
 *  �����ͤǥ�����ͤ򤽤Τޤ��������ޤ���<br>
 *  �����η��� FITS::LOGICAL_T (TFORMn �λ���� 'L' ��ޤ�) �ξ��˺�®��
 *  ���������Ǥ��ޤ���<br>
 *  value �ˤϡ������ͤ򼨤� 'T'�������ͤ򼨤� 'F'��NULL�� �򼨤�����¾����
 *  (�㤨�� 0) ��Ϳ���ޤ���
 *
 * @param   value ����������
 * @param   row_index �ԥ���ǥå���
 * @param   elem_index ���ǥ���ǥå��� (TDIMn ��1�����ܤΥ���ǥå���)
 * @param   repetition_idx  TDIMn ��2�����ܤΥ���ǥå���
 * @return  ���Ȥλ���
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
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
 * @brief  ������(short��)�ǥ�����ͤ򤽤Τޤ����� (���٥�)
 *
 *  ������(short��)�ǥ�����ͤ򤽤Τޤ��������ޤ���TZEROn �� TSCALn ���ͤˤ��
 *  �Ѵ��ϹԤʤ��ޤ���<br>
 *  �����η��� FITS::SHORT_T (TFORMn �λ���� 'I' ��ޤ�) �ξ��˺�®��
 *  ���������Ǥ��ޤ���
 *
 * @param   value ����������
 * @param   row_index �ԥ���ǥå���
 * @return  ���Ȥλ���
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
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
 * @brief  ������(short��)�ǥ�����ͤ򤽤Τޤ����� (���٥�)
 *
 *  ������(short��)�ǥ�����ͤ򤽤Τޤ��������ޤ���TZEROn �� TSCALn ���ͤˤ��
 *  �Ѵ��ϹԤʤ��ޤ���<br>
 *  �����η��� FITS::SHORT_T (TFORMn �λ���� 'I' ��ޤ�) �ξ��˺�®��
 *  ���������Ǥ��ޤ���
 *
 * @param   value ����������
 * @param   row_index �ԥ���ǥå���
 * @param   elem_name ����̾ (TELEMn ��¸�ߤ���̾��)
 * @param   repetition_idx  TDIMn ��2�����ܤΥ���ǥå���
 * @return  ���Ȥλ���
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
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
 * @brief  ������(short��)�ǥ�����ͤ򤽤Τޤ����� (���٥�)
 *
 *  ������(short��)�ǥ�����ͤ򤽤Τޤ��������ޤ���TZEROn �� TSCALn ���ͤˤ��
 *  �Ѵ��ϹԤʤ��ޤ���<br>
 *  �����η��� FITS::SHORT_T (TFORMn �λ���� 'I' ��ޤ�) �ξ��˺�®��
 *  ���������Ǥ��ޤ���
 *
 * @param   value ����������
 * @param   row_index �ԥ���ǥå���
 * @param   elem_index ���ǥ���ǥå��� (TDIMn ��1�����ܤΥ���ǥå���)
 * @param   repetition_idx  TDIMn ��2�����ܤΥ���ǥå���
 * @return  ���Ȥλ���
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
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
 * @brief  ������(long��)�ǥ�����ͤ򤽤Τޤ����� (���٥�)
 *
 *  ������(long��)�ǥ�����ͤ򤽤Τޤ��������ޤ���TZEROn �� TSCALn ���ͤˤ��
 *  �Ѵ��ϹԤʤ��ޤ���<br>
 *  �����η��� FITS::LONG_T (TFORMn �λ���� 'J' ��ޤ�) �ξ��˺�®��
 *  ���������Ǥ��ޤ���
 *
 * @param   value ����������
 * @param   row_index �ԥ���ǥå���
 * @return  ���Ȥλ���
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
 * @note  long���� 32-bit OS �Ǥ� 32-bit��64-bit OS �Ǥ� 64-bit ��������Ļ���
 *        �ۤȤ�ɤǤ���
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
 * @brief  ������(long��)�ǥ�����ͤ򤽤Τޤ����� (���٥�)
 *
 *  ������(long��)�ǥ�����ͤ򤽤Τޤ��������ޤ���TZEROn �� TSCALn ���ͤˤ��
 *  �Ѵ��ϹԤʤ��ޤ���<br>
 *  �����η��� FITS::LONG_T (TFORMn �λ���� 'J' ��ޤ�) �ξ��˺�®��
 *  ���������Ǥ��ޤ���
 *
 * @param   value ����������
 * @param   row_index �ԥ���ǥå���
 * @param   elem_name ����̾ (TELEMn ��¸�ߤ���̾��)
 * @param   repetition_idx  TDIMn ��2�����ܤΥ���ǥå���
 * @return  ���Ȥλ���
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
 * @note  long���� 32-bit OS �Ǥ� 32-bit��64-bit OS �Ǥ� 64-bit ��������Ļ���
 *        �ۤȤ�ɤǤ���
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
 * @brief  ������(long��)�ǥ�����ͤ򤽤Τޤ����� (���٥�)
 *
 *  ������(long��)�ǥ�����ͤ򤽤Τޤ��������ޤ���TZEROn �� TSCALn ���ͤˤ��
 *  �Ѵ��ϹԤʤ��ޤ���<br>
 *  �����η��� FITS::LONG_T (TFORMn �λ���� 'J' ��ޤ�) �ξ��˺�®��
 *  ���������Ǥ��ޤ���
 *
 * @param   value ����������
 * @param   row_index �ԥ���ǥå���
 * @param   elem_index ���ǥ���ǥå��� (TDIMn ��1�����ܤΥ���ǥå���)
 * @param   repetition_idx  TDIMn ��2�����ܤΥ���ǥå���
 * @return  ���Ȥλ���
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
 * @note  long���� 32-bit OS �Ǥ� 32-bit��64-bit OS �Ǥ� 64-bit ��������Ļ���
 *        �ۤȤ�ɤǤ���
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
 * @brief  ������(long long��)�ǥ�����ͤ򤽤Τޤ����� (���٥�)
 *
 *  ������(long long��)�ǥ�����ͤ򤽤Τޤ��������ޤ���TZEROn �� TSCALn ���ͤ�
 *  ����Ѵ��ϹԤʤ��ޤ���<br>
 *  �����η��� FITS::LONGLONG_T (TFORMn �λ���� 'K' ��ޤ�) �ξ��˺�®��
 *  ���������Ǥ��ޤ���
 *
 * @param   value ����������
 * @param   row_index �ԥ���ǥå���
 * @return  ���Ȥλ���
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
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
 * @brief  ������(long long��)�ǥ�����ͤ򤽤Τޤ����� (���٥�)
 *
 *  ������(long long��)�ǥ�����ͤ򤽤Τޤ��������ޤ���TZEROn �� TSCALn ���ͤ�
 *  ����Ѵ��ϹԤʤ��ޤ���<br>
 *  �����η��� FITS::LONGLONG_T (TFORMn �λ���� 'K' ��ޤ�) �ξ��˺�®��
 *  ���������Ǥ��ޤ���
 *
 * @param   value ����������
 * @param   row_index �ԥ���ǥå���
 * @param   elem_name ����̾ (TELEMn ��¸�ߤ���̾��)
 * @param   repetition_idx  TDIMn ��2�����ܤΥ���ǥå���
 * @return  ���Ȥλ���
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
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
 * @brief  ������(long long��)�ǥ�����ͤ򤽤Τޤ����� (���٥�)
 *
 *  ������(long long��)�ǥ�����ͤ򤽤Τޤ��������ޤ���TZEROn �� TSCALn ���ͤ�
 *  ����Ѵ��ϹԤʤ��ޤ���<br>
 *  �����η��� FITS::LONGLONG_T (TFORMn �λ���� 'K' ��ޤ�) �ξ��˺�®��
 *  ���������Ǥ��ޤ���
 *
 * @param   value ����������
 * @param   row_index �ԥ���ǥå���
 * @param   elem_index ���ǥ���ǥå��� (TDIMn ��1�����ܤΥ���ǥå���)
 * @param   repetition_idx  TDIMn ��2�����ܤΥ���ǥå���
 * @return  ���Ȥλ���
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
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
 * @brief  ������(byte��)�ǥ�����ͤ򤽤Τޤ����� (���٥�)
 *
 *  ������(unsigned char��)�ǥ�����ͤ򤽤Τޤ��������ޤ���TZEROn �� TSCALn ��
 *  �ͤˤ���Ѵ��ϹԤʤ��ޤ���<br>
 *  �����η��� FITS::BYTE_T (TFORMn �λ���� 'B' ��ޤ�) �ξ��˺�®��
 *  ���������Ǥ��ޤ���
 *
 * @param   value ����������
 * @param   row_index �ԥ���ǥå���
 * @return  ���Ȥλ���
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
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
 * @brief  ������(byte��)�ǥ�����ͤ򤽤Τޤ����� (���٥�)
 *
 *  ������(unsigned char��)�ǥ�����ͤ򤽤Τޤ��������ޤ���TZEROn �� TSCALn ��
 *  �ͤˤ���Ѵ��ϹԤʤ��ޤ���<br>
 *  �����η��� FITS::BYTE_T (TFORMn �λ���� 'B' ��ޤ�) �ξ��˺�®��
 *  ���������Ǥ��ޤ���
 *
 * @param   value ����������
 * @param   row_index �ԥ���ǥå���
 * @param   elem_name ����̾ (TELEMn ��¸�ߤ���̾��)
 * @param   repetition_idx  TDIMn ��2�����ܤΥ���ǥå���
 * @return  ���Ȥλ���
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
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
 * @brief  ������(byte��)�ǥ�����ͤ򤽤Τޤ����� (���٥�)
 *
 *  ������(unsigned char��)�ǥ�����ͤ򤽤Τޤ��������ޤ���TZEROn �� TSCALn ��
 *  �ͤˤ���Ѵ��ϹԤʤ��ޤ���<br>
 *  �����η��� FITS::BYTE_T (TFORMn �λ���� 'B' ��ޤ�) �ξ��˺�®��
 *  ���������Ǥ��ޤ���
 *
 * @param   value ����������
 * @param   row_index �ԥ���ǥå���
 * @param   elem_index ���ǥ���ǥå��� (TDIMn ��1�����ܤΥ���ǥå���)
 * @param   repetition_idx  TDIMn ��2�����ܤΥ���ǥå���
 * @return  ���Ȥλ���
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
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
 * @brief  ������(float��)�ǥ�����ͤ򤽤Τޤ����� (���٥�)
 *
 *  �¿���(float��)�ǥ�����ͤ򤽤Τޤ��������ޤ���TZEROn �� TSCALn ���ͤˤ��
 *  �Ѵ��ϹԤʤ��ޤ���<br>
 *  �����η��� FITS::FLOAT_T (TFORMn �λ���� 'E' ��ޤ�) �ξ��˺�®��
 *  ���������Ǥ��ޤ���
 *
 * @param   value ����������
 * @param   row_index �ԥ���ǥå���
 * @return  ���Ȥλ���
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
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
 * @brief  ������(float��)�ǥ�����ͤ򤽤Τޤ����� (���٥�)
 *
 *  �¿���(float��)�ǥ�����ͤ򤽤Τޤ��������ޤ���TZEROn �� TSCALn ���ͤˤ��
 *  �Ѵ��ϹԤʤ��ޤ���<br>
 *  �����η��� FITS::FLOAT_T (TFORMn �λ���� 'E' ��ޤ�) �ξ��˺�®��
 *  ���������Ǥ��ޤ���
 *
 * @param   value ����������
 * @param   row_index �ԥ���ǥå���
 * @param   elem_name ����̾ (TELEMn ��¸�ߤ���̾��)
 * @param   repetition_idx  TDIMn ��2�����ܤΥ���ǥå���
 * @return  ���Ȥλ���
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
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
 * @brief  ������(float��)�ǥ�����ͤ򤽤Τޤ����� (���٥�)
 *
 *  �¿���(float��)�ǥ�����ͤ򤽤Τޤ��������ޤ���TZEROn �� TSCALn ���ͤˤ��
 *  �Ѵ��ϹԤʤ��ޤ���<br>
 *  �����η��� FITS::FLOAT_T (TFORMn �λ���� 'E' ��ޤ�) �ξ��˺�®��
 *  ���������Ǥ��ޤ���
 *
 * @param   value ����������
 * @param   row_index �ԥ���ǥå���
 * @param   elem_index ���ǥ���ǥå��� (TDIMn ��1�����ܤΥ���ǥå���)
 * @param   repetition_idx  TDIMn ��2�����ܤΥ���ǥå���
 * @return  ���Ȥλ���
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
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
 * @brief  ������(double��)�ǥ�����ͤ򤽤Τޤ����� (���٥�)
 *
 *  �¿���(double��)�ǥ�����ͤ򤽤Τޤ��������ޤ���TZEROn �� TSCALn ���ͤˤ��
 *  �Ѵ��ϹԤʤ��ޤ���<br>
 *  �����η��� FITS::DOUBLE_T (TFORMn �λ���� 'D' ��ޤ�) �ξ��˺�®��
 *  ���������Ǥ��ޤ���
 *
 * @param   value ����������
 * @param   row_index �ԥ���ǥå���
 * @return  ���Ȥλ���
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
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
 * @brief  ������(double��)�ǥ�����ͤ򤽤Τޤ����� (���٥�)
 *
 *  �¿���(double��)�ǥ�����ͤ򤽤Τޤ��������ޤ���TZEROn �� TSCALn ���ͤˤ��
 *  �Ѵ��ϹԤʤ��ޤ���<br>
 *  �����η��� FITS::DOUBLE_T (TFORMn �λ���� 'D' ��ޤ�) �ξ��˺�®��
 *  ���������Ǥ��ޤ���
 *
 * @param   value ����������
 * @param   row_index �ԥ���ǥå���
 * @param   elem_name ����̾ (TELEMn ��¸�ߤ���̾��)
 * @param   repetition_idx  TDIMn ��2�����ܤΥ���ǥå���
 * @return  ���Ȥλ���
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
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
 * @brief  ������(double��)�ǥ�����ͤ򤽤Τޤ����� (���٥�)
 *
 *  �¿���(double��)�ǥ�����ͤ򤽤Τޤ��������ޤ���TZEROn �� TSCALn ���ͤˤ��
 *  �Ѵ��ϹԤʤ��ޤ���<br>
 *  �����η��� FITS::DOUBLE_T (TFORMn �λ���� 'D' ��ޤ�) �ξ��˺�®��
 *  ���������Ǥ��ޤ���
 *
 * @param   value ����������
 * @param   row_index �ԥ���ǥå���
 * @param   elem_index ���ǥ���ǥå��� (TDIMn ��1�����ܤΥ���ǥå���)
 * @param   repetition_idx  TDIMn ��2�����ܤΥ���ǥå���
 * @return  ���Ȥλ���
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
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
 * @brief  ʸ�����ͤǥ�����ͤ򤽤Τޤ����� (���٥�)
 *
 *  ʸ�����ͤǥ�����ͤ򤽤Τޤ��������ޤ���<br>
 *  �Х��ʥ�ơ��֥�Υ����η��� FITS::ASCII_T (TFORMn �λ���� 'A' ��ޤ�) 
 *  �ξ�硤�ޤ��ϥ��������ơ��֥�ξ��˺�®�ǥ��������Ǥ��ޤ���<br>
 *  �����η��� FITS::ASCII_T �ξ�硤value �� NULL ��Ϳ����ȡ�ʸ��������
 *  �٥��NULL�͡פ򥻥åȤ��ޤ�(TNULLn �Ȥ���ӤϹԤʤ��ޤ���)��FITS::ASCII_T
 *  �ʳ��ξ��ϡ�value �� NULL ��Ϳ����� FITS ����˽���������� NULL�ͤ�
 *  ���åȤ��ޤ���
 *
 * @param   value ���������͡�
 * @param   row_index �ԥ���ǥå���
 * @return  ���Ȥλ���
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
 * @note  �ܺ٤�ư��: ʸ���󷿤Υ����ξ�硤value �� NULL �ξ��ϥ��뤬 '\0'
 *        �������ޤ�����value �� "" �ξ��ϥ��뤬����ʸ���������ޤ���
 * @note  SFITSIO �Ǥϡ�����෿�� FITS::ASCII_T (TFORMn �λ���� 'A' ��ޤ�) 
 *        �ξ�硤TNULLn �����꤬�ʤ���С���Ƭʸ���� '\0' �Υ���� NULL�ͤȤ�
 *        �ޤ���������ʸ��������٥��NULL�͡פȸƤ�Ǥ��ޤ���
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
 * @brief  ʸ�����ͤǥ�����ͤ򤽤Τޤ����� (���٥�)
 *
 *  ʸ�����ͤǥ�����ͤ򤽤Τޤ��������ޤ���<br>
 *  �Х��ʥ�ơ��֥�Υ����η��� FITS::ASCII_T (TFORMn �λ���� 'A' ��ޤ�) 
 *  �ξ�硤�ޤ��ϥ��������ơ��֥�ξ��˺�®�ǥ��������Ǥ��ޤ���<br>
 *  �����η��� FITS::ASCII_T �ξ�硤value �� NULL ��Ϳ����ȡ�ʸ��������
 *  �٥��NULL�͡פ򥻥åȤ��ޤ�(TNULLn �Ȥ���ӤϹԤʤ��ޤ���)��FITS::ASCII_T
 *  �ʳ��ξ��ϡ�value �� NULL ��Ϳ����� FITS ����˽���������� NULL�ͤ�
 *  ���åȤ��ޤ���
 *
 * @param   value ���������͡�
 * @param   row_index �ԥ���ǥå���
 * @param   elem_name ����̾ (TELEMn ��¸�ߤ���̾��)
 * @param   repetition_idx  TDIMn ��2�����ܤΥ���ǥå���
 * @return  ���Ȥλ���
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
 * @note  �ܺ٤�ư��: ʸ���󷿤Υ����ξ�硤value �� NULL �ξ��ϥ��뤬 '\0'
 *        �������ޤ�����value �� "" �ξ��ϥ��뤬����ʸ���������ޤ���
 * @note  SFITSIO �Ǥϡ�����෿�� FITS::ASCII_T (TFORMn �λ���� 'A' ��ޤ�) 
 *        �ξ�硤TNULLn �����꤬�ʤ���С���Ƭʸ���� '\0' �Υ���� NULL�ͤȤ�
 *        �ޤ���������ʸ��������٥��NULL�͡פȸƤ�Ǥ��ޤ���
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
 * @brief  ʸ�����ͤǥ�����ͤ򤽤Τޤ����� (���٥�)
 *
 *  ʸ�����ͤǥ�����ͤ򤽤Τޤ��������ޤ���<br>
 *  �Х��ʥ�ơ��֥�Υ����η��� FITS::ASCII_T (TFORMn �λ���� 'A' ��ޤ�) 
 *  �ξ�硤�ޤ��ϥ��������ơ��֥�ξ��˺�®�ǥ��������Ǥ��ޤ���<br>
 *  �����η��� FITS::ASCII_T �ξ�硤value �� NULL ��Ϳ����ȡ�ʸ��������
 *  �٥��NULL�͡פ򥻥åȤ��ޤ�(TNULLn �Ȥ���ӤϹԤʤ��ޤ���)��FITS::ASCII_T
 *  �ʳ��ξ��ϡ�value �� NULL ��Ϳ����� FITS ����˽���������� NULL�ͤ�
 *  ���åȤ��ޤ���
 *
 * @param   value ���������͡�
 * @param   row_index �ԥ���ǥå���
 * @param   elem_index ���ǥ���ǥå��� (TDIMn ��1�����ܤΥ���ǥå���)
 * @param   repetition_idx  TDIMn ��2�����ܤΥ���ǥå���
 * @return  ���Ȥλ���
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
 * @note  �ܺ٤�ư��: ʸ���󷿤Υ����ξ�硤value �� NULL �ξ��ϥ��뤬 '\0'
 *        �������ޤ�����value �� "" �ξ��ϥ��뤬����ʸ���������ޤ���
 * @note  SFITSIO �Ǥϡ�����෿�� FITS::ASCII_T (TFORMn �λ���� 'A' ��ޤ�) 
 *        �ξ�硤TNULLn �����꤬�ʤ���С���Ƭʸ���� '\0' �Υ���� NULL�ͤȤ�
 *        �ޤ���������ʸ��������٥��NULL�͡פȸƤ�Ǥ��ޤ���
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
 * @brief  ������(bit��)�ǥ�����ͤ򤽤Τޤ����� (���٥�)
 *
 *  ������(bit��)�ǥ�����ͤ򤽤Τޤ��������ޤ���<br>
 *  �����η��� FITS::BIT_T (TFORMn �λ���� 'X' ��ޤ�) �ξ��˺�®��
 *  ���������Ǥ�������ΥХ������Ǥκ�ü�ΥӥåȤ��ͤ򥻥åȤ��ޤ���
 *
 * @param   value ���������͡�
 * @param   row_index �ԥ���ǥå���
 * @return  ���Ȥλ���
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
 */
fits_table_col &fits_table_col::assign_bit( long long value, long row_index )
{
    if ( row_index < 0 || this->row_size_rec <= row_index ) {
	return *this;	/* invalid */
    }

    if ( this->type_rec == FITS::BIT_T ) {
	/* MSB�˽񤯻� */
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
 * @brief  ������(bit��)�ǥ�����ͤ򤽤Τޤ����� (���٥�)
 *
 *  ������(bit��)�ǥ�����ͤ򤽤Τޤ��������ޤ���<br>
 *  �����η��� FITS::BIT_T (TFORMn �λ���� 'X' ��ޤ�) �ξ��˻��Ѥ���
 *  ���дؿ��ǡ�Ǥ�դΥӥå����Ǥ��鱦������Ǥ�դΥӥåȿ��������ͤ򥻥å�
 *  �Ǥ��ޤ���
 *
 * @param   value ���������͡�
 * @param   row_index �ԥ���ǥå���
 * @param   elem_name ����̾ (TELEMn ��¸�ߤ���̾��)
 * @param   repetition_idx  TDIMn ��2�����ܤΥ���ǥå���
 * @param   nbit �ӥå��� (��ά���� TELEMn ���ͤΥӥåȥե�����ɻ����Ŭ��)
 * @return  ���Ȥλ���
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
 * @note  �ӥåȱ黻��Ԥʤ����ᡤ�¹ԥ����Ȥ��礭�����⤷��ޤ���
 */
fits_table_col &fits_table_col::assign_bit( long long value, long row_index,
			 const char *elem_name, long repetition_idx, int nbit )
{
    long elem_index = this->elem_index_rec.index(elem_name,0);
    return this->assign_bit( value, 
			     row_index, elem_index, repetition_idx, nbit );
}

/**
 * @brief  ������(bit��)�ǥ�����ͤ򤽤Τޤ����� (���٥�)
 *
 *  ������(bit��)�ǥ�����ͤ򤽤Τޤ��������ޤ���<br>
 *  �����η��� FITS::BIT_T (TFORMn �λ���� 'X' ��ޤ�) �ξ��˻��Ѥ���
 *  ���дؿ��ǡ�Ǥ�դΥӥå����Ǥ��鱦������Ǥ�դΥӥåȿ��������ͤ򥻥å�
 *  �Ǥ��ޤ���
 *
 * @param   value ���������͡�
 * @param   row_index �ԥ���ǥå���
 * @param   elem_index ���ǥ���ǥå��� (TDIMn ��1�����ܤΥ���ǥå���)
 * @param   repetition_idx  TDIMn ��2�����ܤΥ���ǥå���
 * @param   nbit �ӥå��� (��ά���� 1 ��Ŭ��)
 * @return  ���Ȥλ���
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
 * @note  �ӥåȱ黻��Ԥʤ����ᡤ�¹ԥ����Ȥ��礭�����⤷��ޤ���
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
	if ( e_mod + e_align < 8 ) {		/* case A : ���إ��եȤ��� */
	    int n_shift = 7 - (e_mod + e_align);	/* ���եȤ���bit�� */
	    unsigned char mask;
	    int sft;
	    i = n_bit;					/* ̤����bit�� */
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
	else {					/* case B : ���إ��եȤ��� */
	    int n_shift = e_mod + e_align - 7;		/* ���եȤ���bit�� */
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
 * @brief  ����Ĺ��������󵭽һҤ򥻥å� (���٥�)
 *
 *  elem_idx �ϡ�����Ĺ��������󵭽һҤ�ʣ��¸�ߤ�����˻��ꤷ�ޤ���
 *  �㤨�� TFORM ������� '4PE(999)' �ξ�� elem_idx �� 0 �� 3 ���ͤ�Ȥ����
 *  �Ǥ��ޤ���
 *
 * @param   length ���󵭽һ�: ���ǿ�
 * @param   offset ���󵭽һ�: �Х��ȥ��ե��å�
 * @param   row_index �ԥ���ǥå���
 * @param   elem_idx ���󵭽һҤ������ֹ� (��ά����0)
 * @return  ���Ȥλ���
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
 * @brief  row�ѥ��������ˡ��������ѹ� (������������)
 *
 *  row�ѥ������ݤ�����ˡ��ɤΤ褦����ˡ�ǹԤʤ�������ꤷ�ޤ���
 *  ���� 3 �Ĥ������򤷤ޤ���<br>
 *    "min", "auto"  ... �Ǿ��¤���ݡ��ꥵ��������ɬ��realloc()���ƤФ��<br>
 *    "pow"  ... 2��n��ǳ���<br>
 *    NULL��¾ ... ���ߤ���ˡ��ݻ�
 *
 * @param   strategy ���������ˡ������
 * @return  ���Ȥλ���
 *
 */
fits_table_col &fits_table_col::set_alloc_strategy( const char *strategy )
{
    this->data_rec.set_alloc_strategy(strategy);
    return *this;
}


/* */


/**
 * @brief  shallow copy ����Ĥ�����˻��� (̤����)
 * @note   ������֥������Ȥ� return ��ľ���ǻ��Ѥ��롥
 */
/* ���: ���Ȥ��֤������Ȥ��������return ʸ�Ǥ������ȥ��󥹥��� */
/*       ��2�󥳥ԡ������Ȥ����������ʻ��ˤʤ�Τ� void �ˤʤäƤ��� */
void fits_table_col::set_scopy_flag()
{
    this->shallow_copy_ok = true;
    return;
}

/* protected */

/**
 * @brief  ����μ����Τ���Υ��дؿ�
 *
 * @note   ���Υ��дؿ��� protected �Ǥ���
 */
fits_table_col &fits_table_col::set_protection( bool protect )
{
    this->protected_rec = protect;
    return *this;
}

/**
 * @brief  ���Ȥ�������� fits_table ���֥������Ȥ���Ͽ
 *
 * @note   ���Υ��дؿ��� protected �Ǥ���
 */
fits_table_col &fits_table_col::register_manager( fits_table *ptr )
{
    this->manager = ptr;
    return *this;
}

/* private */

/**
 * @brief  ������ tnull ���ͤ򥻥å� or �ꥻ�å� (������NULL�ǥꥻ�å�)
 *
 * @note   ���Υ��дؿ��� private �Ǥ���
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
 * @brief  this->def_all_rec��this->def_rec �򹹿�
 *
 * @note   ���Υ��дؿ��� private �Ǥ���
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
 * @brief  �ǥե�����ͤ˥���򥻥å�
 *
 * @note   ���Υ��дؿ��� private �Ǥ���
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
 * @brief  �ǥե�����ͤ˥���򥻥å�
 *
 * @note   ���Υ��дؿ��� private �Ǥ���
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
 * ���: fits_image �ˤ� convert_image_type() ��¸�ߤ��롥���Ƥ�����Ʊ��
 *       �ʤΤǡ������ɤ��ѹ��������Ʊ���˹Ԥʤ�����
 */

struct fits_table_col_cnv_prms {
    /* ���饤���Ȥ����(sizeof��Ĺ����Τ�����!) */
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
 * @brief  �ԥ������ͤ��Ѵ����� mdarray ��ɸ���Ѵ����ɤ����ɤ�����Ƚ�� (������)
 *
 * @note    private �ʴؿ��Ǥ���
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

    /* ξ���Ȥ���ư���������ξ�� */
    if ( src_is_float_type == true && dst_is_float_type == true &&
	 src.tzero()  == dst_tzero && 
	 src.tscal() == dst_tscal ) {
	do_simple_conv = true;
    }
    /* ξ���Ȥ������ξ�� */
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
    /* �������� float ���Ѵ�������� */
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
 * @brief  �ǡ��������Ѵ����ѹ� (���٥�)
 *
 * @note   ���Υ��дؿ��� private �Ǥ���
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

    /* ���Ѵ��Τ���δؿ������� */
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
 * @brief  shallow copy ����ǽ�����֤� (̤����)
 * 
 * @return  shallow copy����ǽ�ʤ鿿<br>
 *          ����ʳ��λ��ϵ�
 * @note  ���Υ��дؿ��� private �Ǥ�
 */
bool fits_table_col::request_shallow_copy( fits_table_col *from_obj ) const
{
    return false;
}

/** 
 * @brief  shallow copy �򥭥�󥻥뤹�� (̤����)
 * 
 * @note  ���Υ��дؿ��� private �Ǥ�
 */
void fits_table_col::cancel_shallow_copy( fits_table_col *from_obj ) const
{
    return;
}

/**
 * @brief  ���ȤˤĤ��ơ�shallow copy �ط��Υ��꡼�󥢥åפ�Ԥʤ� (̤����)
 * 
 * @note  ���Υ��дؿ��� private �Ǥ�
 */
void fits_table_col::cleanup_shallow_copy( bool do_deep_copy_for_this ) const
{
    return;
}


/**
 * @brief  inline���дؿ���throw������˻���
 *
 * @note   ���Υ��дؿ��� private �Ǥ���
 */
void *fits_table_col::err_throw_void_p( const char *fnc, 
				    const char *lv, const char *mes)
{
    err_throw(fnc,lv,mes);
    return (void *)NULL;
}

/**
 * @brief  inline���дؿ���throw������˻���
 *
 * @note   ���Υ��дؿ��� private �Ǥ���
 */
const void *fits_table_col::err_throw_const_void_p( const char *fnc, 
					 const char *lv, const char *mes) const
{
    err_throw(fnc,lv,mes);
    return (const void *)NULL;
}


}	/* namespace sli */

#include "private/c_memset.cc"

