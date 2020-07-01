/**
 * @file main.cpp
 * @author Jeffin Sam
 */

#include <iostream>
#include <unistd.h>
#include <dirent.h>

//#define CATCH_CONFIG_MAIN  // catch.hpp will define main().
#define CATCH_CONFIG_RUNNER  // We will define main().
#include "catch.hpp"

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
      int status = rmdir(entry->d_name);
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

  removeTempDirs();

  return testStatus;
}
