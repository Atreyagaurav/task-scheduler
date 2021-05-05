CC = gcc
CFLAGS = -Wall
LIBS = 
PLUGFLAGS = 
OBJ = task.o task_scope.o
CONFIG =

all: task task_scope

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) $(LIBS) -c -o $@ $<

clean:
	-rm *.o task

task: task.o
	gcc $(CFLAGS) -o $@ $^

task_scope: task_scope.o
	gcc $(CFLAGS) -o $@ $^

install:
	-sudo cp task /usr/local/bin/
	-sudo cp task_scope /usr/local/bin/
	-sudo mkdir -p /etc/task
	-sudo cp task.sch /etc/task

uninstall:
	-sudo rm /usr/local/bin/task
	-sudo rm /usr/local/bin/task_scope
	-sudo rm -r /etc/task
