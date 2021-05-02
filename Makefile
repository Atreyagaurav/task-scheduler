CC = gcc
CFLAGS = -Wall
LIBS = 
PLUGFLAGS = 
OBJ = tst.o scope.o
CONFIG =

all: tst scope

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) $(LIBS) -c -o $@ $<

clean:
	-rm *.o *.so tst

tst: tst.o
	gcc $(CFLAGS) -o $@ $^

scope: scope.o
	gcc $(CFLAGS) -o $@ $^

install:
	-sudo cp tst /usr/local/bin/

uninstall:
	-sudo rm /usr/local/bin/tst
