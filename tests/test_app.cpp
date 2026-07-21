#include "app/app.hpp"
#include "config/config.hpp"
#include "constants/constants.hpp"
#include "platform/fs.hpp"
#include "platform/process.hpp"
#include "runtime/runtime.hpp"
#include "tests/model_store.hpp"
#include "tests/tests.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <libassert/assert.hpp>
#include <span>
#include <string>
#include <sys/types.h>
#include <vector>

namespace {

std::filesystem::path runtime_file(const std::string& name)
{
  return platform::get_runtime_directory() / name;
}

void clean_runtime_files()
{
  platform::safe_delete_file(runtime_file(std::string(constants::runtime::recorder_pid_file)));
  platform::safe_delete_file(runtime_file(std::string(constants::runtime::recorder_wav_file)));
  platform::safe_delete_file(runtime_file(std::string(constants::runtime::recorder_error_file)));
  platform::safe_delete_file(runtime_file(std::string(constants::runtime::cancel_marker_file)));
  platform::safe_delete_file(runtime_file(std::string(constants::runtime::state_file)));
}

bool recording_files_exist()
{
  return std::filesystem::exists(
             runtime_file(std::string(constants::runtime::recorder_pid_file))) ||
         std::filesystem::exists(
             runtime_file(std::string(constants::runtime::recorder_wav_file))) ||
         std::filesystem::exists(runtime_file(std::string(constants::runtime::state_file)));
}

void reset_config()
{
  config::Config cfg;
  cfg.language = std::string(constants::config::english_language);
  config::save_config(cfg);
}

void assert_control_command_does_not_record(const std::vector<std::string>& args)
{
  clean_runtime_files();
  ASSERT(app::run(args) == 0);
  ASSERT(runtime::get_status() == std::string(constants::runtime::idle_state));
  ASSERT(!recording_files_exist());
}

void assert_control_commands_do_not_record(std::span<const std::vector<std::string>> commands)
{
  for (const auto& command : commands) {
    assert_control_command_does_not_record(command);
  }
}

void stop_started_recording()
{
  std::ifstream pid_file(runtime_file(std::string(constants::runtime::recorder_pid_file)));
  pid_t pid = 0;
  pid_file >> pid;

  if (pid > 0) {
    if (pid != getpid()) {
      platform::stop_process(pid);
      platform::wait_process(pid);
    }
  }

  clean_runtime_files();
}

} // namespace

void run_test_app()
{
  model_store::write_default_model_and_vad();
  reset_config();
  clean_runtime_files();

  ASSERT(app::run({}) == 0);
  ASSERT(runtime::get_status() == std::string(constants::runtime::recording_state));
  ASSERT(recording_files_exist());
  stop_started_recording();

  const std::vector<std::vector<std::string>> control_commands = {
      {"status"},
      {"--language", "en"},
      {"--model", "list"},
      {"--model", "install", "base.en"},
      {"--model", "use", "base.en"},
      {"--model", "uninstall", "tiny.en"},
      {"--pipe-to", "tee -a ~/x.txt"},
      {"cancel"},
  };
  assert_control_commands_do_not_record(control_commands);

  auto cfg = config::load_config();
  ASSERT(cfg.pipe_to == std::string("tee -a ~/x.txt"));

  assert_control_command_does_not_record({"--no-pipe"});
  cfg = config::load_config();
  ASSERT(cfg.pipe_to == std::string(""));

  clean_runtime_files();
  ASSERT(app::run({"--output", "clipboard"}) == 1);
  ASSERT(runtime::get_status() == std::string(constants::runtime::idle_state));
  ASSERT(!recording_files_exist());

  ASSERT(app::run({"--output", "exec", "--pipe-to", "tee -a ~/x.txt"}) == 1);
  ASSERT(runtime::get_status() == std::string(constants::runtime::idle_state));
  ASSERT(!recording_files_exist());

  model_store::delete_default_model_and_vad();
  std::cout << "test_app passed\n";
}
