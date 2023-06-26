#include <type_traits>

#include <fmt/core.h>
#include <gtest/gtest.h>

#include <patrick/word.h>

using namespace patrick;

///
/// Helpers
///

// clang-format off
template <typename T, typename U>
concept equality_comparable = requires (T a, U b)
{
  { a == b } -> std::same_as<bool>;
};
// clang-format on

TEST (Patrick, TestWordConstruction)
{
  codeword c1{ "0101" };
  codeword c2{ "0101" };
  codeword c3{ "1010" };
  EXPECT_EQ (c1, c2);
  EXPECT_NE (c1, c3);
  EXPECT_EQ (fmt::format ("{}", c1), "0101");
  EXPECT_EQ (fmt::format ("{}", c3), "1010");

  infoword i1{ "0101" };
  infoword i2{ "0101" };
  infoword i3{ "1010" };
  EXPECT_EQ (i1, i2);
  EXPECT_NE (i1, i3);
  EXPECT_EQ (fmt::format ("{}", i1), "0101");
  EXPECT_EQ (fmt::format ("{}", i3), "1010");

  static_assert (!std::is_convertible_v<infoword, codeword>,
                 "infoword is convertible to codeword");
  static_assert (!std::is_convertible_v<codeword, infoword>,
                 "codeword is convertible to infoword");
  static_assert (!equality_comparable<infoword, codeword>,
                 "infoword is convertible to codeword");
  static_assert (!equality_comparable<codeword, infoword>,
                 "codeword is convertible to infoword");

  codeword c4{ i1 };
  std::vector<bool> expected_data{ false, true, false, true };
  EXPECT_EQ (c4.data, a);
}
