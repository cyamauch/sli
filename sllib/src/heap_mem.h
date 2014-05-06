/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2013-05-30 12:42:30 cyamauch> */

#ifndef _SLI__HEAP_MEM_H
#define _SLI__HEAP_MEM_H 1

/**
 * @file   heap_mem.h
 * @brief  ����Ū�ʥ�������Τ���Υ��饹 heap_mem �Υ�����
 */

#include <stdlib.h>
#include <stddef.h>

namespace sli
{

/**
 * @class  sli::heap_mem
 * @brief  ����Ū�ʥ�������Τ���Υƥ�ץ졼�ȥ��饹
 *
 *         heap_mem �ƥ�ץ졼�ȥ��饹�ϡ��ץ�ߥƥ��ַ��ȹ�¤�Τθ���Ū������
 *         �򰷤������Ǥ��ޤ���strdup()��asprintf() �Τ褦�� C����δؿ�������
 *         �����ɥ쥹����Ͽ�����������פ�ȴ����ȼ�ưŪ�˥�������Ǥ���
 *         �����ȥݥ��󥿤Ȥ��Ƥ����Ѳ�ǽ�Ǥ���
 *
 * @note  ���饹��ܤ������ std::vector �� sli::tarray ��Ȥ�����
 * @author  Chisato YAMAUCHI
 * @date  2013-04-01 00:00:00
 */

template <class datatype> class heap_mem
{

    /* 
     * constructor and destructor
     */

public:
    /**
     * @brief  ���󥹥ȥ饯��
     */
    heap_mem()
    {
	this->ptr_rec = NULL;
    };

private:
    /* DO NOT USE THIS */
    /**
     * @deprecated  �����Բ�
     */
    heap_mem(const heap_mem &obj)
    {
	this->ptr_rec = NULL;
    };

public:
    /**
     * @brief  �ǥ��ȥ饯��
     */
    virtual ~heap_mem()
    {
	if ( this->ptr_rec != NULL ) free(this->ptr_rec);
    };

    /*
     * operators
     */

private:
    /* DO NOT USE THIS */
    /**
     * @deprecated  �����Բ�
     */
    virtual heap_mem &operator=(const heap_mem &obj)
    {
	return *this;
    };

public:
    /* use this to register external heap pointer such as */
    /* return value of strdup().                          */
    /* Give NULL to free registered area.                 */
    /**
     * @brief  �ҡ����ΰ�Υ��ɥ쥹����Ͽ
     */
    virtual datatype *operator=(datatype *p)
    {
	if ( this->ptr_rec != NULL ) free(this->ptr_rec);
	this->ptr_rec = p;
	return p;
    };

    /* access an element */
    /**
     * @brief  1���ǤؤΥ������� (�Хåե����������å�̵��)
     */
    virtual inline datatype &operator[]( size_t index )
    {
	return this->ptr_rec[index];
    };
    /* access an element */
    /**
     * @brief  1���ǤؤΥ������� (�ɼ����ѡ��Хåե����������å�̵��)
     */
    virtual inline const datatype &operator[]( size_t index ) const
    {
	return this->ptr_rec[index];
    };

    /*
     * member functions
     */

public:
    /* swap contents between two objects */
    /**
     * @brief  2�ĤΥ��֥������ȴ֤Ǥ����ƤΥ���å�
     */
    virtual heap_mem &swap(heap_mem &obj)
    {
	datatype *tmp__ptr_rec;
	tmp__ptr_rec = this->ptr_rec;
	this->ptr_rec = obj.ptr_rec;
	obj.ptr_rec = tmp__ptr_rec;
	return *this;
    }

    /* simple free() and malloc().  Only free() is called when len=0. */
    /* This returns negative value when malloc() is failed            */
    /**
     * @brief  �ҡ��ץ���γ���(malloc()�ؿ��θƽ�)�ȳ���
     */
    virtual int allocate(size_t len)
    {
	if ( this->ptr_rec != NULL ) {
	    free(this->ptr_rec);
	    this->ptr_rec = NULL;
	}
	if ( len == 0 ) return 0;
	else {
	    this->ptr_rec = (datatype *)malloc(sizeof(datatype)*len);
	    if ( this->ptr_rec == NULL ) return -1;
	    else return 0;
	}
    };

    /* simple realloc().                                    */
    /* This returns negative value when realloc() is failed */
    /**
     * @brief  �ҡ��ץ���κƳ���(realloc()�ؿ��θƽ�)
     */
    virtual int reallocate(size_t len)
    {
	void *tmp_ptr;
	if ( this->ptr_rec == NULL && len == 0 ) return 0;
	tmp_ptr = realloc(this->ptr_rec, sizeof(datatype)*len);
	if ( tmp_ptr == NULL && 0 < len ) return -1;
	else {
	    this->ptr_rec = (datatype *)tmp_ptr;
	    return 0;
	}
    };

    /* execute malloc() and store 16-byte aligned pointer into *ret_ptr16. */
    /* This returns negative value when malloc() is failed.                */
    /**
     * @brief  �ҡ��ץ���γ��� (16-byte aligned ���ɥ쥹�������)
     */
    virtual int allocate_aligned16(size_t len, datatype **ret_ptr16)
    {
	if ( this->ptr_rec != NULL ) {
	    free(this->ptr_rec);
	    this->ptr_rec = NULL;
	}
	if ( len == 0 ) {
	    *ret_ptr16 = NULL;
	    return 0;
	}
	else {
	    this->ptr_rec = (datatype *)malloc(16 + sizeof(datatype)*len);
	    if ( this->ptr_rec == NULL ) {
		*ret_ptr16 = NULL;
		return -1;
	    }
	    else {
		size_t mm = ((size_t)(this->ptr_rec) & 0x0f);
		if ( mm == 0 ) *ret_ptr16 = this->ptr_rec;
		else {
		    mm = 16 - mm;
		    *ret_ptr16 = (datatype *)((char *)(this->ptr_rec) + mm);
		}
		return 0;
	    }
	}
    };

    /* execute malloc() and store 32-byte aligned pointer into *ret_ptr32. */
    /* This returns negative value when malloc() is failed.                */
    /**
     * @brief  �ҡ��ץ���γ��� (32-byte aligned ���ɥ쥹�������)
     */
    virtual int allocate_aligned32(size_t len, datatype **ret_ptr32)
    {
	if ( this->ptr_rec != NULL ) {
	    free(this->ptr_rec);
	    this->ptr_rec = NULL;
	}
	if ( len == 0 ) {
	    *ret_ptr32 = NULL;
	    return 0;
	}
	else {
	    this->ptr_rec = (datatype *)malloc(32 + sizeof(datatype)*len);
	    if ( this->ptr_rec == NULL ) {
		*ret_ptr32 = NULL;
		return -1;
	    }
	    else {
		size_t mm = ((size_t)(this->ptr_rec) & 0x0f);
		if ( mm == 0 ) *ret_ptr32 = this->ptr_rec;
		else {
		    mm = 32 - mm;
		    *ret_ptr32 = (datatype *)((char *)(this->ptr_rec) + mm);
		}
		return 0;
	    }
	}
    };

    /* simple memset                                       */
    /* This returns negative value when error is detected. */
    /**
     * @brief  1�Ĥ��ͤǥҡ��ץ����ѥǥ���
     */
    virtual inline int fill(datatype value, size_t len)
    {
	if ( 0 < len && this->ptr_rec == NULL ) return -1;
	else {
	    datatype *dest = this->ptr_rec;
	    while ( 0 < len ) {
		len --;
		*dest = value;  dest ++;
	    }
	    return 0;
	}
    };
    /* simple memset                                       */
    /* This returns negative value when error is detected. */
    /**
     * @brief  1�Ĥ��ͤǥҡ��ץ����ѥǥ���
     */
    virtual inline int fill(size_t begin, datatype value, size_t len)
    {
	if ( 0 < len && this->ptr_rec == NULL ) return -1;
	else {
	    datatype *dest = this->ptr_rec + begin;
	    while ( 0 < len ) {
		len --;
		*dest = value;  dest ++;
	    }
	    return 0;
	}
    };

    /* simple memory copy: from ext to in.                 */
    /* This returns negative value when error is detected. */
    /**
     * @brief  �����Хåե����鼫�ȤؤΥ��ԡ�
     */
    virtual inline int copyfrom(const datatype *src, size_t len)
    {
	if ( 0 < len && (this->ptr_rec == NULL || src == NULL) ) return -1;
	else {
	    datatype *dest = this->ptr_rec;
	    while ( 0 < len ) {
		len --;
		*dest = *src;  dest ++;  src ++;
	    }
	    return 0;
	}
    };

    /* simple memory copy: from in to ext.                 */
    /* This returns negative value when error is detected. */
    /**
     * @brief  ���Ȥ��鳰���Хåե��ؤΥ��ԡ�
     */
    virtual inline int copyto(datatype *dest, size_t len) const
    {
	if ( 0 < len && (this->ptr_rec == NULL || dest == NULL) ) return -1;
	else {
	    const datatype *src = this->ptr_rec;
	    while ( 0 < len ) {
		len --;
		*dest = *src;  dest ++;  src ++;
	    }
	    return 0;
	}
    };
    /**
     * @brief  ���Ȥ��鳰���Хåե��ؤΥ��ԡ�
     */
    virtual inline int copyto(size_t begin, datatype *dest, size_t len) const
    {
	if ( 0 < len && (this->ptr_rec == NULL || dest == NULL) ) return -1;
	else {
	    const datatype *src = this->ptr_rec + begin;
	    while ( 0 < len ) {
		len --;
		*dest = *src;  dest ++;  src ++;
	    }
	    return 0;
	}
    };

    /* returns registered pointer value that points to a heap area */
    /**
     * @brief  ��Ͽ����Ƥ���ҡ����ΰ�Υ��ɥ쥹���֤�
     */
    virtual inline datatype *ptr()
    {
	return this->ptr_rec;
    };
    /**
     * @brief  ��Ͽ����Ƥ���ҡ����ΰ�Υ��ɥ쥹���֤� (�ɼ�����)
     */
    virtual inline const datatype *ptr() const
    {
	return this->ptr_rec;
    };

    /* programmers can use address of internal pointer value to register */
    /* a pointer when operator= cannot be used such as asprintf().       */
    /* Here is an example with asprintf():                               */
    /*   heap_mem<char> buf;                                             */
    /*       :                                                           */
    /*   buf = NULL;                         <= free current area        */
    /*   asprintf(buf.ptr_address(), ... );  <= register new area        */
    /**
     * @brief  �ҡ����ΰ�Υ��ɥ쥹���ݻ���������ѿ��Υ��ɥ쥹���֤�
     *
     *  asprintf() �ʤɤξ��˻��Ѥ��ޤ���<br>
     *  �����<br>
     *  heap_mem<char> buf;                                       <br>
     *  :                                                         <br>
     *  buf = NULL;             <= ���ߴ������Ƥ���Хåե����� <br>
     *  asprintf(buf.ptr_address(), ... );  <= �������ҡ��פ���Ͽ
     */
    virtual inline datatype **ptr_address()
    {
	return &(this->ptr_rec);
    };

    /*
     * member
     */

private:
    datatype *ptr_rec;

};

}	/* namespace sli */

#endif	/* _SLI__HEAP_MEM_H */
