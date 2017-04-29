#ifndef STACK_H_
#define STACK_H_

#include <inttypes.h>
#include <stdbool.h>

typedef struct StackNode {
	int32_t data;
	struct StackNode* next;
} StackNode;

typedef struct Stack {
	struct StackNode* head;
} Stack;

extern Stack* Stack_new();
extern bool Stack_isEmpty(Stack* stack);
extern void Stack_push(Stack* stack, int32_t data);
extern int32_t Stak_pop(Stack* stack);
extern int32_t Stack_peek(Stack* stack);
extern void Stack_delete(Stack* stack);

#endif