/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2013-05-13 23:20:03 cyamauch> */

/**
 * @file   cstreamio.cc
 * @brief  ���ȥ꡼�������ϤΤ������ݴ��쥯�饹 cstreamio �Υ�����
 */

#define CLASS_NAME "cstreamio"

#include "config.h"

#include "cstreamio.h"
#include "heap_mem.h"

#include <stdlib.h>
#include <limits.h>
#include <sys/stat.h>
#include <assert.h>

#include "sli_funcs.h"
#include "sli_seek.h"
#include "private/err_report.h"

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#include "private/c_close.h"
#include "private/c_dup.h"

#include "private/c_vsnprintf.h"
#include "private/c_vsscanf.h"
#include "private/c_fgetc.h"
#include "private/c_fgets.h"
#include "private/c_fputc.h"
#include "private/c_fputs.h"
#include "private/c_feof.h"
#include "private/c_ferror.h"
#include "private/c_clearerr.h"
#include "private/c_fflush.h"
#include "private/c_fread.h"
#include "private/c_fwrite.h"
#include "private/c_fclose.h"
#include "private/c_fseek.h"
#include "private/c_rewind.h"
#include "private/c_ftell.h"
#include "private/c_fdopen.h"

#include "private/c_strncmp.h"
#include "private/c_strlen.h"
#include "private/c_memcpy.h"
#include "private/cpy_with_byte_swap.h"


namespace sli
{

/* 
 * endian info
 */
static const bool Byte_order_is_little = Sli__byte_order_is_little_endian;
static const bool Float_word_order_is_little = Sli__float_word_order_is_little_endian;

/**
 * @brief  ���󥹥ȥ饯��
 */
cstreamio::cstreamio()
{
    /* */
    this->size_line_buffer=0;
    this->line_buffer=NULL;
    /* */
    this->size_printf_buffer=0;
    this->printf_buffer=NULL;
    /* */
    this->read_buffer = NULL;
    this->read_buffer_next_pointer = NULL;
    this->read_buffer_read_length = 0;
    this->read_buffer_size = 0;
    /* */
    this->fp = NULL;
    this->fp_seekable_rec = false;
    /* */
    this->ind_sp = NULL;
    /* */
    this->eofflg_rec = 0;
    this->errflg_rec = 0;

    return;
}

/**
 * @brief  �ǥ��ȥ饯��
 */
cstreamio::~cstreamio()
{
    this->close();
    return;
}

/* copy constructor (dummy) */

/**
 * @deprecated  ���Ѥ��ʤ��ǲ�������
 */
cstreamio::cstreamio(const cstreamio &obj)
{
    sli__eprintf(
	    "[FATAL ERROR] Do not pass by value for cstreamio classes!!\n");
    abort();
    return;
}

/* overloading operator = (dummy) */

/**
 * @deprecated  ���Ѥ��ʤ��ǲ�������
 */
cstreamio &cstreamio::operator=(const cstreamio &obj)
{
    sli__eprintf(
	    "[FATAL ERROR] Do not use '=' operator for cstreamio classes!!\n");
    abort();
    return *this;
}

/*
const char *cstreamio::operator=(const char *s)
{
    this->putstr(s);
    return s;
}
*/

/**
 * @brief  ���ȥ꡼��Υ����ץ�
 *
 *  �ե�����̾����ꤷ�ơ����ȥ꡼��򥪡��ץ󤷤ޤ���
 *
 * @param      mode ���ȥ꡼��򳫤��⡼�� ("r" or "w")
 * @param      format �ե�����̾�Υե����ޥåȻ���
 * @param      ap �ե�����̾�������ǥǡ���
 * @return     ������������0<br>
 *             ���顼�ξ�������
 * @note       cstreamio ���饹����ݥ��饹�ʤΤǡ��桼����ľ�����Ѥ������
 *             �Ǥ��ޤ���
 */
int cstreamio::vopenf( const char *mode, const char *format, va_list ap )
{
    assert( mode != NULL );
    assert( format != NULL );

    char pathname[PATH_MAX+1];
    c_vsnprintf( pathname, PATH_MAX+1, format, ap );
    return this->open(mode,pathname);
}

/**
 * @brief  ���ȥ꡼��Υ����ץ�
 *
 *  �ե�����̾����ꤷ�ơ����ȥ꡼��򥪡��ץ󤷤ޤ���
 *
 * @param      mode ���ȥ꡼��򳫤��⡼�� ("r" or "w")
 * @param      format �ե�����̾�Υե����ޥåȻ���
 * @param      ... �ե�����̾�γ����ǥǡ���
 * @return     ������������0<br>
 *             ���顼�ξ�������
 * @note       cstreamio ���饹����ݥ��饹�ʤΤǡ��桼����ľ�����Ѥ������
 *             �Ǥ��ޤ���
 */
int cstreamio::openf( const char *mode, const char *format, ... )
{
    assert( mode != NULL );
    assert( format != NULL );

    int return_status;
    va_list ap;
    va_start(ap, format);
    try {
	return_status = this->vopenf( mode, format, ap );
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","unexpected exception");
    }
    va_end(ap);
    return return_status;
}

/**
 * @brief  ��ñ�̤ǤΥե����ޥå��ɤ߹���
 *
 *  open()�ǳ��������ȥ꡼�फ��ե����ޥåȤ˽��äơ���ñ�̤��ɤ߹��ߤޤ���
 *
 * @param      format �ɤ߹��ߥե����ޥåȻ���
 * @param      ap �ɤ߹��������ǥǡ���
 * @return     �������������������ǿ�<br>
 *             ���ȥ꡼��ν���ޤ��ϥ��顼�ξ���EOF
 * @throw      �ǡ����ɤ߹����Ѥ��ΰ�γ��ݤ˼��Ԥ������
 * @note       cstreamio ���饹����ݥ��饹�ʤΤǡ��桼����ľ�����Ѥ������
 *             �Ǥ��ޤ���
 */
int cstreamio::vscanf( const char *format, va_list ap )
{
    assert( format != NULL );

    int return_status = EOF;
    const char *line;

    line = this->getline();
    if ( line == NULL ) goto quit;

    return_status = c_vsscanf(line,format,ap);

 quit:
    return return_status;
}

/**
 * @brief  ��ñ�̤ǤΥե����ޥå��ɤ߹���
 *
 *  open()�ǳ��������ȥ꡼�फ��ե����ޥåȤ˽��äơ���ñ�̤��ɤ߹��ߤޤ���
 *
 * @param      format �ɤ߹��ߥե����ޥåȻ��� 
 * @param      ... �ɤ߹�������ǥǡ���
 * @return     �������������������ǿ�<br>
 *             ���ȥ꡼��ν���ޤ��ϥ��顼�ξ���EOF
 * @throw      �ǡ����ɤ߹����Ѥ��ΰ�γ��ݤ˼��Ԥ������
 * @note       cstreamio ���饹����ݥ��饹�ʤΤǡ��桼����ľ�����Ѥ������
 *             �Ǥ��ޤ���
 */
int cstreamio::scanf( const char *format, ... )
{
    assert( format != NULL );

    int return_status;
    va_list ap;
    va_start(ap, format);
    try {
	return_status = this->vscanf( format, ap );
    }
    catch ( err_rec st ) {
	va_end(ap);
	err_throw(st.func_name, st.level, st.message);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","unexpected exception");
    }
    va_end(ap);
    return return_status;
}

/**
 * @brief  �ե����ޥå��ɤ߹���(ʸ��������ͭ)
 *
 *  open()�ǳ��������ȥ꡼�फ��ե����ޥåȤ˽��ä��ɤ߹��ߤޤ���
 *
 * @param      nchars �ɤ߹���ʸ������������
 * @param      format �ɤ߹��ߥե����ޥåȻ���
 * @param      ap �ɤ߹��������ǥǡ���
 * @return     �������������������ǿ�<br>
 *             ���ȥ꡼��ν���ޤ��ϥ��顼�ξ���EOF
 * @throw      �ǡ����ɤ߹����Ѥ��ΰ�γ��ݤ˼��Ԥ������
 * @note       cstreamio ���饹����ݥ��饹�ʤΤǡ��桼����ľ�����Ѥ������
 *             �Ǥ��ޤ���
 */
int cstreamio::vscanf( size_t nchars, const char *format, va_list ap )
{
    assert( format != NULL );

    int return_status = EOF;
    const char *line;

    line = this->getline(nchars);
    if ( line == NULL ) goto quit;

    return_status = c_vsscanf(line,format,ap);

 quit:
    return return_status;
}

/**
 * @brief  �ե����ޥå��ɤ߹���(ʸ��������ͭ)
 *
 *  open()�ǳ��������ȥ꡼�फ��ե����ޥåȤ˽��ä��ɤ߹��ߤޤ���
 *
 * @param      nchars �ɤ߹���ʸ������������
 * @param      format �ɤ߹��ߥե����ޥåȻ���
 * @param      ... �ɤ߹�������ǥǡ���
 * @return     �������������������ǿ�<br>
 *             ���ȥ꡼��ν���ޤ��ϥ��顼�ξ���EOF
 * @throw      �ǡ����ɤ߹����Ѥ��ΰ�γ��ݤ˼��Ԥ������
 * @note       cstreamio ���饹����ݥ��饹�ʤΤǡ��桼����ľ�����Ѥ������
 *             �Ǥ��ޤ���
 */
int cstreamio::scanf( size_t nchars, const char *format, ... )
{
    assert( format != NULL );

    int return_status;
    va_list ap;
    va_start(ap, format);
    try {
	return_status = this->vscanf( nchars, format, ap );
    }
    catch ( err_rec st ) {
        va_end(ap);
        err_throw(st.func_name, st.level, st.message);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","unexpected exception");
    }
    va_end(ap);
    return return_status;
}




#define BUF_BLOCK 512

/**
 * @brief  �ե����ޥåȽ񤭹���
 *
 *  open()�ǳ��������ȥ꡼��ˡ��ե����ޥåȤ˽��äƽ񤭹��ߤޤ���
 * 
 * @param      format �񤭹��ߥե����ޥåȻ���
 * @param      ap �񤭹��������ǥǡ���
 * @return     �����������Ͻ񤭹��ޤ줿ʸ����<br>
 *             ���顼�ξ�������
 * @throw      �ǡ����񤭹����Ѥ��ΰ�γ��ݤ˼��Ԥ������
 * @throw      �񤭹��ߤ˼��Ԥ������
 * @note       cstreamio ���饹����ݥ��饹�ʤΤǡ��桼����ľ�����Ѥ������
 *             �Ǥ��ޤ���
 */
int cstreamio::vprintf( const char *format, va_list ap )
{
    assert( format != NULL );

    int return_value=-1;
    int nn;
    size_t size_nn;
    va_list aq;

    va_copy(aq, ap);

    try {
	if ( this->printf_buffer == NULL ) {
	    const size_t new_alloc = BUF_BLOCK;
	    if ( this->realloc_printf_buffer(new_alloc) < 0 ) {
		err_throw(__FUNCTION__,"FATAL","realloc() failed");
	    }
	    this->size_printf_buffer = new_alloc;
	}

	nn = c_vsnprintf( this->printf_buffer, this->size_printf_buffer, 
			  format, aq );
	if ( nn < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","vsnprintf() failed");
	}
	size_nn = nn;
    }
    catch (...) {
	va_end(aq);
	err_throw(__FUNCTION__,"FATAL","caught exception");
    }

    va_end(aq);

    if ( this->size_printf_buffer < size_nn+1 ) {
        const size_t t = size_nn / BUF_BLOCK + 1;
        const size_t new_alloc = BUF_BLOCK * t;
	if ( this->realloc_printf_buffer(new_alloc) < 0 ) {
            err_throw(__FUNCTION__,"FATAL","realloc() failed");
        }
        this->size_printf_buffer = new_alloc;

        nn=c_vsnprintf( this->printf_buffer, this->size_printf_buffer, 
                      format, ap );
	if ( nn < 0 ) {
            err_throw(__FUNCTION__,"FATAL","vsnprintf() failed");
	}
	size_nn = nn;

        if ( this->size_printf_buffer < size_nn+1 ) {
            err_throw(__FUNCTION__,"FATAL",
		      "unexpected return value of vsnprintf()");
        }
    }

    return_value = this->write( this->printf_buffer, size_nn );

    // quit:
    return return_value;
}

/**
 * @brief  �ե����ޥåȽ񤭹���
 *
 *  open()�ǳ��������ȥ꡼��ˡ��ե����ޥåȤ˽��äƽ񤭹��ߤޤ���
 *
 * @param      format �񤭹��ߥե����ޥåȻ���
 * @param      ... �񤭹�������ǥǡ���
 * @return     �����������Ͻ񤭹��ޤ줿ʸ����<br>
 *             ���顼�ξ�������
 * @throw      �ǡ����񤭹����Ѥ��ΰ�γ��ݤ˼��Ԥ������
 * @throw      �񤭹��ߤ˼��Ԥ������
 * @note       cstreamio ���饹����ݥ��饹�ʤΤǡ��桼����ľ�����Ѥ������
 *             �Ǥ��ޤ���
 */
int cstreamio::printf( const char *format, ... )
{
    assert( format != NULL );

    int return_status;
    va_list ap;
    va_start(ap, format);
    try {
	return_status = this->vprintf( format, ap );
    }
    catch ( err_rec st ) {
        va_end(ap);
        err_throw(st.func_name, st.level, st.message);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","unexpected exception");
    }
    va_end(ap);
    return return_status;
}

/**
 * @brief  �ƥ����ȥե������1���ɤ߹���
 *  
 *  ���߰��֤������ʸ���ޤǤ�ʸ�����饤�֥�������ΥХåե����ɤ߹��ߡ�
 *  ���������Хåե��Υ��ɥ쥹���֤��ޤ���
 *
 * @return     �����������������Хåե��Υ��ɥ쥹<br>
 *             ���ȥ꡼��ν���ޤ��ϥ��顼�ξ���NULL
 * @throw      �ǡ����ɤ߹����Ѥ��ΰ�γ��ݤ˼��Ԥ������
 * @attention  �֤��줿�Хåե���ץ���ޤ��������ƤϤ����ޤ���
 * @note       cstreamio ���饹����ݥ��饹�ʤΤǡ��桼����ľ�����Ѥ������
 *             �Ǥ��ޤ���
 */
const char *cstreamio::getline() 
{
    char *return_value = NULL;
    char *ptr;
    size_t len;
    if ( this->line_buffer == NULL ) {
	if ( this->realloc_line_buffer(BUF_BLOCK) < 0 ) {
            err_throw(__FUNCTION__,"FATAL","realloc() failed");
	}
	this->size_line_buffer = BUF_BLOCK;
    }
    /* */
    ptr = this->getstr( this->line_buffer, this->size_line_buffer );
    if ( ptr == NULL ) {
	this->line_buffer[0] = '\0';
	goto quit;
    }
    /* */
    len = c_strlen(this->line_buffer);
    if ( len < 1 ) {
	this->line_buffer[0] = '\0';
	return_value = this->line_buffer;
	goto quit;
    }
    /* */
    while ( 1 ) {
	if ( this->line_buffer[len-1] == '\n' ) {
	    return_value = this->line_buffer;
	    break;
	}
	if (this->realloc_line_buffer(this->size_line_buffer + BUF_BLOCK) < 0) {
            err_throw(__FUNCTION__,"FATAL","realloc() failed");
	}
	this->size_line_buffer += BUF_BLOCK;
	ptr = this->getstr( this->line_buffer + len,
			    this->size_line_buffer - len );
	if ( ptr == NULL ) {
	    this->line_buffer[len] = '\0';
	    return_value = this->line_buffer;
	    break;
	}
	len = c_strlen(this->line_buffer);
    }

 quit:
    return return_value;
}

/**
 * @brief  �ƥ����ȥե������1���ɤ߹���(ʸ��������ͭ)
 *
 *  ���߰��֤������ʸ���ޤǤ�ʸ�����饤�֥�������ΥХåե����ɤ߹��ߡ�
 *  ���������Хåե��Υ��ɥ쥹���֤��ޤ���<br>
 *  ����ʸ��������뤫������ʸ������ã����ޤ��ɤ߹��ߤޤ���
 *
 * @param      nchars �ɤ߹���ʸ������������
 * @return     �����������������Хåե��Υ��ɥ쥹<br>
 *             ���ȥ꡼��ν���ޤ��ϥ��顼�ξ���NULL
 * @throw      �ǡ����ɤ߹����Ѥ��ΰ�γ��ݤ˼��Ԥ������
 * @attention  �֤��줿�Хåե���ץ���ޤ��������ƤϤ����ޤ���
 * @note       cstreamio ���饹����ݥ��饹�ʤΤǡ��桼����ľ�����Ѥ������
 *             �Ǥ��ޤ���
 */

const char *cstreamio::getline( size_t nchars )
{
    char *return_value = NULL;
    char *ptr;
    if ( nchars <= 0 ) return this->getline();
    if ( this->size_line_buffer <= nchars ) {
	const size_t n = 1 + nchars / BUF_BLOCK;
	if ( this->realloc_line_buffer(n * BUF_BLOCK) < 0 ) {
            err_throw(__FUNCTION__,"FATAL","realloc() failed");
	}
	this->size_line_buffer = n * BUF_BLOCK;
    }
    /* */
    ptr = this->getstr( this->line_buffer, nchars + 1 );
    if ( ptr == NULL ) {
	this->line_buffer[0] = '\0';
	goto quit;
    }
    return_value = this->line_buffer;

 quit:
    return return_value;
}

#include "private/byte_swap.cc"

/**
 * @brief  �Х��ʥꥹ�ȥ꡼����ɤ߼��(�Х��ȥ��������Ѵ����б�)
 *
 *  open()�ǳ��������ȥ꡼�फ�������ޤ�����ư�������ͤ��ɤ߹��ߡ�Ϳ����줿
 *  ���˳�Ǽ���ޤ���<br>
 *  ���ȥ꡼��ǡ����Υ���ǥ��������˱����ơ��Х��ȥ����������Ѵ����ޤ���
 *
 * @param    buf �ǡ����γ�Ǽ���
 * @param    sz_type ���ȥ꡼��ǡ������̤ȥХ��ȿ�(��ξ�����ư��������)
 * @param    n �ǡ����θĿ�
 * @param    little_endian ���ȥ꡼��ǡ����Υ���ǥ��������
 * @return   �����������ϥХ��ȿ�<br>
 *           ���ȥ꡼�ब����ξ�硤0�Х��Ȥ��ɤ߹��ߤ����ꤵ�줿����0<br>
 *           ���顼�ξ�������
 * @note     cstreamio ���饹����ݥ��饹�ʤΤǡ��桼����ľ�����Ѥ������
 *           �Ǥ��ޤ���
 */
ssize_t cstreamio::bread( void *buf, ssize_t sz_type,
			  size_t n, bool little_endian )
{
    ssize_t ret;
    const size_t sz_bytes = (0 <= sz_type) ? sz_type : -sz_type;

    ret = this->read(buf, sz_bytes * n);

    if ( buf != NULL && 1 < sz_bytes && 1 < ret ) {
	bool swap;
	if ( sz_type < -1 ) {
	    if ( Float_word_order_is_little == little_endian ) swap = false;
	    else swap = true;
	}
	else {
	    if ( Byte_order_is_little == little_endian ) swap = false;
	    else swap = true;
	}
	if ( swap == true ) {
	    byte_swap( (unsigned char *)buf, sz_bytes, ret / sz_bytes );
	}
    }
    return ret;
}

/**
 * @brief  �Х��ʥꥹ�ȥ꡼����ɤ߼��(ʣ���η��ΥХ��ȥ����������Ѵ����б�)
 *
 *  open()�ǳ��������ȥ꡼�फ�������ޤ�����ư�������ͤ��ɤ߹��ߡ�Ϳ����줿
 *  ���˳�Ǽ���ޤ���<br>
 *  ���ȥ꡼��ǡ����Υ���ǥ��������˱����ơ��Х��ȥ����������Ѵ����ޤ���
 *
 * @param    buf �ǡ����γ�Ǽ���
 * @param    binfo ���ȥ꡼��ǡ������̤ȥХ��ȿ��ξ��󥻥å�
 * @param    n �ǡ������åȤθĿ�
 * @param    little_endian ���ȥ꡼��ǡ����Υ���ǥ��������
 * @return   �����������ϥХ��ȿ�<br>
 *           ���ȥ꡼�ब����ξ�硤0�Х��Ȥ��ɤ߹��ߤ����ꤵ�줿����0<br>
 *           ���顼�ξ�������
 * @note     cstreamio ���饹����ݥ��饹�ʤΤǡ��桼����ľ�����Ѥ������
 *           �Ǥ��ޤ���
 */
ssize_t cstreamio::bread( void *buf, const bstream_info binfo[],
			  size_t n, bool little_endian )
{
    unsigned char *buf_out = (unsigned char *)buf;
    ssize_t l_done, ret_all = 0;
    size_t len_block;
    size_t ii, jj;
    if ( buf_out == NULL ) return -1;
    if ( binfo == NULL ) return -1;

    /* binfo 1��ʬ��1�֥�å��Ȥ��� */
    len_block = 0;
    for ( ii=0 ; binfo[ii].sz_type != 0 ; ii++ ) {
	const ssize_t sz_type = binfo[ii].sz_type;
	const size_t sz_bytes = (0 <= sz_type) ? sz_type : -sz_type;
	len_block += sz_bytes * binfo[ii].length;
    }
    if ( len_block == 0 ) goto quit;

    /* �ɤ������ɤ� */
    ret_all = this->read(buf_out, len_block * n);
    if ( ret_all < 0 ) {
	goto quit;
    }

    l_done = 0;
    for ( jj=0 ; jj < n ; jj++ ) {
	for ( ii=0 ; binfo[ii].sz_type != 0 ; ii++ ) {
	    const ssize_t sz_type = binfo[ii].sz_type;
	    const size_t length = binfo[ii].length;
	    size_t sz_bytes;
	    bool swap;

	    if ( ret_all <= l_done ) goto quit;
   
	    if ( sz_type < -1 ) {
		sz_bytes = -sz_type;
		if ( Float_word_order_is_little == little_endian ) swap = false;
		else swap = true;
	    }
	    else if ( 1 < sz_type ) {
		sz_bytes = sz_type;
		if ( Byte_order_is_little == little_endian ) swap = false;
		else swap = true;
	    }
	    else {
		sz_bytes = 1;
		swap = false;
	    }

	    if ( swap == true ) {
		byte_swap(buf_out + l_done, sz_bytes, length);
	    }

	    l_done += sz_bytes * length;
	}
    }

 quit:
    return ret_all;
}

#define DEFAULT_TMP_BUFSIZE 65536

/**
 * @brief  �Х��ʥꥹ�ȥ꡼��ν񤭹���(�Х��ȥ����������Ѵ����б�)
 *
 *  ���ꤵ�줿��꤫�����������ޤ�����ư�������ͤ�open()�ǳ��������ȥ꡼���
 *  �񤭹��ߤޤ���<br>
 *  ���ȥ꡼��ǡ����Υ���ǥ��������˱����ơ��Х��ȥ����������Ѵ����ޤ���
 *
 * @param      buf �ǡ����γ�Ǽ���
 * @param      sz_type ���ȥ꡼��ǡ������̤ȥХ��ȿ�(��ξ�����ư��������)
 * @param      n �ǡ����θĿ�
 * @param      little_endian ���ȥ꡼��ǡ����Υ���ǥ��������
 * @return     �����������ϥХ��ȿ�<br>
 *             ���顼�ξ�������
 * @throw      ����ǥ������Ѵ��Ѥΰ���ΰ�γ��ݤ˼��Ԥ������
 * @note       cstreamio ���饹����ݥ��饹�ʤΤǡ��桼����ľ�����Ѥ������
 *             �Ǥ��ޤ���
 */
ssize_t cstreamio::bwrite( const void *buf, ssize_t sz_type,
			   size_t n, bool little_endian )
{
    ssize_t return_value = -1;
    const size_t sz_bytes = (0 <= sz_type) ? sz_type : -sz_type;

    if ( buf != NULL ) {
	bool swap;
	if ( 1 < sz_bytes && 0 < n ) {
	    if ( sz_type < -1 ) {
	       if ( Float_word_order_is_little == little_endian ) swap = false;
	       else swap = true;
	    }
	    else {
	       if ( Byte_order_is_little == little_endian ) swap = false;
	       else swap = true;
	    }
	}
	else swap = false;
	if ( swap == true ) {
	    const unsigned char *src_ptr;
	    heap_mem<unsigned char> tmp_buf;
	    size_t tmp_buf_size;
	    size_t i, n1;
	    ssize_t w_bytes;
	    if ( sz_bytes * n < DEFAULT_TMP_BUFSIZE )
		tmp_buf_size = sz_bytes * n;
	    else {
		if ( sz_bytes < DEFAULT_TMP_BUFSIZE ) 
		    tmp_buf_size = DEFAULT_TMP_BUFSIZE;
		else tmp_buf_size = sz_bytes;
	    }
	    if ( tmp_buf.allocate(tmp_buf_size) < 0 ) {
		err_throw(__FUNCTION__,"FATAL","malloc() failed");
	    }
	    n1 = tmp_buf_size / sz_bytes;
	    w_bytes = 0;
	    src_ptr = (const unsigned char *)buf;
	    for ( i=0 ; i < n ; ) {
		ssize_t ret_w;
		size_t nn;
		/* determine n of elements */
		if ( i + n1 <= n ) nn = n1;
		else nn = n - i;
		/* copy with endian conversion */
		cpy_with_byte_swap( src_ptr, sz_bytes, nn, tmp_buf.ptr() );
		src_ptr += sz_bytes * nn;
		i += nn;
		/* output ... */
		try {
		    ret_w = this->write(tmp_buf.ptr(), sz_bytes * nn);
		}
		catch (...) {
		    err_throw(__FUNCTION__,"FATAL","unexpected exception");
		}
		if ( ret_w < 0 ) goto quit;
		w_bytes += ret_w;
		if ( ret_w < (ssize_t)(sz_bytes * nn) ) {
		    return_value = w_bytes;
		    goto quit;
		}
	    }
	    return_value = w_bytes;
	}
	else {
	    return_value = this->write(buf, sz_bytes * n);
	}
    }

 quit:
    return return_value;
}

/**
 * @brief  �Х��ʥꥹ�ȥ꡼��ν񤭹���(ʣ���η��ΥХ��ȥ����������Ѵ����б�)
 *
 *  ���ꤵ�줿��꤫�����������ޤ�����ư�������ͤ�open()�ǳ��������ȥ꡼���
 *  �񤭹��ߤޤ���<br>
 *  ���ȥ꡼��ǡ����Υ���ǥ��������˱����ơ��Х��ȥ����������Ѵ����ޤ���
 *
 * @param      buf �ǡ����γ�Ǽ���
 * @param      binfo ���ȥ꡼��ǡ������̤ȥХ��ȿ��ξ��󥻥å�
 * @param      n �ǡ������åȤθĿ�
 * @param      little_endian ���ȥ꡼��ǡ����Υ���ǥ��������
 * @return     �����������ϥХ��ȿ�<br>
 *             ���顼�ξ�������
 * @throw      ����ǥ������Ѵ��Ѥΰ���ΰ�γ��ݤ˼��Ԥ������
 * @note       cstreamio ���饹����ݥ��饹�ʤΤǡ��桼����ľ�����Ѥ������
 *             �Ǥ��ޤ���
 */
ssize_t cstreamio::bwrite( const void *buf, const bstream_info binfo[],
			   size_t n, bool little_endian )
{
    const unsigned char *buf_in = (const unsigned char *)buf;
    ssize_t return_value = -1;
    ssize_t ret_all = 0;
    heap_mem<unsigned char> tmp_buf;
    size_t tmp_buf_size;
    size_t len_block, n_block, ii, jj, kk;

    if ( buf_in == NULL ) goto quit;
    if ( binfo == NULL ) goto quit;

    /* binfo 1��ʬ��1�֥�å��Ȥ��� */
    len_block = 0;
    for ( ii=0 ; binfo[ii].sz_type != 0 ; ii++ ) {
	const ssize_t sz_type = binfo[ii].sz_type;
	const size_t sz_bytes = (0 <= sz_type) ? sz_type : -sz_type;
	len_block += sz_bytes * binfo[ii].length;
    }
    if ( len_block == 0 ) goto quit;

    /* ����� 1�֥�å��������ܳ��ݤ��� */
    if ( len_block < DEFAULT_TMP_BUFSIZE ) {
	n_block = DEFAULT_TMP_BUFSIZE / len_block;
	if ( n < n_block ) n_block = n;
    } else {
	n_block = 1;
    }
    tmp_buf_size = len_block * n_block;
    if ( tmp_buf.allocate(tmp_buf_size) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","malloc() failed");
    }

    for ( jj=0 ; jj < n ; ) {
	register unsigned char *dest_ptr = tmp_buf.ptr();
	for ( kk=0 ; kk < n_block && jj < n ; kk++, jj++ ) {
	    for ( ii=0 ; binfo[ii].sz_type != 0 ; ii++ ) {
		const ssize_t sz_type = binfo[ii].sz_type;
		const size_t length = binfo[ii].length;
		size_t sz_bytes;
		bool swap;

		if ( sz_type < -1 ) {
		    sz_bytes = -sz_type;
		    if ( Float_word_order_is_little == little_endian ) swap = false;
		    else swap = true;
		}
		else if ( 1 < sz_type ) {
		    sz_bytes = sz_type;
		    if ( Byte_order_is_little == little_endian ) swap = false;
		    else swap = true;
		}
		else {
		    sz_bytes = 1;
		    swap = false;
		}

		if ( swap == true ) {
		    /* copy with endian conversion */
		    cpy_with_byte_swap( buf_in, sz_bytes, length, dest_ptr );
		    dest_ptr += sz_bytes * length;
		    buf_in += sz_bytes * length;
		}
		else {
		    c_memcpy(dest_ptr, buf_in, sz_bytes * length);
		    dest_ptr += sz_bytes * length;
		    buf_in += sz_bytes * length;
		}
	    }
	}
	if ( 0 < kk ) {
	    ssize_t ret;
	    try {
		ret = this->write(tmp_buf.ptr(), len_block * kk);
	    }
	    catch (...) {
		err_throw(__FUNCTION__,"FATAL","unexpected exception");
	    }
	    if ( ret < 0 ) {
		if ( ret_all == 0 ) ret_all = ret;
		return_value = ret_all;
		goto quit;
	    }
	    ret_all += ret;
	    if ( ret < (ssize_t)(len_block * kk) ) {
		return_value = ret_all;
		goto quit;
	    }
	}
    }

    return_value = ret_all;
 quit:
    return return_value;
}

#undef DEFAULT_TMP_BUFSIZE


/**
 * @brief  1ʸ�����ɤ߹���
 *
 *  open()�ǳ��������ȥ꡼�फ�鼡��ʸ�����ɤ߹��ߡ�int�����֤��ޤ���
 *
 * @return     �������������ɤ߹����ʸ����int���˥��㥹�Ȥ�����<br>
 *             ���ȥ꡼��ν���ޤ��ϥ��顼�ξ���EOF
 * @note       cstreamio ���饹����ݥ��饹�ʤΤǡ��桼����ľ�����Ѥ������
 *             �Ǥ��ޤ���
 */
int cstreamio::getchr()
{
    int c;

    if ( this->fp != NULL ) {
	c = c_fgetc(this->fp);
	goto quit;
    }

    /* check buffer */
    if ( this->read_buffer_next_pointer != NULL ) {
	c = ((unsigned char *)(this->read_buffer_next_pointer))[0];
	this->read_buffer_next_pointer++;
	if ( this->read_buffer + this->read_buffer_read_length <=
	     this->read_buffer_next_pointer ) {
	    this->read_buffer_next_pointer = NULL;
	}
    }
    else {
	unsigned char buf[1];
	if ( this->raw_read( buf, 1 ) == 1 ) {
	    c = buf[0];
	}
	else c = EOF;
    }

 quit:
    return c;
}

/**  
 * @brief  ʸ������ɤ߹���
 *
 *  open() �ǳ��������ȥ꡼�फ��ʸ�����ɤ߹��ߡ����ꤵ�줿�Хåե��˳�Ǽ
 *  ���ޤ���
 *  �ɤ߹��ߤ�EOF�ޤ��ϲ���ʸ�����ɤ߹���������ߤ��ޤ���<br>
 *  �ɤ߹��ޤ줿����ʸ������ꤵ�줿�Хåե��˳�Ǽ����ޤ���
 *
 * @param      s �ɤ߹����ʸ���γ�Ǽ���
 * @param      size �ɤ߹���ʸ����
 * @return     �����������ϳ�Ǽ�Хåե��Υ��ɥ쥹<br>
 *             ���ȥ꡼��ν���ޤ��ϥ��顼�ξ���NULL
 * @throw      �ǡ����ɤ߹����Ѥ��ΰ�γ��ݤ˼��Ԥ������
 * @note       cstreamio ���饹����ݥ��饹�ʤΤǡ��桼����ľ�����Ѥ������
 *             �Ǥ��ޤ���
 */
char *cstreamio::getstr( char *s, size_t size )
{
    char *return_pointer = NULL;
    char *new_s_ptr = s;
    size_t new_size = size;
    ssize_t l;

    if ( size == 0 || s == NULL ) goto quit;

    if ( this->fp != NULL ) {
	if ( size == 1 ) s[0] = '\0';
	return_pointer = c_fgets(s,size,this->fp);
	goto quit;
    }

    /* check internal buffer */
    if ( this->read_buffer_next_pointer != NULL ) {
	const size_t done = this->read_buffer_next_pointer - this->read_buffer;
	const size_t left = this->read_buffer_read_length - done;
	size_t i;
	for ( i=0 ; i < new_size-1 && i < left ; i++ ) {
	    new_s_ptr[i] = this->read_buffer_next_pointer[0];
	    this->read_buffer_next_pointer++;
	    if ( new_s_ptr[i] == '\n' ) {
		i++;
		break;
	    }
	}
	if ( left <= i ) {
	    this->read_buffer_next_pointer = NULL;
	}
	if ( new_s_ptr[i-1] == '\n' || new_size-1 <= i ) {
	    new_s_ptr[i] = '\0';
	    if ( 1 < size ) {
		return_pointer = s;
	    }
	    goto quit;
	}
	new_size -= i;
	new_s_ptr += i;
    }
    /* re-allocating internal buffer */
    if ( this->read_buffer_size < new_size-1 ) {
	if ( this->realloc_read_buffer(new_size-1) < 0 ) {
            err_throw(__FUNCTION__,"FATAL","realloc() failed");
	}
	this->read_buffer_size = new_size-1;
    }
    /* reading file */
    l = this->raw_read( this->read_buffer, new_size-1 );
    this->read_buffer_read_length = l;
    if ( l <= 0 ) {
	if ( new_s_ptr == s ) {
	    if ( 0 < size ) new_s_ptr[0] = '\0';
	    /* EOF */
	}
	else {
	    if ( 0 < new_size ) new_s_ptr[0] = '\0';
	    return_pointer = s;
	}
    }
    else {
	ssize_t i;
	for ( i=0 ; i < l ; i++ ) {
	    new_s_ptr[i] = this->read_buffer[i];
	    if ( new_s_ptr[i] == '\n' ) {
		i++;
		break;
	    }
	}
	new_s_ptr[i] = '\0';
	if ( i < l ) {
	    this->read_buffer_next_pointer = this->read_buffer + i;
	}
	return_pointer = s;
    }

 quit:
    return return_pointer;
}

/**
 * @brief  �ǡ������ɤ߹���
 *
 *  open()�ǳ��������ȥ꡼�फ��ǡ������ɤ߹��ߡ�Ϳ����줿���˳�Ǽ���ޤ���
 *
 * @param    buf �ǡ����γ�Ǽ���
 * @param    size �ǡ����θĿ�
 * @return   �����������ϥХ��ȿ�<br>
 *           ���ȥ꡼�ब����ξ�硤0�Х��Ȥ��ɤ߹��ߤ����ꤵ�줿����0<br>
 *           ���顼�ξ�������
 * @note     cstreamio ���饹����ݥ��饹�ʤΤǡ��桼����ľ�����Ѥ������
 *           �Ǥ��ޤ���
 */
ssize_t cstreamio::read( void *buf, size_t size )
{
    ssize_t return_len = -1;
    size_t new_size = size;
    char *new_buf_ptr = (char *)buf;
    ssize_t l;

    if ( buf == NULL && 0 < size ) goto quit;

    if ( this->fp != NULL ) {
	if ( c_ferror(this->fp) ) return_len = -1;
	else if ( c_feof(this->fp) ) return_len = 0;
	else if ( size == 0 ) return_len = 0;
	else {
	    size_t n;
	    n = c_fread( buf, 1,size, this->fp );
	    if ( 0 < n ) return_len = n;
	    else {
		if ( c_ferror(this->fp) ) return_len = -1;
		else if ( c_feof(this->fp) ) return_len = 0;
	    }
	}
	goto quit;
    }

    /* checking internal buffer */
    if ( this->read_buffer_next_pointer != NULL ) {
	size_t done = this->read_buffer_next_pointer - this->read_buffer;
	size_t left = this->read_buffer_read_length - done;
	if ( 0 < new_size ) {
	    if ( new_size < left ) {
		c_memcpy(new_buf_ptr,this->read_buffer_next_pointer,new_size);
		this->read_buffer_next_pointer += new_size;
		new_size = 0;
	    }
	    else {
		c_memcpy(new_buf_ptr,this->read_buffer_next_pointer,left);
		this->read_buffer_next_pointer = NULL;
		new_size -= left;
		new_buf_ptr += left;
	    }
	    if ( new_size == 0 ) {
		return_len = size;
		goto quit;
	    }
	}
	else {
	    return_len = 0;
	    goto quit;
	}
    }

    /* reading from file */
    l = this->raw_read( new_buf_ptr, new_size );
    if ( l <= 0 ) {
	if ( size == new_size ) return_len = l;
	else return_len = size - new_size;
    }
    else return_len = size - new_size + l;

 quit:
    return return_len;
}

/**
 * @brief  1ʸ���ν񤭹���
 *
 *  ���ꤵ�줿ʸ����open()�ǳ��������ȥ꡼��˽񤭹��ߤޤ���
 *
 * @param      c �񤭹���ʸ��
 * @return     �����������Ͻ񤭹��ޤ줿ʸ����int���˥��㥹�Ȥ�����<br>
 *             ���顼�ξ���EOF
 * @note       cstreamio ���饹����ݥ��饹�ʤΤǡ��桼����ľ�����Ѥ������
 *             �Ǥ��ޤ���
 */
int cstreamio::putchr( int c )
{
    if ( this->fp != NULL ) {
	return c_fputc(c,this->fp);
    }
    else {
	ssize_t l;
	unsigned char ch = c;
	l = this->raw_write(&ch,1);
	if ( l != 1 ) return EOF;
	else return 1;
    }
}

/**
 * @brief  ʸ����ν񤭹���
 *
 *  ���ꤵ�줿ʸ�����open()�ǳ��������ȥ꡼��˽񤭹��ߤޤ���
 *
 * @param      s �񤭹���ʸ����
 * @return     ��������������ǤϤʤ���<br>
 *             ���顼�ξ���EOF
 * @note       cstreamio ���饹����ݥ��饹�ʤΤǡ��桼����ľ�����Ѥ������
 *             �Ǥ��ޤ���
 */
int cstreamio::putstr( const char *s )
{
    if ( s == NULL ) return EOF;
    if ( this->fp != NULL ) {
	return c_fputs(s,this->fp);
    }
    else {
	ssize_t l,len;
	len = c_strlen(s);
	l = this->raw_write(s,len);
	if ( l == len ) return 1;
	else return EOF;
    }
}

/**
 * @brief  �ǡ����ν񤭹���
 *
 *  ���ꤵ�줿�ǡ�����open()�ǳ��������ȥ꡼��˽񤭹��ߤޤ���
 *
 * @param      buf �ǡ����γ�Ǽ���
 * @param      size �ǡ����θĿ�
 * @return     �����������ϥХ��ȿ�<br>
 *             ���顼�ξ�������
 * @note       cstreamio ���饹����ݥ��饹�ʤΤǡ��桼����ľ�����Ѥ������
 *             �Ǥ��ޤ���
 */
ssize_t cstreamio::write( const void *buf, size_t size )
{
    if ( buf == NULL && 0 < size ) return -1;
    if ( this->fp != NULL ) {
	size_t n;
	n = c_fwrite( buf, 1,size, this->fp );
	if ( 0 < n ) return n;
	else {
	    if ( size == 0 ) return 0;
	    else return -1;
	}
    }
    else {
	return this->raw_write( buf, size );
    }
}

#define DEFAULT_TMP_BUFSIZE 65536

/**
 * @brief  �ɤ����Ф�(��ǽ�ʤ饷����������)
 *
 *  �ɤ߹����ѥ��ȥ꡼����Ф��ơ���ǽ�Ǥ���� n �Х��ȤΥ�������Ԥʤ��ޤ���
 *  ���������Ǥ��ʤ����ϡ�n �Х��Ȥ��ɤ����Ф���Ԥʤ��ޤ���
 *
 * @param   n �Х��ȿ�
 * @return  �����������ϥХ��ȿ�<br>
 *          ���ȥ꡼�ब����ξ�硤0�Х��Ȥ��ɤ߹��ߤ����ꤵ�줿����0<br>
 *          ���顼�ξ�������
 * @throw   ����ΰ�γ��ݤ˼��Ԥ������
 * @note    ����������ǽ�ʾ��ϼºݤˤ��ɤ߹��ߤϹԤʤ��ޤ���<br>
 *          cstreamio ���饹����ݥ��饹�ʤΤǡ��桼����ľ�����Ѥ������
 *          �Ǥ��ޤ���
 */
ssize_t cstreamio::rskip( size_t n )
{
    heap_mem<unsigned char> tmp_buf;
    size_t tmp_buf_size;
    ssize_t ret_all = 0;

    /*
     * �̾�ե�����Ǥ��ꡤ��������ǽ�ե饰��true�ξ��
     */
    if ( this->fp != NULL && this->fp_seekable_rec == true ) {
	//err_report(__FUNCTION__,"DEBUG","mark [seekable]");
	if ( n == 0 ) return n;
	else {
	    ssize_t pos0,pos1,sz;
	    int ret, ch;
	    pos0 = c_ftell( this->fp );
	    /* fseek() �λ��ͤäơ�2nd arg �� EOF �����ӱۤ������ؤ��Ƥ��Ƥ�
	       ���Τޤޥ������Ǥ������ˤ����㤦�Τ�orz */
	    ret = c_fseek( this->fp, n - 1, SEEK_CUR );
	    if ( ret != 0 ) return -1;
	    ch = c_fgetc(this->fp);
	    if ( c_ferror(this->fp) ) return -1;
	    if ( ch == EOF ) {
		ret = c_fseek( this->fp, 0, SEEK_END );	/* goto EOF */
		ch = c_fgetc(this->fp);			/* set EOF flag */
	    }
	    if ( ret != 0 ) return -1;
	    pos1 = c_ftell( this->fp );
	    //err_report1(__FUNCTION__,"DEBUG","mark %d", c_feof(this->fp));
	    //err_report1(__FUNCTION__,"DEBUG","mark %d", c_ferror(this->fp));
	    sz = pos1 - pos0;
	    return sz;
	}
    }

    /*
     * �ʲ����������Ǥ��ʤ����(�ɤ����Ф�)
     */

    //err_report(__FUNCTION__,"DEBUG","mark [non seekable]");

    if ( DEFAULT_TMP_BUFSIZE < n ) tmp_buf_size = DEFAULT_TMP_BUFSIZE;
    else tmp_buf_size = n;

    if ( 0 < tmp_buf_size ) {
	if ( tmp_buf.allocate(tmp_buf_size) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","malloc() failed");
	}
    }

    while ( 1 ) {
	ssize_t ret;
	size_t length;
	if ( tmp_buf_size < n ) length = tmp_buf_size;
	else length = n;
	/* cstreamio������Ƥ֤Τϡ�raw_read() ��Ƥ֤Τ������ˤ��뤿�� */
	ret = this->cstreamio::read(tmp_buf.ptr(), length);
	if ( ret < 0 ) {
	    /* error */
	    if ( ret_all == 0 ) ret_all = ret;
	    break;
	}
	else if ( ret <= (ssize_t)length ) {
	    /* OK */
	    if ( ret == 0 ) break;	/* maybe EOF */
	    ret_all += ret;
	    n -= ret;
	    if ( n == 0 ) break;	/* finished */
	}
	else {
	    /* never */
	    err_throw(__FUNCTION__,"FATAL","internal error");
	}
    }

    return ret_all;
}

/**
 * @brief  �����Ф�(��ǽ�ʤ饷����������)
 *
 *  �񤭹����ѥ��ȥ꡼����Ф��ơ���ǽ�Ǥ���� n �Х��ȤΥ�������Ԥʤ��ޤ���
 *  ���������Ǥ��ʤ����ϡ�n �Х���ʬ����饯�� ch ��񤭹��ߤޤ���
 *
 * @param     n �Х��ȿ�
 * @param     ch �������Ǥ��ʤ����˽񤭹��७��饯��
 * @return    �����������ϥХ��ȿ�<br>
 *            ���顼�ξ�������
 * @throw     ����ΰ�γ��ݤ˼��Ԥ������
 * @note      ��������ǽ�ʾ�硤ch �ǻ��ꤷ������饯���Ͻ񤭹��ޤ�ޤ���<br>
 *            cstreamio ���饹����ݥ��饹�ʤΤǡ��桼����ľ�����Ѥ������
 *            �Ǥ��ޤ���
 */
ssize_t cstreamio::wskip( size_t n, int ch )
{
    heap_mem<unsigned char> tmp_buf;
    size_t tmp_buf_size, i;
    ssize_t ret_all = 0;

    /*
     * �̾�ե�����Ǥ��ꡤ��������ǽ�ե饰��true�ξ��
     */
    if ( this->fp != NULL && this->fp_seekable_rec == true ) {
	int ret = c_fseek( this->fp, n, SEEK_CUR );
	if ( ret == 0 ) return n;
	else return -1;
    }

    /*
     * �ʲ����������Ǥ��ʤ����(ch��n�Х�������)
     */

    if ( DEFAULT_TMP_BUFSIZE < n ) tmp_buf_size = DEFAULT_TMP_BUFSIZE;
    else tmp_buf_size = n;

    if ( 0 < tmp_buf_size ) {
	if ( tmp_buf.allocate(tmp_buf_size) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","malloc() failed");
	}
	for ( i=0 ; i < tmp_buf_size; i++ ) tmp_buf[i] = ch;
    }

    while ( 1 ) {
	ssize_t ret;
	size_t length;
	if ( tmp_buf_size < n ) length = tmp_buf_size;
	else length = n;
	/* write */
	try {
	    /* cstreamio������Ƥ֤Τϡ�raw_write() ��Ƥ֤Τ������ˤ��뤿�� */
	    ret = this->cstreamio::write(tmp_buf.ptr(), length);
	}
	catch (...) {
	    err_throw(__FUNCTION__,"FATAL","unexpected exception");
	}
	if ( ret < 0 ) {
	    /* error */
	    if ( ret_all == 0 ) ret_all = ret;
	    break;
	}
	else if ( ret <= (ssize_t)length ) {
	    /* basically OK */
	    ret_all += ret;
	    n -= ret;
	    if ( ret < (ssize_t)length ) break;	/* maybe error */
	    if ( n == 0 ) break;		/* finished */
	}
	else {
	    /* never */
	    err_throw(__FUNCTION__,"FATAL","internal error");
	}
    }
    
    return ret_all;
}

#undef DEFAULT_TMP_BUFSIZE


/**
 * @brief  ��������ǽ���ɤ������֤�
 */
bool cstreamio::is_seekable() const
{
    return this->fp_seekable_rec;
}

/**
 * @brief  ���ȥ꡼��ΰ��֤��ѹ�
 *
 *  ���ȥ꡼��ΰ��֤��ѹ����ޤ�
 *
 * @param      offset ���ȥ꡼�����ɽ���ҤΥ��ե��å�
 * @param      whence ���ȥ꡼�����ɽ���Ҥδ�����
 * @return     ������������0<br>
 *             ���顼�ξ�������
 * @note       cstreamio ���饹����ݥ��饹�ʤΤǡ��桼����ľ�����Ѥ������
 *             �Ǥ��ޤ���
 */
int cstreamio::seek( long offset, int whence )
{
    if ( this->fp != NULL && this->fp_seekable_rec == true ) {
	return c_fseek(this->fp, offset, whence);
    }
    else return -1;
}

/**
 * @brief  ���ȥ꡼��ΰ��֤���Ƭ���ѹ�
 *
 *  ���ȥ꡼��ΰ��֤���Ƭ���ѹ����ޤ�
 *
 * @return     ������������0<br>
 *             ���顼�ξ�������
 * @note       cstreamio ���饹����ݥ��饹�ʤΤǡ��桼����ľ�����Ѥ������
 *             �Ǥ��ޤ���
 */
int cstreamio::rewind()
{
    if ( this->fp != NULL && this->fp_seekable_rec == true ) {
	c_rewind(this->fp);
	return 0;
    }
    else return -1;
}

/**
 * @brief  ���ȥ꡼����֤μ���
 *
 *  ���ȥ꡼�����ɽ���Ҥ��ͤ��֤��ޤ�
 *
 * @return     ������������0�ʾ����<br>
 *             ���顼�ξ�������
 * @note       cstreamio ���饹����ݥ��饹�ʤΤǡ��桼����ľ�����Ѥ������
 *             �Ǥ��ޤ���
 */
long cstreamio::tell()
{
    if ( this->fp != NULL && this->fp_seekable_rec == true ) {
	return c_ftell(this->fp);
    }
    else return -1;
}

/**
 * @brief  �񤭹����ѥ��ȥ꡼��Υե�å���
 * 
 *  open()�ǳ��������ȥ꡼����ΥХåե��˳�Ǽ����Ƥ��뤹�٤ƤΥǡ������
 *  ���ߤޤ���
 *
 * @return     ������������0<br>
 *             ���顼�ξ���0�ʳ�����
 * @note       cstreamio ���饹����ݥ��饹�ʤΤǡ��桼����ľ�����Ѥ������
 *             �Ǥ��ޤ���
 */
int cstreamio::flush()
{
    if ( this->fp != NULL ) {
	return c_fflush(this->fp);
    }
    else if ( this->ind_sp != NULL ) {
	return this->ind_sp->flush();
    }
    else {
	return EOF;
    }
}

/* same as feof() in stdio.h */

/**
 * @brief  EOF�ؼ��Ҥμ���
 */
int cstreamio::eof()
{
    if ( this->fp != NULL ) {
	return c_feof(this->fp);
    }
    else {
	return this->eofflg_rec;
    }
}

/* same as ferror() in stdio.h */

/**
 * @brief  ���顼�ؼ��Ҥμ���
 */
int cstreamio::error()
{
    if ( this->fp != NULL ) {
	return c_ferror(this->fp);
    }
    else {
	return this->errflg_rec;
    }
}

/* same as clearerr() in stdio.h */

/**
 * @brief  EOF�ؼ��Ҥȥ��顼�ؼ��ҤΥꥻ�å�
 */
cstreamio &cstreamio::reseterr()
{
    if ( this->fp != NULL ) {
	c_clearerr(this->fp);
    }
    else {
	this->eofflg_rec = 0;
	this->errflg_rec = 0;
    }
    return *this;
}

/* this is a fall-back member function */

/**  
 * @brief  ɸ�������ϤΥ����ץ�
 *
 *  ɸ�������Ϥ򥪡��ץ󤷤ޤ���<br>
 *  mode �� "r" �����ꤵ�줿����ɸ�����Ϥ�"w" �����ꤵ�줿����ɸ����Ϥ�
 *  �����ץ󤷤ޤ�.
 *
 * @param      mode ���ȥ꡼��򳫤��⡼�� ("r" or "w")
 * @return     ������������0<br>
 *             ���顼�ξ�������
 * @throw      err_rec ɸ�������ϤΥե����롦�ǥ�������ץ���ʣ���˼��Ԥ������
 * @note       cstreamio ���饹����ݥ��饹�ʤΤǡ��桼����ľ�����Ѥ������
 *             �Ǥ��ޤ���
 */
int cstreamio::open( const char *mode )
{
    assert( mode != NULL );

    int return_status = -1;
    int fd = -1;
    if ( this->fp != NULL || this->ind_sp != NULL ) goto quit;

    if ( c_strncmp(mode,"r",1) == 0 ) {
	fd=c_dup(0);
	if ( fd == -1 ) {
            err_throw(__FUNCTION__,"FATAL","dup() failed");
	}
    }
    else if ( c_strncmp(mode,"w",1) == 0 ) {
	fd=c_dup(1);
	if ( fd == -1 ) {
            err_throw(__FUNCTION__,"FATAL","dup() failed");
	}
    }
    this->fp = c_fdopen(fd,mode);
    if ( this->fp == NULL ) goto quit;

    return_status = 0;
 quit:
    if ( return_status != 0 && 0 <= fd ) c_close(fd);
    return return_status;
}

/* this is a fall-back member function */

/**
 * @brief  �ǥ�������ץ�����ˤ�륹�ȥ꡼��Υ����ץ�
 *
 *  �ǥ�������ץ�����ꤷ�ơ����ȥ꡼��򥪡��ץ󤷤ޤ���
 *
 * @param      mode ���ȥ꡼��򳫤��⡼�� ("r" or "w")
 * @param      fd   �ե�����ǥ�������ץ�
 * @return     ������������0<br>
 *             ���顼�ξ�������
 * @note       cstreamio ���饹����ݥ��饹�ʤΤǡ��桼����ľ�����Ѥ������
 *             �Ǥ��ޤ���
 */
int cstreamio::open( const char *mode, int fd )
{
    assert( mode != NULL );

    int return_status = -1;
    int fd_new = -1;
    struct stat st;

    if ( this->fp != NULL || this->ind_sp != NULL ) goto quit;

    if ( fd == 0 ) {
        if ( c_strncmp(mode,"r",1) != 0 ) goto quit;
    } else if (fd == 1 || fd == 2) {
        if ( c_strncmp(mode,"w",1) != 0 ) goto quit;
    } else {
        /* DO_NOTHIG */
    }
    fd_new = c_dup(fd);
    this->fp = c_fdopen(fd_new,mode);
    if ( this->fp == NULL ) goto quit;

    if ( fstat(fd_new, &st) == 0 ) {
	if ( S_ISREG(st.st_mode) ) this->fp_seekable_rec = true;
    }

    return_status = 0;
 quit:
    if ( return_status != 0 && 0 <= fd_new ) c_close(fd_new);
    return return_status;
}

/**
 * @brief  ���ȥ꡼��Υ�����
 *
 *  open()�ǳ��������ȥ꡼��򥯥������ޤ���
 *
 * @return     ������������0<br>
 *             ���顼�ξ���0�ʳ�����
 * @note       cstreamio ���饹����ݥ��饹�ʤΤǡ��桼����ľ�����Ѥ������
 *             �Ǥ��ޤ���
 */
int cstreamio::close()
{
    int status = 0;

    this->free_line_buffer();
    this->size_line_buffer = 0;
    this->free_printf_buffer();
    this->size_printf_buffer = 0;
    this->free_read_buffer();
    this->read_buffer_next_pointer = NULL;
    this->read_buffer_read_length = 0;
    this->read_buffer_size = 0;

    if ( this->fp != NULL ) {
        status = c_fclose(this->fp);
	this->fp = NULL;
	this->fp_seekable_rec = false;
    }

    this->ind_sp = NULL;

    return status;
}

/**
 * @brief  cstreamio�ηѾ����饹�Υ��ȥ꡼����Ф��륪���ץ�
 *
 *  cstreamio�ηѾ����饹�Υ��֥������Ȥ���ꤷ�ơ����ȥ꡼��򳫤��ޤ���
 *
 * @param      mode ���ȥ꡼��򳫤��⡼�� ("r" or "w")
 * @param      sref cstreamio���饹�ηѾ����饹�Υ��󥹥���
 * @return     ������������0<br>
 *             ���顼�ξ�������
 * @note       cstreamio ���饹����ݥ��饹�ʤΤǡ��桼����ľ�����Ѥ������
 *             �Ǥ��ޤ���
 */
int cstreamio::open( const char *mode, cstreamio &sref )
{
    assert( mode != NULL );

    int return_status = -1;

    if ( this->fp != NULL || this->ind_sp != NULL ) goto quit;

    this->ind_sp = &sref;

    return_status = 0;
 quit:
    return return_status;
}

/* raw r/w member functions */
/* Do not use cstreamio::raw_read() and cstreamio::raw_write(). */
/* These member functions should be overridden. */

/**
 * @brief  �Ѿ����饹�ˤƥ����Х饤�ɤ����٤����ɤ߹����Ѹ��ϴؿ��Υ���å�
 *
 * @param    buf �ǡ����γ�Ǽ���
 * @param    size �ǡ����θĿ�
 * @return   �����������ϥХ��ȿ�<br>
 *           ���ȥ꡼�ब����ξ�硤0�Х��Ȥ��ɤ߹��ߤ����ꤵ�줿����0<br>
 *           ���顼�ξ������
 * @note     ���Υ��дؿ���protected�Ǥ�
 * @note     cstreamio ���饹����ݥ��饹�ʤΤǡ��桼����ľ�����Ѥ������
 *           �Ǥ��ޤ���
 */
ssize_t cstreamio::raw_read( void *buf, size_t size )
{
    ssize_t return_len = -1;

    if ( buf == NULL && 0 < size ) goto quit;

    if ( this->ind_sp != NULL ) {
	return_len = this->ind_sp->read(buf,size);
    }
    
 quit:
    return return_len;
}

/**
 * @brief  �Ѿ����饹�ˤƥ����Х饤�ɤ����٤����񤭹����Ѹ��ϴؿ��Υ���å�
 *
 * @param      buf �ǡ����γ�Ǽ���
 * @param      size �ǡ����θĿ�
 * @return     �����������ϥХ��ȿ�<br>
 *             ���顼�ξ�������
 * @note       ���Υ��дؿ���protected�Ǥ�
 * @note       cstreamio ���饹����ݥ��饹�ʤΤǡ��桼����ľ�����Ѥ������
 *             �Ǥ��ޤ���
 */
ssize_t cstreamio::raw_write( const void *buf, size_t size )
{
    ssize_t return_len = -1;

    if ( buf == NULL && 0 < size ) goto quit;

    if ( this->ind_sp != NULL ) {
	return_len = this->ind_sp->write(buf,size);
    }
    
 quit:
    return return_len;
}

/*
 * private member functions
 */

int cstreamio::realloc_line_buffer( size_t len_bytes )
{
    void *tmp_ptr;
    if ( this->line_buffer == NULL && len_bytes == 0 ) return 0;
    tmp_ptr = realloc(this->line_buffer, len_bytes);
    if ( tmp_ptr == NULL && 0 < len_bytes ) return -1;
    else {
	this->line_buffer = (char *)tmp_ptr;
	return 0;
    }
}

int cstreamio::realloc_printf_buffer( size_t len_bytes )
{
    void *tmp_ptr;
    if ( this->printf_buffer == NULL && len_bytes == 0 ) return 0;
    tmp_ptr = realloc(this->printf_buffer, len_bytes);
    if ( tmp_ptr == NULL && 0 < len_bytes ) return -1;
    else {
	this->printf_buffer = (char *)tmp_ptr;
	return 0;
    }
}

int cstreamio::realloc_read_buffer( size_t len_bytes )
{
    void *tmp_ptr;
    if ( this->read_buffer == NULL && len_bytes == 0 ) return 0;
    tmp_ptr = realloc(this->read_buffer, len_bytes);
    if ( tmp_ptr == NULL && 0 < len_bytes ) return -1;
    else {
	this->read_buffer = (char *)tmp_ptr;
	return 0;
    }
}

void cstreamio::free_line_buffer()
{
    if ( this->line_buffer != NULL ) {
	free(this->line_buffer);
	this->line_buffer = NULL;
    }
    return;
}

void cstreamio::free_printf_buffer()
{
    if ( this->printf_buffer != NULL ) {
	free(this->printf_buffer);
	this->printf_buffer = NULL;
    }
    return;
}

void cstreamio::free_read_buffer()
{
    if ( this->read_buffer != NULL ) {
	free(this->read_buffer);
	this->read_buffer = NULL;
    }
    return;
}


}	/* namespace sli */


#include "private/c_close.c"
#include "private/c_dup.c"

#include "private/c_vsnprintf.c"
#include "private/c_vsscanf.c"
#include "private/c_fgetc.cc"
#include "private/c_fgets.c"
#include "private/c_fputc.cc"
#include "private/c_fputs.c"
#include "private/c_feof.c"
#include "private/c_ferror.c"
#include "private/c_clearerr.c"
#include "private/c_fflush.c"
#include "private/c_fread.c"
#include "private/c_fwrite.c"
#include "private/c_fclose.c"
#include "private/c_fseek.c"
#include "private/c_rewind.c"
#include "private/c_ftell.c"
#include "private/c_fdopen.c"

#include "private/c_strncmp.cc"
#include "private/c_strlen.cc"
#include "private/c_memcpy.cc"
#include "private/cpy_with_byte_swap.cc"
