QrtCell *exec_cell(QrtCtx *ctx, QrtCell *actor, QrtCell *args){
    print_indent(ctx->indent);printf("\x1b[34m");print_cell(actor);
    QrtBlock *block, *vblock, *closeb, *ablock, *bblock;
    block = ctx->block;
    QrtCell *cell; 
    QrtOpp *opp;
    if((opp = asQrtOpp(actor->value))){
        ctx->block->func = opp;
    }
    if((vblock = asQrtBlock(actor->value))){
        if(vblock->type == 'x'){
            ctx->indent += 4;
            push_block(ctx, vblock);
            cell = vblock->branch;
            while(cell){
                cell = exec_cell(ctx, cell, cell->next);
            }
            pop_block(ctx);
            ctx->indent -= 4;
            actor = cell;
        }
    }
    if(block->func){
        if(block->opp->opp_type == '+'){
            if(block->req == NULL){
                block->reg = args; 
            }
        }
    }
    return args;
}

void exec(QrtCtx *ctx){
    QrtCell *cell = ctx->start;   
    while(cell){
        cell = exec_cell(ctx, cell, cell->next);
    }
}
