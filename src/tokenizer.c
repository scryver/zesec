internal Token *
tokenize(char *filename)
{
    // NOTE(michiel): Example of allocating token memory in chunks until we run out of chunks
    // TODO(michiel): Run out of chunks
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
                prevToken->nextToken = token;
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
    for (Token *it = tokens; it; it = it->nextToken)
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
