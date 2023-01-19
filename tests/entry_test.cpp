#include <exl/core.hpp>
#include <gtest/gtest.h>

using namespace exl; // NOLINT

TEST(fmt, TestPrint) {
  ::testing::internal::CaptureStdout();
  std::string_view world = "World";
  print("Hellol {}", world);
  ASSERT_EQ(::testing::internal::GetCapturedStdout(), "Hellol World");
}

TEST(fmt, TestPrintln) {
  ::testing::internal::CaptureStdout();
  std::string_view world = "World";
  println("Hellol {}", world);
  ASSERT_EQ(::testing::internal::GetCapturedStdout(), "Hellol World\n");
}

TEST(defer, TestDefer) {
  auto res = 0;
  auto _ = defer([&res]() { ++res; });
  ASSERT_EQ(res, 0);
}

TEST(iter, TestRangePositive) {
  ssize res = 0;
  for (auto i : Range(0, 3, 1)) {
    res += i;
  }
  ASSERT_EQ(res, 6);
}

TEST(iter, TestRangeNegative) {
  ssize res = 6; // NOLINT
  for (auto i : Range(3, 0, -1)) {
    res -= i;
  }
  ASSERT_EQ(res, 0);
}

TEST(iter, TestIter) {
  const auto len = 10;
  u8 arr[len]{0};                                   // NOLINT
  auto slice = Slice<u8>::from_unchecked(arr, len); // NOLINT

  for (auto &elem : slice) {
    ASSERT_EQ(elem, 0);
  }
}

TEST(iter, TestAsPtr) {
  const auto len = 10;
  u8 arr[len]{0};                            // NOLINT
  auto it = Iter<u8>::from_unchecked(arr);   // NOLINT
  auto cit = CIter<u8>::from_unchecked(arr); // NOLINT

  ASSERT_EQ(arr, it.as_ptr());
  ASSERT_EQ(arr, cit.as_ptr());
}

TEST(iter, TestAsRef) {
  const auto len = 10;
  u8 arr[len]{0};                            // NOLINT
  auto it = Iter<u8>::from_unchecked(arr);   // NOLINT
  auto cit = CIter<u8>::from_unchecked(arr); // NOLINT

  ASSERT_EQ(0, it.as_ref());
  ASSERT_EQ(0, cit.as_ref());
}

TEST(mem, TestCopy) {
  // NOLINTBEGIN
  const auto len = 3;
  u8 arr1[len]{1, 2, 3};
  u8 arr2[len]{0, 0, 0};

  auto res = ptr::copy(arr1, arr2, len);
  ASSERT_EQ(1, res[0]);
  ASSERT_EQ(2, res[1]);
  ASSERT_EQ(3, res[2]);
  // NOLINTEND
}

TEST(mem, TestCopyBytes) {
  // NOLINTBEGIN
  const auto len = 3;
  u8 arr1[len]{1, 2, 3};
  u8 arr2[len]{0, 0, 0};

  auto res = ptr::copy_bytes(arr1, arr2, len);
  ASSERT_EQ(1, res[0]);
  ASSERT_EQ(2, res[1]);
  ASSERT_EQ(3, res[2]);
  // NOLINTEND
}

TEST(mem, TestAsRef) {
  const auto len = 1;
  u8 arr[len]{0}; // NOLINT

  ASSERT_EQ(0, ptr::as_ref(arr)); // NOLINT
}

TEST(mem, TestAddr) {
  auto n = 10; // NOLINT
  auto n_addr = ptr::addr(n);

  ASSERT_EQ(&n, n_addr);

  auto n_addr_converted =
      ptr::addr<decltype(n)>(fmt::format("{}", fmt::ptr(&n)));

  ASSERT_EQ(&n, n_addr_converted);
}

TEST(mem, TestSliceAsPtr) {
  const auto len = 3;
  u8 arr[len]{1, 2, 3};                             // NOLINT
  auto slice = Slice<u8>::from_unchecked(arr, len); // NOLINT

  ASSERT_EQ(arr + 1, slice.as_ptr(1)); // NOLINT
}

TEST(mem, TestSliceAsRef) {
  const auto len = 3;
  u8 arr[len]{1, 2, 3};                             // NOLINT
  auto slice = Slice<u8>::from_unchecked(arr, len); // NOLINT

  ASSERT_EQ(arr[1], slice.as_ref(1));
}

TEST(mem, TestSliceSize) {
  const auto len = 3;
  u8 arr[len]{1, 2, 3};                             // NOLINT
  auto slice = Slice<u8>::from_unchecked(arr, len); // NOLINT

  ASSERT_EQ(len - 1, slice.size());
}

TEST(mem, TestSliceIter) {
  const auto len = 3;
  u8 arr[len]{0, 0, 0};                                    // NOLINT
  for (auto &elem : Slice<u8>::from_unchecked(arr, len)) { // NOLINT
    ASSERT_EQ(0, elem);
  }
}

TEST(mem, TestSliceIndex) {
  const auto len = 3;
  u8 arr[len]{1, 2, 3};                             // NOLINT
  auto slice = Slice<u8>::from_unchecked(arr, len); // NOLINT

  ASSERT_EQ(arr[1], slice[1]);
}

TEST(traits, IsPattern) {
  static_assert(traits::Pattern<Option<u8>>);
}

auto main(int argc, char **argv) -> int {
  panic_register();
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}