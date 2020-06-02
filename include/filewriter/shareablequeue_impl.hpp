/**
 * @file sharebalequeue_impl.hpp
 * @author Jeffin Sam
 */
#ifndef MLBOARD_SHAREABLE_QUEUE_IMPL_HPP
#define MLBOARD_SHAREABLE_QUEUE_IMPL_HPP

#include "shareablequeue.hpp"
namespace mlboard {

template <typename Datatype>
Datatype SharebaleQueue<Datatype>::pop()
{
  std::unique_lock<std::mutex> mlock(mutex_);
  while (queue_.empty())
  {
    queueempty.wait(mlock);
  }
  auto item = queue_.front();
  queue_.pop();
  // Notify queueFull 
  mlock.unlock();
  queueFull.notify_all();
  return item;
}

template <typename Datatype>
void SharebaleQueue<Datatype>::push(const Datatype& item)
{
  std::unique_lock<std::mutex> mlock(mutex_);
  while(queue_.size() > maxSize)
  {
    queueFull.wait(mlock);
  }
  queue_.push(item);
  mlock.unlock();
  queueempty.notify_all();
}

} // namespace mlboard

#endif
