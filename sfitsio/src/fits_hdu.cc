/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2015-11-20 14:31:27 cyamauch> */

/**
 * @file   fits_hdu.cc
 * @brief  FITS �� HDU ��ɽ��������쥯�饹 fits_hdu �Υ�����
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
 * @brief  ���󥹥ȥ饯��
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
 * @brief  ���ԡ����󥹥ȥ饯��
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
 * @brief  �ǥ��ȥ饯��
 */
fits_hdu::~fits_hdu()
{
    return;
}

/**
 * @brief  ���֥������ȤΥ��ԡ�
 */
fits_hdu &fits_hdu::operator=(const fits_hdu &obj)
{
    return this->init(obj);
}

/**
 * @brief  ���֥������Ȥν����
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

    /* �إå��������õ�� */
    this->header_rec.init();

    this->checksum_error_rec = false;
    this->datasum_error_rec = false;

    return *this;
}

/**
 * @brief  ���֥������ȤΥ��ԡ�
 */
fits_hdu &fits_hdu::init(const fits_hdu &obj)
{
    if ( &obj == this ) return *this;

    //err_report("init()","DEBUG","fits_hdu::init(const fits_hdu &obj) ...");
    this->fits_hdu::init();

    try {
	//this->hdutype_rec = obj.hdutype_rec;	/* ���Ѿ����饹�Ǥ�� */
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
 * @brief  2�ĤΥ��֥������ȴ֤Ǥ����ƤΥ���å�
 *
 *  ���ꤵ�줿���֥�������sobj �����Ƥȼ��Ȥ����Ƥ������ؤ��ޤ���
 *  �إå������ơ�°�������٤Ƥξ��֤������ؤ��ޤ���
 *
 * @param   sobj fits_hdu ���饹�Υ��֥�������
 * @return  ���Ȥλ���    
 * @note    ���Υ��дؿ��� protected �Ǥ���
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
 * @brief  HDU �μ���(FITS::IMAGE_HDU��FITS::BINARY_TABLE_HDU ��)�����
 */
int fits_hdu::hdutype() const
{
    return this->hdutype_rec;
}

/**
 * @brief  ���饹�ηѾ���٥�����
 */
int fits_hdu::classlevel() const
{
    return this->classlevel_rec;
}

/*
 * �桼���Υإå��ޤ�� 
 */

/* ���Τ��Խ� */

/**
 * @brief  FITS�إå��ν����
 */
fits_hdu &fits_hdu::header_init()
{
    this->header_rec.init();
    return *this;
}

/**
 * @brief  FITS�إå��Υ��ԡ� (���� fits_hedaer ���֥������Ȥ򥳥ԡ�)
 *
 * @note   Data Unit �˴ط�����ͽ�󥭡����(BITPIX ��)����ĥ쥳���ɤ�
 *         ���ԡ�����ޤ���
 */
fits_hdu &fits_hdu::header_init( const fits_header &obj )
{
    this->header_rec.init(obj);
    return *this;
}

/**
 * @brief  FITS�إå��ν���� (fits::header_def ��¤�Τǻ���)
 *
 * @note   Data Unit �˴ط�����ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_hdu &fits_hdu::header_init( const fits::header_def defs[] )
{
    this->header_rec.init(defs);
    return *this;
}

/**
 * @brief  fits_header���֥������ȴ֤Ǥ����ƤΥ���å�
 *
 *  ͽ�󥭡���ɰʳ��ˤĤ��ơ��إå������Ƥ������ؤ��ޤ���
 *
 * @note   Data Unit �˴ط�����ͽ�󥭡����(BITPIX ��)����ĥ쥳���ɤ�
 *         �����ؤ��������Ԥʤ��ޤ���
 */
fits_hdu &fits_hdu::header_swap( fits_header &obj )
{
    this->header_rec.swap(obj);
    return *this;
}

/**
 * @brief  ʣ���Υإå��쥳���ɤ��ɲá�����
 *
 *  ������ɤ�¸�ߤ��ʤ������ɲä���Ʊ��Υ�����ɤ�¸�ߤ������
 *  ���ꤵ�줿���ƤǾ�񤭤��ޤ���
 *
 * @param  obj ������
 * @note   Data Unit �˴ط�����ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_hdu &fits_hdu::header_update_records( const fits_header &obj )
{
    this->header_rec.update_records( obj );
    return *this;
}

/**
 * @brief  ʣ���Υإå��쥳���ɤ��ɲ� (fits::header_def ��¤�Τǻ���)
 *
 * @note   Data Unit �˴ط�����ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_hdu &fits_hdu::header_append_records( const fits::header_def defs[] )
{
    this->header_rec.append_records(defs);
    return *this;
}

/**
 * @brief  ʣ���Υإå��쥳���ɤ��ɲ� (fits::header_def ��¤�Τǻ���)
 *
 * @note   Data Unit �˴ط�����ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_hdu &fits_hdu::header_append_records( const fits::header_def defs[],
					   long num_defs, bool warn )
{
    this->header_rec.append_records(defs, num_defs, warn);
    return *this;
}

/**
 * @brief  ʣ���Υإå��쥳���ɤ��ɲ� (fits_header ���֥������Ȥǻ���)
 *
 * @note   Data Unit �˴ط�����ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_hdu &fits_hdu::header_append_records( const fits_header &obj, bool warn )
{
    this->header_rec.append_records(obj, warn);
    return *this;
}

/**
 * @brief  ʣ���Υإå��쥳���ɤ����� (fits::header_def ��¤�Τǻ���)
 *
 * @note   Data Unit �˴ط�����ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_hdu &fits_hdu::header_insert_records( long index0,
					   const fits::header_def defs[] )
{
    this->header_rec.insert_records(index0,defs);
    return *this;
}

/**
 * @brief  ʣ���Υإå��쥳���ɤ����� (fits::header_def ��¤�Τǻ���)
 *
 * @note   Data Unit �˴ط�����ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_hdu &fits_hdu::header_insert_records( const char *keyword0,
					   const fits::header_def defs[] )
{
    this->header_rec.insert_records(keyword0,defs);
    return *this;
}

/**
 * @brief  ʣ���Υإå��쥳���ɤ����� (fits::header_def ��¤�Τǻ���)
 *
 * @note   Data Unit �˴ط�����ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_hdu &fits_hdu::header_insert_records( long index0,
					   const fits::header_def defs[],
					   long num_defs, bool warn )
{
    this->header_rec.insert_records(index0,defs,num_defs,warn);
    return *this;
}

/**
 * @brief  ʣ���Υإå��쥳���ɤ����� (fits::header_def ��¤�Τǻ���)
 *
 * @note   Data Unit �˴ط�����ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_hdu &fits_hdu::header_insert_records( const char *keyword0,
					   const fits::header_def defs[],
					   long num_defs, bool warn )
{
    this->header_rec.insert_records(keyword0,defs,num_defs,warn);
    return *this;
}

/**
 * @brief  ʣ���Υإå��쥳���ɤ����� (fits_header ���֥������Ȥǻ���)
 *
 * @note   Data Unit �˴ط�����ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_hdu &fits_hdu::header_insert_records( long index0,
					   const fits_header &obj, bool warn )
{
    this->header_rec.insert_records(index0,obj,warn);
    return *this;
}

/**
 * @brief  ʣ���Υإå��쥳���ɤ����� (fits_header ���֥������Ȥǻ���)
 *
 * @note   Data Unit �˴ط�����ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_hdu &fits_hdu::header_insert_records( const char *keyword0,
					   const fits_header &obj, bool warn )
{
    this->header_rec.insert_records(keyword0,obj,warn);
    return *this;
}

/**
 * @brief  ʣ���Υإå��쥳���ɤκ��
 *
 * @note   Data Unit �˴ط�����ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_hdu &fits_hdu::header_erase_records( long index0, long num_records )
{
    this->header_rec.erase_records(index0,num_records);
    return *this;
}

/**
 * @brief  ʣ���Υإå��쥳���ɤκ��
 *
 * @note   Data Unit �˴ط�����ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_hdu &fits_hdu::header_erase_records( const char *keyword0, long num_records )
{
    this->header_rec.erase_records(keyword0,num_records);
    return *this;
}

/* 1�ԤǤ��ɲäȤ� */

/**
 * @brief  1�ĤΥإå��쥳���ɤ��ɲá�����
 *
 *  ������ɤ�¸�ߤ��ʤ������ɲä���Ʊ��Υ�����ɤ�¸�ߤ������
 *  ���ꤵ�줿���ƤǾ�񤭤��ޤ���
 *
 * @note   Data Unit �˴ط�����ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_hdu &fits_hdu::header_update( const char *keyword, const char *value, 
				   const char *comment )
{
    this->header_rec.update(keyword,value,comment);
    return *this;
}

/**
 * @brief  1�ĤΥإå��쥳���ɤ��ɲ�,����(fits_header_record���֥������Ȥǻ���)
 *
 *  ������ɤ�¸�ߤ��ʤ������ɲä���Ʊ��Υ�����ɤ�¸�ߤ������
 *  ���ꤵ�줿���ƤǾ�񤭤��ޤ���
 *
 * @note   Data Unit �˴ط�����ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_hdu &fits_hdu::header_update( const fits_header_record &obj )
{
    this->header_rec.update( obj );
    return *this;
}

/**
 * @brief  1�ĤΥإå��쥳���ɤ��ɲ� (������ɤΤ߻���)
 *
 * @note   Data Unit �˴ط�����ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_hdu &fits_hdu::header_append( const char *keyword )
{
    this->header_rec.append(keyword);
    return *this;
}

/**
 * @brief  1�ĤΥإå��쥳���ɤ��ɲ�
 *
 * @note   Data Unit �˴ط�����ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_hdu &fits_hdu::header_append( const char *keyword, const char *value, 
				   const char *comment )
{
    this->header_rec.append(keyword,value,comment);
    return *this;
}

/**
 * @brief  1�Ĥε��Ҽ��إå��쥳����(HISTORY��COMMENT)���ɲ�
 */
fits_hdu &fits_hdu::header_append( const char *keyword, const char *description )
{
    this->header_rec.append(keyword,description);
    return *this;
}

/**
 * @brief  1�ĤΥإå��쥳���ɤ��ɲ� (fits::header_def ��¤�Τǻ���)
 *
 * @note   Data Unit �˴ط�����ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_hdu &fits_hdu::header_append( const fits::header_def &def )
{
    this->header_rec.append(def);
    return *this;
}

/**
 * @brief  1�ĤΥإå��쥳���ɤ��ɲ� (fits_header_record ���֥������Ȥǻ���)
 *
 * @note   Data Unit �˴ط�����ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_hdu &fits_hdu::header_append( const fits_header_record &obj )
{
    this->header_rec.append(obj);
    return *this;
}

/**
 * @brief  1�ĤΥإå��쥳���ɤ����� (������ɤΤ߻���)
 *
 * @note   Data Unit �˴ط�����ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_hdu &fits_hdu::header_insert( long index0, const char *kwd )
{
    this->header_rec.insert(index0, kwd);
    return *this;
}

/**
 * @brief  1�ĤΥإå��쥳���ɤ����� (������ɤΤ߻���)
 *
 * @note   Data Unit �˴ط�����ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_hdu &fits_hdu::header_insert( const char *keyword0, const char *kwd )
{
    this->header_rec.insert(keyword0, kwd);
    return *this;
}

/**
 * @brief  1�ĤΥإå��쥳���ɤ�����
 *
 * @note   Data Unit �˴ط�����ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_hdu &fits_hdu::header_insert( long index0,
				   const char *k, const char *v, const char *c )
{
    this->header_rec.insert(index0,k,v,c);
    return *this;
}

/**
 * @brief  1�ĤΥإå��쥳���ɤ�����
 *
 * @note   Data Unit �˴ط�����ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_hdu &fits_hdu::header_insert( const char *keyword0,
				   const char *k, const char *v, const char *c )
{
    this->header_rec.insert(keyword0,k,v,c);
    return *this;
}

/**
 * @brief  1�Ĥε��Ҽ��إå��쥳����(HISTORY��COMMENT)������
 *
 * @note   Data Unit �˴ط�����ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_hdu &fits_hdu::header_insert( long index0, 
				   const char *keywd, const char *description )
{
    this->header_rec.insert(index0,keywd,description);
    return *this;
}

/**
 * @brief  1�Ĥε��Ҽ��إå��쥳����(HISTORY��COMMENT)������
 *
 * @note   Data Unit �˴ط�����ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_hdu &fits_hdu::header_insert( const char *keyword0, 
				   const char *keywd, const char *description )
{
    this->header_rec.insert(keyword0,keywd,description);
    return *this;
}

/**
 * @brief  1�ĤΥإå��쥳���ɤ����� (fits::header_def ��¤�Τǻ���)
 *
 * @note   Data Unit �˴ط�����ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_hdu &fits_hdu::header_insert( long index0,
				   const fits::header_def &def )
{
    this->header_rec.insert(index0,def);
    return *this;
}

/**
 * @brief  1�ĤΥإå��쥳���ɤ����� (fits::header_def ��¤�Τǻ���)
 *
 * @note   Data Unit �˴ط�����ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_hdu &fits_hdu::header_insert( const char *keyword0,
				   const fits::header_def &def )
{
    this->header_rec.insert(keyword0,def);
    return *this;
}

/**
 * @brief  1�ĤΥإå��쥳���ɤ����� (fits_header_record ���֥������Ȥǻ���)
 *
 * @note   Data Unit �˴ط�����ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_hdu &fits_hdu::header_insert( long index0,
				   const fits_header_record &obj )
{
    this->header_rec.insert(index0,obj);
    return *this;
}

/**
 * @brief  1�ĤΥإå��쥳���ɤ����� (fits_header_record ���֥������Ȥǻ���)
 *
 * @note   Data Unit �˴ط�����ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_hdu &fits_hdu::header_insert( const char *keyword0,
				   const fits_header_record &obj )
{
    this->header_rec.insert(keyword0,obj);
    return *this;
}

/**
 * @brief  1�ĤΥإå��������̾���ѹ�
 *
 * @note   Data Unit �˴ط�����ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_hdu &fits_hdu::header_rename( long index0, const char *new_name )
{
    this->header_rec.rename(index0, new_name);
    return *this;
}

/**
 * @brief  1�ĤΥإå��������̾���ѹ�
 *
 * @note   Data Unit �˴ط�����ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_hdu &fits_hdu::header_rename( const char *keyword0, const char *new_name )
{
    this->header_rec.rename(keyword0, new_name);
    return *this;
}

/**
 * @brief  1�ĤΥإå��쥳���ɤξõ�
 *
 * @note   Data Unit �˴ط�����ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_hdu &fits_hdu::header_erase( long index0 )
{
    this->header_rec.erase(index0);
    return *this;
}

/**
 * @brief  1�ĤΥإå��쥳���ɤξõ�
 *
 * @note   Data Unit �˴ط�����ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_hdu &fits_hdu::header_erase( const char *keyword0 )
{
    this->header_rec.erase(keyword0);
    return *this;
}

/* low-level member functions */

/**
 * @brief  1�ĤΥإå��쥳���ɤ򹹿� (fits::header_def ��¤�Τǻ���)
 *
 * @note   Data Unit �˴ط�����ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_hdu &fits_hdu::header_assign( long index0, const fits::header_def &def )
{
    this->header_rec.assign(index0,def);
    return *this;
}

/**
 * @brief  1�ĤΥإå��쥳���ɤ򹹿� (fits::header_def ��¤�Τǻ���)
 *
 * @note   Data Unit �˴ط�����ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_hdu &fits_hdu::header_assign( const char *keyword0, 
				   const fits::header_def &def )
{
    this->header_rec.assign(keyword0,def);
    return *this;
}

/**
 * @brief  1�ĤΥإå��쥳���ɤ򹹿� (fits_header_record ���֥������Ȥǻ���)
 *
 * @note   Data Unit �˴ط�����ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_hdu &fits_hdu::header_assign( long index0, const fits_header_record &obj )
{
    this->header_rec.assign(index0,obj);
    return *this;
}

/**
 * @brief  1�ĤΥإå��쥳���ɤ򹹿� (fits_header_record ���֥������Ȥǻ���)
 *
 * @note   Data Unit �˴ط�����ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_hdu &fits_hdu::header_assign( const char *keyword0, 
				   const fits_header_record &obj )
{
    this->header_rec.assign(keyword0,obj);
    return *this;
}

/**
 * @brief  1�ĤΥإå��쥳���ɤ򹹿�
 *
 * @note   Data Unit �˴ط�����ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_hdu &fits_hdu::header_assign( long index0, 
				   const char *keyword, const char *value, 
				   const char *comment )
{
    this->header_rec.assign(index0,keyword,value,comment);
    return *this;
}

/**
 * @brief  1�ĤΥإå��쥳���ɤ򹹿�
 *
 * @note   Data Unit �˴ط�����ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_hdu &fits_hdu::header_assign( const char *keyword0, 
				   const char *keyword, const char *value, 
				   const char *comment )
{
    this->header_rec.assign(keyword0,keyword,value,comment);
    return *this;
}

/**
 * @brief  1�ĤΥإå��쥳���ɤ򵭽Ҽ��쥳���ɤǹ���
 *
 * @note   Data Unit �˴ط�����ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_hdu &fits_hdu::header_assign( long index0, 
				 const char *keyword, const char *description )
{
    this->header_rec.assign(index0,keyword,description);
    return *this;
}

/**
 * @brief  1�ĤΥإå��쥳���ɤ򵭽Ҽ��쥳���ɤǹ���
 *
 * @note   Data Unit �˴ط�����ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_hdu &fits_hdu::header_assign( const char *keyword0, 
				 const char *keyword, const char *description )
{
    this->header_rec.assign(keyword0,keyword,description);
    return *this;
}

/**
 * @brief  1�ĤΥإå��쥳���ɤ��ͤ򹹿� (���٥롦printf()�ε�ˡ)
 *
 * @note   Data Unit �˴ط�����ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_hdu &fits_hdu::header_vassignf_value( long index0, 
					   const char *format, va_list ap )
{
    this->header_rec.vassignf_value(index0,format,ap);
    return *this;
}

/**
 * @brief  1�ĤΥإå��쥳���ɤ��ͤ򹹿� (���٥롦printf()�ε�ˡ)
 *
 * @note   Data Unit �˴ط�����ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_hdu &fits_hdu::header_vassignf_value( const char *keyword0, 
					   const char *format, va_list ap )
{
    this->header_rec.vassignf_value(keyword0,format,ap);
    return *this;
}

/**
 * @brief  1�ĤΥإå��쥳���ɤ��ͤ򹹿� (���٥롦printf()�ε�ˡ)
 *
 * @note   Data Unit �˴ط�����ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
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
 * @brief  1�ĤΥإå��쥳���ɤ��ͤ򹹿� (���٥롦printf()�ε�ˡ)
 *
 * @note   Data Unit �˴ط�����ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
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
 * @brief  1�ĤΥإå��쥳���ɤΥ����Ȥ򹹿� (printf()�ε�ˡ)
 *
 * @note   Data Unit �˴ط�����ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
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
 * @brief  1�ĤΥإå��쥳���ɤΥ����Ȥ򹹿� (printf()�ε�ˡ)
 *
 * @note   Data Unit �˴ط�����ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_hdu &fits_hdu::header_vassignf_comment( const char *keyword0, 
					     const char *format, va_list ap )
{
    return this->header_vassignf_comment(this->header_index(keyword0),
					 format,ap);
}

/**
 * @brief  1�ĤΥإå��쥳���ɤΥ����Ȥ򹹿� (printf()�ε�ˡ)
 *
 * @note   Data Unit �˴ط�����ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
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
 * @brief  1�ĤΥإå��쥳���ɤΥ����Ȥ򹹿� (printf()�ε�ˡ)
 *
 * @note   Data Unit �˴ط�����ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
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
 * @brief  �����Ȥ�¸�ߤˤ�����餺�����ߤΥإå������ȼ�������Ƥ�����
 *
 *  ���ƤΥإå��쥳���ɤΤ��������ߤΥ����ȼ���ȥ�����ɤ����פ����硤
 *  �����Υ쥳���ɤΥ����Ȥ򸽺ߤΥ����ȼ��񤬻��ĥ�����ʸ�Ǿ�񤭤�
 *  �ޤ���
 */
fits_hdu &fits_hdu::header_assign_default_comments( int hdutype )
{
    this->header_rec.assign_default_comments(hdutype);
    return *this;
}

/* Fill all blank comments with SFITSIO built-in comment strings  */
/**
 * @brief  �����Ȥ�¸�ߤ��ʤ���硤���ߤΥإå������ȼ�������Ƥ�����
 *
 *  ���ƤΥإå��쥳���ɤΥ����Ȥ�����ʤ�ΤˤĤ��ơ����ߤΥ����ȼ����
 *  ������ɤ����פ����硤�����Υ쥳���ɤ˸��ߤΥ����ȼ��񤬻���
 *  ������ʸ�򥻥åȤ��ޤ���
 */
fits_hdu &fits_hdu::header_fill_blank_comments( int hdutype )
{
    this->header_rec.fill_blank_comments(hdutype);
    return *this;
}

/**
 * @brief  fits_header ���֥������Ȥؤλ��Ȥ����
 *
 *  fits_header ���饹�� FITS �إå����Τ�ɽ�����Ƥ��ޤ���
 *
 *  ��:  v = fits.image(0L).header().at("TELESCOP").dvalue();
 */
fits_header &fits_hdu::header()
{
    return this->header_rec;
}

#ifdef SLI__OVERLOAD_CONST_AT
/**
 * @brief  fits_header ���֥������Ȥؤλ��Ȥ���� (�ɼ�����)
 *
 *  fits_header ���饹�� FITS �إå����Τ�ɽ�����Ƥ��ޤ���
 *
 *  ��:  v = fits.image(0L).header().at("TELESCOP").dvalue();
 */
const fits_header &fits_hdu::header() const
{
    return this->header_rec;
}
#endif

/**
 * @brief  fits_header ���֥������Ȥؤλ��Ȥ���� (�ɼ�����)
 *
 *  fits_header ���饹�� FITS �إå����Τ�ɽ�����Ƥ��ޤ���
 *
 *  ��:  v = fits.image(0L).header_cs().at("TELESCOP").dvalue();
 */
const fits_header &fits_hdu::header_cs() const
{
    return this->header_rec;
}

/**
 * @brief  fits_header_record ���֥������Ȥؤλ��Ȥ����
 *
 *  fits_header_record ���饹�� 1�Ĥ� FITS �إå��쥳���ɤ�ɽ�����Ƥ��ޤ���
 *
 *  ��:  v = fits.image(0L).header(i).dvalue();
 */
fits_header_record &fits_hdu::header( long index0 )
{
    if ( index0 < 0 || this->header_length() <= index0 ) {
	err_throw(__FUNCTION__,"ERROR","Invalid header index");
    }
    return this->header_rec.record(index0);
}

/**
 * @brief  fits_header_record ���֥������Ȥؤλ��Ȥ����
 *
 *  fits_header_record ���饹�� 1�Ĥ� FITS �إå��쥳���ɤ�ɽ�����Ƥ��ޤ���
 *
 *  ��:  v = fits.image(0L).header("TELESCOP").dvalue();
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
 * @brief  fits_header_record ���֥������Ȥؤλ��Ȥ���� (printf()�ε�ˡ)
 *
 *  fits_header_record ���饹�� 1�Ĥ� FITS �إå��쥳���ɤ�ɽ�����Ƥ��ޤ���
 *
 *  ��:  v = fits.image(0L).headerf("CRVAL%d",i).dvalue();
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
 * @brief  fits_header_record ���֥������Ȥؤλ��Ȥ���� (printf()�ε�ˡ)
 *
 *  fits_header_record ���饹�� 1�Ĥ� FITS �إå��쥳���ɤ�ɽ�����Ƥ��ޤ���
 *
 *  ��:  v = fits.image(0L).headerf("CRVAL%d",i).dvalue();
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
 * @brief  fits_header_record ���֥������Ȥؤλ��Ȥ���� (�ɼ�����)
 *
 *  fits_header_record ���饹�� 1�Ĥ� FITS �إå��쥳���ɤ�ɽ�����Ƥ��ޤ���
 *
 *  ��:  v = fits.image(0L).header(i).dvalue();
 */
const fits_header_record &fits_hdu::header( long index0 ) const
{
    return this->header_cs(index0);
}

/**
 * @brief  fits_header_record ���֥������Ȥؤλ��Ȥ���� (�ɼ�����)
 *
 *  fits_header_record ���饹�� 1�Ĥ� FITS �إå��쥳���ɤ�ɽ�����Ƥ��ޤ���
 *
 *  ��:  v = fits.image(0L).header("TELESCOP").dvalue();
 */
const fits_header_record &fits_hdu::header( const char *keyword0 ) const
{
    return this->header_cs(keyword0);
}

/**
 * @brief  fits_header_record ���֥������Ȥؤλ��Ȥ���� (�ɼ����ѡ�printf()�ε�ˡ)
 *
 *  fits_header_record ���饹�� 1�Ĥ� FITS �إå��쥳���ɤ�ɽ�����Ƥ��ޤ���
 *
 *  ��:  v = fits.image(0L).headerf("CRVAL%d",i).dvalue();
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
 * @brief  fits_header_record ���֥������Ȥؤλ��Ȥ���� (�ɼ����ѡ�printf()�ε�ˡ)
 *
 *  fits_header_record ���饹�� 1�Ĥ� FITS �إå��쥳���ɤ�ɽ�����Ƥ��ޤ���
 *
 *  ��:  v = fits.image(0L).headerf("CRVAL%d",i).dvalue();
 */
const fits_header_record &fits_hdu::vheaderf( const char *fmt, va_list ap ) const
{
    return this->vheaderf_cs(fmt, ap);
}
#endif

/**
 * @brief  fits_header_record ���֥������Ȥؤλ��Ȥ���� (�ɼ�����)
 *
 *  fits_header_record ���饹�� 1�Ĥ� FITS �إå��쥳���ɤ�ɽ�����Ƥ��ޤ���
 *
 *  ��:  v = fits.image(0L).header_cs(i).dvalue();
 */
const fits_header_record &fits_hdu::header_cs( long index0 ) const
{
    if ( index0 < 0 || this->header_length() <= index0 ) {
	err_throw(__FUNCTION__,"ERROR","Invalid header index");
    }
    return this->header_rec.record_cs(index0);
}

/**
 * @brief  fits_header_record ���֥������Ȥؤλ��Ȥ���� (�ɼ�����)
 *
 *  fits_header_record ���饹�� 1�Ĥ� FITS �إå��쥳���ɤ�ɽ�����Ƥ��ޤ���
 *
 *  ��:  v = fits.image(0L).header_cs("TELESCOP").dvalue();
 */
const fits_header_record &fits_hdu::header_cs( const char *keyword0 ) const
{
    long index0 = this->header_index(keyword0);
    return this->header_cs( index0 );
}

/**
 * @brief  fits_header_record ���֥������Ȥؤλ��Ȥ���� (�ɼ����ѡ�printf()�ε�ˡ)
 *
 *  fits_header_record ���饹�� 1�Ĥ� FITS �إå��쥳���ɤ�ɽ�����Ƥ��ޤ���
 *
 *  ��:  v = fits.image(0L).headerf_cs("CRVAL%d",i).dvalue();
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
 * @brief  fits_header_record ���֥������Ȥؤλ��Ȥ���� (�ɼ����ѡ�printf()�ε�ˡ)
 *
 *  fits_header_record ���饹�� 1�Ĥ� FITS �إå��쥳���ɤ�ɽ�����Ƥ��ޤ���
 *
 *  ��:  v = fits.image(0L).headerf_cs("CRVAL%d",i).dvalue();
 */
const fits_header_record &fits_hdu::vheaderf_cs( const char *fmt, va_list ap ) const
{
    tstring buf;

    if ( fmt == NULL ) return this->header_cs(fmt);

    buf.vprintf(fmt,ap);
    return this->header_cs(buf.cstr());
}

/* �إå������ɤ߼�� */

/**
 * @brief  ������ɤ��б�����إå��쥳�����ֹ����� (���Ҽ��쥳���ɤϽ���)
 */
long fits_hdu::header_index( const char *keyword0 ) const
{
    return this->header_rec.index(keyword0);
}

/**
 * @brief  ������ɤ��б�����إå��쥳�����ֹ�����
 */
long fits_hdu::header_index( const char *keyword0, bool is_description ) const
{
    return this->header_rec.index(keyword0,is_description);
}

/**
 * @brief  POSIX��ĥ����ɽ���ǥ�����ɤ򸡺�
 */
long fits_hdu::header_regmatch( const char *keypat, 
				ssize_t *rpos, size_t *rlen )
{
    return this->header_rec.regmatch(keypat,rpos,rlen);
}

/**
 * @brief  POSIX��ĥ����ɽ���ǥ�����ɤ�Ϣ³Ū�˸���
 */
long fits_hdu::header_regmatch( long index0, const char *keypat, 
				ssize_t *rpos, size_t *rlen )
{
    return this->header_rec.regmatch(index0,keypat,rpos,rlen);
}

/* returns length of raw value. More than 0 means NON-NULL. */
/* Negative value is returned when a record is not found.   */

/**
 * @brief  ������ɤ��б���������ʸ�����ͤ�Ĺ������� (¸�ߥ����å��˻��Ѳ�)
 *
 * @param   keyword �إå��������
 * @return  ������: ����ʸ������(��'�פ�ޤ�)��Ĺ�� <br>
 *          0: �ͤ�¸�ߤ��ʤ���� <br>
 *          �����: ������ɤ�¸�ߤ��ʤ����
 */
long fits_hdu::header_value_length( const char *keyword ) const
{
    return this->header_rec.value_length(keyword);
}

/**
 * @brief  ������ɤ��б���������ʸ�����ͤ�Ĺ������� (¸�ߥ����å��˻��Ѳ�)
 *
 * @param   index �쥳�����ֹ�
 * @return  ������: ����ʸ������(��'�פ�ޤ�)��Ĺ�� <br>
 *          0: �ͤ�¸�ߤ��ʤ���� <br>
 *          �����: ������ɤ�¸�ߤ��ʤ����
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
 * @brief  �إå��쥳���ɤ�Ĺ�������
 */
long fits_hdu::header_length() const
{
    return this->header_rec.length();
}

/**
 * @brief  �إå��쥳���ɤ�Ĺ�������
 *
 * @note   fits_hdu::header_length() �Ȥΰ㤤�Ϥ���ޤ���
 */
long fits_hdu::header_size() const
{
    return this->header_rec.length();
}

/* discard original 80-char record, and reformat all records */
/**
 * @brief  ���إå��쥳���ɤκƥե����ޥåȤ�Ԥʤ�
 *
 *  �� fits_header_record �������ݻ����Ƥ��� 80ʸ���Υե����ޥåȺѤ�ʸ�����
 *  ���ߤΥ�����ɡ��͡������Ȥ����Ƥ�����ʤ����ޤ���
 */
fits_hdu &fits_hdu::header_reformat()
{
    this->header_rec.reformat();
    return *this;
}

/* ����� fitscc ���饹�������Ѥ��ƤϤ����ʤ� */
/**
 * @brief  ���إå��쥳���ɤΥե����ޥåȺѤ�ʸ����(80��nʸ��)�����
 *
 *  ���֥�����������������줿�����إå��쥳���ɤΥե����ޥåȺѤ�ʸ����
 *  (80��nʸ��)���֤��ޤ�������ʸ���ϴޤޤ줺��ʸ����� '\0' �ǽ�ü���Ƥ��ޤ���
 *  
 * @return  �ե����ޥåȺѤ�ʸ����(80��nʸ��)
 */
const char *fits_hdu::header_formatted_string()
{
    ssize_t len;
    const char *return_value = NULL;

    /* calling overridden member function; setup system header */
    len = -1;
    if ( this->manager == NULL ) {	/* fitscc �δ��������֤���Ƥʤ���� */
	len = this->stream_length();
    }
    else {				/* fitscc �δ��������֤���Ƥ���� */
	long i;
	for ( i=0 ; i < this->manager->length() ; i++ ) {
	    if ( &(this->manager->hdu(i)) == this ) {
		/* fitscc �� stream_length() ��Ƥ� */
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
 * @deprecated  �ߴ����ݤĤ����¸�ߡ��Ȥ�ʤ��Ǥ���������
 */
long fits_hdu::sysheader_length() const
{
    return this->header_rec.length();
}

/**
 * @deprecated  �ߴ����ݤĤ����¸�ߡ��Ȥ�ʤ��Ǥ���������
 */
long fits_hdu::sysheader_size() const
{
    return this->header_rec.length();
}

/**
 * @deprecated  �ߴ����ݤĤ����¸�ߡ��Ȥ�ʤ��Ǥ���������
 */
long fits_hdu::sysheader_index( const char *keyword0 ) const
{
    return this->header_rec.index(keyword0);
}

/**
 * @deprecated  �ߴ����ݤĤ����¸�ߡ��Ȥ�ʤ��Ǥ���������
 */
const char *fits_hdu::sysheader_keyword( long index ) const
{
    return this->header_rec.record_cs(index).keyword();
}

/**
 * @deprecated  �ߴ����ݤĤ����¸�ߡ��Ȥ�ʤ��Ǥ���������
 */
const char *fits_hdu::sysheader_value( long index ) const
{
    return this->header_rec.record_cs(index).value();
}

/**
 * @deprecated  �ߴ����ݤĤ����¸�ߡ��Ȥ�ʤ��Ǥ���������
 */
const char *fits_hdu::sysheader_value( const char *keyword ) const
{
    long idx = this->header_rec.index(keyword);
    if ( idx < 0 ) return NULL;
    else return this->header_rec.record_cs(idx).value();
}

/**
 * @deprecated  �ߴ����ݤĤ����¸�ߡ��Ȥ�ʤ��Ǥ���������
 */
const char *fits_hdu::sysheader_comment( long index ) const
{
    return this->header_rec.record_cs(index).comment();
}

/**
 * @deprecated  �ߴ����ݤĤ����¸�ߡ��Ȥ�ʤ��Ǥ���������
 */
const char *fits_hdu::sysheader_comment( const char *keyword ) const
{
    long idx = this->header_rec.index(keyword);
    if ( idx < 0 ) return NULL;
    return this->header_rec.record_cs(idx).comment();
}

/* ����� fitscc ���饹�������Ѥ��ƤϤ����ʤ� */
/**
 * @deprecated  �ߴ����ݤĤ����¸�ߡ��Ȥ�ʤ��Ǥ���������
 */
const char *fits_hdu::sysheader_formatted_string()
{
    return this->header_formatted_string();
}


/* ����� fitscc ���饹�������Ѥ��ƤϤ����ʤ� */
/**
 * @brief  HDU ��̾��(EXTNAME)������
 */
fits_hdu &fits_hdu::assign_hduname( const char *hduname )
{
    return this->assign_extname(hduname);
}

/* ����� fitscc ���饹�������Ѥ��ƤϤ����ʤ� */
/**
 * @brief  HDU ��̾��(EXTNAME)������
 *
 * @note  fits_hdu::assign_hduname() �Ȥΰ㤤�Ϥ���ޤ���
 */
fits_hdu &fits_hdu::assign_extname( const char *extname )
{
    //err_report(__FUNCTION__,"DEBUG","called!!");
    if ( this->manager == NULL ) {	/* fitscc �δ��������֤���Ƥʤ���� */
	return this->_assign_extname(extname);
    }
    else {				/* fitscc �δ��������֤���Ƥ���� */
	long i;
	for ( i=0 ; i < this->manager->length() ; i++ ) {
	    if ( &(this->manager->hdu(i)) == this ) {
		/* ����Ū�� _assign_extname() ��Ƥ� */
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
 * @brief  HDU �ΥС������(EXTVER)������
 */
fits_hdu &fits_hdu::assign_hduver( long long hduver )
{
    return this->assign_extver(hduver);
}

/**
 * @brief  HDU �ΥС������(EXTVER)������
 *
 * @note  fits_hdu::assign_hduver() �Ȥΰ㤤�Ϥ���ޤ���
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
 * @brief  HDU �Υ�٥�(EXTLEVEL)������
 */
fits_hdu &fits_hdu::assign_hdulevel( long long hdulevel )
{
    return this->assign_extlevel(hdulevel);
}

/**
 * @brief  HDU �Υ�٥�(EXTLEVEL)������
 *
 * @note  fits_hdu::assign_hdulevel() �Ȥΰ㤤�Ϥ���ޤ���
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
 * @brief  HDU ��̾��(EXTNAME)�����
 */
const char *fits_hdu::hduname() const
{
    return this->extname_rec.cstr();
}

/**
 * @brief  HDU ��̾��(EXTNAME)�����
 *
 * @note  fitscc::extname() �Ȥΰ㤤�Ϥ���ޤ���
 */
const char *fits_hdu::extname() const
{
    return this->extname_rec.cstr();
}

/**
 * @brief  HDU �ΥС������(EXTVER)�����
 */
long long fits_hdu::hduver() const
{
    return this->extver_rec.atoll();
}

/**
 * @brief  HDU �ΥС������(EXTVER)�����
 *
 * @note  fits_hdu::hduver() �Ȥΰ㤤�Ϥ���ޤ���
 */
long long fits_hdu::extver() const
{
    return this->extver_rec.atoll();
}

/**
 * @brief  HDU �ΥС������(EXTVER)��ʸ����Ǽ���
 */
const char *fits_hdu::hduver_value() const
{
    return this->extver_rec.cstr();
}

/**
 * @brief  HDU �ΥС������(EXTVER)��ʸ����Ǽ���
 *
 * @note  fits_hdu::hduver_value() �Ȥΰ㤤�Ϥ���ޤ���
 */
const char *fits_hdu::extver_value() const
{
    return this->extver_rec.cstr();
}

/**
 * @brief  HDU �Υ�٥�(EXTLEVEL)�����
 */
long long fits_hdu::hdulevel() const
{
    return this->extlevel_rec.atoll();
}

/**
 * @brief  HDU �Υ�٥�(EXTLEVEL)�����
 *
 * @note  fits_hdu::hdulevel() �Ȥΰ㤤�Ϥ���ޤ���
 */
long long fits_hdu::extlevel() const
{
    return this->extlevel_rec.atoll();
}

/**
 * @brief  HDU �Υ�٥�(EXTLEVEL)��ʸ����Ǽ���
 */
const char *fits_hdu::hdulevel_value() const
{
    return this->extlevel_rec.cstr();
}

/**
 * @brief  HDU �Υ�٥�(EXTLEVEL)��ʸ����Ǽ���
 *
 * @note  fits_hdu::hdulevel_value() �Ȥΰ㤤�Ϥ���ޤ���
 */
const char *fits_hdu::extlevel_value() const
{
    return this->extlevel_rec.cstr();
}

/**
 * @brief  HDU �ΥС������(EXTVER)�����åȤ���Ƥ��뤫���֤�
 */
bool fits_hdu::hduver_is_set() const
{
    return (this->extver_rec.cstr() != NULL);
}

/**
 * @brief  HDU �ΥС������(EXTVER)�����åȤ���Ƥ��뤫���֤�
 *
 * @note  fits_hdu::hduver_is_set() �Ȥΰ㤤�Ϥ���ޤ���
 */
bool fits_hdu::extver_is_set() const
{
    return (this->extver_rec.cstr() != NULL);
}

/**
 * @brief  HDU �Υ�٥�(EXTLEVEL)�����åȤ���Ƥ��뤫���֤�
 */
bool fits_hdu::hdulevel_is_set() const
{
    return (this->extlevel_rec.cstr() != NULL);
}

/**
 * @brief  HDU �Υ�٥�(EXTLEVEL)�����åȤ���Ƥ��뤫���֤�
 *
 * @note  fits_hdu::hdulevel_is_set() �Ȥΰ㤤�Ϥ���ޤ���
 */
bool fits_hdu::extlevel_is_set() const
{
    return (this->extlevel_rec.cstr() != NULL);
}

/**
 * @brief  CHECKSUM �����顼���ɤ������֤� (̤����)
 *
 * @deprecated  ̤������
 */
bool fits_hdu::checksum_error() const
{
    return this->checksum_error_rec;
}

/**
 * @brief  DATASUM �����顼���ɤ������֤� (̤����)
 *
 * @deprecated  ̤������
 */
bool fits_hdu::datasum_error() const
{
    return this->datasum_error_rec;
}

/**
 * @brief  �إå��ξ��󤫤� HDU �Υ����פ�Ƚ�ꤷ���֤�
 */
int fits_hdu::hdutype_on_header()
{
    int htype = this->header_rec.hdutype();

    if ( htype == FITS::ANY_HDU ) {
	err_report(__FUNCTION__,"ERROR","unsupported HDU type");
    }

    return htype;
}

/* (�ߴ����ݤĤ���ˤ������:�Ȥ�ʤ�����) */
/* ����� fitscc ���饹�������Ѥ��ƤϤ����ʤ� */
/**
 * @deprecated  �ߴ����ݤĤ����¸�ߡ��Ȥ�ʤ��Ǥ���������
 */
const char *fits_hdu::allheader_formatted_string()
{
    return this->header_formatted_string();
}

/*
 * protected member functions
 */

/**
 * @brief  fits_header���֥������Ȥ������Ϥ����إå����ɤ�
 *
 * @return  ������: �ɤ߹�����Х��ȿ�<br>
 *          ����: ���顼
 * @note   ���Υ��дؿ��� protected �Ǥ���
 */
int fits_hdu::read_header_object( const fits_header &header_all )
{
    return this->header_load(header_all);
}

/**
 * @brief  ���ȥ꡼�फ��إå����ɤ�
 *
 * @param   sref �ɤ߹����оݤΥ��ȥ꡼�� (cstreamio�ηѾ����饹)
 * @param   max_bytes_read �ɤ߹��ޤ��٤�����ΥХ��ȿ�
 * @return  ������: �ɤ߹�����Х��ȿ�<br>
 *          ����: ���顼
 * @note   ���Υ��дؿ��� protected �Ǥ���
 */
ssize_t fits_hdu::read_stream( cstreamio &sref, size_t max_bytes_read )
{
    return this->header_load(sref,&max_bytes_read);
}

/**
 * @brief  ���ȥ꡼�फ��إå����ɤ�
 *
 * @param   sref �ɤ߹����оݤΥ��ȥ꡼�� (cstreamio�ηѾ����饹)
 * @return  ������: �ɤ߹�����Х��ȿ�<br>
 *          ����: ���顼
 * @note   ���Υ��дؿ��� protected �Ǥ���
 */
ssize_t fits_hdu::read_stream( cstreamio &sref )
{
    return this->header_load(sref,NULL);
}

/**
 * @brief  ���ȥ꡼��ؤ� Header Unit �� Data Unit �ν񤭹���
 *
 *  ���Υ��дؿ��ϷѾ����饹�� write_stream() ����ƤӽФ��졤EXTEND�� ��
 *  ���åȤ����إå��ȥǡ�������¸���롥<br>
 *  �إå��� CHECKSUM��DATASUM ��¸�ߤ�����Ϸ׻������ͤ����Ƥ�����¸���롥
 *  �ƤӽФ��ν�ϡ��㤨�� fits_table �ξ�硤<br>
 *   fits_table::write_stream() <br>
 *   -> fits_hdu::write_stream <br>
 *   -> fits_table::save_or_check_data() <br>
 *  �Ȥ�����ϩ�ˤʤ롥
 *
 * @param   sref �񤭹����оݤΥ��ȥ꡼�� (cstreamio�ηѾ����饹)
 * @return  ������: �񤭹�����Х��ȿ�<br>
 *          ����: ���顼
 * @note    �㳰�Ϥ����Ǥ�ª���ʤ��Τǡ��Ѿ����饹��ª�������<br>
 *          ���Υ��дؿ��� protected �Ǥ���
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
	/* get datasum (����ϷѾ����饹�Υ��дؿ�) */
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
 * @brief  ���ȥ꡼��˥�����������Data Unit ���ɤ����Ф�
 *
 * @param   sref �ɤ����Ф��оݤΥ��ȥ꡼�� (cstreamio�ηѾ����饹)
 * @param   max_bytes_skip �ɤ����Ф����٤�����ΥХ��ȿ�
 * @return  ������: �ɤ����Ф����Х��ȿ�<br>
 *          ����: ���顼
 * @note    ���Υ��дؿ��� protected �Ǥ���
 */
ssize_t fits_hdu::skip_data_stream( cstreamio &sref, size_t max_bytes_skip )
{
    return this->header_rec.skip_data_stream(sref,max_bytes_skip);
}

/**
 * @brief  ���ȥ꡼��˥�����������Data Unit ���ɤ����Ф�
 *
 * @param   sref �ɤ����Ф��оݤΥ��ȥ꡼�� (cstreamio�ηѾ����饹)
 * @return  ������: �ɤ����Ф����Х��ȿ�<br>
 *          ����: ���顼
 * @note    ���Υ��дؿ��� protected �Ǥ���
 */
ssize_t fits_hdu::skip_data_stream( cstreamio &sref )
{
    return this->header_rec.skip_data_stream(sref);
}

/**
 * @brief  Data Unit �񤭹��� or �����å����������Τ���δؿ�����å�
 *
 * @note   fits_hdu::write_stream() ����ƤӽФ���롥<br>
 *         �Ѿ����饹�ǥ����С��饤�ɤ��뤳�ȡ�<br>
 *         ���Υ��дؿ��� protected �Ǥ���
 */
ssize_t fits_hdu::save_or_check_data( cstreamio *sptr, void *c_sum_info )
{
    return 0;
}

/* protected*/
/**
 * @brief  ���ȥ꡼��˽񤭹��ޤ��Ǥ����Х��ȿ����֤�
 *
 * @return  ������: �񤭹��ޤ��Ǥ����Х��ȿ�<br>
 *          ����: ���顼
 * @note   ���Υ��дؿ��� protected �Ǥ���
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
 * @brief  ���Ȥ�������뤿��Υ��֥������Ȥ���Ͽ
 *
 * @note   ���Υ��дؿ��� protected �Ǥ���
 */
fits_hdu &fits_hdu::register_manager( fitscc *ptr )
{
    this->manager = ptr;
    return *this;
}

/**
 * @brief  HDU ��̾��(EXTNAME)������ (���٥�)
 *
 * @note   ����� fitscc ���饹�����Ѥ��롥<br>
 *         ���Υ��дؿ��� protected �Ǥ���
 */
fits_hdu &fits_hdu::_assign_extname( const char *extname )
{
    //err_report(__FUNCTION__,"DEBUG","called!!");
    if ( extname != NULL ) {
#if 0	/* �����ϼ�ͳ�ˤ����� */
	if ( this->is_primary_rec == true &&
	     c_strcmp(extname,"Primary") != 0 ) {
	    extname = "Primary";
	    err_report(__FUNCTION__,"WARNING",
		       "cannot set the extname to the Primary HDU");
	}
#endif
	/* extname �� NULL ���ä����� NULL �ˤ��� */
	/* init() �Ȥ� swap() �Ȥ��ǤϾ��󤬤��Τޤޥ��ԡ�����ʤ��Ⱥ���Τ� */
	//this->extname_rec.assign(extname);
	this->extname_rec = extname;
    }

    return *this;
}

/**
 * @brief  ���饹�ηѾ���٥��1�ĥ��å�
 *
 * @note   �Ѿ����饹�Υ��󥹥ȥ饯���ǻ��ѡ�<br>
 *         ���Υ��дؿ��� protected �Ǥ���
 */
int fits_hdu::increase_classlevel()
{
    this->classlevel_rec ++;
    return this->classlevel_rec;
}

/**
 * @brief  HDU �μ��̤�����
 *
 * @note   �Ѿ����饹�Υ��󥹥ȥ饯���ǻ��ѡ�<br>
 *         ���Υ��дؿ��� protected �Ǥ���
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

#if 0	/* �����ϼ�ͳ�ˤ����� */
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
 * @brief  fitscc ���֥������Ȥδ������ˤ����硤HDU �ֹ���֤�
 *
 * @note   �Ѿ����饹���� HDU �ֹ���䤤��碌����˻Ȥ���<br>
 *         ���Υ��дؿ��� protected �Ǥ���
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
 * @brief  Data Unit �˴�Ϣ����إå��쥳���ɤ򥻥åȥ��å�
 *
 * @note   write_stream() ������ʤɤˡ��Ѿ����饹�� setup_sys_header() ����
 *         �ƤФ�롥<br>
 *         ���Υ��дؿ��� protected �Ǥ���
 */
fits_hdu &fits_hdu::setup_sys_header()
{
    fits_header &hdr_rec = this->header_rec;
    bool primary_hdu = (this->hdu_index() == 0L);
    const char *fmttype = NULL;
    const char *ftypever = NULL;

    /*
     * FMTTYPE, FTYPEVER �ˤĤ��Ƥϡ�fitscc �� private ���Ф��ͤ����åȤ����
     * ������Τ߽�����Ԥʤ���
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
	        /* �ե����뤫����ɤ߼�����ư��ˤ��碌�� */
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
	     * fitscc �� private ���Ф���Ͽ����Ƥ�����ϡ�����оݤȤ���
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

		/* ������ */
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

		/* ������ */
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

		/* ������ */
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

	    /* �ե����뤫����ɤ߼�����ư��ˤ��碌�� */
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
	      /* �ե����뤫����ɤ߼�����ư��ˤ��碌�� */
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
	      /* �ե����뤫����ɤ߼�����ư��ˤ��碌�� */
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
 * @brief  Data Unit �˴�Ϣ����إå��쥳���ɤ��Խ��ػߥե饰�򥻥å�
 *
 *  true �򥻥åȤ���ȡ������ƥ�ͽ�󥭡���ɤ���Ͽ�ԲĤˤʤ롥
 *
 * @note   �Ѿ����饹�ǥ����ƥ७����ɤ�񤭹�����˻��ѡ�<br>
 *         ���Υ��дؿ��� protected �Ǥ���
 */
fits_hdu &fits_hdu::header_set_sysrecords_prohibition( bool flag )
{
    //err_report1(__FUNCTION__,"DEBUG","called !! [%d]",(int)flag);
    this->header_rec.set_sysrecords_prohibition(flag);
    return *this;
}

/**
 * @brief  ���ꤵ�줿�إå��쥳���ɥ��֥������ȤΥ��ɥ쥹���֤�
 *
 * @note   �桼���γ�ĥ���饹�ǻ��Ѥ����ꡥ�ݥ��󥿤�ĥ�����ɬ�ס�<br>
 *         ���Υ��дؿ��� protected �Ǥ���
 */
fits_header_record *fits_hdu::header_record_ptr( long index )
{
    fits_header *header_ptr;
    header_ptr = &this->header_rec;
    return header_ptr->get_ptr(index);
}

/**
 * @brief  ���ꤵ�줿�إå��쥳���ɤ��ݸ�˴ؤ�������
 *
 * @note   �ݥ��󥿤�ĥ�ä������ä��ʤ��褦���ݸ�뤿���ɬ�ס�<br>
 *         ���Υ��дؿ��� protected �Ǥ���
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
 * @brief  shallow copy ����Ĥ�����˻���
 *
 * @note   ������֥������Ȥ� return ��ľ���ǻ��Ѥ��롥
 */
/* ���: ���Ȥ��֤������Ȥ��������return ʸ�Ǥ������ȥ��󥹥��� */
/*       ��2�󥳥ԡ������Ȥ����������ʻ��ˤʤ�Τ� void �ˤʤäƤ��� */
void fits_hdu::set_scopy_flag()
{
    this->header_rec.set_scopy_flag();
    return;
}

/*
 * private member functions 
 */

/**
 * @brief  Data Unit �˴�Ϣ����إå��쥳���ɤ���Ƭ��ʬ�򥻥åȥ��å�
 *
 * @note   sysrecords_prohibition �� false �ˤ��Ƥ���Ȥ�����<br>
 *         ���Υ��дؿ��� private �Ǥ���
 */
int fits_hdu::header_setup_top_records( fits::header_def defs[],
					const char *removed_keywords[] )
{
    int status = -1;
    long i;

    /* �õ�٤���Τ�����оõ�� */
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

	/* ¸�ߤ��Ƥ����硤���Υ쥳���ɤ򤽤Τޤ޻Ȥ� */
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
 * @brief  ���ȥ꡼�फ�� FITS �إå����ɤ߹��ߡ��إå����֥������Ȥ���
 *
 * @param   sref �ɤ߹����оݤΥ��ȥ꡼�� (cstreamio�ηѾ����饹)
 * @param   max_bytes_ptr �ɤ߹��ޤ��٤�����ΥХ��ȿ����������׻���NULL��
 * @return  ������: �ɤ߹�����Х��ȿ�<br>
 *          ����: ���顼
 * @note    ���� HDU �����뤫�ʤ���������å�������ˤ�Ȥ��롥<br>
 *          ���Υ��дؿ��� private �Ǥ���
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
 * @brief  ���إå��쥳���ɤ�ޤ��ʸ������ɤ߹��ߡ��إå����֥������Ȥ���
 *
 * @param  header_all �إå������� ('\0' �ǽ�ü����ʸ����)
 * @note   ���Υ��дؿ��� private �Ǥ���
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
 * @brief  fits_header ���֥������Ȥ��ɤ߹��ߡ��إå����֥������Ȥ���
 *
 * @note   fitscc ���饹�ǤΥƥ�ץ졼�ȥե������ɤ߹��ߥ��ݡ��ȤΤ����
 *         fits_hdu::header_load(cstreamio &,const size_t *) ����ʬ�䡥<br>
 *         ���Υ��дؿ��� private �Ǥ���
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

