#ifndef LINKEDLIST_H_
#define LINKEDLIST_H_

#include <inttypes.h>

typedef struct LinkedListNode {
	__uint128_t data;
	struct LinkedListNode* next;
	struct LinkedListNode* prev;
} LinkedListNode;

typedef struct LinkedList {
	int offsetSize, tagSize, indexSize, setSize, size;
	struct LinkedListNode* head;
	struct LinkedListNode* tail;
} LinkedList;

extern LinkedListNode* LinkedListNode_new(__uint128_t data);
extern LinkedList* LinkedList_new(int lineSize, int cacheSize, int associativity);
extern bool LinkedList_isEmpty(LinkedList* list)
extern void LinkedList_add(LinkedList* list);
extern void LinkedList_remove(LinkedList* list, __uint128_t data);
extern void LinkedList_delete(LinkedList* list);

#endif
