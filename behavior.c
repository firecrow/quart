/* Copyright 2020 Firecrow Silvernight (fire@firecrow.com) licensed under the MIT License see LICENSE file */

CtlAbs *math_call(QrtOpp *opp, QrtCell * args){
    char type = opp->opp_type;

    CtlInt *value = ctl_int_alloc(asCtlInt(args->value)->value);
    args = args->next;
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
