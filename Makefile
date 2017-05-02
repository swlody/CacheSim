FILES := src/cache.c

cacheSim: $(FILES)
	gcc -o $@ $^ -O3 -lm -Wall

cacheSimDebug: $(FILES)
	gcc -o $@ $^ -g -lm -Wall

clean:
	rm -f cacheSimDebug
	rm -f cacheSim

.PHONY : clean
