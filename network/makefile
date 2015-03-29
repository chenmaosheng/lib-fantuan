WARNING=-Wall
OPTIMIZE=-O0
DEBUG=-g3
COMPILE=-c
INCLUDE=-Isrc -I../common/src
PREPROCESSOR=-D_LINUX=1
INLINE=-finline-functions
CFLAGS=-pthread -fno-rtti -Wreorder -Woverloaded-virtual -ansi
PLATFORM=-m32
CC=gcc
CFLAG_END=-lrt -lstdc++ -std=c++0x

Network.a:
	$(CC) $(WARNING) $(CFLAGS) $(OPTIMIZE) $(DEBUG) $(INCLUDE) $(PREPROCESSOR) $(INLINE) -o easy_connection.o -c src/easy_connection.cpp $(CFLAG_END)
	
	$(CC) $(WARNING) $(CFLAGS) $(OPTIMIZE) $(DEBUG) $(INCLUDE) $(PREPROCESSOR) $(INLINE) -o easy_acceptor.o -c src/easy_acceptor.cpp $(CFLAG_END)
	
	$(CC) $(WARNING) $(CFLAGS) $(OPTIMIZE) $(DEBUG) $(INCLUDE) $(PREPROCESSOR) $(INLINE) -o easy_worker.o -c src/easy_worker.cpp $(CFLAG_END)

	ar crv Network.a easy_connection.o easy_acceptor.o easy_worker.o

clean:
	rm *.o
