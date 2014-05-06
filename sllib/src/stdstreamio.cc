/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2013-05-13 23:20:29 cyamauch> */

/**
 * @file  stdstreamio.cc
 * @brief ɸ�������Ϥȥ�����ե��������Υ��ȥ꡼��򰷤� stdstreamio �Υ�����
 */

#define CLASS_NAME "stdstreamio"

#include "config.h"

#include "stdstreamio.h"
#include <stdlib.h>
#include <sys/stat.h>
#include <assert.h>

#include "private/err_report.h"

#include "private/c_stdout.h"
#include "private/c_stdin.h"
#include "private/c_stderr.h"
#include "private/c_fopen.h"
#include "private/c_fclose.h"
#include "private/c_vfprintf.h"
#include "private/c_fflush.h"
#include "private/c_fileno.h"
#include "private/get_file_size.h"


namespace sli
{

/**
 * @brief  ���󥹥ȥ饯��
 */
stdstreamio::stdstreamio()
{
    this->p_stdout = c_stdout();
    this->fp = this->p_stdout;
    return;
}

/**
 * @brief  ���󥹥ȥ饯��
 *
 *  ɸ����ϤΤ�����ɸ�२�顼���Ϥ�ǥե���Ȥν�����ˤ�����˻Ȥ��ޤ���
 */
stdstreamio::stdstreamio(bool to_stderr)
{
    if ( to_stderr == true ) this->p_stdout = c_stderr();
    else this->p_stdout = c_stdout();
    this->fp = this->p_stdout;
    return;
}

/**
 * @brief  �ǥ��ȥ饯��
 */
stdstreamio::~stdstreamio()
{
    this->close();
    if ( this->fp == this->p_stdout || this->fp == c_stdin() ) this->fp = NULL;
    return;
}

/* overridden member functions */

/**
 * @brief  ɸ�������ϤΥ����ץ�
 *
 *  ɸ�������Ϥ򥪡��ץ󤷤ޤ�.<br>
 *  mode�� "r" �����ꤵ�줿����ɸ�����Ϥ�"w" �����ꤵ�줿����ɸ����Ϥ�
 *  �����ץ󤷤ޤ�.
 *
 * @param      mode ���ȥ꡼��򳫤��⡼�� ("r" or "w")
 * @return     ������������0<br>
 *             ���顼�ξ�������
 * @throw      err_rec ɸ�������ϤΥե����롦�ǥ�������ץ���ʣ���˼��Ԥ������
 */
int stdstreamio::open( const char *mode )
{
    assert( mode != NULL );

    if ( this->fp == this->p_stdout || this->fp == c_stdin() ) this->fp = NULL;
    return this->cstreamio::open(mode);
}

/**
 * @brief  �ե�����Υ����ץ�
 *
 *  �ե�����̾����ꤷ�ơ����ȥ꡼��򥪡��ץ󤷤ޤ���
 *
 * @param      mode ���ȥ꡼��򳫤��⡼�� ("r" or "w")
 * @param      path   �ե�����̾
 * @return     ������������0<br>
 *             ���顼�ξ�������
 * @throw      err_rec ɸ�������ϤΥե����롦�ǥ�������ץ���ʣ���˼��Ԥ������
 */
int stdstreamio::open( const char *mode, const char *path )
{
    assert( mode != NULL );

    int return_status = -1;
    struct stat st;

    if ( path == NULL ) return this->open(mode);

    if ( this->fp == this->p_stdout || this->fp == c_stdin() ) this->fp = NULL;
    if ( this->fp != NULL ) goto quit;
    this->fp = c_fopen(path,mode);
    if ( this->fp == NULL ) goto quit;

    if ( stat(path,&st) == 0 ) {
	if ( S_ISREG(st.st_mode) ) this->fp_seekable_rec = true;
    }

    return_status = 0;
 quit:
    return return_status;
}

/**
 * @brief  �ǥ�������ץ�����ˤ�륹�ȥ꡼��Υ����ץ�
 *
 *  �ǥ�������ץ�����ꤷ�ơ����ȥ꡼��򥪡��ץ󤷤ޤ���
 *
 * @param       mode ���ȥ꡼��򳫤��⡼�� ("r" or "w")
 * @param       fd   �ե�����ǥ�������ץ�
 * @return      ������������0<br>
 *              ���顼�ξ�������
 */
int stdstreamio::open( const char *mode, int fd )
{
    assert( mode != NULL );

    if ( this->fp == this->p_stdout || this->fp == c_stdin() ) this->fp = NULL;
    return this->cstreamio::open(mode,fd);
}

/**
 * @brief  ���ȥ꡼��Υ�����
 *
 *  open()�ǳ��������ȥ꡼��򥯥������ޤ���
 *
 * @return     ������������0<br>
 *             ���顼�ξ���0�ʳ�����
 */
int stdstreamio::close()
{
    int status = 0;
    if ( this->fp != NULL ) {
	if ( this->fp == this->p_stdout || this->fp == c_stdin() ) {
	    void *p_fp;
	    p_fp = this->fp;
	    this->fp = NULL;
	    this->cstreamio::close();
	    this->fp = p_fp;
	}
	else {
	    status = c_fclose(this->fp);
	    this->fp = NULL;
	    this->cstreamio::close();
	}
    }
    else this->cstreamio::close();

    return status;
}

/**
 * @brief  1ʸ�����ɤ߹���
 *
 *  open()�ǳ��������ȥ꡼�फ�鼡��ʸ�����ɤ߹��ߡ�int�����֤��ޤ���
 *
 * @return     �������������ɤ߹����ʸ����int���˥��㥹�Ȥ�����<br>
 *             ���ȥ꡼��ν���ޤ��ϥ��顼�ξ���EOF
 */
int stdstreamio::getchr()
{
    if ( this->fp == this->p_stdout ) this->fp = c_stdin();
    return this->cstreamio::getchr();
}

/**
 * @brief  ʸ������ɤ߹���
 *
 *  open() �ǳ��������ȥ꡼�फ��ʸ�����ɤ߹��ߡ����ꤵ�줿�Хåե��˳�Ǽ����
 *  ����<br>
 *  �ɤ߹��ߤ� EOF �ޤ��ϲ���ʸ�����ɤ߹���������ߤ��ޤ���
 *  �ɤ߹��ޤ줿����ʸ������ꤵ�줿�Хåե��˳�Ǽ����ޤ���
 *
 * @param      s �ɤ߹����ʸ���γ�Ǽ���
 * @param      size �ɤ߹���ʸ����
 * @return     �����������ϳ�Ǽ�Хåե��Υ��ɥ쥹<br>
 *             ���ȥ꡼��ν���ޤ��ϥ��顼�ξ���NULL
 * @throw      �ǡ����ɤ߹����Ѥ��ΰ�γ��ݤ˼��Ԥ������
 */
char *stdstreamio::getstr( char *s, size_t size )
{
    if ( this->fp == this->p_stdout ) this->fp = c_stdin();
    return this->cstreamio::getstr(s,size);
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
 */
ssize_t stdstreamio::read( void *buf, size_t size )
{
    if ( this->fp == this->p_stdout ) this->fp = c_stdin();
    return this->cstreamio::read(buf,size);
}

/**
 * @brief  1ʸ���ν񤭹���
 *
 *  ���ꤵ�줿ʸ����open()�ǳ��������ȥ꡼��˽񤭹��ߤޤ���
 *
 * @param      c �񤭹���ʸ��
 * @return     �����������Ͻ񤭹��ޤ줿ʸ����int���˥��㥹�Ȥ�����<br>
 *             ���顼�ξ���EOF
 */
int stdstreamio::putchr( int c )
{
    if ( this->fp == c_stdin() ) this->fp = this->p_stdout;
    return this->cstreamio::putchr(c);
}

/**
 * @brief  ʸ����ν񤭹���
 *
 *  ���ꤵ�줿ʸ�����open()�ǳ��������ȥ꡼��˽񤭹��ߤޤ���
 *
 * @param      s �񤭹���ʸ����
 * @return     ��������������ǤϤʤ���<br>
 *             ���顼�ξ���EOF
 */
int stdstreamio::putstr( const char *s )
{
    if ( this->fp == c_stdin() ) this->fp = this->p_stdout;
    return this->cstreamio::putstr(s);
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
 */
ssize_t stdstreamio::write( const void *buf, size_t size )
{
    if ( this->fp == c_stdin() ) this->fp = this->p_stdout;
    return this->cstreamio::write(buf,size);
}

/**
 * @brief  �ɤ����Ф�(��ǽ�ʤ饷����������)
 *
 *  �ɤ߹����ѥ��ȥ꡼����Ф��� ��ǽ�Ǥ���� n �Х��ȤΥ�������Ԥʤ��ޤ���
 *  ���������Ǥ��ʤ����ϡ�n �Х��Ȥ��ɤ����Ф���Ԥʤ��ޤ���
 *
 * @param   n �Х��ȿ�
 * @return  �����������ϥХ��ȿ�<br>
 *          ���ȥ꡼�ब����ξ�硤0�Х��Ȥ��ɤ߹��ߤ����ꤵ�줿����0<br>
 *          ���顼�ξ�������
 * @throw   ����ΰ�γ��ݤ˼��Ԥ������
 */
ssize_t stdstreamio::rskip( size_t n )
{
    if ( this->fp == this->p_stdout ) this->fp = c_stdin();
    return this->cstreamio::rskip(n);
}

/**
 * @brief  �����Ф�(��ǽ�ʤ饷����������)
 *
 *  �񤭹����ѥ��ȥ꡼����Ф��� ��ǽ�Ǥ���� n �Х��ȤΥ�������Ԥʤ��ޤ���
 *  ���������Ǥ��ʤ����ϡ�n �Х���ʬ����饯�� ch ��񤭹��ߤޤ���
 *
 * @param      n �Х��ȿ�
 * @param      ch �������Ǥ��ʤ����˽񤭹��७��饯��
 * @return     �����������ϥХ��ȿ�<br>
 *             ���顼�ξ�������
 * @throw      ����ΰ�γ��ݤ˼��Ԥ������
 */
ssize_t stdstreamio::wskip( size_t n, int ch )
{
    if ( this->fp == c_stdin() ) this->fp = this->p_stdout;
    return this->cstreamio::wskip(n,ch);
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
 */
int stdstreamio::seek( long offset, int whence )
{
    return this->cstreamio::seek(offset, whence);
}

/**
 * @brief  ���ȥ꡼��ΰ��֤���Ƭ���ѹ�
 *
 *  ���ȥ꡼��ΰ��֤���Ƭ���ѹ����ޤ�
 *
 * @return     ������������0<br>
 *             ���顼�ξ�������
 */
int stdstreamio::rewind()
{
    return this->cstreamio::rewind();
}

/**
 * @brief  ���ȥ꡼����֤μ���
 *
 *  ���ȥ꡼�����ɽ���Ҥ��ͤ��֤��ޤ�
 *
 * @return     ������������0�ʾ����<br>
 *             ���顼�ξ�������
 */
long stdstreamio::tell()
{
    return this->cstreamio::tell();
}

/**
 * @brief  �񤭹����ѥ��ȥ꡼��Υե�å���
 *
 *  ɸ����ϥ��ȥ꡼��˥Хåե��˳�Ǽ����Ƥ��뤹�٤ƤΥǡ�����񤭹��ߤޤ���
 *
 * @return     ������������0<br>
 *             ���顼�ξ���0�ʳ�����
 */
int stdstreamio::flush()
{
    if ( this->fp == c_stdin() ) this->fp = this->p_stdout;
    return this->cstreamio::flush();
}

/**
 * @brief  EOF�ؼ��Ҥμ���
 */
int stdstreamio::eof()
{
    if ( this->fp == this->p_stdout ) this->fp = c_stdin();
    return this->cstreamio::eof();
}

/**
 * @brief  ���顼�ؼ��Ҥμ���
 */
int stdstreamio::error()
{
    if ( this->fp == this->p_stdout ) this->fp = c_stdin();
    return this->cstreamio::error();
}

/**
 * @brief  EOF�ؼ��Ҥȥ��顼�ؼ��ҤΥꥻ�å�
 */
stdstreamio &stdstreamio::reseterr()
{
    if ( this->fp == this->p_stdout ) this->fp = c_stdin();
    this->cstreamio::reseterr();
    return *this;
}

/**
 * @brief  ɸ�२�顼���ϤؤΥե����ޥåȽ񤭹���
 *
 *  ɸ�२�顼���ϥ��ȥ꡼��ˡ��ե����ޥåȤ˽��äƽ񤭹��ߤޤ���
 *
 * @param      format �񤭹��ߥե����ޥåȻ���
 * @param      ... �񤭹�������ǥǡ���
 * @return     �����������Ͻ񤭹��ޤ줿ʸ����<br>
 *             ���顼�ξ�������
 */
int stdstreamio::eprintf(const char *format, ... )
{
    assert( format != NULL );

    va_list ap;
    int ret;
    va_start(ap,format);
    try {
	ret = c_vfprintf(c_stderr(),format,ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","unexpected exception");
    }
    va_end(ap);
    return ret;
}

/**
 * @brief  ɸ�२�顼���ϤؤΥե����ޥåȽ񤭹���
 *
 *  ɸ�२�顼���ϥ��ȥ꡼��ˡ��ե����ޥåȤ˽��äƽ񤭹��ߤޤ���
 *
 * @param      format �񤭹��ߥե����ޥåȻ���
 * @param      ap �񤭹��������ǥǡ���
 * @return     �����������Ͻ񤭹��ޤ줿ʸ����<br>
 *             ���顼�ξ�������
 */
int stdstreamio::veprintf( const char *format, va_list ap )
{
    assert( format != NULL );

    return c_vfprintf(c_stderr(),format,ap);
}

/**
 * @brief  ɸ�२�顼���ϥ��ȥ꡼��Υե�å���
 *
 *  ɸ�२�顼���ϥ��ȥ꡼����ΥХåե��˳�Ǽ����Ƥ��뤹�٤ƤΥǡ������
 *  ���ߤޤ���
 *
 * @return     ������������0<br>
 *             ���顼�ξ���0�ʳ�����
 */
int stdstreamio::eflush()
{
    return c_fflush(c_stderr());
}

/**
 * @brief  �ե����륵�����μ���
 *
 *  �ե�����Υ��������֤��ޤ���
 *
 * @return     �����������ϥե����륵����<br>
 *             ���顼�ξ�������
 * @note       ɸ�����Ϥ��Ф��Ƥ� 0 ���֤��ޤ���
 */
long long stdstreamio::content_length() const
{
    void *fp_r = this->fp;
    if ( fp_r == this->p_stdout ) fp_r = c_stdin();
    if ( fp_r == NULL ) return -1;
    else return get_file_size(c_fileno(fp_r));
}

}	/* namespace sli */


#include "private/c_stdout.c"
#include "private/c_stdin.c"
#include "private/c_stderr.c"
#include "private/c_fopen.c"
#include "private/c_fclose.c"
#include "private/c_vfprintf.c"
#include "private/c_fflush.c"
#include "private/c_fileno.c"
#include "private/get_file_size.c"
