
    /* copy a region to another mdarray object and clear */
    virtual mdarray &cut( mdarray *dest, 
			  ssize_t col_idx, size_t col_len=MDARRAY_ALL,
			  ssize_t row_idx=0, size_t row_len=MDARRAY_ALL, 
			  ssize_t layer_idx=0, size_t layer_len=MDARRAY_ALL );
    /* not recommended */
    virtual mdarray &cut( mdarray &dest, 
			  ssize_t col_idx, size_t col_len=MDARRAY_ALL,
			  ssize_t row_idx=0, size_t row_len=MDARRAY_ALL, 
			  ssize_t layer_idx=0, size_t layer_len=MDARRAY_ALL );


    void image_copy_and_cut( fits_image *dest_img,
			     long col_index, long col_size, 
			     long row_index, long row_size,
			     long layer_index, long layer_size );
