typedef enum {
    TOKEN_EOF = 0,
    TOKEN_LEFT_BRACE,
    TOKEN_RIGHT_BRACE,
    TOKEN_LEFT_BRACKET,
    TOKEN_RIGHT_BRACKET,
    TOKEN_WHITESPACE,
    TOKEN_COMMA,
    TOKEN_COLON,
    TOKEN_STRING,
    TOKEN_NUMBER,
    TOKEN_TRUE,
    TOKEN_FALSE,
    TOKEN_NULL,
    TOKEN_ERROR
} token_t;

char* token_names[] = {
    "TOKEN_EOF",
    "TOKEN_LEFT_BRACE",
    "TOKEN_RIGHT_BRACE",
    "TOKEN_LEFT_BRACKET",
    "TOKEN_RIGHT_BRACKET",
    "TOKEN_WHITESPACE",
    "TOKEN_COMMA",
    "TOKEN_COLON",
    "TOKEN_STRING",
    "TOKEN_NUMBER",
    "TOKEN_TRUE",
    "TOKEN_FALSE",
    "TOKEN_NULL",
    "TOKEN_ERROR"
};

typedef struct token_node_ {
    struct token_node_* next;
    token_t value;
} token_node;
