#include <gtest/gtest.h>

#include <Eigen/Dense>
#include <fmt/os.h>
#include <fmt/ostream.h>

#include <patrick/core.h>

using namespace patrick;

struct Hamming74Test : public ::testing::Test
{
  // Generator matrix
  static inline const Eigen::MatrixXi G = [] () {
    Eigen::MatrixXi G_{ 4, 7 };
    // clang-format off
        G_ << 1, 0, 0, 0, 0, 1, 1,
              0, 1, 0, 0, 1, 0, 1,
              0, 0, 1, 0, 1, 1, 0,
              0, 0, 0, 1, 1, 1, 1;
    // clang-format on
    return G_;
  }();

  // Parity matrix
  static inline const Eigen::MatrixXi H = [] () {
    Eigen::MatrixXi H{ 3, 7 };
    // clang-format off
        H << 0, 1, 1, 1, 1, 0, 0,
			 1, 0, 1, 1, 0, 1, 0,
			 1, 1, 0, 1, 0, 0, 1;
    // clang-format on
    return H;
  }();

  linearcode code = linearcode::from_generator (G);
};

struct Hamming84Test : public ::testing::Test
{
  static inline const Eigen::MatrixXi G = [] () {
    Eigen::MatrixXi G_{ 4, 8 };
    // clang-format off
        G_ << 1, 0, 0, 0, 0, 1, 1, 1,
              0, 1, 0, 0, 1, 0, 1, 1,
              0, 0, 1, 0, 1, 1, 0, 1,
              0, 0, 0, 1, 1, 1, 1, 0;
    // clang-format on
    return G_;
  }();

  linearcode code = linearcode::from_generator (G);
};

#ifdef DEBUG
TEST_F (Hamming74Test, TestCachedCodewords)
{
  auto out = fmt::output_file ("codewords_of_hamming74.txt");
  int i = 0;
  for (const auto &c : *code.codewords ())
    out.print ("{}:\t{}\n", i++, c);
}

TEST_F (Hamming84Test, TestCachedCodewords)
{
  auto out = fmt::output_file ("codewords_of_hamming84.txt");
  int i = 0;
  for (const auto &c : *code.codewords ())
    out.print ("{}:\t{}\n", i++, c);
}
#endif

TEST_F (Hamming74Test, TestEncoding)
{
  infoword i1{ "1011" };
  codeword c1 = code.encode (i1);
  EXPECT_EQ (fmt::format ("{}", c1), "1011010");

  infoword i2{ "1001" };
  codeword c2 = code.encode (i2);
  EXPECT_EQ (fmt::format ("{}", c2), "1001100");
}

TEST_F (Hamming84Test, TestEncoding)
{
  infoword i1{ "1011" };
  codeword c1 = code.encode (i1);
  EXPECT_EQ (fmt::format ("{}", c1), "10110100");

  infoword i2{ "1001" };
  codeword c2 = code.encode (i2);
  EXPECT_EQ (fmt::format ("{}", c2), "10011001");
}

TEST_F (Hamming74Test, TestProperties)
{
  const auto &props = code.properties ();
  EXPECT_EQ (props.basis_size, 4);
  EXPECT_EQ (props.word_size, 7);
  EXPECT_EQ (props.min_distance, 3);
  EXPECT_EQ (props.max_errors_detect, 2);
  EXPECT_EQ (props.max_errors_correct, 1);
}

TEST_F (Hamming84Test, TestProperties)
{
  const auto &props = code.properties ();
  EXPECT_EQ (props.basis_size, 4);
  EXPECT_EQ (props.word_size, 8);
  EXPECT_EQ (props.min_distance, 4);
  EXPECT_EQ (props.max_errors_detect, 3);
  EXPECT_EQ (props.max_errors_correct, 1);
}
TEST_F (Hamming74Test, TestParityMatrix)
{
  const auto &parity_matrix = code.parity_matrix ();
  EXPECT_EQ (parity_matrix, H);
}

TEST_F (Hamming74Test, TestSyndromesAndContains)
{
  assert (code.codewords ().has_value ());
  for (const auto &c : *code.codewords ())
    EXPECT_TRUE (code.contains (c));

  codeword c1{ "0001101" };
  auto s1 = code.syndrome_of (c1);
  EXPECT_EQ (fmt::format ("{}", s1), "010");
  EXPECT_FALSE (code.contains (c1));

  codeword c2{ "1001100" };
  auto s2 = code.syndrome_of (c2);
  EXPECT_EQ (fmt::format ("{}", s2), "000");
  EXPECT_TRUE (code.contains (c2));
}
