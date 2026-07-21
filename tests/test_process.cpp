#include "platform/process.hpp"
#include "tests/tests.hpp"

#include <iostream>
#include <libassert/assert.hpp>
#include <string>
#include <unistd.h>

void run_test_process()
{
  ASSERT(platform::command_exists("sh"));
  ASSERT(platform::run_process_blocking({"sh", "-c", "exit 0"}));
  ASSERT(!platform::run_process_blocking({"sh", "-c", "exit 7"}));
  ASSERT(platform::run_process_with_stdin({"sh", "-c", "cat >/dev/null"}, "hello"));
  ASSERT(!platform::run_process_with_stdin({"sh", "-c", "exit 7"}, "hello"));
  ASSERT(platform::is_process_running(getpid()));

  std::cout << "test_process passed\n";
}
