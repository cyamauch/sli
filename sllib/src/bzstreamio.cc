/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2013-05-13 23:24:08 cyamauch> */

/**
 * @file   bzstreamio.cc
 * @brief  bzip2���̥��ȥ꡼��򰷤� bzstreamio ���饹�Υ�����
 */

#define CLASS_NAME "bzstreamio"

#include "config.h"

#ifdef HAVE_LIBBZ2

/* BZ2_bzopen() ����Ȥ��ȡ�eof() �ʤɤξ��󤬤Ȥ�ʤ��Τ� */
/* ���٥� API �Τߤ�Ȥ� */
#define SLI__BZ2_ONLY_USE_LOW_API 1

#include "bzstreamio.h"

#include <stdlib.h>
#include <bzlib.h>
#include <assert.h>

#include "private/err_report.h"

#include "private/c_close.h"
#include "private/c_strncmp.h"

#ifndef SLI__BZ2_ONLY_USE_LOW_API
#include "private/c_dup.h"
#endif

/* raw_read(), raw_write() �Ǥκ�����������  */
static const size_t RW_BUFFER_SIZE_UNIT = 65536;

namespace sli
{

/**
 * @brief  ���󥹥ȥ饯��
 */
bzstreamio::bzstreamio()
{
    this->bzfp = NULL;
    this->fp_for_indrw = NULL;
    this->indw_buffer_size = 0;
    return;
}

/**
 * @brief  �ǥ��ȥ饯��
 */
bzstreamio::~bzstreamio()
{
    this->close();
    return;
}

/* overridden member functions */
/**
 * @brief  �񤭹����ѥ��ȥ꡼��Υե�å���
 *
 *  open()�ǳ��������ȥ꡼��ˡ��Хåե��˳�Ǽ����Ƥ��뤹�٤ƤΥǡ�����
 *  bzip2�ե����ޥåȤǽ񤭹��ߤޤ���
 *
 * @return     ������������0<br>
 *             ���顼�ξ���0�ʳ�����
 */
int bzstreamio::flush()
{
    if ( this->ind_sp != NULL ) {
	return this->ind_sp->flush();
    }
    else if ( this->bzfp != NULL ) {
	return BZ2_bzflush(this->bzfp);
    }
    else return -1;
}

/**
 * @brief  ɸ�������ϤΥ����ץ�
 *
 *  ɸ�������Ϥ�bzip2�ե����ޥåȤΥ��ȥ꡼��Ȥ��ƥ����ץ󤷤ޤ���
 *  mode�� "r" �����ꤵ�줿����ɸ�����Ϥ�"w" �����ꤵ�줿����ɸ����Ϥ�
 *  �����ץ󤷤ޤ���
 *
 * @param      mode ���ȥ꡼��򳫤��⡼��(r or w)
 * @return     ������������0<br>
 *             ���顼�ξ�������
 * @throw      err_rec ɸ�������ϤΥե����롦�ǥ�������ץ���ʣ���˼��Ԥ������
 */
int bzstreamio::open( const char *mode )
{
    assert( mode != NULL );

    int return_status = -1;
    int fd = -1;

    if ( this->ind_sp != NULL || this->bzfp != NULL ) goto quit;

#ifdef SLI__BZ2_ONLY_USE_LOW_API
    try {
	if ( this->fp_for_indrw != NULL ) delete this->fp_for_indrw;
	this->fp_for_indrw = new stdstreamio;
    }
    catch (...) {
	this->fp_for_indrw = NULL;
	err_throw(__FUNCTION__,"FATAL","new failed");
    }
    if ( this->fp_for_indrw->open(mode) < 0 ) {
	delete this->fp_for_indrw;
	this->fp_for_indrw = NULL;
	goto quit;
    }
    if ( this->open(mode, *(this->fp_for_indrw)) < 0 ) {
	this->fp_for_indrw->close();
	delete this->fp_for_indrw;
	this->fp_for_indrw = NULL;
	goto quit;
    }
#else
    if ( c_strncmp(mode,"r",1) == 0 ) {
	fd = c_dup(0);
	if ( fd == -1 ) {
            err_throw(__FUNCTION__,"FATAL","dup() failed");
	}
    }
    else if ( c_strncmp(mode,"w",1) == 0 ) {
	fd = c_dup(1);
	if ( fd == -1 ) {
            err_throw(__FUNCTION__,"FATAL","dup() failed");
	}
    }

    this->bzfp = BZ2_bzdopen(fd,mode);
    if ( this->bzfp == NULL ) goto quit;
#endif

    return_status = 0;
 quit:
    if ( return_status != 0 && 0 <= fd ) c_close(fd);
    return return_status;
}

/**
 * @brief  �ե�����Υ����ץ�
 *
 *  �ե�����̾����ꤷ�ơ�bzip2�ե����ޥåȤΥ��ȥ꡼��򥪡��ץ󤷤ޤ���
 *
 * @param      mode ���ȥ꡼��򳫤��⡼��(r or w)
 * @param      path �ե�����̾
 * @return     ������������0<br>
 *             ���顼�ξ�������
 * @throw      err_rec ɸ�������ϤΥե����롦�ǥ�������ץ���ʣ���˼��Ԥ������
 */
int bzstreamio::open( const char *mode, const char *path )
{
    assert( mode != NULL );

    int return_status = -1;

    if ( path == NULL ) return this->open(mode);

    if ( this->ind_sp != NULL || this->bzfp != NULL ) goto quit;

    if ( (c_strncmp(mode,"r",1) != 0) && (c_strncmp(mode,"w",1) != 0) ) goto quit;

#ifdef SLI__BZ2_ONLY_USE_LOW_API
    try {
	if ( this->fp_for_indrw != NULL ) delete this->fp_for_indrw;
	this->fp_for_indrw = new stdstreamio;
    }
    catch (...) {
	this->fp_for_indrw = NULL;
	err_throw(__FUNCTION__,"FATAL","new failed");
    }
    if ( this->fp_for_indrw->open(mode,path) < 0 ) {
	delete this->fp_for_indrw;
	this->fp_for_indrw = NULL;
	goto quit;
    }
    if ( this->open(mode, *(this->fp_for_indrw)) < 0 ) {
	this->fp_for_indrw->close();
	delete this->fp_for_indrw;
	this->fp_for_indrw = NULL;
	goto quit;
    }
#else
    this->bzfp = BZ2_bzopen(path,mode);
    if ( this->bzfp == NULL ) goto quit;
#endif

    return_status = 0;
 quit:
    return return_status;
}

/**
 * @brief  �ǥ�������ץ�����ˤ�륹�ȥ꡼��Υ����ץ�
 *
 *  �ǥ�������ץ�����ꤷ�ơ�bzip2�ե����ޥåȤΥ��ȥ꡼��򥪡��ץ󤷤ޤ���
 *
 * @param   mode ���ȥ꡼��򳫤��⡼��(r or w)
 * @param   fd   �ե�����ǥ�������ץ�
 * @return  ������������0<br>
 *          ���顼�ξ�������
 */
int bzstreamio::open( const char *mode, int fd )
{
    assert( mode != NULL );

    int return_status = -1;
    int fd_new = -1;

    if ( this->ind_sp != NULL || this->bzfp != NULL ) goto quit;

    if ( fd == 0 ) {
        if ( c_strncmp(mode,"r",1) != 0 ) goto quit;
    } else if (fd == 1 || fd == 2) {
        if ( c_strncmp(mode,"w",1) != 0 ) goto quit;
    } else {
        /* DO_NOTHIG */
    }

#ifdef SLI__BZ2_ONLY_USE_LOW_API
    try {
	if ( this->fp_for_indrw != NULL ) delete this->fp_for_indrw;
	this->fp_for_indrw = new stdstreamio;
    }
    catch (...) {
	this->fp_for_indrw = NULL;
	err_throw(__FUNCTION__,"FATAL","new failed");
    }
    if ( this->fp_for_indrw->open(mode,fd) < 0 ) {
	delete this->fp_for_indrw;
	this->fp_for_indrw = NULL;
	goto quit;
    }
    if ( this->open(mode, *(this->fp_for_indrw)) < 0 ) {
	this->fp_for_indrw->close();
	delete this->fp_for_indrw;
	this->fp_for_indrw = NULL;
	goto quit;
    }
#else
    fd_new = c_dup(fd);
    this->bzfp = BZ2_bzdopen(fd_new,mode);
    if ( this->bzfp == NULL ) goto quit;
#endif

    return_status = 0;
 quit:
    if ( return_status != 0 && 0 <= fd_new ) c_close(fd_new);
    return return_status;
}

/**
 * @brief  ���ȥ꡼��Υ�����
 *
 *  open() �ǳ����� bzip2 �ե����ޥåȤΥ��ȥ꡼��򥯥������ޤ���
 *
 * @return     ������������0<br>
 *             ���顼�ξ���0�ʳ�����
 */
int bzstreamio::close()
{
    int status = 0;

    if ( this->bzfp != NULL ) {
	BZ2_bzclose(this->bzfp);
	this->bzfp = NULL;
    }

    if ( this->indr_bzptr.ptr() != NULL ) {
	bz_stream *bzsp = (bz_stream *)(this->indr_bzptr.ptr());
	BZ2_bzDecompressEnd(bzsp);
	this->indr_bzptr = NULL;
    }
    this->indr_buffer = NULL;

    /* */
    if ( this->indw_bzptr.ptr() != NULL ) {
	bz_stream *bzsp = (bz_stream *)(this->indw_bzptr.ptr());

	if ( this->indw_buffer.ptr() != NULL ) {
	    ssize_t sz;
	    bool done = false;
	    do {
		int zstatus;
		if ( bzsp->avail_out < this->indw_buffer_size ) {
		    sz = this->ind_sp->write(this->indw_buffer.ptr(),
				     this->indw_buffer_size - bzsp->avail_out);
		    if ( sz < 0 ) {
			status = EOF;
			this->errflg_rec = 1;
			err_report(__FUNCTION__,"ERROR","ind_sp->write() failed");
			goto quit0;
		    }
		}
		if ( done != false ) break;
		/* */
		bzsp->next_out = this->indw_buffer.ptr();
		bzsp->avail_out = this->indw_buffer_size;
		/* */
		zstatus = BZ2_bzCompress(bzsp,BZ_FINISH);
		if ( zstatus == BZ_STREAM_END ) {
		    this->eofflg_rec = 1;
		    done = true;
		}
		else if ( bzsp->avail_out != 0 ) {
		    done = true;
		}
		else if ( zstatus != BZ_FINISH_OK ) {
		    status = EOF;
		    this->errflg_rec = zstatus;
		    err_report(__FUNCTION__,"ERROR","BZ2_bzCompress() failed");
		    goto quit0;
		}
	    } while ( 1 );

	    this->ind_sp->flush();
	}
    quit0:
	BZ2_bzCompressEnd(bzsp);
	this->indw_bzptr = NULL;
    }
    this->indw_buffer = NULL;
    this->indw_buffer_size = 0;

    this->ind_sp = NULL;

    if ( this->fp_for_indrw != NULL ) {
	int fp_status;
	fp_status = this->fp_for_indrw->close();
	delete this->fp_for_indrw;
	this->fp_for_indrw = NULL;
	if ( status == 0 && fp_status != 0 ) status = fp_status;
    }

    if ( status == 0 ) {
	this->eofflg_rec = 0;
	this->errflg_rec = 0;
    }

    this->cstreamio::close();

    return status;
}

/*
 * ����:
 *       bzlib �� bzlib.c
 */
/**
 * @brief  cstreamio�ηѾ����饹�Υ��ȥ꡼����Ф��륪���ץ�
 *
 *  ������Υ��ȥ꡼��򥪡��ץ󤷤Ƥ���cstreamio���饹�ηѾ����饹�Υ��֥�����
 *  �Ȥ���ꤷ�ơ�bzip2�ե����ޥåȤΥ��ȥ꡼��򳫤��ޤ���
 *
 * @param      mode ���ȥ꡼��򳫤��⡼��(r or w)
 * @param      sref cstreamio���饹�ηѾ����饹�Υ��󥹥���
 * @return     ������������0<br>
 *             ���顼�ξ�������
 */
int bzstreamio::open( const char *mode, cstreamio &sref )
{
    assert( mode != NULL );

    int return_status = -1;
    bz_stream *bzsp;

    /* err_report(__FUNCTION__,"DEBUG","indirect mode"); */

    if ( this->ind_sp != NULL || this->bzfp != NULL ) goto quit;
    if ( this->indr_bzptr.ptr() != NULL ) goto quit;
    if ( this->indw_bzptr.ptr() != NULL ) goto quit;

    if ( c_strncmp(mode,"w",1) == 0 ) {

	if ( this->indw_bzptr.allocate(sizeof(bz_stream)) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","malloc() failed");
	}
	this->indw_bzptr.fill(0, sizeof(bz_stream));
	bzsp = (bz_stream *)(this->indw_bzptr.ptr());

	/* bzlib �ǥ����������� */
	bzsp->opaque = NULL;
	bzsp->bzalloc = NULL;
	bzsp->bzfree = NULL;
	/* ��������� */
	bzsp->avail_in = 0;
	bzsp->next_in = NULL;
	bzsp->avail_out = 0;
	bzsp->next_out = NULL;

	if ( BZ2_bzCompressInit ( bzsp, 9, 0, 30 ) != BZ_OK ) {
	    err_throw(__FUNCTION__,"FATAL","BZ2_bzCompressInit() failed");
	}

	/* ���ϥХåե���Ŭ���ʥ������Ǥ�ꤢ�ƤƤ��� */
	if ( this->indw_buffer.ptr() == NULL ) {
	    if ( this->indw_buffer.allocate(4096) < 0 ) {
		err_throw(__FUNCTION__,"FATAL","malloc() failed");
	    }
	    this->indw_buffer_size = 4096;
	    bzsp->next_out = this->indw_buffer.ptr();
	    bzsp->avail_out = this->indw_buffer_size;
	}
    }
    else if ( c_strncmp(mode,"r",1) == 0 ) {

	if ( this->indr_bzptr.allocate(sizeof(bz_stream)) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","malloc() failed");
	}
	this->indr_bzptr.fill(0, sizeof(bz_stream));
	bzsp = (bz_stream *)(this->indr_bzptr.ptr());
	
	/* bzlib �ǥ����������� */
	bzsp->opaque = NULL;
	bzsp->bzalloc = NULL;
	bzsp->bzfree = NULL;
	/* ��������� */
	bzsp->avail_in = 0;
	bzsp->next_in = NULL;
	bzsp->avail_out = 0;
	bzsp->next_out = NULL;

	if ( BZ2_bzDecompressInit ( bzsp, 0, 0 ) != BZ_OK ) {
	    err_throw(__FUNCTION__,"FATAL","BZ2_bzDecompressInit() failed");
	}
    }
    else {
        err_report1(__FUNCTION__,"ERROR","unsupported mode: `%s'",mode);
        goto quit;
    }

    this->ind_sp = &sref;

    this->eofflg_rec = 0;
    this->errflg_rec = 0;

    return_status = 0;
 quit:
    return return_status;
}

/**
 * @brief  EOF�ؼ��Ҥμ���
 */
int bzstreamio::eof()
{
    return this->cstreamio::eof();
}

/**
 * @brief  ���顼�ؼ��Ҥμ���
 */
int bzstreamio::error()
{
    return this->cstreamio::error();
}

/**
 * @brief  EOF�ؼ��Ҥȥ��顼�ؼ��ҤΥꥻ�å�
 */
bzstreamio &bzstreamio::reseterr()
{
    this->cstreamio::reseterr();
    return *this;
}

/**
 * @brief  �ɤ߹����Ѹ��ϴؿ��Υ���å� (���Υ��饹�ˤƥ����Х饤��)
 *
 * @param    buf �ǡ����γ�Ǽ���
 * @param    size �����Υǡ����ΥХ��ȿ�
 * @return   �������������ɤ߹�����ǡ����β����ΥХ��ȿ�<br>
 *           ���ȥ꡼�ब����ξ�硤0�Х��Ȥ��ɤ߹��ߤ����ꤵ�줿����0<br>
 *           ���顼�ξ�������
 * @note     ��������󤹤뤿�ᡤsize ���礭������ _raw_read() �ؿ���
 *           �����ʬ���ƸƤӽФ��ޤ���<br>
 *           ���Υ��дؿ��� protected �Ǥ���
 */
ssize_t bzstreamio::raw_read( void *buf, size_t size )
{
    char *buf_ptr = (char *)buf;
    size_t cnt = size / RW_BUFFER_SIZE_UNIT;
    size_t left = size - RW_BUFFER_SIZE_UNIT * cnt;
    size_t i;
    ssize_t ret = 0;

    if ( buf == NULL && 0 < size ) return -1;

    /* save the memory cost */
    for ( i=0 ; i < cnt ; i++ ) {
	ssize_t s = this->_raw_read(buf_ptr, RW_BUFFER_SIZE_UNIT);
	if ( s < 0 ) {
	    if ( ret == 0 ) ret = s;
	    break;
	}
	else {
	    ret += s;
	    if ( s < (ssize_t)RW_BUFFER_SIZE_UNIT ) break;
	    buf_ptr += RW_BUFFER_SIZE_UNIT;
	}
    }
    if ( i == cnt ) {
	ssize_t s = this->_raw_read(buf_ptr, left);
	if ( s < 0 ) {
	    if ( ret == 0 ) ret = s;
	}
	else {
	    ret += s;
	}
    }
    return ret;
}

/**
 * @brief  ���٥���ɤ߹����Ѹ��ϴؿ�
 *
 *  indirect_read() ���дؿ��ޤ��� BZ2_bzread() �ؿ���ƤӽФ���bzip2�ե���
 *  �ޥåȤΥ��ȥ꡼�फ����� size �Х��ȤΥǡ������ɤߡ��Хåե��˽񤭹���
 *  �ޤ���
 *
 * @param    buf �ǡ����γ�Ǽ���
 * @param    size �����Υǡ����ΥХ��ȿ�
 * @return   �������������ɤ߹�����ǡ����β����ΥХ��ȿ�<br>
 *           ���ȥ꡼�ब����ξ�硤0�Х��Ȥ��ɤ߹��ߤ����ꤵ�줿����0<br>
 *           ���顼�ξ�������
 * @note     ���Υ��дؿ��� private �Ǥ���
 */
ssize_t bzstreamio::_raw_read( void *buf, size_t size )
{
    if ( this->ind_sp != NULL ) {
	return this->indirect_read(buf,size);
    }
    if ( this->bzfp == NULL ) return -1;
    if ( buf == NULL && size == 0 ) return 0;
    return BZ2_bzread( this->bzfp, buf, size );
}

/**
 * @brief  �񤭹����Ѹ��ϴؿ��Υ���å� (���Υ��饹�ˤƥ����Х饤��)
 *
 * @param      buf �ǡ����γ�Ǽ���
 * @param      size �ǡ����ΰ������ΥХ��ȿ�
 * @return     �����������ϰ������ΥХ��ȿ�<br>
 *             ���顼�ξ�������
 * @note       ��������󤹤뤿�ᡤsize ���礭������ _raw_write() �ؿ���
 *             �����ʬ���ƸƤӽФ��ޤ���<br>
 *             ���Υ��дؿ��� protected �Ǥ���
 */
ssize_t bzstreamio::raw_write( const void *buf, size_t size )
{
    const char *buf_ptr = (const char *)buf;
    size_t cnt = size / RW_BUFFER_SIZE_UNIT;
    size_t left = size - RW_BUFFER_SIZE_UNIT * cnt;
    size_t i;
    ssize_t ret = 0;

    if ( buf == NULL && 0 < size ) return -1;

    /* save the memory cost */
    for ( i=0 ; i < cnt ; i++ ) {
	ssize_t s = this->_raw_write(buf_ptr, RW_BUFFER_SIZE_UNIT);
	if ( s < 0 ) {
	    if ( ret == 0 ) ret = s;
	    break;
	}
	else {
	    ret += s;
	    if ( s < (ssize_t)RW_BUFFER_SIZE_UNIT ) break;
	    buf_ptr += RW_BUFFER_SIZE_UNIT;
	}
    }
    if ( i == cnt ) {
	ssize_t s = this->_raw_write(buf_ptr, left);
	if ( s < 0 ) {
	    if ( ret == 0 ) ret = s;
	}
	else {
	    ret += s;
	}
    }
    return ret;
}

/**
 * @brief  ���٥�ʽ񤭹����Ѹ��ϴؿ�
 *
 *  indirect_write() ���дؿ��� BZ2_bzwrite() �ؿ���ƤӽФ���Ϳ����줿�Х�
 *  �ե�������� size �Х��ȤΥǡ������ɤ߹��ߡ����߳�����Ƥ��륹�ȥ꡼��˽�
 *  �����ߤޤ���
 *
 * @param      buf �ǡ����γ�Ǽ���
 * @param      size �ǡ����ΰ������ΥХ��ȿ�
 * @return     �����������ϰ������ΥХ��ȿ�<br>
 *             ���顼�ξ�������
 * @note       ���Υ��дؿ��� private �Ǥ���
 */
ssize_t bzstreamio::_raw_write( const void *buf, size_t size )
{
    if ( this->ind_sp != NULL ) {
	return this->indirect_write(buf,size);
    }
    if ( this->bzfp == NULL ) return -1;
    if ( buf == NULL && size == 0 ) return 0;
    return BZ2_bzwrite( this->bzfp, (void *)buf, size );
}

/**
 * @brief  Ķ���٥���ɤ߹����Ѹ��ϴؿ� (����read)
 *
 *  cstreamio �ηѾ����饹�ǥ����ץ󤵤줿���ȥ꡼����Ф��ơ��ܥ��饹�Ǥ����
 *  �����ץ󤷤����˻Ȥ��ޤ���
 *
 * @param    buf �ǡ����γ�Ǽ���
 * @param    size �����Υǡ����ΥХ��ȿ�
 * @return   �������������ɤ߹�����ǡ����β����ΥХ��ȿ�<br>
 *           ���ȥ꡼�ब����ξ�硤0�Х��Ȥ��ɤ߹��ߤ����ꤵ�줿����0<br>
 *           ���顼�ξ�������
 * @note     ���Υ��дؿ��� private �Ǥ���
 */
ssize_t bzstreamio::indirect_read( void *buf, size_t size )
{
    ssize_t return_len = -1;
    int zstatus = BZ_OK;
    bz_stream *bzsp = (bz_stream *)(this->indr_bzptr.ptr());

    if ( buf == NULL && 0 < size ) goto quit;
    if ( size == 0 ) return 0;
    if ( bzsp == NULL ) {
	if ( this->ind_sp != NULL ) return 0;
	goto quit;
    }

    bzsp->next_out = (char *)buf;	/* ���ϥХåե��򥻥å� */
    bzsp->avail_out = size;

    do {
        if ( bzsp->avail_in == 0 ) {	/* �ɤ߽���ä���� */
	    ssize_t sz;
	    if ( this->indr_buffer.reallocate(2 * size) < 0 ) {
		this->errflg_rec = 1;
		err_throw(__FUNCTION__,"FATAL","realloc() failed");
	    }
            bzsp->next_in = this->indr_buffer.ptr();
	    sz = this->ind_sp->read(this->indr_buffer.ptr(), 2 * size);
	    if ( sz < 0 ) {
		this->errflg_rec = 1;
		err_report(__FUNCTION__,"ERROR","ind_sp->read() failed");
		goto quit;
	    }
	    else if ( sz == 0 ) {
		this->errflg_rec = 1;
		err_report(__FUNCTION__,"ERROR","unexpected EOF");
		goto quit;
	    }
            bzsp->avail_in = sz;
        }
        zstatus = BZ2_bzDecompress(bzsp);
        if ( zstatus == BZ_STREAM_END ) {
	    this->eofflg_rec = 1;
	    break;
	}
        if ( zstatus != BZ_OK ) {
	    this->errflg_rec = zstatus;
	    err_report(__FUNCTION__,"ERROR","BZ2_bzDecompress() failed");
	    goto quit;
        }
    } while ( 0 < bzsp->avail_out );

    return_len = (ssize_t)size - bzsp->avail_out;

    if ( zstatus == BZ_STREAM_END ) {
	/* ����� */
	BZ2_bzDecompressEnd(bzsp);
	this->indr_buffer = NULL;
	this->indr_bzptr = NULL;
    }

 quit:
    return return_len;
}

/**
 * @brief  Ķ���٥�ʽ񤭹����Ѹ��ϴؿ� (����write)
 *
 *  cstreamio �ηѾ����饹�ǥ����ץ󤵤줿���ȥ꡼����Ф��ơ��ܥ��饹�Ǥ����
 *  �����ץ󤷤����˻Ȥ��ޤ���
 *
 * @param      buf �ǡ����γ�Ǽ���
 * @param      size �ǡ����ΰ������ΥХ��ȿ�
 * @return     �����������ϰ������ΥХ��ȿ�<br>
 *             ���顼�ξ�������
 * @note       ���Υ��дؿ��� private �Ǥ���
 */
ssize_t bzstreamio::indirect_write( const void *buf, size_t size )
{
    ssize_t return_len = -1;
    int zstatus = BZ_RUN_OK;
    bz_stream *bzsp = (bz_stream *)(this->indw_bzptr.ptr());

    if ( buf == NULL && 0 < size ) goto quit;
    if ( size == 0 ) return 0;
    if ( bzsp == NULL ) {
	goto quit;
    }

    bzsp->next_in = (char *)buf;	/* ���ϥХåե��򥻥å� */
    bzsp->avail_in = size;

    while ( 0 < bzsp->avail_in ) {	/* ���Ϥ����񤵤��ޤ� */
        //this->indw_in += zsp->avail_in;
	zstatus = BZ2_bzCompress(bzsp,BZ_RUN);
        //this->indw_in -= zsp->avail_in;
        if ( zstatus != BZ_RUN_OK ) {
	    this->errflg_rec = zstatus;
	    err_report(__FUNCTION__,"ERROR","BZ2_bzCompress() failed");
	    goto quit;
        }
        if ( bzsp->avail_out == 0 ) {	/* ��������� */
	    ssize_t sz;
	    /* �񤭽Ф� */
	    sz = this->ind_sp->write(this->indw_buffer.ptr(),
				     this->indw_buffer_size);
	    if ( sz < 0 ) {
		this->errflg_rec = 1;
		err_report(__FUNCTION__,"ERROR","ind_sp->write() failed");
		goto quit;
	    }
	    /* */
	    if ( this->indw_buffer.reallocate(2 * size) < 0 ) {
		this->errflg_rec = 1;
		err_throw(__FUNCTION__,"FATAL","realloc() failed");
	    }
	    this->indw_buffer_size = 2 * size;
	    /* */
            bzsp->next_out = this->indw_buffer.ptr();
            bzsp->avail_out = this->indw_buffer_size;
        }
    }


    return_len = size;
 quit:
    return return_len;
}

}	/* namespace sli */


#include "private/c_close.c"
#include "private/c_strncmp.cc"

#ifndef SLI__BZ2_ONLY_USE_LOW_API
#include "private/c_dup.c"
#endif

#endif	/* #ifdef HAVE_LIBBZ2 */
