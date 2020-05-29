/* Copyright 2020 Firecrow Silvernight (fire@firecrow.com) licensed under the MIT License see LICENSE file */
char *get_node_value_str(CtlAbs *value){
    char *node_value = "";
    if(value->base.class == CLASS_BLOCK){
        node_value = ctl_counted_to_cstr(ctl_counted_format("%c", ((QrtBlock *)value)->type));
    }else if(value->base.class == CLASS_OPP){
        node_value = ctl_counted_to_cstr(ctl_counted_format("%c", ((QrtOpp *)value)->opp_type));
    }else if(value->base.class == CLASS_INT){
        node_value = ctl_counted_to_cstr(ctl_counted_format("%d", asCtlInt(value)->value));
    }else if(value->base.class == CLASS_SYMBOL){
        QrtSymbol *symbol = (QrtSymbol *)value;
        node_value = ctl_counted_to_cstr(symbol->name);
        if(symbol->value){
            CtlAbs *value = symbol->value;
            if(value->base.class == CLASS_INT){
                node_value = ctl_counted_to_cstr(ctl_counted_format("%s %d", node_value, asCtlInt(value)->value));
            }
            if(value->base.class == CLASS_BLOCK){
                node_value = ctl_counted_to_cstr(ctl_counted_format("%s %c", node_value, asQrtBlock(value)->type));
            }
        }
    }
    return node_value;
}

void print_block(QrtBlock *block){
    char *node_value = ctl_counted_to_cstr(ctl_counted_format("%c", block->type));
    int prev_id = block->parent != NULL ? block->parent->base.id  : -1;

    int r = 0;
    CtlInt *reg = asCtlInt(block->reg); 
    if(reg){
       r = reg->value; 
    }
    /*
    printf("|%s %c id:%d p:%d %d|\n",
        block->type == '{' ? "BLOCK" : "BCELL", block->type, block->base.id, prev_id, r 
    );
    */
    printf("\x1b[31m%c>> #%d(%d) %d\x1b[0m\n",
        block->type, block->base.id, prev_id, r 
    );

    CtlTreeIter *iter = ctl_tree_iter(block->namespace);
    if(iter->size(iter)){
        CtlTreeNode *node = NULL;
        while((node = iter->next(iter)) != NULL){
            printf("\x1b[33m(%s):%s %s\x1b[0m\n", ctl_to_cstr(asCtlCounted(node->key)), get_class_str((CtlAbs *)node->data), get_node_value_str((CtlAbs *)node->data));
        }
    }

}
void print_indent(int i){
    while(i--){ printf(" ");}
}

void print_value(CtlAbs *value){
    if(!value){
        printf("[\x1b[33mNULL\x1b[0m]\n");
    }
    printf("[%s \x1b[33m%s\x1b[0m]\n", get_class_str(value), get_node_value_str(value));
}

void print_cell(QrtCell *cell){
    int next_id = cell->next ? cell->next->base.id : -1;
    printf("<%s %d \x1b[33m%s\x1b[0m ->%d>\n", get_class_str(cell->value), cell->value->base.id, get_node_value_str(cell->value), next_id);
}
