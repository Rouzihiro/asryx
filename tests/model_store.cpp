#include "tests/model_store.hpp"

#include "constants/constants.hpp"
#include "model/model.hpp"
#include "platform/fs.hpp"

#include <filesystem>
#include <fstream>
#include <string>

namespace model_store {

void write_model(const std::string& name)
{
  const auto path = std::filesystem::path(model::get_model_path(name));
  std::filesystem::create_directories(path.parent_path());
  std::ofstream model_file(path);
  model_file << "fake model content";
}

void write_default_model_and_vad()
{
  write_model(std::string(constants::config::default_model));
  std::ofstream vad_file(model::get_vad_model_path());
  vad_file << "fake VAD model content";
}

void delete_default_model_and_vad()
{
  platform::safe_delete_file(model::get_model_path(std::string(constants::config::default_model)));
  platform::safe_delete_file(model::get_vad_model_path());
}

} // namespace model_store
