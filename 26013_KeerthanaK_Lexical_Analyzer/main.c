/**************************************************************************************************
 * NAME: KEERTHANA K
 * PROJECT: LEXICAL_ANALYSER
 * DESCRIPTION: This project reads a C code file character by character.
 *                It breaks the code down into smaller parts and labels them 
 *                as keywords, identifiers, numbers, operators, or headers.
 * ************************************************************************************************/#include <stdio.h>
#include "lexer.h"

const char* tokenTypeNames[] = {
    "Keyword     ",
    "Operator    ",
    "Operator    ",   // Maps SPECIAL_CHARACTER to "Operator"
    "Literal     ",   // Maps CONSTANT to "Literal"
    "Identifier  ",
    "Preprocessor",
    "Unknown     "
};

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: ./lexical_analyser <.c file>\n");
        return 1;
    }

    printf("Open     : %s : Success\n", argv[1]);
    printf("Parsing  : %s : Started\n\n", argv[1]);

    initializeLexer(argv[1]);

    Token token;
    while ((token = getNextToken()).type != TOKEN_EOF) {
        if (token.type == UNKNOWN) {
            printf("%s : %s\n", tokenTypeNames[token.type], token.lexeme);
            continue;
        }
        printf("%s : %s\n", tokenTypeNames[token.type], token.lexeme);
    }    
    printf("\nParsing  : %s : Done\n", argv[1]);

    return 0;
}