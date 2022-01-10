#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//
// tokenize.c
//

typedef enum {
  TK_RESERVED, // Keywords or punctuators
  TK_IDENT,    // Identifier
  TK_NUM,      // Integer literals
  TK_RETURN,   // Return
  TK_IF,       // If
  TK_ELSE,     // Else
  TK_WHILE,    // While
  TK_EOF,      // End-of-file markers
} TokenKind;

// Token type
typedef struct Token Token;
struct Token {
  TokenKind kind; // Token kind
  Token *next;    // Next token
  long val;       // If kind is TK_NUM, its value
  char *str;      // Token string
  int len;        // Token length
};

void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);
bool consume(char *op);
Token *consume_ident();
bool consume_return();
bool consume_if();
bool consume_else();
bool consume_while();
void expect(char *op);
long expect_number(void);
bool is_token(TokenKind tk, int n, Token *token);
bool at_eof(void);
Token *tokenize(void);

extern char *user_input;
extern Token *token;

//
// parse.c
//

typedef enum {
  ND_ADD,    // +
  ND_SUB,    // -
  ND_MUL,    // *
  ND_DIV,    // /
  ND_EQ,     // ==
  ND_NE,     // !=
  ND_LT,     // <
  ND_LE,     // <=
  ND_LVAR,   // Local Identifier
  ND_NUM,    // Integer
  ND_ASSIGN, // Assignment
  ND_RETURN, // Return
  ND_IF,     // If
  ND_WHILE,  // While
} NodeKind;

// AST node type
typedef struct Node Node;
struct Node {
  NodeKind kind; // Node kind
  Node *lhs;     // Left-hand side
  Node *rhs;     // Right-hand side
  long val;      // Used if kind == ND_NUM
  long offset;   // Used if kind == ND_LVAR
  // If Statement
  Node *cond;    // Condition
  Node *then;    // Then
  Node *els;     // Else
};

// IDENTIFIER
typedef struct LVar LVar;
struct LVar {
  LVar *next;
  char *name;
  int len;
  int offset;
};
void program(void);

extern Node *code[100];

//
// codegen.c
//

void gen(Node *node);