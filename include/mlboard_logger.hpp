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
   * @param epochCount Interval of epochs youw want to log your data.
   * @param accTag Tag to use for accuracy.
   * @param lossTag Tag to use for loss
   */
  MlboardLogger(
      mlboard::FileWriter& output,
      int epochCount = 1,
      std::string accTag = "accuracy",
      std::string lossTag = "loss") : callbackUsed(false),
      output(output),
      summaryType(""),
      epochCount(epochCount),
      accTag(accTag),
      lossTag(lossTag)
  { /* Nothing to do here. */ }

  /**
   *
   * @param output Filewriter object to log the metrics.
   * @param func A custom function to log some metrics, which return the
   *    value to be logged.
   * @param epochCount Interval of epochs youw want to log your data.
   * @param accTag Tag to use for accuracy.
   * @param lossTag Tag to use for loss.
   */
  MlboardLogger(
      mlboard::FileWriter& output,
      std::function<double()> func,
      int epochCount = 1,
      std::string accTag = "accuracy",
      std::string lossTag = "loss")
    : callbackUsed(true),
      output(output),
      summaryType(""),
      localFunc(func),
      epochCount(epochCount),
      accTag(accTag),
      lossTag(lossTag)
  {
    // Nothing to do here.
  }

  /**
   *
   * @param output Filewriter object to log the metrics.
   * @param func A custom function to log some metrics, which return the
   *    value to be logged.
   * @param epochCount Interval of epochs youw want to log your data.
   * @param summaryTag Tag to use for summary.
   * @param summaryType Type of summary to be logged.
   * @param embeddingMetadata Metadata for embedding.
   * @param heighofImage The height of image to be logged.
   * @param widthofImage The width of image to be logged.
   */
  MlboardLogger(
      mlboard::FileWriter& output,
      std::function<arma::mat()> func,
      std::string summaryTag,
      std::string summaryType,
      std::vector<std::string> embeddingMetadata = {},
      size_t heightofImage = 0,
      size_t widthofImage = 0,
      int epochCount = 1)
    : callbackUsed(true),
      output(output),
      matFunc(func),
      widthofImage(widthofImage),
      heightofImage(heightofImage),
      embeddingMetadata(embeddingMetadata),
      summaryType(summaryType),
      epochCount(epochCount),
      summaryTag(summaryTag)
  {
    // Nothing to do here.
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
    if (callbackUsed && summaryType == "")
    {
      objective = localFunc();
    }
    if (summaryType == "")
    {
      if (epoch % epochCount == 0)
      {
        mlboard::SummaryWriter<mlboard::FileWriter>::Scalar(lossTag,
            epoch / epochCount, objective, output);
        mlboard::SummaryWriter<mlboard::FileWriter>::Scalar(accTag,
            epoch / epochCount, 1 - objective, output);
      }
    }
    if(callbackUsed && (epoch % epochCount == 0) && summaryType != "")
    {
      arma::mat valueToBeLogged = matFunc();
      if(summaryType == "histogram")
      {
        mlboard::SummaryWriter<mlboard::FileWriter>::Histogram(summaryTag,
            epoch / epochCount, arma::rowvec(valueToBeLogged), output);
      }
      else if(summaryType == "embedding")
      {
        mlboard::SummaryWriter<mlboard::FileWriter>::Embedding(summaryTag,
            valueToBeLogged, embeddingMetaData, output);
      }
      else if(summaryType == "image")
      {
        // channel is not needed to log image
        mlpack::data::ImageInfo info(widthofImage, heightofImage, 0);
        mlboard::SummaryWriter<mlboard::FileWriter>::Image(
              summaryTag, epoch / epochCount , valueTobeLogged,
              info, output);
      }
      else
      {
        /* Do nothing */
      } 
    }
  }

 private:
  //! False if the first constructor is called, true if the user passed a
  // lambda.
  bool callbackUsed;

  //! Function to call at the end of the epoch.
  std::function<double()> localFunc;

  //! Function to call at the end of the epoch.
  std::function<arma::mat()> matFunc;

  //! Interval for the summary to be logged.
  int epochCount;

  //! Path for the metadata for embedding.
  std::vector<std::string> embeddingMetadata;

  //! Tag to log summary accuracy scaler.
  std::string accTag;

  //! Height of the image.
  size_t heightofImage;

  //! width of the image.
  size_t widthofImage;

  //! Tag to log summary loss scaler. 
  std::string lossTag;

  //! Tag to log other type of summary
  std::string summaryTag;

  //! Type of summary you want to log.
  std::string summaryType;

  //! Filewriter object will will log the data.
  mlboard::FileWriter& output;
};

} // namespace ens

#endif
