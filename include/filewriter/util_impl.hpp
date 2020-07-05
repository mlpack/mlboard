/**
 * @file util_impl.hpp
 * @author Jeffin Sam
 */
#ifndef MLBOARD_UTIL_IMPL_HPP
#define MLBOARD_UTIL_IMPL_HPP

#include <mlboard/core.hpp>
#include "util.hpp"

namespace mlboard {
namespace util {

inline void EncodeImage(std::vector<std::string>& filePaths,
                 std::vector<std::string>& encodedImages)
{
  std::ifstream fin;
  encodedImages.resize(filePaths.size());
  for (size_t i = 0; i < encodedImages.size(); i++)
  {
    std::ostringstream ss;
    fin.open(filePaths[i], std::ios::binary);
    ss << fin.rdbuf();
    encodedImages[i] = ss.str();
    fin.close();
  }
}

} // namespace util
} // namespace mlboard

#endif
