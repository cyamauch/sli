#!/bin/sh

filter() {
  cat $1 | sed \
    -e "s/@CLASS_NAME_U@/$CLASS_NAME_U/g" \
    -e "s/@CLASS_NAME_L@/$CLASS_NAME_L/g" \
    -e "s/@TYPE_L@/$TYPE_L/g" \
    -e "s/@SZ_TYPE@/$SZ_TYPE/g" \
    -e "s/@VA_TYPE@/$VA_TYPE/g" \
    -e "s/@ELM_FNC@/$ELM_FNC/g" \
    -e "s/@ELM_CS_FNC@/$ELM_CS_FNC/g" \
    -e "s/@SUPPORTS_VA@/$SUPPORTS_VA/g" \
    -e "s/@ZERO_VALUE@/$ZERO_VALUE/g" \
    -e "s/@USE_COMPLEX@/$USE_COMPLEX/g" \
    -e "s/@INDEF_VALUE@/$INDEF_VALUE/g" 
}

# default
SUPPORTS_VA="0"

# not for complex
USE_COMPLEX="0"
ZERO_VALUE="0"

CLASS_NAME_U="MDARRAY_FLOAT"
CLASS_NAME_L="mdarray_float"
TYPE_L="float"
SZ_TYPE="FLOAT_ZT"
#SUPPORTS_VA="1"
VA_TYPE="double"
ELM_FNC="f"
ELM_CS_FNC="f_cs"
INDEF_VALUE="NAN"

filter mdarray_types.h.src > ${CLASS_NAME_L}.h
filter mdarray_types.cc.src > ${CLASS_NAME_L}.cc

CLASS_NAME_U="MDARRAY_DOUBLE"
CLASS_NAME_L="mdarray_double"
TYPE_L="double"
SZ_TYPE="DOUBLE_ZT"
#SUPPORTS_VA="1"
VA_TYPE="double"
ELM_FNC="d"
ELM_CS_FNC="d_cs"
INDEF_VALUE="NAN"

filter mdarray_types.h.src > ${CLASS_NAME_L}.h
filter mdarray_types.cc.src > ${CLASS_NAME_L}.cc

CLASS_NAME_U="MDARRAY_UCHAR"
CLASS_NAME_L="mdarray_uchar"
TYPE_L="unsigned char"
SZ_TYPE="(ssize_t)UCHAR_ZT"
#SUPPORTS_VA="1"
VA_TYPE="int"
ELM_FNC="c"
ELM_CS_FNC="c_cs"
INDEF_VALUE="INDEF_UCHAR"

filter mdarray_types.h.src > ${CLASS_NAME_L}.h
filter mdarray_types.cc.src > ${CLASS_NAME_L}.cc

CLASS_NAME_U="MDARRAY_SHORT"
CLASS_NAME_L="mdarray_short"
TYPE_L="short"
SZ_TYPE="(ssize_t)SHORT_ZT"
#SUPPORTS_VA="1"
VA_TYPE="int"
ELM_FNC="s"
ELM_CS_FNC="s_cs"
INDEF_VALUE="INDEF_SHORT"

filter mdarray_types.h.src > ${CLASS_NAME_L}.h
filter mdarray_types.cc.src > ${CLASS_NAME_L}.cc

CLASS_NAME_U="MDARRAY_INT"
CLASS_NAME_L="mdarray_int"
TYPE_L="int"
SZ_TYPE="(ssize_t)INT_ZT"
#SUPPORTS_VA="1"
VA_TYPE="int"
ELM_FNC="i"
ELM_CS_FNC="i_cs"
INDEF_VALUE="INDEF_INT"

filter mdarray_types.h.src > ${CLASS_NAME_L}.h
filter mdarray_types.cc.src > ${CLASS_NAME_L}.cc

CLASS_NAME_U="MDARRAY_LONG"
CLASS_NAME_L="mdarray_long"
TYPE_L="long"
SZ_TYPE="(ssize_t)LONG_ZT"
#SUPPORTS_VA="1"
VA_TYPE="long"
ELM_FNC="l"
ELM_CS_FNC="l_cs"
INDEF_VALUE="INDEF_LONG"

filter mdarray_types.h.src > ${CLASS_NAME_L}.h
filter mdarray_types.cc.src > ${CLASS_NAME_L}.cc

CLASS_NAME_U="MDARRAY_LLONG"
CLASS_NAME_L="mdarray_llong"
TYPE_L="long long"
SZ_TYPE="(ssize_t)LLONG_ZT"
#SUPPORTS_VA="1"
VA_TYPE="long long"
ELM_FNC="ll"
ELM_CS_FNC="ll_cs"
INDEF_VALUE="INDEF_LLONG"

filter mdarray_types.h.src > ${CLASS_NAME_L}.h
filter mdarray_types.cc.src > ${CLASS_NAME_L}.cc

CLASS_NAME_U="MDARRAY_INT16"
CLASS_NAME_L="mdarray_int16"
TYPE_L="int16_t"
SZ_TYPE="(ssize_t)INT16_ZT"
SUPPORTS_VA="0"
VA_TYPE="int16_t"
ELM_FNC="i16"
ELM_CS_FNC="i16_cs"
INDEF_VALUE="INDEF_INT16"

filter mdarray_types.h.src > ${CLASS_NAME_L}.h
filter mdarray_types.cc.src > ${CLASS_NAME_L}.cc

CLASS_NAME_U="MDARRAY_INT32"
CLASS_NAME_L="mdarray_int32"
TYPE_L="int32_t"
SZ_TYPE="(ssize_t)INT32_ZT"
SUPPORTS_VA="0"
VA_TYPE="int32_t"
ELM_FNC="i32"
ELM_CS_FNC="i32_cs"
INDEF_VALUE="INDEF_INT32"

filter mdarray_types.h.src > ${CLASS_NAME_L}.h
filter mdarray_types.cc.src > ${CLASS_NAME_L}.cc

CLASS_NAME_U="MDARRAY_INT64"
CLASS_NAME_L="mdarray_int64"
TYPE_L="int64_t"
SZ_TYPE="(ssize_t)INT64_ZT"
SUPPORTS_VA="0"
VA_TYPE="int64_t"
ELM_FNC="i64"
ELM_CS_FNC="i64_cs"
INDEF_VALUE="INDEF_INT64"

filter mdarray_types.h.src > ${CLASS_NAME_L}.h
filter mdarray_types.cc.src > ${CLASS_NAME_L}.cc

# size_t は可変引数も演算もNG

CLASS_NAME_U="MDARRAY_SIZE"
CLASS_NAME_L="mdarray_size"
TYPE_L="size_t"
SZ_TYPE="(ssize_t)SIZE_ZT"
SUPPORTS_VA="0"
VA_TYPE="size_t"
ELM_FNC="z"
ELM_CS_FNC="z_cs"
INDEF_VALUE="0"

filter mdarray_types.h.src > ${CLASS_NAME_L}.h
filter mdarray_types.cc.src > ${CLASS_NAME_L}.cc

# ssize_t は可変引数はサポートされないが，演算はOK

CLASS_NAME_U="MDARRAY_SSIZE"
CLASS_NAME_L="mdarray_ssize"
TYPE_L="ssize_t"
SZ_TYPE="(ssize_t)SSIZE_ZT"
SUPPORTS_VA="0"
VA_TYPE="ssize_t"
ELM_FNC="sz"
ELM_CS_FNC="sz_cs"
INDEF_VALUE="0"

filter mdarray_types.h.src > ${CLASS_NAME_L}.h
filter mdarray_types.cc.src > ${CLASS_NAME_L}.cc

# bool は可変引数はサポートされるが，演算の結果は未定義

CLASS_NAME_U="MDARRAY_BOOL"
CLASS_NAME_L="mdarray_bool"
TYPE_L="bool"
SZ_TYPE="(ssize_t)BOOL_ZT"
#SUPPORTS_VA="1"
VA_TYPE="int"
ELM_FNC="b"
ELM_CS_FNC="b_cs"
INDEF_VALUE="false"

filter mdarray_types.h.src > ${CLASS_NAME_L}.h
filter mdarray_types.cc.src > ${CLASS_NAME_L}.cc

# uintptr_t は可変引数も演算もNG

CLASS_NAME_U="MDARRAY_UINTPTR"
CLASS_NAME_L="mdarray_uintptr"
TYPE_L="uintptr_t"
SZ_TYPE="(ssize_t)UINTPTR_ZT"
SUPPORTS_VA="0"
VA_TYPE="uintptr_t"
ELM_FNC="p"
ELM_CS_FNC="p_cs"
INDEF_VALUE="(uintptr_t)(NULL)"

filter mdarray_types.h.src > ${CLASS_NAME_L}.h
filter mdarray_types.cc.src > ${CLASS_NAME_L}.cc


# for complex
USE_COMPLEX="1"
ZERO_VALUE="(0.0 + 0.0*I)"

CLASS_NAME_U="MDARRAY_FCOMPLEX"
CLASS_NAME_L="mdarray_fcomplex"
TYPE_L="fcomplex"
SZ_TYPE="FCOMPLEX_ZT"
SUPPORTS_VA="0"
VA_TYPE="fcomplex"
ELM_FNC="fx"
ELM_CS_FNC="fx_cs"
INDEF_VALUE="NAN + NAN*I"

filter mdarray_types.h.src > ${CLASS_NAME_L}.h
filter mdarray_types.cc.src > ${CLASS_NAME_L}.cc

CLASS_NAME_U="MDARRAY_DCOMPLEX"
CLASS_NAME_L="mdarray_dcomplex"
TYPE_L="dcomplex"
SZ_TYPE="DCOMPLEX_ZT"
SUPPORTS_VA="0"
VA_TYPE="dcomplex"
ELM_FNC="dx"
ELM_CS_FNC="dx_cs"
INDEF_VALUE="NAN + NAN*I"

filter mdarray_types.h.src > ${CLASS_NAME_L}.h
filter mdarray_types.cc.src > ${CLASS_NAME_L}.cc

