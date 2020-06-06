/* Copyright 2020 Firecrow Silvernight (fire@firecrow.com) licensed under the MIT License see LICENSE file */

QrtOpp *opp_assign(QrtSymbol *symbol);
CtlAbs *exec_cell(QrtCtx *ctx, QrtCell *cell);

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
    block->opp = opp->parent;
    opp->next = opp->parent = NULL;
    return opp;
}

CtlAbs *exec_value(QrtCtx *ctx, CtlAbs *value){
    QrtBlock *block;
    QrtBlock *vblock;
    QrtOpp *opp = NULL;
    QrtSep *sep;
    QrtSymbol *symbol;
    CtlAbs *newval;
    CtlInt *intvalue;
    if((symbol = asQrtSymbol(value))){
        if(symbol->type == 'x'){
            vblock = ctx->block;
            newval = NULL;
            while(!newval && vblock){
                newval = ctl_tree_get(vblock->namespace, (CtlAbs *)symbol->name);
                vblock = vblock->parent;
            }
            if(!newval){
                QrtError("value not found");
            }
            value = newval;
        }else{
            opp = push_opp(ctx->block, opp_assign(symbol));
        }
    }
    if((block = asQrtBlock(value))){
        ctx->block = block;
        ctx->indent += 2;
        printf("shelf.....................................\n");
        value = exec_cell(ctx, ctx->block->shelf);
        printf("branch.....................................\n");
        value = exec_cell(ctx, ctx->block->branch);
        ctx->indent -= 2;
        ctx->block = ctx->block->parent;
    }
    if((sep = asQrtSep(value))){
        ctx->block->reg = NULL;
        ctx->block->opp = NULL;
    }
    if(asQrtOpp(value)){
        opp = push_opp(ctx->block, asQrtOpp(value));
    }
    if(!opp && ctx->block->opp){
        printf("in block opp: ");print_value((CtlAbs *)ctx->block->opp);
        value = ctx->block->opp->call(ctx, value);
    }
    if((intvalue = asCtlInt(value))){
        if(ctx->block->reg == NULL){
            ctx->block->reg = (CtlAbs *)intvalue;
        }
    }
    return value;
}

CtlAbs *exec_cell(QrtCtx *ctx, QrtCell *cell){
    if(!cell) return NULL;
    CtlAbs *value = NULL;

    while(cell){
        print_indent(ctx->indent);print_cell(cell); 
        print_indent(ctx->indent);print_block(ctx->block); 
        value = exec_value(ctx, cell->value);
        cell = cell->next;
    }
    return value;
}

void exec(QrtCtx *ctx){
    exec_cell(ctx, ctx->start);
}
