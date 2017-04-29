#include "Stack.h"
#include <stdlib.h>
#include <stdbool.h>

StackNode* StackNode_new(int32_t data, StackNode* next) {
	StackNode* node = (StackNode*)malloc(sizeof(StackNode));
	if(node == NULL) {
		abort();
	}
	node->data = data;
	node->next = next;
	return node;
}

Stack* Stack_new()
{
	Stack* stack = (Stack*)malloc(sizeof(Stack));
	stack->head = NULL;
	return stack;
}

bool Stack_isEmpty(Stack* stack) {
	if(stack->head == NULL) {
		return true;
	} else {
		return false;
	}
}

void Stack_push(Stack* stack, int32_t data)
{
	if(Stack_isEmpty(stack)) {
		stack->head = StackNode_new(data, NULL);
	} else {
		stack->head = StackNode_new(data, stack->head->next);
	}
}

// TODO This and peek returns 0 when the stack is emptys
int32_t Stack_pop(Stack* stack)
{
	if(Stack_isEmpty(stack)) {
		return 0;
	} else {
		int32_t data = stack->head->data;
		free(stack->head);
		stack->head = stack->head->next;
		return data;
	}
}

int32_t Stack_peek(Stack* stack)
{
	if(Stack_isEmpty(stack)) {
		return 0;
	} else {
		return stack->head->data;
	}
}

void Stack_delete(Stack* stack)
{
	if(stack->head == NULL) {
		free(stack);
	} else {
		StackNode* node = stack->head;
		while(node->next != NULL) {
			StackNode* newNode = node->next;
			free(node);
			node = newNode;
		}
		free(node);
		free(stack);
	}
}
