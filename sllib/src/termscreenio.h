/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2013-04-27 20:18:52 cyamauch> */

#ifndef _SLI__TERMSCREENIO_H
#define _SLI__TERMSCREENIO_H 1

/**
 * @file   termscreenio.h
 * @brief  ���ǥ������ڡ������ͳ�ǥ��ȥ꡼��򰷤� termscreenio ���饹�����
 */

#include "cstreamio.h"

#include <sys/types.h>

#define TERMSCREENIO__SIZE_TMPFILENAME 128

namespace sli
{

/*
 * termscreenio class -- input/output on a tty screen.
 *                       execute pager for write, execute editor for read.
 *                       (concrete derived class of cstreamio)
 *
 * linker options : none
 *
 */

/**
 * @class  sli::termscreenio
 * @brief  ���ϻ��ϥڡ���������ϻ��ϥ��ǥ��������Ѥ������ȥ꡼��������
 *
 *   termscreenio���饹�ϡ������ߥʥ��ǤΥ��ǥ�����ͳ�Ǥ����ϡ��ڡ�����ؤ�
 *   ���Ϥ�Ԥʤ��ޤ���w3m �Τ褦�ʥġ��������������Ǥ���
 *
 * @attention  LIBC �� stdio.h ��ʻ�Ѥ����硤termstreamio.h ��������
 *             stdio.h �� include ���Ƥ���������
 * @attention  ��ݴ��쥯�饹�Ǥ��� cstreamio ��Ѿ����Ƥ��ޤ���<br>
 *             ����Ū�� API �ˤĤ��Ƥ� cstreamio ���饹�� public ���дؿ��Ȥ�
 *             ���������Ƥ��ޤ��Τǡ��������������������
 * @author  Chisato YAMAUCHI
 * @date  2013-04-01 00:00:00
 */

  class termscreenio : public cstreamio
  {

  public:
    /* constructor & destructor */
    termscreenio();
    ~termscreenio();

    /*
     * overridden member functions:  See cstreamio.h for details.
     */
    /* set editor command to path for "r" mode, pager command for "w" mode */
    int open( const char *mode, const char *path );
    int open( const char *mode, int fd );
    int open( const char *mode );
    int close();

    /*
     * added member functions
     */
    /* set editor command to argv for "r" mode, pager command for "w" mode */
    virtual int open( const char *mode, const char *const argv[] );

  private:
    int pfds[2];			/* �������� */
    pid_t pid;
    char tmpfile[TERMSCREENIO__SIZE_TMPFILENAME];

  };
}

#ifndef _SLI__TERMSCREENIO_CC
#undef TERMSCREENIO__SIZE_TMPFILENAME
#endif

#endif	/* _SLI__TERMSCREENIO_H */
