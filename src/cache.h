/*
 * Ari Geller & Sam Wlody
 * CSC 252 - Project 4
 */

#ifndef CACHE_H_
#define CACHE_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>

extern int write_xactions;
extern int read_xactions;

typedef struct Block {
	uint32_t tag;
	bool dirty;
	struct Block* next;
} Block;


// A set is a LinkedList of blocks
typedef struct Set {
	struct Block* first;
	struct Block* last;
	struct Set* next;
	int size;
} Set;

// A cache is a LinkedList of sets
typedef struct Cache {
	int setSize;
	struct Set* first;
	struct Set* last;
} Cache;

extern Block* Block_new();
extern Set* Set_new(int setSize);
extern Cache* Cache_new(int setSize);
extern bool Set_isEmpty(Set* set);
extern bool Cache_isEmpty(Cache* cache);
extern void Cache_addSet(Cache* cache, int index);
extern void Cache_delete(Cache* cache);

void printHelp(const char * prog);
void cache_init(int setSize, int sets);

#endif
