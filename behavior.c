CtlAbs *math_call(QrtOpp *opp, QrtCell * args){
    printf("jsmkdjfksjdkfj\n");
     
    char type = opp->opp_type;

    CtlInt *value = ctl_int_alloc(type == '*' ? 1 : 0);
    while(args){
        if(!args->value)
            QrtError("value missing from call");
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
    return (CtlAbs *)value;
}
