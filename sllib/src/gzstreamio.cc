/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2013-05-13 23:19:41 cyamauch> */

/**
 * @file   gzstreamio.cc
 * @brief  gzip���̥��ȥ꡼��򰷤� gzstreamio ���饹�Υ�����
 */

#define CLASS_NAME "gzstreamio"

#include "config.h"

#ifdef HAVE_LIBZ

#include "gzstreamio.h"

#include <stdlib.h>
#include <zlib.h>
#include <zconf.h>
#include <assert.h>

/* gzip flag byte */
#define ASCII_FLAG   0x01 /* bit 0 set: file probably ascii text */
#define HEAD_CRC     0x02 /* bit 1 set: header CRC present */
#define EXTRA_FIELD  0x04 /* bit 2 set: extra field present */
#define ORIG_NAME    0x08 /* bit 3 set: original file name present */
#define COMMENT      0x10 /* bit 4 set: file comment present */
#define RESERVED     0xE0 /* bits 5..7: reserved */

#if MAX_MEM_LEVEL >= 8
#  define DEF_MEM_LEVEL 8
#else
#  define DEF_MEM_LEVEL MAX_MEM_LEVEL
#endif

#define OS_CODE  0x03 /* UNIX */

#include "private/err_report.h"

#include "private/c_close.h"
#include "private/c_dup.h"
#include "private/c_strncmp.h"

/* raw_read(), raw_write() �Ǥκ�����������  */
static const size_t RW_BUFFER_SIZE_UNIT = 65536;

namespace sli
{

/**
 * @brief  ���󥹥ȥ饯��
 */
gzstreamio::gzstreamio()
{
    this->gzfp = NULL;
    this->indw_buffer_size = 0;
    this->indw_crc = 0;
    this->indw_in = 0;
    return;
}

/**
 * @brief  �ǥ��ȥ饯��
 */
gzstreamio::~gzstreamio()
{
    this->close();
    return;
}

/*
  [citation from zlib.h]
  All output is flushed as with sync(), and the compression state is
  reset so that decompression can restart from this point if previous 
  compressed data has been damaged or if random access is desired. Using 
  flush() too often can seriously degrade the compression.
 */
/**
 * @brief  �񤭹����ѥ��ȥ꡼��Υե�å���
 *
 *  open()�ǳ��������ȥ꡼��ˡ��Хåե��˳�Ǽ����Ƥ��뤹�٤ƤΥǡ�����
 *  gzip�ե����ޥåȤǽ񤭹��ߤޤ���
 *
 * @return     ������������0<br>
 *             ���顼�ξ���0�ʳ�����
 * @attention  ���ߤ˻Ȥ��Ȱ���Ψ�ΰ����򾷤��ޤ���
 */
int gzstreamio::flush()
{
    if ( this->ind_sp != NULL )
	return this->ind_sp->flush();
    else if ( this->gzfp != NULL )
	return gzflush((gzFile)(this->gzfp), Z_FULL_FLUSH);
    else return -1;
}

/**
 * @brief  ɸ�������ϤΥ����ץ�
 *
 *  ɸ�������Ϥ�gzip�ե����ޥåȤΥ��ȥ꡼��Ȥ��ƥ����ץ󤷤ޤ���<br>
 *  mode �� "r" �����ꤵ�줿����ɸ�����Ϥ�"w" �����ꤵ�줿����ɸ����Ϥ�
 *  �����ץ󤷤ޤ���
 *
 * @param      mode ���ȥ꡼��򳫤��⡼��(r or w)�Ȱ��̥⡼�� 
 * @return     ������������0<br>
 *             ���顼�ξ�������
 * @throw      err_rec ɸ�������ϤΥե����롦�ǥ�������ץ���ʣ���˼��Ԥ������
 */
int gzstreamio::open( const char *mode )
{
    assert( mode != NULL );

    int return_status = -1;
    int fd = -1;
    if ( this->ind_sp != NULL || this->gzfp != NULL ) goto quit;
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
    this->gzfp = gzdopen(fd,mode);
    if ( this->gzfp == NULL ) goto quit;

    return_status = 0;
 quit:
    if ( return_status != 0 && 0 <= fd ) c_close(fd);
    return return_status;
}

/**
 * @brief  �ե�����Υ����ץ�
 *
 *  �ե�����̾����ꤷ�ơ�gzip�ե����ޥåȤΥ��ȥ꡼��򥪡��ץ󤷤ޤ���
 *
 * @param      mode ���ȥ꡼��򳫤��⡼��(r or w)�Ȱ��̥⡼��
 * @param      path �ե�����̾
 * @return     ������������0<br>
 *             ���顼�ξ�������
 * @throw      err_rec ɸ�������ϤΥե����롦�ǥ�������ץ���ʣ���˼��Ԥ������
 */
int gzstreamio::open( const char *mode, const char *path )
{
    assert( mode != NULL );

    int return_status = -1;

    if ( path == NULL ) return this->open(mode);

    if ( this->ind_sp != NULL || this->gzfp != NULL ) goto quit;
    this->gzfp = gzopen(path,mode);
    if ( this->gzfp == NULL ) goto quit;

    return_status = 0;
 quit:
    return return_status;
}

/**
 * @brief  �ǥ�������ץ�����ˤ�륹�ȥ꡼��Υ����ץ�
 *
 *  �ǥ�������ץ�����ꤷ�ơ�gzip�ե����ޥåȤΥ��ȥ꡼��򥪡��ץ󤷤ޤ���
 *
 * @param       mode ���ȥ꡼��򳫤��⡼��(r or w)�Ȱ��̥⡼��
 * @param       fd   �ե�����ǥ�������ץ�
 * @return      ������������0<br>
 *              ���顼�ξ�������
 */
int gzstreamio::open( const char *mode, int fd )
{
    assert( mode != NULL );

    int return_status = -1;
    int fd_new = -1;

    if ( this->ind_sp != NULL || this->gzfp != NULL ) goto quit;

    if ( fd == 0 ) {
        if ( c_strncmp(mode,"r",1) != 0 ) goto quit;
    } else if (fd == 1 || fd == 2) {
        if ( c_strncmp(mode,"w",1) != 0 ) goto quit;
    } else {
        /* DO_NOTHIG */
    } 
    fd_new = c_dup(fd);
    this->gzfp = gzdopen(fd_new,mode);
    if ( this->gzfp == NULL ) goto quit;

    return_status = 0;
 quit:
    if ( return_status != 0 && 0 <= fd_new ) c_close(fd_new);
    return return_status;
}

/**
 * @brief  ��ȥ륨��ǥ�����Ǥ� 4 �Х��������ν񤭹��� (������)
 *
 *  ���ꤵ�줿���ȥ꡼����Ф��� 4 �Х��Ȥ��������ǡ����򡤽����Ϥ˰�¸����
 *  ���Ʊ���Х��ȥ������ǽ񤭹��ߤޤ���
 *
 * @param       sp cstreamio���饹�ηѾ����饹�Υ��󥹥���
 * @param       val ���ʤ�long���ǡ���
 */
static void put_ulong32( cstreamio *sp, unsigned long val )
{
    assert( sp != NULL );

    int i;
    for ( i = 0 ; i < 4 ; i++ ) {
        sp->putchr((int)(val & 0x00ff));
        val >>= 8;
    }
    return;
}

/**
 * @brief  ���ȥ꡼��Υ�����
 *
 *  open() �ǳ����� gzip �ե����ޥåȤΥ��ȥ꡼��򥯥������ޤ���
 *
 * @return     ������������0<br>
 *             ���顼�ξ���0�ʳ�����
 */
int gzstreamio::close()
{
    int status = 0;

    if ( this->gzfp != NULL ) {
	status = gzclose((gzFile)(this->gzfp));
	this->gzfp = NULL;
    }

    if ( this->indr_zptr.ptr() != NULL ) {
	int zstatus;
	z_stream *zsp = (z_stream *)(this->indr_zptr.ptr());
	if ( (zstatus = inflateEnd(zsp)) != Z_OK ) {
	    status = EOF;
	    this->errflg_rec = zstatus;
	    err_report1(__FUNCTION__,"ERROR","inflateEnd() failed, msg: %s",
			zsp->msg);
	}
	this->indr_zptr = NULL;
    }
    this->indr_buffer = NULL;

    /* */
    if ( this->indw_zptr.ptr() != NULL ) {
	int zstatus;
	z_stream *zsp = (z_stream *)(this->indw_zptr.ptr());

	if ( this->indw_buffer.ptr() != NULL ) {
	    ssize_t sz;
	    bool done = false;
	    do {
		if ( zsp->avail_out < this->indw_buffer_size ) {
		    sz = this->ind_sp->write(this->indw_buffer.ptr(),
				      this->indw_buffer_size - zsp->avail_out);
		    if ( sz < 0 ) {
			status = EOF;
			this->errflg_rec = 1;
			err_report(__FUNCTION__,"ERROR","ind_sp->write() failed");
			goto quit0;
		    }
		}
		if ( done != false ) break;
		/* */
		zsp->next_out = (Bytef *)(this->indw_buffer.ptr());
		zsp->avail_out = this->indw_buffer_size;
		/* */
		zstatus = deflate(zsp,Z_FINISH);
		if ( zstatus == Z_STREAM_END ) {
		    this->eofflg_rec = 1;
		    done = true;
		}
		else if ( zsp->avail_out != 0 ) {
		    done = true;
		}
		else if ( zstatus != Z_OK ) {
		    status = EOF;
		    this->errflg_rec = zstatus;
		    err_report1(__FUNCTION__,"ERROR","deflate() failed, msg: %s",
				zsp->msg);
		    goto quit0;
		}
	    } while ( 1 );
	    /* CRC �Ȥ������ */
	    put_ulong32(this->ind_sp,this->indw_crc);
	    put_ulong32(this->ind_sp,
			(unsigned long)(this->indw_in & 0xffffffff));
	    this->ind_sp->flush();
	}
    quit0:
	if ( (zstatus = deflateEnd(zsp)) != Z_OK ) {
	    status = EOF;
	    this->errflg_rec = zstatus;
	    err_report1(__FUNCTION__,"ERROR","deflateEnd() failed, msg: %s",
			zsp->msg);
	}
	this->indw_zptr = NULL;
    }
    this->indw_buffer = NULL;
    this->indw_buffer_size = 0;
    this->indw_crc = 0;
    this->indw_in = 0;

    this->ind_sp = NULL;

    if ( status == 0 ) {
	this->eofflg_rec = 0;
	this->errflg_rec = 0;
    }

    this->cstreamio::close();

    return status;
}

/*
 * ����:
 *       zlib �� gzio.c
 *       ���Τؤ� �� http://www.limo.fumi2kick.com/tips/rrda/rrda01.html
 *       ���Τؤ� �� http://oku.edu.mie-u.ac.jp/~okumura/compression/zlib.html
 */
/**
 * @brief  cstreamio�ηѾ����饹�Υ��ȥ꡼����Ф��륪���ץ�
 *
 *  ������Υ��ȥ꡼��򥪡��ץ󤷤Ƥ���cstreamio���饹�ηѾ����饹�Υ��֥�����
 *  �Ȥ���ꤷ�ơ�gzip�ե����ޥåȤΥ��ȥ꡼��򳫤��ޤ���
 *
 * @param      mode ���ȥ꡼��򳫤��⡼��(r or w)�Ȱ��̥⡼��
 * @param      sref cstreamio���饹�ηѾ����饹�Υ��󥹥���
 * @return     ������������0<br>
 *             ���顼�ξ�������
 */
int gzstreamio::open( const char *mode, cstreamio &sref )
{
    int return_status = -1;
    z_stream *zsp;

    if ( this->ind_sp != NULL || this->gzfp != NULL ) goto quit;
    if ( this->indr_zptr.ptr() != NULL ) goto quit;
    if ( this->indw_zptr.ptr() != NULL ) goto quit;

    if ( c_strncmp(mode,"w",1) == 0 ) {

	if ( this->indw_zptr.allocate(sizeof(z_stream)) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","malloc() failed");
	}
	this->indw_zptr.fill(0,sizeof(z_stream));
	zsp = (z_stream *)(this->indw_zptr.ptr());
	
	/* zlib �ǥ����������� */
	zsp->zalloc = (alloc_func)0;
	zsp->zfree = (free_func)0; 
	zsp->opaque = (voidpf)0;
	/* ����� */
	zsp->avail_in = 0;
	zsp->next_in = Z_NULL;
	zsp->avail_out = 0;
	zsp->next_out = Z_NULL;

	/* ����� */
        if ( deflateInit2(zsp, Z_DEFAULT_COMPRESSION,
			  Z_DEFLATED, -MAX_WBITS, DEF_MEM_LEVEL, 
			  Z_DEFAULT_STRATEGY) != Z_OK ) {
	    err_throw(__FUNCTION__,"FATAL","deflateInit2() failed");
	}

	/* ���ϥХåե���Ŭ���ʥ������Ǥ�ꤢ�ƤƤ��� */
	if ( this->indw_buffer.ptr() == NULL ) {
	    if ( this->indw_buffer.allocate(4096) < 0 ) {
		err_throw(__FUNCTION__,"FATAL","malloc() failed");
	    }
	    this->indw_buffer_size = 4096;
	    zsp->next_out = (Bytef *)(this->indw_buffer.ptr());
	    zsp->avail_out = this->indw_buffer_size;
	}

        sref.printf("%c%c%c%c%c%c%c%c%c%c", 0x01f, 0x08b,
		    Z_DEFLATED, 0 /* flags */, 
		    0,0,0,0 /* time */, 0 /*xflags*/, 
		    OS_CODE);

	this->indw_crc = crc32(0L, Z_NULL, 0);
	this->indw_in = 0;
    }
    else if ( c_strncmp(mode,"r",1) == 0 ) {
	int c0 = EOF,c1 = EOF;

	if ( this->indr_zptr.allocate(sizeof(z_stream)) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","malloc() failed");
	}
	this->indr_zptr.fill(0,sizeof(z_stream));
	zsp = (z_stream *)(this->indr_zptr.ptr());
	
	/* zlib �ǥ����������� */
	zsp->zalloc = (alloc_func)0;
	zsp->zfree = (free_func)0; 
	zsp->opaque = (voidpf)0;
	/* ����� */
	zsp->avail_in = 0;
	zsp->next_in = Z_NULL;
	zsp->avail_out = 0;
	zsp->next_out = Z_NULL;

	/* ����� */
	if ( inflateInit2(zsp,-MAX_WBITS) != Z_OK ) {
	    err_throw(__FUNCTION__,"FATAL","inflateInit() failed");
	}

	/* magic number ������å� */
	c0 = sref.getchr();
	if ( c0 != EOF ) {
	    c1 = sref.getchr();
	}
	if ( c0 != 0x01f || c1 != 0x08b ) {	/* .gz �Ǥʤ���� */
	    if ( this->indr_buffer.reallocate(2) < 0 ) {
		err_throw(__FUNCTION__,"FATAL","realloc() failed");
	    }
	    if ( c0 != EOF && c1 != EOF ) {
		zsp->next_in = (Bytef *)(this->indr_buffer.ptr());
		zsp->avail_in = 2;
		this->indr_buffer[0] = c0;
		this->indr_buffer[1] = c1;
	    }
	    else if ( c0 != EOF && c1 == EOF ) {
		zsp->next_in = (Bytef *)(this->indr_buffer.ptr());
		zsp->avail_in = 1;
		this->indr_buffer[0] = c0;
	    }
	}
	else {					/* .gz �ξ�� */
	    int method = 0, flags = 0, i, c = 0;
	    unsigned int len;
	    c = sref.getchr();
	    if ( c != EOF ) {
		method = c;
		c = sref.getchr();
		if ( c != EOF ) {
		    flags = c;
		    if (method != Z_DEFLATED || (flags & RESERVED) != 0) {
			err_report(__FUNCTION__,"ERROR","data error");
			goto quit;
		    }
		}
	    }
	    /* Discard time, xflags and OS code: */
	    for ( i=0 ; i < 6 && c != EOF ; i++ ) c = sref.getchr();
	    /* skip the extra field */
	    if ( c != EOF && (flags & EXTRA_FIELD) != 0 ) {
		c = sref.getchr();
		if ( c != EOF ) {
		    len  =  (unsigned int)c;
		    c = sref.getchr();
		    if ( c != EOF ) {
			len += ((unsigned int)c)<<8;
			/* len is garbage if EOF but the loop below will quit 
			   anyway */
			while ( len-- != 0 && (c = sref.getchr()) != EOF ) ;
		    }
		}
	    }
	    /* skip the original file name */
	    if ( c != EOF && (flags & ORIG_NAME) != 0 ) {
		while ( (c = sref.getchr()) != 0 && c != EOF ) ;
	    }
	    /* skip the .gz file comment */
	    if ( c != EOF && (flags & COMMENT) != 0 ) {
		while ( (c = sref.getchr()) != 0 && c != EOF) ;
	    }
	    /* skip the header crc */
	    if ( c != EOF && (flags & HEAD_CRC) != 0 ) {
		for ( i = 0 ; i < 2 && c != EOF ; i++ ) c = sref.getchr();
	    }
	    if ( c == EOF ) {
		err_report(__FUNCTION__,"ERROR","unexpected EOF");
		goto quit;
	    }
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
int gzstreamio::eof()
{
    if ( this->gzfp != NULL ) return gzeof((gzFile)(this->gzfp));
    else return this->cstreamio::eof();
}

/**
 * @brief  ���顼�ؼ��Ҥμ���
 */
int gzstreamio::error()
{
    if ( this->gzfp != NULL ) {
	int errnum = 0;
	const char *msg;
	msg = gzerror((gzFile)(this->gzfp), &errnum);
	// err_report1(__FUNCTION__,"DEBUG","msg: `%s'",msg);
	if ( errnum < 0 ) return errnum;
	else return 0;
    }
    else return this->cstreamio::error();
}

/**
 * @brief  EOF�ؼ��Ҥȥ��顼�ؼ��ҤΥꥻ�å�
 */
gzstreamio &gzstreamio::reseterr()
{
    if ( this->gzfp != NULL ) gzclearerr((gzFile)(this->gzfp));
    else this->cstreamio::reseterr();
    return *this;
}


/*
  [citation from zlib.h]
  All pending output is flushed to the output buffer and the output is aligned
  on a byte boundary, so that the decompressor can get all input data available
  so far.  Flushing may degrade compression for some compression algorithms and
  so it should be used only when necessary.
*/
/**
 * @brief  �񤭹����ѥ��ȥ꡼��Υե�å��� (�Х��ȶ����ǥ��饤��)
 *
 *  open()�ǳ��������ȥ꡼��ˡ��Хåե��˳�Ǽ����Ƥ��뤹�٤ƤΥǡ�����
 *  gzip�ե����ޥåȤǽ񤭹��ߤޤ���
 *
 * @return     ������������0<br>
 *             ���顼�ξ���0�ʳ�����
 * @attention  ���ߤ˻Ȥ��Ȱ���Ψ�ΰ����򾷤��ޤ���
 */
int gzstreamio::sync()
{
    if ( this->gzfp != NULL )
	return gzflush((gzFile)(this->gzfp), Z_SYNC_FLUSH);
    else if ( this->ind_sp != NULL )
	return this->ind_sp->flush();
    else return -1;

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
/* cstreamio �˼������٤����⤷��ʤ�����cstreamio ��ʣ��������Τȡ� */
/* �Ѿ����饹¦�μ�ͳ�٤�ͤ����Ѿ����饹¦�Ǽ�������                 */
ssize_t gzstreamio::raw_read( void *buf, size_t size )
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
 *  indirect_read() ���дؿ��ޤ��� gzread() �ؿ���ƤӽФ���gzip �ե����ޥå�
 *  �Υ��ȥ꡼�फ����� size �Х��ȤΥǡ������ɤߡ��Хåե��˽񤭹��ߤޤ���
 *
 * @param    buf �ǡ����γ�Ǽ���
 * @param    size �����Υǡ����ΥХ��ȿ�
 * @return   �������������ɤ߹�����ǡ����β����ΥХ��ȿ�<br>
 *           ���ȥ꡼�ब����ξ�硤0�Х��Ȥ��ɤ߹��ߤ����ꤵ�줿����0<br>
 *           ���顼�ξ�������
 * @note     ���Υ��дؿ��� private �Ǥ���
 */
ssize_t gzstreamio::_raw_read( void *buf, size_t size )
{
    ssize_t l;
    if ( this->ind_sp != NULL ) {
	return this->indirect_read(buf,size);
    }
    if ( this->gzfp == NULL ) return -1;
    if ( buf == NULL && 0 < size ) return -1;
    /* gzread��0�Х����ɤ�Ǥ��ޤ��ȵ�ư�Կ��ˤʤ� 2012/10/19 */
    if ( size == 0 ) return 0;
    l = gzread( (gzFile)(this->gzfp), buf, size );
    if ( l < 0 ) return -1;
    else return l;
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
ssize_t gzstreamio::raw_write( const void *buf, size_t size )
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
 *  indirect_write() ���дؿ��� gzwrite() �ؿ���ƤӽФ���Ϳ����줿�Хåե�
 *  ������� size �Х��ȤΥǡ������ɤ߹��ߡ����߳�����Ƥ��륹�ȥ꡼��˽񤭹�
 *  �ߤޤ���
 *
 * @param      buf �ǡ����γ�Ǽ���
 * @param      size �ǡ����ΰ������ΥХ��ȿ�
 * @return     �����������ϰ������ΥХ��ȿ�<br>
 *             ���顼�ξ�������
 * @note       ���Υ��дؿ��� private �Ǥ���
 */
ssize_t gzstreamio::_raw_write( const void *buf, size_t size )
{
    ssize_t l;
    if ( this->ind_sp != NULL ) {
	return this->indirect_write(buf,size);
    }
    if ( this->gzfp == NULL ) return -1;
    if ( buf == NULL && 0 < size ) return -1;
    //fprintf(stderr,"debug: size = %ld\n",size);
    /* Ĺ������Τ��񤳤��Ȥ���ȥ��ߤ�����餷����orz */
    if ( size == 0 ) return 0;
    l = gzwrite( (gzFile)(this->gzfp), buf, size );
    if ( l < 0 ) return -1;
    else return l;
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
ssize_t gzstreamio::indirect_read( void *buf, size_t size )
{
    ssize_t return_len = -1;
    int zstatus = Z_OK;
    z_stream *zsp = (z_stream *)(this->indr_zptr.ptr());

    if ( buf == NULL && 0 < size ) goto quit;
    if ( size == 0 ) return 0;
    if ( zsp == NULL ) {
	if ( this->ind_sp != NULL ) return 0;
	goto quit;
    }

    zsp->next_out = (Bytef *)buf;	/* ���ϥХåե��򥻥å� */
    zsp->avail_out = size;

    do {
        if ( zsp->avail_in == 0 ) {	/* �ɤ߽���ä���� */
	    ssize_t sz;
	    if ( this->indr_buffer.reallocate(2 * size) < 0 ) {
		this->errflg_rec = 1;
		err_throw(__FUNCTION__,"FATAL","realloc() failed");
	    }
            zsp->next_in = (Bytef *)(this->indr_buffer.ptr());
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
            zsp->avail_in = sz;
        }
        zstatus = inflate(zsp, Z_NO_FLUSH);
        if ( zstatus == Z_STREAM_END ) {
	    this->eofflg_rec = 1;
	    break;
	}
        if ( zstatus != Z_OK ) {
	    this->errflg_rec = zstatus;
	    err_report1(__FUNCTION__,"ERROR","inflate() failed, msg: %s",
			zsp->msg);
	    goto quit;
        }
    } while ( 0 < zsp->avail_out );

    return_len = (ssize_t)size - zsp->avail_out;

    if ( zstatus == Z_STREAM_END ) {
	int zs;
	/* ����� */
	if ( (zs = inflateEnd(zsp)) != Z_OK ) {
	    this->errflg_rec = zs;
	    err_report1(__FUNCTION__,"ERROR","inflateEnd() failed, msg: %s",
			zsp->msg);
	}
	this->indr_buffer = NULL;
	this->indr_zptr = NULL;
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
ssize_t gzstreamio::indirect_write( const void *buf, size_t size )
{
    ssize_t return_len = -1;
    int zstatus = Z_OK;
    z_stream *zsp = (z_stream *)(this->indw_zptr.ptr());

    if ( buf == NULL && 0 < size ) goto quit;
    if ( size == 0 ) return 0;
    if ( zsp == NULL ) {
	goto quit;
    }

    zsp->next_in = (Bytef *)buf;	/* ���ϥХåե��򥻥å� */
    zsp->avail_in = size;

    while ( 0 < zsp->avail_in ) {	/* ���Ϥ����񤵤��ޤ� */
        this->indw_in += zsp->avail_in;
	zstatus = deflate(zsp,Z_NO_FLUSH);
        this->indw_in -= zsp->avail_in;
        if ( zstatus != Z_OK ) {
	    this->errflg_rec = zstatus;
	    err_report1(__FUNCTION__,"ERROR","deflate() failed, msg: %s",
			zsp->msg);
	    goto quit;
        }
        if ( zsp->avail_out == 0 ) {	/* ��������� */
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
            zsp->next_out = (Bytef *)(this->indw_buffer.ptr());
            zsp->avail_out = this->indw_buffer_size;
        }
    }

    this->indw_crc = crc32(this->indw_crc, (const Bytef *)buf, size);

    return_len = size;
 quit:
    return return_len;
}

}	/* namespace sli */


#include "private/c_close.c"
#include "private/c_dup.c"
#include "private/c_strncmp.cc"

#endif	/* #ifdef HAVE_LIBZ */
