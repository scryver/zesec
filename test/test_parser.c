#include "../src/common.h"
#include "../src/tokenizer.h"
#include "../src/parser.h"

#include "../src/common.c"
#include "../src/tokenizer.c"
#include "../src/parser.c"

int test_parser(void)
{
    String assignment = create_string("x = 4;");
    Token *tokens = tokenize_string(assignment);
    Program *program = parse(tokens);

    i_expect(program->nrStatements == 1);
    i_expect(program->statements);

    Statement *statement = program->statements;
    i_expect(statement->kind == STATEMENT_ASSIGN);

    Assignment *assign = statement->assign;
    i_expect(assign->id);
    i_expect(assign->expr);

    Identifier *id = assign->id;
    i_expect(id->name.size == 1);
    i_expect(strings_are_equal(id->name, create_string("x")));

    Expression *expr = assign->expr;
    i_expect(expr->op == EXPR_OP_NOP);
    i_expect(expr->leftKind == EXPRESSION_VAR);
    i_expect(expr->left);
    i_expect(expr->rightKind == EXPRESSION_NULL);
    i_expect(expr->right == 0);

    Variable *var = expr->left;
    i_expect(var->kind == VARIABLE_CONSTANT);
    i_expect(var->constant);

    Constant *constant = var->constant;
    i_expect(constant->value == 4);

    return 0;
}

int main(int argc, char **argv)
{
    int errors = test_parser();

    return errors;
}
