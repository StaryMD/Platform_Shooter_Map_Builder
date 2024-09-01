CFLAGS = -Ofast -std=c++20 -I include -lsfml-window -lsfml-system -lsfml-graphics

# don't touch or I'll steal your kneecaps
SRCS = $(wildcard src/*.cpp)
OBJS = $(SRCS:.cpp=.o)
VPATH = src

# black magic
%.o: %.cpp
	g++ -c -o $@ $< $(CFLAGS)

runme: $(OBJS)
	g++ -o $@ $^ $(CFLAGS)

.PHONY: clean

clear:
	rm -f src/*.o

clean:
	rm -f src/*.o
	rm -f runme
