
//static unsigned char (*fnptr_f2c[])(float) = { &trunc_f2c, &round_f2c };

//static int16_t (*fnptr_f2i16[])(float) = { &trunc_f2i16, &round_f2i16 };

//static int32_t (*fnptr_f2i32[])(float) = { &trunc_f2i32, &round_f2i32 };

//static int64_t (*fnptr_f2i64[])(float) = { &trunc_f2i64, &round_f2i64 };

//static unsigned char (*fnptr_d2c[])(double) = { &trunc_d2c, &round_d2c };

//static int16_t (*fnptr_d2i16[])(double) = { &trunc_d2i16, &round_d2i16 };

//static int32_t (*fnptr_d2i32[])(double) = { &trunc_d2i32, &round_d2i32 };

//static int64_t (*fnptr_d2i64[])(double) = { &trunc_d2i64, &round_d2i64 };

//static unsigned char (*fnptr_ld2c[])(long double) = { &trunc_ld2c, &round_ld2c };

//static int16_t (*fnptr_ld2i16[])(long double) = { &trunc_ld2i16, &round_ld2i16 };

//static int32_t (*fnptr_ld2i32[])(long double) = { &trunc_ld2i32, &round_ld2i32 };

//static int64_t (*fnptr_ld2i64[])(long double) = { &trunc_ld2i64, &round_ld2i64 };

//static unsigned char (*fnptr_fx2c[])(fcomplex) = { &trunc_fx2c, &round_fx2c };

//static int16_t (*fnptr_fx2i16[])(fcomplex) = { &trunc_fx2i16, &round_fx2i16 };

//static int32_t (*fnptr_fx2i32[])(fcomplex) = { &trunc_fx2i32, &round_fx2i32 };

//static int64_t (*fnptr_fx2i64[])(fcomplex) = { &trunc_fx2i64, &round_fx2i64 };

//static unsigned char (*fnptr_dx2c[])(dcomplex) = { &trunc_dx2c, &round_dx2c };

//static int16_t (*fnptr_dx2i16[])(dcomplex) = { &trunc_dx2i16, &round_dx2i16 };

//static int32_t (*fnptr_dx2i32[])(dcomplex) = { &trunc_dx2i32, &round_dx2i32 };

//static int64_t (*fnptr_dx2i64[])(dcomplex) = { &trunc_dx2i64, &round_dx2i64 };

//static unsigned char (*fnptr_ldx2c[])(ldcomplex) = { &trunc_ldx2c, &round_ldx2c };

//static int16_t (*fnptr_ldx2i16[])(ldcomplex) = { &trunc_ldx2i16, &round_ldx2i16 };

//static int32_t (*fnptr_ldx2i32[])(ldcomplex) = { &trunc_ldx2i32, &round_ldx2i32 };

//static int64_t (*fnptr_ldx2i64[])(ldcomplex) = { &trunc_ldx2i64, &round_ldx2i64 };


#if 0

namespace cnv
{
#define MAKE_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
static void fncname(const void *org_val_ptr, void *new_val_ptr, void *u_ptr) \
{ \
    org_type v0 = *((const org_type *)org_val_ptr); \
    *((new_type *)new_val_ptr) = (new_type)(fnc(v0)); \
}
//SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(MAKE_FUNC, fnptr_f2c[*((size_t *)u_ptr)],fnptr_f2i16[*((size_t *)u_ptr)],fnptr_f2i32[*((size_t *)u_ptr)],fnptr_f2i64[*((size_t *)u_ptr)], fnptr_d2c[*((size_t *)u_ptr)],fnptr_d2i16[*((size_t *)u_ptr)],fnptr_d2i32[*((size_t *)u_ptr)],fnptr_d2i64[*((size_t *)u_ptr)], fnptr_ld2c[*((size_t *)u_ptr)],fnptr_ld2i16[*((size_t *)u_ptr)],fnptr_ld2i32[*((size_t *)u_ptr)],fnptr_ld2i64[*((size_t *)u_ptr)], fnptr_fx2c[*((size_t *)u_ptr)],fnptr_fx2i16[*((size_t *)u_ptr)],fnptr_fx2i32[*((size_t *)u_ptr)],fnptr_fx2i64[*((size_t *)u_ptr)], fnptr_dx2c[*((size_t *)u_ptr)],fnptr_dx2i16[*((size_t *)u_ptr)],fnptr_dx2i32[*((size_t *)u_ptr)],fnptr_dx2i64[*((size_t *)u_ptr)], fnptr_ldx2c[*((size_t *)u_ptr)],fnptr_ldx2i16[*((size_t *)u_ptr)],fnptr_ldx2i32[*((size_t *)u_ptr)],fnptr_ldx2i64[*((size_t *)u_ptr)],f2fx,d2dx,ld2ldx,);
SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(MAKE_FUNC, *((size_t *)u_ptr) ? round_f2c(v0) : trunc_f2c,*((size_t *)u_ptr) ? round_f2i16(v0) : trunc_f2i16,*((size_t *)u_ptr) ? round_f2i32(v0) : trunc_f2i32,*((size_t *)u_ptr) ? round_f2i64(v0) : trunc_f2i64, *((size_t *)u_ptr) ? round_d2c(v0) : trunc_d2c,*((size_t *)u_ptr) ? round_d2i16(v0) : trunc_d2i16,*((size_t *)u_ptr) ? round_d2i32(v0) : trunc_d2i32,*((size_t *)u_ptr) ? round_d2i64(v0) : trunc_d2i64, *((size_t *)u_ptr) ? round_ld2c(v0) : trunc_ld2c,*((size_t *)u_ptr) ? round_ld2i16(v0) : trunc_ld2i16,*((size_t *)u_ptr) ? round_ld2i32(v0) : trunc_ld2i32,*((size_t *)u_ptr) ? round_ld2i64(v0) : trunc_ld2i64, *((size_t *)u_ptr) ? round_fx2c(v0) : trunc_fx2c,*((size_t *)u_ptr) ? round_fx2i16(v0) : trunc_fx2i16,*((size_t *)u_ptr) ? round_fx2i32(v0) : trunc_fx2i32,*((size_t *)u_ptr) ? round_fx2i64(v0) : trunc_fx2i64, *((size_t *)u_ptr) ? round_dx2c(v0) : trunc_dx2c,*((size_t *)u_ptr) ? round_dx2i16(v0) : trunc_dx2i16,*((size_t *)u_ptr) ? round_dx2i32(v0) : trunc_dx2i32,*((size_t *)u_ptr) ? round_dx2i64(v0) : trunc_dx2i64, *((size_t *)u_ptr) ? round_ldx2c(v0) : trunc_ldx2c,*((size_t *)u_ptr) ? round_ldx2i16(v0) : trunc_ldx2i16,*((size_t *)u_ptr) ? round_ldx2i32(v0) : trunc_ldx2i32,*((size_t *)u_ptr) ? round_ldx2i64(v0) : trunc_ldx2i64,f2fx,d2dx,ld2ldx,);
#undef MAKE_FUNC
}


/* ������Ǥ�����Ǥ�OK�ʾ�� */
namespace cnv_n
{
#define MAKE_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
static void fncname(const void *org_val_ptr, void *new_val_ptr, size_t n, void *u_ptr) \
{ \
    size_t i; \
    const org_type *src = (const org_type *)org_val_ptr; \
    new_type *dst = (new_type *)new_val_ptr; \
    const size_t flg = *((size_t *)u_ptr); \
    for ( i=0 ; i < n ; i++ ) { \
        dst[i] = (new_type)(fnc(src[i])); \
    } \
    i = flg; /* junk code for warning */ \
}
//SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(MAKE_FUNC, fnptr_f2c[*((size_t *)u_ptr)],fnptr_f2i16[*((size_t *)u_ptr)],fnptr_f2i32[*((size_t *)u_ptr)],fnptr_f2i64[*((size_t *)u_ptr)], fnptr_d2c[*((size_t *)u_ptr)],fnptr_d2i16[*((size_t *)u_ptr)],fnptr_d2i32[*((size_t *)u_ptr)],fnptr_d2i64[*((size_t *)u_ptr)], fnptr_ld2c[*((size_t *)u_ptr)],fnptr_ld2i16[*((size_t *)u_ptr)],fnptr_ld2i32[*((size_t *)u_ptr)],fnptr_ld2i64[*((size_t *)u_ptr)], fnptr_fx2c[*((size_t *)u_ptr)],fnptr_fx2i16[*((size_t *)u_ptr)],fnptr_fx2i32[*((size_t *)u_ptr)],fnptr_fx2i64[*((size_t *)u_ptr)], fnptr_dx2c[*((size_t *)u_ptr)],fnptr_dx2i16[*((size_t *)u_ptr)],fnptr_dx2i32[*((size_t *)u_ptr)],fnptr_dx2i64[*((size_t *)u_ptr)], fnptr_ldx2c[*((size_t *)u_ptr)],fnptr_ldx2i16[*((size_t *)u_ptr)],fnptr_ldx2i32[*((size_t *)u_ptr)],fnptr_ldx2i64[*((size_t *)u_ptr)],f2fx,d2dx,ld2ldx,);
SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(MAKE_FUNC, flg ? round_f2c(src[i]) : trunc_f2c,flg ? round_f2i16(src[i]) : trunc_f2i16,flg ? round_f2i32(src[i]) : trunc_f2i32,flg ? round_f2i64(src[i]) : trunc_f2i64, flg ? round_d2c(src[i]) : trunc_d2c,flg ? round_d2i16(src[i]) : trunc_d2i16,flg ? round_d2i32(src[i]) : trunc_d2i32,flg ? round_d2i64(src[i]) : trunc_d2i64, flg ? round_ld2c(src[i]) : trunc_ld2c,flg ? round_ld2i16(src[i]) : trunc_ld2i16,flg ? round_ld2i32(src[i]) : trunc_ld2i32,flg ? round_ld2i64(src[i]) : trunc_ld2i64, flg ? round_fx2c(src[i]) : trunc_fx2c,flg ? round_fx2i16(src[i]) : trunc_fx2i16,flg ? round_fx2i32(src[i]) : trunc_fx2i32,flg ? round_fx2i64(src[i]) : trunc_fx2i64, flg ? round_dx2c(src[i]) : trunc_dx2c,flg ? round_dx2i16(src[i]) : trunc_dx2i16,flg ? round_dx2i32(src[i]) : trunc_dx2i32,flg ? round_dx2i64(src[i]) : trunc_dx2i64, flg ? round_ldx2c(src[i]) : trunc_ldx2c,flg ? round_ldx2i16(src[i]) : trunc_ldx2i16,flg ? round_ldx2i32(src[i]) : trunc_ldx2i32,flg ? round_ldx2i64(src[i]) : trunc_ldx2i64,f2fx,d2dx,ld2ldx,);
#undef MAKE_FUNC
}

#endif	/* 0 */



/* ������Ǥ�����Ǥ�OK�ʾ��(dest¦��step�ǥ�����ȥ��å�) */
namespace cnv_n_dstep
{
#define MAKE_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
static void fncname(const void *org_val_ptr, void *new_val_ptr, size_t n, size_t dest_step, void *u_ptr) \
{ \
    size_t i, j; \
    const org_type *src = (const org_type *)org_val_ptr; \
    new_type *dst = (new_type *)new_val_ptr; \
    const size_t flg = *((size_t *)u_ptr); \
    j = 0; \
    for ( i=0 ; i < n ; i++ ) { \
        dst[j] = (new_type)(fnc(src[i])); \
        j += dest_step; \
    } \
    i = flg; /* junk code for warning */ \
}
//SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(MAKE_FUNC, fnptr_f2c[*((size_t *)u_ptr)],fnptr_f2i16[*((size_t *)u_ptr)],fnptr_f2i32[*((size_t *)u_ptr)],fnptr_f2i64[*((size_t *)u_ptr)], fnptr_d2c[*((size_t *)u_ptr)],fnptr_d2i16[*((size_t *)u_ptr)],fnptr_d2i32[*((size_t *)u_ptr)],fnptr_d2i64[*((size_t *)u_ptr)], fnptr_ld2c[*((size_t *)u_ptr)],fnptr_ld2i16[*((size_t *)u_ptr)],fnptr_ld2i32[*((size_t *)u_ptr)],fnptr_ld2i64[*((size_t *)u_ptr)], fnptr_fx2c[*((size_t *)u_ptr)],fnptr_fx2i16[*((size_t *)u_ptr)],fnptr_fx2i32[*((size_t *)u_ptr)],fnptr_fx2i64[*((size_t *)u_ptr)], fnptr_dx2c[*((size_t *)u_ptr)],fnptr_dx2i16[*((size_t *)u_ptr)],fnptr_dx2i32[*((size_t *)u_ptr)],fnptr_dx2i64[*((size_t *)u_ptr)], fnptr_ldx2c[*((size_t *)u_ptr)],fnptr_ldx2i16[*((size_t *)u_ptr)],fnptr_ldx2i32[*((size_t *)u_ptr)],fnptr_ldx2i64[*((size_t *)u_ptr)],f2fx,d2dx,ld2ldx,);
SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(MAKE_FUNC, flg ? round_f2c(src[i]) : trunc_f2c,flg ? round_f2i16(src[i]) : trunc_f2i16,flg ? round_f2i32(src[i]) : trunc_f2i32,flg ? round_f2i64(src[i]) : trunc_f2i64, flg ? round_d2c(src[i]) : trunc_d2c,flg ? round_d2i16(src[i]) : trunc_d2i16,flg ? round_d2i32(src[i]) : trunc_d2i32,flg ? round_d2i64(src[i]) : trunc_d2i64, flg ? round_ld2c(src[i]) : trunc_ld2c,flg ? round_ld2i16(src[i]) : trunc_ld2i16,flg ? round_ld2i32(src[i]) : trunc_ld2i32,flg ? round_ld2i64(src[i]) : trunc_ld2i64, flg ? round_fx2c(src[i]) : trunc_fx2c,flg ? round_fx2i16(src[i]) : trunc_fx2i16,flg ? round_fx2i32(src[i]) : trunc_fx2i32,flg ? round_fx2i64(src[i]) : trunc_fx2i64, flg ? round_dx2c(src[i]) : trunc_dx2c,flg ? round_dx2i16(src[i]) : trunc_dx2i16,flg ? round_dx2i32(src[i]) : trunc_dx2i32,flg ? round_dx2i64(src[i]) : trunc_dx2i64, flg ? round_ldx2c(src[i]) : trunc_ldx2c,flg ? round_ldx2i16(src[i]) : trunc_ldx2i16,flg ? round_ldx2i32(src[i]) : trunc_ldx2i32,flg ? round_ldx2i64(src[i]) : trunc_ldx2i64,f2fx,d2dx,ld2ldx,);
#undef MAKE_FUNC
}

/* ������Ǥ�����Ǥ�OK�ʾ��(src¦��step�ǥ�����ȥ��å�) */
namespace cnv_n_sstep
{
#define MAKE_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
static void fncname(const void *org_val_ptr, void *new_val_ptr, size_t n, size_t src_step, void *u_ptr) \
{ \
    size_t i, j; \
    const org_type *src = (const org_type *)org_val_ptr; \
    new_type *dst = (new_type *)new_val_ptr; \
    const size_t flg = *((size_t *)u_ptr); \
    j = 0; \
    for ( i=0 ; i < n ; i++ ) { \
        dst[i] = (new_type)(fnc(src[j])); \
        j += src_step; \
    } \
    i = flg; /* junk code for warning */ \
}
//SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(MAKE_FUNC, fnptr_f2c[*((size_t *)u_ptr)],fnptr_f2i16[*((size_t *)u_ptr)],fnptr_f2i32[*((size_t *)u_ptr)],fnptr_f2i64[*((size_t *)u_ptr)], fnptr_d2c[*((size_t *)u_ptr)],fnptr_d2i16[*((size_t *)u_ptr)],fnptr_d2i32[*((size_t *)u_ptr)],fnptr_d2i64[*((size_t *)u_ptr)], fnptr_ld2c[*((size_t *)u_ptr)],fnptr_ld2i16[*((size_t *)u_ptr)],fnptr_ld2i32[*((size_t *)u_ptr)],fnptr_ld2i64[*((size_t *)u_ptr)], fnptr_fx2c[*((size_t *)u_ptr)],fnptr_fx2i16[*((size_t *)u_ptr)],fnptr_fx2i32[*((size_t *)u_ptr)],fnptr_fx2i64[*((size_t *)u_ptr)], fnptr_dx2c[*((size_t *)u_ptr)],fnptr_dx2i16[*((size_t *)u_ptr)],fnptr_dx2i32[*((size_t *)u_ptr)],fnptr_dx2i64[*((size_t *)u_ptr)], fnptr_ldx2c[*((size_t *)u_ptr)],fnptr_ldx2i16[*((size_t *)u_ptr)],fnptr_ldx2i32[*((size_t *)u_ptr)],fnptr_ldx2i64[*((size_t *)u_ptr)],f2fx,d2dx,ld2ldx,);
SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(MAKE_FUNC, flg ? round_f2c(src[i]) : trunc_f2c,flg ? round_f2i16(src[i]) : trunc_f2i16,flg ? round_f2i32(src[i]) : trunc_f2i32,flg ? round_f2i64(src[i]) : trunc_f2i64, flg ? round_d2c(src[i]) : trunc_d2c,flg ? round_d2i16(src[i]) : trunc_d2i16,flg ? round_d2i32(src[i]) : trunc_d2i32,flg ? round_d2i64(src[i]) : trunc_d2i64, flg ? round_ld2c(src[i]) : trunc_ld2c,flg ? round_ld2i16(src[i]) : trunc_ld2i16,flg ? round_ld2i32(src[i]) : trunc_ld2i32,flg ? round_ld2i64(src[i]) : trunc_ld2i64, flg ? round_fx2c(src[i]) : trunc_fx2c,flg ? round_fx2i16(src[i]) : trunc_fx2i16,flg ? round_fx2i32(src[i]) : trunc_fx2i32,flg ? round_fx2i64(src[i]) : trunc_fx2i64, flg ? round_dx2c(src[i]) : trunc_dx2c,flg ? round_dx2i16(src[i]) : trunc_dx2i16,flg ? round_dx2i32(src[i]) : trunc_dx2i32,flg ? round_dx2i64(src[i]) : trunc_dx2i64, flg ? round_ldx2c(src[i]) : trunc_ldx2c,flg ? round_ldx2i16(src[i]) : trunc_ldx2i16,flg ? round_ldx2i32(src[i]) : trunc_ldx2i32,flg ? round_ldx2i64(src[i]) : trunc_ldx2i64,f2fx,d2dx,ld2ldx,);
#undef MAKE_FUNC
}


#if 0
/**
 * @deprecated ̤�����Ǥ�.
 */
mdarray &mdarray::cropf( const char *exp_fmt, ... )
{
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	this->vcropf(exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vcropf() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @deprecated ̤�����Ǥ�.
 */
mdarray &mdarray::vcropf( const char *exp_fmt, va_list ap )
{
    err_throw(__FUNCTION__,"FATAL","Do not use this member function");
    return *this;
}
#endif	/* 0 */

#if 0
mdarray &mdarray::clean()
{
    if ( this->default_rec != NULL ) {
	return this->clean(this->default_rec);
    }
    else {
	size_t i;
	size_t sz = this->length() * this->bytes();
	unsigned char *pp = (unsigned char *)(this->data_ptr());
	for ( i=0 ; i < sz ; i++ ) {
	    pp[i] = 0;
	}
	return *this;
    }
}

mdarray &mdarray::clean(const void *value_ptr)
{
    size_t len = this->length();
    size_t sz  = this->bytes();
    if ( value_ptr != NULL ) {
	size_t i, k;
	unsigned char *pp = (unsigned char *)(this->data_ptr());
	for ( i=0, k=0 ; i < len ; i++ ) {
	    size_t j;
	    for ( j=0 ; j < sz ; j++, k++ ) {
		pp[k] = ((const unsigned char *)value_ptr)[j];
	    }
	}
    }
    return *this;
}
#endif

/**
 * ���Ȥ�����λ��ꤵ�줿�ϰϤ����Ǥ򡤹�ñ�̤ǥ�����󤷤ޤ���(�饹������
 * �����)��1��ʬ�Υǡ����ˤĤ�1�󡤥桼������ؿ� func ���ƤӽФ��졤func ��
 * ������Ϳ����줿1��ʬ�Υǡ����ˤĤ��Ʋ��餫�ν�����Ԥʤ������Ǥ��ޤ���
 * ����1��ʬ�Υǡ����ϥ饤�֥�꤬��������ƥ�ݥ��Хåե��Ǥ��ꡤ���֥�����
 * �Ȥ������������Ǥ��äƤ� double �����󶡤���ޤ���<br>
 * �桼������ؿ�func�ΰ����ˤϽ�ˡ����Ȥ����Ǥ�1��ʬ�����󡤤�������θĿ���
 * ����֡��԰��֡��쥤����֡����ȤΥ��֥������ȤΥ��ɥ쥹���桼���ݥ��󥿤���
 * ��Ϳ�����ޤ������δؿ��ϡ�ͭ�������Ǥο����֤��褦�ˤ��ޤ���<br>
 * �����ǡ��������פ�Ȥ���ʤɤ˻Ȥ����дؿ��Ǥ���
 *
 * @param     func_dest2d ���Ȥ����ǡ�����double �����Ѵ����뤿��Υ桼���ؿ��Υ��ɥ쥹
 * @param     user_ptr_dest2d func dest2d �κǸ��Ϳ������桼���Υݥ���
 * @param     func ���ǥ������ΰ٤Υ桼���ؿ��Υ��ɥ쥹
 * @param     user_ptr func �κǸ��Ϳ������桼���Υݥ���
 * @param     col_index �����
 * @param     col_size �󥵥���
 * @param     row_index �԰���
 * @param     row_size �ԥ�����
 * @param     layer_index �쥤�����
 * @param     layer_size �쥤�䥵����
 * @return    �������ˤ��ͭ�����Ǥο������顼�ξ������͡�
 */
ssize_t mdarray::scan_along_x( 
	void (*func_dest2d)(const void *,void *,size_t,void *), 
	void *user_ptr_dest2d,
	ssize_t (*func)(double [],size_t,ssize_t,ssize_t,ssize_t,const mdarray *,void *),
        void *user_ptr_func,
	ssize_t col_index, size_t col_size,
	ssize_t row_index, size_t row_size,
	ssize_t layer_index, size_t layer_size ) const
{
    const size_t this_bytes = this->bytes();
    ssize_t ret_len = -1;
    size_t col_blen, colrow_blen;
    size_t f_idx;	/* ��ư�����������������Ѵ�������ˤɤ����뤫 */

    heap_mem<double> x_buffer;	/* buffer for 1st arg of user's function */
    double *x_buf_ptr32;	/* 32-byte aligned pointer               */

    if ( this->rounding_rec == true ) f_idx = 1;	/* �ͼθ������Ѵ� */
    else f_idx = 0;					/* �ڤ�ΤƤ��Ѵ� */

    if ( func_dest2d == NULL ) {
        user_ptr_dest2d = (void *)&f_idx;
	func_dest2d = this->func_cnv_x2d_n;
    }

    if ( func_dest2d == NULL ) {
	err_report(__FUNCTION__,"WARNING","invalid size of type; ignored");
	goto quit;
    }

    if ( this->length() == 0 ) goto quit;

    if ( this->fix_section_args( &col_index, &col_size, 
				 &row_index, &row_size,
				 &layer_index, &layer_size ) < 0 ) goto quit;

    ret_len = 0;

    col_blen = this_bytes * this->col_length();
    colrow_blen = col_blen * this->row_length();

    if ( x_buffer.allocate_aligned32(col_size, &x_buf_ptr32) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","malloc() failed");
    }
    else {
	size_t j, k;
	const char *k_ptr;
	const char *j_ptr;
	ssize_t ii = col_index;				/* fixed */
	k_ptr = (const char *)(this->data_ptr()) 
		+ colrow_blen * layer_index		/* offset */
		+ col_blen * row_index
		+ this_bytes * col_index;
	for ( k=0 ; k < layer_size ; k++ ) {
	    ssize_t kk = layer_index + k;
	    j_ptr = k_ptr;
	    for ( j=0 ; j < row_size ; j++ ) {
		ssize_t jj = row_index + j;
		ssize_t n_valid;
		/* */
		(*func_dest2d)((const void *)j_ptr, (void *)x_buf_ptr32,
			       col_size, user_ptr_dest2d);
		n_valid = (*func)(x_buf_ptr32,col_size,ii,jj,kk,this,user_ptr_func);
		/* count valid elements */
		ret_len += n_valid;
		/* */
		j_ptr += col_blen;
	    }
	    k_ptr += colrow_blen;
	}
    }

 quit:
    return ret_len;
}

/**
 * ���Ȥ�����λ��ꤵ�줿�ϰϤ����Ǥ򡤹�ñ�̤ǥ�����󤷤ޤ���(�饹������
 * �����)��1��ʬ�Υǡ����ˤĤ�1�󡤥桼������ؿ� func ���ƤӽФ��졤func ��
 * ������Ϳ����줿1��ʬ�Υǡ����ˤĤ��Ʋ��餫�ν�����Ԥʤ������Ǥ��ޤ���
 * ����1��ʬ�Υǡ����ϥ饤�֥�꤬��������ƥ�ݥ��Хåե��Ǥ��ꡤ���֥�����
 * �Ȥ������������Ǥ��äƤ� double �����󶡤���ޤ���<br>
 * �桼������ؿ�func�ΰ����ˤϽ�ˡ����Ȥ����Ǥ�1��ʬ�����󡤤�������θĿ���
 * ����֡��԰��֡��쥤����֡����ȤΥ��֥������ȤΥ��ɥ쥹���桼���ݥ��󥿤���
 * ��Ϳ�����ޤ������δؿ��ϡ�ͭ�������Ǥο����֤��褦�ˤ��ޤ���<br>
 * �����ǡ��������פ�Ȥ���ʤɤ˻Ȥ����дؿ��Ǥ���
 *
 * @param     func ���ǥ������ΰ٤Υ桼���ؿ��Υ��ɥ쥹
 * @param     user_ptr func �κǸ��Ϳ������桼���Υݥ���
 * @param     col_index �����
 * @param     col_size �󥵥���
 * @param     row_index �԰���
 * @param     row_size �ԥ�����
 * @param     layer_index �쥤�����
 * @param     layer_size �쥤�䥵����
 * @return    �������ˤ��ͭ�����Ǥο������顼�ξ������͡�
 */
ssize_t mdarray::scan_along_x( 
	ssize_t (*func)(double [],size_t,ssize_t,ssize_t,ssize_t,const mdarray *,void *),
	void *user_ptr,
	ssize_t col_index, size_t col_size,
	ssize_t row_index, size_t row_size,
	ssize_t layer_index, size_t layer_size ) const
{
    return this->scan_along_x(NULL,NULL,func,user_ptr,
	       col_index,col_size, row_index,row_size, layer_index,layer_size);
}


/**
 * ���Ȥ�����λ��ꤵ�줿�ϰϤ����Ǥ���ñ�̤ǥ�����󤷤ޤ���1��ʬ�Υǡ�����
 * �Ĥ�1�󡤥桼������ؿ� func ���ƤӽФ��졤func �ϰ�����Ϳ����줿1��ʬ��
 * �ǡ����ˤĤ��Ʋ��餫�ν�����Ԥʤ������Ǥ��ޤ���<br>
 * ����1��ʬ�Υǡ����ϥ饤�֥�꤬��������ƥ�ݥ��Хåե��Ǥ��ꡤ���֥�����
 * �Ȥ������������Ǥ��äƤ� double �����󶡤���ޤ���<br>
 * �桼������ؿ�func�ΰ����ˤϽ�ˡ����Ȥ����Ǥ�1��ʬ�����󡤤�������θĿ���
 * ����֡��԰��֡��쥤����֡����ȤΥ��֥������ȤΥ��ɥ쥹���桼���ݥ��󥿤���
 * ��Ϳ�����ޤ������δؿ��ϡ�ͭ�������Ǥο����֤��褦�ˤ��ޤ���<br>
 * �����ǡ��������פ�Ȥ���ʤɤ˻Ȥ����дؿ��Ǥ���
 *
 * @param     func_dest2d ���Ȥ����ǡ�����double �����Ѵ����뤿��Υ桼���ؿ��Υ��ɥ쥹
 * @param     user_ptr_dest2d func dest2d �κǸ��Ϳ������桼���Υݥ���
 * @param     func ���ǥ������ΰ٤Υ桼���ؿ��Υ��ɥ쥹
 * @param     user_ptr func �κǸ��Ϳ������桼���Υݥ���
 * @param     col_index �����
 * @param     col_size �󥵥���
 * @param     row_index �԰���
 * @param     row_size �ԥ�����
 * @param     layer_index �쥤�����
 * @param     layer_size �쥤�䥵����
 * @return    �������ˤ��ͭ�����Ǥο������顼�ξ������͡�
 */
ssize_t mdarray::scan_along_y( 
	void (*func_dest2d)(const void *,void *,size_t,size_t,void *), 
	void *user_ptr_dest2d,
	ssize_t (*func)(double [],size_t,ssize_t,ssize_t,ssize_t,const mdarray *,void *),
        void *user_ptr_func,
	ssize_t col_index, size_t col_size,
	ssize_t row_index, size_t row_size,
	ssize_t layer_index, size_t layer_size ) const
{
    const size_t this_bytes = this->bytes();
    ssize_t ret_len = -1;
    size_t col_blen, colrow_blen;
    size_t f_idx;	/* ��ư�����������������Ѵ�������ˤɤ����뤫 */

    heap_mem<double> x_buffer;	/* buffer for 1st arg of user's function */
    double *x_buf_ptr32;	/* 32-byte aligned pointer               */

    if ( this->rounding_rec == true ) f_idx = 1;	/* �ͼθ������Ѵ� */
    else f_idx = 0;					/* �ڤ�ΤƤ��Ѵ� */

    if ( func_dest2d == NULL ) {
      user_ptr_dest2d = (void *)&f_idx;
      /* ���Ѵ��Τ���δؿ�������(����obj�η���double) */
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
      if ( this->sz_type_rec == org_sz_type && DOUBLE_ZT == new_sz_type ) { \
        func_dest2d = &cnv_n_sstep::fncname; \
      }
      SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(SEL_FUNC,,,,,,,,,,,,,,,,,,,,,,,,,,,,else);
#undef SEL_FUNC
    }

    if ( func_dest2d == NULL ) {
	err_report(__FUNCTION__,"WARNING","invalid size of type; ignored");
	goto quit;
    }

    if ( this->length() == 0 ) goto quit;

    if ( this->fix_section_args( &col_index, &col_size, 
				 &row_index, &row_size,
				 &layer_index, &layer_size ) < 0 ) goto quit;

    ret_len = 0;

    col_blen = this_bytes * this->col_length();
    colrow_blen = col_blen * this->row_length();

    if ( x_buffer.allocate_aligned32(row_size, &x_buf_ptr32) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","malloc() failed");
    }
    else {
	const size_t col_len = this->col_length();
	size_t j, k;
	const char *k_ptr;
	const char *j_ptr;
	ssize_t jj = row_index;				/* fixed */
	k_ptr = (const char *)(this->data_ptr()) 
		+ colrow_blen * layer_index		/* offset */
		+ col_blen * row_index
		+ this_bytes * col_index;
	for ( k=0 ; k < layer_size ; k++ ) {
	    ssize_t kk = layer_index + k;
	    j_ptr = k_ptr;
	    for ( j=0 ; j < col_size ; j++ ) {
		ssize_t ii = col_index + j;
		ssize_t n_valid;
		/* */
		(*func_dest2d)((const void *)j_ptr, (void *)x_buf_ptr32,
			       row_size, col_len /* step for src */,
			       user_ptr_dest2d);
		n_valid = (*func)(x_buf_ptr32,row_size,ii,jj,kk,this,user_ptr_func);
		/* count valid elements */
		ret_len += n_valid;
		/* */
		j_ptr += this_bytes;
	    }
	    k_ptr += colrow_blen;
	}
    }

 quit:
    return ret_len;
}

/**
 * ���Ȥ�����λ��ꤵ�줿�ϰϤ����Ǥ���ñ�̤ǥ�����󤷤ޤ���1��ʬ�Υǡ�����
 * �Ĥ�1�󡤥桼������ؿ� func ���ƤӽФ��졤func �ϰ�����Ϳ����줿1��ʬ��
 * �ǡ����ˤĤ��Ʋ��餫�ν�����Ԥʤ������Ǥ��ޤ���
 * ����1��ʬ�Υǡ����ϥ饤�֥�꤬��������ƥ�ݥ��Хåե��Ǥ��ꡤ���֥�����
 * �Ȥ������������Ǥ��äƤ� double �����󶡤���ޤ���<br>
 * �桼������ؿ�func�ΰ����ˤϽ�ˡ����Ȥ����Ǥ�1��ʬ�����󡤤�������θĿ���
 * ����֡��԰��֡��쥤����֡����ȤΥ��֥������ȤΥ��ɥ쥹���桼���ݥ��󥿤���
 * ��Ϳ�����ޤ������δؿ��ϡ�ͭ�������Ǥο����֤��褦�ˤ��ޤ���<br>
 * �����ǡ��������פ�Ȥ���ʤɤ˻Ȥ����дؿ��Ǥ���
 *
 * @param     func ���ǥ������ΰ٤Υ桼���ؿ��Υ��ɥ쥹
 * @param     user_ptr func �κǸ��Ϳ������桼���Υݥ���
 * @param     col_index �����
 * @param     col_size �󥵥���
 * @param     row_index �԰���
 * @param     row_size �ԥ�����
 * @param     layer_index �쥤�����
 * @param     layer_size �쥤�䥵����
 * @return    �������ˤ��ͭ�����Ǥο������顼�ξ������͡�
 */
ssize_t mdarray::scan_along_y( 
	ssize_t (*func)(double [],size_t,ssize_t,ssize_t,ssize_t,const mdarray *,void *),
	void *user_ptr,
	ssize_t col_index, size_t col_size,
	ssize_t row_index, size_t row_size,
	ssize_t layer_index, size_t layer_size ) const
{
    return this->scan_along_y(NULL,NULL,func,user_ptr,
	       col_index,col_size, row_index,row_size, layer_index,layer_size);
}


/**
 * ���Ȥ�����λ��ꤵ�줿�ϰϤ����Ǥ�(x,y)�ΰ���1��1�ĤˤĤ��ơ��쥤��������
 * ������󤷤ޤ���(x,y)�ΰ���1�ĤˤĤ�1�󡤥桼������ؿ� func ���ƤӽФ��졤
 * func �ϰ�����Ϳ����줿1�����ǡ����ˤĤ��Ʋ��餫�ν�����Ԥʤ������Ǥ��ޤ���
 * ����1�����ǡ����ϥ饤�֥�꤬��������ƥ�ݥ��Хåե��Ǥ��ꡤ���֥�������
 * �������������Ǥ��äƤ� double �����󶡤���ޤ���<br>
 * �桼������ؿ� func �ΰ����ˤϽ�ˡ����Ȥ������ͤ�z�����ˤ���������(������
 * �쥤�䤬��Ƭ)��z�����ˤ���������θĿ�������֡��԰��֡��쥤����֡����Ȥ�
 * ���֥������ȤΥ��ɥ쥹���桼���ݥ��󥿤��ͤ�Ϳ�����ޤ������δؿ��ϡ�ͭ��
 * �����Ǥο����֤��褦�ˤ��ޤ���<br>
 * ʣ���β����ǡ����� median ��Ȥ���ʤɤ˻Ȥ����дؿ��Ǥ���
 *
 * @param     func_dest2d ���Ȥ����ǡ�����double �����Ѵ����뤿��Υ桼���ؿ��Υ��ɥ쥹
 * @param     user_ptr_dest2d func dest2d �κǸ��Ϳ������桼���Υݥ���
 * @param     func ���ǥ������ΰ٤Υ桼���ؿ��Υ��ɥ쥹
 * @param     user_ptr func �κǸ��Ϳ������桼���Υݥ���
 * @param     col_index �����
 * @param     col_size �󥵥���
 * @param     row_index �԰���
 * @param     row_size �ԥ�����
 * @param     layer_index �쥤�����
 * @param     layer_size �쥤�䥵����
 * @return    �������ˤ��ͭ�����Ǥο������顼�ξ������͡�
 */
ssize_t mdarray::scan_along_z( 
	void (*func_dest2d)(const void *,void *,size_t,size_t,void *), 
	void *user_ptr_dest2d,
	ssize_t (*func)(double [],size_t,ssize_t,ssize_t,ssize_t,const mdarray *,void *),
        void *user_ptr_func,
	ssize_t col_index, size_t col_size,
	ssize_t row_index, size_t row_size,
	ssize_t layer_index, size_t layer_size ) const
{
    const size_t this_bytes = this->bytes();
    ssize_t ret_len = -1;
    size_t col_blen, colrow_blen;
    size_t f_idx;	/* ��ư�����������������Ѵ�������ˤɤ����뤫 */

    heap_mem<double> x_buffer;	/* buffer for 1st arg of user's function */
    double *x_buf_ptr32;	/* 32-byte aligned pointer               */

    if ( this->rounding_rec == true ) f_idx = 1;	/* �ͼθ������Ѵ� */
    else f_idx = 0;					/* �ڤ�ΤƤ��Ѵ� */

    if ( func_dest2d == NULL ) {
      user_ptr_dest2d = (void *)&f_idx;
      /* ���Ѵ��Τ���δؿ�������(����obj�η���double) */
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
      if ( this->sz_type_rec == org_sz_type && DOUBLE_ZT == new_sz_type ) { \
        func_dest2d = &cnv_n_sstep::fncname; \
      }
      SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(SEL_FUNC,,,,,,,,,,,,,,,,,,,,,,,,,,,,else);
#undef SEL_FUNC
    }

    if ( func_dest2d == NULL ) {
	err_report(__FUNCTION__,"WARNING","invalid size of type; ignored");
	goto quit;
    }

    if ( this->length() == 0 ) goto quit;

    if ( this->fix_section_args( &col_index, &col_size, 
				 &row_index, &row_size,
				 &layer_index, &layer_size ) < 0 ) goto quit;

    ret_len = 0;

    col_blen = this_bytes * this->col_length();
    colrow_blen = col_blen * this->row_length();

    if ( x_buffer.allocate_aligned32(layer_size, &x_buf_ptr32) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","malloc() failed");
    }
    else {
	const size_t col_len = this->col_length();
	const size_t row_len = this->row_length();
	size_t j, k;
	const char *k_ptr;
	const char *j_ptr;
	ssize_t kk = layer_index;			/* fixed */
	k_ptr = (const char *)(this->data_ptr()) 
		+ colrow_blen * layer_index		/* offset */
		+ col_blen * row_index
		+ this_bytes * col_index;
	for ( k=0 ; k < row_size ; k++ ) {
	    ssize_t jj = row_index + k;
	    j_ptr = k_ptr;
	    for ( j=0 ; j < col_size ; j++ ) {
		ssize_t ii = col_index + j;
		ssize_t n_valid;
		/* */
		(*func_dest2d)((const void *)j_ptr, (void *)x_buf_ptr32,
			      layer_size, col_len * row_len /* step for src */,
			      user_ptr_dest2d);
		n_valid = (*func)(x_buf_ptr32,layer_size,ii,jj,kk,this,user_ptr_func);
		/* count valid elements */
		ret_len += n_valid;
		/* */
		j_ptr += this_bytes;
	    }
	    k_ptr += col_blen;
	}
    }

 quit:
    return ret_len;
}

/**
 * ���Ȥ�����λ��ꤵ�줿�ϰϤ����Ǥ�(x,y)�ΰ���1��1�ĤˤĤ��ơ��쥤��������
 * ������󤷤ޤ���(x,y)�ΰ���1�ĤˤĤ�1�󡤥桼������ؿ� func ���ƤӽФ��졤
 * func �ϰ�����Ϳ����줿1�����ǡ����ˤĤ��Ʋ��餫�ν�����Ԥʤ������Ǥ��ޤ���
 * ����1�����ǡ����ϥ饤�֥�꤬��������ƥ�ݥ��Хåե��Ǥ��ꡤ���֥�������
 * �������������Ǥ��äƤ� double �����󶡤���ޤ���<br>
 * �桼������ؿ� func �ΰ����ˤϽ�ˡ����Ȥ������ͤ�z�����ˤ���������(������
 * �쥤�䤬��Ƭ)��z�����ˤ���������θĿ�������֡��԰��֡��쥤����֡����Ȥ�
 * ���֥������ȤΥ��ɥ쥹���桼���ݥ��󥿤��ͤ�Ϳ�����ޤ������δؿ��ϡ�ͭ����
 * ���Ǥο����֤��褦�ˤ��ޤ���<br>
 * ʣ���β����ǡ����� median ��Ȥ���ʤɤ˻Ȥ����дؿ��Ǥ���
 *
 * @param     func ���ǥ������ΰ٤Υ桼���ؿ��Υ��ɥ쥹
 * @param     user_ptr func �κǸ��Ϳ������桼���Υݥ���
 * @param     col_index �����
 * @param     col_size �󥵥���
 * @param     row_index �԰���
 * @param     row_size �ԥ�����
 * @param     layer_index �쥤�����
 * @param     layer_size �쥤�䥵����
 * @return    �������ˤ��ͭ�����Ǥο������顼�ξ������͡�
 */
ssize_t mdarray::scan_along_z( 
	ssize_t (*func)(double [],size_t,ssize_t,ssize_t,ssize_t,const mdarray *,void *),
	void *user_ptr,
	ssize_t col_index, size_t col_size,
	ssize_t row_index, size_t row_size,
	ssize_t layer_index, size_t layer_size ) const
{
    return this->scan_along_z(NULL,NULL,func,user_ptr,
	       col_index,col_size, row_index,row_size, layer_index,layer_size);
}


/**
 * ���Ȥ�����λ��ꤵ�줿�ϰϤ����Ǥ�xy��ñ�̤ǥ�����󤷤ޤ���z�ΰ���1�Ĥ�
 * �Ĥ�1�󡤥桼������ؿ� func ���ƤӽФ��졤func �ϰ�����Ϳ����줿2����
 * �ǡ���(1�����ܤ�x������2�����ܤ�y����)�ˤĤ��Ʋ��餫�ν�����Ԥʤ������Ǥ���
 * ��������2�����ǡ����ϥ饤�֥�꤬��������ƥ�ݥ��Хåե��Ǥ��ꡤ���֥���
 * ���Ȥ������������Ǥ��äƤ� double �����󶡤���ޤ���<br>
 * �桼������ؿ� func �ΰ����ˤϽ�ˡ����Ȥ������ͤ�xy�̤ˤ���������x������
 * ����������θĿ���y�����ˤ���������θĿ�������֡��԰��֡��쥤����֡�����
 * �Υ��֥������ȤΥ��ɥ쥹���桼���ݥ��󥿤��ͤ�Ϳ�����ޤ������δؿ��ϡ�ͭ��
 * �����Ǥο����֤��褦�ˤ��ޤ���<br>
 * �����ǡ���1��� median ��Ȥ���ʤɤ˻Ȥ����дؿ��Ǥ���
 *
 * @param     func_dest2d ���Ȥ����ǡ�����double �����Ѵ����뤿��Υ桼���ؿ��Υ��ɥ쥹
 * @param     user_ptr_dest2d func dest2d �κǸ��Ϳ������桼���Υݥ���
 * @param     func ���ǥ������ΰ٤Υ桼���ؿ��Υ��ɥ쥹
 * @param     user_ptr func �κǸ��Ϳ������桼���Υݥ���
 * @param     col_index �����
 * @param     col_size �󥵥���
 * @param     row_index �԰���
 * @param     row_size �ԥ�����
 * @param     layer_index �쥤�����
 * @param     layer_size �쥤�䥵����
 * @return    �������ˤ��ͭ�����Ǥο������顼�ξ������͡�
 */
ssize_t mdarray::scan_xy_planes( 
	void (*func_dest2d)(const void *,void *,size_t,void *),
	void *user_ptr_dest2d,
	ssize_t (*func)(double [],size_t,size_t,ssize_t,ssize_t,ssize_t,const mdarray *,void *),
        void *user_ptr_func,
	ssize_t col_index, size_t col_size,
	ssize_t row_index, size_t row_size,
	ssize_t layer_index, size_t layer_size ) const
{
    const size_t this_bytes = this->bytes();
    ssize_t ret_len = -1;
    size_t col_blen, colrow_blen;
    size_t f_idx;	/* ��ư�����������������Ѵ�������ˤɤ����뤫 */

    heap_mem<double> x_buffer;	/* buffer for 1st arg of user's function */
    double *x_buf_ptr32;	/* 32-byte aligned pointer               */

    if ( this->rounding_rec == true ) f_idx = 1;	/* �ͼθ������Ѵ� */
    else f_idx = 0;					/* �ڤ�ΤƤ��Ѵ� */

    if ( func_dest2d == NULL ) {
        user_ptr_dest2d = (void *)&f_idx;
	func_dest2d = this->func_cnv_x2d_n;
    }

    if ( func_dest2d == NULL ) {
	err_report(__FUNCTION__,"WARNING","invalid size of type; ignored");
	goto quit;
    }

    if ( this->length() == 0 ) goto quit;

    if ( this->fix_section_args( &col_index, &col_size, 
				 &row_index, &row_size,
				 &layer_index, &layer_size ) < 0 ) goto quit;

    ret_len = 0;

    col_blen = this_bytes * this->col_length();
    colrow_blen = col_blen * this->row_length();

    if ( x_buffer.allocate_aligned32(col_size * row_size, &x_buf_ptr32) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","malloc() failed");
    }
    else {
	size_t j, k;
	const char *k_ptr;
	const char *j_ptr;
	ssize_t ii = col_index;				/* fixed */
	ssize_t jj = row_index;				/* fixed */
	double *x_buf_p;
	k_ptr = (const char *)(this->data_ptr()) 
		+ colrow_blen * layer_index		/* offset */
		+ col_blen * row_index
		+ this_bytes * col_index;
	for ( k=0 ; k < layer_size ; k++ ) {
	    ssize_t kk = layer_index + k;
	    ssize_t n_valid;
	    x_buf_p = x_buf_ptr32;
	    j_ptr = k_ptr;
	    for ( j=0 ; j < row_size ; j++ ) {
		/* */
		(*func_dest2d)((const void *)j_ptr, (void *)x_buf_p,
			       col_size, user_ptr_dest2d);
		x_buf_p += col_size;
		/* */
		j_ptr += col_blen;
	    }
	    n_valid = (*func)(x_buf_ptr32,col_size,row_size,ii,jj,kk,this,user_ptr_func);
	    /* count valid elements */
	    ret_len += n_valid;

	    k_ptr += colrow_blen;
	}
    }

 quit:
    return ret_len;
}

/**
 * ���Ȥ�����λ��ꤵ�줿�ϰϤ����Ǥ�xy��ñ�̤ǥ�����󤷤ޤ���z�ΰ���1�Ĥ�
 * �Ĥ�1�󡤥桼������ؿ� func ���ƤӽФ��졤func �ϰ�����Ϳ����줿2����
 * �ǡ���(1�����ܤ�x������2�����ܤ�y����)�ˤĤ��Ʋ��餫�ν�����Ԥʤ������Ǥ���
 * ��������2�����ǡ����ϥ饤�֥�꤬��������ƥ�ݥ��Хåե��Ǥ��ꡤ���֥���
 * ���Ȥ������������Ǥ��äƤ� double �����󶡤���ޤ���<br>
 * �桼������ؿ� func �ΰ����ˤϽ�ˡ����Ȥ������ͤ�xy�̤ˤ���������x������
 * ����������θĿ���y�����ˤ���������θĿ�������֡��԰��֡��쥤����֡�����
 * �Υ��֥������ȤΥ��ɥ쥹���桼���ݥ��󥿤��ͤ�Ϳ�����ޤ������δؿ��ϡ�ͭ��
 * �����Ǥο����֤��褦�ˤ��ޤ���<br>
 * �����ǡ���1��� median ��Ȥ���ʤɤ˻Ȥ����дؿ��Ǥ���
 *
 * @param     func ���ǥ������ΰ٤Υ桼���ؿ��Υ��ɥ쥹
 * @param     user_ptr func �κǸ��Ϳ������桼���Υݥ���
 * @param     col_index �����
 * @param     col_size �󥵥���
 * @param     row_index �԰���
 * @param     row_size �ԥ�����
 * @param     layer_index �쥤�����
 * @param     layer_size �쥤�䥵����
 * @return    �������ˤ��ͭ�����Ǥο������顼�ξ������͡�
 */
ssize_t mdarray::scan_xy_planes( 
	ssize_t (*func)(double [],size_t,size_t,ssize_t,ssize_t,ssize_t,const mdarray *,void *),
	void *user_ptr,
	ssize_t col_index, size_t col_size,
	ssize_t row_index, size_t row_size,
	ssize_t layer_index, size_t layer_size ) const
{
    return this->scan_xy_planes(NULL,NULL,func,user_ptr,
	       col_index,col_size, row_index,row_size, layer_index,layer_size);
}


/**
 * ���Ȥ�����λ��ꤵ�줿�ϰϤ����Ǥ�zx��ñ�̤ǥ�����󤷤ޤ���y�ΰ���1�Ĥ�
 * �Ĥ�1�󡤥桼������ؿ� func ���ƤӽФ��졤func �ϰ�����Ϳ����줿2����
 * �ǡ���(1�����ܤ�z������2�����ܤ�x����)�ˤĤ��Ʋ��餫�ν�����Ԥʤ������Ǥ���
 * ��������2�����ǡ����ϥ饤�֥�꤬��������ƥ�ݥ��Хåե��Ǥ��ꡤ���֥���
 * ���Ȥ������������Ǥ��äƤ� double �����󶡤���ޤ���<br>
 * �桼������ؿ� func �ΰ����ˤϽ�ˡ����Ȥ������ͤ�zx�̤ˤ���������z������
 * ����������θĿ���x�����ˤ���������θĿ�������֡��԰��֡��쥤����֡�����
 * �Υ��֥������ȤΥ��ɥ쥹���桼���ݥ��󥿤��ͤ�Ϳ�����ޤ������δؿ��ϡ�ͭ��
 * �����Ǥο����֤��褦�ˤ��ޤ���<br>
 * ʣ���β����ǡ����� median ��Ȥ���ʤɤ˻Ȥ����дؿ��Ǥ���
 *
 * @param     func_dest2d ���Ȥ����ǡ�����double �����Ѵ����뤿��Υ桼���ؿ��Υ��ɥ쥹
 * @param     user_ptr_dest2d func dest2d �κǸ��Ϳ������桼���Υݥ���
 * @param     func ���ǥ������ΰ٤Υ桼���ؿ��Υ��ɥ쥹
 * @param     user_ptr func �κǸ��Ϳ������桼���Υݥ���
 * @param     col_index �����
 * @param     col_size �󥵥���
 * @param     row_index �԰���
 * @param     row_size �ԥ�����
 * @param     layer_index �쥤�����
 * @param     layer_size �쥤�䥵����
 * @return    �������ˤ��ͭ�����Ǥο������顼�ξ������͡�
 */
ssize_t mdarray::scan_zx_planes( 
	void (*func_dest2d)(const void *,void *,size_t,size_t,void *),
	void *user_ptr_dest2d,
	ssize_t (*func)(double [],size_t,size_t,ssize_t,ssize_t,ssize_t,const mdarray *,void *),
        void *user_ptr_func,
	ssize_t col_index, size_t col_size,
	ssize_t row_index, size_t row_size,
	ssize_t layer_index, size_t layer_size ) const
{
    const size_t this_bytes = this->bytes();
    ssize_t ret_len = -1;
    heap_mem<double> z_buffer;	/* buffer for 1st arg of user's function */
    double *z_buf_ptr32;	/* 32-byte aligned pointer               */
    size_t j, k;
    size_t col_blen, colrow_blen;
    const char *k_ptr;
    const char *j_ptr;
    size_t f_idx;	/* ��ư�����������������Ѵ�������ˤɤ����뤫 */

    if ( this->rounding_rec == true ) f_idx = 1;	/* �ͼθ������Ѵ� */
    else f_idx = 0;					/* �ڤ�ΤƤ��Ѵ� */

    if ( func_dest2d == NULL ) {
      user_ptr_dest2d = (void *)&f_idx;
      /* ���Ѵ��Τ���δؿ�������(����obj�η���double) */
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
      if ( this->sz_type_rec == org_sz_type && DOUBLE_ZT == new_sz_type ) { \
        func_dest2d = &cnv_n_dstep::fncname; \
      }
      SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(SEL_FUNC,,,,,,,,,,,,,,,,,,,,,,,,,,,,else);
#undef SEL_FUNC
    }

    if ( func_dest2d == NULL ) {
	err_report(__FUNCTION__,"WARNING","invalid size of type; ignored");
	goto quit;
    }

    if ( this->length() == 0 ) goto quit;

    if ( this->fix_section_args( &col_index, &col_size, 
				 &row_index, &row_size,
				 &layer_index, &layer_size ) < 0 ) goto quit;

    if ( layer_size == 0 ) goto quit;

    ret_len = 0;

    col_blen = this_bytes * this->col_length();
    colrow_blen = col_blen * this->row_length();

    if (z_buffer.allocate_aligned32(col_size * layer_size, &z_buf_ptr32) < 0) {
	err_throw(__FUNCTION__,"FATAL","malloc() failed");
    }
    else {
	ssize_t n_valid = 0;
	ssize_t kk = layer_index;			/* fixed */
	ssize_t ii = col_index;				/* fixed */
	k_ptr = (const char *)(this->data_ptr()) 
		+ colrow_blen * layer_index		/* offset */
		+ col_blen * row_index
		+ this_bytes * col_index;
	for ( j=0 ; j < row_size ; j++ ) {
	    ssize_t jj = row_index + j;
	    j_ptr = k_ptr;
	    for ( k=0 ; k < layer_size ; k++ ) {
		(*func_dest2d)((const void *)j_ptr, (void *)(z_buf_ptr32 + k),
			       col_size, layer_size /* step for dest */,
			       user_ptr_dest2d);
		j_ptr += colrow_blen;
	    }
	    n_valid += (*func)(z_buf_ptr32,layer_size,col_size,ii,jj,kk,this,user_ptr_func);
	    /* */
	    k_ptr += col_blen;
	}
	ret_len = n_valid;
    }

 quit:
    return ret_len;
}

/**
 * ���Ȥ�����λ��ꤵ�줿�ϰϤ����Ǥ�zx��ñ�̤ǥ�����󤷤ޤ���y�ΰ���1�Ĥ�
 * �Ĥ�1�󡤥桼������ؿ� func ���ƤӽФ��졤func �ϰ�����Ϳ����줿2����
 * �ǡ���(1�����ܤ�z������2�����ܤ�x����)�ˤĤ��Ʋ��餫�ν�����Ԥʤ������Ǥ���
 * ��������2�����ǡ����ϥ饤�֥�꤬��������ƥ�ݥ��Хåե��Ǥ��ꡤ���֥���
 * ���Ȥ������������Ǥ��äƤ� double �����󶡤���ޤ���<br>
 * �桼������ؿ� func �ΰ����ˤϽ�ˡ����Ȥ������ͤ�zx�̤ˤ���������z������
 * ����������θĿ���x�����ˤ���������θĿ�������֡��԰��֡��쥤����֡�����
 * �Υ��֥������ȤΥ��ɥ쥹���桼���ݥ��󥿤��ͤ�Ϳ�����ޤ������δؿ��ϡ�ͭ��
 * �����Ǥο����֤��褦�ˤ��ޤ���<br>
 * ʣ���β����ǡ����� median ��Ȥ���ʤɤ˻Ȥ����дؿ��Ǥ���
 *
 * @param     func ���ǥ������ΰ٤Υ桼���ؿ��Υ��ɥ쥹
 * @param     user_ptr func �κǸ��Ϳ������桼���Υݥ���
 * @param     col_index �����
 * @param     col_size �󥵥���
 * @param     row_index �԰���
 * @param     row_size �ԥ�����
 * @param     layer_index �쥤�����
 * @param     layer_size �쥤�䥵����
 * @return    �������ˤ��ͭ�����Ǥο������顼�ξ������͡�
 */
ssize_t mdarray::scan_zx_planes( 
	ssize_t (*func)(double [],size_t,size_t,ssize_t,ssize_t,ssize_t,const mdarray *,void *),
	void *user_ptr,
	ssize_t col_index, size_t col_size,
	ssize_t row_index, size_t row_size,
	ssize_t layer_index, size_t layer_size ) const
{
    return this->scan_zx_planes(NULL,NULL,func,user_ptr,
	       col_index,col_size, row_index,row_size, layer_index,layer_size);
}


/**
 * ���Ȥ�����λ��ꤵ�줿3�������ϰϤ����Ǥ�1�٤˥�����󤷤ޤ����桼�������
 * �� func ��1������ƤӽФ��졤func �ϰ�����Ϳ����줿3�����ǡ����ˤĤ��Ʋ���
 * ���ν�����Ԥʤ������Ǥ��ޤ�������3�����ǡ����ϥ饤�֥�꤬��������ƥ�ݥ�
 * ��Хåե��Ǥ��ꡤ���֥������Ȥ������������Ǥ��äƤ� double �����󶡤����
 * ����<br>
 * �桼������ؿ� func �ΰ����ˤϽ�ˡ����Ȥλ��ꤵ�줿�ϰϤˤ����������ͤ�
 * ����x�����ˤ���������θĿ���y�����ˤ���������θĿ���z�����ˤ����������
 * �Ŀ�������֡��԰��֡��쥤����֡����ȤΥ��֥������ȤΥ��ɥ쥹���桼���ݥ���
 * �����ͤ�Ϳ�����ޤ������δؿ��ϡ�ͭ�������Ǥο����֤��褦�ˤ��ޤ���<br>
 * ʣ���β����ǡ����� median ��Ȥ���ʤɤ˻Ȥ����дؿ��Ǥ���
 *
 * @param     func_dest2d ���Ȥ����ǡ�����double �����Ѵ����뤿��Υ桼���ؿ��Υ��ɥ쥹
 * @param     user_ptr_dest2d func dest2d �κǸ��Ϳ������桼���Υݥ���
 * @param     func ���ǥ������ΰ٤Υ桼���ؿ��Υ��ɥ쥹
 * @param     user_ptr func �κǸ��Ϳ������桼���Υݥ���
 * @param     col_index �����
 * @param     col_size �󥵥���
 * @param     row_index �԰���
 * @param     row_size �ԥ�����
 * @param     layer_index �쥤�����
 * @param     layer_size �쥤�䥵����
 * @return    �������ˤ��ͭ�����Ǥο������顼�ξ������͡�
 */
ssize_t mdarray::scan_a_cube( 
	void (*func_dest2d)(const void *,void *,size_t,void *),
	void *user_ptr_dest2d,
	ssize_t (*func)(double [],size_t,size_t,size_t,ssize_t,ssize_t,ssize_t,const mdarray *,void *),
        void *user_ptr_func,
	ssize_t col_index, size_t col_size,
	ssize_t row_index, size_t row_size,
	ssize_t layer_index, size_t layer_size ) const
{
    const size_t this_bytes = this->bytes();
    ssize_t ret_len = -1;
    size_t col_blen, colrow_blen;
    size_t f_idx;	/* ��ư�����������������Ѵ�������ˤɤ����뤫 */

    heap_mem<double> x_buffer;	/* buffer for 1st arg of user's function */
    double *x_buf_ptr32;	/* 32-byte aligned pointer               */

    if ( this->rounding_rec == true ) f_idx = 1;	/* �ͼθ������Ѵ� */
    else f_idx = 0;					/* �ڤ�ΤƤ��Ѵ� */

    if ( func_dest2d == NULL ) {
        user_ptr_dest2d = (void *)&f_idx;		/* �ºݤϻ��Ѥ��줺 */
	func_dest2d = this->func_cnv_x2d_n;
    }

    if ( func_dest2d == NULL ) {
	err_report(__FUNCTION__,"WARNING","invalid size of type; ignored");
	goto quit;
    }

    if ( this->length() == 0 ) goto quit;

    if ( this->fix_section_args( &col_index, &col_size, 
				 &row_index, &row_size,
				 &layer_index, &layer_size ) < 0 ) goto quit;

    ret_len = 0;

    col_blen = this_bytes * this->col_length();
    colrow_blen = col_blen * this->row_length();

    if ( x_buffer.allocate_aligned32(col_size * row_size * layer_size,
				     &x_buf_ptr32) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","malloc() failed");
    }
    else {
	size_t j, k;
	const char *k_ptr;
	const char *j_ptr;
	ssize_t ii = col_index;				/* fixed */
	ssize_t jj = row_index;				/* fixed */
	ssize_t kk = layer_index;			/* fixed */
	double *x_buf_p;
	ssize_t n_valid;
	k_ptr = (const char *)(this->data_ptr()) 
		+ colrow_blen * layer_index		/* offset */
		+ col_blen * row_index
		+ this_bytes * col_index;
	x_buf_p = x_buf_ptr32;
	for ( k=0 ; k < layer_size ; k++ ) {
	    j_ptr = k_ptr;
	    for ( j=0 ; j < row_size ; j++ ) {
		/* */
		(*func_dest2d)((const void *)j_ptr, (void *)x_buf_p,
			       col_size, user_ptr_dest2d);
		x_buf_p += col_size;
		/* */
		j_ptr += col_blen;
	    }
	    k_ptr += colrow_blen;
	}
	n_valid = (*func)(x_buf_ptr32,col_size,row_size,layer_size,ii,jj,kk,this,user_ptr_func);
	/* count valid elements */
	ret_len += n_valid;
    }

 quit:
    return ret_len;
}

/**
 * ���Ȥ�����λ��ꤵ�줿3�������ϰϤ����Ǥ�1�٤˥�����󤷤ޤ����桼�������
 * �� func ��1������ƤӽФ��졤func �ϰ�����Ϳ����줿3�����ǡ����ˤĤ��Ʋ���
 * ���ν�����Ԥʤ������Ǥ��ޤ�������3�����ǡ����ϥ饤�֥�꤬��������ƥ�ݥ�
 * ��Хåե��Ǥ��ꡤ���֥������Ȥ������������Ǥ��äƤ� double �����󶡤����
 * ����<br>
 * �桼������ؿ� func �ΰ����ˤϽ�ˡ����Ȥλ��ꤵ�줿�ϰϤˤ����������ͤ�
 * ����x�����ˤ���������θĿ���y�����ˤ���������θĿ���z�����ˤ����������
 * �Ŀ�������֡��԰��֡��쥤����֡����ȤΥ��֥������ȤΥ��ɥ쥹���桼���ݥ���
 * �����ͤ�Ϳ�����ޤ������δؿ��ϡ�ͭ�������Ǥο����֤��褦�ˤ��ޤ���<br>
 * ʣ���β����ǡ����� median ��Ȥ���ʤɤ˻Ȥ����дؿ��Ǥ���
 *
 * @param     func ���ǥ������ΰ٤Υ桼���ؿ��Υ��ɥ쥹
 * @param     user_ptr func �κǸ��Ϳ������桼���Υݥ���
 * @param     col_index �����
 * @param     col_size �󥵥���
 * @param     row_index �԰���
 * @param     row_size �ԥ�����
 * @param     layer_index �쥤�����
 * @param     layer_size �쥤�䥵����
 * @return    �������ˤ��ͭ�����Ǥο������顼�ξ������͡�
 */
ssize_t mdarray::scan_a_cube( 
	ssize_t (*func)(double [],size_t,size_t,size_t,ssize_t,ssize_t,ssize_t,const mdarray *,void *),
	void *user_ptr,
	ssize_t col_index, size_t col_size,
	ssize_t row_index, size_t row_size,
	ssize_t layer_index, size_t layer_size ) const
{
    return this->scan_a_cube(NULL,NULL,func,user_ptr,
	       col_index,col_size, row_index,row_size, layer_index,layer_size);
}
