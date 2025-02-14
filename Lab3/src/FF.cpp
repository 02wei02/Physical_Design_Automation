#include "FF.h"

FF::FF() {
  // Initialize your data members here if needed
}

FF::FF(std::string merged_name, double x, double y, double width, double height,
       std::vector<std::string> ff_list)
    : merged_name(merged_name),
      x(x),
      y(y),
      width(width),
      height(height),
      ff_list(ff_list) {
  // Constructor
}

FF::~FF() {
  // Destructor
}