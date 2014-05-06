


    /* horizontally scans the specified section using a user-defined       */
    /* function.  a temporary buffer of 1-D array is prepared.             */
    /* scan order is displayed in pseudocode:                              */
    /*  for(...) {      <- layer                                           */
    /*    for(...) {    <- row                                             */
    /*      for(...) {  <- column                                          */
    /* arguments of func are                                               */
    /*   1. prepared temporary buffer                                      */
    /*   2. length of buffer of 1. (i.e., length of columns to be scanned) */
    /*   3. position of x (always first position)                          */
    /*   4. position of y                                                  */
    /*   5. position of z                                                  */
    /*   6. address of self                                                */
    /*   7. user pointer                                                   */
    virtual long scan_along_x( 
        long (*func)(double [],long, long,long,long,const fits_image *,void *),
		 void *user_ptr,
		 long col_index = 0, long col_size = FITS::ALL, 
		 long row_index = 0, long row_size = FITS::ALL,
		 long layer_index = 0, long layer_size = FITS::ALL ) const;

    /* vertically scans the specified section using a user-defined function. */
    /* a temporary buffer of 1-D array is prepared.                          */
    /* scan order is displayed in pseudocode:                                */
    /*  for(...) {      <- layer                                             */
    /*    for(...) {    <- column                                            */
    /*      for(...) {  <- row                                               */
    /* arguments of func are                                                 */
    /*   1. prepared temporary buffer                                        */
    /*   2. length of buffer of 1. (i.e., length of rows to be scanned)      */
    /*   3. position of x                                                    */
    /*   4. position of y (always first position)                            */
    /*   5. position of z                                                    */
    /*   6. address of self                                                  */
    /*   7. user pointer                                                     */
    virtual long scan_along_y( 
        long (*func)(double [],long, long,long,long,const fits_image *,void *),
		 void *user_ptr,
		 long col_index = 0, long col_size = FITS::ALL, 
		 long row_index = 0, long row_size = FITS::ALL,
		 long layer_index = 0, long layer_size = FITS::ALL ) const;

    /* scan the specified section along z-axis using a user-defined       */
    /* function.  a temporary buffer of 1-D array is prepared.            */
    /* scan order is displayed in pseudocode:                             */
    /*  for(...) {      <- row                                            */
    /*    for(...) {    <- column                                         */
    /*      for(...) {  <- layer                                          */
    /* arguments of func are                                              */
    /*   1. prepared temporary buffer whose length is n_layers            */
    /*   2. length of buffer of 1. (i.e., length of layers to be scanned) */
    /*   3. position of x                                                 */
    /*   4. position of y                                                 */
    /*   5. position of z (always first position)                         */
    /*   6. address of self                                               */
    /*   7. user pointer                                                  */
    virtual long scan_along_z( 
	      long (*func)(double [],long, long,long,long, const fits_image *,void *),
	      void *user_ptr,
	      long col_index = 0, long col_size = FITS::ALL, 
	      long row_index = 0, long row_size = FITS::ALL,
	      long layer_index = 0, long layer_size = FITS::ALL ) const;

    /* scan the specified 3-D section with plane by plane (xy plane) using a */
    /* user-defined function.  a temporary buffer of 2-D array is prepared.  */
    /* scan order is displayed by pseudocode:                                */
    /*  for(...) {      <- layer                                             */
    /*    for(...) {    <- row                                               */
    /*      for(...) {  <- column                                            */
    /* arguments of func are                                                 */
    /*   1. prepared temporary buffer whose length is n_cols * n_rows        */
    /*   2. x length of buffer of 1. (i.e., length of columns to be scanned) */
    /*   3. y length of buffer of 1. (i.e., length of rows to be scanned)    */
    /*   4. position of x (always first position)                            */
    /*   5. position of y (always first position)                            */
    /*   6. position of z                                                    */
    /*   7. address of self                                                  */
    /*   8. user pointer                                                     */
    virtual long scan_xy_planes(
	      long (*func)(double [],long,long, long,long,long, const fits_image *,void *),
	      void *user_ptr,
	      long col_index = 0, long col_size = FITS::ALL, 
	      long row_index = 0, long row_size = FITS::ALL,
	      long layer_index = 0, long layer_size = FITS::ALL ) const;

    /* scan the specified 3-D section with plane by plane (zx plane) using a */
    /* user-defined function.  a temporary buffer of 2-D array is prepared.  */
    /* scan order is displayed by pseudocode:                                */
    /*  for(...) {      <- row                                               */
    /*    for(...) {    <- column                                            */
    /*      for(...) {  <- layer                                             */
    /* arguments of func are                                                 */
    /*   1. prepared temporary buffer whose length is n_layers * n_cols      */
    /*   2. z length of buffer of 1. (i.e., length of layers to be scanned)  */
    /*   3. x length of buffer of 1. (i.e., length of columns to be scanned) */
    /*   4. position of x (always first position)                            */
    /*   5. position of y                                                    */
    /*   6. position of z (always first position)                            */
    /*   7. address of self                                                  */
    /*   8. user pointer                                                     */
    virtual long scan_zx_planes( 
	      long (*func)(double [],long,long, long,long,long, const fits_image *,void *),
	      void *user_ptr,
	      long col_index = 0, long col_size = FITS::ALL, 
	      long row_index = 0, long row_size = FITS::ALL,
	      long layer_index = 0, long layer_size = FITS::ALL ) const;

    /* scan the specified 3-D section using a user-defined function.   */
    /* a temporary buffer of 3-D array is prepared.                    */
    /* scan order is displayed by pseudocode:                          */
    /*  for(...) {      <- layer                                       */
    /*    for(...) {    <- row                                         */
    /*      for(...) {  <- column                                      */
    /* arguments of func are                                           */
    /*   1. prepared temporary buffer whose length is n_x * n_y * n_z  */
    /*   2. x length of buffer of 1. (i.e., length of x to be scanned) */
    /*   3. y length of buffer of 1. (i.e., length of y to be scanned) */
    /*   4. z length of buffer of 1. (i.e., length of z to be scanned) */
    /*   5. position of x (always first position)                      */
    /*   6. position of y (always first position)                      */
    /*   7. position of z                                              */
    /*   8. address of self                                            */
    /*   9. user pointer                                               */
    virtual long scan_a_cube(
	      long (*func)(double [],long,long,long, long,long,long, const fits_image *,void *),
	      void *user_ptr,
	      long col_index = 0, long col_size = FITS::ALL, 
	      long row_index = 0, long row_size = FITS::ALL,
	      long layer_index = 0, long layer_size = FITS::ALL ) const;

    /* get pixel statistics                                                */
    /*   options: results= (npix,mean,stddev,median,min,max,skew,kurtosis) */
    /*            e.g., "results=npix,mean,min,max,median"                 */
    virtual long stat_pixels( double results[], size_t results_buf_len,
			      const char *options,
		     long col_index = 0, long col_size = FITS::ALL, 
		     long row_index = 0, long row_size = FITS::ALL,
		     long layer_index = 0, long layer_size = FITS::ALL ) const;
    /* This stores results into object of fits_header class. */
    virtual long stat_pixels( fits_header *results, const char *options,
		     long col_index = 0, long col_size = FITS::ALL, 
		     long row_index = 0, long row_size = FITS::ALL,
		     long layer_index = 0, long layer_size = FITS::ALL ) const;

    /* combine cols of each (y)                                             */
    /*   options: combine=average (average|median|sum|min|max)              */
    /*            outtype=double  (short|ushort|long|longlong|float|double) */
    /*            e.g., "combine=median outtype=ushort"                     */
    virtual long combine_cols( fits_image *dest_img, const char *options,
		     long col_index = 0, long col_size = FITS::ALL, 
		     long row_index = 0, long row_size = FITS::ALL,
		     long layer_index = 0, long layer_size = FITS::ALL ) const;

    /* combine rows of each (x)                                             */
    /*   options: combine=average (average|median|sum|min|max)              */
    /*            outtype=double  (short|ushort|long|longlong|float|double) */
    /*            e.g., "combine=median outtype=ushort"                     */
    virtual long combine_rows( fits_image *dest_img, const char *options,
		     long col_index = 0, long col_size = FITS::ALL, 
		     long row_index = 0, long row_size = FITS::ALL,
		     long layer_index = 0, long layer_size = FITS::ALL ) const;

    /* combine layers of each (x,y)                                         */
    /*   options: combine=average (average|median|sum|min|max)              */
    /*            outtype=double  (short|ushort|long|longlong|float|double) */
    /*            e.g., "combine=median outtype=ushort"                     */
    virtual long combine_layers( fits_image *dest_img, const char *options,
		     long col_index = 0, long col_size = FITS::ALL, 
		     long row_index = 0, long row_size = FITS::ALL,
		     long layer_index = 0, long layer_size = FITS::ALL ) const;

