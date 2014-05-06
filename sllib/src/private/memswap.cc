#include <string.h>

/**  
 * 2�ĤΥХåե�������(���饤���ȵ���ʤ�)�������ؤ��ޤ���
 *
 * @param      buf0 1�Ĥ���������Ƭ���ɥ쥹
 * @param      buf1 2�Ĥ���������Ƭ���ɥ쥹
 * @param      n ����ΥХ���Ĺ
 * @note       memcpy() �������Τ�®��ư���ޤ���<br>
 *             �ƥ��ȴĶ��� GCC 3.3.2 + PentiumM�Τ��Ť���åץȥåס�
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
