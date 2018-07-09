#include "../src/common.h"
#include "../src/tokenizer.h"
#include "../src/parser.h"

#include "../src/common.c"
#include "../src/tokenizer.c"
#include "../src/parser.c"

// TODO(michiel): Correct spelling
int test_parse_parentisize(void)
{
    String parentisized = create_string("(4 + 5) * 2;");
    String unparentisized = create_string("4 + 5 * 2;");

    Token *tokenParen = tokenize_string(parentisized);
    Token *tokenNonParen = tokenize_string(unparentisized);

    Expression *exprParen = parse_expression(&tokenParen);
    Expression *exprNonParen = parse_expression(&tokenNonParen);

    i_expect(exprParen);
    i_expect(exprParen->op == EXPR_OP_MUL);
    i_expect(exprParen->leftKind == EXPRESSION_EXPR);
    i_expect(exprParen->leftExpr);
    i_expect(exprParen->leftExpr->op == EXPR_OP_ADD);
    i_expect(exprParen->leftExpr->left);
    i_expect(exprParen->leftExpr->left->kind == VARIABLE_CONSTANT);
    i_expect(exprParen->leftExpr->left->constant);
    i_expect(exprParen->leftExpr->left->constant->value == 4);
    i_expect(exprParen->leftExpr->right);
    i_expect(exprParen->leftExpr->right->kind == VARIABLE_CONSTANT);
    i_expect(exprParen->leftExpr->right->constant);
    i_expect(exprParen->leftExpr->right->constant->value == 5);
    i_expect(exprParen->rightKind == EXPRESSION_VAR);
    i_expect(exprParen->right);
    i_expect(exprParen->right->kind == VARIABLE_CONSTANT);
    i_expect(exprParen->right->constant);
    i_expect(exprParen->right->constant->value == 2);

    i_expect(exprNonParen);
    i_expect(exprNonParen->op == EXPR_OP_ADD);
    i_expect(exprNonParen->leftKind == EXPRESSION_VAR);
    i_expect(exprNonParen->left);
    i_expect(exprNonParen->left->kind == VARIABLE_CONSTANT);
    i_expect(exprNonParen->left->constant);
    i_expect(exprNonParen->left->constant->value == 4);
    i_expect(exprNonParen->rightKind == EXPRESSION_EXPR);
    i_expect(exprNonParen->rightExpr);
    i_expect(exprNonParen->rightExpr->op == EXPR_OP_MUL);
    i_expect(exprNonParen->rightExpr->left);
    i_expect(exprNonParen->rightExpr->left->kind == VARIABLE_CONSTANT);
    i_expect(exprNonParen->rightExpr->left->constant);
    i_expect(exprNonParen->rightExpr->left->constant->value == 5);
    i_expect(exprNonParen->rightExpr->right);
    i_expect(exprNonParen->rightExpr->right->kind == VARIABLE_CONSTANT);
    i_expect(exprNonParen->rightExpr->right->constant);
    i_expect(exprNonParen->rightExpr->right->constant->value == 2);

    return 0;
}

int test_parse_statement(void)
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

int test_parser(void)
{
    int errors = 0;
    errors |= test_parse_parentisize();
    errors |= test_parse_statement();
    return errors;
}

int main(int argc, char **argv)
{
    int errors = test_parser();

    return errors;
}
