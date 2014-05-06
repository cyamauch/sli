/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2013-04-27 20:20:27 cyamauch> */

#ifndef _SLI__TREGEX_H
#define _SLI__TREGEX_H 1

/**
 * @file   tregex.h
 * @brief  POSIX��ĥ����ɽ���򰷤�����δ���Ū�ʥ��饹 tregex �����
 */

#include <stddef.h>

#include "slierr.h"

namespace sli
{

/*
 * sli::tregex class manages compiled data of a POSIX extended regular 
 * expression pattern.
 * use this class to optimize performance of pattern matching of strings
 * with sli::tstring, sli::tarray_tstring, etc.
 */

/**
 * @class  sli::tregex
 * @brief  POSIX��ĥ����ɽ���򰷤�����δ���Ū�ʥ��饹
 * 
 *   tregex���饹�ϡ����ꤵ�줿 POSIX ��ĥ����ɽ���򥳥�ѥ��뤷�����η�̤�
 *   ��¸����������Ԥʤ������Ǥ��ޤ���
 *
 * @note  �����Ǥ� LIBC �� regcomp��regexec ������Ѥ��Ƥ��ޤ���
 * @author  Chisato YAMAUCHI
 * @date  2013-04-01 00:00:00
 */

  class tregex
  {

  public:
    /* constructor and destructor */
    tregex();
    tregex(const char *regstr);
    tregex(const tregex &obj);
    virtual ~tregex();

    /*
     * operator
     */
    virtual tregex &operator=(const tregex &obj);
    virtual tregex &operator=(const char *regstr);

    /*
     * member functions
     */
    /* complete initialization of objects */
    virtual tregex &init();
    virtual tregex &init(const tregex &obj);

    /* compile regular expression */
    virtual int compile(const char *regstr);

    /* swap self and another */
    virtual tregex &swap(tregex &sobj);

    /* returns regular expression pattern string stored in object */
    virtual const char *cstr() const;

    /* returns preg_rec                             */
    /* returning NULL indicates error in .compile() */
    virtual const void *cregex() const;

    /* execute pattern matching                                           */
    /*   str:       string on which to perform matching                   */
    /*   notbol:    set true to fail the match-beginning-of-line operator */
    /*   noteol:    set true to fail the match-end-of-line operator       */
    /*   max_nelem: buffer length of pos_r or len_r                       */
    /*   pos_r:     position of matched substring                         */
    /*   len_r:     length of matched substring                           */
    /*   nelem_r:   length of returned elements of pos_r or len_r         */
    virtual int exec( const char *str, bool notbol, bool noteol,
		      size_t max_nelem, size_t pos_r[], size_t len_r[],
		      size_t *nelem_r ) const;

    /* returns error message of compile */
    virtual const char *cerrstr() const;

    /* ������֥������Ȥ� return ��ľ���˻Ȥ���shallow copy ����Ĥ������ */
    /* �Ȥ� (̤����)                                                         */
    virtual void set_scopy_flag();

  private:
    bool request_shallow_copy( tregex *from_obj ) const;

  private:
    char *regstr_rec;				/* required free() */
    void *preg_rec;				/* requires c_regfree() */
    char errstr_rec[192];

    /* shallow copy �Τ���Υե饰��= ���Ǥϥ��ԡ�����ʤ� */
    /* ���ԡ��� src ¦�Υե饰 */
    bool shallow_copy_ok;		/* set_scopy_flag() �ǥ��åȤ���� */
    tregex *shallow_copy_dest_obj;	/* ���ԡ���: ͭ���ʤ� non-NULL */

    /* ���ԡ��� dest ¦�Υե饰(���ԡ�������񤭹��ޤ��) */
    tregex *shallow_copy_src_obj;	/* ���ԡ���: �����Ƥ���� non-NULL */

    /* __shallow_init(), __deep_init() ������˹Ԥʤ�����Υե饰 */
    bool __copying;

  };
}

#endif	/* _SLI__TREGEX_H */
