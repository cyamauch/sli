/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2015-11-18 23:35:43 cyamauch> */

/**
 * @file   fits_header_record.cc
 * @brief  FITSヘッダの1つのレコードを表現するクラスfits_header_recordのコード
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
 * @brief  コメント辞書の定義 (fallback)
 *
 * @note   各 HDU タイプ用の辞書に無い場合に使われる．<br>
 *         追加すべきキーワードがあればお知らせください．
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
	/* 入れない方がよさげだ… 何の位置に対して使われるのかわからんから */
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
 * @brief  コメント辞書の定義 (Image HDU)
 *
 * @note   Image HDU の場合に使われる．<br>
 *         追加すべきキーワードがあればお知らせください．
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
 * @brief  コメント辞書の定義 (Binary Table HDU)
 *
 * @note   Binary Table HDU の場合に使われる．<br>
 *         追加すべきキーワードがあればお知らせください．
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
 * @brief  コメント辞書の定義 (ASCII Table HDU)
 *
 * @note   ASCII Table HDU の場合に使われる．<br>
 *         追加すべきキーワードがあればお知らせください．
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
 * @brief  コメント辞書の更新
 *
 *  プログラマがコメント辞書を更新する事ができます．
 *
 * @param   hdutype HDUの種別
 * @param   new_kwd_comments キーワードとコメントのセットが記述された
 *                           文字列配列．NULL で終端すること．
 * @return  書き込まれた asarray_tstring オグジェクトの参照 (読取専用)
 * @note    グローバルな静的オブジェクトに書き込まれます．
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
 * @brief  HDUタイプ,キーワード,値に応じてコメント辞書からコメント文字列を取得
 *
 * @note  private な関数です．
 */
static const char *get_default_comment( int hdutype, const char *keyword,
					const char *value,
					tstring *return_comment_ptr )
{
    const char *return_value = NULL;
    tstring kwd_num[8];		/* PC?_? などの ? の部分を保存する */
    size_t n_kwd_num = 0;	/* kwd_num に登録された個数 */
    tstring kwd_with_sh;	/* PC1_1 => PC#_# のように変換したものを保存 */
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
	    if ( 0 < n_spn && n_kwd_num < 8 ) {		/* 数字部分を拾う */
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
	    /* 普通の場合: これが最優先 */
	    return_comment = Image_comments.cstr(keyword);
	}
	/* WCS のキーワードはけっこうめんどくさい */
	if ( 0 < kwd_with_sh.length() ) {
	    if ( return_comment.cstr() == NULL ) {
		/* PC#_# のようなものを試す */
		return_comment = Image_comments.cstr(kwd_with_sh.cstr());
	    }
	    if ( return_comment.cstr() == NULL ) {
		/* PC1_1  => PC#_#? のようなものを試す */
		kwd_with_x.assign(kwd_with_sh).append('?',1);
		return_comment = Image_comments.cstr(kwd_with_x.cstr());
	    }	    
	    if ( return_comment.cstr() == NULL ) {
		/* PC1_1A => PC#_#? のようなものを試す */
		kwd_with_x.assign(kwd_with_sh);
		if ( 'A' <= kwd_with_x.cchr(kwd_with_x.length()-1) &&
		     kwd_with_x.cchr(kwd_with_x.length()-1) <= 'Z' ) {
		    kwd_with_x.replace(kwd_with_x.length()-1,1,"?");
		    return_comment = Image_comments.cstr(kwd_with_x.cstr());
		}
	    }
	}
	if ( return_comment.cstr() == NULL ) {
	    /* LONPOLE  => LONPOLE? のようなものを試す */
	    kwd_with_x.assign(keyword).append('?',1);
	    return_comment = Image_comments.cstr(kwd_with_x.cstr());
	}	    
	if ( return_comment.cstr() == NULL ) {
	    /* LONPOLEA => LONPOLE? のようなものを試す */
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

    /* コメント内の # にも対応する */
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
 * @brief  コンストラクタ
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
 * @brief  コピーコンストラクタ
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
 * @brief  デストラクタ
 */
fits_header_record::~fits_header_record()
{
    return;
}

/*
 * Special member functions
 */

/* fits_header class は this->manager をセットした状態でコレを使わない事 */
/**
 * @brief  オブジェクトのコピー
 */
fits_header_record &fits_header_record::operator=(const fits_header_record &obj)
{
    this->init(obj);
    return *this;
}

/* fits_header class は this->manager をセットした状態でコレを使わない事 */
/**
 * @brief  オブジェクトの初期化
 */
fits_header_record &fits_header_record::init()
{
    if ( this->manager != NULL ) {
	long i;
	fits::header_def tmp_def = {"","",""};	/* 消去を試みる */
	for ( i=0 ; i < this->manager->length() ; i++ ) {
	    if ( &(this->manager->record(i)) == this ) {
		/* index更新のため，間接的に this->assign_any() を呼ぶ */
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
 * @brief  低レベルな初期化
 *
 * @note   このメンバ関数は protected です．
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

/* fits_header class は this->manager をセットした状態でコレを使わない事 */
/**
 * @brief  オブジェクトのコピー
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
		    /* index更新のため，間接的に this->assign_any() を呼ぶ */
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
 * @brief  低レベルなオブジェクトのコピー
 *
 * @note   このメンバ関数は protected です．
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

/* fits_header class は this->manager をセットした状態でコレを使わない事 */
/**
 * @brief  オブジェクトの内容の更新 (fits::header_def 構造体で指定)
 */
fits_header_record &fits_header_record::assign( const fits::header_def &def )
{
    if ( this->manager != NULL ) {
	long i;
	for ( i=0 ; i < this->manager->length() ; i++ ) {
	    if ( &(this->manager->record(i)) == this ) {
		/* index更新のため，間接的に this->assign_any() を呼ぶ */
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

/* fits_header class は this->manager をセットした状態でコレを使わない事 */
/**
 * @brief  オブジェクトの内容の更新 (fits_header_record オブジェクトで指定)
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

/* fits_header class は this->manager をセットした状態でコレを使わない事 */
/**
 * @brief  オブジェクトの内容の更新 (keyword, value, commentで指定)
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

/* fits_header class は this->manager をセットした状態でコレを使わない事 */
/**
 * @brief  オブジェクトの内容の更新 (keyword, descriptionで指定)
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
 * @brief  ヘッダレコードの保護に関する設定
 *
 * @note   ポインタを張った時，消えないように保護するために必要．<br>
 *         このメンバ関数は protected です．
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
 * @brief  内部の tarray_tstring オブジェクトへの参照を返す
 *
 * @note   このメンバ関数は protected です．
 */
tarray_tstring &fits_header_record::arr()
{
    return this->arr_rec;
}

/**
 * @brief  内部の tarray_tstring オブジェクトへの参照を返す (読取専用)
 *
 * @note   このメンバ関数は protected です．
 */
const tarray_tstring &fits_header_record::arr_cs() const
{
    return this->arr_rec;
}

/**
 * @brief  自身を管理する fits_header オブジェクトを登録
 *
 * @note   このメンバ関数は protected です．
 */
fits_header_record &fits_header_record::register_manager( fits_header *ptr )
{
    this->manager = ptr;
    return *this;
}

/* **** internal functions **** */

/**
 * @brief  値 value から型を判定し，各型の値を返す (内部用)
 *
 * @param   value 生の文字列値
 * @param   ret_bool_value bool型の値 (return)
 * @param   ret_longlong_value long long型の値 (return)
 * @param   ret_double_value double型の値 (return)
 * @param   ret_str_begin 文字列の開始位置 (return)
 * @param   ret_str_len 文字列の長さ (return)
 * @param   remove_str_quote 文字列中のシングルクォートを除去するなら true．
 *          ただし，文字列中の「''」はそのままである．
 * @return  型種別
 * @note    private な関数です．
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

    /* value の左右の空白を除去した部分の情報を取得 */
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

    /* 複素数かどうかをチェック */
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
 * @brief  値 value から型を判定し，各型の値を返す (内部用)
 *
 * @return  型種別
 * @note    private な関数です．
 */
static int get_type( const char *value )
{
    return get_type_and_values(value,NULL,NULL,NULL,NULL,NULL,false);
}

/* protected */

/**
 * @brief  キーワードを設定 (printf()の記法)
 *
 * @note   このメンバ関数は protected です．
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
 * @brief  キーワードを設定 (printf()の記法)
 *
 * @note   このメンバ関数は protected です．
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
 * @brief  コメント文字列を設定 (printf()の記法)
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
 * @brief  コメント文字列を設定 (printf()の記法)
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
 * @brief  コメント文字列を設定
 */
fits_header_record &fits_header_record::assign_comment( const char *str )
{
    fits::header_def rv = {NULL,NULL,NULL};
    rv.comment = str;
    this->assign_any(rv);
    return *this;
}

/**
 * @brief  コメント辞書に存在するコメント文字列を自身のレコードに設定
 *
 *  自身のキーワードがコメント辞書に存在する場合，コメント辞書にあるコメント
 *  文字列を自身のレコードに書き込みます．
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
 * @brief  内容の更新 (低レベル)
 *
 * @note   このメンバ関数は protected です．
 */
fits_header_record &fits_header_record::assign_any( const fits::header_def &arg_def )
{
    tstring new_keyword;
    tstring new_value;
    tstring new_comment;
    fits::header_def def = {NULL,NULL,NULL};
    bool be_description = false;
    bool is_updated = false;		/* this->arr_rec が更新されたらtrue */

    def.keyword = arg_def.keyword;
    def.value = arg_def.value;
    def.comment = arg_def.comment;

    /* キーワードに空白が与えられ，かつ値かコメントに空白が与えられた場合は */
    /* レコードの初期化を行なう */
    if ( this->keyword_protected_rec == false ) {
	if ( (def.keyword != NULL && def.keyword[0] == '\0') &&
	     ( (def.value != NULL && def.value[0] == '\0') ||
	       (def.comment != NULL && def.comment[0] == '\0') ) ) {
	    this->_init();
	    return *this;
	}
    }

    /* 新規 or コメント行 */
    if ( this->arr_rec.cstr(COMMENT_IDX) == NULL ) {
	/* comment に description を書いても良い事にする */
	if ( arg_def.value == NULL ) {
	    def.value = arg_def.comment;
	    def.comment = arg_def.value;
	}
	/* コメント行 */
	if ( this->arr_rec.cstr(KEYWORD_IDX) != NULL ) {
	    /* description の場合は description のまま */
	    def.comment = NULL;
	    be_description = true;
	}
	else {
	    /* 新規の場合 */
	    /* 両方 NULL なら何もしない */
	    if ( def.keyword == NULL && def.value == NULL ) {
		return *this;
	    }
	    if ( def.keyword == NULL ) def.keyword = "";
	    if ( def.value == NULL ) def.value = "";
	    if ( def.comment == NULL ) be_description = true;
	}
    }

    /* 管理領域を指定されても良いようにする */
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
	/* "HIERARCH " で始まる場合は空白も許可する */
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
		/* description 形式の場合は，そのまんまの文字列が入る */
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
		/* ここでは常に false */
		const bool rm_str_quote = false;
		
		if ( this->value_type_protected_rec == true ) {
		    current_value_type = get_type(this->rec.value);
		}

		/* str_begin,str_lenには左右の空白を除去した部分の情報がくる */
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
			    /* 必要ならシングルクォートを追加する */
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
					/* 途中のシングルクォート */
					str_val[ii] = '\'';  ii ++;
					if ( str_begin[i+1] == '\'' ) i ++;
				    }
				    else {
					/* シングルクォートで終わっている */
					break;
				    }
				}
				str_val[ii] = str_begin[i];
				ii ++;
			    }
			    /* シングルクォートで始まっていない場合 */
			    if ( quot_begin_ok == false ) {
				while ( ii < 1 + FITS::HEADER_STRING_VALUE_ALIGNED_LENGTH ) {
				    str_val[ii] = ' ';
				    ii ++;
				}
			    }
			    /* 最後のシングルクォート */
			    str_val[ii] = '\'';
			    ii ++;
			    
			    fmt.printf("%%.%llds",(long long)ii);
			    this->arr_rec.putf(VALUE_IDX,
					       1,fmt.cstr(),str_val.cstr());
			}
		    }
		    else {
			/* str_begin, str_len の該当部分だけ保存 */
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

    /* 更新された場合，80-char を保存しているバッファを初期化する */
    if ( is_updated == true ) {
	this->a_formatted_rec = NULL;
    }

    return *this;
}

#if 1		/* Improved code by K. Matsuzaki (ISAS/JAXA). 2015.06.07 */

/**
 * @brief  double型値，文字列(高レベル)等 全ての内部キャッシュを更新
 *
 * @note   このメンバ関数は protected です．
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
	/* 「''」→「'」の変換 */
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
 * @brief  文字列(高レベル)の内部キャッシュを更新
 *
 * @note   このメンバ関数は protected です．
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
	/* 「''」→「'」の変換 */
	while ( 0 <= (pos = this->svalue_rec.find(pos,"''")) ) {
	    this->svalue_rec.replace(pos,2,"'");
	    pos++;
	}
    }
    else this->svalue_rec.assign("");

    return *this;
}

/**
 * @brief  bool型値の内部キャッシュを更新
 *
 * @note   このメンバ関数は protected です．
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
 * @brief  double型値の内部キャッシュを更新
 *
 * @note   このメンバ関数は protected です．
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
 * @brief  long long型値の内部キャッシュを更新
 *
 * @note   このメンバ関数は protected です．
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
 * @brief  値を設定 (低レベル・printf()の記法)
 *
 * @note  文字列値の場合，両端のシングルクォートを含んだ文字列を設定します．
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
 * @brief  値を設定 (低レベル・printf()の記法)
 *
 * @note  文字列値の場合，両端のシングルクォートを含んだ文字列を設定します．
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
 * @brief  値を設定 (低レベル)
 *
 * @note  文字列値の場合，両端のシングルクォートを含んだ文字列を設定します．
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
 * @brief  文字列値を設定 (printf()の記法)
 *
 * @note  両端のシングルクォートと空白を付加する必要はありません．
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
 * @brief  文字列値を設定 (printf()の記法)
 *
 * @note  両端のシングルクォートと空白を付加する必要はありません．
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
 * @brief  文字列値を設定
 *
 * @note  両端のシングルクォートと空白を付加する必要はありません．
 */
fits_header_record &fits_header_record::assign( const char *str )
{
    fits::header_def rv = {NULL,NULL,NULL};
    if ( str == NULL ) {
	this->assign_any(rv);
    }
    else {
	tstring tstr, tstr0;
	/* ' → '' の置き換えを行なう */
	tstr0.assign(str).strreplace("'","''",true);
	if ( tstr0.length() < FITS::HEADER_STRING_VALUE_ALIGNED_LENGTH ) {
	    /* 空白を追加 */
	    tstr0.resize(FITS::HEADER_STRING_VALUE_ALIGNED_LENGTH);
	}
	tstr.assignf("'%s'",tstr0.cstr());
	rv.value = tstr.cstr();
	this->assign_any(rv);
    }
    return *this;
}

/**
 * @brief  bool型で値を設定
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
 * @brief  int型で値を設定
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
 * @brief  long型で値を設定
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
 * @brief  long long 型で値を設定
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
 * @brief  double型で値を設定
 *
 * @param   val 値
 * @param   prec 精度(桁数)．省略時は15．
 * @return  自身の参照
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
    /* %.xxg の場合，. が書かれない事がある */
    if ( ep < 0 && sval.strchr('.') < 0 ) sval.append(".");
    /* E -> D にする */
    if ( 0 <= ep ) sval.at(ep) = 'D';

    rv.value = sval.cstr();
    this->assign_any(rv);
    return *this;
}

/**
 * @brief  float型で値を設定
 *
 * @param   val 値
 * @param   prec 精度(桁数)．省略時は6．
 * @return  自身の参照
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
    /* %.xxg の場合，. が書かれない事がある */
    if ( ep < 0 && sval.strchr('.') < 0 ) sval.append(".");

    rv.value = sval.cstr();
    this->assign_any(rv);
    return *this;
}

/* set date string [yyyy-mm-ddThh:mm:ss] */
/**
 * @brief  現在の UTC 時刻を「yyyy-mm-ddThh:mm:ss」の形式で設定
 *
 * @return  自身の参照
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
 * @brief  レコードの形式を取得
 *
 * @return  FITS::NORMAL_RECORD: キーワード,値,コメントの組み合わせ <br>
 *          FITS::DESCRIPTION_RECORD: キーワード,コメントの組み合わせ <br>
 *          FITS::NULL_RECORD: キーワードなし <br>
 */
int fits_header_record::status() const
{
    if ( this->arr_rec.length() == 3 ) return FITS::NORMAL_RECORD;
    if ( this->arr_rec.length() == 2 ) return FITS::DESCRIPTION_RECORD;
    else return FITS::NULL_RECORD;
}

/**
 * @brief  値の型を取得
 *
 * @return  FITS::STRING_T: 文字列値 <br>
 *          FITS::LONGLONG_T: 整数値 <br>
 *          FITS::DOUBLE_T: 実数値 <br>
 *          FITS::LOGICAL_T: 論理値 <br>
 *          FITS::DOUBLECOMPLEX_T: 複素数値 <br>
 *          FITS::ANY_T: 不明．判定不能
 * @attention  FITS::BYTE_T, FITS::SHORT_T, FITS::LONG_T, FITS::FLOAT_T, 
 *             FITS::COMPLEX_T を返す事はありません．
 */
int fits_header_record::type() const
{
    return get_type(this->rec.value);
}

/**
 * @brief  キーワード文字列を取得
 */
const char *fits_header_record::keyword() const
{
    return this->rec.keyword;
}

/**
 * @brief  プログラマのバッファにキーワード文字列を取得
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
 * @brief  キーワードを保持している内部オブジェクトの参照を取得 (読取専用)
 */
const tstring &fits_header_record::keyword_cs() const
{
    return this->arr_rec.at_cs(KEYWORD_IDX);
}

/**
 * @brief  キーワード文字列の長さを取得
 */
size_t fits_header_record::keyword_length() const
{
    return this->arr_rec.at_cs(KEYWORD_IDX).length();
}

/**
 * @brief  値文字列を取得 (低レベル)
 *
 * @note  文字列値の場合，両端のシングルクォートを含んだ文字列を返します．
 */
const char *fits_header_record::value() const
{
    return this->rec.value;
}

/**
 * @brief  プログラマのバッファに値文字列を取得 (低レベル)
 *
 * @note  文字列値の場合，両端のシングルクォートを含んだ文字列を返します．
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
 * @brief  値を保持している内部オブジェクトの参照を取得 (低レベル・読取専用)
 *
 * @note  文字列値の場合，両端のシングルクォートを含みます．
 */
const tstring &fits_header_record::value_cs() const
{
    return this->arr_rec.at_cs(VALUE_IDX);
}

/**
 * @brief  値文字列の長さを取得 (低レベル)
 *
 * @note  文字列値の場合，両端のシングルクォートを含みます．
 */
size_t fits_header_record::value_length() const
{
    return this->arr_rec.at_cs(VALUE_IDX).length();
}

/**
 * @brief  文字列値を取得
 *
 * @note  文字列値の場合，両端のシングルクォートと空白は除去されたものを
 *        返します．
 */
const char *fits_header_record::svalue() const
{
    return this->svalue_rec.cstr();
}

/**
 * @brief  文字列値を保持している内部オブジェクトの参照を取得 (読取専用)
 *
 * @note  文字列値の場合，両端のシングルクォートと空白は除去されています．
 */
const tstring &fits_header_record::svalue_cs() const
{
    return this->svalue_rec;
}

/**
 * @brief  文字列値の長さを取得
 *
 * @note  文字列値の場合，両端のシングルクォートと空白は除去した長さです．
 */
size_t fits_header_record::svalue_length() const
{
    return this->svalue_rec.length();
}

/**
 * @brief  プログラマのバッファに文字列値を取得
 *
 * @note  文字列値の場合，両端のシングルクォートと空白は除去されたものを
 *        返します．
 */
ssize_t fits_header_record::get_svalue( char *dest_buf, 
					size_t buf_size ) const
{
    return this->svalue_rec.getstr(dest_buf,buf_size);
}

/**
 * @brief  値を bool 型で取得
 */
bool fits_header_record::bvalue() const
{
    return this->bvalue_rec;
}

/**
 * @brief  値を long 型で取得
 *
 * @attention  fits_header_record::type() では 32-bit整数か 64-bit整数かを
 *             判定できません．
 *             32-bit OS を考慮する場合は，fits_header_record::llvalue() を
 *             お使いください．
 */
long fits_header_record::lvalue() const
{
    return this->llvalue_rec;
}

/**
 * @brief  値を long long 型で取得
 */
long long fits_header_record::llvalue() const
{
    return this->llvalue_rec;
}

/**
 * @brief  値を double 型で取得
 */
double fits_header_record::dvalue() const
{
    return this->dvalue_rec;
}

#include "private/parse_section_expression.cc"

/* get IRAF-style section info (e.g. BIASSEC, TRIMSEC, etc.) */
/**
 * @brief  IRAF形式のセクション情報を取得
 *
 *  IRAF でオーバスキャン領域を示すために用いられている '[3074:3104,1:512]' 
 *  のような文字列から画像のセクション情報を取得します．代表的なキーワード
 *  として，BIASSEC，TRIMSEC，DATASEC，CCDSEC，ORIGSEC があります．
 *
 * @param   ret_begin セクションの開始位置 (return)
 * @param   ret_length セクションの長さ (return)
 * @param   ret_flip_flag 反転フラグ (return)
 * @param   buf_len ret_begin[] 等のバッファの長さ (最大の次元数)
 * @return  文字列に含まれるセクション情報の次元数
 * @attention  文字列は 1-indexed で表現されますが，返り値は 0-indexed です．
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
 * @brief  IRAF形式のセクション情報を取得
 *
 *  IRAF でオーバスキャン領域を示すために用いられている '[3074:3104,1:512]' 
 *  のような文字列から画像のセクション情報を取得します．代表的なキーワード
 *  として，BIASSEC，TRIMSEC，DATASEC，CCDSEC，ORIGSEC があります．<br>
 *  例えば，ヘッダレコード BIASSEC = '[3074:3104,1:512] ' の場合，配列
 *  dest_buf には (3073,31,0,512) のように，xの位置(0-indexed)，長さ，
 *  yの位置(0-indexed)，長さの順に値が返されます．
 *
 * @param   dest_buf セクションの情報 (return)
 * @param   buf_len dest_buf[] のバッファの長さ (最大の次元数)
 * @return  文字列に含まれるセクション情報の次元数
 * @attention  文字列は 1-indexed で表現されますが，返り値は 0-indexed です．
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
 * @brief  80文字レコードの再フォーマットを行なう
 *
 *  現在保持している 80文字のフォーマット済み文字列を，現在のキーワード，値，
 *  コメントの内容から作りなおします．
 */
fits_header_record &fits_header_record::reformat()
{
    tstring *dst = (tstring *)(&this->a_formatted_rec);
    header_format_a_record( *this, dst );
    return *this;
}

/**
 * @brief  フォーマット済み文字列(80×n文字)を取得
 *
 *  オブジェクト内で生成された，フォーマット済み文字列(80×n文字)を返します．
 *  改行文字は含まれず，文字列は '\0' で終端しています．
 *  
 * @return  フォーマット済み文字列(80×n文字)
 */
const char *fits_header_record::formatted_string() const
{
    /* 80-char のバッファが空の場合はフォーマットしたものを作る */
    if ( this->a_formatted_rec.cstr() == NULL ) {
	tstring *dst = (tstring *)(&this->a_formatted_rec);
	header_format_a_record( *this, dst );
    }
    return this->a_formatted_rec.cstr();
}

/**
 * @brief  フォーマット済み文字列(80×n文字)の長さを取得
 *
 *  オブジェクト内で生成された，フォーマット済み文字列(80×n文字)の長さを返し
 *  ます．この長さには，文字列の終端文字 '\0' は含みません．
 *  
 * @return  フォーマット済み文字列(80×n文字)の長さ
 */
size_t fits_header_record::formatted_length() const
{
    return header_format_a_record( *this, NULL );
}

/**
 * @brief  コメント文字列を取得
 */
const char *fits_header_record::comment() const
{
    if ( this->status() == FITS::DESCRIPTION_RECORD )
	return this->rec.value;
    else
	return this->rec.comment;
}

/**
 * @brief  プログラマのバッファにコメント文字列を取得
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
 * @brief  コメント文字列値を保持している内部オブジェクトの参照を取得(読取専用)
 */
const tstring &fits_header_record::comment_cs() const
{
    return this->arr_rec.at_cs(COMMENT_IDX);
}

/**
 * @brief  コメント文字列の長さを取得
 */
size_t fits_header_record::comment_length() const
{
    return this->arr_rec.at_cs(COMMENT_IDX).length();
}

/**
 * @brief  キーワード保護の有無を取得
 */
bool fits_header_record::keyword_protected() const
{
    return this->keyword_protected_rec;
}

/**
 * @brief  値保護の有無を取得
 */
bool fits_header_record::value_protected() const
{
    return this->value_protected_rec;
}

/**
 * @brief  値型の保護の有無を取得
 */
bool fits_header_record::value_type_protected() const
{
    return this->value_type_protected_rec;
}

/**
 * @brief  コメント値保護の有無を取得
 */
bool fits_header_record::comment_protected() const
{
    return this->comment_protected_rec;
}

/**
 * @brief  ヘッダレコードを保持している内部構造体の参照を返す (読取専用)
 */
const fits::header_def &fits_header_record::raw_record() const
{
    return this->rec;
}

/**
 * @brief  shallow copy を許可する場合に使用 (未実装)
 * @note   一時オブジェクトの return の直前で使用する．
 */
/* 注意: 参照を返したいところだが，return 文でそれをやるとインスタンス */
/*       を2回コピーされるというおかしな事になるので void になっている */
void fits_header_record::set_scopy_flag()
{
    return;
}


#include "private/header_format_a_record.cc"

}	/* namespace sli */

