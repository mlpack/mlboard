/**
 * @file filewriter/summarywriter.hpp
 * @author Jeffin Sam
 */
#ifndef MLBOARD_SUMMARY_WRITER_HPP
#define MLBOARD_SUMMARY_WRITER_HPP

#include <mlboard/core.hpp>
#include "filewriter.hpp"
#include "util.hpp"
#include <proto/summary.pb.h>
#include <proto/plugin_pr_curve.pb.h>

namespace mlboard {

/**
 * Class responsible to create a summary to be logged.
 * 
 * @tparam filewriter The filewriter object which would convert it
 *    into events and then log to a file. 
 */
template<typename Filewriter = mlboard::FileWriter>
class SummaryWriter
{
 public:
  /**
   * A function to create a scalar summary.
   * 
   * @param tag Tag to uniquely identify the scalar type.
   * @param step The step at which scalar was logged.
   * @param value Scalar value to be logged.
   * @param fw Filewriter object.
   */
  static void Scalar(const std::string& tag,
                     int step,
                     double value,
                     Filewriter& fw);

  /**
   * A function to create a text summary.
   * 
   * @param tag Tag to uniquely identify the scaler type.
   * @param step The step at which scaler was logged.
   * @param value Text value to be logged.
   * @param fw Filewriter object.
   */
  static void Text(const std::string& tag,
                   int step,
                   const std::string& text,
                   Filewriter& fw);

  /**
   * A function to create an image summary.
   * 
   * @param tag Tag to uniquely identify the image type.
   * @param step The step at which scalar was logged.
   * @param encodedImage Image encoded in string datatype.
   * @param height Height of image.
   * @param width Width of image.
   * @param channel Channel of image.
   * @param fw Filewriter object.
   * @param displayName Metadata for displaying Name of image.
   * @param description Metadata for the description of the image.
   */
  static void Image(const std::string& tag,
                    int step,
                    const std::string& encodedImage,
                    int height,
                    int width,
                    int channel,
                    Filewriter& fw,
                    const std::string& displayName = "",
                    const std::string& description = "");

  /**
   * A overloaded function to create multiple image summary.
   * 
   * @param tag Tag to uniquely identify the image type.
   * @param step The step at which scalar was logged.
   * @param encodedImages A vector of Images encoded in string datatype.
   * @param height Height of image.
   * @param width Width of image.
   * @param channel Channel of image.
   * @param fw Filewriter object.
   * @param displayName Metadata for displaying Name of image.
   * @param desctiption Metadata for description of image.
   */
  static void Image(const std::string& tag,
                    int step,
                    const std::vector<std::string>& encodedImages,
                    int height,
                    int width,
                    Filewriter& fw,
                    const std::string& displayName = "",
                    const std::string& description = "");

  /**
   * A overloaded function to create multiple image summary that is stored
   * in armadillo matrix (arma::mat).
   * 
   * Please not that this function is very slow since the image has to be 
   * processed and then converted to summary and hence this has to be run in
   * seperate thread so that this function doesn't hinder any other operation.
   * Also you can avoid creating a thread if logging time is not of concern for
   * the application. Also make sure that all the images are of same height and
   * width, since mlapck::load() function only supports loading images of same
   * sizes.
   * 
   * @param tag Tag to uniquely identify the image type.
   * @param step The step at which scalar was logged.
   * @param matrix Matrix which holds the information about images.
   * @param height Height of image.
   * @param width Width of image.
   * @param channel Channel of image.
   * @param fw Filewriter object.
   * @param displayName Metadata for displaying Name of image.
   * @param desctiption Metadata for description of image.
   */  
  template<typename eT>
  static void Image(const std::string& tag,
                    int step,
                    arma::Mat<eT>& matrix,
                    mlpack::data::ImageInfo& info,
                    Filewriter& fw,
                    const std::string& displayName = "",
                    const std::string& description = "");

  /**
   * A function to create a PR-Curve summary.
   * 
   * @param tag Tag to uniquely identify the scalar type.
   * @param labels Vector of ground truth values.
   * @param predictions Vector of predictions.
   * @param fw Filewriter object.
   * @param threshold Number of thresholds.
   * @param weights Vector having the weights of labels,
   *    Individual counts are multiplied by this value.
   * @param displayName Optional name for this summary.
   * @param description Optional long-form description for this summary.
   */
  static void PrCurve(const std::string& tag,
                      const std::vector<double>& labels,
                      const std::vector<double>& predictions,
                      Filewriter& fw,
                      int threshold = 127,
                      std::vector<double>weights = {},
                      const std::string& displayName = "",
                      const std::string& description = "");

  /**
   * An overloaded function to create a PR-Curve summary using armadillo
   * vector, either rowvec or colvec.
   * 
   * @param tag Tag to uniquely identify the scalar type.
   * @param labels Ground truth values of arma::vec type.
   * @param predictions Armadiilo vector of predictions.
   * @param fw Filewriter object.
   * @param threshold Number of thresholds.
   * @param weights Armadillo vector having the weights of labels,
   *    Individual counts are multiplied by this value.
   * @param displayName Optional name for this summary.
   * @param description Optional long-form description for this summary.
   */
  template<typename vecType>
  static void PrCurve(const std::string& tag,
                      const vecType& labels,
                      const vecType& predictions,
                      Filewriter& fw,
                      int threshold = 10,
                      vecType weights = {},
                      const std::string& displayName = "",
                      const std::string& description = "");
};

} // namespace mlboard

// Include implementation.
#include "summarywriter_impl.hpp"

#endif
