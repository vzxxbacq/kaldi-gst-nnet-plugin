#include "simple_option.h"
#include <algorithm>

namespace kaldi {

void SimpleOptionsGst::Register(const std::string &name, bool *ptr,
                                const std::string &doc) {
  std::string new_name = TransformName(name);
  SimpleOptions::Register(new_name, ptr, doc);
}

void SimpleOptionsGst::Register(const std::string &name, int32 *ptr,
                                const std::string &doc) {
  std::string new_name = TransformName(name);
  SimpleOptions::Register(new_name, ptr, doc);
}

void SimpleOptionsGst::Register(const std::string &name, uint32 *ptr,
                                const std::string &doc) {
  std::string new_name = TransformName(name);
  SimpleOptions::Register(new_name, ptr, doc);
}

void SimpleOptionsGst::Register(const std::string &name, float *ptr,
                                const std::string &doc) {
  std::string new_name = TransformName(name);
  SimpleOptions::Register(new_name, ptr, doc);
}
void SimpleOptionsGst::Register(const std::string &name, double *ptr,
                                const std::string &doc) {
  std::string new_name = TransformName(name);
  SimpleOptions::Register(new_name, ptr, doc);
}
void SimpleOptionsGst::Register(const std::string &name, std::string *ptr,
                                const std::string &doc) {
  std::string new_name = TransformName(name);
  SimpleOptions::Register(new_name, ptr, doc);
}

std::string SimpleOptionsGst::TransformName(const std::string &name) {
  std::string new_name = name;
  std::replace(new_name.begin(), new_name.end(), '.', '-');
  return new_name;
};
}