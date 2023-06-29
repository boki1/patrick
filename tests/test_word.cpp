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

TEST (TestPatrick, TestWordConstruction)
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
}

TEST (TestPatrick, TestWordInt)
{
  std::vector<codeword> expected;
  expected.emplace_back ("0000");
  expected.emplace_back ("0001");
  expected.emplace_back ("0010");
  expected.emplace_back ("0011");
  expected.emplace_back ("0100");
  expected.emplace_back ("0101");
  expected.emplace_back ("0110");
  expected.emplace_back ("0111");
  expected.emplace_back ("1000");
  expected.emplace_back ("1001");
  expected.emplace_back ("1010");
  expected.emplace_back ("1011");
  expected.emplace_back ("1100");
  expected.emplace_back ("1101");
  expected.emplace_back ("1110");
  expected.emplace_back ("1111");

  std::vector<codeword> actual;
  actual.reserve (16);
  for (unsigned i = 0; i < 16; ++i)
    actual.emplace_back (i, 4);

  for (unsigned i = 0; i < 16; ++i)
    EXPECT_EQ (i, actual[i].to_ullong ());

  for (int i = 0; i < 16; ++i)
    EXPECT_EQ (fmt::format ("{}", expected[i]), fmt::format ("{}", actual[i]));
}
