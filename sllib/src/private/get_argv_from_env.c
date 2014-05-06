#include <stdlib.h>
#include <string.h>

/*
  [example]

  char **editor_argv = 
      get_argv_from_env("EDITOR",DEFAULT_EDITOR,filename);
  execvp( *editor_argv, editor_argv );
  free(editor_argv[0]);
  free(editor_argv);
*/

static char **get_argv_from_env( const char *env_keyword, 
				 const char *default_value,
				 const char *filename )
{
    char **return_argv = NULL;
    char *buf = NULL;
    const char *env_p;
    const char *env_val;

    env_p = getenv(env_keyword);
    if ( env_p == NULL ) {
        env_p = default_value;
    }

    if ( env_p != NULL ) {
	for ( ; *env_p == ' ' ; env_p++ );	/* skip space */
    }

    if ( env_p != NULL && 0 < strlen(env_p) ) env_val = env_p;
    else env_val = default_value;
      
    /* parsing environment variable and constructing argv */
    if ( env_val != NULL && 0 < strlen(env_val) ) {
        char *p;
	int i,cnt;
        buf = strdup(env_val);
	if ( buf == NULL ) goto quit;
	/* get the number of argv */
	for ( cnt=1,p=buf ; (p=strchr(p,' ')) != NULL ; cnt++ ) {
	    for ( p++ ; *p == ' ' ; p++ );
	}
	return_argv = (char **)malloc(sizeof(*return_argv)*(cnt+1+1));
	if ( return_argv == NULL ) goto quit;
	/* setting argv */
	p=buf;
	return_argv[0] = p;
	for ( i=1 ; i<cnt ; i++ ) {
	    p=strchr(p,' ');
	    *p = '\0';
	    for ( p++ ; *p == ' ' ; p++ );
	    if ( *p == '\0' ) break;
	    return_argv[i] = p;
	}
	return_argv[i] = (char *)filename;
	i++;
	return_argv[i] = NULL;
	//for ( i=0 ; return_argv[i] != NULL ; i++ ) {
	//    fprintf(stderr,"debug: [%s]\n",return_argv[i]);
	//}
    }
 quit:
    if ( return_argv == NULL ) {
        if ( buf != NULL ) free(buf);
    }
    return return_argv;
}

