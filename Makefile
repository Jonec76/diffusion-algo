CFLAGS = -B -g -Wall 
CFLAGS += -O2
CC = g++
.PHONY : clean

OBJS := init.o diff_func.o diffusion.o graph.o algo.o
TARGETS := greedy baseline

all:$(TARGETS)
$(TARGETS): $(OBJS) greedy.cpp baseline.cpp
	g++ -o $@ $@.cpp $(OBJS) $(CFLAGS) -std=c++11
%.o: %.cpp %.h
	$(CC) -o $@ -c $< $(CFLAGS) -std=c++11
clean:
	rm *.o greedy baseline