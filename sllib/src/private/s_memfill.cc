#include <string.h>

#include "_test_sse2.h"

#ifdef _SSE2_IS_OK
#include <emmintrin.h>
#include "_tune_sse2.h"
#endif

/**  
 * @brief  SIMD命令(SSE2)を使った高速な領域塗り潰し (nバイトのパターン)
 *
 *  任意のバイト長のバイトパターン(アライメント規定なし)の繰り返しでバッファ
 *  領域を塗り潰します(SSE2対応版)．
 *
 * @param      dest 出力先のバッファの先頭アドレス
 * @param      src_pattern バイトパターン
 * @param      word_bytes バイトパターンのバイト長
 * @param      len_elements 塗り潰すべきバイトパターンの個数
 * @param      total_len_elements 繰り返し呼び出す時のバイトパターンの総数
 * @note       パターンおよび出力先バッファの大きさに応じてそこそこ良い性能が
 *             出るように調整してあります．<br>
 *             テスト環境は GCC 3.3.2 + PentiumMのやや古いラップトップと
 *             GCC-4 + 64-bit Linux．<br>
 *             標準ライブラリの wmemset() も参照．<br>
 *             メモ: いろいろと試した結果，32-bit環境では n = 8バイト固定の
 *                   memcpy()のループが最速という結論になった．
 */
inline static void s_memfill( void *dest, 
	      const void *src_pattern, size_t word_bytes, size_t len_elements,
	      size_t total_len_elements )
{
#ifdef _SSE2_IS_OK
    const size_t op_bytes = 16;			/* for SSE2 */
#else
    const size_t op_bytes = 2 * sizeof(size_t);	/* good bytes for memcpy() */
#endif
    const unsigned char *src_pat = (const unsigned char *)src_pattern;
    unsigned char *d_ptr = (unsigned char *)dest;
    size_t i, j;

    /*
     * ロジックを単純なコードで表現した場合:
     *
     *	    for ( i=0 ; i < len ; i++ ) {
     *		for ( j=0 ; j < word_bytes ; j++ ) {
     *		    d_ptr[j] = src_pat[j];
     *		}
     *		d_ptr += word_bytes;
     *	    }
     */

    /* BLOCK-A と BLOCK-B との分岐条件 */
    const size_t len_blk = 4 * op_bytes;	/* ← この 4 は実測で調整 */

#ifdef _SSE2_IS_OK
    size_t blen_all = word_bytes * len_elements;
    /*                                                        */
    /* BLOCK-S : special code for word_bytes = 2, 4, 8 and 16 */
    /*                                                        */
    if ( _SSE2_MIN_NBYTES <= blen_all && 
	 word_bytes <= 16 && (16 % word_bytes) == 0 ) {
        const size_t nn = 16 / word_bytes;
	const size_t mm = ((size_t)d_ptr & (word_bytes - 1));
	unsigned char src_al[16] __attribute__((aligned(16)));
	size_t ix;
	bool use_mm_stream;
	if ( _SSE2_CPU_CACHE_SIZE <= blen_all || 
	     _SSE2_CPU_CACHE_SIZE <= word_bytes * total_len_elements ) {
	    use_mm_stream = true;
	}
	else use_mm_stream = false;
	/* create shifted pattern */
	ix = mm;
	for ( i=0 ; i < nn ; i++ ) {
	    for ( j=0 ; j < word_bytes ; j++ ) {
		src_al[ix] = src_pat[j];
		ix++;
		if ( 16 <= ix ) ix = 0;
	    }
	}
	__m128i r0 = _mm_load_si128((__m128i *)src_al);
	/* not aligned part */
	j = mm;
	while ( ((size_t)d_ptr & 0x0f) != 0 ) {
	    blen_all --;
	    *d_ptr = src_al[j];
	    d_ptr ++;  j++;
	}
	/* aligned part */
	if ( use_mm_stream == true ) {
	    while ( 16 <= blen_all ) {
		blen_all -= 16;
		/* without polluting the cache */
		_mm_stream_si128((__m128i *)d_ptr, r0);
		d_ptr += 16;
	    }
	}
	else {
	    while ( 16 <= blen_all ) {
		blen_all -= 16;
		_mm_store_si128((__m128i *)d_ptr, r0);
		d_ptr += 16;
	    }
	}
	/* not aligned part */
	j = 0;
	while ( 0 < blen_all ) {
	    blen_all --;
	    *d_ptr = src_al[j];
	    d_ptr ++;  j++;
	}
    }
    else
#endif	/* _SSE2_IS_OK */
    /*                                    */
    /* BLOCK-A : when word_bytes is small */
    /*                                    */
    if ( word_bytes < len_blk ) {
        /* factor_nn の設定はかなり難しい．少なくとも2以上を設定しないと、 */
        /* 例えば word_bytes = 3 の場合にパフォーマンスが出なくなる        */
        const size_t factor_nn = 2;
	const size_t nn = factor_nn * (len_blk / word_bytes);
	const unsigned char *s_ptr0 = src_pat;		/* コピー元アドレス */
	if ( nn < len_elements ) {
	    s_ptr0 = d_ptr;	  /* コピー元アドレスを変更(先頭番地を記録) */
	    /* まず先頭の nn 個を埋める */
	    len_elements -= nn;
	    for ( i=0 ; i < nn ; i++ ) {
		for ( j=0 ; j < word_bytes ; j++ ) d_ptr[j] = src_pat[j];
		d_ptr += word_bytes;
	    }
	    /* 続く部分を nn 個ずつ高速なブロック転送で埋める */
	    while ( nn <= len_elements ) {
		const unsigned char *s_ptr = s_ptr0;
		len_elements -= nn;
		j = nn * word_bytes;
#ifdef _SSE2_IS_OK
		while ( op_bytes <= j ) {
		    j -= op_bytes;
		    __m128i r0 = _mm_loadu_si128((__m128i *)s_ptr);
		    _mm_storeu_si128((__m128i *)d_ptr, r0);
		    s_ptr += op_bytes;  d_ptr += op_bytes;
		}
#else
		while ( op_bytes <= j ) {
		    j -= op_bytes;
		    memcpy(d_ptr, s_ptr, op_bytes);
		    s_ptr += op_bytes;  d_ptr += op_bytes;
		}
#endif	/* _SSE2_IS_OK */
		while ( 0 < j ) {
		    j--;
		    *d_ptr = *s_ptr;
		    s_ptr ++;  d_ptr ++;
		}
	    }
	}
	/* 残りの部分を埋める */
	while ( 0 < len_elements ) {
	    len_elements --;
	    for ( j=0 ; j < word_bytes ; j++ ) d_ptr[j] = s_ptr0[j];
	    d_ptr += word_bytes;
	}
    }
    /*                                    */
    /* BLOCK-B : when word_bytes is large */
    /*                                    */
    else {
	for ( i=0 ; i < len_elements ; i++ ) {
	    const unsigned char *s_ptr = src_pat;
	    j = word_bytes;
#ifdef _SSE2_IS_OK
	    while ( op_bytes <= j ) {
	        j -= op_bytes;
		__m128i r0 = _mm_loadu_si128((__m128i *)s_ptr);
		_mm_storeu_si128((__m128i *)d_ptr, r0);
		s_ptr += op_bytes;  d_ptr += op_bytes;
	    }
#else
	    while ( op_bytes <= j ) {
		j -= op_bytes;
		memcpy(d_ptr, s_ptr, op_bytes);
		s_ptr += op_bytes;  d_ptr += op_bytes;
	    }
#endif	/* _SSE2_IS_OK */
	    while ( 0 < j ) {
		j--;
		*d_ptr = *s_ptr;
		s_ptr ++;  d_ptr ++;
	    }
	}
    }

    return;
}
