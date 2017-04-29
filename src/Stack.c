#include "Stack.h"
#include <stdlib.h>
#include <stdbool.h>

int size = 0;
int maxSize;

StackNode* StackNode_new(int32_t data, StackNode* next)
{
	StackNode* node = (StackNode*)malloc(sizeof(StackNode));
	if(node == NULL)
		abort();
	node->data = data;
	node->next = next;
	return node;
}

Stack* Stack_new(int size)
{
	Stack* stack = (Stack*)malloc(sizeof(Stack));
	if(stack == NULL)
		abort();
	stack->head = NULL;
	stack->maxSize = size * 1024;
	return stack;
}

bool Stack_isEmpty(Stack* stack)
{
	return size == 0;
}

bool Stack_push(Stack* stack, int32_t data)
{
	if(size >= maxSize)
		return false;
	size++;
	if(Stack_isEmpty(stack)) {
		stack->head = StackNode_new(data, NULL);
	} else {
		stack->head = StackNode_new(data, stack->head->next);
	}
	return true;
}

// TODO This and peek returns 0 when the stack is emptys
int32_t Stack_pop(Stack* stack)
{
	size--;
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
