#include <stdio.h>
#include "token.h"

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

void scan_file(FILE *pFile){
    while(1){
        token_t token = scan_token(pFile);
        if (token == TOKEN_EOF){
            printf("End of file\n");
            return;
        }
        if (token == TOKEN_ERROR){
            printf("Error, quitting\n");
            return;
        }
        printf("Token: %s\n", token_names[token]);
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

    scan_file(pFile);

    fclose(pFile);
    return 0;
}
