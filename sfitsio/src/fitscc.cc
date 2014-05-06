/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2013-05-14 18:06:20 cyamauch> */

/**
 * @file   fitscc.cc
 * @brief  1�Ĥ� FITS �ե��������Τ�ɽ�����륯�饹 fitscc �Υ�����
 */

#define CLASS_NAME "fitscc"

#include "config.h"

#include "fitscc.h"

#include <sli/digeststreamio.h>
#include <sli/tarray.h>

#include "private/err_report.h"


namespace sli
{

#include "private/parse_a_header_record.h"

/*
 * private functions for internal
 */

/**
 * @brief  "foo.fits[...]" ��ʸ�����ѡ�������HDU���󡦥���������������
 *
 *  CFITSIO���� �ޤ��� SFITSIO�����ǽ񤫤줿 "foo.fits[...]" ������ʸ�����
 *  �ѡ�������HDU���󡤥������������ʬ�䤷���֤��ޤ���<br>
 *
 * ��: path_in ... "foo.fits [ Primary[1:100,10:210]; EVENT::1(*,9:209) ]" <br>
 *     real_path => foo.fits                                               <br>
 *     hdu_info  => <Primary><EVENT::1>                                    <br>
 *     sect_info => <[1:100,10:210]><(*,9:209)>                            <br>
 *
 * @note    private �ʴؿ��Ǥ���
 */
inline static void parse_path( const char *path_in, tstring *real_path,
			  tarray_tstring *hdu_info, tarray_tstring *sect_info )
{
    tstring target;
    tstring ex0, ex1, extmp;		/* expression 0 and 1 */
    tarray_tstring arr;
    ssize_t pos;
    size_t startpos, nextpos, len, n_bkt;
    bool ex0_is_section = false;
    size_t i;

    target.assign(path_in);

    /* search extend expression part */
    n_bkt = 0;			/* number of bracket pairs of lowest depth */
    target.trim();
    startpos = target.length() - 1;
    pos = target.rfind_quoted(startpos, "'\"[]()", '\\', &len, &nextpos);
    while ( (ssize_t)(startpos + 1 - len) == pos ) {
	n_bkt ++;
	ex1 = ex0;
	target.copy(pos, len, &ex0);
	startpos = nextpos;
	startpos -= target.strrspn(startpos," ");
	pos = target.rfind_quoted(startpos, "'\"[]()", '\\', &len, &nextpos);
    }
    target.copy(0, startpos + 1, real_path);

    /* check HDU info (using comma delimiter) with CFITSIO style or */
    /* section info of IRAF style                                   */
    ex0.copy(1, ex0.length()-2, &extmp);
    arr.split(extmp, ",", false, "'\"[]()", '\\', false).trim();

    if ( n_bkt == 1 ) {
	if ( 1 < arr.length() || arr[0].strcmp("*") != 0 ) {
	    tregex rex0, rex1, rex2, rex3;
	    size_t junk;
	    /* image section => [*,1] [*,2:100] [1:10,2:100] [1:10,1] [-*,1] */
	    /* not image section => [1,1]                                    */
	    rex0.compile("^[-]?[0-9]+[ ]*+[:][ ]*[-]?[0-9]+$");
	    rex1.compile("^[-]?[0-9]+$");
	    rex2.compile("^[-][ ]*[*]$");
	    rex3.compile("^[-]?[0-9]+[ ]*+[:][ ]*[*]?$");
	    /* ���٤ƿ��������ξ���HDU��ɽ���Ȥߤʤ� */
	    for ( i=0 ; i < arr.length() ; i++ ) {
		if ( arr[i].regmatch(rex1,&junk) != 0 ) break;
	    }
	    if ( i != arr.length() ) {
		for ( i=0 ; i < arr.length() ; i++ ) {
		    if ( arr[i].strcmp("*") != 0 &&
			 arr[i].regmatch(rex0,&junk) != 0 &&
			 arr[i].regmatch(rex1,&junk) != 0 &&
			 arr[i].regmatch(rex2,&junk) != 0 &&
			 arr[i].regmatch(rex3,&junk) != 0 ) break;
		}
		if ( i == arr.length() ) ex0_is_section = true;
	    }
	}
    }

    /* convert expression to SFITSIO style */
    if ( ex0_is_section == true ) {
	extmp.assign(ex0);
	ex0.printf("[0%s]",extmp.cstr());
    }
    else {
	if ( arr.length() == 2 ) {
	    ex0.printf("[*::%s::%s]",arr.cstr(0),arr.cstr(1));
	}
	else if ( 3 <= arr.length() ) {
	    ex0.printf("[%s::%s::%s]",arr.cstr(2),arr.cstr(0),arr.cstr(1));
	}
    }

    /* check CSFITSIO style of multiple [] brackets on the same depth */
    if ( 1 < n_bkt ) {
	/* convert expression to SFITSIO style */
	ex0.insert(ex0.length()-1,ex1);
    }

    //err_report1(__FUNCTION__,"DEBUG","final ex0 = <%s>",ex0.cstr());

    /* initialize results */
    hdu_info->init();
    sect_info->init();

    /* ex0 is always SFITSIO style expression */
    if ( 2 < ex0.length() ) {
	tarray_tstring &hinfo = *hdu_info;
	tarray_tstring &sinfo = *sect_info;
	ex0.resizeby(-1).erase(0,1);
	/* split multiple HDU info with ';' */
	arr.split(ex0, ";", false, "'\"[]()", '\\', false).trim();
	/* extract HDU info and section info */
	for ( i=0 ; i < arr.length() ; i++ ) {
	    startpos = arr[i].length() - 1;
	    pos = arr[i].rfind_quoted(startpos, "'\"[]()", '\\', 
				      &len, &nextpos);
	    if ( 0 <= pos && (arr[i][pos] == '(' || arr[i][pos] == '[') ) {
		arr[i].copy(pos,len, &(sinfo[i]));
		arr[i].erase(pos,len).trim();
	    }
	    hinfo[i].assign(arr[i]);
	}
    }

    return;
}

/*
 * member functions of fitscc class
 */

/**
 * @brief  ���󥹥ȥ饯��
 */
fitscc::fitscc()
{
    this->classlevel_rec = 0;
    this->num_hdus_rec = 0;
    this->hdus_rec.init(sizeof(fits_hdu *), true);
    this->hdus_rec.register_extptr(&this->hdus_ptr_rec);	/* [void **] */
    this->hdu_is_stack_rec.init(sizeof(bool), true);

    return;
}

/**
 * @brief  ���ԡ����󥹥ȥ饯��
 */
fitscc::fitscc(const fitscc &obj)
{
    this->classlevel_rec = 0;
    this->num_hdus_rec = 0;
    this->hdus_rec.init(sizeof(fits_hdu *), true);
    this->hdus_rec.register_extptr(&this->hdus_ptr_rec);	/* [void **] */
    this->hdu_is_stack_rec.init(sizeof(bool), true);

    this->init(obj);

    return;
}

/**
 * @brief  �ǥ��ȥ饯��
 */
fitscc::~fitscc()
{
    if ( this->hdus_ptr_rec != NULL ) {
	long i;
	for ( i=0 ; i < this->num_hdus_rec ; i++ ) {
	    if ( this->hdus_ptr_rec[i] != NULL ) {
		if ( this->hdu_is_stack_rec.b(i) == false ) {
		    delete this->hdus_ptr_rec[i];
		}
		else {
		    this->hdus_ptr_rec[i]->init();
		}
	    }
	}
    }

    return;
}

/**
 * @brief  ���֥������ȤΥ��ԡ�
 */
fitscc &fitscc::operator=(const fitscc &obj)
{
    this->init(obj);
    return *this;
}

/**
 * @brief  ���٥�ʽ����
 *
 * @note   ���Υ��дؿ��Ǥ���ʬ�ɤߤλ���ϥ��ꥢ����ʤ���<br>
 *         ���Υ��дؿ��� protected �Ǥ���
 */
fitscc &fitscc::_init()	/* protected */
{
    if ( this->hdus_ptr_rec != NULL ) {
	long i;
	for ( i=0 ; i < this->num_hdus_rec ; i++ ) {
	    if ( this->hdus_ptr_rec[i] != NULL ) {
		if ( this->hdu_is_stack_rec.b(i) == false ) {
		    delete this->hdus_ptr_rec[i];
		}
		else {
		    this->hdus_ptr_rec[i]->init();
		}
	    }
	}
	this->hdus_rec.init(sizeof(fits_hdu *), true);
    }
    this->hdu_is_stack_rec.init(sizeof(bool), true);
    this->num_hdus_rec = 0;
    this->fmttype_rec = NULL;
    this->ftypever_rec = NULL;
    this->index_rec.init();

    return *this;
}

/**
 * @brief  ���֥������Ȥν����
 */
fitscc &fitscc::init()
{
    this->fitscc::_init();

    this->hdus_to_read_rec.init();
    this->section_to_read_rec.init();

    return *this;
}

/**
 * @brief  ���֥������ȤΥ��ԡ�
 */
fitscc &fitscc::init(const fitscc &obj)
{
    if ( &obj == this ) return *this;

    this->fitscc::init();

    if ( obj.hdus_ptr_rec != NULL ) {
	try {
	    this->hdus_rec.resize(obj.num_hdus_rec + 1).clean();
	}
	catch (...) {
	    err_throw(__FUNCTION__,"FATAL","hdus_rec.resize() failed");
	}
    }

    try {
	this->hdu_is_stack_rec.resize(obj.num_hdus_rec).clean();
    }
    catch (...) {
	this->hdus_rec.init(sizeof(fits_hdu *), true);
	err_throw(__FUNCTION__,"FATAL","hdu_is_stack_rec.resize() failed");
    }

    try {
	if ( obj.hdus_ptr_rec != NULL ) {
	    long i;
	    for ( i=0 ; i < obj.num_hdus_rec ; i++ ) {
		this->hdus_ptr_rec[i] = NULL;
		if ( obj.hdus_ptr_rec[i] != NULL ) {
		    if ( obj.hdus_ptr_rec[i]->classlevel() == 0 ) {
			fits_hdu *tmp_obj;
			tmp_obj = new fits_hdu;
			*tmp_obj = *(obj.hdus_ptr_rec[i]);
			tmp_obj->register_manager(this);
			this->hdus_ptr_rec[i] = tmp_obj;
		    }
		    /* ��٥� 2 �ʾ�ξ����٥� 1 �Ȥ��ƽ�������� */
		    else if ( 1 <= obj.hdus_ptr_rec[i]->classlevel() ) {
			int htype = obj.hdus_ptr_rec[i]->hdutype();
			if ( htype == FITS::IMAGE_HDU ) {
			    fits_image *tmp_obj;
			    tmp_obj = new fits_image;
			    *tmp_obj = *((fits_image *)(obj.hdus_ptr_rec[i]));
			    tmp_obj->register_manager(this);
			    this->hdus_ptr_rec[i] = tmp_obj;
			}
			else if ( htype == FITS::BINARY_TABLE_HDU ||
				  htype == FITS::ASCII_TABLE_HDU ) {
			    fits_table *tmp_obj;
			    tmp_obj = new fits_table;
			    *tmp_obj = *((fits_table *)(obj.hdus_ptr_rec[i]));
			    tmp_obj->register_manager(this);
			    this->hdus_ptr_rec[i] = tmp_obj;
			}
		    }
		}
		this->hdu_is_stack_rec.b(i) = false;
	    }
	    this->hdus_ptr_rec[i] = NULL;
	}
	this->fmttype_rec = obj.fmttype_rec;
	this->ftypever_rec = obj.ftypever_rec;
	this->index_rec = obj.index_rec;
    }
    catch (...) {
	this->fitscc::init();
	err_throw(__FUNCTION__,"FATAL","new or '=' failed");
    }

    this->num_hdus_rec = obj.num_hdus_rec;

    return *this;
}

/**
 * @brief  FITS�ե�������ɤ߼��
 */
ssize_t fitscc::read_stream( cstreamio &sref, size_t max_bytes_read )
{
    return this->fits_load( sref, &max_bytes_read );
}

/**
 * @brief  FITS�ե�������ɤ߼��
 */
ssize_t fitscc::read_stream( cstreamio &sref )
{
    return this->fits_load( sref, NULL );
}

/**
 * @brief  FITS�ե�������ɤ߼��
 */
ssize_t fitscc::read_stream( const char *path )
{
    ssize_t return_val = -1;
    digeststreamio sio;
    tstring real_path;
    int status;

    if ( path == NULL ) goto quit;

    try {
	parse_path( path, &real_path,
		    &(this->hdus_to_read_rec), &(this->section_to_read_rec) );
	//err_report1(__FUNCTION__,"DEBUG","real_path=<%s>",real_path.cstr());
	//this->hdus_to_read_rec.dprint();
	//this->section_to_read_rec.dprint();
	status = sio.open("r",real_path.cstr());
    }
    catch (...) {
	err_throw(__FUNCTION__,"FATAL","unexpected exception");
    }
    if ( status < 0 ) {
	err_report1(__FUNCTION__,"ERROR","cannot open: %s",path);
	goto quit;
    }
    return_val = this->fits_load( sio, NULL );

    sio.close();

 quit:
    return return_val;
}

/**
 * @brief  FITS�ե�������ɤ߼�� (printf()�ε�ˡ�ǥѥ������)
 */
ssize_t fitscc::vreadf_stream( const char *path_fmt, va_list ap )
{
    ssize_t return_val = -1;
    tstring path;
    if ( path_fmt == NULL ) {
	return_val = this->fitscc::read_stream( path_fmt );
    }
    else {
        path.vassignf(path_fmt,ap);
	return_val = this->fitscc::read_stream( path.cstr() );
    }

    return return_val;
}

/**
 * @brief  FITS�ե�������ɤ߼�� (printf()�ε�ˡ�ǥѥ������)
 */
ssize_t fitscc::readf_stream( const char *path_fmt, ... )
{
    ssize_t return_val;
    va_list ap;
    va_start(ap,path_fmt);
    try {
	return_val = this->fitscc::vreadf_stream( path_fmt, ap );
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","caught exception");
    }
    va_end(ap);
    return return_val;
}

/* */

/**
 * @brief  FITS�ե�����ؤν񤭽Ф�
 */
ssize_t fitscc::write_stream( cstreamio &sref )
{
    ssize_t return_sz = -1;
    size_t all_sz = 0;
    long i;

    for ( i=0 ; i < this->num_hdus_rec ; i++ ) {
	ssize_t sz;
	sz = this->hdu(i).write_stream(sref);
	if ( sz < 0 ) {
	    err_report1(__FUNCTION__,"ERROR",
			"hdu(%ld).write_stream() failed",i);
	    goto quit;
	}
	all_sz += sz;
    }

    return_sz = all_sz;
 quit:
    return return_sz;
}

/**
 * @brief  FITS�ե�����ؤν񤭽Ф�
 */
ssize_t fitscc::write_stream( const char *path )
{
    ssize_t return_val = -1;
    digeststreamio sio;
    int status;

    if ( path == NULL ) goto quit;

    try {
	status = sio.open("w",path);
    }
    catch (...) {
	err_throw(__FUNCTION__,"FATAL","unexpected exception");
    }
    if ( status < 0 ) {
	err_report1(__FUNCTION__,"ERROR","cannot open: %s",path);
	goto quit;
    }
    return_val = this->fitscc::write_stream(sio);

    sio.close();

 quit:
    return return_val;
}

/**
 * @brief  FITS�ե�����ؤν񤭽Ф� (printf()�ε�ˡ�ǥѥ������)
 */
ssize_t fitscc::vwritef_stream( const char *path_fmt, va_list ap )
{
    ssize_t return_val = -1;
    tstring path;
    if ( path_fmt == NULL ) {
	return_val = this->fitscc::write_stream( path_fmt );
    }
    else {
        path.vassignf(path_fmt,ap);
	return_val = this->fitscc::write_stream( path.cstr() );
    }

    return return_val;
}

/**
 * @brief  FITS�ե�����ؤν񤭽Ф� (printf()�ε�ˡ�ǥѥ������)
 */
ssize_t fitscc::writef_stream( const char *path_fmt, ... )
{
    ssize_t return_val;
    va_list ap;
    va_start(ap,path_fmt);
    try {
	return_val = this->fitscc::vwritef_stream( path_fmt, ap );
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","caught exception");
    }
    va_end(ap);
    return return_val;
}


/**
 * @brief  ���ޥ�ɷ�ͳ�Ǥ�FITS�ե�������ɤ߽� (printf()�ε�ˡ�ǥѥ������)
 *
 *  Perl �饤���� path �����Ǥ��ޤ���<br>
 *  fits.access_stream("| lbzip2 > hoge.fits.bz2"); <br>
 *  fits.access_stream("cat hoge.fits.bz2 | lbzip2 -d |"); <br>
 *  �Τ褦�ʻȤ�������ǽ�Ǥ���
 *
 */
ssize_t fitscc::access_stream( const char *path )
{
    ssize_t return_val = -1;
    digeststreamio sio;
    tstring real_path;
    int status;

    if ( path == NULL ) goto quit;

    try {
	parse_path( path, &real_path,
		    &(this->hdus_to_read_rec), &(this->section_to_read_rec) );
	status = sio.openp(real_path.cstr());
    }
    catch (...) {
	err_throw(__FUNCTION__,"FATAL","unexpected exception");
    }
    if ( status < 0 ) {
	err_report1(__FUNCTION__,"ERROR","cannot open: %s",path);
	goto quit;
    }
    if ( sio.is_write_mode() ) return_val = this->fitscc::write_stream(sio);
    else return_val = this->fitscc::read_stream(sio);

    sio.close();

 quit:
    return return_val;
}

/**
 * @brief  ���ޥ�ɷ�ͳ�Ǥ�FITS�ե�������ɤ߽� (printf()�ε�ˡ�ǥѥ������)
 */
ssize_t fitscc::vaccessf_stream( const char *path_fmt, va_list ap )
{
    ssize_t return_val = -1;
    tstring path;
    if ( path_fmt == NULL ) {
	return_val = this->fitscc::access_stream( path_fmt );
    }
    else {
        path.vassignf(path_fmt,ap);
	return_val = this->fitscc::access_stream( path.cstr() );
    }

    return return_val;
}

/**
 * @brief  ���ޥ�ɷ�ͳ�Ǥ�FITS�ե�������ɤ߽� (printf()�ε�ˡ�ǥѥ������)
 */
ssize_t fitscc::accessf_stream( const char *path_fmt, ... )
{
    ssize_t return_val;
    va_list ap;
    va_start(ap,path_fmt);
    try {
	return_val = this->fitscc::vaccessf_stream( path_fmt, ap );
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","caught exception");
    }
    va_end(ap);
    return return_val;
}

/**
 * @brief  �ƥ�ץ졼�ȥե�������ɤ߼��
 */
ssize_t fitscc::read_template( int flags, const char *path )
{
    ssize_t return_val = -1;

    if ( path == NULL ) goto quit;

    try {
	/* initialize buffer */
	this->template_buffer.init();
	this->template_buffer.resize(10000);
	this->template_buffer_idx = 0;
	/* load */
	this->template_load_recursively(flags, path, NULL);
	/* resize and parse template */
	this->template_buffer.resize(this->template_buffer_idx);
	return_val = this->template_load( flags, 
					  this->template_buffer.cstr() );
    }
    catch (...) {
	/* initialize buffer */
	this->template_buffer.init();
	this->template_buffer_idx = 0;
	err_throw(__FUNCTION__,"FATAL","caught exception");
    }

 quit:
    /* initialize buffer */
    this->template_buffer.init();
    this->template_buffer_idx = 0;

    return return_val;
}

/**
 * @brief  �ƥ�ץ졼�ȥե�������ɤ߼��
 */
ssize_t fitscc::read_template( const char *path )
{
    return this->fitscc::read_template(0, path);
}

/**
 * @brief  �ƥ�ץ졼�ȥե�������ɤ߼�� (printf()�ε�ˡ�ǥѥ������)
 */
ssize_t fitscc::vreadf_template( const char *path_fmt, va_list ap )
{
    ssize_t return_val = -1;
    tstring path;
    if ( path_fmt == NULL ) {
	return_val = this->fitscc::read_template( 0, path_fmt );
    }
    else {
        path.vassignf(path_fmt,ap);
	return_val = this->fitscc::read_template( 0, path.cstr() );
    }

    return return_val;
}

/**
 * @brief  �ƥ�ץ졼�ȥե�������ɤ߼�� (printf()�ε�ˡ�ǥѥ������)
 */
ssize_t fitscc::readf_template( const char *path_fmt, ... )
{
    ssize_t return_val;
    va_list ap;
    va_start(ap,path_fmt);
    try {
	return_val = this->fitscc::vreadf_template( path_fmt, ap );
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","caught exception");
    }
    va_end(ap);
    return return_val;
}


/**
 * @brief  ���Ϥ�����󰵽�FITS�ե�����ΥХ���Ĺ�����
 */
ssize_t fitscc::stream_length()
{
    ssize_t return_sz = -1;
    size_t all_sz = 0;
    long i;

    for ( i=0 ; i < this->num_hdus_rec ; i++ ) {
	ssize_t sz;
	sz = this->hdu(i).stream_length();
	if ( sz < 0 ) {
	    err_report1(__FUNCTION__,"ERROR",
			"hdu(%ld).stream_length() failed",i);
	    goto quit;
	}
	all_sz += sz;
    }

    return_sz = all_sz;
 quit:
    return return_sz;
}

/**
 * @brief  ���Ϥ�����󰵽�FITS�ե�����ΥХ���Ĺ�����
 *
 * @note  fitscc::stream_length() �Ȥΰ㤤�Ϥ���ޤ���
 */
ssize_t fitscc::stream_size()
{
    return this->stream_length();
}

/**
 * @brief  HDU�θĿ������
 */
long fitscc::length() const
{
    return this->num_hdus_rec;
}

/**
 * @brief  HDU�θĿ������
 *
 * @note  fitscc::length() �Ȥΰ㤤�Ϥ���ޤ���
 */
long fitscc::size() const
{
    return this->num_hdus_rec;
}

/**
 * @brief  ���饹�ηѾ���٥�����
 */
int fitscc::classlevel() const
{
    return this->classlevel_rec;
}

/**
 * @deprecated  ��侩�������������ɤǤϻȤ�ʤ��Ǥ���������
 */
fitscc &fitscc::create()
{
    return this->create_image(NULL,0,FITS::SHORT_T,(long *)NULL,0,false);
}

/**
 * @deprecated  ��侩�������������ɤǤϻȤ�ʤ��Ǥ���������
 */
fitscc &fitscc::create( const char *fmttype, long long ftypever )
{
    return this->create_image(fmttype,ftypever,
			      FITS::SHORT_T,(long *)NULL,0,false);
}

/**
 * @deprecated  ��侩�������������ɤǤϻȤ�ʤ��Ǥ���������
 */
fitscc &fitscc::create_image( int type, long naxisx[], long ndim, 
			      bool init_buf )
{
    return this->create_image(NULL,0, type,naxisx,ndim, init_buf);
}

/**
 * @deprecated  ��侩�������������ɤǤϻȤ�ʤ��Ǥ���������
 */
fitscc &fitscc::create_image( const char *fmttype, long long ftypever,
			    int type, long naxisx[], long ndim, bool init_buf )
{
    if ( 0 < this->num_hdus_rec ) {
	err_report(__FUNCTION__,"WARNING","Primary HDU already exists; ignored");
	return *this;
    }

    if ( fmttype != NULL ) {
	this->assign_fmttype(fmttype,ftypever);
    }

    this->append_image( NULL, 0, type, naxisx, ndim, init_buf );

    return *this;
}

/**
 * @deprecated  ��侩�������������ɤǤϻȤ�ʤ��Ǥ���������
 */
fitscc &fitscc::create_image( int type, long naxis0, long naxis1, long naxis2 )
{
    return this->create_image( NULL,0, type,naxis0,naxis1,naxis2 );
}

/**
 * @deprecated  ��侩�������������ɤǤϻȤ�ʤ��Ǥ���������
 */
fitscc &fitscc::create_image( const char *fmttype, long long ftypever,
			      int type, long naxis0, long naxis1, long naxis2 )
{
    if ( 0 < this->num_hdus_rec ) {
	err_report(__FUNCTION__,"WARNING","Primary HDU already exists; ignored");
	return *this;
    }

    if ( fmttype != NULL ) {
	this->assign_fmttype(fmttype,ftypever);
    }

    this->append_image( NULL, 0, type, naxis0, naxis1, naxis2 );

    return *this;
}

/**
 * @deprecated  ��侩�������������ɤǤϻȤ�ʤ��Ǥ���������
 */
fitscc &fitscc::create_image( const fits_image &src )
{
    return this->create_image(NULL,0, src);
}

/**
 * @deprecated  ��侩�������������ɤǤϻȤ�ʤ��Ǥ���������
 */
fitscc &fitscc::create_image( const char *fmttype, long long ftypever,
			      const fits_image &src )
{
    if ( 0 < this->num_hdus_rec ) {
	err_report(__FUNCTION__,"WARNING","Primary HDU already exists; ignored");
	return *this;
    }

    if ( fmttype != NULL ) {
	this->assign_fmttype(fmttype,ftypever);
    }

    this->append_image( NULL, 0, src );

    return *this;
}

/**
 * @brief  Image HDU ���ɲ�
 */
fitscc &fitscc::append_image( const char *extname, long long extver )
{
    long index0;

    this->append(FITS::IMAGE_HDU);

    index0 = this->length() - 1;

    /* index_rec �򹹿� */
    if ( extname != NULL ) {
	this->assign_extname( index0, extname );
	this->assign_extver( index0, extver );
    }

    return *this;
}

/**
 * @brief  Image HDU ���ɲ� (n�����β����ǡ�����������)
 */
fitscc &fitscc::append_image( const char *extname, long long extver,
			      int type, long naxisx[], long ndim, 
			      bool init_buf )
{
    long index0;

    this->append(FITS::IMAGE_HDU);

    index0 = this->length() - 1;
    this->image(index0).init(type,naxisx,ndim,init_buf);

    /* index_rec �򹹿� */
    if ( extname != NULL ) {
	this->assign_extname( index0, extname );
	this->assign_extver( index0, extver );
    }

    return *this;
}

/**
 * @brief  Image HDU ���ɲ� (1��3�����β����ǡ�����������)
 */
fitscc &fitscc::append_image( const char *extname, long long extver,
			      int type, long naxis0, long naxis1, long naxis2 )
{
    long index0;

    this->append(FITS::IMAGE_HDU);

    index0 = this->length() - 1;
    this->image(index0).init(type,naxis0,naxis1,naxis2);
    
    /* index_rec �򹹿� */
    if ( extname != NULL ) {
	this->assign_extname( index0, extname );
	this->assign_extver( index0, extver );
    }

    return *this;
}

/**
 * @brief  Image HDU ���ɲä��������� fits_image ���֥������Ȥ򥳥ԡ�
 */
fitscc &fitscc::append_image( const char *extname, long long extver,
			      const fits_image &src )
{
    long index0;

    this->append(FITS::IMAGE_HDU);

    index0 = this->length() - 1;
    this->image(index0).init(src);

    /* index_rec �򹹿� */
    if ( extname != NULL ) {
	this->assign_extname( index0, extname );
	this->assign_extver( index0, extver );
    }

    return *this;
}

/**
 * @brief  Image HDU ���ɲä��������� fits_image ���֥������Ȥ򥳥ԡ�
 */
fitscc &fitscc::append_image( const fits_image &src )
{
    return this->append_image((const char *)NULL, 0, src);
}

/**
 * @brief  ASCII Table �ޤ��� Binary Table HDU ���ɲ�
 */
fitscc &fitscc::append_table( const char *extname, long long extver,
			      bool ascii )
{
    long index0;

    if ( ascii == true ) this->append(FITS::ASCII_TABLE_HDU);
    else this->append(FITS::BINARY_TABLE_HDU);

    index0 = this->length() - 1;

    /* index_rec �򹹿� */
    if ( extname != NULL ) {
	this->assign_extname( index0, extname );
	this->assign_extver( index0, extver );
    }

    return *this;
}

/**
 * @brief  ASCII Table �ޤ��� Binary Table HDU ���ɲ�
 */
fitscc &fitscc::append_table( const char *extname, long long extver,
			      const fits::table_def defs[], bool ascii )
{
    long index0, olen;

    olen = this->length();

    if ( ascii == true ) this->append(FITS::ASCII_TABLE_HDU);
    else this->append(FITS::BINARY_TABLE_HDU);

    index0 = this->length() - 1;
    this->table(index0).init(defs);

    if ( ascii == true && this->hdutype(index0) == FITS::BINARY_TABLE_HDU ) {
	err_report(__FUNCTION__,"WARNING",
	"Definition is not valid for ASCII table; a BINARY table is appended");
    }

    /* index_rec �򹹿� */
    if ( extname != NULL ) {
	this->assign_extname( index0, extname );
	this->assign_extver( index0, extver );
    }

    return *this;
}

/**
 * @brief  ASCII Table �ޤ��� Binary Table HDU ���ɲ�
 */
fitscc &fitscc::append_table( const char *extname, long long extver,
			      const fits::table_def_all defs[], bool ascii )
{
    long index0, olen;

    olen = this->length();

    if ( ascii == true ) this->append(FITS::ASCII_TABLE_HDU);
    else this->append(FITS::BINARY_TABLE_HDU);

    index0 = this->length() - 1;
    this->table(index0).init(defs);

    if ( ascii == true && this->hdutype(index0) == FITS::BINARY_TABLE_HDU ) {
	err_report(__FUNCTION__,"WARNING",
	"Definition is not valid for ASCII table; a BINARY table is appended");
    }
    
    /* index_rec �򹹿� */
    if ( extname != NULL ) {
	this->assign_extname( index0, extname );
	this->assign_extver( index0, extver );
    }

    return *this;
}

/**
 * @brief  ASCII Table �ޤ��� Binary Table HDU ���ɲä����̥��֥������Ȥ򥳥ԡ�
 */
fitscc &fitscc::append_table( const char *extname, long long extver,
			      const fits_table &src )
{
    long index0, olen;

    olen = this->length();

    this->append(FITS::BINARY_TABLE_HDU);

    index0 = this->length() - 1;
    this->table(index0).init(src);

    /* index_rec �򹹿� */
    if ( extname != NULL ) {
	this->assign_extname( index0, extname );
	this->assign_extver( index0, extver );
    }

    return *this;
}

/**
 * @brief  ASCII Table �ޤ��� Binary Table HDU ���ɲä����̥��֥������Ȥ򥳥ԡ�
 */
fitscc &fitscc::append_table( const fits_table &src )
{
    return this->append_table((const char *)NULL, 0, src);
}

/**
 * @brief  Image HDU ������
 */
fitscc &fitscc::insert_image( long index0, 
			      const char *extname, long long extver )
{
    if ( index0 < 0 || this->num_hdus_rec < index0 ) {
	err_report(__FUNCTION__,"WARNING","invalid index0; ignored");
	return *this;
    }

    this->insert(index0,FITS::IMAGE_HDU);

    /* index_rec �򹹿� */
    if ( extname != NULL ) {
	this->assign_extname( index0, extname );
	this->assign_extver( index0, extver );
    }

    return *this;
}

/**
 * @brief  Image HDU ������
 */
fitscc &fitscc::insert_image( const char *extname0, 
			      const char *extname, long long extver )
{
    long idx;

    if ( extname0 == NULL ) {
	err_report(__FUNCTION__,"WARNING","NULL extname0; ignored");
	return *this;
    }

    idx = this->index_rec.index(extname0,0);
    if ( idx < 0 ) {
	err_report1(__FUNCTION__,"WARNING",
		    "HDU `%s' is not found; ignored",extname0);
	return *this;
    }

    this->insert_image(idx, extname, extver);

    return *this;
}

/**
 * @brief  Image HDU ������ (n�����β����ǡ�����������)
 */
fitscc &fitscc::insert_image( long index0, 
			      const char *extname, long long extver,
			      int type, long naxisx[], long ndim,
			      bool init_buf )
{
    if ( index0 < 0 || this->num_hdus_rec < index0 ) {
	err_report(__FUNCTION__,"WARNING","invalid index0; ignored");
	return *this;
    }

    this->insert(index0,FITS::IMAGE_HDU);

    this->image(index0).init(type,naxisx,ndim,init_buf);
    
    /* index_rec �򹹿� */
    if ( extname != NULL ) {
	this->assign_extname( index0, extname );
	this->assign_extver( index0, extver );
    }

    return *this;
}

/**
 * @brief  Image HDU ������ (1��3�����β����ǡ�����������)
 */
fitscc &fitscc::insert_image( long index0, 
			      const char *extname, long long extver,
			      int type, long naxis0, long naxis1, long naxis2 )
{
    if ( index0 < 0 || this->num_hdus_rec < index0 ) {
	err_report(__FUNCTION__,"WARNING","invalid index0; ignored");
	return *this;
    }

    this->insert(index0,FITS::IMAGE_HDU);

    this->image(index0).init(type,naxis0,naxis1,naxis2);
    
    /* index_rec �򹹿� */
    if ( extname != NULL ) {
	this->assign_extname( index0, extname );
	this->assign_extver( index0, extver );
    }

    return *this;
}

/**
 * @brief  Image HDU ������ (n�����β����ǡ�����������)
 */
fitscc &fitscc::insert_image( const char *extname0, 
			      const char *extname, long long extver,
			      int type, long naxisx[], long ndim,
			      bool init_buf )
{
    long idx;

    if ( extname0 == NULL ) {
	err_report(__FUNCTION__,"WARNING","NULL extname0; ignored");
	return *this;
    }

    idx = this->index_rec.index(extname0,0);
    if ( idx < 0 ) {
	err_report1(__FUNCTION__,"WARNING",
		    "HDU `%s' is not found; ignored",extname0);
	return *this;
    }

    this->insert_image(idx, extname, extver, type, naxisx, ndim, init_buf);

    return *this;
}

/**
 * @brief  Image HDU ������ (1��3�����β����ǡ�����������)
 */
fitscc &fitscc::insert_image( const char *extname0, 
			      const char *extname, long long extver,
			      int type, long naxis0, long naxis1, long naxis2 )
{
    long idx;

    if ( extname0 == NULL ) {
	err_report(__FUNCTION__,"WARNING","NULL extname0; ignored");
	return *this;
    }

    idx = this->index_rec.index(extname0,0);
    if ( idx < 0 ) {
	err_report1(__FUNCTION__,"WARNING",
		    "HDU `%s' is not found; ignored",extname0);
	return *this;
    }

    this->insert_image( idx, extname, extver, 
			type, naxis0, naxis1, naxis2 );
    return *this;
}

/**
 * @brief  Image HDU ���������������� fits_image ���֥������Ȥ򥳥ԡ�
 */
fitscc &fitscc::insert_image( long index0,
			      const char *extname, long long extver,
			      const fits_image &src )
{
    if ( index0 < 0 || this->num_hdus_rec < index0 ) {
	err_report(__FUNCTION__,"WARNING","invalid index0; ignored");
	return *this;
    }

    this->insert(index0,FITS::IMAGE_HDU);

    this->image(index0).init(src);
    
    /* index_rec �򹹿� */
    if ( extname != NULL ) {
	this->assign_extname( index0, extname );
	this->assign_extver( index0, extver );
    }

    return *this;
}

/**
 * @brief  Image HDU ���������������� fits_image ���֥������Ȥ򥳥ԡ�
 */
fitscc &fitscc::insert_image( const char *extname0,
			      const char *extname, long long extver,
			      const fits_image &src )
{
    long idx;

    if ( extname0 == NULL ) {
	err_report(__FUNCTION__,"WARNING","NULL extname0; ignored");
	return *this;
    }

    idx = this->index_rec.index(extname0,0);
    if ( idx < 0 ) {
	err_report1(__FUNCTION__,"WARNING",
		    "HDU `%s' is not found; ignored",extname0);
	return *this;
    }

    this->insert_image(idx, extname, extver, src);

    return *this;
}

/**
 * @brief  Image HDU ���������������� fits_image ���֥������Ȥ򥳥ԡ�
 */
fitscc &fitscc::insert_image( long index0, const fits_image &src )
{
    return this->insert_image(index0, (const char *)NULL, 0, src);
}

/**
 * @brief  Image HDU ���������������� fits_image ���֥������Ȥ򥳥ԡ�
 */
fitscc &fitscc::insert_image( const char *extname0, const fits_image &src )
{
    return this->insert_image(extname0, (const char *)NULL, 0, src);
}

/**
 * @brief  ASCII Table �ޤ��� Binary Table HDU ������
 */
fitscc &fitscc::insert_table( long index0, 
			      const char *extname, long long extver,
			      bool ascii )
{
    if ( index0 < 0 || this->num_hdus_rec < index0 ) {
	err_report(__FUNCTION__,"WARNING","invalid index0; ignored");
	return *this;
    }

    if ( ascii == true ) this->insert(index0,FITS::ASCII_TABLE_HDU);
    else this->insert(index0,FITS::BINARY_TABLE_HDU);

    /* index_rec �򹹿� */
    if ( extname != NULL ) {
	this->assign_extname( index0, extname );
	this->assign_extver( index0, extver );
    }

    return *this;
}

/**
 * @brief  ASCII Table �ޤ��� Binary Table HDU ������
 */
fitscc &fitscc::insert_table( const char *extname0, 
			      const char *extname, long long extver,
			      bool ascii )
{
    long idx;

    if ( extname0 == NULL ) {
	err_report(__FUNCTION__,"WARNING","NULL extname0; ignored");
	return *this;
    }

    idx = this->index_rec.index(extname0,0);
    if ( idx < 0 ) {
	err_report1(__FUNCTION__,"WARNING",
		    "HDU `%s' is not found; ignored",extname0);
	return *this;
    }

    this->insert_table(idx, extname, extver, ascii);

    return *this;
}

/**
 * @brief  ASCII Table �ޤ��� Binary Table HDU ������
 */
fitscc &fitscc::insert_table( long index0, 
			      const char *extname, long long extver,
			      const fits::table_def defs[], bool ascii )
{
    if ( index0 < 0 || this->num_hdus_rec < index0 ) {
	err_report(__FUNCTION__,"WARNING","invalid index0; ignored");
	return *this;
    }

    if ( ascii == true ) this->insert(index0,FITS::ASCII_TABLE_HDU);
    else this->insert(index0,FITS::BINARY_TABLE_HDU);

    this->table(index0).init(defs);
    
    if ( ascii == true && this->hdutype(index0) == FITS::BINARY_TABLE_HDU ) {
	err_report(__FUNCTION__,"WARNING",
	"Definition is not valid for ASCII table; a BINARY table is inserted");
    }

    /* index_rec �򹹿� */
    if ( extname != NULL ) {
	this->assign_extname( index0, extname );
	this->assign_extver( index0, extver );
    }

    return *this;
}

/**
 * @brief  ASCII Table �ޤ��� Binary Table HDU ������
 */
fitscc &fitscc::insert_table( long index0, 
			      const char *extname, long long extver,
			      const fits::table_def_all defs[], bool ascii )
{
    if ( index0 < 0 || this->num_hdus_rec < index0 ) {
	err_report(__FUNCTION__,"WARNING","invalid index0; ignored");
	return *this;
    }

    if ( ascii == true ) this->insert(index0,FITS::ASCII_TABLE_HDU);
    else this->insert(index0,FITS::BINARY_TABLE_HDU);

    this->table(index0).init(defs);
    
    if ( ascii == true && this->hdutype(index0) == FITS::BINARY_TABLE_HDU ) {
	err_report(__FUNCTION__,"WARNING",
	"Definition is not valid for ASCII table; a BINARY table is inserted");
    }

    /* index_rec �򹹿� */
    if ( extname != NULL ) {
	this->assign_extname( index0, extname );
	this->assign_extver( index0, extver );
    }

    return *this;
}

/**
 * @brief  ASCII Table �ޤ��� Binary Table HDU ������
 */
fitscc &fitscc::insert_table( const char *extname0, 
			      const char *extname, long long extver,
			      const fits::table_def defs[], bool ascii )
{
    long idx;

    if ( extname0 == NULL ) {
	err_report(__FUNCTION__,"WARNING","NULL extname0; ignored");
	return *this;
    }

    idx = this->index_rec.index(extname0,0);
    if ( idx < 0 ) {
	err_report1(__FUNCTION__,"WARNING",
		    "HDU `%s' is not found; ignored",extname0);
	return *this;
    }

    this->insert_table(idx, extname, extver, defs, ascii);

    return *this;
}

/**
 * @brief  ASCII Table �ޤ��� Binary Table HDU ������
 */
fitscc &fitscc::insert_table( const char *extname0, 
			      const char *extname, long long extver,
			      const fits::table_def_all defs[], bool ascii )
{
    long idx;

    if ( extname0 == NULL ) {
	err_report(__FUNCTION__,"WARNING","NULL extname0; ignored");
	return *this;
    }

    idx = this->index_rec.index(extname0,0);
    if ( idx < 0 ) {
	err_report1(__FUNCTION__,"WARNING",
		    "HDU `%s' is not found; ignored",extname0);
	return *this;
    }

    this->insert_table(idx, extname, extver, defs, ascii);

    return *this;
}

/**
 * @brief  ASCII Table �ޤ��� Binary Table HDU �����������̥��֥������Ȥ򥳥ԡ�
 */
fitscc &fitscc::insert_table( long index0,
			      const char *extname, long long extver,
			      const fits_table &src )
{
    if ( index0 < 0 || this->num_hdus_rec < index0 ) {
	err_report(__FUNCTION__,"WARNING","invalid index0; ignored");
	return *this;
    }

    this->insert(index0,FITS::BINARY_TABLE_HDU);

    this->table(index0).init(src);
    
    /* index_rec �򹹿� */
    if ( extname != NULL ) {
	this->assign_extname( index0, extname );
	this->assign_extver( index0, extver );
    }

    return *this;
}

/**
 * @brief  ASCII Table �ޤ��� Binary Table HDU �����������̥��֥������Ȥ򥳥ԡ�
 */
fitscc &fitscc::insert_table( const char *extname0,
			      const char *extname, long long extver,
			      const fits_table &src )
{
    long idx;

    if ( extname0 == NULL ) {
	err_report(__FUNCTION__,"WARNING","NULL extname0; ignored");
	return *this;
    }

    idx = this->index_rec.index(extname0,0);
    if ( idx < 0 ) {
	err_report1(__FUNCTION__,"WARNING",
		    "HDU `%s' is not found; ignored",extname0);
	return *this;
    }

    this->insert_table(idx, extname, extver, src);

    return *this;
}

/**
 * @brief  ASCII Table �ޤ��� Binary Table HDU �����������̥��֥������Ȥ򥳥ԡ�
 */
fitscc &fitscc::insert_table( long index0, const fits_table &src )
{
    return this->insert_table(index0, (const char *)NULL, 0, src);
}

/**
 * @brief  ASCII Table �ޤ��� Binary Table HDU �����������̥��֥������Ȥ򥳥ԡ�
 */
fitscc &fitscc::insert_table( const char *extname0, const fits_table &src )
{
    return this->insert_table(extname0, (const char *)NULL, 0, src);
}

/**
 * @brief  HDU �κ��
 */
fitscc &fitscc::erase( long index0 )
{
    long i;
    tstring ename;

    if ( index0 < 0 || this->num_hdus_rec <= index0 ) {
	err_report1(__FUNCTION__,"WARNING",
		    "Invalid HDU index: %ld; ignored",index0);
	return *this;
    }

    /* Primary HDU �������褦�Ȥ��ơ����� Primary �ˤʤ�Τ� IMAGE �Ǥʤ�
       ���ϥ��顼�Ȥ��� */
    if ( index0 == 0 && 1 < this->num_hdus_rec && 
	 this->hdus_ptr_rec[1]->hdutype() != FITS::IMAGE_HDU ) {
	err_report(__FUNCTION__,"WARNING","Cannot erase Primary HDU");
	return *this;
    }

    /* index �Ϥ�ʤ��� */
    ename = this->hdus_ptr_rec[index0]->extname();
    if ( ename.cstr() != NULL ) {
	if ( ename.strcmp("Primary") != 0 || 0 < index0 ) {
	    if (this->index_rec.erase(ename.cstr(), index0) < 0){
		err_throw(__FUNCTION__,"FATAL",
			  "this->index_rec.erase() failed");
	    }
	}
    }
    for ( i=index0 + 1 ; i < this->num_hdus_rec ; i++ ) {
	ename = this->hdus_ptr_rec[i]->extname();
	if ( ename.cstr() != NULL ) {
	    if ( ename.strcmp("Primary") == 0 && i-1 == 0 ) {
		if (this->index_rec.erase(ename.cstr(), i) < 0) {
		    err_throw(__FUNCTION__,"FATAL",
			     "Internal ERROR: this->index_rec.erase() failed");
		}
	    }
	    else {
		if (this->index_rec.update(ename.cstr(), i, i-1) < 0) {
		    err_throw(__FUNCTION__,"FATAL",
			    "Internal ERROR: this->index_rec.update() failed");
		}
	    }
	}
    }

    /* ������� */
    if ( this->hdu_is_stack_rec.b(index0) == false ) {
	/* �ҡ��פξ��� delete */
	delete this->hdus_ptr_rec[index0];
    }
    else {
	/* �����å��ξ��Ͻ���� */
	this->hdus_ptr_rec[index0]->init();
    }
    /* �Ĥ�� */
    for ( i=index0 ; i + 1 < this->num_hdus_rec ; i++ ) {
	this->hdus_ptr_rec[i] = this->hdus_ptr_rec[i+1];
	this->hdu_is_stack_rec.b(i) = this->hdu_is_stack_rec.b(i+1);
    }
    this->hdus_ptr_rec[i] = NULL;

    this->num_hdus_rec --;

    try {
	this->hdus_rec.resize( this->num_hdus_rec + 1 );
    }
    catch (...) {
	err_throw(__FUNCTION__,"FATAL","hdus_rec.resize() failed");
    }

    try {
	this->hdu_is_stack_rec.resize( this->num_hdus_rec );
    }
    catch (...) {
	err_throw(__FUNCTION__,"FATAL","hdu_is_stack_rec.resize() failed");
    }

    return *this;
}

/**
 * @brief  HDU �κ��
 */
fitscc &fitscc::erase( const char *extname0 )
{
    long idx;

    if ( extname0 == NULL ) {
	err_report(__FUNCTION__,"WARNING","NULL extname0; ignored");
	return *this;
    }

    idx = this->index_rec.index(extname0,0);
    if ( idx < 0 ) {
	err_report1(__FUNCTION__,"WARNING",
		    "HDU `%s' is not found; ignored",extname0);
	return *this;
    }

    this->erase(idx);

    return *this;
}

/**
 * @brief  ���ꤵ�줿 HDU ̾���б����� HDU �ֹ���֤�
 */
long fitscc::index( const char *extname ) const
{
    long return_val = -1;

    if ( extname == NULL ) {
	goto quit;
    }

    return_val = this->index_rec.index(extname,0);

 quit:
    return return_val;
}

/**
 * @brief  printf()�ε�ˡ�ǻ��ꤵ�줿 HDU ̾���б����� HDU �ֹ���֤�
 */
long fitscc::indexf( const char *fmt, ... ) const
{
    long ret;
    va_list ap;
    va_start(ap,fmt);
    try {
	ret = this->vindexf(fmt,ap);
    }
    catch (...) {
	va_end(ap);
        err_throw(__FUNCTION__,"FATAL","this->vindexf() failed");
    }
    va_end(ap);
    return ret;
}

/**
 * @brief  printf()�ε�ˡ�ǻ��ꤵ�줿 HDU ̾���б����� HDU �ֹ���֤�
 */
long fitscc::vindexf( const char *fmt, va_list ap ) const
{
    tstring buf;

    if ( fmt == NULL ) return this->index(fmt);

    buf.vprintf(fmt,ap);
    return this->index(buf.cstr());
}

/**
 * @brief  fits_hdu ���֥������Ȥؤλ��Ȥ��֤�
 */
fits_hdu &fitscc::hdu( long index )
{
    if ( index < 0 || this->num_hdus_rec <= index ) {
	err_throw1(__FUNCTION__,"ERROR","Invalid HDU index: %ld",index);
    }
    return *(this->hdus_ptr_rec[index]);
}

/**
 * @brief  fits_hdu ���֥������Ȥؤλ��Ȥ��֤�
 */
fits_hdu &fitscc::hdu( const char *extname )
{
    long idx;
    if ( extname == NULL ) {
	err_throw(__FUNCTION__,"ERROR","NULL extname ... ?");
    }
    idx = this->index_rec.index(extname,0);
#if 1
    if ( idx < 0 ) {
	this->append_image( extname, 0, FITS::BYTE_T, 0 );
	idx = this->index_rec.index(extname,0);
    }
#endif
    if ( idx < 0 ) {
	err_throw1(__FUNCTION__,"ERROR","HDU `%s' is not found",extname);
    }
    return this->hdu(idx);
}

/**
 * @brief  fits_hdu ���֥������Ȥؤλ��Ȥ��֤� (printf()�ε�ˡ)
 */
fits_hdu &fitscc::hduf( const char *fmt, ... )
{
    va_list ap;
    va_start(ap,fmt);
    try {
	fits_hdu &ret = this->vhduf(fmt,ap);
	va_end(ap);
	return ret;
    }
    catch (...) {
	va_end(ap);
        err_throw(__FUNCTION__,"FATAL","this->vhduf() failed");
    }
}

/**
 * @brief  fits_hdu ���֥������Ȥؤλ��Ȥ��֤� (printf()�ε�ˡ)
 */
fits_hdu &fitscc::vhduf( const char *fmt, va_list ap )
{
    tstring buf;

    if ( fmt == NULL ) return this->hdu(fmt);

    buf.vprintf(fmt,ap);
    return this->hdu(buf.cstr());
}

#ifdef SLI__OVERLOAD_CONST_AT
/**
 * @brief  fits_hdu ���֥������Ȥؤλ��Ȥ��֤� (�ɼ�����)
 */
const fits_hdu &fitscc::hdu( long index ) const
{
    return this->hdu_cs(index);
}

/**
 * @brief  fits_hdu ���֥������Ȥؤλ��Ȥ��֤� (�ɼ�����)
 */
const fits_hdu &fitscc::hdu( const char *extname ) const
{
    return this->hdu_cs(extname);
}

/**
 * @brief  fits_hdu ���֥������Ȥؤλ��Ȥ��֤� (�ɼ����ѡ�printf()�ε�ˡ)
 */
const fits_hdu &fitscc::hduf( const char *fmt, ... ) const
{
    va_list ap;
    va_start(ap,fmt);
    try {
	const fits_hdu &ret = this->vhduf_cs(fmt,ap);
	va_end(ap);
	return ret;
    }
    catch (...) {
	va_end(ap);
        err_throw(__FUNCTION__,"FATAL","this->vhduf_cs() failed");
    }
}

/**
 * @brief  fits_hdu ���֥������Ȥؤλ��Ȥ��֤� (�ɼ����ѡ�printf()�ε�ˡ)
 */
const fits_hdu &fitscc::vhduf( const char *fmt, va_list ap ) const
{
    return this->vhduf_cs(fmt, ap);
}
#endif

/**
 * @brief  fits_hdu ���֥������Ȥؤλ��Ȥ��֤� (�ɼ����ѡ�printf()�ε�ˡ)
 */
const fits_hdu &fitscc::hdu_cs( long index ) const
{
    if ( index < 0 || this->num_hdus_rec <= index ) {
	err_throw1(__FUNCTION__,"ERROR","Invalid HDU index: %ld",index);
    }
    return *(this->hdus_ptr_rec[index]);
}

/**
 * @brief  fits_hdu ���֥������Ȥؤλ��Ȥ��֤� (�ɼ�����)
 */
const fits_hdu &fitscc::hdu_cs( const char *extname ) const
{
    long idx;
    if ( extname == NULL ) {
	err_throw(__FUNCTION__,"ERROR","NULL extname ... ?");
    }
    idx = this->index_rec.index(extname,0);
    return this->hdu_cs(idx);
}

/**
 * @brief  fits_hdu ���֥������Ȥؤλ��Ȥ��֤� (�ɼ����ѡ�printf()�ε�ˡ)
 */
const fits_hdu &fitscc::hduf_cs( const char *fmt, ... ) const
{
    va_list ap;
    va_start(ap,fmt);
    try {
	const fits_hdu &ret = this->vhduf_cs(fmt,ap);
	va_end(ap);
	return ret;
    }
    catch (...) {
	va_end(ap);
        err_throw(__FUNCTION__,"FATAL","this->vhduf_cs() failed");
    }
}

/**
 * @brief  fits_hdu ���֥������Ȥؤλ��Ȥ��֤� (�ɼ����ѡ�printf()�ε�ˡ)
 */
const fits_hdu &fitscc::vhduf_cs( const char *fmt, va_list ap ) const
{
    tstring buf;

    if ( fmt == NULL ) return this->hdu_cs(fmt);

    buf.vprintf(fmt,ap);
    return this->hdu_cs(buf.cstr());
}

/**
 * @brief  fits_image ���֥������Ȥؤλ��Ȥ��֤�
 */
fits_image &fitscc::image( long index )
{
    if ( index < 0 || this->num_hdus_rec <= index ) {
	err_throw1(__FUNCTION__,"ERROR","Invalid HDU index: %ld",index);
    }
    //fprintf(stderr,"debug: fitscc::image: hdus_rec[%ld]->hdutype() = %d\n",
    //	    index,this->hdus_ptr_rec[index]->hdutype());
    if ( this->hdus_ptr_rec[index]->hdutype() != FITS::IMAGE_HDU ) {
	err_throw1(__FUNCTION__,"ERROR","HDU %ld is not IMAGE HDU",index);
    }

    return *((fits_image *)(this->hdus_ptr_rec[index]));
}

/**
 * @brief  fits_image ���֥������Ȥؤλ��Ȥ��֤�
 */
fits_image &fitscc::image( const char *extname )
{
    long idx;
    if ( extname == NULL ) {
	err_throw(__FUNCTION__,"ERROR","NULL extname ... ?");
    }
    idx = this->index_rec.index(extname,0);
#if 1
    if ( idx < 0 ) {
	this->append_image( extname, 0, FITS::BYTE_T, 0 );
	idx = this->index_rec.index(extname,0);
    }
#endif
    if ( idx < 0 ) {
	err_throw1(__FUNCTION__,"ERROR","HDU `%s' is not found",extname);
    }
    return this->image(idx);
}

/**
 * @brief  fits_image ���֥������Ȥؤλ��Ȥ��֤� (printf()�ε�ˡ)
 */
fits_image &fitscc::imagef( const char *fmt, ... )
{
    va_list ap;
    va_start(ap,fmt);
    try {
	fits_image &ret = this->vimagef(fmt,ap);
	va_end(ap);
	return ret;
    }
    catch (...) {
	va_end(ap);
        err_throw(__FUNCTION__,"FATAL","this->vimagef() failed");
    }
}

/**
 * @brief  fits_image ���֥������Ȥؤλ��Ȥ��֤� (printf()�ε�ˡ)
 */
fits_image &fitscc::vimagef( const char *fmt, va_list ap )
{
    tstring buf;

    if ( fmt == NULL ) return this->image(fmt);

    buf.vprintf(fmt,ap);
    return this->image(buf.cstr());
}

#ifdef SLI__OVERLOAD_CONST_AT
/**
 * @brief  fits_image ���֥������Ȥؤλ��Ȥ��֤� (�ɼ�����)
 */
const fits_image &fitscc::image( long index ) const
{
    return this->image_cs(index);
}

/**
 * @brief  fits_image ���֥������Ȥؤλ��Ȥ��֤� (�ɼ�����)
 */
const fits_image &fitscc::image( const char *extname ) const
{
    return this->image_cs(extname);
}

/**
 * @brief  fits_image ���֥������Ȥؤλ��Ȥ��֤� (�ɼ����ѡ�printf()�ε�ˡ)
 */
const fits_image &fitscc::imagef( const char *fmt, ... ) const
{
    va_list ap;
    va_start(ap,fmt);
    try {
	const fits_image &ret = this->vimagef_cs(fmt,ap);
	va_end(ap);
	return ret;
    }
    catch (...) {
	va_end(ap);
        err_throw(__FUNCTION__,"FATAL","this->vimagef_cs() failed");
    }
}

/**
 * @brief  fits_image ���֥������Ȥؤλ��Ȥ��֤� (�ɼ����ѡ�printf()�ε�ˡ)
 */
const fits_image &fitscc::vimagef( const char *fmt, va_list ap ) const
{
    return this->vimagef_cs(fmt, ap);
}
#endif

/**
 * @brief  fits_image ���֥������Ȥؤλ��Ȥ��֤� (�ɼ�����)
 */
const fits_image &fitscc::image_cs( long index ) const
{
    if ( index < 0 || this->num_hdus_rec <= index ) {
	err_throw1(__FUNCTION__,"ERROR","Invalid HDU index: %ld",index);
    }
    if ( this->hdus_ptr_rec[index]->hdutype() != FITS::IMAGE_HDU ) {
	err_throw1(__FUNCTION__,"ERROR","HDU %ld is not IMAGE HDU",index);
    }

    return *((fits_image *)(this->hdus_ptr_rec[index]));
}

/**
 * @brief  fits_image ���֥������Ȥؤλ��Ȥ��֤� (�ɼ�����)
 */
const fits_image &fitscc::image_cs( const char *extname ) const
{
    long idx;
    if ( extname == NULL ) {
	err_throw(__FUNCTION__,"ERROR","NULL extname ... ?");
    }
    idx = this->index_rec.index(extname,0);
    return this->image_cs(idx);
}

/**
 * @brief  fits_image ���֥������Ȥؤλ��Ȥ��֤� (�ɼ����ѡ�printf()�ε�ˡ)
 */
const fits_image &fitscc::imagef_cs( const char *fmt, ... ) const
{
    va_list ap;
    va_start(ap,fmt);
    try {
	const fits_image &ret = this->vimagef_cs(fmt,ap);
	va_end(ap);
	return ret;
    }
    catch (...) {
	va_end(ap);
        err_throw(__FUNCTION__,"FATAL","this->vimagef_cs() failed");
    }
}

/**
 * @brief  fits_image ���֥������Ȥؤλ��Ȥ��֤� (�ɼ����ѡ�printf()�ε�ˡ)
 */
const fits_image &fitscc::vimagef_cs( const char *fmt, va_list ap ) const
{
    tstring buf;

    if ( fmt == NULL ) return this->image_cs(fmt);

    buf.vprintf(fmt,ap);
    return this->image_cs(buf.cstr());
}

/**
 * @brief  fits_table ���֥������Ȥؤλ��Ȥ��֤�
 */
fits_table &fitscc::table( long index )
{
    if ( index < 0 || this->num_hdus_rec <= index ) {
	err_throw1(__FUNCTION__,"ERROR","Invalid HDU index: %ld",index);
    }
    if ( this->hdus_ptr_rec[index]->hdutype() != FITS::BINARY_TABLE_HDU &&
	 this->hdus_ptr_rec[index]->hdutype() != FITS::ASCII_TABLE_HDU ) {
	err_throw1(__FUNCTION__,"ERROR","HDU %ld is not TABLE HDU",index);
    }

    return *((fits_table *)(this->hdus_ptr_rec[index]));
}

/**
 * @brief  fits_table ���֥������Ȥؤλ��Ȥ��֤�
 */
fits_table &fitscc::table( const char *extname )
{
    long idx;
    if ( extname == NULL ) {
	err_throw(__FUNCTION__,"ERROR","NULL extname ... ?");
    }
    idx = this->index_rec.index(extname,0);
#if 1
    if ( idx < 0 ) {
	this->append_table( extname, 0, (fits::table_def *)NULL );
	idx = this->index_rec.index(extname,0);
    }
#endif
    if ( idx < 0 ) {
	err_throw1(__FUNCTION__,"ERROR","HDU `%s' is not found",extname);
    }
    return this->table(idx);
}

/**
 * @brief  fits_table ���֥������Ȥؤλ��Ȥ��֤� (printf()�ε�ˡ)
 */
fits_table &fitscc::tablef( const char *fmt, ... )
{
    va_list ap;
    va_start(ap,fmt);
    try {
	fits_table &ret = this->vtablef(fmt,ap);
	va_end(ap);
	return ret;
    }
    catch (...) {
	va_end(ap);
        err_throw(__FUNCTION__,"FATAL","this->vtablef() failed");
    }
}

/**
 * @brief  fits_table ���֥������Ȥؤλ��Ȥ��֤� (printf()�ε�ˡ)
 */
fits_table &fitscc::vtablef( const char *fmt, va_list ap )
{
    tstring buf;

    if ( fmt == NULL ) return this->table(fmt);

    buf.vprintf(fmt,ap);
    return this->table(buf.cstr());
}

#ifdef SLI__OVERLOAD_CONST_AT
/**
 * @brief  fits_table ���֥������Ȥؤλ��Ȥ��֤� (�ɼ�����)
 */
const fits_table &fitscc::table( long index ) const
{
    return this->table_cs(index);
}

/**
 * @brief  fits_table ���֥������Ȥؤλ��Ȥ��֤� (�ɼ�����)
 */
const fits_table &fitscc::table( const char *extname ) const
{
    return this->table_cs(extname);
}

/**
 * @brief  fits_table ���֥������Ȥؤλ��Ȥ��֤� (�ɼ����ѡ�printf()�ε�ˡ)
 */
const fits_table &fitscc::tablef( const char *fmt, ... ) const
{
    va_list ap;
    va_start(ap,fmt);
    try {
	const fits_table &ret = this->vtablef_cs(fmt,ap);
	va_end(ap);
	return ret;
    }
    catch (...) {
	va_end(ap);
        err_throw(__FUNCTION__,"FATAL","this->vtablef_cs() failed");
    }
}

/**
 * @brief  fits_table ���֥������Ȥؤλ��Ȥ��֤� (�ɼ����ѡ�printf()�ε�ˡ)
 */
const fits_table &fitscc::vtablef( const char *fmt, va_list ap ) const
{
    return this->vtablef_cs(fmt, ap);
}
#endif

/**
 * @brief  fits_table ���֥������Ȥؤλ��Ȥ��֤� (�ɼ�����)
 */
const fits_table &fitscc::table_cs( long index ) const
{
    if ( index < 0 || this->num_hdus_rec <= index ) {
	err_throw1(__FUNCTION__,"ERROR","Invalid HDU index: %ld",index);
    }
    if ( this->hdus_ptr_rec[index]->hdutype() != FITS::BINARY_TABLE_HDU &&
	 this->hdus_ptr_rec[index]->hdutype() != FITS::ASCII_TABLE_HDU ) {
	err_throw1(__FUNCTION__,"ERROR","HDU %ld is not TABLE HDU",index);
    }

    return *((fits_table *)(this->hdus_ptr_rec[index]));
}

/**
 * @brief  fits_table ���֥������Ȥؤλ��Ȥ��֤� (�ɼ�����)
 */
const fits_table &fitscc::table_cs( const char *extname ) const
{
    long idx;
    if ( extname == NULL ) {
	err_throw(__FUNCTION__,"ERROR","NULL extname ... ?");
    }
    idx = this->index_rec.index(extname,0);
    return this->table_cs(idx);
}

/**
 * @brief  fits_table ���֥������Ȥؤλ��Ȥ��֤� (�ɼ����ѡ�printf()�ε�ˡ)
 */
const fits_table &fitscc::tablef_cs( const char *fmt, ... ) const
{
    va_list ap;
    va_start(ap,fmt);
    try {
	const fits_table &ret = this->vtablef_cs(fmt,ap);
	va_end(ap);
	return ret;
    }
    catch (...) {
	va_end(ap);
        err_throw(__FUNCTION__,"FATAL","this->vtablef_cs() failed");
    }
}

/**
 * @brief  fits_table ���֥������Ȥؤλ��Ȥ��֤� (�ɼ����ѡ�printf()�ε�ˡ)
 */
const fits_table &fitscc::vtablef_cs( const char *fmt, va_list ap ) const
{
    tstring buf;

    if ( fmt == NULL ) return this->table_cs(fmt);

    buf.vprintf(fmt,ap);
    return this->table_cs(buf.cstr());
}

/**
 * @brief  HDU �μ���(FITS::IMAGE_HDU��FITS::BINARY_TABLE_HDU ��)�����
 */
int fitscc::hdutype( long index ) const
{
    if ( index < 0 || this->num_hdus_rec <= index ) return -1;
    else return this->hdu_cs(index).hdutype();
}

/**
 * @brief  HDU �μ���(FITS::IMAGE_HDU��FITS::BINARY_TABLE_HDU ��)�����
 */
int fitscc::hdutype( const char *hduname ) const
{
    return this->hdutype( this->index_rec.index(hduname,0) );
}

/**
 * @brief  HDU �μ���(FITS::IMAGE_HDU��FITS::BINARY_TABLE_HDU ��)�����
 *
 * @note  fitscc::hdutype() �Ȥΰ㤤�Ϥ���ޤ���
 */
int fitscc::exttype( long index ) const
{
    if ( index < 0 || this->num_hdus_rec <= index ) return -1;
    else return this->hdu_cs(index).hdutype();
}

/**
 * @brief  HDU �μ���(FITS::IMAGE_HDU��FITS::BINARY_TABLE_HDU ��)�����
 *
 * @note  fitscc::hdutype() �Ȥΰ㤤�Ϥ���ޤ���
 */
int fitscc::exttype( const char *extname ) const
{
    return this->exttype( this->index_rec.index(extname,0) );
}

/**
 * @brief  HDU ��̾��(EXTNAME)�����
 */
const char *fitscc::hduname( long index ) const
{
    return this->extname(index);
}

/**
 * @brief  HDU ��̾��(EXTNAME)�����
 *
 * @note  fitscc::hduname() �Ȥΰ㤤�Ϥ���ޤ���
 */
const char *fitscc::extname( long index ) const
{
    if ( index < 0 || this->num_hdus_rec <= index ) return NULL;

    if ( index == 0 && this->hdu_cs(index).extname() == NULL )
	return "Primary";
    else
	return this->hdu_cs(index).extname();
}

/**
 * @brief  HDU �ΥС������(EXTVER)�����
 */
long long fitscc::hduver( long index ) const
{
    return this->extver(index);
}

/**
 * @brief  HDU �ΥС������(EXTVER)�����
 *
 * @note  fitscc::hduver() �Ȥΰ㤤�Ϥ���ޤ���
 */
long long fitscc::extver( long index ) const
{
    if ( index < 0 || this->num_hdus_rec <= index ) return 0;
    else return this->hdu_cs(index).extver();
}

/**
 * @brief  HDU �ΥС������(EXTVER)�����
 */
long long fitscc::hduver( const char *hduname ) const
{
    return this->extver(hduname);
}

/**
 * @brief  HDU �ΥС������(EXTVER)�����
 *
 * @note  fitscc::hduver() �Ȥΰ㤤�Ϥ���ޤ���
 */
long long fitscc::extver( const char *extname ) const
{
    return this->extver( this->index_rec.index(extname,0) );
}

/**
 * @brief  HDU �Υ�٥�(EXTLEVEL)�����
 */
long long fitscc::hdulevel( long index ) const
{
    return this->extlevel(index);
}

/**
 * @brief  HDU �Υ�٥�(EXTLEVEL)�����
 *
 * @note  fitscc::hdulevel() �Ȥΰ㤤�Ϥ���ޤ���
 */
long long fitscc::extlevel( long index ) const
{
    if ( index < 0 || this->num_hdus_rec <= index ) return 0;
    else return this->hdu_cs(index).extlevel();
}

/**
 * @brief  HDU �Υ�٥�(EXTLEVEL)�����
 */
long long fitscc::hdulevel( const char *hduname ) const
{
    return this->extlevel(hduname);
}

/**
 * @brief  HDU �Υ�٥�(EXTLEVEL)�����
 *
 * @note  fitscc::hdulevel() �Ȥΰ㤤�Ϥ���ޤ���
 */
long long fitscc::extlevel( const char *extname ) const
{
    return this->extlevel( this->index_rec.index(extname,0) );
}

/**
 * @brief  HDU �ΥС������(EXTVER)�����åȤ���Ƥ��뤫���֤�
 */
bool fitscc::hduver_is_set( long index ) const
{
    return this->extver_is_set(index);
}

/**
 * @brief  HDU �ΥС������(EXTVER)�����åȤ���Ƥ��뤫���֤�
 *
 * @note  fitscc::hduver_is_set() �Ȥΰ㤤�Ϥ���ޤ���
 */
bool fitscc::extver_is_set( long index ) const
{
    if ( index < 0 || this->num_hdus_rec <= index ) {
	return false;
    }
    return this->hdu_cs(index).extver_is_set();
}

/**
 * @brief  HDU �ΥС������(EXTVER)�����åȤ���Ƥ��뤫���֤�
 */
bool fitscc::hduver_is_set( const char *hduname ) const
{
    return this->extver_is_set(hduname);
}

/**
 * @brief  HDU �ΥС������(EXTVER)�����åȤ���Ƥ��뤫���֤�
 *
 * @note  fitscc::hduver_is_set() �Ȥΰ㤤�Ϥ���ޤ���
 */
bool fitscc::extver_is_set( const char *extname ) const
{
    return this->extver_is_set( this->index_rec.index(extname,0) );
}

/**
 * @brief  HDU �Υ�٥�(EXTLEVEL)�����åȤ���Ƥ��뤫���֤�
 */
bool fitscc::hdulevel_is_set( long index ) const
{
    return this->extlevel_is_set(index);
}

/**
 * @brief  HDU �Υ�٥�(EXTLEVEL)�����åȤ���Ƥ��뤫���֤�
 *
 * @note  fitscc::hdulevel_is_set() �Ȥΰ㤤�Ϥ���ޤ���
 */
bool fitscc::extlevel_is_set( long index ) const
{
    if ( index < 0 || this->num_hdus_rec <= index ) {
	return false;
    }
    return this->hdu_cs(index).extlevel_is_set();
}

/**
 * @brief  HDU �Υ�٥�(EXTLEVEL)�����åȤ���Ƥ��뤫���֤�
 */
bool fitscc::hdulevel_is_set( const char *hduname ) const
{
    return this->extlevel_is_set(hduname);
}

/**
 * @brief  HDU �Υ�٥�(EXTLEVEL)�����åȤ���Ƥ��뤫���֤�
 *
 * @note  fitscc::hdulevel_is_set() �Ȥΰ㤤�Ϥ���ޤ���
 */
bool fitscc::extlevel_is_set( const char *extname ) const
{
    return this->extlevel_is_set( this->index_rec.index(extname,0) );
}

/**
 * @brief  HDU ��̾��(EXTNAME)������
 */
fitscc &fitscc::assign_hduname( long index, const char *hduname )
{
    return this->assign_extname(index,hduname);
}

/**
 * @brief  HDU ��̾��(EXTNAME)������
 *
 * @note  fitscc::assign_hduname() �Ȥΰ㤤�Ϥ���ޤ���
 */
fitscc &fitscc::assign_extname( long index, const char *extname )
{
    tstring ename;
    //err_report(__FUNCTION__,"DEBUG","called!!");
    if ( index < 0 || this->num_hdus_rec <= index ) {
	err_report1(__FUNCTION__,
		    "WARNING","Invalid HDU index: %ld; ignored",index);
	return *this;
    }

    ename = this->hdus_ptr_rec[index]->extname();
    if ( ename.cstr() != NULL ) {
	if ( ename.strcmp("Primary") != 0 || 0 < index ) {
	    ssize_t idx;
	    int ii = 0;
	    while ( 0 <= (idx=this->index_rec.index(ename.cstr(),ii)) ) {
		if ( idx == index ) {
		    if ( this->index_rec.erase(ename.cstr(), index) < 0 ) {
			err_throw(__FUNCTION__,"FATAL",
				  "this->index_rec.erase() failed");
		    }
		    break;
		}
		ii++;
	    }
	}
    }

    this->hdus_ptr_rec[index]->_assign_extname(extname);

    ename = this->hdus_ptr_rec[index]->extname();
    if ( ename.cstr() != NULL ) {
	if ( ename.strcmp("Primary") != 0 || 0 < index ) {
	    if ( this->index_rec.append(ename.cstr(), index) < 0 ) {
		err_throw(__FUNCTION__,"FATAL",
			  "this->index_rec.append() failed");
	    }
	}
    }
    
    return *this;
}

/**
 * @brief  HDU �ΥС������(EXTVER)������
 */
fitscc &fitscc::assign_hduver( long index, long long hduver )
{
    return this->assign_extver(index,hduver);
}

/**
 * @brief  HDU �ΥС������(EXTVER)������
 *
 * @note  fitscc::assign_hduver() �Ȥΰ㤤�Ϥ���ޤ���
 */
fitscc &fitscc::assign_extver( long index, long long extver )
{
    if ( index < 0 || this->num_hdus_rec <= index ) {
	err_report1(__FUNCTION__,"WARNING",
		    "Invalid HDU index: %ld; ignored",index);
	return *this;
    }

    this->hdus_ptr_rec[index]->assign_extver(extver);

    return *this;
}

/**
 * @brief  HDU �Υ�٥�(EXTLEVEL)������
 */
fitscc &fitscc::assign_hdulevel( long index, long long hdulevel )
{
    return this->assign_extlevel(index,hdulevel);
}

/**
 * @brief  HDU �Υ�٥�(EXTLEVEL)������
 *
 * @note  fitscc::assign_hdulevel() �Ȥΰ㤤�Ϥ���ޤ���
 */
fitscc &fitscc::assign_extlevel( long index, long long extlevel )
{
    if ( index < 0 || this->num_hdus_rec <= index ) {
	err_report1(__FUNCTION__,"WARNING",
		    "Invalid HDU index: %ld; ignored",index);
	return *this;
    }

    this->hdus_ptr_rec[index]->assign_extlevel(extlevel);

    return *this;
}

/**
 * @brief  �إå��� FMTTYPE ���ͤ����
 */
const char *fitscc::fmttype() const
{
#ifdef FMTTYPE_IS_SPECIAL
#else
    if ( 0 < this->length() && 0 < this->hdu(0L).header_index("FMTTYPE") ) {
	return this->hdu(0L).header("FMTTYPE").svalue();
    }
#endif
    return this->fmttype_rec.cstr();
}

/**
 * @brief  �إå��� FTYPEVER ���ͤ������Ǽ���
 */
long long fitscc::ftypever() const
{
#ifdef FMTTYPE_IS_SPECIAL
#else
    if ( 0 < this->length() && 0 < this->hdu(0L).header_index("FTYPEVER") ) {
	return this->hdu(0L).header("FTYPEVER").llvalue();
    }
#endif
    return this->ftypever_rec.atoll();
}

/**
 * @brief  �إå��� FTYPEVER ���ͤ�ʸ����Ǽ���
 */
const char *fitscc::ftypever_value() const
{
#ifdef FMTTYPE_IS_SPECIAL
#else
    if ( 0 < this->length() && 0 < this->hdu(0L).header_index("FTYPEVER") ) {
	return this->hdu(0L).header("FTYPEVER").svalue();
    }
#endif
    return this->ftypever_rec.cstr();
}

/**
 * @brief  �إå��� FMTTYPE��FTYPEVER ���ͤ�����
 */
fitscc &fitscc::assign_fmttype( const char *fmttype, long long ftypever )
{
    if ( fmttype != NULL ) {
	this->fmttype_rec.assign(fmttype).strtrim(" ");
    }
    if ( ftypever != INDEF_LLONG && ftypever != FITS::INDEF ) {
	this->ftypever_rec.assignf("%lld",ftypever);
    }

    if ( 0 < this->length() ) {
	fits_hdu &pri = this->hdu(0L);
	pri.setup_sys_header();
    }

    return *this;
}

/**
 * @brief  �إå��� FMTTYPE ���ͤ�����
 */
fitscc &fitscc::assign_fmttype( const char *fmttype )
{
    if ( fmttype != NULL ) {
	this->fmttype_rec.assign(fmttype).strtrim(" ");
    }

    if ( 0 < this->length() ) {
	fits_hdu &pri = this->hdu(0L);
	pri.setup_sys_header();
    }

    return *this;
}

/**
 * @brief  �إå��� FTYPEVER ���ͤ�����
 */
fitscc &fitscc::assign_ftypever( long long ftypever )
{
    if ( ftypever != INDEF_LLONG && ftypever != FITS::INDEF ) {
	this->ftypever_rec.assignf("%lld",ftypever);
    }

    if ( 0 < this->length() ) {
	fits_hdu &pri = this->hdu(0L);
	pri.setup_sys_header();
    }

    return *this;
}

/* protected member functions */

/**
 * @brief  ���饹�ηѾ���٥��1�ĥ��å�
 *
 * @note   ���Υ��дؿ��� protected �Ǥ���
 */
int fitscc::increase_classlevel()
{
    this->classlevel_rec ++;
    return this->classlevel_rec;
}

typedef struct _hdu_exp_info {
    tarray_tstring hdu_type;	/* HDU info */
    tarray_tstring hdu_name;
    tarray_tstring hdu_ver;
    mdarray_long cnt_to_find;	/* how many HDUs to be found for each spec  */
    mdarray_bool digit_flag;	/* true for elem that expresses HDU number  */
    mdarray_bool del_flag;	/* true for columns to be deleted           */
} hdu_exp_info;

/**
 * @brief  ���ȥ꡼�फ�� FITS �ǡ������ɤ߹���
 *
 * @note   ���Υ��дؿ��� protected �Ǥ���
 */
ssize_t fitscc::fits_load( cstreamio &sref, const size_t *max_bytes_ptr )
{
    ssize_t return_sz = -1;
    fits_hdu tmp_hdu;
    ssize_t sz;
    size_t all_sz = 0;
    long hdu_cnt = 0;
    long header_cnt = 0;
    hdu_exp_info hdus_spec;
    tarray_tstring tmp_arr;
    size_t i;

    this->_init();

    /* extract HDU spec info */
    for ( i=0 ; i < this->hdus_to_read_rec.length() ; i++ ) {
	tstring tmp_spec;
	size_t len, nextpos;
	ssize_t pos;
	long count_to_find = -1;
	tmp_spec = this->hdus_to_read_rec[i];
	/* check '-' sign */
	if ( tmp_spec.cchr(0) == '-' ) {
	    hdus_spec.del_flag[i] = true;
	    tmp_spec.erase(0,1).ltrim();
	}
	else {
	    hdus_spec.del_flag[i] = false;
	}
	/* check {...} */
	pos = tmp_spec.rfind_quoted("{}", '\\', &len, &nextpos);
	if ( 0 <= pos && (size_t)pos + len == tmp_spec.length() ) {
	    size_t pos1 = pos + 1 + tmp_spec.strspn(pos + 1," ");
	    if ( 0 < tmp_spec.strspn(pos1,"0123456789") ) {
		count_to_find = tmp_spec.atol(pos1);
	    }
	    tmp_spec.erase(pos,len);
	}
	hdus_spec.cnt_to_find[i] = count_to_find;
	/* <b::EVENT::1>  =>  <b> <EVENT> <1> */
	tmp_arr.explode(tmp_spec.cstr(), "::", true, "'\"", '\\', false).trim();
	if ( tmp_arr.length() == 1 ) {
	    hdus_spec.hdu_type[i].assign("");
	    hdus_spec.hdu_name[i] = tmp_arr[0];
	    hdus_spec.hdu_ver[i].assign("");
	}
	else if ( tmp_arr.length() == 2 ) {
	    hdus_spec.hdu_type[i].assign("");
	    hdus_spec.hdu_name[i] = tmp_arr[0];
	    hdus_spec.hdu_ver[i] = tmp_arr[1];
	}
	else {
	    hdus_spec.hdu_type[i] = tmp_arr[0];
	    hdus_spec.hdu_type[i].tolower();
	    hdus_spec.hdu_name[i] = tmp_arr[1];
	    hdus_spec.hdu_ver[i] = tmp_arr[2];
	}
	/* hdu_name[] ���������ɤ��������å����� */
	if ( hdus_spec.hdu_name[i].strspn("0123456789") == 
	     hdus_spec.hdu_name[i].length() ) {
	    hdus_spec.digit_flag[i] = true;
	}
	else hdus_spec.digit_flag[i] = false;
	/* �������ơ�������õ��                 */
	/* (strmatch() �ǻȤ������ escapeʸ���ϻĤ�) */
	hdus_spec.hdu_name[i].erase_quotes(0, "'\"", '\\', false, &len, true);
    }

    do {

      long hdu_idx = -1;
      //err_report1(__FUNCTION__,"DEBUG","header_cnt: [%ld]",(long)header_cnt);

      /* �إå������ɤ� */
      if ( max_bytes_ptr != NULL )
	  sz = tmp_hdu.read_stream(sref, *max_bytes_ptr - all_sz);
      else
	  sz = tmp_hdu.read_stream(sref);
      if ( sz < 0 ) {
	  err_report(__FUNCTION__,"ERROR","tmp_hdu.read_stream() failed");
	  goto quit;
      }
      header_cnt ++;				/* number of read header */
      all_sz += sz;

      /* �ǡ��������ɤ� */
      if ( (ssize_t)FITS::FILE_RECORD_UNIT <= sz ) {
	  bool go_read_data = true;		/* �ɤ�٤�HDU?         */
	  bool last_hdu_to_read = false;	/* �ɤ�٤��Ǹ��HDU?   */
	  const char *section_info = NULL;
	  size_t go_on_i = 0;			/* �ɤ� i �ǥҥåȤ���? */
	  /* ��ʬ�ɤߤΤ��������                                  */
	  /* hdus_spec �����ꤵ��Ƥ����硤�ɤ�٤� HDU ����� */
	  /* (data ���򥹥��åפ��뤫���ɤ߹��फ)                 */
	  if ( 0 < hdus_spec.hdu_name.length() ) {
	      bool go_by_digit = false;		/* HDU�ֹ�ǥҥå�?  */
	      tstring c_hduname(true);		/* ���ߤ�HDU̾       */
	      tstring c_hduver(true);		/* ���ߤ�HDU version */
	      int c_hdutype;			/* ���ߤ�HDU type    */
	      long ix;
	      /* get values of EXTNAME and EXTVER */
	      ix = tmp_hdu.header_index("EXTNAME");
	      if (0 <= ix) c_hduname.assign(tmp_hdu.header("EXTNAME").svalue());
	      ix = tmp_hdu.header_index("EXTVER");
	      if (0 <= ix) c_hduver.assign(tmp_hdu.header("EXTVER").svalue());
	      c_hdutype = tmp_hdu.hdutype_on_header();
	      /* check */
	      go_read_data = false;
	      for ( i=0 ; i < hdus_spec.hdu_name.length() ; i++ ) {
		  bool c_del_flag = hdus_spec.del_flag[i];
		  /* ��Ƭ��'-'��������ϡ���˻�Ԥ���                    */
		  /* ��Ƭ��'-'��̵�����ϡ�go_read_data��false�ʤ��Ԥ��� */
		  if ( c_del_flag == true || 
		       ( c_del_flag == false && go_read_data == false ) ) {
		      const tstring &spec_hdu_name = hdus_spec.hdu_name[i];
		      bool match_ok = false;	   /* �ޥå�������?          */
		      bool match_by_digit = false; /* HDU�ֹ�ǥޥå�������? */
		      /* check EXTNAME */
		      if ( c_hduname.strmatch(spec_hdu_name.cstr()) == 0 ) {
			  match_ok = true;
		      }
		      if ( hdus_spec.digit_flag[i] == true ) {
			  if ( spec_hdu_name.atol() + 1 == header_cnt ) {
			      match_ok = true;
			      match_by_digit = true;
			  }
		      }
		      if ( spec_hdu_name.strcmp("Primary") == 0 ) {
		      if ( header_cnt == 1 ) match_ok = true;
		      }
		      /* check EXTVER */
		      if ( match_ok == true && 
			   0 < hdus_spec.hdu_ver[i].length() ) {
			  if (c_hduver.strmatch(hdus_spec.hdu_ver[i].cstr()) != 0) {
			      match_ok = false;
			  }
		      }
		      /* check HDU type */
		      if ( match_ok == true && 
			   0 < hdus_spec.hdu_type[i].length() ) {
			  const tstring &spec_hdu_type = hdus_spec.hdu_type[i];
			  if ( spec_hdu_type.at(0) == 'i' ) {
			      if ( c_hdutype != FITS::IMAGE_HDU ) {
				  match_ok = false;
			      }
			  }
			  else if ( spec_hdu_type.at(0) == 'b' ) {
			      if ( c_hdutype != FITS::BINARY_TABLE_HDU ) {
				  match_ok = false;
			      }
			  }
			  else if ( spec_hdu_type.at(0) == 'a' ) {
			      if ( c_hdutype != FITS::ASCII_TABLE_HDU ) {
				  match_ok = false;
			      }
			  }
			  else if ( spec_hdu_type.at(0) != '*' ) {
			      match_ok = false;
			  }
		      }
		      if ( c_del_flag == false ) {
			  if ( match_ok == true && hdus_spec.cnt_to_find[i] != 0 ) {
			      go_read_data = true;
			      if ( 2 <= this->section_to_read_rec[i].length() ) {
				  section_info = this->section_to_read_rec.cstr(i);
			      }
			      else section_info = NULL;
			      go_by_digit = match_by_digit;
			      go_on_i = i;
			  }
		      }
		      else {
			  /* ��Ƭ��'-'�������� */
			  if ( match_ok == true ) {
			      go_read_data = false;
			  }
			  else {
			      /* �ǽ��'-'��������ϡ���������ν���Ȥ��� */
			      if ( i == 0 && hdus_spec.cnt_to_find[i] != 0 ) {
				  go_read_data = true;
				  if ( 2 <= this->section_to_read_rec[i].length() ) {
				      section_info = this->section_to_read_rec.cstr(i);
				  }
				  else section_info = NULL;
				  go_on_i = i;
			      }
			  }
		      }
		  }
	      }
	      /* �ҥåȤ�����硤���Ĥ���٤�HDU�θĿ��򸺤餹 */
	      if ( go_read_data == true ) {
		  if ( go_by_digit == true ) {
		      /* HDU�ֹ�ǥݥ��ƥ��֥ҥåȤξ�������̵�Ѥ�0�ˤ��� */
		      hdus_spec.cnt_to_find[go_on_i] = 0;
		  }
		  else {
		      /* HDU̾�ǥҥåȤ�����硤������꤬������ϸ��餹 */
		      if ( 0 < hdus_spec.cnt_to_find[go_on_i] ) {
			  hdus_spec.cnt_to_find[go_on_i] --;
		      }
		  }
		  /* ���줬�Ǹ���ɤ�٤� HDU ���ɤ���Ƚ�ꤹ�� */
		  for ( i=0 ; i < hdus_spec.hdu_name.length() ; i++ ) {
		      if ( hdus_spec.cnt_to_find[i] != 0 ) break;
		  }
		  if ( i == hdus_spec.hdu_name.length() ) {
		      last_hdu_to_read = true;
		  }
	      }
	  }
	  /* �ɤ����Ф� */
	  if ( go_read_data == false ) {
	      if ( max_bytes_ptr != NULL )
		  sz = tmp_hdu.skip_data_stream(sref, *max_bytes_ptr - all_sz);
	      else
		  sz = tmp_hdu.skip_data_stream(sref);
	      if ( sz < 0 ) {
		  err_report(__FUNCTION__,"ERROR",
			     "tmp_hdu.skip_data_stream() failed");
		  goto quit;
	      }
	      all_sz += sz;
	  }
	  /* �ɤ� */
	  else {
	      int hdutype = tmp_hdu.hdutype_on_header();
	      this->append(hdutype);
	      hdu_idx = this->length() - 1;
	      if ( hdutype == FITS::IMAGE_HDU ) {
		  if ( max_bytes_ptr != NULL )
		      sz = this->image(hdu_idx).read_stream(&tmp_hdu,sref,
						 section_info,last_hdu_to_read,
						 *max_bytes_ptr - all_sz);
		  else
		      sz = this->image(hdu_idx).read_stream(&tmp_hdu,sref,
						section_info,last_hdu_to_read);
		  //err_report1(__FUNCTION__,"DEBUG",
		  //		"image().read_stream sz = %zd",sz);
		  if ( sz < 0 ) {
		      err_report1(__FUNCTION__,"ERROR",
				  "image(%ld).read_stream() failed",hdu_idx);
		      goto quit;
		  }
	      }
	      else if ( hdutype == FITS::BINARY_TABLE_HDU ||
			hdutype == FITS::ASCII_TABLE_HDU ) {
		  if ( max_bytes_ptr != NULL )
		      sz = this->table(hdu_idx).read_stream(&tmp_hdu,sref,
						 section_info,last_hdu_to_read,
						 *max_bytes_ptr - all_sz);
		  else
		      sz = this->table(hdu_idx).read_stream(&tmp_hdu,sref,
						section_info,last_hdu_to_read);
		  //err_report1(__FUNCTION__,"DEBUG",
		  //		"table().read_stream sz = %zd",sz);
		  if ( sz < 0 ) {
		      err_report1(__FUNCTION__,"ERROR",
				  "table(%ld).read_stream() failed",hdu_idx);
		      goto quit;
		  }
	      }
	      else {
		  err_report1(__FUNCTION__,"ERROR","Unsupported HDU type: %d",
			      hdutype);
		  goto quit;
	      }
	      /* index_rec �򹹿� */
	      if ( tmp_hdu.extname() != NULL ) 
		  this->assign_extname( hdu_idx, tmp_hdu.extname() );
	      if ( tmp_hdu.extver_is_set() == true )
		  this->assign_extver( hdu_idx, tmp_hdu.extver() );
	      if ( tmp_hdu.extlevel_is_set() == true )
		  this->assign_extlevel( hdu_idx, tmp_hdu.extlevel() );
	      //fprintf(stderr,"debug1: sz = %lld\n",sz);
	      all_sz += sz;
	  }
	  /* Register FMTTYPE and FTYPEVER */
	  if ( hdu_idx == 0 ) {
#ifdef FMTTYPE_IS_SPECIAL
	      long idx;
	      idx = this->hdu(hdu_idx).sysheader_index("FMTTYPE");
	      if ( 0 <= idx ) {
		  const char *v = 
		      this->hdu(hdu_idx).header_rec.record(idx).svalue();
		  this->fmttype_rec.assign(v);
	      }
	      idx = this->hdu(hdu_idx).sysheader_index("FTYPEVER");
	      if ( 0 <= idx ) {
		  this->ftypever_rec = 
		      this->hdu(hdu_idx).header_rec.record(idx).svalue();
	      }
#endif
	  }
	  /* �ɤ�٤��Ǹ�� HDU �ʤ����Ǥ���̵�̤� I/O �򸺤餹 */
	  if ( last_hdu_to_read == true ) {
	      break;
	  }
      }
      else {
	  break;
      }
      //if ( max_bytes_ptr != NULL ) {
      //  if ( (*max_bytes_ptr) < FITS::FILE_RECORD_UNIT + all_sz ) {
      //      break;
      //  }
      //}
      hdu_cnt ++;
    } while ( 1 );

    return_sz = all_sz;
 quit:
    return return_sz;
}

/**
 * @brief  �Ƶ��ƤӽФ��ˤ�ꡤ���ȥ꡼�फ�� FITS �ƥ�ץ졼�Ȥ��ɤ߹���
 *
 *  read_stream() ����θƤӽФ����ѤǤ���<br>
 *  �ƤӽФ����� this->template_buffer, this->template_buffer_idx �򥻥å�
 *  ���Ƥ���ɬ�פ�����ޤ���<br>
 *  �ƤӽФ�����ˤϡ�this->template_buffer, this->template_buffer_idx ��
 *  ��������뤳�ȡ�
 *
 * @note   ���Υ��дؿ��� protected �Ǥ���
 */
int fitscc::template_load_recursively( int flags, const char *path,
				       const char *base_dir )
{
    int return_status = -1;
    digeststreamio fin;
    tstring base_directory, path_to_open;
    int status;

    if ( path == NULL ) goto quit;

    /* 1st case */
    if ( base_dir == NULL ) {
	ssize_t pos;
	base_directory = path;
	pos = base_directory.rfind('/');
	if ( pos < 0 ) {
	    base_directory = "./";
	}
	else {
	    base_directory.resize(pos + 1);
	}
	path_to_open = path;
    }
    /* 2nd case */
    else {
	size_t junk;
	base_directory = base_dir;
	path_to_open = path;
	if ( path_to_open.cchr(0) != '/' &&
	     path_to_open.regmatch("^[a-z]+://",&junk) < 0 ) {
	    path_to_open.printf("%s%s", base_dir, path);
	}
    }

    /* open stream */
    try {
	//err_report1(__FUNCTION__,"DEBUG","open: [%s]", path_to_open.cstr());
	status = fin.open("r",path_to_open.cstr());
    }
    catch (...) {
	err_throw(__FUNCTION__,"FATAL","unexpected exception");
    }
    if ( status < 0 ) {
	err_report1(__FUNCTION__,"ERROR","cannot open: %s",path);
	goto quit;
    }

    /* read template recursively */
    try {
	tstring line;
	/* loop: one by one */
	while ( (line=fin.getline()) != NULL ) {
	    /* check include */
	    if ( line.strncasecmp("\\include ",9) == 0 ||
		 line.strncasecmp("\\include\t",9) == 0 ) {
		tstring path_to_include;
		line.copy(9, path_to_include);
		path_to_include.trim();
		status = this->template_load_recursively(flags, 
							path_to_include.cstr(),
							base_directory.cstr());
		if ( status < 0 ) {
		    err_report(__FUNCTION__,"ERROR",
			       "this->template_load_recursively() failed");
		    fin.close();
		    goto quit;
		}
	    }
	    else {
		/* not include */
		if ( this->template_buffer.length() < 
		     this->template_buffer_idx + line.length() ) {
		    this->template_buffer.resizeby(
					       this->template_buffer.length());
		}
		this->template_buffer.put(this->template_buffer_idx, line);
		this->template_buffer_idx += line.length();
	    }
	}
    }
    catch (...) {
	fin.close();
	err_throw(__FUNCTION__,"FATAL","caught exception");
    }

    fin.close();

    return_status = 0;
 quit:
    return return_status;
}

/**
 * @brief  ʸ����Хåե����� FITS �ƥ�ץ졼�Ȥ��ɤ߹���
 *
 * @note   ���Υ��дؿ��� protected �Ǥ���
 */
int fitscc::template_load( int flags, const char *templ )
{
    int return_status = -1;
    int status;
    size_t buf_idx;
    long hdu_cnt = 0;
    long hrec_cnt = 0;				/* �إå��쥳���ɤΥ������ */
    tstring header_all;
    fits_header templhdr;
    templhdr.suppress_dupkey_warning() = true;

    this->_init();

    if ( templ == NULL ) {
	err_report(__FUNCTION__,"ERROR","2nd argument 'templ' is NULL.");
	goto quit;
    }

    buf_idx = 0;
    while ( templ[buf_idx] != '\0' ) {
	size_t line_spn, buf_idx0;
	tstring line;
	fits_header_record a_record;
	/*
	 * 1�Ԥ��ļ��Ф���line �ˤ����
	 */
	line_spn = 0;
	buf_idx0 = buf_idx;
	while ( templ[buf_idx] != '\n' && templ[buf_idx] != '\r' && 
		templ[buf_idx] != '\0' ) {
	    buf_idx++;
	}
	line_spn = buf_idx - buf_idx0;
	if ( templ[buf_idx] == '\r' && templ[buf_idx+1] == '\n' ) buf_idx += 2;
	else if (templ[buf_idx] == '\r' || templ[buf_idx] == '\n') buf_idx ++;
	line.assign(templ + buf_idx0, line_spn);
	/* TAB��Ÿ������: ����� CFITSIO �ƥ�ץ��ư��ǤϤʤ��餷�� */
	/* line.expand_tabs(); */
	/* ñ����ִ�: CFITSIO �ƥ�ץ��ư��Ϥ���餷�� */
	line.strreplace("\t"," ",true);
	//err_report1(__FUNCTION__,"DEBUG","[%s]",line.cstr());
	/*
	 * �������� line �ˤĤ��Ƥ������Ƚ�� 
	 */
	if ( line.cchr(0) != '#' &&
	     ( 0 < hrec_cnt || 
	       line.length() != line.strspn(" \f\v") ) ) {
	    /* if length of spc < 8 and some chrs begin, erase the spc */
	    size_t spc_spn = line.strspn(' ');
	    if ( spc_spn < 8 && spc_spn < line.length() ) {
		line.erase(0, spc_spn);
	    }
	    /* if there is no space chars ... */
	    if ( 0 < line.length() && line.length() == line.strcspn(' ') ) {
		/* to show that this line is not junk data */
		line.append(" ");
	    }
	    /* parse a line (private/parse_a_header_record.cc) */
	    if ( parse_a_header_record(line, true, &a_record) == 0 ) {
		tstring keywd;
		keywd.assign(a_record.keyword());
		if ( keywd.strcmp("XTENSION") == 0 ) {
		    templhdr.expand_continue_records();
		    /* ��­����إå��쥳���ɤ���ä� HDU ���� */
		    status = this->interpolate_templhdr_and_append_hdu(&templhdr);
		    if ( status < 0 ) { 
			err_report(__FUNCTION__,"ERROR",
			"this->interpolate_templhdr_and_append_hdu() failed.");
			err_report1(__FUNCTION__,"ERROR", 
				    "count of HDU: %ld.", hdu_cnt);
			goto quit;
		    }
		    hdu_cnt ++;
		    /* initialize buffer of template */
		    templhdr.init();
		    templhdr.suppress_dupkey_warning() = true;
		    //err_report1(__FUNCTION__,"DEBUG",
		    //"templhder.length()=[%ld]",templhdr.length());
		    hrec_cnt = 0;
		}
		/* ʸ�����ͤξ��� "'" �ǰϤ�褦�ˤ��� */
		if ( a_record.status() == FITS::NORMAL_RECORD &&
		     a_record.type() == FITS::STRING_T ) {
		    tstring val;
		    /* ���� "'" �ǰϤޤ�Ƥ����餽�ΤޤޤǤ��� */
		    val.assign(a_record.value());
		    if ( 2 <= val.length() && val.at(0) == '\'' &&
			 val.at(val.length()-1) == '\'' ) {
			/* do nothing */
		    }
		    else {
			val.assign(a_record.svalue());
			a_record.assign(val.cstr());
		    }
		}
		templhdr.append( a_record );
		hrec_cnt ++;
	    }
	}
    }

    templhdr.expand_continue_records();
    /* ��­����إå��쥳���ɤ���ä� HDU ���� */
    status = this->interpolate_templhdr_and_append_hdu(&templhdr);
    if ( status < 0 ) { 
	err_report(__FUNCTION__,"ERROR",
		   "this->interpolate_templhdr_and_append_hdu() failed.");
	err_report1(__FUNCTION__,"ERROR", "count of HDU: %ld.", hdu_cnt);
	goto quit;
    }
    hdu_cnt ++;
    /* initialize buffer of template */
    templhdr.init();

    return_status = 0;
 quit:
    return return_status;
}

/**
 * @brief  FITS �ƥ�ץ졼�Ȥν����ˤ����ƥإå��쥳���ɤν��Ĵ�� (������)
 *
 * @note    private �ʴؿ��Ǥ���
 */
static void adjust_record_order( fits_header *templhdr,
				 const char *keyword, long order )
{
    long idx;

    idx = templhdr->index(keyword);

    if ( 0 < idx ) {
	/* �쥳���ɤ�����ξ��Ǥ�̵����硤��ư������ */
	if ( idx != order ) {
	    fits_header_record tmprec;
	    tmprec.assign(templhdr->record(idx));
	    templhdr->erase(idx);
	    templhdr->insert(order, tmprec);
	}
	/* ;ʬ�ʤΤ����ä���硤�ٹ𤷤ƾõ�� */
	while ( order < (idx = templhdr->index(keyword)) ) {
	    err_report1(__FUNCTION__,"WARNING",
			"duplicated %s keyword is erased.",keyword);
	    templhdr->erase(idx);
	}
    }

    return;
}

/**
 * @brief  FITS �ƥ�ץ졼�Ȥ����Ƥ��������إå����Ѵ�����HDU ���ɲ�
 *
 * @note   ���Υ��дؿ��� protected �Ǥ���
 */
int fitscc::interpolate_templhdr_and_append_hdu( fits_header *templhdr )
{
    int return_status = -1;
    long idx, rec_cnt, new_hdu_idx;
    long i;
    int sh_cnt;
    int hdutype;
    tstring v_xtension;
    tstring auton_trigger_str;
    fits_hdu tmp_hdu;
    stdstreamio dev_zero;
    asarray_tstring colkwd;

    /* Index of HDU to append */
    new_hdu_idx = this->length();

    /* SIMPLE or XTENSION �ν��� */

    /* Not Primary HDU */
    if ( 0 < new_hdu_idx ) {
	/*  SIMPLE ������ɤ����뤫�⤷��ʤ��Τǡ��õ�Ƥ��� */
	while ( 0 <= (idx = templhdr->index("SIMPLE")) ) {
	    err_report(__FUNCTION__,"WARNING",
		       "invalid SIMPLE keyword is erased.");
	    templhdr->erase(idx);
	}
    }
    /* Primary HDU */
    else {
	idx = templhdr->index("SIMPLE");
	if ( 0 <= idx ) {
	    tstring val;
	    /* true, yes �Τ褦��ɽ���Ǥ��ɤ����ˤ��� */
	    val.assign(templhdr->record(idx).svalue()).toupper();
	    if ( val.cchr(0) == 'T' || val.cchr(0) == 'Y' ) 
		templhdr->record(idx).assign(true);
	    else
		templhdr->record(idx).assign(false);
	    adjust_record_order(templhdr, "SIMPLE", 0);
	}
	else {
	    templhdr->insert(0L, "SIMPLE", "T", "");
	}
    }

    /* ���: XTENSION ������ɤϤ����ʤ���뤳�ȤϤʤ� */
    /* ɬ��2���ܰʹߤθƤӽФ����ˤ��� */

    v_xtension = "";
    idx = templhdr->index("XTENSION");
    if ( 0 <= idx ) {
	v_xtension.assign(templhdr->record(idx).svalue()).toupper();
	templhdr->record(idx).assign(v_xtension.cstr());
	adjust_record_order(templhdr, "XTENSION", 0);
    }

    /* Setup basic keywords: BITPIX, NAXIS, ... */
    rec_cnt = 1;
    /*
     * Primary HDU or IMAGE HDU
     */
    if ( new_hdu_idx == 0 || v_xtension.strcmp("IMAGE") == 0 ) {
	int j, max_j;
	long max_len;
	idx = templhdr->index("BITPIX");
	if ( idx < 0 ) {
	    templhdr->insert(rec_cnt, "BITPIX","16","");
	}
	else adjust_record_order(templhdr, "BITPIX", rec_cnt);
	rec_cnt ++;
	/* Check NAXIS */
	idx = templhdr->index("NAXIS");
	if ( idx < 0 ) {
 	    templhdr->insert(rec_cnt, "NAXIS","0","");
	}
	else adjust_record_order(templhdr, "NAXIS", rec_cnt);
	rec_cnt ++;
	/* NAXISn ... */
	/* �ޤ��ǽ�˺���μ��ֹ桤����Ĺ������� */
	max_j = 0;
	max_len = 0;
	for ( j=1 ; j <= 999 ; j++ ) {
	    tstring kw;
	    kw.printf("NAXIS%d",j);
	    idx = templhdr->index(kw.cstr());
	    if ( 0 <= idx ) {
		long l;
		max_j = j;
		l = templhdr->record(idx).lvalue();
		if ( max_len < l ) max_len = l;
	    }
	}
	/* ����� NAXISn ��ȴ���Ƥ������������� */
	for ( j=1 ; j <= max_j ; j++ ) {
	    tstring kw;
	    kw.printf("NAXIS%d",j);
	    idx = templhdr->index(kw.cstr());
	    if ( idx < 0 ) {
		tstring cm;
		cm.printf("length of data axis %d",j);
		if ( 0 < max_len ) {
		    templhdr->insert(rec_cnt, kw.cstr(), "1", "");
		}
		/* �¤ϡ������� 0 �ˤ���ȡ�fits_image ���饹�ǥ��顼�� */
		/* �ʤ롥����ϻ��ͤȤ������ǡ� */
		else {
		    templhdr->insert(rec_cnt, kw.cstr(), "0", "");
		}
	    }
	    else adjust_record_order(templhdr, kw.cstr(), rec_cnt);
	    rec_cnt ++;
	}
	/* ���θĿ� max_j ��Ƚ�������Τǡ�NAXIS �ˤ���� */
	templhdr->record("NAXIS").assign(max_j);
	/* EXTEND or PCOUNT, GCOUNT */
	if ( new_hdu_idx == 0 ) {		/* Primary �ξ�� */
	    idx = templhdr->index("EXTEND");
	    if ( idx < 0 ) {
		templhdr->insert(rec_cnt, "EXTEND","T","");
	    }
	    else adjust_record_order(templhdr, "EXTEND", rec_cnt);
	    /* ���ɤ��ΤǾ�� T �ˤ���(����ˤʤ���Ϥʤ���) */
	    templhdr->record("EXTEND").assign(true);
	    rec_cnt ++;
	}
	else {					/* Primary �Ǥʤ���� */
	    fits::header_def defs[] = { 
		{"PCOUNT","0",""}, 
		{"GCOUNT","1",""}, 
		{NULL}
	    };
	    j = 0;
	    while ( defs[j].keyword != NULL ) {
		idx = templhdr->index(defs[j].keyword);
		if ( idx < 0 ) {
		    templhdr->insert(rec_cnt, defs[j].keyword, defs[j].value, 
				     defs[j].comment);
		}
		else adjust_record_order(templhdr, defs[j].keyword, rec_cnt);
		rec_cnt ++;
		j++;
	    }
	}
    }
    /*
     * binary table or ASCII table
     */
    else if ( v_xtension.strcmp("TABLE") == 0 ||
	      v_xtension.strcmp("BINTABLE") == 0 ) {
	int j;
	fits::header_def defs[] = { 
	    {"BITPIX","8",""}, 
	    {"NAXIS", "2",""}, 
	    {"NAXIS1","0",""}, 
	    {"NAXIS2","0",""}, 
	    {"PCOUNT","0",""}, 
	    {"GCOUNT","1",""}, 
	    {"TFIELDS","0",""}, 
	    {NULL}
	};
	j = 0;
	while ( defs[j].keyword != NULL ) {
	    idx = templhdr->index(defs[j].keyword);
	    if ( idx < 0 ) {
		templhdr->insert(rec_cnt, defs[j].keyword, defs[j].value, 
				 defs[j].comment);
	    }
	    else adjust_record_order(templhdr, defs[j].keyword, rec_cnt);
	    rec_cnt ++;
	    j++;
	}
    }
    else {
	err_report1(__FUNCTION__,"ERROR","Unsupported XTENSION value: %s.",
		    v_xtension.cstr());
	goto quit;
    }

    /* 
     *  �������� # ����ˤ�뼫ư�ʥ�Х�󥰤�Ԥʤ�
     */
    sh_cnt = 0;
    for ( i=0 ; i < templhdr->length() ; i++ ) {
	if ( templhdr->record(i).status() == FITS::NORMAL_RECORD ) {
	    tstring kw, kw_base, new_kw;
	    kw = templhdr->record(i).keyword();
	    if ( 2 <= kw.length() && kw.strrspn('#') == 1 ) {
		kw_base.assign(kw, 0, kw.length() - 1);
		if ( auton_trigger_str.length() < 1 ) {
		    auton_trigger_str = kw;
		}
		if ( kw.strcmp(auton_trigger_str) == 0 ) {
		    sh_cnt ++;
		    /* �⤷��sh_cnt ���ֹ椬�إå���¸�ߤ����硤
		       �ֹ�����䤹 */
		    while ( 1 ) {
			new_kw.printf("%s%d",kw_base.cstr(),sh_cnt);
			if ( templhdr->index(new_kw.cstr()) < 0 ) break;
			else sh_cnt ++;
		    }
		}
		else new_kw.printf("%s%d", kw_base.cstr(), sh_cnt);
		templhdr->insert(i, new_kw.cstr(), templhdr->record(i).value(),
				 templhdr->record(i).comment());
		templhdr->erase(i+1);
		/* */
		colkwd[kw_base.cstr()] = "1";
	    }
	}
    }

    /*
     * �Х��ʥꡦASCII�ơ��֥�ξ�����­�إå��ͤ�������
     */
    if ( v_xtension.strcmp("BINTABLE") == 0 ) {
	long n_rows = 0, n_cols = 0;
	long bytes_per_row = 0;
	tstring kw, val;
	int j;
	j = 1;
	while ( 1 ) {
	    long bytes;
	    fits_table_col col;
	    fits::table_def tdef = { "JUNK","", NULL,NULL, "","", "", "", "" };
	    /* toupper: TDISPn */
	    kw.printf("TDISP%d",j);
	    idx = templhdr->index(kw.cstr());
	    if ( 0 <= idx ) {
		val.assign(templhdr->record(idx).svalue()).toupper();
		templhdr->record(idx).assign(val.cstr());
	    }
	    /* toupper: TFORMn */
	    kw.printf("TFORM%d",j);
	    idx = templhdr->index(kw.cstr());
	    if ( idx < 0 ) break;
	    val.assign(templhdr->record(idx).svalue()).toupper();
	    templhdr->record(idx).assign(val.cstr());
	    /* get byte info */
	    tdef.ttype = kw.cstr();	/* <- dummy */
	    tdef.tform = templhdr->record(idx).svalue();
	    col.define(tdef);
	    bytes = col.elem_byte_length();
	    if ( bytes <= 0 ) {
		err_report1(__FUNCTION__,"ERROR","Unsupported TFORM: [%s].",
			    tdef.tform);
		goto quit;
	    }
	    n_cols ++;
	    bytes_per_row += bytes;
	    j++;
	}
	templhdr->record("NAXIS1").assign(bytes_per_row);
	templhdr->record("TFIELDS").assign(n_cols);
	if ( templhdr->record("NAXIS2").lvalue() < 0L ) {
	    templhdr->record("NAXIS2").assign(0L);
	}
	n_rows = templhdr->record("NAXIS2").lvalue();
	/* toupper for TXFLDKWD */
	if ( 0 <= templhdr->index("TXFLDKWD") ) {
	    val.assign(templhdr->record("TXFLDKWD").svalue()).toupper();
	    templhdr->record("TXFLDKWD").assign(val.cstr());
	}
	/* set PCOUNT */
	if ( 0 <= templhdr->index("THEAP") ) {
	    long long reserved_area_length;
	    long long heap_off = templhdr->record("THEAP").llvalue();
	    if ( heap_off < bytes_per_row * n_rows ) {
		heap_off = bytes_per_row * n_rows;
		templhdr->record("THEAP").assign(heap_off);
	    }
	    //err_report1(__FUNCTION__,"DEBUG","========%lld",
	    //		  templhdr->record("THEAP").llvalue());
	    reserved_area_length = heap_off - (bytes_per_row * n_rows);
	    if (templhdr->record("PCOUNT").llvalue() < reserved_area_length) {
		templhdr->record("PCOUNT").assign(reserved_area_length);
	    }
	}
    }
    /*
     * ASCII �ơ��֥�ϸ��ʳ��ǤϤ��ޤ꼫ư�����Ƥ��ʤ�
     * TBCOLn �� "+3" �Τ褦�˻��ꤹ��ȡ�TBCOLx �� NAXIS1 ����ưŪ��
     * ���åȤ���롤�Ȥ����ΤϤ�������(�ͤ���)
     */
    else if ( v_xtension.strcmp("TABLE") == 0 ) {
	long n_cols = 0;
	tstring kw, val;
	int j;
	j = 1;
	while ( 1 ) {
	    kw.printf("TFORM%d",j);
	    idx = templhdr->index(kw.cstr());
	    if ( idx < 0 ) break;
	    val.assign(templhdr->record(idx).svalue()).toupper();
	    templhdr->record(idx).assign(val.cstr());
	    n_cols ++;
	    j++;
	}
	templhdr->record("TFIELDS").assign(n_cols);
	/* toupper for TXFLDKWD */
	if ( 0 <= templhdr->index("TXFLDKWD") ) {
	    val.assign(templhdr->record("TXFLDKWD").svalue()).toupper();
	    templhdr->record("TXFLDKWD").assign(val.cstr());
	}
    }

    /* ɬ�פʾ�硤TXFLDKWD ���ɲä�����ư�ʥ�Х�󥰤�����줿������ɤ� */
    /* �����̵��������ɤϡ�TXFLDKWD ���ͤ�����롥 */
    if ( v_xtension.strcmp("BINTABLE") == 0 || 
	 v_xtension.strcmp("TABLE") == 0 ) {
	/* FITS ����Ƿ�ޤäƤ��륭����� */
	const char *field_keywords[] = {"TTYPE", "TUNIT",
					"TDISP", "TFORM", "TBCOL", "TDIM", 
					"TZERO", "TSCAL", "TNULL", NULL};
	size_t j;
	bool needs_txfldkwd = false;
	for ( j=0 ; field_keywords[j] != NULL ; j++ ) {
	    if ( 0 <= colkwd.index(field_keywords[j]) ) {
		colkwd[field_keywords[j]] = "0";
	    }
	}
	for ( j=0 ; j < colkwd.length() ; j++ ) {
	    if ( colkwd.at(j).strcmp("1") == 0 ) needs_txfldkwd = true;
	    //err_report1(__FUNCTION__,"DEBUG","key=[%s]",colkwd.key(j));
	    //err_report1(__FUNCTION__,"DEBUG","val=[%s]",colkwd.at(j).cstr());
	}

	if ( needs_txfldkwd == true ) {
	    tarray_tstring arr;
	    tstring new_val;
	    if ( templhdr->index("TXFLDKWD") < 0 ) {
		long idx_to_insert = templhdr->index("TFIELDS") + 1;
		templhdr->insert(idx_to_insert, "TXFLDKWD");
	    }
	    arr.explode(templhdr->record("TXFLDKWD").svalue(), ",", true).trim();
	    for ( j=0 ; j < arr.length() ; ) {
		if ( 0 < arr[j].length() ) {
		    if ( 0 <= colkwd.index(arr[j].cstr()) ) {
			colkwd[arr[j].cstr()] = "0";
		    }
		    j++;
		}
		else arr.erase(j,1);
	    }
	    for ( j=0 ; j < colkwd.length() ; j++ ) {
		if ( colkwd.at(j).strcmp("1") == 0 ) {
		    arr.append(colkwd.key(j),1);
		}
	    }
	    new_val.implode(arr.cstrarray(), ",");
	    //err_report1(__FUNCTION__,"DEBUG","new_val=[%s]",new_val.cstr());
	    templhdr->record("TXFLDKWD").assign(new_val.cstr());
	}
    }

    /*
     * HDU ������Ƚ��
     */
    hdutype = templhdr->hdutype();

    /* ��ޤäƤ��ʤ��إå��Υ����Ȥ����� */
    for ( i=0 ; i < templhdr->length() ; i++ ) {
	if ( templhdr->record(i).status() == FITS::NORMAL_RECORD ) {
	    if ( templhdr->record(i).comment_length() == 0 ) {
		templhdr->record(i).assign_default_comment(hdutype);
	    }
	}
    }

#if 0
    {
	long i;
	for ( i=0 ; i < templhdr->length() ; i++ ) {
	    err_report1(__FUNCTION__,"DEBUG","key=[%s]",
			templhdr->record(i).keyword());
	    err_report1(__FUNCTION__,"DEBUG","val=[%s]",
			templhdr->record(i).value());
	    err_report1(__FUNCTION__,"DEBUG","type=[%d]",
			templhdr->record(i).type());
	}
	err_report(__FUNCTION__,"DEBUG","========");
    }
#endif

    //err_report1(__FUNCTION__,"DEBUG","=[%s]=",templhdr->formatted_string());

    /*
     * �������餤�褤�� HDU �����
     */

    if ( tmp_hdu.read_header_object(*templhdr) < 0 ) {
	err_report(__FUNCTION__,"ERROR",
		   "read_header_object(*templhdr) failed");
	goto quit;
    }

    if ( dev_zero.open("r", "/dev/zero") < 0 ) {
	err_report(__FUNCTION__,"ERROR", "cannot open /dev/zero.");
	goto quit;
    }

    if ( hdutype == FITS::IMAGE_HDU ) {
	ssize_t sz;
	this->append(hdutype);
	sz = this->image(new_hdu_idx).read_stream(&tmp_hdu, dev_zero);
	if ( sz < 0 ) {
	    err_report1(__FUNCTION__,"ERROR",
			"image(%ld).read_stream() failed",new_hdu_idx);
	    dev_zero.close();
	    goto quit;
	}
	this->image(new_hdu_idx).fill(0);
    }
    else if ( hdutype == FITS::BINARY_TABLE_HDU ||
	      hdutype == FITS::ASCII_TABLE_HDU ) {
	ssize_t sz;
	this->append(hdutype);
	sz = this->table(new_hdu_idx).read_stream(&tmp_hdu, dev_zero);
	if ( sz < 0 ) {
	    err_report1(__FUNCTION__,"ERROR",
			"table(%ld).read_stream() failed",new_hdu_idx);
	    dev_zero.close();
	    goto quit;
	}
	for ( i=0 ; i < this->table(new_hdu_idx).col_length() ; i++ ) {
	    this->table(new_hdu_idx).col(i).clean();
	}
    }
    else {
	err_report1(__FUNCTION__,"ERROR","Unsupported HDU type: %d",
		    hdutype);
	dev_zero.close();
	goto quit;
    }

    /* Update index_rec */
    if ( tmp_hdu.extname() != NULL ) 
	this->assign_extname( new_hdu_idx, tmp_hdu.extname() );
    if ( tmp_hdu.extver_is_set() == true )
	this->assign_extver( new_hdu_idx, tmp_hdu.extver() );
    if ( tmp_hdu.extlevel_is_set() == true )
	this->assign_extlevel( new_hdu_idx, tmp_hdu.extlevel() );

    /* Register FMTTYPE and FTYPEVER */
    if ( new_hdu_idx == 0 ) {
#ifdef FMTTYPE_IS_SPECIAL
	idx = this->hdu(new_hdu_idx).sysheader_index("FMTTYPE");
	if ( 0 <= idx ) {
	    const char *v = 
		this->hdu(new_hdu_idx).header_rec.record(idx).svalue();
	    this->fmttype_rec.assign(v);
	}
	idx = this->hdu(new_hdu_idx).sysheader_index("FTYPEVER");
	if ( 0 <= idx ) {
	    this->ftypever_rec = 
		this->hdu(new_hdu_idx).header_rec.record(idx).svalue();
	}
#endif
    }

    /* close "/dev/zero" */
    dev_zero.close();

    //this->append(FITS::IMAGE_HDU);

    return_status = 0;
 quit:
    return return_status;
}

/**
 * @brief  HDU ���ɲ� (���٥�)
 *
 * @note   ���Υ��дؿ��� protected �Ǥ���
 */
fitscc &fitscc::append( int hdutype )
{
    long index0;
    fits_hdu *objp = NULL;

    if ( this->num_hdus_rec == 0 ) {
	/* �����ʤ� IMAGE �ʳ������Ȥ����顤Primary ���� */
	if ( hdutype != FITS::IMAGE_HDU ) {
	    this->append(FITS::IMAGE_HDU);
	}
    }

    index0 = this->num_hdus_rec;

    try {
	this->hdus_rec.resize( index0 + 1 + 1 );
    }
    catch (...) {
	err_throw(__FUNCTION__,"FATAL","hdus_rec.resize() failed");
    }
    this->hdus_ptr_rec[index0] = NULL;
    this->hdus_ptr_rec[index0 + 1] = NULL;

    try {
	this->hdu_is_stack_rec.resize( index0 + 1 );
    }
    catch (...) {
	err_throw(__FUNCTION__,"FATAL","hdu_is_stack_rec.resize() failed");
    }

    try {
	if ( hdutype == FITS::IMAGE_HDU ) {
	    //fprintf(stderr,"debug: new fits_image\n");
	    objp = new fits_image;
	    objp->register_manager(this);
	}
	else if ( hdutype == FITS::BINARY_TABLE_HDU ||
		  hdutype == FITS::ASCII_TABLE_HDU ) {
	    objp = new fits_table;
	    objp->set_hdutype(hdutype);
	    objp->register_manager(this);
	}
    }
    catch ( ... ) {
	err_throw(__FUNCTION__,"FATAL","`new' failed");
    }

    if ( objp == NULL ) {
	err_throw1(__FUNCTION__,"ERROR","HDU type: %d is not supported",hdutype);
    }
    this->hdus_ptr_rec[index0] = objp;
    this->hdu_is_stack_rec.b(index0) = false;
    this->num_hdus_rec = index0 + 1;
    
    /* index_rec �򹹿� */
    //if ( extname != NULL ) {
    //  if ( this->assign_extname( index0, extname ) < 0 ) goto quit;
    //  if ( this->assign_extver( index0, extver ) < 0 ) goto quit;
    //}

    /* "Primary" �� index_rec ������� */
    if ( this->num_hdus_rec == 1 ) {
	if ( this->index_rec.index("Primary",0) < 0 ) {
	    if ( this->index_rec.append("Primary",0) < 0 ) {
		err_throw(__FUNCTION__,"FATAL","this->index_rec.append() failed");
	    }
	}
    }

    return *this;
}

/**
 * @brief  HDU ������ (���٥�)
 *
 * @note   ���Υ��дؿ��� protected �Ǥ���
 */
fitscc &fitscc::insert( long index0, int hdutype )
{
    fits_hdu *objp = NULL;
    long i;
    tstring ename;

    if ( index0 < 0 || this->num_hdus_rec < index0 ) {
	err_report1(__FUNCTION__,"WARNING",
		    "Invalid HDU index: %ld; ignored",index0);
	return *this;
    }

    if ( this->num_hdus_rec == index0 ) {
	this->append(hdutype);
	return *this;
    }

    /* �ޤ���object ���äƤߤ� */
    try {
	if ( hdutype == FITS::IMAGE_HDU ) {
	    objp = new fits_image;
	    objp->register_manager(this);
	}
	else if ( hdutype == FITS::BINARY_TABLE_HDU ||
		  hdutype == FITS::ASCII_TABLE_HDU ) {
	    objp = new fits_table;
	    objp->register_manager(this);
	}
    }
    catch ( ... ) {
	err_throw(__FUNCTION__,"FATAL","`new' failed");
    }

    if ( objp == NULL ) {
	err_throw1(__FUNCTION__,"ERROR","HDU type: %d is not supported",hdutype);
    }
    
    try {
	this->hdus_rec.resize( this->num_hdus_rec + 1 + 1 );
    }
    catch (...) {
	delete objp;
	err_throw(__FUNCTION__,"FATAL","hdus_rec.resize() failed");
    }
    this->hdus_ptr_rec[this->num_hdus_rec] = NULL;
    this->hdus_ptr_rec[this->num_hdus_rec + 1] = NULL;

    try {
	this->hdu_is_stack_rec.resize( this->num_hdus_rec + 1 );
    }
    catch (...) {
	delete objp;
	err_throw(__FUNCTION__,"FATAL","hdu_is_stack_rec.resize() failed");
    }
    this->hdu_is_stack_rec.b(this->num_hdus_rec) = false;

    for ( i=this->num_hdus_rec ; index0 < i ; ) {
	i--;
	this->hdus_ptr_rec[i+1] = this->hdus_ptr_rec[i];
	this->hdu_is_stack_rec.b(i+1) = this->hdu_is_stack_rec.b(i);
    }
    this->hdus_ptr_rec[i] = objp;
    this->hdu_is_stack_rec.b(i) = false;

    /* index_rec �Ϥ�ʤ��� */
    for ( i=this->num_hdus_rec ; index0 < i ; ) {
	i--;
	ename = this->hdus_ptr_rec[i+1]->extname();
	if ( ename.cstr() != NULL ) {
	    if ( ename.strcmp("Primary") == 0 && i == 0 ) {
		if ( this->index_rec.append(ename.cstr(), i+1) < 0 ) {
		    err_throw(__FUNCTION__,"FATAL",
			      "this->index_rec.append() failed");
		}
	    }
	    else {
		if ( this->index_rec.update(ename.cstr(), i, i+1) < 0 ) {
		    err_throw(__FUNCTION__,"FATAL",
			      "this->index_rec.update() failed");
		}
	    }
	}
    }

    this->num_hdus_rec ++;

    /* index_rec �򹹿� */
    //if ( extname != NULL ) {
    //	if ( this->assign_extname( index0, extname ) < 0 ) goto quit;
    //	if ( this->assign_extver( index0, extver ) < 0 ) goto quit;
    //}

    return *this;
}

/**
 * @brief  HDU ������ (���٥�)
 *
 * @note   ���Υ��дؿ��� protected �Ǥ���
 */
fitscc &fitscc::insert( const char *extname0, int hdutype )
{
    long idx;

    if ( extname0 == NULL ) {
	err_report(__FUNCTION__,"WARNING","NULL extname0; ignored");
	return *this;
    }

    idx = this->index_rec.index(extname0,0);
    if ( idx < 0 ) {
	err_report1(__FUNCTION__,"WARNING",
		    "HDU `%s' is not found; ignored",extname0);
	return *this;
    }

    this->insert(idx, hdutype);

    return *this;
}

/**
 * @brief  �Х��ʥ�ơ��֥�Υǡ�����������Ƥ���ݥ����ѿ��Υ��ɥ쥹���֤�
 *
 * @note   ���Υ��дؿ��� protected �Ǥ���
 */
void **fitscc::table_data_ptr_mng_ptr( long hdu_index, long col_index )
{
    if ( hdu_index < 0 || this->num_hdus_rec < hdu_index ) return NULL;
    if ( this->hdu(hdu_index).hdutype() != FITS::BINARY_TABLE_HDU &&
	 this->hdu(hdu_index).hdutype() != FITS::ASCII_TABLE_HDU ) return NULL;
    return this->table(hdu_index).data_ptr_mng_ptr( col_index );
}

#include "private/parse_a_header_record.cc"

}	/* namespace sli */

