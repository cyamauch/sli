/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2013-06-05 11:25:43 cyamauch> */

#ifndef _SLI__TARRAY_TSTRING_H
#define _SLI__TARRAY_TSTRING_H 1

/**
 * @file   tarray_tstring.h
 * @brief  文字列配列を扱うtarray_tstringクラスの定義とinlineメンバ関数
 */

#include <stddef.h>
#include <stdarg.h>

#include "sli_config.h"
#include "tstring.h"
#include "tarray.h"

#include "slierr.h"

namespace sli
{

/*
 * sli::tarray_tstring class manages a string array in its object, and 
 * provides a lot of useful member functions to process the string array. 
 */

/**
 * @class  sli::tarray_tstring
 * @brief  文字列配列を扱うためのクラス
 *
 *   tarray_tstring は文字列配列を扱うためのクラスで，tstring クラスと同様，
 *   スクリプト言語のような強力な文字列処理用 API を提供します．下記のような
 *   機能を持ちます．<br>
 *    - 値のセット，追加，挿入等で printf() の記法が使える API <br>
 *    - 要素単位のアクセス <br>
 *    - 基本的な文字・文字列の検索 <br>
 *    - 左右の空白，改行文字の除去 <br>
 *    - 文字列のデリミタでの分割 <br>
 *    - シェル風の文字列マッチ <br>
 *    - 正規表現による検索，置換，後方参照情報の取得 <br>
 *    - 要素文字列に対するポインタ配列の取得 <br>
 *
 * @note  内部のバッファは 1Gbyte 未満では 2^n 単位で確保されるため，かなり
 *        頻繁な更新処理に対しても十分なパフォーマンスが得られるでしょう．
 * @author  Chisato YAMAUCHI
 * @date  2013-04-01 00:00:00
 */
 
  class tarray_tstring
  {

  public:
    /* constructor and destructor */
    tarray_tstring();
    tarray_tstring(const char *const *elements);
    tarray_tstring(const char *el0, const char *el1, ...);     /* 要NULL終端 */
    tarray_tstring(const tarray_tstring &obj);
    virtual ~tarray_tstring();

    /*
     * operator
     */
    /* same as .init(obj) */
    virtual tarray_tstring &operator=(const tarray_tstring &obj);

    /* same as .append(obj) */
    virtual tarray_tstring &operator+=(const tarray_tstring &obj);

    /* same as .assign(elemens) for elements != NULL, same as .init() for */
    /* elements == NULL, but returns address of internal buffer           */
    virtual const char *const *operator=(const char *const *elements);

    /* same as .append(elements), but returns address of internal buffer */
    virtual const char *const *operator+=(const char *const *elements);

    /* same as .append(str,1) */
    virtual tarray_tstring &operator+=(const char *str);

    /* same as .append(one,1) */
    virtual tarray_tstring &operator+=(const tstring &one);

    /* same as .at(index) */
    virtual tstring &operator[]( size_t index );
    virtual const tstring &operator[]( size_t index ) const;

    /*
     * member functions
     */
    /* complete initialization of objects */
    virtual tarray_tstring &init();
    virtual tarray_tstring &init(const tarray_tstring &obj);

    /* clean() member functions pad all the element values for an existing */
    /* array (length of array is not changed)                              */
    virtual tarray_tstring &clean(const char *str = "");
    virtual tarray_tstring &clean(const tstring &one);

    /* set a string array */
    virtual tarray_tstring &assign( const char *const *elements );
    virtual tarray_tstring &assign( const char *const *elements, size_t n );
    virtual tarray_tstring &assign(const tarray_tstring &src, size_t idx2 = 0);
    virtual tarray_tstring &assign( const tarray_tstring &src, 
				    size_t idx2, size_t n2 );
    virtual tarray_tstring &assign( const char *el0, const char *el1,
				    ... );	/* 要NULL終端 */
    virtual tarray_tstring &vassign( const char *el0, const char *el1, 
				     va_list ap );
    virtual tarray_tstring &assign( const char *str, size_t n );
    virtual tarray_tstring &assign( const tstring &one, size_t n );
    virtual tarray_tstring &assignf( size_t n, const char *format, ... );
    virtual tarray_tstring &vassignf( size_t n, const char *fmt, va_list ap );

    /* same as explode() of PHP:                                            */
    /* split string and save the elements into object.                      */
    /*   src_str: string to be divided                                      */
    /*   delim:   delimiter string (not character-set)                      */
    /*   zero_str:   whether or not to allow string elements with a length  */
    /*               of 0 after division (true for csv format)              */
    /*   quot_bkt:   string that includes quotation and blacket characters  */
    /*   escape:     escape character                                       */
    /*   rm_escape:  flag used to indicate whether or not to delete escape  */
    /*               characters (true/false)                                */
    virtual tarray_tstring &explode( const char *src_str, const char *delim,
				     bool zero_str );
    virtual tarray_tstring &explode(const tstring &src_str, const char *delim,
				    bool zero_str);
    /*
     * New member functions of 1.3.0
     */
    virtual tarray_tstring &explode( const char *src_str, const char *delim,
	    bool zero_str, const char *quot_bkt, int escape, bool rm_escape );
    virtual tarray_tstring &explode(const tstring &src_str, const char *delim,
	    bool zero_str, const char *quot_bkt, int escape, bool rm_escape );

    /* divides a string in an argument and assigns it to an array in object */
    /*   src_str:    string to be divided                                   */
    /*   delims:     string that includes a delimiter character string      */
    /*               (i.e., character-set)                                  */
    /*   zero_str:   whether or not to allow string elements with a length  */
    /*               of 0 after division (true for csv format)              */
    /*   quot_bkt:   string that includes quotation and blacket characters  */
    /*   escape:     escape character                                       */
    /*   rm_escape:  flag used to indicate whether or not to delete escape  */
    /*               characters (true/false)                                */
    virtual tarray_tstring &split( const char *src_str, const char *delims, 
				   bool zero_str );
    virtual tarray_tstring &split( const tstring &src_str, const char *delims, 
				   bool zero_str );
    virtual tarray_tstring &split( const char *src_str, const char *delims, 
				   bool zero_str, const char *quot_bkt,
				   int escape, bool rm_escape );
    virtual tarray_tstring &split( const tstring &src_str, const char *delims, 
				   bool zero_str, const char *quot_bkt,
				   int escape, bool rm_escape );

    /* performs regular expression matching on strings in an argument, and */
    /* assigns the result to an array in object.                           */
    virtual tarray_tstring &regassign( const char *src_str, const char *pat );
    virtual tarray_tstring &regassign( const char *src_str, size_t pos,
				       const char *pat );
    virtual tarray_tstring &regassign( const char *src_str, size_t pos,
				       const char *pat, size_t *nextpos );
    virtual tarray_tstring &regassign(const tstring &src_str, const char *pat);
    virtual tarray_tstring &regassign( const tstring &src_str, size_t pos,
				       const char *pat );
    virtual tarray_tstring &regassign( const tstring &src_str, size_t pos,
				       const char *pat, size_t *nextpos );
    virtual tarray_tstring &regassign(const char *src_str, const tstring &pat);
    virtual tarray_tstring &regassign( const char *src_str, size_t pos,
				       const tstring &pat );
    virtual tarray_tstring &regassign( const char *src_str, size_t pos,
				       const tstring &pat, size_t *nextpos );
    virtual tarray_tstring &regassign( const tstring &src_str, const tstring &pat);
    virtual tarray_tstring &regassign( const tstring &src_str, size_t pos,
				       const tstring &pat );
    virtual tarray_tstring &regassign( const tstring &src_str, size_t pos,
				       const tstring &pat, size_t *nextpos );
    virtual tarray_tstring &regassign(const char *src_str, const tregex &pat);
    virtual tarray_tstring &regassign( const char *src_str, size_t pos,
				       const tregex &pat );
    virtual tarray_tstring &regassign( const char *src_str, size_t pos,
				       const tregex &pat, size_t *nextpos );
    virtual tarray_tstring &regassign( const tstring &src_str, const tregex &pat );
    virtual tarray_tstring &regassign( const tstring &src_str, size_t pos,
				       const tregex &pat );
    virtual tarray_tstring &regassign( const tstring &src_str, size_t pos,
				       const tregex &pat, size_t *nextpos );

    /* append elements to array in object */
    virtual tarray_tstring &append( const char *const *elements );
    virtual tarray_tstring &append( const char *const *elements, size_t n );
    virtual tarray_tstring &append(const tarray_tstring &src, size_t idx2 = 0);
    virtual tarray_tstring &append( const tarray_tstring &src, 
				    size_t idx2, size_t n2 );
    virtual tarray_tstring &append( const char *el0, const char *el1, 
				    ... );	/* 要NULL終端 */
    virtual tarray_tstring &vappend( const char *el0, const char *el1,
				     va_list ap );
    virtual tarray_tstring &append( const char *str, size_t n );
    virtual tarray_tstring &append( const tstring &one, size_t n );
    virtual tarray_tstring &appendf( size_t n, const char *format, ... );
    virtual tarray_tstring &vappendf( size_t n, const char *fmt, va_list ap );

    /* insert elements into array in object */
    virtual tarray_tstring &insert(size_t index, const char *const *elements);
    virtual tarray_tstring &insert( size_t index, 
				    const char *const *elements, size_t n );
    virtual tarray_tstring &insert( size_t index, 
				  const tarray_tstring &src, size_t idx2 = 0 );
    virtual tarray_tstring &insert( size_t index, 
			   const tarray_tstring &src, size_t idx2, size_t n2 );
    virtual tarray_tstring &insert( size_t index, 
				    const char *el0, const char *el1, 
				    ... );	/* 要NULL終端 */
    virtual tarray_tstring &vinsert( size_t index, 
				     const char *el0, const char *el1, 
				     va_list ap );
    virtual tarray_tstring &insert( size_t index, const char *str, size_t n );
    virtual tarray_tstring &insert(size_t index, const tstring &one, size_t n);
    virtual tarray_tstring &insertf( size_t index, 
				     size_t n, const char *format, ... );
    virtual tarray_tstring &vinsertf( size_t index, 
				      size_t n, const char *fmt, va_list ap );

    /* replace a part of elements with specified ones */
    virtual tarray_tstring &replace( size_t idx1, size_t n1,
				     const char *const *elements );
    virtual tarray_tstring &replace( size_t idx1, size_t n1,
				     const char *const *elements, size_t n2 );
    virtual tarray_tstring &replace( size_t idx1, size_t n1,
				  const tarray_tstring &src, size_t idx2 = 0 );
    virtual tarray_tstring &replace( size_t idx1, size_t n1,
			   const tarray_tstring &src, size_t idx2, size_t n2 );
    virtual tarray_tstring &replace( size_t idx1, size_t n1, 
				     const char *el0, const char *el1, 
				     ... );	/* 要NULL終端 */
    virtual tarray_tstring &vreplace( size_t idx1, size_t n1, 
				      const char *el0, const char *el1, 
				      va_list ap );
    virtual tarray_tstring &replacef( size_t idx1, size_t n1, size_t n2,
				      const char *format, ... );
    virtual tarray_tstring &vreplacef( size_t idx1, size_t n1, size_t n2,
				       const char *fmt, va_list ap );
    virtual tarray_tstring &replace( size_t idx1, size_t n1, 
				     const char *str, size_t n2 );
    virtual tarray_tstring &replace( size_t idx1, size_t n1, 
				     const tstring &one, size_t n2 );

    /* overwrite elements */
    virtual tarray_tstring &put(size_t index, const char *const *elements);
    virtual tarray_tstring &put( size_t index, 
				 const char *const *elements, size_t n );
    virtual tarray_tstring &put( size_t index, 
				 const tarray_tstring &src, size_t idx2 = 0 );
    virtual tarray_tstring &put( size_t index, 
			   const tarray_tstring &src, size_t idx2, size_t n2 );
    virtual tarray_tstring &put( size_t index, 
				 const char *el0, const char *el1, 
				 ... );	/* 要NULL終端 */
    virtual tarray_tstring &vput( size_t index, 
				  const char *el0, const char *el1, 
				  va_list ap );
    virtual tarray_tstring &put( size_t index, const char *str, size_t n );
    virtual tarray_tstring &put( size_t index, const tstring &one, size_t n );
    virtual tarray_tstring &putf( size_t index, 
				  size_t n, const char *format, ... );
    virtual tarray_tstring &vputf( size_t index, 
				   size_t n, const char *fmt, va_list ap );

    /* cutout elements */
    virtual tarray_tstring &crop( size_t idx, size_t len );
    virtual tarray_tstring &crop( size_t idx );

    /* erase specified elements */
    virtual tarray_tstring &erase();
    virtual tarray_tstring &erase( size_t index, size_t num_elements = 1 );

    /* resize array */
    virtual tarray_tstring &resize( size_t new_num_elements );
    virtual tarray_tstring &resizeby( ssize_t len );	/* relatively */

    /* swap contents between this object and sobj */
    virtual tarray_tstring &swap( tarray_tstring &sobj );

    /* copy all or some elements to another object */
    virtual ssize_t copy( size_t index, size_t n, tarray_tstring *dest ) const;
    virtual ssize_t copy( size_t index, tarray_tstring *dest ) const;
    virtual ssize_t copy( tarray_tstring *dest ) const;
    /* not recommended */
    virtual ssize_t copy( size_t index, size_t n, tarray_tstring &dest ) const;
    /* not recommended */
    virtual ssize_t copy( size_t index, tarray_tstring &dest ) const;
    /* not recommended */
    virtual ssize_t copy( tarray_tstring &dest ) const;

    /* perform simple string replacement for all elements:  search org_str */
    /* and replace it with new_str.                                        */
    /* すべての文字列要素に対する文字列置換                                */
    virtual tarray_tstring &strreplace( const char *org_str,
					const char *new_str,
					bool all = false );
    virtual tarray_tstring &strreplace( const tstring &org_str, 
					const char *new_str, 
					bool all = false );
    virtual tarray_tstring &strreplace( const char *org_str,
					const tstring &new_str,
					bool all = false );
    virtual tarray_tstring &strreplace( const tstring &org_str, 
					const tstring &new_str, 
					bool all = false );

    /* elimination of newline characters in all the elements */
    /* perform tstring::chop() for all elements              */
    virtual tarray_tstring &chop();
    /* perform tstring::chomp() for all elements             */
    virtual tarray_tstring &chomp( const char *rs = "\n" );
    virtual tarray_tstring &chomp( const tstring &rs );

    /* elimination of spaces at both ends of all the elements */
    /* (perform tstring::trim() for all elements)             */
    virtual tarray_tstring &trim( const char *side_spaces = " \t\n\r\f\v" );
    virtual tarray_tstring &trim( const tstring &side_spaces );
    virtual tarray_tstring &trim( int side_space );

    /* elimination of a space at the left end of all the elements */
    /* (perform tstring::ltrim() for all elements)                */
    virtual tarray_tstring &ltrim( const char *side_spaces = " \t\n\r\f\v" );
    virtual tarray_tstring &ltrim( const tstring &side_spaces );
    virtual tarray_tstring &ltrim( int side_space );

    /* elimination of a space at the right end of all the elements */
    /* (perform tstring::rtrim() for all elements)                 */
    virtual tarray_tstring &rtrim( const char *side_spaces = " \t\n\r\f\v" );
    virtual tarray_tstring &rtrim( const tstring &side_spaces );
    virtual tarray_tstring &rtrim( int side_space );

    /* same as trim() */
    virtual tarray_tstring &strtrim( const char *side_spaces = " \t\n\r\f\v" );
    virtual tarray_tstring &strtrim( const tstring &side_spaces );
    virtual tarray_tstring &strtrim( int side_space );

    /* convert all elements to lowercase */
    virtual tarray_tstring &tolower();

    /* convert all elements to uppercase */
    virtual tarray_tstring &toupper();

    /* replacement of tab or space characters for all elements */
    /* perform tstring::expand_tabs() for all elements         */
    virtual tarray_tstring &expand_tabs( size_t tab_width = 8 );
    /* perform tstring::contract_spaces() for all elements     */
    virtual tarray_tstring &contract_spaces( size_t tab_width = 8 );

    /* string search and replacement of all the elements using a regular */
    /* expression (perform tstring::regreplace() for all elements)       */
    virtual tarray_tstring &regreplace( const char *pat, 
				       const char *new_str, bool all = false );
    virtual tarray_tstring &regreplace( const tstring &pat, 
				       const char *new_str, bool all = false );
    virtual tarray_tstring &regreplace( const tregex &pat, 
				       const char *new_str, bool all = false );
    virtual tarray_tstring &regreplace( const char *pat, 
				    const tstring &new_str, bool all = false );
    virtual tarray_tstring &regreplace( const tstring &pat, 
				    const tstring &new_str, bool all = false );
    virtual tarray_tstring &regreplace( const tregex &pat, 
				    const tstring &new_str, bool all = false );

    /* search from the left side (beginning) of an array element */
    /* str に一致する要素を返す                                  */
    virtual ssize_t find_elem( const char *str ) const;
    virtual ssize_t find_elem( size_t idx, const char *str ) const;
    virtual ssize_t find_elem( size_t idx, const char *str,
			       size_t *nextidx ) const;
    virtual ssize_t find_elem( const tstring &str ) const;
    virtual ssize_t find_elem( size_t idx, const tstring &str ) const;
    virtual ssize_t find_elem(size_t idx, const tstring &str,
			      size_t *nextidx) const;

    /* search from the right side (end) of an array element */
    virtual ssize_t rfind_elem( const char *str ) const;
    virtual ssize_t rfind_elem( size_t idx, const char *str ) const;
    virtual ssize_t rfind_elem( size_t idx, const char *str,
				size_t *nextidx ) const;
    virtual ssize_t rfind_elem( const tstring &str ) const;
    virtual ssize_t rfind_elem( size_t idx, const tstring &str ) const;
    virtual ssize_t rfind_elem(size_t idx, const tstring &str,
			       size_t *nextidx) const;

    /* search an array from the left side (beginning) for a string */
    virtual ssize_t find( const char *str, ssize_t *pos_r ) const;
    virtual ssize_t find( size_t idx, size_t pos, const char *str,
			  ssize_t *pos_r ) const;
    virtual ssize_t find( size_t idx, size_t pos, const char *str,
			  ssize_t *pos_r,
			  size_t *nextidx, size_t *nextpos) const;
    virtual ssize_t find( const tstring &str, ssize_t *pos_r ) const;
    virtual ssize_t find( size_t idx, size_t pos, const tstring &str,
			  ssize_t *pos_r) const;
    virtual ssize_t find( size_t idx, size_t pos, const tstring &str,
			  ssize_t *pos_r, 
			  size_t *nextidx, size_t *nextpos) const;

    /* search an array from the right side (end) for a string */
    virtual ssize_t rfind( const char *str, ssize_t *pos_r ) const;
    virtual ssize_t rfind( size_t idx, size_t pos, const char *str,
			   ssize_t *pos_r ) const;
    virtual ssize_t rfind( size_t idx, size_t pos, const char *str,
			   ssize_t *pos_r,
			   size_t *nextidx, size_t *nextpos) const;
    virtual ssize_t rfind( const tstring &str, ssize_t *pos_r ) const;
    virtual ssize_t rfind( size_t idx, size_t pos, const tstring &str,
			   ssize_t *pos_r ) const;
    virtual ssize_t rfind( size_t idx, size_t pos, const tstring &str,
			   ssize_t *pos_r, 
			   size_t *nextidx, size_t *nextpos ) const;

    /* search for an element (string) that matches a pattern */
    virtual ssize_t find_matched_str( const char *str ) const;
    virtual ssize_t find_matched_str( size_t idx, const char *str ) const;
    virtual ssize_t find_matched_str( size_t idx, const char *str,
				      size_t *nextidx ) const;
    virtual ssize_t find_matched_str( const tstring &str ) const;
    virtual ssize_t find_matched_str( size_t idx, const tstring &str ) const;
    virtual ssize_t find_matched_str( size_t idx, const tstring &str,
				      size_t *nextidx ) const;

    /* search for an element (file name) that matches a pattern */
    virtual ssize_t find_matched_fn( const char *str ) const;
    virtual ssize_t find_matched_fn( size_t idx, const char *str ) const;
    virtual ssize_t find_matched_fn( size_t idx, const char *str,
				     size_t *nextidx ) const;
    virtual ssize_t find_matched_fn( const tstring &str ) const;
    virtual ssize_t find_matched_fn( size_t idx, const tstring &str ) const;
    virtual ssize_t find_matched_fn( size_t idx, const tstring &str,
				     size_t *nextidx ) const;

    /* search for an element (path name) that matches a pattern */
    virtual ssize_t find_matched_pn( const char *str ) const;
    virtual ssize_t find_matched_pn( size_t idx, const char *str ) const;
    virtual ssize_t find_matched_pn( size_t idx, const char *str,
				     size_t *nextidx ) const;
    virtual ssize_t find_matched_pn( const tstring &str ) const;
    virtual ssize_t find_matched_pn( size_t idx, const tstring &str ) const;
    virtual ssize_t find_matched_pn( size_t idx, const tstring &str,
				     size_t *nextidx ) const;

    /* search for a string using an extended regular expression */
    virtual ssize_t regmatch( const char *pat, ssize_t *pos_r,
			      size_t *span_r ) const;
    virtual ssize_t regmatch( size_t idx, size_t pos, const char *pat,
			      ssize_t *pos_r, size_t *span_r ) const;
    virtual ssize_t regmatch( size_t idx, size_t pos, const char *pat,
			      ssize_t *pos_r, size_t *span_r,
			      size_t *nextidx, size_t *nextpos ) const;
    virtual ssize_t regmatch( const tstring &pat,
			      ssize_t *pos_r, size_t *span_r ) const;
    virtual ssize_t regmatch( size_t idx, size_t pos, const tstring &pat,
			      ssize_t *pos_r, size_t *span_r ) const;
    virtual ssize_t regmatch( size_t idx, size_t pos, const tstring &pat,
			      ssize_t *pos_r, size_t *span_r,
			      size_t *nextidx, size_t *nextpos ) const;
    virtual ssize_t regmatch( const tregex &pat,
			      ssize_t *pos_r, size_t *span_r ) const;
    virtual ssize_t regmatch( size_t idx, size_t pos, const tregex &pat,
			      ssize_t *pos_r, size_t *span_r ) const;

    virtual ssize_t regmatch( size_t idx, size_t pos, const tregex &pat,
			      ssize_t *pos_r, size_t *span_r,
			      size_t *nextidx, size_t *nextpos ) const;

    /* search for a string using an extended regular expression */
    virtual ssize_t regmatch( const char *pat, tarray_tstring *result );
    virtual ssize_t regmatch( size_t idx, size_t pos, const char *pat,
			      tarray_tstring *result );
    virtual ssize_t regmatch( size_t idx, size_t pos, const char *pat,
			      tarray_tstring *result,
			      size_t *nextidx, size_t *nextpos );
    virtual ssize_t regmatch( const tstring &pat,
			      tarray_tstring *result );
    virtual ssize_t regmatch( size_t idx, size_t pos, const tstring &pat,
			      tarray_tstring *result );
    virtual ssize_t regmatch( size_t idx, size_t pos, const tstring &pat,
			      tarray_tstring *result,
			      size_t *nextidx, size_t *nextpos );
    virtual ssize_t regmatch( const tregex &pat,
			      tarray_tstring *result ) const;
    virtual ssize_t regmatch( size_t idx, size_t pos, const tregex &pat,
			      tarray_tstring *result ) const;
    virtual ssize_t regmatch( size_t idx, size_t pos, const tregex &pat,
			      tarray_tstring *result,
			      size_t *nextidx, size_t *nextpos ) const;
    /* not recommended */
    virtual ssize_t regmatch( const char *pat, tarray_tstring &result );
    virtual ssize_t regmatch( size_t idx, size_t pos, const char *pat,
			      tarray_tstring &result );
    virtual ssize_t regmatch( size_t idx, size_t pos, const char *pat,
			      tarray_tstring &result,
			      size_t *nextidx, size_t *nextpos );
    virtual ssize_t regmatch( const tstring &pat,
			      tarray_tstring &result );
    virtual ssize_t regmatch( size_t idx, size_t pos, const tstring &pat,
			      tarray_tstring &result );
    virtual ssize_t regmatch( size_t idx, size_t pos, const tstring &pat,
			      tarray_tstring &result,
			      size_t *nextidx, size_t *nextpos );
    virtual ssize_t regmatch( const tregex &pat,
			      tarray_tstring &result ) const;
    virtual ssize_t regmatch( size_t idx, size_t pos, const tregex &pat,
			      tarray_tstring &result ) const;
    virtual ssize_t regmatch( size_t idx, size_t pos, const tregex &pat,
			      tarray_tstring &result,
			      size_t *nextidx, size_t *nextpos ) const;

    /* returns NULL-terminated pointer array of string elements */
    virtual const char *const *cstrarray() const;

    /* these functions return value of tstring::cstr() */
    virtual const char *c_str( size_t index ) const;
    virtual const char *cstr( size_t index ) const;

    /* these functions return reference of internal tstring object */
    virtual tstring &at( size_t index );
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const tstring &at( size_t index ) const;
#endif
    virtual tstring &element( size_t index );
    virtual const tstring &at_cs( size_t index ) const;

    /* copy internal string into user's buffer */
    virtual ssize_t getstr( size_t index, 
			    char *dest_buf, size_t buf_size ) const;

    /* returns length of array being stored in object */
    virtual size_t length() const;
    /* not recommended */
    virtual size_t size() const;

    /* length of a string element of index */
    virtual size_t length( size_t index ) const;
    /* not recommended */
    virtual size_t size( size_t index ) const;

    /* this returns character at position = pos of string element of index */
    virtual int cchr( size_t index, size_t pos ) const;

    /* this returns the position of the matching string */
    virtual size_t reg_pos( size_t idx ) const;

    /* shallow copy を許可する場合に使う (未実装) */
    virtual void set_scopy_flag();

    /* display internal information of object. */
    /* (for user's debug)                      */
    /* ユーザの debug 用                       */
    virtual void dprint( const char *msg = NULL ) const;

  private:
    tarray_tstring &update_cstr_ptrs_rec();
    bool is_my_buffer( const char *const *ptr ) const;
    ssize_t regexp_match_advanced( const tstring &target, size_t pos,
				   const tregex &regex_ref, size_t *nextpos );
    void init_reg_results();
    /* at(), at_cs() 専用のメンバ関数 */
    tstring &tstring_ref_after_resize( size_t index );
    const tstring &err_throw_const_tstring_ref( const char *fnc,
				       const char *lv, const char *mes ) const;

  private:
    /* 内部バッファ */
    tarray<tstring> arrs_rec;
    mdarray cstr_ptrs_rec;	/* arrs_rec の文字列ポインタのコピー */
    /* 正規表現用 */
    mdarray reg_pos_rec;
    mdarray reg_length_rec;
    tregex regex_rec;

  private:
    /*
      gcc でエラーを出すための，ダミーのメンバ関数

      参照と整数とのオーバーロードはかなりヤバいようだ…
      というわけで，private に退避．．．
     */
    tarray_tstring &assign( size_t n );
    tarray_tstring &append( size_t n );
    tarray_tstring &insert( size_t index, size_t n );
    tarray_tstring &replace( size_t idx1, size_t n1, size_t n2 );
    tarray_tstring &put( size_t index, size_t n );

  };

/*
 * inline member functions
 */

/**
 * @brief  [] で指定された要素値の参照(tstring)を返す
 * 
 *  添え字で指定された配列要素(tstring クラスのオブジェクト) の参照を返します．
 *
 * @param   index 0 から始まる要素番号
 * @return  添え字で指定された配列要素の参照
 * @throw   内部バッファの確保に失敗した場合
 *
 */
inline tstring &tarray_tstring::operator[]( size_t index )
{
    return this->at(index);
}

/**
 * @brief  [] で指定された要素値の参照(tstring)を返す (読取専用)
 *
 *  添え字で指定された配列要素(tstring クラスのオブジェクト) の参照を返します．
 *
 * @param   index 0 から始まる要素番号
 * @return  添え字で指定された配列要素の参照
 * @throw   index に配列長以上の値が指定された場合
 *
 */
inline const tstring &tarray_tstring::operator[]( size_t index ) const
{
    return this->at_cs(index);
}


/**
 * @brief  文字列配列に対するポインタ配列を取得 (読取専用)
 *
 *  自身が持つ文字列配列のそれぞれの文字列要素についてのポインタ配列のアドレス
 *  を返します．
 *
 * @return  文字列バッファへのポインタ配列(NULL 終端)
 *
 */
inline const char *const *tarray_tstring::cstrarray() const
{
    return (const char *const *)(this->cstr_ptrs_rec.data_ptr_cs());
}

/**
 * @brief  指定された要素の値文字列に対するアドレスを返す (読取専用)
 *
 *  indexで指定された要素の値文字列のアドレスを返します．<br>
 *  indexに配列の長さ以上の値が指定された場合，NULL が返ります．
 *
 * @return  値文字列のアドレス
 * @note    tarray_tstring::c_str() との違いはありません．
 */
inline const char *tarray_tstring::cstr( size_t index ) const
{
    return ( (this->length() <= index) ? 
	     NULL : this->arrs_rec.at_cs(index).cstr()
           );
}

/**
 * @brief  指定された要素の値文字列に対するアドレスを返す (読取専用)
 *
 *  indexで指定された要素の値文字列のアドレスを返します．<br>
 *  indexに配列の長さ以上の値が指定された場合，NULL が返ります．
 *
 * @return  値文字列のアドレス
 * @note    tarray_tstring::cstr() との違いはありません．
 */
inline const char *tarray_tstring::c_str( size_t index ) const
{
    return ( (this->length() <= index) ?
	     NULL : this->arrs_rec.at_cs(index).c_str()
	   );
}

/**
 * @brief  指定された配列要素の参照(tstring)を返す
 * 
 * @param   index 要素番号
 * @return  指定された要素番号に該当する文字列の参照
 * @throw   内部バッファの確保に失敗した場合
 */
inline tstring &tarray_tstring::element( size_t index )
{
    return this->at(index);
}

/**
 * @brief  指定された配列要素の参照(tstring)を返す
 *
 * @param   index 要素番号
 * @return  指定された要素番号に該当する文字列オブジェクトの参照
 * @throw   内部バッファの確保に失敗した場合
 */
inline tstring &tarray_tstring::at( size_t index )
{
    return ( (this->length() <= index) ? 
	     this->tstring_ref_after_resize(index) : this->arrs_rec.at(index)
	   );
}

/**
 * @brief  指定された配列要素の参照(tstring)を返す (読取専用)
 *
 * @param   index 要素番号
 * @return  指定された要素番号に該当する文字列オブジェクトの参照
 * @throw   配列長以上の index が指定された場合
 */
#ifdef SLI__OVERLOAD_CONST_AT
inline const tstring &tarray_tstring::at( size_t index ) const
{
    return this->at_cs(index);
}
#endif

/**
 * @brief  指定された配列要素の参照(tstring)を返す (読取専用)
 *
 * @param   index 要素番号
 * @return  指定された要素番号に該当する文字列オブジェクトの参照
 * @throw   配列長以上の index が指定された場合
 */
inline const tstring &tarray_tstring::at_cs( size_t index ) const
{
    return ( (this->length() <= index) ? 
	     this->err_throw_const_tstring_ref("tarray_tstring::at_cs()",
					       "ERROR","Invalid index")
	     :
	     this->arrs_rec.at_cs(index)
           );
}

/**
 * @brief  配列の長さ(個数)を取得
 *
 * @return  自身の文字列配列数
 */
inline size_t tarray_tstring::length() const
{
    return this->arrs_rec.length();
}

/**
 * @brief  配列の長さ(個数)を取得
 *
 * @return  自身の文字列配列数
 */
inline size_t tarray_tstring::size() const
{
    return this->arrs_rec.length();
}

/**
 * @brief  指定された要素の文字列長を返す
 *
 *  引数の要素番号に該当する要素の文字列の長さを返します．
 *
 * @param   index 要素番号
 * @return  指定要素の文字列長
 * @note    tarray_tstring::size() との違いはありません．
 */
inline size_t tarray_tstring::length( size_t index ) const
{
    return ( (this->length() <= index) ? 
	     0 : this->arrs_rec.at_cs(index).length() );
}

/**
 * @brief  指定された要素の文字列長を返す
 *
 *  引数の要素番号に該当する要素の文字列の長さを返します．
 * 
 * @param   index 要素番号
 * @return  指定要素の文字列長
 * @note    tarray_tstring::length() との違いはありません．
 */
inline size_t tarray_tstring::size( size_t index ) const
{
    return this->length(index);
}

/**
 * @brief  指定された要素文字列の位置posにある文字を返す
 *
 *  要素番号で指定された要素の文字列の位置posにある文字を返します．<br>
 *  なお，文字列の先頭位置は 0 です．
 * 
 * @param   index 要素番号
 * @param   pos 文字列の位置
 * @return  正の値: 指定した位置にある文字 <br>
 *          負の値: posに自身の文字列長以上の値を指定した場合
 */
inline int tarray_tstring::cchr( size_t index, size_t pos ) const
{
    return ( (this->length() <= index) ? 
	     -1 : this->arrs_rec.at_cs(index).cchr(pos)
	   );
}


/**
 * @example  examples_sllib/string_array_basic.cc
 *           文字列配列の扱い方の基本を示したコード
 */

/**
 * @example  examples_sllib/split_string.cc
 *           文字列のデリミタでの分割を行なう例
 *           (csvを分割，ssvを分割，高度な分割)
 */

/**
 * @example  examples_sllib/regexp_back_reference.cc
 *           文字列に対しPOSIX拡張正規表現マッチを行ない，後方参照の情報を
 *           含んだ文字列配列を取得する例
 */

}

#endif	/* _SLI__TARRAY_TSTRING_H */
