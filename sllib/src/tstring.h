/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2013-06-05 11:26:16 cyamauch> */

#ifndef _SLI__TSTRING_H
#define _SLI__TSTRING_H 1

/**
 * @file   tstring.h
 * @brief  ʸ����򰷤�����Υ��饹 tstring �������inline���дؿ�
 */

#ifdef TSTRING__USE_SOLO_NARG
#undef TSTRING__USE_SOLO_NARG
#endif
//#define TSTRING__USE_SOLO_NARG

#include <stddef.h>
#include <sys/types.h>
#include <stdarg.h>

#undef strcat
#undef strncat
#undef tolower
#undef toupper
#undef atoi
#undef atol
#undef atoll
#undef atof
#undef strtol
#undef strtoll
#undef strtoul
#undef strtoull
#undef strtod
#undef strncmp
#undef strcmp
#undef strcasecmp
#undef strncasecmp
#undef strstr
#undef strchr
#undef strrchr
#undef isalnum
#undef isalpha
#undef iscntrl
#undef isdigit
#undef isgraph
#undef islower
#undef isprint
#undef ispunct
#undef isspace
#undef isupper
#undef isxdigit
#undef strpbrk
#undef strspn
#undef strcspn

#include "sli_config.h"
#include "tregex.h"

#include "sli_eof.h"

#include "slierr.h"

#ifndef SLI_TSTRING_NO_CSET
#define CSET_ALNUM  "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define CSET_ALPHA  "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define CSET_LOWER  "abcdefghijklmnopqrstuvwxyz"
#define CSET_UPPER  "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define CSET_DIGIT  "0123456789"
#define CSET_XDIGIT "0123456789abcdefABCDEF"
#endif

namespace sli
{

/*
 * sli::tstring class manages a string in its object, and provides a lot of 
 * useful member functions to process the string.  Provided member functions
 * include APIs found in LIBC, Perl, PHP, etc.  POSIX extended regular 
 * expression is also supported, and a number of member functions support 
 * printf() style variable arguments.
 */

/**
 * @class  sli::tstring
 * @brief  ʸ����򰷤�����Υ��饹
 *
 *   tstring ���饹�ϡ�������ץȸ���Τ褦�ʶ��Ϥ�ʸ��������� API �������
 *   ���� LIBC �� stdio.h��string.h��strings.h��stdlib.h��ctype.h ���󶡤����
 *   �ؿ��Ȥ��ä����ʸ����������Ԥ���API���󶡤��ޤ��������Τ褦�ʵ�ǽ�����
 *   �ޤ���<br>
 *    - �ͤΥ��åȡ��ɲá��������� printf() �ε�ˡ���Ȥ��� API <br>
 *    - 1ʸ��ñ�̤Υ������� <br>
 *    - ����Ū��ʸ����ʸ����θ��� <br>
 *    - ʸ�����Ƚ�� <br>
 *    - ���ͤؤ��Ѵ� <br>
 *    - �����ζ��򡤲���ʸ���ν��� <br>
 *    - ʸ��������η�� <br>
 *    - ����������ʸ����ޥå� <br>
 *    - ����ɽ���ˤ�븡�����ִ�(�������Ȥ��ǽ) <br>
 *    - �����γ�̤β��� �ʤ�
 *
 * @attention  LIBC �� string.h ��ʻ�Ѥ����硤tstring.h �������� string.h
 *             �� include ���Ƥ���������
 * @note  ʸ�����ѤΥХåե��� 1Gbyte ̤���Ǥ� 2^n ñ�̤ǳ��ݤ���뤿�ᡤ���ʤ�
 *        ���ˤʹ����������Ф��Ƥ⽽ʬ�ʥѥե����ޥ󥹤�������Ǥ��礦��
 * @author Chisato YAMAUCHI
 * @date 2013-04-01 00:00:00
 */

  class tstring
  {

  public:
    /* constructor                                                           */
    /* There are three operating modes for use with tstring class objects.   */
    /*  - Normal mode (default)                                              */
    /*    .cstr() can return NULL.                                           */
    /*    `obj = NULL;' or `obj.init();' frees internal buffer.              */
    /*  - NULL-free mode                                                     */
    /*    .cstr() never returns NULL.                                        */
    /*    `obj = NULL;' or `obj.init();' does not free internal buffer.      */
    /*    Set true to `buffer_keeps_string' arg to use this mode.            */
    /*  - Fixed-length buffer mode                                           */
    /*    In this mode only strings equal to or shorter than the string      */
    /*    length initially specified can be handled, with the member         */
    /*    functions for editing strings being designed to run at high        */
    /*    speed.  In this mode, .cstr() never returns NULL.  Set an integer  */
    /*    value to `max_length' arg to use this mode.                        */
    /* see description of register_extptr() for extptr_address arg.          */
    tstring(char **extptr_address = NULL);
    tstring(bool buffer_keeps_string, char **extptr_address = NULL);
    tstring(size_t max_length, char **extptr_address = NULL);
    tstring(int max_length, char **extptr_address = NULL);

    /* copy constructor */
    tstring(const tstring &obj);
    tstring(const char *str);

    /* destructor */
    virtual ~tstring();

    /*
     * operator
     */
    /* same as .init(obj) */
    virtual tstring &operator=(const tstring &obj);

    /* same as .append(obj,0) */
    virtual tstring &operator+=(const tstring &obj);

    /* same as (self.compare(obj,0) == 0) */
    virtual bool operator==(const tstring &obj) const;

    /* same as (self.compare(obj,0) != 0) */
    virtual bool operator!=(const tstring &obj) const;

    /* same as (self.compare(obj,0) < 0) */
    virtual bool operator<(const tstring &obj) const;

    /* same as .assign(str) for str != NULL, same as init() for str == NULL */
    /* but returns address of internal buffer                               */
    virtual const char *operator=(const char *str);

    /* same as .append(str) but returns address of internal buffer */
    virtual const char *operator+=(const char *str);

    /* same as (self.compare(str) == 0) */
    virtual bool operator==(const char *str) const;

    /* same as (self.compare(str) != 0) */
    virtual bool operator!=(const char *str) const;

    /* same as (self.compare(str) < 0) */
    virtual bool operator<(const char *str) const;

    /* same as .at(pos) */
    virtual unsigned char &operator[]( size_t pos );
    virtual const unsigned char &operator[]( size_t pos ) const;

#if 0
    virtual operator const char *() const;
#endif

    /*
     * member functions
     */
    /* initialize completely */
    virtual tstring &init();
    virtual tstring &init(const tstring &obj);		/* copy from obj */

    /* change length of string */
    virtual tstring &resize( size_t len );
    virtual tstring &resizeby( ssize_t len );		/* relatively */

    /* fill all chars with `ch' (length of string is not changed) */
    virtual tstring &clean( int ch = ' ' );

    /* same as snprintf() of LIBC */
    virtual tstring &printf( const char *format, ... );
    virtual tstring &vprintf( const char *format, va_list ap );

    /* set a string                         */
    /*   src: source string                 */
    /*   pos: position of first char in src */
    /*   n:   length of used string in src  */
    virtual tstring &assign( const tstring &src, size_t pos2 = 0 );
    virtual tstring &assign( const tstring &src, size_t pos2, size_t n2 );
    virtual tstring &assignf( const char *format, ... );
    virtual tstring &vassignf( const char *format, va_list ap );
    /*   str: source string                 */
    /*   n:   length of used string in str  */
    virtual tstring &assign( const char *str, size_t n );
    virtual tstring &assign( const char *str );
    /* set a string consist of character code = ch, length = n */
    virtual tstring &assign( int ch, size_t n );
#ifdef TSTRING__USE_SOLO_NARG
    virtual tstring &assign( size_t n );
#endif

    /* overwrite a string at position = pos   */
    /* (supports automatic resize of buffer)  */
    virtual tstring &putf( size_t pos, const char *format, ... );
    virtual tstring &vputf( size_t pos, const char *format, va_list ap );
    /*   str: source string                   */
    /*   n:   length of used string in str    */
    virtual tstring &put( size_t pos, const char *str );
    virtual tstring &put( size_t pos, const char *str, size_t n );
    /* a string consist of character code = ch, length = n is used */
    virtual tstring &put( size_t pos, int ch, size_t n );
#ifdef TSTRING__USE_SOLO_NARG
    virtual tstring &put( size_t pos, size_t n );
#endif
    /*   src:  source string                  */
    /*   pos2: position of first char in src  */
    /*   n2:   length of used string in src   */
    virtual tstring &put( size_t pos1, const tstring &src, size_t pos2 = 0 );
    virtual tstring &put( size_t pos1, 
			  const tstring &src, size_t pos2, size_t n2 );

    /* append a string                       */
    virtual tstring &strcat( const char *str );		/* same as append() */
    virtual tstring &strncat( const char *str, size_t n );
    virtual tstring &strcat( const tstring &src, size_t pos2 = 0 );
    virtual tstring &strncat( const tstring &src, size_t pos2, size_t n2 );
    /* argument is the same as that of printf() of LIBC */
    virtual tstring &appendf( const char *format, ... );
    virtual tstring &vappendf( const char *format, va_list ap );
    /*   str: source string                  */
    /*   n:   length of used string in str   */
    virtual tstring &append( const char *str, size_t n );
    virtual tstring &append( const char *str );
    /*   src:  source string                 */
    /*   pos2: position of first char in src */
    /*   n2:   length of used string in src  */
    virtual tstring &append( const tstring &src, size_t pos2 = 0 );
    virtual tstring &append( const tstring &src, size_t pos2, size_t n2 );
    /* a string consist of character code = ch, length = n is used */
    virtual tstring &append( int ch, size_t n );
#ifdef TSTRING__USE_SOLO_NARG
    virtual tstring &append( size_t n );
#endif

    /* insert a string at position = pos1    */
    virtual tstring &insertf( size_t pos1, const char *format, ... );
    virtual tstring &vinsertf( size_t pos1, const char *format, va_list ap );
    /*   str: source string                  */
    /*   n:   length of used string in str   */
    virtual tstring &insert( size_t pos1, const char *str, size_t n );
    virtual tstring &insert( size_t pos1, const char *str );
    /*   src:  source string                 */
    /*   pos2: position of first char in src */
    /*   n2:   length of used string in src  */
    virtual tstring &insert( size_t pos1, 
			     const tstring &src, size_t pos2 = 0 );
    virtual tstring &insert( size_t pos1, 
			     const tstring &src, size_t pos2, size_t n2 );
    /* a string consist of character code = ch, length = n is used */
    virtual tstring &insert( size_t pos1, int ch, size_t n );
#ifdef TSTRING__USE_SOLO_NARG
    virtual tstring &insert( size_t pos1, size_t n );
#endif

    /* replace a part of string (position = pos1, length = n) with */
    /* specified one.                                              */
    /* pos1ʸ���ܤ���n1ʸ���� str��n2ʸ�����֤�������              */
    virtual tstring &replacef( size_t pos1, size_t n1, 
			       const char *format, ... );
    virtual tstring &vreplacef( size_t pos1, size_t n1, 
				const char *format, va_list ap );
    /*   str: source string                  */
    /*   n2:  length of used string in str   */
    virtual tstring &replace( size_t pos1, size_t n1, 
			      const char *str, size_t n2 );
    virtual tstring &replace( size_t pos1, size_t n1, const char *str );
    /*   src:  source string                 */
    /*   pos2: position of first char in src */
    /*   n2:   length of used string in src  */
    virtual tstring &replace( size_t pos1, size_t n1, 
			      const tstring &src, size_t pos2 = 0 );
    virtual tstring &replace( size_t pos1, size_t n1, 
			      const tstring &src, size_t pos2, size_t n2 );
    /* a string consist of character code = ch, length = n2 is used */
    virtual tstring &replace( size_t pos1, size_t n1, int ch, size_t n2 );
#ifdef TSTRING__USE_SOLO_NARG
    virtual tstring &replace( size_t pos1, size_t n1, size_t n2 );
#endif

    /* erase a part of string (position = pos, length = n) */
    virtual tstring &erase( size_t pos, size_t n = 1 );
    virtual tstring &erase();

    /* cutout string of position = pos, length = n */
    /* posʸ���ܤ���nʸ����ʸ����ˤ���            */
    virtual tstring &crop( size_t pos, size_t n );
    virtual tstring &crop( size_t pos );

    /* simple replacement:  search org_str and replace it with new_str. */
    /*   pos: starting position of replacement                          */
    /*   org_str: string to be searched                                 */
    /*   new_str: string to be replaced with                            */
    /*   all: set true when performing successive replacement           */
    virtual ssize_t strreplace( const char *org_str, 
				const char *new_str, bool all = false );
    virtual ssize_t strreplace( size_t pos, const char *org_str, 
				const char *new_str, bool all = false );
    virtual ssize_t strreplace( const tstring &org_str, 
				const char *new_str, bool all = false );
    virtual ssize_t strreplace( size_t pos, const tstring &org_str, 
				const char *new_str, bool all = false );
    virtual ssize_t strreplace( const char *org_str, 
				const tstring &new_str, bool all = false );
    virtual ssize_t strreplace( size_t pos, const char *org_str, 
				const tstring &new_str, bool all = false );
    virtual ssize_t strreplace( const tstring &org_str, 
				const tstring &new_str, bool all = false );
    virtual ssize_t strreplace( size_t pos, const tstring &org_str, 
				const tstring &new_str, bool all = false );

    /*
     * New member functions of 1.3.0
     */
    virtual ssize_t erase_quotes( const char *quot_bkt, int escape,
				  bool rm_escape, size_t *new_quoted_spn,
				  bool all = false );
    virtual ssize_t erase_quotes( size_t pos, const char *quot_bkt, int escape,
				  bool rm_escape, size_t *new_quoted_spn,
				  bool all = false );

    /* same as chop(), chomp() of Perl */
    virtual tstring &chop();
    virtual tstring &chomp( const char *rs = "\n" );
    virtual tstring &chomp( const tstring &rs );

    /* erase left-side and/or right side of blank characters. */
    /* �����ζ���ʸ����ä�                                   */
    /* side_spaces ������ɽ���� "[^A-Z]" �Ȥ���ɽ���Ǥ� OK    */
    /* trim(): both left and right */
    virtual tstring &trim( const char *side_spaces = " \t\n\r\f\v" );
    virtual tstring &trim( const tstring &side_spaces );
    virtual tstring &trim( int side_space );
    /* ltrim(): left only  */
    virtual tstring &ltrim( const char *side_spaces = " \t\n\r\f\v" );
    virtual tstring &ltrim( const tstring &side_spaces );
    virtual tstring &ltrim( int side_space );
    /* rtrim(): right only */
    virtual tstring &rtrim( const char *side_spaces = " \t\n\r\f\v" );
    virtual tstring &rtrim( const tstring &side_spaces );
    virtual tstring &rtrim( int side_space );

    /* same as trim() */
    virtual tstring &strtrim( const char *side_spaces = " \t\n\r\f\v" );
    virtual tstring &strtrim( const tstring &side_spaces );
    virtual tstring &strtrim( int side_space );

    /* convert string to lowercase    */
    /*   pos:  starting position      */
    /*   n:    length to be converted */
    virtual tstring &tolower( size_t pos = 0 );
    virtual tstring &tolower( size_t pos, size_t n );

    /* convert string to uppercase    */
    /*   pos:  starting position      */
    /*   n:    length to be converted */
    virtual tstring &toupper( size_t pos = 0 );
    virtual tstring &toupper( size_t pos, size_t n );

    /* replacement of tab or space characters */
    /* TAB ʸ����Ÿ����TAB ʸ���ؤμ���       */
    virtual tstring &expand_tabs( size_t tab_width = 8 );
    virtual tstring &contract_spaces( size_t tab_width = 8 );

    /* swap contents between this object and sobj */
    virtual tstring &swap( tstring &sobj );

    /* join array elements with a string, and store it. */
    /* (same as implode function of PHP)                */
    /*   arr:   pointer array of strings                */
    /*   delim: delimiter string                        */
    /* see also tarray_tstring::explode().              */
    virtual tstring &implode( const char *const *arr, const char *delim );

    /* import binary data as a string.                                       */
    /*   buf:     pointer of buffer                                          */
    /*   bufsize: byte length of data                                        */
    /*   altchr:  character to be replaced with when '\0' is found in buffer */
    virtual tstring &import_binary( const char *buf, size_t bufsize,
				    int altchr = '\0' );

    /* scan internal buffer, and update internal length info.                */
    /* use this member function when user's program directly update internal */
    /* buffer of fixed-length buffer mode.                                   */
    /* ����Ĺ�Хåե��⡼�ɻ��ˡ��Хåե��˻����񤭤������˻��Ѥ���        */
    virtual tstring &update_length();

    /* copy internal string into user's buffer          */
    /*   pos:      starting position of internal string */
    /*   dest_str: pointer of user's buffer             */
    /*   bufsize:  byte length to be copied             */
    /* dest_str��posʸ���ܤ��饳�ԡ�                    */
    virtual ssize_t getstr( size_t pos, char *dest_str, size_t bufsize ) const;
    virtual ssize_t getstr( char *dest_str, size_t bufsize ) const;

    /* copy internal string into another tstring object */
    /*   pos:  starting position of internal string     */
    /*   n:    length to be copied                      */
    /*   dest: destination object                       */
    /* dest���֥������Ȥˡ����� pos ���� n ʸ�����ԡ�   */
    virtual ssize_t copy( size_t pos, size_t n, tstring *dest ) const;
    virtual ssize_t copy( size_t pos, tstring *dest ) const;
    virtual ssize_t copy( tstring *dest ) const;
    /* not recommended */
    virtual ssize_t copy( size_t pos, size_t n, tstring &dest ) const;
    /* not recommended */
    virtual ssize_t copy( size_t pos, tstring &dest ) const;
    /* not recommended */
    virtual ssize_t copy( tstring &dest ) const;

    /* move all contents into another object. */
    /* a string of this object becomes empty. */
    virtual tstring &cut( tstring *dest );

    /* these member functions return address of internal string with '\0' */
    /* termination.                                                       */
    /* same as c_str() of std::string.                                    */
    /* '\0'�ǽ����ʸ�������ΤؤΥݥ��󥿤��֤�                           */
    virtual const char *c_str() const;			/* same as cstr() */
    virtual const char *cstr() const;
    virtual char *str_ptr();
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const char *str_ptr() const;		/* same as cstr() */
#endif
    virtual const char *str_ptr_cs() const;		/* same as cstr() */

    /* this returns character at position = pos of internal string. */
    /* '\0' can be returned.                                        */
    /* pos���ܤ�ʸ�����֤�(��ü��0��ޤ�)                           */
    virtual int cchr( size_t pos ) const;

    /* read or write character at position = pos of internal string.      */
    /* length of internal buffer is automatically updated.                */
    /* pos���ܤ�ʸ�����ɤ߽񤭡�pos�˱����ƥХåե��Υ������ϼ�ư�ꥵ���� */
    virtual unsigned char &at( size_t pos );
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const unsigned char &at( size_t pos ) const;
#endif
    /* �ɤ߹�������(�ꥵ�����Ϥ��ʤ�)�����̤ϻȤ�ʤ� */
    virtual const unsigned char &at_cs( size_t pos ) const;

    /* returns length of string being stored in object */
    virtual size_t length() const;
    /* not recommended */
    virtual size_t size() const;			/* same as length() */

    /* maximum length of string that can be stored in object when */
    /* fixed-length buffer mode                                   */
    /* ����Ĺ�Хåե��ξ���ʸ����κ����Ĺ�����֤�             */
    virtual size_t max_length() const;

    /* convert string to numeric                         */
    /*   pos:    starting position of internal string    */
    /*   n:      length to be used                       */
    /*   endpos: (*endpos == used length) means no error */
    /* see also manual of LIBC.                          */
    virtual int atoi( size_t pos = 0 ) const;
    virtual int atoi( size_t pos, size_t n ) const;
    virtual long atol( size_t pos = 0 ) const;
    virtual long atol( size_t pos, size_t n ) const;
    virtual long long atoll( size_t pos = 0 ) const;
    virtual long long atoll( size_t pos, size_t n ) const;
    virtual double atof( size_t pos = 0 ) const;
    virtual double atof( size_t pos, size_t n ) const;
    virtual long strtol( int base, size_t *endpos ) const;
    virtual long strtol( size_t pos, int base, size_t *endpos ) const;
    virtual long strtol(size_t pos, size_t n, int base, size_t *endpos) const;
    virtual long long strtoll( int base, size_t *endpos ) const;
    virtual long long strtoll( size_t pos, int base, size_t *endpos ) const;
    virtual long long strtoll( size_t pos, size_t n, 
			       int base, size_t *endpos ) const;
    virtual unsigned long strtoul( int base, size_t *endpos ) const;
    virtual unsigned long strtoul(size_t pos, int base, size_t *endpos) const;
    virtual unsigned long strtoul( size_t pos, size_t n,
				   int base, size_t *endpos ) const;
    virtual unsigned long long strtoull( int base, size_t *endpos ) const;
    virtual unsigned long long strtoull( size_t pos, 
					 int base, size_t *endpos ) const;
    virtual unsigned long long strtoull( size_t pos, size_t n,
					 int base, size_t *endpos ) const;
    virtual double strtod( size_t *endpos ) const;
    virtual double strtod( size_t pos, size_t *endpos ) const;
    virtual double strtod( size_t pos, size_t n, size_t *endpos ) const;

    /* same as scanf() in LIBC */
    virtual int scanf( const char *format, ... ) const;
    virtual int vscanf( const char *format, va_list ap ) const;

    /* compare between internal string and string of argument. */
    /* return 0 for equal.                                     */
    /*   pos:  starting position of internal string            */
    /*   str:  string to be compared                           */
    /*   n:    length for comparison                           */
    /*   pos2: starting position of str                        */
    /* see also manual of strcmp() of LIBC.                    */
    virtual int compare( size_t pos, const char *str, size_t n ) const;
    virtual int compare( size_t pos, const char *str ) const;
    virtual int compare( const char *str, size_t n ) const;
    virtual int compare( const char *str ) const;
    virtual int compare( size_t pos1, 
			 const tstring &str, size_t pos2 = 0 ) const;
    virtual int compare( size_t pos1, 
			 const tstring &str, size_t pos2, size_t n ) const;
    virtual int compare( const tstring &str, size_t pos2 = 0 ) const;
    virtual int compare( const tstring &str, size_t pos2, size_t n ) const;
    /* same as compare() */
    virtual int strncmp( size_t pos, const char *str, size_t n ) const;
    virtual int strcmp( size_t pos, const char *str ) const;
    virtual int strncmp( const char *str, size_t n ) const;
    virtual int strcmp( const char *str ) const;
    virtual int strcmp( size_t pos1, 
			const tstring &str, size_t pos2 = 0 ) const;
    virtual int strncmp( size_t pos1, 
			 const tstring &str, size_t pos2, size_t n ) const;
    virtual int strcmp( const tstring &str, size_t pos2 = 0 ) const;
    virtual int strncmp( const tstring &str, size_t pos2, size_t n ) const;

    /* case-insensitive version of strcmp() and strncmp(). */
    /* ��ʸ������ʸ������̤��ʤ������                    */
    virtual int strncasecmp( size_t pos, const char *str, size_t n ) const;
    virtual int strcasecmp( size_t pos, const char *str ) const;
    virtual int strncasecmp( const char *str, size_t n ) const;
    virtual int strcasecmp( const char *str ) const;
    virtual int strcasecmp( size_t pos1, 
			    const tstring &str, size_t pos2 = 0 ) const;
    virtual int strncasecmp( size_t pos1, 
			     const tstring &str, size_t pos2, size_t n ) const;
    virtual int strcasecmp( const tstring &str, size_t pos2 = 0 ) const;
    virtual int strncasecmp( const tstring &str, size_t pos2, size_t n ) const;

    /* search string from left-side and return the position when it is    */
    /* found, otherwise return negative value.                            */
    /*   pos:     starting position of internal string                    */
    /*   str:     string to be searched for                               */
    /*   n:       used length of str                                      */
    /*   nextpos: position for next search is returned                    */
    /* posʸ���ܤ���nʸ����ʸ����str��õ��                                */
    virtual ssize_t strstr( const char *str ) const;	/* same as find() */
    virtual ssize_t strstr( size_t pos, const char *str ) const;
    virtual ssize_t strstr(size_t pos, const char *str, size_t *nextpos) const;
    virtual ssize_t strstr( const tstring &str ) const;
    virtual ssize_t strstr( size_t pos, const tstring &str ) const;
    virtual ssize_t strstr( size_t pos, const tstring &str,
			    size_t *nextpos ) const;
    /*                                                                    */
    virtual ssize_t find( const char *str ) const;
    virtual ssize_t find( const char *str, size_t n ) const;
    virtual ssize_t find( size_t pos, const char *str ) const;
    virtual ssize_t find( size_t pos, const char *str, size_t n ) const;
    virtual ssize_t find( size_t pos, const char *str, size_t *nextpos ) const;
    virtual ssize_t find( size_t pos, const char *str, size_t n, size_t *nextpos ) const;
    virtual ssize_t find( const tstring &str ) const;
    virtual ssize_t find( size_t pos, const tstring &str ) const;
    virtual ssize_t find( size_t pos, const tstring &str,
			  size_t *nextpos ) const;
    /* ���ޥ� */
#if 1
    virtual ssize_t findf( const char *format, ... ) const;
    virtual ssize_t vfindf( const char *format, va_list ap ) const;
    virtual ssize_t findf( size_t pos, const char *format, ... ) const;
    virtual ssize_t vfindf( size_t pos, const char *format, va_list ap ) const;
#endif

    /* search string from right-side and return the position when it is    */
    /* found, otherwise return negative value.                             */
    virtual ssize_t strrstr( const char *str ) const;	/* same as rfind() */
    virtual ssize_t strrstr( size_t pos, const char *str ) const;
    virtual ssize_t strrstr( size_t pos, const char *str,
			     size_t *nextpos) const;
    virtual ssize_t strrstr( const tstring &str ) const;
    virtual ssize_t strrstr( size_t pos, const tstring &str ) const;
    virtual ssize_t strrstr( size_t pos, const tstring &str,
			     size_t *nextpos ) const;
    /*                                                                     */
    virtual ssize_t rfind( const char *str ) const;
    virtual ssize_t rfind( const char *str, size_t n ) const;
    virtual ssize_t rfind( size_t pos, const char *str ) const;
    virtual ssize_t rfind( size_t pos, const char *str, size_t n ) const;
    virtual ssize_t rfind(size_t pos, const char *str, size_t *nextpos) const;
    virtual ssize_t rfind( size_t pos, const char *str, size_t n,
			   size_t *nextpos ) const;
    virtual ssize_t rfind( const tstring &str ) const;
    virtual ssize_t rfind( size_t pos, const tstring &str ) const;
    virtual ssize_t rfind( size_t pos, const tstring &str,
			   size_t *nextpos ) const;
    /* ���ޥ� */
#if 1
    virtual ssize_t rfindf( const char *format, ... ) const;
    virtual ssize_t vrfindf( const char *format, va_list ap ) const;
    virtual ssize_t rfindf( size_t pos, const char *format, ... ) const;
    virtual ssize_t vrfindf( size_t pos, const char *format, va_list ap) const;
#endif

    /* search a character from left-side and return the position when it is */
    /* found, otherwise return negative value.                              */
    /*   pos:     starting position of internal string                      */
    /*   ch:      character to be searched for                              */
    /*   nextpos: position for next search is returned                      */
    /* posʸ���ܤ���ʸ��ch��õ��                                            */
    virtual ssize_t strchr( int ch ) const;		/* same as find() */
    virtual ssize_t strchr( size_t pos, int ch ) const;
    virtual ssize_t strchr( size_t pos, int ch, size_t *nextpos ) const;
    virtual ssize_t find( int ch ) const;
    virtual ssize_t find( size_t pos, int ch ) const;
    virtual ssize_t find( size_t pos, int ch, size_t *nextpos ) const;

    /* search a character from right-side and return the position when it is */
    /* found, otherwise return negative value.                               */
    virtual ssize_t strrchr( int ch ) const;		/* same as rfind() */
    virtual ssize_t strrchr( size_t pos, int ch ) const;
    virtual ssize_t strrchr( size_t pos, int ch, size_t *nextpos ) const;
    virtual ssize_t rfind( int ch ) const;
    virtual ssize_t rfind( size_t pos, int ch ) const;
    virtual ssize_t rfind( size_t pos, int ch, size_t *nextpos ) const;

    /* search characters exists in specified character-set from left-side, */
    /* and return the first position when they are found, otherwise return */
    /* negative value.                                                     */
    /*   pos:     starting position of internal string                     */
    /*   str:     character-set to be searched for                         */
    /*   n:       used length of str                                       */
    /*   nextpos: position for next search is returned                     */
    /* these member functions are almost the same to strpbrk() of LIBC.    */
    /* posʸ���ܤ���nʸ����ʸ����str�˴ޤޤ�뤤���줫��ʸ����õ��         */
    virtual ssize_t find_first_of( const char *str ) const;
    virtual ssize_t find_first_of( const char *str, size_t n ) const;
    virtual ssize_t find_first_of( size_t pos, const char *str ) const;
    virtual ssize_t find_first_of(size_t pos, const char *str, size_t n) const;
    virtual ssize_t find_first_of( size_t pos, const char *str,
				   size_t *nextpos ) const;
    virtual ssize_t find_first_of( size_t pos, const char *str, size_t n,
				   size_t *nextpos ) const;
    virtual ssize_t find_first_of( const tstring &str ) const;
    virtual ssize_t find_first_of( size_t pos, const tstring &str ) const;
    virtual ssize_t find_first_of( size_t pos, const tstring &str,
				   size_t *nextpos ) const;

    /* search characters exists in specified character-set from right-side, */
    /* and return the first position when they are found, otherwise return  */
    /* negative value.                                                      */
    virtual ssize_t find_last_of( const char *str ) const;
    virtual ssize_t find_last_of( const char *str, size_t n ) const;
    virtual ssize_t find_last_of( size_t pos, const char *str ) const;
    virtual ssize_t find_last_of( size_t pos, const char *str, size_t n) const;
    virtual ssize_t find_last_of( size_t pos, const char *str,
				  size_t *nextpos ) const;
    virtual ssize_t find_last_of( size_t pos, const char *str, size_t n,
				  size_t *nextpos ) const;
    virtual ssize_t find_last_of( const tstring &str ) const;
    virtual ssize_t find_last_of( size_t pos, const tstring &str ) const;
    virtual ssize_t find_last_of( size_t pos, const tstring &str,
				  size_t *nextpos ) const;

    /* same as find() or rfind() */
    virtual ssize_t find_first_of( int ch ) const;	/* same as find()    */
    virtual ssize_t find_last_of( int ch ) const;	/* same as rfind()   */
    virtual ssize_t find_first_of( size_t pos, int ch ) const;	/* ==find()  */
    virtual ssize_t find_last_of( size_t pos, int ch ) const;	/* ==rfind() */
    virtual ssize_t find_first_of( size_t pos, int ch,
				   size_t *nextpos ) const;	/* ==find()  */
    virtual ssize_t find_last_of( size_t pos, int ch,
				  size_t *nextpos ) const;	/* ==rfind() */

    /* search characters not exists in specified character-set from  */
    /* left-side, and return the first position when they are found, */
    /* otherwise return negative value.                              */
    /*   pos:     starting position of internal string               */
    /*   str:     character-set to be rejected                       */
    /*   n:       used length of str                                 */
    /*   nextpos: position for next search is returned               */
    /* posʸ���ܤ���nʸ����ʸ����str�˴ޤޤ�ʤ�ʸ����õ��           */
    virtual ssize_t find_first_not_of( const char *str ) const;
    virtual ssize_t find_first_not_of( const char *str, size_t n ) const;
    virtual ssize_t find_first_not_of( size_t pos, const char *str ) const;
    virtual ssize_t find_first_not_of( size_t pos, 
				       const char *str, size_t n ) const;
    virtual ssize_t find_first_not_of( size_t pos, const char *str,
				       size_t *nextpos ) const;
    virtual ssize_t find_first_not_of( size_t pos, const char *str, size_t n,
				       size_t *nextpos ) const;
    virtual ssize_t find_first_not_of( const tstring &str ) const;
    virtual ssize_t find_first_not_of( size_t pos, const tstring &str ) const;
    virtual ssize_t find_first_not_of( size_t pos, const tstring &str,
				       size_t *nextpos ) const;

    /* search characters not exists in specified character-set from   */
    /* right-side, and return the first position when they are found, */
    /* otherwise return negative value.                               */
    virtual ssize_t find_last_not_of( const char *str ) const;
    virtual ssize_t find_last_not_of( const char *str, size_t n ) const;
    virtual ssize_t find_last_not_of( size_t pos, const char *str ) const;
    virtual ssize_t find_last_not_of( size_t pos, 
				      const char *str, size_t n ) const;
    virtual ssize_t find_last_not_of( size_t pos, const char *str,
				      size_t *nextpos ) const;
    virtual ssize_t find_last_not_of( size_t pos, const char *str, size_t n,
				      size_t *nextpos ) const;
    virtual ssize_t find_last_not_of( const tstring &str ) const;
    virtual ssize_t find_last_not_of( size_t pos, const tstring &str ) const;
    virtual ssize_t find_last_not_of( size_t pos, const tstring &str,
				      size_t *nextpos ) const;

    /* search a character that is not equal to ch from left-side, and  */
    /* return the first position when it is found, otherwise return    */
    /* negative value.                                                 */
    /*   pos:     starting position of internal string                 */
    /*   ch:      character to be rejected                             */
    /*   nextpos: position for next search is returned                 */
    /* posʸ���ܤ���ch�ǤϤʤ�ʸ����õ��                               */
    virtual ssize_t find_first_not_of( int ch ) const;
    virtual ssize_t find_first_not_of( size_t pos, int ch ) const;
    virtual ssize_t find_first_not_of( size_t pos, int ch,
				       size_t *nextpos ) const;

    /* search a character that is not equal to ch from right-side, and  */
    /* return the first position when it is found, otherwise return     */
    /* negative value.                                                  */
    virtual ssize_t find_last_not_of( int ch ) const;
    virtual ssize_t find_last_not_of( size_t pos, int ch ) const;
    virtual ssize_t find_last_not_of( size_t pos, int ch,
				      size_t *nextpos ) const;

    /*
     * New member functions of 1.3.0
     */
    virtual ssize_t find_quoted( const char *quot_bkt, int escape,
		    size_t *quoted_span = NULL, size_t *nextpos = NULL ) const;
    virtual ssize_t find_quoted( size_t pos, const char *quot_bkt, int escape,
		    size_t *quoted_span = NULL, size_t *nextpos = NULL ) const;
    virtual ssize_t rfind_quoted( const char *quot_bkt, int escape,
		    size_t *quoted_span = NULL, size_t *nextpos = NULL ) const;
    virtual ssize_t rfind_quoted( size_t pos, const char *quot_bkt, int escape,
		    size_t *quoted_span = NULL, size_t *nextpos = NULL ) const;

    /* character classification.                  */
    /*   pos:  position of character to be tested */
    /* see manual of LIBC.                        */
    /* pos���ܤ�ʸ���μ����Ƚ�ꤹ��              */
    virtual bool isalnum( size_t pos ) const;
    virtual bool isalpha( size_t pos ) const;
    virtual bool iscntrl( size_t pos ) const;
    virtual bool isdigit( size_t pos ) const;
    virtual bool isgraph( size_t pos ) const;
    virtual bool islower( size_t pos ) const;
    virtual bool isprint( size_t pos ) const;
    virtual bool ispunct( size_t pos ) const;
    virtual bool isspace( size_t pos ) const;
    virtual bool isupper( size_t pos ) const;
    virtual bool isxdigit( size_t pos ) const;

    /* search characters exists in character-set `accept' from left-side,  */
    /* and return the first position when they are found, otherwise return */
    /* negative value.                                                     */
    /* regular-expression style such as "[A-Z]" or "[^A-Z]" can be set to  */
    /* argument `accept'. (mixed style like "[A-Z]123" cannot be used)     */
    /*   pos:     starting position of internal string                     */
    /*   accept:  character-set to be searched for                         */
    /*   n:       used length of str                                       */
    /*   nextpos: position for next search is returned                     */
    /* posʸ���ܤ���ʸ����accept�˴ޤޤ�뤤���줫��ʸ����õ��             */
    /* accept ������ɽ���� "[A-Z]" �Ȥ���ɽ���Ǥ� OK��                     */
    /* "[^A-Z]" �ޤ��� "[!A-Z]" �Ǵޤޤ�ʤ�ʸ����õ���褦�ˤʤ�           */
    virtual ssize_t strpbrk( const char *accept ) const;
    virtual ssize_t strpbrk( size_t pos, const char *accept ) const;
    virtual ssize_t strpbrk( size_t pos, const char *accept,
			     size_t *nextpos ) const;
    virtual ssize_t strpbrk( const tstring &accept ) const;
    virtual ssize_t strpbrk( size_t pos, const tstring &accept ) const;
    virtual ssize_t strpbrk( size_t pos, const tstring &accept,
			     size_t *nextpos ) const;

    /* search characters exists in character-set `accept' from right-side,  */
    /* and return the first position when they are found, otherwise return  */
    /* negative value.                                                      */
    /* regular-expression style such as "[A-Z]" or "[^A-Z]" can be set to   */
    /* argument `accept'. (mixed style like "[A-Z]123" cannot be used)      */
    virtual ssize_t strrpbrk( const char *accept ) const;
    virtual ssize_t strrpbrk( size_t pos, const char *accept ) const;
    virtual ssize_t strrpbrk( size_t pos, const char *accept,
			      size_t *nextpos ) const;
    virtual ssize_t strrpbrk( const tstring &accept ) const;
    virtual ssize_t strrpbrk( size_t pos, const tstring &accept ) const;
    virtual ssize_t strrpbrk( size_t pos, const tstring &accept,
			      size_t *nextpos ) const;

    /* return the length of string that consists of specified character-set */
    /* `accept' at the position specified by `pos'.                         */
    /* regular-expression style such as "[A-Z]" or "[^A-Z]" can be set to   */
    /* argument `accept'. (mixed style like "[A-Z]123" cannot be used)      */
    /*   pos:     starting position of internal string                      */
    /*   accept:  character-set to be accepted                              */
    /*   nextpos: position for next search is returned                      */
    /* strspn() member functions scan string from left, strrspn() scan from */
    /* right.                                                               */
    /* ���ꤷ������ pos ��ʸ�����å�(accept)������С�����Ĺ�����֤�        */
    /* (�ʤ�����0)��accept �� "[A-Z]" �Ȥ���ɽ���Ǥ� OK��                 */
    virtual size_t strspn( const char *accept ) const;
    virtual size_t strspn( size_t pos, const char *accept ) const;
    virtual size_t strspn( size_t pos, const char *accept,
			   size_t *nextpos ) const;
    virtual size_t strspn( const tstring &accept ) const;
    virtual size_t strspn( size_t pos, const tstring &accept ) const;
    virtual size_t strspn( size_t pos, const tstring &accept,
			   size_t *nextpos ) const;
    virtual size_t strspn( int accept ) const;
    virtual size_t strspn( size_t pos, int accept ) const;
    virtual size_t strspn( size_t pos, int accept, size_t *nextpos ) const;
    /*                                                                      */
    virtual size_t strrspn( const char *accept ) const;
    virtual size_t strrspn( size_t pos, const char *accept ) const;
    virtual size_t strrspn( size_t pos, const char *accept,
			    size_t *nextpos ) const;
    virtual size_t strrspn( const tstring &accept ) const;
    virtual size_t strrspn( size_t pos, const tstring &accept ) const;
    virtual size_t strrspn( size_t pos, const tstring &accept,
			    size_t *nextpos ) const;
    virtual size_t strrspn( int accept ) const;
    virtual size_t strrspn( size_t pos, int accept ) const;
    virtual size_t strrspn( size_t pos, int accept, size_t *nextpos ) const;

    /* return the length of string that does not consist of specified     */
    /* character-set `reject' at the position specified by `pos'.         */
    /* regular-expression style such as "[A-Z]" or "[^A-Z]" can be set to */
    /* argument `reject'. (mixed style like "[A-Z]123" cannot be used)    */
    virtual size_t strcspn( const char *reject ) const;
    virtual size_t strcspn( size_t pos, const char *reject ) const;
    virtual size_t strcspn( size_t pos, const char *reject,
			    size_t *nextpos ) const;
    virtual size_t strcspn( const tstring &reject ) const;
    virtual size_t strcspn( size_t pos, const tstring &reject ) const;
    virtual size_t strcspn( size_t pos, const tstring &reject,
			    size_t *nextpos ) const;
    virtual size_t strcspn( int reject ) const;
    virtual size_t strcspn( size_t pos, int reject ) const;
    virtual size_t strcspn( size_t pos, int reject, size_t *nextpos ) const;

    /* match general string, filename, or pathname.                         */
    /* these member functions return 0 when internal string matches pattern */
    /* specified by `pat', otherwise return negative value.                 */
    /* see also fnmatch() of LIBC.                                          */
    /* ������饤���ʥѥ�����ޥå� strmatch(), fnmatch(), pnmatch()        */
    /* �ޥå����Ƥ���� 0���ޥå����ʤ���������ͤ��֤�                   */
    /* these member functions use fnmatch() of LIBC with no flags.          */
    virtual int strmatch( const char *pat ) const;
    virtual int strmatch( size_t pos, const char *pat ) const;
    virtual int strmatch( const tstring &pat ) const;
    virtual int strmatch( size_t pos, const tstring &pat ) const;
    /* these member functions use fnmatch() of LIBC with FNM_PERIOD flags.  */
    /* ��Ƭ�� . �����̰�������(�ե�����̾����)                              */
    virtual int fnmatch( const char *pat ) const;
    virtual int fnmatch( size_t pos, const char *pat ) const;
    virtual int fnmatch( const tstring &pat ) const;
    virtual int fnmatch( size_t pos, const tstring &pat ) const;
    /* these member functions use fnmatch() of LIBC with                    */
    /* FNM_PATHNAME | FNM_PERIOD.                                           */
    /* / �� . �����̰�������(�ѥ�̾����)                                    */
    virtual int pnmatch( const char *pat ) const;
    virtual int pnmatch( size_t pos, const char *pat ) const;
    virtual int pnmatch( const tstring &pat ) const;
    virtual int pnmatch( size_t pos, const tstring &pat ) const;

    /* match based on POSIX extended regular expression pattern.     */
    /*   pos:      starting position of internal string              */
    /*   pat:      regular expression pattern                        */
    /*   ret_span: length of string that matches pattern is returned */
    /*   nextpos:  position for next search is returned              */
    /* these member functions return the position of matched string  */
    /* when internal string matches the pattern, otherwise return    */
    /* negative value.                                               */
    /* see also regexec() of LIBC.                                   */
    /* POSIX��ĥ����ɽ���ǤΥѥ�����ޥå� regmatch()                */
    /* �ޥå�����ʸ�����õ�����ޥå�����ʸ����ΰ���(�֤���)��Ĺ��  */
    /* (*ret_span)���֤����ޥå����ʤ���������ͤ��֤�             */
    virtual ssize_t regmatch( const char *pat, size_t *ret_span ) const;
    virtual ssize_t regmatch( size_t pos, const char *pat,
			      size_t *ret_span ) const;
    virtual ssize_t regmatch( size_t pos, const char *pat,
			      size_t *ret_span, size_t *nextpos ) const;
    virtual ssize_t regmatch( const tstring &pat, size_t *ret_span ) const;
    virtual ssize_t regmatch( size_t pos, const tstring &pat,
			      size_t *ret_span ) const;
    virtual ssize_t regmatch( size_t pos, const tstring &pat,
			      size_t *ret_span, size_t *nextpos ) const;
    virtual ssize_t regmatch( const tregex &pat, size_t *ret_span ) const;
    virtual ssize_t regmatch( size_t pos, const tregex &pat,
			      size_t *ret_span ) const;
    virtual ssize_t regmatch( size_t pos, const tregex &pat,
			      size_t *ret_span, size_t *nextpos ) const;

    /* Not recommended.                                                 */
    /* New programs should apply tarray_tstring::regassign().           */
    /* ��侩: POSIX��ĥ����ɽ���ǤΥѥ�����ޥå�(Advanced�Ǥ���1)     */
    /* �������ץ����ϡ�tarray_tstring::regassign() ��ȤäƤ������� */
    virtual ssize_t regmatch( const char *pat, 
			      size_t max_nelem, size_t pos_r[], size_t len_r[],
			      size_t *nelem_r ) const;
    virtual ssize_t regmatch( size_t pos, const char *pat,
			      size_t max_nelem, size_t pos_r[], size_t len_r[],
			      size_t *nelem_r ) const;
    virtual ssize_t regmatch( size_t pos, const char *pat,
			      size_t max_nelem, size_t pos_r[], size_t len_r[],
			      size_t *nelem_r, size_t *nextpos ) const;
    virtual ssize_t regmatch( const tstring &pat,
			      size_t max_nelem, size_t pos_r[], size_t len_r[],
			      size_t *nelem_r ) const;
    virtual ssize_t regmatch( size_t pos, const tstring &pat,
			      size_t max_nelem, size_t pos_r[], size_t len_r[],
			      size_t *nelem_r ) const;
    virtual ssize_t regmatch( size_t pos, const tstring &pat,
			      size_t max_nelem, size_t pos_r[], size_t len_r[],
			      size_t *nelem_r, size_t *nextpos ) const;
    virtual ssize_t regmatch( const tregex &pat,
			      size_t max_nelem, size_t pos_r[], size_t len_r[],
			      size_t *nelem_r ) const;
    virtual ssize_t regmatch( size_t pos, const tregex &pat,
			      size_t max_nelem, size_t pos_r[], size_t len_r[],
			      size_t *nelem_r ) const;
    virtual ssize_t regmatch( size_t pos, const tregex &pat,
			      size_t max_nelem, size_t pos_r[], size_t len_r[],
			      size_t *nelem_r, size_t *nextpos ) const;

    /* Not recommended.                                                 */
    /* New programs should apply tarray_tstring::regassign().           */
    /* ��侩: POSIX��ĥ����ɽ���ǤΥѥ�����ޥå�(Advanced�Ǥ���2)     */
    /* �������ץ����ϡ�tarray_tstring::regassign() ��ȤäƤ������� */
    virtual ssize_t regmatch( const char *pat );
    virtual ssize_t regmatch( size_t pos, const char *pat );
    virtual ssize_t regmatch( const tstring &pat );
    virtual ssize_t regmatch( size_t pos, const tstring &pat );
    virtual ssize_t regmatch( const tregex &pat );
    virtual ssize_t regmatch( size_t pos, const tregex &pat );
    /*                                                                  */
    virtual size_t reg_elem_length() const;
    virtual size_t reg_pos( size_t idx ) const;
    virtual size_t reg_length( size_t idx ) const;
    virtual const char *reg_cstr( size_t idx ) const;
    virtual const char *const *reg_cstrarray() const;

    /* match and replace based on POSIX extended regular expression pattern. */
    /*   pat:      regular expression pattern                                */
    /*   new_str:  string to be replaced with                                */
    /*   all:      set true when performing successive replacement           */
    /* POSIX����ɽ���ˤ���ִ� */
    virtual ssize_t regreplace( const char *pat, 
				const char *new_str, bool all = false );
    virtual ssize_t regreplace( size_t pos, const char *pat, 
				const char *new_str, bool all = false );
    virtual ssize_t regreplace( const tstring &pat, 
				const char *new_str, bool all = false );
    virtual ssize_t regreplace( size_t pos, const tstring &pat, 
				const char *new_str, bool all = false );
    virtual ssize_t regreplace( const tregex &pat, 
				const char *new_str, bool all = false );
    virtual ssize_t regreplace( size_t pos, const tregex &pat,
				const char *new_str, bool all = false );
    /*                                                                       */
    virtual ssize_t regreplace( const char *pat, 
				const tstring &new_str, bool all = false );
    virtual ssize_t regreplace( size_t pos, const char *pat, 
				const tstring &new_str, bool all = false );
    virtual ssize_t regreplace( const tstring &pat, 
				const tstring &new_str, bool all = false );
    virtual ssize_t regreplace( size_t pos, const tstring &pat, 
				const tstring &new_str, bool all = false );
    virtual ssize_t regreplace( const tregex &pat, 
				const tstring &new_str, bool all = false );
    virtual ssize_t regreplace( size_t pos, const tregex &pat,
				const tstring &new_str, bool all = false );

    /* register the address of an external pointer variable that is    */
    /* automatically updated when changed address of buffer in object. */
    /* (for tarray_tstring class)                                      */
    virtual tstring &register_extptr(char **extptr_address);
    virtual tstring &register_extptr(const char **extptr_address);

    /* ������֥������Ȥ� return ��ľ���˻Ȥ���shallow copy ����Ĥ������ */
    /* �Ȥ���                                                                */
    virtual void set_scopy_flag();

    /* display internal information of object. */
    /* (for user's debug)                      */
    /* �桼���� debug ��                       */
    virtual void dprint( const char *msg = NULL ) const;

  private:
    /* constructor �Ѥ����̤ʥ��˥���饤�� */
    void __force_init( bool is_constructor );
    /* destructor �Ѥ����̤ʥ��곫���ѥ��дؿ� */
    void __force_free();
    /* shallow copy �ط� */
    bool request_shallow_copy( tstring *from_obj ) const;
    void cancel_shallow_copy( tstring *from_obj ) const;
    void cleanup_shallow_copy( bool do_deep_copy_for_this ) const;
    /* ���̤ʥ��˥���饤�� */
    tstring &__shallow_init( const tstring &obj, bool is_move );
    tstring &__deep_init( const tstring &obj );
    void __init_non_null_str_rec();
    void __init_fixed_str_rec( size_t max_length );
    /* realloc and free for this->_str_rec */
    int realloc_str_rec( size_t len_bytes );
    void free_str_rec();
    /* for reg_pos_rec, etc. */
    int realloc_reg_pos_rec( size_t len_elements );
    void free_reg_pos_rec();
    int realloc_reg_length_rec( size_t len_elements );
    void free_reg_length_rec();
    int realloc_reg_cstr_ptr_rec( size_t len_elements );
    void free_reg_cstr_ptr_rec();
    int realloc_reg_cstrbuf_rec( size_t len_bytes );
    void free_reg_cstrbuf_rec();
    /* */
    bool is_my_buffer( const char *ptr ) const;
    ssize_t str_match( size_t pos, const char *pat, size_t *ret_span ) const;
    ssize_t regexp_match( size_t pos, 
			  const tregex &regex_ref, size_t *ret_span ) const;
    ssize_t regexp_match_advanced1( size_t pos, const tregex &regex_ref,
				    size_t nelem,
				    size_t pos_r[], size_t len_r[],
				    size_t *nelem_r ) const;
    ssize_t regexp_match_advanced2( size_t pos, const tregex &regex_ref );
    void init_reg_results();
    ssize_t regexp_replace( size_t pos, const tregex &pat,
			    const char *new_str, bool all );
    /* at(), at_cs() ���ѥ��дؿ� */
    unsigned char &uchar_ref_after_resize( size_t pos );
    unsigned char &err_throw_uchar_ref( const char *func, 
					const char *lv, const char *mes );
    const unsigned char &err_throw_const_uchar_ref( const char *func, 
				       const char *lv, const char *mes ) const;

  private:
    /* ʸ����Хåե� */
    char *_str_rec;
    /* _str_rec �Ѥ˳��ݤ��Ƥ���Хåե��ΥХ���Ĺ */
    size_t str_alloc_blen_rec;
    /* �Ǹ���׵ᤵ�줿 _str_rec �ΥХåե�Ĺ                               */
    /* (����Ĺ�Хåե�mode�Ǥ�ʸ����Ĺ�ξ��+1��¾mode�Ǥ�ʸ����Ĺ+1�򼨤�) */
    size_t str_buf_length_rec;
    /* ����Ĺ�Хåե�mode�ξ���ʸ����Ĺ��¾mode�Ǥ� UNDEF ������ */
    size_t str_length_rec;
    /* NULL̵���⡼�ɤǤ� true */
    bool str_rec_keeps_non_null;
    /* �����ݥ����ѿ��Υ��ɥ쥹 (̤���Ѥʤ�NULL) */
    char **extptr_rec;
    /* ����ɽ���ǻȤ� */
    tregex regex_rec;
    /* ����ɽ���η�̤��Ǽ����(��free) */
    size_t reg_elem_length_rec;
    size_t *_reg_pos_rec;
    size_t *_reg_length_rec;
    char **_reg_cstr_ptr_rec;
    size_t reg_cstrbuf_length_rec;
    char *_reg_cstrbuf_rec;

    /* �Хåե��ηѾ�(shallow copy)�Τ���Υե饰��= ���Ǥϥ��ԡ�����ʤ� */
    /* ���ԡ��� src ¦�Υե饰 */
    bool shallow_copy_ok;		/* set_scopy_flag() �ǥ��åȤ���� */
    tstring *shallow_copy_dest_obj;	/* ���ԡ���: ͭ���ʤ� non-NULL */

    /* ���ԡ��� dest ¦�Υե饰(���ԡ�������񤭹��ޤ��) */
    tstring *shallow_copy_src_obj;	/* ���ԡ���: �����Ƥ���� non-NULL */

    /* __shallow_init(), __deep_init() ������˹Ԥʤ�����Υե饰 */
    bool __copying;

#ifndef TSTRING__USE_SOLO_NARG
  private:
    /*
      ����ѥ�����˥��顼��Ф�����Ρ����ߡ��Υ��дؿ���

      ���̤ο��ͤ�Ϳ����ȡ����Ȥΰ�������ĥ��дؿ������Ф�Ƥ��ޤ���
      �㤨�С�hoge.append(' '); �Ȥ���ȡ�����ѥ������
      append( const tstring &src, size_t pos = 0 ) �����Ф�Ƥ��ޤ���
      �����ư��ʤ��������� private ���֤����ǡ�����������ɻߤ��롥
    */
    void assign( int ch );
    void put( size_t pos, int ch );
    void append( int ch );
    void insert( size_t pos1, int ch );
    void replace( size_t pos1, size_t n1, int ch );
#endif

  };

/*
 * inline member functions
 */

namespace _TSTRING_INTERNAL
{
    static const size_t _UNDEF = ~((size_t)0);
}

/**
 * @brief  [] �ǻ��ꤵ�줿���֤�ʸ���λ���(unsigned char��)���֤�
 *
 * @param      pos ʸ����ΰ���
 * @return     ���ꤵ�줿���֤ˤ���ʸ���λ���
 * @throw      ����Ĺ�Хåե��⡼�ɤ�pos �˺���ʸ����Ĺ�ʾ���ͤ���ꤷ�����
 * 
 */
inline unsigned char &tstring::operator[]( size_t pos )
{
    return this->at(pos);
}

/**
 * @brief  [] �ǻ��ꤵ�줿���֤�ʸ���λ���(unsigned char��)���֤� (�ɼ�����)
 *
 * @param      pos ʸ����ΰ���
 * @return     ���ꤵ�줿���֤ˤ���ʸ���λ���
 * @throw      pos ��ʸ����Ĺ�ʾ���ͤ���ꤷ�����
 * 
 */
inline const unsigned char &tstring::operator[]( size_t pos ) const
{
    return this->at_cs(pos);
}

/**
 * @brief  ʸ�������Ƭ���ɥ쥹����� (�ɼ�����)
 *
 * @return     ���Ȥ�����ʸ�������Ƭ���ɥ쥹
 *
 */
inline const char *tstring::c_str() const
{
    if ( this->shallow_copy_dest_obj != NULL ||
	 this->shallow_copy_src_obj != NULL ) this->cleanup_shallow_copy(true);
    return this->_str_rec;
}

/**
 * @brief  ʸ�������Ƭ���ɥ쥹����� (�ɼ�����)
 *
 * @return     ���Ȥ�����ʸ�������Ƭ���ɥ쥹
 *
 */
inline const char *tstring::cstr() const
{
    if ( this->shallow_copy_dest_obj != NULL ||
	 this->shallow_copy_src_obj != NULL ) this->cleanup_shallow_copy(true);
    return this->_str_rec;
}

/**
 * @brief  ʸ�������Ƭ���ɥ쥹�����
 *
 * @return     ���Ȥ�����ʸ�������Ƭ���ɥ쥹
 * @note       ���򤨤ʤ����ʳ��ϡ����Υ��дؿ���Ȥ�ʤ��Ǥ���������
 *
 */
inline char *tstring::str_ptr()
{
    if ( this->shallow_copy_dest_obj != NULL ||
	 this->shallow_copy_src_obj != NULL ) this->cleanup_shallow_copy(true);
    return this->_str_rec;
}

#ifdef SLI__OVERLOAD_CONST_AT
/**
 * @brief  ʸ�������Ƭ���ɥ쥹����� (�ɼ�����)
 *
 * @return     ���Ȥ�����ʸ�������Ƭ���ɥ쥹
 * @note       ���򤨤ʤ����ʳ��ϡ����Υ��дؿ���Ȥ�ʤ��Ǥ���������
 *
 */
inline const char *tstring::str_ptr() const
{
    if ( this->shallow_copy_dest_obj != NULL ||
	 this->shallow_copy_src_obj != NULL ) this->cleanup_shallow_copy(true);
    return this->_str_rec;
}
#endif

/**
 * @brief  ʸ�������Ƭ���ɥ쥹����� (�ɼ�����)
 *
 * @return     ���Ȥ�����ʸ�������Ƭ���ɥ쥹
 * @note       ���򤨤ʤ����ʳ��ϡ����Υ��дؿ���Ȥ�ʤ��Ǥ���������
 *
 */
inline const char *tstring::str_ptr_cs() const
{
    if ( this->shallow_copy_dest_obj != NULL ||
	 this->shallow_copy_src_obj != NULL ) this->cleanup_shallow_copy(true);
    return this->_str_rec;
}

/**
 * @brief  ���Ȥ�����ʸ����ΰ��� pos �ˤ���ʸ��(int��)���֤�
 *
 * @param      pos ʸ����ΰ���
 * @return     �����������ϻ��ꤵ�줿���֤ˤ���ʸ��<br>
 *             ���Ԥ�����������
 *
 */
inline int tstring::cchr( size_t pos ) const
{
    return (
	     ( 0 <= pos && pos <= this->length() ) ?
	     ((unsigned char *)this->_str_rec)[pos] : -1
	   );
}

/**
 * @brief  ���Ȥ�����ʸ����ΰ��� pos �ˤ���ʸ���λ���(unsigned char��)���֤�
 *
 * @param      pos ʸ����ΰ���
 * @return     ���ꤵ�줿���֤ˤ���ʸ���λ���
 * @throw      ����Ĺ�Хåե��⡼�ɤ�pos �˺���ʸ����Ĺ�ʾ���ͤ���ꤷ�����
 *
 */
inline unsigned char &tstring::at( size_t pos )
{
    if ( this->shallow_copy_dest_obj != NULL ||
	 this->shallow_copy_src_obj != NULL ) this->cleanup_shallow_copy(true);
    return ( (this->str_length_rec != _TSTRING_INTERNAL::_UNDEF) ?
	       /* ����Ĺ�Хåե��ξ�� */
	       ( (this->str_buf_length_rec <= pos + 1) ?
		 this->err_throw_uchar_ref("tstring::at()",
					   "ERROR","invalid pos argument")
		 :
	         ( (this->length() < pos + 1) ?
		   this->uchar_ref_after_resize(pos)
		   :
		   ((unsigned char *)this->_str_rec)[pos]
	         )
	       )
	       :
	       ( (this->length() < pos + 1) ?
		 this->uchar_ref_after_resize(pos)
		 :
		 ((unsigned char *)this->_str_rec)[pos]
	       )
	   );
}

#ifdef SLI__OVERLOAD_CONST_AT
/**
 * @brief  ʸ����ΰ��� pos �ˤ���ʸ���λ���(unsigned char��)���֤� (�ɼ�����)
 *
 * @param      pos ʸ����ΰ���
 * @return     ���ꤵ�줿���֤ˤ���ʸ���λ���
 * @throw      pos ��ʸ����Ĺ�ʾ���ͤ���ꤷ�����
 *
 */
inline const unsigned char &tstring::at( size_t pos ) const
{
    return this->at_cs(pos);
}
#endif

/**
 * @brief  ʸ����ΰ��� pos �ˤ���ʸ���λ���(unsigned char��)���֤� (�ɼ�����)
 *
 * @param      pos ʸ����ΰ���
 * @return     ���ꤵ�줿���֤ˤ���ʸ���λ���
 * @throw      pos ��ʸ����Ĺ�ʾ���ͤ���ꤷ�����
 *
 */
inline const unsigned char &tstring::at_cs( size_t pos ) const
{
    return ( (this->length() < pos) ? 
	     this->err_throw_const_uchar_ref("tstring::at_cs()",
					     "ERROR","invalid pos argument")
	     :
	     ((const unsigned char *)this->_str_rec)[pos]
	   );
}

/**
 * @brief  ʸ�����Ĺ��('\0'�ϴޤޤʤ�)�����
 *
 * @return  ���Ȥ�ʸ����Ĺ
 *
 */
inline size_t tstring::length() const
{
    return ( (this->str_length_rec != _TSTRING_INTERNAL::_UNDEF) ?
	this->str_length_rec	/* ����Ĺ�Хåե��ξ�� */
	:
	( (1 < this->str_buf_length_rec) ? (this->str_buf_length_rec - 1) : 0 )
      );
}

/**
 * @brief  ʸ�����Ĺ��('\0'�ϴޤޤʤ�)�����
 *
 * @return  ���Ȥ�ʸ����Ĺ
 */
inline size_t tstring::size() const
{
    return ( (this->str_length_rec != _TSTRING_INTERNAL::_UNDEF) ? 
	this->str_length_rec	/* ����Ĺ�Хåե��ξ�� */
	: 
	( (1 < this->str_buf_length_rec) ? (this->str_buf_length_rec - 1) : 0 )
      );
}

/**
 * @brief  ����Ĺ�Хåե��⡼�ɤǤκ����ʸ����Ĺ�����
 *
 * @return     �����ʸ����Ĺ
 */
inline size_t tstring::max_length() const
{
    return ( (this->str_length_rec != _TSTRING_INTERNAL::_UNDEF) ? 
	     (this->str_buf_length_rec - 1)	/* ����Ĺ�Хåե��ξ�� */
	     :
	     0
	   );
}


/**
 * @example  examples_sllib/string_basic.cc
 *           ʸ����ΰ������δ��ܤ򼨤���������
 */

/**
 * @example  examples_sllib/string_edit.cc
 *           ʸ������Խ�����򼨤���������
 */

/**
 * @example  examples_sllib/string_match.cc
 *           ʸ����Υ�����Ū�ʥޥå��󥰤���򼨤���������
 */

/**
 * @example  examples_sllib/string_regexp.cc
 *           ʸ������Ф���POSIX��ĥ����ɽ����ȤäƤߤ���
 */

/**
 * @example  examples_sllib/string_bracket.cc
 *           ʸ����γ�̤˰Ϥޤ줿���֤�Ĺ����Ĵ�٤���
 */

/**
 * @example  examples_sllib/verbose_grep.cc
 *           �ͥåȥ�����б�������Ĺ�� grep ���ޥ��
 */

}

#endif  /* _SLI__TSTRING_H */
