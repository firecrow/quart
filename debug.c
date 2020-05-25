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
    int next_id = block->next != NULL ? block->next->base.id  : -1;
    int prev_id = block->parent != NULL ? block->parent->base.id  : -1;
    int branch_id = block->branch != NULL ? block->branch->base.id  : -1;

    printf("|%s id:%d p:%d n:%d branch:%d|\n",
        block->type == '{' ? "BLOCK" : "BCELL", block->base.id, prev_id, next_id, branch_id 
    );

    CtlTreeIter *iter = ctl_tree_iter(block->namespace);
    if(iter->size(iter)){
        CtlTreeNode *node = NULL;
        while((node = iter->next(iter)) != NULL){
            printf("\x1b[33m(%s):%s\x1b[0m\n", ctl_to_cstr(asCtlCounted(node->key)), get_class_str((CtlAbs *)node->data));
        }
    }

}
void print_indent(int i){
    while(i--){ printf(" ");}
}

void print_cell(QrtCell *cell){
    int prev_id = cell->prev ? cell->prev->base.id : -1;
    int next_id = cell->next ? cell->next->base.id : -1;
    printf("<%s \x1b[33m%s\x1b[0m %d:%d>\n", get_class_str(cell->value), get_node_value_str(cell->value), prev_id, next_id);
}
