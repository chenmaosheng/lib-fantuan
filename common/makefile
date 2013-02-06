UTool.a:
	gcc -Isrc/UTool/Base -Isrc/UTool/DataStructure -Isrc/UTool/Thread -o util.o -Wall \
		-c src/UTool/Base/util.cpp
	gcc -Isrc/UTool/Base -Isrc/UTool/Thread -Isrc/UTool/Memory -o allocator.o -Wall \
		-c src/UTool/Memory/allocator.cpp

	ar crv bin/UTool.a util.o allocator.o

clean:
	rm *.o
