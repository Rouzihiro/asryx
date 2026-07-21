#ifndef ASRYX_TESTS_MODEL_STORE_HPP
#define ASRYX_TESTS_MODEL_STORE_HPP

#include <string>

namespace model_store {

void write_model(const std::string& name);
void write_default_model_and_vad();
void delete_default_model_and_vad();

} // namespace model_store

#endif // ASRYX_TESTS_MODEL_STORE_HPP
