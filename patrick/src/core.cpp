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
  // TODO: Put the generator matrix into standard form.
  // For now, assume that is done already.
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

[[nodiscard]] syndrome
linearcode::syndrome_of (const codeword &cword) const
{
  if (cword.vec.cols () != m_generator.cols ())
    throw linearcode_exception{ fmt::format (
        "Codeword '{}' has incompatible dimensions to be part "
        "of a code, whose generator matrix has {} columns.",
        cword, m_generator.cols ()) };
  const auto &H = parity_matrix ();
  const auto product
      = cword.vec.operator* (H.transpose ()).unaryExpr ([&] (const int x) {
          return x % 2;
        });
  return syndrome{ std::move (product) };
}

[[nodiscard]] bool
linearcode::contains (const codeword &cword) const
{
  try
    {
      auto s = syndrome_of (cword);
      return s.vec.isZero ();
    }
  catch (const linearcode_exception &)
    {
      // No need to throw exceptions here. Just say that the codeword is not
      // part of this linearcode and be done with it. Otherwise would be way
      // too hard for this function to be called - it returning a boolean
      // and\or throwing and exception which is actually a boolean.
      return false;
    }
}

void
linearcode::prepare_parity_matrix () const
{
  const std::size_t k = m_generator.rows ();
  const std::size_t n = m_generator.cols ();
  const std::size_t t = n - k;
  Eigen::MatrixXi _parity_matrix (t, k + t);
  _parity_matrix.rightCols (t) = Eigen::MatrixXi::Identity (t, t);
  _parity_matrix.leftCols (k) = m_generator.rightCols (t).transpose ();
  m_parity_matrix.emplace (std::move (_parity_matrix));
}

const Eigen::MatrixXi &
linearcode::parity_matrix () const
{
  if (!m_parity_matrix.has_value ())
    prepare_parity_matrix ();
  // We either had it before or we just evaluated the parity matrix.
  assert (m_parity_matrix);
  return *m_parity_matrix;
}

const Eigen::MatrixXi &
linearcode::generator_matrix () const
{
  return m_generator;
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

  const auto product
      = iword.vec.operator* (m_generator).unaryExpr ([&] (const int x) {
          return x % 2;
        });
  return codeword{ std::move (product) };
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
