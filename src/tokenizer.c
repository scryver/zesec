#include <stdint.h>
#include <stdio.h>

#define internal static
#define global   static
#define persist  static

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t   s8;
typedef int16_t  s16;
typedef int32_t  s32;
typedef int64_t  s64;

typedef int8_t   b8;
typedef int16_t  b16;
typedef int32_t  b32;
typedef int64_t  b64;

typedef float    f32;
typedef double   f64;

typedef struct Buffer
{
    u32 size;
    u8 *data;
} Buffer;

typedef Buffer String;

typedef enum TokenKind
{
    TOKEN_NULL,

    NUM_TOKENS, // NOTE(michiel): Always at the end of the enum to indicate how many tokens we have.
} TokenKind;
typedef struct Token
{
    TokenKind kind;
    String    value;

    struct Token *next_token;
} Token;

internal Token *
tokenize(char *filename)
{
    Token *result = NULL;

    return result;
}

internal void
print_token_kind(Token *token)
{
    switch (token->kind)
    {
        case TOKEN_NULL: {
            fprintf(stdout, "null");
        } break;

        default: {
            fprintf(stdout, "unknown");
        } break;
    }
}

internal void
print_tokens(Token *tokens)
{
    for (Token *it = tokens; it; it = it->next_token)
    {
        fprintf(stdout, "<Token kind=");
        print_token_kind(it);
        fprintf(stdout, ", value=%.*s>\n", it->value.size, it->value.data);
    }
}

int main(int argc, char **argv)
{
    int errors = 0;

    if (argc == 2)
    {
        fprintf(stdout, "Tokenizing file: %s\n", argv[1]);

        Token *tokens = tokenize(argv[1]);
        print_tokens(tokens);
    }
    else
    {
        fprintf(stdout, "Usage: %s <input-file>\n", argv[0]);
        errors = 1;
    }

    return errors;
}
