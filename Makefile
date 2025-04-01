CC = g++
CFLAGS = -Wall -ggdb3 -std=c++11 -lcurl -lpthread

main:  main.o Stock.o DataFetcher.o Matrix.o Group.o
	$(CC) $(CFLAGS) -o main  main.o Stock.o DataFetcher.o Matrix.o Group.o

main.o: main.cpp Stock.h DataFetcher.h Matrix.h Group.h
	$(CC) $(CFLAGS) -c main.cpp

Stock.o: Stock.h Stock.cpp Matrix.h
	$(CC) $(CFLAGS) -c Stock.cpp

DataFetcher.o: DataFetcher.h DataFetcher.cpp Stock.h 
	$(CC) $(CFLAGS) -c DataFetcher.cpp

Matrix.o: Matrix.h Matrix.cpp
	$(CC) $(CFLAGS) -c Matrix.cpp

Group.o:Group.h Group.cpp Matrix.h Stock.h DataFetcher.h
	$(CC) $(CFLAGS) -c Group.cpp

clean:
	rm -rf  main  *.o