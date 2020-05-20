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

QrtBlock *push_block(Crray *stack, QrtBlock *current, QrtBlock *new){
    new->parent = current;
    current->branch = new;
    ctl_crray_push(stack, (CtlAbs *)ctl_block_incr(new));
    return new;
}

QrtBlock *pop_block(Crray *stack, QrtBlock *block, QrtBlock *new){
    QrtBlock *current, *next_block;
    ctl_crray_remove(stack,  -1);
    next_block = ctl_crray_tail(stack);
    current = next_block;
    while((next_block = next_block->next)) current = next_block;
    current->next = new;
    new->parent = current;
    return new;
}

QrtStatement *push_statement(QrtBlock *block, QrtStatement *stmt){
    if(block->statement_root == NULL)
        block->statement_root = block->statement_next = stmt;
    else
        block->statement_next->next =  stmt;
    return stmt;
}

void consolidate_value(QrtBlock *block, QrtCell *cell, int skip){
    if(cell->value && cell->value->base.class == CLASS_SYMBOL){
        QrtSymbol *symbol = asQrtSymbol(cell->value);
        symbol->value = get_following_value(cell);
        if(symbol->value){
            if(asCtlInt(symbol->value)){
                if(symbol->type != 'x'){
                    ctl_tree_insert(block->namespace, symbol->name, symbol->value);
                    if(!skip)
                        cell->next = cell->next->next;
                }
            }else if(asQrtBlock(symbol->value)){
                if(symbol->type != 'x'){
                    ctl_tree_insert(block->namespace, symbol->name, symbol->value);
                    if(!skip)
                        cell->next = cell->next->next;
                }
            }
        }
    }
}

CtlAbs *build_expressions(QrtBlock *block, QrtStatement *stmt){
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
    QrtBlock *new;
    QrtBlock *current;
    QrtBlock *next_block;
    
    QrtStatement *stmt = qrt_statement_alloc(block, NULL, cell);
    block->statement_root = block->statement_next = stmt;
    while(cell){
        if(new = asQrtBlock(cell->value)){
            ctl_crray_push(list, new);

            if(new->type == '{'){
                block = push_block(stack, block, new);
                cell = break_chain_cell(cell);
                stmt = push_statement(block, qrt_statement_alloc(block, NULL, cell));
                continue;
            }else{
                block =  pop_block(stack, current, new);
                stmt = push_statement(block, qrt_statement_alloc(block, NULL, cell));
                cell = break_chain_cell(cell);
                stmt->cell_root = cell;
                continue;
            }
        }
        if(is_break_value(cell->value)){
            cell = break_chain_cell(cell);
            stmt = push_statement(block, qrt_statement_alloc(block, NULL, cell));
            continue;
        }
        cell = cell->next;
    }
    return ctx;
}


QrtCtx *build(char *source){
    struct qrt_ctx *ctx = parse(source);
    blocks(ctx);
    pre_proc(ctx);
    return ctx;
}
