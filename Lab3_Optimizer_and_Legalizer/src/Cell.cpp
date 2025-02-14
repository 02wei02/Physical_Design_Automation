#include "Cell.h"

Cell::Cell() {
  // Constructor
  name = "";
  x = 0.0;
  y = 0.0;
  width = 0.0;
  height = 0.0;
  x_rt = 0.0;
  y_rt = 0.0;
  fixed = false;
}

Cell::Cell(const std::string &name, double x, double y, double width,
           double height, double x_rt, double y_rt, int fixed)
    : name(name),
      x(x),
      y(y),
      width(width),
      height(height),
      x_rt(x_rt),
      y_rt(y_rt),
      fixed(fixed) {}

Cell::~Cell() {
  // Destructor
}