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



/*
int printout(struct qrt_ctx * ctx){
    QrtBlock *block = ctx->root;
    QrtBlock *newblock = NULL;
    QrtStatement *prev_statement;
    struct qrt_cell *node = block->root->root;
    CtlCounted *space = ctl_counted_alloc("                    ", 20);
    struct qrt_cell *latest = node;
    space->length = 0;

    do {
        printf("open\n");
        if(node->value){
            if(node->value->base.class == CLASS_BLOCK){
                print_node(node, space);
                newblock = node->value;
                if(newblock->type == '{'){
                   space->length+=4;
                   print_node(newblock->root, space);
                   node = newblock->root->root;
                   continue;
                }else{
                   node = block->root->next;
                   block = block->parent;
                   space->length-= 4;
                }
            } 
            if(node->value->base.class == CLASS_SEP){
                QrtStatement *statement = qrt_statement_alloc(block, prev_statement, prev_statement->root->next);
                prev_statement = statement;
            }
            printf("found statement 2\n");
        }
        if(!node){
            break;
        }
        printf("found statement 3\n");
        print_node(node, space);
    }while(node && (node = node->next));
    printf("\x1b[35m---------------------------------------------\x1b[0m\n");
    return 0;

}
*/

enum classes identify_token(CtlCounted *token){
    int i = 0;
    char c; 
    int class = CLASS_NULL;
    for(int i = 0; i < token->length; i++){
        c = token->data[i]; 
        if(i == 0){
            if(is_numeric(c)){
                class = CLASS_INT;
            } 
            if(is_alpha(c)){
                class = CLASS_SYMBOL;
            }
            if(c == ':'){
                class = CLASS_DEFINE;
                continue;
            }
            if(c == '"'){
                class = CLASS_COUNTED;
                if(token->data[token->length-1] != '"')
                    return CLASS_INVALID;
                continue;
            }
            if(is_cmp(c)){
                return CLASS_OPP;
            } 
            if(c == '{' || c == '}'){
                return CLASS_BLOCK;
            }
            if(c == ';' || c == '\n'){
                return CLASS_SEP;
            }
        }
        if(class == CLASS_INT){
            if(!is_numeric(c)){
                return CLASS_INVALID;
            }
        }
        if(class == CLASS_SYMBOL || class == CLASS_DEFINE){
            if(!is_alpha_numeric(c)){
                return CLASS_INVALID;
            }
        }
        if(class == CLASS_COUNTED){
            if(c == '"' && token->data[i-1] != '\\' && i != token->length-1)
                continue;
            return CLASS_INVALID;

        }
    }
    return class;
}

QrtCell *sequence_tokens(struct qrt_ctx *ctx, CtlCounted *name, QrtCell *current){
    struct qrt_cell *node = qrt_cell_alloc();
    enum classes token_type = identify_token(name);
    if(token_type == CLASS_OPP){
        node->value = (CtlAbs *)qrt_opp_alloc(name->data[0]); 
    }else if(token_type == CLASS_BLOCK){
        node->value = (CtlAbs *)qrt_block_alloc(name->data[0], NULL); 
    }else if(token_type == CLASS_INT){
        node->value = (CtlAbs *)ctl_int_alloc(atoi(ctl_counted_to_cstr(name)));
    }else if(token_type == CLASS_SEP){
        node->value = (CtlAbs *)qrt_sep_alloc();
    }else if(token_type == CLASS_COUNTED){
        node->value = (CtlAbs *)name;
    }else if(token_type == CLASS_SYMBOL || token_type == CLASS_DEFINE){
        QrtSymbol *symbol = qrt_symbol_alloc(node, token_type == CLASS_DEFINE); 
        symbol->name = name;
        node->value = (CtlAbs *)symbol; 
    }

    current->next = node;
    node->previous = current;
    return node;
}

struct qrt_ctx *parse(char *source){
    char *p = source;
    struct qrt_ctx *ctx = qrt_ctx_alloc();
    ctx->shelf = ctl_counted_alloc(NULL, 0);
    struct qrt_cell *cell = qrt_cell_alloc();
    ctx->start = cell;

    if(p == '\0') 
        return NULL;
    do {
        if(*p == ':'){
            ctl_counted_push(ctx->shelf, p, 1);
        }else{
            if((ctx->shelf->length == 0 && is_alpha(*p)) || is_alpha_numeric(*p)){
                ctl_counted_push(ctx->shelf, p, 1);
            }else if(ctx->shelf->length > 0){
                cell = sequence_tokens(ctx, ctx->shelf, cell); 
                ctx->shelf = ctl_counted_alloc(NULL, 0);
            }
            if(is_cmp(*p) || is_block(*p) || is_sep(*p)){
                cell = sequence_tokens(ctx, ctl_counted_alloc(p, 1), cell);
            }
        }
    }while(*++p != '\0');
    return ctx;
}


int main(){
    char *sourcev = ":y 3\n :run {\n write y\n write x*2 \n}\n :z = run :x 5 :y 10 :hightlight 23 :play { y + 3  * { x + 3 + 9 } + 2 }\n ";
    char *source = ":y 3 :x 2; :z 7\n :j 25 ";
    printf("source\n%s\n---------------\n", sourcev);

    struct qrt_ctx *ctx = parse(sourcev);
    print_sequence(ctx);
    /*exec(ctx);*/
    return 0;
}
