QrtOpp *push_opp(QrtBlock *block, QrtOpp *opp){
    if(block->opp){
        opp->parent = block->opp;
        opp->parent->next = opp;
    }
    return block->opp = opp;
}

QrtOpp *pop_opp(QrtBlock *block){
    QrtOpp *opp = block->opp;
    if(opp->parent){
        opp->parent->next = opp->next;
    }
    if(opp->next){
        opp->next->parent = opp->parent;
    }
    opp->next = opp->parent = NULL;
    block->opp = opp->parent;
    return opp;
}

CtlAbs *exec_cell(QrtCtx *ctx, QrtCell *cell){
    if(!cell) return NULL;
    QrtBlock *block;
    CtlAbs *value = NULL;
    QrtOpp *opp;
    QrtSep *sep;
    QrtSymbol *symbol;
    CtlInt *intvalue;

    while(cell->next){
        print_indent(ctx->indent);print_cell(cell); 
        print_indent(ctx->indent);print_block(ctx->block);
        value = cell->value;
        if((symbol = asQrtSymbol(cell->value))){
            if(symbol->type == 'x'){
                /* value = fetch value */
            }else{
                opp = push_opp(block, opp_assign(symbol));
            }
        }
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
        }
        if((opp = asQrtOpp(cell->value))){
            opp = push_opp(block, opp);
        }
        if(!opp && ctx->opp){
            value = ctx->block->opp->call(ctx, value);
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
