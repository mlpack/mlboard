/**
 * @file mlboard_logger.hpp
 * @author Jeffin Sam
 *
 * Implementation of the mlbaord logger callback function.
 */
#ifndef ENSMALLEN_CALLBACKS_MLBOARD_LOGGER_HPP
#define ENSMALLEN_CALLBACKS_MLBOARD_LOGGER_HPP

#include <ensmallen.hpp>
#include <mlboard/core.hpp>
#include <mlboard/mlboard.hpp>

namespace ens {

/**
 * Mlboard Logger function, based on the EndEpoch callback function.
 */
class MlboardLogger
{
 public:
  /**
   *
   * @param output Filewriter object to log the metrics.
   */
  MlboardLogger(mlboard::FileWriter& output,
      std::string accTag = "accuracy",
      std::string lossTag = "loss") : callbackUsed(false),
      output(output),
      accTag(accTag),
      lossTag(lossTag)
  { /* Nothing to do here. */ }

  MlboardLogger(
      mlboard::FileWriter& output,
      std::function<double()> func,
      std::string accTag = "accuracy",
      std::string lossTag = "loss")
    : callbackUsed(true),
      output(output),
      localFunc(func),
      accTag(accTag),
      lossTag(lossTag)
  {
    // Nothing to do here
  }
  /**
   * Callback function called at the end of a pass over the data.
   *
   * @param optimizer The optimizer used to update the function.
   * @param function Function to optimize.
   * @param coordinates Starting point.
   * @param epoch The index of the current epoch.
   * @param objective Objective value of the current point.
   */
  template<typename OptimizerType, typename FunctionType, typename MatType>
  void EndEpoch(OptimizerType& /* optimizer */,
                FunctionType& /* function */,
                const MatType& /* coordinates */,
                const size_t epoch,
                double objective)
  {
    if (callbackUsed)
    {
      objective = localFunc();
    }
    mlboard::SummaryWriter<mlboard::FileWriter>::Scalar(lossTag,
      epoch, objective, output);
    mlboard::SummaryWriter<mlboard::FileWriter>::Scalar(accTag,
      epoch, 1 - objective, output);
  }

 private:
  //! False if the first constructor is called, true if the user passed a lambda. 
  bool callbackUsed;
    //! Function to call at the end of the epoch.
  std::function<double()> localFunc;
  std::string accTag;
  std::string lossTag;
  //! Filewriter object will will log the data.
  mlboard::FileWriter& output;

};

} // namespace ens

#endif