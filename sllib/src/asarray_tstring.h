/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2013-06-05 11:24:24 cyamauch> */

#ifndef _SLI__ASARRAY_TSTRING_H
#define _SLI__ASARRAY_TSTRING_H 1

/**
 * @file   asarray_tstring.h
 * @brief  ʸ�����Ϣ������򰷤� asarray_tstring ���饹�Υإå��ե�����
 */

#include "sli_config.h"
#include "tarray_tstring.h"
#include "ctindex.h"

namespace sli
{

/*
 * sli::asarray_tstring class manages an associative string array in object,
 * and provides a lot of useful member functions to process associative array.
 */

/**
 * @class  sli::asarray_tstring
 * @brief  ʸ�����Ϣ������򰷤�����Υ��饹
 *
 *   asarray_tstring ��ʸ�����Ϣ������򰷤�����Υ��饹�ǡ�tstring ���饹��
 *   Ʊ�͡�������ץȸ���Τ褦�ʶ��Ϥ�ʸ��������� API ���󶡤��ޤ���������
 *   �褦�ʵ�ǽ������ޤ���<br>
 *    - �ͤΥ��åȡ��ɲá��������� printf() �ε�ˡ���Ȥ��� API <br>
 *    - ����ñ�̤Υ������� <br>
 *    - �����ζ��򡤲���ʸ���ν��� <br>
 *    - ʸ����Υǥ�ߥ��Ǥ�ʬ�� <br>
 *    - ����ɽ���ˤ���ִ� <br>
 *    - ����ʸ��������ʸ������Ф���ݥ�������μ��� <br>
 *
 * @note  ���֥������������Ǥϡ�ʸ�����ñ�������Ȥ�����¸����ޤ���Ʊ���ˡ�
 *        ����ʸ����������ֹ�Ȥδط�����¸���졤����ˤ� ctindex ���饹���Ѥ�
 *        �Ƥ��ޤ���ctindex ���饹�ϥ���ʸ����������ֹ�Ȥδط��򡤡��ڡ׹�¤
 *        �ǵ�Ͽ������®�ʸ���(����ʸ���󤫤������ֹ�ؤ��Ѵ�)���ǽ�ˤ��Ƥ���
 *        ���������������ڡ׹�¤�Ǥ��Τǡ������ˤϤ����֤�������ޤ���
 * @note  �����ΥХåե��� 1Gbyte ̤���Ǥ� 2^n ñ�̤ǳ��ݤ���뤿�ᡤ���ʤ�
 *        ���ˤʹ����������Ф��Ƥ⽽ʬ�ʥѥե����ޥ󥹤�������Ǥ��礦��
 * @author  Chisato YAMAUCHI
 * @date  2013-04-01 00:00:00
 */

  typedef struct {
    const char *key;
    const char *value;
  } asarrdef_tstring;

  class asarray_tstring
  {

  public:
    /* constructor and destructor */
    asarray_tstring();
    asarray_tstring(const asarrdef_tstring elements[]);
    asarray_tstring(const char *key0, const char *val0,
		    const char *key1, ...);	/* ��NULL��ü */
    asarray_tstring(const asarray_tstring &obj);
    virtual ~asarray_tstring();

    /*
     * operator
     */
    /* same as .init(obj) */
    virtual asarray_tstring &operator=(const asarray_tstring &obj);

    /* same as .at(key) */
    virtual tstring &operator[]( const char *key );
    virtual const tstring &operator[]( const char *key ) const;

    /*
     * member functions
     */
    /* complete initialization of objects */
    virtual asarray_tstring &init();
    virtual asarray_tstring &init(const asarray_tstring &obj);

    /* clean() member functions pads all the element values of an existing */
    /* associative array with any string.                                  */
    virtual asarray_tstring &clean(const char *str = "");
    virtual asarray_tstring &clean(const tstring &str);

    /* set an associative string array */
    virtual asarray_tstring &assign( const asarray_tstring &src );
    virtual asarray_tstring &assign( const asarrdef_tstring elements[] );
    virtual asarray_tstring &assign( const asarrdef_tstring elements[],
				     size_t n );
    virtual asarray_tstring &assign( const char *key, const char *val );
    virtual asarray_tstring &assign( const char *key, const tstring &val );
    virtual asarray_tstring &assignf( const char *key, const char *fmt, ... );
    virtual asarray_tstring &vassignf( const char *key, 
				       const char *fmt, va_list ap );
    virtual asarray_tstring &assign( const char *key0, const char *val0,
				     const char *key1, ... );
    virtual asarray_tstring &vassign( const char *key0, const char *val0,
				      const char *key1, va_list ap );

    /* set multiple strings or a string array to keys */
    virtual asarray_tstring &assign_keys( const char *key0, ... );
    virtual asarray_tstring &vassign_keys( const char *key0, va_list ap );
    virtual asarray_tstring &assign_keys( const char *const *keys );
    virtual asarray_tstring &assign_keys( const tarray_tstring &keys );

    /* divide strings and sets them to keys */
    virtual asarray_tstring &split_keys( const char *src_str, 
					 const char *delims, 
					 bool zero_str, const char *quotations,
					 int escape, bool rm_escape );
    virtual asarray_tstring &split_keys( const char *src_str, 
					 const char *delims,
					 bool zero_str = false );
    virtual asarray_tstring &split_keys( const tstring &src_str,
					 const char *delims, 
					 bool zero_str, const char *quotations,
					 int escape, bool rm_escape );
    virtual asarray_tstring &split_keys( const tstring &src_str,
					 const char *delims,
					 bool zero_str = false );

    /* set multiple strings or a string array to values */
    virtual asarray_tstring &assign_values( const char *val0, ... );
    virtual asarray_tstring &vassign_values( const char *val0, va_list ap );
    virtual asarray_tstring &assign_values( const char *const *values );
    virtual asarray_tstring &assign_values( const tarray_tstring &values );

    /* divide strings and sets them to values */
    virtual asarray_tstring &split_values( const char *src_str, 
					 const char *delims, 
					 bool zero_str, const char *quotations,
					 int escape, bool rm_escape );
    virtual asarray_tstring &split_values( const char *src_str, 
					   const char *delims,
					   bool zero_str = false );
    virtual asarray_tstring &split_values( const tstring &src_str, 
					 const char *delims, 
					 bool zero_str, const char *quotations,
					 int escape, bool rm_escape );
    virtual asarray_tstring &split_values( const tstring &src_str, 
					   const char *delims,
					   bool zero_str = false );

    /* add elements */
    virtual asarray_tstring &append( const asarray_tstring &src );
    virtual asarray_tstring &append( const asarrdef_tstring elements[] );
    virtual asarray_tstring &append( const asarrdef_tstring elements[], 
				     size_t n );
    virtual asarray_tstring &append( const char *key, const char *val );
    virtual asarray_tstring &append( const char *key, const tstring &val );
    virtual asarray_tstring &appendf( const char *key, const char *fmt, ... );
    virtual asarray_tstring &vappendf( const char *key, 
				       const char *fmt, va_list ap );
    virtual asarray_tstring &append( const char *key0, const char *val0, 
				     const char *key1, ... );
    virtual asarray_tstring &vappend( const char *key0, const char *val0, 
				      const char *key1, va_list ap );

    /* insert elements */
    virtual asarray_tstring &insert( const char *key,
				     const asarray_tstring &src );
    virtual asarray_tstring &insert( const char *key, 
				     const asarrdef_tstring elements[] );
    virtual asarray_tstring &insert( const char *key, 
				     const asarrdef_tstring elements[], 
				     size_t n );
    virtual asarray_tstring &insert( const char *key,
				     const char *newkey, const char *newval );
    virtual asarray_tstring &insert( const char *key,
				   const char *newkey, const tstring &newval );
    virtual asarray_tstring &insertf( const char *key,
				      const char *newkey, 
				      const char *fmt, ... );
    virtual asarray_tstring &vinsertf( const char *key,
				       const char *newkey, 
				       const char *fmt, va_list ap );
    virtual asarray_tstring &insert( const char *key, 
				     const char *key0, const char *val0, 
				     const char *key1, ... );
    virtual asarray_tstring &vinsert( const char *key,
				      const char *key0, const char *val0, 
				      const char *key1, va_list ap );

    /* deletes elements */
    virtual asarray_tstring &erase();
    virtual asarray_tstring &erase( const char *key, size_t num_elements = 1 );

    /* interchanges objects */
    virtual asarray_tstring &swap( asarray_tstring &sobj );

    /* changes of key strings */
    virtual asarray_tstring &rename_a_key( const char *org_key,
					   const char *new_key );

    /* ���٤Ƥ�ʸ�������Ǥ��Ф����Խ� */

    /* string search and replacement of all the elements */
    virtual asarray_tstring &strreplace( const char *org_str,
					 const char *new_str,
					 bool all = false );
    virtual asarray_tstring &strreplace( const tstring &org_str, 
					 const char *new_str, 
					 bool all = false );
    virtual asarray_tstring &strreplace( const char *org_str,
					 const tstring &new_str,
					 bool all = false );
    virtual asarray_tstring &strreplace( const tstring &org_str, 
					 const tstring &new_str, 
					 bool all = false );

    /* elimination of newline characters in all the elements */
    /* perform tstring::chop() for all elements              */
    virtual asarray_tstring &chop();
    /* perform tstring::chomp() for all elements             */
    virtual asarray_tstring &chomp( const char *rs = "\n" );
    virtual asarray_tstring &chomp( const tstring &rs );

    /* elimination of spaces at both ends of all the elements */
    /* (perform tstring::trim() for all elements)             */
    virtual asarray_tstring &trim( const char *side_spaces = " \t\n\r\f\v" );
    virtual asarray_tstring &trim( const tstring &side_spaces );
    virtual asarray_tstring &trim( int side_space );

    /* elimination of a space at the left end of all the elements */
    /* (perform tstring::ltrim() for all elements)                */
    virtual asarray_tstring &ltrim( const char *side_spaces = " \t\n\r\f\v" );
    virtual asarray_tstring &ltrim( const tstring &side_spaces );
    virtual asarray_tstring &ltrim( int side_space );

    /* elimination of a space at the right end of all the elements */
    /* (perform tstring::rtrim() for all elements)                 */
    virtual asarray_tstring &rtrim( const char *side_spaces = " \t\n\r\f\v" );
    virtual asarray_tstring &rtrim( const tstring &side_spaces );
    virtual asarray_tstring &rtrim( int side_space );

    /* same as trim() */
    virtual asarray_tstring &strtrim(const char *side_spaces = " \t\n\r\f\v");
    virtual asarray_tstring &strtrim( const tstring &side_spaces );
    virtual asarray_tstring &strtrim( int side_space );

    /* convert all elements to lowercase */
    virtual asarray_tstring &tolower();

    /* convert all elements to uppercase */
    virtual asarray_tstring &toupper();

    /* replacement of tab or space characters for all elements */
    /* perform tstring::expand_tabs() for all elements         */
    virtual asarray_tstring &expand_tabs( size_t tab_width = 8 );
    /* perform tstring::contract_spaces() for all elements     */
    virtual asarray_tstring &contract_spaces( size_t tab_width = 8 );

    /* string search and replacement of all the elements using a regular */
    /* expression (perform tstring::regreplace() for all elements)       */
    virtual asarray_tstring &regreplace( const char *pat, 
				       const char *new_str, bool all = false );
    virtual asarray_tstring &regreplace( const tstring &pat, 
				       const char *new_str, bool all = false );
    virtual asarray_tstring &regreplace( const tregex &pat, 
				       const char *new_str, bool all = false );
    virtual asarray_tstring &regreplace( const char *pat, 
				    const tstring &new_str, bool all = false );
    virtual asarray_tstring &regreplace( const tstring &pat, 
				    const tstring &new_str, bool all = false );
    virtual asarray_tstring &regreplace( const tregex &pat, 
				    const tstring &new_str, bool all = false );

    /* returns NULL-terminated pointer array of string elements */
    virtual const char *const *cstrarray() const;

    /* these functions return value of tstring::cstr() */
    virtual const char *c_str( const char *key ) const;
    virtual const char *cstr( const char *key ) const;
    virtual const char *cstrf( const char *fmt, ... ) const;
    virtual const char *vcstrf( const char *fmt, va_list ap ) const;

    /* these functions return reference to the element value object */
    /* (tstring class) corresponding to a specified key string.     */
    virtual tstring &at( const char *key );
    virtual tstring &atf( const char *fmt, ... );
    virtual tstring &vatf( const char *fmt, va_list ap );
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const tstring &at( const char *key ) const;
    virtual const tstring &atf( const char *fmt, ... ) const;
    virtual const tstring &vatf( const char *fmt, va_list ap ) const;
#endif
    virtual const tstring &at_cs( const char *key ) const;
    virtual const tstring &atf_cs( const char *fmt, ... ) const;
    virtual const tstring &vatf_cs( const char *fmt, va_list ap ) const;
    virtual tstring &element( const char *key );

    /* copy internal string into user's buffer */
    virtual ssize_t getstr( const char *key, 
			    char *dest_buf, size_t buf_size ) const;

    /* returns length of array being stored in object */
    virtual size_t length() const;
    /* not recommended */
    virtual size_t size() const;

    /* length of a string element */
    virtual size_t length( const char *key ) const;
    /* not recommended */
    virtual size_t size( const char *key ) const;

    /* this returns character at position = pos of a string element */
    virtual int cchr( const char *key, size_t pos ) const;

    /* acquire the element number corresponding to a key string */
    virtual ssize_t index( const char *key ) const;
    virtual ssize_t indexf( const char *fmt, ... ) const;
    virtual ssize_t vindexf( const char *fmt, va_list ap ) const;

    /* acquire the key string corresponding to an element number */
    virtual const char *key( size_t index ) const;

    /* acquire the value string corresponding to an element number */
    virtual const char *cstr( size_t index ) const;

    /* these functions return reference to the element value object */
    /* (tstring class) corresponding to a specified element number. */
    virtual tstring &at( size_t index );
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const tstring &at( size_t index ) const;
#endif
    virtual tstring &element( size_t index );
    virtual const tstring &at_cs( size_t index ) const;

    /* �쥳���ɤλ��Ȥ򤽤Τޤ��֤����������ɤ߼������ */

    /* returns reference of the array object for key strings (read only) */
    virtual const tarray_tstring &keys() const;

    /* returns reference of the array object for value strings (read only) */
    virtual const tarray_tstring &values() const;

    /* shallow copy ����Ĥ�����˻Ȥ� (̤����) */
    virtual void set_scopy_flag();

    /* display internal information of object. */
    /* (for user's debug)                      */
    /* �桼���� debug ��                       */
    virtual void dprint( const char *msg = NULL ) const;

  private:
    tarray_tstring key_rec;
    tarray_tstring value_rec;
    ctindex index_rec;
    /* ����ɽ���� */
    tregex regex_rec;

  };


/**
 * @example  examples_sllib/associative_string_array.cc
 *           ʸ�����Ϣ������ΰ������򼨤���������
 */

}

#endif	/* _SLI__ASARRAY_TSTRING_H */
