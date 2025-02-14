.PHONY: all clean run verifier pic

folder0 = publicCase/testcase0
folder1 = publicCase/testcase1
folder2 = publicCase/testcase2
folder3 = publicCase/testcase_me3

case = testcase0
case0 = $(folder0)/testcase0
case1 = $(folder1)/testcase1
case2 = $(folder2)/testcase2
case3 = $(folder3)/testcase_me3

# Compiler settings
CXX = g++
CXXFLAGS = -I ./inc -std=c++11
WARNINGS = -g -Wall -O3 -flto -funroll-loops -finline-functions -ffast-math -Iinc -fno-strict-aliasing -fopenmp

# Source files and object files
SRCDIR = src
OBJDIR = obj
SRCS = $(wildcard $(SRCDIR)/*.cpp)
OBJS = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRCS))

# Name of the executable
TARGET = D2DGRter

all: $(TARGET)

$(OBJDIR):
	@mkdir -p $(OBJDIR)  # Create directory if it doesn't exist

$(TARGET): main.cpp $(OBJS)
	$(CXX) $(WARNINGS) $(CXXFLAGS) $^ -o $@

# Compilation rule for object files with automatic dependency generation
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR) Makefile
	$(CXX) $(WARNINGS) $(CXXFLAGS) -c $< -o $@

# ./D2DGRter *.gmp *.gcl *.cst *.lg
# Run the program
run:
	./$(TARGET) $(case0).gmp $(case0).gcl $(case0).cst $(case0).lg

# Verifier target
eva:
	./Evaluator $(cell) $(opt) $(output)

pic:
	python3 visualize_flip_flops.py output.txt output.png


test0:
	./$(TARGET) $(case0).gmp $(case0).gcl $(case0).cst $(case0).lg
	./Evaluator/Evaluator ./publicCase/testcase0 testcase0

test1:
	./$(TARGET) $(case1).gmp $(case1).gcl $(case1).cst $(case1).lg
	./Evaluator/Evaluator ./publicCase/testcase1 testcase1

test2:
	./$(TARGET) $(case2).gmp $(case2).gcl $(case2).cst $(case2).lg
	./Evaluator/Evaluator ./publicCase/testcase2 testcase2

test3:
	./$(TARGET) $(case3).gmp $(case3).gcl $(case3).cst $(case3).lg
	./Evaluator/Evaluator ./publicCase/testcase_me3 testcase_me3

# Clean rule
clean:
	@echo "Cleaning up..."
	rm -rf $(OBJDIR)/*.o $(TARGET)

cfile: