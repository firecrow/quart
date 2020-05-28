QrtCell *exec_cell(QrtCtx *ctx, QrtCell *cell){
    if(!cell) return NULL;
    print_cell(cell); 
    QrtBlock *block;
    if((block = asQrtBlock(cell->value))){
        exec_cell(ctx, block->branch);  
    }
    if(cell->next){
        exec_cell(ctx, cell->next);
    }
    return cell->next;
}

void exec(QrtCtx *ctx){
    exec_cell(ctx, ctx->start);
}
