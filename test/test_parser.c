#include "../src/common.h"
#include "../src/tokenizer.h"
#include "../src/parser.h"

#include "../src/common.c"
#include "../src/tokenizer.c"
#include "../src/parser.c"

#define TEST_ID(n) at = n##Token; \
    Identifier *n##Id = parse_identifier(&at); \
    i_expect(strings_are_equal(n##Id->name, n##String))
#define TEST_AGAIN(n) at = n##Token; \
    Identifier *n##Again = parse_identifier(&at); \
    i_expect(n##Again == n##Id)


int test_parse_identifiers(void)
{
    String xString = create_string("x");
    String yString = create_string("y");
    String zString = create_string("z");
    String aString = create_string("a");
    String bString = create_string("b");
    String cString = create_string("c");
    String _String = create_string("_");
    String underString = create_string("_sadsad");
    String underNumString = create_string("_01654");
    String longString = create_string("das646sd_asda56aw5e");

    Token *xToken = tokenize_string(xString);
    Token *yToken = tokenize_string(yString);
    Token *zToken = tokenize_string(zString);
    Token *aToken = tokenize_string(aString);
    Token *bToken = tokenize_string(bString);
    Token *cToken = tokenize_string(cString);
    Token *_Token = tokenize_string(_String);
    Token *underToken = tokenize_string(underString);
    Token *underNumToken = tokenize_string(underNumString);
    Token *longToken = tokenize_string(longString);

    Token *TEST_ID(x);
    TEST_ID(y);
    TEST_ID(z);
    TEST_ID(a);
    TEST_ID(b);
    TEST_ID(c);
    TEST_ID(_);
    TEST_ID(under);
    TEST_ID(underNum);
    TEST_ID(long);

    TEST_AGAIN(x);
    TEST_AGAIN(y);
    TEST_AGAIN(z);
    TEST_AGAIN(a);
    TEST_AGAIN(b);
    TEST_AGAIN(c);
    TEST_AGAIN(_);
    TEST_AGAIN(under);
    TEST_AGAIN(underNum);
    TEST_AGAIN(long);

    i_expect(xAgain != yId);
    i_expect(yAgain != zId);

    return 0;
}

#undef TEST_ID
#undef TEST_AGAIN

int test_parse_parenthesize(void)
{
    String parenthesized = str_internalize_cstring("(4 + 5) * 2;");
    String unparenthesized = str_internalize_cstring("4 + 5 * 2;");

    Token *tokenParen = tokenize_string(parenthesized);
    Token *tokenNonParen = tokenize_string(unparenthesized);

    Expression *exprParen = parse_expression(&tokenParen);
    Expression *exprNonParen = parse_expression(&tokenNonParen);

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
    errors |= test_parse_identifiers();
    errors |= test_parse_parenthesize();
    errors |= test_parse_statement();
    return errors;
}

int main(int argc, char **argv)
{
    int errors = test_parser();

    return errors;
}
