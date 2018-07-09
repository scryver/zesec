internal void
advance(TokenEater *eater)
{
    ++eater->scanner;
    ++eater->columnNumber;
}

internal Token *
tokenize(char *filename)
{
    // NOTE(michiel): Example of allocating token memory in chunks until we run out of chunks
    // TODO(michiel): Run out of chunks
    Token *result = allocate_array(MAX_TOKEN_MEM_CHUNK, Token, 0);
    Token *prevToken = NULL;
    u32 tokenIndex = 0;

    Buffer fileBuffer = read_entire_file(filename);

    TokenEater eater = {1, 1, (char *)fileBuffer.data};
    while (*eater.scanner)
    {
        Token *token = NULL;
        if ((eater.scanner[0] == ' ') ||
            (eater.scanner[0] == '\r') ||
            (eater.scanner[0] == '\t'))
        {
            // NOTE(michiel): Skip these
            advance(&eater);
        }
        else if (eater.scanner[0] == '\n')
        {
            token = next_token(result, tokenIndex++);
            token->kind = TOKEN_EOL;
            token->value.size = 1;
            token->value.data = (u8 *)eater.scanner;
            token->colNumber = eater.columnNumber;
            advance(&eater);
        }
        else if (eater.scanner[0] == ';')
        {
            token = next_token(result, tokenIndex++);
            token->kind = TOKEN_SEMI;
            token->value.size = 1;
            token->value.data = (u8 *)eater.scanner;
            token->colNumber = eater.columnNumber;
            advance(&eater);
        }
        else if (eater.scanner[0] == '=')
        {
            token = next_token(result, tokenIndex++);
            token->kind = TOKEN_ASSIGN;
            token->value.size = 1;
            token->value.data = (u8 *)eater.scanner;
            token->colNumber = eater.columnNumber;
            advance(&eater);
        }
        else if (eater.scanner[0] == '*')
        {
            token = next_token(result, tokenIndex++);
            token->kind = TOKEN_MUL;
            token->value.size = 1;
            token->value.data = (u8 *)eater.scanner;
            token->colNumber = eater.columnNumber;
            advance(&eater);
        }
        else if (eater.scanner[0] == '/')
        {
            token = next_token(result, tokenIndex++);
            token->kind = TOKEN_DIV;
            token->value.size = 1;
            token->value.data = (u8 *)eater.scanner;
            token->colNumber = eater.columnNumber;
            advance(&eater);
        }
        else if (eater.scanner[0] == '&')
        {
            token = next_token(result, tokenIndex++);
            token->kind = TOKEN_AND;
            token->value.size = 1;
            token->value.data = (u8 *)eater.scanner;
            token->colNumber = eater.columnNumber;
            advance(&eater);
        }
        else if (eater.scanner[0] == '<')
        {
            if (eater.scanner[1] && (eater.scanner[1] == '<'))
            {
                // NOTE(michiel): >> == Logical right shift
                token = next_token(result, tokenIndex++);
                token->value.size = 2;
                token->value.data = (u8 *)eater.scanner;
                token->kind = TOKEN_SLL;
                token->colNumber = eater.columnNumber;
                advance(&eater);
                advance(&eater);
            }
            else
            {
                fprintf(stderr, "Incomplete shift left operator\n");
                advance(&eater);
            }
        }
        else if (eater.scanner[0] == '>')
        {
            if (eater.scanner[1] && (eater.scanner[1] == '>'))
            {
                // NOTE(michiel): >> == Logical right shift
                token = next_token(result, tokenIndex++);
                token->value.size = 2;
                token->value.data = (u8 *)eater.scanner;
                token->kind = TOKEN_SRA;
                token->colNumber = eater.columnNumber;
                if (eater.scanner[2] && (eater.scanner[2] == '>'))
                {
                    // NOTE(michiel): >>> == Logical right shift
                    ++token->value.size;
                    token->kind = TOKEN_SRL;
                    advance(&eater);
                }
                advance(&eater);
                advance(&eater);
            }
            else
            {
                fprintf(stderr, "Incomplete shift right operator\n");
                advance(&eater);
            }
        }
        else if (eater.scanner[0] == '-')
        {
            token = next_token(result, tokenIndex++);
            token->kind = TOKEN_SUB;
            token->value.size = 1;
            token->value.data = (u8 *)eater.scanner;
            token->colNumber = eater.columnNumber;
            advance(&eater);
        }
        else if (eater.scanner[0] == '+')
        {
            token = next_token(result, tokenIndex++);
            token->kind = TOKEN_ADD;
            token->value.size = 1;
            token->value.data = (u8 *)eater.scanner;
            token->colNumber = eater.columnNumber;
            advance(&eater);
        }
        else if (eater.scanner[0] == '|')
        {
            token = next_token(result, tokenIndex++);
            token->kind = TOKEN_OR;
            token->value.size = 1;
            token->value.data = (u8 *)eater.scanner;
            token->colNumber = eater.columnNumber;
            advance(&eater);
        }
        else if (eater.scanner[0] == '^')
        {
            token = next_token(result, tokenIndex++);
            token->kind = TOKEN_XOR;
            token->value.size = 1;
            token->value.data = (u8 *)eater.scanner;
            token->colNumber = eater.columnNumber;
            advance(&eater);
        }
        else if (('0' <= eater.scanner[0]) && (eater.scanner[0] <= '9'))
        {
            token = next_token(result, tokenIndex++);
            token->kind = TOKEN_NUMBER;
            token->value.size = 1;
            token->value.data = (u8 *)eater.scanner;
            token->colNumber = eater.columnNumber;
            advance(&eater);
            while (('0' <= eater.scanner[0]) && (eater.scanner[0] <= '9'))
            {
                ++token->value.size;
                advance(&eater);
            }
        }
        else if ((eater.scanner[0] == '_') ||
                 (('A' <= eater.scanner[0]) && (eater.scanner[0] <= 'Z')) ||
                 (('a' <= eater.scanner[0]) && (eater.scanner[0] <= 'z')))
        {
            token = next_token(result, tokenIndex++);
            token->kind = TOKEN_ID;
            token->value.size = 1;
            token->value.data = (u8 *)eater.scanner;
            token->colNumber = eater.columnNumber;
            advance(&eater);
            while ((eater.scanner[0] == '_') ||
                   (('A' <= eater.scanner[0]) && (eater.scanner[0] <= 'Z')) ||
                   (('a' <= eater.scanner[0]) && (eater.scanner[0] <= 'z')) ||
                   (('0' <= eater.scanner[0]) && (eater.scanner[0] <= '9')))
            {
                ++token->value.size;
                advance(&eater);
            }
        }
        else
        {
            fprintf(stderr, "Unhandled scan item: %c\n", eater.scanner[0]);
            advance(&eater);
        }

        if (token)
        {
            token->lineNumber = eater.lineNumber;
            token->filename = (String){.size=string_length(filename), .data=(u8 *)filename};
            if (token->kind == TOKEN_EOL)
            {
                ++eater.lineNumber;
                eater.columnNumber = 1;
            }

            if (prevToken)
            {
                prevToken->nextToken = token;
            }
            prevToken = token;
        }
    }

    return result;
}

#define CASE(name) case TOKEN_##name: { fprintf(fileStream.file, #name); } break
internal void
print_token_kind(FileStream fileStream, Token *token)
{
    switch (token->kind)
    {
        CASE(NULL);
        CASE(NUMBER);
        CASE(ID);
        CASE(MUL);
        CASE(DIV);
        CASE(AND);
        CASE(SLL);
        CASE(SRL);
        CASE(SRA);
        CASE(SUB);
        CASE(ADD);
        CASE(OR);
        CASE(XOR);
        CASE(ASSIGN);
        CASE(SEMI);
        CASE(EOL);

        default: {
            fprintf(stdout, "unknown");
        } break;
    }
}
#undef CASE

internal void
print_token(FileStream fileStream, Token *token)
{
    fprintf(fileStream.file, "%.*s:%d:%d < ", token->filename.size, token->filename.data,
            token->lineNumber, token->colNumber);
    print_token_kind(fileStream, token);
    if (token->kind == TOKEN_EOL)
    {
        fprintf(fileStream.file, ",\\n");
    }
    else if (token->value.size)
    {
        fprintf(fileStream.file, ",%.*s", token->value.size, token->value.data);
    }
    fprintf(fileStream.file, " >");
}

internal void
print_tokens(Token *tokens)
{
    for (Token *it = tokens; it; it = it->nextToken)
    {
        print_token((FileStream){.file=stdout}, it);
        fprintf(stdout, "\n");
    }
}
