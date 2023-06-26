#include <cassert>

#include <patrick/core.h>

namespace patrick
{

template <typename... T>
inline void
unimplemented ([[maybe_unused]] T &&...args)
{
  assert (0);
}

///
/// Constructors
///

[[nodiscard]] [[maybe_unused]] linearcode
linearcode::from_parity_equations (const Eigen::MatrixXi &parity_equations)
{
  return linearcode{ parity_equations };
}

[[nodiscard]] [[maybe_unused]] linearcode
linearcode::from_generator (const Eigen::MatrixXi &generator_matrix)
{
  return linearcode{ generator_matrix };
}

[[nodiscard]] [[maybe_unused]] linearcode
linearcode::from_dual (const linearcode &code)
{
  return linearcode{ code.dual () };
}

linearcode::linearcode (const Eigen::MatrixXi &generator_matrix)
    : m_generator{ generator_matrix }
{
  evaluate_properties_of ();
}

///
/// Operations
///

void
linearcode::prepare_codeword_cache ()
{
  // Use the rows of the generator matrix, because properties may still not be
  // initialized.
  const std::size_t basis_size = m_generator.rows ();
  const std::size_t total_codeword_count = 1 << basis_size;
  std::vector<codeword> codewords;
  codewords.reserve (total_codeword_count);
  for (std::size_t iword_as_num = 0; iword_as_num < total_codeword_count;
       ++iword_as_num)
    codewords.push_back (encode (infoword{ iword_as_num, basis_size }));
  std::sort (std::begin (codewords), std::end (codewords),
             [] (const auto &c1, const auto &c2) {
               return c1.weight () < c2.weight ();
             });
  m_cached_codewords = codewords;
}

void
linearcode::evaluate_properties_of ()
{
  prepare_codeword_cache ();
  // Safety: prepare_codeword_cache() populates this optional.
  assert (m_cached_codewords.has_value ());
  auto it = std::find_if (std::cbegin (*m_cached_codewords),
                          std::cend (*m_cached_codewords),
                          [&] (const auto &c) { return c.weight () > 0; });
  if (it == std::cend (*m_cached_codewords))
    throw linearcode_exception{ "Cannot find min_distance parameter." };
  auto min_distance = it->weight ();
  m_properties.min_distance = min_distance;
  m_properties.word_size = static_cast<std::size_t> (m_generator.cols ());
  m_properties.basis_size = static_cast<std::size_t> (m_generator.rows ());
  m_properties.max_errors_detect = min_distance - 1;
  m_properties.max_errors_correct = (min_distance - 1) / 2;
}

[[nodiscard]] [[maybe_unused]] linearcode
linearcode::dual () const noexcept
{
  unimplemented ();
}

[[nodiscard]] [[maybe_unused]] codeword
linearcode::encode (const infoword &iword) const
{
  if (iword.vec.cols () != m_generator.rows ())
    throw linearcode_exception{ fmt::format (
        "Trying to encode infoword '{}' which has size n={}, whereas the code "
        "expects n={}.",
        iword, iword.vec.cols (), m_generator.rows ()) };

  auto result{ iword.vec * m_generator };
  auto result_f2 = result.unaryExpr ([&] (const int x) { return x % 2; });
  return codeword{ result_f2 };
}

///
/// Decoding
///

[[nodiscard]] std::optional<infoword>
linearcode::decode_with_slepian (const codeword &cword) const
{
  unimplemented (cword);
}

[[nodiscard]] std::optional<infoword>
linearcode::decode_with_syndromes (const codeword &cword) const
{
  unimplemented (cword);
}

} // namespace patrick
