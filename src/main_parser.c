#include "./common.h"
#include "./tokenizer.h"
#include "./parser.h"

#include "./common.c"
#include "./tokenizer.c"
#include "./parser.c"

int main(int argc, char **argv)
{
    int errors = 0;

    if (argc == 2)
    {
        fprintf(stdout, "Parsing file: %s\n", argv[1]);

        Token *tokens = tokenize_file(argv[1]);
        Program *program = parse(tokens);

        print_parsed_program(program, false);
    }
    else
    {
        fprintf(stdout, "Usage: %s <input-file>\n", argv[0]);
        errors = 0;
    }

    return errors;
}
