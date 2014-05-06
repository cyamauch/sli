


    /* horizontally scans the specified section using a user-defined        */
    /* function.  a temporary buffer of 1-D array is prepared.              */
    /* scan order is displayed in pseudocode:                               */
    /*  for(...) {      <- layer                                            */
    /*    for(...) {    <- row                                              */
    /*      for(...) {  <- column                                           */
    /* arguments of func are                                                */
    /*   1. prepared temporary buffer                                       */
    /*   2. length of buffer of 1. (i.e., length of columns to be scanned)  */
    /*   3. position of x (always first position)                           */
    /*   4. position of y                                                   */
    /*   5. position of z                                                   */
    /*   6. address of self                                                 */
    /*   7. user pointer                                                    */
    virtual ssize_t scan_along_x( 
	void (*func_dest2d)(const void *,void *,size_t,void *), 
	void *user_ptr_dest2d,
	ssize_t (*func)(double [],size_t, ssize_t,ssize_t,ssize_t,const mdarray *,void *),
        void *user_ptr_func,
	ssize_t col_index = 0, size_t col_size = MDARRAY_ALL,
	ssize_t row_index = 0, size_t row_size = MDARRAY_ALL,
	ssize_t layer_index = 0, size_t layer_size = MDARRAY_ALL ) const;
    virtual ssize_t scan_along_x(
	ssize_t (*func)(double[],size_t,ssize_t,ssize_t,ssize_t,const mdarray *,void *),
	void *user_ptr,
	ssize_t col_index = 0, size_t col_size = MDARRAY_ALL,
	ssize_t row_index = 0, size_t row_size = MDARRAY_ALL,
	ssize_t layer_index = 0, size_t layer_size = MDARRAY_ALL ) const;

    /* vertically scans the specified section using a user-defined function. */
    /* a temporary buffer of 1-D array is prepared.                          */
    /* scan order is displayed by pseudocode:                                */
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
    virtual ssize_t scan_along_y( 
	void (*func_dest2d)(const void *,void *,size_t,size_t,void *), 
	void *user_ptr_dest2d,
	ssize_t (*func)(double [],size_t, ssize_t,ssize_t,ssize_t,const mdarray *,void *),
        void *user_ptr_func,
	ssize_t col_index = 0, size_t col_size = MDARRAY_ALL,
	ssize_t row_index = 0, size_t row_size = MDARRAY_ALL,
	ssize_t layer_index = 0, size_t layer_size = MDARRAY_ALL ) const;
    virtual ssize_t scan_along_y(
	ssize_t (*func)(double[],size_t,ssize_t,ssize_t,ssize_t,const mdarray *,void *),
	void *user_ptr,
	ssize_t col_index = 0, size_t col_size = MDARRAY_ALL,
	ssize_t row_index = 0, size_t row_size = MDARRAY_ALL,
	ssize_t layer_index = 0, size_t layer_size = MDARRAY_ALL ) const;

    /* scan the specified section along z-axis using a user-defined       */
    /* function.  a temporary buffer of 1-D array is prepared.            */
    /* scan order is displayed by pseudocode:                             */
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
    virtual ssize_t scan_along_z( 
	void (*func_dest2d)(const void *,void *,size_t,size_t,void *),
	void *user_ptr_dest2d,
	ssize_t (*func)(double [],size_t,ssize_t,ssize_t,ssize_t,const mdarray *,void *),
        void *user_ptr_func,
	ssize_t col_index = 0, size_t col_size = MDARRAY_ALL,
	ssize_t row_index = 0, size_t row_size = MDARRAY_ALL,
	ssize_t layer_index = 0, size_t layer_size = MDARRAY_ALL ) const;
    virtual ssize_t scan_along_z(
	ssize_t (*func)(double [],size_t,ssize_t,ssize_t,ssize_t,const mdarray *,void *),
	void *user_ptr,
	ssize_t col_index = 0, size_t col_size = MDARRAY_ALL,
	ssize_t row_index = 0, size_t row_size = MDARRAY_ALL,
	ssize_t layer_index = 0, size_t layer_size = MDARRAY_ALL ) const;

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
    virtual ssize_t scan_xy_planes(
	void (*func_dest2d)(const void *,void *,size_t,void *),
	void *user_ptr_dest2d,
	ssize_t (*func)(double [],size_t,size_t,ssize_t,ssize_t,ssize_t,const mdarray *,void *),
        void *user_ptr_func,
	ssize_t col_index = 0, size_t col_size = MDARRAY_ALL,
	ssize_t row_index = 0, size_t row_size = MDARRAY_ALL,
	ssize_t layer_index = 0, size_t layer_size = MDARRAY_ALL ) const;
    virtual ssize_t scan_xy_planes(
	ssize_t (*func)(double [],size_t,size_t,ssize_t,ssize_t,ssize_t,const mdarray *,void *),
	void *user_ptr,
	ssize_t col_index = 0, size_t col_size = MDARRAY_ALL,
	ssize_t row_index = 0, size_t row_size = MDARRAY_ALL,
	ssize_t layer_index = 0, size_t layer_size = MDARRAY_ALL ) const;

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
    virtual ssize_t scan_zx_planes( 
	void (*func_dest2d)(const void *,void *,size_t,size_t,void *),
	void *user_ptr_dest2d,
	ssize_t (*func)(double [],size_t,size_t,ssize_t,ssize_t,ssize_t,const mdarray *,void *),
        void *user_ptr_func,
	ssize_t col_index = 0, size_t col_size = MDARRAY_ALL,
	ssize_t row_index = 0, size_t row_size = MDARRAY_ALL,
	ssize_t layer_index = 0, size_t layer_size = MDARRAY_ALL ) const;
    virtual ssize_t scan_zx_planes(
	ssize_t (*func)(double [],size_t,size_t,ssize_t,ssize_t,ssize_t,const mdarray *,void *),
	void *user_ptr,
	ssize_t col_index = 0, size_t col_size = MDARRAY_ALL,
	ssize_t row_index = 0, size_t row_size = MDARRAY_ALL,
	ssize_t layer_index = 0, size_t layer_size = MDARRAY_ALL ) const;

    /* scan the specified 3-D section using a user-defined function.         */
    /* a temporary buffer of 3-D array is prepared.                          */
    /* scan order is displayed by pseudocode:                                */
    /*  for(...) {      <- layer                                             */
    /*    for(...) {    <- row                                               */
    /*      for(...) {  <- column                                            */
    /* arguments of func are                                                 */
    /*   1. prepared temporary buffer whose length is n_x * n_y * n_z        */
    /*   2. x length of buffer of 1. (i.e., length of x to be scanned)       */
    /*   3. y length of buffer of 1. (i.e., length of y to be scanned)       */
    /*   4. z length of buffer of 1. (i.e., length of z to be scanned)       */
    /*   5. position of x (always first position)                            */
    /*   6. position of y (always first position)                            */
    /*   7. position of z (always first position)                            */
    /*   8. address of self                                                  */
    /*   9. user pointer                                                     */
    virtual ssize_t scan_a_cube(
	void (*func_dest2d)(const void *,void *,size_t,void *),
	void *user_ptr_dest2d,
	ssize_t (*func)(double [],size_t,size_t,size_t,ssize_t,ssize_t,ssize_t,const mdarray *,void *),
        void *user_ptr_func,
	ssize_t col_index = 0, size_t col_size = MDARRAY_ALL,
	ssize_t row_index = 0, size_t row_size = MDARRAY_ALL,
	ssize_t layer_index = 0, size_t layer_size = MDARRAY_ALL ) const;
    virtual ssize_t scan_a_cube(
	ssize_t (*func)(double [],size_t,size_t,size_t,ssize_t,ssize_t,ssize_t,const mdarray *,void *),
	void *user_ptr,
	ssize_t col_index = 0, size_t col_size = MDARRAY_ALL,
	ssize_t row_index = 0, size_t row_size = MDARRAY_ALL,
	ssize_t layer_index = 0, size_t layer_size = MDARRAY_ALL ) const;

