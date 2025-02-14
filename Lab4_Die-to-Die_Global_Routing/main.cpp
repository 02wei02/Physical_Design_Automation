#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "routing.h"

int main(int argc, char* argv[]) {
  double time;
  clock_t time_start = clock();
  Routing routing(argc, argv);
  routing.run();
  clock_t time_end = clock();
  time = (double)(time_end - time_start) / CLOCKS_PER_SEC;
  std::cout << "Total time: " << time << std::endl;
  return 0;
}


/*
Search Algorithm A*
(1) Mark source s as "open" and calculate \hat{f}(s)
(2) Select an open node n with smallest \hat{f}, if ties, choose arbitrarily, but always in favor of goal node t \in T
(3) If n belongs to T, mark n "closed" and terminate the Algorithm
(4) Otherwise, mark n closed and apply \Lambda(n), mark each successor as "open" if it is not already marked as "closed". Re-calculate f of each successor of n, if a "closed" successor's f is smaller than f(n), mark it as "open". Finally, go to step 2.

T is the set of terminal points of source s
Define e_{pq} as an arc from node n_p to n_q. if e_{pq} exists, we call n_q a successor of n_p
\Lambda(n) is a function which returns all successors of node n
By graph theory, we can prove that it is impossible for a "closed" successor to have a smaller \hat{f} than the current point. Thus, the 4^{th} step can be written as: Otherwise, mark n "closed" and apply \Lambda(n), mark each successor as "open" if it is not already marked as "closed". Finally, go to step2.
*/

// M1 : Vertical
// M2 : Horizontal