#ifndef ROUTING_H
#define ROUTING_H

#include <cmath>
#include <fstream>
#include <map>
#include <queue>
#include <sstream>
#include <string>
#include <vector>
#include <stack>
#include <iostream>
#include <time.h>

 // down, left, right, top
enum Direction{
  down,
  left, 
  right,
  top
};

struct Node {
  int x, y;          // index
  bool is_vertical;  // is_vertical
  double F, G, H;

  // F = G + H
  // H : The estimated cost
  // G : The actual cost
  Node(int x, int y) : x(x), y(y) {}

  bool operator<(const Node& a) const { return F == a.F ? G > a.G : F > a.F; }
};

// Struct to define a location with boundaries and dimensions
struct Loc {
  int x_lb, y_lb;     // Lower bounds for x and y
  int width, height;  // Width and height

  Loc(){}
  Loc(int x, int y, int w, int h) : x_lb(x), y_lb(y), width(w), height(h) {}
  Loc(int x, int y) : x_lb(x), y_lb(y) {}
};

// Struct to define the size of an object
struct Size {
  int width, height;

  Size(){}
  Size(int w, int h) : width(w), height(h) {}
};

// Struct for bumps with an ID and location
struct Bump {
  int id;
  int x_lb, y_lb;  // Lower bounds for x and y
  int x_index, y_index;

  Bump(int id, int x, int y, int x_index, int y_index)
      : id(id), x_lb(x), y_lb(y), x_index(x_index), y_index(y_index) {}
};

// Struct for grid cells with boundaries
struct Gcell {
  int left, bottom, right, top;  // Boundaries of the cell
  int left_pass, bottom_pass, right_pass, top_pass;
  double cost;

  Gcell() : left(0), bottom(0), right(0), top(0) {
    left_pass = 0;
    bottom_pass = 0;
    right_pass = 0;
    top_pass = 0;
  }  // Default constructor
  Gcell(int left, int bottom) : left(left), bottom(bottom), right(0), top(0) {
    left_pass = 0;
    bottom_pass = 0;
    right_pass = 0;
    top_pass = 0;
  }
};

struct Segment {
  int x1, y1, x2, y2;  // Segment start point and end point
  bool is_vertical;
  Segment(int x1, int y1, int x2, int y2) : x1(x1), y1(y1), x2(x2), y2(y2) {}
};

// Main Routing class
class Routing {
 public:
  Routing(int argc, char* argv[]);
  void run();

 private:
  // Routing area and grid size
  Loc routing_area_;  // Default initialized to zero
  Size grid_;               // Default initialized to zero

  // Map to store bumps categorized by chip ID
  std::map<int, std::vector<Bump>> bumps_vec_;

  // Weight parameters for routing
  double alpha_ = 0.0, beta_ = 0.0, gamma_ = 0.0, delta_ = 0.0, via_cost_ = 0.0;
  const int next_position[4][2] = {{-1, 0}, {0, -1}, {0, 1}, {1, 0}}; // (0: y, 1: x) 
  // down, left, right, top

  // 2D vector of grid cells
  std::vector<std::vector<Gcell>> metal1_;
  std::vector<std::vector<Gcell>> metal2_;

  int x_gcell_num_, y_gcell_num_;

  std::vector<std::vector<bool>>
      close_;  // visit the condition record. closed list
  std::vector<std::vector<int>> valueF_;  // record every node F value
  std::priority_queue<Node> open_;        // open_ list

  // Output file path and results
  std::string out_;
  std::vector<std::string> results_;

  std::vector<std::string> filePaths_;

  std::vector<std::vector<Loc>> pre_;

  std::stack<Segment> segments_;

  std::vector<int> max_metal_; // 0 for meta1, 1 for metal2
  std::ofstream out;

  // Functions for reading input files
  void readGMP(const std::string& filePath);
  void readGCL(const std::string& filePath);
  void readCST(const std::string& filePath);

  int Manhatten(int x, int y, int x1, int y1);

  double addCost(const Node current_node, const Node next_node, Direction dir);
  void costFunc(const Node current_node, Node& next_node, Direction dir, int x_end,
                int y_end);

  bool isValidNode(Node next_node);
  void Astar(int x_start, int y_start, int x_end, int y_end);
  void outputFile(int iter);
  void Initialized();
  void retracePath(int x1, int y1);
  void printGrid(std::vector<std::vector<int>>);
};

#endif  // ROUTING_H
