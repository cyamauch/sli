/**  
 * 任意のバイト長の配列(アライメント規定なし)のバイトオーダを変換しながら別の
 * バッファに配列をコピーします．
 *
 * @param      buf_in 読み取る配列の先頭アドレス
 * @param      sz_bytes 1要素のバイト長
 * @param      len 配列の長さ
 * @param      buf_out 出力先のバッファの先頭アドレス
 * @note       バイト長が2,4,8バイトの場合は特別なコードで動作し，特に高速
 *             です．その他のバイト長では汎用コードで動作しますが，8バイトを
 *             越える場合ではそこそこ速く動くように調整してあります．<br>
 *             テスト環境は GCC 3.3.2 + PentiumMのやや古いラップトップ．<br>
 *             標準ライブラリの htonl(), htons(), ntohl(), ntohs(), swab()
 *             も参照のこと．
 */
#include <string.h>
#include <unistd.h>

#define USE_UNLOOPED_CODE__CPY_WITH_BYTE_SWAP 1

inline static void cpy_with_byte_swap( const void *buf_in, 
				       size_t sz_bytes, size_t len,
				       void *buf_out )
{
    /*
     * ロジックの単純なコードでの表現:
     *	 const unsigned char *s_ptr = (const unsigned char *)buf_in;
     *	 unsigned char *d_ptr = (unsigned char *)buf_out;
     *   for ( i=0 ; i < len_all ; i++ ) {
     *       for ( j=0 ; j < sz_bytes ; j++ ) {
     *	         d_ptr[j] = s_ptr[sz_bytes-1-j];
     *       }
     *       d_ptr += sz_bytes;
     *       s_ptr += sz_bytes;
     *   }
     */

    /* fast vesion for 2,4 and 8 bytes */
    if ( sz_bytes == 2 ) {
	/* アライメントがOKな場合は swab() を使う                          */
	/* swab() や htons()等はバイト境界が揃っていないと遅くなる事がある */
	if ( ((size_t)(buf_in) & 1) == 0 && ((size_t)(buf_out) & 1) == 0 ) {
	    /* Linux の swab() はけっこう速い */
	    swab(buf_in, buf_out, 2 * len);
	    /*
	     * 実験では swab() より次のコードが速かったが，パフォーマンスは
	     * htons() の実装に依存するので採用は見送り．
	     *  
	     *  uint16_t *d_ptr_s = (uint16_t *)d_ptr;
	     *  const uint16_t *s_ptr_s = (const uint16_t *)s_ptr;
	     *  for ( i=0 ; i < len_all ; i++ ) {
	     *	    *d_ptr_s = htons(*s_ptr_s);
	     *	    d_ptr_s ++;  s_ptr_s ++;
	     *  }
	     */
	}
	else {
	    /* swab() より1割くらい遅いがアライメント関係なしに動くコード */
	    /* (もちろん，アライメントが一致してない場合はもっと遅くなる) */
	    register const unsigned char *s_ptr = (const unsigned char *)buf_in;
	    register unsigned char *d_ptr = (unsigned char *)buf_out;
	    register size_t i;
	    size_t len_all = len;
#ifdef USE_UNLOOPED_CODE__CPY_WITH_BYTE_SWAP
	    /* 256 は loop_cnt の計算が 8-bit シフトでいける事から決定 */
	    if ( 256 <= len_all ) {
		const size_t loop_cnt = len_all >> 8;
		register unsigned char tmp;
		size_t j;
		for ( j=0 ; j < loop_cnt ; j++ ) {
		  /* 256 elements */
		  for ( i=0 ; i < 32 ; i++ ) {
		    /* ブロック長は 8,16,32 と試したが，16が最も良いようだ */
		    memcpy(d_ptr, s_ptr, 2 * 8);  /* memcpy() の威力を借りる */
		    s_ptr += 2 * 8;
		    tmp = d_ptr[0];  d_ptr[0] = d_ptr[1];  d_ptr[1] = tmp;
		    tmp = d_ptr[2];  d_ptr[2] = d_ptr[3];  d_ptr[3] = tmp;
		    tmp = d_ptr[4];  d_ptr[4] = d_ptr[5];  d_ptr[5] = tmp;
		    tmp = d_ptr[6];  d_ptr[6] = d_ptr[7];  d_ptr[7] = tmp;
		    tmp = d_ptr[8];  d_ptr[8] = d_ptr[9];  d_ptr[9] = tmp;
		    tmp = d_ptr[10];  d_ptr[10] = d_ptr[11];  d_ptr[11] = tmp;
		    tmp = d_ptr[12];  d_ptr[12] = d_ptr[13];  d_ptr[13] = tmp;
		    tmp = d_ptr[14];  d_ptr[14] = d_ptr[15];  d_ptr[15] = tmp;
		    d_ptr += 2 * 8;
		  }
		}
		len_all &= 0x00ff;
	    }
#endif
	    for ( i=0 ; i < len_all ; i++ ) {
		d_ptr[0] = s_ptr[1];  d_ptr[1] = s_ptr[0];
		d_ptr += 2;  s_ptr += 2;
	    }
	}
    }
    else if ( sz_bytes == 4 ) {
       /*
        * このブロックのコードは，htonl() を使った↓のコード
        * {
        *     uint32_t *d_ptr_l = (uint32_t *)d_ptr;
        *     const uint32_t *s_ptr_l = (const uint32_t *)s_ptr;
        *     for ( i=0 ; i < len_all ; i++ ) {
        *         *d_ptr_l = htonl(*s_ptr_l);
        *         d_ptr_l ++;  s_ptr_l ++;
        *     }
        * }
        * と同じ性能が出る．
        */
	register const unsigned char *s_ptr = (const unsigned char *)buf_in;
	register unsigned char *d_ptr = (unsigned char *)buf_out;
	register size_t i;
	size_t len_all = len;
#ifdef USE_UNLOOPED_CODE__CPY_WITH_BYTE_SWAP
	if ( 256 <= len_all ) {
	    const size_t loop_cnt = len_all >> 8;
	    register unsigned char tmp;
	    size_t j;
	    for ( j=0 ; j < loop_cnt ; j++ ) {
		/* 256 elements */
                for ( i=0 ; i < 64 ; i++ ) {
		    memcpy(d_ptr, s_ptr, 4 * 4);
		    s_ptr += 4 * 4;
		    tmp = d_ptr[0];  d_ptr[0] = d_ptr[3];  d_ptr[3] = tmp;
		    tmp = d_ptr[1];  d_ptr[1] = d_ptr[2];  d_ptr[2] = tmp;
		    tmp = d_ptr[4];  d_ptr[4] = d_ptr[7];  d_ptr[7] = tmp;
		    tmp = d_ptr[5];  d_ptr[5] = d_ptr[6];  d_ptr[6] = tmp;
		    tmp = d_ptr[8];  d_ptr[8] = d_ptr[11];  d_ptr[11] = tmp;
		    tmp = d_ptr[9];  d_ptr[9] = d_ptr[10];  d_ptr[10] = tmp;
		    tmp = d_ptr[12];  d_ptr[12] = d_ptr[15];  d_ptr[15] = tmp;
		    tmp = d_ptr[13];  d_ptr[13] = d_ptr[14];  d_ptr[14] = tmp;
		    d_ptr += 4 * 4;
                }
	    }
	    len_all &= 0x00ff;
	}
#endif
	for ( i=0 ; i < len_all ; i++ ) {
	    d_ptr[0] = s_ptr[3];
	    d_ptr[1] = s_ptr[2];
	    d_ptr[2] = s_ptr[1];
	    d_ptr[3] = s_ptr[0];
	    d_ptr += 4;  s_ptr += 4;
	}
    }
    else if ( sz_bytes == 8 ) {
	const size_t len_all = len;
	register const unsigned char *s_ptr = (const unsigned char *)buf_in;
	register unsigned char *d_ptr = (unsigned char *)buf_out;
	register size_t i;
	/* 8バイトの場合は memcpy() を使うよりこの方が若干速い */
	/* memcpy() を使うかどうかの境界は 8バイトにあるらしい */
	/* なお，*d_ptr++ は使うとやや遅くなる                 */
	for ( i=0 ; i < len_all ; i++ ) {
	    d_ptr[0] = s_ptr[7];
	    d_ptr[1] = s_ptr[6];
	    d_ptr[2] = s_ptr[5];
	    d_ptr[3] = s_ptr[4];
	    d_ptr[4] = s_ptr[3];
	    d_ptr[5] = s_ptr[2];
	    d_ptr[6] = s_ptr[1];
	    d_ptr[7] = s_ptr[0];
	    d_ptr += 8;  s_ptr += 8;
	}
    }
    /* fallback                                                  */
    /* sz_bytesが小さい場合は遅いが，8バイトを越えると性能が出る */
    else {
	const size_t len_all = len;
	register const unsigned char *s_ptr = (const unsigned char *)buf_in;
	register unsigned char *d_ptr = (unsigned char *)buf_out;
	register const unsigned char *s_ptr_x;
	register size_t i, j;
	for ( i=0 ; i < len_all ; i++ ) {
	    s_ptr += sz_bytes;
	    s_ptr_x = s_ptr;
	    j = sz_bytes;
#ifdef USE_UNLOOPED_CODE__CPY_WITH_BYTE_SWAP
	    while ( 8 <= j ) {
		s_ptr_x -= 8;
		d_ptr[0] = s_ptr_x[7];
		d_ptr[1] = s_ptr_x[6];
		d_ptr[2] = s_ptr_x[5];
		d_ptr[3] = s_ptr_x[4];
		d_ptr[4] = s_ptr_x[3];
		d_ptr[5] = s_ptr_x[2];
		d_ptr[6] = s_ptr_x[1];
		d_ptr[7] = s_ptr_x[0];
		d_ptr += 8;
		j -= 8;
	    }
#endif
	    s_ptr_x -= j;
	    while ( 0 < j ) {
		j--;
		*d_ptr = s_ptr_x[j];
		d_ptr++;
	    }
	}
    }
    return;
}
