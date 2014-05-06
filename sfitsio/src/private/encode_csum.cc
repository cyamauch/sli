/* This is Licence of CFITSIO roooooooooooooooutine :-) */
/*
Copyright (Unpublished--all rights reserved under the copyright laws of
the United States), U.S. Government as represented by the Administrator
of the National Aeronautics and Space Administration.  No copyright is
claimed in the United States under Title 17, U.S. Code.

Permission to freely use, copy, modify, and distribute this software
and its documentation without fee is hereby granted, provided that this
copyright notice and disclaimer of warranty appears in all copies.
(However, see the restriction on the use of the gzip compression code,
below).

DISCLAIMER:

THE SOFTWARE IS PROVIDED 'AS IS' WITHOUT ANY WARRANTY OF ANY KIND,
EITHER EXPRESSED, IMPLIED, OR STATUTORY, INCLUDING, BUT NOT LIMITED TO,
ANY WARRANTY THAT THE SOFTWARE WILL CONFORM TO SPECIFICATIONS, ANY
IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
PURPOSE, AND FREEDOM FROM INFRINGEMENT, AND ANY WARRANTY THAT THE
DOCUMENTATION WILL CONFORM TO THE SOFTWARE, OR ANY WARRANTY THAT THE
SOFTWARE WILL BE ERROR FREE.  IN NO EVENT SHALL NASA BE LIABLE FOR ANY
DAMAGES, INCLUDING, BUT NOT LIMITED TO, DIRECT, INDIRECT, SPECIAL OR
CONSEQUENTIAL DAMAGES, ARISING OUT OF, RESULTING FROM, OR IN ANY WAY
CONNECTED WITH THIS SOFTWARE, WHETHER OR NOT BASED UPON WARRANTY,
CONTRACT, TORT , OR OTHERWISE, WHETHER OR NOT INJURY WAS SUSTAINED BY
PERSONS OR PROPERTY OR OTHERWISE, AND WHETHER OR NOT LOSS WAS SUSTAINED
FROM, OR AROSE OUT OF THE RESULTS OF, OR USE OF, THE SOFTWARE OR
SERVICES PROVIDED HEREUNDER.
*/
static
void ffesum(unsigned long sum,  /* I - accumulated checksum                */
           int complm,          /* I - = 1 to encode complement of the sum */
           char *ascii)         /* O - 16-char ASCII encoded checksum      */
/*
    encode the 32 bit checksum by converting every 
    2 bits of each byte into an ASCII character (32 bit word encoded 
    as 16 character string).   Only ASCII letters and digits are used
    to encode the values (no ASCII punctuation characters).

    If complm=TRUE, then the complement of the sum will be encoded.

    This routine is based on the C algorithm developed by Rob
    Seaman at NOAO that was presented at the 1994 ADASS conference,
    published in the Astronomical Society of the Pacific Conference Series.
*/
{
    unsigned int exclude[13] = { 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f, 0x40,
                                       0x5b, 0x5c, 0x5d, 0x5e, 0x5f, 0x60 };
    unsigned long mask[4] = { 0xff000000, 0xff0000, 0xff00, 0xff  };

    int offset = 0x30;     /* ASCII 0 (zero) */

    unsigned long value;
    int byte, quotient, remainder, ch[4], check, ii, jj, kk;
    char asc[32];

    if (complm)
        value = 0xFFFFFFFF - sum;   /* complement each bit of the value */
    else
        value = sum;

    for (ii = 0; ii < 4; ii++)
    {
        byte = (value & mask[ii]) >> (24 - (8 * ii));
        quotient = byte / 4 + offset;
        remainder = byte % 4;
        for (jj = 0; jj < 4; jj++)
            ch[jj] = quotient;

        ch[0] += remainder;

        for (check = 1; check;)   /* avoid ASCII  punctuation */
            for (check = 0, kk = 0; kk < 13; kk++)
                for (jj = 0; jj < 4; jj += 2)
                    if ((unsigned char) ch[jj] == exclude[kk] ||
                        (unsigned char) ch[jj+1] == exclude[kk])
                    {
                        ch[jj]++;
                        ch[jj+1]--;
                        check++;
                    }

        for (jj = 0; jj < 4; jj++)        /* assign the bytes */
            asc[4*jj+ii] = ch[jj];
    }

    for (ii = 0; ii < 16; ii++)       /* shift the bytes 1 to the right */
        ascii[ii] = asc[(ii+15)%16];

    ascii[16] = '\0';
}

static void encode_csum( const fitsio_csum &suminfo, char *ret_encoded )
{
    ffesum(suminfo.sum, 1, ret_encoded);
    return;
}
