/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2013-05-30 11:03:03 cyamauch> */

/**
 * @file   sli_funcs.cc
 * @brief  SLIライブラリで使用する関数のコード
 */

#include "config.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

/**
 * @brief  標準エラー出力へのフォーマット出力
 */
int sli__eprintf(const char *format, ...)
{
    int s;
    va_list ap;
    va_start(ap,format);
    s = vfprintf(stderr,format,ap);
    va_end(ap);
    return s;
}

/**
 * @brief  文字列バッファへのフォーマット書き込み
 */
int sli__snprintf(char *str, size_t size, const char *format, ...)
{
    int s;
    va_list ap;
    va_start(ap,format);
    s = vsnprintf(str,size,format,ap);
    va_end(ap);
    return s;
}

/**
 * @brief  文字列の比較
 */
int sli__strcmp(const char *s1, const char *s2)
{
    return strcmp(s1,s2);
}

/**
 * @brief  EOF を返す
 */
int sli__eof()
{
    return EOF;
}

/**
 * @brief  SEEK_SET を返す
 */
int sli__seek_set()
{
    return SEEK_SET;
}

/**
 * @brief  SEEK_CUR を返す
 */
int sli__seek_cur()
{
    return SEEK_CUR;
}

/**
 * @brief  SEEK_END を返す
 */
int sli__seek_end()
{
    return SEEK_END;
}
