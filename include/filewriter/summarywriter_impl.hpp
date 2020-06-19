/**
 * @file filewriter.hpp
 * @author Jeffin Sam
 */
#ifndef MLBOARD_SUMMARY_WRITER_IMPL_HPP
#define MLBOARD_SUMMARY_WRITER_IMPL_HPP

#include <core.hpp>
#include "summarywriter.hpp"

namespace mlboard {

template<typename filewriter>
void SummaryWriter<filewriter>::scalar(const std::string& tag,
                                       int step,
                                       double value,
                                       filewriter& fw)
{
    mlboard::Summary *summary = new Summary();
    mlboard::Summary_Value *v = summary->add_value();
    v->set_tag(tag);
    v->set_simple_value(value);
    fw.CreateEvent(step, summary);
}

} // namespace mlboard

#endif
