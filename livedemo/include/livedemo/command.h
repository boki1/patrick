/// \file

#ifndef LIVEDEMO_COMMAND_H_INCLUDED
#define LIVEDEMO_COMMAND_H_INCLUDED

#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>

#include <fmt/core.h>
#include <patrick/core.h>

#include <livedemo/channel.h>

namespace livedemo
{

///
/// \class command_line_exception
/// \brief Indicates an exceptional behaviour during
///        an operation of a \ref command_line instance.
///
class command_line_exception : public std::runtime_error
{
public:
  explicit command_line_exception (const std::string &msg)
      : std::runtime_error{ fmt::format ("command_line_exception: {}", msg) }
  {
  }
};

class command_line
{
  static constexpr inline std::string prompt = "> ";

public:
  command_line (const std::string &greeting, std::ostream &os = std::cout,
                std::istream &is = std::cin)
      : m_out{ os }, m_in{ is }
  {
    m_out << greeting << '\n';
  }

public:
  using code_type = patrick::linearcode;
  using codeword_type = patrick::codeword;
  using infoword_type = patrick::infoword;

  /// \brief Returns whether to exit the command entering process.
  using cmd_type = bool (*) (command_line &);

  void add_cmd (const std::string &cmd_name, cmd_type func);

  bool perform_cmd (const std::string &cmd_name);

  void loop ();

public:
  [[nodiscard]] std::ostream &
      out ()
      & noexcept
  {
    return m_out;
  }

  [[nodiscard]] std::istream &
      in ()
      & noexcept
  {
    return m_in;
  }

  [[nodiscard]] code_type &
  get_code () &
  {
    if (has_code ())
      return *m_code;
    throw command_line_exception{ "" };
  }

  [[nodiscard]] channel &
  get_channel () &
  {
    if (!has_channel ())
      throw command_line_exception{ "no channel to access" };
    return *m_channel;
  }

  [[nodiscard]] bool
  has_code () const noexcept
  {
    return m_code.has_value ();
  }

  [[nodiscard]] bool
  has_channel () const noexcept
  {
    return m_channel.get () != nullptr;
  }

  void
  set_code (code_type c)
  {
    m_code.emplace (std::move (c));
  }
  void
  unset_code ()
  {
    m_code.reset ();
  }

  template <typename T>
  void
  set_channel (T &&channel)
  {
    m_channel = std::make_unique<T> (std::forward<T> (channel));
  }

private:
  std::unordered_map<std::string, cmd_type> m_commands;
  std::ostream &m_out;
  std::istream &m_in;
  std::optional<code_type> m_code;
  std::unique_ptr<channel> m_channel;
};

namespace commands
{

bool set_channel (command_line &);
bool transfer_through_channel (command_line &);

bool unload_code (command_line &);
bool load_code (command_line &);
bool encode (command_line &);
bool decode (command_line &);

bool props (command_line &);
bool show_slepian_table (command_line &);
bool show_codewords (command_line &);
bool show_generator (command_line &);
bool show_parity (command_line &);

bool exit (command_line &);
bool help (command_line &);

} // namespace commands

} // namespace livedemo

#endif // LIVEDEMO_COMMAND_H_INCLUDED
