/*
 * Ari Geller & Sam Wlody
 * CSC 252 - Project 4
 */

#include "LinkedList.h"
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>

LinkedListNode* LinkedListNode_new(int setSize)
{
    LinkedListNode* newNode = (LinkedListNode*)malloc(sizeof(LinkedListNode));
    newNode->set = malloc(16*setSize);
    newNode->next = NULL;
    newNode->valid = false;
    newNode->dirty = false;
    newNode->tag = 0;
    return newNode;
}

LinkedList* LinkedList_new(int setSize)
{
    LinkedList* list = (LinkedList*)malloc(sizeof(LinkedList));
    list->head = NULL;
    list->tail = NULL;
    list->setSize = setSize;
    return list;
}

bool LinkedList_isEmpty(LinkedList* list)
{
    return list->head == NULL;
}

void LinkedList_add(LinkedList* list)
{
    LinkedListNode* newNode = LinkedListNode_new(list->setSize);
    if(LinkedList_isEmpty(list)) {
        list->head = newNode;
        list->tail = list->head;
    } else {
        list->tail->next = newNode;
    }
}

void LinkedList_delete(LinkedList* list)
{
    if(LinkedList_isEmpty(list)) {
        free(list);
    } else {
        LinkedListNode* node = list->head;
        while(node != NULL) {
            LinkedListNode* newNode = node->next;
            free(node->set);
            free(node);
            node = newNode;
        }
        free(list);
    }
}
