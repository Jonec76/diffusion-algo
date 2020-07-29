CFLAGS = -g -Wall
CFLAGS += -O2

all: diffusion.cpp func.cpp
	g++ -o main diffusion.cpp func.cpp $(CFLAGS) -std=c++11

diffusion:
	g++ -c diffusion.cpp $(CFLAGS) -std=c++11

.PHONY : clean
clean:
	rm *.o diffusion main