/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2013-06-05 11:25:55 cyamauch> */

#ifndef _SLI__TERMLINEIO_H
#define _SLI__TERMLINEIO_H 1

/**
 * @file   termlineio.h
 * @brief  GNU readline���Ф���wrapper  termlineio ���饹�����
 */

#include "cstreamio.h"
#include "heap_mem.h"

#include <sys/types.h>

#define TERMLINEIO__SIZE_MODESTRING 8

namespace sli
{

/*
 * termlineio class -- line-based tty input/output.
 *                     wrapper for GNU readline.
 *                     (concrete derived class of cstreamio)
 *
 * linker options : -lreadline -lncurses
 *
 */

/**
 * @class  sli::termlineio
 * @brief  GNU readline���Ф���wrapper
 *
 *   termlineo���饹�ϡ��ɤ߹��ߥ⡼�ɤˤ����ơ�GNU readline�饤�֥������Ѥ�
 *   �����ޥ�����Ϥ˴ؤ���ٱ��Ԥʤ��ޤ���<br>
 *   �ɤ߹��ߥ⡼�ɤǤ� open() ���дؿ��� path �����˥��ޥ�ɤ�����ե������
 *   Ϳ���ޤ����񤭹��ߥ⡼�ɤǤϡ�termscreenio���饹�ξ���Ʊ�ͤ�ư��Ǥ�
 *   (�ڡ������ư���ޤ�)��
 *
 * @attention  LIBC �� stdio.h ��ʻ�Ѥ����硤termlineio.h �������� stdio.h
 *             �� include ���Ƥ���������
 * @attention  ��ݴ��쥯�饹�Ǥ��� cstreamio ��Ѿ����Ƥ��ޤ���<br>
 *             ����Ū�� API �ˤĤ��Ƥ� cstreamio ���饹�� public ���дؿ��Ȥ�
 *             ���������Ƥ��ޤ��Τǡ��������������������
 * @author  Chisato YAMAUCHI
 * @date  2013-04-01 00:00:00
 */

  class termlineio : public cstreamio
  {

  public:
    /* constructor & destructor */
    termlineio();
    ~termlineio();

    /*
     * overridden member functions:  See cstreamio.h for details.
     */
    int getchr();
    char *getstr( char *s, size_t size );
    ssize_t read( void *buf, size_t size );

    /* set history file to `path' arg for read mode, and set pager command */
    /* for write mode.                                                     */
    int open( const char *mode, const char *path );
    int open( const char *mode, int fd );
    int open( const char *mode );

    int close();

    /*
     * added member functions
     */
    /* set history file to `path' arg for read mode, and set pager command */
    /* for write mode.                                                     */
    virtual int open( const char *mode, const char *const argv[] );

    /* set prompt for read mode. default is zero-length prompt */
    virtual termlineio &set_prompt( const char *prompt );
    virtual termlineio &setf_prompt( const char *format, ... );
    virtual termlineio &vsetf_prompt( const char *fmt, va_list ap );

    /* set true to `tf' arg to enable auto-save of history */
    virtual termlineio &automate_history( bool tf );

    /* add a command line to history buffer */
    virtual termlineio &add_history( const char *line );

    /* erase all entries in history buffer */
    virtual termlineio &clear_history();

    /* set the limit of entries of history buffer */
    virtual termlineio &stifle_history( int num_lines );

    /* disable the limit set by stifle_history() */
    virtual termlineio &unstifle_history();

    /* read and write a history file */
    virtual int read_history( const char *path );
    virtual int readf_history( const char *format, ... );
    virtual int vreadf_history( const char *fmt, va_list ap );
    virtual int write_history( const char *path );
    virtual int writef_history( const char *format, ... );
    virtual int vwritef_history( const char *fmt, va_list ap );

  private:
    char *fill_read_buffer();

  private:
    char mode[TERMLINEIO__SIZE_MODESTRING];
    int pfds[2];			/* �������� */
    pid_t pid;
    /* for readline */
    heap_mem<char> path;
    heap_mem<char> prompt;
    bool auto_history;			/* ��ư�ǥҥ��ȥ���ɲä��뤫 */
    bool rl_eof;			/* readline �� EOF �򸡽Ф����� */
    /* read(),getxxx() �ǻ��� */
    heap_mem<char> rl_read_buffer;
    char *rl_read_buffer_next_pointer;	/* �����ɤ�٤�����(���ɤʤ�NULL) */
    size_t rl_read_buffer_read_length;	/* ��Ǽ����Ƥ���ʸ����Ĺ */
    size_t rl_read_buffer_size;		/* �Хåե������� */

  };


/**
 * @example  examples_sllib/readline.cc
 *           GNU readline ��Ȥäƥ����ߥʥ뤫�饳�ޥ�����Ϥ�����դ�����
 */

}

#ifndef _SLI__TERMLINEIO_CC
#undef TERMLINEIO__SIZE_MODESTRING
#endif

#endif	/* _SLI__TERMLINEIO_H */
