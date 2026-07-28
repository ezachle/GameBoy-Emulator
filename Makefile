CXX = g++
INCL = include
FLAGS = -Wall -Werror --std=c++20 -I $(INCL)
SRC = $(wildcard src/*.cpp)
SRC_O = $(SRC:.cpp=.o)
TARGET = GameBoy

.PHONY: all $(TARGET) clean DEBUG

all: $(TARGET)

DEBUG: FLAGS += -g -DDEBUG
DEBUG: $(TARGET)

$(TARGET): $(SRC_O)
	$(CXX) $(FLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(FLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(SRC_O)
