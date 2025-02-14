#include "PlacementRow.h"

PlacementRow::PlacementRow() {
  // Constructor
  start_x = 0.0;
  start_y = 0.0;
  site_width = 0.0;
  site_height = 0.0;
  num_sites = 0;
}

PlacementRow::PlacementRow(double start_x, double start_y, double site_width,
                           double site_height, int num_sites)
    : start_x(start_x),
      start_y(start_y),
      site_width(site_width),
      site_height(site_height),
      num_sites(num_sites) {}

PlacementRow::~PlacementRow() {
  // Destructor
}