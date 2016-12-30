test: test.c sem.o tas64.o
	gcc -g -o test test.c sem.o tas64.o

tas64.o: ../tas.h ../tas64.S
	gcc -c ../tas.h ../tas64.S

sem.o: sem.c sem.h ../nproc.h
	gcc -c sem.c sem.h ../nproc.h

clean:
	rm *.gch *.o
