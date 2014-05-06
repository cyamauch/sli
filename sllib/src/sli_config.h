/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2013-04-27 20:06:54 cyamauch> */

#ifndef _SLI__SLI_CONFIG_H
#define _SLI__SLI_CONFIG_H 1

/**
 * @file   sli_config.h
 * @brief  SLIライブラリ全般の設定
 */


/* at() の const 版をサポートする */
#define SLI__OVERLOAD_CONST_AT 1

/*
 * 注意: 下記の設定はユーザ・プログラムにおける SIMD 命令関係の設定．
 *       SLLIBのビルド時の設定は config.h で行なう．
 */

/* Enable SIMD of inline functions */
#define SLI__USE_SIMD_FOR_INLINE 1

#if defined(SLI__USE_SIMD_FOR_INLINE) && defined(__SSE2__)
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__INTEL_COMPILER)
#define SLI__INLINE_SSE2_IS_OK 1
#endif
#endif

/* Minimum byte length of buffer read or written by SIMD. */
/* NOTE: Do not set values less than 16.                  */
#define SLI__SIMD_MIN_NBYTES 64

/* Typical size of CPU cache in functions using SIMD. */
/* This is used for switching _mm_store/_mm_stream.   */
#define SLI__SIMD_CPU_CACHE_SIZE (1024*1024*8)


#endif  /* _SLI_CONFIG_H */
