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

int is_punc(char c){
    return (c == ';' || c == ',' || c == '<' || c == '>' || c == '=' || c == '{' || c == '}' || c == '[' || c == ']');
}

int is_cmp(char c){
    return (c == '*' || c == '+' || c == '-' || c == '!' || c == '/' || c == '>' || c == '<');
}

int is_block(char c){
    return (c == '{' || c == '}' || c == '[' || c == ']' || c == ',');
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

struct qrt_ctx {
    struct base base;
    CtlTree *namespace;
    CtlCounted *shelf;
    struct qrt_ctx *parent;
    struct qrt_cell *start;
    struct qrt_cell *next;
    struct qrt_cell *reg;
};

typedef struct qrt_cell {
    struct base base;
    int status;
    int id;
    CtlAbs *value;
    struct qrt_cell *next;
    struct qrt_cell *previous;
} CtlCell;

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

int qrt_ctx_id = 0;
struct qrt_ctx * qrt_ctx_alloc(){
    struct qrt_ctx *ctx;
    ctl_xptr(ctx = malloc(sizeof(struct qrt_ctx)));
    bzero(ctx, sizeof(struct qrt_ctx));
    ctx->base.class = CLASS_BLOCK;
    ctx->base.id = ++qrt_ctx_id;
    ctx->namespace = ctl_tree_alloc(ctl_tree_crownumber_int_cmp);
    ctx->next = ctx->start = qrt_cell_alloc();
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
    printf("token(%d):%s\n", identify_token(name), ctl_counted_to_cstr(name));
    struct qrt_cell *current = ctx->next;

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
        ;
    }else if(token_type == CLASS_INT){
        node->value = (CtlAbs *)ctl_int_alloc(atoi(ctl_counted_to_cstr(name)));
    }else if(token_type == CLASS_COUNTED){

    }else if(token_type == CLASS_SYMBOL || token_type == CLASS_DEFINE){
        QrtSymbol *symbol = qrt_symbol_alloc(node, token_type == CLASS_DEFINE); 
        symbol->name = name;
        if(symbol->is_define){
            ctl_tree_insert(ctx->namespace, (CtlAbs *)name, (CtlAbs *)symbol);
        }
        node->value = (CtlAbs *)symbol; 
    }

    node->previous = current;
    current->next = node;
    ctx->next = node;
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
            if(is_cmp(*p) || is_block(*p)){
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
    }else if(class == CLASS_COUNTED){
        return "COUNTED";
    }else if(class == CLASS_OPP){
        return "OPERATOR";
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

void print_node(struct qrt_cell *node){
    char *node_value = "";
    int next_id = node->next != NULL ? node->next->base.id  : -1;
    int prev_id = node->previous != NULL ? node->previous->base.id  : -1;
    int class = node->base.class;
    struct qrt_cell *cell = node;
    if(node->value){
        if(node->value->base.class == CLASS_OPP){
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
    printf("<%s id:%2d next:%d prev:%d %s >\n",
        get_class_str((CtlAbs *)cell), node->base.id, next_id, prev_id, node_value
    );
}

int main(){
    char *source = ":y 3\n :run {\n write y\n write x*2 \n}\n :z = run :x 5 ";
    printf("source\n%s\n---------------\n", source);
    struct qrt_ctx *ctx = parse(source);
    struct qrt_cell *node = ctx->start;
    do {
        print_node(node);
        /*
        if(node->opp_type){
            if(is_between_opp_type(node->opp_type)){
                ctx->reg = node->call(node, node->previous, node->next); 
            }
        }
        */
    }while((node = node->next));
}
