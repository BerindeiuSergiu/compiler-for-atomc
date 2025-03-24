#include <stdio.h>
#include <stdlib.h>

#include "lexer/lexer.h"
#include "utils/utils.h"
#include "parser/parser.h"

int main(int argc, char **argv)
{

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <argument>\n", argv[0]);
        return EXIT_FAILURE;
    }

    /*
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <argument>\n", argv[0]);
        return EXIT_FAILURE;
    }
    */

    Token *tokens = NULL;
    char *bufferCool = NULL;

    bufferCool = loadFile(argv[1]);
    tokens = tokenize(bufferCool);

    //showTokens(tokens);

    parse(tokens);
    
    free(bufferCool);
    // TO DO make funciton to free up the list //
    return 0;
}