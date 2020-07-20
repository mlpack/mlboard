/**
 * @file filewriter/util_impl.hpp
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

inline void histogramEdges(const std::vector<double>& range,
                           size_t countofBins,
                           std::vector<double>& edges)
{
        edges.clear();
        double value = range[0];
        double width = (range[1] - range[0]) / countofBins;
        if (width == 0)
            width = 1;
        while (value <= range[1]) {
            edges.push_back(value);
            value = value + width;
        }
}
} // namespace util
} // namespace mlboard

#endif
