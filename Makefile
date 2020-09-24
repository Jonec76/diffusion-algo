CFLAGS = -g -Wall -B
CFLAGS += -O2

.PHONY : *.o

all: diff_func.o diffusion.o graph.o greedy.o
	g++ -o main main.cpp diff_func.o diffusion.o graph.o $(CFLAGS) -std=c++11

graph.o:
	g++ -c graph.cpp $(CFLAGS) -std=c++11

diffusion.o:diffusion.cpp diff_func.o
	g++ -c diffusion.cpp diff_func.o $(CFLAGS) -std=c++11

diff_func.o:diff_func.cpp diff_func.h 
	g++ -c diff_func.cpp $(CFLAGS) -std=c++11

greedy.o:
	g++ -c greedy.cpp $(CFLAGS) -std=c++11
	
greedy_func.o:
	g++ -c greedy_func.cpp $(CFLAGS) -std=c++11
clean:
	rm *.o main