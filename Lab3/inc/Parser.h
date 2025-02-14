#ifndef _PARSER_H_
#define _PARSER_H_

#include <fstream>
#include <iostream>
#include <sstream>  // Needed for std::istringstream

#include "Cell.h"
#include "Manager.h"

class Manager;

class Parser {
 private:
  std::ifstream lg_in;
  std::ifstream opt_in;

 public:
  Parser(const std::string &lg, const std::string &opt);
  ~Parser();
  void parser(Manager &mgr);
  void readWeight(Manager &mgr);
  void readDieSize(Manager &mgr);
  void readCellCoordinate(Manager &mgr);
  void readOptimizeStep(Manager &mgr);
};

#endif  // _PARSER_H_