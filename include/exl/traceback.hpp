#pragma once

#include <exl/fmt.hpp>

#include <csignal>
#include <atomic>

#include <ucontext.h>
#include <execinfo.h>

namespace exl {

static constexpr auto MAX_DUMP = 17;

// NOLINTBEGIN
namespace {
  std::atomic<bool> REGISTERED (false);
}
// NOLINTEND

auto inline panic(const std::string_view msg, const bool warn = true) -> void {
  if (warn && !REGISTERED) {
    eprintln("[WARN] Call panic_register() for debug info...\n");
  }
  eprintln("[PANIC] - {}\n", msg);
  if (raise(SIGUSR1) != 0) {
    std::abort();
  }  
}

template <auto DSize = MAX_DUMP>
static auto panic_handler(int signo, siginfo_t* info, void* extra) -> void {
  #if SYSTEM_LINUX

  auto ctx = (ucontext_t*)(extra); // NOLINT
  
  #if defined(__i386__)
    auto ill_ins = (void*)(ctx->uc_mcontext.gregs[14]); // NOLINT
  #elif defined(__X86_64__) || defined(__x86_64__)
    auto ill_ins = (void*)(ctx->uc_mcontext.gregs[REG_RIP]); // NOLINT
  #elif defined(__ia64__)
    auto ill_ins = (void*)(uc->uc_mcontext.sc_ip); // NOLINT
  #endif

  eprintln("\nLast Instruction: {}\n", fmt::ptr(ill_ins)); // NOLINT

  #endif
 
  void* arr[DSize]; // NOLINT
  auto trace_size = backtrace(arr, DSize);
  auto trace_symbols = backtrace_symbols(arr, trace_size);

  eprintln("Backtrace Symbols");
  for (auto i = 0; i < trace_size; ++i) {
    eprintln("{}", trace_symbols[i]);
  }

  std::free(trace_symbols); // NOLINT
  std::signal(signo, SIG_DFL); // This will leave the panic_handler function out of the core dump
}

template <auto DSize = MAX_DUMP>
auto panic_register() -> void {
  if (!REGISTERED) {
    REGISTERED.exchange(true);
  }

  struct sigaction action {};
  action.sa_flags = SA_SIGINFO;
  action.sa_sigaction = panic_handler<DSize>; // NOLINT

  if (sigaction(SIGUSR1, &action, nullptr) == -1) {
    perror("sigusr1: sigaction");
    std::exit(SIGUSR1);
  }

  if (sigaction(SIGFPE, &action, nullptr) == -1) {
    perror("sigfpe: sigaction");
    std::exit(SIGFPE);
  }

  if (sigaction(SIGSEGV, &action, nullptr) == -1) {
    perror("sigfpe: sigaction");
    std::exit(SIGSEGV);
  }

  if (sigaction(SIGABRT, &action, nullptr) == -1) {
    perror("sigabrt: sigaction");
    std::exit(SIGABRT);
  }

  if (sigaction(SIGILL, &action, nullptr) == -1) {
    perror("sigill: sigaction");
    std::exit(SIGILL);
  }

  if (sigaction(SIGBUS, &action, nullptr) == -1) {
    perror("sigbus: sigaction");
    std::exit(SIGBUS);
  }
}

} // namespace exl