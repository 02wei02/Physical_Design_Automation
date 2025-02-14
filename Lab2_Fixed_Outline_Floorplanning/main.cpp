#include <ctime>  // clock def.

#include "b_star_tree.h"  // BStarTree class

int main(int argc, char *argv[]) {
  // Start the time count
  (void)argc;
  std::clock_t time_start, time_end;
  time_start = std::clock();

  BStarTree *tree = new BStarTree(argv[1], argv[2], argv[3], argv[4]);

  // Read input file
  tree->InputFile();

  // Run the Simulated Annealing (SA) algorithm
  tree->SA();

  // Stop the time count
  time_end = std::clock();

  // Calculate and display the total time spent
  double total_time =
      static_cast<double>(time_end - time_start) / CLOCKS_PER_SEC;
  std::cout << "Spend time: " << total_time << "s" << std::endl;

  // Output the result to a file, including the time spent
  tree->OutputFile(total_time);

  // Delete the dynamically allocated BStarTree object
  return 0;
}
