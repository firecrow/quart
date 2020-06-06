/* Copyright 2020 Firecrow Silvernight (fire@firecrow.com) licensed under the MIT License see LICENSE file */
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

int is_symbol_type(char c){
    return (c == ':' || c == '.' || c == '&');
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
    if(!value) return "";
    int class = value->base.class;
    if(class == CLASS_INT){
        return ctl_counted_to_cstr(ctl_counted_format("INT(%d)", asCtlInt(value)->value));
    /*
    }else if(class == CLASS_BLOCK){
        QrtBlock *block = (QrtBlock *)value;
        if(block->type == '{') return "BLOCK_OPEN";
        else return  "BLOCK_CLOSE";
    */
    }else if(class == CLASS_COUNTED){
        return "COUNTED";
    }else if(class == CLASS_SEP){
        return "SEPORATOR";
    }else if(class == CLASS_OPP){
        return "OPERATOR";
    }else if(class == CLASS_BLOCK){
        return "BLOCK";
    }else if(class == CLASS_STATEMENT){
        return "STATEMENT";
    }else if(class == CLASS_SYMBOL){
        QrtSymbol *symbol = (QrtSymbol *)value;
        if(symbol->type == ':') return "DEFINITION";
        else if(symbol->type == '.') return "LOCAL_DEFINITION";
        else if(symbol->type == '&') return "REASSIGN";
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
        case CLASS_BLOCK:
            return 1;
        default:
            return 0;
    }
}

int is_break_value(CtlAbs *value){
    if(!value)
        return 0;
    switch(value->base.class){
        case CLASS_SEP:
            return 1;
        default:
            return 0;
    }
}

int is_valid_cell_next(QrtCell *cell){
   QrtBlock *block;
   if((block = asQrtBlock(cell->value)) && block->type == '}'){
       return 0; 
   }
   return 1;
}

int is_breaking_value(CtlAbs *value){
   if(asQrtBlock(value)){
       return 1; 
   }
   if(asQrtSep(value)){
       return 1; 
   }
   return 0;
}


#define QrtError(XARG) \
    if(XARG)\
        printf("RuntimeError %s:%d:%s %s\n", __FILE__, __LINE__, __func__, XARG); \
