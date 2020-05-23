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

void print_cell(QrtCell *cell){
    int prev_id = cell->prev ? cell->prev->base.id : -1;
    int next_id = cell->next ? cell->next->base.id : -1;
    printf("<%s %d:%d>\n", get_class_str(cell->value), prev_id, next_id);
}
