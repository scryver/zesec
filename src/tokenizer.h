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

    struct Token *nextToken;
} Token;

#define MAX_TOKEN_MEM_CHUNK 2048
internal inline Token *
next_token(Token *tokens, u32 index)
{
    i_expect(index < MAX_TOKEN_MEM_CHUNK);
    return tokens + index;
}
