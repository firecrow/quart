/* Copyright 2020 Firecrow Silvernight (fire@firecrow.com) licensed under the MIT License see LICENSE file */

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
            new = ctl_tree_get(block->namespace, (CtlAbs *)symbol->name);
            if(new)
                return new;
            block = block->parent;
        }
    }
    return value;
}

CtlAbs *put_value(QrtBlock *block, QrtSymbol *symbol, CtlAbs *value){
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

QrtCell *build_cell(QrtCtx *ctx, QrtCell *actor, QrtCell *args){
    /*print_indent(ctx->indent);*/
    print_cell(actor);

    QrtSymbol *symbol;
    QrtSep *sep;
    QrtBlock *block = ctx->block;
    QrtBlock *vblock, *closeb, *ablock;
    CtlInt *qnumber;
    QrtOpp *opp;

    CtlAbs *value = fetch_value(block, actor->value);
    if((sep = asQrtSep(value))){
        if(ctx->block && ctx->block->is_live){
            ctx->block->is_live = 0;
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
        if(symbol->type == ':' || symbol->type == '&'){
            argvalue = put_value(block, symbol, args->value);
            value = args->value;
        }
        if(symbol->type == 'x'){
            printf("\x1b[31min x\n\x1b[0m");
            CtlAbs *symbol_value = fetch_value(block, (CtlAbs *)symbol);
            if((ablock = asQrtBlock(symbol_value))){
                printf("\x1b[32msetting islive\n\x1b[0m");
                ablock->is_live = 1;
                push_block(ctx, ablock);
                ctx->indent += 4;
            }
        }
    }
    if((vblock = asQrtBlock(actor->value))){
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
    }
    if((opp = asQrtOpp(value))){
    }
    return args;
}

void build(QrtCtx *ctx){
    QrtCell *cell = ctx->start;
    if(!ctx->block) ctx->block = qrt_block_alloc('{', NULL);
    QrtBlock *block = ctx->block; 
    while(cell){
        cell = build_cell(ctx, cell, cell->next);
        if(cell == NULL && ctx->block->resume){
            cell = ctx->block->resume;
            ctx->block->resume = NULL;
            ctx->block = ctx->block->parent;
        }
    }
    block = ctx->block;
    while(block){
        print_block(block);
        block = block->parent;
    }
}




















