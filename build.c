CtlAbs *get_following_value(QrtCell *cell){
    if(cell->next && cell->next->value)
        return asQrtCell(cell->next)->value;
            
}
QrtCell *break_chain_cell(QrtCell *cell){
   QrtCell *next;
    next = cell->next;
    cell->next = NULL;
    return next;
}

void consolidate_value(QrtBlock *block, QrtCell *cell, int skip){
    if(cell->value && cell->value->base.class == CLASS_SYMBOL){
        QrtSymbol *symbol = asQrtSymbol(cell->value);
        symbol->value = get_following_value(cell);
        if(symbol->value){
            if(asCtlInt(symbol->value)){
                if(symbol->is_define){
                    ctl_tree_insert(block->namespace, symbol->name, symbol->value);
                    if(!skip)
                        cell->next = cell->next->next;
                }
            }else if(asQrtBlock(symbol->value)){
                if(symbol->is_define){
                    ctl_tree_insert(block->namespace, symbol->name, symbol->value);
                    if(!skip)
                        cell->next = cell->next->next;
                }
            }
        }
    }
}

CtlAbs *build_expressions(QrtBlock *block, QrtStatement *stmt){
    /* sum up and return a value */
    /* assign symbol values */
    /* operators ! + * */
    QrtCell *next = stmt->cell_root;
    while(block){
        if(block->type == '{')
            break;
         block = block->parent;
    }
    if(next){
        consolidate_value(block, next, 1);
        stmt->cell_lead = next;
        next = stmt->cell_root = break_chain_cell(next);
    }
    while(next){
        if(next->value){
            consolidate_value(block, next, 0);
        }
        next = next->next;
    }
}

CtlAbs *build_statements(QrtBlock *block){
    /* if/else etc */
    QrtStatement *stmt =  block->statement_root;
    while(stmt){
        build_expressions(block, stmt);
        stmt = stmt->next;
    }

}

CtlAbs *pre_proc(QrtCtx *ctx){
    Crray *stack = ctl_crray_alloc(16);
    QrtBlock *block = ctx->root;
    QrtBlock *prev;
    QrtBlock *current;
    ctl_crray_push(stack, block);
    while(block){
        build_statements(block);
        if(block->branch){
            ctl_crray_push(stack, block);
            block = block->branch;
            if(block->branch)
                continue;
            build_statements(block);
        }
        if(block->next == NULL && stack->length > 1){
            prev = ctl_crray_pop(stack, -1); 
            block = prev;
        }
        block = block->next;
    } 
}


QrtCtx *blocks(QrtCtx *ctx){
    QrtBlock *block = ctx->root;
    QrtBlock *newblock;
   
    QrtCell *start = ctx->start;
    QrtCell *cell = start;
    QrtCell *next = cell;
    Crray *stack = ctl_crray_alloc(16);
    ctl_crray_push(stack, block);
    Crray *list = ctl_crray_alloc(16);
    ctl_crray_push(list, block);
    QrtStatement *newstmt;
    
    QrtStatement *stmt = qrt_statement_alloc(block, NULL, cell);
    block->statement_root = block->statement_next = stmt;
    while(cell){
        if(cell->value && cell->value->base.class == CLASS_BLOCK){
            QrtBlock *new = (QrtBlock *)cell->value; 
            QrtBlock *current;
            QrtBlock *next;
            ctl_crray_push(list, new);

            CtlCounted *space = ctl_counted_alloc("hello", 5);
            space->length = 0;
            
            if(new->type == '{'){
                new->parent = block;
                block->branch = new;
                ctl_crray_push(stack, ctl_block_incr(new));
                block = new;

                cell = break_chain_cell(cell);
                newstmt = qrt_statement_alloc(block, NULL, cell);
                if(block->statement_root == NULL){
                    block->statement_root = block->statement_next = newstmt;
                }else{
                    block->statement_next->next =  newstmt;
                }
                block->statement_next = newstmt;
                stmt = newstmt;
                continue;
            }else{
                ctl_crray_remove(stack,  -1);
                next = ctl_crray_tail(stack);
                current = next;
                while((next = next->next)) current = next;
                current->next = new;
                new->parent = current;

                block = new;
                newstmt = qrt_statement_alloc(block, NULL, cell);
                if(block->statement_root == NULL){
                    block->statement_root = block->statement_next = newstmt;
                }else{
                    block->statement_next->next =  newstmt;
                }

                stmt = block->statement_root;


                cell = break_chain_cell(cell);
                stmt->cell_root = cell;
                continue;

            }

        }
        if(is_break_value(cell->value)){
            cell = break_chain_cell(cell);
            newstmt = qrt_statement_alloc(block, stmt, cell);
            if(block->statement_root == NULL){
                block->statement_root = block->statement_next = newstmt;
            }else{
                block->statement_next->next = newstmt;
            }
            block->statement_next = newstmt;
            continue;
        }
        cell = cell->next;
    }
    /*
    for(int i=0; i< list->length; i++){
        print_block(list->content[i], ctl_counted_from_cstr("+ "));
    }
    */
    return ctx;
}


QrtCtx *build(char *source){
    struct qrt_ctx *ctx = parse(source);
    blocks(ctx);
    pre_proc(ctx);
    return ctx;
}
