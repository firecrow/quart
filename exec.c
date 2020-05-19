CtlAbs *onBlock(QrtMapper *map, QrtBlock *block){
    print_block(block, map->space, 0);
}
CtlAbs *onStmt(QrtMapper *map, QrtStatement *stmt){
    print_stmt(stmt, map->space);
}
CtlAbs *onCell(QrtMapper *map, QrtCell *cell){
    print_node(cell, map->space);
}

CtlAbs *exec_expressions(QrtBlock *block, QrtStatement *stmt, QrtMapper *map){
    QrtCell *next = stmt->cell_root;
    while(next){
        map->onCell(map, next);
        next = next->next;
    }
}

CtlAbs *exec_statements(QrtBlock *block, QrtMapper *map){
    QrtStatement *stmt =  block->statement_root;
    while(stmt){
        map->onStmt(map, stmt);
        exec_expressions(block, stmt, map);
        stmt = stmt->next;
    }
}


CtlAbs *mapper(QrtCtx *ctx, QrtMapper *map){
    Crray *stack = ctl_crray_alloc(16);
    QrtBlock *block = ctx->root;
    QrtBlock *prev;
    QrtBlock *current;
    ctl_crray_push(stack, block);
    while(block){
        map->onBlock(map, block);
        exec_statements(block, map);
        if(block->branch){
            map->space->length += 4;
            ctl_crray_push(stack, block);
            block = block->branch;
            if(block->branch)
                continue;
            map->onBlock(map, block);
            exec_statements(block, map);
        }
        if(block->next == NULL && stack->length > 1){
            map->space->length -= 4;
            prev = ctl_crray_pop(stack, -1); 
            block = prev;
        }
        block = block->next;
    } 
}

void mapper_example(QrtCtx *ctx){
    QrtMapper *map = qrt_mapper_alloc(ctx, onBlock, onStmt, onCell);
    mapper(ctx, map);
}
