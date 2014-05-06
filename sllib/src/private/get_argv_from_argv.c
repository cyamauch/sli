#include <stdlib.h>
#include <string.h>

/*
  [example]

  const char **editor_argv = 
      get_argv_from_argv(argv,filename);
  execvp( *editor_argv, editor_argv );
  free(editor_argv);
*/

static const char **get_argv_from_argv( const char *const argv[],
					const char *filename )
{
    const char **return_argv = NULL;
    int i,cnt;
    if ( argv != NULL ) {
	for ( cnt=0 ; argv[cnt] != NULL ; cnt++ );
	return_argv = (const char **)malloc(sizeof(*return_argv)*(cnt+1+1));
	if ( return_argv == NULL ) goto quit;
	for ( i=0 ; i<cnt ; i++ ) {
	    return_argv[i] = argv[i];
	}
	return_argv[i] = (char *)filename;
	i++;
	return_argv[i] = NULL;
    }
 quit:
    return return_argv;
}

