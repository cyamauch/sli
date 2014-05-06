/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2013-02-03 00:57:05 cyamauch> */

#include "config.h"

#include "ldsrpc.h"
#include "inetstreamio.h"
#include "stdstreamio.h"
#include "tstring.h"
#include "xmlparser.h"

#include <stdlib.h>
#include <pwd.h>

#include "private/err_report.h"

#include "private/c_stderr.h"
#include "private/c_fprintf.h"
#include "private/c_snprintf.h"

#include "private/c_memset.h"
#include "private/c_strdup.h"
#include "private/c_strchr.h"
#include "private/c_strlen.h"
#include "private/c_strcat.h"
#include "private/c_strcpy.h"
#include "private/c_strspn.h"
#include "private/c_strncmp.h"
#include "private/c_strcmp.h"

#include "private/c_gethostname.h"
#include "private/c_geteuid.h"

/* ================================================================ */
/* ldsrpc_xmlparser */
/* ================================================================ */

#define CLASS_NAME "ldsrpc_xmlparser"

namespace sli
{

class ldsrpc_xmlparser : public xmlparser
{
public:
    ldsrpc_xmlparser( ldsrpc *thisp );
    virtual bool is_fault() const;
protected:
    /* overridden */
    int element_handler( int type, int current_level, const char *element );
private:
    ldsrpc *ldsrpc_ptr;
    bool is_fault_rec;
    long resp_index_rec;
    bool resp_value_ok;
    int  resp_value_type;
    tstring resp_name_rec;
};

ldsrpc_xmlparser::ldsrpc_xmlparser( ldsrpc *thisp )
{
    this->ldsrpc_ptr = thisp;
    this->is_fault_rec = false;
    this->resp_index_rec = 0;
    this->resp_value_ok = false;
    this->resp_value_type = Ldsrpc__String_t;
}

bool ldsrpc_xmlparser::is_fault() const
{
    return this->is_fault_rec;
}

#define FUNC_NAME "element_handler"
int ldsrpc_xmlparser::element_handler( int type, int current_level, 
				       const char *element )
{
    int return_status = -1;

    if ( this->resp_name_rec.cstr() == NULL ) {
        this->resp_name_rec.assign("");
    }

    switch ( type ) {
    case Xmlparser__Header:
        break;
    case Xmlparser__End:
	if ( this->test_hierarchy(3,"data","value","struct") == 0 ) {
	    if ( this->ldsrpc_ptr->verbose() != false ) {
		c_fprintf(c_stderr(),"info: xml index = %ld\r",this->resp_index_rec);
	    }
	    this->resp_index_rec ++;
	}
	else if ( this->test_hierarchy(5,"data","value","struct",
				       "member","value") == 0 ) {
	    resp_value_ok = false;
	}
	break;
    case Xmlparser__Begin:
	if ( resp_value_ok == true ) {
	    if ( this->test_hierarchy(1,"boolean") == 0 ) {
		this->resp_value_type = Ldsrpc__Boolean_t;
	    }
	    else if ( this->test_hierarchy(1,"int") == 0 ) {
		this->resp_value_type = Ldsrpc__Int_t;
	    }
	    else if ( this->test_hierarchy(1,"long") == 0 ) {
		this->resp_value_type = Ldsrpc__Long_t;
	    }
	    else if ( this->test_hierarchy(1,"double") == 0 ) {
		this->resp_value_type = Ldsrpc__Double_t;
	    }
	    else {
		this->resp_value_type = Ldsrpc__String_t;
	    }
	}
	else {
	    if ( this->test_hierarchy(5,"data","value","struct",
				      "member","value") == 0 ) {
		resp_value_ok = true;
	    }
	}
	break;
    case Xmlparser__Value:
	if ( resp_value_ok == true ) {
	    this->ldsrpc_ptr->append_a_response_member(
		        this->resp_index_rec, this->resp_name_rec.cstr(),
		        this->resp_value_type, element );
	}
	else if ( this->test_hierarchy(5,"data","value","struct",
				       "member","name") == 0 ) {
	    this->resp_name_rec.assign(element);
	}
	/* display fault */
	else if ( this->test_hierarchy(5,"fault","value","struct",
				       "member","name") == 0 ) {
	    this->is_fault_rec = true;
	    this->resp_name_rec.assign(element);
	}
	else if ( this->test_hierarchy(6,"fault","value","struct",
				       "member","value","int") == 0 ) {
	    this->is_fault_rec = true;
	    c_fprintf(c_stderr(),"%s = %s\n",this->resp_name_rec.cstr(),element);
	}
	else if ( this->test_hierarchy(6,"fault","value","struct",
				       "member","value","string") == 0 ) {
	    this->is_fault_rec = true;
	    c_fprintf(c_stderr(),"%s = %s\n",this->resp_name_rec.cstr(),element);
	}
	break;
    }

    return_status = 0;

    return return_status;
}
#undef FUNC_NAME

}	/* namespace sli */

/* ================================================================ */
/* ldsrpc */
/* ================================================================ */

#undef CLASS_NAME
#define CLASS_NAME "ldsrpc"

namespace sli
{

static int members_to_xml( const char *method_name,
			   long num_members, 
			   const ldsrpc__member *members,
			   tstring *ret_xml );

/* constructor */
ldsrpc::ldsrpc()
{
    this->classlevel_rec = 0;
    this->call_members_size_rec = 0;
    this->call_members_rec = NULL;
    this->response_members_size_rec = 0;
    this->response_members_rec = NULL;
    this->verbose_rec = false;

    return;
}

/* copy constructor */
#define FUNC_NAME "ldsrpc"
ldsrpc::ldsrpc( const ldsrpc &obj )
{
    this->classlevel_rec = 0;
    this->call_members_size_rec = 0;
    this->call_members_rec = NULL;
    this->response_members_size_rec = 0;
    this->response_members_rec = NULL;
    this->verbose_rec = false;

    this->init(obj);

    return;
}
#undef FUNC_NAME

/* destructor */
ldsrpc::~ldsrpc()
{
    this->clear_call_members();
    this->clear_response_members();

    return;
}

#define FUNC_NAME "operator="
ldsrpc &ldsrpc::operator=(const ldsrpc &obj)
{
    this->init(obj);
    return *this;
}
#undef FUNC_NAME

ldsrpc &ldsrpc::init()
{
    this->clear_call_members();
    this->clear_response_members();
    this->verbose_rec = false;

    return *this;
}

#define FUNC_NAME "init"
ldsrpc &ldsrpc::init(const ldsrpc &obj)
{
    void *tmp_ptr;

    ldsrpc::init();

    if ( obj.call_members_rec != NULL ) {
	size_t sz;
	long i;
	/* */
	sz = sizeof(ldsrpc__member) * obj.call_members_size_rec;
	tmp_ptr = malloc(sz);
	if ( tmp_ptr == NULL ) {
            err_throw(FUNC_NAME,"FATAL","malloc() failed");
	}
	this->call_members_rec = (ldsrpc__member *)tmp_ptr;
	c_memset(tmp_ptr,0,sz);
	this->call_members_size_rec = obj.call_members_size_rec;
	for ( i=0 ; i < obj.call_members_size_rec ; i++ ) {
	    this->call_members_rec[i].index = obj.call_members_rec[i].index;
	    this->call_members_rec[i].type = obj.call_members_rec[i].type;
	    this->call_members_rec[i].name = 
		c_strdup(obj.call_members_rec[i].name);
	    if ( this->call_members_rec[i].name == NULL ) {
		ldsrpc::init();
		err_throw(FUNC_NAME,"FATAL","strdup() failed");
	    }
	    this->call_members_rec[i].value = 
		c_strdup(obj.call_members_rec[i].value);
	    if ( this->call_members_rec[i].value == NULL ) {
		ldsrpc::init();
		err_throw(FUNC_NAME,"FATAL","strdup() failed");
	    }
	}
	/* */
	sz = sizeof(ldsrpc__member) * obj.response_members_size_rec;
	tmp_ptr = malloc(sz);
	if ( tmp_ptr == NULL ) {
	    ldsrpc::init();
            err_throw(FUNC_NAME,"FATAL","malloc() failed");
	}
	this->response_members_rec = (ldsrpc__member *)tmp_ptr;
	c_memset(tmp_ptr,0,sz);
	this->response_members_size_rec = obj.response_members_size_rec;
	for ( i=0 ; i < obj.response_members_size_rec ; i++ ) {
	    this->response_members_rec[i].index = 
		obj.response_members_rec[i].index;
	    this->response_members_rec[i].type = 
		obj.response_members_rec[i].type;
	    this->response_members_rec[i].name = 
		c_strdup(obj.response_members_rec[i].name);
	    if ( this->response_members_rec[i].name == NULL ) {
		ldsrpc::init();
		err_throw(FUNC_NAME,"FATAL","strdup() failed");
	    }
	    this->response_members_rec[i].value = 
		c_strdup(obj.response_members_rec[i].value);
	    if ( this->response_members_rec[i].value == NULL ) {
		ldsrpc::init();
		err_throw(FUNC_NAME,"FATAL","strdup() failed");
	    }
	}
    }

    this->verbose_rec = obj.verbose_rec;

    return *this;
}
#undef FUNC_NAME

ldsrpc &ldsrpc::append( long index, const char *name, 
			const char *value )
{
    return this->append_a_call_member( index, name, Ldsrpc__String_t, value );
}

ldsrpc &ldsrpc::append( long index, const char *name,
			ldsrpc__boolean_t value )
{
    char buf[64];
    c_snprintf(buf,64,"%d",(int)value);
    return this->append_a_call_member( index, name, Ldsrpc__Boolean_t, buf );
}

ldsrpc &ldsrpc::append( long index, const char *name,
			ldsrpc__int_t value )
{
    char buf[64];
    c_snprintf(buf,64,"%ld",(long)value);
    return this->append_a_call_member( index, name, Ldsrpc__Int_t, buf );
}

ldsrpc &ldsrpc::append( long index, const char *name,
			ldsrpc__long_t value )
{
    char buf[64];
    c_snprintf(buf,64,"%lld",(long long)value);
    return this->append_a_call_member( index, name, Ldsrpc__Long_t, buf );
}

ldsrpc &ldsrpc::append( long index, const char *name,
			ldsrpc__double_t value )
{
    const char *ptr;
    char buf[64];
    c_snprintf(buf,64,"%.15g",(double)value);
    /* %.15g の場合，. が書かれない事がある */
    if ( c_strchr(buf,'e') == NULL ) {
        if ( c_strchr(buf,'.') == NULL ) {
            if ( c_strlen(buf) < 64 - 1 ) {
                c_strcat(buf,".");
            }
        }
    }
    for ( ptr = buf ; *ptr == ' ' ; ptr++ );
    return this->append_a_call_member( index, name, Ldsrpc__Double_t, ptr );
}

#define FUNC_NAME "call"
int ldsrpc::call( const char *url, const char *method_name, 
		  const char *written_file_or_dir )
{
    int return_status = -1;
    inetstreamio sio;
    char hostname[256];
    struct passwd *userinfo;
    const char *username = NULL;
    tstring xml;
    char receive_buf[65536];
    long long length_all = 0;
    long long length_xml = 0;
    long long length_length_xml = 0;
    long long length_binary = 0;
    long i;
    char *rw_buf = NULL;
    ldsrpc_xmlparser xmlp(this);

    if ( url == NULL ) {
	err_report(FUNC_NAME,"ERROR","NULL url??");
	goto quit;
    }
    
    if ( method_name == NULL ) method_name = "";
    if ( members_to_xml( method_name,
			 this->call_members_size_rec, this->call_members_rec,
			 &xml ) < 0 ) {
	err_report(FUNC_NAME,"ERROR","members_to_xml() failed");
	goto quit;
    }
    
    /* ホスト名 */
    if ( c_gethostname(hostname,256) != 0 ) {
	c_strcpy(hostname,"VeryLongHostname");
    }
    hostname[255] = '\0';
    /* ユーザ名 */
    userinfo = getpwuid(c_geteuid());
    if ( userinfo != NULL ) {
	username = userinfo->pw_name;
    }

    if ( this->verbose() != false ) {
	c_fprintf(c_stderr(),"info: connecting the server...\n");
    }
    /* サーバへ接続 */
    if ( sio.open("r+",url) < 0 ) {
	err_report1(FUNC_NAME,"ERROR","cannot open url: %s",url);
	goto quit;
    }

    if ( this->verbose() != false ) {
	c_fprintf(c_stderr(),"info: sending request...\n");
    }
    /* ヘッダを送信 */
    sio.printf("POST %s HTTP/1.1\r\n",sio.path());
    sio.printf("Content-Type: text/xml\r\n");
    sio.printf("Content-Length: %ld\r\n",(long)xml.length());
    if ( username != NULL ) {
	sio.printf("User-Agent: %s@%s %s-%s::ldsrpc\r\n",
		   username,hostname, PACKAGE_NAME,PACKAGE_VERSION);
    }
    else {
	sio.printf("User-Agent: %ld@%s %s-%s::ldsrpc\r\n",
		   (long)c_geteuid(),hostname, PACKAGE_NAME,PACKAGE_VERSION);
    }
    //header = [ header, 'SOAPAction: ' + class + '#' + method ]
    sio.printf("Host: %s\r\n",sio.host());
    sio.printf("Connection: close\r\n");
    sio.printf("\r\n");

    /* XML を送信 */
    sio.printf("%s",xml.cstr());

    /* フラッシュ!! */
    sio.flush();

    xml.init();

    /* 受信バッファ初期化 */
    this->clear_response_members();

    if ( this->verbose() != false ) {
	c_fprintf(c_stderr(),"info: receiving results...\n");
    }

    /* HEADER を取得．Content-Length だけゲット */
    while ( sio.getstr(receive_buf,65536) != NULL ) {
	const char *str;
	if ( c_strcmp(receive_buf,"\r\n") == 0 ) break;
	str = "Content-Length: ";
	if ( c_strncmp(receive_buf,str,c_strlen(str)) == 0 ) {
	    char *rr;
	    rr = c_strchr(receive_buf,'\r');
	    if ( rr != NULL ) *rr='\0';
	    length_all=atoll(receive_buf+c_strlen(str));
	}
	/* 手抜きだが，長すぎのものは捨てる */
	if ( c_strchr(receive_buf,'\n') == NULL ) {
	    int c;
	    while( (c=sio.getchr()) != '\n' ) {
		if ( c == EOF ) break;
	    }
	}
    }

    if ( length_all <= 0 ) {
	err_report(FUNC_NAME,"ERROR","the server returns no Content-Length");
	goto quit;
    }

    /* XML の長さを取得する */
    sio.getstr(receive_buf,65536);
    i = c_strspn(receive_buf,"0123456789");
    if ( i==0 || receive_buf[i] != '\n' ) {
	err_report1(FUNC_NAME,"ERROR","the server returns error. "
		    "length of XML = %s ??", receive_buf);
	goto quit;
    }
    receive_buf[i] = '\0';
    length_xml = atoll(receive_buf);
    length_length_xml = c_strlen(receive_buf) + 1;

    if ( this->verbose() != false ) {
	c_fprintf(c_stderr(),"info: length_xml = %lld\n",length_xml);
    }

    if ( length_xml <= 0 ) {
	err_report(FUNC_NAME,"ERROR","the server returns no XML length.");
	goto quit;
    }

    length_binary = length_all - length_length_xml - length_xml;

    if ( this->verbose() != false ) {
	c_fprintf(c_stderr(),"info: length_binary = %lld\n",length_binary);
    }

    /* XML のパーサへ GO ! */
    if ( xmlp.main(sio,length_xml) < 0 ) {
	err_report(FUNC_NAME,"ERROR","xmlp.main() failed");
	goto quit;
    }

    if ( xmlp.is_fault() != false ) {
	err_report(FUNC_NAME,"ERROR","the server returns fault");
	goto quit;
    }

    if ( this->verbose() != false ) {
	c_fprintf(c_stderr(),"info: xmlparser__main() done.\n");
    }

    /* バイナリ・パート */

    if ( 0 < length_binary ) {
	const char *filename = written_file_or_dir;
	const char *dirname = NULL;
	const char *dirname_with_dot = NULL;
	if ( written_file_or_dir != NULL ) {
	    size_t len_written = c_strlen(written_file_or_dir);
	    if ( 0 < len_written ){
		if ( written_file_or_dir[len_written-1] == '/' ) {
		    dirname = written_file_or_dir;
		    filename = NULL;
		}
		else if ( written_file_or_dir[len_written-1] == '.' ) {
		    dirname_with_dot = written_file_or_dir;
		    filename = NULL;
		}
	    }
	}
	/* Server から指定されたファイル名を調べる */
	if ( filename == NULL ) {
	  for ( i=0 ; i < this->response_members_size_rec ; i++ ) {
	    if ( c_strcmp(this->response_members_rec[i].name,"filename") == 0 &&
		 this->response_members_rec[i].type == Ldsrpc__String_t ) {
		filename = this->response_members_rec[i].value;
	    }
	  }
	}
	/* バイナリファイルを書き出す */
	if ( filename != NULL ) {
	    stdstreamio fp_w;
	    long long len_left = length_binary;
	    tstring fullpath;
	    if ( dirname ) {
	        fullpath.assign(dirname);
		fullpath.append(filename);
	    }
	    else if ( dirname_with_dot != NULL ) {
		fullpath.assign(dirname_with_dot);
		fullpath.append("/");
		fullpath.append(filename);
	    }
	    else {
		fullpath.assign(filename);
	    }
	    if ( this->verbose() != false ) {
		c_fprintf(c_stderr(),"info: writing %s.\n",fullpath.cstr());
	    }

	    if ( fp_w.open("w",fullpath.cstr()) < 0 ) {
		err_report(FUNC_NAME,"ERROR","fp_w.open() failed");
		goto quit;
	    }
#define BUFSIZE 1048576
	    rw_buf = (char *)malloc(BUFSIZE);
	    if ( rw_buf == NULL ) {
		err_throw(FUNC_NAME,"FATAL","malloc() failed");
	    }
	    try {
	      while ( 0 < len_left ) {
		ssize_t read_size, sz;
		read_size = BUFSIZE < len_left ? BUFSIZE : len_left;
		sz = sio.read(rw_buf, read_size);
		if ( sz != read_size ) {
		    err_report(FUNC_NAME,"ERROR","sio.read() failed");
		    goto quit;
		}
		if ( 0 < sz ) {
		    if ( fp_w.write(rw_buf,sz) != sz ) {
			err_report(FUNC_NAME,"ERROR","fp_w.write() failed");
			goto quit;
		    }
		}
		len_left -= read_size;
		if ( this->verbose() != false ) {
		    c_fprintf(c_stderr(),"info: downloading... %.1lf %% left.\r",
			    (double)100*len_left/length_binary);
		}
	      }
	    }
	    catch (...) {
		if ( rw_buf != NULL ) free(rw_buf);
		err_throw(FUNC_NAME,"FATAL","r/w failed");
	    }
	    fp_w.close();
#undef BUFSIZE
	}
    }

    if ( this->verbose() != false ) {
	c_fprintf(c_stderr(),"info: downloading... done.                   \n");
    }

    return_status = 0;
 quit:
    sio.close();
    if ( rw_buf != NULL ) free(rw_buf);
    return return_status;
}
#undef FUNC_NAME

long ldsrpc::response_size() const
{
    return this->response_members_size_rec;
}

long ldsrpc::response_length() const
{
    return this->response_members_size_rec;
}

#define FUNC_NAME "respons"
const ldsrpc__member &ldsrpc::response( long index ) const
{
    if ( index < 0 || this->response_members_size_rec <= index ) {
        err_throw1(FUNC_NAME,"FATAL","response No.%ld is not found",index);
    }
    return this->response_members_rec[index];
}
#undef FUNC_NAME

//ldsrpc &ldsrpc::assign_verbose( bool val )
//{
//    this->verbose_rec = val;
//    return *this;
//}

bool &ldsrpc::verbose()
{
    return this->verbose_rec;
}

int ldsrpc::classlevel() const
{
    return this->classlevel_rec;
}

/* private */

#define FUNC_NAME "members_to_xml"
static int members_to_xml( const char *method_name,
			   long num_members, 
			   const ldsrpc__member *members,
			   tstring *ret_xml )
{
    int return_status = -1;
    long i, current_idx = 0;

    ret_xml->init();

    //c_fprintf(c_stderr(),"debug: called: members_to_xml()\n");

    ret_xml->assign("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
    ret_xml->append("<methodCall>");
    ret_xml->appendf("<methodName>%s</methodName>",method_name);
    ret_xml->append("<params><param><value>");

    for ( i=0 ; i < num_members ; i++ ) {
	int type = members[i].type;
	if ( i == 0 ) {
	    current_idx = members[i].index;
	    ret_xml->append("<struct>");
	}
	else if ( current_idx != members[i].index ) {
	    current_idx = members[i].index;
	    ret_xml->append("</struct><struct>");
	}
	ret_xml->appendf("<member><name>%s</name><value>",members[i].name);
	switch ( type ) {
	case Ldsrpc__String_t:
	    ret_xml->appendf("<string>%s</string>",members[i].value);
	    break;
	case Ldsrpc__Boolean_t:
	    ret_xml->appendf("<boolean>%s</boolean>",members[i].value);
	    break;
	case Ldsrpc__Int_t:
	    ret_xml->appendf("<int>%s</int>",members[i].value);
	    break;
	case Ldsrpc__Long_t:
	    ret_xml->appendf("<long>%s</long>",members[i].value);
	    break;
	case Ldsrpc__Double_t:
	    ret_xml->appendf("<double>%s</double>",members[i].value);
	    break;
	default:
	    err_report1(FUNC_NAME,"WARNING","unsupported type: %d",type);
	    break;
	}
	ret_xml->append("</value></member>");
    }
    if ( 0 < num_members ) {
        ret_xml->append("</struct>");
    }
    ret_xml->append("</value></param></params>");
    ret_xml->append("</methodCall>");

    //c_fprintf(c_stderr(),"debug: xml:[[%s]]\n",ret_xml->cstr());

    return_status = 0;

    return return_status;
}
#undef FUNC_NAME

#define FUNC_NAME "append_a_member"
static int append_a_member( ldsrpc__member **ret_members, 
			    long *ret_num_members,
			    long index, const char *name,
			    int type, const char *value )
{
    int return_status = -1;
    void *tmp_ptr;
    size_t sz;
    long target_idx = *ret_num_members;

    if ( name == NULL ) {
	name = "";
	err_report(FUNC_NAME,"WARNING","NULL name ??");
    }
    if ( value == NULL ) {
	value = "";
	err_report(FUNC_NAME,"WARNING","NULL value ??");
    }

    sz = sizeof(ldsrpc__member) * (target_idx + 1);
    tmp_ptr = realloc((*ret_members), sz);
    if ( tmp_ptr == NULL ) {
	err_throw(FUNC_NAME,"FATAL","realloc() failed");
    }
    (*ret_members) = (ldsrpc__member *)tmp_ptr;
    (*ret_num_members) ++;

    (*ret_members)[target_idx].index = index;
    (*ret_members)[target_idx].type = type;
    (*ret_members)[target_idx].name = c_strdup(name);
    (*ret_members)[target_idx].value = c_strdup(value);
    if ( (*ret_members)[target_idx].name == NULL ) {
	err_throw(FUNC_NAME,"FATAL","strdup() failed");
    }
    if ( (*ret_members)[target_idx].value == NULL ) {
	err_throw(FUNC_NAME,"FATAL","strdup() failed");
    }
    
    return_status = 0;
    // quit:
    return return_status;
}
#undef FUNC_NAME

#define FUNC_NAME "append_a_call_member"
ldsrpc &ldsrpc::append_a_call_member( long index, const char *name, 
				      int type, const char *value )
{
    int s;
    s = append_a_member( &this->call_members_rec, 
			 &this->call_members_size_rec,
			 index, name, type, value );
    if ( s < 0 ) {
	err_throw(FUNC_NAME,"FATAL","append_a_member() failed");
    }
    return *this;
}
#undef FUNC_NAME

#define FUNC_NAME "append_a_response_member"
ldsrpc &ldsrpc::append_a_response_member( long index, const char *name, 
					  int type, const char *value )
{
    int s;
    s = append_a_member( &this->response_members_rec, 
			 &this->response_members_size_rec,
			 index, name, type, value );
    if ( s < 0 ) {
	err_throw(FUNC_NAME,"FATAL","append_a_member() failed");
    }
    return *this;
}
#undef FUNC_NAME

ldsrpc &ldsrpc::clear_call_members()
{
    if ( this->call_members_rec != NULL ) {
	long i;
	for ( i=0 ; i < this->call_members_size_rec ; i++ ) {
	    if ( this->call_members_rec[i].name != NULL ) {
		free(this->call_members_rec[i].name);
	    }
	    if ( this->call_members_rec[i].value != NULL ) {
		free(this->call_members_rec[i].value);
	    }
	}
	free(this->call_members_rec);
	this->call_members_rec = NULL;
    }
    this->call_members_size_rec = 0;
    return *this;
}

ldsrpc &ldsrpc::clear_response_members()
{
    if ( this->response_members_rec != NULL ) {
	long i;
	for ( i=0 ; i < this->response_members_size_rec ; i++ ) {
	    if ( this->response_members_rec[i].name != NULL ) {
		free(this->response_members_rec[i].name);
	    }
	    if ( this->response_members_rec[i].value != NULL ) {
		free(this->response_members_rec[i].value);
	    }
	}
	free(this->response_members_rec);
	this->response_members_rec = NULL;
    }
    this->response_members_size_rec = 0;
    return *this;
}

}	/* namespace sli */

#include "private/c_stderr.c"
#include "private/c_fprintf.c"
#include "private/c_snprintf.c"

#include "private/c_memset.cc"
#include "private/c_strdup.c"
#include "private/c_strchr.cc"
#include "private/c_strlen.cc"
#include "private/c_strcat.c"
#include "private/c_strcpy.c"
#include "private/c_strspn.c"
#include "private/c_strncmp.cc"
#include "private/c_strcmp.cc"

#include "private/c_gethostname.c"
#include "private/c_geteuid.c"
