/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2009-01-10 15:27:10 cyamauch> */

#ifndef _ASARRAY_PLAIN__TYPE
# error "_ASARRAY_PLAIN__TYPE is not defined"
#endif
#ifndef _ASARRAY_PLAIN
# error "_ASARRAY_PLAIN is not defined"
#endif

#ifdef _ASARRAY_PLAIN__CREATE_STRUCT
# ifndef _ASARRAY_PLAIN__ASARRDEF
#  error "_ASARRAY_PLAIN__ASARRDEF is not defined"
# endif
#endif

#ifdef _ASARRAY_PLAIN__USE_VARG
# ifndef _ASARRAY_PLAIN__VATYPE
#  error "_ASARRAY_PLAIN__VATYPE is not defined"
# endif
#endif

/* header */
#ifndef _ASARRAY_PLAIN__GET_SOURCE

#include "tarray_plain.h"
#include "tarray_tstring.h"
#include "ctindex.h"

namespace sli
{

#ifdef _ASARRAY_PLAIN__CREATE_STRUCT
  typedef struct {
    const char *key;
    _ASARRAY_PLAIN__TYPE value;
  } _ASARRAY_PLAIN__ASARRDEF;
#endif

  class _ASARRAY_PLAIN
  {
  public:
    _ASARRAY_PLAIN();
    _ASARRAY_PLAIN(const _ASARRAY_PLAIN &obj);
    virtual ~_ASARRAY_PLAIN();
    virtual _ASARRAY_PLAIN &operator=(const _ASARRAY_PLAIN &obj);
#ifdef _ASARRAY_PLAIN__USE_OP
#if 0
    virtual _ASARRAY_PLAIN &operator+=(const _ASARRAY_PLAIN &obj);
    virtual _ASARRAY_PLAIN &operator-=(const _ASARRAY_PLAIN &obj);
    virtual _ASARRAY_PLAIN &operator*=(const _ASARRAY_PLAIN &obj);
    virtual _ASARRAY_PLAIN &operator/=(const _ASARRAY_PLAIN &obj);
#endif
#endif
    virtual _ASARRAY_PLAIN &init();
    virtual _ASARRAY_PLAIN &init(const _ASARRAY_PLAIN &obj);
    virtual _ASARRAY_PLAIN &assign_default(const _ASARRAY_PLAIN__TYPE &val);
    virtual _ASARRAY_PLAIN &clean();
    virtual _ASARRAY_PLAIN &clean( const _ASARRAY_PLAIN__TYPE &val );
    virtual _ASARRAY_PLAIN &assign( const _ASARRAY_PLAIN &obj );
#ifdef _ASARRAY_PLAIN__CREATE_STRUCT
    virtual _ASARRAY_PLAIN &assign( const _ASARRAY_PLAIN__ASARRDEF elements[] );
    virtual _ASARRAY_PLAIN &assign( const _ASARRAY_PLAIN__ASARRDEF elements[],
				    size_t n );
#endif
    virtual _ASARRAY_PLAIN &assign( const char *key, const _ASARRAY_PLAIN__TYPE &val );
#ifdef _ASARRAY_PLAIN__USE_VARG
    virtual _ASARRAY_PLAIN &assign( const char *key0, _ASARRAY_PLAIN__VATYPE val0,
				    const char *key1, ... );
    virtual _ASARRAY_PLAIN &vassign( const char *key0, _ASARRAY_PLAIN__VATYPE val0,
				     const char *key1, va_list ap );
#endif
    virtual _ASARRAY_PLAIN &split_keys( const char *line, const char *delims, 
					bool zero_str, const char *quotations,
					int escape );
    virtual _ASARRAY_PLAIN &split_keys( const char *line, const char *delims,
					bool zero_str = false );
#ifdef _ASARRAY_PLAIN__USE_SPLIT
    virtual _ASARRAY_PLAIN &split_values( const char *line, const char *delims,
					 bool zero_str, const char *quotations,
					 int escape );
    virtual _ASARRAY_PLAIN &split_values( const char *line, const char *delims,
					  bool zero_str = false );
#endif
#ifdef _ASARRAY_PLAIN__CREATE_STRUCT
    virtual _ASARRAY_PLAIN &append( const _ASARRAY_PLAIN__ASARRDEF elements[] );
    virtual _ASARRAY_PLAIN &append( const _ASARRAY_PLAIN__ASARRDEF elements[],
				    size_t n );
#endif
    virtual _ASARRAY_PLAIN &append( const _ASARRAY_PLAIN &src );
    virtual _ASARRAY_PLAIN &append( const char *key, const _ASARRAY_PLAIN__TYPE &val );
#ifdef _ASARRAY_PLAIN__USE_VARG
    virtual _ASARRAY_PLAIN &append( const char *key0, _ASARRAY_PLAIN__VATYPE val0, 
				    const char *key1, ... );
    virtual _ASARRAY_PLAIN &vappend( const char *key0, _ASARRAY_PLAIN__VATYPE val0, 
				     const char *key1, va_list ap );
#endif
#ifdef _ASARRAY_PLAIN__CREATE_STRUCT
    virtual _ASARRAY_PLAIN &insert( const char *key, 
				    const _ASARRAY_PLAIN__ASARRDEF elements[] );
    virtual _ASARRAY_PLAIN &insert( const char *key, 
				    const _ASARRAY_PLAIN__ASARRDEF elements[],
				    size_t n );
#endif
    virtual _ASARRAY_PLAIN &insert( const char *key, const _ASARRAY_PLAIN &src );
    virtual _ASARRAY_PLAIN &insert( const char *key,
				    const char *newkey, _ASARRAY_PLAIN__TYPE newval );
#ifdef _ASARRAY_PLAIN__USE_VARG
    virtual _ASARRAY_PLAIN &insert( const char *key, 
				    const char *key0, _ASARRAY_PLAIN__VATYPE val0, 
				    const char *key1, ... );
    virtual _ASARRAY_PLAIN &vinsert( const char *key,
				     const char *key0, _ASARRAY_PLAIN__VATYPE val0, 
				     const char *key1, va_list ap );
#endif
    virtual _ASARRAY_PLAIN &erase();
    virtual _ASARRAY_PLAIN &erase( const char *key, size_t num_elements = 1 );
    virtual _ASARRAY_PLAIN &swap( _ASARRAY_PLAIN &sobj );
    /* */
    virtual const _ASARRAY_PLAIN__TYPE *carray() const;
    virtual _ASARRAY_PLAIN__TYPE &at( const char *key );
    virtual _ASARRAY_PLAIN__TYPE &atf( const char *fmt, ... );
    virtual _ASARRAY_PLAIN__TYPE &vatf( const char *fmt, va_list ap );
    virtual _ASARRAY_PLAIN__TYPE &element( const char *key );
    virtual size_t length() const;
    virtual size_t size() const;
    /* */
    virtual ssize_t index( const char *key ) const;
    virtual ssize_t indexf( const char *fmt, ... ) const;
    virtual ssize_t vindexf( const char *fmt, va_list ap ) const;
    virtual const char *key( size_t index ) const;
    virtual _ASARRAY_PLAIN__TYPE &at( size_t index );
    virtual _ASARRAY_PLAIN__TYPE &element( size_t index );
    virtual const _ASARRAY_PLAIN__TYPE &at_cs( size_t index ) const;
  private:
    tarray_tstring key_rec;
    tarray_plain *value_rec;
    _ASARRAY_PLAIN__TYPE *value_ptr;
    ctindex index_rec;
  };
}

#endif	/* #ifndef _ASARRAY_PLAIN__GET_SOURCE */

/* Source */
#if defined(_ASARRAY_PLAIN__GET_SOURCE) || defined(_ASARRAY_PLAIN__GET_BOTH)

#ifdef _ASARRAY_PLAIN__USE_SPLIT
# ifndef _ASARRAY_PLAIN__ATOX
#  error "_ASARRAY_PLAIN__ATOX is not defined"
# endif
#endif

#define CLASS_NAME "_ASARRAY_PLAIN"

#include "private/err_report.h"

namespace sli
{

/* constructor */
#define FUNC_NAME "_ASARRAY_PLAIN"
_ASARRAY_PLAIN::_ASARRAY_PLAIN()
{
    try {
	this->value_rec = new tarray_plain(sizeof(_ASARRAY_PLAIN__TYPE),
				     (void **)&this->value_ptr);
    }
    catch (...) {
        err_throw(FUNC_NAME,"FATAL","new failed");
    }
    return;
}

/* copy constructor */
_ASARRAY_PLAIN::_ASARRAY_PLAIN(const _ASARRAY_PLAIN &obj)
{
    try {
	this->value_rec = new tarray_plain(sizeof(_ASARRAY_PLAIN__TYPE),
				     (void **)&this->value_ptr);
    }
    catch (...) {
        err_throw(FUNC_NAME,"FATAL","new failed");
    }
    this->init(obj);
    return;
}
#undef FUNC_NAME

/* destructor */
_ASARRAY_PLAIN::~_ASARRAY_PLAIN()
{
    delete this->value_rec;
    return;
}

_ASARRAY_PLAIN &_ASARRAY_PLAIN::operator=(const _ASARRAY_PLAIN &obj)
{
    this->init(obj);
    return *this;
}

/*
 * public method
 */

_ASARRAY_PLAIN &_ASARRAY_PLAIN::init()
{
    this->index_rec.init();
    this->key_rec.init();
    this->value_rec->init();
    return *this;
}

_ASARRAY_PLAIN &_ASARRAY_PLAIN::init(const _ASARRAY_PLAIN &obj)
{
    this->value_rec->init(*(obj.value_rec));
    this->key_rec.init(obj.key_rec);
    this->index_rec.init(obj.index_rec);
    return *this;
}

_ASARRAY_PLAIN &_ASARRAY_PLAIN::assign_default( const _ASARRAY_PLAIN__TYPE &val )
{
    this->value_rec->assign_default(&val);
    return *this;
}

_ASARRAY_PLAIN &_ASARRAY_PLAIN::clean()
{
    this->value_rec->clean();
    return *this;
}

_ASARRAY_PLAIN &_ASARRAY_PLAIN::clean( const _ASARRAY_PLAIN__TYPE &val )
{
    this->value_rec->clean(&val);
    return *this;
}

_ASARRAY_PLAIN &_ASARRAY_PLAIN::assign(const _ASARRAY_PLAIN &obj)
{
    _ASARRAY_PLAIN::erase();
    return _ASARRAY_PLAIN::append(obj);
}

#ifdef _ASARRAY_PLAIN__CREATE_STRUCT
_ASARRAY_PLAIN &_ASARRAY_PLAIN::assign( const _ASARRAY_PLAIN__ASARRDEF elements[] )
{
    _ASARRAY_PLAIN::erase();
    if ( elements != NULL ) {
	size_t i;
	for ( i=0 ; elements[i].key != NULL ; i++ ) {
	    _ASARRAY_PLAIN::append(elements[i].key,elements[i].value);
	}
    }
    return *this;
}

_ASARRAY_PLAIN &_ASARRAY_PLAIN::assign( const _ASARRAY_PLAIN__ASARRDEF elements[], 
					size_t n )
{
    _ASARRAY_PLAIN::erase();
    if ( 0 < n && elements != NULL ) {
	size_t i;
	for ( i=0 ; i < n && elements[i].key != NULL ; i++ ) {
	    _ASARRAY_PLAIN::append(elements[i].key,elements[i].value);
	}
    }
    return *this;
}
#endif	/* _ASARRAY_PLAIN__CREATE_STRUCT */

_ASARRAY_PLAIN &_ASARRAY_PLAIN::assign( const char *key, const _ASARRAY_PLAIN__TYPE &val )
{
    _ASARRAY_PLAIN::erase();
    _ASARRAY_PLAIN::append(key,val);
    return *this;
}

#ifdef _ASARRAY_PLAIN__USE_VARG
#define FUNC_NAME "assign"
_ASARRAY_PLAIN &_ASARRAY_PLAIN::assign( const char *key0, 
					_ASARRAY_PLAIN__VATYPE val0,
					const char *key1, ... )
{
    va_list ap;
    va_start(ap,key1);
    try {
	this->vassign(key0,val0,key1,ap);
    }
    catch (...) {
	va_end(ap);
        err_throw(FUNC_NAME,"FATAL","this->vassign() failed");
    }
    va_end(ap);
    return *this;
}
#undef FUNC_NAME

_ASARRAY_PLAIN &_ASARRAY_PLAIN::vassign( const char *key0, 
					 _ASARRAY_PLAIN__VATYPE val0,
					 const char *key1, va_list ap )
{
    _ASARRAY_PLAIN::erase();
    _ASARRAY_PLAIN::append(key0,val0);
    if ( key0 != NULL && key1 != NULL ) {
	_ASARRAY_PLAIN__TYPE val1 = va_arg(ap,_ASARRAY_PLAIN__VATYPE);
	_ASARRAY_PLAIN::append(key1,val1);
	while ( 1 ) {
	    const char *keyx = va_arg(ap,char *);
	    _ASARRAY_PLAIN__TYPE valx = va_arg(ap,_ASARRAY_PLAIN__VATYPE);
	    if ( keyx == NULL ) break;
	    _ASARRAY_PLAIN::append(keyx,valx);
	}
    }
    return *this;
}
#endif	/* _ASARRAY_PLAIN__USE_VARG */

_ASARRAY_PLAIN &_ASARRAY_PLAIN::split_keys( const char *line, 
					    const char *delims, 
					    bool zero_str, 
					    const char *quotations,
					    int escape )
{
    size_t i;
    tarray_tstring tmp_keys;

    _ASARRAY_PLAIN__TYPE zero;
    unsigned char *p = (unsigned char *)&zero;
    for ( i=0 ; i < sizeof(_ASARRAY_PLAIN__TYPE) ; i++ )
	p[i] = 0;

    _ASARRAY_PLAIN::erase();
    tmp_keys.split(line,delims,zero_str,quotations,escape);

    for ( i=0 ; i < tmp_keys.length() ; i++ ) {
	/* 同じキー名がきた場合，name, name(1), name(2), ... のようにする */
	int ix=0;
	tstring r_key;
	r_key.assign(tmp_keys.cstr(i));
	while ( 0 <= this->index_rec.index(r_key.cstr(),0) ) {
	    ix ++;
	    r_key.printf("%s(%d)",tmp_keys.cstr(i),ix);
	}
	_ASARRAY_PLAIN::append(r_key.cstr(),zero);
    }
    return *this;
}

_ASARRAY_PLAIN &_ASARRAY_PLAIN::split_keys( const char *line, 
					    const char *delims,
					    bool zero_str )
{
    return this->split_keys(line,delims,zero_str,NULL,'\0');
}

#ifdef _ASARRAY_PLAIN__USE_SPLIT
_ASARRAY_PLAIN &_ASARRAY_PLAIN::split_values( const char *line, 
					      const char *delims, 
					      bool zero_str, 
					      const char *quotations,
					      int escape )
{
    size_t i;
    tarray_tstring tmp_values;

    tmp_values.split(line,delims,zero_str,quotations,escape);

    for ( i=0 ; i < tmp_values.length() && i < this->length() ; i++ ) {
	this->at(i) = 
	    (_ASARRAY_PLAIN__TYPE)(tmp_values.at(i)._ASARRAY_PLAIN__ATOX());
    }
    return *this;
}

_ASARRAY_PLAIN &_ASARRAY_PLAIN::split_values( const char *line, 
					      const char *delims,
					      bool zero_str )
{
    return this->split_values(line,delims,zero_str,NULL,'\0');
}
#endif	/* _ASARRAY_PLAIN__USE_SPLIT */

#ifdef _ASARRAY_PLAIN__CREATE_STRUCT
_ASARRAY_PLAIN &_ASARRAY_PLAIN::append( const _ASARRAY_PLAIN__ASARRDEF elements[] )
{
    if ( elements != NULL ) {
	size_t i;
	for ( i=0 ; elements[i].key != NULL ; i++ ) {
	    this->append(elements[i].key,elements[i].value);
	}
    }
    return *this;
}

_ASARRAY_PLAIN &_ASARRAY_PLAIN::append( const _ASARRAY_PLAIN__ASARRDEF elements[],
					size_t n )
{
    if ( 0 < n && elements != NULL ) {
	size_t i;
	for ( i=0 ; i < n && elements[i].key != NULL ; i++ ) {
	    this->append(elements[i].key,elements[i].value);
	}
    }
    return *this;
}
#endif	/* _ASARRAY_PLAIN__CREATE_STRUCT */

_ASARRAY_PLAIN &_ASARRAY_PLAIN::append( const _ASARRAY_PLAIN &src )
{
    size_t i;
    for ( i=0 ; i < src.length() ; i++ ) {
	this->append(src.key(i),src.at_cs(i));
    }
    return *this;
}

#define FUNC_NAME "append"
_ASARRAY_PLAIN &_ASARRAY_PLAIN::append( const char *key, 
					const _ASARRAY_PLAIN__TYPE &val )
{
    if ( key != NULL ) {
	if ( this->index_rec.index(key,0) < 0 ) {
	    this->value_rec->append(&val,1);
	    this->key_rec.append(key,1);
	    this->index_rec.append(key,this->key_rec.length()-1);
	}
	else {
	    err_report1(FUNC_NAME,"WARNING","cannot append key '%s'",key);
	}
    }
    return *this;
}
#undef FUNC_NAME

#ifdef _ASARRAY_PLAIN__USE_VARG
#define FUNC_NAME "append"
_ASARRAY_PLAIN &_ASARRAY_PLAIN::append( const char *key0, _ASARRAY_PLAIN__VATYPE val0,
					const char *key1, ... )
{
    va_list ap;
    va_start(ap,key1);
    try {
	this->vappend(key0,val0,key1,ap);
    }
    catch (...) {
	va_end(ap);
        err_throw(FUNC_NAME,"FATAL","this->vappend() failed");
    }
    va_end(ap);
    return *this;
}
#undef FUNC_NAME

_ASARRAY_PLAIN &_ASARRAY_PLAIN::vappend( const char *key0, 
					 _ASARRAY_PLAIN__VATYPE val0,
					 const char *key1, va_list ap )
{
    this->append(key0,val0);
    if ( key0 != NULL && key1 != NULL ) {
	_ASARRAY_PLAIN__TYPE val1 = va_arg(ap,_ASARRAY_PLAIN__VATYPE);
	this->append(key1,val1);
	while ( 1 ) {
	    const char *keyx = va_arg(ap,char *);
	    _ASARRAY_PLAIN__TYPE valx = va_arg(ap,_ASARRAY_PLAIN__VATYPE);
	    if ( keyx == NULL ) break;
	    this->append(keyx,valx);
	}
    }
    return *this;
}
#endif	/* _ASARRAY_PLAIN__USE_VARG */

#ifdef _ASARRAY_PLAIN__CREATE_STRUCT
#define FUNC_NAME "insert"
_ASARRAY_PLAIN &_ASARRAY_PLAIN::insert( const char *key, 
				    const _ASARRAY_PLAIN__ASARRDEF elements[] )
{
    if ( key != NULL && elements != NULL ) {
	size_t i;
	if ( this->index_rec.index(key,0) < 0 ) {
	    err_report1(FUNC_NAME,"WARNING","unknown key '%s'",key);
	    goto quit;
	}
	for ( i=0 ; elements[i].key != NULL ; i++ ) {
	    this->insert(key,elements[i].key,elements[i].value);
	}
    }
 quit:
    return *this;
}

_ASARRAY_PLAIN &_ASARRAY_PLAIN::insert( const char *key, 
				    const _ASARRAY_PLAIN__ASARRDEF elements[],
				    size_t n )
{
    if ( key != NULL && 0 < n && elements != NULL ) {
	size_t i;
	if ( this->index_rec.index(key,0) < 0 ) {
	    err_report1(FUNC_NAME,"WARNING","unknown key '%s'",key);
	    goto quit;
	}
	for ( i=0 ; i < n && elements[i].key != NULL ; i++ ) {
	    this->insert(key,elements[i].key,elements[i].value);
	}
    }
 quit:
    return *this;
}
#undef FUNC_NAME
#endif	/* _ASARRAY_PLAIN__CREATE_STRUCT */

#define FUNC_NAME "insert"
_ASARRAY_PLAIN &_ASARRAY_PLAIN::insert( const char *key, 
					const _ASARRAY_PLAIN &src )
{
    if ( key != NULL ) {
	size_t i;
	if ( this->index_rec.index(key,0) < 0 ) {
	    err_report1(FUNC_NAME,"WARNING","unknown key '%s'",key);
	    goto quit;
	}
	for ( i=0 ; i < src.length() ; i++ ) {
	    this->insert(key,src.key(i),src.at_cs(i));
	}
    }
 quit:
    return *this;
}

_ASARRAY_PLAIN &_ASARRAY_PLAIN::insert( const char *key,
					const char *newkey, 
					_ASARRAY_PLAIN__TYPE newval )
{
    if ( key != NULL && newkey != NULL ) {
	size_t i;
	ssize_t idx = this->index_rec.index(key,0);
	if ( idx < 0 ) {
	    err_report1(FUNC_NAME,"WARNING","unknown key '%s'",key);
	    goto quit;
	}
	if ( 0 <= this->index_rec.index(newkey,0) ) {
	    err_report1(FUNC_NAME,"WARNING","cannot insert key '%s'",newkey);
	    goto quit;
	}
	this->value_rec->insert(idx,&newval,1);
	this->key_rec.insert(idx,newkey,1);
	this->index_rec.append(newkey,idx);
	for ( i=idx+1 ; i < this->key_rec.length() ; i++ ) {
	    this->index_rec.update(this->key_rec.cstr(i),i-1,i);
	}
    }
 quit:
    return *this;
}
#undef FUNC_NAME

#ifdef _ASARRAY_PLAIN__USE_VARG
#define FUNC_NAME "insert"
_ASARRAY_PLAIN &_ASARRAY_PLAIN::insert( const char *key,
					const char *key0, 
					_ASARRAY_PLAIN__VATYPE val0,
					const char *key1, ... )
{
    va_list ap;
    va_start(ap,key1);
    try {
	this->vinsert(key,key0,val0,key1,ap);
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
_ASARRAY_PLAIN &_ASARRAY_PLAIN::vinsert( const char *key,
					 const char *key0, 
					 _ASARRAY_PLAIN__VATYPE val0,
					 const char *key1, va_list ap )
{
    if ( key == NULL ) goto quit;
    if ( this->index_rec.index(key,0) < 0 ) {
	err_report1(FUNC_NAME,"WARNING","unknown key '%s'",key);
	goto quit;
    }
    this->insert(key,key0,val0);
    if ( key0 != NULL && key1 != NULL ) {
	_ASARRAY_PLAIN__TYPE val1 = va_arg(ap,_ASARRAY_PLAIN__VATYPE);
	this->insert(key,key1,val1);
	while ( 1 ) {
	    const char *keyx = va_arg(ap,char *);
	    _ASARRAY_PLAIN__TYPE valx = va_arg(ap,_ASARRAY_PLAIN__VATYPE);
	    if ( keyx == NULL ) break;
	    this->insert(key,keyx,valx);
	}
    }
 quit:
    return *this;
}
#undef FUNC_NAME
#endif	/* _ASARRAY_PLAIN__USE_VARG */

_ASARRAY_PLAIN &_ASARRAY_PLAIN::erase()
{
    this->index_rec.init();
    this->key_rec.erase();
    this->value_rec->erase();
    return *this;
}

#define FUNC_NAME "erase"
_ASARRAY_PLAIN &_ASARRAY_PLAIN::erase( const char *key, size_t num_elements )
{
    size_t i, maxels;
    ssize_t idx;
    if ( key == NULL ) goto quit;
    idx = this->index_rec.index(key,0);
    if ( idx < 0 ) {
	err_report1(FUNC_NAME,"WARNING","unknown key '%s'",key);
	goto quit;
    }
    maxels = this->key_rec.length() - idx;
    if ( maxels < num_elements ) num_elements = maxels;

    for ( i=idx ; i < idx + num_elements ; i++ ) {
	this->index_rec.erase(this->key_rec.cstr(i),i);
    }
    for ( ; i < this->key_rec.length() ; i++ ) {
	this->index_rec.update(this->key_rec.cstr(i),i,i-num_elements);
    }

    this->key_rec.erase(idx,num_elements);
    this->value_rec->erase(idx,num_elements);

 quit:
    return *this;
}
#undef FUNC_NAME

_ASARRAY_PLAIN &_ASARRAY_PLAIN::swap( _ASARRAY_PLAIN &sobj )
{
    this->key_rec.swap( sobj.key_rec );
    this->value_rec->swap( *(sobj.value_rec) );
    this->index_rec.swap( sobj.index_rec );
    return *this;
}

/* */
const _ASARRAY_PLAIN__TYPE *_ASARRAY_PLAIN::carray() const
{
    return this->value_ptr;
}

#define FUNC_NAME "at"
_ASARRAY_PLAIN__TYPE &_ASARRAY_PLAIN::at( const char *key )
{
    ssize_t idx;
    if ( key == NULL ) {
        err_throw(FUNC_NAME,"ERROR","NULL key...?");
    }
    idx = this->index_rec.index(key,0);
    if ( idx < 0 ) {
	size_t i;
	_ASARRAY_PLAIN__TYPE zero;
	unsigned char *p = (unsigned char *)&zero;
	for ( i=0 ; i < sizeof(_ASARRAY_PLAIN__TYPE) ; i++ )
	    p[i] = 0;
	_ASARRAY_PLAIN::append(key,zero);
	idx = this->index_rec.index(key,0);
    }
    if ( idx < 0 ) {
        err_throw1(FUNC_NAME,"ERROR","not found key '%s'",key);
    }
    return this->value_ptr[idx];
}
#undef FUNC_NAME

#define FUNC_NAME "atf"
_ASARRAY_PLAIN__TYPE &_ASARRAY_PLAIN::atf( const char *fmt, ... )
{
    va_list ap;
    va_start(ap,fmt);
    try {
	_ASARRAY_PLAIN__TYPE &ret = this->vatf(fmt,ap);
	va_end(ap);
	return ret;
    }
    catch (...) {
	va_end(ap);
        err_throw(FUNC_NAME,"FATAL","this->vatf() failed");
    }
}
#undef FUNC_NAME

#define FUNC_NAME "vatf"
_ASARRAY_PLAIN__TYPE &_ASARRAY_PLAIN::vatf( const char *fmt, va_list ap )
{
    tstring buf;

    if ( fmt == NULL ) return this->at(fmt);

    buf.vprintf(fmt,ap);
    return this->at(buf.cstr());
}
#undef FUNC_NAME

_ASARRAY_PLAIN__TYPE &_ASARRAY_PLAIN::element( const char *key )
{
    return _ASARRAY_PLAIN::at(key);
}

size_t _ASARRAY_PLAIN::length() const
{
    return this->key_rec.length();
}

size_t _ASARRAY_PLAIN::size() const
{
    return this->key_rec.size();
}

ssize_t _ASARRAY_PLAIN::index( const char *key ) const
{
    if ( key == NULL ) return -1;
    return this->index_rec.index(key,0);
}

#define FUNC_NAME "indexf"
ssize_t _ASARRAY_PLAIN::indexf( const char *fmt, ... ) const
{
    ssize_t ret;
    va_list ap;
    va_start(ap,fmt);
    try {
	ret = this->vindexf(fmt,ap);
    }
    catch (...) {
	va_end(ap);
        err_throw(FUNC_NAME,"FATAL","this->vindexf() failed");
    }
    va_end(ap);
    return ret;
}
#undef FUNC_NAME

#define FUNC_NAME "vindexf"
ssize_t _ASARRAY_PLAIN::vindexf( const char *fmt, va_list ap ) const
{
    tstring buf;

    if ( fmt == NULL ) return this->index(fmt);

    buf.vprintf(fmt,ap);
    return this->index(buf.cstr());
}
#undef FUNC_NAME

const char *_ASARRAY_PLAIN::key( size_t index ) const
{
    if ( index < 0 || this->length() <= index ) return NULL;
    else return this->key_rec.cstr(index);
}

#define FUNC_NAME "at"
_ASARRAY_PLAIN__TYPE &_ASARRAY_PLAIN::at( size_t index )
{
    if ( index < 0 || this->length() <= index ) {
        err_throw1(FUNC_NAME,"ERROR","invalid index: %d",index);
    }
    return this->value_ptr[index];
}
#undef FUNC_NAME

#define FUNC_NAME "element"
_ASARRAY_PLAIN__TYPE &_ASARRAY_PLAIN::element( size_t index )
{
    if ( index < 0 || this->length() <= index ) {
        err_throw1(FUNC_NAME,"ERROR","invalid index: %d",index);
    }
    return this->value_ptr[index];
}
#undef FUNC_NAME

#define FUNC_NAME "at_cs"
const _ASARRAY_PLAIN__TYPE &_ASARRAY_PLAIN::at_cs( size_t index ) const
{
    if ( index < 0 || this->length() <= index ) {
        err_throw1(FUNC_NAME,"ERROR","invalid index: %d",index);
    }
    return this->value_ptr[index];
}
#undef FUNC_NAME

}	/* namespace sli */

#ifdef _ASARRAY_PLAIN__USE_SPLIT
# undef _ASARRAY_PLAIN__ATOX
#endif

#endif	/* end of source */

#undef _ASARRAY_PLAIN__ASARRDEF
#undef _ASARRAY_PLAIN__TYPE
#undef _ASARRAY_PLAIN

#ifdef _ASARRAY_PLAIN__USE_VARG
# undef _ASARRAY_PLAIN__VATYPE
#endif

#ifdef _ASARRAY_PLAIN__CREATE_STRUCT
# undef _ASARRAY_PLAIN__ASARRDEF
#endif
