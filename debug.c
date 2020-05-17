void qrt_out(void *_n){
    struct node *n = (struct node *)_n;
    CtlCounted *key = (CtlCounted *)n->key;
    if(n->is_red) printf("\x1b[31m%s -> %s\x1b[0m", ctl_counted_to_cstr(key),get_class_str(n->data));
    else printf("%s -> %s", ctl_counted_to_cstr(key), get_class_str(n->data));
}

void print_node(struct qrt_cell *node, CtlCounted *space){
    char *node_value = "";
    int next_id = node->next != NULL ? node->next->base.id  : -1;
    int prev_id = node->previous != NULL ? node->previous->base.id  : -1;
    int class = node->base.class;
    struct qrt_cell *cell = node;
    if(node->value){
        if(node->value->base.class == CLASS_BLOCK){
            node_value = ctl_counted_to_cstr(ctl_counted_format("%c", ((QrtBlock *)node->value)->type));
        }else if(node->value->base.class == CLASS_OPP){
            node_value = ctl_counted_to_cstr(ctl_counted_format("%c", ((QrtOpp *)node->value)->opp_type));
        }else if(node->value->base.class == CLASS_INT){
            node_value = ctl_counted_to_cstr(ctl_counted_format("%d", asCtlInt(node->value)->value));
        }else if(node->value->base.class == CLASS_SYMBOL){
            QrtSymbol *symbol = (QrtSymbol *)node->value;
            node_value = ctl_counted_to_cstr(symbol->name);
            if(symbol->value){
                CtlAbs *value = symbol->value;
                if(value->base.class == CLASS_INT){
                    node_value = ctl_counted_to_cstr(ctl_counted_format("%s %d", node_value, asCtlInt(value)->value));
                }
            }
        }
        class = node->value->base.class;
        cell = (struct qrt_cell *)node->value;
    }
    printf("%s<%s %s id:%d pv:%d n:%d>\n",
        ctl_counted_to_cstr(space), get_class_str((CtlAbs *)cell), node_value, node->base.id, prev_id, next_id 
    );
}

void print_sequence(QrtCtx *ctx){
    CtlCounted * space = ctl_counted_alloc("                ", 16);
    space->length = 0;
    QrtCell *cell = ctx->start;
    do {
        print_node(cell, space);
    }while((cell = cell->next));

}

void print_branches(QrtCtx *ctx){
    CtlCounted * space = ctl_counted_alloc("                ", 16);
    space->length = 0;

    QrtStatement *stmt = ctx->root->statement_root;

    QrtCell *cell;
    while(stmt){
        printf("----------------------------------statement boundry ------------------------------\n");
        cell = stmt->cell_root;
        while(cell){
            printf("X");
            print_node(cell, space);
            cell = cell->next;
        }
        
        stmt = stmt->next;
    }
}
