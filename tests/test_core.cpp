#include <gtest/gtest.h>

#include <Eigen/Dense>
#include <fmt/os.h>
#include <fmt/ostream.h>

#include <patrick/core.h>

using namespace patrick;

struct Hamming73Test : public ::testing::Test
{
  // Generator matrix
  static inline const Eigen::MatrixXi G = [] () {
    Eigen::MatrixXi G_{ 3, 7 };
    // clang-format off
        G_ << 1, 0, 0, 0, 1, 1, 1,
              0, 1, 0, 1, 0, 1, 1,
              0, 0, 1, 1, 1, 0, 1;
    // clang-format on
    return G_;
  }();

  // Parity matrix
  static inline const Eigen::MatrixXi H = [] () {
    Eigen::MatrixXi H{ 4, 7 };
    // clang-format off
      H << 0, 1, 1, 1, 0, 0, 0,
           1, 0, 1, 0, 1, 0, 0,
           1, 1, 0, 0, 0, 1, 0,
           1, 1, 1, 0, 0, 0, 1;
    // clang-format on
    return H;
  }();

  linearcode code = linearcode::from_generator (G);
};

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

TEST_F (Hamming73Test, TestEncoding)
{
  infoword i1{ "001" };
  codeword c1 = code.encode (i1);
  EXPECT_EQ (fmt::format ("{}", c1), "0011101");

  std::vector<codeword> expected;
  expected.emplace_back ("0000000");
  expected.emplace_back ("0011101");
  expected.emplace_back ("0101011");
  expected.emplace_back ("1000111");
  expected.emplace_back ("0110110");
  expected.emplace_back ("1011010");
  expected.emplace_back ("1101100");
  expected.emplace_back ("1110001");

  const auto &actual = *code.codewords ();
  EXPECT_EQ (expected.size (), actual.size ());
  for (const auto &c : expected)
    EXPECT_NE (std::ranges::find (actual, c), actual.cend ());
}

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

TEST_F (Hamming73Test, TestProperties)
{
  const auto &props = code.properties ();
  EXPECT_EQ (props.basis_size, 3);
  EXPECT_EQ (props.word_size, 7);
  EXPECT_EQ (props.min_distance, 4);
  EXPECT_EQ (props.max_errors_detect, 3);
  EXPECT_EQ (props.max_errors_correct, 1);
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

// TEST_F (Hamming74Test, TestDual)
//{
//   auto dual = linearcode::from_dual(code);

//  auto out = fmt::output_file ("codewords_of_hamming74_dual.txt");
//  int i = 0;
//  for (const auto &c : *dual.codewords ())
//    out.print ("{}:\t{}\n", i++, c);

//  const std::size_t word_size = 7;
//  const std::size_t num_limit = (1 << (word_size + 1)) - 1;
//  for (std::size_t i = 0; i < num_limit; ++i)
//    {
//      codeword c{ i, word_size };
//      EXPECT_NE (code.contains (c), dual.contains (c));
//    }
//}

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

TEST_F (Hamming73Test, TestPreparingSlepianTable)
{
  // Force preparing it.
  using enum linearcode::decoding_strategy;
  (void)code.decode<SlepyanTable> (codeword{ "0000000" });

  const unsigned long long total_num_words = 1 << code.properties ().word_size;
  const auto &table = *code.slepian_table ();
  for (auto i = 0ull; i < total_num_words; ++i)
    {
      const codeword c
          = code.encode (infoword{ i, code.properties ().basis_size });
      bool met = false;
      for (const auto &row : table)
        if ((met
             = row.leader == c
               || std::ranges::find (row.columns, c) != row.columns.cend ()))
          break;
      EXPECT_TRUE (met);
    }
}

TEST_F (Hamming73Test, TestDecodingWithSlepianTable)
{
  using enum linearcode::decoding_strategy;

  /// Decode a word which is part of the code.

  infoword i1{ "101" };
  codeword c1 = code.encode (i1);
  const auto d1 = code.decode<SlepyanTable> (c1);
  EXPECT_EQ (fmt::format ("{}", d1.error), "0000000");
  EXPECT_EQ (fmt::format ("{}", d1.iword), "101");

  /// Decode words which are not part of the code.

  const codeword c2{ "0010011" };
  const auto d2 = code.decode<SlepyanTable> (c2);
  EXPECT_EQ (fmt::format ("{}", d2.error), "0001110");
  EXPECT_EQ (fmt::format ("{}", d2.iword), "001");
  const auto c2_ = code.encode (infoword{ "001" });
  EXPECT_EQ (fmt::format ("{}", c2 + d2.error), fmt::format ("{}", c2_));

  const codeword c3{ "1111111" };
  const auto d3 = code.decode<SlepyanTable> (c3);
  EXPECT_EQ (fmt::format ("{}", d3.error), "0001110");
  EXPECT_EQ (fmt::format ("{}", d3.iword), "111");
  const auto c3_ = code.encode (infoword{ "111" });
  EXPECT_EQ (fmt::format ("{}", c3 + d3.error), fmt::format ("{}", c3_));
}

TEST_F (Hamming73Test, TestDecodingWithSyndromes)
{
  using enum linearcode::decoding_strategy;

  /// Decode a word which is part of the code.

  infoword i1{ "101" };
  codeword c1 = code.encode (i1);
  const auto d1 = code.decode<Syndromes> (c1);
  EXPECT_EQ (fmt::format ("{}", d1.error), "0000000");
  EXPECT_EQ (fmt::format ("{}", d1.iword), "101");

  /// Decode words which are not part of the code.

  const codeword c2{ "0010011" };
  const auto d2 = code.decode<Syndromes> (c2);
  EXPECT_EQ (fmt::format ("{}", d2.error), "0001110");
  EXPECT_EQ (fmt::format ("{}", d2.iword), "001");
  const auto c2_ = code.encode (infoword{ "001" });
  EXPECT_EQ (fmt::format ("{}", c2 + d2.error), fmt::format ("{}", c2_));

  const codeword c3{ "1111111" };
  const auto d3 = code.decode<Syndromes> (c3);
  EXPECT_EQ (fmt::format ("{}", d3.error), "0001110");
  EXPECT_EQ (fmt::format ("{}", d3.iword), "111");
  const auto c3_ = code.encode (infoword{ "111" });
  EXPECT_EQ (fmt::format ("{}", c3 + d3.error), fmt::format ("{}", c3_));
}

TEST_F (Hamming73Test, TestParityMatrix)
{
  const auto &parity_matrix = code.parity_matrix ();
  EXPECT_EQ (parity_matrix, H);
}

TEST_F (Hamming73Test, TestSyndromesAndContains)
{
  assert (code.codewords ().has_value ());
  for (const auto &c : *code.codewords ())
    EXPECT_TRUE (code.contains (c));

  codeword c1{ "0000001" };
  auto s1 = code.syndrome_of (c1);
  EXPECT_EQ (fmt::format ("{}", s1), "0001");
  EXPECT_FALSE (code.contains (c1));

  codeword c2{ "0110010" };
  auto s2 = code.syndrome_of (c2);
  EXPECT_EQ (fmt::format ("{}", s2), "0100");
  EXPECT_FALSE (code.contains (c2));

  codeword c3{ "0101011" };
  auto s3 = code.syndrome_of (c3);
  EXPECT_TRUE (code.contains (c3));
}
