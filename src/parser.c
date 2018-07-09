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
        if (strings_are_equal(ident->name, (*at)->value))
        {
            result = ident;
            *at = (*at)->nextToken;
            break;
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
        if (!((*at)->kind == TOKEN_ID))
        {
            fprintf(stderr, "Unknown variable token: ");
            print_token((FileStream){stderr}, *at);
            fprintf(stderr, "\n");
            deallocate(result);
            result = 0;
        }
        else
        {
            result->kind = VARIABLE_IDENTIFIER;
            result->id = parse_identifier(at);
        }
    }
    return result;
}

// TODO(michiel): Clean up handling, so this isn't needed to get rid of empty results
internal inline Expression *
dust_off_expression(Expression *expr)
{
    if (expr)
    {
        while ((expr->op == EXPR_OP_NOP) &&
               (expr->leftKind == EXPRESSION_EXPR))
        {
            i_expect(expr->rightKind == EXPRESSION_NULL);
            i_expect(expr->rightExpr == 0);
            Expression *remove = expr;
            expr = expr->leftExpr;
            deallocate(remove);
        }
    }

    return expr;
}

internal inline Expression *
new_op_has_precedence(Expression *newOp, Expression *oldOp)
{
    while ((oldOp->rightKind == EXPRESSION_EXPR) &&
           (oldOp->rightExpr->op > newOp->op))
    {
        oldOp = oldOp->rightExpr;
    }
    newOp->left = oldOp->right;
    newOp->leftKind = oldOp->rightKind;
    i_expect(newOp->rightKind == EXPRESSION_VAR);
    oldOp->rightExpr = newOp;
    oldOp->rightKind = EXPRESSION_EXPR;
    return oldOp;
}

internal inline Expression *
old_op_has_precedence(Expression *newOp, Expression *oldOp)
{
    newOp->leftExpr = oldOp;
    newOp->leftKind = EXPRESSION_EXPR;
    return newOp;
}

internal inline Expression *
parse_expression_precedence(Token **at, Expression *curExpr, Expression *leftExpr, ExpressionOp op)
{
    Expression *result = curExpr;
    result->op = op;
    *at = (*at)->nextToken;
    result->right = parse_variable(at);
    result->rightKind = EXPRESSION_VAR;

    result = dust_off_expression(result);
    leftExpr = dust_off_expression(leftExpr);

    if (leftExpr)
    {
        if (leftExpr->op <= result->op)
        {
            result = old_op_has_precedence(result, leftExpr);
        }
        else
        {
            i_expect(leftExpr->op != EXPR_OP_NOP);
            result = new_op_has_precedence(result, leftExpr);
        }
    }

    return result;
}

#define CASE(x) case TOKEN_##x: { result = parse_expression_precedence(at, result, leftExpr, EXPR_OP_##x); } break

internal Expression *
parse_expression_mul_op(Token **at, Expression *leftExpr)
{
    b32 done = false;
    Expression *result = allocate_struct(Expression, 0);
    result->op = EXPR_OP_NOP;
    if (!leftExpr)
    {
        result->left = parse_variable(at);
        result->leftKind = EXPRESSION_VAR;
    }

    switch ((*at)->kind)
    {
        CASE(MUL);
        CASE(DIV);
        CASE(AND);
        CASE(SLL);
        CASE(SRL);
        CASE(SRA);
        default:
        {
            if (leftExpr)
            {
                result = leftExpr;
            }
            done = true;
        } break;
    }

    if (!done &&
        ((EXPR_OP_MUL <= result->op) && (result->op <= EXPR_OP_SRA)))
    {
        result = parse_expression_mul_op(at, result);
    }

    return result;
}

internal Expression *
parse_expression_add_op(Token **at, Expression *leftExpr)
{
    Expression *result = allocate_struct(Expression, 0);
    b32 done = false;
    result->op = EXPR_OP_NOP;
    if (!leftExpr)
    {
        result->leftExpr = parse_expression_mul_op(at, 0);
        result->leftKind = EXPRESSION_EXPR;
        if (result->leftExpr->op == EXPR_OP_NOP)
        {
            Expression *leftE = result->leftExpr;

            result->left = leftE->left;
            result->leftKind = EXPRESSION_VAR;
            deallocate(leftE);
        }
    }
    else
    {
        result->leftExpr = leftExpr;
        result->leftKind = EXPRESSION_EXPR;
    }

    switch ((*at)->kind)
    {
        CASE(SUB);
        CASE(ADD);
        CASE(OR);
        CASE(XOR);
        default:
        {
            result = parse_expression_mul_op(at, result);
            if (result->op == EXPR_OP_NOP)
            {
                done = true;
            }
        } break;
    }

    if (!done &&
        (EXPR_OP_MUL <= result->op) &&
        (result->op <= EXPR_OP_XOR))
    {
        result = parse_expression_add_op(at, result);
    }

    return result;
}

#undef CASE

internal Expression *
parse_expression(Token **at)
{
    return parse_expression_add_op(at, 0);
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
            if (!((at->kind == TOKEN_EOL) || (at->kind == TOKEN_SEMI)))
            {
                fprintf(stderr, "Statement not closed by newline or semi-colon!\nStuck at: ");
                print_token((FileStream){stderr}, at);
                fprintf(stderr, "\n");
            }
            i_expect((at->kind == TOKEN_EOL) || (at->kind == TOKEN_SEMI));
            at = at->nextToken;
        }
        while (at && ((at->kind == TOKEN_EOL) || (at->kind == TOKEN_SEMI)));
    }

    return program;
}

internal void
print_constant(Constant *constant, b32 verbose)
{
    char *format = "%d";
    if (verbose)
    {
        format = "(const %d)";
    }
    fprintf(stdout, format, constant->value);
}

internal void
print_identifier(Identifier *id, b32 verbose)
{
    char *format = "'%.*s'";
    if (verbose)
    {
        format = "(id '%.*s')";
    }
    fprintf(stdout, format, id->name.size, (char *)id->name.data);
}

internal void
print_variable(Variable *var, b32 verbose)
{
    if (verbose)
    {
        fprintf(stdout, "(var ");
    }
    if (!var)
    {
        fprintf(stdout, "EMPTY_VAR");
    }
    else
    {
        switch (var->kind)
        {
            case VARIABLE_NULL:
            {
                // TODO(michiel): Error?
            } break;

            case VARIABLE_IDENTIFIER:
            {
                print_identifier(var->id, verbose);
            } break;

            case VARIABLE_CONSTANT:
            {
                print_constant(var->constant, verbose);
            } break;

            INVALID_DEFAULT_CASE;
        }
    }
    if (verbose)
    {
        fprintf(stdout, ")");
    }
}

internal void
print_expression_op(Expression *expr, char *opstr, b32 verbose)
{
    fprintf(stdout, "(%s ", opstr);
    if (expr->leftKind == EXPRESSION_VAR)
    {
        print_variable(expr->left, verbose);
    }
    else
    {
        i_expect(expr->leftKind == EXPRESSION_EXPR);
        print_expression(expr->leftExpr, verbose);
    }

    fprintf(stdout, " ");

    if (expr->rightKind == EXPRESSION_VAR)
    {
        print_variable(expr->right, verbose);
    }
    else
    {
        i_expect(expr->rightKind == EXPRESSION_EXPR);
        print_expression(expr->rightExpr, verbose);
    }
    fprintf(stdout, ")");
}

#define CASE(name, printName) case EXPR_OP_##name: { print_expression_op(expr, #printName, verbose); } break

internal void
print_expression(Expression *expr, b32 verbose)
{
    if (verbose)
    {
        fprintf(stdout, "(expr ");
    }
    switch (expr->op)
    {
        case EXPR_OP_NOP:
        {
            if (expr->leftKind == EXPRESSION_VAR)
            {
                print_variable(expr->left, verbose);
            }
            else if (expr->leftKind == EXPRESSION_EXPR)
            {
                print_expression(expr->leftExpr, verbose);
            }
            else
            {
                fprintf(stdout, "EMPTY");
            }
        } break;

        CASE(MUL, mul);
        CASE(DIV, div);
        CASE(AND, and);
        CASE(SLL, sll);
        CASE(SRL, srl);
        CASE(SRA, sra);
        CASE(SUB, sub);
        CASE(ADD, add);
        CASE(OR, or);
        CASE(XOR, xor);

        INVALID_DEFAULT_CASE;
    }
}

#undef CASE

internal void
print_assignment(Assignment *assign, b32 verbose)
{
    fprintf(stdout, "(assign ");
    print_identifier(assign->id, verbose);
    fprintf(stdout, " ");
    print_expression(assign->expr, verbose);
    fprintf(stdout, ")");
}

internal void
print_statement(Statement *statement, b32 verbose)
{
    if (verbose)
    {
        fprintf(stdout, "(stmt ");
    }
    switch (statement->kind)
    {
        case STATEMENT_ASSIGN: print_assignment(statement->assign, verbose); break;
        case STATEMENT_EXPR:   print_expression(statement->expr, verbose); break;
        INVALID_DEFAULT_CASE;
    }
    if (verbose)
    {
        fprintf(stdout, ")");
    }
}

internal void
print_parsed_program(Program *program, b32 verbose)
{
    if (verbose)
    {
        fprintf(stdout, "(program \n  ");
    }
    for (u32 statementIndex = 0;
         statementIndex < program->nrStatements;
         ++statementIndex)
    {
        Statement *statement = program->statements + statementIndex;
        print_statement(statement, verbose);
        if (statementIndex < (program->nrStatements - 1))
        {
            fprintf(stdout, "\n%s", verbose ? "  " : "");
        }
    }
    if (verbose)
    {
        fprintf(stdout, ")\n");
    }
    else
    {
        fprintf(stdout, "\n");
    }
}
