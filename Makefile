CFLAGS = -B -g -Wall -std=c++11
CFLAGS += -O2
CC = g++
.PHONY : clean

OBJS := init.o diff_func.o diffusion_algo.o graph.o algo.o mipc.o
TARGETS := main greedy diffusion opt

all:$(TARGETS)
$(TARGETS): $(OBJS) main.cpp greedy.cpp mipc.cpp diffusion.cpp opt.cpp
	g++ -o $@ $@.cpp $(OBJS) $(CFLAGS) 
%.o: %.cpp *.h
	$(CC) -o $@ -c $< $(CFLAGS)
clean:
	rm *.o main greedy mipc diffusion opt
