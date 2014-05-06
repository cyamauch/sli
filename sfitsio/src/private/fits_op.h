#define DO_OPERATION(MACRO_TO_DO) \
      MACRO_TO_DO(FITS__DOUBLE_T,fits__double_t,) \
      else MACRO_TO_DO(FITS__FLOAT_T,fits__float_t,) \
      else MACRO_TO_DO(FITS__SHORT_T,fits__short_t,c_lround) \
      else MACRO_TO_DO(FITS__LONG_T,fits__long_t,c_lround) \
      else MACRO_TO_DO(FITS__BYTE_T,fits__byte_t,c_lround) \
      else MACRO_TO_DO(FITS__LONGLONG_T,fits__longlong_t,c_llround)

#define DO_OPERATION_2TYPES(MACRO_TO_DO) \
      /* */ \
      MACRO_TO_DO(FITS__DOUBLE_T,fits__double_t, \
	       FITS__DOUBLE_T,fits__double_t,) \
      else MACRO_TO_DO(FITS__DOUBLE_T,fits__double_t, \
	       FITS__FLOAT_T,fits__float_t,) \
      else MACRO_TO_DO(FITS__DOUBLE_T,fits__double_t, \
	       FITS__SHORT_T,fits__short_t,c_lround) \
      else MACRO_TO_DO(FITS__DOUBLE_T,fits__double_t, \
	       FITS__LONG_T,fits__long_t,c_lround) \
      else MACRO_TO_DO(FITS__DOUBLE_T,fits__double_t, \
	       FITS__BYTE_T,fits__byte_t,c_lround) \
      else MACRO_TO_DO(FITS__DOUBLE_T,fits__double_t, \
	       FITS__LONGLONG_T,fits__longlong_t,c_llround) \
      /* */ \
      else MACRO_TO_DO(FITS__FLOAT_T,fits__float_t, \
	       FITS__DOUBLE_T,fits__double_t,) \
      else MACRO_TO_DO(FITS__FLOAT_T,fits__float_t, \
	       FITS__FLOAT_T,fits__float_t,) \
      else MACRO_TO_DO(FITS__FLOAT_T,fits__float_t, \
	       FITS__SHORT_T,fits__short_t,c_lround) \
      else MACRO_TO_DO(FITS__FLOAT_T,fits__float_t, \
	       FITS__LONG_T,fits__long_t,c_lround) \
      else MACRO_TO_DO(FITS__FLOAT_T,fits__float_t, \
	       FITS__BYTE_T,fits__byte_t,c_lround) \
      else MACRO_TO_DO(FITS__FLOAT_T,fits__float_t, \
	       FITS__LONGLONG_T,fits__longlong_t,c_llround) \
      /* */ \
      else MACRO_TO_DO(FITS__SHORT_T,fits__short_t, \
	       FITS__DOUBLE_T,fits__double_t,) \
      else MACRO_TO_DO(FITS__SHORT_T,fits__short_t, \
	       FITS__FLOAT_T,fits__float_t,) \
      else MACRO_TO_DO(FITS__SHORT_T,fits__short_t, \
	       FITS__SHORT_T,fits__short_t,c_lround) \
      else MACRO_TO_DO(FITS__SHORT_T,fits__short_t, \
	       FITS__LONG_T,fits__long_t,c_lround) \
      else MACRO_TO_DO(FITS__SHORT_T,fits__short_t, \
	       FITS__BYTE_T,fits__byte_t,c_lround) \
      else MACRO_TO_DO(FITS__SHORT_T,fits__short_t, \
	       FITS__LONGLONG_T,fits__longlong_t,c_llround) \
      /* */ \
      else MACRO_TO_DO(FITS__LONG_T,fits__long_t, \
	       FITS__DOUBLE_T,fits__double_t,) \
      else MACRO_TO_DO(FITS__LONG_T,fits__long_t, \
	       FITS__FLOAT_T,fits__float_t,) \
      else MACRO_TO_DO(FITS__LONG_T,fits__long_t, \
	       FITS__SHORT_T,fits__short_t,c_lround) \
      else MACRO_TO_DO(FITS__LONG_T,fits__long_t, \
	       FITS__LONG_T,fits__long_t,c_lround) \
      else MACRO_TO_DO(FITS__LONG_T,fits__long_t, \
	       FITS__BYTE_T,fits__byte_t,c_lround) \
      else MACRO_TO_DO(FITS__LONG_T,fits__long_t, \
	       FITS__LONGLONG_T,fits__longlong_t,c_llround) \
      /* */ \
      else MACRO_TO_DO(FITS__BYTE_T,fits__byte_t, \
	       FITS__DOUBLE_T,fits__double_t,) \
      else MACRO_TO_DO(FITS__BYTE_T,fits__byte_t, \
	       FITS__FLOAT_T,fits__float_t,) \
      else MACRO_TO_DO(FITS__BYTE_T,fits__byte_t, \
	       FITS__SHORT_T,fits__short_t,c_lround) \
      else MACRO_TO_DO(FITS__BYTE_T,fits__byte_t, \
	       FITS__LONG_T,fits__long_t,c_lround) \
      else MACRO_TO_DO(FITS__BYTE_T,fits__byte_t, \
	       FITS__BYTE_T,fits__byte_t,c_lround) \
      else MACRO_TO_DO(FITS__BYTE_T,fits__byte_t, \
	       FITS__LONGLONG_T,fits__longlong_t,c_llround) \
      /* */ \
      else MACRO_TO_DO(FITS__BYTE_T,fits__byte_t, \
	       FITS__DOUBLE_T,fits__double_t,) \
      else MACRO_TO_DO(FITS__BYTE_T,fits__byte_t, \
	       FITS__FLOAT_T,fits__float_t,) \
      else MACRO_TO_DO(FITS__BYTE_T,fits__byte_t, \
	       FITS__SHORT_T,fits__short_t,c_lround) \
      else MACRO_TO_DO(FITS__BYTE_T,fits__byte_t, \
	       FITS__LONG_T,fits__long_t,c_lround) \
      else MACRO_TO_DO(FITS__BYTE_T,fits__byte_t, \
	       FITS__BYTE_T,fits__byte_t,c_lround) \
      else MACRO_TO_DO(FITS__BYTE_T,fits__byte_t, \
	       FITS__LONGLONG_T,fits__longlong_t,c_llround)
