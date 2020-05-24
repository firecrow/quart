/* Copyright 2020 Firecrow Silvernight (fire@firecrow.com) licensed under the MIT License see LICENSE file */

CtlAbs *get_compatible_value(QrtBlock *block, QrtCell *cell, int class){
    QrtSymbol *symbol;
    CtlAbs *value;

    if((symbol = asQrtSymbol(cell->value))){
        value = ctl_tree_get(block->namespace, (CtlAbs *)symbol->name);
    }else{
        value = cell->value;
    }
    if(!value){
        return NULL;
    }
    if(value->base.class != class){
        return NULL;
    }
    if(asCtlInt(value)){
        return value;
    }
    return NULL;
}


QrtCell *call(QrtCtx *ctx, QrtBlock *block, QrtCell *actor, QrtCell *args){
    QrtOpp *opp;
    QrtSymbol *symbol;
    QrtSep *sep = NULL;
    CtlInt *value =  NULL;
    CtlInt *local = NULL;;
    CtlAbs *actor_value =  actor->value;
    if(!args)
        return actor;
    /* func */
    if((sep = asQrtSep(actor_value))){
        ctx->reg = NULL;
        return args;
    }
    if((opp = asQrtOpp(actor_value))){
        char type = opp->opp_type;
        while(args){
            local = asCtlInt(get_compatible_value(block, args, CLASS_INT));
            if(value == NULL){
                value = ctl_int_alloc(local->value);
                args = args->next;
                continue;
            }
            if(!local){
                ctx->reg = 0;
                return args;
            }
            switch(type){
                case '*':
                    value->value *= local->value;
                    break;
                case '+':
                    value->value += local->value;
                    break;
                case '-':
                    value->value -= local->value;
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
            symbol->name->length--;
            symbol->name->data++;
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
