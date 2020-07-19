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
void SummaryWriter<Filewriter>::PrCurve(const std::string tag,
                                        const std::vector<double>& labels,
                                        const std::vector<double>& predictions,
                                        Filewriter& fw,
                                        int threshold,
                                        std::vector<double>weights,
                                        const std::string& displayName,
                                        const std::string& description)
{
    // Pr plugin
    mlboard::PrCurvePluginData *prCurvePlugin = new PrCurvePluginData();
    prCurvePlugin->set_version(0);
    prCurvePlugin->set_num_thresholds(threshold);
    std::string pr_curve_content;
    prCurvePlugin->SerializeToString(&pr_curve_content);

    // PluginMeta data
    mlboard::SummaryMetadata_PluginData *pluginData =
        new SummaryMetadata::PluginData();
    pluginData->set_plugin_name("pr_curves");
    pluginData->set_content(pr_curve_content);

    // Summary Meta data
    mlboard::SummaryMetadata *metaData = new SummaryMetadata();
    metaData->set_display_name(displayName == "" ? tag : displayName);
    metaData->set_summary_description(description);
    metaData->set_allocated_plugin_data(plugin_data);

    // misbheaves when thresholds is greater than 127
    threshold = (std::min)(threshold, 127);
    double minCount = 1e-7;
    std::vector<std::vector<double>> data;
    while (weights.size() < labels.size())
    {
        weights.push_back(1.0);
    }
    std::vector<double>edges;
    mlboard::util::histogramEdges({0, (double)threshold - 1},
      threshold, edges);
    std::vector<double> tp(edges.size(), 0), fp(edges.size(), 0);
    
    for (size_t i = 0; i < labels.size(); ++i)
    {
        float v = labels[i];
        int item = predictions[i] * (threshold -1);
        auto lb =
            lower_bound(edges.begin(), edges.end(), item);
        // Include the exact number in previous bucket
        if(*lb != item)
            lb--;        
        tp[lb - edges.begin()] = tp[lb - edges.begin()] + (v * weights[i]);
        fp[lb - edges.begin()] = fp[lb - edges.begin()] + ((1 - v) * weights[i]);
        
    }

    // Reverse cummulative sum 
    for(int i = tp.size() - 2; i >= 0 ;i--)
    {
        tp[i] = tp[i] + tp[i+1];
        fp[i] = fp[i] + fp[i+1];
    }
    std::vector<double> tn(tp.size()), fn(tp.size()),
        precision(tp.size()), recall(tp.size());
    for(size_t i = 0; i < tp.size() ;i++)
    {
        fn[i] = tp[0] - tp[i];
        tn[i] = fp[0] - fp[i];
        precision[i] = tp[i] / (std::max)(minCount, tp[i] + fp[i]);
        recall[i] = tp[i] / (std::max)(minCount, tp[i] + fn[i]);
    }
    data.push_back(tp);
    data.push_back(fp);
    data.push_back(tn);
    data.push_back(fn);
    data.push_back(precision);
    data.push_back(recall);


    // Prepare Tensor
    mlboard::TensorShapeProto *tensorshape = new TensorShapeProto();
    mlboard::TensorShapeProto_Dim *rowdim = tensorshape->add_dim();
    rowdim->set_size(data.size());
    mlboard::TensorShapeProto_Dim *coldim = tensorshape->add_dim();
    coldim->set_size(data[0].size());   
    mlboard::TensorProto *tensor = new TensorProto();
    tensor->set_dtype(mlboard::DataType::DT_DOUBLE);
    tensor->set_allocated_tensor_shape(tensorshape);
    for(int i = 0; i < data.size(); i++)
    {
        for(int j = 0;j < data[0].size(); j++)
        {
            tensor->add_double_val(data[i][j]);
        }
    }

    mlboard::Summary *summary = new Summary();
    mlboard::Summary_Value *value = summary->add_value();
    value->set_tag(tag);
    value->set_allocated_tensor(tensor);
    value->set_allocated_metadata(meta);

    fw.CreateEvent(0, summary);
}

template<typename Filewriter>
template<typename vecType>
void SummaryWriter<Filewriter>::PrCurve(const std::string tag,
                                        const vecType& labels,
                                        const vecType& predictions,
                                        Filewriter& fw,
                                        int threshold,
                                        vecType weights,
                                        const std::string& displayName,
                                        const std::string& description)
{
  std::vector<double>convLables(labels.memptr(), labels.memptr() + labels.n_elem);
  std::vector<double>convPredictions(predictions.memptr(),
      predictions.memptr() + predictions.n_elem);
  std::vector<double>convWeights(weights.memptr(), weights.memptr() + weights.n_elem);
  PrCurve(tag, convLables, convPredictions, fw, threshold, convWeights,
      displayName, description);
}

} // namespace mlboard

#endif
