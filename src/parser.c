
typedef struct Constant
{
    int value;
} Constant;

typedef struct Identifier
{
    String name;
    int associatedValue;
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
    EXPRESSION_ADD,
    EXPRESSION_SUB,
} ExpressionKind;
typedef struct Expression
{
    ExpressionKind kind;
    Variable *left;
    Variable *right;
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

internal Assignment *
parse_assignment(Token **at)
{
    *at = (*at)->nextToken;
    return NULL;
}

internal Expression *
parse_expression(Token **at)
{
    *at = (*at)->nextToken;
    return NULL;
}

internal void
parse_statement(Token **at, Statement *statement)
{
    if ((*at)->nextToken && ((*at)->nextToken->kind == TOKEN_ASSIGN))
    {
        statement->kind = STATEMENT_ASSIGN;
        statement->assign = parse_assignment(at);
    }
    else
    {
        statement->kind = STATEMENT_EXPR;
        statement->expr = parse_expression(at);
    }
}

internal Program *
parse(char *filename)
{
    Token *tokens = tokenize(filename);

    Program *program = allocate_struct(Program, 0);

    Token *at = tokens;
    while (at)
    {
        i_expect(program->nrStatements < MAX_NR_STATEMENTS);
        parse_statement(&at, program->statements + program->nrStatements++);
    }

    return program;
}

internal void
print_parsed_program(Program *program)
{

}
