/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2013-05-07 00:14:29 cyamauch> */

/**
 * @file   fits_header.cc
 * @brief  FITS �إå����Τ�ɽ�����륯�饹 fits_header �Υ�����
 */

#define CLASS_NAME "fits_header"

#include "config.h"

#include "fits_header.h"
#include "fits_hdu.h"

#include "private/err_report.h"

#include "private/rm_side_spaces.h"
#include "private/c_memcpy.h"


namespace sli
{

static const tstring END = "END";

#include "private/parse_a_header_record.h"
#include "private/initialize_csum.h"
#include "private/encode_csum.h"
#include "private/write_stream_or_get_csum.h"

/**
 * @brief  �����ƥ�¦�ǽ񤤤Ƥ��ޤ�ͽ�󥭡����
 *
 * @note  ͽ�󥭡���ɤ� '.' ������Ȥ�䤳�������ˤʤ�Τ�����ʤ�����<br>
 *        ('HOGE DEATH' �Τ褦�ʥ�����ɤ����ꤵ�줿����fits_header_record
 *        �Ǥ� ' ' �� '.' ���֤������뤿��)
 */
static const char *Header_reserved_keys[] = {
    "SIMPLE",
    "BITPIX","NAXIS","EXTEND", "XTENSION",
    "PCOUNT","GCOUNT","TFIELDS","TXFLDKWD","EXTNAME","EXTVER","EXTLEVEL",
    "BSCALE","BZERO",
#ifdef BUNIT_IS_SPECIAL
    "BUNIT",
#endif
    "BLANK", "THEAP",
#ifdef FMTTYPE_IS_SPECIAL
    "FMTTYPE","FTYPEVER",
#endif
    NULL };

/**
 * @brief  �����ƥ�¦�ǽ񤤤Ƥ��ޤ�ͽ�󥭡���� (�����˿������Ĥ����)
 */
static const char *Header_reserved_keys_with_digit[] = {
    "NAXIS",
    "TTYPE","TUNIT","TDISP","TBCOL","TFORM","TDIM",
    "TNULL","TZERO","TSCAL",
    "TELEM","TALAS",
    NULL };

/**
 * @brief  �����ƥ�¦��ͽ�󤷤Ƥ��륭����ɤ��ɤ�����Ƚ�� (������)
 *
 * @note    private �ʴؿ��Ǥ���
 */
static bool is_header_reserved_key( const char *keyword )
{
    int j;
    const char **reserved_keys;
    tstring keywd;
    if ( keyword == NULL ) return false;

    keywd.assign(keyword).strtrim();
    if ( keywd.length() == 0 ) return false;

    /* SIMPLE �Ȥ�������å����� */
    reserved_keys = Header_reserved_keys;
    for ( j=0 ; reserved_keys[j] != NULL ; j++ ) {
	if ( keywd.strcmp(reserved_keys[j]) == 0 ) break;
    }
    if ( reserved_keys[j] != NULL ) return true;

    /* TTYPE99 �Ȥ�������å����� */
    reserved_keys = Header_reserved_keys_with_digit;
    for ( j=0 ; reserved_keys[j] != NULL ; j++ ) {
	tstring rkeys;
	size_t len_reserved;
	len_reserved = rkeys.assign(reserved_keys[j]).length();
	/* reserved_keys[j] ��³���������ʤ����ϡ�reserved �Ȥ��ʤ� */
	if ( len_reserved < keywd.length() ) {
	    if ( keywd.strncmp(reserved_keys[j], len_reserved) == 0 ) {
		size_t p0 = len_reserved;
		if ( keywd.strpbrk(p0, "[0-9]") == (ssize_t)p0 ) {
		    p0 += keywd.strspn(p0, "[0-9]");
		    /* �Ǹ�ޤǤ��äƤ���� */
		    if ( p0 == keywd.length() ) break;
		}
	    }
	}
    }
    if ( reserved_keys[j] != NULL ) return true;

    return false;
}

/* constructor */
/**
 * @brief  ���󥹥ȥ饯��
 */
fits_header::fits_header()
{
    this->num_records_rec = 0;
    this->records_rec.init(sizeof(fits_header_record *), true);
    this->records_rec.register_extptr(&this->records_ptr_rec);	/* [void **] */
    c_memcpy(this->encoded_chksum_rec,"0000000000000000",17);
    this->sysrecords_prohibition_rec = false;
    this->suppress_dupkey_warning_rec = false;
    this->manager = NULL;

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
fits_header::fits_header(const fits_header &obj)
{
    this->num_records_rec = 0;
    this->records_rec.init(sizeof(fits_header_record *), true);
    this->records_rec.register_extptr(&this->records_ptr_rec);	/* [void **] */
    c_memcpy(this->encoded_chksum_rec,"0000000000000000",17);
    this->sysrecords_prohibition_rec = false;
    this->suppress_dupkey_warning_rec = false;
    this->manager = NULL;

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
fits_header::~fits_header()
{
    if ( this->records_ptr_rec != NULL ) {
	long i;
	for ( i=0 ; i < this->num_records_rec ; i++ ) {
	    if ( this->records_ptr_rec[i] != NULL ) {
		delete this->records_ptr_rec[i];
	    }
	}
    }
    return;
}

/**
 * @brief  ���֥������ȤΥ��ԡ�
 *
 * @note  ���Ȥ� fits_hdu ���֥������Ȥδ������ˤ����硤Data Unit �˴ط�����
 *        ͽ�󥭡����(BITPIX ��)����ĥ쥳���ɤϥ��ԡ�����ޤ���
 */
fits_header &fits_header::operator=(const fits_header &obj)
{
    return this->init(obj);
}

/* ���: ���Υ��饹����� this->manager->setup_sys_header() �θƤӽФ��� */
/*       1���дؿ��ˤĤ���1�ٸ¤�Ȥʤ�褦�ˤ��ʤ���Фʤ�ʤ���      */
/*       2��ƤӽФ��褦�ˤʤäƤ���ȡ�̵�¥롼�פ˴٤��롥             */

/* protected */
/**
 * @brief  ���֥������Ȥν���� (���٥�)
 *
 * @note  ����ϡ�this->manager->setup_sys_header() ��ƤӽФ��ʤ���<br>
 *        ���Υ��饹���� init() ��Ȥ��������˻Ȥ���<br>
 *        ���Υ��дؿ��� protected �Ǥ���
 */
fits_header &fits_header::_init()
{
    //err_report(__FUNCTION__,"DEBUG","this is fits_header::_init()");

    /* clear! */
    if ( this->records_ptr_rec != NULL ) {
	long i;
	for ( i=0 ; i < this->num_records_rec ; i++ ) {
	    if ( this->records_ptr_rec[i] != NULL ) {
		delete this->records_ptr_rec[i];
	    }
	}
	this->records_rec.init(sizeof(fits_header_record *), true);
    }
    this->num_records_rec = 0;
    c_memcpy(this->encoded_chksum_rec,"0000000000000000",17);
    this->suppress_dupkey_warning_rec = false;
    this->index_rec.init();
    this->formatted_rec.init();

    return *this;
}

/**
 * @brief  ���֥������Ȥν����
 */
fits_header &fits_header::init()
{
    //err_report(__FUNCTION__,"DEBUG","this is fits_header::init()");

    this->fits_header::_init();

    /* �õ��Ƥ� Data Unit �˴�Ϣ����쥳���ɤϤ��������褵���� */
    if ( this->sysrecords_prohibition_rec == true && this->manager != NULL ) {
	/* this will call an overridden function */
	//err_report(__FUNCTION__,"DEBUG","called ...... from [A]");
	this->manager->setup_sys_header();
    }

    return *this;
}

/**
 * @brief  ���֥������ȤΥ��ԡ�
 *
 * @note  ���Ȥ� fits_hdu ���֥������Ȥδ������ˤ����硤Data Unit �˴ط�����
 *        ͽ�󥭡����(BITPIX ��)����ĥ쥳���ɤϥ��ԡ�����ޤ���
 */
fits_header &fits_header::init(const fits_header &obj)
{
    if ( &obj == this ) return *this;

    //err_report(__FUNCTION__,"DEBUG","this is fits_header::init(...)");

    this->fits_header::_init();

    if ( obj.records_ptr_rec != NULL ) {
	long i;
	try {
	    this->records_rec.resize( obj.num_records_rec + 1 ).clean();
	}
	catch (...) {
	    err_throw(__FUNCTION__,"FATAL","records_rec.resize() failed");
	}
	try {
	    for ( i=0 ; i < obj.num_records_rec ; i++ ) {
		this->records_ptr_rec[i] = new fits_header_record;
		/* ����Ǥ��ޤ������Τ�??? */
		*(this->records_ptr_rec[i]) = (*(obj.records_ptr_rec[i]));
		this->records_ptr_rec[i]->register_manager(this);
	    }
	    this->records_ptr_rec[i] = NULL;
	}
	catch (...) {
	    for ( i=0 ; i < obj.num_records_rec ; i++ ) {
		if ( this->records_ptr_rec[i] != NULL  ) delete this->records_ptr_rec[i];
	    }
	    this->records_rec.init(sizeof(fits_header_record *), true);
	    err_throw(__FUNCTION__,"FATAL","'new' or '=' failed");
	}
	this->num_records_rec = obj.num_records_rec;
    }

    try {
	this->index_rec = obj.index_rec;
    }
    catch (...) {
	this->fits_header::_init();
	err_throw(__FUNCTION__,"FATAL","'=' failed");
    }

    this->suppress_dupkey_warning_rec = obj.suppress_dupkey_warning_rec;

    /* �����ƥ�إå����ػߤ���Ƥ����硤����к������ */
    if ( this->sysrecords_prohibition_rec == true ) {
	long i;
	for ( i = 0 ; i < this->length() ; i++ ) {
	    if ( this->is_sysrecord(i) == true ) {
		//err_report1(__FUNCTION__,"WARNING",
		//	    "keyword '%s' cannot be appended by the user",
		//	    this->record_cs(i).keyword());
		this->fits_header::_erase_records(i,1);
		i--;
	    }
	}
    }

    /* �õ��Ƥ� Data Unit �˴�Ϣ����쥳���ɤϤ��������褵���� */
    if ( this->sysrecords_prohibition_rec == true && this->manager != NULL ) {
	/* this will call an overridden function */
	//err_report(__FUNCTION__,"DEBUG","called ...... from [B]");
	this->manager->setup_sys_header();
    }

    //try {
    //	this->formatted_rec = obj.formatted_rec;
    //}
    //catch (...) {
    //	err_throw(__FUNCTION__,"FATAL","'=' failed");
    //}

    return *this;
}

/**
 * @brief  ���֥������Ȥν���� (fits::header_def ��¤�Τǻ���)
 *
 * @note  ���Ȥ� fits_hdu ���֥������Ȥδ������ˤ����硤Data Unit �˴ط�����
 *        ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_header &fits_header::init( const fits::header_def defs[] )
{
    this->fits_header::_init();

    if ( defs != NULL ) {
	this->append_records(defs);
    }

    /* �õ��Ƥ� Data Unit �˴�Ϣ����쥳���ɤϤ��������褵���� */
    if ( this->sysrecords_prohibition_rec == true && this->manager != NULL ) {
	/* this will call an overridden function */
	//err_report(__FUNCTION__,"DEBUG","called ...... from [C]");
	this->manager->setup_sys_header();
    }

    return *this;
}

/**
 * @brief  2�ĤΥ��֥������ȴ֤Ǥ����ƤΥ���å�
 *
 *  ���ꤵ�줿���֥������� obj �����Ƥȼ��Ȥ����Ƥ������ؤ��ޤ���
 *
 * @param   obj fits_header ���饹�Υ��֥�������
 * @return  ���Ȥλ���    
 * @note  ���Ȥ� fits_hdu ���֥������Ȥδ������ˤ����硤Data Unit �˴ط�����
 *        ͽ�󥭡����(BITPIX ��)����ĥ쥳���ɤ������ؤ��ޤ���
 */
/* ��: chksum �Ϥ����Ǥϰ���ʤ� */
fits_header &fits_header::swap( fits_header &obj )
{
    long tmp_num_records_rec = obj.num_records_rec;
    bool tmp_suppress_dupkey_warning_rec = obj.suppress_dupkey_warning_rec;

    if ( &obj == this ) return *this;

    obj.num_records_rec = this->num_records_rec;

    this->num_records_rec = tmp_num_records_rec;

    this->records_rec.swap(obj.records_rec);

    this->index_rec.swap(obj.index_rec);

    this->formatted_rec.swap(obj.formatted_rec);

    obj.suppress_dupkey_warning_rec = this->suppress_dupkey_warning_rec;
    this->suppress_dupkey_warning_rec = tmp_suppress_dupkey_warning_rec;

    /* �����ƥ�إå����ػߤ���Ƥ����硤����к������ */
    if ( this->sysrecords_prohibition_rec == true ) {
	long i;
	for ( i = 0 ; i < this->length() ; i++ ) {
	    if ( this->is_sysrecord(i) == true ) {
		//err_report1(__FUNCTION__,"WARNING",
		//	    "keyword '%s' cannot be appended by the user",
		//	    this->record_cs(i).keyword());
		this->fits_header::_erase_records(i,1);
		i--;
	    }
	}
    }
    if ( obj.sysrecords_prohibition_rec == true ) {
	long i;
	for ( i = 0 ; i < obj.length() ; i++ ) {
	    if ( obj.is_sysrecord(i) == true ) {
		//err_report1(__FUNCTION__,"WARNING",
		//	    "keyword '%s' cannot be appended by the user",
		//	    obj.record_cs(i).keyword());
		obj.fits_header::_erase_records(i,1);
		i--;
	    }
	}
    }

    /* �õ��Ƥ� Data Unit �˴�Ϣ����쥳���ɤϤ��������褵���� */
    if ( this->sysrecords_prohibition_rec == true && this->manager != NULL ) {
	/* this will call an overridden function */
	this->manager->setup_sys_header();
    }
    if ( obj.sysrecords_prohibition_rec == true && obj.manager != NULL ) {
	/* this will call an overridden function */
	obj.manager->setup_sys_header();
    }

    return *this;
}

/*
 * �ե����������� : �ɤ߽񤭤����Х��ȿ����֤�
 */

/**
 * @brief  ���ȥ꡼�फ�� Header Unit ���ɤ߹���
 *
 * @param   sref �ɤ߹����оݤΥ��ȥ꡼�� (cstreamio�ηѾ����饹)
 * @param   max_bytes_read �ɤ߹��ޤ��٤�����ΥХ��ȿ�
 * @return  ������: �ɤ߹�����Х��ȿ�<br>
 *          ����: ���顼
 */
ssize_t fits_header::read_stream( cstreamio &sref, size_t max_bytes_read )
{
    if ( this->sysrecords_prohibition_rec == false ) {
	return this->header_load(sref,&max_bytes_read);
    }
    else {
	err_report(__FUNCTION__,"ERROR",
		   "cannot use this member function.");
	return -1;
    }
}

/**
 * @brief  ���ȥ꡼�फ�� Header Unit ���ɤ߹���
 *
 * @param   sref �ɤ߹����оݤΥ��ȥ꡼�� (cstreamio�ηѾ����饹)
 * @return  ������: �ɤ߹�����Х��ȿ�<br>
 *          ����: ���顼
 */
ssize_t fits_header::read_stream( cstreamio &sref )
{
    if ( this->sysrecords_prohibition_rec == false ) {
	return this->header_load(sref,NULL);
    }
    else {
	err_report(__FUNCTION__,"ERROR",
		   "cannot use this member function.");
	return -1;
    }
}

/**
 * @brief  ʸ����Хåե����� Header Unit ���ɤ߹���
 *
 * @param   buffer �ɤ߹����оݤΥХåե�
 * @return  ������: �ɤ߹�����Х��ȿ�<br>
 *          ����: ���顼
 */
ssize_t fits_header::read_buffer( const char *buffer )
{
    if ( this->sysrecords_prohibition_rec == false ) {
	return this->header_load(buffer);
    }
    else {
	err_report(__FUNCTION__,"ERROR",
		   "cannot use this member function.");
	return -1;
    }
}

/**
 * @brief  ���ȥ꡼��˥�����������Data Unit ���ɤ����Ф�
 *
 * @param   sref �ɤ����Ф��оݤΥ��ȥ꡼�� (cstreamio�ηѾ����饹)
 * @param   max_bytes_skip �ɤ����Ф����٤�����ΥХ��ȿ�
 * @return  ������: �ɤ����Ф����Х��ȿ�<br>
 *          ����: ���顼
 */
ssize_t fits_header::skip_data_stream( cstreamio &sref, size_t max_bytes_skip )
{
    return this->data_skip(sref,&max_bytes_skip);
}

/**
 * @brief  ���ȥ꡼��˥�����������Data Unit ���ɤ����Ф�
 *
 * @param   sref �ɤ����Ф��оݤΥ��ȥ꡼�� (cstreamio�ηѾ����饹)
 * @return  ������: �ɤ����Ф����Х��ȿ�<br>
 *          ����: ���顼
 */
ssize_t fits_header::skip_data_stream( cstreamio &sref )
{
    return this->data_skip(sref,NULL);
}

/**
 * @brief  ���ȥ꡼��ؤ� Header Unit �ν񤭹���
 *
 * @param   sref �񤭹����оݤΥ��ȥ꡼�� (cstreamio�ηѾ����饹)
 * @param   end_and_blank ��END�ץ�����ɤ�2880�Х��ȥ��饤��Τ����;���
 *                        �ɲä������ true
 * @return  ������: �񤭹�����Х��ȿ�<br>
 *          ����: ���顼
 */
ssize_t fits_header::write_stream( cstreamio &sref, bool end_and_blank )
{
    return this->header_save( &sref, NULL, end_and_blank );
}

/* discard original 80-char record, and reformat all records */
/**
 * @brief  ���إå��쥳���ɤκƥե����ޥåȤ�Ԥʤ�
 *
 *  �� fits_header_record �������ݻ����Ƥ��� 80ʸ���Υե����ޥåȺѤ�ʸ�����
 *  ���ߤΥ�����ɡ��͡������Ȥ����Ƥ�����ʤ����ޤ���
 */
fits_header &fits_header::reformat()
{
    long i;
    for ( i=0 ; i < this->length() ; i++ ) {
	this->record(i).reformat();
    }
    return *this;
}

/**
 * @brief  ���إå��쥳���ɤΥե����ޥåȺѤ�ʸ����(80��nʸ��)�����
 *
 *  ���֥�����������������줿�����إå��쥳���ɤΥե����ޥåȺѤ�ʸ����
 *  (80��nʸ��)���֤��ޤ�������ʸ���ϴޤޤ줺��ʸ����� '\0' �ǽ�ü���Ƥ��ޤ���
 *  
 * @return  �ե����ޥåȺѤ�ʸ����(80��nʸ��)
 */
const char *fits_header::formatted_string()
{
    long i;
    size_t pos = 0;
    for ( i=0 ; i < this->length() ; i++ ) {
	tstring tmp_str;
	size_t len;
	tmp_str = this->record(i).formatted_string();
	if ( 0 < (len=tmp_str.length()) ) {
	    if ( this->formatted_rec.length() < pos + len ) {
		size_t to_add;
		/* ���ä�����ݤ��� */
		to_add = FITS::HEADER_RECORD_UNIT * this->length() * 2;
		this->formatted_rec.resizeby(to_add);
	    }
	    this->formatted_rec.put(pos,tmp_str);
	    pos += len;
	}
    }
    /* �ǽ�Ū��Ĺ��Ĵ�� */
    this->formatted_rec.resize(pos);

    return this->formatted_rec.cstr();
}

/**
 * @brief  ���إå��쥳���ɤΥե����ޥåȺѤ�ʸ����(80��nʸ��)��Ĺ�������
 *
 * @note  '\0' ��Ĺ���˴ޤޤ�ޤ���
 */
size_t fits_header::formatted_length()
{
    long i;
    size_t len_all = 0;
    for ( i=0 ; i < this->length() ; i++ ) {
	len_all += this->record(i).formatted_length();
    }
    return len_all;
}

/* ʣ���ԤǤ��Խ� */

/**
 * @brief  ʣ���Υإå��쥳���ɤ��ɲá�����
 *
 *  ������ɤ�¸�ߤ��ʤ������ɲä���Ʊ��Υ�����ɤ�¸�ߤ������
 *  ���ꤵ�줿���ƤǾ�񤭤��ޤ���
 *
 * @param  obj ������
 * @note  ���Ȥ� fits_hdu ���֥������Ȥδ������ˤ����硤Data Unit �˴ط�����
 *        ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_header &fits_header::update_records( const fits_header &obj )
{
    long i, n_new = 0;
    /* keyrowd, value, length */
    fits::header_def *tmp_defs;
    mdarray tmp_defs_rec(sizeof(*tmp_defs), true, &tmp_defs);	/* [void **] */
    /* formatted 80-char string */
    const char **tmp_formatted;
    mdarray tmp_formatted_rec(sizeof(*tmp_formatted), true, 
			      &tmp_formatted);	/* [void **] */

    tmp_defs_rec.resize(obj.length());
    tmp_formatted_rec.resize(obj.length());

    for ( i=0 ; i < obj.length() ; i++ ) {
	const fits_header_record &robj = obj.record_cs(i);
	if ( robj.status() == FITS::NORMAL_RECORD ) {
	    long idx;
	    /* description �Ǥʤ� index ��õ�� */
	    idx = this->index(robj.keyword());
	    if ( 0 <= idx ) this->assign(idx, robj);
	}
    }
    for ( i=0 ; i < obj.length() ; i++ ) {
	const fits_header_record &robj = obj.record_cs(i);
	if ( robj.status() == FITS::NORMAL_RECORD ) {
	    long idx;
	    /* description �Ǥʤ� index ��õ�� */
	    idx = this->index(robj.keyword());
	    if ( idx < 0 ) {
		tmp_defs[n_new] = robj.raw_record();
		tmp_formatted[n_new] = robj.a_formatted_rec.cstr();
		n_new ++;
	    }
	}
    }
    this->append_records(tmp_defs, tmp_formatted, n_new);

    return *this;
}

/**
 * @brief  ͽ�󥭡����(BITPIX ��)��Ѳ����뤿��������Ѥδؿ� (������)
 *
 * @note    private �ʴؿ��Ǥ���
 */
static long reject_sysheader( const fits::header_def defs[], 
			      const char *formatted_str[], long num_defs,
			      mdarray *tmp_defs_rec, 
			      mdarray *tmp_formatted_str,
			      const char *fncnam )
{
    long i;
    bool ok_all = true;
    fits::header_def *tmp_defs = NULL;
    const char **tmp_formatted = NULL;
    
    /* �����ƥ��ѥ�����ɤϥꥸ�����Ȥ��� */
    for ( i=0 ; i < num_defs ; i++ ) {
	if ( defs[i].value != NULL && defs[i].comment != NULL &&
	     is_header_reserved_key(defs[i].keyword) == true )
	    ok_all = false;
    }

    if ( ok_all == false ) {
	long j;
	tmp_defs_rec->resize(num_defs);
	tmp_defs = (fits::header_def *)tmp_defs_rec->data_ptr();
	if ( formatted_str != NULL ) {
	    tmp_formatted_str->resize(num_defs);
	    tmp_formatted = (const char **)tmp_formatted_str->data_ptr();
	}
	for ( i=0, j=0 ; i < num_defs ; i++ ) {
	    if ( defs[i].value != NULL && defs[i].comment != NULL &&
		 is_header_reserved_key(defs[i].keyword) == true ) {
		err_report1(fncnam,"WARNING",
			    "keyword '%s' cannot be appended by the user",
			    defs[i].keyword);
	    }
	    else {
		tmp_defs[j].keyword = defs[i].keyword;
		tmp_defs[j].value   = defs[i].value;
		tmp_defs[j].comment = defs[i].comment;
		if ( formatted_str != NULL ) {
		    tmp_formatted[j] = formatted_str[i];
		}
		j++;
	    }
	}
	num_defs = j;
    }
 
    return num_defs;
}


/* private */

/**
 * @brief  ʣ���Υإå��쥳���ɤ��ɲ� (����������)
 *
 * @note   ���Υ��дؿ��� private �Ǥ���
 */
fits_header &fits_header::append_records(const fits::header_def defs[], 
 					 const char *formatted_str[],
					 long num_defs)
{
    long i;
    fits_header_record *tmp_rec_ptr;
    long begin_index;
    mdarray tmp_defs_rec(sizeof(fits::header_def), true);
    mdarray tmp_formatted_str(sizeof(const char *), true);
    const fits::header_def *defs_ptr = defs;

    begin_index = this->num_records_rec;

    /* ���������å� */
    if ( num_defs < 0 ) goto quit;
    if ( defs == NULL || num_defs == 0 ) {
	goto quit;
    }

    /* �����ƥ�إå����ػߤ���Ƥ����硤������ʤ���Τ�ꥸ�����Ȥ��� */
    if ( this->sysrecords_prohibition_rec == true ) {
	long n = reject_sysheader(defs, formatted_str, num_defs, 
			      &tmp_defs_rec, &tmp_formatted_str, __FUNCTION__);
	if ( n < num_defs ) {
	    num_defs = n;
	    defs_ptr = (fits::header_def *)tmp_defs_rec.data_ptr();
	    if ( formatted_str != NULL ) {
		formatted_str = (const char **)tmp_formatted_str.data_ptr();
	    }
	}
    }

    /* ���ɥ쥹�ơ��֥�򹭤��� */
    try {
	this->records_rec.resize(this->num_records_rec + num_defs + 1);
    }
    catch (...) {
	err_throw(__FUNCTION__,"FATAL","records_rec.resize() failed");
    }
    
    /* 1��1�� new ���� */
    for ( i=0 ; i < num_defs ; i++ ) {
	bool register_index = true;
	//fprintf(stderr,"debug: idx=%d\n",i);
	try {
	    tmp_rec_ptr = new fits_header_record;
	    tmp_rec_ptr->register_manager(this);
	}
	catch (...) {
	    err_throw(__FUNCTION__,"FATAL","'new' failed");
	}
	try {
	    tmp_rec_ptr->assign_any(defs_ptr[i]);
	    if ( formatted_str != NULL ) {
		tmp_rec_ptr->a_formatted_rec = formatted_str[i];
	    }
	}
	catch (...) {
	    delete tmp_rec_ptr;
	    err_throw(__FUNCTION__,"FATAL","tmp_rec_ptr->assign_any() failed");
	}
	if ( tmp_rec_ptr->status() != FITS::NORMAL_RECORD ) {
	    register_index = false; /* normal �ʳ��Ǥ� index ��������ʤ� */
	}
	else {
	    if ( 0 <= this->index(tmp_rec_ptr->keyword()) ) {
		if ( this->suppress_dupkey_warning_rec == false ) {
		    err_report1(__FUNCTION__,"WARNING",
		 "duplicated keyword '%s' is appended",tmp_rec_ptr->keyword());
		}
	    }
	}
	if ( register_index == true ) {	/* ����ǥå������������ */
	    int s;
	    /* 
	       [���]
	       index_rec �ؤ� keyword ��Ͽ�� fits_header_record ���֥������Ȥ�
	       ��Ͽ�ѤߤΤ�Τ��ɲä��뤳��(FITS���ʤ�Ŭ�礷�ʤ�������ɤ�
	       ��硤ʸ�����֤��������礬����Τ�)
	       duplicated keyword �Ƿٹ��Ф�����Ʊ�͡�
	     */
	    s = this->index_rec.append( tmp_rec_ptr->keyword(),
					begin_index + i );
	    if ( s < 0 ) {
		delete tmp_rec_ptr;
		err_throw(__FUNCTION__,"FATAL",
			  "this->index_rec.append() failed");
	    }
	}

	this->records_ptr_rec[begin_index + i] = tmp_rec_ptr;
	tmp_rec_ptr = NULL;

	this->num_records_rec ++;
    }

 quit:
    return *this;
}

/* public */

/**
 * @brief  ʣ���Υإå��쥳���ɤ��ɲ� (fits::header_def ��¤�Τǻ���)
 *
 * @note  ���Ȥ� fits_hdu ���֥������Ȥδ������ˤ����硤Data Unit �˴ط�����
 *        ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_header &fits_header::append_records( const fits::header_def defs[] )
{
    long num_defs = 0;
    if ( defs == NULL ) {
	return this->append_records( defs, num_defs );
    }
    for ( num_defs=0 ; defs[num_defs].keyword != NULL ; num_defs++ ) {
	if ( END.strcmp(defs[num_defs].keyword) == 0 &&
	     defs[num_defs].value == NULL && 
	     defs[num_defs].comment == NULL ) break;
    }
    return this->append_records( defs, num_defs );
}

/**
 * @brief  ʣ���Υإå��쥳���ɤ��ɲ� (fits::header_def ��¤�Τǻ���)
 *
 * @note  ���Ȥ� fits_hdu ���֥������Ȥδ������ˤ����硤Data Unit �˴ط�����
 *        ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_header &fits_header::append_records(const fits::header_def defs[], 
					 long num_defs)
{
    return this->append_records(defs, NULL, num_defs);
}

/**
 * @brief  ʣ���Υإå��쥳���ɤ��ɲ� (fits_header ���֥������Ȥǻ���)
 *
 * @note  ���Ȥ� fits_hdu ���֥������Ȥδ������ˤ����硤Data Unit �˴ط�����
 *        ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_header &fits_header::append_records( const fits_header &obj )
{
    if ( 0 < obj.length() ) {
	long i;
	/* keyword, value, comment */
	fits::header_def *tmp_defs;
	mdarray tmp_defs_rec(sizeof(*tmp_defs),true,&tmp_defs); /* [void **] */
	/* formatted 80-char string */
	const char **tmp_formatted;
	mdarray tmp_formatted_rec(sizeof(*tmp_formatted),true,
				  &tmp_formatted); /* [void **] */

	tmp_defs_rec.resize(obj.length());
	tmp_formatted_rec.resize(obj.length());

	for ( i=0 ; i < obj.length() ; i++ ) {
	    tmp_defs[i] = obj.record_cs(i).raw_record();
	    tmp_formatted[i] = obj.record_cs(i).a_formatted_rec.cstr();
	}

	this->append_records(tmp_defs, tmp_formatted, obj.length());
    }

    return *this;
}


/* private */

/**
 * @brief  ʣ���Υإå��쥳���ɤ����� (����������)
 *
 * @note   ���Υ��дؿ��� private �Ǥ���
 */
fits_header &fits_header::insert_records( long index0, 
					  const fits::header_def defs[], 
					  const char *formatted_str[],
					  long num_defs )
{
    fits_header_record *tmp_rec_ptr;
    long i;
    mdarray tmp_defs_rec(sizeof(fits::header_def), true);
    mdarray tmp_formatted_str(sizeof(const char *), true);
    const fits::header_def *defs_ptr = defs;

    /* ���������å� */
    if ( index0 < 0 ) index0 = 0;
    if ( this->num_records_rec < index0 ) index0 = this->num_records_rec;
    if ( this->num_records_rec == index0 ) {
	return append_records(defs,num_defs);
    }
    if ( num_defs < 0 ) goto quit;
    if ( defs == NULL || num_defs == 0 ) {
	goto quit;
    }

    /* �����ƥ�إå����ػߤ���Ƥ����硤������ʤ���Τ�ꥸ�����Ȥ��� */
    if ( this->sysrecords_prohibition_rec == true ) {
	long n = reject_sysheader(defs, formatted_str, num_defs, 
			      &tmp_defs_rec, &tmp_formatted_str, __FUNCTION__);
	if ( n < num_defs ) {
	    num_defs = n;
	    defs_ptr = (fits::header_def *)tmp_defs_rec.data_ptr();
	    if ( formatted_str != NULL ) {
		formatted_str = (const char **)tmp_formatted_str.data_ptr();
	    }
	}
    }

    /* ���ɥ쥹�ơ��֥�򹭤��� */
    try {
	this->records_rec.resize(this->num_records_rec + num_defs + 1);
    }
    catch (...) {
	err_throw(__FUNCTION__,"FATAL","records_rec.resize() failed");
    }

    /* 1��1�� new ���� */
    for ( i=0 ; i < num_defs ; i++ ) {
	bool register_index = true;
	long j;
	try {
	    tmp_rec_ptr = new fits_header_record;
	    tmp_rec_ptr->register_manager(this);
	}
	catch (...) {
	    err_throw(__FUNCTION__,"FATAL","'new' failed");
	}
	try {
	    tmp_rec_ptr->assign_any(defs_ptr[i]);
	    if ( formatted_str != NULL ) {
		tmp_rec_ptr->a_formatted_rec = formatted_str[i];
	    }
	}
	catch (...) {
	    delete tmp_rec_ptr;
	    err_throw(__FUNCTION__,"FATAL","tmp_rec_ptr->assign_any() failed");
	}
	if ( tmp_rec_ptr->status() != FITS::NORMAL_RECORD ) {
	    register_index = false; /* normal �ʳ��Ǥ� index ��������ʤ� */
	}
	else {
	    if ( 0 <= this->index(tmp_rec_ptr->keyword()) ) {
		if ( this->suppress_dupkey_warning_rec == false ) {
		    err_report1(__FUNCTION__,"WARNING",
		 "duplicated keyword '%s' is inserted",tmp_rec_ptr->keyword());
		}
	    }
	}

	/* �������� insert ���褦�Ȥ���Τ�� index ����������          */
	/*   duplicated keyword �λ��ˡ���ʣ���� index ��������褦�Ȥ��� */
	/*   ���Ԥ���                                                     */

	/* �������ϼ��Ԥ��ʤ� */

	/* ���餷��(��ü��NULL��ʬ��)����֤���Ͽ���� */
	this->records_rec.move( index0 + i, 
				this->num_records_rec - i - index0 + 1,
				index0 + i + 1, false );

	this->records_ptr_rec[index0 + i] = tmp_rec_ptr;
	this->num_records_rec ++;

	//fprintf(stderr,"debug: i=%ld\n",i);
	/* ���餷���Ȥ���� index ����ʤ��� */
	for ( j = index0 + i + 1 ; j < this->num_records_rec ; j++ ) {
	    if ( this->records_ptr_rec[j] != NULL &&
		 this->records_ptr_rec[j]->status() == FITS::NORMAL_RECORD ) {
		int s;
		//fprintf(stderr,"debug: updated A i=%ld [%ld]\n",i,j);
		s= this->index_rec.update(this->records_ptr_rec[j]->keyword(), j-1, j);
		if ( s < 0 ) {
		    err_throw(__FUNCTION__,"FATAL",
			    "Internal ERROR: this->index_rec.update() failed");
		}
	    }
	}

	/* insert ���褦�Ȥ���Τ�Υ���ǥå������������ */
	if ( register_index == true ) {
	    int s;
	    s = this->index_rec.append(tmp_rec_ptr->keyword(),
				       index0 + i);
	    if ( s < 0 ) {
		delete tmp_rec_ptr;
		err_throw(__FUNCTION__,"FATAL",
			  "this->index_rec.append() failed");
	    }
	}
    }


 quit:
    return *this;
}

/* public */

/**
 * @brief  ʣ���Υإå��쥳���ɤ����� (fits::header_def ��¤�Τǻ���)
 *
 * @note  ���Ȥ� fits_hdu ���֥������Ȥδ������ˤ����硤Data Unit �˴ط�����
 *        ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_header &fits_header::insert_records( long index0,
					  const fits::header_def defs[] )
{
    long num_defs = 0;
    if ( defs == NULL ) {
	return this->insert_records( index0, defs, num_defs );
    }
    for ( num_defs=0 ; defs[num_defs].keyword != NULL ; num_defs++ ) {
	if ( END.strcmp(defs[num_defs].keyword) == 0 &&
	     defs[num_defs].value == NULL && 
	     defs[num_defs].comment == NULL ) break;
    }
    return this->insert_records( index0, defs, num_defs );
}

/**
 * @brief  ʣ���Υإå��쥳���ɤ����� (fits::header_def ��¤�Τǻ���)
 *
 * @note  ���Ȥ� fits_hdu ���֥������Ȥδ������ˤ����硤Data Unit �˴ط�����
 *        ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_header &fits_header::insert_records( long index0, 
					  const fits::header_def defs[], 
					  long num_defs )
{
    return this->insert_records( index0, defs, NULL, num_defs);
}

/**
 * @brief  ʣ���Υإå��쥳���ɤ����� (fits::header_def ��¤�Τǻ���)
 *
 * @note  ���Ȥ� fits_hdu ���֥������Ȥδ������ˤ����硤Data Unit �˴ط�����
 *        ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_header &fits_header::insert_records( const char *keyword0, 
					  const fits::header_def defs[] )
{
    return this->insert_records( this->index(keyword0), defs );
}

/**
 * @brief  ʣ���Υإå��쥳���ɤ����� (fits::header_def ��¤�Τǻ���)
 *
 * @note  ���Ȥ� fits_hdu ���֥������Ȥδ������ˤ����硤Data Unit �˴ط�����
 *        ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_header &fits_header::insert_records( const char *keyword0, 
				 const fits::header_def defs[], long num_defs )
{
    return this->insert_records( this->index(keyword0), defs, num_defs );
}

/**
 * @brief  ʣ���Υإå��쥳���ɤ����� (fits_header ���֥������Ȥǻ���)
 *
 * @note  ���Ȥ� fits_hdu ���֥������Ȥδ������ˤ����硤Data Unit �˴ط�����
 *        ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_header &fits_header::insert_records( long index0, const fits_header &obj )
{
    if ( 0 < obj.length() ) {
	long i;
	/* keyword, value, comment */
	fits::header_def *tmp_defs;
	mdarray tmp_defs_rec(sizeof(*tmp_defs),true,&tmp_defs); /* [void **] */
	/* formatted 80-char string */
	const char **tmp_formatted;
	mdarray tmp_formatted_rec(sizeof(*tmp_formatted),true,
				  &tmp_formatted); /* [void **] */

	tmp_defs_rec.resize(obj.length());
	tmp_formatted_rec.resize(obj.length());

	for ( i=0 ; i < obj.length() ; i++ ) {
	    tmp_defs[i] = obj.record_cs(i).raw_record();
	    tmp_formatted[i] = obj.record_cs(i).a_formatted_rec.cstr();
	}

	this->insert_records(index0, tmp_defs, tmp_formatted, obj.length());
    }

    return *this;
}

/**
 * @brief  ʣ���Υإå��쥳���ɤ����� (fits_header ���֥������Ȥǻ���)
 *
 * @note  ���Ȥ� fits_hdu ���֥������Ȥδ������ˤ����硤Data Unit �˴ط�����
 *        ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_header &fits_header::insert_records( const char *keyword0,
					  const fits_header &obj )
{
    return this->insert_records( this->index(keyword0), obj );
}

/* protected */

/**
 * @brief  ʣ���Υإå��쥳���ɤκ�� (���٥�)
 *
 * @note   ����ϡ�this->manager->setup_sys_header() ��ƤӽФ��ʤ���<br>
 *         ���Υ��饹���� erase_records() ��Ȥ��������˻Ȥ���<br>
 *         ���Υ��дؿ��� protected �Ǥ���
 */
bool fits_header::_erase_records( long index0, long num_records )
{
    long i, num_rm;
    long idx_to_rm;
    bool found_sys_record = false;

    /* ���������å� */
    if ( index0 < 0 || this->num_records_rec <= index0 ) {
	goto quit;
    }
    if ( this->num_records_rec < index0 + num_records ) {
	num_records = this->num_records_rec - index0;
    }
    if ( num_records < 0 ) {
	goto quit;
    }
    if ( num_records == 0 ) {
	goto quit;
    }

    num_rm = 0;
    idx_to_rm = index0;
    /* �ޤ�������줿���֤��� */
    for ( i=0 ; i < num_records ; i++ ) {
	//fprintf(stderr,"debug: idx=%d\n",i);
	if ( this->records_ptr_rec[idx_to_rm] == NULL ||
	     this->records_ptr_rec[idx_to_rm]->keyword_protected() == false ) {
	    long sz;
	    if ( this->records_ptr_rec[idx_to_rm] != NULL ) {
		/* index ���� */
		if (this->records_ptr_rec[idx_to_rm]->status() == FITS::NORMAL_RECORD){
		    int s;
		    //fprintf(stderr,"debug: erase: [%s]\n",
		    //	    this->records_ptr_rec[idx_to_rm]->keyword());
		    s = this->index_rec.erase(
					   this->records_ptr_rec[idx_to_rm]->keyword(),
					   idx_to_rm + num_rm);
		    if ( s < 0 ) {
			/* ������realloc���顼��������� s=-1 �����뤬��
			   ����ǥå������Τ�����˺������Ƥ��� */
			err_throw(__FUNCTION__,"FATAL",
				  "this->index_rec.erase() failed");
		    }
		    /* sys record ���ɤ��������å� */
		    if ( is_header_reserved_key(
		       this->records_ptr_rec[idx_to_rm]->keyword()) == true ) {
			found_sys_record = true;
		    }
		}
		/* ���Τ��� */
		delete this->records_ptr_rec[idx_to_rm];
	    }
	    /* NULL ����ʬ��ޤ�ư�ư */
	    sz = this->num_records_rec - idx_to_rm - num_rm;
	    //fprintf(stderr,"debug: sz=%ld\n",sz);
	    this->records_rec.move( idx_to_rm + 1, sz, idx_to_rm, false );

	    num_rm ++;
	}
	else {
	    /* index ��ĥ��ʤ��� */
	    if ( this->records_ptr_rec[idx_to_rm]->status() == FITS::NORMAL_RECORD ) {
		int s;
		//fprintf(stderr,"debug: re-assigned A [%ld]\n",idx_to_rm);
		s = this->index_rec.update(this->records_ptr_rec[idx_to_rm]->keyword(),
					   idx_to_rm + num_rm, idx_to_rm);
		if ( s < 0 ) {
		    err_throw(__FUNCTION__,"FATAL",
			    "Internal ERROR: this->index_rec.update() failed");
		}
	    }
	    err_report1(__FUNCTION__,"WARNING","keyword='%s' is protected; "
			"cannot erase", this->records_ptr_rec[idx_to_rm]->keyword());
	    idx_to_rm ++;
	}
    }
    this->num_records_rec -= num_rm;

    /* index ��ĥ��ʤ��� */
    for ( i=idx_to_rm ; i < this->num_records_rec ; i++ ) {
	if ( this->records_ptr_rec[i] != NULL && 
	     this->records_ptr_rec[i]->status() == FITS::NORMAL_RECORD ) {
	    int s;
	    //fprintf(stderr,"debug: re-assigned B [%ld]\n",i);
	    s = this->index_rec.update( this->records_ptr_rec[i]->keyword(),
					i + num_rm, i );
	    if ( s < 0 ) {
	    	err_throw(__FUNCTION__,"FATAL",
			  "Internal ERROR: this->index_rec.update() failed");
	    }
	}
    }

    /* ���ɥ쥹�ơ��֥�򶹤�� */
    try {
	this->records_rec.resize( this->num_records_rec + 1 );
    }
    catch (...) {
	err_throw(__FUNCTION__,"FATAL","records_rec.resize() failed");
    }

 quit:
    return found_sys_record;
}

/**
 * @brief  ʣ���Υإå��쥳���ɤκ��
 *
 * @note  ���Ȥ� fits_hdu ���֥������Ȥδ������ˤ����硤Data Unit �˴ط�����
 *        ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_header &fits_header::erase_records( long index0, long num_records )
{
    bool found_sys_record;

    found_sys_record = this->fits_header::_erase_records(index0, num_records);

    /* sys record ���õ�줿��� */
    if ( found_sys_record == true ) {
	/* �õ��Ƥ� Data Unit �˴�Ϣ����쥳���ɤϤ��������褵���� */
	if ( this->sysrecords_prohibition_rec == true &&
	     this->manager != NULL ) {
	    /* this will call an overridden function */
	    //err_report(__FUNCTION__,"DEBUG","called ...... from [F]");
	    this->manager->setup_sys_header();
	}
    }

    return *this;
}

/**
 * @brief  ʣ���Υإå��쥳���ɤκ��
 *
 * @note  ���Ȥ� fits_hdu ���֥������Ȥδ������ˤ����硤Data Unit �˴ط�����
 *        ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_header &fits_header::erase_records( const char *keyword0, long num_records )
{
    return this->erase_records( this->index(keyword0), num_records );
}

//fits_header &fits_header::erase_all_sysrecords()
//{
//    long i;
//    for ( i = this->length() ; 0 < i ; ) {
//	i--;
//	if ( this->is_sysrecord(i) == true ) this->erase(i);
//    }
//    return *this;
//}

/*
 */

/* 1�ԤǤ��ɲäȤ� */

/**
 * @brief  1�ĤΥإå��쥳���ɤ��ɲá�����
 *
 *  ������ɤ�¸�ߤ��ʤ������ɲä���Ʊ��Υ�����ɤ�¸�ߤ������
 *  ���ꤵ�줿���ƤǾ�񤭤��ޤ���
 *
 * @note  ���Ȥ� fits_hdu ���֥������Ȥδ������ˤ����硤Data Unit �˴ط�����
 *        ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_header &fits_header::update( const char *keyword, const char *value, 
				  const char *comment )
{
    fits::header_def defs[] = { {NULL,NULL,NULL}, {NULL} };
    long idx;

    if ( keyword == NULL ) return this->append_records(defs);

    defs[0].keyword = keyword;
    defs[0].value   = value;
    defs[0].comment = comment;

    /* description �Ǥʤ� index ��õ�� */
    idx = this->index(keyword);

    if ( idx < 0 ) {
	if ( value == NULL ) defs[0].value = "";
	if ( comment == NULL ) defs[0].comment = "";
	return this->append_records(defs);
    }
    else {
	this->assign(idx,defs[0]);
	return *this;
    }
}

/**
 * @brief  1�ĤΥإå��쥳���ɤ��ɲ�,����(fits_header_record���֥������Ȥǻ���)
 *
 *  ������ɤ�¸�ߤ��ʤ������ɲä���Ʊ��Υ�����ɤ�¸�ߤ������
 *  ���ꤵ�줿���ƤǾ�񤭤��ޤ���
 *
 * @note  ���Ȥ� fits_hdu ���֥������Ȥδ������ˤ����硤Data Unit �˴ط�����
 *        ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_header &fits_header::update( const fits_header_record &obj )
{
    if ( obj.status() == FITS::NORMAL_RECORD ) {
	long idx;
	/* description �Ǥʤ� index ��õ�� */
	idx = this->index(obj.keyword());
	if ( idx < 0 ) {
	    this->append(obj);
	}
	else {
	    this->assign(idx, obj);
	}
    }
    return *this;
}

/**
 * @brief  1�ĤΥإå��쥳���ɤ��ɲ� (������ɤΤ߻���)
 *
 * @note  ���Ȥ� fits_hdu ���֥������Ȥδ������ˤ����硤Data Unit �˴ط�����
 *        ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_header &fits_header::append( const char *keyword )
{
    if ( keyword != NULL ) {
	fits_header_record rec;
        rec.assign(keyword,"","").assign_default_comment(this->hdutype());
	this->append(rec.raw_record());
    }
    return *this;
}

/**
 * @brief  1�ĤΥإå��쥳���ɤ��ɲ�
 *
 * @note  ���Ȥ� fits_hdu ���֥������Ȥδ������ˤ����硤Data Unit �˴ط�����
 *        ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_header &fits_header::append( const char *keyword, const char *value, 
				  const char *comment )
{
    if ( keyword != NULL ) {
	fits_header_record rec;
	bool flg = false;
	if ( value == NULL ) value = "";
	if ( comment == NULL ) {
	    comment = "";
	    flg = true;
	}
        rec.assign(keyword,value,comment);
	if ( flg == true ) rec.assign_default_comment(this->hdutype());
	this->append(rec.raw_record());
    }
    return *this;
}

/**
 * @brief  1�Ĥε��Ҽ��إå��쥳����(HISTORY��COMMENT)���ɲ�
 *
 * @note  ���Ȥ� fits_hdu ���֥������Ȥδ������ˤ����硤Data Unit �˴ط�����
 *        ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_header &fits_header::append(const char *keyword, const char *description)
{
    fits::header_def defs[] = { {NULL,NULL,NULL}, {NULL} };
    defs[0].keyword = keyword;
    defs[0].value   = description;
    return this->append_records(defs);
}

/**
 * @brief  1�ĤΥإå��쥳���ɤ��ɲ� (fits::header_def ��¤�Τǻ���)
 *
 * @note  ���Ȥ� fits_hdu ���֥������Ȥδ������ˤ����硤Data Unit �˴ط�����
 *        ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_header &fits_header::append( const fits::header_def &def )
{
    fits::header_def defs[] = { {NULL,NULL,NULL}, {NULL} };
    defs[0] = def;
    return this->append_records(defs);
}

/**
 * @brief  1�ĤΥإå��쥳���ɤ��ɲ� (������ɤΤ߻���)
 *
 * @note  ���Ȥ� fits_hdu ���֥������Ȥδ������ˤ����硤Data Unit �˴ط�����
 *        ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_header &fits_header::append( const fits_header_record &obj )
{
    const fits::header_def &def = obj.raw_record();
    const char *formatted = obj.a_formatted_rec.cstr();
    /* use private member function */
    return this->append_records(&def, &formatted, 1);
}

/**
 * @brief  1�ĤΥإå��쥳���ɤ����� (������ɤΤ߻���)
 *
 * @note  ���Ȥ� fits_hdu ���֥������Ȥδ������ˤ����硤Data Unit �˴ط�����
 *        ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_header &fits_header::insert( long index0, const char *keyword )
{
    if ( keyword != NULL ) {
	fits_header_record rec;
        rec.assign(keyword,"","").assign_default_comment(this->hdutype());
	this->insert(index0, rec);
    }
    return *this;
}

/**
 * @brief  1�ĤΥإå��쥳���ɤ����� (������ɤΤ߻���)
 *
 * @note  ���Ȥ� fits_hdu ���֥������Ȥδ������ˤ����硤Data Unit �˴ط�����
 *        ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_header &fits_header::insert( const char *keyword0, const char *keyword )
{
    if ( keyword != NULL ) {
	fits_header_record rec;
        rec.assign(keyword,"","").assign_default_comment(this->hdutype());
	this->insert(keyword0, rec);
    }
    return *this;
}

/**
 * @brief  1�ĤΥإå��쥳���ɤ�����
 *
 * @note  ���Ȥ� fits_hdu ���֥������Ȥδ������ˤ����硤Data Unit �˴ط�����
 *        ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_header &fits_header::insert( long index0,
				  const char *k, const char *v, const char *c )
{
    if ( k != NULL ) {
	fits_header_record rec;
	bool flg = false;
	if ( v == NULL ) v = "";
	if ( c == NULL ) {
	    c = "";
	    flg = true;
	}
        rec.assign(k, v, c);
	if ( flg == true ) rec.assign_default_comment(this->hdutype());
	this->insert(index0, rec.raw_record());
    }
    return *this;
}

/**
 * @brief  1�ĤΥإå��쥳���ɤ�����
 *
 * @note  ���Ȥ� fits_hdu ���֥������Ȥδ������ˤ����硤Data Unit �˴ط�����
 *        ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_header &fits_header::insert( const char *keyword0,
				  const char *k, const char *v, const char *c )
{
    if ( k != NULL ) {
	fits_header_record rec;
	bool flg = false;
	if ( v == NULL ) v = "";
	if ( c == NULL ) {
	    c = "";
	    flg = true;
	}
        rec.assign(k, v, c);
	if ( flg == true ) rec.assign_default_comment(this->hdutype());
	this->insert(keyword0, rec.raw_record());
    }
    return *this;
}

/**
 * @brief  1�Ĥε��Ҽ��إå��쥳����(HISTORY��COMMENT)������
 *
 * @note  ���Ȥ� fits_hdu ���֥������Ȥδ������ˤ����硤Data Unit �˴ط�����
 *        ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_header &fits_header::insert( long index0, 
				  const char *keywd, const char *description )
{
    fits::header_def defs[] = { {NULL,NULL,NULL}, {NULL} };
    defs[0].keyword = keywd;
    defs[0].value   = description;
    return this->insert_records(index0,defs);
}

/**
 * @brief  1�Ĥε��Ҽ��إå��쥳����(HISTORY��COMMENT)������
 *
 * @note  ���Ȥ� fits_hdu ���֥������Ȥδ������ˤ����硤Data Unit �˴ط�����
 *        ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_header &fits_header::insert( const char *keyword0, 
				  const char *keywd, const char *description )
{
    fits::header_def defs[] = { {NULL,NULL,NULL}, {NULL} };
    defs[0].keyword = keywd;
    defs[0].value   = description;
    return this->insert_records(keyword0,defs);
}

/**
 * @brief  1�ĤΥإå��쥳���ɤ����� (fits::header_def ��¤�Τǻ���)
 *
 * @note  ���Ȥ� fits_hdu ���֥������Ȥδ������ˤ����硤Data Unit �˴ط�����
 *        ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_header &fits_header::insert( long index0, const fits::header_def &def )
{
    fits::header_def defs[] = { {NULL,NULL,NULL}, {NULL} };
    defs[0] = def;
    return this->insert_records(index0,defs);
}

/**
 * @brief  1�ĤΥإå��쥳���ɤ����� (fits::header_def ��¤�Τǻ���)
 *
 * @note  ���Ȥ� fits_hdu ���֥������Ȥδ������ˤ����硤Data Unit �˴ط�����
 *        ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_header &fits_header::insert( const char *keyword0, 
				  const fits::header_def &def )
{
    fits::header_def defs[] = { {NULL,NULL,NULL}, {NULL} };
    defs[0] = def;
    return this->insert_records(keyword0,defs);
}

/**
 * @brief  1�ĤΥإå��쥳���ɤ����� (fits_header_record ���֥������Ȥǻ���)
 *
 * @note  ���Ȥ� fits_hdu ���֥������Ȥδ������ˤ����硤Data Unit �˴ط�����
 *        ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_header &fits_header::insert( long index0, const fits_header_record &obj )
{
    const fits::header_def &def = obj.raw_record();
    const char *formatted = obj.a_formatted_rec.cstr();
    /* use private member function */
    return this->insert_records(index0, &def, &formatted, 1);
}

/**
 * @brief  1�ĤΥإå��쥳���ɤ����� (fits_header_record ���֥������Ȥǻ���)
 *
 * @note  ���Ȥ� fits_hdu ���֥������Ȥδ������ˤ����硤Data Unit �˴ط�����
 *        ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_header &fits_header::insert( const char *keyword0,
				  const fits_header_record &obj )
{
    const fits::header_def &def = obj.raw_record();
    const char *formatted = obj.a_formatted_rec.cstr();
    /* use private member function */
    return this->insert_records(this->index(keyword0), &def, &formatted, 1);
}

/**
 * @brief  1�ĤΥإå��������̾���ѹ�
 *
 * @note  ���Ȥ� fits_hdu ���֥������Ȥδ������ˤ����硤Data Unit �˴ط�����
 *        ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_header &fits_header::rename( long index0, const char *new_name )
{
    fits::header_def def = {new_name,NULL,NULL};

    /* check args */
    if ( index0 < 0 || this->num_records_rec <= index0 ) goto quit;
    if ( new_name == NULL ) goto quit;

    /* rename */
    this->assign(index0, def);

 quit:
    return *this;
}

/**
 * @brief  1�ĤΥإå��������̾���ѹ�
 *
 * @note  ���Ȥ� fits_hdu ���֥������Ȥδ������ˤ����硤Data Unit �˴ط�����
 *        ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_header &fits_header::rename( const char *keyword0, const char *new_name )
{
    return this->rename( this->index(keyword0), new_name );
}

/**
 * @brief  1�ĤΥإå��쥳���ɤξõ�
 *
 * @note  ���Ȥ� fits_hdu ���֥������Ȥδ������ˤ����硤Data Unit �˴ط�����
 *        ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_header &fits_header::erase( long index0 )
{
    this->erase_records(index0,1);
    return *this;
}

/**
 * @brief  1�ĤΥإå��쥳���ɤξõ�
 *
 * @note  ���Ȥ� fits_hdu ���֥������Ȥδ������ˤ����硤Data Unit �˴ط�����
 *        ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_header &fits_header::erase( const char *keyword0 )
{
    this->erase_records(keyword0,1);
    return *this;
}

/* 
 * wrappers for low-level member functions
 */

/**
 * @brief  1�ĤΥإå��쥳���ɤ򹹿� (fits::header_def ��¤�Τǻ���)
 *
 * @note  ���Ȥ� fits_hdu ���֥������Ȥδ������ˤ����硤Data Unit �˴ط�����
 *        ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_header &fits_header::assign( long index0, const fits::header_def &def )
{
    //err_report(__FUNCTION__,"DEBUG","called!!");
    int s;
    tstring old_key;
    fits_header_record tmp_hrec;
    fits::header_def tmp_def = {NULL,def.value,def.comment};

    if ( index0 < 0 || this->num_records_rec <= index0 ) {
	err_report(__FUNCTION__,"WARNING","invalid index0");
	goto quit;
    }
    if ( this->records_ptr_rec[index0] == NULL ) goto quit;

    //fprintf(stderr,"debug : def.comment = [%s]\n",def.comment);

    /* ������ɤ˶���Ϳ����졤�����ͤ������Ȥ˶���Ϳ����줿���� */
    /* �쥳���ɤν������Ԥʤ� */
    if ( (def.keyword != NULL && def.keyword[0]=='\0') &&
	 ( (def.value != NULL && def.value[0]=='\0') ||
	   (def.comment != NULL && def.comment[0]=='\0') ) ) {
	/* description �Ǥʤ���硤keyword ����¸���Ƥ��� */
	if ( this->records_ptr_rec[index0]->status() == FITS::NORMAL_RECORD ) {
	    old_key = this->records_ptr_rec[index0]->keyword();
	}
	/* ��������ߤ� */
	this->records_ptr_rec[index0]->assign_any(def);
	/* index */
	if ( old_key.cstr() != NULL ) {	/* description �Ǥʤ���� */
	    /* ������ɤ��ѹ������ä�������Ͽ���� */
	    /* (keyword protected �ʾ��ϡ��ѹ��Ǥ��ʤ�) */
	    if ( this->records_ptr_rec[index0]->status() != FITS::NORMAL_RECORD ||
		 old_key.strcmp(this->records_ptr_rec[index0]->keyword()) != 0 ) {
		s = this->index_rec.erase(old_key.cstr(),index0);
		if ( s < 0 ) {
		    err_throw(__FUNCTION__,"FATAL",
			      "this->index_rec.erase() failed");
		}
	    }
	}
	goto quit;
    }

    /* �����ƥ�إå����ػߤ���Ƥ����硤�ٹ��Ф��ƥ��顼�Ȥ��� */
    if ( this->sysrecords_prohibition_rec == true ) {
	if ( def.keyword != NULL && 
	     is_header_reserved_key(def.keyword) == true ) {
	    err_report1(__FUNCTION__,"WARNING",
			"keyword '%s' cannot be assigned by the user",
			def.keyword);
	    goto quit;
	}
    }

    /* ��ö���ƥ�ݥ��˽񤭽Ф��Ƥߤ�(������ɤ�ʸ�����ִ��Τ���) */
    if ( def.keyword != NULL ) {
	tmp_hrec.assign_any(def);
	tmp_def.keyword = tmp_hrec.keyword();
    }

    /* �����Υ쥳���ɤ� description �Ǥʤ���� */
    if ( this->records_ptr_rec[index0]->status() == FITS::NORMAL_RECORD ) {
	/* keyword ����¸���Ƥ��� */
        old_key = this->records_ptr_rec[index0]->keyword();
	/* ��Ͽ�Ѥߤ� keyword ������褦�Ȥ�����硤�ٹ𤹤� */
	if ( tmp_def.keyword != NULL && 0 <= this->index(tmp_def.keyword) ) {
	    if ( this->index(tmp_def.keyword) != index0 ) {
		if ( this->suppress_dupkey_warning_rec == false ) {
		    err_report1(__FUNCTION__,"WARNING",
			"duplicated keyword '%s' is appended",tmp_def.keyword);
		}
	    }
	}	
    }
    /* ��Ͽ���� */
    try {
	this->records_ptr_rec[index0]->assign_any(def);
    }
    catch (...) {
	this->fits_header::_erase_records(index0,1);
	err_throw(__FUNCTION__,"FATAL",
		  "this->records_ptr_rec[index0]->assign_any(def) failed");
    }
    
    if ( old_key.cstr() == NULL ) {	/* �����ξ�� */
	if ( this->records_ptr_rec[index0]->status() == FITS::NORMAL_RECORD ) {
	    s= this->index_rec.append(this->records_ptr_rec[index0]->keyword(),index0);
	    if ( s < 0 ) {
		this->fits_header::_erase_records(index0,1);
		err_throw(__FUNCTION__,"FATAL",
			  "this->index_rec.append() failed");
	    }
	}
    }
    else {				/* �������Τ� description �Ǥʤ���� */
	/* description �ˤʤä���硤������ɤ��ѹ������ä����Ͼõ�� */
	if ( this->records_ptr_rec[index0]->status() != FITS::NORMAL_RECORD ||
	     old_key.strcmp(this->records_ptr_rec[index0]->keyword()) != 0) {
	    s= this->index_rec.erase(old_key.cstr(),index0);
	    if ( s < 0 ) {
		err_throw(__FUNCTION__,"FATAL","this->index_rec.erase() failed");
	    }
	}
	/* description �ǤϤʤ���������ɤ��ѹ������ä����Ͼõ�� */
	if ( this->records_ptr_rec[index0]->status() == FITS::NORMAL_RECORD &&
	     old_key.strcmp(this->records_ptr_rec[index0]->keyword()) != 0) {
	    s= this->index_rec.append(this->records_ptr_rec[index0]->keyword(),index0);
	    if ( s < 0 ) {
		this->fits_header::_erase_records(index0,1);
		err_throw(__FUNCTION__,"FATAL",
			  "this->index_rec.append() failed");
	    }
	}
    }

 quit:
    return *this;
}

/**
 * @brief  1�ĤΥإå��쥳���ɤ򹹿� (fits::header_def ��¤�Τǻ���)
 *
 * @note  ���Ȥ� fits_hdu ���֥������Ȥδ������ˤ����硤Data Unit �˴ط�����
 *        ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_header &fits_header::assign( const char *keyword0, const fits::header_def &def )
{
    return this->assign( this->index(keyword0), def );
}

/**
 * @brief  1�ĤΥإå��쥳���ɤ򹹿� (fits_header_record ���֥������Ȥǻ���)
 *
 * @note  ���Ȥ� fits_hdu ���֥������Ȥδ������ˤ����硤Data Unit �˴ط�����
 *        ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_header &fits_header::assign( long index0, const fits_header_record &obj )
{
    fits::header_def tmp_def = {"","",""};

    if ( index0 < 0 || this->num_records_rec <= index0 ) {
	err_report(__FUNCTION__,"WARNING","invalid index0");
	return *this;
    }

    /* obj�����Ǥ˴������ˤʤ��������å� */
    fits_header_record &dest = this->at(index0);
    if ( &dest == &obj ) return *this;

    this->assign( index0, tmp_def );	/* �õ� */

    if ( this->records_ptr_rec[index0]->status() == FITS::NULL_RECORD ||
	 this->records_ptr_rec[index0]->status() == obj.status() ) {
	this->assign(index0, obj.raw_record());
	this->at(index0).a_formatted_rec = obj.a_formatted_rec;
    }

    return *this;
}

/**
 * @brief  1�ĤΥإå��쥳���ɤ򹹿� (fits_header_record ���֥������Ȥǻ���)
 *
 * @note  ���Ȥ� fits_hdu ���֥������Ȥδ������ˤ����硤Data Unit �˴ط�����
 *        ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_header &fits_header::assign( const char *keyword0,
				  const fits_header_record &obj )
{
    return this->assign( this->index(keyword0), obj );
}

/**
 * @brief  1�ĤΥإå��쥳���ɤ򹹿�
 *
 * @note  ���Ȥ� fits_hdu ���֥������Ȥδ������ˤ����硤Data Unit �˴ط�����
 *        ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_header &fits_header::assign( long index0, 
				  const char *keyword, const char *value, 
				  const char *comment )
{
    if ( index0 < 0 || this->num_records_rec <= index0 ) {
	err_report(__FUNCTION__,"WARNING","invalid index0");
	return *this;
    }

    if ( this->records_ptr_rec[index0]->status() == FITS::NORMAL_RECORD ) {
	fits::header_def def = {keyword,value,comment};
	this->assign( index0, def );
    }
    else if ( keyword != NULL && keyword[0] != '\0' ) {
	fits::header_def tmp_def = {"","",""};
	this->assign( index0, tmp_def );	/* �õ� */
	if ( value   == NULL ) value   = "";
	if ( comment == NULL ) comment = "";
	if ( this->records_ptr_rec[index0]->status() == FITS::NULL_RECORD ) {
	    fits::header_def def = {keyword,value,comment};
	    this->assign( index0, def );
	}
    }
    else if ( keyword != NULL && keyword[0] == '\0' &&
	      ( (value != NULL && value[0] == '\0') ||
		(comment != NULL && comment[0] == '\0') ) ) {
	fits::header_def tmp_def = {"","",""};
	this->assign( index0, tmp_def );	/* �õ� */
    }

    return *this;
}

/**
 * @brief  1�ĤΥإå��쥳���ɤ򹹿�
 *
 * @note  ���Ȥ� fits_hdu ���֥������Ȥδ������ˤ����硤Data Unit �˴ط�����
 *        ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_header &fits_header::assign( const char *keyword0, 
				  const char *keyword, const char *value, 
				  const char *comment )
{
    return this->assign( this->index(keyword0), keyword, value, comment );
}

/**
 * @brief  1�ĤΥإå��쥳���ɤ򵭽Ҽ��쥳���ɤǹ���
 *
 * @note  ���Ȥ� fits_hdu ���֥������Ȥδ������ˤ����硤Data Unit �˴ط�����
 *        ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_header &fits_header::assign( long index0, 
				const char *keyword, const char *description )
{
    if ( index0 < 0 || this->num_records_rec <= index0 ) {
	err_report(__FUNCTION__,"WARNING","invalid index0");
	return *this;
    }

    if ( this->records_ptr_rec[index0]->status() == FITS::DESCRIPTION_RECORD ) {
	fits::header_def def = {keyword,description,NULL};
	this->assign( index0, def );
    }
    else if ( (keyword != NULL && keyword[0] != '\0') ||
	      (description != NULL && description[0] != '\0') ) {
	fits::header_def tmp_def = {"","",""};
	this->assign( index0, tmp_def );	/* �õ� */
	if ( keyword == NULL ) keyword = "";
	if ( description == NULL ) description = "";
	if ( this->records_ptr_rec[index0]->status() == FITS::NULL_RECORD ) {
	    fits::header_def def = {keyword,description,NULL};
	    this->assign( index0, def );
	}
    }
    else if ( keyword != NULL && keyword[0] == '\0' &&
	      description != NULL && description[0] == '\0' ) {
	fits::header_def tmp_def = {"","",""};
	this->assign( index0, tmp_def );	/* �õ� */
    }

    return *this;
}

/**
 * @brief  1�ĤΥإå��쥳���ɤ򵭽Ҽ��쥳���ɤǹ���
 *
 * @note  ���Ȥ� fits_hdu ���֥������Ȥδ������ˤ����硤Data Unit �˴ط�����
 *        ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_header &fits_header::assign( const char *keyword0, 
				const char *keyword, const char *description )
{
    return this->assign( this->index(keyword0), keyword, description );
}

/* value */

/**
 * @brief  1�ĤΥإå��쥳���ɤ��ͤ򹹿� (���٥롦printf()�ε�ˡ)
 *
 * @note  ���Ȥ� fits_hdu ���֥������Ȥδ������ˤ����硤Data Unit �˴ط�����
 *        ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_header &fits_header::vassignf_value( long index0, const char *format,
					  va_list ap )
{
    if ( index0 < 0 || this->num_records_rec <= index0 ) {
	err_report(__FUNCTION__,"WARNING","invalid index0");
    }
    else this->records_ptr_rec[index0]->vassignf_value(format,ap);
    return *this;
}

/**
 * @brief  1�ĤΥإå��쥳���ɤ��ͤ򹹿� (���٥롦printf()�ε�ˡ)
 *
 * @note  ���Ȥ� fits_hdu ���֥������Ȥδ������ˤ����硤Data Unit �˴ط�����
 *        ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_header &fits_header::vassignf_value( const char *keyword0, 
					  const char *format, va_list ap )
{
    return this->vassignf_value( this->index(keyword0), format, ap );
}

/**
 * @brief  1�ĤΥإå��쥳���ɤ��ͤ򹹿� (���٥롦printf()�ε�ˡ)
 *
 * @note  ���Ȥ� fits_hdu ���֥������Ȥδ������ˤ����硤Data Unit �˴ط�����
 *        ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_header &fits_header::assignf_value( long index0, const char *format, ... )
{
    if ( index0 < 0 || this->num_records_rec <= index0 ) {
	err_report(__FUNCTION__,"WARNING","invalid index0");
    }
    else {
	va_list ap;
	va_start(ap,format);
	try {
	    this->vassignf_value(index0,format,ap);
	}
	catch (...) {
	    va_end(ap);
	    err_throw(__FUNCTION__,"FATAL","this->vassignf_value() failed");
	}
	va_end(ap);
    }
    return *this;
}

/**
 * @brief  1�ĤΥإå��쥳���ɤ��ͤ򹹿� (���٥롦printf()�ε�ˡ)
 *
 * @note  ���Ȥ� fits_hdu ���֥������Ȥδ������ˤ����硤Data Unit �˴ط�����
 *        ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_header &fits_header::assignf_value( const char *keyword0, 
					 const char *format, ... )
{
    va_list ap;
    va_start(ap,format);
    try {
	this->vassignf_value(keyword0,format,ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","this->vassignf_value() failed");
    }
    va_end(ap);
    return *this;
}

/* comment */

/**
 * @brief  1�ĤΥإå��쥳���ɤΥ����Ȥ򹹿� (printf()�ε�ˡ)
 *
 * @note  ���Ȥ� fits_hdu ���֥������Ȥδ������ˤ����硤Data Unit �˴ط�����
 *        ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_header &fits_header::vassignf_comment( long index0, const char *format, 
					    va_list ap )
{
    tstring tstr;
    fits::header_def rv = {NULL,NULL,NULL};

    if ( index0 < 0 || this->num_records_rec <= index0 ) {
	err_report(__FUNCTION__,"WARNING","invalid index0");
	goto quit;
    }

    if ( format == NULL ) {
	this->assign(index0,rv);
	goto quit;
    }

    tstr.vassignf(format,ap);

    rv.comment = tstr.cstr();
    this->assign(index0,rv);

 quit:
    return *this;
}

/**
 * @brief  1�ĤΥإå��쥳���ɤΥ����Ȥ򹹿� (printf()�ε�ˡ)
 *
 * @note  ���Ȥ� fits_hdu ���֥������Ȥδ������ˤ����硤Data Unit �˴ط�����
 *        ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_header &fits_header::vassignf_comment( const char *keyword0, 
					    const char *format, va_list ap )
{
    return this->vassignf_comment( this->index(keyword0), format, ap );
}

/**
 * @brief  1�ĤΥإå��쥳���ɤΥ����Ȥ򹹿� (printf()�ε�ˡ)
 *
 * @note  ���Ȥ� fits_hdu ���֥������Ȥδ������ˤ����硤Data Unit �˴ط�����
 *        ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_header &fits_header::assignf_comment( long index0, const char *format, ... )
{
    if ( index0 < 0 || this->num_records_rec <= index0 ) {
	err_report(__FUNCTION__,"WARNING","invalid index0");
	return *this;
    }
    else {
	va_list ap;
	va_start(ap,format);
	try {
	    this->vassignf_comment(index0,format,ap);
	}
	catch (...) {
	    va_end(ap);
	    err_throw(__FUNCTION__,"FATAL","this->vassignf_comment()");
	}
	va_end(ap);
	return *this;
    }
}

/**
 * @brief  1�ĤΥإå��쥳���ɤΥ����Ȥ򹹿� (printf()�ε�ˡ)
 *
 * @note  ���Ȥ� fits_hdu ���֥������Ȥδ������ˤ����硤Data Unit �˴ط�����
 *        ͽ�󥭡����(BITPIX ��)�ϻ���Ǥ��ޤ���
 */
fits_header &fits_header::assignf_comment( const char *keyword0, 
					   const char *format, ... )
{
    va_list ap;
    va_start(ap,format);
    try {
	this->vassignf_comment(keyword0,format,ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","this->vassignf_comment()");
    }
    va_end(ap);
    return *this;
}

/*
 */

/* Overwrite all header comments with SFITSIO built-in comment strings  */
/**
 * @brief  �����Ȥ�¸�ߤˤ�����餺�����ߤΥإå������ȼ�������Ƥ�����
 *
 *  ���ƤΥإå��쥳���ɤΤ��������ߤΥ����ȼ���ȥ�����ɤ����פ����硤
 *  �����Υ쥳���ɤΥ����Ȥ򸽺ߤΥ����ȼ��񤬻��ĥ�����ʸ�Ǿ�񤭤�
 *  �ޤ���
 */
fits_header &fits_header::assign_default_comments( int hdutype )
{
    long i;
    for ( i=0 ; i < this->length() ; i++ ) {
	if ( this->records_ptr_rec[i]->status() == FITS::NORMAL_RECORD ) {
	    this->records_ptr_rec[i]->assign_default_comment(hdutype);
	}
    }
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
fits_header &fits_header::fill_blank_comments( int hdutype )
{
    long i;
    for ( i=0 ; i < this->length() ; i++ ) {
	if ( this->records_ptr_rec[i]->status() == FITS::NORMAL_RECORD &&
	     this->records_ptr_rec[i]->comment_length() == 0 ) {
	    this->records_ptr_rec[i]->assign_default_comment(hdutype);
	}
    }
    return *this;
}

/*
 */

/**
 * @brief  fits_header_record ���֥������Ȥؤλ��Ȥ����
 *
 * @note   fits_header::at() �Ȥΰ㤤�Ϥ���ޤ���
 */
fits_header_record &fits_header::record( long index0 )
{
    if ( index0 < 0 || this->num_records_rec <= index0 ) {
	err_throw(__FUNCTION__,"ERROR","Invalid record index");
    }
    return *(this->records_ptr_rec[index0]);
}

/**
 * @brief  fits_header_record ���֥������Ȥؤλ��Ȥ����
 *
 * @note   fits_header::at() �Ȥΰ㤤�Ϥ���ޤ���
 */
fits_header_record &fits_header::record( const char *keyword0 )
{
    long index0 = this->index(keyword0);
#if 1
    if ( index0 < 0 ) {
	this->append(keyword0);
	index0 = this->index(keyword0);
    }
#endif
    if ( index0 < 0 ) {
	err_throw1(__FUNCTION__,"ERROR","keyword '%s' is not found",keyword0);
    }
    return this->record( index0 );
}

#ifdef SLI__OVERLOAD_CONST_AT
/**
 * @brief  fits_header_record ���֥������Ȥؤλ��Ȥ���� (�ɼ�����)
 *
 * @note   fits_header::at() �Ȥΰ㤤�Ϥ���ޤ���
 */
const fits_header_record &fits_header::record( long index0 ) const
{
    return this->record_cs(index0);
}

/**
 * @brief  fits_header_record ���֥������Ȥؤλ��Ȥ���� (�ɼ�����)
 *
 * @note   fits_header::at() �Ȥΰ㤤�Ϥ���ޤ���
 */
const fits_header_record &fits_header::record( const char *keyword0 ) const
{
    return this->record_cs(keyword0);
}
#endif

/**
 * @brief  fits_header_record ���֥������Ȥؤλ��Ȥ���� (�ɼ�����)
 *
 * @note   fits_header::at_cs() �Ȥΰ㤤�Ϥ���ޤ���
 */
const fits_header_record &fits_header::record_cs( long index0 ) const
{
    if ( index0 < 0 || this->num_records_rec <= index0 ) {
	err_throw(__FUNCTION__,"ERROR","Invalid record index");
    }
    return *(this->records_ptr_rec[index0]);
}

/**
 * @brief  fits_header_record ���֥������Ȥؤλ��Ȥ���� (�ɼ�����)
 *
 * @note   fits_header::at_cs() �Ȥΰ㤤�Ϥ���ޤ���
 */
const fits_header_record &fits_header::record_cs( const char *keyword0 ) const
{
    long idx = this->index(keyword0);
    if ( idx < 0 ) {
	err_throw1(__FUNCTION__,"ERROR","keyword '%s' is not found",keyword0);
    }
    return this->record_cs( idx );
}

/* same as fits_header::record() */

/**
 * @brief  fits_header_record ���֥������Ȥؤλ��Ȥ����
 *
 * @note   fits_header::record() �Ȥΰ㤤�Ϥ���ޤ���
 */
fits_header_record &fits_header::at( long index0 )
{
    return this->record(index0);
}

/**
 * @brief  fits_header_record ���֥������Ȥؤλ��Ȥ����
 *
 * @note   fits_header::record() �Ȥΰ㤤�Ϥ���ޤ���
 */
fits_header_record &fits_header::at( const char *keyword0 )
{
    return this->record(keyword0);
}

#ifdef SLI__OVERLOAD_CONST_AT
/**
 * @brief  fits_header_record ���֥������Ȥؤλ��Ȥ���� (�ɼ�����)
 *
 * @note   fits_header::record() �Ȥΰ㤤�Ϥ���ޤ���
 */
const fits_header_record &fits_header::at( long index0 ) const
{
    return this->record_cs(index0);
}

/**
 * @brief  fits_header_record ���֥������Ȥؤλ��Ȥ���� (�ɼ�����)
 *
 * @note   fits_header::record() �Ȥΰ㤤�Ϥ���ޤ���
 */
const fits_header_record &fits_header::at( const char *keyword0 ) const
{
    return this->record_cs(keyword0);
}
#endif

/**
 * @brief  fits_header_record ���֥������Ȥؤλ��Ȥ���� (�ɼ�����)
 *
 * @note   fits_header::record_cs() �Ȥΰ㤤�Ϥ���ޤ���
 */
const fits_header_record &fits_header::at_cs( long index0 ) const
{
    return this->record_cs(index0);
}

/**
 * @brief  fits_header_record ���֥������Ȥؤλ��Ȥ���� (�ɼ�����)
 *
 * @note   fits_header::record_cs() �Ȥΰ㤤�Ϥ���ޤ���
 */
const fits_header_record &fits_header::at_cs( const char *keyword0 ) const
{
    return this->record_cs(keyword0);
}


/*
 * �ɤ߼��
 */

/**
 * @brief  ������ɤ��б�����إå��쥳�����ֹ����� (���Ҽ��쥳���ɤϽ���)
 */
long fits_header::index( const char *keyword0 ) const
{
    return this->index(keyword0,false);
}

/**
 * @brief  ������ɤ��б�����إå��쥳�����ֹ�����
 */
long fits_header::index( const char *keyword0, bool is_description ) const
{
    long return_index = -1;
    long i;
    const char *str_beg;
    size_t j,len;
    char kwd[FITS::HEADER_KEYWORD_MAX_LENGTH + 1];

    if ( keyword0 == NULL ) goto quit;

    /* keyword0 ��ξ�����ɤΥ��ڡ�������� */
    rm_side_spaces(keyword0," ",&str_beg,&len);
    for ( j=0 ; j < FITS::HEADER_KEYWORD_MAX_LENGTH && j < len ; j++ ) {
	kwd[j] = str_beg[j];
    }
    kwd[j] = '\0';
	      
    if ( is_description == true ) {
	/* HISTORY �Ȥ� COMMENT ��ñ�������鸡��������� */
	for ( i=0 ; i < this->num_records_rec ; i++ ) {
	    if ( this->records_ptr_rec[i] != NULL &&
		 this->records_ptr_rec[i]->status() == FITS::DESCRIPTION_RECORD ) {
		if ( this->records_ptr_rec[i]->keyword_cs().strcmp(kwd) == 0 ) {
		    return_index = i;
		    break;
		}
	    }
	}
    }
    else {
	return_index = this->index_rec.index(kwd,0);
    }

 quit:
    return return_index;
}

/**
 * @brief  POSIX��ĥ����ɽ���ǥ�����ɤ򸡺�
 */
long fits_header::regmatch( const char *keypat, 
			    ssize_t *rpos, size_t *rlen ) const
{
    return this->regmatch(0L,keypat,rpos,rlen);
}

/**
 * @brief  POSIX��ĥ����ɽ���ǥ�����ɤ�Ϣ³Ū�˸���
 */
long fits_header::regmatch( long index0, const char *keypat, 
			    ssize_t *rpos, size_t *rlen ) const
{
    long i;
    for ( i=index0 ; i < this->length() ; i++ ) {
	ssize_t pos;
	size_t len;
	if ( this->record_cs(i).status() == FITS::NORMAL_RECORD ) {
	    pos = this->record_cs(i).arr_cs().at_cs(0L).regmatch(keypat,&len);
	    if ( 0 <= pos ) {
		if ( rpos != NULL ) *rpos = pos;
		if ( rlen != NULL ) *rlen = len;
		return i;
	    }
	}
    }
    return -1;
}

/* returns length of raw value. More than 0 means NON-NULL. */

/**
 * @brief  ������ɤ��б���������ʸ�����ͤ�Ĺ������� (¸�ߥ����å��˻��Ѳ�)
 *
 * @param   keyword �إå��������
 * @return  ������: ����ʸ������(��'�פ�ޤ�)��Ĺ�� <br>
 *          0: �ͤ�¸�ߤ��ʤ���� <br>
 *          �����: ������ɤ�¸�ߤ��ʤ����
 */
long fits_header::value_length( const char *keyword ) const
{
    long idx = this->index(keyword, false);
    if ( 0 <= idx ) return this->at_cs(idx).value_length();
    else return -1;
}

/**
 * @brief  ������ɤ��б���������ʸ�����ͤ�Ĺ������� (¸�ߥ����å��˻��Ѳ�)
 *
 * @param   index �쥳�����ֹ�
 * @return  ������: ����ʸ������(��'�פ�ޤ�)��Ĺ�� <br>
 *          0: �ͤ�¸�ߤ��ʤ���� <br>
 *          �����: ������ɤ�¸�ߤ��ʤ����
 */
long fits_header::value_length( long index ) const
{
    if ( 0 <= index && index < this->length() ) {
	const fits_header_record &h_rec = this->at_cs(index);
	if (h_rec.status() == FITS::NORMAL_RECORD) return h_rec.value_length();
	else return -1;
    }
    else return -1;
}

#if 0
long fits_header::num_records()
{
    return this->num_records_rec;
}
#endif

/**
 * @brief  �إå��쥳���ɤ�Ĺ�������
 */
long fits_header::length() const
{
    return this->num_records_rec;
}

/**
 * @brief  �إå��쥳���ɤ�Ĺ�������
 *
 * @note   fits_header::length() �Ȥΰ㤤�Ϥ���ޤ���
 */
long fits_header::size() const
{
    return this->num_records_rec;
}

/**
 * @brief  �إå��ξ��󤫤� HDU �Υ����פ�Ƚ�ꤷ���֤�
 */
int fits_header::hdutype() const
{
    int htype = FITS::ANY_HDU;

    if ( this->index("SIMPLE") == 0 ) {
	htype = FITS::IMAGE_HDU;
    }
    else if ( this->index("XTENSION") == 0 ) {
	const tstring &xtension = this->record(0L).svalue_cs();

	if ( xtension.strcmp("IMAGE") == 0 ) {
	    htype = FITS::IMAGE_HDU;
	}
	else if ( xtension.strcmp("BINTABLE") == 0 ) {
	    htype = FITS::BINARY_TABLE_HDU;
	}
	else if ( xtension.strcmp("TABLE") == 0 ) {
	    htype = FITS::ASCII_TABLE_HDU;
	}
    }

    return htype;
}

/**
 * @brief  Data Unit �˴ط�����ͽ�󥭡����(BITPIX ��)���ɤ������֤�
 */
bool fits_header::is_sysrecord( long index ) const
{
    if ( index < 0 || this->num_records_rec <= index ) return false;

    if ( this->record_cs(index).status() == FITS::NORMAL_RECORD ) {
	return is_header_reserved_key(this->record_cs(index).keyword());
    }
    else return false;
}

/**
 * @brief  CHECKSUM ��׻������֤�
 */
unsigned long fits_header::checksum( unsigned long sum_in )
{
    fitsio_csum csum_info;
    /* init */
    initialize_csum( &csum_info, sum_in );
    /* get chksum */
    this->header_save( NULL, (void *)&csum_info, true );

    return csum_info.sum;
}

/**
 * @brief  ���󥳡��ɤ��줿 CHECKSUM ��׻������֤�
 */
const char *fits_header::encoded_checksum( unsigned long sum_in )
{
    fitsio_csum csum_info;
    /* init */
    initialize_csum( &csum_info, sum_in );
    /* get chksum */
    this->header_save( NULL, (void *)&csum_info, true );
    encode_csum( csum_info, this->encoded_chksum_rec );

    return this->encoded_chksum_rec;
}

/**
 * @brief  ��ʣ������ɤηٹ�˴ؤ�������
 *
 *  ����� true �ˤ���ȡ���ʣ����������ɤ򥻥åȤ��Ƥ�ٹ��Ф��ʤ�
 *  �ʤ�ޤ���
 */
bool &fits_header::suppress_dupkey_warning()
{
    return this->suppress_dupkey_warning_rec;
}

/**
 * @brief  fits_header::expand_continue_records() �ǻ��� (������)
 *
 * @note    private �ʴؿ��Ǥ���
 */
static bool is_continue_record( const fits_header_record &ref )
{
    if ( ref.status() == FITS::DESCRIPTION_RECORD &&
	 ref.keyword_cs().strcmp("CONTINUE") == 0 &&
	 1 <= ref.value_cs().length() ) return true;
    else return false;
}

/**
 * @brief  CONTINUE �쥳���ɤ�Ÿ��
 *
 * @attention  �ۤȤ�����������Ѥǡ��ץ���ޤϻȤ�ɬ�פ�̵�����дؿ��Ǥ���
 * @note   fitscc�����Ȥ��롥
 */
fits_header &fits_header::expand_continue_records()
{
    fits_header *target_header = this;
    long j;
    //fprintf(stderr,"debug: expanding CONTINUE\n");

    /*
     * SFITSIO 'CONTINUE' convension:
     *
     * - Basic Convension:
     *
     *     FOO      = 'aaaaaaaaaaaa & '
     *     CONTINUE   'bbbbbbbbbbb&' / this is 
     *     CONTINUE   '' / a looooooooooooooooong
     *     CONTINUE   / comment.
     *
     *  - NOTE
     *
     *   Most easy way to generate header records with long string + long
     *   comment is that we reserve byte 79-80 for "&'" string in a 
     *   header record.  We show an example:
     *
     *                                    70        80
     *   12345678901234567890 ...         01234567890
     *   KEYWORD = '......... ... .................&'
     *   CONTINUE  '......... ... .................&'
     *   CONTINUE  '......... ... .................&'
     *   CONTINUE  / this is a comment.
     *
     *   Note that "''" should not be put on 78th column:
     *
     *   CONTINUE  '......... ... ................'&'       <= No good
     *   CONTINUE  ''........ ... .................&'       <= No good
     *
     */

    for ( j=1 ; j < target_header->length() ; j++ ) {

	if ( is_continue_record(target_header->record(j)) ) {

	    const long j0 = j - 1;			/* master record */
	    long n_erase;
	    bool prev_value_amp_exists = false;
	    bool prev_comment_amp_exists = false;
	    tstring long_value;
	    tstring long_comment;
	    const tstring &v0_ref = target_header->record(j0).value_cs();
	    /* ��¦�ζ���Ͻ����Ƥ��� */
	    const tstring &c0_ref = target_header->record(j0).comment_cs();
	    size_t v0_len = v0_ref.length();
	    size_t c0_len = c0_ref.length();

	    //err_report1(__FUNCTION__,"DEBUG","key = [%s]",
	    //	    target_header->record(j0).keyword());
	    //err_report1(__FUNCTION__,"DEBUG","val = [%s]", v0_ref.cstr());
	    //err_report1(__FUNCTION__,"DEBUG","com = [%s]", c0_ref.cstr());

	    /* �ޤ��ϡ�v0 �����(�Ǹ�� "'" ��Ĥ��ʤ�������) */
	    if ( target_header->record(j0).type() == FITS::STRING_T ) {
		size_t s_pos;
		/*
		 * ��
		 */
		/* 2ʸ���ʲ��ξ��ϡ�1ʸ���������Ѥ������ */
		if ( v0_len <= 2 ) {
		    long_value.append(v0_ref,0,1);
		}
		/* 3ʸ���ʾ�ξ��ϡ��Ǹ�� "'"," ","&" �ν�����ɬ�� */
		else {
		    size_t r_spn;
		    s_pos = v0_len - 1;
		    /* �Ǹ�� '&' ������å�(�����ǧ���٤��ǤϤʤ���) */
		    if ( v0_ref.cchr(s_pos) == '&' ) {
			s_pos --;
		    }
		    /* ���ڡ���������å� */
		    r_spn = v0_ref.strrspn(s_pos,' ');
		    s_pos -= r_spn;
		    /* �Ǹ�Υ������ơ�����������å� */
		    if ( v0_ref.cchr(s_pos) == '\'' ) {
			s_pos --;
		    }
		    /* �������¦�Υ��ڡ���������å� */
		    r_spn = v0_ref.strrspn(s_pos,' ');
		    s_pos -= r_spn;
		    /* �Ǹ�� & ������å� */
		    if ( v0_ref.cchr(s_pos) == '&' ) {
			prev_value_amp_exists = true;
			s_pos --;
		    }
		    long_value.append(v0_ref, 0, s_pos + 1);
		}
		/*
		 * ������
		 */
		/* �Ǹ�� '&' ������å� */
		s_pos = c0_len - 1;
		//if ( c0_ref.cchr(s_pos) == '&' ) {
		//    s_pos --;
		//    prev_comment_amp_exists = true;
		//}
		long_comment.append(c0_ref, 0, s_pos + 1);
	    }
	    else {
		size_t s_pos;
		long_value.append(v0_ref);
		/* �Ǹ�� '&' ������å� */
		s_pos = c0_len - 1;
		//if ( c0_ref.cchr(s_pos) == '&' ) {
		//    s_pos --;
		//    prev_comment_amp_exists = true;
		//}
		long_comment.append(c0_ref, 0, s_pos + 1);
	    }

	    //err_report1(__FUNCTION__,"DEBUG","l_v = [%s]",long_value.cstr());
	    //err_report1(__FUNCTION__,"DEBUG","l_c = [%s]",long_comment.cstr());

	    /* ³�� CONTINUE ʸ��������äƤ��ޤ� */
	    /* (�����Ѥߤ� CONTINUE �쥳���ɤϺ������) */
	    n_erase = 0;
	    while ( j < target_header->length() &&
		    is_continue_record(target_header->record(j)) == true ) {
		bool curr_comment_amp_exists = false;
		/* �ʥޤ��͡������ǤϺ����ζ���Ϥ��Τޤ� */
		const tstring &vref = target_header->record(j).value_cs();
		size_t l_spn, r_spn;
		bool next_is_continue;
		/* ���� CONTINUE ���ɤ��� */
		next_is_continue = ( j+1 < target_header->length() &&
				     is_continue_record(target_header->record(j+1)) == true );
		/* �����Υ��ڡ���������å� */
		l_spn = vref.strspn(' ');
		if ( vref.length() == l_spn ) r_spn = 0;
		else {
		    r_spn = vref.strrspn(' ');
		    /* ���� CONTINUE �ξ��� '&' ������å� */
		    //if ( next_is_continue ) {
		    //	if (vref.cchr(vref.length() - r_spn - 1) == '&') {
		    //	    r_spn ++;
		    //	    curr_comment_amp_exists = true;
		    //	}
		    //}
		}
		
		if (target_header->record(j0).type() == FITS::STRING_T) {
		    size_t s_pos = l_spn;
		    /* �������� "'" ��õ���Ƥ��� */
		    if ( prev_value_amp_exists == true &&
			 vref.cchr(s_pos) == '\'' ) {
			size_t v_begin;	/* ʸ�����ͤγ����� */
			s_pos ++;
			v_begin = s_pos;
			while ( 1 ) {
			    ssize_t f_pos;
			    if ( (f_pos=vref.find(s_pos,'\'')) < 0 ) {
				size_t v_len;
				/* ��ü�� "'" ��̵�����ϡ������ͤˤ��� */
				v_len = vref.length() - v_begin;
				/* �Ǹ�Υ��ڡ����ξõ� */
				if ( r_spn <= v_len ) v_len -= r_spn;
				/* �Ǹ�� "&" �����ä��餽�Τޤޤˤ��Ȥ� */
				if ( curr_comment_amp_exists == true ) v_len ++;
				long_value.append(vref, v_begin, v_len);
				prev_value_amp_exists = false;
				break;
			    }
			    if ( vref.cchr(f_pos+1) == '\'' ) {
				/* ��''�פΤ���ν��� */
				s_pos = f_pos+2;
				/* while() ����� */
			    }
			    else {
				/* comment ʸ�����õ�� */
				/* "/" or '&' ��õ���ơ�����ʤ����
				   while() ����� */
				s_pos = f_pos + 1;
				/* '&' ��õ�������������ƤϤ����ʤ�*/
				s_pos += vref.strspn(s_pos,' ');
				if ( s_pos == vref.length() ||
				     vref.cchr(s_pos) == '/' ||
				     vref.cchr(s_pos) == '&' ) {
				    /* �褦�䤯��� */
				    size_t s_spn, v_len, c_len;
				    if ( s_pos < vref.length() ) {
					s_pos++;
					s_pos += vref.strspn(s_pos,' ');
				    }
				    /* register comment */
				    c_len = vref.length() - s_pos;
				    /* �Ǹ�Υ��ڡ����� '&' �ξõ� */
				    if ( r_spn <= c_len ) c_len -= r_spn;
				    if ( 0 < c_len ) {
					if ( 0 < long_comment.length() &&
					     prev_comment_amp_exists == false ) {
					    long_comment.append(" ");
					}
					long_comment.append(vref, 
							    s_pos, c_len);
				    }
				    /* register value */
				    s_spn = vref.strrspn(f_pos-1,' ');
				    /* ���� CONTINUE �ξ��� '&' ������å� */
				    prev_value_amp_exists = false;  /* reset */
				    if ( next_is_continue ) {
					if ( vref.cchr(f_pos-s_spn-1) == '&') {
					    prev_value_amp_exists = true;
					    s_spn++;
					}
				    }
				    v_len = f_pos - s_spn - v_begin;
				    long_value.append(vref, v_begin,v_len);
				    break;
				}
				s_pos = f_pos+1;
				/* while() ����� */
			    }
			}
		    }
		    else {
			size_t c_len;
			/* ʸ�����Ƚ��Ǥ��ʤ����ϥ����ȤȤ��ƽ��� */
			c_len = vref.length() - l_spn - r_spn;
			if ( 0 < c_len ) {
			    if ( 0 < long_comment.length() &&
			    	 prev_comment_amp_exists == false ) {
			    	long_comment.append(" ");
			    }
			    /* ��Ƭ�ˡ�/�פ�����н��� */
			    if ( 2 <= c_len && vref.cchr(l_spn) == '/' && 
				 vref.cchr(l_spn + 1) == ' ' ) {
				long_comment.append(vref, l_spn+2, c_len-2);
			    }
			    else if ( 1 <= c_len && vref.cchr(l_spn) == '/' ) {
				long_comment.append(vref, l_spn+1, c_len-1);
			    }
			    else {
				long_comment.append(vref, l_spn, c_len);
			    }
			}
		    }
		}
		else {
		    size_t c_len;
		    /* ʸ���󷿰ʳ��ξ���ñ�˥����ȤȤ��ƽ��� */
		    c_len = vref.length() - l_spn - r_spn;
		    if ( 0 < c_len ) {
			if ( 0 < long_comment.length() &&
			     prev_comment_amp_exists == false ) {
			    long_comment.append(" ");
			}
			/* ��Ƭ�ˡ�/�פ�����н��� */
			if ( 2 <= c_len && vref.cchr(l_spn) == '/' && 
			     vref.cchr(l_spn + 1) == ' ' ) {
			    long_comment.append(vref, l_spn+2, c_len-2);
			}
			else if ( 1 <= c_len && vref.cchr(l_spn) == '/' ) {
			    long_comment.append(vref, l_spn+1, c_len-1);
			}
			else {
			    long_comment.append(vref, l_spn, c_len);
			}
		    }
		}

		/* count CONTINUE records to erase */
		n_erase ++;
		j ++ ;

		prev_comment_amp_exists = curr_comment_amp_exists;

	    }	/* while ( ) ... */

	    /* ʸ�����ͤξ��ϡ��Ǹ�� "'" ���ɲ� */
	    if ( target_header->record(j0).type() == FITS::STRING_T ) {
		long_value.append("'");
	    }
	    
	    //err_report1(__FUNCTION__,"DEBUG","length of long_value = [%d]",
	    //			(int)(long_value.length()));
	    //err_report1(__FUNCTION__,"DEBUG","long_value = [%s]",
	    //			    long_value.cstr());
	    //err_report1(__FUNCTION__,"DEBUG","long_comment = [%s]",
	    //			    long_comment.cstr());
	    //err_report1(__FUNCTION__,"DEBUG","%s","========");
	    
	    /* ���ꥸ�ʥ�� 80-char ����¸�����ͤ򹹿� */
	    {
		fits_header_record &rec_j0 = target_header->at(j0);
		tstring a_formatted_rec_bak;
		long i;

		/* a_formatted_rec ��Хå����å� */
		a_formatted_rec_bak = rec_j0.a_formatted_rec;
		/* �ͤ򹹿� (a_formatted_rec �Ͼõ���) */
		target_header->assignf_comment(j0, "%s",long_comment.cstr());
		target_header->assignf_value(j0, "%s", long_value.cstr());

		/* fitscc ����Ȥ�줿��硤���ꥸ�ʥ�� 80-char �Ϥʤ� */
		if ( a_formatted_rec_bak.cstr() != NULL ) {
		    /* 80 x n ʸ���� a_formatted_rec ����¸ */
		    rec_j0.a_formatted_rec
			.resize(FITS::HEADER_RECORD_UNIT * (1 + n_erase));
		    rec_j0.a_formatted_rec.put(0, a_formatted_rec_bak);
		    for ( i=0 ; i < n_erase ; i++ ) {
			rec_j0.a_formatted_rec.put(
				FITS::HEADER_RECORD_UNIT * (1 + i),
				target_header->at(j0 + 1 + i).a_formatted_rec);
		    }
		}
	    }

	    /* erase CONTINUE records */
	    target_header->erase_records(j0 + 1, n_erase);

	    /* important! */
	    j = j0;
	    
	}	/* if ( is_continue_record(...) ) ... */
    }
    
    return *this;
}

/**
 * @brief  shallow copy ����Ĥ�����˻��� (̤����)
 * @note   ������֥������Ȥ� return ��ľ���ǻ��Ѥ��롥
 */
/* ���: ���Ȥ��֤������Ȥ��������return ʸ�Ǥ������ȥ��󥹥��� */
/*       ��2�󥳥ԡ������Ȥ����������ʻ��ˤʤ�Τ� void �ˤʤäƤ��� */
void fits_header::set_scopy_flag()
{
    this->shallow_copy_ok = true;
    return;
}


/*
 * protected member functions
 */

/**
 * @brief  fits_header_record ���֥������ȤΥ��ɥ쥹���֤�
 *
 * @note   ���Υ��дؿ��� protected �Ǥ���
 */
fits_header_record *fits_header::get_ptr( long index0 )
{
    if ( index0 < 0 || this->num_records_rec <= index0 ) {
	return NULL;
    }
    else return this->records_ptr_rec[index0];
}

/**
 * @brief  ���ꤵ�줿�إå��쥳���ɥ��֥������ȤΥ��ɥ쥹���֤�
 *
 * @note   �桼���γ�ĥ���饹�ǻ��Ѥ����ꡥ�ݥ��󥿤�ĥ�����ɬ�ס�<br>
 *         ���Υ��дؿ��� protected �Ǥ���
 */
fits_header_record *fits_header::get_ptr( const char *keyword0 )
{
    return this->get_ptr( this->index(keyword0,false) );
}

/**
 * @brief  ���ꤵ�줿�إå��쥳���ɤ��ݸ�˴ؤ�������
 *
 * @note   �ݥ��󥿤�ĥ�ä������ä��ʤ��褦���ݸ�뤿���ɬ�ס�<br>
 *         ���Υ��дؿ��� protected �Ǥ���
 */
fits_header &fits_header::set_protections( long index0, bool keyword, bool value_type, 
					   bool value, bool comment )
{
    fits_header_record *sys_ptr;
    sys_ptr = this->get_ptr(index0);
    if ( sys_ptr == NULL ) {
	err_report(__FUNCTION__,"WARNING","invalid index0; ignored");
	return *this;
    }
    //bool tf = sys_ptr->keyword_protected_rec;
    sys_ptr->set_protections(keyword,value_type,value,comment);
    return *this;
}

/**
 * @brief  ���Ȥ�������뤿��Υ��֥������Ȥ���Ͽ
 *
 * @note   fits_hdu �Υ��󥹥ȥ饯���ǻ��ѡ�<br>
 *         ���Υ��дؿ��� protected �Ǥ���
 */
fits_header &fits_header::register_manager( fits_hdu *ptr )
{
    this->manager = ptr;
    return *this;
}

/* fits_hdu ����Ȥ� */
/**
 * @brief  Data Unit �˴�Ϣ����إå��쥳���ɤ��Խ��ػߥե饰�򥻥å�
 *
 *  true �򥻥åȤ���ȡ������ƥ�ͽ�󥭡���ɤ���Ͽ�ԲĤˤʤ롥
 *
 * @note   �Ѿ����饹�ǥ����ƥ७����ɤ�񤭹�����˻��ѡ�<br>
 *         ���Υ��дؿ��� protected �Ǥ���
 */
fits_header &fits_header::set_sysrecords_prohibition( bool value )
{
    this->sysrecords_prohibition_rec = value;
    return *this;
}

/**
 * @brief  ���ȥ꡼��˥�����������Data Unit ���ɤ����Ф�
 *
 * @param   sref �ɤ����Ф��оݤΥ��ȥ꡼�� (cstreamio�ηѾ����饹)
 * @param   max_bytes_ptr �ɤ����Ф����٤�����ΥХ��ȿ����������׻���NULL��
 * @return  ������: �ɤ����Ф����Х��ȿ�<br>
 *          ����: ���顼
 * @note    ���Υ��дؿ��� private �Ǥ���
 */
ssize_t fits_header::data_skip( cstreamio &sref, const size_t *max_bytes_ptr )
{
    ssize_t return_len_skip = -1;
    size_t len_data_part, len_all, len_skiped_all;
    size_t len_to_skip[3];
    long naxis;
    long bytepix;
    long idx,i;
    int hdu_type;
    bool extend = true;

    hdu_type = this->hdutype();

    if ( 0 <= this->index("SIMPLE") ) {
	idx = this->index("EXTEND");
	if ( 0 <= idx ) extend = this->record(idx).bvalue();
    }

    /* normal data area */
    idx = this->index("BITPIX");
    if ( idx < 0 ) goto quit;
    bytepix = this->record(idx).llvalue();
    if ( bytepix == 0 ) {
	return_len_skip = 0;
	goto quit;
    }
    if ( bytepix < 0 ) bytepix *= -1;
    bytepix--;
    bytepix /= 8;
    bytepix++;

    idx = this->index("NAXIS");
    if ( idx < 0 ) goto quit;
    naxis = this->record(idx).llvalue();
    if ( naxis < 0 ) goto quit;
    if ( naxis == 0 ) {
	return_len_skip = 0;
	goto quit;
    }

    len_to_skip[0] = bytepix;
    for ( i=0 ; i < naxis ; i++ ) {
	tstring kwd;
	long long llv;
	kwd.assignf("NAXIS%ld",i+1);
	idx = this->index(kwd.cstr());
	if ( idx < 0 ) goto quit;
	llv = this->record(idx).llvalue();
	if ( 0 <= llv ) len_to_skip[0] *= llv;
    }

    /* heap area of binary table HDU */
    len_to_skip[1] = 0;
    idx = this->index("PCOUNT");
    if ( hdu_type == FITS::BINARY_TABLE_HDU && 0 <= idx ) {
	long long llv;
	llv = this->record(idx).llvalue();
	if ( 0 <= llv ) len_to_skip[1] += llv;
    }

    if ( len_to_skip[0] == 0 && len_to_skip[1] == 0 ) {
	return_len_skip = 0;
	goto quit;
    }

    len_data_part = len_to_skip[0] + len_to_skip[1];

    /* 2880 ���ܡ��ˤ��� */
    if ( len_data_part % FITS::FILE_RECORD_UNIT != 0 ) {
	len_to_skip[2] = 
	    FITS::FILE_RECORD_UNIT - (len_data_part % FITS::FILE_RECORD_UNIT);
    }

    len_all = len_to_skip[0] + len_to_skip[1] + len_to_skip[2];

    /* max_bytes_ptr != NULL �����½��� */
    if ( max_bytes_ptr != NULL ) {
	if ( (*max_bytes_ptr) < len_to_skip[0] ) {
	    len_to_skip[0] = (*max_bytes_ptr);
	    len_to_skip[1] = 0;
	    len_to_skip[2] = 0;
	}
	else if ( (*max_bytes_ptr) < len_data_part ) {
	    len_to_skip[1] = (*max_bytes_ptr) - len_to_skip[0];
	    len_to_skip[2] = 0;
	}
	else if ( (*max_bytes_ptr) < len_all ) {
	    len_to_skip[2] = (*max_bytes_ptr) - len_data_part;
	}
    }

    /* EXTEND = F �ξ��ǺǸ夬 0 �ǥѥǥ��󥰤���Ƥ��ʤ�����ͤ��� */
    /* len_to_skip[0], len_to_skip[1], len_to_skip[2] �ν��skip����    */

    len_skiped_all = 0;
    for ( i=0 ; i < 3 ; i++ ) {
	size_t len_skiped = 0;
	if ( 0 < len_to_skip[i] ) {
	    ssize_t sz;
	    /* Use seek when available :-) */
	    try {
		sz = sref.rskip(len_to_skip[i]);
	    }
	    catch (...) {
		err_throw(__FUNCTION__,"FATAL","unexpected exception");
	    }
	    if ( i == 2 && extend == false && sz == 0 ) {
		/* EXTEND = F �ξ��� 0 �ѥǥ��󥰤�̵�����: OK */
		len_skiped = sz;
	    }
	    else if ( sz < 0 ) {
		err_report(__FUNCTION__,"ERROR","sref.rskip() failed");
		goto quit;
	    }
	    else if ( (size_t)sz != len_to_skip[i] ) {
		err_report(__FUNCTION__,"ERROR","sref.rskip() failed");
		goto quit;
	    }
	    else {
		len_skiped = sz;
	    }
	}
	len_skiped_all += len_skiped;
    }

    //err_report1(__FUNCTION__,"DEBUG","len_skiped_all = %zd",len_skiped_all);

    return_len_skip = len_skiped_all;
 quit:
    return return_len_skip;
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
ssize_t fits_header::header_load(cstreamio &sref, const size_t *max_bytes_ptr)
{
    ssize_t return_len_read = -1;
    size_t len_read_all = 0;
    bool flag_end = false;
    char rec_buf[FITS::HEADER_RECORD_UNIT + 1];
    tstring rec_str, header_all;
    long rec_index = 0;

    rec_buf[FITS::HEADER_RECORD_UNIT] = '\0';

    /* initialize; erase all header records */
    this->fits_header::_init();

    /* read header records one by one ... */
    while ( 1 ) {
	ssize_t len_read;
	/* END �����Ĥ��äƤ��ơ��ԥå���ξ��ϥ롼��ȴ�� */
	if ( flag_end == true ) {
	    if ( len_read_all % FITS::FILE_RECORD_UNIT == 0 ) break;
	}
	/* �ɤ��Х��ȿ������¤�������Υ����å� */
	if ( max_bytes_ptr != NULL ) {
	    if ( (*max_bytes_ptr) < len_read_all + FITS::HEADER_RECORD_UNIT ) {
		/* ����Ⱦü�ξ��ϵ�����Ƥ���Ȥ��ޤ��ɤߤ��äƤ��ޤ� */
		try {
		    len_read = sref.read(rec_buf, (*max_bytes_ptr) - len_read_all);
		}
		catch (...) {
		    err_throw(__FUNCTION__,"FATAL","unexpected exception");
		}
		if ( len_read < 0 ) {
		    err_report(__FUNCTION__,"ERROR","sref.read() failed");
		    goto quit;
		}
		len_read_all += len_read;
		break;
	    }
	}
	/* read stream ... */
	try {
	    len_read = sref.read(rec_buf, FITS::HEADER_RECORD_UNIT);
	}
	catch (...) {
	    err_throw(__FUNCTION__,"FATAL","unexpected exception");
	}
	if ( len_read < 0 ) {
	    err_report1(__FUNCTION__,"ERROR",
			"sref.read() failed: len_read=%zd",len_read);
	    goto quit;
	}
	len_read_all += len_read;

	if ( len_read != (ssize_t)FITS::HEADER_RECORD_UNIT ) break; /* EOF */

	/* store a record to temp buffer */
	rec_str.import_binary(rec_buf, len_read, ' ');

	if ( flag_end == false ) {
	    /* When END is detected��set flag_end = true */
	    if ( rec_str.strncmp("END",3) == 0 &&
		 rec_str.strspn(3," \t\n\r\f\v") == FITS::HEADER_RECORD_UNIT - 3 ) {
		flag_end = true;
	    }
	}

	/* append a record to header buffer */
	if (header_all.length() < (rec_index + 1)*FITS::HEADER_RECORD_UNIT) {
	    if ( header_all.length() < FITS::FILE_RECORD_UNIT * 10 ) {
		header_all.resizeby(FITS::FILE_RECORD_UNIT * 10);
	    } else {
		header_all.resizeby( header_all.length() );
	    }
	}
	header_all.put(rec_index * FITS::HEADER_RECORD_UNIT, rec_str);

	rec_index ++;
    }

    header_all.resize(rec_index * FITS::HEADER_RECORD_UNIT);

    //err_report1(__FUNCTION__,"DEBUG","hdr length = %zd",header_all.length());
    //err_report1(__FUNCTION__,"DEBUG","cstr = [%s]",header_all.cstr());

    return_len_read = this->header_load(header_all.cstr());

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
ssize_t fits_header::header_load( const char *header_all )
{
    ssize_t return_len_read = -1;
    size_t len_read_all = 0;
    bool flag_end = false;
    tstring rec_str(FITS::HEADER_RECORD_UNIT);	/* fixed-length mode */
    long rec_index = 0;

    /* initialize; erase all header records */
    this->fits_header::_init();

    /* read header records one by one ... */
    while ( 1 ) {
	ssize_t len_read;
	if ( header_all == NULL ) break;
	if ( flag_end == true ) {
	    if ( len_read_all % FITS::FILE_RECORD_UNIT == 0 ) break;
	}
	rec_str.assign(header_all + rec_index * FITS::HEADER_RECORD_UNIT,
		       FITS::HEADER_RECORD_UNIT);
	len_read = rec_str.length();

	len_read_all += len_read;

	if ( len_read != (ssize_t)FITS::HEADER_RECORD_UNIT ) break; /* EOF */

	if ( flag_end == false ) {
	    /* When END is detected��set flag_end = true */
	    if ( rec_str.strncmp("END",3) == 0 &&
		 rec_str.strspn(3," \t\n\r\f\v") == FITS::HEADER_RECORD_UNIT - 3 ) {
		flag_end = true;
	    }
	}

	if ( flag_end == false ) {
	    fits_header_record a_record;
	    /* parse a line (private/parse_a_header_record.cc) */
	    if ( parse_a_header_record(rec_str, false, &a_record) < 0 ) {
		if ( rec_index == 0 ) {
		    err_report(__FUNCTION__,"ERROR",
			       "This is not FITS header!");
		    goto quit;
		}
	    }
	    /* ���ꥸ�ʥ�� 80-char ����¸ */
	    a_record.a_formatted_rec = rec_str;
	    /* �ɲ� */
	    this->append( a_record );
	}
	rec_index ++;
    }

    //err_report(__FUNCTION__,"DEBUG","1st step parse end");

    if ( this->length() < 1 ) {
	return_len_read = len_read_all;
	goto quit;
    }
    else {
	/* CONTINUE ��Ÿ������ */
	this->expand_continue_records();
    }

#if 0
    for ( i=0 ; i < this->length() ; i++ ) {
    	fprintf(stderr,"debug1: [%s] = [%s] / [%s]\n",
    		this->record(i).keyword(),
    		this->record(i).value(),
    		this->record(i).comment());
    }
#endif

    //err_report(__FUNCTION__,"DEBUG","2st step parse end");

    return_len_read = len_read_all;
 quit:
    return return_len_read;

}

/**
 * @brief  �إå������Ƥ򥹥ȥ꡼��ؽ񤭽Ф�
 *
 * @note   ���: �����Ǥϥ����å�����Ͻ��������ʤ����ƤӽФ�¦�ǽ��������
 *         ����<br>
 *         ���Υ��дؿ��� private �Ǥ���
 */
ssize_t fits_header::header_save( cstreamio *sptr, void *c_sum_info, 
				  bool end_and_blank )
{
    ssize_t return_size = -1;
    size_t size_all = 0;
    fitsio_csum *csum_info = (fitsio_csum *)c_sum_info;
    ssize_t len;

    if ( csum_info != NULL && end_and_blank == false ) {
	err_report(__FUNCTION__,"WARNING",
		   "Invalid end_and_blank arg; ignored");
	end_and_blank = true;
    }

    /* write formatted string to stream */
    this->formatted_string();
    len = this->formatted_rec.length();
    if ( 0 < len ) {
	ssize_t len_wrote;
	try {
	    len_wrote = write_stream_or_get_csum( this->formatted_rec.cstr(),
						  len, sptr, csum_info );
	}
	catch (...) {
	    err_throw(__FUNCTION__,"FATAL",
		      "unexpected exception in write_stream_or_get_csum()");
	}
	if ( len_wrote != len ) {
	    err_report(__FUNCTION__,"ERROR",
		       "write_stream_or_get_csum() failed");
	    goto quit;
	}
	size_all += len;
    }

    /* write "END" string and white space to make total length 2880 x N */
    if ( 0 < len && end_and_blank == true ) {
	tstring tmp_str;
	size_t block;
	ssize_t len_wrote;

	len = FITS::HEADER_RECORD_UNIT;
	/* END ��� */
	tmp_str.assign("END");
	tmp_str.append(' ',len - 3);
	try {
	    len_wrote = write_stream_or_get_csum( tmp_str.cstr(), len,
						  sptr, csum_info );
	}
	catch (...) {
	    err_throw(__FUNCTION__,"FATAL",
		      "unexpected exception in write_stream_or_get_csum()");
	}
	if ( len_wrote != len ) {
	    err_report(__FUNCTION__,"ERROR",
		       "write_stream_or_get_csum() failed");
	    goto quit;
	}
	size_all += len;

	block = 1 + (size_all - 1) / FITS::FILE_RECORD_UNIT;

	tmp_str.assign(' ',len);

	while ( size_all < block * FITS::FILE_RECORD_UNIT ) {
	    try {
		len_wrote = write_stream_or_get_csum( tmp_str.cstr(), len,
						      sptr, csum_info );
	    }
	    catch (...) {
		err_throw(__FUNCTION__,"FATAL",
			 "unexpected exception in write_stream_or_get_csum()");
	    }
	    if ( len_wrote != len ) {
		err_report(__FUNCTION__,"ERROR",
			   "write_stream_or_get_csum() failed");
		goto quit;
	    }
	    size_all += len;
	}

#if 0
	try {
	    if ( 0 < size_all && sptr != NULL ) sptr->flush();
	}
	catch (...) {
	    err_throw(__FUNCTION__,"FATAL",
		      "unexpected exception in sptr->flush()");
	}
#endif

    }

    return_size = size_all;
 quit:
    return return_size;
}


/** 
 * @brief  shallow copy ����ǽ�����֤� (̤����)
 * 
 * @return  shallow copy����ǽ�ʤ鿿<br>
 *          ����ʳ��λ��ϵ�
 * @note  ���Υ��дؿ��� private �Ǥ�
 */
bool fits_header::request_shallow_copy( fits_header *from_obj ) const
{
    return false;
}

/** 
 * @brief  shallow copy �򥭥�󥻥뤹�� (̤����)
 * 
 * @note  ���Υ��дؿ��� private �Ǥ�
 */
void fits_header::cancel_shallow_copy( fits_header *from_obj ) const
{
    return;
}

/**
 * @brief  ���ȤˤĤ��ơ�shallow copy �ط��Υ��꡼�󥢥åפ�Ԥʤ� (̤����)
 * 
 * @note  ���Υ��дؿ��� private �Ǥ�
 */
void fits_header::cleanup_shallow_copy( bool do_deep_copy_for_this ) const
{
    return;
}



#include "private/parse_a_header_record.cc"
#include "private/initialize_csum.cc"
#include "private/encode_csum.cc"
#include "private/write_stream_or_get_csum.cc"

}	/* namespace sli */

#include "private/rm_side_spaces.c"
#include "private/c_memcpy.cc"
