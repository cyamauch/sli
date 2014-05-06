/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2013-04-29 12:03:06 cyamauch> */

#ifndef _SLI__FITS_H
#define _SLI__FITS_H 1

/**
 * @file   fits.h
 * @brief  SFITSIO の API で用いる各種定数の定義
 */

/*
 * Basic constant and type to handle FITS files
 */

#include <stddef.h>
#include <stdint.h>

#ifdef BUILD_SFITSIO
#include <sli/numeric_indefs.h>
#else
#include "numeric_indefs.h"
#endif


namespace sli
{

  namespace FITS
  {
    /* */
    const char *const PRIMARY_EXTNAME = "Primary";
    const char *const DEFAULT_EXTNAME = "unknown";
    const int DEFAULT_BITPIX = 16;
    /* */
    const size_t FILE_RECORD_UNIT = 2880;
    const size_t HEADER_RECORD_UNIT = 80;

    const size_t HEADER_KEYWORD_ALIGNED_LENGTH = 8;
    const size_t HEADER_STRING_VALUE_ALIGNED_LENGTH = 8;

/*           20        30        40        50        60        70          */
/* 1234 ... 90123456789012345678901234567890123456789012345678901234567890 */
/* LOOO ... OOOOOOOOOOOOOOOOOOOOOOOOOONG_KEYWORD=                   16 / & */
/*                            64-bit integer  =>  -9223372036854775807     */

    const size_t HEADER_KEYWORD_MAX_LENGTH = 54;

/*          10        20        30 */
/* 1234567890123456789012345678901234 */
/* SIMPLE  =                    T / file does conform to FITS standard */

    const size_t HEADER_KEYWORD_AND_VALUE_ALIGNED_LENGTH = 30;

    //const size_t Header_value_max_length = 70;
    //const size_t Header_value_nominal_max_length = 20;
    //const size_t Header_comment_max_length = 72;

    /* ここは無制限にする */
    //const long Table_max_num_of_columns = 999;
    //const long Table_max_num_of_named_elements = 99;

    /*
     * Code for HDU Type
     */
    const int ANY_HDU = 127;		/* ANY_HDU */
    const int IMAGE_HDU = 0;		/* IMAGE_HDU */
    const int BINARY_TABLE_HDU = 2;	/* BINARY_TBL */
    const int ASCII_TABLE_HDU = 1;	/* ASCII_TBL */

    const int NUM_HDU_TYPES = 3;

    const long ALL = (long)((unsigned long)(-1L) >> 1);
    const long INDEF = (long)1 << (8*sizeof(long)-1);

    /*
     * Code for Header Record
     */
    const int NULL_RECORD = 0;
    const int NORMAL_RECORD = 1;
    const int DESCRIPTION_RECORD = 2;

    /*
     * Type code for header records and all type of data units
     */
    /* supported */
    const int ANY_T = 127;
    const int BIT_T = 88;		/* 'X' */
    const int BYTE_T = 66;		/* 'B' */
    const int LOGICAL_T = 76;		/* 'L' */
    const int BOOL_T = 76;		/* 'L' */
    const int ASCII_T = 65;		/* 'A' */
    const int STRING_T = 65;		/* 'A' */
    const int SHORT_T = 73;		/* 'I' */
    const int LONG_T = 74;		/* 'J' */
    const int LONGLONG_T = 75;		/* 'K' */
    const int FLOAT_T = 69;		/* 'E' */
    const int DOUBLE_T = 68;		/* 'D' */
    const int COMPLEX_T = 67;		/* 'C' */
    const int DOUBLECOMPLEX_T = 77;	/* 'M' */
    const int LONGARRDESC_T = 80;	/* 'P' */
    const int LLONGARRDESC_T = 81;	/* 'Q' */
    /* reserved */
    const int SBYTE_T = 83;		/* 'S' */
    const int USHORT_T = 85;		/* 'U' */
    const int ULONG_T = 86;		/* 'V' */
    const int ULONGLONG_T = 87;		/* 'W' */
  }


  /**
   * @brief  SFITSIO の API で用いる構造体です．
   * @author Chisato YAMAUCHI
   * @date   2013-03-26 00:00:00
   */

  namespace fits
  {

    /*
     * Type definitions independent of machine environment
     */
    /* supported */
    typedef struct _bit_t {
	unsigned char elem7:1;	/* right */
	unsigned char elem6:1;
	unsigned char elem5:1;
	unsigned char elem4:1;
	unsigned char elem3:1;
	unsigned char elem2:1;
	unsigned char elem1:1;
	unsigned char elem0:1;	/* left */
    } bit_t;
    typedef uint8_t  byte_t;
    typedef uint8_t  logical_t;
    typedef char     ascii_t;
    typedef int16_t  short_t;
    typedef int32_t  long_t;
    typedef float    float_t;
    typedef int64_t  longlong_t;
    typedef double   double_t;
    typedef float _Complex   complex_t;
    typedef double _Complex  doublecomplex_t;
    typedef struct _longarrdesc_t {
	uint32_t length;	/* length of element (not byte) */
	uint32_t offset;	/* byte offset (beginning with 0) */
    } longarrdesc_t;
    typedef struct _llongarrdesc_t {
	uint64_t length;
	uint64_t offset; 
    } llongarrdesc_t;
    /* reserved */
    typedef int8_t   sbyte_t;
    typedef uint16_t ushort_t;
    typedef uint32_t ulong_t;
    typedef uint64_t ulonglong_t;

    /*
     * a header record
     */
    typedef struct _header_def {
	const char *keyword;
	const char *value;
	const char *comment;
    } header_def;

    /*
     * definition of a column
     */
    typedef struct _table_def_all {
	const char *ttype;
	const char *ttype_comment;
	const char *talas;
	const char *talas_comment;
	const char *telem;		/* trepeat != 1 の時の element name．
					   trepeat の数に満たない場合は NULL 
					   で終わっていること */
	const char *telem_comment;
	const char *tunit;
	const char *tunit_comment;
	const char *tdisp;
	const char *tdisp_comment;
	const char *tform;
	const char *tform_comment;
	const char *tdim;
	const char *tdim_comment;
	const char *tnull;
	const char *tnull_comment;
	const char *tzero;
	const char *tzero_comment;
	const char *tscal;
	const char *tscal_comment;
    } table_def_all;

    typedef struct _table_def {
	const char *ttype;
	const char *ttype_comment;
	const char *talas;
	const char *telem;		/* trepeat != 1 の時の element name．
					   trepeat の数に満たない場合は NULL 
					   で終わっていること */
	const char *tunit;
	const char *tunit_comment;
	const char *tdisp;
	const char *tform;
	const char *tdim;
	const char *tnull;
	const char *tzero;
	const char *tscal;
    } table_def;

  }

}	/* namespace sli */


#endif	/* _SLI__FITS_H */
