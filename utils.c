int is_punc(char c){
    return (c == ';' || c == ',' || c == '<' || c == '>' || c == '=' || c == '{' || c == '}' || c == '[' || c == ']');
}

int is_cmp(char c){
    return (c == '*' || c == '+' || c == '-' || c == '!' || c == '/' || c == '>' || c == '<');
}

int is_block(char c){
    return (c == '{' || c == '}' || c == '[' || c == ']' || c == ',');
}

int is_sep(char c){
    return (c == ';' || c == '\n');
}

int is_alpha(char c){
    return (c >= 65 && c <= 90) || (c >=97 && c <= 122);
}

int is_numeric(char c){
    return (c >= 48 && c <= 57);
}

int is_alpha_numeric(char c){
    return is_numeric(c) || is_alpha(c);
}

char *get_class_str(CtlAbs *value){
    int class = value->base.class;
    if(class == CLASS_INT){
        return "INT";
    }else if(class == CLASS_BLOCK){
        QrtBlock *block = (QrtBlock *)value;
        if(block->type == '{') return "BLOCK_OPEN";
        else return  "BLOCK_CLOSE";
    }else if(class == CLASS_COUNTED){
        return "COUNTED";
    }else if(class == CLASS_SEP){
        return "SEPORATOR";
    }else if(class == CLASS_OPP){
        return "OPERATOR";
    }else if(class == CLASS_SEP){
        return "SPORATOR";
    }else if(class == CLASS_STATEMENT){
        return "STATEMENT";
    }else if(class == CLASS_SYMBOL){
        QrtSymbol *symbol = (QrtSymbol *)value;
        if(symbol->is_define) return "DEFINITION";
        else return "SYMBOL";
        return "POO";
    }else if(class == CLASS_CELL){
        return "CELL";
    }
    return "UNKNOWN";
}

int is_variable_value(CtlAbs *value){
    switch(value->base.class){
        case CLASS_INT:
        case CLASS_COUNTED:
        /*case CLASS_BLOCK:*/
            return 1;
    }
    return 0;
}
