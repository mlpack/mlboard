/**
 * @file filewriter.hpp
 * @author Jeffin Sam
 */
#ifndef MLBOARD_FILE_WRITER_HPP
#define MLBOARD_FILE_WRITER_HPP

#include <mlboard/core.hpp>
#include "sharedqueue.hpp"
#include <proto/event.pb.h>
#include "crc.hpp"

namespace mlboard {

/**
 * Class resposnible for writing the event to a event file. The writing is a
 * async operation which is running a separte thread. Look writeSummary()
 * for more information.
 */
class fileWriter
{
 public:
 /**
  * Constructor Responsible for filewriter object.
  * 
  * @param logdir Path to store the log files.
  * @param maxQeueSize The maximum number of event to be store at a time.
  * @param flushec Interval to perform the writing operation.
  */
  fileWriter(std::string logdir,
            int maxQueueSize = 10,
            size_t flushsec = 2);

  /**
   * A function to write the event in queue to event files. This function
   * is intialised as a thread to complete the function without hindering
   * any other option.
   */
  void writeSummary();

  /**
   * A helper function to chnage summary to event. The function should be
   * always be there, since it is called from summary instance.
   * 
   * @param summary Summary which you want to convert to event type.
   * @param step The step number associated with the summary.
   */
  void createEvent(size_t step, mlboard::Summary *summary);

  /**
   * A function to flush everything succefully and close the thread.
   */
  void flush();

  /** 
   * Responsible for indicating the class to close the thread.
   */ 
  void close();

  //! Get the path of log directory.
  std::string logDir() const { return logdir; }
 private:
  SharedQueue <mlboard::Event> q;
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
