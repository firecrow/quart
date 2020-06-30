/* Copyright 2020 Firecrow Silvernight (fire@firecrow.com) licensed under the MIT License see LICENSE file */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "../crowtils/crowx.c"
#include "../crowtils/poly.c"
#include "../crowtils/counted.c"
#include "../crowtils/crownumber.c"
#include "../crowtils/crray.c"
#include "../crowtils/tree.c"
#include "../crowtils/treeprint.c"
#include "core.c"
#include "utils.c"
#include "debug.c"
#include "build.c"
#include "exec.c"
#include "behavior.c"
#include "parse.c"

int run(char *source){
    printf("---------------------------------------------------->\n%s\n", source);
    QrtCtx *ctx = parse(source);
    build(ctx);
    printf("---------------------------------------------------->\n%s\n", source);
    exec(ctx);
    printf("---------------------------------------------------->\n%s\n", source);
    return 0;
}

#define BUFFLEN 16384
int main(){
    char buff[BUFFLEN];
    int len = fread(&buff, 1, BUFFLEN-1, stdin); 
    buff[len] = '\0';
    run(buff);
    return 0;
}
