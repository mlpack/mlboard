/**
 * @file util.hpp
 * @author Jeffin Sam
 */
#ifndef MLBOARD_UTIL_HPP
#define MLBOARD_UTIL_HPP

#include <mlboard/core.hpp>

namespace mlboard {
namespace util {

/**
 * Function to encode images at a given path to a string
 * 
 * @param filePaths A vetor of filepath of images to be encoded.
 * @param encodedImages Output vector to store the encoded string format image.
 */
void EncodeImage(vector<std::string>& filePaths,
                 std::vector<std::string>>& encodedImages)

} // namespace util
} // namespace mlboard


#include "util_impl.hpp"

#endif
