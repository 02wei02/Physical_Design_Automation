#ifndef _PlacementRow_H_
#define _PlacementRow_H_

#include <iostream>

class PlacementRow {
 public:
  double start_x;
  double start_y;
  double site_width;
  double site_height;
  int num_sites;

  PlacementRow();
  ~PlacementRow();
  PlacementRow(double start_x, double start_y, double site_width,
               double site_height, int num_sites);
};

#endif  // _PlacementRow_H_