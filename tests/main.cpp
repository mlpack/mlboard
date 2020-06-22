/**
 * @file main.cpp
 * @author Jeffin Sam
 */

#include <iostream>
#include <mlboard/core.hpp>

//#define CATCH_CONFIG_MAIN  // catch.hpp will define main().
#define CATCH_CONFIG_RUNNER  // We will define main().
#include "catch.hpp"

int main(int argc, char** argv)
{
  // GOOGLE_PROTOBUF_VERIFY_VERSION;
  std::cout << "Start testing " << std::endl;
  // google::protobuf::ShutdownProtobufLibrary();
  return Catch::Session().run(argc, argv);
}