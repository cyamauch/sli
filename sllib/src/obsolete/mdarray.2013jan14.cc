
/** 
 * 配列の一部を切り出し,別オブジェクトへコピーします.
 * 
 * @param dest_obj コピー先のオブジェクト
 * @param col_index コピー元の列の位置
 * @param col_size コピー元の列サイズ
 * @param row_index コピー元の行位置
 * @param row_size コピー元の行サイズ
 * @param layer_index コピー元のレイヤ位置
 * @param layer_size コピー元のレイヤサイズ
 * @return 自身の参照
 * @throw バッファの確保に失敗した場合．メモリ破壊を起こした場合
 * @note このメンバ関数はprivateです．<br>
 *       dest_objに自分自身あるはNULLを与えると，clean()と同じ動作をします．
 */
ssize_t mdarray::image_copy_and_cut( mdarray *dest_obj,
				     ssize_t col_index, size_t col_size, 
				     ssize_t row_index, size_t row_size,
				     ssize_t layer_index, size_t layer_size )
{
    const size_t this_bytes = this->bytes();
    ssize_t ret = -1;
    bool col_ok, row_ok, layer_ok;
    size_t i, j;
    const char *j_src_ptr;
    const char *i_src_ptr;
    char *dest_ptr;
    heap_mem<size_t> tmp_size;
    size_t tmp_dim_size;
    bool layer_all = (layer_index == 0 && layer_size == MDARRAY_ALL);

    if ( this->length() == 0 ) goto quit;

    if ( this->test_section_args( &col_index, &col_size, 
				 &row_index, &row_size,
				 &layer_index, &layer_size,
				 &col_ok, &row_ok, &layer_ok ) < 0 ) goto quit;

    /* 元の次元情報をコピー */
    tmp_dim_size = this->dim_size_rec;
    tmp_size = dup_size_rec(this->size_rec, this->dim_size_rec);
    if ( tmp_size.ptr() == NULL ) {
	err_throw(__FUNCTION__,"FATAL","malloc() failed");
    }

    /* 自分自身では無い場合 */
    if ( dest_obj != NULL && dest_obj != this ) {
	size_t col_blen, colrow_blen, dest_line_unit;

	if ( row_ok == false ) {			/* 1 次元 */
	    const size_t naxisx[1] = {col_size};
	    const size_t ndim = 1;
	    if ( dest_obj->size_type() == this->size_type() &&
		 dest_obj->length() == col_size ) {
		dest_obj->reallocate(naxisx, ndim, false);
	    }
	    else dest_obj->init(this->size_type(), naxisx, ndim, false);
	}
	else if ( layer_ok == false ) {			/* 2 次元 */
	    const size_t naxisx[2] = {col_size,row_size};
	    const size_t ndim = 2;
	    if ( dest_obj->size_type() == this->size_type() &&
		 dest_obj->length() == col_size * row_size ) {
		dest_obj->reallocate(naxisx, ndim, false);
	    }
	    else dest_obj->init(this->size_type(), naxisx, ndim, false);
	}
	else {						/* 3 次元以上 */
	    /* 4次元以上の場合，全レイヤ指定なら4次元以降の軸情報を残す */
	    if ( 3 < this->dim_length() && layer_all == true ) {
		size_t len_dest = 1;
		/* 1〜2次元だけ変更 */
		tmp_size[0] = col_size;
		tmp_size[1] = row_size;
		for ( i=0 ; i < tmp_dim_size ; i++ ) len_dest *= tmp_size[i];
		if ( dest_obj->size_type() == this->size_type() &&
		     dest_obj->length() == len_dest ) {
		    dest_obj->reallocate(tmp_size.ptr(), tmp_dim_size, false);
		}
		else {
		    dest_obj->init(this->size_type(), 
				   tmp_size.ptr(), tmp_dim_size, false);
		}
	    }
	    else {
		const size_t naxisx[3] = {col_size,row_size,layer_size};
		const size_t ndim = 3;
		if ( dest_obj->size_type() == this->size_type() &&
		     dest_obj->length() == col_size * row_size * layer_size ) {
		    dest_obj->reallocate(naxisx, ndim, false);
		}
		else dest_obj->init(this->size_type(), naxisx, ndim, false);
	    }
	}
	dest_obj->init_properties(*this);
	dest_obj->assign_default(this->default_rec);

	col_blen = this_bytes * this->col_length();
	colrow_blen = col_blen * this->row_length();
	dest_line_unit = this_bytes * col_size;

	dest_ptr = (char *)dest_obj->data_ptr();
	j_src_ptr = (const char *)(this->data_ptr_cs()) 
		    + colrow_blen * layer_index		/* offset */
		    + col_blen * row_index
		    + this_bytes * col_index;
	for ( j=0 ; j < layer_size ; j++ ) {
	    i_src_ptr = j_src_ptr;
	    for ( i=0 ; i < row_size ; i++ ) {
		s_memcpy( dest_ptr, i_src_ptr, dest_line_unit,
			  dest_line_unit * row_size * layer_size );
		dest_ptr += dest_line_unit;
		i_src_ptr += col_blen;
	    }
	    j_src_ptr += colrow_blen;
	}
    }

    /* cut! */
    this->clean( col_index,col_size, row_index,row_size,
		 layer_index,layer_size );


    ret = layer_size * row_size * col_size;

 quit:
    return ret;
}


/* double 型のスカラー演算専用 */
namespace calc_sc_n
{
#define MAKE_FUNC(fncname,dest_sz_type,dest_type,fnc_d2x,fnc_x2d) \
static void fncname(void *_dst, double src, size_t n, int ope, bool _flg) \
{ \
    size_t i; \
    dest_type *dst = (dest_type *)_dst; \
    const bool flg = _flg; \
    double v0; \
    if ( ope == Ope_plus ) { \
        for ( i=0 ; i < n ; i++ ) { \
            v0 = (double)(fnc_x2d(dst[i])) + src; \
            dst[i] = (dest_type)(fnc_d2x(v0)); \
        } \
    } else if ( ope == Ope_minus ) { \
        for ( i=0 ; i < n ; i++ ) { \
            v0 = (double)(fnc_x2d(dst[i])) - src; \
            dst[i] = (dest_type)(fnc_d2x(v0)); \
        } \
    } else if ( ope == Ope_star ) { \
        for ( i=0 ; i < n ; i++ ) { \
            v0 = (double)(fnc_x2d(dst[i])) * src; \
            dst[i] = (dest_type)(fnc_d2x(v0)); \
        } \
    } else { \
        for ( i=0 ; i < n ; i++ ) { \
            v0 = (double)(fnc_x2d(dst[i])) / src; \
            dst[i] = (dest_type)(fnc_d2x(v0)); \
        } \
    } \
    i = flg; /* junk code for warning */ \
}
SLI__MDARRAY__DO_OPERATION_DOUBLE2ANY_ALL(MAKE_FUNC,flg ? round_d2c(v0) : trunc_d2c,flg ? round_d2i16(v0) : trunc_d2i16,flg ? round_d2i32(v0) : trunc_d2i32,flg ? round_d2i64(v0) : trunc_d2i64,f2fx,d2dx,ld2ldx,);
#undef MAKE_FUNC
}


/* secinfo に従って再帰的にセクションの clean を行なう */
static void calc_scalar_r( double value, int ope, 
			   void *this_buf, mdarray *thisp,
			   void (*func)(void *, double, size_t, int, bool),
			   const heap_mem<section_expinfo> &secinfo,
			   size_t dim_ix, size_t this_len_block )
{
    const size_t bytes = thisp->bytes();
    const size_t *this_dim = thisp->cdimarray();
    char *d_ptr = (char *)this_buf;
    if ( dim_ix == 0 ) {
	d_ptr += bytes * secinfo[0].begin;
	(*func)(d_ptr, value, secinfo[0].length, ope, thisp->rounding());
    }
    else {
	size_t i;
	/* */
	this_len_block /= this_dim[dim_ix];
	/* */
	d_ptr += bytes * this_len_block * secinfo[dim_ix].begin;
	for ( i=0 ; i < secinfo[dim_ix].length ; i++ ) {
	    calc_scalar_r(value, ope, d_ptr, thisp, func,
			  secinfo, dim_ix - 1, this_len_block);
	    d_ptr += bytes * this_len_block;
	}
    }
    return;
}

mdarray &mdarray::image_vcalcf_scalar( double value, int ope,
				       const char *exp_fmt, va_list ap )
{
    void (*func)(void *, double, size_t, int, bool);
    heap_mem<char> secexp;
    heap_mem<section_expinfo> secinfo;
    heap_mem<size_t> naxisx;
    size_t n_secinfo = 0;
    size_t this_total_elements, i;

    if ( this->length() == 0 ) goto quit;
    if ( exp_fmt == NULL ) goto quit;

    func = NULL;
#define SEL_FUNC(fncname,dest_sz_type,dest_type,fnc_d2x,fnc_x2d) \
    if ( this->sz_type_rec == dest_sz_type ) { \
        func = &calc_sc_n::fncname; \
    }
    SLI__MDARRAY__DO_OPERATION_DOUBLE2ANY_ALL(SEL_FUNC,,,,,,,,else);
#undef SEL_FUNC

    if ( func == NULL ) {
	err_report(__FUNCTION__,"WARNING","invalid size of type; ignored");
	goto quit;
    }

    if ( c_vasprintf(secexp.ptr_address(), exp_fmt, ap) < 0 ) {
	err_report(__FUNCTION__,"FATAL","c_vasprintf() failed");
	goto quit;
    }

    /* パースし，secinfo を生成 */
    if ( parse_section_exp(&secinfo, &n_secinfo, secexp.ptr()) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","Error in parse_section_exp()");
    }
    if ( n_secinfo == 0 ) goto quit;

    /* secinfo をチェック，修正 */
    if ( test_section_info( secexp.ptr(), *this, 
			    &secinfo, &n_secinfo, &naxisx ) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","Error in test_section_info()");
    }
    if ( n_secinfo == 0 ) goto quit;

    this_total_elements = 1;
    for ( i=0 ; i < n_secinfo ; i++ ) {
	this_total_elements *= this->length(i);
    }

    /* 再帰的に n 次元のセクションの clean を行なう */
    calc_scalar_r( value, ope, 
		   this->data_ptr(), this, func, 
		   secinfo, n_secinfo - 1, this_total_elements );

 quit:
    return *this;
}


/* 加算，減算，乗算，除算に特化した関数 */
mdarray &mdarray::image_calc_scalar( double value, int ope,
				     ssize_t col_index, size_t col_size,
				     ssize_t row_index, size_t row_size,
				     ssize_t layer_index, size_t layer_size )
{
    const size_t this_bytes = this->bytes();
    void (*func)(void *, double, size_t, int, bool);
    size_t j, k;
    size_t col_blen, colrow_blen;
    char *k_ptr;
    char *j_ptr;

    if ( this->length() == 0 ) goto quit;

    func = NULL;
#define SEL_FUNC(fncname,dest_sz_type,dest_type,fnc_d2x,fnc_x2d) \
    if ( this->sz_type_rec == dest_sz_type ) { \
        func = &calc_sc_n::fncname; \
    }
    SLI__MDARRAY__DO_OPERATION_DOUBLE2ANY_ALL(SEL_FUNC,,,,,,,,else);
#undef SEL_FUNC

    if ( func == NULL ) {
	err_report(__FUNCTION__,"WARNING","invalid size of type; ignored");
	goto quit;
    }

    if ( this->fix_section_args( &col_index, &col_size, 
				 &row_index, &row_size,
				 &layer_index, &layer_size ) < 0 ) goto quit;

    if ( col_size == 0 ) goto quit;

    col_blen = this_bytes * this->col_length();
    colrow_blen = col_blen * this->row_length();

    k_ptr = (char *)(this->data_ptr()) 
	  + colrow_blen * layer_index			/* offset */
	  + col_blen * row_index
	  + this_bytes * col_index;
    for ( k=0 ; k < layer_size ; k++ ) {
	j_ptr = k_ptr;
	for ( j=0 ; j < row_size ; j++ ) {
	    (*func)((void *)j_ptr, value, col_size, ope, this->rounding());
	    j_ptr += col_blen;
	}
	k_ptr += colrow_blen;
    }

 quit:
    return *this;
}
