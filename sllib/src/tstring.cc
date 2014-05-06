/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2013-05-14 18:04:31 cyamauch> */

/**
 * @file   tstring.cc
 * @brief  文字列を扱うためのクラス tstring のコード
 */

#define CLASS_NAME "tstring"

#include "config.h"

#include "tstring.h"
#include "heap_mem.h"

#include <stdlib.h>
#include <math.h>

/* #include "tarray_plain.h" */

#include "private/err_report.h"

#include "private/c_memcpy.h"
#include "private/c_memset.h"
#include "private/c_memmove.h"
#include "private/c_strcmp.h"
#include "private/c_strncmp.h"
#include "private/c_strlen.h"

#include "private/c_vsnprintf.h"
#include "private/c_vsscanf.h"
#include "private/c_vasprintf.h"

#include "private/c_isalpha.h"
#include "private/c_isalnum.h"
#include "private/c_strtox.h"
#include "private/c_tolower.h"
#include "private/c_toupper.h"

#include "private/c_strmatch.h"
#include "private/c_regsearch.h"
#include "private/c_regsearchx.h"
#include "private/c_regfatal.h"

/* shallow copy 関係の debug 用 */
//#define debug_report_scopy(arg) err_report(__FUNCTION__,"DEBUG",arg)
#define debug_report_scopy(arg)
//#define debug_report_scopy1(a1,a2) err_report1(__FUNCTION__,"DEBUG",a1,a2)
#define debug_report_scopy1(a1,a2)

/* 正規表現の検索の時，内部キャッシュを使う */
#define REGFUNCS_USING_CACHE 1

namespace sli
{

/* make_accepts_regexp() and is_found() */
#include "private/tstring_regexp_cset.cc"

//static const size_t UNDEF = ~((size_t)0);
static const size_t UNDEF = _TSTRING_INTERNAL::_UNDEF;


static char *find_pattern( const char *s_in, size_t len_in, 
			   const char *accept, int flags, bool bl )
{
    size_t i;
    bool ok = false;
    const unsigned char *s = (const unsigned char *)s_in;
    for ( i=0 ; i < len_in ; i++ ) {
	bool found = is_found(s[i], accept, flags);
	if ( bl == true ) {
	    if ( found == true ) {
		ok = true;
		break;
	    }
	}
	else {
	    if ( found == false ) {
		ok = true;
		break;
	    }
	}
    }
    if ( ok == true ) return (char *)s + i;
    else return NULL;
}

static char *rfind_pattern( const char *s_in, size_t len_in, 
			    const char *accept, int flags, bool bl )
{
    size_t i;
    bool ok = false;
    const unsigned char *s = (const unsigned char *)s_in;
    for ( i=len_in ; 0 < i ; ) {
	bool found;
	i--;
	found = is_found(s[i], accept, flags);
	if ( bl == true ) {
	    if ( found == true ) {
		ok = true;
		break;
	    }
	}
	else {
	    if ( found == false ) {
		ok = true;
		break;
	    }
	}
    }
    if ( ok == true ) return (char *)s + i;
    else return NULL;
}

static size_t pattern_length( const char *s_in, size_t len_in,
			      const char *accept, int flags, bool tf )
{
    size_t i;
    const unsigned char *s = (const unsigned char *)s_in;
    for ( i=0 ; i < len_in ; i++ ) {
	bool found = is_found(s[i], accept, flags);
	if ( tf == true ) {
	    if ( found == false ) break;
	}
	else {
	    if ( found == true ) break;
	}
    }
    return i;
}

static size_t pattern_rlength( const char *s_in, size_t len_in,
			       const char *accept, int flags, bool tf )
{
    size_t i, j;
    const unsigned char *s = (const unsigned char *)s_in;
    for ( i=0, j=len_in-1 ; i < len_in ; i++, j-- ) {
	bool found;
	found = is_found(s[j], accept, flags);
	if ( tf == true ) {
	    if ( found == false ) break;
	}
	else {
	    if ( found == true ) break;
	}
    }
    return i;
}


static void rm_side_spaces( const char *str, 
			    const char *spaces, int flags, bool tf, 
			    const char **ret_str_begin, size_t *ret_str_len )
{
    size_t len_str;
    const unsigned char *p0;
    const unsigned char *p1;

    if ( str == NULL ) {
	if ( ret_str_begin != NULL ) *ret_str_begin = NULL;
	if ( ret_str_len != NULL ) *ret_str_len = 0;
	return;
    }

    len_str = c_strlen(str);
    for ( p0=(const unsigned char *)str ;  ; p0++ ) {	/* skipping ' ' */
	if ( tf == true ) {
	    if ( is_found(*p0, spaces, flags) == false ) break;
	}
	else {
	    if ( is_found(*p0, spaces, flags) == true ) break;
	}
    }
    for ( p1=(const unsigned char *)str+len_str ; p0 < p1 ; p1-- ) {
	if ( *p1 != '\0' ) {
	    if ( tf == true ) {
		if ( is_found(*p1, spaces, flags) == false ) break;
	    }
	    else {
		if ( is_found(*p1, spaces, flags) == true ) break;
	    }
	}
    }
    if ( ret_str_begin != NULL && ret_str_len != NULL ) {
	if ( *p0 == '\0' ) {
	    *ret_str_begin = (const char *)p0;
	    *ret_str_len = 0;
	}
	else {
	    *ret_str_begin = (const char *)p0;
	    *ret_str_len = p1 - p0 + 1;
	}
    }
    return;
}

static int c_strcasecmp(const char *s1_in, const char *s2_in)
{
    const unsigned char *s1 = (const unsigned char *)s1_in;
    const unsigned char *s2 = (const unsigned char *)s2_in;
    int c1, c2;
    size_t i = 0;
    while (1) {
	c1 = c_tolower(s1[i]);
	c2 = c_tolower(s2[i]);
	if ( c1 != c2 ) break;
	else if ( c1 == '\0' ) break;
	i++;
    };
    return c1-c2;
}

static int c_strncasecmp(const char *s1_in, const char *s2_in, size_t n)
{
    const unsigned char *s1 = (const unsigned char *)s1_in;
    const unsigned char *s2 = (const unsigned char *)s2_in;
    int c1 = 0, c2 = 0;
    size_t i = 0;
    while ( i < n ) {
	c1 = c_tolower(s1[i]);
	c2 = c_tolower(s2[i]);
	if ( c1 != c2 ) break;
	else if ( c1 == '\0' ) break;
	i++;
    };
    return c1-c2;
}

/* */
static int c_binstrcmp(const char *s1_in, size_t n1,
                       const char *s2_in, size_t n2)
{
    const unsigned char *s1 = (const unsigned char *)s1_in;
    const unsigned char *s2 = (const unsigned char *)s2_in;
    int c1 = 0, c2 = 0;
    size_t nn = n1, i = 0;

    if ( n2 < nn ) nn = n2;

    /* nn 文字比較する */
    while ( i < nn ) {
        c1 = s1[i];
        c2 = s2[i];
        if ( c1 != c2 ) break;
        i++;
    }
    if ( n1 == n2 ) {   /* n1, n2 が同じの場合 */
        return c1-c2;
    }
    else {              /* n1, n2 が異なる場合 */
        int d = c1-c2;
        if ( d == 0 ) {
            /* nn までのバイナリが一致していても，n1,n2 の */
            /* 長さが違う場合は，異なる事を報告する */
            if ( n1 < n2 ) return -256;
            else if ( n2 < n1 ) return 256;
            else return 0;      /* ←ここには来ない */
        }
        else {
            return d;
        }
    }
}

static int c_binstrncmp(const char *s1_in, size_t n1,
                        const char *s2_in, size_t n2, size_t n)
{
    const unsigned char *s1 = (const unsigned char *)s1_in;
    const unsigned char *s2 = (const unsigned char *)s2_in;
    int c1 = 0, c2 = 0;
    size_t nn = n, i = 0;

    /* n が大きすぎる場合，比較対象を nn 文字に制限する */
    if ( n1 < nn ) nn = n1;
    if ( n2 < nn ) nn = n2;

    /* nn 文字比較する */
    while ( i < nn ) {
        c1 = s1[i];
        c2 = s2[i];
        if ( c1 != c2 ) break;
        i++;
    }
    if ( nn == n ) {    /* nn が初めから変化なしの場合 */
        return c1-c2;
    }
    else {              /* n に比べ n1,n2 が小さくて nn が更新された場合 */
        int d = c1-c2;
        if ( d == 0 ) {
            /* nn までのバイナリが一致していても，n1,n2 の */
            /* 長さが違う場合は，異なる事を報告する */
            if ( n1 < n2 ) return -256;
            else if ( n2 < n1 ) return 256;
            else return 0;
        }
        else {
            return d;
        }
    }
}

static int c_binstrcasecmp(const char *s1_in, size_t n1,
                           const char *s2_in, size_t n2)
{
    const unsigned char *s1 = (const unsigned char *)s1_in;
    const unsigned char *s2 = (const unsigned char *)s2_in;
    int c1 = 0, c2 = 0;
    size_t nn = n1, i = 0;

    if ( n2 < nn ) nn = n2;

    /* nn 文字比較する */
    while ( i < nn ) {
        c1 = c_tolower(s1[i]);
        c2 = c_tolower(s2[i]);
        if ( c1 != c2 ) break;
        i++;
    }
    if ( n1 == n2 ) {   /* n1, n2 が同じの場合 */
        return c1-c2;
    }
    else {              /* n1, n2 が異なる場合 */
        int d = c1-c2;
        if ( d == 0 ) {
            /* nn までのバイナリが一致していても，n1,n2 の */
            /* 長さが違う場合は，異なる事を報告する */
            if ( n1 < n2 ) return -256;
            else if ( n2 < n1 ) return 256;
            else return 0;      /* ←ここには来ない */
        }
        else {
            return d;
        }
    }
}

static int c_binstrncasecmp(const char *s1_in, size_t n1,
                            const char *s2_in, size_t n2, size_t n)
{
    const unsigned char *s1 = (const unsigned char *)s1_in;
    const unsigned char *s2 = (const unsigned char *)s2_in;
    int c1 = 0, c2 = 0;
    size_t nn = n, i = 0;

    /* n が大きすぎる場合，比較対象を nn 文字に制限する */
    if ( n1 < nn ) nn = n1;
    if ( n2 < nn ) nn = n2;

    /* nn 文字比較する */
    while ( i < nn ) {
        c1 = c_tolower(s1[i]);
        c2 = c_tolower(s2[i]);
        if ( c1 != c2 ) break;
        i++;
    }
    if ( nn == n ) {    /* nn が初めから変化なしの場合 */
        return c1-c2;
    }
    else {              /* n に比べ n1,n2 が小さくて nn が更新された場合 */
        int d = c1-c2;
        if ( d == 0 ) {
            /* nn までのバイナリが一致していても，n1,n2 の */
            /* 長さが違う場合は，異なる事を報告する */
            if ( n1 < n2 ) return -256;
            else if ( n2 < n1 ) return 256;
            else return 0;
        }
        else {
            return d;
        }
    }
}

/**
 * @brief  クォーテーションまたは括弧に囲まれた部分の情報を取得(左側からパース)
 *
 * @note   len_to_be_parsed は必ず正しい値をセットすること．<br>
 *         private な関数です．
 */
inline static ssize_t find_quoted_string( 
				  const char *src_str, size_t len_to_be_parsed,
				  size_t pos, const char *quot_bkt, int escape,
				  size_t *quoted_span, size_t *nextpos )
{
    ssize_t ret_pos = -1;			/* returned value */
    size_t ret_quoted_len = 0;
    size_t ret_nextpos = len_to_be_parsed + 1;

    char quot[96];				/* list of quotations */
    char bkt_l[5];				/* list of brackets (left) */
    char bkt_r[5];				/* list of brackets (right) */

    bool prev_escape = false;
    char quot_to_find = '\0';			/* quotation to be found */
    tstring bkt_to_find;			/* 見つけるべきペアの括弧 */
    size_t depth_bkt = 0;			/* 括弧の深さ */

    ssize_t start_pos = -1;

    size_t i;

    if ( src_str == NULL ) goto quit;		/* error */
    if ( len_to_be_parsed < pos ) goto quit;	/* error */

    /*
     * setup quotations and bracket info: quot[] bkt_l[] bkt_r[]
     */
    if ( quot_bkt != NULL ) {
	bool b0=false, b1=false, b2=false, b3=false;
	size_t j=0, k=0;
	for ( i=0 ; quot_bkt[i] != '\0' ; i++ ) {
	    const char ch = quot_bkt[i];
	    if ( ch == '[' || ch == ']' ) {
		if ( b0 == false ) {
		    bkt_l[k] = '[';  bkt_r[k] = ']';
		    k++;  b0 = true;
		}
	    }
	    else if ( ch == '{' || ch == '}' ) {
		if ( b1 == false ) {
		    bkt_l[k] = '{';  bkt_r[k] = '}';
		    k++;  b1 = true;
		}
	    }
	    else if ( ch == '(' || ch == ')' ) {
		if ( b2 == false ) {
		    bkt_l[k] = '(';  bkt_r[k] = ')';
		    k++;  b2 = true;
		}
	    }
	    else if ( ch == '<' || ch == '>' ) {
		if ( b3 == false ) {
		    bkt_l[k] = '<';  bkt_r[k] = '>';
		    k++;  b3 = true;
		}
	    }
	    else {
		quot[j] = ch;
		if ( j + 1 < 96 ) j++;
	    }
	}
	quot[j] = '\0';
	bkt_l[k] = '\0';
	bkt_r[k] = '\0';
    }
    else {
	quot[0] = '\0';
	bkt_l[0] = '\0';
	bkt_r[0] = '\0';
    }
    
    /*
     * parse
     */
    for ( i=pos ; i < len_to_be_parsed ; i++ ) {
	char ch = src_str[i];
	if ( ch == '\0' ) break;
	if ( prev_escape == true ) {		/* 前のchrがescape文字なら */
	    prev_escape = false;
	}
	else if ( ch == escape ) {		/* 今のchrがescape文字なら */
	    prev_escape = true;
	}
	else if ( quot_to_find != '\0' ) {	/* ペアの quot を見つける */
	    if ( ch == quot_to_find ) {
		quot_to_find = '\0';
		if ( 0 <= start_pos && depth_bkt == 0 ) {
		    ret_quoted_len = i + 1 - start_pos;
		    break;
		}
	    }
	}
	else {
	    size_t j;
	    /* quot の開始を見つける */
	    for ( j=0 ; quot[j] != '\0' ; j++ ) {
		if ( ch == quot[j] ) {
		    quot_to_find = ch;
		    if ( start_pos < 0 ) start_pos = i;
		    break;
		}
	    }
	    if ( quot[j] == '\0' ) {
		/* 新規の括弧を見つける */
		for ( j=0 ; bkt_l[j] != '\0' ; j++ ) {
		    if ( ch == bkt_l[j] ) {
			bkt_to_find.at(depth_bkt) = bkt_r[j];
			depth_bkt ++;
			if ( start_pos < 0 ) start_pos = i;
			break;
		    }
		}
	    }
	    if ( bkt_l[j] == '\0' ) {
		/* ペアの括弧を見つける */
		if ( 0 < depth_bkt ) {
		    if ( ch == bkt_to_find.at_cs(depth_bkt-1) ) {
			depth_bkt --;
			if ( 0 <= start_pos && depth_bkt == 0 && quot_to_find == '\0' ) {
			    ret_quoted_len = i + 1 - start_pos;
			    break;
			}
		    }
		}
	    }
	}
    }

    if ( 0 <= start_pos ) {
	/* ペアの括弧，クォーテーションが見つからなかった場合 */
	if ( ret_quoted_len == 0 ) {
	    ret_quoted_len = len_to_be_parsed - start_pos;
	    ret_nextpos = start_pos;
	    ret_pos = -2;
	}
	else {
	    ret_pos = start_pos;
	    ret_nextpos = start_pos + ret_quoted_len;
	}
    }

 quit:
    if ( quoted_span != NULL ) *quoted_span = ret_quoted_len;
    if ( nextpos != NULL ) *nextpos = ret_nextpos;
    return ret_pos;
}

/**
 * @brief  クォーテーションまたは括弧に囲まれた部分の情報を取得(右側からパース)
 *
 * @note   len_to_be_parsed は必ず正しい値をセットすること．<br>
 *         private な関数です．
 */
inline static ssize_t rfind_quoted_string( 
				  const char *src_str, size_t len_to_be_parsed,
				  size_t pos, const char *quot_bkt, int escape,
				  size_t *quoted_span, size_t *nextpos )
{
    ssize_t ret_pos = -1;			/* returned value */
    size_t ret_quoted_len = 0;
    size_t ret_nextpos = len_to_be_parsed + 1;

    char quot[96];				/* list of quotations */
    char bkt_l[5];				/* list of brackets (left) */
    char bkt_r[5];				/* list of brackets (right) */

    bool prev_escape;
    char quot_to_find = '\0';			/* quotation to be found */
    tstring bkt_to_find;			/* 見つけるべきペアの括弧 */
    size_t depth_bkt = 0;			/* 括弧の深さ */

    ssize_t start_pos = -1;

    size_t i;

    //stdstreamio sio;

    if ( src_str == NULL ) goto quit;		/* error */
    if ( len_to_be_parsed < pos ) goto quit;	/* error */

    /*
     * setup quotations and bracket info: quot[] bkt_l[] bkt_r[]
     */
    if ( quot_bkt != NULL ) {
	bool b0=false, b1=false, b2=false, b3=false;
	size_t j=0, k=0;
	for ( i=0 ; quot_bkt[i] != '\0' ; i++ ) {
	    const char ch = quot_bkt[i];
	    if ( ch == '[' || ch == ']' ) {
		if ( b0 == false ) {
		    bkt_l[k] = '[';  bkt_r[k] = ']';
		    k++;  b0 = true;
		}
	    }
	    else if ( ch == '{' || ch == '}' ) {
		if ( b1 == false ) {
		    bkt_l[k] = '{';  bkt_r[k] = '}';
		    k++;  b1 = true;
		}
	    }
	    else if ( ch == '(' || ch == ')' ) {
		if ( b2 == false ) {
		    bkt_l[k] = '(';  bkt_r[k] = ')';
		    k++;  b2 = true;
		}
	    }
	    else if ( ch == '<' || ch == '>' ) {
		if ( b3 == false ) {
		    bkt_l[k] = '<';  bkt_r[k] = '>';
		    k++;  b3 = true;
		}
	    }
	    else {
		quot[j] = ch;
		if ( j + 1 < 96 ) j++;
	    }
	}
	quot[j] = '\0';
	bkt_l[k] = '\0';
	bkt_r[k] = '\0';
    }
    else {
	quot[0] = '\0';
	bkt_l[0] = '\0';
	bkt_r[0] = '\0';
    }
    
    /*
     * parse
     */
    for ( i=pos+1 ; 0 < i ; ) {
	char ch;
	i --;
	ch = src_str[i];
	//sio.printf("[%c]\n",ch);
	if ( ch != '\0' ) {
	    prev_escape = false;
	    if ( 0 < i ) {			/* escape文字をチェック */
		size_t j = i;
		while ( 0 < j ) {
		    j --;
		    if ( src_str[j] != escape ) break;
		}
		if ( 1 < (i - j) && ((i - j) % 2) == 0 ) prev_escape = true;
	    }
	    if ( prev_escape == true ) {	/* 前のchrがescape文字なら */
		i --;
	    }
	    else if ( quot_to_find != '\0' ) {	/* ペアの quot を見つける */
		if ( ch == quot_to_find ) {
		    quot_to_find = '\0';
		    if ( 0 <= start_pos && depth_bkt == 0 ) {
			ret_quoted_len = start_pos + 1 - i;
			break;
		    }
		}
	    }
	    else {
		size_t j;
		/* quot の開始を見つける */
		for ( j=0 ; quot[j] != '\0' ; j++ ) {
		    if ( ch == quot[j] ) {
			quot_to_find = ch;
			if ( start_pos < 0 ) start_pos = i;
			break;
		    }
		}
		if ( quot[j] == '\0' ) {
		    /* 新規の括弧を見つける */
		    for ( j=0 ; bkt_r[j] != '\0' ; j++ ) {
			if ( ch == bkt_r[j] ) {
			    bkt_to_find.at(depth_bkt) = bkt_l[j];
			    depth_bkt ++;
			    if ( start_pos < 0 ) start_pos = i;
			    break;
			}
		    }
		}
		if ( bkt_r[j] == '\0' ) {
		    /* ペアの括弧を見つける */
		    if ( 0 < depth_bkt ) {
			if ( ch == bkt_to_find.at_cs(depth_bkt-1) ) {
			    depth_bkt --;
			    if ( 0 <= start_pos && depth_bkt == 0 && quot_to_find == '\0' ) {
				ret_quoted_len = start_pos + 1 - i;
				break;
			    }
			}
		    }
		}
	    }
	}
    }

    if ( 0 <= start_pos ) {
	/* ペアの括弧，クォーテーションが見つからなかった場合 */
	if ( ret_quoted_len == 0 ) {
	    ret_quoted_len = start_pos + 1;
	    ret_nextpos = start_pos;
	    ret_pos = -2;
	}
	else {
	    ret_pos = start_pos + 1 - ret_quoted_len;
	    if ( 0 < ret_pos ) ret_nextpos = ret_pos - 1;
	}
    }

 quit:
    if ( quoted_span != NULL ) *quoted_span = ret_quoted_len;
    if ( nextpos != NULL ) *nextpos = ret_nextpos;
    return ret_pos;
}

/**
 * @brief  一番外側のクォーテーションまたは括弧を消去
 *
 *  左側からクォーテーションまたは括弧に囲まれた部分を探し，その部分文字列の
 *  うち，一番外側のクォーテーションまたは括弧を消去する．
 *
 * @note  *len_target_str は必ず正しい値をセットすること．<br>
 *        private な関数です．
 */
inline static ssize_t erase_quotations( 
				 char *target_str, size_t *len_target_str,
				 size_t pos, const char *quot_bkt, int escape,
				 bool rm_escape, size_t *quoted_span,
				 size_t *nextpos, bool do_all )
{
    const size_t len_to_be_parsed = *len_target_str;
    ssize_t ret_pos = -1;			/* returned value */
    size_t ret_quoted_len = 0;
    size_t ret_nextpos = len_to_be_parsed + 1;

    const char *src_str = target_str;
    char *dest_str = target_str;

    char quot[96];				/* list of quotations */
    char bkt_l[5];				/* list of brackets (left) */
    char bkt_r[5];				/* list of brackets (right) */

    bool prev_escape;
    char quot_to_find = '\0';			/* quotation to be found */
    tstring bkt_to_find;			/* 見つけるべきペアの括弧 */
    size_t depth_bkt = 0;			/* 括弧の深さ */

    ssize_t start_pos = -1;
    size_t src_quoted_len = 0;

    size_t i;		/* for src */
    size_t ii;		/* for dest */

    if ( src_str == NULL ) goto quit;		/* error */
    if ( len_to_be_parsed < pos ) goto quit;	/* error */

    /*
     * setup quotations and bracket info: quot[] bkt_l[] bkt_r[]
     */
    if ( quot_bkt != NULL ) {
	bool b0=false, b1=false, b2=false, b3=false;
	size_t j=0, k=0;
	for ( i=0 ; quot_bkt[i] != '\0' ; i++ ) {
	    const char ch = quot_bkt[i];
	    if ( ch == '[' || ch == ']' ) {
		if ( b0 == false ) {
		    bkt_l[k] = '[';  bkt_r[k] = ']';
		    k++;  b0 = true;
		}
	    }
	    else if ( ch == '{' || ch == '}' ) {
		if ( b1 == false ) {
		    bkt_l[k] = '{';  bkt_r[k] = '}';
		    k++;  b1 = true;
		}
	    }
	    else if ( ch == '(' || ch == ')' ) {
		if ( b2 == false ) {
		    bkt_l[k] = '(';  bkt_r[k] = ')';
		    k++;  b2 = true;
		}
	    }
	    else if ( ch == '<' || ch == '>' ) {
		if ( b3 == false ) {
		    bkt_l[k] = '<';  bkt_r[k] = '>';
		    k++;  b3 = true;
		}
	    }
	    else {
		quot[j] = ch;
		if ( j + 1 < 96 ) j++;
	    }
	}
	quot[j] = '\0';
	bkt_l[k] = '\0';
	bkt_r[k] = '\0';
    }
    else {
	quot[0] = '\0';
	bkt_l[0] = '\0';
	bkt_r[0] = '\0';
    }
    
    /*
     * parse
     */
    ii = pos;	/* idx for dest */

    while ( 1 ) {

	bool cpy_all_and_break = false;

	prev_escape = false;
	for ( i=pos ; i < len_to_be_parsed ; i++ ) {
	    char ch = src_str[i];
	    if ( ch == '\0' ) break;
	    if ( prev_escape == true ) {	/* 前のchrがescape文字なら */
		prev_escape = false;
	    }
	    else if ( ch == escape ) {		/* 今のchrがescape文字なら */
		prev_escape = true;
	    }
	    else if ( quot_to_find != '\0' ) {	/* ペアの quot を見つける */
		if ( ch == quot_to_find ) {
		    quot_to_find = '\0';
		    if ( 0 <= start_pos && depth_bkt == 0 ) {
			src_quoted_len = i + 1 - start_pos;
			break;
		    }
		}
	    }
	    else {
		size_t j;
		/* quot の開始を見つける */
		for ( j=0 ; quot[j] != '\0' ; j++ ) {
		    if ( ch == quot[j] ) {
			quot_to_find = ch;
			if ( start_pos < 0 ) start_pos = i;
			break;
		    }
		}
		if ( quot[j] == '\0' ) {
		    /* 新規の括弧を見つける */
		    for ( j=0 ; bkt_l[j] != '\0' ; j++ ) {
			if ( ch == bkt_l[j] ) {
			    bkt_to_find.at(depth_bkt) = bkt_r[j];
			    depth_bkt ++;
			    if ( start_pos < 0 ) start_pos = i;
			    break;
			}
		    }
		}
		if ( bkt_l[j] == '\0' ) {
		    /* ペアの括弧を見つける */
		    if ( 0 < depth_bkt ) {
			if ( ch == bkt_to_find.at_cs(depth_bkt-1) ) {
			    depth_bkt --;
			    if ( 0 <= start_pos && depth_bkt == 0 && quot_to_find == '\0' ) {
				src_quoted_len = i + 1 - start_pos;
				break;
			    }
			}
		    }
		}
	    }
	}

	if ( 0 <= start_pos ) {
	    /* ペアの括弧，クォーテーションが見つからなかった場合 */
	    if ( src_quoted_len == 0 ) {
		ret_pos = -2;
		cpy_all_and_break = true;
		break;
	    }
	    else {
		ret_pos = start_pos;
		/* update non-quoted part */
		i = pos;
		prev_escape = false;
		while ( i < (size_t)start_pos ) {
		    char ch = src_str[i];
		    bool cpy_ok = true;
		    if ( prev_escape == true ) {  /* 前のchrがescape文字なら */
			prev_escape = false;
		    }
		    else if ( ch == escape ) {	  /* 今のchrがescape文字なら */
			prev_escape = true;
			if ( rm_escape == true ) cpy_ok = false;
		    }
		    if ( cpy_ok == true ) {
			dest_str[ii] = ch;
			ii ++;
		    }
		    i ++;
		}
		ret_quoted_len = ii;
		/* update quoted part */
		i ++;	/* skip a quote char */
		prev_escape = false;
		while ( i + 1 < start_pos + src_quoted_len ) {
		    char ch = src_str[i];
		    bool cpy_ok = true;
		    if ( prev_escape == true ) {  /* 前のchrがescape文字なら */
			prev_escape = false;
		    }
		    else if ( ch == escape ) {	  /* 今のchrがescape文字なら */
			prev_escape = true;
			if ( rm_escape == true ) cpy_ok = false;
		    }
		    if ( cpy_ok == true ) {
			dest_str[ii] = ch;
			ii ++;
		    }
		    i ++;
		}
		i ++;	/* skip a quote char */
		/* */
		ret_nextpos = ii;	/* mark */
		ret_quoted_len = ii - ret_quoted_len;
		pos = i;
		if ( do_all == false ) {
		    rm_escape = false;		/* copy only */
		    cpy_all_and_break = true;
		}
		else {
		    /* reset */
		    src_quoted_len = 0;
		    start_pos = -1;
		    /* go to the next loop */
		}
	    }
	}
	else {
	    cpy_all_and_break = true;
	}

	if ( cpy_all_and_break == true ) {
	    /* update string */
	    i = pos;
	    prev_escape = false;
	    while ( i < len_to_be_parsed ) {
		char ch = src_str[i];
		bool cpy_ok = true;
		if ( prev_escape == true ) {	  /* 前のchrがescape文字なら */
		    prev_escape = false;
		}
		else if ( ch == escape ) {	  /* 今のchrがescape文字なら */
		    prev_escape = true;
		    if ( rm_escape == true ) cpy_ok = false;
		}
		if ( cpy_ok == true ) {
		    dest_str[ii] = ch;
		    ii ++;
		}
		i ++;
	    }
	    dest_str[ii] = '\0';
	    *len_target_str = ii;		/* update length of string */
	    break;
	}

    }

    if ( ret_nextpos == len_to_be_parsed + 1 ) {
	ret_nextpos = *len_target_str + 1;
    }

 quit:
    if ( nextpos != NULL ) *nextpos = ret_nextpos;
    if ( quoted_span != NULL ) *quoted_span = ret_quoted_len;
    return ret_pos;
}


/*
 * 基本的には，オブジェクト生成時は，ヒープに領域を確保しない．
 * したがって，何もしないと obj.cstr() は NULL が返る．
 *
 * ただし，コンストラクタで引数に true が与えられた場合，
 * this->_str_rec が NULL になる事はない．
 * この場合は，文字列長 0 の文字列で初期化される．
 * 
 * オブジェクトを初期状態に戻したい場合は，
 * obj = NULL;
 * とする．
 */

/**
 * @brief    constructor 用の特別なイニシャライザ
 *
 * @param    is_constructor コンストラクタによって呼び出される場合は true を，
 *                          init()等から呼ぶ場合は false をセットする．
 *
 * @note     このメンバ関数は private です．<br>
 *           固定長バッファの場合，バッファ長の情報が消えるので注意．
 *
 */
void tstring::__force_init( bool is_constructor )
{
    this->_str_rec = NULL;
    this->str_alloc_blen_rec = 0;
    this->str_buf_length_rec = 0;

    this->reg_elem_length_rec = 0;
    this->_reg_pos_rec = NULL;
    this->_reg_length_rec = NULL;
    this->_reg_cstr_ptr_rec = NULL;
    this->reg_cstrbuf_length_rec = 0;
    this->_reg_cstrbuf_rec = NULL;

    if ( is_constructor == true ) {

	/* これらは .init() では初期化されない */
	this->str_length_rec = UNDEF;
	this->str_rec_keeps_non_null = false;

	this->extptr_rec = NULL;
	this->shallow_copy_ok = false;
	this->shallow_copy_dest_obj = NULL;
	this->shallow_copy_src_obj = NULL;
	this->__copying = false;

    }

    return;
}

/**
 * @brief  コンストラクタ
 *
 *  コンストラクタです．<br>
 *  ユーザのポインタ変数のアドレスを extptr_address に与えると，ユーザのポイン
 *  タ変数がオブジェクトに登録され，オブジェクトが持つ文字列の先頭アドレスを
 *  常にユーザのポインタ変数に保持させておくことができます．
 * 
 * @param   extptr_address ユーザのポインタ変数のアドレス (省略可)
 * @attention  ユーザのポインタ変数を登録すると shallow copy が機能しなくなり
 *             ます．
 */
tstring::tstring(char **extptr_address)
{
    this->__force_init(true);

    this->extptr_rec = extptr_address;

    if ( this->extptr_rec != NULL ) *(this->extptr_rec) = this->_str_rec;

    return;
}

/**
 * @brief  コンストラクタ
 *
 *  コンストラクタです．
 *  buffer_keeps_non_nullがtureの場合，NULL無しモードで初期化します．<br>
 *  ユーザのポインタ変数のアドレスを extptr_address に与えると，ユーザのポイン
 *  タ変数がオブジェクトに登録され，オブジェクトが持つ文字列の先頭アドレスを
 *  常にユーザのポインタ変数に保持させておくことができます．
 * 
 * @param  buffer_keeps_non_null NULL無しモードにするかどうかのフラグ
 * @param  extptr_address ユーザのポインタ変数のアドレス (省略可)
 * @attention  ユーザのポインタ変数を登録すると shallow copy が機能しなくなり
 *             ます．
 */
tstring::tstring(bool buffer_keeps_non_null, char **extptr_address)
{
    this->__force_init(true);

    this->str_rec_keeps_non_null = buffer_keeps_non_null;
    this->extptr_rec = extptr_address;

    if ( this->extptr_rec != NULL ) *(this->extptr_rec) = this->_str_rec;

    if ( this->str_rec_keeps_non_null == true ) {
        this->__init_non_null_str_rec();
    }

    return;
}

/*
 * このコンストラクタは，固定長バッファ(高速動作)モードを使いたい時に使う．
 * これで初期化すると，バッファの大きさは変えられないが，printf()などのメ
 * ンバ関数はヒープの再確保を最小限しかやらなくなるので，高速動作が期待で
 * きる．
 */

/**
 * @brief  コンストラクタ
 *
 *  コンストラクタです．固定長バッファモードで初期化を行います．<br>
 *  ユーザのポインタ変数のアドレスを extptr_address に与えると，ユーザのポイン
 *  タ変数がオブジェクトに登録され，オブジェクトが持つ文字列の先頭アドレスを
 *  常にユーザのポインタ変数に保持させておくことができます．
 * 
 * @param  max_length 自身の扱う文字数
 * @param  extptr_address ユーザのポインタ変数のアドレス (省略可)
 * @attention  ユーザのポインタ変数を登録すると shallow copy が機能しなくなり
 *             ます．
 */
tstring::tstring(size_t max_length, char **extptr_address)
{
    this->__force_init(true);

    this->extptr_rec = extptr_address;

    if ( 0 < max_length ) {
	this->__init_fixed_str_rec(max_length);
    }

    if ( this->extptr_rec != NULL ) *(this->extptr_rec) = this->_str_rec;

    return;
}

/**
 * @brief  コンストラクタ
 *
 *  コンストラクタです．固定長バッファモードで初期化を行います．<br>
 *  ユーザのポインタ変数のアドレスを extptr_address に与えると，ユーザのポイン
 *  タ変数がオブジェクトに登録され，オブジェクトが持つ文字列の先頭アドレスを
 *  常にユーザのポインタ変数に保持させておくことができます．
 * 
 * @param  max_length 自身の扱う文字数
 * @param  extptr_address ユーザのポインタ変数のアドレス (省略可)
 * @attention  ユーザのポインタ変数を登録すると shallow copy が機能しなくなり
 *             ます．
 */
tstring::tstring(int max_length, char **extptr_address)
{
    this->__force_init(true);

    this->extptr_rec = extptr_address;

    if ( 0 < max_length ) {
	this->__init_fixed_str_rec(max_length);
    }

    if ( this->extptr_rec != NULL ) *(this->extptr_rec) = this->_str_rec;

    return;
}

/**
 * @brief  コピーコンストラクタ
 *
 *  コピーコンストラクタです．自身を obj の内容で初期化します．
 * 
 * @param  obj コピー元のオブジェト
 */
tstring::tstring(const tstring &obj)
{
    this->__force_init(true);

    this->init(obj);

    return;
}

/**
 * @brief  コピーコンストラクタ
 *
 *  コピーコンストラクタです．自身を初期化し，文字列 str を代入します．
 * 
 * @param  str 源泉となる文字列
 */
tstring::tstring(const char *str)
{
    this->__force_init(true);

    if ( str != NULL ) {
        this->assign(str);
    }

    return;
}

/**
 * @brief  destructor 用の特別なメモリ開放用メンバ関数
 *
 * @note   このメンバ関数は private です．
 *
 */
void tstring::__force_free()
{
    if ( this->_str_rec != NULL ) {
	free(this->_str_rec);
    }
    if ( this->_reg_pos_rec != NULL ) {
	free(this->_reg_pos_rec);
    }
    if ( this->_reg_length_rec != NULL ) {
	free(this->_reg_length_rec);
    }
    if ( this->_reg_cstr_ptr_rec != NULL ) {
	free(this->_reg_cstr_ptr_rec);
    }
    if ( this->_reg_cstrbuf_rec != NULL ) {
	free(this->_reg_cstrbuf_rec);
    }

    return;
}

/**
 * @brief  デストラクタ
 *
 */
tstring::~tstring()
{
    /* shallow copy された後，順当に消滅する場合 (copy の src 側の処理) */
    if ( this->shallow_copy_dest_obj != NULL ) {
	/* こちら側が消滅した事を通知する */
	this->shallow_copy_dest_obj->shallow_copy_src_obj = NULL;
	/* バッファは開放してはいけない */
    	debug_report_scopy("destructor not free() [0]");
	return;
    }
    /* shallow copy の src がまだ生きている場合 (copy の dest 側の処理) */
    else if ( this->shallow_copy_src_obj != NULL ) {
	/* shallow copy をキャンセルする */
	this->shallow_copy_src_obj->cancel_shallow_copy(this);
	/* バッファは開放してはいけない */
    	debug_report_scopy("destructor not free() [1]");
	return;
    }
    else {
	this->__force_free();
    	debug_report_scopy("destructor done free()");
	return;
    }
}


/**
 * @brief  shallow copy が可能かを取得
 * 
 *  src側で実行する．<br>
 *  SLLIBの実装では，shallow copy は特定の相手とのみ可．
 * 
 * @param   from_obj リクエストを送信する側のオブジェクトのアドレス
 * @return  shallow copyが可能なら真<br>
 *          それ以外の時は偽
 * @note このメンバ関数は private です
 */
bool tstring::request_shallow_copy( tstring *from_obj ) const
{
    if ( this->shallow_copy_ok == true ) {
	/* 双方とも shallow copy に無関係である事をチェック */
	if ( this->shallow_copy_dest_obj == NULL &&
	     this->shallow_copy_src_obj == NULL &&
	     from_obj->shallow_copy_dest_obj == NULL &&
	     from_obj->shallow_copy_src_obj == NULL ) {
	    /* 両者の関係を登録 */
	    tstring *thisp = (tstring *)this;
	    /* 相手を自身のメンバ変数にマーク */
	    /* (相手が消滅または deep copy 処理後にNULLになる) */
	    thisp->shallow_copy_dest_obj = from_obj;
	    /* 自身を相手のメンバ変数にマーク */
	    /* (自身が消滅したらNULLになる．See デストラクタ) */
	    from_obj->shallow_copy_src_obj = thisp;
	    /* */
	    debug_report_scopy("accepted shallow copy");
	    return true;
	}
    }
    debug_report_scopy("rejected shallow copy");
    return false;
}

/**
 * @brief  shallow copy をキャンセル
 *
 *  src側で実行する．
 * 
 * @param  from_obj リクエストを送信した側のオブジェクトのアドレス
 * @note   from_obj から呼んだ直後に from_obj にて __force_init() を実行する
 *         必要があります．<br>
 *         このメンバ関数は private です
 */
void tstring::cancel_shallow_copy( tstring *from_obj ) const
{
    debug_report_scopy1("arg: from_obj = %zx",(size_t)from_obj);

    if ( this->shallow_copy_dest_obj == from_obj ) {
	tstring *thisp = (tstring *)this;
	/* 相手側をNULLに */
	from_obj->shallow_copy_src_obj = NULL;
	/* 自身のもNULLに */
	thisp->shallow_copy_dest_obj = NULL;
    }
    else {
 	err_throw(__FUNCTION__,"FATAL","internal error");
    }

    return;
}

/**
 * @brief  自身について，shallow copy 関係についてのクリーンアップを行なう
 *
 * @param  do_deep_copy_for_this shallow copy のキャンセル後に，自身について
 *                               deep copy を行なう場合は true をセットする．
 * @note   次の場合に呼び出す必要がある．<br>
 *          1. バッファに書き込む場合<br>
 *          2. バッファのアドレスを返す場合<br>
 *          3. __shallow_init(), __deep_init() 呼び出しの直前<br>
 *         このメンバ関数は private です．
 */
void tstring::cleanup_shallow_copy( bool do_deep_copy_for_this ) const
{
    //debug_report_scopy1("arg: do_deep_copy_for_this = %d",
    //			(int)do_deep_copy_for_this);

    tstring *thisp = (tstring *)this;

    /* 念のため... */
    if ( this->__copying == true ) {
	err_report(__FUNCTION__,"WARNING","This should not be reported");
    }

    /* shallow copy の src がまだ生きている場合(dest 側の処理)，*/
    /* shallow copy のキャンセル処理を行なう */
    if ( thisp->shallow_copy_src_obj != NULL ) {

	const tstring &_src_obj = *(thisp->shallow_copy_src_obj);
	tstring *_dest_obj = thisp;

	/* shallow copy をキャンセルし，強制初期化 */
	_src_obj.cancel_shallow_copy(_dest_obj);
	_dest_obj->__force_init(false);

	if ( do_deep_copy_for_this == true ) {
	    /* deep copy を実行 */
	    _dest_obj->__deep_init(_src_obj);
	}
    }
    /* dest_obj が shallow copy の src になっていた場合，切りはなす */
    else if ( thisp->shallow_copy_dest_obj != NULL ) {

	const tstring &_src_obj = *thisp;
	tstring *_dest_obj = thisp->shallow_copy_dest_obj;

	/* shallow copy をキャンセルし，強制初期化 */
	_src_obj.cancel_shallow_copy(_dest_obj);
	_dest_obj->__force_init(false);

	/* deep copy を実行 */
	_dest_obj->__deep_init(_src_obj);
    }

    return;
}

/**
 * @brief  obj の内容を自身にコピー (shallow copy; 超低レベル)
 *
 *  shallow copy を行なう場合か，データの移動を行なう場合に利用する．
 *
 * @param  obj コピー元オブジェクトのアドレス
 * @param  is_move データコンテンツの「移動」の場合，true をセット
 * @note   メンバ変数を完全に初期化・コピーする．バッファはアドレスだけがコピー
 *         される．ただし this->shallow_copy_src_obj だけは消去されない．
 *         バッファを全て開放し，単純にバッファを share してしまうので，使用に
 *         は注意が必要．<br>
 *         このメンバ関数を使う前に shallow copy 関係をクリーンにしておく．<br>
 *         このメンバ関数は private です．
 */
tstring &tstring::__shallow_init( const tstring &obj, bool is_move )
{
    /*
     *  ここでは，this でも obj でも cleanup_shallow_copy() を使っているもの
     *  を呼ばないよう注意．
     */

    debug_report_scopy1("arg: obj = %zx",(size_t)(&obj));

    tstring *const obj_bak = this->shallow_copy_src_obj;
    
    if ( this->str_length_rec != UNDEF ) {	/* 固定長の場合は使えない */
	err_throw(__FUNCTION__,"FATAL",
		  "Internal error: cannot perform shallow copy [0]");
    }
    if ( this->str_rec_keeps_non_null == true ) {
	/* shallow copy の場合．NULL無しモードで相手がNULLだとダメである */
	if ( is_move == false && obj._str_rec == NULL ) {
	    err_throw(__FUNCTION__,"FATAL",
		      "Internal error: cannot perform shallow copy [1]");
	}
    }
    if ( is_move == false ) {
	/* いわゆる shallow copy の場合．外部ptrの使用は禁止される */
	if ( this->extptr_rec != NULL ) {
	    err_throw(__FUNCTION__,"FATAL",
		      "Internal error: cannot perform shallow copy [2]");
	}
    }

    /* 強制初期化 */
    this->__force_free();
    this->__force_init(false);

    tstring *objp = (tstring *)(&obj);
    objp->__copying = true;
    this->__copying = true;

    try {

	/* アドレスだけコピー */
	this->_str_rec = obj._str_rec;
	this->str_alloc_blen_rec = obj.str_alloc_blen_rec;
	this->str_buf_length_rec = obj.str_buf_length_rec;

	this->reg_elem_length_rec = obj.reg_elem_length_rec;
	this->_reg_pos_rec = obj._reg_pos_rec;
	this->_reg_length_rec = obj._reg_length_rec;
	this->_reg_cstr_ptr_rec = obj._reg_cstr_ptr_rec;
	this->reg_cstrbuf_length_rec = obj.reg_cstrbuf_length_rec;
	this->_reg_cstrbuf_rec = obj._reg_cstrbuf_rec;

	if ( is_move == true ) {

	    /* この部分は，データの行き先が確定している「移動」の場合しか */
	    /* 許されない                                                 */

	    /* NULL無しモードの場合の設定 */
	    if ( this->str_rec_keeps_non_null == true ) {
		if ( this->_str_rec == NULL ) this->__init_non_null_str_rec();
	    }

	    /* 外部ポインタの更新 */
	    if ( this->extptr_rec != NULL ) {
		*(this->extptr_rec) = this->_str_rec;
	    }

	}

    }
    catch (...) {
	objp->__copying = false;
	this->__copying = false;
	this->shallow_copy_src_obj = obj_bak;
	err_throw(__FUNCTION__,"FATAL","caught exception");
    }
    objp->__copying = false;
    this->__copying = false;
    this->shallow_copy_src_obj = obj_bak;

    return *this;
}

/**
 * @brief  obj の内容を自身にコピー (deep copy; 超低レベル)
 *
 * @param  obj コピー元オブジェクトのアドレス
 * @note   obj 側については，_defalut_rec などを直接みにいくので注意．<br>
 *         このメンバ関数を使う前にshallow copy関係をクリーンにしておく事．<br>
 *         このメンバ関数は private です．
 */
tstring &tstring::__deep_init( const tstring &obj )
{
    /*
     *  ここでは，obj 側で cleanup_shallow_copy() を使っているもの
     *  を呼ばないよう注意．
     */

    debug_report_scopy1("arg: obj = %zx",(size_t)(&obj));

    tstring *objp = (tstring *)(&obj);
    objp->__copying = true;

    try {

	/*
	 * Note: obj はどの動作モードがきても良いようにしている
	 */
	if ( this->str_length_rec != UNDEF ) {		/* 固定長の場合 */

	    if ( this->str_buf_length_rec < obj.str_buf_length_rec ) {
		/* バッファ長が足りない場合 */
		c_memcpy(this->_str_rec, obj._str_rec, 
			 this->str_buf_length_rec);
		if ( 0 < this->str_buf_length_rec ) {
		    /* 足りないので \0 を打つ */
		    this->_str_rec[this->str_buf_length_rec - 1] = '\0';
		    /* 入れるべきは obj.length() である */
		    if ( obj.length() < this->str_buf_length_rec ) 
			this->str_length_rec = obj.length();
		    else this->str_length_rec = this->str_buf_length_rec - 1;
		}
		else this->str_length_rec = 0;	/* これはありえないが… */
	    }
	    else {
		/* ピッタリまたは余る場合 */
		c_memcpy(this->_str_rec, obj._str_rec, 
			 obj.str_buf_length_rec);
		c_memset(this->_str_rec + obj.str_buf_length_rec, 0, 
			 this->str_buf_length_rec - obj.str_buf_length_rec);
		this->str_length_rec = obj.length();
	    }

	}
	else {						/* 通常モードの場合 */

	    if ( obj._str_rec == NULL ) {
		if ( this->str_rec_keeps_non_null == true ) {
		    this->__init_non_null_str_rec();
		}
		else {
		    this->free_str_rec();
		    this->str_buf_length_rec = 0;
		}
	    }
	    else {
		/* 文字列長が等しい場合は現在のバッファをそのまま使う */
		if ( this->str_buf_length_rec == obj.length() + 1 ) {
		    c_memcpy(this->_str_rec, obj._str_rec, 
			     this->str_buf_length_rec);
		}
		else {
		    if ( this->realloc_str_rec(obj.length() + 1) < 0 ) {
			err_throw(__FUNCTION__,"FATAL","realloc() failed");
		    }
		    this->str_buf_length_rec = obj.length() + 1;
		    c_memcpy(this->_str_rec, obj._str_rec, 
			     this->str_buf_length_rec);
		}
	    }

	}

	if ( this->extptr_rec != NULL ) *(this->extptr_rec) = this->_str_rec;

	/*
	 * copy results of regmatch()
	 */

	if ( obj.reg_elem_length_rec == 0 ) {
	    if ( 0 < this->reg_elem_length_rec ) {
		/* */
		this->free_reg_pos_rec();
		this->free_reg_length_rec();
		this->free_reg_cstr_ptr_rec();
		/* */
		this->reg_elem_length_rec = 0;
	    }
	} 
	else { 
	    const size_t new_n = obj.reg_elem_length_rec;
	    size_t i;
	    /* */
	    if ( this->realloc_reg_pos_rec(new_n) < 0 ) {
		this->init_reg_results();
		err_throw(__FUNCTION__,"FATAL","realloc() failed");
	    }
	    for ( i=0 ; i < new_n ; i++ ) 
		this->_reg_pos_rec[i] = obj._reg_pos_rec[i];
	    /* */
	    if ( this->realloc_reg_length_rec(new_n) < 0 ) {
		this->init_reg_results();
		err_throw(__FUNCTION__,"FATAL","realloc() failed");
	    }
	    for ( i=0 ; i < new_n ; i++ ) 
		this->_reg_length_rec[i] = obj._reg_length_rec[i];
	    /* */
	    if ( this->realloc_reg_cstr_ptr_rec(new_n + 1) < 0 ) {
		this->init_reg_results();
		err_throw(__FUNCTION__,"FATAL","realloc() failed");
	    }
	    for ( i=0 ; i < new_n + 1 ; i++ )
		this->_reg_cstr_ptr_rec[i] = obj._reg_cstr_ptr_rec[i];
	    /* */
	    this->reg_elem_length_rec = new_n;
	}

	if ( obj.reg_cstrbuf_length_rec == 0 ) {
	    if ( 0 < this->reg_cstrbuf_length_rec ) {
		this->free_reg_cstrbuf_rec();
		/* */
		this->reg_cstrbuf_length_rec = 0;
	    }
	} 
	else { 
	    const size_t new_n = obj.reg_cstrbuf_length_rec;
	    size_t i;
	    /* */
	    if ( this->realloc_reg_cstrbuf_rec(new_n) < 0 ) {
		this->init_reg_results();
		err_throw(__FUNCTION__,"FATAL","realloc() failed");
	    }
	    for ( i=0 ; i < new_n ; i++ )
		this->_reg_cstrbuf_rec[i] = obj._reg_cstrbuf_rec[i];
	    /* */
	    this->reg_cstrbuf_length_rec = new_n;
	}

    }
    catch (...) {
	objp->__copying = false;
	err_throw(__FUNCTION__,"FATAL","caught exception");
    }
    objp->__copying = false;

    return *this;
}

/**
 * @brief  長さゼロの文字列をオブジェクトに格納 (NULL無しモード専用)
 *
 * @note       このメンバ関数はprivateです 
 */
void tstring::__init_non_null_str_rec()
{
    if ( this->str_buf_length_rec == 1 ) {
	this->_str_rec[0] = '\0';
    }
    else {
	if ( this->realloc_str_rec(1) < 0 ) {
	    if ( this->_str_rec != NULL ) {
		this->_str_rec[0] = '\0';
		this->str_buf_length_rec = 1;
	    }
	    err_throw(__FUNCTION__,"FATAL","realloc() failed");
	}
	if ( this->extptr_rec != NULL ) *(this->extptr_rec) = this->_str_rec;
	this->_str_rec[0] = '\0';
	this->str_buf_length_rec = 1;
    }

    return;
}

/**
 * @brief  長さゼロの文字列をオブジェクトに格納 (固定長バッファモード専用)
 *
 *  長さゼロの文字列をオブジェクトに格納する (固定長バッファモード専用) <br>
 *  引数 max_length は constructor で指定し，その他の場合は 0 を与える．
 *
 * @max_length  最大の文字列長(constructorの時に指定する) <br>
 *              0 の場合，現在の設定を改変せず，文字列だけ初期化する．
 * @note        このメンバ関数はprivateです 
 */
void tstring::__init_fixed_str_rec( size_t max_length )
{
    if ( 0 < max_length ) {

	/* realloc_str_rec() の中で使うので，先に値を入れておく */
	this->str_length_rec = 0;

	if ( this->realloc_str_rec(max_length + 1) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","this->realloc_str_rec() failed");
	}
	this->str_buf_length_rec = max_length + 1;
	this->_str_rec[0] = '\0';

    }
    else {

	this->_str_rec[0] = '\0';
	this->str_length_rec = 0;
	
    }

    return;
}


/**
 * @brief  オブジェクトのコピー
 *
 *  演算子の右側(引数) で指定したオブジェクト・文字列を自身に代入をします.
 *
 * @param      obj tstringクラスのオブジェクト
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 * @throw      メモリ破壊を起こした場合
 *
 */
tstring &tstring::operator=(const tstring &obj)
{
    this->init(obj);
    return *this;
}

/**
 * @brief  文字列の追加
 * 
 *  自身の文字列に，演算子の右側(引数) で指定した文字列の追加を行います.
 *
 * @param      obj tstringクラスのオブジェクト
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 *
 */
tstring &tstring::operator+=(const tstring &obj)
{
    if ( obj.cstr() != NULL ) {
        this->append(obj,0);
    }
    return *this;
}

/**
 * @brief  文字列の比較
 *
 *  自身の文字列と，演算子の右側(引数) で指定した文字列が
 *  一致しているかどうかの比較を行い，その結果を返します．
 *
 * @param      obj tstringクラスのオブジェクト
 * @return     文字列が一致した場合はtrue<br>
 *             文字列が不一致である場合はfalse
 * 
 */
bool tstring::operator==(const tstring &obj) const
{
    if ( this->compare(obj,0) == 0 ) return true;
    else return false;
}

/**
 * @brief  文字列の比較
 *
 *  自身の文字列と，演算子の右側(引数) で指定した文字列が<br>
 *  不一致かどうかの比較を行い，その結果を返します．
 *
 * @param      obj tstringクラスのオブジェクト
 * @return     文字列が不一致である場合はtrue<br>
 *             文字列が一致した場合はfalse
 * 
 */
bool tstring::operator!=(const tstring &obj) const
{
    if ( this->compare(obj,0) == 0 ) return false;
    else return true;
}

/* This is required by STL map */
/**
 * @brief  文字列の辞書的な比較
 *
 *  自身の文字列と,右辺ので指定した文字列とを辞書的に比較し，
 *  "自身 < 右辺" が成立した場合 true を返します．
 * 
 * @param   obj tstringクラスのオブジェクト
 * @return  真偽値
 * @note    STL の map に載せるために必要．
 */
bool tstring::operator<(const tstring &obj) const
{
    return ( this->compare(obj,0) < 0 );
}

/**
 * @brief  文字列の代入
 *
 *  演算子の右側(引数) で指定したオブジェクト・文字列を自身に代入をします.
 *
 * @param      str 文字列のアドレス
 * @return     内部バッファのアドレス
 * @throw      内部バッファの確保に失敗した場合
 * 
 */
const char *tstring::operator=(const char *str)
{
    if ( str != NULL ) {
        this->assign(str);
    }
    else {
        this->init();
    }
    return this->cstr();
}

/**
 * @brief  文字列の追加
 *
 *  自身の文字列に，演算子の右側(引数) で指定した文字列の追加を行います.
 *
 * @param      str 文字列のアドレス
 * @return     内部バッファのアドレス
 * @throw      内部バッファの確保に失敗した場合
 * 
 */
const char *tstring::operator+=(const char *str)
{
    if ( str != NULL ) {
        this->append(str);
    }
    return this->cstr();
}

/**
 * @brief  文字列の比較
 *
 *  自身の文字列と，演算子の右側(引数) で指定した文字列が
 *  一致しているかどうかの比較を行い，その結果を返します．
 *
 * @param      str 文字列のアドレス
 * @return     文字列が一致した場合はtrue<br>
 *             文字列が不一致である場合はfalse
 * 
 */
bool tstring::operator==(const char *str) const
{
    if ( this->compare(str) == 0 ) return true;
    else return false;
}

/**
 * @brief  文字列の比較
 *
 *  自身の文字列と，演算子の右側(引数) で指定した文字列が
 *  不一致かどうかの比較を行い，その結果を返します．
 *
 * @param      str 文字列のアドレス
 * @return     文字列が不一致である場合はtrue<br>
 *             文字列が一致した場合はfalse
 * 
 */
bool tstring::operator!=(const char *str) const
{
    if ( this->compare(str) == 0 ) return false;
    else return true;
}

/**
 * @brief  文字列の辞書的な比較
 *
 *  自身の文字列と,右辺ので指定した文字列とを辞書的に比較し，
 *  "自身 < 右辺" が成立した場合 true を返します．
 * 
 * @param   obj tstringクラスのオブジェクト
 * @return  真偽値
 */
bool tstring::operator<(const char *str) const
{
    return ( this->compare(str) < 0 );
}

#if 0
tstring::operator const char *() const
{
    return this->cstr();
}
#endif


/*
 * private member functions
 */

static unsigned char junk = '\0';

unsigned char &tstring::uchar_ref_after_resize( size_t pos )
{
    this->tstring::resize(pos + 1);
    return ((unsigned char *)this->str_ptr())[pos];
}

unsigned char &tstring::err_throw_uchar_ref( const char *func,
					     const char *lv, const char *mes )
{
    err_throw(func,lv,mes);
    return junk;		/* never */
}

const unsigned char &tstring::err_throw_const_uchar_ref( const char *func,
					const char *lv, const char *mes ) const
{
    err_throw(func,lv,mes);
    return junk;		/* never */
}


/*
 * public member functions
 *
 * 基本方針として，引数の文字列,ポインタ配列は object 内部で管理されている
 * 領域が与えられても動作する仕様とする．したがって，realloc() は基本的に
 * 使わない．realloc() を使う場合は，strdup() した文字列を参照するように
 * している．
 *
 */

/**
 * @brief  ユーザのポインタ変数の登録
 *
 *  ユーザのポインタ変数のアドレスを extptr_address に与えると，ユーザのポイン
 *  タ変数がオブジェクトに登録され，オブジェクトが持つ文字列の先頭アドレスを
 *  常にユーザのポインタ変数に保持させておくことができます．
 * 
 * @param   extptr_address ユーザのポインタ変数のアドレス
 * @return  自身の参照
 * @attention  ユーザのポインタ変数を登録すると shallow copy が機能しなくなり
 *             ます．
 */
tstring &tstring::register_extptr(char **extptr_address)
{
    this->extptr_rec = extptr_address;
    if ( this->extptr_rec != NULL ) *(this->extptr_rec) = this->str_ptr();
    return *this;
}

/**
 * @brief  ユーザのポインタ変数(読み取り専用)の登録
 *
 *  ユーザのポインタ変数のアドレスを extptr_address に与えると，ユーザのポイン
 *  タ変数がオブジェクトに登録され，オブジェクトが持つ文字列の先頭アドレスを
 *  常にユーザのポインタ変数に保持させておくことができます．
 * 
 * @param   extptr_address ユーザのポインタ変数のアドレス
 * @return  自身の参照
 * @attention  ユーザのポインタ変数を登録すると shallow copy が機能しなくなり
 *             ます．
 */
tstring &tstring::register_extptr(const char **extptr_address)
{
    this->extptr_rec = (char **)extptr_address;
    if ( this->extptr_rec != NULL ) *(this->extptr_rec) = this->str_ptr();
    return *this;
}

/**
 * @brief  オブジェクトの初期化
 *
 *  自身の初期化を行います．
 *
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 * 
 */
tstring &tstring::init()
{
    this->regex_rec.init();

    if ( this->str_length_rec != UNDEF ) {	/* 固定長バッファの場合 */

	this->__init_fixed_str_rec(0);

	this->init_reg_results();

    }
    else {

	/* shallow copy 関係のクリーンアップ */
	this->cleanup_shallow_copy(false);

	/* 強制初期化 */
	this->__force_free();
	this->__force_init(false);

	if ( this->str_rec_keeps_non_null == true ) {
	    this->__init_non_null_str_rec();
	}

	if ( this->extptr_rec != NULL ) *(this->extptr_rec) = this->str_ptr();
    }

    return *this;
}

/**
 * @brief  オブジェクトのコピー
 *
 *  引数 obj の内容で自身を初期化します．
 *
 * @param      obj コピー元となる文字列を持つオブジェクト
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 * @throw      メモリ破壊を起こした場合
 * 
 */
tstring &tstring::init(const tstring &obj)
{
    if ( &obj == this ) return *this;

    debug_report_scopy1("arg: obj = %zx",(size_t)(&obj));

    this->regex_rec.init();

    if ( this->str_length_rec != UNDEF ) {	/* 固定長バッファの場合 */

	this->__deep_init(obj);

    }
    else {

	bool do_shallow_copy = false;

	/* shallow copy 関係のクリーンアップ */
	this->cleanup_shallow_copy(false);

	if ( obj._str_rec != NULL && this->extptr_rec == NULL ) {
	    do_shallow_copy = obj.request_shallow_copy(this);
	}
	if ( do_shallow_copy == true ) this->__shallow_init(obj,false);
	else this->__deep_init(obj);
	     
    }

    return *this;
}

/**
 * @brief  文字列長の変更
 *
 *  自身が持つ文字列の長さをlen に変更します．
 *
 * @param      len 変更後の文字列長
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 * 
 */
tstring &tstring::resize( size_t len )
{
    if ( len < this->length() ) {
	tstring::replace( len, this->length() - len, ' ', (size_t)0 );
    }
    else {
	tstring::replace( this->length(), 0, ' ', len - this->length() );
    }
    return *this;
}

/**
 * @brief  相対的な文字列長の変更
 *
 *  自身が持つ文字列の長さをlen の長さ分だけ変更します．
 *
 * @param      len 文字列長の増分・減分
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 * 
 */
tstring &tstring::resizeby( ssize_t len )
{
    size_t new_len, a_len;
    a_len = ((len < 0) ? -len : len);
    if ( len < 0 ) {
	if ( a_len < this->length() ) 
	    new_len = this->length() - a_len;
	else
	    new_len = 0;
    }
    else {
	new_len = this->length() + a_len;
    }

    return this->resize(new_len);
}

/**
 * @brief  文字列全体を指定キャラクタでパディング
 *
 *  自身が持つ文字列全体を，文字ch でパディングします．
 *
 * @param      ch 文字列をパディングする文字
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 * 
 */
tstring &tstring::clean( int ch )
{
    if ( this->str_length_rec != UNDEF ) {	/* 固定長バッファの場合 */
	size_t i;
	for ( i=0 ; i < this->length() ; i++ ) {
	    ((unsigned char *)this->_str_rec)[i] = ch;
	}
	this->_str_rec[i] = '\0';
    }
    else {
	tstring::replace(0,this->length(), ch,this->length());
    }
    return *this;
}

/**
 * @brief  printf()の記法で文字列を代入
 *
 *  自身の文字列を，引数で指定された文字列で初期化します．
 *
 * @param      format 源泉となる文字列のためのフォーマット指定
 * @param      ... formatに対応した可変長引数の各要素データ
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 * @throw      可変長引数の各要素データが指定された変換フォーマットで変換
 *             できない値の場合
 * 
 */
tstring &tstring::printf( const char *format, ... )
{
    va_list ap;
    va_start(ap,format);
    try {
	this->vassignf(format,ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vassignf() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  printf()の記法で文字列を代入
 *
 *  自身の文字列を，引数で指定された文字列で初期化します．
 *
 * @param      format 源泉となる文字列のためのフォーマット指定
 * @param      ap formatに対応した可変長引数のリスト
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 * @throw      可変長引数の各要素データが指定された変換フォーマットで変換
 *             できない値の場合
 * 
 */
tstring &tstring::vprintf( const char *format, va_list ap )
{
    return this->vassignf(format,ap);
}

/**
 * @brief  文字列，部分文字列の代入
 *
 *  自身の文字列を，引数で指定された文字列で初期化します．
 *
 * @param      src 源泉となる文字列を持つtstringクラスのオブジェクト
 * @param      pos2 src が持つ文字列の開始位置
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 * 
 */
tstring &tstring::assign( const tstring &src, size_t pos2 )
{
    return this->assign(src,pos2,src.length());
}

/**
 * @brief  部分文字列の代入
 *
 *  自身の文字列を，引数で指定された文字列で初期化します．
 *
 * @param      src 源泉となる文字列を持つtstringクラスのオブジェクト
 * @param      pos2 src が持つ文字列の開始位置
 * @param      n2 書き込まれる文字列の長さ
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 * 
 */
tstring &tstring::assign( const tstring &src, size_t pos2, size_t n2 )
{
    return tstring::replace(0,this->length(), src,pos2,n2);
}

/**
 * @brief  printf()の記法で文字列を代入
 *
 *  自身の文字列を，引数で指定された文字列で初期化します．
 *
 * @param      format 源泉となる文字列のためのフォーマット指定
 * @param      ... formatに対応した可変長引数の各要素データ
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 * @throw      可変長引数の各要素データが指定された変換フォーマットで変換
 *             できない値の場合
 * 
 */
tstring &tstring::assignf( const char *format, ... )
{
    va_list ap;
    va_start(ap,format);
    try {
	this->vassignf(format,ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vassignf() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  printf()の記法で文字列を代入
 *
 *  自身の文字列を，引数で指定された文字列で初期化します．
 *
 * @param      format 源泉となる文字列のためのフォーマット指定
 * @param      ap formatに対応した可変長引数のリスト
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 * @throw      可変長引数の各要素データが指定された変換フォーマットで変換
 *             できない値の場合
 * 
 */
tstring &tstring::vassignf( const char *format, va_list ap )
{
    if ( this->str_length_rec != UNDEF ) {	/* 固定長バッファの場合 */
	int l;
	if ( format == NULL ) return *this;
	l = c_vsnprintf(this->_str_rec, this->str_buf_length_rec, format, ap);
	if ( l < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","c_vsnprintf() failed");
	}
	if ( this->str_buf_length_rec <= (size_t)l ) this->str_length_rec = this->str_buf_length_rec-1;
	else this->str_length_rec = l;
    }
    else {
	heap_mem<char> work_buf;
	if ( format == NULL ) return this->assign(format);
	if ( c_vasprintf(work_buf.ptr_address(), format, ap) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","c_vasprintf() failed");
	}
	this->assign(work_buf.ptr());
    }
    return *this;
}

/**
 * @brief  文字列の代入
 *
 *  自身の文字列を，引数で指定された文字列で初期化します．
 *
 * @param      str 源泉となる文字列
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 * 
 */
tstring &tstring::assign( const char *str )
{
    if ( this->str_length_rec != UNDEF ) {	/* 固定長バッファの場合 */
	size_t i;
	if ( str == NULL ) return *this;
	for ( i=0 ; str[i] != '\0' && i+1 < this->str_buf_length_rec ; i++ ) {
	    this->_str_rec[i] = str[i];
	}
	this->_str_rec[i] = '\0';
	this->str_length_rec = i;
    }
    else {
	if ( str == NULL ) this->assign( str, (size_t)0 );
	else this->assign( str, c_strlen(str) );
    }
    return *this;
}

/**
 * @brief  文字列の代入
 *
 *  自身の文字列を，引数で指定された文字列で初期化します．
 *
 * @param      str 源泉となる文字列
 * @param      n strの長さ
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 * 
 */
tstring &tstring::assign( const char *str, size_t n )
{
    if ( this->str_length_rec != UNDEF ) {	/* 固定長バッファの場合 */
	size_t i;
	if ( str == NULL && 0 < n ) return *this;	/* invalid */
	for ( i=0 ; i < n && str[i] != '\0' && i+1 < this->str_buf_length_rec ; i++ ) {
	    this->_str_rec[i] = str[i];
	}
	this->_str_rec[i] = '\0';
	this->str_length_rec = i;
    }
    else {
	tstring::replace(0,this->length(), str,n);
    }
    return *this;
}

/**
 * @brief  文字 ch が n 個連続した文字列を代入
 *
 *  自身の文字列を，引数で指定された文字列で初期化します．
 *
 * @param      ch 源泉となる文字
 * @param      n chの個数
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 * 
 */
tstring &tstring::assign( int ch, size_t n )
{
    if ( this->str_length_rec != UNDEF ) {	/* 固定長バッファの場合 */
	size_t i;
	for ( i=0 ; i < n && i+1 < this->str_buf_length_rec ; i++ ) {
	    ((unsigned char *)this->_str_rec)[i] = ch;
	}
	this->_str_rec[i] = '\0';
	this->str_length_rec = i;
    }
    else {
	tstring::replace(0,this->length(), ch,n);
    }
    return *this;
}

#ifdef TSTRING__USE_SOLO_NARG
tstring &tstring::assign( size_t n )
{
    return this->assign(' ',n);
}
#endif

/**
 * @brief  printf()の記法で指定された文字列により，指定位置から上書き
 *
 *  自身の文字列を位置pos から，引数で指定された文字列で上書きします．
 *
 * @param      pos オブジェクト内の文字列の開始位置
 * @param      format 源泉となる文字列のためのフォーマット指定
 * @param      ... formatに対応した可変長引数の各要素データ
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 * @throw      可変長引数の各要素データが指定された変換フォーマットで変換
 *             できない値の場合
 * 
 */
tstring &tstring::putf( size_t pos, const char *format, ... )
{
    va_list ap;
    va_start(ap,format);
    try {
	this->vputf(pos,format,ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vputf() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  printf()の記法で指定された文字列により，指定位置から上書き
 *
 *  自身の文字列を位置pos から，引数で指定された文字列で上書きします．
 *
 * @param      pos オブジェクト内の文字列の開始位置
 * @param      format 源泉となる文字列のためのフォーマット指定
 * @param      ap formatに対応した可変長引数のリスト
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 * @throw      可変長引数の各要素データが指定された変換フォーマットで変換
 *             できない値の場合
 * 
 */
tstring &tstring::vputf( size_t pos, const char *format, va_list ap )
{
    heap_mem<char> work_buf;
    if ( format == NULL ) return this->put(pos,format);
    if ( c_vasprintf(work_buf.ptr_address(), format, ap) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","c_vasprintf() failed");
    }
    this->put(pos, work_buf.ptr());
    return *this;
}

/**
 * @brief  指定された文字列により，指定位置から上書き
 *
 *  自身の文字列を位置 pos から，引数で指定された文字列で上書きします．
 *
 * @param      pos オブジェクト内の文字列の開始位置
 * @param      str 源泉となる文字列
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 * 
 */
tstring &tstring::put( size_t pos, const char *str )
{
    if ( this->str_length_rec != UNDEF ) {	/* 固定長バッファの場合 */
	size_t i;
	if ( this->str_length_rec < pos ) {
	    for ( i=this->str_length_rec ; i < pos && i+1 < this->str_buf_length_rec ; i++ )
		this->_str_rec[i]=' ';
	    this->_str_rec[i] = '\0';
	    this->str_length_rec = i;
	}
	if ( pos + 1 < this->str_buf_length_rec ) {
	    if ( str == NULL ) return *this;
	    for (i=0 ; str[i] != '\0' && pos+i+1 < this->str_buf_length_rec ; i++) {
		this->_str_rec[pos+i] = str[i];
	    }
	    if ( this->str_length_rec < pos+i ) {
		this->_str_rec[pos+i] = '\0';
		this->str_length_rec = pos+i;
	    }
	}
    }
    else {
	if ( str != NULL ) this->put(pos,str,c_strlen(str));
	else this->put(pos,str,(size_t)0);
    }
    return *this;
}

/**
 * @brief  指定された文字列により，指定位置から上書き
 *
 *  自身の文字列を位置pos から，引数で指定された文字列で上書きします．
 *
 * @param      pos オブジェクト内の文字列の開始位置
 * @param      str 源泉となる文字列
 * @param      n strの長さ
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 * 
 */
tstring &tstring::put( size_t pos, const char *str, size_t n )
{
    if ( this->str_length_rec != UNDEF ) {	/* 固定長バッファの場合 */
	size_t i;
	if ( str == NULL && 0 < n ) return *this;	/* invalid */
	if ( this->str_length_rec < pos ) {
	    for ( i=this->str_length_rec ; i < pos && i+1 < this->str_buf_length_rec ; i++ )
		this->_str_rec[i]=' ';
	    this->_str_rec[i] = '\0';
	    this->str_length_rec = i;
	}
	if ( pos + 1 < this->str_buf_length_rec ) {
	    for (i=0 ; i<n && str[i]!='\0' && pos+i+1 < this->str_buf_length_rec ; i++) {
		this->_str_rec[pos+i] = str[i];
	    }
	    if ( this->str_length_rec < pos+i ) {
		this->_str_rec[pos+i] = '\0';
		this->str_length_rec = pos+i;
	    }
	}
    }
    else {
	const char *src_ptr = str;
	heap_mem<char> work_buf;
	size_t i;

	if ( 0 < n && str == NULL ) return *this;

	for ( i=0 ; i < n && str[i] != '\0' ; i++ );
	n = i;

	if ( 0 < n && this->is_my_buffer(str) == true ) {
	    if ( work_buf.allocate(n) < 0 ) {
		err_throw(__FUNCTION__,"FATAL","malloc() failed");
	    }
	    c_memcpy(work_buf.ptr(), str, n);
	    src_ptr = work_buf.ptr();
	}

	if ( this->length() < pos + n )
	    tstring::replace(this->length(),0, ' ',(pos + n) - this->length());
	tstring::replace(pos,n, src_ptr,n);

    }
    return *this;
}

/**
 * @brief  文字 ch が n 個連続した文字列により，指定位置から上書き
 *
 *  自身の文字列を位置pos から，引数で指定された文字列で上書きします．
 *
 * @param      pos オブジェクト内の文字列の開始位置
 * @param      ch 源泉となる文字
 * @param      n chの個数
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 * 
 */
tstring &tstring::put( size_t pos, int ch, size_t n )
{
    if ( this->str_length_rec != UNDEF ) {	/* 固定長バッファの場合 */
	size_t i;
	if ( this->str_length_rec < pos ) {
	    for ( i=this->str_length_rec ; i < pos && i+1 < this->str_buf_length_rec ; i++ )
		this->_str_rec[i]=' ';
	    this->_str_rec[i] = '\0';
	    this->str_length_rec = i;
	}
	if ( pos + 1 < this->str_buf_length_rec ) {
	    for ( i=0 ; i < n && pos+i+1 < this->str_buf_length_rec ; i++ ) {
		((unsigned char *)this->_str_rec)[pos+i] = ch;
	    }
	    if ( this->str_length_rec < pos+i ) {
		this->_str_rec[pos+i] = '\0';
		this->str_length_rec = pos+i;
	    }
	}
    }
    else {
	if ( this->length() < pos + n )
	    tstring::replace(this->length(),0, ' ',(pos + n) - this->length());
	tstring::replace(pos,n, ch,n);
    }
    return *this;
}

#ifdef TSTRING__USE_SOLO_NARG
tstring &tstring::put( size_t pos, size_t n )
{
    return this->put(pos,' ',n);
}
#endif

/**
 * @brief  指定された文字列により，指定位置から上書き
 *
 *  自身の文字列を位置pos1 から，引数で指定された文字列で上書きします．
 *
 * @param      pos1 オブジェクト内の文字列の開始位置
 * @param      src 源泉となる文字列を持つtstringクラスのオブジェクト
 * @param      pos2 src が持つ文字列の開始位置
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 * 
 */
tstring &tstring::put( size_t pos1, const tstring &src, size_t pos2 )
{
    return this->put(pos1, src, pos2, src.length());
}

/**
 * @brief  指定された文字列により，指定位置から上書き
 *
 *  自身の文字列を位置pos1 から，引数で指定された文字列で上書きします．
 *
 * @param      pos1 オブジェクト内の文字列の開始位置
 * @param      src 源泉となる文字列を持つtstringクラスのオブジェクト
 * @param      pos2 src が持つ文字列の開始位置
 * @param      n2 書き込まれる文字列の長さ
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 * 
 */
tstring &tstring::put( size_t pos1, const tstring &src, size_t pos2, size_t n2)
{
    return tstring::replace(pos1,n2, src,pos2,n2);
}

/**
 * @brief  文字列の追加
 *
 *  自身の文字列に，引数で指定された文字列を追加します．
 *
 * @param      str 源泉となる文字列
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 * 
 */
tstring &tstring::strcat( const char *str )
{
    return this->append( str );
}

/**
 * @brief  部分文字列の追加
 *
 *  自身の文字列に，引数で指定された文字列を追加します．
 *
 * @param      str 源泉となる文字列
 * @param      n strの長さ
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 * 
 */
tstring &tstring::strncat( const char *str, size_t n )
{
    return this->append( str, n );
}

/**
 * @brief  文字列・部分文字列の追加
 *
 *  自身の文字列に，引数で指定された文字列を追加します．
 *
 * @param      src 源泉となる文字列を持つtstringクラスのオブジェクト
 * @param      pos2 srcが持つ文字列の開始位置
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 * 
 */
tstring &tstring::strcat(const tstring &src, size_t pos2 )
{
    return this->append(src,pos2,src.length());
}

/**
 * @brief  部分文字列の追加
 *
 *  自身の文字列に，引数で指定された文字列を追加します．
 *
 * @param      src 源泉となる文字列を持つtstringクラスのオブジェクト
 * @param      pos2 srcが持つ文字列の開始位置
 * @param      n2 追加する文字列の長さ
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 * 
 */
tstring &tstring::strncat(const tstring &src, size_t pos2, size_t n2 )
{
    return tstring::replace(this->length(),0, src,pos2,n2);
}

/**
 * @brief  printf()の記法で指定された文字列を追加
 *
 *  自身の文字列に，引数で指定された文字列を追加します．
 *
 * @param      format 源泉となる文字列のためのフォーマット指定
 * @param      ... formatに対応した可変長引数の各要素データ
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 * @throw      可変長引数の各要素データが指定された変換フォーマットで変換
 *             できない値の場合
 * 
 */
tstring &tstring::appendf( const char *format, ... )
{
    va_list ap;
    va_start(ap,format);
    try {
	this->vappendf(format,ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vappendf() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  printf()の記法で指定された文字列を追加
 *
 *  自身の文字列に，引数で指定された文字列を追加します．
 *
 * @param      format 源泉となる文字列のためのフォーマット指定
 * @param      ap formatに対応した可変長引数のリスト
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 * @throw      可変長引数の各要素データが指定された変換フォーマットで変換
 *             できない値の場合
 * 
 */
tstring &tstring::vappendf( const char *format, va_list ap )
{
    if ( this->str_length_rec != UNDEF ) {	/* 固定長バッファの場合 */
	size_t pos = this->str_length_rec;
	if ( pos + 1 < this->str_buf_length_rec ) {
	    int l;
	    if ( format == NULL ) return *this;
	    l = c_vsnprintf(this->_str_rec+pos,this->str_buf_length_rec-pos,format,ap);
	    if ( l < 0 ) {
		err_throw(__FUNCTION__,"FATAL","c_vsnprintf() failed");
	    }
	    if ( this->str_buf_length_rec <= pos + l ) {
		this->str_length_rec = this->str_buf_length_rec-1;
	    }
	    else this->str_length_rec = pos + l;
	}
    }
    else {
	heap_mem<char> work_buf;
	if ( format == NULL ) return this->append(format);
	if ( c_vasprintf(work_buf.ptr_address(), format, ap) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","c_vasprintf() failed");
	}
	this->append(work_buf.ptr());
    }
    return *this;
}

/**
 * @brief  文字列の追加
 *
 *  自身の文字列に，引数で指定された文字列を追加します．
 *
 * @param      str 源泉となる文字列
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 * 
 */
tstring &tstring::append( const char *str )
{
    if ( this->str_length_rec != UNDEF ) {	/* 固定長バッファの場合 */
	size_t pos = this->str_length_rec;
	if ( pos + 1 < this->str_buf_length_rec ) {
	    size_t i;
	    if ( str == NULL ) return *this;
	    for ( i=0 ; str[i] != '\0' && pos+i+1 < this->str_buf_length_rec ; i++ ) {
		this->_str_rec[pos+i] = str[i];
	    }
	    this->_str_rec[pos+i] = '\0';
	    this->str_length_rec = pos+i;
	}
    }
    else {
	if ( str == NULL ) this->append( str, (size_t)0 );
	else this->append( str, c_strlen(str) );
    }
    return *this;
}

/**
 * @brief  文字列・部分文字列の追加
 *
 *  自身の文字列に，引数で指定された文字列を追加します．
 *
 * @param      src 源泉となる文字列を持つtstringクラスのオブジェクト
 * @param      pos2 srcが持つ文字列の開始位置
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 * 
 */
tstring &tstring::append(const tstring &src, size_t pos2 )
{
    return this->append(src,pos2,src.length());
}

/**
 * @brief  部分文字列の追加
 *
 *  自身の文字列に，引数で指定された文字列を追加します．
 *
 * @param      src 源泉となる文字列を持つtstringクラスのオブジェクト
 * @param      pos2 srcが持つ文字列の開始位置
 * @param      n2 追加する文字列の長さ
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 * 
 */
tstring &tstring::append(const tstring &src, size_t pos2, size_t n2 )
{
    return tstring::replace(this->length(),0, src,pos2,n2);
}

/**
 * @brief  文字列の追加
 *
 *  自身の文字列に，引数で指定された文字列を追加します．
 *
 * @param      str 源泉となる文字列
 * @param      n strの長さ
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 * 
 */
tstring &tstring::append( const char *str, size_t n )
{
    if ( this->str_length_rec != UNDEF ) {	/* 固定長バッファの場合 */
	size_t pos = this->str_length_rec;
	if ( pos + 1 < this->str_buf_length_rec ) {
	    size_t i;
	    if ( str == NULL ) return *this;
	    for (i=0 ; i<n && str[i]!='\0' && pos+i+1 < this->str_buf_length_rec ; i++) {
		this->_str_rec[pos+i] = str[i];
	    }
	    this->_str_rec[pos+i] = '\0';
	    this->str_length_rec = pos+i;
	}
    }
    else {
	tstring::replace(this->length(),0, str,n);
    }
    return *this;
}

/**
 * @brief  文字 ch が n 個連続した文字列を追加
 *
 *  自身の文字列に，引数で指定された文字列を追加します．
 *
 * @param      ch 源泉となる文字
 * @param      n chの個数
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 * 
 */
tstring &tstring::append( int ch, size_t n )
{
    if ( this->str_length_rec != UNDEF ) {	/* 固定長バッファの場合 */
	size_t pos = this->str_length_rec;
	if ( pos + 1 < this->str_buf_length_rec ) {
	    size_t i;
	    for ( i=0 ; i < n && pos+i+1 < this->str_buf_length_rec ; i++ ) {
		((unsigned char *)this->_str_rec)[pos+i] = ch;
	    }
	    this->_str_rec[pos+i] = '\0';
	    this->str_length_rec = pos+i;
	}
    }
    else {
	tstring::replace(this->length(),0, ch,n);
    }
    return *this;
}

#ifdef TSTRING__USE_SOLO_NARG
tstring &tstring::append( size_t n )
{
    return this->append(' ',n);
}
#endif

/**
 * @brief  printf()の記法で指定された文字列を指定位置へ挿入
 *
 *  自身の文字列の位置 pos1 に，引数で指定された文字列を挿入します．
 *
 * @param      pos1 オブジェクト内の文字列の開始位置
 * @param      format 源泉となる文字列のためのフォーマット指定
 * @param      ... formatに対応した可変長引数の各要素データ
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 * @throw      可変長引数の各要素データが指定された変換フォーマットで変換
 *             できない値の場合
 * 
 */
tstring &tstring::insertf( size_t pos1, const char *format, ... )
{
    va_list ap;
    va_start(ap,format);
    try {
	this->vinsertf(pos1,format,ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vinsertf() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  printf()の記法で指定された文字列を指定位置へ挿入
 *
 *  自身の文字列の位置 pos1 に，引数で指定された文字列を挿入します．
 *
 * @param      pos1 オブジェクト内の文字列の開始位置
 * @param      format 源泉となる文字列のためのフォーマット指定
 * @param      ap formatに対応した可変長引数のリスト
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 * @throw      可変長引数の各要素データが指定された変換フォーマットで変換
 *             できない値の場合
 * 
 */
tstring &tstring::vinsertf( size_t pos1, const char *format, va_list ap )
{
    heap_mem<char> work_buf;
    if ( format == NULL ) return this->insert(pos1,format);
    if ( c_vasprintf(work_buf.ptr_address(), format, ap) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","c_vasprintf() failed");
    }
    tstring &return_ref = this->insert(pos1, work_buf.ptr());
    return return_ref;
}

/**
 * @brief  文字列の指定位置への挿入
 *
 *  自身の文字列の位置pos1 に，引数で指定された文字列を挿入します．
 *
 * @param      pos1 オブジェクト内の文字列の開始位置
 * @param      str 源泉となる文字列
 * @param      n strの長さ
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 * 
 */
tstring &tstring::insert( size_t pos1, const char *str, size_t n )
{
    return tstring::replace(pos1,0, str,n);
}

/**
 * @brief  文字列の指定位置への挿入
 *
 *  自身の文字列の位置pos1 に，引数で指定された文字列を挿入します．
 *
 * @param      pos1 オブジェクト内の文字列の開始位置
 * @param      str 源泉となる文字列
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 * 
 */
tstring &tstring::insert( size_t pos1, const char *str )
{
    if ( str == NULL ) return this->insert( pos1, str, (size_t)0 );
    else return this->insert( pos1, str, c_strlen(str) );
}

/**
 * @brief  文字列・部分文字列の指定位置への挿入
 *
 *  自身の文字列の位置pos1 に，引数で指定された文字列を挿入します．
 *
 * @param      pos1 オブジェクト内の文字列の開始位置
 * @param      src 源泉となる文字列を持つtstringクラスのオブジェクト
 * @param      pos2 srcが持つ文字列の開始位置
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 * 
 */
tstring &tstring::insert( size_t pos1, const tstring &src, size_t pos2 )
{
    return this->insert(pos1, src, pos2, src.length());
}

/**
 * @brief  部分文字列の指定位置への挿入
 *
 *  自身の文字列の位置pos1 に，引数で指定された文字列を挿入します．
 *
 * @param      pos1 オブジェクト内の文字列の開始位置
 * @param      src 源泉となる文字列を持つtstringクラスのオブジェクト
 * @param      pos2 srcが持つ文字列の開始位置
 * @param      n2 挿入される文字列の長さ
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 * 
 */
tstring &tstring::insert( size_t pos1, 
			  const tstring &src, size_t pos2, size_t n2 )
{
    return tstring::replace(pos1,0, src,pos2,n2);
}

/**
 * @brief  文字 ch が n 個連続した文字列の指定位置への挿入
 *
 *  自身の文字列の位置pos1 に，引数で指定された文字列を挿入します．
 *
 * @param      pos1 オブジェクト内の文字列の開始位置
 * @param      ch 源泉となる文字
 * @param      n chの個数
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 * 
 */
tstring &tstring::insert( size_t pos1, int ch, size_t n )
{
    return tstring::replace(pos1,0, ch,n);
}

#ifdef TSTRING__USE_SOLO_NARG
tstring &tstring::insert( size_t pos1, size_t n )
{
    return this->insert(pos1,' ',n);
}
#endif

/**
 * @brief  指定部分を printf()の記法で指定された文字列で置換
 *
 *  自身の文字列の位置pos1 からn1 文字を，指定された文字列で置き換えます．
 *
 * @param      pos1 オブジェクト内の文字列の開始位置
 * @param      n1 置換する文字数
 * @param      format 源泉となる文字列のためのフォーマット指定
 * @param      ... formatに対応した可変長引数の各要素データ
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 * @throw      可変長引数の各要素データが指定された変換フォーマットで変換
 *             できない値の場合
 * 
 */
tstring &tstring::replacef( size_t pos1, size_t n1, const char *format, ... )
{
    va_list ap;
    va_start(ap,format);
    try {
	this->vreplacef(pos1,n1,format,ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","this->vreplacef() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  指定部分を printf()の記法で指定された文字列で置換
 *
 *  自身の文字列の位置pos1 からn1 文字を，指定された文字列で置き換えます．
 *
 * @param      pos1 オブジェクト内の文字列の開始位置
 * @param      n1 置換する文字数
 * @param      format 源泉となる文字列のためのフォーマット指定
 * @param      ap formatに対応した可変長引数のリスト
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 * @throw      可変長引数の各要素データが指定された変換フォーマットで変換
 *             できない値の場合
 * 
 */
tstring &tstring::vreplacef( size_t pos1, size_t n1, const char *format, va_list ap )
{
    heap_mem<char> work_buf;
    if ( format == NULL ) return this->replace(pos1,n1,format);
    if ( c_vasprintf(work_buf.ptr_address(), format, ap) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","c_vasprintf() failed");
    }
    tstring &return_ref = this->replace(pos1, n1, work_buf.ptr());
    return return_ref;
}

/**
 * @brief  指定部分の文字列の置換
 *
 *  自身の文字列の位置pos1 からn1 文字を，指定された文字列で置き換えます．
 *
 * @param      pos1 オブジェクト内の文字列の開始位置
 * @param      n1 置換する文字数
 * @param      str 源泉となる文字列
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 * 
 */
tstring &tstring::replace( size_t pos1, size_t n1, const char *str )
{
    if ( str == NULL ) return this->replace( pos1, n1, str, (size_t)0 );
    else return this->replace( pos1, n1, str, c_strlen(str) );
}

/**
 * @brief  指定部分の文字列の置換
 *
 *  自身の文字列の位置pos1 からn1 文字を，指定された文字列で置き換えます．
 *
 * @param      pos1 オブジェクト内の文字列の開始位置
 * @param      n1 置換する文字数
 * @param      src 源泉となる文字列を持つtstringクラスのオブジェクト
 * @param      pos2 srcが持つ文字列の開始位置
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 * 
 */
tstring &tstring::replace( size_t pos1, size_t n1, 
			   const tstring &src, size_t pos2 )
{
    return this->replace(pos1,n1,src,pos2,src.length());
}

/**
 * @brief  指定部分の文字列の置換
 *
 *  自身の文字列の位置pos1 からn1 文字を，指定された文字列で置き換えます．
 *
 * @param      pos1 オブジェクト内の文字列の開始位置
 * @param      n1 置換する文字数
 * @param      src 源泉となる文字列を持つtstringクラスのオブジェクト
 * @param      pos2 srcが持つ文字列の開始位置
 * @param      n2 srcが持つ文字列の長さ
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 * 
 */
tstring &tstring::replace( size_t pos1, size_t n1, 
			   const tstring &src, size_t pos2, size_t n2 )
{
    size_t len = src.length();
    if ( pos2 <= len ) {
	size_t i;
	const char *src_buf = src.str_ptr_cs() + pos2;
	size_t max = len - pos2;
	heap_mem<char> work_buf;
	if ( max < n2 ) n2 = max;
	if ( this->str_length_rec == UNDEF ) {	/* 可変長バッファの場合のみ */
	    if ( &src == this ) {		/* 引数が自身の場合は退避 */
		if ( work_buf.allocate(n2) < 0 ) {
		    err_throw(__FUNCTION__,"FATAL","malloc() failed");
		}
		c_memcpy(work_buf.ptr(), src_buf, n2);
		src_buf = work_buf.ptr();
	    }
	}
	this->replace(pos1,n1, ' ',n2);
	/* バイナリデータの場合にも対応させる */
	char *dest_buf = this->str_ptr();
	for ( i=0 ; i < n2 && pos1 + i < this->length() ; i++ ) {
	    dest_buf[pos1 + i] = src_buf[i];
	}
    }
    return *this;
}

/**
 * @brief  指定部分の文字列の置換
 *
 *  自身の文字列の位置pos1 からn1 文字を，指定された文字列で置き換えます．
 *
 * @param      pos1 オブジェクト内の文字列の開始位置
 * @param      n1 置換する文字数
 * @param      str 源泉となる文字列
 * @param      n2 strが持つ文字列の長さ
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 * 
 */
tstring &tstring::replace( size_t pos1, size_t n1, const char *str, size_t n2 )
{
    const char *src_ptr = str;
    heap_mem<char> work_buf;
    size_t new_length;
    size_t i;

    if ( 0 < n2 && str == NULL ) return *this;

    /* n2 より小さい場合の対応 */
    for ( i=0 ; i < n2 && str[i] != '\0' ; i++ );
    n2 = i;

    if ( this->str_length_rec == UNDEF ) {	/* 可変長バッファの場合のみ */
	if ( 0 < n2 && this->is_my_buffer(str) == true ) {
	    /* str が this->_str_rec の内容を指している場合  */
	    if ( work_buf.allocate(n2) < 0 ) {
		err_throw(__FUNCTION__,"FATAL","malloc() failed");
	    }
	    c_memcpy(work_buf.ptr(), str, n2);
	    src_ptr = work_buf.ptr();
	}
    }

    if ( this->length() < pos1 ) pos1 = this->length();
    if ( this->length() < pos1 + n1 ) n1 = this->length() - pos1;

    if ( n1 < n2 ) {
	new_length = this->length() + (n2 - n1);
    }
    else if ( n2 < n1 ) {
	new_length = this->length() - (n1 - n2);
	c_memmove(this->str_ptr() + (pos1 + n2),
		  this->str_ptr() + (pos1 + n1),
		  this->length() - pos1 - n1);
    }
    else new_length = this->length();

    if ( this->str_length_rec != UNDEF ) {	/* 固定長バッファの場合 */
	if ( n1 < n2 ) {
	    if ( (pos1 + n2) + 1 < this->str_buf_length_rec ) {
		size_t sz = this->length() - pos1 - n1;
		if ( this->str_buf_length_rec < (pos1 + n2) + sz + 1 ) {
		    sz = this->str_buf_length_rec - 1 - (pos1 + n2);
		}
		c_memmove(this->_str_rec + (pos1 + n2),
			  this->_str_rec + (pos1 + n1),
			  sz);
	    }
	}
	for ( i=0 ; i < n2 && pos1 + i + 1 < this->str_buf_length_rec ; i++ ) {
	    this->_str_rec[pos1 + i] = src_ptr[i];
	}
	if ( this->str_buf_length_rec < new_length + 1 ) new_length = this->str_buf_length_rec - 1;
	this->_str_rec[new_length] = '\0';

	this->str_length_rec = new_length;
    }
    else {					/* 可変長バッファの場合 */
	if ( new_length != this->length() || this->str_ptr() == NULL ) {
	    if ( this->realloc_str_rec(new_length + 1) < 0 ) {
		err_throw(__FUNCTION__,"FATAL","realloc() failed");
	    }
	    if ( this->extptr_rec != NULL ) *(this->extptr_rec) = this->str_ptr();
	}
	char *this_str_ptr = this->str_ptr();
	if ( n1 < n2 ) {
	    c_memmove(this_str_ptr + (pos1 + n2), this_str_ptr + (pos1 + n1),
		      this->length() - pos1 - n1);
	}
	for ( i=0 ; i < n2 ; i++ ) {
	    this_str_ptr[pos1 + i] = src_ptr[i];
	}
	this_str_ptr[new_length] = '\0';

	this->str_buf_length_rec = new_length + 1;
    }

    return *this;
}

/**
 * @brief  指定部分の文字列を，文字 ch が n 個連続した文字列で置換
 *
 *  自身の文字列の位置pos1 からn1 文字を，指定された文字列で置き換えます．
 *
 * @param      pos1 オブジェクト内の文字列の開始位置
 * @param      n1 置換する文字数
 * @param      ch 源泉となる文字
 * @param      n2 chの個数
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 * 
 */
tstring &tstring::replace( size_t pos1, size_t n1, int ch, size_t n2 )
{
    size_t new_length;
    size_t i;

    /* if ( ch == '\0' ) ch = ' '; */

    if ( this->length() < pos1 ) pos1 = this->length();
    if ( this->length() < pos1 + n1 ) n1 = this->length() - pos1;

    if ( n1 < n2 ) {
	new_length = this->length() + (n2 - n1);
    }
    else if ( n2 < n1 ) {
	new_length = this->length() - (n1 - n2);
	c_memmove(this->str_ptr() + (pos1 + n2),
		  this->str_ptr() + (pos1 + n1),
		  this->length() - pos1 - n1);
    }
    else new_length = this->length();

    if ( this->str_length_rec != UNDEF ) {	/* 固定長バッファの場合 */
	if ( n1 < n2 ) {
	    if ( (pos1 + n2) + 1 < this->str_buf_length_rec ) {
		size_t sz = this->length() - pos1 - n1;
		if ( this->str_buf_length_rec < (pos1 + n2) + sz + 1 ) {
		    sz = this->str_buf_length_rec - 1 - (pos1 + n2);
		}
		c_memmove(this->_str_rec + (pos1 + n2),
			  this->_str_rec + (pos1 + n1),
			  sz);
	    }
	}
	for ( i=0 ; i < n2 && pos1 + i + 1 < this->str_buf_length_rec ; i++ ) {
	    ((unsigned char *)this->_str_rec)[pos1 + i] = ch;
	}
	if ( this->str_buf_length_rec < new_length + 1 ) new_length = this->str_buf_length_rec - 1;
	this->_str_rec[new_length] = '\0';

	this->str_length_rec = new_length;
    }
    else {					/* 可変長バッファの場合 */
	if ( new_length != this->length() || this->str_ptr() == NULL ) {
	    if ( this->realloc_str_rec(new_length + 1) < 0 ) {
		err_throw(__FUNCTION__,"FATAL","realloc() failed");
	    }
	    if ( this->extptr_rec != NULL ) *(this->extptr_rec) = this->str_ptr();
	}

	char *this_str_ptr = this->str_ptr();
	if ( n1 < n2 ) {
	    c_memmove(this_str_ptr + (pos1 + n2),
		      this_str_ptr + (pos1 + n1),
		      this->length() - pos1 - n1);
	}
	for ( i=0 ; i < n2 ; i++ ) {
	    ((unsigned char *)this_str_ptr)[pos1 + i] = ch;
	}
	this_str_ptr[new_length] = '\0';

	this->str_buf_length_rec = new_length + 1;
    }

    return *this;
}

#ifdef TSTRING__USE_SOLO_NARG
tstring &tstring::replace( size_t pos1, size_t n1, size_t n2 )
{
    return this->replace(pos1,n1,' ',n2);
}
#endif

/**
 * @brief  文字列の一部を消去
 *
 *  自身が持つ文字列の文字を消去します．
 *
 * @param      pos 消去の開始位置
 * @param      n 消去する文字数
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 * 
 */
tstring &tstring::erase( size_t pos, size_t n )
{
    return tstring::replace(pos,n, ' ',(size_t)0);
}

/**
 * @brief  文字列の全てを消去
 *
 *  自身が持つ文字列の文字を消去します．
 *
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 * 
 */
tstring &tstring::erase()
{
    return this->erase(0,this->length());

}

/**
 * @brief  文字列の一部分以外を消去
 *
 *  自身の文字列を，位置pos からn 個の文字列だけにします．
 *
 * @param      pos 切り抜きの開始位置
 * @param      n 切り抜く文字数
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 * 
 */
tstring &tstring::crop( size_t pos, size_t n )
{
    size_t max_n;
    if ( this->length() < pos ) pos = this->length();
    max_n = this->length() - pos;
    if ( max_n < n ) n = max_n;
    tstring::erase(0,pos);
    tstring::erase(n,this->length() - n);
    return *this;
}

/**
 * @brief  文字列の一部分以外を消去
 *
 *  自身の文字列を，位置pos からn 個の文字列だけにします．
 *
 * @param      pos 切り抜きの開始位置
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 * 
 */
tstring &tstring::crop( size_t pos )
{
    if ( this->length() < pos ) pos = this->length();
    return this->crop(pos,this->length() - pos);
}

/* 非推奨 */
#if 0
tstring &tstring::substr( size_t pos, size_t n )
{
    return this->crop(pos,n);
}
tstring &tstring::substr( size_t pos )
{
    return this->crop(pos);
}
#endif

/**
 * @brief  文字列の単純検索と置換
 *
 *  自身が持つ文字列の左側から文字列org_str を検索し，
 *  見つかった場合は文字列new_str で置き換えます．
 *
 * @param      org_str 検出する文字列
 * @param      new_str 置換の源泉となる文字列
 * @param      all 全置換のフラグ
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 * @throw      内部バッファの確保に失敗した場合
 * 
 */
ssize_t tstring::strreplace( const char *org_str, const char *new_str,
			     bool all )
{
    ssize_t pret = -1, pos = 0;
    size_t org_len, new_len;
    if ( org_str == NULL ) return -1;
    if ( new_str == NULL ) return -1;
    org_len = c_strlen(org_str);
    new_len = c_strlen(new_str);
    while ( 0 <= (pos=this->find(pos, org_str)) ) {
	this->replace(pos, org_len, new_str);
	pos += new_len;
	pret = pos;
	if ( all == false ) break;
    }
    return pret;
}

/**
 * @brief  文字列の単純検索と置換
 *
 *  自身が持つ文字列の左側から文字列org_str を検索し，<br>
 *  見つかった場合は文字列new_str で置き換えます．
 *
 * @param      pos 文字列検索の開始位置
 * @param      org_str 検出する文字列
 * @param      new_str 置換の源泉となる文字列
 * @param      all 全置換のフラグ
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 * @throw      内部バッファの確保に失敗した場合
 * 
 */
ssize_t tstring::strreplace( size_t pos, const char *org_str,
			     const char *new_str, bool all )
{
    ssize_t pret = -1, spos = pos;
    size_t org_len, new_len;
    if ( org_str == NULL ) return -1;
    if ( new_str == NULL ) return -1;
    org_len = c_strlen(org_str);
    new_len = c_strlen(new_str);
    while ( 0 <= (spos=this->find(spos, org_str)) ) {
	this->replace(spos, org_len, new_str);
	spos += new_len;
	pret = spos;
	if ( all == false ) break;
    }
    return pret;
}

/**
 * @brief  文字列の単純検索と置換
 *
 *  自身が持つ文字列の左側から文字列org_str を検索し，<br>
 *  見つかった場合は文字列new_str で置き換えます．
 *
 * @param      org_str 検出する文字列
 * @param      new_str 置換の源泉となる文字列
 * @param      all 全置換のフラグ
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 * @throw      内部バッファの確保に失敗した場合
 * 
 */
ssize_t tstring::strreplace( const tstring &org_str, const char *new_str,
			     bool all )
{
    return this->strreplace(org_str.cstr(), new_str, all);
}

/**
 * @brief  文字列の単純検索と置換
 *
 *  自身が持つ文字列の左側から文字列org_str を検索し，<br>
 *  見つかった場合は文字列new_str で置き換えます．
 *
 * @param      pos 文字列検索の開始位置
 * @param      org_str 検出する文字列
 * @param      new_str 置換の源泉となる文字列
 * @param      all 全置換のフラグ
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 * @throw      内部バッファの確保に失敗した場合
 * 
 */
ssize_t tstring::strreplace( size_t pos, const tstring &org_str,
			     const char *new_str, bool all )
{
    return this->strreplace(pos, org_str.cstr(), new_str, all);
}


/**
 * @brief  文字列の単純検索と置換
 *
 *  自身が持つ文字列の左側から文字列org_str を検索し，<br>
 *  見つかった場合は文字列new_str で置き換えます．
 *
 * @param      org_str 検出する文字列
 * @param      new_str 置換の源泉となる文字列
 * @param      all 全置換のフラグ
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 * @throw      内部バッファの確保に失敗した場合
 * 
 */
ssize_t tstring::strreplace( const char *org_str, const tstring &new_str,
			     bool all )
{
    return this->strreplace(org_str, new_str.cstr(), all);
}

/**
 * @brief  文字列の単純検索と置換
 *
 *  自身が持つ文字列の左側から文字列org_str を検索し，<br>
 *  見つかった場合は文字列new_str で置き換えます．
 *
 * @param      pos 文字列検索の開始位置
 * @param      org_str 検出する文字列
 * @param      new_str 置換の源泉となる文字列
 * @param      all 全置換のフラグ
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 * @throw      内部バッファの確保に失敗した場合
 * 
 */
ssize_t tstring::strreplace( size_t pos, const char *org_str,
			     const tstring &new_str, bool all )
{
    return this->strreplace(pos, org_str, new_str.cstr(), all);
}

/**
 * @brief  文字列の単純検索と置換
 *
 *  自身が持つ文字列の左側から文字列org_str を検索し，<br>
 *  見つかった場合は文字列new_str で置き換えます．
 *
 * @param      org_str 検出する文字列
 * @param      new_str 置換の源泉となる文字列
 * @param      all 全置換のフラグ
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 * @throw      内部バッファの確保に失敗した場合
 * 
 */
ssize_t tstring::strreplace( const tstring &org_str, const tstring &new_str,
			     bool all )
{
    return this->strreplace(org_str.cstr(), new_str.cstr(), all);
}

/**
 * @brief  文字列の単純検索と置換
 *
 *  自身が持つ文字列の左側から文字列org_str を検索し，<br>
 *  見つかった場合は文字列new_str で置き換えます．
 *
 * @param      pos 文字列検索の開始位置
 * @param      org_str 検出する文字列
 * @param      new_str 置換の源泉となる文字列
 * @param      all 全置換のフラグ
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 * @throw      内部バッファの確保に失敗した場合
 * 
 */
ssize_t tstring::strreplace( size_t pos, const tstring &org_str,
			     const tstring &new_str, bool all )
{
    return this->strreplace(pos, org_str.cstr(), new_str.cstr(), all);
}


/*
 * New member functions of 1.3.0
 */

/**
 * @brief  一番外側のクォーテーションまたは括弧の検索と消去
 */
ssize_t tstring::erase_quotes( const char *quot_bkt, int escape,
			       bool rm_escape, size_t *new_qspn, bool all )
{
    ssize_t ret;
    size_t nextpos, len_this = this->length();
    ret = erase_quotations( this->str_ptr(), &len_this,
			    0, quot_bkt, escape, rm_escape, new_qspn, &nextpos, all);
    if ( len_this != this->length() ) this->resize(len_this);
    if ( 0 <= ret ) return nextpos;
    else return ret;
}

/**
 * @brief  一番外側のクォーテーションまたは括弧の検索と消去
 */
ssize_t tstring::erase_quotes( size_t pos, const char *quot_bkt, int escape,
			       bool rm_escape, size_t *new_qspn, bool all )
{
    ssize_t ret;
    size_t nextpos, len_this = this->length();
    ret = erase_quotations( this->str_ptr(), &len_this,
			    pos, quot_bkt, escape, rm_escape, new_qspn, &nextpos, all);
    if ( len_this != this->length() ) this->resize(len_this);
    if ( 0 <= ret ) return nextpos;
    else return ret;
}


/**
 * @brief  末尾の文字の除去
 *
 *  自身が持つ文字列の末尾の文字を除去します
 *
 * @return  自身の参照
 * @throw   内部バッファの削除に失敗した場合
 */
tstring &tstring::chop()
{
    if ( 0 < this->length() ) this->resize(this->length()-1);
    return *this;
}

/**
 * @brief  文字列の右端の改行文字の除去
 *
 *  自身が持つ文字列の右端の改行文字を除去します．
 *
 * @param      rs 改行文字列
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 * 
 */
tstring &tstring::chomp( const char *rs )
{
    if ( rs != NULL && rs[0] != '\0' ) {
	size_t len_rs = c_strlen(rs);
	if ( len_rs <= this->length() ) {
	    if ( this->strcmp(this->length()-len_rs, rs) == 0 ) {
		this->resize(this->length()-len_rs);
	    }
	}
    }
    return *this;
}

/**
 * @brief  文字列の右端の改行文字の除去
 *
 *  自身が持つ文字列の右端の改行文字を除去します．
 *
 * @param      rs 改行文字列
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 * 
 */
tstring &tstring::chomp( const tstring &rs )
{
    return this->chomp(rs.cstr());
}

/**
 * @brief  文字列の両端の不要文字の除去
 *
 *  自身が持つ文字列の両端にある任意文字を除去します．
 *
 * @param   side_spaces 除去対象の文字セット．[A-Z] といった表現も使用可能．
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 * 
 */
tstring &tstring::trim( const char *side_spaces )
{
    if ( 0 < this->length() && side_spaces != NULL ) {
	const char *str_beg;
	size_t len = c_strlen(side_spaces);
	if ( 0 < len && side_spaces[0] == '[' && side_spaces[len-1] == ']' ) {
	    tstring accepts;
	    bool bl;
	    int flags = 0;
	    size_t p0 = 0;
	    p0 += make_accepts_regexp(side_spaces, true, accepts, &flags, &bl);
	    if ( p0 == 0 ) {	/* パターンがおかしい */
		rm_side_spaces(this->str_ptr(),side_spaces,0,true,
			       &str_beg,&len);
	    }
	    else {
		rm_side_spaces(this->str_ptr(),accepts.cstr(),flags,bl,
			       &str_beg,&len);
	    }
	}
	else {
	    rm_side_spaces(this->str_ptr(),side_spaces,0,true, &str_beg,&len);
	}
	return tstring::crop( str_beg - this->str_ptr() , len );
    }
    return *this;
}

/**
 * @brief  文字列の両端の不要文字の除去
 *
 *  自身が持つ文字列の両端にある任意文字を除去します．
 *
 * @param   side_spaces 除去対象の文字セット．[A-Z] といった表現も使用可能．
 * @return  自身の参照
 * @throw   内部バッファの確保に失敗した場合
 * 
 */
tstring &tstring::trim( const tstring &side_spaces )
{
    return this->trim(side_spaces.cstr());
}

/**
 * @brief  文字列の両端の不要文字の除去
 *
 *  自身が持つ文字列の両端にある任意文字を除去します．
 *
 * @param      side_space 除去対象の文字
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 * 
 */
tstring &tstring::trim( int side_space )
{
    //unsigned char s[2] = { side_space, '\0' };
    //return this->trim( (const char *)s );
    if ( 0 < this->length() ) {
	ssize_t pos;
	pos = tstring::find_first_not_of(side_space);
	if ( pos < 0 ) {
	    return tstring::crop( 0, 0 );
	}
	else {
	    ssize_t rpos;
	    rpos = tstring::find_last_not_of(side_space);
	    return tstring::crop( pos, rpos - pos + 1 );
	}
    }
    return *this;
}

/**
 * @brief  文字列の左端の不要文字の除去
 *
 *  自身が持つ文字列の左端の空白文字を除去します．
 *
 * @param      side_spaces 除去対象の文字セット．[A-Z] といった表現も使用可能．
 * @return     自身の参照
 * 
 */
tstring &tstring::ltrim( const char *side_spaces )
{
    if ( 0 < this->length() && side_spaces != NULL ) {
	ssize_t pos;
	size_t len = c_strlen(side_spaces);
	if ( 0 < len && side_spaces[0] == '[' && side_spaces[len-1] == ']' ) {
	    tstring accepts;
	    bool bl;
	    int flags = 0;
	    size_t p0 = 0;
	    p0 += make_accepts_regexp(side_spaces, true, accepts, &flags, &bl);
	    if ( p0 == 0 ) {	/* パターンがおかしい */
		pos = tstring::find_first_not_of(side_spaces);
	    }
	    else {
		const char *fptr;
		bl = (bl == true) ? false : true;
		fptr = find_pattern(this->str_ptr(), this->length(),
				    accepts.cstr(), flags, bl);
		if ( fptr != NULL ) pos = fptr - this->str_ptr();
		else pos = -1;
	    }
	}
	else {
	    pos = tstring::find_first_not_of(side_spaces);
	}
	if ( pos < 0 ) pos = this->length();
	return tstring::crop( pos, this->length() - pos );
    }
    return *this;
}

/**
 * @brief  文字列の左端の不要文字の除去
 *
 *  自身が持つ文字列の左端の空白文字を除去します．
 *
 * @param      side_spaces 除去対象の文字セット．[A-Z] といった表現も使用可能．
 * @return     自身の参照
 * 
 */
tstring &tstring::ltrim( const tstring &side_spaces )
{
    return this->ltrim(side_spaces.cstr());
}

/**
 * @brief  文字列の左端の不要文字の除去
 *
 *  自身が持つ文字列の左端の空白文字を除去します．
 *
 * @param      side_space 除去対象の文字
 * @return     自身の参照
 * 
 */
tstring &tstring::ltrim( int side_space )
{
    //unsigned char s[2] = { side_space, '\0' };
    //return this->ltrim( (const char *)s );
    if ( 0 < this->length() ) {
	ssize_t pos;
	pos = tstring::find_first_not_of(side_space);
	if ( pos < 0 ) pos = this->length();
	return tstring::crop( pos, this->length() - pos );
    }
    return *this;
}

/**
 * @brief  文字列の右端の不要文字の除去
 *
 *  自身が持つ文字列の右端の空白文字を除去します．
 *
 * @param      side_spaces 除去対象の文字セット．[A-Z] といった表現も使用可能．
 * @return     自身の参照
 * 
 */
tstring &tstring::rtrim( const char *side_spaces )
{
    if ( 0 < this->length() && side_spaces != NULL ) {
	ssize_t pos;
	size_t len = c_strlen(side_spaces);
	if ( 0 < len && side_spaces[0] == '[' && side_spaces[len-1] == ']' ) {
	    tstring accepts;
	    bool bl;
	    int flags = 0;
	    size_t p0 = 0;
	    p0 += make_accepts_regexp(side_spaces, true, accepts, &flags, &bl);
	    if ( p0 == 0 ) {	/* パターンがおかしい */
		pos = tstring::find_last_not_of(side_spaces);
	    }
	    else {
		const char *fptr;
		bl = (bl == true) ? false : true;
		fptr = rfind_pattern(this->str_ptr(), this->length(),
				     accepts.cstr(), flags, bl);
		if ( fptr != NULL ) pos = fptr - this->str_ptr();
		else pos = -1;
	    }
	}
	else {
	    pos = tstring::find_last_not_of(side_spaces);
	}
	if ( pos < 0 ) pos = -1;
	return tstring::crop( 0, 1 + pos );
    }
    return *this;
}

/**
 * @brief  文字列の右端の不要文字の除去
 *
 *  自身が持つ文字列の右端の空白文字を除去します．
 *
 * @param      side_spaces 除去対象の文字セット．[A-Z] といった表現も使用可能．
 * @return     自身の参照
 * 
 */
tstring &tstring::rtrim( const tstring &side_spaces )
{
    return this->rtrim(side_spaces.cstr());
}

/**
 * @brief  文字列の右端の不要文字の除去
 *
 *  自身が持つ文字列の右端の空白文字を除去します．
 *
 * @param      side_space 除去対象の文字
 * @return     自身の参照
 * 
 */
tstring &tstring::rtrim( int side_space )
{
    //unsigned char s[2] = { side_space, '\0' };
    //return this->rtrim( (const char *)s );
    if ( 0 < this->length() ) {
	ssize_t pos;
	pos = tstring::find_last_not_of(side_space);
	if ( pos < 0 ) pos = -1;
	return tstring::crop( 0, 1 + pos );
    }
    return *this;
}

/**
 * @brief  文字列の両端の不要文字の除去
 *
 *  自身が持つ文字列の両端にある任意文字を除去します．
 *
 * @param      side_spaces 除去対象の文字セット．[A-Z] といった表現も使用可能．
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 * 
 */
tstring &tstring::strtrim( const char *side_spaces )
{
    return this->trim(side_spaces);
}

/**
 * @brief  文字列の両端の不要文字の除去
 *
 *  自身が持つ文字列の両端にある任意文字を除去します．
 *
 * @param      side_spaces 除去対象の文字セット．[A-Z] といった表現も使用可能．
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 * 
 */
tstring &tstring::strtrim( const tstring &side_spaces )
{
    return this->trim(side_spaces);
}

/**
 * @brief  文字列の両端の不要文字の除去
 *
 *  自身が持つ文字列の両端にある任意文字を除去します．
 *
 * @param      side_spaces 除去対象の文字
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 * 
 */
tstring &tstring::strtrim( int side_space )
{
    return this->trim(side_space);
}

/**
 * @brief  大文字を小文字に変換
 *
 *  自身の文字列が持つアルファベットの大文字を小文字に変換します．
 *
 * @param      pos 変換の開始位置
 * @return     自身の参照
 * 
 */
tstring &tstring::tolower( size_t pos )
{
    return this->tolower(pos,this->length());
}

/**
 * @brief  大文字を小文字に変換
 *
 *  自身の文字列が持つアルファベットの大文字を小文字に変換します．
 *
 * @param      pos 変換の開始位置
 * @param      n 変換する文字
 * @return     自身の参照
 * 
 */
tstring &tstring::tolower( size_t pos, size_t n )
{
    unsigned char *this_buffer = (unsigned char *)this->str_ptr();
    if ( pos < this->length() ) {
	size_t i;
	size_t maxn = this->length() - pos;
	if ( maxn < n ) n = maxn;
	for ( i=0 ; i < n ; i++ ) {
	    this_buffer[pos+i] = c_tolower(this_buffer[pos+i]);
	}
    }
    return *this;
}

/**
 * @brief  小文字を大文字に変換
 *
 *  自身の文字列が持つアルファベットの小文字を大文字に変換します．
 *
 * @param      pos 変換の開始位置
 * @return     自身の参照
 * 
 */
tstring &tstring::toupper( size_t pos )
{
    return this->toupper(pos,this->length());
}

/**
 * @brief  小文字を大文字に変換
 *
 *  自身の文字列が持つアルファベットの小文字を大文字に変換します．
 *
 * @param      pos 変換の開始位置
 * @param      n 変換する文字
 * @return     自身の参照
 * 
 */
tstring &tstring::toupper( size_t pos, size_t n )
{
    unsigned char *this_buffer = (unsigned char *)this->str_ptr();
    if ( pos < this->length() ) {
	size_t i;
	size_t maxn = this->length() - pos;
	if ( maxn < n ) n = maxn;
	for ( i=0 ; i < n ; i++ ) {
	    this_buffer[pos+i] = c_toupper(this_buffer[pos+i]);
	}
    }
    return *this;
}

/**
 * @brief  タブ文字を桁揃えして空白文字に置換
 *
 *  自身が持つ文字列の水平タブ文字 '\t' を，
 *  tab_width の値に桁揃えをして空白文字に置換します．
 *
 * @param      tab_width タブ幅．省略時は8
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 * 
 */
tstring &tstring::expand_tabs( size_t tab_width )
{
    this->cleanup_shallow_copy(true);    /* バッファを直接操作するため呼ぶ */

    size_t i,j,len;
    if ( tab_width < 1 ) tab_width = 8;
    len = this->length();
    for ( i=0,j=0 ; i < len ; ) {
	if ( this->_str_rec[i] == '\n' ) {
	    j=0;
	    i++;
	}
	else if ( this->_str_rec[i] == '\t' ) {
	    size_t n = tab_width - (j % tab_width);
	    tstring::replace(i,1,' ',n);
	    len = this->length();
	    j += n;
	    i += n;
	}
	else {
	    j++;
	    i++;
	}
    }
    return *this;
}

/**
 * @brief  空白文字を桁揃えしてタブ文字に置換
 *
 *  自身が持つ文字列の 2 文字以上連続した空白文字 ' ' すべてを対象にし，
 *  指定したタブ幅 tab_width で桁揃えして '\t' で置換します．
 *
 * @param      tab_width タブ幅．省略時は8
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 * @note       tab width=1 を設定した時の動作は未定義です．
 *
 */
tstring &tstring::contract_spaces( size_t tab_width )
{
    this->cleanup_shallow_copy(true);    /* バッファを直接操作するため呼ぶ */

    size_t i,j,len;
    if ( tab_width < 1 ) tab_width = 8;
    /* 一旦，展開する(1文字分の'\t'についてはそのまま残す) */
    len = this->length();
    for ( i=0,j=0 ; i < len ; ) {
	if ( this->_str_rec[i] == '\n' ) {
	    j=0;
	    i++;
	}
	else if ( this->_str_rec[i] == '\t' ) {
	    size_t n = tab_width - (j % tab_width);
	    if ( 1 < n ) {
		tstring::replace(i,1,' ',n);
		len = this->length();
	    }
	    j += n;
	    i += n;
	}
	else {
	    j++;
	    i++;
	}
    }
    /* '\t' を使って縮約する */
    len = this->length();
    for ( i=0,j=0 ; i < len ; ) {
	if ( this->_str_rec[i] == '\n' ) {
	    j=0;
	    i++;
	}
	else if ( this->_str_rec[i] == '\t' ) {
	    j += tab_width - (j % tab_width);
	    i++;
	}
	else if ( this->_str_rec[i] == ' ' ) {
	    size_t spn = tstring::strspn(i,' ');
	    size_t nbegin = tab_width - (j % tab_width);
	    if ( nbegin <= spn ) {
		tstring tmpstr;
		if ( nbegin == 1 )
		    tmpstr.assign(" ").append('\t',((spn-nbegin)/tab_width));
		else
		    tmpstr.assign('\t',1+((spn-nbegin)/tab_width));
		tmpstr.append(' ',(spn-nbegin) % tab_width);
		tstring::replace(i,spn,tmpstr,0);
		len = this->length();
		j += spn;
		i += tmpstr.length();
	    }
	    else {
		j += spn;
		i += spn;
	    }
	}
	else {
	    j++;
	    i++;
	}
    }
    return *this;
}

/**
 * @brief  2つのオブジェクト間での内容のスワップ
 *
 *  オブジェクトsobj の内容と自身の内容とを入れ替えます．
 *
 * @param      sobj 自身と内容を入れ替えるオブジェクト
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合
 * @throw      メモリ破壊を起こした場合
 *
 */
tstring &tstring::swap( tstring &sobj )
{
    if ( &sobj == this ) return *this;

    /* バッファを直接操作するため呼ぶ */
    this->cleanup_shallow_copy(true);
    sobj.cleanup_shallow_copy(true);

    char *tmp__str_rec;
    size_t tmp__str_alloc_blen_rec;
    size_t tmp__str_buf_length_rec;

    size_t tmp__reg_elem_length_rec;
    size_t *tmp__reg_pos_rec;
    size_t *tmp__reg_length_rec;
    char **tmp__reg_cstr_ptr_rec;
    size_t tmp__reg_cstrbuf_length_rec;
    char *tmp__reg_cstrbuf_rec;

    if ( this->str_length_rec != UNDEF || sobj.str_length_rec != UNDEF ) {
	try {
	    if ( this->length() < sobj.length() ) {
		tstring tmp;
		tmp = (*this);
		(*this) = sobj;
		sobj = tmp;
	    }
	    else {
		tstring tmp;
		tmp = sobj;
		sobj = (*this);
		(*this) = tmp;
	    }
	}
	catch (...) {
	    err_throw(__FUNCTION__,"FATAL","= failed");
	}
	return *this;
    }

    tmp__str_rec   = sobj._str_rec;
    tmp__str_alloc_blen_rec = sobj.str_alloc_blen_rec;
    tmp__str_buf_length_rec = sobj.str_buf_length_rec;

    sobj._str_rec   = this->_str_rec;
    sobj.str_alloc_blen_rec = this->str_alloc_blen_rec;
    sobj.str_buf_length_rec = this->str_buf_length_rec;
    if ( sobj.extptr_rec != NULL ) *(sobj.extptr_rec) = sobj._str_rec;
    if ( sobj._str_rec == NULL && sobj.str_rec_keeps_non_null == true ) {
        sobj.__init_non_null_str_rec();
    }

    this->_str_rec   = tmp__str_rec;
    this->str_alloc_blen_rec = tmp__str_alloc_blen_rec;
    this->str_buf_length_rec = tmp__str_buf_length_rec;
    if ( this->extptr_rec != NULL ) *(this->extptr_rec) = this->_str_rec;
    if ( this->_str_rec == NULL && this->str_rec_keeps_non_null == true ) {
        this->__init_non_null_str_rec();
    }

    this->regex_rec.swap(sobj.regex_rec);

    tmp__reg_elem_length_rec = this->reg_elem_length_rec;
    this->reg_elem_length_rec = sobj.reg_elem_length_rec;
    sobj.reg_elem_length_rec = tmp__reg_elem_length_rec;

    tmp__reg_pos_rec = this->_reg_pos_rec;
    this->_reg_pos_rec = sobj._reg_pos_rec;
    sobj._reg_pos_rec = tmp__reg_pos_rec;

    tmp__reg_length_rec = this->_reg_length_rec;
    this->_reg_length_rec = sobj._reg_length_rec;
    sobj._reg_length_rec = tmp__reg_length_rec;

    tmp__reg_cstr_ptr_rec = this->_reg_cstr_ptr_rec;
    this->_reg_cstr_ptr_rec = sobj._reg_cstr_ptr_rec;
    sobj._reg_cstr_ptr_rec = tmp__reg_cstr_ptr_rec;

    tmp__reg_cstrbuf_length_rec = this->reg_cstrbuf_length_rec;
    this->reg_cstrbuf_length_rec = sobj.reg_cstrbuf_length_rec;
    sobj.reg_cstrbuf_length_rec = tmp__reg_cstrbuf_length_rec;

    tmp__reg_cstrbuf_rec = this->_reg_cstrbuf_rec;
    this->_reg_cstrbuf_rec = sobj._reg_cstrbuf_rec;
    sobj._reg_cstrbuf_rec = tmp__reg_cstrbuf_rec;

    return *this;
}

/**
 * @brief  文字列のポインタ配列を，区切り文字列 delim で結合
 *
 *  arr で指定した文字列のポインタ配列(NULL終端)を読み込み，
 *  区切り文字列 delim で結合して自身に格納します．
 *
 * @param      arr 文字列のポインタ配列の先頭アドレス
 * @param      delim 区切り文字列
 * @return     自身の参照
 * @throw      オブジェクト内のバッファの確保に失敗した場合
 *
 */
tstring &tstring::implode( const char *const *arr, const char *delim )
{
    size_t len_all, len_delim;
    size_t i, j;
    heap_mem<size_t> len_buf;

    if ( arr == NULL ) goto quit;

    if ( delim == NULL ) delim = "";
    len_delim = c_strlen(delim);

    /* 配列長を調べる */
    for ( i=0 ; arr[i] != NULL ; i++ );

    /* ゼロなら… */
    if ( i == 0 ) {
	this->resize(0);
	goto quit;
    }

    /* 各文字列の長さを保存する配列 */
    if ( len_buf.allocate(i) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","malloc() failed");
    }

    //err_report1(__FUNCTION__,"DEBUG","len of arr = %zd",i);

    /* 各文字列の長さを調べる */
    len_all = 0;
    i = 0;
    if ( arr[i] != NULL ) {
	j = c_strlen(arr[i]);
	len_buf[i] = j;
	len_all += j;
	i ++;
    }
    while ( arr[i] != NULL ) {
	j = c_strlen(arr[i]);
	len_all += len_delim;
	len_buf[i] = j;
	len_all += j;
	i ++;
    }

    //err_report1(__FUNCTION__,"DEBUG","len_al = %zd",len_all);

    /* alloc buffer */
    this->resize(len_all);

    /* set each into buffer */
    i = 0;
    j = 0;
    if ( arr[i] != NULL ) {
	this->put(j, arr[i]);
	j += len_buf[i];
	i ++;
    }
    while ( arr[i] != NULL ) {
	this->put(j, delim);
	j += len_delim;
	this->put(j, arr[i]);
	j += len_buf[i];
	i ++;
    }

 quit:
    return *this;
}

/**
 * @brief  バイナリデータの取り込み
 *
 *  buf で指定したバッファから bufsize バイトを読み込み，自身に格納します．
 *
 * @param      buf ユーザバッファのアドレス
 * @param      bufsize ユーザバッファのサイズ
 * @param      altchr ユーザバッファに文字 '\0' が存在した場合に置き換える文字
 * @return     自身の参照
 * @throw      オブジェクト内のバッファの確保に失敗した場合
 * @attention  altchr が無指定または '\0' が指定された場合，各種検索処理などの
 *             結果は未定義です．
 *
 */
tstring &tstring::import_binary( const char *buf, size_t bufsize, int altchr )
{
    if ( buf == NULL && 0 < bufsize ) return *this;

    this->resize(bufsize);

    if ( this->str_buf_length_rec < bufsize + 1 ) bufsize = this->str_buf_length_rec - 1;

    char *this_str_ptr = this->str_ptr();
    c_memmove(this_str_ptr, buf, bufsize);
    if ( altchr != '\0' ) {
	size_t i;
	for ( i=0 ; i < bufsize ; i++ ) {
	    if ( this_str_ptr[i] == '\0' ) 
		((unsigned char *)this_str_ptr)[i] = altchr;
	}
    }
    this_str_ptr[bufsize] = '\0';

    return *this;
}

/**
 * @brief  固定長バッファモードでの文字列長情報の更新
 *
 *  固定長バッファモードで，ユーザプログラムがオブジェクトが管理している
 *  バッファに時下書きした場合に，オブジェクトの文字列長の情報を更新する．
 *
 * @return     自身の参照
 *
 */
tstring &tstring::update_length()
{
    if ( this->str_length_rec != UNDEF ) {	/* 固定長バッファの場合 */
	if ( 0 < this->str_buf_length_rec ) {
	    size_t i;
	    for ( i=0 ; i+1 < this->str_buf_length_rec ; i++ ) {
		if ( this->_str_rec[i] == '\0' ) break;
	    }
	    this->str_length_rec = i;
	    if ( i+1 == this->str_buf_length_rec ) this->_str_rec[i] = '\0';
	}
    }

    return *this;
}

/**
 * @brief  文字列の外部バッファへのコピー
 *
 *  自身の文字列を，外部バッファdest_str にコピーします．
 *
 * @param      pos コピーの開始位置
 * @param      dest_str コピー先の外部バッファアドレス
 * @param      buf_size 外部バッファのサイズ
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::getstr( size_t pos, char *dest_str, size_t buf_size ) const
{
    size_t i, len = this->length();

    if ( 0 < buf_size && dest_str == NULL ) return -1;
    if ( len < pos ) {
	if ( 0 < buf_size ) dest_str[0] = '\0';
	return -1;
    }

    const char *this_str_ptr = this->str_ptr_cs();
    for ( i=0 ; i+1 < buf_size && pos+i < len ; i++ ) {
	dest_str[i] = this_str_ptr[pos + i];
    }
    if ( i < buf_size ) dest_str[i] = '\0';

    return len - pos;
}

/**
 * @brief  文字列の外部バッファへのコピー
 *
 *  自身の文字列を，外部バッファdest_str にコピーします．
 *
 * @param      dest_str コピー先の外部バッファアドレス
 * @param      buf_size 外部バッファのサイズ
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::getstr( char *dest_str, size_t buf_size ) const
{
    return this->getstr(0,dest_str,buf_size);
}

/**
 * @brief  文字列の一部または全てを，指定オブジェクトへコピー
 * 
 *  自身の文字列のすべてまたは一部を，dest で指定されたオブジェクトにコピー
 *  します．
 *
 * @param      pos コピー元の文字列の開始位置
 * @param      n コピーする文字数
 * @param      dest コピー先の外部tstringクラスのオブジェクト
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 * @throw      内部バッファの確保に失敗した場合
 *
 */
ssize_t tstring::copy( size_t pos, size_t n, tstring *dest ) const
{
    if ( dest == NULL ) return -1;

    size_t len = this->length();

    if ( len < pos ) {
	dest->erase();
	return -1;
    }
    if ( this->str_ptr_cs() == NULL ) {
	dest->erase();
	return 0;
    }

    if ( len - pos < n ) n = len - pos;

    dest->replace(0,dest->length(), *this, pos, n);

    return n;
}

/**
 * @brief  文字列の一部または全てを，指定オブジェクトへコピー
 *
 *  自身の文字列のすべてまたは一部を，dest で指定されたオブジェクトにコピー
 *  します．
 *
 * @param      pos コピー元の文字列の開始位置
 * @param      dest コピー先の外部tstringクラスのオブジェクト
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 * @throw      内部バッファの確保に失敗した場合
 *
 */
ssize_t tstring::copy( size_t pos, tstring *dest ) const
{
    return this->copy(pos, this->length(), dest);
}

/**
 * @brief  自身の内容を指定オブジェクトへコピー
 *
 *  自身の内容を，指定されたオブジェクト dest にコピーします．<br>
 *  「=」演算子や .init(obj) とは異なり，常に deep copy が実行されます．
 *
 * @param      dest コピー先の外部tstringクラスのオブジェクト
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 * @throw      内部バッファの確保に失敗した場合
 *
 */
ssize_t tstring::copy( tstring *dest ) const
{
    if ( dest == NULL ) return -1;
    if ( dest == this ) return dest->length();

    if ( dest->str_length_rec != UNDEF ) {	/* 固定長バッファの場合 */

	dest->__deep_init(*this);

    }
    else {

	/* shallow copy 関係のクリーンアップ */
	dest->cleanup_shallow_copy(false);

	dest->__deep_init(*this);
	     
    }

    return dest->length();
}

/**
 * @brief  自身の内容を指定オブジェクトへコピー (非推奨)
 * @deprecated  非推奨．<br>
 * ssize_t tstring::copy( size_t pos, size_t n, tstring *dest ) const <br>
 * をお使いください．
 */
ssize_t tstring::copy( size_t pos, size_t n, tstring &dest ) const
{
    return this->copy(pos, n, &dest);
}

/**
 * @brief  自身の内容を指定オブジェクトへコピー (非推奨)
 * @deprecated  非推奨．<br>
 * ssize_t tstring::copy( size_t pos, tstring *dest ) const <br>
 * をお使いください．
 */
ssize_t tstring::copy( size_t pos, tstring &dest ) const
{
    return this->copy(pos, &dest);
}

/**
 * @brief  自身の内容を指定オブジェクトへコピー (非推奨)
 * @deprecated  非推奨．<br>
 * ssize_t tstring::copy( tstring *dest ) const <br>
 * をお使いください．
 */
ssize_t tstring::copy( tstring &dest ) const
{
    return this->copy(&dest);
}

/**
 * @brief  自身の内容を指定オブジェクトへ移管
 *
 *  自身の内容を，dest により指定されたオブジェクトへ「移管」します．
 *  移管の結果，自身の文字列長はゼロになります．<br>
 *  dest についての文字列バッファの再確保は行なわれず，自身の文字列バッファ
 *  についての管理権限を dest に譲渡する実装になっており，高速に動作します．
 *  ただし，固定長バッファモード時には，「移管」ではなく，コピー & 消去として
 *  動作します．
 *
 * @param     dest 移管・コピー先のオブジェクトのアドレス
 * @return    自身の参照
 * @throw     バッファの確保に失敗した場合．メモリ破壊を起こした場合
 */
tstring &tstring::cut( tstring *dest )
{
    if ( dest == NULL ) {
	this->init();
    }
    /* dest が自身なら何もしない */
    else if ( dest != this ) {

	if ( dest->str_length_rec != UNDEF ) {	/* 固定長バッファの場合 */

	    dest->__deep_init(*this);
	    this->init();

	}
	else {

	    /* バッファを直接操作するため呼ぶ */
	    this->cleanup_shallow_copy(true);
	    dest->cleanup_shallow_copy(false);

	    /* shallow copy の機能を使ってバッファアドレスをコピー */
	    try {
		dest->__shallow_init(*this, true);
	    }
	    catch (...) {
		this->__force_init(false);
		err_throw(__FUNCTION__,"FATAL","caught exception");
	    }
	    this->__force_init(false);

	    if ( this->str_rec_keeps_non_null == true ) {
		this->__init_non_null_str_rec();
	    }

	    if ( this->extptr_rec != NULL ) {
		*(this->extptr_rec) = this->_str_rec;
	    }

	}

    }

    return *this;
}

/**
 * @brief  文字列を10進の整数値(int型)に変換
 *
 *  自身が持つ文字列の位置pos 以降の文字を，10 進の整数値に変換し，返します．
 *
 * @param      pos 自身が持つ文字列の変換開始位置
 * @return     整数
 * @note       [0-9] 以外の文字が現れた時点で変換は終了します
 *
 */
int tstring::atoi( size_t pos ) const
{
    const char *this_str_ptr = this->str_ptr_cs();
    if ( this_str_ptr != NULL && pos < this->length() ) 
	return c_strtol(this_str_ptr + pos, (char **)NULL, 10);
    else return 0;
}

/**
 * @brief  文字列を10進の整数値(int型)に変換
 *
 *  自身が持つ文字列の位置pos 以降の文字を，10 進の整数値に変換し，返します．
 *
 * @param      pos 自身が持つ文字列の変換開始位置
 * @param      n 整数値に変換する文字数
 * @return     整数
 * @throw      内部バッファの確保に失敗した場合
 * @note       [0-9] 以外の文字が現れた時点で変換は終了します
 *
 */
int tstring::atoi( size_t pos, size_t n ) const
{
    const char *this_str_ptr = this->str_ptr_cs();
    if ( this_str_ptr != NULL && pos < this->length() ) {
	tstring s;
	s.assign(this_str_ptr + pos,n);
	return c_strtol(s.cstr(),(char **)NULL,10);
    }
    else return 0;
}

/**
 * @brief  文字列を10進の整数値(long型)に変換
 *
 *  自身が持つ文字列の位置pos 以降の文字を，10 進の整数値に変換し，返します．
 *
 * @param      pos 自身が持つ文字列の変換開始位置
 * @return     整数
 * @note       [0-9] 以外の文字が現れた時点で変換は終了します
 *
 */
long tstring::atol( size_t pos ) const
{
    const char *this_str_ptr = this->str_ptr_cs();
    if ( this_str_ptr != NULL && pos < this->length() ) 
	return c_strtol(this_str_ptr + pos, (char **)NULL, 10);
    else return 0;
}

/**
 * @brief  文字列を10進の整数値(long型)に変換
 *
 *  自身が持つ文字列の位置pos 以降の文字を，10 進の整数値に変換し，返します．
 *
 * @param      pos 自身が持つ文字列の変換開始位置
 * @param      n 整数値に変換する文字数
 * @return     整数
 * @throw      内部バッファの確保に失敗した場合
 * @note       [0-9] 以外の文字が現れた時点で変換は終了します
 *
 */
long tstring::atol( size_t pos, size_t n ) const
{
    const char *this_str_ptr = this->str_ptr_cs();
    if ( this_str_ptr != NULL && pos < this->length() ) {
	tstring s;
	s.assign(this_str_ptr + pos,n);
	return c_strtol(s.cstr(),(char **)NULL,10);
    }
    else return 0;
}

/**
 * @brief  文字列を10進の整数値(long long型)に変換
 *
 *  自身が持つ文字列の位置pos 以降の文字を，10 進の整数値に変換し，返します．
 *
 * @param      pos 自身が持つ文字列の変換開始位置
 * @return     整数
 * @note       [0-9] 以外の文字が現れた時点で変換は終了します
 *
 */
long long tstring::atoll( size_t pos ) const
{
    const char *this_str_ptr = this->str_ptr_cs();
    if ( this_str_ptr != NULL && pos < this->length() ) 
	return c_strtoll(this_str_ptr + pos, (char **)NULL, 10);
    else return 0;
}

/**
 * @brief  文字列を10進の整数値(long long型)に変換
 *
 *  自身が持つ文字列の位置pos 以降の文字を，10 進の整数値に変換し，返します．
 *
 * @param      pos 自身が持つ文字列の変換開始位置
 * @param      n 整数値に変換する文字数
 * @return     整数
 * @throw      内部バッファの確保に失敗した場合
 * @note       [0-9] 以外の文字が現れた時点で変換は終了します
 *
 */
long long tstring::atoll( size_t pos, size_t n ) const
{
    const char *this_str_ptr = this->str_ptr_cs();
    if ( this_str_ptr != NULL && pos < this->length() ) {
	tstring s;
	s.assign(this_str_ptr + pos,n);
	return c_strtoll(s.cstr(),(char **)NULL,10);
    }
    else return 0;
}

/**
 * @brief  文字列を実数値(float型)に変換
 *
 *  自身が持つ文字列の位置pos 以降の文字を実数値に変換し，返します．
 *
 * @param      pos 自身が持つ文字列の変換開始位置
 * @return     実数
 * @throw      内部バッファの確保に失敗した場合
 * @note       基数に対して有効でない数字が現れた時点で変換は終了します．
 *
 */
double tstring::atof( size_t pos ) const
{
    const char *this_str_ptr = this->str_ptr_cs();
    if ( this_str_ptr != NULL && pos < this->length() ) 
	return this->strtod(pos, (size_t *)NULL);
    else return 0;
}

/**
 * @brief  文字列を実数値(float型)に変換
 *
 *  自身が持つ文字列の位置pos 以降の文字を実数値に変換し，返します．
 *
 * @param      pos 自身が持つ文字列の変換開始位置
 * @param      n 実数値に変換する文字数
 * @return     実数
 * @throw      内部バッファの確保に失敗した場合
 * @note       基数に対して有効でない数字が現れた時点で変換は終了します．
 *
 */
double tstring::atof( size_t pos, size_t n ) const
{
    const char *this_str_ptr = this->str_ptr_cs();
    if ( this_str_ptr != NULL && pos < this->length() ) {
	return this->strtod(pos, n, (size_t *)NULL);
    }
    else return 0;
}

/**
 * @brief  文字列を指定された基数で整数値(long型)に変換
 *
 *  自身の文字列を，基数base で整数値に変換し，返します．
 *
 * @param      base 基数
 * @param      endpos 自身の文字列の変換されなかった文字の位置
 * @return     整数
 *
 */
long tstring::strtol( int base, size_t *endpos ) const
{
    return this->strtol(0,base,endpos);
}

/**
 * @brief  文字列を指定された基数で整数値(long型)に変換
 *
 *  自身の文字列を，基数base で整数値に変換し，返します．
 *
 * @param      pos 自身が持つ文字列の変換開始位置
 * @param      base 基数
 * @param      endpos 自身の文字列の変換されなかった文字の位置
 * @return     整数
 *
 */
long tstring::strtol( size_t pos, int base, size_t *endpos ) const
{
    const char *this_str_ptr = this->str_ptr_cs();
    if ( this_str_ptr != NULL && pos < this->length() ) {
	char *endptr;
	long ret = c_strtol(this_str_ptr + pos ,&endptr,base);
	if ( endpos != NULL ) *endpos = endptr - this_str_ptr;
	return ret;
    }
    else {
	if ( endpos != NULL ) 
	    *endpos = this->length();
	return 0;
    }
}

/**
 * @brief  文字列を指定された基数で整数値(long型)に変換
 *
 *  自身の文字列を，基数base で整数値に変換し，返します．
 *
 * @param      pos 自身が持つ文字列の変換開始位置
 * @param      n 整数値に変換する文字数
 * @param      base 基数
 * @param      endpos 自身の文字列の変換されなかった文字の位置
 * @return     整数
 * @throw      内部バッファの確保に失敗した場合
 *
 */
long tstring::strtol( size_t pos, size_t n, int base, size_t *endpos ) const
{
    const char *this_str_ptr = this->str_ptr_cs();
    if ( this_str_ptr != NULL && pos < this->length() ) {
	tstring s;
	long ret;
	char *endptr;
	s.assign(this_str_ptr + pos,n);
	ret = c_strtol(s.cstr(),&endptr,base);
	if ( endpos != NULL ) *endpos = pos + (endptr - s.cstr());
	return ret;
    }
    else {
	if ( endpos != NULL ) 
	    *endpos = this->length();
	return 0;
    }
}

/**
 * @brief  文字列を指定された基数で整数値(long long型)に変換
 *
 *  自身の文字列を，基数base で整数値に変換し，返します．
 *
 * @param      base 基数
 * @param      endpos 自身の文字列の変換されなかった文字の位置
 * @return     整数
 *
 */
long long tstring::strtoll( int base, size_t *endpos ) const
{
    return this->strtoll(0,base,endpos);
}

/**
 * @brief  文字列を指定された基数で整数値(long long型)に変換
 *
 *  自身の文字列を，基数base で整数値に変換し，返します．
 *
 * @param      pos 自身が持つ文字列の変換開始位置
 * @param      base 基数
 * @param      endpos 自身の文字列の変換されなかった文字の位置
 * @return     整数
 *
 */
long long tstring::strtoll( size_t pos, int base, size_t *endpos ) const
{
    const char *this_str_ptr = this->str_ptr_cs();
    if ( this_str_ptr != NULL && pos < this->length() ) {
	char *endptr;
	long long ret = c_strtoll(this_str_ptr + pos ,&endptr,base);
	if ( endpos != NULL ) *endpos = endptr - this_str_ptr;
	return ret;
    }
    else {
	if ( endpos != NULL ) 
	    *endpos = this->length();
	return 0;
    }
}

/**
 * @brief  文字列を指定された基数で整数値(long long型)に変換
 *
 *  自身の文字列を，基数base で整数値に変換し，返します．
 *
 * @param      pos 自身が持つ文字列の変換開始位置
 * @param      n 整数値に変換する文字数
 * @param      base 基数
 * @param      endpos 自身の文字列の変換されなかった文字の位置
 * @return     整数
 * @throw      内部バッファの確保に失敗した場合
 *
 */
long long tstring::strtoll( size_t pos, size_t n, 
			    int base, size_t *endpos ) const
{
    const char *this_str_ptr = this->str_ptr_cs();
    if ( this_str_ptr != NULL && pos < this->length() ) {
	tstring s;
	long long ret;
	char *endptr;
	s.assign(this_str_ptr + pos,n);
	ret = c_strtoll(s.cstr(),&endptr,base);
	if ( endpos != NULL ) *endpos = pos + (endptr - s.cstr());
	return ret;
    }
    else {
	if ( endpos != NULL ) 
	    *endpos = this->length();
	return 0;
    }
}

/**
 * @brief  文字列を指定された基数で符号無し整数値(unsigned long型)に変換
 *
 *  自身の文字列を，基数base で符号無し整数値に変換し，返します．
 *
 * @param      base 基数
 * @param      endpos 自身の文字列の変換されなかった文字の位置
 * @return     整数
 *
 */
unsigned long tstring::strtoul( int base, size_t *endpos ) const
{
    return this->strtoul(0,base,endpos);
}

/**
 * @brief  文字列を指定された基数で符号無し整数値(unsigned long型)に変換
 *
 *  自身の文字列を，基数base で符号無し整数値に変換し，返します．
 *
 * @param      pos 自身が持つ文字列の変換開始位置
 * @param      base 基数
 * @param      endpos 自身の文字列の変換されなかった文字の位置
 * @return     整数
 *
 */
unsigned long tstring::strtoul( size_t pos, int base, size_t *endpos ) const
{
    const char *this_str_ptr = this->str_ptr_cs();
    if ( this_str_ptr != NULL && pos < this->length() ) {
	char *endptr;
	long ret = c_strtoul(this_str_ptr + pos ,&endptr,base);
	if ( endpos != NULL ) *endpos = endptr - this_str_ptr;
	return ret;
    }
    else {
	if ( endpos != NULL ) 
	    *endpos = this->length();
	return 0;
    }
}

/**
 * @brief  文字列を指定された基数で符号無し整数値(unsigned long型)に変換
 *
 *  自身の文字列を，基数base で符号無し整数値に変換し，返します．
 *
 * @param      pos 自身が持つ文字列の変換開始位置
 * @param      n 整数値に変換する文字数
 * @param      base 基数
 * @param      endpos 自身の文字列の変換されなかった文字の位置
 * @return     整数
 * @throw      内部バッファの確保に失敗した場合
 *
 */
unsigned long tstring::strtoul( size_t pos, size_t n, 
				int base, size_t *endpos ) const
{
    const char *this_str_ptr = this->str_ptr_cs();
    if ( this_str_ptr != NULL && pos < this->length() ) {
	tstring s;
	long ret;
	char *endptr;
	s.assign(this_str_ptr + pos,n);
	ret = c_strtoul(s.cstr(),&endptr,base);
	if ( endpos != NULL ) *endpos = pos + (endptr - s.cstr());
	return ret;
    }
    else {
	if ( endpos != NULL ) 
	    *endpos = this->length();
	return 0;
    }
}

/**
 * @brief  文字列を指定された基数で符号無し整数値(unsigned long long型)に変換
 *
 *  自身の文字列を，基数base で符号無し整数値に変換し，返します．
 *
 * @param      base 基数
 * @param      endpos 自身の文字列の変換されなかった文字の位置
 * @return     整数
 *
 */
unsigned long long tstring::strtoull( int base, size_t *endpos ) const
{
    return this->strtoull(0,base,endpos);
}

/**
 * @brief  文字列を指定された基数で符号無し整数値(unsigned long long型)に変換
 *
 *  自身の文字列を，基数base で符号無し整数値に変換し，返します．
 *
 * @param      pos 自身が持つ文字列の変換開始位置
 * @param      base 基数
 * @param      endpos 自身の文字列の変換されなかった文字の位置
 * @return     整数
 *
 */
unsigned long long tstring::strtoull( size_t pos, 
				      int base, size_t *endpos ) const
{
    const char *this_str_ptr = this->str_ptr_cs();
    if ( this_str_ptr != NULL && pos < this->length() ) {
	char *endptr;
	long long ret = c_strtoull(this_str_ptr + pos ,&endptr,base);
	if ( endpos != NULL ) *endpos = endptr - this_str_ptr;
	return ret;
    }
    else {
	if ( endpos != NULL ) 
	    *endpos = this->length();
	return 0;
    }
}

/**
 * @brief  文字列を指定された基数で符号無し整数値(unsigned long long型)に変換
 *
 * 自身の文字列を，基数base で符号無し整数値に変換し，返します．
 *
 * @param      pos 自身が持つ文字列の変換開始位置
 * @param      n 整数値に変換する文字数
 * @param      base 基数
 * @param      endpos 自身の文字列の変換されなかった文字の位置
 * @return     整数
 * @throw      内部バッファの確保に失敗した場合
 *
 */
unsigned long long tstring::strtoull( size_t pos, size_t n,
				      int base, size_t *endpos ) const
{
    const char *this_str_ptr = this->str_ptr_cs();
    if ( this_str_ptr != NULL && pos < this->length() ) {
	tstring s;
	long long ret;
	char *endptr;
	s.assign(this_str_ptr + pos,n);
	ret = c_strtoull(s.cstr(),&endptr,base);
	if ( endpos != NULL ) *endpos = pos + (endptr - s.cstr());
	return ret;
    }
    else {
	if ( endpos != NULL ) 
	    *endpos = this->length();
	return 0;
    }
}

/**
 * @brief  文字列を実数値(double型)に変換
 *
 *  自身の文字列を，実数値に変換し，返します．<br>
 *  変換されなかった文字列の位置をendposに返します．
 *
 * @param      endpos 自身の文字列の変換されなかった文字の位置
 * @return     実数
 * @throw      内部バッファの確保に失敗した場合
 * @note       FORTRAN形式の「1.23D12」「1.23d12」のように，「e」のかわりに
 *             「d」が使われている場合にも対応しています．
 *
 */
double tstring::strtod( size_t *endpos ) const
{
    return this->strtod(0,endpos);
}

/**
 * @brief  文字列を実数値(double型)に変換
 *
 *  自身の文字列を，実数値に変換し，返します．<br>
 *  変換されなかった文字列の位置をendposに返します．
 *
 * @param      pos 自身が持つ文字列の変換開始位置
 * @param      endpos 自身の文字列の変換されなかった文字の位置
 * @return     実数
 * @throw      内部バッファの確保に失敗した場合
 * @note       FORTRAN形式の「1.23D12」「1.23d12」のように，「e」のかわりに
 *             「d」が使われている場合にも対応しています．
 *
 */
double tstring::strtod( size_t pos, size_t *endpos ) const
{
    const char *this_str_ptr = this->str_ptr_cs();
    if ( this_str_ptr != NULL && pos < this->length() ) {
	char *endptr;
	double ret = c_strtod(this_str_ptr + pos ,&endptr);
	/* 1.23D12 の形式でもいけるようにする */
	if ( this_str_ptr + pos < endptr && (*endptr == 'd'||*endptr == 'D') ){
	    tstring sv;
	    sv.assign(this_str_ptr).put(endptr - this_str_ptr, 'e', 1);
	    const char *sv_cstr = sv.cstr();
	    ret = c_strtod(sv_cstr + pos, &endptr);
	    if ( endpos != NULL ) *endpos = endptr - sv_cstr;
	    return ret;
	}
	if ( endpos != NULL ) *endpos = endptr - this_str_ptr;
	return ret;
    }
    else {
	if ( endpos != NULL ) 
	    *endpos = this->length();
	return 0;
    }
}

/**
 * @brief  文字列を実数値(double型)に変換
 *
 *  自身の文字列を，実数値に変換し，返します．<br>
 *  また，変換されなかった文字列の位置をendposに返します．
 *
 * @param      pos 自身が持つ文字列の変換開始位置
 * @param      n 実数値に変換する文字数
 * @param      endpos 自身の文字列の変換されなかった文字の位置
 * @return     実数
 * @throw      内部バッファの確保に失敗した場合
 * @note       FORTRAN形式の「1.23D12」「1.23d12」のように，「e」のかわりに
 *             「d」が使われている場合にも対応しています．
 *
 */
double tstring::strtod( size_t pos, size_t n, size_t *endpos ) const
{
    const char *this_str_ptr = this->str_ptr_cs();
    if ( this_str_ptr != NULL && pos < this->length() ) {
	tstring s;
	double ret;
	char *endptr;
	s.assign(this_str_ptr + pos,n);
	const char *s_cstr = s.cstr();
	ret = c_strtod(s_cstr,&endptr);
	/* 1.23D12 の形式でもいけるようにする */
	if ( s_cstr < endptr && (*endptr == 'd' || *endptr == 'D') ) {
	    tstring sv;
	    sv.assign(s_cstr).put(endptr - s_cstr, 'e', 1);
	    const char *sv_cstr = sv.cstr();
	    ret = c_strtod(sv_cstr, &endptr);
	    if ( endpos != NULL ) *endpos = pos + (endptr - sv_cstr);
	    return ret;
	}
	if ( endpos != NULL ) *endpos = pos + (endptr - s_cstr);
	return ret;
    }
    else {
	if ( endpos != NULL ) 
	    *endpos = this->length();
	return 0;
    }
}

/* 互換用 */
#if 0
long tstring::strtol( size_t *endpos, int base ) const
{
    return this->strtol(base,endpos);
}
long tstring::strtol( size_t pos, size_t *endpos, int base ) const
{
    return this->strtol(pos,base,endpos);
}
long tstring::strtol( size_t pos, size_t n, size_t *endpos, int base ) const
{
    return this->strtol(pos,n,base,endpos);
}
long long tstring::strtoll( size_t *endpos, int base ) const
{
    return this->strtoll(base,endpos);
}
long long tstring::strtoll( size_t pos, size_t *endpos, int base ) const
{
    return this->strtoll(pos,base,endpos);
}
long long tstring::strtoll( size_t pos, size_t n, size_t *endpos, int base ) const
{
    return this->strtoll(pos,n,base,endpos);
}
unsigned long tstring::strtoul( size_t *endpos, int base ) const
{
    return this->strtoul(base,endpos);
}
unsigned long tstring::strtoul( size_t pos, size_t *endpos, int base ) const
{
    return this->strtoul(pos,base,endpos);
}
unsigned long tstring::strtoul( size_t pos, size_t n, size_t *endpos, int base ) const
{
    return this->strtoul(pos,n,base,endpos);
}
unsigned long long tstring::strtoull( size_t *endpos, int base ) const
{
    return this->strtoull(base,endpos);
}
unsigned long long tstring::strtoull( size_t pos, size_t *endpos, int base ) const
{
    return this->strtoull(pos,base,endpos);
}
unsigned long long tstring::strtoull( size_t pos, size_t n, size_t *endpos, int base ) const
{
    return this->strtoull(pos,n,base,endpos);
}
#endif


/**
 * @brief  文字列を format の指定に従って変換
 *
 *  自身の文字列を format の指定に従って変換し，format 以降の引数に格納します．
 *
 * @param      format 読み込みフォーマット指定
 * @param      ap 書き込み先となる可変長引数のリスト
 * @return     成功した場合は非負数<br>
 *             失敗した場合はEOF
 * @attention  format に "%s" が指定された場合，格納バッファの大きさ以上の
 *             文字列が入力されるとバッファオーバーランが発生します．
 * 
 */
int tstring::vscanf( const char *format, va_list ap ) const
{
    int return_status = EOF;
    const char *line;

    line = this->cstr();
    if ( line == NULL ) goto quit;

    return_status = c_vsscanf(line,format,ap);

 quit:
    return return_status;
}

/**
 * @brief  文字列を format の指定に従って変換
 *
 *  自身の文字列をformat の指定に従って変換し，format 以降の引数に格納します．
 *
 * @param      format 読み込みフォーマット指定
 * @param      ... 書き込み先となる可変長引数の各要素データ
 * @return     成功した場合は非負数<br>
 *             失敗した場合はEOF
 * @attention  format に "%s" が指定された場合，格納バッファの大きさ以上の
 *             文字列が入力されるとバッファオーバーランが発生します．
 * 
 */
int tstring::scanf( const char *format, ... ) const
{
    int return_status;
    va_list ap;
    va_start(ap, format);
    return_status = this->vscanf( format, ap );
    va_end(ap);
    return return_status;
}

/**
 * @brief  文字列の辞書的な比較
 *
 *  自身の文字列と指定された文字列 str とを辞書的に比較し，その結果を返します．
 *
 * @param   pos 自身の文字列の開始位置
 * @param   str 比較に用いる文字列
 * @param   n 比較する文字数
 * @return  自身の文字列がstr と等しい場合は0
 * @return  自身の文字列がstr に較べて辞書的に大きい場合は正値
 * @return  自身の文字列がstr に較べて辞書的に小さい場合は負値
 * @return  自身が文字列バッファを持ち，str に NULL を指定した場合は256(エラー)
 * @return  自身が文字列バッファを持たず，str を指定した場合は-256(エラー)
 * 
 */
int tstring::compare( size_t pos, const char *str, size_t n ) const
{
    if ( this->length() < pos ) return -256;

    if ( this->cstr() == NULL ) {
	if ( str == NULL && n == 0 ) return 0;
	else return -256;
    }
    else {
	if ( str == NULL ) return 256;
	else return c_strncmp(this->cstr()+pos,str,n);
    }
}

/**
 * @brief  文字列の辞書的な比較
 *
 *  自身の文字列と指定された文字列 str とを辞書的に比較し，その結果を返します．
 *
 * @param   pos 自身が持つ文字列の開始位置
 * @param   str 比較に用いる文字列
 * @return  自身の文字列がstr と等しい場合は0
 * @return  自身の文字列がstr に較べて辞書的に大きい場合は正値
 * @return  自身の文字列がstr に較べて辞書的に小さい場合は負値
 * @return  自身が文字列バッファを持ち，str に NULL を指定した場合は256(エラー)
 * @return  自身が文字列バッファを持たず，str を指定した場合は-256(エラー)
 * 
 */
int tstring::compare( size_t pos, const char *str ) const
{
    if ( this->length() < pos ) return -256;

    if ( str == NULL ) {
	if ( this->cstr() == NULL ) return 0;
	else return -256;
    }

    if ( this->cstr() == NULL ) {
	if ( str == NULL ) return 0;
	else return -256;
    }
    else {
	if ( str == NULL ) return 256;
	else return c_strcmp(this->cstr()+pos,str);
    }
}

/**
 * @brief  文字列の辞書的な比較
 *
 *  自身の文字列と指定された文字列 str とを辞書的に比較し，その結果を返します．
 *
 * @param   str 比較に用いる文字列
 * @param   n 比較する文字数
 * @return  自身の文字列がstr と等しい場合は0
 * @return  自身の文字列がstr に較べて辞書的に大きい場合は正値
 * @return  自身の文字列がstr に較べて辞書的に小さい場合は負値
 * @return  自身が文字列バッファを持ち，str に NULL を指定した場合は256(エラー)
 * @return  自身が文字列バッファを持たず，str を指定した場合は-256(エラー)
 * 
 */
int tstring::compare( const char *str, size_t n ) const
{
    if ( this->cstr() == NULL ) {
	if ( str == NULL ) return 0;
	else return -256;
    }
    else {
	if ( str == NULL ) return 256;
	else return c_strncmp(this->cstr(),str,n);
    }
}

/**
 * @brief  文字列の辞書的な比較
 *
 *  自身の文字列と指定された文字列 str とを辞書的に比較し，その結果を返します．
 *
 * @param   str 比較に用いる文字列
 * @return  自身の文字列がstr と等しい場合は0
 * @return  自身の文字列がstr に較べて辞書的に大きい場合は正値
 * @return  自身の文字列がstr に較べて辞書的に小さい場合は負値
 * @return  自身が文字列バッファを持ち，str に NULL を指定した場合は256(エラー)
 * @return  自身が文字列バッファを持たず，str を指定した場合は-256(エラー)
 * 
 */
int tstring::compare( const char *str ) const
{
    if ( this->cstr() == NULL ) {
	if ( str == NULL ) return 0;
	else return -256;
    }
    else {
	if ( str == NULL ) return 256;
	else return c_strcmp(this->cstr(),str);
    }
}

/**
 * @brief  文字列の辞書的な比較
 *
 *  自身の文字列と指定された文字列 str とを辞書的に比較し，その結果を返します．
 *
 * @param   pos1 自身が持つ文字列の開始位置
 * @param   str 比較に用いる文字列
 * @param   pos2 オブジェクトstr が持つ文字列の開始位置
 * @return  自身の文字列がstr と等しい場合は0
 * @return  自身の文字列がstr に較べて辞書的に大きい場合は正値
 * @return  自身の文字列がstr に較べて辞書的に小さい場合は負値
 * @return  自身が文字列バッファを持ち，str に NULL を指定した場合は256(エラー)
 * @return  自身が文字列バッファを持たず，str を指定した場合は-256(エラー)
 * 
 */
int tstring::compare( size_t pos1, 
		      const tstring &str, size_t pos2 ) const
{
    if ( str.length() < pos2 ) return this->strcmp(pos1, (char *)NULL);
    else if ( this->length() < pos1 ) return -256;
    else return c_binstrcmp(this->cstr() + pos1, this->length() - pos1,
			    str.cstr() + pos2, str.length() - pos2 );
}

/**
 * @brief  文字列の辞書的な比較
 *
 *  自身の文字列と指定された文字列 str とを辞書的に比較し，その結果を返します．
 *
 * @param   pos1 自身の文字列の開始位置
 * @param   str 比較に用いる文字列
 * @param   pos2 str が持つ文字列の開始位置
 * @param   n 比較する文字数
 * @return  自身の文字列がstr と等しい場合は0
 * @return  自身の文字列がstr に較べて辞書的に大きい場合は正値
 * @return  自身の文字列がstr に較べて辞書的に小さい場合は負値
 * @return  自身が文字列バッファを持ち，str に NULL を指定した場合は256(エラー)
 * @return  自身が文字列バッファを持たず，str を指定した場合は-256(エラー)
 * 
 */
int tstring::compare( size_t pos1, 
		      const tstring &str, size_t pos2, size_t n ) const
{
    if ( str.length() < pos2 ) return this->strncmp(pos1, (char *)NULL, n);
    else if ( this->length() < pos1 ) return -256;
    else return c_binstrncmp(this->cstr() + pos1, this->length() - pos1,
			     str.cstr() + pos2, str.length() - pos2, n);
}

/**
 * @brief  文字列の辞書的な比較
 *
 *  自身の文字列と指定された文字列 str とを辞書的に比較し，その結果を返します．
 *
 * @param   str 比較に用いる文字列
 * @param   pos2 オブジェクトstr が持つ文字列の開始位置
 * @return  自身の文字列がstr と等しい場合は0
 * @return  自身の文字列がstr に較べて辞書的に大きい場合は正値
 * @return  自身の文字列がstr に較べて辞書的に小さい場合は負値
 * @return  自身が文字列バッファを持ち，str に NULL を指定した場合は256(エラー)
 * @return  自身が文字列バッファを持たず，str を指定した場合は-256(エラー)
 * 
 */
int tstring::compare( const tstring &str, size_t pos2 ) const
{
    return this->compare(0,str,pos2);
}

/**
 * @brief  文字列の辞書的な比較
 *
 *  自身の文字列と指定された文字列 str とを辞書的に比較し，その結果を返します．
 *
 * @param   str 比較に用いる文字列
 * @param   pos2 str が持つ文字列の開始位置
 * @param   n 比較する文字数
 * @return  自身の文字列がstr と等しい場合は0
 * @return  自身の文字列がstr に較べて辞書的に大きい場合は正値
 * @return  自身の文字列がstr に較べて辞書的に小さい場合は負値
 * @return  自身が文字列バッファを持ち，str に NULL を指定した場合は256(エラー)
 * @return  自身が文字列バッファを持たず，str を指定した場合は-256(エラー)
 * 
 */
int tstring::compare( const tstring &str, size_t pos2, size_t n ) const
{
    return this->compare(0,str,pos2,n);
}

/**
 * @brief  文字列の辞書的な比較
 *
 *  自身の文字列と指定された文字列 str とを辞書的に比較し，その結果を返します．
 *
 * @param   pos 自身の文字列の開始位置
 * @param   str 比較に用いる文字列
 * @param   n 比較する文字数
 * @return  自身の文字列がstr と等しい場合は0
 * @return  自身の文字列がstr に較べて辞書的に大きい場合は正値
 * @return  自身の文字列がstr に較べて辞書的に小さい場合は負値
 * @return  自身が文字列バッファを持ち，str に NULL を指定した場合は256(エラー)
 * @return  自身が文字列バッファを持たず，str を指定した場合は-256(エラー)
 * 
 */
int tstring::strncmp( size_t pos, const char *str, size_t n ) const
{
    return this->compare(pos,str,n);
}

/**
 * @brief  文字列の辞書的な比較
 *
 *  自身の文字列と指定された文字列 str とを辞書的に比較し，その結果を返します．
 *
 * @param   pos 自身が持つ文字列の開始位置
 * @param   str 比較に用いる文字列
 * @return  自身の文字列がstr と等しい場合は0
 * @return  自身の文字列がstr に較べて辞書的に大きい場合は正値
 * @return  自身の文字列がstr に較べて辞書的に小さい場合は負値
 * @return  自身が文字列バッファを持ち，str に NULL を指定した場合は256(エラー)
 * @return  自身が文字列バッファを持たず，str を指定した場合は-256(エラー)
 * 
 */
int tstring::strcmp( size_t pos, const char *str ) const
{
    return this->compare(pos,str);
}

/**
 * @brief  文字列の辞書的な比較
 *
 *  自身の文字列と指定された文字列 str とを辞書的に比較し，その結果を返します．
 *
 * @param   str 比較に用いる文字列
 * @param   n 比較する文字数
 * @return  自身の文字列がstr と等しい場合は0
 * @return  自身の文字列がstr に較べて辞書的に大きい場合は正値
 * @return  自身の文字列がstr に較べて辞書的に小さい場合は負値
 * @return  自身が文字列バッファを持ち，str に NULL を指定した場合は256(エラー)
 * @return  自身が文字列バッファを持たず，str を指定した場合は-256(エラー)
 * 
 */
int tstring::strncmp( const char *str, size_t n ) const
{
    return this->compare(str,n);
}

/**
 * @brief  文字列の辞書的な比較
 *
 *  自身の文字列と指定された文字列 str とを辞書的に比較し，その結果を返します．
 *
 * @param   str 比較に用いる文字列
 * @return  自身の文字列がstr と等しい場合は0
 * @return  自身の文字列がstr に較べて辞書的に大きい場合は正値
 * @return  自身の文字列がstr に較べて辞書的に小さい場合は負値
 * @return  自身が文字列バッファを持ち，str に NULL を指定した場合は256(エラー)
 * @return  自身が文字列バッファを持たず，str を指定した場合は-256(エラー)
 * 
 */
int tstring::strcmp( const char *str ) const
{
    return this->compare(str);
}

/**
 * @brief  文字列の辞書的な比較
 *
 *  自身の文字列と指定された文字列 str とを辞書的に比較し，その結果を返します．
 *
 * @param   pos1 自身が持つ文字列の開始位置
 * @param   str 比較に用いる文字列
 * @param   pos2 オブジェクトstr が持つ文字列の開始位置
 * @return  自身の文字列がstr と等しい場合は0
 * @return  自身の文字列がstr に較べて辞書的に大きい場合は正値
 * @return  自身の文字列がstr に較べて辞書的に小さい場合は負値
 * @return  自身が文字列バッファを持ち，str に NULL を指定した場合は256(エラー)
 * @return  自身が文字列バッファを持たず，str を指定した場合は-256(エラー)
 * 
 */
int tstring::strcmp( size_t pos1, 
		     const tstring &str, size_t pos2 ) const
{
    if ( str.length() < pos2 ) return this->strcmp(pos1, (char *)NULL);
    else if ( this->length() < pos1 ) return -256;
    else return c_binstrcmp(this->cstr() + pos1, this->length() - pos1,
			    str.cstr() + pos2, str.length() - pos2 );
}

/**
 * @brief  文字列の辞書的な比較
 *
 *  自身の文字列と指定された文字列 str とを辞書的に比較し，その結果を返します．
 *
 * @param   pos1 自身の文字列の開始位置
 * @param   str 比較に用いる文字列
 * @param   pos2 str が持つ文字列の開始位置
 * @param   n 比較する文字数
 * @return  自身の文字列がstr と等しい場合は0
 * @return  自身の文字列がstr に較べて辞書的に大きい場合は正値
 * @return  自身の文字列がstr に較べて辞書的に小さい場合は負値
 * @return  自身が文字列バッファを持ち，str に NULL を指定した場合は256(エラー)
 * @return  自身が文字列バッファを持たず，str を指定した場合は-256(エラー)
 * 
 */
int tstring::strncmp( size_t pos1, 
		      const tstring &str, size_t pos2, size_t n ) const
{
    if ( str.length() < pos2 ) return this->strncmp(pos1, (char *)NULL, n);
    else if ( this->length() < pos1 ) return -256;
    else return c_binstrncmp(this->cstr() + pos1, this->length() - pos1,
			     str.cstr() + pos2, str.length() - pos2, n);
}

/**
 * @brief  文字列の辞書的な比較
 *
 *  自身の文字列と指定された文字列 str とを辞書的に比較し，その結果を返します．
 *
 * @param   str 比較に用いる文字列
 * @param   pos2 オブジェクトstr が持つ文字列の開始位置
 * @return  自身の文字列がstr と等しい場合は0
 * @return  自身の文字列がstr に較べて辞書的に大きい場合は正値
 * @return  自身の文字列がstr に較べて辞書的に小さい場合は負値
 * @return  自身が文字列バッファを持ち，str に NULL を指定した場合は256(エラー)
 * @return  自身が文字列バッファを持たず，str を指定した場合は-256(エラー)
 * 
 */
int tstring::strcmp( const tstring &str, size_t pos2 ) const
{
    return this->strcmp(0,str,pos2);
}

/**
 * @brief  文字列の辞書的な比較
 *
 *  自身の文字列と指定された文字列 str とを辞書的に比較し，その結果を返します．
 *
 * @param   str 比較に用いる文字列
 * @param   pos2 str が持つ文字列の開始位置
 * @param   n 比較する文字数
 * @return  自身の文字列がstr と等しい場合は0
 * @return  自身の文字列がstr に較べて辞書的に大きい場合は正値
 * @return  自身の文字列がstr に較べて辞書的に小さい場合は負値
 * @return  自身が文字列バッファを持ち，str に NULL を指定した場合は256(エラー)
 * @return  自身が文字列バッファを持たず，str を指定した場合は-256(エラー)
 * 
 */
int tstring::strncmp( const tstring &str, size_t pos2, size_t n ) const
{
    return this->strncmp(0,str,pos2,n);
}

/**
 * @brief  大文字・小文字を区別しない文字列の辞書的な比較
 *
 *  自身の文字列と指定された文字列 str とを，アルファベットの大文字と小文字と
 *  を区別せず辞書的に比較し，その結果を返します．
 *
 * @param   pos1 自身の文字列の開始位置
 * @param   str 比較に用いる文字列
 * @param   n 比較する文字数
 * @return  自身の文字列がstr と等しい場合は0
 * @return  自身の文字列がstr に較べて辞書的に大きい場合は正値
 * @return  自身の文字列がstr に較べて辞書的に小さい場合は負値
 * @return  自身が文字列バッファを持ち，str に NULL を指定した場合は256(エラー)
 * @return  自身が文字列バッファを持たず，str を指定した場合は-256(エラー)
 * 
 */
int tstring::strncasecmp( size_t pos, const char *str, size_t n ) const
{
    if ( this->length() < pos ) return -256;

    if ( this->cstr() == NULL ) {
	if ( str == NULL && n == 0 ) return 0;
	else return -256;
    }
    else {
	if ( str == NULL ) return 256;
	else return c_strncasecmp(this->cstr()+pos,str,n);
    }
}

/**
 * @brief  大文字・小文字を区別しない文字列の辞書的な比較
 *
 *  自身の文字列と指定された文字列 str とを，アルファベットの大文字と小文字と
 *  を区別せず辞書的に比較し，その結果を返します．
 *
 * @param   pos1 自身の文字列の開始位置
 * @param   str 比較に用いる文字列
 * @return  自身の文字列がstr と等しい場合は0
 * @return  自身の文字列がstr に較べて辞書的に大きい場合は正値
 * @return  自身の文字列がstr に較べて辞書的に小さい場合は負値
 * @return  自身が文字列バッファを持ち，str に NULL を指定した場合は256(エラー)
 * @return  自身が文字列バッファを持たず，str を指定した場合は-256(エラー)
 * 
 */
int tstring::strcasecmp( size_t pos, const char *str ) const
{
    if ( this->length() < pos ) return -256;

    if ( str == NULL ) {
	if ( this->cstr() == NULL ) return 0;
	else return -256;
    }

    if ( this->cstr() == NULL ) {
	if ( str == NULL ) return 0;
	else return -256;
    }
    else {
	if ( str == NULL ) return 256;
	else return c_strcasecmp(this->cstr()+pos,str);
    }
}

/**
 * @brief  大文字・小文字を区別しない文字列の辞書的な比較
 *
 *  自身の文字列と指定された文字列 str とを，アルファベットの大文字と小文字と
 *  を区別せず辞書的に比較し，その結果を返します．
 *
 * @param   str 比較に用いる文字列
 * @param   n 比較する文字数
 * @return  自身の文字列がstr と等しい場合は0
 * @return  自身の文字列がstr に較べて辞書的に大きい場合は正値
 * @return  自身の文字列がstr に較べて辞書的に小さい場合は負値
 * @return  自身が文字列バッファを持ち，str に NULL を指定した場合は256(エラー)
 * @return  自身が文字列バッファを持たず，str を指定した場合は-256(エラー)
 * 
 */
int tstring::strncasecmp( const char *str, size_t n ) const
{
    if ( this->cstr() == NULL ) {
	if ( str == NULL ) return 0;
	else return -256;
    }
    else {
	if ( str == NULL ) return 256;
	else return c_strncasecmp(this->cstr(),str,n);
    }
}

/**
 * @brief  大文字・小文字を区別しない文字列の辞書的な比較
 *
 *  自身の文字列と指定された文字列 str とを，アルファベットの大文字と小文字と
 *  を区別せず辞書的に比較し，その結果を返します．
 *
 * @param   str 比較に用いる文字列
 * @return  自身の文字列がstr と等しい場合は0
 * @return  自身の文字列がstr に較べて辞書的に大きい場合は正値
 * @return  自身の文字列がstr に較べて辞書的に小さい場合は負値
 * @return  自身が文字列バッファを持ち，str に NULL を指定した場合は256(エラー)
 * @return  自身が文字列バッファを持たず，str を指定した場合は-256(エラー)
 * 
 */
int tstring::strcasecmp( const char *str ) const
{
    if ( this->cstr() == NULL ) {
	if ( str == NULL ) return 0;
	else return -256;
    }
    else {
	if ( str == NULL ) return 256;
	else return c_strcasecmp(this->cstr(),str);
    }
}

/**
 * @brief  大文字・小文字を区別しない文字列の辞書的な比較
 *
 *  自身の文字列と指定された文字列 str とを，アルファベットの大文字と小文字と
 *  を区別せず辞書的に比較し，その結果を返します．
 *
 * @param   pos1 自身の文字列の開始位置
 * @param   str 比較に用いる文字列
 * @param   pos2 オブジェクトstr が持つ文字列の開始位置
 * @return  自身の文字列がstr と等しい場合は0
 * @return  自身の文字列がstr に較べて辞書的に大きい場合は正値
 * @return  自身の文字列がstr に較べて辞書的に小さい場合は負値
 * @return  自身が文字列バッファを持ち，str に NULL を指定した場合は256(エラー)
 * @return  自身が文字列バッファを持たず，str を指定した場合は-256(エラー)
 * 
 */
int tstring::strcasecmp( size_t pos1, 
			 const tstring &str, size_t pos2 ) const
{
    if ( str.length() < pos2 ) return this->strcasecmp(pos1, (char *)NULL);
    else if ( this->length() < pos1 ) return -256;
    else return c_binstrcasecmp(this->cstr() + pos1, this->length() - pos1,
				str.cstr() + pos2, str.length() - pos2 );
}

/**
 * @brief  大文字・小文字を区別しない文字列の辞書的な比較
 *
 *  自身の文字列と指定された文字列 str とを，アルファベットの大文字と小文字と
 *  を区別せず辞書的に比較し，その結果を返します．
 *
 * @param   pos1 自身の文字列の開始位置
 * @param   str 比較に用いる文字列
 * @param   pos2 オブジェクトstr が持つ文字列の開始位置
 * @param   n 比較する文字数
 * @return  自身の文字列がstr と等しい場合は0
 * @return  自身の文字列がstr に較べて辞書的に大きい場合は正値
 * @return  自身の文字列がstr に較べて辞書的に小さい場合は負値
 * @return  自身が文字列バッファを持ち，str に NULL を指定した場合は256(エラー)
 * @return  自身が文字列バッファを持たず，str を指定した場合は-256(エラー)
 * 
 */
int tstring::strncasecmp( size_t pos1, 
			  const tstring &str, size_t pos2, size_t n ) const
{
    if ( str.length() < pos2 ) return this->strncasecmp(pos1, (char *)NULL, n);
    else if ( this->length() < pos1 ) return -256;
    else return c_binstrncasecmp(this->cstr() + pos1, this->length() - pos1,
				 str.cstr() + pos2, str.length() - pos2, n);
}

/**
 * @brief  大文字・小文字を区別しない文字列の辞書的な比較
 *
 *  自身の文字列と指定された文字列 str とを，アルファベットの大文字と小文字と
 *  を区別せず辞書的に比較し，その結果を返します．
 *
 * @param   str 比較に用いる文字列
 * @param   pos2 オブジェクトstr が持つ文字列の開始位置
 * @return  自身の文字列がstr と等しい場合は0
 * @return  自身の文字列がstr に較べて辞書的に大きい場合は正値
 * @return  自身の文字列がstr に較べて辞書的に小さい場合は負値
 * @return  自身が文字列バッファを持ち，str に NULL を指定した場合は256(エラー)
 * @return  自身が文字列バッファを持たず，str を指定した場合は-256(エラー)
 * 
 */
int tstring::strcasecmp( const tstring &str, size_t pos2 ) const
{
    return this->strcasecmp(0, str, pos2);
}

/**
 * @brief  大文字・小文字を区別しない文字列の辞書的な比較
 *
 *  自身の文字列と指定された文字列 str とを，アルファベットの大文字と小文字と
 *  を区別せず辞書的に比較し，その結果を返します．
 *
 * @param   str 比較に用いる文字列
 * @param   pos2 オブジェクトstr が持つ文字列の開始位置
 * @param   n 比較する文字数
 * @return  自身の文字列がstr と等しい場合は0
 * @return  自身の文字列がstr に較べて辞書的に大きい場合は正値
 * @return  自身の文字列がstr に較べて辞書的に小さい場合は負値
 * @return  自身が文字列バッファを持ち，str に NULL を指定した場合は256(エラー)
 * @return  自身が文字列バッファを持たず，str を指定した場合は-256(エラー)
 * 
 */
int tstring::strncasecmp( const tstring &str, size_t pos2, size_t n ) const
{
    return this->strncasecmp(0, str, pos2, 2);
}

/**
 * @brief  左側からの文字列(const char *)の検索
 *
 *  自身が持つ文字列の左側から文字列 str を検索し，最初に出現する位置を
 *  返します．
 *
 * @param      str 検出する文字列
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::strstr( const char *str ) const
{
    return this->find((size_t)0,str);
}

/**
 * @brief  左側からの文字列(const char *)の検索
 *
 *  自身が持つ文字列の左側から文字列 str を検索し，最初に出現する位置を
 *  返します．
 *
 * @param      pos 自身の文字列の開始位置
 * @param      str 検出する文字列
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::strstr( size_t pos, const char *str ) const
{
    return this->find(pos,str);
}

/**
 * @brief  左側からの文字列(const char *)の連続的な検索
 *
 *  自身が持つ文字列の左側から文字列 str を検索し，最初に出現する位置を
 *  返します．
 *
 * @param      pos 自身の文字列の開始位置
 * @param      str 検出する文字列
 * @param      nextpos 次回のpos
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::strstr( size_t pos, const char *str, size_t *nextpos ) const
{
    return this->find(pos,str,nextpos);
}

/**
 * @brief  左側からの文字列(tstring)の検索
 *
 *  自身が持つ文字列の左側から文字列 str を検索し，最初に出現する位置を
 *  返します．
 *
 * @param      str 検出する文字列
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::strstr( const tstring &str ) const
{
    return this->find((size_t)0,str.cstr());
}

/**
 * @brief  左側からの文字列(tstring)の検索
 *
 *  自身が持つ文字列の左側から文字列 str を検索し，最初に出現する位置を
 *  返します．
 *
 * @param      pos 自身の文字列の開始位置
 * @param      str 検出する文字列
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::strstr( size_t pos, const tstring &str ) const
{
    return this->find(pos,str.cstr());
}

/**
 * @brief  左側からの文字列(tstring)の連続的な検索
 *
 *  自身が持つ文字列の左側から文字列 str を検索し，最初に出現する位置を
 *  返します．
 *
 * @param      pos 自身の文字列の開始位置
 * @param      str 検出する文字列
 * @param      nextpos 次回のpos
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::strstr(size_t pos, const tstring &str, size_t *nextpos) const
{
    return this->find(pos,str.cstr(),nextpos);
}

/**
 * @brief  左側からの文字列(const char *)の検索
 *
 *  自身が持つ文字列の左側から文字列 str を検索し，最初に出現する位置を
 *  返します．
 *
 * @param      str 検出する文字列
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::find( const char *str ) const
{
    return this->find((size_t)0,str);
}

/**
 * @brief  左側からの文字列(const char *)の検索
 *
 *  自身が持つ文字列の左側から文字列 str を検索し，最初に出現する位置を
 *  返します．
 *
 * @param      str 検出する文字列
 * @param      n 検出する文字数
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::find( const char *str, size_t n ) const
{
    return this->find((size_t)0,str,n);
}

/**
 * @brief  左側からの文字列(const char *)の検索
 *
 *  自身が持つ文字列の左側から文字列 str を検索し，最初に出現する位置を
 *  返します．
 *
 * @param      pos 自身の文字列の開始位置
 * @param      str 検出する文字列
 * @param      nextpos 次回のpos
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::find( size_t pos, const char *str ) const
{
    return this->find(pos,str,(size_t *)NULL);
}

/**
 * @brief  左側からの文字列(const char *)の連続的な検索
 *
 *  自身が持つ文字列の左側から文字列 str を検索し，最初に出現する位置を
 *  返します．
 *
 * @param      pos 自身の文字列の開始位置
 * @param      str 検出する文字列
 * @param      nextpos 次回のpos
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::find( size_t pos, const char *str, size_t *nextpos ) const
{
    if ( this->cstr() != NULL && pos <= this->length() && str != NULL ) {
	size_t len, i;
	len = c_strlen(str);
	for ( i=pos ; i <= this->length() ; i++ ) {
	    if ( c_strncmp(this->cstr() + i, str, len) == 0 ) {
		if ( nextpos != NULL ) {
		    size_t add_span = ((len == 0) ? 1 : len);
		    *nextpos = i + add_span;
		}
		return i;
	    }
	}
    }
    /* not found */
    if ( nextpos != NULL ) *nextpos = this->length() + 1;
    return -1;
}

/**
 * @brief  左側からの文字列(const char *)の検索
 *
 *  自身が持つ文字列の左側から文字列 str を検索し，最初に出現する位置を
 *  返します．
 *
 * @param      pos 自身の文字列の開始位置
 * @param      str 検出する文字列
 * @param      n 検出する文字数
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::find( size_t pos, const char *str, size_t n ) const
{
    return this->find(pos,str,n,(size_t *)NULL);
}

/**
 * @brief  左側からの文字列(const char *)の連続的な検索
 *
 *  自身が持つ文字列の左側から文字列 str を検索し，最初に出現する位置を
 *  返します．
 *
 * @param      pos 自身の文字列の開始位置
 * @param      str 検出する文字列
 * @param      n 検出する文字数
 * @param      nextpos 次回のpos
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::find( size_t pos, const char *str, size_t n,
		       size_t *nextpos ) const
{
    if ( this->cstr() != NULL && pos <= this->length() && str != NULL ) {
	size_t len, i;
	/* n より小さい場合の対応 */
	for ( i=0 ; i < n && str[i] != '\0' ; i++ );
	len = i;
	for ( i=pos ; i <= this->length() ; i++ ) {
	    if ( c_strncmp(this->cstr() + i, str, len) == 0 ) {
		if ( nextpos != NULL ) {
		    size_t add_span = ((len == 0) ? 1 : len);
		    *nextpos = i + add_span;
		}
		return i;
	    }
	}
    }
    /* not found */
    if ( nextpos != NULL ) *nextpos = this->length() + 1;
    return -1;
}

/**
 * @brief  左側からの文字列(tstring)の検索
 *
 *  自身が持つ文字列の左側から文字列 str を検索し，最初に出現する位置を
 *  返します．
 *
 * @param      str 検出する文字列
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::find( const tstring &str ) const
{
    return this->find((size_t)0,str.cstr());
}

/**
 * @brief  左側からの文字列(tstring)の検索
 *
 *  自身が持つ文字列の左側から文字列 str を検索し，最初に出現する位置を
 *  返します．
 *
 * @param      pos 自身の文字列の開始位置
 * @param      str 検出する文字列
 * @param      nextpos 次回のpos
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::find( size_t pos, const tstring &str ) const
{
    return this->find(pos,str.cstr());
}

/**
 * @brief  左側からの文字列(tstring)の連続的な検索
 *
 *  自身が持つ文字列の左側から文字列 str を検索し，最初に出現する位置を
 *  返します．
 *
 * @param      pos 自身の文字列の開始位置
 * @param      str 検出する文字列
 * @param      nextpos 次回のpos
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::find( size_t pos, const tstring &str, size_t *nextpos ) const
{
    return this->find(pos,str.cstr(),nextpos);
}

/* オマケ */

/**
 * @brief  printf()の記法で指定された文字列を左側から検索
 *
 *  自身が持つ文字列の左側から，指定された文字列を検索し，最初に出現する
 *  位置を返します．この関数では，検索する文字列を printf()関数と同様の
 *  フォーマットと可変引数でセットできます．
 *
 * @param   format 検索文字列のためのフォーマット指定 
 * @param   ... formatに対応した可変長引数の各要素
 * @return  成功した場合は非負数<br>
 *          失敗した場合は負値
 */
ssize_t tstring::findf( const char *format, ... ) const
{
    ssize_t return_status;
    va_list ap;
    va_start(ap, format);
    try {
	return_status = this->vfindf( format, ap );
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","this->vfindf() failed");
    }
    va_end(ap);
    return return_status;
}

/**
 * @brief  printf()の記法で指定された文字列を左側から検索
 *
 *  自身が持つ文字列の左側から，指定された文字列を検索し，最初に出現する
 *  位置を返します．この関数では，検索する文字列を printf()関数と同様の
 *  フォーマットと可変引数でセットできます．
 *
 * @param   format 検索文字列のためのフォーマット指定 
 * @param   ap formatに対応した可変長引数のリスト
 * @return  成功した場合は非負数<br>
 *          失敗した場合は負値
 */
ssize_t tstring::vfindf( const char *format, va_list ap ) const
{
    ssize_t return_status = -1;
    heap_mem<char> work_buf;
    if ( format == NULL ) return this->find(format);
    if ( c_vasprintf(work_buf.ptr_address(), format, ap) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","c_vasprintf() failed");
    }
    return_status = this->find(work_buf.ptr());
    return return_status;
}

/**
 * @brief  printf()の記法で指定された文字列を左側から検索
 *
 *  自身が持つ文字列の pos の位置から，指定された文字列を検索し，最初に出現
 *  する位置を返します．この関数では，検索する文字列を printf()関数と同様の
 *  フォーマットと可変引数でセットできます．
 *
 * @param   pos 自身の文字列の開始位置
 * @param   format 検索文字列のためのフォーマット指定 
 * @param   ... formatに対応した可変長引数の各要素
 * @return  成功した場合は非負数<br>
 *          失敗した場合は負値
 */
ssize_t tstring::findf( size_t pos, const char *format, ... ) const
{
    ssize_t return_status;
    va_list ap;
    va_start(ap, format);
    try {
	return_status = this->vfindf( pos, format, ap );
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","this->vfindf() failed");
    }
    va_end(ap);
    return return_status;
}

/**
 * @brief  printf()の記法で指定された文字列を左側から検索
 *
 *  自身が持つ文字列の pos の位置から，指定された文字列を検索し，最初に出現
 *  する位置を返します．この関数では，検索する文字列を printf()関数と同様の
 *  フォーマットと可変引数でセットできます．
 *
 * @param   pos 自身の文字列の開始位置
 * @param   format 検索文字列のためのフォーマット指定 
 * @param   ap formatに対応した可変長引数のリスト
 * @return  成功した場合は非負数<br>
 *          失敗した場合は負値
 */
ssize_t tstring::vfindf( size_t pos, const char *format, va_list ap ) const
{
    ssize_t return_status = -1;
    heap_mem<char> work_buf;
    if ( format == NULL ) return this->find(pos,format);
    if ( c_vasprintf(work_buf.ptr_address(), format, ap) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","c_vasprintf() failed");
    }
    return_status = this->find(pos, work_buf.ptr());
    return return_status;
}

/**
 * @brief  右側からの文字列(const char *)の検索
 *
 *  自身が持つ文字列の右側から左方向に文字列 str を検索し，
 *  最初に出現する位置を返します．
 *
 * @param      str 検出する文字列
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::strrstr( const char *str ) const
{
    return this->rfind(this->length(),str);
}

/**
 * @brief  右側からの文字列(const char *)の検索
 *
 *  自身が持つ文字列の右側から左方向に文字列 str を検索し，
 *  最初に出現する位置を返します．
 *
 * @param      pos オブジェクト内の文字列の開始位置
 * @param      str 検出する文字列
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::strrstr( size_t pos, const char *str ) const
{
    return this->rfind(pos,str);
}

/**
 * @brief  右側からの文字列(const char *)の連続的な検索
 *
 *  自身が持つ文字列の右側から左方向に文字列 str を検索し，
 *  最初に出現する位置を返します．
 *
 * @param      pos オブジェクト内の文字列の開始位置
 * @param      str 検出する文字列
 * @param      nextpos 次回のpos
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::strrstr( size_t pos, const char *str, size_t *nextpos ) const
{
    return this->rfind(pos,str,nextpos);
}

/**
 * @brief  右側からの文字列(tstring)の検索
 *
 *  自身が持つ文字列の右側から左方向に文字列 str を検索し，
 *  最初に出現する位置を返します．
 *
 * @param      str 検出する文字列
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::strrstr( const tstring &str ) const
{
    return this->rfind(this->length(),str.cstr());
}

/**
 * @brief  右側からの文字列(tstring)の検索
 *
 *  自身が持つ文字列の右側から左方向に文字列 str を検索し，
 *  最初に出現する位置を返します．
 *
 * @param      pos オブジェクト内の文字列の開始位置
 * @param      str 検出する文字列
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::strrstr( size_t pos, const tstring &str ) const
{
    return this->rfind(pos,str.cstr());
}

/**
 * @brief  右側からの文字列(tstring)の連続的な検索
 *
 *  自身が持つ文字列の右側から左方向に文字列 str を検索し，
 *  最初に出現する位置を返します．
 *
 * @param      pos オブジェクト内の文字列の開始位置
 * @param      str 検出する文字列
 * @param      nextpos 次回のpos
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::strrstr(size_t pos, const tstring &str, size_t *nextpos) const
{
    return this->rfind(pos,str.cstr(),nextpos);
}

/**
 * @brief  右側からの文字列(const char *)の検索
 *
 *  自身が持つ文字列の右側から左方向に文字列 str を検索し，
 *  最初に出現する位置を返します．
 *
 * @param      str 検出する文字列
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::rfind( const char *str ) const
{
    return this->rfind(this->length(), str);
}

/**
 * @brief  右側からの文字列(const char *)の検索
 *
 *  自身が持つ文字列の右側から左方向に文字列 str を検索し，
 *  最初に出現する位置を返します．
 *
 * @param      str 検出する文字列
 * @param      n 検出する文字数
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::rfind( const char *str, size_t n ) const
{
    return this->rfind(this->length(), str, n);
}

/**
 * @brief  右側からの文字列(const char *)の検索
 *
 *  自身が持つ文字列の右側から左方向に文字列 str を検索し，
 *  最初に出現する位置を返します．
 *
 * @param      pos オブジェクト内の文字列の開始位置
 * @param      str 検出する文字列
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::rfind( size_t pos, const char *str ) const
{
    return this->rfind(pos, str,(size_t *)NULL);
}

/**
 * @brief  右側からの文字列(const char *)の連続的な検索
 *
 *  自身が持つ文字列の右側から左方向に文字列str を検索し，
 *  最初に出現する位置を返します．
 *
 * @param      pos オブジェクト内の文字列の開始位置
 * @param      str 検出する文字列
 * @param      nextpos 次回のpos
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::rfind( size_t pos, const char *str, size_t *nextpos ) const
{
    if ( this->cstr() != NULL && pos <= this->length() && str != NULL ) {
	size_t len, i;
	len = c_strlen(str);
	for ( i=pos+1 ; 0 < i ; ) {
	    i--;
	    if ( c_strncmp(this->cstr() + i, str, len) == 0 ) {
		if ( nextpos != NULL ) {
		    size_t add_span = ((len == 0) ? 1 : len);
		    if ( add_span <= i ) *nextpos = i - add_span;
		    else *nextpos = this->length() + 1;	/* finish */
		}
		return i;
	    }
	}
    }
    /* not found */
    if ( nextpos != NULL ) *nextpos = this->length() + 1;
    return -1;
}

/**
 * @brief  右側からの文字列(const char *)の検索
 *
 *  自身が持つ文字列の右側から左方向に文字列 str を検索し，
 *  最初に出現する位置を返します．
 *
 * @param      pos オブジェクト内の文字列の開始位置
 * @param      str 検出する文字列
 * @param      n 検出する文字数
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::rfind( size_t pos, const char *str, size_t n ) const
{
    return this->rfind(pos,str,n,(size_t *)NULL);
}

/**
 * @brief  右側からの文字列(const char *)の連続的な検索
 *
 *  自身が持つ文字列の右側から左方向に文字列str を検索し，
 *  最初に出現する位置を返します．
 *
 * @param      pos オブジェクト内の文字列の開始位置
 * @param      str 検出する文字列
 * @param      n 検出する文字数
 * @param      nextpos 次回のpos
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::rfind( size_t pos, const char *str, size_t n,
			size_t *nextpos ) const
{
    if ( this->cstr() != NULL && pos <= this->length() && str != NULL ) {
	size_t len, i;
	/* n より小さい場合の対応 */
	for ( i=0 ; i < n && str[i] != '\0' ; i++ );
	len = i;
	for ( i=pos+1 ; 0 < i ; ) {
	    i--;
	    if ( c_strncmp(this->cstr() + i, str, len) == 0 ) {
		if ( nextpos != NULL ) {
		    size_t add_span = ((len == 0) ? 1 : len);
		    if ( add_span <= i ) *nextpos = i - add_span;
		    else *nextpos = this->length() + 1;	/* finish */
		}
		return i;
	    }
	}
    }
    /* not found */
    if ( nextpos != NULL ) *nextpos = this->length() + 1;
    return -1;
}

/**
 * @brief  右側からの文字列(tstring)の検索
 *
 *  自身が持つ文字列の右側から左方向に文字列str を検索し，
 *  最初に出現する位置を返します．
 *
 * @param      str 検出する文字列
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::rfind( const tstring &str ) const
{
    return this->rfind(this->length(), str.cstr());
}

/**
 * @brief  右側からの文字列(tstring)の検索
 *
 *  自身が持つ文字列の右側から左方向に文字列str を検索し，
 *  最初に出現する位置を返します．
 *
 * @param      pos オブジェクト内の文字列の開始位置
 * @param      str 検出する文字列
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::rfind( size_t pos, const tstring &str ) const
{
    return this->rfind(pos, str.cstr());
}

/**
 * @brief  右側からの文字列(tstring)の連続的な検索
 *
 *  自身が持つ文字列の右側から左方向に文字列str を検索し，
 *  最初に出現する位置を返します．
 *
 * @param      pos オブジェクト内の文字列の開始位置
 * @param      str 検出する文字列
 * @param      nextpos 次回のpos
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::rfind( size_t pos, const tstring &str, size_t *nextpos ) const
{
    return this->rfind(pos, str.cstr(), nextpos);
}

/* オマケ */

/**
 * @brief  printf()の記法で指定された文字列を右側から検索
 *
 *  自身が持つ文字列の右側から，指定された文字列を検索し，最初に出現する
 *  位置を返します．この関数では，検索する文字列を printf()関数と同様の
 *  フォーマットと可変引数でセットできます．
 *
 * @param   format 検索文字列のためのフォーマット指定 
 * @param   ... formatに対応した可変長引数の各要素
 * @return  成功した場合は非負数<br>
 *          失敗した場合は負値
 */
ssize_t tstring::rfindf( const char *format, ... ) const
{
    ssize_t return_status;
    va_list ap;
    va_start(ap, format);
    try {
	return_status = this->vrfindf( format, ap );
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","this->vrfindf() failed");
    }
    va_end(ap);
    return return_status;
}
/**
 * @brief  printf()の記法で指定された文字列を右側から検索
 *
 *  自身が持つ文字列の右側から，指定された文字列を検索し，最初に出現する
 *  位置を返します．この関数では，検索する文字列を printf()関数と同様の
 *  フォーマットと可変引数でセットできます．
 *
 * @param   format 検索文字列のためのフォーマット指定 
 * @param   ap formatに対応した可変長引数のリスト
 * @return  成功した場合は非負数<br>
 *          失敗した場合は負値
 */
ssize_t tstring::vrfindf( const char *format, va_list ap ) const
{
    ssize_t return_status = -1;
    heap_mem<char> work_buf;
    if ( format == NULL ) return this->rfind(format);
    if ( c_vasprintf(work_buf.ptr_address(), format, ap) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","c_vasprintf() failed");
    }
    return_status = this->rfind(work_buf.ptr());
    return return_status;
}
/**
 * @brief  printf()の記法で指定された文字列を右側から検索
 *
 *  自身が持つ文字列の pos の位置を開始点とし，指定された文字列を右側から検索
 *  して最初に出現する位置を返します．この関数では，検索する文字列を printf()
 *  関数と同様のフォーマットと可変引数でセットできます．
 *
 * @param   pos 自身の文字列の開始位置
 * @param   format 検索文字列のためのフォーマット指定 
 * @param   ... formatに対応した可変長引数の各要素
 * @return  成功した場合は非負数<br>
 *          失敗した場合は負値
 */
ssize_t tstring::rfindf( size_t pos, const char *format, ... ) const
{
    ssize_t return_status;
    va_list ap;
    va_start(ap, format);
    try {
	return_status = this->vrfindf( pos, format, ap );
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","this->vrfindf() failed");
    }
    va_end(ap);
    return return_status;
}
/**
 * @brief  printf()の記法で指定された文字列を右側から検索
 *
 *  自身が持つ文字列の pos の位置を開始点とし，指定された文字列を右側から検索
 *  して最初に出現する位置を返します．この関数では，検索する文字列を printf()
 *  関数と同様のフォーマットと可変引数でセットできます．
 *
 * @param   pos 自身の文字列の開始位置
 * @param   format 検索文字列のためのフォーマット指定 
 * @param   ap formatに対応した可変長引数のリスト
 * @return  成功した場合は非負数<br>
 *          失敗した場合は負値
 */
ssize_t tstring::vrfindf( size_t pos, const char *format, va_list ap ) const
{
    ssize_t return_status = -1;
    heap_mem<char> work_buf;
    if ( format == NULL ) return this->rfind(pos,format);
    if ( c_vasprintf(work_buf.ptr_address(), format, ap) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","c_vasprintf() failed");
    }
    return_status = this->rfind(pos, work_buf.ptr());
    return return_status;
}

/**
 * @brief  左側からの文字の検索
 *
 *  自身が持つ文字列の左側から右方向に文字 ch を検索し，
 *  最初に出現する位置を返します．
 *
 * @param      ch 検出する文字
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::strchr( int ch ) const
{
    return this->find((size_t)0,ch);
}

/**
 * @brief  左側からの文字の検索
 *
 *  自身が持つ文字列の左側から右方向に文字 ch を検索し，
 *  最初に出現する位置を返します．
 *
 * @param      pos 自身の文字列の開始位置
 * @param      ch 検出する文字
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::strchr( size_t pos, int ch ) const
{
    return this->find(pos,ch);
}

/**
 * @brief  左側からの文字の連続的な検索
 *
 *  自身が持つ文字列の左側から右方向に文字 ch を検索し，
 *  最初に出現する位置を返します．
 *
 * @param      pos 自身の文字列の開始位置
 * @param      ch 検出する文字
 * @param      nextpos 次回のpos
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::strchr( size_t pos, int ch, size_t *nextpos ) const
{
    return this->find(pos,ch,nextpos);
}

/**
 * @brief  左側からの文字の検索
 *
 *  自身が持つ文字列の左側から右方向に文字 ch を検索し，
 *  最初に出現する位置を返します．
 *
 * @param      ch 検出する文字
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::find( int ch ) const
{
    return this->find((size_t)0,ch);
}

/**
 * @brief  左側からの文字の検索
 *
 *  自身が持つ文字列の左側から右方向に文字 ch を検索し，
 *  最初に出現する位置を返します．
 *
 * @param      pos 自身の文字列の開始位置
 * @param      ch 検出する文字
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::find( size_t pos, int ch ) const
{
    return this->find(pos,ch,(size_t *)NULL);
}

/**
 * @brief  左側からの文字の連続的な検索
 *
 *  自身が持つ文字列の左側から右方向に文字 ch を検索し，
 *  最初に出現する位置を返します．
 *
 * @param      pos 自身の文字列の開始位置
 * @param      ch 検出する文字
 * @param      nextpos 次回のpos
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::find( size_t pos, int ch, size_t *nextpos ) const
{
    if ( 0 < this->length() && pos < this->length() /* && ch != '\0' */ ) {
	size_t i;
	for ( i=pos ; i < this->length() ; i++ ) {
	    if ( ((unsigned char *)this->_str_rec)[i] == ch ) {
		if ( nextpos != NULL ) {
		    *nextpos = i + 1;
		}
		return i;
	    }
	}
    }
    /* not found */
    if ( nextpos != NULL ) *nextpos = this->length();
    return -1;
}

/**
 * @brief  右側からの文字の検索
 *
 *  自身が持つ文字列の右側から左方向に文字 ch を検索し，
 *  最初に出現する位置を返します．
 *
 * @param      ch 検出する文字
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::strrchr( int ch ) const
{
    if ( this->length() < 1 ) return -1;
    return this->rfind((size_t)(this->length() - 1),ch);
}

/**
 * @brief  右側からの文字の検索
 *
 *  自身が持つ文字列の右側から左方向に文字 ch を検索し，
 *  最初に出現する位置を返します．
 *
 * @param      pos 自身の文字列の開始位置
 * @param      ch 検出する文字
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::strrchr( size_t pos, int ch ) const
{
    return this->rfind(pos,ch);
}

/**
 * @brief  右側からの文字の連続的な検索
 *
 *  自身が持つ文字列の右側から左方向に文字 ch を検索し，
 *  最初に出現する位置を返します．
 *
 * @param      pos 自身の文字列の開始位置
 * @param      ch 検出する文字
 * @param      nextpos 次回のpos
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::strrchr( size_t pos, int ch, size_t *nextpos ) const
{
    return this->rfind(pos,ch,nextpos);
}

/**
 * @brief  右側からの文字の検索
 *
 *  自身が持つ文字列の右側から左方向に文字 ch を検索し，
 *  最初に出現する位置を返します．
 *
 * @param      ch 検出する文字
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::rfind( int ch ) const
{
    if ( this->length() < 1 ) return -1;
    return this->rfind((size_t)(this->length() - 1),ch);
}

/**
 * @brief  右側からの文字の検索
 *
 *  自身が持つ文字列の右側から左方向に文字 ch を検索し，
 *  最初に出現する位置を返します．
 *
 * @param      pos 自身の文字列の開始位置
 * @param      ch 検出する文字
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::rfind( size_t pos, int ch ) const
{
    return this->rfind(pos,ch,(size_t *)NULL);
}

/**
 * @brief  右側からの文字の連続的な検索
 *
 *  自身が持つ文字列の右側から左方向に文字 ch を検索し，
 *  最初に出現する位置を返します．
 *
 * @param      pos 自身の文字列の開始位置
 * @param      ch 検出する文字
 * @param      nextpos 次回のpos
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::rfind( size_t pos, int ch, size_t *nextpos ) const
{
    if ( 0 < this->length() && pos < this->length() /* && ch != '\0' */ ) {
	size_t i;
	for ( i=pos+1 ; 0 < i ; ) {
	    i--;
	    if ( ((unsigned char *)this->_str_rec)[i] == ch ) {
		if ( nextpos != NULL ) {
		    if ( 0 < i ) *nextpos = i - 1;
		    else *nextpos = this->length();
		}
		return i;
	    }
	}
    }
    /* not found */
    if ( nextpos != NULL ) *nextpos = this->length();
    return -1;
}

/**
 * @brief  文字セットに含まれる文字を左側から検索
 *
 *  自身が持つ文字列の左側から右方向に，文字セット str に含まれる
 *  文字を検索し，最初に出現する位置を返します．
 *
 * @param      str 検出対象の文字セット
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::find_first_of( const char *str ) const
{
    if ( str != NULL ) return this->find_first_of((size_t)0,str,c_strlen(str));
    else return -1;
}

/**
 * @brief  文字セットに含まれる文字を左側から検索
 *
 *  自身が持つ文字列の左側から右方向に，文字セット str に含まれる
 *  文字を検索し，最初に出現する位置を返します．
 *
 * @param      str 検出対象の文字セット
 * @param      n 文字セットstr の文字数
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::find_first_of( const char *str, size_t n ) const
{
    if ( str != NULL ) return this->find_first_of((size_t)0,str,n);
    else return -1;
}

/**
 * @brief  文字セットに含まれる文字を左側から検索
 *
 *  自身が持つ文字列の左側から右方向に，文字セット str に含まれる
 *  文字を検索し，最初に出現する位置を返します．
 *
 * @param      pos 検出の開始位置
 * @param      str 検出対象の文字セット
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::find_first_of( size_t pos, const char *str ) const
{
    if ( str != NULL ) return this->find_first_of(pos,str,c_strlen(str));
    else return -1;
}

/**
 * @brief  文字セットに含まれる文字を左側から連続的に検索
 *
 *  自身が持つ文字列の左側から右方向に，文字セット str に含まれる
 *  文字を検索し，最初に出現する位置を返します．
 *
 * @param      pos 検出の開始位置
 * @param      str 検出対象の文字セット
 * @param      nextpos 次回のpos
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::find_first_of( size_t pos, const char *str,
				size_t *nextpos ) const
{
    if ( str != NULL ) return this->find_first_of(pos,str,c_strlen(str),
						  nextpos);
    else return -1;
}

/**
 * @brief  文字セットに含まれる文字を左側から検索
 *
 *  自身が持つ文字列の左側から右方向に，文字セット str に含まれる
 *  文字を検索し，最初に出現する位置を返します．
 *
 * @param      pos 検出の開始位置
 * @param      str 検出対象の文字セット
 * @param      n 文字セットstr の文字数
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::find_first_of( size_t pos, const char *str, size_t n ) const
{
    return this->find_first_of(pos,str,n,(size_t *)NULL);
}

/**
 * @brief  文字セットに含まれる文字を左側から連続的に検索
 *
 *  自身が持つ文字列の左側から右方向に，文字セット str に含まれる
 *  文字を検索し，最初に出現する位置を返します．
 *
 * @param      pos 検出の開始位置
 * @param      str 検出対象の文字セット
 * @param      n 文字セットstr の文字数
 * @param      nextpos 次回のpos
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::find_first_of( size_t pos, const char *str, size_t n,
				size_t *nextpos ) const
{
    if ( 0 < this->length() && pos < this->length() && 
	 str != NULL && 0 < n ) {
	size_t len, i, j;
	/* n より小さい場合の対応 */
	for ( i=0 ; i < n && str[i] != '\0' ; i++ );
	len = i;
	for ( i=pos ; i < this->length() ; i++ ) {
	    for ( j=0 ; j < len ; j++ ) {
		if ( this->_str_rec[i] == str[j] ) {
		    if ( nextpos != NULL ) {
			*nextpos = i + 1;
		    }
		    return (ssize_t)i;
		}
	    }
	}
    }
    /* not found */
    if ( nextpos != NULL ) *nextpos = this->length();
    return -1;
}

/**
 * @brief  文字セットに含まれる文字を左側から検索
 *
 *  自身が持つ文字列の左側から右方向に，文字セット str に含まれる
 *  文字を検索し，最初に出現する位置を返します．
 *
 * @param      str 検出対象の文字セット
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::find_first_of( const tstring &str ) const
{
    if ( str.cstr() != NULL ) 
	return this->find_first_of((size_t)0,str.cstr(),str.length());
    else return -1;
}

/**
 * @brief  文字セットに含まれる文字を左側から検索
 *
 *  自身が持つ文字列の左側から右方向に，文字セット str に含まれる
 *  文字を検索し，最初に出現する位置を返します．
 *
 * @param      pos 検出の開始位置
 * @param      str 検出対象の文字セット
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::find_first_of( size_t pos, const tstring &str ) const
{
    if ( str.cstr() != NULL ) 
	return this->find_first_of(pos,str.cstr(),str.length());
    else return -1;
}

/**
 * @brief  文字セットに含まれる文字を左側から連続的に検索
 *
 *  自身が持つ文字列の左側から右方向に，文字セット str に含まれる
 *  文字を検索し，最初に出現する位置を返します．
 *
 * @param      pos 検出の開始位置
 * @param      str 検出対象の文字セット
 * @param      nextpos 次回のpos
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::find_first_of( size_t pos, const tstring &str,
				size_t *nextpos ) const
{
    if ( str.cstr() != NULL ) 
	return this->find_first_of(pos,str.cstr(),str.length(),nextpos);
    else return -1;
}

/**
 * @brief  文字セットに含まれる文字を右側から検索
 *
 *  自身が持つ文字列の右側から左方向に，文字セット str に
 *  含まれる文字を検索し，最初に出現する位置を返します．
 *
 * @param      str 検出対象の文字セット
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::find_last_of( const char *str ) const
{
    if ( 0 < this->length() && str != NULL ) 
        return this->find_last_of(this->length() - 1,str,c_strlen(str));
    else return -1;
}

/**
 * @brief  文字セットに含まれる文字を右側から検索
 *
 *  自身が持つ文字列の右側から左方向に，文字セット str に
 *  含まれる文字を検索し，最初に出現する位置を返します．
 *
 * @param      str 検出対象の文字セット
 * @param      n 文字セットstr の文字数
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::find_last_of( const char *str, size_t n ) const
{
    if ( 0 < this->length() && str != NULL ) 
        return this->find_last_of(this->length() - 1,str,n);
    else return -1;
}

/**
 * @brief  文字セットに含まれる文字を右側から検索
 *
 *  自身が持つ文字列の右側から左方向に，文字セット str に
 *  含まれる文字を検索し，最初に出現する位置を返します．
 *
 * @param      pos 検出の開始位置
 * @param      str 検出対象の文字セット
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::find_last_of( size_t pos, const char *str ) const
{
    if ( str != NULL ) return this->find_last_of(pos,str,c_strlen(str));
    else return -1;
}

/**
 * @brief  文字セットに含まれる文字を右側から連続的に検索
 *
 *  自身が持つ文字列の右側から左方向に，文字セット str に
 *  含まれる文字を検索し，最初に出現する位置を返します．
 *
 * @param      pos 検出の開始位置
 * @param      str 検出対象の文字セット
 * @param      nextpos 次回のpos
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::find_last_of( size_t pos, const char *str,
			       size_t *nextpos ) const
{
    if ( str != NULL ) return this->find_last_of(pos,str,c_strlen(str),
						 nextpos);
    else return -1;
}

/**
 * @brief  文字セットに含まれる文字を右側から検索
 *
 *  自身が持つ文字列の右側から左方向に，文字セット str に
 *  含まれる文字を検索し，最初に出現する位置を返します．
 *
 * @param      pos 検出の開始位置
 * @param      str 検出対象の文字セット
 * @param      n 文字セットstr の文字数
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::find_last_of( size_t pos, const char *str, size_t n ) const
{
    return this->find_last_of(pos,str,n,(size_t *)NULL);
}

/**
 * @brief  文字セットに含まれる文字を右側から連続的に検索
 *
 *  自身が持つ文字列の右側から左方向に，文字セット str に
 *  含まれる文字を検索し，最初に出現する位置を返します．
 *
 * @param      pos 検出の開始位置
 * @param      str 検出対象の文字セット
 * @param      n 文字セットstr の文字数
 * @param      nextpos 次回のpos
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::find_last_of( size_t pos, const char *str, size_t n,
			       size_t *nextpos ) const
{
    if ( 0 < this->length() && pos < this->length() && 
	 str != NULL && 0 < n ) {
	size_t len, i, j;
	/* n より小さい場合の対応 */
	for ( i=0 ; i < n && str[i] != '\0' ; i++ );
	len = i;
	for ( i=pos+1 ; 0 < i ; ) {
	    i--;
	    for ( j=0 ; j < len ; j++ ) {
		if ( this->_str_rec[i] == str[j] ) {
		    if ( nextpos != NULL ) {
			if ( 0 < i ) *nextpos = i - 1;
			else *nextpos = this->length();
		    }
		    return (ssize_t)i;
		}
	    }
	}
    }
    /* not found */
    if ( nextpos != NULL ) *nextpos = this->length();
    return -1;
}

/**
 * @brief  文字セットに含まれる文字を右側から検索
 *
 *  自身が持つ文字列の右側から左方向に，文字セット str に
 *  含まれる文字を検索し，最初に出現する位置を返します．
 *
 * @param      str 検出対象の文字セット
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::find_last_of( const tstring &str ) const
{
    if ( 0 < this->length() && str.cstr() != NULL ) 
        return this->find_last_of(this->length() - 1,str.cstr(),str.length());
    else return -1;
}

/**
 * @brief  文字セットに含まれる文字を右側から検索
 *
 *  自身が持つ文字列の右側から左方向に，文字セット str に
 *  含まれる文字を検索し，最初に出現する位置を返します．
 *
 * @param      pos 検出の開始位置
 * @param      str 検出対象の文字セット
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::find_last_of( size_t pos, const tstring &str ) const
{
    if ( str.cstr() != NULL )
	return this->find_last_of(pos,str.cstr(),str.length());
    else return -1;
}

/**
 * @brief  文字セットに含まれる文字を右側から連続的に検索
 *
 *  自身が持つ文字列の右側から左方向に，文字セット str に
 *  含まれる文字を検索し，最初に出現する位置を返します．
 *
 * @param      pos 検出の開始位置
 * @param      str 検出対象の文字セット
 * @param      nextpos 次回のpos
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::find_last_of( size_t pos, const tstring &str,
			       size_t *nextpos ) const
{
    if ( str.cstr() != NULL )
	return this->find_last_of(pos,str.cstr(),str.length(),nextpos);
    else return -1;
}

/**
 * @brief  左側からの文字の検索
 *
 *  自身が持つ文字列の左側から右方向に文字 ch を検索し，
 *  最初に出現する位置を返します．
 *
 * @param      ch 検出する文字
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::find_first_of( int ch ) const	/* find() と同じ */
{
    return tstring::find(ch);
}

/**
 * @brief  右側からの文字の検索
 *
 *  自身が持つ文字列の右側から左方向に文字 ch を検索し，
 *  最初に出現する位置を返します．
 *
 * @param      ch 検出する文字
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::find_last_of( int ch ) const	/* rfind() と同じ */
{
    return tstring::rfind(ch);
}

/**
 * @brief  左側からの文字の検索
 *
 *  自身が持つ文字列の左側から右方向に文字 ch を検索し，
 *  最初に出現する位置を返します．
 *
 * @param      pos 自身の文字列の開始位置
 * @param      ch 検出する文字
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::find_first_of( size_t pos, int ch ) const /* find() と同じ */
{
    return tstring::find(pos,ch);
}

/**
 * @brief  右側からの文字の検索
 *
 *  自身が持つ文字列の右側から左方向に文字 ch を検索し，
 *  最初に出現する位置を返します．
 *
 * @param      ch 検出する文字
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::find_last_of( size_t pos, int ch ) const /* rfind() と同じ */
{
    return tstring::rfind(pos,ch);
}

/**
 * @brief  左側からの文字の連続的な検索
 *
 *  自身が持つ文字列の左側から右方向に文字 ch を検索し，
 *  最初に出現する位置を返します．
 *
 * @param      pos 自身の文字列の開始位置
 * @param      ch 検出する文字
 * @param      nextpos 次回のpos
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::find_first_of( size_t pos, int ch, size_t *nextpos ) const /* find() と同じ */
{
    return tstring::find(pos,ch,nextpos);
}

/**
 * @brief  右側からの文字の連続的な検索
 *
 *  自身が持つ文字列の右側から左方向に文字 ch を検索し，
 *  最初に出現する位置を返します．
 *
 * @param      pos 自身の文字列の開始位置
 * @param      ch 検出する文字
 * @param      nextpos 次回のpos
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::find_last_of( size_t pos, int ch, size_t *nextpos ) const /* rfind() と同じ */
{
    return tstring::rfind(pos,ch,nextpos);
}

/**
 * @brief  文字セットに含まれない文字を左側から検索
 *
 *  自身が持つ文字列の左側から右方向に，文字セット str に
 *  含まれない文字を検索し，最初に出現する位置を返します．
 *
 * @param      str 文字セット
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::find_first_not_of( const char *str ) const
{
    if ( str != NULL ) return this->find_first_not_of((size_t)0,str,c_strlen(str));
    else return -1;
}

/**
 * @brief  文字セットに含まれない文字を左側から検索
 *
 *  自身が持つ文字列の左側から右方向に，文字セットstr に<br>
 *  含まれない文字を検索し，最初に出現する位置を返します．
 *
 * @param      str 文字セット
 * @param      n 文字セットstr の文字数
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::find_first_not_of( const char *str, size_t n ) const
{
    if ( str != NULL ) return this->find_first_not_of((size_t)0,str,n);
    else return -1;
}

/**
 * @brief  文字セットに含まれない文字を左側から検索
 *
 *  自身が持つ文字列の左側から右方向に，文字セット str に
 *  含まれない文字を検索し，最初に出現する位置を返します．
 *
 * @param      pos 検出の開始位置
 * @param      str 文字セット
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::find_first_not_of( size_t pos, const char *str ) const
{
    if ( str != NULL ) return this->find_first_not_of(pos,str,c_strlen(str));
    else return -1;
}

/**
 * @brief  文字セットに含まれない文字を左側から連続的に検索
 *
 *  自身が持つ文字列の左側から右方向に，文字セット str に
 *  含まれない文字を検索し，最初に出現する位置を返します．
 *
 * @param      pos 検出の開始位置
 * @param      str 文字セット
 * @param      nextpos 次回のpos
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::find_first_not_of( size_t pos, const char *str,
				    size_t *nextpos ) const
{
    if ( str != NULL ) return this->find_first_not_of(pos,str,c_strlen(str),
						      nextpos);
    else return -1;
}

/**
 * @brief  文字セットに含まれない文字を左側から検索
 *
 *  自身が持つ文字列の左側から右方向に，文字セット str に
 *  含まれない文字を検索し，最初に出現する位置を返します．
 *
 * @param      pos 検出の開始位置
 * @param      str 文字セット
 * @param      n 文字セットstr の文字数
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::find_first_not_of( size_t pos, 
				    const char *str, size_t n ) const
{
    return this->find_first_not_of(pos,str,n,(size_t *)NULL);
}

/**
 * @brief  文字セットに含まれない文字を左側から連続的に検索
 *
 *  自身が持つ文字列の左側から右方向に，文字セット str に
 *  含まれない文字を検索し，最初に出現する位置を返します．
 *
 * @param      pos 検出の開始位置
 * @param      str 文字セット
 * @param      n 文字セットstr の文字数
 * @param      nextpos 次回のpos
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::find_first_not_of( size_t pos, const char *str, size_t n,
				    size_t *nextpos ) const
{
    if ( 0 < this->length() && pos < this->length() && str != NULL ) {
	size_t len, i, j;
	/* n より小さい場合の対応 */
	for ( i=0 ; i < n && str[i] != '\0' ; i++ );
	len = i;
	for ( i=pos ; i < this->length() ; i++ ) {
	    bool flg = false;
	    for ( j=0 ; j < len ; j++ ) {
	        if ( this->_str_rec[i] == str[j] ) {
		    flg = true;
		    break;
		}
	    }
	    if ( flg == false ) {
		if ( nextpos != NULL ) {
		    *nextpos = i + 1;
		}
		return (ssize_t)i;
	    }
	}
    }
    /* not found */
    if ( nextpos != NULL ) *nextpos = this->length();
    return -1;
}

/**
 * @brief  文字セットに含まれない文字を左側から検索
 *
 *  自身が持つ文字列の左側から右方向に，文字セット str に
 *  含まれない文字を検索し，最初に出現する位置を返します．
 *
 * @param      str 文字セット
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::find_first_not_of( const tstring &str ) const
{
    if ( str.cstr() != NULL )
	return this->find_first_not_of((size_t)0,str.cstr(),str.length());
    else return -1;
}

/**
 * @brief  文字セットに含まれない文字を左側から検索
 *
 *  自身が持つ文字列の左側から右方向に，文字セット str に
 *  含まれない文字を検索し，最初に出現する位置を返します．
 *
 * @param      pos 検出の開始位置
 * @param      str 文字セット
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::find_first_not_of( size_t pos, const tstring &str ) const
{
    if ( str.cstr() != NULL )
	return this->find_first_not_of(pos,str.cstr(),str.length());
    else return -1;
}

/**
 * @brief  文字セットに含まれない文字を左側から連続的に検索
 *
 *  自身が持つ文字列の左側から右方向に，文字セット str に
 *  含まれない文字を検索し，最初に出現する位置を返します．
 *
 * @param      pos 検出の開始位置
 * @param      str 文字セット
 * @param      nextpos 次回のpos
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::find_first_not_of( size_t pos, const tstring &str,
				    size_t *nextpos ) const
{
    if ( str.cstr() != NULL )
	return this->find_first_not_of(pos,str.cstr(),str.length(),nextpos);
    else return -1;
}

/**
 * @brief  文字セットに含まれない文字を右側から検索
 *
 *  自身が持つ文字列の右側から左方向に，文字セット str に
 *  含まれない文字を検索し，最初に出現する位置を返します．
 *
 * @param      str 文字列に含まれていない文字セット
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::find_last_not_of( const char *str ) const
{
    if ( 0 < this->length() && str != NULL ) 
        return this->find_last_not_of(this->length() - 1,str,c_strlen(str));
    else return -1;
}

/**
 * @brief  文字セットに含まれない文字を右側から検索
 *
 *  自身が持つ文字列の右側から左方向に，文字セット str に
 *  含まれない文字を検索し，最初に出現する位置を返します．
 *
 * @param      str 文字列に含まれていない文字セット
 * @param      n 文字セットstr の文字数
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::find_last_not_of( const char *str, size_t n ) const
{
    if ( 0 < this->length() && str != NULL ) 
        return this->find_last_not_of(this->length() - 1,str,n);
    else return -1;
}

/**
 * @brief  文字セットに含まれない文字を右側から検索
 *
 *  自身が持つ文字列の右側から左方向に，文字セット str に
 *  含まれない文字を検索し，最初に出現する位置を返します．
 *
 * @param      pos 検出の開始位置
 * @param      str 文字列に含まれていない文字セット
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::find_last_not_of( size_t pos, const char *str ) const
{
    if ( str != NULL ) return this->find_last_not_of(pos,str,c_strlen(str));
    else return -1;
}

/**
 * @brief  文字セットに含まれない文字を右側から連続的に検索
 *
 *  自身が持つ文字列の右側から左方向に，文字セット str に
 *  含まれない文字を検索し，最初に出現する位置を返します．
 *
 * @param      pos 検出の開始位置
 * @param      str 文字列に含まれていない文字セット
 * @param      nextpos 次回のpos
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::find_last_not_of( size_t pos, const char *str,
				   size_t *nextpos ) const
{
    if (str != NULL) return this->find_last_not_of(pos,str,c_strlen(str),
						   nextpos);
    else return -1;
}

/**
 * @brief  文字セットに含まれない文字を右側から検索
 *
 *  自身が持つ文字列の右側から左方向に，文字セット str に
 *  含まれない文字を検索し，最初に出現する位置を返します．
 *
 * @param      pos 検出の開始位置
 * @param      str 文字列に含まれていない文字セット
 * @param      n 文字セットstr の文字数
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::find_last_not_of( size_t pos, 
				   const char *str, size_t n ) const
{
    return this->find_last_not_of(pos,str,n,(size_t *)NULL);
}

/**
 * @brief  文字セットに含まれない文字を右側から連続的に検索
 *
 *  自身が持つ文字列の右側から左方向に，文字セット str に
 *  含まれない文字を検索し，最初に出現する位置を返します．
 *
 * @param      pos 検出の開始位置
 * @param      str 文字列に含まれていない文字セット
 * @param      n 文字セットstr の文字数
 * @param      nextpos 次回のpos
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::find_last_not_of( size_t pos, const char *str, size_t n,
				   size_t *nextpos ) const
{
    if ( 0 < this->length() && pos < this->length() && str != NULL ) {
	size_t len, i, j;
	/* n より小さい場合の対応 */
	for ( i=0 ; i < n && str[i] != '\0' ; i++ );
	len = i;
	for ( i=pos+1 ; 0 < i ; ) {
	    bool flg = false;
	    i--;
	    for ( j=0 ; j < len ; j++ ) {
	        if ( this->_str_rec[i] == str[j] ) {
		    flg = true;
		    break;
		}
	    }
	    if ( flg == false ) {
		if ( nextpos != NULL ) {
		    if ( 0 < i ) *nextpos = i - 1;
		    else *nextpos = this->length();
		}
		return (ssize_t)i;
	    }
	}
    }
    /* not found */
    if ( nextpos != NULL ) *nextpos = this->length();
    return -1;
}

/**
 * @brief  文字セットに含まれない文字を右側から検索
 *
 *  自身が持つ文字列の右側から左方向に，文字セット str に
 *  含まれない文字を検索し，最初に出現する位置を返します．
 *
 * @param      str 文字列に含まれていない文字セット
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::find_last_not_of( const tstring &str ) const
{
    if ( 0 < this->length() && str.cstr() != NULL ) 
        return this->find_last_not_of(this->length() - 1,
				      str.cstr(), str.length());
    else return -1;
}

/**
 * @brief  文字セットに含まれない文字を右側から検索
 *
 *  自身が持つ文字列の右側から左方向に，文字セット str に
 *  含まれない文字を検索し，最初に出現する位置を返します．
 *
 * @param      pos 検出の開始位置
 * @param      str 文字列に含まれていない文字セット
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::find_last_not_of( size_t pos, const tstring &str ) const
{
    if ( str.cstr() != NULL )
	return this->find_last_not_of(pos, str.cstr(), str.length());
    else return -1;
}

/**
 * @brief  文字セットに含まれない文字を右側から連続的に検索
 *
 *  自身が持つ文字列の右側から左方向に，文字セット str に
 *  含まれない文字を検索し，最初に出現する位置を返します．
 *
 * @param      pos 検出の開始位置
 * @param      str 文字列に含まれていない文字セット
 * @param      nextpos 次回のpos
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::find_last_not_of( size_t pos, const tstring &str,
				   size_t *nextpos ) const
{
    if ( str.cstr() != NULL )
	return this->find_last_not_of(pos, str.cstr(), str.length(), nextpos);
    else return -1;
}

/**
 * @brief  指定された文字ではない文字を左側から検索
 *
 *  自身が持つ文字列の左側から右方向に，文字 ch ではない
 *  文字を検索し，最初に出現する位置を返します．
 *
 * @param      ch 検出を除外する文字
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::find_first_not_of( int ch ) const
{
    return this->find_first_not_of((size_t)0,ch);
}

/**
 * @brief  指定された文字ではない文字を左側から検索
 *
 *  自身が持つ文字列の左側から右方向に，文字 ch ではない
 *  文字を検索し，最初に出現する位置を返します．
 *
 * @param      pos 検出の開始位置
 * @param      ch 検出を除外する文字
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::find_first_not_of( size_t pos, int ch ) const
{
    return this->find_first_not_of(pos,ch,(size_t *)NULL);
}

/**
 * @brief  指定された文字ではない文字を左側から連続的に検索
 *
 *  自身が持つ文字列の左側から右方向に，文字 ch ではない
 *  文字を検索し，最初に出現する位置を返します．
 *
 * @param      pos 検出の開始位置
 * @param      ch 検出を除外する文字
 * @param      nextpos 次回のpos
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::find_first_not_of( size_t pos, int ch, size_t *nextpos ) const
{
    if ( 0 < this->length() && pos < this->length() ) {
	size_t i;
	for ( i=pos ; i < this->length() ; i++ ) {
	    if ( ((unsigned char *)this->_str_rec)[i] != ch ) {
		if ( nextpos != NULL ) {
		    *nextpos = i + 1;
		}
	        return (ssize_t)i;
	    }
	}
    }
    /* not found */
    if ( nextpos != NULL ) *nextpos = this->length();
    return -1;
}

/**
 * @brief  指定された文字ではない文字を右側から検索
 *
 *  自身が持つ文字列の右側から左方向に，文字 ch ではない
 *  文字を検索し，最初に出現する位置を返します．
 *
 * @param      ch 検出を除外する文字
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::find_last_not_of( int ch ) const
{
    if ( 0 < this->length() ) 
        return this->find_last_not_of(this->length() - 1,ch);
    else return -1;
}

/**
 * @brief  指定された文字ではない文字を右側から検索
 *
 *  自身が持つ文字列の右側から左方向に，文字 ch ではない
 *  文字を検索し，最初に出現する位置を返します．
 *
 * @param      pos 検出の開始位置
 * @param      ch 検出を除外する文字
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::find_last_not_of( size_t pos, int ch ) const
{
    return this->find_last_not_of(pos,ch,(size_t *)NULL);
}

/**
 * @brief  指定された文字ではない文字を右側から連続的に検索
 *
 *  自身が持つ文字列の右側から左方向に，文字 ch ではない
 *  文字を検索し，最初に出現する位置を返します．
 *
 * @param      pos 検出の開始位置
 * @param      ch 検出を除外する文字
 * @param      nextpos 次回のpos
 * @return     成功した場合は非負数<br>
 *             失敗した場合は負値
 *
 */
ssize_t tstring::find_last_not_of( size_t pos, int ch, size_t *nextpos ) const
{
    if ( 0 < this->length() && pos < this->length() ) {
	size_t i;
	for ( i=pos+1 ; 0 < i ; ) {
	    i--;
	    if ( ((unsigned char *)this->_str_rec)[i] != ch ) {
		if ( nextpos != NULL ) {
		    if ( 0 < i ) *nextpos = i - 1;
		    else *nextpos = this->length();
		}
		return (ssize_t)i;
	    }
	}
    }
    /* not found */
    if ( nextpos != NULL ) *nextpos = this->length();
    return -1;
}

/*
 * New member functions of 1.3.0
 */

/**
 * @brief  文字列を左側からパースし，クォーテーションまたは括弧の対応関係を取得
 */
ssize_t tstring::find_quoted( const char *quot_bkt, int escape,
			      size_t *quoted_span, size_t *nextpos ) const
{
    return find_quoted_string( this->cstr(), this->length(), 
			       0, quot_bkt, escape, quoted_span, nextpos );
}

/**
 * @brief  文字列を左側からパースし，クォーテーションまたは括弧の対応関係を取得
 */
ssize_t tstring::find_quoted( size_t pos, const char *quot_bkt, int escape,
			      size_t *quoted_span, size_t *nextpos ) const
{
    return find_quoted_string( this->cstr(), this->length(), 
			       pos, quot_bkt, escape, quoted_span, nextpos );
}

/**
 * @brief  文字列を右側からパースし，クォーテーションまたは括弧の対応関係を取得
 */
ssize_t tstring::rfind_quoted( const char *quot_bkt, int escape,
			       size_t *quoted_span, size_t *nextpos ) const
{
    return rfind_quoted_string( this->cstr(), this->length(), 
		      this->length(), quot_bkt, escape, quoted_span, nextpos );
}

/**
 * @brief  文字列を右側からパースし，クォーテーションまたは括弧の対応関係を取得
 */
ssize_t tstring::rfind_quoted( size_t pos, const char *quot_bkt, int escape,
			       size_t *quoted_span, size_t *nextpos ) const
{
    return rfind_quoted_string( this->cstr(), this->length(), 
				pos, quot_bkt, escape, quoted_span, nextpos );
}


/**
 * @brief  英字または数字であるかを調べる
 *
 *  自身が持つ文字列の位置pos にある文字を，現在のロケールにより分類し，
 *  その結果を返します．
 *
 * @param      pos 分類する文字の位置
 * @return     posにある文字が，そのメンバ関数が対応する文字に
 *             一致する場合はtrue<br>
 *             不一致である場合はfalse
 *
 */
bool tstring::isalnum( size_t pos ) const
{
    if ( pos <= this->length() ) 
	return c_isalnum( ((unsigned char *)this->_str_rec)[pos] );
    else
	return false;
}

/**
 * @brief  アルファベットかどうか調べる
 *
 *  自身が持つ文字列の位置pos にある文字を，現在のロケールにより分類し，
 *  その結果を返します．
 *
 * @param      pos 分類する文字の位置
 * @return     posにある文字が，そのメンバ関数が対応する文字に
 *             一致する場合はtrue<br>
 *             不一致である場合はfalse
 *
 */
bool tstring::isalpha( size_t pos ) const
{
    if ( pos <= this->length() ) 
	return c_isalpha( ((unsigned char *)this->_str_rec)[pos] );
    else
	return false;
}

/**
 * @brief  制御文字かどうかを調べる
 *
 *  自身が持つ文字列の位置pos にある文字を，現在のロケールにより分類し，
 *  その結果を返します．
 *
 * @param      pos 分類する文字の位置
 * @return     posにある文字が，そのメンバ関数が対応する文字に
 *             一致する場合はtrue<br>
 *             不一致である場合はfalse
 *
 */
bool tstring::iscntrl( size_t pos ) const
{
    if ( pos <= this->length() ) 
	return c_iscntrl( ((unsigned char *)this->_str_rec)[pos] );
    else
	return false;
}

/**
 * @brief  数字 (0から9まで) かどうかを調べる
 *
 *  自身が持つ文字列の位置pos にある文字を，現在のロケールにより分類し，
 *  その結果を返します．
 *
 * @param      pos 分類する文字の位置
 * @return     posにある文字が，そのメンバ関数が対応する文字に
 *             一致する場合はtrue<br>
 *             不一致である場合はfalse
 *
 */
bool tstring::isdigit( size_t pos ) const
{
    if ( pos <= this->length() ) 
	return c_isdigit( ((unsigned char *)this->_str_rec)[pos] );
    else
	return false;
}

/**
 * @brief  表示可能な文字かどうかを調べる
 *
 *  自身が持つ文字列の位置pos にある文字を，現在のロケールにより分類し，
 *  その結果を返します．
 *
 * @param      pos 分類する文字の位置
 * @return     posにある文字が，そのメンバ関数が対応する文字に
 *             一致する場合はtrue<br>
 *             不一致である場合はfalse
 *
 */
bool tstring::isgraph( size_t pos ) const
{
    if ( pos <= this->length() ) 
	return c_isgraph( ((unsigned char *)this->_str_rec)[pos] );
    else
	return false;
}

/**
 * @brief  小文字かどうかを調べる
 *
 *  自身が持つ文字列の位置pos にある文字を，現在のロケールにより分類し，
 *  その結果を返します．
 *
 * @param      pos 分類する文字の位置
 * @return     posにある文字が，そのメンバ関数が対応する文字に
 *             一致する場合はtrue<br>
 *             不一致である場合はfalse
 *
 */
bool tstring::islower( size_t pos ) const
{
    if ( pos <= this->length() ) 
	return c_islower( ((unsigned char *)this->_str_rec)[pos] );
    else
	return false;
}

/**
 * @brief  表示可能な文字かどうかを調べる(空白を除く)
 *
 *  自身が持つ文字列の位置pos にある文字を，現在のロケールにより分類し，
 *  その結果を返します．
 *
 * @param      pos 分類する文字の位置
 * @return     posにある文字が，そのメンバ関数が対応する文字に
 *             一致する場合はtrue<br>
 *             不一致である場合はfalse
 *
 */
bool tstring::isprint( size_t pos ) const
{
    if ( pos <= this->length() ) 
	return c_isprint( ((unsigned char *)this->_str_rec)[pos] );
    else
	return false;
}

/**
 * @brief  表示可能な文字かどうかを調べる(空白と英数字を除く)
 *
 *  自身が持つ文字列の位置pos にある文字を，現在のロケールにより分類し，
 *  その結果を返します．
 *
 * @param      pos 分類する文字の位置
 * @return     posにある文字が，そのメンバ関数が対応する文字に
 *             一致する場合はtrue<br>
 *             不一致である場合はfalse
 *
 */
bool tstring::ispunct( size_t pos ) const
{
    if ( pos <= this->length() ) 
	return c_ispunct( ((unsigned char *)this->_str_rec)[pos] );
    else
	return false;
}

/**
 * @brief  空白文字かどうかを調べる
 *
 *  自身が持つ文字列の位置pos にある文字を，現在のロケールにより分類し，
 *  その結果を返します．
 *
 * @param      pos 分類する文字の位置
 * @return     posにある文字が，そのメンバ関数が対応する文字に
 *             一致する場合はtrue<br>
 *             不一致である場合はfalse
 *
 */
bool tstring::isspace( size_t pos ) const
{
    if ( pos <= this->length() ) 
	return c_isspace( ((unsigned char *)this->_str_rec)[pos] );
    else
	return false;
}

/**
 * @brief  大文字かどうかを調べる
 *
 *  自身が持つ文字列の位置pos にある文字を，現在のロケールにより分類し，
 *  その結果を返します．
 *
 * @param      pos 分類する文字の位置
 * @return     posにある文字が，そのメンバ関数が対応する文字に
 *             一致する場合はtrue<br>
 *             不一致である場合はfalse
 *
 */
bool tstring::isupper( size_t pos ) const
{
    if ( pos <= this->length() ) 
	return c_isupper( ((unsigned char *)this->_str_rec)[pos] );
    else
	return false;
}

/**
 * @brief  16 進数での数字かどうかを調べる
 *
 *  自身が持つ文字列の位置pos にある文字を，現在のロケールにより分類し，
 *  その結果を返します．
 *
 * @param      pos 分類する文字の位置
 * @return     posにある文字が，そのメンバ関数が対応する文字に
 *             一致する場合はtrue<br>
 *             不一致である場合はfalse
 *
 */
bool tstring::isxdigit( size_t pos ) const
{
    if ( pos <= this->length() ) 
	return c_isxdigit( ((unsigned char *)this->_str_rec)[pos] );
    else
	return false;
}

/**
 * @brief  文字セットに含まれる文字を検索
 *
 *  自身が持つ文字列の左側から右方向に，文字セットstrに含まれる文字を検索し，
 *  最初に出現する位置を返します．<br>
 *  なお，文字列の先頭位置は 0 です．
 *
 * @param   str 検出対象の文字セット
 * @return  成功した場合は非負数<br>
 *          失敗した場合は負値
 * @throw   内部バッファの確保に失敗した場合
 */
ssize_t tstring::strpbrk( const char *str ) const
{
    if ( str != NULL ) return this->strpbrk((size_t)0,str);
    else return -1;
}

/**
 * @brief  文字セットに含まれる文字を検索
 *
 *  自身が持つ文字列の左側から右方向に，文字セットstrに含まれる文字を検索し，
 *  最初に出現する位置を返します．<br>
 *  なお，文字列の先頭位置は 0 です．
 *
 * @param   pos 検出の開始位置
 * @param   str 検出対象の文字セット
 * @return  成功した場合は非負数<br>
 *          失敗した場合は負値
 * @throw   内部バッファの確保に失敗した場合
 */
ssize_t tstring::strpbrk( size_t pos, const char *str ) const
{
    return this->strpbrk(pos,str,(size_t *)NULL);
}

/**
 * @brief  文字セットに含まれる文字を連続的に検索
 *
 *  自身が持つ文字列の左側から右方向に，文字セットstrに含まれる文字を検索し，
 *  最初に出現する位置を返します．<br>
 *  なお，文字列の先頭位置は 0 です．
 *
 * @param   pos 検出の開始位置
 * @param   str 検出対象の文字セット
 * @param   nextpos 次回のpos
 * @return  成功した場合は非負数<br>
 *          失敗した場合は負値
 * @throw   内部バッファの確保に失敗した場合
 */
ssize_t tstring::strpbrk( size_t pos, const char *str, size_t *nextpos ) const
{
    if ( 0 < this->length() && pos < this->length() && str != NULL ) {
	size_t len = c_strlen(str);
	if ( 0 < len && str[0] == '[' && str[len-1] == ']' ) {
	    tstring accepts;
	    bool bl;
	    int flags = 0;
	    size_t p0 = 0;
	    p0 += make_accepts_regexp( str, true, accepts, &flags, &bl );
	    if ( p0 == 0 ) {	/* パターンがおかしい */
		return tstring::find_first_of(pos,str,len,nextpos);
	    }
	    else {
		const char *fptr;
		const char *this_str_ptr = this->str_ptr_cs();
		fptr = find_pattern(this_str_ptr + pos, this->length() - pos,
				    accepts.cstr(), flags, bl);
		if ( fptr != NULL ) {
		    size_t ret = fptr - this_str_ptr;
		    if ( nextpos != NULL ) {
			*nextpos = ret + 1;
		    }
		    return ret;
		}
	    }
	}
	else {
	    return tstring::find_first_of(pos,str,len,nextpos);
	}
    }
    /* not found */
    if ( nextpos != NULL ) *nextpos = this->length();
    return -1;
}

/**
 * @brief  文字セットに含まれる文字を検索
 *
 *  自身が持つ文字列の左側から右方向に，文字セットstrに含まれる文字を検索し，
 *  最初に出現する位置を返します．<br>
 *  なお，文字列の先頭位置は 0 です．
 *
 * @param   str 検出対象の文字セット
 * @return  成功した場合は非負数<br>
 *          失敗した場合は負値
 * @throw   内部バッファの確保に失敗した場合
 */
ssize_t tstring::strpbrk( const tstring &str ) const
{
    if ( str.cstr() != NULL ) return this->strpbrk((size_t)0, str.cstr());
    else return -1;
}

/**
 * @brief  文字セットに含まれる文字を検索
 *
 *  自身が持つ文字列の左側から右方向に，文字セットstrに含まれる文字を検索し，
 *  最初に出現する位置を返します．<br>
 *  なお，文字列の先頭位置は 0 です．
 *
 * @param   pos 検出の開始位置
 * @param   str 検出対象の文字セット
 * @return  成功した場合は非負数<br>
 *          失敗した場合は負値
 * @throw   内部バッファの確保に失敗した場合
 */
ssize_t tstring::strpbrk( size_t pos, const tstring &str ) const
{
    if ( str.cstr() != NULL ) return this->strpbrk(pos, str.cstr());
    else return -1;
}

/**
 * @brief  文字セットに含まれる文字を連続的に検索
 *
 *  自身が持つ文字列の左側から右方向に，文字セットstrに含まれる文字を検索し，
 *  最初に出現する位置を返します．<br>
 *  なお，文字列の先頭位置は 0 です．
 *
 * @param   pos 検出の開始位置
 * @param   str 検出対象の文字セット
 * @param   nextpos 次回のpos
 * @return  成功した場合は非負数<br>
 *          失敗した場合は負値
 * @throw   内部バッファの確保に失敗した場合
 */
ssize_t tstring::strpbrk( size_t pos, const tstring &str,
			  size_t *nextpos ) const
{
    if ( str.cstr() != NULL ) return this->strpbrk(pos, str.cstr(), nextpos);
    else return -1;
}

/**
 * @brief  文字セットに含まれる文字を，右側から検索
 *
 *  自身が持つ文字列の右側から左方向に，文字列セット str に含まれる文字を検索し
 *  最初に出現する位置を返します．
 *  なお，文字列の先頭位置は 0 です．
 *
 * @param    str 検出対象の文字セット
 * @return   成功した場合は非負数<br>
 *           失敗した場合は負値
 * @throw    内部バッファの確保に失敗した場合
 */
ssize_t tstring::strrpbrk( const char *str ) const
{
    if ( 0 < this->length() && str != NULL ) 
        return this->strrpbrk(this->length() - 1, str);
    else return -1;
}

/**
 * @brief  文字セットに含まれる文字を，右側から検索
 *
 *  自身が持つ文字列の右側から左方向に，文字列セット str に含まれる文字を検索し
 *  最初に出現する位置を返します．
 *  なお，文字列の先頭位置は 0 です．
 * 
 * @param    pos 検出の開始位置
 * @param    str 検出対象の文字セット
 * @return   成功した場合は非負数<br>
 *           失敗した場合は負値
 * @throw    内部バッファの確保に失敗した場合
 */
ssize_t tstring::strrpbrk( size_t pos, const char *str ) const
{
    return this->strrpbrk(pos,str,(size_t *)NULL);
}

/**
 * @brief  文字セットに含まれる文字を，右側から連続的に検索
 *
 *  自身が持つ文字列の右側から左方向に，文字列セット str に含まれる文字を検索し
 *  最初に出現する位置を返します．
 *  なお，文字列の先頭位置は 0 です．
 * 
 * @param    pos 検出の開始位置
 * @param    str 検出対象の文字セット
 * @param    nextpos 次回のposが代入される
 * @return   成功した場合は非負数<br>
 *           失敗した場合は負値
 * @throw    内部バッファの確保に失敗した場合
 */
ssize_t tstring::strrpbrk( size_t pos, const char *str, size_t *nextpos ) const
{
    if ( 0 < this->length() && pos < this->length() && str != NULL ) {
	size_t len = c_strlen(str);
	if ( 0 < len && str[0] == '[' && str[len-1] == ']' ) {
	    tstring accepts;
	    bool bl;
	    int flags = 0;
	    size_t p0 = 0;
	    p0 += make_accepts_regexp( str, true, accepts, &flags, &bl );
	    if ( p0 == 0 ) {	/* パターンがおかしい */
		return tstring::find_last_of(pos,str,len,nextpos);
	    }
	    else {
		const char *fptr;
		fptr = rfind_pattern(this->cstr(), pos + 1,
				     accepts.cstr(), flags, bl);
		if ( fptr != NULL ) {
		    size_t ret = fptr - this->cstr();
		    if ( nextpos != NULL ) {
			if ( 0 < ret ) *nextpos = ret - 1;
			else *nextpos = this->length();
		    }
		    return ret;
		}
	    }
	}
	else {
	    return tstring::find_last_of(pos,str,len,nextpos);
	}
    }
    /* not found */
    if ( nextpos != NULL ) *nextpos = this->length();
    return -1;
}

/**
 * @brief  文字セットに含まれる文字を，右側から検索
 *
 *  自身が持つ文字列の右側から左方向に，文字列セット str に含まれる文字を検索し
 *  最初に出現する位置を返します．
 *  なお，文字列の先頭位置は 0 です．
 * 
 * @param    str 検出対象の文字セット
 * @return   成功した場合は非負数<br>
 *           失敗した場合は負値
 * @throw    内部バッファの確保に失敗した場合
 */
ssize_t tstring::strrpbrk( const tstring &str ) const
{

    if ( 0 < this->length() && str.cstr() != NULL ) 
        return this->strrpbrk(this->length() - 1, str.cstr());
    else return -1;
}

/**
 * @brief  文字セットに含まれる文字を，右側から検索
 *
 *  自身が持つ文字列の右側から左方向に，文字列セット str に含まれる文字を検索し
 *  最初に出現する位置を返します．
 *  なお，文字列の先頭位置は 0 です．
 * 
 * @param    pos 検出の開始位置
 * @param    str 検出対象の文字セット
 * @return   成功した場合は非負数<br>
 *           失敗した場合は負値
 * @throw    内部バッファの確保に失敗した場合
 */
ssize_t tstring::strrpbrk( size_t pos, const tstring &str ) const
{
    if ( str.cstr() != NULL ) return this->strrpbrk(pos, str.cstr());
    else return -1;
}

/**
 * @brief  文字セットに含まれる文字を，右側から連続的に検索
 *
 *  自身が持つ文字列の右側から左方向に，文字列セット str に含まれる文字を検索し
 *  最初に出現する位置を返します．
 *  なお，文字列の先頭位置は 0 です．
 * 
 * @param    pos 検出の開始位置
 * @param    str 検出対象の文字セット
 * @param    nextpos 次回のposが代入される
 * @return   成功した場合は非負数<br>
 *           失敗した場合は負値
 * @throw    内部バッファの確保に失敗した場合
 */
ssize_t tstring::strrpbrk( size_t pos, const tstring &str,
			   size_t *nextpos ) const
{
    if ( str.cstr() != NULL ) return this->strrpbrk(pos, str.cstr(), nextpos);
    else return -1;
}

/**
 * @brief  文字セットに含まれる文字が連続する長さを取得
 *
 *  自身が持つ文字列の左側から右方向に，文字セット accept が
 *  連続する長さを検索し，その長さを返します．
 *
 * @param      accept 検出対象の文字セット
 * @return     成功した場合は正値<br>
 *             失敗した場合は0
 * @throw      内部バッファの確保に失敗した場合
 *
 */
size_t tstring::strspn( const char *accept ) const
{
    return this->strspn((size_t)0,accept);
}

/**
 * @brief  文字セットに含まれる文字が連続する長さを取得
 *
 *  自身が持つ文字列の左側から右方向に，文字セット accept が
 *  連続する長さを検索し，その長さを返します．
 *
 * @param      pos 検出の開始位置
 * @param      accept 検出対象の文字セット
 * @return     成功した場合は正値<br>
 *             失敗した場合は0
 * @throw      内部バッファの確保に失敗した場合
 *
 */
size_t tstring::strspn( size_t pos, const char *accept ) const
{
    return this->strspn(pos,accept,(size_t *)NULL);
}

/**
 * @brief  文字セットに含まれる文字が連続する長さを連続的に取得
 *
 *  自身が持つ文字列の左側から右方向に，文字セット accept が
 *  連続する長さを検索し，その長さを返します．
 *
 * @param      pos 検出の開始位置
 * @param      accept 検出対象の文字セット
 * @param      nextpos 次回のpos
 * @return     成功した場合は正値<br>
 *             失敗した場合は0
 * @throw      内部バッファの確保に失敗した場合
 *
 */
size_t tstring::strspn( size_t pos, const char *accept, size_t *nextpos ) const
{
    size_t ret = 0;
    if ( 0 < this->length() && pos < this->length() ) {
        if ( accept != NULL ) {
	    size_t len = c_strlen(accept);
	    if ( 0 < len && accept[0] == '[' && accept[len-1] == ']' ) {
		tstring accepts;
		bool bl;
		int flags = 0;
		size_t p0 = 0;
		p0 += make_accepts_regexp(accept, true, accepts, &flags, &bl);
		if ( p0 == 0 ) {	/* パターンがおかしい */
		    ssize_t rp = this->tstring::find_first_not_of(pos,accept);
		    if ( rp < 0 ) ret = this->length() - pos;
		    else ret = rp - pos;
		}
		else {
		    ret = pattern_length(this->str_ptr_cs() + pos,
					 this->length() - pos,
					 accepts.cstr(), flags, bl);
		}
	    }
	    else {
		ssize_t rp = this->tstring::find_first_not_of(pos,accept);
		if ( rp < 0 ) ret = this->length() - pos;
		else ret = rp - pos;
	    }
	}
	else {
	    ssize_t rp = this->tstring::find_first_not_of(pos,"");
	    if ( rp < 0 ) ret = this->length() - pos;
	    else ret = rp - pos;
	}
    }
    else {
	pos = this->length();
    }
    if ( nextpos != NULL ) {
	if ( 0 < ret ) *nextpos = pos + ret;
	else {
	    if ( pos < this->length() ) *nextpos = pos + 1;
	    else *nextpos = this->length();
	}
    }
    return ret;
}

/**
 * @brief  文字セットに含まれる文字が連続する長さを取得
 *
 *  自身が持つ文字列の左側から右方向に，文字セット accept が
 *  連続する長さを検索し，その長さを返します．
 *
 * @param      accept 検出対象の文字セット
 * @return     成功した場合は正値<br>
 *             失敗した場合は0
 * @throw      内部バッファの確保に失敗した場合
 *
 */
size_t tstring::strspn( const tstring &accept ) const
{
    return this->strspn((size_t)0, accept.cstr());
}

/**
 * @brief  文字セットに含まれる文字が連続する長さを取得
 *
 *  自身が持つ文字列の左側から右方向に，文字セット accept が
 *  連続する長さを検索し，その長さを返します．
 *
 * @param      pos 検出の開始位置
 * @param      accept 検出対象の文字セット
 * @return     成功した場合は正値<br>
 *             失敗した場合は0
 * @throw      内部バッファの確保に失敗した場合
 *
 */
size_t tstring::strspn( size_t pos, const tstring &accept ) const
{
    return this->strspn(pos, accept.cstr());
}

/**
 * @brief  文字セットに含まれる文字が連続する長さを連続的に取得
 *
 *  自身が持つ文字列の左側から右方向に，文字セット accept が
 *  連続する長さを検索し，その長さを返します．
 *
 * @param      pos 検出の開始位置
 * @param      accept 検出対象の文字セット
 * @param      nextpos 次回のpos
 * @return     成功した場合は正値<br>
 *             失敗した場合は0
 * @throw      内部バッファの確保に失敗した場合
 *
 */
size_t tstring::strspn( size_t pos, const tstring &accept, 
			size_t *nextpos ) const
{
    return this->strspn(pos, accept.cstr(), nextpos);
}

/**
 * @brief  文字が連続する長さを取得
 *
 *  自身が持つ文字列の左側から右方向に，文字セット accept が
 *  連続する長さを検索し，その長さを返します．
 *
 * @param      accept 検出対象の文字セット
 * @return     成功した場合は正値<br>
 *             失敗した場合は0
 * @throw      内部バッファの確保に失敗した場合
 *
 */
size_t tstring::strspn( int accept ) const
{
    return this->strspn((size_t)0,accept);
}

/**
 * @brief  文字が連続する長さを取得
 *
 *  自身が持つ文字列の左側から右方向に，文字セット accept が
 *  連続する長さを検索し，その長さを返します．
 *
 * @param      pos 検出の開始位置
 * @param      accept 検出対象の文字セット
 * @return     成功した場合は正値<br>
 *             失敗した場合は0
 * @throw      内部バッファの確保に失敗した場合
 *
 */
size_t tstring::strspn( size_t pos, int accept ) const
{
    return this->strspn(pos,accept,(size_t *)NULL);
}

/**
 * @brief  文字が連続する長さを連続的に取得
 *
 *  自身が持つ文字列の左側から右方向に，文字セット accept が
 *  連続する長さを検索し，その長さを返します．
 *
 * @param      pos 検出の開始位置
 * @param      accept 検出対象の文字セット
 * @param      nextpos 次回のpos
 * @return     成功した場合は正値<br>
 *             失敗した場合は0
 * @throw      内部バッファの確保に失敗した場合
 *
 */
size_t tstring::strspn( size_t pos, int accept, size_t *nextpos ) const
{
    size_t ret = 0;
    if ( 0 < this->length() && pos < this->length() ) {
	ssize_t rp = tstring::find_first_not_of(pos,accept);
	if ( rp < 0 ) ret = this->length() - pos;
	else ret = rp - pos;
    }
    else {
	pos = this->length();
    }
    if ( nextpos != NULL ) {
	if ( 0 < ret ) *nextpos = pos + ret;
	else {
	    if ( pos < this->length() ) *nextpos = pos + 1;
	    else *nextpos = this->length();
	}
    }
    return ret;
}

/**
 * @brief  文字セットに含まれる文字が連続する長さを取得(右方向から検索)
 *
 *  自身が持つ文字列の右側から左方向に，文字セット accept が
 *  連続する長さを検索し，その長さを返します．
 *
 * @param      accept 検出対象の文字セット
 * @return     成功した場合は正値<br>
 *             失敗した場合は0
 * @throw      内部バッファの確保に失敗した場合
 *
 */
size_t tstring::strrspn( const char *accept ) const
{
    if ( 0 < this->length() )
	return this->strrspn(this->length() - 1, accept);
    return 0;
}

/**
 * @brief  文字セットに含まれる文字が連続する長さを取得(右方向から検索)
 *
 *  自身が持つ文字列の右側から左方向に，文字セット accept が
 *  連続する長さを検索し，その長さを返します．
 *
 * @param      pos 検出の開始位置
 * @param      accept 検出対象の文字セット
 * @return     成功した場合は正値<br>
 *             失敗した場合は0
 * @throw      内部バッファの確保に失敗した場合
 *
 */
size_t tstring::strrspn( size_t pos, const char *accept ) const
{
    return this->strrspn(pos,accept,(size_t *)NULL);
}

/**
 * @brief  文字セットに含まれる文字が連続する長さを取得(右方向から連続的に検索)
 *
 *  自身が持つ文字列の右側から左方向に，文字セット accept が
 *  連続する長さを検索し，その長さを返します．
 *
 * @param      pos 検出の開始位置
 * @param      accept 検出対象の文字セット
 * @param      nextpos 次回のpos
 * @return     成功した場合は正値<br>
 *             失敗した場合は0
 * @throw      内部バッファの確保に失敗した場合
 *
 */
size_t tstring::strrspn( size_t pos, const char *accept, size_t *nextpos ) const
{
    size_t ret = 0;
    if ( 0 < this->length() && pos < this->length() ) {
        if ( accept != NULL ) {
	    size_t len = c_strlen(accept);
	    if ( 0 < len && accept[0] == '[' && accept[len-1] == ']' ) {
		tstring accepts;
		bool bl;
		int flags = 0;
		size_t p0 = 0;
		p0 += make_accepts_regexp(accept, true, accepts, &flags, &bl);
		if ( p0 == 0 ) {	/* パターンがおかしい */
		    ssize_t rp = tstring::find_last_not_of(pos,accept);
		    if ( rp < 0 ) ret = pos + 1;
		    else ret = pos - rp;
		}
		else {
		    ret = pattern_rlength(this->cstr(),
					  pos + 1,
					  accepts.cstr(), flags, bl);
		}
	    }
	    else {
		ssize_t rp = tstring::find_last_not_of(pos,accept);
		if ( rp < 0 ) ret = pos + 1;
		else ret = pos - rp;
	    }
	}
	else {
	    ssize_t rp = tstring::find_last_not_of(pos,"");
	    if ( rp < 0 ) ret = pos + 1;
	    else ret = pos - rp;
	}
    }
    else {
	pos = 0;
    }
    if ( nextpos != NULL ) {
	if ( 0 < ret ) {
	    if ( ret <= pos ) *nextpos = pos - ret;
	    else *nextpos = this->length();
	}
	else {
	    if ( 0 < pos ) *nextpos = pos - 1;
	    else *nextpos = this->length();
	}
    }
    return ret;
}

/**
 * @brief  文字セットに含まれる文字が連続する長さを取得(右方向から検索)
 *
 *  自身が持つ文字列の右側から左方向に，文字セット accept が
 *  連続する長さを検索し，その長さを返します．
 *
 * @param      accept 検出対象の文字セット
 * @return     成功した場合は正値<br>
 *             失敗した場合は0
 * @throw      内部バッファの確保に失敗した場合
 *
 */
size_t tstring::strrspn( const tstring &accept ) const
{
    if ( 0 < this->length() )
	return this->strrspn(this->length() - 1, accept.cstr());
    return 0;
}

/**
 * @brief  文字セットに含まれる文字が連続する長さを取得(右方向から検索)
 *
 *  自身が持つ文字列の右側から左方向に，文字セット accept が
 *  連続する長さを検索し，その長さを返します．
 *
 * @param      pos 検出の開始位置
 * @param      accept 検出対象の文字セット
 * @return     成功した場合は正値<br>
 *             失敗した場合は0
 * @throw      内部バッファの確保に失敗した場合
 *
 */
size_t tstring::strrspn( size_t pos, const tstring &accept ) const
{
    return this->strrspn(pos, accept.cstr());
}

/**
 * @brief  文字セットに含まれる文字が連続する長さを取得(右方向から連続的に検索)
 *
 *  自身が持つ文字列の右側から左方向に，文字セット accept が
 *  連続する長さを検索し，その長さを返します．
 *
 * @param      pos 検出の開始位置
 * @param      accept 検出対象の文字セット
 * @param      nextpos 次回のpos
 * @return     成功した場合は正値<br>
 *             失敗した場合は0
 * @throw      内部バッファの確保に失敗した場合
 *
 */
size_t tstring::strrspn( size_t pos, const tstring &accept, 
			size_t *nextpos ) const
{
    return this->strrspn(pos, accept.cstr(), nextpos);
}

/**
 * @brief  文字が連続する長さを取得(右方向から検索)
 *
 *  自身が持つ文字列の右側から左方向に，文字セット accept が
 *  連続する長さを検索し，その長さを返します．
 *
 * @param      accept 検出対象の文字セット
 * @return     成功した場合は正値<br>
 *             失敗した場合は0
 * @throw      内部バッファの確保に失敗した場合
 *
 */
size_t tstring::strrspn( int accept ) const
{
    if ( 0 < this->length() )
	return this->strrspn(this->length() - 1,accept);
    return 0;
}

/**
 * @brief  文字が連続する長さを取得(右方向から検索)
 *
 *  自身が持つ文字列の右側から左方向に，文字セット accept が
 *  連続する長さを検索し，その長さを返します．
 *
 * @param      pos 検出の開始位置
 * @param      accept 検出対象の文字セット
 * @return     成功した場合は正値<br>
 *             失敗した場合は0
 * @throw      内部バッファの確保に失敗した場合
 *
 */
size_t tstring::strrspn( size_t pos, int accept ) const
{
    return this->strrspn(pos,accept,(size_t *)NULL);
}

/**
 * @brief  文字が連続する長さを取得(右方向から連続的に検索)
 *
 *  自身が持つ文字列の右側から左方向に，文字セット accept が
 *  連続する長さを検索し，その長さを返します．
 *
 * @param      pos 検出の開始位置
 * @param      accept 検出対象の文字セット
 * @param      nextpos 次回のpos
 * @return     成功した場合は正値<br>
 *             失敗した場合は0
 * @throw      内部バッファの確保に失敗した場合
 *
 */
size_t tstring::strrspn( size_t pos, int accept, size_t *nextpos ) const
{
    size_t ret = 0;
    if ( 0 < this->length() && pos < this->length() ) {
	ssize_t rp = tstring::find_last_not_of(pos,accept);
	if ( rp < 0 ) ret = pos + 1;
	else ret = pos - rp;
    }
    else {
	pos = 0;
    }
    if ( nextpos != NULL ) {
	if ( 0 < ret ) {
	    if ( ret <= pos ) *nextpos = pos - ret;
	    else *nextpos = this->length();
	}
	else {
	    if ( 0 < pos ) *nextpos = pos - 1;
	    else *nextpos = this->length();
	}
    }
    return ret;
}

/**
 * @brief  文字セットに含まれない文字が連続する長さを取得
 *
 *  自身が持つ文字列の左側から右方向に，文字セット reject が最初に出現する
 *  までの文字列の連続する長さを検索し，その長さを返します．
 *
 * @param      reject 検出対象外の文字セット
 * @return     成功した場合は正値<br>
 *             失敗した場合は0
 * @throw      内部バッファの確保に失敗した場合
 *
 */
size_t tstring::strcspn( const char *reject ) const
{
    return this->strcspn((size_t)0,reject);
}

/**
 * @brief  文字セットに含まれない文字が連続する長さを取得
 *
 *  自身が持つ文字列の左側から右方向に，文字セット reject が最初に出現する
 *  までの文字列の連続する長さを検索し，その長さを返します．
 *
 * @param      pos 検出の開始位置
 * @param      reject 検出対象外の文字セット
 * @return     成功した場合は正値<br>
 *             失敗した場合は0
 * @throw      内部バッファの確保に失敗した場合
 *
 */
size_t tstring::strcspn( size_t pos, const char *reject ) const
{
    return this->strcspn(pos,reject,(size_t *)NULL);
}

/**
 * @brief  文字セットに含まれない文字が連続する長さを取得(連続的に検索)
 *
 *  自身が持つ文字列の左側から右方向に，文字セット reject が最初に出現する
 *  までの文字列の連続する長さを検索し，その長さを返します．
 *
 * @param      pos 検出の開始位置
 * @param      reject 検出対象外の文字セット
 * @param      nextpos 次回のpos
 * @return     成功した場合は正値<br>
 *             失敗した場合は0
 * @throw      内部バッファの確保に失敗した場合
 *
 */
size_t tstring::strcspn(size_t pos, const char *reject, size_t *nextpos) const
{
    size_t ret = 0;
    if ( 0 < this->length() && pos < this->length() ) {
        if ( reject != NULL ) {
	    size_t len = c_strlen(reject);
	    if ( 0 < len && reject[0] == '[' && reject[len-1] == ']' ) {
		tstring rejects;
		bool bl;
		int flags = 0;
		size_t p0 = 0;
		p0 += make_accepts_regexp(reject, true, rejects, &flags, &bl);
		if ( p0 == 0 ) {	/* パターンがおかしい */
		    ssize_t rp = this->tstring::find_first_of(pos,reject);
		    if ( rp < 0 ) ret = this->length() - pos;
		    else ret = rp - pos;
		}
		else {
		    bool bl1 = ((bl == true) ? false : true);
		    ret = pattern_length(this->str_ptr_cs() + pos,
					 this->length() - pos,
					 rejects.cstr(), flags, bl1);
		}
	    }
	    else {
		ssize_t rp = this->tstring::find_first_of(pos,reject);
		if ( rp < 0 ) ret = this->length() - pos;
		else ret = rp - pos;
	    }
	}
	else {
	    ssize_t rp = this->tstring::find_first_of(pos,"");
	    if ( rp < 0 ) ret = this->length() - pos;
	    else ret = rp - pos;
	}
    }
    else {
	pos = this->length();
    }
    if ( nextpos != NULL ) {
	if ( 0 < ret ) *nextpos = pos + ret;
	else {
	    if ( pos < this->length() ) *nextpos = pos + 1;
	    else *nextpos = this->length();
	}
    }
    return ret;
}

/**
 * @brief  文字セットに含まれない文字が連続する長さを取得
 *
 *  自身が持つ文字列の左側から右方向に，文字セット reject が最初に出現する
 *  までの文字列の連続する長さを検索し，その長さを返します．
 *
 * @param      reject 検出対象外の文字セット
 * @return     成功した場合は正値<br>
 *             失敗した場合は0
 * @throw      内部バッファの確保に失敗した場合
 *
 */
size_t tstring::strcspn( const tstring &reject ) const
{
    return this->strcspn((size_t)0, reject.cstr());
}

/**
 * @brief  文字セットに含まれない文字が連続する長さを取得
 *
 *  自身が持つ文字列の左側から右方向に，文字セット reject が最初に出現する
 *  までの文字列の連続する長さを検索し，その長さを返します．
 *
 * @param      pos 検出の開始位置
 * @param      reject 検出対象外の文字セット
 * @return     成功した場合は正値<br>
 *             失敗した場合は0
 * @throw      内部バッファの確保に失敗した場合
 *
 */
size_t tstring::strcspn( size_t pos, const tstring &reject ) const
{
    return this->strcspn(pos, reject.cstr());
}

/**
 * @brief  文字セットに含まれない文字が連続する長さを取得(連続的に検索)
 *
 *  自身が持つ文字列の左側から右方向に，文字セット reject が最初に出現する
 *  までの文字列の連続する長さを検索し，その長さを返します．
 *
 * @param      pos 検出の開始位置
 * @param      reject 検出対象外の文字セット
 * @param      nextpos 次回のpos
 * @return     成功した場合は正値<br>
 *             失敗した場合は0
 * @throw      内部バッファの確保に失敗した場合
 *
 */
size_t tstring::strcspn( size_t pos, const tstring &reject, 
			 size_t *nextpos ) const
{
    return this->strcspn(pos, reject.cstr(), nextpos);
}

/**
 * @brief  指定された文字ではない文字が連続する長さを返す
 *
 *  自身が持つ文字列の左側から右方向に，文字セット reject が最初に出現する
 *  までの文字列の連続する長さを検索し，その長さを返します．
 *
 * @param      reject 検出対象外の文字セット
 * @return     成功した場合は正値<br>
 *             失敗した場合は0
 * @throw      内部バッファの確保に失敗した場合
 *
 */
size_t tstring::strcspn( int reject ) const
{
    return this->strcspn((size_t)0,reject);
}

/**
 * @brief  指定された文字ではない文字が連続する長さを返す
 *
 *  自身が持つ文字列の左側から右方向に，文字セット reject が最初に出現する
 *  までの文字列の連続する長さを検索し，その長さを返します．
 *
 * @param      pos 検出の開始位置
 * @param      reject 検出対象外の文字セット
 * @return     成功した場合は正値<br>
 *             失敗した場合は0
 * @throw      内部バッファの確保に失敗した場合
 *
 */
size_t tstring::strcspn( size_t pos, int reject ) const
{
    return this->strcspn(pos,reject,(size_t *)NULL);
}

/**
 * @brief  指定された文字ではない文字が連続する長さを返す(連続的に検索)
 *
 *  自身が持つ文字列の左側から右方向に，文字セット reject が最初に出現する
 *  までの文字列の連続する長さを検索し，その長さを返します．
 *
 * @param      pos 検出の開始位置
 * @param      reject 検出対象外の文字セット
 * @param      nextpos 次回のpos
 * @return     成功した場合は正値<br>
 *             失敗した場合は0
 * @throw      内部バッファの確保に失敗した場合
 *
 */
size_t tstring::strcspn( size_t pos, int reject, size_t *nextpos ) const
{
    size_t ret = 0;
    if ( 0 < this->length() && pos < this->length() ) {
	ssize_t rp = tstring::find_first_of(pos,reject);
	if ( rp < 0 ) ret = this->length() - pos;
	else ret = rp - pos;
    }
    else {
	pos = this->length();
    }
    if ( nextpos != NULL ) {
	if ( 0 < ret ) *nextpos = pos + ret;
	else {
	    if ( pos < this->length() ) *nextpos = pos + 1;
	    else *nextpos = this->length();
	}
    }
    return ret;
}

/**
 * @brief  シェルのワイルドカードパターンを用いた文字列マッチを試行
 *
 *  自身の文字列に対し，シェルのワイルドカードパターンを用いた文字列マッチを
 *  試行し，その結果を返します．
 *
 * @param      pat 文字パターン
 * @return     成功した場合は0<br>
 *             失敗した場合は負値
 *
 */
int tstring::strmatch( const char *pat ) const
{
    const char *this_str_ptr = this->str_ptr_cs();

    if ( this_str_ptr == NULL || pat == NULL ) return -1;

    return c_strmatch(pat,this_str_ptr) == 0 ? 0 : -1;
}

/**
 * @brief  シェルのワイルドカードパターンを用いた文字列マッチを試行
 *
 *  自身の文字列に対し，シェルのワイルドカードパターンを用いた文字列マッチを
 *  試行し，その結果を返します．
 *
 * @param      pos 文字列マッチの開始位置
 * @param      pat 文字パターン
 * @return     成功した場合は0<br>
 *             失敗した場合は負値
 *
 */
int tstring::strmatch( size_t pos, const char *pat ) const
{
    const char *this_str_ptr = this->str_ptr_cs();

    if ( this_str_ptr == NULL || pat == NULL ) return -1;
    if ( this->length() < pos ) return -1;

    return c_strmatch(pat,this_str_ptr + pos) == 0 ? 0 : -1;
}

/**
 * @brief  シェルのワイルドカードパターンを用いた文字列マッチを試行
 *
 *  自身の文字列に対し，シェルのワイルドカードパターンを用いた文字列マッチを
 *  試行し，その結果を返します．
 *
 * @param      pat 文字パターン
 * @return     成功した場合は0<br>
 *             失敗した場合は負値
 *
 */
int tstring::strmatch( const tstring &pat ) const
{
    const char *this_str_ptr = this->str_ptr_cs();
    const char *pat_cstr = pat.cstr();

    if ( this_str_ptr == NULL || pat_cstr == NULL ) return -1;

    return c_strmatch(pat_cstr,this_str_ptr) == 0 ? 0 : -1;
}

/**
 * @brief  シェルのワイルドカードパターンを用いた文字列マッチを試行
 *
 *  自身の文字列に対し，シェルのワイルドカードパターンを用いた文字列マッチを
 *  試行し，その結果を返します．
 *
 * @param      pos 文字列マッチの開始位置
 * @param      pat 文字パターン
 * @return     成功した場合は0<br>
 *             失敗した場合は負値
 *
 */
int tstring::strmatch( size_t pos, const tstring &pat ) const
{
    const char *this_str_ptr = this->str_ptr_cs();
    const char *pat_cstr = pat.cstr();

    if ( this_str_ptr == NULL || pat_cstr == NULL ) return -1;
    if ( this->length() < pos ) return -1;

    return c_strmatch(pat_cstr,this_str_ptr + pos) == 0 ? 0 : -1;
}

/**
 * @brief  シェルのワイルドカードパターンを用いた文字列マッチ(ファイル名向き)
 *
 *  自身の文字列に対し，シェルのワイルドカードパターンを用いた文字列マッチを
 *  試行し，その結果を返します．<br>
 *  自身の文字列の先頭のピリオドは pat 中のピリオドそのものにしかマッチ
 *  しません．
 *
 * @param      pat 文字パターン
 * @return     成功した場合は0<br>
 *             失敗した場合は負値
 *
 */
int tstring::fnmatch( const char *pat ) const
{
    const char *this_str_ptr = this->str_ptr_cs();

    if ( this_str_ptr == NULL || pat == NULL ) return -1;

    return c_fnmatch(pat,this_str_ptr) == 0 ? 0 : -1;
}

/**
 * @brief  シェルのワイルドカードパターンを用いた文字列マッチ(ファイル名向き)
 *
 *  自身の文字列に対し，シェルのワイルドカードパターンを用いた文字列マッチを
 *  試行し，その結果を返します．<br>
 *  自身の文字列の先頭のピリオドは pat 中のピリオドそのものにしかマッチ
 *  しません．
 *
 * @param      pos 文字列マッチの開始位置
 * @param      pat 文字パターン
 * @return     成功した場合は0<br>
 *             失敗した場合は負値
 *
 */
int tstring::fnmatch( size_t pos, const char *pat ) const
{
    const char *this_str_ptr = this->str_ptr_cs();

    if ( this_str_ptr == NULL || pat == NULL ) return -1;
    if ( this->length() < pos ) return -1;

    return c_fnmatch(pat,this_str_ptr + pos) == 0 ? 0 : -1;
}

/**
 * @brief  シェルのワイルドカードパターンを用いた文字列マッチ(ファイル名向き)
 *
 *  自身の文字列に対し，シェルのワイルドカードパターンを用いた文字列マッチを
 *  試行し，その結果を返します．<br>
 *  自身の文字列の先頭のピリオドは pat 中のピリオドそのものにしかマッチ
 *  しません．
 *
 * @param      pat 文字パターン
 * @return     成功した場合は0<br>
 *             失敗した場合は負値
 *
 */
int tstring::fnmatch( const tstring &pat ) const
{
    const char *this_str_ptr = this->str_ptr_cs();
    const char *pat_cstr = pat.cstr();

    if ( this_str_ptr == NULL || pat_cstr == NULL ) return -1;

    return c_fnmatch(pat_cstr,this_str_ptr) == 0 ? 0 : -1;
}

/**
 * @brief  シェルのワイルドカードパターンを用いた文字列マッチ(ファイル名向き)
 *
 *  自身の文字列に対し，シェルのワイルドカードパターンを用いた文字列マッチを
 *  試行し，その結果を返します．<br>
 *  自身の文字列の先頭のピリオドは pat 中のピリオドそのものにしかマッチ
 *  しません．
 *
 * @param      pos 文字列マッチの開始位置
 * @param      pat 文字パターン
 * @return     成功した場合は0<br>
 *             失敗した場合は負値
 *
 */
int tstring::fnmatch( size_t pos, const tstring &pat ) const
{
    const char *this_str_ptr = this->str_ptr_cs();
    const char *pat_cstr = pat.cstr();

    if ( this_str_ptr == NULL || pat_cstr == NULL ) return -1;
    if ( this->length() < pos ) return -1;

    return c_fnmatch(pat_cstr,this_str_ptr + pos) == 0 ? 0 : -1;
}

/**
 * @brief  シェルのワイルドカードパターンを用いた文字列マッチ(パス名向き)
 *
 *  自身の文字列に対し，シェルのワイルドカードパターンを用いた文字列マッチを
 *  試行し，その結果を返します．<br>
 *  自身の文字列の先頭のピリオド，スラッシュの直後のピリオド，スラッシュ '/'
 *  は pat 中のワイルドカードや [] シーケンス等にはマッチしません．
 *
 * @param      pat 文字パターン
 * @return     成功した場合は0<br>
 *             失敗した場合は負値
 *
 */
int tstring::pnmatch( const char *pat ) const
{
    const char *this_str_ptr = this->str_ptr_cs();

    if ( this_str_ptr == NULL || pat == NULL ) return -1;

    return c_pnmatch(pat,this_str_ptr) == 0 ? 0 : -1;
}

/**
 * @brief  シェルのワイルドカードパターンを用いた文字列マッチ(パス名向き)
 *
 *  自身の文字列に対し，シェルのワイルドカードパターンを用いた文字列マッチを
 *  試行し，その結果を返します．<br>
 *  自身の文字列の先頭のピリオド，スラッシュの直後のピリオド，スラッシュ '/'
 *  は pat 中のワイルドカードや [] シーケンス等にはマッチしません．
 *
 * @param      pos 文字列マッチの開始位置
 * @param      pat 文字パターン
 * @return     成功した場合は0<br>
 *             失敗した場合は負値
 *
 */
int tstring::pnmatch( size_t pos, const char *pat ) const
{
    const char *this_str_ptr = this->str_ptr_cs();

    if ( this_str_ptr == NULL || pat == NULL ) return -1;
    if ( this->length() < pos ) return -1;

    return c_pnmatch(pat,this_str_ptr + pos) == 0 ? 0 : -1;
}

/**
 * @brief  シェルのワイルドカードパターンを用いた文字列マッチ(パス名向き)
 *
 *  自身の文字列に対し，シェルのワイルドカードパターンを用いた文字列マッチを
 *  試行し，その結果を返します．<br>
 *  自身の文字列の先頭のピリオド，スラッシュの直後のピリオド，スラッシュ '/'
 *  は pat 中のワイルドカードや [] シーケンス等にはマッチしません．
 *
 * @param      pat 文字パターン
 * @return     成功した場合は0<br>
 *             失敗した場合は負値
 *
 */
int tstring::pnmatch( const tstring &pat ) const
{
    const char *this_str_ptr = this->str_ptr_cs();
    const char *pat_cstr = pat.cstr();

    if ( this_str_ptr == NULL || pat_cstr == NULL ) return -1;

    return c_pnmatch(pat_cstr,this_str_ptr) == 0 ? 0 : -1;
}

/**
 * @brief  シェルのワイルドカードパターンを用いた文字列マッチ(パス名向き)
 *
 *  自身の文字列に対し，シェルのワイルドカードパターンを用いた文字列マッチを
 *  試行し，その結果を返します．<br>
 *  自身の文字列の先頭のピリオド，スラッシュの直後のピリオド，スラッシュ '/'
 *  は pat 中のワイルドカードや [] シーケンス等にはマッチしません．
 *
 * @param      pos 文字列マッチの開始位置
 * @param      pat 文字パターン
 * @return     成功した場合は0<br>
 *             失敗した場合は負値
 *
 */
int tstring::pnmatch( size_t pos, const tstring &pat ) const
{
    const char *this_str_ptr = this->str_ptr_cs();
    const char *pat_cstr = pat.cstr();

    if ( this_str_ptr == NULL || pat_cstr == NULL ) return -1;
    if ( this->length() < pos ) return -1;

    return c_pnmatch(pat_cstr,this_str_ptr + pos) == 0 ? 0 : -1;
}

/* regmatch */

#ifdef REGFUNCS_USING_CACHE
/* 強引にキャストする関数 */
static tregex *coerce_into_casting( const tregex *in )
{ 
    return (tregex *)in;
} 
#endif

/**
 * @brief  正規表現による文字列マッチを試行
 *
 *  自身の文字列に対し，POSIX 拡張正規表現(以下，正規表現) による文字列マッチを
 *  試行し，その結果を返します．
 *
 * @param      pat 文字パターン(正規表現)
 * @param      ret_span マッチした文字列の長さ
 * @return     成功した場合は非負値<br>
 *             失敗した場合は負値
 * @throw      regexルーチンがメモリを使い果たしている場合
 * @throw      内部バッファの確保に失敗した場合
 * @throw      メモリ破壊を起こした場合
 *
 */
ssize_t tstring::regmatch( const char *pat, size_t *ret_span ) const
{
    return this->regmatch((size_t)0, pat, ret_span, (size_t *)NULL);
}

/**
 * @brief  正規表現による文字列マッチを試行
 *
 *  自身の文字列に対し，POSIX 拡張正規表現(以下，正規表現) による文字列マッチを
 *  試行し，その結果を返します．
 *
 * @param      pos 文字列マッチの開始位置
 * @param      pat 文字パターン(正規表現)
 * @param      ret_span マッチした文字列の長さ
 * @return     成功した場合は非負値<br>
 *             失敗した場合は負値
 * @throw      regexルーチンがメモリを使い果たしている場合
 * @throw      内部バッファの確保に失敗した場合
 * @throw      メモリ破壊を起こした場合
 *
 */
ssize_t tstring::regmatch( size_t pos, const char *pat, 
			   size_t *ret_span ) const
{
    return this->regmatch(pos, pat, ret_span, (size_t *)NULL);
}

/**
 * @brief  正規表現による文字列マッチを連続的に試行
 *
 *  自身の文字列に対し，POSIX 拡張正規表現(以下，正規表現) による文字列マッチを
 *  試行し，その結果を返します．
 *
 * @param      pos 文字列マッチの開始位置
 * @param      pat 文字パターン(正規表現)
 * @param      ret_span マッチした文字列の長さ
 * @param      nextpos 次回のpos
 * @return     成功した場合は非負値<br>
 *             失敗した場合は負値
 * @throw      regexルーチンがメモリを使い果たしている場合
 * @throw      内部バッファの確保に失敗した場合
 * @throw      メモリ破壊を起こした場合
 *
 */
ssize_t tstring::regmatch( size_t pos, const char *pat, size_t *ret_span,
			   size_t *nextpos ) const
{
    tregex *epat_p;
#ifdef REGFUNCS_USING_CACHE
    epat_p = coerce_into_casting(&(this->regex_rec));
#else
    tregex epat;
    epat_p = &epat;
#endif
    if ( epat_p->compile(pat) < 0 ) {
	err_report1(__FUNCTION__,"ERROR","%s",epat_p->cerrstr());
    }
    return this->regmatch(pos, *epat_p, ret_span, nextpos);
}

/**
 * @brief  正規表現による文字列マッチを試行
 *
 *  自身の文字列に対し，POSIX 拡張正規表現(以下，正規表現) による文字列マッチを
 *  試行し，その結果を返します．
 *
 * @param      pat 文字パターン(正規表現)
 * @param      ret_span マッチした文字列の長さ
 * @return     成功した場合は非負値<br>
 *             失敗した場合は負値
 * @throw      regexルーチンがメモリを使い果たしている場合
 * @throw      内部バッファの確保に失敗した場合
 * @throw      メモリ破壊を起こした場合
 *
 */
ssize_t tstring::regmatch( const tstring &pat, size_t *ret_span ) const
{
    return this->regmatch((size_t)0, pat, ret_span, (size_t *)NULL);
}

/**
 * @brief  正規表現による文字列マッチを試行
 *
 *  自身の文字列に対し，POSIX 拡張正規表現(以下，正規表現) による文字列マッチを
 *  試行し，その結果を返します．
 *
 * @param      pos 文字列マッチの開始位置
 * @param      pat 文字パターン(正規表現)
 * @param      ret_span マッチした文字列の長さ
 * @return     成功した場合は非負値<br>
 *             失敗した場合は負値
 * @throw      regexルーチンがメモリを使い果たしている場合
 * @throw      内部バッファの確保に失敗した場合
 * @throw      メモリ破壊を起こした場合
 *
 */
ssize_t tstring::regmatch( size_t pos, const tstring &pat, 
			   size_t *ret_span ) const
{
    return this->regmatch(pos, pat, ret_span, (size_t *)NULL);
}

/**
 * @brief  正規表現による文字列マッチを連続的に試行
 *
 *  自身の文字列に対し，POSIX 拡張正規表現(以下，正規表現) による文字列マッチを
 *  試行し，その結果を返します．
 *
 * @param      pos 文字列マッチの開始位置
 * @param      pat 文字パターン(正規表現)
 * @param      ret_span マッチした文字列の長さ
 * @param      nextpos 次回のpos
 * @return     成功した場合は非負値<br>
 *             失敗した場合は負値
 * @throw      regexルーチンがメモリを使い果たしている場合
 * @throw      内部バッファの確保に失敗した場合
 * @throw      メモリ破壊を起こした場合
 *
 */
ssize_t tstring::regmatch( size_t pos, const tstring &pat, size_t *ret_span,
			   size_t *nextpos ) const
{
    tregex *epat_p;
#ifdef REGFUNCS_USING_CACHE
    epat_p = coerce_into_casting(&(this->regex_rec));
#else
    tregex epat;
    epat_p = &epat;
#endif
    if ( epat_p->compile(pat.cstr()) < 0 ) {
	err_report1(__FUNCTION__,"ERROR","%s",epat_p->cerrstr());
    }
    return this->regmatch(pos, *epat_p, ret_span, nextpos);
}


/**
 * @brief  正規表現による文字列マッチを試行
 *
 *  自身の文字列に対し，POSIX 拡張正規表現(以下，正規表現) による文字列マッチを
 *  試行し，その結果を返します．
 *
 * @param      pat tregex クラスのコンパイル済オブジェクト
 * @param      ret_span マッチした文字列の長さ
 * @return     成功した場合は非負値<br>
 *             失敗した場合は負値
 * @throw      regexルーチンがメモリを使い果たしている場合
 * @throw      内部バッファの確保に失敗した場合
 * @throw      メモリ破壊を起こした場合
 *
 */
ssize_t tstring::regmatch( const tregex &pat, size_t *ret_span ) const
{
    return this->regexp_match((size_t)0, pat, ret_span);
}

/**
 * @brief  正規表現による文字列マッチを試行
 *
 *  自身の文字列に対し，POSIX 拡張正規表現(以下，正規表現) による文字列マッチを
 *  試行し，その結果を返します．
 *
 * @param      pos 文字列マッチの開始位置
 * @param      pat tregex クラスのコンパイル済オブジェクト
 * @param      ret_span マッチした文字列の長さ
 * @return     成功した場合は非負値<br>
 *             失敗した場合は負値
 * @throw      regexルーチンがメモリを使い果たしている場合
 * @throw      内部バッファの確保に失敗した場合
 * @throw      メモリ破壊を起こした場合
 *
 */
ssize_t tstring::regmatch( size_t pos, const tregex &pat, 
			   size_t *ret_span ) const
{
    return this->regexp_match(pos, pat, ret_span);
}

/**
 * @brief  正規表現による文字列マッチを連続的に試行
 *
 *  自身の文字列に対し，POSIX 拡張正規表現(以下，正規表現) による文字列マッチを
 *  試行し，その結果を返します．
 *
 * @param      pos 文字列マッチの開始位置
 * @param      pat tregex クラスのコンパイル済オブジェクト
 * @param      ret_span マッチした文字列の長さ
 * @param      nextpos 次回のpos
 * @return     成功した場合は非負値<br>
 *             失敗した場合は負値
 * @throw      regexルーチンがメモリを使い果たしている場合
 * @throw      内部バッファの確保に失敗した場合
 * @throw      メモリ破壊を起こした場合
 *
 */
ssize_t tstring::regmatch( size_t pos, const tregex &pat, size_t *ret_span,
			   size_t *nextpos ) const
{
    ssize_t ret;
    size_t r_span;
    ret = this->regexp_match(pos, pat, &r_span);
    if ( nextpos != NULL ) {
	size_t add_span = ((r_span == 0) ? 1 : r_span);
	if ( 0 <= ret ) *nextpos = ret + add_span;
	else *nextpos = this->length() + 1;
    }
    if ( ret_span != NULL ) *ret_span = r_span;
    return ret;
}

/* regmatch advanced 1 */

/**
 * @brief  正規表現による文字列マッチを試行
 *
 *  自身の文字列に対し，POSIX 拡張正規表現(以下，正規表現) による文字列マッチを
 *  試行し，後方参照の情報を含む結果を得る事ができます．
 *
 * @param      pat 文字パターン(正規表現)
 * @param      max_nelem pos_r，len_r のバッファ長
 * @param      pos_r マッチした文字列の位置
 * @param      len_r マッチした文字列の長さ
 * @param      nelem_r pos_r，len_r にセットされた値の個数
 * @return     成功した場合は非負値<br>
 *             失敗した場合は負値
 * @throw      regexルーチンがメモリを使い果たしている場合
 * @throw      内部バッファの確保に失敗した場合
 * @throw      メモリ破壊を起こした場合
 *
 */
ssize_t tstring::regmatch( const char *pat,
			   size_t max_nelem, size_t pos_r[], size_t len_r[],
			   size_t *nelem_r ) const
{
    return this->regmatch((size_t)0, pat, 
			  max_nelem, pos_r, len_r, nelem_r, (size_t *)NULL);
}

/**
 * @brief  正規表現による文字列マッチを試行
 *
 *  自身の文字列に対し，POSIX 拡張正規表現(以下，正規表現) による文字列マッチを
 *  試行し，後方参照の情報を含む結果を得る事ができます．
 *
 * @param      pos 文字列マッチの開始位置
 * @param      pat 文字パターン(正規表現)
 * @param      max_nelem pos_r，len_r のバッファ長
 * @param      pos_r マッチした文字列の位置
 * @param      len_r マッチした文字列の長さ
 * @param      nelem_r pos_r，len_r にセットされた値の個数
 * @return     成功した場合は非負値<br>
 *             失敗した場合は負値
 * @throw      regexルーチンがメモリを使い果たしている場合
 * @throw      内部バッファの確保に失敗した場合
 * @throw      メモリ破壊を起こした場合
 *
 */
ssize_t tstring::regmatch( size_t pos, const char *pat, 
			   size_t max_nelem, size_t pos_r[], size_t len_r[],
			   size_t *nelem_r ) const
{
    return this->regmatch(pos, pat,
			  max_nelem, pos_r, len_r, nelem_r, (size_t *)NULL);
}

/**
 * @brief  正規表現による文字列マッチを連続的に試行
 *
 *  自身の文字列に対し，POSIX 拡張正規表現(以下，正規表現) による文字列マッチを
 *  試行し，後方参照の情報を含む結果を得る事ができます．
 *
 * @param      pos 文字列マッチの開始位置
 * @param      pat 文字パターン(正規表現)
 * @param      max_nelem pos_r，len_r のバッファ長
 * @param      pos_r マッチした文字列の位置
 * @param      len_r マッチした文字列の長さ
 * @param      nelem_r pos_r，len_r にセットされた値の個数
 * @param      nextpos 次回のpos
 * @return     成功した場合は非負値<br>
 *             失敗した場合は負値
 * @throw      regexルーチンがメモリを使い果たしている場合
 * @throw      内部バッファの確保に失敗した場合
 * @throw      メモリ破壊を起こした場合
 *
 */
ssize_t tstring::regmatch( size_t pos, const char *pat, 
			   size_t max_nelem, size_t pos_r[], size_t len_r[],
			   size_t *nelem_r, size_t *nextpos ) const
{
    tregex *epat_p;
#ifdef REGFUNCS_USING_CACHE
    epat_p = coerce_into_casting(&(this->regex_rec));
#else
    tregex epat;
    epat_p = &epat;
#endif
    if ( epat_p->compile(pat) < 0 ) {
	err_report1(__FUNCTION__,"ERROR","%s",epat_p->cerrstr());
    }
    return this->regmatch(pos, *epat_p,
			  max_nelem, pos_r, len_r, nelem_r, nextpos);
}

/**
 * @brief  正規表現による文字列マッチを試行
 *
 *  自身の文字列に対し，POSIX 拡張正規表現(以下，正規表現) による文字列マッチを
 *  試行し，後方参照の情報を含む結果を得る事ができます．
 *
 * @param      pat 文字パターン(正規表現)
 * @param      max_nelem pos_r，len_r のバッファ長
 * @param      pos_r マッチした文字列の位置
 * @param      len_r マッチした文字列の長さ
 * @param      nelem_r pos_r，len_r にセットされた値の個数
 * @return     成功した場合は非負値<br>
 *             失敗した場合は負値
 * @throw      regexルーチンがメモリを使い果たしている場合
 * @throw      内部バッファの確保に失敗した場合
 * @throw      メモリ破壊を起こした場合
 *
 */
ssize_t tstring::regmatch( const tstring &pat, 
			   size_t max_nelem, size_t pos_r[], size_t len_r[],
			   size_t *nelem_r ) const
{
    return this->regmatch((size_t)0, pat,
			  max_nelem, pos_r, len_r, nelem_r, (size_t *)NULL);
}

/**
 * @brief  正規表現による文字列マッチを試行
 *
 *  自身の文字列に対し，POSIX 拡張正規表現(以下，正規表現) による文字列マッチを
 *  試行し，後方参照の情報を含む結果を得る事ができます．
 *
 * @param      pos 文字列マッチの開始位置
 * @param      pat 文字パターン(正規表現)
 * @param      max_nelem pos_r，len_r のバッファ長
 * @param      pos_r マッチした文字列の位置
 * @param      len_r マッチした文字列の長さ
 * @param      nelem_r pos_r，len_r にセットされた値の個数
 * @return     成功した場合は非負値<br>
 *             失敗した場合は負値
 * @throw      regexルーチンがメモリを使い果たしている場合
 * @throw      内部バッファの確保に失敗した場合
 * @throw      メモリ破壊を起こした場合
 *
 */
ssize_t tstring::regmatch( size_t pos, const tstring &pat,
			   size_t max_nelem, size_t pos_r[], size_t len_r[],
			   size_t *nelem_r ) const
{
    return this->regmatch(pos, pat,
			  max_nelem, pos_r, len_r, nelem_r, (size_t *)NULL);
}

/**
 * @brief  正規表現による文字列マッチを連続的に試行
 *
 *  自身の文字列に対し，POSIX 拡張正規表現(以下，正規表現) による文字列マッチを
 *  試行し，後方参照の情報を含む結果を得る事ができます．
 *
 * @param      pos 文字列マッチの開始位置
 * @param      pat 文字パターン(正規表現)
 * @param      max_nelem pos_r，len_r のバッファ長
 * @param      pos_r マッチした文字列の位置
 * @param      len_r マッチした文字列の長さ
 * @param      nelem_r pos_r，len_r にセットされた値の個数
 * @param      nextpos 次回のpos
 * @return     成功した場合は非負値<br>
 *             失敗した場合は負値
 * @throw      regexルーチンがメモリを使い果たしている場合
 * @throw      内部バッファの確保に失敗した場合
 * @throw      メモリ破壊を起こした場合
 *
 */
ssize_t tstring::regmatch( size_t pos, const tstring &pat,
			   size_t max_nelem, size_t pos_r[], size_t len_r[],
			   size_t *nelem_r, size_t *nextpos ) const
{
    tregex *epat_p;
#ifdef REGFUNCS_USING_CACHE
    epat_p = coerce_into_casting(&(this->regex_rec));
#else
    tregex epat;
    epat_p = &epat;
#endif
    if ( epat_p->compile(pat.cstr()) < 0 ) {
	err_report1(__FUNCTION__,"ERROR","%s",epat_p->cerrstr());
    }
    return this->regmatch(pos, *epat_p,
			  max_nelem, pos_r, len_r, nelem_r, nextpos);
}

/**
 * @brief  正規表現による文字列マッチを試行
 *
 *  自身の文字列に対し，POSIX 拡張正規表現(以下，正規表現) による文字列マッチを
 *  試行し，後方参照の情報を含む結果を得る事ができます．
 *
 * @param      pat tregex クラスのコンパイル済オブジェクト
 * @param      max_nelem pos_r，len_r のバッファ長
 * @param      pos_r マッチした文字列の位置
 * @param      len_r マッチした文字列の長さ
 * @param      nelem_r pos_r，len_r にセットされた値の個数
 * @return     成功した場合は非負値<br>
 *             失敗した場合は負値
 * @throw      regexルーチンがメモリを使い果たしている場合
 * @throw      内部バッファの確保に失敗した場合
 * @throw      メモリ破壊を起こした場合
 *
 */
ssize_t tstring::regmatch( const tregex &pat, 
			   size_t max_nelem, size_t pos_r[], size_t len_r[],
			   size_t *nelem_r ) const
{
    return this->regexp_match_advanced1((size_t)0, pat,
					max_nelem, pos_r, len_r, nelem_r);
}

/**
 * @brief  正規表現による文字列マッチを試行
 *
 *  自身の文字列に対し，POSIX 拡張正規表現(以下，正規表現) による文字列マッチを
 *  試行し，後方参照の情報を含む結果を得る事ができます．
 *
 * @param      pos 文字列マッチの開始位置
 * @param      pat tregex クラスのコンパイル済オブジェクト
 * @param      max_nelem pos_r，len_r のバッファ長
 * @param      pos_r マッチした文字列の位置
 * @param      len_r マッチした文字列の長さ
 * @param      nelem_r pos_r，len_r にセットされた値の個数
 * @return     成功した場合は非負値<br>
 *             失敗した場合は負値
 * @throw      regexルーチンがメモリを使い果たしている場合
 * @throw      内部バッファの確保に失敗した場合
 * @throw      メモリ破壊を起こした場合
 *
 */
ssize_t tstring::regmatch( size_t pos, const tregex &pat,
			   size_t max_nelem, size_t pos_r[], size_t len_r[], 
			   size_t *nelem_r ) const
{
    return this->regexp_match_advanced1(pos, pat,
					max_nelem, pos_r, len_r, nelem_r);
}

/**
 * @brief  正規表現による文字列マッチを連続的に試行
 *
 *  自身の文字列に対し，POSIX 拡張正規表現(以下，正規表現) による文字列マッチを
 *  試行し，後方参照の情報を含む結果を得る事ができます．
 *
 * @param      pos 文字列マッチの開始位置
 * @param      pat tregex クラスのコンパイル済オブジェクト
 * @param      max_nelem pos_r，len_r のバッファ長
 * @param      pos_r マッチした文字列の位置
 * @param      len_r マッチした文字列の長さ
 * @param      nelem_r pos_r，len_r にセットされた値の個数
 * @param      nextpos 次回のpos
 * @return     成功した場合は非負値<br>
 *             失敗した場合は負値
 * @throw      regexルーチンがメモリを使い果たしている場合
 * @throw      内部バッファの確保に失敗した場合
 * @throw      メモリ破壊を起こした場合
 *
 */
ssize_t tstring::regmatch( size_t pos, const tregex &pat,
			   size_t max_nelem, size_t pos_r[], size_t len_r[], 
			   size_t *nelem_r, size_t *nextpos ) const
{
    ssize_t ret;
    ret = this->regexp_match_advanced1(pos, pat,
				       max_nelem, pos_r, len_r, nelem_r);
    if ( nextpos != NULL ) {
	size_t add_span = ((len_r[0] == 0) ? 1 : len_r[0]);
	if ( 0 <= ret ) *nextpos = ret + add_span;
	else *nextpos = this->length() + 1;
    }
    return ret;
}

/* regmatch advanced 2 */

/**
 * @brief  正規表現による文字列マッチを試行 (非推奨)
 *
 *  自身の文字列に対し，POSIX 拡張正規表現(以下，正規表現) による文字列マッチを
 *  試行し，その結果を自身の内部バッファに保存します．
 *
 * @deprecated 非推奨．後方参照の情報を得たい場合は，tarray_tstring の 
 *             regassign() をご利用ください．
 * @param      pat 文字パターン(正規表現)
 * @return     成功した場合は非負値<br>
 *             失敗した場合は負値
 * @throw      regexルーチンがメモリを使い果たしている場合
 * @throw      内部バッファの確保に失敗した場合
 * @throw      メモリ破壊を起こした場合
 *
 */
ssize_t tstring::regmatch( const char *pat )
{
    if ( this->regex_rec.compile(pat) < 0 ) {
	err_report1(__FUNCTION__,"ERROR","%s",this->regex_rec.cerrstr());
    }
    return this->regmatch((size_t)0, this->regex_rec);
}

/**
 * @brief  正規表現による文字列マッチを試行 (非推奨)
 *
 *  自身の文字列に対し，POSIX 拡張正規表現(以下，正規表現) による文字列マッチを
 *  試行し，その結果を自身の内部バッファに保存します．
 *
 * @deprecated 非推奨．後方参照の情報を得たい場合は，tarray_tstring の 
 *             regassign() をご利用ください．
 * @param      pos 文字列マッチの開始位置
 * @param      pat 文字パターン(正規表現)
 * @return     成功した場合は非負値<br>
 *             失敗した場合は負値
 * @throw      regexルーチンがメモリを使い果たしている場合
 * @throw      内部バッファの確保に失敗した場合
 * @throw      メモリ破壊を起こした場合
 *
 */
ssize_t tstring::regmatch( size_t pos, const char *pat )
{
    if ( this->regex_rec.compile(pat) < 0 ) {
	err_report1(__FUNCTION__,"ERROR","%s",this->regex_rec.cerrstr());
    }
    return this->regmatch(pos, this->regex_rec);
}

/**
 * @brief  正規表現による文字列マッチを試行 (非推奨)
 *
 *  自身の文字列に対し，POSIX 拡張正規表現(以下，正規表現) による文字列マッチを
 *  試行し，その結果を自身の内部バッファに保存します．
 *
 * @deprecated 非推奨．後方参照の情報を得たい場合は，tarray_tstring の 
 *             regassign() をご利用ください．
 * @param      pat 文字パターン(正規表現)
 * @return     成功した場合は非負値<br>
 *             失敗した場合は負値
 * @throw      regexルーチンがメモリを使い果たしている場合
 * @throw      内部バッファの確保に失敗した場合
 * @throw      メモリ破壊を起こした場合
 *
 */
ssize_t tstring::regmatch( const tstring &pat )
{
    if ( this->regex_rec.compile(pat.cstr()) < 0 ) {
	err_report1(__FUNCTION__,"ERROR","%s",this->regex_rec.cerrstr());
    }
    return this->regmatch((size_t)0, this->regex_rec);
}

/**
 * @brief  正規表現による文字列マッチを試行 (非推奨)
 *
 *  自身の文字列に対し，POSIX 拡張正規表現(以下，正規表現) による文字列マッチを
 *  試行し，その結果を自身の内部バッファに保存します．
 *
 * @deprecated 非推奨．後方参照の情報を得たい場合は，tarray_tstring の 
 *             regassign() をご利用ください．
 * @param      pos 文字列マッチの開始位置
 * @param      pat 文字パターン(正規表現)
 * @return     成功した場合は非負値<br>
 *             失敗した場合は負値
 * @throw      regexルーチンがメモリを使い果たしている場合
 * @throw      内部バッファの確保に失敗した場合
 * @throw      メモリ破壊を起こした場合
 *
 */
ssize_t tstring::regmatch( size_t pos, const tstring &pat )
{
    if ( this->regex_rec.compile(pat.cstr()) < 0 ) {
	err_report1(__FUNCTION__,"ERROR","%s",this->regex_rec.cerrstr());
    }
    return this->regmatch(pos, this->regex_rec);
}

/**
 * @brief  正規表現による文字列マッチを試行 (非推奨)
 *
 *  自身の文字列に対し，POSIX 拡張正規表現(以下，正規表現) による文字列マッチを
 *  試行し，その結果を自身の内部バッファに保存します．
 *
 * @deprecated 非推奨．後方参照の情報を得たい場合は，tarray_tstring の 
 *             regassign() をご利用ください．
 * @param      pat tregex クラスのコンパイル済オブジェクト
 * @return     成功した場合は非負値<br>
 *             失敗した場合は負値
 * @throw      regexルーチンがメモリを使い果たしている場合
 * @throw      内部バッファの確保に失敗した場合
 * @throw      メモリ破壊を起こした場合
 *
 */
ssize_t tstring::regmatch( const tregex &pat )
{
    return this->regexp_match_advanced2((size_t)0, pat);
}

/**
 * @brief  正規表現による文字列マッチを試行 (非推奨)
 *
 *  自身の文字列に対し，POSIX 拡張正規表現(以下，正規表現) による文字列マッチを
 *  試行し，その結果を自身の内部バッファに保存します．
 *
 * @deprecated 非推奨．後方参照の情報を得たい場合は，tarray_tstring の 
 *             regassign() をご利用ください．
 * @param      pos 文字列マッチの開始位置
 * @param      pat tregex クラスのコンパイル済オブジェクト
 * @return     成功した場合は非負値<br>
 *             失敗した場合は負値
 * @throw      regexルーチンがメモリを使い果たしている場合
 * @throw      内部バッファの確保に失敗した場合
 * @throw      メモリ破壊を起こした場合
 *
 */
ssize_t tstring::regmatch( size_t pos, const tregex &pat )
{
    ssize_t ret;
    ret = this->regexp_match_advanced2(pos, pat);
    return ret;
}


/**
 * @brief  正規表現マッチ結果の要素数を取得
 *
 *  保存された正規表現マッチの結果の要素数を返します
 * 
 * @return  保存された結果の要素数
 */
size_t tstring::reg_elem_length() const
{
    return this->reg_elem_length_rec;
}


/**
 * @brief  正規表現マッチ結果のマッチ位置を取得
 *
 *  保存された正規表現マッチ結果の，マッチした部分の位置を返します．
 * 
 * @param   idx 0:マッチした文字列全体の情報<br>
 *              1以降:正規表現それぞれにマッチした部分文字列の情報.
 * 
 * @return  保存されたマッチした部分の位置
 */
size_t tstring::reg_pos( size_t idx ) const
{
    if ( idx < this->reg_elem_length_rec )
	return this->_reg_pos_rec[idx];
    else 
	return 0;
}


/**
 * @brief  正規表現マッチ結果のマッチ部分の文字列長を取得
 *
 *  保存された正規表現マッチ結果の，マッチした部分の文字列長を返します．
 * 
 * @param   idx 0:マッチした文字列全体の情報<br>
 *              1以降:正規表現それぞれにマッチした部分文字列の情報.
 * @return  保存されたマッチした部分の文字列長
 */
size_t tstring::reg_length( size_t idx ) const
{
    if ( idx < this->reg_elem_length_rec )
	return this->_reg_length_rec[idx];
    else 
	return 0;
}


/**
 * @brief  正規表現マッチ結果のマッチ部分の文字列を取得
 *
 *  保存された正規表現マッチ結果の，マッチした部分の文字列を返します．
 * 
 * @param   idx 0:マッチした文字列全体の情報<br>
 *              1以降:正規表現それぞれにマッチした部分文字列の情報.
 * @return  保存されたマッチした部分の文字列
 */
const char *tstring::reg_cstr( size_t idx ) const
{
    if ( idx < this->reg_elem_length_rec )
	return this->_reg_cstr_ptr_rec[idx];
    else 
	return NULL;
}


/**
 * @brief  正規表現マッチ結果のマッチ部分の文字列ポインタ配列を取得
 *
 *  保存された正規表現マッチ結果の，マッチした部分の文字列に対する
 *  ポインタ配列を返します．
 * 
 * @return  マッチした部分に対するポインタ配列
 */
const char *const *tstring::reg_cstrarray() const
{
    this->cleanup_shallow_copy(true);    /* バッファを直接操作するため呼ぶ */

    if ( 0 < this->reg_elem_length_rec )
	return this->_reg_cstr_ptr_rec;
    else 
	return NULL;
}


/**
 * @brief  正規表現による置換
 *
 *  自身の文字列に対し，pat で指定されたPOSIX 拡張正規表現(以下，正規表現) で
 *  マッチした部分を文字列 new_str で置き換えます．
 *
 * @param      pat 文字パターン(正規表現)
 * @param      new_str 置換後の文字列
 * @param      all すべて置換する場合は true (省略時は false)
 * @return     成功した場合は，置換された文字列の次の位置のインデックス<br>
 *             失敗した場合は負値
 */
ssize_t tstring::regreplace( const char *pat, const char *new_str, bool all )
{
    ssize_t pos = 0;
    if ( new_str == NULL ) return -1;
    if ( this->regex_rec.compile(pat) < 0 ) {
	err_report1(__FUNCTION__,"ERROR","%s",this->regex_rec.cerrstr());
    }
    return this->regexp_replace(pos, this->regex_rec, new_str, all);
}

/**
 * @brief  正規表現による置換
 *
 *  自身の文字列に対し，pat で指定されたPOSIX 拡張正規表現(以下，正規表現) で
 *  マッチした部分を文字列 new_str で置き換えます．
 *
 * @param      pos 文字列マッチの開始位置
 * @param      pat 文字パターン(正規表現)
 * @param      new_str 置換後の文字列
 * @param      all すべて置換する場合は true (省略時は false)
 * @return     成功した場合は，置換された文字列の次の位置のインデックス<br>
 *             失敗した場合は負値
 */
ssize_t tstring::regreplace( size_t pos, const char *pat, const char *new_str,
			     bool all )
{
    if ( new_str == NULL ) return -1;
    if ( this->regex_rec.compile(pat) < 0 ) {
	err_report1(__FUNCTION__,"ERROR","%s",this->regex_rec.cerrstr());
    }
    return this->regexp_replace(pos, this->regex_rec, new_str, all);
}

/**
 * @brief  正規表現による置換
 *
 *  自身の文字列に対し，pat で指定されたPOSIX 拡張正規表現(以下，正規表現) で
 *  マッチした部分を文字列 new_str で置き換えます．
 *
 * @param      pat 文字パターン(正規表現)
 * @param      new_str 置換後の文字列
 * @param      all すべて置換する場合は true (省略時は false)
 * @return     成功した場合は，置換された文字列の次の位置のインデックス<br>
 *             失敗した場合は負値
 */
ssize_t tstring::regreplace(const tstring &pat, const char *new_str, bool all)
{
    ssize_t pos = 0;
    if ( new_str == NULL ) return -1;
    if ( this->regex_rec.compile(pat.cstr()) < 0 ) {
	err_report1(__FUNCTION__,"ERROR","%s",this->regex_rec.cerrstr());
    }
    return this->regexp_replace(pos, this->regex_rec, new_str, all);
}

/**
 * @brief  正規表現による置換
 *
 *  自身の文字列に対し，pat で指定されたPOSIX 拡張正規表現(以下，正規表現) で
 *  マッチした部分を文字列 new_str で置き換えます．
 *
 * @param      pos 文字列マッチの開始位置
 * @param      pat 文字パターン(正規表現)
 * @param      new_str 置換後の文字列
 * @param      all すべて置換する場合は true (省略時は false)
 * @return     成功した場合は，置換された文字列の次の位置のインデックス<br>
 *             失敗した場合は負値
 */
ssize_t tstring::regreplace( size_t pos, const tstring &pat,
			     const char *new_str, bool all )
{
    if ( new_str == NULL ) return -1;
    if ( this->regex_rec.compile(pat.cstr()) < 0 ) {
	err_report1(__FUNCTION__,"ERROR","%s",this->regex_rec.cerrstr());
    }
    return this->regexp_replace(pos, this->regex_rec, new_str, all);
}

/**
 * @brief  正規表現による置換
 *
 *  自身の文字列に対し，pat で指定されたPOSIX 拡張正規表現(以下，正規表現) で
 *  マッチした部分を文字列 new_str で置き換えます．
 *
 * @param      pat tregex クラスのコンパイル済オブジェクト
 * @param      new_str 置換後の文字列
 * @param      all すべて置換する場合は true (省略時は false)
 * @return     成功した場合は，置換された文字列の次の位置のインデックス<br>
 *             失敗した場合は負値
 */
ssize_t tstring::regreplace( const tregex &pat, const char *new_str, bool all )
{
    ssize_t pos = 0;
    if ( new_str == NULL ) return -1;
    return this->regexp_replace(pos, pat, new_str, all);
}

/**
 * @brief  正規表現による置換
 *
 *  自身の文字列に対し，pat で指定されたPOSIX 拡張正規表現(以下，正規表現) で
 *  マッチした部分を文字列 new_str で置き換えます．
 *
 * @param      pos 文字列マッチの開始位置
 * @param      pat tregex クラスのコンパイル済オブジェクト
 * @param      new_str 置換後の文字列
 * @param      all すべて置換する場合は true (省略時は false)
 * @return     成功した場合は，置換された文字列の次の位置のインデックス<br>
 *             失敗した場合は負値
 */
ssize_t tstring::regreplace( size_t pos, 
			     const tregex &pat, const char *new_str, bool all )
{
    if ( new_str == NULL ) return -1;
    return this->regexp_replace(pos, pat, new_str, all);
}

/**
 * @brief  正規表現による置換
 *
 *  自身の文字列に対し，pat で指定されたPOSIX 拡張正規表現(以下，正規表現) で
 *  マッチした部分を文字列 new_str で置き換えます．
 *
 * @param      pat 文字パターン(正規表現)
 * @param      new_str 置換後の文字列
 * @param      all すべて置換する場合は true (省略時は false)
 * @return     成功した場合は，置換された文字列の次の位置のインデックス<br>
 *             失敗した場合は負値
 */
ssize_t tstring::regreplace(const char *pat, const tstring &new_str, bool all)
{
    return this->regreplace(pat, new_str.cstr(), all);
}

/**
 * @brief  正規表現による置換
 *
 *  自身の文字列に対し，pat で指定されたPOSIX 拡張正規表現(以下，正規表現) で
 *  マッチした部分を文字列 new_str で置き換えます．
 *
 * @param      pos 文字列マッチの開始位置
 * @param      pat 文字パターン(正規表現)
 * @param      new_str 置換後の文字列
 * @param      all すべて置換する場合は true (省略時は false)
 * @return     成功した場合は，置換された文字列の次の位置のインデックス<br>
 *             失敗した場合は負値
 */
ssize_t tstring::regreplace( size_t pos, const char *pat,
			     const tstring &new_str, bool all )
{
    return this->regreplace(pos, pat, new_str.cstr(), all);
}

/**
 * @brief  正規表現による置換
 *
 *  自身の文字列に対し，pat で指定されたPOSIX 拡張正規表現(以下，正規表現) で
 *  マッチした部分を文字列 new_str で置き換えます．
 *
 * @param      pat 文字パターン(正規表現)
 * @param      new_str 置換後の文字列
 * @param      all すべて置換する場合は true (省略時は false)
 * @return     成功した場合は，置換された文字列の次の位置のインデックス<br>
 *             失敗した場合は負値
 */
ssize_t tstring::regreplace( const tstring &pat, const tstring &new_str,
			     bool all)
{
    return this->regreplace(pat, new_str.cstr(), all);
}

/**
 * @brief  正規表現による置換
 *
 *  自身の文字列に対し，pat で指定されたPOSIX 拡張正規表現(以下，正規表現) で
 *  マッチした部分を文字列 new_str で置き換えます．
 *
 * @param      pos 文字列マッチの開始位置
 * @param      pat 文字パターン(正規表現)
 * @param      new_str 置換後の文字列
 * @param      all すべて置換する場合は true (省略時は false)
 * @return     成功した場合は，置換された文字列の次の位置のインデックス<br>
 *             失敗した場合は負値
 */
ssize_t tstring::regreplace( size_t pos, const tstring &pat,
			     const tstring &new_str, bool all )
{
    return this->regreplace(pos, pat, new_str.cstr(), all);
}

/**
 * @brief  正規表現による置換
 *
 *  自身の文字列に対し，pat で指定されたPOSIX 拡張正規表現(以下，正規表現) で
 *  マッチした部分を文字列 new_str で置き換えます．
 *
 * @param      pat tregex クラスのコンパイル済オブジェクト
 * @param      new_str 置換後の文字列
 * @param      all すべて置換する場合は true (省略時は false)
 * @return     成功した場合は，置換された文字列の次の位置のインデックス<br>
 *             失敗した場合は負値
 */
ssize_t tstring::regreplace( const tregex &pat, const tstring &new_str,
			     bool all )
{
    return this->regreplace(pat, new_str.cstr(), all);
}

/**
 * @brief  正規表現による置換
 *
 *  自身の文字列に対し，pat で指定されたPOSIX 拡張正規表現(以下，正規表現) で
 *  マッチした部分を文字列 new_str で置き換えます．
 *
 * @param      pos 文字列マッチの開始位置
 * @param      pat tregex クラスのコンパイル済オブジェクト
 * @param      new_str 置換後の文字列
 * @param      all すべて置換する場合は true (省略時は false)
 * @return     成功した場合は，置換された文字列の次の位置のインデックス<br>
 *             失敗した場合は負値
 */
ssize_t tstring::regreplace( size_t pos, const tregex &pat,
			     const tstring &new_str, bool all )
{
    return this->regreplace(pos, pat, new_str.cstr(), all);
}


static void dump_tstring( const tstring &src )
{
    if ( 0 <= src.strchr('"') ) {
	size_t j;
	sli__eprintf("\"");
	for ( j=0 ; j < src.length() ; j++ ) {
	    int ch = src.cchr(j);
	    if ( ch == '"' ) sli__eprintf("\\\"");
	    else sli__eprintf("%c", ch);
	}
	sli__eprintf("\"");
    }
    else {
	sli__eprintf("\"%s\"", src.cstr());
    }
    return;
}

/**
 * @brief  テンポラリオブジェクトのためのshallow copy属性の付与
 *
 *  自身が「=」演算子または init(obj) の引数に与えられた時の shallow copy を許
 *  可する設定を行ないます．この設定は，関数またはメンバ関数によって返されるテ
 *  ンポラリオブジェクトにのみ付与されるべきものです．<br>
 *
 *  tstring クラスの shallow copy の解除のタイミングは，文字列に対するあらゆる
 *  読み書きであって「書き込み」ではありません．読み取り専用のメンバ関数が使用
 *  された場合も，shallow copy 解除のための deep copy が走ります．したがって，
 *  テンポラリオブジェクト以外の場合に set_scopy_flag() を使っても全く意味が
 *  ありません．
 *
 * @note  固定長バッファモードでは使用不可
 *
 */
/* 注意: 参照を返したいところだが，return 文でそれをやるとインスタンス */
/*       を2回コピーされるというおかしな事になるので void になっている */
void tstring::set_scopy_flag()
{
    if ( this->str_length_rec != UNDEF ) {	/* 固定長の場合は使えない */
	err_throw(__FUNCTION__,"WARNING", "Cannot set flag of shallow copy");
    }
    else {
	this->shallow_copy_ok = true;
    }
    return;
}

/**
 * @brief  オブジェクト情報を標準エラー出力へ出力
 *
 *  自身のオブジェクト情報を標準エラー出力へ出力します．
 *
 */
void tstring::dprint( const char *msg ) const
{
    if ( msg != NULL ) {
	sli__eprintf("%s sli::%s[obj=0x%zx] = ",
		     msg, CLASS_NAME, (const size_t)this);
    }
    else {
	sli__eprintf("sli::%s[obj=0x%zx] = ",CLASS_NAME,(const size_t)this);
    }
    if ( this->cstr() != NULL ) { 
	dump_tstring(*this);
	sli__eprintf("\n");
    } else {
	sli__eprintf("NULL\n");
    }
    return;
}

/*
 *  private member functions
 */

/* realloc and free for this->_str_rec */

/**
 * @brief  _str_rec の確保
 *
 * @note   このメンバ関数はprivateです 
 */
int tstring::realloc_str_rec( size_t len_bytes )
{
    this->cleanup_shallow_copy(true);	/* バッファを直接操作するため呼ぶ */

    size_t len_alloc;

    if ( this->_str_rec == NULL && len_bytes == 0 ) return 0;

    /* 可変長バッファの場合で 1GB 未満の時 */
    if ( this->str_length_rec == UNDEF && 
	 0 < len_bytes && len_bytes < (size_t)1024*1024*1024 ) {
	if ( len_bytes <= 32 ) {		/* 要求が小さい場合 */
	    len_alloc = 32;
	}
	else {					/* 2^n でとるように手配 */
	    const double base = 2.0;
	    size_t nn = (size_t)ceil( log((double)len_bytes) / log(base) );
	    size_t len = (size_t)pow(base, (double)nn);
	    /* 念のためチェック */
	    if ( len < len_bytes ) {
		len = (size_t)pow(base, (double)(nn + 1));
		if ( len < len_bytes ) {
		    err_throw(__FUNCTION__,"FATAL","internal error");
		}
	    }
	    /* */
	    len_alloc = len;
	}
    }
    else {
	len_alloc = len_bytes;
    }

    if ( this->str_alloc_blen_rec != len_alloc ) {

	void *tmp_ptr;

	tmp_ptr = realloc(this->_str_rec, len_alloc);
	if ( tmp_ptr == NULL && 0 < len_alloc ) return -1;
	else {
	    this->_str_rec = (char *)tmp_ptr;
	    this->str_alloc_blen_rec = len_alloc;
	    return 0;
	}

    }
    else {

	return 0;

    }

}

/**
 * @brief  _str_rec の開放
 *
 * @note   このメンバ関数はprivateです 
 */
void tstring::free_str_rec()
{
    this->cleanup_shallow_copy(true);	/* バッファを直接操作するため呼ぶ */

    if ( this->_str_rec != NULL ) {
	free(this->_str_rec);
	this->_str_rec = NULL;
	this->str_alloc_blen_rec = 0;
    }

    return;
}

/* for reg_pos_rec, etc. */

/**
 * @brief  _reg_pos_rec の確保
 *
 * @note   このメンバ関数はprivateです 
 */
int tstring::realloc_reg_pos_rec( size_t len_elements )
{
    this->cleanup_shallow_copy(true);	/* バッファを直接操作するため呼ぶ */

    void *tmp_ptr;

    if ( this->_reg_pos_rec == NULL && len_elements == 0 ) return 0;
    tmp_ptr = realloc(this->_reg_pos_rec,
		      sizeof(*(this->_reg_pos_rec)) * len_elements);
    if ( tmp_ptr == NULL && 0 < len_elements ) return -1;
    else {
	this->_reg_pos_rec = (size_t *)tmp_ptr;
	return 0;
    }
}

/**
 * @brief  _reg_pos_rec の開放
 *
 * @note   このメンバ関数はprivateです 
 */
void tstring::free_reg_pos_rec()
{
    this->cleanup_shallow_copy(true);	/* バッファを直接操作するため呼ぶ */

    if ( this->_reg_pos_rec != NULL ) {
	free(this->_reg_pos_rec);
	this->_reg_pos_rec = NULL;
    }

    return;
}

/**
 * @brief  _reg_length_rec の確保
 *
 * @note   このメンバ関数はprivateです 
 */
int tstring::realloc_reg_length_rec( size_t len_elements )
{
    this->cleanup_shallow_copy(true);	/* バッファを直接操作するため呼ぶ */

    void *tmp_ptr;

    if ( this->_reg_length_rec == NULL && len_elements == 0 ) return 0;
    tmp_ptr = realloc(this->_reg_length_rec,
		      sizeof(*(this->_reg_length_rec)) * len_elements);
    if ( tmp_ptr == NULL && 0 < len_elements ) return -1;
    else {
	this->_reg_length_rec = (size_t *)tmp_ptr;
	return 0;
    }
}

/**
 * @brief  _reg_length_rec の開放
 *
 * @note   このメンバ関数はprivateです 
 */
void tstring::free_reg_length_rec()
{
    this->cleanup_shallow_copy(true);	/* バッファを直接操作するため呼ぶ */

    if ( this->_reg_length_rec != NULL ) {
	free(this->_reg_length_rec);
	this->_reg_length_rec = NULL;
    }

    return;
}

/**
 * @brief  _reg_cstr_ptr_rec の確保
 *
 * @note   このメンバ関数はprivateです 
 */
int tstring::realloc_reg_cstr_ptr_rec( size_t len_elements )
{
    this->cleanup_shallow_copy(true);	/* バッファを直接操作するため呼ぶ */

    void *tmp_ptr;

    if ( this->_reg_cstr_ptr_rec == NULL && len_elements == 0 ) return 0;
    tmp_ptr = realloc(this->_reg_cstr_ptr_rec,
		      sizeof(*(this->_reg_cstr_ptr_rec)) * len_elements);
    if ( tmp_ptr == NULL && 0 < len_elements ) return -1;
    else {
	this->_reg_cstr_ptr_rec = (char **)tmp_ptr;
	return 0;
    }
}

/**
 * @brief  _reg_cstr_ptr_rec の開放
 *
 * @note   このメンバ関数はprivateです 
 */
void tstring::free_reg_cstr_ptr_rec()
{
    this->cleanup_shallow_copy(true);	/* バッファを直接操作するため呼ぶ */

    if ( this->_reg_cstr_ptr_rec != NULL ) {
	free(this->_reg_cstr_ptr_rec);
	this->_reg_cstr_ptr_rec = NULL;
    }

    return;
}

/**
 * @brief  _reg_cstrbuf_rec の確保
 *
 * @note   このメンバ関数はprivateです 
 */
int tstring::realloc_reg_cstrbuf_rec( size_t len_bytes )
{
    this->cleanup_shallow_copy(true);	/* バッファを直接操作するため呼ぶ */

    void *tmp_ptr;

    if ( this->_reg_cstrbuf_rec == NULL && len_bytes == 0 ) return 0;
    tmp_ptr = realloc(this->_reg_cstrbuf_rec, len_bytes);
    if ( tmp_ptr == NULL && 0 < len_bytes ) return -1;
    else {
	this->_reg_cstrbuf_rec = (char *)tmp_ptr;
	return 0;
    }
}

/**
 * @brief  _reg_cstrbuf_rec の開放
 *
 * @note   このメンバ関数はprivateです 
 */
void tstring::free_reg_cstrbuf_rec()
{
    this->cleanup_shallow_copy(true);	/* バッファを直接操作するため呼ぶ */

    if ( this->_reg_cstrbuf_rec != NULL ) {
	free(this->_reg_cstrbuf_rec);
	this->_reg_cstrbuf_rec = NULL;
    }

    return;
}

/**
 * @brief  アドレスが自身のデータ領域内か調べる
 *
 *  ptrのアドレスが自身のデータ領域内かをチェックします．
 * 
 * @param   ptr チェックするアドレス
 * @return  自身の領域内の時は真<br>
 *          それ以外の時は偽
 * @note    このメンバ関数はprivateです 
 */
bool tstring::is_my_buffer( const char *ptr ) const
{
    const char *this_str_ptr = this->str_ptr_cs();

    if ( ptr == NULL || this_str_ptr == NULL ) return false;
    if ( this_str_ptr <= ptr && ptr < this_str_ptr + this->str_buf_length_rec )
	return true;
    else
	return false;
}


/**
 * @brief  正規表現によるマッチを試行(private)
 *
 *  自身の文字列に対し，POSIX 拡張正規表現による文字列マッチを試行し，
 *  その結果を返します．
 *
 * @param   pos 文字列マッチの開始位置
 * @param   regex_ref tregex クラスのコンパイル済オブジェクト
 * @param   ret_span マッチした文字列の長さ
 * @return  成功した場合はマッチした自身の文字列の位置<br>
 *          失敗した場合は負値
 * @throw   regexルーチンがメモリを使い果たしている場合
 * @throw   内部バッファの確保に失敗した場合
 * @throw   メモリ破壊を起こした場合
 * @note    このメンバ関数はprivateです 
 */
ssize_t tstring::regexp_match( size_t pos, const tregex &regex_ref, 
			       size_t *ret_span ) const
{
    int status;
    ssize_t r_pos;
    size_t r_len;

    if ( this->cstr() == NULL ) {
	if ( ret_span != NULL ) *ret_span = 1;	/* 1 means Error */
	return -1;
    }
    if ( this->length() < pos ) {
	if ( ret_span != NULL ) *ret_span = 1;	/* 1 means Error */
	return -1;
    }

    if ( regex_ref.cregex() == NULL ) {
	if ( ret_span != NULL ) *ret_span = 1;	/* 1 means Error */
	return -1;
    }

    status = c_regsearch(regex_ref.cregex(),
			 this->cstr() + pos, (0 < pos), false, &r_pos, &r_len);
    if ( status != 0 ) {
	if ( c_regfatal(status) ) {
	    err_throw(__FUNCTION__,"FATAL","Out of memory in c_regsearch()");
	}
	if ( ret_span != NULL ) *ret_span = 0;
	return -1;
    }
    else {
	if ( 0 <= r_pos ) {
	    if ( ret_span != NULL ) *ret_span = r_len;
	    return pos + r_pos;
	}
	else {
	    if ( ret_span != NULL ) *ret_span = 0;
	    return -1;
	}
    }
}

/**
 * @brief  正規表現によるマッチを試行(private)
 *
 *  自身の文字列に対し，POSIX 拡張正規表現による文字列マッチを試行し，
 *  後方参照の情報を含む結果を得る事ができます．
 * 
 * @param   pos 文字列マッチの開始位置
 * @param   regex_ref pat tregex クラスのコンパイル済オブジェクト
 * @param   nelem pos_r，len_r のバッファ長
 * @param   pos_r[] pos_r マッチした文字列の位置
 * @param   len_r[] len_r マッチした文字列の長さ
 * @param   nelem_r pos_r，len_r にセットされた値の個数
 * @return  成功した場合はマッチした自身の文字列の位置<br>
 *          失敗した場合は負値
 * @throw   regexルーチンがメモリを使い果たしている場合
 * @throw   内部バッファの確保に失敗した場合
 * @throw   メモリ破壊を起こした場合
 * @note    このメンバ関数はprivateです 
 */
ssize_t tstring::regexp_match_advanced1( size_t pos, const tregex &regex_ref,
					 size_t nelem,
					 size_t pos_r[], size_t len_r[],
					 size_t *nelem_r ) const
{
    int status;
    size_t i, n_result;

    if ( this->cstr() == NULL ) return -1;
    if ( this->length() < pos ) return -1;
    if ( regex_ref.cregex() == NULL ) return -1;
    if ( nelem_r == NULL ) return -1;
    if ( nelem < 1 ) return -1;

    /* */
    status = c_regsearchx(regex_ref.cregex(), this->cstr() + pos, 
			  (0 < pos), false,
			  nelem, pos_r, len_r, &n_result);
    if ( status != 0 ) {
	if ( c_regfatal(status) ) {
	    err_throw(__FUNCTION__,"FATAL","Out of memory in c_regsearchx()");
	}
	*nelem_r = 0;
	return -1;
    }

    for ( i=0 ; i < n_result ; i++ ) {
	pos_r[i] += pos;		/* fix position */
    }
    *nelem_r = n_result;

    if ( 0 < n_result ) return pos_r[0];
    else return -1;
}

/**
 * @brief  正規表現によるマッチを試行(private)
 *
 *  自身の文字列に対し，POSIX 拡張正規表現による文字列マッチを試行し，
 *  その結果を自身の内部バッファに保存します．
 * 
 * @param   pos 文字列マッチの開始位置.
 * @param   regex_ref tregex クラスのコンパイル済オブジェクト
 * @return  成功した場合はマッチした自身の文字列の位置<br>
 *          失敗した場合は負値
 * @throw   regexルーチンがメモリを使い果たしている場合
 * @throw   内部バッファの確保に失敗した場合
 * @throw   メモリ破壊を起こした場合
 * @note    このメンバ関数はprivateです 
 */
ssize_t tstring::regexp_match_advanced2( size_t pos, const tregex &regex_ref )
{
    int status;
    size_t to_append, new_n, n_result, off, i;
    const char *str_ptr;

    this->cleanup_shallow_copy(true);    /* バッファを直接操作するため呼ぶ */

    if ( this->cstr() == NULL ) {
	this->init_reg_results();
	return -1;
    }
    if ( this->length() < pos ) {
	this->init_reg_results();
	return -1;
    }

    if ( regex_ref.cregex() == NULL ) {
	this->init_reg_results();
	return -1;
    }

    to_append = 1;
    str_ptr = regex_ref.cstr();
    /* '(' の個数を数え，それを追加分にする．'\\(' もカウントされるがOK */
    if ( str_ptr != NULL ) {
	for ( i=0 ; str_ptr[i] != '\0' ; i++ ) {
	    if ( str_ptr[i] == '(' ) to_append++;
	}
    }

    this->reg_elem_length_rec = 1;
    do {
	this->reg_elem_length_rec += to_append;
	/* */
	new_n = this->reg_elem_length_rec;
	if ( this->realloc_reg_pos_rec(new_n) < 0 ) {
	    this->init_reg_results();
	    err_throw(__FUNCTION__,"FATAL","realloc() failed");
	}
	for ( i=0 ; i < new_n ; i++ ) this->_reg_pos_rec[i] = 0;

	new_n = this->reg_elem_length_rec;
	if ( this->realloc_reg_length_rec(new_n) < 0 ) {
	    this->init_reg_results();
	    err_throw(__FUNCTION__,"FATAL","realloc() failed");
	}
	for ( i=0 ; i < new_n ; i++ ) this->_reg_length_rec[i] = 0;
	/* */
	status = c_regsearchx(regex_ref.cregex(), this->cstr() + pos, 
			      (0 < pos), false, 
			      this->reg_elem_length_rec, this->_reg_pos_rec,
			      this->_reg_length_rec, &n_result);
	if ( status != 0 ) {
	   this->init_reg_results();
	   if ( c_regfatal(status) ) {
	     err_throw(__FUNCTION__,"FATAL","Out of memory in c_regsearchx()");
	   }
	   return -1;
	}
	if ( n_result == 0 ) {	/* never */
	    this->init_reg_results();
	    return -1;
	}
    } while ( this->reg_elem_length_rec <= n_result );

    this->reg_elem_length_rec = n_result;

    new_n = this->reg_elem_length_rec + 1;
    if ( this->realloc_reg_cstr_ptr_rec(new_n) < 0 ) {
	this->init_reg_results();
	err_throw(__FUNCTION__,"FATAL","realloc() failed");
    }
    for ( i=0 ; i < new_n ; i++ ) this->_reg_cstr_ptr_rec[i] = NULL;

    new_n = 0;
    for ( i=0 ; i < n_result ; i++ ) {
	new_n += this->_reg_length_rec[i] + 1;	/* string + '\0' */
	this->_reg_pos_rec[i] += pos;		/* fix position */
    }

    this->reg_cstrbuf_length_rec = new_n;

    if ( this->realloc_reg_cstrbuf_rec(this->reg_cstrbuf_length_rec) < 0 ) {
	this->init_reg_results();
	err_throw(__FUNCTION__,"FATAL","realloc() failed");
    }

    off = 0;
    for ( i=0 ; i < n_result ; i++ ) {
	c_memcpy(this->_reg_cstrbuf_rec + off, 
		 this->cstr() + this->_reg_pos_rec[i], this->_reg_length_rec[i]);
	this->_reg_cstrbuf_rec[off + this->_reg_length_rec[i]] = '\0';
	this->_reg_cstr_ptr_rec[i] = this->_reg_cstrbuf_rec + off;
	off += this->_reg_length_rec[i] + 1;
    }

    if ( 0 < n_result ) return this->_reg_pos_rec[0];
    else return -1;
}

/**
 * @brief  正規表現の結果の格納先を初期化(private)
 *
 * @note   このメンバ関数はprivateです 
 */
void tstring::init_reg_results()
{
    this->cleanup_shallow_copy(true);	/* バッファを直接操作するため呼ぶ */

    this->reg_elem_length_rec = 0;
    this->free_reg_pos_rec();
    this->free_reg_length_rec();
    this->free_reg_cstr_ptr_rec();
    this->reg_cstrbuf_length_rec = 0;
    this->free_reg_cstrbuf_rec();

    return;
}

/**
 * @brief  正規表現による置換(private)
 *
 *  自身の文字列に対し，pat で指定されたPOSIX 拡張正規表現で
 *  マッチした部分を文字列 new_str で置き換えます．
 * 
 * @param   pos 文字列マッチの開始位置
 * @param   pat tregex クラスのコンパイル済オブジェクト
 * @param   new_str 置換後の文字列
 * @param   all すべて置換する場合は true (省略時は false)
 * @return  成功した場合は，置換された文字列の次の位置のインデックス<br>
 *          失敗した場合は負値
 * @throw   regexルーチンがメモリを使い果たしている場合
 * @throw   内部バッファの確保に失敗した場合
 * @throw   メモリ破壊を起こした場合
 * @note    このメンバ関数はprivateです 
 */
ssize_t tstring::regexp_replace( size_t pos, const tregex &pat,
				 const char *new_str, bool all )
{
    ssize_t pret = -1;
    bool has_reference = false;
    size_t i;
    if ( new_str == NULL ) return -1;

#if 1
    /* "\\\\" にも対応しないといけないので，これは常に true */
    has_reference = true;
#else
    /* まず，new_str が後方参照を含んでいるか調べる */
    for ( i=0 ; new_str[i] != '\0' ; i++ ) {
	if ( new_str[i] == '\\' ) {
	    if ( '0' <= new_str[i+1] && new_str[i+1] <= '9' ) {
		has_reference = true;
		break;
	    }
	    else {
		if ( new_str[i+1] != '\0' ) i++;
	    }
	}
    }
#endif

    /* バックスラッシュ + [1-9] による後方参照がある場合 */
    if ( has_reference == true ) {
	while ( 0 <= this->regexp_match_advanced2(pos, pat) ) {
	    /* スキャンしながら new_str をもとに新しいのんを作る  */
	    tstring new_one;
	    size_t prev_idx = 0;
	    pos = this->reg_pos(0);
	    for ( i=0 ; new_str[i] != '\0' ; i++ ) {
		if ( new_str[i] == '\\' ) {
		    if ( '0' <= new_str[i+1] && new_str[i+1] <= '9' ) {
			size_t idx;
			new_one.append(new_str + prev_idx, i - prev_idx);
			idx = new_str[i + 1] - '0' ;
			new_one.append(this->reg_cstr(idx));
			prev_idx = i + 1 + 1;
		    }
		    else {
			new_one.append(new_str + prev_idx, i - prev_idx);
			prev_idx = i + 1;
			if ( new_str[i+1] != '\0' ) {
			    new_one.append((int)(new_str[i+1]), 1);
			    i++;
			    prev_idx ++;
			}
			else {
			    new_one.append((int)(new_str[i]), 1);
			}
		    }
		}
	    }
	    new_one.append(new_str + prev_idx, i - prev_idx);
	    this->replace(pos, this->reg_length(0), new_one.cstr());
	    pos += new_one.length();
	    pret = pos;
	    if ( all == false ) break;
	}
    }
    /* 後方参照が無い場合(ここは使われない) */
    else {
	size_t ret_span, new_len;
	ssize_t spos = pos;
	new_len = c_strlen(new_str);
	while ( 0 <= (spos=this->regexp_match(spos, pat, &ret_span)) ) {
	    this->replace(spos, ret_span, new_str);
	    spos += new_len;
	    pret = spos;
	    if ( all == false ) break;
	}
    }

    return pret;
}

/* Dummy member functions */
#ifndef TSTRING__USE_SOLO_NARG

/**
 * @brief  コンパイル時にエラーを出力させるためのダミーのメンバ関数
 * @deprecated  使用不可．
 *  
 */
void tstring::assign( int ch )
{
    sli__eprintf("[FATAL ERROR] Do not use tstring::assign( int ch ) !!\n");
    abort();
    return;
}

/**
 * @brief  コンパイル時にエラーを出力させるためのダミーのメンバ関数
 * @deprecated  使用不可．
 *  
 */
void tstring::put( size_t pos, int ch )
{
    sli__eprintf(
	  "[FATAL ERROR] Do not use tstring::put( size_t pos, int ch ) !!\n");
    abort();
    return;
}

/**
 * @brief  コンパイル時にエラーを出力させるためのダミーのメンバ関数
 * @deprecated  使用不可．
 *  
 */
void tstring::append( int ch )
{
    sli__eprintf("[FATAL ERROR] Do not use tstring::append( int ch ) !!\n");
    abort();
    return;
}

/**
 * @brief  コンパイル時にエラーを出力させるためのダミーのメンバ関数
 * @deprecated  使用不可．
 *  
 */
void tstring::insert( size_t pos, int ch )
{
    sli__eprintf(
       "[FATAL ERROR] Do not use tstring::insert( size_t pos, int ch ) !!\n");
    abort();
    return;
}

/**
 * @brief  コンパイル時にエラーを出力させるためのダミーのメンバ関数
 * @deprecated  使用不可．
 *  
 */
void tstring::replace( size_t pos, size_t n, int ch )
{
    sli__eprintf("[FATAL ERROR] Do not use "
		  "tstring::replace( size_t pos, size_t n, int ch ) !!\n");
    abort();
    return;
}
#endif

}	/* namespace sli */


#include "private/c_memcpy.cc"
#include "private/c_memset.cc"
#include "private/c_memmove.cc"
#include "private/c_strcmp.cc"
#include "private/c_strncmp.cc"
#include "private/c_strlen.cc"

#include "private/c_vsnprintf.c"
#include "private/c_vsscanf.c"
#include "private/c_vasprintf.c"

#include "private/c_isalpha.cc"
#include "private/c_isalnum.cc"
#include "private/c_strtox.c"
#include "private/c_tolower.cc"
#include "private/c_toupper.cc"

#include "private/c_strmatch.c"
#include "private/c_regsearch.c"
#include "private/c_regsearchx.c"
#include "private/c_regfatal.c"
