/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2013-05-14 18:04:31 cyamauch> */

/**
 * @file   tstring.cc
 * @brief  ʸ����򰷤�����Υ��饹 tstring �Υ�����
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

/* shallow copy �ط��� debug �� */
//#define debug_report_scopy(arg) err_report(__FUNCTION__,"DEBUG",arg)
#define debug_report_scopy(arg)
//#define debug_report_scopy1(a1,a2) err_report1(__FUNCTION__,"DEBUG",a1,a2)
#define debug_report_scopy1(a1,a2)

/* ����ɽ���θ����λ�����������å����Ȥ� */
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

    /* nn ʸ����Ӥ��� */
    while ( i < nn ) {
        c1 = s1[i];
        c2 = s2[i];
        if ( c1 != c2 ) break;
        i++;
    }
    if ( n1 == n2 ) {   /* n1, n2 ��Ʊ���ξ�� */
        return c1-c2;
    }
    else {              /* n1, n2 ���ۤʤ��� */
        int d = c1-c2;
        if ( d == 0 ) {
            /* nn �ޤǤΥХ��ʥ꤬���פ��Ƥ��Ƥ⡤n1,n2 �� */
            /* Ĺ�����㤦���ϡ��ۤʤ������𤹤� */
            if ( n1 < n2 ) return -256;
            else if ( n2 < n1 ) return 256;
            else return 0;      /* �������ˤ���ʤ� */
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

    /* n ���礭�������硤����оݤ� nn ʸ�������¤��� */
    if ( n1 < nn ) nn = n1;
    if ( n2 < nn ) nn = n2;

    /* nn ʸ����Ӥ��� */
    while ( i < nn ) {
        c1 = s1[i];
        c2 = s2[i];
        if ( c1 != c2 ) break;
        i++;
    }
    if ( nn == n ) {    /* nn ����ᤫ���Ѳ��ʤ��ξ�� */
        return c1-c2;
    }
    else {              /* n ����� n1,n2 ���������� nn ���������줿��� */
        int d = c1-c2;
        if ( d == 0 ) {
            /* nn �ޤǤΥХ��ʥ꤬���פ��Ƥ��Ƥ⡤n1,n2 �� */
            /* Ĺ�����㤦���ϡ��ۤʤ������𤹤� */
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

    /* nn ʸ����Ӥ��� */
    while ( i < nn ) {
        c1 = c_tolower(s1[i]);
        c2 = c_tolower(s2[i]);
        if ( c1 != c2 ) break;
        i++;
    }
    if ( n1 == n2 ) {   /* n1, n2 ��Ʊ���ξ�� */
        return c1-c2;
    }
    else {              /* n1, n2 ���ۤʤ��� */
        int d = c1-c2;
        if ( d == 0 ) {
            /* nn �ޤǤΥХ��ʥ꤬���פ��Ƥ��Ƥ⡤n1,n2 �� */
            /* Ĺ�����㤦���ϡ��ۤʤ������𤹤� */
            if ( n1 < n2 ) return -256;
            else if ( n2 < n1 ) return 256;
            else return 0;      /* �������ˤ���ʤ� */
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

    /* n ���礭�������硤����оݤ� nn ʸ�������¤��� */
    if ( n1 < nn ) nn = n1;
    if ( n2 < nn ) nn = n2;

    /* nn ʸ����Ӥ��� */
    while ( i < nn ) {
        c1 = c_tolower(s1[i]);
        c2 = c_tolower(s2[i]);
        if ( c1 != c2 ) break;
        i++;
    }
    if ( nn == n ) {    /* nn ����ᤫ���Ѳ��ʤ��ξ�� */
        return c1-c2;
    }
    else {              /* n ����� n1,n2 ���������� nn ���������줿��� */
        int d = c1-c2;
        if ( d == 0 ) {
            /* nn �ޤǤΥХ��ʥ꤬���פ��Ƥ��Ƥ⡤n1,n2 �� */
            /* Ĺ�����㤦���ϡ��ۤʤ������𤹤� */
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
 * @brief  �������ơ������ޤ��ϳ�̤˰Ϥޤ줿��ʬ�ξ�������(��¦����ѡ���)
 *
 * @note   len_to_be_parsed ��ɬ���������ͤ򥻥åȤ��뤳�ȡ�<br>
 *         private �ʴؿ��Ǥ���
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
    tstring bkt_to_find;			/* ���Ĥ���٤��ڥ��γ�� */
    size_t depth_bkt = 0;			/* ��̤ο��� */

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
	if ( prev_escape == true ) {		/* ����chr��escapeʸ���ʤ� */
	    prev_escape = false;
	}
	else if ( ch == escape ) {		/* ����chr��escapeʸ���ʤ� */
	    prev_escape = true;
	}
	else if ( quot_to_find != '\0' ) {	/* �ڥ��� quot �򸫤Ĥ��� */
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
	    /* quot �γ��Ϥ򸫤Ĥ��� */
	    for ( j=0 ; quot[j] != '\0' ; j++ ) {
		if ( ch == quot[j] ) {
		    quot_to_find = ch;
		    if ( start_pos < 0 ) start_pos = i;
		    break;
		}
	    }
	    if ( quot[j] == '\0' ) {
		/* �����γ�̤򸫤Ĥ��� */
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
		/* �ڥ��γ�̤򸫤Ĥ��� */
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
	/* �ڥ��γ�̡��������ơ�����󤬸��Ĥ���ʤ��ä���� */
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
 * @brief  �������ơ������ޤ��ϳ�̤˰Ϥޤ줿��ʬ�ξ�������(��¦����ѡ���)
 *
 * @note   len_to_be_parsed ��ɬ���������ͤ򥻥åȤ��뤳�ȡ�<br>
 *         private �ʴؿ��Ǥ���
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
    tstring bkt_to_find;			/* ���Ĥ���٤��ڥ��γ�� */
    size_t depth_bkt = 0;			/* ��̤ο��� */

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
	    if ( 0 < i ) {			/* escapeʸ��������å� */
		size_t j = i;
		while ( 0 < j ) {
		    j --;
		    if ( src_str[j] != escape ) break;
		}
		if ( 1 < (i - j) && ((i - j) % 2) == 0 ) prev_escape = true;
	    }
	    if ( prev_escape == true ) {	/* ����chr��escapeʸ���ʤ� */
		i --;
	    }
	    else if ( quot_to_find != '\0' ) {	/* �ڥ��� quot �򸫤Ĥ��� */
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
		/* quot �γ��Ϥ򸫤Ĥ��� */
		for ( j=0 ; quot[j] != '\0' ; j++ ) {
		    if ( ch == quot[j] ) {
			quot_to_find = ch;
			if ( start_pos < 0 ) start_pos = i;
			break;
		    }
		}
		if ( quot[j] == '\0' ) {
		    /* �����γ�̤򸫤Ĥ��� */
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
		    /* �ڥ��γ�̤򸫤Ĥ��� */
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
	/* �ڥ��γ�̡��������ơ�����󤬸��Ĥ���ʤ��ä���� */
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
 * @brief  ���ֳ�¦�Υ������ơ������ޤ��ϳ�̤�õ�
 *
 *  ��¦���饯�����ơ������ޤ��ϳ�̤˰Ϥޤ줿��ʬ��õ����������ʬʸ�����
 *  ���������ֳ�¦�Υ������ơ������ޤ��ϳ�̤�õ�롥
 *
 * @note  *len_target_str ��ɬ���������ͤ򥻥åȤ��뤳�ȡ�<br>
 *        private �ʴؿ��Ǥ���
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
    tstring bkt_to_find;			/* ���Ĥ���٤��ڥ��γ�� */
    size_t depth_bkt = 0;			/* ��̤ο��� */

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
	    if ( prev_escape == true ) {	/* ����chr��escapeʸ���ʤ� */
		prev_escape = false;
	    }
	    else if ( ch == escape ) {		/* ����chr��escapeʸ���ʤ� */
		prev_escape = true;
	    }
	    else if ( quot_to_find != '\0' ) {	/* �ڥ��� quot �򸫤Ĥ��� */
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
		/* quot �γ��Ϥ򸫤Ĥ��� */
		for ( j=0 ; quot[j] != '\0' ; j++ ) {
		    if ( ch == quot[j] ) {
			quot_to_find = ch;
			if ( start_pos < 0 ) start_pos = i;
			break;
		    }
		}
		if ( quot[j] == '\0' ) {
		    /* �����γ�̤򸫤Ĥ��� */
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
		    /* �ڥ��γ�̤򸫤Ĥ��� */
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
	    /* �ڥ��γ�̡��������ơ�����󤬸��Ĥ���ʤ��ä���� */
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
		    if ( prev_escape == true ) {  /* ����chr��escapeʸ���ʤ� */
			prev_escape = false;
		    }
		    else if ( ch == escape ) {	  /* ����chr��escapeʸ���ʤ� */
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
		    if ( prev_escape == true ) {  /* ����chr��escapeʸ���ʤ� */
			prev_escape = false;
		    }
		    else if ( ch == escape ) {	  /* ����chr��escapeʸ���ʤ� */
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
		if ( prev_escape == true ) {	  /* ����chr��escapeʸ���ʤ� */
		    prev_escape = false;
		}
		else if ( ch == escape ) {	  /* ����chr��escapeʸ���ʤ� */
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
 * ����Ū�ˤϡ����֥��������������ϡ��ҡ��פ��ΰ����ݤ��ʤ���
 * �������äơ����⤷�ʤ��� obj.cstr() �� NULL ���֤롥
 *
 * �����������󥹥ȥ饯���ǰ����� true ��Ϳ����줿��硤
 * this->_str_rec �� NULL �ˤʤ���Ϥʤ���
 * ���ξ��ϡ�ʸ����Ĺ 0 ��ʸ����ǽ��������롥
 * 
 * ���֥������Ȥ������֤��ᤷ�������ϡ�
 * obj = NULL;
 * �Ȥ��롥
 */

/**
 * @brief    constructor �Ѥ����̤ʥ��˥���饤��
 *
 * @param    is_constructor ���󥹥ȥ饯���ˤ�äƸƤӽФ������� true ��
 *                          init()������Ƥ־��� false �򥻥åȤ��롥
 *
 * @note     ���Υ��дؿ��� private �Ǥ���<br>
 *           ����Ĺ�Хåե��ξ�硤�Хåե�Ĺ�ξ��󤬾ä���Τ���ա�
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

	/* ������ .init() �ǤϽ��������ʤ� */
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
 * @brief  ���󥹥ȥ饯��
 *
 *  ���󥹥ȥ饯���Ǥ���<br>
 *  �桼���Υݥ����ѿ��Υ��ɥ쥹�� extptr_address ��Ϳ����ȡ��桼���Υݥ���
 *  ���ѿ������֥������Ȥ���Ͽ���졤���֥������Ȥ�����ʸ�������Ƭ���ɥ쥹��
 *  ��˥桼���Υݥ����ѿ����ݻ������Ƥ������Ȥ��Ǥ��ޤ���
 * 
 * @param   extptr_address �桼���Υݥ����ѿ��Υ��ɥ쥹 (��ά��)
 * @attention  �桼���Υݥ����ѿ�����Ͽ����� shallow copy ����ǽ���ʤ��ʤ�
 *             �ޤ���
 */
tstring::tstring(char **extptr_address)
{
    this->__force_init(true);

    this->extptr_rec = extptr_address;

    if ( this->extptr_rec != NULL ) *(this->extptr_rec) = this->_str_rec;

    return;
}

/**
 * @brief  ���󥹥ȥ饯��
 *
 *  ���󥹥ȥ饯���Ǥ���
 *  buffer_keeps_non_null��ture�ξ�硤NULL̵���⡼�ɤǽ�������ޤ���<br>
 *  �桼���Υݥ����ѿ��Υ��ɥ쥹�� extptr_address ��Ϳ����ȡ��桼���Υݥ���
 *  ���ѿ������֥������Ȥ���Ͽ���졤���֥������Ȥ�����ʸ�������Ƭ���ɥ쥹��
 *  ��˥桼���Υݥ����ѿ����ݻ������Ƥ������Ȥ��Ǥ��ޤ���
 * 
 * @param  buffer_keeps_non_null NULL̵���⡼�ɤˤ��뤫�ɤ����Υե饰
 * @param  extptr_address �桼���Υݥ����ѿ��Υ��ɥ쥹 (��ά��)
 * @attention  �桼���Υݥ����ѿ�����Ͽ����� shallow copy ����ǽ���ʤ��ʤ�
 *             �ޤ���
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
 * ���Υ��󥹥ȥ饯���ϡ�����Ĺ�Хåե�(��®ư��)�⡼�ɤ�Ȥ��������˻Ȥ���
 * ����ǽ��������ȡ��Хåե����礭�����Ѥ����ʤ�����printf()�ʤɤΥ�
 * ��дؿ��ϥҡ��פκƳ��ݤ�Ǿ��¤������ʤ��ʤ�Τǡ���®ư����Ԥ�
 * ���롥
 */

/**
 * @brief  ���󥹥ȥ饯��
 *
 *  ���󥹥ȥ饯���Ǥ�������Ĺ�Хåե��⡼�ɤǽ������Ԥ��ޤ���<br>
 *  �桼���Υݥ����ѿ��Υ��ɥ쥹�� extptr_address ��Ϳ����ȡ��桼���Υݥ���
 *  ���ѿ������֥������Ȥ���Ͽ���졤���֥������Ȥ�����ʸ�������Ƭ���ɥ쥹��
 *  ��˥桼���Υݥ����ѿ����ݻ������Ƥ������Ȥ��Ǥ��ޤ���
 * 
 * @param  max_length ���Ȥΰ���ʸ����
 * @param  extptr_address �桼���Υݥ����ѿ��Υ��ɥ쥹 (��ά��)
 * @attention  �桼���Υݥ����ѿ�����Ͽ����� shallow copy ����ǽ���ʤ��ʤ�
 *             �ޤ���
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
 * @brief  ���󥹥ȥ饯��
 *
 *  ���󥹥ȥ饯���Ǥ�������Ĺ�Хåե��⡼�ɤǽ������Ԥ��ޤ���<br>
 *  �桼���Υݥ����ѿ��Υ��ɥ쥹�� extptr_address ��Ϳ����ȡ��桼���Υݥ���
 *  ���ѿ������֥������Ȥ���Ͽ���졤���֥������Ȥ�����ʸ�������Ƭ���ɥ쥹��
 *  ��˥桼���Υݥ����ѿ����ݻ������Ƥ������Ȥ��Ǥ��ޤ���
 * 
 * @param  max_length ���Ȥΰ���ʸ����
 * @param  extptr_address �桼���Υݥ����ѿ��Υ��ɥ쥹 (��ά��)
 * @attention  �桼���Υݥ����ѿ�����Ͽ����� shallow copy ����ǽ���ʤ��ʤ�
 *             �ޤ���
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
 * @brief  ���ԡ����󥹥ȥ饯��
 *
 *  ���ԡ����󥹥ȥ饯���Ǥ������Ȥ� obj �����Ƥǽ�������ޤ���
 * 
 * @param  obj ���ԡ����Υ��֥�����
 */
tstring::tstring(const tstring &obj)
{
    this->__force_init(true);

    this->init(obj);

    return;
}

/**
 * @brief  ���ԡ����󥹥ȥ饯��
 *
 *  ���ԡ����󥹥ȥ饯���Ǥ������Ȥ���������ʸ���� str ���������ޤ���
 * 
 * @param  str �����Ȥʤ�ʸ����
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
 * @brief  destructor �Ѥ����̤ʥ��곫���ѥ��дؿ�
 *
 * @note   ���Υ��дؿ��� private �Ǥ���
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
 * @brief  �ǥ��ȥ饯��
 *
 */
tstring::~tstring()
{
    /* shallow copy ���줿�塤�����˾��Ǥ����� (copy �� src ¦�ν���) */
    if ( this->shallow_copy_dest_obj != NULL ) {
	/* ������¦�����Ǥ����������Τ��� */
	this->shallow_copy_dest_obj->shallow_copy_src_obj = NULL;
	/* �Хåե��ϳ������ƤϤ����ʤ� */
    	debug_report_scopy("destructor not free() [0]");
	return;
    }
    /* shallow copy �� src ���ޤ������Ƥ����� (copy �� dest ¦�ν���) */
    else if ( this->shallow_copy_src_obj != NULL ) {
	/* shallow copy �򥭥�󥻥뤹�� */
	this->shallow_copy_src_obj->cancel_shallow_copy(this);
	/* �Хåե��ϳ������ƤϤ����ʤ� */
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
 * @brief  shallow copy ����ǽ�������
 * 
 *  src¦�Ǽ¹Ԥ��롥<br>
 *  SLLIB�μ����Ǥϡ�shallow copy ����������ȤΤ߲ġ�
 * 
 * @param   from_obj �ꥯ�����Ȥ���������¦�Υ��֥������ȤΥ��ɥ쥹
 * @return  shallow copy����ǽ�ʤ鿿<br>
 *          ����ʳ��λ��ϵ�
 * @note ���Υ��дؿ��� private �Ǥ�
 */
bool tstring::request_shallow_copy( tstring *from_obj ) const
{
    if ( this->shallow_copy_ok == true ) {
	/* �����Ȥ� shallow copy ��̵�ط��Ǥ����������å� */
	if ( this->shallow_copy_dest_obj == NULL &&
	     this->shallow_copy_src_obj == NULL &&
	     from_obj->shallow_copy_dest_obj == NULL &&
	     from_obj->shallow_copy_src_obj == NULL ) {
	    /* ξ�Ԥδط�����Ͽ */
	    tstring *thisp = (tstring *)this;
	    /* ���򼫿ȤΥ����ѿ��˥ޡ��� */
	    /* (��꤬���Ǥޤ��� deep copy �������NULL�ˤʤ�) */
	    thisp->shallow_copy_dest_obj = from_obj;
	    /* ���Ȥ����Υ����ѿ��˥ޡ��� */
	    /* (���Ȥ����Ǥ�����NULL�ˤʤ롥See �ǥ��ȥ饯��) */
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
 * @brief  shallow copy �򥭥�󥻥�
 *
 *  src¦�Ǽ¹Ԥ��롥
 * 
 * @param  from_obj �ꥯ�����Ȥ���������¦�Υ��֥������ȤΥ��ɥ쥹
 * @note   from_obj ����Ƥ��ľ��� from_obj �ˤ� __force_init() ��¹Ԥ���
 *         ɬ�פ�����ޤ���<br>
 *         ���Υ��дؿ��� private �Ǥ�
 */
void tstring::cancel_shallow_copy( tstring *from_obj ) const
{
    debug_report_scopy1("arg: from_obj = %zx",(size_t)from_obj);

    if ( this->shallow_copy_dest_obj == from_obj ) {
	tstring *thisp = (tstring *)this;
	/* ���¦��NULL�� */
	from_obj->shallow_copy_src_obj = NULL;
	/* ���ȤΤ�NULL�� */
	thisp->shallow_copy_dest_obj = NULL;
    }
    else {
 	err_throw(__FUNCTION__,"FATAL","internal error");
    }

    return;
}

/**
 * @brief  ���ȤˤĤ��ơ�shallow copy �ط��ˤĤ��ƤΥ��꡼�󥢥åפ�Ԥʤ�
 *
 * @param  do_deep_copy_for_this shallow copy �Υ���󥻥��ˡ����ȤˤĤ���
 *                               deep copy ��Ԥʤ����� true �򥻥åȤ��롥
 * @note   ���ξ��˸ƤӽФ�ɬ�פ����롥<br>
 *          1. �Хåե��˽񤭹�����<br>
 *          2. �Хåե��Υ��ɥ쥹���֤����<br>
 *          3. __shallow_init(), __deep_init() �ƤӽФ���ľ��<br>
 *         ���Υ��дؿ��� private �Ǥ���
 */
void tstring::cleanup_shallow_copy( bool do_deep_copy_for_this ) const
{
    //debug_report_scopy1("arg: do_deep_copy_for_this = %d",
    //			(int)do_deep_copy_for_this);

    tstring *thisp = (tstring *)this;

    /* ǰ�Τ���... */
    if ( this->__copying == true ) {
	err_report(__FUNCTION__,"WARNING","This should not be reported");
    }

    /* shallow copy �� src ���ޤ������Ƥ�����(dest ¦�ν���)��*/
    /* shallow copy �Υ���󥻥������Ԥʤ� */
    if ( thisp->shallow_copy_src_obj != NULL ) {

	const tstring &_src_obj = *(thisp->shallow_copy_src_obj);
	tstring *_dest_obj = thisp;

	/* shallow copy �򥭥�󥻥뤷����������� */
	_src_obj.cancel_shallow_copy(_dest_obj);
	_dest_obj->__force_init(false);

	if ( do_deep_copy_for_this == true ) {
	    /* deep copy ��¹� */
	    _dest_obj->__deep_init(_src_obj);
	}
    }
    /* dest_obj �� shallow copy �� src �ˤʤäƤ�����硤�ڤ�Ϥʤ� */
    else if ( thisp->shallow_copy_dest_obj != NULL ) {

	const tstring &_src_obj = *thisp;
	tstring *_dest_obj = thisp->shallow_copy_dest_obj;

	/* shallow copy �򥭥�󥻥뤷����������� */
	_src_obj.cancel_shallow_copy(_dest_obj);
	_dest_obj->__force_init(false);

	/* deep copy ��¹� */
	_dest_obj->__deep_init(_src_obj);
    }

    return;
}

/**
 * @brief  obj �����Ƥ򼫿Ȥ˥��ԡ� (shallow copy; Ķ���٥�)
 *
 *  shallow copy ��Ԥʤ���礫���ǡ����ΰ�ư��Ԥʤ��������Ѥ��롥
 *
 * @param  obj ���ԡ������֥������ȤΥ��ɥ쥹
 * @param  is_move �ǡ�������ƥ�ĤΡְ�ư�פξ�硤true �򥻥å�
 * @note   �����ѿ������˽���������ԡ����롥�Хåե��ϥ��ɥ쥹���������ԡ�
 *         ����롥������ this->shallow_copy_src_obj �����Ͼõ��ʤ���
 *         �Хåե������Ƴ�������ñ��˥Хåե��� share ���Ƥ��ޤ��Τǡ����Ѥ�
 *         ����դ�ɬ�ס�<br>
 *         ���Υ��дؿ���Ȥ����� shallow copy �ط��򥯥꡼��ˤ��Ƥ�����<br>
 *         ���Υ��дؿ��� private �Ǥ���
 */
tstring &tstring::__shallow_init( const tstring &obj, bool is_move )
{
    /*
     *  �����Ǥϡ�this �Ǥ� obj �Ǥ� cleanup_shallow_copy() ��ȤäƤ�����
     *  ��ƤФʤ��褦��ա�
     */

    debug_report_scopy1("arg: obj = %zx",(size_t)(&obj));

    tstring *const obj_bak = this->shallow_copy_src_obj;
    
    if ( this->str_length_rec != UNDEF ) {	/* ����Ĺ�ξ��ϻȤ��ʤ� */
	err_throw(__FUNCTION__,"FATAL",
		  "Internal error: cannot perform shallow copy [0]");
    }
    if ( this->str_rec_keeps_non_null == true ) {
	/* shallow copy �ξ�硥NULL̵���⡼�ɤ���꤬NULL���ȥ���Ǥ��� */
	if ( is_move == false && obj._str_rec == NULL ) {
	    err_throw(__FUNCTION__,"FATAL",
		      "Internal error: cannot perform shallow copy [1]");
	}
    }
    if ( is_move == false ) {
	/* ������ shallow copy �ξ�硥����ptr�λ��Ѥ϶ػߤ���� */
	if ( this->extptr_rec != NULL ) {
	    err_throw(__FUNCTION__,"FATAL",
		      "Internal error: cannot perform shallow copy [2]");
	}
    }

    /* ��������� */
    this->__force_free();
    this->__force_init(false);

    tstring *objp = (tstring *)(&obj);
    objp->__copying = true;
    this->__copying = true;

    try {

	/* ���ɥ쥹�������ԡ� */
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

	    /* ������ʬ�ϡ��ǡ����ιԤ��褬���ꤷ�Ƥ���ְ�ư�פξ�礷�� */
	    /* ������ʤ�                                                 */

	    /* NULL̵���⡼�ɤξ������� */
	    if ( this->str_rec_keeps_non_null == true ) {
		if ( this->_str_rec == NULL ) this->__init_non_null_str_rec();
	    }

	    /* �����ݥ��󥿤ι��� */
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
 * @brief  obj �����Ƥ򼫿Ȥ˥��ԡ� (deep copy; Ķ���٥�)
 *
 * @param  obj ���ԡ������֥������ȤΥ��ɥ쥹
 * @note   obj ¦�ˤĤ��Ƥϡ�_defalut_rec �ʤɤ�ľ�ܤߤˤ����Τ���ա�<br>
 *         ���Υ��дؿ���Ȥ�����shallow copy�ط��򥯥꡼��ˤ��Ƥ�������<br>
 *         ���Υ��дؿ��� private �Ǥ���
 */
tstring &tstring::__deep_init( const tstring &obj )
{
    /*
     *  �����Ǥϡ�obj ¦�� cleanup_shallow_copy() ��ȤäƤ�����
     *  ��ƤФʤ��褦��ա�
     */

    debug_report_scopy1("arg: obj = %zx",(size_t)(&obj));

    tstring *objp = (tstring *)(&obj);
    objp->__copying = true;

    try {

	/*
	 * Note: obj �Ϥɤ�ư��⡼�ɤ����Ƥ��ɤ��褦�ˤ��Ƥ���
	 */
	if ( this->str_length_rec != UNDEF ) {		/* ����Ĺ�ξ�� */

	    if ( this->str_buf_length_rec < obj.str_buf_length_rec ) {
		/* �Хåե�Ĺ��­��ʤ���� */
		c_memcpy(this->_str_rec, obj._str_rec, 
			 this->str_buf_length_rec);
		if ( 0 < this->str_buf_length_rec ) {
		    /* ­��ʤ��Τ� \0 ���Ǥ� */
		    this->_str_rec[this->str_buf_length_rec - 1] = '\0';
		    /* �����٤��� obj.length() �Ǥ��� */
		    if ( obj.length() < this->str_buf_length_rec ) 
			this->str_length_rec = obj.length();
		    else this->str_length_rec = this->str_buf_length_rec - 1;
		}
		else this->str_length_rec = 0;	/* ����Ϥ��ꤨ�ʤ����� */
	    }
	    else {
		/* �ԥå���ޤ���;���� */
		c_memcpy(this->_str_rec, obj._str_rec, 
			 obj.str_buf_length_rec);
		c_memset(this->_str_rec + obj.str_buf_length_rec, 0, 
			 this->str_buf_length_rec - obj.str_buf_length_rec);
		this->str_length_rec = obj.length();
	    }

	}
	else {						/* �̾�⡼�ɤξ�� */

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
		/* ʸ����Ĺ�����������ϸ��ߤΥХåե��򤽤Τޤ޻Ȥ� */
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
 * @brief  Ĺ�������ʸ����򥪥֥������Ȥ˳�Ǽ (NULL̵���⡼������)
 *
 * @note       ���Υ��дؿ���private�Ǥ� 
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
 * @brief  Ĺ�������ʸ����򥪥֥������Ȥ˳�Ǽ (����Ĺ�Хåե��⡼������)
 *
 *  Ĺ�������ʸ����򥪥֥������Ȥ˳�Ǽ���� (����Ĺ�Хåե��⡼������) <br>
 *  ���� max_length �� constructor �ǻ��ꤷ������¾�ξ��� 0 ��Ϳ���롥
 *
 * @max_length  �����ʸ����Ĺ(constructor�λ��˻��ꤹ��) <br>
 *              0 �ξ�硤���ߤ��������Ѥ�����ʸ���������������롥
 * @note        ���Υ��дؿ���private�Ǥ� 
 */
void tstring::__init_fixed_str_rec( size_t max_length )
{
    if ( 0 < max_length ) {

	/* realloc_str_rec() ����ǻȤ��Τǡ�����ͤ�����Ƥ��� */
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
 * @brief  ���֥������ȤΥ��ԡ�
 *
 *  �黻�Ҥα�¦(����) �ǻ��ꤷ�����֥������ȡ�ʸ����򼫿Ȥ������򤷤ޤ�.
 *
 * @param      obj tstring���饹�Υ��֥�������
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * @throw      �����˲��򵯤��������
 *
 */
tstring &tstring::operator=(const tstring &obj)
{
    this->init(obj);
    return *this;
}

/**
 * @brief  ʸ������ɲ�
 * 
 *  ���Ȥ�ʸ����ˡ��黻�Ҥα�¦(����) �ǻ��ꤷ��ʸ������ɲä�Ԥ��ޤ�.
 *
 * @param      obj tstring���饹�Υ��֥�������
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
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
 * @brief  ʸ��������
 *
 *  ���Ȥ�ʸ����ȡ��黻�Ҥα�¦(����) �ǻ��ꤷ��ʸ����
 *  ���פ��Ƥ��뤫�ɤ�������Ӥ�Ԥ������η�̤��֤��ޤ���
 *
 * @param      obj tstring���饹�Υ��֥�������
 * @return     ʸ���󤬰��פ�������true<br>
 *             ʸ�����԰��פǤ������false
 * 
 */
bool tstring::operator==(const tstring &obj) const
{
    if ( this->compare(obj,0) == 0 ) return true;
    else return false;
}

/**
 * @brief  ʸ��������
 *
 *  ���Ȥ�ʸ����ȡ��黻�Ҥα�¦(����) �ǻ��ꤷ��ʸ����<br>
 *  �԰��פ��ɤ�������Ӥ�Ԥ������η�̤��֤��ޤ���
 *
 * @param      obj tstring���饹�Υ��֥�������
 * @return     ʸ�����԰��פǤ������true<br>
 *             ʸ���󤬰��פ�������false
 * 
 */
bool tstring::operator!=(const tstring &obj) const
{
    if ( this->compare(obj,0) == 0 ) return false;
    else return true;
}

/* This is required by STL map */
/**
 * @brief  ʸ����μ���Ū�����
 *
 *  ���Ȥ�ʸ�����,���դΤǻ��ꤷ��ʸ����Ȥ򼭽�Ū����Ӥ���
 *  "���� < ����" ����Ω������� true ���֤��ޤ���
 * 
 * @param   obj tstring���饹�Υ��֥�������
 * @return  ������
 * @note    STL �� map �˺ܤ��뤿���ɬ�ס�
 */
bool tstring::operator<(const tstring &obj) const
{
    return ( this->compare(obj,0) < 0 );
}

/**
 * @brief  ʸ���������
 *
 *  �黻�Ҥα�¦(����) �ǻ��ꤷ�����֥������ȡ�ʸ����򼫿Ȥ������򤷤ޤ�.
 *
 * @param      str ʸ����Υ��ɥ쥹
 * @return     �����Хåե��Υ��ɥ쥹
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
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
 * @brief  ʸ������ɲ�
 *
 *  ���Ȥ�ʸ����ˡ��黻�Ҥα�¦(����) �ǻ��ꤷ��ʸ������ɲä�Ԥ��ޤ�.
 *
 * @param      str ʸ����Υ��ɥ쥹
 * @return     �����Хåե��Υ��ɥ쥹
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
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
 * @brief  ʸ��������
 *
 *  ���Ȥ�ʸ����ȡ��黻�Ҥα�¦(����) �ǻ��ꤷ��ʸ����
 *  ���פ��Ƥ��뤫�ɤ�������Ӥ�Ԥ������η�̤��֤��ޤ���
 *
 * @param      str ʸ����Υ��ɥ쥹
 * @return     ʸ���󤬰��פ�������true<br>
 *             ʸ�����԰��פǤ������false
 * 
 */
bool tstring::operator==(const char *str) const
{
    if ( this->compare(str) == 0 ) return true;
    else return false;
}

/**
 * @brief  ʸ��������
 *
 *  ���Ȥ�ʸ����ȡ��黻�Ҥα�¦(����) �ǻ��ꤷ��ʸ����
 *  �԰��פ��ɤ�������Ӥ�Ԥ������η�̤��֤��ޤ���
 *
 * @param      str ʸ����Υ��ɥ쥹
 * @return     ʸ�����԰��פǤ������true<br>
 *             ʸ���󤬰��פ�������false
 * 
 */
bool tstring::operator!=(const char *str) const
{
    if ( this->compare(str) == 0 ) return false;
    else return true;
}

/**
 * @brief  ʸ����μ���Ū�����
 *
 *  ���Ȥ�ʸ�����,���դΤǻ��ꤷ��ʸ����Ȥ򼭽�Ū����Ӥ���
 *  "���� < ����" ����Ω������� true ���֤��ޤ���
 * 
 * @param   obj tstring���饹�Υ��֥�������
 * @return  ������
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
 * �������ˤȤ��ơ�������ʸ����,�ݥ�������� object �����Ǵ�������Ƥ���
 * �ΰ褬Ϳ�����Ƥ�ư�����ͤȤ��롥�������äơ�realloc() �ϴ���Ū��
 * �Ȥ�ʤ���realloc() ��Ȥ����ϡ�strdup() ����ʸ����򻲾Ȥ���褦��
 * ���Ƥ��롥
 *
 */

/**
 * @brief  �桼���Υݥ����ѿ�����Ͽ
 *
 *  �桼���Υݥ����ѿ��Υ��ɥ쥹�� extptr_address ��Ϳ����ȡ��桼���Υݥ���
 *  ���ѿ������֥������Ȥ���Ͽ���졤���֥������Ȥ�����ʸ�������Ƭ���ɥ쥹��
 *  ��˥桼���Υݥ����ѿ����ݻ������Ƥ������Ȥ��Ǥ��ޤ���
 * 
 * @param   extptr_address �桼���Υݥ����ѿ��Υ��ɥ쥹
 * @return  ���Ȥλ���
 * @attention  �桼���Υݥ����ѿ�����Ͽ����� shallow copy ����ǽ���ʤ��ʤ�
 *             �ޤ���
 */
tstring &tstring::register_extptr(char **extptr_address)
{
    this->extptr_rec = extptr_address;
    if ( this->extptr_rec != NULL ) *(this->extptr_rec) = this->str_ptr();
    return *this;
}

/**
 * @brief  �桼���Υݥ����ѿ�(�ɤ߼������)����Ͽ
 *
 *  �桼���Υݥ����ѿ��Υ��ɥ쥹�� extptr_address ��Ϳ����ȡ��桼���Υݥ���
 *  ���ѿ������֥������Ȥ���Ͽ���졤���֥������Ȥ�����ʸ�������Ƭ���ɥ쥹��
 *  ��˥桼���Υݥ����ѿ����ݻ������Ƥ������Ȥ��Ǥ��ޤ���
 * 
 * @param   extptr_address �桼���Υݥ����ѿ��Υ��ɥ쥹
 * @return  ���Ȥλ���
 * @attention  �桼���Υݥ����ѿ�����Ͽ����� shallow copy ����ǽ���ʤ��ʤ�
 *             �ޤ���
 */
tstring &tstring::register_extptr(const char **extptr_address)
{
    this->extptr_rec = (char **)extptr_address;
    if ( this->extptr_rec != NULL ) *(this->extptr_rec) = this->str_ptr();
    return *this;
}

/**
 * @brief  ���֥������Ȥν����
 *
 *  ���Ȥν������Ԥ��ޤ���
 *
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * 
 */
tstring &tstring::init()
{
    this->regex_rec.init();

    if ( this->str_length_rec != UNDEF ) {	/* ����Ĺ�Хåե��ξ�� */

	this->__init_fixed_str_rec(0);

	this->init_reg_results();

    }
    else {

	/* shallow copy �ط��Υ��꡼�󥢥å� */
	this->cleanup_shallow_copy(false);

	/* ��������� */
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
 * @brief  ���֥������ȤΥ��ԡ�
 *
 *  ���� obj �����ƤǼ��Ȥ��������ޤ���
 *
 * @param      obj ���ԡ����Ȥʤ�ʸ�������ĥ��֥�������
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * @throw      �����˲��򵯤��������
 * 
 */
tstring &tstring::init(const tstring &obj)
{
    if ( &obj == this ) return *this;

    debug_report_scopy1("arg: obj = %zx",(size_t)(&obj));

    this->regex_rec.init();

    if ( this->str_length_rec != UNDEF ) {	/* ����Ĺ�Хåե��ξ�� */

	this->__deep_init(obj);

    }
    else {

	bool do_shallow_copy = false;

	/* shallow copy �ط��Υ��꡼�󥢥å� */
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
 * @brief  ʸ����Ĺ���ѹ�
 *
 *  ���Ȥ�����ʸ�����Ĺ����len ���ѹ����ޤ���
 *
 * @param      len �ѹ����ʸ����Ĺ
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
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
 * @brief  ����Ū��ʸ����Ĺ���ѹ�
 *
 *  ���Ȥ�����ʸ�����Ĺ����len ��Ĺ��ʬ�����ѹ����ޤ���
 *
 * @param      len ʸ����Ĺ����ʬ����ʬ
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
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
 * @brief  ʸ�������Τ���ꥭ��饯���ǥѥǥ���
 *
 *  ���Ȥ�����ʸ�������Τ�ʸ��ch �ǥѥǥ��󥰤��ޤ���
 *
 * @param      ch ʸ�����ѥǥ��󥰤���ʸ��
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * 
 */
tstring &tstring::clean( int ch )
{
    if ( this->str_length_rec != UNDEF ) {	/* ����Ĺ�Хåե��ξ�� */
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
 * @brief  printf()�ε�ˡ��ʸ���������
 *
 *  ���Ȥ�ʸ����򡤰����ǻ��ꤵ�줿ʸ����ǽ�������ޤ���
 *
 * @param      format �����Ȥʤ�ʸ����Τ���Υե����ޥåȻ���
 * @param      ... format���б���������Ĺ�����γ����ǥǡ���
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * @throw      ����Ĺ�����γ����ǥǡ��������ꤵ�줿�Ѵ��ե����ޥåȤ��Ѵ�
 *             �Ǥ��ʤ��ͤξ��
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
 * @brief  printf()�ε�ˡ��ʸ���������
 *
 *  ���Ȥ�ʸ����򡤰����ǻ��ꤵ�줿ʸ����ǽ�������ޤ���
 *
 * @param      format �����Ȥʤ�ʸ����Τ���Υե����ޥåȻ���
 * @param      ap format���б���������Ĺ�����Υꥹ��
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * @throw      ����Ĺ�����γ����ǥǡ��������ꤵ�줿�Ѵ��ե����ޥåȤ��Ѵ�
 *             �Ǥ��ʤ��ͤξ��
 * 
 */
tstring &tstring::vprintf( const char *format, va_list ap )
{
    return this->vassignf(format,ap);
}

/**
 * @brief  ʸ������ʬʸ���������
 *
 *  ���Ȥ�ʸ����򡤰����ǻ��ꤵ�줿ʸ����ǽ�������ޤ���
 *
 * @param      src �����Ȥʤ�ʸ��������tstring���饹�Υ��֥�������
 * @param      pos2 src ������ʸ����γ��ϰ���
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * 
 */
tstring &tstring::assign( const tstring &src, size_t pos2 )
{
    return this->assign(src,pos2,src.length());
}

/**
 * @brief  ��ʬʸ���������
 *
 *  ���Ȥ�ʸ����򡤰����ǻ��ꤵ�줿ʸ����ǽ�������ޤ���
 *
 * @param      src �����Ȥʤ�ʸ��������tstring���饹�Υ��֥�������
 * @param      pos2 src ������ʸ����γ��ϰ���
 * @param      n2 �񤭹��ޤ��ʸ�����Ĺ��
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * 
 */
tstring &tstring::assign( const tstring &src, size_t pos2, size_t n2 )
{
    return tstring::replace(0,this->length(), src,pos2,n2);
}

/**
 * @brief  printf()�ε�ˡ��ʸ���������
 *
 *  ���Ȥ�ʸ����򡤰����ǻ��ꤵ�줿ʸ����ǽ�������ޤ���
 *
 * @param      format �����Ȥʤ�ʸ����Τ���Υե����ޥåȻ���
 * @param      ... format���б���������Ĺ�����γ����ǥǡ���
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * @throw      ����Ĺ�����γ����ǥǡ��������ꤵ�줿�Ѵ��ե����ޥåȤ��Ѵ�
 *             �Ǥ��ʤ��ͤξ��
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
 * @brief  printf()�ε�ˡ��ʸ���������
 *
 *  ���Ȥ�ʸ����򡤰����ǻ��ꤵ�줿ʸ����ǽ�������ޤ���
 *
 * @param      format �����Ȥʤ�ʸ����Τ���Υե����ޥåȻ���
 * @param      ap format���б���������Ĺ�����Υꥹ��
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * @throw      ����Ĺ�����γ����ǥǡ��������ꤵ�줿�Ѵ��ե����ޥåȤ��Ѵ�
 *             �Ǥ��ʤ��ͤξ��
 * 
 */
tstring &tstring::vassignf( const char *format, va_list ap )
{
    if ( this->str_length_rec != UNDEF ) {	/* ����Ĺ�Хåե��ξ�� */
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
 * @brief  ʸ���������
 *
 *  ���Ȥ�ʸ����򡤰����ǻ��ꤵ�줿ʸ����ǽ�������ޤ���
 *
 * @param      str �����Ȥʤ�ʸ����
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * 
 */
tstring &tstring::assign( const char *str )
{
    if ( this->str_length_rec != UNDEF ) {	/* ����Ĺ�Хåե��ξ�� */
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
 * @brief  ʸ���������
 *
 *  ���Ȥ�ʸ����򡤰����ǻ��ꤵ�줿ʸ����ǽ�������ޤ���
 *
 * @param      str �����Ȥʤ�ʸ����
 * @param      n str��Ĺ��
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * 
 */
tstring &tstring::assign( const char *str, size_t n )
{
    if ( this->str_length_rec != UNDEF ) {	/* ����Ĺ�Хåե��ξ�� */
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
 * @brief  ʸ�� ch �� n ��Ϣ³����ʸ���������
 *
 *  ���Ȥ�ʸ����򡤰����ǻ��ꤵ�줿ʸ����ǽ�������ޤ���
 *
 * @param      ch �����Ȥʤ�ʸ��
 * @param      n ch�θĿ�
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * 
 */
tstring &tstring::assign( int ch, size_t n )
{
    if ( this->str_length_rec != UNDEF ) {	/* ����Ĺ�Хåե��ξ�� */
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
 * @brief  printf()�ε�ˡ�ǻ��ꤵ�줿ʸ����ˤ�ꡤ������֤�����
 *
 *  ���Ȥ�ʸ��������pos ���顤�����ǻ��ꤵ�줿ʸ����Ǿ�񤭤��ޤ���
 *
 * @param      pos ���֥����������ʸ����γ��ϰ���
 * @param      format �����Ȥʤ�ʸ����Τ���Υե����ޥåȻ���
 * @param      ... format���б���������Ĺ�����γ����ǥǡ���
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * @throw      ����Ĺ�����γ����ǥǡ��������ꤵ�줿�Ѵ��ե����ޥåȤ��Ѵ�
 *             �Ǥ��ʤ��ͤξ��
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
 * @brief  printf()�ε�ˡ�ǻ��ꤵ�줿ʸ����ˤ�ꡤ������֤�����
 *
 *  ���Ȥ�ʸ��������pos ���顤�����ǻ��ꤵ�줿ʸ����Ǿ�񤭤��ޤ���
 *
 * @param      pos ���֥����������ʸ����γ��ϰ���
 * @param      format �����Ȥʤ�ʸ����Τ���Υե����ޥåȻ���
 * @param      ap format���б���������Ĺ�����Υꥹ��
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * @throw      ����Ĺ�����γ����ǥǡ��������ꤵ�줿�Ѵ��ե����ޥåȤ��Ѵ�
 *             �Ǥ��ʤ��ͤξ��
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
 * @brief  ���ꤵ�줿ʸ����ˤ�ꡤ������֤�����
 *
 *  ���Ȥ�ʸ�������� pos ���顤�����ǻ��ꤵ�줿ʸ����Ǿ�񤭤��ޤ���
 *
 * @param      pos ���֥����������ʸ����γ��ϰ���
 * @param      str �����Ȥʤ�ʸ����
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * 
 */
tstring &tstring::put( size_t pos, const char *str )
{
    if ( this->str_length_rec != UNDEF ) {	/* ����Ĺ�Хåե��ξ�� */
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
 * @brief  ���ꤵ�줿ʸ����ˤ�ꡤ������֤�����
 *
 *  ���Ȥ�ʸ��������pos ���顤�����ǻ��ꤵ�줿ʸ����Ǿ�񤭤��ޤ���
 *
 * @param      pos ���֥����������ʸ����γ��ϰ���
 * @param      str �����Ȥʤ�ʸ����
 * @param      n str��Ĺ��
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * 
 */
tstring &tstring::put( size_t pos, const char *str, size_t n )
{
    if ( this->str_length_rec != UNDEF ) {	/* ����Ĺ�Хåե��ξ�� */
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
 * @brief  ʸ�� ch �� n ��Ϣ³����ʸ����ˤ�ꡤ������֤�����
 *
 *  ���Ȥ�ʸ��������pos ���顤�����ǻ��ꤵ�줿ʸ����Ǿ�񤭤��ޤ���
 *
 * @param      pos ���֥����������ʸ����γ��ϰ���
 * @param      ch �����Ȥʤ�ʸ��
 * @param      n ch�θĿ�
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * 
 */
tstring &tstring::put( size_t pos, int ch, size_t n )
{
    if ( this->str_length_rec != UNDEF ) {	/* ����Ĺ�Хåե��ξ�� */
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
 * @brief  ���ꤵ�줿ʸ����ˤ�ꡤ������֤�����
 *
 *  ���Ȥ�ʸ��������pos1 ���顤�����ǻ��ꤵ�줿ʸ����Ǿ�񤭤��ޤ���
 *
 * @param      pos1 ���֥����������ʸ����γ��ϰ���
 * @param      src �����Ȥʤ�ʸ��������tstring���饹�Υ��֥�������
 * @param      pos2 src ������ʸ����γ��ϰ���
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * 
 */
tstring &tstring::put( size_t pos1, const tstring &src, size_t pos2 )
{
    return this->put(pos1, src, pos2, src.length());
}

/**
 * @brief  ���ꤵ�줿ʸ����ˤ�ꡤ������֤�����
 *
 *  ���Ȥ�ʸ��������pos1 ���顤�����ǻ��ꤵ�줿ʸ����Ǿ�񤭤��ޤ���
 *
 * @param      pos1 ���֥����������ʸ����γ��ϰ���
 * @param      src �����Ȥʤ�ʸ��������tstring���饹�Υ��֥�������
 * @param      pos2 src ������ʸ����γ��ϰ���
 * @param      n2 �񤭹��ޤ��ʸ�����Ĺ��
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * 
 */
tstring &tstring::put( size_t pos1, const tstring &src, size_t pos2, size_t n2)
{
    return tstring::replace(pos1,n2, src,pos2,n2);
}

/**
 * @brief  ʸ������ɲ�
 *
 *  ���Ȥ�ʸ����ˡ������ǻ��ꤵ�줿ʸ������ɲä��ޤ���
 *
 * @param      str �����Ȥʤ�ʸ����
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * 
 */
tstring &tstring::strcat( const char *str )
{
    return this->append( str );
}

/**
 * @brief  ��ʬʸ������ɲ�
 *
 *  ���Ȥ�ʸ����ˡ������ǻ��ꤵ�줿ʸ������ɲä��ޤ���
 *
 * @param      str �����Ȥʤ�ʸ����
 * @param      n str��Ĺ��
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * 
 */
tstring &tstring::strncat( const char *str, size_t n )
{
    return this->append( str, n );
}

/**
 * @brief  ʸ������ʬʸ������ɲ�
 *
 *  ���Ȥ�ʸ����ˡ������ǻ��ꤵ�줿ʸ������ɲä��ޤ���
 *
 * @param      src �����Ȥʤ�ʸ��������tstring���饹�Υ��֥�������
 * @param      pos2 src������ʸ����γ��ϰ���
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * 
 */
tstring &tstring::strcat(const tstring &src, size_t pos2 )
{
    return this->append(src,pos2,src.length());
}

/**
 * @brief  ��ʬʸ������ɲ�
 *
 *  ���Ȥ�ʸ����ˡ������ǻ��ꤵ�줿ʸ������ɲä��ޤ���
 *
 * @param      src �����Ȥʤ�ʸ��������tstring���饹�Υ��֥�������
 * @param      pos2 src������ʸ����γ��ϰ���
 * @param      n2 �ɲä���ʸ�����Ĺ��
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * 
 */
tstring &tstring::strncat(const tstring &src, size_t pos2, size_t n2 )
{
    return tstring::replace(this->length(),0, src,pos2,n2);
}

/**
 * @brief  printf()�ε�ˡ�ǻ��ꤵ�줿ʸ������ɲ�
 *
 *  ���Ȥ�ʸ����ˡ������ǻ��ꤵ�줿ʸ������ɲä��ޤ���
 *
 * @param      format �����Ȥʤ�ʸ����Τ���Υե����ޥåȻ���
 * @param      ... format���б���������Ĺ�����γ����ǥǡ���
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * @throw      ����Ĺ�����γ����ǥǡ��������ꤵ�줿�Ѵ��ե����ޥåȤ��Ѵ�
 *             �Ǥ��ʤ��ͤξ��
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
 * @brief  printf()�ε�ˡ�ǻ��ꤵ�줿ʸ������ɲ�
 *
 *  ���Ȥ�ʸ����ˡ������ǻ��ꤵ�줿ʸ������ɲä��ޤ���
 *
 * @param      format �����Ȥʤ�ʸ����Τ���Υե����ޥåȻ���
 * @param      ap format���б���������Ĺ�����Υꥹ��
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * @throw      ����Ĺ�����γ����ǥǡ��������ꤵ�줿�Ѵ��ե����ޥåȤ��Ѵ�
 *             �Ǥ��ʤ��ͤξ��
 * 
 */
tstring &tstring::vappendf( const char *format, va_list ap )
{
    if ( this->str_length_rec != UNDEF ) {	/* ����Ĺ�Хåե��ξ�� */
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
 * @brief  ʸ������ɲ�
 *
 *  ���Ȥ�ʸ����ˡ������ǻ��ꤵ�줿ʸ������ɲä��ޤ���
 *
 * @param      str �����Ȥʤ�ʸ����
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * 
 */
tstring &tstring::append( const char *str )
{
    if ( this->str_length_rec != UNDEF ) {	/* ����Ĺ�Хåե��ξ�� */
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
 * @brief  ʸ������ʬʸ������ɲ�
 *
 *  ���Ȥ�ʸ����ˡ������ǻ��ꤵ�줿ʸ������ɲä��ޤ���
 *
 * @param      src �����Ȥʤ�ʸ��������tstring���饹�Υ��֥�������
 * @param      pos2 src������ʸ����γ��ϰ���
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * 
 */
tstring &tstring::append(const tstring &src, size_t pos2 )
{
    return this->append(src,pos2,src.length());
}

/**
 * @brief  ��ʬʸ������ɲ�
 *
 *  ���Ȥ�ʸ����ˡ������ǻ��ꤵ�줿ʸ������ɲä��ޤ���
 *
 * @param      src �����Ȥʤ�ʸ��������tstring���饹�Υ��֥�������
 * @param      pos2 src������ʸ����γ��ϰ���
 * @param      n2 �ɲä���ʸ�����Ĺ��
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * 
 */
tstring &tstring::append(const tstring &src, size_t pos2, size_t n2 )
{
    return tstring::replace(this->length(),0, src,pos2,n2);
}

/**
 * @brief  ʸ������ɲ�
 *
 *  ���Ȥ�ʸ����ˡ������ǻ��ꤵ�줿ʸ������ɲä��ޤ���
 *
 * @param      str �����Ȥʤ�ʸ����
 * @param      n str��Ĺ��
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * 
 */
tstring &tstring::append( const char *str, size_t n )
{
    if ( this->str_length_rec != UNDEF ) {	/* ����Ĺ�Хåե��ξ�� */
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
 * @brief  ʸ�� ch �� n ��Ϣ³����ʸ������ɲ�
 *
 *  ���Ȥ�ʸ����ˡ������ǻ��ꤵ�줿ʸ������ɲä��ޤ���
 *
 * @param      ch �����Ȥʤ�ʸ��
 * @param      n ch�θĿ�
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * 
 */
tstring &tstring::append( int ch, size_t n )
{
    if ( this->str_length_rec != UNDEF ) {	/* ����Ĺ�Хåե��ξ�� */
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
 * @brief  printf()�ε�ˡ�ǻ��ꤵ�줿ʸ����������֤�����
 *
 *  ���Ȥ�ʸ����ΰ��� pos1 �ˡ������ǻ��ꤵ�줿ʸ������������ޤ���
 *
 * @param      pos1 ���֥����������ʸ����γ��ϰ���
 * @param      format �����Ȥʤ�ʸ����Τ���Υե����ޥåȻ���
 * @param      ... format���б���������Ĺ�����γ����ǥǡ���
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * @throw      ����Ĺ�����γ����ǥǡ��������ꤵ�줿�Ѵ��ե����ޥåȤ��Ѵ�
 *             �Ǥ��ʤ��ͤξ��
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
 * @brief  printf()�ε�ˡ�ǻ��ꤵ�줿ʸ����������֤�����
 *
 *  ���Ȥ�ʸ����ΰ��� pos1 �ˡ������ǻ��ꤵ�줿ʸ������������ޤ���
 *
 * @param      pos1 ���֥����������ʸ����γ��ϰ���
 * @param      format �����Ȥʤ�ʸ����Τ���Υե����ޥåȻ���
 * @param      ap format���б���������Ĺ�����Υꥹ��
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * @throw      ����Ĺ�����γ����ǥǡ��������ꤵ�줿�Ѵ��ե����ޥåȤ��Ѵ�
 *             �Ǥ��ʤ��ͤξ��
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
 * @brief  ʸ����λ�����֤ؤ�����
 *
 *  ���Ȥ�ʸ����ΰ���pos1 �ˡ������ǻ��ꤵ�줿ʸ������������ޤ���
 *
 * @param      pos1 ���֥����������ʸ����γ��ϰ���
 * @param      str �����Ȥʤ�ʸ����
 * @param      n str��Ĺ��
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * 
 */
tstring &tstring::insert( size_t pos1, const char *str, size_t n )
{
    return tstring::replace(pos1,0, str,n);
}

/**
 * @brief  ʸ����λ�����֤ؤ�����
 *
 *  ���Ȥ�ʸ����ΰ���pos1 �ˡ������ǻ��ꤵ�줿ʸ������������ޤ���
 *
 * @param      pos1 ���֥����������ʸ����γ��ϰ���
 * @param      str �����Ȥʤ�ʸ����
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * 
 */
tstring &tstring::insert( size_t pos1, const char *str )
{
    if ( str == NULL ) return this->insert( pos1, str, (size_t)0 );
    else return this->insert( pos1, str, c_strlen(str) );
}

/**
 * @brief  ʸ������ʬʸ����λ�����֤ؤ�����
 *
 *  ���Ȥ�ʸ����ΰ���pos1 �ˡ������ǻ��ꤵ�줿ʸ������������ޤ���
 *
 * @param      pos1 ���֥����������ʸ����γ��ϰ���
 * @param      src �����Ȥʤ�ʸ��������tstring���饹�Υ��֥�������
 * @param      pos2 src������ʸ����γ��ϰ���
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * 
 */
tstring &tstring::insert( size_t pos1, const tstring &src, size_t pos2 )
{
    return this->insert(pos1, src, pos2, src.length());
}

/**
 * @brief  ��ʬʸ����λ�����֤ؤ�����
 *
 *  ���Ȥ�ʸ����ΰ���pos1 �ˡ������ǻ��ꤵ�줿ʸ������������ޤ���
 *
 * @param      pos1 ���֥����������ʸ����γ��ϰ���
 * @param      src �����Ȥʤ�ʸ��������tstring���饹�Υ��֥�������
 * @param      pos2 src������ʸ����γ��ϰ���
 * @param      n2 ���������ʸ�����Ĺ��
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * 
 */
tstring &tstring::insert( size_t pos1, 
			  const tstring &src, size_t pos2, size_t n2 )
{
    return tstring::replace(pos1,0, src,pos2,n2);
}

/**
 * @brief  ʸ�� ch �� n ��Ϣ³����ʸ����λ�����֤ؤ�����
 *
 *  ���Ȥ�ʸ����ΰ���pos1 �ˡ������ǻ��ꤵ�줿ʸ������������ޤ���
 *
 * @param      pos1 ���֥����������ʸ����γ��ϰ���
 * @param      ch �����Ȥʤ�ʸ��
 * @param      n ch�θĿ�
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
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
 * @brief  ������ʬ�� printf()�ε�ˡ�ǻ��ꤵ�줿ʸ������ִ�
 *
 *  ���Ȥ�ʸ����ΰ���pos1 ����n1 ʸ���򡤻��ꤵ�줿ʸ������֤������ޤ���
 *
 * @param      pos1 ���֥����������ʸ����γ��ϰ���
 * @param      n1 �ִ�����ʸ����
 * @param      format �����Ȥʤ�ʸ����Τ���Υե����ޥåȻ���
 * @param      ... format���б���������Ĺ�����γ����ǥǡ���
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * @throw      ����Ĺ�����γ����ǥǡ��������ꤵ�줿�Ѵ��ե����ޥåȤ��Ѵ�
 *             �Ǥ��ʤ��ͤξ��
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
 * @brief  ������ʬ�� printf()�ε�ˡ�ǻ��ꤵ�줿ʸ������ִ�
 *
 *  ���Ȥ�ʸ����ΰ���pos1 ����n1 ʸ���򡤻��ꤵ�줿ʸ������֤������ޤ���
 *
 * @param      pos1 ���֥����������ʸ����γ��ϰ���
 * @param      n1 �ִ�����ʸ����
 * @param      format �����Ȥʤ�ʸ����Τ���Υե����ޥåȻ���
 * @param      ap format���б���������Ĺ�����Υꥹ��
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * @throw      ����Ĺ�����γ����ǥǡ��������ꤵ�줿�Ѵ��ե����ޥåȤ��Ѵ�
 *             �Ǥ��ʤ��ͤξ��
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
 * @brief  ������ʬ��ʸ������ִ�
 *
 *  ���Ȥ�ʸ����ΰ���pos1 ����n1 ʸ���򡤻��ꤵ�줿ʸ������֤������ޤ���
 *
 * @param      pos1 ���֥����������ʸ����γ��ϰ���
 * @param      n1 �ִ�����ʸ����
 * @param      str �����Ȥʤ�ʸ����
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * 
 */
tstring &tstring::replace( size_t pos1, size_t n1, const char *str )
{
    if ( str == NULL ) return this->replace( pos1, n1, str, (size_t)0 );
    else return this->replace( pos1, n1, str, c_strlen(str) );
}

/**
 * @brief  ������ʬ��ʸ������ִ�
 *
 *  ���Ȥ�ʸ����ΰ���pos1 ����n1 ʸ���򡤻��ꤵ�줿ʸ������֤������ޤ���
 *
 * @param      pos1 ���֥����������ʸ����γ��ϰ���
 * @param      n1 �ִ�����ʸ����
 * @param      src �����Ȥʤ�ʸ��������tstring���饹�Υ��֥�������
 * @param      pos2 src������ʸ����γ��ϰ���
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * 
 */
tstring &tstring::replace( size_t pos1, size_t n1, 
			   const tstring &src, size_t pos2 )
{
    return this->replace(pos1,n1,src,pos2,src.length());
}

/**
 * @brief  ������ʬ��ʸ������ִ�
 *
 *  ���Ȥ�ʸ����ΰ���pos1 ����n1 ʸ���򡤻��ꤵ�줿ʸ������֤������ޤ���
 *
 * @param      pos1 ���֥����������ʸ����γ��ϰ���
 * @param      n1 �ִ�����ʸ����
 * @param      src �����Ȥʤ�ʸ��������tstring���饹�Υ��֥�������
 * @param      pos2 src������ʸ����γ��ϰ���
 * @param      n2 src������ʸ�����Ĺ��
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
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
	if ( this->str_length_rec == UNDEF ) {	/* ����Ĺ�Хåե��ξ��Τ� */
	    if ( &src == this ) {		/* ���������Ȥξ������� */
		if ( work_buf.allocate(n2) < 0 ) {
		    err_throw(__FUNCTION__,"FATAL","malloc() failed");
		}
		c_memcpy(work_buf.ptr(), src_buf, n2);
		src_buf = work_buf.ptr();
	    }
	}
	this->replace(pos1,n1, ' ',n2);
	/* �Х��ʥ�ǡ����ξ��ˤ��б������� */
	char *dest_buf = this->str_ptr();
	for ( i=0 ; i < n2 && pos1 + i < this->length() ; i++ ) {
	    dest_buf[pos1 + i] = src_buf[i];
	}
    }
    return *this;
}

/**
 * @brief  ������ʬ��ʸ������ִ�
 *
 *  ���Ȥ�ʸ����ΰ���pos1 ����n1 ʸ���򡤻��ꤵ�줿ʸ������֤������ޤ���
 *
 * @param      pos1 ���֥����������ʸ����γ��ϰ���
 * @param      n1 �ִ�����ʸ����
 * @param      str �����Ȥʤ�ʸ����
 * @param      n2 str������ʸ�����Ĺ��
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * 
 */
tstring &tstring::replace( size_t pos1, size_t n1, const char *str, size_t n2 )
{
    const char *src_ptr = str;
    heap_mem<char> work_buf;
    size_t new_length;
    size_t i;

    if ( 0 < n2 && str == NULL ) return *this;

    /* n2 ��꾮���������б� */
    for ( i=0 ; i < n2 && str[i] != '\0' ; i++ );
    n2 = i;

    if ( this->str_length_rec == UNDEF ) {	/* ����Ĺ�Хåե��ξ��Τ� */
	if ( 0 < n2 && this->is_my_buffer(str) == true ) {
	    /* str �� this->_str_rec �����Ƥ�ؤ��Ƥ�����  */
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

    if ( this->str_length_rec != UNDEF ) {	/* ����Ĺ�Хåե��ξ�� */
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
    else {					/* ����Ĺ�Хåե��ξ�� */
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
 * @brief  ������ʬ��ʸ�����ʸ�� ch �� n ��Ϣ³����ʸ������ִ�
 *
 *  ���Ȥ�ʸ����ΰ���pos1 ����n1 ʸ���򡤻��ꤵ�줿ʸ������֤������ޤ���
 *
 * @param      pos1 ���֥����������ʸ����γ��ϰ���
 * @param      n1 �ִ�����ʸ����
 * @param      ch �����Ȥʤ�ʸ��
 * @param      n2 ch�θĿ�
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
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

    if ( this->str_length_rec != UNDEF ) {	/* ����Ĺ�Хåե��ξ�� */
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
    else {					/* ����Ĺ�Хåե��ξ�� */
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
 * @brief  ʸ����ΰ�����õ�
 *
 *  ���Ȥ�����ʸ�����ʸ����õ�ޤ���
 *
 * @param      pos �õ�γ��ϰ���
 * @param      n �õ��ʸ����
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * 
 */
tstring &tstring::erase( size_t pos, size_t n )
{
    return tstring::replace(pos,n, ' ',(size_t)0);
}

/**
 * @brief  ʸ��������Ƥ�õ�
 *
 *  ���Ȥ�����ʸ�����ʸ����õ�ޤ���
 *
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * 
 */
tstring &tstring::erase()
{
    return this->erase(0,this->length());

}

/**
 * @brief  ʸ����ΰ���ʬ�ʳ���õ�
 *
 *  ���Ȥ�ʸ����򡤰���pos ����n �Ĥ�ʸ��������ˤ��ޤ���
 *
 * @param      pos �ڤ�ȴ���γ��ϰ���
 * @param      n �ڤ�ȴ��ʸ����
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
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
 * @brief  ʸ����ΰ���ʬ�ʳ���õ�
 *
 *  ���Ȥ�ʸ����򡤰���pos ����n �Ĥ�ʸ��������ˤ��ޤ���
 *
 * @param      pos �ڤ�ȴ���γ��ϰ���
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * 
 */
tstring &tstring::crop( size_t pos )
{
    if ( this->length() < pos ) pos = this->length();
    return this->crop(pos,this->length() - pos);
}

/* ��侩 */
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
 * @brief  ʸ�����ñ�㸡�����ִ�
 *
 *  ���Ȥ�����ʸ����κ�¦����ʸ����org_str �򸡺�����
 *  ���Ĥ��ä�����ʸ����new_str ���֤������ޤ���
 *
 * @param      org_str ���Ф���ʸ����
 * @param      new_str �ִ��θ����Ȥʤ�ʸ����
 * @param      all ���ִ��Υե饰
 * @return     �����������������<br>
 *             ���Ԥ�����������
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
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
 * @brief  ʸ�����ñ�㸡�����ִ�
 *
 *  ���Ȥ�����ʸ����κ�¦����ʸ����org_str �򸡺�����<br>
 *  ���Ĥ��ä�����ʸ����new_str ���֤������ޤ���
 *
 * @param      pos ʸ���󸡺��γ��ϰ���
 * @param      org_str ���Ф���ʸ����
 * @param      new_str �ִ��θ����Ȥʤ�ʸ����
 * @param      all ���ִ��Υե饰
 * @return     �����������������<br>
 *             ���Ԥ�����������
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
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
 * @brief  ʸ�����ñ�㸡�����ִ�
 *
 *  ���Ȥ�����ʸ����κ�¦����ʸ����org_str �򸡺�����<br>
 *  ���Ĥ��ä�����ʸ����new_str ���֤������ޤ���
 *
 * @param      org_str ���Ф���ʸ����
 * @param      new_str �ִ��θ����Ȥʤ�ʸ����
 * @param      all ���ִ��Υե饰
 * @return     �����������������<br>
 *             ���Ԥ�����������
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * 
 */
ssize_t tstring::strreplace( const tstring &org_str, const char *new_str,
			     bool all )
{
    return this->strreplace(org_str.cstr(), new_str, all);
}

/**
 * @brief  ʸ�����ñ�㸡�����ִ�
 *
 *  ���Ȥ�����ʸ����κ�¦����ʸ����org_str �򸡺�����<br>
 *  ���Ĥ��ä�����ʸ����new_str ���֤������ޤ���
 *
 * @param      pos ʸ���󸡺��γ��ϰ���
 * @param      org_str ���Ф���ʸ����
 * @param      new_str �ִ��θ����Ȥʤ�ʸ����
 * @param      all ���ִ��Υե饰
 * @return     �����������������<br>
 *             ���Ԥ�����������
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * 
 */
ssize_t tstring::strreplace( size_t pos, const tstring &org_str,
			     const char *new_str, bool all )
{
    return this->strreplace(pos, org_str.cstr(), new_str, all);
}


/**
 * @brief  ʸ�����ñ�㸡�����ִ�
 *
 *  ���Ȥ�����ʸ����κ�¦����ʸ����org_str �򸡺�����<br>
 *  ���Ĥ��ä�����ʸ����new_str ���֤������ޤ���
 *
 * @param      org_str ���Ф���ʸ����
 * @param      new_str �ִ��θ����Ȥʤ�ʸ����
 * @param      all ���ִ��Υե饰
 * @return     �����������������<br>
 *             ���Ԥ�����������
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * 
 */
ssize_t tstring::strreplace( const char *org_str, const tstring &new_str,
			     bool all )
{
    return this->strreplace(org_str, new_str.cstr(), all);
}

/**
 * @brief  ʸ�����ñ�㸡�����ִ�
 *
 *  ���Ȥ�����ʸ����κ�¦����ʸ����org_str �򸡺�����<br>
 *  ���Ĥ��ä�����ʸ����new_str ���֤������ޤ���
 *
 * @param      pos ʸ���󸡺��γ��ϰ���
 * @param      org_str ���Ф���ʸ����
 * @param      new_str �ִ��θ����Ȥʤ�ʸ����
 * @param      all ���ִ��Υե饰
 * @return     �����������������<br>
 *             ���Ԥ�����������
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * 
 */
ssize_t tstring::strreplace( size_t pos, const char *org_str,
			     const tstring &new_str, bool all )
{
    return this->strreplace(pos, org_str, new_str.cstr(), all);
}

/**
 * @brief  ʸ�����ñ�㸡�����ִ�
 *
 *  ���Ȥ�����ʸ����κ�¦����ʸ����org_str �򸡺�����<br>
 *  ���Ĥ��ä�����ʸ����new_str ���֤������ޤ���
 *
 * @param      org_str ���Ф���ʸ����
 * @param      new_str �ִ��θ����Ȥʤ�ʸ����
 * @param      all ���ִ��Υե饰
 * @return     �����������������<br>
 *             ���Ԥ�����������
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * 
 */
ssize_t tstring::strreplace( const tstring &org_str, const tstring &new_str,
			     bool all )
{
    return this->strreplace(org_str.cstr(), new_str.cstr(), all);
}

/**
 * @brief  ʸ�����ñ�㸡�����ִ�
 *
 *  ���Ȥ�����ʸ����κ�¦����ʸ����org_str �򸡺�����<br>
 *  ���Ĥ��ä�����ʸ����new_str ���֤������ޤ���
 *
 * @param      pos ʸ���󸡺��γ��ϰ���
 * @param      org_str ���Ф���ʸ����
 * @param      new_str �ִ��θ����Ȥʤ�ʸ����
 * @param      all ���ִ��Υե饰
 * @return     �����������������<br>
 *             ���Ԥ�����������
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
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
 * @brief  ���ֳ�¦�Υ������ơ������ޤ��ϳ�̤θ����Ⱦõ�
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
 * @brief  ���ֳ�¦�Υ������ơ������ޤ��ϳ�̤θ����Ⱦõ�
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
 * @brief  ������ʸ���ν���
 *
 *  ���Ȥ�����ʸ�����������ʸ�������ޤ�
 *
 * @return  ���Ȥλ���
 * @throw   �����Хåե��κ���˼��Ԥ������
 */
tstring &tstring::chop()
{
    if ( 0 < this->length() ) this->resize(this->length()-1);
    return *this;
}

/**
 * @brief  ʸ����α�ü�β���ʸ���ν���
 *
 *  ���Ȥ�����ʸ����α�ü�β���ʸ�������ޤ���
 *
 * @param      rs ����ʸ����
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
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
 * @brief  ʸ����α�ü�β���ʸ���ν���
 *
 *  ���Ȥ�����ʸ����α�ü�β���ʸ�������ޤ���
 *
 * @param      rs ����ʸ����
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * 
 */
tstring &tstring::chomp( const tstring &rs )
{
    return this->chomp(rs.cstr());
}

/**
 * @brief  ʸ�����ξü������ʸ���ν���
 *
 *  ���Ȥ�����ʸ�����ξü�ˤ���Ǥ��ʸ�������ޤ���
 *
 * @param   side_spaces �����оݤ�ʸ�����åȡ�[A-Z] �Ȥ��ä�ɽ������Ѳ�ǽ��
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
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
	    if ( p0 == 0 ) {	/* �ѥ����󤬤������� */
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
 * @brief  ʸ�����ξü������ʸ���ν���
 *
 *  ���Ȥ�����ʸ�����ξü�ˤ���Ǥ��ʸ�������ޤ���
 *
 * @param   side_spaces �����оݤ�ʸ�����åȡ�[A-Z] �Ȥ��ä�ɽ������Ѳ�ǽ��
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 * 
 */
tstring &tstring::trim( const tstring &side_spaces )
{
    return this->trim(side_spaces.cstr());
}

/**
 * @brief  ʸ�����ξü������ʸ���ν���
 *
 *  ���Ȥ�����ʸ�����ξü�ˤ���Ǥ��ʸ�������ޤ���
 *
 * @param      side_space �����оݤ�ʸ��
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
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
 * @brief  ʸ����κ�ü������ʸ���ν���
 *
 *  ���Ȥ�����ʸ����κ�ü�ζ���ʸ�������ޤ���
 *
 * @param      side_spaces �����оݤ�ʸ�����åȡ�[A-Z] �Ȥ��ä�ɽ������Ѳ�ǽ��
 * @return     ���Ȥλ���
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
	    if ( p0 == 0 ) {	/* �ѥ����󤬤������� */
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
 * @brief  ʸ����κ�ü������ʸ���ν���
 *
 *  ���Ȥ�����ʸ����κ�ü�ζ���ʸ�������ޤ���
 *
 * @param      side_spaces �����оݤ�ʸ�����åȡ�[A-Z] �Ȥ��ä�ɽ������Ѳ�ǽ��
 * @return     ���Ȥλ���
 * 
 */
tstring &tstring::ltrim( const tstring &side_spaces )
{
    return this->ltrim(side_spaces.cstr());
}

/**
 * @brief  ʸ����κ�ü������ʸ���ν���
 *
 *  ���Ȥ�����ʸ����κ�ü�ζ���ʸ�������ޤ���
 *
 * @param      side_space �����оݤ�ʸ��
 * @return     ���Ȥλ���
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
 * @brief  ʸ����α�ü������ʸ���ν���
 *
 *  ���Ȥ�����ʸ����α�ü�ζ���ʸ�������ޤ���
 *
 * @param      side_spaces �����оݤ�ʸ�����åȡ�[A-Z] �Ȥ��ä�ɽ������Ѳ�ǽ��
 * @return     ���Ȥλ���
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
	    if ( p0 == 0 ) {	/* �ѥ����󤬤������� */
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
 * @brief  ʸ����α�ü������ʸ���ν���
 *
 *  ���Ȥ�����ʸ����α�ü�ζ���ʸ�������ޤ���
 *
 * @param      side_spaces �����оݤ�ʸ�����åȡ�[A-Z] �Ȥ��ä�ɽ������Ѳ�ǽ��
 * @return     ���Ȥλ���
 * 
 */
tstring &tstring::rtrim( const tstring &side_spaces )
{
    return this->rtrim(side_spaces.cstr());
}

/**
 * @brief  ʸ����α�ü������ʸ���ν���
 *
 *  ���Ȥ�����ʸ����α�ü�ζ���ʸ�������ޤ���
 *
 * @param      side_space �����оݤ�ʸ��
 * @return     ���Ȥλ���
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
 * @brief  ʸ�����ξü������ʸ���ν���
 *
 *  ���Ȥ�����ʸ�����ξü�ˤ���Ǥ��ʸ�������ޤ���
 *
 * @param      side_spaces �����оݤ�ʸ�����åȡ�[A-Z] �Ȥ��ä�ɽ������Ѳ�ǽ��
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * 
 */
tstring &tstring::strtrim( const char *side_spaces )
{
    return this->trim(side_spaces);
}

/**
 * @brief  ʸ�����ξü������ʸ���ν���
 *
 *  ���Ȥ�����ʸ�����ξü�ˤ���Ǥ��ʸ�������ޤ���
 *
 * @param      side_spaces �����оݤ�ʸ�����åȡ�[A-Z] �Ȥ��ä�ɽ������Ѳ�ǽ��
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * 
 */
tstring &tstring::strtrim( const tstring &side_spaces )
{
    return this->trim(side_spaces);
}

/**
 * @brief  ʸ�����ξü������ʸ���ν���
 *
 *  ���Ȥ�����ʸ�����ξü�ˤ���Ǥ��ʸ�������ޤ���
 *
 * @param      side_spaces �����оݤ�ʸ��
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * 
 */
tstring &tstring::strtrim( int side_space )
{
    return this->trim(side_space);
}

/**
 * @brief  ��ʸ����ʸ�����Ѵ�
 *
 *  ���Ȥ�ʸ���󤬻��ĥ���ե��٥åȤ���ʸ����ʸ�����Ѵ����ޤ���
 *
 * @param      pos �Ѵ��γ��ϰ���
 * @return     ���Ȥλ���
 * 
 */
tstring &tstring::tolower( size_t pos )
{
    return this->tolower(pos,this->length());
}

/**
 * @brief  ��ʸ����ʸ�����Ѵ�
 *
 *  ���Ȥ�ʸ���󤬻��ĥ���ե��٥åȤ���ʸ����ʸ�����Ѵ����ޤ���
 *
 * @param      pos �Ѵ��γ��ϰ���
 * @param      n �Ѵ�����ʸ��
 * @return     ���Ȥλ���
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
 * @brief  ��ʸ������ʸ�����Ѵ�
 *
 *  ���Ȥ�ʸ���󤬻��ĥ���ե��٥åȤξ�ʸ������ʸ�����Ѵ����ޤ���
 *
 * @param      pos �Ѵ��γ��ϰ���
 * @return     ���Ȥλ���
 * 
 */
tstring &tstring::toupper( size_t pos )
{
    return this->toupper(pos,this->length());
}

/**
 * @brief  ��ʸ������ʸ�����Ѵ�
 *
 *  ���Ȥ�ʸ���󤬻��ĥ���ե��٥åȤξ�ʸ������ʸ�����Ѵ����ޤ���
 *
 * @param      pos �Ѵ��γ��ϰ���
 * @param      n �Ѵ�����ʸ��
 * @return     ���Ȥλ���
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
 * @brief  ����ʸ�����·�����ƶ���ʸ�����ִ�
 *
 *  ���Ȥ�����ʸ����ο�ʿ����ʸ�� '\t' ��
 *  tab_width ���ͤ˷�·���򤷤ƶ���ʸ�����ִ����ޤ���
 *
 * @param      tab_width ����������ά����8
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * 
 */
tstring &tstring::expand_tabs( size_t tab_width )
{
    this->cleanup_shallow_copy(true);    /* �Хåե���ľ�����뤿��Ƥ� */

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
 * @brief  ����ʸ�����·�����ƥ���ʸ�����ִ�
 *
 *  ���Ȥ�����ʸ����� 2 ʸ���ʾ�Ϣ³��������ʸ�� ' ' ���٤Ƥ��оݤˤ���
 *  ���ꤷ�������� tab_width �Ƿ�·������ '\t' ���ִ����ޤ���
 *
 * @param      tab_width ����������ά����8
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * @note       tab width=1 �����ꤷ������ư���̤����Ǥ���
 *
 */
tstring &tstring::contract_spaces( size_t tab_width )
{
    this->cleanup_shallow_copy(true);    /* �Хåե���ľ�����뤿��Ƥ� */

    size_t i,j,len;
    if ( tab_width < 1 ) tab_width = 8;
    /* ��ö��Ÿ������(1ʸ��ʬ��'\t'�ˤĤ��ƤϤ��Τޤ޻Ĥ�) */
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
    /* '\t' ��Ȥäƽ��󤹤� */
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
 * @brief  2�ĤΥ��֥������ȴ֤Ǥ����ƤΥ���å�
 *
 *  ���֥�������sobj �����Ƥȼ��Ȥ����ƤȤ������ؤ��ޤ���
 *
 * @param      sobj ���Ȥ����Ƥ������ؤ��륪�֥�������
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * @throw      �����˲��򵯤��������
 *
 */
tstring &tstring::swap( tstring &sobj )
{
    if ( &sobj == this ) return *this;

    /* �Хåե���ľ�����뤿��Ƥ� */
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
 * @brief  ʸ����Υݥ�������򡤶��ڤ�ʸ���� delim �Ƿ��
 *
 *  arr �ǻ��ꤷ��ʸ����Υݥ�������(NULL��ü)���ɤ߹��ߡ�
 *  ���ڤ�ʸ���� delim �Ƿ�礷�Ƽ��Ȥ˳�Ǽ���ޤ���
 *
 * @param      arr ʸ����Υݥ����������Ƭ���ɥ쥹
 * @param      delim ���ڤ�ʸ����
 * @return     ���Ȥλ���
 * @throw      ���֥���������ΥХåե��γ��ݤ˼��Ԥ������
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

    /* ����Ĺ��Ĵ�٤� */
    for ( i=0 ; arr[i] != NULL ; i++ );

    /* ����ʤ�� */
    if ( i == 0 ) {
	this->resize(0);
	goto quit;
    }

    /* ��ʸ�����Ĺ������¸�������� */
    if ( len_buf.allocate(i) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","malloc() failed");
    }

    //err_report1(__FUNCTION__,"DEBUG","len of arr = %zd",i);

    /* ��ʸ�����Ĺ����Ĵ�٤� */
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
 * @brief  �Х��ʥ�ǡ����μ�����
 *
 *  buf �ǻ��ꤷ���Хåե����� bufsize �Х��Ȥ��ɤ߹��ߡ����Ȥ˳�Ǽ���ޤ���
 *
 * @param      buf �桼���Хåե��Υ��ɥ쥹
 * @param      bufsize �桼���Хåե��Υ�����
 * @param      altchr �桼���Хåե���ʸ�� '\0' ��¸�ߤ��������֤�������ʸ��
 * @return     ���Ȥλ���
 * @throw      ���֥���������ΥХåե��γ��ݤ˼��Ԥ������
 * @attention  altchr ��̵����ޤ��� '\0' �����ꤵ�줿��硤�Ƽ︡�������ʤɤ�
 *             ��̤�̤����Ǥ���
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
 * @brief  ����Ĺ�Хåե��⡼�ɤǤ�ʸ����Ĺ����ι���
 *
 *  ����Ĺ�Хåե��⡼�ɤǡ��桼���ץ���ब���֥������Ȥ��������Ƥ���
 *  �Хåե��˻����񤭤������ˡ����֥������Ȥ�ʸ����Ĺ�ξ���򹹿����롥
 *
 * @return     ���Ȥλ���
 *
 */
tstring &tstring::update_length()
{
    if ( this->str_length_rec != UNDEF ) {	/* ����Ĺ�Хåե��ξ�� */
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
 * @brief  ʸ����γ����Хåե��ؤΥ��ԡ�
 *
 *  ���Ȥ�ʸ����򡤳����Хåե�dest_str �˥��ԡ����ޤ���
 *
 * @param      pos ���ԡ��γ��ϰ���
 * @param      dest_str ���ԡ���γ����Хåե����ɥ쥹
 * @param      buf_size �����Хåե��Υ�����
 * @return     �����������������<br>
 *             ���Ԥ�����������
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
 * @brief  ʸ����γ����Хåե��ؤΥ��ԡ�
 *
 *  ���Ȥ�ʸ����򡤳����Хåե�dest_str �˥��ԡ����ޤ���
 *
 * @param      dest_str ���ԡ���γ����Хåե����ɥ쥹
 * @param      buf_size �����Хåե��Υ�����
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tstring::getstr( char *dest_str, size_t buf_size ) const
{
    return this->getstr(0,dest_str,buf_size);
}

/**
 * @brief  ʸ����ΰ����ޤ������Ƥ򡤻��ꥪ�֥������Ȥإ��ԡ�
 * 
 *  ���Ȥ�ʸ����Τ��٤Ƥޤ��ϰ�����dest �ǻ��ꤵ�줿���֥������Ȥ˥��ԡ�
 *  ���ޤ���
 *
 * @param      pos ���ԡ�����ʸ����γ��ϰ���
 * @param      n ���ԡ�����ʸ����
 * @param      dest ���ԡ���γ���tstring���饹�Υ��֥�������
 * @return     �����������������<br>
 *             ���Ԥ�����������
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
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
 * @brief  ʸ����ΰ����ޤ������Ƥ򡤻��ꥪ�֥������Ȥإ��ԡ�
 *
 *  ���Ȥ�ʸ����Τ��٤Ƥޤ��ϰ�����dest �ǻ��ꤵ�줿���֥������Ȥ˥��ԡ�
 *  ���ޤ���
 *
 * @param      pos ���ԡ�����ʸ����γ��ϰ���
 * @param      dest ���ԡ���γ���tstring���饹�Υ��֥�������
 * @return     �����������������<br>
 *             ���Ԥ�����������
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
ssize_t tstring::copy( size_t pos, tstring *dest ) const
{
    return this->copy(pos, this->length(), dest);
}

/**
 * @brief  ���Ȥ����Ƥ���ꥪ�֥������Ȥإ��ԡ�
 *
 *  ���Ȥ����Ƥ򡤻��ꤵ�줿���֥������� dest �˥��ԡ����ޤ���<br>
 *  ��=�ױ黻�Ҥ� .init(obj) �Ȥϰۤʤꡤ��� deep copy ���¹Ԥ���ޤ���
 *
 * @param      dest ���ԡ���γ���tstring���饹�Υ��֥�������
 * @return     �����������������<br>
 *             ���Ԥ�����������
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
ssize_t tstring::copy( tstring *dest ) const
{
    if ( dest == NULL ) return -1;
    if ( dest == this ) return dest->length();

    if ( dest->str_length_rec != UNDEF ) {	/* ����Ĺ�Хåե��ξ�� */

	dest->__deep_init(*this);

    }
    else {

	/* shallow copy �ط��Υ��꡼�󥢥å� */
	dest->cleanup_shallow_copy(false);

	dest->__deep_init(*this);
	     
    }

    return dest->length();
}

/**
 * @brief  ���Ȥ����Ƥ���ꥪ�֥������Ȥإ��ԡ� (��侩)
 * @deprecated  ��侩��<br>
 * ssize_t tstring::copy( size_t pos, size_t n, tstring *dest ) const <br>
 * �򤪻Ȥ�����������
 */
ssize_t tstring::copy( size_t pos, size_t n, tstring &dest ) const
{
    return this->copy(pos, n, &dest);
}

/**
 * @brief  ���Ȥ����Ƥ���ꥪ�֥������Ȥإ��ԡ� (��侩)
 * @deprecated  ��侩��<br>
 * ssize_t tstring::copy( size_t pos, tstring *dest ) const <br>
 * �򤪻Ȥ�����������
 */
ssize_t tstring::copy( size_t pos, tstring &dest ) const
{
    return this->copy(pos, &dest);
}

/**
 * @brief  ���Ȥ����Ƥ���ꥪ�֥������Ȥإ��ԡ� (��侩)
 * @deprecated  ��侩��<br>
 * ssize_t tstring::copy( tstring *dest ) const <br>
 * �򤪻Ȥ�����������
 */
ssize_t tstring::copy( tstring &dest ) const
{
    return this->copy(&dest);
}

/**
 * @brief  ���Ȥ����Ƥ���ꥪ�֥������Ȥذܴ�
 *
 *  ���Ȥ����Ƥ�dest �ˤ����ꤵ�줿���֥������Ȥءְܴɡפ��ޤ���
 *  �ܴɤη�̡����Ȥ�ʸ����Ĺ�ϥ���ˤʤ�ޤ���<br>
 *  dest �ˤĤ��Ƥ�ʸ����Хåե��κƳ��ݤϹԤʤ�줺�����Ȥ�ʸ����Хåե�
 *  �ˤĤ��Ƥδ������¤� dest �˾��Ϥ�������ˤʤäƤ��ꡤ��®��ư��ޤ���
 *  ������������Ĺ�Хåե��⡼�ɻ��ˤϡ��ְܴɡפǤϤʤ������ԡ� & �õ�Ȥ���
 *  ư��ޤ���
 *
 * @param     dest �ܴɡ����ԡ���Υ��֥������ȤΥ��ɥ쥹
 * @return    ���Ȥλ���
 * @throw     �Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤��������
 */
tstring &tstring::cut( tstring *dest )
{
    if ( dest == NULL ) {
	this->init();
    }
    /* dest �����Ȥʤ鲿�⤷�ʤ� */
    else if ( dest != this ) {

	if ( dest->str_length_rec != UNDEF ) {	/* ����Ĺ�Хåե��ξ�� */

	    dest->__deep_init(*this);
	    this->init();

	}
	else {

	    /* �Хåե���ľ�����뤿��Ƥ� */
	    this->cleanup_shallow_copy(true);
	    dest->cleanup_shallow_copy(false);

	    /* shallow copy �ε�ǽ��ȤäƥХåե����ɥ쥹�򥳥ԡ� */
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
 * @brief  ʸ�����10�ʤ�������(int��)���Ѵ�
 *
 *  ���Ȥ�����ʸ����ΰ���pos �ʹߤ�ʸ����10 �ʤ������ͤ��Ѵ������֤��ޤ���
 *
 * @param      pos ���Ȥ�����ʸ������Ѵ����ϰ���
 * @return     ����
 * @note       [0-9] �ʳ���ʸ�������줿�������Ѵ��Ͻ�λ���ޤ�
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
 * @brief  ʸ�����10�ʤ�������(int��)���Ѵ�
 *
 *  ���Ȥ�����ʸ����ΰ���pos �ʹߤ�ʸ����10 �ʤ������ͤ��Ѵ������֤��ޤ���
 *
 * @param      pos ���Ȥ�����ʸ������Ѵ����ϰ���
 * @param      n �����ͤ��Ѵ�����ʸ����
 * @return     ����
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * @note       [0-9] �ʳ���ʸ�������줿�������Ѵ��Ͻ�λ���ޤ�
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
 * @brief  ʸ�����10�ʤ�������(long��)���Ѵ�
 *
 *  ���Ȥ�����ʸ����ΰ���pos �ʹߤ�ʸ����10 �ʤ������ͤ��Ѵ������֤��ޤ���
 *
 * @param      pos ���Ȥ�����ʸ������Ѵ����ϰ���
 * @return     ����
 * @note       [0-9] �ʳ���ʸ�������줿�������Ѵ��Ͻ�λ���ޤ�
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
 * @brief  ʸ�����10�ʤ�������(long��)���Ѵ�
 *
 *  ���Ȥ�����ʸ����ΰ���pos �ʹߤ�ʸ����10 �ʤ������ͤ��Ѵ������֤��ޤ���
 *
 * @param      pos ���Ȥ�����ʸ������Ѵ����ϰ���
 * @param      n �����ͤ��Ѵ�����ʸ����
 * @return     ����
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * @note       [0-9] �ʳ���ʸ�������줿�������Ѵ��Ͻ�λ���ޤ�
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
 * @brief  ʸ�����10�ʤ�������(long long��)���Ѵ�
 *
 *  ���Ȥ�����ʸ����ΰ���pos �ʹߤ�ʸ����10 �ʤ������ͤ��Ѵ������֤��ޤ���
 *
 * @param      pos ���Ȥ�����ʸ������Ѵ����ϰ���
 * @return     ����
 * @note       [0-9] �ʳ���ʸ�������줿�������Ѵ��Ͻ�λ���ޤ�
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
 * @brief  ʸ�����10�ʤ�������(long long��)���Ѵ�
 *
 *  ���Ȥ�����ʸ����ΰ���pos �ʹߤ�ʸ����10 �ʤ������ͤ��Ѵ������֤��ޤ���
 *
 * @param      pos ���Ȥ�����ʸ������Ѵ����ϰ���
 * @param      n �����ͤ��Ѵ�����ʸ����
 * @return     ����
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * @note       [0-9] �ʳ���ʸ�������줿�������Ѵ��Ͻ�λ���ޤ�
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
 * @brief  ʸ�����¿���(float��)���Ѵ�
 *
 *  ���Ȥ�����ʸ����ΰ���pos �ʹߤ�ʸ����¿��ͤ��Ѵ������֤��ޤ���
 *
 * @param      pos ���Ȥ�����ʸ������Ѵ����ϰ���
 * @return     �¿�
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * @note       ������Ф���ͭ���Ǥʤ����������줿�������Ѵ��Ͻ�λ���ޤ���
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
 * @brief  ʸ�����¿���(float��)���Ѵ�
 *
 *  ���Ȥ�����ʸ����ΰ���pos �ʹߤ�ʸ����¿��ͤ��Ѵ������֤��ޤ���
 *
 * @param      pos ���Ȥ�����ʸ������Ѵ����ϰ���
 * @param      n �¿��ͤ��Ѵ�����ʸ����
 * @return     �¿�
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * @note       ������Ф���ͭ���Ǥʤ����������줿�������Ѵ��Ͻ�λ���ޤ���
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
 * @brief  ʸ�������ꤵ�줿�����������(long��)���Ѵ�
 *
 *  ���Ȥ�ʸ����򡤴��base �������ͤ��Ѵ������֤��ޤ���
 *
 * @param      base ���
 * @param      endpos ���Ȥ�ʸ������Ѵ�����ʤ��ä�ʸ���ΰ���
 * @return     ����
 *
 */
long tstring::strtol( int base, size_t *endpos ) const
{
    return this->strtol(0,base,endpos);
}

/**
 * @brief  ʸ�������ꤵ�줿�����������(long��)���Ѵ�
 *
 *  ���Ȥ�ʸ����򡤴��base �������ͤ��Ѵ������֤��ޤ���
 *
 * @param      pos ���Ȥ�����ʸ������Ѵ����ϰ���
 * @param      base ���
 * @param      endpos ���Ȥ�ʸ������Ѵ�����ʤ��ä�ʸ���ΰ���
 * @return     ����
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
 * @brief  ʸ�������ꤵ�줿�����������(long��)���Ѵ�
 *
 *  ���Ȥ�ʸ����򡤴��base �������ͤ��Ѵ������֤��ޤ���
 *
 * @param      pos ���Ȥ�����ʸ������Ѵ����ϰ���
 * @param      n �����ͤ��Ѵ�����ʸ����
 * @param      base ���
 * @param      endpos ���Ȥ�ʸ������Ѵ�����ʤ��ä�ʸ���ΰ���
 * @return     ����
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
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
 * @brief  ʸ�������ꤵ�줿�����������(long long��)���Ѵ�
 *
 *  ���Ȥ�ʸ����򡤴��base �������ͤ��Ѵ������֤��ޤ���
 *
 * @param      base ���
 * @param      endpos ���Ȥ�ʸ������Ѵ�����ʤ��ä�ʸ���ΰ���
 * @return     ����
 *
 */
long long tstring::strtoll( int base, size_t *endpos ) const
{
    return this->strtoll(0,base,endpos);
}

/**
 * @brief  ʸ�������ꤵ�줿�����������(long long��)���Ѵ�
 *
 *  ���Ȥ�ʸ����򡤴��base �������ͤ��Ѵ������֤��ޤ���
 *
 * @param      pos ���Ȥ�����ʸ������Ѵ����ϰ���
 * @param      base ���
 * @param      endpos ���Ȥ�ʸ������Ѵ�����ʤ��ä�ʸ���ΰ���
 * @return     ����
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
 * @brief  ʸ�������ꤵ�줿�����������(long long��)���Ѵ�
 *
 *  ���Ȥ�ʸ����򡤴��base �������ͤ��Ѵ������֤��ޤ���
 *
 * @param      pos ���Ȥ�����ʸ������Ѵ����ϰ���
 * @param      n �����ͤ��Ѵ�����ʸ����
 * @param      base ���
 * @param      endpos ���Ȥ�ʸ������Ѵ�����ʤ��ä�ʸ���ΰ���
 * @return     ����
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
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
 * @brief  ʸ�������ꤵ�줿��������̵��������(unsigned long��)���Ѵ�
 *
 *  ���Ȥ�ʸ����򡤴��base �����̵�������ͤ��Ѵ������֤��ޤ���
 *
 * @param      base ���
 * @param      endpos ���Ȥ�ʸ������Ѵ�����ʤ��ä�ʸ���ΰ���
 * @return     ����
 *
 */
unsigned long tstring::strtoul( int base, size_t *endpos ) const
{
    return this->strtoul(0,base,endpos);
}

/**
 * @brief  ʸ�������ꤵ�줿��������̵��������(unsigned long��)���Ѵ�
 *
 *  ���Ȥ�ʸ����򡤴��base �����̵�������ͤ��Ѵ������֤��ޤ���
 *
 * @param      pos ���Ȥ�����ʸ������Ѵ����ϰ���
 * @param      base ���
 * @param      endpos ���Ȥ�ʸ������Ѵ�����ʤ��ä�ʸ���ΰ���
 * @return     ����
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
 * @brief  ʸ�������ꤵ�줿��������̵��������(unsigned long��)���Ѵ�
 *
 *  ���Ȥ�ʸ����򡤴��base �����̵�������ͤ��Ѵ������֤��ޤ���
 *
 * @param      pos ���Ȥ�����ʸ������Ѵ����ϰ���
 * @param      n �����ͤ��Ѵ�����ʸ����
 * @param      base ���
 * @param      endpos ���Ȥ�ʸ������Ѵ�����ʤ��ä�ʸ���ΰ���
 * @return     ����
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
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
 * @brief  ʸ�������ꤵ�줿��������̵��������(unsigned long long��)���Ѵ�
 *
 *  ���Ȥ�ʸ����򡤴��base �����̵�������ͤ��Ѵ������֤��ޤ���
 *
 * @param      base ���
 * @param      endpos ���Ȥ�ʸ������Ѵ�����ʤ��ä�ʸ���ΰ���
 * @return     ����
 *
 */
unsigned long long tstring::strtoull( int base, size_t *endpos ) const
{
    return this->strtoull(0,base,endpos);
}

/**
 * @brief  ʸ�������ꤵ�줿��������̵��������(unsigned long long��)���Ѵ�
 *
 *  ���Ȥ�ʸ����򡤴��base �����̵�������ͤ��Ѵ������֤��ޤ���
 *
 * @param      pos ���Ȥ�����ʸ������Ѵ����ϰ���
 * @param      base ���
 * @param      endpos ���Ȥ�ʸ������Ѵ�����ʤ��ä�ʸ���ΰ���
 * @return     ����
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
 * @brief  ʸ�������ꤵ�줿��������̵��������(unsigned long long��)���Ѵ�
 *
 * ���Ȥ�ʸ����򡤴��base �����̵�������ͤ��Ѵ������֤��ޤ���
 *
 * @param      pos ���Ȥ�����ʸ������Ѵ����ϰ���
 * @param      n �����ͤ��Ѵ�����ʸ����
 * @param      base ���
 * @param      endpos ���Ȥ�ʸ������Ѵ�����ʤ��ä�ʸ���ΰ���
 * @return     ����
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
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
 * @brief  ʸ�����¿���(double��)���Ѵ�
 *
 *  ���Ȥ�ʸ����򡤼¿��ͤ��Ѵ������֤��ޤ���<br>
 *  �Ѵ�����ʤ��ä�ʸ����ΰ��֤�endpos���֤��ޤ���
 *
 * @param      endpos ���Ȥ�ʸ������Ѵ�����ʤ��ä�ʸ���ΰ���
 * @return     �¿�
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * @note       FORTRAN�����Ρ�1.23D12�ס�1.23d12�פΤ褦�ˡ���e�פΤ�����
 *             ��d�פ��Ȥ��Ƥ�����ˤ��б����Ƥ��ޤ���
 *
 */
double tstring::strtod( size_t *endpos ) const
{
    return this->strtod(0,endpos);
}

/**
 * @brief  ʸ�����¿���(double��)���Ѵ�
 *
 *  ���Ȥ�ʸ����򡤼¿��ͤ��Ѵ������֤��ޤ���<br>
 *  �Ѵ�����ʤ��ä�ʸ����ΰ��֤�endpos���֤��ޤ���
 *
 * @param      pos ���Ȥ�����ʸ������Ѵ����ϰ���
 * @param      endpos ���Ȥ�ʸ������Ѵ�����ʤ��ä�ʸ���ΰ���
 * @return     �¿�
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * @note       FORTRAN�����Ρ�1.23D12�ס�1.23d12�פΤ褦�ˡ���e�פΤ�����
 *             ��d�פ��Ȥ��Ƥ�����ˤ��б����Ƥ��ޤ���
 *
 */
double tstring::strtod( size_t pos, size_t *endpos ) const
{
    const char *this_str_ptr = this->str_ptr_cs();
    if ( this_str_ptr != NULL && pos < this->length() ) {
	char *endptr;
	double ret = c_strtod(this_str_ptr + pos ,&endptr);
	/* 1.23D12 �η����Ǥ⤤����褦�ˤ��� */
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
 * @brief  ʸ�����¿���(double��)���Ѵ�
 *
 *  ���Ȥ�ʸ����򡤼¿��ͤ��Ѵ������֤��ޤ���<br>
 *  �ޤ����Ѵ�����ʤ��ä�ʸ����ΰ��֤�endpos���֤��ޤ���
 *
 * @param      pos ���Ȥ�����ʸ������Ѵ����ϰ���
 * @param      n �¿��ͤ��Ѵ�����ʸ����
 * @param      endpos ���Ȥ�ʸ������Ѵ�����ʤ��ä�ʸ���ΰ���
 * @return     �¿�
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * @note       FORTRAN�����Ρ�1.23D12�ס�1.23d12�פΤ褦�ˡ���e�פΤ�����
 *             ��d�פ��Ȥ��Ƥ�����ˤ��б����Ƥ��ޤ���
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
	/* 1.23D12 �η����Ǥ⤤����褦�ˤ��� */
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

/* �ߴ��� */
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
 * @brief  ʸ����� format �λ���˽��ä��Ѵ�
 *
 *  ���Ȥ�ʸ����� format �λ���˽��ä��Ѵ�����format �ʹߤΰ����˳�Ǽ���ޤ���
 *
 * @param      format �ɤ߹��ߥե����ޥåȻ���
 * @param      ap �񤭹�����Ȥʤ����Ĺ�����Υꥹ��
 * @return     �����������������<br>
 *             ���Ԥ�������EOF
 * @attention  format �� "%s" �����ꤵ�줿��硤��Ǽ�Хåե����礭���ʾ��
 *             ʸ�������Ϥ����ȥХåե������С����ȯ�����ޤ���
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
 * @brief  ʸ����� format �λ���˽��ä��Ѵ�
 *
 *  ���Ȥ�ʸ�����format �λ���˽��ä��Ѵ�����format �ʹߤΰ����˳�Ǽ���ޤ���
 *
 * @param      format �ɤ߹��ߥե����ޥåȻ���
 * @param      ... �񤭹�����Ȥʤ����Ĺ�����γ����ǥǡ���
 * @return     �����������������<br>
 *             ���Ԥ�������EOF
 * @attention  format �� "%s" �����ꤵ�줿��硤��Ǽ�Хåե����礭���ʾ��
 *             ʸ�������Ϥ����ȥХåե������С����ȯ�����ޤ���
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
 * @brief  ʸ����μ���Ū�����
 *
 *  ���Ȥ�ʸ����Ȼ��ꤵ�줿ʸ���� str �Ȥ򼭽�Ū����Ӥ������η�̤��֤��ޤ���
 *
 * @param   pos ���Ȥ�ʸ����γ��ϰ���
 * @param   str ��Ӥ��Ѥ���ʸ����
 * @param   n ��Ӥ���ʸ����
 * @return  ���Ȥ�ʸ����str ������������0
 * @return  ���Ȥ�ʸ����str �˳Ӥ٤Ƽ���Ū���礭����������
 * @return  ���Ȥ�ʸ����str �˳Ӥ٤Ƽ���Ū�˾�������������
 * @return  ���Ȥ�ʸ����Хåե��������str �� NULL ����ꤷ������256(���顼)
 * @return  ���Ȥ�ʸ����Хåե����������str ����ꤷ������-256(���顼)
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
 * @brief  ʸ����μ���Ū�����
 *
 *  ���Ȥ�ʸ����Ȼ��ꤵ�줿ʸ���� str �Ȥ򼭽�Ū����Ӥ������η�̤��֤��ޤ���
 *
 * @param   pos ���Ȥ�����ʸ����γ��ϰ���
 * @param   str ��Ӥ��Ѥ���ʸ����
 * @return  ���Ȥ�ʸ����str ������������0
 * @return  ���Ȥ�ʸ����str �˳Ӥ٤Ƽ���Ū���礭����������
 * @return  ���Ȥ�ʸ����str �˳Ӥ٤Ƽ���Ū�˾�������������
 * @return  ���Ȥ�ʸ����Хåե��������str �� NULL ����ꤷ������256(���顼)
 * @return  ���Ȥ�ʸ����Хåե����������str ����ꤷ������-256(���顼)
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
 * @brief  ʸ����μ���Ū�����
 *
 *  ���Ȥ�ʸ����Ȼ��ꤵ�줿ʸ���� str �Ȥ򼭽�Ū����Ӥ������η�̤��֤��ޤ���
 *
 * @param   str ��Ӥ��Ѥ���ʸ����
 * @param   n ��Ӥ���ʸ����
 * @return  ���Ȥ�ʸ����str ������������0
 * @return  ���Ȥ�ʸ����str �˳Ӥ٤Ƽ���Ū���礭����������
 * @return  ���Ȥ�ʸ����str �˳Ӥ٤Ƽ���Ū�˾�������������
 * @return  ���Ȥ�ʸ����Хåե��������str �� NULL ����ꤷ������256(���顼)
 * @return  ���Ȥ�ʸ����Хåե����������str ����ꤷ������-256(���顼)
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
 * @brief  ʸ����μ���Ū�����
 *
 *  ���Ȥ�ʸ����Ȼ��ꤵ�줿ʸ���� str �Ȥ򼭽�Ū����Ӥ������η�̤��֤��ޤ���
 *
 * @param   str ��Ӥ��Ѥ���ʸ����
 * @return  ���Ȥ�ʸ����str ������������0
 * @return  ���Ȥ�ʸ����str �˳Ӥ٤Ƽ���Ū���礭����������
 * @return  ���Ȥ�ʸ����str �˳Ӥ٤Ƽ���Ū�˾�������������
 * @return  ���Ȥ�ʸ����Хåե��������str �� NULL ����ꤷ������256(���顼)
 * @return  ���Ȥ�ʸ����Хåե����������str ����ꤷ������-256(���顼)
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
 * @brief  ʸ����μ���Ū�����
 *
 *  ���Ȥ�ʸ����Ȼ��ꤵ�줿ʸ���� str �Ȥ򼭽�Ū����Ӥ������η�̤��֤��ޤ���
 *
 * @param   pos1 ���Ȥ�����ʸ����γ��ϰ���
 * @param   str ��Ӥ��Ѥ���ʸ����
 * @param   pos2 ���֥�������str ������ʸ����γ��ϰ���
 * @return  ���Ȥ�ʸ����str ������������0
 * @return  ���Ȥ�ʸ����str �˳Ӥ٤Ƽ���Ū���礭����������
 * @return  ���Ȥ�ʸ����str �˳Ӥ٤Ƽ���Ū�˾�������������
 * @return  ���Ȥ�ʸ����Хåե��������str �� NULL ����ꤷ������256(���顼)
 * @return  ���Ȥ�ʸ����Хåե����������str ����ꤷ������-256(���顼)
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
 * @brief  ʸ����μ���Ū�����
 *
 *  ���Ȥ�ʸ����Ȼ��ꤵ�줿ʸ���� str �Ȥ򼭽�Ū����Ӥ������η�̤��֤��ޤ���
 *
 * @param   pos1 ���Ȥ�ʸ����γ��ϰ���
 * @param   str ��Ӥ��Ѥ���ʸ����
 * @param   pos2 str ������ʸ����γ��ϰ���
 * @param   n ��Ӥ���ʸ����
 * @return  ���Ȥ�ʸ����str ������������0
 * @return  ���Ȥ�ʸ����str �˳Ӥ٤Ƽ���Ū���礭����������
 * @return  ���Ȥ�ʸ����str �˳Ӥ٤Ƽ���Ū�˾�������������
 * @return  ���Ȥ�ʸ����Хåե��������str �� NULL ����ꤷ������256(���顼)
 * @return  ���Ȥ�ʸ����Хåե����������str ����ꤷ������-256(���顼)
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
 * @brief  ʸ����μ���Ū�����
 *
 *  ���Ȥ�ʸ����Ȼ��ꤵ�줿ʸ���� str �Ȥ򼭽�Ū����Ӥ������η�̤��֤��ޤ���
 *
 * @param   str ��Ӥ��Ѥ���ʸ����
 * @param   pos2 ���֥�������str ������ʸ����γ��ϰ���
 * @return  ���Ȥ�ʸ����str ������������0
 * @return  ���Ȥ�ʸ����str �˳Ӥ٤Ƽ���Ū���礭����������
 * @return  ���Ȥ�ʸ����str �˳Ӥ٤Ƽ���Ū�˾�������������
 * @return  ���Ȥ�ʸ����Хåե��������str �� NULL ����ꤷ������256(���顼)
 * @return  ���Ȥ�ʸ����Хåե����������str ����ꤷ������-256(���顼)
 * 
 */
int tstring::compare( const tstring &str, size_t pos2 ) const
{
    return this->compare(0,str,pos2);
}

/**
 * @brief  ʸ����μ���Ū�����
 *
 *  ���Ȥ�ʸ����Ȼ��ꤵ�줿ʸ���� str �Ȥ򼭽�Ū����Ӥ������η�̤��֤��ޤ���
 *
 * @param   str ��Ӥ��Ѥ���ʸ����
 * @param   pos2 str ������ʸ����γ��ϰ���
 * @param   n ��Ӥ���ʸ����
 * @return  ���Ȥ�ʸ����str ������������0
 * @return  ���Ȥ�ʸ����str �˳Ӥ٤Ƽ���Ū���礭����������
 * @return  ���Ȥ�ʸ����str �˳Ӥ٤Ƽ���Ū�˾�������������
 * @return  ���Ȥ�ʸ����Хåե��������str �� NULL ����ꤷ������256(���顼)
 * @return  ���Ȥ�ʸ����Хåե����������str ����ꤷ������-256(���顼)
 * 
 */
int tstring::compare( const tstring &str, size_t pos2, size_t n ) const
{
    return this->compare(0,str,pos2,n);
}

/**
 * @brief  ʸ����μ���Ū�����
 *
 *  ���Ȥ�ʸ����Ȼ��ꤵ�줿ʸ���� str �Ȥ򼭽�Ū����Ӥ������η�̤��֤��ޤ���
 *
 * @param   pos ���Ȥ�ʸ����γ��ϰ���
 * @param   str ��Ӥ��Ѥ���ʸ����
 * @param   n ��Ӥ���ʸ����
 * @return  ���Ȥ�ʸ����str ������������0
 * @return  ���Ȥ�ʸ����str �˳Ӥ٤Ƽ���Ū���礭����������
 * @return  ���Ȥ�ʸ����str �˳Ӥ٤Ƽ���Ū�˾�������������
 * @return  ���Ȥ�ʸ����Хåե��������str �� NULL ����ꤷ������256(���顼)
 * @return  ���Ȥ�ʸ����Хåե����������str ����ꤷ������-256(���顼)
 * 
 */
int tstring::strncmp( size_t pos, const char *str, size_t n ) const
{
    return this->compare(pos,str,n);
}

/**
 * @brief  ʸ����μ���Ū�����
 *
 *  ���Ȥ�ʸ����Ȼ��ꤵ�줿ʸ���� str �Ȥ򼭽�Ū����Ӥ������η�̤��֤��ޤ���
 *
 * @param   pos ���Ȥ�����ʸ����γ��ϰ���
 * @param   str ��Ӥ��Ѥ���ʸ����
 * @return  ���Ȥ�ʸ����str ������������0
 * @return  ���Ȥ�ʸ����str �˳Ӥ٤Ƽ���Ū���礭����������
 * @return  ���Ȥ�ʸ����str �˳Ӥ٤Ƽ���Ū�˾�������������
 * @return  ���Ȥ�ʸ����Хåե��������str �� NULL ����ꤷ������256(���顼)
 * @return  ���Ȥ�ʸ����Хåե����������str ����ꤷ������-256(���顼)
 * 
 */
int tstring::strcmp( size_t pos, const char *str ) const
{
    return this->compare(pos,str);
}

/**
 * @brief  ʸ����μ���Ū�����
 *
 *  ���Ȥ�ʸ����Ȼ��ꤵ�줿ʸ���� str �Ȥ򼭽�Ū����Ӥ������η�̤��֤��ޤ���
 *
 * @param   str ��Ӥ��Ѥ���ʸ����
 * @param   n ��Ӥ���ʸ����
 * @return  ���Ȥ�ʸ����str ������������0
 * @return  ���Ȥ�ʸ����str �˳Ӥ٤Ƽ���Ū���礭����������
 * @return  ���Ȥ�ʸ����str �˳Ӥ٤Ƽ���Ū�˾�������������
 * @return  ���Ȥ�ʸ����Хåե��������str �� NULL ����ꤷ������256(���顼)
 * @return  ���Ȥ�ʸ����Хåե����������str ����ꤷ������-256(���顼)
 * 
 */
int tstring::strncmp( const char *str, size_t n ) const
{
    return this->compare(str,n);
}

/**
 * @brief  ʸ����μ���Ū�����
 *
 *  ���Ȥ�ʸ����Ȼ��ꤵ�줿ʸ���� str �Ȥ򼭽�Ū����Ӥ������η�̤��֤��ޤ���
 *
 * @param   str ��Ӥ��Ѥ���ʸ����
 * @return  ���Ȥ�ʸ����str ������������0
 * @return  ���Ȥ�ʸ����str �˳Ӥ٤Ƽ���Ū���礭����������
 * @return  ���Ȥ�ʸ����str �˳Ӥ٤Ƽ���Ū�˾�������������
 * @return  ���Ȥ�ʸ����Хåե��������str �� NULL ����ꤷ������256(���顼)
 * @return  ���Ȥ�ʸ����Хåե����������str ����ꤷ������-256(���顼)
 * 
 */
int tstring::strcmp( const char *str ) const
{
    return this->compare(str);
}

/**
 * @brief  ʸ����μ���Ū�����
 *
 *  ���Ȥ�ʸ����Ȼ��ꤵ�줿ʸ���� str �Ȥ򼭽�Ū����Ӥ������η�̤��֤��ޤ���
 *
 * @param   pos1 ���Ȥ�����ʸ����γ��ϰ���
 * @param   str ��Ӥ��Ѥ���ʸ����
 * @param   pos2 ���֥�������str ������ʸ����γ��ϰ���
 * @return  ���Ȥ�ʸ����str ������������0
 * @return  ���Ȥ�ʸ����str �˳Ӥ٤Ƽ���Ū���礭����������
 * @return  ���Ȥ�ʸ����str �˳Ӥ٤Ƽ���Ū�˾�������������
 * @return  ���Ȥ�ʸ����Хåե��������str �� NULL ����ꤷ������256(���顼)
 * @return  ���Ȥ�ʸ����Хåե����������str ����ꤷ������-256(���顼)
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
 * @brief  ʸ����μ���Ū�����
 *
 *  ���Ȥ�ʸ����Ȼ��ꤵ�줿ʸ���� str �Ȥ򼭽�Ū����Ӥ������η�̤��֤��ޤ���
 *
 * @param   pos1 ���Ȥ�ʸ����γ��ϰ���
 * @param   str ��Ӥ��Ѥ���ʸ����
 * @param   pos2 str ������ʸ����γ��ϰ���
 * @param   n ��Ӥ���ʸ����
 * @return  ���Ȥ�ʸ����str ������������0
 * @return  ���Ȥ�ʸ����str �˳Ӥ٤Ƽ���Ū���礭����������
 * @return  ���Ȥ�ʸ����str �˳Ӥ٤Ƽ���Ū�˾�������������
 * @return  ���Ȥ�ʸ����Хåե��������str �� NULL ����ꤷ������256(���顼)
 * @return  ���Ȥ�ʸ����Хåե����������str ����ꤷ������-256(���顼)
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
 * @brief  ʸ����μ���Ū�����
 *
 *  ���Ȥ�ʸ����Ȼ��ꤵ�줿ʸ���� str �Ȥ򼭽�Ū����Ӥ������η�̤��֤��ޤ���
 *
 * @param   str ��Ӥ��Ѥ���ʸ����
 * @param   pos2 ���֥�������str ������ʸ����γ��ϰ���
 * @return  ���Ȥ�ʸ����str ������������0
 * @return  ���Ȥ�ʸ����str �˳Ӥ٤Ƽ���Ū���礭����������
 * @return  ���Ȥ�ʸ����str �˳Ӥ٤Ƽ���Ū�˾�������������
 * @return  ���Ȥ�ʸ����Хåե��������str �� NULL ����ꤷ������256(���顼)
 * @return  ���Ȥ�ʸ����Хåե����������str ����ꤷ������-256(���顼)
 * 
 */
int tstring::strcmp( const tstring &str, size_t pos2 ) const
{
    return this->strcmp(0,str,pos2);
}

/**
 * @brief  ʸ����μ���Ū�����
 *
 *  ���Ȥ�ʸ����Ȼ��ꤵ�줿ʸ���� str �Ȥ򼭽�Ū����Ӥ������η�̤��֤��ޤ���
 *
 * @param   str ��Ӥ��Ѥ���ʸ����
 * @param   pos2 str ������ʸ����γ��ϰ���
 * @param   n ��Ӥ���ʸ����
 * @return  ���Ȥ�ʸ����str ������������0
 * @return  ���Ȥ�ʸ����str �˳Ӥ٤Ƽ���Ū���礭����������
 * @return  ���Ȥ�ʸ����str �˳Ӥ٤Ƽ���Ū�˾�������������
 * @return  ���Ȥ�ʸ����Хåե��������str �� NULL ����ꤷ������256(���顼)
 * @return  ���Ȥ�ʸ����Хåե����������str ����ꤷ������-256(���顼)
 * 
 */
int tstring::strncmp( const tstring &str, size_t pos2, size_t n ) const
{
    return this->strncmp(0,str,pos2,n);
}

/**
 * @brief  ��ʸ������ʸ������̤��ʤ�ʸ����μ���Ū�����
 *
 *  ���Ȥ�ʸ����Ȼ��ꤵ�줿ʸ���� str �Ȥ򡤥���ե��٥åȤ���ʸ���Ⱦ�ʸ����
 *  ����̤�������Ū����Ӥ������η�̤��֤��ޤ���
 *
 * @param   pos1 ���Ȥ�ʸ����γ��ϰ���
 * @param   str ��Ӥ��Ѥ���ʸ����
 * @param   n ��Ӥ���ʸ����
 * @return  ���Ȥ�ʸ����str ������������0
 * @return  ���Ȥ�ʸ����str �˳Ӥ٤Ƽ���Ū���礭����������
 * @return  ���Ȥ�ʸ����str �˳Ӥ٤Ƽ���Ū�˾�������������
 * @return  ���Ȥ�ʸ����Хåե��������str �� NULL ����ꤷ������256(���顼)
 * @return  ���Ȥ�ʸ����Хåե����������str ����ꤷ������-256(���顼)
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
 * @brief  ��ʸ������ʸ������̤��ʤ�ʸ����μ���Ū�����
 *
 *  ���Ȥ�ʸ����Ȼ��ꤵ�줿ʸ���� str �Ȥ򡤥���ե��٥åȤ���ʸ���Ⱦ�ʸ����
 *  ����̤�������Ū����Ӥ������η�̤��֤��ޤ���
 *
 * @param   pos1 ���Ȥ�ʸ����γ��ϰ���
 * @param   str ��Ӥ��Ѥ���ʸ����
 * @return  ���Ȥ�ʸ����str ������������0
 * @return  ���Ȥ�ʸ����str �˳Ӥ٤Ƽ���Ū���礭����������
 * @return  ���Ȥ�ʸ����str �˳Ӥ٤Ƽ���Ū�˾�������������
 * @return  ���Ȥ�ʸ����Хåե��������str �� NULL ����ꤷ������256(���顼)
 * @return  ���Ȥ�ʸ����Хåե����������str ����ꤷ������-256(���顼)
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
 * @brief  ��ʸ������ʸ������̤��ʤ�ʸ����μ���Ū�����
 *
 *  ���Ȥ�ʸ����Ȼ��ꤵ�줿ʸ���� str �Ȥ򡤥���ե��٥åȤ���ʸ���Ⱦ�ʸ����
 *  ����̤�������Ū����Ӥ������η�̤��֤��ޤ���
 *
 * @param   str ��Ӥ��Ѥ���ʸ����
 * @param   n ��Ӥ���ʸ����
 * @return  ���Ȥ�ʸ����str ������������0
 * @return  ���Ȥ�ʸ����str �˳Ӥ٤Ƽ���Ū���礭����������
 * @return  ���Ȥ�ʸ����str �˳Ӥ٤Ƽ���Ū�˾�������������
 * @return  ���Ȥ�ʸ����Хåե��������str �� NULL ����ꤷ������256(���顼)
 * @return  ���Ȥ�ʸ����Хåե����������str ����ꤷ������-256(���顼)
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
 * @brief  ��ʸ������ʸ������̤��ʤ�ʸ����μ���Ū�����
 *
 *  ���Ȥ�ʸ����Ȼ��ꤵ�줿ʸ���� str �Ȥ򡤥���ե��٥åȤ���ʸ���Ⱦ�ʸ����
 *  ����̤�������Ū����Ӥ������η�̤��֤��ޤ���
 *
 * @param   str ��Ӥ��Ѥ���ʸ����
 * @return  ���Ȥ�ʸ����str ������������0
 * @return  ���Ȥ�ʸ����str �˳Ӥ٤Ƽ���Ū���礭����������
 * @return  ���Ȥ�ʸ����str �˳Ӥ٤Ƽ���Ū�˾�������������
 * @return  ���Ȥ�ʸ����Хåե��������str �� NULL ����ꤷ������256(���顼)
 * @return  ���Ȥ�ʸ����Хåե����������str ����ꤷ������-256(���顼)
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
 * @brief  ��ʸ������ʸ������̤��ʤ�ʸ����μ���Ū�����
 *
 *  ���Ȥ�ʸ����Ȼ��ꤵ�줿ʸ���� str �Ȥ򡤥���ե��٥åȤ���ʸ���Ⱦ�ʸ����
 *  ����̤�������Ū����Ӥ������η�̤��֤��ޤ���
 *
 * @param   pos1 ���Ȥ�ʸ����γ��ϰ���
 * @param   str ��Ӥ��Ѥ���ʸ����
 * @param   pos2 ���֥�������str ������ʸ����γ��ϰ���
 * @return  ���Ȥ�ʸ����str ������������0
 * @return  ���Ȥ�ʸ����str �˳Ӥ٤Ƽ���Ū���礭����������
 * @return  ���Ȥ�ʸ����str �˳Ӥ٤Ƽ���Ū�˾�������������
 * @return  ���Ȥ�ʸ����Хåե��������str �� NULL ����ꤷ������256(���顼)
 * @return  ���Ȥ�ʸ����Хåե����������str ����ꤷ������-256(���顼)
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
 * @brief  ��ʸ������ʸ������̤��ʤ�ʸ����μ���Ū�����
 *
 *  ���Ȥ�ʸ����Ȼ��ꤵ�줿ʸ���� str �Ȥ򡤥���ե��٥åȤ���ʸ���Ⱦ�ʸ����
 *  ����̤�������Ū����Ӥ������η�̤��֤��ޤ���
 *
 * @param   pos1 ���Ȥ�ʸ����γ��ϰ���
 * @param   str ��Ӥ��Ѥ���ʸ����
 * @param   pos2 ���֥�������str ������ʸ����γ��ϰ���
 * @param   n ��Ӥ���ʸ����
 * @return  ���Ȥ�ʸ����str ������������0
 * @return  ���Ȥ�ʸ����str �˳Ӥ٤Ƽ���Ū���礭����������
 * @return  ���Ȥ�ʸ����str �˳Ӥ٤Ƽ���Ū�˾�������������
 * @return  ���Ȥ�ʸ����Хåե��������str �� NULL ����ꤷ������256(���顼)
 * @return  ���Ȥ�ʸ����Хåե����������str ����ꤷ������-256(���顼)
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
 * @brief  ��ʸ������ʸ������̤��ʤ�ʸ����μ���Ū�����
 *
 *  ���Ȥ�ʸ����Ȼ��ꤵ�줿ʸ���� str �Ȥ򡤥���ե��٥åȤ���ʸ���Ⱦ�ʸ����
 *  ����̤�������Ū����Ӥ������η�̤��֤��ޤ���
 *
 * @param   str ��Ӥ��Ѥ���ʸ����
 * @param   pos2 ���֥�������str ������ʸ����γ��ϰ���
 * @return  ���Ȥ�ʸ����str ������������0
 * @return  ���Ȥ�ʸ����str �˳Ӥ٤Ƽ���Ū���礭����������
 * @return  ���Ȥ�ʸ����str �˳Ӥ٤Ƽ���Ū�˾�������������
 * @return  ���Ȥ�ʸ����Хåե��������str �� NULL ����ꤷ������256(���顼)
 * @return  ���Ȥ�ʸ����Хåե����������str ����ꤷ������-256(���顼)
 * 
 */
int tstring::strcasecmp( const tstring &str, size_t pos2 ) const
{
    return this->strcasecmp(0, str, pos2);
}

/**
 * @brief  ��ʸ������ʸ������̤��ʤ�ʸ����μ���Ū�����
 *
 *  ���Ȥ�ʸ����Ȼ��ꤵ�줿ʸ���� str �Ȥ򡤥���ե��٥åȤ���ʸ���Ⱦ�ʸ����
 *  ����̤�������Ū����Ӥ������η�̤��֤��ޤ���
 *
 * @param   str ��Ӥ��Ѥ���ʸ����
 * @param   pos2 ���֥�������str ������ʸ����γ��ϰ���
 * @param   n ��Ӥ���ʸ����
 * @return  ���Ȥ�ʸ����str ������������0
 * @return  ���Ȥ�ʸ����str �˳Ӥ٤Ƽ���Ū���礭����������
 * @return  ���Ȥ�ʸ����str �˳Ӥ٤Ƽ���Ū�˾�������������
 * @return  ���Ȥ�ʸ����Хåե��������str �� NULL ����ꤷ������256(���顼)
 * @return  ���Ȥ�ʸ����Хåե����������str ����ꤷ������-256(���顼)
 * 
 */
int tstring::strncasecmp( const tstring &str, size_t pos2, size_t n ) const
{
    return this->strncasecmp(0, str, pos2, 2);
}

/**
 * @brief  ��¦�����ʸ����(const char *)�θ���
 *
 *  ���Ȥ�����ʸ����κ�¦����ʸ���� str �򸡺������ǽ�˽и�������֤�
 *  �֤��ޤ���
 *
 * @param      str ���Ф���ʸ����
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tstring::strstr( const char *str ) const
{
    return this->find((size_t)0,str);
}

/**
 * @brief  ��¦�����ʸ����(const char *)�θ���
 *
 *  ���Ȥ�����ʸ����κ�¦����ʸ���� str �򸡺������ǽ�˽и�������֤�
 *  �֤��ޤ���
 *
 * @param      pos ���Ȥ�ʸ����γ��ϰ���
 * @param      str ���Ф���ʸ����
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tstring::strstr( size_t pos, const char *str ) const
{
    return this->find(pos,str);
}

/**
 * @brief  ��¦�����ʸ����(const char *)��Ϣ³Ū�ʸ���
 *
 *  ���Ȥ�����ʸ����κ�¦����ʸ���� str �򸡺������ǽ�˽и�������֤�
 *  �֤��ޤ���
 *
 * @param      pos ���Ȥ�ʸ����γ��ϰ���
 * @param      str ���Ф���ʸ����
 * @param      nextpos �����pos
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tstring::strstr( size_t pos, const char *str, size_t *nextpos ) const
{
    return this->find(pos,str,nextpos);
}

/**
 * @brief  ��¦�����ʸ����(tstring)�θ���
 *
 *  ���Ȥ�����ʸ����κ�¦����ʸ���� str �򸡺������ǽ�˽и�������֤�
 *  �֤��ޤ���
 *
 * @param      str ���Ф���ʸ����
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tstring::strstr( const tstring &str ) const
{
    return this->find((size_t)0,str.cstr());
}

/**
 * @brief  ��¦�����ʸ����(tstring)�θ���
 *
 *  ���Ȥ�����ʸ����κ�¦����ʸ���� str �򸡺������ǽ�˽и�������֤�
 *  �֤��ޤ���
 *
 * @param      pos ���Ȥ�ʸ����γ��ϰ���
 * @param      str ���Ф���ʸ����
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tstring::strstr( size_t pos, const tstring &str ) const
{
    return this->find(pos,str.cstr());
}

/**
 * @brief  ��¦�����ʸ����(tstring)��Ϣ³Ū�ʸ���
 *
 *  ���Ȥ�����ʸ����κ�¦����ʸ���� str �򸡺������ǽ�˽и�������֤�
 *  �֤��ޤ���
 *
 * @param      pos ���Ȥ�ʸ����γ��ϰ���
 * @param      str ���Ф���ʸ����
 * @param      nextpos �����pos
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tstring::strstr(size_t pos, const tstring &str, size_t *nextpos) const
{
    return this->find(pos,str.cstr(),nextpos);
}

/**
 * @brief  ��¦�����ʸ����(const char *)�θ���
 *
 *  ���Ȥ�����ʸ����κ�¦����ʸ���� str �򸡺������ǽ�˽и�������֤�
 *  �֤��ޤ���
 *
 * @param      str ���Ф���ʸ����
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tstring::find( const char *str ) const
{
    return this->find((size_t)0,str);
}

/**
 * @brief  ��¦�����ʸ����(const char *)�θ���
 *
 *  ���Ȥ�����ʸ����κ�¦����ʸ���� str �򸡺������ǽ�˽и�������֤�
 *  �֤��ޤ���
 *
 * @param      str ���Ф���ʸ����
 * @param      n ���Ф���ʸ����
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tstring::find( const char *str, size_t n ) const
{
    return this->find((size_t)0,str,n);
}

/**
 * @brief  ��¦�����ʸ����(const char *)�θ���
 *
 *  ���Ȥ�����ʸ����κ�¦����ʸ���� str �򸡺������ǽ�˽и�������֤�
 *  �֤��ޤ���
 *
 * @param      pos ���Ȥ�ʸ����γ��ϰ���
 * @param      str ���Ф���ʸ����
 * @param      nextpos �����pos
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tstring::find( size_t pos, const char *str ) const
{
    return this->find(pos,str,(size_t *)NULL);
}

/**
 * @brief  ��¦�����ʸ����(const char *)��Ϣ³Ū�ʸ���
 *
 *  ���Ȥ�����ʸ����κ�¦����ʸ���� str �򸡺������ǽ�˽и�������֤�
 *  �֤��ޤ���
 *
 * @param      pos ���Ȥ�ʸ����γ��ϰ���
 * @param      str ���Ф���ʸ����
 * @param      nextpos �����pos
 * @return     �����������������<br>
 *             ���Ԥ�����������
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
 * @brief  ��¦�����ʸ����(const char *)�θ���
 *
 *  ���Ȥ�����ʸ����κ�¦����ʸ���� str �򸡺������ǽ�˽и�������֤�
 *  �֤��ޤ���
 *
 * @param      pos ���Ȥ�ʸ����γ��ϰ���
 * @param      str ���Ф���ʸ����
 * @param      n ���Ф���ʸ����
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tstring::find( size_t pos, const char *str, size_t n ) const
{
    return this->find(pos,str,n,(size_t *)NULL);
}

/**
 * @brief  ��¦�����ʸ����(const char *)��Ϣ³Ū�ʸ���
 *
 *  ���Ȥ�����ʸ����κ�¦����ʸ���� str �򸡺������ǽ�˽и�������֤�
 *  �֤��ޤ���
 *
 * @param      pos ���Ȥ�ʸ����γ��ϰ���
 * @param      str ���Ф���ʸ����
 * @param      n ���Ф���ʸ����
 * @param      nextpos �����pos
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tstring::find( size_t pos, const char *str, size_t n,
		       size_t *nextpos ) const
{
    if ( this->cstr() != NULL && pos <= this->length() && str != NULL ) {
	size_t len, i;
	/* n ��꾮���������б� */
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
 * @brief  ��¦�����ʸ����(tstring)�θ���
 *
 *  ���Ȥ�����ʸ����κ�¦����ʸ���� str �򸡺������ǽ�˽и�������֤�
 *  �֤��ޤ���
 *
 * @param      str ���Ф���ʸ����
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tstring::find( const tstring &str ) const
{
    return this->find((size_t)0,str.cstr());
}

/**
 * @brief  ��¦�����ʸ����(tstring)�θ���
 *
 *  ���Ȥ�����ʸ����κ�¦����ʸ���� str �򸡺������ǽ�˽и�������֤�
 *  �֤��ޤ���
 *
 * @param      pos ���Ȥ�ʸ����γ��ϰ���
 * @param      str ���Ф���ʸ����
 * @param      nextpos �����pos
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tstring::find( size_t pos, const tstring &str ) const
{
    return this->find(pos,str.cstr());
}

/**
 * @brief  ��¦�����ʸ����(tstring)��Ϣ³Ū�ʸ���
 *
 *  ���Ȥ�����ʸ����κ�¦����ʸ���� str �򸡺������ǽ�˽и�������֤�
 *  �֤��ޤ���
 *
 * @param      pos ���Ȥ�ʸ����γ��ϰ���
 * @param      str ���Ф���ʸ����
 * @param      nextpos �����pos
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tstring::find( size_t pos, const tstring &str, size_t *nextpos ) const
{
    return this->find(pos,str.cstr(),nextpos);
}

/* ���ޥ� */

/**
 * @brief  printf()�ε�ˡ�ǻ��ꤵ�줿ʸ�����¦���鸡��
 *
 *  ���Ȥ�����ʸ����κ�¦���顤���ꤵ�줿ʸ����򸡺������ǽ�˽и�����
 *  ���֤��֤��ޤ������δؿ��Ǥϡ���������ʸ����� printf()�ؿ���Ʊ�ͤ�
 *  �ե����ޥåȤȲ��Ѱ����ǥ��åȤǤ��ޤ���
 *
 * @param   format ����ʸ����Τ���Υե����ޥåȻ��� 
 * @param   ... format���б���������Ĺ�����γ�����
 * @return  �����������������<br>
 *          ���Ԥ�����������
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
 * @brief  printf()�ε�ˡ�ǻ��ꤵ�줿ʸ�����¦���鸡��
 *
 *  ���Ȥ�����ʸ����κ�¦���顤���ꤵ�줿ʸ����򸡺������ǽ�˽и�����
 *  ���֤��֤��ޤ������δؿ��Ǥϡ���������ʸ����� printf()�ؿ���Ʊ�ͤ�
 *  �ե����ޥåȤȲ��Ѱ����ǥ��åȤǤ��ޤ���
 *
 * @param   format ����ʸ����Τ���Υե����ޥåȻ��� 
 * @param   ap format���б���������Ĺ�����Υꥹ��
 * @return  �����������������<br>
 *          ���Ԥ�����������
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
 * @brief  printf()�ε�ˡ�ǻ��ꤵ�줿ʸ�����¦���鸡��
 *
 *  ���Ȥ�����ʸ����� pos �ΰ��֤��顤���ꤵ�줿ʸ����򸡺������ǽ�˽и�
 *  ������֤��֤��ޤ������δؿ��Ǥϡ���������ʸ����� printf()�ؿ���Ʊ�ͤ�
 *  �ե����ޥåȤȲ��Ѱ����ǥ��åȤǤ��ޤ���
 *
 * @param   pos ���Ȥ�ʸ����γ��ϰ���
 * @param   format ����ʸ����Τ���Υե����ޥåȻ��� 
 * @param   ... format���б���������Ĺ�����γ�����
 * @return  �����������������<br>
 *          ���Ԥ�����������
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
 * @brief  printf()�ε�ˡ�ǻ��ꤵ�줿ʸ�����¦���鸡��
 *
 *  ���Ȥ�����ʸ����� pos �ΰ��֤��顤���ꤵ�줿ʸ����򸡺������ǽ�˽и�
 *  ������֤��֤��ޤ������δؿ��Ǥϡ���������ʸ����� printf()�ؿ���Ʊ�ͤ�
 *  �ե����ޥåȤȲ��Ѱ����ǥ��åȤǤ��ޤ���
 *
 * @param   pos ���Ȥ�ʸ����γ��ϰ���
 * @param   format ����ʸ����Τ���Υե����ޥåȻ��� 
 * @param   ap format���б���������Ĺ�����Υꥹ��
 * @return  �����������������<br>
 *          ���Ԥ�����������
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
 * @brief  ��¦�����ʸ����(const char *)�θ���
 *
 *  ���Ȥ�����ʸ����α�¦���麸������ʸ���� str �򸡺�����
 *  �ǽ�˽и�������֤��֤��ޤ���
 *
 * @param      str ���Ф���ʸ����
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tstring::strrstr( const char *str ) const
{
    return this->rfind(this->length(),str);
}

/**
 * @brief  ��¦�����ʸ����(const char *)�θ���
 *
 *  ���Ȥ�����ʸ����α�¦���麸������ʸ���� str �򸡺�����
 *  �ǽ�˽и�������֤��֤��ޤ���
 *
 * @param      pos ���֥����������ʸ����γ��ϰ���
 * @param      str ���Ф���ʸ����
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tstring::strrstr( size_t pos, const char *str ) const
{
    return this->rfind(pos,str);
}

/**
 * @brief  ��¦�����ʸ����(const char *)��Ϣ³Ū�ʸ���
 *
 *  ���Ȥ�����ʸ����α�¦���麸������ʸ���� str �򸡺�����
 *  �ǽ�˽и�������֤��֤��ޤ���
 *
 * @param      pos ���֥����������ʸ����γ��ϰ���
 * @param      str ���Ф���ʸ����
 * @param      nextpos �����pos
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tstring::strrstr( size_t pos, const char *str, size_t *nextpos ) const
{
    return this->rfind(pos,str,nextpos);
}

/**
 * @brief  ��¦�����ʸ����(tstring)�θ���
 *
 *  ���Ȥ�����ʸ����α�¦���麸������ʸ���� str �򸡺�����
 *  �ǽ�˽и�������֤��֤��ޤ���
 *
 * @param      str ���Ф���ʸ����
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tstring::strrstr( const tstring &str ) const
{
    return this->rfind(this->length(),str.cstr());
}

/**
 * @brief  ��¦�����ʸ����(tstring)�θ���
 *
 *  ���Ȥ�����ʸ����α�¦���麸������ʸ���� str �򸡺�����
 *  �ǽ�˽и�������֤��֤��ޤ���
 *
 * @param      pos ���֥����������ʸ����γ��ϰ���
 * @param      str ���Ф���ʸ����
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tstring::strrstr( size_t pos, const tstring &str ) const
{
    return this->rfind(pos,str.cstr());
}

/**
 * @brief  ��¦�����ʸ����(tstring)��Ϣ³Ū�ʸ���
 *
 *  ���Ȥ�����ʸ����α�¦���麸������ʸ���� str �򸡺�����
 *  �ǽ�˽и�������֤��֤��ޤ���
 *
 * @param      pos ���֥����������ʸ����γ��ϰ���
 * @param      str ���Ф���ʸ����
 * @param      nextpos �����pos
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tstring::strrstr(size_t pos, const tstring &str, size_t *nextpos) const
{
    return this->rfind(pos,str.cstr(),nextpos);
}

/**
 * @brief  ��¦�����ʸ����(const char *)�θ���
 *
 *  ���Ȥ�����ʸ����α�¦���麸������ʸ���� str �򸡺�����
 *  �ǽ�˽и�������֤��֤��ޤ���
 *
 * @param      str ���Ф���ʸ����
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tstring::rfind( const char *str ) const
{
    return this->rfind(this->length(), str);
}

/**
 * @brief  ��¦�����ʸ����(const char *)�θ���
 *
 *  ���Ȥ�����ʸ����α�¦���麸������ʸ���� str �򸡺�����
 *  �ǽ�˽и�������֤��֤��ޤ���
 *
 * @param      str ���Ф���ʸ����
 * @param      n ���Ф���ʸ����
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tstring::rfind( const char *str, size_t n ) const
{
    return this->rfind(this->length(), str, n);
}

/**
 * @brief  ��¦�����ʸ����(const char *)�θ���
 *
 *  ���Ȥ�����ʸ����α�¦���麸������ʸ���� str �򸡺�����
 *  �ǽ�˽и�������֤��֤��ޤ���
 *
 * @param      pos ���֥����������ʸ����γ��ϰ���
 * @param      str ���Ф���ʸ����
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tstring::rfind( size_t pos, const char *str ) const
{
    return this->rfind(pos, str,(size_t *)NULL);
}

/**
 * @brief  ��¦�����ʸ����(const char *)��Ϣ³Ū�ʸ���
 *
 *  ���Ȥ�����ʸ����α�¦���麸������ʸ����str �򸡺�����
 *  �ǽ�˽и�������֤��֤��ޤ���
 *
 * @param      pos ���֥����������ʸ����γ��ϰ���
 * @param      str ���Ф���ʸ����
 * @param      nextpos �����pos
 * @return     �����������������<br>
 *             ���Ԥ�����������
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
 * @brief  ��¦�����ʸ����(const char *)�θ���
 *
 *  ���Ȥ�����ʸ����α�¦���麸������ʸ���� str �򸡺�����
 *  �ǽ�˽и�������֤��֤��ޤ���
 *
 * @param      pos ���֥����������ʸ����γ��ϰ���
 * @param      str ���Ф���ʸ����
 * @param      n ���Ф���ʸ����
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tstring::rfind( size_t pos, const char *str, size_t n ) const
{
    return this->rfind(pos,str,n,(size_t *)NULL);
}

/**
 * @brief  ��¦�����ʸ����(const char *)��Ϣ³Ū�ʸ���
 *
 *  ���Ȥ�����ʸ����α�¦���麸������ʸ����str �򸡺�����
 *  �ǽ�˽и�������֤��֤��ޤ���
 *
 * @param      pos ���֥����������ʸ����γ��ϰ���
 * @param      str ���Ф���ʸ����
 * @param      n ���Ф���ʸ����
 * @param      nextpos �����pos
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tstring::rfind( size_t pos, const char *str, size_t n,
			size_t *nextpos ) const
{
    if ( this->cstr() != NULL && pos <= this->length() && str != NULL ) {
	size_t len, i;
	/* n ��꾮���������б� */
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
 * @brief  ��¦�����ʸ����(tstring)�θ���
 *
 *  ���Ȥ�����ʸ����α�¦���麸������ʸ����str �򸡺�����
 *  �ǽ�˽и�������֤��֤��ޤ���
 *
 * @param      str ���Ф���ʸ����
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tstring::rfind( const tstring &str ) const
{
    return this->rfind(this->length(), str.cstr());
}

/**
 * @brief  ��¦�����ʸ����(tstring)�θ���
 *
 *  ���Ȥ�����ʸ����α�¦���麸������ʸ����str �򸡺�����
 *  �ǽ�˽и�������֤��֤��ޤ���
 *
 * @param      pos ���֥����������ʸ����γ��ϰ���
 * @param      str ���Ф���ʸ����
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tstring::rfind( size_t pos, const tstring &str ) const
{
    return this->rfind(pos, str.cstr());
}

/**
 * @brief  ��¦�����ʸ����(tstring)��Ϣ³Ū�ʸ���
 *
 *  ���Ȥ�����ʸ����α�¦���麸������ʸ����str �򸡺�����
 *  �ǽ�˽и�������֤��֤��ޤ���
 *
 * @param      pos ���֥����������ʸ����γ��ϰ���
 * @param      str ���Ф���ʸ����
 * @param      nextpos �����pos
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tstring::rfind( size_t pos, const tstring &str, size_t *nextpos ) const
{
    return this->rfind(pos, str.cstr(), nextpos);
}

/* ���ޥ� */

/**
 * @brief  printf()�ε�ˡ�ǻ��ꤵ�줿ʸ�����¦���鸡��
 *
 *  ���Ȥ�����ʸ����α�¦���顤���ꤵ�줿ʸ����򸡺������ǽ�˽и�����
 *  ���֤��֤��ޤ������δؿ��Ǥϡ���������ʸ����� printf()�ؿ���Ʊ�ͤ�
 *  �ե����ޥåȤȲ��Ѱ����ǥ��åȤǤ��ޤ���
 *
 * @param   format ����ʸ����Τ���Υե����ޥåȻ��� 
 * @param   ... format���б���������Ĺ�����γ�����
 * @return  �����������������<br>
 *          ���Ԥ�����������
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
 * @brief  printf()�ε�ˡ�ǻ��ꤵ�줿ʸ�����¦���鸡��
 *
 *  ���Ȥ�����ʸ����α�¦���顤���ꤵ�줿ʸ����򸡺������ǽ�˽и�����
 *  ���֤��֤��ޤ������δؿ��Ǥϡ���������ʸ����� printf()�ؿ���Ʊ�ͤ�
 *  �ե����ޥåȤȲ��Ѱ����ǥ��åȤǤ��ޤ���
 *
 * @param   format ����ʸ����Τ���Υե����ޥåȻ��� 
 * @param   ap format���б���������Ĺ�����Υꥹ��
 * @return  �����������������<br>
 *          ���Ԥ�����������
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
 * @brief  printf()�ε�ˡ�ǻ��ꤵ�줿ʸ�����¦���鸡��
 *
 *  ���Ȥ�����ʸ����� pos �ΰ��֤򳫻����Ȥ������ꤵ�줿ʸ�����¦���鸡��
 *  ���ƺǽ�˽и�������֤��֤��ޤ������δؿ��Ǥϡ���������ʸ����� printf()
 *  �ؿ���Ʊ�ͤΥե����ޥåȤȲ��Ѱ����ǥ��åȤǤ��ޤ���
 *
 * @param   pos ���Ȥ�ʸ����γ��ϰ���
 * @param   format ����ʸ����Τ���Υե����ޥåȻ��� 
 * @param   ... format���б���������Ĺ�����γ�����
 * @return  �����������������<br>
 *          ���Ԥ�����������
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
 * @brief  printf()�ε�ˡ�ǻ��ꤵ�줿ʸ�����¦���鸡��
 *
 *  ���Ȥ�����ʸ����� pos �ΰ��֤򳫻����Ȥ������ꤵ�줿ʸ�����¦���鸡��
 *  ���ƺǽ�˽и�������֤��֤��ޤ������δؿ��Ǥϡ���������ʸ����� printf()
 *  �ؿ���Ʊ�ͤΥե����ޥåȤȲ��Ѱ����ǥ��åȤǤ��ޤ���
 *
 * @param   pos ���Ȥ�ʸ����γ��ϰ���
 * @param   format ����ʸ����Τ���Υե����ޥåȻ��� 
 * @param   ap format���б���������Ĺ�����Υꥹ��
 * @return  �����������������<br>
 *          ���Ԥ�����������
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
 * @brief  ��¦�����ʸ���θ���
 *
 *  ���Ȥ�����ʸ����κ�¦���鱦������ʸ�� ch �򸡺�����
 *  �ǽ�˽и�������֤��֤��ޤ���
 *
 * @param      ch ���Ф���ʸ��
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tstring::strchr( int ch ) const
{
    return this->find((size_t)0,ch);
}

/**
 * @brief  ��¦�����ʸ���θ���
 *
 *  ���Ȥ�����ʸ����κ�¦���鱦������ʸ�� ch �򸡺�����
 *  �ǽ�˽и�������֤��֤��ޤ���
 *
 * @param      pos ���Ȥ�ʸ����γ��ϰ���
 * @param      ch ���Ф���ʸ��
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tstring::strchr( size_t pos, int ch ) const
{
    return this->find(pos,ch);
}

/**
 * @brief  ��¦�����ʸ����Ϣ³Ū�ʸ���
 *
 *  ���Ȥ�����ʸ����κ�¦���鱦������ʸ�� ch �򸡺�����
 *  �ǽ�˽и�������֤��֤��ޤ���
 *
 * @param      pos ���Ȥ�ʸ����γ��ϰ���
 * @param      ch ���Ф���ʸ��
 * @param      nextpos �����pos
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tstring::strchr( size_t pos, int ch, size_t *nextpos ) const
{
    return this->find(pos,ch,nextpos);
}

/**
 * @brief  ��¦�����ʸ���θ���
 *
 *  ���Ȥ�����ʸ����κ�¦���鱦������ʸ�� ch �򸡺�����
 *  �ǽ�˽и�������֤��֤��ޤ���
 *
 * @param      ch ���Ф���ʸ��
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tstring::find( int ch ) const
{
    return this->find((size_t)0,ch);
}

/**
 * @brief  ��¦�����ʸ���θ���
 *
 *  ���Ȥ�����ʸ����κ�¦���鱦������ʸ�� ch �򸡺�����
 *  �ǽ�˽и�������֤��֤��ޤ���
 *
 * @param      pos ���Ȥ�ʸ����γ��ϰ���
 * @param      ch ���Ф���ʸ��
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tstring::find( size_t pos, int ch ) const
{
    return this->find(pos,ch,(size_t *)NULL);
}

/**
 * @brief  ��¦�����ʸ����Ϣ³Ū�ʸ���
 *
 *  ���Ȥ�����ʸ����κ�¦���鱦������ʸ�� ch �򸡺�����
 *  �ǽ�˽и�������֤��֤��ޤ���
 *
 * @param      pos ���Ȥ�ʸ����γ��ϰ���
 * @param      ch ���Ф���ʸ��
 * @param      nextpos �����pos
 * @return     �����������������<br>
 *             ���Ԥ�����������
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
 * @brief  ��¦�����ʸ���θ���
 *
 *  ���Ȥ�����ʸ����α�¦���麸������ʸ�� ch �򸡺�����
 *  �ǽ�˽и�������֤��֤��ޤ���
 *
 * @param      ch ���Ф���ʸ��
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tstring::strrchr( int ch ) const
{
    if ( this->length() < 1 ) return -1;
    return this->rfind((size_t)(this->length() - 1),ch);
}

/**
 * @brief  ��¦�����ʸ���θ���
 *
 *  ���Ȥ�����ʸ����α�¦���麸������ʸ�� ch �򸡺�����
 *  �ǽ�˽и�������֤��֤��ޤ���
 *
 * @param      pos ���Ȥ�ʸ����γ��ϰ���
 * @param      ch ���Ф���ʸ��
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tstring::strrchr( size_t pos, int ch ) const
{
    return this->rfind(pos,ch);
}

/**
 * @brief  ��¦�����ʸ����Ϣ³Ū�ʸ���
 *
 *  ���Ȥ�����ʸ����α�¦���麸������ʸ�� ch �򸡺�����
 *  �ǽ�˽и�������֤��֤��ޤ���
 *
 * @param      pos ���Ȥ�ʸ����γ��ϰ���
 * @param      ch ���Ф���ʸ��
 * @param      nextpos �����pos
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tstring::strrchr( size_t pos, int ch, size_t *nextpos ) const
{
    return this->rfind(pos,ch,nextpos);
}

/**
 * @brief  ��¦�����ʸ���θ���
 *
 *  ���Ȥ�����ʸ����α�¦���麸������ʸ�� ch �򸡺�����
 *  �ǽ�˽и�������֤��֤��ޤ���
 *
 * @param      ch ���Ф���ʸ��
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tstring::rfind( int ch ) const
{
    if ( this->length() < 1 ) return -1;
    return this->rfind((size_t)(this->length() - 1),ch);
}

/**
 * @brief  ��¦�����ʸ���θ���
 *
 *  ���Ȥ�����ʸ����α�¦���麸������ʸ�� ch �򸡺�����
 *  �ǽ�˽и�������֤��֤��ޤ���
 *
 * @param      pos ���Ȥ�ʸ����γ��ϰ���
 * @param      ch ���Ф���ʸ��
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tstring::rfind( size_t pos, int ch ) const
{
    return this->rfind(pos,ch,(size_t *)NULL);
}

/**
 * @brief  ��¦�����ʸ����Ϣ³Ū�ʸ���
 *
 *  ���Ȥ�����ʸ����α�¦���麸������ʸ�� ch �򸡺�����
 *  �ǽ�˽и�������֤��֤��ޤ���
 *
 * @param      pos ���Ȥ�ʸ����γ��ϰ���
 * @param      ch ���Ф���ʸ��
 * @param      nextpos �����pos
 * @return     �����������������<br>
 *             ���Ԥ�����������
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
 * @brief  ʸ�����åȤ˴ޤޤ��ʸ����¦���鸡��
 *
 *  ���Ȥ�����ʸ����κ�¦���鱦�����ˡ�ʸ�����å� str �˴ޤޤ��
 *  ʸ���򸡺������ǽ�˽и�������֤��֤��ޤ���
 *
 * @param      str �����оݤ�ʸ�����å�
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tstring::find_first_of( const char *str ) const
{
    if ( str != NULL ) return this->find_first_of((size_t)0,str,c_strlen(str));
    else return -1;
}

/**
 * @brief  ʸ�����åȤ˴ޤޤ��ʸ����¦���鸡��
 *
 *  ���Ȥ�����ʸ����κ�¦���鱦�����ˡ�ʸ�����å� str �˴ޤޤ��
 *  ʸ���򸡺������ǽ�˽и�������֤��֤��ޤ���
 *
 * @param      str �����оݤ�ʸ�����å�
 * @param      n ʸ�����å�str ��ʸ����
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tstring::find_first_of( const char *str, size_t n ) const
{
    if ( str != NULL ) return this->find_first_of((size_t)0,str,n);
    else return -1;
}

/**
 * @brief  ʸ�����åȤ˴ޤޤ��ʸ����¦���鸡��
 *
 *  ���Ȥ�����ʸ����κ�¦���鱦�����ˡ�ʸ�����å� str �˴ޤޤ��
 *  ʸ���򸡺������ǽ�˽и�������֤��֤��ޤ���
 *
 * @param      pos ���Фγ��ϰ���
 * @param      str �����оݤ�ʸ�����å�
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tstring::find_first_of( size_t pos, const char *str ) const
{
    if ( str != NULL ) return this->find_first_of(pos,str,c_strlen(str));
    else return -1;
}

/**
 * @brief  ʸ�����åȤ˴ޤޤ��ʸ����¦����Ϣ³Ū�˸���
 *
 *  ���Ȥ�����ʸ����κ�¦���鱦�����ˡ�ʸ�����å� str �˴ޤޤ��
 *  ʸ���򸡺������ǽ�˽и�������֤��֤��ޤ���
 *
 * @param      pos ���Фγ��ϰ���
 * @param      str �����оݤ�ʸ�����å�
 * @param      nextpos �����pos
 * @return     �����������������<br>
 *             ���Ԥ�����������
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
 * @brief  ʸ�����åȤ˴ޤޤ��ʸ����¦���鸡��
 *
 *  ���Ȥ�����ʸ����κ�¦���鱦�����ˡ�ʸ�����å� str �˴ޤޤ��
 *  ʸ���򸡺������ǽ�˽и�������֤��֤��ޤ���
 *
 * @param      pos ���Фγ��ϰ���
 * @param      str �����оݤ�ʸ�����å�
 * @param      n ʸ�����å�str ��ʸ����
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tstring::find_first_of( size_t pos, const char *str, size_t n ) const
{
    return this->find_first_of(pos,str,n,(size_t *)NULL);
}

/**
 * @brief  ʸ�����åȤ˴ޤޤ��ʸ����¦����Ϣ³Ū�˸���
 *
 *  ���Ȥ�����ʸ����κ�¦���鱦�����ˡ�ʸ�����å� str �˴ޤޤ��
 *  ʸ���򸡺������ǽ�˽и�������֤��֤��ޤ���
 *
 * @param      pos ���Фγ��ϰ���
 * @param      str �����оݤ�ʸ�����å�
 * @param      n ʸ�����å�str ��ʸ����
 * @param      nextpos �����pos
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tstring::find_first_of( size_t pos, const char *str, size_t n,
				size_t *nextpos ) const
{
    if ( 0 < this->length() && pos < this->length() && 
	 str != NULL && 0 < n ) {
	size_t len, i, j;
	/* n ��꾮���������б� */
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
 * @brief  ʸ�����åȤ˴ޤޤ��ʸ����¦���鸡��
 *
 *  ���Ȥ�����ʸ����κ�¦���鱦�����ˡ�ʸ�����å� str �˴ޤޤ��
 *  ʸ���򸡺������ǽ�˽и�������֤��֤��ޤ���
 *
 * @param      str �����оݤ�ʸ�����å�
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tstring::find_first_of( const tstring &str ) const
{
    if ( str.cstr() != NULL ) 
	return this->find_first_of((size_t)0,str.cstr(),str.length());
    else return -1;
}

/**
 * @brief  ʸ�����åȤ˴ޤޤ��ʸ����¦���鸡��
 *
 *  ���Ȥ�����ʸ����κ�¦���鱦�����ˡ�ʸ�����å� str �˴ޤޤ��
 *  ʸ���򸡺������ǽ�˽и�������֤��֤��ޤ���
 *
 * @param      pos ���Фγ��ϰ���
 * @param      str �����оݤ�ʸ�����å�
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tstring::find_first_of( size_t pos, const tstring &str ) const
{
    if ( str.cstr() != NULL ) 
	return this->find_first_of(pos,str.cstr(),str.length());
    else return -1;
}

/**
 * @brief  ʸ�����åȤ˴ޤޤ��ʸ����¦����Ϣ³Ū�˸���
 *
 *  ���Ȥ�����ʸ����κ�¦���鱦�����ˡ�ʸ�����å� str �˴ޤޤ��
 *  ʸ���򸡺������ǽ�˽и�������֤��֤��ޤ���
 *
 * @param      pos ���Фγ��ϰ���
 * @param      str �����оݤ�ʸ�����å�
 * @param      nextpos �����pos
 * @return     �����������������<br>
 *             ���Ԥ�����������
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
 * @brief  ʸ�����åȤ˴ޤޤ��ʸ����¦���鸡��
 *
 *  ���Ȥ�����ʸ����α�¦���麸�����ˡ�ʸ�����å� str ��
 *  �ޤޤ��ʸ���򸡺������ǽ�˽и�������֤��֤��ޤ���
 *
 * @param      str �����оݤ�ʸ�����å�
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tstring::find_last_of( const char *str ) const
{
    if ( 0 < this->length() && str != NULL ) 
        return this->find_last_of(this->length() - 1,str,c_strlen(str));
    else return -1;
}

/**
 * @brief  ʸ�����åȤ˴ޤޤ��ʸ����¦���鸡��
 *
 *  ���Ȥ�����ʸ����α�¦���麸�����ˡ�ʸ�����å� str ��
 *  �ޤޤ��ʸ���򸡺������ǽ�˽и�������֤��֤��ޤ���
 *
 * @param      str �����оݤ�ʸ�����å�
 * @param      n ʸ�����å�str ��ʸ����
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tstring::find_last_of( const char *str, size_t n ) const
{
    if ( 0 < this->length() && str != NULL ) 
        return this->find_last_of(this->length() - 1,str,n);
    else return -1;
}

/**
 * @brief  ʸ�����åȤ˴ޤޤ��ʸ����¦���鸡��
 *
 *  ���Ȥ�����ʸ����α�¦���麸�����ˡ�ʸ�����å� str ��
 *  �ޤޤ��ʸ���򸡺������ǽ�˽и�������֤��֤��ޤ���
 *
 * @param      pos ���Фγ��ϰ���
 * @param      str �����оݤ�ʸ�����å�
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tstring::find_last_of( size_t pos, const char *str ) const
{
    if ( str != NULL ) return this->find_last_of(pos,str,c_strlen(str));
    else return -1;
}

/**
 * @brief  ʸ�����åȤ˴ޤޤ��ʸ����¦����Ϣ³Ū�˸���
 *
 *  ���Ȥ�����ʸ����α�¦���麸�����ˡ�ʸ�����å� str ��
 *  �ޤޤ��ʸ���򸡺������ǽ�˽и�������֤��֤��ޤ���
 *
 * @param      pos ���Фγ��ϰ���
 * @param      str �����оݤ�ʸ�����å�
 * @param      nextpos �����pos
 * @return     �����������������<br>
 *             ���Ԥ�����������
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
 * @brief  ʸ�����åȤ˴ޤޤ��ʸ����¦���鸡��
 *
 *  ���Ȥ�����ʸ����α�¦���麸�����ˡ�ʸ�����å� str ��
 *  �ޤޤ��ʸ���򸡺������ǽ�˽и�������֤��֤��ޤ���
 *
 * @param      pos ���Фγ��ϰ���
 * @param      str �����оݤ�ʸ�����å�
 * @param      n ʸ�����å�str ��ʸ����
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tstring::find_last_of( size_t pos, const char *str, size_t n ) const
{
    return this->find_last_of(pos,str,n,(size_t *)NULL);
}

/**
 * @brief  ʸ�����åȤ˴ޤޤ��ʸ����¦����Ϣ³Ū�˸���
 *
 *  ���Ȥ�����ʸ����α�¦���麸�����ˡ�ʸ�����å� str ��
 *  �ޤޤ��ʸ���򸡺������ǽ�˽и�������֤��֤��ޤ���
 *
 * @param      pos ���Фγ��ϰ���
 * @param      str �����оݤ�ʸ�����å�
 * @param      n ʸ�����å�str ��ʸ����
 * @param      nextpos �����pos
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tstring::find_last_of( size_t pos, const char *str, size_t n,
			       size_t *nextpos ) const
{
    if ( 0 < this->length() && pos < this->length() && 
	 str != NULL && 0 < n ) {
	size_t len, i, j;
	/* n ��꾮���������б� */
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
 * @brief  ʸ�����åȤ˴ޤޤ��ʸ����¦���鸡��
 *
 *  ���Ȥ�����ʸ����α�¦���麸�����ˡ�ʸ�����å� str ��
 *  �ޤޤ��ʸ���򸡺������ǽ�˽и�������֤��֤��ޤ���
 *
 * @param      str �����оݤ�ʸ�����å�
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tstring::find_last_of( const tstring &str ) const
{
    if ( 0 < this->length() && str.cstr() != NULL ) 
        return this->find_last_of(this->length() - 1,str.cstr(),str.length());
    else return -1;
}

/**
 * @brief  ʸ�����åȤ˴ޤޤ��ʸ����¦���鸡��
 *
 *  ���Ȥ�����ʸ����α�¦���麸�����ˡ�ʸ�����å� str ��
 *  �ޤޤ��ʸ���򸡺������ǽ�˽и�������֤��֤��ޤ���
 *
 * @param      pos ���Фγ��ϰ���
 * @param      str �����оݤ�ʸ�����å�
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tstring::find_last_of( size_t pos, const tstring &str ) const
{
    if ( str.cstr() != NULL )
	return this->find_last_of(pos,str.cstr(),str.length());
    else return -1;
}

/**
 * @brief  ʸ�����åȤ˴ޤޤ��ʸ����¦����Ϣ³Ū�˸���
 *
 *  ���Ȥ�����ʸ����α�¦���麸�����ˡ�ʸ�����å� str ��
 *  �ޤޤ��ʸ���򸡺������ǽ�˽и�������֤��֤��ޤ���
 *
 * @param      pos ���Фγ��ϰ���
 * @param      str �����оݤ�ʸ�����å�
 * @param      nextpos �����pos
 * @return     �����������������<br>
 *             ���Ԥ�����������
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
 * @brief  ��¦�����ʸ���θ���
 *
 *  ���Ȥ�����ʸ����κ�¦���鱦������ʸ�� ch �򸡺�����
 *  �ǽ�˽и�������֤��֤��ޤ���
 *
 * @param      ch ���Ф���ʸ��
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tstring::find_first_of( int ch ) const	/* find() ��Ʊ�� */
{
    return tstring::find(ch);
}

/**
 * @brief  ��¦�����ʸ���θ���
 *
 *  ���Ȥ�����ʸ����α�¦���麸������ʸ�� ch �򸡺�����
 *  �ǽ�˽и�������֤��֤��ޤ���
 *
 * @param      ch ���Ф���ʸ��
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tstring::find_last_of( int ch ) const	/* rfind() ��Ʊ�� */
{
    return tstring::rfind(ch);
}

/**
 * @brief  ��¦�����ʸ���θ���
 *
 *  ���Ȥ�����ʸ����κ�¦���鱦������ʸ�� ch �򸡺�����
 *  �ǽ�˽и�������֤��֤��ޤ���
 *
 * @param      pos ���Ȥ�ʸ����γ��ϰ���
 * @param      ch ���Ф���ʸ��
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tstring::find_first_of( size_t pos, int ch ) const /* find() ��Ʊ�� */
{
    return tstring::find(pos,ch);
}

/**
 * @brief  ��¦�����ʸ���θ���
 *
 *  ���Ȥ�����ʸ����α�¦���麸������ʸ�� ch �򸡺�����
 *  �ǽ�˽и�������֤��֤��ޤ���
 *
 * @param      ch ���Ф���ʸ��
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tstring::find_last_of( size_t pos, int ch ) const /* rfind() ��Ʊ�� */
{
    return tstring::rfind(pos,ch);
}

/**
 * @brief  ��¦�����ʸ����Ϣ³Ū�ʸ���
 *
 *  ���Ȥ�����ʸ����κ�¦���鱦������ʸ�� ch �򸡺�����
 *  �ǽ�˽и�������֤��֤��ޤ���
 *
 * @param      pos ���Ȥ�ʸ����γ��ϰ���
 * @param      ch ���Ф���ʸ��
 * @param      nextpos �����pos
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tstring::find_first_of( size_t pos, int ch, size_t *nextpos ) const /* find() ��Ʊ�� */
{
    return tstring::find(pos,ch,nextpos);
}

/**
 * @brief  ��¦�����ʸ����Ϣ³Ū�ʸ���
 *
 *  ���Ȥ�����ʸ����α�¦���麸������ʸ�� ch �򸡺�����
 *  �ǽ�˽и�������֤��֤��ޤ���
 *
 * @param      pos ���Ȥ�ʸ����γ��ϰ���
 * @param      ch ���Ф���ʸ��
 * @param      nextpos �����pos
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tstring::find_last_of( size_t pos, int ch, size_t *nextpos ) const /* rfind() ��Ʊ�� */
{
    return tstring::rfind(pos,ch,nextpos);
}

/**
 * @brief  ʸ�����åȤ˴ޤޤ�ʤ�ʸ����¦���鸡��
 *
 *  ���Ȥ�����ʸ����κ�¦���鱦�����ˡ�ʸ�����å� str ��
 *  �ޤޤ�ʤ�ʸ���򸡺������ǽ�˽и�������֤��֤��ޤ���
 *
 * @param      str ʸ�����å�
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tstring::find_first_not_of( const char *str ) const
{
    if ( str != NULL ) return this->find_first_not_of((size_t)0,str,c_strlen(str));
    else return -1;
}

/**
 * @brief  ʸ�����åȤ˴ޤޤ�ʤ�ʸ����¦���鸡��
 *
 *  ���Ȥ�����ʸ����κ�¦���鱦�����ˡ�ʸ�����å�str ��<br>
 *  �ޤޤ�ʤ�ʸ���򸡺������ǽ�˽и�������֤��֤��ޤ���
 *
 * @param      str ʸ�����å�
 * @param      n ʸ�����å�str ��ʸ����
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tstring::find_first_not_of( const char *str, size_t n ) const
{
    if ( str != NULL ) return this->find_first_not_of((size_t)0,str,n);
    else return -1;
}

/**
 * @brief  ʸ�����åȤ˴ޤޤ�ʤ�ʸ����¦���鸡��
 *
 *  ���Ȥ�����ʸ����κ�¦���鱦�����ˡ�ʸ�����å� str ��
 *  �ޤޤ�ʤ�ʸ���򸡺������ǽ�˽и�������֤��֤��ޤ���
 *
 * @param      pos ���Фγ��ϰ���
 * @param      str ʸ�����å�
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tstring::find_first_not_of( size_t pos, const char *str ) const
{
    if ( str != NULL ) return this->find_first_not_of(pos,str,c_strlen(str));
    else return -1;
}

/**
 * @brief  ʸ�����åȤ˴ޤޤ�ʤ�ʸ����¦����Ϣ³Ū�˸���
 *
 *  ���Ȥ�����ʸ����κ�¦���鱦�����ˡ�ʸ�����å� str ��
 *  �ޤޤ�ʤ�ʸ���򸡺������ǽ�˽и�������֤��֤��ޤ���
 *
 * @param      pos ���Фγ��ϰ���
 * @param      str ʸ�����å�
 * @param      nextpos �����pos
 * @return     �����������������<br>
 *             ���Ԥ�����������
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
 * @brief  ʸ�����åȤ˴ޤޤ�ʤ�ʸ����¦���鸡��
 *
 *  ���Ȥ�����ʸ����κ�¦���鱦�����ˡ�ʸ�����å� str ��
 *  �ޤޤ�ʤ�ʸ���򸡺������ǽ�˽и�������֤��֤��ޤ���
 *
 * @param      pos ���Фγ��ϰ���
 * @param      str ʸ�����å�
 * @param      n ʸ�����å�str ��ʸ����
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tstring::find_first_not_of( size_t pos, 
				    const char *str, size_t n ) const
{
    return this->find_first_not_of(pos,str,n,(size_t *)NULL);
}

/**
 * @brief  ʸ�����åȤ˴ޤޤ�ʤ�ʸ����¦����Ϣ³Ū�˸���
 *
 *  ���Ȥ�����ʸ����κ�¦���鱦�����ˡ�ʸ�����å� str ��
 *  �ޤޤ�ʤ�ʸ���򸡺������ǽ�˽и�������֤��֤��ޤ���
 *
 * @param      pos ���Фγ��ϰ���
 * @param      str ʸ�����å�
 * @param      n ʸ�����å�str ��ʸ����
 * @param      nextpos �����pos
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tstring::find_first_not_of( size_t pos, const char *str, size_t n,
				    size_t *nextpos ) const
{
    if ( 0 < this->length() && pos < this->length() && str != NULL ) {
	size_t len, i, j;
	/* n ��꾮���������б� */
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
 * @brief  ʸ�����åȤ˴ޤޤ�ʤ�ʸ����¦���鸡��
 *
 *  ���Ȥ�����ʸ����κ�¦���鱦�����ˡ�ʸ�����å� str ��
 *  �ޤޤ�ʤ�ʸ���򸡺������ǽ�˽и�������֤��֤��ޤ���
 *
 * @param      str ʸ�����å�
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tstring::find_first_not_of( const tstring &str ) const
{
    if ( str.cstr() != NULL )
	return this->find_first_not_of((size_t)0,str.cstr(),str.length());
    else return -1;
}

/**
 * @brief  ʸ�����åȤ˴ޤޤ�ʤ�ʸ����¦���鸡��
 *
 *  ���Ȥ�����ʸ����κ�¦���鱦�����ˡ�ʸ�����å� str ��
 *  �ޤޤ�ʤ�ʸ���򸡺������ǽ�˽и�������֤��֤��ޤ���
 *
 * @param      pos ���Фγ��ϰ���
 * @param      str ʸ�����å�
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tstring::find_first_not_of( size_t pos, const tstring &str ) const
{
    if ( str.cstr() != NULL )
	return this->find_first_not_of(pos,str.cstr(),str.length());
    else return -1;
}

/**
 * @brief  ʸ�����åȤ˴ޤޤ�ʤ�ʸ����¦����Ϣ³Ū�˸���
 *
 *  ���Ȥ�����ʸ����κ�¦���鱦�����ˡ�ʸ�����å� str ��
 *  �ޤޤ�ʤ�ʸ���򸡺������ǽ�˽и�������֤��֤��ޤ���
 *
 * @param      pos ���Фγ��ϰ���
 * @param      str ʸ�����å�
 * @param      nextpos �����pos
 * @return     �����������������<br>
 *             ���Ԥ�����������
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
 * @brief  ʸ�����åȤ˴ޤޤ�ʤ�ʸ����¦���鸡��
 *
 *  ���Ȥ�����ʸ����α�¦���麸�����ˡ�ʸ�����å� str ��
 *  �ޤޤ�ʤ�ʸ���򸡺������ǽ�˽и�������֤��֤��ޤ���
 *
 * @param      str ʸ����˴ޤޤ�Ƥ��ʤ�ʸ�����å�
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tstring::find_last_not_of( const char *str ) const
{
    if ( 0 < this->length() && str != NULL ) 
        return this->find_last_not_of(this->length() - 1,str,c_strlen(str));
    else return -1;
}

/**
 * @brief  ʸ�����åȤ˴ޤޤ�ʤ�ʸ����¦���鸡��
 *
 *  ���Ȥ�����ʸ����α�¦���麸�����ˡ�ʸ�����å� str ��
 *  �ޤޤ�ʤ�ʸ���򸡺������ǽ�˽и�������֤��֤��ޤ���
 *
 * @param      str ʸ����˴ޤޤ�Ƥ��ʤ�ʸ�����å�
 * @param      n ʸ�����å�str ��ʸ����
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tstring::find_last_not_of( const char *str, size_t n ) const
{
    if ( 0 < this->length() && str != NULL ) 
        return this->find_last_not_of(this->length() - 1,str,n);
    else return -1;
}

/**
 * @brief  ʸ�����åȤ˴ޤޤ�ʤ�ʸ����¦���鸡��
 *
 *  ���Ȥ�����ʸ����α�¦���麸�����ˡ�ʸ�����å� str ��
 *  �ޤޤ�ʤ�ʸ���򸡺������ǽ�˽и�������֤��֤��ޤ���
 *
 * @param      pos ���Фγ��ϰ���
 * @param      str ʸ����˴ޤޤ�Ƥ��ʤ�ʸ�����å�
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tstring::find_last_not_of( size_t pos, const char *str ) const
{
    if ( str != NULL ) return this->find_last_not_of(pos,str,c_strlen(str));
    else return -1;
}

/**
 * @brief  ʸ�����åȤ˴ޤޤ�ʤ�ʸ����¦����Ϣ³Ū�˸���
 *
 *  ���Ȥ�����ʸ����α�¦���麸�����ˡ�ʸ�����å� str ��
 *  �ޤޤ�ʤ�ʸ���򸡺������ǽ�˽и�������֤��֤��ޤ���
 *
 * @param      pos ���Фγ��ϰ���
 * @param      str ʸ����˴ޤޤ�Ƥ��ʤ�ʸ�����å�
 * @param      nextpos �����pos
 * @return     �����������������<br>
 *             ���Ԥ�����������
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
 * @brief  ʸ�����åȤ˴ޤޤ�ʤ�ʸ����¦���鸡��
 *
 *  ���Ȥ�����ʸ����α�¦���麸�����ˡ�ʸ�����å� str ��
 *  �ޤޤ�ʤ�ʸ���򸡺������ǽ�˽и�������֤��֤��ޤ���
 *
 * @param      pos ���Фγ��ϰ���
 * @param      str ʸ����˴ޤޤ�Ƥ��ʤ�ʸ�����å�
 * @param      n ʸ�����å�str ��ʸ����
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tstring::find_last_not_of( size_t pos, 
				   const char *str, size_t n ) const
{
    return this->find_last_not_of(pos,str,n,(size_t *)NULL);
}

/**
 * @brief  ʸ�����åȤ˴ޤޤ�ʤ�ʸ����¦����Ϣ³Ū�˸���
 *
 *  ���Ȥ�����ʸ����α�¦���麸�����ˡ�ʸ�����å� str ��
 *  �ޤޤ�ʤ�ʸ���򸡺������ǽ�˽и�������֤��֤��ޤ���
 *
 * @param      pos ���Фγ��ϰ���
 * @param      str ʸ����˴ޤޤ�Ƥ��ʤ�ʸ�����å�
 * @param      n ʸ�����å�str ��ʸ����
 * @param      nextpos �����pos
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tstring::find_last_not_of( size_t pos, const char *str, size_t n,
				   size_t *nextpos ) const
{
    if ( 0 < this->length() && pos < this->length() && str != NULL ) {
	size_t len, i, j;
	/* n ��꾮���������б� */
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
 * @brief  ʸ�����åȤ˴ޤޤ�ʤ�ʸ����¦���鸡��
 *
 *  ���Ȥ�����ʸ����α�¦���麸�����ˡ�ʸ�����å� str ��
 *  �ޤޤ�ʤ�ʸ���򸡺������ǽ�˽и�������֤��֤��ޤ���
 *
 * @param      str ʸ����˴ޤޤ�Ƥ��ʤ�ʸ�����å�
 * @return     �����������������<br>
 *             ���Ԥ�����������
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
 * @brief  ʸ�����åȤ˴ޤޤ�ʤ�ʸ����¦���鸡��
 *
 *  ���Ȥ�����ʸ����α�¦���麸�����ˡ�ʸ�����å� str ��
 *  �ޤޤ�ʤ�ʸ���򸡺������ǽ�˽и�������֤��֤��ޤ���
 *
 * @param      pos ���Фγ��ϰ���
 * @param      str ʸ����˴ޤޤ�Ƥ��ʤ�ʸ�����å�
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tstring::find_last_not_of( size_t pos, const tstring &str ) const
{
    if ( str.cstr() != NULL )
	return this->find_last_not_of(pos, str.cstr(), str.length());
    else return -1;
}

/**
 * @brief  ʸ�����åȤ˴ޤޤ�ʤ�ʸ����¦����Ϣ³Ū�˸���
 *
 *  ���Ȥ�����ʸ����α�¦���麸�����ˡ�ʸ�����å� str ��
 *  �ޤޤ�ʤ�ʸ���򸡺������ǽ�˽и�������֤��֤��ޤ���
 *
 * @param      pos ���Фγ��ϰ���
 * @param      str ʸ����˴ޤޤ�Ƥ��ʤ�ʸ�����å�
 * @param      nextpos �����pos
 * @return     �����������������<br>
 *             ���Ԥ�����������
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
 * @brief  ���ꤵ�줿ʸ���ǤϤʤ�ʸ����¦���鸡��
 *
 *  ���Ȥ�����ʸ����κ�¦���鱦�����ˡ�ʸ�� ch �ǤϤʤ�
 *  ʸ���򸡺������ǽ�˽и�������֤��֤��ޤ���
 *
 * @param      ch ���Ф��������ʸ��
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tstring::find_first_not_of( int ch ) const
{
    return this->find_first_not_of((size_t)0,ch);
}

/**
 * @brief  ���ꤵ�줿ʸ���ǤϤʤ�ʸ����¦���鸡��
 *
 *  ���Ȥ�����ʸ����κ�¦���鱦�����ˡ�ʸ�� ch �ǤϤʤ�
 *  ʸ���򸡺������ǽ�˽и�������֤��֤��ޤ���
 *
 * @param      pos ���Фγ��ϰ���
 * @param      ch ���Ф��������ʸ��
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tstring::find_first_not_of( size_t pos, int ch ) const
{
    return this->find_first_not_of(pos,ch,(size_t *)NULL);
}

/**
 * @brief  ���ꤵ�줿ʸ���ǤϤʤ�ʸ����¦����Ϣ³Ū�˸���
 *
 *  ���Ȥ�����ʸ����κ�¦���鱦�����ˡ�ʸ�� ch �ǤϤʤ�
 *  ʸ���򸡺������ǽ�˽и�������֤��֤��ޤ���
 *
 * @param      pos ���Фγ��ϰ���
 * @param      ch ���Ф��������ʸ��
 * @param      nextpos �����pos
 * @return     �����������������<br>
 *             ���Ԥ�����������
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
 * @brief  ���ꤵ�줿ʸ���ǤϤʤ�ʸ����¦���鸡��
 *
 *  ���Ȥ�����ʸ����α�¦���麸�����ˡ�ʸ�� ch �ǤϤʤ�
 *  ʸ���򸡺������ǽ�˽и�������֤��֤��ޤ���
 *
 * @param      ch ���Ф��������ʸ��
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tstring::find_last_not_of( int ch ) const
{
    if ( 0 < this->length() ) 
        return this->find_last_not_of(this->length() - 1,ch);
    else return -1;
}

/**
 * @brief  ���ꤵ�줿ʸ���ǤϤʤ�ʸ����¦���鸡��
 *
 *  ���Ȥ�����ʸ����α�¦���麸�����ˡ�ʸ�� ch �ǤϤʤ�
 *  ʸ���򸡺������ǽ�˽и�������֤��֤��ޤ���
 *
 * @param      pos ���Фγ��ϰ���
 * @param      ch ���Ф��������ʸ��
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tstring::find_last_not_of( size_t pos, int ch ) const
{
    return this->find_last_not_of(pos,ch,(size_t *)NULL);
}

/**
 * @brief  ���ꤵ�줿ʸ���ǤϤʤ�ʸ����¦����Ϣ³Ū�˸���
 *
 *  ���Ȥ�����ʸ����α�¦���麸�����ˡ�ʸ�� ch �ǤϤʤ�
 *  ʸ���򸡺������ǽ�˽и�������֤��֤��ޤ���
 *
 * @param      pos ���Фγ��ϰ���
 * @param      ch ���Ф��������ʸ��
 * @param      nextpos �����pos
 * @return     �����������������<br>
 *             ���Ԥ�����������
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
 * @brief  ʸ�����¦����ѡ��������������ơ������ޤ��ϳ�̤��б��ط������
 */
ssize_t tstring::find_quoted( const char *quot_bkt, int escape,
			      size_t *quoted_span, size_t *nextpos ) const
{
    return find_quoted_string( this->cstr(), this->length(), 
			       0, quot_bkt, escape, quoted_span, nextpos );
}

/**
 * @brief  ʸ�����¦����ѡ��������������ơ������ޤ��ϳ�̤��б��ط������
 */
ssize_t tstring::find_quoted( size_t pos, const char *quot_bkt, int escape,
			      size_t *quoted_span, size_t *nextpos ) const
{
    return find_quoted_string( this->cstr(), this->length(), 
			       pos, quot_bkt, escape, quoted_span, nextpos );
}

/**
 * @brief  ʸ�����¦����ѡ��������������ơ������ޤ��ϳ�̤��б��ط������
 */
ssize_t tstring::rfind_quoted( const char *quot_bkt, int escape,
			       size_t *quoted_span, size_t *nextpos ) const
{
    return rfind_quoted_string( this->cstr(), this->length(), 
		      this->length(), quot_bkt, escape, quoted_span, nextpos );
}

/**
 * @brief  ʸ�����¦����ѡ��������������ơ������ޤ��ϳ�̤��б��ط������
 */
ssize_t tstring::rfind_quoted( size_t pos, const char *quot_bkt, int escape,
			       size_t *quoted_span, size_t *nextpos ) const
{
    return rfind_quoted_string( this->cstr(), this->length(), 
				pos, quot_bkt, escape, quoted_span, nextpos );
}


/**
 * @brief  �ѻ��ޤ��Ͽ����Ǥ��뤫��Ĵ�٤�
 *
 *  ���Ȥ�����ʸ����ΰ���pos �ˤ���ʸ���򡤸��ߤΥ�����ˤ��ʬ�ष��
 *  ���η�̤��֤��ޤ���
 *
 * @param      pos ʬ�ह��ʸ���ΰ���
 * @return     pos�ˤ���ʸ���������Υ��дؿ����б�����ʸ����
 *             ���פ������true<br>
 *             �԰��פǤ������false
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
 * @brief  ����ե��٥åȤ��ɤ���Ĵ�٤�
 *
 *  ���Ȥ�����ʸ����ΰ���pos �ˤ���ʸ���򡤸��ߤΥ�����ˤ��ʬ�ष��
 *  ���η�̤��֤��ޤ���
 *
 * @param      pos ʬ�ह��ʸ���ΰ���
 * @return     pos�ˤ���ʸ���������Υ��дؿ����б�����ʸ����
 *             ���פ������true<br>
 *             �԰��פǤ������false
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
 * @brief  ����ʸ�����ɤ�����Ĵ�٤�
 *
 *  ���Ȥ�����ʸ����ΰ���pos �ˤ���ʸ���򡤸��ߤΥ�����ˤ��ʬ�ष��
 *  ���η�̤��֤��ޤ���
 *
 * @param      pos ʬ�ह��ʸ���ΰ���
 * @return     pos�ˤ���ʸ���������Υ��дؿ����б�����ʸ����
 *             ���פ������true<br>
 *             �԰��פǤ������false
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
 * @brief  ���� (0����9�ޤ�) ���ɤ�����Ĵ�٤�
 *
 *  ���Ȥ�����ʸ����ΰ���pos �ˤ���ʸ���򡤸��ߤΥ�����ˤ��ʬ�ष��
 *  ���η�̤��֤��ޤ���
 *
 * @param      pos ʬ�ह��ʸ���ΰ���
 * @return     pos�ˤ���ʸ���������Υ��дؿ����б�����ʸ����
 *             ���פ������true<br>
 *             �԰��פǤ������false
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
 * @brief  ɽ����ǽ��ʸ�����ɤ�����Ĵ�٤�
 *
 *  ���Ȥ�����ʸ����ΰ���pos �ˤ���ʸ���򡤸��ߤΥ�����ˤ��ʬ�ष��
 *  ���η�̤��֤��ޤ���
 *
 * @param      pos ʬ�ह��ʸ���ΰ���
 * @return     pos�ˤ���ʸ���������Υ��дؿ����б�����ʸ����
 *             ���פ������true<br>
 *             �԰��פǤ������false
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
 * @brief  ��ʸ�����ɤ�����Ĵ�٤�
 *
 *  ���Ȥ�����ʸ����ΰ���pos �ˤ���ʸ���򡤸��ߤΥ�����ˤ��ʬ�ष��
 *  ���η�̤��֤��ޤ���
 *
 * @param      pos ʬ�ह��ʸ���ΰ���
 * @return     pos�ˤ���ʸ���������Υ��дؿ����б�����ʸ����
 *             ���פ������true<br>
 *             �԰��פǤ������false
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
 * @brief  ɽ����ǽ��ʸ�����ɤ�����Ĵ�٤�(��������)
 *
 *  ���Ȥ�����ʸ����ΰ���pos �ˤ���ʸ���򡤸��ߤΥ�����ˤ��ʬ�ष��
 *  ���η�̤��֤��ޤ���
 *
 * @param      pos ʬ�ह��ʸ���ΰ���
 * @return     pos�ˤ���ʸ���������Υ��дؿ����б�����ʸ����
 *             ���פ������true<br>
 *             �԰��פǤ������false
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
 * @brief  ɽ����ǽ��ʸ�����ɤ�����Ĵ�٤�(����ȱѿ��������)
 *
 *  ���Ȥ�����ʸ����ΰ���pos �ˤ���ʸ���򡤸��ߤΥ�����ˤ��ʬ�ष��
 *  ���η�̤��֤��ޤ���
 *
 * @param      pos ʬ�ह��ʸ���ΰ���
 * @return     pos�ˤ���ʸ���������Υ��дؿ����б�����ʸ����
 *             ���פ������true<br>
 *             �԰��פǤ������false
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
 * @brief  ����ʸ�����ɤ�����Ĵ�٤�
 *
 *  ���Ȥ�����ʸ����ΰ���pos �ˤ���ʸ���򡤸��ߤΥ�����ˤ��ʬ�ष��
 *  ���η�̤��֤��ޤ���
 *
 * @param      pos ʬ�ह��ʸ���ΰ���
 * @return     pos�ˤ���ʸ���������Υ��дؿ����б�����ʸ����
 *             ���פ������true<br>
 *             �԰��פǤ������false
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
 * @brief  ��ʸ�����ɤ�����Ĵ�٤�
 *
 *  ���Ȥ�����ʸ����ΰ���pos �ˤ���ʸ���򡤸��ߤΥ�����ˤ��ʬ�ष��
 *  ���η�̤��֤��ޤ���
 *
 * @param      pos ʬ�ह��ʸ���ΰ���
 * @return     pos�ˤ���ʸ���������Υ��дؿ����б�����ʸ����
 *             ���פ������true<br>
 *             �԰��פǤ������false
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
 * @brief  16 �ʿ��Ǥο������ɤ�����Ĵ�٤�
 *
 *  ���Ȥ�����ʸ����ΰ���pos �ˤ���ʸ���򡤸��ߤΥ�����ˤ��ʬ�ष��
 *  ���η�̤��֤��ޤ���
 *
 * @param      pos ʬ�ह��ʸ���ΰ���
 * @return     pos�ˤ���ʸ���������Υ��дؿ����б�����ʸ����
 *             ���פ������true<br>
 *             �԰��פǤ������false
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
 * @brief  ʸ�����åȤ˴ޤޤ��ʸ���򸡺�
 *
 *  ���Ȥ�����ʸ����κ�¦���鱦�����ˡ�ʸ�����å�str�˴ޤޤ��ʸ���򸡺�����
 *  �ǽ�˽и�������֤��֤��ޤ���<br>
 *  �ʤ���ʸ�������Ƭ���֤� 0 �Ǥ���
 *
 * @param   str �����оݤ�ʸ�����å�
 * @return  �����������������<br>
 *          ���Ԥ�����������
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
ssize_t tstring::strpbrk( const char *str ) const
{
    if ( str != NULL ) return this->strpbrk((size_t)0,str);
    else return -1;
}

/**
 * @brief  ʸ�����åȤ˴ޤޤ��ʸ���򸡺�
 *
 *  ���Ȥ�����ʸ����κ�¦���鱦�����ˡ�ʸ�����å�str�˴ޤޤ��ʸ���򸡺�����
 *  �ǽ�˽и�������֤��֤��ޤ���<br>
 *  �ʤ���ʸ�������Ƭ���֤� 0 �Ǥ���
 *
 * @param   pos ���Фγ��ϰ���
 * @param   str �����оݤ�ʸ�����å�
 * @return  �����������������<br>
 *          ���Ԥ�����������
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
ssize_t tstring::strpbrk( size_t pos, const char *str ) const
{
    return this->strpbrk(pos,str,(size_t *)NULL);
}

/**
 * @brief  ʸ�����åȤ˴ޤޤ��ʸ����Ϣ³Ū�˸���
 *
 *  ���Ȥ�����ʸ����κ�¦���鱦�����ˡ�ʸ�����å�str�˴ޤޤ��ʸ���򸡺�����
 *  �ǽ�˽и�������֤��֤��ޤ���<br>
 *  �ʤ���ʸ�������Ƭ���֤� 0 �Ǥ���
 *
 * @param   pos ���Фγ��ϰ���
 * @param   str �����оݤ�ʸ�����å�
 * @param   nextpos �����pos
 * @return  �����������������<br>
 *          ���Ԥ�����������
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
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
	    if ( p0 == 0 ) {	/* �ѥ����󤬤������� */
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
 * @brief  ʸ�����åȤ˴ޤޤ��ʸ���򸡺�
 *
 *  ���Ȥ�����ʸ����κ�¦���鱦�����ˡ�ʸ�����å�str�˴ޤޤ��ʸ���򸡺�����
 *  �ǽ�˽и�������֤��֤��ޤ���<br>
 *  �ʤ���ʸ�������Ƭ���֤� 0 �Ǥ���
 *
 * @param   str �����оݤ�ʸ�����å�
 * @return  �����������������<br>
 *          ���Ԥ�����������
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
ssize_t tstring::strpbrk( const tstring &str ) const
{
    if ( str.cstr() != NULL ) return this->strpbrk((size_t)0, str.cstr());
    else return -1;
}

/**
 * @brief  ʸ�����åȤ˴ޤޤ��ʸ���򸡺�
 *
 *  ���Ȥ�����ʸ����κ�¦���鱦�����ˡ�ʸ�����å�str�˴ޤޤ��ʸ���򸡺�����
 *  �ǽ�˽и�������֤��֤��ޤ���<br>
 *  �ʤ���ʸ�������Ƭ���֤� 0 �Ǥ���
 *
 * @param   pos ���Фγ��ϰ���
 * @param   str �����оݤ�ʸ�����å�
 * @return  �����������������<br>
 *          ���Ԥ�����������
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
ssize_t tstring::strpbrk( size_t pos, const tstring &str ) const
{
    if ( str.cstr() != NULL ) return this->strpbrk(pos, str.cstr());
    else return -1;
}

/**
 * @brief  ʸ�����åȤ˴ޤޤ��ʸ����Ϣ³Ū�˸���
 *
 *  ���Ȥ�����ʸ����κ�¦���鱦�����ˡ�ʸ�����å�str�˴ޤޤ��ʸ���򸡺�����
 *  �ǽ�˽и�������֤��֤��ޤ���<br>
 *  �ʤ���ʸ�������Ƭ���֤� 0 �Ǥ���
 *
 * @param   pos ���Фγ��ϰ���
 * @param   str �����оݤ�ʸ�����å�
 * @param   nextpos �����pos
 * @return  �����������������<br>
 *          ���Ԥ�����������
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
ssize_t tstring::strpbrk( size_t pos, const tstring &str,
			  size_t *nextpos ) const
{
    if ( str.cstr() != NULL ) return this->strpbrk(pos, str.cstr(), nextpos);
    else return -1;
}

/**
 * @brief  ʸ�����åȤ˴ޤޤ��ʸ���򡤱�¦���鸡��
 *
 *  ���Ȥ�����ʸ����α�¦���麸�����ˡ�ʸ���󥻥å� str �˴ޤޤ��ʸ���򸡺���
 *  �ǽ�˽и�������֤��֤��ޤ���
 *  �ʤ���ʸ�������Ƭ���֤� 0 �Ǥ���
 *
 * @param    str �����оݤ�ʸ�����å�
 * @return   �����������������<br>
 *           ���Ԥ�����������
 * @throw    �����Хåե��γ��ݤ˼��Ԥ������
 */
ssize_t tstring::strrpbrk( const char *str ) const
{
    if ( 0 < this->length() && str != NULL ) 
        return this->strrpbrk(this->length() - 1, str);
    else return -1;
}

/**
 * @brief  ʸ�����åȤ˴ޤޤ��ʸ���򡤱�¦���鸡��
 *
 *  ���Ȥ�����ʸ����α�¦���麸�����ˡ�ʸ���󥻥å� str �˴ޤޤ��ʸ���򸡺���
 *  �ǽ�˽и�������֤��֤��ޤ���
 *  �ʤ���ʸ�������Ƭ���֤� 0 �Ǥ���
 * 
 * @param    pos ���Фγ��ϰ���
 * @param    str �����оݤ�ʸ�����å�
 * @return   �����������������<br>
 *           ���Ԥ�����������
 * @throw    �����Хåե��γ��ݤ˼��Ԥ������
 */
ssize_t tstring::strrpbrk( size_t pos, const char *str ) const
{
    return this->strrpbrk(pos,str,(size_t *)NULL);
}

/**
 * @brief  ʸ�����åȤ˴ޤޤ��ʸ���򡤱�¦����Ϣ³Ū�˸���
 *
 *  ���Ȥ�����ʸ����α�¦���麸�����ˡ�ʸ���󥻥å� str �˴ޤޤ��ʸ���򸡺���
 *  �ǽ�˽и�������֤��֤��ޤ���
 *  �ʤ���ʸ�������Ƭ���֤� 0 �Ǥ���
 * 
 * @param    pos ���Фγ��ϰ���
 * @param    str �����оݤ�ʸ�����å�
 * @param    nextpos �����pos�����������
 * @return   �����������������<br>
 *           ���Ԥ�����������
 * @throw    �����Хåե��γ��ݤ˼��Ԥ������
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
	    if ( p0 == 0 ) {	/* �ѥ����󤬤������� */
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
 * @brief  ʸ�����åȤ˴ޤޤ��ʸ���򡤱�¦���鸡��
 *
 *  ���Ȥ�����ʸ����α�¦���麸�����ˡ�ʸ���󥻥å� str �˴ޤޤ��ʸ���򸡺���
 *  �ǽ�˽и�������֤��֤��ޤ���
 *  �ʤ���ʸ�������Ƭ���֤� 0 �Ǥ���
 * 
 * @param    str �����оݤ�ʸ�����å�
 * @return   �����������������<br>
 *           ���Ԥ�����������
 * @throw    �����Хåե��γ��ݤ˼��Ԥ������
 */
ssize_t tstring::strrpbrk( const tstring &str ) const
{

    if ( 0 < this->length() && str.cstr() != NULL ) 
        return this->strrpbrk(this->length() - 1, str.cstr());
    else return -1;
}

/**
 * @brief  ʸ�����åȤ˴ޤޤ��ʸ���򡤱�¦���鸡��
 *
 *  ���Ȥ�����ʸ����α�¦���麸�����ˡ�ʸ���󥻥å� str �˴ޤޤ��ʸ���򸡺���
 *  �ǽ�˽и�������֤��֤��ޤ���
 *  �ʤ���ʸ�������Ƭ���֤� 0 �Ǥ���
 * 
 * @param    pos ���Фγ��ϰ���
 * @param    str �����оݤ�ʸ�����å�
 * @return   �����������������<br>
 *           ���Ԥ�����������
 * @throw    �����Хåե��γ��ݤ˼��Ԥ������
 */
ssize_t tstring::strrpbrk( size_t pos, const tstring &str ) const
{
    if ( str.cstr() != NULL ) return this->strrpbrk(pos, str.cstr());
    else return -1;
}

/**
 * @brief  ʸ�����åȤ˴ޤޤ��ʸ���򡤱�¦����Ϣ³Ū�˸���
 *
 *  ���Ȥ�����ʸ����α�¦���麸�����ˡ�ʸ���󥻥å� str �˴ޤޤ��ʸ���򸡺���
 *  �ǽ�˽и�������֤��֤��ޤ���
 *  �ʤ���ʸ�������Ƭ���֤� 0 �Ǥ���
 * 
 * @param    pos ���Фγ��ϰ���
 * @param    str �����оݤ�ʸ�����å�
 * @param    nextpos �����pos�����������
 * @return   �����������������<br>
 *           ���Ԥ�����������
 * @throw    �����Хåե��γ��ݤ˼��Ԥ������
 */
ssize_t tstring::strrpbrk( size_t pos, const tstring &str,
			   size_t *nextpos ) const
{
    if ( str.cstr() != NULL ) return this->strrpbrk(pos, str.cstr(), nextpos);
    else return -1;
}

/**
 * @brief  ʸ�����åȤ˴ޤޤ��ʸ����Ϣ³����Ĺ�������
 *
 *  ���Ȥ�����ʸ����κ�¦���鱦�����ˡ�ʸ�����å� accept ��
 *  Ϣ³����Ĺ���򸡺���������Ĺ�����֤��ޤ���
 *
 * @param      accept �����оݤ�ʸ�����å�
 * @return     ����������������<br>
 *             ���Ԥ�������0
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
size_t tstring::strspn( const char *accept ) const
{
    return this->strspn((size_t)0,accept);
}

/**
 * @brief  ʸ�����åȤ˴ޤޤ��ʸ����Ϣ³����Ĺ�������
 *
 *  ���Ȥ�����ʸ����κ�¦���鱦�����ˡ�ʸ�����å� accept ��
 *  Ϣ³����Ĺ���򸡺���������Ĺ�����֤��ޤ���
 *
 * @param      pos ���Фγ��ϰ���
 * @param      accept �����оݤ�ʸ�����å�
 * @return     ����������������<br>
 *             ���Ԥ�������0
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
size_t tstring::strspn( size_t pos, const char *accept ) const
{
    return this->strspn(pos,accept,(size_t *)NULL);
}

/**
 * @brief  ʸ�����åȤ˴ޤޤ��ʸ����Ϣ³����Ĺ����Ϣ³Ū�˼���
 *
 *  ���Ȥ�����ʸ����κ�¦���鱦�����ˡ�ʸ�����å� accept ��
 *  Ϣ³����Ĺ���򸡺���������Ĺ�����֤��ޤ���
 *
 * @param      pos ���Фγ��ϰ���
 * @param      accept �����оݤ�ʸ�����å�
 * @param      nextpos �����pos
 * @return     ����������������<br>
 *             ���Ԥ�������0
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
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
		if ( p0 == 0 ) {	/* �ѥ����󤬤������� */
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
 * @brief  ʸ�����åȤ˴ޤޤ��ʸ����Ϣ³����Ĺ�������
 *
 *  ���Ȥ�����ʸ����κ�¦���鱦�����ˡ�ʸ�����å� accept ��
 *  Ϣ³����Ĺ���򸡺���������Ĺ�����֤��ޤ���
 *
 * @param      accept �����оݤ�ʸ�����å�
 * @return     ����������������<br>
 *             ���Ԥ�������0
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
size_t tstring::strspn( const tstring &accept ) const
{
    return this->strspn((size_t)0, accept.cstr());
}

/**
 * @brief  ʸ�����åȤ˴ޤޤ��ʸ����Ϣ³����Ĺ�������
 *
 *  ���Ȥ�����ʸ����κ�¦���鱦�����ˡ�ʸ�����å� accept ��
 *  Ϣ³����Ĺ���򸡺���������Ĺ�����֤��ޤ���
 *
 * @param      pos ���Фγ��ϰ���
 * @param      accept �����оݤ�ʸ�����å�
 * @return     ����������������<br>
 *             ���Ԥ�������0
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
size_t tstring::strspn( size_t pos, const tstring &accept ) const
{
    return this->strspn(pos, accept.cstr());
}

/**
 * @brief  ʸ�����åȤ˴ޤޤ��ʸ����Ϣ³����Ĺ����Ϣ³Ū�˼���
 *
 *  ���Ȥ�����ʸ����κ�¦���鱦�����ˡ�ʸ�����å� accept ��
 *  Ϣ³����Ĺ���򸡺���������Ĺ�����֤��ޤ���
 *
 * @param      pos ���Фγ��ϰ���
 * @param      accept �����оݤ�ʸ�����å�
 * @param      nextpos �����pos
 * @return     ����������������<br>
 *             ���Ԥ�������0
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
size_t tstring::strspn( size_t pos, const tstring &accept, 
			size_t *nextpos ) const
{
    return this->strspn(pos, accept.cstr(), nextpos);
}

/**
 * @brief  ʸ����Ϣ³����Ĺ�������
 *
 *  ���Ȥ�����ʸ����κ�¦���鱦�����ˡ�ʸ�����å� accept ��
 *  Ϣ³����Ĺ���򸡺���������Ĺ�����֤��ޤ���
 *
 * @param      accept �����оݤ�ʸ�����å�
 * @return     ����������������<br>
 *             ���Ԥ�������0
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
size_t tstring::strspn( int accept ) const
{
    return this->strspn((size_t)0,accept);
}

/**
 * @brief  ʸ����Ϣ³����Ĺ�������
 *
 *  ���Ȥ�����ʸ����κ�¦���鱦�����ˡ�ʸ�����å� accept ��
 *  Ϣ³����Ĺ���򸡺���������Ĺ�����֤��ޤ���
 *
 * @param      pos ���Фγ��ϰ���
 * @param      accept �����оݤ�ʸ�����å�
 * @return     ����������������<br>
 *             ���Ԥ�������0
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
size_t tstring::strspn( size_t pos, int accept ) const
{
    return this->strspn(pos,accept,(size_t *)NULL);
}

/**
 * @brief  ʸ����Ϣ³����Ĺ����Ϣ³Ū�˼���
 *
 *  ���Ȥ�����ʸ����κ�¦���鱦�����ˡ�ʸ�����å� accept ��
 *  Ϣ³����Ĺ���򸡺���������Ĺ�����֤��ޤ���
 *
 * @param      pos ���Фγ��ϰ���
 * @param      accept �����оݤ�ʸ�����å�
 * @param      nextpos �����pos
 * @return     ����������������<br>
 *             ���Ԥ�������0
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
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
 * @brief  ʸ�����åȤ˴ޤޤ��ʸ����Ϣ³����Ĺ�������(���������鸡��)
 *
 *  ���Ȥ�����ʸ����α�¦���麸�����ˡ�ʸ�����å� accept ��
 *  Ϣ³����Ĺ���򸡺���������Ĺ�����֤��ޤ���
 *
 * @param      accept �����оݤ�ʸ�����å�
 * @return     ����������������<br>
 *             ���Ԥ�������0
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
size_t tstring::strrspn( const char *accept ) const
{
    if ( 0 < this->length() )
	return this->strrspn(this->length() - 1, accept);
    return 0;
}

/**
 * @brief  ʸ�����åȤ˴ޤޤ��ʸ����Ϣ³����Ĺ�������(���������鸡��)
 *
 *  ���Ȥ�����ʸ����α�¦���麸�����ˡ�ʸ�����å� accept ��
 *  Ϣ³����Ĺ���򸡺���������Ĺ�����֤��ޤ���
 *
 * @param      pos ���Фγ��ϰ���
 * @param      accept �����оݤ�ʸ�����å�
 * @return     ����������������<br>
 *             ���Ԥ�������0
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
size_t tstring::strrspn( size_t pos, const char *accept ) const
{
    return this->strrspn(pos,accept,(size_t *)NULL);
}

/**
 * @brief  ʸ�����åȤ˴ޤޤ��ʸ����Ϣ³����Ĺ�������(����������Ϣ³Ū�˸���)
 *
 *  ���Ȥ�����ʸ����α�¦���麸�����ˡ�ʸ�����å� accept ��
 *  Ϣ³����Ĺ���򸡺���������Ĺ�����֤��ޤ���
 *
 * @param      pos ���Фγ��ϰ���
 * @param      accept �����оݤ�ʸ�����å�
 * @param      nextpos �����pos
 * @return     ����������������<br>
 *             ���Ԥ�������0
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
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
		if ( p0 == 0 ) {	/* �ѥ����󤬤������� */
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
 * @brief  ʸ�����åȤ˴ޤޤ��ʸ����Ϣ³����Ĺ�������(���������鸡��)
 *
 *  ���Ȥ�����ʸ����α�¦���麸�����ˡ�ʸ�����å� accept ��
 *  Ϣ³����Ĺ���򸡺���������Ĺ�����֤��ޤ���
 *
 * @param      accept �����оݤ�ʸ�����å�
 * @return     ����������������<br>
 *             ���Ԥ�������0
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
size_t tstring::strrspn( const tstring &accept ) const
{
    if ( 0 < this->length() )
	return this->strrspn(this->length() - 1, accept.cstr());
    return 0;
}

/**
 * @brief  ʸ�����åȤ˴ޤޤ��ʸ����Ϣ³����Ĺ�������(���������鸡��)
 *
 *  ���Ȥ�����ʸ����α�¦���麸�����ˡ�ʸ�����å� accept ��
 *  Ϣ³����Ĺ���򸡺���������Ĺ�����֤��ޤ���
 *
 * @param      pos ���Фγ��ϰ���
 * @param      accept �����оݤ�ʸ�����å�
 * @return     ����������������<br>
 *             ���Ԥ�������0
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
size_t tstring::strrspn( size_t pos, const tstring &accept ) const
{
    return this->strrspn(pos, accept.cstr());
}

/**
 * @brief  ʸ�����åȤ˴ޤޤ��ʸ����Ϣ³����Ĺ�������(����������Ϣ³Ū�˸���)
 *
 *  ���Ȥ�����ʸ����α�¦���麸�����ˡ�ʸ�����å� accept ��
 *  Ϣ³����Ĺ���򸡺���������Ĺ�����֤��ޤ���
 *
 * @param      pos ���Фγ��ϰ���
 * @param      accept �����оݤ�ʸ�����å�
 * @param      nextpos �����pos
 * @return     ����������������<br>
 *             ���Ԥ�������0
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
size_t tstring::strrspn( size_t pos, const tstring &accept, 
			size_t *nextpos ) const
{
    return this->strrspn(pos, accept.cstr(), nextpos);
}

/**
 * @brief  ʸ����Ϣ³����Ĺ�������(���������鸡��)
 *
 *  ���Ȥ�����ʸ����α�¦���麸�����ˡ�ʸ�����å� accept ��
 *  Ϣ³����Ĺ���򸡺���������Ĺ�����֤��ޤ���
 *
 * @param      accept �����оݤ�ʸ�����å�
 * @return     ����������������<br>
 *             ���Ԥ�������0
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
size_t tstring::strrspn( int accept ) const
{
    if ( 0 < this->length() )
	return this->strrspn(this->length() - 1,accept);
    return 0;
}

/**
 * @brief  ʸ����Ϣ³����Ĺ�������(���������鸡��)
 *
 *  ���Ȥ�����ʸ����α�¦���麸�����ˡ�ʸ�����å� accept ��
 *  Ϣ³����Ĺ���򸡺���������Ĺ�����֤��ޤ���
 *
 * @param      pos ���Фγ��ϰ���
 * @param      accept �����оݤ�ʸ�����å�
 * @return     ����������������<br>
 *             ���Ԥ�������0
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
size_t tstring::strrspn( size_t pos, int accept ) const
{
    return this->strrspn(pos,accept,(size_t *)NULL);
}

/**
 * @brief  ʸ����Ϣ³����Ĺ�������(����������Ϣ³Ū�˸���)
 *
 *  ���Ȥ�����ʸ����α�¦���麸�����ˡ�ʸ�����å� accept ��
 *  Ϣ³����Ĺ���򸡺���������Ĺ�����֤��ޤ���
 *
 * @param      pos ���Фγ��ϰ���
 * @param      accept �����оݤ�ʸ�����å�
 * @param      nextpos �����pos
 * @return     ����������������<br>
 *             ���Ԥ�������0
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
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
 * @brief  ʸ�����åȤ˴ޤޤ�ʤ�ʸ����Ϣ³����Ĺ�������
 *
 *  ���Ȥ�����ʸ����κ�¦���鱦�����ˡ�ʸ�����å� reject ���ǽ�˽и�����
 *  �ޤǤ�ʸ�����Ϣ³����Ĺ���򸡺���������Ĺ�����֤��ޤ���
 *
 * @param      reject �����оݳ���ʸ�����å�
 * @return     ����������������<br>
 *             ���Ԥ�������0
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
size_t tstring::strcspn( const char *reject ) const
{
    return this->strcspn((size_t)0,reject);
}

/**
 * @brief  ʸ�����åȤ˴ޤޤ�ʤ�ʸ����Ϣ³����Ĺ�������
 *
 *  ���Ȥ�����ʸ����κ�¦���鱦�����ˡ�ʸ�����å� reject ���ǽ�˽и�����
 *  �ޤǤ�ʸ�����Ϣ³����Ĺ���򸡺���������Ĺ�����֤��ޤ���
 *
 * @param      pos ���Фγ��ϰ���
 * @param      reject �����оݳ���ʸ�����å�
 * @return     ����������������<br>
 *             ���Ԥ�������0
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
size_t tstring::strcspn( size_t pos, const char *reject ) const
{
    return this->strcspn(pos,reject,(size_t *)NULL);
}

/**
 * @brief  ʸ�����åȤ˴ޤޤ�ʤ�ʸ����Ϣ³����Ĺ�������(Ϣ³Ū�˸���)
 *
 *  ���Ȥ�����ʸ����κ�¦���鱦�����ˡ�ʸ�����å� reject ���ǽ�˽и�����
 *  �ޤǤ�ʸ�����Ϣ³����Ĺ���򸡺���������Ĺ�����֤��ޤ���
 *
 * @param      pos ���Фγ��ϰ���
 * @param      reject �����оݳ���ʸ�����å�
 * @param      nextpos �����pos
 * @return     ����������������<br>
 *             ���Ԥ�������0
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
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
		if ( p0 == 0 ) {	/* �ѥ����󤬤������� */
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
 * @brief  ʸ�����åȤ˴ޤޤ�ʤ�ʸ����Ϣ³����Ĺ�������
 *
 *  ���Ȥ�����ʸ����κ�¦���鱦�����ˡ�ʸ�����å� reject ���ǽ�˽и�����
 *  �ޤǤ�ʸ�����Ϣ³����Ĺ���򸡺���������Ĺ�����֤��ޤ���
 *
 * @param      reject �����оݳ���ʸ�����å�
 * @return     ����������������<br>
 *             ���Ԥ�������0
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
size_t tstring::strcspn( const tstring &reject ) const
{
    return this->strcspn((size_t)0, reject.cstr());
}

/**
 * @brief  ʸ�����åȤ˴ޤޤ�ʤ�ʸ����Ϣ³����Ĺ�������
 *
 *  ���Ȥ�����ʸ����κ�¦���鱦�����ˡ�ʸ�����å� reject ���ǽ�˽и�����
 *  �ޤǤ�ʸ�����Ϣ³����Ĺ���򸡺���������Ĺ�����֤��ޤ���
 *
 * @param      pos ���Фγ��ϰ���
 * @param      reject �����оݳ���ʸ�����å�
 * @return     ����������������<br>
 *             ���Ԥ�������0
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
size_t tstring::strcspn( size_t pos, const tstring &reject ) const
{
    return this->strcspn(pos, reject.cstr());
}

/**
 * @brief  ʸ�����åȤ˴ޤޤ�ʤ�ʸ����Ϣ³����Ĺ�������(Ϣ³Ū�˸���)
 *
 *  ���Ȥ�����ʸ����κ�¦���鱦�����ˡ�ʸ�����å� reject ���ǽ�˽и�����
 *  �ޤǤ�ʸ�����Ϣ³����Ĺ���򸡺���������Ĺ�����֤��ޤ���
 *
 * @param      pos ���Фγ��ϰ���
 * @param      reject �����оݳ���ʸ�����å�
 * @param      nextpos �����pos
 * @return     ����������������<br>
 *             ���Ԥ�������0
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
size_t tstring::strcspn( size_t pos, const tstring &reject, 
			 size_t *nextpos ) const
{
    return this->strcspn(pos, reject.cstr(), nextpos);
}

/**
 * @brief  ���ꤵ�줿ʸ���ǤϤʤ�ʸ����Ϣ³����Ĺ�����֤�
 *
 *  ���Ȥ�����ʸ����κ�¦���鱦�����ˡ�ʸ�����å� reject ���ǽ�˽и�����
 *  �ޤǤ�ʸ�����Ϣ³����Ĺ���򸡺���������Ĺ�����֤��ޤ���
 *
 * @param      reject �����оݳ���ʸ�����å�
 * @return     ����������������<br>
 *             ���Ԥ�������0
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
size_t tstring::strcspn( int reject ) const
{
    return this->strcspn((size_t)0,reject);
}

/**
 * @brief  ���ꤵ�줿ʸ���ǤϤʤ�ʸ����Ϣ³����Ĺ�����֤�
 *
 *  ���Ȥ�����ʸ����κ�¦���鱦�����ˡ�ʸ�����å� reject ���ǽ�˽и�����
 *  �ޤǤ�ʸ�����Ϣ³����Ĺ���򸡺���������Ĺ�����֤��ޤ���
 *
 * @param      pos ���Фγ��ϰ���
 * @param      reject �����оݳ���ʸ�����å�
 * @return     ����������������<br>
 *             ���Ԥ�������0
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
size_t tstring::strcspn( size_t pos, int reject ) const
{
    return this->strcspn(pos,reject,(size_t *)NULL);
}

/**
 * @brief  ���ꤵ�줿ʸ���ǤϤʤ�ʸ����Ϣ³����Ĺ�����֤�(Ϣ³Ū�˸���)
 *
 *  ���Ȥ�����ʸ����κ�¦���鱦�����ˡ�ʸ�����å� reject ���ǽ�˽и�����
 *  �ޤǤ�ʸ�����Ϣ³����Ĺ���򸡺���������Ĺ�����֤��ޤ���
 *
 * @param      pos ���Фγ��ϰ���
 * @param      reject �����оݳ���ʸ�����å�
 * @param      nextpos �����pos
 * @return     ����������������<br>
 *             ���Ԥ�������0
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
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
 * @brief  ������Υ磻��ɥ����ɥѥ�������Ѥ���ʸ����ޥå�����
 *
 *  ���Ȥ�ʸ������Ф���������Υ磻��ɥ����ɥѥ�������Ѥ���ʸ����ޥå���
 *  ��Ԥ������η�̤��֤��ޤ���
 *
 * @param      pat ʸ���ѥ�����
 * @return     ������������0<br>
 *             ���Ԥ�����������
 *
 */
int tstring::strmatch( const char *pat ) const
{
    const char *this_str_ptr = this->str_ptr_cs();

    if ( this_str_ptr == NULL || pat == NULL ) return -1;

    return c_strmatch(pat,this_str_ptr) == 0 ? 0 : -1;
}

/**
 * @brief  ������Υ磻��ɥ����ɥѥ�������Ѥ���ʸ����ޥå�����
 *
 *  ���Ȥ�ʸ������Ф���������Υ磻��ɥ����ɥѥ�������Ѥ���ʸ����ޥå���
 *  ��Ԥ������η�̤��֤��ޤ���
 *
 * @param      pos ʸ����ޥå��γ��ϰ���
 * @param      pat ʸ���ѥ�����
 * @return     ������������0<br>
 *             ���Ԥ�����������
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
 * @brief  ������Υ磻��ɥ����ɥѥ�������Ѥ���ʸ����ޥå�����
 *
 *  ���Ȥ�ʸ������Ф���������Υ磻��ɥ����ɥѥ�������Ѥ���ʸ����ޥå���
 *  ��Ԥ������η�̤��֤��ޤ���
 *
 * @param      pat ʸ���ѥ�����
 * @return     ������������0<br>
 *             ���Ԥ�����������
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
 * @brief  ������Υ磻��ɥ����ɥѥ�������Ѥ���ʸ����ޥå�����
 *
 *  ���Ȥ�ʸ������Ф���������Υ磻��ɥ����ɥѥ�������Ѥ���ʸ����ޥå���
 *  ��Ԥ������η�̤��֤��ޤ���
 *
 * @param      pos ʸ����ޥå��γ��ϰ���
 * @param      pat ʸ���ѥ�����
 * @return     ������������0<br>
 *             ���Ԥ�����������
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
 * @brief  ������Υ磻��ɥ����ɥѥ�������Ѥ���ʸ����ޥå�(�ե�����̾����)
 *
 *  ���Ȥ�ʸ������Ф���������Υ磻��ɥ����ɥѥ�������Ѥ���ʸ����ޥå���
 *  ��Ԥ������η�̤��֤��ޤ���<br>
 *  ���Ȥ�ʸ�������Ƭ�Υԥꥪ�ɤ� pat ��Υԥꥪ�ɤ��Τ�Τˤ����ޥå�
 *  ���ޤ���
 *
 * @param      pat ʸ���ѥ�����
 * @return     ������������0<br>
 *             ���Ԥ�����������
 *
 */
int tstring::fnmatch( const char *pat ) const
{
    const char *this_str_ptr = this->str_ptr_cs();

    if ( this_str_ptr == NULL || pat == NULL ) return -1;

    return c_fnmatch(pat,this_str_ptr) == 0 ? 0 : -1;
}

/**
 * @brief  ������Υ磻��ɥ����ɥѥ�������Ѥ���ʸ����ޥå�(�ե�����̾����)
 *
 *  ���Ȥ�ʸ������Ф���������Υ磻��ɥ����ɥѥ�������Ѥ���ʸ����ޥå���
 *  ��Ԥ������η�̤��֤��ޤ���<br>
 *  ���Ȥ�ʸ�������Ƭ�Υԥꥪ�ɤ� pat ��Υԥꥪ�ɤ��Τ�Τˤ����ޥå�
 *  ���ޤ���
 *
 * @param      pos ʸ����ޥå��γ��ϰ���
 * @param      pat ʸ���ѥ�����
 * @return     ������������0<br>
 *             ���Ԥ�����������
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
 * @brief  ������Υ磻��ɥ����ɥѥ�������Ѥ���ʸ����ޥå�(�ե�����̾����)
 *
 *  ���Ȥ�ʸ������Ф���������Υ磻��ɥ����ɥѥ�������Ѥ���ʸ����ޥå���
 *  ��Ԥ������η�̤��֤��ޤ���<br>
 *  ���Ȥ�ʸ�������Ƭ�Υԥꥪ�ɤ� pat ��Υԥꥪ�ɤ��Τ�Τˤ����ޥå�
 *  ���ޤ���
 *
 * @param      pat ʸ���ѥ�����
 * @return     ������������0<br>
 *             ���Ԥ�����������
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
 * @brief  ������Υ磻��ɥ����ɥѥ�������Ѥ���ʸ����ޥå�(�ե�����̾����)
 *
 *  ���Ȥ�ʸ������Ф���������Υ磻��ɥ����ɥѥ�������Ѥ���ʸ����ޥå���
 *  ��Ԥ������η�̤��֤��ޤ���<br>
 *  ���Ȥ�ʸ�������Ƭ�Υԥꥪ�ɤ� pat ��Υԥꥪ�ɤ��Τ�Τˤ����ޥå�
 *  ���ޤ���
 *
 * @param      pos ʸ����ޥå��γ��ϰ���
 * @param      pat ʸ���ѥ�����
 * @return     ������������0<br>
 *             ���Ԥ�����������
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
 * @brief  ������Υ磻��ɥ����ɥѥ�������Ѥ���ʸ����ޥå�(�ѥ�̾����)
 *
 *  ���Ȥ�ʸ������Ф���������Υ磻��ɥ����ɥѥ�������Ѥ���ʸ����ޥå���
 *  ��Ԥ������η�̤��֤��ޤ���<br>
 *  ���Ȥ�ʸ�������Ƭ�Υԥꥪ�ɡ�����å����ľ��Υԥꥪ�ɡ�����å��� '/'
 *  �� pat ��Υ磻��ɥ����ɤ� [] �����������ˤϥޥå����ޤ���
 *
 * @param      pat ʸ���ѥ�����
 * @return     ������������0<br>
 *             ���Ԥ�����������
 *
 */
int tstring::pnmatch( const char *pat ) const
{
    const char *this_str_ptr = this->str_ptr_cs();

    if ( this_str_ptr == NULL || pat == NULL ) return -1;

    return c_pnmatch(pat,this_str_ptr) == 0 ? 0 : -1;
}

/**
 * @brief  ������Υ磻��ɥ����ɥѥ�������Ѥ���ʸ����ޥå�(�ѥ�̾����)
 *
 *  ���Ȥ�ʸ������Ф���������Υ磻��ɥ����ɥѥ�������Ѥ���ʸ����ޥå���
 *  ��Ԥ������η�̤��֤��ޤ���<br>
 *  ���Ȥ�ʸ�������Ƭ�Υԥꥪ�ɡ�����å����ľ��Υԥꥪ�ɡ�����å��� '/'
 *  �� pat ��Υ磻��ɥ����ɤ� [] �����������ˤϥޥå����ޤ���
 *
 * @param      pos ʸ����ޥå��γ��ϰ���
 * @param      pat ʸ���ѥ�����
 * @return     ������������0<br>
 *             ���Ԥ�����������
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
 * @brief  ������Υ磻��ɥ����ɥѥ�������Ѥ���ʸ����ޥå�(�ѥ�̾����)
 *
 *  ���Ȥ�ʸ������Ф���������Υ磻��ɥ����ɥѥ�������Ѥ���ʸ����ޥå���
 *  ��Ԥ������η�̤��֤��ޤ���<br>
 *  ���Ȥ�ʸ�������Ƭ�Υԥꥪ�ɡ�����å����ľ��Υԥꥪ�ɡ�����å��� '/'
 *  �� pat ��Υ磻��ɥ����ɤ� [] �����������ˤϥޥå����ޤ���
 *
 * @param      pat ʸ���ѥ�����
 * @return     ������������0<br>
 *             ���Ԥ�����������
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
 * @brief  ������Υ磻��ɥ����ɥѥ�������Ѥ���ʸ����ޥå�(�ѥ�̾����)
 *
 *  ���Ȥ�ʸ������Ф���������Υ磻��ɥ����ɥѥ�������Ѥ���ʸ����ޥå���
 *  ��Ԥ������η�̤��֤��ޤ���<br>
 *  ���Ȥ�ʸ�������Ƭ�Υԥꥪ�ɡ�����å����ľ��Υԥꥪ�ɡ�����å��� '/'
 *  �� pat ��Υ磻��ɥ����ɤ� [] �����������ˤϥޥå����ޤ���
 *
 * @param      pos ʸ����ޥå��γ��ϰ���
 * @param      pat ʸ���ѥ�����
 * @return     ������������0<br>
 *             ���Ԥ�����������
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
/* �����˥��㥹�Ȥ���ؿ� */
static tregex *coerce_into_casting( const tregex *in )
{ 
    return (tregex *)in;
} 
#endif

/**
 * @brief  ����ɽ���ˤ��ʸ����ޥå�����
 *
 *  ���Ȥ�ʸ������Ф���POSIX ��ĥ����ɽ��(�ʲ�������ɽ��) �ˤ��ʸ����ޥå���
 *  ��Ԥ������η�̤��֤��ޤ���
 *
 * @param      pat ʸ���ѥ�����(����ɽ��)
 * @param      ret_span �ޥå�����ʸ�����Ĺ��
 * @return     ������������������<br>
 *             ���Ԥ�����������
 * @throw      regex�롼���󤬥����Ȥ��̤����Ƥ�����
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * @throw      �����˲��򵯤��������
 *
 */
ssize_t tstring::regmatch( const char *pat, size_t *ret_span ) const
{
    return this->regmatch((size_t)0, pat, ret_span, (size_t *)NULL);
}

/**
 * @brief  ����ɽ���ˤ��ʸ����ޥå�����
 *
 *  ���Ȥ�ʸ������Ф���POSIX ��ĥ����ɽ��(�ʲ�������ɽ��) �ˤ��ʸ����ޥå���
 *  ��Ԥ������η�̤��֤��ޤ���
 *
 * @param      pos ʸ����ޥå��γ��ϰ���
 * @param      pat ʸ���ѥ�����(����ɽ��)
 * @param      ret_span �ޥå�����ʸ�����Ĺ��
 * @return     ������������������<br>
 *             ���Ԥ�����������
 * @throw      regex�롼���󤬥����Ȥ��̤����Ƥ�����
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * @throw      �����˲��򵯤��������
 *
 */
ssize_t tstring::regmatch( size_t pos, const char *pat, 
			   size_t *ret_span ) const
{
    return this->regmatch(pos, pat, ret_span, (size_t *)NULL);
}

/**
 * @brief  ����ɽ���ˤ��ʸ����ޥå���Ϣ³Ū�˻��
 *
 *  ���Ȥ�ʸ������Ф���POSIX ��ĥ����ɽ��(�ʲ�������ɽ��) �ˤ��ʸ����ޥå���
 *  ��Ԥ������η�̤��֤��ޤ���
 *
 * @param      pos ʸ����ޥå��γ��ϰ���
 * @param      pat ʸ���ѥ�����(����ɽ��)
 * @param      ret_span �ޥå�����ʸ�����Ĺ��
 * @param      nextpos �����pos
 * @return     ������������������<br>
 *             ���Ԥ�����������
 * @throw      regex�롼���󤬥����Ȥ��̤����Ƥ�����
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * @throw      �����˲��򵯤��������
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
 * @brief  ����ɽ���ˤ��ʸ����ޥå�����
 *
 *  ���Ȥ�ʸ������Ф���POSIX ��ĥ����ɽ��(�ʲ�������ɽ��) �ˤ��ʸ����ޥå���
 *  ��Ԥ������η�̤��֤��ޤ���
 *
 * @param      pat ʸ���ѥ�����(����ɽ��)
 * @param      ret_span �ޥå�����ʸ�����Ĺ��
 * @return     ������������������<br>
 *             ���Ԥ�����������
 * @throw      regex�롼���󤬥����Ȥ��̤����Ƥ�����
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * @throw      �����˲��򵯤��������
 *
 */
ssize_t tstring::regmatch( const tstring &pat, size_t *ret_span ) const
{
    return this->regmatch((size_t)0, pat, ret_span, (size_t *)NULL);
}

/**
 * @brief  ����ɽ���ˤ��ʸ����ޥå�����
 *
 *  ���Ȥ�ʸ������Ф���POSIX ��ĥ����ɽ��(�ʲ�������ɽ��) �ˤ��ʸ����ޥå���
 *  ��Ԥ������η�̤��֤��ޤ���
 *
 * @param      pos ʸ����ޥå��γ��ϰ���
 * @param      pat ʸ���ѥ�����(����ɽ��)
 * @param      ret_span �ޥå�����ʸ�����Ĺ��
 * @return     ������������������<br>
 *             ���Ԥ�����������
 * @throw      regex�롼���󤬥����Ȥ��̤����Ƥ�����
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * @throw      �����˲��򵯤��������
 *
 */
ssize_t tstring::regmatch( size_t pos, const tstring &pat, 
			   size_t *ret_span ) const
{
    return this->regmatch(pos, pat, ret_span, (size_t *)NULL);
}

/**
 * @brief  ����ɽ���ˤ��ʸ����ޥå���Ϣ³Ū�˻��
 *
 *  ���Ȥ�ʸ������Ф���POSIX ��ĥ����ɽ��(�ʲ�������ɽ��) �ˤ��ʸ����ޥå���
 *  ��Ԥ������η�̤��֤��ޤ���
 *
 * @param      pos ʸ����ޥå��γ��ϰ���
 * @param      pat ʸ���ѥ�����(����ɽ��)
 * @param      ret_span �ޥå�����ʸ�����Ĺ��
 * @param      nextpos �����pos
 * @return     ������������������<br>
 *             ���Ԥ�����������
 * @throw      regex�롼���󤬥����Ȥ��̤����Ƥ�����
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * @throw      �����˲��򵯤��������
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
 * @brief  ����ɽ���ˤ��ʸ����ޥå�����
 *
 *  ���Ȥ�ʸ������Ф���POSIX ��ĥ����ɽ��(�ʲ�������ɽ��) �ˤ��ʸ����ޥå���
 *  ��Ԥ������η�̤��֤��ޤ���
 *
 * @param      pat tregex ���饹�Υ���ѥ���ѥ��֥�������
 * @param      ret_span �ޥå�����ʸ�����Ĺ��
 * @return     ������������������<br>
 *             ���Ԥ�����������
 * @throw      regex�롼���󤬥����Ȥ��̤����Ƥ�����
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * @throw      �����˲��򵯤��������
 *
 */
ssize_t tstring::regmatch( const tregex &pat, size_t *ret_span ) const
{
    return this->regexp_match((size_t)0, pat, ret_span);
}

/**
 * @brief  ����ɽ���ˤ��ʸ����ޥå�����
 *
 *  ���Ȥ�ʸ������Ф���POSIX ��ĥ����ɽ��(�ʲ�������ɽ��) �ˤ��ʸ����ޥå���
 *  ��Ԥ������η�̤��֤��ޤ���
 *
 * @param      pos ʸ����ޥå��γ��ϰ���
 * @param      pat tregex ���饹�Υ���ѥ���ѥ��֥�������
 * @param      ret_span �ޥå�����ʸ�����Ĺ��
 * @return     ������������������<br>
 *             ���Ԥ�����������
 * @throw      regex�롼���󤬥����Ȥ��̤����Ƥ�����
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * @throw      �����˲��򵯤��������
 *
 */
ssize_t tstring::regmatch( size_t pos, const tregex &pat, 
			   size_t *ret_span ) const
{
    return this->regexp_match(pos, pat, ret_span);
}

/**
 * @brief  ����ɽ���ˤ��ʸ����ޥå���Ϣ³Ū�˻��
 *
 *  ���Ȥ�ʸ������Ф���POSIX ��ĥ����ɽ��(�ʲ�������ɽ��) �ˤ��ʸ����ޥå���
 *  ��Ԥ������η�̤��֤��ޤ���
 *
 * @param      pos ʸ����ޥå��γ��ϰ���
 * @param      pat tregex ���饹�Υ���ѥ���ѥ��֥�������
 * @param      ret_span �ޥå�����ʸ�����Ĺ��
 * @param      nextpos �����pos
 * @return     ������������������<br>
 *             ���Ԥ�����������
 * @throw      regex�롼���󤬥����Ȥ��̤����Ƥ�����
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * @throw      �����˲��򵯤��������
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
 * @brief  ����ɽ���ˤ��ʸ����ޥå�����
 *
 *  ���Ȥ�ʸ������Ф���POSIX ��ĥ����ɽ��(�ʲ�������ɽ��) �ˤ��ʸ����ޥå���
 *  ��Ԥ����������Ȥξ����ޤ��̤���������Ǥ��ޤ���
 *
 * @param      pat ʸ���ѥ�����(����ɽ��)
 * @param      max_nelem pos_r��len_r �ΥХåե�Ĺ
 * @param      pos_r �ޥå�����ʸ����ΰ���
 * @param      len_r �ޥå�����ʸ�����Ĺ��
 * @param      nelem_r pos_r��len_r �˥��åȤ��줿�ͤθĿ�
 * @return     ������������������<br>
 *             ���Ԥ�����������
 * @throw      regex�롼���󤬥����Ȥ��̤����Ƥ�����
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * @throw      �����˲��򵯤��������
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
 * @brief  ����ɽ���ˤ��ʸ����ޥå�����
 *
 *  ���Ȥ�ʸ������Ф���POSIX ��ĥ����ɽ��(�ʲ�������ɽ��) �ˤ��ʸ����ޥå���
 *  ��Ԥ����������Ȥξ����ޤ��̤���������Ǥ��ޤ���
 *
 * @param      pos ʸ����ޥå��γ��ϰ���
 * @param      pat ʸ���ѥ�����(����ɽ��)
 * @param      max_nelem pos_r��len_r �ΥХåե�Ĺ
 * @param      pos_r �ޥå�����ʸ����ΰ���
 * @param      len_r �ޥå�����ʸ�����Ĺ��
 * @param      nelem_r pos_r��len_r �˥��åȤ��줿�ͤθĿ�
 * @return     ������������������<br>
 *             ���Ԥ�����������
 * @throw      regex�롼���󤬥����Ȥ��̤����Ƥ�����
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * @throw      �����˲��򵯤��������
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
 * @brief  ����ɽ���ˤ��ʸ����ޥå���Ϣ³Ū�˻��
 *
 *  ���Ȥ�ʸ������Ф���POSIX ��ĥ����ɽ��(�ʲ�������ɽ��) �ˤ��ʸ����ޥå���
 *  ��Ԥ����������Ȥξ����ޤ��̤���������Ǥ��ޤ���
 *
 * @param      pos ʸ����ޥå��γ��ϰ���
 * @param      pat ʸ���ѥ�����(����ɽ��)
 * @param      max_nelem pos_r��len_r �ΥХåե�Ĺ
 * @param      pos_r �ޥå�����ʸ����ΰ���
 * @param      len_r �ޥå�����ʸ�����Ĺ��
 * @param      nelem_r pos_r��len_r �˥��åȤ��줿�ͤθĿ�
 * @param      nextpos �����pos
 * @return     ������������������<br>
 *             ���Ԥ�����������
 * @throw      regex�롼���󤬥����Ȥ��̤����Ƥ�����
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * @throw      �����˲��򵯤��������
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
 * @brief  ����ɽ���ˤ��ʸ����ޥå�����
 *
 *  ���Ȥ�ʸ������Ф���POSIX ��ĥ����ɽ��(�ʲ�������ɽ��) �ˤ��ʸ����ޥå���
 *  ��Ԥ����������Ȥξ����ޤ��̤���������Ǥ��ޤ���
 *
 * @param      pat ʸ���ѥ�����(����ɽ��)
 * @param      max_nelem pos_r��len_r �ΥХåե�Ĺ
 * @param      pos_r �ޥå�����ʸ����ΰ���
 * @param      len_r �ޥå�����ʸ�����Ĺ��
 * @param      nelem_r pos_r��len_r �˥��åȤ��줿�ͤθĿ�
 * @return     ������������������<br>
 *             ���Ԥ�����������
 * @throw      regex�롼���󤬥����Ȥ��̤����Ƥ�����
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * @throw      �����˲��򵯤��������
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
 * @brief  ����ɽ���ˤ��ʸ����ޥå�����
 *
 *  ���Ȥ�ʸ������Ф���POSIX ��ĥ����ɽ��(�ʲ�������ɽ��) �ˤ��ʸ����ޥå���
 *  ��Ԥ����������Ȥξ����ޤ��̤���������Ǥ��ޤ���
 *
 * @param      pos ʸ����ޥå��γ��ϰ���
 * @param      pat ʸ���ѥ�����(����ɽ��)
 * @param      max_nelem pos_r��len_r �ΥХåե�Ĺ
 * @param      pos_r �ޥå�����ʸ����ΰ���
 * @param      len_r �ޥå�����ʸ�����Ĺ��
 * @param      nelem_r pos_r��len_r �˥��åȤ��줿�ͤθĿ�
 * @return     ������������������<br>
 *             ���Ԥ�����������
 * @throw      regex�롼���󤬥����Ȥ��̤����Ƥ�����
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * @throw      �����˲��򵯤��������
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
 * @brief  ����ɽ���ˤ��ʸ����ޥå���Ϣ³Ū�˻��
 *
 *  ���Ȥ�ʸ������Ф���POSIX ��ĥ����ɽ��(�ʲ�������ɽ��) �ˤ��ʸ����ޥå���
 *  ��Ԥ����������Ȥξ����ޤ��̤���������Ǥ��ޤ���
 *
 * @param      pos ʸ����ޥå��γ��ϰ���
 * @param      pat ʸ���ѥ�����(����ɽ��)
 * @param      max_nelem pos_r��len_r �ΥХåե�Ĺ
 * @param      pos_r �ޥå�����ʸ����ΰ���
 * @param      len_r �ޥå�����ʸ�����Ĺ��
 * @param      nelem_r pos_r��len_r �˥��åȤ��줿�ͤθĿ�
 * @param      nextpos �����pos
 * @return     ������������������<br>
 *             ���Ԥ�����������
 * @throw      regex�롼���󤬥����Ȥ��̤����Ƥ�����
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * @throw      �����˲��򵯤��������
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
 * @brief  ����ɽ���ˤ��ʸ����ޥå�����
 *
 *  ���Ȥ�ʸ������Ф���POSIX ��ĥ����ɽ��(�ʲ�������ɽ��) �ˤ��ʸ����ޥå���
 *  ��Ԥ����������Ȥξ����ޤ��̤���������Ǥ��ޤ���
 *
 * @param      pat tregex ���饹�Υ���ѥ���ѥ��֥�������
 * @param      max_nelem pos_r��len_r �ΥХåե�Ĺ
 * @param      pos_r �ޥå�����ʸ����ΰ���
 * @param      len_r �ޥå�����ʸ�����Ĺ��
 * @param      nelem_r pos_r��len_r �˥��åȤ��줿�ͤθĿ�
 * @return     ������������������<br>
 *             ���Ԥ�����������
 * @throw      regex�롼���󤬥����Ȥ��̤����Ƥ�����
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * @throw      �����˲��򵯤��������
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
 * @brief  ����ɽ���ˤ��ʸ����ޥå�����
 *
 *  ���Ȥ�ʸ������Ф���POSIX ��ĥ����ɽ��(�ʲ�������ɽ��) �ˤ��ʸ����ޥå���
 *  ��Ԥ����������Ȥξ����ޤ��̤���������Ǥ��ޤ���
 *
 * @param      pos ʸ����ޥå��γ��ϰ���
 * @param      pat tregex ���饹�Υ���ѥ���ѥ��֥�������
 * @param      max_nelem pos_r��len_r �ΥХåե�Ĺ
 * @param      pos_r �ޥå�����ʸ����ΰ���
 * @param      len_r �ޥå�����ʸ�����Ĺ��
 * @param      nelem_r pos_r��len_r �˥��åȤ��줿�ͤθĿ�
 * @return     ������������������<br>
 *             ���Ԥ�����������
 * @throw      regex�롼���󤬥����Ȥ��̤����Ƥ�����
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * @throw      �����˲��򵯤��������
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
 * @brief  ����ɽ���ˤ��ʸ����ޥå���Ϣ³Ū�˻��
 *
 *  ���Ȥ�ʸ������Ф���POSIX ��ĥ����ɽ��(�ʲ�������ɽ��) �ˤ��ʸ����ޥå���
 *  ��Ԥ����������Ȥξ����ޤ��̤���������Ǥ��ޤ���
 *
 * @param      pos ʸ����ޥå��γ��ϰ���
 * @param      pat tregex ���饹�Υ���ѥ���ѥ��֥�������
 * @param      max_nelem pos_r��len_r �ΥХåե�Ĺ
 * @param      pos_r �ޥå�����ʸ����ΰ���
 * @param      len_r �ޥå�����ʸ�����Ĺ��
 * @param      nelem_r pos_r��len_r �˥��åȤ��줿�ͤθĿ�
 * @param      nextpos �����pos
 * @return     ������������������<br>
 *             ���Ԥ�����������
 * @throw      regex�롼���󤬥����Ȥ��̤����Ƥ�����
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * @throw      �����˲��򵯤��������
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
 * @brief  ����ɽ���ˤ��ʸ����ޥå����� (��侩)
 *
 *  ���Ȥ�ʸ������Ф���POSIX ��ĥ����ɽ��(�ʲ�������ɽ��) �ˤ��ʸ����ޥå���
 *  ��Ԥ������η�̤򼫿Ȥ������Хåե�����¸���ޤ���
 *
 * @deprecated ��侩���������Ȥξ�������������ϡ�tarray_tstring �� 
 *             regassign() �����Ѥ���������
 * @param      pat ʸ���ѥ�����(����ɽ��)
 * @return     ������������������<br>
 *             ���Ԥ�����������
 * @throw      regex�롼���󤬥����Ȥ��̤����Ƥ�����
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * @throw      �����˲��򵯤��������
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
 * @brief  ����ɽ���ˤ��ʸ����ޥå����� (��侩)
 *
 *  ���Ȥ�ʸ������Ф���POSIX ��ĥ����ɽ��(�ʲ�������ɽ��) �ˤ��ʸ����ޥå���
 *  ��Ԥ������η�̤򼫿Ȥ������Хåե�����¸���ޤ���
 *
 * @deprecated ��侩���������Ȥξ�������������ϡ�tarray_tstring �� 
 *             regassign() �����Ѥ���������
 * @param      pos ʸ����ޥå��γ��ϰ���
 * @param      pat ʸ���ѥ�����(����ɽ��)
 * @return     ������������������<br>
 *             ���Ԥ�����������
 * @throw      regex�롼���󤬥����Ȥ��̤����Ƥ�����
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * @throw      �����˲��򵯤��������
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
 * @brief  ����ɽ���ˤ��ʸ����ޥå����� (��侩)
 *
 *  ���Ȥ�ʸ������Ф���POSIX ��ĥ����ɽ��(�ʲ�������ɽ��) �ˤ��ʸ����ޥå���
 *  ��Ԥ������η�̤򼫿Ȥ������Хåե�����¸���ޤ���
 *
 * @deprecated ��侩���������Ȥξ�������������ϡ�tarray_tstring �� 
 *             regassign() �����Ѥ���������
 * @param      pat ʸ���ѥ�����(����ɽ��)
 * @return     ������������������<br>
 *             ���Ԥ�����������
 * @throw      regex�롼���󤬥����Ȥ��̤����Ƥ�����
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * @throw      �����˲��򵯤��������
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
 * @brief  ����ɽ���ˤ��ʸ����ޥå����� (��侩)
 *
 *  ���Ȥ�ʸ������Ф���POSIX ��ĥ����ɽ��(�ʲ�������ɽ��) �ˤ��ʸ����ޥå���
 *  ��Ԥ������η�̤򼫿Ȥ������Хåե�����¸���ޤ���
 *
 * @deprecated ��侩���������Ȥξ�������������ϡ�tarray_tstring �� 
 *             regassign() �����Ѥ���������
 * @param      pos ʸ����ޥå��γ��ϰ���
 * @param      pat ʸ���ѥ�����(����ɽ��)
 * @return     ������������������<br>
 *             ���Ԥ�����������
 * @throw      regex�롼���󤬥����Ȥ��̤����Ƥ�����
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * @throw      �����˲��򵯤��������
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
 * @brief  ����ɽ���ˤ��ʸ����ޥå����� (��侩)
 *
 *  ���Ȥ�ʸ������Ф���POSIX ��ĥ����ɽ��(�ʲ�������ɽ��) �ˤ��ʸ����ޥå���
 *  ��Ԥ������η�̤򼫿Ȥ������Хåե�����¸���ޤ���
 *
 * @deprecated ��侩���������Ȥξ�������������ϡ�tarray_tstring �� 
 *             regassign() �����Ѥ���������
 * @param      pat tregex ���饹�Υ���ѥ���ѥ��֥�������
 * @return     ������������������<br>
 *             ���Ԥ�����������
 * @throw      regex�롼���󤬥����Ȥ��̤����Ƥ�����
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * @throw      �����˲��򵯤��������
 *
 */
ssize_t tstring::regmatch( const tregex &pat )
{
    return this->regexp_match_advanced2((size_t)0, pat);
}

/**
 * @brief  ����ɽ���ˤ��ʸ����ޥå����� (��侩)
 *
 *  ���Ȥ�ʸ������Ф���POSIX ��ĥ����ɽ��(�ʲ�������ɽ��) �ˤ��ʸ����ޥå���
 *  ��Ԥ������η�̤򼫿Ȥ������Хåե�����¸���ޤ���
 *
 * @deprecated ��侩���������Ȥξ�������������ϡ�tarray_tstring �� 
 *             regassign() �����Ѥ���������
 * @param      pos ʸ����ޥå��γ��ϰ���
 * @param      pat tregex ���饹�Υ���ѥ���ѥ��֥�������
 * @return     ������������������<br>
 *             ���Ԥ�����������
 * @throw      regex�롼���󤬥����Ȥ��̤����Ƥ�����
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * @throw      �����˲��򵯤��������
 *
 */
ssize_t tstring::regmatch( size_t pos, const tregex &pat )
{
    ssize_t ret;
    ret = this->regexp_match_advanced2(pos, pat);
    return ret;
}


/**
 * @brief  ����ɽ���ޥå���̤����ǿ������
 *
 *  ��¸���줿����ɽ���ޥå��η�̤����ǿ����֤��ޤ�
 * 
 * @return  ��¸���줿��̤����ǿ�
 */
size_t tstring::reg_elem_length() const
{
    return this->reg_elem_length_rec;
}


/**
 * @brief  ����ɽ���ޥå���̤Υޥå����֤����
 *
 *  ��¸���줿����ɽ���ޥå���̤Ρ��ޥå�������ʬ�ΰ��֤��֤��ޤ���
 * 
 * @param   idx 0:�ޥå�����ʸ�������Τξ���<br>
 *              1�ʹ�:����ɽ�����줾��˥ޥå�������ʬʸ����ξ���.
 * 
 * @return  ��¸���줿�ޥå�������ʬ�ΰ���
 */
size_t tstring::reg_pos( size_t idx ) const
{
    if ( idx < this->reg_elem_length_rec )
	return this->_reg_pos_rec[idx];
    else 
	return 0;
}


/**
 * @brief  ����ɽ���ޥå���̤Υޥå���ʬ��ʸ����Ĺ�����
 *
 *  ��¸���줿����ɽ���ޥå���̤Ρ��ޥå�������ʬ��ʸ����Ĺ���֤��ޤ���
 * 
 * @param   idx 0:�ޥå�����ʸ�������Τξ���<br>
 *              1�ʹ�:����ɽ�����줾��˥ޥå�������ʬʸ����ξ���.
 * @return  ��¸���줿�ޥå�������ʬ��ʸ����Ĺ
 */
size_t tstring::reg_length( size_t idx ) const
{
    if ( idx < this->reg_elem_length_rec )
	return this->_reg_length_rec[idx];
    else 
	return 0;
}


/**
 * @brief  ����ɽ���ޥå���̤Υޥå���ʬ��ʸ��������
 *
 *  ��¸���줿����ɽ���ޥå���̤Ρ��ޥå�������ʬ��ʸ������֤��ޤ���
 * 
 * @param   idx 0:�ޥå�����ʸ�������Τξ���<br>
 *              1�ʹ�:����ɽ�����줾��˥ޥå�������ʬʸ����ξ���.
 * @return  ��¸���줿�ޥå�������ʬ��ʸ����
 */
const char *tstring::reg_cstr( size_t idx ) const
{
    if ( idx < this->reg_elem_length_rec )
	return this->_reg_cstr_ptr_rec[idx];
    else 
	return NULL;
}


/**
 * @brief  ����ɽ���ޥå���̤Υޥå���ʬ��ʸ����ݥ�����������
 *
 *  ��¸���줿����ɽ���ޥå���̤Ρ��ޥå�������ʬ��ʸ������Ф���
 *  �ݥ���������֤��ޤ���
 * 
 * @return  �ޥå�������ʬ���Ф���ݥ�������
 */
const char *const *tstring::reg_cstrarray() const
{
    this->cleanup_shallow_copy(true);    /* �Хåե���ľ�����뤿��Ƥ� */

    if ( 0 < this->reg_elem_length_rec )
	return this->_reg_cstr_ptr_rec;
    else 
	return NULL;
}


/**
 * @brief  ����ɽ���ˤ���ִ�
 *
 *  ���Ȥ�ʸ������Ф���pat �ǻ��ꤵ�줿POSIX ��ĥ����ɽ��(�ʲ�������ɽ��) ��
 *  �ޥå�������ʬ��ʸ���� new_str ���֤������ޤ���
 *
 * @param      pat ʸ���ѥ�����(����ɽ��)
 * @param      new_str �ִ����ʸ����
 * @param      all ���٤��ִ�������� true (��ά���� false)
 * @return     �����������ϡ��ִ����줿ʸ����μ��ΰ��֤Υ���ǥå���<br>
 *             ���Ԥ�����������
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
 * @brief  ����ɽ���ˤ���ִ�
 *
 *  ���Ȥ�ʸ������Ф���pat �ǻ��ꤵ�줿POSIX ��ĥ����ɽ��(�ʲ�������ɽ��) ��
 *  �ޥå�������ʬ��ʸ���� new_str ���֤������ޤ���
 *
 * @param      pos ʸ����ޥå��γ��ϰ���
 * @param      pat ʸ���ѥ�����(����ɽ��)
 * @param      new_str �ִ����ʸ����
 * @param      all ���٤��ִ�������� true (��ά���� false)
 * @return     �����������ϡ��ִ����줿ʸ����μ��ΰ��֤Υ���ǥå���<br>
 *             ���Ԥ�����������
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
 * @brief  ����ɽ���ˤ���ִ�
 *
 *  ���Ȥ�ʸ������Ф���pat �ǻ��ꤵ�줿POSIX ��ĥ����ɽ��(�ʲ�������ɽ��) ��
 *  �ޥå�������ʬ��ʸ���� new_str ���֤������ޤ���
 *
 * @param      pat ʸ���ѥ�����(����ɽ��)
 * @param      new_str �ִ����ʸ����
 * @param      all ���٤��ִ�������� true (��ά���� false)
 * @return     �����������ϡ��ִ����줿ʸ����μ��ΰ��֤Υ���ǥå���<br>
 *             ���Ԥ�����������
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
 * @brief  ����ɽ���ˤ���ִ�
 *
 *  ���Ȥ�ʸ������Ф���pat �ǻ��ꤵ�줿POSIX ��ĥ����ɽ��(�ʲ�������ɽ��) ��
 *  �ޥå�������ʬ��ʸ���� new_str ���֤������ޤ���
 *
 * @param      pos ʸ����ޥå��γ��ϰ���
 * @param      pat ʸ���ѥ�����(����ɽ��)
 * @param      new_str �ִ����ʸ����
 * @param      all ���٤��ִ�������� true (��ά���� false)
 * @return     �����������ϡ��ִ����줿ʸ����μ��ΰ��֤Υ���ǥå���<br>
 *             ���Ԥ�����������
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
 * @brief  ����ɽ���ˤ���ִ�
 *
 *  ���Ȥ�ʸ������Ф���pat �ǻ��ꤵ�줿POSIX ��ĥ����ɽ��(�ʲ�������ɽ��) ��
 *  �ޥå�������ʬ��ʸ���� new_str ���֤������ޤ���
 *
 * @param      pat tregex ���饹�Υ���ѥ���ѥ��֥�������
 * @param      new_str �ִ����ʸ����
 * @param      all ���٤��ִ�������� true (��ά���� false)
 * @return     �����������ϡ��ִ����줿ʸ����μ��ΰ��֤Υ���ǥå���<br>
 *             ���Ԥ�����������
 */
ssize_t tstring::regreplace( const tregex &pat, const char *new_str, bool all )
{
    ssize_t pos = 0;
    if ( new_str == NULL ) return -1;
    return this->regexp_replace(pos, pat, new_str, all);
}

/**
 * @brief  ����ɽ���ˤ���ִ�
 *
 *  ���Ȥ�ʸ������Ф���pat �ǻ��ꤵ�줿POSIX ��ĥ����ɽ��(�ʲ�������ɽ��) ��
 *  �ޥå�������ʬ��ʸ���� new_str ���֤������ޤ���
 *
 * @param      pos ʸ����ޥå��γ��ϰ���
 * @param      pat tregex ���饹�Υ���ѥ���ѥ��֥�������
 * @param      new_str �ִ����ʸ����
 * @param      all ���٤��ִ�������� true (��ά���� false)
 * @return     �����������ϡ��ִ����줿ʸ����μ��ΰ��֤Υ���ǥå���<br>
 *             ���Ԥ�����������
 */
ssize_t tstring::regreplace( size_t pos, 
			     const tregex &pat, const char *new_str, bool all )
{
    if ( new_str == NULL ) return -1;
    return this->regexp_replace(pos, pat, new_str, all);
}

/**
 * @brief  ����ɽ���ˤ���ִ�
 *
 *  ���Ȥ�ʸ������Ф���pat �ǻ��ꤵ�줿POSIX ��ĥ����ɽ��(�ʲ�������ɽ��) ��
 *  �ޥå�������ʬ��ʸ���� new_str ���֤������ޤ���
 *
 * @param      pat ʸ���ѥ�����(����ɽ��)
 * @param      new_str �ִ����ʸ����
 * @param      all ���٤��ִ�������� true (��ά���� false)
 * @return     �����������ϡ��ִ����줿ʸ����μ��ΰ��֤Υ���ǥå���<br>
 *             ���Ԥ�����������
 */
ssize_t tstring::regreplace(const char *pat, const tstring &new_str, bool all)
{
    return this->regreplace(pat, new_str.cstr(), all);
}

/**
 * @brief  ����ɽ���ˤ���ִ�
 *
 *  ���Ȥ�ʸ������Ф���pat �ǻ��ꤵ�줿POSIX ��ĥ����ɽ��(�ʲ�������ɽ��) ��
 *  �ޥå�������ʬ��ʸ���� new_str ���֤������ޤ���
 *
 * @param      pos ʸ����ޥå��γ��ϰ���
 * @param      pat ʸ���ѥ�����(����ɽ��)
 * @param      new_str �ִ����ʸ����
 * @param      all ���٤��ִ�������� true (��ά���� false)
 * @return     �����������ϡ��ִ����줿ʸ����μ��ΰ��֤Υ���ǥå���<br>
 *             ���Ԥ�����������
 */
ssize_t tstring::regreplace( size_t pos, const char *pat,
			     const tstring &new_str, bool all )
{
    return this->regreplace(pos, pat, new_str.cstr(), all);
}

/**
 * @brief  ����ɽ���ˤ���ִ�
 *
 *  ���Ȥ�ʸ������Ф���pat �ǻ��ꤵ�줿POSIX ��ĥ����ɽ��(�ʲ�������ɽ��) ��
 *  �ޥå�������ʬ��ʸ���� new_str ���֤������ޤ���
 *
 * @param      pat ʸ���ѥ�����(����ɽ��)
 * @param      new_str �ִ����ʸ����
 * @param      all ���٤��ִ�������� true (��ά���� false)
 * @return     �����������ϡ��ִ����줿ʸ����μ��ΰ��֤Υ���ǥå���<br>
 *             ���Ԥ�����������
 */
ssize_t tstring::regreplace( const tstring &pat, const tstring &new_str,
			     bool all)
{
    return this->regreplace(pat, new_str.cstr(), all);
}

/**
 * @brief  ����ɽ���ˤ���ִ�
 *
 *  ���Ȥ�ʸ������Ф���pat �ǻ��ꤵ�줿POSIX ��ĥ����ɽ��(�ʲ�������ɽ��) ��
 *  �ޥå�������ʬ��ʸ���� new_str ���֤������ޤ���
 *
 * @param      pos ʸ����ޥå��γ��ϰ���
 * @param      pat ʸ���ѥ�����(����ɽ��)
 * @param      new_str �ִ����ʸ����
 * @param      all ���٤��ִ�������� true (��ά���� false)
 * @return     �����������ϡ��ִ����줿ʸ����μ��ΰ��֤Υ���ǥå���<br>
 *             ���Ԥ�����������
 */
ssize_t tstring::regreplace( size_t pos, const tstring &pat,
			     const tstring &new_str, bool all )
{
    return this->regreplace(pos, pat, new_str.cstr(), all);
}

/**
 * @brief  ����ɽ���ˤ���ִ�
 *
 *  ���Ȥ�ʸ������Ф���pat �ǻ��ꤵ�줿POSIX ��ĥ����ɽ��(�ʲ�������ɽ��) ��
 *  �ޥå�������ʬ��ʸ���� new_str ���֤������ޤ���
 *
 * @param      pat tregex ���饹�Υ���ѥ���ѥ��֥�������
 * @param      new_str �ִ����ʸ����
 * @param      all ���٤��ִ�������� true (��ά���� false)
 * @return     �����������ϡ��ִ����줿ʸ����μ��ΰ��֤Υ���ǥå���<br>
 *             ���Ԥ�����������
 */
ssize_t tstring::regreplace( const tregex &pat, const tstring &new_str,
			     bool all )
{
    return this->regreplace(pat, new_str.cstr(), all);
}

/**
 * @brief  ����ɽ���ˤ���ִ�
 *
 *  ���Ȥ�ʸ������Ф���pat �ǻ��ꤵ�줿POSIX ��ĥ����ɽ��(�ʲ�������ɽ��) ��
 *  �ޥå�������ʬ��ʸ���� new_str ���֤������ޤ���
 *
 * @param      pos ʸ����ޥå��γ��ϰ���
 * @param      pat tregex ���饹�Υ���ѥ���ѥ��֥�������
 * @param      new_str �ִ����ʸ����
 * @param      all ���٤��ִ�������� true (��ά���� false)
 * @return     �����������ϡ��ִ����줿ʸ����μ��ΰ��֤Υ���ǥå���<br>
 *             ���Ԥ�����������
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
 * @brief  �ƥ�ݥ�ꥪ�֥������ȤΤ����shallow copy°������Ϳ
 *
 *  ���Ȥ���=�ױ黻�Ҥޤ��� init(obj) �ΰ�����Ϳ����줿���� shallow copy ���
 *  �Ĥ��������Ԥʤ��ޤ�����������ϡ��ؿ��ޤ��ϥ��дؿ��ˤ�ä��֤�����
 *  ��ݥ�ꥪ�֥������ȤˤΤ���Ϳ�����٤���ΤǤ���<br>
 *
 *  tstring ���饹�� shallow copy �β���Υ����ߥ󥰤ϡ�ʸ������Ф��뤢����
 *  �ɤ߽񤭤Ǥ��äơֽ񤭹��ߡפǤϤ���ޤ����ɤ߼�����ѤΥ��дؿ�������
 *  ���줿���⡤shallow copy ����Τ���� deep copy ������ޤ����������äơ�
 *  �ƥ�ݥ�ꥪ�֥������Ȱʳ��ξ��� set_scopy_flag() ��ȤäƤ�������̣��
 *  ����ޤ���
 *
 * @note  ����Ĺ�Хåե��⡼�ɤǤϻ����Բ�
 *
 */
/* ���: ���Ȥ��֤������Ȥ��������return ʸ�Ǥ������ȥ��󥹥��� */
/*       ��2�󥳥ԡ������Ȥ����������ʻ��ˤʤ�Τ� void �ˤʤäƤ��� */
void tstring::set_scopy_flag()
{
    if ( this->str_length_rec != UNDEF ) {	/* ����Ĺ�ξ��ϻȤ��ʤ� */
	err_throw(__FUNCTION__,"WARNING", "Cannot set flag of shallow copy");
    }
    else {
	this->shallow_copy_ok = true;
    }
    return;
}

/**
 * @brief  ���֥������Ⱦ����ɸ�२�顼���Ϥؽ���
 *
 *  ���ȤΥ��֥������Ⱦ����ɸ�२�顼���Ϥؽ��Ϥ��ޤ���
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
 * @brief  _str_rec �γ���
 *
 * @note   ���Υ��дؿ���private�Ǥ� 
 */
int tstring::realloc_str_rec( size_t len_bytes )
{
    this->cleanup_shallow_copy(true);	/* �Хåե���ľ�����뤿��Ƥ� */

    size_t len_alloc;

    if ( this->_str_rec == NULL && len_bytes == 0 ) return 0;

    /* ����Ĺ�Хåե��ξ��� 1GB ̤���λ� */
    if ( this->str_length_rec == UNDEF && 
	 0 < len_bytes && len_bytes < (size_t)1024*1024*1024 ) {
	if ( len_bytes <= 32 ) {		/* �׵᤬��������� */
	    len_alloc = 32;
	}
	else {					/* 2^n �ǤȤ�褦�˼��� */
	    const double base = 2.0;
	    size_t nn = (size_t)ceil( log((double)len_bytes) / log(base) );
	    size_t len = (size_t)pow(base, (double)nn);
	    /* ǰ�Τ�������å� */
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
 * @brief  _str_rec �γ���
 *
 * @note   ���Υ��дؿ���private�Ǥ� 
 */
void tstring::free_str_rec()
{
    this->cleanup_shallow_copy(true);	/* �Хåե���ľ�����뤿��Ƥ� */

    if ( this->_str_rec != NULL ) {
	free(this->_str_rec);
	this->_str_rec = NULL;
	this->str_alloc_blen_rec = 0;
    }

    return;
}

/* for reg_pos_rec, etc. */

/**
 * @brief  _reg_pos_rec �γ���
 *
 * @note   ���Υ��дؿ���private�Ǥ� 
 */
int tstring::realloc_reg_pos_rec( size_t len_elements )
{
    this->cleanup_shallow_copy(true);	/* �Хåե���ľ�����뤿��Ƥ� */

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
 * @brief  _reg_pos_rec �γ���
 *
 * @note   ���Υ��дؿ���private�Ǥ� 
 */
void tstring::free_reg_pos_rec()
{
    this->cleanup_shallow_copy(true);	/* �Хåե���ľ�����뤿��Ƥ� */

    if ( this->_reg_pos_rec != NULL ) {
	free(this->_reg_pos_rec);
	this->_reg_pos_rec = NULL;
    }

    return;
}

/**
 * @brief  _reg_length_rec �γ���
 *
 * @note   ���Υ��дؿ���private�Ǥ� 
 */
int tstring::realloc_reg_length_rec( size_t len_elements )
{
    this->cleanup_shallow_copy(true);	/* �Хåե���ľ�����뤿��Ƥ� */

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
 * @brief  _reg_length_rec �γ���
 *
 * @note   ���Υ��дؿ���private�Ǥ� 
 */
void tstring::free_reg_length_rec()
{
    this->cleanup_shallow_copy(true);	/* �Хåե���ľ�����뤿��Ƥ� */

    if ( this->_reg_length_rec != NULL ) {
	free(this->_reg_length_rec);
	this->_reg_length_rec = NULL;
    }

    return;
}

/**
 * @brief  _reg_cstr_ptr_rec �γ���
 *
 * @note   ���Υ��дؿ���private�Ǥ� 
 */
int tstring::realloc_reg_cstr_ptr_rec( size_t len_elements )
{
    this->cleanup_shallow_copy(true);	/* �Хåե���ľ�����뤿��Ƥ� */

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
 * @brief  _reg_cstr_ptr_rec �γ���
 *
 * @note   ���Υ��дؿ���private�Ǥ� 
 */
void tstring::free_reg_cstr_ptr_rec()
{
    this->cleanup_shallow_copy(true);	/* �Хåե���ľ�����뤿��Ƥ� */

    if ( this->_reg_cstr_ptr_rec != NULL ) {
	free(this->_reg_cstr_ptr_rec);
	this->_reg_cstr_ptr_rec = NULL;
    }

    return;
}

/**
 * @brief  _reg_cstrbuf_rec �γ���
 *
 * @note   ���Υ��дؿ���private�Ǥ� 
 */
int tstring::realloc_reg_cstrbuf_rec( size_t len_bytes )
{
    this->cleanup_shallow_copy(true);	/* �Хåե���ľ�����뤿��Ƥ� */

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
 * @brief  _reg_cstrbuf_rec �γ���
 *
 * @note   ���Υ��дؿ���private�Ǥ� 
 */
void tstring::free_reg_cstrbuf_rec()
{
    this->cleanup_shallow_copy(true);	/* �Хåե���ľ�����뤿��Ƥ� */

    if ( this->_reg_cstrbuf_rec != NULL ) {
	free(this->_reg_cstrbuf_rec);
	this->_reg_cstrbuf_rec = NULL;
    }

    return;
}

/**
 * @brief  ���ɥ쥹�����ȤΥǡ����ΰ��⤫Ĵ�٤�
 *
 *  ptr�Υ��ɥ쥹�����ȤΥǡ����ΰ��⤫������å����ޤ���
 * 
 * @param   ptr �����å����륢�ɥ쥹
 * @return  ���Ȥ��ΰ���λ��Ͽ�<br>
 *          ����ʳ��λ��ϵ�
 * @note    ���Υ��дؿ���private�Ǥ� 
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
 * @brief  ����ɽ���ˤ��ޥå�����(private)
 *
 *  ���Ȥ�ʸ������Ф���POSIX ��ĥ����ɽ���ˤ��ʸ����ޥå����Ԥ���
 *  ���η�̤��֤��ޤ���
 *
 * @param   pos ʸ����ޥå��γ��ϰ���
 * @param   regex_ref tregex ���饹�Υ���ѥ���ѥ��֥�������
 * @param   ret_span �ޥå�����ʸ�����Ĺ��
 * @return  �����������ϥޥå��������Ȥ�ʸ����ΰ���<br>
 *          ���Ԥ�����������
 * @throw   regex�롼���󤬥����Ȥ��̤����Ƥ�����
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 * @throw   �����˲��򵯤��������
 * @note    ���Υ��дؿ���private�Ǥ� 
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
 * @brief  ����ɽ���ˤ��ޥå�����(private)
 *
 *  ���Ȥ�ʸ������Ф���POSIX ��ĥ����ɽ���ˤ��ʸ����ޥå����Ԥ���
 *  �������Ȥξ����ޤ��̤���������Ǥ��ޤ���
 * 
 * @param   pos ʸ����ޥå��γ��ϰ���
 * @param   regex_ref pat tregex ���饹�Υ���ѥ���ѥ��֥�������
 * @param   nelem pos_r��len_r �ΥХåե�Ĺ
 * @param   pos_r[] pos_r �ޥå�����ʸ����ΰ���
 * @param   len_r[] len_r �ޥå�����ʸ�����Ĺ��
 * @param   nelem_r pos_r��len_r �˥��åȤ��줿�ͤθĿ�
 * @return  �����������ϥޥå��������Ȥ�ʸ����ΰ���<br>
 *          ���Ԥ�����������
 * @throw   regex�롼���󤬥����Ȥ��̤����Ƥ�����
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 * @throw   �����˲��򵯤��������
 * @note    ���Υ��дؿ���private�Ǥ� 
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
 * @brief  ����ɽ���ˤ��ޥå�����(private)
 *
 *  ���Ȥ�ʸ������Ф���POSIX ��ĥ����ɽ���ˤ��ʸ����ޥå����Ԥ���
 *  ���η�̤򼫿Ȥ������Хåե�����¸���ޤ���
 * 
 * @param   pos ʸ����ޥå��γ��ϰ���.
 * @param   regex_ref tregex ���饹�Υ���ѥ���ѥ��֥�������
 * @return  �����������ϥޥå��������Ȥ�ʸ����ΰ���<br>
 *          ���Ԥ�����������
 * @throw   regex�롼���󤬥����Ȥ��̤����Ƥ�����
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 * @throw   �����˲��򵯤��������
 * @note    ���Υ��дؿ���private�Ǥ� 
 */
ssize_t tstring::regexp_match_advanced2( size_t pos, const tregex &regex_ref )
{
    int status;
    size_t to_append, new_n, n_result, off, i;
    const char *str_ptr;

    this->cleanup_shallow_copy(true);    /* �Хåե���ľ�����뤿��Ƥ� */

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
    /* '(' �θĿ��������������ɲ�ʬ�ˤ��롥'\\(' �⥫����Ȥ���뤬OK */
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
 * @brief  ����ɽ���η�̤γ�Ǽ�������(private)
 *
 * @note   ���Υ��дؿ���private�Ǥ� 
 */
void tstring::init_reg_results()
{
    this->cleanup_shallow_copy(true);	/* �Хåե���ľ�����뤿��Ƥ� */

    this->reg_elem_length_rec = 0;
    this->free_reg_pos_rec();
    this->free_reg_length_rec();
    this->free_reg_cstr_ptr_rec();
    this->reg_cstrbuf_length_rec = 0;
    this->free_reg_cstrbuf_rec();

    return;
}

/**
 * @brief  ����ɽ���ˤ���ִ�(private)
 *
 *  ���Ȥ�ʸ������Ф���pat �ǻ��ꤵ�줿POSIX ��ĥ����ɽ����
 *  �ޥå�������ʬ��ʸ���� new_str ���֤������ޤ���
 * 
 * @param   pos ʸ����ޥå��γ��ϰ���
 * @param   pat tregex ���饹�Υ���ѥ���ѥ��֥�������
 * @param   new_str �ִ����ʸ����
 * @param   all ���٤��ִ�������� true (��ά���� false)
 * @return  �����������ϡ��ִ����줿ʸ����μ��ΰ��֤Υ���ǥå���<br>
 *          ���Ԥ�����������
 * @throw   regex�롼���󤬥����Ȥ��̤����Ƥ�����
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 * @throw   �����˲��򵯤��������
 * @note    ���Υ��дؿ���private�Ǥ� 
 */
ssize_t tstring::regexp_replace( size_t pos, const tregex &pat,
				 const char *new_str, bool all )
{
    ssize_t pret = -1;
    bool has_reference = false;
    size_t i;
    if ( new_str == NULL ) return -1;

#if 1
    /* "\\\\" �ˤ��б����ʤ��Ȥ����ʤ��Τǡ�����Ͼ�� true */
    has_reference = true;
#else
    /* �ޤ���new_str ���������Ȥ�ޤ�Ǥ��뤫Ĵ�٤� */
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

    /* �Хå�����å��� + [1-9] �ˤ��������Ȥ������� */
    if ( has_reference == true ) {
	while ( 0 <= this->regexp_match_advanced2(pos, pat) ) {
	    /* ������󤷤ʤ��� new_str ���Ȥ˿������Τ����  */
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
    /* �������Ȥ�̵�����(�����ϻȤ��ʤ�) */
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
 * @brief  ����ѥ�����˥��顼����Ϥ����뤿��Υ��ߡ��Υ��дؿ�
 * @deprecated  �����Բġ�
 *  
 */
void tstring::assign( int ch )
{
    sli__eprintf("[FATAL ERROR] Do not use tstring::assign( int ch ) !!\n");
    abort();
    return;
}

/**
 * @brief  ����ѥ�����˥��顼����Ϥ����뤿��Υ��ߡ��Υ��дؿ�
 * @deprecated  �����Բġ�
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
 * @brief  ����ѥ�����˥��顼����Ϥ����뤿��Υ��ߡ��Υ��дؿ�
 * @deprecated  �����Բġ�
 *  
 */
void tstring::append( int ch )
{
    sli__eprintf("[FATAL ERROR] Do not use tstring::append( int ch ) !!\n");
    abort();
    return;
}

/**
 * @brief  ����ѥ�����˥��顼����Ϥ����뤿��Υ��ߡ��Υ��дؿ�
 * @deprecated  �����Բġ�
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
 * @brief  ����ѥ�����˥��顼����Ϥ����뤿��Υ��ߡ��Υ��дؿ�
 * @deprecated  �����Բġ�
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
