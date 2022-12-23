#pragma once

#include <exl/option.hpp>
#include <exl/types.hpp>
#include <type_traits>

#include <variant>

namespace exl::traits {

template <template <typename> typename T, typename U = int>
concept Templated = std::is_same_v < T<U>,
typename T<U>::Self >
    &&std::is_same_v<U, typename T<U>::Val> &&std::is_same_v<
        U *, typename T<U>::Ptr> &&std::is_same_v<U &, typename T<U>::Ref>;

template <typename T, typename... Args>
concept Unchecked = requires(T t, Args &&...args) {
  { T::from_unchecked(std::forward<Args...>(args...)) } -> std::same_as<T>;
};

template <typename Opt, typename T, typename... Args>
concept Checked = requires(T t, Args &&...args) {
  { T::from(std::forward<Args...>(args...)) } -> std::same_as<Opt>;
};

template <typename Opt, typename T, typename... Args>
concept From = Unchecked<T, Args...> && Checked<Opt, T, Args...>;

template <typename T>
concept Default = requires(T t) {
  { T::init() } -> std::same_as<T>;
};

template <typename Opt, typename T, typename... Args>
concept ExlCompliant = From<Opt, T, Args...> &&
    std::is_constructible_v<T, Args...> && !std::is_default_constructible_v<T>;

template <typename Opt, template <typename> typename T, typename... Args,
          typename U = int>
concept ExlTemplateCompliant =
    ExlCompliant<Opt, T<int>, Args...> && Templated<T, U>;

template <typename T>
concept Iterator = requires(T t) {
  {t.next()};
};

template <typename T>
concept Matchable = requires(T t) {
  {std::visit(t)};
};

template <typename T>
concept Sized = requires(T t) {
  {t.size()};
};

template <typename T>
concept ToPointer = requires(T ptr) {
  {ptr.as_ptr()};
};

template <typename T>
concept Range = requires(T ptr) {
  {ptr.begin()};
  {ptr.end()};
};

template <typename Iter, typename T>
concept Iterable = requires(Iter it) {
  { it.next() } -> std::same_as<Option<T>>;
};

} // namespace exl::traits