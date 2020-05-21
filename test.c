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
#include "../crowtils/tested.c" 
#include "core.c"
#include "utils.c"
#include "debug.c"

CtlCounted *token_out;
Crray *tokens;
QrtCell *make_token_mock(struct qrt_ctx *ctx, CtlCounted *name, QrtCell *current){
    ctl_crray_push(tokens, (CtlAbs *)name);
    return NULL; 
}

#include "parse.c"
#include "build.c"
#include "exec.c"
 

/*parse.c*/
/* basic parse and emitting 
    enum classes identify_token(CtlCounted *token){
    QrtCell *make_token(struct qrt_ctx *ctx, CtlCounted *name, QrtCell *current){
    struct qrt_ctx *parse(char *source){
*/
/* token identifyer */
/* token cell creator*/

/*utility functions */

/* break chain */
/* push/pop block */
/* push/pop statement */

/* build cell chain */
/* associating values */
/* parse by block */
/* parse by statement */
/* full tier test with all the variables */

int main(){
  tokens = ctl_crray_alloc(16);

  /* parse.c:parse */
  parse(":hi"); 
  printf("%zu\no",tokens->length); 
  printf("%s\n", ctl_counted_to_cstr(tokens->content[0]));
  handle_case(tokens->length == 1, "first token parsed length 1", tokens->length, "");
  char *str = ctl_counted_to_cstr(asCtlCounted(tokens->content[0]));
  handle_case(strcmp(":hi", str) == 0, "content of symbol parsed is accurate", 0, str);

  tokens = ctl_crray_alloc(16);
  parse(":hi 3"); 
  handle_case(tokens->length == 2, "second token parsed should have lenght 2", tokens->length, "");
  str = ctl_counted_to_cstr(asCtlCounted(tokens->content[1]));
  handle_case(strcmp("3", str) == 0, "content of int parsed is accurate", 0, str);

  tokens = ctl_crray_alloc(16);
  parse(":hi 3 :run { write x\nwrite y}; run :x 3 :y 4 "); 
  str = ctl_counted_to_cstr(asCtlCounted(tokens->content[2]));
  handle_case(strcmp(":run", str) == 0, "content of define is accurate", 0, str);

  str = ctl_counted_to_cstr(asCtlCounted(tokens->content[3]));
  handle_case(strcmp("{", str) == 0, "content of block open is accurate", 0, str);
  
  str = ctl_counted_to_cstr(asCtlCounted(tokens->content[9]));
  handle_case(strcmp("}", str) == 0, "content of block close parsed is accurate", 0, str);
}
