
server: main.o httpd.o includes.o
	gcc main.o httpd.o includes.o -o server

main.o: main.c httpd.h includes.h
	gcc main.c -c

httpd.o: httpd.c httpd.h
	gcc httpd.c -c

includes.o: includes.c includes.h
	gcc includes.c -c