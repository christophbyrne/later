CC = g++
all: main.cc util.cc
	$(CC) -o later.o main.cc util.cc

clean: *.o
	rm -rf test/* ./*.o