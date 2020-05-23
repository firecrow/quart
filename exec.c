QrtCell *call(QrtCtx *ctx, QrtCell *actor, QrtCell *args){
    QrtOpp *opp;
    QrtSymbol *symbol;
    CtlAbs *actor_value =  actor->value;
    if(!args)
        return actor;
    /* func */
    if((opp = asQrtOpp(actor_value))){
        char type = opp->opp_type;

        CtlInt *value = ctl_int_alloc(asCtlInt(args->value)->value);
        args = args->next;
        while(args){
            if(!asCtlInt(args->value))
                break;
            switch(type){
                case '*':
                    value->value *=  asCtlInt(args->value)->value;
                    break;
                case '+':
                    value-> value += asCtlInt(args->value)->value;
                    break;
                case '-':
                    value->value -= asCtlInt(args->value)->value;
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
        /* define */
        if(symbol->type == ':'){
            symbol->value = (CtlAbs *)args->value;
        /* assign */
        }else if(symbol->type == '&'){
            symbol->value = (CtlAbs *)args->value;
        }
        ctx->reg = NULL;
        return args->next;
    }
    ctx->reg = NULL;
    return args->next;
}

int exec(QrtCtx *ctx){
    QrtCell *cell = ctx->start;
    int value;

    while(cell){
        cell = call(ctx, cell, cell->next);
        if(asCtlInt(ctx->reg)) value  = asCtlInt(ctx->reg)->value;
        else value = 0;
        printf("reg:%d\n", value);
    }
    return 1;
}
