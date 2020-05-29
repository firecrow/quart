/* Copyright 2020 Firecrow Silvernight (fire@firecrow.com) licensed under the MIT License see LICENSE file */

CtlAbs *math_call(QrtBlock *block, CtlAbs *value){
    QrtOpp *opp = block->opp;
    if(!asCtlInt(value)){
        block->opp = NULL;
        block->reg = NULL;
        return value;
    }
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
