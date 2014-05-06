#ifndef __TUNE_SSE2_H
#define __TUNE_SSE2_H 1

/* Minimum byte length of buffer read or written by SSE2. */
/* NOTE: Do not set values less than 16.                 */
#define _SSE2_MIN_NBYTES 64

/* Typical size of CPU cache in functions using SSE2. */
/* This is used for switching _mm_store/_mm_stream.   */
#define _SSE2_CPU_CACHE_SIZE (1024*8192)

#endif
