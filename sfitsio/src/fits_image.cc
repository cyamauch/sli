/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2013-05-06 22:00:46 cyamauch> */

/**
 * @file   fits_image.cc
 * @brief  FITS �� Image HDU ��ɽ�����륯�饹 fits_image �Υ�����
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
 * @brief  SFITSIO �ǡ��������� �� mdarray �ǡ��������� ���Ѵ� (������)
 *
 * @note    private �ʴؿ��Ǥ���
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
 * @brief  FITS::ALL => MDARRAY_ALL ��ޤࡤsection������������Ѵ� (������)
 *
 * @note    private �ʴؿ��Ǥ���
 */
inline static size_t fits_seclen_to_mdarray_seclen( long fits_sz )
{
    return (fits_sz == FITS::ALL) ? 
           MDARRAY_ALL : ((fits_sz < 0) ? 0 : ((size_t)fits_sz));
}

/**
 * @brief  �ƥǡ������κǾ��ͤ���� (������)
 *
 * @note    private �ʴؿ��Ǥ���
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
 * @brief  �ƥǡ������κ����ͤ���� (������)
 *
 * @note    private �ʴؿ��Ǥ���
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
    else return INDEF_LLONG;
}

/**
 * @brief  �񤭽Ф������� blank �ͤ��֤� (������)
 *
 * @note    private �ʴؿ��Ǥ���
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
 * @brief  �����ǡ������ݻ������������֥������Ȥ��٤Ƥ�����
 * 
 * @param  sz_type mdarray�Ǥη�����
 * @note   ���Υ��дؿ��� private �Ǥ���
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
 * @brief  this->data_rec ������
 * 
 * @param  sz_type mdarray�Ǥη�����
 * @note   ���Υ��дؿ��� private �Ǥ���
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
 * @brief  ���󥹥ȥ饯��
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
 * @brief  ���ԡ����󥹥ȥ饯��
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
 * @brief  �ǥ��ȥ饯��
 */
fits_image::~fits_image()
{
    return;
}

/**
 * @brief  ���֥������ȤΥ��ԡ�
 */
fits_image &fits_image::operator=(const fits_image &obj)
{
    this->init(obj);
    return *this;
}

/**
 * @brief  ���֥������Ȥν����
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
 * @brief  bzero, bscale �ʤɤ�°���򥳥ԡ�
 *
 * @note   ���Υ��дؿ��� private �Ǥ���
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
 * @brief  ���֥������ȤΥ��ԡ�
 */
fits_image &fits_image::init( const fits_image &obj )
{
    if ( &obj == this ) return *this;

    //err_report("init()","DEBUG","fits_image::init(const fits_image &) ...");
    this->fits_image::init();
    this->fits_hdu::init(obj);

    /* �����ˤ� fits_image ���饹������ʤ��ϥ�������������å� */
    if ( 1 <= obj.classlevel() && obj.hdutype() == FITS::IMAGE_HDU ) {
	const fits_image &obj1 = (const fits_image &)obj;

	try {
	    /* �Хåե������ */
	    this->_init_all_data_recs(obj1.data_array().size_type());
	    this->type_rec = obj1.type_rec;

	    this->data_array().init(obj1.data_array());
	}
	catch (...) {
	    err_throw(__FUNCTION__,"FATAL","this->data_array().init() failed");
	}

	/* bzero, bscale �ʤɤ�°���򥳥ԡ� */
	this->init_properties(obj1);

	this->coord_offset_rec.init(obj1.coord_offset_rec);
	this->is_flipped_rec.init(obj1.is_flipped_rec);
    }

    return *this;
}

/**
 * @brief  ���֥������Ȥν�����Ȳ����ǡ�������Υꥵ���� (1��3����)
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
 * @brief  ���֥������Ȥν�����Ȳ����ǡ�������Υꥵ���� (n����)
 *
 * @param     type �ǡ�������η����� (FITS::SHORT_T��FITS::FLOAT_T ��)
 * @param     naxisx[] �Ƽ��������ǿ�
 * @param     ndim ����μ�����
 * @param     buf_init �����ͤ�ǥե�����ͤ�����ʤ� true
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
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

    /* �������� 0 �ʲ��μ�������ȥޥ����Τǥ����å� */
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
	/* �Хåե������ */
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
 * @brief  �����ǡ�������ꤵ�줿�����Ѵ�
 *
 * @param     new_type �Ѵ���η����� (FITS::FLOAT_T��FITS::DOUBLE_T ��)
 * @return    ���Ȥλ���
 */
fits_image &fits_image::convert_type( int new_type )
{
    /* convert ... */
    return this->convert_image_type( new_type, NULL, NULL, NULL );

    /* setup system header of this HDU */
    this->setup_sys_header();
}

/**
 * @brief  ���ꤵ�줿����ZERO�ͤ���Ĳ����ǡ������Ѵ�
 */
fits_image &fits_image::convert_type( int new_type, double new_zero )
{
    /* convert ... */
    return this->convert_image_type( new_type, &new_zero, NULL, NULL );

    /* setup system header of this HDU */
    this->setup_sys_header();
}

/**
 * @brief  ���ꤵ�줿����ZERO�͡�SCALE�ͤ���Ĳ����ǡ������Ѵ�
 */
fits_image &fits_image::convert_type( int new_type, double new_zero, double new_scale )
{
    /* convert ... */
    return this->convert_image_type( new_type, &new_zero, &new_scale, NULL );

    /* setup system header of this HDU */
    this->setup_sys_header();
}

/**
 * @brief  ���ꤵ�줿����ZERO�͡�SCALE�͡�BLANK�ͤ���Ĳ����ǡ������Ѵ�
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
 * @brief  BZERO���ͤ�����
 *
 * @param   zero BZERO����
 * @param   prec ����(���)����ά����15��
 * @return  ���Ȥλ���
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
 * @brief  BSCALE���ͤ�����
 *
 * @param   scale BSCALE����
 * @param   prec ����(���)����ά����15��
 * @return  ���Ȥλ���
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
 * @brief  BLANK���ͤ�����
 *
 * @param   blank BLANK����
 * @return  ���Ȥλ���
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
 * @brief  BUNIT���ͤ�����
 *
 * @param   unit BUNIT����
 * @return  ���Ȥλ���
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
 * @brief  BZERO�������õ�
 *
 * @return  ���Ȥλ���
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
 * @brief  BSCALE�������õ�
 *
 * @return  ���Ȥλ���
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
 * @brief  BLANK�������õ�
 *
 * @return  ���Ȥλ���
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
 * @brief  BUNIT�������õ�
 *
 * @return  ���Ȥλ���
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
 * @brief  2�ĤΥ��֥������ȴ֤Ǥ����ƤΥ���å�
 *
 *  ���ꤵ�줿���֥�������sobj �����Ƥȼ��Ȥ����Ƥ������ؤ��ޤ���
 *  �����ǡ������󡦥إå������ơ�°�������٤Ƥξ��֤������ؤ��ޤ���
 *
 * @param     sobj fits_image ���饹�Υ��֥�������
 * @return    ���Ȥλ���    
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

    /* data_rec �ݥ���Ž��ʤ��� */
    this->_setup_data_rec(image_type_to_size_type(this->type_rec));
    obj._setup_data_rec(image_type_to_size_type(obj.type_rec));

    fits_hdu::swap(obj);

    return *this;
}

/**
 * @brief  ���Ȥ����Ƥ���ꥪ�֥������Ȥإ��ԡ�
 *
 *  ���Ȥ����Ƥ����Ƥ���ꤵ�줿���֥������� dest_img �إ��ԡ����ޤ���
 *  �����ǡ������󡦥إå������ơ�°�������٤Ƥ����ԡ�����ޤ�������(���ԡ���)
 *  �ϲ��Ѥ���ޤ���<br>
 *  ���Ȳ����ǡ�����������Ĺ�� dest_img �ȼ��ȤȤ����������ϡ������ѥХåե�
 *  �κƳ��ݤϹԤʤ�줺����¸�ΥХåե�����������Ƥ����ԡ�����ޤ���
 *
 * @param     dest_img ���ԡ���Υ��֥������ȤΥ��ɥ쥹
 * @throw     �Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
 */
void fits_image::copy( fits_image *dest_img ) const
{
    if ( dest_img == NULL ) {
	return;
    }
    /* dest_img �����Ȥʤ鲿�⤷�ʤ� */
    else if ( dest_img != this ) {

	if ( dest_img->type() != this->type() ) {
	    /* �Хåե������ */
	    dest_img->_init_all_data_recs(this->data_array().size_type());
	    dest_img->type_rec = this->type_rec;
	}

	/* ���ȤΥХåե������Ƥ򥳥ԡ� */
	this->data_array().copy( dest_img->data_rec );

	/* bzero, bscale �ʤɤ�°���򥳥ԡ� */
	dest_img->init_properties(*this);
	/* �إå��򥳥ԡ� */
	dest_img->header_init( this->header() );

	return;
    }
}

/**
 * @deprecated ��侩��<br>
 * void fits_image::copy( fits_image *dest_img ) const <br>
 * �򤪻Ȥ�����������
 */
void fits_image::copy( fits_image &dest_img ) const
{
    this->copy(&dest_img);
    return;
}

/**
 * @brief  ���Ȥ����Ƥ���ꥪ�֥������Ȥذܴ�
 *
 *  ���ȤΥǡ�����������Ƥ�dest_img �ˤ����ꤵ�줿���֥������Ȥءְܴɡ�
 *  ���ޤ�(��������Ĺ���Ƽ�°�������ꤵ�줿���֥������Ȥ˥��ԡ�����ޤ�)���ܴ�
 *  �η�̡����ȤΥǡ��������Ĺ���ϥ���ˤʤ�ޤ���<br>
 *  dest_img �ˤĤ��Ƥ������ѥХåե��κƳ��ݤϹԤʤ�줺�����Ȥ������ѥХåե�
 *  �ˤĤ��Ƥδ������¤� dest_img �˾��Ϥ�������ˤʤäƤ��� ��®��ư��ޤ�
 *  (���������إå��˴ؤ��Ƥϸ����Ǥϥ��ԡ�&�õ�μ����ˤʤäƤ��ޤ�)��
 *
 * @param     dest_img �ܴɡ����ԡ���Υ��֥������ȤΥ��ɥ쥹
 * @return    ���Ȥλ���
 * @throw     �Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
 */
fits_image &fits_image::cut( fits_image *dest_img )
{
    if ( dest_img == NULL ) {
	if ( 0 < this->dim_length() ) this->reallocate(NULL,0,false);
	this->header_init();
    }
    /* dest_img �����Ȥʤ鲿�⤷�ʤ� */
    else if ( dest_img != this ) {

	if ( dest_img->type() != this->type() ) {
	    /* �Хåե������ */
	    dest_img->_init_all_data_recs(this->data_array().size_type());
	    dest_img->type_rec = this->type_rec;
	}

	/* ���ȤΥХåե������Ƥ��ư */
	this->data_array().cut( dest_img->data_rec );

	/* bzero, bscale �ʤɤ�°���򥳥ԡ� */
	dest_img->init_properties(*this);
	/* �إå��򥳥ԡ� */
	dest_img->header_init( this->header() );

	this->header_init();

    }

    return *this;
}

/* interchange rows and columns */
/**
 * @brief  ���Ȥβ����ǡ�������� (x,y) �ǤΥȥ�󥹥ݡ���
 *
 *  ���Ȥβ����ǡ�������Υ����ȥ��Ȥ������ؤ��ޤ���
 * 
 * @note  ��®�ʥ��르�ꥺ�����Ѥ��Ƥ��ޤ���
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
 * @brief  ���Ȥβ����ǡ�������� (x,y,z)��(z,x,y) �Υȥ�󥹥ݡ���
 *
 *  ���Ȥβ����ǡ�������μ� (x,y,z) �� (z,x,y) ���Ѵ����ޤ���
 * 
 * @note  ��®�ʥ��르�ꥺ�����Ѥ��Ƥ��ޤ���
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
 * @brief  ���Ȥβ����ǡ�������� (x,y) �Ǥβ�ž (90��ñ��)
 *
 *  ���Ȥβ����ǡ�������� (x,y) �̤ˤĤ��Ƥβ�ž(90��ñ��)��Ԥʤ��ޤ���<br>
 *  ���������Ȥ�����硤ȿ���פޤ��ǻ��ꤷ�ޤ���
 *
 * @param  angle 90,-90, 180 �Τ����줫�����
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
 * @brief  BUNIT���ͤ����
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
 * @brief  BUNIT���ͤ����ꤵ��Ƥ��뤫�ɤ������֤�
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
 * @brief  ���Ȥβ����ǡ��������桼�����Хåե��إ��ԡ� (�Х��ȿ��Ǥλ���)
 *
 *  ���Ȥβ����ǡ�����������Ƥ� dest_buf �ǻ��ꤵ�줿�桼�����Хåե��إ��ԡ�
 *  ���ޤ���<br>
 *  �Хåե����礭�� buf_size �ϡ��Х���ñ�̤�Ϳ���ޤ���axis0, axis1, axis2 ��
 *  ���Ȥ�����Ǥγ���������ꤷ�ޤ���<br>
 *  �����ˡ�FITS::INDEF ������Ū��Ϳ���ʤ��Ǥ���������
 *
 * @param     dest_buf �桼�����Хåե��Υ��ɥ쥹 (���ԡ���)
 * @param     buf_size �Хåե������� (�Х���ñ��)
 * @param     axis0 �����ֹ� 0 �μ���(1������)�������ֹ� (��ά��)
 * @param     axis1 �����ֹ� 1 �μ���(2������)�������ֹ� (��ά��)
 * @param     axis2 �����ֹ� 2 �μ���(3������)�������ֹ� (��ά��)
 * @return    �桼���ΥХåե�Ĺ(buf_size) ����ʬ�ʾ��˥��ԡ������
 *            �Х��ȥ�����
 * @throw     �����˲��򵯤��������
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
 * @brief  �桼�����Хåե����鼫�Ȥβ����ǡ�������إ��ԡ� (�Х��ȿ��Ǥλ���)
 *
 *  src_buf �ǻ��ꤵ�줿�桼�����Хåե������Ƥ򼫿Ȥβ����ǡ�������إ��ԡ���
 *  �ޤ���<br>
 *  �Хåե����礭�� buf_size �ϡ��Х���ñ�̤�Ϳ���ޤ���axis0, axis1, axis2 ��
 *  ���Ȥ�����Ǥγ���������ꤷ�ޤ���<br>
 *  �����ˡ�FITS::INDEF ������Ū��Ϳ���ʤ��Ǥ���������
 *
 * @param     src_buf  �桼�����Хåե��Υ��ɥ쥹 (���ԡ���)
 * @param     buf_size �桼�����Хåե��Υ����� (�Х���ñ��)
 * @param     axis0 �����ֹ� 0 �μ���(1������)�������ֹ� (��ά��)
 * @param     axis1 �����ֹ� 1 �μ���(2������)�������ֹ� (��ά��)
 * @param     axis2 �����ֹ� 2 �μ���(3������)�������ֹ� (��ά��)
 * @return    �桼���ΥХåե�Ĺ(buf_size) ����ʬ�ʾ��˥��ԡ������
 *            �Х��ȥ�����
 * @throw     �����˲��򵯤��������
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
 * @brief  ��������1�ĳ�ĥ
 *
 *  ���Ȥ����Ĳ����ǡ�������μ������� 1�ĳ�ĥ���ޤ���
 *
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
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
 * @brief  ��������1�ĳ�ĥ
 *
 *  ���Ȥ����Ĳ����ǡ�������μ������� 1�ĳ�ĥ���ޤ���
 *
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
 * @note      fits_image::increase_dim() �Ȥΰ㤤�Ϥ���ޤ���
 */
fits_image &fits_image::increase_axis()
{
    this->increase_dim();
    return *this;
}

/**
 * @brief  ��������1�Ľ̾�
 *
 *  ���Ȥ����Ĳ����ǡ�������μ����� 1�Ľ̾����ޤ���
 *
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
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
 * @brief  ��������1�Ľ̾�
 *
 *  ���Ȥ����Ĳ����ǡ�������μ����� 1�Ľ̾����ޤ���
 *
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
 * @note      fits_image::decrease_dim() �Ȥΰ㤤�Ϥ���ޤ���
 */
fits_image &fits_image::decrease_axis()
{
    this->decrease_dim();
    return *this;
}

/**
 * @brief  Ǥ�դ�1�Ĥμ����ˤĤ��Ƥβ����ǡ�������Ĺ���ѹ�
 *
 *  ���Ȥ����Ĳ����ǡ��������Ĺ�����ѹ����ޤ���<br>
 *  ����Ĺ���ĥ�����硤���Ǥ��ͤϥǥե�����ͤ������ޤ���<br>
 *  ����Ĺ����̤����硤len�ʹߤ����ǤϺ������ޤ���<br>
 *  �����ֹ� axis �ǽ����оݤȤ��뼡�������Ǥ��ޤ���
 *
 * @param     axis �����ֹ�
 * @param     size ���ǤθĿ�
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
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
 * @brief  �����ǡ�������Ĺ���ѹ� (1����)
 *
 *  ���Ȥ����Ĳ����ǡ��������Ĺ�����ѹ���������Ĺ x_len ��1��������Ȥ��ޤ���
 */
fits_image &fits_image::resize_1d( long x_len )
{
    const long nx[] = {x_len};
    return this->resize(nx, 1, true);
}

/* change the length of the 2-d array */
/**
 * @brief  �����ǡ�������Ĺ���ѹ� (2����)
 *
 *  ���Ȥ����Ĳ����ǡ��������Ĺ�����ѹ���������Ĺ (x_len, y_len) �� 
 *  2��������Ȥ��ޤ���
 */
fits_image &fits_image::resize_2d( long x_len, long y_len )
{
    const long nx[] = {x_len, y_len};
    return this->resize(nx, 2, true);
}

/* change the length of the 3-d array */
/**
 * @brief  �����ǡ�������Ĺ���ѹ� (3����)
 *
 *  ���Ȥ����Ĳ����ǡ��������Ĺ�����ѹ���������Ĺ (x_len, y_len, z_len) ��
 *  3��������Ȥ��ޤ���
 */
fits_image &fits_image::resize_3d( long x_len, long y_len, long z_len )
{
    const long nx[] = {x_len, y_len, z_len};
    return this->resize(nx, 3, true);
}

/**
 * @brief  �����ǡ�������Ĺ���ѹ� (ʣ���μ���������)
 *
 *  ���Ȥ����Ĳ����ǡ��������Ĺ�����ѹ����ޤ���<br>
 *  ����Ĺ���ĥ�����硤���Ǥ��ͤ�ǥե�����ͤ���뤫�ɤ����� buf_init ��
 *  ����Ǥ��ޤ���<br>
 *  ����Ĺ����̤����硤����Ĺ�������ʤ���ʬ�����ǤϺ������ޤ���<br>
 *
 * @param     naxisx[] �Ƽ��������ǿ�
 * @param     ndim ����μ�����
 * @param     buf_init ����Ĺ�γ�ĥ���������ͤ�ǥե�����ͤ�����ʤ� true
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
 * @note      2�����ʾ�Υꥵ�����ξ�硤�㥳���ȤǹԤʤ��ޤ���1���������ꥵ��
 *            ������ʤ顤resize(axis, ...) �������㥳���ȤǤ���
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
 * @brief  �����ǡ�������Ĺ���ѹ� (IDL�ε�ˡ)
 *
 *  ���Ȥ����Ĳ����ǡ��������Ĺ�����ѹ����ޤ���Ĺ���λ���ϡ�printf() ������
 *  ����Ĺ�����ǻ����ǽ�ʥ���޶��ڤ��ʸ����(�㤨��"10,5")�ˤ�äƹԤʤ���
 *  ����<br>
 *  ���ꤵ�줿ʸ����������Ǥ������ǤϤʤ���硤���μ�����Ĺ�����ѹ����ޤ���
 *  �㤨�С� ",2" �����ꤵ�줿��硤�ǽ�μ�����Ĺ�����ѹ�����ޤ���
 *
 * @param   exp_fmt csv������Ĺ������ʸ����Τ���Υե����ޥåȻ���
 * @param   ...     exp_fmt���б���������Ĺ�����γ����ǥǡ���
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
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
 * @brief  �����ǡ�������Ĺ���ѹ� (IDL�ε�ˡ)
 *
 *  ���Ȥ����Ĳ����ǡ��������Ĺ�����ѹ����ޤ���Ĺ���λ���ϡ�printf() ������
 *  ����Ĺ�����ǻ����ǽ�ʥ���޶��ڤ��ʸ����(�㤨��"10,5")�ˤ�äƹԤʤ���
 *  ����<br>
 *  ���ꤵ�줿ʸ����������Ǥ������ǤϤʤ���硤���μ�����Ĺ�����ѹ����ޤ���
 *  �㤨�С� ",2" �����ꤵ�줿��硤�ǽ�μ�����Ĺ�����ѹ�����ޤ���
 *
 * @param   exp_fmt csv������Ĺ������ʸ����Τ���Υե����ޥåȻ���
 * @param   ap      exp_fmt���б���������Ĺ�����Υꥹ��
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
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
 * @brief  Ǥ�դ�1�Ĥμ����ˤĤ��Ʋ����ǡ�������Ĺ������Ū���ѹ�
 *
 *  ���Ȥ����Ĳ����ǡ��������Ĺ���� size_rel �λ���ʬ����ĥ���̾����ޤ���<br>
 *  resizeby() ������󥵥����ϡ����������Ĺ���� size_rel ��ä�����ΤȤʤ�
 *  �ޤ���
 *  �������ν̾��ϡ�size_rel �˥ޥ��ʥ��ͤ���ꤹ�뤳�Ȥˤ�äƹԤ��ޤ���<br>
 *  �����ֹ� axis �ǽ����оݤȤ��뼡�������Ǥ��ޤ���
 *
 * @param     axis �����ֹ�
 * @param     size_rel ���ǸĿ�����ʬ����ʬ
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
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
 * @brief  �����ǡ�������Ĺ������Ū���ѹ� (1������)
 */
fits_image &fits_image::resizeby_1d( long x_len )
{
    const long nx[] = {x_len};
    return this->resizeby(nx, 1, true);
}

/* change the length of the 2-d array relatively */
/**
 * @brief  �����ǡ�������Ĺ������Ū���ѹ� (2������)
 */
fits_image &fits_image::resizeby_2d( long x_len, long y_len )
{
    const long nx[] = {x_len, y_len};
    return this->resizeby(nx, 2, true);
}

/* change the length of the 3-d array relatively */
/**
 * @brief  �����ǡ�������Ĺ������Ū���ѹ� (3������)
 */
fits_image &fits_image::resizeby_3d( long x_len, long y_len, long z_len )
{
    const long nx[] = {x_len, y_len, z_len};
    return this->resizeby(nx, 3, true);
}

/**
 * @brief  �����ǡ�������Ĺ������Ū���ѹ� (ʣ���μ���������)
 *
 *  ���Ȥ����Ĳ����ǡ��������Ĺ���� naxisx_rel[] �λ���ʬ����ĥ���̾�����
 *  ����<br>
 *  resizeby() ������󥵥����ϡ����������Ĺ���� naxisx_rel[] ��ä������
 *  �Ȥʤ�ޤ����������ν̾��ϡ�naxisx_rel[] �˥ޥ��ʥ��ͤ���ꤹ�뤳�Ȥˤ�ä�
 *  �Ԥ��ޤ���<br>
 *  buf_buf �ǥ�������ĥ���˿����˺�����������Ǥν�����򤹤뤫�ɤ�����
 *  ����Ǥ��ޤ���
 *
 * @param     naxisx_rel ���ǸĿ�����ʬ����ʬ
 * @param     ndim naxisx_rel[] �θĿ�
 * @param     buf_init �����˺�����������Ǥν������Ԥʤ����� true
 * @return    ���Ȥλ���
 * @note      ����Ĺ�����䤹���ϤǤ��ޤ��������餹���ϤǤ��ޤ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
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
 * @brief  �����ǡ�������Ĺ������Ū���ѹ� (IDL�ε�ˡ)
 *
 *  ���Ȥ����Ĳ����ǡ��������Ĺ�������ʬ����ĥ���̾����ޤ���Ĺ���λ���ϡ�
 *  printf() �����β���Ĺ�����ǻ����ǽ�ʥ���޶��ڤ��ʸ����(�㤨��"10,-5")��
 *  ��äƹԤʤ��ޤ���<br>
 *  resizebyf()������󥵥����ϡ����������Ĺ���˻���ʬ��ä�����ΤȤʤ�ޤ���
 *  �������ν̾��ϡ��ޥ��ʥ��ͤ���ꤹ�뤳�Ȥˤ�äƹԤ��ޤ���<br>
 *  ���ꤵ�줿ʸ����������Ǥ������ǤϤʤ���硤���μ�����Ĺ�����ѹ����ޤ���
 *  �㤨�С� ",2" �����ꤵ�줿��硤�ǽ�μ�����Ĺ�����ѹ�����ޤ���
 *
 * @param   exp_fmt csv������Ĺ������ʸ����Τ���Υե����ޥåȻ���
 * @param   ...     exp_fmt���б���������Ĺ�����γ����ǥǡ���
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
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
 * @brief  �����ǡ�������Ĺ������Ū���ѹ� (IDL�ε�ˡ)
 *
 *  ���Ȥ����Ĳ����ǡ��������Ĺ�������ʬ����ĥ���̾����ޤ���Ĺ���λ���ϡ�
 *  printf() �����β���Ĺ�����ǻ����ǽ�ʥ���޶��ڤ��ʸ����(�㤨��"10,-5")��
 *  ��äƹԤʤ��ޤ���<br>
 *  resizebyf()������󥵥����ϡ����������Ĺ���˻���ʬ��ä�����ΤȤʤ�ޤ���
 *  �������ν̾��ϡ��ޥ��ʥ��ͤ���ꤹ�뤳�Ȥˤ�äƹԤ��ޤ���<br>
 *  ���ꤵ�줿ʸ����������Ǥ������ǤϤʤ���硤���μ�����Ĺ�����ѹ����ޤ���
 *  �㤨�С� ",2" �����ꤵ�줿��硤�ǽ�μ�����Ĺ�����ѹ�����ޤ���
 *
 * @param   exp_fmt csv������Ĺ������ʸ����Τ���Υե����ޥåȻ���
 * @param   ap      exp_fmt���б���������Ĺ�����Υꥹ��
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
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
 * @brief  �����ǡ�������ΥХåե�����֤�Ĵ���򤻤��ˡ������ѥХåե���Ƴ���
 *
 *  �����ǡ�������ΥХåե�����֤�Ĵ���򤻤��ˡ����Ȥ����ļ������礭��������
 *  ��Ĺ�����ѹ����ޤ����Ĥޤꡤ�����ѥХåե����Ф��Ƥ�ñ���realloc()��Ƥ֤�
 *  ���ν�����Ԥʤ��ޤ���
 *
 * @param   naxisx[] �Ƽ��������ǿ�
 * @param   ndim ����μ�����
 * @param   buf_init �����˳��ݤ�����ʬ�������ͤ�ǥե�����ͤ�����ʤ� true
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
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
 * @brief  �����ǡ��������ѥХåե����ö�������������˳���
 *
 *  �����ǡ�����������Ƥ��ö�˴����������Ĺ�����ѹ����ޤ����Ĥޤꡤ������
 *  �Хåե����Ф��� free()��malloc() ��Ƥ֤����ν�����Ԥʤ��ޤ���
 *
 * @param   naxisx[] �Ƽ��������ǿ�
 * @param   ndim ����μ�����
 * @param   buf_init �����˳��ݤ�����ʬ�������ͤ�ǥե�����ͤ�����ʤ� true
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
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
 * @brief  �����ǡ�������Υ�������ĥ���ν���ͤ�����
 *
 *  �����ǡ�������Υ�������ĥ���ν���ͤ����ꤷ�ޤ������ꤵ�줿�ͤϴ�¸������
 *  �ˤϺ��Ѥ�������������ĥ����ͭ���Ȥʤ�ޤ���
 *
 * @param     value ���󥵥�����ĥ���ν����
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
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
 * @brief  �����ǡ�������Υ�������ĥ���ν���ͤ����� (���٥�)
 *
 *  �����ǡ�������Υ�������ĥ���ν���ͤ����ꤷ�ޤ������ꤵ�줿�ͤϴ�¸������
 *  �ˤϺ��Ѥ�������������ĥ����ͭ���Ȥʤ�ޤ���
 *
 * @param     value_ptr ���󥵥�����ĥ���ν���ͤΥ��ɥ쥹
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
 */
fits_image &fits_image::assign_default_value( const void *value_ptr )
{
    this->data_array().mdarray::assign_default(value_ptr);

    return *this;
}

/* insert a blank section */
/**
 * @brief  Ǥ�դ�1�Ĥμ����ˤĤ��ƿ����������Ǥ�����
 *
 *  ���Ȥβ����ǡ�����������ǰ��� idx �ˡ�len ��ʬ�����Ǥ��������ޤ����ʤ���
 *  ������������Ǥ��ͤϥǥե�����ͤǤ���<br>
 *  �����ֹ� dim_index �ǽ����оݤȤ��뼡�������Ǥ��ޤ���
 *
 * @param     dim_index �����ֹ�
 * @param     idx �������֤������ֹ�
 * @param     len ���ǤθĿ�
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
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
 * @brief  Ǥ�դ�1�Ĥμ����ˤĤ����������Ǥκ��
 *
 *  ���Ȥβ����ǡ������󤫤���ꤵ�줿��ʬ�����Ǥ������ޤ����������ʬ��
 *  Ĺ����û���ʤ�ޤ���<br>
 *  �����ֹ� dim_index �ǽ����оݤȤ��뼡�������Ǥ��ޤ���
 *
 * @param     dim_index �����ֹ�
 * @param     idx ���ϰ��֤������ֹ�
 * @param     len ���ǤθĿ�
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
 */
fits_image &fits_image::erase( size_t dim_index, long idx, size_t len )
{
    this->data_array().erase(dim_index, idx, len);

    /* setup system header of this HDU */
    this->setup_sys_header();

    return *this;
}

/* copy values between elements (without automatic resizing) */
/* ��ư(�Хåե��Υ��������ѹ����ʤ�)                        */
/**
 * @brief  Ǥ�դ�1�Ĥμ����ˤĤ����������Ǵ֤Ǥ��ͤΥ��ԡ�
 *
 *  ���Ȥβ����ǡ�����������Ǵ֤��ͤ򥳥ԡ����ޤ���<br>
 *  ���� clr �� false �����ꤵ�줿���ϥ��ԡ������ͤϻĤ�ޤ�����true ������
 *  ���줿���ϥ��ԡ������ͤϻĤ餺�ǥե�����ͤ������ޤ���<br>
 *  idx_dst �˴�¸������Ĺ����礭���ͤ����ꤷ�Ƥ⡤���󥵥������Ѥ��ޤ���
 *  ������������ cpy() ���дؿ��Ȥϰۤʤ�ޤ���<br>
 *  �����ֹ� dim_index �ǽ����оݤȤ��뼡�������Ǥ��ޤ���
 *
 * @param     dim_index �����ֹ�
 * @param     idx_src ���ԡ����������ֹ�
 * @param     len ���ԡ��������Ǥ�Ĺ��
 * @param     idx_dst ���ԡ���������ֹ�
 * @param     clr ���ԡ������ͤΥ��ꥢ����
 * @return    ���Ȥλ���
 */
fits_image &fits_image::move( long dim_index, 
			      long idx_src, long len, long idx_dst, bool clr )
{
    this->data_array().move(dim_index, idx_src, len, idx_dst, clr);
    return *this;
}

/* copy values between elements (with automatic resizing) */
/* ��ư(�Хåե��Υ�������ɬ�פ˱������ѹ�����)           */
/**
 * @brief  Ǥ�դ�1�Ĥμ����ˤĤ����������Ǵ֤Ǥ��ͤΥ��ԡ� (������ϼ�ư��ĥ)
 *
 *  ���Ȥβ����ǡ����������Ǵ֤��ͤ򥳥ԡ����ޤ���<br>
 *  ���� clr �� false �����ꤵ�줿���ϥ��ԡ������ͤϻĤ�ޤ�����true ������
 *  ���줿���ϥ��ԡ������ͤϻĤ餺�ǥե�����ͤ������ޤ���<br>
 *  idx_dst + len ����¸������Ĺ����礭����硤���󥵥����ϼ�ư��ĥ�����
 *  ����<br>
 *  �����ֹ� dim_index �ǽ����оݤȤ��뼡�������Ǥ��ޤ���
 *
 * @param     dim_index �����ֹ�
 * @param     idx_src ���ԡ����������ֹ�
 * @param     len ���ԡ��������Ǥ�Ĺ��
 * @param     idx_dst ���ԡ���������ֹ�
 * @param     clr ���ԡ������ͤΥ��ꥢ����
 * @return    ���Ȥλ���
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
/* �Хåե���Ǥ����촹�� */
/**
 * @brief  Ǥ�դ�1�Ĥμ����ˤĤ����������Ǵ֤Ǥ��ͤ����촹��
 *
 *  ���Ȥβ����ǡ�����������Ǵ֤��ͤ������ؤ��ޤ���<br>
 *  �����ֹ� dim_index �������ֹ� idx_src ���� len ��ʬ�����Ǥ������ֹ� 
 *  idx_dst ���� len ��ʬ�����Ǥ������ؤ��ޤ���<br>
 *  idx_dst + len �����󥵥�����Ķ������ϡ����󥵥����ޤǤν������Ԥ���
 *  ���������ؤ����ΰ褬�Ťʤä���硤�ŤʤäƤ��ʤ� src ���ΰ���Ф��ƤΤ���
 *  ���ؤ��������Ԥ��ޤ���
 *
 * @param     dim_index �����ֹ�
 * @param     idx_src �����ؤ����������ֹ�
 * @param     len �����ؤ��������Ǥ�Ĺ��
 * @param     idx_dst �����ؤ���������ֹ�
 * @return    ���Ȥλ���
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
 * @brief  Ǥ�դ�1�Ĥμ����ˤĤ��������������ʬ�ξõ�
 *
 *  ���Ȥβ����ǡ�����������ǰ��� idx ���� len �Ĥ����Ǥ����ˤ��ޤ���<br>
 *  �����ֹ� dim_index �ǽ����оݤȤ��뼡�������Ǥ��ޤ���
 *
 * @param     dim_index �����ֹ�
 * @param     idx �ڤ�Ф����ϰ��֤������ֹ�
 * @param     len ���ǤθĿ�
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
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
 * @brief  Ǥ�դ�1�Ĥμ����ˤĤ���������¤Ӥ�ȿž
 *
 *  ���Ȥβ����ǡ�����������ǰ��� idx ���� len �Ĥ����Ǥ�ȿž�����ޤ���<br>
 *  �����ֹ� dim_index �ǽ����оݤȤ��뼡�������Ǥ��ޤ���
 *
 * @param     dim_index �����ֹ�
 * @param     idx ���ϰ��֤������ֹ�
 * @param     len ���ǤθĿ�
 * @return    ���Ȥλ���
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
 * @brief  �������Ǥ��ڤ�Ф��������ƥ�ݥ�ꥪ�֥������ȤǼ��� (IDL�ε�ˡ)
 *
 * ���Ȥβ����ǡ�������Τ������ꤵ�줿��ʬ������ƥ�ݥ�ꥪ�֥������Ȥ˥��ԡ�
 * ����������֤��ޤ���<br>
 * ���Ф���뤵����ϰϤ�Ǥ�դμ����ޤǻ����ǽ�� "0:99,*" �Τ褦�� IDL ��
 * IRAF ��Ʊ�ͤ�ʸ����ǻ��ꤷ�ޤ�������ʸ������������ֹ�� 0-indexed �Ǥ���
 *
 * @param   exp_fmt  csv�������ϰϻ���ʸ����Τ���Υե����ޥåȻ���
 * @param   ...      exp_fmt���б���������Ĺ�����γ����ǥǡ���
 * @return  �ƥ�ݥ�ꥪ�֥�������
 * @note    "[1:100,*]" �Τ褦�� [] �ǰϤ�ȡ�1-indexed �Ȥ��ư����ޤ���
 * @throw   �Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
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
 * @brief  �������Ǥ��ڤ�Ф��������ƥ�ݥ�ꥪ�֥������ȤǼ��� (IDL�ε�ˡ)
 *
 * ���Ȥβ����ǡ�������Τ������ꤵ�줿��ʬ������ƥ�ݥ�ꥪ�֥������Ȥ˥��ԡ�
 * ����������֤��ޤ���<br>
 * ���Ф���뤵����ϰϤ�Ǥ�դμ����ޤǻ����ǽ�� "0:99,*" �Τ褦�� IDL ��
 * IRAF ��Ʊ�ͤ�ʸ����ǻ��ꤷ�ޤ�������ʸ������������ֹ�� 0-indexed �Ǥ���
 *
 * @param   exp_fmt  csv�������ϰϻ���ʸ����Τ���Υե����ޥåȻ���
 * @param   ap       exp_fmt���б���������Ĺ�����Υꥹ��
 * @return  �ƥ�ݥ�ꥪ�֥�������
 * @note    "[1:100,*]" �Τ褦�� [] �ǰϤ�ȡ�1-indexed �Ȥ��ư����ޤ���
 * @throw   �Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
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
 * @brief  �������Ǥ��̥��֥������Ȥإ��ԡ� (IDL�ε�ˡ)
 *
 *  ���Ȥβ����ǡ�����������Ƥΰ���ʬ����ꤵ�줿���֥������� dest_img ��
 *  ���ԡ����ޤ���<br>
 *  ���ԡ�������ϰϤ�Ǥ�դμ����ޤǻ����ǽ�� "0:99,*" �Τ褦�� IDL �� IRAF ��
 *  Ʊ�ͤ�ʸ����ǻ��ꤷ�ޤ�������ʸ������������ֹ�� 0-indexed �Ǥ���<br>
 *  ���ԡ���ؤϡ������ǡ��������إå������ơ��Ƽ�°���ʤɤ��٤Ƥ򥳥ԡ�����
 *  ��������(���ԡ���) ������ϲ��Ѥ���ޤ���<br>
 *  dest_img �˼��Ȥ�Ϳ������硤trimf()��¹Ԥ�������Ʊ����̤ˤʤ�ޤ���
 *
 * @param   dest_img ���ԡ���Υ��֥�������
 * @param   exp_fmt  csv�������ϰϻ���ʸ����Τ���Υե����ޥåȻ���
 * @param   ...      exp_fmt���б���������Ĺ�����γ����ǥǡ���
 * @note    "[1:100,*]" �Τ褦�� [] �ǰϤ�ȡ�1-indexed �Ȥ��ư����ޤ���
 * @throw   �Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
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
 * @brief  �������Ǥ��̥��֥������Ȥإ��ԡ� (IDL�ε�ˡ)
 *
 *  ���Ȥβ����ǡ�����������Ƥΰ���ʬ����ꤵ�줿���֥������� dest_img ��
 *  ���ԡ����ޤ���<br>
 *  ���ԡ�������ϰϤ�Ǥ�դμ����ޤǻ����ǽ�� "0:99,*" �Τ褦�� IDL �� IRAF ��
 *  Ʊ�ͤ�ʸ����ǻ��ꤷ�ޤ�������ʸ������������ֹ�� 0-indexed �Ǥ���<br>
 *  ���ԡ���ؤϡ������ǡ��������إå������ơ��Ƽ�°���ʤɤ��٤Ƥ򥳥ԡ�����
 *  ��������(���ԡ���) ������ϲ��Ѥ���ޤ���<br>
 *  dest_img �˼��Ȥ�Ϳ������硤trimf()��¹Ԥ�������Ʊ����̤ˤʤ�ޤ���
 *
 * @param   dest_img ���ԡ���Υ��֥�������
 * @param   exp_fmt  csv�������ϰϻ���ʸ����Τ���Υե����ޥåȻ���
 * @param   ap       exp_fmt���б���������Ĺ�����Υꥹ��
 * @note    "[1:100,*]" �Τ褦�� [] �ǰϤ�ȡ�1-indexed �Ȥ��ư����ޤ���
 * @throw   �Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
 */
void fits_image::vcopyf( fits_image *dest_img, 
			 const char *exp_fmt, va_list ap ) const
{
    if ( dest_img == NULL ) goto quit;

    /* ���: ���ԡ��� dest_img �����Ȥξ��Ǥ�Ԥʤ��� */

    /* ���η����ۤʤ��� */
    if ( dest_img->type() != this->type() ) {
	dest_img->_init_all_data_recs(this->data_array().size_type());
	dest_img->type_rec = this->type_rec;
    }

    this->data_array().vcopyf( dest_img->data_rec, exp_fmt, ap );

    if ( dest_img != this ) {
	/* bzero, bscale �ʤɤ�°���򥳥ԡ� */
	dest_img->init_properties(*this);
	/* �إå��򥳥ԡ� */
	dest_img->header_init( this->header() );
    }

 quit:
    return;
}

/* trim a section                 */
/* Flipping elements is supported */
/**
 * @brief  ������ʬ�ʳ����������Ǥξõ� (�����ǡ����Υȥ�ߥ󥰡�IDL�ε�ˡ)
 *
 *  ���Ȥβ����ǡ�������Τ������ꤵ�줿��ʬ������Ĥ������곰����ʬ��õ�
 *  ���ޤ���<br>
 *  �ڤ�Ф���뤵����ϰϤ�Ǥ�դμ����ޤǻ����ǽ�� "0:99,*" �Τ褦�� IDL ��
 *  IRAF ��Ʊ�ͤ�ʸ����ǻ��ꤷ�ޤ�������ʸ������������ֹ�� 0-indexed �Ǥ���
 *
 * @param   exp_fmt  csv�������ϰϻ���ʸ����Τ���Υե����ޥåȻ���
 * @param   ...      exp_fmt���б���������Ĺ�����γ����ǥǡ���
 * @return  ���Ȥλ���
 * @note    "[1:100,*]" �Τ褦�� [] �ǰϤ�ȡ�1-indexed �Ȥ��ư����ޤ���
 * @throw   �Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
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
 * @brief  ������ʬ�ʳ����������Ǥξõ� (�����ǡ����Υȥ�ߥ󥰡�IDL�ε�ˡ)
 *
 *  ���Ȥβ����ǡ�������Τ������ꤵ�줿��ʬ������Ĥ������곰����ʬ��õ�
 *  ���ޤ���<br>
 *  �ڤ�Ф���뤵����ϰϤ�Ǥ�դμ����ޤǻ����ǽ�� "0:99,*" �Τ褦�� IDL ��
 *  IRAF ��Ʊ�ͤ�ʸ����ǻ��ꤷ�ޤ�������ʸ������������ֹ�� 0-indexed �Ǥ���
 *
 * @param   exp_fmt  csv�������ϰϻ���ʸ����Τ���Υե����ޥåȻ���
 * @param   ap       exp_fmt���б���������Ĺ�����Υꥹ��
 * @return  ���Ȥλ���
 * @note    "[1:100,*]" �Τ褦�� [] �ǰϤ�ȡ�1-indexed �Ȥ��ư����ޤ���
 * @throw   �Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
 */
fits_image &fits_image::vtrimf( const char *exp_fmt, va_list ap )
{
    this->vcopyf(this, exp_fmt, ap);
    return *this;
}

/* flip elements in a section */
/**
 * @brief  Ǥ�դμ���(ʣ�����)�ǲ����ǡ���������¤Ӥ�ȿž (IDL�ε�ˡ)
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
 * @brief  Ǥ�դμ���(ʣ�����)�ǲ����ǡ���������¤Ӥ�ȿž (IDL�ε�ˡ)
 */
fits_image &fits_image::vflipf( const char *exp_fmt, va_list ap )
{
    this->data_array().vflipf(exp_fmt,ap);
    return *this;
}

/* interchange rows and columns and copy */
/* Flipping elements is supported        */
/**
 * @brief  (x,y)�Υȥ�󥹥ݡ�����Ԥʤä������ǡ����������� (IDL�ε�ˡ)
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
 * @brief  (x,y)�Υȥ�󥹥ݡ�����Ԥʤä������ǡ����������� (IDL�ε�ˡ)
 */
void fits_image::vtransposef_xy_copy( fits_image *dest_img, 
				      const char *exp_fmt, va_list ap ) const
{
    if ( dest_img == NULL ) goto quit;

    /* ���: dest_img �����Ȥξ��Ǥ�Ԥʤ��� */

    /* ���η����ۤʤ��� */
    if ( dest_img->type() != this->type() ) {
	dest_img->_init_all_data_recs(this->data_array().size_type());
	dest_img->type_rec = this->type_rec;
    }

    this->data_array().vtransposef_xy_copy( dest_img->data_rec, exp_fmt, ap );

    if ( dest_img != this ) {
	/* bzero, bscale �ʤɤ�°���򥳥ԡ� */
	dest_img->init_properties(*this);
	/* �إå��򥳥ԡ� */
	dest_img->header_init( this->header() );
    }

 quit:
    return;
}

/* interchange xyz to zxy and copy */
/* Flipping elements is supported  */
/**
 * @brief  (x,y,z)��(z,x,y)�Υȥ�󥹥ݡ�����Ԥʤä��������� (IDL�ε�ˡ)
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
 * @brief  (x,y,z)��(z,x,y)�Υȥ�󥹥ݡ�����Ԥʤä��������� (IDL�ε�ˡ)
 */
void fits_image::vtransposef_xyz2zxy_copy( fits_image *dest_img, 
				      const char *exp_fmt, va_list ap ) const
{
    if ( dest_img == NULL ) goto quit;

    /* ���: dest_img �����Ȥξ��Ǥ�Ԥʤ��� */

    /* ���η����ۤʤ��� */
    if ( dest_img->type() != this->type() ) {
	dest_img->_init_all_data_recs(this->data_array().size_type());
	dest_img->type_rec = this->type_rec;
    }

    this->data_array().vtransposef_xyz2zxy_copy(dest_img->data_rec, exp_fmt, ap);

    if ( dest_img != this ) {
	/* bzero, bscale �ʤɤ�°���򥳥ԡ� */
	dest_img->init_properties(*this);
	/* �إå��򥳥ԡ� */
	dest_img->header_init( this->header() );
    }

 quit:
    return;
}

/* padding existing values in an array */
/**
 * @brief  ���Ȥβ����ǡ��������ǥե�����ͤǥѥǥ��� (IDL�ε�ˡ)
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
 * @brief  ���Ȥβ����ǡ��������ǥե�����ͤǥѥǥ��� (IDL�ε�ˡ)
 */
fits_image &fits_image::vcleanf( const char *exp_fmt, va_list ap )
{
    this->data_array().vcleanf(exp_fmt,ap);
    return *this;
}

/* rewrite element values with a value in a section */
/**
 * @brief  ���Ȥβ����ǡ����������ꤵ�줿�����顼�ͤǽ񤭴��� (IDL�ε�ˡ)
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
 * @brief  ���Ȥβ����ǡ����������ꤵ�줿�����顼�ͤǽ񤭴��� (IDL�ε�ˡ)
 */
fits_image &fits_image::vfillf( double value, 
				const char *exp_fmt, va_list ap )
{
    return this->image_vfillf_fast( value, exp_fmt, ap );
}

/**
 * @brief  ���ȤΥǡ�������򥹥��顼�ͤǽ񤭴��� (�桼���ؿ���ͳ��IDL�ε�ˡ)
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
 * @brief  ���ȤΥǡ�������򥹥��顼�ͤǽ񤭴��� (�桼���ؿ���ͳ��IDL�ε�ˡ)
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
 * @brief  ���Ȥβ����ǡ�������ˡ����ꤵ�줿�����顼�ͤǲû� (IDL�ε�ˡ)
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
 * @brief  ���Ȥβ����ǡ�������ˡ����ꤵ�줿�����顼�ͤǲû� (IDL�ε�ˡ)
 */
fits_image &fits_image::vaddf( double value, 
			       const char *exp_fmt, va_list ap )
{
    return this->image_vfillf( value, &add_func, NULL, exp_fmt, ap );
}

/* subtract a scalar value from element values in a section */
/**
 * @brief  ���Ȥβ����ǡ�������򡤻��ꤵ�줿�����顼�ͤǸ��� (IDL�ε�ˡ)
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
 * @brief  ���Ȥβ����ǡ�������򡤻��ꤵ�줿�����顼�ͤǸ��� (IDL�ε�ˡ)
 */
fits_image &fits_image::vsubtractf( double value, 
				    const char *exp_fmt, va_list ap )
{
    return this->image_vfillf( value, &subtract_func, NULL, exp_fmt, ap );
}

/* multiply element values in a section by a scalar value */
/**
 * @brief  ���Ȥβ����ǡ�������ˡ����ꤵ�줿�����顼�ͤ�軻 (IDL�ε�ˡ)
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
 * @brief  ���Ȥβ����ǡ�������ˡ����ꤵ�줿�����顼�ͤ�軻 (IDL�ε�ˡ)
 */
fits_image &fits_image::vmultiplyf( double value, 
				    const char *exp_fmt, va_list ap )
{
    return this->image_vfillf( value, &multiply_func, NULL, exp_fmt, ap );
}

/* divide element values in a section by a scalar value */
/**
 * @brief  ���Ȥβ����ǡ�������򡤻��ꤵ�줿�����顼�ͤǽ��� (IDL�ε�ˡ)
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
 * @brief  ���Ȥβ����ǡ�������򡤻��ꤵ�줿�����顼�ͤǽ��� (IDL�ε�ˡ)
 */
fits_image &fits_image::vdividef( double value, 
				  const char *exp_fmt, va_list ap )
{
    return this->image_vfillf( value, &divide_func, NULL, exp_fmt, ap );
}

/* paste up an array object                                           */

/* paste without operation using fast method of that of .convert().   */
/* �黻�ʤ��ڡ�����: convert() ��Ʊ����ˡ���Ѵ�������̤��®�˳�Ǽ�� */
/**
 * @brief  ���ȤΥǡ�������˻��ꤵ�줿���֥������Ȥ������Ž���դ� (IDL�ε�ˡ)
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
 * @brief  ���ȤΥǡ�������˻��ꤵ�줿���֥������Ȥ������Ž���դ� (IDL�ε�ˡ)
 */
fits_image &fits_image::vpastef( const fits_image &src,
				 const char *exp_fmt, va_list ap )
{
    return this->image_vpastef_fast(src, exp_fmt, ap);
}

/* paste with operation: all elements are converted into double type, so */
/* the performance is inferior to above paste().                         */
/* �黻����ڡ�����: ��ö double �����Ѵ�����Ƥ���黻������̤��Ǽ��  */
/* ���������α黻�ʤ��Ǥ���٤��٤���                                    */
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
 * @brief  ���Ȥ�����˻��ꤵ�줿�����Ž���դ� (�桼���ؿ���ͳ��IDL�ε�ˡ)
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
 * @brief  ���Ȥ�����˻��ꤵ�줿�����Ž���դ� (�桼���ؿ���ͳ��IDL�ε�ˡ)
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
 * @brief  ���ȤΥǡ�������ˡ����ꤵ�줿���֥������Ȥ������û� (IDL�ε�ˡ)
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
 * @brief  ���ȤΥǡ�������ˡ����ꤵ�줿���֥������Ȥ������û� (IDL�ε�ˡ)
 */
fits_image &fits_image::vaddf( const fits_image &src_img, 
			       const char *exp_fmt, va_list ap )
{
    return this->image_vpastef(src_img, &add_func_n, NULL, exp_fmt, ap);
}

/* subtract an array object */
/**
 * @brief  ���ȤΥǡ�������򡤻��ꤵ�줿���֥������Ȥ�����Ǹ��� (IDL�ε�ˡ)
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
 * @brief  ���ȤΥǡ�������򡤻��ꤵ�줿���֥������Ȥ�����Ǹ��� (IDL�ε�ˡ)
 */
fits_image &fits_image::vsubtractf( const fits_image &src_img, 
				    const char *exp_fmt, va_list ap )
{
    return this->image_vpastef(src_img, &subtract_func_n, NULL, exp_fmt, ap);
}

/* multiply an array object */
/**
 * @brief  ���ȤΥǡ�������ˡ����ꤵ�줿���֥������Ȥ������軻 (IDL�ε�ˡ)
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
 * @brief  ���ȤΥǡ�������ˡ����ꤵ�줿���֥������Ȥ������軻 (IDL�ε�ˡ)
 */
fits_image &fits_image::vmultiplyf( const fits_image &src_img, 
				    const char *exp_fmt, va_list ap )
{
    return this->image_vpastef(src_img, &multiply_func_n, NULL, exp_fmt, ap);
}

/* divide an array object */
/**
 * @brief  ���ȤΥǡ�������򡤻��ꤵ�줿���֥������Ȥ�����ǽ��� (IDL�ε�ˡ)
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
 * @brief  ���ȤΥǡ�������򡤻��ꤵ�줿���֥������Ȥ�����ǽ��� (IDL�ε�ˡ)
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
 * @brief  �����ǡ�������ΰ������Ǥ��ڤ�Ф����ƥ�ݥ�ꥪ�֥������ȤǼ���
 *
 *  ���Ȥβ����ǡ�������Τ������ꤵ�줿��ʬ������ƥ�ݥ�ꥪ�֥������Ȥ�
 *  ���ԡ�����������֤��ޤ���
 *
 * @param     col_index ���ԡ����������
 * @param     col_size ���ԡ������󥵥���
 * @param     row_index ���ԡ����ι԰���
 * @param     row_size ���ԡ����ιԥ�����
 * @param     layer_index ���ԡ����Υ쥤�����
 * @param     layer_size ���ԡ����Υ쥤�䥵����
 * @return    �ƥ�ݥ�ꥪ�֥�������
 * @throw     �Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
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
 * @brief  �����ǡ�������ΰ������Ǥ��̥��֥������Ȥإ��ԡ�
 *
 *  ���Ȥβ����ǡ�����������Ƥΰ���ʬ����ꤵ�줿���֥������� dest_img ��
 *  ���ԡ����ޤ���<br>
 *  ���ԡ���ؤϡ������ǡ��������إå������ơ��Ƽ�°���ʤɤ��٤Ƥ򥳥ԡ�����
 *  ��������(���ԡ���) ������ϲ��Ѥ���ޤ���<br>
 *  dest_img �˼��Ȥ�Ϳ������硤trim()��¹Ԥ�������Ʊ����̤ˤʤ�ޤ���
 *
 * @param     dest_img ���ԡ���Υ��֥�������
 * @param     col_index ���ԡ����������
 * @param     col_size ���ԡ������󥵥���
 * @param     row_index ���ԡ����ι԰���
 * @param     row_size ���ԡ����ιԥ�����
 * @param     layer_index ���ԡ����Υ쥤�����
 * @param     layer_size ���ԡ����Υ쥤�䥵����
 * @throw     �Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
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

    /* ���: dest_img �����Ȥξ��Ǥ�Ԥʤ��� */

    /* ���η����ۤʤ��� */
    if ( dest_img->type() != this->type() ) {
	dest_img->_init_all_data_recs(this->data_array().size_type());
	dest_img->type_rec = this->type_rec;
    }

    this->data_array().copy( dest_img->data_rec, col_index, z_col_size, 
			  row_index, z_row_size, layer_index, z_layer_size );

    if ( dest_img != this ) {
	/* bzero, bscale �ʤɤ�°���򥳥ԡ� */
	dest_img->init_properties(*this);
	/* �إå��򥳥ԡ� */
	dest_img->header_init( this->header() );
    }

 quit:
    return;
}

/* set a section to be copied by move_to() */
/**
 * @brief  ����� move_to() �ˤ������ǡ������ư�������ΰ�λ���
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
 * @brief  move_from() �ǻ��ꤵ�줿�����ǡ����ΰ�ΰ�ư
 */
fits_image &fits_image::move_to(long dest_col, long dest_row, long dest_layer)
{
    this->data_array().move_to(dest_col, dest_row, dest_layer);
    return *this;
}

/* trim a section */
/**
 * @brief  ������ʬ�ʳ��β����ǡ����������Ǥξõ� (�����ǡ����Υȥ�ߥ�)
 *
 *  ���Ȥβ����ǡ�������Τ������ꤵ�줿��ʬ������Ĥ������곰����ʬ��
 *  �õ�ޤ���<br>
 *
 * @param     col_idx �����
 * @param     col_len �󥵥���
 * @param     row_idx �԰���
 * @param     row_len �ԥ�����
 * @param     layer_idx �쥤�����
 * @param     layer_len �쥤�䥵����
 * @return    ���Ȥλ���
 * @throw     �Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
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
 * @brief  �����ǡ�������ο�ʿ�����Ǥ�ȿž
 *
 *  ���Ȥβ����ǡ�������λ��ꤵ�줿�ϰϤ����ǤˤĤ��ơ��������Ƥ��ʿ������
 *  ȿž�����ޤ���
 *
 * @param     col_idx �����
 * @param     col_len �󥵥���
 * @param     row_idx �԰���
 * @param     row_len �ԥ�����
 * @param     layer_idx �쥤�����
 * @param     layer_len �쥤�䥵����
 * @return    ���Ȥλ���
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
 * @brief  �����ǡ�������ο�ľ�����Ǥ�ȿž
 *
 *  ���Ȥβ����ǡ�������λ��ꤵ�줿�ϰϤ����ǤˤĤ��ơ��������Ƥ��ľ������
 *  ȿž�����ޤ���
 *
 * @param     col_idx �����
 * @param     col_len �󥵥���
 * @param     row_idx �԰���
 * @param     row_len �ԥ�����
 * @param     layer_idx �쥤�����
 * @param     layer_len �쥤�䥵����
 * @return    ���Ȥλ���
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
 * @brief  (x,y)�ǤΥȥ�󥹥ݡ�����Ԥʤä������ǡ��������
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

    /* ���: dest_img �����Ȥξ��Ǥ�Ԥʤ��� */

    /* ���η����ۤʤ��� */
    if ( dest_img->type() != this->type() ) {
	dest_img->_init_all_data_recs(this->data_array().size_type());
	dest_img->type_rec = this->type_rec;
    }

    this->data_array().transpose_xy_copy(dest_img->data_rec, 
	    col_idx, z_col_size, row_idx, z_row_size, layer_idx, z_layer_size);

    if ( dest_img != this ) {
	/* bzero, bscale �ʤɤ�°���򥳥ԡ� */
	dest_img->init_properties(*this);
	/* �إå��򥳥ԡ� */
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
 * @brief  (x,y,z)��(z,x,y)�Υȥ�󥹥ݡ�����Ԥʤä������ǡ��������
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

    /* ���: dest_img �����Ȥξ��Ǥ�Ԥʤ��� */

    /* ���η����ۤʤ��� */
    if ( dest_img->type() != this->type() ) {
	dest_img->_init_all_data_recs(this->data_array().size_type());
	dest_img->type_rec = this->type_rec;
    }

    this->data_array().transpose_xyz2zxy_copy(dest_img->data_rec, 
	    col_idx, z_col_size, row_idx, z_row_size, layer_idx, z_layer_size);

    if ( dest_img != this ) {
	/* bzero, bscale �ʤɤ�°���򥳥ԡ� */
	dest_img->init_properties(*this);
	/* �إå��򥳥ԡ� */
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
 * @brief  (x,y)�ǲ�ž�����������ǡ�������� (��ž��90��ñ��)
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

    /* ���: dest_img �����Ȥξ��Ǥ�Ԥʤ��� */

    /* ���η����ۤʤ��� */
    if ( dest_img->type() != this->type() ) {
	dest_img->_init_all_data_recs(this->data_array().size_type());
	dest_img->type_rec = this->type_rec;
    }

    this->data_array().rotate_xy_copy(dest_img->data_rec, angle,
	    col_idx, z_col_size, row_idx, z_row_size, layer_idx, z_layer_size);

    if ( dest_img != this ) {
	/* bzero, bscale �ʤɤ�°���򥳥ԡ� */
	dest_img->init_properties(*this);
	/* �إå��򥳥ԡ� */
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
 * @brief  ���Ȥβ����ǡ��������ǥե�����ͤǥѥǥ���
 *
 *  ���Ȥβ����ǡ�����������Ǥ�ǥե�����ͤǥѥǥ��󥰤��ޤ���<br>
 *  �����ϻ��ꤷ�ʤ��Ƥ���ѤǤ��ޤ������ξ��ϡ������Ǥ������оݤǤ���
 *  clean() ��¹Ԥ��Ƥ�����Ĺ���Ѳ����ޤ���
 *
 * @param     col_idx �����
 * @param     col_len �󥵥���
 * @param     row_idx �԰���
 * @param     row_len �ԥ�����
 * @param     layer_idx �쥤�����
 * @param     layer_len �쥤�䥵����
 * @return    ���Ȥλ���
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
 * @brief  ���Ȥβ����ǡ����������ꤵ�줿�����顼�ͤǽ񤭴���
 *
 *  ���Ȥβ����ǡ�������λ��ꤵ�줿�ϰϤ����Ǥ򡤻��ꤵ�줿�ͤǽ񴹤��ޤ���
 *
 * @param     value �񤭹�����
 * @param     col_index �����
 * @param     col_size �󥵥���
 * @param     row_index �԰���
 * @param     row_size �ԥ�����
 * @param     layer_index �쥤�����
 * @param     layer_size �쥤�䥵����
 * @return    ���Ȥλ���
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
 * @brief  ���Ȥβ����ǡ����������ꤵ�줿�����顼�ͤǽ񤭴���(�桼���ؿ���ͳ)
 *
 *  ���Ȥβ����ǡ�������λ��ꤵ�줿�ϰϤ����Ǥ򡤥桼������ؿ���ͳ�ǽ񴹤�
 *  �ޤ���
 *
 * @param      value �񤭹�����
 * @param      func ���Ѵ��ΰ٤Υ桼���ؿ��Υ��ɥ쥹
 * @param      user_ptr func �κǸ��Ϳ������桼���Υݥ���
 * @param      col_index �����
 * @param      col_size �󥵥���
 * @param      row_index �԰���
 * @param      row_size �ԥ�����
 * @param      layer_index �쥤�����
 * @param      layer_size �쥤�䥵����
 * @return     ���Ȥλ���
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
 * @brief  ���Ȥβ����ǡ�������ˡ����ꤵ�줿�����顼�ͤ�û�
 *
 *  ���Ȥβ����ǡ�������λ��ꤵ�줿�ϰϤ����Ǥ� value ��û����ޤ���
 *
 * @param     value �û�������
 * @param     col_index �����
 * @param     col_size �󥵥���
 * @param     row_index �԰���
 * @param     row_size �ԥ�����
 * @param     layer_index �쥤�����
 * @param     layer_size �쥤�䥵����
 * @return    ���Ȥλ���
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
 * @brief  ���Ȥβ����ǡ�������򡤻��ꤵ�줿�����顼�ͤǸ���
 *
 *  ���Ȥβ����ǡ�������λ��ꤵ�줿�ϰϤ����Ǥ��� value �򸺻����ޤ���
 *
 * @param     value ����������
 * @param     col_index �����
 * @param     col_size �󥵥���
 * @param     row_index �԰���
 * @param     row_size �ԥ�����
 * @param     layer_index �쥤�����
 * @param     layer_size �쥤�䥵����
 * @return    ���Ȥλ���
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
 * @brief  ���Ȥβ����ǡ�������ˡ����ꤵ�줿�����顼�ͤ�軻
 *
 *  ���Ȥβ����ǡ�������λ��ꤵ�줿�ϰϤ����Ǥ��ͤ� value ��軻���ޤ���
 *
 * @param     value �軻������
 * @param     col_index �����
 * @param     col_size �󥵥���
 * @param     row_index �԰���
 * @param     row_size �ԥ�����
 * @param     layer_index �쥤�����
 * @param     layer_size �쥤�䥵����
 * @return    ���Ȥλ���
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
 * @brief  ���Ȥβ����ǡ�������򡤻��ꤵ�줿�����顼�ͤǽ���
 *
 *  ���Ȥβ����ǡ�������λ��ꤵ�줿�ϰϤ����Ǥ��ͤˤĤ��� value �ǽ������ޤ���
 *
 * @param     value ����������
 * @param     col_index �����
 * @param     col_size �󥵥���
 * @param     row_index �԰���
 * @param     row_size �ԥ�����
 * @param     layer_index �쥤�����
 * @param     layer_size �쥤�䥵����
 * @return    ���Ȥλ���
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
 * @brief  ���Ȥβ����ǡ�������˻��ꤵ�줿���֥������Ȥ������Ž���դ�
 *
 *  ���Ȥβ����ǡ�������λ��ꤵ�줿�ϰϤ������ͤˡ�src_img �ǻ��ꤵ�줿
 *  ���֥������Ȥγ������ͤ�Ž���դ��ޤ���
 *
 * @param     src_img �����Ȥʤ��������ĥ��֥�������
 * @param     dest_col �����
 * @param     dest_row �԰���
 * @param     dest_layer �쥤�����
 * @return    ���Ȥλ���
 */
fits_image &fits_image::paste( const fits_image &src_img,
			       long dest_col, long dest_row, long dest_layer )
{
    return this->image_paste_fast(src_img, dest_col, dest_row, dest_layer);
}

/**
 * @brief  ���Ȥ�����˻��ꤵ�줿���֥������Ȥ������Ž���դ� (�桼���ؿ���ͳ)
 *
 *  ���Ȥβ����ǡ�������λ��ꤵ�줿�ϰϤ������ͤˡ�src_img �ǻ��ꤵ�줿
 *  ���֥������Ȥ� �������ͤ�桼������ؿ���ͳ��Ž���դ��ޤ����桼������ؿ�
 *  ��Ϳ����Ž���դ����ε�ư���Ѥ��뤳�Ȥ��Ǥ��ޤ���<br>
 *  �桼������ؿ� func �ΰ����ˤϽ�ˡ����Ȥ�������(����)�����֥������� 
 *  src_img ��������(����)���ǽ��2�Ĥΰ���������θĿ�������֡��԰��֡�
 *  �쥤����֡����ȤΥ��֥������ȤΥ��ɥ쥹���桼���ݥ��� user_ptr ����
 *  ��Ϳ�����ޤ���
 *
 * @param     src_img �����Ȥʤ��������ĥ��֥�������
 * @param     func ���Ѵ��Τ���Υ桼���ؿ��Υ��ɥ쥹
 * @param     user_ptr func �κǸ��Ϳ������桼���Υݥ���
 * @param     dest_col �����
 * @param     dest_row �԰���
 * @param     dest_layer �쥤�����
 * @return    ���Ȥλ���
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
 * @brief  ���Ȥβ����ǡ�������ˡ����ꤵ�줿���֥������Ȥ������û�
 *
 *  ���Ȥβ����ǡ�����������Ǥ˥��֥������� src_img �����������û����ޤ���
 *  �󡦹ԡ��쥤��ˤĤ��Ƥ��줾��βû�Ŭ�ѳ��ϰ��֤����Ǥ��ޤ���
 *
 * @param     src_img �黻�˻Ȥ��������ĥ��֥�������
 * @param     dest_col �û����ϰ���(��)
 * @param     dest_row �û����ϰ���(��)
 * @param     dest_layer �û����ϰ���(�쥤��)
 * @return    ���Ȥλ���
 */
fits_image &fits_image::add( const fits_image &src_img,
			     long dest_col, long dest_row, long dest_layer )
{
    return this->image_paste(src_img,&add_func_n,NULL,
			     dest_col,dest_row,dest_layer);
}

/**
 * @brief  ���Ȥβ����ǡ�������򡤻��ꤵ�줿���֥������Ȥ�����Ǹ���
 *
 *  ���Ȥβ����ǡ�������������ͤ��饪�֥������� src_img ����������������ͤ�
 *  �������ޤ���
 *  �󡦹ԡ��쥤��ˤĤ��Ƥ��줾��θ���Ŭ�ѳ��ϰ��֤����Ǥ��ޤ���
 *
 * @param     src_img �黻�˻Ȥ��������ĥ��֥�������
 * @param     dest_col �������ϰ���(��)
 * @param     dest_row �������ϰ���(��)
 * @param     dest_layer �������ϰ���(�쥤��)
 * @return    ���Ȥλ���
 */
fits_image &fits_image::subtract( const fits_image &src_img,
				long dest_col, long dest_row, long dest_layer )
{
    return this->image_paste(src_img,&subtract_func_n,NULL,
			     dest_col,dest_row,dest_layer);
}

/**
 * @brief  ���Ȥβ����ǡ�������ˡ����ꤵ�줿���֥������Ȥ������軻
 *
 *  ���Ȥβ����ǡ�������������ͤ˥��֥������� src_img �����������軻���ޤ���
 *  �󡦹ԡ��쥤��ˤĤ��Ƥ��줾��ξ軻Ŭ�ѳ��ϰ��֤����Ǥ��ޤ���
 *
 * @param     src_img �黻�˻Ȥ��������ĥ��֥�������
 * @param     dest_col �軻���ϰ���(��)
 * @param     dest_row �軻���ϰ���(��)
 * @param     dest_layer �軻���ϰ���(�쥤��)
 * @return    ���Ȥλ���
 */
fits_image &fits_image::multiply( const fits_image &src_img,
				long dest_col, long dest_row, long dest_layer )
{
    return this->image_paste(src_img,&multiply_func_n,NULL,
			     dest_col,dest_row,dest_layer);
}

/**
 * @brief  ���Ȥβ����ǡ�������򡤻��ꤵ�줿���֥������Ȥ�����ǽ���
 *
 *  ���Ȥβ����ǡ�������������ͤ��饪�֥������� src_img ��������������
 *  ���ޤ���
 *  �󡦹ԡ��쥤��ˤĤ��Ƥ��줾��ν���Ŭ�ѳ��ϰ��֤����Ǥ��ޤ���
 *
 * @param     src_img �黻�˻Ȥ��������ĥ��֥�������
 * @param     dest_col �������ϰ���(��)
 * @param     dest_row �������ϰ���(��)
 * @param     dest_layer �������ϰ���(�쥤��)
 * @return    ���Ȥλ���
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
 * @brief  section() ���ΰ�����Ĵ�٤�Ŭ�ڤʺ�ɸ�Υѥ�᡼��������
 * 
 *  �����κ�ɸ�����������֤򼨤��Ƥ뤫��Ƚ�Ǥ���
 *  �������ʤ����(�Ϥ߽Ф��Ƥ�����)�Ϻ�ɸ�Υѥ�᡼����Ĵ�����ޤ���
 * 
 * @param   r_col_index �����
 * @param   r_col_size �󥵥���
 * @param   r_row_index �԰���
 * @param   r_row_size �ԥ�����
 * @param   r_layer_index �쥤�����
 * @param   r_layer_size �쥤�䥵����
 * @return  ���������Ȥ�������ΰ���򼨤��Ƥ�������0��
 *          �ΰ褫��Ϥ߽Ф��Ƥ��뤬ͭ���ΰ褬¸�ߤ�����������͡�
 *          ͭ���ΰ褬̵���������͡�
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
    /* ���饤���Ȥ����(sizeof��Ĺ����Τ�����!) */
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
 * @brief  scan�ϥ��дؿ��Τ���ν���
 *
 * @note   ���Υ��дؿ��� private �Ǥ���
 */
void fits_image::prepare_scan_a_section() const
{
    fits_image *thisp = (fits_image *)this;		/* ����Ū�˥��㥹�� */
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
 * @brief  �����ǡ����������Ǥ�x������ʬ��ǤΥ�����󡦥������󥹤γ���
 *
 * ���Ȥβ����ǡ�������λ��ꤵ�줿�ϰϤ����Ǥ򡤹�ñ�̤ǥ������(�饹����
 * �������)���뤿��ν����򤷤ޤ�����ñ�̤Υ�������Ԥʤ���硤���μ���
 * ��Ϣ��API��Ȥ��ޤ���<br>
 * - beginf_scan_along_x() �ǽ����򤹤롥<br>
 * - scan_along_x() ��Ԥο������ƤӽФ����Ԥ��Ȥν�����Ԥʤ���<br>
 * - end_scan_along_x() �ǽ�λ����(����Хåե��γ���)��Ԥʤ���<br>
 * scan_along_x() ��1��ʬ�����Ǥ����ä�����Хåե��Υ��ɥ쥹(32�Х��Ȥǥ��饤
 * �󤵤�Ƥ���)���֤��ޤ������ΰ���Хåե��ϥ��֥������Ȥ������������Ǥ��ä�
 * �� double �����󶡤���ޤ����ʤ������ΥХåե��ϥ��֥������ȤǴ�������Ƥ���
 * ���ᡤ�ץ���ޤ��������ƤϤ����ޤ���<br>
 * scan_along_x() �ϻ����ϰ����ƤΥ�����󤬴�λ���Ƥ�����ˤ� NULL ���֤���
 * ����<br>
 * �����ǡ��������פ�Ȥ���ʤɤ˻Ȥ����дؿ��Ǥ���
 *
 * @param     exp_fmt ������󤹤��ϰϤ򼨤�ʸ����(��: "0:99,*")�Τ����
 *                    �ե����ޥåȻ���(printf��Ʊ��)
 * @param     ... exp_fmt���б���������Ĺ�����γ����ǥǡ���
 * @return    �������ˤ��ͭ�����Ǥο������顼�ξ������͡�
 * @note      "[1:100,*]" �Τ褦�� [] �ǰϤ�ȡ�1-indexed �Ȥ��ư����ޤ���
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
 * @brief  �����ǡ����������Ǥ�x������ʬ��ǤΥ�����󡦥������󥹤γ���
 *
 * ���Ȥβ����ǡ�������λ��ꤵ�줿�ϰϤ����Ǥ򡤹�ñ�̤ǥ������(�饹����
 * �������)���뤿��ν����򤷤ޤ�����ñ�̤Υ�������Ԥʤ���硤���μ���
 * ��Ϣ��API��Ȥ��ޤ���<br>
 * - beginf_scan_along_x() �ǽ����򤹤롥<br>
 * - scan_along_x() ��Ԥο������ƤӽФ����Ԥ��Ȥν�����Ԥʤ���<br>
 * - end_scan_along_x() �ǽ�λ����(����Хåե��γ���)��Ԥʤ���<br>
 * scan_along_x() ��1��ʬ�����Ǥ����ä�����Хåե��Υ��ɥ쥹(32�Х��Ȥǥ��饤
 * �󤵤�Ƥ���)���֤��ޤ������ΰ���Хåե��ϥ��֥������Ȥ������������Ǥ��ä�
 * �� double �����󶡤���ޤ����ʤ������ΥХåե��ϥ��֥������ȤǴ�������Ƥ���
 * ���ᡤ�ץ���ޤ��������ƤϤ����ޤ���<br>
 * scan_along_x() �ϻ����ϰ����ƤΥ�����󤬴�λ���Ƥ�����ˤ� NULL ���֤���
 * ����<br>
 * �����ǡ��������פ�Ȥ���ʤɤ˻Ȥ����дؿ��Ǥ���
 *
 * @param     exp_fmt ������󤹤��ϰϤ򼨤�ʸ����(��: "0:99,*")�Τ����
 *                    �ե����ޥåȻ���(printf��Ʊ��)
 * @param     ap exp_fmt���б���������Ĺ�����Υꥹ��
 * @return    �������ˤ��ͭ�����Ǥο������顼�ξ������͡�
 * @note      "[1:100,*]" �Τ褦�� [] �ǰϤ�ȡ�1-indexed �Ȥ��ư����ޤ���
 */
long fits_image::vbeginf_scan_along_x( const char *exp_fmt, va_list ap ) const
{
    fits_image *thisp = (fits_image *)this;		/* ����Ū�˥��㥹�� */

    thisp->prepare_scan_a_section();
    thisp->scan_use_flags |= Bit_use_along_x;

    return this->data_array().vbeginf_scan_along_x( exp_fmt, ap );
}

/**
 * @brief  �����ǡ����������Ǥ�x������ʬ��ǤΥ�����󡦥������󥹤γ���
 *
 * ���Ȥβ����ǡ�������λ��ꤵ�줿�ϰϤ����Ǥ򡤹�ñ�̤ǥ������(�饹����
 * �������)���뤿��ν����򤷤ޤ�����ñ�̤Υ�������Ԥʤ���硤���μ���
 * ��Ϣ��API��Ȥ��ޤ���<br>
 * - begin_scan_along_x() �ǽ����򤹤롥<br>
 * - scan_along_x() ��Ԥο������ƤӽФ����Ԥ��Ȥν�����Ԥʤ���<br>
 * - end_scan_along_x() �ǽ�λ����(����Хåե��γ���)��Ԥʤ���<br>
 * scan_along_x() ��1��ʬ�����Ǥ����ä�����Хåե��Υ��ɥ쥹(32�Х��Ȥǥ��饤
 * �󤵤�Ƥ���)���֤��ޤ������ΰ���Хåե��ϥ��֥������Ȥ������������Ǥ��ä�
 * �� double �����󶡤���ޤ����ʤ������ΥХåե��ϥ��֥������ȤǴ�������Ƥ���
 * ���ᡤ�ץ���ޤ��������ƤϤ����ޤ���<br>
 * scan_along_x() �ϻ����ϰ����ƤΥ�����󤬴�λ���Ƥ�����ˤ� NULL ���֤���
 * ����<br>
 * �����ǡ��������פ�Ȥ���ʤɤ˻Ȥ����дؿ��Ǥ���
 *
 * @param     col_index �����
 * @param     col_size �󥵥���
 * @param     row_index �԰���
 * @param     row_size �ԥ�����
 * @param     layer_index �쥤�����
 * @param     layer_size �쥤�䥵����
 * @return    �������ˤ��ͭ�����Ǥο������顼�ξ������͡�
 */
long fits_image::begin_scan_along_x( long col_index, long col_size,
				     long row_index, long row_size,
				     long layer_index, long layer_size ) const
{
    fits_image *thisp = (fits_image *)this;		/* ����Ū�˥��㥹�� */
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
 * @brief  �����ǡ����������Ǥ�x������ʬ���Ϣ³Ū�˥������ (�֤��ͤ�double��)
 */
double *fits_image::scan_along_x( long *n, long *x, long *y, long *z ) const
{
    double *ret_p;
    fits_image *thisp = (fits_image *)this;		/* ����Ū�˥��㥹�� */
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
 * @brief  �����ǡ����������Ǥ�x������ʬ���Ϣ³Ū�˥������ (�֤��ͤ�float��)
 */
float *fits_image::scan_along_x_f( long *n, long *x, long *y, long *z ) const
{
    float *ret_p;
    fits_image *thisp = (fits_image *)this;		/* ����Ū�˥��㥹�� */
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
 * @brief  begin_scan_along_x() �ǳ��Ϥ���������󡦥������󥹤ν�λ
 */
void fits_image::end_scan_along_x() const
{
    fits_image *thisp = (fits_image *)this;		/* ����Ū�˥��㥹�� */
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
 * @brief  �����ǡ����������Ǥ�y������ʬ��ǤΥ�����󡦥������󥹤γ���
 *
 * ���Ȥβ����ǡ�������λ��ꤵ�줿�ϰϤ����Ǥ���ñ�̤ǥ������(��������
 * �������)���뤿��ν����򤷤ޤ��������ǥ֥�å�ñ�̤ι�®transpose(�ǡ���
 * ���쥤�ˤ�����x����y���Ȥ����촹��)��Ԥʤ����ᡤ�⤤�ѥե����ޥ󥹤������
 * �ޤ���<br>
 * ��ñ�̤Υ�������Ԥʤ���硤���μ��ǰ�Ϣ��API��Ȥ��ޤ���<br>
 * - beginf_scan_along_y() �ǽ����򤹤롥<br>
 * - scan_along_y() ����ο������ƤӽФ����󤴤Ȥν�����Ԥʤ���<br>
 * - end_scan_along_y() �ǽ�λ����(����Хåե��γ���)��Ԥʤ���<br>
 * scan_along_y() ��1��ʬ�����Ǥ����ä�����Хåե��Υ��ɥ쥹(32�Х��Ȥǥ��饤
 * �󤵤�Ƥ���)���֤��ޤ������ΰ���Хåե��ϥ��֥������Ȥ������������Ǥ��ä�
 * �� double �����󶡤���ޤ����ʤ������ΥХåե��ϥ��֥������ȤǴ�������Ƥ���
 * ���ᡤ�ץ���ޤ��������ƤϤ����ޤ���<br>
 * scan_along_y() �ϻ����ϰ����ƤΥ�����󤬴�λ���Ƥ�����ˤ� NULL ���֤���
 * ����<br>
 * �����ǡ��������פ�Ȥ���ʤɤ˻Ȥ����дؿ��Ǥ���
 *
 * @param     exp_fmt ������󤹤��ϰϤ򼨤�ʸ����(��: "0:99,*")�Τ����
 *                    �ե����ޥåȻ���(printf��Ʊ��)
 * @param     ... exp_fmt���б���������Ĺ�����γ����ǥǡ���
 * @return    �������ˤ��ͭ�����Ǥο������顼�ξ������͡�
 * @note      "[1:100,*]" �Τ褦�� [] �ǰϤ�ȡ�1-indexed �Ȥ��ư����ޤ���
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
 * @brief  �����ǡ����������Ǥ�y������ʬ��ǤΥ�����󡦥������󥹤γ���
 *
 * ���Ȥβ����ǡ�������λ��ꤵ�줿�ϰϤ����Ǥ���ñ�̤ǥ������(��������
 * �������)���뤿��ν����򤷤ޤ��������ǥ֥�å�ñ�̤ι�®transpose(�ǡ���
 * ���쥤�ˤ�����x����y���Ȥ����촹��)��Ԥʤ����ᡤ�⤤�ѥե����ޥ󥹤������
 * �ޤ���<br>
 * ��ñ�̤Υ�������Ԥʤ���硤���μ��ǰ�Ϣ��API��Ȥ��ޤ���<br>
 * - beginf_scan_along_y() �ǽ����򤹤롥<br>
 * - scan_along_y() ����ο������ƤӽФ����󤴤Ȥν�����Ԥʤ���<br>
 * - end_scan_along_y() �ǽ�λ����(����Хåե��γ���)��Ԥʤ���<br>
 * scan_along_y() ��1��ʬ�����Ǥ����ä�����Хåե��Υ��ɥ쥹(32�Х��Ȥǥ��饤
 * �󤵤�Ƥ���)���֤��ޤ������ΰ���Хåե��ϥ��֥������Ȥ������������Ǥ��ä�
 * �� double �����󶡤���ޤ����ʤ������ΥХåե��ϥ��֥������ȤǴ�������Ƥ���
 * ���ᡤ�ץ���ޤ��������ƤϤ����ޤ���<br>
 * scan_along_y() �ϻ����ϰ����ƤΥ�����󤬴�λ���Ƥ�����ˤ� NULL ���֤���
 * ����<br>
 * �����ǡ��������פ�Ȥ���ʤɤ˻Ȥ����дؿ��Ǥ���
 *
 * @param     exp_fmt ������󤹤��ϰϤ򼨤�ʸ����(��: "0:99,*")�Τ����
 *                    �ե����ޥåȻ���(printf��Ʊ��)
 * @param     ap exp_fmt���б���������Ĺ�����Υꥹ��
 * @return    �������ˤ��ͭ�����Ǥο������顼�ξ������͡�
 * @note      "[1:100,*]" �Τ褦�� [] �ǰϤ�ȡ�1-indexed �Ȥ��ư����ޤ���
 */
long fits_image::vbeginf_scan_along_y( const char *exp_fmt, va_list ap ) const
{
    fits_image *thisp = (fits_image *)this;		/* ����Ū�˥��㥹�� */

    thisp->prepare_scan_a_section();
    thisp->scan_use_flags |= Bit_use_along_y;

    return this->data_array().vbeginf_scan_along_y( exp_fmt, ap );
}

/**
 * @brief  �����ǡ����������Ǥ�y������ʬ��ǤΥ�����󡦥������󥹤γ���
 *
 * ���Ȥβ����ǡ�������λ��ꤵ�줿�ϰϤ����Ǥ���ñ�̤ǥ������(��������
 * �������)���뤿��ν����򤷤ޤ��������ǥ֥�å�ñ�̤ι�®transpose(�ǡ���
 * ���쥤�ˤ�����x����y���Ȥ����촹��)��Ԥʤ����ᡤ�⤤�ѥե����ޥ󥹤������
 * �ޤ���<br>
 * ��ñ�̤Υ�������Ԥʤ���硤���μ��ǰ�Ϣ��API��Ȥ��ޤ���<br>
 * - begin_scan_along_y() �ǽ����򤹤롥<br>
 * - scan_along_y() ����ο������ƤӽФ����󤴤Ȥν�����Ԥʤ���<br>
 * - end_scan_along_y() �ǽ�λ����(����Хåե��γ���)��Ԥʤ���<br>
 * scan_along_y() ��1��ʬ�����Ǥ����ä�����Хåե��Υ��ɥ쥹(32�Х��Ȥǥ��饤
 * �󤵤�Ƥ���)���֤��ޤ������ΰ���Хåե��ϥ��֥������Ȥ������������Ǥ��ä�
 * �� double �����󶡤���ޤ����ʤ������ΥХåե��ϥ��֥������ȤǴ�������Ƥ���
 * ���ᡤ�ץ���ޤ��������ƤϤ����ޤ���<br>
 * scan_along_y() �ϻ����ϰ����ƤΥ�����󤬴�λ���Ƥ�����ˤ� NULL ���֤���
 * ����<br>
 * �����ǡ��������פ�Ȥ���ʤɤ˻Ȥ����дؿ��Ǥ���
 *
 * @param     col_index �����
 * @param     col_size �󥵥���
 * @param     row_index �԰���
 * @param     row_size �ԥ�����
 * @param     layer_index �쥤�����
 * @param     layer_size �쥤�䥵����
 * @return    �������ˤ��ͭ�����Ǥο������顼�ξ������͡�
 */
long fits_image::begin_scan_along_y( long col_index, long col_size,
				     long row_index, long row_size,
				     long layer_index, long layer_size ) const
{
    fits_image *thisp = (fits_image *)this;		/* ����Ū�˥��㥹�� */
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
 * @brief  �����ǡ����������Ǥ�y������ʬ���Ϣ³Ū�˥������ (�֤��ͤ�double��)
 */
double *fits_image::scan_along_y( long *n, long *x, long *y, long *z ) const
{
    double *ret_p;
    fits_image *thisp = (fits_image *)this;		/* ����Ū�˥��㥹�� */
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
 * @brief  �����ǡ����������Ǥ�y������ʬ���Ϣ³Ū�˥������ (�֤��ͤ�float��)
 */
float *fits_image::scan_along_y_f( long *n, long *x, long *y, long *z ) const
{
    float *ret_p;
    fits_image *thisp = (fits_image *)this;		/* ����Ū�˥��㥹�� */
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
 * @brief  begin_scan_along_y() �ǳ��Ϥ���������󡦥������󥹤ν�λ
 */
void fits_image::end_scan_along_y() const
{
    fits_image *thisp = (fits_image *)this;		/* ����Ū�˥��㥹�� */
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
 * @brief  �����ǡ����������Ǥ�z������ʬ��ǤΥ�����󡦥������󥹤γ���
 *
 * ���Ȥβ����ǡ�������λ��ꤵ�줿�ϰϤ����Ǥ�(x,y)�ΰ���1��1�ĤˤĤ��ơ�
 * z�����˥�����󤹤뤿��ν����򤷤ޤ��������ǥ֥�å�ñ�̤ι�®transpose
 * (�ǡ������쥤�ˤ�����x����z���Ȥ����촹��)��Ԥʤ����ᡤ�⤤�ѥե����ޥ�
 * �������ޤ���<br>
 * z������������Ԥʤ���硤���μ��ǰ�Ϣ��API��Ȥ��ޤ���<br>
 * - beginf_scan_along_z() �ǽ����򤹤롥<br>
 * - scan_along_z() ��ɬ�פʿ������ƤӽФ���(x,y)���Ȥν�����Ԥʤ���<br>
 * - end_scan_along_z() �ǽ�λ����(����Хåե��γ���)��Ԥʤ���<br>
 * scan_along_z() ��(x,y)�ΰ���1�ĤˤĤ��Ƥ�z���������Ǥ����ä�����Хåե���
 * ���ɥ쥹(32�Х��Ȥǥ��饤�󤵤�Ƥ���)���֤��ޤ������ΰ���Хåե��ϥ���
 * �������Ȥ������������Ǥ��äƤ� double �����󶡤���ޤ����ʤ������ΥХåե�
 * �ϥ��֥������ȤǴ�������Ƥ��뤿�ᡤ�ץ���ޤ��������ƤϤ����ޤ���<br>
 * scan_along_z() �ϻ����ϰ����ƤΥ�����󤬴�λ���Ƥ�����ˤ� NULL ���֤���
 * ����<br>
 * �����ǡ��������פ�Ȥ���ʤɤ˻Ȥ����дؿ��Ǥ���z�������礭������ʬ�Ǥʤ�
 * ��硤�ѥե����ޥ󥹤������ʤ���������ޤ������ξ��ϡ�scan_zx_planes()
 * �򤪻����������
 *
 * @param     exp_fmt ������󤹤��ϰϤ򼨤�ʸ����(��: "0:99,*")�Τ����
 *                    �ե����ޥåȻ���(printf��Ʊ��)
 * @param     ... exp_fmt���б���������Ĺ�����γ����ǥǡ���
 * @return    �������ˤ��ͭ�����Ǥο������顼�ξ������͡�
 * @note      "[1:100,*]" �Τ褦�� [] �ǰϤ�ȡ�1-indexed �Ȥ��ư����ޤ���
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
 * @brief  �����ǡ����������Ǥ�z������ʬ��ǤΥ�����󡦥������󥹤γ���
 *
 * ���Ȥβ����ǡ�������λ��ꤵ�줿�ϰϤ����Ǥ�(x,y)�ΰ���1��1�ĤˤĤ��ơ�
 * z�����˥�����󤹤뤿��ν����򤷤ޤ��������ǥ֥�å�ñ�̤ι�®transpose
 * (�ǡ������쥤�ˤ�����x����z���Ȥ����촹��)��Ԥʤ����ᡤ�⤤�ѥե����ޥ�
 * �������ޤ���<br>
 * z������������Ԥʤ���硤���μ��ǰ�Ϣ��API��Ȥ��ޤ���<br>
 * - beginf_scan_along_z() �ǽ����򤹤롥<br>
 * - scan_along_z() ��ɬ�פʿ������ƤӽФ���(x,y)���Ȥν�����Ԥʤ���<br>
 * - end_scan_along_z() �ǽ�λ����(����Хåե��γ���)��Ԥʤ���<br>
 * scan_along_z() ��(x,y)�ΰ���1�ĤˤĤ��Ƥ�z���������Ǥ����ä�����Хåե���
 * ���ɥ쥹(32�Х��Ȥǥ��饤�󤵤�Ƥ���)���֤��ޤ������ΰ���Хåե��ϥ���
 * �������Ȥ������������Ǥ��äƤ� double �����󶡤���ޤ����ʤ������ΥХåե�
 * �ϥ��֥������ȤǴ�������Ƥ��뤿�ᡤ�ץ���ޤ��������ƤϤ����ޤ���<br>
 * scan_along_z() �ϻ����ϰ����ƤΥ�����󤬴�λ���Ƥ�����ˤ� NULL ���֤���
 * ����<br>
 * �����ǡ��������פ�Ȥ���ʤɤ˻Ȥ����дؿ��Ǥ���z�������礭������ʬ�Ǥʤ�
 * ��硤�ѥե����ޥ󥹤������ʤ���������ޤ������ξ��ϡ�scan_zx_planes()
 * �򤪻����������
 *
 * @param     exp_fmt ������󤹤��ϰϤ򼨤�ʸ����(��: "0:99,*")�Τ����
 *                    �ե����ޥåȻ���(printf��Ʊ��)
 * @param     ap exp_fmt���б���������Ĺ�����Υꥹ��
 * @return    �������ˤ��ͭ�����Ǥο������顼�ξ������͡�
 * @note      "[1:100,*]" �Τ褦�� [] �ǰϤ�ȡ�1-indexed �Ȥ��ư����ޤ���
 */
long fits_image::vbeginf_scan_along_z( const char *exp_fmt, va_list ap ) const
{
    fits_image *thisp = (fits_image *)this;		/* ����Ū�˥��㥹�� */

    thisp->prepare_scan_a_section();
    thisp->scan_use_flags |= Bit_use_along_z;

    return this->data_array().vbeginf_scan_along_z( exp_fmt, ap );
}

/**
 * @brief  �����ǡ����������Ǥ�z������ʬ��ǤΥ�����󡦥������󥹤γ���
 *
 * ���Ȥβ����ǡ�������λ��ꤵ�줿�ϰϤ����Ǥ�(x,y)�ΰ���1��1�ĤˤĤ��ơ�
 * z�����˥�����󤹤뤿��ν����򤷤ޤ��������ǥ֥�å�ñ�̤ι�®transpose
 * (�ǡ������쥤�ˤ�����x����z���Ȥ����촹��)��Ԥʤ����ᡤ�⤤�ѥե����ޥ�
 * �������ޤ���<br>
 * z������������Ԥʤ���硤���μ��ǰ�Ϣ��API��Ȥ��ޤ���<br>
 * - begin_scan_along_z() �ǽ����򤹤롥<br>
 * - scan_along_z() ��ɬ�פʿ������ƤӽФ���(x,y)���Ȥν�����Ԥʤ���<br>
 * - end_scan_along_z() �ǽ�λ����(����Хåե��γ���)��Ԥʤ���<br>
 * scan_along_z() ��(x,y)�ΰ���1�ĤˤĤ��Ƥ�z���������Ǥ����ä�����Хåե���
 * ���ɥ쥹(32�Х��Ȥǥ��饤�󤵤�Ƥ���)���֤��ޤ������ΰ���Хåե��ϥ���
 * �������Ȥ������������Ǥ��äƤ� double �����󶡤���ޤ����ʤ������ΥХåե�
 * �ϥ��֥������ȤǴ�������Ƥ��뤿�ᡤ�ץ���ޤ��������ƤϤ����ޤ���<br>
 * scan_along_z() �ϻ����ϰ����ƤΥ�����󤬴�λ���Ƥ�����ˤ� NULL ���֤���
 * ����<br>
 * �����ǡ��������פ�Ȥ���ʤɤ˻Ȥ����дؿ��Ǥ���z�������礭������ʬ�Ǥʤ�
 * ��硤�ѥե����ޥ󥹤������ʤ���������ޤ������ξ��ϡ�scan_zx_planes()
 * �򤪻����������
 *
 * @param     col_index �����
 * @param     col_size �󥵥���
 * @param     row_index �԰���
 * @param     row_size �ԥ�����
 * @param     layer_index �쥤�����
 * @param     layer_size �쥤�䥵����
 * @return    �������ˤ��ͭ�����Ǥο������顼�ξ������͡�
 */
long fits_image::begin_scan_along_z( long col_index, long col_size,
				     long row_index, long row_size,
				     long layer_index, long layer_size ) const
{
    fits_image *thisp = (fits_image *)this;		/* ����Ū�˥��㥹�� */
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
 * @brief  �����ǡ����������Ǥ�z������ʬ���Ϣ³Ū�˥������ (�֤��ͤ�double��)
 */
double *fits_image::scan_along_z( long *n, long *x, long *y, long *z ) const
{
    double *ret_p;
    fits_image *thisp = (fits_image *)this;		/* ����Ū�˥��㥹�� */
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
 * @brief  �����ǡ����������Ǥ�z������ʬ���Ϣ³Ū�˥������ (�֤��ͤ�float��)
 */
float *fits_image::scan_along_z_f( long *n, long *x, long *y, long *z ) const
{
    float *ret_p;
    fits_image *thisp = (fits_image *)this;		/* ����Ū�˥��㥹�� */
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
 * @brief  begin_scan_along_z() �ǳ��Ϥ���������󡦥������󥹤ν�λ
 */
void fits_image::end_scan_along_z() const
{
    fits_image *thisp = (fits_image *)this;		/* ����Ū�˥��㥹�� */
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
 * @brief  �����ǡ����������Ǥ�zx��ñ�̤ǤΥ�����󡦥������󥹤γ���
 *
 * ���Ȥβ����ǡ�������λ��ꤵ�줿�ϰϤ����Ǥ�zx��ñ�̤ǥ�����󤹤뤿���
 * �����򤷤ޤ��������ǥ֥�å�ñ�̤ι�®transpose(�ǡ������쥤�ˤ�����x����z��
 * �Ȥ����촹��)��Ԥʤ����ᡤ�⤤�ѥե����ޥ󥹤������ޤ���<br>
 * zx��ñ�̤ǥ�������Ԥʤ���硤���μ��ǰ�Ϣ��API��Ȥ��ޤ���<br>
 * - beginf_scan_zx_planes() �ǽ����򤹤롥<br>
 * - scan_zx_planes() ��ɬ�פʿ������ƤӽФ����̤��Ȥν�����Ԥʤ���<br>
 * - end_scan_zx_planes() �ǽ�λ����(����Хåե��γ���)��Ԥʤ���<br>
 * scan_zx_planes() ��y�ΰ���1�ĤˤĤ��Ƥ�zx�̤����Ǥ����ä�����Хåե��Υ���
 * �쥹(32�Х��Ȥǥ��饤�󤵤�Ƥ���)���֤��ޤ������ΰ���Хåե��ϥ��֥�������
 * �������������Ǥ��äƤ� double �����󶡤���ޤ����ʤ������ΥХåե��ϥ��֥���
 * ���ȤǴ�������Ƥ��뤿�ᡤ�ץ���ޤ��������ƤϤ����ޤ���<br>
 * scan_zx_planes() �ϻ����ϰ����ƤΥ�����󤬴�λ���Ƥ�����ˤ� NULL ���֤�
 * �ޤ���<br>
 * �����ǡ��������פ�Ȥ���ʤɤ˻Ȥ����дؿ��Ǥ���
 *
 * @param     exp_fmt ������󤹤��ϰϤ򼨤�ʸ����(��: "0:99,*")�Τ����
 *                    �ե����ޥåȻ���(printf��Ʊ��)
 * @param     ... exp_fmt���б���������Ĺ�����γ����ǥǡ���
 * @return    �������ˤ��ͭ�����Ǥο������顼�ξ������͡�
 * @note      "[1:100,*]" �Τ褦�� [] �ǰϤ�ȡ�1-indexed �Ȥ��ư����ޤ���
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
 * @brief  �����ǡ����������Ǥ�zx��ñ�̤ǤΥ�����󡦥������󥹤γ���
 *
 * ���Ȥβ����ǡ�������λ��ꤵ�줿�ϰϤ����Ǥ�zx��ñ�̤ǥ�����󤹤뤿���
 * �����򤷤ޤ��������ǥ֥�å�ñ�̤ι�®transpose(�ǡ������쥤�ˤ�����x����z��
 * �Ȥ����촹��)��Ԥʤ����ᡤ�⤤�ѥե����ޥ󥹤������ޤ���<br>
 * zx��ñ�̤ǥ�������Ԥʤ���硤���μ��ǰ�Ϣ��API��Ȥ��ޤ���<br>
 * - beginf_scan_zx_planes() �ǽ����򤹤롥<br>
 * - scan_zx_planes() ��ɬ�פʿ������ƤӽФ����̤��Ȥν�����Ԥʤ���<br>
 * - end_scan_zx_planes() �ǽ�λ����(����Хåե��γ���)��Ԥʤ���<br>
 * scan_zx_planes() ��y�ΰ���1�ĤˤĤ��Ƥ�zx�̤����Ǥ����ä�����Хåե��Υ���
 * �쥹(32�Х��Ȥǥ��饤�󤵤�Ƥ���)���֤��ޤ������ΰ���Хåե��ϥ��֥�������
 * �������������Ǥ��äƤ� double �����󶡤���ޤ����ʤ������ΥХåե��ϥ��֥���
 * ���ȤǴ�������Ƥ��뤿�ᡤ�ץ���ޤ��������ƤϤ����ޤ���<br>
 * scan_zx_planes() �ϻ����ϰ����ƤΥ�����󤬴�λ���Ƥ�����ˤ� NULL ���֤�
 * �ޤ���<br>
 * �����ǡ��������פ�Ȥ���ʤɤ˻Ȥ����дؿ��Ǥ���
 *
 * @param     exp_fmt ������󤹤��ϰϤ򼨤�ʸ����(��: "0:99,*")�Τ����
 *                    �ե����ޥåȻ���(printf��Ʊ��)
 * @param     ap exp_fmt���б���������Ĺ�����Υꥹ��
 * @return    �������ˤ��ͭ�����Ǥο������顼�ξ������͡�
 * @note      "[1:100,*]" �Τ褦�� [] �ǰϤ�ȡ�1-indexed �Ȥ��ư����ޤ���
 */
long fits_image::vbeginf_scan_zx_planes( const char *exp_fmt, va_list ap ) const
{
    fits_image *thisp = (fits_image *)this;		/* ����Ū�˥��㥹�� */

    thisp->prepare_scan_a_section();
    thisp->scan_use_flags |= Bit_use_zx_planes;

    return this->data_array().vbeginf_scan_zx_planes( exp_fmt, ap );
}

/**
 * @brief  �����ǡ����������Ǥ�zx��ñ�̤ǤΥ�����󡦥������󥹤γ���
 *
 * ���Ȥβ����ǡ�������λ��ꤵ�줿�ϰϤ����Ǥ�zx��ñ�̤ǥ�����󤹤뤿���
 * �����򤷤ޤ��������ǥ֥�å�ñ�̤ι�®transpose(�ǡ������쥤�ˤ�����x����z��
 * �Ȥ����촹��)��Ԥʤ����ᡤ�⤤�ѥե����ޥ󥹤������ޤ���<br>
 * zx��ñ�̤ǥ�������Ԥʤ���硤���μ��ǰ�Ϣ��API��Ȥ��ޤ���<br>
 * - begin_scan_zx_planes() �ǽ����򤹤롥<br>
 * - scan_zx_planes() ��ɬ�פʿ������ƤӽФ����̤��Ȥν�����Ԥʤ���<br>
 * - end_scan_zx_planes() �ǽ�λ����(����Хåե��γ���)��Ԥʤ���<br>
 * scan_zx_planes() ��y�ΰ���1�ĤˤĤ��Ƥ�zx�̤����Ǥ����ä�����Хåե��Υ���
 * �쥹(32�Х��Ȥǥ��饤�󤵤�Ƥ���)���֤��ޤ������ΰ���Хåե��ϥ��֥�������
 * �������������Ǥ��äƤ� double �����󶡤���ޤ����ʤ������ΥХåե��ϥ��֥���
 * ���ȤǴ�������Ƥ��뤿�ᡤ�ץ���ޤ��������ƤϤ����ޤ���<br>
 * scan_zx_planes() �ϻ����ϰ����ƤΥ�����󤬴�λ���Ƥ�����ˤ� NULL ���֤�
 * �ޤ���<br>
 * �����ǡ��������פ�Ȥ���ʤɤ˻Ȥ����дؿ��Ǥ���
 *
 * @param     col_index �����
 * @param     col_size �󥵥���
 * @param     row_index �԰���
 * @param     row_size �ԥ�����
 * @param     layer_index �쥤�����
 * @param     layer_size �쥤�䥵����
 * @return    �������ˤ��ͭ�����Ǥο������顼�ξ������͡�
 */
long fits_image::begin_scan_zx_planes( 
				      long col_index, long col_size,
				      long row_index, long row_size,
				      long layer_index, long layer_size ) const
{
    fits_image *thisp = (fits_image *)this;		/* ����Ū�˥��㥹�� */
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
 * @brief  �����ǡ����������Ǥ�zx��ñ�̤�Ϣ³Ū�˥������ (�֤��ͤ�double��)
 */
double *fits_image::scan_zx_planes( 
			long *n_z, long *n_x, long *x, long *y, long *z ) const
{
    double *ret_p;
    fits_image *thisp = (fits_image *)this;		/* ����Ū�˥��㥹�� */
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
 * @brief  �����ǡ����������Ǥ�zx��ñ�̤�Ϣ³Ū�˥������ (�֤��ͤ�float��)
 */
float *fits_image::scan_zx_planes_f( 
			long *n_z, long *n_x, long *x, long *y, long *z ) const
{
    float *ret_p;
    fits_image *thisp = (fits_image *)this;		/* ����Ū�˥��㥹�� */
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
 * @brief  begin_scan_zx_planes() �ǳ��Ϥ���������󡦥������󥹤ν�λ
 */
void fits_image::end_scan_zx_planes() const
{
    fits_image *thisp = (fits_image *)this;		/* ����Ū�˥��㥹�� */
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
 * @brief  �����ǡ��������3�������ϰϤ����ǤΤ���Υ�����󡦥������󥹤γ���
 *
 * ���Ȥβ����ǡ�������λ��ꤵ�줿3�������ϰϤ����Ǥ�1�٤˥�����󤹤뤿��
 * �ν����򤷤ޤ���3������祹������Ԥʤ���硤���μ��ǰ�Ϣ��API��Ȥ���
 * ����<br>
 * - beginf_scan_a_cube() �ǽ����򤹤롥<br>
 * - scan_a_cube() ��ɬ�פʿ������ƤӽФ������塼�֤��Ȥν�����Ԥʤ���<br>
 * - end_scan_a_cube() �ǽ�λ����(����Хåե��γ���)��Ԥʤ���<br>
 * scan_a_cube() �ϻ�����ʬ�����Ǥ����ä�����Хåե��Υ��ɥ쥹(32�Х��Ȥǥ���
 * ���󤵤�Ƥ���)���֤��ޤ������ΰ���Хåե��ϡ�scan_a_cube() �ξ��ϥ���
 * �������Ȥ������������Ǥ��äƤ� double ���ǡ�scan_a_cube_f() �ξ��� float
 * �����󶡤���ޤ����ʤ������ΥХåե��ϥ��֥������ȤǴ�������Ƥ��뤿�ᡤ��
 * ����ޤ��������ƤϤ����ޤ���<br>
 * �����ǡ��������פ�Ȥ���ʤɤ˻Ȥ����дؿ��Ǥ���
 *
 * @param     exp_fmt ������󤹤��ϰϤ򼨤�ʸ����(��: "0:99,*")�Τ����
 *                    �ե����ޥåȻ���(printf��Ʊ��)
 * @param     ... exp_fmt���б���������Ĺ�����γ����ǥǡ���
 * @return    �������ˤ��ͭ�����Ǥο������顼�ξ������͡�
 * @note      "[1:100,*]" �Τ褦�� [] �ǰϤ�ȡ�1-indexed �Ȥ��ư����ޤ���
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
 * @brief  �����ǡ��������3�������ϰϤ����ǤΤ���Υ�����󡦥������󥹤γ���
 *
 * ���Ȥβ����ǡ�������λ��ꤵ�줿3�������ϰϤ����Ǥ�1�٤˥�����󤹤뤿��
 * �ν����򤷤ޤ���3������祹������Ԥʤ���硤���μ��ǰ�Ϣ��API��Ȥ���
 * ����<br>
 * - beginf_scan_a_cube() �ǽ����򤹤롥<br>
 * - scan_a_cube() ��ɬ�פʿ������ƤӽФ������塼�֤��Ȥν�����Ԥʤ���<br>
 * - end_scan_a_cube() �ǽ�λ����(����Хåե��γ���)��Ԥʤ���<br>
 * scan_a_cube() �ϻ�����ʬ�����Ǥ����ä�����Хåե��Υ��ɥ쥹(32�Х��Ȥǥ���
 * ���󤵤�Ƥ���)���֤��ޤ������ΰ���Хåե��ϡ�scan_a_cube() �ξ��ϥ���
 * �������Ȥ������������Ǥ��äƤ� double ���ǡ�scan_a_cube_f() �ξ��� float
 * �����󶡤���ޤ����ʤ������ΥХåե��ϥ��֥������ȤǴ�������Ƥ��뤿�ᡤ��
 * ����ޤ��������ƤϤ����ޤ���<br>
 * �����ǡ��������פ�Ȥ���ʤɤ˻Ȥ����дؿ��Ǥ���
 *
 * @param     exp_fmt ������󤹤��ϰϤ򼨤�ʸ����(��: "0:99,*")�Τ����
 *                    �ե����ޥåȻ���(printf��Ʊ��)
 * @param     ap exp_fmt���б���������Ĺ�����Υꥹ��
 * @return    �������ˤ��ͭ�����Ǥο������顼�ξ������͡�
 * @note      "[1:100,*]" �Τ褦�� [] �ǰϤ�ȡ�1-indexed �Ȥ��ư����ޤ���
 */
long fits_image::vbeginf_scan_a_cube( const char *exp_fmt, va_list ap ) const
{
    fits_image *thisp = (fits_image *)this;		/* ����Ū�˥��㥹�� */

    thisp->prepare_scan_a_section();
    thisp->scan_use_flags |= Bit_use_cube;

    return this->data_array().vbeginf_scan_a_cube( exp_fmt, ap );
}

/**
 * @brief  �����ǡ��������3�������ϰϤ����ǤΤ���Υ�����󡦥������󥹤γ���
 *
 * ���Ȥβ����ǡ�������λ��ꤵ�줿3�������ϰϤ����Ǥ�1�٤˥�����󤹤뤿��
 * �ν����򤷤ޤ���3������祹������Ԥʤ���硤���μ��ǰ�Ϣ��API��Ȥ���
 * ����<br>
 * - begin_scan_a_cube() �ǽ����򤹤롥<br>
 * - scan_a_cube() ��ɬ�פʿ������ƤӽФ������塼�֤��Ȥν�����Ԥʤ���<br>
 * - end_scan_a_cube() �ǽ�λ����(����Хåե��γ���)��Ԥʤ���<br>
 * scan_a_cube() �ϻ�����ʬ�����Ǥ����ä�����Хåե��Υ��ɥ쥹(32�Х��Ȥǥ���
 * ���󤵤�Ƥ���)���֤��ޤ������ΰ���Хåե��ϡ�scan_a_cube() �ξ��ϥ���
 * �������Ȥ������������Ǥ��äƤ� double ���ǡ�scan_a_cube_f() �ξ��� float
 * �����󶡤���ޤ����ʤ������ΥХåե��ϥ��֥������ȤǴ�������Ƥ��뤿�ᡤ��
 * ����ޤ��������ƤϤ����ޤ���<br>
 * �����ǡ��������פ�Ȥ���ʤɤ˻Ȥ����дؿ��Ǥ���
 *
 * @param     col_index �����
 * @param     col_size �󥵥���
 * @param     row_index �԰���
 * @param     row_size �ԥ�����
 * @param     layer_index �쥤�����
 * @param     layer_size �쥤�䥵����
 * @return    �������ˤ��ͭ�����Ǥο������顼�ξ������͡�
 */
long fits_image::begin_scan_a_cube( long col_index, long col_size,
				    long row_index, long row_size,
				    long layer_index, long layer_size ) const
{
    fits_image *thisp = (fits_image *)this;		/* ����Ū�˥��㥹�� */
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
 * @brief  �����ǡ��������3�������ϰϤ����Ǥ�Ϣ³Ū�˥������ (�֤��ͤ�double)
 */
double *fits_image::scan_a_cube( long *n_x, long *n_y, long *n_z, 
				 long *x, long *y, long *z ) const
{
    double *ret_p;
    fits_image *thisp = (fits_image *)this;		/* ����Ū�˥��㥹�� */
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
 * @brief  �����ǡ��������3�������ϰϤ����Ǥ�Ϣ³Ū�˥������ (�֤��ͤ�float)
 */
float *fits_image::scan_a_cube_f( long *n_x, long *n_y, long *n_z, 
				  long *x, long *y, long *z ) const
{
    float *ret_p;
    fits_image *thisp = (fits_image *)this;		/* ����Ū�˥��㥹�� */
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
 * @brief  begin_scan_a_cube() �ǳ��Ϥ���������󡦥������󥹤ν�λ
 */
void fits_image::end_scan_a_cube() const
{
    fits_image *thisp = (fits_image *)this;		/* ����Ū�˥��㥹�� */
    this->data_array().end_scan_a_cube();
    thisp->scan_use_flags &= ~Bit_use_cube;
    if ( thisp->scan_use_flags == 0 ) thisp->scan_prms_rec = NULL;
}


/*
 * etc.
 */

/**
 * @brief  �ƥ�ݥ�ꥪ�֥������ȤΤ����shallow copy°������Ϳ
 *
 *  shallow copy ����Ĥ�����˻��Ѥ��ޤ�(����ǡ������Ф���ͭ��)��
 *
 * @note   ������֥������Ȥ� return ��ľ���ǻ��Ѥ��롥
 */
/* ���: ���Ȥ��֤������Ȥ��������return ʸ�Ǥ������ȥ��󥹥��� */
/*       ��2�󥳥ԡ������Ȥ����������ʻ��ˤʤ�Τ� void �ˤʤäƤ��� */
void fits_image::set_scopy_flag()
{
    this->fits_hdu::set_scopy_flag();
    this->data_array().set_scopy_flag();
    return;
}


/* 
 * protected member functions
 */

/* get_idx_3d_cs() �� inline �ˤ����Τǥإå��ե�����˰�ư���� */

#if 0
/* ��������(inline�Τ�Ĥ�����)��if��else �ǽ񤯤ȡ������ʤ� */
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
/* ����ϸŤ� mdarray::get_idx_3d_cs() ��Ʊ������ */
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
 * @brief  ���ȥ꡼�फ�� Header Unit �� Data Unit ���ɤ�
 *
 * @note   ���Υ��дؿ��� protected �Ǥ���
 */
ssize_t fits_image::read_stream( cstreamio &sref )
{
    return this->image_load(NULL,sref,NULL,false,NULL);
}

/**
 * @brief  ���ȥ꡼�फ�� Header Unit �� Data Unit ���ɤ�
 *
 * @note   ���Υ��дؿ��� protected �Ǥ���
 */
ssize_t fits_image::read_stream( cstreamio &sref, size_t max_bytes_read )
{
    return this->image_load(NULL,sref,NULL,false,&max_bytes_read);
}

/**
 * @brief  ���ȥ꡼�फ��ɬ�פ˱����� Header Unit ���ɤߡ�Data Unit ���ɤ�
 *
 * @note   ���Υ��дؿ��� protected �Ǥ���
 */
ssize_t fits_image::read_stream( fits_hdu *objp, cstreamio &sref )
{
    return this->image_load(objp,sref,NULL,false,NULL);
}

/**
 * @brief  ���ȥ꡼�फ��ɬ�פ˱����� Header Unit ���ɤߡ�Data Unit ���ɤ�
 *
 * @note   ���Υ��дؿ��� protected �Ǥ���
 */
ssize_t fits_image::read_stream( fits_hdu *objp, cstreamio &sref, 
				const char *section_to_read, bool is_last_hdu )
{
    return this->image_load(objp,sref,section_to_read,is_last_hdu,NULL);
}

/**
 * @brief  ���ȥ꡼�फ��ɬ�פ˱����� Header Unit ���ɤߡ�Data Unit ���ɤ�
 *
 * @note   ���Υ��дؿ��� protected �Ǥ���
 */
ssize_t fits_image::read_stream( fits_hdu *objp, cstreamio &sref, 
				 const char *section_to_read, bool is_last_hdu,
				 size_t max_bytes_read )
{
    return this->image_load(objp,sref,section_to_read,is_last_hdu,
			    &max_bytes_read);
}

/**
 * @brief  �ǡ����ѡ��Ȥ���¸ or �����å�����η׻� (protected)
 *
 * @note ���Υ��дؿ��Ǥϥ���ǥ������Ĵ�����ʤ��ΤǸƤӽФ����ˤ�äƤ���
 *       ���ȡ�<br>
 *       ���Υ��дؿ��ϡ�<br>
 *         fits_image::write_stream() <br>
 *         -> fits_hdu::write_stream() <br>
 *         -> fits_image::save_or_check_data() <br>
 *       �Ȥ�����ϩ�ǸƤФ�롥
 * @note   ���Υ��дؿ��� protected �Ǥ���
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
 * @brief  Header Unit �� Data Unit �򥹥ȥ꡼��˽񤭹���
 *
 * @note   ���Υ��дؿ��� protected �Ǥ���
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
 * @brief  Header Unit �� Data Unit �򥹥ȥ꡼��˽񤭹�������ΥХ���Ĺ�����
 *
 * @note   ���Υ��дؿ��� protected �Ǥ���
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
    /* 2880���ܡ��ˤ��� */
    if ( data_total_bytes % FITS::FILE_RECORD_UNIT != 0 ) {
	size_t u = data_total_bytes / FITS::FILE_RECORD_UNIT;
	data_total_bytes = FITS::FILE_RECORD_UNIT * ( u + 1 );
    }

    return_value = header_total_bytes + data_total_bytes;
 quit:
    return return_value;
}


/**
 * @brief  fits_image �Ǵ������Ƥ�����ܤˤĤ��ơ��إå����֥������Ȥ򹹿�
 *
 * @note   ���Υ��дؿ��� protected �Ǥ���
 */
fits_image &fits_image::setup_sys_header()
{
    fits_header &hdr_rec = this->header_rec;

    //err_report(__FUNCTION__,"DEBUG","called!");

    /* ����Ū�ʥإå������(fits_hdu���饹�Υ��дؿ�) */
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

	    /* ¸�ߤ��Ƥ����硤�Ǥ���������Υ쥳���ɤ򤽤Τޤ޻Ȥ� */
	    /* ɬ�פʾ��Τ��ͤ򹹿�����                             */
	    if ( 0 <= idx ) {
		rec_src = hdr_rec.at_cs(idx);
	    }
	    else {
		fits::header_def def = {"", "", ""};
		def.keyword = keyword.cstr();
	        rec_src.assign(def);
	        rec_src.assign_default_comment(FITS::IMAGE_HDU);
	    }

	    /* ���֤���������� */
	    if ( idx == idx0 + 1 ) {
		if ( hdr_rec.at(idx).type() != FITS::LONGLONG_T ||
		     hdr_rec.at(idx).llvalue() != nxs ) {
		    hdr_rec.at(idx).assign(nxs);
		}
		idx0 = idx;
	    }
	    /* ���֤��������ʤ���� */
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

	/* �ĳ������뤫�⤷��ʤ��ΤǾä��Ƥ��� */
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
 * @brief  FITS �إå����֥������Ȥ� BZERO��BSCALE�ʤɤ������ݥ���Ȥ򸡺�
 *
 * @note   ���Υ��дؿ��� private �Ǥ���
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
 * @brief  blank �ͤ򥻥å� or �ꥻ�å� (������NULL�ǥꥻ�å�)
 *
 * @note   ���Υ��дؿ��� private �Ǥ���
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
 * @brief  ���ष�� z �����ͤ����
 *
 * @note   ���Υ��дؿ��� private �Ǥ���
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
 * @brief  this->data_rec ��Υ������򹹿�
 *
 * @note   ���Υ��дؿ��� private �Ǥ���
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
 * @brief  �����ֹ�ν���
 *
 * @note   ���Υ��дؿ��� private �Ǥ���
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
 * @brief  read_image_section()����Ƶ�Ū�˸ƤӽФ���n��������ʬ�ɤ߽Ф���¹�
 *
 * @note    private �ʴؿ��Ǥ���
 * @attention  ndim �� 0 �ˤ��ƤϤ����ʤ���
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
    size_t dim_ix = ndim - 1;			/* ���ߤμ����ֹ�(0-indexed) */
    const long s_start = s_begin[dim_ix];
    const long s_len = s_length[dim_ix];
    const long s_last1 = s_begin[dim_ix] + s_length[dim_ix];

    /* �����ֹ椬 0 �ξ�� */
    if ( dim_ix == 0 ) {
	size_t len_in;
	if ( 0 < s_start ) {					/* ���Ф� */
	    len_in = sz_bytes * s_start;
	    if ( max_bytes_ptr != NULL ) {
		if ( (*max_bytes_ptr) < (*len_read_all) + len_in ) {
		    len_in = (*max_bytes_ptr) - (*len_read_all);
		}
	    }
	    *rest_skip += len_in;
	    *len_read_all += len_in;
	}
	if ( 0 < s_len ) {					/* �ɤ� */
	    ssize_t len_read;
	    /* *rest_skip ��ί�äƤ���ʬ��쵤�� rskip ���� */
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
	    /* �ɤ� */
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
	if ( s_last1 < (long)(n_axisx_org[dim_ix]) ) {		/* ���Ф� */
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
    /* �����ֹ椬 1 �ʾ�ξ�� */
    else {
	/* ���Ф����ΥХ��ȿ� */
	long j;
	blen_block /= n_axisx_org[dim_ix];
	/* �ɤ� or ���Ф� */
	for ( j = 0 ; j < (long)(n_axisx_org[dim_ix]) ; j++ ) {
	    /* �����ϰϤˤ����� */
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
	    /* �����ϰϳ��ξ�� */
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
 * @brief  ���ȥ꡼�फ��� n ��������ʬ�ɤ߽Ф���¹�
 *
 * @note   ���Υ��дؿ��� private �Ǥ���
 */
int fits_image::read_image_section( cstreamio &sref, ssize_t sz_type,
				    const size_t *n_axisx_org, size_t ndim_org,
				    void *_sect_info, bool is_last_hdu,
				    const size_t *max_bytes_ptr, 
				    size_t *len_read_all )
{
    int ret_value = -1;
    size_t i, ndim;
    mdarray_size n_axisx;	/* �������������������� */
    section_exp_info *sect_info = (section_exp_info *)_sect_info;
    fits_header &hdr_rec = this->header_rec;

    size_t rest_skip;		/* 1��ǤޤȤ�� rskip ���뤿��Υ��塼 */
    char *dest_buf;
    char *dest_buf_current;

    /* ������������ndim: ndim_org ��Ʊ���ˤʤ뤫 0 �ˤʤ뤫�Τɤ��餫 */
    ndim = ndim_org;
    n_axisx.resize(ndim_org);
    this->coord_offset_rec.resize(ndim_org);
    this->is_flipped_rec.resize(ndim_org);

    /* �ϰϤ������� */
    for ( i=0 ; i < ndim_org ; i++ ) {
	const long c_len = n_axisx_org[i];
	bool fixed_vals = false;
	if ( sect_info->begin.length() <= i ) {	/* ���ꤵ��Ƥ��ʤ��ϰ� */
	    sect_info->begin[i] = 0;
	    sect_info->length[i] = c_len;
	    sect_info->flip_flag[i] = false;
	}
	else {					/* ���ꤵ��Ƥ����ϰ� */
	    if ( sect_info->length[i] == FITS::ALL ) {
		if ( sect_info->begin[i] < 0 ) {
		    sect_info->begin[i] = 0;
		    fixed_vals = true;
		}
		sect_info->length[i] = c_len - sect_info->begin[i];
	    }
	    /* �����ΰ褬�Ϥ߽Ф��Ƥ�����ν��� */
	    if ( sect_info->begin[i] < 0 ) {
		if ( sect_info->begin[i] + sect_info->length[i] <= 0 ) {
		    /* �ɤ�٤�Ĺ���ϥ��� */
		    sect_info->length[i] = 0;
		}
		else {
		    /* �ɤ�٤�Ĺ���򾮤������� */
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
	/* �⤷ 0 �ʤ� NAXIS=0 �ξ��֤ǽ�������Ƥ���� */
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

    /* �Хåե������� */
    try {
	this->_init_all_data_recs(sz_type);
	this->data_array().reallocate(n_axisx.array_ptr_cs(), ndim, false);
    }
    catch (...) {
	err_throw(__FUNCTION__,"FATAL",
		  "this->data_array().reallocate() failed");
    }

    /* �إå������ */
    this->header_set_sysrecords_prohibition(false);
    try {
	hdr_rec.record("NAXIS").assign((long long)ndim);
	for ( i=0 ; i < ndim ; i++ ) {
	    long long ll = n_axisx.at(i);
	    tstring keyword;
	    keyword.assignf("NAXIS%ld",(long)(i+1));
	    hdr_rec.record(keyword.cstr()).assign(ll);
	}
	for ( ; i < ndim_org ; i++ ) {	/* ���������������ʤä���� */
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
	rest_skip = 0;					/* �������˺���� */
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
    /* rest_skip ��ί�äƤ���ʬ��쵤�� rskip ����   */
    /* (�Ǹ���ɤ�٤�HDU�ξ��Ϥ����ϼ¹Ԥ���ʤ�) */
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
	/* �ɤ߹��ߤ���λ���Ƥʤ����ϡ�������ʬ��0�Ǥ���� */
	if ( dest_buf_current < last1 ) {
	    c_memset(dest_buf_current, 0, last1 - dest_buf_current);
	}
    }

    /* ȿž������Ԥʤ� */
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
 * @brief  ���ȥ꡼�फ�� Image HDU ���ɤ߽Ф�
 *
 * @note   ���Υ��дؿ��� private �Ǥ���
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

    /* ����äݤˤ��� */
    this->init();
    
    /* �ޤ��إå����ɤ�Ǥ��ʤ���� */
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
	/* ����������(�إå��ʤ�)�򥳥ԡ����� */
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

    /* PCOUNT �� GCOUNT �Ȥ�����å�����(�����) */
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

    /* ���� */
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
	/* ������ */
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
	     * ��������(�Ĥޤ� NAXISn �� 0 �ˤ���)�� fv �� ds9 �ǥ��顼�ˤʤ�
	     * ��Ϥꡤ�����٤��ǤϤʤ���������2011/11/28
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

    /* ��ʬ�ɤߤλ��꤬������ϡ���ֻ����ʸ�����ѡ�������*/
    if ( section_to_read != NULL ) {
	if ( parse_section_expression(section_to_read, &section_info) < 0 ) {
	    err_report1(__FUNCTION__,"WARNING",
	           "syntax error in expression '%s'; ignored",section_to_read);
	    section_to_read = NULL;
	}
    }

    //err_report1(__FUNCTION__,"DEBUG","[1] total_read_len %zd",total_read_len);

    /* ��ʬ�ɤߤλ��꤬������(�������ơ��֥��Ѥλ����̵�뤹��) */
    if ( section_to_read != NULL && section_info.sel_cols.length() == 0 ) {
	if ( read_image_section(sref, new_sz_type, n_axisx.array_ptr_cs(),
				ndim, (void *)(&section_info), is_last_hdu,
				max_bytes_ptr, &total_read_len) < 0 ) {
	    err_report(__FUNCTION__,"ERROR","read_image_section() failed;");
	    goto quit;
	}
    }
    /* �����ɤߤξ�� */
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
	    /* �ɤ��Х��ȿ������¤���Ƥ����� */
	    if ( max_bytes_ptr != NULL &&
		 (*max_bytes_ptr) < total_read_len + bytes_to_read ) {
		bytes_to_read = (*max_bytes_ptr) - total_read_len;
	    }
	}

	if ( 0 < bytes_to_read ) {
	    /*
	     * �ѥ�����A: �֥�å�ñ�̤ǡ�read��byteswap �򷫤��֤�������
	     */
#if 1
	    /* i7 �Υޥ���Υƥ��ȤǤ� 64kB �� 128kB �ǥ٥��Ȥ��ä� */
	    const size_t blen_block_cd = 65536;		/* �֥�å������� */
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
		/* ����ǥ�����Ĵ�� */
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
	     * �ѥ�����B: ������ϰ쵤���ɤॳ���ɡ��ѥ�����A������®��
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

	    /* �ɤ��Х��ȿ������¤���Ƥ��ơ������ɤ�ʤ��ä���� */
	    if ( max_bytes_ptr != NULL && 
		 bytes_to_read < this->data_array().byte_length() ) {
		c_memset((char *)(this->data_array().data_ptr()) + bytes_to_read,
			 0, this->data_array().byte_length() - bytes_to_read);
	    }
	}

    }

    //err_report1(__FUNCTION__,"DEBUG","[2] total_read_len %zd",total_read_len);

    if ( is_last_hdu == false ) {

	/* �Ĥ��;����ɤ߿ʤ��                        */
	/* (�Ǹ���ɤ�٤�HDU�ξ��Ϥ����ϼ¹Ԥ���ʤ�) */

	bytepix = this->data_array().bytes();
	if ( ((bytepix * npix_org_img) % FITS::FILE_RECORD_UNIT) != 0 ) {
	    len_padding = FITS::FILE_RECORD_UNIT
		- ((bytepix * npix_org_img) % FITS::FILE_RECORD_UNIT);
	}
	else {
	    len_padding = 0;
	}

	/* �ɤ��Х��ȿ������¤���Ƥ����� */
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
    
    /* endian �Ҥä��꤫���� */
    if ( reverse_endian_is_done == false ) {
	this->data_array().mdarray::reverse_endian(false);
    }
    
    return_val = total_read_len;
 quit:
    return return_val;
}

/* ���᡼���Խ� */

/*
 * BEGIN : convert_image_type()
 *
 * ���: fits_table_col �ˤ� convert_col_type() ��¸�ߤ��롥���Ƥ�����Ʊ��
 *       �ʤΤǡ������ɤ��ѹ��������Ʊ���˹Ԥʤ�����
 */

struct fits_image_cnv_prms {
    /* ���饤���Ȥ����(sizeof��Ĺ����Τ�����!) */
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
 * @brief  �ԥ������ͤ��Ѵ����� mdarray ��ɸ���Ѵ����ɤ����ɤ�����Ƚ�� (������)
 *
 * @note    private �ʴؿ��Ǥ���
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

    /* ξ���Ȥ���ư���������ξ�� */
    if ( src_is_float_type == true && dst_is_float_type == true &&
	 src.bzero()  == dst_bzero && 
	 src.bscale() == dst_bscale ) {
	do_simple_conv = true;
    }
    /* ξ���Ȥ������ξ�� */
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
    /* �������� float ���Ѵ�������� */
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
 * @brief  ���ꤵ�줿����ZERO�͡�SCALE�͡�BLANK�ͤ���Ĳ����ǡ������Ѵ�
 *
 * @note   ���Υ��дؿ��� private �Ǥ���
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

    /* �ǡ��������Ƥ� tmp_data_rec �˰�ư */
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

    /* ���Ѵ��Τ���δؿ������� */
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

    /* tmp_data_rec ���� this->data_rec �إǡ������᤹ */
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
 * @brief  ��® image fill��paste �Ѥν���
 *
 *  src_img �� NULL �ξ�硤�����顼�η��� src_size_type �ǻ��ꤹ�롥
 *
 * @note   ���Υ��дؿ��� private �Ǥ���
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

    /* ���Ѵ��Τ���δؿ������� */
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
 * BEGIN : image_fill() �黻�ʤ��� (��®)
 */

/**
 * @brief  image_fill() �黻�ʤ��� (��®��IDL�ε�ˡ)
 *
 * @note   ���Υ��дؿ��� private �Ǥ���
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
 * @brief  image_fill() �黻�ʤ��� (��®)
 *
 * @note   ���Υ��дؿ��� private �Ǥ���
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
 * BEGIN : image_fill() �黻������
 */

struct fits_image_fill_paste_prms {
    /* ���饤���Ȥ����(sizeof��Ĺ����Τ�����!) */
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
 * @brief  image_fill() �黻�����Ǥν���
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
 * @brief  image_fill() �黻������ (IDL�ε�ˡ)
 *
 * @note   ���Υ��дؿ��� private �Ǥ���
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
			&fill_x::u_func, (void *)&prms,       /* prms �϶��� */
			exp_fmt, ap );

    this->data_array().set_rounding(rounding_bak);

    return *this;
}

/**
 * @brief  image_fill() �黻������
 *
 * @note   ���Υ��дؿ��� private �Ǥ���
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
			&fill_x::u_func, (void *)&prms,       /* prms �϶��� */
			col_index, z_col_size, row_index, z_row_size,
			layer_index, z_layer_size );

    this->data_array().set_rounding(rounding_bak);

    return *this;
}

/*
 * BEGIN : image_paste() �黻�ʤ��� (��®)
 */

/**
 * @brief  image_paste() �黻�ʤ��� (��®��IDL�ε�ˡ)
 *
 * @note   ���Υ��дؿ��� private �Ǥ���
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
 * @brief  image_paste() �黻�ʤ��� (��®)
 *
 * @note   ���Υ��дؿ��� private �Ǥ���
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
 * BEGIN : image_paste() �黻������
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
 * @brief  image_paste() �ν���
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

    /* ���Ѵ��Τ���δؿ������� */
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
 * @brief  image_paste() �黻������ (IDL�ε�ˡ)
 *
 * @note   ���Υ��дؿ��� private �Ǥ���
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
			&paste_x::u_func, (void *)&p_dst,    /* p_dst �϶��� */
			exp_fmt, ap );

    this->data_array().set_rounding(rounding_bak);

 quit:
    return *this;
}

/**
 * @brief  image_paste() �黻������
 *
 * @note   ���Υ��дؿ��� private �Ǥ���
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
			&paste_x::u_func, (void *)&p_dst,    /* p_dst �϶��� */
			dest_col, dest_row, dest_layer );

    this->data_array().set_rounding(rounding_bak);

 quit:
    return *this;
}


/**
 * @brief  inline���дؿ���throw������˻���
 *
 * @note   ���Υ��дؿ��� private �Ǥ���
 */
void *fits_image::err_throw_void_p( const char *fnc, 
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
const void *fits_image::err_throw_const_void_p( const char *fnc, 
					 const char *lv, const char *mes) const
{
    err_throw(fnc,lv,mes);
    return (const void *)NULL;
}


#include "private/write_stream_or_get_csum.cc"

}	/* namespace sli */

#include "private/c_memset.cc"
