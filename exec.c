/* Copyright 2020 Firecrow Silvernight (fire@firecrow.com) licensed under the MIT License see LICENSE file */

QrtCell *break_chain_cell(QrtCell *cell){
    QrtCell *next;
    next = cell->next;
    cell->next = NULL;
    return next;
}

CtlAbs *get_compatible_value(QrtBlock *block, QrtCell *cell, int class){
    QrtSymbol *symbol;
    CtlAbs *value;

    if((symbol = asQrtSymbol(cell->value))){
        value = ctl_tree_get(block->namespace, (CtlAbs *)symbol->name);
    }else{
        value = cell->value;
    }
    if(!value){
        printf("ERROR: value for symbol '%s' not found\n", ctl_to_cstr(symbol->name));
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
    printf("               ");print_cell(actor);
    QrtOpp *opp;
    QrtSymbol *symbol;
    QrtSep *sep = NULL;
    CtlInt *local = NULL;;
    CtlAbs *actor_value =  actor->value;
    QrtBlock *nblock;
    if((opp = asQrtOpp(actor_value))){
        char type = opp->opp_type;
        while(args){
            print_cell(args);
            local = asCtlInt(get_compatible_value(block, args, CLASS_INT));
            if(ctx->reg == NULL){
                ctx->reg = ctl_int_alloc(local->value);
                args = args->next;
                continue;
            }
            if(!local){
                printf(".....");print_cell(args);
                return args;
            }
            switch(type){
                case '*':
                    asCtlInt(ctx->reg)->value *= local->value;
                    break;
                case '+':
                    asCtlInt(ctx->reg)->value += local->value;
                    break;
                case '-':
                    asCtlInt(ctx->reg)->value -= local->value;
                    break;
                default:
                    break;
            }
            args = args->next;
        }
        return args;
    }
    if((symbol = asQrtSymbol(actor_value))){
        if(!is_variable_value(args->value)){
            ctx->reg = symbol->value;
            return args;
        }
        if(symbol->type == ':' || symbol->type == '&'){
            symbol->value = (CtlAbs *)args->value;
            symbol->name->length--;
            symbol->name->data++;
            ctl_tree_insert(block->namespace, (CtlAbs *)symbol->name, symbol->value);
        }
        ctx->reg = args->value;
        return args->next;
    }
    if((nblock = asQrtBlock(actor_value))){
        /*print_block(nblock);*/
        if(nblock->type == '{'){
            ctl_crray_push(ctx->stack, (CtlAbs *)nblock);
            nblock->cell = args;
            while(args){
                if((nblock = asQrtBlock(args->value)) && nblock->type == '}'){
                    return break_chain_cell(args); 
                }
                args = args->next;
            }
        }
    }
    if((sep = asQrtSep(actor_value))){
        ctx->reg = NULL;
        if(ctx->stack->length > 0){
            if(nblock = asQrtBlock(ctl_crray_pop(ctx->stack, -1))){
                return nblock->cell;
            }else{
                printf("ERROR: block not found on stack\n");
            }
        }
    }
    return args;
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
