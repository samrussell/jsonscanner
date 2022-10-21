#include <stdio.h>
#include <stdlib.h>
#include "token.h"

void parse_value(token_node** pTokens);

int is_hex_char(int test_char){
    switch (test_char) {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
            return 1;
    }
    return 0;
}

int process_escape_char(FILE* pFile){
    int next_char = fgetc(pFile);
    switch (next_char) {
        case '"':
        case '\\':
        case '/':
        case 'b':
        case 'f':
        case 'n':
        case 'r':
        case 't':
            return next_char;
        case 'u':
            for(int i=0; i<4; i++){
                next_char = fgetc(pFile);
                if (!is_hex_char(next_char)){
                    printf("Bad hex char following u: %c", next_char);
                    return 0;
                }
            }
            return 'u';
    }

    // error, return zero
    return 0;
}

token_t process_string(FILE *pFile){
    // we have already taken off the first "
    while(1){
        int next_char = fgetc(pFile);
        if (next_char == EOF){
            printf("EOF while reading string\n");
            return TOKEN_ERROR;
        }
        if (next_char == '"'){
            // end of string
            return TOKEN_STRING;
        }
        if (next_char == '\\'){
            if(!process_escape_char(pFile)){
                // returns zero if error
                return TOKEN_ERROR;
            }
        }
    }
}

token_t process_whitespace(FILE* pFile){
    // we already ate one whitespace char, keep eating until we hit something else
    // then put it back on
    while (1) {
        int next_char = fgetc(pFile);
        switch (next_char) {
            case ' ':
            case '\t':
            case '\n':
            case '\r':
                continue;
        }
        ungetc(next_char, pFile);
        return TOKEN_WHITESPACE;
    }
}

token_t process_boolean(FILE* pFile, int first_char){
    // this feels yuck but it works
    if (first_char == 't'){
        if (fgetc(pFile) != 'r')
            return TOKEN_ERROR;
        if (fgetc(pFile) != 'u')
            return TOKEN_ERROR;
        if (fgetc(pFile) != 'e')
            return TOKEN_ERROR;
        return TOKEN_TRUE;
    }
    if (first_char == 'f'){
        if (fgetc(pFile) != 'a')
            return TOKEN_ERROR;
        if (fgetc(pFile) != 'l')
            return TOKEN_ERROR;
        if (fgetc(pFile) != 's')
            return TOKEN_ERROR;
        if (fgetc(pFile) != 'e')
            return TOKEN_ERROR;
        return TOKEN_FALSE;
    }

    return TOKEN_ERROR;
}

token_t process_number(FILE* pFile) {
    while (1) {
        int next_char = fgetc(pFile);
        switch (next_char) {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            case '.':
            case 'e':
            case 'E':
            case '+':
            case '-':
                continue;
        }
        ungetc(next_char, pFile);
        break;
    }

    return TOKEN_NUMBER;
}

token_t scan_token(FILE *pFile){
    int next_char = fgetc(pFile);
    switch (next_char){
        case EOF:
            return TOKEN_EOF;
        case '{':
            return TOKEN_LEFT_BRACE;
        case '}':
            return TOKEN_RIGHT_BRACE;
        case '[':
            return TOKEN_LEFT_BRACKET;
        case ']':
            return TOKEN_RIGHT_BRACKET;
        case ':':
            return TOKEN_COLON;
        case ',':
            return TOKEN_COMMA;
        case ' ':
        case '\t':
        case '\r':
        case '\n':
            return process_whitespace(pFile);
        case 'f':
        case 't':
            return process_boolean(pFile, next_char);
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '-':
            return process_number(pFile);
        case '"':
            return process_string(pFile);
    }

    printf("Error, char not understood: %c\n", next_char);
    for(int i=0; i<10; i++){
        printf("%c", fgetc(pFile));
    }
    printf("\n");
    return TOKEN_ERROR;
}

token_node* scan_file(FILE *pFile){
    token_node* first_token = NULL;
    token_node* current_token = NULL;

    while(1){
        token_t token = scan_token(pFile);
        if (token == TOKEN_EOF){
            printf("End of file\n");
            break;
        }
        if (token == TOKEN_ERROR){
            printf("Error, quitting\n");
            break;
        }
        //printf("Token: %s\n", token_names[token]);

        token_node* next_token = calloc(sizeof(token_node), 1);
        next_token->value = token;

        if (current_token) {
            current_token->next = next_token;
        }
        else{
            first_token = next_token;
        }
        
        current_token = next_token;
    }

    return first_token;
}

void advance_token(token_node** pTokens){
    token_node* old_tokens;
    old_tokens = *pTokens;
    *pTokens = (*pTokens)->next;
    free(old_tokens);
}

void handle_whitespace(token_node** pTokens){
    // ignore whitespace because our tokeniser doesn't always pick it up
    if ((*pTokens)->value == TOKEN_WHITESPACE){
        advance_token(pTokens);
    }
}

void expect_token(token_node** pTokens, token_t expected_token){
    handle_whitespace(pTokens);
    token_t token = (*pTokens)->value;
    if (token != expected_token){
        printf("Error parsing object, expected %s but got %s\n", token_names[expected_token], token_names[token]);
        *pTokens = NULL;
        return;
    }

    advance_token(pTokens);
}

void parse_object(token_node** pTokens){
    printf("Parsing object\n");
    expect_token(pTokens, TOKEN_LEFT_BRACE);

    if ((*pTokens)->value == TOKEN_RIGHT_BRACE){
        advance_token(pTokens);
        return;
    }
    

    while(1) {
        expect_token(pTokens, TOKEN_STRING);
        expect_token(pTokens, TOKEN_COLON);
        parse_value(pTokens);
        handle_whitespace(pTokens);
        if ((*pTokens)->value != TOKEN_COMMA)
            break;
        
        advance_token(pTokens);
    }

    expect_token(pTokens, TOKEN_RIGHT_BRACE);
}

void parse_array(token_node** pTokens){
    printf("Parsing array\n");
    expect_token(pTokens, TOKEN_LEFT_BRACKET);

    if ((*pTokens)->value == TOKEN_RIGHT_BRACKET){
        advance_token(pTokens);
        return;
    }

    while(1) {
        parse_value(pTokens);
        handle_whitespace(pTokens);
        if ((*pTokens)->value != TOKEN_COMMA)
            break;
        
        advance_token(pTokens);
    }

    expect_token(pTokens, TOKEN_RIGHT_BRACKET);
}

void parse_value(token_node** pTokens){
    printf("Parsing value\n");
    if (*pTokens) {
        handle_whitespace(pTokens);
        
        switch ((*pTokens)->value){
            case TOKEN_LEFT_BRACE:
                parse_object(pTokens);
                break;
            case TOKEN_LEFT_BRACKET:
                parse_array(pTokens);
                break;
            case TOKEN_STRING:
            case TOKEN_NUMBER:
            case TOKEN_TRUE:
            case TOKEN_FALSE:
            case TOKEN_NULL:
                printf("Consuming %s\n", token_names[(*pTokens)->value]);
                advance_token(pTokens);
                break;
            default:
                printf("Unexpected token value: %s\n", token_names[(*pTokens)->value]);
                *pTokens = NULL;
                break;
        }
    }
}

int main(int argc, char** argv){
    if(argc != 2){
        printf("Usage: %s filename\n", argv[0]);
        return 1;
    }
    FILE* pFile = fopen(argv[1], "r");
    if(!pFile){
        printf("Could not open %s for reading\n", argv[1]);
        return 2;
    }
    printf("Loaded %s, scanning\n", argv[1]);

    token_node* tokens = scan_file(pFile);

    parse_value(&tokens);

    fclose(pFile);
    return 0;
}
