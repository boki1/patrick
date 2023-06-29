#include <fstream>
#include <sstream>

#include <Eigen/Dense>
#include <fmt/core.h>
#include <fmt/ranges.h>
#include <patrick/core.h>

#include <livedemo/command.h>

using namespace patrick;

namespace livedemo
{

void
command_line::add_cmd (const std::string &cmd_name, cmd_type func)
{
  m_commands.emplace (cmd_name, func);
}

bool
command_line::perform_cmd (const std::string &cmd_name)
{
  if (m_commands.contains (cmd_name))
    {
      return m_commands.at (cmd_name) (*this);
    }
  else
    {
      out () << "Error: Unknown command.\n";
    }

  return false;
}

void
command_line::loop ()
{
  std::string cmd_name;
  while (true)
    {
      out () << command_line::prompt;
      in () >> cmd_name;
      if (perform_cmd (cmd_name))
        break;
    }
}

namespace commands
{

template <typename T>
static T
read_word (std::istream &is)
{
  std::string w;
  is >> w;
  return T{ w };
}

bool
load_code (command_line &l)
{
  std::string fname;
  l.in () >> fname;
  std::ifstream ifs{ fmt::format ("livedemo/presets/{}-generator.txt",
                                  fname) };
  std::string first_line;
  std::getline (ifs, first_line);

  ifs.seekg (0, std::ios_base::beg);
  auto matrix_flat = std::vector<int> (std::istream_iterator<int> (ifs),
                                       std::istream_iterator<int> ());

  const std::size_t num_columns = std::ranges::count (first_line, ' ') + 1;
  const std::size_t num_rows = matrix_flat.size () / num_columns;
  Eigen::MatrixXi generator_matrix (num_rows, num_columns);
  for (std::size_t i = 0; i < num_rows; ++i)
    for (std::size_t j = 0; j < num_columns; ++j)
      generator_matrix (i, j) = matrix_flat[num_columns * i + j];

  l.set_code (linearcode::from_generator (generator_matrix));
  return false;
}

bool
unload_code (command_line &l)
{
  l.unset_code ();
  return false;
}

bool
encode (command_line &l)
{
  if (!l.has_code ())
    l.out () << "Error: Load a code before using its decoding capabilities.\n";
  auto iword = read_word<infoword> (l.in ());
  try
    {
      const codeword cword = l.get_code ().encode (iword);
      l.out () << fmt::format ("Success: '{}'.\n", cword);
    }
  catch (const linearcode_exception &le)
    {
      l.out () << fmt::format ("Error: Failed encoding infoword '{}'.\n",
                               iword);
    }
  return false;
}

bool
decode (command_line &l)
{
  if (!l.has_code ())
    l.out () << "Error: Load a code before using its decoding capabilities.\n";
  const auto cword = read_word<codeword> (l.in ());
  try
    {
      const linearcode::decoding_result result = l.get_code ().decode (cword);
      l.out () << fmt::format ("Success: '{}' with error '{}'.\n",
                               result.iword, result.error);
    }
  catch (const linearcode_exception &le)
    {
      l.out () << fmt::format ("Error: {}\n", le.what ());
    }
  return false;
}

bool
props (command_line &l)
{
  if (!l.has_code ())
    l.out () << "Error: Load a code before accessing its properties.\n";
  l.out () << fmt::format ("{}\n", l.get_code ().properties ());
  return false;
}

bool
show_generator (command_line &l)
{
  if (!l.has_code ())
    l.out () << "Error: Load a code before accessing its properties.\n";

  const auto &generator = l.get_code ().generator_matrix ();
  std::stringstream sstr;
  sstr << generator;
  l.out () << fmt::format ("{}\n", sstr.str ());

  return false;
}

bool
show_parity (command_line &l)
{
  if (!l.has_code ())
    l.out () << "Error: Load a code before accessing its properties.\n";

  const auto &parity = l.get_code ().parity_matrix ();
  std::stringstream sstr;
  sstr << parity;
  l.out () << fmt::format ("{}\n", sstr.str ());

  return false;
}

bool
show_codewords (command_line &l)
{
  if (!l.has_code ())
    l.out () << "Error: Load a code before accessing its properties.\n";

  const auto &codewords = *l.get_code ().codewords ();
  l.out () << fmt::format ("{}\n", fmt::join (codewords, ",\n"));

  return false;
}

bool
show_slepian_table (command_line &l)
{
  if (!l.has_code ())
    l.out () << "Error: Load a code before accessing its properties.\n";

  const auto &table = *l.get_code ().slepian_table ();
  for (const auto &row : table)
    l.out () << fmt::format ("{} | {}\n", row.leader,
                             fmt::join (row.columns, " "));

  return false;
}

bool
set_channel (command_line &l)
{
  std::string channel_name;
  l.in () >> channel_name;
  if (channel_name == "binary_symm" || channel_name == "default")
    {
      l.set_channel (binary_symm_channel{});
    }
  else if (channel_name == "lossless")
    l.set_channel (lossless_channel{});
  else
    l.out () << "Error: Trying to set unknown channel type.\n";
}

bool
transfer_through_channel (command_line &l)
{
  if (!l.has_code ())
    l.out () << "Error: Load a code before trying to use it.\n";
  if (!l.has_channel ())
    l.out () << "Error: Set a channel type before trying to use it.\n";

  auto iword = read_word<infoword> (l.in ());

  if (const auto result = l.get_channel ().transfer (iword, l.get_code ());
      result)
    {
      l.out () << fmt::format ("Success: Received '{}' with error '{}'.\n",
                               result->iword, result->error);
    }
  else
    {
      l.out () << fmt::format ("Error: Failed trasferring infoword '{}'.\n",
                               iword);
    }

  return false;
}

bool
exit (command_line &)
{
  return true;
}

bool
help (command_line &l)
{
  l.out () << "livedemo:\n";
  l.out () << "  exit\n";
  l.out () << "  help\n";
  l.out () << "  props\n";
  l.out () << "  slepian_table\n";
  l.out () << "  load_code\n";
  l.out () << "  encode\n";
  l.out () << "  decode\n";
  l.out () << "  set_channel\n";
  l.out () << "  transfer_through_channel\n";
  return false;
}

} // namespace commands

} // namespace livedemo
