#include "lexer.c"
#include <stdio.h>

int main() {
  const char *source = "let mut x = 10;\n"
                       "fn add(a: int, b: int) -> int {\n"
                       "    return a + b;\n"
                       "}\n"
                       "if (x > 5) {\n"
                       "    println!(\"x is greater than 5\");\n"
                       "} else {\n"
                       "    print!(\"x is small\");\n"
                       "}\n";
  initLexer(source);
  Token token;
  do {
    token = scanToken();
    printf("%d: %.*s -> %s\n", token.line, token.length, token.start,
           tokenName(token.type));
  } while (token.type != TOKEN_EOF);
  return 0;
}
