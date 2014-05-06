

/*****************************************************************************
 *
 * BEGIN : scan_along_x(), scan_along_y(), scan_along_z()
 *
 *****************************************************************************/

struct scan_prms {
    /* アライメントに注意(sizeofが長いものから順に!) */
    double bzero;
    double bscale;
    long long blank_r;
    long (*usr_func_1d)(double [],long, long,long,long,const fits_image *,void *);
    long (*usr_func_2d)(double [],long,long, long,long,long,const fits_image *,void *);
    long (*usr_func_3d)(double [],long,long,long, long,long,long,const fits_image *,void *);
    const fits_image *this_ptr;
    void *usr_ptr;
    bool blank_is_set;
};

namespace scan_x2d_n
{
#define MAKE_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
static void fncname(const void *org_val_ptr, void *new_val_ptr, size_t n, void *u_ptr) \
{ \
    const struct scan_prms *p = (const struct scan_prms *)u_ptr; \
    const org_type *org_t_ptr = (const org_type *)org_val_ptr; \
    new_type *new_t_ptr = (new_type *)new_val_ptr; \
    double val; \
    size_t i; \
    if ( p->bzero == 0.0 && p->bscale == 1.0 ) { \
        if ( p->blank_is_set == true ) { \
            for ( i=0 ; i < n ; i++ ) { \
                if ( p->blank_r == org_t_ptr[i] ) val = NAN; \
                else val = org_t_ptr[i]; \
                new_t_ptr[i] = (new_type)val; \
            } \
        } \
        else { \
            for ( i=0 ; i < n ; i++ ) { \
	        val = org_t_ptr[i]; \
                new_t_ptr[i] = (new_type)val; \
            } \
        } \
    } \
    else { \
        const double p_bscale = p->bscale; \
        const double p_bzero = p->bzero; \
        if ( p->blank_is_set == true ) { \
            for ( i=0 ; i < n ; i++ ) { \
                if ( p->blank_r == org_t_ptr[i] ) val = NAN; \
                else { \
	            val = org_t_ptr[i]; \
	            val *= p_bscale; \
	            val += p_bzero; \
                } \
                new_t_ptr[i] = (new_type)val; \
            } \
        } \
        else { \
            for ( i=0 ; i < n ; i++ ) { \
	        val = org_t_ptr[i]; \
	        val *= p_bscale; \
	        val += p_bzero; \
                new_t_ptr[i] = (new_type)val; \
            } \
        } \
    } \
}
SLI__MDARRAY__DO_OPERATION_2TYPES(MAKE_FUNC,round_f2i32,round_f2i32,round_f2i32,round_f2i64,round_d2i32,round_d2i32,round_d2i32,round_d2i64,,,,,);
#undef MAKE_FUNC
}

namespace scan_1d_x
{
static ssize_t u_func(double vals[], size_t n, ssize_t i, ssize_t j, ssize_t k,
		     const mdarray *x ,void *u_ptr )
{
    struct scan_prms *p = (struct scan_prms *)u_ptr;
    return p->usr_func_1d(vals,n, i,j,k, p->this_ptr, p->usr_ptr);
}
}

/* horizontally scans the specified section using a user-defined function. */
/* a temporary buffer for 1-D array is prepared.                           */
long fits_image::scan_along_x( 
	long (*func)(double [],long, long,long,long,const fits_image *,void *),
		 void *user_ptr,
		 long col_index, long col_size, 
		 long row_index, long row_size,
		 long layer_index, long layer_size ) const
{
    long ret_len = -1;
    struct scan_prms prms;
    void (*func_src2d)(const void *,void *,size_t,void *);
    size_t z_col_size = fits_seclen_to_mdarray_seclen(col_size);
    size_t z_row_size = fits_seclen_to_mdarray_seclen(row_size);
    size_t z_layer_size = fits_seclen_to_mdarray_seclen(layer_size);

    prms.blank_is_set = false;
    prms.blank_r = 0;
    prms.bzero = this->bzero();
    prms.bscale = this->bscale();

    if ( this->type_rec != FITS__DOUBLE_T &&
	 this->type_rec != FITS__FLOAT_T ) {
	prms.blank_r = this->blank();
	prms.blank_is_set = this->blank_is_set();
    }

    func_src2d = NULL;

#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
    if ( this->data_rec.size_type() == org_sz_type && DOUBLE_ZT == new_sz_type ) { \
        func_src2d = &scan_x2d_n::fncname; \
    }
    SLI__MDARRAY__DO_OPERATION_2TYPES(SEL_FUNC,,,,,,,,,,,,,else);
#undef SEL_FUNC

    if ( func_src2d == NULL ) {
	err_throw(__FUNCTION__,"FATAL","detected unexpected NULL pointer");
    }

    prms.usr_func_1d = func;
    prms.this_ptr = this;
    prms.usr_ptr  = user_ptr;

    /* call mdarray::scan_along_x */
    ret_len = this->data_rec.scan_along_x(
				 func_src2d, (void *)&prms,
				 &scan_1d_x::u_func, (void *)&prms,
				 col_index, z_col_size, row_index, z_row_size,
				 layer_index, z_layer_size );

    return ret_len;
}


/*****************************************************************************
 *
 * BEGIN : scan_along_y(), scan_along_z(), combine_layers()
 *
 *****************************************************************************/

namespace scan_x2d_n_sstep
{
#define MAKE_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
static void fncname(const void *org_val_ptr, void *new_val_ptr, size_t n, size_t src_step, void *u_ptr) \
{ \
    const struct scan_prms *p = (const struct scan_prms *)u_ptr; \
    const org_type *org_t_ptr = (const org_type *)org_val_ptr; \
    new_type *new_t_ptr = (new_type *)new_val_ptr; \
    double val; \
    size_t i,j; \
    if ( p->bzero == 0.0 && p->bscale == 1.0 ) { \
        if ( p->blank_is_set == true ) { \
            j = 0; \
            for ( i=0 ; i < n ; i++ ) { \
                if ( p->blank_r == org_t_ptr[j] ) val = NAN; \
                else val = org_t_ptr[j]; \
                new_t_ptr[i] = (new_type)val; \
                j += src_step; \
            } \
        } \
        else { \
            j = 0; \
            for ( i=0 ; i < n ; i++ ) { \
	        val = org_t_ptr[j]; \
                new_t_ptr[i] = (new_type)val; \
                j += src_step; \
            } \
        } \
    } \
    else { \
        const double p_bscale = p->bscale; \
        const double p_bzero = p->bzero; \
        if ( p->blank_is_set == true ) { \
            j = 0; \
            for ( i=0 ; i < n ; i++ ) { \
                if ( p->blank_r == org_t_ptr[j] ) val = NAN; \
                else { \
	            val = org_t_ptr[j]; \
	            val *= p_bscale; \
	            val += p_bzero; \
                } \
                new_t_ptr[i] = (new_type)val; \
                j += src_step; \
            } \
        } \
        else { \
            j = 0; \
            for ( i=0 ; i < n ; i++ ) { \
	        val = org_t_ptr[j]; \
	        val *= p_bscale; \
	        val += p_bzero; \
                new_t_ptr[i] = (new_type)val; \
                j += src_step; \
            } \
        } \
    } \
}
SLI__MDARRAY__DO_OPERATION_2TYPES(MAKE_FUNC,round_f2i32,round_f2i32,round_f2i32,round_f2i64,round_d2i32,round_d2i32,round_d2i32,round_d2i64,,,,,);
#undef MAKE_FUNC
}


/* vertically scans the specified section using a user-defined function. */
/* a temporary buffer for 1-D array is prepared.                         */
long fits_image::scan_along_y( 
	long (*func)(double [],long, long,long,long,const fits_image *,void *),
		 void *user_ptr,
		 long col_index, long col_size, 
		 long row_index, long row_size,
		 long layer_index, long layer_size ) const
{
    long ret_len = -1;
    struct scan_prms prms;
    void (*func_src2d)(const void *,void *,size_t,size_t,void *);
    size_t z_col_size = fits_seclen_to_mdarray_seclen(col_size);
    size_t z_row_size = fits_seclen_to_mdarray_seclen(row_size);
    size_t z_layer_size = fits_seclen_to_mdarray_seclen(layer_size);

    prms.blank_is_set = false;
    prms.blank_r = 0;
    prms.bzero = this->bzero();
    prms.bscale = this->bscale();

    if ( this->type_rec != FITS__DOUBLE_T &&
	 this->type_rec != FITS__FLOAT_T ) {
	prms.blank_r = this->blank();
	prms.blank_is_set = this->blank_is_set();
    }

    func_src2d = NULL;

#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
    if ( this->data_rec.size_type() == org_sz_type && DOUBLE_ZT == new_sz_type ) { \
        func_src2d = &scan_x2d_n_sstep::fncname; \
    }
    SLI__MDARRAY__DO_OPERATION_2TYPES(SEL_FUNC,,,,,,,,,,,,,else);
#undef SEL_FUNC

    if ( func_src2d == NULL ) {
	err_throw(__FUNCTION__,"FATAL","detected unexpected NULL pointer");
    }

    prms.usr_func_1d = func;
    prms.this_ptr = this;
    prms.usr_ptr  = user_ptr;

    /* call mdarray::scan_along_y */
    ret_len = this->data_rec.scan_along_y(
				 func_src2d, (void *)&prms,
				 &scan_1d_x::u_func, (void *)&prms,
				 col_index, z_col_size, row_index, z_row_size,
				 layer_index, z_layer_size );

    return ret_len;
}

/* scan the specified section along z-axis using a user-defined function. */
/* a temporary buffer for 1-D array is prepared.                          */
long fits_image::scan_along_z( 
	long (*func)(double [],long, long,long,long,const fits_image *,void *),
		 void *user_ptr,
		 long col_index, long col_size, 
		 long row_index, long row_size,
		 long layer_index, long layer_size ) const
{
    long ret_len = -1;
    struct scan_prms prms;
    void (*func_src2d)(const void *,void *,size_t,size_t,void *);
    size_t z_col_size = fits_seclen_to_mdarray_seclen(col_size);
    size_t z_row_size = fits_seclen_to_mdarray_seclen(row_size);
    size_t z_layer_size = fits_seclen_to_mdarray_seclen(layer_size);

    prms.blank_is_set = false;
    prms.blank_r = 0;
    prms.bzero = this->bzero();
    prms.bscale = this->bscale();

    if ( this->type_rec != FITS__DOUBLE_T &&
	 this->type_rec != FITS__FLOAT_T ) {
	prms.blank_r = this->blank();
	prms.blank_is_set = this->blank_is_set();
    }

    func_src2d = NULL;

#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
    if ( this->data_rec.size_type() == org_sz_type && DOUBLE_ZT == new_sz_type ) { \
        func_src2d = &scan_x2d_n_sstep::fncname; \
    }
    SLI__MDARRAY__DO_OPERATION_2TYPES(SEL_FUNC,,,,,,,,,,,,,else);
#undef SEL_FUNC

    if ( func_src2d == NULL ) {
	err_throw(__FUNCTION__,"FATAL","detected unexpected NULL pointer");
    }

    prms.usr_func_1d = func;
    prms.this_ptr = this;
    prms.usr_ptr  = user_ptr;

    /* call mdarray::scan_along_z */
    ret_len = this->data_rec.scan_along_z(
				 func_src2d, (void *)&prms,
				 &scan_1d_x::u_func, (void *)&prms,
				 col_index, z_col_size, row_index, z_row_size,
				 layer_index, z_layer_size );

    return ret_len;
}

namespace scan_x2d_n_dstep
{
#define MAKE_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
static void fncname(const void *org_val_ptr, void *new_val_ptr, size_t n, size_t dest_step, void *u_ptr) \
{ \
    const struct scan_prms *p = (const struct scan_prms *)u_ptr; \
    const org_type *org_t_ptr = (const org_type *)org_val_ptr; \
    new_type *new_t_ptr = (new_type *)new_val_ptr; \
    double val; \
    size_t i,j; \
    if ( p->bzero == 0.0 && p->bscale == 1.0 ) { \
        if ( p->blank_is_set == true ) { \
            j = 0; \
            for ( i=0 ; i < n ; i++ ) { \
                if ( p->blank_r == org_t_ptr[i] ) val = NAN; \
                else val = org_t_ptr[i]; \
                new_t_ptr[j] = (new_type)val; \
                j += dest_step; \
            } \
        } \
        else { \
            j = 0; \
            for ( i=0 ; i < n ; i++ ) { \
	        val = org_t_ptr[i]; \
                new_t_ptr[j] = (new_type)val; \
                j += dest_step; \
            } \
        } \
    } \
    else { \
        const double p_bscale = p->bscale; \
        const double p_bzero = p->bzero; \
        if ( p->blank_is_set == true ) { \
            j = 0; \
            for ( i=0 ; i < n ; i++ ) { \
                if ( p->blank_r == org_t_ptr[i] ) val = NAN; \
                else { \
	            val = org_t_ptr[i]; \
	            val *= p_bscale; \
	            val += p_bzero; \
                } \
                new_t_ptr[j] = (new_type)val; \
                j += dest_step; \
            } \
        } \
        else { \
            j = 0; \
            for ( i=0 ; i < n ; i++ ) { \
	        val = org_t_ptr[i]; \
	        val *= p_bscale; \
	        val += p_bzero; \
                new_t_ptr[j] = (new_type)val; \
                j += dest_step; \
            } \
        } \
    } \
}
SLI__MDARRAY__DO_OPERATION_2TYPES(MAKE_FUNC,round_f2i32,round_f2i32,round_f2i32,round_f2i64,round_d2i32,round_d2i32,round_d2i32,round_d2i64,,,,,);
#undef MAKE_FUNC
}

namespace scan_2d_x
{
static ssize_t u_func(double vals[], size_t n0, size_t n1, 
		      ssize_t i, ssize_t j, ssize_t k,
		      const mdarray *x ,void *u_ptr )
{
    struct scan_prms *p = (struct scan_prms *)u_ptr;
    return p->usr_func_2d(vals, n0,n1 ,i,j,k, p->this_ptr, p->usr_ptr);
}
}

/* scan the specified 3-D section with plane by plane (xy plane) using a */
/* user-defined function.  a temporary buffer of 2-D array is prepared.  */
long fits_image::scan_xy_planes( 
	      long (*func)(double [],long,long,long,long,long,const fits_image *,void *),
	      void *user_ptr,
	      long col_index, long col_size, 
	      long row_index, long row_size,
	      long layer_index, long layer_size ) const
{
    long ret_len = -1;
    struct scan_prms prms;
    void (*func_src2d)(const void *,void *,size_t,void *);
    size_t z_col_size = fits_seclen_to_mdarray_seclen(col_size);
    size_t z_row_size = fits_seclen_to_mdarray_seclen(row_size);
    size_t z_layer_size = fits_seclen_to_mdarray_seclen(layer_size);

    prms.blank_is_set = false;
    prms.blank_r = 0;
    prms.bzero = this->bzero();
    prms.bscale = this->bscale();

    if ( this->type_rec != FITS__DOUBLE_T &&
	 this->type_rec != FITS__FLOAT_T ) {
	prms.blank_r = this->blank();
	prms.blank_is_set = this->blank_is_set();
    }

    func_src2d = NULL;

#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
    if ( this->data_rec.size_type() == org_sz_type && DOUBLE_ZT == new_sz_type ) { \
        func_src2d = &scan_x2d_n::fncname; \
    }
    SLI__MDARRAY__DO_OPERATION_2TYPES(SEL_FUNC,,,,,,,,,,,,,else);
#undef SEL_FUNC

    if ( func_src2d == NULL ) {
	err_throw(__FUNCTION__,"FATAL","detected unexpected NULL pointer");
    }

    prms.usr_func_2d = func;
    prms.this_ptr = this;
    prms.usr_ptr  = user_ptr;

    /* call mdarray::scan_xy_planes */
    ret_len = this->data_rec.scan_xy_planes(
				 func_src2d, (void *)&prms,
				 &scan_2d_x::u_func, (void *)&prms,
				 col_index, z_col_size, row_index, z_row_size,
				 layer_index, z_layer_size );

    return ret_len;
}

/* scan the specified 3-D section with plane by plane (zx plane) using a */
/* user-defined function.  a temporary buffer for 2-D array is prepared. */
long fits_image::scan_zx_planes( 
	      long (*func)(double [],long,long,long,long,long,const fits_image *,void *),
	      void *user_ptr,
	      long col_index, long col_size, 
	      long row_index, long row_size,
	      long layer_index, long layer_size ) const
{
    long ret_len = -1;
    struct scan_prms prms;
    void (*func_src2d)(const void *,void *,size_t,size_t,void *);
    size_t z_col_size = fits_seclen_to_mdarray_seclen(col_size);
    size_t z_row_size = fits_seclen_to_mdarray_seclen(row_size);
    size_t z_layer_size = fits_seclen_to_mdarray_seclen(layer_size);

    prms.blank_is_set = false;
    prms.blank_r = 0;
    prms.bzero = this->bzero();
    prms.bscale = this->bscale();

    if ( this->type_rec != FITS__DOUBLE_T &&
	 this->type_rec != FITS__FLOAT_T ) {
	prms.blank_r = this->blank();
	prms.blank_is_set = this->blank_is_set();
    }

    func_src2d = NULL;

#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
    if ( this->data_rec.size_type() == org_sz_type && DOUBLE_ZT == new_sz_type ) { \
        func_src2d = &scan_x2d_n_dstep::fncname; \
    }
    SLI__MDARRAY__DO_OPERATION_2TYPES(SEL_FUNC,,,,,,,,,,,,,else);
#undef SEL_FUNC

    if ( func_src2d == NULL ) {
	err_throw(__FUNCTION__,"FATAL","detected unexpected NULL pointer");
    }

    prms.usr_func_2d = func;
    prms.this_ptr = this;
    prms.usr_ptr  = user_ptr;

    /* call mdarray::scan_zx_planes */
    ret_len = this->data_rec.scan_zx_planes(
				 func_src2d, (void *)&prms,
				 &scan_2d_x::u_func, (void *)&prms,
				 col_index, z_col_size, row_index, z_row_size,
				 layer_index, z_layer_size );

    return ret_len;
}

namespace scan_3d_x
{
static ssize_t u_func(double vals[], size_t n0, size_t n1, size_t n2,
		      ssize_t i, ssize_t j, ssize_t k,
		      const mdarray *x ,void *u_ptr )
{
    struct scan_prms *p = (struct scan_prms *)u_ptr;
    return p->usr_func_3d(vals, n0,n1,n2, i,j,k, p->this_ptr, p->usr_ptr);
}
}

/* scan the specified 3-D section using a user-defined function. */
/* a temporary buffer of 3-D array is prepared.                  */
long fits_image::scan_a_cube( 
	      long (*func)(double [],long,long,long,long,long,long,const fits_image *,void *),
	      void *user_ptr,
	      long col_index, long col_size, 
	      long row_index, long row_size,
	      long layer_index, long layer_size ) const
{
    long ret_len = -1;
    struct scan_prms prms;
    void (*func_src3d)(const void *,void *,size_t,void *);
    size_t z_col_size = fits_seclen_to_mdarray_seclen(col_size);
    size_t z_row_size = fits_seclen_to_mdarray_seclen(row_size);
    size_t z_layer_size = fits_seclen_to_mdarray_seclen(layer_size);

    prms.blank_is_set = false;
    prms.blank_r = 0;
    prms.bzero = this->bzero();
    prms.bscale = this->bscale();

    if ( this->type_rec != FITS__DOUBLE_T &&
	 this->type_rec != FITS__FLOAT_T ) {
	prms.blank_r = this->blank();
	prms.blank_is_set = this->blank_is_set();
    }

    func_src3d = NULL;

#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
    if ( this->data_rec.size_type() == org_sz_type && DOUBLE_ZT == new_sz_type ) { \
        func_src3d = &scan_x2d_n::fncname; \
    }
    SLI__MDARRAY__DO_OPERATION_2TYPES(SEL_FUNC,,,,,,,,,,,,,else);
#undef SEL_FUNC

    if ( func_src3d == NULL ) {
	err_throw(__FUNCTION__,"FATAL","detected unexpected NULL pointer");
    }

    prms.usr_func_3d = func;
    prms.this_ptr = this;
    prms.usr_ptr  = user_ptr;

    /* call mdarray::scan_xy_planes */
    ret_len = this->data_rec.scan_a_cube(
				 func_src3d, (void *)&prms,
				 &scan_3d_x::u_func, (void *)&prms,
				 col_index, z_col_size, row_index, z_row_size,
				 layer_index, z_layer_size );
    return ret_len;
}


/*
 * Parse options string and return associative string array.
 * This functions is used stat_pixels, combine_layers(), etc.
 */
static void parse_option_args( const char *options_str, 
			       asarray_tstring *ret_options )
{
    asarray_tstring &options = (*ret_options);
    tarray_tstring option_arr;
    size_t i;

    /* split options arg with " ", and update all strings to lower case */
    option_arr.split(options_str, " ", false, "\"'", '\\', false).tolower();

    /* check all options */
    for ( i=0 ; i < option_arr.length() ; i++ ) {
	tarray_tstring pvs;

	/* split: [foo="aaa,bbb,ccc"] => [foo] ["aaa,bbb,ccc"] */
	pvs.split(option_arr[i], "=", true, "\"'", '\\', false);
	if ( 2 <= pvs.length() ) {
	    size_t j, l;
	    if ( 2 < pvs.length() ) {
		for ( j=2 ; j < pvs.length() ; j++ ) {
		    pvs[1].append("=").append(pvs[j]);
		}
	    }
	    for ( j=0 ; j < 2 ; j++ ) {
		tstring &pv = pvs[j];
		if ( pv[0] == '"' ) pv.erase_quotes("\"", '\\', true, &l);
		else if ( pv[0] == '\'' ) pv.erase_quotes("'", '\\', true, &l);
	    }
	    options[pvs[0].cstr()] = pvs[1];
	}
    }

    return;
}


/*
 *
 *  STAT_PIXELS begins
 *
 */

/* foo=var */
/* static const tregex Pat_param_eq_values("^([^=]+)([ ]*[=][ ]*)(.+)$"); */

struct stat_pixels_results {
    /* アライメントに注意(sizeofが長いものから順に!) */
    double sum;
    double sum2;
    double sum3;
    double sum4;
    double min;
    double max;
    double mean;
    double stddev;
    double skew;
    double kurtosis;
    double median;
    double median_d;
    double *img_buf;	/* buffer for image */
    size_t img_buf_cnt;	/* counter of img_buf */
    long npix;
    bool do_sum3;
} __attribute__((aligned(16)));		/* for multi-threaded coding */


/******* This code is generated by create_loops.sh *******/
static long stat_pixels_sum_minmax( double vals[],long n, long ii,long jj,long kk,
                                    const fits_image *this_p, void *_p )
{
    struct stat_pixels_results *resp = (struct stat_pixels_results *)_p;
    double res_sum = resp->sum;
    double res_sum2 = resp->sum2;
    double res_sum3 = resp->sum3;
    double res_sum4 = resp->sum4;
    double res_min = resp->min;
    double res_max = resp->max;
    double *res_img_buf = resp->img_buf;
    size_t res_img_buf_cnt = resp->img_buf_cnt;
    long i, cnt;
    double val, vv;
    cnt = 0;
if ( isfinite(res_min) && isfinite(res_max) ) {
if ( res_img_buf != NULL ) {
if ( resp->do_sum3 == true ) {
    for ( i=0 ; i < n ; i++ ) {
        val = vals[i];
        if ( isfinite(val) ) {
            res_sum += val;
            vv = val * val;
            res_sum2 += vv;
res_sum3 += vv * val;
res_sum4 += vv * vv;
res_img_buf[res_img_buf_cnt] = val;
res_img_buf_cnt ++;
if ( val < res_min ) res_min = val;
if ( res_max < val ) res_max = val;
            cnt ++;
        }
    }
}
else {
    for ( i=0 ; i < n ; i++ ) {
        val = vals[i];
        if ( isfinite(val) ) {
            res_sum += val;
            vv = val * val;
            res_sum2 += vv;
res_img_buf[res_img_buf_cnt] = val;
res_img_buf_cnt ++;
if ( val < res_min ) res_min = val;
if ( res_max < val ) res_max = val;
            cnt ++;
        }
    }
}
}
else {
if ( resp->do_sum3 == true ) {
    for ( i=0 ; i < n ; i++ ) {
        val = vals[i];
        if ( isfinite(val) ) {
            res_sum += val;
            vv = val * val;
            res_sum2 += vv;
res_sum3 += vv * val;
res_sum4 += vv * vv;
if ( val < res_min ) res_min = val;
if ( res_max < val ) res_max = val;
            cnt ++;
        }
    }
}
else {
    for ( i=0 ; i < n ; i++ ) {
        val = vals[i];
        if ( isfinite(val) ) {
            res_sum += val;
            vv = val * val;
            res_sum2 += vv;
if ( val < res_min ) res_min = val;
if ( res_max < val ) res_max = val;
            cnt ++;
        }
    }
}
}
}
else {
if ( res_img_buf != NULL ) {
if ( resp->do_sum3 == true ) {
    for ( i=0 ; i < n ; i++ ) {
        val = vals[i];
        if ( isfinite(val) ) {
            res_sum += val;
            vv = val * val;
            res_sum2 += vv;
res_sum3 += vv * val;
res_sum4 += vv * vv;
res_img_buf[res_img_buf_cnt] = val;
res_img_buf_cnt ++;
if ( isfinite(res_min) ) {
if ( val < res_min ) res_min = val;
} else res_min = val;
if ( isfinite(res_max) ) {
if ( res_max < val ) res_max = val;
} else res_max = val;
            cnt ++;
        }
    }
}
else {
    for ( i=0 ; i < n ; i++ ) {
        val = vals[i];
        if ( isfinite(val) ) {
            res_sum += val;
            vv = val * val;
            res_sum2 += vv;
res_img_buf[res_img_buf_cnt] = val;
res_img_buf_cnt ++;
if ( isfinite(res_min) ) {
if ( val < res_min ) res_min = val;
} else res_min = val;
if ( isfinite(res_max) ) {
if ( res_max < val ) res_max = val;
} else res_max = val;
            cnt ++;
        }
    }
}
}
else {
if ( resp->do_sum3 == true ) {
    for ( i=0 ; i < n ; i++ ) {
        val = vals[i];
        if ( isfinite(val) ) {
            res_sum += val;
            vv = val * val;
            res_sum2 += vv;
res_sum3 += vv * val;
res_sum4 += vv * vv;
if ( isfinite(res_min) ) {
if ( val < res_min ) res_min = val;
} else res_min = val;
if ( isfinite(res_max) ) {
if ( res_max < val ) res_max = val;
} else res_max = val;
            cnt ++;
        }
    }
}
else {
    for ( i=0 ; i < n ; i++ ) {
        val = vals[i];
        if ( isfinite(val) ) {
            res_sum += val;
            vv = val * val;
            res_sum2 += vv;
if ( isfinite(res_min) ) {
if ( val < res_min ) res_min = val;
} else res_min = val;
if ( isfinite(res_max) ) {
if ( res_max < val ) res_max = val;
} else res_max = val;
            cnt ++;
        }
    }
}
}
}

    resp->sum = res_sum;
    resp->sum2 = res_sum2;
    resp->sum3 = res_sum3;
    resp->sum4 = res_sum4;
    resp->min = res_min;
    resp->max = res_max;
    resp->img_buf_cnt = res_img_buf_cnt;

    return cnt;
}
/******* End of generated code *******/

/******* This code is generated by create_loops.sh *******/
static long stat_pixels_sum( double vals[],long n, long ii,long jj,long kk,
			     const fits_image *this_p, void *_p )
{
    struct stat_pixels_results *resp = (struct stat_pixels_results *)_p;
    double res_sum = resp->sum;
    double res_sum2 = resp->sum2;
    double res_sum3 = resp->sum3;
    double res_sum4 = resp->sum4;
    double *res_img_buf = resp->img_buf;
    size_t res_img_buf_cnt = resp->img_buf_cnt;
    long i, cnt;
    double val, vv;
    cnt = 0;
if ( res_img_buf != NULL ) {
if ( resp->do_sum3 == true ) {
    for ( i=0 ; i < n ; i++ ) {
        val = vals[i];
        if ( isfinite(val) ) {
            res_sum += val;
            vv = val * val;
            res_sum2 += vv;
res_sum3 += vv * val;
res_sum4 += vv * vv;
res_img_buf[res_img_buf_cnt] = val;
res_img_buf_cnt ++;
            cnt ++;
        }
    }
}
else {
    for ( i=0 ; i < n ; i++ ) {
        val = vals[i];
        if ( isfinite(val) ) {
            res_sum += val;
            vv = val * val;
            res_sum2 += vv;
res_img_buf[res_img_buf_cnt] = val;
res_img_buf_cnt ++;
            cnt ++;
        }
    }
}
}
else {
if ( resp->do_sum3 == true ) {
    for ( i=0 ; i < n ; i++ ) {
        val = vals[i];
        if ( isfinite(val) ) {
            res_sum += val;
            vv = val * val;
            res_sum2 += vv;
res_sum3 += vv * val;
res_sum4 += vv * vv;
            cnt ++;
        }
    }
}
else {
    for ( i=0 ; i < n ; i++ ) {
        val = vals[i];
        if ( isfinite(val) ) {
            res_sum += val;
            vv = val * val;
            res_sum2 += vv;
            cnt ++;
        }
    }
}
}

    resp->sum = res_sum;
    resp->sum2 = res_sum2;
    resp->sum3 = res_sum3;
    resp->sum4 = res_sum4;
    resp->img_buf_cnt = res_img_buf_cnt;

    return cnt;
}
/******* End of generated code *******/


#if 0
static long stat_pixels_sum( double vals[],long n, long ii,long jj,long kk,
			     const fits_image *this_p, void *_p )
{
    struct stat_pixels_results *resp = (struct stat_pixels_results *)_p;
    long i, cnt;
    double vv;
    cnt = 0;
    if ( resp->img_buf != NULL ) {
	for ( i=0 ; i < n ; i++ ) {
	    if ( isfinite(vals[i]) ) {
		resp->sum += vals[i];
		vv = vals[i] * vals[i];
		resp->sum2 += vv;
		resp->sum3 += vv * vals[i];
		resp->sum4 += vv * vv;
		/* set buffer */
		resp->img_buf[resp->img_buf_cnt] = vals[i];
		resp->img_buf_cnt ++;
		cnt ++;
	    }
	}
    }
    else {
	for ( i=0 ; i < n ; i++ ) {
	    if ( isfinite(vals[i]) ) {
		resp->sum += vals[i];
		vv = vals[i] * vals[i];
		resp->sum2 += vv;
		resp->sum3 += vv * vals[i];
		resp->sum4 += vv * vv;
		cnt ++;
	    }
	}
    }
    return cnt;
}

static long stat_pixels_sum_minmax( double vals[],long n, long ii,long jj,long kk,
			     const fits_image *this_p, void *_p )
{
    struct stat_pixels_results *resp = (struct stat_pixels_results *)_p;
    long i, cnt;
    double val, vv;
    cnt = 0;
    if ( isfinite(resp->min) && isfinite(resp->max) ) {
	if ( resp->img_buf != NULL ) {
	    for ( i=0 ; i < n ; i++ ) {
		val = vals[i];
		if ( isfinite(val) ) {
		    /* min, max */
		    if ( val < resp->min ) resp->min = val;
		    if ( resp->max < val ) resp->max = val;
		    /* sum */
		    resp->sum += vals[i];
		    vv = vals[i] * vals[i];
		    resp->sum2 += vv;
		    resp->sum3 += vv * vals[i];
		    resp->sum4 += vv * vv;
		    /* set buffer */
		    resp->img_buf[resp->img_buf_cnt] = vals[i];
		    resp->img_buf_cnt ++;
		    cnt ++;
		}
	    }
	}
	else {
	    for ( i=0 ; i < n ; i++ ) {
		val = vals[i];
		if ( isfinite(val) ) {
		    /* min, max */
		    if ( val < resp->min ) resp->min = val;
		    if ( resp->max < val ) resp->max = val;
		    /* sum */
		    resp->sum += vals[i];
		    vv = vals[i] * vals[i];
		    resp->sum2 += vv;
		    resp->sum3 += vv * vals[i];
		    resp->sum4 += vv * vv;
		    cnt ++;
		}
	    }
	}
    }
    else {
	for ( i=0 ; i < n ; i++ ) {
	    val = vals[i];
	    if ( isfinite(val) ) {
		/* min, max */
		if ( isfinite(resp->min) ) {
		    if ( val < resp->min ) resp->min = val;
		}
		else resp->min = val;
		if ( isfinite(resp->max) ) {
		    if ( resp->max < val ) resp->max = val;
		}
		else resp->max = val;
		/* sum */
		resp->sum += vals[i];
		vv = vals[i] * vals[i];
		resp->sum2 += vv;
		resp->sum3 += vv * vals[i];
		resp->sum4 += vv * vv;
		/* set buffer */
		if ( resp->img_buf != NULL ) {
		    resp->img_buf[resp->img_buf_cnt] = vals[i];
		    resp->img_buf_cnt ++;
		}
		cnt ++;
	    }
	}
    }
    return cnt;
}
#endif	/* if 0 */

#if 0
static long stat_pixels_minmax( double vals[],long n ,long ii,long jj,long kk,
				const fits_image *this_p, void *_p )
{
    struct stat_pixels_results *resp = (struct stat_pixels_results *)_p;
    long i, cnt;
    double val;
    cnt = 0;
    if ( isfinite(resp->min) && isfinite(resp->max) ) {
	for ( i=0 ; i < n ; i++ ) {
	    val = vals[i];
	    if ( isfinite(val) ) {
		if ( val < resp->min ) resp->min = val;
		if ( resp->max < val ) resp->max = val;
		cnt ++;
	    }
	}
    }
    else {
	for ( i=0 ; i < n ; i++ ) {
	    val = vals[i];
	    if ( isfinite(val) ) {
		if ( isfinite(resp->min) ) {
		    if ( val < resp->min ) resp->min = val;
		}
		else resp->min = val;
		if ( isfinite(resp->max) ) {
		    if ( resp->max < val ) resp->max = val;
		}
		else resp->max = val;
		cnt ++;
	    }
	}
    }
    return cnt;
}

static long stat_pixels_med( double vals[],long n ,long ii,long jj,long kk,
			     const fits_image *this_p, void *_p )
{
    struct stat_pixels_results *resp = (struct stat_pixels_results *)_p;
    long i, cnt;
    cnt = 0;
    for ( i=0 ; i < n ; i++ ) {
	if ( isfinite(vals[i]) ) {
	    resp->img_buf[resp->img_buf_cnt] = vals[i];
	    resp->img_buf_cnt ++;
	    cnt ++;
	}
    }
    return cnt;
}
#endif	/* if 0 */

#ifndef USE_STD_SORT
static int compar_double( const void *_a, const void *_b )
{
    register const double *a = (const double *)_a;
    register const double *b = (const double *)_b;
    if ( *a < *b ) return -1;
    else if ( *b < *a ) return 1;
    else return 0;
}
#endif


/*
 * 正確な median (IRAF の「インチキmedian」こと midpt ではない) を求める．
 *
 * quick sort のアルゴリズムを参考に，必要な部分だけ並べかえるアルゴリズム
 * を使っている．std::sort() でやるより 2倍程度速く，IRAF の midpt 並の速度
 * で計算できる．やってる事は八木先生謹製のネコソフト median 関数とほぼ同じ．
 *
 * リファレンス:
 * http://www.sciencedirect.com/science/article/pii/S0022000073800339
 *
 * buf_ptr:  配列の先頭アドレス．バッファには NAN や INF を入れてはいけない．
 *           なお，バッファの内容は上書きされるので注意．
 * buf_len:  配列の個数
 *
 */
/* デバッグのスイッチ */
//#define DEBUG_GET_MEDIAN 1
static double get_median( double *buf_ptr, const size_t buf_len )
{
    double ret_value = NAN;
    size_t src_start, src_end1;			/* ループの開始，終了+1 */
    size_t buf_pos_right1;			/* ループ終了時の分岐点 */
    double pivot;

    if ( buf_ptr == NULL ) goto quit;
    if ( buf_len == 0 ) goto quit;

    src_start = 0;
    src_end1 = buf_len;

    pivot = buf_ptr[0];

    /*
     * メインループ
     */
    while ( 1 ) {
	double v, v1;
	size_t i, j;

#ifdef DEBUG_GET_MEDIAN
	err_report1(__FUNCTION__,"DEBUG", "buf_len/2: %zu", buf_len / 2);
	err_report1(__FUNCTION__,"DEBUG", "src_start: %zu", src_start);
	err_report1(__FUNCTION__,"DEBUG", "src_end1: %zu", src_end1);
	err_report1(__FUNCTION__,"DEBUG", "pivot: %.15g", pivot);
#endif

	j = src_end1;
	for ( i=src_start ; i < j ; i++ ) {
	    v = buf_ptr[i];
	    /* この不等号は `<=' である必要がある */
	    if ( v <= pivot ) {
		/* ここで次の pivot 値の候補をとっても良いが， */
		/* こっちの区間が捨てられる可能性があるからなぁ．．． */
	    }
	    else {
		while ( i < j ) {
		    /* ここは i==j の可能性があるが，OK */
		    j--;
		    v1 = buf_ptr[j];
		    if ( v1 <= pivot ) {
			buf_ptr[j] = v;
			buf_ptr[i] = v1;
			break;
		    }
		}
	    }
	}
	/* 注意: ここで i を取得してはいけない */
	buf_pos_right1 = j;

	/*
	 * 次回の試行区間を決定する
	 */

	/*
	 * pivot 値がほぼ median 値としてヒットしてしまった場合
	 */
	if ( buf_len % 2 == 0 && buf_pos_right1 == buf_len / 2 ) {
	    size_t m_pos;
	    double l_max, r_min;
	    /*
	     * 更新されたバッファにおいて，
	     * 左側区間では最大値を，右側区間では最小値をとる
	     */
	    m_pos = buf_len / 2;
	    /* */
	    j = src_start;
	    l_max = buf_ptr[j];
	    j++;
	    for ( ; j < m_pos ; j++ ) {
		if ( l_max < buf_ptr[j] ) l_max = buf_ptr[j];
	    }
	    /* */
	    j = m_pos;
	    r_min = buf_ptr[j];
	    j++;
	    for ( ; j < src_end1 ; j++ ) {
		if ( buf_ptr[j] < r_min ) r_min = buf_ptr[j];
	    }
	    /* 左側の最大，右側の最小，の平均をとって，終了 */
	    ret_value = (l_max + r_min) / 2.0;
#ifdef DEBUG_GET_MEDIAN
	    err_report(__FUNCTION__,"DEBUG", "case [X]");
#endif
	    goto quit;
	}
	/*  pivot が左側にある場合
	 *                   piv    |
	 *  |_________________+____________________________|
	 *      do not use                 use
	 */
	/* ここの不等号は，要素数が奇数の場合に `<=' である必要がある．  */
	/* 要素数が偶数の時に == な場合は，↑の if 文の条件にかかるので，*/
	/* ここにはこない．                                              */
	else if ( buf_pos_right1 <= buf_len / 2 ) {
	    pivot = buf_ptr[buf_pos_right1];
	    src_start = buf_pos_right1;
#ifdef DEBUG_GET_MEDIAN
	    err_report(__FUNCTION__,"DEBUG", "case [A]");
#endif
	}
	/*  pivot が右側にある場合
	 *                          |  piv
	 *  |___________________________+___________________|
	 *             use                   do not use
	 */
	else {

	    /* pivot 値よりすべてが左側にきた場合 */
	    if ( src_end1 == buf_pos_right1 ) {
		/* 次の pivot 値を見つける */
		for ( j=src_start ; j < src_end1 ; j++ ) {
		    if ( buf_ptr[j] != pivot ) {
			pivot = buf_ptr[j];
			break;
		    }
		}
		if ( j == src_end1 ) {
		    ret_value = buf_ptr[src_start];
		    goto quit;				/* finish! */
		}
	    }
	    else {
		pivot = buf_ptr[src_start];
	    }

	    /* */
	    src_end1 = buf_pos_right1;
#ifdef DEBUG_GET_MEDIAN
	    err_report(__FUNCTION__,"DEBUG", "case [B]");
#endif
	}

#ifdef DEBUG_GET_MEDIAN
	err_report(__FUNCTION__,"DEBUG", "-----------------");
#endif

    }

#ifdef DEBUG_GET_MEDIAN
    err_report1(__FUNCTION__,"DEBUG", 
		"ret_value of median: %.15g", ret_value);
#endif


 quit:
    return ret_value;
}


/* get pixel statistics */
long fits_image::stat_pixels( double results[], size_t results_buf_len, 
			      const char *options_str,
			      long col_index, long col_size, 
			      long row_index, long row_size,
			      long layer_index, long layer_size ) const
{
    long return_val = -1;
    fits_header res;
    long i;

    return_val = this->stat_pixels(&res, options_str,
				   col_index, col_size, row_index, row_size,
				   layer_index, layer_size);

    if ( results == NULL ) goto quit;

    /* set results to results[] */
    for ( i=0 ; i < res.length() && i < (long)results_buf_len ; i++ ) {
	results[i] = res.at(i).dvalue();
    }

quit:
    return return_val;
}

long fits_image::stat_pixels( fits_header *results, const char *options_str, 
			      long col_index, long col_size, 
			      long row_index, long row_size,
			      long layer_index, long layer_size ) const
{
    asarray_tstring options;
    const char *key = NULL;
    asarray<size_t> odr_res;
    struct stat_pixels_results res;
    mdarray image_buf(DOUBLE_ZT);
    size_t image_buf_len;
    bool do_minmax_with_sum;

    res.npix = -1;

    /*
     * Parse options arg and get fields
     */

    parse_option_args( options_str, &options );

    if ( 0 <= options.index("results") ) key = "results";
    else if ( 0 <= options.index("fields") ) key = "fields";
    if ( key != NULL ) {
	tarray_tstring ret_list;
	size_t j;
	ret_list.split(options[key], ", ", false).toupper();
	for ( j=0 ; j < ret_list.length() ; j++ ) {
	    odr_res[ret_list.cstr(j)] = j;
	    if ( results != NULL ) {
		if ( results->index(ret_list.cstr(j)) < 0 ) {
		    results->append(ret_list.cstr(j));
		}
	    }
	}
    }

    /*
     * Statistics
     */

    /* obtain actual region */
    if ( this->fix_section_args(&col_index, &col_size, 
				&row_index, &row_size,
				&layer_index, &layer_size) < 0 ) goto quit;

    image_buf_len = col_size *  row_size * layer_size;

    //err_report1(__FUNCTION__,"DEBUG","nbuf ... %ld",
    //	col_size *  row_size * layer_size);

    if ( 0 <= odr_res.index("MEDIAN") || 0 <= odr_res.index("MEDIAN_DEBUG") ) {
	//err_report(__FUNCTION__,"DEBUG","alloc start");
	image_buf.set_auto_init(false);
	image_buf.resize(image_buf_len);
	res.img_buf = (double *)(image_buf.data_ptr());
	//err_report(__FUNCTION__,"DEBUG","alloc end");
    }
    else res.img_buf = NULL;
    res.img_buf_cnt = 0;
    res.do_sum3 = false;

    /* sum と同時に min,max をとる */
    do_minmax_with_sum = 
	(0 <= odr_res.index("MIN") || 0 <= odr_res.index("MAX"));

    res.do_sum3 =
	( 0 <= odr_res.index("SKEW") || 0 <= odr_res.index("KURTOSIS") );

    /* get mean,stddev,skew,kurtosis, min and max */
    if ( 0 <= odr_res.index("NPIX") || do_minmax_with_sum ||
	 0 <= odr_res.index("MEAN") || 0 <= odr_res.index("STDDEV") || 
	 res.do_sum3 == true ) {
	double s_sum, k_sum;
	res.sum = 0;
	res.sum2 = 0;
	res.sum3 = 0;
	res.sum4 = 0;
	//err_report(__FUNCTION__,"DEBUG","scan start");
	if ( do_minmax_with_sum ) {
	    res.min = NAN;
	    res.max = NAN;
	    res.npix = this->scan_along_x(&stat_pixels_sum_minmax, (void *)&res,
				       col_index, col_size, 
				       row_index, row_size,
				       layer_index, layer_size );
	}
	else {
	    res.npix = this->scan_along_x( &stat_pixels_sum, (void *)&res,
					col_index, col_size, 
					row_index, row_size,
					layer_index, layer_size );
	}
	//err_report(__FUNCTION__,"DEBUG","scan end");
	/* STDDEV */
	res.mean = res.sum / res.npix;
	res.stddev = sqrt( (res.sum2 - 2 * res.mean * res.sum 
			   + res.mean * res.mean * res.npix) / (res.npix - 1));
	if ( res.do_sum3 == true ) {
	    /* SKEW */
	    s_sum = res.sum3 - 3 * res.mean * res.sum2 
		+ 3 * res.mean * res.mean * res.sum - pow(res.mean,3) * res.npix;
	    res.skew = (s_sum / pow(res.stddev,3)) / (res.npix - 1);
	    /* KURTOSIS */
	    k_sum = res.sum4 - 4 * res.mean * res.sum3 
		+ 6 * res.mean * res.mean * res.sum2 
		- 4 * pow(res.mean,3) * res.sum + pow(res.mean,4) * res.npix;
	    res.kurtosis = ((k_sum / pow(res.stddev,4)) / (res.npix - 1)) - 3.0;
	}
    }

    /* get min and max */
    //if ( 0 <= odr_res.index("MIN") || 0 <= odr_res.index("MAX") ) {
    //	res.min = NAN;
    //	res.max = NAN;
    //	res.npix = this->scan_along_x( &stat_pixels_minmax, (void *)&res,
    //				    col_index, col_size, 
    //				    row_index, row_size,
    //				    layer_index, layer_size );
    //}

    /* get median */
    if ( 0 <= odr_res.index("MEDIAN") ) {
	//mdarray image_buf(DOUBLE_ZT);
	//image_buf.resize(image_buf_len);
	//res.img_buf = (double *)(image_buf.data_ptr());
	//res.img_buf_cnt = 0;
	//res.npix = this->scan_along_x( &stat_pixels_med, (void *)&res,
	//			    col_index, col_size, 
	//			    row_index, row_size,
	//			    layer_index, layer_size );
	if ( 3 <= res.img_buf_cnt ) {
	    /* obtain median */
	    res.median = get_median(res.img_buf, res.img_buf_cnt);
	}
	else if ( res.img_buf_cnt == 2 ) {
	    res.median = (res.img_buf[0] + res.img_buf[1]) / 2.0;
	}
	else if ( res.img_buf_cnt == 1 ) {
	    res.median = res.img_buf[0];
	}
	else res.median = NAN;
    }

    /* get median (for debug) */
    if ( 0 <= odr_res.index("MEDIAN_DEBUG") ) {
	//mdarray image_buf(DOUBLE_ZT);
	//image_buf.resize(image_buf_len);
	//res.img_buf = (double *)(image_buf.data_ptr());
	//res.img_buf_cnt = 0;
	//res.npix = this->scan_along_x( &stat_pixels_med, (void *)&res,
	//			    col_index, col_size, 
	//			    row_index, row_size,
	//			    layer_index, layer_size );
	if ( 3 <= res.img_buf_cnt ) {
	    /* obtain median */
#ifdef USE_STD_SORT
	    /* partial_sort(), stable_sort() も試したが，遅すぎてダメ */
	    std::sort(res.img_buf, res.img_buf + res.img_buf_cnt);
#else
	    /* これはもっと遅い */
	    qsort((void *)(res.img_buf), res.img_buf_cnt, sizeof(double),
		  &compar_double);
#endif
	    if ( res.img_buf_cnt % 2 == 0 ) {	/* 偶数 */
		res.median_d = (res.img_buf[res.img_buf_cnt / 2] +
			      res.img_buf[(res.img_buf_cnt / 2) - 1]) /2.0;
	    } else {
		res.median_d = res.img_buf[res.img_buf_cnt / 2];
	    }
	}
	else if ( res.img_buf_cnt == 2 ) {
	    res.median_d = (res.img_buf[0] + res.img_buf[1]) / 2.0;
	}
	else if ( res.img_buf_cnt == 1 ) {
	    res.median_d = res.img_buf[0];
	}
	else res.median_d = NAN;
    }

    if ( results == NULL ) goto quit;

    /* set results */
    if ( results != NULL ) {
	if ( 0 <= odr_res.index("NPIX") )
	    results->at("NPIX").assign(res.npix);
	if ( 0 <= odr_res.index("MEAN") )
	    results->at("MEAN").assign(res.mean);
	if ( 0 <= odr_res.index("STDDEV") )
	    results->at("STDDEV").assign(res.stddev);
	if ( 0 <= odr_res.index("SKEW") )
	    results->at("SKEW").assign(res.skew);
	if ( 0 <= odr_res.index("KURTOSIS") )
	    results->at("KURTOSIS").assign(res.kurtosis);
	if ( 0 <= odr_res.index("MIN") )
	    results->at("MIN").assign(res.min);
	if ( 0 <= odr_res.index("MAX") )
	    results->at("MAX").assign(res.max);
	if ( 0 <= odr_res.index("MEDIAN") )
	    results->at("MEDIAN").assign(res.median);
	if ( 0 <= odr_res.index("MEDIAN_DEBUG") )
	    results->at("MEDIAN_DEBUG").assign(res.median_d);
    }

quit:
    return res.npix;
}


/*
 *  COMBINE
 */

static const int COMBINE_AVERAGE = 0;
static const int COMBINE_MEDIAN  = 1;
static const int COMBINE_SUM     = 2;
static const int COMBINE_MIN     = 3;
static const int COMBINE_MAX     = 4;

struct combine_prms {
    double *out_buf_current;
    int method;
};

/*
 *
 *  COMBINE_COLS and COMBINE_ROWS begin
 *
 */

static long combine_1d( double pix[], long n0, long x, long y, long z,
			const fits_image *this_p, void *_prms )
{
    struct combine_prms *prms = (struct combine_prms *)_prms;
    double *out_buf = prms->out_buf_current;
    double v;
    long j, n0_valid, n1_valid = 0, ix = 0;
    const long i = 0, n1 = 1;

    if ( prms->method == COMBINE_AVERAGE ) {
	v = 0;
	n0_valid = 0;
	for ( j=0 ; j < n0 ; j++ ) {
	    if ( isfinite(pix[ix]) ) {			/* if valid */
		v += pix[ix];
		n0_valid ++;
	    }
	    ix ++;					/* count always */
	}
	if ( 0 < n0_valid ) {
	    out_buf[i] = v / n0_valid;			/* save the result */
	    n1_valid ++;
	}
	else out_buf[i] = NAN;
    }
    else if ( prms->method == COMBINE_MEDIAN ) {
	long ix0 = n0 * i;
	v = NAN;
	n0_valid = 0;
	/* use valid pixels only */
	for ( j=0 ; j < n0 ; j++ ) {
	    if ( isfinite(pix[ix0 + j]) ) {		/* if valid */
		pix[ix0 + n0_valid] = pix[ix0 + j];
		n0_valid ++;
	    }
	}
	/* obtain median */
	if ( 3 <= n0_valid ) {
	    out_buf[i] = get_median(pix + ix0, n0_valid);
	    n1_valid ++;
	}
	else if ( n0_valid == 2 ) {
	    out_buf[i] = (pix[ix0] + pix[ix0 + 1]) / 2.0;
	    n1_valid ++;
	}
	else if ( n0_valid == 1 ) {
	    out_buf[i] = pix[ix0];
	    n1_valid ++;
	}
	else out_buf[i] = NAN;
    }
    else if ( prms->method == COMBINE_SUM ) {
	v = 0;
	n0_valid = 0;
	for ( j=0 ; j < n0 ; j++ ) {
	    if ( isfinite(pix[ix]) ) {			/* if valid */
		v += pix[ix];
		n0_valid ++;
	    }
	    ix ++;					/* count always */
	}
	if ( 0 < n0_valid ) {
	    out_buf[i] = v;				/* save the result */
	    n1_valid ++;
	}
	else out_buf[i] = NAN;
    }
    else if ( prms->method == COMBINE_MAX ) {
	v = NAN;
	n0_valid = 0;
	for ( j=0 ; j < n0 ; j++ ) {
	    if ( isfinite(pix[ix]) ) {			/* if valid */
		if ( ! isfinite(v) ) v = pix[ix];
		else if ( v < pix[ix] ) v = pix[ix];
		n0_valid ++;
	    }
	    ix ++;					/* count always */
	}
	if ( 0 < n0_valid ) {
	    out_buf[i] = v;				/* save the result */
	    n1_valid ++;
	}
	else out_buf[i] = NAN;
    }
    else if ( prms->method == COMBINE_MIN ) {
	v = NAN;
	n0_valid = 0;
	for ( j=0 ; j < n0 ; j++ ) {
	    if ( isfinite(pix[ix]) ) {			/* if valid */
		if ( ! isfinite(v) ) v = pix[ix];
		else if ( pix[ix] < v ) v = pix[ix];
		n0_valid ++;
	    }
	    ix ++;					/* count always */
	}
	if ( 0 < n0_valid ) {
	    out_buf[i] = v;				/* save the result */
	    n1_valid ++;
	}
	else out_buf[i] = NAN;
    }
    else {
	out_buf[i] = NAN;
    }

    prms->out_buf_current += n1;			/* set next position */
    return n1_valid;

}

/* combine cols of each (y)                                  */
/* combine=average (average|median|sum|min|max)              */
/* outtype=double  (short|ushort|long|longlong|float|double) */
long fits_image::combine_cols( fits_image *dest_img, const char *options_str,
			       long col_index, long col_size, 
			       long row_index, long row_size,
			       long layer_index, long layer_size ) const
{
    long ret_value = -1;
    asarray_tstring options;
    struct combine_prms prms;
    long ndim = 3;
    long naxis[3];

    if ( dest_img == NULL ) goto quit;

    if ( options_str == NULL ) options_str = "";

    /* parse options string and get associative string array */
    parse_option_args( options_str, &options );

    if ( this->fix_section_args(&col_index, &col_size, 
				&row_index, &row_size,
				&layer_index, &layer_size) < 0 ) goto quit;

    /*
     * prepare buffer for output
     */
    /* initialize */
    naxis[0] = 1;
    naxis[1] = row_size;
    naxis[2] = layer_size;
    if ( this->dim_length() == 2 ) ndim = 2;
    dest_img->init(FITS::DOUBLE_T, naxis, ndim, false);
    /* copy header*/
    dest_img->header_init(this->header());
    /* get buffer address */
    prms.out_buf_current = (double *)dest_img->data_ptr();

    /* combine=average (average|median|sum|min|max) */

    prms.method = COMBINE_AVERAGE;			/* default: average */
    if ( 0 <= options.index("combine") ) {
	const tstring &optval = options["combine"];
	if ( optval.strcmp("average") == 0 ) prms.method = COMBINE_AVERAGE;
	else if ( optval.strcmp("median") == 0 ) prms.method = COMBINE_MEDIAN;
	else if ( optval.strcmp("sum") == 0 ) prms.method = COMBINE_SUM;
	else if ( optval.strcmp("max") == 0 ) prms.method = COMBINE_MAX;
	else if ( optval.strcmp("min") == 0 ) prms.method = COMBINE_MIN;
    }

    /* perform combine */

    ret_value = this->scan_along_x(&combine_1d, (void *)(&prms),
				   col_index, col_size, row_index, row_size,
				   layer_index, layer_size);

    /* outtype=double (short|ushort|long|longlong|float|double) */

    if ( 0 <= options.index("outtype") ) {
	const tstring &optval = options["outtype"];
	/* 先に丸めておかないと，rawデータが正負で丸めの方向が変わってしまう */
	if ( optval.strcmp("ushort") == 0 ) {
	    dest_img->data_array().round();
	    dest_img->convert_type(FITS::SHORT_T, 32768.0);
	}
	else {
	    int tp = FITS::DOUBLE_T;
	    if ( optval.strcmp("short") == 0 ) tp = FITS::SHORT_T;
	    else if ( optval.strcmp("long") == 0 ) tp = FITS::LONG_T;
	    else if ( optval.strcmp("float") == 0 ) tp = FITS::FLOAT_T;
	    else if ( optval.strcmp("longlong") == 0 ) tp = FITS::LONGLONG_T;
	    if ( tp != FITS::DOUBLE_T ) dest_img->convert_type(tp);
	}
    }

 quit:
    return ret_value;
}

/* combine rows of each (x)                                  */
/* combine=average (average|median|sum|min|max)              */
/* outtype=double  (short|ushort|long|longlong|float|double) */
long fits_image::combine_rows( fits_image *dest_img, const char *options_str,
			       long col_index, long col_size, 
			       long row_index, long row_size,
			       long layer_index, long layer_size ) const
{
    long ret_value = -1;
    asarray_tstring options;
    struct combine_prms prms;
    long ndim = 3;
    long naxis[3];

    if ( dest_img == NULL ) goto quit;

    if ( options_str == NULL ) options_str = "";

    /* parse options string and get associative string array */
    parse_option_args( options_str, &options );

    if ( this->fix_section_args(&col_index, &col_size, 
				&row_index, &row_size,
				&layer_index, &layer_size) < 0 ) goto quit;

    /*
     * prepare buffer for output
     */
    /* initialize */
    naxis[0] = col_size;
    naxis[1] = 1;
    naxis[2] = layer_size;
    if ( this->dim_length() == 2 ) ndim = 2;
    dest_img->init(FITS::DOUBLE_T, naxis, ndim, false);
    /* copy header*/
    dest_img->header_init(this->header());
    /* get buffer address */
    prms.out_buf_current = (double *)dest_img->data_ptr();

    /* combine=average (average|median|sum|min|max) */

    prms.method = COMBINE_AVERAGE;			/* default: average */
    if ( 0 <= options.index("combine") ) {
	const tstring &optval = options["combine"];
	if ( optval.strcmp("average") == 0 ) prms.method = COMBINE_AVERAGE;
	else if ( optval.strcmp("median") == 0 ) prms.method = COMBINE_MEDIAN;
	else if ( optval.strcmp("sum") == 0 ) prms.method = COMBINE_SUM;
	else if ( optval.strcmp("max") == 0 ) prms.method = COMBINE_MAX;
	else if ( optval.strcmp("min") == 0 ) prms.method = COMBINE_MIN;
    }

    /* perform combine */

    ret_value = this->scan_along_y(&combine_1d, (void *)(&prms),
	    col_index, col_size, row_index, row_size, layer_index, layer_size);

    /* outtype=double (short|ushort|long|longlong|float|double) */

    if ( 0 <= options.index("outtype") ) {
	const tstring &optval = options["outtype"];
	/* 先に丸めておかないと，rawデータが正負で丸めの方向が変わってしまう */
	if ( optval.strcmp("ushort") == 0 ) {
	    dest_img->data_array().round();
	    dest_img->convert_type(FITS::SHORT_T, 32768.0);
	}
	else {
	    int tp = FITS::DOUBLE_T;
	    if ( optval.strcmp("short") == 0 ) tp = FITS::SHORT_T;
	    else if ( optval.strcmp("long") == 0 ) tp = FITS::LONG_T;
	    else if ( optval.strcmp("float") == 0 ) tp = FITS::FLOAT_T;
	    else if ( optval.strcmp("longlong") == 0 ) tp = FITS::LONGLONG_T;
	    if ( tp != FITS::DOUBLE_T ) dest_img->convert_type(tp);
	}
    }

 quit:
    return ret_value;
}

/*
 *
 *  COMBINE_LAYERS begins
 *
 */

static long combine_2d( double pix[], long n0, long n1, long x, long y, long z,
			const fits_image *this_p, void *_prms )
{
    struct combine_prms *prms = (struct combine_prms *)_prms;
    double *out_buf = prms->out_buf_current;
    double v;
    long i,j, n0_valid, n1_valid = 0, ix = 0;

    if ( prms->method == COMBINE_AVERAGE ) {
	for ( i=0 ; i < n1 ; i++ ) {			/* loop for X */
	    v = 0;
	    n0_valid = 0;
	    for ( j=0 ; j < n0 ; j++ ) {		/* loop for Z */
		if ( isfinite(pix[ix]) ) {		/* if valid */
		    v += pix[ix];
		    n0_valid ++;
		}
		ix ++;					/* count always */
	    }
	    if ( 0 < n0_valid ) {
		out_buf[i] = v / n0_valid;		/* save the result */
		n1_valid ++;
	    }
	    else out_buf[i] = NAN;
	}
    }
    else if ( prms->method == COMBINE_MEDIAN ) {
	for ( i=0 ; i < n1 ; i++ ) {			/* loop for X */
	    long ix0 = n0 * i;
	    v = NAN;
	    n0_valid = 0;
	    /* use valid pixels only */
	    for ( j=0 ; j < n0 ; j++ ) {		/* loop for Z */
		if ( isfinite(pix[ix0 + j]) ) {		/* if valid */
		    pix[ix0 + n0_valid] = pix[ix0 + j];
		    n0_valid ++;
		}
	    }
	    /* obtain median */
	    if ( 3 <= n0_valid ) {
		out_buf[i] = get_median(pix + ix0, n0_valid);
		n1_valid ++;
	    }
	    else if ( n0_valid == 2 ) {
		out_buf[i] = (pix[ix0] + pix[ix0 + 1]) / 2.0;
		n1_valid ++;
	    }
	    else if ( n0_valid == 1 ) {
		out_buf[i] = pix[ix0];
		n1_valid ++;
	    }
	    else out_buf[i] = NAN;
	}
    }
    else if ( prms->method == COMBINE_SUM ) {
	for ( i=0 ; i < n1 ; i++ ) {			/* loop for X */
	    v = 0;
	    n0_valid = 0;
	    for ( j=0 ; j < n0 ; j++ ) {		/* loop for Z */
		if ( isfinite(pix[ix]) ) {		/* if valid */
		    v += pix[ix];
		    n0_valid ++;
		}
		ix ++;					/* count always */
	    }
	    if ( 0 < n0_valid ) {
		out_buf[i] = v;				/* save the result */
		n1_valid ++;
	    }
	    else out_buf[i] = NAN;
	}
    }
    else if ( prms->method == COMBINE_MAX ) {
	for ( i=0 ; i < n1 ; i++ ) {			/* loop for X */
	    v = NAN;
	    n0_valid = 0;
	    for ( j=0 ; j < n0 ; j++ ) {		/* loop for Z */
		if ( isfinite(pix[ix]) ) {		/* if valid */
		    if ( ! isfinite(v) ) v = pix[ix];
		    else if ( v < pix[ix] ) v = pix[ix];
		    n0_valid ++;
		}
		ix ++;					/* count always */
	    }
	    if ( 0 < n0_valid ) {
		out_buf[i] = v;				/* save the result */
		n1_valid ++;
	    }
	    else out_buf[i] = NAN;
	}
    }
    else if ( prms->method == COMBINE_MIN ) {
	for ( i=0 ; i < n1 ; i++ ) {			/* loop for X */
	    v = NAN;
	    n0_valid = 0;
	    for ( j=0 ; j < n0 ; j++ ) {		/* loop for Z */
		if ( isfinite(pix[ix]) ) {		/* if valid */
		    if ( ! isfinite(v) ) v = pix[ix];
		    else if ( pix[ix] < v ) v = pix[ix];
		    n0_valid ++;
		}
		ix ++;					/* count always */
	    }
	    if ( 0 < n0_valid ) {
		out_buf[i] = v;				/* save the result */
		n1_valid ++;
	    }
	    else out_buf[i] = NAN;
	}
    }
    else {
	for ( i=0 ; i < n1 ; i++ ) out_buf[i] = NAN;
    }

    prms->out_buf_current += n1;			/* set next position */
    return n1_valid;

}

/* combine layers of each (x,y).                             */
/* combine=average (average|median|sum|min|max)              */
/* outtype=double  (short|ushort|long|longlong|float|double) */
long fits_image::combine_layers( fits_image *dest_img, const char *options_str,
				 long col_index, long col_size, 
				 long row_index, long row_size,
				 long layer_index, long layer_size ) const
{
    long ret_value = -1;
    asarray_tstring options;
    struct combine_prms prms;
    long naxis[2];

    if ( dest_img == NULL ) goto quit;

    if ( options_str == NULL ) options_str = "";

    /* parse options string and get associative string array */
    parse_option_args( options_str, &options );

    if ( this->fix_section_args(&col_index, &col_size, 
				&row_index, &row_size,
				&layer_index, &layer_size) < 0 ) goto quit;

    /*
     * prepare buffer for output
     */
    /* initialize */
    naxis[0] = col_size;
    naxis[1] = row_size;
    dest_img->init(FITS::DOUBLE_T, naxis, 2, false);
    /* copy header*/
    dest_img->header_init(this->header());
    /* get buffer address */
    prms.out_buf_current = (double *)dest_img->data_ptr();

    /* combine=average (average|median|sum|min|max) */

    prms.method = COMBINE_AVERAGE;			/* default: average */
    if ( 0 <= options.index("combine") ) {
	const tstring &optval = options["combine"];
	if ( optval.strcmp("average") == 0 ) prms.method = COMBINE_AVERAGE;
	else if ( optval.strcmp("median") == 0 ) prms.method = COMBINE_MEDIAN;
	else if ( optval.strcmp("sum") == 0 ) prms.method = COMBINE_SUM;
	else if ( optval.strcmp("max") == 0 ) prms.method = COMBINE_MAX;
	else if ( optval.strcmp("min") == 0 ) prms.method = COMBINE_MIN;
    }

    /* perform combine */

    ret_value = this->scan_zx_planes(&combine_2d, (void *)(&prms),
	    col_index, col_size, row_index, row_size, layer_index, layer_size);

    /* outtype=double (short|ushort|long|longlong|float|double) */

    if ( 0 <= options.index("outtype") ) {
	const tstring &optval = options["outtype"];
	/* 先に丸めておかないと，rawデータが正負で丸めの方向が変わってしまう */
	if ( optval.strcmp("ushort") == 0 ) {
	    dest_img->data_array().round();
	    dest_img->convert_type(FITS::SHORT_T, 32768.0);
	}
	else {
	    int tp = FITS::DOUBLE_T;
	    if ( optval.strcmp("short") == 0 ) tp = FITS::SHORT_T;
	    else if ( optval.strcmp("long") == 0 ) tp = FITS::LONG_T;
	    else if ( optval.strcmp("float") == 0 ) tp = FITS::FLOAT_T;
	    else if ( optval.strcmp("longlong") == 0 ) tp = FITS::LONGLONG_T;
	    if ( tp != FITS::DOUBLE_T ) dest_img->convert_type(tp);
	}
    }

 quit:
    return ret_value;
}


