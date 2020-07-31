/**
 * @file filewriter/summarywriter_impl.hpp
 * @author Jeffin Sam
 */
#ifndef MLBOARD_SUMMARY_WRITER_IMPL_HPP
#define MLBOARD_SUMMARY_WRITER_IMPL_HPP

#include <mlboard/core.hpp>
#include "summarywriter.hpp"

namespace mlboard {

template<typename Filewriter>
void SummaryWriter<Filewriter>::Scalar(const std::string& tag,
                                       int step,
                                       double value,
                                       Filewriter& fw)
{
  mlboard::Summary *summary = new Summary();
  mlboard::Summary_Value *v = summary->add_value();
  v->set_tag(tag);
  v->set_simple_value(value);
  fw.CreateEvent(step, summary);
}

template<typename Filewriter>
void SummaryWriter<Filewriter>::Text(const std::string &tag,
                                     int step,
                                     const std::string& text,
                                     Filewriter& fw)
{
  mlboard::SummaryMetadata_PluginData *pluginData =
      new SummaryMetadata::PluginData();
  pluginData->set_plugin_name("text");

  mlboard::SummaryMetadata *meta = new SummaryMetadata();
  meta->set_allocated_plugin_data(pluginData);

  mlboard::TensorProto *tensor = new TensorProto();
  tensor->set_dtype(mlboard::DataType::DT_STRING);

  tensor->add_string_val(text);

  mlboard::Summary *summary = new Summary();
  mlboard::Summary_Value *v = summary->add_value();
  v->set_tag(tag);
  v->set_allocated_tensor(tensor);
  v->set_allocated_metadata(meta);

  fw.CreateEvent(step, summary);
}

template<typename Filewriter>
void SummaryWriter<Filewriter>::Image(const std::string& tag,
                                      int step,
                                      const std::string& encodedImage,
                                      int height,
                                      int width,
                                      int channel,
                                      Filewriter& fw,
                                      const std::string& displayName,
                                      const std::string& description)
{
  mlboard::SummaryMetadata *meta = new SummaryMetadata();
  meta->set_display_name(displayName == "" ? tag : displayName);
  meta->set_summary_description(description);

  mlboard::Summary_Image *image = new Summary::Image();
  image->set_height(height);
  image->set_width(width);
  image->set_colorspace(channel);
  image->set_encoded_image_string(encodedImage);

  mlboard::Summary *summary = new Summary();
  mlboard::Summary_Value *v = summary->add_value();
  v->set_tag(tag);
  v->set_allocated_image(image);
  v->set_allocated_metadata(meta);

  fw.CreateEvent(step, summary);
}

template<typename Filewriter>
void SummaryWriter<Filewriter>::Image(
    const std::string& tag,
    int step,
    const std::vector<std::string>& encodedImages,
    int height,
    int width,
    Filewriter& fw,
    const std::string& displayName,
    const std::string& description)
{
  mlboard::SummaryMetadata_PluginData *pluginData =
    new SummaryMetadata::PluginData();
  pluginData->set_plugin_name("images");
  mlboard::SummaryMetadata *meta = new SummaryMetadata();
  meta->set_display_name(displayName == "" ? tag : displayName);
  meta->set_summary_description(description);
  meta->set_allocated_plugin_data(pluginData);

  mlboard::TensorProto *tensor = new TensorProto();
  tensor->set_dtype(mlboard::DataType::DT_STRING);
  tensor->add_string_val(std::to_string(width));
  tensor->add_string_val(std::to_string(height));
  for (const std::string& image : encodedImages)
      tensor->add_string_val(image);

  mlboard::Summary *summary = new Summary();
  mlboard::Summary_Value *v = summary->add_value();
  v->set_tag(tag);
  v->set_allocated_tensor(tensor);
  v->set_allocated_metadata(meta);
  fw.CreateEvent(step, summary);
}

template<typename Filewriter>
template<typename eT>
void SummaryWriter<Filewriter>::Image(const std::string& tag,
                int step,
                arma::Mat<eT>& matrix,
                mlpack::data::ImageInfo& info,
                Filewriter& fw,
                const std::string& displayName,
                const std::string& description)
{
  // Create a temp directory.
  int check;
  #if defined(_WIN32)
    check = mkdir("_preprocess_");
  #else
    check = mkdir("_preprocess_", 0777);
  #endif

  // Create a vector of temp file names.
  std::vector<std::string> fileNames(matrix.n_cols);
  for (size_t i = 0; i < matrix.n_cols; i++)
    fileNames[i] = "_preprocess_/"+std::to_string(i)+".png";

  mlpack::data::Save(fileNames, matrix, info, false);

  std::vector<std::string> encodedImages;
  mlboard::util::EncodeImage(fileNames, encodedImages);

  Image(tag, step, encodedImages, info.Height(),
    info.Width(), fw, displayName, description);

  // Remove all the files.
  for (size_t i = 0; i < matrix.n_cols; i++)
    remove(fileNames[i].c_str());
}

template<typename Filewriter>
void SummaryWriter<Filewriter>::Histogram(const std::string& tag,
                                          int step,
                                          std::vector<double>& values,
                                          std::vector<double>& bins,
                                          Filewriter& fw)
{
    size_t num = values.size();
    double min = std::numeric_limits<double>::max();
    double max = std::numeric_limits<double>::lowest();
    double sum = 0.0;
    double sumofSquares = 0.0;

    std::vector<int> counts(bins.size(), 0);
    for (size_t i = 0; i < num; ++i) {
        float v = values[i];
        auto lb =
            lower_bound(bins.begin(), bins.end(), v);
        counts[lb - bins.begin()]++;
        sum += v;
        sumofSquares += v * v;
        if (v > max) {
            max = v;
        } else if (v < min) {
            min = v;
        }
    }

    mlboard::HistogramProto *histo = new HistogramProto();
    histo->set_min(min);
    histo->set_max(max);
    histo->set_num(num);
    histo->set_sum(sum);
    histo->set_sum_squares(sumofSquares);
    for (size_t i = 0; i < counts.size(); ++i) {
        if (counts[i] > 0) {
            histo->add_bucket_limit(bins[i]);
            histo->add_bucket(counts[i]);
        }
    }

    mlboard::Summary *summary = new Summary();
    mlboard::Summary_Value *v = summary->add_value();
    v->set_tag(tag);
    v->set_allocated_histo(histo);
    fw.CreateEvent(step, summary);
}

template<typename Filewriter>
void SummaryWriter<Filewriter>::Histogram(const std::string& tag,
                                          int step,
                                          std::vector<double>& values,
                                          Filewriter& fw)
{
  std::vector<double> edges;
  std::vector<double> posEdges, negEdges;
  double v = 1e-12;
  while (v < 1e20) {
      posEdges.push_back(v);
      negEdges.push_back(-v);
      v *= 1.1;
  }
  posEdges.push_back(std::numeric_limits<double>::max());
  negEdges.push_back(std::numeric_limits<double>::lowest());

  edges.insert(edges.end(), negEdges.rbegin(),
                          negEdges.rend());
  edges.insert(edges.end(), posEdges.begin(),
                          posEdges.end());
  Histogram(tag, step, values, edges, fw);
}


template<typename Filewriter>
template<typename RowType>
void SummaryWriter<Filewriter>::Histogram(const std::string& tag,
                                          int step,
                                          const RowType& values,
                                          Filewriter& fw)
{
  std::vector<double>tempValues(values.memptr(),
      values.memptr() + values.n_elem);
  Histogram(tag, step, tempValues, fw);
}

} // namespace mlboard

#endif
