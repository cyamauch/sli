/**  
 * 任意のバイト長の配列(アライメント規定なし)のバイトオーダを変換します．
 *
 * @param      buf 配列の先頭アドレス
 * @param      sz_bytes 1要素のバイト長
 * @param      len 配列の長さ
 * @note       バイト長が2,4,8バイトの場合は特別なコードで動作し，特に高速
 *             です．その他のバイト長では汎用コードで動作しますが，8バイトを
 *             越える場合ではそこそこ速く動くように調整してあります．<br>
 *             テスト環境は GCC 3.3.2 + PentiumMのやや古いラップトップ．<br>
 *             標準ライブラリの htonl(), htons(), ntohl(), ntohs(), swab()
 *             も参照のこと．
 */

#define USE_UNLOOPED_CODE__BYTE_SWAP 1

inline static void byte_swap( void *buf, size_t sz_bytes, size_t len )
{
    /*
     * ロジックの単純なコードでの表現:
     *	const size_t len_all = len;
     *	const size_t len_each = sz_bytes / 2;
     *	unsigned char *d_ptr1 = d_ptr + sz_bytes - 1;
     *	for ( i=0 ; i < len_all ; i++ ) {
     *	    for ( j=0 ; j < len_each ; j++ ) {
     *		tmp = d_ptr[j];
     *		d_ptr[j] = d_ptr1[-j];
     *		d_ptr1[-j] = tmp;
     *	    }
     *	    d_ptr += sz_bytes;
     *	    d_ptr1 += sz_bytes;
     *	}
     */
    register unsigned char *d_ptr = (unsigned char *)buf;
    register unsigned char tmp;
    register size_t i;
    /* fast vesion for 2,4 and 8 bytes */
    if ( sz_bytes == 2 ) {
	size_t len_all = len;
#ifdef USE_UNLOOPED_CODE__BYTE_SWAP
	/* 256 は loop_cnt の計算が 8-bit シフトでいける事からエイヤで決定 */
	if ( 256 <= len_all ) {
	    const size_t loop_cnt = len_all >> 8;
	    size_t j;
	    for ( j=0 ; j < loop_cnt ; j++ ) {
		/* 256 elements */
		for ( i=0 ; i < 16 ; i++ ) {
		 /*
		  * ここは次のコードをズラズラと書く方法もあるが，ほとんど
		  * パフォーマンスは改善しない．
		  * tmp = *d_ptr; *d_ptr = d_ptr[1]; *(++d_ptr) = tmp; d_ptr++;
		  * ↑このコードではループ展開している場合は若干良くなるが，
		  * ループ展開しない場合は逆に遅くなる．
		  */
		    tmp = d_ptr[0];  d_ptr[0] = d_ptr[1];  d_ptr[1] = tmp;
		    tmp = d_ptr[2];  d_ptr[2] = d_ptr[3];  d_ptr[3] = tmp;
		    tmp = d_ptr[4];  d_ptr[4] = d_ptr[5];  d_ptr[5] = tmp;
		    tmp = d_ptr[6];  d_ptr[6] = d_ptr[7];  d_ptr[7] = tmp;
		    tmp = d_ptr[8];  d_ptr[8] = d_ptr[9];  d_ptr[9] = tmp;
		    tmp = d_ptr[10];  d_ptr[10] = d_ptr[11];  d_ptr[11] = tmp;
		    tmp = d_ptr[12];  d_ptr[12] = d_ptr[13];  d_ptr[13] = tmp;
		    tmp = d_ptr[14];  d_ptr[14] = d_ptr[15];  d_ptr[15] = tmp;
		    d_ptr += 2 * 8;
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
	    tmp = d_ptr[0];
	    d_ptr[0] = d_ptr[1];
	    d_ptr[1] = tmp;
	    d_ptr += 2;
	}
    }
    else if ( sz_bytes == 4 ) {
	size_t len_all = len;
#ifdef USE_UNLOOPED_CODE__BYTE_SWAP
	if ( 256 <= len_all ) {
	    const size_t loop_cnt = len_all >> 8;
	    size_t j;
	    for ( j=0 ; j < loop_cnt ; j++ ) {
		/* 256 elements */
		for ( i=0 ; i < 32 ; i++ ) {
		    tmp = d_ptr[0];  d_ptr[0] = d_ptr[3];  d_ptr[3] = tmp;
		    tmp = d_ptr[1];  d_ptr[1] = d_ptr[2];  d_ptr[2] = tmp;
		    tmp = d_ptr[4];  d_ptr[4] = d_ptr[7];  d_ptr[7] = tmp;
		    tmp = d_ptr[5];  d_ptr[5] = d_ptr[6];  d_ptr[6] = tmp;
		    tmp = d_ptr[8];  d_ptr[8] = d_ptr[11];  d_ptr[11] = tmp;
		    tmp = d_ptr[9];  d_ptr[9] = d_ptr[10];  d_ptr[10] = tmp;
		    tmp = d_ptr[12];  d_ptr[12] = d_ptr[15];  d_ptr[15] = tmp;
		    tmp = d_ptr[13];  d_ptr[13] = d_ptr[14];  d_ptr[14] = tmp;
		    d_ptr += 4 * 4;
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
	    tmp = d_ptr[0];  d_ptr[0] = d_ptr[3];  d_ptr[3] = tmp;
	    tmp = d_ptr[1];  d_ptr[1] = d_ptr[2];  d_ptr[2] = tmp;
	    d_ptr += 4;
	}
    }
    else if ( sz_bytes == 8 ) {
	size_t len_all = len;
#ifdef USE_UNLOOPED_CODE__BYTE_SWAP
	if ( 256 <= len_all ) {
	    const size_t loop_cnt = len_all >> 8;
	    size_t j;
	    for ( j=0 ; j < loop_cnt ; j++ ) {
		/* 256 elements */
		for ( i=0 ; i < 64 ; i++ ) {
		    tmp = d_ptr[0];  d_ptr[0] = d_ptr[7];  d_ptr[7] = tmp;
		    tmp = d_ptr[1];  d_ptr[1] = d_ptr[6];  d_ptr[6] = tmp;
		    tmp = d_ptr[2];  d_ptr[2] = d_ptr[5];  d_ptr[5] = tmp;
		    tmp = d_ptr[3];  d_ptr[3] = d_ptr[4];  d_ptr[4] = tmp;
		    tmp = d_ptr[8];  d_ptr[8] = d_ptr[15];  d_ptr[15] = tmp;
		    tmp = d_ptr[9];  d_ptr[9] = d_ptr[14];  d_ptr[14] = tmp;
		    tmp = d_ptr[10];  d_ptr[10] = d_ptr[13];  d_ptr[13] = tmp;
		    tmp = d_ptr[11];  d_ptr[11] = d_ptr[12];  d_ptr[12] = tmp;
		    d_ptr += 8 * 2;
		    tmp = d_ptr[0];  d_ptr[0] = d_ptr[7];  d_ptr[7] = tmp;
		    tmp = d_ptr[1];  d_ptr[1] = d_ptr[6];  d_ptr[6] = tmp;
		    tmp = d_ptr[2];  d_ptr[2] = d_ptr[5];  d_ptr[5] = tmp;
		    tmp = d_ptr[3];  d_ptr[3] = d_ptr[4];  d_ptr[4] = tmp;
		    tmp = d_ptr[8];  d_ptr[8] = d_ptr[15];  d_ptr[15] = tmp;
		    tmp = d_ptr[9];  d_ptr[9] = d_ptr[14];  d_ptr[14] = tmp;
		    tmp = d_ptr[10];  d_ptr[10] = d_ptr[13];  d_ptr[13] = tmp;
		    tmp = d_ptr[11];  d_ptr[11] = d_ptr[12];  d_ptr[12] = tmp;
		    d_ptr += 8 * 2;
		}
	    }
	    len_all &= 0x00ff;
	}
#endif
	for ( i=0 ; i < len_all ; i++ ) {
	    tmp = d_ptr[0];  d_ptr[0] = d_ptr[7];  d_ptr[7] = tmp;
	    tmp = d_ptr[1];  d_ptr[1] = d_ptr[6];  d_ptr[6] = tmp;
	    tmp = d_ptr[2];  d_ptr[2] = d_ptr[5];  d_ptr[5] = tmp;
	    tmp = d_ptr[3];  d_ptr[3] = d_ptr[4];  d_ptr[4] = tmp;
	    d_ptr += 8;
	}
    }
    /* fallback                                                     */
    /* 実験では sz_bytes=4 の時に遅くなるが，他の場合はそこそこ速い */
    else {
	const size_t len_all = len;
	const size_t len_each = sz_bytes / 2;
	register unsigned char *d_ptr1 = d_ptr;
	register unsigned char *d_ptr_x;
	register size_t j;
	for ( i=0 ; i < len_all ; i++ ) {
	    d_ptr1 += sz_bytes;
	    d_ptr_x = d_ptr1;
	    j = len_each;
#ifdef USE_UNLOOPED_CODE__BYTE_SWAP
	    while ( 4 <= j ) {
		d_ptr_x -= 4;
		tmp = d_ptr[0];  d_ptr[0] = d_ptr_x[3];  d_ptr_x[3] = tmp;
		tmp = d_ptr[1];  d_ptr[1] = d_ptr_x[2];  d_ptr_x[2] = tmp;
		tmp = d_ptr[2];  d_ptr[2] = d_ptr_x[1];  d_ptr_x[1] = tmp;
		tmp = d_ptr[3];  d_ptr[3] = d_ptr_x[0];  d_ptr_x[0] = tmp;
		d_ptr += 4;
		j -= 4;
	    }
#endif
	    d_ptr_x -= j;
	    while ( 0 < j ) {
		j--;
		tmp = *d_ptr;  *d_ptr = d_ptr_x[j];  d_ptr_x[j] = tmp;
		d_ptr++;
	    }
	    d_ptr = d_ptr1;
	}
    }
    return;
}
