CC = g++
CFLAGS = -std=c++11 -pthread

all: main

main: main.cpp
	$(CC) $(CFLAGS) -o main main.cpp -g -Wall && ./main && rm -f main

clean:
	rm -f main
