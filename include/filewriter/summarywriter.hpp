/**
 * @file summarywriter.hpp
 * @author Jeffin Sam
 */
#ifndef MLBOARD_SUMMARY_WRITER_HPP
#define MLBOARD_SUMMARY_WRITER_HPP

#include <mlboard/core.hpp>
#include "filewriter.hpp"
#include <proto/summary.pb.h>
#include <proto/projector_config.pb.h>
#include <google/protobuf/text_format.h>

namespace mlboard {

/**
 * Class responsible to create a summary to be logged.
 * 
 * @tparam filewriter The filewriter object whould would convert it
 *    into events and then log to a file. 
 */
template<typename Filewriter>
class SummaryWriter
{
 public:
  /**
   * A function to create a scaler summary.
   * 
   * @param tag Tag to uniquely identify the scaler type.
   * @param step The step at which scaler was logged.
   * @param value Scaler value to be logged.
   * @param fw Filewriter object.
   */
  static void Scalar(const std::string& tag,
                     int step,
                     double value,
                     Filewriter& fw);
  static void Embedding(const std::string& tensorName,
                        const std::string& tensordataPath,
                        Filewriter& fw,
                        const std::string& metadataPath = "",
                        const std::vector<size_t>& tensorShape = std::vector<size_t>());
  static void Embedding(const std::string& tensorName,
                        const arma::mat& tensordata,
                        const std::vector<std::string>& metadata,
                        Filewriter& fw,
                        const std::string& tensordataPath = "",
                        const std::string& metadataPath = "");

};

} // namespace mlboard

// Include implementation.
#include "summarywriter_impl.hpp"

#endif
