/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2009-01-10 15:33:15 cyamauch> */

#ifndef _TARRAY_PLAIN_H
#define _TARRAY_PLAIN_H 1

#include <stddef.h>
#include <sys/types.h>

#include "slierr.h"

namespace sli
{
  class tarray_plain
  {
  protected:
    tarray_plain();
  public:
    tarray_plain( size_t nbytes, void **extptr_ptr = NULL );
    tarray_plain( const tarray_plain &obj );
    virtual ~tarray_plain();
    /* */
    virtual tarray_plain &operator=(const tarray_plain &obj);
    virtual bool operator==(const tarray_plain &obj) const;
    virtual bool operator!=(const tarray_plain &obj) const;
    virtual tarray_plain &init();
    virtual tarray_plain &init(size_t n_bytes);
    virtual tarray_plain &init(const tarray_plain &obj);
    virtual tarray_plain &assign_default(const void *value_ptr);
    virtual tarray_plain &clean();
    virtual tarray_plain &clean( const void *value_ptr );
    virtual tarray_plain &assign( size_t n );
    virtual tarray_plain &assign( const void *values_ptr, size_t n );
    virtual tarray_plain &assign( const tarray_plain &src, size_t idx2 = 0 );
    virtual tarray_plain &assign( const tarray_plain &src, size_t idx2, size_t n2 );
    virtual tarray_plain &append( size_t n );
    virtual tarray_plain &append( const void *values_ptr, size_t n );
    virtual tarray_plain &append( const tarray_plain &src, size_t idx2 = 0 );
    virtual tarray_plain &append( const tarray_plain &src, size_t idx2, size_t n2 );
    virtual tarray_plain &insert( size_t index, size_t n );
    virtual tarray_plain &insert( size_t index, const void *values_ptr, size_t n );
    virtual tarray_plain &insert( size_t index, 
			    const tarray_plain &src, size_t idx2 = 0 );
    virtual tarray_plain &insert( size_t index, 
			    const tarray_plain &src, size_t idx2, size_t n2 );
    virtual tarray_plain &replace( size_t idx1, size_t n1, size_t n2 );
    virtual tarray_plain &replace( size_t idx1, size_t n1,
			     const void *values_ptr, size_t n2 );
    virtual tarray_plain &replace( size_t idx1, size_t n1,
			     const tarray_plain &src, size_t idx2 = 0 );
    virtual tarray_plain &replace( size_t idx1, size_t n1,
			     const tarray_plain &src, size_t idx2, size_t n2 );
    virtual tarray_plain &put( size_t index, size_t n );
    virtual tarray_plain &put( size_t index, const void *values_ptr, size_t n );
    virtual tarray_plain &put( size_t index, const tarray_plain &src, size_t idx2 = 0 );
    virtual tarray_plain &put( size_t index, 
			 const tarray_plain &src, size_t idx2, size_t n2 );
    virtual tarray_plain &erase();
    virtual tarray_plain &erase( size_t index, size_t num_elements = 1 );
    virtual tarray_plain &resize( size_t new_num_elements );
    virtual tarray_plain &swap( tarray_plain &sobj );
    virtual ssize_t getdata( size_t index, 
			     void *dest_buf, size_t buf_size ) const;
    virtual ssize_t getdata( void *dest_buf, size_t buf_size ) const;
    virtual ssize_t copy( size_t index, size_t n, tarray_plain &dest ) const;
    virtual ssize_t copy( size_t index, tarray_plain &dest ) const;
    virtual ssize_t copy( tarray_plain &dest ) const;
    virtual tarray_plain &register_extptr(void **extptr_ptr);
    virtual tarray_plain &register_extptr(const void **extptr_ptr);
    /* */
    virtual const void *default_value() const;
    virtual const void *carray() const;
    virtual void *data_ptr();
    virtual size_t length() const;
    virtual size_t size() const;
    virtual size_t byte_length() const;
    virtual size_t bytes() const;	/* num_bytes を返す */
  private:
    bool is_my_buffer( const void *ptr ) const;
    size_t num_bytes;		/* 構造体のバイト数 */
    void *default_rec;		/* 構造体の初期値 */
    size_t num_elem;		/* 配列の個数 */
    void *arr_rec;		/* 配列へのポインタ */
    void **to_update;		/* 外部ポインタのアドレス */
  };
}


#endif	/* _TARRAY_PLAIN_H */
