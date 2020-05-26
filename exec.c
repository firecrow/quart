QrtCell *break_chain_cell(QrtCell *cell){
    QrtCell *next;
    if(!cell) return NULL;
    next = cell->next;
    cell->next = NULL;
    return next;
}

CtlAbs *fetch_value(CtlTree *namespace, CtlAbs *value){
    CtlAbs *new;
    QrtSymbol *symbol;
    if((symbol = asQrtSymbol(value))){
        new = ctl_tree_get(namespace, (CtlAbs *)symbol->name);
        if(new)
            return new;
    }
    return value;
}

QrtCell *exec_cell(QrtCtx *ctx, QrtCell *actor, QrtCell *args){
    print_cell(actor);

    QrtSymbol *symbol;
    QrtSep *sep;
    QrtBlock *block = ctx->block;
    QrtBlock *vblock, *closeb, *ablock;
    CtlInt *qnumber;
    QrtOpp *opp;

    CtlAbs *value = fetch_value(block->namespace, actor->value);
    if((sep = asQrtSep(value))){
        printf("sep---------%s\n", get_node_value_str(value));
        block->resume = args;
        printf("sep next\n");print_cell(args);
        if(ctx->block)
            return ctx->block->cell;
    }
    if(!args){
        return NULL;
    }
    CtlAbs *argvalue = fetch_value(block->namespace, args->value);

    if((symbol = asQrtSymbol(value))){
        printf("symbol---------%s\n", get_node_value_str(value));
        if((ablock = asQrtBlock(symbol->value))){
            printf("ablock baby %s\n", get_node_value_str(symbol->value));
        }
    }
    if((vblock = asQrtBlock(value))){
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
            vblock->parent = block;
            block->next = vblock;
            ctx->block = vblock;
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
}




















