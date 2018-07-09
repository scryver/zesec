#include "../src/common.h"
#include "../src/tokenizer.h"
#include "../src/parser.h"
//#include "../src/interpreter.h"

#include "../src/common.c"
#include "../src/tokenizer.c"
#include "../src/parser.c"
#include "../src/interpreter.c"

int test_interpret_expr(void)
{
    String tokenString = create_string("4 + 1;");
    Token *tokens = tokenize_string(tokenString);
    Program *program = parse(tokens);
    i_expect(program->nrStatements == 1);
    i_expect(program->statements[0].kind == STATEMENT_EXPR);

    Expression *expr = program->statements[0].expr;
    int value = interpret_expression(expr);
    i_expect(value == 4 + 1);

    return 0;
}

int test_interpret_assign(void)
{
    String tokenString = create_string("x = 9 + 7;");
    Token *tokens = tokenize_string(tokenString);
    Program *program = parse(tokens);
    i_expect(program->nrStatements == 1);
    i_expect(program->statements[0].kind == STATEMENT_ASSIGN);
    i_expect(program->statements[0].assign->expr);

    Symbol *symbol = interpret_assign(program->statements[0].assign);
    i_expect(symbol->value == 9 + 7);

    return 0;
}

int test_interpret_multi_statements(void)
{
    String tokenString = create_string("x = 9 + 7; x * 2 - 1\n");
    Token *tokens = tokenize_string(tokenString);
    Program *program = parse(tokens);
    i_expect(program->nrStatements == 2);
    i_expect(program->statements[0].kind == STATEMENT_ASSIGN);
    i_expect(program->statements[0].assign);
    i_expect(program->statements[0].assign->expr);
    i_expect(program->statements[1].kind == STATEMENT_EXPR);
    i_expect(program->statements[1].expr);

    Assignment *assign = program->statements[0].assign;
    Symbol *symbol = interpret_assign(assign);
    i_expect(symbol);
    i_expect(symbol->value == 9 + 7);

    Expression *expr = program->statements[1].expr;
    int value = interpret_expression(expr);
    i_expect(value == (9 + 7) * 2 - 1);

    return 0;
}

int test_interpret_program(void)
{
    String tokenString = create_string("x = 9 + 7; y = x * 2 - 1\nz = x & y;");
    Token *tokens = tokenize_string(tokenString);
    Program *program = parse(tokens);
    i_expect(program->nrStatements == 3);

    interpret(program);

    int x = 9 + 7;
    int y = x * 2 - 1;
    int z = x & y;

    i_expect(gSymbolCount == 3);
    i_expect(strings_are_equal(gSymbols[0].name, create_string("x")));
    i_expect(gSymbols[0].value == x);
    i_expect(strings_are_equal(gSymbols[1].name, create_string("y")));
    i_expect(gSymbols[1].value == y);
    i_expect(strings_are_equal(gSymbols[2].name, create_string("z")));
    i_expect(gSymbols[2].value == z);

    return 0;
}

int test_interpreter(void)
{
    int errors = 0;
    errors |= test_interpret_expr();
    errors |= test_interpret_assign();
    return errors;
}

int main(int argc, char **argv)
{
    int errors = test_interpreter();

    return errors;
}
