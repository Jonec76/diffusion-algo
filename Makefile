CFLAGS = -g -Wall
CFLAGS += -O2

all: diffusion.o
	g++ -o main main.cpp diffusion.o diff_func.o $(CFLAGS) -std=c++11

diffusion.o:diffusion.cpp diff_func.o
	g++ -c diffusion.cpp diff_func.o $(CFLAGS) -std=c++11

diff_func.o:diff_func.cpp
	g++ -c diff_func.cpp $(CFLAGS) -std=c++11

.PHONY : clean
clean:
	rm *.o main