/* Copyright 2020 Firecrow Silvernight (fire@firecrow.com) licensed under the MIT License see LICENSE file */

QrtCell *break_chain_cell(QrtCell *cell){
    QrtCell *next;
    if(!cell) return NULL;
    next = cell->next;
    cell->next = NULL;
    return next;
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

CtlAbs *fetch_or_set_value(QrtBlock *block, CtlAbs *value, QrtCell *args){
    CtlAbs *new = NULL;
    QrtSymbol *symbol;
    if((symbol = asQrtSymbol(value))){
        if(symbol->type == ':' || symbol->type == '&'){
            put_value(block, symbol, args->value);
        }else{
            while(block){
                new = ctl_tree_get(block->namespace, (CtlAbs *)symbol->name);
                if(new)
                    return new;
                block = block->parent;
            }
        }
    }

    return value;
}

void push_block(QrtCtx *ctx, QrtBlock *block){ 
    if(block != ctx->block){
        block->type = 'x';
        block->parent = ctx->block;
        ctx->block = block;
    }
}

QrtBlock *pop_block(QrtCtx *ctx){ 
    if(ctx->block->parent){
        ctx->block = ctx->block->parent;
    }
    return ctx->block;
}

QrtCell *build_cell(QrtCtx *ctx, QrtCell *actor, QrtCell *args){
    print_indent(ctx->indent);print_cell(actor);

    QrtSymbol *symbol;
    QrtSep *sep;
    QrtBlock *block = ctx->block;
    QrtBlock *vblock, *closeb, *ablock;
    CtlInt *qnumber;
    QrtOpp *opp;

    CtlAbs *value = fetch_or_set_value(block, actor->value, args);
    if((sep = asQrtSep(value))){
    }
    if(!args){
        return NULL;
    }
    if((vblock = asQrtBlock(value))){
        if(vblock->type == '{'){
            vblock->branch = args;
            push_block(ctx, vblock);
            ctx->indent += 4;
        }else if(vblock->type == '}'){
            block = ctx->block;
            vblock = pop_block(ctx);
            if(block && block->parent_cell){
                args = break_chain_cell(actor);
                block->parent_cell->next = args;
                ctx->indent -= 4;
            }
        }else{

        }

    }
    /*
    if((qnumber = asCtlInt(value))){
    }
    if((opp = asQrtOpp(value))){
    }
    */
    return args;
}

void build(QrtCtx *ctx){
    QrtCell *cell = ctx->start;
    QrtCell *prior;
    if(!ctx->block) ctx->block = qrt_block_alloc('{', NULL);
    QrtBlock *block = ctx->block; 
    while(cell){
        prior = cell;
        cell = build_cell(ctx, cell, cell->next);
        /*
        if(cell == NULL && ctx->block->parent_cell && ctx->block->parent_cell->next){
            cell = ctx->block->parent_cell->next;
            ctx->block->parent_cell->next = NULL;
            ctx->block = ctx->block->parent;
        }
        */
        while(prior && prior != cell){
             print_indent(ctx->indent);printf("\x1b[36m");print_cell(prior);printf("\x1b[0m");
            prior = prior->next;
        }
    }
    block = ctx->block;
    while(block){
        block = block->parent;
    }
}




















