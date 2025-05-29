CC = g++
CFLAGS = -std=c++11 -pthread

all: main

main: main.cpp
	$(CC) $(CFLAGS) -o main main.cpp -g -Wall && ./main && rm -f main

hw3: hw3_ReadWriteLock_problem_sol_20191564.cpp
	$(CC) $(CFLAGS) -o hw3 hw3_ReadWriteLock_problem_sol_20191564.cpp -g -Wall && ./hw3 && rm -f hw3

clean:
	rm -f main
