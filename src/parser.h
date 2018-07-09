typedef struct Constant
{
    int value;
} Constant;

typedef struct Identifier
{
    String name;
} Identifier;

typedef enum VariableKind
{
    VARIABLE_NULL,
    VARIABLE_IDENTIFIER,
    VARIABLE_CONSTANT,
} VariableKind;
typedef struct Variable
{
    VariableKind kind;
    union
    {
        Constant *constant;
        Identifier *id;
    };
} Variable;

typedef enum ExpressionOp
{
    EXPR_OP_NOP,
    EXPR_OP_ADD,
    EXPR_OP_SUB,
} ExpressionOp;
typedef enum ExpressionKind
{
    EXPRESSION_VAR,
    EXPRESSION_EXPR,
} ExpressionKind;
typedef struct Expression
{
    ExpressionOp op;

    ExpressionKind leftKind;
    union {
        Variable *left;
        struct Expression *leftExpr;
    };

    ExpressionKind rightKind;
    union {
        Variable *right;
        struct Expression *rightExpr;
    };
} Expression;

typedef struct Assignment
{
    Identifier *id;
    Expression *expr;
} Assignment;

typedef enum StatementKind
{
    STATEMENT_ASSIGN,
    STATEMENT_EXPR,
} StatementKind;
typedef struct Statement
{
    StatementKind kind;
    union
    {
        Assignment *assign;
        Expression *expr;
    };
} Statement;

#define MAX_NR_STATEMENTS 1024
typedef struct Program
{
    u32 nrStatements;
    Statement statements[MAX_NR_STATEMENTS];
} Program;
