
#define USE_OPTIMIZED_BYTE_SWAP 1

/* GCC builtin-byteswap (__builtin_bswap32, __builtin_bswap64) */
#if (defined(__GNUC__) && __GNUC__ >= 4 && __GNUC_MINOR__ >= 3)
#define _GCC_BSWAP_OK 1
#endif

#include "_test_sse2.h"

#ifdef _SSE2_IS_OK
#include <emmintrin.h>
#include "_tune_sse2.h"
#endif

/**
 * @brief  SIMD命令(SSE2)を使った2バイト値のバイトオーダの高速変換
 */
inline static void s_byteswap2( void *buf, size_t len_elements )
{
    unsigned char *d_ptr = (unsigned char *)buf;
    unsigned char tmp;

#ifdef USE_OPTIMIZED_BYTE_SWAP
#ifdef _SSE2_IS_OK
    if ( _SSE2_MIN_NBYTES <= 2 * len_elements ) {
	bool is_aligned = ( ((size_t)d_ptr & 0x01) == 0 );
	if ( is_aligned == true ) {			/* align if possible */
	    while ( ((size_t)d_ptr & 0x0f) != 0 ) {
		len_elements --;
		tmp = d_ptr[0];  d_ptr[0] = d_ptr[1];  d_ptr[1] = tmp;
		d_ptr += 2;
	    }
	}
	/* NOTE: Do not use _mm_stream...() that causes slow down.      */
	/*       Using load+store improves slightly, but u+u is enough. */
	while ( 8 <= len_elements ) {
	    len_elements -= 8;
	    __m128i r0 = _mm_loadu_si128((__m128i *)d_ptr);
	    __m128i r1 = r0;
	    /* 1-byte shift */
	    r0 = _mm_srli_epi16(r0, 8);
	    r1 = _mm_slli_epi16(r1, 8);
	    /* or */
	    r0 = _mm_or_si128(r0, r1);
	    _mm_storeu_si128((__m128i *)d_ptr, r0);
	    d_ptr += 2 * 8;
	}
    }
#else
    while ( 8 <= len_elements ) {
	len_elements -= 8;
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
#endif	/* _SSE2_IS_OK */
#endif	/* USE_OPTIMIZED_BYTE_SWAP */

    while ( 0 < len_elements ) {
	len_elements --;
	tmp = d_ptr[0];  d_ptr[0] = d_ptr[1];  d_ptr[1] = tmp;
	d_ptr += 2;
    }

    return;
}

/**
 * @brief  SIMD命令(SSE2)を使った4バイト値のバイトオーダの高速変換
 */
inline static void s_byteswap4( void *buf, size_t len_elements )
{
    unsigned char *d_ptr = (unsigned char *)buf;
    unsigned char tmp;

#ifdef USE_OPTIMIZED_BYTE_SWAP
#ifdef _SSE2_IS_OK
    if ( _SSE2_MIN_NBYTES <= 4 * len_elements ) {
	bool is_aligned = ( ((size_t)d_ptr & 0x03) == 0 );
	if ( is_aligned == true ) {			/* align if possible */
	    while ( ((size_t)d_ptr & 0x0f) != 0 ) {
		len_elements --;
		tmp = d_ptr[0];  d_ptr[0] = d_ptr[3];  d_ptr[3] = tmp;
		tmp = d_ptr[1];  d_ptr[1] = d_ptr[2];  d_ptr[2] = tmp;
		d_ptr += 4;
	    }
	}
	/* NOTE: Do not use _mm_stream...() that causes slow down.      */
	/*       Using load+store improves slightly, but u+u is enough. */
	while ( 4 <= len_elements ) {
	    len_elements -= 4;
	    __m128i r0 = _mm_loadu_si128((__m128i *)d_ptr);
	    /* 0123 => 2301 */
	    r0 = _mm_shufflelo_epi16(r0, _MM_SHUFFLE(2,3,0,1));
	    r0 = _mm_shufflehi_epi16(r0, _MM_SHUFFLE(2,3,0,1));
	    __m128i r1 = r0;
	    /* 1-byte shift */
	    r0 = _mm_srli_epi16(r0, 8);
	    r1 = _mm_slli_epi16(r1, 8);
	    /* or */
	    r0 = _mm_or_si128(r0, r1);
	    _mm_storeu_si128((__m128i *)d_ptr, r0);
	    d_ptr += 4 * 4;
	}
    }
#else
    while ( 4 <= len_elements ) {
	len_elements -= 4;
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
#endif	/* _SSE2_IS_OK */
#endif	/* USE_OPTIMIZED_BYTE_SWAP */

    while ( 0 < len_elements ) {
	len_elements --;
	tmp = d_ptr[0];  d_ptr[0] = d_ptr[3];  d_ptr[3] = tmp;
	tmp = d_ptr[1];  d_ptr[1] = d_ptr[2];  d_ptr[2] = tmp;
	d_ptr += 4;
    }

    return;
}

/**
 * @brief  SIMD命令(SSE2)を使った8バイト値のバイトオーダの高速変換
 */
inline static void s_byteswap8( void *buf, size_t len_elements )
{
    unsigned char *d_ptr = (unsigned char *)buf;
    unsigned char tmp;

#ifdef USE_OPTIMIZED_BYTE_SWAP
#ifdef _SSE2_IS_OK
    if ( _SSE2_MIN_NBYTES <= 8 * len_elements ) {
	bool is_aligned = ( ((size_t)d_ptr & 0x07) == 0 );
	if ( is_aligned == true ) {			/* align if possible */
	    while ( ((size_t)d_ptr & 0x0f) != 0 ) {
		len_elements --;
		tmp = d_ptr[0];  d_ptr[0] = d_ptr[7];  d_ptr[7] = tmp;
		tmp = d_ptr[1];  d_ptr[1] = d_ptr[6];  d_ptr[6] = tmp;
		tmp = d_ptr[2];  d_ptr[2] = d_ptr[5];  d_ptr[5] = tmp;
		tmp = d_ptr[3];  d_ptr[3] = d_ptr[4];  d_ptr[4] = tmp;
		d_ptr += 8;
	    }
	}
	/* NOTE: Do not use _mm_stream...() that causes slow down.      */
	/*       Using load+store improves slightly, but u+u is enough. */
	while ( 2 <= len_elements ) {
	    len_elements -= 2;
	    __m128i r0 = _mm_loadu_si128((__m128i *)d_ptr);
	    /* 01234567 => 67452301 */
	    r0 = _mm_shufflelo_epi16(r0, _MM_SHUFFLE(0,1,2,3));
	    r0 = _mm_shufflehi_epi16(r0, _MM_SHUFFLE(0,1,2,3));
	    __m128i r1 = r0;
	    /* 1-byte shift */
	    r0 = _mm_srli_epi16(r0, 8);
	    r1 = _mm_slli_epi16(r1, 8);
	    /* or */
	    r0 = _mm_or_si128(r0, r1);
	    _mm_storeu_si128((__m128i *)d_ptr, r0);
	    d_ptr += 8 * 2;
	}
    }
#else
    while ( 2 <= len_elements ) {
	len_elements -= 2;
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
#endif	/* _SSE2_IS_OK */
#endif	/* USE_OPTIMIZED_BYTE_SWAP */

    while ( 0 < len_elements ) {
	len_elements --;
	tmp = d_ptr[0];  d_ptr[0] = d_ptr[7];  d_ptr[7] = tmp;
	tmp = d_ptr[1];  d_ptr[1] = d_ptr[6];  d_ptr[6] = tmp;
	tmp = d_ptr[2];  d_ptr[2] = d_ptr[5];  d_ptr[5] = tmp;
	tmp = d_ptr[3];  d_ptr[3] = d_ptr[4];  d_ptr[4] = tmp;
	d_ptr += 8;
    }

    return;
}

/**
 * @brief  SIMD命令(SSE2)を使ったバイトオーダの高速変換
 *
 * 任意のバイト長の配列(アライメント規定なし)のバイトオーダを変換します
 * (SSE2対応版)
 *
 * @param      buf 配列の先頭アドレス
 * @param      word_bytes 1要素のバイト長
 * @param      len_elements 配列の要素の個数
 * @note       バイト長が2,4,8,16バイトの場合は特別なコードで動作し，特に高速
 *             です．その他のバイト長では汎用コードで動作しますが，8バイトを
 *             越える場合ではそこそこ速く動くように調整してあります．<br>
 *             テスト環境は GCC 3.3.2 + PentiumMのやや古いラップトップと
 *             64-bit Linux．<br>
 *             標準ライブラリの htonl(), htons(), ntohl(), ntohs(), swab()
 *             も参照のこと．
 */
inline static void s_byteswap( void *buf, 
			       size_t word_bytes, size_t len_elements )
{
    unsigned char *d_ptr = (unsigned char *)buf;
    unsigned char tmp;
    size_t i;

    /*
     * ロジックの単純なコードでの表現:
     *	const size_t len_each = word_bytes / 2;
     *	unsigned char *d_ptr1 = d_ptr + word_bytes - 1;
     *	for ( i=0 ; i < len_elements ; i++ ) {
     *	    for ( j=0 ; j < len_each ; j++ ) {
     *		tmp = d_ptr[j];
     *		d_ptr[j] = d_ptr1[-j];
     *		d_ptr1[-j] = tmp;
     *	    }
     *	    d_ptr += word_bytes;
     *	    d_ptr1 += word_bytes;
     *	}
     */

    /* fast vesion for 2,4 and 8 bytes */
    if ( word_bytes == 2 ) s_byteswap2(buf, len_elements);
    else if ( word_bytes == 4 ) s_byteswap4(buf, len_elements);
    else if ( word_bytes == 8 ) s_byteswap8(buf, len_elements);
#ifdef USE_OPTIMIZED_BYTE_SWAP
#ifdef _SSE2_IS_OK
    else if ( 16 <= word_bytes && _SSE2_MIN_NBYTES <= word_bytes * len_elements ) {
	const size_t len_each = word_bytes / 2;
	unsigned char *d_ptr1 = d_ptr;
	unsigned char *d_ptr_x;
	size_t j;
	for ( i=0 ; i < len_elements ; i++ ) {
	    d_ptr1 += word_bytes;			/* next point */
	    d_ptr_x = d_ptr1;
	    j = len_each;
	    while ( 8 <= j ) {
		if ( 16 <= j ) j -= 16;		/* not overwrap */
		else j = 0;			/* overwrapped */
		d_ptr_x -= 16;
		__m128i r0 = _mm_loadu_si128((__m128i *)d_ptr);
		__m128i r1 = _mm_loadu_si128((__m128i *)d_ptr_x);
		/* 01234567 89abcdef => 89abcdef 01234567 */
		r0 = _mm_shuffle_epi32(r0, _MM_SHUFFLE(1,0,3,2));
		r1 = _mm_shuffle_epi32(r1, _MM_SHUFFLE(1,0,3,2));
		/* 01234567 => 67452301 */
		r0 = _mm_shufflelo_epi16(r0, _MM_SHUFFLE(0,1,2,3));
		r1 = _mm_shufflelo_epi16(r1, _MM_SHUFFLE(0,1,2,3));
		r0 = _mm_shufflehi_epi16(r0, _MM_SHUFFLE(0,1,2,3));
		r1 = _mm_shufflehi_epi16(r1, _MM_SHUFFLE(0,1,2,3));
		__m128i r0a = r0;
		__m128i r1a = r1;
		/* 1-byte shift */
		r0 = _mm_srli_epi16(r0, 8);
		r1 = _mm_srli_epi16(r1, 8);
		r0a = _mm_slli_epi16(r0a, 8);
		r1a = _mm_slli_epi16(r1a, 8);
		/* or */
		r0 = _mm_or_si128(r0, r0a);
		r1 = _mm_or_si128(r1, r1a);
		_mm_storeu_si128((__m128i *)d_ptr_x, r0);
		_mm_storeu_si128((__m128i *)d_ptr, r1);
		d_ptr += 16;
	    }
	    d_ptr_x -= j;
	    while ( 0 < j ) {
		j--;
		tmp = *d_ptr;  *d_ptr = d_ptr_x[j];  d_ptr_x[j] = tmp;
		d_ptr++;
	    }
	    d_ptr = d_ptr1;
	}
    }
#endif	/* _SSE2_IS_OK */
#endif	/* USE_OPTIMIZED_BYTE_SWAP */
    /* fallback                                                       */
    /* 実験では word_bytes=4 の時に遅くなるが，他の場合はそこそこ速い */
    else {
	const size_t len_each = word_bytes / 2;
	unsigned char *d_ptr1 = d_ptr;
	unsigned char *d_ptr_x;
	size_t j;
	for ( i=0 ; i < len_elements ; i++ ) {
	    d_ptr1 += word_bytes;
	    d_ptr_x = d_ptr1;
	    j = len_each;
#ifdef USE_OPTIMIZED_BYTE_SWAP
	    while ( 4 <= j ) {
		j -= 4;
		d_ptr_x -= 4;
		tmp = d_ptr[0];  d_ptr[0] = d_ptr_x[3];  d_ptr_x[3] = tmp;
		tmp = d_ptr[1];  d_ptr[1] = d_ptr_x[2];  d_ptr_x[2] = tmp;
		tmp = d_ptr[2];  d_ptr[2] = d_ptr_x[1];  d_ptr_x[1] = tmp;
		tmp = d_ptr[3];  d_ptr[3] = d_ptr_x[0];  d_ptr_x[0] = tmp;
		d_ptr += 4;
	    }
#endif	/* USE_OPTIMIZED_BYTE_SWAP */
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

#ifdef USE_OPTIMIZED_BYTE_SWAP
#undef USE_OPTIMIZED_BYTE_SWAP
#endif
