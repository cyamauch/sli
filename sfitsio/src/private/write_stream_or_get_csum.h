#include "fitsio_csum.h"
static ssize_t write_stream_or_get_csum( const void *buf, size_t len_buf,
				   cstreamio *sptr, fitsio_csum *ret_suminfo );
