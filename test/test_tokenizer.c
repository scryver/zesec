#include "../src/common.h"
#include "../src/tokenizer.h"

#include "../src/common.c"
#include "../src/tokenizer.c"

#define TEST_TOKEN(token, type, string) i_expect(token); \
    i_expect(token->kind == TOKEN_##type); \
    i_expect(strings_are_equal(token->value, str_internalize_cstring(string))); \
    token = token->nextToken

int test_token_open_close(void)
{
    String tokenString = str_internalize_cstring("( ) (() (())(;");
    Token *tokens = tokenize_string(tokenString);

    Token *curToken = tokens;
    TEST_TOKEN(curToken, PAREN_OPEN, "(");
    TEST_TOKEN(curToken, PAREN_CLOSE, ")");
    TEST_TOKEN(curToken, PAREN_OPEN, "(");
    TEST_TOKEN(curToken, PAREN_OPEN, "(");
    TEST_TOKEN(curToken, PAREN_CLOSE, ")");
    TEST_TOKEN(curToken, PAREN_OPEN, "(");
    TEST_TOKEN(curToken, PAREN_OPEN, "(");
    TEST_TOKEN(curToken, PAREN_CLOSE, ")");
    TEST_TOKEN(curToken, PAREN_CLOSE, ")");
    TEST_TOKEN(curToken, PAREN_OPEN, "(");

    return 0;
}

int test_token_incdec(void)
{
    String tokenString = str_internalize_cstring("++a; --a; a++; a--; ++++a; ++--a; --++a; ----a; a++++; a++--; a--++; a----;");
    Token *tokens = tokenize_string(tokenString);

    Token *curToken = tokens;

    TEST_TOKEN(curToken, INC, "++");
    TEST_TOKEN(curToken, ID, "a");
    TEST_TOKEN(curToken, SEMI, ";");

    TEST_TOKEN(curToken, DEC, "--");
    TEST_TOKEN(curToken, ID, "a");
    TEST_TOKEN(curToken, SEMI, ";");

    TEST_TOKEN(curToken, ID, "a");
    TEST_TOKEN(curToken, INC, "++");
    TEST_TOKEN(curToken, SEMI, ";");

    TEST_TOKEN(curToken, ID, "a");
    TEST_TOKEN(curToken, DEC, "--");
    TEST_TOKEN(curToken, SEMI, ";");

    TEST_TOKEN(curToken, INC, "++");
    TEST_TOKEN(curToken, INC, "++");
    TEST_TOKEN(curToken, ID, "a");
    TEST_TOKEN(curToken, SEMI, ";");

    TEST_TOKEN(curToken, INC, "++");
    TEST_TOKEN(curToken, DEC, "--");
    TEST_TOKEN(curToken, ID, "a");
    TEST_TOKEN(curToken, SEMI, ";");

    TEST_TOKEN(curToken, DEC, "--");
    TEST_TOKEN(curToken, INC, "++");
    TEST_TOKEN(curToken, ID, "a");
    TEST_TOKEN(curToken, SEMI, ";");

    TEST_TOKEN(curToken, DEC, "--");
    TEST_TOKEN(curToken, DEC, "--");
    TEST_TOKEN(curToken, ID, "a");
    TEST_TOKEN(curToken, SEMI, ";");

    TEST_TOKEN(curToken, ID, "a");
    TEST_TOKEN(curToken, INC, "++");
    TEST_TOKEN(curToken, INC, "++");
    TEST_TOKEN(curToken, SEMI, ";");

    TEST_TOKEN(curToken, ID, "a");
    TEST_TOKEN(curToken, INC, "++");
    TEST_TOKEN(curToken, DEC, "--");
    TEST_TOKEN(curToken, SEMI, ";");

    TEST_TOKEN(curToken, ID, "a");
    TEST_TOKEN(curToken, DEC, "--");
    TEST_TOKEN(curToken, INC, "++");
    TEST_TOKEN(curToken, SEMI, ";");

    TEST_TOKEN(curToken, ID, "a");
    TEST_TOKEN(curToken, DEC, "--");
    TEST_TOKEN(curToken, DEC, "--");
    TEST_TOKEN(curToken, SEMI, ";");

    return 0;
}

int test_token_unary(void)
{
    String tokenString = str_internalize_cstring("-a; !a; ~a;");
    Token *tokens = tokenize_string(tokenString);

    Token *curToken = tokens;

    TEST_TOKEN(curToken, SUB, "-");
    TEST_TOKEN(curToken, ID, "a");
    TEST_TOKEN(curToken, SEMI, ";");

    TEST_TOKEN(curToken, NOT, "!");
    TEST_TOKEN(curToken, ID, "a");
    TEST_TOKEN(curToken, SEMI, ";");

    TEST_TOKEN(curToken, INV, "~");
    TEST_TOKEN(curToken, ID, "a");
    TEST_TOKEN(curToken, SEMI, ";");

    return 0;
}

int test_token_base(void)
{
    String tokenString = str_internalize_cstring("ab = _ask92; _02 + 0239; / * << >> >>> ^ & | \n\tce = 0;");
    Token *tokens = tokenize_string(tokenString);

    Token *curToken = tokens;
    TEST_TOKEN(curToken, ID, "ab");
    TEST_TOKEN(curToken, ASSIGN, "=");
    TEST_TOKEN(curToken, ID, "_ask92");
    TEST_TOKEN(curToken, SEMI, ";");
    TEST_TOKEN(curToken, ID, "_02");
    TEST_TOKEN(curToken, ADD, "+");
    TEST_TOKEN(curToken, NUMBER, "0239");
    TEST_TOKEN(curToken, SEMI, ";");
    TEST_TOKEN(curToken, DIV, "/");
    TEST_TOKEN(curToken, MUL, "*");
    TEST_TOKEN(curToken, SLL, "<<");
    TEST_TOKEN(curToken, SRA, ">>");
    TEST_TOKEN(curToken, SRL, ">>>");
    TEST_TOKEN(curToken, XOR, "^");
    TEST_TOKEN(curToken, AND, "&");
    TEST_TOKEN(curToken, OR, "|");
    TEST_TOKEN(curToken, EOL, "\n");
    TEST_TOKEN(curToken, ID, "ce");
    TEST_TOKEN(curToken, ASSIGN, "=");
    TEST_TOKEN(curToken, NUMBER, "0");
    TEST_TOKEN(curToken, SEMI, ";");
    i_expect(!curToken);

    return 0;
}

int test_tokenizer(void)
{
    int errors = 0;
    errors |= test_token_open_close();
    errors |= test_token_incdec();
    errors |= test_token_unary();
    errors |= test_token_base();
    return errors;
}

int main(int argc, char **argv)
{
    int errors = test_tokenizer();

    return errors;
}
