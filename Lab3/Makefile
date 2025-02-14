.PHONY: all clean run verifier pic

folder = testcase
case = testcase1_16900
case1 = testcase1_16900
case2 = testcase1_ALL0_5000
case3 = testcase2_100
case4 = testcase1_MBFF_LIB_7000
cell = $(folder)/$(case).lg
opt =  $(folder)/$(case).opt
output = $(folder)/$(case)_post.lg

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
TARGET = Legalizer

all: $(TARGET)

$(OBJDIR):
	@mkdir -p $(OBJDIR)  # Create directory if it doesn't exist

$(TARGET): main.cpp $(OBJS)
	$(CXX) $(WARNINGS) $(CXXFLAGS) $^ -o $@

# Compilation rule for object files with automatic dependency generation
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR) Makefile
	$(CXX) $(WARNINGS) $(CXXFLAGS) -c $< -o $@

# Run the program
run:
	./$(TARGET) $(cell) $(opt) $(output)

# Verifier target
eva:
	./Evaluator $(cell) $(opt) $(output)

pic:
	python3 visualize_flip_flops.py output.txt output.png


test1:
	./$(TARGET) $(folder)/$(case1).lg $(folder)/$(case1).opt $(folder)/$(case1)_post.lg
	./Evaluator $(folder)/$(case1).lg $(folder)/$(case1).opt $(folder)/$(case1)_post.lg

test2:
	./$(TARGET) $(folder)/$(case2).lg $(folder)/$(case2).opt $(folder)/$(case2)_post.lg
	./Evaluator $(folder)/$(case2).lg $(folder)/$(case2).opt $(folder)/$(case2)_post.lg

test3:
	./$(TARGET) $(folder)/$(case3).lg $(folder)/$(case3).opt $(folder)/$(case3)_post.lg
	./Evaluator $(folder)/$(case3).lg $(folder)/$(case3).opt $(folder)/$(case3)_post.lg

test4:
	./$(TARGET) $(folder)/$(case4).lg $(folder)/$(case4).opt $(folder)/$(case4)_post.lg
	./Evaluator $(folder)/$(case4).lg $(folder)/$(case4).opt $(folder)/$(case4)_post.lg
	

# Clean rule
clean:
	@echo "Cleaning up..."
	rm -rf $(OBJDIR)/*.o $(TARGET)

cfile: