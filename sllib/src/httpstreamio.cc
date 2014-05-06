/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2013-05-13 23:26:14 cyamauch> */

/**
 * @file   httpstreamio.cc
 * @brief  HTTP�����Ф��Ф��ƥ��ȥ꡼�����Ϥ�Ԥʤ� httpstreamio ���饹�Υ�����
 */

#define CLASS_NAME "httpstreamio"

#include "config.h"

#include "httpstreamio.h"
#include "inetstreamio.h"
#ifdef HAVE_LIBZ
#include "gzstreamio.h"
#endif
#ifdef HAVE_LIBBZ2
#include "bzstreamio.h"
#endif
#include "tstring.h"

#include <assert.h>

#include "private/err_report.h"

#include "private/c_gethostname.h"

#include "private/c_fdopen.h"
#include "private/c_fclose.h"

#include "private/c_strcpy.h"
#include "private/c_strncmp.h"
#include "private/c_dup.h"
#include "private/c_close.h"

#include "private/suffix2filetype.h"


namespace sli
{

/**
 * @brief  ���󥹥ȥ饯��
 */
httpstreamio::httpstreamio()
{
    this->inet_sp = NULL;
    this->filter_sp = NULL;
    this->content_length_rec = -1;
    return;
}

/**
 * @brief  �ǥ��ȥ饯��
 */
httpstreamio::~httpstreamio()
{
    this->close();
    return;
}

/**
 * @brief  http:// ����Ϥޤ� URL �Υ����ץ�
 *
 *  �ѥ�����ꤷ�ơ�URL�򥪡��ץ󤷤ޤ���<br>
 *  mode �� "r%" ����ꤹ��ȡ����̥��ȥ꡼��ξ��ϼ�ưŪ�˿�Ĺ��Ԥʤ��ޤ���
 *
 * @param      mode URL�򳫤��⡼��("r" or "r%")
 * @param      path URL�Υѥ�
 * @return     ������������0<br>
 *             ���顼�ξ�������
 * @throw      ����γ�����Ƥ˼��Ԥ������
 */
int httpstreamio::open( const char *mode, const char *path )
{
    assert( mode != NULL );
    
    int return_status = -1;
    
    /* "r%" ����ꤹ����ǡ����̥��ȥ꡼���Ÿ�����б� */
    if ( c_strncmp(mode,"r%",2) == 0 || c_strncmp(mode,"rb%",3) == 0 ) {
        return_status = this->open_r(mode,path,true);
    } else if ( c_strncmp(mode,"r",1) == 0 || c_strncmp(mode,"rb",2) == 0 ) {
        return_status = this->open_r(mode,path,false);
    } else {
        return_status = -1;
    }
    return return_status;
}

/**
 * @brief  URL�Υ����ץ� (���٥�)
 *
 *  �ѥ�����ꤷ�ơ�URL�򥪡��ץ󤷤ޤ���
 *
 * @param      mode URL�򳫤��⡼�� ("r" or "r%")
 * @param      path URL�Υѥ�
 * @param      decompress_by_suffix ���̥��ȥ꡼�࿭Ĺ�����̵ͭ
 * @return     ������������0<br>
 *             ���顼�ξ�������
 * @throw      ����γ�����Ƥ˼��Ԥ������
 * @note       ���Υ��дؿ��� protected �Ǥ���
 */
int httpstreamio::open_r( const char *mode, const char *path,
			  bool decompress_by_suffix )
{
    int return_status = -1;
    inetstreamio *isp = NULL;	/* inetstreamio */
    cstreamio *dsp = NULL;	/* gzstreamio or bzstreamio */
    char rw_mode[2] = { 'r', '\0' };

    if ( mode == NULL ) goto quit;
    if ( path == NULL ) goto quit;
    if ( c_strncmp(path,"http://",7) != 0 ) goto quit;
    if ( this->inet_sp != NULL ) goto quit;

    if ( mode[0] == 'w' ) rw_mode[0] = 'w';

    try {
	bool path_is_gzip = false, path_is_bzip2 = false;
	bool enc_is_gzip = false, enc_is_bzip2 = false;
	char hostname[256];
	const char *l_ptr;
	tstring uagent;

	isp = new inetstreamio;

	/* �ۥ���̾ */
	if ( c_gethostname(hostname,256) != 0 ) {
	    c_strcpy(hostname,"VeryLongHostname");
	}
	hostname[255] = '\0';

	if ( 0 < this->user_agent_rec.length() &&
	     this->user_agent_rec.strpbrk("[[:cntrl:]]") < 0 ) {
	    uagent.assign(this->user_agent_rec.cstr()).strtrim();
	}
	else {
	    uagent.printf("%s %s-%s::httpstreamio",hostname,
			  PACKAGE_NAME,PACKAGE_VERSION);
	}

	suffix2filetype(path,&path_is_gzip,&path_is_bzip2);
	
	if ( isp->open("r+",path) < 0 ) {
	    err_report1(__FUNCTION__,"ERROR","cannot open url: %s",path);
	    goto quit;
	}

	/* ���� */
	isp->printf("GET %s HTTP/1.0\r\n",isp->path());
	if ( path_is_gzip == false && path_is_bzip2 == false ) {
	    isp->printf("Accept-Encoding: "
		    "gzip, x-gzip, compress, x-compress, deflate, x-deflate, "
		    "bzip, x-bzip, bzip2, x-bzip2\r\n");
	}
	isp->printf("User-Agent: %s\r\n",uagent.cstr());
	isp->printf("Host: %s\r\n",isp->host());
	isp->printf("Connection: close\r\n");
	isp->printf("\r\n");

	isp->flush();

	/* ���� */
	while ( (l_ptr=isp->getline()) != NULL ) {
	    tstring rline, str;
	    rline.assign(l_ptr);
	    if ( rline.strcmp("\r\n") == 0 ) break;
	    //fprintf(stderr,"debug: hd = %s\n",l_ptr);
	    str = "Content-Encoding: ";
	    if ( rline.strncmp(str,0,str.length()) == 0 ) {
		tstring enc;
		enc.assign(rline,str.length()).strtrim(" \r\n");
		if ( enc.strcmp("gzip") == 0 ||
		     enc.strcmp("x-gzip") == 0 ||
		     enc.strcmp("compress") == 0 ||
		     enc.strcmp("x-compress") == 0 ||
		     enc.strcmp("deflate") == 0 ||
		     enc.strcmp("x-deflate") == 0 ) {
		    enc_is_gzip = true;
		}
		if ( enc.strcmp("bzip") == 0 || enc.strcmp("x-bzip") == 0 ||
		     enc.strcmp("bzip2") == 0 || enc.strcmp("x-bzip2") == 0 ) {
		    enc_is_bzip2 = true;
		}
	    }
	    str = "Content-Length: ";
	    if ( rline.strncmp(str,0,str.length()) == 0 ) {
		long long l;
		l = rline.atoll(str.length());
		this->content_length_rec = l;
	    }
	    str = "Content-Type: ";
	    if ( rline.strncmp(str,0,str.length()) == 0 ) {
		ssize_t p  = rline.strchr(str.length(),'/');
		ssize_t p1 = rline.strchr(str.length(),';');
		if ( 0 <= p ) {
		    tstring typ;
		    p++;
		    if ( 0 <= p1 && p < p1 ) {
			typ.assign(rline,p,(size_t)(p1 - p)).strtrim(" \r\n");
		    }
		    else {
			typ.assign(rline,p).strtrim(" \r\n");
		    }
		    //fprintf(stderr,"debug: p = [%s]\n",p);
		    if ( typ.strcmp("gzip") == 0 || 
			 typ.strcmp("x-gzip") == 0 ||
			 typ.strcmp("compress") == 0 || 
			 typ.strcmp("x-compress") == 0 ||
			 typ.strcmp("deflate") == 0 || 
			 typ.strcmp("x-deflate") == 0 ) {
			path_is_gzip = true;
			path_is_bzip2 = false;
		    }
		    if ( typ.strcmp("bzip") == 0 || 
			 typ.strcmp("x-bzip") == 0 ||
			 typ.strcmp("bzip2") == 0 || 
			 typ.strcmp("x-bzip2") == 0 ) {
			path_is_bzip2 = true;
			path_is_gzip = false;
		    }
		}
	    }
	}

#ifndef HAVE_LIBZ
	enc_is_gzip = false;
	path_is_gzip = false;
#endif
#ifndef HAVE_LIBBZ2
	enc_is_bzip2 = false;
	path_is_bzip2 = false;
#endif

	if ( enc_is_gzip != false || enc_is_bzip2 != false ) {
	    if ( enc_is_gzip != false ) {
#ifdef HAVE_LIBZ
		dsp = new gzstreamio;
#endif
	    }
	    else if ( enc_is_bzip2 != false ) {
#ifdef HAVE_LIBBZ2
		dsp = new bzstreamio;
#endif
	    }
	    if ( dsp != NULL ) {
	      if ( dsp->open("r",*isp) < 0 ) {
		err_report(__FUNCTION__,"ERROR","dsp->open() failed");
		goto quit;
	      }
	    }
	    else {
	      /* never */
	    }
	}
	else {
	    if ( decompress_by_suffix != false &&
		 ( path_is_gzip != false || path_is_bzip2 != false ) ) {
		if ( path_is_gzip != false ) {
#ifdef HAVE_LIBZ
		    dsp = new gzstreamio;
#endif
		}
		else if ( path_is_bzip2 != false ) {
#ifdef HAVE_LIBBZ2
		    dsp = new bzstreamio;
#endif
		}
		if ( dsp != NULL ) {
		  if ( dsp->open("r",*isp) < 0 ) {
		    err_report(__FUNCTION__,"ERROR","dsp->open() failed");
		    goto quit;
		  }
		}
		else {
		  /* never */
		}
	    }
	}

    }
    catch ( ... ) {
	if ( dsp != NULL ) delete dsp;
	if ( isp != NULL ) delete isp;
	err_throw(__FUNCTION__,"FATAL","Memory allocation failed");
    }

    return_status = 0;
 quit:
    if ( return_status < 0 ) {
	if ( isp != NULL ) {
	    isp->close();
	    delete isp;
	}
	if ( dsp != NULL ) {
	    dsp->close();
	    delete dsp;
	}
	this->content_length_rec = -1;
    }
    else {
	if ( isp != NULL ) this->inet_sp = isp;
	if ( dsp != NULL ) this->filter_sp = dsp;
    }
    return return_status;
}

/**
 * @brief  ���ȥ꡼��ΥХ���Ĺ�����
 *
 *  HTTP �إå��� Content-Length ���ͤ򤽤Τޤ��֤��ޤ���
 *
 * @return     ������: HTTP �إå��� Content-Length ����<br>
 *             ����: HTTP �إå��� Content-Length ��¸�ߤ��ʤ����
 */
long long httpstreamio::content_length() const
{
    return this->content_length_rec;
}

/**
 * @brief  �桼��������������������Ѥ�ʸ���󥪥֥������Ȥ��֤�
 *
 * @return     ʸ���󥪥֥�������(tstring)�λ���
 */
tstring &httpstreamio::user_agent()
{
    return this->user_agent_rec;
}

/**
 * @brief  ɸ�������ϤΥ����ץ�
 *
 *  ɸ�������Ϥ򥪡��ץ󤷤ޤ���<br>
 *  mode �� "r" �����ꤵ�줿����ɸ�����Ϥ�"w" �����ꤵ�줿����ɸ����Ϥ�
 *  �����ץ󤷤ޤ�.
 *
 * @deprecated  ���Ѥ����̣�Ϥ���ޤ���<br>
 *              ���Υ��дؿ��Ǥϡ��ѥ�����ꤷ�ơ�URL�򥪡��ץ󤹤����
 *              �Ǥ��ޤ���
 * @param     mode ���ȥ꡼��򳫤��⡼�� ("r" or "w")
 * @return    ������������0<br>
 *            ���顼�ξ�������
 * @throw     err_rec ɸ�������ϤΥե����롦�ǥ�������ץ���ʣ���˼��Ԥ������
 * @note      ���������ͳ�Ǻ������줿���дؿ��Ǥ���
 */
int httpstreamio::open( const char *mode )
{
    assert( mode != NULL );

    return this->cstreamio::open(mode);
}

/**
 * @brief  �ǥ�������ץ�����ˤ�륹�ȥ꡼��Υ����ץ�
 *
 *  �ǥ�������ץ�����ꤷ�ơ����ȥ꡼��򥪡��ץ󤷤ޤ���
 *
 * @deprecated  ���Ѥ����̣�Ϥ���ޤ���<br>
 *              ���Υ��дؿ��Ǥϡ��ѥ�����ꤷ�ơ�URL�򥪡��ץ󤹤����
 *              �Ǥ��ޤ���
 * @param       mode ���ȥ꡼��򳫤��⡼�� ("r" or "w")
 * @param       fd   �ե�����ǥ�������ץ�
 * @return      ������������0<br>
 *              ���顼�ξ�������
 * @note        ���������ͳ�Ǻ������줿���дؿ��Ǥ���
 */
int httpstreamio::open( const char *mode, int fd )
{
    assert( mode != NULL );

    int return_status = -1;
    int fd_new = -1;

    if ( this->fp != NULL ) goto quit;

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
 */
int httpstreamio::close()
{
    int return_status = 0;

    if ( this->filter_sp != NULL ) {
	return_status = this->filter_sp->close();
	delete this->filter_sp;
	this->filter_sp = NULL;
    }
    if ( this->inet_sp != NULL ) {
	int status;
	status = this->inet_sp->close();
	delete this->inet_sp;
	this->inet_sp = NULL;
	if ( status != 0 ) return_status = status;
    }
    this->content_length_rec = -1;

    if ( this->fp != NULL ) {
	int status;
        status = c_fclose(this->fp);
        this->fp = NULL;
	if ( status != 0 ) return_status = status;
    }

    this->cstreamio::close();

    return return_status;
}

/**
 * @brief  EOF�ؼ��Ҥμ���
 */
int httpstreamio::eof()
{
    if ( this->filter_sp != NULL ) {
	return this->filter_sp->eof();
    }
    else if ( this->inet_sp != NULL ) {
	return this->inet_sp->eof();
    }
    else return this->cstreamio::eof();
}

/**
 * @brief  ���顼�ؼ��Ҥμ���
 */
int httpstreamio::error()
{
    if ( this->filter_sp != NULL ) {
	return this->filter_sp->error();
    }
    else if ( this->inet_sp != NULL ) {
	return this->inet_sp->error();
    }
    else return this->cstreamio::error();
}

/**
 * @brief  EOF�ؼ��Ҥȥ��顼�ؼ��ҤΥꥻ�å�
 */
httpstreamio &httpstreamio::reseterr()
{
    if ( this->filter_sp != NULL ) {
	this->filter_sp->reseterr();
    }
    else if ( this->inet_sp != NULL ) {
	this->inet_sp->reseterr();
    }
    else this->cstreamio::reseterr();
    return *this;
}

/**
 * @brief  �ɤ߹����Ѹ��ϴؿ��Υ���å� (���Υ��饹�ˤƥ����Х饤��)
 *
 * @param    buf �ǡ����γ�Ǽ���
 * @param    size �ǡ����θĿ�
 * @return   �����������ϥХ��ȿ�<br>
 *           ���ȥ꡼�ब����ξ�硤0�Х��Ȥ��ɤ߹��ߤ����ꤵ�줿����0<br>
 *           ���顼�ξ�������
 * @note     ���Υ��дؿ��� protected �Ǥ���
 */
ssize_t httpstreamio::raw_read( void *buf, size_t size )
{
    if ( this->filter_sp != NULL ) {
	return this->filter_sp->read(buf,size);
    }
    else if ( this->inet_sp != NULL ) {
	return this->inet_sp->read(buf,size);
    }
    else return -1;
}

}	/* namespace sli */


#include "private/c_gethostname.c"

#include "private/c_fdopen.c"
#include "private/c_fclose.c"

#include "private/c_strcpy.c"
#include "private/c_strncmp.cc"
#include "private/c_dup.c"
#include "private/c_close.c"

#include "private/suffix2filetype.c"
