#include <string.h>

/**  
 * 任意のバイト長のバイトパターン(アライメント規定なし)の繰り返しでバッファ領域
 * を塗り潰します．
 *
 * @param      dest 出力先のバッファの先頭アドレス
 * @param      src_pattern バイトパターン
 * @param      word_bytes バイトパターンのバイト長
 * @param      len_elements 塗り潰すべきバイトパターンの個数
 * @note       パターンおよび出力先バッファの大きさに応じてそこそこ良い性能が
 *             出るように調整してあります．<br>
 *             テスト環境は GCC 3.3.2 + PentiumMのやや古いラップトップ．<br>
 *             標準ライブラリの wmemset() も参照．<br>
 *             メモ: いろいろと試した結果，32-bit環境では n = 8バイト固定の
 *                   memcpy()のループが最速という結論になった．
 */
inline static void memfill( void *dest, 
	      const void *src_pattern, size_t word_bytes, size_t len_elements )
{
    const size_t op_bytes = 2 * sizeof(size_t);	/* good bytes for memcpy() */
    const unsigned char *src_pat = (const unsigned char *)src_pattern;
    unsigned char *d_ptr = (unsigned char *)dest;
    size_t i, j;

    /*
     * ロジックを単純なコードで表現した場合:
     *
     *	    for ( i=0 ; i < len_elements ; i++ ) {
     *		for ( j=0 ; j < word_bytes ; j++ ) {
     *		    d_ptr[j] = src_pat[j];
     *		}
     *		d_ptr += word_bytes;
     *	    }
     */

    /* BLOCK-A と BLOCK-B との分岐条件 */
    const size_t len_blk = 4 * op_bytes;	/* ← この 4 は実測で調整 */

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
		while ( op_bytes <= j ) {
		    j -= op_bytes;
		    memcpy(d_ptr, s_ptr, op_bytes);
		    s_ptr += op_bytes;  d_ptr += op_bytes;
		}
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
	    while ( op_bytes <= j ) {
		j -= op_bytes;
		memcpy(d_ptr, s_ptr, op_bytes);
		s_ptr += op_bytes;  d_ptr += op_bytes;
	    }
	    while ( 0 < j ) {
		j--;
		*d_ptr = *s_ptr;
		s_ptr ++;  d_ptr ++;
	    }
	}
    }

    return;
}
