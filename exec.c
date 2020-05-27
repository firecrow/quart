QrtCell *break_chain_cell(QrtCell *cell){
    QrtCell *next;
    if(!cell) return NULL;
    next = cell->next;
    cell->next = NULL;
    return next;
}

CtlAbs *fetch_value(QrtBlock *block, CtlAbs *value){
    CtlAbs *new = NULL;
    QrtSymbol *symbol;
    if((symbol = asQrtSymbol(value))){
        while(block){
            printf(".");
            new = ctl_tree_get(block->namespace, (CtlAbs *)symbol->name);
            if(new)
                return new;
            block = block->parent;
        }
    }
    return value;
}

CtlAbs *put_value(QrtBlock *block, QrtSymbol *symbol, CtlAbs *value){
    printf("put value-");
    CtlCounted *name = symbol->name;
    char c = name->data[0];
    if(c == ':' || c == '.' || c == '&'){
        name->data++;
        name->length--;
    }
    ctl_tree_insert(block->namespace, (CtlAbs *)symbol->name, value);
    return value;
}

void push_block(QrtCtx *ctx, QrtBlock *block){ 
    if(block != ctx->block){
        block->parent = ctx->block;
        ctx->block = block;
    }
}

QrtCell *exec_cell(QrtCtx *ctx, QrtCell *actor, QrtCell *args){
    print_indent(ctx->indent);print_cell(actor);

    QrtSymbol *symbol;
    QrtSep *sep;
    QrtBlock *block = ctx->block;
    QrtBlock *vblock, *closeb, *ablock;
    CtlInt *qnumber;
    QrtOpp *opp;

    CtlAbs *value = fetch_value(block, actor->value);
    if((sep = asQrtSep(value))){
        printf("sep---------%s\n", get_node_value_str(value));
        if(ctx->block && ctx->block->is_live){
            block->resume = args;
            ctx->indent -= 4;
            return ctx->block->cell;
        }
    }
    if(!args){
        return NULL;
    }
    CtlAbs *argvalue = fetch_value(block, args->value);
    if((symbol = asQrtSymbol(actor->value))){
        printf("\x1b[31m%c\n\x1b[0m", symbol->type);
        if(symbol->type == ':' || symbol->type == '&'){
            argvalue = put_value(block, symbol, args->value);
            value = args->value;
        }
        if(symbol->type == 'x'){
            printf("\x1b[31min x\n\x1b[0m");
            CtlAbs *symbol_value = fetch_value(block, (CtlAbs *)symbol);
            print_value(symbol_value);
            if((ablock = asQrtBlock(symbol_value))){
                printf("\x1b[31min ablock\n\x1b[0m");
                printf("setting islive\n");
                ablock->is_live = 1;
                push_block(ctx, ablock);
                ctx->indent += 4;
            }
        }
    }
    if((vblock = asQrtBlock(actor->value))){
        printf("block---------%s\n", get_node_value_str(value));
        if(vblock->type == '{'){
            vblock->cell = args;
            while(args){
                closeb = asQrtBlock(args->value);
                if(closeb && closeb->type == '}'){
                    break;
                }
                args = args->next;
            }
            args = break_chain_cell(args);
            push_block(ctx, vblock);
        }else{
            return block->resume;
        }
    }
    if((qnumber = asCtlInt(value))){
        printf("int---------%s\n", get_node_value_str(value));
    }
    if((opp = asQrtOpp(value))){
        printf("opp---------%s\n", get_node_value_str(value));
    }
    return args;
}

void exec(QrtCtx *ctx){
    QrtCell *cell = ctx->start;
    if(!ctx->block) ctx->block = qrt_block_alloc('{', NULL);
    QrtBlock *block = ctx->block; while(cell){
        cell = exec_cell(ctx, cell, cell->next);
    }
    print_block(block);
}




















