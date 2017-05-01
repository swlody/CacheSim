/*
 * Ari Geller & Sam Wlody
 * CSC 252 - Project 4
 */

#ifndef CACHE_H_
#define CACHE_H_

#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

extern int write_xactions;
extern int read_xactions;

typedef struct Block {
	uint32_t tag;
	bool dirty;
	struct Block* next;
} Block;

// A set is a LinkedList (queue) of blocks
typedef struct Set {
	struct Block* front;
	struct Block* back;
	struct Set* next; // Only used for fully-associative cache
	int size;
} Set;

extern void printHelp(const char * prog);

extern void initialize(int sets);
extern uint32_t getIndex(uint32_t addr, int tagSize, int offsetSize);
extern uint32_t getTag(uint32_t addr, int indexSize, int offsetSize);
extern bool FoundInSet(Set* set, uint32_t tag, bool store);
extern Block* Block_new(bool dirty, uint32_t tag);
extern Set* Set_new();
extern bool Set_isEmpty(Set* set);
extern void Set_addBlock(Set* set, Block* newBlock, int setSize);
extern void Cache_delete(int sets);
extern void Set_delete(Set* set);

#endif
