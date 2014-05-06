#ifndef _ERR_REPORT_H
#define _ERR_REPORT_H 1

#include <sli/sli_funcs.h>

#ifdef NOUSETHROW

#define err_report(func,level,message) { \
	sli__eprintf(CLASS_NAME "::%s(): [%s] ",func,level); \
	sli__eprintf(message); \
	sli__eprintf("\n"); \
}
#define err_report1(func,level,message,arg) { \
	sli__eprintf(CLASS_NAME "::%s(): [%s] ",func,level); \
	sli__eprintf(message,arg); \
	sli__eprintf("\n"); \
}

#else

#define err_report(func0,level0,message0) { \
	sli__eprintf(CLASS_NAME "::%s(): [%s] ",func0,level0); \
	sli__eprintf(message0); \
	sli__eprintf("\n"); \
        if ( sli__strcmp(level0,"FATAL")==0 ) { \
          sli::err_rec st; \
          sli__snprintf(st.class_name,64,"%s",CLASS_NAME); \
          sli__snprintf(st.func_name,64,"%s",func0); \
          sli__snprintf(st.level,64,"%s",level0); \
          sli__snprintf(st.message,128,"%s",message0); \
          throw st; \
        } \
}
#define err_report1(func0,level0,message0,arg0) { \
	sli__eprintf(CLASS_NAME "::%s(): [%s] ",func0,level0); \
	sli__eprintf(message0,arg0); \
	sli__eprintf("\n"); \
        if ( sli__strcmp(level0,"FATAL")==0 ) { \
          sli::err_rec st; \
          sli__snprintf(st.class_name,64,"%s",CLASS_NAME); \
          sli__snprintf(st.func_name,64,"%s",func0); \
          sli__snprintf(st.level,64,"%s",level0); \
          sli__snprintf(st.message,128,message0,arg0); \
          throw st; \
        } \
}

#endif

#define err_throw(func0,level0,message0) \
	sli__eprintf(CLASS_NAME "::%s(): [%s] ",func0,level0); \
	sli__eprintf(message0); \
	sli__eprintf("\n"); \
        { \
          sli::err_rec st; \
          sli__snprintf(st.class_name,64,"%s",CLASS_NAME); \
          sli__snprintf(st.func_name,64,"%s",func0); \
          sli__snprintf(st.level,64,"%s",level0); \
          sli__snprintf(st.message,128,"%s",message0); \
          throw st; \
        }

#define err_throw1(func0,level0,message0,arg0) \
	sli__eprintf(CLASS_NAME "::%s(): [%s] ",func0,level0); \
	sli__eprintf(message0,arg0); \
	sli__eprintf("\n"); \
        { \
          sli::err_rec st; \
          sli__snprintf(st.class_name,64,"%s",CLASS_NAME); \
          sli__snprintf(st.func_name,64,"%s",func0); \
          sli__snprintf(st.level,64,"%s",level0); \
          sli__snprintf(st.message,128,message0,arg0); \
          throw st; \
        }

#endif	/* _ERR_REPORT_H */
