CtlAbs *onBlock(QrtMapper *map, QrtBlock *block){
    print_block(block, map->space, 0);
}
CtlAbs *onStmt(QrtMapper *map, QrtStatement *stmt){
    print_stmt(stmt, map->space);
}
CtlAbs *onCell(QrtMapper *map, QrtCell *cell){
    print_node(cell, map->space);
}

CtlAbs *exec_expressions(QrtBlock *block, QrtStatement *stmt){
    /* sum up and return a value */
    /* assign symbol values */
    /* operators ! + * */
    QrtCell *next = stmt->cell_root;
    CtlAbs *follows;
    while(block){
        if(block->type == '{')
            break;
         block = block->parent;
    }
    if(next){
        stmt->cell_lead = next;
        next = stmt->cell_root = next->next;
    }
    while(next){
        if(next->value){
            if(next->value->base.class == CLASS_SYMBOL){
                QrtSymbol *symbol = asQrtSymbol(next->value);
                if(next->next && next->next->value){
                    follows = asQrtCell(next->next)->value;
                    if(follows->base.class == CLASS_INT){
                        if(symbol->is_define){
                            symbol->value = follows;    
                            ctl_tree_insert(block->namespace, symbol->name, follows);
                            next->next = next->next->next;
                        }
                    }else if(follows->base.class == CLASS_BLOCK){
                        follows = asQrtBlock(next->next->value);
                        if(symbol->is_define){
                            symbol->value = follows;    
                            ctl_tree_insert(block->namespace, symbol->name, follows);
                            next->next = next->next->next;
                        }
                    }
                }
            }
        }
        next = next->next;
    }
}

CtlAbs *exec_statements(QrtBlock *block, QrtMapper *map){
    QrtStatement *stmt =  block->statement_root;
    while(stmt){
        map->onStmt(map, stmt);
        /*
        exec_expressions(block, stmt);
        */
        stmt = stmt->next;
    }
}


CtlAbs *pre_proc_exec(QrtCtx *ctx, QrtMapper *map){
    Crray *stack = ctl_crray_alloc(16);
    QrtBlock *block = ctx->root;
    QrtBlock *prev;
    QrtBlock *current;
    ctl_crray_push(stack, block);
    while(block){
        map->onBlock(map, block);
        exec_statements(block, map);
        if(block->branch){
            ctl_crray_push(stack, block);
            block = block->branch;
            if(block->branch)
                continue;
            map->onBlock(map, block);
            exec_statements(block, map);
        }
        if(block->next == NULL && stack->length > 1){
            prev = ctl_crray_pop(stack, -1); 
            block = prev;
        }
        block = block->next;
    } 
}

void exec(QrtCtx *ctx){
    QrtMapper *map = qrt_mapper_alloc(ctx, onBlock, onStmt, onCell);
    pre_proc_exec(ctx, map);
}
