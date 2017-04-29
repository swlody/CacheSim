#include "cache.h"
#include "trace.h"
#include "LinkedList.h"
#include "Queue.h"

int write_xactions = 0;
int read_xactions = 0;
LinkedList* cache;
Queue* cacheQueue;

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
uint64_t 128to64(__uint128_t i)
{
	return (uint64_t) i;
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

	char * filename;

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
				return -1; //input failure
			}
		} else if(!strcmp(traceString, argv[i])) {
			filename = argv[++i];
		} else {
			printf("Unrecognized argument. Exiting.\n");
			return -1;
		}
	}

	int sets = (size * 1024) / (line * ways);
	int offsetSize = log2((double) line);
	int indexSize = log2((double) setSize);
	int tagSize = 32 - (indexSize - offsetSize);
	cache_init(ways, sets);

	printf("Ways: %u; Sets: %u; Line Size: %uB\n", ways, sets, line);
	printf("Tag: %d bits; Index: %d bits; Offset: %d bits\n", tagSize, indexSize, offsetSize);

	FILE* fp = fopen(filename, "r");
	char[12] buff;
	while(fgets(buff, 12, fp) != NULL) {
		// TODO do whatever with buff
	}
	// If read interrupted end with error
	if(!feof(fp))
		return -1;

	// TODO Simulate cache

	// Print results
	printf("Miss Rate: %8lf%%\n", ((double) totalMisses) / ((double) totalMisses + (double) totalHits) * 100.0);
	printf("Read Transactions: %d\n", read_xactions);
	printf("Write Transactions: %d\n", write_xactions);

	// TODO Output file

	// TODO Cleanup
}

void cache_init(int setSize, int sets)
{
	cache = LinkedList_new(setSize);
	int i;
	for(i=0; i<sets; i++) {
		LinkedList_add(cache);
	}
}
