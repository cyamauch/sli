/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2013-04-27 19:48:24 cyamauch> */

#ifndef _SLI__CTINDEX_H
#define _SLI__CTINDEX_H 1

/**
 * @file   ctindex.h
 * @brief  ����ʸ������ֹ�Ȥδط���������륯�饹 ctindex �Υإå��ե�����
 */

#include <stddef.h>
#include <sys/types.h>

#include "slierr.h"

/*
 * Character Tree Index
 *
 * ʸ���� �� index(size_t) �κ�����Character Tree ��Ȥäƺ�롥
 *
 */

namespace sli
{

/*
 * sli::ctindex class manages the relationship between the key string and the
 * index.  Once the relationship between a string (key) and a number (index) is
 * registered, the index can be retrieved quickly by using key string.  Its
 * behavior is similar to that of hash, but no collision occurs because this 
 * class manages the relationship between the key string and the index with a 
 * dictionary.
 */

/**
 * @class  sli::ctindex
 * @brief  ����ʸ����ȥ���ǥå���(���ʤ�����)�Ȥδط���������륯�饹
 *
 *   ctindex ���饹�ϡ�����ʸ����ȥ���ǥå���(���ʤ�����)�Ȥδط���
 *   ���ڡ׹�¤�ǵ�Ͽ������®�ʸ���(����ʸ���󤫤������ֹ�ؤ��Ѵ�)����ǽ�Ǥ���
 *   Ʊ��Υ���ʸ������Ф���ʣ���Υ���ǥå�������Ͽ�������Ǥ��ޤ���<br>
 *   �����ˤ�������֤ϡ���Ͽ���줿����ʸ����ο��ǤϤʤ����ơ��Υ���ʸ����
 *   ��Ĺ�������㤷�ޤ�(����ʸ�����Ĺ�����������ۤ�®��)�������������ڡ׹�¤
 *   �Ǥ��Τǡ������ˤϤ����֤�������ޤ���
 *   
 * @author  Chisato YAMAUCHI
 * @date  2013-04-01 00:00:00
 */

  class ctindex
  {

  public:
    /* constructor and destructor */
    ctindex();
    ctindex(const ctindex &obj);
    virtual ~ctindex();

    /*
     * operator
     */
    virtual ctindex &operator=(const ctindex &obj);

    /*
     * member functions
     */
    /* complete initialization of objects */
    virtual ctindex &init();
    virtual ctindex &init(const ctindex &obj);

    /* append an index */
    virtual int append( const char *key, size_t index );

    /* update an index associate by a key */
    virtual int update( const char *key, 
			size_t current_index, size_t new_index );

    /* erase an index */
    virtual int erase( const char *key, size_t index );

    /* returns an index */
    virtual ssize_t index( const char *key, int index_of_index = 0 ) const;

    /* swap self and another */
    virtual ctindex &swap( ctindex &sobj );

    /* shallow copy ����Ĥ�����˻Ȥ� (̤����) */
    virtual void set_scopy_flag();

  private:
    ctindex *configure_ctable( int ch );
    int realloc_index_rec( size_t len_elements );
    void free_index_rec();
    int realloc_ctable_rec( size_t len_elements );
    void free_ctable_rec();
    bool request_shallow_copy( ctindex *from_obj ) const;

  private:
    size_t *_index_rec;		/* ��Ū�� index */
    size_t index_alloc_blen_rec;
    int index_rec_size;		/* index_rec �ο� */
    int ctable_begin_ch;	/* ctable �γ��ϥ���饯��('A'�Ȥ�) */
    int ctable_size;		/* ctable �Υ�����('A' ���� 'E' �ʤ� 5) */
    ctindex **_ctable_rec;
    size_t ctable_alloc_blen_rec;

    /* shallow copy �Τ���Υե饰��= ���Ǥϥ��ԡ�����ʤ� */
    /* ���ԡ��� src ¦�Υե饰 */
    bool shallow_copy_ok;		/* set_scopy_flag() �ǥ��åȤ���� */
    ctindex *shallow_copy_dest_obj;	/* ���ԡ���: ͭ���ʤ� non-NULL */

    /* ���ԡ��� dest ¦�Υե饰(���ԡ�������񤭹��ޤ��) */
    ctindex *shallow_copy_src_obj;	/* ���ԡ���: �����Ƥ���� non-NULL */

    /* __shallow_init(), __deep_init() ������˹Ԥʤ�����Υե饰 */
    bool __copying;

  };
}

#endif  /* _SLI__CTINDEX_H */
