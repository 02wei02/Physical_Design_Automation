#include "b_star_tree.h"

std::random_device rd;
std::default_random_engine gen(rd());

double BStarTree::CostFunc() {
  int width_error = std::max(0, width_ - outline_width_);
  int height_error = std::max(0, height_ - outline_height_);

  // If no dimension error, compute area cost with penalty factor
  if (width_error == 0 && height_error == 0) {
    int area = width_ * height_;
    int wire_length = HPWL();
    int weighted_cost = alpha_ * area + (1 - alpha_) * wire_length;
    return -(outline_width_ * outline_height_ * 5 / weighted_cost);
  }
  // Compute penalty for layout exceeding outline dimensions
  return width_error * width_error + height_error * height_error;
}

void BStarTree::OutputFile(double total_time) {
  std::ofstream output(output_str);  // Create an output file stream
  XY();                              // Call the coordinate function

  int wirelength = HPWL();  // Calculate half-perimeter wire length
  int area = AREA();        // Calculate area
  int cost = Cost();        // Calculate cost

  // Write results to the output file
  output << cost << std::endl;
  output << wirelength << std::endl;
  output << area << std::endl;
  output << width_ << " " << height_ << std::endl;
  output << total_time << std::endl;

  // Output node information
  for (size_t i = 0; i < node_vec_.size(); i++) {
    Node *tmp_node = node_vec_[i];  // Get the current node
    output << tmp_node->name << " " << tmp_node->lb_x << " " << tmp_node->lb_y
           << " " << tmp_node->rt_x << " " << tmp_node->rt_y
           << std::endl;  // Write node details to the file
  }
}

void BStarTree::InputFile() {
  int num_terminal, num_net;
  std::string buf;  // Use std::string instead of string
  alpha_ =
      std::stod(alpha_str);  // Use std::stod for string to double conversion
  std::ifstream input_block(inblock_str);  // Open block input file
  std::ifstream input_net(innet_str);      // Open net input file

  // Block input
  input_block >> buf >> outline_width_ >>
      outline_height_;                 // Read outline dimensions
  input_block >> buf >> block_num_;    // Read number of blocks
  input_block >> buf >> num_terminal;  // Read number of terminals

  int width, height;
  std::string name;
  for (int i = 0; i < block_num_; i++) {
    input_block >> name >> width >>
        height;  // Read block name, width, and height
    Node *tmp_node = new Node(width, height, i, name);  // Create new node
    Insert(tmp_node);  // Insert node into the tree
  }
  CreateNodeList();  // Create node vector

  int x, y;
  for (int i = 0; i < num_terminal; ++i) {
    input_block >> name >> buf >> x >> y;  // Read terminal name and coordinates
    Terminal *terminals = new Terminal(name, x, y);  // Create new terminal
    terminal_map_.insert(std::pair<std::string, Terminal *>(
        name, terminals));  // Insert terminal into map
  }

  input_block.close();  // Close block input file

  input_net >> buf >> num_net;  // Read number of nets
  for (int i = 0; i < num_net; i++) {
    int net_degree;
    input_net >> buf >> net_degree;    // Read net degree
    net_vec_.emplace_back(new Net());  // Create new net
    for (int j = 0; j < net_degree; j++) {
      std::string connect_name;   // Use std::string for connect_name
      input_net >> connect_name;  // Read connection name
      net_vec_[i]->net_connect_name.emplace_back(
          connect_name);  // Add to net's connection list
    }
  }
  input_net.close();  // Close net input file
}

int BStarTree::Cost() {
  int area = AREA();        // Calculate area
  int wirelength = HPWL();  // Calculate wirelength
  return static_cast<int>(
      (alpha_ * area) +
      ((1 - alpha_) * wirelength));  // Calculate and return total cost
}

int BStarTree::AREA() {
  return height_ * width_;  // Calculate and return area
}

int BStarTree::HPWL() {
  int total_length = 0;
  for (const auto &net : net_vec_) {
    int min_x = INT_MAX, max_x = 0, min_y = INT_MAX, max_y = 0;
    for (const auto &block_name : net->net_connect_name) {
      int x = 0, y = 0;
      if (block_map_.count(block_name)) {
        Node *block = block_map_[block_name];
        x = block->lb_x + block->width / 2;
        y = block->lb_y + block->height / 2;
      } else if (terminal_map_.count(block_name)) {
        Terminal *terminal = terminal_map_[block_name];
        x = terminal->x;
        y = terminal->y;
      } else {
        continue;  // Skip if block/terminal not found
      }
      min_x = std::min(min_x, x);
      max_x = std::max(max_x, x);
      min_y = std::min(min_y, y);
      max_y = std::max(max_y, y);
    }
    total_length += (max_x - min_x) + (max_y - min_y);
  }
  return total_length;
}

void BStarTree::SA() {
  double T = MAX_TEMP;
  uniform_real_distribution<> disexp(0.0, 1.0);
  start_time_ = clock();
  double cost_now;
  double cost_after;
  double prob;
  int loc_rotate_block;
  Node *refresh_parent;
  bool taken_from_left;
  int loc_block1;
  int loc_block2;

  while (true) {
    for (int i = 0; i < 1000; ++i) {
      XY();
      cost_now = CostFunc();
      uniform_int_distribution<> dis(1, 3);
      int op_swap = dis(gen);

      if (op_swap == 1) {
        uniform_int_distribution<> dis(0, block_num_ - 1);
        loc_rotate_block = dis(gen);
        Rotate(loc_rotate_block);
      } else if (op_swap == 2) {
        uniform_int_distribution<> dis1(0, block_num_ - 1);
        uniform_int_distribution<> dis2(0, block_num_ - 1);
        loc_block1 = dis1(gen);
        loc_block2 = dis2(gen);
        while ( (node_vec_[loc_block1]->left != nullptr) ||
               (node_vec_[loc_block1]->right != nullptr) || (node_vec_[loc_block1] == root)
               )
          loc_block1 = dis1(gen);
        while (((node_vec_[loc_block2]->left != nullptr) &&
                (node_vec_[loc_block2]->right != nullptr)) || (node_vec_[loc_block2] == node_vec_[loc_block1]))
          loc_block2 = dis2(gen);
        refresh_parent = node_vec_[loc_block1]->parent;
        if (refresh_parent->left == node_vec_[loc_block1])
          taken_from_left = true;
        else
          taken_from_left = false;

        MoveNode(loc_block1, loc_block2);
      } else if (op_swap == 3) {
        uniform_int_distribution<> dis_swap_1(0, block_num_ - 1);
        uniform_int_distribution<> dis_swap_2(0, block_num_ - 1);
        loc_block1 = dis_swap_1(gen);
        loc_block2 = dis_swap_2(gen);
        while (node_vec_[loc_block1] == node_vec_[loc_block2])
          loc_block2 = dis_swap_2(gen);

        Swap(loc_block1, loc_block2);
      }
      XY();
      cost_after = CostFunc();
      prob = disexp(gen);
      if (cost_now < cost_after &&
          prob >= exp((double)(cost_now - cost_after) / T)) {
        if (op_swap == 1) {
          Rotate(loc_rotate_block);
        } else if (op_swap == 2) {
          if (node_vec_[loc_block2]->left == node_vec_[loc_block1])
            node_vec_[loc_block2]->left = nullptr;
          else if (node_vec_[loc_block2]->right == node_vec_[loc_block1])
            node_vec_[loc_block2]->right = nullptr;
          else
            cout << "Error: swap 2 blocks !" << endl;
          if (taken_from_left)
            refresh_parent->left = node_vec_[loc_block1];
          else
            refresh_parent->right = node_vec_[loc_block1];

          node_vec_[loc_block1]->parent = refresh_parent;
        } else if (op_swap == 3) {
          Swap(loc_block1, loc_block2);
        }
      }
    }
    T *= 0.6;
    end_time_ = clock();
    time_ = static_cast<double>(end_time_ - start_time_) / CLOCKS_PER_SEC;
    cout << time_ << "s\tTemp: " << T  << "\tCost:" << Cost()<< endl;
    if (T < MIN_TEMP &&
        ((width_ <= outline_width_) && (height_ <= outline_height_)))
      break;
    if (time_ > 280 && (width_ <= outline_width_) &&
        (height_ <= outline_height_))
      break;
  }
}

void BStarTree::MoveNode(int source_node_id, int target_node_id) {
  Node *source_node = node_vec_[source_node_id];
  Node *target_node = node_vec_[target_node_id];

  // Detach source node from its parent
  Node *parent_node = source_node->parent;
  if (parent_node) {
    if (parent_node->left == source_node) {
      parent_node->left = nullptr;
    } else if (parent_node->right == source_node) {
      parent_node->right = nullptr;
    } else {
      cout << "Error: Invalid source node!" << endl;
      return;
    }
  }

  // Attach source node to target node
  if (!target_node->left) {
    target_node->left = source_node;
  } else if (!target_node->right) {
    target_node->right = source_node;
  } else {
    cout << "Error: Target node has no available child slot!" << endl;
    return;
  }

  // Update the parent reference of the moved node
  source_node->parent = target_node;
}

void BStarTree::Swap(int index_1, int index_2) {
  Node *node_1 = node_vec_[index_1];
  Node *node_2 = node_vec_[index_2];

  // Update the block_map_ to reflect the swap in node pointers
  std::swap(block_map_[node_1->name], block_map_[node_2->name]);

  // Swap attributes directly using std::swap for each field
  std::swap(node_1->name, node_2->name);
  std::swap(node_1->width, node_2->width);
  std::swap(node_1->height, node_2->height);
  std::swap(node_1->lb_x, node_2->lb_x);
  std::swap(node_1->lb_y, node_2->lb_y);
  std::swap(node_1->rt_x, node_2->rt_x);
  std::swap(node_1->rt_y, node_2->rt_y);
}

void BStarTree::Rotate(int index) {
  Node *x = node_vec_[index];
  std::swap(x->height, x->width);
}

void BStarTree::XY(Node *leaf) {
  if (!leaf) return;

  if (leaf == root) {
    contour_.assign(leaf->width, 0);  // Resize contour directly to leaf width
    leaf->lb_x = leaf->lb_y = 0;
    width_ = height_ = 0;
  } else {
    Node *parent = leaf->parent;
    leaf->lb_x = (parent->left == leaf) ? parent->rt_x : parent->lb_x;
    leaf->lb_y = 0;

    // Ensure `contour_` has enough elements and calculate `lb_y`
    if (contour_.size() < static_cast<size_t>(leaf->lb_x + leaf->width)) {
      contour_.resize(leaf->lb_x + leaf->width, 0);
    }
    for (int i = leaf->lb_x; i < leaf->lb_x + leaf->width; ++i) {
      leaf->lb_y = std::max(leaf->lb_y, contour_[i]);
    }
  }

  leaf->rt_x = leaf->lb_x + leaf->width;
  leaf->rt_y = leaf->lb_y + leaf->height;
  width_ = std::max(width_, leaf->rt_x);
  height_ = std::max(height_, leaf->rt_y);

  block_map_.emplace(leaf->name, leaf);

  // Update contour with the height of the current leaf
  for (int i = leaf->lb_x; i < leaf->rt_x; ++i) {
    contour_[i] = std::max(contour_[i], leaf->rt_y);
  }

  // Recurse for child nodes
  XY(leaf->left);
  XY(leaf->right);
}

void BStarTree::Insert(Node *in) {
  if (in == nullptr) {
    std::cout << "Error: Attempting to insert a null node." << std::endl;
    return;  // Handle the case where the input node is null
  }
  if (root == nullptr) {
    // Initialize the root node
    root = new Node(in->width, in->height, in->name);
  } else {
    // Insert the new node into the tree
    Insert(in, root);
  }
}

void BStarTree::Insert(Node *in, Node *leaf) {
  if (in->width < leaf->width) {
    if (leaf->left == nullptr) {
      leaf->left = new Node(in->width, in->height, in->name, leaf);
    } else {
      Insert(in, leaf->left);
    }
  } else if (in->width >= leaf->width) {
    if (leaf->right == nullptr) {
      leaf->right = new Node(in->width, in->height, in->name, leaf);
    } else {
      Insert(in, leaf->right);
    }
  }
}

void BStarTree::CreateNodeList(Node *leaf) {
    if (!leaf) return;
    // In-order traversal to populate node_vec_
    CreateNodeList(leaf->left);
    node_vec_.emplace_back(leaf);
    CreateNodeList(leaf->right);
}