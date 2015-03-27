WARNING=-Wall #\
#		-Wcast-align -Wsign-compare -Wpacked -Werror \
#		-Winline -Wunused-variable -Wunused-value -Wunused-function \
#		-Wextra -Wshadow -Wpointer-arith -Wcast-qual -Wundef \
#		-Wredundant-decls

OPTIMIZE=-O0
DEBUG=-g3
COMPILE=-c
INCLUDE=-Icommon/src -Itest/src/ArrayTest -Itest/src/NetworkTest -Inetwork/src
PREPROCESSOR=-D_LINUX=1
INLINE=-finline-functions
CFLAGS=-pthread -fno-rtti -Wreorder -Woverloaded-virtual -ansi
PLATFORM=-m32
CC=gcc
LIB=common/Common.a
CFLAGS_END=-lrt -lstdc++

arraytest:
	(cd common/;make)
	$(CC) $(WARNING) $(CFLAGS) $(OPTMIZE) $(DEBUG) $(INCLUDE) $(PREPROCESSOR) $(INLINE) -o arraytest \
		test/src/ArrayTest/main.cpp test/src/ArrayTest/allocator_test.cpp test/src/ArrayTest/array_test.cpp test/src/ArrayTest/string_test.cpp $(LIB) $(CFLAGS_END)

networktest:
	(cd common/;make) 
	(cd network/;make)
	$(CC) $(WARNING) $(CFLAGS) $(OPTMIZE) $(DEBUG) $(INCLUDE) $(PREPROCESSOR) $(INLINE) -o networktest \
		test/src/NetworkTest/main.cpp network/Network.a $(LIB) $(CFLAGS_END)
			
clean:
	rm -f arraytest
	rm -f networktest
	rm -f main
	rm -f common/*.a
	rm -f common/*.o
	rm -f network/*.a
	rm -f network/*.o
