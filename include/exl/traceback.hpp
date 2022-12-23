#pragma once

#include <exl/fmt.hpp>

#include <csignal>
#include <cstdlib>
#include <cstring>

#include <dlfcn.h>
#include <execinfo.h>
#include <ucontext.h>
#include <unwind.h>

namespace exl {

// NOLINTBEGIN
namespace {
volatile bool REGISTERED = false;
}
// NOLINTEND

static constexpr const auto MAX_DEPTH = 10;
static constexpr const auto MAX_LINE_LEN = 61;
static constexpr const auto TRACE_IGNORE = 8;

// TODO (Carter) panic_handler is platform dependant
// TODO (Carter) add verbosity level to panic_handler
// TODO (Carter) allocing strings won't work when OOM or other alloc issues
// formatting NOLINTBEGIN
auto inline panic_handler(int signo, siginfo_t *info, void *extra) -> void {

  // TODO (Carter) Greg dump is platform dependent

  void *arr[MAX_DEPTH];

  auto trace_size = backtrace(arr, MAX_DEPTH);
  auto trace_symbols = backtrace_symbols(arr, trace_size);

  auto line_len = MAX_LINE_LEN;
  auto line = std::string(line_len, '=');

  auto backtrace_msg = std::string{"- Backtrace Symbols -"};

  eprintln("{}", center(backtrace_msg, line_len));

  eprintln("{}", line);
  for (auto i = 0; i < trace_size; ++i) {
    eprintln(" {}", trace_symbols[i]);
  }
  eprintln("{}\n", line);

  free(trace_symbols);

  auto panic_ctx = reinterpret_cast<ucontext_t *>(extra);
  auto panic_ins = fmt::format(
      "{}", reinterpret_cast<void *>(panic_ctx->uc_mcontext.gregs[REG_RIP]));

  backtrace_msg = std::string{"- Bad Instruction -"};
  eprintln("{}", center(backtrace_msg, line_len));
  eprintln("{}", line);
  eprintln(" [{}]", panic_ins);
  eprintln("{}", line);

  std::signal(signo, SIG_DFL);
}

// NOLINTEND

auto inline panic_register() -> void {
  struct sigaction action {};
  action.sa_flags = SA_SIGINFO;
  action.sa_sigaction = panic_handler;

  if (sigaction(SIGFPE, &action, nullptr) == -1) {
    perror("sigfpe: sigaction");
    std::exit(1);
  }

  if (sigaction(SIGSEGV, &action, nullptr) == -1) {
    perror("sigsegv: sigaction");
    std::exit(1);
  }

  if (sigaction(SIGABRT, &action, nullptr) == -1) {
    perror("sigabrt: sigaction");
    std::exit(1);
  }

  if (sigaction(SIGILL, &action, nullptr) == -1) {
    perror("sigill: sigaction");
    std::exit(1);
  }

  if (sigaction(SIGBUS, &action, nullptr) == -1) {
    perror("sigbus: sigaction");
    std::exit(1);
  }
}

auto inline panic(std::string_view msg) -> void {
  eprintln("\n[PANIC] - '{}'\n", msg);
  if (!REGISTERED) {
    panic_register();
    REGISTERED = true;
  }
  std::raise(SIGABRT);
}

} // namespace exl