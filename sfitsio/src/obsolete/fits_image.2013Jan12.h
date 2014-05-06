
    /* not recommended */
    virtual long axis_size() const;
    virtual long size() const;
    virtual long size( long axis ) const;

    /* not recommended */
    virtual long col_size() const;
    virtual long row_size() const;
    virtual long layer_size() const;


    /* variable args version; not recommended.                               */
    /* 注意: 可変長引数の場合，数字をジカに指定する場合は，100L といったよう */
    /*       に型を明示的に指定する必要がある．「100」だけだと，64ビット機で */
    /*	     変な事になる．dvalue_v()等は，あまり使わない方がいいと思われる．*/
    virtual double dvalue_v( long num_axisx, 
			     long axis0, long axis1, long axis2, ... ) const;
    virtual double va_dvalue_v( long num_axisx, 
				long axis0, long axis1, long axis2, 
				va_list ap ) const;
    virtual long lvalue_v( long num_axisx, 
			   long axis0, long axis1, long axis2, ... ) const;
    virtual long va_lvalue_v( long num_axisx, 
			      long axis0, long axis1, long axis2, 
			      va_list ap ) const;
    virtual long long llvalue_v( long num_axisx, 
			       long axis0, long axis1, long axis2, ... ) const;
    virtual long long va_llvalue_v( long num_axisx, 
				    long axis0, long axis1, long axis2, 
				    va_list ap ) const;

    /* variable args version; not recommended. */
    virtual double double_value_v( long num_axisx, 
			       long axis0, long axis1, long axis2, ... ) const;
    virtual double va_double_value_v( long num_axisx, 
			long axis0, long axis1, long axis2, va_list ap ) const;
    virtual float float_value_v( long num_axisx, 
			       long axis0, long axis1, long axis2, ... ) const;
    virtual float va_float_value_v( long num_axisx, 
			long axis0, long axis1, long axis2, va_list ap ) const;
    virtual long long longlong_value_v( long num_axisx, 
				long axis0, long axis1, long axis2, ...) const;
    virtual long long va_longlong_value_v( long num_axisx, 
			long axis0, long axis1, long axis2, va_list ap ) const;
    virtual long long_value_v( long num_axisx, 
			       long axis0, long axis1, long axis2, ... ) const;
    virtual long va_long_value_v( long num_axisx, 
			long axis0, long axis1, long axis2, va_list ap ) const;
    virtual short short_value_v( long num_axisx,
			       long axis0, long axis1, long axis2, ... ) const;
    virtual short va_short_value_v( long num_axisx,
			long axis0, long axis1, long axis2, va_list ap ) const;
    virtual unsigned char byte_value_v( long num_axisx, 
				long axis0, long axis1, long axis2, ...) const;
    virtual unsigned char va_byte_value_v( long num_axisx, 
			long axis0, long axis1, long axis2, va_list ap ) const;


    /* variable args version; not recommended. */
    virtual fits_image &assign_v( double value, long num_axisx, 
				  long axis0, long axis1, long axis2, ... );
    virtual fits_image &va_assign_v( double value, long num_axisx, 
			      long axis0, long axis1, long axis2, va_list ap );

    /* variable args version; not recommended. */
    virtual fits_image &assign_double_v( double value, long num_axisx, 
				     long axis0, long axis1, long axis2, ... );
    virtual fits_image &va_assign_double_v( double value, long num_axisx, 
			      long axis0, long axis1, long axis2, va_list ap );
    virtual fits_image &assign_float_v( float value, long num_axisx, 
				     long axis0, long axis1, long axis2, ... );
    virtual fits_image &va_assign_float_v( float value, long num_axisx, 
			      long axis0, long axis1, long axis2, va_list ap );
    virtual fits_image &assign_longlong_v( long long value, long num_axisx, 
				     long axis0, long axis1, long axis2, ... );
    virtual fits_image &va_assign_longlong_v( long long value, long num_axisx, 
			      long axis0, long axis1, long axis2, va_list ap );
    virtual fits_image &assign_long_v( long value, long num_axisx, 
				     long axis0, long axis1, long axis2, ... );
    virtual fits_image &va_assign_long_v( long value, long num_axisx, 
			      long axis0, long axis1, long axis2, va_list ap );
    virtual fits_image &assign_short_v( short value, long num_axisx, 
				     long axis0, long axis1, long axis2, ... );
    virtual fits_image &va_assign_short_v( short value, long num_axisx, 
			      long axis0, long axis1, long axis2, va_list ap );
    virtual fits_image &assign_byte_v( unsigned char value, long num_axisx, 
				     long axis0, long axis1, long axis2, ... );
    virtual fits_image &va_assign_byte_v( unsigned char value, long num_axisx, 
			      long axis0, long axis1, long axis2, va_list ap );


    /* variable args version; not recommended. */
    virtual ssize_t get_data_v( void *dest_buf, size_t buf_size,
			      long num_axisx, 
			      long axis0, long axis1, long axis2, ... ) const;
    virtual ssize_t va_get_data_v( void *dest_buf, size_t buf_size,
			long num_axisx, 
			long axis0, long axis1, long axis2, va_list ap ) const;
    virtual ssize_t put_data_v( const void *src_buf, size_t buf_size,
				long num_axisx, 
				long axis0, long axis1, long axis2, ... );
    virtual ssize_t va_put_data_v( const void *src_buf, size_t buf_size,
			      long num_axisx, 
			      long axis0, long axis1, long axis2, va_list ap );


    /* variable args version; not recommended. */
    virtual void *data_ptr_v( long num_axisx, 
			      long axis0, long axis1, long axis2, ... );
    virtual void *va_data_ptr_v( long num_axisx, 
			      long axis0, long axis1, long axis2, va_list ap );
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const void *data_ptr_v( long num_axisx, 
			       long axis0, long axis1, long axis2, ... ) const;
    virtual const void *va_data_ptr_v( long num_axisx, 
			long axis0, long axis1, long axis2, va_list ap ) const;
#endif
    virtual const void *data_ptr_v_cs( long num_axisx, 
			       long axis0, long axis1, long axis2, ... ) const;
    virtual const void *va_data_ptr_v_cs( long num_axisx, 
			long axis0, long axis1, long axis2, va_list ap ) const;


    /* copy a region to another fits_image object and clear */
    virtual fits_image &cut( fits_image *dest_img, 
			   long col_index, long col_size = FITS::ALL,
			   long row_index = 0, long row_size = FITS::ALL,
			   long layer_index = 0, long layer_size = FITS::ALL );
    /* not recommended */
    virtual fits_image &cut( fits_image &dest_img, 
			   long col_index, long col_size = FITS::ALL,
			   long row_index = 0, long row_size = FITS::ALL,
			   long layer_index = 0, long layer_size = FITS::ALL );


    void image_copy( fits_image *dest_img,
		     long col_index, long col_size, 
		     long row_index, long row_size,
		     long layer_index, long layer_size ) const;



inline long fits_image::axis_size() const
{
    return this->cached_dim_length_rec;	/* == this->data_rec.dim_length() */
}

inline long fits_image::size() const
{
    return this->cached_length_rec;	/* == this->data_rec.length(); */
}

inline long fits_image::size( long axis ) const
{
    return this->data_rec.length(axis);
}

inline long fits_image::col_size() const
{
    return this->cached_col_length_rec;	/* == this->data_rec.col_length(); */
}

inline long fits_image::row_size() const
{
    return this->cached_row_length_rec;	/* == this->data_rec.row_length(); */
}

inline long fits_image::layer_size() const
{
    return this->cached_layer_length_rec;	/* == ...rec.layer_length(); */
}

