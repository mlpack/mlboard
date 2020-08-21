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
void SummaryWriter<Filewriter>::Embedding(
    const std::string &tensorName,
    const std::string &tensordataPath,
    Filewriter& fw,
    const std::string &metadataPath,
    const std::vector<size_t> &tensorShape)
{
    mlboard::SummaryMetadata_PluginData *pluginData = new SummaryMetadata::PluginData();
    pluginData->set_plugin_name("projector");
    mlboard::SummaryMetadata *metadata = new SummaryMetadata();
    metadata->set_allocated_plugin_data(pluginData);

    const std::string &filename = fw.LogDir() + "/projector_config.pbtxt";;
    mlboard::ProjectorConfig *config = new ProjectorConfig();

    // Parse possibly existing config file.
    std::ifstream fin(filename);
    if (fin.is_open()) 
    {
        std::ostringstream ss;
        ss << fin.rdbuf();
        google::protobuf::TextFormat::ParseFromString(ss.str(), config);
        fin.close();
    }

    mlboard::EmbeddingInfo *embedding = config->add_embeddings();
    embedding->set_tensor_name(tensorName);
    embedding->set_tensor_path(tensordataPath);
    if (metadataPath != "") 
    {
        embedding->set_metadata_path(metadataPath);
    }
    if (tensorShape.size() > 0) 
    {
        for (size_t shape : tensorShape) embedding->add_tensor_shape(shape);
    }

    // The `config` and `embedding` pointers will be deleted by the
    // ProjectorConfig destructor.
    std::ofstream fout(filename);
    std::string content;
    google::protobuf::TextFormat::PrintToString(*config, &content);
    fout << content;
    fout.close();

    mlboard::Summary *summary = new Summary();
    mlboard::Summary_Value *v = summary->add_value();
    v->set_tag("embedding");
    v->set_allocated_metadata(metadata);

    fw.CreateEvent(1, summary);
}

template<typename Filewriter>
void SummaryWriter<Filewriter>::Embedding(
      const std::string& tensorName,
      const arma::mat& tensordata,
      const std::vector<std::string> &metadata,
      Filewriter& fw,
      std::string tensordataPath,
      std::string metadataPath,
      std::string relativeTensordataPath,
      std::string relativeMetadataPath)
{
  // Deafult file name.
  if(tensordataPath == "")
    tensordataPath = fw.LogDir() + "/tensor.tsv";
  if(metadataPath == "")
    metadataPath = fw.LogDir() + "/meta.tsv";
  std::ofstream tensorDataFile(tensordataPath);
  if (!tensorDataFile.is_open())
  {
    throw std::runtime_error("failed to open tensordata file " +
    tensordataPath);
  }

  // Note : We save the matrix as it is, it is on user's hand to transpose it 
  // if needed.
  for (size_t i = 0; i < tensordata.n_cols; i++)
  {
    for (size_t j = 0; j < tensordata.n_rows; j++)
    {
      tensorDataFile << tensordata(j, i);
      if (j != tensordata.n_rows - 1)
      {
        tensorDataFile << "\t";
      }   
    }
    tensorDataFile << std::endl;
  }
  tensorDataFile.close();
  if (metadata.size() > 0)
  {
    if (metadata.size() != tensordata.n_rows)
    {
        throw std::runtime_error("tensor size != metadata size");
    }
    std::ofstream metadataFile(metadataPath);
    if (!metadataFile.is_open())
    {
        throw std::runtime_error("failed to open metadata file " +
            metadataPath);
    }
    for (const std::string& meta : metadata)
    {
      metadataFile << meta << std::endl;
    }
    metadataFile.close();
  }
  std::vector<size_t> tensorShape = {tensordata.n_rows, tensordata.n_cols};
  // Default path should be relative to the logging directory.
  if(tensordataPath == fw.LogDir() + "/tensor.tsv")
    relativeTensordataPath = "tensor.tsv";
  if(metadataPath == fw.LogDir() + "/meta.tsv")
    relativeMetadataPath = "meta.tsv";
  Embedding(tensorName, relativeTensordataPath, fw, relativeMetadataPath, tensorShape);
}

template<typename Filewriter>
void SummaryWriter<Filewriter>::PRCurve(const std::string& tag,
                                        const std::vector<double>& labels,
                                        const std::vector<double>& predictions,
                                        Filewriter& fw,
                                        int threshold,
                                        std::vector<double>weights,
                                        const std::string& displayName,
                                        const std::string& description)
{
    // PR-Curve plugin.
    mlboard::PrCurvePluginData *prCurvePlugin = new PrCurvePluginData();
    prCurvePlugin->set_version(0);
    prCurvePlugin->set_num_thresholds(threshold);
    std::string prCurveContent;
    prCurvePlugin->SerializeToString(&prCurveContent);

    // Plugin metadata.
    mlboard::SummaryMetadata_PluginData *pluginData =
        new SummaryMetadata::PluginData();
    pluginData->set_plugin_name("pr_curves");
    pluginData->set_content(prCurveContent);

    // Summary metadata.
    mlboard::SummaryMetadata *metadata = new SummaryMetadata();
    metadata->set_display_name(displayName == "" ? tag : displayName);
    metadata->set_summary_description(description);
    metadata->set_allocated_plugin_data(pluginData);

    double minCount = 1e-7;
    std::vector<std::vector<double>> data;
    while (weights.size() < labels.size())
    {
        weights.push_back(1.0);
    }
    std::vector<double> edges;
    mlboard::util::histogramEdges({0, (double)threshold - 1},
      threshold, edges);
    std::vector<double> truePositives(edges.size(), 0);
    std::vector<double> falsePositives(edges.size(), 0);

    for (size_t i = 0; i < labels.size(); ++i)
    {
        double v = labels[i];
        int item = predictions[i] * (threshold -1);
        auto lb =
            lower_bound(edges.begin(), edges.end(), item);
        // Include the exact number in previous bucket.
        if (*lb != item)
            lb--;
        truePositives[lb - edges.begin()] = truePositives[lb - edges.begin()] + (v * weights[i]);
        falsePositives[lb - edges.begin()] = falsePositives[lb - edges.begin()] +
            ((1 - v) * weights[i]);
    }

    // Reverse cummulative sum.
    for (int i = truePositives.size() - 2; i >= 0; i--)
    {
        truePositives[i] = truePositives[i] + truePositives[i+1];
        falsePositives[i] = falsePositives[i] + falsePositives[i+1];
    }
    std::vector<double> trueNegatives(truePositives.size()),
        falseNegatives(truePositives.size()), precision(truePositives.size()),
        recall(truePositives.size());
    for (size_t i = 0; i < truePositives.size(); i++)
    {
        falseNegatives[i] = truePositives[0] - truePositives[i];
        trueNegatives[i] = falsePositives[0] - falsePositives[i];
        precision[i] = truePositives[i] / (std::max)(minCount, truePositives[i] +
            falsePositives[i]);
        recall[i] = truePositives[i] / (std::max)(minCount, truePositives[i] +
            falseNegatives[i]);
    }
    data.push_back(truePositives);
    data.push_back(falsePositives);
    data.push_back(trueNegatives);
    data.push_back(falseNegatives);
    data.push_back(precision);
    data.push_back(recall);

    // Prepare Tensor.
    mlboard::TensorShapeProto *tensorShape = new TensorShapeProto();
    mlboard::TensorShapeProto_Dim *rowDim = tensorShape->add_dim();
    rowDim->set_size(data.size());
    mlboard::TensorShapeProto_Dim *colDim = tensorShape->add_dim();
    colDim->set_size(data[0].size());
    mlboard::TensorProto *tensor = new TensorProto();
    tensor->set_dtype(mlboard::DataType::DT_DOUBLE);
    tensor->set_allocated_tensor_shape(tensorShape);
    for (int i = 0; i < data.size(); i++)
    {
        for (int j = 0; j < data[0].size(); j++)
        {
            tensor->add_double_val(data[i][j]);
        }
    }

    mlboard::Summary *summary = new Summary();
    mlboard::Summary_Value *value = summary->add_value();
    value->set_tag(tag);
    value->set_allocated_tensor(tensor);
    value->set_allocated_metadata(metadata);

    fw.CreateEvent(0, summary);
}

template<typename Filewriter>
template<typename vecType>
void SummaryWriter<Filewriter>::PRCurve(const std::string& tag,
                                        const vecType& labels,
                                        const vecType& predictions,
                                        Filewriter& fw,
                                        int threshold,
                                        vecType weights,
                                        const std::string& displayName,
                                        const std::string& description)
{
  std::vector<double>convLabels(labels.memptr(), labels.memptr() +
      labels.n_elem);
  std::vector<double>convPredictions(predictions.memptr(),
      predictions.memptr() + predictions.n_elem);
  std::vector<double>convWeights(weights.memptr(), weights.memptr() +
      weights.n_elem);
  PRCurve(tag, convLabels, convPredictions, fw, threshold, convWeights,
      displayName, description);
}

} // namespace mlboard

#endif
