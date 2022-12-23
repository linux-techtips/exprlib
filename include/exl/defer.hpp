#pragma once

#include <exl/pattern.hpp>
#include <functional>

namespace exl {

template <typename TF>
requires std::is_invocable_v<TF>
struct Defer : TF { // NOLINT
  using TF::operator();

  template <typename TFFwd>
  [[nodiscard]] constexpr explicit Defer(TFFwd &&fn)
      : TF{std::forward<TFFwd>(fn)} {}

  ~Defer() { std::invoke(std::forward<TF>(*this)); }
};

template <typename TF>
requires std::is_invocable_v<TF>
[[nodiscard]] constexpr auto defer(TF &&fn) -> decltype(auto) {
  return Defer<std::decay_t<TF>>(std::forward<TF>(fn));
}

} // namespace exl