/// \file

#ifndef LIVEDEMO_CHANNEL_H_INCLUDED
#define LIVEDEMO_CHANNEL_H_INCLUDED

#include <optional>

#include <patrick/core.h>

namespace livedemo
{

///
/// \class command_line_exception
/// \brief Indicates an exceptional behaviour during
///        an operation of a \ref command_line instance.
///
class channel_exception : public std::runtime_error
{
public:
  explicit channel_exception (const std::string &msg)
      : std::runtime_error{ fmt::format ("channel_exception: {}", msg) }
  {
  }
};

///
/// \brief Base class for channel types that may or may not ever exist.
/// Its only capability is to send information and has the property called
/// \a crossover probability which is the probability of any single bit that is
/// transfferd being flipped.
///
class channel
{
public:
  virtual ~channel () noexcept = default;

  using infoword_type = patrick::infoword;
  using codeword_type = patrick::codeword;

  virtual std::optional<patrick::linearcode::decoding_result>
  transfer (const infoword_type &sent, patrick::linearcode &code) = 0;

  virtual double crossover_probability () const noexcept = 0;
};

///
/// \brief Binary symmetric channel mock.
///
class binary_symm_channel final : public channel
{
public:
  ///
  /// Helpers
  ///

  static double binary_entropy_function (double);

  static double evaluate_capacity (double p);

public:
  ///
  /// Special member functions
  ///
  explicit binary_symm_channel (double p = 0.3) : m_crossover_probability{ p }
  {
  }

public:
  ///
  /// Operations
  ///

  [[nodiscard]] std::optional<patrick::linearcode::decoding_result>
  transfer (const infoword_type &sent, patrick::linearcode &code) override
  {
    try
      {
        codeword_type encoded = code.encode (sent);
        with_noise (encoded);
        return code.decode (encoded);
      }
    catch (const patrick::linearcode_exception &le)
      {
        return std::nullopt;
      }
  }

private:
  void
  with_noise (codeword_type &)
  {
  }

public:
  ///
  /// Properties
  ///

  [[nodiscard]] double
  capacity () const noexcept
  {
    return m_capacity;
  }

  [[nodiscard]] double
  crossover_probability () const noexcept override
  {
    return m_crossover_probability;
  }

private:
  double m_capacity;
  double m_crossover_probability;
};

///
/// \brief Channel which introduces no noise. Of course, such doesn't actually
/// exist but is useful for debugging purposes.
///
class lossless_channel final : public channel
{
public:
  ///
  /// \brief The received word is the same as what is sent.
  ///
  [[nodiscard]] std::optional<patrick::linearcode::decoding_result>
  transfer (const infoword_type &sent, patrick::linearcode &code) override
  {
    return patrick::linearcode::decoding_result{
      .iword = sent, .error = codeword_type{ 0, code.properties ().word_size }
    };
  }

  ///
  /// \brief The probability of such lossless channel is always 0.
  ///
  [[nodiscard]] double
  crossover_probability () const noexcept override
  {
    return 0;
  }
};

} // namespace livedemo

#endif // LIVEDEMO_CHANNEL_H_INCLUDED
