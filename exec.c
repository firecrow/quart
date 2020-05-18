CtlAbs *exec_expressions(QrtBlock *block, QrtStatement *stmt){
    /* sum up and return a value */
    /* assign symbol values */
    /* operators ! + * */
    QrtCell *next = stmt->cell_root;
    while(block){
        if(block->type == '{')
            break;
         block = block->parent;
    }

    CtlAbs *follows;
    while(next){
        if(next->value){
            if(next->value->base.class == CLASS_SYMBOL){
                QrtSymbol *symbol = asQrtSymbol(next->value);
                if(next->next && next->next->value){
                    follows = asQrtCell(next->next)->value;
                    if(follows->base.class == CLASS_INT){
                        symbol->value = follows;    
                        if(symbol->is_define){
                            ctl_tree_insert(block->namespace, symbol->name, follows);
                            /* skip this cell*/
                            next->next = next->next->next;
                        }
                    }
                }
            }
        }
        next = next->next;
    }
}

CtlAbs *exec_statements(QrtBlock *block){
    /* if/else etc */
    printf("hello\n");
    QrtStatement *stmt =  block->statement_root;
    while(stmt){
        printf(";\n");
        exec_expressions(block, stmt);
        stmt = stmt->next;
    }

}

CtlAbs *exec(QrtCtx *ctx){
    printf("executing\n");
    Crray *stack = ctl_crray_alloc(16);
    QrtBlock *block = ctx->root;
    QrtBlock *prev;
    QrtBlock *current;
    ctl_crray_push(stack, block);
    while(block){
        exec_statements(block);
        if(block->branch){
            ctl_crray_push(stack, block);
            block = block->branch;
            if(block->branch)
                continue;
            exec_statements(block);
        }
        if(block->next == NULL && stack->length > 1){
            prev = ctl_crray_pop(stack, -1); 
            block = prev;
        }
        block = block->next;
    } 
}
