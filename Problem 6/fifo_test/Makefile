
acid_test: sem.o tas64.o fifo.o acid_test.c
	gcc -g -o acid_test sem.o tas64.o fifo.o acid_test.c

basic_test: sem.o tas64.o fifo.o basic_test.c
	gcc -g -o basic_test sem.o tas64.o fifo.o basic_test.c

sem.o: ../semaphore/sem.c ../semaphore/sem.h ../nproc.h
	gcc -c ../semaphore/sem.c ../semaphore/sem.h ../nproc.h

tas64.o: ../tas64.S ../tas.h
	gcc -c ../tas64.S ../tas.h

fifo.o: ../fifo/fifo.h ../fifo/fifo.c
	gcc -c ../fifo/fifo.h ../fifo/fifo.c

clean:
	rm *.gch *.o
