/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2013-05-13 23:47:25 cyamauch> */

/**
 * @file   inetstreamio.cc
 * @brief  ���٥��Internet��������(telnet)��Ԥʤ�inetstreamio���饹�Υ�����
 */

#define CLASS_NAME "inetstreamio"

#include "config.h"

#include "inetstreamio.h"

#include <stdlib.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <assert.h>

#include "private/err_report.h"

#include "private/c_close.h"

#include "private/c_fflush.h"
#include "private/c_fdopen.h"
#include "private/c_fclose.h"
#include "private/c_fputc.h"
#include "private/c_fputs.h"
#include "private/c_fwrite.h"
#include "private/c_dup.h"
#include "private/c_strncmp.h"
#include "private/c_strstr.h"
#include "private/c_strchr.h"
#include "private/c_strdup.h"
#include "private/c_memset.h"
#include "private/c_memcpy.h"

namespace sli
{

/**
 * @brief  ���󥹥ȥ饯��
 */
inetstreamio::inetstreamio()
{
    this->fpw = NULL;
    return;
}

/**
 * @brief  �ǥ��ȥ饯��
 */
inetstreamio::~inetstreamio()
{
    this->close();
    return;
}

/**
 * @brief  �����ѥ��ȥ꡼��Υե�å���
 *
 *  �Хåե��˳�Ǽ����Ƥ��뤹�٤ƤΥǡ����������ѥ��ȥ꡼��˽񤭹��ߤޤ���
 */
int inetstreamio::flush()
{
    if ( this->fpw != NULL )
	return c_fflush(this->fpw);
    else
	return EOF;
}

/**
 * @brief  URL�Υ����ץ� (������)
 *
 *  URL ����ꤷ��, ������(�����ѡ�������)�Υ��ȥ꡼��򥪡��ץ󤷤ޤ���
 *
 * @param      mode  ���ȥ꡼��򳫤��⡼��
 * @param      path  URL
 * @return     ������������0<br>
 *             ���顼�ξ�������
 * @throw      ����γ�����Ƥ˼��Ԥ������ 
 * @throw      �����å��̿��˼��Ԥ������
 * @throw      Ϳ����줿mode�ǥ����åȤ�open����ʤ��ä���� 
 */
int inetstreamio::open( const char *mode, const char *path )
{
    assert( mode != NULL );

    int return_status = -1;
    struct hostent *svrhost;
    struct sockaddr_in server;
    int sock=-1;
    heap_mem<char> url;

    if ( path == NULL ) return this->open(mode);

    if ( mode == NULL ) goto quit;
    if ( mode[0] != 'r' && mode[0] != 'w' ) goto quit;
    if ( mode[1] != '+' && mode[1] != '\0' ) goto quit;
    if ( mode[1] == '+' && mode[2] != '\0' ) goto quit;
    if ( this->fp != NULL ) goto quit;
    if ( this->fpw != NULL ) goto quit;

    c_memset(&server,0,sizeof(server));

    url = c_strdup(path);
    if ( url.ptr() == NULL ) {
	err_throw(__FUNCTION__,"FATAL","strdup() failed");
    }

    try {
	struct servent *service;
	char *slasla;
	char *sla;
	char *colon;

	slasla = (char *)c_strstr(url.ptr(),"://");
	if ( slasla == NULL ) {
	    err_report1(__FUNCTION__,"ERROR","invalid url : %s",path);
	    goto quit;
	}
	*slasla = '\0';

	/* service name -> port */
	service = getservbyname(url.ptr() /* protocol */, "tcp");
	if ( service != NULL ) {
	    server.sin_port = service->s_port;
	} else {
	    server.sin_port = htons(80);
	}
	
	sla = c_strchr(slasla + 3, '/');
	if ( sla != NULL ) {
	    *sla = '\0';
	}
	
	this->host_rec = c_strdup(slasla+3);
	if ( this->host_rec.ptr() == NULL ) {
	    err_throw(__FUNCTION__,"FATAL","strdup() failed");
	}
	
	colon = c_strchr(slasla + 3, ':');
	if ( colon != NULL ) {
	    uint16_t port;
	    *colon = '\0';
	    port = atoi(colon + 1);
	    if ( 0 < port ) server.sin_port = htons(port);
	}
	
	/* hostname -> IP */
	svrhost = gethostbyname(slasla+3 /* host */);
	if ( svrhost == NULL ){
	    err_report1(__FUNCTION__,"ERROR","cannot be resolved : %s",slasla+3);
	    goto quit;
	}
	
	if ( sla != NULL ) {
	    *sla = '/';
	    this->path_rec = c_strdup(sla);
	}
	else {
	    this->path_rec = c_strdup("/");
	}
	if ( this->path_rec.ptr() == NULL ) {
	    err_throw(__FUNCTION__,"FATAL","strdup() failed");
	}

    }
    catch (...) {
	err_throw(__FUNCTION__,"FATAL","Memory allocation failed");
    }

    server.sin_family = AF_INET;
    c_memcpy(&server.sin_addr, svrhost->h_addr, svrhost->h_length);

    /* �����Ф���³ */
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if ( sock < 0 ) {
	err_throw(__FUNCTION__,"FATAL","cannot create socket");
    }

    if ( connect(sock, (struct sockaddr *)&server, sizeof(server)) == -1 ) {
	err_report(__FUNCTION__,"ERROR","cannot connect to the server");
	goto quit;
    }

    if ( mode[0] == 'w' || (mode[0] == 'r' && mode[1] == '+') ) {
	this->fpw = c_fdopen(sock,"w");
	if ( this->fpw == NULL ) {
	    c_close(sock);
	    err_throw(__FUNCTION__,"FATAL","fdopen() failed");
	}
    }

    if ( mode[0] == 'r' || (mode[0] == 'w' && mode[1] == '+') ) {
	this->fp = c_fdopen(sock,"r");
	if ( this->fp == NULL ) {
	    if ( this->fpw != NULL ) {
		c_fclose(this->fpw);
		this->fpw = NULL;
	    }
	    else {
		c_close(sock);
	    }
	    err_throw(__FUNCTION__,"FATAL","fdopen() failed");
	}
    }

    return_status = 0;
 quit:
    if ( return_status != 0 ) {
	if ( 0 <= sock ) c_close(sock);
	this->path_rec = NULL;
	this->host_rec = NULL;
    }
    return return_status;
}

/**
 * @brief  ɸ�������ϤΥ����ץ�
 *
 *  ɸ�������Ϥ򥪡��ץ󤷤ޤ�.<br>
 *  mode �� "r" �����ꤵ�줿����ɸ�����Ϥ�"w" �����ꤵ�줿����ɸ����Ϥ�
 *  �����ץ󤷤ޤ�.
 *
 * @deprecated  ���Ѥ��ʤ��ǲ�������<br>
 *              ���Υ��дؿ��Ǥϡ��ѥ�����ꤷ�ơ�URL�򥪡��ץ󤹤����
 *              �Ǥ��ޤ���
 * @param     mode ���ȥ꡼��򳫤��⡼�� ("r" or "w")
 * @return    ������������0<br>
 *            ���顼�ξ�������
 * @throw     err_rec ɸ�������ϤΥե����롦�ǥ�������ץ���ʣ���˼��Ԥ������
 * @note      ���������ͳ�Ǻ������줿���дؿ��Ǥ��ꡤ�桼����ľ������
 *            ���٤��ǤϤ��ꤻ��
 */
int inetstreamio::open( const char *mode )
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
 *              ���Υ��дؿ��Ǥϡ��ѥ�����ꤷ�ơ�URL�򥪡��ץ󤹤����<br>
 *              �Ǥ��ޤ���
 * @param       mode ���ȥ꡼��򳫤��⡼��
 * @param       fd   �ե�����ǥ�������ץ�
 * @return      ������������0<br>
 *              ���顼�ξ�������
 * @note        ���������ͳ�Ǻ������줿���дؿ��Ǥ���
 */
int inetstreamio::open( const char *mode, int fd )
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
    this->fpw = this->fp;
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
 * @return     ������������0 <br>
 *             ���顼�ξ���0�ʳ�����
 */
int inetstreamio::close()
{
    int return_status = 0;
    int status;
    if ( this->fpw == this->fp ) {
	this->fpw = NULL;
    }
    if ( this->fp != NULL ) {
	status = c_fclose(this->fp);
	this->fp = NULL;
	if ( status != 0 ) return_status = status;
    }
    if ( this->fpw != NULL ) {
	status = c_fclose(this->fpw);
	this->fpw = NULL;
	/* ������ status �Ͼ�� EOF ���֤�Τǡ��������𤷤ʤ� */
	/* �񤭹���¦�ޤ����ɤ߽Ф�¦�ΰ����������������ȡ�¾���ϥ��顼 */
	/* �ˤʤ�餷���� */
	/* if ( status != 0 ) return_status = status; */
    }
    this->path_rec = NULL;
    this->host_rec = NULL;
    return return_status;
}

/**
 * @brief  1ʸ���ν񤭹���
 *
 *  ���ꤵ�줿ʸ����open()�ǳ����������ѥ��ȥ꡼��˽񤭹��ߤޤ���
 *
 * @param      c �񤭹���ʸ��
 * @return     �����������Ͻ񤭹��ޤ줿ʸ����int���˥��㥹�Ȥ�����<br>
 *             ���顼�ξ���EOF
 */
int inetstreamio::putchr( int c )
{
    if ( this->fpw != NULL ) {
	return c_fputc(c,this->fpw);
    }
    else return EOF;
}

/**
 * @brief  ʸ����ν񤭹���
 *
 *  ���ꤵ�줿ʸ�����open()�ǳ����������ѥ��ȥ꡼��˽񤭹��ߤޤ���
 *
 * @param      s �񤭹���ʸ����
 * @return     ��������������ǤϤʤ���<br>
 *             ���顼�ξ���EOF
 */
int inetstreamio::putstr( const char *s )
{
    assert( s != NULL );

    if ( this->fpw != NULL ) {
	return c_fputs(s,this->fpw);
    }
    else return EOF;
}

/**
 * @brief  �ǡ����ν񤭹���
 *
 *  ���ꤵ�줿�ǡ�����open()�ǳ����������ѥ��ȥ꡼��˽񤭹��ߤޤ���
 *
 * @param      buf �ǡ����γ�Ǽ���
 * @param      size �ǡ����θĿ�
 * @return     �����������ϥХ��ȿ�<br>
 *             ���顼�ξ�������
 */
ssize_t inetstreamio::write( const void *buf, size_t size )
{
    assert( buf != NULL );

    if ( this->fpw != NULL ) {
	size_t n;
	n = c_fwrite( buf, 1,size, this->fpw );
	if ( 0 < n ) return n;
	else {
	    if ( n == size ) return n;
	    else return -1;
	}
    }
    else return -1;
}

/**
 * @brief  �ѥ�(URL��"/"�ʹߤ�ʸ����)���֤�
 *
 * @return     URL��Υѥ� (��: http://foo.com/abc �ʤ� /abc ���֤�)
 */
const char *inetstreamio::path() const
{
    return this->path_rec.ptr();
}

/**
 * @brief  �ۥ���̾(URL�� "://" �ȼ��� "/" �˶��ޤ줿ʸ����)���֤�
 *
 * @return     URL��Υۥ���̾ (��: http://foo.com/abc �ʤ� foo.com ���֤�)
 */
const char *inetstreamio::host() const
{
    return this->host_rec.ptr();
}

}	/* namespace sli */


#include "private/c_close.c"

#include "private/c_fflush.c"
#include "private/c_fdopen.c"
#include "private/c_fclose.c"
#include "private/c_fputc.cc"
#include "private/c_fputs.c"
#include "private/c_fwrite.c"
#include "private/c_dup.c"
#include "private/c_strncmp.cc"
#include "private/c_strstr.cc"
#include "private/c_strchr.cc"
#include "private/c_strdup.c"
#include "private/c_memset.cc"
#include "private/c_memcpy.cc"
