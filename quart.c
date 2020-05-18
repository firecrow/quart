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
    char *s = "\n:run { write x :z 5 write z} :alpha { 2 + {:j 13 :i 0 {7*1 {write use save out run p}}}}";
    char *z = ":javalin 0, :run { write x, write 2*4*y, &javalin y} :juliet 7 + 2 + 4, run :x 2 :y 3";
    printf("source\n%s\n---------------\n",z);

    QrtCtx *ctx = build(z);
    CtlAbs *reg = exec(ctx);
    print_blocks(ctx);
    return 0;
}
