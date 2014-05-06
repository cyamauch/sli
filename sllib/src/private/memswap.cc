#include <string.h>

/**  
 * 2つのバッファの内容(アライメント規定なし)を入れ替えます．
 *
 * @param      buf0 1つめの配列の先頭アドレス
 * @param      buf1 2つめの配列の先頭アドレス
 * @param      n 配列のバイト長
 * @note       memcpy() が効くので速く動きます．<br>
 *             テスト環境は GCC 3.3.2 + PentiumMのやや古いラップトップ．
 */
inline static void memswap( void *buf0, void *buf1, size_t n )
{
    const size_t op_bytes = sizeof(size_t) * 2;
    unsigned char *s_ptr = (unsigned char *)buf0;
    unsigned char *d_ptr = (unsigned char *)buf1;
    unsigned char tmpbuf[op_bytes];
    unsigned char tmp;
    size_t i;

    i = n;

    while ( op_bytes <= i ) {
	i -= op_bytes;
	memcpy(tmpbuf,s_ptr,op_bytes);
	memcpy(s_ptr,d_ptr,op_bytes);
	memcpy(d_ptr,tmpbuf,op_bytes);
	s_ptr += op_bytes;  d_ptr += op_bytes;
    }

    while ( 0 < i ) {
	i--;
	tmp = *s_ptr;  *s_ptr = *d_ptr;  *d_ptr = tmp;
	s_ptr ++;  d_ptr ++;
    }

    return;
}
