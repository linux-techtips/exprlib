#pragma once

#include <exl/types.hpp>
#include <exl/option.hpp>

namespace exl::traits {

template <typename T>
concept Pattern = requires(T t) {
  {std::visit([](auto&& arg) { return; }, std::forward<T>(t))};
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

} // namespace exl::traits