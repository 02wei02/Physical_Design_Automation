#include "routing.h"

Routing::Routing(int argc, char* argv[]) {
  if (argc < 5) {
    std::cerr << "Error: Insufficient arguments. Usage: <gmp> <gcl> <cst> <lg>"
              << std::endl;
    exit(1);
  }

  // Store file paths from command-line arguments
  for (int i = 1; i < argc; ++i) {
    filePaths_.emplace_back(argv[i]);
  }

  out.open(filePaths_[3]);

  if (!out.is_open()) {
    std::cerr << "Error: Unable to open_ output file: " << filePaths_[3]
              << std::endl;
    exit(1);
  }
}

void Routing::run() {
  // Step 1: Read all files
  readGMP(filePaths_[0]);
  readGCL(filePaths_[1]);
  readCST(filePaths_[2]);

  // Step 2: Process the data
  for (unsigned int i = 0; i < bumps_vec_[0].size(); ++i) {
    Bump bump_start = bumps_vec_[0][i];
    Bump bump_end = bumps_vec_[1][i];

    Astar(bump_start.x_index, bump_start.y_index, bump_end.x_index,
          bump_end.y_index);

    retracePath(bump_end.x_index, bump_end.y_index);
    // Step 3: Output results
    outputFile(i);
  }
}

void Routing::readGMP(const std::string& filePath) {
  std::ifstream file(filePath);
  if (!file.is_open()) {
    std::cerr << "Error: Unable to open_ file .gmp " << filePath << std::endl;
    exit(1);
  }

  int id = -1;
  std::string line;
  while (std::getline(file, line)) {
    // Remove trailing '\r' if present
    if (!line.empty() && line.back() == '\r') {
        line.pop_back();
    }

    int x_chip, y_chip, w_chip, h_chip;  // chip information
    if (line == ".ra") {
        std::getline(file, line);
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        std::istringstream iss(line);
        int x_ra, y_ra, w_ra, h_ra;  // routing area
        iss >> x_ra >> y_ra >> w_ra >> h_ra;
        routing_area_ = Loc(x_ra, y_ra, w_ra, h_ra);
    } else if (line == ".g") {
        std::getline(file, line);
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        std::istringstream iss(line);
        iss >> grid_.width >> grid_.height;
    } else if (line == ".c") {
        std::getline(file, line);
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        std::istringstream iss(line);
        iss >> x_chip >> y_chip >> w_chip >> h_chip;
        ++id;
    } else if (line == ".b" || line.empty()) {
        continue;
    } else {
        std::istringstream iss(line);
        int id_bump, x, y;
        iss >> id_bump >> x >> y;
        int x_real = x_chip + x;
        int y_real = y_chip + y;
        int x_index = x_real / grid_.width;
        int y_index = y_real / grid_.height;
        Bump bump(id_bump, x_real, y_real, x_index, y_index);
        bumps_vec_[id].emplace_back(bump);
    }
}
  file.close();
}

void Routing::readGCL(const std::string& filePath) {
  std::ifstream file(filePath);
  if (!file.is_open()) {
    std::cerr << "Error: Unable to open_ file .gcl " << filePath << std::endl;
    exit(1);
  }

  int id = 0;
  std::string line;
  x_gcell_num_ = routing_area_.width / grid_.width;
  y_gcell_num_ = routing_area_.height / grid_.height;

  metal1_.resize(y_gcell_num_, std::vector<Gcell>(x_gcell_num_, Gcell()));
  metal2_.resize(y_gcell_num_, std::vector<Gcell>(x_gcell_num_, Gcell()));

  while (std::getline(file, line)) {
    if (!line.empty() && line.back() == '\r') {
        line.pop_back();
    }
    if (line == ".ec") {
      continue;
    }
    int x_g = id % x_gcell_num_;
    int y_g = id / x_gcell_num_;
    std::istringstream iss(line);
    int left, bottom;
    iss >> left >> bottom;

    metal1_[y_g][x_g] = Gcell(left, bottom);
    metal2_[y_g][x_g] = Gcell(left, bottom);

    if (y_g >= 1) {
      metal1_[y_g - 1][x_g].top = bottom;
      metal2_[y_g - 1][x_g].top = bottom;
    }
    if (x_g >= 1) {
      metal1_[y_g][x_g - 1].right = left;
      metal2_[y_g][x_g - 1].right = left;
    }
    id++;
  }

  file.close();
}

void Routing::readCST(const std::string& filePath) {
  std::ifstream file(filePath);
  if (!file.is_open()) {
    std::cerr << "Error: Unable to open_ file .cst " << filePath << std::endl;
    exit(1);
  }

  int current_layer = -1;
  std::string line;

  max_metal_.assign(2, 0);

  int x_l = 0, y_l = 0, xy = 0;
  while (std::getline(file, line)) {
    std::istringstream iss(line);
    std::string key;

    if (line[0] == '.') {
      iss >> key;
      if (key == ".alpha") {
        iss >> alpha_;
      } else if (key == ".beta") {
        iss >> beta_;
      } else if (key == ".gamma") {
        iss >> gamma_;
      } else if (key == ".delta") {
        iss >> delta_;
      } else if (key == ".v") {
        std::getline(file, line);
        std::istringstream isss(line);
        isss >> via_cost_;
      } else if (key == ".l") {
        ++current_layer;
        x_l = 0;
        y_l = 0;
        xy = 0;
      }
    } else {
      double cost;
      while (iss >> cost) {
        if (current_layer == 0) {
          metal1_[y_l][x_l].cost = cost;
          if(cost > max_metal_[0]){
            max_metal_[0] = cost;
          }
        } else {
          metal2_[y_l][x_l].cost = cost;
          if(cost > max_metal_[1]){
            max_metal_[1] = cost;
          }
        }
        ++xy;
        x_l = xy % x_gcell_num_;
        y_l = xy / x_gcell_num_;
      }
    }
  }

  file.close();
}

void Routing::outputFile(int iter) {
  // Write processed routing results here
  out << "n" << iter + 1 << std::endl;
  while (!segments_.empty()) {
    Segment seg = segments_.top();
    segments_.pop();

    int x2 = seg.x1 * grid_.width + routing_area_.x_lb;
    int y2 = seg.y1 * grid_.height + routing_area_.y_lb;
    int x1 = seg.x2 * grid_.width + routing_area_.x_lb;
    int y1 = seg.y2 * grid_.height + routing_area_.y_lb;

    if (seg.y1 == seg.y2) {  // horizontal M2
      out << "via" << std::endl;
      out << "M2 " << x1 << " " << y1 << " " << x2 << " " << y2 << std::endl;
      out << "via" << std::endl;
    } else {
      out << "M1 " << x1 << " " << y1 << " " << x2 << " " << y2 << std::endl;
    }
  }
  out << ".end" << std::endl;
}

int Routing::Manhatten(int x, int y, int x1, int y1) {  // gcell unit
  return (abs(x - x1) + abs(y - y1));
}

bool Routing::isValidNode(Node next_node) {
  if (next_node.x < 0 || next_node.x >= x_gcell_num_) {
    return false;
  }
  if (next_node.y < 0 || next_node.y >= y_gcell_num_) {
    return false;
  }
  if (close_[next_node.y][next_node.x]) {
    return false;
  }
  return true;
}

void Routing::Initialized() {
  close_.assign(y_gcell_num_, std::vector<bool>(x_gcell_num_, false));
  valueF_.assign(y_gcell_num_, std::vector<int>(x_gcell_num_, 0));
  pre_.assign(y_gcell_num_,
              std::vector<Loc>(x_gcell_num_, Loc(-1, -1)));  // (x, y)
  while (!open_.empty()) {
    open_.pop();
  }
}

void Routing::Astar(int x_start, int y_start, int x_end, int y_end) {
  Initialized();

  Node node(x_start, y_start);
  node.G = 0;
  node.H = Manhatten(x_start, y_start, x_end, y_end);
  node.F = node.G + node.H;

  valueF_[y_start][x_start] = node.F;
  open_.push(node);

  while (!open_.empty()) {
    Node current_node = open_.top();
    open_.pop();

    close_[current_node.y][current_node.x] = true;

    if (current_node.x == x_end && current_node.y == y_end) {
      break;
    }

    for (int i = 0; i < 4; i++) {
      Node next_node(current_node.x + next_position[i][1],
                     current_node.y + next_position[i][0]);
      if (i == down || i == top) {
        next_node.is_vertical = 1;
      } else {
        next_node.is_vertical = 0;
      }
      Direction direction = static_cast<Direction>(i);
      if (isValidNode(next_node)) {
        costFunc(current_node, next_node, direction, x_end, y_end);
        // 1. Find the better path
        // 2. This node is not in open_ list
        if (next_node.F < valueF_[next_node.y][next_node.x] ||
            valueF_[next_node.y][next_node.x] == 0) {
          pre_[next_node.y][next_node.x].x_lb = current_node.x;
          pre_[next_node.y][next_node.x].y_lb = current_node.y;
          valueF_[next_node.y][next_node.x] = next_node.F;
          open_.push(next_node);
        }
      }
    }
  }
}

void Routing::costFunc(const Node current_node, Node& next_node, Direction dir,
                       int x_end, int y_end) {
  /*
  oriCost = \alpha * WL_{total} + \beta * OV_{total} + \gamma * cellCost_{total}
  + \delta * viaCost_{total}
  */
  next_node.G = current_node.G + addCost(current_node, next_node, dir);
  next_node.H = Manhatten(next_node.x, next_node.y, x_end, y_end);
  next_node.F = next_node.G + next_node.H;
}

// left, down, top, right
double Routing::addCost(const Node current_node, const Node next_node,
                        Direction dir) {
  // grid_half
  double wire_length = (dir == top || dir == down) ? grid_.height : grid_.width;
  // capacity overflow
  double overflow = 0;
  int x_c = current_node.x;
  int y_c = current_node.y;
  int x_n = next_node.x;
  int y_n = next_node.y;
  Gcell m1_c = metal1_[y_c][x_c];
  Gcell m2_c = metal2_[y_c][x_c];
  Gcell m1_n = metal1_[y_n][x_n];
  Gcell m2_n = metal2_[y_n][x_n];
  if (dir == left) {  // left
    overflow = overflow + m2_c.left_pass - m2_c.left;
    overflow = overflow + m2_n.right_pass - m2_n.right;
  } else if (dir == down) {  // down
    overflow = overflow + m1_c.bottom_pass - m1_c.bottom;
    overflow = overflow + m1_n.top_pass - m1_n.top;
  } else if (dir == top) {  // top
    overflow = overflow + m1_c.top_pass - m1_c.top;
    overflow = overflow + m1_n.bottom_pass - m1_n.bottom;
  } else if (dir == right) {  // right
    overflow = overflow + m2_c.right_pass - m2_c.right;
    overflow = overflow + m2_n.left_pass - m2_n.left;
  }
  overflow = overflow * 0.5;
  // std::cout << "c is v, n is v\t" << next_node.is_vertical << " " << current_node.is_vertical << std::endl;
  double cell_cost = (dir == down || dir == top)
                         ? metal1_[next_node.y][next_node.x].cost
                         : metal2_[next_node.y][next_node.x].cost;
  double via_cell_cost = (next_node.is_vertical != current_node.is_vertical)
                        ? (metal1_[next_node.y][next_node.x].cost +
                           metal2_[next_node.y][next_node.x].cost) /
                              2
                        : 0;
  double ori_cost = alpha_ * wire_length + beta_ * overflow +
                    gamma_ * cell_cost + gamma_ * via_cell_cost + delta_ * via_cost_;

  return ori_cost;
}

void Routing::retracePath(int x1, int y1) {
  std::vector<std::vector<int>> map_ans;
  map_ans.assign(y_gcell_num_, std::vector<int>(x_gcell_num_, -1));

  int x = x1;
  int y = y1;
  int a, b;

  int x_start, y_start, x_end, y_end;
  x_start = x1;
  y_start = y1;
  x_end = x1;
  y_end = y1;
  while (x != -1 && y != -1) {
    // printf("( %d, %d )\n", x, y);
    if (x == x_start || y == y_start) {  // vertial or horizontal line
      if (x == x_start)                  // vertial
      {
        if (y > y_end) { // now > previous
          metal1_[y_end][x_end].top_pass += 1;
          metal1_[y][x].bottom_pass += 1;
        } else {
          metal1_[y_end][x_end].bottom_pass += 1;
          metal1_[y][x].top_pass += 1;
        }
      } else {  // horizontal
        if (x > x_end) { // now > previous
          metal2_[y_end][x_end].right_pass += 1;
          metal2_[y][x].left_pass += 1;
        } else {
          metal2_[y_end][x_end].left_pass += 1;
          metal2_[y][x].right_pass += 1;
        }
      }
      x_end = x;
      y_end = y;
    }
    map_ans[y][x] = 2;
    a = pre_[y][x].x_lb;
    b = pre_[y][x].y_lb;
    x = a;
    y = b;
    if (a != x_start && b != y_start) {
      Segment seg(x_start, y_start, x_end, y_end);
      segments_.push(seg);
      x_start = x_end;
      y_start = y_end;
    }
  }
  // printGrid(map_ans);
}

void Routing::printGrid(std::vector<std::vector<int>> map_ans) {
  // 假設 map_ans 是一個 2D vector，存儲地圖的狀態
  for (int i = 0; i < y_gcell_num_; ++i) {
    for (int j = 0; j < x_gcell_num_; ++j) {
      // 根據地圖上的值選擇顯示的字符
      switch (map_ans[i][j]) {
        case -1:
          std::cout << " . ";  // 未經過的格子
          break;
        case 0:
          std::cout << "   ";  // 空白格子
          break;
        case 1:
          std::cout << " # ";  // 障礙物
          break;
        case 2:
          std::cout << " @ ";  // 可行路徑
          break;
        default:
          std::cout << " ? ";  // 未知狀態
          break;
      }
    }
    std::cout << std::endl;  // 每行結束後換行
  }

  std::cout << "--------------------------------------------------"
            << std::endl;
  std::cout << std::endl;
}

// map_ans[y][x] = 2; 會印出 @
// 把他這個路徑用一個 segment 存起來，垂直線斷一條 水平線斷一條
//  @  @  @  @  @  @  @  @  @  @  @  @  @  @  @  @  .  .
//  @  .  .  .  .  .  .  .  .  .  .  .  .  .  .  @  .  .
//  @  .  .  .  .  .  .  .  .  .  .  .  .  .  .  @  .  .
//  @  .  .  .  .  .  .  .  .  .  .  .  .  .  .  @  .  .
//  @  .  .  .  .  .  .  .  .  .  .  .  .  .  .  @  .  .
//  @  .  .  .  .  .  .  .  .  .  .  .  .  .  .  @  .  .
//  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  @  .  .
//  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .
//  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .
//  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .
//  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .
//  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .
// --------------------------------------------