flag = -Wall
complier = gcc -std=c99
deps = check transferclient transferserver transfer.o gram.o sha1.o 
justcomp = -c
# mysqldeps = -I/usr/local/mysql/include/ -L/usr/local/mysql/lib/ -lmysqlclient

all:transferclient transferserver check

check:check.c transfer.o gram.o sha1.o 
	${complier} ${flag} check.c -o check transfer.o gram.o sha1.o 
transferclient:client.c transfer.o gram.o sha1.o
	${complier} ${flag} client.c -o transferclient transfer.o gram.o sha1.o
transferserver:server.c transfer.o gram.o sha1.o
	${complier} ${flag} server.c -o transferserver transfer.o gram.o sha1.o
transfer.o:transfer.c transfer.h
	${complier} ${justcomp} ${flag} transfer.c -o transfer.o
gram.o:gram.h transfer.h sha1.o
	${complier} ${justcomp} ${flag} gram.c -o gram.o
sha1.o:sha1.c sha1.h
	${complier} ${justcomp} ${flag} sha1.c -o sha1.o
#database.o:database.c database.h
#	${complier} ${justcomp} ${flag} database.c -o database.o
clean:
	-rm -Rf ${deps}
