typedef struct Symbol
{
    String name;
    int value;
} Symbol;

#define MAX_SYMBOLS 1024
global Symbol gSymbols[MAX_SYMBOLS];
global u32 gSymbolCount;

internal Symbol *
get_symbol(Identifier *id)
{
    Symbol *result = NULL;
    for (u32 symbolIndex = 0;
         symbolIndex < gSymbolCount;
         ++symbolIndex)
    {
        Symbol *symbol = gSymbols + symbolIndex;
        if ((symbol->name.size == id->name.size) &&
            (symbol->name.data == id->name.data))
        {
            result = symbol;
            break;
        }
    }
    return result;
}

internal Symbol *
get_or_create_symbol(Identifier *id)
{
    Symbol *result = get_symbol(id);
    if (!result)
    {
        i_expect(gSymbolCount < MAX_SYMBOLS);
        result = gSymbols + gSymbolCount++;
        result->name = id->name;
        result->value = 0;
    }
    return result;
}

internal int
get_variable_value(Variable *variable)
{
    int result = 0;
    if (variable->kind == VARIABLE_IDENTIFIER)
    {
        Symbol *symbol = get_symbol(variable->id);
        if (!symbol)
        {
            fprintf(stderr, "Unidentified symbol '%.*s' used in expression.\n",
                    variable->id->name.size, (char *)variable->id->name.data);
        }
        else
        {
            result = symbol->value;
        }
    }
    else
    {
        i_expect(variable->kind == VARIABLE_CONSTANT);
        result = variable->constant->value;
    }
    return result;
}

internal int
interpret_expression(Expression *expr)
{
    i_expect((expr->kind == EXPRESSION_NULL) ||
             (expr->kind == EXPRESSION_ADD) ||
             (expr->kind == EXPRESSION_SUB));

    int result = get_variable_value(expr->left);

    do
    {
        if (expr->kind == EXPRESSION_ADD)
        {
            int right = get_variable_value(expr->right->left);
            result += right;
        }
        else if (expr->kind == EXPRESSION_SUB)
        {
            int right = get_variable_value(expr->right->left);
            result -= right;
        }
        expr = expr->right;
    }
    while (expr && ((expr->kind == EXPRESSION_ADD) ||
                    (expr->kind == EXPRESSION_SUB)));

    return result;
}

internal void
interpret_assign(Assignment *assign)
{
    Symbol *symbol = get_or_create_symbol(assign->id);
    symbol->value = interpret_expression(assign->expr);
}

internal void
interpret_statement(Statement *statement)
{
    if (statement->kind == STATEMENT_ASSIGN)
    {
        interpret_assign(statement->assign);
    }
    else
    {
        i_expect(statement->kind == STATEMENT_EXPR);
        (void)interpret_expression(statement->expr);
    }
}

internal void
print_symbol(Symbol *symbol)
{
    fprintf(stdout, "%.*s: %d", symbol->name.size, (char *)symbol->name.data,
            symbol->value);
}

internal void
print_interpretation(void)
{
    for (u32 symbolIndex = 0;
         symbolIndex < gSymbolCount;
         ++symbolIndex)
    {
        Symbol *symbol = gSymbols + symbolIndex;
        print_symbol(symbol);
        fprintf(stdout, "\n");
    }
}

internal void
interpret(Program *program)
{
    for (u32 statementIndex = 0;
         statementIndex < program->nrStatements;
         ++statementIndex)
    {
        Statement *statement = program->statements + statementIndex;
        interpret_statement(statement);
    }

    print_interpretation();
}
