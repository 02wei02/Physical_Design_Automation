#ifndef _MANAGER_H_
#define _MANAGER_H_

#include <algorithm>
#include <climits>
#include <iomanip>  // 用於 std::fixed 和 std::setprecision
#include <iostream>
#include <map>
#include <queue>
#include <random>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <ctime>

#include "Cell.h"
#include "Coor.h"
#include "FF.h"
#include "Parser.h"
#include "PlacementRow.h"

class Manager {
 public:
  double alpha;
  double beta;
  std::unordered_map<std::string, Cell> cells_map;
  std::map<double, PlacementRow> rows_map;  // key by y
  std::vector<FF> merged_vec;
  std::map<double, std::map<double, Cell>> cells_2d;
  std::queue<Cell> q_cell;
  std::queue<bool> q_is_left; 
  std::set<std::string> pushed_cells;
  Coor die;  // record die size
  std::multimap<double, Cell> cells_by_x;
  std::multimap<double, Cell> cells_by_y;
  std::ofstream lg_out;  // Add this line for the output file stream
  double window_y_upper, window_y_lower;
  double window_x_right, window_x_left;
  double placement_row_height;
  double placement_row_width;
  double placement_row_end_x;
  double placement_row_end_y;
  double placement_row_start_x;
  double placement_row_start_y;
  std::clock_t time_start, time_end;
  double duration;

 public:
  Manager();
  ~Manager();
  void Run();  // run for all execution
  void parser(const std::string &lg, const std::string &opt,
              const std::string &out);
  void outputFile(const Cell &cell);
  void setDieSize(double x1, double y1, double x2, double y2) {
    die.setBorder(x1, y1, x2, y2);
  }
  friend class Parser;

  void makeLegalize();
  void makeWindow(const FF &ff);
  void outFinalPosition();
  void initialCell2D();
  bool isLegal(Cell &target_cell);
  void leftRightLegalize(Cell &target_cell);
  void removeCell2D(const std::string &target);
  void addCell2D(Cell &target);

  //------------------------------------------------------------------//
  //                               Fail                               //
  //------------------------------------------------------------------//    
  void legalAlg(const std::string &ff);
};

#endif  // _MANAGER_H_