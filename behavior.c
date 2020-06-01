/* Copyright 2020 Firecrow Silvernight (fire@firecrow.com) licensed under the MIT License see LICENSE file */

CtlAbs *math_call(QrtCtx *ctx, CtlAbs *value){
    printf("\x1b[35mmath_call:"); print_value(value);
    QrtBlock *block = ctx->block;
    print_value(value);
    QrtOpp *opp = block->opp;
    if(!asCtlInt(value)){
        pop_opp(block);
        return value;
    }

    int regval = block->reg != NULL ? asCtlInt(block->reg)->value : -1000;
    printf("\x1b[36m%d %c %d\x1b[0m\n", regval, opp->opp_type, asCtlInt(value)->value); 


    if(block->reg == NULL){
        block->reg = value;
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
    return block->reg;
}

CtlAbs *opp_assign_call(QrtCtx *ctx, CtlAbs *value){
    printf("opp_assign_call\n");
    QrtBlock *block = ctx->block;
    printf("assign................\n");
    QrtSymbol *symbol;
    if((symbol = asQrtSymbol(block->opp->value))){
        if(symbol->name->data[0] == ':' || symbol->name->data[0] == '&'){
            symbol->name->data++;
            symbol->name->length--;
            ctl_tree_insert(block->namespace, (CtlAbs *)symbol->name, value);
        }else{
            value = ctl_tree_get(block->namespace, (CtlAbs *)symbol->name);
            if(!value){
                ctl_xerrlog("value not found");
            }
        }
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
