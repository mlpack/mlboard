/**
 * @file summarywriter.hpp
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
};

} // namespace mlboard

// Include implementation.
#include "summarywriter_impl.hpp"

#endif
