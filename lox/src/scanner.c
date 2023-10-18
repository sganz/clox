#include <stdio.h>
#include <string.h>

#include "common.h"
#include "scanner.h"

typedef struct {
    const char* start;
    const char* current;
    int line;
} Scanner;

Scanner scanner;

void initScanner(const char* source) {
    scanner.start = source;
    scanner.current = source;
    scanner.line = 1;
}

static bool isAlpha(char c) {
    return (c >= 'a' && c <= 'z') ||
            (c >= 'A' && c <= 'Z') ||
            c == '_';
}

static bool isDigit(char c) {
    return c >= '0' && c <= '9';
}

static bool isAtEnd() {
    return *scanner.current == '\0';
}

static char advance() {
    scanner.current++;
    return scanner.current[-1];
}

static char peek() {
    return *scanner.current;
}

static char peekNext() {
    if (isAtEnd()) return '\0';
    return scanner.current[1];
}

static bool match(char expected) {
    if (isAtEnd()) return false;
    if (*scanner.current != expected) return false;

    scanner.current++;
    return true;
}

static Token makeToken(TokenType type) {
    Token token;
    token.type = type;
    token.start = scanner.start;
    token.length = (int)(scanner.current - scanner.start);
    token.line = scanner.line;

    return token;
}

static Token errorToken(const char* message) {
    Token token;
    token.type = TOKEN_ERROR;
    token.start = message;
    token.length = (int)strlen(message);
    token.line = scanner.line;

    return token;
}

static void skipWhitespace() {
    for(;;) {
        char c = peek();
        switch (c) {
        case ' ':
        case '\r':
        case '\t':
            advance();
            break;
        case '\n':
            scanner.line++;
            advance();
            break;
        case '/':
            if (peekNext() == '/') {
                // A comment goes until the end of the line.
                while (peek() != '\n' && !isAtEnd()) advance();
            } else if (peekNext() == '*') { // see if a C style comment
                // Multiline comment
                int depth = 0;
                for (;;) {
                    if (peek() == '*' && peekNext() == '/') {
                        depth--;
                    } else if (peek() == '/' && peekNext() == '*') {
                        depth++;
                    }
                    if (depth == 0) {
                        // Advance twice to consume closing '*/'
                        advance();
                        advance();
                        break;
                    }
                    if (!isAtEnd()) {
                        advance();
                    } else {
                        break;
                    }
                }
            } else {
                    return;
                }
            break;
        default:
            return;
        }
    }
}

static TokenType checkKeyword(int start, int length, const char* rest, TokenType type) {
    if (scanner.current - scanner.start == start + length && 
        memcmp(scanner.start + start, rest, length) == 0) {
        return type;
    }

    return TOKEN_IDENTIFIER;
}

static TokenType identifierType() {
    switch (scanner.start[0]) {
        case 'a': return checkKeyword(1, 2, "nd", TOKEN_AND);
        case 'b': return checkKeyword(1, 4, "reak", TOKEN_BREAK);
//        case 'c': return checkKeyword(1, 4, "lass", TOKEN_CLASS);
        case 'c':
            // sjg - first attempt at a mod, is it continue or class!
            if (scanner.current - scanner.start > 1) {
                switch (scanner.start[1]) {
                case 'l': return checkKeyword(2, 3, "ass", TOKEN_FALSE);
                case 'o': return checkKeyword(2, 6, "ntinue", TOKEN_FOR);
                }
            }

        case 'e': return checkKeyword(1, 3, "lse", TOKEN_ELSE);
        case 'f':
            if (scanner.current - scanner.start > 1) {
                switch (scanner.start[1]) {
                case 'a': return checkKeyword(2, 3, "lse", TOKEN_FALSE);
                case 'o': return checkKeyword(2, 1, "r", TOKEN_FOR);
                case 'u': return checkKeyword(2, 1, "n", TOKEN_FUN);
                }
            }
        break;
        case 'i': return checkKeyword(1, 1, "f", TOKEN_IF);
        case 'l': return checkKeyword(1, 2, "et", TOKEN_LET);
        case 'n': return checkKeyword(1, 2, "il", TOKEN_NIL);
        case 'o': return checkKeyword(1, 1, "r", TOKEN_OR);
        case 'p': return checkKeyword(1, 4, "rint", TOKEN_PRINT);
        case 'r': return checkKeyword(1, 5, "eturn", TOKEN_RETURN);
        case 's': return checkKeyword(1, 4, "uper", TOKEN_SUPER);
        case 't':
            if (scanner.current - scanner.start > 1) {
                switch (scanner.start[1]) {
                case 'h': return checkKeyword(2, 2, "is", TOKEN_THIS);
                case 'r': return checkKeyword(2, 2, "ue", TOKEN_TRUE);
                }
            }
            break;
        case 'v': return checkKeyword(1, 2, "ar", TOKEN_VAR);
        case 'w': return checkKeyword(1, 4, "hile", TOKEN_WHILE);
    }

    return TOKEN_IDENTIFIER;
}

static Token identifier() {
    while (isAlpha(peek()) || isDigit(peek())) advance();
    
    return makeToken(identifierType());
}

static Token number() {
    while (isDigit(peek())) advance();

    // Look for a fractional part
    if (peek() == '.' && isDigit(peekNext())) {
        // Consume the "."
        advance();

        while (isDigit(peek())) advance();
    }

    return makeToken(TOKEN_NUMBER);
}

// sjg - this might be the same as NQQ raw string??
// remove it and use the NQQ template
static Token string() {
    while (peek() != '"' && !isAtEnd()) {
        if(peek() == '\n') scanner.line++;
        advance();
    }

    if(isAtEnd()) return errorToken("Unterminated string.");

    // The closing quote
    advance();
    return makeToken(TOKEN_STRING);
}

static Token basicString() {
    while (peek() != '\'' && peek() != '\n' && !isAtEnd()) {
        if (peek() == '\n') {
            scanner.line++;
        } else if (peek() == '\\' && peekNext() == '\'') {
            advance();
        } else if (peek() == '\\' && peekNext() == '\n') {
            advance();
            scanner.line++;
        } else if (peek() == '\\' && peekNext() == '\\') {
            advance();
        }
        advance();
    }

    if (isAtEnd()) return errorToken("Unterminated basic string.");

    // The closing quote.
    advance();
    return makeToken(TOKEN_BASIC_STRING);
}

static Token templateString() {
    while (peek() != '"' && peek() != '\n' && !isAtEnd()) {
        if (peek() == '\n') {
            scanner.line++;
        } else if (peek() == '\\' && peekNext() == '\"') {
            advance();
        } else if (peek() == '\\' && peekNext() == '\n') {
            advance();
            scanner.line++;
        } else if (peek() == '\\' && peekNext() == '\\') {
            advance();
        }
        advance();
    }

    if (isAtEnd()) return errorToken("Unterminated template string.");

    // The closing quote.
    advance();
    return makeToken(TOKEN_TEMPLATE_STRING);
}

static Token rawString() {
    while (peek() != '`' && !isAtEnd()) {
        if (peek() == '\n') scanner.line++;
        advance();
    }

    if (isAtEnd()) return errorToken("Unterminated raw string.");

    // The closing quote.
    advance();
    return makeToken(TOKEN_RAW_STRING);
}

Token scanToken() {
    skipWhitespace();

    scanner.start = scanner.current;

    if (isAtEnd()) return makeToken(TOKEN_EOF);

    char c = advance();
    if (isAlpha(c)) return identifier();
    if (isDigit(c)) return number();

    switch (c) {
        case '(': return makeToken(TOKEN_LEFT_PAREN);
        case ')': return makeToken(TOKEN_RIGHT_PAREN);
        case '{': return makeToken(TOKEN_LEFT_BRACE);
        case '}': return makeToken(TOKEN_RIGHT_BRACE);
        case '[': return makeToken(TOKEN_LEFT_BRACKET);
        case ']': return makeToken(TOKEN_RIGHT_BRACKET);
        case ';': return makeToken(TOKEN_SEMICOLON);
        case ':': return makeToken(TOKEN_COLON);
        case ',': return makeToken(TOKEN_COMMA);
        case '.': return makeToken(TOKEN_DOT);
// sjg - removed simple cases to add NQQ assignment ops
//        case '-': return makeToken(TOKEN_MINUS);
//        case '+': return makeToken(TOKEN_PLUS);
//        case '/': return makeToken(TOKEN_SLASH);
//        case '*': return makeToken(TOKEN_STAR);
        case '-': // TODO: Add --
            if (match('=')) {
                return makeToken(TOKEN_MINUS_EQUAL);
            } else {
                return makeToken(TOKEN_MINUS);
            }
        case '+':   // TODO: Add ++
            if (match('=')) {
                return makeToken(TOKEN_PLUS_EQUAL);
            } else {
                return makeToken(TOKEN_PLUS);
            }
        case '/':
            if (match('=')) {
                return makeToken(TOKEN_SLASH_EQUAL);
            } else {
                return makeToken(TOKEN_SLASH);
            }
        case '%':
            if (match('=')) {
                return makeToken(TOKEN_PERCENT_EQUAL);
            } else {
                return makeToken(TOKEN_PERCENT);
            }
        case '*':
            if (match('*')) {
                if (match('=')) {
                    return makeToken(TOKEN_STAR_STAR_EQUAL);
                } else {
                    return makeToken(TOKEN_STAR_STAR);
                }
            } else if (match('=')) {
                return makeToken(TOKEN_STAR_EQUAL);
            } else {
                return makeToken(TOKEN_STAR);
            }
        case '!':
            return makeToken(match('=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
        case '=':
            return makeToken(match('=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
        case '<':
            return makeToken(match('=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
        case '>':
            return makeToken(match('=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);
//      CLOX Maps " to string() where this maps to templateString() in NQQ
//      Have to look what this means. Likely some places will get templateString()
//      and some will get rawString() when replaceing the clox string()...
// 
//      case '"': return string();

        case '\'': return basicString();
        case '"': return templateString();
        case '`': return rawString();
    }

    return errorToken("Unexpected character.");
}