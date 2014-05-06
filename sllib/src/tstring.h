/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2013-06-05 11:26:16 cyamauch> */

#ifndef _SLI__TSTRING_H
#define _SLI__TSTRING_H 1

/**
 * @file   tstring.h
 * @brief  文字列を扱うためのクラス tstring の定義とinlineメンバ関数
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
 * @brief  文字列を扱うためのクラス
 *
 *   tstring クラスは，スクリプト言語のような強力な文字列処理用 API を持ち，
 *   かつ LIBC の stdio.h，string.h，strings.h，stdlib.h，ctype.h で提供される
 *   関数とそっくりの文字列処理が行えるAPIを提供します．下記のような機能を持ち
 *   ます．<br>
 *    - 値のセット，追加，挿入等で printf() の記法が使える API <br>
 *    - 1文字単位のアクセス <br>
 *    - 基本的な文字・文字列の検索 <br>
 *    - 文字種の判定 <br>
 *    - 数値への変換 <br>
 *    - 左右の空白，改行文字の除去 <br>
 *    - 文字列配列の結合 <br>
 *    - シェル風の文字列マッチ <br>
 *    - 正規表現による検索，置換(後方参照も可能) <br>
 *    - 数式の括弧の解析 など
 *
 * @attention  LIBC の string.h を併用する場合，tstring.h よりも前に string.h
 *             を include してください．
 * @note  文字列用のバッファは 1Gbyte 未満では 2^n 単位で確保されるため，かなり
 *        頻繁な更新処理に対しても十分なパフォーマンスが得られるでしょう．
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
    /* pos1文字目からn1文字を strのn2文字で置き換える              */
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
    /* pos文字目からn文字の文字列にする            */
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
    /* 左右の空白文字を消す                                   */
    /* side_spaces は正規表現の "[^A-Z]" という表現でも OK    */
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
    /* TAB 文字の展開・TAB 文字への収縮       */
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
    /* 固定長バッファモード時に，バッファに時下書きした場合に使用する        */
    virtual tstring &update_length();

    /* copy internal string into user's buffer          */
    /*   pos:      starting position of internal string */
    /*   dest_str: pointer of user's buffer             */
    /*   bufsize:  byte length to be copied             */
    /* dest_strにpos文字目からコピー                    */
    virtual ssize_t getstr( size_t pos, char *dest_str, size_t bufsize ) const;
    virtual ssize_t getstr( char *dest_str, size_t bufsize ) const;

    /* copy internal string into another tstring object */
    /*   pos:  starting position of internal string     */
    /*   n:    length to be copied                      */
    /*   dest: destination object                       */
    /* destオブジェクトに，位置 pos から n 文字コピー   */
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
    /* '\0'で終わる文字列本体へのポインタを返す                           */
    virtual const char *c_str() const;			/* same as cstr() */
    virtual const char *cstr() const;
    virtual char *str_ptr();
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const char *str_ptr() const;		/* same as cstr() */
#endif
    virtual const char *str_ptr_cs() const;		/* same as cstr() */

    /* this returns character at position = pos of internal string. */
    /* '\0' can be returned.                                        */
    /* pos番目の文字を返す(終端の0も含む)                           */
    virtual int cchr( size_t pos ) const;

    /* read or write character at position = pos of internal string.      */
    /* length of internal buffer is automatically updated.                */
    /* pos番目の文字の読み書き．posに応じてバッファのサイズは自動リサイズ */
    virtual unsigned char &at( size_t pos );
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const unsigned char &at( size_t pos ) const;
#endif
    /* 読み込み専用(リサイズはしない)．普通は使わない */
    virtual const unsigned char &at_cs( size_t pos ) const;

    /* returns length of string being stored in object */
    virtual size_t length() const;
    /* not recommended */
    virtual size_t size() const;			/* same as length() */

    /* maximum length of string that can be stored in object when */
    /* fixed-length buffer mode                                   */
    /* 固定長バッファの場合の文字列の最大の長さを返す             */
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
    /* 大文字，小文字を区別しないで比較                    */
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
    /* pos文字目からn文字の文字列strを探す                                */
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
    /* オマケ */
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
    /* オマケ */
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
    /* pos文字目から文字chを探す                                            */
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
    /* pos文字目からn文字の文字列strに含まれるいずれかの文字を探す         */
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
    /* pos文字目からn文字の文字列strに含まれない文字を探す           */
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
    /* pos文字目からchではない文字を探す                               */
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
    /* pos番目の文字の種類を判定する              */
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
    /* pos文字目から文字列acceptに含まれるいずれかの文字を探す             */
    /* accept は正規表現の "[A-Z]" という表現でも OK．                     */
    /* "[^A-Z]" または "[!A-Z]" で含まれない文字を探すようになる           */
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
    /* 指定した位置 pos に文字セット(accept)があれば，その長さを返す        */
    /* (ない場合は0)．accept は "[A-Z]" という表現でも OK．                 */
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
    /* シェルライクなパターンマッチ strmatch(), fnmatch(), pnmatch()        */
    /* マッチしていれば 0，マッチしない場合は負の値を返す                   */
    /* these member functions use fnmatch() of LIBC with no flags.          */
    virtual int strmatch( const char *pat ) const;
    virtual int strmatch( size_t pos, const char *pat ) const;
    virtual int strmatch( const tstring &pat ) const;
    virtual int strmatch( size_t pos, const tstring &pat ) const;
    /* these member functions use fnmatch() of LIBC with FNM_PERIOD flags.  */
    /* 先頭の . は特別扱いする(ファイル名向け)                              */
    virtual int fnmatch( const char *pat ) const;
    virtual int fnmatch( size_t pos, const char *pat ) const;
    virtual int fnmatch( const tstring &pat ) const;
    virtual int fnmatch( size_t pos, const tstring &pat ) const;
    /* these member functions use fnmatch() of LIBC with                    */
    /* FNM_PATHNAME | FNM_PERIOD.                                           */
    /* / や . は特別扱いする(パス名向け)                                    */
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
    /* POSIX拡張正規表現でのパターンマッチ regmatch()                */
    /* マッチする文字列を探し，マッチする文字列の位置(返り値)と長さ  */
    /* (*ret_span)を返す．マッチしない場合は負の値を返す             */
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
    /* 非推奨: POSIX拡張正規表現でのパターンマッチ(Advanced版その1)     */
    /* 新しいプログラムは，tarray_tstring::regassign() を使ってください */
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
    /* 非推奨: POSIX拡張正規表現でのパターンマッチ(Advanced版その2)     */
    /* 新しいプログラムは，tarray_tstring::regassign() を使ってください */
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
    /* POSIX正規表現による置換 */
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

    /* 一時オブジェクトの return の直前に使い，shallow copy を許可する場合に */
    /* 使う．                                                                */
    virtual void set_scopy_flag();

    /* display internal information of object. */
    /* (for user's debug)                      */
    /* ユーザの debug 用                       */
    virtual void dprint( const char *msg = NULL ) const;

  private:
    /* constructor 用の特別なイニシャライザ */
    void __force_init( bool is_constructor );
    /* destructor 用の特別なメモリ開放用メンバ関数 */
    void __force_free();
    /* shallow copy 関係 */
    bool request_shallow_copy( tstring *from_obj ) const;
    void cancel_shallow_copy( tstring *from_obj ) const;
    void cleanup_shallow_copy( bool do_deep_copy_for_this ) const;
    /* 特別なイニシャライザ */
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
    /* at(), at_cs() 専用メンバ関数 */
    unsigned char &uchar_ref_after_resize( size_t pos );
    unsigned char &err_throw_uchar_ref( const char *func, 
					const char *lv, const char *mes );
    const unsigned char &err_throw_const_uchar_ref( const char *func, 
				       const char *lv, const char *mes ) const;

  private:
    /* 文字列バッファ */
    char *_str_rec;
    /* _str_rec 用に確保しているバッファのバイト長 */
    size_t str_alloc_blen_rec;
    /* 最後に要求された _str_rec のバッファ長                               */
    /* (固定長バッファmodeでは文字列長の上限+1，他modeでは文字列長+1を示す) */
    size_t str_buf_length_rec;
    /* 固定長バッファmodeの場合の文字列長．他modeでは UNDEF が入る */
    size_t str_length_rec;
    /* NULL無しモードでは true */
    bool str_rec_keeps_non_null;
    /* 外部ポインタ変数のアドレス (未使用ならNULL) */
    char **extptr_rec;
    /* 正規表現で使う */
    tregex regex_rec;
    /* 正規表現の結果を格納する(要free) */
    size_t reg_elem_length_rec;
    size_t *_reg_pos_rec;
    size_t *_reg_length_rec;
    char **_reg_cstr_ptr_rec;
    size_t reg_cstrbuf_length_rec;
    char *_reg_cstrbuf_rec;

    /* バッファの継承(shallow copy)のためのフラグ．= 等ではコピーされない */
    /* コピーの src 側のフラグ */
    bool shallow_copy_ok;		/* set_scopy_flag() でセットされる */
    tstring *shallow_copy_dest_obj;	/* コピー先: 有効なら non-NULL */

    /* コピーの dest 側のフラグ(コピー元から書き込まれる) */
    tstring *shallow_copy_src_obj;	/* コピー元: 生きていれば non-NULL */

    /* __shallow_init(), __deep_init() を安全に行なうためのフラグ */
    bool __copying;

#ifndef TSTRING__USE_SOLO_NARG
  private:
    /*
      コンパイル時にエラーを出すための，ダミーのメンバ関数．

      普通の数値を与えると，参照の引数を持つメンバ関数が選ばれてしまう．
      例えば，hoge.append(' '); とすると，コンパイル時に
      append( const tstring &src, size_t pos = 0 ) が選ばれてしまい，
      正常に動作しない．これらを private に置く事で，この問題を防止する．
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
 * @brief  [] で指定された位置の文字の参照(unsigned char型)を返す
 *
 * @param      pos 文字列の位置
 * @return     指定された位置にある文字の参照
 * @throw      固定長バッファモードでpos に最大文字列長以上の値を指定した場合
 * 
 */
inline unsigned char &tstring::operator[]( size_t pos )
{
    return this->at(pos);
}

/**
 * @brief  [] で指定された位置の文字の参照(unsigned char型)を返す (読取専用)
 *
 * @param      pos 文字列の位置
 * @return     指定された位置にある文字の参照
 * @throw      pos に文字列長以上の値を指定した場合
 * 
 */
inline const unsigned char &tstring::operator[]( size_t pos ) const
{
    return this->at_cs(pos);
}

/**
 * @brief  文字列の先頭アドレスを取得 (読取専用)
 *
 * @return     自身が持つ文字列の先頭アドレス
 *
 */
inline const char *tstring::c_str() const
{
    if ( this->shallow_copy_dest_obj != NULL ||
	 this->shallow_copy_src_obj != NULL ) this->cleanup_shallow_copy(true);
    return this->_str_rec;
}

/**
 * @brief  文字列の先頭アドレスを取得 (読取専用)
 *
 * @return     自身が持つ文字列の先頭アドレス
 *
 */
inline const char *tstring::cstr() const
{
    if ( this->shallow_copy_dest_obj != NULL ||
	 this->shallow_copy_src_obj != NULL ) this->cleanup_shallow_copy(true);
    return this->_str_rec;
}

/**
 * @brief  文字列の先頭アドレスを取得
 *
 * @return     自身が持つ文字列の先頭アドレス
 * @note       やむをえない場合以外は，このメンバ関数を使わないでください．
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
 * @brief  文字列の先頭アドレスを取得 (読取専用)
 *
 * @return     自身が持つ文字列の先頭アドレス
 * @note       やむをえない場合以外は，このメンバ関数を使わないでください．
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
 * @brief  文字列の先頭アドレスを取得 (読取専用)
 *
 * @return     自身が持つ文字列の先頭アドレス
 * @note       やむをえない場合以外は，このメンバ関数を使わないでください．
 *
 */
inline const char *tstring::str_ptr_cs() const
{
    if ( this->shallow_copy_dest_obj != NULL ||
	 this->shallow_copy_src_obj != NULL ) this->cleanup_shallow_copy(true);
    return this->_str_rec;
}

/**
 * @brief  自身が持つ文字列の位置 pos にある文字(int型)を返す
 *
 * @param      pos 文字列の位置
 * @return     成功した場合は指定された位置にある文字<br>
 *             失敗した場合は負値
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
 * @brief  自身が持つ文字列の位置 pos にある文字の参照(unsigned char型)を返す
 *
 * @param      pos 文字列の位置
 * @return     指定された位置にある文字の参照
 * @throw      固定長バッファモードでpos に最大文字列長以上の値を指定した場合
 *
 */
inline unsigned char &tstring::at( size_t pos )
{
    if ( this->shallow_copy_dest_obj != NULL ||
	 this->shallow_copy_src_obj != NULL ) this->cleanup_shallow_copy(true);
    return ( (this->str_length_rec != _TSTRING_INTERNAL::_UNDEF) ?
	       /* 固定長バッファの場合 */
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
 * @brief  文字列の位置 pos にある文字の参照(unsigned char型)を返す (読取専用)
 *
 * @param      pos 文字列の位置
 * @return     指定された位置にある文字の参照
 * @throw      pos に文字列長以上の値を指定した場合
 *
 */
inline const unsigned char &tstring::at( size_t pos ) const
{
    return this->at_cs(pos);
}
#endif

/**
 * @brief  文字列の位置 pos にある文字の参照(unsigned char型)を返す (読取専用)
 *
 * @param      pos 文字列の位置
 * @return     指定された位置にある文字の参照
 * @throw      pos に文字列長以上の値を指定した場合
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
 * @brief  文字列の長さ('\0'は含まない)を取得
 *
 * @return  自身の文字列長
 *
 */
inline size_t tstring::length() const
{
    return ( (this->str_length_rec != _TSTRING_INTERNAL::_UNDEF) ?
	this->str_length_rec	/* 固定長バッファの場合 */
	:
	( (1 < this->str_buf_length_rec) ? (this->str_buf_length_rec - 1) : 0 )
      );
}

/**
 * @brief  文字列の長さ('\0'は含まない)を取得
 *
 * @return  自身の文字列長
 */
inline size_t tstring::size() const
{
    return ( (this->str_length_rec != _TSTRING_INTERNAL::_UNDEF) ? 
	this->str_length_rec	/* 固定長バッファの場合 */
	: 
	( (1 < this->str_buf_length_rec) ? (this->str_buf_length_rec - 1) : 0 )
      );
}

/**
 * @brief  固定長バッファモードでの最大の文字列長を取得
 *
 * @return     最大の文字列長
 */
inline size_t tstring::max_length() const
{
    return ( (this->str_length_rec != _TSTRING_INTERNAL::_UNDEF) ? 
	     (this->str_buf_length_rec - 1)	/* 固定長バッファの場合 */
	     :
	     0
	   );
}


/**
 * @example  examples_sllib/string_basic.cc
 *           文字列の扱い方の基本を示したコード
 */

/**
 * @example  examples_sllib/string_edit.cc
 *           文字列の編集の例を示したコード
 */

/**
 * @example  examples_sllib/string_match.cc
 *           文字列のシェル的なマッチングの例を示したコード
 */

/**
 * @example  examples_sllib/string_regexp.cc
 *           文字列に対してPOSIX拡張正規表現を使ってみた例
 */

/**
 * @example  examples_sllib/string_bracket.cc
 *           文字列の括弧に囲まれた位置と長さを調べる例
 */

/**
 * @example  examples_sllib/verbose_grep.cc
 *           ネットワークに対応した冗長な grep コマンド
 */

}

#endif  /* _SLI__TSTRING_H */
