#include <iostream>

#include "parser.h"

using namespace std;

/*************************************/
/*          main design              */
/*************************************/
int main(int argc, char *argv[]) {
  Parser parser(argv[1], argv[2]);
  parser.Run();
  return 0;
}
