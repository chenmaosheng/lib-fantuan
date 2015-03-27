Common.a:
	gcc -Isrc -o util.o -Wall -c src/util.cpp
	gcc -Isrc -o allocator.o -Wall -c src/allocator.cpp

	ar crv Common.a util.o allocator.o

clean:
	rm *.o
