#include <cassert>

#include <patrick/core.h>

namespace patrick
{

template <typename... T>
[[noreturn]] inline void
unimplemented ([[maybe_unused]] T &&...args)
{
  assert (0);
}

///
/// Constructors
///

[[nodiscard]] [[maybe_unused]] static linearcode
from_parity_equations (const Eigen::MatrixXd &parity_equations)
{
  unimplemented (parity_equations);
}

[[nodiscard]] [[maybe_unused]] linearcode
linearcode::from_generator (const Eigen::MatrixXd &generator_matrix)
{
  return linearcode{ generator_matrix };
}

[[nodiscard]] [[maybe_unused]] linearcode
linearcode::from_dual (const linearcode &code)
{
  return linearcode{ code.dual () };
}

linearcode::linearcode (const Eigen::MatrixXd &generator_matrix)
    : m_generator{ generator_matrix }, m_properties{ evaluate_properties_of (
                                           generator_matrix) }
{
}

///
/// Operations
///

[[maybe_unused]] linearcode::properties_type
linearcode::evaluate_properties_of (const Eigen::MatrixXd &generator_matrix)
{
  unimplemented (generator_matrix);
}

[[nodiscard]] [[maybe_unused]] linearcode
linearcode::dual () const noexcept
{
  unimplemented ();
}

[[nodiscard]] [[maybe_unused]] linearcode::codeword
linearcode::encode (const infoword &iword) const
{
  unimplemented (iword);
}

///
/// Decoding
///

[[nodiscard]] std::optional<linearcode::infoword>
linearcode::decode_with_slepian (const codeword &cword) const
{
  unimplemented (cword);
}

[[nodiscard]] std::optional<linearcode::infoword>
linearcode::decode_with_syndromes (const codeword &cword) const
{
  unimplemented (cword);
}

} // namespace patrick
