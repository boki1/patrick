/// \file

#ifndef PATRICK_WORD_H_INCLUDED
#define PATRICK_WORD_H_INCLUDED

#include <algorithm>
#include <bitset>
#include <stdexcept>
#include <vector>

#include <Eigen/Core>

#include <fmt/core.h>

namespace patrick
{

namespace details
{

class word_exception : public std::runtime_error
{
public:
  explicit word_exception (const std::string &msg)
      : std::runtime_error{ fmt::format ("word_exception: {}", msg) }
  {
  }
};

struct codeword_tag
{
};

struct infoword_tag
{
};

struct syndrome_tag
{
};

template <typename T> struct word
{
  ///
  /// Constructors
  ///

  word () = default;

  explicit word (const Eigen::RowVectorXi &t_vec) : vec{ t_vec } {}

  explicit word (Eigen::RowVectorXi &&t_vec) : vec{ std::move (t_vec) } {}

  ///
  /// \brief Construct a word from a different tag. This is the ctor used from
  /// coversion between a \ref codeword and an \ref infoword.
  ///
  template <typename U> explicit word (const word<U> &other) : vec{ other.vec }
  {
  }

  ///
  /// \brief Construct a word from a string which contains only '1' and '0'.
  ///
  explicit word (const std::string &bitstr)
  {
    const std::size_t cols = bitstr.size ();
    vec.resize (1, cols);

    std::size_t index = 0;
    for (const char b : bitstr)
      if (b == '0')
        vec (index++) = 0;
      else if (b == '1')
        vec (index++) = 1;
      else
        throw word_exception{ "bad input string" };
  }

  ///
  /// \brief Construct a word from a number (may as well be a bit literal such
  /// as 0b...).
  /// \param word_as_num The number.
  /// \param num_bits This parameters exist because there could be
  /// an unlimited amount of zeroes in the front of the number.
  ///
  word (unsigned long long word_as_num, std::size_t num_bits)
  {
    vec.resize (1, num_bits);

    for (std::size_t i = 0; i < num_bits; ++i)
      vec (num_bits - i - 1) = (word_as_num & (1 << i)) > 0;
  }

  ///
  /// \brief Make word<T> castable to the row vector type in Eigen.
  ///
  explicit operator Eigen::RowVectorXi () { return vec; }

  ///
  /// Comparison and ordering
  ///
  [[nodiscard]] bool
  operator== (const word &rhs) const noexcept
  {
    return vec == rhs.vec;
  }
  std::strong_ordering operator<=> (const word &) const noexcept = default;

  [[nodiscard]] std::size_t
  weight () const noexcept
  {
    return std::count (std::cbegin (vec), std::cend (vec), 1);
  }

  [[nodiscard]] unsigned long long
  to_ullong () const noexcept
  {
    auto result = 0ull;
    for (long i = 0; i < vec.size (); ++i)
      result |= vec (vec.size () - i - 1) << i;
    return result;
  }

  ///
  /// Arithmetics
  /// \note The operations are mod 2.
  ///

  template <typename U>
  word<T>
  operator+ (const word<U> &rhs) const
  {
    word<T> copy{ vec };
    return word<T>{ copy.vec.operator+ (rhs.vec).unaryExpr (
        [&] (const int x) { return x % 2; }) };
  }

  template <typename U>
  word<T>
  operator+= (const word<U> &rhs)
  {
    vec = vec.operator+ (rhs.vec).unaryExpr (
        [&] (const int x) { return x % 2; });
    return *this;
  }

  Eigen::RowVectorXi vec;
};

} // namespace details

using codeword = details::word<details::codeword_tag>;
using infoword = details::word<details::infoword_tag>;
using syndrome = details::word<details::syndrome_tag>;

} // namespace patrick

template <typename Tag>
struct fmt::formatter<patrick::details::word<Tag> >
    : fmt::formatter<std::string_view>
{
  using word_type = patrick::details::word<Tag>;

  template <typename FormatContext>
  auto
  format (const word_type &w, FormatContext &ctx) const
  {
    std::string bitstr;
    bitstr.resize (w.vec.cols ());
    std::size_t index = 0;
    for (const auto &b : w.vec)
      bitstr[index++] = b ? '1' : '0';
    return format_to (ctx.out (), "{}", bitstr);
  }
};

#endif // PATRICK_WORD_H_INCLUDED
