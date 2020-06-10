/**
 * @file filewriter.hpp
 * @author Jeffin Sam
 */
#ifndef MLBOARD_FILE_WRITER_HPP
#define MLBOARD_FILE_WRITER_HPP

#include "../core.hpp"
#include "../proto.hpp"
#include "sharedqueue.hpp"

#include "crc.hpp"

namespace mlboard {

/**
 * Class responsible for writing the event to a event file. The writing is a
 * async operation which is running a separte thread. Look writeSummary()
 * for more information.
 */
class FileWriter
{
 public:
 /**
  * Constructor Responsible for filewriter object.
  *
  * @param logdir Path to store the log files.
  * @param maxQeueSize The maximum number of event to be store at a time.
  * @param flushmilis Interval to perform the writing operation (milliseconds).
  */
  FileWriter(std::string logdir,
             int maxQueueSize = 10,
             size_t flushmilis = 5000);

  /**
   * A function to write the event in queue to event files. This function
   * is intialised as a thread to complete the function without hindering
   * any other option.
   */
  void WriteSummary();

  /**
   * A helper function to change summary to event. The function should
   * always be there, since it is called from the summary instance.
   *
   * @param summary Summary which you want to convert to event type.
   * @param step The step number associated with the summary.
   */
  void CreateEvent(size_t step, mlboard::Summary *summary);

  /**
   * A function to flush everything successfully and close the thread.
   */
  void flush();

  /**
   * Responsible for indicating the class to close the thread.
   */ 
  void close();

  //! Get the path of log directory.
  std::string LogDir() const { return logdir; }
  //! Modify the flusmilis.
  size_t& FlushMilis() { return flushmilis; }
  //! Get the flusmilis.
  size_t FlushMilis() const { return flushmilis; }
  //! Get the maximum size of the queue.
  size_t MaxSize() const { return q.MaxSize(); }
  //! Modify the maximum size of the queue.
  size_t& MaxSize() { return q.MaxSize(); }
 private:
  SharedQueue<mlboard::Event> q;
  // std::thread does not have copy constructor hence pointer is safe.
  std::thread *thread_;
  std::size_t flushmilis;
  std::string logdir;
  std::time_t nexttime;
  std::ofstream outfile;
  bool close_;
};

} // namespace mlboard

// Include implementation.
#include "filewriter_impl.hpp"

#endif
