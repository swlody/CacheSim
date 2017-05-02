/*
 * Ari Geller & Sam Wlody
 * CSC 252 - Project 4
 */

#include "cache.h"
#include <math.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

int write_xactions = 0;
int read_xactions = 0;
Set* fullyAssociativeCache;
Set* infiniteFullyAssociativeCache;
Set** cache; // cacheTable

// Print help message to user
void printHelp(const char * prog)
{
	printf("%s [-h] | [-s <size>] [-w <ways>] [-l <line>] [-t <trace>]\n", prog);
	printf("options:\n");
	printf("-h: print out help text\n");
	printf("-s <cache size>: set the total size of the cache in KB\n");
	printf("-w <ways>: set the number of ways in each set\n");
	printf("-l <line size>: set the size of each cache line in bytes\n");
	printf("-t <trace>: use <trace> as the input file for memory traces\n");
}

/*
	Main function. Feed to options to set the cache
	
	Options:
	-h : print out help message
	-s : set L1 cache Size (KB)
	-w : set L1 cache ways
	-l : set L1 cache line size
*/

int main(int argc, char* argv[])
{
	int i;
	uint32_t size = 32; // Total size of L1$ (KB)
	uint32_t ways = 1;  // # of ways in L1. Default to direct-mapped
	uint32_t line = 32; // Line size (B)

	// Hit and miss counts
	int totalHits = 0;
	int totalMisses = 0;

	char* filename = "";

	// Strings to compare
	const char* helpString = "-h";
	const char* sizeString = "-s";
	const char* waysString = "-w";
	const char* lineString = "-l";
	const char* traceString = "-t";

	if(argc == 1) {
		// No arguments passed, show help
		printHelp(argv[0]);
		return 1;
	}

	/******** Parse command line ********/
	for(i=1; i<argc; i++) {
		//check for help
		if(!strcmp(helpString, argv[i])) {
			// Print out help text and terminate
			printHelp(argv[0]);
			return 1; //return 1 for help termination
		} else if(!strcmp(sizeString, argv[i])) {
			// Take next string and convert to int
			i++; // Increment i so that it skips data string in the next loop iteration
			// Check next string's first char. If not digit, fail
			if(isdigit(argv[i][0])) {
				size = atoi(argv[i]);
			} else {
				printf("Incorrect formatting of size value\n");
				printHelp(argv[0]);
				return -1; //input failure
			}
		} else if(!strcmp(waysString, argv[i])) {
			//take next string and convert to int
			i++; //increment i so that it skips data string in the next loop iteration
			//check next string's first char. If not digit, fail
			if(isdigit(argv[i][0])) {
				ways = atoi(argv[i]);
			} else {
				printf("Incorrect formatting of ways value\n");
				printHelp(argv[0]);
				return -1; //input failure
			}
		} else if(!strcmp(lineString, argv[i])) {
			// Take next string and convert to int
			i++; // Increment i so that it skips data string in the next loop iteration
			// Check next string's first char. If not digit, fail
			if(isdigit(argv[i][0])) {
				line = atoi(argv[i]);
			} else {
				printf("Incorrect formatting of line size value\n");
				printHelp(argv[0]);
				return -1; //input failure
			}
		} else if(!strcmp(traceString, argv[i])) {
			filename = argv[++i];
		} else {
			printf("Unrecognized argument.\n");
			printHelp(argv[0]);
			return -1;
		}
	}

	/******** Initialization ********/
	// Calculations
	const int sets = (size * 1024) / (line * ways);
	const int offsetSize = log2((double) line);
	const int indexSize = log2((double) sets);
	const int tagSize = 32 - (indexSize + offsetSize);
	const int blocks = ways * sets;

	// Initialize the cache
	initialize(sets);

	// Print info
	printf("Ways: %u; Sets: %u; Line Size: %uB\n", ways, sets, line);
	printf("Tag: %d bits; Index: %d bits; Offset: %d bits\n", tagSize, indexSize, offsetSize);

	// Open trace file
	FILE* fp = fopen(filename, "r");
	strcat(filename, ".simulated");
	FILE* fpw = fopen(filename, "w");
	char buff[13];

	/******** Simulate cache ********/
	// Until end of trace file...
	while(fgets(buff, 13, fp) != NULL) {
		// Get load or store from line - defaults to load
		bool store = buff[0] == 's';
		char strAddr[11];

		// Get address in string form from line
		memcpy(strAddr, buff+2, 10);
		strAddr[10] = '\0';

		// Convert hex string to integer representation
		uint32_t addr = (uint32_t)strtol(strAddr, NULL, 0);

		// Retrieve index and tag
		int index = getIndex(addr, tagSize, offsetSize) % sets;
		// The mapped cache uses the tag portion of the address only
		uint32_t tag = getTag(addr, indexSize, offsetSize);
		// Fully associative caches use the entire address ignoring the offset
		uint32_t fullTag = getTag(addr, 0, offsetSize);
		char* classification;

		// Check if value is found in set based on tag
		if(Set_contains(cache[index], tag, store)) {
			// Hit
			totalHits++;
			classification = "hit";
			if(!Set_contains(fullyAssociativeCache, fullTag, store))
				Set_addBlock(fullyAssociativeCache, blocks, fullTag, store);
		} else {
			// Miss
			totalMisses++;
			// Classify the miss
			if(!Set_contains(infiniteFullyAssociativeCache, fullTag, store)) {
				// Address has never been seen before
				classification = "compulsory";
				/* 
				 * Using -1 as max size makes the cache effectively infinite.
				 * Size starts at 0 and == is used for comparison, so -1 will not be reached 
				 * until after a full overflow cycle (INT_MAX * 2 elements in cache).
				 */
				Set_addBlock(infiniteFullyAssociativeCache, -1, fullTag, store);
				Set_addBlock(fullyAssociativeCache, blocks, fullTag, store);
			} else if(Set_contains(fullyAssociativeCache, fullTag, store)) {
				classification = "conflict";
			} else {
				classification = "capacity";
				Set_addBlock(fullyAssociativeCache, blocks, fullTag, store);
			}

			// A set does not know its size, so set size (ways) is passed to function
			Set_addBlock(cache[index], ways, tag, store);
		}

		// Print output to file
		fprintf(fpw, "%c %s %s\n", buff[0], strAddr, classification);
	}

	/******** Cleanup ********/
	fclose(fpw);
	Cache_delete(sets);
	Set_delete(fullyAssociativeCache);
	Set_delete(infiniteFullyAssociativeCache);

	// If read was interrupted, terminate with error code
	int returnValue = 0;
	if(!feof(fp)) {
		printf("Trace file read error.\n");
		returnValue = -1;
	}
	fclose(fp);

	// Print results
	read_xactions = totalMisses;
	printf("Miss Rate: %8lf%%\n", ((double) totalMisses) / ((double) totalMisses + (double) totalHits) * 100.0);
	printf("Read Transactions: %d\n", read_xactions);
	printf("Write Transactions: %d\n", write_xactions);

	return returnValue;
}

void initialize(int sets)
{
	// Create new associative cache with given number of sets
	cache = (Set**)malloc(sizeof(Set*) * sets);
	int i;
	for(i=0; i<sets; i++)
		cache[i] = Set_new();

	// Create new fully associative cache - a single set (FIFO queue) of blocks
	fullyAssociativeCache = Set_new();
	infiniteFullyAssociativeCache = Set_new();
}

// Retrieve raw index from address
uint32_t getIndex(uint32_t addr, int tagSize, int offsetSize)
{
	return (addr << tagSize) >> (offsetSize + tagSize);
}

// Retrieve tag from address
uint32_t getTag(uint32_t addr, int indexSize, int offsetSize)
{
	return addr >> (indexSize + offsetSize);
}

// Return whether a set contains the given tag
// Dirty bit toggled if store is true
bool Set_contains(Set* set, uint32_t tag, bool store)
{
	Block* temp = set->front;
	// Iterate through blocks in set until tag found
	while(temp != NULL) {
		if(temp->tag == tag) {
			if(store)
				temp->dirty = true;
			return true;
		}
		temp = temp->next;
	}
	// If tag not found return false
	return false;
}

// Create a new block
Block* Block_new(uint32_t tag, bool dirty)
{
	Block* block = (Block*)malloc(sizeof(Block));
	if(block == NULL)
		abort();
	block->dirty = dirty;
	block->tag = tag;
	block->next = NULL;
	return block;
}

// Create empty set
Set* Set_new()
{
	Set* set = (Set*)malloc(sizeof(Set));
	if(set == NULL)
		abort();
	set->front = NULL;
	set->back = NULL;
	set->size = 0;
	return set;
}

/*
 * This function adds a new block with given tag to provided set.
 * The store boolean determines the dirty bit of the block.
 * The setSize argument is passed in and is used to determine whether or not
 * a replacement is necessary. A set does not know its maximum size, so it is
 * passed in with the function call.
 */
void Set_addBlock(Set* set, int setSize, uint32_t tag, bool store)
{
	Block* newBlock = Block_new(tag, store);
	if(!set->size) {
		// Set is empty -> increment size and add front block
		set->size++;
		set->front = newBlock;
		set->back = set->front;
	} else if(set->size == setSize) {
		// Set is full -> get rid of first element of set
		// TODO This check can be moved outside of method to decrease runtime?
		if(set->front->dirty && set != fullyAssociativeCache && set != infiniteFullyAssociativeCache)
			write_xactions++;
		
		if(setSize == 1) {
			// Direct-mapped cache - replace only element
			free(set->front);
			set->front = newBlock;
		} else {
			// Save next before free
			Block* temp = set->front->next;
			free(set->front);

			// Replace front with next
			set->front = temp;
			set->back->next = newBlock;
		}
		// Add new block to end
		set->back = newBlock;
	} else {
		// Increment size and concatenate block to end of set
		set->size++;
		set->back->next = newBlock;
		set->back = newBlock;
	}
}

// Clean up the cache
void Cache_delete(int sets)
{
	// Iterate through all sets in cache deleting one-by-one
	if(cache != NULL) {
		// The else condition should never arise
		int i;
		for(i=0; i<sets; i++)
			Set_delete(cache[i]);

		// Free allocated cache
		free(cache);
	}
}

// Clean up set, also used for the fully associative cache
void Set_delete(Set* set)
{
	if(set != NULL) {
		// Else condition should never arise
		Block* block = set->front;
		// Iterate through blocks in set and delete
		while(block != NULL) {
			Block* temp = block->next;
			free(block);
			block = temp;
		}
		// Free allocated set
		free(set);
	}
}
