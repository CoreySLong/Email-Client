# Make environment for CMSC257 Project3

my257sh :
	gcc -pthread -Wall shellex.c csapp.c -o sh257

clean : 
	rm -v sh257
