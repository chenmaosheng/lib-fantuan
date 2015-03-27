WARNING=-Wall
OPTIMIZE=-O0
DEBUG=-g3
COMPILE=-c
INCLUDE=-Isrc
PREPROCESSOR=-D_LINUX=1
INLINE=-finline-functions
CFLAGS=-pthread -fno-rtti -Wreorder -Woverloaded-virtual -ansi
PLATFORM=-m32
CC=gcc
CFLAG_END=-lrt -lstdc++


Common.a:
	$(CC) $(WARNING) $(CFLAGS) $(OPTIMIZE) $(DEBUG) $(INCLUDE) $(PREPROCESSOR) $(INLINE) -o util.o -c src/util.cpp $(CFLAG_END)
	$(CC) $(WARNING) $(CFLAGS) $(OPTIMIZE) $(DEBUG) $(INCLUDE) $(PREPROCESSOR) $(INLINE) -o allocator.o -c src/allocator.cpp $(CFLAG_END)
	ar crv Common.a util.o allocator.o

clean:
	rm *.o
