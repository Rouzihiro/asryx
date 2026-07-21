#include "config/config.hpp"
#include "constants/constants.hpp"
#include "tests/tests.hpp"

#include <iostream>
#include <libassert/assert.hpp>

void run_test_config()
{
  config::Config cfg = config::load_config();
  ASSERT(cfg.model == std::string(constants::config::default_model));
  ASSERT(cfg.language == std::string(constants::config::default_language));
  ASSERT(cfg.pipe_to == std::string(""));

  cfg.model = "small.en";
  cfg.pipe_to = "cat >/dev/null";
  config::save_config(cfg);

  config::Config cfg2 = config::load_config();
  ASSERT(cfg2.model == std::string("small.en"));
  ASSERT(cfg2.language == std::string(constants::config::default_language));
  ASSERT(cfg2.pipe_to == std::string("cat >/dev/null"));

  std::cout << "test_config passed\n";
}
