.PHONY: all clean

case = testcase/case7.txt

# Compiler settings
CXX = g++
CXXFLAGS = -I ./inc -std=c++11
WARNINGS = -g -Wall

# Source files and object files
SRCDIR = src
OBJDIR = obj
INCDIR = inc
SRCS = $(wildcard $(SRCDIR)/*.cpp)
OBJS = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRCS))


# Name of the executable
TARGET = Lab1

all: $(TARGET)

$(OBJDIR):
	@mkdir $(OBJDIR)

$(TARGET): main.cpp $(OBJS)
	$(CXX) $(WARNINGS) $(CXXFLAGS) $^ -o $@

# Compilation rule for object files with automatic dependency generation
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR) Makefile
	$(CXX) $(WARNINGS) $(CXXFLAGS) -c $< -o $@

test0:
	./$(TARGET) testcase/case0.txt out.txt

test1:
	./$(TARGET) testcase/case1.txt out.txt

test2:
	./$(TARGET) testcase/case2.txt out.txt

test7:
	./$(TARGET) testcase/case7.txt out.txt

py:
	python3 draw_block_layout.py out.txt out.png

# Clean rule
clean:
	rm -rf $(OBJDIR)/*.o $(TARGET)
