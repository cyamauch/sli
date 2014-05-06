/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2013-05-06 22:00:46 cyamauch> */

/**
 * @file   fits_image.cc
 * @brief  FITS の Image HDU を表現するクラス fits_image のコード
 */

#define CLASS_NAME "fits_image"

#include "config.h"

#include "fits_image.h"

#include <math.h>
#include <stdlib.h>
#include <sli/numeric_minmax.h>
#include <sli/asarray.h>

#include "private/err_report.h"
#include "private/c_memset.h"


namespace sli
{

#include "private/write_stream_or_get_csum.h"


/*===========================================================================*/

/*
 * FITS_IMAGE
 */

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

/**
 * @brief  SFITSIO データ型種別 → mdarray データ型種別 の変換 (内部用)
 *
 * @note    private な関数です．
 */
inline static ssize_t image_type_to_size_type( int fits_type )
{
    ssize_t ret;
    switch ( fits_type ) {
    case FITS::DOUBLE_T:
	ret = DOUBLE_ZT;
	break;
    case FITS::FLOAT_T:
	ret = FLOAT_ZT;
	break;
    case FITS::LONGLONG_T:
	ret = INT64_ZT;
	break;
    case FITS::LONG_T:
	ret = INT32_ZT;
	break;
    case FITS::SHORT_T:
	ret = INT16_ZT;
	break;
    case FITS::BYTE_T:
	ret = UCHAR_ZT;
	break;
    default:
	ret = 0;	/* ERROR */
	break;
    }
    return ret;
}

/**
 * @brief  FITS::ALL => MDARRAY_ALL を含む，sectionサイズ指定の変換 (内部用)
 *
 * @note    private な関数です．
 */
inline static size_t fits_seclen_to_mdarray_seclen( long fits_sz )
{
    return (fits_sz == FITS::ALL) ? 
           MDARRAY_ALL : ((fits_sz < 0) ? 0 : ((size_t)fits_sz));
}

/**
 * @brief  各データ型の最小値を取得 (内部用)
 *
 * @note    private な関数です．
 */
inline static double get_min_for_type( int type )
{
    if ( type == FITS::BYTE_T ) return MIN_DOUBLE_ROUND_UCHAR;
    else if ( type == FITS::SHORT_T ) return MIN_DOUBLE_ROUND_INT16;
    else if ( type == FITS::LONG_T ) return MIN_DOUBLE_ROUND_INT32;
    else if ( type == FITS::LONGLONG_T ) return MIN_DOUBLE_ROUND_INT64;
    else return MIN_DOUBLE_ROUND_LLONG;
}

/**
 * @brief  各データ型の最大値を取得 (内部用)
 *
 * @note    private な関数です．
 */
inline static double get_max_for_type( int type )
{
    if ( type == FITS::BYTE_T ) return MAX_DOUBLE_ROUND_UCHAR;
    else if ( type == FITS::SHORT_T ) return MAX_DOUBLE_ROUND_INT16;
    else if ( type == FITS::LONG_T ) return MAX_DOUBLE_ROUND_INT32;
    else if ( type == FITS::LONGLONG_T ) return MAX_DOUBLE_ROUND_INT64;
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
    else return INDEF_LLONG;
}

/**
 * @brief  書き出される時の blank 値を返す (内部用)
 *
 * @note    private な関数です．
 */
inline static long long get_blank_for_type( int type, long long blank )
{
    if ( type == FITS::BYTE_T ) {
	if ( MIN_UCHAR <= blank && blank <= MAX_UCHAR ) return blank;
	else return INDEF_UCHAR;
    }
    else if ( type == FITS::SHORT_T ) {
	if ( MIN_INT16 <= blank && blank <= MAX_INT16 ) return blank;
	else return INDEF_INT16;
    }
    else if ( type == FITS::LONG_T ) {
	if ( MIN_INT32 <= blank && blank <= MAX_INT32 ) return blank;
	else return INDEF_INT32;
    }
    else if ( type == FITS::LONGLONG_T ) {
	if ( MIN_INT64 <= blank && blank <= MAX_INT64 ) return blank;
	else return INDEF_INT64;
    }
    else return blank;
}

/**
 * @brief  画像データを保持する内部オブジェクトすべてを初期化
 * 
 * @param  sz_type mdarrayでの型種別
 * @note   このメンバ関数は private です．
 */
void fits_image::_init_all_data_recs( int sz_type )
{
    this->uchar_data_rec.init();
    this->uchar_data_rec.set_auto_resize(false);
    this->int16_data_rec.init();
    this->int16_data_rec.set_auto_resize(false);
    this->int32_data_rec.init();
    this->int32_data_rec.set_auto_resize(false);
    this->int64_data_rec.init();
    this->int64_data_rec.set_auto_resize(false);
    this->float_data_rec.init();
    this->float_data_rec.set_auto_resize(false);
    this->double_data_rec.init();
    this->double_data_rec.set_auto_resize(false);

    this->_setup_data_rec(sz_type);

    return;
}

/**
 * @brief  this->data_rec を設定
 * 
 * @param  sz_type mdarrayでの型種別
 * @note   このメンバ関数は private です．
 */
void fits_image::_setup_data_rec( int sz_type )
{
    if ( sz_type == UCHAR_ZT ) {
	this->data_rec = &(this->uchar_data_rec);
    }
    else if ( sz_type == INT16_ZT ) {
	this->data_rec = &(this->int16_data_rec);
    }
    else if ( sz_type == INT32_ZT ) {
	this->data_rec = &(this->int32_data_rec);
    }
    else if ( sz_type == INT64_ZT ) {
	this->data_rec = &(this->int64_data_rec);
    }
    else if ( sz_type == FLOAT_ZT ) {
	this->data_rec = &(this->float_data_rec);
    }
    else if ( sz_type == DOUBLE_ZT ) {
	this->data_rec = &(this->double_data_rec);
    }
    else {
	err_throw(__FUNCTION__,"FATAL","Internal error");
    }

    return;
}


/**
 * @brief  コンストラクタ
 */
fits_image::fits_image()
{
    this->_init_all_data_recs(UCHAR_ZT);
    this->type_rec = FITS::BYTE_T;
    this->bzero_double_rec = 0.0;
    this->bscale_double_rec = 1.0;
    this->set_blank(NULL);
    this->coord_offset_rec.init();
    this->is_flipped_rec.init();
    this->scan_use_flags = 0;

    if ( this->classlevel() < 1 ) this->increase_classlevel();
    this->set_hdutype(FITS::IMAGE_HDU);

    return;
}

/**
 * @brief  コピーコンストラクタ
 */
fits_image::fits_image(const fits_image &obj)
{
    this->_init_all_data_recs(UCHAR_ZT);
    this->type_rec = FITS::BYTE_T;
    this->bzero_double_rec = 0.0;
    this->bscale_double_rec = 1.0;
    this->set_blank(NULL);
    this->coord_offset_rec.init();
    this->is_flipped_rec.init();
    this->scan_use_flags = 0;

    if ( this->classlevel() < 1 ) this->increase_classlevel();
    this->set_hdutype(FITS::IMAGE_HDU);

    this->init(obj);

    return;
}

/**
 * @brief  デストラクタ
 */
fits_image::~fits_image()
{
    return;
}

/**
 * @brief  オブジェクトのコピー
 */
fits_image &fits_image::operator=(const fits_image &obj)
{
    this->init(obj);
    return *this;
}

/**
 * @brief  オブジェクトの初期化
 */
fits_image &fits_image::init()
{
    //err_report("init()","DEBUG","fits_image::init() ...");
    this->fits_hdu::init();

    /* init ... */
    this->_init_all_data_recs(UCHAR_ZT);
    this->type_rec = FITS::BYTE_T;

    this->bzero_rec = NULL;
    this->bscale_rec = NULL;
    this->blank_rec = NULL;
    this->bunit_rec = NULL;

    this->bzero_double_rec = 0.0;
    this->bscale_double_rec = 1.0;
    this->set_blank(NULL);

    this->coord_offset_rec.init();
    this->is_flipped_rec.init();

    /* setup system header of this HDU */
    this->setup_sys_header();

    return *this;
}

/* private */

/**
 * @brief  bzero, bscale などの属性をコピー
 *
 * @note   このメンバ関数は private です．
 */
void fits_image::init_properties( const fits_image &obj )
{
    this->bzero_rec = obj.bzero_rec;
    this->bscale_rec = obj.bscale_rec;
    this->blank_rec = obj.blank_rec;
    this->bunit_rec = obj.bunit_rec;

    this->bzero_double_rec = obj.bzero_double_rec;
    this->bscale_double_rec = obj.bscale_double_rec;
    this->blank_r_rec = obj.blank_r_rec;
    this->blank_w_rec = obj.blank_w_rec;
    this->blank_longlong_rec = obj.blank_longlong_rec;
    this->blank_long_rec = obj.blank_long_rec;
    this->blank_short_rec = obj.blank_short_rec;
    this->blank_byte_rec = obj.blank_byte_rec;
    this->blank_is_set_rec = obj.blank_is_set_rec;

    return;
}

/**
 * @brief  オブジェクトのコピー
 */
fits_image &fits_image::init( const fits_image &obj )
{
    if ( &obj == this ) return *this;

    //err_report("init()","DEBUG","fits_image::init(const fits_image &) ...");
    this->fits_image::init();
    this->fits_hdu::init(obj);

    /* 引数には fits_image クラスしか来ないハズだが一応チェック */
    if ( 1 <= obj.classlevel() && obj.hdutype() == FITS::IMAGE_HDU ) {
	const fits_image &obj1 = (const fits_image &)obj;

	try {
	    /* バッファ初期化 */
	    this->_init_all_data_recs(obj1.data_array().size_type());
	    this->type_rec = obj1.type_rec;

	    this->data_array().init(obj1.data_array());
	}
	catch (...) {
	    err_throw(__FUNCTION__,"FATAL","this->data_array().init() failed");
	}

	/* bzero, bscale などの属性をコピー */
	this->init_properties(obj1);

	this->coord_offset_rec.init(obj1.coord_offset_rec);
	this->is_flipped_rec.init(obj1.is_flipped_rec);
    }

    return *this;
}

/**
 * @brief  オブジェクトの初期化と画像データ配列のリサイズ (1〜3次元)
 */
fits_image &fits_image::init( int type, long naxis0, long naxis1, long naxis2 )
{
    long ndim = 0;

    if ( 0 < naxis0 ) {
	ndim ++;
	if ( 0 < naxis1 ) {
	    ndim ++;
	    if ( 0 < naxis2 ) {
		ndim ++;
	    }
	}
    }

    if ( ndim <= 0 ) {
	long dummy[] = {0};
	this->init(type,dummy,0,true);
    }
    else {
	long i;
	mdarray_long num_pixels;
	try {
	    num_pixels.resize(ndim);
	}
	catch (...) {
	    err_throw(__FUNCTION__,"FATAL","num_pixels.resize() failed");
	}
	i = 0;
	num_pixels[i] = naxis0;
	i++;
	if ( i < ndim ) {
	    num_pixels[i] = naxis1;
	    i++;
	    if ( i < ndim ) {
		num_pixels[i] = naxis2;
		i++;
	    }
	}
	this->init(type, num_pixels.array_ptr_cs(), ndim, true);
    }

    return *this;
}

/**
 * @brief  オブジェクトの初期化と画像データ配列のリサイズ (n次元)
 *
 * @param     type データ配列の型種別 (FITS::SHORT_T，FITS::FLOAT_T 等)
 * @param     naxisx[] 各次元の要素数
 * @param     ndim 配列の次元数
 * @param     buf_init 要素値をデフォルト値で埋めるなら true
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合
 */
fits_image &fits_image::init( int type, const long naxisx[], long ndim,
			      bool buf_init )
{
    ssize_t sz_type;
    long i;
    mdarray_size n_axisx;

    this->init();

    if ( ndim < 0 ) ndim = 0;
    if ( 0 < ndim && naxisx == NULL ) {
	err_report(__FUNCTION__,"WARNING","NULL naxisx; setting ndim = 0");
	ndim = 0;
    }

    sz_type = image_type_to_size_type(type);
    if ( sz_type == 0 ) {
	type = FITS::BYTE_T;
	sz_type = sizeof(fits::byte_t);
	err_report(__FUNCTION__,"WARNING",
		   "unsupported type; FITS::BYTE_T is used");
    }

    /* サイズが 0 以下の軸があるとマズいのでチェック */
    for ( i=0 ; i < ndim ; i++ ) {
	if ( naxisx[i] <= 0 ) {
	    err_report1(__FUNCTION__,"WARNING",
			"invalid naxisx; setting ndim = %ld",i);
	    ndim = i;
	    break;
	}
    }

    try {
	n_axisx.resize(ndim);
    }
    catch (...) {
	err_throw(__FUNCTION__,"FATAL","n_axisx.resize() failed");
    }

    for ( i=0 ; i < ndim ; i++ ) {
	n_axisx[i] = naxisx[i];
    }

    try {
	/* バッファ初期化 */
	this->_init_all_data_recs(image_type_to_size_type(type));
	this->type_rec = type;

	this->data_array().reallocate(n_axisx.array_ptr_cs(), ndim, buf_init);

	this->update_zero_of_data_rec();
    }
    catch (...) {
	err_throw(__FUNCTION__,"FATAL",
		  "this->data_array().reallocate() failed");
    }

    /* setup system header of this HDU */
    this->setup_sys_header();

    return *this;
}

/**
 * @brief  画像データを指定された型へ変換
 *
 * @param     new_type 変換後の型種別 (FITS::FLOAT_T，FITS::DOUBLE_T 等)
 * @return    自身の参照
 */
fits_image &fits_image::convert_type( int new_type )
{
    /* convert ... */
    return this->convert_image_type( new_type, NULL, NULL, NULL );

    /* setup system header of this HDU */
    this->setup_sys_header();
}

/**
 * @brief  指定された型，ZERO値を持つ画像データへ変換
 */
fits_image &fits_image::convert_type( int new_type, double new_zero )
{
    /* convert ... */
    return this->convert_image_type( new_type, &new_zero, NULL, NULL );

    /* setup system header of this HDU */
    this->setup_sys_header();
}

/**
 * @brief  指定された型，ZERO値，SCALE値を持つ画像データへ変換
 */
fits_image &fits_image::convert_type( int new_type, double new_zero, double new_scale )
{
    /* convert ... */
    return this->convert_image_type( new_type, &new_zero, &new_scale, NULL );

    /* setup system header of this HDU */
    this->setup_sys_header();
}

/**
 * @brief  指定された型，ZERO値，SCALE値，BLANK値を持つ画像データへ変換
 */
fits_image &fits_image::convert_type( int new_type, double new_zero, double new_scale,
				      long long new_blank )
{
    /* convert ... */
    return this->convert_image_type( new_type, &new_zero, &new_scale, 
				     &new_blank );

    /* setup system header of this HDU */
    this->setup_sys_header();
}

/**
 * @brief  BZEROの値を設定
 *
 * @param   zero BZEROの値
 * @param   prec 精度(桁数)．省略時は15．
 * @return  自身の参照
 */
fits_image &fits_image::assign_bzero( double zero, int prec )
{
    fits_header &hdr_rec = this->header_rec;

    this->header_set_sysrecords_prohibition(false);
    try {
	long idx = hdr_rec.index("BZERO");
	if ( idx < 0 ) {
	    const char *search_kwds[] = {"BSCALE",NULL};
	    fits::header_def defs[] = { {"BZERO","",""}, {NULL} };
	    hdr_rec.insert_records(
		     this->find_sysheader_insert_point(search_kwds), defs);
	    idx = hdr_rec.index("BZERO");
	    hdr_rec.record(idx).assign_default_comment(FITS::IMAGE_HDU);
	}
	hdr_rec.record(idx).assign(zero,prec);
	this->bzero_rec = hdr_rec.record(idx).value();
	this->bzero_double_rec = hdr_rec.record(idx).dvalue();
	this->update_zero_of_data_rec();
    }
    catch (...) {
	this->header_set_sysrecords_prohibition(true);
	err_throw(__FUNCTION__,"FATAL","caught exception");
    }
    this->header_set_sysrecords_prohibition(true);

    return *this;
}

/**
 * @brief  BSCALEの値を設定
 *
 * @param   scale BSCALEの値
 * @param   prec 精度(桁数)．省略時は15．
 * @return  自身の参照
 */
fits_image &fits_image::assign_bscale( double scale, int prec )
{
    fits_header &hdr_rec = this->header_rec;

    this->header_set_sysrecords_prohibition(false);
    try {
	long idx = hdr_rec.index("BSCALE");
	if ( scale == 0 ) {
	    err_report(__FUNCTION__,"WARNING","ZERO BSCALE cannot be set, ignored.");
	    goto quit;
	}
	if ( idx < 0 ) {
	    const char *search_kwds[] = {"BZERO",NULL};
	    fits::header_def defs[] = { {"BSCALE","",""}, {NULL} };
	    hdr_rec.insert_records(
		      this->find_sysheader_insert_point(search_kwds),defs);
	    idx = hdr_rec.index("BSCALE");
	    hdr_rec.record(idx).assign_default_comment(FITS::IMAGE_HDU);
	}
	hdr_rec.record(idx).assign(scale,prec);
	this->bscale_rec = hdr_rec.record(idx).value();
	this->bscale_double_rec = hdr_rec.record(idx).dvalue();
	this->update_zero_of_data_rec();
    }
    catch (...) {
	this->header_set_sysrecords_prohibition(true);
	err_throw(__FUNCTION__,"FATAL","caught exception");
    }
    this->header_set_sysrecords_prohibition(true);

 quit:
    return *this;
}

/**
 * @brief  BLANKの値を設定
 *
 * @param   blank BLANKの値
 * @return  自身の参照
 */
fits_image &fits_image::assign_blank( long long blank )
{
    fits_header &hdr_rec = this->header_rec;

    this->header_set_sysrecords_prohibition(false);
    try {
	long long blnk;
	long idx = hdr_rec.index("BLANK");
	if ( idx < 0 ) {
	    const char *search_kwds[] = {"BZERO","BSCALE",NULL};
	    fits::header_def defs[] = { {"BLANK","",""}, {NULL} };
	    hdr_rec.insert_records(
		      this->find_sysheader_insert_point(search_kwds),defs);
	    idx = hdr_rec.index("BLANK");
	    hdr_rec.record(idx).assign_default_comment(FITS::IMAGE_HDU);
	}
	hdr_rec.record(idx).assign(blank);
	this->blank_rec = hdr_rec.record(idx).value();
	blnk = hdr_rec.record(idx).llvalue();
	this->set_blank(&blnk);
    }
    catch (...) {
	this->header_set_sysrecords_prohibition(true);
	err_throw(__FUNCTION__,"FATAL","caught exception");
    }
    this->header_set_sysrecords_prohibition(true);

    return *this;
}

/**
 * @brief  BUNITの値を設定
 *
 * @param   unit BUNITの値
 * @return  自身の参照
 */
fits_image &fits_image::assign_bunit( const char *unit )
{
    const char *search_kwds[] = {"BZERO","BSCALE","BLANK",NULL};
    fits_header &hdr_rec = this->header_rec;

#ifdef BUNIT_IS_SPECIAL
    this->header_set_sysrecords_prohibition(false);
    try {
	long idx = hdr_rec.index("BUNIT");
	if ( idx < 0 ) {
	    fits::header_def defs[] = { {"BUNIT","",""}, {NULL} };
	    hdr_rec.insert_records(
		      this->find_sysheader_insert_point(search_kwds),defs);
	    idx = hdr_rec.index("BUNIT");
	    hdr_rec.record(idx).assign_default_comment(FITS::IMAGE_HDU);
	}
	hdr_rec.record(idx).assign(unit);
	this->bunit_rec = hdr_rec.record(idx).svalue();
    }
    catch (...) {
	this->header_set_sysrecords_prohibition(true);
	err_throw(__FUNCTION__,"FATAL","caught exception");
    }
    this->header_set_sysrecords_prohibition(true);
#else
    long idx = hdr_rec.index("BUNIT");
    if ( idx < 0 ) {
	fits::header_def defs[] = { {"BUNIT","",""}, {NULL} };
	hdr_rec.insert_records(
		      this->find_sysheader_insert_point(search_kwds),defs);
	idx = hdr_rec.index("BUNIT");
	hdr_rec.record(idx).assign_default_comment(FITS::IMAGE_HDU);
    }
    hdr_rec.record(idx).assign(unit);
#endif

    return *this;
}

/**
 * @brief  BZEROの設定を消去
 *
 * @return  自身の参照
 */
fits_image &fits_image::erase_bzero()
{
    fits_header &hdr_rec = this->header_rec;

    this->header_set_sysrecords_prohibition(false);
    try {
	long idx = hdr_rec.index("BZERO");
	if ( 0 <= idx ) {
	    hdr_rec.erase_records(idx,1);
	}
	this->bzero_rec = NULL;
	this->bzero_double_rec = 0.0;
	this->update_zero_of_data_rec();
    }
    catch (...) {
	this->header_set_sysrecords_prohibition(true);
	err_throw(__FUNCTION__,"FATAL","caught exception");
    }
    this->header_set_sysrecords_prohibition(true);

    return *this;
}

/**
 * @brief  BSCALEの設定を消去
 *
 * @return  自身の参照
 */
fits_image &fits_image::erase_bscale()
{
    fits_header &hdr_rec = this->header_rec;

    this->header_set_sysrecords_prohibition(false);
    try {
	long idx = hdr_rec.index("BSCALE");
	if ( 0 <= idx ) {
	    hdr_rec.erase_records(idx,1);
	}
	this->bscale_rec = NULL;
	this->bscale_double_rec = 1.0;
	this->update_zero_of_data_rec();
    }
    catch (...) {
	this->header_set_sysrecords_prohibition(true);
	err_throw(__FUNCTION__,"FATAL","caught exception");
    }
    this->header_set_sysrecords_prohibition(true);

    return *this;
}

/**
 * @brief  BLANKの設定を消去
 *
 * @return  自身の参照
 */
fits_image &fits_image::erase_blank()
{
    fits_header &hdr_rec = this->header_rec;

    this->header_set_sysrecords_prohibition(false);
    try {
	long idx = hdr_rec.index("BLANK");
	if ( 0 <= idx ) {
	    hdr_rec.erase_records(idx,1);
	}
	this->blank_rec = NULL;
	this->set_blank(NULL);
    }
    catch (...) {
	this->header_set_sysrecords_prohibition(true);
	err_throw(__FUNCTION__,"FATAL","caught exception");
    }
    this->header_set_sysrecords_prohibition(true);

    return *this;
}

/**
 * @brief  BUNITの設定を消去
 *
 * @return  自身の参照
 */
fits_image &fits_image::erase_bunit()
{
    fits_header &hdr_rec = this->header_rec;

#ifdef BUNIT_IS_SPECIAL
    this->header_set_sysrecords_prohibition(false);
    try {
	long idx = hdr_rec.index("BUNIT");
	if ( 0 <= idx ) {
	    hdr_rec.erase_records(idx,1);
	}
	this->bunit_rec = NULL;
    }
    catch (...) {
	this->header_set_sysrecords_prohibition(true);
	err_throw(__FUNCTION__,"FATAL","caught exception");
    }
    this->header_set_sysrecords_prohibition(true);
#else
    long idx = hdr_rec.index("BUNIT");
    if ( 0 <= idx ) {
	hdr_rec.erase_records(idx,1);
    }
#endif

    return *this;
}

/**
 * @brief  2つのオブジェクト間での内容のスワップ
 *
 *  指定されたオブジェクトsobj の内容と自身の内容を入れ替えます．
 *  画像データ配列・ヘッダの内容，属性等すべての状態が入れ替わります．
 *
 * @param     sobj fits_image クラスのオブジェクト
 * @return    自身の参照    
 */
fits_image &fits_image::swap( fits_image &obj )
{
    if ( &obj == this ) return *this;

    int tmp_type_rec = obj.type_rec;
    double tmp_bzero_double_rec = obj.bzero_double_rec;
    double tmp_bscale_double_rec = obj.bscale_double_rec;
    long long tmp_blank_r_rec = obj.blank_r_rec;
    long long tmp_blank_w_rec = obj.blank_w_rec;
    long long tmp_blank_longlong_rec = obj.blank_longlong_rec;
    long tmp_blank_long_rec = obj.blank_long_rec;
    short tmp_blank_short_rec = obj.blank_short_rec;
    unsigned char tmp_blank_byte_rec = obj.blank_byte_rec;
    bool tmp_blank_is_set_rec = obj.blank_is_set_rec;

    obj.type_rec = this->type_rec;
    obj.bzero_double_rec = this->bzero_double_rec;
    obj.bscale_double_rec = this->bscale_double_rec;
    obj.blank_r_rec = this->blank_r_rec;
    obj.blank_w_rec = this->blank_w_rec;
    obj.blank_longlong_rec = this->blank_longlong_rec;
    obj.blank_long_rec = this->blank_long_rec;
    obj.blank_short_rec = this->blank_short_rec;
    obj.blank_byte_rec = this->blank_byte_rec;
    obj.blank_is_set_rec = this->blank_is_set_rec;

    this->type_rec = tmp_type_rec;

    this->bzero_double_rec = tmp_bzero_double_rec;
    this->bscale_double_rec = tmp_bscale_double_rec;
    this->blank_r_rec = tmp_blank_r_rec;
    this->blank_w_rec = tmp_blank_w_rec;
    this->blank_longlong_rec = tmp_blank_longlong_rec;
    this->blank_long_rec = tmp_blank_long_rec;
    this->blank_short_rec = tmp_blank_short_rec;
    this->blank_byte_rec = tmp_blank_byte_rec;
    this->blank_is_set_rec = tmp_blank_is_set_rec;

    this->bzero_rec.swap(obj.bzero_rec);
    this->bscale_rec.swap(obj.bscale_rec);
    this->blank_rec.swap(obj.blank_rec);
    this->bunit_rec.swap(obj.bunit_rec);

    this->coord_offset_rec.swap(obj.coord_offset_rec);
    this->is_flipped_rec.swap(obj.is_flipped_rec);

    this->uchar_data_rec.swap(obj.uchar_data_rec);
    this->int16_data_rec.swap(obj.int16_data_rec);
    this->int32_data_rec.swap(obj.int32_data_rec);
    this->int64_data_rec.swap(obj.int64_data_rec);
    this->float_data_rec.swap(obj.float_data_rec);
    this->double_data_rec.swap(obj.double_data_rec);

    /* data_rec ポインタ貼りなおし */
    this->_setup_data_rec(image_type_to_size_type(this->type_rec));
    obj._setup_data_rec(image_type_to_size_type(obj.type_rec));

    fits_hdu::swap(obj);

    return *this;
}

/**
 * @brief  自身の内容を指定オブジェクトへコピー
 *
 *  自身の全ての内容を指定されたオブジェクト dest_img へコピーします．
 *  画像データ配列・ヘッダの内容，属性等すべてがコピーされます．自身(コピー元)
 *  は改変されません．<br>
 *  型と画像データの全配列長が dest_img と自身とで等しい場合は，配列用バッファ
 *  の再確保は行なわれず，既存のバッファに配列の内容がコピーされます．
 *
 * @param     dest_img コピー先のオブジェクトのアドレス
 * @throw     バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 */
void fits_image::copy( fits_image *dest_img ) const
{
    if ( dest_img == NULL ) {
	return;
    }
    /* dest_img が自身なら何もしない */
    else if ( dest_img != this ) {

	if ( dest_img->type() != this->type() ) {
	    /* バッファ初期化 */
	    dest_img->_init_all_data_recs(this->data_array().size_type());
	    dest_img->type_rec = this->type_rec;
	}

	/* 自身のバッファの内容をコピー */
	this->data_array().copy( dest_img->data_rec );

	/* bzero, bscale などの属性をコピー */
	dest_img->init_properties(*this);
	/* ヘッダをコピー */
	dest_img->header_init( this->header() );

	return;
    }
}

/**
 * @deprecated 非推奨．<br>
 * void fits_image::copy( fits_image *dest_img ) const <br>
 * をお使いください．
 */
void fits_image::copy( fits_image &dest_img ) const
{
    this->copy(&dest_img);
    return;
}

/**
 * @brief  自身の内容を指定オブジェクトへ移管
 *
 *  自身のデータ配列の内容を，dest_img により指定されたオブジェクトへ「移管」
 *  します(型，配列長，各種属性が指定されたオブジェクトにコピーされます)．移管
 *  の結果，自身のデータ配列の長さはゼロになります．<br>
 *  dest_img についての配列用バッファの再確保は行なわれず，自身の配列用バッファ
 *  についての管理権限を dest_img に譲渡する実装になっており 高速に動作します
 *  (ただし，ヘッダに関しては現状ではコピー&消去の実装になっています)．
 *
 * @param     dest_img 移管・コピー先のオブジェクトのアドレス
 * @return    自身の参照
 * @throw     バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 */
fits_image &fits_image::cut( fits_image *dest_img )
{
    if ( dest_img == NULL ) {
	if ( 0 < this->dim_length() ) this->reallocate(NULL,0,false);
	this->header_init();
    }
    /* dest_img が自身なら何もしない */
    else if ( dest_img != this ) {

	if ( dest_img->type() != this->type() ) {
	    /* バッファ初期化 */
	    dest_img->_init_all_data_recs(this->data_array().size_type());
	    dest_img->type_rec = this->type_rec;
	}

	/* 自身のバッファの内容を移動 */
	this->data_array().cut( dest_img->data_rec );

	/* bzero, bscale などの属性をコピー */
	dest_img->init_properties(*this);
	/* ヘッダをコピー */
	dest_img->header_init( this->header() );

	this->header_init();

    }

    return *this;
}

/* interchange rows and columns */
/**
 * @brief  自身の画像データ配列の (x,y) でのトランスポーズ
 *
 *  自身の画像データ配列のカラムとロウとを入れ替えます．
 * 
 * @note  高速なアルゴリズムを使用しています．
 */
fits_image &fits_image::transpose_xy()
{
    this->data_array().transpose_xy();

    /* setup system header of this HDU */
    this->setup_sys_header();

    return *this;
}

/* interchange xyz to zxy */
/**
 * @brief  自身の画像データ配列の (x,y,z)→(z,x,y) のトランスポーズ
 *
 *  自身の画像データ配列の軸 (x,y,z) を (z,x,y) へ変換します．
 * 
 * @note  高速なアルゴリズムを使用しています．
 */
fits_image &fits_image::transpose_xyz2zxy()
{
    this->data_array().transpose_xyz2zxy();

    /* setup system header of this HDU */
    this->setup_sys_header();

    return *this;
}

/* rotate image */
/**
 * @brief  自身の画像データ配列の (x,y) での回転 (90度単位)
 *
 *  自身の画像データ配列の (x,y) 面についての回転(90度単位)を行ないます．<br>
 *  左下を原点とした場合，反時計まわりで指定します．
 *
 * @param  angle 90,-90, 180 のいずれかを指定
 * 
 */
fits_image &fits_image::rotate_xy( int angle )
{
    this->data_array().rotate_xy( angle );

    /* setup system header of this HDU */
    this->setup_sys_header();

    return *this;
}


/*
 * obtain basic information of this image.
 *
 * Note that argument `axis' is 0-indexed.
 */

/**
 * @brief  BUNITの値を取得
 */
const char *fits_image::bunit() const
{
#ifdef BUNIT_IS_SPECIAL
    if ( 0 < this->bunit_rec.length() ) return this->bunit_rec.cstr();
    else return "";
#else
    long idx = this->header_rec.index("BUNIT");
    if ( idx < 0 ) return "";
    else return this->header_rec.at_cs(idx).svalue();
#endif
}

/**
 * @brief  BUNITの値が設定されているかどうかを返す
 */
bool fits_image::bunit_is_set() const
{
#ifdef BUNIT_IS_SPECIAL
    if ( 0 < this->bunit_rec.length() ) return true;
    else return false;
#else
    long idx = this->header_rec.index("BUNIT");
    if ( idx < 0 ) return false;
    else {
	if ( 0 < this->header_rec.at_cs(idx).svalue_length() ) return true;
	else return false;
    }
#endif
}


/*
 * C-like APIs to input and output data
 */

/**
 * @brief  自身の画像データ配列をユーザ・バッファへコピー (バイト数での指定)
 *
 *  自身の画像データ配列の内容を dest_buf で指定されたユーザ・バッファへコピー
 *  します．<br>
 *  バッファの大きさ buf_size は，バイト単位で与えます．axis0, axis1, axis2 で
 *  自身の配列での開始点を指定します．<br>
 *  引数に，FITS::INDEF を明示的に与えないでください．
 *
 * @param     dest_buf ユーザ・バッファのアドレス (コピー先)
 * @param     buf_size バッファサイズ (バイト単位)
 * @param     axis0 次元番号 0 の次元(1次元目)の要素番号 (省略可)
 * @param     axis1 次元番号 1 の次元(2次元目)の要素番号 (省略可)
 * @param     axis2 次元番号 2 の次元(3次元目)の要素番号 (省略可)
 * @return    ユーザのバッファ長(buf_size) が十分な場合にコピーされる
 *            バイトサイズ
 * @throw     メモリ破壊を起こした場合
 */
ssize_t fits_image::get_data( void *dest_buf, size_t buf_size,
			      long axis0, long axis1, long axis2 ) const
{
    ssize_t ix0,ix1,ix2;
    ix0 = (axis0 == FITS::INDEF) ? MDARRAY_INDEF : axis0;
    ix1 = (axis1 == FITS::INDEF) ? MDARRAY_INDEF : axis1;
    ix2 = (axis2 == FITS::INDEF) ? MDARRAY_INDEF : axis2;
    return this->data_array().getdata(dest_buf, buf_size, ix0,ix1,ix2);
}

/**
 * @brief  ユーザ・バッファから自身の画像データ配列へコピー (バイト数での指定)
 *
 *  src_buf で指定されたユーザ・バッファの内容を自身の画像データ配列へコピーし
 *  ます．<br>
 *  バッファの大きさ buf_size は，バイト単位で与えます．axis0, axis1, axis2 で
 *  自身の配列での開始点を指定します．<br>
 *  引数に，FITS::INDEF を明示的に与えないでください．
 *
 * @param     src_buf  ユーザ・バッファのアドレス (コピー元)
 * @param     buf_size ユーザ・バッファのサイズ (バイト単位)
 * @param     axis0 次元番号 0 の次元(1次元目)の要素番号 (省略可)
 * @param     axis1 次元番号 1 の次元(2次元目)の要素番号 (省略可)
 * @param     axis2 次元番号 2 の次元(3次元目)の要素番号 (省略可)
 * @return    ユーザのバッファ長(buf_size) が十分な場合にコピーされる
 *            バイトサイズ
 * @throw     メモリ破壊を起こした場合
 */
ssize_t fits_image::put_data( const void *src_buf, size_t buf_size,
			      long axis0, long axis1, long axis2 )
{
    ssize_t ix0,ix1,ix2;
    ix0 = (axis0 == FITS::INDEF) ? MDARRAY_INDEF : axis0;
    ix1 = (axis1 == FITS::INDEF) ? MDARRAY_INDEF : axis1;
    ix2 = (axis2 == FITS::INDEF) ? MDARRAY_INDEF : axis2;
    return this->put_data(src_buf,buf_size, ix0,ix1,ix2);
}


/*
 * member functions to update length, type, etc.
 */

/**
 * @brief  次元数を1つ拡張
 *
 *  自身が持つ画像データ配列の次元数を 1つ拡張します．
 *
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合
 */
fits_image &fits_image::increase_dim()
{
    /* increase dimension */
    this->data_array().increase_dim();

    /* setup system header of this HDU */
    this->setup_sys_header();

    return *this;
}

/**
 * @brief  次元数を1つ拡張
 *
 *  自身が持つ画像データ配列の次元数を 1つ拡張します．
 *
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合
 * @note      fits_image::increase_dim() との違いはありません．
 */
fits_image &fits_image::increase_axis()
{
    this->increase_dim();
    return *this;
}

/**
 * @brief  次元数を1つ縮小
 *
 *  自身が持つ画像データ配列の次元を 1つ縮小します．
 *
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 */
fits_image &fits_image::decrease_dim()
{
    /* decrease dimension */
    this->data_array().decrease_dim();

    /* setup system header of this HDU */
    this->setup_sys_header();

    return *this;
}

/**
 * @brief  次元数を1つ縮小
 *
 *  自身が持つ画像データ配列の次元を 1つ縮小します．
 *
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 * @note      fits_image::decrease_dim() との違いはありません．
 */
fits_image &fits_image::decrease_axis()
{
    this->decrease_dim();
    return *this;
}

/**
 * @brief  任意の1つの次元についての画像データ配列長の変更
 *
 *  自身が持つ画像データ配列の長さを変更します．<br>
 *  配列長を拡張する場合，要素の値はデフォルト値で埋められます．<br>
 *  配列長を収縮する場合，len以降の要素は削除されます．<br>
 *  次元番号 axis で処理対象とする次元を指定できます．
 *
 * @param     axis 次元番号
 * @param     size 要素の個数
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合
 */
fits_image &fits_image::resize( long axis, long size )
{
    if ( axis < 0 || size < 0 ) return *this;	/* invalid */

    /* resize image buffer */
    this->data_array().resize(axis, size);

    /* setup system header of this HDU */
    this->setup_sys_header();

    return *this;
}

/* change the length of the 1-d array */
/**
 * @brief  画像データ配列長の変更 (1次元)
 *
 *  自身が持つ画像データ配列の長さを変更し，配列長 x_len の1次元配列とします．
 */
fits_image &fits_image::resize_1d( long x_len )
{
    const long nx[] = {x_len};
    return this->resize(nx, 1, true);
}

/* change the length of the 2-d array */
/**
 * @brief  画像データ配列長の変更 (2次元)
 *
 *  自身が持つ画像データ配列の長さを変更し，配列長 (x_len, y_len) の 
 *  2次元配列とします．
 */
fits_image &fits_image::resize_2d( long x_len, long y_len )
{
    const long nx[] = {x_len, y_len};
    return this->resize(nx, 2, true);
}

/* change the length of the 3-d array */
/**
 * @brief  画像データ配列長の変更 (3次元)
 *
 *  自身が持つ画像データ配列の長さを変更し，配列長 (x_len, y_len, z_len) の
 *  3次元配列とします．
 */
fits_image &fits_image::resize_3d( long x_len, long y_len, long z_len )
{
    const long nx[] = {x_len, y_len, z_len};
    return this->resize(nx, 3, true);
}

/**
 * @brief  画像データ配列長の変更 (複数の次元を指定可)
 *
 *  自身が持つ画像データ配列の長さを変更します．<br>
 *  配列長を拡張する場合，要素の値をデフォルト値で埋るかどうかは buf_init で
 *  設定できます．<br>
 *  配列長を収縮する場合，配列長に満たない部分の要素は削除されます．<br>
 *
 * @param     naxisx[] 各次元の要素数
 * @param     ndim 配列の次元数
 * @param     buf_init 配列長の拡張時，要素値をデフォルト値で埋めるなら true
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合
 * @note      2次元以上のリサイズの場合，低コストで行なえます．1次元だけリサイ
 *            ズするなら，resize(axis, ...) の方が低コストです．
 */
fits_image &fits_image::resize( const long naxisx[], long ndim,
				bool buf_init )
{
    mdarray_size n_axisx;
    long i;

    if ( ndim < 0 ) ndim = 0;
    if ( 0 < ndim && naxisx == NULL ) {
	err_report(__FUNCTION__,"WARNING","NULL naxisx; setting ndim = 0");
	ndim = 0;
    }

    try {
	n_axisx.resize(ndim);
    }
    catch (...) {
	err_throw(__FUNCTION__,"FATAL","n_axisx.resize() failed");
    }

    for ( i=0 ; i < ndim ; i++ ) {
	if ( naxisx[i] == FITS::ALL || naxisx[i] <= 0 )
	    n_axisx[i] = MDARRAY_ALL;
	else
	    n_axisx[i] = naxisx[i];
    }

    /* resize image buffer */
    try {
	this->data_array().resize(n_axisx.array_ptr_cs(), ndim, buf_init);
    }
    catch (...) {
	err_throw(__FUNCTION__,"FATAL","this->data_array().resize() failed");
    }

    /* setup system header of this HDU */
    this->setup_sys_header();

    return *this;
}

/**
 * @brief  画像データ配列長の変更 (IDLの記法)
 *
 *  自身が持つ画像データ配列の長さを変更します．長さの指定は，printf() 形式の
 *  可変長引数で指定可能なカンマ区切りの文字列(例えば"10,5")によって行ないま
 *  す．<br>
 *  指定された文字列中の要素が数字ではない場合，その次元は長さを変更しません．
 *  例えば， ",2" が指定された場合，最初の次元の長さは変更されません．
 *
 * @param   exp_fmt csv形式の長さ指定文字列のためのフォーマット指定
 * @param   ...     exp_fmtに対応した可変長引数の各要素データ
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
fits_image &fits_image::resizef( const char *exp_fmt, ... )
{
    /* resize image buffer */
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	this->data_array().vresizef(exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","this->data_array().vresizef() failed");
    }
    va_end(ap);

    /* setup system header of this HDU */
    this->setup_sys_header();

    return *this;
}

/**
 * @brief  画像データ配列長の変更 (IDLの記法)
 *
 *  自身が持つ画像データ配列の長さを変更します．長さの指定は，printf() 形式の
 *  可変長引数で指定可能なカンマ区切りの文字列(例えば"10,5")によって行ないま
 *  す．<br>
 *  指定された文字列中の要素が数字ではない場合，その次元は長さを変更しません．
 *  例えば， ",2" が指定された場合，最初の次元の長さは変更されません．
 *
 * @param   exp_fmt csv形式の長さ指定文字列のためのフォーマット指定
 * @param   ap      exp_fmtに対応した可変長引数のリスト
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
fits_image &fits_image::vresizef( const char *exp_fmt, va_list ap )
{
    /* resize image buffer */
    this->data_array().vresizef(exp_fmt, ap);

    /* setup system header of this HDU */
    this->setup_sys_header();

    return *this;
}

/**
 * @brief  任意の1つの次元について画像データ配列長の相対的な変更
 *
 *  自身が持つ画像データ配列の長さを size_rel の指定分，拡張・縮小します．<br>
 *  resizeby() 後の配列サイズは，元の配列の長さに size_rel を加えたものとなり
 *  ます．
 *  サイズの縮小は，size_rel にマイナス値を指定することによって行います．<br>
 *  次元番号 axis で処理対象とする次元を指定できます．
 *
 * @param     axis 次元番号
 * @param     size_rel 要素個数の増分・減分
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合
 */
fits_image &fits_image::resizeby( long axis, long size_rel )
{
    if ( axis < 0 ) return *this;	/* invalid */

    /* resize image buffer */
    this->data_array().resizeby(axis, size_rel);

    /* setup system header of this HDU */
    this->setup_sys_header();

    return *this;
}

/* change the length of the 1-d array relatively */
/**
 * @brief  画像データ配列長の相対的な変更 (1次元用)
 */
fits_image &fits_image::resizeby_1d( long x_len )
{
    const long nx[] = {x_len};
    return this->resizeby(nx, 1, true);
}

/* change the length of the 2-d array relatively */
/**
 * @brief  画像データ配列長の相対的な変更 (2次元用)
 */
fits_image &fits_image::resizeby_2d( long x_len, long y_len )
{
    const long nx[] = {x_len, y_len};
    return this->resizeby(nx, 2, true);
}

/* change the length of the 3-d array relatively */
/**
 * @brief  画像データ配列長の相対的な変更 (3次元用)
 */
fits_image &fits_image::resizeby_3d( long x_len, long y_len, long z_len )
{
    const long nx[] = {x_len, y_len, z_len};
    return this->resizeby(nx, 3, true);
}

/**
 * @brief  画像データ配列長の相対的な変更 (複数の次元を指定可)
 *
 *  自身が持つ画像データ配列の長さを naxisx_rel[] の指定分，拡張・縮小しま
 *  す．<br>
 *  resizeby() 後の配列サイズは，元の配列の長さに naxisx_rel[] を加えたもの
 *  となります．サイズの縮小は，naxisx_rel[] にマイナス値を指定することによって
 *  行います．<br>
 *  buf_buf でサイズ拡張時に新規に作られる配列要素の初期化をするかどうかを
 *  指定できます．
 *
 * @param     naxisx_rel 要素個数の増分・減分
 * @param     ndim naxisx_rel[] の個数
 * @param     buf_init 新規に作られる配列要素の初期化を行なう場合は true
 * @return    自身の参照
 * @note      次元長を増やす事はできますが，減らす事はできません．
 * @throw     内部バッファの確保に失敗した場合
 */
fits_image &fits_image::resizeby( const long naxisx_rel[], long ndim,
				  bool buf_init )
{
    mdarray_ssize n_axisx_rel;
    long i;

    if ( ndim < 0 ) ndim = 0;
    if ( 0 < ndim && naxisx_rel == NULL ) {
	err_report(__FUNCTION__,"WARNING","NULL naxisx_rel; setting ndim = 0");
	ndim = 0;
    }

    try {
	n_axisx_rel.resize(ndim);
    }
    catch (...) {
	err_throw(__FUNCTION__,"FATAL","n_axisx_rel.resize() failed");
    }

    for ( i=0 ; i < ndim ; i++ ) {
	n_axisx_rel[i] = naxisx_rel[i];
    }

    /* resize image buffer */
    try {
	this->data_array().resizeby(n_axisx_rel.array_ptr_cs(), ndim, buf_init);
    }
    catch (...) {
	err_throw(__FUNCTION__,"FATAL","this->data_array().resizeby() failed");
    }

    /* setup system header of this HDU */
    this->setup_sys_header();

    return *this;
}

/**
 * @brief  画像データ配列長の相対的な変更 (IDLの記法)
 *
 *  自身が持つ画像データ配列の長さを指定分，拡張・縮小します．長さの指定は，
 *  printf() 形式の可変長引数で指定可能なカンマ区切りの文字列(例えば"10,-5")に
 *  よって行ないます．<br>
 *  resizebyf()後の配列サイズは，元の配列の長さに指定分を加えたものとなります．
 *  サイズの縮小は，マイナス値を指定することによって行います．<br>
 *  指定された文字列中の要素が数字ではない場合，その次元は長さを変更しません．
 *  例えば， ",2" が指定された場合，最初の次元の長さは変更されません．
 *
 * @param   exp_fmt csv形式の長さ指定文字列のためのフォーマット指定
 * @param   ...     exp_fmtに対応した可変長引数の各要素データ
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
fits_image &fits_image::resizebyf( const char *exp_fmt, ... )
{
    /* resize image buffer */
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	this->data_array().vresizebyf(exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","this->data_array().vresizebyf() failed");
    }
    va_end(ap);

    /* setup system header of this HDU */
    this->setup_sys_header();

    return *this;
}

/**
 * @brief  画像データ配列長の相対的な変更 (IDLの記法)
 *
 *  自身が持つ画像データ配列の長さを指定分，拡張・縮小します．長さの指定は，
 *  printf() 形式の可変長引数で指定可能なカンマ区切りの文字列(例えば"10,-5")に
 *  よって行ないます．<br>
 *  resizebyf()後の配列サイズは，元の配列の長さに指定分を加えたものとなります．
 *  サイズの縮小は，マイナス値を指定することによって行います．<br>
 *  指定された文字列中の要素が数字ではない場合，その次元は長さを変更しません．
 *  例えば， ",2" が指定された場合，最初の次元の長さは変更されません．
 *
 * @param   exp_fmt csv形式の長さ指定文字列のためのフォーマット指定
 * @param   ap      exp_fmtに対応した可変長引数のリスト
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
fits_image &fits_image::vresizebyf( const char *exp_fmt, va_list ap )
{
    /* resize image buffer */
    this->data_array().vresizebyf(exp_fmt, ap);

    /* setup system header of this HDU */
    this->setup_sys_header();

    return *this;
}

/**
 * @brief  画像データ配列のバッファ内位置の調整をせずに，配列用バッファを再確保
 *
 *  画像データ配列のバッファ内位置に調整をせずに，自身が持つ次元の大きさと配列
 *  の長さを変更します．つまり，配列用バッファに対しては単純にrealloc()を呼ぶだ
 *  けの処理を行ないます．
 *
 * @param   naxisx[] 各次元の要素数
 * @param   ndim 配列の次元数
 * @param   buf_init 新たに確保した部分の要素値をデフォルト値で埋めるなら true
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
fits_image &fits_image::reallocate( const long naxisx[], long ndim,
				    bool buf_init )
{
    mdarray_size n_axisx;
    long i;

    if ( ndim < 0 ) ndim = 0;
    if ( 0 < ndim && naxisx == NULL ) {
	err_report(__FUNCTION__,"WARNING","NULL naxisx; setting ndim = 0");
	ndim = 0;
    }

    try {
	n_axisx.resize(ndim);
    }
    catch (...) {
	err_throw(__FUNCTION__,"FATAL","n_axisx.resize() failed");
    }

    for ( i=0 ; i < ndim ; i++ ) {
	if ( naxisx[i] == FITS::ALL || naxisx[i] <= 0 )
	    n_axisx[i] = MDARRAY_ALL;
	else
	    n_axisx[i] = naxisx[i];
    }

    /* resize image buffer */
    try {
	this->data_array().reallocate(n_axisx.array_ptr_cs(), ndim, buf_init);
    }
    catch (...) {
	err_throw(__FUNCTION__,"FATAL",
		  "this->data_array().reallocate() failed");
    }

    /* setup system header of this HDU */
    this->setup_sys_header();

    return *this;
}

/**
 * @brief  画像データ配列用バッファを一旦開放し，新規に確保
 *
 *  画像データ配列の内容を一旦破棄し，配列の長さを変更します．つまり，配列用
 *  バッファに対して free()，malloc() を呼ぶだけの処理を行ないます．
 *
 * @param   naxisx[] 各次元の要素数
 * @param   ndim 配列の次元数
 * @param   buf_init 新たに確保した部分の要素値をデフォルト値で埋めるなら true
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 */
fits_image &fits_image::allocate( const long naxisx[], long ndim,
				  bool buf_init )
{
    mdarray_size n_axisx;
    long i;

    if ( ndim < 0 ) ndim = 0;
    if ( 0 < ndim && naxisx == NULL ) {
	err_report(__FUNCTION__,"WARNING","NULL naxisx; setting ndim = 0");
	ndim = 0;
    }

    try {
	n_axisx.resize(ndim);
    }
    catch (...) {
	err_throw(__FUNCTION__,"FATAL","n_axisx.resize() failed");
    }

    for ( i=0 ; i < ndim ; i++ ) {
	if ( naxisx[i] == FITS::ALL || naxisx[i] <= 0 )
	    n_axisx[i] = MDARRAY_ALL;
	else
	    n_axisx[i] = naxisx[i];
    }

    /* resize image buffer */
    try {
	this->data_array().allocate(n_axisx.array_ptr_cs(), ndim, buf_init);
    }
    catch (...) {
	err_throw(__FUNCTION__,"FATAL","this->data_array().allocate() failed");
    }

    /* setup system header of this HDU */
    this->setup_sys_header();

    return *this;
}

/*
 * Set default value when resizing
 * Note: default value will be resetted when changing attributes of column
 */
/* high-level */
/**
 * @brief  画像データ配列のサイズ拡張時の初期値の設定
 *
 *  画像データ配列のサイズ拡張時の初期値を設定します．設定された値は既存の要素
 *  には作用せず，サイズ拡張時に有効となります．
 *
 * @param     value 配列サイズ拡張時の初期値
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合
 */
fits_image &fits_image::assign_default( double value )
{
    fits_image dummy;
    dummy.init(this->type(), 1);
    if ( this->bzero_is_set() == true ) dummy.assign_bzero(this->bzero());
    if ( this->bscale_is_set() == true ) dummy.assign_bscale(this->bscale());
    if ( this->blank_is_set() == true ) dummy.assign_blank(this->blank());
    dummy.assign(value,0);
    this->data_array().mdarray::assign_default((const void *)(dummy.data_ptr_cs()));

    return *this;
}

/* low-level */
/**
 * @brief  画像データ配列のサイズ拡張時の初期値の設定 (低レベル)
 *
 *  画像データ配列のサイズ拡張時の初期値を設定します．設定された値は既存の要素
 *  には作用せず，サイズ拡張時に有効となります．
 *
 * @param     value_ptr 配列サイズ拡張時の初期値のアドレス
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合
 */
fits_image &fits_image::assign_default_value( const void *value_ptr )
{
    this->data_array().mdarray::assign_default(value_ptr);

    return *this;
}

/* insert a blank section */
/**
 * @brief  任意の1つの次元について新規配列要素の挿入
 *
 *  自身の画像データ配列の要素位置 idx に，len 個分の要素を挿入します．なお，
 *  挿入される要素の値はデフォルト値です．<br>
 *  次元番号 dim_index で処理対象とする次元を指定できます．
 *
 * @param     dim_index 次元番号
 * @param     idx 挿入位置の要素番号
 * @param     len 要素の個数
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合
 */
fits_image &fits_image::insert( size_t dim_index, long idx, size_t len )
{
    this->data_array().insert(dim_index, idx, len);

    /* setup system header of this HDU */
    this->setup_sys_header();

    return *this;
}

/* erase a section */
/**
 * @brief  任意の1つの次元について配列要素の削除
 *
 *  自身の画像データ配列から指定された部分の要素を削除します．削除した分，
 *  長さは短くなります．<br>
 *  次元番号 dim_index で処理対象とする次元を指定できます．
 *
 * @param     dim_index 次元番号
 * @param     idx 開始位置の要素番号
 * @param     len 要素の個数
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合
 */
fits_image &fits_image::erase( size_t dim_index, long idx, size_t len )
{
    this->data_array().erase(dim_index, idx, len);

    /* setup system header of this HDU */
    this->setup_sys_header();

    return *this;
}

/* copy values between elements (without automatic resizing) */
/* 移動(バッファのサイズは変更しない)                        */
/**
 * @brief  任意の1つの次元について配列要素間での値のコピー
 *
 *  自身の画像データ配列の要素間で値をコピーします．<br>
 *  引数 clr に false が指定された場合はコピー元の値は残りますが，true が指定
 *  された場合はコピー元の値は残らずデフォルト値で埋められます．<br>
 *  idx_dst に既存の配列長より大きな値を設定しても，配列サイズは変わりません．
 *  この点が次の cpy() メンバ関数とは異なります．<br>
 *  次元番号 dim_index で処理対象とする次元を指定できます．
 *
 * @param     dim_index 次元番号
 * @param     idx_src コピー元の要素番号
 * @param     len コピー元の要素の長さ
 * @param     idx_dst コピー先の要素番号
 * @param     clr コピー元の値のクリア可否
 * @return    自身の参照
 */
fits_image &fits_image::move( long dim_index, 
			      long idx_src, long len, long idx_dst, bool clr )
{
    this->data_array().move(dim_index, idx_src, len, idx_dst, clr);
    return *this;
}

/* copy values between elements (with automatic resizing) */
/* 移動(バッファのサイズは必要に応じて変更する)           */
/**
 * @brief  任意の1つの次元について配列要素間での値のコピー (配列数は自動拡張)
 *
 *  自身の画像データ配列要素間で値をコピーします．<br>
 *  引数 clr に false が指定された場合はコピー元の値は残りますが，true が指定
 *  された場合はコピー元の値は残らずデフォルト値で埋められます．<br>
 *  idx_dst + len が既存の配列長より大きい場合，配列サイズは自動拡張されま
 *  す．<br>
 *  次元番号 dim_index で処理対象とする次元を指定できます．
 *
 * @param     dim_index 次元番号
 * @param     idx_src コピー元の要素番号
 * @param     len コピー元の要素の長さ
 * @param     idx_dst コピー先の要素番号
 * @param     clr コピー元の値のクリア可否
 * @return    自身の参照
 */
fits_image &fits_image::cpy( long dim_index, 
			     long idx_src, long len, long idx_dst, bool clr )
{
    this->data_array().cpy(dim_index, idx_src, len, idx_dst, clr);

    /* setup system header of this HDU */
    this->setup_sys_header();

    return *this;
}

/* replace values between elements */
/* バッファ内での入れ換え */
/**
 * @brief  任意の1つの次元について配列要素間での値の入れ換え
 *
 *  自身の画像データ配列の要素間で値を入れ替えます．<br>
 *  次元番号 dim_index の要素番号 idx_src から len 個分の要素を，要素番号 
 *  idx_dst から len 個分の要素と入れ替えます．<br>
 *  idx_dst + len が配列サイズを超える場合は，配列サイズまでの処理が行われま
 *  す．入れ替える領域が重なった場合，重なっていない src の領域に対してのみ入
 *  れ替え処理が行われます．
 *
 * @param     dim_index 次元番号
 * @param     idx_src 入れ替え元の要素番号
 * @param     len 入れ替え元の要素の長さ
 * @param     idx_dst 入れ替え先の要素番号
 * @return    自身の参照
 */
fits_image &fits_image::swap( long dim_index, 
			      long idx_src, long len, long idx_dst )
{
    this->data_array().swap(dim_index, idx_src, len, idx_dst);
    return *this;
}

/* extract a section           */
/* see also trimf() and trim() */
/**
 * @brief  任意の1つの次元について配列の不要部分の消去
 *
 *  自身の画像データ配列を，要素位置 idx から len 個の要素だけにします．<br>
 *  次元番号 dim_index で処理対象とする次元を指定できます．
 *
 * @param     dim_index 次元番号
 * @param     idx 切り出し開始位置の要素番号
 * @param     len 要素の個数
 * @return    自身の参照
 * @throw     内部バッファの確保に失敗した場合
 */
fits_image &fits_image::crop( long dim_index, long idx, long len )
{
    this->data_array().crop(dim_index, idx, len);

    /* setup system header of this HDU */
    this->setup_sys_header();

    return *this;
}

/* flip a section   */
/* see also flipf() */
/**
 * @brief  任意の1つの次元について配列の並びを反転
 *
 *  自身の画像データ配列の要素位置 idx から len 個の要素を反転させます．<br>
 *  次元番号 dim_index で処理対象とする次元を指定できます．
 *
 * @param     dim_index 次元番号
 * @param     idx 開始位置の要素番号
 * @param     len 要素の個数
 * @return    自身の参照
 */
fits_image &fits_image::flip( long dim_index, long idx, long len )
{
    this->data_array().flip(dim_index, idx, len);
    return *this;
}


/* ------------------------------------------------------------------------- */

static void add_func( double orig_pix_vals[], double src_pix_val, 
		      long n,
		      long col_idx, long row_idx, long layer_idx, 
		      fits_image *objp, void *any )
{
    if ( isfinite(src_pix_val) ) {
	long i;
	for ( i=0 ; i < n ; i++ ) orig_pix_vals[i] += src_pix_val;
    }
    return;
}

static void subtract_func( double orig_pix_vals[], double src_pix_val, 
			   long n,
			   long col_idx, long row_idx, long layer_idx, 
			   fits_image *objp, void *any )
{
    if ( isfinite(src_pix_val) ) {
	long i;
	for ( i=0 ; i < n ; i++ ) orig_pix_vals[i] -= src_pix_val;
    }
    return;
}

static void multiply_func( double orig_pix_vals[], double src_pix_val, 
			   long n,
			   long col_idx, long row_idx, long layer_idx, 
			   fits_image *objp, void *any )
{
    if ( isfinite(src_pix_val) ) {
	long i;
	for ( i=0 ; i < n ; i++ ) orig_pix_vals[i] *= src_pix_val;
    }
    return;
}

static void divide_func( double orig_pix_vals[], double src_pix_val, 
			 long n,
			 long col_idx, long row_idx, long layer_idx, 
			 fits_image *objp, void *any )
{
    if ( isfinite(src_pix_val) ) {
	long i;
	for ( i=0 ; i < n ; i++ ) orig_pix_vals[i] /= src_pix_val;
    }
    return;
}

/* */

static void add_func_n( double orig_pix_vals[], double src_pix_vals[], 
			long n,
			long col_idx, long row_idx, long layer_idx, 
			fits_image *objp, void *any )
{
    long i;
    for ( i=0 ; i < n ; i++ ) {
	if ( isfinite(src_pix_vals[i]) ) 
	    orig_pix_vals[i] += src_pix_vals[i];
    }
    return;
}

static void subtract_func_n( double orig_pix_vals[], double src_pix_vals[], 
			     long n,
			     long col_idx, long row_idx, long layer_idx, 
			     fits_image *objp, void *any )
{
    long i;
    for ( i=0 ; i < n ; i++ ) {
	if ( isfinite(src_pix_vals[i]) ) 
	    orig_pix_vals[i] -= src_pix_vals[i];
    }
    return;
}

static void multiply_func_n( double orig_pix_vals[], double src_pix_vals[], 
			     long n,
			     long col_idx, long row_idx, long layer_idx, 
			     fits_image *objp, void *any )
{
    long i;
    for ( i=0 ; i < n ; i++ ) {
	if ( isfinite(src_pix_vals[i]) ) 
	    orig_pix_vals[i] *= src_pix_vals[i];
    }
    return;
}

static void divide_func_n( double orig_pix_vals[], double src_pix_vals[], 
			   long n,
			   long col_idx, long row_idx, long layer_idx, 
			   fits_image *objp, void *any )
{
    long i;
    for ( i=0 ; i < n ; i++ ) {
	if ( isfinite(src_pix_vals[i]) ) 
	    orig_pix_vals[i] /= src_pix_vals[i];
    }
    return;
}


/* 
 * member functions for image processing using IDL style argument 
 * such as "0:99,*".  The expression is set to exp_fmt argument in 
 * member functions.
 */

/* returns trimmed array          */
/* Flipping elements is supported */
/**
 * @brief  一部要素を切り出した配列をテンポラリオブジェクトで取得 (IDLの記法)
 *
 * 自身の画像データ配列のうち指定された部分だけをテンポラリオブジェクトにコピー
 * し，それを返します．<br>
 * 取り出されるされる範囲は任意の次元まで指定可能で "0:99,*" のように IDL や
 * IRAF と同様の文字列で指定します．この文字列中の要素番号は 0-indexed です．
 *
 * @param   exp_fmt  csv形式の範囲指定文字列のためのフォーマット指定
 * @param   ...      exp_fmtに対応した可変長引数の各要素データ
 * @return  テンポラリオブジェクト
 * @note    "[1:100,*]" のように [] で囲むと，1-indexed として扱われます．
 * @throw   バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 */
fits_image fits_image::sectionf( const char *exp_fmt, ... ) const
{
    fits_image ret;

    va_list ap;
    va_start(ap, exp_fmt);
    try {
	this->vcopyf(&ret, exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vcopyf() failed");
    }
    va_end(ap);

    ret.set_scopy_flag();
    return ret;
}

/**
 * @brief  一部要素を切り出した配列をテンポラリオブジェクトで取得 (IDLの記法)
 *
 * 自身の画像データ配列のうち指定された部分だけをテンポラリオブジェクトにコピー
 * し，それを返します．<br>
 * 取り出されるされる範囲は任意の次元まで指定可能で "0:99,*" のように IDL や
 * IRAF と同様の文字列で指定します．この文字列中の要素番号は 0-indexed です．
 *
 * @param   exp_fmt  csv形式の範囲指定文字列のためのフォーマット指定
 * @param   ap       exp_fmtに対応した可変長引数のリスト
 * @return  テンポラリオブジェクト
 * @note    "[1:100,*]" のように [] で囲むと，1-indexed として扱われます．
 * @throw   バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 */
fits_image fits_image::vsectionf( const char *exp_fmt, va_list ap ) const
{
    fits_image ret;

    this->vcopyf(&ret, exp_fmt, ap);

    ret.set_scopy_flag();
    return ret;
}

/* copy all or a section to another fits_image object */
/* Flipping elements is supported                     */
/**
 * @brief  一部要素を別オブジェクトへコピー (IDLの記法)
 *
 *  自身の画像データ配列の内容の一部分を指定されたオブジェクト dest_img へ
 *  コピーします．<br>
 *  コピーされる範囲は任意の次元まで指定可能で "0:99,*" のように IDL や IRAF と
 *  同様の文字列で指定します．この文字列中の要素番号は 0-indexed です．<br>
 *  コピー先へは，画像データ配列やヘッダの内容，各種属性などすべてをコピーしま
 *  す．自身(コピー元) の配列は改変されません．<br>
 *  dest_img に自身を与えた場合，trimf()を実行した場合と同じ結果になります．
 *
 * @param   dest_img コピー先のオブジェクト
 * @param   exp_fmt  csv形式の範囲指定文字列のためのフォーマット指定
 * @param   ...      exp_fmtに対応した可変長引数の各要素データ
 * @note    "[1:100,*]" のように [] で囲むと，1-indexed として扱われます．
 * @throw   バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 */
void fits_image::copyf( fits_image *dest_img, const char *exp_fmt, ... ) const
{
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	this->vcopyf(dest_img, exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vcopyf() failed");
    }
    va_end(ap);
    return;
}

/**
 * @brief  一部要素を別オブジェクトへコピー (IDLの記法)
 *
 *  自身の画像データ配列の内容の一部分を指定されたオブジェクト dest_img へ
 *  コピーします．<br>
 *  コピーされる範囲は任意の次元まで指定可能で "0:99,*" のように IDL や IRAF と
 *  同様の文字列で指定します．この文字列中の要素番号は 0-indexed です．<br>
 *  コピー先へは，画像データ配列やヘッダの内容，各種属性などすべてをコピーしま
 *  す．自身(コピー元) の配列は改変されません．<br>
 *  dest_img に自身を与えた場合，trimf()を実行した場合と同じ結果になります．
 *
 * @param   dest_img コピー先のオブジェクト
 * @param   exp_fmt  csv形式の範囲指定文字列のためのフォーマット指定
 * @param   ap       exp_fmtに対応した可変長引数のリスト
 * @note    "[1:100,*]" のように [] で囲むと，1-indexed として扱われます．
 * @throw   バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 */
void fits_image::vcopyf( fits_image *dest_img, 
			 const char *exp_fmt, va_list ap ) const
{
    if ( dest_img == NULL ) goto quit;

    /* 注意: コピーは dest_img が自身の場合でも行なわれる */

    /* 相手の型が異なる場合 */
    if ( dest_img->type() != this->type() ) {
	dest_img->_init_all_data_recs(this->data_array().size_type());
	dest_img->type_rec = this->type_rec;
    }

    this->data_array().vcopyf( dest_img->data_rec, exp_fmt, ap );

    if ( dest_img != this ) {
	/* bzero, bscale などの属性をコピー */
	dest_img->init_properties(*this);
	/* ヘッダをコピー */
	dest_img->header_init( this->header() );
    }

 quit:
    return;
}

/* trim a section                 */
/* Flipping elements is supported */
/**
 * @brief  指定部分以外の配列要素の消去 (画像データのトリミング・IDLの記法)
 *
 *  自身の画像データ配列のうち指定された部分だけを残し，指定外の部分を消去
 *  します．<br>
 *  切り出されるされる範囲は任意の次元まで指定可能で "0:99,*" のように IDL や
 *  IRAF と同様の文字列で指定します．この文字列中の要素番号は 0-indexed です．
 *
 * @param   exp_fmt  csv形式の範囲指定文字列のためのフォーマット指定
 * @param   ...      exp_fmtに対応した可変長引数の各要素データ
 * @return  自身の参照
 * @note    "[1:100,*]" のように [] で囲むと，1-indexed として扱われます．
 * @throw   バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 */
fits_image &fits_image::trimf( const char *exp_fmt, ... )
{
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	this->vtrimf(exp_fmt, ap);
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
 *  自身の画像データ配列のうち指定された部分だけを残し，指定外の部分を消去
 *  します．<br>
 *  切り出されるされる範囲は任意の次元まで指定可能で "0:99,*" のように IDL や
 *  IRAF と同様の文字列で指定します．この文字列中の要素番号は 0-indexed です．
 *
 * @param   exp_fmt  csv形式の範囲指定文字列のためのフォーマット指定
 * @param   ap       exp_fmtに対応した可変長引数のリスト
 * @return  自身の参照
 * @note    "[1:100,*]" のように [] で囲むと，1-indexed として扱われます．
 * @throw   バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 */
fits_image &fits_image::vtrimf( const char *exp_fmt, va_list ap )
{
    this->vcopyf(this, exp_fmt, ap);
    return *this;
}

/* flip elements in a section */
/**
 * @brief  任意の次元(複数も可)で画像データ配列の並びを反転 (IDLの記法)
 */
fits_image &fits_image::flipf( const char *exp_fmt, ... )
{
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	this->vflipf(exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vflipf() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  任意の次元(複数も可)で画像データ配列の並びを反転 (IDLの記法)
 */
fits_image &fits_image::vflipf( const char *exp_fmt, va_list ap )
{
    this->data_array().vflipf(exp_fmt,ap);
    return *this;
}

/* interchange rows and columns and copy */
/* Flipping elements is supported        */
/**
 * @brief  (x,y)のトランスポーズを行なった画像データ配列を取得 (IDLの記法)
 */
void fits_image::transposef_xy_copy( fits_image *dest_img, const char *exp_fmt, ... ) const
{
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	this->vtransposef_xy_copy(dest_img, exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vtransposef_xy_copy() failed");
    }
    va_end(ap);
    return;
}

/**
 * @brief  (x,y)のトランスポーズを行なった画像データ配列を取得 (IDLの記法)
 */
void fits_image::vtransposef_xy_copy( fits_image *dest_img, 
				      const char *exp_fmt, va_list ap ) const
{
    if ( dest_img == NULL ) goto quit;

    /* 注意: dest_img が自身の場合でも行なわれる */

    /* 相手の型が異なる場合 */
    if ( dest_img->type() != this->type() ) {
	dest_img->_init_all_data_recs(this->data_array().size_type());
	dest_img->type_rec = this->type_rec;
    }

    this->data_array().vtransposef_xy_copy( dest_img->data_rec, exp_fmt, ap );

    if ( dest_img != this ) {
	/* bzero, bscale などの属性をコピー */
	dest_img->init_properties(*this);
	/* ヘッダをコピー */
	dest_img->header_init( this->header() );
    }

 quit:
    return;
}

/* interchange xyz to zxy and copy */
/* Flipping elements is supported  */
/**
 * @brief  (x,y,z)→(z,x,y)のトランスポーズを行なった配列を取得 (IDLの記法)
 */
void fits_image::transposef_xyz2zxy_copy( fits_image *dest_img, const char *exp_fmt, ... ) const
{
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	this->vtransposef_xyz2zxy_copy(dest_img, exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vtransposef_xyz2zxy_copy() failed");
    }
    va_end(ap);
    return;
}

/**
 * @brief  (x,y,z)→(z,x,y)のトランスポーズを行なった配列を取得 (IDLの記法)
 */
void fits_image::vtransposef_xyz2zxy_copy( fits_image *dest_img, 
				      const char *exp_fmt, va_list ap ) const
{
    if ( dest_img == NULL ) goto quit;

    /* 注意: dest_img が自身の場合でも行なわれる */

    /* 相手の型が異なる場合 */
    if ( dest_img->type() != this->type() ) {
	dest_img->_init_all_data_recs(this->data_array().size_type());
	dest_img->type_rec = this->type_rec;
    }

    this->data_array().vtransposef_xyz2zxy_copy(dest_img->data_rec, exp_fmt, ap);

    if ( dest_img != this ) {
	/* bzero, bscale などの属性をコピー */
	dest_img->init_properties(*this);
	/* ヘッダをコピー */
	dest_img->header_init( this->header() );
    }

 quit:
    return;
}

/* padding existing values in an array */
/**
 * @brief  自身の画像データ配列をデフォルト値でパディング (IDLの記法)
 */
fits_image &fits_image::cleanf( const char *exp_fmt, ... )
{
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	this->vcleanf(exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vcleanf() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  自身の画像データ配列をデフォルト値でパディング (IDLの記法)
 */
fits_image &fits_image::vcleanf( const char *exp_fmt, va_list ap )
{
    this->data_array().vcleanf(exp_fmt,ap);
    return *this;
}

/* rewrite element values with a value in a section */
/**
 * @brief  自身の画像データ配列を指定されたスカラー値で書き換え (IDLの記法)
 */
fits_image &fits_image::fillf( double value, const char *exp_fmt, ... )
{
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	this->vfillf(value, exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vfillf() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  自身の画像データ配列を指定されたスカラー値で書き換え (IDLの記法)
 */
fits_image &fits_image::vfillf( double value, 
				const char *exp_fmt, va_list ap )
{
    return this->image_vfillf_fast( value, exp_fmt, ap );
}

/**
 * @brief  自身のデータ配列をスカラー値で書き換え (ユーザ関数経由・IDLの記法)
 */
fits_image &fits_image::fillf_via_udf( double value, 
			     void (*func)(double [],double,long, 
					  long,long,long,fits_image *,void *),
			     void *user_ptr, const char *exp_fmt, ... )
{
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	this->vfillf_via_udf(value, func, user_ptr, exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vfillf_via_udf() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  自身のデータ配列をスカラー値で書き換え (ユーザ関数経由・IDLの記法)
 */
fits_image &fits_image::vfillf_via_udf( double value, 
       void (*func)(double [],double,long, long,long,long,fits_image *,void *),
       void *user_ptr, const char *exp_fmt, va_list ap )
{
    if ( func == NULL ) {
	return this->image_vfillf_fast( value, exp_fmt, ap );
    }
    else {
	return this->image_vfillf( value, func, user_ptr, exp_fmt, ap );
    }
}

/* add a scalar value to element values in a section */
/**
 * @brief  自身の画像データ配列に，指定されたスカラー値で加算 (IDLの記法)
 */
fits_image &fits_image::addf( double value, 
			      const char *exp_fmt, ... )
{
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	this->vaddf(value, exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vaddf() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  自身の画像データ配列に，指定されたスカラー値で加算 (IDLの記法)
 */
fits_image &fits_image::vaddf( double value, 
			       const char *exp_fmt, va_list ap )
{
    return this->image_vfillf( value, &add_func, NULL, exp_fmt, ap );
}

/* subtract a scalar value from element values in a section */
/**
 * @brief  自身の画像データ配列を，指定されたスカラー値で減算 (IDLの記法)
 */
fits_image &fits_image::subtractf( double value, 
				   const char *exp_fmt, ... )
{
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	this->vsubtractf(value, exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vsubtractf() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  自身の画像データ配列を，指定されたスカラー値で減算 (IDLの記法)
 */
fits_image &fits_image::vsubtractf( double value, 
				    const char *exp_fmt, va_list ap )
{
    return this->image_vfillf( value, &subtract_func, NULL, exp_fmt, ap );
}

/* multiply element values in a section by a scalar value */
/**
 * @brief  自身の画像データ配列に，指定されたスカラー値を乗算 (IDLの記法)
 */
fits_image &fits_image::multiplyf( double value, 
				   const char *exp_fmt, ... )
{
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	this->vmultiplyf(value, exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vmultiplyf() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  自身の画像データ配列に，指定されたスカラー値を乗算 (IDLの記法)
 */
fits_image &fits_image::vmultiplyf( double value, 
				    const char *exp_fmt, va_list ap )
{
    return this->image_vfillf( value, &multiply_func, NULL, exp_fmt, ap );
}

/* divide element values in a section by a scalar value */
/**
 * @brief  自身の画像データ配列を，指定されたスカラー値で除算 (IDLの記法)
 */
fits_image &fits_image::dividef( double value, 
				 const char *exp_fmt, ... )
{
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	this->vdividef(value, exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vdividef() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  自身の画像データ配列を，指定されたスカラー値で除算 (IDLの記法)
 */
fits_image &fits_image::vdividef( double value, 
				  const char *exp_fmt, va_list ap )
{
    return this->image_vfillf( value, &divide_func, NULL, exp_fmt, ap );
}

/* paste up an array object                                           */

/* paste without operation using fast method of that of .convert().   */
/* 演算なしペースト: convert() と同じ手法で変換した結果を高速に格納． */
/**
 * @brief  自身のデータ配列に指定されたオブジェクトの配列を貼り付け (IDLの記法)
 */
fits_image &fits_image::pastef( const fits_image &src,
				const char *exp_fmt, ... )
{
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	this->vpastef(src,exp_fmt,ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vpastef() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  自身のデータ配列に指定されたオブジェクトの配列を貼り付け (IDLの記法)
 */
fits_image &fits_image::vpastef( const fits_image &src,
				 const char *exp_fmt, va_list ap )
{
    return this->image_vpastef_fast(src, exp_fmt, ap);
}

/* paste with operation: all elements are converted into double type, so */
/* the performance is inferior to above paste().                         */
/* 演算ありペースト: 一旦 double 型に変換されてから演算した結果を格納．  */
/* 当然，↑の演算なし版に比べて遅い．                                    */
/* arguments of func are                                                 */
/*   1. prepared temporary buffer that has converted elements from self; */
/*      this array is the destination for programmer's calculation.      */
/*   2. prepared temporary buffer that have converted elements from src  */
/*   3. length of buffer of 1. or 2. (always column length of region)    */
/*   4. position of x (always first position)                            */
/*   5. position of y                                                    */
/*   6. position of z                                                    */
/*   7. address of self                                                  */
/*   8. user pointer                                                     */
/**
 * @brief  自身の配列に指定された配列を貼り付け (ユーザ関数経由・IDLの記法)
 */
fits_image &fits_image::pastef_via_udf( const fits_image &src,
			      void (*func)(double [],double [],long, 
					   long,long,long,fits_image *,void *),
			      void *user_ptr, const char *exp_fmt, ... )
{
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	this->vpastef_via_udf(src,func,user_ptr,exp_fmt,ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vpastef_via_udf() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  自身の配列に指定された配列を貼り付け (ユーザ関数経由・IDLの記法)
 */
fits_image &fits_image::vpastef_via_udf( const fits_image &src,
			     void (*func)(double [],double [],long, 
					  long,long,long,fits_image *,void *),
			     void *user_ptr, const char *exp_fmt, va_list ap )
{
    if ( func == NULL ) {
	return this->image_vpastef_fast(src, exp_fmt, ap);
    } else {
	return this->image_vpastef(src, func, user_ptr, exp_fmt, ap);
    }
}

/* add an array object */
/**
 * @brief  自身のデータ配列に，指定されたオブジェクトの配列を加算 (IDLの記法)
 */
fits_image &fits_image::addf( const fits_image &src_img, 
			      const char *exp_fmt, ... )
{
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	this->vaddf(src_img, exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vaddf() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  自身のデータ配列に，指定されたオブジェクトの配列を加算 (IDLの記法)
 */
fits_image &fits_image::vaddf( const fits_image &src_img, 
			       const char *exp_fmt, va_list ap )
{
    return this->image_vpastef(src_img, &add_func_n, NULL, exp_fmt, ap);
}

/* subtract an array object */
/**
 * @brief  自身のデータ配列を，指定されたオブジェクトの配列で減算 (IDLの記法)
 */
fits_image &fits_image::subtractf( const fits_image &src_img, 
				   const char *exp_fmt, ... )
{
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	this->vsubtractf(src_img, exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vsubtractf() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  自身のデータ配列を，指定されたオブジェクトの配列で減算 (IDLの記法)
 */
fits_image &fits_image::vsubtractf( const fits_image &src_img, 
				    const char *exp_fmt, va_list ap )
{
    return this->image_vpastef(src_img, &subtract_func_n, NULL, exp_fmt, ap);
}

/* multiply an array object */
/**
 * @brief  自身のデータ配列に，指定されたオブジェクトの配列を乗算 (IDLの記法)
 */
fits_image &fits_image::multiplyf( const fits_image &src_img, 
				   const char *exp_fmt, ... )
{
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	this->vmultiplyf(src_img, exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vmultiplyf() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  自身のデータ配列に，指定されたオブジェクトの配列を乗算 (IDLの記法)
 */
fits_image &fits_image::vmultiplyf( const fits_image &src_img, 
				    const char *exp_fmt, va_list ap )
{
    return this->image_vpastef(src_img, &multiply_func_n, NULL, exp_fmt, ap);
}

/* divide an array object */
/**
 * @brief  自身のデータ配列を，指定されたオブジェクトの配列で除算 (IDLの記法)
 */
fits_image &fits_image::dividef( const fits_image &src_img, 
				 const char *exp_fmt, ... )
{
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	this->vdividef(src_img, exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vdividef() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  自身のデータ配列を，指定されたオブジェクトの配列で除算 (IDLの記法)
 */
fits_image &fits_image::vdividef( const fits_image &src_img, 
				  const char *exp_fmt, va_list ap )
{
    return this->image_vpastef(src_img, &divide_func_n, NULL, exp_fmt, ap);
}


/* 
 * member functions for image manipulation and operations.
 */

/* returns trimmed array */
/**
 * @brief  画像データ配列の一部要素を切り出し，テンポラリオブジェクトで取得
 *
 *  自身の画像データ配列のうち指定された部分だけをテンポラリオブジェクトに
 *  コピーし，それを返します．
 *
 * @param     col_index コピー元の列位置
 * @param     col_size コピー元の列サイズ
 * @param     row_index コピー元の行位置
 * @param     row_size コピー元の行サイズ
 * @param     layer_index コピー元のレイヤ位置
 * @param     layer_size コピー元のレイヤサイズ
 * @return    テンポラリオブジェクト
 * @throw     バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 */
fits_image fits_image::section( long col_index, long col_size, 
				long row_index, long row_size,
				long layer_index, long layer_size ) const
{
    fits_image ret;

    this->copy( &ret, col_index, col_size,
		row_index, row_size, layer_index, layer_size );

    ret.set_scopy_flag();
    return ret;
}

/* copy all or a region to another fits_image object */
/**
 * @brief  画像データ配列の一部要素を別オブジェクトへコピー
 *
 *  自身の画像データ配列の内容の一部分を指定されたオブジェクト dest_img へ
 *  コピーします．<br>
 *  コピー先へは，画像データ配列やヘッダの内容，各種属性などすべてをコピーしま
 *  す．自身(コピー元) の配列は改変されません．<br>
 *  dest_img に自身を与えた場合，trim()を実行した場合と同じ結果になります．
 *
 * @param     dest_img コピー先のオブジェクト
 * @param     col_index コピー元の列位置
 * @param     col_size コピー元の列サイズ
 * @param     row_index コピー元の行位置
 * @param     row_size コピー元の行サイズ
 * @param     layer_index コピー元のレイヤ位置
 * @param     layer_size コピー元のレイヤサイズ
 * @throw     バッファの確保に失敗した場合．メモリ破壊を起こした場合．
 */
void fits_image::copy( fits_image *dest_img,
		       long col_index, long col_size, 
		       long row_index, long row_size,
		       long layer_index, long layer_size ) const
{
    size_t z_col_size = fits_seclen_to_mdarray_seclen(col_size);
    size_t z_row_size = fits_seclen_to_mdarray_seclen(row_size);
    size_t z_layer_size = fits_seclen_to_mdarray_seclen(layer_size);

    if ( dest_img == NULL ) goto quit;

    /* 注意: dest_img が自身の場合でも行なわれる */

    /* 相手の型が異なる場合 */
    if ( dest_img->type() != this->type() ) {
	dest_img->_init_all_data_recs(this->data_array().size_type());
	dest_img->type_rec = this->type_rec;
    }

    this->data_array().copy( dest_img->data_rec, col_index, z_col_size, 
			  row_index, z_row_size, layer_index, z_layer_size );

    if ( dest_img != this ) {
	/* bzero, bscale などの属性をコピー */
	dest_img->init_properties(*this);
	/* ヘッダをコピー */
	dest_img->header_init( this->header() );
    }

 quit:
    return;
}

/* set a section to be copied by move_to() */
/**
 * @brief  次回の move_to() により画像データを移動させる領域の指定
 */
fits_image &fits_image::move_from( long col_idx, long col_len,
				   long row_idx, long row_len, 
				   long layer_idx, long layer_len )
{
    size_t z_col_size = fits_seclen_to_mdarray_seclen(col_len);
    size_t z_row_size = fits_seclen_to_mdarray_seclen(row_len);
    size_t z_layer_size = fits_seclen_to_mdarray_seclen(layer_len);

    this->data_array().move_from( col_idx, z_col_size, 
			       row_idx, z_row_size, layer_idx, z_layer_size );

    return *this;
}

/* copy a section specified by move_from() */
/**
 * @brief  move_from() で指定された画像データ領域の移動
 */
fits_image &fits_image::move_to(long dest_col, long dest_row, long dest_layer)
{
    this->data_array().move_to(dest_col, dest_row, dest_layer);
    return *this;
}

/* trim a section */
/**
 * @brief  指定部分以外の画像データ配列要素の消去 (画像データのトリミング)
 *
 *  自身の画像データ配列のうち指定された部分だけを残し，指定外の部分を
 *  消去します．<br>
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
fits_image &fits_image::trim( long col_idx, long col_len,
			      long row_idx, long row_len, 
			      long layer_idx, long layer_len )
{
    size_t z_col_size = fits_seclen_to_mdarray_seclen(col_len);
    size_t z_row_size = fits_seclen_to_mdarray_seclen(row_len);
    size_t z_layer_size = fits_seclen_to_mdarray_seclen(layer_len);

    this->data_array().trim( col_idx, z_col_size, 
			     row_idx, z_row_size,
			     layer_idx, z_layer_size );

    /* setup system header of this HDU */
    this->setup_sys_header();

    return *this;
}

/* flip horizontal within the specified section */
/**
 * @brief  画像データ配列の水平方向での反転
 *
 *  自身の画像データ配列の指定された範囲の要素について，その内容を水平方向に
 *  反転させます．
 *
 * @param     col_idx 列位置
 * @param     col_len 列サイズ
 * @param     row_idx 行位置
 * @param     row_len 行サイズ
 * @param     layer_idx レイヤ位置
 * @param     layer_len レイヤサイズ
 * @return    自身の参照
 */
fits_image &fits_image::flip_cols( long col_idx, long col_len,
				   long row_idx, long row_len, 
				   long layer_idx, long layer_len )
{
    size_t z_col_size = fits_seclen_to_mdarray_seclen(col_len);
    size_t z_row_size = fits_seclen_to_mdarray_seclen(row_len);
    size_t z_layer_size = fits_seclen_to_mdarray_seclen(layer_len);

    this->data_array().flip_cols( col_idx, z_col_size, 
				  row_idx, z_row_size,
				  layer_idx, z_layer_size );

    return *this;
}

/* flip vertical within the specified section */
/**
 * @brief  画像データ配列の垂直方向での反転
 *
 *  自身の画像データ配列の指定された範囲の要素について，その内容を垂直方向に
 *  反転させます．
 *
 * @param     col_idx 列位置
 * @param     col_len 列サイズ
 * @param     row_idx 行位置
 * @param     row_len 行サイズ
 * @param     layer_idx レイヤ位置
 * @param     layer_len レイヤサイズ
 * @return    自身の参照
 */
fits_image &fits_image::flip_rows( long col_idx, long col_len,
				   long row_idx, long row_len, 
				   long layer_idx, long layer_len )
{
    size_t z_col_size = fits_seclen_to_mdarray_seclen(col_len);
    size_t z_row_size = fits_seclen_to_mdarray_seclen(row_len);
    size_t z_layer_size = fits_seclen_to_mdarray_seclen(layer_len);

    this->data_array().flip_rows( col_idx, z_col_size, 
				  row_idx, z_row_size,
				  layer_idx, z_layer_size );

    return *this;
}

/* interchange rows and columns and copy */
/**
 * @brief  (x,y)でのトランスポーズを行なった画像データを取得
 */
void fits_image::transpose_xy_copy( fits_image *dest_img,
				    long col_idx, long col_size, 
				    long row_idx, long row_size,
				    long layer_idx, long layer_size ) const
{
    size_t z_col_size = fits_seclen_to_mdarray_seclen(col_size);
    size_t z_row_size = fits_seclen_to_mdarray_seclen(row_size);
    size_t z_layer_size = fits_seclen_to_mdarray_seclen(layer_size);

    if ( dest_img == NULL ) goto quit;

    /* 注意: dest_img が自身の場合でも行なわれる */

    /* 相手の型が異なる場合 */
    if ( dest_img->type() != this->type() ) {
	dest_img->_init_all_data_recs(this->data_array().size_type());
	dest_img->type_rec = this->type_rec;
    }

    this->data_array().transpose_xy_copy(dest_img->data_rec, 
	    col_idx, z_col_size, row_idx, z_row_size, layer_idx, z_layer_size);

    if ( dest_img != this ) {
	/* bzero, bscale などの属性をコピー */
	dest_img->init_properties(*this);
	/* ヘッダをコピー */
	dest_img->header_init( this->header() );
    }
    else {
	fits_image *thisp = (fits_image *)this;
	/* setup system header of this HDU */
	thisp->setup_sys_header();
    }

 quit:
    return;
}

/* interchange xyz to zxy and copy */
/**
 * @brief  (x,y,z)→(z,x,y)のトランスポーズを行なった画像データを取得
 */
void fits_image::transpose_xyz2zxy_copy( fits_image *dest_img,
					long col_idx, long col_size, 
					long row_idx, long row_size,
					long layer_idx, long layer_size ) const
{
    size_t z_col_size = fits_seclen_to_mdarray_seclen(col_size);
    size_t z_row_size = fits_seclen_to_mdarray_seclen(row_size);
    size_t z_layer_size = fits_seclen_to_mdarray_seclen(layer_size);

    if ( dest_img == NULL ) goto quit;

    /* 注意: dest_img が自身の場合でも行なわれる */

    /* 相手の型が異なる場合 */
    if ( dest_img->type() != this->type() ) {
	dest_img->_init_all_data_recs(this->data_array().size_type());
	dest_img->type_rec = this->type_rec;
    }

    this->data_array().transpose_xyz2zxy_copy(dest_img->data_rec, 
	    col_idx, z_col_size, row_idx, z_row_size, layer_idx, z_layer_size);

    if ( dest_img != this ) {
	/* bzero, bscale などの属性をコピー */
	dest_img->init_properties(*this);
	/* ヘッダをコピー */
	dest_img->header_init( this->header() );
    }
    else {
	fits_image *thisp = (fits_image *)this;
	/* setup system header of this HDU */
	thisp->setup_sys_header();
    }

 quit:
    return;
}

/* rotate and copy a region to another fits_image object */
/**
 * @brief  (x,y)で回転させた画像データを取得 (回転は90度単位)
 */
void fits_image::rotate_xy_copy( fits_image *dest_img, int angle,
				 long col_idx, long col_size, 
				 long row_idx, long row_size,
				 long layer_idx, long layer_size ) const
{
    size_t z_col_size = fits_seclen_to_mdarray_seclen(col_size);
    size_t z_row_size = fits_seclen_to_mdarray_seclen(row_size);
    size_t z_layer_size = fits_seclen_to_mdarray_seclen(layer_size);

    if ( dest_img == NULL ) goto quit;

    /* 注意: dest_img が自身の場合でも行なわれる */

    /* 相手の型が異なる場合 */
    if ( dest_img->type() != this->type() ) {
	dest_img->_init_all_data_recs(this->data_array().size_type());
	dest_img->type_rec = this->type_rec;
    }

    this->data_array().rotate_xy_copy(dest_img->data_rec, angle,
	    col_idx, z_col_size, row_idx, z_row_size, layer_idx, z_layer_size);

    if ( dest_img != this ) {
	/* bzero, bscale などの属性をコピー */
	dest_img->init_properties(*this);
	/* ヘッダをコピー */
	dest_img->header_init( this->header() );
    }
    else {
	fits_image *thisp = (fits_image *)this;
	/* setup system header of this HDU */
	thisp->setup_sys_header();
    }

 quit:
    return;
}

/* padding existing elements with default value within specified section */
/**
 * @brief  自身の画像データ配列をデフォルト値でパディング
 *
 *  自身の画像データ配列の要素をデフォルト値でパディングします．<br>
 *  引数は指定しなくても使用できます．その場合は，全要素が処理対象です．
 *  clean() を実行しても配列長は変化しません．
 *
 * @param     col_idx 列位置
 * @param     col_len 列サイズ
 * @param     row_idx 行位置
 * @param     row_len 行サイズ
 * @param     layer_idx レイヤ位置
 * @param     layer_len レイヤサイズ
 * @return    自身の参照
 */
fits_image &fits_image::clean( long col_idx, long col_len,
			       long row_idx, long row_len, 
			       long layer_idx, long layer_len )
{
    size_t z_col_size = fits_seclen_to_mdarray_seclen(col_len);
    size_t z_row_size = fits_seclen_to_mdarray_seclen(row_len);
    size_t z_layer_size = fits_seclen_to_mdarray_seclen(layer_len);

    this->data_array().clean( 
	   col_idx, z_col_size, row_idx, z_row_size, layer_idx, z_layer_size );

    return *this;
}

/* */

/* rewrite element values with a scalar value in a rectangular section */
/**
 * @brief  自身の画像データ配列を指定されたスカラー値で書き換え
 *
 *  自身の画像データ配列の指定された範囲の要素を，指定された値で書換えます．
 *
 * @param     value 書き込む値
 * @param     col_index 列位置
 * @param     col_size 列サイズ
 * @param     row_index 行位置
 * @param     row_size 行サイズ
 * @param     layer_index レイヤ位置
 * @param     layer_size レイヤサイズ
 * @return    自身の参照
 */
fits_image &fits_image::fill( double value, long col_index, long col_size, 
			      long row_index, long row_size,
			      long layer_index, long layer_size )
{
    return this->image_fill_fast( value, 
				  col_index, col_size, row_index, row_size,
				  layer_index, layer_size );
}

/**
 * @brief  自身の画像データ配列を指定されたスカラー値で書き換え(ユーザ関数経由)
 *
 *  自身の画像データ配列の指定された範囲の要素を，ユーザ定義関数経由で書換え
 *  ます．
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
fits_image &fits_image::fill_via_udf( double value, 
       void (*func)(double [],double,long, long,long,long,fits_image *,void *),
	 void *user_ptr,
	 long col_index, long col_size, 
	 long row_index, long row_size,
	 long layer_index, long layer_size )
{
    if ( func == NULL ) {
	return this->image_fill_fast( value, 
				      col_index, col_size, row_index, row_size,
				      layer_index, layer_size );
    }
    else {
	return this->image_fill( value, func, user_ptr,
				 col_index, col_size, row_index, row_size,
				 layer_index, layer_size );
    }
}

/* */

/**
 * @brief  自身の画像データ配列に，指定されたスカラー値を加算
 *
 *  自身の画像データ配列の指定された範囲の要素に value を加算します．
 *
 * @param     value 加算する値
 * @param     col_index 列位置
 * @param     col_size 列サイズ
 * @param     row_index 行位置
 * @param     row_size 行サイズ
 * @param     layer_index レイヤ位置
 * @param     layer_size レイヤサイズ
 * @return    自身の参照
 */
fits_image &fits_image::add( double value, long col_index, long col_size, 
			     long row_index, long row_size,
			     long layer_index, long layer_size )
{
    return this->image_fill( value, &add_func, NULL,
			     col_index, col_size, row_index, row_size,
			     layer_index, layer_size );
}

/**
 * @brief  自身の画像データ配列を，指定されたスカラー値で減算
 *
 *  自身の画像データ配列の指定された範囲の要素から value を減算します．
 *
 * @param     value 減算する値
 * @param     col_index 列位置
 * @param     col_size 列サイズ
 * @param     row_index 行位置
 * @param     row_size 行サイズ
 * @param     layer_index レイヤ位置
 * @param     layer_size レイヤサイズ
 * @return    自身の参照
 */
fits_image &fits_image::subtract( double value, long col_index, long col_size, 
			     long row_index, long row_size,
			     long layer_index, long layer_size )
{
    return this->image_fill( value, &subtract_func, NULL,
			     col_index, col_size, row_index, row_size,
			     layer_index, layer_size );
}

/**
 * @brief  自身の画像データ配列に，指定されたスカラー値を乗算
 *
 *  自身の画像データ配列の指定された範囲の要素の値に value を乗算します．
 *
 * @param     value 乗算する値
 * @param     col_index 列位置
 * @param     col_size 列サイズ
 * @param     row_index 行位置
 * @param     row_size 行サイズ
 * @param     layer_index レイヤ位置
 * @param     layer_size レイヤサイズ
 * @return    自身の参照
 */
fits_image &fits_image::multiply( double value, long col_index, long col_size, 
				  long row_index, long row_size,
				  long layer_index, long layer_size )
{
    return this->image_fill( value, &multiply_func, NULL,
			     col_index, col_size, row_index, row_size,
			     layer_index, layer_size );
}

/**
 * @brief  自身の画像データ配列を，指定されたスカラー値で除算
 *
 *  自身の画像データ配列の指定された範囲の要素の値について value で除算します．
 *
 * @param     value 除算する値
 * @param     col_index 列位置
 * @param     col_size 列サイズ
 * @param     row_index 行位置
 * @param     row_size 行サイズ
 * @param     layer_index レイヤ位置
 * @param     layer_size レイヤサイズ
 * @return    自身の参照
 */
fits_image &fits_image::divide( double value, long col_index, long col_size, 
				long row_index, long row_size,
				long layer_index, long layer_size )
{
    return this->image_fill( value, &divide_func, NULL,
			     col_index, col_size, row_index, row_size,
			     layer_index, layer_size );
}

/**
 * @brief  自身の画像データ配列に指定されたオブジェクトの配列を貼り付け
 *
 *  自身の画像データ配列の指定された範囲の要素値に，src_img で指定された
 *  オブジェクトの各要素値を貼り付けます．
 *
 * @param     src_img 源泉となる配列を持つオブジェクト
 * @param     dest_col 列位置
 * @param     dest_row 行位置
 * @param     dest_layer レイヤ位置
 * @return    自身の参照
 */
fits_image &fits_image::paste( const fits_image &src_img,
			       long dest_col, long dest_row, long dest_layer )
{
    return this->image_paste_fast(src_img, dest_col, dest_row, dest_layer);
}

/**
 * @brief  自身の配列に指定されたオブジェクトの配列を貼り付け (ユーザ関数経由)
 *
 *  自身の画像データ配列の指定された範囲の要素値に，src_img で指定された
 *  オブジェクトの 各要素値をユーザ定義関数経由で貼り付けます．ユーザ定義関数
 *  を与えて貼り付け時の挙動を変えることができます．<br>
 *  ユーザ定義関数 func の引数には順に，自身の要素値(配列)，オブジェクト 
 *  src_img の要素値(配列)，最初の2つの引数の配列の個数，列位置，行位置，
 *  レイヤ位置，自身のオブジェクトのアドレス，ユーザポインタ user_ptr の値
 *  が与えられます．
 *
 * @param     src_img 源泉となる配列を持つオブジェクト
 * @param     func 値変換のためのユーザ関数のアドレス
 * @param     user_ptr func の最後に与えられるユーザのポインタ
 * @param     dest_col 列位置
 * @param     dest_row 行位置
 * @param     dest_layer レイヤ位置
 * @return    自身の参照
 */
fits_image &fits_image::paste_via_udf( const fits_image &src_img,
    void (*func)(double [],double [],long, long,long,long,fits_image *,void *),
    void *user_ptr, long dest_col, long dest_row, long dest_layer )
{
    if ( func == NULL ) {
	return this->image_paste_fast(src_img, dest_col, dest_row, dest_layer);
    } else {
	return this->image_paste(src_img, func, user_ptr,
				 dest_col, dest_row, dest_layer);
    }
}

/**
 * @brief  自身の画像データ配列に，指定されたオブジェクトの配列を加算
 *
 *  自身の画像データ配列の要素にオブジェクト src_img が持つ配列を加算します．
 *  列・行・レイヤについてそれぞれの加算適用開始位置を指定できます．
 *
 * @param     src_img 演算に使う配列を持つオブジェクト
 * @param     dest_col 加算開始位置(列)
 * @param     dest_row 加算開始位置(行)
 * @param     dest_layer 加算開始位置(レイヤ)
 * @return    自身の参照
 */
fits_image &fits_image::add( const fits_image &src_img,
			     long dest_col, long dest_row, long dest_layer )
{
    return this->image_paste(src_img,&add_func_n,NULL,
			     dest_col,dest_row,dest_layer);
}

/**
 * @brief  自身の画像データ配列を，指定されたオブジェクトの配列で減算
 *
 *  自身の画像データ配列の要素値からオブジェクト src_img が持つ配列の要素値を
 *  減算します．
 *  列・行・レイヤについてそれぞれの減算適用開始位置を指定できます．
 *
 * @param     src_img 演算に使う配列を持つオブジェクト
 * @param     dest_col 減算開始位置(列)
 * @param     dest_row 減算開始位置(行)
 * @param     dest_layer 減算開始位置(レイヤ)
 * @return    自身の参照
 */
fits_image &fits_image::subtract( const fits_image &src_img,
				long dest_col, long dest_row, long dest_layer )
{
    return this->image_paste(src_img,&subtract_func_n,NULL,
			     dest_col,dest_row,dest_layer);
}

/**
 * @brief  自身の画像データ配列に，指定されたオブジェクトの配列を乗算
 *
 *  自身の画像データ配列の要素値にオブジェクト src_img が持つ配列を乗算します．
 *  列・行・レイヤについてそれぞれの乗算適用開始位置を指定できます．
 *
 * @param     src_img 演算に使う配列を持つオブジェクト
 * @param     dest_col 乗算開始位置(列)
 * @param     dest_row 乗算開始位置(行)
 * @param     dest_layer 乗算開始位置(レイヤ)
 * @return    自身の参照
 */
fits_image &fits_image::multiply( const fits_image &src_img,
				long dest_col, long dest_row, long dest_layer )
{
    return this->image_paste(src_img,&multiply_func_n,NULL,
			     dest_col,dest_row,dest_layer);
}

/**
 * @brief  自身の画像データ配列を，指定されたオブジェクトの配列で除算
 *
 *  自身の画像データ配列の要素値からオブジェクト src_img が持つ配列を除算
 *  します．
 *  列・行・レイヤについてそれぞれの除算適用開始位置を指定できます．
 *
 * @param     src_img 演算に使う配列を持つオブジェクト
 * @param     dest_col 除算開始位置(列)
 * @param     dest_row 除算開始位置(行)
 * @param     dest_layer 除算開始位置(レイヤ)
 * @return    自身の参照
 */
fits_image &fits_image::divide( const fits_image &src_img,
				long dest_col, long dest_row, long dest_layer )
{
    return this->image_paste(src_img,&divide_func_n,NULL,
			     dest_col,dest_row,dest_layer);
}


/* 
 * member functions for image analysis.
 */

/**
 * @brief  section() 等の引数を調べて適切な座標のパラメータを設定
 * 
 *  引数の座標が正しい位置を示してるかを判断し，
 *  正しくない場合(はみ出している場合)は座標のパラメータを調整します．
 * 
 * @param   r_col_index 列位置
 * @param   r_col_size 列サイズ
 * @param   r_row_index 行位置
 * @param   r_row_size 行サイズ
 * @param   r_layer_index レイヤ位置
 * @param   r_layer_size レイヤサイズ
 * @return  引数が自身の配列の領域内を示していた場合は0，
 *          領域からはみ出しているが有効領域が存在する場合は正の値，
 *          有効領域が無い場合は負値　
 */
int fits_image::fix_section_args( long *r_col_index, long *r_col_size,
				long *r_row_index, long *r_row_size,
				long *r_layer_index, long *r_layer_size ) const
{
    int status;
    ssize_t col_index = 0;
    size_t col_size   = MDARRAY_ALL;
    ssize_t row_index = 0;
    size_t row_size   = MDARRAY_ALL;
    ssize_t layer_index = 0;
    size_t layer_size   = MDARRAY_ALL;
    //bool col_ok, row_ok, layer_ok;

    if ( r_col_index != NULL ) col_index = *r_col_index;
    if ( r_col_size != NULL ) {
	col_size = fits_seclen_to_mdarray_seclen(*r_col_size);
    }

    if ( r_row_index != NULL ) row_index = *r_row_index;
    if ( r_row_size != NULL ) {
	row_size = fits_seclen_to_mdarray_seclen(*r_row_size);
    }

    if ( r_layer_index != NULL ) layer_index = *r_layer_index;
    if ( r_layer_size != NULL ) {
	layer_size = fits_seclen_to_mdarray_seclen(*r_layer_size);
    }

    status = this->data_array().fix_section_args( &col_index, &col_size, 
						  &row_index, &row_size,
						  &layer_index, &layer_size );

    if ( r_col_index != NULL ) *r_col_index = col_index;
    if ( r_col_size != NULL ) *r_col_size = col_size;
    if ( r_row_index != NULL ) *r_row_index = row_index;
    if ( r_row_size != NULL ) *r_row_size = row_size;
    if ( r_layer_index != NULL ) *r_layer_index = layer_index;
    if ( r_layer_size != NULL ) *r_layer_size = layer_size;

    //if ( r_col_ok != NULL ) *r_col_ok = col_ok;
    //if ( r_row_ok != NULL ) *r_row_ok = row_ok;
    //if ( r_layer_ok != NULL ) *r_layer_ok = layer_ok;

    return status;
}


/*****************************************************************************
 *
 * BEGIN : scan_along_x(), scan_along_y(), scan_along_z()
 *
 *****************************************************************************/

struct fits_image_scan_prms {
    /* アライメントに注意(sizeofが長いものから順に!) */
    double bzero;
    double bscale;
    long long blank_r;
    bool blank_is_set;
};

namespace scan_cnv_nd_x2d
{
#define MAKE_FUNC(fncname,src_sz_type,src_type,fnc_d2x) \
static void fncname(const void *org_val_ptr, void *new_val_ptr, size_t n, int f_b, void *u_ptr) \
{ \
    const struct fits_image_scan_prms *p = (const struct fits_image_scan_prms *)u_ptr; \
    const src_type *org_t_ptr = (const src_type *)org_val_ptr; \
    double *new_t_ptr = (double *)new_val_ptr; \
    size_t i; \
    if ( p->bzero == 0.0 && p->bscale == 1.0 ) { \
	double val; \
	if ( p->blank_is_set == true ) { \
	    for ( i=0 ; i < n ; i++ ) { \
		if ( p->blank_r == org_t_ptr[i] ) val = NAN; \
		else val = org_t_ptr[i]; \
		new_t_ptr[i] = val; \
	    } \
	} \
	else { \
	    for ( i=0 ; i < n ; i++ ) { \
		val = org_t_ptr[i]; \
		new_t_ptr[i] = val; \
	    } \
	} \
    } \
    else { \
	const double p_bscale = p->bscale; \
	const double p_bzero = p->bzero; \
	double val; \
	if ( p->blank_is_set == true ) { \
	    for ( i=0 ; i < n ; i++ ) { \
		if ( p->blank_r == org_t_ptr[i] ) val = NAN; \
		else { \
		    val = org_t_ptr[i]; \
		    val *= p_bscale; \
		    val += p_bzero; \
		} \
		new_t_ptr[i] = (double)val; \
	    } \
	} \
	else { \
	    for ( i=0 ; i < n ; i++ ) { \
		val = org_t_ptr[i]; \
		val *= p_bscale; \
		val += p_bzero; \
		new_t_ptr[i] = (double)val; \
	    } \
	} \
    } \
}
SLI__MDARRAY__DO_OPERATION_DOUBLE2ANY(MAKE_FUNC,,,,,);
#undef MAKE_FUNC
}

namespace scan_cnv_nd_x2f
{
#define MAKE_FUNC(fncname,src_sz_type,src_type,fnc_f2x) \
static void fncname(const void *org_val_ptr, void *new_val_ptr, size_t n, int f_b, void *u_ptr) \
{ \
    const struct fits_image_scan_prms *p = (const struct fits_image_scan_prms *)u_ptr; \
    const src_type *org_t_ptr = (const src_type *)org_val_ptr; \
    float *new_t_ptr = (float *)new_val_ptr; \
    size_t i; \
    if ( p->bzero == 0.0 && p->bscale == 1.0 ) { \
	float val; \
	if ( p->blank_is_set == true ) { \
	    for ( i=0 ; i < n ; i++ ) { \
		if ( p->blank_r == org_t_ptr[i] ) val = NAN; \
		else val = org_t_ptr[i]; \
		new_t_ptr[i] = val; \
	    } \
	} \
	else { \
	    for ( i=0 ; i < n ; i++ ) { \
		val = org_t_ptr[i]; \
		new_t_ptr[i] = val; \
	    } \
	} \
    } \
    else { \
	const double p_bscale = p->bscale; \
	const double p_bzero = p->bzero; \
	double val; \
	if ( p->blank_is_set == true ) { \
	    for ( i=0 ; i < n ; i++ ) { \
		if ( p->blank_r == org_t_ptr[i] ) val = NAN; \
		else { \
		    val = org_t_ptr[i]; \
		    val *= p_bscale; \
		    val += p_bzero; \
		} \
		new_t_ptr[i] = (float)val; \
	    } \
	} \
	else { \
	    for ( i=0 ; i < n ; i++ ) { \
		val = org_t_ptr[i]; \
		val *= p_bscale; \
		val += p_bzero; \
		new_t_ptr[i] = (float)val; \
	    } \
	} \
    } \
}
SLI__MDARRAY__DO_OPERATION_FLOAT2ANY(MAKE_FUNC,,,,,);
#undef MAKE_FUNC
}

namespace scan_gencpy2d_x2d
{
#define MAKE_FUNC(fncname,src_sz_type,src_type,fnc_d2x) \
static void fncname(const void *_src, void *_dest, size_t len_0, size_t _src_step_0, size_t _dest_step_0, size_t len_1, size_t _src_step_1, size_t _dest_step_1, void *u_ptr) \
{ \
    const struct fits_image_scan_prms *p = (const struct fits_image_scan_prms *)u_ptr; \
    const src_type *src = (const src_type *)_src; \
    double *dest = (double *)_dest; \
    const size_t src_step_1 = _src_step_1; \
    const size_t dest_step_1 = _dest_step_1; \
    const size_t src_step_0 = _src_step_0; \
    const size_t dest_step_0 = _dest_step_0; \
    size_t i, j; \
    if ( p->bzero == 0.0 && p->bscale == 1.0 ) { \
	double val; \
	if ( p->blank_is_set == true ) { \
	    for ( j=len_1 ; 0 < j ; --j ) { \
		const src_type *s_p = src; \
		double *d_p = dest; \
		for ( i=len_0 ; 0 < i ; --i ) { \
		    if ( p->blank_r == *(s_p) ) val = NAN; \
		    else val = *(s_p); \
		    *d_p = val; \
		    s_p += src_step_0; \
		    d_p += dest_step_0; \
		} \
		src += src_step_1; \
		dest += dest_step_1; \
	    } \
	} \
	else { \
	    for ( j=len_1 ; 0 < j ; --j ) { \
		const src_type *s_p = src; \
		double *d_p = dest; \
		for ( i=len_0 ; 0 < i ; --i ) { \
		    val = *(s_p); \
		    *d_p = val; \
		    s_p += src_step_0; \
		    d_p += dest_step_0; \
		} \
		src += src_step_1; \
		dest += dest_step_1; \
	    } \
	} \
    } \
    else { \
	const double p_bscale = p->bscale; \
	const double p_bzero = p->bzero; \
	double val; \
	if ( p->blank_is_set == true ) { \
	    for ( j=len_1 ; 0 < j ; --j ) { \
		const src_type *s_p = src; \
		double *d_p = dest; \
		for ( i=len_0 ; 0 < i ; --i ) { \
		    if ( p->blank_r == *(s_p) ) val = NAN; \
		    else { \
			val = *(s_p); \
			val *= p_bscale; \
			val += p_bzero; \
		    } \
		    *d_p = val; \
		    s_p += src_step_0; \
		    d_p += dest_step_0; \
		} \
		src += src_step_1; \
		dest += dest_step_1; \
	    } \
	} \
	else { \
	    for ( j=len_1 ; 0 < j ; --j ) { \
		const src_type *s_p = src; \
		double *d_p = dest; \
		for ( i=len_0 ; 0 < i ; --i ) { \
		    val = *(s_p); \
		    val *= p_bscale; \
		    val += p_bzero; \
		    *d_p = val; \
		    s_p += src_step_0; \
		    d_p += dest_step_0; \
		} \
		src += src_step_1; \
		dest += dest_step_1; \
	    } \
	} \
    } \
}
SLI__MDARRAY__DO_OPERATION_DOUBLE2ANY(MAKE_FUNC,,,,,);
#undef MAKE_FUNC
}

namespace scan_gencpy2d_x2f
{
#define MAKE_FUNC(fncname,src_sz_type,src_type,fnc_f2x) \
static void fncname(const void *_src, void *_dest, size_t len_0, size_t _src_step_0, size_t _dest_step_0, size_t len_1, size_t _src_step_1, size_t _dest_step_1, void *u_ptr) \
{ \
    const struct fits_image_scan_prms *p = (const struct fits_image_scan_prms *)u_ptr; \
    const src_type *src = (const src_type *)_src; \
    float *dest = (float *)_dest; \
    const size_t src_step_1 = _src_step_1; \
    const size_t dest_step_1 = _dest_step_1; \
    const size_t src_step_0 = _src_step_0; \
    const size_t dest_step_0 = _dest_step_0; \
    size_t i, j; \
    if ( p->bzero == 0.0 && p->bscale == 1.0 ) { \
	double val; \
	if ( p->blank_is_set == true ) { \
	    for ( j=len_1 ; 0 < j ; --j ) { \
		const src_type *s_p = src; \
		float *d_p = dest; \
		for ( i=len_0 ; 0 < i ; --i ) { \
		    if ( p->blank_r == *(s_p) ) val = NAN; \
		    else val = *(s_p); \
		    *d_p = val; \
		    s_p += src_step_0; \
		    d_p += dest_step_0; \
		} \
		src += src_step_1; \
		dest += dest_step_1; \
	    } \
	} \
	else { \
	    for ( j=len_1 ; 0 < j ; --j ) { \
		const src_type *s_p = src; \
		float *d_p = dest; \
		for ( i=len_0 ; 0 < i ; --i ) { \
		    val = *(s_p); \
		    *d_p = val; \
		    s_p += src_step_0; \
		    d_p += dest_step_0; \
		} \
		src += src_step_1; \
		dest += dest_step_1; \
	    } \
	} \
    } \
    else { \
	const double p_bscale = p->bscale; \
	const double p_bzero = p->bzero; \
	double val; \
	if ( p->blank_is_set == true ) { \
	    for ( j=len_1 ; 0 < j ; --j ) { \
		const src_type *s_p = src; \
		float *d_p = dest; \
		for ( i=len_0 ; 0 < i ; --i ) { \
		    if ( p->blank_r == *(s_p) ) val = NAN; \
		    else { \
			val = *(s_p); \
			val *= p_bscale; \
			val += p_bzero; \
		    } \
		    *d_p = val; \
		    s_p += src_step_0; \
		    d_p += dest_step_0; \
		} \
		src += src_step_1; \
		dest += dest_step_1; \
	    } \
	} \
	else { \
	    for ( j=len_1 ; 0 < j ; --j ) { \
		const src_type *s_p = src; \
		float *d_p = dest; \
		for ( i=len_0 ; 0 < i ; --i ) { \
		    val = *(s_p); \
		    val *= p_bscale; \
		    val += p_bzero; \
		    *d_p = val; \
		    s_p += src_step_0; \
		    d_p += dest_step_0; \
		} \
		src += src_step_1; \
		dest += dest_step_1; \
	    } \
	} \
    } \
}
SLI__MDARRAY__DO_OPERATION_FLOAT2ANY(MAKE_FUNC,,,,,);
#undef MAKE_FUNC
}

/**
 * @brief  scan系メンバ関数のための準備
 *
 * @note   このメンバ関数は private です．
 */
void fits_image::prepare_scan_a_section() const
{
    fits_image *thisp = (fits_image *)this;		/* 強制的にキャスト */
    struct fits_image_scan_prms *prms;

    if ( thisp->scan_prms_rec.allocate(sizeof(*prms)) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","malloc() failed");
    }
    prms = (struct fits_image_scan_prms *)(thisp->scan_prms_rec.ptr());

    prms->blank_is_set = false;
    prms->blank_r = 0;
    prms->bzero = this->bzero();
    prms->bscale = this->bscale();

    if ( this->type_rec != FITS::DOUBLE_T &&
	 this->type_rec != FITS::FLOAT_T ) {
	prms->blank_r = this->blank();
	prms->blank_is_set = this->blank_is_set();
    }

    thisp->scan_func_cnv_nd_x2d = NULL;
    thisp->scan_func_gencpy2d_x2d = NULL;

#define SEL_FUNC(fncname,src_sz_type,src_type,fnc_d2x) \
    if ( this->data_array().size_type() == src_sz_type ) { \
	thisp->scan_func_cnv_nd_x2d = &scan_cnv_nd_x2d::fncname; \
	thisp->scan_func_gencpy2d_x2d = &scan_gencpy2d_x2d::fncname; \
    }
    SLI__MDARRAY__DO_OPERATION_DOUBLE2ANY(SEL_FUNC,,,,,else);
#undef SEL_FUNC

    if ( thisp->scan_func_cnv_nd_x2d == NULL ) {
	err_throw(__FUNCTION__,"FATAL","detected unexpected NULL pointer");
    }
    if ( thisp->scan_func_gencpy2d_x2d == NULL ) {
	err_throw(__FUNCTION__,"FATAL","detected unexpected NULL pointer");
    }

    thisp->scan_func_cnv_nd_x2f = NULL;
    thisp->scan_func_gencpy2d_x2f = NULL;

#define SEL_FUNC(fncname,src_sz_type,src_type,fnc_f2x) \
    if ( this->data_array().size_type() == src_sz_type ) { \
	thisp->scan_func_cnv_nd_x2f = &scan_cnv_nd_x2f::fncname; \
	thisp->scan_func_gencpy2d_x2f = &scan_gencpy2d_x2f::fncname; \
    }
    SLI__MDARRAY__DO_OPERATION_FLOAT2ANY(SEL_FUNC,,,,,else);
#undef SEL_FUNC

    if ( thisp->scan_func_cnv_nd_x2f == NULL ) {
	err_throw(__FUNCTION__,"FATAL","detected unexpected NULL pointer");
    }
    if ( thisp->scan_func_gencpy2d_x2f == NULL ) {
	err_throw(__FUNCTION__,"FATAL","detected unexpected NULL pointer");
    }

    return;
}

static const unsigned int Bit_use_along_x   = 0x0001;
static const unsigned int Bit_use_along_y   = 0x0002;
static const unsigned int Bit_use_along_z   = 0x0004;
static const unsigned int Bit_use_xy_planes = 0x0008;
static const unsigned int Bit_use_xz_planes = 0x0010;
static const unsigned int Bit_use_yx_planes = 0x0020;
static const unsigned int Bit_use_yz_planes = 0x0040;
static const unsigned int Bit_use_zx_planes = 0x0080;
static const unsigned int Bit_use_zy_planes = 0x0100;
static const unsigned int Bit_use_cube      = 0x0200;

/* horizontally scans the specified section.  A temporary buffer of 1-D */
/* array is prepared and scan_along_x() returns it.                     */
/* A scan order is displayed in pseudocode:                             */
/*  for(...) {      <- layer                                            */
/*    for(...) {    <- row                                              */
/*      for(...) {  <- column                                           */
/**
 * @brief  画像データ配列要素のx方向線分毎でのスキャン・シーケンスの開始
 *
 * 自身の画像データ配列の指定された範囲の要素を，行単位でスキャン(ラスター
 * スキャン)するための準備をします．行単位のスキャンを行なう場合，次の手順で
 * 一連のAPIを使います．<br>
 * - beginf_scan_along_x() で準備をする．<br>
 * - scan_along_x() を行の数だけ呼び出し，行ごとの処理を行なう．<br>
 * - end_scan_along_x() で終了処理(一時バッファの開放)を行なう．<br>
 * scan_along_x() は1行分の要素が入った一時バッファのアドレス(32バイトでアライ
 * ンされている)を返します．この一時バッファはオブジェクトの内部型が何であって
 * も double 型で提供されます．なお，このバッファはオブジェクトで管理されている
 * ため，プログラマが開放してはいけません．<br>
 * scan_along_x() は指定範囲全てのスキャンが完了している場合には NULL を返しま
 * す．<br>
 * 画像データの統計をとる時などに使うメンバ関数です．
 *
 * @param     exp_fmt スキャンする範囲を示す文字列(例: "0:99,*")のための
 *                    フォーマット指定(printfと同様)
 * @param     ... exp_fmtに対応した可変長引数の各要素データ
 * @return    スキャンによる有効要素の数．エラーの場合は負値．
 * @note      "[1:100,*]" のように [] で囲むと，1-indexed として扱われます．
 */
long fits_image::beginf_scan_along_x( const char *exp_fmt, ... ) const
{
    long ret;
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	ret = this->vbeginf_scan_along_x(exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vbeginf_scan_along_x() failed");
    }
    va_end(ap);
    return ret;
}

/**
 * @brief  画像データ配列要素のx方向線分毎でのスキャン・シーケンスの開始
 *
 * 自身の画像データ配列の指定された範囲の要素を，行単位でスキャン(ラスター
 * スキャン)するための準備をします．行単位のスキャンを行なう場合，次の手順で
 * 一連のAPIを使います．<br>
 * - beginf_scan_along_x() で準備をする．<br>
 * - scan_along_x() を行の数だけ呼び出し，行ごとの処理を行なう．<br>
 * - end_scan_along_x() で終了処理(一時バッファの開放)を行なう．<br>
 * scan_along_x() は1行分の要素が入った一時バッファのアドレス(32バイトでアライ
 * ンされている)を返します．この一時バッファはオブジェクトの内部型が何であって
 * も double 型で提供されます．なお，このバッファはオブジェクトで管理されている
 * ため，プログラマが開放してはいけません．<br>
 * scan_along_x() は指定範囲全てのスキャンが完了している場合には NULL を返しま
 * す．<br>
 * 画像データの統計をとる時などに使うメンバ関数です．
 *
 * @param     exp_fmt スキャンする範囲を示す文字列(例: "0:99,*")のための
 *                    フォーマット指定(printfと同様)
 * @param     ap exp_fmtに対応した可変長引数のリスト
 * @return    スキャンによる有効要素の数．エラーの場合は負値．
 * @note      "[1:100,*]" のように [] で囲むと，1-indexed として扱われます．
 */
long fits_image::vbeginf_scan_along_x( const char *exp_fmt, va_list ap ) const
{
    fits_image *thisp = (fits_image *)this;		/* 強制的にキャスト */

    thisp->prepare_scan_a_section();
    thisp->scan_use_flags |= Bit_use_along_x;

    return this->data_array().vbeginf_scan_along_x( exp_fmt, ap );
}

/**
 * @brief  画像データ配列要素のx方向線分毎でのスキャン・シーケンスの開始
 *
 * 自身の画像データ配列の指定された範囲の要素を，行単位でスキャン(ラスター
 * スキャン)するための準備をします．行単位のスキャンを行なう場合，次の手順で
 * 一連のAPIを使います．<br>
 * - begin_scan_along_x() で準備をする．<br>
 * - scan_along_x() を行の数だけ呼び出し，行ごとの処理を行なう．<br>
 * - end_scan_along_x() で終了処理(一時バッファの開放)を行なう．<br>
 * scan_along_x() は1行分の要素が入った一時バッファのアドレス(32バイトでアライ
 * ンされている)を返します．この一時バッファはオブジェクトの内部型が何であって
 * も double 型で提供されます．なお，このバッファはオブジェクトで管理されている
 * ため，プログラマが開放してはいけません．<br>
 * scan_along_x() は指定範囲全てのスキャンが完了している場合には NULL を返しま
 * す．<br>
 * 画像データの統計をとる時などに使うメンバ関数です．
 *
 * @param     col_index 列位置
 * @param     col_size 列サイズ
 * @param     row_index 行位置
 * @param     row_size 行サイズ
 * @param     layer_index レイヤ位置
 * @param     layer_size レイヤサイズ
 * @return    スキャンによる有効要素の数．エラーの場合は負値．
 */
long fits_image::begin_scan_along_x( long col_index, long col_size,
				     long row_index, long row_size,
				     long layer_index, long layer_size ) const
{
    fits_image *thisp = (fits_image *)this;		/* 強制的にキャスト */
    const size_t z_col_size = fits_seclen_to_mdarray_seclen(col_size);
    const size_t z_row_size = fits_seclen_to_mdarray_seclen(row_size);
    const size_t z_layer_size = fits_seclen_to_mdarray_seclen(layer_size);

    thisp->prepare_scan_a_section();
    thisp->scan_use_flags |= Bit_use_along_x;

    return this->data_array().begin_scan_along_x( col_index, z_col_size, 
					       row_index, z_row_size,
					       layer_index, z_layer_size );
}

/**
 * @brief  画像データ配列要素をx方向線分毎に連続的にスキャン (返り値はdouble型)
 */
double *fits_image::scan_along_x( long *n, long *x, long *y, long *z ) const
{
    double *ret_p;
    fits_image *thisp = (fits_image *)this;		/* 強制的にキャスト */
    size_t n0;
    ssize_t x0, y0, z0;
    ret_p = this->data_array().scan_along_x_via_udf( thisp->scan_func_cnv_nd_x2d, 
						  thisp->scan_prms_rec.ptr(), 
						  &n0, &x0, &y0, &z0 );
    if ( ret_p != NULL ) {
	if ( n != NULL ) *n = n0;
	if ( x != NULL ) *x = x0;
	if ( y != NULL ) *y = y0;
	if ( z != NULL ) *z = z0;
    }

    return ret_p;
}

/**
 * @brief  画像データ配列要素をx方向線分毎に連続的にスキャン (返り値はfloat型)
 */
float *fits_image::scan_along_x_f( long *n, long *x, long *y, long *z ) const
{
    float *ret_p;
    fits_image *thisp = (fits_image *)this;		/* 強制的にキャスト */
    size_t n0;
    ssize_t x0, y0, z0;
    ret_p = this->data_array().scan_along_x_f_via_udf(thisp->scan_func_cnv_nd_x2f,
						   thisp->scan_prms_rec.ptr(), 
						   &n0, &x0, &y0, &z0);
    if ( ret_p != NULL ) {
	if ( n != NULL ) *n = n0;
	if ( x != NULL ) *x = x0;
	if ( y != NULL ) *y = y0;
	if ( z != NULL ) *z = z0;
    }

    return ret_p;
}

/**
 * @brief  begin_scan_along_x() で開始したスキャン・シーケンスの終了
 */
void fits_image::end_scan_along_x() const
{
    fits_image *thisp = (fits_image *)this;		/* 強制的にキャスト */
    this->data_array().end_scan_along_x();
    thisp->scan_use_flags &= ~Bit_use_along_x;
    if ( thisp->scan_use_flags == 0 ) thisp->scan_prms_rec = NULL;
}

/* vertically scans the specified section.  A temporary buffer of 1-D */
/* array is prepared and scan_along_y() returns it.                   */
/* scan order is displayed by pseudocode:                             */
/*  for(...) {      <- layer                                          */
/*    for(...) {    <- column                                         */
/*      for(...) {  <- row                                            */
/**
 * @brief  画像データ配列要素のy方向線分毎でのスキャン・シーケンスの開始
 *
 * 自身の画像データ配列の指定された範囲の要素を，列単位でスキャン(縦方向の
 * スキャン)するための準備をします．内部でブロック単位の高速transpose(データ
 * アレイにおけるx軸とy軸との入れ換え)を行なうため，高いパフォーマンスが得られ
 * ます．<br>
 * 行単位のスキャンを行なう場合，次の手順で一連のAPIを使います．<br>
 * - beginf_scan_along_y() で準備をする．<br>
 * - scan_along_y() を列の数だけ呼び出し，列ごとの処理を行なう．<br>
 * - end_scan_along_y() で終了処理(一時バッファの開放)を行なう．<br>
 * scan_along_y() は1列分の要素が入った一時バッファのアドレス(32バイトでアライ
 * ンされている)を返します．この一時バッファはオブジェクトの内部型が何であって
 * も double 型で提供されます．なお，このバッファはオブジェクトで管理されている
 * ため，プログラマが開放してはいけません．<br>
 * scan_along_y() は指定範囲全てのスキャンが完了している場合には NULL を返しま
 * す．<br>
 * 画像データの統計をとる時などに使うメンバ関数です．
 *
 * @param     exp_fmt スキャンする範囲を示す文字列(例: "0:99,*")のための
 *                    フォーマット指定(printfと同様)
 * @param     ... exp_fmtに対応した可変長引数の各要素データ
 * @return    スキャンによる有効要素の数．エラーの場合は負値．
 * @note      "[1:100,*]" のように [] で囲むと，1-indexed として扱われます．
 */
long fits_image::beginf_scan_along_y( const char *exp_fmt, ... ) const
{
    long ret;
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	ret = this->vbeginf_scan_along_y(exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vbeginf_scan_along_y() failed");
    }
    va_end(ap);
    return ret;
}

/**
 * @brief  画像データ配列要素のy方向線分毎でのスキャン・シーケンスの開始
 *
 * 自身の画像データ配列の指定された範囲の要素を，列単位でスキャン(縦方向の
 * スキャン)するための準備をします．内部でブロック単位の高速transpose(データ
 * アレイにおけるx軸とy軸との入れ換え)を行なうため，高いパフォーマンスが得られ
 * ます．<br>
 * 行単位のスキャンを行なう場合，次の手順で一連のAPIを使います．<br>
 * - beginf_scan_along_y() で準備をする．<br>
 * - scan_along_y() を列の数だけ呼び出し，列ごとの処理を行なう．<br>
 * - end_scan_along_y() で終了処理(一時バッファの開放)を行なう．<br>
 * scan_along_y() は1列分の要素が入った一時バッファのアドレス(32バイトでアライ
 * ンされている)を返します．この一時バッファはオブジェクトの内部型が何であって
 * も double 型で提供されます．なお，このバッファはオブジェクトで管理されている
 * ため，プログラマが開放してはいけません．<br>
 * scan_along_y() は指定範囲全てのスキャンが完了している場合には NULL を返しま
 * す．<br>
 * 画像データの統計をとる時などに使うメンバ関数です．
 *
 * @param     exp_fmt スキャンする範囲を示す文字列(例: "0:99,*")のための
 *                    フォーマット指定(printfと同様)
 * @param     ap exp_fmtに対応した可変長引数のリスト
 * @return    スキャンによる有効要素の数．エラーの場合は負値．
 * @note      "[1:100,*]" のように [] で囲むと，1-indexed として扱われます．
 */
long fits_image::vbeginf_scan_along_y( const char *exp_fmt, va_list ap ) const
{
    fits_image *thisp = (fits_image *)this;		/* 強制的にキャスト */

    thisp->prepare_scan_a_section();
    thisp->scan_use_flags |= Bit_use_along_y;

    return this->data_array().vbeginf_scan_along_y( exp_fmt, ap );
}

/**
 * @brief  画像データ配列要素のy方向線分毎でのスキャン・シーケンスの開始
 *
 * 自身の画像データ配列の指定された範囲の要素を，列単位でスキャン(縦方向の
 * スキャン)するための準備をします．内部でブロック単位の高速transpose(データ
 * アレイにおけるx軸とy軸との入れ換え)を行なうため，高いパフォーマンスが得られ
 * ます．<br>
 * 行単位のスキャンを行なう場合，次の手順で一連のAPIを使います．<br>
 * - begin_scan_along_y() で準備をする．<br>
 * - scan_along_y() を列の数だけ呼び出し，列ごとの処理を行なう．<br>
 * - end_scan_along_y() で終了処理(一時バッファの開放)を行なう．<br>
 * scan_along_y() は1列分の要素が入った一時バッファのアドレス(32バイトでアライ
 * ンされている)を返します．この一時バッファはオブジェクトの内部型が何であって
 * も double 型で提供されます．なお，このバッファはオブジェクトで管理されている
 * ため，プログラマが開放してはいけません．<br>
 * scan_along_y() は指定範囲全てのスキャンが完了している場合には NULL を返しま
 * す．<br>
 * 画像データの統計をとる時などに使うメンバ関数です．
 *
 * @param     col_index 列位置
 * @param     col_size 列サイズ
 * @param     row_index 行位置
 * @param     row_size 行サイズ
 * @param     layer_index レイヤ位置
 * @param     layer_size レイヤサイズ
 * @return    スキャンによる有効要素の数．エラーの場合は負値．
 */
long fits_image::begin_scan_along_y( long col_index, long col_size,
				     long row_index, long row_size,
				     long layer_index, long layer_size ) const
{
    fits_image *thisp = (fits_image *)this;		/* 強制的にキャスト */
    size_t z_col_size = fits_seclen_to_mdarray_seclen(col_size);
    size_t z_row_size = fits_seclen_to_mdarray_seclen(row_size);
    size_t z_layer_size = fits_seclen_to_mdarray_seclen(layer_size);

    thisp->prepare_scan_a_section();
    thisp->scan_use_flags |= Bit_use_along_y;

    return this->data_array().begin_scan_along_y( col_index, z_col_size, 
					       row_index, z_row_size,
					       layer_index, z_layer_size );
}

/**
 * @brief  画像データ配列要素をy方向線分毎に連続的にスキャン (返り値はdouble型)
 */
double *fits_image::scan_along_y( long *n, long *x, long *y, long *z ) const
{
    double *ret_p;
    fits_image *thisp = (fits_image *)this;		/* 強制的にキャスト */
    size_t n0;
    ssize_t x0, y0, z0;
    ret_p = this->data_array().scan_along_y_via_udf( thisp->scan_func_cnv_nd_x2d, 
						  thisp->scan_prms_rec.ptr(), 
						  &n0, &x0, &y0, &z0 );
    if ( ret_p != NULL ) {
	if ( n != NULL ) *n = n0;
	if ( x != NULL ) *x = x0;
	if ( y != NULL ) *y = y0;
	if ( z != NULL ) *z = z0;
    }

    return ret_p;
}

/**
 * @brief  画像データ配列要素をy方向線分毎に連続的にスキャン (返り値はfloat型)
 */
float *fits_image::scan_along_y_f( long *n, long *x, long *y, long *z ) const
{
    float *ret_p;
    fits_image *thisp = (fits_image *)this;		/* 強制的にキャスト */
    size_t n0;
    ssize_t x0, y0, z0;
    ret_p = this->data_array().scan_along_y_f_via_udf(thisp->scan_func_cnv_nd_x2f,
						   thisp->scan_prms_rec.ptr(), 
						   &n0, &x0, &y0, &z0);
    if ( ret_p != NULL ) {
	if ( n != NULL ) *n = n0;
	if ( x != NULL ) *x = x0;
	if ( y != NULL ) *y = y0;
	if ( z != NULL ) *z = z0;
    }

    return ret_p;
}

/**
 * @brief  begin_scan_along_y() で開始したスキャン・シーケンスの終了
 */
void fits_image::end_scan_along_y() const
{
    fits_image *thisp = (fits_image *)this;		/* 強制的にキャスト */
    this->data_array().end_scan_along_y();
    thisp->scan_use_flags &= ~Bit_use_along_y;
    if ( thisp->scan_use_flags == 0 ) thisp->scan_prms_rec = NULL;
}

/* scan the specified section along z-axis.  A temporary buffer of 1-D */
/* array is prepared and scan_along_z() returns it.                    */
/* scan order is displayed by pseudocode:                              */
/*  for(...) {      <- row                                             */
/*    for(...) {    <- column                                          */
/*      for(...) {  <- layer                                           */
/**
 * @brief  画像データ配列要素のz方向線分毎でのスキャン・シーケンスの開始
 *
 * 自身の画像データ配列の指定された範囲の要素を，(x,y)の位置1つ1つについて，
 * z方向にスキャンするための準備をします．内部でブロック単位の高速transpose
 * (データアレイにおけるx軸とz軸との入れ換え)を行なうため，高いパフォーマンス
 * が得られます．<br>
 * z方向スキャンを行なう場合，次の手順で一連のAPIを使います．<br>
 * - beginf_scan_along_z() で準備をする．<br>
 * - scan_along_z() を必要な数だけ呼び出し，(x,y)ごとの処理を行なう．<br>
 * - end_scan_along_z() で終了処理(一時バッファの開放)を行なう．<br>
 * scan_along_z() は(x,y)の位置1つについてのz方向の要素が入った一時バッファの
 * アドレス(32バイトでアラインされている)を返します．この一時バッファはオブ
 * ジェクトの内部型が何であっても double 型で提供されます．なお，このバッファ
 * はオブジェクトで管理されているため，プログラマが開放してはいけません．<br>
 * scan_along_z() は指定範囲全てのスキャンが完了している場合には NULL を返しま
 * す．<br>
 * 画像データの統計をとる時などに使うメンバ関数です．z方向の大きさが十分でない
 * 場合，パフォーマンスが得られない事があります．その場合は，scan_zx_planes()
 * をお試しください．
 *
 * @param     exp_fmt スキャンする範囲を示す文字列(例: "0:99,*")のための
 *                    フォーマット指定(printfと同様)
 * @param     ... exp_fmtに対応した可変長引数の各要素データ
 * @return    スキャンによる有効要素の数．エラーの場合は負値．
 * @note      "[1:100,*]" のように [] で囲むと，1-indexed として扱われます．
 */
long fits_image::beginf_scan_along_z( const char *exp_fmt, ... ) const
{
    long ret;
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	ret = this->vbeginf_scan_along_z(exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vbeginf_scan_along_z() failed");
    }
    va_end(ap);
    return ret;
}

/**
 * @brief  画像データ配列要素のz方向線分毎でのスキャン・シーケンスの開始
 *
 * 自身の画像データ配列の指定された範囲の要素を，(x,y)の位置1つ1つについて，
 * z方向にスキャンするための準備をします．内部でブロック単位の高速transpose
 * (データアレイにおけるx軸とz軸との入れ換え)を行なうため，高いパフォーマンス
 * が得られます．<br>
 * z方向スキャンを行なう場合，次の手順で一連のAPIを使います．<br>
 * - beginf_scan_along_z() で準備をする．<br>
 * - scan_along_z() を必要な数だけ呼び出し，(x,y)ごとの処理を行なう．<br>
 * - end_scan_along_z() で終了処理(一時バッファの開放)を行なう．<br>
 * scan_along_z() は(x,y)の位置1つについてのz方向の要素が入った一時バッファの
 * アドレス(32バイトでアラインされている)を返します．この一時バッファはオブ
 * ジェクトの内部型が何であっても double 型で提供されます．なお，このバッファ
 * はオブジェクトで管理されているため，プログラマが開放してはいけません．<br>
 * scan_along_z() は指定範囲全てのスキャンが完了している場合には NULL を返しま
 * す．<br>
 * 画像データの統計をとる時などに使うメンバ関数です．z方向の大きさが十分でない
 * 場合，パフォーマンスが得られない事があります．その場合は，scan_zx_planes()
 * をお試しください．
 *
 * @param     exp_fmt スキャンする範囲を示す文字列(例: "0:99,*")のための
 *                    フォーマット指定(printfと同様)
 * @param     ap exp_fmtに対応した可変長引数のリスト
 * @return    スキャンによる有効要素の数．エラーの場合は負値．
 * @note      "[1:100,*]" のように [] で囲むと，1-indexed として扱われます．
 */
long fits_image::vbeginf_scan_along_z( const char *exp_fmt, va_list ap ) const
{
    fits_image *thisp = (fits_image *)this;		/* 強制的にキャスト */

    thisp->prepare_scan_a_section();
    thisp->scan_use_flags |= Bit_use_along_z;

    return this->data_array().vbeginf_scan_along_z( exp_fmt, ap );
}

/**
 * @brief  画像データ配列要素のz方向線分毎でのスキャン・シーケンスの開始
 *
 * 自身の画像データ配列の指定された範囲の要素を，(x,y)の位置1つ1つについて，
 * z方向にスキャンするための準備をします．内部でブロック単位の高速transpose
 * (データアレイにおけるx軸とz軸との入れ換え)を行なうため，高いパフォーマンス
 * が得られます．<br>
 * z方向スキャンを行なう場合，次の手順で一連のAPIを使います．<br>
 * - begin_scan_along_z() で準備をする．<br>
 * - scan_along_z() を必要な数だけ呼び出し，(x,y)ごとの処理を行なう．<br>
 * - end_scan_along_z() で終了処理(一時バッファの開放)を行なう．<br>
 * scan_along_z() は(x,y)の位置1つについてのz方向の要素が入った一時バッファの
 * アドレス(32バイトでアラインされている)を返します．この一時バッファはオブ
 * ジェクトの内部型が何であっても double 型で提供されます．なお，このバッファ
 * はオブジェクトで管理されているため，プログラマが開放してはいけません．<br>
 * scan_along_z() は指定範囲全てのスキャンが完了している場合には NULL を返しま
 * す．<br>
 * 画像データの統計をとる時などに使うメンバ関数です．z方向の大きさが十分でない
 * 場合，パフォーマンスが得られない事があります．その場合は，scan_zx_planes()
 * をお試しください．
 *
 * @param     col_index 列位置
 * @param     col_size 列サイズ
 * @param     row_index 行位置
 * @param     row_size 行サイズ
 * @param     layer_index レイヤ位置
 * @param     layer_size レイヤサイズ
 * @return    スキャンによる有効要素の数．エラーの場合は負値．
 */
long fits_image::begin_scan_along_z( long col_index, long col_size,
				     long row_index, long row_size,
				     long layer_index, long layer_size ) const
{
    fits_image *thisp = (fits_image *)this;		/* 強制的にキャスト */
    size_t z_col_size = fits_seclen_to_mdarray_seclen(col_size);
    size_t z_row_size = fits_seclen_to_mdarray_seclen(row_size);
    size_t z_layer_size = fits_seclen_to_mdarray_seclen(layer_size);

    thisp->prepare_scan_a_section();
    thisp->scan_use_flags |= Bit_use_along_z;

    return this->data_array().begin_scan_along_z( col_index, z_col_size, 
					       row_index, z_row_size,
					       layer_index, z_layer_size );
}

/**
 * @brief  画像データ配列要素をz方向線分毎に連続的にスキャン (返り値はdouble型)
 */
double *fits_image::scan_along_z( long *n, long *x, long *y, long *z ) const
{
    double *ret_p;
    fits_image *thisp = (fits_image *)this;		/* 強制的にキャスト */
    size_t n0;
    ssize_t x0, y0, z0;
    ret_p = this->data_array().scan_along_z_via_udf( thisp->scan_func_cnv_nd_x2d, 
						  thisp->scan_prms_rec.ptr(), 
						  &n0, &x0, &y0, &z0 );
    if ( ret_p != NULL ) {
	if ( n != NULL ) *n = n0;
	if ( x != NULL ) *x = x0;
	if ( y != NULL ) *y = y0;
	if ( z != NULL ) *z = z0;
    }

    return ret_p;
}

/**
 * @brief  画像データ配列要素をz方向線分毎に連続的にスキャン (返り値はfloat型)
 */
float *fits_image::scan_along_z_f( long *n, long *x, long *y, long *z ) const
{
    float *ret_p;
    fits_image *thisp = (fits_image *)this;		/* 強制的にキャスト */
    size_t n0;
    ssize_t x0, y0, z0;
    ret_p = this->data_array().scan_along_z_f_via_udf(thisp->scan_func_cnv_nd_x2f,
						   thisp->scan_prms_rec.ptr(), 
						   &n0, &x0, &y0, &z0);
    if ( ret_p != NULL ) {
	if ( n != NULL ) *n = n0;
	if ( x != NULL ) *x = x0;
	if ( y != NULL ) *y = y0;
	if ( z != NULL ) *z = z0;
    }

    return ret_p;
}

/**
 * @brief  begin_scan_along_z() で開始したスキャン・シーケンスの終了
 */
void fits_image::end_scan_along_z() const
{
    fits_image *thisp = (fits_image *)this;		/* 強制的にキャスト */
    this->data_array().end_scan_along_z();
    thisp->scan_use_flags &= ~Bit_use_along_z;
    if ( thisp->scan_use_flags == 0 ) thisp->scan_prms_rec = NULL;
}

/* scan the specified 3-D section with plane by plane (zx plane). */
/* A temporary buffer of 2-D array is prepared.                   */
/* scan order is displayed by pseudocode:                         */
/*  for(...) {      <- row                                        */
/*    for(...) {    <- column                                     */
/*      for(...) {  <- layer                                      */
/**
 * @brief  画像データ配列要素のzx面単位でのスキャン・シーケンスの開始
 *
 * 自身の画像データ配列の指定された範囲の要素を，zx面単位でスキャンするための
 * 準備をします．内部でブロック単位の高速transpose(データアレイにおけるx軸とz軸
 * との入れ換え)を行なうため，高いパフォーマンスが得られます．<br>
 * zx面単位でスキャンを行なう場合，次の手順で一連のAPIを使います．<br>
 * - beginf_scan_zx_planes() で準備をする．<br>
 * - scan_zx_planes() を必要な数だけ呼び出し，面ごとの処理を行なう．<br>
 * - end_scan_zx_planes() で終了処理(一時バッファの開放)を行なう．<br>
 * scan_zx_planes() はyの位置1つについてのzx面の要素が入った一時バッファのアド
 * レス(32バイトでアラインされている)を返します．この一時バッファはオブジェクト
 * の内部型が何であっても double 型で提供されます．なお，このバッファはオブジェ
 * クトで管理されているため，プログラマが開放してはいけません．<br>
 * scan_zx_planes() は指定範囲全てのスキャンが完了している場合には NULL を返し
 * ます．<br>
 * 画像データの統計をとる時などに使うメンバ関数です．
 *
 * @param     exp_fmt スキャンする範囲を示す文字列(例: "0:99,*")のための
 *                    フォーマット指定(printfと同様)
 * @param     ... exp_fmtに対応した可変長引数の各要素データ
 * @return    スキャンによる有効要素の数．エラーの場合は負値．
 * @note      "[1:100,*]" のように [] で囲むと，1-indexed として扱われます．
 */
long fits_image::beginf_scan_zx_planes( const char *exp_fmt, ... ) const
{
    long ret;
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	ret = this->vbeginf_scan_zx_planes(exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vbeginf_scan_zx_planes() failed");
    }
    va_end(ap);
    return ret;
}

/**
 * @brief  画像データ配列要素のzx面単位でのスキャン・シーケンスの開始
 *
 * 自身の画像データ配列の指定された範囲の要素を，zx面単位でスキャンするための
 * 準備をします．内部でブロック単位の高速transpose(データアレイにおけるx軸とz軸
 * との入れ換え)を行なうため，高いパフォーマンスが得られます．<br>
 * zx面単位でスキャンを行なう場合，次の手順で一連のAPIを使います．<br>
 * - beginf_scan_zx_planes() で準備をする．<br>
 * - scan_zx_planes() を必要な数だけ呼び出し，面ごとの処理を行なう．<br>
 * - end_scan_zx_planes() で終了処理(一時バッファの開放)を行なう．<br>
 * scan_zx_planes() はyの位置1つについてのzx面の要素が入った一時バッファのアド
 * レス(32バイトでアラインされている)を返します．この一時バッファはオブジェクト
 * の内部型が何であっても double 型で提供されます．なお，このバッファはオブジェ
 * クトで管理されているため，プログラマが開放してはいけません．<br>
 * scan_zx_planes() は指定範囲全てのスキャンが完了している場合には NULL を返し
 * ます．<br>
 * 画像データの統計をとる時などに使うメンバ関数です．
 *
 * @param     exp_fmt スキャンする範囲を示す文字列(例: "0:99,*")のための
 *                    フォーマット指定(printfと同様)
 * @param     ap exp_fmtに対応した可変長引数のリスト
 * @return    スキャンによる有効要素の数．エラーの場合は負値．
 * @note      "[1:100,*]" のように [] で囲むと，1-indexed として扱われます．
 */
long fits_image::vbeginf_scan_zx_planes( const char *exp_fmt, va_list ap ) const
{
    fits_image *thisp = (fits_image *)this;		/* 強制的にキャスト */

    thisp->prepare_scan_a_section();
    thisp->scan_use_flags |= Bit_use_zx_planes;

    return this->data_array().vbeginf_scan_zx_planes( exp_fmt, ap );
}

/**
 * @brief  画像データ配列要素のzx面単位でのスキャン・シーケンスの開始
 *
 * 自身の画像データ配列の指定された範囲の要素を，zx面単位でスキャンするための
 * 準備をします．内部でブロック単位の高速transpose(データアレイにおけるx軸とz軸
 * との入れ換え)を行なうため，高いパフォーマンスが得られます．<br>
 * zx面単位でスキャンを行なう場合，次の手順で一連のAPIを使います．<br>
 * - begin_scan_zx_planes() で準備をする．<br>
 * - scan_zx_planes() を必要な数だけ呼び出し，面ごとの処理を行なう．<br>
 * - end_scan_zx_planes() で終了処理(一時バッファの開放)を行なう．<br>
 * scan_zx_planes() はyの位置1つについてのzx面の要素が入った一時バッファのアド
 * レス(32バイトでアラインされている)を返します．この一時バッファはオブジェクト
 * の内部型が何であっても double 型で提供されます．なお，このバッファはオブジェ
 * クトで管理されているため，プログラマが開放してはいけません．<br>
 * scan_zx_planes() は指定範囲全てのスキャンが完了している場合には NULL を返し
 * ます．<br>
 * 画像データの統計をとる時などに使うメンバ関数です．
 *
 * @param     col_index 列位置
 * @param     col_size 列サイズ
 * @param     row_index 行位置
 * @param     row_size 行サイズ
 * @param     layer_index レイヤ位置
 * @param     layer_size レイヤサイズ
 * @return    スキャンによる有効要素の数．エラーの場合は負値．
 */
long fits_image::begin_scan_zx_planes( 
				      long col_index, long col_size,
				      long row_index, long row_size,
				      long layer_index, long layer_size ) const
{
    fits_image *thisp = (fits_image *)this;		/* 強制的にキャスト */
    size_t z_col_size = fits_seclen_to_mdarray_seclen(col_size);
    size_t z_row_size = fits_seclen_to_mdarray_seclen(row_size);
    size_t z_layer_size = fits_seclen_to_mdarray_seclen(layer_size);

    thisp->prepare_scan_a_section();
    thisp->scan_use_flags |= Bit_use_zx_planes;

    return this->data_array().begin_scan_zx_planes( col_index, z_col_size, 
						 row_index, z_row_size,
						 layer_index, z_layer_size );
}

/**
 * @brief  画像データ配列要素をzx面単位で連続的にスキャン (返り値はdouble型)
 */
double *fits_image::scan_zx_planes( 
			long *n_z, long *n_x, long *x, long *y, long *z ) const
{
    double *ret_p;
    fits_image *thisp = (fits_image *)this;		/* 強制的にキャスト */
    size_t n_z0, n_x0;
    ssize_t x0, y0, z0;
    ret_p = this->data_array().scan_zx_planes_via_udf(
		   thisp->scan_func_gencpy2d_x2d, thisp->scan_prms_rec.ptr(), 
						  &n_z0, &n_x0, &x0, &y0, &z0);
    if ( ret_p != NULL ) {
	if ( n_z != NULL ) *n_z = n_z0;
	if ( n_x != NULL ) *n_x = n_x0;
	if ( x != NULL ) *x = x0;
	if ( y != NULL ) *y = y0;
	if ( z != NULL ) *z = z0;
    }

    return ret_p;
}

/**
 * @brief  画像データ配列要素をzx面単位で連続的にスキャン (返り値はfloat型)
 */
float *fits_image::scan_zx_planes_f( 
			long *n_z, long *n_x, long *x, long *y, long *z ) const
{
    float *ret_p;
    fits_image *thisp = (fits_image *)this;		/* 強制的にキャスト */
    size_t n_z0, n_x0;
    ssize_t x0, y0, z0;
    ret_p = this->data_array().scan_zx_planes_f_via_udf(
		   thisp->scan_func_gencpy2d_x2f, thisp->scan_prms_rec.ptr(), 
						  &n_z0, &n_x0, &x0, &y0, &z0);
    if ( ret_p != NULL ) {
	if ( n_z != NULL ) *n_z = n_z0;
	if ( n_x != NULL ) *n_x = n_x0;
	if ( x != NULL ) *x = x0;
	if ( y != NULL ) *y = y0;
	if ( z != NULL ) *z = z0;
    }

    return ret_p;
}

/**
 * @brief  begin_scan_zx_planes() で開始したスキャン・シーケンスの終了
 */
void fits_image::end_scan_zx_planes() const
{
    fits_image *thisp = (fits_image *)this;		/* 強制的にキャスト */
    this->data_array().end_scan_zx_planes();
    thisp->scan_use_flags &= ~Bit_use_zx_planes;
    if ( thisp->scan_use_flags == 0 ) thisp->scan_prms_rec = NULL;
}

/* scan the specified 3-D section.  A temporary buffer of 1-D array is */
/* prepared and scan_a_cube() returns it.                              */
/*  for(...) {      <- layer                                           */
/*    for(...) {    <- row                                             */
/*      for(...) {  <- column                                          */
/**
 * @brief  画像データ配列の3次元の範囲の要素のためのスキャン・シーケンスの開始
 *
 * 自身の画像データ配列の指定された3次元の範囲の要素を，1度にスキャンするため
 * の準備をします．3次元一括スキャンを行なう場合，次の手順で一連のAPIを使いま
 * す．<br>
 * - beginf_scan_a_cube() で準備をする．<br>
 * - scan_a_cube() を必要な数だけ呼び出し，キューブごとの処理を行なう．<br>
 * - end_scan_a_cube() で終了処理(一時バッファの開放)を行なう．<br>
 * scan_a_cube() は指定部分の要素が入った一時バッファのアドレス(32バイトでアラ
 * インされている)を返します．この一時バッファは，scan_a_cube() の場合はオブ
 * ジェクトの内部型が何であっても double 型で，scan_a_cube_f() の場合は float
 * 型で提供されます．なお，このバッファはオブジェクトで管理されているため，プ
 * ログラマが開放してはいけません．<br>
 * 画像データの統計をとる時などに使うメンバ関数です．
 *
 * @param     exp_fmt スキャンする範囲を示す文字列(例: "0:99,*")のための
 *                    フォーマット指定(printfと同様)
 * @param     ... exp_fmtに対応した可変長引数の各要素データ
 * @return    スキャンによる有効要素の数．エラーの場合は負値．
 * @note      "[1:100,*]" のように [] で囲むと，1-indexed として扱われます．
 */
long fits_image::beginf_scan_a_cube( const char *exp_fmt, ... ) const
{
    long ret;
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	ret = this->vbeginf_scan_a_cube(exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vbeginf_scan_a_cube() failed");
    }
    va_end(ap);
    return ret;
}

/**
 * @brief  画像データ配列の3次元の範囲の要素のためのスキャン・シーケンスの開始
 *
 * 自身の画像データ配列の指定された3次元の範囲の要素を，1度にスキャンするため
 * の準備をします．3次元一括スキャンを行なう場合，次の手順で一連のAPIを使いま
 * す．<br>
 * - beginf_scan_a_cube() で準備をする．<br>
 * - scan_a_cube() を必要な数だけ呼び出し，キューブごとの処理を行なう．<br>
 * - end_scan_a_cube() で終了処理(一時バッファの開放)を行なう．<br>
 * scan_a_cube() は指定部分の要素が入った一時バッファのアドレス(32バイトでアラ
 * インされている)を返します．この一時バッファは，scan_a_cube() の場合はオブ
 * ジェクトの内部型が何であっても double 型で，scan_a_cube_f() の場合は float
 * 型で提供されます．なお，このバッファはオブジェクトで管理されているため，プ
 * ログラマが開放してはいけません．<br>
 * 画像データの統計をとる時などに使うメンバ関数です．
 *
 * @param     exp_fmt スキャンする範囲を示す文字列(例: "0:99,*")のための
 *                    フォーマット指定(printfと同様)
 * @param     ap exp_fmtに対応した可変長引数のリスト
 * @return    スキャンによる有効要素の数．エラーの場合は負値．
 * @note      "[1:100,*]" のように [] で囲むと，1-indexed として扱われます．
 */
long fits_image::vbeginf_scan_a_cube( const char *exp_fmt, va_list ap ) const
{
    fits_image *thisp = (fits_image *)this;		/* 強制的にキャスト */

    thisp->prepare_scan_a_section();
    thisp->scan_use_flags |= Bit_use_cube;

    return this->data_array().vbeginf_scan_a_cube( exp_fmt, ap );
}

/**
 * @brief  画像データ配列の3次元の範囲の要素のためのスキャン・シーケンスの開始
 *
 * 自身の画像データ配列の指定された3次元の範囲の要素を，1度にスキャンするため
 * の準備をします．3次元一括スキャンを行なう場合，次の手順で一連のAPIを使いま
 * す．<br>
 * - begin_scan_a_cube() で準備をする．<br>
 * - scan_a_cube() を必要な数だけ呼び出し，キューブごとの処理を行なう．<br>
 * - end_scan_a_cube() で終了処理(一時バッファの開放)を行なう．<br>
 * scan_a_cube() は指定部分の要素が入った一時バッファのアドレス(32バイトでアラ
 * インされている)を返します．この一時バッファは，scan_a_cube() の場合はオブ
 * ジェクトの内部型が何であっても double 型で，scan_a_cube_f() の場合は float
 * 型で提供されます．なお，このバッファはオブジェクトで管理されているため，プ
 * ログラマが開放してはいけません．<br>
 * 画像データの統計をとる時などに使うメンバ関数です．
 *
 * @param     col_index 列位置
 * @param     col_size 列サイズ
 * @param     row_index 行位置
 * @param     row_size 行サイズ
 * @param     layer_index レイヤ位置
 * @param     layer_size レイヤサイズ
 * @return    スキャンによる有効要素の数．エラーの場合は負値．
 */
long fits_image::begin_scan_a_cube( long col_index, long col_size,
				    long row_index, long row_size,
				    long layer_index, long layer_size ) const
{
    fits_image *thisp = (fits_image *)this;		/* 強制的にキャスト */
    size_t z_col_size = fits_seclen_to_mdarray_seclen(col_size);
    size_t z_row_size = fits_seclen_to_mdarray_seclen(row_size);
    size_t z_layer_size = fits_seclen_to_mdarray_seclen(layer_size);

    thisp->prepare_scan_a_section();
    thisp->scan_use_flags |= Bit_use_cube;

    return this->data_array().begin_scan_a_cube( col_index, z_col_size, 
					      row_index, z_row_size,
					      layer_index, z_layer_size );
}

/* double version */
/**
 * @brief  画像データ配列の3次元の範囲の要素を連続的にスキャン (返り値はdouble)
 */
double *fits_image::scan_a_cube( long *n_x, long *n_y, long *n_z, 
				 long *x, long *y, long *z ) const
{
    double *ret_p;
    fits_image *thisp = (fits_image *)this;		/* 強制的にキャスト */
    size_t n_x0, n_y0, n_z0;
    ssize_t x0, y0, z0;
    ret_p = this->data_array().scan_a_cube_via_udf( thisp->scan_func_cnv_nd_x2d, 
						 thisp->scan_prms_rec.ptr(), 
						 &n_x0, &n_y0, &n_z0, 
						 &x0, &y0, &z0 );
    if ( ret_p != NULL ) {
	if ( n_x != NULL ) *n_x = n_x0;
	if ( n_y != NULL ) *n_y = n_y0;
	if ( n_z != NULL ) *n_z = n_z0;
	if ( x != NULL ) *x = x0;
	if ( y != NULL ) *y = y0;
	if ( z != NULL ) *z = z0;
    }

    return ret_p;
}

/* float version */
/**
 * @brief  画像データ配列の3次元の範囲の要素を連続的にスキャン (返り値はfloat)
 */
float *fits_image::scan_a_cube_f( long *n_x, long *n_y, long *n_z, 
				  long *x, long *y, long *z ) const
{
    float *ret_p;
    fits_image *thisp = (fits_image *)this;		/* 強制的にキャスト */
    size_t n_x0, n_y0, n_z0;
    ssize_t x0, y0, z0;
    ret_p = this->data_array().scan_a_cube_f_via_udf(thisp->scan_func_cnv_nd_x2f, 
						  thisp->scan_prms_rec.ptr(), 
						  &n_x0, &n_y0, &n_z0, 
						  &x0, &y0, &z0);
    if ( ret_p != NULL ) {
	if ( n_x != NULL ) *n_x = n_x0;
	if ( n_y != NULL ) *n_y = n_y0;
	if ( n_z != NULL ) *n_z = n_z0;
	if ( x != NULL ) *x = x0;
	if ( y != NULL ) *y = y0;
	if ( z != NULL ) *z = z0;
    }

    return ret_p;
}

/**
 * @brief  begin_scan_a_cube() で開始したスキャン・シーケンスの終了
 */
void fits_image::end_scan_a_cube() const
{
    fits_image *thisp = (fits_image *)this;		/* 強制的にキャスト */
    this->data_array().end_scan_a_cube();
    thisp->scan_use_flags &= ~Bit_use_cube;
    if ( thisp->scan_use_flags == 0 ) thisp->scan_prms_rec = NULL;
}


/*
 * etc.
 */

/**
 * @brief  テンポラリオブジェクトのためのshallow copy属性の付与
 *
 *  shallow copy を許可する場合に使用します(配列データに対して有効)．
 *
 * @note   一時オブジェクトの return の直前で使用する．
 */
/* 注意: 参照を返したいところだが，return 文でそれをやるとインスタンス */
/*       を2回コピーされるというおかしな事になるので void になっている */
void fits_image::set_scopy_flag()
{
    this->fits_hdu::set_scopy_flag();
    this->data_array().set_scopy_flag();
    return;
}


/* 
 * protected member functions
 */

/* get_idx_3d_cs() は inline にしたのでヘッダファイルに移動した */

#if 0
/* 新しい版(inlineのやつと等価)．if〜else で書くと，こうなる */
long fits_image::get_idx_3d_cs( long ix0, long ix1, long ix2 ) const
{
    if ( ix1 == FITS::INDEF ) {
      /* 1-dim */
      if ( ix0 < 0 || this->length() <= ix0 ) return -1;
      else return ix0;
    }
    else {
      /* 2-dim or 3-dim */
      if ( ix2 == FITS::INDEF ) {
	/* 2-dim */
	if ( ix0 < 0 || this->col_length() <= ix0 ) return -1;
	else {
	  if ( ix1 < 0 || this->row_layer_length() <= ix1 ) return -1;
	  else return ix0 + this->col_length() * ix1;
	}
      }
      else {
	/* 3-dim */
	if ( ix0 < 0 || this->col_length() <= ix0 ) return -1;
	else {
	  if ( ix1 < 0 || this->row_length() <= ix1 ) return -1;
	  else {
	    if ( ix2 < 0 || this->layer_length() <= ix2 ) return -1;
	    else return ix0 + this->col_length() * ix1
			    + this->col_row_length() * ix2;
	  }
	}
      }
    }
}
/* これは古い mdarray::get_idx_3d_cs() と同じ内容 */
long fits_image::get_idx_3d_cs( long ix0, long ix1, long ix2 ) const
{
    long idx;
    long max;
    if ( this->dim_length() == 0 ) return -1;
    if ( ix1 == FITS::INDEF ) max = this->length();
    else max = this->col_length();
    if ( 0 <= ix0 && ix0 < max ) idx = ix0;
    else return -1;
    if ( 1 < this->dim_length() && ix1 != FITS::INDEF ) {
	if ( ix2 == FITS::INDEF ) 
	    max = this->row_length() * this->layer_length();
	else max = this->row_length();
	if ( 0 <= ix1 && ix1 < max ) {
	    idx += this->col_length() * ix1;
	}
	else return -1;
    }
    if ( 2 < this->dim_length() && ix2 != FITS::INDEF ) {
	if ( 0 <= ix2 ) idx += this->col_length() * this->row_length() * ix2;
	else return -1;
    }
    if ( this->data_rec->data_ptr_cs() == NULL ) return -1;
    if ( this->length() <= idx ) return -1;
    return idx;
}
#endif

/**
 * @brief  ストリームから Header Unit と Data Unit を読む
 *
 * @note   このメンバ関数は protected です．
 */
ssize_t fits_image::read_stream( cstreamio &sref )
{
    return this->image_load(NULL,sref,NULL,false,NULL);
}

/**
 * @brief  ストリームから Header Unit と Data Unit を読む
 *
 * @note   このメンバ関数は protected です．
 */
ssize_t fits_image::read_stream( cstreamio &sref, size_t max_bytes_read )
{
    return this->image_load(NULL,sref,NULL,false,&max_bytes_read);
}

/**
 * @brief  ストリームから必要に応じて Header Unit を読み，Data Unit を読む
 *
 * @note   このメンバ関数は protected です．
 */
ssize_t fits_image::read_stream( fits_hdu *objp, cstreamio &sref )
{
    return this->image_load(objp,sref,NULL,false,NULL);
}

/**
 * @brief  ストリームから必要に応じて Header Unit を読み，Data Unit を読む
 *
 * @note   このメンバ関数は protected です．
 */
ssize_t fits_image::read_stream( fits_hdu *objp, cstreamio &sref, 
				const char *section_to_read, bool is_last_hdu )
{
    return this->image_load(objp,sref,section_to_read,is_last_hdu,NULL);
}

/**
 * @brief  ストリームから必要に応じて Header Unit を読み，Data Unit を読む
 *
 * @note   このメンバ関数は protected です．
 */
ssize_t fits_image::read_stream( fits_hdu *objp, cstreamio &sref, 
				 const char *section_to_read, bool is_last_hdu,
				 size_t max_bytes_read )
{
    return this->image_load(objp,sref,section_to_read,is_last_hdu,
			    &max_bytes_read);
}

/**
 * @brief  データパートの保存 or チェックサムの計算 (protected)
 *
 * @note このメンバ関数ではエンディアンは調整しないので呼び出す前にやっておく
 *       こと．<br>
 *       このメンバ関数は，<br>
 *         fits_image::write_stream() <br>
 *         -> fits_hdu::write_stream() <br>
 *         -> fits_image::save_or_check_data() <br>
 *       という経路で呼ばれる．
 * @note   このメンバ関数は protected です．
 */
ssize_t fits_image::save_or_check_data( cstreamio *sptr, void *c_sum_info )
{
    ssize_t return_value = -1;
    size_t sz;
    ssize_t len_written;
    fitsio_csum *csum_info = (fitsio_csum *)c_sum_info;

    //err_report1(__FUNCTION__,"DEBUG","called!! adr=[%zx]",(size_t)sptr);

    /* write or calculate data */
    sz = this->data_array().byte_length();
    if ( sz == 0 ) len_written = 0;
    else {
       try {
	  len_written = write_stream_or_get_csum(this->data_array().data_ptr_cs(),
						 sz, sptr, csum_info);
       }
       catch (...) {
	  err_throw(__FUNCTION__,"FATAL",
		    "unexpected exception in write_stream_or_get_csum()");
       }
    }

    if ( len_written != (ssize_t)sz ) {
	err_report(__FUNCTION__,"ERROR","write_stream_or_get_csum() failed");
	goto quit;
    }

    /* write or calculate blank */
    if ( len_written % FITS::FILE_RECORD_UNIT != 0 ) {
	ssize_t len;
	mdarray tmp_buf(1, false);
	sz = FITS::FILE_RECORD_UNIT - ( len_written % FITS::FILE_RECORD_UNIT );
	try {
	    tmp_buf.resize(sz);
	}
	catch (...) {
	    err_throw(__FUNCTION__,"FATAL","tmp_buf.resize() failed");
	}
	try {
	    len = write_stream_or_get_csum( tmp_buf.data_ptr_cs(), sz, 
					    sptr, csum_info );
	}
	catch (...) {
	    err_throw(__FUNCTION__,"FATAL",
		      "unexpected exception in write_stream_or_get_csum()");
	}
	if ( len != (ssize_t)sz ) {
	    err_report(__FUNCTION__,"ERROR",
		       "write_stream_or_get_csum() failed");
	    goto quit;
	}
	len_written += len;
    }

    return_value = len_written;
 quit:
    return return_value;
}

/**
 * @brief  Header Unit と Data Unit をストリームに書き込む
 *
 * @note   このメンバ関数は protected です．
 */
ssize_t fits_image::write_stream( cstreamio &sref )
{
    ssize_t return_value = -1;
    ssize_t written_total_bytes;

    //err_report(__FUNCTION__,"DEBUG","called!!");

    /* setup system header of this HDU */
    this->setup_sys_header();

    /* adjust endian for data part */
    this->data_array().mdarray::reverse_endian(false);

    /* Calculate chksum (if required), and write header+data to stream      */
    /* Note that fits_hdu::write_stream() calls this->save_or_check_data(). */
    try {
	written_total_bytes = this->fits_hdu::write_stream(sref);
	if ( written_total_bytes < 0 ) {
	    err_report(__FUNCTION__,"ERROR",
		       "this->fits_hdu::write_stream() failed");
	    goto quit;
	}
    }
    catch (...) {
	/* restore endian for data part */
	this->data_array().mdarray::reverse_endian(false);
	err_throw(__FUNCTION__,"FATAL","caught exception");
    }

    return_value = written_total_bytes;
 quit:
    /* restore endian for data part */
    this->data_array().mdarray::reverse_endian(false);

    return return_value;
}

/**
 * @brief  Header Unit と Data Unit をストリームに書き込んだ時のバイト長を取得
 *
 * @note   このメンバ関数は protected です．
 */
ssize_t fits_image::stream_length()
{
    ssize_t return_value = -1;
    ssize_t header_total_bytes;
    size_t data_total_bytes;

    /* setup system header of this HDU */
    this->setup_sys_header();

    header_total_bytes = fits_hdu::stream_length();
    if ( header_total_bytes < 0 ) {
	err_report(__FUNCTION__,"ERROR",
		   "this->fits_hdu::stream_length() failed");
	goto quit;
    }

    data_total_bytes = this->data_array().byte_length();
    /* 2880の倍々にする */
    if ( data_total_bytes % FITS::FILE_RECORD_UNIT != 0 ) {
	size_t u = data_total_bytes / FITS::FILE_RECORD_UNIT;
	data_total_bytes = FITS::FILE_RECORD_UNIT * ( u + 1 );
    }

    return_value = header_total_bytes + data_total_bytes;
 quit:
    return return_value;
}


/**
 * @brief  fits_image で管理している項目について，ヘッダオブジェクトを更新
 *
 * @note   このメンバ関数は protected です．
 */
fits_image &fits_image::setup_sys_header()
{
    fits_header &hdr_rec = this->header_rec;

    //err_report(__FUNCTION__,"DEBUG","called!");

    /* 基本的なヘッダを作成(fits_hduクラスのメンバ関数) */
    this->fits_hdu::setup_sys_header();

    //err_report(__FUNCTION__,"DEBUG"," start: set to false!");
    this->header_set_sysrecords_prohibition(false);
    try {
	long i, idx0;
	bool found;

	idx0 = hdr_rec.index("NAXIS");
    
	if ( hdr_rec.at(idx0).type() != FITS::LONGLONG_T ||
	     hdr_rec.at(idx0).lvalue() != this->dim_length() ) {
	    hdr_rec.at(idx0).assign(this->dim_length());
	}

	if ( 1 /* 0 < this->size_size_rec */ ) {
	    long long bitpix = 0;
	    bitpix = 8 * image_type_to_size_type(this->type_rec);
	    if ( bitpix == 0 ) {
		err_throw(__FUNCTION__,"FATAL","unsupported type");
	    }
	    if ( hdr_rec.at("BITPIX").type() != FITS::LONGLONG_T ||
		 hdr_rec.at("BITPIX").llvalue() != bitpix ) {
		hdr_rec.at("BITPIX").assign(bitpix);
	    }
	}

	for ( i=0 ; i < this->dim_length() ; i++ ) {
	    long idx;
	    tstring keyword;
	    fits_header_record rec_src;
	    const long long nxs = this->length(i);

	    keyword.assignf("NAXIS%ld",i+1);

	    idx = hdr_rec.index(keyword.cstr());

	    /* 存在している場合，できるだけ元のレコードをそのまま使い */
	    /* 必要な場合のみ値を更新する                             */
	    if ( 0 <= idx ) {
		rec_src = hdr_rec.at_cs(idx);
	    }
	    else {
		fits::header_def def = {"", "", ""};
		def.keyword = keyword.cstr();
	        rec_src.assign(def);
	        rec_src.assign_default_comment(FITS::IMAGE_HDU);
	    }

	    /* 順番が正しい場合 */
	    if ( idx == idx0 + 1 ) {
		if ( hdr_rec.at(idx).type() != FITS::LONGLONG_T ||
		     hdr_rec.at(idx).llvalue() != nxs ) {
		    hdr_rec.at(idx).assign(nxs);
		}
		idx0 = idx;
	    }
	    /* 順番が正しくない場合 */
	    else {
		if ( 0 <= idx ) {
		    hdr_rec.erase_records(idx,1);
		    if ( idx < idx0 ) idx0--;
		}
		hdr_rec.insert(idx0+1, rec_src);
		if ( hdr_rec.at(idx0+1).type() != FITS::LONGLONG_T ||
		     hdr_rec.at(idx0+1).llvalue() != nxs ) {
		    hdr_rec.at(idx0+1).assign(nxs);
		}
		idx0++;
	    }
	}

	/* 残骸があるかもしれないので消していく */
	do {
	    long idx;
	    tstring keyword;
	    found = false;

	    keyword.assignf("NAXIS%ld",i+1);
	    idx = hdr_rec.index(keyword.cstr());
	    if ( 0 <= idx ) {
		found = true;
		hdr_rec.erase_records(idx,1);
	    }

	    i++;
	} while ( found != false );

	/* insert BZERO, BSCALE, BLANK, and BUNIT, if they are set */
#ifdef BUNIT_IS_SPECIAL
	idx0 = hdr_rec.index("BUNIT");
	if ( 0 < this->bunit_rec.length() ) {
	    if ( idx0 < 0 ) {
		const char *search_kwds[] = {"BZERO","BSCALE","BLANK",NULL};
		hdr_rec.insert(
			   this->find_sysheader_insert_point(search_kwds), 
			   "BUNIT", "", "");
		idx0 = hdr_rec.index("BUNIT");
		hdr_rec.record(idx0).assign(this->bunit_rec.cstr());
		hdr_rec.record(idx0).assign_default_comment(FITS::IMAGE_HDU);
	    }
	}
	else {
	    if ( 0 <= idx0 && 0 < hdr_rec.record(idx0).value_length() ) {
		hdr_rec.erase(idx0);
	    }
	}
#endif
	idx0 = hdr_rec.index("BLANK");
	if ( 0 < this->blank_rec.length() ) {
	    if ( idx0 < 0 ) {
		const char *search_kwds[] = {"BZERO","BSCALE",NULL};
		hdr_rec.insert(
			   this->find_sysheader_insert_point(search_kwds), 
			   "BLANK", this->blank_rec.cstr(), "");
		idx0 = hdr_rec.index("BLANK");
		hdr_rec.record(idx0).assign_default_comment(FITS::IMAGE_HDU);
	    }
	}
	else {
	    if ( 0 <= idx0 && 0 < hdr_rec.record(idx0).value_length() ) {
		hdr_rec.erase(idx0);
	    }
	}
	idx0 = hdr_rec.index("BSCALE");
	if ( 0 < this->bscale_rec.length() ) {
	    if ( idx0 < 0 ) {
		const char *search_kwds[] = {"BZERO",NULL};
		hdr_rec.insert(
			   this->find_sysheader_insert_point(search_kwds), 
			   "BSCALE", this->bscale_rec.cstr(), "");
		idx0 = hdr_rec.index("BSCALE");
		hdr_rec.record(idx0).assign_default_comment(FITS::IMAGE_HDU);
	    }
	}
	else {
	    if ( 0 <= idx0 && 0 < hdr_rec.record(idx0).value_length() ) {
		hdr_rec.erase(idx0);
	    }
	}
	idx0 = hdr_rec.index("BZERO");
	if ( 0 < this->bzero_rec.length() ) {
	    if ( idx0 < 0 ) {
		const char *search_kwds[] = {"BSCALE",NULL};
		hdr_rec.insert(
			   this->find_sysheader_insert_point(search_kwds), 
			   "BZERO", this->bzero_rec.cstr(), "");
		idx0 = hdr_rec.index("BZERO");
		hdr_rec.record(idx0).assign_default_comment(FITS::IMAGE_HDU);
	    }
	}
	else {
	    if ( 0 <= idx0 && 0 < hdr_rec.record(idx0).value_length() ) {
		hdr_rec.erase(idx0);
	    }
	}
    }
    catch (...) {
	this->header_set_sysrecords_prohibition(true);
	err_throw(__FUNCTION__,"FATAL","caught exception");
    }
    this->header_set_sysrecords_prohibition(true);

    //err_report(__FUNCTION__,"DEBUG"," end: set to true!");

    return *this;
}


/* 
 * private member functions
 */

/** 
 * @brief  FITS ヘッダオブジェクトで BZERO，BSCALEなどの挿入ポイントを検索
 *
 * @note   このメンバ関数は private です．
 */
/* This member function is private.      */
long fits_image::find_sysheader_insert_point( const char *search_kwds[] ) const
{
    long i, idx, ret = 0;
    const fits_header &hdr_rec = this->header_rec;
    const char *kwds[] = {"SIMPLE","XTENSION", "BITPIX","NAXIS","EXTEND",
#ifdef FMTTYPE_IS_SPECIAL
			  "FMTTYPE","FTYPEVER", 
#endif
			  "PCOUNT","GCOUNT", 
			  "EXTNAME", "EXTVER", "EXTLEVEL", NULL};

    for ( i = 0 ; ; i++ ) {
	tstring kw;
	kw.printf("NAXIS%ld",i+1);
	idx = hdr_rec.index(kw.cstr());
	if ( 0 <= idx ) {
	    if ( ret < idx + 1 ) ret = idx + 1;
	}
	else {
	    break;
	}
    }

    for ( i = 0 ; kwds[i] != NULL ; i++ ) {
	idx = hdr_rec.index(kwds[i]);
	if ( 0 <= idx && ret < idx + 1 ) ret = idx + 1;
    }

    if ( search_kwds != NULL ) {
	for ( i = 0 ; search_kwds[i] != NULL ; i++ ) {
	    idx = hdr_rec.index(search_kwds[i]);
	    if ( 0 <= idx && ret < idx + 1 ) ret = idx + 1;
	}
    }

    return ret;
}

/**
 * @brief  blank 値をセット or リセット (引数がNULLでリセット)
 *
 * @note   このメンバ関数は private です．
 */
void fits_image::set_blank( const long long *new_blank_ptr )
{
    int the_type = this->type_rec;

    if ( new_blank_ptr == NULL ) {
      this->blank_r_rec = INDEF_LLONG;
      this->blank_w_rec = get_indef_for_type(the_type);
      this->blank_longlong_rec = INDEF_LLONG;
      this->blank_long_rec = INDEF_LONG;
      this->blank_short_rec = INDEF_SHORT;
      this->blank_byte_rec = INDEF_UCHAR;
      this->blank_is_set_rec = false;
    }
    else {
      this->blank_r_rec = *new_blank_ptr;
      this->blank_w_rec = get_blank_for_type(the_type, *new_blank_ptr);
      if ( MIN_LLONG <= *new_blank_ptr && *new_blank_ptr <= MAX_LLONG )
	  this->blank_longlong_rec = *new_blank_ptr;
      else this->blank_longlong_rec = INDEF_LLONG;
      if ( MIN_LONG <= *new_blank_ptr && *new_blank_ptr <= MAX_LONG ) 
	  this->blank_long_rec = *new_blank_ptr;
      else this->blank_long_rec = INDEF_LONG;
      if ( MIN_SHORT <= *new_blank_ptr && *new_blank_ptr <= MAX_SHORT ) 
	  this->blank_short_rec = *new_blank_ptr;
      else this->blank_short_rec = INDEF_SHORT;
      if ( MIN_UCHAR <= *new_blank_ptr && *new_blank_ptr <= MAX_UCHAR ) 
	  this->blank_byte_rec = *new_blank_ptr;
      else this->blank_byte_rec = INDEF_UCHAR;
      this->blank_is_set_rec = true;
    }

    return;
}

/**
 * @brief  縮退した z 軸の値を取得
 *
 * @note   このメンバ関数は private です．
 */
long fits_image::get_degenerated_zindex( long num_axisx, long axis_z, 
					 va_list ap ) const
{
    long degenerate_zidx;
    long i, sz = 1;
    if ( this->dim_length() < 3 || num_axisx < 1 ) return -1;
    degenerate_zidx = axis_z;
    for ( i=3 ; i < this->dim_length() && i < num_axisx ; i++ ) {
	long v = va_arg(ap,long);
	sz *= this->length(i-1);
	degenerate_zidx += sz * v;
    }

    if ( sz * this->length(i-1) <= degenerate_zidx ) return -1;
    return degenerate_zidx;
}

/**
 * @brief  this->data_rec 内のゼロ点を更新
 *
 * @note   このメンバ関数は private です．
 */
void fits_image::update_zero_of_data_rec()
{
    double zval = 0;
    zval -= this->bzero_double_rec;
    zval /= this->bscale_double_rec;
    if ( this->type() == FITS::DOUBLE_T ) {
	fits::double_t v = zval;
	this->data_array().mdarray::assign_default((const void *)&v);
    }
    else if ( this->type() == FITS::FLOAT_T ) {
	fits::float_t v = zval;
	this->data_array().mdarray::assign_default((const void *)&v);
    }
    else if ( this->type() == FITS::SHORT_T ) {
	if ( MIN_DOUBLE_ROUND_INT16 <= zval && zval <= MAX_DOUBLE_ROUND_INT16 ) {
	    fits::short_t v = round_d2i32(zval);
	    this->data_array().mdarray::assign_default((const void *)&v);
	}
	else this->data_array().mdarray::assign_default((const void *)&INDEF_INT16);
    }
    else if ( this->type() == FITS::LONG_T ) {
	if ( MIN_DOUBLE_ROUND_INT32 <= zval && zval <= MAX_DOUBLE_ROUND_INT32 ) {
	    fits::long_t v = round_d2i32(zval);
	    this->data_array().mdarray::assign_default((const void *)&v);
	}
	else this->data_array().mdarray::assign_default((const void *)&INDEF_INT32);
    }
    else if ( this->type() == FITS::BYTE_T ) {
	if ( MIN_DOUBLE_ROUND_UCHAR <= zval && zval <= MAX_DOUBLE_ROUND_UCHAR ) {
	    fits::byte_t v = round_d2i32(zval);
	    this->data_array().mdarray::assign_default((const void *)&v);
	}
	else this->data_array().mdarray::assign_default((const void *)&INDEF_UCHAR);
    }
    else if ( this->type() == FITS::LONGLONG_T ) {
	if ( MIN_DOUBLE_ROUND_INT64 <= zval && zval <= MAX_DOUBLE_ROUND_INT64 ) {
	    fits::longlong_t v = round_d2i64(zval);
	    this->data_array().mdarray::assign_default((const void *)&v);
	}
	else this->data_array().mdarray::assign_default((const void *)&INDEF_INT64);
    }
    else {
	this->data_array().mdarray::assign_default((const void *)NULL);
    }
}

/**
 * @brief  要素番号の修正
 *
 * @note   このメンバ関数は private です．
 */
void fits_image::fix_indices( long *col_index, long *row_index, 
			      long *layer_index ) const
{
    if ( *col_index < 0 ) {
	*col_index = 0;
    }
    else if ( this->col_length() <= *col_index ) {
	*col_index = this->col_length() - 1;
    }
    if ( *row_index < 0 ) {
	*row_index = 0;
    }
    else if ( this->row_length() <= *row_index ) {
	*row_index = this->row_length() - 1;
    }
    if ( *layer_index < 0 ) {
	*layer_index = 0;
    }
    else if ( this->layer_length() <= *layer_index ) {
	*layer_index = this->layer_length() - 1;
    }
}

#include "private/parse_section_expression.cc"

/**
 * @brief  read_image_section()から再帰的に呼び出されn次元の部分読み出しを実行
 *
 * @note    private な関数です．
 * @attention  ndim を 0 にしてはいけない．
 */
static int read_section_r( cstreamio &sref, size_t sz_bytes,
			   const size_t *n_axisx_org,
			   const long s_begin[], const long s_length[],
			   size_t ndim, size_t blen_block, 
			   const size_t *max_bytes_ptr,
			   size_t *rest_skip,
			   char **dest_buf_ptr, size_t *len_read_all )
{
    int ret_value = -1;
    size_t dim_ix = ndim - 1;			/* 現在の次元番号(0-indexed) */
    const long s_start = s_begin[dim_ix];
    const long s_len = s_length[dim_ix];
    const long s_last1 = s_begin[dim_ix] + s_length[dim_ix];

    /* 次元番号が 0 の場合 */
    if ( dim_ix == 0 ) {
	size_t len_in;
	if ( 0 < s_start ) {					/* 飛ばし */
	    len_in = sz_bytes * s_start;
	    if ( max_bytes_ptr != NULL ) {
		if ( (*max_bytes_ptr) < (*len_read_all) + len_in ) {
		    len_in = (*max_bytes_ptr) - (*len_read_all);
		}
	    }
	    *rest_skip += len_in;
	    *len_read_all += len_in;
	}
	if ( 0 < s_len ) {					/* 読み */
	    ssize_t len_read;
	    /* *rest_skip に溜っている分を一気に rskip する */
	    if ( 0 < (*rest_skip) ) {
		ssize_t len_skiped;
		try {
		    len_skiped = sref.rskip((*rest_skip));
		}
		catch (...) {
		    err_throw(__FUNCTION__,"FATAL","unexpected exception");
		}
		if ( len_skiped < 0 || (size_t)len_skiped != (*rest_skip) ) {
		    err_report(__FUNCTION__,"ERROR","sref.rskip() failed");
		    goto quit;
		}
		*rest_skip = 0;
	    }
	    /* 読む */
	    len_in = sz_bytes * s_len;
	    if ( max_bytes_ptr != NULL ) {
		if ( (*max_bytes_ptr) < (*len_read_all) + len_in ) {
		    len_in = (*max_bytes_ptr) - (*len_read_all);
		}
	    }
	    try {
		len_read = sref.read((void *)(*dest_buf_ptr),len_in);
	    }
	    catch (...) {
		err_throw(__FUNCTION__,"FATAL","unexpected exception");
	    }
	    if ( len_read < 0 || (size_t)len_read != len_in ) {
		err_report(__FUNCTION__,"ERROR","sref.read() failed");
		goto quit;
	    }
	    *dest_buf_ptr += len_in;
	    *len_read_all += len_in;
	}
	if ( s_last1 < (long)(n_axisx_org[dim_ix]) ) {		/* 飛ばし */
	    len_in = sz_bytes* ((long)(n_axisx_org[dim_ix]) - s_last1);
	    if ( max_bytes_ptr != NULL ) {
		if ( (*max_bytes_ptr) < (*len_read_all) + len_in ) {
		    len_in = (*max_bytes_ptr) - (*len_read_all);
		}
	    }
	    *rest_skip += len_in;
	    *len_read_all += len_in;
	}
    }
    /* 次元番号が 1 以上の場合 */
    else {
	/* 飛ばす場合のバイト数 */
	long j;
	blen_block /= n_axisx_org[dim_ix];
	/* 読む or 飛ばす */
	for ( j = 0 ; j < (long)(n_axisx_org[dim_ix]) ; j++ ) {
	    /* 選択範囲にある場合 */
	    if ( s_start <= j && j < s_last1 ) {
		if ( read_section_r(sref, sz_bytes, n_axisx_org,
			  s_begin, s_length, dim_ix, blen_block, max_bytes_ptr,
			  rest_skip,
			  dest_buf_ptr, len_read_all) < 0 ) {
		    err_report(__FUNCTION__,"ERROR",
			       "fits_hdu::read_section_r() failed");
		    goto quit;
		}
	    }
	    /* 選択範囲外の場合 */
	    else {
		size_t len_in = blen_block;
		if ( max_bytes_ptr != NULL ) {
		    if ( (*max_bytes_ptr) < (*len_read_all) + len_in ) {
			len_in = (*max_bytes_ptr) - (*len_read_all);
		    }
		}
		*rest_skip += len_in;
		*len_read_all += len_in;
	    }
	}
    }

    ret_value = 0;
 quit:
    return ret_value;
}

/**
 * @brief  ストリームからの n 次元の部分読み出しを実行
 *
 * @note   このメンバ関数は private です．
 */
int fits_image::read_image_section( cstreamio &sref, ssize_t sz_type,
				    const size_t *n_axisx_org, size_t ndim_org,
				    void *_sect_info, bool is_last_hdu,
				    const size_t *max_bytes_ptr, 
				    size_t *len_read_all )
{
    int ret_value = -1;
    size_t i, ndim;
    mdarray_size n_axisx;	/* 新しい次元サイズ情報 */
    section_exp_info *sect_info = (section_exp_info *)_sect_info;
    fits_header &hdr_rec = this->header_rec;

    size_t rest_skip;		/* 1回でまとめて rskip するためのキュー */
    char *dest_buf;
    char *dest_buf_current;

    /* 新しい次元数ndim: ndim_org と同じになるか 0 になるかのどちらか */
    ndim = ndim_org;
    n_axisx.resize(ndim_org);
    this->coord_offset_rec.resize(ndim_org);
    this->is_flipped_rec.resize(ndim_org);

    /* 範囲を修正する */
    for ( i=0 ; i < ndim_org ; i++ ) {
	const long c_len = n_axisx_org[i];
	bool fixed_vals = false;
	if ( sect_info->begin.length() <= i ) {	/* 指定されていない範囲 */
	    sect_info->begin[i] = 0;
	    sect_info->length[i] = c_len;
	    sect_info->flip_flag[i] = false;
	}
	else {					/* 指定されている範囲 */
	    if ( sect_info->length[i] == FITS::ALL ) {
		if ( sect_info->begin[i] < 0 ) {
		    sect_info->begin[i] = 0;
		    fixed_vals = true;
		}
		sect_info->length[i] = c_len - sect_info->begin[i];
	    }
	    /* 指定領域がはみ出している場合の処置 */
	    if ( sect_info->begin[i] < 0 ) {
		if ( sect_info->begin[i] + sect_info->length[i] <= 0 ) {
		    /* 読むべき長さはゼロ */
		    sect_info->length[i] = 0;
		}
		else {
		    /* 読むべき長さを小さくする */
		    sect_info->length[i] += sect_info->begin[i];
		}
		sect_info->begin[i] = 0;
		fixed_vals = true;
	    }
	    else if ( c_len <= sect_info->begin[i] ) {
		sect_info->begin[i] = 0;
		sect_info->length[i] = 0;
	    }
	    if ( c_len < sect_info->begin[i] + sect_info->length[i]) {
		sect_info->length[i] = c_len - sect_info->begin[i];
		fixed_vals = true;
	    }
	}
	n_axisx.at(i) = sect_info->length[i];
	/* もし 0 なら NAXIS=0 の状態で初期化しておわり */
	if ( n_axisx.at(i) == 0 ) {
	    ndim = 0;
	    this->coord_offset_rec.init();
	    this->is_flipped_rec.init();
	    err_report(__FUNCTION__,"WARNING",
	       "expression indicates out of range; no data unit will be read");
	    break;
	}
	if ( fixed_vals == true ) {
	    tstring rng_str;
	    if ( sect_info->zero_indexed == false ) {
		rng_str.printf("[%ld:%ld] (NAXIS=%ld)",
		       (long)(sect_info->begin[i] + 1),
		       (long)(sect_info->begin[i] + sect_info->length[i]),
		       (long)(i+1));
	    }
	    else {
		rng_str.printf("(%ld:%ld) (NAXIS=%ld)",
		       (long)(sect_info->begin[i]),
		       (long)(sect_info->begin[i] + sect_info->length[i] - 1),
		       (long)(i+1));
	    }
	    err_report1(__FUNCTION__,"NOTICE", 
			"fixed range to %s",rng_str.cstr());
	}
	this->coord_offset_rec[i] = sect_info->begin[i];
	this->is_flipped_rec[i] = sect_info->flip_flag[i];
    }

    /* バッファを初期化 */
    try {
	this->_init_all_data_recs(sz_type);
	this->data_array().reallocate(n_axisx.array_ptr_cs(), ndim, false);
    }
    catch (...) {
	err_throw(__FUNCTION__,"FATAL",
		  "this->data_array().reallocate() failed");
    }

    /* ヘッダを改変 */
    this->header_set_sysrecords_prohibition(false);
    try {
	hdr_rec.record("NAXIS").assign((long long)ndim);
	for ( i=0 ; i < ndim ; i++ ) {
	    long long ll = n_axisx.at(i);
	    tstring keyword;
	    keyword.assignf("NAXIS%ld",(long)(i+1));
	    hdr_rec.record(keyword.cstr()).assign(ll);
	}
	for ( ; i < ndim_org ; i++ ) {	/* 次元数が小さくなった場合 */
	    tstring keyword;
	    long idx;
	    keyword.assignf("NAXIS%ld",(long)(i+1));
	    idx = hdr_rec.index(keyword.cstr());
	    if ( 0 <= idx ) hdr_rec.erase_records(idx,1);
	}
    }
    catch (...) {
	this->header_set_sysrecords_prohibition(true);
	err_throw(__FUNCTION__,"FATAL","caught exception");
    }
    this->header_set_sysrecords_prohibition(true);

    dest_buf = (char *)(this->data_array().data_ptr());
    dest_buf_current = dest_buf;
    if ( 0 < ndim ) {
	size_t blen_org = this->data_array().bytes();
	for ( i=0 ; i < ndim ; i++ ) blen_org *= n_axisx_org[i];
	rest_skip = 0;					/* ←これを忘れるな */
	if ( read_section_r( sref, this->data_array().bytes(), n_axisx_org, 
		   sect_info->begin.array_ptr(), sect_info->length.array_ptr(),
		   ndim, blen_org, max_bytes_ptr, &rest_skip, 
		   &dest_buf_current, len_read_all ) < 0 ) {
	    goto quit;
	}
    }
    else {
	if ( 0 < ndim_org ) {
	    rest_skip = this->data_array().bytes();
	    for ( i=0 ; i < ndim_org ; i++ ) rest_skip *= n_axisx_org[i];
	}
	else rest_skip = 0;
	*len_read_all += rest_skip;
    }
    /* rest_skip に溜っている分を一気に rskip する   */
    /* (最後の読むべきHDUの場合はここは実行されない) */
    if ( is_last_hdu == false ) {
	if ( 0 < rest_skip ) {
	    ssize_t len_skiped;
	    try {
		len_skiped = sref.rskip(rest_skip);
	    }
	    catch (...) {
		err_throw(__FUNCTION__,"FATAL","unexpected exception");
	    }
	    if ( len_skiped < 0 || (size_t)len_skiped != rest_skip ) {
		err_report(__FUNCTION__,"ERROR","sref.rskip() failed");
		goto quit;
	    }
	    rest_skip = 0;
	}
    }

    if ( max_bytes_ptr != NULL ) {
	char *last1 = dest_buf + this->data_array().byte_length();
	/* 読み込みが完了してない場合は，その部分を0でうめる */
	if ( dest_buf_current < last1 ) {
	    c_memset(dest_buf_current, 0, last1 - dest_buf_current);
	}
    }

    /* 反転処理を行なう */
    for ( i=0 ; i < ndim ; i++ ) {
	if ( sect_info->flip_flag[i] == true ) {
	    this->data_array().flip(i, 0, n_axisx[i]);
	}
    }

    ret_value = 0;
 quit:
    return ret_value;
}

/**
 * @brief  ストリームから Image HDU を読み出し
 *
 * @note   このメンバ関数は private です．
 */
ssize_t fits_image::image_load( const fits_hdu *objp, cstreamio &sref, 
				const char *section_to_read, bool is_last_hdu,
				const size_t *max_bytes_ptr )
{
    ssize_t return_val = -1;
    size_t total_read_len = 0;
    size_t len_padding, npix_org_img, bytepix;
    ssize_t new_sz_type = 0;
    fits_header &hdr_rec = this->header_rec;
    long ndim, bitpix;
    long idx, i;
    section_exp_info section_info;
    mdarray_size n_axisx;
    bool reverse_endian_is_done = false;

    /* からっぽにする */
    this->init();
    
    /* まだヘッダを読んでいない場合 */
    if ( objp == NULL ) {
	ssize_t len_read;
	if ( max_bytes_ptr != NULL ) {
	    len_read = fits_hdu::read_stream( sref, *max_bytes_ptr );
	}
	else {
	    len_read = fits_hdu::read_stream( sref );
	}
	if ( len_read < 0 ) {
	    err_report(__FUNCTION__,"ERROR","fits_hdu::read_stream() failed");
	    goto quit;
	}
	total_read_len += len_read;
    }
    else {
	/* 引数の内容(ヘッダなど)をコピーする */
	this->fits_hdu::init(*objp);
    }

    if ( hdr_rec.index("SIMPLE") < 0 ) {
	const tstring &xtension = hdr_rec.record("XTENSION").svalue_cs();
	if ( xtension.cstr() == NULL ) {
	    err_report(__FUNCTION__,"ERROR","XTENSION keyword is not found");
	    goto quit;
	}
	if ( xtension.strcmp("IMAGE") != 0 ) {
	    err_report(__FUNCTION__,"ERROR","This HDU is not IMAGE");
	    goto quit;
	}
    }

    /* PCOUNT と GCOUNT とをチェックする(あれば) */
    if ( 0 <= (idx = hdr_rec.index("PCOUNT")) ) {
	if ( hdr_rec.record(idx).llvalue() != 0 ) {
	    err_report(__FUNCTION__,"ERROR","Unsupported PCOUNT");
	    goto quit;
	}
    }

    if ( 0 <= (idx = hdr_rec.index("GCOUNT")) ) {
	if ( hdr_rec.record(idx).llvalue() != 1 ) {
	    err_report(__FUNCTION__,"ERROR","Unsupported GCOUNT");
	    goto quit;
	}
    }

    /* BITPIX */
    bitpix = hdr_rec.record("BITPIX").llvalue();
    if ( bitpix == 8 ) {
	this->type_rec = FITS::BYTE_T;
	new_sz_type = UCHAR_ZT;
    }
    else if ( bitpix == 16 ) {
	this->type_rec = FITS::SHORT_T;
	new_sz_type = INT16_ZT;
    }
    else if ( bitpix == 32 ) {
	this->type_rec = FITS::LONG_T;
	new_sz_type = INT32_ZT;
    }
    else if ( bitpix == 64 ) {
	this->type_rec = FITS::LONGLONG_T;
	new_sz_type = INT64_ZT;
    }
    else if ( bitpix == -64 ) {
	this->type_rec = FITS::DOUBLE_T;
	new_sz_type = DOUBLE_ZT;
    }
    else if ( bitpix == -32 ) {
	this->type_rec = FITS::FLOAT_T;
	new_sz_type = FLOAT_ZT;
    }
    else {
	err_report1(__FUNCTION__,"ERROR","Invalid BITPIX: %ld",bitpix);
	goto quit;
    }

    /* NAXIS */
    ndim = hdr_rec.record("NAXIS").llvalue();
    if ( ndim < 0 ) {
	err_report(__FUNCTION__,"ERROR","Invalid NAXIS");
	goto quit;
    }

    /* 確保 */
    try {
	n_axisx.resize(ndim);
    }
    catch (...) {
	err_throw(__FUNCTION__,"FATAL","n_axisx.resize() failed");
    }

    if ( ndim == 0 ) {
	npix_org_img = 0;
    }
    else {
	/* 軸情報 */
	npix_org_img = 1;
	for ( i=0 ; i < ndim ; i++ ) {
	    long long ll;
	    tstring keyword;
	    keyword.assignf("NAXIS%ld",i+1);
	    idx = hdr_rec.index(keyword.cstr());
	    if ( idx < 0 ) {
		err_report1(__FUNCTION__,"ERROR","keyword [%s] is not found",
			    keyword.cstr());
		goto quit;
	    }
	    ll = hdr_rec.record(idx).llvalue();
	    /*
	     * これを許す(つまり NAXISn を 0 にする)と fv や ds9 でエラーになる
	     * やはり，許すべきではないか．．．2011/11/28
	     */
	    if ( ll <= 0 ) {
		err_report1(__FUNCTION__,"ERROR","Invalid %s value",
			    keyword.cstr());
		goto quit;
	    }
	    n_axisx.at(i) = ll;
	    npix_org_img *= n_axisx.at(i);
	}
    }

#ifdef BUNIT_IS_SPECIAL
    idx = hdr_rec.index("BUNIT");
    if ( 0 <= idx && 0 < hdr_rec.record(idx).svalue_cs().length() ) {
	this->bunit_rec = hdr_rec.record(idx).svalue();
    }
#endif
    idx = hdr_rec.index("BZERO");
    if ( 0 <= idx && 0 < hdr_rec.record(idx).svalue_cs().length() ) {
	this->bzero_rec = hdr_rec.record(idx).value();
	this->bzero_double_rec = hdr_rec.record(idx).dvalue();
    }
    idx = hdr_rec.index("BSCALE");
    if ( 0 <= idx && 0 < hdr_rec.record(idx).svalue_cs().length() ) {
	this->bscale_rec = hdr_rec.record(idx).value();
	this->bscale_double_rec = hdr_rec.record(idx).dvalue();
    }
    idx = hdr_rec.index("BLANK");
    if ( 0 <= idx && 0 < hdr_rec.record(idx).svalue_cs().length() ) {
	long long blnk = hdr_rec.record(idx).llvalue();
	this->blank_rec = hdr_rec.record(idx).value();
	this->set_blank(&blnk);
    }
    else {
	this->blank_rec = NULL;
	this->set_blank(NULL);
    }
    this->update_zero_of_data_rec();

    /* 部分読みの指定がある場合は，区間指定の文字列をパースする*/
    if ( section_to_read != NULL ) {
	if ( parse_section_expression(section_to_read, &section_info) < 0 ) {
	    err_report1(__FUNCTION__,"WARNING",
	           "syntax error in expression '%s'; ignored",section_to_read);
	    section_to_read = NULL;
	}
    }

    //err_report1(__FUNCTION__,"DEBUG","[1] total_read_len %zd",total_read_len);

    /* 部分読みの指定がある場合(ただしテーブル用の指定は無視する) */
    if ( section_to_read != NULL && section_info.sel_cols.length() == 0 ) {
	if ( read_image_section(sref, new_sz_type, n_axisx.array_ptr_cs(),
				ndim, (void *)(&section_info), is_last_hdu,
				max_bytes_ptr, &total_read_len) < 0 ) {
	    err_report(__FUNCTION__,"ERROR","read_image_section() failed;");
	    goto quit;
	}
    }
    /* 全部読みの場合 */
    else {
	size_t bytes_to_read;
	ssize_t len_read = 0;

	try {
	    this->_init_all_data_recs(new_sz_type);
	    this->data_array().reallocate(n_axisx.array_ptr_cs(), ndim, false);
	}
	catch (...) {
	    err_throw(__FUNCTION__,"FATAL",
		      "this->data_array().reallocate() failed");
	}

	bytes_to_read = this->data_array().byte_length();

	if ( 0 < bytes_to_read ) {
	    /* 読めるバイト数が制限されている場合 */
	    if ( max_bytes_ptr != NULL &&
		 (*max_bytes_ptr) < total_read_len + bytes_to_read ) {
		bytes_to_read = (*max_bytes_ptr) - total_read_len;
	    }
	}

	if ( 0 < bytes_to_read ) {
	    /*
	     * パターンA: ブロック単位で，read→byteswap を繰り返すコード
	     */
#if 1
	    /* i7 のマシンのテストでは 64kB 〜 128kB でベストだった */
	    const size_t blen_block_cd = 65536;		/* ブロックサイズ */
	    const size_t bytes = this->data_array().bytes();
	    const size_t len_block = 
		(blen_block_cd / bytes <= this->data_array().length()) 
		? blen_block_cd / bytes : this->data_array().length();
	    const size_t blen_block = bytes * len_block;

	    size_t blen_left = bytes_to_read;
	    size_t dest_idx = 0;
	    while ( 0 < blen_left ) {
		const size_t blen_rd = 
			    (blen_block <= blen_left) ? blen_block : blen_left;
		const size_t len_rd = blen_rd / bytes;
		ssize_t blen_ret;
		try {
		    blen_ret = sref.read( (char *)(this->data_array().data_ptr())
					  + bytes * dest_idx, blen_rd );
		}
		catch (...) {
		    err_throw(__FUNCTION__,"FATAL","unexpected exception");
		}
		if ( blen_ret < 0 || blen_ret != (ssize_t)blen_rd ) {
		    err_report(__FUNCTION__,"ERROR",
			       "sref.read() failed; blen_ret != blen_rd");
		    goto quit;
		}
		/* エンディアン調整 */
		this->data_array().mdarray::reverse_byte_order( false, 
						this->data_array().size_type(),
						dest_idx, len_rd );
		/* */
		dest_idx += len_rd;
 		blen_left -= blen_rd;
		/* */
		len_read += blen_rd;
	    }
	    reverse_endian_is_done = true;
#endif
	    /*
	     * パターンB: こちらは一気に読むコード．パターンAの方が速い
	     */
#if 0
	    try {
		len_read = sref.read(this->data_array().data_ptr(), bytes_to_read);
	    }
	    catch (...) {
		err_throw(__FUNCTION__,"FATAL","unexpected exception");
	    }
	    if ( len_read < 0 || len_read != (ssize_t)bytes_to_read ) {
		err_report(__FUNCTION__,"ERROR",
			   "sref.read() failed; len_read != bytes_to_read");
		goto quit;
	    }
#endif
	    total_read_len += len_read;

	    /* 読めるバイト数が制限されていて，全部読めなかった場合 */
	    if ( max_bytes_ptr != NULL && 
		 bytes_to_read < this->data_array().byte_length() ) {
		c_memset((char *)(this->data_array().data_ptr()) + bytes_to_read,
			 0, this->data_array().byte_length() - bytes_to_read);
	    }
	}

    }

    //err_report1(__FUNCTION__,"DEBUG","[2] total_read_len %zd",total_read_len);

    if ( is_last_hdu == false ) {

	/* 残りの余白を読み進める                        */
	/* (最後の読むべきHDUの場合はここは実行されない) */

	bytepix = this->data_array().bytes();
	if ( ((bytepix * npix_org_img) % FITS::FILE_RECORD_UNIT) != 0 ) {
	    len_padding = FITS::FILE_RECORD_UNIT
		- ((bytepix * npix_org_img) % FITS::FILE_RECORD_UNIT);
	}
	else {
	    len_padding = 0;
	}

	/* 読めるバイト数が制限されている場合 */
	if ( max_bytes_ptr != NULL &&
	     (*max_bytes_ptr) < total_read_len + len_padding ) {
	    len_padding = (*max_bytes_ptr) - total_read_len;
	}

	if ( 0 < len_padding ) {
	    bool chk_flg = false;
	    ssize_t len_skiped;
	    if ( hdr_rec.index("SIMPLE") == 0 ) {	/* Primary HDU */
		if ( 0 <= (idx=hdr_rec.index("EXTEND")) ) {
		    if ( hdr_rec.record(idx).bvalue() == true ) chk_flg = true;
		}
	    }
	    else {					/* not Primary HDU */
		chk_flg = true;
	    }
	    try {
		len_skiped = sref.rskip(len_padding);
	    }
	    catch (...) {
		err_throw(__FUNCTION__,"FATAL","unexpected exception");
	    }
	    if ( len_skiped < 0 || 
		 (chk_flg == true && len_skiped != (ssize_t)len_padding) ) {
		err_report1(__FUNCTION__,"ERROR",
		   "sref.rskip() failed; len_skiped = %ld",(long)len_skiped);
		err_report1(__FUNCTION__,"ERROR",
		   "                     len_padding = %ld",(long)len_padding);
		goto quit;
	    }
	    total_read_len += len_skiped;
	}
    }

    //err_report1(__FUNCTION__,"DEBUG","[3] total_read_len %zd",total_read_len);
    
    /* endian ひっくりかえし */
    if ( reverse_endian_is_done == false ) {
	this->data_array().mdarray::reverse_endian(false);
    }
    
    return_val = total_read_len;
 quit:
    return return_val;
}

/* イメージ編集 */

/*
 * BEGIN : convert_image_type()
 *
 * 注意: fits_table_col には convert_col_type() が存在する．内容は全く同じ
 *       なので，コードを変更する場合は同時に行なう事．
 */

struct fits_image_cnv_prms {
    /* アライメントに注意(sizeofが長いものから順に!) */
    double new_min;
    double new_max;
    double old_zero;
    double new_zero;
    double old_scale;
    double new_scale;
    long long old_blank;
    long long new_blank;
    bool old_blank_is_set;
};

namespace cnv_nd
{
#define PM05_FOR_ROUND(dv) (((dv) < 0) ? (dv)-0.5 : (dv)+0.5)
#define MAKE_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
static void fncname(const void *org_val_ptr, void *new_val_ptr, size_t n, int f_b, void *u_ptr) \
{ \
    const struct fits_image_cnv_prms *p = (const struct fits_image_cnv_prms *)u_ptr; \
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
	    *new_t_ptr = p->new_blank; \
	  } \
	  else { \
	    val = *org_t_ptr; \
	    val *= p_old_scale; \
	    val += p_old_zero; \
	    val -= p_new_zero; \
	    val /= p_new_scale; \
	    if ( isfinite(val) && p_new_min <= val && val <= p_new_max ) \
	      *new_t_ptr = (new_type)fnc(val); \
	    else *new_t_ptr = p->new_blank; \
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
	  if ( p->old_blank_is_set == true && p->old_blank == *org_t_ptr ) { \
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
	  if ( p->old_blank_is_set == true && p->old_blank == *org_t_ptr ) { \
	    *new_t_ptr = p->new_blank; \
	  } \
	  else { \
	    val = *org_t_ptr; \
	    val *= p_old_scale; \
	    val += p_old_zero; \
	    val -= p_new_zero; \
	    val /= p_new_scale; \
	    if ( isfinite(val) && p_new_min <= val && val <= p_new_max ) \
	      *new_t_ptr = (new_type)fnc(val); \
	    else *new_t_ptr = p->new_blank; \
	  } \
	  new_t_ptr += d; \
	  org_t_ptr += d; \
	} \
	for ( ; i < n ; i++ ) new_t_ptr[i] = new_t_ptr[0]; \
      } \
    } \
}
SLI__MDARRAY__DO_OPERATION_2TYPES(MAKE_FUNC,PM05_FOR_ROUND,PM05_FOR_ROUND,PM05_FOR_ROUND,PM05_FOR_ROUND,PM05_FOR_ROUND,PM05_FOR_ROUND,PM05_FOR_ROUND,PM05_FOR_ROUND,,,,,);
#undef MAKE_FUNC
#undef PM05_FOR_ROUND
}

/**
 * @brief  ピクセル値の変換時に mdarray の標準変換で良いかどうかを判定 (内部用)
 *
 * @note    private な関数です．
 */
static bool simple_conv_is_ok( const fits_image &src, 
			       int dst_type, long dst_bytes, 
			       double dst_bzero, double dst_bscale,
			       bool dst_blank_is_set, long long dst_blank )
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
	 src.bzero()  == dst_bzero && 
	 src.bscale() == dst_bscale ) {
	do_simple_conv = true;
    }
    /* 両方とも整数の場合 */
    else if ( src_is_float_type == false && 
	      dst_is_float_type == false &&
	      src.bytes() <= dst_bytes &&
	      src.bzero()        == dst_bzero &&
	      src.bscale()       == dst_bscale &&
	      ((src.blank_is_set() == false && dst_blank_is_set == false) ||
	       (src.blank_is_set() == true && dst_blank_is_set == true &&
		src.blank() == dst_blank)) ) {
	do_simple_conv = true;
    }
    /* 整数から float に変換される場合 */
    else if ( src_is_float_type == false && 
	      dst_is_float_type == true &&
	      src.bzero()        == dst_bzero &&
	      src.bscale()       == dst_bscale &&
	      src.blank_is_set() == false ) {
	do_simple_conv = true;
    }

    return do_simple_conv;
}

/**
 * @brief  指定された型，ZERO値，SCALE値，BLANK値を持つ画像データへ変換
 *
 * @note   このメンバ関数は private です．
 */
fits_image &fits_image::convert_image_type( int new_type, 
					    const double *new_zero_ptr, 
					    const double *new_scale_ptr,
					    const long long *new_blank_ptr )
{
    struct fits_image_cnv_prms prms;
    void (*func_cnv_ptr)(const void *,void *,size_t,int,void *);
    ssize_t org_size_type = this->data_array().size_type();
    ssize_t new_size_type = org_size_type;
    bool rounding_bak;
    mdarray tmp_data_rec;

    tmp_data_rec.init_properties(this->data_array());

    prms.old_blank = 0;
    prms.old_blank_is_set = false;
    prms.new_zero = 0.0;
    prms.new_scale = 1.0;
    prms.new_blank = 0;
    prms.new_min = 0.0;
    prms.new_max = 0.0;

    prms.old_zero = this->bzero();
    if ( new_zero_ptr != NULL ) prms.new_zero = *new_zero_ptr;

    prms.old_scale = this->bscale();
    if ( new_scale_ptr != NULL ) prms.new_scale = *new_scale_ptr;
    if ( prms.new_scale == 0 ) {
	err_report(__FUNCTION__,"WARNING",
		   "ZERO BSCALE cannot be set, 1.0 is used.");
	prms.new_scale = 1.0;
    }

    if ( this->type() == FITS::DOUBLE_T ||
	 this->type() == FITS::FLOAT_T ) {
    }
    else {
	prms.old_blank = this->blank();
	prms.old_blank_is_set = this->blank_is_set();
    }
    if ( new_type == FITS::DOUBLE_T ||
	 new_type == FITS::FLOAT_T ) {
	new_blank_ptr = NULL;
    }
    else {
	if ( new_blank_ptr != NULL ) {
	    prms.new_blank = get_blank_for_type(new_type, *(new_blank_ptr));
	}
	else {
	    prms.new_blank = get_indef_for_type(new_type);
	}
	prms.new_min = get_min_for_type(new_type);
	prms.new_max = get_max_for_type(new_type);
    }

    new_size_type = image_type_to_size_type( new_type );
    if ( new_size_type == 0 ) {
	err_report1(__FUNCTION__,"WARNING",
		    "unsuppoeted type %d; unchanged",new_type);
	new_type = this->type_rec;
	new_size_type = this->data_array().size_type();
    }

    rounding_bak = this->data_array().rounding();

    /* データの内容を tmp_data_rec に移動 */
    this->data_array().mdarray::cut(&tmp_data_rec);

    tmp_data_rec.set_rounding(true);

    try {

	if ( simple_conv_is_ok(*this, 
			   new_type, abs(new_size_type),
			   prms.new_zero, prms.new_scale,
			   (new_blank_ptr != NULL), prms.new_blank) == true ) {

	    tmp_data_rec.convert( new_size_type );

	}
	else {

	    func_cnv_ptr = NULL;

    /* 型変換のための関数を選択 */
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
	if ( org_size_type == org_sz_type && new_size_type == new_sz_type ) { \
	    func_cnv_ptr = &cnv_nd::fncname; \
	}
	SLI__MDARRAY__DO_OPERATION_2TYPES(SEL_FUNC,,,,,,,,,,,,,else);
#undef SEL_FUNC

	    if ( func_cnv_ptr == NULL ) {
		err_throw(__FUNCTION__,"FATAL",
			  "detected unexpected NULL pointer");
	    }

	    tmp_data_rec.convert_via_udf( new_size_type, 
					  func_cnv_ptr, (void *)&prms );

	}

    }
    catch (...) {
	this->data_array().set_rounding(rounding_bak);
	err_throw(__FUNCTION__,"FATAL","caught exception");
    }

    /* tmp_data_rec から this->data_rec へデータを戻す */
    this->_init_all_data_recs(tmp_data_rec.size_type());
    this->type_rec = new_type;

    tmp_data_rec.cut(this->data_rec);

    this->data_array().set_rounding(rounding_bak);

    if ( new_zero_ptr != NULL ) {
	this->assign_bzero(prms.new_zero);
    }
    else {
	this->erase_bzero();
    }
    if ( new_scale_ptr != NULL ) {
	this->assign_bscale(prms.new_scale);
    }
    else {
	this->erase_bscale();
    }
    if ( new_blank_ptr != NULL ) {
	this->assign_blank(*new_blank_ptr);
    }
    else {
	this->erase_blank();
    }

    return *this;
}

/* */

/**
 * @brief  高速 image fill・paste 用の準備
 *
 *  src_img が NULL の場合，スカラーの型を src_size_type で指定する．
 *
 * @note   このメンバ関数は private です．
 */
void fits_image::prepare_image_fill_paste_fast( const fits_image *src_img,
	       ssize_t src_size_type,
	       void *prms_p,
	       void (**func_cnv_ptr_p)(const void *,void *,size_t,int,void *),
	       bool *simple_cnv_ok_p ) const
{
    struct fits_image_cnv_prms &prms = *((struct fits_image_cnv_prms *)prms_p);

    ssize_t org_size_type = src_size_type;
    ssize_t new_size_type = this->data_array().size_type();
    bool old_is_float_type;
    bool new_is_float_type;

    prms.old_blank = 0;
    prms.old_blank_is_set = false;
    prms.new_zero = 0.0;
    prms.new_scale = 1.0;
    prms.new_blank = 0;
    prms.new_min = 0.0;
    prms.new_max = 0.0;

    if ( src_img == NULL ) prms.old_zero = 0.0;
    else prms.old_zero = src_img->bzero();
    prms.new_zero = this->bzero();

    if ( src_img == NULL ) prms.old_scale = 1.0;
    else prms.old_scale = src_img->bscale();
    prms.new_scale = this->bscale();
    if ( prms.new_scale == 0 ) {
	err_report(__FUNCTION__,"WARNING",
		   "ZERO BSCALE cannot be set, 1.0 is used.");
	prms.new_scale = 1.0;
    }

    if ( org_size_type < 0 ) {
	old_is_float_type = true;
    }
    else {
	old_is_float_type = false;
	if ( src_img == NULL ) {
	    prms.old_blank = 0;
	    prms.old_blank_is_set = false;
	}
	else {
	    prms.old_blank = src_img->blank();
	    prms.old_blank_is_set = src_img->blank_is_set();
	}
    }
    if ( this->data_array().size_type() < 0 ) {
	new_is_float_type = true;
    }
    else {
	new_is_float_type = false;
	if ( this->blank_is_set() ) {
	    prms.new_blank = get_blank_for_type(this->type(), this->blank());
	}
	else {
	    prms.new_blank = get_indef_for_type(this->type());
	}
	prms.new_min = get_min_for_type(this->type());
	prms.new_max = get_max_for_type(this->type());
    }

    if ( src_img != NULL && 
	 simple_conv_is_ok(*src_img, 
		    this->type(), this->bytes(), this->bzero(), this->bscale(),
		    this->blank_is_set(), this->blank()) == true ) {
	if ( simple_cnv_ok_p != NULL ) *simple_cnv_ok_p = true;
	*func_cnv_ptr_p = NULL;
    }
    else {

	if ( simple_cnv_ok_p != NULL ) *simple_cnv_ok_p = false;

	*func_cnv_ptr_p = NULL;

    /* 型変換のための関数を選択 */
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
	if ( org_size_type == org_sz_type && new_size_type == new_sz_type ) { \
	    *func_cnv_ptr_p = &cnv_nd::fncname; \
	}
	SLI__MDARRAY__DO_OPERATION_2TYPES(SEL_FUNC,,,,,,,,,,,,,else);
#undef SEL_FUNC

	if ( *func_cnv_ptr_p == NULL ) {
	    err_throw(__FUNCTION__,"FATAL","detected unexpected NULL pointer");
	}

    }

    return;
}

/*
 * BEGIN : image_fill() 演算なし版 (高速)
 */

/**
 * @brief  image_fill() 演算なし版 (高速・IDLの記法)
 *
 * @note   このメンバ関数は private です．
 */
fits_image &fits_image::image_vfillf_fast( double value,
					   const char *exp_fmt, va_list ap )
{
    struct fits_image_cnv_prms prms;
    void (*func_cnv_ptr)(const void *,void *,size_t,int,void *);
    bool rounding_bak;

    this->prepare_image_fill_paste_fast( NULL, DOUBLE_ZT,
					 &prms, &func_cnv_ptr, NULL);


    rounding_bak = this->data_array().rounding();
    this->data_array().set_rounding(true);

    this->data_array().vfillf_via_udf( value, func_cnv_ptr, (void *)&prms,
				    exp_fmt, ap );

    this->data_array().set_rounding(rounding_bak);

    return *this;
}

/**
 * @brief  image_fill() 演算なし版 (高速)
 *
 * @note   このメンバ関数は private です．
 */
fits_image &fits_image::image_fill_fast( double value, 
					 long col_index, long col_size, 
					 long row_index, long row_size,
					 long layer_index, long layer_size )
{
    struct fits_image_cnv_prms prms;
    void (*func_cnv_ptr)(const void *,void *,size_t,int,void *);
    bool rounding_bak;

    this->prepare_image_fill_paste_fast( NULL, DOUBLE_ZT,
					 &prms, &func_cnv_ptr, NULL);

    rounding_bak = this->data_array().rounding();
    this->data_array().set_rounding(true);

    this->data_array().fill_via_udf( value, func_cnv_ptr, (void *)&prms,
				     col_index, col_size, row_index, row_size,
				     layer_index, layer_size );

    this->data_array().set_rounding(rounding_bak);

    return *this;
}

/*
 * BEGIN : image_fill() 演算あり版
 */

struct fits_image_fill_paste_prms {
    /* アライメントに注意(sizeofが長いものから順に!) */
    double x_min;
    double x_max;
    double bzero;
    double bscale;
    long long blank_r;
    long long blank_w;
    void (*usr_func_fill)(double [],double,long, long,long,long,fits_image *,void *);
    void (*usr_func_paste)(double [],double [],long, long,long,long,fits_image *,void *);
    fits_image *this_ptr;
    void *usr_ptr;
    bool blank_is_set;
};

namespace fill_paste_nd_x2d
{
#define MAKE_FUNC(fncname,org_sz_type,org_type,fnc_d2x) \
static void fncname(const void *org_val_ptr, void *new_val_ptr, size_t n, int f_b, void *u_ptr) \
{ \
    const struct fits_image_fill_paste_prms *p = (const struct fits_image_fill_paste_prms *)u_ptr; \
    const org_type *org_t_ptr = (const org_type *)org_val_ptr; \
    double *new_t_ptr = (double *)new_val_ptr; \
    double val; \
    size_t i; \
    if ( p->bzero == 0.0 && p->bscale == 1.0 ) { \
	if ( p->blank_is_set == true ) { \
	  for ( i=0 ; i < n ; i++ ) { \
	    if ( p->blank_r == org_t_ptr[i] ) { \
	      val = NAN; \
	    } \
	    else { \
	      val = org_t_ptr[i]; \
	    } \
	    new_t_ptr[i] = val; \
	  } \
	} \
	else { \
	  for ( i=0 ; i < n ; i++ ) { \
	    val = org_t_ptr[i]; \
	    new_t_ptr[i] = val; \
	  } \
	} \
    } \
    else { \
	const double p_bscale = p->bscale; \
	const double p_bzero = p->bzero; \
	if ( p->blank_is_set == true ) { \
	  for ( i=0 ; i < n ; i++ ) { \
	    if ( p->blank_r == org_t_ptr[i] ) { \
	      val = NAN; \
	    } \
	    else { \
	      val = org_t_ptr[i]; \
	      val *= p_bscale; \
	      val += p_bzero; \
	    } \
	    new_t_ptr[i] = val; \
	  } \
	} \
	else { \
	  for ( i=0 ; i < n ; i++ ) { \
	    val = org_t_ptr[i]; \
	    val *= p_bscale; \
	    val += p_bzero; \
	    new_t_ptr[i] = val; \
	  } \
	} \
    } \
}
SLI__MDARRAY__DO_OPERATION_DOUBLE2ANY(MAKE_FUNC,,,,,);
#undef MAKE_FUNC
}
namespace fill_paste_nd_d2x
{
#define PM05_FOR_ROUND(dv) (((dv) < 0) ? (dv)-0.5 : (dv)+0.5)
#define MAKE_FUNC(fncname,new_sz_type,new_type,fnc_d2x) \
static void fncname(const void *org_val_ptr, void *new_val_ptr, size_t n, int f_b, void *u_ptr) \
{ \
    const struct fits_image_fill_paste_prms *p = (const struct fits_image_fill_paste_prms *)u_ptr; \
    const double *org_t_ptr = (const double *)org_val_ptr; \
    new_type *new_t_ptr = (new_type *)new_val_ptr; \
    double val; \
    size_t i; \
    if ( p->bzero == 0.0 && p->bscale == 1.0 ) { \
	if ( new_sz_type < 0 /* float type */ ) { \
	  for ( i=0 ; i < n ; i++ ) { \
	    val = org_t_ptr[i]; \
	    new_t_ptr[i] = (new_type)val; \
	  } \
	} \
	else { /* not float type */ \
	  for ( i=0 ; i < n ; i++ ) { \
	    val = org_t_ptr[i]; \
	    if ( isfinite(val) && p->x_min <= val && val <= p->x_max ) \
	      new_t_ptr[i] = (new_type)fnc_d2x(val); \
	    else new_t_ptr[i] = (new_type)(p->blank_w); \
	  } \
	} \
    } \
    else { \
	const double p_bscale = p->bscale; \
	const double p_bzero = p->bzero; \
	if ( new_sz_type < 0 /* float type */ ) { \
	  for ( i=0 ; i < n ; i++ ) { \
	    val = org_t_ptr[i]; \
	    val -= p_bzero; \
	    val /= p_bscale; \
	    new_t_ptr[i] = (new_type)val; \
	  } \
	} \
	else { /* not float type */ \
	  for ( i=0 ; i < n ; i++ ) { \
	    val = org_t_ptr[i]; \
	    val -= p_bzero; \
	    val /= p_bscale; \
	    if ( isfinite(val) && p->x_min <= val && val <= p->x_max ) \
	      new_t_ptr[i] = (new_type)fnc_d2x(val); \
	    else new_t_ptr[i] = (new_type)(p->blank_w); \
	  } \
	} \
    } \
}
SLI__MDARRAY__DO_OPERATION_DOUBLE2ANY(MAKE_FUNC,PM05_FOR_ROUND,PM05_FOR_ROUND,PM05_FOR_ROUND,PM05_FOR_ROUND,);
#undef MAKE_FUNC
#undef PM05_FOR_ROUND
}
namespace fill_x
{
static void u_func(double vals[],double sval,size_t n, ssize_t i, ssize_t j, ssize_t k,
		   mdarray *x ,void *u_ptr )
{
    struct fits_image_fill_paste_prms *p = (struct fits_image_fill_paste_prms *)u_ptr;
    p->usr_func_fill(vals,sval,n, i,j,k, p->this_ptr, p->usr_ptr);
    return;
}
}

/**
 * @brief  image_fill() 演算あり版の準備
 */
void fits_image::prepare_image_fill( void *prms_p,
	     void (**func_dst2d_p)(const void *,void *,size_t,int,void *),
	     void (**func_d2dst_p)(const void *,void *,size_t,int,void *) ) const
{
    struct fits_image_fill_paste_prms &prms = *((struct fits_image_fill_paste_prms *)prms_p);

    prms.blank_is_set = false;
    prms.blank_r = 0;
    prms.blank_w = 0;
    prms.bzero = this->bzero();
    prms.bscale = this->bscale();
    prms.x_min = 0.0;
    prms.x_max = 0.0;

    if ( this->type_rec != FITS::DOUBLE_T &&
	 this->type_rec != FITS::FLOAT_T ) {
	prms.blank_r = this->blank();
	prms.blank_w = this->blank_w_rec;
	prms.blank_is_set = this->blank_is_set();
	prms.x_min = get_min_for_type(this->type_rec);
	prms.x_max = get_max_for_type(this->type_rec);
    }

    *func_dst2d_p = NULL;
    *func_d2dst_p = NULL;

#define SEL_FUNC(fncname,sztp,tp,fnc_d2x) \
    if ( this->data_array().size_type() == sztp ) { \
	*func_dst2d_p = &fill_paste_nd_x2d::fncname; \
	*func_d2dst_p = &fill_paste_nd_d2x::fncname; \
    }
    SLI__MDARRAY__DO_OPERATION_DOUBLE2ANY(SEL_FUNC,,,,,else);
#undef SEL_FUNC

    if ( *func_dst2d_p == NULL || *func_d2dst_p == NULL ) {
	err_throw(__FUNCTION__,"FATAL","detected unexpected NULL pointer");
    }

    return;
}

/**
 * @brief  image_fill() 演算あり版 (IDLの記法)
 *
 * @note   このメンバ関数は private です．
 */
fits_image &fits_image::image_vfillf( double value, 
       void (*func)(double [],double,long, long,long,long,fits_image *,void *),
       void *user_ptr, const char *exp_fmt, va_list ap )
{
    struct fits_image_fill_paste_prms prms;
    void (*func_dst2d)(const void *,void *,size_t,int,void *);
    void (*func_d2dst)(const void *,void *,size_t,int,void *);
    bool rounding_bak;

    this->prepare_image_fill(&prms, &func_dst2d, &func_d2dst);
    
    prms.usr_func_fill = func;
    prms.this_ptr = this;
    prms.usr_ptr  = user_ptr;

    rounding_bak = this->data_array().rounding();
    this->data_array().set_rounding(true);

    this->data_array().vfillf_via_udf( value,
			func_dst2d, (void *)&prms, func_d2dst, (void *)&prms,
			&fill_x::u_func, (void *)&prms,       /* prms は共用 */
			exp_fmt, ap );

    this->data_array().set_rounding(rounding_bak);

    return *this;
}

/**
 * @brief  image_fill() 演算あり版
 *
 * @note   このメンバ関数は private です．
 */
fits_image &fits_image::image_fill( double value,
       void (*func)(double [],double,long, long,long,long,fits_image *,void *),
	      void *user_ptr,
	      long col_index, long col_size, long row_index, long row_size,
	      long layer_index, long layer_size )
{
    struct fits_image_fill_paste_prms prms;
    void (*func_dst2d)(const void *,void *,size_t,int,void *);
    void (*func_d2dst)(const void *,void *,size_t,int,void *);
    size_t z_col_size = fits_seclen_to_mdarray_seclen(col_size);
    size_t z_row_size = fits_seclen_to_mdarray_seclen(row_size);
    size_t z_layer_size = fits_seclen_to_mdarray_seclen(layer_size);
    bool rounding_bak;

    this->prepare_image_fill(&prms, &func_dst2d, &func_d2dst);
    
    prms.usr_func_fill = func;
    prms.this_ptr = this;
    prms.usr_ptr  = user_ptr;

    rounding_bak = this->data_array().rounding();
    this->data_array().set_rounding(true);

    this->data_array().fill_via_udf( value,
			func_dst2d, (void *)&prms, func_d2dst, (void *)&prms,
			&fill_x::u_func, (void *)&prms,       /* prms は共用 */
			col_index, z_col_size, row_index, z_row_size,
			layer_index, z_layer_size );

    this->data_array().set_rounding(rounding_bak);

    return *this;
}

/*
 * BEGIN : image_paste() 演算なし版 (高速)
 */

/**
 * @brief  image_paste() 演算なし版 (高速・IDLの記法)
 *
 * @note   このメンバ関数は private です．
 */
fits_image &fits_image::image_vpastef_fast( const fits_image &src_img,
					    const char *exp_fmt, va_list ap )
{
    struct fits_image_cnv_prms prms;
    void (*func_cnv_ptr)(const void *,void *,size_t,int,void *);
    bool simple_cnv_ok;
    bool rounding_bak;

    this->prepare_image_fill_paste_fast(&src_img, 
					src_img.data_array().size_type(),
					&prms, &func_cnv_ptr, &simple_cnv_ok);

    rounding_bak = this->data_array().rounding();
    this->data_array().set_rounding(true);

    if ( simple_cnv_ok == true ) {
	this->data_array().vpastef( src_img.data_array(), exp_fmt, ap );
    }
    else {
	this->data_array().vpastef_via_udf( src_img.data_array(), 
					    func_cnv_ptr, (void *)&prms,
					    exp_fmt, ap );
    }

    this->data_array().set_rounding(rounding_bak);

    return *this;
}

/**
 * @brief  image_paste() 演算なし版 (高速)
 *
 * @note   このメンバ関数は private です．
 */
fits_image &fits_image::image_paste_fast( const fits_image &src_img,
	      long dest_col, long dest_row, long dest_layer )
{
    struct fits_image_cnv_prms prms;
    void (*func_cnv_ptr)(const void *,void *,size_t,int,void *);
    bool simple_cnv_ok;
    bool rounding_bak;

    this->prepare_image_fill_paste_fast(&src_img, 
					src_img.data_array().size_type(),
					&prms, &func_cnv_ptr, &simple_cnv_ok);

    rounding_bak = this->data_array().rounding();
    this->data_array().set_rounding(true);

    if ( simple_cnv_ok == true ) {
	this->data_array().paste( src_img.data_array(),
				  dest_col, dest_row, dest_layer );
    }
    else {
	this->data_array().paste_via_udf( src_img.data_array(), 
					  func_cnv_ptr, (void *)&prms,
					  dest_col, dest_row, dest_layer );
    }

    this->data_array().set_rounding(rounding_bak);

    return *this;
}

/*
 * BEGIN : image_paste() 演算あり版
 */

namespace paste_x 
{
static void u_func(double vals[], double svals[], size_t n, 
		   ssize_t i, ssize_t j, ssize_t k,
		   mdarray *x ,void *u_ptr )
{
    struct fits_image_fill_paste_prms *p = (struct fits_image_fill_paste_prms *)u_ptr;
    return p->usr_func_paste(vals,svals,n, i,j,k, p->this_ptr, p->usr_ptr);
}
}

/**
 * @brief  image_paste() の準備
 */
void fits_image::prepare_image_paste( const fits_image &src_img,
	     void *p_src_p,  void *p_dst_p,
	     void (**func_src2d_p)(const void *,void *,size_t,int,void *),
	     void (**func_dst2d_p)(const void *,void *,size_t,int,void *),
	     void (**func_d2dst_p)(const void *,void *,size_t,int,void *) ) const
{
    struct fits_image_fill_paste_prms &p_src = *((struct fits_image_fill_paste_prms *)p_src_p);
    struct fits_image_fill_paste_prms &p_dst = *((struct fits_image_fill_paste_prms *)p_dst_p);

    p_src.blank_is_set = false;
    p_src.blank_r = 0;
    p_src.blank_w = 0;
    p_src.x_min = 0.0;
    p_src.x_max = 0.0;
    p_dst.blank_is_set = false;
    p_dst.blank_r = 0;
    p_dst.blank_w = 0;
    p_dst.x_min = 0.0;
    p_dst.x_max = 0.0;

    p_src.bzero = src_img.bzero();
    p_src.bscale = src_img.bscale();
    p_dst.bzero = this->bzero();
    p_dst.bscale = this->bscale();

    if ( src_img.type() != FITS::DOUBLE_T &&
	 src_img.type() != FITS::FLOAT_T ) {
	p_src.blank_r = src_img.blank();
	p_src.blank_w = src_img.blank_w_rec;
	p_src.blank_is_set = src_img.blank_is_set();
	p_src.x_min = get_min_for_type(src_img.type());
	p_src.x_max = get_max_for_type(src_img.type());
    }
    if ( this->type_rec != FITS::DOUBLE_T &&
	 this->type_rec != FITS::FLOAT_T ) {
	p_dst.blank_r = this->blank();
	p_dst.blank_w = this->blank_w_rec;
	p_dst.blank_is_set = this->blank_is_set();
	p_dst.x_min = get_min_for_type(this->type_rec);
	p_dst.x_max = get_max_for_type(this->type_rec);
    }

    *func_src2d_p = NULL;
    *func_dst2d_p = NULL;
    *func_d2dst_p = NULL;

    /* 型変換のための関数を選択 */
#define SEL_FUNC(fncname,sztp,tp,fnc_d2x) \
    if ( src_img.data_array().size_type() == sztp ) { \
	*func_src2d_p = &fill_paste_nd_x2d::fncname; \
    }
    SLI__MDARRAY__DO_OPERATION_DOUBLE2ANY(SEL_FUNC,,,,,else);
#undef SEL_FUNC
#define SEL_FUNC(fncname,sztp,tp,fnc_d2x) \
    if ( this->data_array().size_type() == sztp ) { \
	*func_dst2d_p = &fill_paste_nd_x2d::fncname; \
	*func_d2dst_p = &fill_paste_nd_d2x::fncname; \
    }
    SLI__MDARRAY__DO_OPERATION_DOUBLE2ANY(SEL_FUNC,,,,,else);
#undef SEL_FUNC

    if ( *func_src2d_p == NULL || *func_dst2d_p == NULL || 
	 *func_d2dst_p == NULL ) {
	err_throw(__FUNCTION__,"FATAL","detected unexpected NULL pointer");
    }

    return;
}

/**
 * @brief  image_paste() 演算あり版 (IDLの記法)
 *
 * @note   このメンバ関数は private です．
 */
fits_image &fits_image::image_vpastef( const fits_image &src_img,
    void (*func)(double [],double [],long, long,long,long,fits_image *,void *),
			      void *user_ptr, const char *exp_fmt, va_list ap )
{
    struct fits_image_fill_paste_prms p_src;
    struct fits_image_fill_paste_prms p_dst;
    void (*func_src2d)(const void *,void *,size_t,int,void *);
    void (*func_dst2d)(const void *,void *,size_t,int,void *);
    void (*func_d2dst)(const void *,void *,size_t,int,void *);
    bool rounding_bak;

    if ( src_img.length() == 0 ) goto quit;

    this->prepare_image_paste( src_img, &p_src, &p_dst, 
			       &func_src2d, &func_dst2d, &func_d2dst );

    p_src.usr_func_paste = NULL;
    p_src.this_ptr = NULL;
    p_src.usr_ptr  = NULL;
    p_dst.usr_func_paste = func;
    p_dst.this_ptr = this;
    p_dst.usr_ptr  = user_ptr;

    rounding_bak = this->data_array().rounding();
    this->data_array().set_rounding(true);

    this->data_array().vpastef_via_udf( src_img.data_array(),
			func_src2d, (void *)&p_src,
			func_dst2d, (void *)&p_dst,
			func_d2dst, (void *)&p_dst,
			&paste_x::u_func, (void *)&p_dst,    /* p_dst は共用 */
			exp_fmt, ap );

    this->data_array().set_rounding(rounding_bak);

 quit:
    return *this;
}

/**
 * @brief  image_paste() 演算あり版
 *
 * @note   このメンバ関数は private です．
 */
fits_image &fits_image::image_paste( const fits_image &src_img,
    void (*func)(double [],double [],long, long,long,long,fits_image *,void *),
	      void *user_ptr,
	      long dest_col, long dest_row, long dest_layer )
{
    struct fits_image_fill_paste_prms p_src;
    struct fits_image_fill_paste_prms p_dst;
    void (*func_src2d)(const void *,void *,size_t,int,void *);
    void (*func_dst2d)(const void *,void *,size_t,int,void *);
    void (*func_d2dst)(const void *,void *,size_t,int,void *);
    bool rounding_bak;

    if ( src_img.length() == 0 ) goto quit;

    this->prepare_image_paste( src_img, &p_src, &p_dst, 
			       &func_src2d, &func_dst2d, &func_d2dst );

    p_src.usr_func_paste = NULL;
    p_src.this_ptr = NULL;
    p_src.usr_ptr  = NULL;
    p_dst.usr_func_paste = func;
    p_dst.this_ptr = this;
    p_dst.usr_ptr  = user_ptr;

    rounding_bak = this->data_array().rounding();
    this->data_array().set_rounding(true);

    this->data_array().paste_via_udf( src_img.data_array(),
			func_src2d, (void *)&p_src,
			func_dst2d, (void *)&p_dst,
			func_d2dst, (void *)&p_dst,
			&paste_x::u_func, (void *)&p_dst,    /* p_dst は共用 */
			dest_col, dest_row, dest_layer );

    this->data_array().set_rounding(rounding_bak);

 quit:
    return *this;
}


/**
 * @brief  inlineメンバ関数でthrowする時に使用
 *
 * @note   このメンバ関数は private です．
 */
void *fits_image::err_throw_void_p( const char *fnc, 
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
const void *fits_image::err_throw_const_void_p( const char *fnc, 
					 const char *lv, const char *mes) const
{
    err_throw(fnc,lv,mes);
    return (const void *)NULL;
}


#include "private/write_stream_or_get_csum.cc"

}	/* namespace sli */

#include "private/c_memset.cc"
