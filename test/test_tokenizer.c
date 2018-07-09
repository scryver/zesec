#include "../src/common.h"
#include "../src/tokenizer.h"

#include "../src/common.c"
#include "../src/tokenizer.c"

int test_tokenizer(void)
{
    String tokenString = create_string("ab = _ask92; _02 + 0239; / * << >> >>> ^ & | \n\tce = 0;");
    Token *tokens = tokenize_string(tokenString);

    Token *curToken = tokens;
    i_expect(curToken->kind == TOKEN_ID);
    i_expect(strings_are_equal(curToken->value, create_string("ab")));

    curToken = curToken->nextToken;
    i_expect(curToken);
    i_expect(curToken->kind == TOKEN_ASSIGN);
    i_expect(strings_are_equal(curToken->value, create_string("=")));

    curToken = curToken->nextToken;
    i_expect(curToken);
    i_expect(curToken->kind == TOKEN_ID);
    i_expect(strings_are_equal(curToken->value, create_string("_ask92")));

    curToken = curToken->nextToken;
    i_expect(curToken);
    i_expect(curToken->kind == TOKEN_SEMI);
    i_expect(strings_are_equal(curToken->value, create_string(";")));

    curToken = curToken->nextToken;
    i_expect(curToken);
    i_expect(curToken->kind == TOKEN_ID);
    i_expect(strings_are_equal(curToken->value, create_string("_02")));

    curToken = curToken->nextToken;
    i_expect(curToken);
    i_expect(curToken->kind == TOKEN_ADD);
    i_expect(strings_are_equal(curToken->value, create_string("+")));

    curToken = curToken->nextToken;
    i_expect(curToken);
    i_expect(curToken->kind == TOKEN_NUMBER);
    i_expect(strings_are_equal(curToken->value, create_string("0239")));

    curToken = curToken->nextToken;
    i_expect(curToken);
    i_expect(curToken->kind == TOKEN_SEMI);
    i_expect(strings_are_equal(curToken->value, create_string(";")));

    curToken = curToken->nextToken;
    i_expect(curToken);
    i_expect(curToken->kind == TOKEN_DIV);
    i_expect(strings_are_equal(curToken->value, create_string("/")));

    curToken = curToken->nextToken;
    i_expect(curToken);
    i_expect(curToken->kind == TOKEN_MUL);
    i_expect(strings_are_equal(curToken->value, create_string("*")));

    curToken = curToken->nextToken;
    i_expect(curToken);
    i_expect(curToken->kind == TOKEN_SLL);
    i_expect(strings_are_equal(curToken->value, create_string("<<")));

    curToken = curToken->nextToken;
    i_expect(curToken);
    i_expect(curToken->kind == TOKEN_SRA);
    i_expect(strings_are_equal(curToken->value, create_string(">>")));

    curToken = curToken->nextToken;
    i_expect(curToken);
    i_expect(curToken->kind == TOKEN_SRL);
    i_expect(strings_are_equal(curToken->value, create_string(">>>")));

    curToken = curToken->nextToken;
    i_expect(curToken);
    i_expect(curToken->kind == TOKEN_XOR);
    i_expect(strings_are_equal(curToken->value, create_string("^")));

    curToken = curToken->nextToken;
    i_expect(curToken);
    i_expect(curToken->kind == TOKEN_AND);
    i_expect(strings_are_equal(curToken->value, create_string("&")));

    curToken = curToken->nextToken;
    i_expect(curToken);
    i_expect(curToken->kind == TOKEN_OR);
    i_expect(strings_are_equal(curToken->value, create_string("|")));

    curToken = curToken->nextToken;
    i_expect(curToken);
    i_expect(curToken->kind == TOKEN_EOL);
    i_expect(strings_are_equal(curToken->value, create_string("\n")));

    curToken = curToken->nextToken;
    i_expect(curToken);
    i_expect(curToken->kind == TOKEN_ID);
    i_expect(strings_are_equal(curToken->value, create_string("ce")));

    curToken = curToken->nextToken;
    i_expect(curToken);
    i_expect(curToken->kind == TOKEN_ASSIGN);
    i_expect(strings_are_equal(curToken->value, create_string("=")));

    curToken = curToken->nextToken;
    i_expect(curToken);
    i_expect(curToken->kind == TOKEN_NUMBER);
    i_expect(strings_are_equal(curToken->value, create_string("0")));

    curToken = curToken->nextToken;
    i_expect(curToken);
    i_expect(curToken->kind == TOKEN_SEMI);
    i_expect(strings_are_equal(curToken->value, create_string(";")));

    curToken = curToken->nextToken;
    i_expect(!curToken);

    return 0;
}

int main(int argc, char **argv)
{
    int errors = test_tokenizer();

    return errors;
}
