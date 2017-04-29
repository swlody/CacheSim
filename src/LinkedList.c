#include "LinkedList.h"
#include <math.h>
#include <stdbool.h>

int main(int argc, char* argv){}

LinkedListNode* LinkedListNode_new(int setSize);
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
    list->setSize = setSize;
    list->size = 0;
    return list;
}

bool LinkedList_isEmpty(LinkedList* list)
{
    return list->head == NULL;
}

void LinkedList_add(LinkedList* list)
{
    if(LinkedList_isEmpty(list)) {
        list->head = LinkedListNode_new(list->setSize);
    } else {
        list->tail->next = LinkedListNode_new(list->setSize);
    }
    size++;
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
