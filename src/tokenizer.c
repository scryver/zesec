#include "./common.h"


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
internal Token *
tokenize(char *filename)
{
    // NOTE(michiel): Example of allocating token memory in chunks until we run out of chunks
    s32 memRemaining = (s32)MAX_TOKEN_MEM_CHUNK;
    unused(memRemaining);
    Token *result = allocate_array(MAX_TOKEN_MEM_CHUNK, Token, 0);

    Buffer fileBuffer = read_entire_file(filename);

    char *scanner = (char *)fileBuffer.data;
    while (*scanner)
    {
        ++scanner;
    }
    deallocate(fileBuffer.data);

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
        if (it->value.size)
        {
            fprintf(stdout, ", value=%.*s>\n", it->value.size, it->value.data);
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
