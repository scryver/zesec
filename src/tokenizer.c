#include "./common.h"

typedef enum TokenKind
{
    TOKEN_NULL,
    TOKEN_NUMBER,
    TOKEN_ID,
    TOKEN_ADD,
    TOKEN_SUB,
    TOKEN_ASSIGN,
    TOKEN_SEMI,
    TOKEN_EOL,

    NUM_TOKENS, // NOTE(michiel): Always at the end of the enum to indicate how many tokens we have.
} TokenKind;
typedef struct Token
{
    TokenKind kind;
    String    value;

    struct Token *next_token;
} Token;

internal Buffer
read_entire_file(char *filename)
{
    Buffer result = {0};

    FILE *file = fopen(filename, "rb");
    // NOTE(michiel): Get the file size
    fseek(file, 0, SEEK_END);
    result.size = safe_truncate_to_u32(ftell(file));
    // NOTE(michiel): Reset the current file pointer to the beginning
    fseek(file, 0, SEEK_SET);
    // NOTE(michiel): Allocate memory for the file data
    result.data = allocate_array(result.size, u8, 0);
    // NOTE(michiel): Read the actual data from the file
    s64 bytesRead = fread(result.data, 1, result.size, file);
    i_expect(bytesRead == (s64)result.size);
    fclose(file);

    return result;
}

#define MAX_TOKEN_MEM_CHUNK 2048
internal inline Token *
next_token(Token *tokens, u32 index)
{
    i_expect(index < MAX_TOKEN_MEM_CHUNK);
    return tokens + index;
}

internal Token *
tokenize(char *filename)
{
    // NOTE(michiel): Example of allocating token memory in chunks until we run out of chunks
    s32 memRemaining = (s32)MAX_TOKEN_MEM_CHUNK;
    unused(memRemaining);
    Token *result = allocate_array(MAX_TOKEN_MEM_CHUNK, Token, 0);
    Token *prevToken = NULL;
    u32 tokenIndex = 0;

    Buffer fileBuffer = read_entire_file(filename);

    char *scanner = (char *)fileBuffer.data;
    while (*scanner)
    {
        Token *token = NULL;
        if ((scanner[0] == ' ') ||
            (scanner[0] == '\r') ||
            (scanner[0] == '\t'))
        {
            // NOTE(michiel): Skip these
            ++scanner;
        }
        else if (scanner[0] == '\n')
        {
            token = next_token(result, tokenIndex++);
            token->kind = TOKEN_EOL;
            token->value.size = 1;
            token->value.data = (u8 *)scanner++;
        }
        else if (scanner[0] == ';')
        {
            token = next_token(result, tokenIndex++);
            token->kind = TOKEN_SEMI;
            token->value.size = 1;
            token->value.data = (u8 *)scanner++;
        }
        else if (scanner[0] == '=')
        {
            token = next_token(result, tokenIndex++);
            token->kind = TOKEN_ASSIGN;
            token->value.size = 1;
            token->value.data = (u8 *)scanner++;
        }
        else if (scanner[0] == '+')
        {
            token = next_token(result, tokenIndex++);
            token->kind = TOKEN_ADD;
            token->value.size = 1;
            token->value.data = (u8 *)scanner++;
        }
        else if (scanner[0] == '-')
        {
            token = next_token(result, tokenIndex++);
            token->kind = TOKEN_SUB;
            token->value.size = 1;
            token->value.data = (u8 *)scanner++;
        }
        else if (('0' <= scanner[0]) && (scanner[0] <= '9'))
        {
            token = next_token(result, tokenIndex++);
            token->kind = TOKEN_NUMBER;
            token->value.size = 1;
            token->value.data = (u8 *)scanner++;
            while (('0' <= scanner[0]) && (scanner[0] <= '9'))
            {
                ++token->value.size;
                ++scanner;
            }
        }
        else if ((scanner[0] == '_') ||
                 (('A' <= scanner[0]) && (scanner[0] <= 'Z')) ||
                 (('a' <= scanner[0]) && (scanner[0] <= 'z')))
        {
            token = next_token(result, tokenIndex++);
            token->kind = TOKEN_ID;
            token->value.size = 1;
            token->value.data = (u8 *)scanner++;
            while ((scanner[0] == '_') ||
                   (('A' <= scanner[0]) && (scanner[0] <= 'Z')) ||
                   (('a' <= scanner[0]) && (scanner[0] <= 'z')) ||
                   (('0' <= scanner[0]) && (scanner[0] <= '9')))
            {
                ++token->value.size;
                ++scanner;
            }
        }
        else
        {
            fprintf(stderr, "Unhandled scan item: %c\n", scanner[0]);
            ++scanner;
        }

        if (token)
        {
            if (prevToken)
            {
                prevToken->next_token = token;
            }
            prevToken = token;
        }
    }

    return result;
}

internal void
print_token_kind(Token *token)
{
    switch (token->kind)
    {
        case TOKEN_NULL: {
            fprintf(stdout, "NULL");
        } break;

        case TOKEN_NUMBER: {
            fprintf(stdout, "NUMBER");
        } break;

        case TOKEN_ID: {
            fprintf(stdout, "ID");
        } break;

        case TOKEN_ADD: {
            fprintf(stdout, "ADD");
        } break;

        case TOKEN_SUB: {
            fprintf(stdout, "SUB");
        } break;

        case TOKEN_ASSIGN: {
            fprintf(stdout, "ASSIGN");
        } break;

        case TOKEN_SEMI: {
            fprintf(stdout, "SEMI");
        } break;

        case TOKEN_EOL: {
            fprintf(stdout, "EOL");
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
        fprintf(stdout, "<");
        print_token_kind(it);
        if (it->kind == TOKEN_EOL)
        {
            fprintf(stdout, ",\\n>\n");
        }
        else if (it->value.size)
        {
            fprintf(stdout, ",%.*s>\n", it->value.size, it->value.data);
        }
        else
        {
            fprintf(stdout, ">\n");
        }
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
