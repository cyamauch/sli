#include <stdio.h>

int main()
{
    printf("#ifndef _SLI_SEEK_H\n");
    printf("#define _SLI_SEEK_H 1\n");
    printf("\n");
    printf("#ifndef SEEK_SET\n");
    printf("#define SEEK_SET (%d)\n",SEEK_SET);
    printf("#endif\n");
    printf("#ifndef SEEK_CUR\n");
    printf("#define SEEK_CUR (%d)\n",SEEK_CUR);
    printf("#endif\n");
    printf("#ifndef SEEK_END\n");
    printf("#define SEEK_END (%d)\n",SEEK_END);
    printf("#endif\n");
    printf("\n");
    printf("#endif  /* _SLI_SEEK_H */\n");

    return 0;
}
