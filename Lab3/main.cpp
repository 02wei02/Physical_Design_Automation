#include <iostream>

#include "Manager.h"

int main(int argc, char *argv[]) {
  Manager mgr;
  mgr.parser(argv[1], argv[2], argv[3]);
  mgr.Run();
  return 0;
}