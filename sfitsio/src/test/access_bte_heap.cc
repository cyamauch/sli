#include <sli/stdstreamio.h>
#include <sli/fitscc.h>
#include <sli/mdarray_float.h>
using namespace sli;

/*
 * A test code to access variable length array of BTE
 *
 * ./access_bte_heap
 *
 */

#define URL_BTE_WITH_VL "../../fits_sample/binary_table/ae_xi2_20051102.rmf.fits.gz"
//#define URL_BTE_WITH_VL "http://www.ir.isas.jaxa.jp/~cyamauch/sli/ae_xi2_20051102.rmf.fits.gz"
//#define URL_BTE_WITH_VL "http://ftp.ledas.ac.uk/suzaku/nra_info/responses/ae_xi2_20051102.rmf"

int main( int argc, char *argv[] )
{
    int return_status = -1;
    stdstreamio sio;
    fitscc fits;
    long i;

    /* read FITS */
    if ( fits.read_stream(URL_BTE_WITH_VL) < 0 ) {
        sio.eprintf("[ERROR] fits.read_stream() failed\n");
	goto quit;
    }

    /* display HDU info */
    sio.printf("N_HDU:%ld\n", fits.length() );
    for ( i=0 ; i < fits.length() ; i++ ) {
        sio.printf("HDU:%ld = [%s]\n", i, fits.hduname(i) );
    }

    /* check "MATRIX" HDU */
    if ( fits.index("MATRIX") < 0 ) {
        sio.eprintf("[ERROR] not found MATRIX HDU\n");
	goto quit;
    }

    /* search a column using variable length array */
    for ( i=0 ; i < fits.table("MATRIX").col_length() ; i++ ) {
        const fits_table &tbl = fits.table("MATRIX");
        const fits_table_col &col = tbl.col(i);
	const long row_length = tbl.row_length();

	/* NOTE: read FITS::FLOAT_T only */
        if ( col.type() == FITS::LONGARRDESC_T &&
	     col.heap_type() == FITS::FLOAT_T ) {
	    const long heap_bytes = col.heap_bytes();
	    mdarray_float tmp_value;
	    float *tmp_value_ptr;
	    long j;

	    sio.printf("found variable length array: column=%ld\n",i);
	    sio.printf("row length=%ld, ",row_length);
	    sio.printf("elem length=%ld, ",col.elem_length());
	    sio.printf("max array length=%ld\n",col.max_array_length());

	    /* register address of ptr and alloc temp area: length = max */
	    tmp_value.register_extptr(&tmp_value_ptr)
		     .resize(col.max_array_length());

	    /*
	     * read variable length array
	     */

	    /* loop for rows */
	    for ( j=0 ; j < row_length ; j++ ) {
		const long len_arr = col.array_length(j);
		const long heap_off = col.array_heap_offset(j);
	        long k;

	        sio.printf("row:%ld length=%ld :: ", j, len_arr);
#if 1
		/* get values from heap area */
		/* NOTE: heap is big-endian   */
		tbl.get_heap(heap_off, 
			     (void *)tmp_value_ptr, heap_bytes * len_arr);
		/* adjust endian */
		/* NOTE: this code can be applied for all architectures */
		tmp_value.reverse_endian(false, 0, len_arr);
		for ( k=0 ; k < len_arr ; k++ ) {
		    sio.printf("[%g]", tmp_value[k]);
		}
		sio.printf("\n");
#endif

#if 0
		/* for performance test (one by one...) */
		for ( k=0 ; k < len_arr ; k++ ) {
		    tbl.get_heap(heap_off + heap_bytes * k,
		    		 (void *)tmp_value_ptr, heap_bytes);
		    tmp_value.reverse_endian(false, 0, 1);
		    sio.printf("[%g]", tmp_value[0]);
		}
		sio.printf("\n");
#endif
	    }

	}
    }
    
    return_status = 0;
 quit:
    return return_status;
}
