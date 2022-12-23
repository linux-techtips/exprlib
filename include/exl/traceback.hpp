#pragma once

#include <exl/defer.hpp>
#include <exl/fmt.hpp>

#include <cstdlib>
#include <cstring>

#include <dlfcn.h>
#include <execinfo.h>
#include <unwind.h>

namespace exl {

// NOLINTBEGIN
template <auto MAX_DEPTH = 10> auto panic(std::string_view msg) -> void {
  println("[PANIC] - '{}'\n", msg);

  void *arr[MAX_DEPTH];

  auto trace_size = backtrace(arr, MAX_DEPTH);
  auto trace_symbols = backtrace_symbols(arr, trace_size);

  auto line_len = std::strlen(*trace_symbols) + 4;
  auto line = std::string(line_len, '=');

  auto backtrace_msg = std::string{"- Backtrace Symbols -"};

  println("{}", center(backtrace_msg, line_len));

  println("{}", line);
  for (auto i = 0; i < trace_size - 3; ++i) {
    println("| {} |", trace_symbols[i]);
  }
  println("{}\n", line);

  free(trace_symbols);
  std::abort();
}
// NOLINTEND

} // namespace exl