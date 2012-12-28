WARNING=-Wall -Wextra -Winline -Wshadow -Wcast-qual -Wpointer-arith -Waggregate-return -Wundef -Wredundant-decls -Wcast-align -Wsign-compare -Wpacked -Wpadded -Werror
OPTIMIZE=-O0
DEBUG=-g3
COMPILE=-c
INCLUDE=-ICommon/Src/Base -ICommon/Src/DataStructure -ICommon/Src/Thread
PREPROCESSOR=-D_LINUX=1
INLINE=-finline-functions
CFLAGS=-lstdc++ -fno-rtti -Wreorder -Woverloaded-virtual -ansi
PLATFORM=-m32
CC=gcc
LIB=Common/Lib/mylib.a

main: main.cpp
	(cd Common/Src/Base;make)
	$(CC) $(WARNING) $(CFLAGS) $(OPTMIZE) $(DEBUG) $(INCLUDE) $(PREPROCESSOR) $(INLINE) $(PLATFORM) -o main main.cpp $(LIB)

clean:
	rm main
	rm Common/Lib/*.*
