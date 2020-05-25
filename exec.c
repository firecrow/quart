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
    print_indent(ctx->indent);print_cell(actor);
    QrtOpp *opp;
    QrtSymbol *symbol;
    QrtSep *sep = NULL;
    CtlInt *local = NULL;;
    CtlAbs *actor_value =  actor->value;
    QrtBlock *ablock;
    QrtBlock *nblock;
    if((opp = asQrtOpp(actor_value))){
        char type = opp->opp_type;
        while(args){
            local = asCtlInt(get_compatible_value(block, args, CLASS_INT));
            if(!local){
                return args;
            }
            if(ctx->reg == NULL){
                ctx->reg = (CtlAbs *)ctl_int_alloc(local->value);
                args = args->next;
                continue;
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
        if(symbol->type == ':' && block->state == QRT_AFTER_FUNC){
            printf("TRIGERED\n");
            block->state = QRT_TRIGGERED;
        }
        if(!is_variable_value(args->value)){ ctx->reg = symbol->value;
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
        if(nblock->type == '{'){
            ctx->indent += 4;
            ctl_crray_push(ctx->stack, (CtlAbs *)nblock);
            /*
            printf("AFTER_FUNC\n");
            nblock->cell = args;
            block->state = QRT_AFTER_FUNC;
            while(args){
                if((nblock = asQrtBlock(args->value)) && nblock->type == '}'){
                    return break_chain_cell(args); 
                }
                args = args->next;
            }
            */
        }else{
            ctx->indent -= 4;
            /*
            ctl_crray_push(ctx->stack, (CtlAbs *)nblock);
            if(ctx->stack->length){
                ablock = asQrtBlock(ctl_crray_pop(ctx->stack, -1));
                return ablock->cell;
            }else{
                return NULL;
            }
            */
        }
    }
    if((sep = asQrtSep(actor_value))){
        ctx->reg = NULL;
        if(ctx->stack->length && block->state == QRT_TRIGGERED ){
            printf("triggered\n");
            ctl_crray_pop(ctx->stack, -1);
            nblock = asQrtBlock(ctl_crray_tail(ctx->stack));
            if(nblock){
                return nblock->cell;
            }else{
                printf("ERROR: block not found on stack\n");
            }
        }
        block->state = QRT_OPEN;
    }
    return args;
}

int exec(QrtCtx *ctx){
    QrtCell *cell = ctx->start;
    QrtBlock *nblock;
    int value;

    QrtBlock *block = ctx->block;
    while(cell){
        cell = call(ctx, block, cell, cell->next);
        if(asCtlInt(ctx->reg)) value  = asCtlInt(ctx->reg)->value;
        else value = 0;
        print_indent(ctx->indent);printf("> reg:%d\n", value);
        if(cell == NULL && ctx->stack->length){
            nblock = asQrtBlock(ctl_crray_pop(ctx->stack, -1));
            if(nblock)
                cell = nblock->cell;
            
        }
    }
    print_block(block);
    return 1;
}
