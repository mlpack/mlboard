/**
 * @file filewriter_impl.hpp
 * @author Jeffin Sam
 */
#ifndef MLBOARD_FILE_WRITER_IMPL_HPP
#define MLBOARD_FILE_WRITER_IMPL_HPP

#include "filewriter.hpp"

namespace mlboard {


inline FileWriter::FileWriter(std::string logdir,
                              int maxQueueSize,
                              std::size_t flushmilis)
{
  const auto p1 = std::chrono::system_clock::now();
  std::string currentTime =
      std::to_string(std::chrono::duration_cast<std::chrono::seconds>(
      p1.time_since_epoch()).count());
  this->logdir = logdir;
  this->filename = this->logdir + "/events.out.tfevents." + currentTime + ".v2";
  close_ = true;
  this->flushmilis = flushmilis;
  size_t &maxSize = this->q.MaxSize();
  maxSize = maxQueueSize;
  thread_ = new std::thread(&FileWriter::WriteSummary, this);
  nexttime =
      std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  outfile.open(this->filename, std::fstream::out |
      std::ios::trunc | std::ios::binary);
}

inline void FileWriter::WriteSummary()
{
  // This is a thread that will continously write summary one by one into file.
  while (true)
  {
    // Break the loop if eveything is done.
    if (!(close_ || q.Size() != 0)) break;
    std::time_t timenow =
        std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    if (timenow >= nexttime)
    {
      while (q.Size() > 0)
      {
        mlboard::Event event = q.Pop();
        std::string buf;
        event.SerializeToString(&buf);
        auto buf_len = static_cast<uint64_t>(buf.size());
        uint32_t len_crc =
            masked_crc32c((char *)&buf_len, sizeof(uint64_t));
        uint32_t data_crc = masked_crc32c(buf.c_str(), buf.size());

        outfile.write((char *)&buf_len, sizeof(uint64_t));
        outfile.write((char *)&len_crc, sizeof(uint32_t));
        outfile.write(buf.c_str(), buf.size());
        outfile.write((char *)&data_crc, sizeof(uint32_t));
        outfile.flush();
      }
      nexttime =
          std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()
          + std::chrono::milliseconds(flushmilis));
    }
  }
}

inline void FileWriter::CreateEvent(size_t step, mlboard::Summary *summary)
{
  Event event;
  double wall_time = time(nullptr);
  event.set_wall_time(wall_time);
  event.set_step(step);
  event.set_allocated_summary(summary);
  q.Push(event);
}

inline void FileWriter::Flush()
{
  // Flush everything successfully and close the thread.
  thread_->join();
}

inline void FileWriter::Close()
{
  close_ = false;
  Flush();
}

inline FileWriter::~FileWriter()
{
  if (close_)
    Close();
}

} // namespace mlboard

#endif
