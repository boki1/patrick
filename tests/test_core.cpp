#include <gtest/gtest.h>

#include <Eigen/Dense>

#include <patrick/core.h>

using namespace patrick;

struct Hamming74Test : public ::testing::Test
{
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
