CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
TARGET   = tictactoe
SRCS     = main.cpp User.cpp Board.cpp

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS)

clean:
	rm -f $(TARGET) players.dat history.dat

run: all
	./$(TARGET)

.PHONY: all clean run
