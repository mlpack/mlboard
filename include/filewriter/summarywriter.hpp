/**
 * @file filewriter.hpp
 * @author Jeffin Sam
 */
#ifndef MLBOARD_SUMMARY_WRITER_HPP
#define MLBOARD_SUMMARY_WRITER_HPP

#include <mlboard/core.hpp>
#include "filewriter.hpp"
#include <proto/summary.pb.h>

namespace mlboard {

/**
 * Class responsible to create a summary to be logged.
 * 
 * @tparam filewriter The filewriter object whould would convert it
 *    into events and then log to a file. 
 */
template<typename filewriter>
class SummaryWriter
{
 public:
  /**
   * A function to create a scaler summary.
   * 
   * @param tag Tag to uniquely identify the scaler type.
   * @param step The step at which scaler was logged.
   * @param value Scaler value to be logged.
   * @param fw filewriter object.
   */
  static void scalar(const std::string& tag,
                     int step,
                     double value,
                     filewriter& fw);

  static void image(const std::string& tag,
                    int step,
                    const std::string& encodedImage,
                    int height,
                    int width,
                    int channel,
                    filewriter& fw,
                    const std::string& displayName = "",
                    const std::string& description = "");

  static void image(const std::string& tag,
                    int step,
                    const std::vector<std::string>& encodedImages,
                    int height,
                    int width,
                    filewriter& fw,
                    const std::string& displayName = "",
                    const std::string& description = "");
  
  template<typename eT>
  static void image(const std::string& tag,
                    int step,
                    arma::Mat<eT>& matrix,
                    mlpack::data::ImageInfo& info,
                    filewriter& fw,
                    const std::string& displayName = "",
                    const std::string& description = "");

};

} // namespace mlboard

// Include implementation.
#include "summarywriter_impl.hpp"

#endif
