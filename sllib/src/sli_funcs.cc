/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2013-05-30 11:03:03 cyamauch> */

/**
 * @file   sli_funcs.cc
 * @brief  SLI�饤�֥��ǻ��Ѥ���ؿ��Υ�����
 */

#include "config.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

/**
 * @brief  ɸ�२�顼���ϤؤΥե����ޥåȽ���
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
 * @brief  ʸ����Хåե��ؤΥե����ޥåȽ񤭹���
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
 * @brief  ʸ��������
 */
int sli__strcmp(const char *s1, const char *s2)
{
    return strcmp(s1,s2);
}

/**
 * @brief  EOF ���֤�
 */
int sli__eof()
{
    return EOF;
}

/**
 * @brief  SEEK_SET ���֤�
 */
int sli__seek_set()
{
    return SEEK_SET;
}

/**
 * @brief  SEEK_CUR ���֤�
 */
int sli__seek_cur()
{
    return SEEK_CUR;
}

/**
 * @brief  SEEK_END ���֤�
 */
int sli__seek_end()
{
    return SEEK_END;
}
