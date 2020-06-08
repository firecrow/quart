/* Copyright 2020 Firecrow Silvernight (fire@firecrow.com) licensed under the MIT License see LICENSE file */

QrtCell *break_chain_cell(QrtCell *cell){
    QrtCell *next;
    if(!cell) return NULL;
    next = cell->next;
    cell->next = NULL;
    return next;
}

QrtCell *skip_cell(QrtCell *cell){
    QrtCell *next;
    if(!cell) return NULL;
    next = cell->next;
    cell->next = NULL;
    return next;
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
    QrtSymbol *symbol;
    QrtBlock *block = ctx->block;
    QrtBlock *vblock, *closeb, *ablock, *bblock;
    CtlInt *qnumber;
    QrtOpp *opp;
    QrtSep *sep;
    int block_is_live = 0;

    CtlAbs *value = actor->value;
    if(!args){
        return NULL;
    }
    if((vblock = asQrtBlock(value))){
        if(vblock->type == '{'){
            vblock->branch = break_chain_cell(actor);
            push_block(ctx, vblock);
        }else if(vblock->type == '}'){
            break_chain_cell(actor->prev);
            bblock = ctx->block; 
            pop_block(ctx);
            bblock->shelf = args;
            block_is_live = 1;
        }
    }
    if(is_breaking_value(value)){
        if(!ctx->block->parent_cell)
            return args;
        if(block_is_live && is_valid_cell_next(actor)){
            block_is_live = 0;
            bblock->parent_cell->next = break_chain_cell(args->next);
        }
    }
    return args;
}

void build(QrtCtx *ctx){
    QrtCell *cell = ctx->start;
    if(!ctx->block) ctx->block = qrt_block_alloc('{', NULL);
    QrtBlock *block = ctx->block; 
    while(cell){
        cell = build_cell(ctx, cell, cell->next);
    }
}





















