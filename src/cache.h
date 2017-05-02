/*
 * Ari Geller & Sam Wlody
 * CSC 252 - Project 4
 */

#ifndef _CACHE_H_
#define _CACHE_H_

#include <stdbool.h>
#include <inttypes.h>

extern int write_xactions;
extern int read_xactions;

// TODO Dirty bit not necessary for fully-associative cache
typedef struct Block {
	uint32_t tag;
	bool dirty;
	struct Block* next;
} Block;

// A set is a LinkedList (FIFO queue) of blocks
typedef struct Set {
	struct Block* front;
	struct Block* back;
	int size;
} Set;

extern void printHelp(const char * prog);

extern void initialize(int sets);
extern uint32_t getIndex(uint32_t addr, int tagSize, int offsetSize);
extern uint32_t getTag(uint32_t addr, int indexSize, int offsetSize);
extern Block* Block_new(uint32_t tag, bool dirty);
extern Set* Set_new();
extern void Set_addBlock(Set* set, int setSize, uint32_t tag, bool store);
extern bool Set_contains(Set* set, uint32_t tag, bool store);
extern void Set_delete(Set* set);
extern void Cache_delete(int sets);

#endif
