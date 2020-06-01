/**
 * @file sharebalequeue.hpp
 * @author Jeffin Sam
 */
#ifndef MLBOARD_SHAREABLE_QUEUE_HPP
#define MLBOARD_SHAREABLE_QUEUE_HPP

#include <mlboard/core.hpp>

namespace mlboard {

template <typename Datatype>
class SharebaleQueue
{
 public:
  void setmaxSize(size_t maxSize);
  Datatype pop();

  void push(const Datatype& item);
  size_t size();
 private:
  std::queue<Datatype> queue_;
  std::mutex mutex_;
  std::condition_variable queueempty;
  std::condition_variable queueFull;
  std::size_t maxSize;
};


} // namespace mlboard

// Include implementation.
#include "shareablequeue_impl.hpp"

#endif