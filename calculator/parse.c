#include "9cc.h"

Node *code[100];
LVar *locals;
Function *functions;

Node *new_node(NodeKind kind) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  return node;
}

Node *new_binary(NodeKind kind, Node *lhs, Node *rhs) {
  Node *node = new_node(kind);
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

Node *new_num(int val) {
  Node *node = new_node(ND_NUM);
  node->val = val;
  return node;
}

LVar *find_lvar(Token *tok) {
  for (LVar *var = locals; var; var = var->next) {
    if(var->len == tok->len && !memcmp(tok->str, var->name, var->len)) {
      return var;
    }
  }
  return NULL;
}

Function *find_function(Token *tok) {
  for (Function *var = functions; var; var = var->next) {
    if(var->len == tok->len && !memcmp(tok->str, var->name, var->len)) {
      return var;
    }
  }
  return NULL;
}

void program();
Node *stmt();
Node *expr();
Node *assign();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *primary();
Node *unary();


void program() {
  int i = 0;
  while (!at_eof())
    code[i++] = stmt();
  code[i] = NULL;
}

Node *stmt() {
  Node *node;
  if (consume_return()) {
    node = calloc(1, sizeof(Node));
    node->kind = ND_RETURN;
    node->lhs = expr();
    expect(";");
  }
  else if (consume_if()) {
    node = calloc(1, sizeof(Node));
    node->kind = ND_IF;
    node->cond = expr();
    node->then = stmt();
    if (consume_else()) {
      node->els = stmt();
    }
  }
  else if (consume_while()) {
    node = calloc(1, sizeof(Node));
    node->kind = ND_WHILE;
    node->cond = expr();
    node->then = stmt();
  }
  else if (consume_block_begin()) {
    node = calloc(1, sizeof(Node));
    node->kind = ND_BLOCK_BEGIN;
    Node *head = node;
    while (!consume_block_end()) {
      Node *new_node = calloc(1, sizeof(Node));
      new_node->kind = ND_BLOCK;
      head->next = new_node;
      new_node->stmt = stmt();
      head = new_node;
    }
    Node *end_node = calloc(1, sizeof(Node));
    end_node->kind = ND_BLOCK_END;
    head->next = end_node;
  }
  else {
    node = expr();
    expect(";");
  }
  return node;
}

Node *expr() {
  return assign();
}

Node *assign() {
  Node *node = equality();
  if (consume("=")) {
    node = new_binary(ND_ASSIGN, node, assign());
  }
  return node;
}

Node *equality() {
  Node *node = relational();

  for (;;) {
    if (consume("+"))
      node = new_binary(ND_ADD, node, relational());
    else if (consume("-"))
      node = new_binary(ND_SUB, node, relational());
    else
      return node;
  }
}

Node *relational() {
  Node *node = add();

  for (;;) {
    if (consume("<"))
      node = new_binary(ND_LT, node, add());
    else if (consume("<="))
      node = new_binary(ND_LE, node, add());
    else if (consume(">"))
      node = new_binary(ND_LT, add(), node);
    else if (consume(">="))
      node = new_binary(ND_LE, add(), node);
    else
      return node;
  }
}

Node *add() {
  Node *node = mul();

  for (;;) {
    if (consume("+"))
      node = new_binary(ND_ADD, node, mul());
    else if (consume("-"))
      node = new_binary(ND_SUB, node, mul());
    else
      return node;
  }
}

Node *mul() {
  Node *node = unary();

  for (;;) {
    if (consume("*"))
      node = new_binary(ND_MUL, node, unary());
    else if (consume("/"))
      node = new_binary(ND_DIV, node, unary());
    else
      return node;
  }
}

Node *unary() {
  if (consume("+"))
    return primary();
  if (consume("-"))
    return new_binary(ND_SUB, new_num(0), primary());
  return primary();
}

Node *primary() {
  if (consume("(")) {
    Node *node = expr();
    expect(")");
    return node;
  }
  
  Token *tok = consume_ident();
  if (tok) {
    if (consume("(")) {
      Node *node = calloc(1, sizeof(Node));
      node->kind = ND_FUNCTION;
      node->name = tok->str;
      node->len = tok->len;
      Function *function = find_function(tok);
      if(function) {
        node->offset = function->offset;
      }
      else {
        function = calloc(1, sizeof(Function));
        function->next = functions;
        function->name = tok->str;
        function->len = tok->len;
        function->offset = functions? functions->offset + 8: 0;
        node->offset = function->offset;
        functions = function;
      }
      expect(")");
      return node;
    }
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_LVAR;
    node->name = tok->str;
    node->len = tok->len;
    LVar *lvar = find_lvar(tok);
    if (lvar) {
      node->offset = lvar->offset;
    }
    else {
      lvar = calloc(1, sizeof(LVar));
      lvar->next = locals;
      lvar->name = tok->str;
      lvar->len = tok->len;
      lvar->offset = locals? locals->offset + 8: 0;
      node->offset = lvar->offset;
      locals = lvar;
    }
    return node;
  }

  return new_num(expect_number());
}
