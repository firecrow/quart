
void qrt_out(void *_n){
    struct node *n = (struct node *)_n;
    CtlCounted *key = (CtlCounted *)n->key;
    if(n->is_red) printf("\x1b[31m%s -> %s\x1b[0m", ctl_counted_to_cstr(key),get_class_str(n->data));
    else printf("%s -> %s", ctl_counted_to_cstr(key), get_class_str(n->data));
}

void print_node(struct qrt_cell *node, CtlCounted *space){
    if(space == NULL) space = ctl_counted_from_cstr("");
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
                if(value->base.class == CLASS_BLOCK){
                    node_value = ctl_counted_to_cstr(ctl_counted_format("%s %c", node_value, asQrtBlock(value)->type));
                }
            }
        }
        class = node->value->base.class;
        cell = (struct qrt_cell *)node->value;
    }
    printf("%s<%s \x1b[33m%s\x1b[0m id:%d pv:%d n:%d>\n",
        ctl_counted_to_cstr(space), get_class_str((CtlAbs *)cell), node_value, node->base.id, prev_id, next_id 
    );
}

void print_stmt(QrtStatement *stmt, CtlCounted *space){
    printf("%s; %d",
        ctl_counted_to_cstr(space), stmt->base.id 
    );
    if(stmt->cell_lead){
        printf("\x1b[35m");
        print_node(stmt->cell_lead, space);
        printf("\x1b[0m");
    }else{
    printf("\n");
    }

}

void print_statements(QrtBlock *block, CtlCounted *space){
    QrtStatement *stmt =  block->statement_root;
    QrtCell *next;
    stmt = block->statement_root;
    while(stmt){
        print_stmt(stmt, space);
        next =  stmt->cell_root;
        while(next){
            print_node(next, space);
            next =  next->next;
        }
        stmt = stmt->next;
    }
}

void print_block(QrtBlock *block, CtlCounted *space, int statements){
    if(space == NULL) space = ctl_counted_from_cstr("$$$");
    char *node_value = ctl_counted_to_cstr(ctl_counted_format("%c", block->type));
    int next_id = block->next != NULL ? block->next->base.id  : -1;
    int prev_id = block->parent != NULL ? block->parent->base.id  : -1;
    int branch_id = block->branch != NULL ? block->branch->base.id  : -1;
    int class = block->base.class;

    CtlTreeIter *iter = ctl_tree_iter(block->namespace);
    if(iter->size(iter)){
        CtlTreeNode *node = NULL;
        while((node = iter->next(iter)) != NULL){
            printf("\x1b[33m(%s):%s\x1b[0m\n", ctl_counted_to_cstr(node->key), get_class_str((CtlAbs *)node->data));
        }
    }

    printf("%s|%s id:%d p:%d n:%d branch:%d|\n",
        ctl_counted_to_cstr(space), block->type == '{' ? "BLOCK" : "BCELL", block->base.id, prev_id, next_id, branch_id 
    );
    if(statements){
        print_statements(block, space);
    }
}

void printStack(Crray *stack){
    printf("\nstack....................XXXXXXXXXXXXXXXXXXx\n");
    for(int i=0; i < stack->length; i++){
        print_block(stack->content[i], ctl_counted_from_cstr(""), 0);
    }
    printf("stack....................END\n\n\n");
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

void print_blocks(QrtCtx *ctx){
    CtlCounted * space = ctl_counted_alloc("                ", 16);
    space->length = 0;
    Crray *stack = ctl_crray_alloc(16);
    QrtBlock *block = ctx->root;
    QrtBlock *prev;
    QrtBlock *current;
    ctl_crray_push(stack, block);
    while(block){
        print_block(block, space, 1);
        if(block->branch){
            space->length += 4;
            ctl_crray_push(stack, block);
            block = block->branch;
            if(block->branch)
                continue;
            print_block(block, space, 1);
        }
        if(block->next == NULL && stack->length > 1){
            space->length -= 4;
            prev = ctl_crray_pop(stack, -1); 
            block = prev;
        }
        block = block->next;
    } 
}
