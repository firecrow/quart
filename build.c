QrtCtx *blocks(QrtCtx *ctx){
    QrtBlock *block = ctx->root;
    QrtBlock *newblock;
   
    QrtCell *start = ctx->start;
    QrtCell *before = start;
    QrtCell *cell = before;
    QrtCell *next = cell;
    Crray *stack = ctl_crray_alloc(16);
    ctl_crray_push(stack, block);
    Crray *list = ctl_crray_alloc(16);
    ctl_crray_push(list, block);
    QrtStatement *newstmt;
    
    QrtStatement *stmt = qrt_statement_alloc(block, NULL, cell);
    block->statement_root = block->statement_next = stmt;
    while(cell){
        if(cell->value && cell->value->base.class == CLASS_BLOCK){
            QrtBlock *new = (QrtBlock *)cell->value; 
            QrtBlock *current;
            QrtBlock *next;
            ctl_crray_push(list, new);

            CtlCounted *space = ctl_counted_alloc("hello", 5);
            space->length = 0;
            
            if(new->type == '{'){
                new->parent = block;
                block->branch = new;
                ctl_crray_push(stack, ctl_block_incr(new));
                block = new;

                before = cell->next;
                cell->next = NULL;
                cell = before;
                newstmt = qrt_statement_alloc(block, NULL, before);
                if(block->statement_root == NULL){
                    block->statement_root = block->statement_next = newstmt;
                }else{
                    block->statement_next->next =  newstmt;
                }
                block->statement_next = newstmt;
                stmt = newstmt;
                continue;
            }else{
                ctl_crray_remove(stack,  -1);
                next = ctl_crray_tail(stack);
                current = next;
                while((next = next->next)) current = next;
                current->next = new;
                new->parent = current;

                block = new;
                newstmt = qrt_statement_alloc(block, NULL, before);
                if(block->statement_root == NULL){
                    block->statement_root = block->statement_next = newstmt;
                }else{
                    block->statement_next->next =  newstmt;
                }

                stmt = block->statement_root;
                before = cell->next;
                cell->next = NULL;
                cell = before;
                stmt->cell_root = cell;
                continue;

            }

        }
        if(is_break_value(cell->value)){
            before = cell->next;
            cell->next = NULL;
            cell = before;
            newstmt = qrt_statement_alloc(block, stmt, before);
            if(block->statement_root == NULL){
                block->statement_root = block->statement_next = newstmt;
            }else{
                block->statement_next->next = newstmt;
            }
            block->statement_next = newstmt;
            continue;
        }
        cell = cell->next;
    }
    /*
    for(int i=0; i< list->length; i++){
        print_block(list->content[i], ctl_counted_from_cstr("+ "));
    }
    */
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

QrtCtx *build(char *source){
    struct qrt_ctx *ctx = parse(source);
    ctx = blocks(ctx);
    return ctx;
}
