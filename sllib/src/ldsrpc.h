/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2009-11-23 17:58:49 cyamauch> */

/*
 *  XMLRPC の規格を一部流用したオレオレプロトコルによる
 *  LDS のクライアント．
 */

#ifndef _SLI__LDSRPC_H
#define _SLI__LDSRPC_H 1

#include <stddef.h>
#include <stdint.h>

#include "slierr.h"

namespace sli
{
  const int Ldsrpc__String_t  = 1;
  const int Ldsrpc__Boolean_t = 2;
  const int Ldsrpc__Int_t     = 3;
  const int Ldsrpc__Long_t    = 4;
  const int Ldsrpc__Double_t  = 5;

  typedef struct {
    long index;		/* 何個目のarrayに属するか */
    char *name;
    int type;
    char *value;
  } ldsrpc__member;

  typedef bool    ldsrpc__boolean_t;
  typedef int32_t ldsrpc__int_t;
  typedef int64_t ldsrpc__long_t;
  typedef double  ldsrpc__double_t;

  class ldsrpc_xmlparser;

  class ldsrpc {
    friend class ldsrpc_xmlparser;
  public:
    /* constructor & destructor */
    ldsrpc();
    ldsrpc( const ldsrpc &obj );
    virtual ~ldsrpc();
    virtual ldsrpc &operator=( const ldsrpc &obj );
    /* */
    virtual ldsrpc &init();
    virtual ldsrpc &init( const ldsrpc &obj );
    /* */
    virtual ldsrpc &append( long index, const char *name, 
			    const char *value );
    virtual ldsrpc &append( long index, const char *name, 
			    ldsrpc__boolean_t value );
    virtual ldsrpc &append( long index, const char *name, 
			    ldsrpc__int_t value );
    virtual ldsrpc &append( long index, const char *name, 
			    ldsrpc__long_t value );
    virtual ldsrpc &append( long index, const char *name, 
			    ldsrpc__double_t value );
    virtual ldsrpc &clear_call_members();
    virtual int call( const char *url, const char *method_name = NULL, 
		      const char *written_file_or_dir = NULL );
    virtual long response_size() const;
    virtual long response_length() const;
    virtual const ldsrpc__member &response( long index ) const;
    //virtual ldsrpc &assign_verbose( bool val );
    virtual bool &verbose();
    virtual int classlevel() const;
  protected:
    virtual ldsrpc &append_a_call_member( long index, const char *name, 
					  int type, const char *value );
    virtual ldsrpc &append_a_response_member( long index, const char *name, 
					      int type, const char *value );
    virtual ldsrpc &clear_response_members();
    int classlevel_rec;
  private:
    /* LDS へ送信するのん */
    long call_members_size_rec;
    ldsrpc__member *call_members_rec;
    /* LDS から受信するのん */
    long response_members_size_rec;
    ldsrpc__member *response_members_rec;
    bool verbose_rec;
  };
}

#endif	/* _SLI__LDSRPC_H */
