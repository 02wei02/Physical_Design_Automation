#ifndef _B_STAR_TREE_H_
#define _B_STAR_TREE_H_

#include <algorithm>
#include <climits>  // Include this header for INT_MAX
#include <fstream>
#include <iostream>  // For input and output operations
#include <map>       // For std::map
#include <random>    // For random number generation
#include <string>    // For std::string
#include <utility>   // For std::move
#include <vector>    // For std::vector

using namespace std;

// Constants for the Simulated Annealing algorithm
#define MAX_TEMP 5e4   // Maximum temperature
#define MIN_TEMP 1e-4  // Minimum temperature
#define PANALTY 100    // Penalty constant

// Declare external random device and generator, defined elsewhere
extern std::random_device
    rd;  // Random device for non-deterministic random numbers
extern std::default_random_engine gen;  // Random number generator

// Structure representing a net with names of connections
struct Net {
  std::vector<std::string>
      net_connect_name;  // Names of nodes connected by the net
};

// Structure representing a node in the B*-tree
struct Node {
  int width = 0;           // Width of the node
  int height = 0;          // Height of the node
  int id = 0;              // Unique identifier for the node
  int lb_x = 0, lb_y = 0;  // Coordinates of the lower-left corner
  int rt_x = 0, rt_y = 0;  // Coordinates of the upper-right corner
  std::string name;        // Name of the node, default is "default"
  Node* parent = nullptr;  // Pointer to the parent node
  Node* left = nullptr;    // Pointer to the left child node
  Node* right = nullptr;   // Pointer to the right child node

  // Default constructor
  Node() : name("default") {};

  // Constructor with parameters for initializing node properties
  Node(int w, int h, const std::string& name)
      : width(w), height(h), name(name) {}
  Node(int w, int h, int index, const std::string& name)
      : width(w), height(h), id(index), name(name) {}
  Node(int w, int h, string name, Node* parent)
      : width(w), height(h), name(name), parent(parent) {}
};

// Structure representing a terminal with coordinates
struct Terminal {
  std::string name;  // Name of the terminal
  int x;             // X coordinate of the terminal
  int y;             // Y coordinate of the terminal

  // Constructor to initialize terminal properties
  Terminal(const std::string& label, int x, int y) : name(label), x(x), y(y) {}
};

// Class representing the B*-Tree structure and operations
class BStarTree {
 public:
  // Constructor that initializes the BStarTree with input parameters
  BStarTree(std::string alpha, std::string inblock, std::string innet,
            std::string outfile)
      : alpha_str(std::move(alpha)),  // Move alpha string to member variable
        inblock_str(
            std::move(inblock)),      // Move inblock string to member variable
        innet_str(std::move(innet)),  // Move innet string to member variable
        output_str(
            std::move(outfile)),  // Move output string to member variable
        root(nullptr) {}          // Initialize root to nullptr

  // Destructor to clean up the tree
  ~BStarTree() { ClearTree(); }
  // Method declarations for various B*-Tree operations
  void Insert(Node* in);               // Method to Insert a node
  void SA();                           // Method to perform simulated annealing
  void InputFile();                    // Method to read input from a file
  void OutputFile(double total_time);  // Method to output results to a file
  void CreateNodeList() {
    CreateNodeList(root);
  };                                      // Method to manage a vector of nodes
  void ClearTree() { ClearTree(root); };  // Method to destroy the tree
  void ClearTree(Node* leaf) {
    if (leaf != nullptr) {     // Check if leaf is not null
      ClearTree(leaf->left);   // Recursively destroy left subtree
      ClearTree(leaf->right);  // Recursively destroy right subtree
      delete leaf;             // Delete the current node
    }
  };                     // Recursively destroy the tree from a specific leaf
  void Rotate(int key);  // Rotate operation (Operation 1)
  void MoveNode(int move_node_id,
                int destinate_node_id);     // Move operation (Operation 2)
  void Swap(int swap_id_1, int swap_id_2);  // Swap operation (Operation 3)
  double CostFunc();                        // Calculate cost
  int HPWL();               // Calculate Half-Perimeter Wire Length (HPWL)
  int AREA();               // Calculate area
  int Cost();               // Calculate real cost
  void XY() { XY(root); };  // Calculate XYs for nodes
  void XY(Node* leaf);      // Calculate coordinates for a specific leaf node
  void CreateNodeList(Node* leaf);    // Manage nodes from a specific leaf
  void Insert(Node* in, Node* leaf);  // Insert a node into the specified leaf

 private:
  // Member variables to hold input/output strings and parameters
  std::string alpha_str, inblock_str, innet_str,
      output_str;       // Input and output strings
  double alpha_;        // Cooling rate for simulated annealing
  int height_, width_;  // Dimensions of the chip
  Node* root;           // Pointer to the root node of the tree
  int block_num_, outline_width_,
      outline_height_;           // Block count and dimensions
  std::vector<Node*> node_vec_;  // List of nodes
  std::vector<Net*> net_vec_;    // List of nets
  std::vector<int> contour_;     // Contour line data

  // Maps for quick lookup of terminals and blocks
  std::map<std::string, Terminal*>
      terminal_map_;                        // Map to find terminals by name
  std::map<std::string, Node*> block_map_;  // Map to find blocks by name
  time_t start_time_, end_time_;
  double time_;
};

#endif  // _B_STAR_TREE_H_
