/**
 * @file filewriter.hpp
 * @author Jeffin Sam
 */
#ifndef MLBOARD_FILE_WRITER_HPP
#define MLBOARD_FILE_WRITER_HPP 

#include <mlboard/core.hpp>
#include "shareablequeue.hpp"
#include <proto/event.pb.h>
#include "crc.hpp"

namespace mlboard {

class fileWriter
{
 public:
 fileWriter(std::string logdir, 
            int maxQueueSize = 10,
            size_t flushsec = 2);

  void writeSummary();

  void createEvent(size_t step, mlboard::Summary *summary);
  void flush();

  void close();

private:
  SharebaleQueue <mlboard::Event> q;
  // std::thread does not have copy constructor hence pointer is safe
  std::thread *thread_;
  std::size_t flushsec;
  std::string logdir;
  std::time_t nexttime;
  std::ofstream outfile;
  bool close_;
};

} // namespace mlboard

// Include implementation.
#include "filewriter_impl.hpp"

#endif