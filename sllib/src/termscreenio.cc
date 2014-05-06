/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2013-05-13 23:33:44 cyamauch> */

/**
 * @file   termscreenio.cc
 * @brief  ���ǥ������ڡ������ͳ�ǥ��ȥ꡼��򰷤� termscreenio ���饹�Υ�����
 */

#define CLASS_NAME "termscreenio"

#define _SLI__TERMSCREENIO_CC 1

#include "config.h"

#include "termscreenio.h"
#include "tarray_tstring.h"

#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <assert.h>

#include "private/err_report.h"

#include "private/c_close.h"
#include "private/c_pipe.h"
#include "private/c_fork.h"
#include "private/c_dup.h"
#include "private/c_dup2.h"
#include "private/c_execvp.h"
#include "private/c_getppid.h"
#include "private/c_isatty.h"
#include "private/c__exit.h"

#include "private/c_fdopen.h"
#include "private/c_snprintf.h"
#include "private/c_fclose.h"
#include "private/c_remove.h"

#include "private/c_strcpy.h"
#include "private/c_strlen.h"
#include "private/c_strcmp.h"

#include "private/get_argv_from_env.h"
#include "private/get_argv_from_argv.h"

#define DEFAULT_PAGER "more"
#define DEFAULT_EDITOR "vi"
#define TMP_DIR "/tmp"


namespace sli
{

/**
 * @brief  ���󥹥ȥ饯��
 */
termscreenio::termscreenio()
{
    this->pfds[0] = -1;
    this->pfds[1] = -1;
    this->fp = NULL;
    this->pid = -1;
    this->tmpfile[0] = '\0';
    return;
}

/**
 * @brief  �ǥ��ȥ饯��
 */
termscreenio::~termscreenio()
{
    this->close();
    return;
}

/* public member functions : etc */
/**
 * @brief  ���ȥ꡼��Υ����ץ�
 *
 *  mode �� "r" �����ꤵ�줿���ϥ��ǥ������̤��ư���ե�������������
 *  ���Ϥ��줿���Ƥ��ɤ߹��ߤޤ���"w" �����ꤵ�줿���ϥڡ�����򥪡��ץ�
 *  ���ޤ���
 *
 * @param      mode   ���ǥ����ޤ��ϥڡ�����򳫤��⡼�� ("r" or "w")
 * @param      argv[] ���ǥ����ޤ��ϥڡ�����������ǥǡ���
 * @return     ������������0<br>
 *             ���顼�ξ�������
 * @throw      �ǡ����ɤ߹����Ѥ��ΰ�γ��ݤ˼��Ԥ������
 * @throw      �ץ����������˼��Ԥ������
 * @throw      �ɤ߹����Ѥ���ӽ񤭹����ѤΥե����롦�ǥ�������ץ���������
 *             ���Ԥ������
 */
int termscreenio::open( const char *mode, const char *const argv[] )
{
    int return_status = -1;
    bool argv_is_argv;
    int efd = -1;
    int status;

    if ( argv != NULL && argv[0] != NULL && 0 < c_strlen(argv[0]) ) {
        argv_is_argv = true;
    }
    else {
        argv_is_argv = false;
    }

    /* setup pager */
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
	    if ( argv_is_argv == true ) c_execvp( *argv, (char *const *)argv );
	    else {
		char **pager_argv = 
		    get_argv_from_env("PAGER",DEFAULT_PAGER,NULL);
	        c_execvp( *pager_argv, pager_argv );
	    }
            err_report(__FUNCTION__,"ERROR","Cannot execute pager");
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
    /* setup editor */
    else if ( c_strcmp(mode,"r") == 0 && this->fp == NULL && c_isatty(0) != 0 ) {
	char tmp_filename[TERMSCREENIO__SIZE_TMPFILENAME];
	c_snprintf(tmp_filename,TERMSCREENIO__SIZE_TMPFILENAME,
		   "%s",TMP_DIR "/tmpsliXXXXXX");
	if ( this->pid != -1 ) goto quit;

	efd = mkstemp(tmp_filename);
	if ( efd == -1 ) {
            err_throw(__FUNCTION__,"FATAL","mkstemp() failed");
	}
	c_strcpy(this->tmpfile,tmp_filename);

	if ( ( this->pid=c_fork() ) < 0 ) {
	    c_close(efd);
	    efd=-1;
	    c_remove(this->tmpfile);
	    this->tmpfile[0] = '\0';
            err_throw(__FUNCTION__,"FATAL","fork() failed");
	}
	if ( this->pid == 0 ) {		/* child process */
	    pid_t ppid;
	    if ( argv_is_argv == true ) {
		const char **editor_argv = 
		    get_argv_from_argv(argv,tmp_filename);
		if ( editor_argv != NULL ) {
		    c_execvp( *editor_argv, (char *const *)editor_argv );
		    free(editor_argv);
		}
	    }
	    else {
		char **editor_argv = 
		    get_argv_from_env("EDITOR",DEFAULT_EDITOR,tmp_filename);
		if ( editor_argv != NULL ) {
		    c_execvp( *editor_argv, editor_argv );
		    free(editor_argv[0]);
		    free(editor_argv);
		}
	    }
            err_report(__FUNCTION__,"ERROR","Cannot execute editor");
	    c_remove(tmp_filename);
	    ppid = c_getppid();
	    kill(ppid,SIGTERM);
	    c__exit(-1);
	}
	/* parent process */
	/* waiting for child */
        while ( waitpid(this->pid,&status,0) != this->pid );
	this->pid = -1;

	this->fp = c_fdopen(efd,"r");
	if ( this->fp == NULL ) {
	    c_close(efd);
	    efd=-1;
	    c_remove(this->tmpfile);
	    this->tmpfile[0] = '\0';
	    goto quit;
	}

	return_status = 0;
    }
    else if ( c_strcmp(mode,"r") == 0 && this->fp == NULL && c_isatty(0) == 0 ) {
	efd = c_dup(0);
	if ( efd == -1 ) {
            err_throw(__FUNCTION__,"FATAL","dup() failed");
	}
	this->fp = c_fdopen(efd,"r");
	if ( this->fp == NULL ) {
	    goto quit;
	}

	return_status = 0;
    }

 quit:
    if ( return_status == -1 ) {
	if ( efd != -1 ) c_close(efd);
    }
    return return_status;
}

/**
 * @brief  ���ȥ꡼��Υ����ץ�
 *
 *  mode �� "r" �����ꤵ�줿���ϥ��ǥ������̤��ư���ե�������������
 *  ���Ϥ��줿���Ƥ��ɤ߹��ߤޤ���"w" �����ꤵ�줿���ϥڡ�����򥪡��ץ�
 *  ���ޤ���
 *
 * @param      mode  ���ǥ����ޤ��ϥڡ�����򳫤��⡼�� ("r" or "w")
 * @param      path  ���ǥ����ޤ��ϥڡ�����
 * @return     ������������0<br>
 *             ���顼�ξ�������
 * @throw      �ǡ����ɤ߹����Ѥ��ΰ�γ��ݤ˼��Ԥ������
 * @throw      �ץ����������˼��Ԥ������
 * @throw      �ɤ߽Ф��Ѥ���ӽ񤭹����ѤΥե����롦�ǥ�������ץ���������
 *             ���Ԥ������
 */
int termscreenio::open( const char *mode, const char *path )
{
    if ( mode == NULL ) return -1;
    if ( mode[0] == 'w' ) {
	const char *argv[4] = {"/bin/sh","-c",path,NULL};
	if ( path == NULL ) argv[0] = NULL;
	return this->open( mode, argv );
    }
    else {
	tarray_tstring sobj;
	sobj.split(path," ",false,NULL,'\\',true);
	return this->open( mode, sobj.cstrarray() );
    }
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
int termscreenio::open( const char *mode, int fd )
{
    assert( mode != NULL );

    return this->cstreamio::open(mode,fd);
}

/**
 * @brief  ���ȥ꡼��Υ����ץ�
 *
 *  mode �� "r" �����ꤵ�줿���ϥ��ǥ������̤��ư���ե�������������
 *  ���Ϥ��줿���Ƥ��ɤ߹��ߤޤ���"w" �����ꤵ�줿���ϥڡ�����򥪡��ץ�
 *  ���ޤ���
 *
 * @param      mode ���ȥ꡼��򳫤��⡼�� ("r" or "w")
 * @return     ������������0<br>
 *             ���顼�ξ�������
 * @throw      �ǡ����ɤ߹����Ѥ��ΰ�γ��ݤ˼��Ԥ������
 * @throw      �ץ����������˼��Ԥ������
 * @throw      �ɤ߹����Ѥ���ӽ񤭹����ѤΥե����롦�ǥ�������ץ���������
 *             ���Ԥ������
 */
int termscreenio::open( const char *mode )
{
    assert( mode != NULL );

    return this->open( mode, (char *)NULL );
}

/**
 * @brief  ���ȥ꡼��Υ�����
 *
 *  open()�ǳ��������ȥ꡼��򥯥������ޤ���
 *
 * @return     ������������0<br>
 *             ���顼�ξ���0�ʳ�����
 */
int termscreenio::close()
{
    int return_status = 0;

    if ( this->fp != NULL ) {
	return_status = c_fclose(this->fp);
	this->fp = NULL;
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
    if ( c_strcmp(this->tmpfile,"") != 0 ) {
	c_remove(this->tmpfile);
	this->tmpfile[0] = '\0';
    }

    this->cstreamio::close();

    return return_status;
}

}	/* namespace sli */


#include "private/c_close.c"
#include "private/c_pipe.c"
#include "private/c_fork.c"
#include "private/c_dup.c"
#include "private/c_dup2.c"
#include "private/c_execvp.c"
#include "private/c_getppid.c"
#include "private/c_isatty.c"
#include "private/c__exit.c"

#include "private/c_fdopen.c"
#include "private/c_snprintf.c"
#include "private/c_fclose.c"
#include "private/c_remove.c"

#include "private/c_strcpy.c"
#include "private/c_strlen.cc"
#include "private/c_strcmp.cc"

#include "private/get_argv_from_env.c"
#include "private/get_argv_from_argv.c"
