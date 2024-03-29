#include "9cc.h"

int label_id = 0;

void gen_lval(Node *node) {
  if (node->kind != ND_LVAR) {
    error("代入の左辺値が変数ではありません");
  }

  printf("  mov rax, rbp\n");
  printf("  sub rax, %ld\n", node->offset);
  printf("  push rax\n");
}

void gen(Node *node) {
  if (node->kind == ND_NUM) {
    printf("  push %ld\n", node->val);
    return;
  }
  if (node->kind == ND_LVAR) {
    gen_lval(node);
    printf("  pop rax\n");
    printf("  mov rax, [rax]\n");
    printf("  push rax\n");
    return;
  }
  if (node->kind == ND_ASSIGN) {
    gen_lval(node->lhs);
    gen(node->rhs);
    printf("  pop rdi\n");
    printf("  pop rax\n");
    printf("  mov [rax], rdi\n");
    printf("  push rdi\n");
    return;
  }
  if (node->kind == ND_RETURN) {
    gen(node->lhs);
    printf("  pop rax\n");
    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");
    return;    
  }
  if (node->kind == ND_IF) {
    int l1 = label_id++;
    int l2 = label_id++;
    int l3 = label_id++;
    gen(node->cond);
    printf("  pop rax\n");
    printf("  test rax, rax\n");
    printf("  jne .L%d\n", l1);
    printf("  jmp .L%d\n", l2);
    printf(".L%d:\n", l1);
    gen(node->then);
    printf("  jmp .L%d\n", l3);
    printf(".L%d:\n", l2);
    if (node->els != NULL) gen(node->els);
    printf(".L%d:\n", l3);
    return;
  }
  if (node->kind == ND_WHILE) {
    int l1 = label_id++;
    int l2 = label_id++;
    printf(".L%d:\n", l1);
    gen(node->cond);
    printf("  pop rax\n");
    printf("  test rax, rax\n");
    printf("  je .L%d\n", l2);
    gen(node->then);
    printf("  jmp .L%d\n", l1);
    printf(".L%d:\n", l2);
    return;
  }
  if (node->kind == ND_BLOCK_BEGIN) {
    node = node->next;
    while(node->kind == ND_BLOCK) {
      gen(node->stmt);
      if(node->next->kind != ND_BLOCK_END) printf("  pop rax\n");
      node = node->next;
    }
    return;
  }
  if (node->kind == ND_FUNCTION) {
    printf("  call %.*s\n", node->len, node->name);
    printf("  push rax\n");
    return;
  }

  gen(node->lhs);
  gen(node->rhs);
  printf("  pop rdi\n");
  printf("  pop rax\n");

  if (node->kind == ND_ADD) {
    printf("  add rax, rdi\n");
  }
  else if (node->kind == ND_SUB) {
    printf("  sub rax, rdi\n");
  }
  else if (node->kind == ND_MUL) {
    printf("  imul rax, rdi\n");    
  }
  else if (node->kind == ND_DIV) {
    printf("  cqo\n");
    printf("  idiv rdi\n");
  }
  else if (node->kind == ND_EQ) {
    printf("  cmp rax, rdi\n");
    printf("  sete al\n");
    printf("  movzb rax, al\n");
  }
  else if (node->kind == ND_NE) {
    printf("  cmp rax, rdi\n");
    printf("  setne al\n");
    printf("  movzb rax, al\n");
  }
  else if (node->kind == ND_LT) {
    printf("  cmp rax, rdi\n");
    printf("  setl al\n");
    printf("  movzb rax, al\n");
  }
  else if (node->kind == ND_LE) {
    printf("  cmp rax, rdi\n");
    printf("  setle al\n");
    printf("  movzb rax, al\n");    
  }
  else {
    error("予期しないトークンが来ました\n");
  }

  printf("  push rax\n");
}
