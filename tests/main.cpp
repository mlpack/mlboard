/**
 * @file main.cpp
 */

#include <iostream>

//#define CATCH_CONFIG_MAIN  // catch.hpp will define main()
#define CATCH_CONFIG_RUNNER  // we will define main()
#include "catch.hpp"

int main(int argc, char** argv)
{
  std::cout << "Start testing " << std::endl;

  return Catch::Session().run(argc, argv);
}