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

template<typename filewriter>
class SummaryWriter
{
 public:
  static void scalar(const std::string &tag,
                     int step,
                     double value,
                     fileWriter &fw);
};

} // namespace mlboard

// Include implementation.
#include "summarywriter_impl.hpp"

#endif
