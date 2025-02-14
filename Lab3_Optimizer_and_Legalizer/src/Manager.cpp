#include "Manager.h"

std::random_device rd;
std::default_random_engine gen(rd());

void Manager::Run() {  // for all execution
  time_start = std::clock();
  initialCell2D();
  makeLegalize();
  // outFinalPosition();
  // std::cout << "placement_row_height: " << placement_row_height << std::endl;
  // std::cout << "end y: " << placement_row_end_y << std::endl;
}

Manager::Manager() {
  // Constructor
  alpha = 0.0;
  beta = 0.0;
}

Manager::~Manager() {
  // Destructor
  lg_out.close();
}

void Manager::parser(const std::string &lg, const std::string &opt,
                    const std::string &out) {
  // Manager function
  Parser parser(lg, opt);
  lg_out.open(out);
  if (!lg_out) {
    std::cerr << "Error: Failed to open file " << out << std::endl;
  } else {
    std::cout << "File " << out << " opened successfully." << std::endl;
  }
  parser.parser(*this);
}

void Manager::outputFile(const Cell &cell) {
  // Output file
  lg_out << cell.x << " " << cell.y << std::endl;
  lg_out << "0" << std::endl;
  // lg_out << "no finish" << std::endl;
}

void removeCellFromMultimap(std::multimap<double, Cell> &cells_by_x, double key,
                            const std::string &cellId) {
  // Find the range of elements with the specified key
  auto range = cells_by_x.equal_range(key);

  for (auto it = range.first; it != range.second; ++it) {
    // Check if this is the Cell we want to remove
    if (it->second.name == cellId) {
      cells_by_x.erase(it);  // Erase the specific Cell
      std::cout << "Cell with id " << cellId << " removed from multimap."
                << std::endl;
      return;
    }
  }
  std::cout << "Cell with id " << cellId << " not found in multimap with key "
            << key << "." << std::endl;
}

void Manager::removeCell2D(const std::string &target) {
  // Find the Cell in the map
  // std::cout << "T: " << target << std::endl;
  Cell cell = cells_map[target];
  // Remove the Cell from the multimap
  int num_erased = cell.height / placement_row_height;
  for (int i = 0; i < num_erased; ++i) {
    if (cells_2d.count(cell.y + i * placement_row_height)) {
      auto &inner_erase = cells_2d[cell.y + i * placement_row_height];
      inner_erase.erase(cell.x);
    }
  }
}

void Manager::addCell2D(Cell &target) {
  for (double y_coor = target.y; y_coor < target.y_rt;
      y_coor += placement_row_height) {
    cells_2d[y_coor][target.x] = target;
  }
}

void Manager::makeLegalize() {
  for (const auto &pair : merged_vec) {
    bool is_down = true;
    bool down_ok, up_ok = false;
    // bool is_push;
    std::cout << "#-------------------------------------------#" <<
    std::endl; std::cout << pair.merged_name << std::endl;
    // std::cout << "--------------------------------------------" <<std::endl; 
    FF ff_merged = pair; 
    Cell cell(pair.merged_name, ff_merged.x,
    ff_merged.y, ff_merged.width,
              ff_merged.height, ff_merged.x + ff_merged.width,
              ff_merged.y + ff_merged.height, 2);
    // delete merged cell in cells_map, cells_by_x and cells_by_y

    for (const auto &ff : ff_merged.ff_list) {
      // std::cout << ff << " ";
      removeCell2D(ff);
      cells_map.erase(ff);
    }
    // std::cout << std::endl;
    while (!isLegal(cell)) {
      leftRightLegalize(cell);
      // std::cout << "is_down: " << is_down << std::endl;
      bool is_left = false;
      Cell cell_pop;
      if(!(down_ok && up_ok)){
        if (!q_cell.empty() &&
            !q_is_left.empty()) {     // make sure q_cell and q_isleft is not empty
          cell_pop = q_cell.front();  // get q_cell front
          q_cell.pop();               // pop q_cell front
          // std::cout << "pop: " << cell_pop.name << std::endl;
          is_left = q_is_left.front();  // get q_isleft front
          q_is_left.pop();              // remove q_isleft front
                                        // process c and is_left
        } else {
          // std::cerr << "Error: Queue is empty, cannot pop elements!" <<
          // std::endl;
          if (is_down) {
            cell.y = cell.y - placement_row_height;
            // std::cout << "placement_row_start_y: " << placement_row_start_y << std::endl; 
            if (cell.y < placement_row_start_y) {
              is_down = false;
              down_ok = true;
              cell.y = ff_merged.y + placement_row_height;
            }
          } else {
            cell.y = cell.y + placement_row_height;
            cell.y_rt = cell.y + cell.height;
            // std::cout << "placement_row_end_y: " << placement_row_end_y << std::endl; 
            if (cell.y > placement_row_end_y) {
              is_down = true;
              up_ok = true;
              cell.y = ff_merged.y - placement_row_height;
            }
          }
          cell.y_rt = cell.y + cell.height;
          cell.x = ff_merged.x;
          cell.x_rt = cell.x + cell.width;
          continue;
        }
        if (is_left) {
          cell.x_rt = cell_pop.x;
          cell.x = cell.x_rt - cell.width;
        } else {
          cell.x = cell_pop.x_rt;
          cell.x_rt = cell.x + cell.width;
        }
      }
      else{
        Cell temp_cell = cell;
        bool is_legal = false;
        for(double y_coor = placement_row_end_y; y_coor >= placement_row_start_y ; y_coor -= placement_row_height){
          for(double x_coor = placement_row_start_x; x_coor <= placement_row_end_x; x_coor += placement_row_width){
            cell.x = x_coor;
            cell.x_rt = x_coor + cell.width;
            cell.y = y_coor;
            cell.y_rt = y_coor + cell.height;
            if(isLegal(cell))
            {
              is_legal = true;
              break;
            }
//             if (cell.name == "FF_5_172") {
//   std::cout << "Checking FF_5_172 at position: x=" << cell.x 
//             << ", x_rt=" << cell.x_rt 
//             << ", y=" << cell.y 
//             << ", y_rt=" << cell.y_rt << std::endl;
// }
          } 
          if(is_legal)break;
        }
      }
    }
    addCell2D(cell);
    cells_map[cell.name] = cell;
    outputFile(cell);
    time_end = std::clock();
    duration = double(time_end - time_start) / CLOCKS_PER_SEC;
    // makeWindow(ff_merged);
    // legalAlg(pair.first);
    while (!q_cell.empty()) {
      q_cell.pop();
      q_is_left.pop();
    }
    pushed_cells.clear();
  }
  std::cout << "Elapsed time: " << duration << " seconds" << std::endl;
}






void Manager::outFinalPosition() {
  std::ofstream out("./output.txt");  // Use std::ofstream for writing
  if (out) {
    std::cout << "File opened successfully." << std::endl;
  } else {
    std::cerr << "Failed to open file!" << std::endl;
    return;
  }
  // out << std::fixed << std::setprecision(0);

  // out << "Alpha " << alpha << std::endl;
  // out << "Beta " << beta << std::endl;
  out << "die " << die.x << " " << die.y << " " << die.x_rt << " " << die.y_rt
      << std::endl;

  for (auto &mapcell : cells_map) {
    auto cell = mapcell.second;
    if (cell.fixed == 1)
      out << "1 ";  // fixed
    else if (cell.fixed == 0)
      out << "3 ";  // moved
    else if (cell.fixed == 2)
      out << "2 ";  // merged
    out << cell.x << " " << cell.y << " " << cell.width << " " << cell.height
        << " ";
    out << std::endl;
  }
  for (auto &row : rows_map) {
    out << "0 " << row.second.start_x << " " << row.second.start_y << " "
        << row.second.site_width << " " << row.second.site_height << std::endl;
  }
  out.close();
}

void Manager::initialCell2D() {
  placement_row_height = rows_map.begin()->second.site_height;
  placement_row_width = rows_map.begin()->second.site_width;
  placement_row_end_x =
      rows_map.begin()->second.start_x +
      placement_row_width * (rows_map.begin()->second.num_sites-1);
  if (!rows_map.empty()) {
    auto max_y_it = std::prev(rows_map.end());  // get the last row
    double max_y = max_y_it->first;
    // PlacementRow &max_row = max_y_it->second;
    placement_row_end_y = max_y;
  }

  placement_row_start_x = rows_map.begin()->second.start_x;
  placement_row_start_y = rows_map.begin()->second.start_y;
  for (const auto &cell : cells_map) {
    for (int y_coor = cell.second.y; y_coor < cell.second.y + cell.second.height; y_coor += placement_row_height) {
      cells_2d[y_coor][cell.second.x] =
          cell.second;
    }
  }
}

bool Manager::isLegal(Cell &target_cell) {
  // Quick bounds check against the die
  if (target_cell.y_rt > die.y_rt || target_cell.y < die.y ||
      target_cell.x_rt > die.x_rt || target_cell.x < die.x) {
    return false;
  }
  if(target_cell.x > placement_row_end_x || target_cell.x < placement_row_start_x || target_cell.y > placement_row_end_y || target_cell.y < placement_row_start_y)
    return false;

  // Iterate over all relevant rows for the target_cell
  for (double y_coor = target_cell.y; y_coor < target_cell.y_rt; y_coor += placement_row_height) {
    // Ensure the row exists
    auto row_it = cells_2d.find(y_coor);
    if (row_it == cells_2d.end()) continue;  // Skip if no corresponding row

    auto &row_map = row_it->second;

    // Use `lower_bound` to locate potential overlapping cells
    auto lower_bound_it = row_map.lower_bound(target_cell.x);

    // Check the left cell (if it exists)
    if (lower_bound_it != row_map.begin()) {
      const Cell &left_cell = std::prev(lower_bound_it)->second;
      if (left_cell.x_rt > target_cell.x) {
        return false;  // Overlap with the left cell
      }
    }

    // Check the right cell (if it exists)
    if (lower_bound_it != row_map.end()) {
      const Cell &right_cell = lower_bound_it->second;
      if (right_cell.x < target_cell.x_rt) {
        return false;  // Overlap with the right cell
      }
    }
  }

  // If no overlaps were found, the cell is legal
  return true;
}


void Manager::leftRightLegalize(Cell &target_cell) {
  for (double y_coor = target_cell.y; y_coor < target_cell.y_rt; y_coor += placement_row_height) {
    // Ensure the row exists
    auto row_it = cells_2d.find(y_coor);
    if (row_it == cells_2d.end()) continue;  // Skip if no corresponding row
    
    auto &row_map = row_it->second;

    // Find the lower_bound for the target cell
    auto lower_bound_it = row_map.lower_bound(target_cell.x_rt);

    // Check right cell
    if (lower_bound_it != row_map.end()) {
      const Cell &right_cell = lower_bound_it->second;
      if (pushed_cells.insert(right_cell.name).second && right_cell.x <= placement_row_end_x) {
        q_cell.push(right_cell);
        q_is_left.push(0);  // Right label
      }
    }

    // Check left cell
    if (lower_bound_it != row_map.begin()) {
      auto prev_it = std::prev(lower_bound_it);  // Get the previous element
      const Cell &left_cell = prev_it->second;
      if (pushed_cells.insert(left_cell.name).second && left_cell.x >= placement_row_start_x) {
        q_cell.push(left_cell);
        q_is_left.push(1);  // Left label
      }
    }
  }
}

//------------------------------------------------------------------//
//                               Fail                               //
//------------------------------------------------------------------//    

/*
algorithm 2 legal placement realization
cell ct is placed at (xt, yt)
QL={ct}
while QL is not empty do
  ci = QL.pop()
  for each left neighboring cell cL of ci do
    if cL overlaps ci then
      xL = xi - wL
      QL.push(xl)
    end if
  end for
end while
QR = {ct}
while QR is not empty do
  ci = QR.pop()
  for each right neighboring cell cR of ci do
    if cR overlaps ci then
      xR = xi + wi
      QR.push(xR)
    end if
  end for
end
*/
void Manager::legalAlg(const std::string &ff) {
  Cell ct = cells_map[ff];  // Assuming `ct` is initialized at (xt, yt)

  // Initialize queues
  std::queue<Cell> QL, QR;
  std::set<std::string> visited;  // Track visited cells by ID
  std::map<std::string, std::pair<double, double>>
      final_positions;  // To store final x and y of cells
  int moved_count = 0;  // Counter for moved cells

  QL.push(ct);
  QR.push(ct);
  visited.insert(ct.name);  // Add the initial cell to visited set

  // Legalize left side
  while (!QL.empty()) {
    Cell ci = QL.front();
    QL.pop();

    // Iterate through cells left of ci within window bounds
    auto range_left_start = cells_by_x.lower_bound(
        ci.x - ci.width);  // Start from `ci.x - ci.width`
    auto range_left_end = cells_by_x.upper_bound(ci.x);  // End at `ci.x`

    for (auto it = range_left_start; it != range_left_end; ++it) {
      Cell &cL = it->second;
      if (visited.count(cL.name) || cells_map[cL.name].fixed == 1)
        continue;  // Skip if already processed
      // std::cout << "ok " << cL.name << std::endl;
      // Check overlap condition
      if (!(cL.x + cL.width <= ci.x || cL.y + cL.height <= ci.y ||
            ci.x + ci.width <= cL.x || ci.y + ci.height <= cL.y)) {
        // Adjust cL to the left
        double old_x = cL.x;  // Record old position
        cL.x = ci.x - cL.width;
        if (cL.x != old_x) {
          moved_count++;  // Increment counter if position changes
          final_positions[cL.name] = std::make_pair(cL.x, cL.y);
        }

        QL.push(cL);
        visited.insert(cL.name);  // Track the cell as visited
      }
    }
  }

  // Legalize right side
  while (!QR.empty()) {
    Cell ci = QR.front();
    QR.pop();

    // Iterate through cells right of ci within window bounds
    auto range_right_start = cells_by_x.lower_bound(
        ci.x + ci.width);  // Start from `ci.x + ci.width`
    auto range_right_end = cells_by_x.upper_bound(
        ci.x + 2 * ci.width);  // Adjust upper bound as needed

    for (auto it = range_right_start; it != range_right_end; ++it) {
      Cell &cR = it->second;
      if (visited.count(cR.name) || cells_map[cR.name].fixed == 1) continue;

      // Check overlap condition
      if (!(cR.x + cR.width <= ci.x || cR.y + cR.height <= ci.y ||
            ci.x + ci.width <= cR.x || ci.y + ci.height <= cR.y)) {
        // Adjust cR to the right
        double old_x = cR.x;  // Record old position
        cR.x = ci.x + ci.width;
        if (cR.x != old_x) {
          moved_count++;  // Increment counter if position changes
          final_positions[cR.name] = std::make_pair(cR.x, cR.y);
        }
        QR.push(cR);
        visited.insert(cR.name);  // Track the cell as visited
      }
    }
  }

  // Print results
  std::cout << "Number of moved cells: " << moved_count << "\n";
  lg_out << moved_count << std::endl;
  for (auto it = final_positions.begin(); it != final_positions.end(); ++it) {
    // std::cout << it->first << ", Final X: " << it->second.first << ", Final
    // Y: " << it->second.second << "\n";
    lg_out << it->first << " " << it->second.first << " " << it->second.second
          << std::endl;
  }
}

void Manager::makeWindow(const FF &ff) {
  PlacementRow find_row = rows_map[ff.y];
  // double window_ratio = find_row.site_height/find_row.site_width;
  double y_window = 2 * find_row.site_height;
  double x_window = y_window * 5;
  auto y_up = rows_map.upper_bound(find_row.start_y + y_window);
  auto y_low = rows_map.lower_bound(find_row.start_y - y_window);
  double window_y_upper, window_y_lower;
  // Check if the iterator is valid (not equal to end)
  if (y_up != rows_map.end()) {
    // Dereference the iterator to access the key
    window_y_upper = y_up->first;
    std::cout << "Upper bound y: " << window_y_upper << std::endl;
  } else {
    std::cout << "No upper bound found.\n";
  }
  if (y_low != rows_map.end()) {
    // Dereference the iterator to access the key
    window_y_lower = y_low->first;
    std::cout << "Lower bound y: " << window_y_lower << std::endl;
  } else {
    std::cout << "No lower bound found.\n";
  }
  window_x_right = ff.x + x_window;
  window_x_left = ff.x - x_window;
  std::cout << "x_left: " << window_x_left << " x_right: " << window_x_right
            << std::endl;
}