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
   * Function to pop an element from the queue.
   */ 
  Datatype Pop();

  /**
   * Function to push an element in the queue.
   * 
   * @param item The element to be pushed.
   */
  void Push(const Datatype& item);
  //! Get the size of the queue.
  size_t Size() const { return queue_.size(); }
  //! Get the maximum size of the queue.
  size_t MaxSize() const { return maxSize; }
  //! Modify the maximum size of the queue.
  size_t& MaxSize() { return maxSize; }
  //! Check if queue is empty.
  bool Empty() { return queue_.empty(); }
 private:
  //! Queue that holds the data being shared across threads.
  std::queue<Datatype> queue_;

  //! Lock that allows single thread access at a time.
  std::mutex mutex_;

  //! Queue empty Condition to block a thread and later notify other
  //! threads when queue is not empty.
  std::condition_variable queueempty;

  //! Queue full Condition to block a thread and later notify other
  //! threads when queue is not full.
  std::condition_variable queueFull;

  // Maximum number of elements that can be stored in queue during a time.
  std::size_t maxSize;
};

} // namespace mlboard

// Include implementation.
#include "sharedqueue_impl.hpp"

#endif
