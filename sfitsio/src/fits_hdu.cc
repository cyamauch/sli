/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2015-11-20 14:31:27 cyamauch> */

/**
 * @file   fits_hdu.cc
 * @brief  FITS の HDU を表現する基底クラス fits_hdu のコード
 */

#define CLASS_NAME "fits_hdu"

#include "config.h"

#include "fits_hdu.h"
#include "fitscc.h"

#include <sli/mdarray.h>

#include "private/err_report.h"


namespace sli
{

#include "private/initialize_csum.h"

/**
 * @brief  コンストラクタ
 */
fits_hdu::fits_hdu()
{
    this->hdutype_rec = FITS::ANY_HDU;
    this->classlevel_rec = 0;
    this->extver_rec = NULL;
    this->extlevel_rec = NULL;
    this->manager = NULL;
    this->header_rec.set_sysrecords_prohibition(true);
    this->header_rec.register_manager(this);
    //this->is_primary_rec = false;
    this->checksum_error_rec = false;
    this->datasum_error_rec = false;
    return;
}

/**
 * @brief  コピーコンストラクタ
 */
fits_hdu::fits_hdu(const fits_hdu &obj)
{
    this->hdutype_rec = FITS::ANY_HDU;
    this->classlevel_rec = 0;
    this->extver_rec = NULL;
    this->extlevel_rec = NULL;
    this->manager = NULL;
    this->header_rec.set_sysrecords_prohibition(true);
    this->header_rec.register_manager(this);
    //this->is_primary_rec = false;
    this->checksum_error_rec = false;
    this->datasum_error_rec = false;
    this->init(obj);
    return;
}

/**
 * @brief  デストラクタ
 */
fits_hdu::~fits_hdu()
{
    return;
}

/**
 * @brief  オブジェクトのコピー
 */
fits_hdu &fits_hdu::operator=(const fits_hdu &obj)
{
    return this->init(obj);
}

/**
 * @brief  オブジェクトの初期化
 */
fits_hdu &fits_hdu::init()
{
    //err_report("init()","DEBUG","fits_hdu::init() ...");
    //this->hdutype_rec = FITS::ANY_HDU;
    //this->is_primary_rec = false;

    if ( this->manager == NULL ) {
	this->extname_rec = NULL;
	this->extver_rec = NULL;
	this->extlevel_rec = NULL;
    }

    /* ヘッダを全部消去する */
    this->header_rec.init();

    this->checksum_error_rec = false;
    this->datasum_error_rec = false;

    return *this;
}

/**
 * @brief  オブジェクトのコピー
 */
fits_hdu &fits_hdu::init(const fits_hdu &obj)
{
    if ( &obj == this ) return *this;

    //err_report("init()","DEBUG","fits_hdu::init(const fits_hdu &obj) ...");
    this->fits_hdu::init();

    try {
	//this->hdutype_rec = obj.hdutype_rec;	/* ←継承クラスでやる */
	this->header_rec.set_sysrecords_prohibition(false);
	this->header_rec = obj.header_rec;
	this->header_rec.set_sysrecords_prohibition(true);

	if ( this->manager == NULL ) {
	    this->extname_rec = obj.extname_rec;
	}
	else {
	    long i;
	    for ( i=0 ; i < this->manager->length() ; i++ ) {
		if ( &(this->manager->hdu(i)) == this ) {
		    this->manager->assign_extname(i, obj.extname_rec.cstr());
		    break;
		}
	    }
	    if ( i == this->manager->length() ) {
		err_throw(__FUNCTION__,"FATAL",
			  "internal error; fitscc does not manage me...");
	    }
	}
	this->extver_rec = obj.extver_rec;
	this->extlevel_rec = obj.extlevel_rec;

	this->checksum_error_rec = obj.checksum_error_rec;
	this->datasum_error_rec = obj.checksum_error_rec;

	//this->is_primary_rec = obj.is_primary_rec;
    }
    catch (...) {
	this->fits_hdu::init();
	this->header_rec.set_sysrecords_prohibition(true);
	err_throw(__FUNCTION__,"FATAL","'=' failed");
    }

    return *this;
}

/**
 * @brief  2つのオブジェクト間での内容のスワップ
 *
 *  指定されたオブジェクトsobj の内容と自身の内容を入れ替えます．
 *  ヘッダの内容，属性等すべての状態が入れ替わります．
 *
 * @param   sobj fits_hdu クラスのオブジェクト
 * @return  自身の参照    
 * @note    このメンバ関数は protected です．
 */
fits_hdu &fits_hdu::swap( fits_hdu &obj )
{
    //err_report(__FUNCTION__,"DEBUG","called!!");
    bool tmp_checksum_error_rec;
    bool tmp_datasum_error_rec;
    tstring extname1 = this->extname_rec;
    tstring extname2 = obj.extname_rec;

    if ( &obj == this ) return *this;

    this->header_rec.set_sysrecords_prohibition(false);
    obj.header_rec.set_sysrecords_prohibition(false);
    this->header_rec.swap(obj.header_rec);
    this->header_rec.set_sysrecords_prohibition(true);
    obj.header_rec.set_sysrecords_prohibition(true);

    this->extver_rec.swap(obj.extver_rec);

    this->extlevel_rec.swap(obj.extlevel_rec);

    tmp_checksum_error_rec = obj.checksum_error_rec;
    obj.checksum_error_rec = this->checksum_error_rec;
    this->checksum_error_rec = tmp_checksum_error_rec;

    tmp_datasum_error_rec = obj.datasum_error_rec;
    obj.datasum_error_rec = this->datasum_error_rec;
    this->datasum_error_rec = tmp_datasum_error_rec;

    if ( this->manager == NULL ) {
	this->extname_rec = extname2;
    }
    else {
	long i;
	for ( i=0 ; i < this->manager->length() ; i++ ) {
	    if ( &(this->manager->hdu(i)) == this ) {
		this->manager->assign_extname(i, extname2.cstr());
		break;
	    }
	}
	if ( i == this->manager->length() ) {
	    err_throw(__FUNCTION__,"FATAL",
		      "internal error; fitscc does not manage me...");
	}
    }
    if ( obj.manager == NULL ) {
	obj.extname_rec = extname1;
    }
    else {
	long i;
	for ( i=0 ; i < obj.manager->length() ; i++ ) {
	    if ( &(obj.manager->hdu(i)) == &obj ) {
		obj.manager->assign_extname(i, extname1.cstr());
		break;
	    }
	}
	if ( i == obj.manager->length() ) {
	    err_throw(__FUNCTION__,"FATAL",
		      "internal error; fitscc does not manage me...");
	}
    }

    return *this;
}

/**
 * @brief  HDU の種別(FITS::IMAGE_HDU，FITS::BINARY_TABLE_HDU 等)を取得
 */
int fits_hdu::hdutype() const
{
    return this->hdutype_rec;
}

/**
 * @brief  クラスの継承レベルを取得
 */
int fits_hdu::classlevel() const
{
    return this->classlevel_rec;
}

/*
 * ユーザのヘッダまわり 
 */

/* 全体の編集 */

/**
 * @brief  FITSヘッダの初期化
 */
fits_hdu &fits_hdu::header_init()
{
    this->header_rec.init();
    return *this;
}

/**
 * @brief  FITSヘッダのコピー (外部 fits_hedaer オブジェクトをコピー)
 *
 * @note   Data Unit に関係する予約キーワード(BITPIX 等)を持つレコードは
 *         コピーされません．
 */
fits_hdu &fits_hdu::header_init( const fits_header &obj )
{
    this->header_rec.init(obj);
    return *this;
}

/**
 * @brief  FITSヘッダの初期化 (fits::header_def 構造体で指定)
 *
 * @note   Data Unit に関係する予約キーワード(BITPIX 等)は指定できません．
 */
fits_hdu &fits_hdu::header_init( const fits::header_def defs[] )
{
    this->header_rec.init(defs);
    return *this;
}

/**
 * @brief  fits_headerオブジェクト間での内容のスワップ
 *
 *  予約キーワード以外について，ヘッダの内容を入れ替えます．
 *
 * @note   Data Unit に関係する予約キーワード(BITPIX 等)を持つレコードは
 *         入れ替え処理が行なわれません．
 */
fits_hdu &fits_hdu::header_swap( fits_header &obj )
{
    this->header_rec.swap(obj);
    return *this;
}

/**
 * @brief  複数のヘッダレコードの追加，更新
 *
 *  キーワードが存在しない場合は追加し，同一のキーワードが存在する場合は
 *  指定された内容で上書きします．
 *
 * @param  obj 源泉値
 * @note   Data Unit に関係する予約キーワード(BITPIX 等)は指定できません．
 */
fits_hdu &fits_hdu::header_update_records( const fits_header &obj )
{
    this->header_rec.update_records( obj );
    return *this;
}

/**
 * @brief  複数のヘッダレコードの追加 (fits::header_def 構造体で指定)
 *
 * @note   Data Unit に関係する予約キーワード(BITPIX 等)は指定できません．
 */
fits_hdu &fits_hdu::header_append_records( const fits::header_def defs[] )
{
    this->header_rec.append_records(defs);
    return *this;
}

/**
 * @brief  複数のヘッダレコードの追加 (fits::header_def 構造体で指定)
 *
 * @note   Data Unit に関係する予約キーワード(BITPIX 等)は指定できません．
 */
fits_hdu &fits_hdu::header_append_records( const fits::header_def defs[],
					   long num_defs, bool warn )
{
    this->header_rec.append_records(defs, num_defs, warn);
    return *this;
}

/**
 * @brief  複数のヘッダレコードの追加 (fits_header オブジェクトで指定)
 *
 * @note   Data Unit に関係する予約キーワード(BITPIX 等)は指定できません．
 */
fits_hdu &fits_hdu::header_append_records( const fits_header &obj, bool warn )
{
    this->header_rec.append_records(obj, warn);
    return *this;
}

/**
 * @brief  複数のヘッダレコードの挿入 (fits::header_def 構造体で指定)
 *
 * @note   Data Unit に関係する予約キーワード(BITPIX 等)は指定できません．
 */
fits_hdu &fits_hdu::header_insert_records( long index0,
					   const fits::header_def defs[] )
{
    this->header_rec.insert_records(index0,defs);
    return *this;
}

/**
 * @brief  複数のヘッダレコードの挿入 (fits::header_def 構造体で指定)
 *
 * @note   Data Unit に関係する予約キーワード(BITPIX 等)は指定できません．
 */
fits_hdu &fits_hdu::header_insert_records( const char *keyword0,
					   const fits::header_def defs[] )
{
    this->header_rec.insert_records(keyword0,defs);
    return *this;
}

/**
 * @brief  複数のヘッダレコードの挿入 (fits::header_def 構造体で指定)
 *
 * @note   Data Unit に関係する予約キーワード(BITPIX 等)は指定できません．
 */
fits_hdu &fits_hdu::header_insert_records( long index0,
					   const fits::header_def defs[],
					   long num_defs, bool warn )
{
    this->header_rec.insert_records(index0,defs,num_defs,warn);
    return *this;
}

/**
 * @brief  複数のヘッダレコードの挿入 (fits::header_def 構造体で指定)
 *
 * @note   Data Unit に関係する予約キーワード(BITPIX 等)は指定できません．
 */
fits_hdu &fits_hdu::header_insert_records( const char *keyword0,
					   const fits::header_def defs[],
					   long num_defs, bool warn )
{
    this->header_rec.insert_records(keyword0,defs,num_defs,warn);
    return *this;
}

/**
 * @brief  複数のヘッダレコードの挿入 (fits_header オブジェクトで指定)
 *
 * @note   Data Unit に関係する予約キーワード(BITPIX 等)は指定できません．
 */
fits_hdu &fits_hdu::header_insert_records( long index0,
					   const fits_header &obj, bool warn )
{
    this->header_rec.insert_records(index0,obj,warn);
    return *this;
}

/**
 * @brief  複数のヘッダレコードの挿入 (fits_header オブジェクトで指定)
 *
 * @note   Data Unit に関係する予約キーワード(BITPIX 等)は指定できません．
 */
fits_hdu &fits_hdu::header_insert_records( const char *keyword0,
					   const fits_header &obj, bool warn )
{
    this->header_rec.insert_records(keyword0,obj,warn);
    return *this;
}

/**
 * @brief  複数のヘッダレコードの削除
 *
 * @note   Data Unit に関係する予約キーワード(BITPIX 等)は指定できません．
 */
fits_hdu &fits_hdu::header_erase_records( long index0, long num_records )
{
    this->header_rec.erase_records(index0,num_records);
    return *this;
}

/**
 * @brief  複数のヘッダレコードの削除
 *
 * @note   Data Unit に関係する予約キーワード(BITPIX 等)は指定できません．
 */
fits_hdu &fits_hdu::header_erase_records( const char *keyword0, long num_records )
{
    this->header_rec.erase_records(keyword0,num_records);
    return *this;
}

/* 1行での追加とか */

/**
 * @brief  1つのヘッダレコードを追加，更新
 *
 *  キーワードが存在しない場合は追加し，同一のキーワードが存在する場合は
 *  指定された内容で上書きします．
 *
 * @note   Data Unit に関係する予約キーワード(BITPIX 等)は指定できません．
 */
fits_hdu &fits_hdu::header_update( const char *keyword, const char *value, 
				   const char *comment )
{
    this->header_rec.update(keyword,value,comment);
    return *this;
}

/**
 * @brief  1つのヘッダレコードを追加,更新(fits_header_recordオブジェクトで指定)
 *
 *  キーワードが存在しない場合は追加し，同一のキーワードが存在する場合は
 *  指定された内容で上書きします．
 *
 * @note   Data Unit に関係する予約キーワード(BITPIX 等)は指定できません．
 */
fits_hdu &fits_hdu::header_update( const fits_header_record &obj )
{
    this->header_rec.update( obj );
    return *this;
}

/**
 * @brief  1つのヘッダレコードを追加 (キーワードのみ指定)
 *
 * @note   Data Unit に関係する予約キーワード(BITPIX 等)は指定できません．
 */
fits_hdu &fits_hdu::header_append( const char *keyword )
{
    this->header_rec.append(keyword);
    return *this;
}

/**
 * @brief  1つのヘッダレコードを追加
 *
 * @note   Data Unit に関係する予約キーワード(BITPIX 等)は指定できません．
 */
fits_hdu &fits_hdu::header_append( const char *keyword, const char *value, 
				   const char *comment )
{
    this->header_rec.append(keyword,value,comment);
    return *this;
}

/**
 * @brief  1つの記述式ヘッダレコード(HISTORYやCOMMENT)を追加
 */
fits_hdu &fits_hdu::header_append( const char *keyword, const char *description )
{
    this->header_rec.append(keyword,description);
    return *this;
}

/**
 * @brief  1つのヘッダレコードを追加 (fits::header_def 構造体で指定)
 *
 * @note   Data Unit に関係する予約キーワード(BITPIX 等)は指定できません．
 */
fits_hdu &fits_hdu::header_append( const fits::header_def &def )
{
    this->header_rec.append(def);
    return *this;
}

/**
 * @brief  1つのヘッダレコードを追加 (fits_header_record オブジェクトで指定)
 *
 * @note   Data Unit に関係する予約キーワード(BITPIX 等)は指定できません．
 */
fits_hdu &fits_hdu::header_append( const fits_header_record &obj )
{
    this->header_rec.append(obj);
    return *this;
}

/**
 * @brief  1つのヘッダレコードを挿入 (キーワードのみ指定)
 *
 * @note   Data Unit に関係する予約キーワード(BITPIX 等)は指定できません．
 */
fits_hdu &fits_hdu::header_insert( long index0, const char *kwd )
{
    this->header_rec.insert(index0, kwd);
    return *this;
}

/**
 * @brief  1つのヘッダレコードを挿入 (キーワードのみ指定)
 *
 * @note   Data Unit に関係する予約キーワード(BITPIX 等)は指定できません．
 */
fits_hdu &fits_hdu::header_insert( const char *keyword0, const char *kwd )
{
    this->header_rec.insert(keyword0, kwd);
    return *this;
}

/**
 * @brief  1つのヘッダレコードを挿入
 *
 * @note   Data Unit に関係する予約キーワード(BITPIX 等)は指定できません．
 */
fits_hdu &fits_hdu::header_insert( long index0,
				   const char *k, const char *v, const char *c )
{
    this->header_rec.insert(index0,k,v,c);
    return *this;
}

/**
 * @brief  1つのヘッダレコードを挿入
 *
 * @note   Data Unit に関係する予約キーワード(BITPIX 等)は指定できません．
 */
fits_hdu &fits_hdu::header_insert( const char *keyword0,
				   const char *k, const char *v, const char *c )
{
    this->header_rec.insert(keyword0,k,v,c);
    return *this;
}

/**
 * @brief  1つの記述式ヘッダレコード(HISTORYやCOMMENT)を挿入
 *
 * @note   Data Unit に関係する予約キーワード(BITPIX 等)は指定できません．
 */
fits_hdu &fits_hdu::header_insert( long index0, 
				   const char *keywd, const char *description )
{
    this->header_rec.insert(index0,keywd,description);
    return *this;
}

/**
 * @brief  1つの記述式ヘッダレコード(HISTORYやCOMMENT)を挿入
 *
 * @note   Data Unit に関係する予約キーワード(BITPIX 等)は指定できません．
 */
fits_hdu &fits_hdu::header_insert( const char *keyword0, 
				   const char *keywd, const char *description )
{
    this->header_rec.insert(keyword0,keywd,description);
    return *this;
}

/**
 * @brief  1つのヘッダレコードを挿入 (fits::header_def 構造体で指定)
 *
 * @note   Data Unit に関係する予約キーワード(BITPIX 等)は指定できません．
 */
fits_hdu &fits_hdu::header_insert( long index0,
				   const fits::header_def &def )
{
    this->header_rec.insert(index0,def);
    return *this;
}

/**
 * @brief  1つのヘッダレコードを挿入 (fits::header_def 構造体で指定)
 *
 * @note   Data Unit に関係する予約キーワード(BITPIX 等)は指定できません．
 */
fits_hdu &fits_hdu::header_insert( const char *keyword0,
				   const fits::header_def &def )
{
    this->header_rec.insert(keyword0,def);
    return *this;
}

/**
 * @brief  1つのヘッダレコードを挿入 (fits_header_record オブジェクトで指定)
 *
 * @note   Data Unit に関係する予約キーワード(BITPIX 等)は指定できません．
 */
fits_hdu &fits_hdu::header_insert( long index0,
				   const fits_header_record &obj )
{
    this->header_rec.insert(index0,obj);
    return *this;
}

/**
 * @brief  1つのヘッダレコードを挿入 (fits_header_record オブジェクトで指定)
 *
 * @note   Data Unit に関係する予約キーワード(BITPIX 等)は指定できません．
 */
fits_hdu &fits_hdu::header_insert( const char *keyword0,
				   const fits_header_record &obj )
{
    this->header_rec.insert(keyword0,obj);
    return *this;
}

/**
 * @brief  1つのヘッダキーワード名の変更
 *
 * @note   Data Unit に関係する予約キーワード(BITPIX 等)は指定できません．
 */
fits_hdu &fits_hdu::header_rename( long index0, const char *new_name )
{
    this->header_rec.rename(index0, new_name);
    return *this;
}

/**
 * @brief  1つのヘッダキーワード名の変更
 *
 * @note   Data Unit に関係する予約キーワード(BITPIX 等)は指定できません．
 */
fits_hdu &fits_hdu::header_rename( const char *keyword0, const char *new_name )
{
    this->header_rec.rename(keyword0, new_name);
    return *this;
}

/**
 * @brief  1つのヘッダレコードの消去
 *
 * @note   Data Unit に関係する予約キーワード(BITPIX 等)は指定できません．
 */
fits_hdu &fits_hdu::header_erase( long index0 )
{
    this->header_rec.erase(index0);
    return *this;
}

/**
 * @brief  1つのヘッダレコードの消去
 *
 * @note   Data Unit に関係する予約キーワード(BITPIX 等)は指定できません．
 */
fits_hdu &fits_hdu::header_erase( const char *keyword0 )
{
    this->header_rec.erase(keyword0);
    return *this;
}

/* low-level member functions */

/**
 * @brief  1つのヘッダレコードを更新 (fits::header_def 構造体で指定)
 *
 * @note   Data Unit に関係する予約キーワード(BITPIX 等)は指定できません．
 */
fits_hdu &fits_hdu::header_assign( long index0, const fits::header_def &def )
{
    this->header_rec.assign(index0,def);
    return *this;
}

/**
 * @brief  1つのヘッダレコードを更新 (fits::header_def 構造体で指定)
 *
 * @note   Data Unit に関係する予約キーワード(BITPIX 等)は指定できません．
 */
fits_hdu &fits_hdu::header_assign( const char *keyword0, 
				   const fits::header_def &def )
{
    this->header_rec.assign(keyword0,def);
    return *this;
}

/**
 * @brief  1つのヘッダレコードを更新 (fits_header_record オブジェクトで指定)
 *
 * @note   Data Unit に関係する予約キーワード(BITPIX 等)は指定できません．
 */
fits_hdu &fits_hdu::header_assign( long index0, const fits_header_record &obj )
{
    this->header_rec.assign(index0,obj);
    return *this;
}

/**
 * @brief  1つのヘッダレコードを更新 (fits_header_record オブジェクトで指定)
 *
 * @note   Data Unit に関係する予約キーワード(BITPIX 等)は指定できません．
 */
fits_hdu &fits_hdu::header_assign( const char *keyword0, 
				   const fits_header_record &obj )
{
    this->header_rec.assign(keyword0,obj);
    return *this;
}

/**
 * @brief  1つのヘッダレコードを更新
 *
 * @note   Data Unit に関係する予約キーワード(BITPIX 等)は指定できません．
 */
fits_hdu &fits_hdu::header_assign( long index0, 
				   const char *keyword, const char *value, 
				   const char *comment )
{
    this->header_rec.assign(index0,keyword,value,comment);
    return *this;
}

/**
 * @brief  1つのヘッダレコードを更新
 *
 * @note   Data Unit に関係する予約キーワード(BITPIX 等)は指定できません．
 */
fits_hdu &fits_hdu::header_assign( const char *keyword0, 
				   const char *keyword, const char *value, 
				   const char *comment )
{
    this->header_rec.assign(keyword0,keyword,value,comment);
    return *this;
}

/**
 * @brief  1つのヘッダレコードを記述式レコードで更新
 *
 * @note   Data Unit に関係する予約キーワード(BITPIX 等)は指定できません．
 */
fits_hdu &fits_hdu::header_assign( long index0, 
				 const char *keyword, const char *description )
{
    this->header_rec.assign(index0,keyword,description);
    return *this;
}

/**
 * @brief  1つのヘッダレコードを記述式レコードで更新
 *
 * @note   Data Unit に関係する予約キーワード(BITPIX 等)は指定できません．
 */
fits_hdu &fits_hdu::header_assign( const char *keyword0, 
				 const char *keyword, const char *description )
{
    this->header_rec.assign(keyword0,keyword,description);
    return *this;
}

/**
 * @brief  1つのヘッダレコードの値を更新 (低レベル・printf()の記法)
 *
 * @note   Data Unit に関係する予約キーワード(BITPIX 等)は指定できません．
 */
fits_hdu &fits_hdu::header_vassignf_value( long index0, 
					   const char *format, va_list ap )
{
    this->header_rec.vassignf_value(index0,format,ap);
    return *this;
}

/**
 * @brief  1つのヘッダレコードの値を更新 (低レベル・printf()の記法)
 *
 * @note   Data Unit に関係する予約キーワード(BITPIX 等)は指定できません．
 */
fits_hdu &fits_hdu::header_vassignf_value( const char *keyword0, 
					   const char *format, va_list ap )
{
    this->header_rec.vassignf_value(keyword0,format,ap);
    return *this;
}

/**
 * @brief  1つのヘッダレコードの値を更新 (低レベル・printf()の記法)
 *
 * @note   Data Unit に関係する予約キーワード(BITPIX 等)は指定できません．
 */
fits_hdu &fits_hdu::header_assignf_value( long index0, 
					  const char *format, ... )
{
    va_list ap;
    va_start(ap,format);
    try {
	this->header_rec.vassignf_value(index0,format,ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL",
		  "this->header_rec.vassignf_value() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  1つのヘッダレコードの値を更新 (低レベル・printf()の記法)
 *
 * @note   Data Unit に関係する予約キーワード(BITPIX 等)は指定できません．
 */
fits_hdu &fits_hdu::header_assignf_value( const char *keyword0, 
					  const char *format, ... )
{
    va_list ap;
    va_start(ap,format);
    try {
	this->header_rec.vassignf_value(keyword0,format,ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL",
		  "this->header_rec.vassignf_value() failed");
    }
    va_end(ap);
    return *this;
}


/**
 * @brief  1つのヘッダレコードのコメントを更新 (printf()の記法)
 *
 * @note   Data Unit に関係する予約キーワード(BITPIX 等)は指定できません．
 */
fits_hdu &fits_hdu::header_vassignf_comment( long index0, 
					     const char *format, va_list ap )
{
    fits::header_def rv = {NULL,NULL,NULL};

    if ( index0 < 0 || this->header_length() <= index0 ) goto quit;

    if ( format == NULL ) {
	this->header_assign(index0,rv);
    }
    else {
	tstring tstr;
	tstr.vassignf(format,ap);
	rv.comment = tstr.cstr();
	this->header_assign(index0,rv);
    }

 quit:
    return *this;
}

/**
 * @brief  1つのヘッダレコードのコメントを更新 (printf()の記法)
 *
 * @note   Data Unit に関係する予約キーワード(BITPIX 等)は指定できません．
 */
fits_hdu &fits_hdu::header_vassignf_comment( const char *keyword0, 
					     const char *format, va_list ap )
{
    return this->header_vassignf_comment(this->header_index(keyword0),
					 format,ap);
}

/**
 * @brief  1つのヘッダレコードのコメントを更新 (printf()の記法)
 *
 * @note   Data Unit に関係する予約キーワード(BITPIX 等)は指定できません．
 */
fits_hdu &fits_hdu::header_assignf_comment( long index0, 
					    const char *format, ... )
{
    va_list ap;
    va_start(ap,format);
    try {
	this->header_vassignf_comment(index0,format,ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL",
		  "this->header_vassignf_comment() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  1つのヘッダレコードのコメントを更新 (printf()の記法)
 *
 * @note   Data Unit に関係する予約キーワード(BITPIX 等)は指定できません．
 */
fits_hdu &fits_hdu::header_assignf_comment( const char *keyword0, 
					    const char *format, ... )
{
    va_list ap;
    va_start(ap,format);
    try {
	this->header_vassignf_comment(this->header_index(keyword0),
				      format,ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL",
		  "this->header_vassignf_comment() failed");
    }
    va_end(ap);
    return *this;
}

/* Overwrite all header comments with SFITSIO built-in comment strings  */
/**
 * @brief  コメントの存在にかかわらず，現在のヘッダコメント辞書の内容で埋める
 *
 *  全てのヘッダレコードのうち，現在のコメント辞書とキーワードが一致する場合，
 *  それらのレコードのコメントを現在のコメント辞書が持つコメント文で上書きし
 *  ます．
 */
fits_hdu &fits_hdu::header_assign_default_comments( int hdutype )
{
    this->header_rec.assign_default_comments(hdutype);
    return *this;
}

/* Fill all blank comments with SFITSIO built-in comment strings  */
/**
 * @brief  コメントが存在しない場合，現在のヘッダコメント辞書の内容で埋める
 *
 *  全てのヘッダレコードのコメントが空白なものについて，現在のコメント辞書と
 *  キーワードが一致する場合，それらのレコードに現在のコメント辞書が持つ
 *  コメント文をセットします．
 */
fits_hdu &fits_hdu::header_fill_blank_comments( int hdutype )
{
    this->header_rec.fill_blank_comments(hdutype);
    return *this;
}

/**
 * @brief  fits_header オブジェクトへの参照を取得
 *
 *  fits_header クラスは FITS ヘッダ全体を表現しています．
 *
 *  例:  v = fits.image(0L).header().at("TELESCOP").dvalue();
 */
fits_header &fits_hdu::header()
{
    return this->header_rec;
}

#ifdef SLI__OVERLOAD_CONST_AT
/**
 * @brief  fits_header オブジェクトへの参照を取得 (読取専用)
 *
 *  fits_header クラスは FITS ヘッダ全体を表現しています．
 *
 *  例:  v = fits.image(0L).header().at("TELESCOP").dvalue();
 */
const fits_header &fits_hdu::header() const
{
    return this->header_rec;
}
#endif

/**
 * @brief  fits_header オブジェクトへの参照を取得 (読取専用)
 *
 *  fits_header クラスは FITS ヘッダ全体を表現しています．
 *
 *  例:  v = fits.image(0L).header_cs().at("TELESCOP").dvalue();
 */
const fits_header &fits_hdu::header_cs() const
{
    return this->header_rec;
}

/**
 * @brief  fits_header_record オブジェクトへの参照を取得
 *
 *  fits_header_record クラスは 1つの FITS ヘッダレコードを表現しています．
 *
 *  例:  v = fits.image(0L).header(i).dvalue();
 */
fits_header_record &fits_hdu::header( long index0 )
{
    if ( index0 < 0 || this->header_length() <= index0 ) {
	err_throw(__FUNCTION__,"ERROR","Invalid header index");
    }
    return this->header_rec.record(index0);
}

/**
 * @brief  fits_header_record オブジェクトへの参照を取得
 *
 *  fits_header_record クラスは 1つの FITS ヘッダレコードを表現しています．
 *
 *  例:  v = fits.image(0L).header("TELESCOP").dvalue();
 */
fits_header_record &fits_hdu::header( const char *keyword0 )
{
    long index0 = this->header_index(keyword0);
#if 1
    if ( index0 < 0 ) {
	this->header_append(keyword0);
	index0 = this->header_index(keyword0);
    }
#endif
    if ( index0 < 0 ) {
	err_throw1(__FUNCTION__,"ERROR","keyword '%s' is not found",keyword0);
    }
    return this->header( index0 );
}

/**
 * @brief  fits_header_record オブジェクトへの参照を取得 (printf()の記法)
 *
 *  fits_header_record クラスは 1つの FITS ヘッダレコードを表現しています．
 *
 *  例:  v = fits.image(0L).headerf("CRVAL%d",i).dvalue();
 */
fits_header_record &fits_hdu::headerf( const char *fmt, ... )
{
    va_list ap;
    va_start(ap,fmt);
    try {
	fits_header_record &ret = this->vheaderf(fmt,ap);
	va_end(ap);
	return ret;
    }
    catch (...) {
	va_end(ap);
        err_throw(__FUNCTION__,"FATAL","this->vheaderf() failed");
    }
}

/**
 * @brief  fits_header_record オブジェクトへの参照を取得 (printf()の記法)
 *
 *  fits_header_record クラスは 1つの FITS ヘッダレコードを表現しています．
 *
 *  例:  v = fits.image(0L).headerf("CRVAL%d",i).dvalue();
 */
fits_header_record &fits_hdu::vheaderf( const char *fmt, va_list ap )
{
    tstring buf;

    if ( fmt == NULL ) return this->header(fmt);

    buf.vprintf(fmt,ap);
    return this->header(buf.cstr());
}

#ifdef SLI__OVERLOAD_CONST_AT
/**
 * @brief  fits_header_record オブジェクトへの参照を取得 (読取専用)
 *
 *  fits_header_record クラスは 1つの FITS ヘッダレコードを表現しています．
 *
 *  例:  v = fits.image(0L).header(i).dvalue();
 */
const fits_header_record &fits_hdu::header( long index0 ) const
{
    return this->header_cs(index0);
}

/**
 * @brief  fits_header_record オブジェクトへの参照を取得 (読取専用)
 *
 *  fits_header_record クラスは 1つの FITS ヘッダレコードを表現しています．
 *
 *  例:  v = fits.image(0L).header("TELESCOP").dvalue();
 */
const fits_header_record &fits_hdu::header( const char *keyword0 ) const
{
    return this->header_cs(keyword0);
}

/**
 * @brief  fits_header_record オブジェクトへの参照を取得 (読取専用・printf()の記法)
 *
 *  fits_header_record クラスは 1つの FITS ヘッダレコードを表現しています．
 *
 *  例:  v = fits.image(0L).headerf("CRVAL%d",i).dvalue();
 */
const fits_header_record &fits_hdu::headerf( const char *fmt, ... ) const
{
    va_list ap;
    va_start(ap,fmt);
    try {
	const fits_header_record &ret = this->vheaderf_cs(fmt,ap);
	va_end(ap);
	return ret;
    }
    catch (...) {
	va_end(ap);
        err_throw(__FUNCTION__,"FATAL","this->vheaderf_cs() failed");
    }
}

/**
 * @brief  fits_header_record オブジェクトへの参照を取得 (読取専用・printf()の記法)
 *
 *  fits_header_record クラスは 1つの FITS ヘッダレコードを表現しています．
 *
 *  例:  v = fits.image(0L).headerf("CRVAL%d",i).dvalue();
 */
const fits_header_record &fits_hdu::vheaderf( const char *fmt, va_list ap ) const
{
    return this->vheaderf_cs(fmt, ap);
}
#endif

/**
 * @brief  fits_header_record オブジェクトへの参照を取得 (読取専用)
 *
 *  fits_header_record クラスは 1つの FITS ヘッダレコードを表現しています．
 *
 *  例:  v = fits.image(0L).header_cs(i).dvalue();
 */
const fits_header_record &fits_hdu::header_cs( long index0 ) const
{
    if ( index0 < 0 || this->header_length() <= index0 ) {
	err_throw(__FUNCTION__,"ERROR","Invalid header index");
    }
    return this->header_rec.record_cs(index0);
}

/**
 * @brief  fits_header_record オブジェクトへの参照を取得 (読取専用)
 *
 *  fits_header_record クラスは 1つの FITS ヘッダレコードを表現しています．
 *
 *  例:  v = fits.image(0L).header_cs("TELESCOP").dvalue();
 */
const fits_header_record &fits_hdu::header_cs( const char *keyword0 ) const
{
    long index0 = this->header_index(keyword0);
    return this->header_cs( index0 );
}

/**
 * @brief  fits_header_record オブジェクトへの参照を取得 (読取専用・printf()の記法)
 *
 *  fits_header_record クラスは 1つの FITS ヘッダレコードを表現しています．
 *
 *  例:  v = fits.image(0L).headerf_cs("CRVAL%d",i).dvalue();
 */
const fits_header_record &fits_hdu::headerf_cs( const char *fmt, ... ) const
{
    va_list ap;
    va_start(ap,fmt);
    try {
	const fits_header_record &ret = this->vheaderf_cs(fmt,ap);
	va_end(ap);
	return ret;
    }
    catch (...) {
	va_end(ap);
        err_throw(__FUNCTION__,"FATAL","this->vheaderf_cs() failed");
    }
}

/**
 * @brief  fits_header_record オブジェクトへの参照を取得 (読取専用・printf()の記法)
 *
 *  fits_header_record クラスは 1つの FITS ヘッダレコードを表現しています．
 *
 *  例:  v = fits.image(0L).headerf_cs("CRVAL%d",i).dvalue();
 */
const fits_header_record &fits_hdu::vheaderf_cs( const char *fmt, va_list ap ) const
{
    tstring buf;

    if ( fmt == NULL ) return this->header_cs(fmt);

    buf.vprintf(fmt,ap);
    return this->header_cs(buf.cstr());
}

/* ヘッダ情報読み取り */

/**
 * @brief  キーワードに対応するヘッダレコード番号を取得 (記述式レコードは除外)
 */
long fits_hdu::header_index( const char *keyword0 ) const
{
    return this->header_rec.index(keyword0);
}

/**
 * @brief  キーワードに対応するヘッダレコード番号を取得
 */
long fits_hdu::header_index( const char *keyword0, bool is_description ) const
{
    return this->header_rec.index(keyword0,is_description);
}

/**
 * @brief  POSIX拡張正規表現でキーワードを検索
 */
long fits_hdu::header_regmatch( const char *keypat, 
				ssize_t *rpos, size_t *rlen )
{
    return this->header_rec.regmatch(keypat,rpos,rlen);
}

/**
 * @brief  POSIX拡張正規表現でキーワードを連続的に検索
 */
long fits_hdu::header_regmatch( long index0, const char *keypat, 
				ssize_t *rpos, size_t *rlen )
{
    return this->header_rec.regmatch(index0,keypat,rpos,rlen);
}

/* returns length of raw value. More than 0 means NON-NULL. */
/* Negative value is returned when a record is not found.   */

/**
 * @brief  キーワードに対応する生の文字列値の長さを取得 (存在チェックに使用可)
 *
 * @param   keyword ヘッダキーワード
 * @return  正の値: 生の文字列値(「'」を含む)の長さ <br>
 *          0: 値が存在しない場合 <br>
 *          負の値: キーワードが存在しない場合
 */
long fits_hdu::header_value_length( const char *keyword ) const
{
    return this->header_rec.value_length(keyword);
}

/**
 * @brief  キーワードに対応する生の文字列値の長さを取得 (存在チェックに使用可)
 *
 * @param   index レコード番号
 * @return  正の値: 生の文字列値(「'」を含む)の長さ <br>
 *          0: 値が存在しない場合 <br>
 *          負の値: キーワードが存在しない場合
 */
long fits_hdu::header_value_length( long index ) const
{
    return this->header_rec.value_length(index);
}

#if 0
long fits_hdu::header_num_records()
{
    return this->header_rec.length();
}
#endif

/**
 * @brief  ヘッダレコードの長さを取得
 */
long fits_hdu::header_length() const
{
    return this->header_rec.length();
}

/**
 * @brief  ヘッダレコードの長さを取得
 *
 * @note   fits_hdu::header_length() との違いはありません．
 */
long fits_hdu::header_size() const
{
    return this->header_rec.length();
}

/* discard original 80-char record, and reformat all records */
/**
 * @brief  全ヘッダレコードの再フォーマットを行なう
 *
 *  各 fits_header_record が現在保持している 80文字のフォーマット済み文字列を，
 *  現在のキーワード，値，コメントの内容から作りなおします．
 */
fits_hdu &fits_hdu::header_reformat()
{
    this->header_rec.reformat();
    return *this;
}

/* これは fitscc クラスから利用してはいけない */
/**
 * @brief  全ヘッダレコードのフォーマット済み文字列(80×n文字)を取得
 *
 *  オブジェクト内で生成された，全ヘッダレコードのフォーマット済み文字列
 *  (80×n文字)を返します．改行文字は含まれず，文字列は '\0' で終端しています．
 *  
 * @return  フォーマット済み文字列(80×n文字)
 */
const char *fits_hdu::header_formatted_string()
{
    ssize_t len;
    const char *return_value = NULL;

    /* calling overridden member function; setup system header */
    len = -1;
    if ( this->manager == NULL ) {	/* fitscc の管理下に置かれてない場合 */
	len = this->stream_length();
    }
    else {				/* fitscc の管理下に置かれている時 */
	long i;
	for ( i=0 ; i < this->manager->length() ; i++ ) {
	    if ( &(this->manager->hdu(i)) == this ) {
		/* fitscc の stream_length() を呼ぶ */
		len = this->manager->stream_length();
		break;
	    }
	}
	if ( i == this->manager->length() ) {
	    err_throw(__FUNCTION__,"FATAL",
		      "internal error; fitscc does not manage me...");
	}
    }
    if ( len < 0 ) {
	err_report(__FUNCTION__,"ERROR","this->stream_length() failed");
	goto quit;
    }

    return_value = this->header_rec.formatted_string();

 quit:
    return return_value;
}

/**
 * @deprecated  互換を保つために存在．使わないでください．
 */
long fits_hdu::sysheader_length() const
{
    return this->header_rec.length();
}

/**
 * @deprecated  互換を保つために存在．使わないでください．
 */
long fits_hdu::sysheader_size() const
{
    return this->header_rec.length();
}

/**
 * @deprecated  互換を保つために存在．使わないでください．
 */
long fits_hdu::sysheader_index( const char *keyword0 ) const
{
    return this->header_rec.index(keyword0);
}

/**
 * @deprecated  互換を保つために存在．使わないでください．
 */
const char *fits_hdu::sysheader_keyword( long index ) const
{
    return this->header_rec.record_cs(index).keyword();
}

/**
 * @deprecated  互換を保つために存在．使わないでください．
 */
const char *fits_hdu::sysheader_value( long index ) const
{
    return this->header_rec.record_cs(index).value();
}

/**
 * @deprecated  互換を保つために存在．使わないでください．
 */
const char *fits_hdu::sysheader_value( const char *keyword ) const
{
    long idx = this->header_rec.index(keyword);
    if ( idx < 0 ) return NULL;
    else return this->header_rec.record_cs(idx).value();
}

/**
 * @deprecated  互換を保つために存在．使わないでください．
 */
const char *fits_hdu::sysheader_comment( long index ) const
{
    return this->header_rec.record_cs(index).comment();
}

/**
 * @deprecated  互換を保つために存在．使わないでください．
 */
const char *fits_hdu::sysheader_comment( const char *keyword ) const
{
    long idx = this->header_rec.index(keyword);
    if ( idx < 0 ) return NULL;
    return this->header_rec.record_cs(idx).comment();
}

/* これは fitscc クラスから利用してはいけない */
/**
 * @deprecated  互換を保つために存在．使わないでください．
 */
const char *fits_hdu::sysheader_formatted_string()
{
    return this->header_formatted_string();
}


/* これは fitscc クラスから利用してはいけない */
/**
 * @brief  HDU の名前(EXTNAME)を設定
 */
fits_hdu &fits_hdu::assign_hduname( const char *hduname )
{
    return this->assign_extname(hduname);
}

/* これは fitscc クラスから利用してはいけない */
/**
 * @brief  HDU の名前(EXTNAME)を設定
 *
 * @note  fits_hdu::assign_hduname() との違いはありません．
 */
fits_hdu &fits_hdu::assign_extname( const char *extname )
{
    //err_report(__FUNCTION__,"DEBUG","called!!");
    if ( this->manager == NULL ) {	/* fitscc の管理下に置かれてない場合 */
	return this->_assign_extname(extname);
    }
    else {				/* fitscc の管理下に置かれている時 */
	long i;
	for ( i=0 ; i < this->manager->length() ; i++ ) {
	    if ( &(this->manager->hdu(i)) == this ) {
		/* 間接的に _assign_extname() を呼ぶ */
		this->manager->assign_extname(i, extname);
		break;
	    }
	}
	if ( i == this->manager->length() ) {
	    err_throw(__FUNCTION__,"FATAL",
		      "internal error; fitscc does not manage me...");
	}
    }
    return *this;
}

/**
 * @brief  HDU のバージョン(EXTVER)を設定
 */
fits_hdu &fits_hdu::assign_hduver( long long hduver )
{
    return this->assign_extver(hduver);
}

/**
 * @brief  HDU のバージョン(EXTVER)を設定
 *
 * @note  fits_hdu::assign_hduver() との違いはありません．
 */
fits_hdu &fits_hdu::assign_extver( long long extver )
{
    if ( extver == INDEF_LLONG || extver == FITS::INDEF ) {
	this->extver_rec = NULL;
    }
    else {
	this->extver_rec.printf("%lld",extver);
    }
    return *this;
}

/**
 * @brief  HDU のレベル(EXTLEVEL)を設定
 */
fits_hdu &fits_hdu::assign_hdulevel( long long hdulevel )
{
    return this->assign_extlevel(hdulevel);
}

/**
 * @brief  HDU のレベル(EXTLEVEL)を設定
 *
 * @note  fits_hdu::assign_hdulevel() との違いはありません．
 */
fits_hdu &fits_hdu::assign_extlevel( long long extlevel )
{
    if ( extlevel == INDEF_LLONG || extlevel == FITS::INDEF ) {
	this->extlevel_rec = NULL;
    }
    else {
	this->extlevel_rec.printf("%lld",extlevel);
    }
    return *this;
}

/**
 * @brief  HDU の名前(EXTNAME)を取得
 */
const char *fits_hdu::hduname() const
{
    return this->extname_rec.cstr();
}

/**
 * @brief  HDU の名前(EXTNAME)を取得
 *
 * @note  fitscc::extname() との違いはありません．
 */
const char *fits_hdu::extname() const
{
    return this->extname_rec.cstr();
}

/**
 * @brief  HDU のバージョン(EXTVER)を取得
 */
long long fits_hdu::hduver() const
{
    return this->extver_rec.atoll();
}

/**
 * @brief  HDU のバージョン(EXTVER)を取得
 *
 * @note  fits_hdu::hduver() との違いはありません．
 */
long long fits_hdu::extver() const
{
    return this->extver_rec.atoll();
}

/**
 * @brief  HDU のバージョン(EXTVER)を文字列で取得
 */
const char *fits_hdu::hduver_value() const
{
    return this->extver_rec.cstr();
}

/**
 * @brief  HDU のバージョン(EXTVER)を文字列で取得
 *
 * @note  fits_hdu::hduver_value() との違いはありません．
 */
const char *fits_hdu::extver_value() const
{
    return this->extver_rec.cstr();
}

/**
 * @brief  HDU のレベル(EXTLEVEL)を取得
 */
long long fits_hdu::hdulevel() const
{
    return this->extlevel_rec.atoll();
}

/**
 * @brief  HDU のレベル(EXTLEVEL)を取得
 *
 * @note  fits_hdu::hdulevel() との違いはありません．
 */
long long fits_hdu::extlevel() const
{
    return this->extlevel_rec.atoll();
}

/**
 * @brief  HDU のレベル(EXTLEVEL)を文字列で取得
 */
const char *fits_hdu::hdulevel_value() const
{
    return this->extlevel_rec.cstr();
}

/**
 * @brief  HDU のレベル(EXTLEVEL)を文字列で取得
 *
 * @note  fits_hdu::hdulevel_value() との違いはありません．
 */
const char *fits_hdu::extlevel_value() const
{
    return this->extlevel_rec.cstr();
}

/**
 * @brief  HDU のバージョン(EXTVER)がセットされているかを返す
 */
bool fits_hdu::hduver_is_set() const
{
    return (this->extver_rec.cstr() != NULL);
}

/**
 * @brief  HDU のバージョン(EXTVER)がセットされているかを返す
 *
 * @note  fits_hdu::hduver_is_set() との違いはありません．
 */
bool fits_hdu::extver_is_set() const
{
    return (this->extver_rec.cstr() != NULL);
}

/**
 * @brief  HDU のレベル(EXTLEVEL)がセットされているかを返す
 */
bool fits_hdu::hdulevel_is_set() const
{
    return (this->extlevel_rec.cstr() != NULL);
}

/**
 * @brief  HDU のレベル(EXTLEVEL)がセットされているかを返す
 *
 * @note  fits_hdu::hdulevel_is_set() との違いはありません．
 */
bool fits_hdu::extlevel_is_set() const
{
    return (this->extlevel_rec.cstr() != NULL);
}

/**
 * @brief  CHECKSUM がエラーかどうかを返す (未実装)
 *
 * @deprecated  未実装．
 */
bool fits_hdu::checksum_error() const
{
    return this->checksum_error_rec;
}

/**
 * @brief  DATASUM がエラーかどうかを返す (未実装)
 *
 * @deprecated  未実装．
 */
bool fits_hdu::datasum_error() const
{
    return this->datasum_error_rec;
}

/**
 * @brief  ヘッダの情報から HDU のタイプを判定して返す
 */
int fits_hdu::hdutype_on_header()
{
    int htype = this->header_rec.hdutype();

    if ( htype == FITS::ANY_HDU ) {
	err_report(__FUNCTION__,"ERROR","unsupported HDU type");
    }

    return htype;
}

/* (互換を保つためにあるだけ:使わないこと) */
/* これは fitscc クラスから利用してはいけない */
/**
 * @deprecated  互換を保つために存在．使わないでください．
 */
const char *fits_hdu::allheader_formatted_string()
{
    return this->header_formatted_string();
}

/*
 * protected member functions
 */

/**
 * @brief  fits_headerオブジェクトから入力し，ヘッダを読む
 *
 * @return  非負値: 読み込んだバイト数<br>
 *          負値: エラー
 * @note   このメンバ関数は protected です．
 */
int fits_hdu::read_header_object( const fits_header &header_all )
{
    return this->header_load(header_all);
}

/**
 * @brief  ストリームからヘッダを読む
 *
 * @param   sref 読み込み対象のストリーム (cstreamioの継承クラス)
 * @param   max_bytes_read 読み込まれるべき最大のバイト数
 * @return  非負値: 読み込んだバイト数<br>
 *          負値: エラー
 * @note   このメンバ関数は protected です．
 */
ssize_t fits_hdu::read_stream( cstreamio &sref, size_t max_bytes_read )
{
    return this->header_load(sref,&max_bytes_read);
}

/**
 * @brief  ストリームからヘッダを読む
 *
 * @param   sref 読み込み対象のストリーム (cstreamioの継承クラス)
 * @return  非負値: 読み込んだバイト数<br>
 *          負値: エラー
 * @note   このメンバ関数は protected です．
 */
ssize_t fits_hdu::read_stream( cstreamio &sref )
{
    return this->header_load(sref,NULL);
}

/**
 * @brief  ストリームへの Header Unit と Data Unit の書き込み
 *
 *  このメンバ関数は継承クラスの write_stream() から呼び出され，EXTEND値 を
 *  セットし，ヘッダとデータを保存する．<br>
 *  ヘッダに CHECKSUM，DATASUM が存在する場合は計算して値を埋めてから保存する．
 *  呼び出しの順は，例えば fits_table の場合，<br>
 *   fits_table::write_stream() <br>
 *   -> fits_hdu::write_stream <br>
 *   -> fits_table::save_or_check_data() <br>
 *  という経路になる．
 *
 * @param   sref 書き込み対象のストリーム (cstreamioの継承クラス)
 * @return  非負値: 書き込んだバイト数<br>
 *          負値: エラー
 * @note    例外はここでは捉えないので，継承クラスで捉える事．<br>
 *          このメンバ関数は protected です．
 */
/* protected */
ssize_t fits_hdu::write_stream( cstreamio &sref )
{
    ssize_t return_value = -1;
    ssize_t len_written_header, len_written_data;
    long extend_idx, checksum_idx, datasum_idx;
    unsigned long datasum = 0;
    tstring old_csum, new_csum;

    //err_report(__FUNCTION__,"DEBUG","called!!");

    extend_idx = this->header_rec.index("EXTEND");

    /* set EXTEND value of Primary HDU */
    if ( 0 <= extend_idx && 
	 this->manager != NULL && this == &(this->manager->hdu(0L)) ) {
	tstring orig_tf;
	orig_tf = this->header_rec.at_cs(extend_idx).svalue();
	/* check number of HDU, and set T or F */
	if ( 1 < this->manager->length() ) {
	    if ( orig_tf.strcmp("T") != 0 ) {
		this->header_rec.at(extend_idx).assign(true);
	    }
	}
	else {
	    if ( orig_tf.strcmp("F") != 0 ) {
		this->header_rec.at(extend_idx).assign(false);
	    }
	}
    }

    checksum_idx = this->header_rec.index("CHECKSUM");
    datasum_idx = this->header_rec.index("DATASUM");

    /* get datasum, if required */
    if ( 0 <= checksum_idx || 0 <= datasum_idx ) {
	fitsio_csum csum_info;
	tstring datasum_str;
	ssize_t len;
	/* init checksum */
	initialize_csum( &csum_info, 0 );
	/* get datasum (これは継承クラスのメンバ関数) */
	len = this->save_or_check_data( NULL, (void *)&csum_info );
	if ( len < 0 ) {
	    err_report(__FUNCTION__,"ERROR","this->image_save() failed");
	    goto quit;
	}
	datasum = csum_info.sum;
	/* convert datasum into string */
	datasum_str.printf("%lu",datasum);
	/* write datasum to header record, if exists */
	if ( 0 <= datasum_idx ) {
	    fits_header_record &hdr_rec = this->header_rec.record(datasum_idx);
	    old_csum = hdr_rec.svalue();
	    if ( datasum_str.strcmp(hdr_rec.svalue()) != 0 ) {
	        hdr_rec.assignf_value("'%10s'",datasum_str.cstr());
	    }
	    new_csum = hdr_rec.svalue();
	    /* compare old and new one */
	    if ( new_csum.strcmp(old_csum) != 0 ) {
		/* update comment string */
		fits_header_record t_rec = hdr_rec;
		t_rec.assign_system_time().assign_default_comment();
		hdr_rec.assignf_comment("%s : %s", 
					t_rec.comment(), t_rec.svalue());
	    }
	}
    }

    /* fill 16 zero chars, calculate, and assign. if exists */
    if ( 0 <= checksum_idx ) {
	fits_header_record &hdr_rec = this->header_rec.record(checksum_idx);
	const char *encoded_csum;
	old_csum = hdr_rec.svalue();
	hdr_rec.assign("0000000000000000");
	encoded_csum = this->header_rec.encoded_checksum(datasum);
	hdr_rec.assign(encoded_csum);
	new_csum = hdr_rec.svalue();
	/* compare old and new one */
	if ( new_csum.strcmp(old_csum) != 0 ) {
	    /* update comment string */
	    fits_header_record t_rec = hdr_rec;
	    t_rec.assign_system_time().assign_default_comment();
	    hdr_rec.assignf_comment("%s : %s", 
				    t_rec.comment(), t_rec.svalue());
	    /* update checksum again due to update of comment */
	    hdr_rec.assign("0000000000000000");
	    encoded_csum = this->header_rec.encoded_checksum(datasum);
	    hdr_rec.assign(encoded_csum);
	}
    }

    /* Output header (includes 'END' keyword) */
    len_written_header = this->header_rec.write_stream(sref,true);
    if ( len_written_header < 0 ) {
	err_report(__FUNCTION__,"ERROR",
		   "this->header_rec.write_stream() failed");
	goto quit;
    }

    if ( 0 < len_written_header ) sref.flush();

    /* write data part to stream */
    len_written_data = this->save_or_check_data( &sref, NULL );
    if ( len_written_data < 0 ) {
	err_report(__FUNCTION__,"ERROR","this->save_or_check_data() failed");
	goto quit;
    }

    if ( 0 < len_written_data ) sref.flush();

    return_value = len_written_header + len_written_data;
 quit:
    return return_value;
}

/**
 * @brief  ストリームにアクセスし，Data Unit を読み飛ばす
 *
 * @param   sref 読み飛ばし対象のストリーム (cstreamioの継承クラス)
 * @param   max_bytes_skip 読み飛ばされるべき最大のバイト数
 * @return  非負値: 読み飛ばしたバイト数<br>
 *          負値: エラー
 * @note    このメンバ関数は protected です．
 */
ssize_t fits_hdu::skip_data_stream( cstreamio &sref, size_t max_bytes_skip )
{
    return this->header_rec.skip_data_stream(sref,max_bytes_skip);
}

/**
 * @brief  ストリームにアクセスし，Data Unit を読み飛ばす
 *
 * @param   sref 読み飛ばし対象のストリーム (cstreamioの継承クラス)
 * @return  非負値: 読み飛ばしたバイト数<br>
 *          負値: エラー
 * @note    このメンバ関数は protected です．
 */
ssize_t fits_hdu::skip_data_stream( cstreamio &sref )
{
    return this->header_rec.skip_data_stream(sref);
}

/**
 * @brief  Data Unit 書き込み or チェックサム生成のための関数スロット
 *
 * @note   fits_hdu::write_stream() から呼び出される．<br>
 *         継承クラスでオーバーライドすること．<br>
 *         このメンバ関数は protected です．
 */
ssize_t fits_hdu::save_or_check_data( cstreamio *sptr, void *c_sum_info )
{
    return 0;
}

/* protected*/
/**
 * @brief  ストリームに書き込まれるであろうバイト数を返す
 *
 * @return  非負値: 書き込まれるであろうバイト数<br>
 *          負値: エラー
 * @note   このメンバ関数は protected です．
 */
ssize_t fits_hdu::stream_length()
{
    ssize_t return_size = -1;
    ssize_t len;

    len = this->header_rec.formatted_length();
    if ( 0 < len ) {
	size_t block, size_all = 0;

	size_all += len;			/* all records  */
	size_all += FITS::HEADER_RECORD_UNIT;	/* "END" record */

	block = 1 + (size_all - 1) / FITS::FILE_RECORD_UNIT;

	return_size = FITS::FILE_RECORD_UNIT * block;
    }

    return return_size;
}


/**
 * @brief  自身を管理するためのオブジェクトを登録
 *
 * @note   このメンバ関数は protected です．
 */
fits_hdu &fits_hdu::register_manager( fitscc *ptr )
{
    this->manager = ptr;
    return *this;
}

/**
 * @brief  HDU の名前(EXTNAME)を設定 (低レベル)
 *
 * @note   これは fitscc クラスで利用する．<br>
 *         このメンバ関数は protected です．
 */
fits_hdu &fits_hdu::_assign_extname( const char *extname )
{
    //err_report(__FUNCTION__,"DEBUG","called!!");
    if ( extname != NULL ) {
#if 0	/* ここは自由にいこう */
	if ( this->is_primary_rec == true &&
	     c_strcmp(extname,"Primary") != 0 ) {
	    extname = "Primary";
	    err_report(__FUNCTION__,"WARNING",
		       "cannot set the extname to the Primary HDU");
	}
#endif
	/* extname が NULL だった場合は NULL にする */
	/* init() とか swap() とかでは情報がそのままコピーされないと困るので */
	//this->extname_rec.assign(extname);
	this->extname_rec = extname;
    }

    return *this;
}

/**
 * @brief  クラスの継承レベルを1つアップ
 *
 * @note   継承クラスのコンストラクタで使用．<br>
 *         このメンバ関数は protected です．
 */
int fits_hdu::increase_classlevel()
{
    this->classlevel_rec ++;
    return this->classlevel_rec;
}

/**
 * @brief  HDU の種別を設定
 *
 * @note   継承クラスのコンストラクタで使用．<br>
 *         このメンバ関数は protected です．
 */
fits_hdu &fits_hdu::set_hdutype( int hdutype )
{
#if 0
    if ( this->is_primary_rec == true ) {
	if ( hdutype != FITS::ANY_HDU && 
	     hdutype != FITS::IMAGE_HDU ) {
	    err_report(__FUNCTION__,"ERROR",
		       "Invalid HDU Type for the Primary HDU");
	    return -1;
	}
    }
#endif
    //fprintf(stderr,"debug: hdutype = %d\n",hdutype);
    this->hdutype_rec = hdutype;
    return *this;
}

#if 0
int fits_hdu::set_primary_hdu( bool is_primary_hdu )
{
    if ( is_primary_hdu == true ) {
	if ( this->hdutype_rec != FITS::ANY_HDU && 
	     this->hdutype_rec != FITS::IMAGE_HDU ) {
	    err_report(__FUNCTION__,"ERROR",
		       "Invalid HDU Type for the Primary HDU");
	    return -1;
	}
    }

    this->is_primary_rec = is_primary_hdu;

#if 0	/* ここは自由にいこう */
    if ( is_primary_hdu == true ) {
	if ( this->extname_rec.cstr() != NULL ) {
	    if ( this->extname_rec.strcmp("Primary") != 0 ) {
		err_report(__FUNCTION__,"WARNING",
			   "the extname is changed to 'Primary'");
		if ( this->extname_rec.assign("Primary") < 0 ) {
		    err_report(__FUNCTION__,"ERROR",
			       "this->extname_rec.assign() failed");
		    return -1;
		}
	    }
	}
    }
#endif
    return 0;
}
#endif

/**
 * @brief  fitscc オブジェクトの管理下にある場合，HDU 番号を返す
 *
 * @note   継承クラスから HDU 番号を問い合わせる時に使う．<br>
 *         このメンバ関数は protected です．
 */
long fits_hdu::hdu_index() const
{
    if ( this->manager != NULL ) {
	long hdu_len = this->manager->length();
	long i;
	for ( i=0 ; i < hdu_len ; i++ ) {
	    if ( &(this->manager->hdu(i)) == this ) return i;
	}
    }
    return -1;
}

//const char *fits_hdu::fmttype() const
//{
//    if ( this->manager != NULL ) return this->manager->fmttype();
//    else return NULL;
//}

//long long fits_hdu::ftypever() const
//{
//    if ( this->manager != NULL ) return this->manager->ftypever();
//    else return 0;
//}

/**
 * @brief  Data Unit に関連するヘッダレコードをセットアップ
 *
 * @note   write_stream() する時などに，継承クラスの setup_sys_header() から
 *         呼ばれる．<br>
 *         このメンバ関数は protected です．
 */
fits_hdu &fits_hdu::setup_sys_header()
{
    fits_header &hdr_rec = this->header_rec;
    bool primary_hdu = (this->hdu_index() == 0L);
    const char *fmttype = NULL;
    const char *ftypever = NULL;

    /*
     * FMTTYPE, FTYPEVER については，fitscc の private メンバに値がセットされて
     * いる場合のみ処理を行なう．
     */
    if ( this->manager != NULL ) {
	fmttype = this->manager->fmttype_rec.cstr();
	ftypever = this->manager->ftypever_rec.cstr();
    }

    //err_report(__FUNCTION__,"DEBUG","called!");

    /* turn prohibition off */
    //err_report(__FUNCTION__,"DEBUG","start: set to false");
    hdr_rec.set_sysrecords_prohibition(false);

    try {
	size_t n_top_recs = 0, insert_point;
	bool set_ok = false;

	if ( primary_hdu == true ) {

	  if ( this->hdutype_rec == FITS::IMAGE_HDU ) {

	    fits::header_def defs[] = { 
		{"SIMPLE","T",""}, 
		{"BITPIX","16",""}, 
		{"NAXIS","0",""}, 
		{NULL,"",""},
		{NULL,"",""},
		{NULL,"",""},
		{NULL,"",""},
		{NULL}
	    };
	    size_t idx_append = 3;

	    const char *removed_keywords[] = {"XTENSION","PCOUNT","GCOUNT",
					      NULL};
	    tstring sval;

	    /* for old FITS files, etc. */
	    if ( this->manager != NULL && this->manager->length() == 1 &&
		 hdr_rec.index("SIMPLE") == 0 &&
		 hdr_rec.index("EXTEND") < 0 ) {
		/* not set EXTEND */
	    }
	    else {
		/* set EXTEND */
		defs[idx_append].keyword = "EXTEND";
		defs[idx_append].value   = "T";
		idx_append ++;
	    }

	    /* append FMTTYPE */
	    if ( fmttype != NULL && fmttype[0] != '\0' ) {
		defs[idx_append].keyword = "FMTTYPE";
		idx_append ++;
	    }
	    /* append FTYPEVER */
	    if ( ftypever != NULL && ftypever[0] != '\0' ) {
		defs[idx_append].keyword = "FTYPEVER";
		idx_append ++;
	    }

	    /* setup ... */
	    if ( this->header_setup_top_records(defs,removed_keywords) < 0 ) {
		err_throw(__FUNCTION__,"FATAL",
			  "this->header_setup_top_records() failed");
	    }
	    for ( n_top_recs=0 ; defs[n_top_recs].keyword != NULL ; )
		n_top_recs ++;

	    /* update FMTTYPE */
	    if ( fmttype != NULL && fmttype[0] != '\0' ) {
		sval = hdr_rec.at("FMTTYPE").svalue();
		if ( sval.strcmp(fmttype) != 0 ) {
		    hdr_rec.at("FMTTYPE").assign(fmttype);
		}
	    }
	    /* update FTYPEVER */
	    if ( ftypever != NULL && ftypever[0] != '\0' ) {
		size_t endpos;
		tstring s = ftypever;
		long long llv;
		double dv;
	        /* ファイルからの読み取り時の動作にあわせる */
		sval = hdr_rec.at("FTYPEVER").svalue();
		if ( sval.strcmp(ftypever) != 0 ) {
		    llv = s.strtoll(10,&endpos);
		    if ( s.length() == endpos ) {
		        hdr_rec.at("FTYPEVER").assign(llv);
		    }
		    else {
		        dv = s.strtod(&endpos);
		        if ( s.length() == endpos ) {
			    hdr_rec.at("FTYPEVER").assign(dv);
		        }
		        else {
			    hdr_rec.at("FTYPEVER").assign(ftypever);
			}
		    }
		}
	    }

	    /* mark */
	    set_ok = true;

	  }	/* this->hdutype_rec == FITS::IMAGE_HDU */

	}
	else {
	    const char *removed_keywords[] = {NULL,NULL,NULL,NULL};
	    size_t idx_append = 0;

	    /*
	     * fitscc の private メンバに登録されている場合は，削除対象とする
	     */
	    if ( fmttype != NULL && fmttype[0] != '\0' ) {
		removed_keywords[idx_append] = "FMTTYPE";
		idx_append ++;
	    }
	    if ( ftypever != NULL && ftypever[0] != '\0' ) {
		removed_keywords[idx_append] = "FTYPEVER";
		idx_append ++;
	    }

	    if ( this->hdutype_rec == FITS::IMAGE_HDU ) {

		fits::header_def defs[] = { 
		    {"XTENSION","'IMAGE'",""}, 
		    {"BITPIX","16",""}, 
		    {"NAXIS","0",""}, 
		    {"PCOUNT","0",""}, 
		    {"GCOUNT","1",""}, 
		    {NULL}
		};
		tstring val;

		if ( this->header_setup_top_records(defs,removed_keywords) < 0 ) {
		    err_throw(__FUNCTION__,"FATAL",
			      "this->header_setup_top_records() failed");
		}
		for ( n_top_recs=0 ; defs[n_top_recs].keyword != NULL ; )
		    n_top_recs ++;

		/* 固定値 */
		val = hdr_rec.at("XTENSION").value();
		if ( val.strcmp("'IMAGE   '") != 0 ) {
		    hdr_rec.at("XTENSION").assign_value("'IMAGE   '");
		}

		/* mark */
		set_ok = true;

	    }
	    else if ( this->hdutype_rec == FITS::BINARY_TABLE_HDU ) {

		fits::header_def defs[] = { 
		    {"XTENSION","'BINTABLE'",""}, 
		    {"BITPIX","8",""}, 
		    {"NAXIS", "2",""}, 
		    {"NAXIS1","0",""}, 
		    {"NAXIS2","0",""}, 
		    {"PCOUNT","0",""}, 
		    {"GCOUNT","1",""}, 
		    {"TFIELDS","0",""}, 
		    {NULL}
		};
		tstring val;

		if ( this->header_setup_top_records(defs,removed_keywords) < 0 ) {
		    err_throw(__FUNCTION__,"FATAL",
			      "this->header_setup_top_records() failed");
		}
		for ( n_top_recs=0 ; defs[n_top_recs].keyword != NULL ; )
		    n_top_recs ++;

		/* 固定値 */
		val = hdr_rec.at("XTENSION").value();
		if ( val.strcmp("'BINTABLE'") != 0 ) {
		    hdr_rec.at("XTENSION").assign_value("'BINTABLE'");
		}
		val = hdr_rec.at("BITPIX").value();
		if ( val.strcmp("8") != 0 ) {
		    hdr_rec.at("BITPIX").assign(8);
		}
		val = hdr_rec.at("NAXIS").value();
		if ( val.strcmp("2") != 0 ) {
		    hdr_rec.at("NAXIS").assign(2);
		}

		/* mark */
		set_ok = true;

	    }
	    else if ( this->hdutype_rec == FITS::ASCII_TABLE_HDU ) {

		fits::header_def defs[] = { 
		    {"XTENSION","'TABLE'",""}, 
		    {"BITPIX","8",""}, 
		    {"NAXIS", "2",""}, 
		    {"NAXIS1","0",""}, 
		    {"NAXIS2","0",""}, 
		    {"PCOUNT","0",""}, 
		    {"GCOUNT","1",""}, 
		    {"TFIELDS","0",""}, 
		    {NULL}
		};
		tstring val;

		if ( this->header_setup_top_records(defs,removed_keywords) < 0 ) {
		    err_throw(__FUNCTION__,"FATAL",
			      "this->header_setup_top_records() failed");
		}
		for ( n_top_recs=0 ; defs[n_top_recs].keyword != NULL ; )
		    n_top_recs ++;

		/* 固定値 */
		val = hdr_rec.at("XTENSION").value();
		if ( val.strcmp("'TABLE   '") != 0 ) {
		    hdr_rec.at("XTENSION").assign_value("'TABLE   '");
		}
		val = hdr_rec.at("BITPIX").value();
		if ( val.strcmp("8") != 0 ) {
		    hdr_rec.at("BITPIX").assign(8);
		}
		val = hdr_rec.at("NAXIS").value();
		if ( val.strcmp("2") != 0 ) {
		    hdr_rec.at("NAXIS").assign(2);
		}

		/* mark */
		set_ok = true;

	    }
	}

	if ( set_ok == true ) {

	  long long llv;
	  double dv;
	  const char *sv;
	  size_t endpos;
	  insert_point = n_top_recs;

	  /* Insert EXTNAME, EXTVER and EXTLEVEL records */

	  if ( this->extname_rec.cstr() != NULL ) {

	    fits::header_def def_extname = {"EXTNAME","",""};
	    long idx;

	    idx = hdr_rec.index("EXTNAME");
	    if ( idx < 0 ) {
		hdr_rec.insert(insert_point, def_extname);
		insert_point ++;
		hdr_rec.at("EXTNAME").assign_default_comment();
	    }
	    else insert_point = idx + 1;

	    /* ファイルからの読み取り時の動作にあわせる */
	    sv = hdr_rec.at("EXTNAME").svalue();
	    if ( this->extname_rec.strcmp(sv) != 0 ) {
	        hdr_rec.at("EXTNAME").assign(this->extname_rec.cstr());
	    }

	  }

	  if ( this->extver_rec.cstr() != NULL ) {

	    fits::header_def def_extver = {"EXTVER","",""};
	    long idx;

	    idx = hdr_rec.index("EXTVER");
	    if ( idx < 0 ) {
		hdr_rec.insert(insert_point, def_extver);
		insert_point ++;
		hdr_rec.at("EXTVER").assign_default_comment();
	    }
	    else insert_point = idx + 1;

	    if ( this->extver_rec.length() == 0 ) {
	      if ( hdr_rec.at("EXTVER").svalue_length() != 0 ) {
	        hdr_rec.at("EXTVER").assign(this->extver_rec.cstr());
	      }
	    }
	    else {
	      /* ファイルからの読み取り時の動作にあわせる */
	      sv = hdr_rec.at("EXTVER").svalue();
	      if ( this->extver_rec.strcmp(sv) != 0 ) {
	        llv = this->extver_rec.strtoll(10,&endpos);
	        if ( this->extver_rec.length() == endpos ) {
		  hdr_rec.at("EXTVER").assign(llv);
	        }
	        else {
		  dv = this->extver_rec.strtod(&endpos);
		  if ( this->extver_rec.length() == endpos ) {
		    hdr_rec.at("EXTVER").assign(dv);
		  }
		  else {
		    hdr_rec.at("EXTVER").assign(this->extver_rec.cstr());
		  }
		}
	      }
	    }

	  }

	  if ( this->extlevel_rec.cstr() != NULL ) {

	    fits::header_def def_extlevel = {"EXTLEVEL","",""};
	    long idx;

	    idx = hdr_rec.index("EXTLEVEL");
	    if ( idx < 0 ) {
		hdr_rec.insert(insert_point, def_extlevel);
		insert_point ++;
		hdr_rec.at("EXTLEVEL").assign_default_comment();
	    }
	    else insert_point = idx + 1;

	    if ( this->extlevel_rec.length() == 0 ) {
	      if ( hdr_rec.at("EXTLEVEL").svalue_length() != 0 ) {
	        hdr_rec.at("EXTLEVEL").assign(this->extlevel_rec.cstr());
	      }
	    }
	    else {
	      /* ファイルからの読み取り時の動作にあわせる */
 	      sv = hdr_rec.at("EXTLEVEL").svalue();
	      if ( this->extlevel_rec.strcmp(sv) != 0 ) {
	        llv = this->extlevel_rec.strtoll(10,&endpos);
	        if ( this->extlevel_rec.length() == endpos ) {
		  hdr_rec.at("EXTLEVEL").assign(llv);
	        }
	        else {
		  dv = this->extlevel_rec.strtod(&endpos);
		  if ( this->extlevel_rec.length() == endpos ) {
		    hdr_rec.at("EXTLEVEL").assign(dv);
		  }
		  else {
		    hdr_rec.at("EXTLEVEL").assign(this->extlevel_rec.cstr());
		  }
		}
	      }
	    }

	  }

	}	/* set_ok == true */

    }
    catch (...) {
	/* turn prohibition on */
	hdr_rec.set_sysrecords_prohibition(true);
	err_throw(__FUNCTION__,"FATAL","caught exception");
    }

    /* turn prohibition on */
    //err_report(__FUNCTION__,"DEBUG","end: set to true");
    hdr_rec.set_sysrecords_prohibition(true);

    return *this;
}

/**
 * @brief  Data Unit に関連するヘッダレコードの編集禁止フラグをセット
 *
 *  true をセットすると，システム予約キーワードは登録不可になる．
 *
 * @note   継承クラスでシステムキーワードを書き込む場合に使用．<br>
 *         このメンバ関数は protected です．
 */
fits_hdu &fits_hdu::header_set_sysrecords_prohibition( bool flag )
{
    //err_report1(__FUNCTION__,"DEBUG","called !! [%d]",(int)flag);
    this->header_rec.set_sysrecords_prohibition(flag);
    return *this;
}

/**
 * @brief  指定されたヘッダレコードオブジェクトのアドレスを返す
 *
 * @note   ユーザの拡張クラスで使用を想定．ポインタを張る場合に必要．<br>
 *         このメンバ関数は protected です．
 */
fits_header_record *fits_hdu::header_record_ptr( long index )
{
    fits_header *header_ptr;
    header_ptr = &this->header_rec;
    return header_ptr->get_ptr(index);
}

/**
 * @brief  指定されたヘッダレコードの保護に関する設定
 *
 * @note   ポインタを張った時，消えないように保護するために必要．<br>
 *         このメンバ関数は protected です．
 */
fits_hdu &fits_hdu::set_header_protections( long index0, 
					    bool keyword, bool value_type,
					    bool value, bool comment )
{
    fits_header *header_ptr;
    header_ptr = &this->header_rec;
    header_ptr->set_protections( index0, keyword, value_type, 
				 value, comment );
    return *this;
}

/**
 * @brief  shallow copy を許可する場合に使用
 *
 * @note   一時オブジェクトの return の直前で使用する．
 */
/* 注意: 参照を返したいところだが，return 文でそれをやるとインスタンス */
/*       を2回コピーされるというおかしな事になるので void になっている */
void fits_hdu::set_scopy_flag()
{
    this->header_rec.set_scopy_flag();
    return;
}

/*
 * private member functions 
 */

/**
 * @brief  Data Unit に関連するヘッダレコードの先頭部分をセットアップ
 *
 * @note   sysrecords_prohibition は false にしてから使う事．<br>
 *         このメンバ関数は private です．
 */
int fits_hdu::header_setup_top_records( fits::header_def defs[],
					const char *removed_keywords[] )
{
    int status = -1;
    long i;

    /* 消去すべきものがあれば消去する */
    if ( removed_keywords != NULL ) {
	for ( i=0 ; removed_keywords[i] != NULL ; i++ ) {
	    long idx = this->header_rec.index(removed_keywords[i]);
	    if ( 0 <= idx ) {
		this->header_rec.erase_records(idx,1);
	    }
	}
    }

    for ( i=0 ; defs[i].keyword != NULL ; i++ ) {
	long idx;
	fits_header_record rec_src;

	idx = this->header_rec.index(defs[i].keyword);

	/* 存在している場合，元のレコードをそのまま使う */
	if ( 0 <= idx ) {
	    if ( idx != i ) {
	        rec_src = this->header_rec.at_cs(idx);
		this->header_rec.erase_records(idx, 1);
		this->header_rec.insert(i, rec_src);
	    }
	}
	else {
	    rec_src.assign(defs[i]);
	    if ( rec_src.status() == FITS::NORMAL_RECORD &&
	         rec_src.comment_length() == 0 ) {
		rec_src.assign_default_comment(this->hdutype_rec);
	    }
	    this->header_rec.insert(i, rec_src);
	}

    }
    status = 0;

    return status;
}

/* Update: 2011/11/23 */
/**
 * @brief  ストリームから FITS ヘッダを読み込み，ヘッダオブジェクトを構成
 *
 * @param   sref 読み込み対象のストリーム (cstreamioの継承クラス)
 * @param   max_bytes_ptr 読み込まれるべき最大のバイト数．設定不要時はNULL．
 * @return  非負値: 読み込んだバイト数<br>
 *          負値: エラー
 * @note    次の HDU があるかないかをチェックする場合にも使われる．<br>
 *          このメンバ関数は private です．
 */
ssize_t fits_hdu::header_load( cstreamio &sref, const size_t *max_bytes_ptr )
{
    ssize_t return_len_read = -1;
    size_t len_read_all = 0;
    fits_header tmp_header;

    /* initialize; erase all header records */
    this->init();

    /* read stream */
    if ( max_bytes_ptr == NULL )
	len_read_all = tmp_header.read_stream(sref);
    else
	len_read_all = tmp_header.read_stream(sref,*max_bytes_ptr);

    if ( len_read_all < 0 ) {
	err_report(__FUNCTION__,"ERROR", "tmp_header.read_stream() failed.");
	return_len_read = len_read_all;
	goto quit;
    }

    /* final setup */
    if ( this->header_load( tmp_header ) < 0 ) {
	err_report(__FUNCTION__,"WARNING", 
		   "this->header_load(tmp_header) failed.");
	goto quit;
    }

    return_len_read = len_read_all;
 quit:
    return return_len_read;
}

/* Update: 2011/11/23 */
/**
 * @brief  全ヘッダレコードを含んだ文字列を読み込み，ヘッダオブジェクトを構成
 *
 * @param  header_all ヘッダの内容 ('\0' で終端した文字列)
 * @note   このメンバ関数は private です．
 */
ssize_t fits_hdu::header_load( const char *header_all )
{
    ssize_t return_len_read = -1;
    size_t len_read_all = 0;
    fits_header tmp_header;

    /* initialize; erase all header records */
    this->init();

    /* read stream */
    len_read_all = tmp_header.read_buffer(header_all);

    if ( len_read_all < 0 ) {
	err_report(__FUNCTION__,"ERROR", "tmp_header.read_buffer() failed.");
	return_len_read = len_read_all;
	goto quit;
    }

    /* final setup */
    if ( this->header_load( tmp_header ) < 0 ) {
	err_report(__FUNCTION__,"ERROR",
		   "this->header_load(tmp_header) failed.");
	goto quit;
    }

    return_len_read = len_read_all;
 quit:
    return return_len_read;
}

/* Update: 2011/11/28 */
/**
 * @brief  fits_header オブジェクトを読み込み，ヘッダオブジェクトを構成
 *
 * @note   fitscc クラスでのテンプレートファイル読み込みサポートのために
 *         fits_hdu::header_load(cstreamio &,const size_t *) から分割．<br>
 *         このメンバ関数は private です．
 */
ssize_t fits_hdu::header_load( const fits_header &header_all )
{
    ssize_t return_status = -1;
    long i;

    /*
     * initialize; erase all header records
     */
    this->init();

    /*
     * Copy all header records in header_all into header_rec
     */
    /* turn prohibition off */
    this->header_rec.set_sysrecords_prohibition(false);
    try {
	/* copy all header contents */
	this->header_rec.init(header_all);
    }
    catch (...) {
	/* turn prohibition on */
	this->header_rec.set_sysrecords_prohibition(true);
	err_throw(__FUNCTION__,"FATAL","caught exception");
    }
    /* turn prohibition on */
    this->header_rec.set_sysrecords_prohibition(true);

#if 0
    for ( i=0 ; i < this->header_rec.length() ; i++ ) {
	err_report1(__FUNCTION__,"DEBUG","keyword = [%s]",
		   this->header_rec.record(i).keyword());
	err_report1(__FUNCTION__,"DEBUG","value = [%s]",
		   this->header_rec.record(i).value());
    }
#endif

    /*
     * Test 'SIMPLE' and 'XTENSION' values
     */

    if ( 0 < header_all.length() ) {

	if ( this->header_rec.index("SIMPLE") == 0 ) {
	    //fprintf(stderr,"debug: found SIMPLE\n");
	    //this->is_primary_rec = true;
	    //this->hdutype_rec = FITS::IMAGE_HDU;

#if 0
	    if ( this->extname_rec.assign("Primary") < 0 ) {
		err_report(__FUNCTION__,"ERROR",
			   "this->extname_rec.assign() failed");
		goto quit;
	    }
#endif

	}
	else if ( this->header_rec.index("XTENSION") == 0 ) {
	    const tstring &xtension = this->header_rec.record((long)0).svalue_cs();
	    
	    //this->is_primary_rec = false;
	    if ( xtension.strcmp("IMAGE") == 0 ) {
		//this->hdutype_rec = FITS::IMAGE_HDU;
	    }
	    else if ( xtension.strcmp("BINTABLE") == 0 ) {
		//this->hdutype_rec = FITS::BINARY_TABLE_HDU;
	    }
	    else if ( xtension.strcmp("TABLE") == 0 ) {
		//this->hdutype_rec = FITS::ASCII_TABLE_HDU;
	    }
	    else {
		err_report1(__FUNCTION__,"WARNING",
			    "unsupported HDU type: %s",xtension.cstr());
	    }
	}
	else {
	    err_report(__FUNCTION__,"WARNING",
		       "unsupported HDU type or invalid padding before EOF.");
	    goto quit;
	}

	/* Store the values and status of EXTNAME and EXTVER */

	i = this->header_rec.index("EXTNAME");
	if ( 0 <= i ) {
	    if ( 0 < this->header_rec.record(i).value_length() ) 
		this->extname_rec = this->header_rec.record(i).svalue();
	    else
		this->extname_rec = NULL;
	}
	else {
	    this->extname_rec = NULL;
	}
	
	i = this->header_rec.index("EXTVER");
	if ( 0 <= i ) {
	    if ( 0 < this->header_rec.record(i).value_length() ) 
		this->extver_rec = this->header_rec.record(i).svalue();
	    else
		this->extver_rec = NULL;
	}
	else {
	    this->extver_rec = NULL;
	}

	i = this->header_rec.index("EXTLEVEL");
	if ( 0 <= i ) {
	    if ( 0 < this->header_rec.record(i).value_length() ) 
		this->extlevel_rec = this->header_rec.record(i).svalue();
	    else
		this->extlevel_rec = NULL;
	}
	else {
	    this->extlevel_rec = NULL;
	}
	
    }		/* if ( 0 < header_all.length() ) ... */

    return_status = 0;
 quit:
    return return_status;
}

#include "private/initialize_csum.cc"

}	/* namespace sli */

