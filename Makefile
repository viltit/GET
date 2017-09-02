OUT = test

SRC = $(wildcard *.cpp)

#DEP =

#INC =

CC = g++
CCFLAGS = -std=c++17

$(OUT) : $(SRC)
	$(CC) $(CCFLAGS) $(SRC) -o $@
