
#include "parser.h"

// (+x, +y)
Block *Parser::PointFinding(const XY &target, Block *start_block) {
  // if point is on the boundary, left top is the block tile it located
  while (
      target.x >= (start_block->rt_point.x) ||
      target.x < (start_block->lb_point.x) ||
      target.y >= (start_block->rt_point.y) ||
      target.y <
          (start_block->lb_point.y))  // if the target is in the starting block
  {
    // top bottom move
    while (target.y >= (start_block->rt_point.y) ||
           target.y < (start_block->lb_point.y)) {
      if (target.y < (start_block->lb_point.y)) start_block = start_block->lb;
      if (target.y >= (start_block->rt_point.y)) start_block = start_block->rt;
    }
    // left right move
    while (target.x >= (start_block->rt_point.x) ||
           target.x < (start_block->lb_point.x)) {
      if (target.x < (start_block->lb_point.x)) start_block = start_block->bl;
      if (target.x >= (start_block->rt_point.x)) start_block = start_block->tr;
    }
  }
  return start_block;
}

// (+x, -y)
Block *Parser ::InsertPointFinding(const XY &target, Block *start_block) {
  while (
      target.x >= (start_block->rt_point.x) ||
      target.x < (start_block->lb_point.x) ||
      target.y > (start_block->rt_point.y) ||
      target.y <=
          (start_block->lb_point.y))  // if the target is in the starting block
  {
    while (target.y <= (start_block->lb_point.y) ||
           target.y > (start_block->rt_point.y)) {
      if (target.y <= (start_block->lb_point.y))
        start_block = start_block->lb;
      else if (target.y > (start_block->rt_point.y))
        start_block = start_block->rt;
    }
    while (target.x < (start_block->lb_point.x) ||
           target.x >= (start_block->rt_point.x)) {
      if (target.x < (start_block->lb_point.x))
        start_block = start_block->bl;
      else if (target.x >= (start_block->rt_point.x))
        start_block = start_block->tr;
    }
  }
  return start_block;
}

int Parser::HorizontalSplit(Block *cut_block, int addBlocky) {
  // create the new vacant tile on top
  Block *top_block = new Block(cut_block->lb_point.x, addBlocky,
                               cut_block->rt_point.x, cut_block->rt_point.y);

  top_block->lb = cut_block;
  top_block->rt = cut_block->rt;
  top_block->tr = cut_block->tr;
  top_block->bl = cut_block->bl;

  vacant_tiles_.emplace_back(top_block);

  cut_block->rt_point.y = addBlocky;  // to refresh the height of block be cut

  while (top_block->bl != nullptr && top_block->bl->rt_point.y <= addBlocky) {
    top_block->bl = top_block->bl->rt;
  }  // the bottommost left of the top block is right

  cut_block->rt = top_block;

  while (cut_block->tr != nullptr && cut_block->tr->lb_point.y >= addBlocky) {
    cut_block->tr = cut_block->tr->lb;
  }  // the topmost right of the block being cut is right

  Block *left_connect = top_block->bl;
  while ((left_connect != nullptr &&
          left_connect->rt_point.y <= top_block->rt_point.y)) {
    left_connect->tr = top_block;
    left_connect = left_connect->rt;
  }
  Block *right_connect = top_block->tr;
  while (right_connect != nullptr &&
         right_connect->lb_point.y >= top_block->lb_point.y) {
    right_connect->bl = top_block;
    right_connect = right_connect->lb;
  }
  Block *top_connect = top_block->rt;
  while (top_connect != nullptr &&
         top_connect->lb_point.x >= top_block->lb_point.x) {
    top_connect->lb = top_block;
    top_connect = top_connect->bl;
  }
  return 1;  // cut 1 time, so vacant tile + 1
}

void Parser::InitializeAddBlock(Block *cut_block, Block *add_block) {
  add_block->lb = cut_block->lb;
  add_block->lb_point.y = cut_block->lb_point.y;
  // Traverse the left edge of the block
  while (add_block->lb != nullptr &&
         add_block->lb->rt_point.x <= add_block->lb_point.x) {
    add_block->lb = add_block->lb->tr;  // Move along the left edge
  }
}

Block *Parser::HandleRightCut(Block *cut_block, Block *add_block, int &tile_num,
                              int &rt_x) {
  Block *right_block = add_block;
  // Check if there is a need to split and create a new right block
  if (rt_x == cut_block->rt_point.x) {
    // No right cut, just link top-right and right blocks
    if (add_block->rt_point.y == cut_block->rt_point.y) {
      add_block->tr = cut_block->tr;
      add_block->rt = cut_block->rt;
    }
  } else {  // right cut
    tile_num++;
    right_block = new Block(rt_x, cut_block->lb_point.y, cut_block->rt_point.x,
                            cut_block->rt_point.y);
    vacant_tiles_.emplace_back(
        right_block);  // add new vacant tile on the right of add block

    // Link the right block
    right_block->bl = add_block;
    right_block->lb = add_block->lb;
    right_block->rt = cut_block->rt;
    right_block->tr = cut_block->tr;
    // Traverse the left edge of the right block
    while (right_block->lb != nullptr &&
           right_block->lb->rt_point.x <= right_block->lb_point.x) {
      right_block->lb = right_block->lb->tr;
    }
    if (add_block->rt_point.y == cut_block->rt_point.y) {
      add_block->tr = right_block;
      add_block->rt = cut_block->rt;
      // Adjust the right block
      while (add_block->rt != nullptr &&
             add_block->rt->lb_point.x >= add_block->rt_point.x) {
        add_block->rt = add_block->rt->bl;
      }
    }
  }
  return right_block;
}

void Parser::UpdateNeighboringBlocks(Block *cut_block, Block *add_block,
                                     Block *right_block, int &lb_x, int &rt_x) {
  Block *right_connect = cut_block->tr;
  Block *top_connect = cut_block->rt;
  Block *bottom_connect = add_block->lb;
  // Update bottom blocks linked to the right block
  while (right_connect != nullptr &&
         right_connect->lb_point.y >= cut_block->lb_point.y) {
    right_connect->bl = right_block;
    right_connect = right_connect->lb;
  }
  // Update left blocks linked to the add block and the right block
  while (top_connect != nullptr && top_connect->lb_point.x >= lb_x) {
    if (top_connect != add_block) {
      if (top_connect->lb_point.x >= rt_x)
        top_connect->lb = right_block;
      else
        top_connect->lb = add_block;
    }
    top_connect = top_connect->bl;
  }
  // Update right blocks linked to the add block and the right block
  while (bottom_connect != nullptr &&
         bottom_connect->rt_point.x <= cut_block->rt_point.x) {
    if (bottom_connect->rt_point.x > rt_x)
      bottom_connect->rt = right_block;
    else
      bottom_connect->rt = add_block;
    bottom_connect = bottom_connect->tr;
  }
}

int Parser::VerticalCut(Block *cut_block, Block *add_block) {
  int lb_x = add_block->lb_point.x;
  int rt_x = add_block->rt_point.x;
  Block *right_block;
  int tile_num = 0;

  InitializeAddBlock(cut_block, add_block);

  right_block = HandleRightCut(cut_block, add_block, tile_num, rt_x);

  UpdateNeighboringBlocks(cut_block, add_block, right_block, lb_x, rt_x);

  if (lb_x != cut_block->lb_point.x) {  // bottom need to be cut
    cut_block->tr = add_block;
    cut_block->rt_point.x = lb_x;  // horizontal cut
    while (cut_block->rt != nullptr &&
           cut_block->rt->lb_point.x >= cut_block->rt_point.x) {
      cut_block->rt = cut_block->rt->bl;  // connect the correct rightmost top
    }
    add_block->bl = cut_block;
  } else {  // bottom don't need to be cut
    Block *right_traversal = cut_block->bl;
    add_block->bl = cut_block->bl;
    while (right_traversal != nullptr &&
           right_traversal->rt_point.y <= cut_block->rt_point.y) {
      right_traversal->tr = add_block;
      right_traversal = right_traversal->rt;
    }

    cut_block = add_block;  // new standard
    tile_num--;
  }

  HandleMerges(add_block, cut_block, right_block, tile_num);

  return tile_num;
}

void Parser::HandleMerges(Block *add_block, Block *cut_block,
                          Block *right_block, int &tile_num) {
  if (right_block->lb != nullptr && right_block != add_block) {
    tile_num += Merge(right_block);
    tile_num += Merge(right_block->lb);
  } else if (right_block != add_block) {
    tile_num += Merge(right_block);
  }
  if (cut_block->lb != nullptr && cut_block != add_block) {
    tile_num += Merge(cut_block);
    tile_num += Merge(cut_block->lb);
  } else if (cut_block != add_block) {
    tile_num += Merge(cut_block);
  }
}

int Parser::Merge(Block *block_bottom) {
  Block *top_block = block_bottom->rt;
  if (top_block != nullptr &&
      block_bottom->lb_point.x == top_block->lb_point.x &&
      block_bottom->rt_point.x == top_block->rt_point.x &&
      block_bottom->vacant_tile && top_block->vacant_tile) {
    block_bottom->rt_point.y = top_block->rt_point.y;
    block_bottom->rt_point.x = top_block->rt_point.x;
    block_bottom->tr = top_block->tr;
    block_bottom->rt = top_block->rt;
    Block *right_connect;
    right_connect = block_bottom->tr;
    while (right_connect != nullptr &&
           right_connect->lb_point.y >= top_block->lb_point.y) {
      right_connect->bl = block_bottom;
      right_connect = right_connect->lb;
    }
    Block *left_connect;
    left_connect = block_bottom->bl;
    while (left_connect != nullptr &&
           left_connect->rt_point.y <= top_block->rt_point.y) {
      left_connect->tr = block_bottom;
      left_connect = left_connect->rt;
    }
    Block *top_connect;
    top_connect = block_bottom->rt;
    while (top_connect != nullptr &&
           top_connect->lb_point.x >= top_block->lb_point.x) {
      top_connect->lb = block_bottom;
      top_connect = top_connect->bl;
    }
    return -1;
  }
  return 0;
}

void Parser::InputFile() {
  ifstream input(in);

  if (!input) {
    cout << "Can't open input file .txt" << endl;
  }

  string line;
  count_vacant_tiles_ = 1;

  input >> layout_width_ >> layout_height_;
  Block *initial_block = new Block(0, 0, layout_width_, layout_height_);
  vacant_tiles_.emplace_back(initial_block);
  while (getline(input, line)) {
    if (line.empty()) continue;
    std::istringstream iss(line);
    if (line[0] == 'P')  // point finding
    {
      string dump;
      XY point_find;
      XY answer_temp;
      iss >> dump >> point_find.x >> point_find.y;
      answer_temp = PointFinding(point_find, initial_block)->lb_point;
      point_ans_.emplace_back(answer_temp);
    } else  // block insert
    {
      int x, y, width, height;
      int block_id;
      iss >> block_id >> x >> y >> width >> height;
      Block *add_block =
          new Block(x, y, x + width, y + height, block_id, false);
      block_tiles_.emplace_back(add_block);

      Block *space_finding = InsertPointFinding({x, y + height}, initial_block);

      if ((space_finding->rt_point.y) > (add_block->rt_point.y)) {
        count_vacant_tiles_ =
            count_vacant_tiles_ +
            HorizontalSplit(space_finding, add_block->rt_point.y);
      }

      int lb_y;
      lb_y = add_block->lb_point.y;

      while (space_finding != nullptr && space_finding->lb_point.y > lb_y) {
        count_vacant_tiles_ += VerticalCut(space_finding, add_block);
        space_finding = add_block->lb;
      }

      if (space_finding != nullptr && space_finding->lb_point.y < lb_y) {
        count_vacant_tiles_ += HorizontalSplit(space_finding, lb_y);
        space_finding = space_finding->rt;
      }

      // final horizontal cut
      count_vacant_tiles_ += VerticalCut(space_finding, add_block);
      initial_block = block_tiles_.front();
    }
  }
  input.close();
}

void Parser::OutputFile() {
  ofstream output(out);

  if (!output) {
    cout << "Can't output file!" << endl;
  }

  output << (block_tiles_.size() + count_vacant_tiles_) << endl;
  for (Block *i : block_tiles_) {
    output << i->index << " ";
    int neighbor_tile = 0;
    int neighbor_space = 0;

    // top neighbor count
    Block *top_count = i->rt;
    while (top_count != nullptr && top_count->rt_point.x > i->lb_point.x) {
      if (!top_count->vacant_tile)
        neighbor_tile = neighbor_tile + 1;
      else
        neighbor_space = neighbor_space + 1;
      top_count = top_count->bl;
    }

    // bottom neighbor count
    Block *bottom_count = i->lb;
    while (bottom_count != nullptr &&
           bottom_count->lb_point.x < i->rt_point.x) {
      if (!bottom_count->vacant_tile)
        neighbor_tile = neighbor_tile + 1;
      else
        neighbor_space = neighbor_space + 1;
      bottom_count = bottom_count->tr;
    }

    // left neighbor count
    Block *left_count = i->bl;
    while (left_count != nullptr && left_count->lb_point.y < i->rt_point.y) {
      if (!left_count->vacant_tile)
        neighbor_tile = neighbor_tile + 1;
      else
        neighbor_space = neighbor_space + 1;
      left_count = left_count->rt;
    }

    // right neighbor count
    Block *right_count = i->tr;
    while (right_count != nullptr && right_count->rt_point.y > i->lb_point.y) {
      if (!right_count->vacant_tile)
        neighbor_tile = neighbor_tile + 1;
      else
        neighbor_space = neighbor_space + 1;
      right_count = right_count->lb;
    }
    output << neighbor_tile << " " << neighbor_space << endl;
  }

  // print the space tile for testing
  for (XY i : point_ans_) {
    output << i.x << " " << i.y << endl;
  }
}