/**
 * @file sharedqueue.hpp
 * @author Jeffin Sam
 */
#ifndef MLBOARD_SHARED_QUEUE_HPP
#define MLBOARD_SHARED_QUEUE_HPP

#include <mlboard/core.hpp>

namespace mlboard {

/**
 * Class responsible to share a queue among main thread and event logger
 * thread.
 * 
 * @tparam Datatype datatype of the elements queue would be holding. 
 */
template <typename Datatype>
class SharedQueue
{
 public:
  /**
   * function to pop an element from the queue.
   */ 
  Datatype pop();

  /**
  * function to push an element in the queue.
  * 
  * @param item The element to be pushed
  */
  void push(const Datatype& item);
  //! Get the size of the queue.
  size_t size() const { return queue_.size(); }
  //! Get the maximum size of the queue.
  size_t MaxSize() const { return maxSize; }
  //! Modify the maximum size of the queue.
  size_t& MaxSize() { return maxSize; }
  // ! Check if queue is empty
  bool empty() { return queue_.empty(); }
 private:
  std::queue<Datatype> queue_;
  std::mutex mutex_;
  std::condition_variable queueempty;
  std::condition_variable queueFull;
  std::size_t maxSize;
};

} // namespace mlboard

// Include implementation.
#include "sharedqueue_impl.hpp"

#endif
