/* MIT License

Copyright (c) 2017 Vittorio Romeo

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE. */

/* Modified by Carter Vavra */
/* Original Talk https://www.youtube.com/watch?v=3KyW5Ve3LtI */

#pragma once

#include <exl/types.hpp>
#include <fmt/core.h>
#include <functional>
#include <tuple>
#include <variant>

namespace exl::traits {
template <typename T>
concept Matchable = requires(T t) {
  {std::visit([](auto &&arg) { return; }, t)};
};

} // namespace exl::traits

namespace exl {

template <typename... Ts> using Union = std::variant<Ts...>;

template <typename TF, typename... TFs>
struct OverloadSet : TF, OverloadSet<TFs...> {
  using TF::operator();
  using OverloadSet<TFs...>::operator();

  template <typename TFFwd, typename... TRest>
  constexpr explicit OverloadSet(TFFwd &&f, TRest &&...rest)
      : TF{std::forward<TFFwd>(f)}, OverloadSet<TFs...>{
                                        std::forward<TRest>(rest)...} {}
};

template <typename TF> struct OverloadSet<TF> : TF {
  using TF::operator();

  template <typename TFFwd>
  constexpr explicit OverloadSet(TFFwd &&f) : TF{std::forward<TFFwd>(f)} {}
};

template <typename... TFs>
constexpr auto overload(TFs &&...fs) -> decltype(auto) {
  return OverloadSet<std::remove_reference_t<TFs>...>(std::forward<TFs>(fs)...);
}

template <typename... TVariants>
constexpr auto match(TVariants &&...vs) -> decltype(auto) {
  return [&vs...](auto &&...fs) -> decltype(auto) {
    auto visitor = overload(std::forward<decltype(fs)>(fs)...);
    return std::visit(visitor, std::forward<TVariants>(vs)...);
  };
}

template <typename TVisitor, typename... TVariants>
constexpr auto visit_recursively(TVisitor &&visitor, TVariants &&...variants)
    -> decltype(auto) {
  return std::visit(std::forward<TVisitor>(visitor),
                    std::forward<TVariants>(variants)._data...);
}

template <typename TF> struct yCombinator {
  TF f;

  template <typename TFFwd>
  constexpr explicit yCombinator(TFFwd &&_f) : f{std::forward<TFFwd>(_f)} {}

  template <typename... Ts>
  constexpr auto operator()(Ts &&...xs) -> decltype(auto) {
    return f(std::ref(*this), std::forward<Ts>(xs)...);
  }
};

template <typename TF> constexpr auto ycombinator(TF &&f) -> decltype(auto) {
  return yCombinator<std::decay_t<TF>>(std::forward<TF>(f));
}

template <typename TReturn, typename... TFs>
constexpr auto make_recursive_visitor(TFs &&...fs) -> decltype(auto) {
  return ycombinator([o = overload(std::forward<TFs>(fs)...)](
                         auto self, auto &&...xs) -> TReturn {
    return o(
        [&self](auto &&...vs) {
          return visit_recursively(self, std::forward<decltype(vs)>(vs)...);
        },
        std::forward<decltype(xs)>(xs)...);
  });
}

template <typename TReturn, typename... TVariants>
constexpr auto match_recursively(TVariants &&...vs) -> decltype(auto) {
  return [&vs...](auto &&...fs) -> TReturn {
    auto visitor =
        make_recursive_visitor<TReturn>(std::forward<decltype(fs)>(fs)...);
    return visit_recursively(visitor, std::forward<TVariants>(vs)...);
  };
}

} // namespace exl

template <typename... Ts> struct fmt::formatter<exl::Union<Ts...>> {

  constexpr auto parse(format_parse_context &ctx) -> decltype(ctx.begin()) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const exl::Union<Ts...> &e, FormatContext &ctx) const
      -> decltype(ctx.out()) {

    return std::visit(
        [&ctx](auto &&arg) { return fmt::format_to(ctx.out(), "{}", arg); }, e);
  }
};
