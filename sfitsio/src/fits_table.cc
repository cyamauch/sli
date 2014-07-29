/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2014-07-30 00:56:44 cyamauch> */

/**
 * @file   fits_table.cc
 * @brief  ASCII or Binary Table����� HDU ��ɽ�����륯�饹 fits_table �Υ�����
 */

#define CLASS_NAME "fits_table"

#include "config.h"

#include "fits_table.h"

#include <sli/sli_funcs.h>

#include "private/err_report.h"


namespace sli
{

#include "private/write_stream_or_get_csum.h"

static const long   DEFAULT_READ_BUFFER_SIZE  = 1048576;	/* 1MB */
static const size_t DEFAULT_WRITE_BUFFER_SIZE = 1048576;	/* 1MB */

static const char *Field_keywords[] = {"TTYPE", "TALAS", "TELEM", "TUNIT",
				       "TDISP", "TFORM", "TBCOL", "TDIM", 
				       "TZERO", "TSCAL", "TNULL", NULL};

/**
 * @brief  size_type(mdarray�η�����) ����Х���Ĺ���Ѵ� (������)
 *
 * @note   private �ʴؿ��Ǥ���
 */
inline static size_t zt2bytes( ssize_t sz_type )
{
    if ( 0 <= sz_type ) return (size_t)sz_type;
    else {
        size_t ret = (size_t)((ssize_t)0 - sz_type);
        if ( ret % 2 != 0 ) ret++;              /* ʣ�ǿ��ξ�� */
        return ret;
    }
}

/**
 * @brief  FITS����mdarray�� �Ѵ��ơ��֥� (������)
 */
static long Type2zt[] = {
    /* 0 1 2 3 4 5 6 7 8 9 a b c d e f */
       0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
       0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
       0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
       0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
       0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
       0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
       0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
       0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

static const bool Needs_reverse_byte_order = Sli__byte_order_is_little_endian;
static const bool Needs_reverse_float_word_order = Sli__float_word_order_is_little_endian;

/**
 * @brief  FITS����mdarray�� �Ѵ��ơ��֥�Υ��åȥ��å� (������)
 *
 * @note   private �ʴؿ��Ǥ���
 */
static void setup_type2zt()
{
    Type2zt[FITS::BYTE_T] = UCHAR_ZT;
    Type2zt[FITS::BOOL_T] = UCHAR_ZT;
    Type2zt[FITS::SHORT_T] = INT16_ZT;
    Type2zt[FITS::LONG_T] = INT32_ZT;
    Type2zt[FITS::LONGLONG_T] = INT64_ZT;
    Type2zt[FITS::FLOAT_T] = FLOAT_ZT;
    Type2zt[FITS::DOUBLE_T] = DOUBLE_ZT;
    Type2zt[FITS::COMPLEX_T] = FCOMPLEX_ZT;
    Type2zt[FITS::DOUBLECOMPLEX_T] = DCOMPLEX_ZT;
    return;
}

/* constructor */
/**
 * @brief  ���󥹥ȥ饯��
 */
fits_table::fits_table()
{
    this->row_size_rec = 0;
    this->col_size_rec = 0;
    this->cols_rec.init(sizeof(fits_table_col *), true);
    this->cols_rec.register_extptr(&this->cols_ptr_rec);	/* [void **] */
    this->default_null_svalue_rec = "NULL";

    this->heap_rec.init(1, false);
    this->heap_rec.set_auto_resize(false);
    this->heap_rec.register_extptr(&this->heap_ptr_rec);	/* [void **] */
    this->reserve_length_rec = 0;

    this->shallow_copy_ok = false;
    this->shallow_copy_dest_obj = NULL;
    this->shallow_copy_src_obj = NULL;
    this->__copying = false;

    setup_type2zt();

    if ( this->classlevel() < 1 ) this->increase_classlevel();
    this->set_hdutype(FITS::BINARY_TABLE_HDU);

    return;
}

/* copy constructor */
/**
 * @brief  ���ԡ����󥹥ȥ饯��
 */
fits_table::fits_table(const fits_table &obj)
{
    this->row_size_rec = 0;
    this->col_size_rec = 0;
    this->cols_rec.init(sizeof(fits_table_col *), true);
    this->cols_rec.register_extptr(&this->cols_ptr_rec);	/* [void **] */
    this->default_null_svalue_rec = "NULL";

    this->heap_rec.init(1, false);
    this->heap_rec.set_auto_resize(false);
    this->heap_rec.register_extptr(&this->heap_ptr_rec);	/* [void **] */
    this->reserve_length_rec = 0;

    this->shallow_copy_ok = false;
    this->shallow_copy_dest_obj = NULL;
    this->shallow_copy_src_obj = NULL;
    this->__copying = false;

    setup_type2zt();

    if ( this->classlevel() < 1 ) this->increase_classlevel();
    this->set_hdutype(FITS::BINARY_TABLE_HDU);

    this->init(obj);

    return;
}

/* destructor */
/**
 * @brief  �ǥ��ȥ饯��
 */
fits_table::~fits_table()
{
    if ( this->cols_ptr_rec != NULL ) {
	long i;
	for ( i=0 ; i < this->col_size_rec ; i++ ) {
	    if ( this->cols_ptr_rec[i] != NULL ) {
		delete this->cols_ptr_rec[i];
	    }
	}
    }

    return;
}

/**
 * @brief  ���֥������ȤΥ��ԡ�
 */
fits_table &fits_table::operator=(const fits_table &obj)
{
    this->init(obj);
    return *this;
}

/**
 * @brief  ���֥������Ȥν����
 */
fits_table &fits_table::init()
{
    this->fits_hdu::init();

    /* init ... */
    if ( this->cols_ptr_rec != NULL ) {
	long i;
	for ( i=0 ; i < this->col_size_rec ; i++ ) {
	    if ( this->cols_ptr_rec[i] != NULL ) {
		delete this->cols_ptr_rec[i];
	    }
	}
	this->cols_rec.init(sizeof(fits_table_col *), true);
    }

    this->index_rec.init();
    this->default_null_svalue_rec = "NULL";

    this->heap_rec.init(1, false);
    this->heap_rec.set_auto_resize(false);
    this->reserve_length_rec = 0;

    this->user_fld_kwd_rec.init();

    this->row_size_rec = 0;
    this->col_size_rec = 0;

    /* setup system header of this HDU */
    this->setup_sys_header();

    return *this;
}

/**
 * @brief  ���֥������ȤΥ��ԡ�
 */
fits_table &fits_table::init( const fits_table &obj )
{
    if ( &obj == this ) return *this;

    this->fits_table::init();
    this->fits_hdu::init(obj);

    /* �����ˤ� fits_table ���饹������ʤ��ϥ�������������å� */
    if ( 1 <= obj.classlevel() && 
	 ( obj.hdutype() == FITS::BINARY_TABLE_HDU || 
	   obj.hdutype() == FITS::ASCII_TABLE_HDU ) ) {
	const fits_table &obj1 = (const fits_table &)obj;
	/* cols_rec �򥳥ԡ� */
	if ( obj1.cols_ptr_rec != NULL ) {
	    long i;
	    try {
		this->cols_rec.resize( obj1.col_size_rec +1 ).clean();
	    }
	    catch (...) {
		this->fits_hdu::init();
		err_throw(__FUNCTION__,"FATAL","cols_rec.resize() failed");
	    }
	    this->col_size_rec = obj1.col_size_rec;
	    try {
		for ( i=0 ; i < obj1.col_size_rec ; i++ ) {
		    if ( obj1.cols_ptr_rec[i] != NULL ) {
			this->cols_ptr_rec[i] = new fits_table_col;
			*(this->cols_ptr_rec[i]) = *(obj1.cols_ptr_rec[i]);
			this->cols_ptr_rec[i]->register_manager(this);
		    }
		}
	    }
	    catch (...) {
		this->fits_table::init();
		this->fits_hdu::init();
		err_throw(__FUNCTION__,"FATAL","new or '=' failed");
	    }
	}
	this->row_size_rec = obj1.row_size_rec;
	/* heap_rec �򥳥ԡ� */
	try {
	    this->heap_rec.init(obj1.heap_rec);
	    this->reserve_length_rec = obj1.reserve_length_rec;
	}
	catch (...) {
	    this->fits_table::init();
	    this->fits_hdu::init();
	    err_throw(__FUNCTION__,"FATAL","this->heap_rec.init() failed");
	}
	/* index_rec �򥳥ԡ� */
	try {
	    this->index_rec = obj1.index_rec;
	    this->default_null_svalue_rec = obj1.default_null_svalue_rec;
	    this->user_fld_kwd_rec = obj1.user_fld_kwd_rec;
	}
	catch (...) {
	    this->fits_table::init();
	    this->fits_hdu::init();
	    err_throw(__FUNCTION__,"FATAL","'=' failed");
	}
	this->set_hdutype(obj.hdutype());
    }

    return *this;
}

/**
 * @brief  ���֥������Ȥν���� (fits::table_def �ǥơ��֥������򥻥å�)
 */
fits_table &fits_table::init( const fits::table_def defs[] )
{
    this->init();

    this->append_cols(defs);

    return *this;
}

/**
 * @brief  ���֥������Ȥν���� (fits::table_def_all �ǥơ��֥������򥻥å�)
 */
fits_table &fits_table::init( const fits::table_def_all defs[] )
{
    this->init();

    this->append_cols(defs);

    return *this;
}

/**
 * @brief  ���������ơ��֥��Х��ʥ�ơ��֥���Ѵ�
 *
 *  FITS�ե������Υ��������ơ��֥�� TFORMn ���� (fits::table_def ��¤�Τ�
 *  tdisp ���Ф���) �ϡ��Х��ʥ�ơ��֥�Ȥ�����¸�������ˤ� TFORMn ��
 *  ��������ʬ����¸����ޤ���TNULLn ���ͤ⡤TNULLn �Υ�������ʬ����¸����
 *  �ޤ�(�ͤ�̤����Ȥʤ�ޤ�)��
 *
 * @return  ���Ȥλ���
 */
fits_table &fits_table::ascii_to_binary()
{
    long idx;

    if ( this->hdutype() == FITS::ASCII_TABLE_HDU ) {
	fits_header &hdr_r = this->header_rec;

	/* update internal attribute */
	this->set_hdutype(FITS::BINARY_TABLE_HDU);
	idx = hdr_r.index("XTENSION");
	if ( 0 <= idx && hdr_r.record(idx).svalue_cs().strcmp("TABLE") == 0 ) {
	    hdr_r.record(idx).assign_default_comment();
	}

	/* setup system header of this HDU */
	this->setup_sys_header();
    }

    return *this;
}

/**
 * @brief  2�ĤΥ��֥������ȴ֤Ǥ����ƤΥ���å�
 *
 *  ���ꤵ�줿���֥�������obj �����Ƥȼ��Ȥ����Ƥ������ؤ��ޤ���
 *  �ǡ������󡦥إå������ơ�°�������٤Ƥξ��֤������ؤ��ޤ���
 *
 * @param   obj fits_table ���饹�Υ��֥�������
 * @return  ���Ȥλ���    
 */
fits_table &fits_table::swap( fits_table &obj )
{
    long tmp_row_size_rec = obj.row_size_rec;
    long tmp_col_size_rec = obj.col_size_rec;
    long long tmp_reserve_length_rec = obj.reserve_length_rec;

    if ( &obj == this ) return *this;

    obj.row_size_rec = this->row_size_rec;
    obj.col_size_rec = this->col_size_rec;
    obj.reserve_length_rec = this->reserve_length_rec;

    this->row_size_rec = tmp_row_size_rec;
    this->col_size_rec = tmp_col_size_rec;
    this->reserve_length_rec = tmp_reserve_length_rec;

    this->cols_rec.swap(obj.cols_rec);
    this->index_rec.swap(obj.index_rec);
    this->default_null_svalue_rec.swap(obj.default_null_svalue_rec);
    this->heap_rec.swap(obj.heap_rec);
    this->user_fld_kwd_rec.swap(obj.user_fld_kwd_rec);

    fits_hdu::swap(obj);

    return *this;
}

/**
 * @brief  �����̾���饫�����ֹ�����
 */
long fits_table::col_index( const char *col_name ) const
{
    return this->index_rec.index(col_name,0);
}

/**
 * @brief  �������ֹ椫�饫���̾�����
 */
const char *fits_table::col_name( long col_index ) const
{
    if ( col_index < 0 || this->col_size_rec <= col_index ) {
	return NULL;
    }
    return this->cols_ptr_rec[col_index]->def_rec.ttype;
}

/**
 * @brief  �����̾������
 */
fits_table &fits_table::assign_col_name( long col_index, const char *newname )
{
    fits::table_def_all def = {NULL};
    def.ttype = newname;
    return this->define_a_col(col_index, def);
}

/**
 * @brief  �����̾������
 */
fits_table &fits_table::assign_col_name( const char *col_name, 
					 const char *newname )
{
    fits::table_def_all def = {NULL};
    def.ttype = newname;
    return this->define_a_col(col_name, def);
}

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
fits_table &fits_table::assign_null_svalue( const char *snull )
{
    if ( snull != NULL ) {
	long i;
	this->default_null_svalue_rec.assign(snull);
	for ( i=0 ; i < this->col_size_rec ; i++ ) {
	    this->col(i).assign_null_svalue(snull);
	}
    }
    return *this;
}

/**
 * @brief  �������ɲä��륫���ˤĤ��Ƥ� NULLʸ���������
 *
 *  fits_table::append_cols() �� fits_table::init( const fits::table_def [] )
 *  �ǿ����˺���륫���ˤĤ��Ƥ� NULLʸ��������ꤷ�ޤ���<br>
 *  NULLʸ����ν�������ͤ� "NULL" �Ǥ���
 */
fits_table &fits_table::assign_default_null_svalue( const char *snull )
{
    if ( snull != NULL ) this->default_null_svalue_rec.assign(snull);
    return *this;
}

/**
 * @brief  �������ɲä��륫���ˤĤ��Ƥ� NULLʸ������֤�
 */
const char *fits_table::default_null_svalue() const
{
    return this->default_null_svalue_rec.cstr();
}

/**
 * @brief  �����������ؤ�
 *
 *  col_name0 �ǻ��ꤵ�줿����फ�� num_cols �ĤΥ���ෲ��col_name1 �ǻ���
 *  ���줿����फ�� num_cols �ĤΥ���ෲ�������ؤ��ޤ���<br>
 *  num_cols �ˤ�äơ�2�ĤΥ���ෲ���Ťʤ���ϡ�num_cols ���ͤ򸺤餷������
 *  �ؤ���Ԥʤ��ޤ���
 *
 * @param   col_name0 �����ؤ����򼨤������̾
 * @param   num_cols �����ؤ��륫����
 * @param   col_name1 �����ؤ���򼨤������̾
 * @return  ���Ȥλ���
 */
fits_table &fits_table::swap_cols( const char *col_name0, long num_cols,
				   const char *col_name1 )
{
    return this->swap_cols(this->col_index(col_name0), num_cols,
			   this->col_index(col_name1));
}

/**
 * @brief  �����������ؤ�
 *
 *  index0 �ǻ��ꤵ�줿����फ�� num_cols �ĤΥ���ෲ��index1 �ǻ���
 *  ���줿����फ�� num_cols �ĤΥ���ෲ�������ؤ��ޤ���<br>
 *  num_cols �ˤ�äơ�2�ĤΥ���ෲ���Ťʤ���ϡ�num_cols ���ͤ򸺤餷������
 *  �ؤ���Ԥʤ��ޤ���
 *
 * @param   index0 �����ؤ����򼨤�����।��ǥå���
 * @param   num_cols �����ؤ��륫����
 * @param   index1 �����ؤ���򼨤�����।��ǥå���
 * @return  ���Ȥλ���
 */
fits_table &fits_table::swap_cols( long index0, long num_cols, long index1 )
{
    long i;
    long hdr_dummy_offset;

    if ( index0 < 0 || this->col_size_rec <= index0 ) goto quit; /* invalid */
    if ( index1 < 0 || this->col_size_rec <= index1 ) goto quit; /* invalid */
    if ( num_cols < 0 ) goto quit;	/* invalid */

    if ( index1 == index0 || num_cols == 0 ) {
	goto quit;
    }
    else if ( index1 < index0 ) {
	long tmp;
	tmp = index0;
	index0 = index1;
	index1 = tmp;
    }

    /* �ΰ褬�Ťʤ�ʤ��褦�ˤ��� */
    if ( index1 < index0 + num_cols ) {		/* fixed 2012-01-30 */
	num_cols = index1 - index0;
    }
    if ( this->col_size_rec < index1 + num_cols ) {
	num_cols = this->col_size_rec - index1;
    }

    //err_report1(__FUNCTION__,"DEBUG","index0 = %ld",index0);
    //err_report1(__FUNCTION__,"DEBUG","index1 = %ld",index1);
    //err_report1(__FUNCTION__,"DEBUG","num_cols = %ld",num_cols);

    /* temporary number to swap header records */
    hdr_dummy_offset = this->col_size_rec;

    /* swap one by one */
    for ( i=0 ; i < num_cols ; i++ ) {
	fits_table_col *colp;
	int s;
	size_t j;

	/* update internal CtIndex [index0] */
	colp = this->cols_ptr_rec[index0 + i];
	s = this->index_rec.update(colp->definition().ttype,
				   index0 + i, index1 + i);
	if ( s < 0 ) {
	    err_throw(__FUNCTION__,"FATAL",
		      "Internal ERROR: index_rec.update() failed");
	}
	for ( j=0 ; j < colp->talas.length() ; j++ ) {
	    s = this->index_rec.update(colp->talas.cstr(j),
				       index0 + i, index1 + i);
	    if ( s < 0 ) {
	    	err_throw(__FUNCTION__,"FATAL",
			  "Internal ERROR: index_rec.update() failed");
	    }
	}

	/* update internal CtIndex [index1] */
	colp = this->cols_ptr_rec[index1 + i];
	s = this->index_rec.update(colp->definition().ttype,
				   index1 + i, index0 + i);
	if ( s < 0 ) {
	    err_throw(__FUNCTION__,"FATAL",
		      "Internal ERROR: index_rec.update() failed");
	}
	for ( j=0 ; j < colp->talas.length() ; j++ ) {
	    s = this->index_rec.update(colp->talas.cstr(j),
				       index1 + i, index0 + i);
	    if ( s < 0 ) {
	    	err_throw(__FUNCTION__,"FATAL",
			   "Internal ERROR: index_rec.update() failed");
	    }
	}

	/* �ºݤ˸򴹤��� */
	colp = this->cols_ptr_rec[index0 + i];
	this->cols_ptr_rec[index0 + i] = this->cols_ptr_rec[index1 + i];
	this->cols_ptr_rec[index1 + i] = colp;

	/* update system header of this HDU */
	this->renumber_hdr_of_a_col(index0 + i, hdr_dummy_offset + i);
	this->renumber_hdr_of_a_col(index1 + i, index0 + i);
	this->renumber_hdr_of_a_col(hdr_dummy_offset + i, index1 + i);
    }

    /* update TBCOL, if required */
    this->update_tbcol_hdr();

 quit:
    return *this;
}

/**
 * @brief  �����ξõ�
 *
 * @param   index0 �õ�ϰ��֤򼨤�����।��ǥå���
 * @param   num_cols �õ�륫����
 * @return  ���Ȥλ���
 */
fits_table &fits_table::erase_cols( long index0, long num_cols )
{
    long i, num_rm;
    long idx_to_rm;

    /* check args */
    if ( index0 < 0 || this->col_size_rec <= index0 ) {
	goto quit;	/* invalid */
    }
    if ( this->col_size_rec < index0 + num_cols ) {
	num_cols = this->col_size_rec - index0;
    }
    if ( num_cols < 0 ) {
	goto quit;	/* invalid */
    }
    if ( num_cols == 0 ) {
	goto quit;
    }

    num_rm = 0;
    idx_to_rm = index0;
    /* �ޤ�������줿���֤��� */
    for ( i=0 ; i < num_cols ; i++ ) {
	fits_table_col *colp = this->cols_ptr_rec[idx_to_rm];
	if ( colp == NULL ||
	     colp->is_protected() == false ) {
	    long sz;
	    if ( colp != NULL ) {
		/* index ���� */
		size_t j;
		int s;
		s = this->index_rec.erase(colp->definition().ttype,
					  idx_to_rm + num_rm);
		if ( s < 0 ) {
		    err_throw(__FUNCTION__,"FATAL",
			       "this->index_rec.erase() failed");
		}
		for ( j=0 ; j < colp->talas.length() ; j++ ) {
		    s = this->index_rec.erase(colp->talas.cstr(j),
					      idx_to_rm + num_rm);
		    if ( s < 0 ) {
			err_throw(__FUNCTION__,"FATAL",
				  "this->index_rec.erase() failed");
		    }
		}
		/* ���Τ��� */
		delete colp;
	    }
	    /* NULL ����ʬ��ޤ�ư�ư */
	    sz = this->col_size_rec - idx_to_rm - num_rm;
	    this->cols_rec.move(idx_to_rm + 1, sz, idx_to_rm, false);

	    /* update system header of this HDU */
	    this->erase_hdr_of_a_col(idx_to_rm + num_rm);

	    num_rm ++;
	}
	else {
	    /* index ��ĥ��ʤ��� */
	    size_t j;
	    int s;
	    s = this->index_rec.update(colp->definition().ttype,
				       idx_to_rm + num_rm, idx_to_rm);
	    if ( s < 0 ) {
	    	err_throw(__FUNCTION__,"FATAL",
			  "Internal ERROR: index_rec.update() failed");
	    }
	    for ( j=0 ; j < colp->talas.length() ; j++ ) {
		s = this->index_rec.update(colp->talas.cstr(j),
					   idx_to_rm + num_rm, idx_to_rm);
		if ( s < 0 ) {
		    err_throw(__FUNCTION__,"FATAL",
			      "Internal ERROR: index_rec.update() failed");
		}
	    }
	    err_report1(__FUNCTION__,"WARNING","column '%s' is protected; "
			"cannot erase", colp->definition().ttype);

	    /* update system header of this HDU */
	    this->renumber_hdr_of_a_col(idx_to_rm + num_rm, idx_to_rm);

	    idx_to_rm ++;
	}
    }
    this->col_size_rec -= num_rm;

    /* index ��ĥ��ʤ��� */
    for ( i=idx_to_rm ; i < this->col_size_rec ; i++ ) {
	fits_table_col *colp = this->cols_ptr_rec[i];
	if ( colp != NULL ) {
	    size_t j;
	    int s;
	    s = this->index_rec.update( colp->definition().ttype,
					i + num_rm, i );
	    if ( s < 0 ) {
	    	err_throw(__FUNCTION__,"FATAL",
			  "Internal ERROR: index_rec.update() failed");
	    }
	    for ( j=0 ; j < colp->talas.length() ; j++ ) {
		s = this->index_rec.update(colp->talas.cstr(j),
					   i + num_rm, i);
		if ( s < 0 ) {
		    err_throw(__FUNCTION__,"FATAL",
			      "Internal ERROR: index_rec.update() failed");
		}
	    }
	}

	/* update system header of this HDU */
	this->renumber_hdr_of_a_col(i + num_rm, i);

    }

    /* update TBCOL, if required */
    this->update_tbcol_hdr();

    /* update NAXIS1 and TFIELD */
    this->update_hdr_of_col_props();

    /* ���ɥ쥹�ơ��֥�򶹤�� */
    try {
	this->cols_rec.resize(this->col_size_rec + 1);
    }
    catch (...) {
	err_throw(__FUNCTION__,"FATAL","cols_rec.resize() failed");
    }

 quit:
    return *this;
}

/**
 * @brief  �����ξõ�
 *
 * @param   col_name �õ�ϰ��֤򼨤������̾
 * @param   num_cols �õ�륫����
 * @return  ���Ȥλ���
 */
fits_table &fits_table::erase_cols( const char *col_name, long num_cols )
{
    return this->erase_cols(this->col_index(col_name),num_cols);
}

/**
 * @brief  �������Ԥ��ɲ�
 *
 *  �ơ��֥�κǸ�ˡ�num_rows �Ĥο������Ԥ��ɲä��ޤ���<br>
 *  ����������ˤϡ��ǥե�����ͤ����åȤ���ޤ����ǥե�����ͤν�������ͤϡ�
 *  �������ξ��� 'F'��ʸ���󷿤ξ��� ' ' ����ʤ�ʸ���󡤤���¾�η��ξ���
 *  0 �Ǥ����ǥե�����ͤ� fits_table_col::assign_default() ���дؿ����ѹ���
 *  ���ޤ���
 *
 * @param   num_rows �ɲä����Կ�
 * @return  ���Ȥλ���
 * @attention  ����Ĺ����ξ�硤���󵭽һҤ��Ф������Ȥʤ�ޤ���
 */
fits_table &fits_table::append_rows( long num_rows )
{
    return this->resize_rows(this->row_size_rec + num_rows);
}

/**
 * @brief  �ơ��֥�Կ����ѹ�
 *
 *  ����������ˤϡ��ǥե�����ͤ����åȤ���ޤ����ǥե�����ͤν�������ͤϡ�
 *  �������ξ��� 'F'��ʸ���󷿤ξ��� ' ' ����ʤ�ʸ���󡤤���¾�η��ξ���
 *  0 �Ǥ����ǥե�����ͤ� fits_table_col::assign_default() ���дؿ����ѹ���
 *  ���ޤ���
 *
 * @param   num_rows �ѹ���ιԿ�
 * @return  ���Ȥλ���
 * @attention  ����Ĺ����ξ�硤���󵭽һҤ��Ф������Ȥʤ�ޤ���
 */
fits_table &fits_table::resize_rows( long num_rows )
{
    long i;

    /* check args */
    if ( num_rows < 0 ) goto quit;	/* invalid */
    if ( this->row_size_rec == num_rows ) {
	goto quit;
    }

    /* resize rows of all columns */
    for ( i=0 ; i < this->col_size_rec ; i++ ) {
	if ( this->cols_ptr_rec[i] != NULL ) {
	    this->cols_ptr_rec[i]->_resize(num_rows);
	}
    }

    /* register */
    this->row_size_rec = num_rows;

    /* update NAXIS2 */
    this->update_hdr_of_row_props();

 quit:
    return *this;
}

/**
 * @brief  �ơ��֥�ιԤ���ԤؤΥ��ԡ�
 *
 * @param   src_index ���ԡ����򼨤��ԥ���ǥå���
 * @param   num_rows ���ԡ���Ԥ��Կ�
 * @param   dest_index ���ԡ���򼨤��ԥ���ǥå���
 * @return  ���Ȥλ���
 * @attention  ����Ĺ����ξ�硤���󵭽һҤ��Ф������Ȥʤ�ޤ���
 */
fits_table &fits_table::move_rows( long src_index, long num_rows,
				   long dest_index )
{
    long i;
    if ( num_rows < 0 ) goto quit;	/* invalid */

    for ( i=0 ; i < this->col_size_rec ; i++ ) {
	if ( this->cols_ptr_rec[i] != NULL ) {
	    this->cols_ptr_rec[i]->move(src_index,num_rows,dest_index);
	}
    }

 quit:
    return *this;
}

/**
 * @brief  �ԤȹԤȤ������ؤ�
 *
 *  ���٤ƤΥ����ˤ����ơ�index0 �ǻ��ꤵ�줿�Ԥ��� num_rows �ĤιԤ�index1
 *  �ǻ��ꤵ�줿�Ԥ��� num_cols �ĤιԤ������ؤ��ޤ���<br>
 *  num_cols �ˤ�äơ��Ťʤ�Ԥ�������ϡ�num_cols ���ͤ򸺤餷�������ؤ���
 *  �Ԥʤ��ޤ���
 *
 * @param   index0 �����ؤ����򼨤��ԥ���ǥå���
 * @param   num_rows �����ؤ���Ԥ��Կ�
 * @param   index1 �����ؤ���򼨤��ԥ���ǥå���
 * @return  ���Ȥλ���
 * @attention  ����Ĺ����ξ�硤���󵭽һҤ��Ф������Ȥʤ�ޤ���
 */
fits_table &fits_table::swap_rows( long index0, long num_rows,
				   long index1 )
{
    long i;
    if ( num_rows < 0 ) goto quit;	/* invalid */

    for ( i=0 ; i < this->col_size_rec ; i++ ) {
	if ( this->cols_ptr_rec[i] != NULL ) {
	    this->cols_ptr_rec[i]->swap(index0,num_rows,index1);
	}
    }

 quit:
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
fits_table &fits_table::flip_rows( long index, long num_rows )
{
    long i;
    if ( num_rows < 0 ) goto quit;	/* invalid */

    for ( i=0 ; i < this->col_size_rec ; i++ ) {
	if ( this->cols_ptr_rec[i] != NULL ) {
	    this->cols_ptr_rec[i]->flip(index, num_rows);
	}
    }

 quit:
    return *this;
}

/**
 * @brief  �Ԥξõ�
 *
 * @param   index ������ϰ��֤򼨤��ԥ���ǥå���
 * @param   num_rows ��������Կ�
 * @return  ���Ȥλ���
 * @attention  �ơ��֥�β������ΥХ��ȿ��ȹԿ����礭�������� index ��������
 *             �ۤɡ�����Υ��ԡ������μ¹ԥ����Ȥ����礷�ޤ���
 * @attention  ����Ĺ����ξ�硤���󵭽һҤ��Ф������Ȥʤ�ޤ���
 */
fits_table &fits_table::erase_rows( long index, long num_rows )
{
    long i;

    /* check args */
    if ( num_rows < 0 ) goto quit;	/* invalid */
    
    /* erase rows of all columns */
    for ( i=0 ; i < this->col_size_rec ; i++ ) {
	if ( this->cols_ptr_rec[i] != NULL ) {
	    this->cols_ptr_rec[i]->_erase(index,num_rows);
	}
    }

    /* register */
    this->row_size_rec -= num_rows;

    /* update NAXIS2 */
    this->update_hdr_of_row_props();

 quit:
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
fits_table &fits_table::clean_rows()
{
    return this->clean_rows(0,this->row_size_rec);
}

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
fits_table &fits_table::clean_rows( long index, long num_rows )
{
    long i;
    if ( num_rows < 0 ) goto quit;	/* invalid */
    
    for ( i=0 ; i < this->col_size_rec ; i++ ) {
	if ( this->cols_ptr_rec[i] != NULL ) {
	    this->cols_ptr_rec[i]->clean(index,num_rows);
	}
    }

 quit:
    return *this;
}

/**
 * @brief  �������Ԥ�����
 *
 *  ����������ˤϡ��ǥե�����ͤ����åȤ���ޤ����ǥե�����ͤν�������ͤϡ�
 *  �������ξ��� 'F'��ʸ���󷿤ξ��� ' ' ����ʤ�ʸ���󡤤���¾�η��ξ���
 *  0 �Ǥ����ǥե�����ͤ� fits_table_col::assign_default() ���дؿ����ѹ���
 *  ���ޤ���
 *
 * @param   index �������֤򼨤��ԥ���ǥå���
 * @param   num_rows ���������Կ�
 * @return  ���Ȥλ���
 * @attention  �ơ��֥�β������ΥХ��ȿ��ȹԿ����礭�������� index ��������
 *             �ۤɡ�����Υ��ԡ������μ¹ԥ����Ȥ����礷�ޤ���
 * @attention  ����Ĺ����ξ�硤���󵭽һҤ��Ф������Ȥʤ�ޤ���
 */
fits_table &fits_table::insert_rows( long index, long num_rows )
{
    long i;

    /* check args */
    if ( num_rows < 0 ) goto quit;	/* invalid */

    /* insert rows for all columns */
    for ( i=0 ; i < this->col_size_rec ; i++ ) {
	if ( this->cols_ptr_rec[i] != NULL ) {
	    this->cols_ptr_rec[i]->_insert(index,num_rows);
	}
    }

    /* register */
    this->row_size_rec += num_rows;

    /* update NAXIS2 */
    this->update_hdr_of_row_props();

 quit:
    return *this;
}

/**
 * @brief  �ơ��֥�Υ���ݡ���
 *
 *  �ơ��֥륪�֥������� from �� idx_begin ���� num_rows �ĤιԤ�dest_index
 *  �ǻ��ꤵ�줿�Ԥ��� num_rows �ĤιԤإ���ݡ��Ȥ��ޤ�������ݡ��Ȥϡ����٤�
 *  �Υ���ब�оݤǤ���<br>
 *  from �Τ��줾��Υ������������֥������ȤΤɤΥ����س�����Ƥ뤫�ϡ�
 *  match_by_name �Ƿ��ޤ���match_by_name �� true �ξ�硤�����̾�����פ���
 *  ��Τ�õ�������פ���Х���ݡ��Ȥ��ޤ���match_by_name �� false �ξ�硤����
 *  ���ֹ� 0 �����˥���ݡ��Ȥ��ޤ���<br>
 *  from �λ��ĥ������������֥������Ȥλ��ĥ����η��ϡ����פ��Ƥ���ɬ�פϤ�
 *  ��ޤ��󡥰��פ��ʤ����ϡ��ͤ��Ѵ����ƥ���ݡ��Ȥ��ޤ���
 *
 * @param   dest_index ����ݡ�����ιԥ���ǥå���
 * @param   match_by_name ����ݡ��Ȼ��Υ����̾���ץե饰
 * @param   from ����ݡ��ȸ��Υơ��֥륪�֥�������
 * @param   idx_begin ����ݡ��ȸ��ιԥ���ǥå���
 * @param   num_rows ����ݡ��Ȥ�Ԥ��Կ�
 * @return  ���Ȥλ���
 * @note  ����ݡ�����(����)�ιԿ����ѹ��ϹԤʤ��ޤ��󡥹Ԥη�»�ʤ�����ݡ���
 *        ����ˤϡ����餫���ώʬ�ʹԿ�����ݤ��Ƥ���ɬ�פ�����ޤ���
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
 */
fits_table &fits_table::import_rows(long dest_index, bool match_by_name,
				    const fits_table &from, 
				    long idx_begin, long num_rows )
{
    long idx0;

    /* check args */
    if ( num_rows == FITS::ALL )
	num_rows = from.row_length() - idx_begin;

    if ( dest_index < 0 || this->row_size_rec <= dest_index || 
	 num_rows < 0 || idx_begin < 0 ) {
	goto quit;	/* invalid */
    }

    if ( from.row_length() <= idx_begin ) {
	goto quit;	/* invalid */
    }
    if ( num_rows == 0 ) {
	goto quit;
    }

    if ( this->row_size_rec < dest_index + num_rows ) {
	num_rows = this->row_size_rec - dest_index;
    }
    if ( from.row_length() < idx_begin + num_rows ) {
	num_rows = from.row_length() - idx_begin;
    }

    /* import */
    for ( idx0 = 0 ; idx0 < this->col_size_rec ; idx0++ ) {
	long idx1 = idx0;
	if ( match_by_name == true ) {
	    idx1 = from.col_index(this->col(idx0).definition().ttype);
	}
	/* match_by_name == false �λ���idx1 ���礭���ʤꤹ����������� */
	if ( idx1 < 0 || from.col_length() <= idx1 ) continue;

	fits_table_col &col0
	    = *(this->cols_ptr_rec[idx0]);
	const fits_table_col &col1
	    = *(from.cols_ptr_rec[idx1]);

	/* note: this rejects variable length array */
	col0.import(dest_index, col1, idx_begin, num_rows);
    }

 quit:
    return *this;
}

/**
 * @brief  1�Ĥο������Ԥ��ɲ�
 *
 * @attention  ����Ĺ����ξ�硤���󵭽һҤ��Ф������Ȥʤ�ޤ���
 */
fits_table &fits_table::append_a_row()
{
    return this->append_rows(1);
}

/**
 * @brief  1�Ĥο������Ԥ�����
 *
 * @param   index �������֤򼨤��ԥ���ǥå���
 * @return  ���Ȥλ���
 * @attention  ����Ĺ����ξ�硤���󵭽һҤ��Ф������Ȥʤ�ޤ���
 */
fits_table &fits_table::insert_a_row( long index )
{
    return this->insert_rows(index,1);
}

/**
 * @brief  1�ĤιԤξõ�
 *
 * @param   index ������֤򼨤��ԥ���ǥå���
 * @return  ���Ȥλ���
 * @attention  ����Ĺ����ξ�硤���󵭽һҤ��Ф������Ȥʤ�ޤ���
 */
fits_table &fits_table::erase_a_row( long index )
{
    return this->erase_rows(index,1);
}

/**
 * @brief  1�ĤιԤΥ�����ͤ�����(�ǥե�����ͤ򥻥å�)
 *
 *  �ǥե�����ͤν�������ͤϡ��������ξ��� 'F'��ʸ���󷿤ξ��� ' ' ����
 *  �ʤ�ʸ���󡤤���¾�η��ξ��� 0 �Ǥ����ǥե�����ͤ� 
 *  fits_table_col::assign_default() ���дؿ����ѹ��Ǥ��ޤ���
 *
 * @param   index ��������֤򼨤��ԥ���ǥå���
 * @return  ���Ȥλ���
 * @attention  ����Ĺ����ξ�硤���󵭽һҤ��Ф������Ȥʤ�ޤ���
 */
fits_table &fits_table::clean_a_row( long index )
{
    return this->clean_rows(index,1);
}

/**
 * @brief  �ᥤ��ơ��֥��ѥ��������ˡ��������ѹ� (������������)
 *
 *  �ᥤ��ơ��֥��ѥ������ݤ�����ˡ��ɤΤ褦����ˡ�ǹԤʤ�������ꤷ�ޤ���
 *  ���� 3 �Ĥ������򤷤ޤ���<br>
 *    "min", "auto"  ... �Ǿ��¤���ݡ��ꥵ��������ɬ��realloc()���ƤФ��<br>
 *    "pow"  ... 2��n��ǳ���<br>
 *    NULL��¾ ... ���ߤ���ˡ��ݻ�
 *
 * @param   strategy ���������ˡ������
 * @return  ���Ȥλ���
 *
 */
fits_table &fits_table::set_alloc_strategy_of_rows( const char *strategy )
{
    long i;
    for ( i=0 ; i < this->col_length() ; i++ ) {
	this->col(i).set_alloc_strategy(strategy);
    }
    return *this;
}


/* protected */
/**
 * @brief  �������ɲ� (���٥�)
 *
 * @param   defs fits::table_def_all ��¤�Τ����� ({NULL}�ǽ�ü)
 * @return  ���Ȥλ���
 * @note   ���Υ��дؿ��� protected �Ǥ���
 */
fits_table &fits_table::_append_cols( const fits::table_def_all defs[] )
{
    fits_table_col *tmp_rec_ptr;
    long num_defs, i;
    long begin_index = this->col_size_rec;

    /* check args */
    if ( defs == NULL ) goto quit;
    for ( num_defs=0 ; defs[num_defs].ttype != NULL ; num_defs++ );
    if ( num_defs == 0 ) goto quit;

    /* ���ɥ쥹�ơ��֥�򹭤��� */
    try {
	this->cols_rec.resize(this->col_size_rec + num_defs + 1);
    }
    catch (...) {
	err_throw(__FUNCTION__,"FATAL","cols_rec.resize() failed");
    }

    /* 1��1�� new ���� */
    for ( i=0 ; i < num_defs ; i++ ) {
	size_t j;
	int s;
	//err_report1(__FUNCTION__,"DEBUG","appending... %ld",i);
	try {
	    tmp_rec_ptr = new fits_table_col;
	}
	catch (...) {
	    err_throw(__FUNCTION__,"FATAL","'new' failed");
	}
	try {
	    tmp_rec_ptr->register_manager(this);
	    tmp_rec_ptr->_define(defs[i]);
	    tmp_rec_ptr->assign_null_svalue(this->default_null_svalue());
	}
	catch (...) {
	    delete tmp_rec_ptr;
	    err_throw(__FUNCTION__,"FATAL","tmp_rec_ptr->_define() failed");
	}
	if ( 0 <= this->index_rec.index(tmp_rec_ptr->definition().ttype,0) ) {
	    err_report1(__FUNCTION__,"WARNING",
		 "duplicated column name (ttype) '%s' is appended",
			tmp_rec_ptr->definition().ttype);
	}
	s = this->index_rec.append( tmp_rec_ptr->definition().ttype,
				    begin_index + i );
	if ( s < 0 ) {
	    delete tmp_rec_ptr;
	    err_throw(__FUNCTION__,"FATAL","this->index_rec.append() failed");
	}
	for ( j=0 ; j < tmp_rec_ptr->talas.length() ; j++ ) {
	    s = this->index_rec.append( tmp_rec_ptr->talas.cstr(j),
					begin_index + i );
	    if ( s < 0 ) {
		delete tmp_rec_ptr;
		err_throw(__FUNCTION__,"FATAL","index_rec.append() failed");
	    }
	}

	/* ASCII �ơ��֥��Ŭ�礷�ʤ����� BINARY ��°���ѹ� */
	if ( tmp_rec_ptr->type() != FITS::STRING_T )
	    this->set_hdutype(FITS::BINARY_TABLE_HDU);
	else {
	    if ( tmp_rec_ptr->dcol_length() != 1 )
		this->set_hdutype(FITS::BINARY_TABLE_HDU);
	    if ( tmp_rec_ptr->elem_length() != 1 )
		this->set_hdutype(FITS::BINARY_TABLE_HDU);
	}

	/* register! */
	this->cols_ptr_rec[begin_index + i] = tmp_rec_ptr;
	tmp_rec_ptr = NULL;

	this->col_size_rec ++;
    }

    /* row ��������Ĵ������ */
    for ( i=0 ; i < num_defs ; i++ ) {
	if ( this->cols_ptr_rec[begin_index + i] != NULL ) {
	    this->cols_ptr_rec[begin_index + i]->_resize(this->row_size_rec);
	}
    }

 quit:
    return *this;
}

/* public */
/**
 * @brief  �������ɲ� (fits::table_def_all ��¤�Τǻ���)
 *
 * @param   defs fits::table_def_all ��¤�Τ����� ({NULL}�ǽ�ü)
 * @return  ���Ȥλ���
 */
fits_table &fits_table::append_cols( const fits::table_def_all defs[] )
{
    long num_defs, i;
    long begin_index = this->col_size_rec;

    /* check args */
    if ( defs == NULL ) goto quit;
    for ( num_defs=0 ; defs[num_defs].ttype != NULL ; num_defs++ );
    if ( num_defs == 0 ) goto quit;

    /* */
    this->_append_cols(defs);

    /* setup column header */
    for ( i=0 ; i < num_defs ; i++ ) {
	this->update_hdr_of_a_col(begin_index + i, NULL, NULL);
    }

    /* update TBCOL, if required */
    this->update_tbcol_hdr();

    /* update NAXIS1 and TFIELD */
    this->update_hdr_of_col_props();

 quit:
    return *this;
}

/* public */
/**
 * @brief  �������ɲ� (fits::table_def ��¤�Τǻ���)
 *
 * @param   defs fits::table_def ��¤�Τ����� ({NULL}�ǽ�ü)
 * @return  ���Ȥλ���
 */
fits_table &fits_table::append_cols( const fits::table_def defs[] )
{
    long i, num_defs = 0;
    fits::table_def_all *p;
    mdarray drec(sizeof(fits::table_def_all), true, (void *)(&p));

    if ( defs == NULL ) {
	goto quit;
    }

    for ( num_defs=0 ; defs[num_defs].ttype != NULL ; num_defs++ );

    if ( num_defs == 0 ) {
	goto quit;
    }

    drec.resize(num_defs + 1);

    for ( i=0 ; i < num_defs ; i++ ) {
	p[i].ttype         = defs[i].ttype;
	p[i].ttype_comment = defs[i].ttype_comment;
	p[i].talas         = defs[i].talas;
	p[i].talas_comment = NULL;
	p[i].telem         = defs[i].telem;
	p[i].telem_comment = NULL;
	p[i].tunit         = defs[i].tunit;
	p[i].tunit_comment = defs[i].tunit_comment;
	p[i].tdisp         = defs[i].tdisp;
	p[i].tdisp_comment = NULL;
	p[i].tform         = defs[i].tform;
	p[i].tform_comment = NULL;
	p[i].tdim          = defs[i].tdim;
	p[i].tdim_comment  = NULL;
	p[i].tnull         = defs[i].tnull;
	p[i].tnull_comment = NULL;
	p[i].tzero         = defs[i].tzero;
	p[i].tzero_comment = NULL;
	p[i].tscal         = defs[i].tscal;
	p[i].tscal_comment = NULL;
    }
    p[i].ttype = NULL;	/* term */

    this->append_cols(p);

 quit:
    return *this;
}

/**
 * @brief  �������ɲäȥ����ͤΥ��ԡ�
 *
 *  src �λ��ĥ���������Ǽ��Ȥ˿����������ɲä��������ͤ򥳥ԡ�
 *  ���ޤ���<br>
 *  ���ȤιԿ�����ʬ�Ǥʤ����Ϥ��٤ƤιԤ����ԡ�����ޤ���
 *
 * @param   src ���ԡ����Ȥʤ륫������ĥ��֥�������
 * @return  ���Ȥλ���
 */
fits_table &fits_table::append_cols( const fits_table &src )
{
    long i;
    long col_len = src.col_length();	/* &src == this �Ǥ��ɤ��褦�� */

    for ( i=0 ; i < col_len ; i++ ) {
	this->append_a_col(src.col_cs(i));
    }

    return *this;
}

/**
 * @brief  ���������� (fits::table_def ��¤�Τǻ���)
 *
 * @param   index0 �������֤򼨤�����।��ǥå���
 * @param   defs fits::table_def ��¤�Τ����� ({NULL}�ǽ�ü)
 * @return  ���Ȥλ���
 */
fits_table &fits_table::insert_cols( long index0, const fits::table_def defs[] )
{
    long i, num_defs = 0;
    fits::table_def_all *p;
    mdarray drec(sizeof(fits::table_def_all), true, (void *)(&p));

    if ( defs == NULL ) {
	goto quit;
    }

    for ( num_defs=0 ; defs[num_defs].ttype != NULL ; num_defs++ );

    if ( num_defs == 0 ) {
	goto quit;
    }

    drec.resize(num_defs + 1);

    for ( i=0 ; i < num_defs ; i++ ) {
	p[i].ttype         = defs[i].ttype;
	p[i].ttype_comment = defs[i].ttype_comment;
	p[i].talas         = defs[i].talas;
	p[i].talas_comment = NULL;
	p[i].telem         = defs[i].telem;
	p[i].telem_comment = NULL;
	p[i].tunit         = defs[i].tunit;
	p[i].tunit_comment = defs[i].tunit_comment;
	p[i].tdisp         = defs[i].tdisp;
	p[i].tdisp_comment = NULL;
	p[i].tform         = defs[i].tform;
	p[i].tform_comment = NULL;
	p[i].tdim          = defs[i].tdim;
	p[i].tdim_comment  = NULL;
	p[i].tnull         = defs[i].tnull;
	p[i].tnull_comment = NULL;
	p[i].tzero         = defs[i].tzero;
	p[i].tzero_comment = NULL;
	p[i].tscal         = defs[i].tscal;
	p[i].tscal_comment = NULL;
    }
    p[i].ttype = NULL;	/* term */

    this->insert_cols(index0, p);

 quit:
    return *this;
}

/**
 * @brief  ���������� (fits::table_def_all ��¤�Τǻ���)
 *
 * @param   index0 �������֤򼨤�����।��ǥå���
 * @param   defs fits::table_def_all ��¤�Τ����� ({NULL}�ǽ�ü)
 * @return  ���Ȥλ���
 */
fits_table &fits_table::insert_cols( long index0, const fits::table_def_all defs[] )
{
    fits_table_col *tmp_rec_ptr;
    long num_defs, i;

    /* check args */
    if ( index0 < 0 || this->col_size_rec < index0 ) {
	goto quit;	/* invalid */
    }
    if ( defs == NULL ) {
	goto quit;
    }
    if ( this->col_size_rec == index0 ) {
	return this->append_cols(defs);
    }
    for ( num_defs=0 ; defs[num_defs].ttype != NULL ; num_defs++ );
    if ( num_defs == 0 ) {
	goto quit;
    }

    /* ���ɥ쥹�ơ��֥�򹭤��� */
    try {
	this->cols_rec.resize(this->col_size_rec + num_defs + 1);
    }
    catch (...) {
	err_throw(__FUNCTION__,"FATAL","cols_rec.resize() failed");
    }

    /* 1��1�� new ���� */
    for ( i=0 ; i < num_defs ; i++ ) {
	int s;
	long j;
	size_t k;
	try {
	    tmp_rec_ptr = new fits_table_col;
	}
	catch (...) {
	    err_throw(__FUNCTION__,"FATAL","'new' failed");
	}
	try {
	    tmp_rec_ptr->register_manager(this);
	    tmp_rec_ptr->_define(defs[i]);
	    tmp_rec_ptr->assign_null_svalue(this->default_null_svalue());
	}
	catch (...) {
	    delete tmp_rec_ptr;
	    err_throw(__FUNCTION__,"FATAL"," tmp_rec_ptr->_define() failed");
	}
	if ( 0 <= this->index_rec.index(tmp_rec_ptr->definition().ttype,0) ) {
	    err_report1(__FUNCTION__,"WARNING",
			"duplicated column name (ttype) '%s' is inserted",
			tmp_rec_ptr->definition().ttype);
	}
	/* ���餷��(��ü��NULL��ʬ��)����֤���Ͽ���� */
	this->cols_rec.move(index0 + i, this->col_size_rec - i - index0 + 1,
			    index0 + i + 1, false);
	this->cols_ptr_rec[index0 + i] = tmp_rec_ptr;
	this->col_size_rec ++;

	/* ���餷���Ȥ���� index ����ʤ��� */
	for ( j = index0 + i + 1 ; j < this->col_size_rec ; j++ ) {
	    fits_table_col *colp = this->cols_ptr_rec[j];
	    s = this->index_rec.update(colp->definition().ttype, j-1, j);
	    if ( s < 0 ) {
		err_throw(__FUNCTION__,"FATAL",
			  "Internal ERROR: index_rec.update() failed");
	    }
	    for ( k=0 ; k < colp->talas.length() ; k++ ) {
		s = this->index_rec.update(colp->talas.cstr(k), j-1, j);
		if ( s < 0 ) {
		    err_throw(__FUNCTION__,"FATAL",
			      "Internal ERROR: index_rec.update() failed");
		}
	    }
	}

	/* insert ������ĤΥ���ǥå������������ */
	s = this->index_rec.append(tmp_rec_ptr->definition().ttype, index0 +i);
	if ( s < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","index_rec.append() failed");
	}
	for ( k=0 ; k < tmp_rec_ptr->talas.length() ; k++ ) {
	    s = this->index_rec.append(tmp_rec_ptr->talas.cstr(k),index0+i);
	    if ( s < 0 ) {
		err_throw(__FUNCTION__,"FATAL","index_rec.append() failed");
	    }
	}

	/* ASCII �ơ��֥��Ŭ�礷�ʤ����� BINARY ��°���ѹ� */
	if ( tmp_rec_ptr->type() != FITS::STRING_T )
	    this->set_hdutype(FITS::BINARY_TABLE_HDU);
	else {
	    if ( tmp_rec_ptr->dcol_length() != 1 )
		this->set_hdutype(FITS::BINARY_TABLE_HDU);
	    if ( tmp_rec_ptr->elem_length() != 1 )
		this->set_hdutype(FITS::BINARY_TABLE_HDU);
	}
    }

    /* row ��������Ĵ������ */
    for ( i=0 ; i < num_defs ; i++ ) {
	if ( this->cols_ptr_rec[index0 + i] != NULL ) {
	    this->cols_ptr_rec[index0 + i]->_resize(this->row_size_rec);
	}
    }

    /* adjust header */
    for ( i = this->col_size_rec ; index0 + num_defs < i ; ) {
	i --;
	/* renumber header */
	this->renumber_hdr_of_a_col(i - num_defs, i);
	//err_report1(__FUNCTION__,"DEBUG","renumber : %ld", i - num_defs);
	//err_report1(__FUNCTION__,"DEBUG","        -> %ld", i);
	//{
	//    long j;
	//    for ( j=0 ; j < this->header_length() ; j++ ) {
	//	err_report1(__FUNCTION__,"DEBUG","HDR: %s", 
	//		    this->header(j).keyword());
	//    }
	//}
    }
    for ( i = index0 ; i < index0 + num_defs ; i++ ) {
	/* update a column header */
	this->update_hdr_of_a_col(i, NULL, NULL);
	//err_report1(__FUNCTION__,"DEBUG","update : %ld", i);
    }

    /* update TBCOL, if required */
    this->update_tbcol_hdr();

    /* update NAXIS1 and TFIELD */
    this->update_hdr_of_col_props();

 quit:
    return *this;
}

/**
 * @brief  ���������� (fits::table_def ��¤�Τǻ���)
 *
 * @param   col_name �������֤򼨤������̾
 * @param   defs fits::table_def ��¤�Τ����� ({NULL}�ǽ�ü)
 * @return  ���Ȥλ���
 */
fits_table &fits_table::insert_cols( const char *col_name, 
				     const fits::table_def defs[] )
{
    return this->insert_cols(this->col_index(col_name),defs);
}

/**
 * @brief  ���������� (fits::table_def_all ��¤�Τǻ���)
 *
 * @param   col_name �������֤򼨤������̾
 * @param   defs fits::table_def_all ��¤�Τ����� ({NULL}�ǽ�ü)
 * @return  ���Ȥλ���
 */
fits_table &fits_table::insert_cols( const char *col_name, 
				     const fits::table_def_all defs[] )
{
    return this->insert_cols(this->col_index(col_name),defs);
}

/**
 * @brief  �����������ȥ����ͤΥ��ԡ�
 *
 *  src �λ��ĥ���������Ǽ��Ȥ˿����������������������ͤ򥳥ԡ�
 *  ���ޤ���<br>
 *  ���ȤιԿ�����ʬ�Ǥʤ����Ϥ��٤ƤιԤ����ԡ�����ޤ���
 *
 * @param   index0 �������֤򼨤�����।��ǥå���
 * @param   src ���ԡ����Ȥʤ륫������ĥ��֥�������
 * @return  ���Ȥλ���
 */
fits_table &fits_table::insert_cols( long index0, const fits_table &src )
{
    long i;
    const fits_table_col **atbl_ptr;
    mdarray atbl(sizeof(const fits_table_col *), true, &atbl_ptr); /* [void **] */
    long col_len = src.col_length();

    /* &src == this �Ǥ��ɤ��褦�� */
    atbl.resize(col_len);
    for ( i=0 ; i < col_len ; i++ ) {
	const fits_table_col &ref = src.col_cs(i);
	atbl_ptr[i] = &ref;
    }

    for ( i=col_len ; 0 < i ; ) {
	i--;
	this->insert_a_col(index0, *(atbl_ptr[i]));
    }

    return *this;
    
}

/**
 * @brief  �����������ȥ����ͤΥ��ԡ�
 *
 *  src �λ��ĥ���������Ǽ��Ȥ˿����������������������ͤ򥳥ԡ�
 *  ���ޤ���<br>
 *  ���ȤιԿ�����ʬ�Ǥʤ����Ϥ��٤ƤιԤ����ԡ�����ޤ���
 *
 * @param   col_name �������֤򼨤������̾
 * @param   src ���ԡ����Ȥʤ륫������ĥ��֥�������
 * @return  ���Ȥλ���
 */
fits_table &fits_table::insert_cols( const char *col_name, 
				     const fits_table &src )
{
    return this->insert_cols(this->col_index(col_name),src);
}


/* public */
/**
 * @brief  1�ĤΥ������ɲ� (fits::table_def ��¤�Τǻ���)
 *
 * @param   def fits::table_def ��¤��
 * @return  ���Ȥλ���
 */
fits_table &fits_table::append_a_col( const fits::table_def &def )
{
    const fits::table_def defs[] = { def, {NULL} };

    return this->append_cols(defs);
}

/**
 * @brief  1�ĤΥ������ɲ� (fits::table_def_all ��¤�Τǻ���)
 *
 * @param   def fits::table_def_all ��¤��
 * @return  ���Ȥλ���
 */
fits_table &fits_table::append_a_col( const fits::table_def_all &def )
{
    const fits::table_def_all defs[] = { def, {NULL} };

    return this->append_cols(defs);
}

/**
 * @brief  1�ĤΥ������ɲäȥ����ͤΥ��ԡ�
 *
 *  src �λ��ĥ���������Ǽ��Ȥ˿����������ɲä��������ͤ򥳥ԡ�
 *  ���ޤ���<br>
 *  ���ȤιԿ�����ʬ�Ǥʤ����Ϥ��٤ƤιԤ����ԡ�����ޤ���
 *
 * @param   src ���ԡ����Ȥʤ륫������ĥ��֥�������
 * @return  ���Ȥλ���
 */
fits_table &fits_table::append_a_col( const fits_table_col &src )
{
    const fits::table_def_all defs[] = { src.definition_all(), {NULL} };
    long old_ncol = this->col_length();
    this->append_cols(defs);
    if ( old_ncol < this->col_length() ) {
	long idx = this->col_length() - 1;
	size_t sz0 = this->col(idx).elem_byte_length() * this->col(idx).length();
	size_t sz1 = src.elem_byte_length() * src.length();
	this->col(idx).data_rec.putdata( src.data_ptr_cs(),
					 sz0 < sz1 ? sz0 : sz1 );
    }
    return *this;
}

/**
 * @brief  1�ĤΥ��������� (fits::table_def ��¤�Τǻ���)
 *
 * @param   col_index �������֤򼨤�����।��ǥå���
 * @param   def fits::table_def ��¤��
 * @return  ���Ȥλ���
 */
fits_table &fits_table::insert_a_col( long col_index, 
				      const fits::table_def &def )
{
    const fits::table_def defs[] = { def, {NULL} };

    return this->insert_cols(col_index,defs);
}

/**
 * @brief  1�ĤΥ��������� (fits::table_def_all ��¤�Τǻ���)
 *
 * @param   col_index �������֤򼨤�����।��ǥå���
 * @param   def fits::table_def_all ��¤��
 * @return  ���Ȥλ���
 */
fits_table &fits_table::insert_a_col( long col_index, 
				      const fits::table_def_all &def )
{
    const fits::table_def_all defs[] = { def, {NULL} };

    return this->insert_cols(col_index,defs);
}

/**
 * @brief  1�ĤΥ����������ȥ����ͤΥ��ԡ�
 *
 *  src �λ��ĥ���������Ǽ��Ȥ˿����������������������ͤ򥳥ԡ�
 *  ���ޤ���<br>
 *  ���ȤιԿ�����ʬ�Ǥʤ����Ϥ��٤ƤιԤ����ԡ�����ޤ���
 *
 * @param   col_index �������֤򼨤�����।��ǥå���
 * @param   src ���ԡ����Ȥʤ륫������ĥ��֥�������
 * @return  ���Ȥλ���
 */
fits_table &fits_table::insert_a_col( long col_index, 
				      const fits_table_col &src )
{
    const fits::table_def_all defs[] = { src.definition_all(), {NULL} };
    long old_ncol = this->col_length();
    this->insert_cols(col_index,defs);
    if ( old_ncol < this->col_length() ) {
	long idx;
	if ( this->col_length() <= col_index ) idx = this->col_length() - 1;
	else if ( col_index < 0 ) idx = 0;
	else idx = col_index;
	size_t sz0 = this->col(idx).elem_byte_length() * this->col(idx).length();
	size_t sz1 = src.elem_byte_length() * src.length();
	this->col(idx).data_rec.putdata( src.data_ptr_cs(),
					 sz0 < sz1 ? sz0 : sz1 );
    }
    return *this;
}

/**
 * @brief  1�ĤΥ����������ȥ����ͤΥ��ԡ�
 *
 *  src �λ��ĥ���������Ǽ��Ȥ˿����������������������ͤ򥳥ԡ�
 *  ���ޤ���<br>
 *  ���ȤιԿ�����ʬ�Ǥʤ����Ϥ��٤ƤιԤ����ԡ�����ޤ���
 *
 * @param   col_name �������֤򼨤������̾
 * @param   src ���ԡ����Ȥʤ륫������ĥ��֥�������
 * @return  ���Ȥλ���
 */
fits_table &fits_table::insert_a_col( const char *col_name, 
				      const fits_table_col &src )
{
    return this->insert_a_col(this->col_index(col_name),src);
}

/**
 * @deprecated ��侩��<br>
 * fits_table &fits_table::put_a_col( long, const fits_table_col & ) <br>
 * �򤪻Ȥ�����������
 */
fits_table &fits_table::paste_a_col( long col_index, 
				     const fits_table_col &src )
{
    const fits::table_def &def = src.definition();
    /* check args */
    if ( col_index < 0 || this->col_size_rec <= col_index ) {
	goto quit;	/* invalid */
    }
    this->define_a_col(col_index, def);
    {
	long idx = col_index;
	size_t sz0 = this->col(idx).elem_byte_length() * this->col(idx).length();
	size_t sz1 = src.elem_byte_length() * src.length();
	this->col(idx).data_rec.putdata( src.data_ptr_cs(),
					 sz0 < sz1 ? sz0 : sz1 );
    }
 quit:
    return *this;
}

/**
 * @deprecated ��侩��<br>
 * fits_table &fits_table::put_a_col(const char *, const fits_table_col &) <br>
 * �򤪻Ȥ�����������
 */
fits_table &fits_table::paste_a_col( const char *col_name, 
				     const fits_table_col &src)
{
    return this->paste_a_col(this->col_index(col_name),src);
}

/**
 * @brief  1�ĤΥ��������� (fits::table_def ��¤�Τǻ���)
 *
 * @param   col_name �������֤򼨤������̾
 * @param   def fits::table_def ��¤��
 * @return  ���Ȥλ���
 */
fits_table &fits_table::insert_a_col( const char *col_name, 
				      const fits::table_def &def )
{
    const fits::table_def defs[] = { def, {NULL} };

    return this->insert_cols(this->col_index(col_name),defs);
}

/**
 * @brief  1�ĤΥ��������� (fits::table_def_all ��¤�Τǻ���)
 *
 * @param   col_name �������֤򼨤������̾
 * @param   def fits::table_def_all ��¤��
 * @return  ���Ȥλ���
 */
fits_table &fits_table::insert_a_col( const char *col_name, 
				      const fits::table_def_all &def )
{
    const fits::table_def_all defs[] = { def, {NULL} };

    return this->insert_cols(this->col_index(col_name),defs);
}

/**
 * @brief  1�ĤΥ����ξõ�
 *
 * @param   col_index �õ���֤򼨤�����।��ǥå���
 * @return  ���Ȥλ���
 */
fits_table &fits_table::erase_a_col( long col_index )
{
    return this->erase_cols(col_index,1);
}

/**
 * @brief  1�ĤΥ����ξõ�
 *
 * @param   col_name �õ���֤򼨤������̾
 * @return  ���Ȥλ���
 */
fits_table &fits_table::erase_a_col( const char *col_name )
{
    return this->erase_cols(this->col_index(col_name),1);
}

/**
 * @brief  ������������ѹ� (fits::table_def ��¤�Τǻ���)
 *
 *  col_index �Ǽ����줿������������ѹ����ޤ���<br>
 *  ������ѹ��������ʤ����ܤˤĤ��Ƥϡ�def �Υ��Ф� NULL �򥻥åȤ��ޤ���
 *
 * @param   col_index ����।��ǥå���
 * @param   def ��������
 * @return  ���Ȥλ���
 */
fits_table &fits_table::define_a_col( long col_index, 
				      const fits::table_def &def )
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

    return this->define_a_col(col_index, def_all);
}

/**
 * @brief  ������������ѹ� (fits::table_def_all ��¤�Τǻ���)
 *
 *  col_index �Ǽ����줿������������ѹ����ޤ���<br>
 *  ������ѹ��������ʤ����ܤˤĤ��Ƥϡ�def �Υ��Ф� NULL �򥻥åȤ��ޤ���
 *
 * @param   col_index ����।��ǥå���
 * @param   def ��������
 * @return  ���Ȥλ���
 */
fits_table &fits_table::define_a_col( long col_index, 
				      const fits::table_def_all &def )
{
    //err_report1(__FUNCTION__,"DEBUG","called!! col_index = %ld",col_index);
    tstring old_ttype;
    tarray_tstring old_talas;
    bool updated;

    /* check args */
    if ( col_index < 0 || this->col_size_rec <= col_index ) {
	goto quit;	/* invalid */
    }

    /* ���Υ����̾(��̾��aliases)����¸���� */
    old_ttype.assign(this->col(col_index).definition().ttype);
    //old_talas.assign(this->col(col_index).definition().talas);
    if ( this->col(col_index).definition().talas != NULL ) {
	if (this->col(col_index).definition().talas[0] == '\0' ) {
	    old_talas.resize(0);
	}
	else {
	    old_talas.explode(this->col(col_index).definition().talas,",",true);
	    old_talas.trim();
	}
    }

    /* ������������ѹ����� */
    if ( this->cols_ptr_rec[col_index] != NULL ) {
	this->cols_ptr_rec[col_index]->_define(def);
    }

    /* �����̾���Ѥ�äƤ���С�����ǥå������ѹ����� */
    if ( old_ttype.strcmp(this->col(col_index).definition().ttype) != 0 ) {
	if ( this->index_rec.erase(old_ttype.cstr(),col_index) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","this->index_rec.erase() failed");
	}
	if ( this->index_rec.append(this->col(col_index).definition().ttype,
				    col_index) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","this->index_rec.append() failed");
	}
    }

    /* TALAS ���ѹ��������äƤʤ��������å����� */
    updated = true;
    if ( old_talas.length() == this->cols_ptr_rec[col_index]->talas.length() ) {
	size_t i;
	for ( i=0 ; i < old_talas.length() ; i++ ) {
	    if ( old_talas.at(i).strcmp(
			  this->cols_ptr_rec[col_index]->talas.cstr(i)) != 0 ) {
		break;
	    }
	}
	if ( i == old_talas.length() ) updated = false;
    }
    if ( updated != false ) {
	size_t i;
	for ( i=0 ; i < old_talas.length() ; i++ ) {
	    if ( this->index_rec.erase(old_talas.cstr(i),col_index) < 0 ) {
		err_throw(__FUNCTION__,"FATAL","index_rec.erase() failed");
	    }
	}
	for ( i=0 ; i < this->cols_ptr_rec[col_index]->talas.length() ; i++ ) {
	    if ( this->index_rec.append(
		 this->cols_ptr_rec[col_index]->talas.cstr(i),col_index) < 0 ) {
		err_throw(__FUNCTION__,"FATAL","index_rec.append() failed");
	    }
	}
    }

    /* ASCII �ơ��֥��Ŭ�礷�ʤ����� BINARY ��°���ѹ� */
    if ( this->cols_ptr_rec[col_index]->type() != FITS::STRING_T )
	this->set_hdutype(FITS::BINARY_TABLE_HDU);
    else {
	if ( this->cols_ptr_rec[col_index]->dcol_length() != 1 )
	    this->set_hdutype(FITS::BINARY_TABLE_HDU);
	if ( this->cols_ptr_rec[col_index]->elem_length() != 1 )
	    this->set_hdutype(FITS::BINARY_TABLE_HDU);
    }

    /* update column header */
    this->update_hdr_of_a_col(col_index, NULL, NULL);

    /* update TBCOL, if required */
    this->update_tbcol_hdr();

    /* update NAXIS1 and TFIELD */
    this->update_hdr_of_col_props();

 quit:
    return *this;
}

/**
 * @brief  ������������ѹ� (fits::table_def ��¤�Τǻ���)
 *
 *  col_name �Ǽ����줿������������ѹ����ޤ���<br>
 *  ������ѹ��������ʤ����ܤˤĤ��Ƥϡ�def �Υ��Ф� NULL �򥻥åȤ��ޤ���
 *
 * @param   col_name �����̾
 * @param   def ��������
 * @return  ���Ȥλ���
 */
fits_table &fits_table::define_a_col( const char *col_name, 
				      const fits::table_def &def )
{
    return this->define_a_col(this->col_index(col_name),def);
}

/**
 * @brief  ������������ѹ� (fits::table_def_all ��¤�Τǻ���)
 *
 *  col_name �Ǽ����줿������������ѹ����ޤ���<br>
 *  ������ѹ��������ʤ����ܤˤĤ��Ƥϡ�def �Υ��Ф� NULL �򥻥åȤ��ޤ���
 *
 * @param   col_name �����̾
 * @param   def ��������
 * @return  ���Ȥλ���
 */
fits_table &fits_table::define_a_col( const char *col_name, 
				      const fits::table_def_all &def )
{
    return this->define_a_col(this->col_index(col_name),def);
}

/**
 * @brief  �桼������Υ���७����ɤΥݥ�����������
 */
const char *const *fits_table::user_col_keywords() const
{
    return this->user_fld_kwd_rec.cstrarray();
}

/**
 * @brief  �桼������Υ���७����ɤ���¸���Ƥ��륪�֥������Ȥλ��Ȥ����
 */
const tarray_tstring &fits_table::user_col_keywords_cs() const
{
    return this->user_fld_kwd_rec;
}

/**
 * @brief  �����Υ�����ɤ��桼������Υ���७����ɤ��ɤ������֤�
 */
bool fits_table::is_user_col_keyword( const char *kwd ) const
{
    tstring ckeywd;
    size_t spn;

    /* check args */
    if ( kwd == NULL ) return false;

    ckeywd.assign(kwd);
    spn = ckeywd.strrspn("0123456789#");
    if ( ckeywd.length() <= spn ) return false;
    ckeywd.resize( ckeywd.length() - spn );

    if ( this->user_fld_kwd_rec.find_elem(ckeywd) < 0 ) return false;
    else return true;
}

/**
 * @brief  �桼������Υ���७����ɤ�1���ɲ�
 */
fits_table &fits_table::append_a_user_col_keyword( const char *kwd )
{
    if ( this->user_fld_kwd_rec.find_elem(kwd) < 0 ) {
	fits_header_record dummy;
	tstring new_ckeywd;
	size_t spn;

	/* filter and check kwd */
	dummy.assign(kwd,"","");
	new_ckeywd = dummy.keyword();
	if ( 0 < (spn=new_ckeywd.strrspn("#")) ) {
	    new_ckeywd.resize(new_ckeywd.length() - spn);
	}
	if ( new_ckeywd.length() == 0 ||
	     0 < new_ckeywd.strrspn("0123456789") ) {
	    err_report1(__FUNCTION__,"ERROR",
			"new column keyword '%s' is invalid",kwd);
	    goto quit;
	}
	this->user_fld_kwd_rec.append(new_ckeywd,1);
    }
 quit:
    return *this;
}

/**
 * @brief  �桼������Υ���७����ɤ�1�ĺ��
 */
fits_table &fits_table::erase_a_user_col_keyword( const char *kwd )
{
    ssize_t i;
    if ( 0 <= (i=this->user_fld_kwd_rec.find_elem(kwd)) ) {
	this->user_fld_kwd_rec.erase(i,1);
    }
    return *this;
}

/**
 * @brief  ʣ���Υ桼������Υ���७����ɤ��ɲ�
 */
fits_table &fits_table::append_user_col_keywords( const char *const kwds[] )
{
    if ( kwds != NULL ) {
	size_t i;
	for ( i=0 ; kwds[i] != NULL ; i++ ) {
	    if ( this->user_fld_kwd_rec.find_elem(kwds[i]) < 0 ) {
		this->append_a_user_col_keyword(kwds[i]);
	    }
	}
    }
    return *this;
}

/**
 * @brief  ʣ���Υ桼������Υ���७����ɤ���
 */
fits_table &fits_table::erase_user_col_keywords( const char *const kwds[] )
{
    if ( kwds != NULL ) {
	size_t i;
	for ( i=0 ; kwds[i] != NULL ; i++ ) {
	    ssize_t j;
	    if ( 0 <= (j=this->user_fld_kwd_rec.find_elem(kwds[i])) ) {
		this->user_fld_kwd_rec.erase(j,1);
	    }
	}
    }
    return *this;
}

/**
 * @brief  ���ꤵ�줿���������Υإå��쥳���ɤ��ֹ���֤�
 *
 * ��:  long idx = fits.table("EVENT").col_header_index("DATE", "TTYPE");
 */
long fits_table::col_header_index( const char *col_name, 
				   const char *kwd ) const
{
    return this->col_header_index(this->col_index(col_name), kwd);
}

/**
 * @brief  ���ꤵ�줿���������Υإå��쥳���ɤ��ֹ���֤�
 */
long fits_table::col_header_index( long col_index, 
				   const char *kwd ) const
{
    const fits_header &hdr_rec = this->header_rec;
    long return_idx = -1;
    bool is_col_kwd;
    tstring keyword;
    size_t i;

    /* check args */
    if ( col_index < 0 || this->col_size_rec <= col_index ) {	/* invalid */
	goto quit;
    }
    if ( kwd == NULL ) goto quit;

    /* check kwd is reserved or user-defined */
    keyword = kwd;
    is_col_kwd = false;
    for ( i = 0 ; Field_keywords[i] != NULL ; i++ ) {
	if ( keyword.strcmp(Field_keywords[i]) == 0 ) {
	    is_col_kwd = true;
	    break;
	}
    }
    if ( is_col_kwd != true ) {
	for ( i=0 ; i < this->user_fld_kwd_rec.length() ; i++ ) {
	    if ( keyword.strcmp(this->user_fld_kwd_rec.cstr(i)) == 0 ) {
		is_col_kwd = true;
		break;
	    }
	}
    }

    if ( is_col_kwd == true ) {
	keyword.printf("%s%ld", kwd, col_index + 1);
	return_idx = hdr_rec.index(keyword.cstr());
    }

 quit:
    return return_idx;
}

/**
 * @brief  ����������Υإå��쥳���ɤؤλ��Ȥ����
 *
 * ��:<br>
 *    printf("TTYPE of 'TIME' = %s\n", <br>
 *    fits.table("EVENT").col_header("TIME","TTYPE").svalue());
 */
fits_header_record &fits_table::col_header( const char *col_name, 
    					    const char *kwd )
{
    return this->col_header(this->col_index(col_name), kwd);
}

/**
 * @brief  ����������Υإå��쥳���ɤؤλ��Ȥ����
 */
fits_header_record &fits_table::col_header( long col_index, 
    					    const char *kwd )
{
    long hdr_idx = this->col_header_index(col_index, kwd);
    if ( hdr_idx < 0 ) {	/* invalid */
	err_throw(__FUNCTION__,"ERROR", "invalid col_index or kwd");
    }
    else {
	fits_header &hdr_rec = this->header_rec;
	return hdr_rec.record(hdr_idx);
    }
}

#ifdef SLI__OVERLOAD_CONST_AT
/**
 * @brief  ����������Υإå��쥳���ɤؤλ��Ȥ���� (�ɼ�����)
 */
const fits_header_record &fits_table::col_header( const char *col_name, 
    						  const char *kwd ) const
{
    return this->col_header_cs(this->col_index(col_name), kwd);
}

/**
 * @brief  ����������Υإå��쥳���ɤؤλ��Ȥ���� (�ɼ�����)
 */
const fits_header_record &fits_table::col_header( long col_index, 
    						  const char *kwd ) const
{
    return this->col_header_cs(col_index, kwd);
}
#endif

/**
 * @brief  ����������Υإå��쥳���ɤؤλ��Ȥ���� (�ɼ�����)
 */
const fits_header_record &fits_table::col_header_cs( const char *col_name, 
    						     const char *kwd ) const
{
    return this->col_header_cs(this->col_index(col_name), kwd);
}

/**
 * @brief  ����������Υإå��쥳���ɤؤλ��Ȥ���� (�ɼ�����)
 */
const fits_header_record &fits_table::col_header_cs( long col_index, 
    						     const char *kwd ) const
{
    long hdr_idx = this->col_header_index(col_index, kwd);
    if ( hdr_idx < 0 ) {	/* invalid */
	err_throw(__FUNCTION__,"ERROR", "invalid col_index or kwd");
    }
    else {
	const fits_header &hdr_rec = this->header_rec;
	return hdr_rec.record_cs(hdr_idx);
    }
}

/**
 * @brief  �桼������Υ���७����ɤ����ƺ��
 */
fits_table &fits_table::erase_user_col_keywords()
{
    this->user_fld_kwd_rec.init();
    return *this;
}

/**
 * @brief  ����������Υإå��쥳���ɤι���
 *
 * ��:  fits.table("EVENT").update_col_header("TIME","TUNIT","s","unit");
 */
fits_table &fits_table::update_col_header( const char *col_name, 
			    const char *kwd, const char *val, const char *com )
{
    return this->update_col_header(this->col_index(col_name), kwd, val, com);
}

/**
 * @brief  ����������Υإå��쥳���ɤι���
 */
fits_table &fits_table::update_col_header( long col_index,
			    const char *kwd, const char *val, const char *com )
{
    fits_header &hdr_rec = this->header_rec;
    tstring keyword, ckeywd;
    fits_header_record dummy;
    tarray_tstring arr;
    size_t i;
    bool found;
    long idx, max_idx;
    size_t spn;

    /* check args */
    if ( col_index < 0 || this->col_size_rec <= col_index ) {	/* invalid */
	goto quit;
    }
    if ( kwd == NULL || ( val == NULL && com == NULL ) ) goto quit;

    /* filter and check kwd */
    dummy.assign(kwd, "", "");
    ckeywd = dummy.keyword();
    if ( 0 < (spn=ckeywd.strrspn("#")) ) {
	ckeywd.resize(ckeywd.length() - spn);
    }
    if ( ckeywd.length() == 0 || 0 < ckeywd.strrspn("0123456789") ) {
	err_report1(__FUNCTION__,"ERROR",
		    "column keyword '%s' is invalid",kwd);
	goto quit;
    }

    /* reserved keywords */
    found = false;
    for ( i = 0 ; Field_keywords[i] != NULL ; i++ ) {
	if ( ckeywd.strcmp(Field_keywords[i]) == 0 ) {
	    fits::table_def_all def = {NULL};
	    if      ( ckeywd == "TTYPE" ) {
		def.ttype = val;  def.ttype_comment = com;
	    }
	    else if ( ckeywd == "TDISP" && 
		      this->hdutype() != FITS::ASCII_TABLE_HDU ) {
		def.tdisp = val;  def.tdisp_comment = com;
	    }
	    else if ( ckeywd == "TFORM" &&
		      this->hdutype() != FITS::ASCII_TABLE_HDU ) {
		def.tform = val;  def.tform_comment = com;
	    }
	    else if ( ckeywd == "TUNIT" ) {
		def.tunit = val;  def.tunit_comment = com;
	    }
	    else if ( ckeywd == "TDIM" ) {
		def.tdim  = val;  def.tdim_comment = com;
	    }
	    else if ( ckeywd == "TZERO" ) {
		def.tzero = val;  def.tzero_comment = com;
	    }
	    else if ( ckeywd == "TSCAL" ) {
		def.tscal = val;  def.tscal_comment = com;
	    }
	    else if ( ckeywd == "TNULL" ) {
		def.tnull = val;  def.tnull_comment = com;
	    }
	    else if ( ckeywd == "TALAS" ) {
		//if ( val != NULL ) {
		//    arr.split(val, ",", true);
		//    def.talas = arr.cstrarray();  def.talas_comment = com;
		//}
		//else {
		//    def.talas = NULL;  def.talas_comment = com;
		//}
		def.talas = val;  def.talas_comment = com;
	    }
	    else if ( ckeywd == "TELEM" ) {
		//if ( val != NULL ) {
		//    arr.split(val, ",", true);
		//    def.telem = arr.cstrarray();  def.telem_comment = com;
		//}
		//else {
		//    def.telem = NULL;  def.telem_comment = com;
		//}
		def.telem = val;  def.telem_comment = com;
	    }
	    else {
		err_report1(__FUNCTION__,"WARNING",
			    "cannot update '%s' in this member function",kwd);
		goto quit;
	    }
	    found = true;
	    this->define_a_col(col_index, def);
	    break;
	}
    }

    if ( found == true ) goto quit;				/* OK */

    /* user keywords */
    found = false;
    for ( i=0 ; i < this->user_fld_kwd_rec.length() ; i++ ) {
	if ( ckeywd.strcmp(this->user_fld_kwd_rec.cstr(i)) == 0 ) {
	    found = true;
	    break;
	}
    }
    /* register new keyword, if not found */
    if ( found == false ) {
	this->user_fld_kwd_rec.append(ckeywd, 1);
    }

    /* actual keyword to set */
    keyword.printf("%s%ld", ckeywd.cstr(), col_index + 1);

    idx = hdr_rec.index(keyword.cstr());
    if ( 0 <= idx ) {
	hdr_rec.update(keyword.cstr(), val, com);
    }
    else {
	tstring tmp_kwd;
	/* search insert point */
	idx = 0;
	max_idx = 0;
	for ( i = 0 ; Field_keywords[i] != NULL ; i++ ) {
	    tmp_kwd.printf("%s%ld", Field_keywords[i], col_index + 1);
	    if ( 0 <= (idx = hdr_rec.index(tmp_kwd.cstr())) ) {
		if ( max_idx < idx ) max_idx = idx;
	    }
	}
	for ( i=0 ; i < this->user_fld_kwd_rec.length() ; i++ ) {
	    if ( this->user_fld_kwd_rec[i].strcmp(ckeywd) == 0 ) break;
	    tmp_kwd.printf("%s%ld", 
			   this->user_fld_kwd_rec.cstr(i), col_index + 1);
	    if ( 0 <= (idx = hdr_rec.index(tmp_kwd.cstr())) ) {
		if ( max_idx < idx ) max_idx = idx;
	    }
	}
	if ( 0 < max_idx ) idx = max_idx + 1;
	else idx = this->find_colhdr_insert_point(col_index);	/* fallback */
	/* insert new user keyword */
	hdr_rec.insert(idx, keyword.cstr(), val, com);
    }

 quit:
    return *this;
}

/**
 * @brief  �桼������Υ�����ѥإå�������ɤ�̾��
 *
 * ��:  fits.table("EVENT").rename_col_header("TLMAX","TMMAX");
 */
fits_table &fits_table::rename_col_header( const char *old_kwd, 
					   const char *new_kwd )
{
    fits_header &hdr_rec = this->header_rec;
    ssize_t ix;
    long i;
    tstring new_keyword, old_ckeywd, new_ckeywd;
    fits_header_record dummy;
    size_t spn;

    /* check old_kwd */
    dummy.assign(old_kwd, "", "");
    old_ckeywd = dummy.keyword();
    if ( 0 < (spn=old_ckeywd.strrspn("#")) ) {
	old_ckeywd.resize(old_ckeywd.length() - spn);
    }
    if ( old_ckeywd.length() == 0 ) {
	err_report1(__FUNCTION__,"ERROR",
		    "new column keyword '%s' is invalid",old_kwd);
	goto quit;
    }

    /* check args */
    ix = this->user_fld_kwd_rec.find_elem(old_ckeywd);
    if ( ix < 0 ) {
	err_report1(__FUNCTION__,"WARNING",
		    "column keyword '%s' is not defined",old_kwd);
	goto quit;
    }

    if ( new_kwd == NULL ) goto quit;

    /* check new_kwd */
    dummy.assign(new_kwd, "", "");
    new_ckeywd = dummy.keyword();
    if ( 0 < (spn=new_ckeywd.strrspn("#")) ) {
	new_ckeywd.resize(new_ckeywd.length() - spn);
    }
    if ( new_ckeywd.length() == 0 || 0 < new_ckeywd.strrspn("0123456789") ) {
	err_report1(__FUNCTION__,"ERROR",
		    "new column keyword '%s' is invalid",new_kwd);
	goto quit;
    }
    for ( i = 0 ; Field_keywords[i] != NULL ; i++ ) {
	if ( new_ckeywd.strcmp(Field_keywords[i]) == 0 ) {
	    err_report1(__FUNCTION__,"ERROR",
			"new column keyword '%s' is invalid",new_kwd);
	    goto quit;
	}
    }
    
    /* replace all */
    for ( i=0 ; i < this->col_size_rec ; i++ ) {
	long hdr_idx = this->col_header_index(i, old_ckeywd.cstr());
	if ( 0 <= hdr_idx ) {
	    new_keyword.printf("%s%ld", new_ckeywd.cstr(), i + 1);
	    hdr_rec.rename(hdr_idx, new_keyword.cstr());
	}
    }

    /* register */
    this->user_fld_kwd_rec[ix].assign(new_ckeywd);

 quit:
    return *this;
}

/**
 * @brief  ����������Υإå��쥳���ɤκ��
 *
 * ��:  fits.table("EVENT").erase_col_header("TIME","TUNIT");
 */
fits_table &fits_table::erase_col_header( const char *col_name, 
					  const char *kwd )
{
    return this->erase_col_header(this->col_index(col_name), kwd);
}

/**
 * @brief  ����������Υإå��쥳���ɤκ��
 */
fits_table &fits_table::erase_col_header( long col_index, 
					  const char *kwd )
{
    fits_header &hdr_rec = this->header_rec;
    tstring keyword, tmp_kwd;
    tarray_tstring arr;
    size_t i;
    long idx, j;
    bool found;

    /* check args */
    if ( col_index < 0 || this->col_size_rec <= col_index ) {	/* invalid */
	goto quit;
    }
    if ( kwd == NULL ) goto quit;

    keyword = kwd;

    /* reserved keywords */
    found = false;
    for ( i = 0 ; Field_keywords[i] != NULL ; i++ ) {
	if ( keyword.strcmp(Field_keywords[i]) == 0 ) {
	    fits::table_def_all def = {NULL};
	    if      ( keyword == "TTYPE" ) {
		def.ttype = "";  def.ttype_comment = "";
	    }
	    else if ( keyword == "TDISP" && 
		      this->hdutype() != FITS::ASCII_TABLE_HDU ) {
		def.tdisp = "";  def.tdisp_comment = "";
	    }
	    else if ( keyword == "TUNIT" ) {
		def.tunit = "";  def.tunit_comment = "";
	    }
	    else if ( keyword == "TDIM" ) {
		def.tdim  = "";  def.tdim_comment = "";
	    }
	    else if ( keyword == "TZERO" ) {
		def.tzero = "";  def.tzero_comment = "";
	    }
	    else if ( keyword == "TSCAL" ) {
		def.tscal = "";  def.tscal_comment = "";
	    }
	    else if ( keyword == "TNULL" ) {
		def.tnull = "";  def.tnull_comment = "";
	    }
	    else if ( keyword == "TALAS" ) {
		//arr.erase();
		//def.talas = arr.cstrarray();  def.talas_comment = "";
		def.talas = "";  def.talas_comment = "";
	    }
	    else if ( keyword == "TELEM" ) {
		//arr.erase();
		//def.telem = arr.cstrarray();  def.telem_comment = "";
		def.telem = "";  def.telem_comment = "";
	    }
	    else {
		err_report1(__FUNCTION__,"WARNING",
			    "cannot update '%s' in this member function",kwd);
		goto quit;
	    }
	    found = true;
	    this->define_a_col(col_index, def);
	    break;
	}
    }

    if ( found == true ) goto quit;				/* OK */

    /* actual keyword to erase */
    keyword.printf("%s%ld", kwd, col_index + 1);

    /* erase, if exists */
    idx = hdr_rec.index(keyword.cstr());
    if ( idx < 0 ) goto quit;
    hdr_rec.erase(idx);

    /* search keywords */
    found = false;
    for ( j=0 ; j < this->col_size_rec ; j++ ) {
	tmp_kwd.printf("%s%ld", kwd, j + 1);
	if ( 0 <= hdr_rec.index(tmp_kwd.cstr()) ) {
	    found = true;
	    break;
	}
    }

    /* erase user's list, if not found */
    if ( found == false ) {
	ssize_t ix;
	ix = this->user_fld_kwd_rec.find_elem(kwd);
	if ( 0 <= ix ) this->user_fld_kwd_rec.erase(ix,1);
    }

 quit:
    return *this;
}

/**
 * @brief  ������ѥإå�������ɤ򥫥���˥�����
 */
fits_table &fits_table::sort_col_header()
{
    fits_header &hdr_rec = this->header_rec;
    long cursor, i;
    tstring tmp_kwd, tmp_val, tmp_com;

    this->header_set_sysrecords_prohibition(false);
    try {
	cursor = -1;
	/* loop for columns */
	for ( i=0 ; i < this->col_size_rec ; i++ ) {
	    size_t j;
	    /* loop for reserved keywords */
	    for ( j = 0 ; Field_keywords[j] != NULL ; j++ ) {
		long idx;
		tmp_kwd.printf("%s%ld", Field_keywords[j], i + 1);
		if ( 0 <= (idx = hdr_rec.index(tmp_kwd.cstr())) ) {
		    if ( cursor < 0 ) cursor = idx + 1;
		    else {
			if ( cursor != idx ) {
			    tmp_val = hdr_rec.record(idx).value();
			    tmp_com = hdr_rec.record(idx).comment();
			    hdr_rec.erase(idx);
			    if ( idx < cursor ) cursor --;
			    hdr_rec.insert(cursor, tmp_kwd.cstr(), 
					   tmp_val.cstr(), tmp_com.cstr());
			}
			cursor ++;
		    }
		}
	    }
	    /* loop for user's keywords */
	    for ( j=0 ; j < this->user_fld_kwd_rec.length() ; j++ ) {
		long idx;
		tmp_kwd.printf("%s%ld", this->user_fld_kwd_rec.cstr(j), i + 1);
		if ( 0 <= (idx = hdr_rec.index(tmp_kwd.cstr())) ) {
		    if ( cursor < 0 ) cursor = idx + 1;
		    else {
			if ( cursor != idx ) {
			    tmp_val = hdr_rec.record(idx).value();
			    tmp_com = hdr_rec.record(idx).comment();
			    hdr_rec.erase(idx);
			    if ( idx < cursor ) cursor --;
			    hdr_rec.insert(cursor, tmp_kwd.cstr(), 
					   tmp_val.cstr(), tmp_com.cstr());
			}
			cursor ++;
		    }
		}
	    }
	}
    }
    catch (...) {
	this->header_set_sysrecords_prohibition(true);
	err_throw(__FUNCTION__,"FATAL","caught exception");
    }
    this->header_set_sysrecords_prohibition(true);

    return *this;
}


/* public */
/**
 * @brief  ���֥������ȤΥ��ԡ�
 *
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
 */
void fits_table::copy( fits_table *dest ) const
{
    if ( dest != NULL ) this->copy(0L, FITS::ALL, dest);
    return;
}

/**
 * @brief  �����ιԤ��̥��֥������Ȥإ��ԡ�
 *
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
 */
void fits_table::copy( long idx_begin, long num_rows, fits_table *dest ) const
{
    long max_rows, i;
    if ( num_rows == FITS::ALL ) num_rows = this->row_length();

    if ( idx_begin < 0 ) idx_begin = 0;
    else if ( this->row_length() < idx_begin ) idx_begin = this->row_length();

    max_rows = this->row_length() - idx_begin;
    if ( num_rows < 0 ) num_rows = 0;
    else if ( max_rows < num_rows ) num_rows = max_rows;

    if ( dest != NULL ) {
	if ( dest != this ) {
	    dest->fits_table::init();
	    dest->fits_hdu::init(*this);
	    for ( i=0 ; i < this->col_length() ; i++ ) {
		dest->append_a_col(this->col_cs(i).definition());
	    }
	    dest->resize_rows(num_rows);
	    /* note: this rejects variable length array */
	    dest->import_rows(0,false,*this,idx_begin,num_rows);
	}
	else {
	    dest->resize_rows(idx_begin + num_rows);
	    dest->erase_rows(0, idx_begin);
	}
    }

    return;
}

/* not recommended */
/**
 * @deprecated ��侩��<br>
 * void fits_table::copy( fits_table *dest ) const <br>
 * �򤪻Ȥ�����������
 */
void fits_table::copy( fits_table &dest ) const
{
    this->copy(0L, FITS::ALL, &dest);
    return;
}

/* not recommended */
/**
 * @deprecated ��侩��<br>
 * void fits_table::copy( long, long, fits_table * ) const <br>
 * �򤪻Ȥ�����������
 */
void fits_table::copy( long idx_begin, long num_rows, fits_table &dest ) const
{
    this->copy(idx_begin, num_rows, &dest);
    return;
}

/* protected */
/**
 * @deprecated  ���Ѷػߡ�
 * @note  ���Υ��дؿ��� protected �Ǥ���
 */
void fits_table::copy( long idx_begin, fits_table *dest ) const
{
    this->copy(idx_begin, FITS::ALL, dest);
    return;
}

/**
 * @deprecated  ���Ѷػߡ�
 * @note  ���Υ��дؿ��� protected �Ǥ���
 */
void fits_table::copy( const char *name_begin, long num_rows, 
		       fits_table *dest ) const
{
    long idx;
    idx = this->col_index(name_begin);
    this->copy(idx, num_rows, dest);
    return;
}

/**
 * @deprecated  ���Ѷػߡ�
 * @note  ���Υ��дؿ��� protected �Ǥ���
 */
void fits_table::copy_cols( const char *const col_names[], 
			    fits_table *dest ) const
{
    long i;

    if ( dest != NULL ) {
	dest->init();
	dest->resize_rows(this->row_size_rec);
	if ( col_names != NULL ) {
	    for ( i=0 ; col_names[i] != NULL ; i++ ) {
		long j = this->col_index(col_names[i]);
		if ( 0 <= j ) {
		    dest->append_a_col(this->col_cs(j));
		}
	    }
	}
    }
    return;
}

/**
 * @deprecated  ���Ѷػߡ�
 * @note  ���Υ��дؿ��� protected �Ǥ���
 */
void fits_table::copy_cols( fits_table *dest,
			    const char *col0, ... ) const
{
    va_list ap;
    va_start(ap,col0);
    try {
	this->va_copy_cols(dest,col0,ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","va_copy_cols() failed");
    }
    va_end(ap);

    return;
}

/**
 * @deprecated  ���Ѷػߡ�
 * @note  ���Υ��дؿ��� protected �Ǥ���
 */
void fits_table::va_copy_cols( fits_table *dest,
			       const char *col0, va_list ap ) const
{
    va_list aq;
    long i, num_cols;
    const char **col_names;
    mdarray col_names_rec(sizeof(*col_names), true, &col_names); /* [void **] */

    num_cols = 0;
    va_copy(aq, ap);
    if ( col0 != NULL ) {
	for ( num_cols = 1 ; ; num_cols++ ) {
	    const char *col = va_arg(aq,const char *);
	    if ( col == NULL ) break;
	}
    }
    va_end(aq);

    try {
	col_names_rec.resize(num_cols + 1);
    }
    catch ( ... ) {
	err_throw(__FUNCTION__,"FATAL","col_names_rec.resize() failed");
    }

    i=0;
    col_names[i] = col0;
    if ( col0 != NULL ) {
	for ( i=1 ; i < num_cols ; i++ ) {
	    const char *col = va_arg(ap,const char *);
	    if ( col == NULL ) break;
	}
	col_names[i] = NULL;
    }

    this->copy_cols(col_names, dest);

    return;
}

/**
 * @deprecated  ���Ѷػߡ�
 * @note  ���Υ��дؿ��� protected �Ǥ���
 */
void fits_table::copy_cols( const long col_idxs[], long num_cols,
			    fits_table *dest ) const
{
    long i;

    if ( dest != NULL ) {
	dest->init();
	dest->resize_rows(this->row_size_rec);
	if ( col_idxs != NULL ) {
	    for ( i=0 ; i < num_cols ; i++ ) {
		long j = col_idxs[i];
		if ( 0 <= j && j < this->col_size_rec ) {
		    dest->append_a_col(this->col_cs(j));
		}
	    }
	}
    }
    return;
}

/**
 * @deprecated  ���Ѷػߡ�
 * @note  ���Υ��дؿ��� protected �Ǥ���
 */
void fits_table::copy_cols( fits_table *dest,
			    long num_cols, ... ) const
{
    va_list ap;
    va_start(ap,num_cols);
    try {
	this->va_copy_cols(dest,num_cols,ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","va_copy_cols() failed");
    }
    va_end(ap);

    return;
}

/**
 * @deprecated  ���Ѷػߡ�
 * @note  ���Υ��дؿ��� protected �Ǥ���
 */
void fits_table::va_copy_cols( fits_table *dest,
			       long num_cols, va_list ap ) const
{
    long i;
    mdarray col_idxs(sizeof(long), true);

    if ( num_cols <= 0 )
	return this->copy_cols(dest,0L,(const long *)NULL);

    try {
	col_idxs.resize(num_cols);
    }
    catch ( ... ) {
	err_throw(__FUNCTION__,"FATAL","col_idxs.resize() failed");
    }

    for ( i=0 ; i < num_cols ; i++ ) {
	long idx = va_arg(ap,long);
	col_idxs.l(i) = idx;
    }

    this->copy_cols((const long *)col_idxs.data_ptr_cs(), num_cols, dest);

    return;
}

/**
 * @brief  ���Ȥδ������ˤ��� fits_table_col ���֥������Ȥؤλ��Ȥ��֤�
 */
fits_table_col &fits_table::col( long col_index )
{
    if ( col_index < 0 || this->col_size_rec <= col_index ) {
	err_throw1(__FUNCTION__,"ERROR","Invalid col_index: %ld",col_index);
    }
    return *(this->cols_ptr_rec[col_index]);
}

/**
 * @brief  ���Ȥδ������ˤ��� fits_table_col ���֥������Ȥؤλ��Ȥ��֤�
 */
fits_table_col &fits_table::col( const char *col_name )
{
    long idx;
    if ( col_name == NULL ) {
	err_throw(__FUNCTION__,"ERROR","NULL col_name ... ?");
    }
    idx = this->col_index(col_name);
#if 1	/* ����Ϥ��ޤ�򤷤��ʤ�����ʤ��� */
    if ( idx < 0 ) {
	fits::table_def def = {col_name,"",NULL,NULL, NULL,NULL,NULL,"1A"};
	this->append_a_col(def);
	idx = this->col_index(col_name);
    }
#endif
    if ( idx < 0 ) {
	err_throw1(__FUNCTION__,"ERROR","column '%s' is not found",col_name);
    }
    return this->col( idx );
}

/**
 * @brief  fits_table_col ���֥������Ȥؤλ��Ȥ��֤� (printf()�ε�ˡ)
 */
fits_table_col &fits_table::colf( const char *fmt, ... )
{
    va_list ap;
    va_start(ap,fmt);
    try {
	fits_table_col &ret = this->vcolf(fmt,ap);
	va_end(ap);
	return ret;
    }
    catch (...) {
	va_end(ap);
        err_throw(__FUNCTION__,"FATAL","this->vcolf() failed");
    }
}

/**
 * @brief  fits_table_col ���֥������Ȥؤλ��Ȥ��֤� (printf()�ε�ˡ)
 */
fits_table_col &fits_table::vcolf( const char *fmt, va_list ap )
{
    tstring buf;

    if ( fmt == NULL ) return this->col(fmt);

    buf.vprintf(fmt,ap);
    return this->col(buf.cstr());
}

#ifdef SLI__OVERLOAD_CONST_AT
/**
 * @brief  fits_table_col ���֥������Ȥؤλ��Ȥ��֤� (�ɼ�����)
 */
const fits_table_col &fits_table::col( long col_index ) const
{
    return this->col_cs(col_index);
}

/**
 * @brief  fits_table_col ���֥������Ȥؤλ��Ȥ��֤� (�ɼ�����)
 */
const fits_table_col &fits_table::col( const char *col_name ) const
{
    return this->col_cs(col_name);
}

/**
 * @brief  fits_table_col ���֥������Ȥؤλ��Ȥ��֤� (printf()�ε�ˡ���ɼ�����)
 */
const fits_table_col &fits_table::colf( const char *fmt, ... ) const
{
    va_list ap;
    va_start(ap,fmt);
    try {
	const fits_table_col &ret = this->vcolf_cs(fmt,ap);
	va_end(ap);
	return ret;
    }
    catch (...) {
	va_end(ap);
        err_throw(__FUNCTION__,"FATAL","this->vcolf_cs() failed");
    }
}

/**
 * @brief  fits_table_col ���֥������Ȥؤλ��Ȥ��֤� (printf()�ε�ˡ���ɼ�����)
 */
const fits_table_col &fits_table::vcolf( const char *fmt, va_list ap ) const
{
    return this->vcolf_cs(fmt, ap);
}
#endif

/**
 * @brief  fits_table_col ���֥������Ȥؤλ��Ȥ��֤� (�ɼ�����)
 */
const fits_table_col &fits_table::col_cs( long col_index ) const
{
    if ( col_index < 0 || this->col_size_rec <= col_index ) {
	err_throw1(__FUNCTION__,"ERROR","Invalid col_index: %ld",col_index);
    }
    return *(this->cols_ptr_rec[col_index]);
}

/**
 * @brief  fits_table_col ���֥������Ȥؤλ��Ȥ��֤� (�ɼ�����)
 */
const fits_table_col &fits_table::col_cs( const char *col_name ) const
{
    long idx;
    if ( col_name == NULL ) {
	err_throw(__FUNCTION__,"ERROR","NULL col_name ... ?");
    }
    idx = this->col_index(col_name);
    return this->col_cs( idx );
}

/**
 * @brief  fits_table_col ���֥������Ȥؤλ��Ȥ��֤� (printf()�ε�ˡ���ɼ�����)
 */
const fits_table_col &fits_table::colf_cs( const char *fmt, ... ) const
{
    va_list ap;
    va_start(ap,fmt);
    try {
	const fits_table_col &ret = this->vcolf_cs(fmt,ap);
	va_end(ap);
	return ret;
    }
    catch (...) {
	va_end(ap);
        err_throw(__FUNCTION__,"FATAL","this->vcolf_cs() failed");
    }
}

/**
 * @brief  fits_table_col ���֥������Ȥؤλ��Ȥ��֤� (printf()�ε�ˡ���ɼ�����)
 */
const fits_table_col &fits_table::vcolf_cs( const char *fmt, va_list ap ) const
{
    tstring buf;

    if ( fmt == NULL ) return this->col_cs(fmt);

    buf.vprintf(fmt,ap);
    return this->col_cs(buf.cstr());
}

/**
 * @brief  1�ĤΥ����ξ��
 *
 *  ���ꤵ�줿������ src �λ��ĥ���������Ǿ�񤭤��������ͤ򥳥ԡ�
 *  ���ޤ���<br>
 *  ���ȤιԿ�����ʬ�Ǥʤ����Ϥ��٤ƤιԤ����ԡ�����ޤ���
 *
 * @param   col_index ��񤭤���륫��।��ǥå���
 * @param   src ���ԡ����Ȥʤ륫������ĥ��֥�������
 * @return  ���Ȥλ���
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
 */
fits_table &fits_table::put_a_col( long col_index, const fits_table_col &src )
{
    if ( col_index < 0 || this->col_size_rec <= col_index ) {
	goto quit;	/* invalid */
    }

#if 0	/* ->init(src) �� define_a_col() ���ƤФ��Τ�ɬ�פʤ� */
    /* ���Υ����̾(��̾��aliases)����¸���� */
    tstring old_ttype;
    tarray_tstring old_talas;
    old_ttype.assign(this->col(col_index).definition().ttype);
    old_talas.assign( this->col(col_index).definition().talas );
#endif

    this->cols_ptr_rec[col_index]->init( src );
    this->cols_ptr_rec[col_index]->_resize( this->row_size_rec );

#if 0	/* ->init(col) �� define_a_col() ���ƤФ��Τ�ɬ�פʤ� */
    /* �����̾���Ѥ�äƤ���С�����ǥå������ѹ����� */
    if ( old_ttype.strcmp(this->col(col_index).definition().ttype) != 0 ) {
	if ( this->index_rec.erase(old_ttype.cstr(),col_index) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","this->index_rec.erase() failed");
	}
	if ( this->index_rec.append(this->col(col_index).definition().ttype,
				    col_index) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","this->index_rec.append() failed");
	}
    }
    
    /* TALAS ���ѹ��������äƤʤ��������å����� */
    bool updated = true;
    if ( old_talas.length() == this->cols_ptr_rec[col_index]->talas.length() ) {
	size_t i;
	for ( i=0 ; i < old_talas.length() ; i++ ) {
	    if ( old_talas.at(i).strcmp(
			  this->cols_ptr_rec[col_index]->talas.cstr(i)) != 0 ) {
		break;
	    }
	}
	if ( i == old_talas.length() ) updated = false;
    }
    if ( updated != false ) {
	size_t i;
	for ( i=0 ; i < old_talas.length() ; i++ ) {
	    if ( this->index_rec.erase(old_talas.cstr(i),col_index) < 0 ) {
		err_throw(__FUNCTION__,"FATAL","index_rec.erase() failed");
	    }
	}
	for ( i=0 ; i < this->cols_ptr_rec[col_index]->talas.length() ; i++ ) {
	    if ( this->index_rec.append(
		 this->cols_ptr_rec[col_index]->talas.cstr(i),col_index) < 0 ) {
		err_throw(__FUNCTION__,"FATAL","index_rec.append() failed");
	    }
	}
    }
#endif

 quit:
    return *this;
}

/**
 * @brief  1�ĤΥ����ξ��
 *
 *  ���ꤵ�줿������ src �λ��ĥ���������Ǿ�񤭤��������ͤ򥳥ԡ�
 *  ���ޤ���<br>
 *  ���ȤιԿ�����ʬ�Ǥʤ����Ϥ��٤ƤιԤ����ԡ�����ޤ���
 *
 * @param   col_name ��񤭤���륫���̾
 * @param   src ���ԡ����Ȥʤ륫������ĥ��֥�������
 * @return  ���Ȥλ���
 * @attention  ����Ĺ����ϥ��ݡ��Ȥ���Ƥ��ޤ���
 */
fits_table &fits_table::put_a_col( const char *col_name, 
				   const fits_table_col &src )
{
    return this->put_a_col( this->col_index(col_name), src );
}

/**
 * @brief  ����Ĺ�����ѤΥҡ��ץǡ��������Хåե��إ��ԡ�
 *
 *  �ҡ��ץХåե��ϥӥå�����ǥ�����ǥǡ�������Ǽ����Ƥ��뤿�ᡤdest_buf ��
 *  ���ԡ����줿�ǡ�����ɬ�פ˱����ƥХ��ȥ����������Ѵ����ƻȤ��ޤ���
 */
ssize_t fits_table::get_heap( void *dest_buf, size_t buf_size ) const
{
    return this->get_heap(0,dest_buf,buf_size);
}

/**
 * @brief  ����Ĺ�����ѤΥҡ��ץǡ��������Хåե��إ��ԡ�
 *
 *  �ҡ��ץХåե��ϥӥå�����ǥ�����ǥǡ�������Ǽ����Ƥ��뤿�ᡤdest_buf ��
 *  ���ԡ����줿�ǡ�����ɬ�פ˱����ƥХ��ȥ����������Ѵ����ƻȤ��ޤ���
 */
ssize_t fits_table::get_heap( long offset, 
			      void *dest_buf, size_t buf_size ) const
{
    return this->heap_rec.getdata(dest_buf, buf_size, offset);
}

/**
 * @brief  �����Хåե��Υǡ��������Ĺ�����ѤΥҡ����ΰ�˥��ԡ�
 *
 *  �ҡ��ץХåե��ϥӥå�����ǥ�����ǥǡ�������Ǽ���ʤ���Фʤ�ʤ����ᡤ
 *  �����ƥХ��ȥ����������Ѵ���ɬ�פǤ����Х��ȥ����������Ѵ��ϡ����餫����
 *  src_buf �ǻ��ꤵ���ǡ������Ф��ƹԤʤ���ˡ�ȡ�fits_table::put_heap() ��
 *  ��� fits_table::reverse_heap_endian() �ǹԤʤ���ˡ������ޤ���
 */
ssize_t fits_table::put_heap( const void *src_buf, size_t buf_size )
{
    return this->put_heap(0,src_buf,buf_size);
}

/**
 * @brief  �����Хåե��Υǡ��������Ĺ�����ѤΥҡ����ΰ�˥��ԡ�
 *
 *  �ҡ��ץХåե��ϥӥå�����ǥ�����ǥǡ�������Ǽ���ʤ���Фʤ�ʤ����ᡤ
 *  src_buf �ǻ��ꤵ�줿�ǡ�����ɬ�פ˱����ƥХ��ȥ����������Ѵ����Ƥ����ޤ���
 */
ssize_t fits_table::put_heap( long offset, 
			      const void *src_buf, size_t buf_size )
{
    return this->heap_rec.putdata(src_buf, buf_size, offset);
}

/**
 * @brief  �ҡ����ΰ�ΰ����ΥХ��ȥ�������ȿž
 *
 *  ���֥������������Υҡ��ץХåե��Υ��ɥ쥹 offset ���� �Ϥޤ�Х��ȥǡ���
 *  �򡤥ǡ������� type �Ȥ���Ĺ�� length ������Ȥߤʤ���ɬ�פ˱����Ƥ�����ʬ
 *  �ΥХ��ȥ�������ȿž�����ޤ���<br>
 *  ���� type �˻���Ǥ����ͤϼ��ΤȤ���Ǥ�: <br>
 *  FITS::SHORT_T��FITS::LONG_T��FITS::LONGLONG_T��FITS::FLOAT_T��
 *  FITS::DOUBLE_T��FITS::COMPLEX_T��FITS::DOUBLECOMPLEX_T��<br>
 *  �Х��ȥ�������ȿž���Ԥʤ���Τϡ����ꤵ�줿�ǡ������η׻����ˤ�����Х�
 *  �ȥ������� little-endian �ξ��Ǥ����դˡ�big-endian�η׻����ˤ����Ƥϡ�
 *  ���Υ��дؿ��ϲ��⤷�ޤ���
 *
 * @param   offset �ҡ��׾�Υ��ɥ쥹(0-indexed���Х���ñ��)
 * @param   type ���ǤΥǡ�����
 * @param   length ���ǤθĿ�
 * @return  ���Ȥλ���
 */
fits_table &fits_table::reverse_heap_endian( long offset, 
					     int type, long length )
{
    long zt;

    if ( offset < 0 || length <= 0 ) goto quit;

    if ( type < 0 || (long)(sizeof(Type2zt)/sizeof(Type2zt[0])) <= type ) {
	err_report1(__FUNCTION__,"WARNING","invalid type: %d; ignored",type);
	goto quit;
    }
    zt = Type2zt[type];
    if ( zt == 0 ) {
	err_report1(__FUNCTION__,"WARNING","invalid type: %d; ignored",type);
	goto quit;
    }

    this->heap_rec.reverse_byte_order(false, zt, offset, zt2bytes(zt)*length);

 quit:
    return *this;
}


/**
 * @brief  �ҡ����ΰ�Υ��������ѹ�
 */
fits_table &fits_table::resize_heap( size_t sz )
{
    this->heap_rec.resize(sz);
    return *this;
}

/**
 * @brief  Data Unit���ͽ���ΰ�(Reserved Area)�ΥХ��ȥ��������֤�
 */
long long fits_table::reserved_area_length() const
{
    return this->reserve_length_rec;
}

/**
 * @brief  Data Unit���ͽ���ΰ�(Reserved Area)�ΥХ���Ĺ���ѹ�
 */
fits_table &fits_table::resize_reserved_area( long long val )
{
    if ( 0 <= val ) this->reserve_length_rec = val;
    else {
	err_report1(__FUNCTION__,"WARNING",
		    "Invalid reserved area length %lld; ignored",val);
    }
    return *this;
}

/**
 * @brief  heap�ѥ��������ˡ��������ѹ� (������������)
 *
 *  heap�ѥ������ݤ�����ˡ��ɤΤ褦����ˡ�ǹԤʤ�������ꤷ�ޤ���
 *  ���� 3 �Ĥ������򤷤ޤ���<br>
 *    "min", "auto"  ... �Ǿ��¤���ݡ��ꥵ��������ɬ��realloc()���ƤФ��<br>
 *    "pow"  ... 2��n��ǳ���<br>
 *    NULL��¾ ... ���ߤ���ˡ��ݻ�
 *
 * @param   strategy ���������ˡ������
 * @return  ���Ȥλ���
 *
 */
fits_table &fits_table::set_alloc_strategy_of_heap( const char *strategy )
{
    this->heap_rec.set_alloc_strategy(strategy);
    return *this;
}


/**
 * @brief  shallow copy ����Ĥ�����˻��� (̤����)
 * @note   ������֥������Ȥ� return ��ľ���ǻ��Ѥ��롥
 */
/* ���: ���Ȥ��֤������Ȥ��������return ʸ�Ǥ������ȥ��󥹥��� */
/*       ��2�󥳥ԡ������Ȥ����������ʻ��ˤʤ�Τ� void �ˤʤäƤ��� */
void fits_table::set_scopy_flag()
{
    this->fits_hdu::set_scopy_flag();
    this->shallow_copy_ok = true;
    return;
}

/*
 *  protected member functions
 */

/**
 * @brief  ���ȥ꡼�फ��ɬ�פ˱����� Header Unit ���ɤߡ�Data Unit ���ɤ�
 *
 * @note   ���Υ��дؿ��� protected �Ǥ���
 */
ssize_t fits_table::read_stream( fits_hdu *objp, cstreamio &sref )
{
    return this->bte_load(objp,sref,NULL,false,NULL);
}

/**
 * @brief  ���ȥ꡼�फ��ɬ�פ˱����� Header Unit ���ɤߡ�Data Unit ���ɤ�
 *
 * @note   ���Υ��дؿ��� protected �Ǥ���
 */
ssize_t fits_table::read_stream( fits_hdu *objp, cstreamio &sref,
				const char *section_to_read, bool is_last_hdu )
{
    return this->bte_load(objp,sref,section_to_read,is_last_hdu,NULL);
}

/**
 * @brief  ���ȥ꡼�फ��ɬ�פ˱����� Header Unit ���ɤߡ�Data Unit ���ɤ�
 *
 * @note   ���Υ��дؿ��� protected �Ǥ���
 */
ssize_t fits_table::read_stream( fits_hdu *objp, cstreamio &sref, 
				 const char *section_to_read, bool is_last_hdu,
				 size_t max_bytes_read )
{
    return this->bte_load(objp,sref,section_to_read,is_last_hdu,&max_bytes_read);
}

/**
 * @brief  ���ȥ꡼�फ�� Header Unit �� Data Unit ���ɤ�
 *
 * @note   ���Υ��дؿ��� protected �Ǥ���
 */
ssize_t fits_table::read_stream( cstreamio &sref, size_t max_bytes_read )
{
    return this->bte_load(NULL,sref,NULL,false,&max_bytes_read);
}

/**
 * @brief  ���ȥ꡼�फ�� Header Unit �� Data Unit ���ɤ�
 *
 * @note   ���Υ��дؿ��� protected �Ǥ���
 */
ssize_t fits_table::read_stream( cstreamio &sref )
{
    return this->bte_load(NULL,sref,NULL,false,NULL);
}

/**
 * @brief  �ǡ����ѡ��Ȥ���¸ or �����å�����η׻� (protected)
 *
 * @note  ���δؿ��Ǥϥ���ǥ������Ĵ�����ʤ��ΤǸƤӽФ����ˤ�äƤ���
 *        ���ȡ�<br>
 *        ���Υ��дؿ��ϡ�<br>
 *        fits_table::write_stream() <br>
 *        -> fits_hdu::write_stream() <br>
 *        -> fits_table::save_or_check_data() <br>
 *        �Ȥ�����ϩ�ǸƤФ�롥
 * @note   ���Υ��дؿ��� protected �Ǥ���
 */
ssize_t fits_table::save_or_check_data( cstreamio *sptr, void *c_sum_info )
{
    ssize_t return_value = -1;
    fitsio_csum *csum_info = (fitsio_csum *)c_sum_info;
    size_t bytes_per_row, write_buffer_size;
    mdarray write_buffer(1, true);
    long target_row_idx, i;
    ssize_t len_written;

    //err_report1(__FUNCTION__,"DEBUG","called!! adr=[%zx]",(size_t)sptr);

    /*
     * data contents
     */

    bytes_per_row = 0;		/* row ������ΥХ��ȿ� */
    for ( i=0 ; i < this->col_size_rec ; i++ ) {
	bytes_per_row += this->col(i).elem_byte_length();
    }

    if ( bytes_per_row == 0 ) {
	return_value = 0;
	goto quit;
    }
    else if ( DEFAULT_WRITE_BUFFER_SIZE < bytes_per_row ) {
	write_buffer_size = bytes_per_row;
    }
    else {
	long u = DEFAULT_WRITE_BUFFER_SIZE / bytes_per_row;
	write_buffer_size = bytes_per_row * u;
    }

    try {
	write_buffer.resize(write_buffer_size);
    }
    catch ( ... ) {
	err_throw(__FUNCTION__,"FATAL","write_buffer.resize() failed");
    }

    len_written = 0;
    target_row_idx = 0;
    while ( target_row_idx < this->row_size_rec ) {
	ssize_t len;
	long off = 0;
	long nrow = write_buffer_size / bytes_per_row;
	long i;
	if ( this->row_size_rec < target_row_idx + nrow ) {
	    nrow = this->row_size_rec - target_row_idx;
	}
	//fprintf(stderr,"debug: 2: nrow=%ld\n",nrow);
	for ( i=0 ; i < nrow ; i++ ) {
	    long j;
	    //fprintf(stderr,"debug: 3: i=%ld\n",i);
	    for ( j=0 ; j < this->col_size_rec ; j++ ) {
		fits_table_col *colp;
		colp = this->cols_ptr_rec[j];
		//fprintf(stderr,"debug: 4: j=%ld\n",j);
		if ( colp->data_ptr_rec != NULL ) {
		    colp->data_rec.getdata( write_buffer.data_ptr(off),
					    colp->elem_byte_length(),
					    0, target_row_idx);
		}
		else {
		    char z_ch;
		    if ( colp->type() == FITS::STRING_T ) z_ch = ' ';
		    else z_ch = 0;
		    write_buffer.put( (const void *)&z_ch, 
				      off, colp->elem_byte_length() );
		}
		off += colp->elem_byte_length();
	    }
	    target_row_idx ++;
	}
	try {
	    len = write_stream_or_get_csum( write_buffer.data_ptr(), 
				       bytes_per_row * nrow, sptr, csum_info );
	}
	catch (...) {
	    err_throw(__FUNCTION__,"FATAL",
		      "unexpected exception in write_stream_or_get_csum()");
	}
	if ( len < 0 || len != (ssize_t)(bytes_per_row * nrow) ) {
	    err_report(__FUNCTION__,"ERROR",
		       "write_stream_or_get_csum() failed");
	    goto quit;
	}
	len_written += len;
    }

    /* output reserved area */
    if ( 0 < this->reserve_length_rec ) {
	size_t len_to_write = this->reserve_length_rec;
	write_buffer_size = DEFAULT_WRITE_BUFFER_SIZE;
	if ( len_to_write < DEFAULT_WRITE_BUFFER_SIZE ) {
	    write_buffer_size = len_to_write;
	}
	try {
	    write_buffer.resize(write_buffer_size);
	}
	catch ( ... ) {
	    err_throw(__FUNCTION__,"FATAL","write_buffer.resize() failed");
	}
	write_buffer.clean();
	while ( 0 < len_to_write ) {
	    ssize_t len;
	    size_t sz = write_buffer_size;
	    if ( len_to_write < write_buffer_size ) {
		sz = len_to_write;
	    }
	    try {
		len = write_stream_or_get_csum( write_buffer.data_ptr(), 
						sz, sptr, csum_info );
	    }
	    catch (...) {
		err_throw(__FUNCTION__,"FATAL",
			 "unexpected exception in write_stream_or_get_csum()");
	    }
	    if ( len < 0 || len != (ssize_t)sz ) {
		err_report(__FUNCTION__,"ERROR",
			   "write_stream_or_get_csum() failed");
		goto quit;
	    }
	    len_to_write -= len;
	    len_written += len;
	}
    }

    /* output heap area */
    if ( 0 < this->heap_rec.length() ) {
	ssize_t len;
	size_t sz = this->heap_rec.length();
	try {
	    len = write_stream_or_get_csum( this->heap_rec.data_ptr(), 
					    sz, sptr, csum_info );
	}
	catch (...) {
	    err_throw(__FUNCTION__,"FATAL",
		      "unexpected exception in write_stream_or_get_csum()");
	}
	if ( len < 0 || len != (ssize_t)sz ) {
	    err_report(__FUNCTION__,"ERROR",
		       "write_stream_or_get_csum() failed");
	    goto quit;
	}
	len_written += len;
    }


    /*
     * padding
     */

    if ( len_written % FITS::FILE_RECORD_UNIT != 0 ) {
	ssize_t len;
	write_buffer_size = FITS::FILE_RECORD_UNIT -
	    ( len_written % FITS::FILE_RECORD_UNIT );
	try {
	    write_buffer.resize(write_buffer_size);
	}
	catch ( ... ) {
	    err_throw(__FUNCTION__,"FATAL","write_buffer.resize() failed");
	}
	try {
	    write_buffer.clean();
	    len = write_stream_or_get_csum( write_buffer.data_ptr(), 
					  write_buffer_size, sptr, csum_info );
	}
	catch (...) {
	    err_throw(__FUNCTION__,"FATAL",
		      "unexpected exception in write_stream_or_get_csum()");
	}
	if ( len < 0 || len != (ssize_t)write_buffer_size ) {
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
ssize_t fits_table::write_stream( cstreamio &sref )
{
    ssize_t return_value = -1;
    ssize_t written_total_bytes;

    //err_report(__FUNCTION__,"DEBUG","called!!");

    //this->ascii_to_binary();	/* fot test only */

    /* setup system header for this HDU */
    this->setup_sys_header();			/* fits_table class */

    /* adjust endian for data part */
    this->reverse_endian();

    /* Calculate chksum (if required), and write header+data to stream.     */
    /* Note that fits_hdu::write_stream() calls this->save_or_check_data(). */
    try {
	written_total_bytes = this->fits_hdu::write_stream(sref);
	if ( written_total_bytes < 0 ) {
	    err_report(__FUNCTION__,
		       "ERROR", "this->fits_hdu::write_stream() failed");
	    goto quit;
	}
    }
    catch (...) {
	/* restore endian for data part */
	this->reverse_endian();
	err_throw(__FUNCTION__,"FATAL","caught exception");
    }

    return_value = written_total_bytes;
 quit:
    /* restore endian for data part */
    this->reverse_endian();

    return return_value;
}

/**
 * @brief  Header Unit �� Data Unit �򥹥ȥ꡼��˽񤭹�������ΥХ���Ĺ�����
 *
 * @note   ���Υ��дؿ��� protected �Ǥ���
 */
ssize_t fits_table::stream_length()
{
    ssize_t return_value = -1;
    ssize_t header_total_bytes;
    size_t data_total_bytes;
    size_t bytes_per_row;
    long i;

    this->setup_sys_header();	/* fits_table class */

    header_total_bytes = fits_hdu::stream_length();
    if ( header_total_bytes < 0 ) {
	err_report(__FUNCTION__,"ERROR",
		   "this->fits_hdu::stream_length() failed");
	goto quit;
    }

    bytes_per_row = 0;		/* row ������ΥХ��ȿ� */
    for ( i=0 ; i < this->col_size_rec ; i++ ) {
	bytes_per_row += this->col(i).elem_byte_length();
    }

    data_total_bytes = bytes_per_row * this->row_size_rec;
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
 * @brief  ����μ����Τ���Υ��дؿ�
 *
 * @note   ���Υ��дؿ��� protected �Ǥ���
 */
fits_table &fits_table::set_protection( long col_index, bool protect )
{
    if ( col_index < 0 || this->col_size_rec <= col_index ) {
	return *this;	/* invalid */
    }
    this->cols_ptr_rec[col_index]->set_protection(protect);
    return *this;
}

/*
 * Search record point to insert TXFLDKWD keyword
 */
/* private */
/**
 * @brief  TXFLDKWD ������ɤ������ݥ���Ȥ򸡺�
 *
 * @note   ���Υ��дؿ��� private �Ǥ���
 */
long fits_table::find_txfldkwd_insert_point() const
{
    long ret;
    const fits_header &hdr_rec = this->header_rec;
    const char *kwds[] = {"XTENSION", "BITPIX", "NAXIS","NAXIS1","NAXIS2",
			  "PCOUNT","GCOUNT", "TFIELDS", "THEAP",
			  "EXTNAME", "EXTVER", "EXTLEVEL", NULL};
    bool found = false;
    size_t i;

    /* search with reg-exp */
    ret = hdr_rec.length();
    for ( i = 0 ; Field_keywords[i] != NULL ; i++ ) {
	long idx;
	tstring keypat;
	keypat.printf("%s[0-9]+",Field_keywords[i]);
	idx = hdr_rec.regmatch(keypat.cstr());
	if ( 0 <= idx && idx < ret ) {
	    ret = idx;
	    found = true;
	}
    }
    for ( i=0 ; i < this->user_fld_kwd_rec.length() ; i++ ) {
	long idx;
	tstring keypat;
	keypat.printf("%s[0-9]+",this->user_fld_kwd_rec.cstr(i));
	idx = hdr_rec.regmatch(keypat.cstr());
	if ( 0 <= idx && idx < ret ) {
	    ret = idx;
	    found = true;
	}
    }

    /* fallback */
    if ( found == false ) {
	size_t i;
	long idx;
	ret = 0;
	for ( i = 0 ; kwds[i] != NULL ; i++ ) {
	    idx = hdr_rec.index(kwds[i]);
	    if ( 0 <= idx && ret < idx + 1 ) ret = idx + 1;
	}
    }
    
    return ret;
}

/*
 * Search record point to insert THEAP keyword
 */
/* private */
/**
 * @brief  THEAP ������ɤ������ݥ���Ȥ򸡺�
 *
 * @note   ���Υ��дؿ��� private �Ǥ���
 */
long fits_table::find_theap_insert_point() const
{
    long ret;
    const fits_header &hdr_rec = this->header_rec;
    const char *kwds[] = {"XTENSION", "BITPIX", "NAXIS","NAXIS1","NAXIS2",
			  "PCOUNT","GCOUNT", "TFIELDS", "TXFLDKWD",
			  "EXTNAME", "EXTVER", "EXTLEVEL", NULL};
    bool found = false;
    size_t i;

    /* search with reg-exp */
    ret = hdr_rec.length();
    for ( i = 0 ; Field_keywords[i] != NULL ; i++ ) {
	long idx;
	tstring keypat;
	keypat.printf("%s[0-9]+",Field_keywords[i]);
	idx = hdr_rec.regmatch(keypat.cstr());
	if ( 0 <= idx && idx < ret ) {
	    ret = idx;
	    found = true;
	}
    }
    for ( i=0 ; i < this->user_fld_kwd_rec.length() ; i++ ) {
	long idx;
	tstring keypat;
	keypat.printf("%s[0-9]+",this->user_fld_kwd_rec.cstr(i));
	idx = hdr_rec.regmatch(keypat.cstr());
	if ( 0 <= idx && idx < ret ) {
	    ret = idx;
	    found = true;
	}
    }

    /* fallback */
    if ( found == false ) {
	size_t i;
	long idx;
	ret = 0;
	for ( i = 0 ; kwds[i] != NULL ; i++ ) {
	    idx = hdr_rec.index(kwds[i]);
	    if ( 0 <= idx && ret < idx + 1 ) ret = idx + 1;
	}
    }
    
    return ret;
}

/* 
 * Search record point to insert TTYPEn records, etc.
 * (NOTE: set column_idx = 0 to insert TTYPE1)
 */
/* private */
/**
 * @brief  TTYPEn ������ɤ������ݥ���Ȥ򸡺�
 *
 *  TTYPE1 ������������ˤϡ�column_idx �� 0 �򥻥åȤ��ޤ���
 *
 * @note   ���Υ��дؿ��� private �Ǥ���
 */
long fits_table::find_colhdr_insert_point( long column_idx ) const
{
    long ret = 0;
    const fits_header &hdr_rec = this->header_rec;
    const char *kwds[] = {"XTENSION", "BITPIX", "NAXIS","NAXIS1","NAXIS2",
			  "PCOUNT","GCOUNT", "TFIELDS", "THEAP", "TXFLDKWD", 
			  "EXTNAME", "EXTVER", "EXTLEVEL", NULL};
    bool found = false;

    if ( 0 < column_idx ) {
	size_t i;
	long j, idx;
	for ( j=0 ; j < column_idx ; j++ ) {
	    /* ���Υ��������ǡ��Ǹ����Τ��õ�� */
	    ret = 0;
	    for ( i = 0 ; Field_keywords[i] != NULL ; i++ ) {
		tstring kw;
		kw.printf("%s%ld",Field_keywords[i],(column_idx + 1)-1-j);
		idx = hdr_rec.index(kw.cstr());
		if ( 0 <= idx && ret < idx + 1 ) {
		    ret = idx + 1;
		    found = true;
		}
	    }
	    for ( i=0 ; i < this->user_fld_kwd_rec.length() ; i++ ) {
		tstring kw;
		kw.printf("%s%ld",
			  this->user_fld_kwd_rec.cstr(i),(column_idx + 1)-1-j);
		idx = hdr_rec.index(kw.cstr());
		if ( 0 <= idx && ret < idx + 1 ) {
		    ret = idx + 1;
		    found = true;
		}
	    }
	    if ( found == true ) break;
	}
    }
    /* ��Υ��������ǡ�������Τ�Τ�õ�� */
    if ( found == false ) {
	size_t i;
	long j, idx;
	for ( j=column_idx+1 ; j < this->col_length() ; j++ ) {
	    ret = hdr_rec.length();
	    for ( i = 0 ; Field_keywords[i] != NULL ; i++ ) {
		tstring kw;
		kw.printf("%s%ld",Field_keywords[i], j + 1);
		idx = hdr_rec.index(kw.cstr());
		if ( 0 <= idx && idx < ret ) {
		    ret = idx;
		    found = true;
		}
	    }
	    for ( i=0 ; i < this->user_fld_kwd_rec.length() ; i++ ) {
		tstring kw;
		kw.printf("%s%ld",
			  this->user_fld_kwd_rec.cstr(i), j + 1);
		idx = hdr_rec.index(kw.cstr());
		if ( 0 <= idx && idx < ret ) {
		    ret = idx;
		    found = true;
		}
	    }
	    if ( found == true ) break;
	}
    }

    /* fallback */
    if ( found == false ) {
	size_t i;
	long idx;
	ret = 0;
	for ( i = 0 ; kwds[i] != NULL ; i++ ) {
	    idx = hdr_rec.index(kwds[i]);
	    if ( 0 <= idx && ret < idx + 1 ) ret = idx + 1;
	}
    }
    
    return ret;
}

/*
 * �إå������ѤΥޥ���
 *
 * ������ʸ�ˤĤ��Ƥϡ�txxx_comment ��������Ϥ��줬��ͥ��Ǻ��Ѥ��졤
 * comm �� NULL �Ǥ�ʤ� "" �Ǥ�ʤ���硤���줬���˺��Ѥ��졤
 * fallback �Ȥ��ƥǥե���ȤΥ����Ȥ����Ѥ���롥
 */
#define HEADER_W(TXXX,hrec_type,def_txxx,comm) \
	keyword.assignf(TXXX "%ld",i+1); \
	idx = hdr_rec.index(keyword.cstr()); \
	if ( def_txxx[0] == '\0' ) { \
	    /* ���ꥸ�ʥ��Ǥ���¤�Ĥ� */ \
	    if ( 0 <= idx ) { \
		if ( hrec_type == FITS::STRING_T ) { \
		    if ( 0 < hdr_rec.at(idx).svalue_length() ) \
			hdr_rec.erase_records(idx,1); \
		} \
		else { \
		    if ( 0 < hdr_rec.at(idx).value_length() ) \
			hdr_rec.erase_records(idx,1); \
		} \
	    } \
	} \
	else { \
	    fits::header_def rec = {keyword.cstr(), def_txxx, comm}; \
	    if ( 0 <= idx ) { \
		const tstring &svr = hdr_rec.at(idx).svalue_cs(); \
		int typ = hdr_rec.at(idx).type(); \
		/* DOUBLE����ξ�硤�����ξ��� OK �Ȥ��� */ \
		if ( hrec_type == FITS::DOUBLE_T && typ == FITS::LONGLONG_T ) \
		    typ = FITS::DOUBLE_T; \
		/* �����ͤ�ξ����Ʊ�������ѹ����ʤ� */ \
		if ( hrec_type != typ || svr.strcmp(def_txxx) != 0 ) { \
		    hdr_rec.assign(idx,rec); \
		    if ( rec.comment == NULL || rec.comment[0] == '\0' ) \
		        hdr_rec.at(idx).assign_default_comment(); \
		    /* TDIM ��ʣ�ǿ����ȸ�ʬ���Ĥ��ʤ��ΤǤ��ν�����ɬ�� */ \
		    if ( hrec_type == FITS::STRING_T && hdr_rec.at(idx).type() != hrec_type ) \
		        hdr_rec.at(idx).assign(def_txxx); \
		} \
		idx0 = idx + 1; \
	    } \
	    else { \
		hdr_rec.insert(idx0,rec); \
		if ( rec.comment == NULL || rec.comment[0] == '\0' ) \
		    hdr_rec.at(idx0).assign_default_comment(); \
		/* TDIM ��ʣ�ǿ����ȸ�ʬ���Ĥ��ʤ��ΤǤ��ν�����ɬ�� */ \
		if ( hrec_type == FITS::STRING_T && hdr_rec.at(idx0).type() != hrec_type ) \
		    hdr_rec.at(idx0).assign(def_txxx); \
		idx0++; \
	    } \
	} \
	if ( max_idx0 < idx0 ) max_idx0 = idx0;

#define HEADER_W_COMM(TXXX,hrec_type,txxx,txxx_comment,comm) \
	keyword.assignf(TXXX "%ld",i+1); \
	idx = hdr_rec.index(keyword.cstr()); \
	if ( txxx[0] == '\0' ) { \
	    /* ���ꥸ�ʥ��Ǥ���¤�Ĥ� */ \
	    if ( 0 <= idx ) { \
		if ( hrec_type == FITS::STRING_T ) { \
		    if ( 0 < hdr_rec.at(idx).svalue_length() ) \
			hdr_rec.erase_records(idx,1); \
		} \
		else { \
		    if ( 0 < hdr_rec.at(idx).value_length() ) \
			hdr_rec.erase_records(idx,1); \
		} \
	    } \
	} \
	else { \
	    fits::header_def rec = {keyword.cstr(), txxx, comm}; \
	    if ( def.txxx_comment[0] != '\0' )  \
		rec.comment = def.txxx_comment; \
	    if ( 0 <= idx ) { \
		const tstring &svr = hdr_rec.at(idx).svalue_cs(); \
		const tstring &cmr = hdr_rec.at(idx).comment_cs(); \
		int typ = hdr_rec.at(idx).type(); \
		/* DOUBLE����ξ�硤�����ξ��� OK �Ȥ��� */ \
		if ( hrec_type == FITS::DOUBLE_T && typ == FITS::LONGLONG_T ) \
		    typ = FITS::DOUBLE_T; \
		/* �����͡������Ȥ�Ʊ�������ѹ����ʤ� */ \
		if ( hrec_type != typ || svr.strcmp(txxx) != 0 || \
		     cmr.strcmp(rec.comment) != 0 ) { \
		    hdr_rec.assign(idx,rec); \
		    if ( rec.comment == NULL || rec.comment[0] == '\0' ) \
		        hdr_rec.at(idx).assign_default_comment(); \
		    /* TDIM ��ʣ�ǿ����ȸ�ʬ���Ĥ��ʤ��ΤǤ��ν�����ɬ�� */ \
		    if ( hrec_type == FITS::STRING_T && hdr_rec.at(idx).type() != hrec_type ) \
		        hdr_rec.at(idx).assign(txxx); \
		} \
		idx0 = idx + 1; \
	    } \
	    else { \
		hdr_rec.insert(idx0,rec); \
		if ( rec.comment == NULL || rec.comment[0] == '\0' ) \
		    hdr_rec.at(idx0).assign_default_comment(); \
		/* TDIM ��ʣ�ǿ����ȸ�ʬ���Ĥ��ʤ��ΤǤ��ν�����ɬ�� */ \
		if ( hrec_type == FITS::STRING_T && hdr_rec.at(idx0).type() != hrec_type ) \
		    hdr_rec.at(idx0).assign(txxx); \
		idx0++; \
	    } \
	} \
	if ( max_idx0 < idx0 ) max_idx0 = idx0;

#if 0

#define HEADER_W_CSV_COMM(TXXX,txxx,txxx_comment,comm) \
	keyword.assignf(TXXX "%ld",i+1); \
	idx = hdr_rec.index(keyword.cstr()); \
	if ( def.txxx == NULL || def.txxx[0] == NULL ) { \
	    /* ���ꥸ�ʥ��Ǥ���¤�Ĥ� */ \
	    if ( 0 <= idx && 0 < hdr_rec.at(idx).svalue_length() ) { \
		hdr_rec.erase_records(idx,1); \
	    } \
	} \
	else { \
	    fits::header_def rec = {keyword.cstr(), "", comm}; \
	    tstring val; \
	    long j; \
	    if ( def.txxx_comment[0] != '\0' )  \
		rec.comment = def.txxx_comment; \
	    for ( j=0 ; def.txxx[j] != NULL ; j++ ) { \
		if ( j==0 ) { \
		    val.assign(def.txxx[j]); \
		} \
		else { \
		    val.appendf(",%s",def.txxx[j]); \
		} \
	    } \
	    rec.value = val.cstr(); \
	    if ( 0 <= idx ) { \
		const tstring &cmr = hdr_rec.at(idx).comment_cs(); \
		/* �ͤȥ����Ȥ�Ʊ�������ѹ����ʤ� */ \
		if ( val.strcmp(hdr_rec.at(idx).svalue()) != 0 || \
		     cmr.strcmp(rec.comment) != 0 ) { \
		    hdr_rec.assign(idx,rec); \
		    if ( rec.comment == NULL || rec.comment[0] == '\0' ) \
		        hdr_rec.at(idx).assign_default_comment(); \
		} \
		idx0 = idx + 1; \
	    } \
	    else { \
		hdr_rec.insert(idx0,rec); \
		if ( rec.comment == NULL || rec.comment[0] == '\0' ) \
		    hdr_rec.at(idx0).assign_default_comment(); \
		idx0++; \
	    } \
	} \
	if ( max_idx0 < idx0 ) max_idx0 = idx0;

#endif	/* 0 */

#define HEADER_ERASE(TXXX) \
	keyword.assignf("%s%ld",TXXX,i+1); \
	idx = hdr_rec.index(keyword.cstr()); \
	if ( 0 <= idx ) { \
	    found = true; \
	    hdr_rec.erase_records(idx,1); \
	} \


/* 
 * Set or erase header records for a column
 */
/* private */
/**
 * @brief  1�ĤΥ����ˤĤ��ƤΡ�������ѥ�����ɤΥ��åȡ��õ�
 *
 * @note   ���Υ��дؿ��� private �Ǥ���
 */
fits_table &fits_table::update_hdr_of_a_col( long col_index, 
					  long *ret_idx0, long *ret_tbcol_idx )
{
    long max_idx0, idx0, tbcol_idx, i, idx;
    size_t ii;
    tstring keyword, tform_comm_str;
    fits_header &hdr_rec = this->header_rec;
    const fits::table_def_all &def = this->col(col_index).definition_all();

    if ( ret_idx0 != NULL ) {
	idx0 = *ret_idx0;
    }
    else {
	idx0 = find_colhdr_insert_point(col_index);
    }

    if ( ret_tbcol_idx != NULL ) {
	tbcol_idx = *ret_tbcol_idx;
    }
    else {
	tbcol_idx = 1;
	for ( i=0 ; i < col_index ; i++ ) {
	    tbcol_idx += this->col(i).elem_byte_length();
	}
    }

    /* for macro: i �ϥޥ��� HEADER_*() �ǻȤ��Τ���� */
    i = col_index;

    max_idx0 = idx0;

    this->header_set_sysrecords_prohibition(false);
    try {
	/* TTYPE */
	HEADER_W_COMM("TTYPE",FITS::STRING_T,def.ttype,ttype_comment,"");

	/* TALAS */
	//HEADER_W_CSV_COMM("TALAS",talas,talas_comment,"");
	HEADER_W_COMM("TALAS",FITS::STRING_T,def.talas,talas_comment,"");

	/* TELEM */
	//HEADER_W_CSV_COMM("TELEM",telem,telem_comment,"");
	HEADER_W_COMM("TELEM",FITS::STRING_T,def.telem,telem_comment,"");

	/* TUNIT */
	HEADER_W_COMM("TUNIT",FITS::STRING_T,def.tunit,tunit_comment,"");

	/* TDISP */
	if ( this->hdutype() == FITS::ASCII_TABLE_HDU ) {
	    tstring tmp_val;
	    if ( this->col(i).type() != FITS::STRING_T ) {
		err_throw(__FUNCTION__,"FATAL","invalid data type");
	    }
	    tmp_val.assign(def.tdisp).strtrim();
	    if ( tmp_val.length() == 0 ) {
		tmp_val.assignf("A%ld",this->col(i).elem_byte_length());
	    }
	    HEADER_W("TFORM",FITS::STRING_T,tmp_val.cstr(),"");
	}
	else {
	    /*
	     * ASCII�ơ��֥뤫��Х��ʥ�ơ��֥���Ѵ��������ˡ�
	     * ASCII�ơ��֥�� TFORMn ���ͤ򥳥��Ȥ���¸�������
	     */
	    if ( this->col(i).type() == FITS::STRING_T && 
		 def.tdisp != NULL &&
		 def.tdisp[0] != '\0' && def.tdisp[0] != 'A' ) {
		bool found;
		tform_comm_str.printf("data format : STRING of '%s'",
				      def.tdisp);
		HEADER_ERASE("TDISP");
	    }
	    else {
		HEADER_W_COMM("TDISP",FITS::STRING_T,def.tdisp,tdisp_comment,"");
	    }
	}

	/* TFORM */
	if ( this->hdutype() == FITS::ASCII_TABLE_HDU ) {
	    tstring tmp_val;
	    tmp_val.printf("%ld",tbcol_idx);
	    HEADER_W("TBCOL",FITS::LONGLONG_T,tmp_val.cstr(),"");
	}
	else {
	    if ( this->col(i).type() == FITS::STRING_T &&
		 0 < tform_comm_str.length() ) {
		const char *comm = tform_comm_str.cstr();
		HEADER_W_COMM("TFORM",FITS::STRING_T,def.tform,tform_comment,comm);
	    }
	    else {
		//err_report1(__FUNCTION__,"DEBUG","[%s]",def.tform_comment);
		HEADER_W_COMM("TFORM",FITS::STRING_T,def.tform,tform_comment,"");
	    }
	}

	/* TDIM */
	HEADER_W_COMM("TDIM",FITS::STRING_T,def.tdim,tdim_comment,"");

	/* TZERO */
	HEADER_W_COMM("TZERO",FITS::DOUBLE_T,def.tzero,tzero_comment,"");

	/* TSCAL */
	HEADER_W_COMM("TSCAL",FITS::DOUBLE_T,def.tscal,tscal_comment,"");

	/* TNULL */
	if ( this->hdutype() == FITS::ASCII_TABLE_HDU ||
	     this->col(i).type() == FITS::STRING_T ) {
	    tstring tmp_val;
	    //err_report1(__FUNCTION__,"DEBUG","tnull = [%s]",def.tnull);
	    /* " " �� NULL �ˤʤꤦ��Τǡ������Ǥ����̤ʽ����򤷤Ƥ��� */
	    tmp_val.assignf("'%s'",def.tnull);
	    if ( 2 < tmp_val.length() ) {
		keyword.assignf("TNULL" "%ld",i+1);
		idx = hdr_rec.index(keyword.cstr());
		if ( 0 <= idx && 2 < hdr_rec.record(idx).value_length() ) {
		    /* �����äƤ����Τ���Ӥ��� */
		    tstring tmp_val1;
		    tmp_val1.assign(hdr_rec.record(idx).value()).trim("'");
		    //.resize(this->col(i).elem_byte_length());
		    tmp_val1.rtrim(" ");	/* ��¦�ζ���Τ߽��� */
		    if ( (long)(tmp_val1.length()) <= this->col(i).bytes() ) {
			tmp_val1.resize(this->col(i).bytes());
		    }
		    /* �ۤʤäƤ������ˤ���Ͽ */
		    if ( tmp_val1.strcmp(def.tnull) != 0 ) {
			HEADER_W_COMM("TNULL",FITS::STRING_T,tmp_val.cstr(),tnull_comment,"");
		    }
		}
		else {
		    HEADER_W_COMM("TNULL",FITS::STRING_T,tmp_val.cstr(),tnull_comment,"");
		}
	    }
	    else {
		HEADER_W_COMM("TNULL",FITS::STRING_T,def.tnull,tnull_comment,"");
	    }
	}
	else {	/* for binary table (not string type) */
	    tstring tmp_val;
	    size_t epos;
	    tmp_val.assign(def.tnull).strtoll(10,&epos);
	    if ( epos != tmp_val.length() ) {
		tstring tmp_comm;
	    /* not integer: ������ TNULL�� �ξ�� */
	    //if ( this->col(i).elem_byte_length() < (long)tmp_val.length() ) {
	    //    tmp_val.resize(this->col(i).elem_byte_length());
	    //}
		err_report1(__FUNCTION__,"WARNING",
			    "Invalid TNULL value : [%s].",tmp_val.cstr());
		/* ��������ʬ����¸����褦�ˤ��� */
		tmp_comm.printf("null value : '%s'", tmp_val.cstr());
		keyword.assignf("TNULL" "%ld",i+1);
		idx = hdr_rec.index(keyword.cstr());
		/* */
		if ( 0 < tmp_val.length() ) {
		    fits::header_def rec = 
			{keyword.cstr(), "", tmp_comm.cstr()};
		    if ( 0 <= idx ) {
			hdr_rec.assign(idx,rec);
			idx0 = idx + 1;
		    }
		    else {
			hdr_rec.insert(idx0,rec);
			idx0++;
		    }
		}
		else {	/* ��������ʤ��Ϥ������� */
		    if ( 0 <= idx ) {
			hdr_rec.erase_records(idx,1);
		    }
		}
		if ( max_idx0 < idx0 ) max_idx0 = idx0;
	    }
	    else {
		HEADER_W_COMM("TNULL",FITS::LONGLONG_T,def.tnull,
			  tnull_comment,"integer null value");
	    }
	}

	/* erase TBCOL on ATE */
	if ( this->hdutype() != FITS::ASCII_TABLE_HDU ) {
	    bool found;
	    HEADER_ERASE("TBCOL");
	}

    }
    catch (...) {
	this->header_set_sysrecords_prohibition(true);
	err_throw(__FUNCTION__,"FATAL","caught exception");
    }
    this->header_set_sysrecords_prohibition(true);

    /* check user's field keywords */
    for ( ii=0 ; ii < this->user_fld_kwd_rec.length() ; ii++ ) {
	tstring keynm;
	keynm.printf("%s%ld", this->user_fld_kwd_rec.cstr(ii), i + 1);
	if ( 0 <= (idx=hdr_rec.index(keynm.cstr())) ) {
	    idx0 = idx + 1;
	    if ( max_idx0 < idx0 ) max_idx0 = idx0;
	}
    }

    /* update values to return */
    idx0 = max_idx0;

    tbcol_idx += this->col(i).elem_byte_length();

    /* return results */
    if ( ret_idx0 != NULL ) {
	*ret_idx0 = idx0;
    }
    if ( ret_tbcol_idx != NULL ) {
	*ret_tbcol_idx = tbcol_idx;
    }

    return *this;
}

/* 
 * Update all TBCOLn header records
 */
/* private */
/**
 * @brief  TBCOLn ������ɤ򹹿�
 *
 * @note   ���Υ��дؿ��� private �Ǥ���
 */
fits_table &fits_table::update_tbcol_hdr()
{
    fits_header &hdr_rec = this->header_rec;
    long i = 0;

    this->header_set_sysrecords_prohibition(false);
    try {
	/* for ASCII table HDU */
	if ( this->hdutype() == FITS::ASCII_TABLE_HDU ) {
	    const long col_len = this->col_length();
	    tstring kwd;
	    long tbcol_idx;
	    tbcol_idx = 1;
	    for ( i=0 ; i < col_len ; i++ ) {
		long idx;
		kwd.printf("TBCOL%ld", i + 1);
		if ( (idx = hdr_rec.index(kwd.cstr())) < 0 ) {
		    idx = this->find_colhdr_insert_point(i);
		    hdr_rec.insert(idx, kwd.cstr());
		}
		if ( hdr_rec.at(idx).type() != FITS::LONGLONG_T ||
		     hdr_rec.at(idx).lvalue() != tbcol_idx ) {
		    hdr_rec.at(idx).assign(tbcol_idx);
		}
		tbcol_idx += this->col(i).elem_byte_length();
	    }
	}
	/* for ASCII and binary table HDU */
	while ( 1 ) {
	    long idx;
	    tstring kwd;
	    kwd.printf("TBCOL%ld", i + 1);
	    if ( (idx = hdr_rec.index(kwd.cstr())) < 0 ) break;
	    hdr_rec.erase(idx);
	    i ++;
	}
    }
    catch (...) {
	this->header_set_sysrecords_prohibition(true);
	err_throw(__FUNCTION__,"FATAL","caught exception");
    }
    this->header_set_sysrecords_prohibition(true);

    return *this;
}

/* 
 * Erase header records for a column
 */
/* private */
/**
 * @brief  1�ĤΥ����Υ�����ѥ�����ɤ��٤Ƥ�õ�
 *
 * @note   ���Υ��дؿ��� private �Ǥ���
 */
fits_table &fits_table::erase_hdr_of_a_col( long col_idx )
{
    fits_header &hdr_rec = this->header_rec;
    tstring kwd;
    size_t i;

    this->header_set_sysrecords_prohibition(false);
    try {
	/* erase system column keywords */
	for ( i=0 ; Field_keywords[i] != NULL ; i++ ) {
	    long idx;
	    kwd.printf("%s%ld", Field_keywords[i], col_idx + 1);
	    if ( 0 <= (idx = hdr_rec.index(kwd.cstr())) ) hdr_rec.erase(idx);
	}

	/* erase user column keywords */
	for ( i=0 ; i < this->user_fld_kwd_rec.length() ; i++ ) {
	    long idx;
	    kwd.printf("%s%ld", this->user_fld_kwd_rec[i].cstr(), col_idx + 1);
	    if ( 0 <= (idx = hdr_rec.index(kwd.cstr())) ) hdr_rec.erase(idx);
	}
    }
    catch (...) {
	this->header_set_sysrecords_prohibition(true);
	err_throw(__FUNCTION__,"FATAL","caught exception");
    }
    this->header_set_sysrecords_prohibition(true);

    return *this;
}

/* 
 * Update header records for a column
 */
/* private */
/**
 * @brief  ������ѥ�����ɤ��ֹ�򹹿�
 *
 * @note   ���Υ��дؿ��� private �Ǥ���
 */
fits_table &fits_table::renumber_hdr_of_a_col( long old_col_idx, 
					       long new_col_idx )
{
    fits_header &hdr_rec = this->header_rec;
    tstring old_kwd, new_kwd;
    size_t i;

    //err_report1(__FUNCTION__,"DEBUG","renumber : %ld", old_col_idx);
    //err_report1(__FUNCTION__,"DEBUG","        -> %ld", new_col_idx);

    this->header_set_sysrecords_prohibition(false);
    try {
	long dst_idx = this->find_colhdr_insert_point(new_col_idx);
	//err_report1(__FUNCTION__,"DEBUG","dst_idx = %ld", dst_idx);
	/* update system column keywords */
	for ( i=0 ; Field_keywords[i] != NULL ; i++ ) {
	    long idx;
	    old_kwd.printf("%s%ld", Field_keywords[i], old_col_idx + 1);
	    if ( 0 <= (idx = hdr_rec.index(old_kwd.cstr())) ) {
		new_kwd.printf("%s%ld", Field_keywords[i], new_col_idx + 1);
		hdr_rec.insert(dst_idx, new_kwd.cstr(), 
			       hdr_rec.record(idx).value(), 
			       hdr_rec.record(idx).comment());
		if ( dst_idx <= idx ) dst_idx ++;	/* difficult logic! */
		hdr_rec.erase(old_kwd.cstr());
	    }
	}

	/* update user column keywords */
	for ( i=0 ; i < this->user_fld_kwd_rec.length() ; i++ ) {
	    long idx;
	    old_kwd.printf("%s%ld", 
			   this->user_fld_kwd_rec[i].cstr(), old_col_idx + 1);
	    if ( 0 <= (idx = hdr_rec.index(old_kwd.cstr())) ) {
		new_kwd.printf("%s%ld", 
			    this->user_fld_kwd_rec[i].cstr(), new_col_idx + 1);
		hdr_rec.insert(dst_idx, new_kwd.cstr(),
			       hdr_rec.record(idx).value(), 
			       hdr_rec.record(idx).comment());
		if ( dst_idx <= idx ) dst_idx ++;	/* difficult logic! */
		hdr_rec.erase(old_kwd.cstr());
	    }
	}
    }
    catch (...) {
	this->header_set_sysrecords_prohibition(true);
	err_throw(__FUNCTION__,"FATAL","caught exception");
    }
    this->header_set_sysrecords_prohibition(true);

    return *this;
}


/*
 * Set or erase TXFLDKWD record
 */
/* private */
/**
 * @brief  TXFLDKWD ������ɤΥ��åȡ��õ�
 *
 * @note   ���Υ��дؿ��� private �Ǥ���
 */
fits_table &fits_table::setup_txfldkwd_hdr()
{
    fits_header &hdr_rec = this->header_rec;

    this->header_set_sysrecords_prohibition(false);
    try {
	long i, idx;
	size_t j;
	tstring value;

	/* prepare value of TXFLDKWD */
	for ( i=0 ; i < this->col_length() ; i++ ) {
	    const fits::table_def_all &def = this->col(i).definition_all();
	    if ( def.talas != NULL && def.talas[0] != '\0' ) {
		value.append("TALAS,");
		break;
	    }
	}
	for ( i=0 ; i < this->col_length() ; i++ ) {
	    const fits::table_def_all &def = this->col(i).definition_all();
	    if ( def.telem != NULL && def.telem[0] != '\0' ) {
		value.append("TELEM,");
		break;
	    }
	}
	for ( j=0 ; j < this->user_fld_kwd_rec.length() ; j++ ) {
	    value.appendf("%s,",this->user_fld_kwd_rec.cstr(j));
	}

	/* erase last "," */
	if ( 0 < value.length() ) value.resizeby(-1);

	/* set/erase TXFLDKWD record */
	if ( 0 < value.length() ) {
	    if ( (idx=hdr_rec.index("TXFLDKWD")) < 0 ) {
		idx = this->find_txfldkwd_insert_point();
		hdr_rec.insert(idx, "TXFLDKWD", "", "");
		hdr_rec.at(idx).assign_default_comment(FITS::BINARY_TABLE_HDU);
	    }
	    if ( value.strcmp(hdr_rec.at(idx).svalue()) != 0 ) {
		hdr_rec.at(idx).assign(value.cstr());
	    }
	}
	else {
	    hdr_rec.erase("TXFLDKWD");
	}
    }
    catch (...) {
	this->header_set_sysrecords_prohibition(true);
	err_throw(__FUNCTION__,"FATAL","caught exception");
    }
    this->header_set_sysrecords_prohibition(true);

    return *this;
}

/*
 * update NAXIS1 and TFIELDS
 */
/* private */
/**
 * @brief  NAXIS1 �� TFIELDS �ι���
 *
 * @note   ���Υ��дؿ��� private �Ǥ���
 */
fits_table &fits_table::update_hdr_of_col_props()
{
    fits_header &hdr_rec = this->header_rec;
    long i;

    /* create top records of sys header, if required */
    if ( hdr_rec.index("NAXIS1") < 0 || hdr_rec.index("TFIELDS") < 0 ) {
	this->fits_hdu::setup_sys_header();
    }

    /* update NAXIS1 and TFIELDS */
    this->header_set_sysrecords_prohibition(false);
    try {
	long long naxis1 = 0;
	for ( i=0 ; i < this->col_size_rec ; i++ ) {
	    naxis1 += this->col(i).elem_byte_length();
	}
	if ( hdr_rec.at("NAXIS1").type() != FITS::LONGLONG_T ||
	     hdr_rec.at("NAXIS1").llvalue() != naxis1 ) {
	    hdr_rec.at("NAXIS1").assign(naxis1);
	}
	if ( hdr_rec.at("TFIELDS").type() != FITS::LONGLONG_T ||
	     hdr_rec.at("TFIELDS").lvalue() != this->col_size_rec ) {
	    hdr_rec.at("TFIELDS").assign(this->col_size_rec);
	}
    }
    catch (...) {
	this->header_set_sysrecords_prohibition(true);
	err_throw(__FUNCTION__,"FATAL","caught exception");
    }
    this->header_set_sysrecords_prohibition(true);

    return *this;
}

/*
 * update NAXIS2
 */
/* private */
/**
 * @brief  NAXIS2 �ι���
 *
 * @note   ���Υ��дؿ��� private �Ǥ���
 */
fits_table &fits_table::update_hdr_of_row_props()
{
    fits_header &hdr_rec = this->header_rec;

    /* create top records of sys header, if required */
    if ( hdr_rec.index("NAXIS2") < 0 ) {
	this->fits_hdu::setup_sys_header();
    }

    /* update NAXIS2 */
    this->header_set_sysrecords_prohibition(false);
    try {
	if ( hdr_rec.at("NAXIS2").type() != FITS::LONGLONG_T ||
	     hdr_rec.at("NAXIS2").lvalue() != this->row_size_rec ) {
	    hdr_rec.at("NAXIS2").assign(this->row_size_rec);
	}
    }
    catch (...) {
	this->header_set_sysrecords_prohibition(true);
	err_throw(__FUNCTION__,"FATAL","caught exception");
    }
    this->header_set_sysrecords_prohibition(true);

    return *this;
}

/*
 * Set up all header records for table contents
 */
/* protected */
/**
 * @brief  �ơ��֥����Ȥ˴�Ϣ�������إå��쥳���ɤΥ��åȥ��å�
 *
 * @note   ���Υ��дؿ��� protected �Ǥ���
 */
fits_table &fits_table::setup_sys_header()
{
    long i, idx0, tbcol_idx;
    bool found;
    fits_header &hdr_rec = this->header_rec;

    /* obtain max length of variable length array and update TFORM */
    for ( i=0 ; i < this->col_size_rec ; i++ ) {
	fits_table_col &col0 = this->col(i);
	long j, sz = this->row_length() * col0.elem_length();
	const fits::table_def &def = col0.definition();
	fits::table_def_all newdef = {NULL};
	tstring tfm;
	ssize_t ix;
	if ( col0.type() == FITS::LONGARRDESC_T ) {
	    fits::longarrdesc_t *d_ptr = 
		(fits::longarrdesc_t *)(col0.data_ptr());
	    uint32_t max_len = 0;
	    for ( j=0 ; j < sz ; j++ ) {
		if ( max_len < d_ptr[j].length ) max_len = d_ptr[j].length;
	    }
	    tfm.assign(def.tform).trim();
	    ix = tfm.strchr('(');
	    if ( 0 <= ix ) tfm.resize(ix);
	    tfm.appendf("(%ld)", (long)max_len);
	    newdef.tform = tfm.cstr();
	    col0._define(newdef);
	}
	else if ( col0.type() == FITS::LLONGARRDESC_T ) {
	    fits::llongarrdesc_t *d_ptr = 
		(fits::llongarrdesc_t *)(col0.data_ptr());
	    uint64_t max_len = 0;
	    for ( j=0 ; j < sz ; j++ ) {
		if ( max_len < d_ptr[j].length ) max_len = d_ptr[j].length;
	    }
	    tfm.assign(def.tform).trim();
	    ix = tfm.strchr('(');
	    if ( 0 <= ix ) tfm.resize(ix);
	    tfm.appendf("(%lld)", (long long)max_len);
	    newdef.tform = tfm.cstr();
	    col0._define(newdef);
	}
    }

    /* ����Ū�ʥإå������(fits_hdu���饹�Υ��дؿ�) */
    this->fits_hdu::setup_sys_header();

    /* set/erase TXFLDKWD, if required */
    this->setup_txfldkwd_hdr();

    /* update NAXIS1 and TFIELDS */
    this->update_hdr_of_col_props();
    /* update NAXIS2 */
    this->update_hdr_of_row_props();

    /* find position to insert new header recs */
    idx0 = this->find_colhdr_insert_point(0L);

    /* update column header */
    tbcol_idx = 1;
    for ( i=0 ; i < this->col_size_rec ; i++ ) {
	this->update_hdr_of_a_col( i, &idx0, &tbcol_idx );
    }

    /* clean up, update THEAP and PCOUNT */
    this->header_set_sysrecords_prohibition(false);
    try {
	long long naxis1 = 0;
	for ( i=0 ; i < this->col_size_rec ; i++ ) {
	    naxis1 += this->col(i).elem_byte_length();
	}

	/* �ĳ������뤫�⤷��ʤ��ΤǾä��Ƥ��� */
	do {
	    long idx;
	    tstring keyword;
	    size_t j;
	    found = false;

	    for ( j=0 ; Field_keywords[j] != NULL ; j++ ) {
		HEADER_ERASE(Field_keywords[j]);
	    }

	    for ( j=0 ; j < this->user_fld_kwd_rec.length() ; j++ ) {
		HEADER_ERASE(this->user_fld_kwd_rec.cstr(j));
	    }

	    i++;
	} while ( found != false );

	/* setup THEAP record */
	if ( 0 < this->reserve_length_rec ) {
	    long idx = hdr_rec.index("THEAP");
	    long long val;
	    if ( idx < 0 ) {
		idx = this->find_theap_insert_point();
		hdr_rec.insert(idx, "THEAP", "", "");
		hdr_rec.at(idx).assign_default_comment(FITS::BINARY_TABLE_HDU);
	    }
	    val = (naxis1 * this->row_size_rec) + this->reserve_length_rec;
	    if ( hdr_rec.at(idx).type() != FITS::LONGLONG_T ||
		 hdr_rec.at(idx).llvalue() != val ) {
		hdr_rec.at(idx).assign(val);
	    }
	}
	else {
	    long idx = hdr_rec.index("THEAP");
	    if ( 0 <= idx ) {
		long long val = naxis1 * this->row_size_rec;
		if ( hdr_rec.at(idx).type() != FITS::LONGLONG_T ||
		     hdr_rec.at(idx).llvalue() != val ) {
		    hdr_rec.at(idx).assign(val);
		}
	    }
	}

	/* setup PCOUNT (length of reserved area + heap) */
	if ( 0 < this->reserve_length_rec || 0 < this->heap_rec.length() ) {
	    long long val = this->reserve_length_rec + this->heap_rec.length();
	    if ( hdr_rec.at("PCOUNT").type() != FITS::LONGLONG_T ||
		 hdr_rec.at("PCOUNT").llvalue() != val ) {
		hdr_rec.at("PCOUNT").assign(val);
	    }
	}
	else {
	    if ( hdr_rec.at("PCOUNT").type() != FITS::LONGLONG_T ||
		 hdr_rec.at("PCOUNT").llvalue() != 0 ) {
		hdr_rec.at("PCOUNT").assign(0L);
	    }
	}

    }
    catch (...) {
	this->header_set_sysrecords_prohibition(true);
	err_throw(__FUNCTION__,"FATAL","caught exception");
    }
    this->header_set_sysrecords_prohibition(true);

    /* debug */
#if 0
    {
    	long i;
    	for ( i=0 ; i < hdr_rec.length() ; i++ ) {
    	    err_report1(__FUNCTION__,"DEBUG","key : [%s]",
    			hdr_rec.record(i).keyword());
    	    err_report1(__FUNCTION__,"DEBUG","value : [%s]",
    			hdr_rec.record(i).value());
    	}
    }
#endif

    return *this;
}

/**
 * @brief  ����μ����Τ���Υ��дؿ�
 *
 * @note   ���Υ��дؿ��� protected �Ǥ���
 */
void **fits_table::data_ptr_mng_ptr( long col_index )
{
    if ( col_index < 0 || this->col_size_rec <= col_index ) return NULL;
    return this->col(col_index).data_ptr_mng_ptr();
}

/**
 * @brief  ����μ����Τ���Υ��дؿ�
 *
 * @note   ���Υ��дؿ��� protected �Ǥ���
 */
void **fits_table::heap_ptr_mng_ptr()
{
    return &this->heap_ptr_rec;
}

/* private member functions */
/**
 * @brief  �Х��ȥ��������Ѵ�
 *
 * @note   ���Υ��дؿ��� private �Ǥ���
 */
void fits_table::reverse_endian()
{
    long i;
    for ( i=0 ; i < this->col_size_rec ; i++ ) {
	fits_table_col *colp;
	colp = this->cols_ptr_rec[i];
	if ( colp->type() == FITS::LOGICAL_T ||
	     colp->type() == FITS::SHORT_T ||
	     colp->type() == FITS::LONG_T ||
	     colp->type() == FITS::LONGLONG_T ||
	     colp->type() == FITS::FLOAT_T ||
	     colp->type() == FITS::DOUBLE_T ) {
	    colp->data_rec.reverse_endian(false);
	}
	else if ( colp->type() == FITS::COMPLEX_T ||
		  colp->type() == FITS::DOUBLECOMPLEX_T ) {
	    ssize_t szt = colp->bytes() / 2;
	    colp->data_rec.reverse_byte_order(false, -szt);
	}
	else if ( colp->type() == FITS::LONGARRDESC_T ||
		  colp->type() == FITS::LLONGARRDESC_T ) {
	    ssize_t szt = colp->bytes() / 2;
	    colp->data_rec.reverse_byte_order(false, szt);
	}
    }
}

#include "private/parse_section_expression.cc"

/**
 * @brief  ���ȥ꡼�फ��ɬ�פ˱����� Header Unit ���ɤߡ�Data Unit ���ɤ�
 *
 *  Ķ���٥�ʥ��дؿ���
 *
 * @note   ���Υ��дؿ��� private �Ǥ���
 */
ssize_t fits_table::bte_load( const fits_hdu *objp, cstreamio &sref, 
       const char *section_to_read, bool is_last_hdu, const size_t *max_bytes_ptr )
{
    ssize_t return_val = -1;
    ssize_t total_read_len = 0;
    tstring xtension, txfldkwd;
    long tfields, naxis1, naxis2, naxis2_obj;
    long bytes_per_row;
    long long heap_length = 0;
    long long reserved_area_length = 0;
    size_t main_bytes_to_skip0;
    size_t main_bytes_to_read;
    size_t main_bytes_to_skip1;
    size_t len_main_heap;	/* byte length of main + heap           */
    size_t len_padding;		/* byte length of pad                   */
    size_t rest_skip;		/* 1��ǤޤȤ�� rskip ���뤿��Υ��塼 */
    mdarray_bool flag_required;
    section_exp_info section_info;
    long i;
    bool load_ate = false;
    const fits_header &hdr_rec = this->header_rec;

    /*
     * be empty
     */
    this->init();

    /* �ޤ��إå����ɤ�Ǥ��ʤ���� */
    if ( objp == NULL ) {
	if ( max_bytes_ptr != NULL ) {
	    total_read_len = fits_hdu::read_stream( sref, *max_bytes_ptr );
	}
	else {
	    total_read_len = fits_hdu::read_stream( sref );
	}
	if ( total_read_len < 0 ) {
	    err_report(__FUNCTION__,"ERROR","fits_hdu::read_stream() failed");
	    goto quit;
	}
    }
    else {
	/* ����������(�إå��ʤ�)�򥳥ԡ����� */
	this->fits_hdu::init(*objp);
    }

    /*
     * here, check system header
     */

    xtension = hdr_rec.record("XTENSION").svalue();
    if ( xtension.cstr() == NULL ) {
	err_report(__FUNCTION__,"ERROR","XTENSION keyword is not found");
	goto quit;
    }
    if ( xtension.strcmp("BINTABLE") != 0 ) {
	if ( xtension.strcmp("TABLE") == 0 ) {
	    load_ate = true;
	    this->set_hdutype(FITS::ASCII_TABLE_HDU);
	}
	else {
	    err_report(__FUNCTION__,"ERROR","This HDU is not TABLE");
	    goto quit;
	}
    }

    /* number of columns */
    tfields = hdr_rec.record("TFIELDS").llvalue();
    if ( tfields < 0 ) {
	err_report(__FUNCTION__,"ERROR","Invalid TFIELDS");
	goto quit;
    }

    /* number of rows */
    naxis2 = hdr_rec.record("NAXIS2").llvalue();
    if ( naxis2 < 0 ) {
	err_report(__FUNCTION__,"ERROR","Invalid NAXIS2");
	goto quit;
    }

    /* bytes of row */
    naxis1 = hdr_rec.record("NAXIS1").llvalue();
    if ( naxis1 < 0 ) {
	err_report(__FUNCTION__,"ERROR","Invalid NAXIS1");
	goto quit;
    }

    /* length of reserved area + heap */
    if ( 0 <= hdr_rec.index("PCOUNT") ) {
	heap_length = hdr_rec.record("PCOUNT").llvalue();
	if ( heap_length < 0 ) {
	    err_report(__FUNCTION__,"ERROR","Invalid PCOUNT");
	    goto quit;
	}
    }

    /* heap offset */
    if ( 0 <= hdr_rec.index("THEAP") ) {
	long long heap_off;
	heap_off = hdr_rec.record("THEAP").llvalue();
	if ( heap_off < naxis1 * naxis2 ) {
	    err_report(__FUNCTION__,"ERROR","Invalid THEAP");
	    goto quit;
	}
	reserved_area_length = heap_off - (naxis1 * naxis2);
	if ( heap_length < reserved_area_length ) {
	    err_report(__FUNCTION__,"ERROR","Invalid THEAP");
	    goto quit;
	}
	this->reserve_length_rec = reserved_area_length;
    }

    /* extended field keywords */
    if ( 0 <= hdr_rec.index("TXFLDKWD") ) {
	txfldkwd = hdr_rec.record("TXFLDKWD").svalue();
	if ( 0 < txfldkwd.length() ) {
	    tarray_tstring karr;
	    size_t j;
	    karr.split(txfldkwd, ",", true).trim();
	    /* erase elements listed in Field_keywords[] */
	    for ( j=0 ; Field_keywords[j] != NULL ; j++ ) {
		ssize_t k;
		if ( 0 <= (k=karr.find_elem(Field_keywords[j])) ) {
		    karr.erase(k,1);
		}
	    }
	    /* erase elements having 0-length string */
	    for ( j=karr.length() ; 0 < j ; ) {
		j--;
		if ( karr[j].length() == 0 ) karr.erase(j,1);
	    }
	    /* copy */
	    this->user_fld_kwd_rec = karr;
	}
    }

    //{
    //	long ii;
    //	for ( ii=0 ; ii < hdr_rec.length() ; ii++ ) {
    //	    err_report1(__FUNCTION__,"DEBUG","[%s]",
    //			hdr_rec.record(ii).keyword());
    //	    err_report1(__FUNCTION__,"DEBUG","index=[%ld]",
    //			hdr_rec.index( hdr_rec.record(ii).keyword() ));
    //	}
    //	err_report(__FUNCTION__,"DEBUG","===");
    //}

    /* 
     * check each column parameters and append each col
     */

    for ( i=0 ; i < tfields ; i++ ) {

	fits::table_def_all defs[] = { {NULL},{NULL} };
	fits::table_def_all &def = defs[0];

	tstring keyword;
	tstring tmp_tform;
	tstring tmp_tnull_val;
	//tarray_tstring talas;
	//tarray_tstring telem;
	long idx;

	def.ttype         = "";
	def.ttype_comment = "";
	//def.talas         = NULL;
	def.talas         = "";
	def.talas_comment = "";
	//def.telem         = NULL;
	def.telem         = "";
	def.telem_comment = "";
	def.tunit         = "";
	def.tunit_comment = "";
	def.tdisp         = "";
	def.tdisp_comment = "";
	def.tform         = "";
	def.tform_comment = "";
	def.tdim          = "";
	def.tdim_comment  = "";
	def.tnull         = "";
	def.tnull_comment = "";
	def.tzero         = "";
	def.tzero_comment = "";
	def.tscal         = "";
	def.tscal_comment = "";

	/* TTYPE */
	keyword.assignf("TTYPE%ld",i+1);
	idx = hdr_rec.index(keyword.cstr());
	if ( 0 <= idx ) {
	    def.ttype = hdr_rec.record(idx).svalue();
	    def.ttype_comment = hdr_rec.record(idx).comment();
	}
	/* TFORM -- ɬ�� */
	if ( load_ate == true ) {
	    long p0,p1;
	    keyword.assignf("TBCOL%ld",i+1);
	    idx = hdr_rec.index(keyword.cstr());
	    if ( idx < 0 ) {
		err_report1(__FUNCTION__,"ERROR","keyword %s is not found",
			    keyword.cstr());
		goto quit;
	    }
	    p0 = hdr_rec.record(idx).lvalue() - 1;
	    if ( i+1 < tfields ) {
		keyword.assignf("TBCOL%ld",i+2);
		idx = hdr_rec.index(keyword.cstr());
		if ( idx < 0 ) {
		    err_report1(__FUNCTION__,"ERROR","keyword %s is not found",
				keyword.cstr());
		    goto quit;
		}
		p1 = hdr_rec.record(idx).lvalue() - 1;
	    }
	    else p1 = (long)naxis1;
	    def.tform = tmp_tform.printf("%ldA",p1-p0).cstr();
	}
	else {
	    keyword.assignf("TFORM%ld",i+1);
	    idx = hdr_rec.index(keyword.cstr());
	    if ( idx < 0 ) {
		err_report1(__FUNCTION__,"ERROR","keyword %s is not found",
			    keyword.cstr());
		goto quit;
	    }
	    def.tform = hdr_rec.record(idx).svalue();
	    def.tform_comment = hdr_rec.record(idx).comment();
	}
	/* TUNIT */
	keyword.assignf("TUNIT%ld",i+1);
	idx = hdr_rec.index(keyword.cstr());
	if ( 0 <= idx ) {
	    def.tunit = hdr_rec.record(idx).svalue();
	    def.tunit_comment = hdr_rec.record(idx).comment();
	}
	/* TDISP */
	if ( load_ate == true ) keyword.assignf("TFORM%ld",i+1);
	else keyword.assignf("TDISP%ld",i+1);
	idx = hdr_rec.index(keyword.cstr());
	if ( 0 <= idx ) {
	    def.tdisp = hdr_rec.record(idx).svalue();
	    def.tdisp_comment = hdr_rec.record(idx).comment();
	}
	/* TDIM */
	keyword.assignf("TDIM%ld",i+1);
	idx = hdr_rec.index(keyword.cstr());
	if ( 0 <= idx ) {
	    def.tdim = hdr_rec.record(idx).svalue();
	    def.tdim_comment = hdr_rec.record(idx).comment();
	}
	/* TNULL */
	/* �����Ͽ��ͤΤߤ��б���ʸ�����ͤˤĤ��Ƥϸ�Ǥ��ʤ��� */
	keyword.assignf("TNULL%ld",i+1);
	idx = hdr_rec.index(keyword.cstr());
	if ( 0 <= idx ) {
	    tmp_tnull_val.assign(hdr_rec.record(idx).value()).trim("'");
	    tmp_tnull_val.trim(" ");
	    def.tnull = tmp_tnull_val.cstr();
	    def.tnull_comment = hdr_rec.record(idx).comment();
	}
	/* TZERO */
	keyword.assignf("TZERO%ld",i+1);
	idx = hdr_rec.index(keyword.cstr());
	if ( 0 <= idx ) {
	    def.tzero = hdr_rec.record(idx).svalue();
	    def.tzero_comment = hdr_rec.record(idx).comment();
	}
	/* TSCAL */
	keyword.assignf("TSCAL%ld",i+1);
	idx = hdr_rec.index(keyword.cstr());
	if ( 0 <= idx ) {
	    def.tscal = hdr_rec.record(idx).svalue();
	    def.tscal_comment = hdr_rec.record(idx).comment();
	}
	/* TALAS */
	keyword.assignf("TALAS%ld",i+1);
	idx = hdr_rec.index(keyword.cstr());
	if ( 0 <= idx ) {
	    //talas.split(hdr_rec.record(idx).svalue(),",",true);
	    //def.talas = talas.cstrarray();
	    def.talas = hdr_rec.record(idx).svalue();
	    def.talas_comment = hdr_rec.record(idx).comment();
	}
	/* TELEM */
	keyword.assignf("TELEM%ld",i+1);
	idx = hdr_rec.index(keyword.cstr());
	if ( 0 <= idx ) {
	    //telem.split(hdr_rec.record(idx).svalue(),",",true);
	    //def.telem = telem.cstrarray();
	    def.telem = hdr_rec.record(idx).svalue();
	    def.telem_comment = hdr_rec.record(idx).comment();
	}

	/* �������ɲ�(�إå��ι����Ϥ��ʤ�) */
	this->_append_cols(defs);

    }

    /* check NAXIS1 value */
    bytes_per_row = 0;    /* row ������ΥХ��ȿ� */
    for ( i=0 ; i < this->col_size_rec ; i++ ) {
	bytes_per_row += this->col(i).elem_byte_length();
    }

    if ( bytes_per_row != naxis1 ) {
	err_report(__FUNCTION__,"ERROR","invalid NAXIS1 value");
	goto quit;
    }

    /* ��ʬ�ɤߤλ��꤬������ϡ���ֻ����ʸ�����ѡ�������*/
    if ( section_to_read != NULL ) {
	if ( parse_section_expression(section_to_read, &section_info) < 0 ) {
	    err_report1(__FUNCTION__,"WARNING",
	           "syntax error in expression '%s'; ignored",section_to_read);
	    section_to_read = NULL;
	}
    }

    /* ɬ�פʥ�����ޡ������� */
    if ( 0 < section_info.begin.length() || 
	 0 < section_info.sel_cols.length() ) {
	try {
	    flag_required.resize(this->col_size_rec);
	}
	catch ( ... ) {
	    err_throw(__FUNCTION__,"FATAL","flag_required.resize() failed");
	}
	/* [1:10] �Τ褦�ʶ�֤ǡ��ϰϤ�����ʤ����Ϸٹ��Ф� */
	if ( section_info.sel_cols.length() == 0 ) {
	    if ( section_info.begin[0] + section_info.length[0] < 1 ||
		 this->col_size_rec <= section_info.begin[0] ) {
		err_report(__FUNCTION__,"WARNING",
	      "expression indicates out of range; no columns will be read");
	    }
	}
	for ( i=0 ; i < this->col_size_rec ; i++ ) {
	    bool hit = false;					/* ɬ�פ� ? */
	    /* [1:10] �Τ褦�ʶ�֤Ǥ����� */
	    if ( section_info.sel_cols.length() == 0 ) {
		if ( section_info.begin[0] <= i &&
		     i < section_info.begin[0] + section_info.length[0] ) {
		    hit = true;
		}
	    }
	    /* [foo; bar; 3] �Τ褦����󷿤Ǥ����� */
	    else {
		const fits::table_def &def = this->cols_ptr_rec[i]->definition();
		tstring def_ttype;		/* ttype         */
		tarray_tstring def_talas;	/* talas         */
		bool match_ok = false;		/* �ޥå�������? */
		size_t j;
		def_ttype.assign(def.ttype);
		if ( def.talas != NULL && def.talas[0] != '\0' ) {
		    def_talas.explode(def.talas,",",true).trim();
		}
		for ( j=0 ; j < section_info.sel_cols.length() ; j++ ) {
		    const tstring &col_pat = section_info.sel_cols[j];
		    bool c_del_flag = section_info.del_flag[j];
		    if ( c_del_flag == true || 
			 ( c_del_flag == false && hit == false ) ) {
			if ( def_ttype.strmatch(col_pat.cstr()) == 0 ) {
			    match_ok = true;
			}
			if ( section_info.digit_flag[j] == true ) {
			    if ( section_info.zero_indexed == true ) {
				if ( col_pat.atol() == i ) {
				    match_ok = true;
				}
			    }
			    else {
				if ( col_pat.atol() == i + 1 ) {
				    match_ok = true;
				}
			    }
			}
			if ( 0 < def_talas.length() ) {
			    size_t k;
			    for ( k=0 ; k < def_talas.length() ; k++ ) {
				if ( def_talas[k].strmatch(col_pat.cstr()) == 0 ) {
				    match_ok = true;
				}
			    }
			}
			if ( c_del_flag == false ) {
			    if ( match_ok == true ) {
				hit = true;
			    }
			}
			else {
			    /* ��Ƭ��'-'�������� */
			    if ( match_ok == true ) {
				hit = false;
			    }
			    else {
				/* �ǽ��'-'��������ϡ���������ν���Ȥ��� */
				if ( j == 0 ) {
				    hit = true;
				}
			    }
			}
		    }
		}
	    }
	    flag_required[i] = hit;
	}	/* for () ... */
    }


    /* 2�����ܤλ��꤬�����硤��ʬŪ�ʹԤ����ɤ�褦���� */
    if ( 1 < section_info.begin.length() ) {
	bool fixed_vals = false;
	/* �ϰϤ������� */
	if ( section_info.length[1] == FITS::ALL ) {
	    if ( section_info.begin[1] < 0 ) {
		section_info.begin[1] = 0;
		fixed_vals = true;
	    }
	    section_info.length[1] = naxis2 - section_info.begin[1];
	}
	/* �����ΰ褬�Ϥ߽Ф��Ƥ�����ν��� */
	if ( section_info.begin[1] < 0 ) {
	    if ( section_info.begin[1] + section_info.length[1] <= 0 ) {
		/* �ɤ�٤�Ĺ���ϥ��� */
		section_info.length[1] = 0;
	    }
	    else {
		/* �ɤ�٤�Ĺ���򾮤������� */
		section_info.length[1] += section_info.begin[1];
	    }
	    section_info.begin[1] = 0;
	    fixed_vals = true;
	}
	else if ( naxis2 <= section_info.begin[1] ) {
	    section_info.begin[1] = 0;
	    section_info.length[1] = 0;
	}
	if ( naxis2 < section_info.begin[1] + section_info.length[1]) {
	    section_info.length[1] = naxis2 - section_info.begin[1];
	    fixed_vals = true;
	}
	main_bytes_to_skip0 = bytes_per_row * section_info.begin[1];
	main_bytes_to_read = bytes_per_row * section_info.length[1];
	main_bytes_to_skip1 = 
	    bytes_per_row * (naxis2 - (section_info.begin[1] + section_info.length[1]));
	naxis2_obj = section_info.length[1];
	/* �ϰϤ�����ʤ����Ϸٹ��Ф� */
	if ( section_info.length[1] == 0 ) {
	    err_report(__FUNCTION__,"WARNING",
		    "expression indicates out of range; no rows will be read");
	}
	else if ( fixed_vals == true ) {
	    tstring rng_str;
	    if ( section_info.zero_indexed == false ) {
		rng_str.printf("[%ld:%ld] (ROW)",
		    (long)(section_info.begin[1] + 1),
		    (long)(section_info.begin[1] + section_info.length[1]));
	    }
	    else {
		rng_str.printf("(%ld:%ld) (ROW)",
		   (long)(section_info.begin[1]),
		   (long)(section_info.begin[1] + section_info.length[1] - 1));
	    }
	    err_report1(__FUNCTION__,"NOTICE", 
			"fixed range to %s",rng_str.cstr());
	}
    }
    /* �����ιԤ��ɤ�褦���� */
    else {
	main_bytes_to_skip0 = 0;
	main_bytes_to_read = bytes_per_row * naxis2;
	main_bytes_to_skip1 = 0;
	naxis2_obj = naxis2;
    }

    len_main_heap = (bytes_per_row * naxis2) + heap_length;
    if ( (len_main_heap % FITS::FILE_RECORD_UNIT) != 0 ) {
	len_padding = FITS::FILE_RECORD_UNIT
	    - (len_main_heap % FITS::FILE_RECORD_UNIT);
    }
    else {
	len_padding = 0;
    }

    if ( flag_required.length() == 0 ) {
	this->resize_rows(naxis2_obj);
    }
    else {
	for ( i=0 ; i < this->col_size_rec ; i++ ) {
	    if ( flag_required[i] != false ) {
		this->cols_ptr_rec[i]->_resize(naxis2_obj);
	    }
	}
	this->row_size_rec = naxis2_obj;
    }

    /*
     * skip 0 for main table
     */
    if ( 0 < main_bytes_to_skip0 ) {
	size_t len_to_skip = main_bytes_to_skip0;
	ssize_t len_skiped;

	/* �ɤ��Х��ȿ������¤���Ƥ����� */
	if ( max_bytes_ptr != NULL &&
	     (*max_bytes_ptr) < total_read_len + len_to_skip ) {
	    len_to_skip = (*max_bytes_ptr) - total_read_len;
	}

	try {
	    len_skiped = sref.rskip(len_to_skip);
	}
	catch (...) {
	    err_throw(__FUNCTION__,"FATAL","unexpected exception");
	}
	if ( len_skiped < 0 || (size_t)len_skiped != len_to_skip ) {
	    err_report(__FUNCTION__,"ERROR","sref.rskip() failed");
	    goto quit;
	}

	total_read_len += len_skiped;
    }

    /*
     * read main table of all or selected part
     */
    if ( 0 < main_bytes_to_read ) {
	size_t blklen_to_read = main_bytes_to_read;
	mdarray read_buffer(1, true);
	size_t read_buffer_size;
	size_t blklen_read;
	long target_row_idx;

	/* �ɤ��Х��ȿ������¤���Ƥ����� */
	if ( max_bytes_ptr != NULL &&
	     (*max_bytes_ptr) < total_read_len + blklen_to_read ) {
	    blklen_to_read = (*max_bytes_ptr) - total_read_len;
	}

	/* ����Хåե��Υ���������� */
	if ( DEFAULT_READ_BUFFER_SIZE < bytes_per_row ) {
	    read_buffer_size = bytes_per_row;
	}
	else {
	    long u = DEFAULT_READ_BUFFER_SIZE / bytes_per_row;
	    read_buffer_size = bytes_per_row * u;
	}

	try {
	    read_buffer.resize(read_buffer_size);
	}
	catch ( ... ) {
	    err_throw(__FUNCTION__,"FATAL","read_buffer.resize() failed");
	}

	blklen_read = 0;
	target_row_idx = 0;
	while ( blklen_read < blklen_to_read ) {
	    size_t len_to_read;
	    ssize_t len_read;
	    long nrow, off;
	    if ( blklen_read + read_buffer_size <= blklen_to_read ) {
		len_to_read = read_buffer_size;
	    }
	    else {
		len_to_read = blklen_to_read - blklen_read;
	    }
	    nrow = len_to_read / bytes_per_row;
	    try {
		len_read = sref.read(read_buffer.data_ptr(), len_to_read);
	    }
	    catch (...) {
		err_throw(__FUNCTION__,"FATAL","unexpected exception");
	    }
	    if ( len_read < 0 || (size_t)len_read != len_to_read ) {
		err_report(__FUNCTION__,"ERROR","sref.read() failed");
		goto quit;
	    }
	    blklen_read += len_read;
	    nrow = len_read / bytes_per_row;

	    off = 0;
	    for ( i=0 ; i < nrow ; i++ ) {
		long j;
		for ( j=0 ; j < this->col_size_rec ; j++ ) {
		    fits_table_col *colp;
		    colp = this->cols_ptr_rec[j];
		    if ( colp->data_ptr_rec != NULL ) {
			colp->data_rec.putdata( read_buffer.data_ptr(off),
						colp->elem_byte_length(),
						0, target_row_idx );
		    }
		    off += colp->elem_byte_length();
		}
		target_row_idx ++;
	    }
	}
	/* free buffer */
	read_buffer.resize(0);

	/* adjust endian of table part */
	this->reverse_endian();

	total_read_len += blklen_read;
    }

    rest_skip = 0;		/* �ޤȤ�� rskip ���뤿��Υ��塼�Υꥻ�å� */

    /*
     * skip 1 for main table
     */
    if ( 0 < main_bytes_to_skip1 ) {
	size_t len_to_skip = main_bytes_to_skip1;

	/* �ɤ��Х��ȿ������¤���Ƥ����� */
	if ( max_bytes_ptr != NULL &&
	     (*max_bytes_ptr) < total_read_len + len_to_skip ) {
	    len_to_skip = (*max_bytes_ptr) - total_read_len;
	}

	rest_skip += len_to_skip;

	total_read_len += len_to_skip;
    }

    /*
     * reading heap area
     */

    if ( 0 < reserved_area_length ) {
	size_t len_to_skip = reserved_area_length;

	/* �ɤ��Х��ȿ������¤���Ƥ����� */
	if ( max_bytes_ptr != NULL &&
	     (*max_bytes_ptr) < total_read_len + len_to_skip ) {
	    len_to_skip = (*max_bytes_ptr) - total_read_len;
	}

	rest_skip += len_to_skip;

	total_read_len += len_to_skip;
    }

    if ( 0 < (heap_length - reserved_area_length) ) {
	size_t len_to_read = (heap_length - reserved_area_length);
	ssize_t len_read;

	/* �ɤ��Х��ȿ������¤���Ƥ����� */
	if ( max_bytes_ptr != NULL &&
	     (*max_bytes_ptr) < total_read_len + len_to_read ) {
	    len_to_read = (*max_bytes_ptr) - total_read_len;
	}

	/* alloc heap buffer and read heap data */
	try {
	    this->heap_rec.resize( (heap_length - reserved_area_length) );
	}
	catch (...) {
	    err_throw(__FUNCTION__,"FATAL", "heap buffer allocation failed");
	}
	/* rest_skip ��ί�äƤ���ʬ��쵤�� rskip ����   */
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
	try {
	    len_read = sref.read(this->heap_rec.data_ptr(), len_to_read);
	}
	catch (...) {
	    err_throw(__FUNCTION__,"FATAL","unexpected exception");
	}
	if ( len_read < 0 || (size_t)len_read != len_to_read ) {
	    err_report(__FUNCTION__,"ERROR", "sref.read() failed");
	    goto quit;
	}

	total_read_len += len_read;
    }

    /*
     * skip pad
     */
    if ( 0 < len_padding ) {
	size_t len_to_skip = len_padding;

	/* �ɤ��Х��ȿ������¤���Ƥ����� */
	if ( max_bytes_ptr != NULL &&
	     (*max_bytes_ptr) < total_read_len + len_to_skip ) {
	    len_to_skip = (*max_bytes_ptr) - total_read_len;
	}

	rest_skip += len_to_skip;

	total_read_len += len_to_skip;
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

    /* ���פʥ����������� */
    if ( 0 < flag_required.length() ) {
	for ( i=this->col_size_rec ; 0 < i ; ) {
	    i--;
	    if ( flag_required[i] == false ) {
		this->erase_cols(i,1);
	    }
	}
    }

    /* ASCII �ơ��֥�ξ�硤���ߤ���¸����Ƥ��������Τǡ�
       ̤�������� ' ' ������ */
    if ( load_ate == true ) {
	tstring fval;
	for ( i=0 ; i < this->col_length() ; i++ ) {
	    const fits::table_def &def = this->col(i).definition();
	    int width = fval.assign(def.tdisp).atoi(1);
	    if ( 0 < width && width < this->col(i).elem_byte_length() ) {
		long j;
		for ( j=0 ; j < this->row_length() ; j++ ) {
		    tstring tval;
		    const char *v = this->col(i).string_value(j);
		    tval.assign(v).put(width,' ',
				       this->col(i).elem_byte_length() - width);
		    this->col(i).assign_string(tval.cstr(),j);
		}
	    }
	}
    }

    /* 
     * ʸ���󥫥����Ф��� TNULL ���ɲý���
     *
     * FITS�إå��ˤ� "" �Ǥ����� "           " �Ǥ����� 
     * '        '
     * �Τ褦��(8ʸ����)��¸����롥����ʸ���� NULL �ͤ˻Ȥ��ˤϡ�
     * ���餫����ꥵ�������Ƥ���ɬ�פ����롥2011/12/1
     */
    for ( i=0 ; i < this->col_length() ; i++ ) {
	if ( this->col(i).type() == FITS::STRING_T ) {
	    tstring keyword;
	    long idx;
	    keyword.assignf("TNULL%ld",i+1);
	    idx = hdr_rec.index(keyword.cstr());
	    if ( 0 <= idx ) {
		fits::table_def_all def;
		tstring tmpval;
		tmpval.assign(hdr_rec.record(idx).value());
		if ( 2 < tmpval.length() ) {
		    tmpval.trim("'");
		    tmpval.rtrim(" ");	/* ��¦�ζ���Τ߽��� */
		    if ( (long)(tmpval.length()) <= this->col(i).bytes() ) {
			tmpval.resize(this->col(i).bytes());
		    }
		    else {
			/* NULL�ͤ�Ĺ�����ǥ���������Ϸٹ��Ф� */
			err_report1(__FUNCTION__,"WARNING",
				    "too long TNULL string value : [%s].",
				    tmpval.cstr());
		    }
		    /* update */
		    def = this->col(i).definition_all();
		    def.tnull = tmpval.cstr();
		    //err_report1(__FUNCTION__,"DEBUG", "TNULL = [%s]",def.tnull);
		    this->col(i)._define(def);
		    //def = this->col(i).definition_all();
		    //err_report1(__FUNCTION__,"DEBUG", "TNULL = [%s]",def.tnull);
		}
	    }
	}
    }

    /* row ��ȿž���� */
    if ( 1 < section_info.begin.length() ) {
	if ( section_info.flip_flag[1] == true ) {
	    this->flip_rows(0, this->row_length());
	}
    }

    return_val = total_read_len;
 quit:
    return return_val;
}

/** 
 * @brief  shallow copy ����ǽ�����֤� (̤����)
 * 
 * @return  shallow copy����ǽ�ʤ鿿<br>
 *          ����ʳ��λ��ϵ�
 * @note  ���Υ��дؿ��� private �Ǥ�
 */
bool fits_table::request_shallow_copy( fits_table *from_obj ) const
{
    return false;
}

/** 
 * @brief  shallow copy �򥭥�󥻥뤹�� (̤����)
 * 
 * @note  ���Υ��дؿ��� private �Ǥ�
 */
void fits_table::cancel_shallow_copy( fits_table *from_obj ) const
{
    return;
}

/**
 * @brief  ���ȤˤĤ��ơ�shallow copy �ط��Υ��꡼�󥢥åפ�Ԥʤ� (̤����)
 * 
 * @note  ���Υ��дؿ��� private �Ǥ�
 */
void fits_table::cleanup_shallow_copy( bool do_deep_copy_for_this ) const
{
    return;
}


#include "private/write_stream_or_get_csum.cc"

}	/* namespace sli */

