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
#include "exec.c"
#include "behavior.c"
#include "parse.c"

int run(char *source){
    printf("---------------------------------------------------->\n%s\n", source);
    QrtCtx *ctx = parse(source);
    exec(ctx);
    return 0;
}

int main(){
    char *v = ":y 3\n :run {\n write y\n write x*2 \n}\n :z = run :x 5 :y 10; :hightlight 23 :play { y + 3  * { x + 3 + 9 } + 2 }\n ";
    char *source = ":y 3 :x 2; :z 7\n :j 25 ";
    char *s = "\n:run { write x :z 5 write z} :alpha { 2 + {:j 13 :i 0 {7*1 {write use save out run p}}}}";
    char *z = ":javalin 0, :run { write x, write 2*4*y, &javalin y} :juliet 7 + 2 + 4, run :x 2 :y 3";
    char *x = ":x 3 :y 3; :z * x y\n :out {\nwrite h\nwrite j \n} out :h 10\n:min {\n .x 10 :z 0 if { x < z } then {x} else {z}\n} min :z 5; min :z 17\n &x 5 &y 10";


    /* basic arithmetic and variables */
    /*
    run("* 5 7");
    run("* 5 7 12"); 
    run("+ 1 2 4"); 
    run("- 10 5"); 
    run(":x 10\n+ x 13"); 
    run(":y 8\n&y 10\n- 25 y");
    */
    /* function literals and pointers*/
    run("{+ 10 x} :x 3\n");
    run("{+ 10 x} :x 3\n- 10 20");
    run("{+ x { - 7 15}:} :x 3\n- 10 20");
    /*
    run(":run {* :x :y }\nrun :x 1 :y 2\n:ptr run\nptr :x 100 :h 3");
    */
    /* lexical scoped symbols*/

    return 0;
}
