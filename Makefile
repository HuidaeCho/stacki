all:	stacki unstacki

clean:
	rm -f stacki unstacki

stacki:	stacki.c
	$(CC) -o stacki stacki.c -Wall

unstacki: unstacki.c
	$(CC) -o unstacki unstacki.c -Wall
