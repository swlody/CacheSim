#include "LinkedList.h"
#include <math.h>
#include <stdbool.h>

int main(int argc, char* argv){}

LinkedListNode* LinkedList_new(__uint128t data, LinkedListNode* next, LinkedListNode* prev);
{
    LinkedListNode* newNode = (LinkedListNode*)malloc(sizeof(LinkedListNode));
    newNode->data = data;
    newNode->next = next;
    newNode->prev = prev;
    return newNode;
}

LinkedList* LinkedList_new(int setSize, int offsetSize, int indexSize, int tagSize)
{
    LinkedList* list = (LinkedList*)malloc(sizeof(LinkedList));
    list->head = NULL;
    list->tail = NULL;
    list->offsetSize = offsetSize;
    list->setSize = setSize;
    list->indexSize = indexSize;
    list->tagSize = tagSize;
    list->size = 0;
    return list;
}

bool LinkedList_isEmpty(LinkedList* list)
{
    return size == 0;
}

void LinkedList_add(LinkedList* list, __uint128t data)
{
    if(LinkedList_isEmpty(list)) {
        list->head = LinkedListNode_new(data, NULL, NULL);
        list->tail = list->head;
    } else {
        list->tail->next = LinkedListNode_new(list, NULL, list->tail);
        list->tail = list->tail->next;
    }
    size++;
}

bool LinkedList_remove(LinkedList* list, __uint128t data)
{
    if(!LinkedList_isEmpty(list)) {
        LinkedListNode* node = list->head;
        while(node != NULL) {
            if(node->data == data) {
                node->next->prev = node->prev;
                node->prev->next = node->next;
                free(node);
                return true;
            } else {
                node = node->next;
            }
        }
    }
    return false;
}

void LinkedList_delete(LinkedList* list)
{
    if(LinkedList_isEmpty(list)) {
        free(list);
    } else {
        LinkedListNode* node = list->head;
        while(node != NULL) {
            LinkedListNode* newNode = node->next;
            free(node);
            node = newNode;
        }
        free(list);
    }
}
