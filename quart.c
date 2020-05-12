#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "../crowtils/crowx/crowx.c"
#include "../crowtils/poly/poly.c"
#include "../crowtils/counted/counted.c"
#include "../crowtils/crownumber/crownumber.c"
#include "../crowtils/crray/crray.c"
#include "../crowtils/crowtree/tree.c"

enum parse_states {
    QRT_OUT = 0,
    QRT_SYMBOL,
    QRT_PRE_VALUE,
    QRT_VALUE,
    QRT_STRING,
    QRT_INT
};

int is_punc(char c){
    return (c == ';' || c == ',' || c == '<' || c == '>' || c == '!' || c == '=' || c == '{' || c == '}' || c == '[' || c == ']');
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
    QRT_TOKEN_BLOCK
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
            }
            if(c == '"'){
                class = QRT_TOKEN_STRING;
                if(token->data[token->length-1] != '"')
                    return QRT_TOKEN_INVALID;
                continue;
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

CtlCounted  *emit_token(CtlCounted *name){
    printf("token(%d):%s\n", identify_token(name), ctl_counted_to_cstr(name));
    return name;
}

CtlCounted  *emit_value(CtlCounted *token, CtlCounted *value){
    printf("pair:%s->%s\n", ctl_counted_to_cstr(token), ctl_counted_to_cstr(value)); 
    return token;
}

void parse(char *source){
    char *p = source;
    enum parse_states state = QRT_OUT;
    CtlCounted *shelf = ctl_counted_alloc(NULL, 0);
    CtlCounted *token = ctl_counted_alloc(NULL, 0);
    int is_definition = 0;
    if(p == '\0') 
        return;
    do {
        /*printf("%c\n", *p);*/
        if(state == QRT_OUT){
            if(*p == ':'){
                /*printf("entering symbol: %c\n", *p);*/
                /*state = QRT_SYMBOL;*/
                is_definition = 1;
            }else{
                /*printf("not : %c\n", *p);*/
                /*state = QRT_SYMBOL;*/
                if((shelf->length == 0 && is_alpha(*p)) || is_alpha_numeric(*p)){
                    /*printf("push : %c\n", *p);*/
                    ctl_counted_push(shelf, p, 1);
                }else if(shelf->length > 0){
                    /*printf("finalize : %c\n", *p);*/
                    token = emit_token(shelf); 
                    shelf = ctl_counted_alloc(NULL, 0);
                    state = QRT_OUT;
                }
            }
        }else if(state == QRT_PRE_VALUE){
            if(is_alpha_numeric(*p)){
                state = QRT_VALUE;
            }
        }
        if(state == QRT_VALUE){
            if(is_alpha_numeric(*p)){
                ctl_counted_push(shelf, p, 1);
            }else if(shelf->length > 0){
                emit_value(token, shelf); 
                shelf = ctl_counted_alloc(NULL, 0);
                state = QRT_OUT;
            }
        }
    }while(*++p != '\0');
}

int main(){
    char *source = ":y 3\n :run {\n write y\n write x*2 \n}\n :z = run :x 5 ";
    printf("source\n%s\n---------------\n", source);
    parse(source);
}
