QrtCell *exec_cell(QrtCtx *ctx, QrtCell *actor, QrtCell *args){
    print_indent(ctx->indent);printf("\x1b[34m");print_cell(actor);
    QrtBlock *vblock, *closeb, *ablock, *bblock;
    QrtCell *cell;
    if((vblock = asQrtBlock(actor->value))){
        if(vblock->type == 'x'){
            push_block(ctx, vblock);
            ctx->indent += 4;
            cell = vblock->branch;
            while(cell){
                cell = exec_cell(ctx, cell, cell->next);
            }
            ctx->indent -= 4;
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
