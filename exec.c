/* Copyright 2020 Firecrow Silvernight (fire@firecrow.com) licensed under the MIT License see LICENSE file */

CtlAbs *get_compatible_value(QrtBlock *block, QrtCell *cell){
    QrtSymbol *symbol;
    CtlAbs *symbol_value;
    if((symbol = asQrtSymbol(cell->value))){
        symbol_value = ctl_tree_get(block->namespace, (CtlAbs *)symbol->name);
        return symbol_value;
    }
    if(asCtlInt(cell->value)){
        return cell->value;
    }
    return NULL;
}


QrtCell *call(QrtCtx *ctx, QrtBlock *block, QrtCell *actor, QrtCell *args){
    print_cell(actor);
    QrtOpp *opp;
    QrtSymbol *symbol;
    CtlAbs *actor_value =  actor->value;
    if(!args)
        return actor;
    /* func */
    if((opp = asQrtOpp(actor_value))){
        char type = opp->opp_type;

        CtlInt *value = asCtlInt(get_compatible_value(block, args));
        int local =  0;
        args = args->next;
        while(args){
            value = asCtlInt(get_compatible_value(block, args));
            if(!value){
                block->reg = 0;
                return args->next;
            }
            switch(type){
                case '*':
                    value->value *= local;
                    break;
                case '+':
                    value-> value += local;
                    break;
                case '-':
                    value->value -= local;
                    break;
                default:
                    break;
            }
            args = args->next;
        }
        ctx->reg = (CtlAbs *)value;
        return args;

    }
    if((symbol = asQrtSymbol(actor_value))){
        if(!is_variable_value(args->value)){
            ctx->reg = symbol->value;
            return args;
        }
        if(symbol->type == ':'){
            symbol->value = (CtlAbs *)args->value;
            ctl_tree_insert(block->namespace, (CtlAbs *)symbol->name, symbol->value);
        }else if(symbol->type == '&'){
            symbol->value = (CtlAbs *)args->value;
        }
        ctx->reg = args->value;
        return args->next;
    }
    ctx->reg = NULL;
    return args->next;
}

int exec(QrtCtx *ctx){
    QrtCell *cell = ctx->start;
    int value;

    QrtBlock *block = ctx->block;
    while(cell){
        cell = call(ctx, block, cell, cell->next);
        if(asCtlInt(ctx->reg)) value  = asCtlInt(ctx->reg)->value;
        else value = 0;
        printf("> reg:%d\n", value);
    }
    print_block(block);
    return 1;
}
