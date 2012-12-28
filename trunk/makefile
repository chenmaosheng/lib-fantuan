WARNING=-Wall -Wextra -Winline -Wshadow -Wcast-qual -Wpointer-arith -Waggregate-return -Wundef -Wredundant-decls -Wcast-align -Wsign-compare -Wpacked -Wpadded -Werror
OPTIMIZE=-O0
DEBUG=-g3
COMPILE=-c
INCLUDE=-Icommon/src/UTool/Base -Icommon/src/UTool/DataStructure -Icommon/src/UTool/Thread
PREPROCESSOR=-D_LINUX=1
INLINE=-finline-functions
CFLAGS=-lstdc++ -fno-rtti -Wreorder -Woverloaded-virtual -ansi
PLATFORM=-m32
CC=gcc
LIB=common/bin/UTool.a

main: test/main.cpp
	(cd common/;make)
	$(CC) $(WARNING) $(CFLAGS) $(OPTMIZE) $(DEBUG) $(INCLUDE) $(PREPROCESSOR) $(INLINE) $(PLATFORM) -o main test/main.cpp $(LIB)

clean:
	rm main
	rm common/bin/*.*
