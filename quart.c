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


int main(){
    char *sourcev = ":y 3\n :run {\n write y\n write x*2 \n}\n :z = run :x 5 :y 10; :hightlight 23 :play { y + 3  * { x + 3 + 9 } + 2 }\n ";
    char *source = ":y 3 :x 2; :z 7\n :j 25 ";
    printf("source\n%s\n---------------\n", sourcev);

    QrtCtx *ctx = build(sourcev);
    CtlAbs *reg = exec(ctx);
    print_blocks(ctx);
    return 0;
}
