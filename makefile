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
CFLAGS=-fno-rtti -Wreorder -Woverloaded-virtual -ansi
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
	$(CC) $(WARNING) $(CFLAGS) $(OPTMIZE) $(DEBUG) $(INCLUDE) $(PREPROCESSOR) $(INLINE) -o networktest \
		test/src/NetworkTest/main.cpp network/Network.a $(LIB) $(CFLAGS_END)
			
main: test/src/ArrayTest/main.cpp
	(cd common/;make)
	$(CC) $(WARNING) $(CFLAGS) $(OPTMIZE) $(DEBUG) $(INCLUDE) $(PREPROCESSOR) $(INLINE) -o main \
		test/src/ArrayTest/main.cpp test/src/ArrayTest/allocator_test.cpp test/src/ArrayTest/array_test.cpp test/src/ArrayTest/string_test.cpp \
		#test/src/NetworkTest/main.cpp network/Network.a
		$(LIB) $(CFLAGS_END)

clean:
	rm main
	rm common/*.a
	rm common/*.o
	rm network/*.a
	rm network/*.o
