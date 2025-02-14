#ifndef _PARSER_
#define _PARSER_

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

struct XY {
  int x, y;
  XY() : x(0), y(0) {};  // Default constructor initializes both x and y to 0
  XY(int x, int y) : x(x), y(y) {};  // Constructor with parameters
};

struct Block {
  XY lb_point;  // left-bottom point
  XY rt_point;  // right-top point

  Block *rt;  // rightmost top pointer
  Block *tr;  // topmost right pointer
  Block *bl;  // bottommost left pointer
  Block *lb;  // leftmost bottom pointer
  int index;
  bool vacant_tile;

  Block(int lb_x = 0, int lb_y = 0, int rt_x = 0, int rt_y = 0, int index = 0,
        bool vacant = true)
      : lb_point(lb_x, lb_y),
        rt_point(rt_x, rt_y),
        rt(nullptr),  // right top
        tr(nullptr),  // top right
        bl(nullptr),  // bottom left
        lb(nullptr),  // left bottom
        index(index),
        vacant_tile(vacant) {}
};

class Parser {
 private:
  int count_vacant_tiles_;
  int layout_width_, layout_height_;
  vector<XY> point_ans_;
  vector<Block *> block_tiles_;
  vector<Block *> vacant_tiles_;
  string in, out;

 public:
  Parser(string argIn, string argOut)
      : in(std::move(argIn)), out(std::move(argOut)) {}
  void Run() {
    InputFile();
    // sort by block index from small to large
    std::sort(
        block_tiles_.begin(), block_tiles_.end(),
        [](const Block *a, const Block *b) { return a->index < b->index; });
    OutputFile();
  }
  void InputFile();
  void OutputFile();
  Block *InsertPointFinding(const XY &, Block *);
  Block *PointFinding(const XY &, Block *);
  int HorizontalSplit(Block *, int);
  int VerticalCut(Block *, Block *);
  void InitializeAddBlock(Block *cut_block, Block *add_block);
  Block *HandleRightCut(Block *cut_block, Block *add_block, int &tile_num,
                        int &rt_x);
  void UpdateNeighboringBlocks(Block *cut_block, Block *add_block,
                               Block *right_block, int &lb_x, int &rt_x);
  void HandleMerges(Block *add_block, Block *cut_block, Block *right_block,
                    int &tile_num);
  int Merge(Block *);
};

#endif