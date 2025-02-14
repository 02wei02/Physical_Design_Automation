#include "Parser.h"

Parser::Parser(const std::string &lg, const std::string &opt) {
  // Constructor
  lg_in.open(lg);
  opt_in.open(opt);
  if (lg_in.fail()) {
    std::cerr << "Failed to open lg file: " << lg << std::endl;
  }

  if (opt_in.fail()) {
    std::cerr << "Failed to open opt file: " << opt << std::endl;
  }
}

Parser::~Parser() {
  // Destructor
  lg_in.close();
  opt_in.close();
}

void Parser::parser(Manager &mgr) {
  // Parse input file
  readWeight(mgr);
  readDieSize(mgr);
  readCellCoordinate(mgr);
  readOptimizeStep(mgr);
}

void Parser::readWeight(Manager &mgr) {
  // Read weight from input file
  std::string buf;
  lg_in >> buf >> mgr.alpha;
  lg_in >> buf >> mgr.beta;
}

void Parser::readDieSize(Manager &mgr) {
  // Read die size from input file
  std::string buf;
  double lb_x, lb_y, rt_x, rt_y;
  lg_in >> buf >> lb_x >> lb_y >> rt_x >> rt_y;
  mgr.setDieSize(lb_x, lb_y, rt_x, rt_y);
}

void Parser::readCellCoordinate(Manager &mgr) {
  std::string buf, line;
  double x, y, width, height;
  int attr;

  // Read each line from lg_in and process it
  while (getline(lg_in, line)) {
    std::istringstream lineStream(line);  // Wrap line in istringstream
    lineStream >> buf >> x >> y >> width >> height;

    if (buf[0] == 'F' || buf[0] == 'C') {
      if (buf[0] == 'F') {
        attr = 0;
      } else {
        attr = 1;
      }
      Cell cell(buf, x, y, width, height, x + width, y + height, attr);
      mgr.cells_map[buf] = cell;
      mgr.cells_by_x.emplace(std::make_pair(x, cell));
      mgr.cells_by_y.emplace(std::make_pair(y, cell));
    } else if (buf == "PlacementRows") {
      int num_sites;
      lineStream >> num_sites;  // Read the remaining data
      PlacementRow row(x, y, width, height, num_sites);
      mgr.rows_map[y] = row;
    }
  }
  // std::cout << mgr.cells_map["FF_1_0"].x << " " << mgr.cells_map["FF_1_0"].y
  // << std::endl;
}

void Parser::readOptimizeStep(Manager &mgr) {
  // Read optimize step from input file
  std::string buf;
  std::vector<std::string> list_of_ff;
  double x, y, width, height;
  while (opt_in >> buf) {
    while (opt_in >> buf) {
      if (buf == "-->") {
        break;
      }
      list_of_ff.push_back(buf);
    }
    opt_in >> buf >> x >> y >> width >> height;
    mgr.merged_vec.emplace_back(FF(buf, x, y, width, height, list_of_ff));
    list_of_ff.clear();
  }
}