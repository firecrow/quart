/* Copyright 2020 Firecrow Silvernight (fire@firecrow.com) licensed under the MIT License see LICENSE file */

QrtCell *break_chain_cell(QrtCell *cell){
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
    QrtSep *sep;
    QrtBlock *block = ctx->block;
    QrtBlock *vblock, *closeb, *ablock, *bblock;
    CtlInt *qnumber;
    QrtOpp *opp;

    CtlAbs *value = actor->value;
    if((sep = asQrtSep(value))){
    }
    if(!args){
        return NULL;
    }
    if((vblock = asQrtBlock(value))){
        if(vblock->type == '{'){
            vblock->branch = break_chain_cell(actor);
            push_block(ctx, vblock);
            ctx->indent += 4;
        }else if(vblock->type == '}'){
            args = break_chain_cell(actor->prev)->next;
            bblock = ctx->block; 
            pop_block(ctx);
            if(bblock && bblock->parent_cell){
                bblock->parent_cell->next = args;
                ctx->indent -= 4;
            }
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





















