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

typedef enum ExpressionKind
{
    EXPRESSION_NULL,
    EXPRESSION_ADD,
    EXPRESSION_SUB,
} ExpressionKind;
typedef struct Expression
{
    ExpressionKind kind;
    Variable *left;
    struct Expression *right;
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
