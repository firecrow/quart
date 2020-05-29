CtlAbs *exec_cell(QrtCtx *ctx, QrtCell *cell){
    if(!cell) return NULL;
    QrtBlock *block;
    CtlAbs *value = NULL;
    QrtOpp *opp;
    QrtSep *sep;

    while(cell){
        print_indent(ctx->indent);print_cell(cell); 
        print_indent(ctx->indent);print_block(ctx->block);
        value = cell->value;
        if((block = asQrtBlock(cell->value))){
            ctx->block = block;
            ctx->indent += 2;
            value = exec_cell(ctx, block->branch);  
            ctx->indent -= 2;
            if(ctx->block->parent)
                ctx->block = ctx->block->parent;
        }else if((opp = asQrtOpp(cell->value))){
            ctx->block->opp = opp;
        }
        if(!opp && ctx->block->opp){
            value = ctx->block->opp->call(ctx->block, value);
        }
        if((sep = asQrtSep(cell->value))){
            ctx->block->reg = NULL;
        }
        cell = cell->next;
    }
    return value;
}

void exec(QrtCtx *ctx){
    exec_cell(ctx, ctx->start);
}
