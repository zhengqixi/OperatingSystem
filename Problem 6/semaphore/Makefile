test: test.o sem.o tas64.o
	gcc -o test test.o sem.o tas64.o

sem.o: sem.c sem.h
	gcc -c sem.c sem.h

tas64.o: ../tas64.S ../tas.h
	gcc -c ../tas64.S ../tas.h
clean:
	rm *.gch *.o
