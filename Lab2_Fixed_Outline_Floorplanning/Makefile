.PHONY: all clean run verifier cfile

case = ami49
alpha = 0.5
block = $(case)/$(case).block
net =  $(case)/$(case).nets
output = $(case)/$(case)_out.txt

# Compiler settings
CXX = g++
CXXFLAGS = -I ./inc -std=c++11
WARNINGS = -g -Wall -Wextra

# Source files and object files
SRCDIR = ./src
OBJDIR = ./obj
SRCS = $(wildcard $(SRCDIR)/*.cpp)
OBJS = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRCS))

# Name of the executable
TARGET = Lab2

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
	./$(TARGET) $(alpha) $(block) $(net) $(output)

# Verifier target
verifier:
	./verifier $(alpha) $(block) $(net) $(output)

# Clean rule
clean:
	@echo "Cleaning up..."
	rm -rf $(OBJDIR)/*.o $(TARGET)

cfile:
	rm -f ami33/*_out.txt ami49/*_out.txt vda317b/*_out.txt
	rm -f ami33/*.HPWL ami49/*.HPWL vda317b/*.HPWL