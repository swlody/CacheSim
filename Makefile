FILES := src/cache.c src/Queue.c

cacheSim: $(FILES)
	gcc -o $@ $^ -O3 -lm -Wall

cacheSimDebug: $(FILES)
	gcc -o $@ $^ -g -lm -Wall

clean:
	rm -rf cacheSimDebug.dSYM
	rm -f cacheSimDebug
	rm -f cacheSim

.PHONY : clean
