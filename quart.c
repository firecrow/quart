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

int is_punc(char c){
    return (c == ';' || c == ',' || c == '<' || c == '>' || c == '=' || c == '{' || c == '}' || c == '[' || c == ']');
}

int is_cmp(char c){
    return (c == '*' || c == '+' || c == '-' || c == '!' || c == '/' || c == '>' || c == '<');
}

int is_block(char c){
    return (c == '{' || c == '}' || c == '[' || c == ']' || c == ',');
}

int is_sep(char c){
    return (c == ';' || c == '\n');
}

int is_alpha(char c){
    return (c >= 65 && c <= 90) || (c >=97 && c <= 122);
}

int is_numeric(char c){
    return (c >= 48 && c <= 57);
}

int is_alpha_numeric(char c){
    return is_numeric(c) || is_alpha(c);
}

enum qrt_opp_types {
    QRT_PLUS = '+',
    QRT_MINUS = '-',
    QRT_DIVIDE = '/',
    QRT_MULTIPLY = '*',
    QRT_GREATER = '>',
    QRT_LESS = '<',
    QRT_NOT = '!'
};

int is_between_opp_type(char type){
    return type != QRT_NOT;
}

typedef struct qrt_cell {
    struct base base;
    int status;
    int id;
    CtlAbs *value;
    struct qrt_cell *next;
    struct qrt_cell *previous;
} CtlCell;

struct qrt_statement;
typedef struct qrt_block {
    struct base base;
    struct qrt_block *parent;
    struct qrt_statement *root;
    struct qrt_statement *next;
    CtlTree *namespace;
    CtlTree *values;
    char type;
} QrtBlock;

typedef struct qrt_statement {
    struct base base;
    struct qrt_block *parent;
    struct qrt_statement *previous;
    CtlCell *root;
    CtlCell *next;
} QrtStatement;

struct qrt_ctx {
    struct base base;
    QrtBlock *root;
    QrtBlock *next;
    CtlCounted *shelf;
};

typedef struct qrt_opp {
    struct base base;
    char opp_type;
    struct qrt_cell *(*call)(struct qrt_opp *opp, CtlAbs *a, CtlAbs *b);
} QrtOpp;

typedef struct qrt_symbol {
   struct base base;
   CtlCell *parent;
   CtlCounted *name;
   CtlAbs *value;
   int is_define;
} QrtSymbol;

typedef struct qrt_sep {
   struct base base;
} QrtSep;

QrtSep *qrt_sep_alloc(){
    QrtSep *sep;
    ctl_xptr(sep = malloc(sizeof(QrtSep)));
    bzero(sep, sizeof(QrtSep));
    sep->base.class = CLASS_SEP;
    return sep;
}

int qrt_opp_id=0;
QrtOpp *qrt_opp_alloc(char type){
    QrtOpp *opp;
    ctl_xptr(opp = malloc(sizeof(QrtOpp)));
    bzero(opp, sizeof(QrtOpp));
    opp->base.class = CLASS_OPP;
    opp->base.id = ++qrt_opp_id;
    opp->opp_type = type;
    return opp;
}

int qrt_symbol_id=0;
QrtSymbol *qrt_symbol_alloc(CtlCell *parent, int is_define){
    QrtSymbol *symbol;
    ctl_xptr(symbol = malloc(sizeof(QrtSymbol)));
    bzero(symbol, sizeof(QrtSymbol));
    symbol->base.class = CLASS_SYMBOL;
    symbol->base.id = ++qrt_symbol_id;
    symbol->is_define = is_define;
    symbol->parent = parent;
    return symbol;
}

int qrt_cell_id=0;
struct qrt_cell *qrt_cell_alloc(){
    struct qrt_cell *node;
    ctl_xptr(node = malloc(sizeof(struct qrt_cell)));
    bzero(node, sizeof(struct qrt_cell));
    node->base.class = CLASS_CELL;
    node->base.id = ++qrt_cell_id;
    node->status = CTL_NOT_STARTED;
    return node;
}

QrtStatement *qrt_statement_alloc(QrtBlock *parent, QrtStatement *previous){
    QrtStatement *statement;
    ctl_xptr(statement = malloc(sizeof(QrtStatement)));
    statement->base.class = CLASS_STATEMENT;
    statement->previous = previous;
    statement->root = statement->next = qrt_cell_alloc();
    return statement;
}

QrtBlock *qrt_block_alloc(char type, QrtBlock *parent){
    QrtBlock *block;
    ctl_xptr(block = malloc(sizeof(QrtBlock)));
    block->base.class = CLASS_BLOCK;
    block->namespace = ctl_tree_alloc(ctl_tree_counted_cmp);
    block->values = ctl_tree_alloc(ctl_tree_counted_cmp);
    block->root = block->next = qrt_cell_alloc();
    block->root = block->next = qrt_statement_alloc(block, NULL);
    block->type = type;
    return block;
}

int qrt_ctx_id = 0;
struct qrt_ctx * qrt_ctx_alloc(){
    struct qrt_ctx *ctx;
    ctl_xptr(ctx = malloc(sizeof(struct qrt_ctx)));
    bzero(ctx, sizeof(struct qrt_ctx));
    ctx->base.class = CLASS_CTX;
    ctx->base.id = ++qrt_ctx_id;
    ctx->root = ctx->next = qrt_block_alloc('{', NULL);
    return ctx;
}

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

CtlCell *multiply_call(QrtOpp *opp, CtlAbs *a, CtlAbs *b){
    CtlCell *node = qrt_cell_alloc();
    if(a->base.class != CLASS_INT || b->base.class != CLASS_INT){ 
        node->status = CTL_INVALID; 
    }else{
        asCtlInt(node->value)->value = asCtlInt(a)->value * asCtlInt(b)->value;
        node->status = CTL_COMPLETE; 
    }
    return node;
}

void handle_token(struct qrt_ctx *ctx, CtlCounted *name){
    QrtStatement *statement = ctx->next->next;
    /*
    struct qrt_cell *current = ctx->next->next;
    QrtBlock *block;

    struct qrt_cell *node = qrt_cell_alloc();
    enum classes token_type = identify_token(name);
    if(token_type == CLASS_OPP){
        QrtOpp *value = qrt_opp_alloc(name->data[0]);
        switch(value->opp_type){
            case QRT_MULTIPLY:
               value->call = multiply_call;
               break;
        }
        node->value = (CtlAbs *)value; 
    }else if(token_type == CLASS_BLOCK){
        block = qrt_block_alloc(name->data[0], ctx->next);
        node->value = (CtlAbs *)block; 
    }else if(token_type == CLASS_INT){
        node->value = (CtlAbs *)ctl_int_alloc(atoi(ctl_counted_to_cstr(name)));
    }else if(token_type == CLASS_SEP){
        node->value = (CtlAbs *)qrt_sep_alloc();
    }else if(token_type == CLASS_COUNTED){

    }else if(token_type == CLASS_SYMBOL || token_type == CLASS_DEFINE){
        QrtSymbol *symbol = qrt_symbol_alloc(node, token_type == CLASS_DEFINE); 
        symbol->name = name;
        node->value = (CtlAbs *)symbol; 
    }
    node->previous = current;
    current->next = node;
    ctx->next->next = node;
    if(token_type == CLASS_BLOCK){
        block->parent = ctx->next;
        ctx->next = block;
    }
    */
}

struct qrt_ctx *parse(char *source){
    char *p = source;
    struct qrt_ctx *ctx = qrt_ctx_alloc();
    ctx->shelf = ctl_counted_alloc(NULL, 0);

    if(p == '\0') 
        return NULL;
    do {
        if(*p == ':'){
            ctl_counted_push(ctx->shelf, p, 1);
        }else{
            if((ctx->shelf->length == 0 && is_alpha(*p)) || is_alpha_numeric(*p)){
                ctl_counted_push(ctx->shelf, p, 1);
            }else if(ctx->shelf->length > 0){
                handle_token(ctx, ctx->shelf); 
                ctx->shelf = ctl_counted_alloc(NULL, 0);
            }
            if(is_cmp(*p) || is_block(*p) || is_sep(*p)){
                handle_token(ctx, ctl_counted_alloc(p, 1));
            }
        }
    }while(*++p != '\0');
    return ctx;
}

char *get_class_str(CtlAbs *value){
    int class = value->base.class;
    if(class == CLASS_INT){
        return "INT";
    }else if(class == CLASS_BLOCK){
        QrtBlock *block = (QrtBlock *)value;
        if(block->type == '{') return "BLOCK_OPEN";
        else return  "BLOCK_CLOSE";
    }else if(class == CLASS_COUNTED){
        return "COUNTED";
    }else if(class == CLASS_SEP){
        return "SEPORATOR";
    }else if(class == CLASS_OPP){
        return "OPERATOR";
    }else if(class == CLASS_SEP){
        return "SPORATORo";
    }else if(class == CLASS_SYMBOL){
        QrtSymbol *symbol = (QrtSymbol *)value;
        if(symbol->is_define) return "DEFINITION";
        else return "SYMBOL";
        return "POO";
    }else if(class == CLASS_CELL){
        return "CELL";
    }
    return "UNKNOWN";
}

void print_node(struct qrt_cell *node, CtlCounted *space){
    char *node_value = "";
    int next_id = node->next != NULL ? node->next->base.id  : -1;
    int prev_id = node->previous != NULL ? node->previous->base.id  : -1;
    int class = node->base.class;
    struct qrt_cell *cell = node;
    if(node->value){
        if(node->value->base.class == CLASS_BLOCK){
            node_value = ctl_counted_to_cstr(ctl_counted_format("%c", ((QrtBlock *)node->value)->type));
        }else if(node->value->base.class == CLASS_OPP){
            node_value = ctl_counted_to_cstr(ctl_counted_format("%c", ((QrtOpp *)node->value)->opp_type));
        }else if(node->value->base.class == CLASS_INT){
            node_value = ctl_counted_to_cstr(ctl_counted_format("%d", asCtlInt(node->value)->value));
        }else if(node->value->base.class == CLASS_SYMBOL){
            QrtSymbol *symbol = (QrtSymbol *)node->value;
            node_value = ctl_counted_to_cstr(symbol->name);
            if(symbol->value){
                CtlAbs *value = symbol->value;
                if(value->base.class == CLASS_INT){
                    node_value = ctl_counted_to_cstr(ctl_counted_format("%s %d", node_value, asCtlInt(value)->value));
                }
            }
        }
        class = node->value->base.class;
        cell = (struct qrt_cell *)node->value;
    }
    printf("%s<%s %s>\n",
        ctl_counted_to_cstr(space), get_class_str((CtlAbs *)cell), node_value
    );
}

int is_variable_value(CtlAbs *value){
    switch(value->base.class){
        case CLASS_INT:
        case CLASS_COUNTED:
        /*case CLASS_BLOCK:*/
            return 1;
    }
    return 0;
}

void qrt_out(void *_n){
    struct node *n = (struct node *)_n;
    CtlCounted *key = (CtlCounted *)n->key;
    if(n->is_red) printf("\x1b[31m%s -> %s\x1b[0m", ctl_counted_to_cstr(key),get_class_str(n->data));
    else printf("%s -> %s", ctl_counted_to_cstr(key), get_class_str(n->data));
}

/*
int printout(struct qrt_ctx * ctx){
    QrtBlock *block = ctx->root;
    struct qrt_cell *node = block->root->root;
    CtlCounted *space = ctl_counted_alloc("                    ", 20);
    space->length = 0;

    do {
        if(node->value){
            if(node->value->base.class == CLASS_BLOCK){
                print_node(node, space);
                if(block->type == '{'){
                   block = (QrtBlock *)node->value;
                   space->length+=4;
                   node = block->root;
                }else{
                   node = block->root->next;
                   block = block->parent;
                   space->length-= 4;
                }
            } 
        }
        if(!node){
            break;
        }
        print_node(node, space);
    }while(node && (node = node->next));
    printf("\x1b[35m---------------------------------------------\x1b[0m\n");
    return 0;

}

int exec(struct qrt_ctx * ctx){


    struct treeprint_head *tph;
    ctl_xptr(tph = malloc(sizeof(struct treeprint_head)));
    bzero(tph, sizeof(struct treeprint_head));
    tph->get_left = get_left;
    tph->get_right = get_right;
    tph->get_parent = get_parent;
    tph->out = qrt_out;

    QrtBlock *block = ctx->root;
    struct qrt_cell *node = block->root;
    CtlCounted *space = ctl_counted_alloc("                    ", 20);
    space->length = 0;


    do {
        if(node->value){
            if(node->value->base.class == CLASS_BLOCK){
                print_node(node, space);
                if(block->type == '{'){
                   block = (QrtBlock *)node->value;
                   space->length+=4;
                   node = block->root;
                }else{
                   if(!ctl_tree_empty(block->namespace)){
                       ct_tree_print(tph, block->namespace->root);
                   }
                   node = block->root->next;
                   block = block->parent;
                   space->length-= 4;
                }

            }else if(node->value->base.class == CLASS_SYMBOL){
                QrtSymbol *symbol = (QrtSymbol *)node->value;
                if(symbol->is_define && node->next && node->next->value && is_variable_value(node->next->value)){
                    CtlAbs *value = node->next->value;
                    symbol->value = value; 
                    node = node->next;
                    ctl_tree_insert(block->namespace, symbol->name, value);
                }
            }
        }
        if(!node){
            break;
        }
        print_node(node, space);
    }while(node && (node = node->next));
    if(!ctl_tree_empty(block->namespace)){
       ct_tree_print(tph, block->namespace->root);
    }
    return 0;
}
*/


int main(){
    char *sourcev = ":y 3\n :run {\n write y\n write x*2 \n}\n :z = run :x 5 :y 10 :hightlight 23 :play { y + 3  * { x + 3 + 9 } + 2 }\n ";
    char *source = ":y 3 :x 2; :z 7\n :j 25 ";
    printf("source\n%s\n---------------\n", sourcev);
    struct qrt_ctx *ctx = parse(sourcev);

    /*printout(ctx);*/
    /*exec(ctx);*/
    return 0;
}
