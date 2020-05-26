
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

QrtCell *exec_cell(QrtBlock *block, QrtCell *actor, QrtCell *args){
    print_cell(actor);

    QrtSymbol *symbol;
    QrtBlock *vblock;
    CtlInt *qnumber;
    QrtOpp *opp;

    CtlAbs *value = fetch_value(block->namespace, actor->value);
    if(!args){
        return NULL;
    }
    CtlAbs *argvalue = fetch_value(block->namespace, args->value);

    if((symbol = asQrtSymbol(value))){
        printf("symbol---------%s\n", get_node_value_str(value));
    }
    if((vblock = asQrtBlock(value))){
        printf("block---------%s\n", get_node_value_str(value));
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
    QrtBlock *block = ctx->block;
    while(cell){
        cell = exec_cell(block, cell, cell->next);
    }
}




















