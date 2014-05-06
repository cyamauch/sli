#include <string.h>

/**  
 * Ǥ�դΥХ���Ĺ������(���饤���ȵ���ʤ�)�������ȿž�����ޤ���
 *
 * @param      buf �������Ƭ���ɥ쥹
 * @param      word_bytes 1���ǤΥХ���Ĺ
 * @param      len_elements ��������ǤθĿ�
 * @note       �Х���Ĺ��1,2,4,8�Х��Ȥξ������̤ʥ����ɤ�ư�����®�Ǥ���
 *             ����¾�ΥХ���Ĺ�Ǥ����ѥ����ɤ�ư��ޤ������ޥ���Х���Ĺ
 *             �ʾ�Ǥ� memcpy() �������Τ�®��ư���ޤ���<br>
 *             �ƥ��ȴĶ��� GCC 3.3.2 + PentiumM�Τ��Ť���åץȥåס�
 */
inline static void memflip( void *buf, size_t word_bytes, size_t len_elements )
{
    const size_t ma_bytes = sizeof(size_t);
    size_t len_loop = len_elements / 2;
    unsigned char *s_ptr0 = (unsigned char *)(buf);		   /* first  */
    unsigned char *s_ptr1 = s_ptr0 + (word_bytes * len_elements);  /* last+1 */

    if ( word_bytes == 1 ) {
	unsigned char tmp;
	while ( 0 < len_loop ) {
	    len_loop --;
	    s_ptr1 --;
	    tmp = *s_ptr0;  *s_ptr0 = *s_ptr1;  *s_ptr1 = tmp;
	    s_ptr0 ++;
	}
    }
    else if ( word_bytes == 2 ) {
	unsigned char tmpbuf[2];
	while ( 0 < len_loop ) {
	    len_loop --;
	    s_ptr1 -= 2;
	    memcpy(tmpbuf,s_ptr0,2);
	    memcpy(s_ptr0,s_ptr1,2);
	    memcpy(s_ptr1,tmpbuf,2);
	    s_ptr0 += 2;
	}
    }
    else if ( word_bytes == 4 ) {
	unsigned char tmpbuf[4];
	while ( 0 < len_loop ) {
	    len_loop --;
	    s_ptr1 -= 4;
	    memcpy(tmpbuf,s_ptr0,4);
	    memcpy(s_ptr0,s_ptr1,4);
	    memcpy(s_ptr1,tmpbuf,4);
	    s_ptr0 += 4;
	}
    }
    else if ( word_bytes == 8 ) {
	unsigned char tmpbuf[8];
	/* 32-bit���Ǥ� 4�Х��Ȥ��Ĥ��Τ�����? */
	while ( 0 < len_loop ) {
	    len_loop --;
	    s_ptr1 -= 8;
	    memcpy(tmpbuf,s_ptr0,8);
	    memcpy(s_ptr0,s_ptr1,8);
	    memcpy(s_ptr1,tmpbuf,8);
	    s_ptr0 += 8;
	}
    }
    else if ( ma_bytes <= word_bytes ) {
	const size_t n_j0 = word_bytes / ma_bytes;
	const size_t n_j1 = word_bytes % ma_bytes;
	unsigned char tmpbuf[ma_bytes];
	unsigned char tmp;
	size_t i,j;
	s_ptr1 += ma_bytes * n_j0;			/* offset */
	for ( i=0 ; i < len_loop ; i++ ) {
	    s_ptr1 -= (ma_bytes * n_j0 + word_bytes);
	    for ( j=0 ; j < n_j0 ; j++ ) {
		memcpy(tmpbuf,s_ptr0,ma_bytes);
		memcpy(s_ptr0,s_ptr1,ma_bytes);
		memcpy(s_ptr1,tmpbuf,ma_bytes);
		s_ptr0 += ma_bytes;
		s_ptr1 += ma_bytes;
	    }
	    for ( j=0 ; j < n_j1 ; j++ ) {
		tmp = s_ptr0[j];  s_ptr0[j] = s_ptr1[j];  s_ptr1[j] = tmp;
	    }
	    s_ptr0 += n_j1;
	}
    }
    else {
	unsigned char tmp;
	size_t i,j;
	for ( i=0 ; i < len_loop ; i++ ) {
	    s_ptr1 -= word_bytes;
	    for ( j=0 ; j < word_bytes ; j++ ) {
		tmp = s_ptr0[j];  s_ptr0[j] = s_ptr1[j];  s_ptr1[j] = tmp;
	    }
	    s_ptr0 += word_bytes;
	}
    }
    return;
}
