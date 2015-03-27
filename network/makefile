Network.a:
	gcc -Isrc -I../common/src -pthread -D_LINUX=1 -o easy_connection.o -Wall -c src/easy_connection.cpp -lstdc++
	gcc -Isrc -I../common/src -pthread -D_LINUX=1 -o easy_acceptor.o -Wall -c src/easy_acceptor.cpp -lstdc++
	gcc -Isrc -I../common/src -pthread -D_LINUX=1 -o easy_worker.o -Wall -c src/easy_worker.cpp -lstdc++

	ar crv Network.a easy_connection.o easy_acceptor.o easy_worker.o

clean:
	rm *.o