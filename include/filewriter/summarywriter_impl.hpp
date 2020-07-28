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
void SummaryWriter<Filewriter>::Embedding(
    const std::string &tensorName,
    const std::string &tensordataPath,
    Filewriter& fw,
    const std::string &metadataPath,
    const std::vector<size_t> &tensorShape)
{
    mlboard::SummaryMetadata_PluginData *plugin_data = new SummaryMetadata::PluginData();
    plugin_data->set_plugin_name("projector");
    mlboard::SummaryMetadata *meta = new SummaryMetadata();
    meta->set_allocated_plugin_data(plugin_data);

    const std::string &filename = fw.LogDir() + "/projector_config.pbtxt";;
    mlboard::ProjectorConfig *conf = new ProjectorConfig();

    // parse possibly existing config file
    std::ifstream fin(filename);
    if (fin.is_open()) {
        std::ostringstream ss;
        ss << fin.rdbuf();
        google::protobuf::TextFormat::ParseFromString(ss.str(), conf);
        fin.close();
    }

    mlboard::EmbeddingInfo *embedding = conf->add_embeddings();
    embedding->set_tensor_name(tensorName);
    embedding->set_tensor_path(tensordataPath);
    if (metadataPath != "") {
        embedding->set_metadata_path(metadataPath);
    }
    if (tensorShape.size() > 0) {
        for (size_t shape : tensorShape) embedding->add_tensor_shape(shape);
    }

    // `conf` and `embedding` will be deleted by ProjectorConfig destructor
    std::ofstream fout(filename);
    std::string content;
    google::protobuf::TextFormat::PrintToString(*conf, &content);
    fout << content;
    fout.close();

    mlboard::Summary *summary = new Summary();
    mlboard::Summary_Value *v = summary->add_value();
    v->set_tag("embedding");
    v->set_allocated_metadata(meta);

    fw.CreateEvent(1, summary);
}

template<typename Filewriter>
void SummaryWriter<Filewriter>::Embedding(
      const std::string& tensorName,
      const arma::mat& tensordata,
      const std::vector<std::string> &metadata,
      Filewriter& fw,
      std::string tensordataPath,
      std::string metadataPath)
{
  // Deafult file name
  if(tensordataPath == "")
    tensordataPath = "tensor.tsv";
  if(metadataPath == "")
    metadataPath = "meta.tsv";
  std::ofstream tensordDataFile(tensordataPath);
  // mlpack::data::Save(tensordataPath, arma::sp_mat(tensordata));
  for (size_t i = 0; i < tensordata.n_cols; i++)
  {
    for (size_t j = 0; j < tensordata.n_rows; j++)
    {
      tensordDataFile << tensordata(j, i) << '\t';
    }
    tensordDataFile << "\n";
  }
  if (metadata.size() > 0)
  {
    if (metadata.size() != tensordata.n_cols)
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
      metadataFile << meta << std::endl;
    metadataFile.close();
  }
  std::vector<size_t> tensor_shape = {tensordata.n_rows, tensordata.n_cols};
  // default path should be relative to logdir
  if(tensordataPath == "tensor.tsv")
    tensordataPath = "../tensor.tsv";
  if(metadataPath == "meta.tsv")
    metadataPath = "../meta.tsv";
  Embedding(tensorName, tensordataPath,fw, metadataPath, tensor_shape);
}
} // namespace mlboard

#endif
