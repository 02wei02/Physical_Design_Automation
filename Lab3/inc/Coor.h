#ifndef _COORDINATE_H_
#define _COORDINATE_H_

#include <iostream>

class Coor {
 public:
  double x;
  double y;
  double x_rt;
  double y_rt;

 public:
  Coor();
  Coor(double x, double y);
  Coor(double x, double y, double x_rt, double y_rt);
  ~Coor();

  void setX(double x);
  void setY(double y);
  void setXY(double x, double y);
  void setBorder(double x, double y, double x_rt, double y_rt);
};

#endif  // _COORDINATE_H_