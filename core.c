
enum qrt_opp_types {
    QRT_PLUS = '+',
    QRT_MINUS = '-',
    QRT_DIVIDE = '/',
    QRT_MULTIPLY = '*',
    QRT_GREATER = '>',
    QRT_LESS = '<',
    QRT_NOT = '!'
};

typedef struct qrt_cell {
    struct base base;
    int status;
    int id;
    CtlAbs *value;
    struct qrt_cell *next;
    struct qrt_cell *previous;
} QrtCell;

struct qrt_statement;
typedef struct qrt_block {
    struct base base;
    struct qrt_block *parent;
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
    QrtCell *cell_root;
    QrtCell *cell_next;
} QrtStatement;

typedef struct qrt_ctx {
    struct base base;
    QrtBlock *root;
    CtlCounted *shelf;
    QrtCell *start; 
} QrtCtx;

typedef struct qrt_opp {
    struct base base;
    char opp_type;
    struct qrt_cell *(*call)(struct qrt_opp *opp, CtlAbs *a, CtlAbs *b);
} QrtOpp;

typedef struct qrt_symbol {
   struct base base;
   QrtCell *parent;
   CtlCounted *name;
   CtlAbs *value;
   int is_define;
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
QrtSymbol *qrt_symbol_alloc(QrtCell *parent, int is_define){
    QrtSymbol *symbol;
    ctl_xptr(symbol = malloc(sizeof(QrtSymbol)));
    bzero(symbol, sizeof(QrtSymbol));
    symbol->base.class = CLASS_SYMBOL;
    symbol->base.id = ++qrt_symbol_id;
    symbol->is_define = is_define;
    symbol->parent = parent;
    return symbol;
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

QrtStatement *qrt_statement_alloc(QrtBlock *parent, QrtStatement *previous, struct qrt_cell *root){
    QrtStatement *statement;
    ctl_xptr(statement = malloc(sizeof(QrtStatement)));
    statement->base.class = CLASS_STATEMENT;
    statement->previous = previous;
    statement->parent = parent;
    statement->cell_root = statement->cell_next = root;
    return statement;
}

QrtBlock *qrt_block_alloc(char type, QrtBlock *parent){
    QrtBlock *block;
    ctl_xptr(block = malloc(sizeof(QrtBlock)));
    block->base.class = CLASS_BLOCK;
    block->namespace = ctl_tree_alloc(ctl_tree_counted_cmp);
    block->values = ctl_tree_alloc(ctl_tree_counted_cmp);
    block->statement_root = block->statement_next = qrt_statement_alloc(block, NULL, NULL);
    block->type = type;
    return block;
}

int qrt_ctx_id = 0;
struct qrt_ctx * qrt_ctx_alloc(){
    struct qrt_ctx *ctx;
    ctl_xptr(ctx = malloc(sizeof(struct qrt_ctx)));
    bzero(ctx, sizeof(struct qrt_ctx));
    ctx->base.class = CLASS_CTX;
    ctx->base.id = ++qrt_ctx_id;
    ctx->root = qrt_block_alloc('{', NULL);
    ctx->start = qrt_cell_alloc(); 
    return ctx;
}

