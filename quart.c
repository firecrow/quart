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

int is_alpha(char c){
    return (c >= 65 && c <= 90) || (c >=97 && c <= 122);
}

int is_numeric(char c){
    return (c >= 48 && c <= 57);
}

int is_alpha_numeric(char c){
    return is_numeric(c) || is_alpha(c);
}

enum token_types {
    QRT_TOKEN_INVALID = -1,
    QRT_TOKEN_NULL = 0,
    QRT_TOKEN_INT,
    QRT_TOKEN_SYMBOL,
    QRT_TOKEN_DECLARE_SYMBOL,
    QRT_TOKEN_STRING,
    QRT_TOKEN_CMP,
    QRT_TOKEN_BLOCK
};

enum qrt_types {
    /* node types */
    QRT_UNKNOWN = 0,
    QRT_NODE = 1,
    QRT_VALUE,
    QRT_OPERATR,
    QRT_BLOCK,
    /*operator_types*/
    QRT_PLUS,
    QRT_MINUS,
    QRT_DIVIDE,
    QRT_MULTIPLY,
    QRT_GREATER,
    QRT_LESS,
    QRT_NOT,
    /*value_types*/
    QRT_INT,
    QRT_STRING,
    QRT_BOOLEAN,
    QRT_TREE,
    QRT_LIS,
    /* parse states */
    QRT_OUT,
    QRT_SYMBOL
};


enum token_types identify_token(CtlCounted *token){
    int i = 0;
    char c; 
    int class = QRT_TOKEN_NULL;
    for(int i = 0; i < token->length; i++){
        c = token->data[i]; 
        if(i == 0){
            if(is_numeric(c)){
                class = QRT_TOKEN_INT;
            } 
            if(is_alpha(c)){
                class = QRT_TOKEN_SYMBOL;
            }
            if(c == ':'){
                class = QRT_TOKEN_DECLARE_SYMBOL;
                continue;
            }
            if(c == '"'){
                class = QRT_TOKEN_STRING;
                if(token->data[token->length-1] != '"')
                    return QRT_TOKEN_INVALID;
                continue;
            }
            if(is_cmp(c)){
                return QRT_TOKEN_CMP;
            } 
        }
        /*printf("%c %d\n", c, class);*/
        if(class == QRT_TOKEN_INT){
            if(!is_numeric(c)){
                return QRT_TOKEN_INVALID;
            }
        }
        if(class == QRT_TOKEN_SYMBOL || class == QRT_TOKEN_DECLARE_SYMBOL){
            if(!is_alpha_numeric(c)){
                return QRT_TOKEN_INVALID;
            }
        }
        if(class == QRT_TOKEN_STRING){
            if(c == '"' && token->data[i-1] != '\\' && i != token->length-1)
                continue;
            return QRT_TOKEN_INVALID;

        }
    }
    return class;
}

struct qrt_ctx {
    int id;
    CtlTree *namespace;
    struct qrt_ctx *parent;
    CtlCounted *shelf;
    Crray *stack;
    enum qrt_types  state;
    struct qrt_node *start;
    struct qrt_node *next;
};

struct qrt_value {
    enum qrt_types type;
    void (*exec)(struct qrt_ctx *, struct qrt_node *);
    int intval;
    char *strval;
    /* block stuff here */
};

struct qrt_value *qrt_value_alloc(enum qrt_types type){
    struct qrt_value *value;
    ctl_xptr(value = malloc(sizeof(struct qrt_value)));
    bzero(value, sizeof(struct qrt_value));
    value->type = type;
    return value;
}

struct qrt_node {
    int id;
    enum qrt_types type;
    enum qrt_types operator_type;
    enum token_types token_type;;
    CtlCounted *symbol;
    void (*exec)(struct qrt_ctx *, struct qrt_node *);
    struct qrt_value *value;
    struct qrt_node *next;
    struct qrt_node *previous;
};

int qrt_node_id=0;
struct qrt_node *qrt_node_alloc(enum qrt_types type){
    struct qrt_node *node;
    ctl_xptr(node = malloc(sizeof(struct qrt_node)));
    bzero(node, sizeof(struct qrt_node));
    node->type = type;
    node->id = ++qrt_node_id;
    return node;
}

int qrt_ctx_id = 0;
struct qrt_ctx * qrt_ctx_alloc(){
    struct qrt_ctx *ctx;
    ctl_xptr(ctx = malloc(sizeof(struct qrt_ctx)));
    bzero(ctx, sizeof(struct qrt_ctx));
    ctx->id = ++qrt_ctx_id;
    ctx->namespace = ctl_tree_alloc(ctl_tree_crownumber_int_cmp);
    ctx->start = qrt_node_alloc(QRT_NODE); 
    ctx->stack = ctl_crray_alloc(16); 
    ctx->next = ctx->start = qrt_node_alloc(QRT_NODE);
    return ctx;
}

void emit_token(struct qrt_ctx *ctx, CtlCounted *name){
    printf("token(%d):%s\n", identify_token(name), ctl_counted_to_cstr(name));
    struct qrt_node *current = ctx->next;

    struct qrt_node *node = qrt_node_alloc(QRT_NODE);
    enum token_types token_type = identify_token(name);
    if(token_type == QRT_TOKEN_DECLARE_SYMBOL){
        node->symbol = name;
    }else if(ctx->next->token_type == QRT_TOKEN_DECLARE_SYMBOL && token_type == QRT_TOKEN_INT){
        struct qrt_value *value = qrt_value_alloc(token_type);
        value->intval = atoi(ctl_counted_to_cstr(name));
        current->value = value;
    }
    node->token_type = token_type;
    node->previous = current;
    current->next = node;
    ctx->next = node;
}

struct qrt_ctx *parse(char *source){
    char *p = source;
    struct qrt_ctx *ctx = qrt_ctx_alloc();
    ctx->state = QRT_OUT;
    ctx->shelf = ctl_counted_alloc(NULL, 0);
    Crray *plane = ctl_crray_alloc(4);
    ctl_crray_push(ctx->stack, (CtlAbs *)plane);

    if(p == '\0') 
        return NULL;
    do {
        /*printf("%c\n", *p);*/
        if(ctx->state == QRT_OUT){
            if(*p == ':'){
                /*printf("entering symbol: %c\n", *p);*/
                /*state = QRT_SYMBOL;*/
                ctl_counted_push(ctx->shelf, p, 1);
            }else{
                /*printf("not : %c\n", *p);*/
                /*state = QRT_SYMBOL;*/
                if((ctx->shelf->length == 0 && is_alpha(*p)) || is_alpha_numeric(*p)){
                    /*printf("push : %c\n", *p);*/
                    ctl_counted_push(ctx->shelf, p, 1);
                }else if(ctx->shelf->length > 0){
                    /*printf("finalize : %c\n", *p);*/
                    emit_token(ctx, ctx->shelf); 
                    ctx->shelf = ctl_counted_alloc(NULL, 0);
                    ctx->state = QRT_OUT;
                    if(is_cmp(*p)){
                        emit_token(ctx, ctl_counted_alloc(p, 1));
                    }
                }
            }
        }
    }while(*++p != '\0');
    return ctx;
}

void print_node(struct qrt_node *node){
    char *symbol_str = node->symbol != NULL ? ctl_counted_to_cstr(node->symbol) : "";
    int next_id = node->next != NULL ? node->next->id  : -1;
    int prev_id = node->previous != NULL ? node->previous->id  : -1;
    int value = node->value != NULL ? node->value->intval : 0;
    printf("<NODE id:%d type:%d tokent:%d symbol:'%s' execnull?:%d next:%d prev:%d intvalue:%d>\n",
        node->id, node->type, node->token_type,symbol_str, node->exec == NULL, next_id, prev_id,value 
    );
}

int main(){
    char *source = ":y 3\n :run {\n write y\n write x*2 \n}\n :z = run :x 5 ";
    printf("source\n%s\n---------------\n", source);
    struct qrt_ctx *ctx = parse(source);
    struct qrt_node *node = ctx->start;
    do {
        print_node(node);
    }while((node = node->next));
}
