
    ssize_t image_copy_and_cut( mdarray *dest_obj,
				ssize_t col_index, size_t col_size, 
				ssize_t row_index, size_t row_size,
				ssize_t layer_index, size_t layer_size );
    mdarray &image_vcalcf_scalar( double value, int ope,
				  const char *exp_fmt, va_list ap );
    mdarray &image_calc_scalar( double value, int ope,
				ssize_t col_index, size_t col_size,
				ssize_t row_index, size_t row_size,
				ssize_t layer_index, size_t layer_size );

