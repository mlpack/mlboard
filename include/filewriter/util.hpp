/**
 * @file filewriter/util.hpp
 * @author Jeffin Sam
 */
#ifndef MLBOARD_UTIL_HPP
#define MLBOARD_UTIL_HPP

#include <mlboard/core.hpp>

namespace mlboard {
namespace util {

/**
 * Function to encode images to a string given their paths.
 * 
 * @param filePaths A vector of filepath of images to be encoded.
 * @param encodedImages Output vector to store the encoded string format image.
 */
void EncodeImage(std::vector<std::string>& filePaths,
                 std::vector<std::string>& encodedImages);


void histogramEdges(const std::vector<double>& range,
               size_t countofBins,
               std::vector<double>& edges);

} // namespace util
} // namespace mlboard


#include "util_impl.hpp"

#endif
