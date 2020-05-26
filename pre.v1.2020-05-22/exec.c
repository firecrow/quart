/* Copyright 2020 Firecrow Silvernight (fire@firecrow.com) licensed under the MIT License see LICENSE file */
void onBlock(QrtMapper *map, QrtBlock *block){
    print_block(block, map->space, 0);
}
void onStmt(QrtMapper *map, QrtStatement *stmt){
    QrtCell *cell = stmt->cell_root;
    print_node(cell, map->space);
    QrtOpp *opp;
    /* opp call */
    if((opp = asQrtOpp(cell->value))){
        stmt->reg = opp->call(opp, cell->next);
    }
    /* define : */
    /* asign  & */
    /* reg */
    print_stmt(stmt, map->space);
}
void onCell(QrtMapper *map, QrtCell *cell){
    print_node(cell, map->space);
}

void exec_expressions(QrtBlock *block, QrtStatement *stmt, QrtMapper *map){
    QrtCell *next = stmt->cell_root;
    while(next){
        if(map->onCell)
            map->onCell(map, next);
        next = next->next;
    }
}

void exec_statements(QrtBlock *block, QrtMapper *map){
    QrtStatement *stmt =  block->statement_root;
    while(stmt){
        if(map->onStmt)
            map->onStmt(map, stmt);
        exec_expressions(block, stmt, map);
        stmt = stmt->next;
    }
}


void mapper(QrtCtx *ctx, QrtMapper *map){
    Crray *stack = ctl_crray_alloc(16);
    QrtBlock *block = ctx->root;
    QrtBlock *prev;
    QrtBlock *current;
    ctl_crray_push(stack, (CtlAbs *)block);
    while(block){
        if(map->onBlock)
            map->onBlock(map, block);
        exec_statements(block, map);
        if(block->branch){
            map->space->length += 4;
            ctl_crray_push(stack, (CtlAbs *)block);
            block = block->branch;
            if(block->branch)
                continue;
            if(map->onBlock)
                map->onBlock(map, block);
            exec_statements(block, map);
        }
        if(block->next == NULL && stack->length > 1){
            map->space->length -= 4;
            prev = asQrtBlock(ctl_crray_pop(stack, -1)); 
            block = prev;
        }
        block = block->next;
    } 
}

