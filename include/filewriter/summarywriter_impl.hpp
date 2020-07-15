/**
 * @file summarywriter_impl.hpp
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
      const std::string& tensordataPath,
      const std::string& metadataPath)
{
  mlpack::data::Save(tensordataPath, tensordata);
  if (metadata.size() > 0)
  {
    if (metadata.size() != tensor.size())
    {
        throw std::runtime_error("tensor size != metadata size");
    }
    ofstream metadataFile(metadataPath);
    if (!metadataFile.is_open())
    {
        throw std::runtime_error("failed to open metadata file " +
        metadataPath);
    }
    for (const auto &meta : metadata) metadataFile << meta << endl;
    metadataFile.close();
  }
  std::vector<size_t> tensor_shape = {tensordata.n_rows, tensordata.n_cols};
  return Embedding(tensor_name, tensordataPath, metadataPath,
                        tensor_shape);

}
} // namespace mlboard

#endif
