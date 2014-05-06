#include <stdio.h>

int main()
{
    printf("#ifndef _SLI_EOF_H\n");
    printf("#define _SLI_EOF_H 1\n");
    printf("\n");
    printf("#ifndef EOF\n");
    printf("#define EOF (%d)\n",EOF);
    printf("#endif\n");
    printf("\n");
    printf("#endif  /* _SLI_EOF_H */\n");

    return 0;
}

