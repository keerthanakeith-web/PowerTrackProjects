#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "lexer.h"

static FILE* file_ptr = NULL;

static const char* keywords[MAX_KEYWORDS] = {
    "int", "float", "return", "if", "else", "while", "for", "do", "break", "continue",
    "char", "double", "void", "switch", "case", "default", "const", "static", "sizeof", "struct"
};

static const char* operators = "+-*/%=!<>&|^~"; 
static const char* specialCharacters = ",;{}()[]";

void initializeLexer(const char* filename) {
    file_ptr = fopen(filename, "r");
    if (!file_ptr) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
}

int isKeyword(const char* str) {
    for (int i = 0; i < MAX_KEYWORDS; i++) {
        if (keywords[i] != NULL && strcmp(str, keywords[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

int isOperator(const char* str) {
    if (strlen(str) == 1 && strchr(operators, str[0]) != NULL) {
        return 1;
    }
    if (strlen(str) == 2) {
        if (str[1] == '=') return 1;
        if (strcmp(str, "++") == 0 || strcmp(str, "--") == 0 || 
            strcmp(str, "&&") == 0 || strcmp(str, "||") == 0) {
            return 1;
        }
    }
    return 0;
}

int isSpecialCharacter(char ch) {
    return (strchr(specialCharacters, ch) != NULL);
}

int isConstant(const char* str) {

    if (((str[0] == '"' && str[strlen(str) - 1] == '"') || 
         (str[0] == '\'' && str[strlen(str) - 1] == '\'')) && strlen(str) >= 2) {
        return 1;
    }

    if (str == NULL || *str == '\0') return 0;
    if (str[0] == '"' && str[strlen(str) - 1] == '"' && strlen(str) >= 2) {
        return 1;
    }
    int has_dot = 0;
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == '.') {
            if (has_dot) return 0;
            has_dot = 1;
        } else if (!isdigit((unsigned char)str[i])) {
            return 0;
        }
    }
    return 1;
}

int isIdentifier(const char* str) {
    if (str == NULL || *str == '\0' || isdigit((unsigned char)str[0])) return 0;
    for (int i = 0; str[i] != '\0'; i++) {
        if (!isalnum((unsigned char)str[i]) && str[i] != '_') {
            return 0;
        }
    }
    return 1;
}

void categorizeToken(Token* token) {
    if (isKeyword(token->lexeme)) {
        token->type = KEYWORD;
    } else if (isOperator(token->lexeme)) {
        token->type = OPERATOR;
    } else if (strlen(token->lexeme) == 1 && isSpecialCharacter(token->lexeme[0])) {
        token->type = SPECIAL_CHARACTER;
    } else if (isConstant(token->lexeme)) {
        token->type = CONSTANT;
    } else if (isIdentifier(token->lexeme)) {
        token->type = IDENTIFIER;
    } else {
        token->type = UNKNOWN;
    }
}

Token getNextToken() {
    Token token;
    memset(token.lexeme, 0, MAX_TOKEN_SIZE);
    token.type = UNKNOWN;

    if (!file_ptr) {
        token.type = TOKEN_EOF;
        return token;
    }

    int ch;
    int len = 0;

    // Skip all whitespace
    while ((ch = fgetc(file_ptr)) != EOF) {
        if (!isspace(ch)) {
            break;
        }
    }

    if (ch == EOF) {
        fclose(file_ptr);
        file_ptr = NULL;
        token.type = TOKEN_EOF;
        return token;
    }

    // Handle preprocessor lines starting with '#'
    if (ch == '#') {
        token.lexeme[len++] = ch;
    
        while ((ch = fgetc(file_ptr)) != EOF && ch != '\n') {
            if (len < MAX_TOKEN_SIZE - 1) {
                token.lexeme[len++] = ch;
            }
        }
        token.lexeme[len] = '\0';
        token.type = PREPROCESSOR;
        return token;
    }

    // Handle string literals
    if (ch == '"') {
        token.lexeme[len++] = ch;
        while ((ch = fgetc(file_ptr)) != EOF) {
            if (len < MAX_TOKEN_SIZE - 1) {
                token.lexeme[len++] = ch;
            }
            if (ch == '"') break;
        }
        token.lexeme[len] = '\0';
        token.type = CONSTANT;
        return token;
    }

    // Handle single special character symbols
    if (isSpecialCharacter(ch)) {
        token.lexeme[0] = ch;
        token.lexeme[1] = '\0';
        token.type = SPECIAL_CHARACTER;
        return token;
    }

    // Handle single or multi-character operators
    if (strchr(operators, ch) != NULL) {
        token.lexeme[len++] = ch;
        int next_ch = fgetc(file_ptr);
        
        if (next_ch == '=') {
            token.lexeme[len++] = next_ch;
        } else if ((ch == '+' && next_ch == '+') ||
                   (ch == '-' && next_ch == '-') ||
                   (ch == '&' && next_ch == '&') ||
                   (ch == '|' && next_ch == '|')) {
            token.lexeme[len++] = next_ch;
        } else {
            if (next_ch != EOF) {
                ungetc(next_ch, file_ptr);
            }
        }
        token.lexeme[len] = '\0';
        token.type = OPERATOR;
        return token;
    }

    // Handle regular words (Identifiers, Keywords, Numbers)
    while (ch != EOF && !isspace(ch) && !isSpecialCharacter(ch) && ch != '#' && strchr(operators, ch) == NULL) {
        if (len < MAX_TOKEN_SIZE - 1) {
            token.lexeme[len++] = ch;
        }
        ch = fgetc(file_ptr);
    }

    if (ch != EOF) {
        ungetc(ch, file_ptr);
    }

    // Handle single character literals e.g., 'k', '\n'
    if (ch == '\'') {
        token.lexeme[len++] = ch;
        while ((ch = fgetc(file_ptr)) != EOF) {
            if (len < MAX_TOKEN_SIZE - 1) {
                token.lexeme[len++] = ch;
            }
            if (ch == '\'') break; // Stop when closing quote is found
        }
        token.lexeme[len] = '\0';
        token.type = CONSTANT;
        return token;
    }

    token.lexeme[len] = '\0';
    categorizeToken(&token);
    return token;
}