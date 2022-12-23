#pragma once

#include <exl/fmt.hpp>
#include <exl/pattern.hpp>
#include <exl/traceback.hpp>

namespace exl {

using None = std::monostate;

template <typename T> struct Option : Union<T, None> {
  using Self = Union<T, None>;

  [[nodiscard]] constexpr auto is_some() const -> bool {
    if (std::holds_alternative<T>(*this)) {
      return true;
    }
    return false;
  }

  [[nodiscard]] constexpr auto is_none() const -> bool { return !is_some(); }

  [[nodiscard]] constexpr auto unwrap() const -> T {
    if (this->is_none()) {
      panic("Bad unwrap of Optional");
    }
    return std::get<T>(*this);
  }

  [[nodiscard]] constexpr auto unwrap_or(const T &data) -> T {
    return match(*this)([](const T &some) { return some; },
                        [&data](None _) { return data; });
  }

  [[nodiscard]] constexpr auto unwrap_or_default() -> T {
    return match(*this)([](const T &some) { return some; },
                        [](None _) { return T{}; });
  }

  [[nodiscard]] constexpr auto unwrap_or_else(const std::function<T(void)> &fn)
      -> T {
    return match(*this)([](const T &ok) { return ok; },
                        [&fn](None _) { return fn(); });
  }

  [[nodiscard]] constexpr auto expext(std::string_view msg) -> T {
    if (this->is_none()) {
      panic(msg);
    }
    return std::get<T>(*this);
  }

  [[nodiscard]] constexpr auto take() -> Option<T> {
    auto ret = *this;
    std::swap(*this, Option());
    return ret;
  }

  [[nodiscard]] constexpr auto replace(const T &data) -> Option<T> {
    auto ret = *this;
    std::swap(*this, Option(data));
    return ret;
  }

  [[nodiscard]] constexpr auto contains(const T &data) const -> bool {
    return match(*this)([&data](const T &some) { return some == data; },
                        [](None _) { return false; });
  }

  // NOLINTBEGIN
  [[nodiscard]] constexpr friend auto operator<=>(const Option &rhs,
                                                  const Option &lhs) = default;

  [[nodiscard]] constexpr Option() : Self{None{}} {}
  [[nodiscard]] constexpr Option(const T &some) : Self{some} {}
  // NOLINTEND
};

template <typename T, typename E> struct Result : Union<T, E> {
  using Self = Union<T, E>;

  [[nodiscard]] constexpr auto is_ok() const -> bool {
    if (std::holds_alternative<T>(*this)) {
      return true;
    }
    return false;
  }

  [[nodiscard]] constexpr auto is_err() const -> bool { return !this->is_ok(); }

  [[nodiscard]] constexpr auto ok() const -> Option<T> {
    if (this->is_ok()) {
      return {std::get<T>(*this)};
    }
    return {};
  }

  [[nodiscard]] constexpr auto err() const -> Option<T> {
    if (this->is_err()) {
      return {std::get<E>(*this)};
    }
    return {};
  }

  [[nodiscard]] constexpr auto unwrap() const -> T {
    if (this->is_err()) {
      panic("Bad unwrap of Result");
    }
    return std::get<T>(*this);
  }

  [[nodiscard]] constexpr auto unwrap_or(const T &data) const -> T {
    return match(*this)([](const T &ok) { return ok; },
                        [&data](const E &err) { return data; });
  }

  [[nodiscard]] constexpr auto unwrap_or_default() const -> T {
    return match(*this)([](const T &ok) { return ok; },
                        [](const E &err) { return T{}; });
  }

  [[nodiscard]] constexpr auto
  unwrap_or_else(const std::function<T(void)> &fn) const -> T {
    return match(*this)([](const T &ok) { return ok; },
                        [&fn](const E &err) { return fn(); });
  }

  [[nodiscard]] constexpr auto expect(std::string_view msg) const -> T {
    if (this->is_err()) {
      panic(msg);
    }
    return std::get<T>(*this);
  }

  // NOLINTBEGIN
  [[nodiscard]] constexpr Result(const T &ok) : Self{ok} {}
  [[nodiscard]] constexpr Result(const E &err) : Self{err} {}
  // NOLINTEND
};

} // namespace exl

template <typename T> struct fmt::formatter<exl::Option<T>> {

  constexpr auto parse(format_parse_context &ctx) -> decltype(ctx.begin()) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const exl::Option<T> &opt, FormatContext &ctx) const
      -> decltype(ctx.out()) {
    return exl::match(opt)(
        [&](T data) { return fmt::format_to(ctx.out(), "Some({})", data); },
        [&](exl::None _) { return fmt::format_to(ctx.out(), "None"); });
  }
};

template <typename T, typename E> struct fmt::formatter<exl::Result<T, E>> {

  constexpr auto parse(format_parse_context &ctx) -> decltype(ctx.begin()) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const exl::Result<T, E> &res, FormatContext &ctx) const
      -> decltype(ctx.out()) {
    return exl::match(res)(
        [&](T ok) { return fmt::format_to(ctx.out(), "Ok({})", ok); },
        [&](E err) { return fmt::format_to(ctx.out(), "Err({})", err); });
  }
};