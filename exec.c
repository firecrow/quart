/* Copyright 2020 Firecrow Silvernight (fire@firecrow.com) licensed under the MIT License see LICENSE file */

QrtCell *break_chain_cell(QrtCell *cell){
    QrtCell *next;
    if(!cell) return NULL;
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
    printf("...");print_indent(ctx->indent);print_cell(actor);
    QrtOpp *opp;
    QrtSymbol *symbol;
    CtlAbs *value;
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
        printf("1\n");
        if(symbol->type == 'x'){
            value = ctl_tree_get(block->namespace, (CtlAbs *)symbol->name);
            printf("value exists %d\n", value != NULL);
            if(!value){
                printf("ERROR: value for symbol '%s' not found\n", ctl_to_cstr(symbol->name));
                return NULL;
            }
        }else{
            value = args->value;
        }
        if(!is_variable_value(value)){ 
            printf("not a variable\n");
            ctx->reg = symbol->value;
            return args;
        }
        printf("2\n");
        nblock = asQrtBlock(value);
        if(nblock)
            print_block(nblock);

        printf("3\n");
        if(symbol->type == ':' || symbol->type == '&'){
            printf("in assign\n");
            symbol->value = value;
            symbol->name->length--;
            symbol->name->data++;
            ctl_tree_insert(block->namespace, (CtlAbs *)symbol->name, symbol->value);
            if(nblock){
                printf("block here %c/%c\n", block->type, symbol->type);
                if(nblock->type == '{'){
                    printf("assigning a block cell to a symbol------------------------------------------>\n");
                    nblock->type = 'x';
                    nblock->cell = args;
                    while(args){
                        if((nblock = asQrtBlock(args->value)) && nblock->type == '}'){
                            break;
                        }
                        args = args->next;
                    }
                    return break_chain_cell(args);
                }
            }
        }else {
            printf("out of assign\n");
            printf("not a definition\n");
            nblock = asQrtBlock(value);
            printf("%c/%c\n", symbol->type, nblock->type);
            if(nblock && symbol->type == 'x' && nblock->type == 'x'){ 
                printf("retrieving a block cell to a symbol------------------------------------------<\n");
                printf("block running \n");
            }
        }
        ctx->reg = value;
        return args->next;
    }
    if((sep = asQrtSep(actor_value))){
        ctx->reg = NULL;
    }
    return args;
}

void exec_cells(QrtCtx *ctx, QrtBlock *block, QrtCell *cell){
    int value;
    while(cell){
        cell = call(ctx, block, cell, cell->next);
        if(asCtlInt(ctx->reg)) value  = asCtlInt(ctx->reg)->value;
        else value = 0;
        print_indent(ctx->indent);printf("> reg:%d\n", value);
        /*
        if(cell == NULL && ctx->stack->length){
            nblock = asQrtBlock(ctl_crray_pop(ctx->stack, -1));
            if(nblock)
                cell = nblock->cell;
            
        }
        */
    }
}

int exec(QrtCtx *ctx){
    QrtCell *cell = ctx->start;
    QrtBlock *nblock;
    QrtBlock *block = ctx->block;
    exec_cells(ctx, block,  cell);
    print_block(block);
    return 1;
}
