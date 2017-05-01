/*
 * Ari Geller & Sam Wlody
 * CSC 252 - Project 4
 */

#include "cache.h"

int write_xactions = 0;
int read_xactions = 0;
Set* fullyAssociativeCache;
Set** cache; //cacheMap

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
	uint32_t size = 32; //total size of L1$ (KB)
	uint32_t ways = 1; //# of ways in L1. Default to direct-mapped
	uint32_t line = 32; //line size (B)

	// hit and miss counts
	int totalHits = 0;
	int totalMisses = 0;

	char* filename = "";

	//strings to compare
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
	initialize(sets);

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
		// Retrieve index and tag
		// TODO Is this right?
		int index = getIndex(addr, tagSize, offsetSize) % sets;
		uint32_t tag = getTag(addr, indexSize, offsetSize);
		char* classification;

		// Check if value is found in set based on tag
		if(FoundInSet(cache[index], tag, store)) {
			totalHits++;
			classification = "hit";
		} else {
			// Miss
			totalMisses++;
			Block* newBlock = Block_new(store, tag);
			Set_addBlock(cache[index], newBlock, ways);
			// Classify the miss
			// FIXME Check these
			if(FoundInSet(fullyAssociativeCache, tag, store)) {
				classification = "conflict";
			} else {
				classification = ways > cache[index]->size ? "compulsory" : "capacity";
				Set_addBlock(fullyAssociativeCache, newBlock, numBlocks);
			}
		}
		fprintf(fpw, "%c %s %s\n", buff[0], strAddr, classification);
	}
	// Cleanup
	fclose(fpw);
	Cache_delete(sets);
	Set_delete(fullyAssociativeCache);

	// If read interrupted, terminate with error code
	int returnValue = 0;
	if(!feof(fp)) {
		printf("Trace file read error.\n");
		returnValue = -1;;
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
	// Create new associative cache with given number of sets containing setSize number of blocks
	cache = (Set**)malloc(sizeof(Set)*sets);
	int i;
	for(i=0; i<sets; i++)
		cache[i] = Set_new();
	// Create new fully associative cache (single set of blocks)
	fullyAssociativeCache = Set_new();
}

uint32_t getIndex(uint32_t addr, int tagSize, int offsetSize)
{
	return ((addr << tagSize) >> (offsetSize + tagSize));
}

uint32_t getTag(uint32_t addr, int indexSize, int offsetSize)
{
	return addr >> (indexSize + offsetSize);
}

bool FoundInSet(Set* set, uint32_t tag, bool store)
{
	Block* temp = set->front;
	// Iterate through blocks in set to find tag
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

Block* Block_new(bool dirty, uint32_t tag)
{
	// Create a new block with dirty initialized to false and no tag
	Block* newBlock = (Block*)malloc(sizeof(Block));
	newBlock->dirty = dirty;
	newBlock->tag = tag;
	newBlock->next = NULL;
	return newBlock;
}

Set* Set_new()
{
	// Create empty set
	Set* newSet = (Set*)malloc(sizeof(Set));
	newSet->front = NULL;
	newSet->back = NULL;
	newSet->next = NULL;
	newSet->size = 0;
	return newSet;
}

bool Set_isEmpty(Set* set)
{
	// Set does not yet contain any blocks
	return !set->size;
}

void Set_addBlock(Set* set, Block* newBlock, int setSize)
{
	if(Set_isEmpty(set)) {
		// Increment size and add front block
		set->size++;
		set->front = newBlock;
		set->back = set->front;
	} else if(set->size == setSize) {
		// Set is full
		// Get rid of first element of set
		Block* temp = set->front->next;
		// TODO is this right?
		write_xactions += set->front->dirty;
		free(set->front);
		// Add block to end of set
		set->front = temp;
		set->back->next = newBlock;
		set->back = newBlock;
	} else {
		// Increment size and concatenate block to end of set
		set->size++;
		set->back->next = newBlock;
		set->back = newBlock;
	}
}

void Cache_delete(int sets)
{
	// Iterate through all sets in cache deleting one-by-one
	if(cache != NULL) {
		int i;
		for(i=0; i<sets; i++) {
			// TODO These next three checks might not be necessary
			if(cache[i] != NULL)
				Set_delete(cache[i]);
		}
	}
	// Free allocated cache
	// TODO
	if(cache != NULL)
		free(cache);
}

void Set_delete(Set* set)
{
	// Iterate through blocks in set and delete
	if(!Set_isEmpty(set)) {
		Block* block = set->front;
		while(block != NULL) {
			Block* temp = block->next;
			free(block);
			block = temp;
		}
	}
	// Free allocated set
	// TODO
	if(set != NULL)
		free(set);
}
