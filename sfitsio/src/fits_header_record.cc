/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2015-11-18 23:35:43 cyamauch> */

/**
 * @file   fits_header_record.cc
 * @brief  FITS�إå���1�ĤΥ쥳���ɤ�ɽ�����륯�饹fits_header_record�Υ�����
 */

#define CLASS_NAME "fits_header_record"

#include "config.h"

#include "fits_header_record.h"
#include "fits_header.h"
#include "fits_table_col.h"

#include <sli/asarray_tstring.h>
#include <sli/numeric_minmax.h>
#include <math.h>
#include <time.h>

#include "private/err_report.h"

#include "private/c_llround.cc"


namespace sli
{

static const int KEYWORD_IDX = 0;
static const int VALUE_IDX   = 1;
static const int COMMENT_IDX = 2;

#include "private/header_format_a_record.h"

namespace fits
{

/* 
 * See http://heasarc.gsfc.nasa.gov/docs/fcg/standard_dict.html for FITS
 * standard keywords and comments. 
 */
/**
 * @brief  �����ȼ������� (fallback)
 *
 * @note   �� HDU �������Ѥμ����̵�����˻Ȥ��롥<br>
 *         �ɲä��٤�������ɤ�����Ф��Τ餻����������
 */
static asarray_tstring Fallback_comments(		/* associative array */
	/*           |MIN                                       MAX| */
	/* system keywords */
        "SIMPLE",   "conformity to FITS standard",
	"BITPIX",   "number of bits per data pixel",
	"NAXIS",    "number of data axes",
	"NAXIS#",   "length of data axis #",
	"EXTEND",   "possibility of presence of extensions",
	"XTENSION", "type of extension",
	"PCOUNT",   "number of parameters per group",
	"GCOUNT",   "number of groups",
	"EXTNAME",  "name of this HDU",
	"EXTVER",   "version of the extension",
	"EXTLEVEL", "hierarchical level of the extension",
	/*           |MIN                                       MAX| */
	/* standard of JAXA data center */
	"FMTTYPE",  "type of format in FITS file",
	"FTYPEVER", "version of FMTTYPE definition",
	"FMTVER",   "version of FMTTYPE definition",
	/* standard keywords */
	"DATE",     "date of file creation",
	"ORIGIN",   "organization responsible for the data",
	"AUTHOR",   "author of the data",
	"REFERENC", "bibliographic reference",
	"DATE-OBS", "date of the observation",
	"TELESCOP", "telescope or mission name",
	"INSTRUME", "instrument name",
	"DETECTOR", "detector name",
	"OBSERVER", "observer who acquired the data",
	"OBJECT",   "name of observed object",
	/*           |MIN                                       MAX| */
	"EPOCH",    "equinox of celestial coordinate system",
	"EQUINOX",  "equinox of celestial coordinate system",
	"TIMESYS",  "explicit time scale specification",
	/* other general keywords */
	"OBSERVAT", "observatory name",
	"CREATOR",  "data generator program",
	"PIPELINE", "data processing pipeline name",
	"FILENAME", "file name",
	"PROPOSAL", "proposal ID",
	"BAND",     "band name",
	"MJD",      "modified Julian date",
	"AIRMASS",  "air mass",
	"EXPTIME",  "exposure time",
	"WEATHER",  "weather condition",
	/* ����ʤ������褵������ ���ΰ��֤��Ф��ƻȤ���Τ��狼��󤫤� */
	//"AZIMUTH",  "azimuth of pointing",
	//"ALTITUDE", "altitude of pointing",
	//"ZD",       "zenith distance",
	/* general keyword of JAXA data center */
	"CNTTYPE",  "type of data content",
	"CNTVER",   "version of data content",
	/* We do not use broken comments like "HDU checksum updated" :-P */
	"CHECKSUM", "HDU checksum",
	"DATASUM",  "data unit checksum",
	/*           |MIN                                       MAX| */
	NULL
);
/**
 * @brief  �����ȼ������� (Image HDU)
 *
 * @note   Image HDU �ξ��˻Ȥ��롥<br>
 *         �ɲä��٤�������ɤ�����Ф��Τ餻����������
 */
static asarray_tstring Image_comments(			/* associative array */
	/*           |MIN                                       MAX| */
        "BZERO",    "zero point in scaling equation",
        "BSCALE",   "linear factor in scaling equation",
	"BLANK",    "value used for undefined pixels",
	"BUNIT",    "physical unit of the pixel values",
	"DATAMIN",  "minimum data value",
	"DATAMAX",  "maximum data value",
	/* WCS */
	"WCSAXES?", "number of axes for WCS",
	"CRVAL#?",  "world coordinate at reference point",
	"CRPIX#?",  "pixel coordinate at reference point",
	"CDELT#?",  "world coordinate increment at reference point",
	"CROTA#",   "coordinate system rotation angle",
	"CTYPE#?",  "type of celestial system and projection system",
	/*           |MIN                                       MAX| */
	"CUNIT#?",  "units of the coordinates along axis",
	"PC#_#?",   "matrix of rotation (#,#)",
	"CD#_#?",   "matrix of rotation and scale (#,#)",
	//"PV#_#?",   "",
	//"PS#_#?",   "",
	"WCSNAME?", "name of WCS",
	//"CRDER#?",  "",
	//"CSYER#?",  "",
	"LONPOLE?", "native longitude of celestial pole",
	"LATPOLE?", "native latitude of celestial pole",
	"EQUINOX?", "equinox of celestial coordinate system",
	"MJD-OBS",  "modified Julian date of observation",
	"CNAME#?",  "description of CTYPE definition",
	"RADESYS?", "default coordinate system",
	//"MJD-AVG",  "",
	//"DATE-AVG", "",
	/*           |MIN                                       MAX| */
	NULL
);
/**
 * @brief  �����ȼ������� (Binary Table HDU)
 *
 * @note   Binary Table HDU �ξ��˻Ȥ��롥<br>
 *         �ɲä��٤�������ɤ�����Ф��Τ餻����������
 */
static asarray_tstring Binary_table_comments(		/* associative array */
	/*           |MIN                                       MAX| */
	"BITPIX",   "number of bits per data element",
        "NAXIS1",   "width of table in bytes",
	"NAXIS2",   "number of rows in table",
	"PCOUNT",   "length of reserved area and heap",
	"TFIELDS",  "number of fields in each row",
        "TXFLDKWD", "extended field keywords",			/* JAXA ext. */
        "TTYPE#",   "field name",
        "TALAS#",   "aliases of field name",			/* JAXA ext. */
        "TELEM#",   "element names",				/* JAXA ext. */
        "TUNIT#",   "physical unit",
	"TDISP#",   "display format",
	"TFORM#",   "data format",
        "TDIM#",    "dimensionality of the array",
        "TZERO#",   "zero point in scaling equation",
        "TSCAL#",   "linear factor in scaling equation",
	"TNULL#",   "value used for undefined cells",
        "THEAP",    "byte offset to heap area",
        /* CFITSIO ext. */
	"TLMIN#",   "minimum value legally allowed",
	"TLMAX#",   "maximum value legally allowed",
	"TDMIN#",   "minimum data value",
	"TDMAX#",   "maximum data value",
	//"TCTYP#",   "",
	//"TCRPX#",   "",
	//"TCRVL#",   "",
	//"TCDLT#",   "",
	//"TCROT#",   "",
	//"TCUNI#",   "",
	/*           |MIN                                       MAX| */
	NULL
);
/**
 * @brief  �����ȼ������� (ASCII Table HDU)
 *
 * @note   ASCII Table HDU �ξ��˻Ȥ��롥<br>
 *         �ɲä��٤�������ɤ�����Ф��Τ餻����������
 */
static asarray_tstring Ascii_table_comments(		/* associative array */
	/*           |MIN                                       MAX| */
	"BITPIX",   "number of bits per data element",
        "NAXIS1",   "width of table in bytes",
	"NAXIS2",   "number of rows in table",
	"TFIELDS",  "number of fields in each row",
        "TXFLDKWD", "extended field keywords",			/* JAXA ext. */
        "TTYPE#",   "field name",
        "TALAS#",   "aliases of field name",			/* JAXA ext. */
        "TUNIT#",   "physical unit",
	"TFORM#",   "display format",
        "TBCOL#",   "starting position in bytes",
        "TZERO#",   "zero point in scaling equation",
        "TSCAL#",   "linear factor in scaling equation",
	"TNULL#",   "value used for undefined cells",
	"TLMIN#",   "minimum value legally allowed",
	"TLMAX#",   "maximum value legally allowed",
	"TDMIN#",   "minimum data value",
	"TDMAX#",   "maximum data value",
	//"TCTYP#",   "",
	//"TCRPX#",   "",
	//"TCRVL#",   "",
	//"TCDLT#",   "",
	//"TCROT#",   "",
	//"TCUNI#",   "",
	/*           |MIN                                       MAX| */
	NULL
);

/* public */
/**
 * @brief  �����ȼ���ι���
 *
 *  �ץ���ޤ������ȼ���򹹿���������Ǥ��ޤ���
 *
 * @param   hdutype HDU�μ���
 * @param   new_kwd_comments ������ɤȥ����ȤΥ��åȤ����Ҥ��줿
 *                           ʸ��������NULL �ǽ�ü���뤳�ȡ�
 * @return  �񤭹��ޤ줿 asarray_tstring �����������Ȥλ��� (�ɼ�����)
 * @note    �����Х����Ū���֥������Ȥ˽񤭹��ޤ�ޤ���
 */
const asarray_tstring &update_comment_dictionary( int hdutype, 
					  const char *const *new_kwd_comments )
{
    size_t i;

    /* select associative array */
    asarray_tstring *tgt = &Fallback_comments;
    if ( hdutype == FITS::IMAGE_HDU ) {
	tgt = &Image_comments;
    }
    else if ( hdutype == FITS::BINARY_TABLE_HDU ) {
	tgt = &Binary_table_comments;
    }
    else if ( hdutype == FITS::ASCII_TABLE_HDU ) {
	tgt = &Ascii_table_comments;
    }

    if ( new_kwd_comments != NULL ) {
	/* update */
	for ( i=0 ; new_kwd_comments[i] != NULL ; i+=2 ) {
	    const char *kwd = new_kwd_comments[i];
	    const char *com = new_kwd_comments[i+1];
	    if ( com == NULL ) break;
	    if ( kwd[0] != '\0' ) {
		tgt->at(kwd) = com;
	    }
	}
    }

    return *tgt;
}

/* private */
/**
 * @brief  HDU������,�������,�ͤ˱����ƥ����ȼ��񤫤饳����ʸ��������
 *
 * @note  private �ʴؿ��Ǥ���
 */
static const char *get_default_comment( int hdutype, const char *keyword,
					const char *value,
					tstring *return_comment_ptr )
{
    const char *return_value = NULL;
    tstring kwd_num[8];		/* PC?_? �ʤɤ� ? ����ʬ����¸���� */
    size_t n_kwd_num = 0;	/* kwd_num ����Ͽ���줿�Ŀ� */
    tstring kwd_with_sh;	/* PC1_1 => PC#_# �Τ褦���Ѵ�������Τ���¸ */
    size_t i;
    tstring &return_comment = *return_comment_ptr;

    return_comment = NULL;

    //err_report1(__FUNCTION__,"DEBUG","len_fallback_comments = [%zd]",
    //Fallback_comments.length());
    //for ( i=0 ; i < Fallback_comments.length() ; i++ ) {
    //	err_report1(__FUNCTION__,"DEBUG","[[%s]]", Fallback_comments.cstr(i));
    //	err_report1(__FUNCTION__,"DEBUG","[[%s]]", Fallback_comments.key(i));
    //}

    //err_report1(__FUNCTION__,"DEBUG","keyword = [%s]",keyword);

    if ( keyword == NULL ) goto quit;
    if ( keyword[0] == '\0' ) goto quit;

    /* convert 'ABC12_34' -> 'ABC#_#' */
    for ( i=0 ; keyword[i] != '\0' ; i++ ) {
	if ( '0' <= keyword[i] && keyword[i] <= '9' ) break;
    }
    if ( keyword[i] != '\0' ) {
	size_t off = 0;
	kwd_with_sh.append(keyword + off, i - off).append('#',1);
	while ( keyword[i] != '\0' ) {
	    size_t n_begin, n_spn = 0;
	    n_begin = i;
	    while ( '0' <= keyword[i] && keyword[i] <= '9' ) {
		i ++;
		n_spn ++;
	    }
	    if ( 0 < n_spn && n_kwd_num < 8 ) {		/* ������ʬ�򽦤� */
		kwd_num[n_kwd_num].assign(keyword + n_begin, n_spn);
		n_kwd_num ++;
	    }
	    off = i;
	    for ( ; keyword[i] != '\0' ; i++ ) {
		if ( '0' <= keyword[i] && keyword[i] <= '9' ) break;
	    }
	    kwd_with_sh.append(keyword + off, i - off);
	    if ( keyword[i] != '\0' ) kwd_with_sh.append('#',1);
	}
    }
   
    /* for IMAGE HDU */
    if ( hdutype == FITS::IMAGE_HDU ) {
	tstring kwd_with_x;
	if ( return_comment.cstr() == NULL ) {
	    /* ���̤ξ��: ���줬��ͥ�� */
	    return_comment = Image_comments.cstr(keyword);
	}
	/* WCS �Υ�����ɤϤ��ä������ɤ����� */
	if ( 0 < kwd_with_sh.length() ) {
	    if ( return_comment.cstr() == NULL ) {
		/* PC#_# �Τ褦�ʤ�Τ� */
		return_comment = Image_comments.cstr(kwd_with_sh.cstr());
	    }
	    if ( return_comment.cstr() == NULL ) {
		/* PC1_1  => PC#_#? �Τ褦�ʤ�Τ� */
		kwd_with_x.assign(kwd_with_sh).append('?',1);
		return_comment = Image_comments.cstr(kwd_with_x.cstr());
	    }	    
	    if ( return_comment.cstr() == NULL ) {
		/* PC1_1A => PC#_#? �Τ褦�ʤ�Τ� */
		kwd_with_x.assign(kwd_with_sh);
		if ( 'A' <= kwd_with_x.cchr(kwd_with_x.length()-1) &&
		     kwd_with_x.cchr(kwd_with_x.length()-1) <= 'Z' ) {
		    kwd_with_x.replace(kwd_with_x.length()-1,1,"?");
		    return_comment = Image_comments.cstr(kwd_with_x.cstr());
		}
	    }
	}
	if ( return_comment.cstr() == NULL ) {
	    /* LONPOLE  => LONPOLE? �Τ褦�ʤ�Τ� */
	    kwd_with_x.assign(keyword).append('?',1);
	    return_comment = Image_comments.cstr(kwd_with_x.cstr());
	}	    
	if ( return_comment.cstr() == NULL ) {
	    /* LONPOLEA => LONPOLE? �Τ褦�ʤ�Τ� */
	    kwd_with_x.assign(keyword);
	    if ( 'A' <= kwd_with_x.cchr(kwd_with_x.length()-1) &&
		 kwd_with_x.cchr(kwd_with_x.length()-1) <= 'Z' ) {
		kwd_with_x.replace(kwd_with_x.length()-1,1,"?");
		return_comment = Image_comments.cstr(kwd_with_x.cstr());
	    }
	}
    }
    /* for Binary Table HDU */
    else if ( hdutype == FITS::BINARY_TABLE_HDU ) {
	if ( return_comment.cstr() == NULL ) 
	    return_comment = Binary_table_comments.cstr(keyword);
	if ( return_comment.cstr() == NULL && 0 < kwd_with_sh.length() ) {
	    return_comment = Binary_table_comments.cstr(kwd_with_sh.cstr());
	    /* Special case */
	    if ( kwd_with_sh.strcmp("TFORM#") == 0 && value != NULL ) {
		int dtp;
		const char *tc = NULL;
		fits_table_col col;
		fits::table_def tdef = { "JUNK","", NULL,NULL, 
					 "","", "", "", "" };
		tdef.tform = value;
		col.define(tdef);
		/* check heap or not */
		if ( col.heap_is_used() == true ) dtp = col.heap_type();
		else dtp = col.type();
		/* get data type */
		if ( dtp == FITS::BIT_T )                tc = "BIT";
		else if ( dtp == FITS::BYTE_T )          tc = "BYTE";
		else if ( dtp == FITS::LOGICAL_T )       tc = "1-byte LOGICAL";
		else if ( dtp == FITS::SHORT_T )         tc = "2-byte INTEGER";
		else if ( dtp == FITS::LONG_T )          tc = "4-byte INTEGER";
		else if ( dtp == FITS::LONGLONG_T )      tc = "8-byte INTEGER";
		else if ( dtp == FITS::FLOAT_T )         tc = "4-byte REAL";
		else if ( dtp == FITS::DOUBLE_T )        tc = "8-byte REAL";
		else if ( dtp == FITS::COMPLEX_T )       tc = "COMPLEX";
		else if ( dtp == FITS::DOUBLECOMPLEX_T ) tc = "DOUBLE COMPLEX";
		else if ( dtp == FITS::STRING_T )        tc = "STRING";
		/* append type info to comment string */
		if ( col.heap_is_used() == true ) {
		    if ( tc != NULL ) 
			return_comment.appendf(" : variable length of %s", tc);
		    else
			return_comment.append(" : variable length array");
		}
		else {
		    if ( tc != NULL ) 
			return_comment.appendf(" : %s", tc);
		}
	    }
	}
    }
    /* for ASCII Table HDU */
    else if ( hdutype == FITS::ASCII_TABLE_HDU ) {
	if ( return_comment.cstr() == NULL ) 
	    return_comment = Ascii_table_comments.cstr(keyword);
	if ( return_comment.cstr() == NULL && 0 < kwd_with_sh.length() )
	    return_comment = Ascii_table_comments.cstr(kwd_with_sh.cstr());
    }
    /* fallback */
    if ( return_comment.cstr() == NULL ) {
	return_comment = Fallback_comments.cstr(keyword);
    }
    if ( return_comment.cstr() == NULL && 0 < kwd_with_sh.length() ) {
	return_comment = Fallback_comments.cstr(kwd_with_sh.cstr());
    }

    /* ��������� # �ˤ��б����� */
    if ( 0 < n_kwd_num ) {
	ssize_t pos = 0;
	i = 0;
	while ( 0 <= (pos=return_comment.find(pos,'#')) ) {
	    size_t spn = return_comment.strspn(pos,'#');
	    if ( spn == 1 ) {
		return_comment.replace(pos,spn, kwd_num[i]);
	    }
	    pos += spn;
	    i++;
	}
    }

    //err_report1(__FUNCTION__,"DEBUG","keyword = [%s]",keyword);
    //err_report1(__FUNCTION__,"DEBUG","tst = [%s]",Fallback_comments.cstr(keyword));
    //err_report1(__FUNCTION__,"DEBUG","return = [%s]",return_comment.cstr());

    return_value = return_comment.cstr();

 quit:
    return return_value;
}

}	/* namespace fits */

/* constructor */
/**
 * @brief  ���󥹥ȥ饯��
 */
fits_header_record::fits_header_record()
{
    this->keyword_protected_rec = false;
    this->value_type_protected_rec = false;
    this->value_protected_rec = false;
    this->comment_protected_rec = false;
    this->rec.keyword = NULL;
    this->rec.value = NULL;
    this->rec.comment = NULL;
    this->bvalue_rec = false;
    this->dvalue_rec = NAN;
    this->llvalue_rec = INDEF_LLONG;
    this->manager = NULL;
    return;
}

/* copy constructor */
/**
 * @brief  ���ԡ����󥹥ȥ饯��
 */
fits_header_record::fits_header_record(const fits_header_record &obj)
{
    this->keyword_protected_rec = false;
    this->value_type_protected_rec = false;
    this->value_protected_rec = false;
    this->comment_protected_rec = false;
    this->rec.keyword = NULL;
    this->rec.value = NULL;
    this->rec.comment = NULL;
    this->bvalue_rec = false;
    this->dvalue_rec = NAN;
    this->llvalue_rec = INDEF_LLONG;
    this->manager = NULL;
    this->_init(obj);
    return;
}

/* destructor */
/**
 * @brief  �ǥ��ȥ饯��
 */
fits_header_record::~fits_header_record()
{
    return;
}

/*
 * Special member functions
 */

/* fits_header class �� this->manager �򥻥åȤ������֤ǥ����Ȥ�ʤ��� */
/**
 * @brief  ���֥������ȤΥ��ԡ�
 */
fits_header_record &fits_header_record::operator=(const fits_header_record &obj)
{
    this->init(obj);
    return *this;
}

/* fits_header class �� this->manager �򥻥åȤ������֤ǥ����Ȥ�ʤ��� */
/**
 * @brief  ���֥������Ȥν����
 */
fits_header_record &fits_header_record::init()
{
    if ( this->manager != NULL ) {
	long i;
	fits::header_def tmp_def = {"","",""};	/* �õ���ߤ� */
	for ( i=0 ; i < this->manager->length() ; i++ ) {
	    if ( &(this->manager->record(i)) == this ) {
		/* index�����Τ��ᡤ����Ū�� this->assign_any() ��Ƥ� */
		//err_report(__FUNCTION__,"DEBUG","calling manager!!");
		this->manager->assign(i, tmp_def);
		break;
	    }
	}
	if ( i == this->manager->length() ) {
	    err_throw(__FUNCTION__,"FATAL",
		      "internal error; fits_header does not manage me...");
	}
    }
    else {
	this->_init();
    }
    return *this;
}

/* protected */
/**
 * @brief  ���٥�ʽ����
 *
 * @note   ���Υ��дؿ��� protected �Ǥ���
 */
fits_header_record &fits_header_record::_init()
{
    this->keyword_protected_rec = false;
    this->value_type_protected_rec = false;
    this->value_protected_rec = false;
    this->comment_protected_rec = false;
    this->arr_rec = NULL;
    this->rec.keyword = NULL;
    this->rec.value = NULL;
    this->rec.comment = NULL;
    this->svalue_rec.init();
    this->bvalue_rec = false;
    this->dvalue_rec = NAN;
    this->llvalue_rec = INDEF_LLONG;
    this->a_formatted_rec = NULL;
    return *this;
}

/* fits_header class �� this->manager �򥻥åȤ������֤ǥ����Ȥ�ʤ��� */
/**
 * @brief  ���֥������ȤΥ��ԡ�
 */
fits_header_record &fits_header_record::init(const fits_header_record &obj)
{
    if ( &obj == this ) return *this;

    if ( this->manager != NULL ) {
	long i;

	this->fits_header_record::init();

	if ( this->status() == FITS::NULL_RECORD ||
	     this->status() == obj.status() ) {
	    for ( i=0 ; i < this->manager->length() ; i++ ) {
		if ( &(this->manager->record(i)) == this ) {
		    /* index�����Τ��ᡤ����Ū�� this->assign_any() ��Ƥ� */
		    //err_report(__FUNCTION__,"DEBUG","calling manager!!");
		    this->manager->assign(i, obj.raw_record());
		    break;
		}
	    }
	    if ( i == this->manager->length() ) {
		err_throw(__FUNCTION__,"FATAL",
			  "internal error; fits_header does not manage me...");
	    }
	}
    }
    else {
	this->_init(obj);
    }

    return *this;
}

/* protected */
/**
 * @brief  ���٥�ʥ��֥������ȤΥ��ԡ�
 *
 * @note   ���Υ��дؿ��� protected �Ǥ���
 */
fits_header_record &fits_header_record::_init(const fits_header_record &obj)
{
    this->fits_header_record::_init();

    this->keyword_protected_rec = obj.keyword_protected_rec;
    this->value_type_protected_rec = obj.value_type_protected_rec;
    this->value_protected_rec = obj.value_protected_rec;
    this->comment_protected_rec = obj.comment_protected_rec;
    try {
	this->arr_rec = obj.arr_rec;
	this->svalue_rec = obj.svalue_rec;
	this->bvalue_rec = obj.bvalue_rec;
	this->dvalue_rec = obj.dvalue_rec;
	this->llvalue_rec = obj.llvalue_rec;
	this->a_formatted_rec = obj.a_formatted_rec;
    }
    catch (...) {
	err_throw(__FUNCTION__,"FATAL","'=' failed");
    }
    this->rec.keyword = this->arr_rec.cstr(KEYWORD_IDX);
    this->rec.value = this->arr_rec.cstr(VALUE_IDX);
    this->rec.comment = this->arr_rec.cstr(COMMENT_IDX);

    return *this;
}

/* fits_header class �� this->manager �򥻥åȤ������֤ǥ����Ȥ�ʤ��� */
/**
 * @brief  ���֥������Ȥ����Ƥι��� (fits::header_def ��¤�Τǻ���)
 */
fits_header_record &fits_header_record::assign( const fits::header_def &def )
{
    if ( this->manager != NULL ) {
	long i;
	for ( i=0 ; i < this->manager->length() ; i++ ) {
	    if ( &(this->manager->record(i)) == this ) {
		/* index�����Τ��ᡤ����Ū�� this->assign_any() ��Ƥ� */
		this->manager->assign(i, def);
		break;
	    }
	}
	if ( i == this->manager->length() ) {
	    err_throw(__FUNCTION__,"FATAL",
		      "internal error; fits_header does not manage me...");
	}
    }
    else {
	this->assign_any(def);
    }
    return *this;
}

/* fits_header class �� this->manager �򥻥åȤ������֤ǥ����Ȥ�ʤ��� */
/**
 * @brief  ���֥������Ȥ����Ƥι��� (fits_header_record ���֥������Ȥǻ���)
 */
fits_header_record &fits_header_record::assign( const fits_header_record &obj )
{
    if ( &obj == this ) return *this;

    this->init();
    
    if ( this->status() == FITS::NULL_RECORD ||
	 this->status() == obj.status() ) {
	this->assign(obj.raw_record());
    }

    return *this;
}

/* fits_header class �� this->manager �򥻥åȤ������֤ǥ����Ȥ�ʤ��� */
/**
 * @brief  ���֥������Ȥ����Ƥι��� (keyword, value, comment�ǻ���)
 */
fits_header_record &fits_header_record::assign( const char *keyword,
						const char *value,
						const char *comment )
{
    if ( this->status() == FITS::NORMAL_RECORD ) {
	fits::header_def def = {keyword,value,comment};
	this->assign(def);
    }
    else if ( keyword != NULL && keyword[0] != '\0' ) {
	if ( value   == NULL ) value   = "";
	if ( comment == NULL ) comment = "";
	this->init();
	if ( this->status() == FITS::NULL_RECORD ) {
	    fits::header_def def = {keyword,value,comment};
	    this->assign(def);
	}
    }
    else if ( keyword != NULL && keyword[0] == '\0' &&
	      ( (value != NULL && value[0] == '\0') ||
		(comment != NULL && comment[0] == '\0') ) ) {
	this->init();
    }
    return *this;
}

/* fits_header class �� this->manager �򥻥åȤ������֤ǥ����Ȥ�ʤ��� */
/**
 * @brief  ���֥������Ȥ����Ƥι��� (keyword, description�ǻ���)
 */
fits_header_record &fits_header_record::assign( const char *keyword,
						const char *description )
{
    if ( this->status() == FITS::DESCRIPTION_RECORD ) {
	fits::header_def def = {keyword,description,NULL};
	this->assign(def);
    }
    else if ( (keyword != NULL && keyword[0] != '\0') ||
	      (description != NULL && description[0] != '\0') ) {
	if ( keyword == NULL ) keyword = "";
	if ( description == NULL ) description = "";
	this->init();
	if ( this->status() == FITS::NULL_RECORD ) {
	    fits::header_def def = {keyword,description,NULL};
	    this->assign(def);
	}
    }
    else if ( keyword != NULL && keyword[0] == '\0' &&
	      description != NULL && description[0] == '\0' ) {
	this->init();
    }
    return *this;
}

/* protected */

/**
 * @brief  �إå��쥳���ɤ��ݸ�˴ؤ�������
 *
 * @note   �ݥ��󥿤�ĥ�ä������ä��ʤ��褦���ݸ�뤿���ɬ�ס�<br>
 *         ���Υ��дؿ��� protected �Ǥ���
 */
fits_header_record &fits_header_record::set_protections( bool keyword, bool value_type, 
							 bool value, bool comment )
{
    this->keyword_protected_rec = keyword;
    this->value_type_protected_rec = value_type;
    this->value_protected_rec = value;
    this->comment_protected_rec = comment;
    return *this;
}

/**
 * @brief  ������ tarray_tstring ���֥������Ȥؤλ��Ȥ��֤�
 *
 * @note   ���Υ��дؿ��� protected �Ǥ���
 */
tarray_tstring &fits_header_record::arr()
{
    return this->arr_rec;
}

/**
 * @brief  ������ tarray_tstring ���֥������Ȥؤλ��Ȥ��֤� (�ɼ�����)
 *
 * @note   ���Υ��дؿ��� protected �Ǥ���
 */
const tarray_tstring &fits_header_record::arr_cs() const
{
    return this->arr_rec;
}

/**
 * @brief  ���Ȥ�������� fits_header ���֥������Ȥ���Ͽ
 *
 * @note   ���Υ��дؿ��� protected �Ǥ���
 */
fits_header_record &fits_header_record::register_manager( fits_header *ptr )
{
    this->manager = ptr;
    return *this;
}

/* **** internal functions **** */

/**
 * @brief  �� value ���鷿��Ƚ�ꤷ���Ʒ����ͤ��֤� (������)
 *
 * @param   value ����ʸ������
 * @param   ret_bool_value bool������ (return)
 * @param   ret_longlong_value long long������ (return)
 * @param   ret_double_value double������ (return)
 * @param   ret_str_begin ʸ����γ��ϰ��� (return)
 * @param   ret_str_len ʸ�����Ĺ�� (return)
 * @param   remove_str_quote ʸ������Υ��󥰥륯�����Ȥ�����ʤ� true��
 *          ��������ʸ������Ρ�''�פϤ��ΤޤޤǤ��롥
 * @return  ������
 * @note    private �ʴؿ��Ǥ���
 */
static int get_type_and_values( const char *value, 
				bool *ret_bool_value,
				long long *ret_longlong_value,
				double *ret_double_value,
				const char **ret_str_begin, 
				size_t *ret_str_len,
				bool remove_str_quote )
{
    int return_value = FITS::ANY_T;
    size_t endpos;
    const char *p0;
    const char *p00;
    const char *p1;
    size_t len_value;
    tstring sval;

    if ( value == NULL ) {
	if ( ret_str_begin != NULL ) *ret_str_begin = NULL;
	if ( ret_str_len != NULL ) *ret_str_len = 0;
	goto quit;
    }

    /* value �κ����ζ����������ʬ�ξ������� */
    /* p0    v    */    /* p0    v       */
    /*    "  T  " */    /*    "  TUVW  " */
    /* p1    A    */    /* p1       A    */
#if 1		/* Improved code by K. Matsuzaki (ISAS/JAXA). 2015.06.16 */
    len_value = 0;
    for ( p0 = value ; *p0 != '\0' ; p0++ ) {
	if ( *p0 != ' ' ) break;
	len_value ++;
    }
    p1 = p0;
    for ( p00 = p0 ; *p00 != '\0' ; p00++ ) {
	if ( *p00 != ' ' ) p1 = p00;
	len_value ++;
    }
    sval.assign(p0, p1 - p0 + 1);
#else
    len_value = sval.assign(value).length();
    p0 = value + (len_value - sval.ltrim().length());
    p1 = p0 + sval.rtrim().length();
    if ( p0 < p1 ) p1--;
#endif

    //sli__eprintf("fits_header_record:: value = [%s]\n", value);
    //sli__eprintf("fits_header_record:: sval = [%s]\n", sval.cstr());
    //sli__eprintf("fits_header_record:: p0 = %llx p1 = %llx\n",
    //		 (long long)p0,(long long)p1);
    //sli__eprintf("fits_header_record:: len_value = %zd\n",
    //		 len_value);

    //for ( p0=value ; *p0 == ' ' ; p0++ );	/* skipping ' ' */
    //for ( p1=value+len_value ; (*p1 == ' ' || *p1 == '\0') && p0 < p1 ; p1-- );
    if ( *p0 != '\0' ) {
    	return_value = FITS::STRING_T;
    }

    //sli__eprintf(" fits_header_record:: p0 = %llx p1 = %llx\n",
    //		  (long long)p0,(long long)p1);

    if ( 0 < sval.length() ) {
	long long longlong_value = sval.strtoll(10,&endpos);
	if ( endpos == sval.length() ) {
	    return_value = FITS::LONGLONG_T;
	    if ( ret_longlong_value != NULL ) 
		*ret_longlong_value = longlong_value;
	    goto next;
	}
    }

    if ( 0 < sval.length() ) {
	double double_value = sval.strtod(&endpos);
	if ( endpos == sval.length() ) {
	    return_value = FITS::DOUBLE_T;
	    if ( ret_double_value != NULL ) {
		*ret_double_value = double_value;
	    }
	    goto next;
	}
    }

    /* ʣ�ǿ����ɤ���������å� */
    if ( 0 < sval.length() ) {
	double v0, v1;
	if ( sval.at(0) == '(' ) {
	    size_t pos = 1;
	    pos += sval.strspn(pos," ");
	    v0 = sval.strtod(pos,&endpos);
	    if ( pos < endpos ) {
		pos = endpos;
		pos += sval.strspn(pos," ");
		if ( sval.at(pos) == ',' ) {
		    pos += 1 + sval.strspn(pos+1," ");
		    v1 = sval.strtod(pos,&endpos);
		    if ( pos < endpos ) {
			pos = endpos;
			pos += sval.strspn(pos," ");
			if ( sval.at(pos) == ')' ) {
			    return_value = FITS::DOUBLECOMPLEX_T;
			    if ( ret_double_value != NULL ) {
				*ret_double_value = v0;
			    }
			    goto next;
			}
		    }
		}
	    }
	}
    }

    if ( p0 == p1 && ( *p0 == 'T' || *p0 == 'F' ) ) {
	return_value = FITS::LOGICAL_T;
	if ( ret_bool_value != NULL ) {
	    if ( *p0 == 'T' ) *ret_bool_value = true;
	    else *ret_bool_value = false;
	}
	goto next;
    }

 next:
    if ( ret_str_begin != NULL && ret_str_len != NULL ) {
	if ( *p0 == '\0' ) {
	    *ret_str_begin = p0;
	    *ret_str_len = 0;
	}
	else {
	    if ( remove_str_quote == false ) {
		*ret_str_begin = p0;
		*ret_str_len = p1 - p0 + 1;
	    }
	    else if ( *p0 != '\'' ) {
		if ( *p1 == '\'' ) {
		    sli__eprintf("fits_header_record:: "
			    "[WARNING] unexpected string value \"%s\"\n",
			    value);
		    for ( p1-- ; *p1 == ' ' && p0 < p1 ; p1-- );
		}
		*ret_str_begin = p0;
		*ret_str_len = p1 - p0 + 1;
	    }
	    else {	/* if ( *p0 == '\'' ) */
		if ( p0 == p1 ) {
		    sli__eprintf("fits_header_record:: "
			    "[WARNING] unexpected string value \"%s\", "
			    "ignored\n",value);
		    *ret_str_begin = value + len_value;
		    *ret_str_len = 0;
		}
		else if ( p0 + 1 == p1 ) {
		    if ( *p1 == '\'' ) {
			*ret_str_begin = value + len_value;
			*ret_str_len = 0;
		    } else {	/* broken */
			sli__eprintf("fits_header_record:: "
				"[WARNING] unexpected string value \"%s\"\n",
				value);
			*ret_str_begin = p0 + 1;
			*ret_str_len = 1;
		    }
		}
		else {
		    /* "' '" */
		    /* "' a" */
		    for ( p0++ ; *p0 == ' ' ; p0++ );	/* skipping ' ' */
		    if ( *p1 == '\'' && p0 < p1 ) p1--;
		    for ( ; (*p1 == ' ' || *p1 == '\0') && p0 < p1 ; p1-- );
		    if ( (*p0 == '\'' && p0 == p1 && p1[1] != '\'' ) 
			 || *p0 == '\0' ) {
			*ret_str_begin = value + len_value;
			*ret_str_len = 0;
		    }
		    else {
			*ret_str_begin = p0;
			*ret_str_len = p1 - p0 + 1;
		    }
		}
	    }
	}
    }

 quit:
    return return_value;
}

/**
 * @brief  �� value ���鷿��Ƚ�ꤷ���Ʒ����ͤ��֤� (������)
 *
 * @return  ������
 * @note    private �ʴؿ��Ǥ���
 */
static int get_type( const char *value )
{
    return get_type_and_values(value,NULL,NULL,NULL,NULL,NULL,false);
}

/* protected */

/**
 * @brief  ������ɤ����� (printf()�ε�ˡ)
 *
 * @note   ���Υ��дؿ��� protected �Ǥ���
 */
fits_header_record &fits_header_record::assignf_keyword( const char *format, ... )
{
    va_list ap;

    va_start(ap,format);
    try {
	this->vassignf_keyword(format,ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","this->vassignf_keyword() failed");
    }
    va_end(ap);

    return *this;
}

/**
 * @brief  ������ɤ����� (printf()�ε�ˡ)
 *
 * @note   ���Υ��дؿ��� protected �Ǥ���
 */
fits_header_record &fits_header_record::vassignf_keyword( const char *format, va_list ap )
{
    fits::header_def rv = {NULL,NULL,NULL};
    if ( format == NULL ) {
	this->assign_any(rv);
    }
    else {
	tstring tstr;
	tstr.vassignf(format,ap);
	rv.keyword = tstr.cstr();
	this->assign_any(rv);
    }
    return *this;
}

/* public */

/**
 * @brief  ������ʸ��������� (printf()�ε�ˡ)
 */
fits_header_record &fits_header_record::assignf_comment( const char *format, ... )
{
    va_list ap;

    va_start(ap,format);
    try {
	this->vassignf_comment(format,ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","this->vassignf_comment() failed");
    }
    va_end(ap);

    return *this;
}

/**
 * @brief  ������ʸ��������� (printf()�ε�ˡ)
 */
fits_header_record &fits_header_record::vassignf_comment( const char *format, va_list ap )
{
    fits::header_def rv = {NULL,NULL,NULL};
    if ( format == NULL ) {
	this->assign_any(rv);
    }
    else {
	tstring tstr;
	tstr.vassignf(format,ap);
	rv.comment = tstr.cstr();
	this->assign_any(rv);
    }
    return *this;
}

/**
 * @brief  ������ʸ���������
 */
fits_header_record &fits_header_record::assign_comment( const char *str )
{
    fits::header_def rv = {NULL,NULL,NULL};
    rv.comment = str;
    this->assign_any(rv);
    return *this;
}

/**
 * @brief  �����ȼ����¸�ߤ��륳����ʸ����򼫿ȤΥ쥳���ɤ�����
 *
 *  ���ȤΥ�����ɤ������ȼ����¸�ߤ����硤�����ȼ���ˤ��륳����
 *  ʸ����򼫿ȤΥ쥳���ɤ˽񤭹��ߤޤ���
 */
fits_header_record &fits_header_record::assign_default_comment( int hdutype )
{
    if ( this->status() == FITS::NORMAL_RECORD ) {
	if ( hdutype == FITS::ANY_HDU ) {
	    if ( this->manager != NULL ) hdutype = this->manager->hdutype();
	}
	fits::header_def rv = {NULL,NULL,NULL};
	tstring ret;
	rv.comment = fits::get_default_comment(hdutype, this->keyword(), 
					       this->svalue(), &ret);
	this->assign_any(rv);
    }
    return *this;
}

/* protected */

/**
 * @brief  ���Ƥι��� (���٥�)
 *
 * @note   ���Υ��дؿ��� protected �Ǥ���
 */
fits_header_record &fits_header_record::assign_any( const fits::header_def &arg_def )
{
    tstring new_keyword;
    tstring new_value;
    tstring new_comment;
    fits::header_def def = {NULL,NULL,NULL};
    bool be_description = false;
    bool is_updated = false;		/* this->arr_rec ���������줿��true */

    def.keyword = arg_def.keyword;
    def.value = arg_def.value;
    def.comment = arg_def.comment;

    /* ������ɤ˶���Ϳ����졤�����ͤ������Ȥ˶���Ϳ����줿���� */
    /* �쥳���ɤν������Ԥʤ� */
    if ( this->keyword_protected_rec == false ) {
	if ( (def.keyword != NULL && def.keyword[0] == '\0') &&
	     ( (def.value != NULL && def.value[0] == '\0') ||
	       (def.comment != NULL && def.comment[0] == '\0') ) ) {
	    this->_init();
	    return *this;
	}
    }

    /* ���� or �����ȹ� */
    if ( this->arr_rec.cstr(COMMENT_IDX) == NULL ) {
	/* comment �� description ��񤤤Ƥ��ɤ����ˤ��� */
	if ( arg_def.value == NULL ) {
	    def.value = arg_def.comment;
	    def.comment = arg_def.value;
	}
	/* �����ȹ� */
	if ( this->arr_rec.cstr(KEYWORD_IDX) != NULL ) {
	    /* description �ξ��� description �Τޤ� */
	    def.comment = NULL;
	    be_description = true;
	}
	else {
	    /* �����ξ�� */
	    /* ξ�� NULL �ʤ鲿�⤷�ʤ� */
	    if ( def.keyword == NULL && def.value == NULL ) {
		return *this;
	    }
	    if ( def.keyword == NULL ) def.keyword = "";
	    if ( def.value == NULL ) def.value = "";
	    if ( def.comment == NULL ) be_description = true;
	}
    }

    /* �����ΰ����ꤵ��Ƥ��ɤ��褦�ˤ��� */
    if ( def.keyword != NULL ) {
	ssize_t pos = 0;
	bool disp = false;
	const char *ch_set;
	new_keyword.assign(def.keyword).strtrim(" ");
	if ( FITS::HEADER_KEYWORD_MAX_LENGTH < new_keyword.length() ) {
	    err_report1(__FUNCTION__,"WARNING",
			"too long keyword [%s], truncated",
			new_keyword.cstr());
	}
	new_keyword.assign(new_keyword.cstr(),
			   FITS::HEADER_KEYWORD_MAX_LENGTH);
	new_keyword.assign(new_keyword.cstr()).strtrim(" ");
	/* "HIERARCH " �ǻϤޤ���϶������Ĥ��� */
	if ( new_keyword.strncmp("HIERARCH ",9)==0 ) ch_set = "=";
	else ch_set = " =";
	while ( 0 <= (pos=new_keyword.find_first_of(pos,ch_set)) ) {
	    if ( disp == false ) {
		err_report1(__FUNCTION__,"WARNING",
			    "'=' or ' ' in keyword [%s] is replaced with '.'",
			    new_keyword.cstr());
		disp = true;
	    }
	    new_keyword.replace(pos,1,'.',1);
	    pos ++;
	}
    }
    if ( def.value != NULL ) {
	const char *spc;
	if ( be_description == false ) spc=" \t\n\r\f\v";
	else spc="";
	new_value.assign(def.value).strtrim(spc);
    }
    if ( def.comment != NULL ) {
        //new_comment.assign(def.comment);
        new_comment.assign(def.comment).strtrim(" ");
    }

    if ( new_keyword.cstr() != NULL ) {
	if ( this->keyword_protected_rec == false ) {
	    this->arr_rec.put(KEYWORD_IDX, new_keyword.cstr(),1);
	    this->rec.keyword = this->arr_rec.cstr(KEYWORD_IDX);
	    this->rec.value = this->arr_rec.cstr(VALUE_IDX);
	    this->rec.comment = this->arr_rec.cstr(COMMENT_IDX);
	    is_updated = true;
	}
	else {
	    err_report1(__FUNCTION__,"WARNING",
			"keyword='%s' : keyword is protected",
			this->rec.keyword);
	}
    }

    if ( new_comment.cstr() != NULL ) {
	if ( this->comment_protected_rec == false ) {
	    this->arr_rec.put(COMMENT_IDX, new_comment.cstr(),1);
	    this->rec.keyword = this->arr_rec.cstr(KEYWORD_IDX);
	    this->rec.value = this->arr_rec.cstr(VALUE_IDX);
	    this->rec.comment = this->arr_rec.cstr(COMMENT_IDX);
	    is_updated = true;
	}
	else {
	    err_report1(__FUNCTION__,"WARNING",
			"keyword='%s' : comment is protected",
			this->rec.keyword);
	}
    }

    if ( new_value.cstr() != NULL ) {
	if ( this->value_protected_rec == false ) {
	    if ( be_description == true ) {
		/* description �����ξ��ϡ����Τޤ�ޤ�ʸ�������� */
	        this->arr_rec.putf(VALUE_IDX, 1,"%s",new_value.cstr());
		this->rec.keyword = this->arr_rec.cstr(KEYWORD_IDX);
		this->rec.value = this->arr_rec.cstr(VALUE_IDX);
		this->rec.comment = this->arr_rec.cstr(COMMENT_IDX);
		/* */
		this->svalue_rec.init();
		this->bvalue_rec = false;
		this->dvalue_rec = NAN;
		this->llvalue_rec = INDEF_LLONG;
		is_updated = true;
	    }
	    else {
		const char *str_begin;
		size_t str_len;
		int current_value_type = FITS::ANY_T;
		int new_value_type;
		/* �����ǤϾ�� false */
		const bool rm_str_quote = false;
		
		if ( this->value_type_protected_rec == true ) {
		    current_value_type = get_type(this->rec.value);
		}

		/* str_begin,str_len�ˤϺ����ζ����������ʬ�ξ��󤬤��� */
		new_value_type = get_type_and_values( new_value.cstr(),
						      NULL,NULL,NULL,
						      &str_begin,&str_len,
						      rm_str_quote );

		if ( current_value_type == FITS::ANY_T ||
		     current_value_type == new_value_type ) {
		    tstring fmt;

		    if ( new_value_type == FITS::ANY_T ) {
		        this->arr_rec.put(VALUE_IDX, "",1);
		    }
		    else if ( new_value_type == FITS::STRING_T ) {
			if ( str_len == 0 ) {
			    this->arr_rec.put(VALUE_IDX, "",1);
			}
			else {
			    /* ɬ�פʤ饷�󥰥륯�����Ȥ��ɲä��� */
			    tstring str_val;
			    bool quot_begin_ok = false;
			    size_t i=0, ii=0;

			    str_val.resize(2 + 2 * str_len);

			    if ( str_begin[i] != '\'' ) {
				str_val[ii] = '\'';
				ii ++;
			    }
			    else quot_begin_ok = true;

			    for ( i=0 ; i < str_len ; i++ ) {
				if ( str_begin[i] == '\'' ) {
				    if ( i == 0 ) {
					/* OK */
				    }
				    else if ( i + 1 < str_len ) {
					/* ����Υ��󥰥륯������ */
					str_val[ii] = '\'';  ii ++;
					if ( str_begin[i+1] == '\'' ) i ++;
				    }
				    else {
					/* ���󥰥륯�����Ȥǽ���äƤ��� */
					break;
				    }
				}
				str_val[ii] = str_begin[i];
				ii ++;
			    }
			    /* ���󥰥륯�����ȤǻϤޤäƤ��ʤ���� */
			    if ( quot_begin_ok == false ) {
				while ( ii < 1 + FITS::HEADER_STRING_VALUE_ALIGNED_LENGTH ) {
				    str_val[ii] = ' ';
				    ii ++;
				}
			    }
			    /* �Ǹ�Υ��󥰥륯������ */
			    str_val[ii] = '\'';
			    ii ++;
			    
			    fmt.printf("%%.%llds",(long long)ii);
			    this->arr_rec.putf(VALUE_IDX,
					       1,fmt.cstr(),str_val.cstr());
			}
		    }
		    else {
			/* str_begin, str_len �γ�����ʬ������¸ */
			fmt.printf("%%.%llds",(long long)str_len);
			this->arr_rec.putf(VALUE_IDX,
					   1,fmt.cstr(),str_begin);
		    }

		    this->rec.keyword = this->arr_rec.cstr(KEYWORD_IDX);
		    this->rec.value = this->arr_rec.cstr(VALUE_IDX);
		    this->rec.comment = this->arr_rec.cstr(COMMENT_IDX);
#if 1
		    this->update_value_recs();
#else
		    this->update_svalue_rec();
		    this->update_bvalue_rec();
		    this->update_dvalue_rec();
		    this->update_llvalue_rec();
#endif
		    is_updated = true;

		}
		else {
		    if ( new_value.cstr() != NULL ) {
			err_report1(__FUNCTION__,"WARNING",
				    "keyword='%s' : value type is protected",
				    this->rec.keyword);
		    }
		}
	    }
	}
	else {
	    err_report1(__FUNCTION__,"WARNING",
			"keyword='%s' : value is protected",
			this->rec.keyword);
	}
    }

    /* �������줿��硤80-char ����¸���Ƥ���Хåե����������� */
    if ( is_updated == true ) {
	this->a_formatted_rec = NULL;
    }

    return *this;
}

#if 1		/* Improved code by K. Matsuzaki (ISAS/JAXA). 2015.06.07 */

/**
 * @brief  double���͡�ʸ����(���٥�)�� ���Ƥ���������å���򹹿�
 *
 * @note   ���Υ��дؿ��� protected �Ǥ���
 */
fits_header_record &fits_header_record::update_value_recs()
{
    int type;

    bool bool_value = false;
    double double_value = NAN;
    long long longlong_value = INDEF_LLONG;
    bool new_bool_value;
    double new_double_value;
    long long new_longlong_value;
    const char *str_begin;
    size_t str_len;

    type = get_type_and_values( this->rec.value,
				&bool_value, &longlong_value, &double_value,
				&str_begin, &str_len,
				true );

    /* bool */
    new_bool_value = bool_value;
    if ( type != FITS::LOGICAL_T && type != FITS::ANY_T ) {
	if ( type == FITS::DOUBLE_T || type == FITS::DOUBLECOMPLEX_T ) 
	    new_bool_value = (round(double_value) == 0.0) ? false : true;
	else if ( type == FITS::LONGLONG_T ) 
	    new_bool_value = (longlong_value == 0) ? false : true;
    }
    this->bvalue_rec = new_bool_value;

    /* double */
    new_double_value = double_value;
    if ( type != FITS::DOUBLE_T && type != FITS::DOUBLECOMPLEX_T &&
	 type != FITS::ANY_T ) {
	if ( type == FITS::LONGLONG_T ) new_double_value = (double)longlong_value;
	else if ( type == FITS::LOGICAL_T ) 
	    new_double_value = (double)bool_value;
    }
    this->dvalue_rec = new_double_value;

    /* longlong */
    new_longlong_value = longlong_value;
    if ( type != FITS::LONGLONG_T && type != FITS::ANY_T ) {
	if ( type == FITS::DOUBLE_T || type == FITS::DOUBLECOMPLEX_T ) {
	    if ( isfinite(double_value) &&
		 MIN_DOUBLE_ROUND_LLONG <= double_value &&
		 double_value <= MAX_DOUBLE_ROUND_LLONG ) {
		new_longlong_value = c_llround(double_value);
	    }
	    else {
		new_longlong_value = INDEF_LLONG;
	    }
	} else if ( type == FITS::LOGICAL_T ) 
	    new_longlong_value = (long long)bool_value;
    }
    this->llvalue_rec = new_longlong_value;

    /* string */
    if ( str_len != 0 ) {
	ssize_t pos = 0;
	this->svalue_rec.assign(str_begin,str_len);
	/* ��''�ע���'�פ��Ѵ� */
	while ( 0 <= (pos = this->svalue_rec.find(pos,"''")) ) {
	    this->svalue_rec.replace(pos,2,"'");
	    pos++;
	}
    }
    else this->svalue_rec.assign("");

    return *this;
}

#else

/**
 * @brief  ʸ����(���٥�)����������å���򹹿�
 *
 * @note   ���Υ��дؿ��� protected �Ǥ���
 */
fits_header_record &fits_header_record::update_svalue_rec()
{
    int type;
    const char *str_begin;
    size_t str_len;

    type = get_type_and_values( this->rec.value,
				NULL,NULL,NULL,
				&str_begin,&str_len,
				true );

    if ( str_len != 0 ) {
	ssize_t pos = 0;
	this->svalue_rec.assign(str_begin,str_len);
	/* ��''�ע���'�פ��Ѵ� */
	while ( 0 <= (pos = this->svalue_rec.find(pos,"''")) ) {
	    this->svalue_rec.replace(pos,2,"'");
	    pos++;
	}
    }
    else this->svalue_rec.assign("");

    return *this;
}

/**
 * @brief  bool���ͤ���������å���򹹿�
 *
 * @note   ���Υ��дؿ��� protected �Ǥ���
 */
fits_header_record &fits_header_record::update_bvalue_rec()
{
    bool bool_value = false;
    long long longlong_value = 0;
    double double_value = 0;
    int type;

    type = get_type_and_values( this->rec.value, 
				&bool_value, &longlong_value, &double_value,
				NULL, NULL, false );
    if ( type != FITS::LOGICAL_T && type != FITS::ANY_T ) {
	if ( type == FITS::DOUBLE_T || type == FITS::DOUBLECOMPLEX_T ) 
	    bool_value = (round(double_value) == 0.0) ? false : true;
	else if ( type == FITS::LONGLONG_T ) 
	    bool_value = (longlong_value == 0) ? false : true;
    }

    this->bvalue_rec = bool_value;

    return *this;
}

/**
 * @brief  double���ͤ���������å���򹹿�
 *
 * @note   ���Υ��дؿ��� protected �Ǥ���
 */
fits_header_record &fits_header_record::update_dvalue_rec()
{
    bool bool_value = false;
    long long longlong_value = 0;
    double double_value = NAN;
    int type;

    type = get_type_and_values( this->rec.value, 
				&bool_value, &longlong_value, &double_value,
				NULL, NULL, false );
    if ( type != FITS::DOUBLE_T && type != FITS::DOUBLECOMPLEX_T &&
	 type != FITS::ANY_T ) {
	if ( type == FITS::LONGLONG_T ) double_value = (double)longlong_value;
	else if ( type == FITS::LOGICAL_T ) 
	    double_value = (double)bool_value;
    }

    this->dvalue_rec = double_value;

    return *this;
}

/**
 * @brief  long long���ͤ���������å���򹹿�
 *
 * @note   ���Υ��дؿ��� protected �Ǥ���
 */
fits_header_record &fits_header_record::update_llvalue_rec()
{
    bool bool_value = false;
    long long longlong_value = INDEF_LLONG;
    double double_value = 0;
    int type;

    type = get_type_and_values( this->rec.value, 
				&bool_value, &longlong_value, &double_value,
				NULL, NULL, false );
    if ( type != FITS::LONGLONG_T && type != FITS::ANY_T ) {
	if ( type == FITS::DOUBLE_T || type == FITS::DOUBLECOMPLEX_T ) {
	    if ( isfinite(double_value) &&
		 MIN_DOUBLE_ROUND_LLONG <= double_value &&
		 double_value <= MAX_DOUBLE_ROUND_LLONG ) {
		longlong_value = c_llround(double_value);
	    }
	    else {
		longlong_value = INDEF_LLONG;
	    }
	} else if ( type == FITS::LOGICAL_T ) 
	    longlong_value = (long long)bool_value;
    }

    this->llvalue_rec = longlong_value;

    return *this;
}

#endif

/* public (low-level) */

/**
 * @brief  �ͤ����� (���٥롦printf()�ε�ˡ)
 *
 * @note  ʸ�����ͤξ�硤ξü�Υ��󥰥륯�����Ȥ�ޤ��ʸ��������ꤷ�ޤ���
 */
fits_header_record &fits_header_record::assignf_value( const char *format, ... )
{
    va_list ap;

    va_start(ap,format);
    try {
	this->vassignf_value(format,ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","this->vassignf_value() failed");
    }
    va_end(ap);

    return *this;
}

/**
 * @brief  �ͤ����� (���٥롦printf()�ε�ˡ)
 *
 * @note  ʸ�����ͤξ�硤ξü�Υ��󥰥륯�����Ȥ�ޤ��ʸ��������ꤷ�ޤ���
 */
fits_header_record &fits_header_record::vassignf_value( const char *format, va_list ap )
{
    fits::header_def rv = {NULL,NULL,NULL};
    if ( format == NULL ) {
	this->assign_any(rv);
    }
    else {
	tstring tstr;
	tstr.vassignf(format,ap);
	rv.value = tstr.cstr();
	this->assign_any(rv);
    }
    return *this;
}

/**
 * @brief  �ͤ����� (���٥�)
 *
 * @note  ʸ�����ͤξ�硤ξü�Υ��󥰥륯�����Ȥ�ޤ��ʸ��������ꤷ�ޤ���
 */
fits_header_record &fits_header_record::assign_value( const char *str )
{
    fits::header_def rv = {NULL,NULL,NULL};
    if ( str == NULL ) {
	this->assign_any(rv);
    }
    else {
	rv.value = str;
	this->assign_any(rv);
    }
    return *this;
}

/**
 * @brief  ʸ�����ͤ����� (printf()�ε�ˡ)
 *
 * @note  ξü�Υ��󥰥륯�����Ȥȶ�����ղä���ɬ�פϤ���ޤ���
 */
fits_header_record &fits_header_record::assignf( const char *format, ... )
{
    va_list ap;

    va_start(ap,format);
    try {
	this->vassignf(format,ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","this->vassignf() failed");
    }
    va_end(ap);

    return *this;
}

/**
 * @brief  ʸ�����ͤ����� (printf()�ε�ˡ)
 *
 * @note  ξü�Υ��󥰥륯�����Ȥȶ�����ղä���ɬ�פϤ���ޤ���
 */
fits_header_record &fits_header_record::vassignf( const char *format, va_list ap )
{
    if ( format == NULL ) {
	this->assign(format);
    }
    else {
	tstring tstr;
	tstr.vassignf(format,ap);
	this->assign(tstr.cstr());
    }
    return *this;
}

/**
 * @brief  ʸ�����ͤ�����
 *
 * @note  ξü�Υ��󥰥륯�����Ȥȶ�����ղä���ɬ�פϤ���ޤ���
 */
fits_header_record &fits_header_record::assign( const char *str )
{
    fits::header_def rv = {NULL,NULL,NULL};
    if ( str == NULL ) {
	this->assign_any(rv);
    }
    else {
	tstring tstr, tstr0;
	/* ' �� '' ���֤�������Ԥʤ� */
	tstr0.assign(str).strreplace("'","''",true);
	if ( tstr0.length() < FITS::HEADER_STRING_VALUE_ALIGNED_LENGTH ) {
	    /* ������ɲ� */
	    tstr0.resize(FITS::HEADER_STRING_VALUE_ALIGNED_LENGTH);
	}
	tstr.assignf("'%s'",tstr0.cstr());
	rv.value = tstr.cstr();
	this->assign_any(rv);
    }
    return *this;
}

/**
 * @brief  bool�����ͤ�����
 */
fits_header_record &fits_header_record::assign( bool tf )
{
    fits::header_def rv = {NULL,NULL,NULL};
    if ( tf == true ) rv.value = "T";
    else rv.value = "F";
    this->assign_any(rv);
    return *this;
}

/**
 * @brief  int�����ͤ�����
 */
fits_header_record &fits_header_record::assign( int val )
{
    tstring sval;
    fits::header_def rv = {NULL,NULL,NULL};

    sval.printf("%d",val);

    rv.value = sval.cstr();
    this->assign_any(rv);

    return *this;
}

/**
 * @brief  long�����ͤ�����
 */
fits_header_record &fits_header_record::assign( long val )
{
    tstring sval;
    fits::header_def rv = {NULL,NULL,NULL};

    sval.printf("%ld",val);

    rv.value = sval.cstr();
    this->assign_any(rv);

    return *this;
}

/**
 * @brief  long long �����ͤ�����
 */
fits_header_record &fits_header_record::assign( long long val )
{
    tstring sval;
    fits::header_def rv = {NULL,NULL,NULL};

    sval.printf("%lld",val);

    rv.value = sval.cstr();
    this->assign_any(rv);

    return *this;
}

/**
 * @brief  double�����ͤ�����
 *
 * @param   val ��
 * @param   prec ����(���)����ά����15��
 * @return  ���Ȥλ���
 */
fits_header_record &fits_header_record::assign( double val, int prec )
{
    tstring sval;
    tstring fmt;
    fits::header_def rv = {NULL,NULL,NULL};
    ssize_t ep;

    fmt.printf("%%.%dG",prec);
    sval.printf(fmt.cstr(),val);

    ep = sval.strchr('E');
    /* %.xxg �ξ�硤. ���񤫤�ʤ��������� */
    if ( ep < 0 && sval.strchr('.') < 0 ) sval.append(".");
    /* E -> D �ˤ��� */
    if ( 0 <= ep ) sval.at(ep) = 'D';

    rv.value = sval.cstr();
    this->assign_any(rv);
    return *this;
}

/**
 * @brief  float�����ͤ�����
 *
 * @param   val ��
 * @param   prec ����(���)����ά����6��
 * @return  ���Ȥλ���
 */
fits_header_record &fits_header_record::assign( float val, int prec )
{
    tstring sval;
    tstring fmt;
    fits::header_def rv = {NULL,NULL,NULL};
    ssize_t ep;

    fmt.printf("%%.%dG",prec);
    sval.printf(fmt.cstr(),(double)val);

    ep = sval.strchr('E');
    /* %.xxg �ξ�硤. ���񤫤�ʤ��������� */
    if ( ep < 0 && sval.strchr('.') < 0 ) sval.append(".");

    rv.value = sval.cstr();
    this->assign_any(rv);
    return *this;
}

/* set date string [yyyy-mm-ddThh:mm:ss] */
/**
 * @brief  ���ߤ� UTC ������yyyy-mm-ddThh:mm:ss�פη���������
 *
 * @return  ���Ȥλ���
 */
fits_header_record &fits_header_record::assign_system_time()
{
    time_t t_sse;
    struct tm *t_ptr;
    char t_str[32];

    time(&t_sse);
    t_ptr = gmtime(&t_sse);

    if ( t_ptr == NULL ) {
	err_report(__FUNCTION__,"WARNING", 
		   "update failed: gmtime() returns NULL");
    }
    else {
	strftime(t_str,25, "%Y-%m-%dT%H:%M:%S",t_ptr);
        this->assign(t_str);
    }

    return *this;
}

/**
 * @brief  �쥳���ɤη��������
 *
 * @return  FITS::NORMAL_RECORD: �������,��,�����Ȥ��Ȥ߹�碌 <br>
 *          FITS::DESCRIPTION_RECORD: �������,�����Ȥ��Ȥ߹�碌 <br>
 *          FITS::NULL_RECORD: ������ɤʤ� <br>
 */
int fits_header_record::status() const
{
    if ( this->arr_rec.length() == 3 ) return FITS::NORMAL_RECORD;
    if ( this->arr_rec.length() == 2 ) return FITS::DESCRIPTION_RECORD;
    else return FITS::NULL_RECORD;
}

/**
 * @brief  �ͤη������
 *
 * @return  FITS::STRING_T: ʸ������ <br>
 *          FITS::LONGLONG_T: ������ <br>
 *          FITS::DOUBLE_T: �¿��� <br>
 *          FITS::LOGICAL_T: ������ <br>
 *          FITS::DOUBLECOMPLEX_T: ʣ�ǿ��� <br>
 *          FITS::ANY_T: ������Ƚ����ǽ
 * @attention  FITS::BYTE_T, FITS::SHORT_T, FITS::LONG_T, FITS::FLOAT_T, 
 *             FITS::COMPLEX_T ���֤����Ϥ���ޤ���
 */
int fits_header_record::type() const
{
    return get_type(this->rec.value);
}

/**
 * @brief  �������ʸ��������
 */
const char *fits_header_record::keyword() const
{
    return this->rec.keyword;
}

/**
 * @brief  �ץ���ޤΥХåե��˥������ʸ��������
 */
ssize_t fits_header_record::get_keyword(char *dest_buf, size_t buf_size) const
{
    size_t str_len = 0;

    if ( 0 < buf_size && dest_buf == NULL ) return -1;

    if ( this->rec.keyword != NULL ) {
	str_len = this->arr_rec.at_cs(KEYWORD_IDX).getstr(dest_buf,buf_size);
    }
    else {
	if ( 0 < buf_size ) dest_buf[0] = '\0';
    }

    return str_len;
}

/**
 * @brief  ������ɤ��ݻ����Ƥ����������֥������Ȥλ��Ȥ���� (�ɼ�����)
 */
const tstring &fits_header_record::keyword_cs() const
{
    return this->arr_rec.at_cs(KEYWORD_IDX);
}

/**
 * @brief  �������ʸ�����Ĺ�������
 */
size_t fits_header_record::keyword_length() const
{
    return this->arr_rec.at_cs(KEYWORD_IDX).length();
}

/**
 * @brief  ��ʸ�������� (���٥�)
 *
 * @note  ʸ�����ͤξ�硤ξü�Υ��󥰥륯�����Ȥ�ޤ��ʸ������֤��ޤ���
 */
const char *fits_header_record::value() const
{
    return this->rec.value;
}

/**
 * @brief  �ץ���ޤΥХåե�����ʸ�������� (���٥�)
 *
 * @note  ʸ�����ͤξ�硤ξü�Υ��󥰥륯�����Ȥ�ޤ��ʸ������֤��ޤ���
 */
ssize_t fits_header_record::get_value( char *dest_buf, size_t buf_size ) const
{
    size_t str_len = 0;

    if ( 0 < buf_size && dest_buf == NULL ) return -1;

    if ( this->rec.value != NULL ) {
	str_len = this->arr_rec.at_cs(VALUE_IDX).getstr(dest_buf,buf_size);
    }
    else {
	if ( 0 < buf_size ) dest_buf[0] = '\0';
    }

    return str_len;
}

/**
 * @brief  �ͤ��ݻ����Ƥ����������֥������Ȥλ��Ȥ���� (���٥롦�ɼ�����)
 *
 * @note  ʸ�����ͤξ�硤ξü�Υ��󥰥륯�����Ȥ�ޤߤޤ���
 */
const tstring &fits_header_record::value_cs() const
{
    return this->arr_rec.at_cs(VALUE_IDX);
}

/**
 * @brief  ��ʸ�����Ĺ������� (���٥�)
 *
 * @note  ʸ�����ͤξ�硤ξü�Υ��󥰥륯�����Ȥ�ޤߤޤ���
 */
size_t fits_header_record::value_length() const
{
    return this->arr_rec.at_cs(VALUE_IDX).length();
}

/**
 * @brief  ʸ�����ͤ����
 *
 * @note  ʸ�����ͤξ�硤ξü�Υ��󥰥륯�����Ȥȶ���Ͻ���줿��Τ�
 *        �֤��ޤ���
 */
const char *fits_header_record::svalue() const
{
    return this->svalue_rec.cstr();
}

/**
 * @brief  ʸ�����ͤ��ݻ����Ƥ����������֥������Ȥλ��Ȥ���� (�ɼ�����)
 *
 * @note  ʸ�����ͤξ�硤ξü�Υ��󥰥륯�����Ȥȶ���Ͻ����Ƥ��ޤ���
 */
const tstring &fits_header_record::svalue_cs() const
{
    return this->svalue_rec;
}

/**
 * @brief  ʸ�����ͤ�Ĺ�������
 *
 * @note  ʸ�����ͤξ�硤ξü�Υ��󥰥륯�����Ȥȶ���Ͻ����Ĺ���Ǥ���
 */
size_t fits_header_record::svalue_length() const
{
    return this->svalue_rec.length();
}

/**
 * @brief  �ץ���ޤΥХåե���ʸ�����ͤ����
 *
 * @note  ʸ�����ͤξ�硤ξü�Υ��󥰥륯�����Ȥȶ���Ͻ���줿��Τ�
 *        �֤��ޤ���
 */
ssize_t fits_header_record::get_svalue( char *dest_buf, 
					size_t buf_size ) const
{
    return this->svalue_rec.getstr(dest_buf,buf_size);
}

/**
 * @brief  �ͤ� bool ���Ǽ���
 */
bool fits_header_record::bvalue() const
{
    return this->bvalue_rec;
}

/**
 * @brief  �ͤ� long ���Ǽ���
 *
 * @attention  fits_header_record::type() �Ǥ� 32-bit������ 64-bit��������
 *             Ƚ��Ǥ��ޤ���
 *             32-bit OS ���θ������ϡ�fits_header_record::llvalue() ��
 *             ���Ȥ�����������
 */
long fits_header_record::lvalue() const
{
    return this->llvalue_rec;
}

/**
 * @brief  �ͤ� long long ���Ǽ���
 */
long long fits_header_record::llvalue() const
{
    return this->llvalue_rec;
}

/**
 * @brief  �ͤ� double ���Ǽ���
 */
double fits_header_record::dvalue() const
{
    return this->dvalue_rec;
}

#include "private/parse_section_expression.cc"

/* get IRAF-style section info (e.g. BIASSEC, TRIMSEC, etc.) */
/**
 * @brief  IRAF�����Υ���������������
 *
 *  IRAF �ǥ����Х�������ΰ�򼨤�������Ѥ����Ƥ��� '[3074:3104,1:512]' 
 *  �Τ褦��ʸ���󤫤�����Υ������������������ޤ�����ɽŪ�ʥ������
 *  �Ȥ��ơ�BIASSEC��TRIMSEC��DATASEC��CCDSEC��ORIGSEC ������ޤ���
 *
 * @param   ret_begin ���������γ��ϰ��� (return)
 * @param   ret_length ����������Ĺ�� (return)
 * @param   ret_flip_flag ȿž�ե饰 (return)
 * @param   buf_len ret_begin[] ���ΥХåե���Ĺ�� (����μ�����)
 * @return  ʸ����˴ޤޤ�륻����������μ�����
 * @attention  ʸ����� 1-indexed ��ɽ������ޤ������֤��ͤ� 0-indexed �Ǥ���
 */
int fits_header_record::get_section_info( long ret_begin[], long ret_length[],
				      bool ret_flip_flag[], int buf_len ) const
{
    int ret_len = -1;
    int i, n, ix;
    section_exp_info section_info;

    if ( parse_section_expression(this->svalue(), &section_info) < 0 ) {
	goto quit;	/* sytax error */
    }
    if ( 0 < section_info.sel_cols.length() ) {
	goto quit;	/* sytax error */
    }

    n = section_info.begin.length();

    ix = 0;
    for ( i=0 ; i < n ; i++ ) {
	if ( buf_len <= ix ) break;
	if ( ret_begin != NULL ) ret_begin[ix] = section_info.begin[i];
	if ( ret_length != NULL ) ret_length[ix] = section_info.length[i];
	if ( ret_flip_flag != NULL ) ret_flip_flag[ix] = section_info.flip_flag[i];
	ix++;
    }
    ret_len = n;

 quit:
    return ret_len;
}

/**
 * @brief  IRAF�����Υ���������������
 *
 *  IRAF �ǥ����Х�������ΰ�򼨤�������Ѥ����Ƥ��� '[3074:3104,1:512]' 
 *  �Τ褦��ʸ���󤫤�����Υ������������������ޤ�����ɽŪ�ʥ������
 *  �Ȥ��ơ�BIASSEC��TRIMSEC��DATASEC��CCDSEC��ORIGSEC ������ޤ���<br>
 *  �㤨�С��إå��쥳���� BIASSEC = '[3074:3104,1:512] ' �ξ�硤����
 *  dest_buf �ˤ� (3073,31,0,512) �Τ褦�ˡ�x�ΰ���(0-indexed)��Ĺ����
 *  y�ΰ���(0-indexed)��Ĺ���ν���ͤ��֤���ޤ���
 *
 * @param   dest_buf ���������ξ��� (return)
 * @param   buf_len dest_buf[] �ΥХåե���Ĺ�� (����μ�����)
 * @return  ʸ����˴ޤޤ�륻����������μ�����
 * @attention  ʸ����� 1-indexed ��ɽ������ޤ������֤��ͤ� 0-indexed �Ǥ���
 */
int fits_header_record::get_section_info( long dest_buf[], int buf_len ) const
{
    int ret_len = -1;
    int i, n, ix;
    section_exp_info section_info;

    if ( parse_section_expression(this->svalue(), &section_info) < 0 ) {
	goto quit;	/* sytax error */
    }
    if ( 0 < section_info.sel_cols.length() ) {
	goto quit;	/* sytax error */
    }

    n = section_info.begin.length();

    if ( dest_buf != NULL ) {
	ix = 0;
	for ( i=0 ; i < n ; i++ ) {
	    if ( buf_len <= ix ) break;
	    dest_buf[ix] = section_info.begin[i];
	    ix++;
	    if ( buf_len <= ix ) break;
	    dest_buf[ix] = section_info.length[i];
	    ix++;
	}
    }

    ret_len = 2 * n;

 quit:
    return ret_len;
}

/* discard original 80-char record, and reformat this record */
/**
 * @brief  80ʸ���쥳���ɤκƥե����ޥåȤ�Ԥʤ�
 *
 *  �����ݻ����Ƥ��� 80ʸ���Υե����ޥåȺѤ�ʸ����򡤸��ߤΥ�����ɡ��͡�
 *  �����Ȥ����Ƥ�����ʤ����ޤ���
 */
fits_header_record &fits_header_record::reformat()
{
    tstring *dst = (tstring *)(&this->a_formatted_rec);
    header_format_a_record( *this, dst );
    return *this;
}

/**
 * @brief  �ե����ޥåȺѤ�ʸ����(80��nʸ��)�����
 *
 *  ���֥�����������������줿���ե����ޥåȺѤ�ʸ����(80��nʸ��)���֤��ޤ���
 *  ����ʸ���ϴޤޤ줺��ʸ����� '\0' �ǽ�ü���Ƥ��ޤ���
 *  
 * @return  �ե����ޥåȺѤ�ʸ����(80��nʸ��)
 */
const char *fits_header_record::formatted_string() const
{
    /* 80-char �ΥХåե������ξ��ϥե����ޥåȤ�����Τ��� */
    if ( this->a_formatted_rec.cstr() == NULL ) {
	tstring *dst = (tstring *)(&this->a_formatted_rec);
	header_format_a_record( *this, dst );
    }
    return this->a_formatted_rec.cstr();
}

/**
 * @brief  �ե����ޥåȺѤ�ʸ����(80��nʸ��)��Ĺ�������
 *
 *  ���֥�����������������줿���ե����ޥåȺѤ�ʸ����(80��nʸ��)��Ĺ�����֤�
 *  �ޤ�������Ĺ���ˤϡ�ʸ����ν�üʸ�� '\0' �ϴޤߤޤ���
 *  
 * @return  �ե����ޥåȺѤ�ʸ����(80��nʸ��)��Ĺ��
 */
size_t fits_header_record::formatted_length() const
{
    return header_format_a_record( *this, NULL );
}

/**
 * @brief  ������ʸ��������
 */
const char *fits_header_record::comment() const
{
    if ( this->status() == FITS::DESCRIPTION_RECORD )
	return this->rec.value;
    else
	return this->rec.comment;
}

/**
 * @brief  �ץ���ޤΥХåե��˥�����ʸ��������
 */
ssize_t fits_header_record::get_comment(char *dest_buf, size_t buf_size) const
{
    size_t str_len = 0;
    const char *val = this->comment();

    if ( 0 < buf_size && dest_buf == NULL ) return -1;

    if ( val != NULL ) {
	str_len = this->arr_rec.at_cs(COMMENT_IDX).getstr(dest_buf,buf_size);
    }
    else {
	if ( 0 < buf_size ) dest_buf[0] = '\0';
    }

    return str_len;
}

/**
 * @brief  ������ʸ�����ͤ��ݻ����Ƥ����������֥������Ȥλ��Ȥ����(�ɼ�����)
 */
const tstring &fits_header_record::comment_cs() const
{
    return this->arr_rec.at_cs(COMMENT_IDX);
}

/**
 * @brief  ������ʸ�����Ĺ�������
 */
size_t fits_header_record::comment_length() const
{
    return this->arr_rec.at_cs(COMMENT_IDX).length();
}

/**
 * @brief  ��������ݸ��̵ͭ�����
 */
bool fits_header_record::keyword_protected() const
{
    return this->keyword_protected_rec;
}

/**
 * @brief  ���ݸ��̵ͭ�����
 */
bool fits_header_record::value_protected() const
{
    return this->value_protected_rec;
}

/**
 * @brief  �ͷ����ݸ��̵ͭ�����
 */
bool fits_header_record::value_type_protected() const
{
    return this->value_type_protected_rec;
}

/**
 * @brief  ���������ݸ��̵ͭ�����
 */
bool fits_header_record::comment_protected() const
{
    return this->comment_protected_rec;
}

/**
 * @brief  �إå��쥳���ɤ��ݻ����Ƥ���������¤�Τλ��Ȥ��֤� (�ɼ�����)
 */
const fits::header_def &fits_header_record::raw_record() const
{
    return this->rec;
}

/**
 * @brief  shallow copy ����Ĥ�����˻��� (̤����)
 * @note   ������֥������Ȥ� return ��ľ���ǻ��Ѥ��롥
 */
/* ���: ���Ȥ��֤������Ȥ��������return ʸ�Ǥ������ȥ��󥹥��� */
/*       ��2�󥳥ԡ������Ȥ����������ʻ��ˤʤ�Τ� void �ˤʤäƤ��� */
void fits_header_record::set_scopy_flag()
{
    return;
}


#include "private/header_format_a_record.cc"

}	/* namespace sli */

