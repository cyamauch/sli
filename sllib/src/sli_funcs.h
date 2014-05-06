#ifndef _SLI__SLI_FUNCS_H
#define _SLI__SLI_FUNCS_H 1

/**
 * @file   sli_funcs.h
 * @brief  SLIライブラリで使用するグローバル定数の定義，関数のプロトタイプ
 */

#include <stddef.h>

extern int sli__eprintf(const char *, ...);
extern int sli__snprintf(char *, size_t, const char *, ...);
extern int sli__strcmp(const char *, const char *);
extern int sli__eof();
extern int sli__seek_set();
extern int sli__seek_cur();
extern int sli__seek_end();

const long Sli__long_1 = 1;
const float Sli__float_2 = 2;
const bool Sli__byte_order_is_little_endian = 
	( ((const char *)(&Sli__long_1))[0] != 0 ? true : false );
const bool Sli__float_word_order_is_little_endian =
	( ((const char *)(&Sli__float_2))[0] != 0 ? false : true );

#endif /* _SLI__SLI_FUNCS_H */
