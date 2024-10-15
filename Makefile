CC = gcc
CFLAGS = -Wall -ansi -pedantic


all: hencode hdecode


hencode: hencode.o huffman.o
	$(CC) $(CFLAGS) $^ -o $@

hencode.o: hencode.c hencode.h
	$(CC) $(CFLAGS) -c $^

huffman.o: huffman.c huffman.h
	$(CC) $(CFLAGS) -c $^

hdecode: hdecode.o huffman.o
	$(CC) $(CFLAGS) $^ -o $@

hdecode.o: hdecode.c hdecode.h
	$(CC) $(CFLAGS) -c $^



test: hencode
	make
	./hencode test.txt test
	./hdecode test test_decoded.txt
	if diff test.txt test_decoded.txt >/dev/null; then \
		echo "Test passed: The files are identical."; \
	else \
		echo "Test failed: The files are different."; \
	fi

clean:
	rm -f hencode hdecode test test_decoded.txt *.o *.out *.gch
