/// \file

#ifndef PATRICK_WORD_H_INCLUDED
#define PATRICK_WORD_H_INCLUDED

#include <bitset>
#include <stdexcept>
#include <vector>

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

template <typename T> struct word
{
  explicit word (const std::vector<bool> &t_data) : data{ t_data } {}

  template <typename U>
  explicit word (const word<U> &other) : data{ other.data }
  {
  }

  explicit word (const std::string &bitstr)
  {
    data.reserve (bitstr.size ());
    for (const char b : bitstr)
      if (b == '0')
        data.push_back (false);
      else if (b == '1')
        data.push_back (true);
      else
        throw word_exception{ "bad input string" };
  }

  explicit operator std::vector<bool> () { return data; }

  [[nodiscard]] bool
  operator== (const word &rhs) const noexcept
  {
    return data == rhs.data;
  }
  std::strong_ordering operator<=> (const word &) const noexcept = default;

  std::vector<bool> data;
};

} // namespace details

using codeword = details::word<details::codeword_tag>;
using infoword = details::word<details::infoword_tag>;

} // namespace patrick

template <typename Tag>
struct fmt::formatter<patrick::details::word<Tag> >
    : fmt::formatter<std::string_view>
{
  using word_type = patrick::details::word<Tag>;

  template <typename FormatContext>
  auto
  format (const word_type &w, FormatContext &ctx)
  {
    std::string bitstr;
    bitstr.reserve (w.data.size ());
    for (const auto &b : w.data)
      bitstr.push_back ((b ? '1' : '0'));
    return format_to (ctx.out (), "{}", bitstr);
  }
};

#endif // PATRICK_WORD_H_INCLUDED
