/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2013-06-05 11:25:20 cyamauch> */

#ifndef _SLI__PIPESTREAMIO_H
#define _SLI__PIPESTREAMIO_H 1

/**
 * @file   pipestreamio.h
 * @brief  �ѥ��פ�Ȥä����ȥ꡼�������Ϥ�Ԥʤ� pipestreamio ���饹�����
 */

#include "cstreamio.h"

#include <sys/types.h>

namespace sli
{

/*
 * pipestreamio class -- read or write using pipe connections.
 *                       (concrete derived class of cstreamio)
 *
 * linker options : none
 *
 */

/**
 * @class  sli::pipestreamio
 * @brief  �ѥ��פ�Ȥä����ȥ꡼�������Ϥ�Ԥʤ�
 *
 *   pipestreamio���饹�ϡ��ե������¹Ԥ������μ¹Ԥ����ץ����ȥѥ��פ�
 *   �Ĥʤ������ȥ꡼�����Ϥޤ��Ͻ��Ϥ�Ԥʤ��ޤ���
 *
 * @attention  LIBC �� stdio.h ��ʻ�Ѥ����硤pipestreamio.h �������� 
 *             stdio.h �� include ���Ƥ���������
 * @attention  ��ݴ��쥯�饹�Ǥ��� cstreamio ��Ѿ����Ƥ��ޤ���<br>
 *             ����Ū�� API �ˤĤ��Ƥ� cstreamio ���饹�� public ���дؿ��Ȥ�
 *             ���������Ƥ��ޤ��Τǡ��������������������
 * @author  Chisato YAMAUCHI
 * @date  2013-04-01 00:00:00
 */

  class pipestreamio : public cstreamio
  {

  public:
    /* constructor & destructor */
    pipestreamio();
    ~pipestreamio();

    /*
     * overridden member functions:  See cstreamio.h for details.
     */
    /* execute a command given by `path', and r/w data via pipe */
    int open( const char *mode, const char *path );
    int open( const char *mode, int fd );
    int open( const char *mode );
    int close();

    /*
     * added member functions
     */
    /* execute a command given by `argv', and r/w data via pipe */
    virtual int open( const char *mode, const char *const argv[] );

  private:
    int pfds[2];			/* �������� */
    pid_t pid;

  };

/**
 * @example  examples_sllib/gnuplot_animation.cc
 *           gnuplot �ǥѥ�����³�������˥᡼������ɽ�����륳����
 */

}

#endif	/* _SLI__PIPESTREAMIO_H */
