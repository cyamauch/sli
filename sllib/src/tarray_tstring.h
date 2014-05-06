/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2013-06-05 11:25:43 cyamauch> */

#ifndef _SLI__TARRAY_TSTRING_H
#define _SLI__TARRAY_TSTRING_H 1

/**
 * @file   tarray_tstring.h
 * @brief  ʸ��������򰷤�tarray_tstring���饹�������inline���дؿ�
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
 * @brief  ʸ��������򰷤�����Υ��饹
 *
 *   tarray_tstring ��ʸ��������򰷤�����Υ��饹�ǡ�tstring ���饹��Ʊ�͡�
 *   ������ץȸ���Τ褦�ʶ��Ϥ�ʸ��������� API ���󶡤��ޤ��������Τ褦��
 *   ��ǽ������ޤ���<br>
 *    - �ͤΥ��åȡ��ɲá��������� printf() �ε�ˡ���Ȥ��� API <br>
 *    - ����ñ�̤Υ������� <br>
 *    - ����Ū��ʸ����ʸ����θ��� <br>
 *    - �����ζ��򡤲���ʸ���ν��� <br>
 *    - ʸ����Υǥ�ߥ��Ǥ�ʬ�� <br>
 *    - ����������ʸ����ޥå� <br>
 *    - ����ɽ���ˤ�븡�����ִ����������Ⱦ���μ��� <br>
 *    - ����ʸ������Ф���ݥ�������μ��� <br>
 *
 * @note  �����ΥХåե��� 1Gbyte ̤���Ǥ� 2^n ñ�̤ǳ��ݤ���뤿�ᡤ���ʤ�
 *        ���ˤʹ����������Ф��Ƥ⽽ʬ�ʥѥե����ޥ󥹤�������Ǥ��礦��
 * @author  Chisato YAMAUCHI
 * @date  2013-04-01 00:00:00
 */
 
  class tarray_tstring
  {

  public:
    /* constructor and destructor */
    tarray_tstring();
    tarray_tstring(const char *const *elements);
    tarray_tstring(const char *el0, const char *el1, ...);     /* ��NULL��ü */
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
				    ... );	/* ��NULL��ü */
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
				    ... );	/* ��NULL��ü */
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
				    ... );	/* ��NULL��ü */
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
				     ... );	/* ��NULL��ü */
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
				 ... );	/* ��NULL��ü */
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
    /* ���٤Ƥ�ʸ�������Ǥ��Ф���ʸ�����ִ�                                */
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
    /* str �˰��פ������Ǥ��֤�                                  */
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

    /* shallow copy ����Ĥ�����˻Ȥ� (̤����) */
    virtual void set_scopy_flag();

    /* display internal information of object. */
    /* (for user's debug)                      */
    /* �桼���� debug ��                       */
    virtual void dprint( const char *msg = NULL ) const;

  private:
    tarray_tstring &update_cstr_ptrs_rec();
    bool is_my_buffer( const char *const *ptr ) const;
    ssize_t regexp_match_advanced( const tstring &target, size_t pos,
				   const tregex &regex_ref, size_t *nextpos );
    void init_reg_results();
    /* at(), at_cs() ���ѤΥ��дؿ� */
    tstring &tstring_ref_after_resize( size_t index );
    const tstring &err_throw_const_tstring_ref( const char *fnc,
				       const char *lv, const char *mes ) const;

  private:
    /* �����Хåե� */
    tarray<tstring> arrs_rec;
    mdarray cstr_ptrs_rec;	/* arrs_rec ��ʸ����ݥ��󥿤Υ��ԡ� */
    /* ����ɽ���� */
    mdarray reg_pos_rec;
    mdarray reg_length_rec;
    tregex regex_rec;

  private:
    /*
      gcc �ǥ��顼��Ф�����Ρ����ߡ��Υ��дؿ�

      ���Ȥ������ȤΥ����С����ɤϤ��ʤ��Ф��褦����
      �Ȥ����櫓�ǡ�private �����򡥡���
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
 * @brief  [] �ǻ��ꤵ�줿�����ͤλ���(tstring)���֤�
 * 
 *  ź�����ǻ��ꤵ�줿��������(tstring ���饹�Υ��֥�������) �λ��Ȥ��֤��ޤ���
 *
 * @param   index 0 ����Ϥޤ������ֹ�
 * @return  ź�����ǻ��ꤵ�줿�������Ǥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
inline tstring &tarray_tstring::operator[]( size_t index )
{
    return this->at(index);
}

/**
 * @brief  [] �ǻ��ꤵ�줿�����ͤλ���(tstring)���֤� (�ɼ�����)
 *
 *  ź�����ǻ��ꤵ�줿��������(tstring ���饹�Υ��֥�������) �λ��Ȥ��֤��ޤ���
 *
 * @param   index 0 ����Ϥޤ������ֹ�
 * @return  ź�����ǻ��ꤵ�줿�������Ǥλ���
 * @throw   index ������Ĺ�ʾ���ͤ����ꤵ�줿���
 *
 */
inline const tstring &tarray_tstring::operator[]( size_t index ) const
{
    return this->at_cs(index);
}


/**
 * @brief  ʸ����������Ф���ݥ����������� (�ɼ�����)
 *
 *  ���Ȥ�����ʸ��������Τ��줾���ʸ�������ǤˤĤ��ƤΥݥ�������Υ��ɥ쥹
 *  ���֤��ޤ���
 *
 * @return  ʸ����Хåե��ؤΥݥ�������(NULL ��ü)
 *
 */
inline const char *const *tarray_tstring::cstrarray() const
{
    return (const char *const *)(this->cstr_ptrs_rec.data_ptr_cs());
}

/**
 * @brief  ���ꤵ�줿���Ǥ���ʸ������Ф��륢�ɥ쥹���֤� (�ɼ�����)
 *
 *  index�ǻ��ꤵ�줿���Ǥ���ʸ����Υ��ɥ쥹���֤��ޤ���<br>
 *  index�������Ĺ���ʾ���ͤ����ꤵ�줿��硤NULL ���֤�ޤ���
 *
 * @return  ��ʸ����Υ��ɥ쥹
 * @note    tarray_tstring::c_str() �Ȥΰ㤤�Ϥ���ޤ���
 */
inline const char *tarray_tstring::cstr( size_t index ) const
{
    return ( (this->length() <= index) ? 
	     NULL : this->arrs_rec.at_cs(index).cstr()
           );
}

/**
 * @brief  ���ꤵ�줿���Ǥ���ʸ������Ф��륢�ɥ쥹���֤� (�ɼ�����)
 *
 *  index�ǻ��ꤵ�줿���Ǥ���ʸ����Υ��ɥ쥹���֤��ޤ���<br>
 *  index�������Ĺ���ʾ���ͤ����ꤵ�줿��硤NULL ���֤�ޤ���
 *
 * @return  ��ʸ����Υ��ɥ쥹
 * @note    tarray_tstring::cstr() �Ȥΰ㤤�Ϥ���ޤ���
 */
inline const char *tarray_tstring::c_str( size_t index ) const
{
    return ( (this->length() <= index) ?
	     NULL : this->arrs_rec.at_cs(index).c_str()
	   );
}

/**
 * @brief  ���ꤵ�줿�������Ǥλ���(tstring)���֤�
 * 
 * @param   index �����ֹ�
 * @return  ���ꤵ�줿�����ֹ�˳�������ʸ����λ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
inline tstring &tarray_tstring::element( size_t index )
{
    return this->at(index);
}

/**
 * @brief  ���ꤵ�줿�������Ǥλ���(tstring)���֤�
 *
 * @param   index �����ֹ�
 * @return  ���ꤵ�줿�����ֹ�˳�������ʸ���󥪥֥������Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
inline tstring &tarray_tstring::at( size_t index )
{
    return ( (this->length() <= index) ? 
	     this->tstring_ref_after_resize(index) : this->arrs_rec.at(index)
	   );
}

/**
 * @brief  ���ꤵ�줿�������Ǥλ���(tstring)���֤� (�ɼ�����)
 *
 * @param   index �����ֹ�
 * @return  ���ꤵ�줿�����ֹ�˳�������ʸ���󥪥֥������Ȥλ���
 * @throw   ����Ĺ�ʾ�� index �����ꤵ�줿���
 */
#ifdef SLI__OVERLOAD_CONST_AT
inline const tstring &tarray_tstring::at( size_t index ) const
{
    return this->at_cs(index);
}
#endif

/**
 * @brief  ���ꤵ�줿�������Ǥλ���(tstring)���֤� (�ɼ�����)
 *
 * @param   index �����ֹ�
 * @return  ���ꤵ�줿�����ֹ�˳�������ʸ���󥪥֥������Ȥλ���
 * @throw   ����Ĺ�ʾ�� index �����ꤵ�줿���
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
 * @brief  �����Ĺ��(�Ŀ�)�����
 *
 * @return  ���Ȥ�ʸ���������
 */
inline size_t tarray_tstring::length() const
{
    return this->arrs_rec.length();
}

/**
 * @brief  �����Ĺ��(�Ŀ�)�����
 *
 * @return  ���Ȥ�ʸ���������
 */
inline size_t tarray_tstring::size() const
{
    return this->arrs_rec.length();
}

/**
 * @brief  ���ꤵ�줿���Ǥ�ʸ����Ĺ���֤�
 *
 *  �����������ֹ�˳����������Ǥ�ʸ�����Ĺ�����֤��ޤ���
 *
 * @param   index �����ֹ�
 * @return  �������Ǥ�ʸ����Ĺ
 * @note    tarray_tstring::size() �Ȥΰ㤤�Ϥ���ޤ���
 */
inline size_t tarray_tstring::length( size_t index ) const
{
    return ( (this->length() <= index) ? 
	     0 : this->arrs_rec.at_cs(index).length() );
}

/**
 * @brief  ���ꤵ�줿���Ǥ�ʸ����Ĺ���֤�
 *
 *  �����������ֹ�˳����������Ǥ�ʸ�����Ĺ�����֤��ޤ���
 * 
 * @param   index �����ֹ�
 * @return  �������Ǥ�ʸ����Ĺ
 * @note    tarray_tstring::length() �Ȥΰ㤤�Ϥ���ޤ���
 */
inline size_t tarray_tstring::size( size_t index ) const
{
    return this->length(index);
}

/**
 * @brief  ���ꤵ�줿����ʸ����ΰ���pos�ˤ���ʸ�����֤�
 *
 *  �����ֹ�ǻ��ꤵ�줿���Ǥ�ʸ����ΰ���pos�ˤ���ʸ�����֤��ޤ���<br>
 *  �ʤ���ʸ�������Ƭ���֤� 0 �Ǥ���
 * 
 * @param   index �����ֹ�
 * @param   pos ʸ����ΰ���
 * @return  ������: ���ꤷ�����֤ˤ���ʸ�� <br>
 *          �����: pos�˼��Ȥ�ʸ����Ĺ�ʾ���ͤ���ꤷ�����
 */
inline int tarray_tstring::cchr( size_t index, size_t pos ) const
{
    return ( (this->length() <= index) ? 
	     -1 : this->arrs_rec.at_cs(index).cchr(pos)
	   );
}


/**
 * @example  examples_sllib/string_array_basic.cc
 *           ʸ��������ΰ������δ��ܤ򼨤���������
 */

/**
 * @example  examples_sllib/split_string.cc
 *           ʸ����Υǥ�ߥ��Ǥ�ʬ���Ԥʤ���
 *           (csv��ʬ�䡤ssv��ʬ�䡤���٤�ʬ��)
 */

/**
 * @example  examples_sllib/regexp_back_reference.cc
 *           ʸ������Ф�POSIX��ĥ����ɽ���ޥå���Ԥʤ����������Ȥξ����
 *           �ޤ��ʸ������������������
 */

}

#endif	/* _SLI__TARRAY_TSTRING_H */
