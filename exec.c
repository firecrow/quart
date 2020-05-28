QrtCell *exec_cell(QrtCtx *ctx, QrtCell *actor, QrtCell *args){
    printf("\x1b[34m");print_cell(actor);
    return args;
}

void exec(QrtCtx *ctx){
    /*QrtCell *cell = asQrtBlock(ctx->start->next->value)->branch; */
    QrtCell *cell = ctx->start;   
    while(cell){
        cell = exec_cell(ctx, cell, cell->next);
        /*
        if(cell == NULL && ctx->block->parent && ctx->block->parent){
            cell = ctx->block->parent_cell->next;
        }
        */
    }
}
