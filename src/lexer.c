#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
  // Single-character tokens
  TOKEN_LEFT_PAREN,
  TOKEN_RIGHT_PAREN,
  TOKEN_LEFT_BRACE,
  TOKEN_RIGHT_BRACE,
  TOKEN_COMMA,
  TOKEN_DOT,
  TOKEN_SEMICOLON,
  TOKEN_PLUS,
  TOKEN_MINUS,
  TOKEN_STAR,
  TOKEN_SLASH,
  TOKEN_PERCENT,
  TOKEN_BANG,

  // One or two character tokens
  TOKEN_BANG_EQUAL,
  TOKEN_EQUAL_EQUAL,
  TOKEN_GREATER,
  TOKEN_GREATER_EQUAL,
  TOKEN_LESS,
  TOKEN_LESS_EQUAL,
  TOKEN_AND_AND,
  TOKEN_OR_OR,
  TOKEN_EQUAL,

  // Literals
  TOKEN_IDENTIFIER,
  TOKEN_STRING,
  TOKEN_NUMBER,

  // Keywords
  TOKEN_LET,
  TOKEN_MUT,
  TOKEN_FN,
  TOKEN_RETURN,
  TOKEN_IF,
  TOKEN_ELSE,
  TOKEN_WHILE,
  TOKEN_FOR,
  TOKEN_TRUE,
  TOKEN_FALSE,
  TOKEN_PRINT,
  TOKEN_PRINTLN,
  TOKEN_LEN,
  TOKEN_INPUT,
  TOKEN_PARSE_INT,
  TOKEN_PARSE_FLOAT,
  TOKEN_TO_STRING,

  // Types
  TOKEN_INT,
  TOKEN_FLOAT,
  TOKEN_BOOL,
  TOKEN_STRING_TYPE,
  TOKEN_VOID,

  TOKEN_ERROR,
  TOKEN_EOF
} TokenType;

typedef struct {
  TokenType type;
  const char *start;
  int length;
  int line;
} Token;

typedef struct {
  const char *start;
  const char *current;
  int line;
} Lexer;

Lexer lexer;

void initLexer(const char *source) {
  lexer.start = source;
  lexer.current = source;
  lexer.line = 1;
}

static bool isAlpha(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

static bool isDigit(char c) { return c >= '0' && c <= '9'; }

static bool isAtEnd() { return *lexer.current == '\0'; }

static char advance() {
  lexer.current++;
  return lexer.current[-1];
}

static char peek() { return *lexer.current; }

static char peekNext() {
  if (isAtEnd())
    return '\0';
  return lexer.current[1];
}

static bool match(char expected) {
  if (isAtEnd())
    return false;
  if (*lexer.current != expected)
    return false;
  lexer.current++;
  return true;
}

static Token makeToken(TokenType type) {
  Token token;
  token.type = type;
  token.start = lexer.start;
  token.length = (int)(lexer.current - lexer.start);
  token.line = lexer.line;
  return token;
}

static Token errorToken(const char *message) {
  Token token;
  token.type = TOKEN_ERROR;
  token.start = message;
  token.length = (int)strlen(message);
  token.line = lexer.line;
  return token;
}

static void skipWhitespace() {
  for (;;) {
    char c = peek();
    switch (c) {
    case ' ':
    case '\r':
    case '\t':
      advance();
      break;
    case '\n':
      lexer.line++;
      advance();
      break;
    case '/':
      if (peekNext() == '/') {
        // A comment goes until the end of the line.
        while (peek() != '\n' && !isAtEnd())
          advance();
      } else {
        return;
      }
      break;
    default:
      return;
    }
  }
}

static TokenType checkKeyword(int start, int length, const char *rest,
                              TokenType type) {
  if (lexer.current - lexer.start == start + length &&
      memcmp(lexer.start + start, rest, length) == 0) {
    return type;
  }

  return TOKEN_IDENTIFIER;
}

static TokenType identifierType() {
  switch (lexer.start[0]) {
  case 'b':
    if (lexer.current - lexer.start > 1) {
      switch (lexer.start[1]) {
      case 'o':
        return checkKeyword(2, 2, "ol", TOKEN_BOOL);
      }
    }
    break;
  case 'e':
    if (lexer.current - lexer.start > 1) {
      switch (lexer.start[1]) {
      case 'l':
        return checkKeyword(2, 2, "se", TOKEN_ELSE);
      }
    }
    break;
  case 'f':
    if (lexer.current - lexer.start > 1) {
      switch (lexer.start[1]) {
      case 'a':
        return checkKeyword(2, 3, "lse", TOKEN_FALSE);
      case 'n':
        return checkKeyword(2, 0, "", TOKEN_FN);
      case 'l':
        return checkKeyword(2, 3, "oat", TOKEN_FLOAT);
      case 'o':
        return checkKeyword(2, 1, "r", TOKEN_FOR);
      }
    }
    break;
  case 'i':
    if (lexer.current - lexer.start > 1) {
      switch (lexer.start[1]) {
      case 'f':
        return checkKeyword(2, 0, "", TOKEN_IF);
      case 'n':
        if (lexer.current - lexer.start > 2) {
          switch (lexer.start[2]) {
          case 'p':
            return checkKeyword(3, 3, "ut", TOKEN_INPUT);
          case 't':
            return checkKeyword(3, 0, "", TOKEN_INT);
          }
        }
        break;
      }
    }
    break;
  case 'l':
    if (lexer.current - lexer.start > 1) {
      switch (lexer.start[1]) {
      case 'e':
        if (lexer.current - lexer.start > 2) {
          switch (lexer.start[2]) {
          case 'n':
            return checkKeyword(3, 0, "", TOKEN_LEN);
          case 't':
            return checkKeyword(3, 0, "", TOKEN_LET);
          }
        }
        break;
      }
    }
    break;
  case 'm':
    if (lexer.current - lexer.start > 1) {
      switch (lexer.start[1]) {
      case 'u':
        return checkKeyword(2, 1, "t", TOKEN_MUT);
      }
    }
    break;
  case 'p':
    if (lexer.current - lexer.start > 1) {
      switch (lexer.start[1]) {
      case 'a':
        if (lexer.current - lexer.start > 5) {
          if (memcmp(lexer.start + 2, "rse_", 4) == 0) {
            if (lexer.start[6] == 'i')
              return TOKEN_PARSE_INT;
            if (lexer.start[6] == 'f')
              return TOKEN_PARSE_FLOAT;
          }
        }
        break;
      case 'r':
        if (lexer.current - lexer.start > 5) {
          if (memcmp(lexer.start + 2, "int", 3) == 0) {
            if (lexer.current - lexer.start == 5)
              return TOKEN_PRINT;
            if (lexer.start[5] == 'l')
              return checkKeyword(6, 2, "n", TOKEN_PRINTLN);
          }
        }
        break;
      }
    }
    break;
  case 'r':
    if (lexer.current - lexer.start > 1) {
      switch (lexer.start[1]) {
      case 'e':
        return checkKeyword(2, 4, "turn", TOKEN_RETURN);
      }
    }
    break;
  case 's':
    if (lexer.current - lexer.start > 1) {
      switch (lexer.start[1]) {
      case 't':
        return checkKeyword(2, 4, "ring", TOKEN_STRING_TYPE);
      }
    }
    break;
  case 't':
    if (lexer.current - lexer.start > 1) {
      switch (lexer.start[1]) {
      case 'o':
        return checkKeyword(2, 7, "_string", TOKEN_TO_STRING);
      case 'r':
        return checkKeyword(2, 2, "ue", TOKEN_TRUE);
      }
    }
    break;
  case 'v':
    if (lexer.current - lexer.start > 1) {
      switch (lexer.start[1]) {
      case 'o':
        return checkKeyword(2, 2, "id", TOKEN_VOID);
      }
    }
    break;
  case 'w':
    if (lexer.current - lexer.start > 1) {
      switch (lexer.start[1]) {
      case 'h':
        return checkKeyword(2, 3, "ile", TOKEN_WHILE);
      }
    }
    break;
  }

  return TOKEN_IDENTIFIER;
}

static Token identifier() {
  while (isAlpha(peek()) || isDigit(peek()))
    advance();
  return makeToken(identifierType());
}

static Token number() {
  while (isDigit(peek()))
    advance();

  // Look for a fractional part.
  if (peek() == '.' && isDigit(peekNext())) {
    // Consume the ".".
    advance();

    while (isDigit(peek()))
      advance();
  }

  return makeToken(TOKEN_NUMBER);
}

static Token string() {
  while (peek() != '"' && !isAtEnd()) {
    if (peek() == '\n')
      lexer.line++;
    advance();
  }

  if (isAtEnd())
    return errorToken("Unterminated string.");

  // The closing quote.
  advance();
  return makeToken(TOKEN_STRING);
}

Token scanToken() {
  skipWhitespace();
  lexer.start = lexer.current;

  if (isAtEnd())
    return makeToken(TOKEN_EOF);

  char c = advance();

  if (isAlpha(c))
    return identifier();
  if (isDigit(c))
    return number();

  switch (c) {
  case '(':
    return makeToken(TOKEN_LEFT_PAREN);
  case ')':
    return makeToken(TOKEN_RIGHT_PAREN);
  case '{':
    return makeToken(TOKEN_LEFT_BRACE);
  case '}':
    return makeToken(TOKEN_RIGHT_BRACE);
  case ';':
    return makeToken(TOKEN_SEMICOLON);
  case ',':
    return makeToken(TOKEN_COMMA);
  case '.':
    return makeToken(TOKEN_DOT);
  case '-':
    return makeToken(TOKEN_MINUS);
  case '+':
    return makeToken(TOKEN_PLUS);
  case '/':
    return makeToken(TOKEN_SLASH);
  case '*':
    return makeToken(TOKEN_STAR);
  case '%':
    return makeToken(TOKEN_PERCENT);
  case '!':
    return makeToken(match('=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
  case '=':
    return makeToken(match('=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
  case '<':
    return makeToken(match('=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
  case '>':
    return makeToken(match('=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);
  case '&':
    if (match('&'))
      return makeToken(TOKEN_AND_AND);
    break;
  case '|':
    if (match('|'))
      return makeToken(TOKEN_OR_OR);
    break;
  case '"':
    return string();
  }

  return errorToken("Unexpected character.");
}

const char *tokenName(TokenType type) {
  switch (type) {
  case TOKEN_LEFT_PAREN:
    return "LEFT_PAREN";
  case TOKEN_RIGHT_PAREN:
    return "RIGHT_PAREN";
  case TOKEN_LEFT_BRACE:
    return "LEFT_BRACE";
  case TOKEN_RIGHT_BRACE:
    return "RIGHT_BRACE";
  case TOKEN_COMMA:
    return "COMMA";
  case TOKEN_DOT:
    return "DOT";
  case TOKEN_SEMICOLON:
    return "SEMICOLON";
  case TOKEN_PLUS:
    return "PLUS";
  case TOKEN_MINUS:
    return "MINUS";
  case TOKEN_STAR:
    return "STAR";
  case TOKEN_SLASH:
    return "SLASH";
  case TOKEN_PERCENT:
    return "PERCENT";
  case TOKEN_BANG:
    return "BANG";
  case TOKEN_BANG_EQUAL:
    return "BANG_EQUAL";
  case TOKEN_EQUAL_EQUAL:
    return "EQUAL_EQUAL";
  case TOKEN_GREATER:
    return "GREATER";
  case TOKEN_GREATER_EQUAL:
    return "GREATER_EQUAL";
  case TOKEN_LESS:
    return "LESS";
  case TOKEN_LESS_EQUAL:
    return "LESS_EQUAL";
  case TOKEN_AND_AND:
    return "AND_AND";
  case TOKEN_OR_OR:
    return "OR_OR";
  case TOKEN_EQUAL:
    return "EQUAL";
  case TOKEN_IDENTIFIER:
    return "IDENTIFIER";
  case TOKEN_STRING:
    return "STRING";
  case TOKEN_NUMBER:
    return "NUMBER";
  case TOKEN_LET:
    return "LET";
  case TOKEN_MUT:
    return "MUT";
  case TOKEN_FN:
    return "FN";
  case TOKEN_RETURN:
    return "RETURN";
  case TOKEN_IF:
    return "IF";
  case TOKEN_ELSE:
    return "ELSE";
  case TOKEN_WHILE:
    return "WHILE";
  case TOKEN_FOR:
    return "FOR";
  case TOKEN_TRUE:
    return "TRUE";
  case TOKEN_FALSE:
    return "FALSE";
  case TOKEN_PRINT:
    return "PRINT";
  case TOKEN_PRINTLN:
    return "PRINTLN";
  case TOKEN_LEN:
    return "LEN";
  case TOKEN_INPUT:
    return "INPUT";
  case TOKEN_PARSE_INT:
    return "PARSE_INT";
  case TOKEN_PARSE_FLOAT:
    return "PARSE_FLOAT";
  case TOKEN_TO_STRING:
    return "TO_STRING";
  case TOKEN_INT:
    return "INT";
  case TOKEN_FLOAT:
    return "FLOAT";
  case TOKEN_BOOL:
    return "BOOL";
  case TOKEN_STRING_TYPE:
    return "STRING_TYPE";
  case TOKEN_VOID:
    return "VOID";
  case TOKEN_ERROR:
    return "ERROR";
  case TOKEN_EOF:
    return "EOF";
  default:
    return "UNKNOWN";
  }
}
