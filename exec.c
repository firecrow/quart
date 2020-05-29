CtlAbs *exec_cell(QrtCtx *ctx, QrtCell *cell){
    if(!cell) return NULL;
    QrtBlock *block;
    CtlAbs *value = NULL;
    QrtOpp *opp;
    QrtSep *sep;
    CtlInt *intvalue;

    while(cell->next){
        print_indent(ctx->indent);print_cell(cell); 
        print_indent(ctx->indent);print_block(ctx->block);
        value = cell->value;
        if((block = asQrtBlock(cell->value))){
            if(block->type == '}'){
                cell = cell->next;
                continue;
            }else{
                ctx->block = block;
                ctx->indent += 2;
                value = exec_cell(ctx, block->branch);  
                printf("-----------: ");print_value(value);
                ctx->indent -= 2;
                ctx->block = ctx->block->parent;
            }
        }else if((opp = asQrtOpp(cell->value))){
            ctx->block->opp = opp;
        }
        if(!opp && ctx->block->opp){
            printf("sending to opp...\n");
            value = ctx->block->opp->call(ctx->block, value);
        }
        if((intvalue = asCtlInt(value))){
            if(ctx->block->reg == NULL){
                ctx->block->reg = (CtlAbs *)intvalue;
            }
        }
        if((sep = asQrtSep(cell->value))){
            ctx->block->reg = NULL;
            ctx->block->opp = NULL;
        }
        cell = cell->next;
    }
    return value;
}

void exec(QrtCtx *ctx){
    exec_cell(ctx, ctx->start);
}
