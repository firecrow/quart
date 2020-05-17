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

QrtCtx *blocks(QrtCtx *ctx){
    QrtBlock *block = ctx->root;
    QrtBlock *newblock;
    QrtCell *start = ctx->start;
    QrtCell *before = start;
    QrtCell *cell = before;
    QrtCell *next = cell;
    while(cell){
        if(cell->value && cell->value->base.class == CLASS_BLOCK){
            QrtBlock *newblock = (QrtBlock *)cell->value; 
            QrtCell *insert = qrt_cell_alloc();
            if(newblock->type == '{'){
                insert->value = cell;  
                insert->previous = cell->previous;
                cell->previous->next = insert;
                before = insert;
            }else{
                before->next = cell->next;
                cell->next = NULL;   
                cell = before->next;
                continue;
            }
        }
        cell = cell->next;
    }
    return ctx;
}

/*
QrtCtx *statements(QrtBlock *block){
    QrtStatement *stmt = block->statement_root;
    QrtStatement *newstmt;
    QrtCell *start = ctx->start;
    QrtCell *before = start;
    QrtCell *cell = before;
    while(cell){
        if(is_break_value(cell->value)){
            newstmt = qrt_statement_alloc(NULL, stmt, before);
            stmt->next = newstmt;
            stmt = newstmt;
            before = cell->next;
            cell->next = NULL;
            cell = before;
            continue;
        }
        cell = cell->next;
    }
    return ctx;
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
    char *sourcev = ":y 3\n :run {\n write y\n write x*2 \n}\n :z = run :x 5 :y 10; :hightlight 23 :play { y + 3  * { x + 3 + 9 } + 2 }\n ";
    char *source = ":y 3 :x 2; :z 7\n :j 25 ";
    printf("source\n%s\n---------------\n", sourcev);

    struct qrt_ctx *ctx = parse(sourcev);
    print_sequence(ctx);
    printf("------sequence--------\n");
    ctx = blocks(ctx);
    print_blocks(ctx);
    /*statements(ctx)*/
    /*exec(ctx);*/
    return 0;
}
