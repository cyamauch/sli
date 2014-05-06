/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2013-05-13 23:32:31 cyamauch> */

/**
 * @file   pipestreamio.cc
 * @brief  �ѥ��פ�Ȥä����ȥ꡼�������Ϥ�Ԥʤ� pipestreamio ���饹�Υ�����
 */

#define CLASS_NAME "pipestreamio"

#include "config.h"

#include "pipestreamio.h"
#include "tarray_tstring.h"

#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <assert.h>

#include "private/err_report.h"

#include "private/c_close.h"
#include "private/c_getppid.h"
#include "private/c_dup2.h"
#include "private/c_pipe.h"
#include "private/c_fork.h"
#include "private/c_execvp.h"
#include "private/c__exit.h"

#include "private/c_fdopen.h"
#include "private/c_fclose.h"

#include "private/c_strlen.h"
#include "private/c_strcmp.h"

namespace sli
{

/**
 * @brief  ���󥹥ȥ饯��
 */
pipestreamio::pipestreamio()
{
    this->pfds[0] = -1;
    this->pfds[1] = -1;
    this->fp = NULL;
    this->pid = -1;
    return;
}

/**
 * @brief  �ǥ��ȥ饯��
 */
pipestreamio::~pipestreamio()
{
    this->close();
    return;
}

/* public member functions : etc */
/**
 * @brief  ���ޥ�ɤ�¹Ԥ����ѥ��פ���³���ơ����ȥ꡼��򥪡��ץ�
 *
 *  ���ޥ�ɤϡ�fork() �ƤӽФ���� execvp() �Ǽ¹Ԥ���ޤ���
 *
 * @param      mode    ���ȥ꡼��򳫤��⡼��
 * @param      argv[]  ���ޥ�ɤ������ǥǡ���
 * @return     ������������0<br>
 *             ���顼�ξ�������
 * @throw      �ɤ߽Ф��Ѥ���ӽ񤭹����ѤΥե����롦�ǥ�������ץ���������
 *             ���Ԥ������
 * @throw      �ץ����������˼��Ԥ������
 */
int pipestreamio::open( const char *mode, const char *const argv[] )
{
    int return_status = -1;
    bool argv_is_argv = false;

    if ( argv == NULL ) return this->open(mode);

    if ( argv != NULL && argv[0] != NULL && 0 < c_strlen(argv[0]) ) {
        argv_is_argv = true;
    }
    else {
        goto quit;
    }

    /* setup output */
    if ( c_strcmp(mode,"w") == 0 && this->fp == NULL ) {
	if ( this->pid != -1 ) goto quit;
	/* prepare pipe */
	if ( c_pipe(this->pfds) < 0 ) {
            err_throw(__FUNCTION__,"FATAL","pipe() failed");
	}
	if ( ( this->pid=c_fork() ) < 0 ) {
	    c_close(this->pfds[1]);
	    this->pfds[1]=-1;
	    c_close(this->pfds[0]);
	    this->pfds[0]=-1;
            err_throw(__FUNCTION__,"FATAL","fork() failed");
	}
	if ( this->pid == 0 ) {		/* child process */
	    pid_t ppid;
	    c_dup2( this->pfds[0],0 );	/* stdin */
	    c_close( this->pfds[1] );
	    c_close( this->pfds[0] );
	    c_execvp( *argv, (char *const *)argv );
            err_report1(__FUNCTION__,"ERROR","Cannot execute %s",*argv);
	    ppid = c_getppid();
	    kill(ppid,SIGTERM);
	    c__exit(-1);
	}
	/* parent process */
	c_close(this->pfds[0]);
	this->pfds[0]=-1;
	this->fp = c_fdopen(this->pfds[1],"w");
	if ( this->fp == NULL ) {
	    c_close(this->pfds[1]);
	    this->pfds[1]=-1;
	    goto quit;
	}

	return_status = 0;
    }
    /* setup input */
    else if ( c_strcmp(mode,"r") == 0 && this->fp == NULL ) {
	if ( this->pid != -1 ) goto quit;
	/* prepare pipe */
	if ( c_pipe(this->pfds) < 0 ) {
            err_throw(__FUNCTION__,"FATAL","pipe() failed");
	}
	if ( ( this->pid=c_fork() ) < 0 ) {
	    c_close(this->pfds[1]);
	    this->pfds[1]=-1;
	    c_close(this->pfds[0]);
	    this->pfds[0]=-1;
            err_throw(__FUNCTION__,"FATAL","fork() failed");
	}
	if ( this->pid == 0 ) {		/* child process */
	    pid_t ppid;
	    c_dup2( this->pfds[1],1 );	/* stdout */
	    c_close( this->pfds[1] );
	    c_close( this->pfds[0] );
	    c_execvp( *argv, (char *const *)argv );
            err_report1(__FUNCTION__,"ERROR","Cannot execute %s",*argv);
	    ppid = c_getppid();
	    kill(ppid,SIGTERM);
	    c__exit(-1);
	}
	/* parent process */
	c_close(this->pfds[1]);
	this->pfds[1]=-1;
	this->fp = c_fdopen(this->pfds[0],"r");
	if ( this->fp == NULL ) {
	    c_close(this->pfds[0]);
	    this->pfds[0]=-1;
	    goto quit;
	}

	return_status = 0;
    }

 quit:
    return return_status;
}

/**
 * @brief  ���ޥ�ɤ�¹Ԥ����ѥ��פ���³���ơ����ȥ꡼��򥪡��ץ�
 *
 *  ���ޥ�ɤϡ�/bin/sh -c �� �η��Ǽ¹Ԥ���ޤ���
 *
 * @param      mode  ���ȥ꡼��򳫤��⡼��
 * @param      path  ���ޥ��
 * @return     ������������0<br>
 *             ���顼�ξ�������
 * @throw      �ɤ߽Ф��Ѥ���ӽ񤭹����ѤΥե����롦�ǥ�������ץ���������
 *             ���Ԥ������
 * @throw      �ץ����������˼��Ԥ������
 */
int pipestreamio::open( const char *mode, const char *path )
{
    assert( mode != NULL );

    const char *argv[4] = {"/bin/sh","-c",path,NULL};
    if ( path == NULL ) return this->open(mode);
    else return this->open( mode, argv );
}

/**
 * @brief  �ǥ�������ץ�����ˤ�륹�ȥ꡼��Υ����ץ�
 *
 *  �ǥ�������ץ�����ꤷ�ơ����ȥ꡼��򥪡��ץ󤷤ޤ���
 *
 * @deprecated  ���Ѥ����̣�Ϥ���ޤ���<br>
 *              ���Υ��дؿ��Ǥϡ����ޥ�ɤ�¹Ԥ����ѥ��פ���³���ơ�
 *              ���ȥ꡼��򥪡��ץ󤹤���ϤǤ��ޤ���
 * @param       mode ���ȥ꡼��򳫤��⡼�� ("r" or "w")
 * @param       fd   �ե�����ǥ�������ץ�
 * @return      ������������0<br>
 *              ���顼�ξ�������
 * @note        ���������ͳ�Ǻ������줿���дؿ��Ǥ���
 */
int pipestreamio::open( const char *mode, int fd )
{
    assert( mode != NULL );

    return this->cstreamio::open(mode,fd);
}

/**
 * @brief  ɸ�������ϤΥ����ץ�
 *
 *  ɸ�������Ϥ򥪡��ץ󤷤ޤ���<br>
 *  mode �� "r" �����ꤵ�줿����ɸ�����Ϥ�"w" �����ꤵ�줿����ɸ����Ϥ�
 *  �����ץ󤷤ޤ�.
 *
 * @deprecated  ���Ѥ����̣�Ϥ���ޤ���<br>
 *              ���Υ��дؿ��Ǥϡ����ޥ�ɤ�¹Ԥ����ѥ��פ���³���ơ�
 *              ���ȥ꡼��򥪡��ץ󤹤���ϤǤ��ޤ���
 * @param     mode ���ȥ꡼��򳫤��⡼��("r" or "w")
 * @return    ������������0<br>
 *            ���顼�ξ�������
 * @throw     err_rec ɸ�������ϤΥե����롦�ǥ�������ץ���ʣ���˼��Ԥ������
 * @note      ���������ͳ�Ǻ������줿���дؿ��Ǥ���
 */
int pipestreamio::open( const char *mode )
{
    assert( mode != NULL );

    return this->cstreamio::open(mode);
}

/**
 * @brief  ���ȥ꡼��Υ�����
 *
 *  open()�ǳ��������ȥ꡼��򥯥������ޤ���
 *
 * @return     ������������0<br>
 *             ���顼�ξ���0�ʳ�����
 */
int pipestreamio::close()
{
    int return_status = 0;

    if ( this->fp != NULL ) {
	return_status = c_fclose(this->fp);
	this->fp = NULL;
	this->pfds[0]=-1;
	this->pfds[1]=-1;
    }
    if ( this->pfds[1] != -1 ) {
	int status;
	status = c_close(this->pfds[1]);
	this->pfds[1]=-1;
	if ( status != 0 ) return_status = status;
    }
    if ( this->pfds[0] != -1 ) {
	int status;
	status = c_close(this->pfds[0]);
	this->pfds[0]=-1;
	if ( status != 0 ) return_status = status;
    }
    if ( 0 <= this->pid ) {
	int status, xstatus;
        while ( waitpid(this->pid,&status,0) != this->pid );
	this->pid = -1;
	if ( WIFEXITED(status) ) {	/* ���ｪλ�ξ�� */
	    xstatus = WEXITSTATUS(status);
	}
	else {				/* �۾ｪλ�ξ�� */
	    xstatus = EOF;
	}
	if ( xstatus != 0 ) return_status = xstatus;
    }

    this->cstreamio::close();

    return return_status;
}

}	/* namespace sli */


#include "private/c_close.c"
#include "private/c_getppid.c"
#include "private/c_dup2.c"
#include "private/c_pipe.c"
#include "private/c_fork.c"
#include "private/c_execvp.c"
#include "private/c__exit.c"

#include "private/c_fdopen.c"
#include "private/c_fclose.c"

#include "private/c_strlen.cc"
#include "private/c_strcmp.cc"
