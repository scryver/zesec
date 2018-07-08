#include "./common.h"
#include "./tokenizer.h"

#include "./common.c"
#include "./tokenizer.c"

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
