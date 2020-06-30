/* Copyright 2020 Firecrow Silvernight (fire@firecrow.com) licensed under the MIT License see LICENSE file */

CtlAbs *math_call(QrtCtx *ctx, CtlAbs *value){
    QrtBlock *block = ctx->block;
    QrtOpp *opp = block->opp;
    if(!asCtlInt(value)){
        pop_opp(block);
        return value;
    }

    int regval = block->reg != NULL ? asCtlInt(block->reg)->value : -1000;


    if(block->reg == NULL){
        block->reg = (CtlAbs *)ctl_int_alloc(asCtlInt(value)->value);
    }else{
        CtlInt *intreg = asCtlInt(block->reg);
        if(opp->opp_type == '-'){
            intreg->value -= asCtlInt(value)->value;    
        }else if(opp->opp_type == '+'){
            intreg->value += asCtlInt(value)->value;    
        } else if(opp->opp_type == '*'){
            intreg->value = intreg->value * asCtlInt(value)->value;    
        }
    }
    print_indent(ctx->indent);printf("\x1b[36m%d  \x1b[0m\n", regval);
    print_indent(ctx->indent);printf("\x1b[36m%c  \x1b[0m:", opp->opp_type);print_value(value);
    print_indent(ctx->indent);printf("\x1b[36m=   \x1b[0m:");print_value(block->reg);
    return block->reg;
}

CtlAbs *opp_assign_call(QrtCtx *ctx, CtlAbs *value){
    QrtBlock *block = ctx->block;
    QrtSymbol *symbol;
    if((symbol = asQrtSymbol(block->opp->value))){
        if(is_symbol_type(symbol->name->data[0])){
            symbol->name->data++;
            symbol->name->length--;
        }
        ctl_tree_insert(block->namespace, (CtlAbs *)symbol->name, value);
    }
    pop_opp(block);
    return exec_value(ctx, value);
}

QrtOpp *opp_assign(QrtSymbol *symbol){
    QrtOpp *opp = qrt_opp_alloc(':'); 
    opp->value = (CtlAbs *)symbol;
    opp->call = opp_assign_call;
    return opp;
}
