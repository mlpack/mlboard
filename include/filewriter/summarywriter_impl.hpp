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

} // namespace mlboard

#endif
