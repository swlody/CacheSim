/*
 * Ari Geller & Sam Wlody
 * CSC 252 - Project 4
 */

#ifndef LINKEDLIST_H_
#define LINKEDLIST_H_

#include <inttypes.h>
#include <stdbool.h>

typedef struct LinkedListNode {
	uint32_t tag;
	bool valid;
	bool dirty;
	__uint128_t* set;
	struct LinkedListNode* next;
	struct LinkedListNode* prev;
} LinkedListNode;

typedef struct LinkedList {
	int setSize;
	struct LinkedListNode* head;
	struct LinkedListNode* tail;
} LinkedList;

extern LinkedListNode* LinkedListNode_new(int setSize);
extern LinkedList* LinkedList_new(int setSize);
extern bool LinkedList_isEmpty(LinkedList* list);
extern void LinkedList_add(LinkedList* list);
extern void LinkedList_delete(LinkedList* list);

#endif
