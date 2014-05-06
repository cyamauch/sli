#ifndef _FITSIO_CSUM_H
#define _FITSIO_CSUM_H 1

typedef struct _fitsio_csum {
    size_t cnt;
    unsigned long hi;
    unsigned long lo;
    unsigned long sum;
} fitsio_csum;

#endif
