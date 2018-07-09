#define MAX_IDENTIFIERS 1024
global Identifier *gIdentifiers[MAX_IDENTIFIERS];
global u32 gIdentifierCount;

internal int
parse_number(String s)
{
    int result = 0;
    for (u32 sIdx = 0; sIdx < s.size; ++sIdx)
    {
        result *= 10;
        result += (s.data[sIdx] - '0');
    }
    return result;
}

internal Constant *
parse_constant(Token **at)
{
    i_expect((*at)->kind == TOKEN_NUMBER);
    Constant *result = allocate_struct(Constant, 0);
    result->value = parse_number((*at)->value);
    *at = (*at)->nextToken;
    return result;
}

internal Identifier *
parse_identifier(Token **at)
{
    i_expect((*at)->kind == TOKEN_ID);
    Identifier *result = 0;
    // NOTE(michiel): Check if we already got this identifier
    for (u32 idIndex = 0; idIndex < gIdentifierCount; ++idIndex)
    {
        Identifier *ident = gIdentifiers[idIndex];
        if (ident->name.size == (*at)->value.size)
        {
            result = ident;
            for (u32 nameIndex = 0; nameIndex < ident->name.size; ++nameIndex)
            {
                if (ident->name.data[nameIndex] != (*at)->value.data[nameIndex])
                {
                    result = 0;
                    break;
                }
            }

            if (result)
            {
                *at = (*at)->nextToken;
                break;
            }
        }
    }

    if (!result)
    {
        i_expect(gIdentifierCount < MAX_IDENTIFIERS);
        result = gIdentifiers[gIdentifierCount++] = allocate_struct(Identifier, 0);
        result->name = (*at)->value;
        *at = (*at)->nextToken;
    }
    return result;
}

internal Variable *
parse_variable(Token **at)
{
    Variable *result = allocate_struct(Variable, 0);
    if ((*at)->kind == TOKEN_NUMBER)
    {
        result->kind = VARIABLE_CONSTANT;
        result->constant = parse_constant(at);
    }
    else
    {
        result->kind = VARIABLE_IDENTIFIER;
        result->id = parse_identifier(at);
    }
    return result;
}

internal Expression *
parse_expression(Token **at)
{
    Expression *result = allocate_struct(Expression, 0);
    result->left = parse_variable(at);
    if ((*at)->kind == TOKEN_ADD)
    {
        result->kind = EXPRESSION_ADD;
        *at = (*at)->nextToken;
        result->right = parse_expression(at);
    }
    else if ((*at)->kind == TOKEN_SUB)
    {
        result->kind = EXPRESSION_SUB;
        *at = (*at)->nextToken;
        result->right = parse_expression(at);
    }
    else
    {
        result->kind = EXPRESSION_NULL;
    }
    return result;
}

internal Assignment *
parse_assignment(Token **at)
{
    Assignment *result = allocate_struct(Assignment, 0);
    result->id = parse_identifier(at);
    i_expect((*at)->kind == TOKEN_ASSIGN);
    *at = (*at)->nextToken;
    result->expr = parse_expression(at);
    return result;
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
parse(Token *tokens)
{
    Program *program = allocate_struct(Program, 0);

    Token *at = tokens;
    while (at)
    {
        i_expect(program->nrStatements < MAX_NR_STATEMENTS);
        parse_statement(&at, program->statements + program->nrStatements++);

        do
        {
            i_expect((at->kind == TOKEN_EOL) || (at->kind == TOKEN_SEMI));
            at = at->nextToken;
        }
        while (at && ((at->kind == TOKEN_EOL) || (at->kind == TOKEN_SEMI)));
    }

    return program;
}

internal void
print_constant(Constant *constant)
{
    fprintf(stdout, "%d", constant->value);
}

internal void
print_identifier(Identifier *id)
{
    fprintf(stdout, "'%.*s'", id->name.size, (char *)id->name.data);
}

internal void
print_variable(Variable *var)
{
    switch (var->kind)
    {
        case VARIABLE_NULL:
        {
           // TODO(michiel): Error?
        } break;

        case VARIABLE_IDENTIFIER:
        {
            print_identifier(var->id);
        } break;

        case VARIABLE_CONSTANT:
        {
            print_constant(var->constant);
        } break;

        INVALID_DEFAULT_CASE;
    }
}

internal void
print_expression(Expression *expr)
{
    switch (expr->kind)
    {
        case EXPRESSION_NULL:
        {
            print_variable(expr->left);
            // NOTE(michiel): Do nothing
        } break;

        case EXPRESSION_ADD:
        case EXPRESSION_SUB:
        {
            fprintf(stdout, "(%s ", expr->kind == EXPRESSION_ADD ? "add" : "sub");
            print_variable(expr->left);
            fprintf(stdout, " ");
            print_expression(expr->right);
            fprintf(stdout, ")");
        } break;

        INVALID_DEFAULT_CASE;
    }
}

internal void
print_assignment(Assignment *assign)
{
    fprintf(stdout, "(assign ");
    print_identifier(assign->id);
    fprintf(stdout, " ");
    print_expression(assign->expr);
    fprintf(stdout, ")");
}

internal void
print_statement(Statement *statement)
{
    switch (statement->kind)
    {
        case STATEMENT_ASSIGN: print_assignment(statement->assign); break;
        case STATEMENT_EXPR:   print_expression(statement->expr); break;
        INVALID_DEFAULT_CASE;
    }
}

internal void
print_parsed_program(Program *program)
{
    for (u32 statementIndex = 0;
         statementIndex < program->nrStatements;
         ++statementIndex)
    {
        Statement *statement = program->statements + statementIndex;
        print_statement(statement);
        fprintf(stdout, "\n");
    }
}
