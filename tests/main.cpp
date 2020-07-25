/**
 * @file tests/main.cpp
 * @author Jeffin Sam
 */

#include <iostream>
#include <mlboard/core.hpp>
#include <mlboard/mlboard.hpp>
#include <cstdio>
#include <sys/stat.h>
#include <dirent.h>

//#define CATCH_CONFIG_MAIN  // catch.hpp will define main().
#define CATCH_CONFIG_RUNNER  // We will define main().
#include "catch.hpp"

// For windows rmdir.
#ifdef _WIN32
  #include <io.h>
  #include <direct.h>
#endif

void removeTempDirs()
{
  std::regex regExp("(_temp)(.*)");

  struct dirent *entry;
  DIR *dir = opendir("./");

  if (dir == NULL)
  {
    return;
  }
  while ((entry = readdir(dir)) != NULL)
  {
    if (entry->d_name != "." && entry->d_name != ".."
      && std::regex_match(entry->d_name, regExp) == true)
    {
      int status; 
      #if defined(_WIN32)
        status = _rmdir(entry->d_name);
      #else
        status = rmdir(entry->d_name);
      #endif
      if (status == -1)
      {
        std::cout << "Error while removing temp directory: " << entry->d_name
            << std::endl;
      }
    }
  }
  closedir(dir);
}

int main(int argc, char** argv)
{
  std::cout << "Start testing " << std::endl;

  int testStatus = Catch::Session().run(argc, argv);

  #ifndef KEEP_TEST_LOGS
    removeTempDirs();
  #endif


  return testStatus;
}
