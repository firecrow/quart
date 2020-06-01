/* Copyright 2020 Firecrow Silvernight (fire@firecrow.com) licensed under the MIT License see LICENSE file */

CtlAbs *math_call(QrtBlock *block, CtlAbs *value){
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

CtlAbs *opp_assign_call(QrtBlock *block, CtlAbs *value){
    printf("assign................\n");
    QrtSymbol *symbol;
    if((symbol = asQrtSymbol(block->opp->value))){
        if(symbol->name->data[0] == ':' || symbol->name->data[0] == '&'){
            symbol->name->data++;
            symbol->name->length--;
        }
    }
    ctl_tree_insert(block->namespace, (CtlAbs *)symbol->name, value);
    pop_opp(block);
    return value;
}

QrtOpp *opp_assign(QrtSymbol *symbol){
    QrtOpp *opp = qrt_opp_alloc(':'); 
    opp->value = (CtlAbs *)symbol;
    opp->call = opp_assign_call;
    return opp;
}
