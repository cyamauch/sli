/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2009-01-10 15:27:57 cyamauch> */

#ifndef _TARRAY_PLAIN__TYPE
# error "_TARRAY_PLAIN__TYPE is not defined"
#endif
#ifndef _TARRAY_PLAIN
# error "_TARRAY_PLAIN is not defined"
#endif

#ifdef _TARRAY_PLAIN__USE_VARG
# ifndef _TARRAY_PLAIN__VATYPE
#  error "_TARRAY_PLAIN__VATYPE is not defined"
# endif
#endif

/* header */
#ifndef _TARRAY_PLAIN__GET_SOURCE

#include <stddef.h>
#include <stdarg.h>
#include <sys/types.h>

#include "tarray_plain.h"
#include "slierr.h"

namespace sli
{
  class _TARRAY_PLAIN
  {
  public:
    _TARRAY_PLAIN();
    _TARRAY_PLAIN(const _TARRAY_PLAIN &obj);
    virtual ~_TARRAY_PLAIN();
    virtual _TARRAY_PLAIN &operator=(const _TARRAY_PLAIN &obj);
#ifdef _TARRAY_PLAIN__USE_OP
    virtual _TARRAY_PLAIN &operator+=(const _TARRAY_PLAIN &obj);
    virtual _TARRAY_PLAIN &operator-=(const _TARRAY_PLAIN &obj);
    virtual _TARRAY_PLAIN &operator*=(const _TARRAY_PLAIN &obj);
    virtual _TARRAY_PLAIN &operator/=(const _TARRAY_PLAIN &obj);
    virtual _TARRAY_PLAIN &operator+=(const _TARRAY_PLAIN__TYPE &val);
    virtual _TARRAY_PLAIN &operator-=(const _TARRAY_PLAIN__TYPE &val);
    virtual _TARRAY_PLAIN &operator*=(const _TARRAY_PLAIN__TYPE &val);
    virtual _TARRAY_PLAIN &operator/=(const _TARRAY_PLAIN__TYPE &val);
#endif
    virtual bool operator==(const _TARRAY_PLAIN &obj) const;
    virtual bool operator!=(const _TARRAY_PLAIN &obj) const;
    virtual _TARRAY_PLAIN &init();
    virtual _TARRAY_PLAIN &init(const _TARRAY_PLAIN &obj);
    virtual _TARRAY_PLAIN &assign_default(const _TARRAY_PLAIN__TYPE &val);
    virtual _TARRAY_PLAIN &clean();
    virtual _TARRAY_PLAIN &clean(const _TARRAY_PLAIN__TYPE &val);
#ifdef TARRAY_PLAIN__USE_SOLO_NARG
    virtual _TARRAY_PLAIN &assign( size_t n );
#endif
    virtual _TARRAY_PLAIN &assign(const _TARRAY_PLAIN__TYPE &val, size_t n);
    virtual _TARRAY_PLAIN &assign(const _TARRAY_PLAIN__TYPE vals[], size_t n);
    virtual _TARRAY_PLAIN &assign(const _TARRAY_PLAIN &src, size_t idx2 = 0);
    virtual _TARRAY_PLAIN &assign( const _TARRAY_PLAIN &src, 
				   size_t idx2, size_t n2 );
#ifdef _TARRAY_PLAIN__USE_VARG
    virtual _TARRAY_PLAIN &assign( size_t n, 
		   _TARRAY_PLAIN__VATYPE el0, _TARRAY_PLAIN__VATYPE el1, ... );
    virtual _TARRAY_PLAIN &vassign( size_t n, 
	    _TARRAY_PLAIN__VATYPE el0, _TARRAY_PLAIN__VATYPE el1, va_list ap );
#endif
#ifdef _TARRAY_PLAIN__USE_SPLIT
    virtual _TARRAY_PLAIN &split( const char *line, const char *delims, 
				  bool zero_str,
				  const char *quotations, int escape );
    virtual _TARRAY_PLAIN &split( const char *line, const char *delims, 
				  bool zero_str = false );
#endif
#ifdef TARRAY_PLAIN__USE_SOLO_NARG
    virtual _TARRAY_PLAIN &append( size_t n );
#endif
    virtual _TARRAY_PLAIN &append(const _TARRAY_PLAIN__TYPE &val, size_t n);
    virtual _TARRAY_PLAIN &append(const _TARRAY_PLAIN__TYPE vals[], size_t n);
    virtual _TARRAY_PLAIN &append( const _TARRAY_PLAIN &src, size_t idx2 = 0 );
    virtual _TARRAY_PLAIN &append( const _TARRAY_PLAIN &src, 
				   size_t idx2, size_t n2 );
#ifdef _TARRAY_PLAIN__USE_VARG
    virtual _TARRAY_PLAIN &append( size_t n,
		   _TARRAY_PLAIN__VATYPE el0, _TARRAY_PLAIN__VATYPE el1, ... );
    virtual _TARRAY_PLAIN &vappend( size_t n, 
	    _TARRAY_PLAIN__VATYPE el0, _TARRAY_PLAIN__VATYPE el1, va_list ap );
#endif
#ifdef TARRAY_PLAIN__USE_SOLO_NARG
    virtual _TARRAY_PLAIN &insert( size_t index, size_t n );
#endif
    virtual _TARRAY_PLAIN &insert( size_t index, 
				   const _TARRAY_PLAIN__TYPE &val, size_t n);
    virtual _TARRAY_PLAIN &insert( size_t index, 
				   const _TARRAY_PLAIN__TYPE vals[], size_t n);
    virtual _TARRAY_PLAIN &insert( size_t index, 
				   const _TARRAY_PLAIN &src, size_t idx2 = 0 );
    virtual _TARRAY_PLAIN &insert( size_t index, 
			    const _TARRAY_PLAIN &src, size_t idx2, size_t n2 );
#ifdef _TARRAY_PLAIN__USE_VARG
    virtual _TARRAY_PLAIN &insert( size_t index, size_t n, 
		   _TARRAY_PLAIN__VATYPE el0, _TARRAY_PLAIN__VATYPE el1, ... );
    virtual _TARRAY_PLAIN &vinsert( size_t index, size_t n, 
	    _TARRAY_PLAIN__VATYPE el0, _TARRAY_PLAIN__VATYPE el1, va_list ap );
#endif
#ifdef TARRAY_PLAIN__USE_SOLO_NARG
    virtual _TARRAY_PLAIN &replace( size_t idx1, size_t n1, size_t n2 );
#endif
    virtual _TARRAY_PLAIN &replace( size_t idx1, size_t n1, 
				    const _TARRAY_PLAIN__TYPE &val, size_t n2 );
    virtual _TARRAY_PLAIN &replace( size_t idx1, size_t n1,
				 const _TARRAY_PLAIN__TYPE vals[], size_t n2 );
    virtual _TARRAY_PLAIN &replace( size_t idx1, size_t n1,
				    const _TARRAY_PLAIN &src, size_t idx2 = 0 );
    virtual _TARRAY_PLAIN &replace( size_t idx1, size_t n1,
			    const _TARRAY_PLAIN &src, size_t idx2, size_t n2 );
#ifdef _TARRAY_PLAIN__USE_VARG
    virtual _TARRAY_PLAIN &replace( size_t idx1, size_t n1,
	size_t n2, _TARRAY_PLAIN__VATYPE el0, _TARRAY_PLAIN__VATYPE el1, ... );
    virtual _TARRAY_PLAIN &vreplace( size_t idx1, size_t n1, size_t n2, 
	    _TARRAY_PLAIN__VATYPE el0, _TARRAY_PLAIN__VATYPE el1, va_list ap );
#endif
#ifdef TARRAY_PLAIN__USE_SOLO_NARG
    virtual _TARRAY_PLAIN &put( size_t index, size_t n );
#endif
    virtual _TARRAY_PLAIN &put( size_t index, const _TARRAY_PLAIN__TYPE &val, size_t n);
    virtual _TARRAY_PLAIN &put( size_t index, 
				const _TARRAY_PLAIN__TYPE vals[], size_t n);
    virtual _TARRAY_PLAIN &put( size_t index, 
				const _TARRAY_PLAIN &src, size_t idx2 = 0 );
    virtual _TARRAY_PLAIN &put( size_t index, 
			    const _TARRAY_PLAIN &src, size_t idx2, size_t n2 );
#ifdef _TARRAY_PLAIN__USE_VARG
    virtual _TARRAY_PLAIN &put( size_t index, size_t n, 
		   _TARRAY_PLAIN__VATYPE el0, _TARRAY_PLAIN__VATYPE el1, ... );
    virtual _TARRAY_PLAIN &vput( size_t index, size_t n, 
	    _TARRAY_PLAIN__VATYPE el0, _TARRAY_PLAIN__VATYPE el1, va_list ap );
#endif
    virtual _TARRAY_PLAIN &erase();
    virtual _TARRAY_PLAIN &erase( size_t index, size_t num_elements = 1 );
    virtual _TARRAY_PLAIN &resize( size_t new_num_elements );
    virtual _TARRAY_PLAIN &swap( _TARRAY_PLAIN &sobj );
    /* */
    virtual ssize_t getdata( size_t index, 
			     void *dest_buf, size_t buf_size ) const;
    virtual ssize_t getdata( void *dest_buf, size_t buf_size ) const;
    virtual ssize_t copy( size_t index, size_t n, _TARRAY_PLAIN &dest ) const;
    virtual ssize_t copy( size_t index, _TARRAY_PLAIN &dest ) const;
    virtual ssize_t copy( _TARRAY_PLAIN &dest ) const;
    /* */
    virtual const _TARRAY_PLAIN__TYPE *carray() const;
    virtual _TARRAY_PLAIN__TYPE *data_ptr();
    virtual _TARRAY_PLAIN__TYPE &at( size_t index );
    virtual _TARRAY_PLAIN__TYPE &element( size_t index );
    virtual const _TARRAY_PLAIN__TYPE &at_cs( size_t index ) const;
    virtual ssize_t find( const _TARRAY_PLAIN__TYPE &val ) const;
    virtual ssize_t find( size_t pos, const _TARRAY_PLAIN__TYPE &val ) const;
    virtual ssize_t rfind( const _TARRAY_PLAIN__TYPE &val ) const;
    virtual ssize_t rfind( size_t pos, const _TARRAY_PLAIN__TYPE &val ) const;
    virtual size_t length() const;
    virtual size_t size() const;
    virtual size_t byte_length() const;
  private:
    tarray_plain *arr_rec;
    _TARRAY_PLAIN__TYPE *arr_ptr;
#ifndef TARRAY_PLAIN__USE_SOLO_NARG
  private:
    /*
      gcc でエラーを出すための，ダミーのメソッド． 

      普通の数値を与えると，参照の引数を持つメンバ関数が選ばれてしまい，
      正常に動作しない．これらを private に置く事で，この問題を防止する．
    */
    void assign( const _TARRAY_PLAIN__TYPE &val );
    void append( const _TARRAY_PLAIN__TYPE &val );
    void insert( size_t index, const _TARRAY_PLAIN__TYPE &val );
    void replace( size_t idx1, size_t n1, const _TARRAY_PLAIN__TYPE &val );
    void put( size_t index, const _TARRAY_PLAIN__TYPE &val );
#endif
  };
}

#endif	/* end of header */ /* #ifndef _TARRAY_PLAIN__GET_SOURCE */


/* Source */
#if defined(_TARRAY_PLAIN__GET_SOURCE) || defined(_TARRAY_PLAIN__GET_BOTH)

#ifdef _TARRAY_PLAIN__USE_SPLIT
# ifndef _TARRAY_PLAIN__ATOX
#  error "_TARRAY_PLAIN__ATOX is not defined"
# endif
#endif

#define CLASS_NAME "_TARRAY_PLAIN"

#include <stdlib.h>
#include "tarray_tstring.h"

#include "private/err_report.h"

namespace sli
{

/* constructor */
#define FUNC_NAME "_TARRAY_PLAIN"
_TARRAY_PLAIN::_TARRAY_PLAIN()
{
    this->arr_ptr = NULL;
    try {
	_TARRAY_PLAIN__TYPE zero;
	unsigned char *p = (unsigned char *)&zero;
	size_t i;
	for ( i=0 ; i < sizeof(_TARRAY_PLAIN__TYPE) ; i++ )
	    p[i] = 0;
	this->arr_rec = new tarray_plain(sizeof(_TARRAY_PLAIN__TYPE),
				   (void **)&this->arr_ptr);
	this->arr_rec->assign_default(&zero);
    }
    catch (...) {
        err_throw(FUNC_NAME,"FATAL","new failed");
    }
    return;
}

/* copy constructor */
_TARRAY_PLAIN::_TARRAY_PLAIN(const _TARRAY_PLAIN &obj)
{
    this->arr_ptr = NULL;
    try {
	_TARRAY_PLAIN__TYPE zero;
	unsigned char *p = (unsigned char *)&zero;
	size_t i;
	for ( i=0 ; i < sizeof(_TARRAY_PLAIN__TYPE) ; i++ )
	    p[i] = 0;
	this->arr_rec = new tarray_plain(sizeof(_TARRAY_PLAIN__TYPE),
				   (void **)&this->arr_ptr);
	this->arr_rec->assign_default(&zero);
    }
    catch (...) {
        err_throw(FUNC_NAME,"FATAL","new failed");
    }
    this->init(obj);
    return;
}
#undef FUNC_NAME

/* destructor */
_TARRAY_PLAIN::~_TARRAY_PLAIN()
{
    delete this->arr_rec;
    return;
}

_TARRAY_PLAIN &_TARRAY_PLAIN::operator=(const _TARRAY_PLAIN &obj)
{
    this->init(obj);
    return *this;
}

#ifdef _TARRAY_PLAIN__USE_OP
_TARRAY_PLAIN &_TARRAY_PLAIN::operator+=(const _TARRAY_PLAIN &obj)
{
    size_t i;
    const _TARRAY_PLAIN__TYPE *src;
    _TARRAY_PLAIN__TYPE *dest;
    size_t max = this->length() < obj.length() ? obj.length() : this->length();
    size_t min = obj.length() < this->length() ? obj.length() : this->length();

    this->resize(max);
    src = obj.arr_ptr;
    dest = this->arr_ptr;
    for ( i=0 ; i < min ; i++ ) {
	dest[i] += src[i];
    }

    return *this;
}

_TARRAY_PLAIN &_TARRAY_PLAIN::operator-=(const _TARRAY_PLAIN &obj)
{
    size_t i;
    const _TARRAY_PLAIN__TYPE *src;
    _TARRAY_PLAIN__TYPE *dest;
    size_t max = this->length() < obj.length() ? obj.length() : this->length();
    size_t min = obj.length() < this->length() ? obj.length() : this->length();

    this->resize(max);
    src = obj.arr_ptr;
    dest = this->arr_ptr;
    for ( i=0 ; i < min ; i++ ) {
	dest[i] -= src[i];
    }

    return *this;
}

_TARRAY_PLAIN &_TARRAY_PLAIN::operator*=(const _TARRAY_PLAIN &obj)
{
    size_t i;
    const _TARRAY_PLAIN__TYPE *src;
    _TARRAY_PLAIN__TYPE *dest;
    size_t max = this->length() < obj.length() ? obj.length() : this->length();
    size_t min = obj.length() < this->length() ? obj.length() : this->length();

    this->resize(max);
    src = obj.arr_ptr;
    dest = this->arr_ptr;
    for ( i=0 ; i < min ; i++ ) {
	dest[i] *= src[i];
    }

    return *this;
}

_TARRAY_PLAIN &_TARRAY_PLAIN::operator/=(const _TARRAY_PLAIN &obj)
{
    size_t i;
    const _TARRAY_PLAIN__TYPE *src;
    _TARRAY_PLAIN__TYPE *dest;
    size_t max = this->length() < obj.length() ? obj.length() : this->length();
    size_t min = obj.length() < this->length() ? obj.length() : this->length();

    this->resize(max);
    src = obj.arr_ptr;
    dest = this->arr_ptr;
    for ( i=0 ; i < min ; i++ ) {
	dest[i] /= src[i];
    }

    return *this;
}

_TARRAY_PLAIN &_TARRAY_PLAIN::operator+=(const _TARRAY_PLAIN__TYPE &val)
{
    size_t i;

    for ( i=0 ; i < this->length() ; i++ ) {
	this->arr_ptr[i] += val;
    }

    return *this;
}

_TARRAY_PLAIN &_TARRAY_PLAIN::operator-=(const _TARRAY_PLAIN__TYPE &val)
{
    size_t i;

    for ( i=0 ; i < this->length() ; i++ ) {
	this->arr_ptr[i] -= val;
    }

    return *this;
}

_TARRAY_PLAIN &_TARRAY_PLAIN::operator*=(const _TARRAY_PLAIN__TYPE &val)
{
    size_t i;

    for ( i=0 ; i < this->length() ; i++ ) {
	this->arr_ptr[i] *= val;
    }

    return *this;
}

_TARRAY_PLAIN &_TARRAY_PLAIN::operator/=(const _TARRAY_PLAIN__TYPE &val)
{
    size_t i;

    for ( i=0 ; i < this->length() ; i++ ) {
	this->arr_ptr[i] /= val;
    }

    return *this;
}
#endif	/* _TARRAY_PLAIN__USE_OP */

bool _TARRAY_PLAIN::operator==(const _TARRAY_PLAIN &obj) const
{
    return (this->arr_rec == obj.arr_rec);
}

bool _TARRAY_PLAIN::operator!=(const _TARRAY_PLAIN &obj) const
{
    return (this->arr_rec != obj.arr_rec);
}

/*
 * public method
 */

_TARRAY_PLAIN &_TARRAY_PLAIN::init()
{
    this->arr_rec->init();
    return *this;
}

_TARRAY_PLAIN &_TARRAY_PLAIN::init(const _TARRAY_PLAIN &obj)
{
    this->arr_rec->init(*(obj.arr_rec));
    return *this;
}

_TARRAY_PLAIN &_TARRAY_PLAIN::assign_default(const _TARRAY_PLAIN__TYPE &val)
{
    this->arr_rec->assign_default(&val);
    return *this;
}

_TARRAY_PLAIN &_TARRAY_PLAIN::clean()
{
    this->arr_rec->clean();
    return *this;
}

_TARRAY_PLAIN &_TARRAY_PLAIN::clean(const _TARRAY_PLAIN__TYPE &val)
{
    this->arr_rec->clean(&val);
    return *this;
}

#ifdef TARRAY_PLAIN__USE_SOLO_NARG
_TARRAY_PLAIN &_TARRAY_PLAIN::assign( size_t n )
{
    this->arr_rec->assign(n);
    return *this;
}
#endif

_TARRAY_PLAIN &_TARRAY_PLAIN::assign( const _TARRAY_PLAIN__TYPE &val, size_t n )
{
    _TARRAY_PLAIN__TYPE *p = 
	(_TARRAY_PLAIN__TYPE *)this->arr_rec->default_value();
    _TARRAY_PLAIN__TYPE df = *p;
    this->arr_rec->assign_default(&val);
    this->arr_rec->assign(n);
    this->arr_rec->assign_default(&df);
    return *this;
}

_TARRAY_PLAIN &_TARRAY_PLAIN::assign( const _TARRAY_PLAIN__TYPE vals[],
				      size_t n )
{
    this->arr_rec->assign(vals,n);
    return *this;
}

_TARRAY_PLAIN &_TARRAY_PLAIN::assign(const _TARRAY_PLAIN &src, size_t idx2)
{
    this->arr_rec->assign(*(src.arr_rec), idx2);
    return *this;
}

_TARRAY_PLAIN &_TARRAY_PLAIN::assign( const _TARRAY_PLAIN &src, 
				      size_t idx2, size_t n2)
{
    this->arr_rec->assign(*(src.arr_rec), idx2, n2);
    return *this;
}

#ifdef _TARRAY_PLAIN__USE_VARG
#define FUNC_NAME "assign"
_TARRAY_PLAIN &_TARRAY_PLAIN::assign( size_t n,
		    _TARRAY_PLAIN__VATYPE el0, _TARRAY_PLAIN__VATYPE el1, ... )
{
    va_list ap;
    va_start(ap, el1);
    try {
	this->vassign(n, el0, el1, ap);
    }
    catch (...) {
	va_end(ap);
        err_throw(FUNC_NAME,"FATAL","this->vassign() failed");
    }
    va_end(ap);
    return *this;
}
#undef FUNC_NAME

#define FUNC_NAME "vassign"
_TARRAY_PLAIN &_TARRAY_PLAIN::vassign( size_t n, 
	     _TARRAY_PLAIN__VATYPE el0, _TARRAY_PLAIN__VATYPE el1, va_list ap )
{
    _TARRAY_PLAIN__TYPE *tmp_ptr = NULL;

    _TARRAY_PLAIN::erase();

    if ( 0 < n ) {
	tmp_ptr = (_TARRAY_PLAIN__TYPE *)malloc(sizeof(*tmp_ptr)*(n));
	if ( tmp_ptr == NULL ) {
	    err_throw(FUNC_NAME,"FATAL","malloc() failed");
	}
        tmp_ptr[0] = el0;
	if ( 1 < n ) {
	    tmp_ptr[1] = el1;
	    if ( 2 < n ) {
		size_t i;
		for ( i=1 ; i < n ; i++ ) {
		    _TARRAY_PLAIN__TYPE elem = 
			va_arg(ap,_TARRAY_PLAIN__VATYPE);
		    tmp_ptr[i] = elem;
		}
	    }
	}
    }

    try {
	this->arr_rec->append(tmp_ptr,n);
    }
    catch (...) {
	if ( tmp_ptr != NULL ) free(tmp_ptr);
        err_throw(FUNC_NAME,"FATAL","this->arr_rec->append() failed");
    }

    if ( tmp_ptr != NULL ) free(tmp_ptr);
    return *this;
}
#undef FUNC_NAME
#endif	/* _TARRAY_PLAIN__USE_VARG */

#ifdef _TARRAY_PLAIN__USE_SPLIT
_TARRAY_PLAIN &_TARRAY_PLAIN::split( const char *line, const char *delims, 
				     bool zero_str,
				     const char *quotations, int escape )
{
    tarray_tstring sarr;
    size_t i;
    sarr.split(line,delims,zero_str,quotations,escape);
    _TARRAY_PLAIN::resize(sarr.length());
    for ( i=0 ; i < this->length() ; i++ ) {
	this->arr_ptr[i] = 
	    (_TARRAY_PLAIN__TYPE)(sarr.at(i)._TARRAY_PLAIN__ATOX());
    }
    return *this;
}

_TARRAY_PLAIN &_TARRAY_PLAIN::split( const char *line, const char *delims, 
				     bool zero_str )
{
    return this->split(line,delims,zero_str,NULL,'\0');
}
#endif	/* _TARRAY_PLAIN__USE_SPLIT */

#ifdef TARRAY_PLAIN__USE_SOLO_NARG
_TARRAY_PLAIN &_TARRAY_PLAIN::append( size_t n )
{
    this->arr_rec->append(n);
    return *this;
}
#endif

_TARRAY_PLAIN &_TARRAY_PLAIN::append( const _TARRAY_PLAIN__TYPE &val, size_t n )
{
    _TARRAY_PLAIN__TYPE *p = 
	(_TARRAY_PLAIN__TYPE *)this->arr_rec->default_value();
    _TARRAY_PLAIN__TYPE df = *p;
    this->arr_rec->assign_default(&val);
    this->arr_rec->append(n);
    this->arr_rec->assign_default(&df);
    return *this;
}

_TARRAY_PLAIN &_TARRAY_PLAIN::append( const _TARRAY_PLAIN__TYPE vals[], 
				      size_t n )
{
    this->arr_rec->append(vals,n);
    return *this;
}

_TARRAY_PLAIN &_TARRAY_PLAIN::append(const _TARRAY_PLAIN &src, size_t idx2)
{
    this->arr_rec->append(*(src.arr_rec), idx2);
    return *this;
}

_TARRAY_PLAIN &_TARRAY_PLAIN::append( const _TARRAY_PLAIN &src, 
				      size_t idx2, size_t n2)
{
    this->arr_rec->append(*(src.arr_rec), idx2, n2);
    return *this;
}

#ifdef _TARRAY_PLAIN__USE_VARG
#define FUNC_NAME "append"
_TARRAY_PLAIN &_TARRAY_PLAIN::append( size_t n,
		    _TARRAY_PLAIN__VATYPE el0, _TARRAY_PLAIN__VATYPE el1, ... )
{
    va_list ap;
    va_start(ap, el1);
    try {
	this->vappend(n, el0, el1, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(FUNC_NAME,"FATAL","this->vappend() failed");
    }
    va_end(ap);
    return *this;
}

_TARRAY_PLAIN &_TARRAY_PLAIN::vappend( size_t n,
	     _TARRAY_PLAIN__VATYPE el0, _TARRAY_PLAIN__VATYPE el1, va_list ap )
{
    _TARRAY_PLAIN__TYPE *tmp_ptr = NULL;

    if ( 0 < n ) {
	tmp_ptr = (_TARRAY_PLAIN__TYPE *)malloc(sizeof(*tmp_ptr)*(n));
	if ( tmp_ptr == NULL ) {
	    err_throw(FUNC_NAME,"FATAL","malloc() failed");
	}
        tmp_ptr[0] = el0;
	if ( 1 < n ) {
	    tmp_ptr[1] = el1;
	    if ( 2 < n ) {
		size_t i;
		for ( i=1 ; i < n ; i++ ) {
		    _TARRAY_PLAIN__TYPE elem =
			va_arg(ap,_TARRAY_PLAIN__VATYPE);
		    tmp_ptr[i] = elem;
		}
	    }
	}
    }

    try {
	this->arr_rec->append(tmp_ptr,n);
    }
    catch (...) {
	if ( tmp_ptr != NULL ) free(tmp_ptr);
        err_throw(FUNC_NAME,"FATAL","this->arr_rec->append() failed");
    }

    if ( tmp_ptr != NULL ) free(tmp_ptr);
    return *this;
}
#undef FUNC_NAME
#endif	/* _TARRAY_PLAIN__USE_VARG */

#ifdef TARRAY_PLAIN__USE_SOLO_NARG
_TARRAY_PLAIN &_TARRAY_PLAIN::insert( size_t index, size_t n )
{
    this->arr_rec->insert(index,n);
    return *this;
}
#endif

_TARRAY_PLAIN &_TARRAY_PLAIN::insert( size_t index,
				      const _TARRAY_PLAIN__TYPE &val, size_t n )
{
    _TARRAY_PLAIN__TYPE *p = 
	(_TARRAY_PLAIN__TYPE *)this->arr_rec->default_value();
    _TARRAY_PLAIN__TYPE df = *p;
    this->arr_rec->assign_default(&val);
    this->arr_rec->insert(index,n);
    this->arr_rec->assign_default(&df);
    return *this;
}

_TARRAY_PLAIN &_TARRAY_PLAIN::insert( size_t index,
				      const _TARRAY_PLAIN &src, size_t idx2)
{
    this->arr_rec->insert(index, *(src.arr_rec), idx2);
    return *this;
}

_TARRAY_PLAIN &_TARRAY_PLAIN::insert( size_t index, 
			      const _TARRAY_PLAIN &src, size_t idx2, size_t n2)
{
    this->arr_rec->insert(index, *(src.arr_rec), idx2, n2);
    return *this;
}

_TARRAY_PLAIN &_TARRAY_PLAIN::insert( size_t index, 
				   const _TARRAY_PLAIN__TYPE vals[], size_t n )
{
    this->arr_rec->insert(index, vals, n);
    return *this;
}

#ifdef _TARRAY_PLAIN__USE_VARG
#define FUNC_NAME "insert"
_TARRAY_PLAIN &_TARRAY_PLAIN::insert( size_t index, size_t n, 
		    _TARRAY_PLAIN__VATYPE el0, _TARRAY_PLAIN__VATYPE el1, ... )
{
    va_list ap;
    va_start(ap, el1);
    try {
	this->vinsert( index, n, el0, el1, ap );
    }
    catch (...) {
	va_end(ap);
        err_throw(FUNC_NAME,"FATAL","this->vinsert() failed");
    }
    va_end(ap);
    return *this;
}
#undef FUNC_NAME

#define FUNC_NAME "vinsert"
_TARRAY_PLAIN &_TARRAY_PLAIN::vinsert( size_t index, 
   size_t n, _TARRAY_PLAIN__VATYPE el0, _TARRAY_PLAIN__VATYPE el1, va_list ap )
{
    _TARRAY_PLAIN__TYPE *tmp_ptr = NULL;

    if ( 0 < n ) {
	tmp_ptr = (_TARRAY_PLAIN__TYPE *)malloc(sizeof(*tmp_ptr)*(n));
	if ( tmp_ptr == NULL ) {
	    err_throw(FUNC_NAME,"FATAL","malloc() failed");
	}
        tmp_ptr[0] = el0;
	if ( 1 < n ) {
	    tmp_ptr[1] = el1;
	    if ( 2 < n ) {
		size_t i;
		for ( i=1 ; i < n ; i++ ) {
		    _TARRAY_PLAIN__TYPE elem = 
			va_arg(ap,_TARRAY_PLAIN__VATYPE);
		    tmp_ptr[i] = elem;
		}
	    }
	}
    }

    try {
	this->arr_rec->insert(index,tmp_ptr,n);
    }
    catch (...) {
	if ( tmp_ptr != NULL ) free(tmp_ptr);
        err_throw(FUNC_NAME,"FATAL","this->arr_rec->insert() failed");
    }

    if ( tmp_ptr != NULL ) free(tmp_ptr);
    return *this;
}
#undef FUNC_NAME
#endif	/* _TARRAY_PLAIN__USE_VARG */

#ifdef TARRAY_PLAIN__USE_SOLO_NARG
_TARRAY_PLAIN &_TARRAY_PLAIN::replace( size_t idx1, size_t n1, size_t n2 )
{
    this->arr_rec->replace(idx1,n1,n2);
    return *this;
}
#endif

_TARRAY_PLAIN &_TARRAY_PLAIN::replace( size_t idx1, size_t n1,
				      const _TARRAY_PLAIN__TYPE &val, size_t n2 )
{
    _TARRAY_PLAIN__TYPE *p = 
	(_TARRAY_PLAIN__TYPE *)this->arr_rec->default_value();
    _TARRAY_PLAIN__TYPE df = *p;
    this->arr_rec->assign_default(&val);
    this->arr_rec->replace(idx1,n1,n2);
    this->arr_rec->assign_default(&df);
    return *this;
}

_TARRAY_PLAIN &_TARRAY_PLAIN::replace( size_t idx1, size_t n1,
				       const _TARRAY_PLAIN &src, size_t idx2)
{
    this->arr_rec->replace(idx1, n1, *(src.arr_rec), idx2);
    return *this;
}

_TARRAY_PLAIN &_TARRAY_PLAIN::replace( size_t idx1, size_t n1, 
			      const _TARRAY_PLAIN &src, size_t idx2, size_t n2)
{
    this->arr_rec->replace(idx1, n1, *(src.arr_rec), idx2, n2);
    return *this;
}

_TARRAY_PLAIN &_TARRAY_PLAIN::replace( size_t idx1, size_t n1, 
				  const _TARRAY_PLAIN__TYPE vals[], size_t n2 )
{
    this->arr_rec->replace(idx1, n1, vals, n2);
    return *this;
}

#ifdef _TARRAY_PLAIN__USE_VARG
#define FUNC_NAME "replace"
_TARRAY_PLAIN &_TARRAY_PLAIN::replace( size_t idx1, size_t n1,
	 size_t n2, _TARRAY_PLAIN__VATYPE el0, _TARRAY_PLAIN__VATYPE el1, ... )
{
    va_list ap;
    va_start(ap, el1);
    try {
	this->vreplace( idx1, n1, n2, el0, el1, ap );
    }
    catch (...) {
	va_end(ap);
        err_throw(FUNC_NAME,"FATAL","this->vreplace() failed");
    }
    va_end(ap);
    return *this;
}
#undef FUNC_NAME

#define FUNC_NAME "vreplace"
_TARRAY_PLAIN &_TARRAY_PLAIN::vreplace( size_t idx1, size_t n1,
  size_t n2, _TARRAY_PLAIN__VATYPE el0, _TARRAY_PLAIN__VATYPE el1, va_list ap )
{
    _TARRAY_PLAIN__TYPE *tmp_ptr = NULL;

    if ( 0 < n2 ) {
	tmp_ptr = (_TARRAY_PLAIN__TYPE *)malloc(sizeof(*tmp_ptr)*(n2));
	if ( tmp_ptr == NULL ) {
	    err_throw(FUNC_NAME,"FATAL","malloc() failed");
	}
        tmp_ptr[0] = el0;
	if ( 1 < n2 ) {
	    tmp_ptr[1] = el1;
	    if ( 2 < n2 ) {
		size_t i;
		for ( i=1 ; i < n2 ; i++ ) {
		    _TARRAY_PLAIN__TYPE elem = 
			va_arg(ap,_TARRAY_PLAIN__VATYPE);
		    tmp_ptr[i] = elem;
		}
	    }
	}
    }

    try {
	this->arr_rec->replace(idx1,n1,tmp_ptr,n2);
    }
    catch (...) {
	if ( tmp_ptr != NULL ) free(tmp_ptr);
        err_throw(FUNC_NAME,"FATAL","this->arr_rec->replace() failed");
    }

    if ( tmp_ptr != NULL ) free(tmp_ptr);
    return *this;
}
#undef FUNC_NAME
#endif	/* _TARRAY_PLAIN__USE_VARG */

#ifdef TARRAY_PLAIN__USE_SOLO_NARG
_TARRAY_PLAIN &_TARRAY_PLAIN::put( size_t index, size_t n )
{
    this->arr_rec->put(index,n);
    return *this;
}
#endif

_TARRAY_PLAIN &_TARRAY_PLAIN::put( size_t index,
				   const _TARRAY_PLAIN__TYPE &val, size_t n )
{
    _TARRAY_PLAIN__TYPE *p = 
	(_TARRAY_PLAIN__TYPE *)this->arr_rec->default_value();
    _TARRAY_PLAIN__TYPE df = *p;
    this->arr_rec->assign_default(&val);
    this->arr_rec->put(index,n);
    this->arr_rec->assign_default(&df);
    return *this;
}

_TARRAY_PLAIN &_TARRAY_PLAIN::put( size_t index,
				   const _TARRAY_PLAIN &src, size_t idx2)
{
    this->arr_rec->put(index, *(src.arr_rec), idx2);
    return *this;
}

_TARRAY_PLAIN &_TARRAY_PLAIN::put( size_t index, 
			      const _TARRAY_PLAIN &src, size_t idx2, size_t n2)
{
    this->arr_rec->put(index, *(src.arr_rec), idx2, n2);
    return *this;
}

_TARRAY_PLAIN &_TARRAY_PLAIN::put( size_t index, 
				   const _TARRAY_PLAIN__TYPE vals[], size_t n )
{
    this->arr_rec->put(index, vals, n);
    return *this;
}

#ifdef _TARRAY_PLAIN__USE_VARG
#define FUNC_NAME "put"
_TARRAY_PLAIN &_TARRAY_PLAIN::put( size_t index, size_t n, 
		    _TARRAY_PLAIN__VATYPE el0, _TARRAY_PLAIN__VATYPE el1, ... )
{
    va_list ap;
    va_start(ap, el1);
    try {
	this->vput( index, n, el0, el1, ap );
    }
    catch (...) {
	va_end(ap);
        err_throw(FUNC_NAME,"FATAL","this->vput() failed");
    }
    va_end(ap);
    return *this;
}
#undef FUNC_NAME

#define FUNC_NAME "vput"
_TARRAY_PLAIN &_TARRAY_PLAIN::vput( size_t index, 
   size_t n, _TARRAY_PLAIN__VATYPE el0, _TARRAY_PLAIN__VATYPE el1, va_list ap )
{
    _TARRAY_PLAIN__TYPE *tmp_ptr = NULL;

    if ( 0 < n ) {
	tmp_ptr = (_TARRAY_PLAIN__TYPE *)malloc(sizeof(*tmp_ptr)*(n));
	if ( tmp_ptr == NULL ) {
	    err_throw(FUNC_NAME,"FATAL","malloc() failed");
	}
        tmp_ptr[0] = el0;
	if ( 1 < n ) {
	    tmp_ptr[1] = el1;
	    if ( 2 < n ) {
		size_t i;
		for ( i=1 ; i < n ; i++ ) {
		    _TARRAY_PLAIN__TYPE elem = 
			va_arg(ap,_TARRAY_PLAIN__VATYPE);
		    tmp_ptr[i] = elem;
		}
	    }
	}
    }

    try {
	this->arr_rec->put(index,tmp_ptr,n);
    }
    catch (...) {
	if ( tmp_ptr != NULL ) free(tmp_ptr);
        err_throw(FUNC_NAME,"FATAL","this->arr_rec->put() failed");
    }

    if ( tmp_ptr != NULL ) free(tmp_ptr);
    return *this;
}
#undef FUNC_NAME
#endif	/* _TARRAY_PLAIN__USE_VARG */

_TARRAY_PLAIN &_TARRAY_PLAIN::erase()
{
    this->arr_rec->erase();
    return *this;
}

#define FUNC_NAME "erase"
_TARRAY_PLAIN &_TARRAY_PLAIN::erase( size_t index, size_t num_el )
{
    this->arr_rec->erase(index, num_el);
    return *this;
}
#undef FUNC_NAME

#define FUNC_NAME "resize"
_TARRAY_PLAIN &_TARRAY_PLAIN::resize( size_t new_num_elements )
{
    this->arr_rec->resize(new_num_elements);
    return *this;
}
#undef FUNC_NAME

_TARRAY_PLAIN &_TARRAY_PLAIN::swap( _TARRAY_PLAIN &src )
{
    this->arr_rec->swap( *(src.arr_rec) );
    return *this;
}

ssize_t _TARRAY_PLAIN::getdata( size_t index, 
				void *dest_buf, size_t buf_size ) const
{
    return this->arr_rec->getdata(index,dest_buf,buf_size);
}

ssize_t _TARRAY_PLAIN::getdata( void *dest_buf, size_t buf_size ) const
{
    return this->arr_rec->getdata(dest_buf,buf_size);
}

ssize_t _TARRAY_PLAIN::copy( size_t index, size_t n, _TARRAY_PLAIN &dest) const
{
    return this->arr_rec->copy(index, n, *(dest.arr_rec));
}

ssize_t _TARRAY_PLAIN::copy( size_t index, _TARRAY_PLAIN &dest ) const
{
    return this->arr_rec->copy(index, *(dest.arr_rec));
}

ssize_t _TARRAY_PLAIN::copy( _TARRAY_PLAIN &dest ) const
{
    return this->arr_rec->copy( *(dest.arr_rec) );
}

const _TARRAY_PLAIN__TYPE *_TARRAY_PLAIN::carray() const
{
    return (const _TARRAY_PLAIN__TYPE *)this->arr_ptr;
}

_TARRAY_PLAIN__TYPE *_TARRAY_PLAIN::data_ptr()
{
    return this->arr_ptr;
}

_TARRAY_PLAIN__TYPE &_TARRAY_PLAIN::element( size_t index )
{
    return _TARRAY_PLAIN::at(index);
}

#define FUNC_NAME "at"
_TARRAY_PLAIN__TYPE &_TARRAY_PLAIN::at( size_t index )
{
#if 1
    if ( this->length() <= index ) {
        _TARRAY_PLAIN::resize(index + 1);
    }
#endif
    if ( index < 0 || this->length() <= index ) {
        err_throw(FUNC_NAME,"ERROR","Invalid index");
    }
    else return this->arr_ptr[index];
}
#undef FUNC_NAME

#define FUNC_NAME "at_cs"
const _TARRAY_PLAIN__TYPE &_TARRAY_PLAIN::at_cs( size_t index ) const
{
    if ( index < 0 || this->length() <= index ) {
        err_throw(FUNC_NAME,"ERROR","Invalid index");
    }
    else return this->arr_ptr[index];
}
#undef FUNC_NAME

ssize_t _TARRAY_PLAIN::find( const _TARRAY_PLAIN__TYPE &val ) const
{
    return this->find(0,val);
}

ssize_t _TARRAY_PLAIN::find( size_t pos, const _TARRAY_PLAIN__TYPE &val ) const
{
    size_t i,j;
    for ( i=pos ; i < this->length() ; i++ ) {
	//if ( this->arr_ptr[i] == val ) return i;
	for ( j=0 ; j < sizeof(val) ; j++ ) {
	    if ( ((const unsigned char *)(this->arr_ptr + i))[j] !=
		 ((const unsigned char *)(&val))[j] ) break;
	}
	if ( j == sizeof(val) ) return i;
    }
    return -1;
}

ssize_t _TARRAY_PLAIN::rfind( const _TARRAY_PLAIN__TYPE &val ) const
{
    if ( this->length() <= 0 ) return -1;
    return this->rfind(this->length() - 1, val);
}

ssize_t _TARRAY_PLAIN::rfind( size_t pos, const _TARRAY_PLAIN__TYPE &val ) const
{
    size_t i,j;
    if ( this->length() <= 0 ) return -1;
    if ( this->length() <= pos ) pos = this->length() - 1;
    for ( i=pos + 1 ; 0 < i ; ) {
	i--;
	//if ( this->arr_ptr[i] == val ) return i;
	for ( j=0 ; j < sizeof(val) ; j++ ) {
	    if ( ((const unsigned char *)(this->arr_ptr + i))[j] !=
		 ((const unsigned char *)(&val))[j] ) break;
	}
	if ( j == sizeof(val) ) return i;
    }
    return -1;
}

size_t _TARRAY_PLAIN::length() const
{
    return this->arr_rec->length();
}

size_t _TARRAY_PLAIN::size() const
{
    return this->arr_rec->length();
}

size_t _TARRAY_PLAIN::byte_length() const
{
    return this->arr_rec->byte_length();
}

/*
 * private 
 */

#ifndef TARRAY_PLAIN__USE_SOLO_NARG
void _TARRAY_PLAIN::assign( const _TARRAY_PLAIN__TYPE &val )
{
    sli__eprintf("[FATAL ERROR] Do not use _TARRAY_PLAIN::assign(val) !!\n");
    abort();
    return;
}

void _TARRAY_PLAIN::append( const _TARRAY_PLAIN__TYPE &val )
{
    sli__eprintf("[FATAL ERROR] Do not use _TARRAY_PLAIN::assign(val) !!\n");
    abort();
    return;
}

void _TARRAY_PLAIN::insert( size_t index, const _TARRAY_PLAIN__TYPE &val )
{
    sli__eprintf("[FATAL ERROR] Do not use "
		  "_TARRAY_PLAIN::insert(index,val) !!\n");
    abort();
    return;
}

void _TARRAY_PLAIN::replace( size_t idx1, size_t n1, const _TARRAY_PLAIN__TYPE &val )
{
    sli__eprintf("[FATAL ERROR] Do not use "
		  "_TARRAY_PLAIN::replace(idx1,n1,val) !!\n");
    abort();
    return;
}

void _TARRAY_PLAIN::put( size_t index, const _TARRAY_PLAIN__TYPE &val )
{
    sli__eprintf("[FATAL ERROR] Do not use "
		  "_TARRAY_PLAIN::put(index,val) !!\n");
    abort();
    return;
}
#endif

}	/* namespace sli */

#ifdef _TARRAY_PLAIN__USE_SPLIT
# undef _TARRAY_PLAIN__ATOX
#endif

#endif	/* end of source */


#undef _TARRAY_PLAIN__TYPE
#undef _TARRAY_PLAIN

#ifdef _TARRAY_PLAIN__USE_VARG
# undef _TARRAY_PLAIN__VATYPE
#endif
