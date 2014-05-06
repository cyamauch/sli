
/* r */

/* high-level access */

double fits_image::dvalue_v( long num_axisx, 
			     long axis0, long axis1, long axis2, ... ) const
{
    double ret;
    va_list ap;
    va_start(ap,axis2);
    try {
	ret = this->va_dvalue_v(num_axisx,axis0,axis1,axis2,ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","unexpected exception");
    }
    va_end(ap);
    return ret;
}

double fits_image::va_dvalue_v( long num_axisx, 
				long axis0, long axis1, long axis2, 
				va_list ap ) const
{
    axis2 = this->get_degenerated_zindex(num_axisx,axis2,ap);
    return this->dvalue(axis0,axis1,axis2);
}

long fits_image::lvalue_v( long num_axisx, 
			   long axis0, long axis1, long axis2, ... ) const
{
    long long ret;
    va_list ap;
    va_start(ap,axis2);
    try {
	ret = this->va_lvalue_v(num_axisx,axis0,axis1,axis2,ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","unexpected exception");
    }
    va_end(ap);
    return ret;
}

long fits_image::va_lvalue_v( long num_axisx, 
			      long axis0, long axis1, long axis2, 
			      va_list ap ) const
{
    double dv = this->va_dvalue_v(num_axisx,axis0,axis1,axis2,ap);
    if ( isfinite(dv) && MIN_DOUBLE_ROUND_LONG <= dv && dv <= MAX_DOUBLE_ROUND_LONG )
	return round_d2l(dv);
    else return INDEF_LONG;
}

long long fits_image::llvalue_v( long num_axisx, 
				long axis0, long axis1, long axis2, ... ) const
{
    long long ret;
    va_list ap;
    va_start(ap,axis2);
    try {
	ret = this->va_llvalue_v(num_axisx,axis0,axis1,axis2,ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","unexpected exception");
    }
    va_end(ap);
    return ret;
}

long long fits_image::va_llvalue_v( long num_axisx, 
				    long axis0, long axis1, long axis2, 
				    va_list ap ) const
{
    axis2 = this->get_degenerated_zindex(num_axisx,axis2,ap);
    if ( this->type_rec == FITS__LONGLONG_T ) {
	const void *d_ptr;
	d_ptr = this->data_ptr_cs(axis0,axis1,axis2);
	if ( d_ptr == NULL ) return INDEF_LLONG;
	fits__longlong_t v0 = *((const fits__longlong_t *)d_ptr);
	if ( this->blank_is_set_rec == true && this->blank_r_rec == v0 ) {
	    return INDEF_LLONG;
	}
	else if ( this->bscale_double_rec == 1.0 && this->bzero_double_rec == 0.0 ) {
	    return v0;
	}
	else {
	    double dv = this->bzero_double_rec + v0 * this->bscale_double_rec;
	    if ( isfinite(dv) && MIN_DOUBLE_ROUND_LLONG <= dv && dv <= MAX_DOUBLE_ROUND_LLONG )
		return round_d2ll(dv);
	    else return INDEF_LLONG;
	}
    }
    else {
	double dv = this->dvalue(axis0,axis1,axis2);
	if ( isfinite(dv) && MIN_DOUBLE_ROUND_LLONG <= dv && dv <= MAX_DOUBLE_ROUND_LLONG )
	    return round_d2ll(dv);
	else return INDEF_LLONG;
    }
}

/* low-level access */

double fits_image::double_value_v( long num_axisx, 
				long axis0, long axis1, long axis2, ... ) const
{
    double ret;
    va_list ap;
    va_start(ap,axis2);
    try {
	ret = this->va_double_value_v(num_axisx,axis0,axis1,axis2,ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","unexpected exception");
    }
    va_end(ap);
    return ret;
}

double fits_image::va_double_value_v( long num_axisx, 
				    long axis0, long axis1, long axis2, 
				    va_list ap ) const
{
    axis2 = this->get_degenerated_zindex(num_axisx,axis2,ap);
    return this->double_value(axis0,axis1,axis2);
}

float fits_image::float_value_v( long num_axisx, 
				long axis0, long axis1, long axis2, ... ) const
{
    float ret;
    va_list ap;
    va_start(ap,axis2);
    try {
	ret = this->va_float_value_v(num_axisx,axis0,axis1,axis2,ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","unexpected exception");
    }
    va_end(ap);
    return ret;
}

float fits_image::va_float_value_v( long num_axisx, 
				    long axis0, long axis1, long axis2, 
				    va_list ap ) const
{
    axis2 = this->get_degenerated_zindex(num_axisx,axis2,ap);
    return this->float_value(axis0,axis1,axis2);
}

long long fits_image::longlong_value_v( long num_axisx, 
				long axis0, long axis1, long axis2, ... ) const
{
    long long ret;
    va_list ap;
    va_start(ap,axis2);
    try {
	ret = this->va_longlong_value_v(num_axisx,axis0,axis1,axis2,ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","unexpected exception");
    }
    va_end(ap);
    return ret;
}

long long fits_image::va_longlong_value_v( long num_axisx, 
					   long axis0, long axis1, long axis2, 
					   va_list ap ) const
{
    axis2 = this->get_degenerated_zindex(num_axisx,axis2,ap);
    return this->longlong_value(axis0,axis1,axis2);
}

long fits_image::long_value_v( long num_axisx, 
			       long axis0, long axis1, long axis2, ... ) const
{
    long ret;
    va_list ap;
    va_start(ap,axis2);
    try {
	ret = this->va_long_value_v(num_axisx,axis0,axis1,axis2,ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","unexpected exception");
    }
    va_end(ap);
    return ret;
}

long fits_image::va_long_value_v( long num_axisx, 
				  long axis0, long axis1, long axis2, 
				  va_list ap ) const
{
    axis2 = this->get_degenerated_zindex(num_axisx,axis2,ap);
    return this->long_value(axis0,axis1,axis2);
}

short fits_image::short_value_v( long num_axisx, 
				long axis0, long axis1, long axis2, ... ) const
{
    short ret;
    va_list ap;
    va_start(ap,axis2);
    try {
	ret = this->va_short_value_v(num_axisx,axis0,axis1,axis2,ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","unexpected exception");
    }
    va_end(ap);
    return ret;
}

short fits_image::va_short_value_v( long num_axisx, 
				    long axis0, long axis1, long axis2, 
				    va_list ap ) const
{
    axis2 = this->get_degenerated_zindex(num_axisx,axis2,ap);
    return this->short_value(axis0,axis1,axis2);
}

unsigned char fits_image::byte_value_v( long num_axisx, 
				long axis0, long axis1, long axis2, ... ) const
{
    unsigned char ret;
    va_list ap;
    va_start(ap,axis2);
    try {
	ret = this->va_byte_value_v(num_axisx,axis0,axis1,axis2,ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","unexpected exception");
    }
    va_end(ap);
    return ret;
}

unsigned char fits_image::va_byte_value_v( long num_axisx, 
					   long axis0, long axis1, long axis2, 
					   va_list ap ) const
{
    axis2 = this->get_degenerated_zindex(num_axisx,axis2,ap);
    return this->byte_value(axis0,axis1,axis2);
}


/* w */

fits_image &fits_image::assign_v( double value, long num_axisx, 
				  long axis0, long axis1, long axis2, ... )
{
    va_list ap;
    va_start(ap,axis2);
    try {
	this->va_assign_v(value,num_axisx,axis0,axis1,axis2,ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","this->va_assign_v() failed");
    }
    va_end(ap);
    return *this;
}

fits_image &fits_image::va_assign_v( double value, long num_axisx, 
				     long axis0, long axis1, long axis2, 
				     va_list ap )
{
    axis2 = this->get_degenerated_zindex(num_axisx,axis2,ap);
    return this->assign(value,axis0,axis1,axis2);
}

fits_image &fits_image::assign_double_v( double value, long num_axisx, 
				       long axis0, long axis1, long axis2, ... )
{
    va_list ap;
    va_start(ap,axis2);
    try {
	this->va_assign_double_v(value,num_axisx,axis0,axis1,axis2,ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","this->va_assign_double_v() failed");
    }
    va_end(ap);
    return *this;
}

fits_image &fits_image::va_assign_double_v( double value, long num_axisx, 
					    long axis0, long axis1, long axis2,
					    va_list ap )
{
    axis2 = this->get_degenerated_zindex(num_axisx,axis2,ap);
    return this->assign_double(value, axis0,axis1,axis2);
}

fits_image &fits_image::assign_float_v( float value, long num_axisx, 
				      long axis0, long axis1, long axis2, ... )
{
    va_list ap;
    va_start(ap,axis2);
    try {
	this->va_assign_float_v(value,num_axisx,axis0,axis1,axis2,ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","this->va_assign_float_v() failed");
    }
    va_end(ap);
    return *this;
}

fits_image &fits_image::va_assign_float_v( float value, long num_axisx, 
					   long axis0, long axis1, long axis2, 
					   va_list ap )
{
    axis2 = this->get_degenerated_zindex(num_axisx,axis2,ap);
    return this->assign_float(value, axis0,axis1,axis2);
}

fits_image &fits_image::assign_longlong_v( long long value, long num_axisx, 
				      long axis0, long axis1, long axis2, ... )
{
    va_list ap;
    va_start(ap,axis2);
    try {
	this->va_assign_longlong_v(value,num_axisx,axis0,axis1,axis2,ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","this->va_assign_longlong_v() failed");
    }
    va_end(ap);
    return *this;
}

fits_image &fits_image::va_assign_longlong_v( long long value, long num_axisx, 
					    long axis0, long axis1, long axis2,
					    va_list ap )
{
    axis2 = this->get_degenerated_zindex(num_axisx,axis2,ap);
    return this->assign_longlong(value, axis0,axis1,axis2);
}

fits_image &fits_image::assign_long_v( long value, long num_axisx, 
				      long axis0, long axis1, long axis2, ... )
{
    va_list ap;
    va_start(ap,axis2);
    try {
	this->va_assign_long_v(value,num_axisx,axis0,axis1,axis2,ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","this->va_assign_long_v() failed");
    }
    va_end(ap);
    return *this;
}

fits_image &fits_image::va_assign_long_v( long value, long num_axisx, 
					  long axis0, long axis1, long axis2, 
					  va_list ap )
{
    axis2 = this->get_degenerated_zindex(num_axisx,axis2,ap);
    return this->assign_long(value, axis0,axis1,axis2);
}

fits_image &fits_image::assign_short_v( short value, long num_axisx, 
				      long axis0, long axis1, long axis2, ... )
{
    va_list ap;
    va_start(ap,axis2);
    try {
	this->va_assign_short_v(value,num_axisx,axis0,axis1,axis2,ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","this->va_assign_short_v() failed");
    }
    va_end(ap);
    return *this;
}

fits_image &fits_image::va_assign_short_v( short value, long num_axisx, 
					   long axis0, long axis1, long axis2,
					   va_list ap )
{
    axis2 = this->get_degenerated_zindex(num_axisx,axis2,ap);
    return this->assign_short(value, axis0,axis1,axis2);
}

fits_image &fits_image::assign_byte_v( unsigned char value, long num_axisx, 
				      long axis0, long axis1, long axis2, ... )
{
    va_list ap;
    va_start(ap,axis2);
    try {
	this->va_assign_byte_v(value,num_axisx,axis0,axis1,axis2,ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","this->va_assign_byte_v() failed");
    }
    va_end(ap);
    return *this;
}

fits_image &fits_image::va_assign_byte_v( unsigned char value, long num_axisx, 
					  long axis0, long axis1, long axis2, 
					  va_list ap )
{
    axis2 = this->get_degenerated_zindex(num_axisx,axis2,ap);
    return this->assign_byte(value, axis0,axis1,axis2);
}


ssize_t fits_image::get_data_v( void *dest_buf, size_t buf_size,
				long num_axisx, 
				long axis0, long axis1, long axis2, ... ) const
{
    ssize_t ret;
    va_list ap;
    va_start(ap,axis2);
    try {
	ret=this->va_get_data_v(dest_buf,buf_size,num_axisx,axis0,axis1,axis2,ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","unexpected exception");
    }
    va_end(ap);
    return ret;
}

ssize_t fits_image::va_get_data_v( void *dest_buf, size_t buf_size,
			 long num_axisx, 
			 long axis0, long axis1, long axis2, va_list ap ) const
{
    axis2 = this->get_degenerated_zindex(num_axisx,axis2,ap);
    return this->data_rec.getdata(dest_buf, buf_size, axis0,axis1,axis2);
}

ssize_t fits_image::put_data_v( const void *src_buf, size_t buf_size,
				long num_axisx, 
				long axis0, long axis1, long axis2, ... )
{
    ssize_t ret;
    va_list ap;
    va_start(ap,axis2);
    try {
	ret=this->va_put_data_v(src_buf,buf_size,num_axisx,axis0,axis1,axis2,ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","unexpected exception");
    }
    va_end(ap);
    return ret;
}

ssize_t fits_image::va_put_data_v( const void *src_buf, size_t buf_size,
			       long num_axisx, 
			       long axis0, long axis1, long axis2, va_list ap )
{
    axis2 = this->get_degenerated_zindex(num_axisx,axis2,ap);
    return this->put_data(src_buf,buf_size, axis0,axis1,axis2);
}


void *fits_image::data_ptr_v( long num_axisx, 
			      long axis0, long axis1, long axis2, ... )
{
    void *ret;
    va_list ap;
    va_start(ap,axis2);
    try {
	ret = this->va_data_ptr_v(num_axisx,axis0,axis1,axis2,ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","unexpected exception");
    }
    va_end(ap);
    return ret;
}

void *fits_image::va_data_ptr_v( long num_axisx, 
			       long axis0, long axis1, long axis2, va_list ap )
{
    axis2 = this->get_degenerated_zindex(num_axisx,axis2,ap);
    return this->data_ptr(axis0,axis1,axis2);
}

#ifdef SLI__OVERLOAD_CONST_AT
const void *fits_image::data_ptr_v( long num_axisx, 
				long axis0, long axis1, long axis2, ... ) const
{
    const void *ret;
    va_list ap;
    va_start(ap,axis2);
    try {
	ret = this->va_data_ptr_v_cs(num_axisx,axis0,axis1,axis2,ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","unexpected exception");
    }
    va_end(ap);
    return ret;
}

const void *fits_image::va_data_ptr_v( long num_axisx, 
			 long axis0, long axis1, long axis2, va_list ap ) const
{
    axis2 = this->get_degenerated_zindex(num_axisx,axis2,ap);
    return this->data_ptr_cs(axis0,axis1,axis2);
}
#endif

const void *fits_image::data_ptr_v_cs( long num_axisx, 
				long axis0, long axis1, long axis2, ... ) const
{
    const void *ret;
    va_list ap;
    va_start(ap,axis2);
    try {
	ret = this->va_data_ptr_v_cs(num_axisx,axis0,axis1,axis2,ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","unexpected exception");
    }
    va_end(ap);
    return ret;
}

const void *fits_image::va_data_ptr_v_cs( long num_axisx, 
			 long axis0, long axis1, long axis2, va_list ap ) const
{
    axis2 = this->get_degenerated_zindex(num_axisx,axis2,ap);
    return this->data_ptr_cs(axis0,axis1,axis2);
}



fits_image &fits_image::cut( fits_image *dest_img,
			     long col_index, long col_size, 
			     long row_index, long row_size,
			     long layer_idx, long layer_size )
{
    this->image_copy_and_cut( dest_img, col_index, col_size,
			      row_index, row_size, layer_idx, layer_size);
    return *this;
}

fits_image &fits_image::cut( fits_image &dest_img,
			     long col_index, long col_size, 
			     long row_index, long row_size,
			     long layer_idx, long layer_size )
{
    this->image_copy_and_cut( &dest_img, col_index, col_size,
			      row_index, row_size, layer_idx, layer_size );
    return *this;
}


void fits_image::image_copy( fits_image *dest_img,
			     long col_index, long col_size, 
			     long row_index, long row_size,
			     long layer_index, long layer_size ) const
{
    size_t z_col_size = fits_seclen_to_mdarray_seclen(col_size);
    size_t z_row_size = fits_seclen_to_mdarray_seclen(row_size);
    size_t z_layer_size = fits_seclen_to_mdarray_seclen(layer_size);

    if ( dest_img == NULL ) goto quit;

    /* dest_img が自身の場合でも行なわれる */
    dest_img->type_rec = this->type_rec;
    this->data_rec.copy(&(dest_img->data_rec), col_index, z_col_size, 
			row_index, z_row_size, layer_index, z_layer_size);

    if ( dest_img != this ) {
	/* bzero, bscale などの属性をコピー */
	dest_img->init_properties(*this);
	/* ヘッダをコピー */
	dest_img->header_init( this->header() );
    }

    /* これはここでやるべきではないか… */
#if 0
    long i, j;
    /* CRPIX1x, CRPIX2x, CRPIX3x を調整する */
    this->fix_indices( &col_index, &row_index, &layer_index );
    for ( j = 0 ; j < 2 ; j++ ) {
	const char *pat;
	pat = (j==0) ? "^CRPIX1$" : "^CRPIX1[A-Z]$";
	for ( i=0 ; 0 <= (i = dest_img->header_regmatch(i,pat)) ; i++ ) {
	    double v = dest_img->header(i).dvalue();
	    dest_img->header(i).assign(v - col_index);
	}
	pat = (j==0) ? "^CRPIX2$" : "^CRPIX2[A-Z]$";
	for ( i=0 ; 0 <= (i = dest_img->header_regmatch(i,pat)) ; i++ ) {
	    double v = dest_img->header(i).dvalue();
	    dest_img->header(i).assign(v - row_index);
	}
	pat = (j==0) ? "^CRPIX3$" : "^CRPIX3[A-Z]$";
	for ( i=0 ; 0 <= (i = dest_img->header_regmatch(i,pat)) ; i++ ) {
	    double v = dest_img->header(i).dvalue();
	    dest_img->header(i).assign(v - layer_index);
	}
    }
#endif

 quit:
    return;
}


void fits_image::image_copy_and_cut( fits_image *dest_img,
				     long col_index, long col_size, 
				     long row_index, long row_size,
				     long layer_index, long layer_size )
{
    size_t z_col_size = fits_seclen_to_mdarray_seclen(col_size);
    size_t z_row_size = fits_seclen_to_mdarray_seclen(row_size);
    size_t z_layer_size = fits_seclen_to_mdarray_seclen(layer_size);

    if ( dest_img == NULL ) goto quit;

    if ( dest_img != this ) {
	if ( this->bzero_is_set() != false ) {
	    dest_img->assign_bzero(this->bzero());
	}
	if ( this->bscale_is_set() != false ) {
	    dest_img->assign_bscale(this->bscale());
	}
	if ( this->blank_is_set() != false ) {
	    dest_img->assign_blank(this->blank());
	}
	if ( this->bunit_is_set() != false ) {
	    dest_img->assign_bunit(this->bunit());
	}
    }

    dest_img->type_rec = this->type_rec;
    this->data_rec.cut(&(dest_img->data_rec), col_index, z_col_size, 
		       row_index, z_row_size, layer_index, z_layer_size);

    if ( dest_img != this ) {
	/* ヘッダをコピーする */
	//for ( i=0 ; i < this->header_length() ; i++ ) {
	//    const fits::header_def &def = this->header_cs(i).raw_record();
	//    dest_img->header_append(def);
	//}
	//err_report(__FUNCTION__,"DEBUG","######## doing header_init...");
	dest_img->header_init( this->header() );
    }

    /* これはここでやるべきではないか… */
#if 0
    long i, j;
    /* CRPIX1x, CRPIX2x, CRPIX3x を調整する */
    this->fix_indices( &col_index, &row_index, &layer_index );
    for ( j = 0 ; j < 2 ; j++ ) {
	const char *pat;
	pat = (j==0) ? "^CRPIX1$" : "^CRPIX1[A-Z]$";
	for ( i=0 ; 0 <= (i = dest_img->header_regmatch(i,pat)) ; i++ ) {
	    double v = dest_img->header(i).dvalue();
	    dest_img->header(i).assign(v - col_index);
	}
	pat = (j==0) ? "^CRPIX2$" : "^CRPIX2[A-Z]$";
	for ( i=0 ; 0 <= (i = dest_img->header_regmatch(i,pat)) ; i++ ) {
	    double v = dest_img->header(i).dvalue();
	    dest_img->header(i).assign(v - row_index);
	}
	pat = (j==0) ? "^CRPIX3$" : "^CRPIX3[A-Z]$";
	for ( i=0 ; 0 <= (i = dest_img->header_regmatch(i,pat)) ; i++ ) {
	    double v = dest_img->header(i).dvalue();
	    dest_img->header(i).assign(v - layer_index);
	}
    }
#endif

 quit:
    return;
}
