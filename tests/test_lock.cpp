#include "constants/constants.hpp"
#include "runtime/runtime.hpp"
#include "tests/tests.hpp"

#include <iostream>
#include <libassert/assert.hpp>

void run_test_lock()
{
  std::string status = runtime::get_status();
  ASSERT(status == std::string(constants::runtime::idle_state));

  std::cout << "test_lock passed\n";
}
