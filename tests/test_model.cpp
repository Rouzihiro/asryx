#include "config/config.hpp"
#include "constants/constants.hpp"
#include "model/model.hpp"
#include "tests/model_store.hpp"
#include "tests/tests.hpp"

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <libassert/assert.hpp>
#include <stdexcept>

namespace {

template <typename Fn> bool runtime_error_equals(Fn fn, const std::string& expected)
{
  try {
    fn();
  }
  catch (const std::runtime_error& e) {
    return std::string(e.what()) == expected;
  }

  return false;
}

} // namespace

void run_test_model()
{
  const auto& supported = model::get_supported_models();
  ASSERT(!supported.empty());
  ASSERT(std::find(supported.begin(), supported.end(),
                   std::string(constants::config::default_model)) != supported.end());

  const auto& languages = model::get_supported_languages();
  ASSERT(std::find(languages.begin(), languages.end(), std::string("fr")) != languages.end());
  ASSERT(std::find(languages.begin(), languages.end(), std::string("ar")) != languages.end());
  ASSERT(std::find(languages.begin(), languages.end(), std::string("yue")) != languages.end());
  ASSERT(model::is_supported_language(std::string(constants::config::auto_language)));
  ASSERT(!model::is_supported_language("jrnfejfef"));

  std::string path = model::get_model_path(std::string(constants::config::default_model));
  ASSERT(!path.empty());
  ASSERT(!model::get_vad_model_path().empty());
  ASSERT(!model::is_vad_model_installed());
  ASSERT(runtime_error_equals([] { model::validate_vad_model(); },
                              "VAD model is not installed: " + model::get_vad_model_path()));

  ASSERT(!model::is_model_installed(std::string(constants::config::default_model)));
  ASSERT(runtime_error_equals(
      [] { model::use_model(std::string(constants::config::default_model)); },
      "model 'base.en' is not installed. Install it with: asryx --model install base.en"));

  model_store::write_model(std::string(constants::config::default_model));
  model::use_model(std::string(constants::config::default_model));
  model::use_language(std::string(constants::config::english_language));

  auto cfg = config::load_config();
  ASSERT(cfg.model == std::string(constants::config::default_model));
  ASSERT(cfg.language == std::string(constants::config::english_language));
  ASSERT(model::transcription_language_for(cfg) ==
         std::string(constants::config::english_language));

  ASSERT(runtime_error_equals([] { model::use_language("jrnfejfef"); },
                              "unsupported language: jrnfejfef"));
  cfg = config::load_config();
  ASSERT(cfg.language == std::string(constants::config::english_language));

  ASSERT(runtime_error_equals(
      [] { model::use_language("fr"); },
      "active model base.en is English-only; use a multilingual model for fr"));
  cfg = config::load_config();
  ASSERT(cfg.language == std::string(constants::config::english_language));

  model_store::write_model("base");
  model::use_model("base");
  model::use_language("fr");
  cfg = config::load_config();
  ASSERT(cfg.model == std::string("base"));
  ASSERT(cfg.language == std::string("fr"));
  ASSERT(model::transcription_language_for(cfg) == std::string("fr"));

  ASSERT(runtime_error_equals(
      [] { model::use_model(std::string(constants::config::default_model)); },
      "active model base.en is English-only; use a multilingual model for fr"));
  cfg = config::load_config();
  ASSERT(cfg.model == std::string("base"));
  ASSERT(cfg.language == std::string("fr"));

  model::use_language(std::string(constants::config::auto_language));
  cfg = config::load_config();
  ASSERT(model::transcription_language_for(cfg) == std::string(""));

  model::use_model(std::string(constants::config::default_model));

  bool active_uninstall_rejected = false;
  try {
    model::uninstall_model(std::string(constants::config::default_model));
  }
  catch (const std::runtime_error& e) {
    active_uninstall_rejected =
        std::string(e.what()) ==
        "cannot uninstall active model 'base.en'; switch models first with: asryx --model use "
        "<other>";
  }
  ASSERT(active_uninstall_rejected);
  ASSERT(std::filesystem::exists(path));

  std::cout << "test_model passed\n";
}
