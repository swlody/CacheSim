/*
 * Ari Geller & Sam Wlody
 * CSC 252 - Project 4
 */

#ifndef QUEUE_H_
#define QUEUE_H_

#include <inttypes.h>
#include <stdbool.h>

typedef struct QueueNode {
	uint32_t data;
	struct QueueNode* next;
	struct QueueNode* prev;
} QueueNode;

typedef struct Queue {
	struct QueueNode* first;
	struct QueueNode* last;
} Queue;

extern QueueNode* QueueNode_new(uint32_t data, QueueNode* next, QueueNode* prev);
extern Queue* Queue_new();
extern bool Queue_isEmpty(Queue* queue);
extern bool Queue_add(Queue* queue, int32_t data);
extern uint32_t Queue_dequeue(Queue* queue);
extern uint32_t Queue_peek(Queue* queue);
extern void Queue_delete(Queue* queue);

#endif
