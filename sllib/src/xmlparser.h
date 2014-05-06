/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2009-11-23 18:07:53 cyamauch> */

#ifndef _SLI__XMLPARSER_H
#define _SLI__XMLPARSER_H 1

#include "cstreamio.h"
#include "tarray_tstring.h"

namespace sli
{
  const int Xmlparser__Begin = 1;
  const int Xmlparser__Solo  = 2;
  const int Xmlparser__Value = 3;
  const int Xmlparser__End   = 4;
  const int Xmlparser__Comment = 5;
  const int Xmlparser__Header  = 6;

  typedef struct {
    char *name;
    char *value;
  } xmlparser__value_leveltable;

  class xmlparser
  {
  public:
    xmlparser();
    virtual ~xmlparser();
    /* main */
    virtual int main( cstreamio &stream );
    virtual int main( cstreamio &stream, size_t xml_size );
    virtual int main( const char *all_xml );
    virtual int main( const char *all_xml, size_t xml_size );
  protected:
    /* Can be used in element_handler(). */
    virtual int test_hierarchy( int num_arg, ... ) const;
    virtual const char *get_element( int level, int index ) const;
    virtual const char *get_value( int level, const char *name ) const;
    /* main() calls this. User should override in user's class */
    virtual int element_handler( int type, int current_level, 
				 const char *element ) = 0;
    /* main() calls this via split_line(). User can override this method. */
    virtual const char *line_reader( ssize_t *size );
    /* main(...) calls this */
    virtual int main();
  private:
    /* dummy... Do not call this!!  */
    xmlparser(const xmlparser &obj);
    /* dummy... Do not call this!!  */
    xmlparser &operator=(const xmlparser &obj);
    /* main() calls this. */
    ssize_t split_line();
  private:
    /* managed by main() */
    int leveltable_currentlevel;
    int leveltable_allocnum;
    /* これら 2 つのテーブルは，
       各レベルの文字列が無効になるタイミングは同時である */
    tarray_tstring **leveltable;
    xmlparser__value_leveltable **value_leveltable;
    /* used in line_reader() and main(...) */
    /* managed by main(...) */
    cstreamio *stream;
    const char *all_xml_ptr;
    ssize_t left_xml_size;
    char *tmp_xml_buffer;
    long num_block;		/* 1 */
    size_t next_tmp_xml_pos;	/* 0 */
    /* used in split_line() and main() */
    /* managed by main() */
    const char *xmlline_ptr;
    size_t xmlline_size;
    size_t xmlline_current_pos;
    size_t xmlline_next_pos;
    char *filled_xmlline_buffer;
    bool filled_xmlline_is_provided;
  };
}

#endif	/* _SLI__XMLPARSER_H */
