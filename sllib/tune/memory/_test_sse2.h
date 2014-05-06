#ifndef __TEST_SSE2_H
#define __TEST_SSE2_H 1

#if defined(USE_SIMD) && defined(__SSE2__)
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__INTEL_COMPILER)
#define _SSE2_IS_OK 1
#endif
#endif

#endif
