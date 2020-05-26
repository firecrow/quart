QrtCell *exec_cell(QrtBlock *block, QrtCell *actor, QrtCell *args){
    print_cell(actor);
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




















