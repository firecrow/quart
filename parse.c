/* Copyright 2020 Firecrow Silvernight (fire@firecrow.com) licensed under the MIT License see LICENSE file */
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
            if(c == ':' || c == '.' || c == '&'){
                class = CLASS_SYMBOL;
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
        if(class == CLASS_SYMBOL){
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

QrtCell *make_token(struct qrt_ctx *ctx, CtlCounted *name, QrtCell *current){
    printf("name:%s\n", ctl_to_cstr(name));
    struct qrt_cell *node = qrt_cell_alloc();
    enum classes token_type = identify_token(name);
    if(token_type == CLASS_OPP){
        QrtOpp *opp = qrt_opp_alloc(name->data[0]);
    /*
        switch(opp->opp_type){
            case '*':
            case '+':
            case '-':
                opp->call = math_call;
                break;
            default: 
                break;
        }
    */
        node->value = (CtlAbs *)opp;

    }else if(token_type == CLASS_BLOCK){
        node->value = (CtlAbs *)qrt_block_alloc(name->data[0], NULL); 
    }else if(token_type == CLASS_INT){
        node->value = (CtlAbs *)ctl_int_alloc(atoi(ctl_counted_to_cstr(name)));
    }else if(token_type == CLASS_SEP){
        node->value = (CtlAbs *)qrt_sep_alloc();
    }else if(token_type == CLASS_COUNTED){
        node->value = (CtlAbs *)name;
    }else if(token_type == CLASS_SYMBOL){
        QrtSymbol *symbol = qrt_symbol_alloc(node, name); 
        node->value = (CtlAbs *)symbol; 
    }

    if(!ctx->start){
        ctx->start = node;
    }else{
        current->next = node;
        node->previous = current;
    }
    return node;
}

#ifdef TEST
    #define make_token make_token_mock
#endif


QrtCtx *parse(char *source){
    char *p = source;
    QrtCtx *ctx = qrt_ctx_alloc();
    ctx->shelf = ctl_counted_alloc(NULL, 0);
    ctx->block = qrt_block_alloc('{', NULL);
    QrtCell *cell;

    if(p == '\0') 
        return NULL;
    do {
        if(*p == ':' || *p == '.' || *p == '&'){
            ctl_counted_push(ctx->shelf, p, 1);
        }else{
            if((ctx->shelf->length == 0 && is_alpha(*p)) || is_alpha_numeric(*p)){
                ctl_counted_push(ctx->shelf, p, 1);
            }else if(ctx->shelf->length > 0){
                cell = make_token(ctx, ctx->shelf, cell); 
                ctx->shelf = ctl_counted_alloc(NULL, 0);
            }
            if(is_cmp(*p) || is_block(*p) || is_sep(*p)){
                cell = make_token(ctx, ctl_counted_alloc(p, 1), cell);
            }
        }
    }while(*p++ != '\0');
    return ctx;
}
