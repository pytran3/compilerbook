#include "9cc.h"

Token *token;
char *user_input;

void error(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

void error_at(char *loc, char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);

  int pos = loc - user_input;
  fprintf(stderr, "%s\n", user_input);
  fprintf(stderr, "%*s", pos, ""); // pos個の空白を出力
  fprintf(stderr, "^ ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

bool is_expect(char *op) {
#pragma GCC diagnostic ignored "-Wsign-compare"
  return !(token->kind != TK_RESERVED ||
      strlen(op) != token->len|| 
      memcmp(token->str, op, token->len));
}

bool consume(char *op) {
  if (!is_expect(op))
    return false;
  token = token->next;
  return true;
}

bool is_ident_char(char c) {
  return  'a' <= c && c <= 'z' ||
          'A' <= c && c <= 'Z' ||
          '0' <= c && c <= '9' ||
          c == '_';
}

Token *consume_ident() {
  if (token->kind == TK_IDENT) {
    Token *tok = token;
    token = token->next;
    return tok;
  }
  return NULL;
}

void expect(char *op) {
  if (!is_expect(op))
    error_at(token->str, "'%c'ではありません", op[0]);
  token = token->next;
}

long expect_number() {
  if (token->kind != TK_NUM)
    error("数ではありません");
  int val = token->val;
  token = token->next;
  return val;
}

bool at_eof() {
  return token->kind == TK_EOF;
}

int len_ident(char *p) {
  char *ret = p;
  for(;*ret;ret++) {
    if (!is_ident_char(*ret)) break;
  }
  return ret - p;
}

Token *new_token(TokenKind kind, Token *cur, char *str, int len) {
  Token *tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->str = str;
  tok->len = len;
  cur->next = tok;
  return tok;
}

bool startswith(char *p, char *q) {
  return memcmp(p, q, strlen(q)) == 0;
}


Token *tokenize() {
  char *p = user_input;
  Token head;
  head.next = NULL;
  Token *cur = &head;

  while (*p) {
    if (isspace(*p)) {
      p++;
      continue;
    }
    if (startswith(p, "==") || startswith(p, "!=") ||
        startswith(p, "<=") || startswith(p, ">=")) {
      cur = new_token(TK_RESERVED, cur, p, 2);
      p += 2;
      continue;
    }

    if (strchr("+-*/()<>=;", *p)) {
      cur = new_token(TK_RESERVED, cur, p++, 1);
      continue;
    }

    if (isdigit(*p)) {
      cur = new_token(TK_NUM, cur, p, 0);
      cur->val = strtol(p, &p, 10);
      continue;
    }

    if ('a' <= *p && *p <= 'z') {
      int len = len_ident(p);
      cur = new_token(TK_IDENT, cur, p, len);
      p += len;
      continue;
    }

    error_at(p, "トークナイズできません");
  }

  new_token(TK_EOF, cur, p, 0);
  return head.next;
}