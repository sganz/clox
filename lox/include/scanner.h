#ifndef clox_scanner_h
#define clox_scanner_h

typedef enum {
    // Single-character tokens.
    
    TOKEN_LEFT_PAREN, 
    TOKEN_RIGHT_PAREN,
    TOKEN_LEFT_BRACE, 
    TOKEN_RIGHT_BRACE,
    
    TOKEN_LEFT_BRACKET,     // sjg - NQQ tokens []
    TOKEN_RIGHT_BRACKET,    // sjg - NQQ tokens []
    TOKEN_COMMA, 
    TOKEN_DOT, 
    TOKEN_MINUS, 
    TOKEN_PLUS,
    TOKEN_SEMICOLON, 
    TOKEN_COLON,            // sjg - NQQ token
    TOKEN_PERCENT,          // sjg - NQQ token
    TOKEN_SLASH, 
    
    // One or two or three character tokens.
    TOKEN_STAR,
    TOKEN_STAR_STAR,     // NQQ power
  
    TOKEN_BANG, 
    TOKEN_BANG_EQUAL,
    TOKEN_EQUAL, 
    TOKEN_EQUAL_EQUAL,
    TOKEN_GREATER, 
    TOKEN_GREATER_EQUAL,
    TOKEN_LESS, TOKEN_LESS_EQUAL,
    TOKEN_PLUS_EQUAL,    // NQQ +=
    TOKEN_MINUS_EQUAL,   // NQQ -=
    TOKEN_STAR_EQUAL,    // NQQ *=
    TOKEN_SLASH_EQUAL,   // NQQ /=
    TOKEN_PERCENT_EQUAL, // NQQ %= (mod)
    TOKEN_STAR_STAR_EQUAL, // NQQ **= (power) sjg - migh remove, seems odd
    
    // Literals.
    TOKEN_BASIC_STRING,     // NQQ - not sure what this is
    TOKEN_TEMPLATE_STRING,  // NQQ - not sure what this is
    TOKEN_STRING,           // orig to CLOX, might be mapped to a NQQ one
    TOKEN_RAW_STRING,       // NQQ - not sure what this is, string literal???
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    
    // Keywords.
    TOKEN_AND,
    TOKEN_BREAK,    // NQQ
    TOKEN_CONTINUE, // NQQ
    TOKEN_CLASS, 
    TOKEN_ELSE, 
    TOKEN_FALSE,
    TOKEN_FOR, 
    TOKEN_FUN, 
    TOKEN_IF, 
    TOKEN_LET,  // NQQ, I think it's a var with assign??
    TOKEN_NIL, 
    TOKEN_OR,
    TOKEN_PRINT,    // sjg - NQQ removed, but keep for now
    TOKEN_RETURN, 
    TOKEN_SUPER, 
    TOKEN_THIS,
    TOKEN_TRUE, 
    TOKEN_VAR,      // Not sure what this is, possible from a let statement??
    TOKEN_WHILE,

    TOKEN_ERROR, 
    TOKEN_EOF
} TokenType;

typedef struct {
    TokenType type;
    const char* start;
    int length;
    int line;
} Token;

void initScanner(const char* source);
Token scanToken();

#endif