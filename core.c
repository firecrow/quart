/* Copyright 2020 Firecrow Silvernight (fire@firecrow.com) licensed under the MIT License see LICENSE file */

typedef struct qrt_cell {
    struct base base;
    int status;
    CtlAbs *value;
    struct qrt_cell *next;
    struct qrt_cell *previous;
} QrtCell;


typedef struct qrt_ctx {
    struct base base;
    CtlCounted *shelf;
    QrtCell *start; 
    CtlAbs *reg;
} QrtCtx;

int qrt_ctx_id = 0;
struct qrt_ctx * qrt_ctx_alloc(){
    struct qrt_ctx *ctx;
    ctl_xptr(ctx = malloc(sizeof(struct qrt_ctx)));
    bzero(ctx, sizeof(struct qrt_ctx));
    ctx->base.class = CLASS_CTX;
    ctx->base.id = ++qrt_ctx_id;
    return ctx;
}

int qrt_cell_id=0;
struct qrt_cell *qrt_cell_alloc(){
    struct qrt_cell *node;
    ctl_xptr(node = malloc(sizeof(struct qrt_cell)));
    bzero(node, sizeof(struct qrt_cell));
    node->base.class = CLASS_CELL;
    node->base.id = ++qrt_cell_id;
    node->status = CTL_NOT_STARTED;
    return node;
}

/* new */
enum qrt_opp_types {
    QRT_PLUS = '+',
    QRT_MINUS = '-',
    QRT_DIVIDE = '/',
    QRT_MULTIPLY = '*',
    QRT_GREATER = '>',
    QRT_LESS = '<',
    QRT_NOT = '!'
};


struct qrt_statement;
typedef struct qrt_block {
    struct base base;
    struct qrt_block *parent;
    struct qrt_block *next;
    struct qrt_block *branch;
    struct qrt_statement *statement_root;
    struct qrt_statement *statement_next;
    CtlTree *namespace;
    CtlTree *values;
    char type;
} QrtBlock;

typedef struct qrt_statement {
    struct base base;
    struct qrt_block *parent;
    struct qrt_statement *previous;
    struct qrt_statement *next;
    QrtCell *cell_lead;
    QrtCell *cell_root;
    QrtCell *cell_next;
    CtlAbs * reg;
} QrtStatement;

/* may change to _func */
struct qrt_opp;
typedef struct qrt_opp {
    struct base base;
    char opp_type;
} QrtOpp;

typedef struct qrt_symbol {
   struct base base;
   QrtCell *parent;
   CtlCounted *name;
   CtlAbs *value;
   char type;
} QrtSymbol;

typedef struct qrt_sep {
   struct base base;
} QrtSep;

QrtSep *qrt_sep_alloc(){
    QrtSep *sep;
    ctl_xptr(sep = malloc(sizeof(QrtSep)));
    bzero(sep, sizeof(QrtSep));
    sep->base.class = CLASS_SEP;
    return sep;
}

int qrt_opp_id=0;
QrtOpp *qrt_opp_alloc(char type){
    QrtOpp *opp;
    ctl_xptr(opp = malloc(sizeof(QrtOpp)));
    bzero(opp, sizeof(QrtOpp));
    opp->base.class = CLASS_OPP;
    opp->base.id = ++qrt_opp_id;
    opp->opp_type = type;
    return opp;
}

int qrt_symbol_id=0;
QrtSymbol *qrt_symbol_alloc(QrtCell *parent, CtlCounted *name){
    QrtSymbol *symbol;
    ctl_xptr(symbol = malloc(sizeof(QrtSymbol)));
    bzero(symbol, sizeof(QrtSymbol));
    symbol->base.class = CLASS_SYMBOL;
    symbol->base.id = ++qrt_symbol_id;
    symbol->parent = parent;
    symbol->name = name;
    char c =  name->data[0];
    if(c != ':' && c != '.' && c != '&'){
        c = 'x';
    }
    symbol->type = c;
    return symbol;
}

int qrt_statement_id=0;
QrtStatement *qrt_statement_alloc(QrtBlock *parent, QrtStatement *previous, struct qrt_cell *root){
    QrtStatement *statement;
    ctl_xptr(statement = malloc(sizeof(QrtStatement)));
    statement->base.class = CLASS_STATEMENT;
    statement->base.id = ++qrt_statement_id;
    statement->previous = previous;
    statement->parent = parent;
    statement->cell_root = statement->cell_next = root;
    return statement;
}

int qrt_block_id = 0;
QrtBlock *qrt_block_alloc(char type, QrtBlock *parent){
    QrtBlock *block;
    ctl_xptr(block = malloc(sizeof(QrtBlock)));
    block->base.class = CLASS_BLOCK;
    block->base.id =  ++qrt_block_id;
    block->namespace = ctl_tree_alloc(ctl_tree_counted_cmp);
    block->values = ctl_tree_alloc(ctl_tree_counted_cmp);
    block->type = type;
    return block;
}

QrtBlock *ctl_block_incr(QrtBlock *block){
    return (QrtBlock *)ctl_ref_incr((CtlAbs *)block);
}

QrtBlock *ctl_block_decr(QrtBlock *block){
    return (QrtBlock *)ctl_ref_decr((CtlAbs *)block);
}

QrtSymbol *asQrtSymbol(CtlAbs *x){
    if(x && x->base.class != CLASS_SYMBOL){
        return NULL;
    }
    return (QrtSymbol *)x; 
}

QrtCell *asQrtCell(CtlAbs *x){
    if(x && x->base.class != CLASS_CELL){
        return NULL;
    }
    return (QrtCell *)x; 
}

QrtBlock *asQrtBlock(CtlAbs *x){
    if(x && x->base.class != CLASS_BLOCK){
        return NULL;
    }
    return (QrtBlock *)x; 
}

QrtOpp *asQrtOpp(CtlAbs *x){
    if(x && x->base.class != CLASS_OPP){
        return NULL;
    }
    return (QrtOpp *)x; 
}

typedef struct qrt_mapper {
    struct base base;
    void (*onBlock)(struct qrt_mapper *mapper, QrtBlock *block);
    void (*onStmt)(struct qrt_mapper *mapper, QrtStatement *stmt);
    void (*onCell)(struct qrt_mapper *mapper, QrtCell *cell);
    CtlCounted *space;
}QrtMapper;


QrtMapper *qrt_mapper_alloc(QrtCtx *ctx,
            void (*onBlock)(struct qrt_mapper *mapper, QrtBlock *block),
            void (*onStmt)(struct qrt_mapper *mapper, QrtStatement *stmt),
            void (*onCell)(struct qrt_mapper *mapper, QrtCell *cell)
        ){
    struct qrt_mapper *mapper;
    ctl_xptr(mapper = malloc(sizeof(struct qrt_mapper)));
    bzero(mapper, sizeof(struct qrt_mapper));
    mapper->base.class = CLASS_UNDEFINED;
    mapper->onBlock = onBlock;
    mapper->onStmt = onStmt;
    mapper->onCell = onCell;
    mapper->space = ctl_counted_from_cstr("                    ");
    mapper->space->length = 0;
    return mapper;
}
