#include "Coor.h"

Coor::Coor() {
  // Constructor
  x = 0.0;
  y = 0.0;
}

Coor::Coor(double x, double y) : x(x), y(y) {
  // Constructor
}

Coor::Coor(double x, double y, double x_rt, double y_rt)
    : x(x), y(y), x_rt(x_rt), y_rt(y_rt) {
        // Constructor
      };

Coor::~Coor() {
  // Destructor
}

void Coor::setX(double x) {
  // Set x
  this->x = x;
}

void Coor::setY(double y) {
  // Set y
  this->y = y;
}

void Coor::setXY(double x, double y) {
  // Set x and y
  this->x = x;
  this->y = y;
}

void Coor::setBorder(double x, double y, double x_rt, double y_rt) {
  // Set lb and rt
  this->x = x;
  this->y = y;
  this->x_rt = x_rt;
  this->y_rt = y_rt;
}