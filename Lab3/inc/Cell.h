#ifndef _CELL_H_
#define _CELL_H_

#include <iostream>

class Cell {
 public:
  std::string name;
  double x;  // left bottom
  double y;  // left bottom
  double width;
  double height;
  double x_rt;
  double y_rt;
  int fixed;  // 0: movable, 1: fixed, 2: merged

 public:
  Cell();
  Cell(const std::string &name, double width, double height);
  Cell(const std::string &name, double x, double y, double width, double height,
       double x_rt, double y_rt, int fixed);

  ~Cell();

  void setPos(double x, double y);
  void setArea();

  // Define a less-than operator based on `id` or other properties
  bool operator<(const Cell &other) const {
    if (x != other.x) {
      return x < other.x;
    }
    return y < other.y;
  }
};

#endif