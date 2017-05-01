/*
 * Ari Geller & Sam Wlody
 * CSC 252 - Project 4
 */

#include "cache.h"
#include "Queue.h"
#include <stdbool.h>
#include <math.h>

int write_xactions = 0;
int read_xactions = 0;
Cache* mappedCache;
Cache* fullyAssociativeCache;
Set** cacheMap; // lol

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

uint32_t getIndex(uint32_t addr, int tagSize, int offsetSize)
{
	return (addr << tagSize) >> (offsetSize + tagSize);
}

uint32_t getTag(uint32_t addr, int indexSize, int offsetSize)
{
	return addr >> (indexSize + offsetSize);
}

bool FoundInSet(Set* set, int tag, bool store)
{
	Block* temp = set->first;
	while(temp != NULL) {
		if(temp->tag == tag) {
			if(store)
				temp->dirty = 1;
			return true;
		}
		temp = temp->next;
	}
	return false;
}

int main(int argc, char* argv[])
{
	int i;
	uint32_t size = 32; //total size of L1$ (KB)
	uint32_t ways = 1; //# of ways in L1. Default to direct-mapped
	uint32_t line = 32; //line size (B)

	// hit and miss counts
	int totalHits = 0;
	int totalMisses = 0;

	char * filename = "";

	//strings to compare
	const char helpString[] = "-h";
	const char sizeString[] = "-s";
	const char waysString[] = "-w";
	const char lineString[] = "-l";
	const char traceString[] = "-t";

	if(argc == 1) {
		// No arguments passed, show help
		printHelp(argv[0]);
		return 1;
	}

	//parse command line
	for(i=1; i<argc; i++) {
		//check for help
		if(!strcmp(helpString, argv[i])) {
			//print out help text and terminate
			printHelp(argv[0]);
			return 1; //return 1 for help termination
		} else if(!strcmp(sizeString, argv[i])) {
			//take next string and convert to int
			i++; //increment i so that it skips data string in the next loop iteration
			//check next string's first char. If not digit, fail
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
			//take next string and convert to int
			i++; //increment i so that it skips data string in the next loop iteration
			//check next string's first char. If not digit, fail
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

	// Calculations
	int sets = (size * 1024) / (line * ways);
	int offsetSize = log2((double) line);
	int indexSize = log2((double) sets);
	int tagSize = 32 - (indexSize + offsetSize);
	int numBlocks = ways * sets;
	// Initialzie the cache
	cache_init(ways, sets);

	// Print info
	printf("Ways: %u; Sets: %u; Line Size: %uB\n", ways, sets, line);
	printf("Tag: %d bits; Index: %d bits; Offset: %d bits\n", tagSize, indexSize, offsetSize);

	// Open trace file
	FILE* fp = fopen(filename, "r");
	strcat(filename, ".simulated");
	FILE* fpw = fopen(filename, "w");
	char buff[14];
	// Until end of trace file...
	while(fgets(buff, 14, fp) != NULL) {
		// Get load or store from line
		bool store = buff[0] == 's';
		char strAddr[11];
		// Get address in string form from line
		memcpy(strAddr, buff+2, 10);
		strAddr[10] = '\0';
		// Convert hex string to integer representation
		uint32_t addr = (uint32_t)strtol(strAddr, NULL, 0);
		uint32_t index = getIndex(addr, tagSize, offsetSize);
		uint32_t tag = getTag(addr, indexSize, offsetSize);
		char* classification;

		if(FoundInSet(cacheMap[index], tag, store)) {
			totalHits++;
			classification = "hit";
		} else {
			totalMisses++;
			Block* newBlock = Block_new();
			newBlock->dirty = store;
			newBlock->tag = tag;
			Set_addBlock(cacheMap[index], newBlock, ways);
			if(ways > cacheMap[index]->size) {
				classification = "compulsory";
				Set_addBlock(fullyAssociativeCache->first, newBlock, numBlocks);
			} else if(FoundInSet(fullyAssociativeCache->first, tag, store)) {
				classification = "conflict";
			} else {
				classification = "capacity";
				Set_addBlock(fullyAssociativeCache->first, newBlock, numBlocks);
			}
		}
		fprintf(fpw, "%c %s %s\n", buff[0], strAddr, classification);
	}
	// Cleanup
	fclose(fpw);
	Cache_delete(mappedCache);
	Cache_delete(fullyAssociativeCache);
	// If read interrupted, terminate with error code
	int returnValue = 0;
	if(!feof(fp)) {
		printf("Trace file read error.\n");
		returnValue = -1;;
	}
	fclose(fp);

	// Print results
	printf("Miss Rate: %8lf%%\n", ((double) totalMisses) / ((double) totalMisses + (double) totalHits) * 100.0);
	printf("Read Transactions: %d\n", read_xactions);
	printf("Write Transactions: %d\n", write_xactions);

	return returnValue;
}

void cache_init(int setSize, int sets)
{
	// Create new associative cache with given number of sets containing setSize number of blocks
	cacheMap = (Set**)malloc(sizeof(Set)*(sets+1));
	mappedCache = Cache_new(setSize);
	int i;
	for(i=0; i<sets; i++) {
		Cache_addSet(mappedCache, i);
	}
	// Create new fully associative cache with one set of blocks
	fullyAssociativeCache = Cache_new(setSize * sets);
	Cache_addSet(fullyAssociativeCache, ++i);
}

Block* Block_new()
{
	// Create a new block with valid and dirty initialized to false and no tag
	Block* newBlock = (Block*)malloc(sizeof(Block));
	newBlock->dirty = false;
	newBlock->tag = 0;
	return newBlock;
}

bool Set_isEmpty(Set* set)
{
	// Set does not yet contain any blocks
	return set->first == NULL;
}

void Set_addBlock(Set* set, Block* newBlock, int setSize)
{
	if(Set_isEmpty(set)) {
		set->size++;
		set->first = newBlock;
		set->last = set->first;
	} else if(set->size == setSize) {
		// Set is full
		Block* temp = set->first->next;
		write_xactions += set->first->dirty;
		free(set->first);
		set->first = temp;
		set->last->next = newBlock;
	} else {
		set->size++;
		Block* temp = set->first;
		while(temp->next != NULL) {
			temp = temp->next;
		}
		temp->next = newBlock;
	}
}

Set* Set_new(int setSize)
{
	// Create a new set with setSize number of blocks
	Set* newSet = (Set*)malloc(sizeof(Set));
	// next is initially null
	newSet->next = NULL;
	newSet->size = 0;
	return newSet;
}

Cache* Cache_new(int setSize)
{
	// Initialize the cache with given setSize (ways)
	Cache* cache = (Cache*)malloc(sizeof(Cache));
	// First and last initially null
	cache->first = NULL;
	cache->last = NULL;
	cache->setSize = setSize;
	return cache;
}

bool Cache_isEmpty(Cache* cache)
{
	// This only happens once
	return cache->first == NULL;
}

void Cache_addSet(Cache* cache, int index)
{
	// Add a new set with the given number of blocks
	Set* newSet = Set_new(cache->setSize);
	if(Cache_isEmpty(cache)) {
		// This only happens once
		cache->first = newSet;
		cache->last = cache->first;
	} else {
		cache->last->next = newSet;
	}
	cacheMap[index] = newSet; // Totally unnecessary for fully-associative cache but whatever
}

void Cache_delete(Cache* cache)
{
	if(Cache_isEmpty(cache)) {
		// This should never happen
		free(cache);
	} else {
		// Get the first set in the cache
		Set* set = cache->first;
		// Until all sets have been read...
		while(set != NULL) {
			Set* newSet = set->next;
			// Iterate through the blocks in the set
			Block* block = set->first;
			while(block != NULL) {
				Block* newBlock = block->next;
				// Free the block
				free(block);
				block = newBlock;
			}
			// Free the set
			free(set);
			set = newSet;
		}
		// Free the cache struct
		free(cache);
	}
}

