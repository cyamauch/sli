/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2009-01-10 15:42:44 cyamauch> */

#define CLASS_NAME "tarray_plain"

#include "tarray_plain.h"

#include <stdlib.h>

#include "private/err_report.h"

#include "private/c_memmove.h"
#include "private/c_memcpy.h"

#if 0
#ifndef _TARRAY_PLAIN__C_MEMMOVE
#define _TARRAY_PLAIN__C_MEMMOVE 1
static void *c_memmove(void *dest, const void *src, size_t n)
{
    unsigned char *d = (unsigned char *)dest;
    const unsigned char *s = (const unsigned char *)src;
    if (dest < src) {
	while ( 0 < n ) {
	    n--;
	    *d = *s;
	    d++;  s++;
	}
    }
    else {
	while ( 0 < n ) {
	    n--;
	    d[n] = s[n];
	}
    }
    return dest;
}
#endif
#endif

namespace sli
{

/* constructor */
tarray_plain::tarray_plain()
{
    this->num_bytes = 1;
    this->default_rec = NULL;
    this->num_elem = 0;
    this->arr_rec = NULL;
    this->to_update = NULL;
    return;
}

tarray_plain::tarray_plain( size_t nbytes, void **extptr_ptr )
{
    this->num_bytes = nbytes;
    this->default_rec = NULL;
    this->num_elem = 0;
    this->arr_rec = NULL;
    this->to_update = extptr_ptr;
    if ( this->to_update != NULL ) *(this->to_update) = this->arr_rec;
    return;
}

/* copy constructor */
tarray_plain::tarray_plain(const tarray_plain &obj)
{
    this->num_bytes = 0;
    this->default_rec = NULL;
    this->num_elem = 0;
    this->arr_rec = NULL;
    this->to_update = NULL;
    this->init(obj);
    return;
}

/* destructor */
tarray_plain::~tarray_plain()
{
    if ( this->default_rec != NULL ) {
	free(this->default_rec);
    }
    if ( this->arr_rec != NULL ) {
	free(this->arr_rec);
    }
    return;
}

tarray_plain &tarray_plain::operator=(const tarray_plain &obj)
{
    this->init(obj);
    return *this;
}

bool tarray_plain::operator==(const tarray_plain &obj) const
{
    size_t i, len = this->num_elem * this->num_bytes;
    if ( len != obj.num_elem * obj.num_bytes ) return false;
    for ( i=0 ; i < len ; i++ ) {
	if ( ((unsigned char *)this->arr_rec)[i] != 
	     ((unsigned char *)obj.arr_rec)[i] ) return false;
    }
    return true;
}

bool tarray_plain::operator!=(const tarray_plain &obj) const
{
    size_t i, len = this->num_elem * this->num_bytes;
    if ( len != obj.num_elem * obj.num_bytes ) return true;
    for ( i=0 ; i < len ; i++ ) {
	if ( ((unsigned char *)this->arr_rec)[i] != 
	     ((unsigned char *)obj.arr_rec)[i] ) return true;
    }
    return false;
}

/*
 * public method
 */

tarray_plain &tarray_plain::init()
{
    return tarray_plain::init((size_t)0);
}

tarray_plain &tarray_plain::init(size_t n_bytes)
{
    if ( this->arr_rec != NULL ) {
	free(this->arr_rec);
	this->arr_rec = NULL;
	if ( this->to_update != NULL ) *(this->to_update) = this->arr_rec;
    }
    this->num_elem = 0;
    if ( this->default_rec != NULL ) {
	free(this->default_rec);
	this->default_rec = NULL;
    }
    if ( 0 < n_bytes ) this->num_bytes = n_bytes;
    return *this;
}

#define FUNC_NAME "init"
tarray_plain &tarray_plain::init(const tarray_plain &obj)
{
    if ( &obj == this ) return *this;

    tarray_plain::init();
    if ( 0 < obj.num_bytes && obj.default_rec != NULL ) {
	this->default_rec = malloc(obj.num_bytes);
	if ( this->default_rec == NULL ) {
	    err_throw(FUNC_NAME,"FATAL","malloc() failed");
	}
	c_memcpy(this->default_rec, obj.default_rec, obj.num_bytes);
    }

    if ( 0 < obj.num_bytes && 0 < obj.num_elem && obj.arr_rec != NULL ) {
	size_t sz = obj.num_bytes * obj.num_elem;
	this->arr_rec = malloc(sz);
	if ( this->to_update != NULL ) *(this->to_update) = this->arr_rec;
	if ( this->arr_rec == NULL ) {
	    if ( this->default_rec != NULL ) free(this->default_rec);
	    this->default_rec = NULL;
	    err_throw(FUNC_NAME,"FATAL","malloc() failed");
	}
	c_memcpy(this->arr_rec, obj.arr_rec, sz);
    }

    this->num_bytes = obj.num_bytes;
    this->num_elem = obj.num_elem;

    return *this;
}
#undef FUNC_NAME

#define FUNC_NAME "assign_default"
tarray_plain &tarray_plain::assign_default(const void *value_ptr)
{
    if ( value_ptr != NULL && 0 < this->num_bytes ) {
	if ( this->default_rec != NULL ) {
	    c_memmove(this->default_rec,value_ptr,this->num_bytes);
	}
	else {
	    this->default_rec = malloc(this->num_bytes);
	    if ( this->default_rec == NULL ) {
		err_throw(FUNC_NAME,"FATAL","malloc() failed");
	    }
	    c_memcpy(this->default_rec,value_ptr,this->num_bytes);
	}
    }
    return *this;
}
#undef FUNC_NAME

tarray_plain &tarray_plain::clean()
{
    if ( this->default_rec != NULL ) {
	return this->clean(this->default_rec);
    }
    else {
	size_t i;
	size_t sz = this->num_bytes * this->num_elem;
	for ( i=0 ; i < sz ; i++ ) {
	    ((unsigned char *)this->arr_rec)[i] = 0;
	}
	return *this;
    }
}

tarray_plain &tarray_plain::clean(const void *value_ptr)
{
    if ( value_ptr != NULL ) {
	size_t i, k;
	for ( i=0, k=0 ; i < this->num_elem ; i++ ) {
	    size_t j;
	    for ( j=0 ; j < this->num_bytes ; j++, k++ ) {
		((unsigned char *)this->arr_rec)[k] = 
		    ((const unsigned char *)value_ptr)[j];
	    }
	}
    }
    return *this;
}

tarray_plain &tarray_plain::assign( size_t n )
{
    return tarray_plain::replace(0, this->num_elem, n);
}

tarray_plain &tarray_plain::assign( const void *values_ptr, size_t n )
{
    return tarray_plain::replace(0, this->num_elem, values_ptr, n);
}

tarray_plain &tarray_plain::assign( const tarray_plain &src, size_t idx2 )
{
    return this->assign(src, idx2, src.num_elem);
}

#define FUNC_NAME "assign"
tarray_plain &tarray_plain::assign( const tarray_plain &src, size_t idx2, size_t n2 )
{
    if ( this->num_bytes != src.num_bytes ) {
	err_report(FUNC_NAME,"WARNING","Invalid src.bytes(); ignored");
    }
    else {
	size_t len = src.num_elem;
	if ( idx2 < len ) {
	    size_t max = len - idx2;
	    if ( max < n2 ) n2 = max;
	    this->assign(
		      (unsigned char *)src.arr_rec + idx2 * src.num_bytes, n2);
	}
    }
    return *this;
}
#undef FUNC_NAME

tarray_plain &tarray_plain::append( size_t n )
{
    return tarray_plain::replace(this->num_elem, 0, n);
}

tarray_plain &tarray_plain::append( const void *values_ptr, size_t n )
{
    return tarray_plain::replace(this->num_elem, 0, values_ptr, n);
}

tarray_plain &tarray_plain::append( const tarray_plain &src, size_t idx2 )
{
    return this->append(src, idx2, src.num_elem);
}

#define FUNC_NAME "append"
tarray_plain &tarray_plain::append( const tarray_plain &src, size_t idx2, size_t n2 )
{
    if ( this->num_bytes != src.num_bytes ) {
	err_report(FUNC_NAME,"WARNING","Invalid src.bytes(); ignored");
    }
    else {
	size_t len = src.num_elem;
	if ( idx2 < len ) {
	    size_t max = len - idx2;
	    if ( max < n2 ) n2 = max;
	    this->append( 
		      (unsigned char *)src.arr_rec + idx2 * src.num_bytes, n2);
	}
    }
    return *this;
}
#undef FUNC_NAME

tarray_plain &tarray_plain::insert( size_t index, size_t n )
{
    return tarray_plain::replace(index, 0, n);
}

tarray_plain &tarray_plain::insert( size_t index, const void *values_ptr, size_t n )
{
    return tarray_plain::replace(index, 0, values_ptr, n);
}

tarray_plain &tarray_plain::insert( size_t index, const tarray_plain &src, size_t idx2 )
{
    return this->insert(index, src, idx2, src.num_elem);
}

#define FUNC_NAME "insert"
tarray_plain &tarray_plain::insert( size_t index, 
			const tarray_plain &src, size_t idx2, size_t n2 )
{
    if ( this->num_bytes != src.num_bytes ) {
	err_report(FUNC_NAME,"WARNING","Invalid src.bytes(); ignored");
    }
    else {
	size_t len = src.num_elem;
	if ( idx2 < len ) {
	    size_t max = len - idx2;
	    if ( max < n2 ) n2 = max;
	    this->insert(index,
		      (unsigned char *)src.arr_rec + idx2 * src.num_bytes, n2);
	}
    }
    return *this;
}
#undef FUNC_NAME


#define FUNC_NAME "replace"
tarray_plain &tarray_plain::replace( size_t idx1, size_t n1, size_t n2 )
{
    size_t crt_num_elem = this->num_elem;
    size_t new_num_elem;
    size_t sz, i, k, off;

    if ( crt_num_elem < idx1 ) idx1 = crt_num_elem;
    if ( crt_num_elem < idx1 + n1 ) n1 = crt_num_elem - idx1;

    if ( n1 < n2 ) {
	new_num_elem = crt_num_elem + (n2 - n1);
    }
    else if ( n2 < n1 ) {
	new_num_elem = crt_num_elem - (n1 - n2);
	c_memmove( (unsigned char *)this->arr_rec
		   + (idx1 + n2) * this->num_bytes,
		   (unsigned char *)this->arr_rec
		   + (idx1 + n1) * this->num_bytes,
		   (crt_num_elem - idx1 - n1) * this->num_bytes );
	this->num_elem = new_num_elem;
    }
    else new_num_elem = crt_num_elem;

    if ( new_num_elem != crt_num_elem ) {
	void *new_buf;
	new_buf = realloc(this->arr_rec,
			  this->num_bytes * new_num_elem);
	if ( 0 < new_num_elem && new_buf == NULL ) {
	    err_throw(FUNC_NAME,"FATAL","realloc() failed");
	}
	this->arr_rec = new_buf;
	if ( this->to_update != NULL ) *(this->to_update) = this->arr_rec;
    }
    if ( n1 < n2 ) {
	c_memmove( (unsigned char *)this->arr_rec
		   + (idx1 + n2) * this->num_bytes,
		   (unsigned char *)this->arr_rec
		   + (idx1 + n1) * this->num_bytes,
		   (crt_num_elem - idx1 - n1) * this->num_bytes );
	this->num_elem = new_num_elem;
    }

    off = this->num_bytes * idx1;
    if ( this->default_rec != NULL ) {
	for ( i=0, k=0 ; i < n2 ; i++ ) {
	    size_t j;
	    for ( j=0 ; j < this->num_bytes ; j++, k++ ) {
		((unsigned char *)this->arr_rec)[off + k] = 
		    ((unsigned char *)this->default_rec)[j];
	    }
	}
    }
    else {
	sz = this->num_bytes * n2;
	for ( i=0 ; i < sz ; i++ ) {
	    ((unsigned char *)this->arr_rec)[off + i] = 0;
	}
    }

    return *this;
}
#undef FUNC_NAME

#define FUNC_NAME "replace"
tarray_plain &tarray_plain::replace( size_t idx1, size_t n1, 
			 const void *values_ptr, size_t n2 )
{
    size_t crt_num_elem = this->num_elem;
    size_t new_num_elem;
    char *tmp_src = NULL;

    if ( 0 < n2 && values_ptr == NULL ) return *this;

    if ( 0 < n2 && this->is_my_buffer(values_ptr) == true ) {
	size_t sz;
	/* values_ptr が this->arr_rec の内容を指している場合  */
	sz = this->num_bytes * n2;
	tmp_src = (char *)malloc(sizeof(*tmp_src) * sz);
	if ( tmp_src == NULL ) {
	    err_throw(FUNC_NAME,"FATAL","malloc() failed");
	}
	c_memcpy(tmp_src, values_ptr, sz);
	values_ptr = tmp_src;
    }

    if ( crt_num_elem < idx1 ) idx1 = crt_num_elem;
    if ( crt_num_elem < idx1 + n1 ) n1 = crt_num_elem - idx1;

    if ( n1 < n2 ) {
	new_num_elem = crt_num_elem + (n2 - n1);
    }
    else if ( n2 < n1 ) {
	new_num_elem = crt_num_elem - (n1 - n2);
	c_memmove( (unsigned char *)this->arr_rec
		   + (idx1 + n2) * this->num_bytes,
		   (unsigned char *)this->arr_rec
		   + (idx1 + n1) * this->num_bytes,
		   (crt_num_elem - idx1 - n1) * this->num_bytes );
	this->num_elem = new_num_elem;
    }
    else new_num_elem = crt_num_elem;

    if ( new_num_elem != crt_num_elem ) {
	void *new_buf;
	new_buf = realloc(this->arr_rec,
			  this->num_bytes * new_num_elem);
	if ( 0 < new_num_elem && new_buf == NULL ) {
	    if ( tmp_src != NULL ) free(tmp_src);
	    err_throw(FUNC_NAME,"FATAL","realloc() failed");
	}
	this->arr_rec = new_buf;
	if ( this->to_update != NULL ) *(this->to_update) = this->arr_rec;
    }
    if ( n1 < n2 ) {
	c_memmove( (unsigned char *)this->arr_rec
		   + (idx1 + n2) * this->num_bytes,
		   (unsigned char *)this->arr_rec
		   + (idx1 + n1) * this->num_bytes,
		   (crt_num_elem - idx1 - n1) * this->num_bytes );
	this->num_elem = new_num_elem;
    }
    c_memcpy( (unsigned char *)this->arr_rec + idx1 * this->num_bytes,
	      values_ptr, n2 * this->num_bytes );

    if ( tmp_src != NULL ) free(tmp_src);

    return *this;
}
#undef FUNC_NAME

tarray_plain &tarray_plain::replace(size_t idx1, size_t n1, const tarray_plain &src, size_t idx2)
{
    return this->replace(idx1, n1, src, idx2, src.num_elem);
}

#define FUNC_NAME "replace"
tarray_plain &tarray_plain::replace( size_t idx1, size_t n1,
			 const tarray_plain &src, size_t idx2, size_t n2 )
{
    if ( this->num_bytes != src.num_bytes ) {
	err_report(FUNC_NAME,"WARNING","Invalid src.bytes(); ignored");
    }
    else {
	size_t len = src.num_elem;
	if ( idx2 < len ) {
	    size_t max = len - idx2;
	    if ( max < n2 ) n2 = max;
	    this->replace(idx1, n1,
		      (unsigned char *)src.arr_rec + idx2 * src.num_bytes, n2);
	}
    }
    return *this;
}
#undef FUNC_NAME

tarray_plain &tarray_plain::put( size_t index, size_t n )
{
    size_t sz, i, k, off;

    if ( 0 < n ) {
	if ( this->num_elem < index + n ) 
	    tarray_plain::replace(this->num_elem,0, (index + n) - this->num_elem);

	off = this->num_bytes * index;
	if ( this->default_rec != NULL ) {
	    for ( i=0, k=0 ; i < n ; i++ ) {
		size_t j;
		for ( j=0 ; j < this->num_bytes ; j++, k++ ) {
		    ((unsigned char *)this->arr_rec)[off + k] = 
			((unsigned char *)this->default_rec)[j];
		}
	    }
	}
	else {
	    sz = this->num_bytes * n;
	    for ( i=0 ; i < sz ; i++ ) {
		((unsigned char *)this->arr_rec)[off + i] = 0;
	    }
	}
    }

    return *this;
}

#define FUNC_NAME "put"
tarray_plain &tarray_plain::put( size_t index, const void *values_ptr, size_t n )
{
    char *tmp_src = NULL;

    if ( 0 < n && values_ptr == NULL ) return *this;

    if ( 0 < n && this->is_my_buffer(values_ptr) == true ) {
	size_t sz;
	sz = this->num_bytes * n;
	tmp_src = (char *)malloc(sizeof(*tmp_src) * sz);
	if ( tmp_src == NULL ) {
	    err_throw(FUNC_NAME,"FATAL","malloc() failed");
	}
	c_memcpy(tmp_src, values_ptr, sz);
	values_ptr = tmp_src;
    }
    
    try {
	if ( this->num_elem < index + n ) 
	    tarray_plain::replace(this->num_elem,0, (index + n) - this->num_elem);
	tarray_plain::replace(index,n, values_ptr,n);
    }
    catch (...) {
	if ( tmp_src != NULL ) free(tmp_src);
	    err_throw(FUNC_NAME,"FATAL","resize() failed");
    }
    
    if ( tmp_src != NULL ) free(tmp_src);

    return *this;
}
#undef FUNC_NAME

tarray_plain &tarray_plain::put( size_t index, const tarray_plain &src, size_t idx2 )
{
    return this->put(index, src, idx2, src.num_elem);
}

#define FUNC_NAME "put"
tarray_plain &tarray_plain::put( size_t index, const tarray_plain &src, size_t idx2, size_t n2 )
{
    if ( this->num_bytes != src.num_bytes ) {
	err_report(FUNC_NAME,"WARNING","Invalid src.bytes(); ignored");
    }
    else {
	size_t len = src.num_elem;
	if ( idx2 < len ) {
	    size_t max = len - idx2;
	    if ( max < n2 ) n2 = max;
	    this->put(index,
		      (unsigned char *)src.arr_rec + idx2 * src.num_bytes, n2);
	}
    }
    return *this;
}
#undef FUNC_NAME

tarray_plain &tarray_plain::erase()
{
    return tarray_plain::replace(0, this->num_elem, 0);
}

tarray_plain &tarray_plain::erase( size_t index, size_t num_el )
{
    return tarray_plain::replace(index,num_el, (size_t)0);
}

#define FUNC_NAME "resize"
tarray_plain &tarray_plain::resize( size_t new_num_elements )
{
    if ( new_num_elements < this->num_elem ) {
        tarray_plain::replace( new_num_elements, this->num_elem - new_num_elements,
			 (size_t)0 );
    }
    else {
	tarray_plain::replace( this->num_elem, 0,
			 new_num_elements - this->num_elem );
    }
    return *this;
}
#undef FUNC_NAME

tarray_plain &tarray_plain::swap( tarray_plain &sobj )
{
    size_t tmp__num_bytes;
    void *tmp__default_rec;
    size_t tmp__num_elem;
    void *tmp__arr_rec;

    tmp__num_bytes = sobj.num_bytes;
    tmp__default_rec = sobj.default_rec;
    tmp__num_elem = sobj.num_elem;
    tmp__arr_rec = sobj.arr_rec;

    sobj.num_bytes = this->num_bytes;
    sobj.default_rec = this->default_rec;
    sobj.num_elem = this->num_elem;
    sobj.arr_rec = this->arr_rec;
    if ( sobj.to_update != NULL ) *(sobj.to_update) = sobj.arr_rec;

    this->num_bytes = tmp__num_bytes;
    this->default_rec = tmp__default_rec;
    this->num_elem = tmp__num_elem;
    this->arr_rec = tmp__arr_rec;
    if ( this->to_update != NULL ) *(this->to_update) = this->arr_rec;

    return *this;
}

ssize_t tarray_plain::getdata( size_t index, 
			 void *dest_buf, size_t buf_size ) const
{
    size_t i, off, max;
    if ( 0 < buf_size && dest_buf == NULL ) return -1;
    if ( this->length() <= index ) return -1;
    max = this->num_bytes * this->length();
    off = this->num_bytes * index;
    for ( i=0 ; i < buf_size && off + i < max ; i++ ) {
	((unsigned char *)dest_buf)[i] = 
	    ((unsigned char *)this->arr_rec)[off+i];
    }
    return max - off;
}

ssize_t tarray_plain::getdata( void *dest_buf, size_t buf_size ) const
{
    return this->getdata(0,dest_buf,buf_size);
}

ssize_t tarray_plain::copy( size_t index, size_t n, tarray_plain &dest ) const
{
    size_t len = this->length();

    if ( dest.bytes() != this->num_bytes ) {
	dest.init(this->num_bytes);
    }

    if ( len <= index ) {
	dest.erase();
	return -1;
    }

    if ( len - index < n ) n = len - index;

    dest.replace(0, dest.length(), 
		 (unsigned char *)this->arr_rec + this->num_bytes * index, n);

    return n;
}

ssize_t tarray_plain::copy( size_t index, tarray_plain &dest ) const
{
    return this->copy(index, this->length(), dest);
}

ssize_t tarray_plain::copy( tarray_plain &dest ) const
{
    return this->copy(0, this->length(), dest);
}

tarray_plain &tarray_plain::register_extptr( void **extptr_ptr )
{
    this->to_update = extptr_ptr;
    if ( this->to_update != NULL ) *(this->to_update) = this->arr_rec;
    return *this;
}

tarray_plain &tarray_plain::register_extptr( const void **extptr_ptr )
{
    this->to_update = (void **)extptr_ptr;
    if ( this->to_update != NULL ) *(this->to_update) = this->arr_rec;
    return *this;
}

const void *tarray_plain::default_value() const
{
    return this->default_rec;
}

const void *tarray_plain::carray() const
{
    return this->arr_rec;
}

void *tarray_plain::data_ptr()
{
    return this->arr_rec;
}

size_t tarray_plain::length() const
{
    return this->num_elem;
}

size_t tarray_plain::size() const
{
    return this->num_elem;
}

size_t tarray_plain::byte_length() const
{
    return this->num_bytes * this->num_elem;
}

size_t tarray_plain::bytes() const
{
    return this->num_bytes;
}

bool tarray_plain::is_my_buffer( const void *ptr ) const
{
    unsigned char *p0 = (unsigned char *)this->arr_rec;
    unsigned char *p1 = 
	(unsigned char *)this->arr_rec + this->num_bytes * this->num_elem;
    if ( ptr == NULL || this->arr_rec == NULL ) return false;
    if ( p0 <= (unsigned char *)ptr && (unsigned char *)ptr < p1 )
	return true;
    else
	return false;
}

}	/* namespace sli */


#include "private/c_memmove.c"
#include "private/c_memcpy.c"
