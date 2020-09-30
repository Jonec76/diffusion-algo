CFLAGS = -B -g -Wall -std=c++11
CFLAGS += -O2
CC = g++
.PHONY : clean

OBJS := init.o diff_func.o diffusion.o graph.o algo.o
TARGETS := greedy baseline mipc

all:$(TARGETS)
$(TARGETS): $(OBJS) greedy.cpp baseline.cpp mipc.cpp
	g++ -o $@ $@.cpp $(OBJS) $(CFLAGS) 
%.o: %.cpp *.h
	$(CC) -o $@ -c $< $(CFLAGS)
clean:
	rm *.o greedy baseline mipc