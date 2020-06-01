QrtOpp *opp_assign(QrtSymbol *symbol);
CtlAbs *exec_cell(QrtCtx *ctx, QrtCell *cell);

QrtOpp *push_opp(QrtBlock *block, QrtOpp *opp){
    printf("push-------------\n");
    if(block->opp){
        opp->parent = block->opp;
        opp->parent->next = opp;
    }
    return block->opp = opp;
}

QrtOpp *pop_opp(QrtBlock *block){
    printf("pop-------------\n");
    QrtOpp *opp = block->opp;
    if(opp->parent){
        opp->parent->next = opp->next;
    }
    if(opp->next){
        opp->next->parent = opp->parent;
    }
    block->opp = opp->parent;
    opp->next = opp->parent = NULL;
    return opp;
}

CtlAbs *exec_value(QrtCtx *ctx, CtlAbs *value){
    printf("\x1b[31m exec_value:");print_value(value);
    QrtBlock *block;
    QrtOpp *opp;
    QrtSep *sep;
    QrtSymbol *symbol;
    CtlInt *intvalue;
    print_indent(ctx->indent);print_block(ctx->block);
    if((opp = asQrtOpp(value))){
        printf("it's an opp :%c\n", opp->opp_type);
        opp = push_opp(ctx->block, opp);
    }
    if((symbol = asQrtSymbol(value))){
        if(symbol->type == 'x'){
            value = ctl_tree_get(ctx->block->namespace, (CtlAbs *)symbol->name);
            if(!value){
                ctl_xerrlog("value not found");
            }
        }else{
            opp = push_opp(ctx->block, opp_assign(symbol));
        }
    }
    if((block = asQrtBlock(value))){
        if(block->type == '}'){
            return NULL;
        }else{
            ctx->block = block;
            ctx->indent += 2;
            value = exec_cell(ctx, block->branch);  
            printf("-----------: ");print_value(value);
            ctx->indent -= 2;
            ctx->block = ctx->block->parent;
        }
    }
    if(!opp && ctx->block->opp){
        printf("it's not an opp run it :%c\n", ctx->block->opp->opp_type);
        value = ctx->block->opp->call(ctx, value);
    }
    if((intvalue = asCtlInt(value))){
        if(ctx->block->reg == NULL){
            ctx->block->reg = (CtlAbs *)intvalue;
        }
    }
    if((sep = asQrtSep(value))){
        ctx->block->reg = NULL;
        ctx->block->opp = NULL;
    }
    return value;
}

CtlAbs *exec_cell(QrtCtx *ctx, QrtCell *cell){
    if(!cell) return NULL;
    CtlAbs *value = NULL;

    while(cell->next){
        print_indent(ctx->indent);print_cell(cell); 
        value = exec_value(ctx, cell->value);
        cell = cell->next;
    }
    return value;
}

void exec(QrtCtx *ctx){
    exec_cell(ctx, ctx->start);
}
