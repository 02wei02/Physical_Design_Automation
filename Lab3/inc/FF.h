#ifndef _FF_H_
#define _FF_H_

#include <iostream>
#include <vector>

class FF {
 public:
  std::string merged_name;
  double x, y, width, height;
  std::vector<std::string> ff_list;

  FF();
  FF(std::string merged_name, double x, double y, double width, double height,
     std::vector<std::string> ff_list);
  ~FF();
};

#endif  // _FF_H_