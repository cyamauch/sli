#include <sli/stdstreamio.h>
#include <sli/fitscc.h>
using namespace sli;

/*
 * A test code to CONTINUE convension in FITS header
 *
 * ./continue_header out_file.fits[.gz or .bz2]
 *
 */

static const long COL_SIZE = 16;
static const long ROW_SIZE = 16;

int main( int argc, char *argv[] )
{
    int return_status = -1;
    stdstreamio sio;
    fitscc fits;
    long i;

    const fits::header_def hdr_defs[] = 
	{ {"COMMENT",  "-------------------------------"},
	  /* Test with comment string */
	  {"LONGVAL0", "0123456789abcdefghijklmnopqrstuvwxyz"
	   "ABCDEFGHIJKLMNOPQRSTUVWXYZ012345",
	   "Sample of long value :-)"},
	  {"LONGVAL1", "0123456789abcdefghijklmnopqrstuvwxyz"
	   "ABCDEFGHIJKLMNOPQRSTUVWXYZ01234",
	   "Sample of long value :-)"},
	  {"LONGVAL2", "0123456789abcdefghijklmnopqrstuvwxyz"
	   "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123",
	   "Sample of long value :-)"},
	  {"LONGVAL3", "0123456789abcdefghijklmnopqrstuvwxyz"
	   "ABCDEFGHIJKLMNOPQRSTUVWXYZ012",
	   "Sample of long value :-)"},
	  {"LONGVAL4", "0123456789abcdefghijklmnopqrstuvwxyz"
	   "ABCDEFGHIJKLMNOPQRSTUVWXYZ01",
	   "Sample of long value :-)"},
	  {"LONGVAL5", "0123456789abcdefghijklmnopqrstuvwxyz"
	   "ABCDEFGHIJKLMNOPQRSTUVWXYZ0",
	   "Sample of long value :-)"},
	  {"COMMENT",  "-------------------------------"},
	  /* Test without comment string */
	  {"LONGVAL6", "0123456789abcdefghijklmnopqrstuvwxyz"
	   "ABCDEFGHIJKLMNOPQRSTUVWXYZ01234567",
	   ""},
	  {"LONGVAL7", "0123456789abcdefghijklmnopqrstuvwxyz"
	   "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456",
	   ""},
	  {"LONGVAL8", "0123456789abcdefghijklmnopqrstuvwxyz"
	   "ABCDEFGHIJKLMNOPQRSTUVWXYZ012345",
	   ""},
	  {"LONGVAL9", "0123456789abcdefghijklmnopqrstuvwxyz"
	   "ABCDEFGHIJKLMNOPQRSTUVWXYZ01234",
	   ""},
	  {"LONGVALA", "0123456789abcdefghijklmnopqrstuvwxyz"
	   "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123",
	   ""},
	  {"LONGVALB", "0123456789abcdefghijklmnopqrstuvwxyz"
	   "ABCDEFGHIJKLMNOPQRSTUVWXYZ012",
	   ""},
	  {"COMMENT",  "-------------------------------"},
	  /* Test with long comment string */
	  {"LONGVALC", "0123456789abcdefghijklmnopqrstuvwxyz"
	   "ABCDEFGHIJKLMNOPQRSTUVWXY",
	   "1 2 3 4 5 6 7 8 9 a b c d e f g h i j k l m n o p q r s t u w x y z"},
	  {"LONGVALD", "0123456789abcdefghijklmnopqrstuvwxyz"
	   "ABCDEFGHIJKLMNOPQRSTUVWXYZ",
	   "1 2 3 4 5 6 7 8 9 a b c d e f g h i j k l m n o p q r s t u w x y z"},
	  {"LONGVALE", "0123456789abcdefghijklmnopqrstuvwxyz"
	   "ABCDEFGHIJKLMNOPQRSTUVWXYZ0",
	   "1 2 3 4 5 6 7 8 9 a b c d e f g h i j k l m n o p q r s t u w x y z"},
	  {"LONGVALF", "0123456789abcdefghijklmnopqrstuvwxyz"
	   "ABCDEFGHIJKLMNOPQRSTUVWXYZ01",
	   "1 2 3 4 5 6 7 8 9 a b c d e f g h i j k l m n o p q r s t u w x y z"},
	  {"LONGVALG", "0123456789abcdefghijklmnopqrstuvwxyz"
	   "ABCDEFGHIJKLMNOPQRSTUVWXYZ012",
	   "1 2 3 4 5 6 7 8 9 a b c d e f g h i j k l m n o p q r s t u w x y z"},
	  {"COMMENT",  "-------------------------------"},
	  /* Test2 with comment string */

	  {"LLNGVAL0", "________12345678901234567890123456789012345678901234567890123456789"
	   "0123456789abcdefghijklmnopqrstuvwxyz"
	   "ABCDEFGHIJKLMNOPQRSTUVWXYZ012345",
	   "Sample of long value :-)"},
	  {"LLNGVAL1", "________12345678901234567890123456789012345678901234567890123456789"
	   "0123456789abcdefghijklmnopqrstuvwxyz"
	   "ABCDEFGHIJKLMNOPQRSTUVWXYZ01234",
	   "Sample of long value :-)"},
	  {"LLNGVAL2", "________12345678901234567890123456789012345678901234567890123456789"
	   "0123456789abcdefghijklmnopqrstuvwxyz"
	   "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123",
	   "Sample of long value :-)"},
	  {"LLNGVAL3", "________12345678901234567890123456789012345678901234567890123456789"
	   "0123456789abcdefghijklmnopqrstuvwxyz"
	   "ABCDEFGHIJKLMNOPQRSTUVWXYZ012",
	   "Sample of long value :-)"},
	  {"LLNGVAL4", "________12345678901234567890123456789012345678901234567890123456789"
	   "0123456789abcdefghijklmnopqrstuvwxyz"
	   "ABCDEFGHIJKLMNOPQRSTUVWXYZ01",
	   "Sample of long value :-)"},
	  {"LLNGVAL5", "________12345678901234567890123456789012345678901234567890123456789"
	   "0123456789abcdefghijklmnopqrstuvwxyz"
	   "ABCDEFGHIJKLMNOPQRSTUVWXYZ0",
	   "Sample of long value :-)"},
	  {"COMMENT",  "-------------------------------"},
	  /* Test2 without comment string */
	  {"LLNGVAL6", "________12345678901234567890123456789012345678901234567890123456789"
	   "0123456789abcdefghijklmnopqrstuvwxyz"
	   "ABCDEFGHIJKLMNOPQRSTUVWXYZ01234567",
	   ""},
	  {"LLNGVAL7", "________12345678901234567890123456789012345678901234567890123456789"
	   "0123456789abcdefghijklmnopqrstuvwxyz"
	   "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456",
	   ""},
	  {"LLNGVAL8", "________12345678901234567890123456789012345678901234567890123456789"
	   "0123456789abcdefghijklmnopqrstuvwxyz"
	   "ABCDEFGHIJKLMNOPQRSTUVWXYZ012345",
	   ""},
	  {"LLNGVAL9", "________12345678901234567890123456789012345678901234567890123456789"
	   "0123456789abcdefghijklmnopqrstuvwxyz"
	   "ABCDEFGHIJKLMNOPQRSTUVWXYZ01234",
	   ""},
	  {"LLNGVALA", "________12345678901234567890123456789012345678901234567890123456789"
	   "0123456789abcdefghijklmnopqrstuvwxyz"
	   "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123",
	   ""},
	  {"LLNGVALB", "________12345678901234567890123456789012345678901234567890123456789"
	   "0123456789abcdefghijklmnopqrstuvwxyz"
	   "ABCDEFGHIJKLMNOPQRSTUVWXYZ012",
	   ""},
	  {"COMMENT",  "-------------------------------"},
	  /* Test2 with long comment string */
	  {"LLNGVALC", "________12345678901234567890123456789012345678901234567890123456789"
	   "0123456789abcdefghijklmnopqrstuvwxyz"
	   "ABCDEFGHIJKLMNOPQRSTUVWXY",
	   "1 2 3 4 5 6 7 8 9 a b c d e f g h i j k l m n o p q r s t u w x y z"},
	  {"LLNGVALD", "________12345678901234567890123456789012345678901234567890123456789"
	   "0123456789abcdefghijklmnopqrstuvwxyz"
	   "ABCDEFGHIJKLMNOPQRSTUVWXYZ",
	   "1 2 3 4 5 6 7 8 9 a b c d e f g h i j k l m n o p q r s t u w x y z"},
	  {"LLNGVALE", "________12345678901234567890123456789012345678901234567890123456789"
	   "0123456789abcdefghijklmnopqrstuvwxyz"
	   "ABCDEFGHIJKLMNOPQRSTUVWXYZ0",
	   "1 2 3 4 5 6 7 8 9 a b c d e f g h i j k l m n o p q r s t u w x y z"},
	  {"LLNGVALF", "________12345678901234567890123456789012345678901234567890123456789"
	   "0123456789abcdefghijklmnopqrstuvwxyz"
	   "ABCDEFGHIJKLMNOPQRSTUVWXYZ01",
	   "1 2 3 4 5 6 7 8 9 a b c d e f g h i j k l m n o p q r s t u w x y z"},
	  {"LLNGVALG", "________12345678901234567890123456789012345678901234567890123456789"
	   "0123456789abcdefghijklmnopqrstuvwxyz"
	   "ABCDEFGHIJKLMNOPQRSTUVWXYZ012",
	   "1 2 3 4 5 6 7 8 9 a b c d e f g h i j k l m n o p q r s t u w x y z"},
	  {"COMMENT",  "-------------------------------"},
	  {"VAL0",     "10",
	   "1 2 3 4 5 6 7 8 9 a b c d e f g h i j k l m n o p q r s t u w x y z"},
	  {"COMMENT",  "-------------------------------"},
	  {"WCVAL0", "'0 1 2 3 4 5 6 7 8 9 a b c d e f g h i j k l m n o p q r s t u v w x y z A B C D E F G '",
	   "Sample of long value :-)"},
	  {"WCVAL1", "'00 1 2 3 4 5 6 7 8 9 a b c d e f g h i j k l m n o p q r s t u v w x y z A B C D E F G '",
	   "Sample of long value :-)"},
	  {"WCVAL2", "'00 11 22 33 44 55 66 77 88 99 aa bb cc dd ee ff gg hh ii jj kk ll mm nn oo pp qq rr ss tt uu vv ww xx yy zz'",
	   "Sample of long value :-)"},
	  {"WCVAL3", "'000 11 22 33 44 55 66 77 88 99 aa bb cc dd ee ff gg hh ii jj kk ll mm nn oo pp qq rr ss tt uu vv ww xx yy zz'",
	   "Sample of long value :-)"},
	  {"WCVAL4", "'0000 11 22 33 44 55 66 77 88 99 aa bb cc dd ee ff gg hh ii jj kk ll mm nn oo pp qq rr ss tt uu vv ww xx yy zz'",
	   "Sample of long value :-)"},
	  {"COMMENT",  "-------------------------------"},
	  /* Test "''" with comment string */
	  {"QCVAL0", "'0000123456789abcdefghijklmnopqrstuvwxyz"
	   "ABCDEFGHIJKLMNOPQRSTUVW''_''_'''",
	   "Sample of long value :-)"},
	  {"QCVAL1", "'0000123456789abcdefghijklmnopqrstuvwxyz"
	   "ABCDEFGHIJKLMNOPQRSTUV''_''_'''",
	   "Sample of long value :-)"},
	  {"QCVAL2", "'0000123456789abcdefghijklmnopqrstuvwxyz"
	   "ABCDEFGHIJKLMNOPQRSTU''_''_'''",
	   "Sample of long value :-)"},
	  {"QCVAL3", "'0000123456789abcdefghijklmnopqrstuvwxyz"
	   "ABCDEFGHIJKLMNOPQRST''_''_'''",
	   "Sample of long value :-)"},
	  {"QCVAL4", "'0000123456789abcdefghijklmnopqrstuvwxyz"
	   "ABCDEFGHIJKLMNOPQRS''_''_'''",
	   "Sample of long value :-)"},
	  {"QCVAL5", "'0000123456789abcdefghijklmnopqrstuvwxyz"
	   "ABCDEFGHIJKLMNOPQR''_''_'''",
	   "Sample of long value :-)"},
	  {"QCVAL6", "'0000123456789abcdefghijklmnopqrstuvwxyz"
	   "ABCDEFGHIJKLMNOPQ''_''_'''",
	   "Sample of long value :-)"},
	  {"QCVAL7", "'0000123456789abcdefghijklmnopqrstuvwxyz"
	   "ABCDEFGHIJKLMNOP''_''_'''",
	   "Sample of long value :-)"},
	  {"COMMENT",  "-------------------------------"},
	  /* 実は，下記のコメントのケースの対策はできていないorz */
	  {"QCVAL10", "'0000123456789abcdefghijklmnopqrstuvwxyz"
	   "ABCDEFGHIJKLMNOPQRSTUVW''_''_'''",
	   "Sample_of_l0000000000000000000000000000000000000000000000000000000ong value :-)"},
	  {"QCVAL11", "'0000123456789abcdefghijklmnopqrstuvwxyz"
	   "ABCDEFGHIJKLMNOPQRSTUV''_''_'''",
	   "Sample_of_l0000000000000000000000000000000000000000000000000000000ong value :-)"},
	  {"QCVAL12", "'0000123456789abcdefghijklmnopqrstuvwxyz"
	   "ABCDEFGHIJKLMNOPQRSTU''_''_'''",
	   "Sample_of_l0000000000000000000000000000000000000000000000000000000ong value :-)"},
	  {"QCVAL13", "'0000123456789abcdefghijklmnopqrstuvwxyz"
	   "ABCDEFGHIJKLMNOPQRST''_''_'''",
	   "Sample_of_l0000000000000000000000000000000000000000000000000000000ong value :-)"},
	  {"QCVAL14", "'0000123456789abcdefghijklmnopqrstuvwxyz"
	   "ABCDEFGHIJKLMNOPQRS''_''_'''",
	   "Sample_of_l0000000000000000000000000000000000000000000000000000000ong value :-)"},
	  {"QCVAL15", "'0000123456789abcdefghijklmnopqrstuvwxyz"
	   "ABCDEFGHIJKLMNOPQR''_''_'''",
	   "Sample_of_l0000000000000000000000000000000000000000000000000000000ong value :-)"},
	  {"QCVAL16", "'0000123456789abcdefghijklmnopqrstuvwxyz"
	   "ABCDEFGHIJKLMNOPQ''_''_'''",
	   "Sample_of_l0000000000000000000000000000000000000000000000000000000ong value :-)"},
	  {"QCVAL17", "'0000123456789abcdefghijklmnopqrstuvwxyz"
	   "ABCDEFGHIJKLMNOP''_''_'''",
	   "Sample_of_l0000000000000000000000000000000000000000000000000000000ong value :-)"},
	  {"COMMENT",  "-------------------------------"},
	  {"QVAL0", "'0123456789abcdefghijklmnopqrstuvwxyz"
	   "ABCDEFGHIJKLMNOPQRSTUVWXYZ''_''_'''",
	   ""},
	  {"QVAL1", "'0123456789abcdefghijklmnopqrstuvwxyz"
	   "ABCDEFGHIJKLMNOPQRSTUVWXY''_''_'''",
	   ""},
	  {"QVAL2", "'0123456789abcdefghijklmnopqrstuvwxyz"
	   "ABCDEFGHIJKLMNOPQRSTUVWX''_''_'''",
	   ""},
	  {"QVAL3", "'0123456789abcdefghijklmnopqrstuvwxyz"
	   "ABCDEFGHIJKLMNOPQRSTUVW''_''_'''",
	   ""},
	  {"QVAL4", "'0123456789abcdefghijklmnopqrstuvwxyz"
	   "ABCDEFGHIJKLMNOPQRSTUV''_''_'''",
	   ""},
	  {"QVAL5", "'0123456789abcdefghijklmnopqrstuvwxyz"
	   "ABCDEFGHIJKLMNOPQRSTU''_''_'''",
	   ""},
	  {"COMMENT",  "-------------------------------"},
	  /* */
	  {"VAL1",     "10",
	   "_2 3 4 5 6 7 8 9 a b c d e f g h i j k l m n o p q r s t u w x y"},
	  {"COMMENT",  "-------------------------------"},
	  {"DESC0",     "This is a description.\\n",
	   "1 2 3 4 5 6 7 8 9 a b c d e f g h i j k l m n o p q r s t u w x y z"},
	  {"DESC1",     "This is a looooooooooooooooooooooooooooooooooooong description.",
	   "1 2 3 4 5 6 7 8 9 a b c d e f g h i j k l m n o p q r s t u w x y z"},
	  {"DESC2",     "This is a loooooooooooooooooooooooooooooooooooooong description.",
	   "1 2 3 4 5 6 7 8 9 a b c d e f g h i j k l m n o p q r s t u w x y z"},
	  {"DESC3",     "This is a looooooooooooooooooooooooooooooooooooooong description.",
	   "1 2 3 4 5 6 7 8 9 a b c d e f g h i j k l m n o p q r s t u w x y z"},
	  {"DESC4",     "This is a loooooooooooooooooooooooooooooooooooooooong description.",
	   "1 2 3 4 5 6 7 8 9 a b c d e f g h i j k l m n o p q r s t u w x y z"},
	  {"DESC5",     "This is a loooooooooooooooooooooooooooooooooooooong description.\\n"
	   "This convention is useful to convert FITS header into HTML, "
	   "VOTable, LaTeX, etc.",
	   "1 2 3 4 5 6 7 8 9 a b c d e f g h i j k l m n o p q r s t u w x y z"},
	  {"DESC6",     "This is a looooooooooooooooooooooooooooooooooooooong description.\\n"
	   "This convention is useful to convert FITS header into HTML, "
	   "VOTable, LaTeX, etc.",
	   "1 2 3 4 5 6 7 8 9 a b c d e f g h i j k l m n o p q r s t u w x y z"},
	  {"DESC7",     "This is a loooooooooooooooooooooooooooooooooooooooong description.\\n"
	   "This convention is useful to convert FITS header into HTML, "
	   "VOTable, LaTeX, etc.",
	   "1 2 3 4 5 6 7 8 9 a b c d e f g h i j k l m n o p q r s t u w x y z"},
	  {"DESC8",     "This is a looooooooooooooooooooooooooooooooooooooooong description.\\n"
	   "This convention is useful to convert FITS header into HTML, "
	   "VOTable, LaTeX, etc.",
	   "1 2 3 4 5 6 7 8 9 a b c d e f g h i j k l m n o p q r s t u w x y z"},
	  {"DESC9",     "This is a loooooooooooooooooooooooooooooooooooooooooong description.\\n"
	   "This convention is useful to convert FITS header into HTML, "
	   "VOTable, LaTeX, etc.",
	   "1 2 3 4 5 6 7 8 9 a b c d e f g h i j k l m n o p q r s t u w x y z"},
	  {"COMMENT",  "-------------------------------"},
	  {"COMMENT", "0123456789abcdefghijklmnopqrstuvwxyz"
	              "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	              "0123456789abcdefghijklmnopqrstuvwxyz"
	              "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	              "0123456789abcdefghijklmnopqrstuvwxyz"
	              "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	  },
	  {"COMMENT",  "-------------------------------"},
	  {"COMMENT",     
	   "Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the \"Software\"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:  "
	   "The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software."
	  },
	  {"COMMENT",  "-------------------------------"},
	  {"LICENSE0",     
	   "Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the \"Software\"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:\\n\\n"
	   "The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.", 
	   ""
	  },
	  {"LICENSE1",     
	   "Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the \"Software\"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:\\n\\n"
	   "The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.", 
	   "This is MIT LICENSE :-)"
	  },
	  {"TELEMl", 
	   "CREON,SHTOP,FWPOSON,FWPOS_B1,FWPOS_B0,MPOSON,MPOS_B1,MPOS_B0,RSTWIDELON,RSTWIDESON,RSTN170ON,RSTN60ON,LWBOOSTON,SWBOOSTON,LWBIASON,SWBIASON,CALALON,CALASON,CALBON,SINALON,SINASON",
	   "elements in STATUS"
	  },
	  {"TELEMm", 
	   //"BAD_FRAME,UNDEF_ANOM_FRAME,BLANK,IN_SAA,NEAR_MOON,UNTRUSTED_FRAME",
	   "BAD_FRAME,UNDEF_ANOM_FRAME,BLANK,IN_SAA,NEAR_MOON,UNTRUSTED_FRAME",
	   "element names"
	  },
	  {"TELEMn", 
	   "QUAL_CV_PARAM:2,QUAL_RC_PARAM:2,QUAL_RC_CF:2,QUAL_DF_EQ:2,"
	   "QUAL_RP_DATA:2,QUAL_RP_PARAM:2,QUAL_RP_TABLE:2,QUAL_FF_PARAM:2,"
	   "QUAL_FF_CF:2,QUAL_GPGL_CORR:2,QUAL_MTGL_CORR:2,QUAL_TR_HIST:2,"
	   "QUAL_TR_PARAM:2,QUAL_DK_DATA:2,QUAL_DK_PARAM:2,QUAL_DK_TABLE:2,"
	   "QUAL_FX_CORR:2,QUAL_FX_PARAM:2,,,,",
	   "element names"
	  },
	  {"TTYPEn", "TRIG", "column name" },
	  {"TDESCn", "Trigger type flag,\\n"
	   "  b1000000:SUD(trigd by SuperUpper Discriminator),\\n"
	   "  b0100000:ANODE,\\n"
	   "  b0010000:PIN0 b0001000:PIN1 b0000100:PIN2,\\n"
	   "  b0000010:PIN3 b0000001:PSEUDO",
	   "description of column"},
	  {"COMMENT",  "-------------------------------"},
	  {"QUOTTST0", 
	   "abcdefghijklmnopqrstuvwxyz",
	   "'abc' 'def' 'ghi' 'jkl' 'mno' 'pqr' 'stu' 'vw' 'xyz'"
	  },
	  {"QUOTTST1", 
	   "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
	   "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz",
	   "'abc' 'def' 'ghi' 'jkl' 'mno' 'pqr' 'stu' 'vw' 'xyz'"
	  },
	  {"QUOTTST2", 
	   "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
	   "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
	   "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz",
	   "'abc' 'def' 'ghi' 'jkl' 'mno' 'pqr' 'stu' 'vw' 'xyz'"
	  },
	  {"QUOTTST3", 
	   "abcdefghijklmnopqrstuvwxyz",
	   "'abc' 'def' 'ghi' 'jkl' 'mno' 'pqr' 'stu' 'vw' 'xyz' '123' '456' '789' 'ABC' 'DEF' 'XYZ'"
	  },
	  {"QUOTTST4", 
	   "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
	   "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz",
	   "'abc' 'def' 'ghi' 'jkl' 'mno' 'pqr' 'stu' 'vw' 'xyz' '123' '456' '789' 'ABC' 'DEF' 'XYZ'"
	  },
	  {"QUOTTST5", 
	   "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
	   "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
	   "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz",
	   "'abc' 'def' 'ghi' 'jkl' 'mno' 'pqr' 'stu' 'vw' 'xyz' '123' '456' '789' 'ABC' 'DEF' 'XYZ'"
	  },
	  {"COMMENT",  "-------------------------------"},
	  {NULL} };

    /* Create Primary HDU */
    fits.append_image("Primary", 0,
		      FITS::SHORT_T, COL_SIZE, ROW_SIZE );

    /* Initialize Header */
    fits.image("Primary").header_init(hdr_defs);

    /* Save to file... */
    if ( 1 < argc ) {
	ssize_t sz;
        const char *out_file = argv[1];
	/* write */
	sz = fits.write_stream(out_file);	/* writing file */
	if ( sz < 0 ) {
	    sio.eprintf("[ERROR] obj.write_stream() failed\n");
	    goto quit;
	}
	/* and read */
        sz = fits.read_stream(out_file);	/* reading file */
	if ( sz < 0 ) {
            sio.eprintf("[ERROR] fits.read_stream() failed\n");
            goto quit;
        }
        /* printing all header */
        for ( i=0 ; i < fits.image("Primary").header_length() ; i++ ) {
            if ( fits.image("Primary").header(i).status() 
		 == FITS::NORMAL_RECORD ) {
                sio.printf("[%s] = [%s] / [%s]\n",
                           fits.image("Primary").header(i).keyword(),
                           fits.image("Primary").header(i).value(),
                           fits.image("Primary").header(i).comment());
            }
            else {
                sio.printf("[%s] [%s]\n",
                           fits.image("Primary").header(i).keyword(),
                           fits.image("Primary").header(i).value());
            }
        }
    }
    /* Display */
    else {
	sio.printf("==== HEADER BEGIN ====\n");
	sio.printf("%s",fits.image("Primary").header_formatted_string());
	sio.printf("==== HEADER END ====\n");
	sio.printf("NOTE:\n");
	sio.printf("Set arg1(filename) to test write and read test.\n");
    }

    return_status = 0;
 quit:
    return return_status;
}
