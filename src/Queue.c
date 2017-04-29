/*
 * Ari Geller & Sam Wlody
 * CSC 252 - Project 4
 */

#include "Queue.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdlib.h>

int size = 0;
int maxSize;

QueueNode* QueueNode_new(uint32_t data, QueueNode* next, QueueNode* prev)
{
	QueueNode* node = (QueueNode*)malloc(sizeof(QueueNode));
	if(node == NULL)
		abort();
	node->data = data;
	node->next = next;
	node->prev = prev;
	return node;
}

Queue* Queue_new(int size)
{
	Queue* queue = (Queue*)malloc(sizeof(Queue));
	if(queue == NULL)
		abort();
	queue->head = NULL;
	queue->tail = NULL;
	return queue;
}

bool Queue_isEmpty(Queue* queue)
{
	return size == 0;
}

bool Queue_add(Queue* queue, int32_t data)
{
	if(size >= maxSize)
		return false;
	if(Queue_isEmpty(queue)) {
		queue->head = QueueNode_new(data, NULL, NULL);
		queue->tail = queue->head;
	} else {
		QueueNode* newNode = QueueNode_new(data, queue->tail, NULL);
		queue->tail = newNode;
	}
	return true;
}

// TODO This and peek returns 0 when the queue is emptys
uint32_t Queue_dequeue(Queue* queue)
{
	if(Queue_isEmpty(queue)) {
		return 0;
	} else {
		uint32_t data = queue->head->data;
		QueueNode* newHead = queue->head->prev;
		free(queue->head);
		queue->head = newHead;
		queue->head->next = NULL;
		return data;
	}
}

uint32_t Queue_peek(Queue* queue)
{
	if(Queue_isEmpty(queue)) {
		return 0;
	} else {
		return queue->head->data;
	}
}

void Queue_delete(Queue* queue)
{
	if(queue->head == NULL) {
		free(queue);
	} else {
		QueueNode* node = queue->head;
		while(node != NULL) {
			QueueNode* newNode = node->prev;
			free(node);
			node = newNode;
		}
		free(queue);
	}
}
