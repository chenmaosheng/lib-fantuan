WARNING=-Wall
OPTIMIZE=-O0
DEBUG=-g3
COMPILE=-c
INCLUDE=-Isrc -I../common/src -I../network/src
PREPROCESSOR=-D_LINUX=1
INLINE=-finline-functions
CFLAGS=-pthread -fno-rtti -Wreorder -Woverloaded-virtual -ansi
PLATFORM=-m32
CC=gcc
CFLAG_END=-lrt -lstdc++ -std=c++0x

Midware.a:
	$(CC) $(WARNING) $(CFLAGS) $(OPTIMIZE) $(DEBUG) $(INCLUDE) $(PREPROCESSOR) $(INLINE) -o easy_baseserver.o -c src/easy_baseserver.cpp $(CFLAG_END)
	
	$(CC) $(WARNING) $(CFLAGS) $(OPTIMIZE) $(DEBUG) $(INCLUDE) $(PREPROCESSOR) $(INLINE) -o easy_command.o -c src/easy_command.cpp $(CFLAG_END)
	
	$(CC) $(WARNING) $(CFLAGS) $(OPTIMIZE) $(DEBUG) $(INCLUDE) $(PREPROCESSOR) $(INLINE) -o easy_dispatcher.o -c src/easy_dispatcher.cpp $(CFLAG_END)
	
	$(CC) $(WARNING) $(CFLAGS) $(OPTIMIZE) $(DEBUG) $(INCLUDE) $(PREPROCESSOR) $(INLINE) -o easy_baseloop.o -c src/easy_baseloop.cpp $(CFLAG_END)
	
	$(CC) $(WARNING) $(CFLAGS) $(OPTIMIZE) $(DEBUG) $(INCLUDE) $(PREPROCESSOR) $(INLINE) -o easy_session.o -c src/easy_session.cpp $(CFLAG_END)

	ar crv Midware.a easy_baseserver.o easy_command.o easy_dispatcher.o easy_baseloop.o easy_session.o

clean:
	rm *.o