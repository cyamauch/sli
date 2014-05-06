/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2013-02-03 00:56:43 cyamauch> */

#define CLASS_NAME "xmlparser"

#include "config.h"

#include "xmlparser.h"

#include <stdlib.h>

#include "private/err_report.h"

#include "private/c_fprintf.h"
#include "private/c_stderr.h"

#include "private/c_strcpy.h"
#include "private/c_strcat.h"
#include "private/c_strlen.h"
#include "private/c_strchr.h"
#include "private/c_strcmp.h"
#include "private/c_strncmp.h"
#include "private/c_strdup.h"
#include "private/c_memcpy.h"
#include "private/c_memchr.h"

#include "private/fillup_inconvenient_chars.h"

static void *my_memrchr(const void *s, int c, size_t n)
{
    const char *in_s = (const char *)s;
    while ( 0 < n ) {
        n--;
        if ( in_s[n] == c ) return (void *)(in_s + n);
    }
    return NULL;
}

/*
 * <，> とか "…" の parse に使う文字列を用意する．つまり．parse に都合の悪い
 * スペースやバックスラシュによるエスケープシーケンスなどを，_ で塗り潰す．
 * 例としては，
 * <foo name="bar \"hoge\" <xyz>">  のようなものを
 * <foo name="bar___hoge____xyz_">  のように書き換えたものを返す
 */
static int xml_fillup_inconvenient_chars( const char *target_string, 
					  ssize_t len_targetstring,
					  char **return_filled )
{
    return fillup_inconvenient_chars( target_string, len_targetstring,
				      "<> \t\n","\"",
				      '\\','_',
				      return_filled);
}

namespace sli
{

/* constructor */
xmlparser::xmlparser()
{
    this->leveltable_currentlevel = -1;
    this->leveltable_allocnum = 0;
    this->leveltable = NULL;
    this->value_leveltable = NULL;

    this->stream = NULL;
    this->all_xml_ptr = NULL;
    this->left_xml_size = -1;
    this->tmp_xml_buffer = NULL;
    this->num_block = 1;
    this->next_tmp_xml_pos = 0;

    this->xmlline_ptr = NULL;
    this->xmlline_size = 0;
    this->xmlline_current_pos = 0;
    this->xmlline_next_pos = 0;
    this->filled_xmlline_buffer = NULL;
    this->filled_xmlline_is_provided = false;

    return;
}

/* destructor */
xmlparser::~xmlparser()
{
    if ( this->leveltable != NULL ) {
	int i;
	for ( i=0 ; i < leveltable_allocnum ; i++ ) {
	    if ( this->leveltable[i] != NULL ) {
		delete this->leveltable[i];
	    }
	}
	free(this->leveltable);
    }
    if ( this->value_leveltable != NULL ) {
	int i;
	for ( i=0 ; i < leveltable_allocnum ; i++ ) {
	    if ( this->value_leveltable[i] != NULL ) {
		int j;
		for ( j=0 ; this->value_leveltable[i][j].name != NULL ; j++ ) {
		    free(this->value_leveltable[i][j].name);
		    free(this->value_leveltable[i][j].value);
		}
		free(this->value_leveltable[i]);
	    }
	}
	free(this->value_leveltable);
    }
    if ( this->tmp_xml_buffer != NULL ) 
	free(this->tmp_xml_buffer);
    if ( this->filled_xmlline_buffer != NULL ) 
	free(this->filled_xmlline_buffer);

    return;
}

/* copy constructor (dummy) */
xmlparser::xmlparser(const xmlparser &obj)
{
    c_fprintf(c_stderr(),
	    "[FATAL ERROR] Do not pass by value for xmlparser classes!!\n");
    abort();
    return;
}

/* overloading operator = (dummy) */
xmlparser &xmlparser::operator=(const xmlparser &obj)
{
    c_fprintf(c_stderr(),
	    "[FATAL ERROR] Do not use '=' operator for xmlparser classes!!\n");
    abort();
    return *this;
}

/* testing XML's hierarchy */
/* 
   example:
   status = this->test_hierarchy(6,"ums:grammar","ums:start","rng:element",
                                 "ums:zeroOrMore","rng:element","tlm:packet");
   if ( status == 0 ) puts("match!");
*/
#define FUNC_NAME "test_hierarchy"
int xmlparser::test_hierarchy( int num_arg, ... ) const
{
    int return_status = -1;
    char **elems = NULL;

    if ( 0 < num_arg ) {
	if ( num_arg <= this->leveltable_currentlevel + 1 ) {
	    int i;
	    elems = (char **)malloc(sizeof(*elems)*num_arg);
	    if ( elems == NULL ) {
		err_throw(FUNC_NAME,"FATAL","malloc() failed");
	    }
	    va_list ap;
	    va_start(ap, num_arg);
	    for ( i=0 ; i < num_arg ; i++ ) {
		elems[i] = va_arg(ap,char *);
	    }
	    va_end(ap);
	    /* begin test... */
	    for ( i=0 ; i < num_arg ; i++ ) {
		if ( elems[num_arg-1 - i] != NULL ) {
		    const char *const *el =
			this->leveltable[this->leveltable_currentlevel - i]->cstrarray();
		    if ( el == NULL ) break;
		    if ( el[0] == NULL ) break;
		    if ( c_strcmp(elems[num_arg-1 - i],el[0]) != 0 ) {
			break;
		    }
		}
	    }
	    if ( i < num_arg ) return_status = 1;
	    else return_status = 0;	/* match! */
	}
	else return_status = 1;
    }
    else if ( num_arg == 0 ) {
	return_status = 0;
    }

    //quit:
    if ( elems != NULL ) free(elems);
    return return_status;
}
#undef FUNC_NAME

const char *xmlparser::get_element( int level, int index ) const
{
    const char *return_ptr = NULL;

    if ( 0 <= level && level <= this->leveltable_currentlevel ) {
	int i;
	const char *const *el =
	    this->leveltable[level]->cstrarray();
	if ( el != NULL ) {
	    for ( i=0 ; el[i] != NULL ; i++ ) {
		if ( i == index ) {
		    return_ptr = el[i];
		    break;
		}
	    }
	}
    }

    return return_ptr;
}

#define FUNC_NAME "get_value"
const char *xmlparser::get_value( int level, const char *name ) const
{
    int i;
    bool flag_quote;
    size_t len;
    const char *ptr0;
    const char *const *table;
    xmlparser__value_leveltable *vtable;
    const char *return_ptr = NULL;
    int num_vtable = 0;

    if ( level < 0 || this->leveltable_currentlevel < level ) {
	goto quit;
    }

    table = this->leveltable[level]->cstrarray();
    if ( table == NULL ) goto quit;

    vtable = this->value_leveltable[level];
    /* キャッシュされているか調べる */
    if ( vtable != NULL ) {
	for ( i=0 ; vtable[i].name != NULL ; i++ ) {
	    if ( c_strcmp(vtable[i].name,name) == 0 ) {
		return_ptr = vtable[i].value;
		goto quit;
	    }
	    num_vtable++;
	}
    }

    /* 実際のテーブルから調べる */
    ptr0=NULL;
    flag_quote=false;
    len = c_strlen(name);
    for ( i=0 ; table[i] != NULL ; i++ ) {
	if ( c_strncmp(table[i],name,len) == 0 ) {
	    if ( table[i][len] == '=' ) {
		if ( table[i][len+1] == '"' ) {
		    flag_quote=true;
		    ptr0 = table[i] + len + 1 + 1;
		}
		else {
		    ptr0 = table[i] + len + 1;
		}
		break;
	    }
	}
    }

    if ( ptr0 != NULL ) {
	char *val_ptr;
	/* キャッシュを確保 */
	if ( vtable == NULL ) {
	    num_vtable = 1;
	    vtable = (xmlparser__value_leveltable *)malloc(sizeof(*vtable)*(num_vtable+1));
	    if ( vtable == NULL ) {
		err_throw(FUNC_NAME,"FATAL","malloc() failed");
	    }
	}
	else {
	    num_vtable++;
	    vtable = (xmlparser__value_leveltable *)realloc(vtable,
							    sizeof(*vtable)*(num_vtable+1));
	    if ( vtable == NULL ) {
		err_throw(FUNC_NAME,"FATAL","realloc() failed");
	    }
	}
	this->value_leveltable[level] = vtable;
	vtable[num_vtable-1].name = c_strdup(name);
	vtable[num_vtable-1].value = NULL;
	vtable[num_vtable].name = NULL;
	vtable[num_vtable].value = NULL;

	if ( vtable[num_vtable-1].name == NULL ) {
	    err_throw(FUNC_NAME,"FATAL","strdup() failed");
	}

	/* */
	len = c_strlen(ptr0);
	val_ptr=(char *)malloc(len+1);
	if ( val_ptr == NULL ) {
	    err_throw(FUNC_NAME,"FATAL","malloc() failed");
	}
	if ( 0 < len ) {
	    size_t j;
	    if ( flag_quote == true && ptr0[len-1] == '"' ) {
		len--;
	    }
	    /* バックスラッシュ + なんとか の場合は \ は取り去る */
	    for ( j=0,i=0 ; j < len ; i++,j++ ) {
		if ( j+1 < len && ptr0[j] == '\\' ) j++;
		val_ptr[i] = ptr0[j];
	    }
	    val_ptr[i] = '\0';
	}
	else {
	    *val_ptr = '\0';
	}

	vtable[num_vtable-1].value = val_ptr;
	return_ptr = val_ptr;
    }

 quit:
    return return_ptr;
}
#undef FUNC_NAME

#define READ_BLOCK_SIZE 512

#define FUNC_NAME "line_reader"
const char *xmlparser::line_reader( ssize_t *size )
{
    const char *return_ptr = NULL;

    if ( this->stream != NULL || this->all_xml_ptr != NULL ) {
	ssize_t l;
	size_t pos;
	if ( this->left_xml_size == 0 ) {
	    *size = 0;
	    goto quit;	/* EOF */
	}
	if ( this->tmp_xml_buffer == NULL ) {
	    this->tmp_xml_buffer =
		(char *)malloc(this->num_block*READ_BLOCK_SIZE+1);
	    if ( this->tmp_xml_buffer == NULL ) {
		*size = -1;
		err_throw(FUNC_NAME,"FATAL","malloc() failed");
	    }
	}
	pos = 0;
	if ( 0 < this->next_tmp_xml_pos ) {
	    size_t i;
	    for ( i=this->next_tmp_xml_pos ; 
		  this->tmp_xml_buffer[i] != '\0';
		  i++,pos++ ) {
		this->tmp_xml_buffer[pos] = this->tmp_xml_buffer[i];
	    }
	}
	do {
	    int status;
	    char *p0;
	    size_t size_to_read = this->num_block*READ_BLOCK_SIZE - pos;
	    if ( 0 <= this->left_xml_size ) {
		if ( this->left_xml_size < (ssize_t)size_to_read )
		    size_to_read = this->left_xml_size;
	    }
	    if ( this->stream != NULL ) {
		l = this->stream->read(this->tmp_xml_buffer + pos, 
				       size_to_read);
		if ( l < 0 ) {
		    *size = -1;
		    err_report(FUNC_NAME,"ERROR","stream->read() failed");
		    goto quit;	/* ERROR */
		}
	    }
	    else {
		for ( l=0 ; l < (ssize_t)size_to_read ; l++ ) {
		    if ( all_xml_ptr[0] == '\0' ) break;
		    this->tmp_xml_buffer[pos+l] = all_xml_ptr[0];
		    all_xml_ptr++;
		}
	    }
	    this->tmp_xml_buffer[pos+l] = '\0';
	    if ( 0 <= this->left_xml_size ) {
		this->left_xml_size -= l;
	    }
	    if ( l+pos == 0 ) {
		*size = 0;
		break;	/* EOF */
	    }
	    status =
		xml_fillup_inconvenient_chars(this->tmp_xml_buffer, l+pos,
					      &(this->filled_xmlline_buffer));
	    if ( status != 0 ) {
		*size = -1;
		err_throw(FUNC_NAME,"FATAL",
			  "xml_fillup_inconvenient_chars() failed");
	    }
	    this->filled_xmlline_is_provided = true;
	    /* EOF になった場合 */
	    if ( l == 0 || this->left_xml_size == 0 ) {
		this->next_tmp_xml_pos = l+pos;
		*size = this->next_tmp_xml_pos;
		return_ptr = this->tmp_xml_buffer;
		break;	/* EOF */
	    }
	    /* XML のタグの '>' を探す */
	    p0 = (char *)my_memrchr(this->filled_xmlline_buffer,'>',l+pos);
	    if ( p0 == NULL ) {
		char *tmp_ptr;
		this->num_block++;
		tmp_ptr =
		    (char *)realloc(this->tmp_xml_buffer,
				    this->num_block*READ_BLOCK_SIZE+1);
		if ( tmp_ptr == NULL ) {
		    this->num_block--;
		    *size = -1;
		    err_throw(FUNC_NAME,"FATAL","realloc() failed");
		}
		this->tmp_xml_buffer = tmp_ptr;
		pos += l;
	    }
	    else {
		this->next_tmp_xml_pos = 
		    (p0 + 1) - this->filled_xmlline_buffer;
		*size = this->next_tmp_xml_pos;
		return_ptr = this->tmp_xml_buffer;
		break;	/* go */
	    }
	} while ( 1 );
    }
    else {
	*size = -1;	/* ERROR */
    }

 quit:
    return return_ptr;
}
#undef FUNC_NAME

int xmlparser::main( cstreamio &stream )
{
    int return_status;

    this->stream = &stream;
    this->left_xml_size = -1;
    return_status = this->main();
    if ( this->tmp_xml_buffer != NULL ) {
	free(this->tmp_xml_buffer);
	this->tmp_xml_buffer = NULL;
    }
    this->next_tmp_xml_pos = 0;
    this->num_block = 1;
    this->left_xml_size = -1;
    this->stream = NULL;
	
    return return_status;
}

int xmlparser::main( cstreamio &stream, size_t xml_size )
{
    int return_status;

    this->stream = &stream;
    this->left_xml_size = xml_size;
    return_status = this->main();
    if ( this->tmp_xml_buffer != NULL ) {
	free(this->tmp_xml_buffer);
	this->tmp_xml_buffer = NULL;
    }
    this->next_tmp_xml_pos = 0;
    this->num_block = 1;
    this->left_xml_size = -1;
    this->stream = NULL;
	
    return return_status;
}

int xmlparser::main( const char *all_xml )
{
    int return_status;

    this->all_xml_ptr = all_xml;
    this->left_xml_size = -1;
    return_status = this->main();
    if ( this->tmp_xml_buffer != NULL ) {
	free(this->tmp_xml_buffer);
	this->tmp_xml_buffer = NULL;
    }
    this->next_tmp_xml_pos = 0;
    this->num_block = 1;
    this->left_xml_size = -1;
    this->all_xml_ptr = NULL;
	
    return return_status;
}

int xmlparser::main( const char *all_xml, size_t xml_size )
{
    int return_status;

    this->all_xml_ptr = all_xml;
    this->left_xml_size = xml_size;
    return_status = this->main();
    if ( this->tmp_xml_buffer != NULL ) {
	free(this->tmp_xml_buffer);
	this->tmp_xml_buffer = NULL;
    }
    this->next_tmp_xml_pos = 0;
    this->num_block = 1;
    this->left_xml_size = -1;
    this->all_xml_ptr = NULL;
	
    return return_status;
}

/* parser本体は長いXMLをparseするのは得意でないので，ここで細かく分割する */
/* return value: size to read from this->xmlline_current_pos */
/* 0: EOF  -1: ERROR */
#define FUNC_NAME "split_line"
ssize_t xmlparser::split_line()
{
    ssize_t return_size = -1;

    if ( this->xmlline_ptr != NULL ) {
	if ( this->xmlline_size <= this->xmlline_next_pos ) {
	    this->xmlline_ptr = NULL;
	    this->xmlline_size = 0;
	    this->xmlline_current_pos = 0;
	    this->xmlline_next_pos = 0;
	}
    }

    if ( this->xmlline_ptr == NULL ) {
	int status;
	ssize_t xml_size = -1;
	this->filled_xmlline_is_provided = false;
	this->xmlline_ptr = this->line_reader(&xml_size);
	if ( this->xmlline_ptr == NULL ) {	/* END of XML */
	    if ( xml_size == 0 ) return_size = 0;
	    goto quit;	/* return 0 or -1 */
	}
	this->xmlline_size = xml_size;
	if ( this->filled_xmlline_is_provided == false ) {
	    /* ユーザの line_reader() の場合 */
	    status =
		xml_fillup_inconvenient_chars(this->xmlline_ptr, xml_size,
					      &(this->filled_xmlline_buffer));
	    if ( status != 0 ) {
		err_throw(FUNC_NAME,"FATAL",
			  "xml_fillup_inconvenient_chars() failed");
	    }
	}
	else {
	    /* go all */
	    return_size = this->xmlline_size;
	    this->xmlline_next_pos += return_size;
	    goto quit;
	}
    }

    if ( this->xmlline_ptr != NULL ) {
	const char *cl;	/* position of '>' */
	const char *current_ptr;
	const char *current_f_ptr;
	this->xmlline_current_pos = this->xmlline_next_pos;
	current_ptr = this->xmlline_ptr + this->xmlline_current_pos;
	current_f_ptr =this->filled_xmlline_buffer + this->xmlline_current_pos;
	cl = c_strchr(current_f_ptr,'>');
	if ( cl == NULL ) {
	    /* go all */
	    return_size = this->xmlline_size - this->xmlline_current_pos;
	    this->xmlline_next_pos += return_size;
	}
	else {
	    return_size = cl - current_f_ptr + 1;
	    this->xmlline_next_pos += return_size;
	}
    }

 quit:
    return return_size;
}
#undef FUNC_NAME

/*
 * XML パーサの main ループ
 *
 *   XML ファイルを parse して，XML の要素( < から > までか，値のみ) ごとに
 *   ハンドラを呼び出します．
 *   上位階層の XML 要素は配列 this->leveltable に保存されており，ハンドラ
 *   this->element_handler() から参照可能ですが，ほどんどの場合，
 *   this->test_hierarchy(), this->get_element(), this->get_value() を使う
 *   だけで済むはず．
 *
 */
#define FUNC_NAME "main"
int xmlparser::main()
{
    int result_return=-1;
    bool error_throw = false;

    /* ここは malloc で確保するので，出るときは要開放 */
    char *prev_line=NULL;
    char *copied_string=NULL;
    register char *target_string=NULL;
    register char *next_line=NULL;
    /* target_string,next_line のバッファのサイズ */
    size_t target_string_alloc_size=0;
    size_t next_line_alloc_size=0;
    /* 文字の長さ */
    size_t len_target_string=0;
    size_t len_next_line=0;

    /* ただのポインタ */
    const char *line_buffer_ptr;
    const char *filled_line_buffer_ptr;
    ssize_t len_line_buffer;
    const char *filled_target_ptr=NULL;
    const char *filled_next_ptr=NULL;

    try {
      /* 行ごとのループ */
      while ( 0 < (len_line_buffer=this->split_line()) ) {

	size_t span;
	char *ptr2;
	size_t len_line_buffer_span;
	void *tmp_ptr;

	line_buffer_ptr = this->xmlline_ptr + this->xmlline_current_pos;
	filled_line_buffer_ptr = 
	    this->filled_xmlline_buffer + this->xmlline_current_pos;

	/* 前と後のタブ・スペースは除去する */
	for ( span=0 ; (ssize_t)span < len_line_buffer ; span++ ) {
	    if ( line_buffer_ptr[span] != ' ' &&
		 line_buffer_ptr[span] != '\t' &&
		 line_buffer_ptr[span] != '\n' ) break;
	}
	/* */
	len_line_buffer_span = len_line_buffer - span;
	if ( target_string_alloc_size < len_line_buffer_span+1 ) {
	    tmp_ptr = realloc(target_string,len_line_buffer_span+1);
	    if ( tmp_ptr == NULL ) {
		err_throw(FUNC_NAME,"FATAL","realloc() failed");
	    }
	    target_string = (char *)tmp_ptr;
	    target_string_alloc_size = len_line_buffer_span+1;
	}
	c_memcpy(target_string,line_buffer_ptr+span,len_line_buffer_span);
	target_string[len_line_buffer_span] = '\0';
	for ( ptr2=target_string+len_line_buffer_span-1 ;
	      target_string <= ptr2 ; ptr2-- ) {
	    if ( *ptr2 != ' ' && *ptr2 != '\t' && *ptr2 != '\n' ) break;
	}
	/* 全部スペースかタブだった */
	if ( target_string > ptr2 ) continue;
	ptr2[1] = '\0';

	len_target_string = (ptr2 + 1) - target_string;
	filled_target_ptr = filled_line_buffer_ptr + span;

	/* 
	 * 行の途中で > や < が見つかった場合のループ
	 * <foo>bar</foo> となっていた場合に以下に突入すると
	 * 1回目に $next_line に bar</foo> が入り，
	 * 2回目に $next_line に </foo> が入る．
	 */
	do {
	    bool flag_found;
	    char *line;
	    const char *string_to_parse;
	    size_t len_string_to_parse;
	    const char *ptr0;
	    const char *ptr1;
	    int result;

	    /* $_ は target_string とする */
	    if ( next_line != NULL && next_line[0] != '\0' ) {
		char *tmp_cptr = target_string;
		size_t tmp_size = target_string_alloc_size;
		/* */
		target_string = next_line;
		target_string_alloc_size = next_line_alloc_size;
		len_target_string = len_next_line;
		filled_target_ptr = filled_next_ptr;
		/* */
		next_line = tmp_cptr;
		next_line_alloc_size = tmp_size;
		next_line[0] = '\0';
		len_next_line = 0;
		filled_next_ptr = NULL;
	    }
	    /* parse するのは，都合の悪いスペースやバックスラシュによる
	       エスケープシーケンスなどを，_ で塗り潰したもの．*/
	    string_to_parse = filled_target_ptr;
	    len_string_to_parse = len_target_string;
		
	    /*
#if DEBUG
	    printf("[target_string]:[%s]\n",target_string);
#endif
	    */

	    /*
	     * 以下で，next_line を準備する
	     */

	    /* 途中で > が見つかった場合 */
	    flag_found=false;
	    ptr0=(char *)c_memchr(string_to_parse,'>',len_string_to_parse);
	    if ( ptr0 != NULL ) {
		ptr1=(char *)c_memchr(string_to_parse+1,'<',len_string_to_parse-1);
		if ( ptr1==NULL || ptr0 < ptr1 ) {
		    size_t span;
		    ptr0++;
		    for ( span=0 ; 
			  ptr0 + span < string_to_parse + len_string_to_parse ;
			  span++ ) {
			if ( ptr0[span] != ' ' &&
			     ptr0[span] != '\t' &&
			     ptr0[span] != '\n' ) break;
		    }
		    ptr1 = ptr0 + span;
		    /* */
		    if ( ptr1 < string_to_parse + len_string_to_parse ) {
			/* ptr1 は有効な文字列で始まっている */
			if ( next_line_alloc_size < target_string_alloc_size ) {
			    tmp_ptr = 
				realloc(next_line,target_string_alloc_size);
			    if ( tmp_ptr == NULL ) {
				err_throw(FUNC_NAME,
					  "FATAL","realloc() failed");
			    }
			    next_line = (char *)tmp_ptr;
			    next_line_alloc_size = target_string_alloc_size;
			}
			c_strcpy(next_line, 
				 target_string + (ptr1-string_to_parse) );
			filled_next_ptr =
			    filled_target_ptr + (ptr1-string_to_parse);
			len_next_line = 
			    len_target_string - (ptr1-string_to_parse);
			len_target_string = ptr0-string_to_parse;
			target_string[len_target_string]='\0';
			flag_found=true;
		    }
		}
	    }
	    /* 途中で < が見つかるかテスト */
	    if ( flag_found == false ) {
		ptr0=(char *)c_memchr(string_to_parse+1,'<',len_string_to_parse-1);
		if ( ptr0 != NULL ) {
		    for ( ptr1=ptr0-1 ; string_to_parse <= ptr1 ; ptr1-- ) {
			if ( *ptr1 != ' ' && *ptr1 != '\t' && *ptr1 != '\n' )
			    break;
		    }
		    if ( string_to_parse <= ptr1 ) {
			if ( next_line_alloc_size < target_string_alloc_size ) {
			    tmp_ptr = 
				realloc(next_line,target_string_alloc_size);
			    if ( tmp_ptr == NULL ) {
				err_throw(FUNC_NAME,
					  "FATAL","realloc() failed");
			    }
			    next_line = (char *)tmp_ptr;
			    next_line_alloc_size = target_string_alloc_size;
			}
			c_strcpy(next_line,
				 target_string + (ptr0-string_to_parse) );
			filled_next_ptr = 
			    filled_target_ptr + (ptr0-string_to_parse);
			len_next_line = 
			    len_target_string - (ptr0-string_to_parse);
			len_target_string = (ptr1-string_to_parse)+1;
			target_string[len_target_string]='\0';
			flag_found=true;
		    }
		}
	    }

	    /*
#if DEBUG
	    printf("[target_string]:[%s] [prev_line]:[%s] [next_line]:[%s]\n",
		   target_string,prev_line,next_line);
#endif
	    */		
	    /*
	     * 以下で，1つの <…> あるいは値について処理する
	     */

	    /* 前から続いていなくて < で始まっていない場合 */
	    if ( prev_line == NULL && target_string[0] != '<' ) {
		line=target_string;
		if ( 0 <= this->leveltable_currentlevel ) {
#if DEBUG
		    printf("[VALUE]:[%s]\n",line);
#endif
		    result = this->element_handler(Xmlparser__Value,
						   this->leveltable_currentlevel,
						   line);
		    if ( result != 0 ) {
			err_report(FUNC_NAME,"ERROR",
				   "element_handler() has reported an error");
			goto quit;
		    }
		}
		else {
#if DEBUG
		    printf("[?????]:[%s]\n",line);
#endif
		}
	    }
	    /* 前から続いている or < で始まっている 場合 */
	    else {
		/* > で閉じてなかった場合 */
		if ( target_string[len_target_string-1] != '>' ) {
		    if ( prev_line == NULL ) {
			prev_line = c_strdup(target_string);
			if ( prev_line == NULL ) {
			    err_throw(FUNC_NAME,"FATAL","strdup() failed");
			}
		    }
		    else {
			size_t len;
			len = c_strlen(prev_line);
			tmp_ptr=realloc(prev_line,len+len_target_string+1+1);
			if ( tmp_ptr == NULL ) {
			    err_throw(FUNC_NAME,"FATAL","realloc() failed");
			}
			prev_line = (char *)tmp_ptr;
			c_strcat(prev_line," ");
			c_strcat(prev_line,target_string);
		    }
		}
		/* > で閉じていた場合 */
		else {
		    size_t i,len;
		    if ( prev_line == NULL ) {
			line=target_string;
		    }
		    else {
			len = c_strlen(prev_line);
			tmp_ptr=realloc(prev_line,len+len_target_string+1+1);
			if ( tmp_ptr == NULL ) {
			    err_throw(FUNC_NAME,"FATAL","realloc() failed");
			}
			prev_line = (char *)tmp_ptr;
			c_strcpy(prev_line+len," ");
			c_strcpy(prev_line+len+1,target_string);
			if ( copied_string != NULL ) free(copied_string);
			copied_string=prev_line;
			prev_line=NULL;
			line=copied_string;
		    }
		    /* 最後の > を消す */ 
		    line[c_strlen(line)-1]='\0';
		    /* 最初の < を消す */
		    line++;
		    /* 最初の空白を消す */
		    while ( *line == ' ' || *line == '\t' || *line == '\n' ) 
			line++;
		    /* 最後の空白を消す */
		    len = c_strlen(line);
		    for ( i=0 ; i<len ; i++ ) {
			if ( line[len-1-i] != ' ' && 
			     line[len-1-i] != '\t' &&
			     line[len-1-i] != '\n' ) break;
		    }
		    if ( i<len ) {
			line[len-1-i+1]='\0';
			/*
			 * 条件に応じてハンドラを呼びだす
			 */
			if ( line[0] == '!' ) {
#if DEBUG
			    printf("[COMMENT]:[%s]\n",line);
#endif
			    result =
				this->element_handler(Xmlparser__Comment,
						      this->leveltable_currentlevel,
						      line);
			    if ( result != 0 ) {
				err_report(FUNC_NAME,"ERROR",
				    "element_handler() has reported an error");
				goto quit;
			    }
			}
			else if ( line[0] == '?' ) {
#if DEBUG
			    printf("[HEADER]:[%s]\n",line);
#endif
			    result = 
				this->element_handler(Xmlparser__Header,
						      this->leveltable_currentlevel,
						      line);
			    if ( result != 0 ) {
				err_report(FUNC_NAME,"ERROR",
				    "element_handler() has reported an error");
				goto quit;
			    }
			}
			else if ( line[0] != '/' ) {
			    this->leveltable_currentlevel++;
			    if ( this->leveltable_allocnum == this->leveltable_currentlevel ) {
				void *tmp_ptr1;
				this->leveltable_allocnum++;
				tmp_ptr = realloc(this->leveltable,sizeof(*(this->leveltable))*this->leveltable_allocnum);
				tmp_ptr1 = realloc(this->value_leveltable,
						   sizeof(*(this->value_leveltable))*this->leveltable_allocnum);
				if ( tmp_ptr != NULL ) {
				    this->leveltable = (tarray_tstring **)tmp_ptr;
				    this->leveltable[this->leveltable_currentlevel]=NULL;
				}
				if ( tmp_ptr1 != NULL ) {
				    this->value_leveltable = (xmlparser__value_leveltable **)tmp_ptr1;
				    this->value_leveltable[this->leveltable_currentlevel]=NULL;
				}
				if ( tmp_ptr == NULL || tmp_ptr1 == NULL ) {
				    this->leveltable_allocnum--;
				    err_throw(FUNC_NAME,"FATAL",
					      "realloc() failed");
				}
			    }

			    /* initialize value_leveltable[current] */
			    if ( this->value_leveltable[this->leveltable_currentlevel] != NULL ) {
				int i = this->leveltable_currentlevel;
				int j;
				for ( j=0 ; this->value_leveltable[i][j].name != NULL ; j++ ) {
				    free(this->value_leveltable[i][j].name);
				    free(this->value_leveltable[i][j].value);
				}
				free(this->value_leveltable[i]);
				this->value_leveltable[i] = NULL;
			    }
			    
			    len = c_strlen(line);
			    if ( line[len-1] == '/' ) {
				tarray_tstring *sobjptr = NULL;
				line[len-1] = '\0';
				if ( this->leveltable[this->leveltable_currentlevel] == NULL ) {
				    try {
					sobjptr = new tarray_tstring;
				    }
				    catch (...) {
					err_throw(FUNC_NAME,"FATAL",
						  "'new' failed");
				    }
				    this->leveltable[this->leveltable_currentlevel] = sobjptr;
				}
				else {
				    sobjptr = this->leveltable[this->leveltable_currentlevel];
				}
				try {
				    sobjptr->split(line," \t\n",false,"\"",'\\',false);
				}
				catch (...) {
				    err_throw(FUNC_NAME,"FATAL",
					      "sobjptr->split() failed");
				}
#if DEBUG
				printf("[SOLO]:[%s]\n",line);
#endif
				result = 
				   this->element_handler(Xmlparser__Solo,
							 this->leveltable_currentlevel,
							 line);
				if ( result != 0 ) {
				    err_report(FUNC_NAME,"ERROR",
				    "element_handler() has reported an error");
				    goto quit;
				}
				this->leveltable_currentlevel--;
			    }
			    else {
				tarray_tstring *sobjptr = NULL;
				if ( this->leveltable[this->leveltable_currentlevel] == NULL ) {
				    try {
					sobjptr = new tarray_tstring;
				    }
				    catch (...) {
					err_throw(FUNC_NAME,"FATAL",
						  "'new' failed");
				    }
				    this->leveltable[this->leveltable_currentlevel] = sobjptr;
				}
				else {
				    sobjptr = this->leveltable[this->leveltable_currentlevel];
				}
				try {
				    sobjptr->split(line," \t\n",false,"\"",'\\',false);
				}
				catch (...) {
				    err_throw(FUNC_NAME,"FATAL",
					      "sobjptr->split() failed");
				}
#if DEBUG
				printf("[BEGIN]:[%s]\n",line);
#endif
				result = 
				  this->element_handler(Xmlparser__Begin,
							this->leveltable_currentlevel,
							line);
				if ( result != 0 ) {
				    err_report(FUNC_NAME,"ERROR",
				    "element_handler() has reported an error");
				    goto quit;
				}
				/* -- */
			    }
			}
			else {
			    bool flag_broken;
			    /* 頭の / を消す */
			    line++;
#if DEBUG
			    printf("[END]:[%s]\n",line);
#endif
			    flag_broken=false;
#if DEBUG
			    printf("[compare:[%s]:[%s]]\n",
				   leveltable[leveltable_currentlevel][0],
				   line);
#endif
			    if ( 0 <= this->leveltable_currentlevel ) {
				const char *const *elems;
				elems = this->leveltable[this->leveltable_currentlevel]->cstrarray();
				if ( elems==NULL || c_strcmp(elems[0],line)!=0 )
				    flag_broken=true;
			    }
			    else flag_broken=true;
			    if ( flag_broken == true ) {
				err_report(FUNC_NAME,"ERROR",
					   "the XML file is broken");
				c_fprintf(c_stderr(),
					"leveltable_currentlevel: %d\n",
					this->leveltable_currentlevel);
				c_fprintf(c_stderr(),"line: [%s]\n",line);
				goto quit;
			    }
			    result= this->element_handler(Xmlparser__End,
							  this->leveltable_currentlevel,
							  line);
			    if ( result != 0 ) {
				err_report(FUNC_NAME,"ERROR",
				    "element_handler has reported an error");
				goto quit;
			    }
			    this->leveltable_currentlevel--;
			}
		    }
		}
	    }
	} while ( next_line != NULL && next_line[0] != '\0' );
      }

    }
    catch (...) {
	error_throw = true;
	goto quit;
    }

    result_return=0;

 quit:

    if ( next_line != NULL ) free(next_line);
    if ( prev_line != NULL ) free(prev_line);
    if ( copied_string != NULL ) free(copied_string);
    if ( target_string != NULL ) free(target_string);
    
    if ( 0 < this->leveltable_allocnum ) {
	if ( this->leveltable != NULL ) {
	    int i;
	    for ( i=0 ; i < this->leveltable_allocnum ; i++ ) {
		if ( leveltable[i] != NULL ) {
		    delete leveltable[i];
		}
	    }
	    free(this->leveltable);
	    this->leveltable = NULL;
	}
	if ( this->value_leveltable != NULL ) {
	    int i;
	    for ( i=0 ; i < leveltable_allocnum ; i++ ) {
		if ( this->value_leveltable[i] != NULL ) {
		    int j;
		    for ( j=0 ; this->value_leveltable[i][j].name != NULL ; j++ ) {
			free(this->value_leveltable[i][j].name);
			free(this->value_leveltable[i][j].value);
		    }
		    free(this->value_leveltable[i]);
		}
	    }
	    free(this->value_leveltable);
	    this->value_leveltable = NULL;
	}
    }
    this->leveltable_allocnum = 0;
    this->leveltable_currentlevel=-1;

    this->xmlline_ptr = NULL;
    this->xmlline_size = 0;
    this->xmlline_current_pos = 0;
    this->xmlline_next_pos = 0;
    if ( this->filled_xmlline_buffer != NULL ) {
	free(this->filled_xmlline_buffer);
	this->filled_xmlline_buffer = NULL;
    }
    this->filled_xmlline_is_provided = false;

    if ( error_throw == true ) {
	err_throw(FUNC_NAME,"FATAL","caught exception");
    }

    return result_return;
}
#undef FUNC_NAME


/* trash... */

#if 0
#define FUNC_MSG "xmlparser::line_reader(): "
const char *xmlparser::line_reader( long long *size )
{
    const char *return_ptr = NULL;

    if ( this->stream != NULL ) {
	if ( this->left_xml_size < 0 ) {
	    const char *p = this->stream->getline();
	    if ( p == NULL ) {
		*size = 0;	/* EOF: return NULL */
	    }
	    else {
		*size = c_strlen(p);
		return_ptr = p;
	    }
	}
	else {
	    if ( 0 < this->left_xml_size ) {
		long long l;
		char *tmp_ptr;
		tmp_ptr = (char *)realloc(this->tmp_xml_buffer,
					  1 + this->left_xml_size);
		if ( tmp_ptr == NULL ) {
		    *size = -1;
		    c_fprintf(c_stderr(),FUNC_MSG 
			    "[FATAL ERROR] realloc() failed\n");
		    goto quit;
		}
		memset( tmp_ptr, 0, 1 + this->left_xml_size );
		this->tmp_xml_buffer = tmp_ptr;
		l=this->stream->read(this->tmp_xml_buffer,this->left_xml_size);
		if ( l < 0 ) {
		    *size = -1;
		    c_fprintf(c_stderr(),FUNC_MSG "[ERROR] stream->read() failed\n");
		    goto quit;
		}
		else if ( l == 0 ) {
		    *size = 0;	/* EOF: return NULL */
		    goto quit;
		}
		this->left_xml_size -= l;
		*size = l;
		return_ptr = this->tmp_xml_buffer;
		if ( this->left_xml_size < 0 ) {
		    this->left_xml_size = 0;	/* something wrong... */
		}
	    }
	    else {
		*size = 0;	/* DONE: return NULL */
	    }
	}
    }
    else if ( this->all_xml_ptr != NULL ) {
	if ( this->left_xml_size < 0 ) {
	    *size = c_strlen(this->all_xml_ptr);
	    return_ptr = this->all_xml_ptr;
	}
	else if ( 0 < this->left_xml_size ) {
	    *size = this->left_xml_size;
	    return_ptr = this->all_xml_ptr;
	}
	else {
	    *size = 0;	/* return NULL */
	}
	this->all_xml_ptr = NULL;	/* always DONE */
	this->left_xml_size = -1;
    }
    else {
	*size = 0;	/* return NULL */
    }

 quit:
    return return_ptr;
}
#undef FUNC_MSG
#endif

}	/* namespace sli */

#include "private/c_fprintf.c"
#include "private/c_stderr.c"

#include "private/c_strcpy.c"
#include "private/c_strcat.c"
#include "private/c_strlen.cc"
#include "private/c_strchr.cc"
#include "private/c_strcmp.cc"
#include "private/c_strncmp.cc"
#include "private/c_strdup.c"
#include "private/c_memcpy.cc"
#include "private/c_memchr.cc"

#include "private/fillup_inconvenient_chars.c"
