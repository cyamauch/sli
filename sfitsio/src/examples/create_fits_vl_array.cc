#include <sli/stdstreamio.h>
#include <sli/fitscc.h>
#include <math.h>
using namespace sli;

/**
 * @file   create_fits_vl_array.cc
 * @brief  可変長配列を含むバイナリテーブル FITS を作る例．低レベル API を使用
 */

/*
 * A sample code to create a FITS binary table with variable length array
 *
 * NOTE: this code uses low-level APIs.
 *
 * ./create_fits_vl_array out_file.fits[.gz or .bz2]
 *
 */

#define get_max(a,b) ((a) < (b) ? (b) : (a))

int main( int argc, char *argv[] )
{
    int return_status = -1;
    stdstreamio sio;					/* standard I/O */
    fitscc fits;					/* FITS object  */
    long i, heap_used_len, max_n_elem;

    /* column definition */
    const fits::table_def tbl_def[] = {
        // TTYPE,comment,TALAS,TELEM,TUNIT,comment,
	//                                         TDISP,    TFORM, TDIM
        {  "VLA",     "",   "",   "",   "",     "",   "", "1PE(0)",   "" },
        { NULL }
    };

    /* Create Binary Table HDU */
    fits.append_table("TEST",0, tbl_def);

    /* prepare aliases to table and table column object */
    fits_table &tbl = fits.table("TEST");
    fits_table_col &tbl_col = fits.table("TEST").col("VLA");

    /* Added checksum */
    tbl.header_append("CHECKSUM");
    tbl.header_append("DATASUM");
    
    /* temporary buffer for data and its ptr */
    float *dat_ptr;
    mdarray_float dat(false,&dat_ptr);

    /* set initial length of each buffer */
    tbl.resize_heap(65536);			/* heap area */
    tbl.resize_rows(1024);			/* main table */
    dat.resize(1024);				/* temporary buffer */

    /*
     * main loop
     */
    heap_used_len = 0;
    max_n_elem = 0;
    for ( i=0 ; i < 3000 ; i++ ) {		/* loop of rows */

        long n_elem, n_bytes, j;

	/* set length of current data array */
	n_elem = i;
	n_bytes = n_elem * sizeof(float);
	if ( max_n_elem < n_elem ) max_n_elem = n_elem;

        /* realloc temporary buffer */
        if ( (long)dat.length() < n_elem ) {
	    dat.resize(get_max((long)dat.length() * 2, n_elem));
        }

	/* store data to temporary buffer */
        for ( j=0 ; j < n_elem ; j++ ) dat_ptr[j] = sqrt(j);
        //for ( j=0 ; j < n_elem ; j++ ) dat[j] = sqrt(j);  /* slightly slow */


	/* realloc heap */
	if ( (long)tbl.heap_length() < heap_used_len + n_bytes ) {
	    tbl.resize_heap(get_max((long)tbl.heap_length() * 2,
				    heap_used_len + n_bytes));
	}
	/* realloc rows */
	if ( tbl.row_length() <= i ) {
	    tbl.resize_rows(tbl.row_length() * 2);
	}

	/* set length and offset to main table */
	tbl_col.assign_arrdesc(n_elem, heap_used_len, i);

	/* adjust endian */
	dat.reverse_endian(false, 0, n_elem);

	/* 注意: 一般的に，2バイト以上の整数，浮動小数点数は，アライメントの */
	/*       問題があるのでヒープバッファのアドレスを取得して直接書き込  */
	/*       んではいけない．                                            */

	/* store dat[...] to heap */
	tbl.put_heap(heap_used_len, dat.data_ptr(), n_bytes);

	/* こちらでエンディアンを調整してもよい */

	/* adjust endian */
	//tbl.reverse_heap_endian(heap_used_len, FITS::FLOAT_T, n_elem);

	/* update heap pos */
	heap_used_len += n_bytes;
    }

    /* fix number of rows */
    tbl.resize_rows(i);


    /*
     * Save to file...
     */
    if ( 1 < argc ) {
	ssize_t sz;
        const char *out_file = argv[1];
	sz = fits.write_stream(out_file);    /* writing file */
	if ( sz < 0 ) {
	    sio.eprintf("[ERROR] obj.write_stream() failed\n");
	    goto quit;
	}
    }

    return_status = 0;
 quit:
    return return_status;
}
