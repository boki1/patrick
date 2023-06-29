#include <cassert>

#include <fmt/os.h>

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

/// Named ctors.

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
  unimplemented (code);
}

/// Actual ctor.

linearcode::linearcode (const Eigen::MatrixXi &generator_matrix)
    : m_generator{ generator_matrix }
{
  if (m_generator.isZero ())
    throw linearcode_exception (
        "Cannot instantiate a linearcode from the empty matrix.");
  evaluate_properties_of ();
}

///
/// Observers
///

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

///
/// Operation helpers
///

void
linearcode::evaluate_properties_of ()
{
  prepare_codewords ();

  // Safety: prepare_codeword_cache() populates this optional.
  assert (m_lazy_codewords.has_value ());

  auto it = std::find_if (std::cbegin (*m_lazy_codewords),
                          std::cend (*m_lazy_codewords),
                          [&] (const auto &c) { return c.weight () > 0; });

  if (it == std::cend (*m_lazy_codewords))
    throw linearcode_exception{ "Cannot find min_distance parameter." };

  auto min_distance = it->weight ();
  m_properties.min_distance = min_distance;
  m_properties.word_size = static_cast<std::size_t> (m_generator.cols ());
  m_properties.basis_size = static_cast<std::size_t> (m_generator.rows ());
  m_properties.max_errors_detect = min_distance - 1;
  m_properties.max_errors_correct = (min_distance - 1) / 2;
}

void
linearcode::prepare_codewords () const
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
  m_lazy_codewords = codewords;
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
  m_lazy_parity_matrix.emplace (std::move (_parity_matrix));
}

///
/// Operations
///

[[nodiscard]] [[maybe_unused]] codeword
linearcode::encode (const infoword &iword) const
{
  // Safety: This invariant is established during instantiation.
  assert (!m_generator.isZero ());

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

///
/// \brief Creates the Slepian table for this code.
/// \details The Slepian table is a rectangular table, whose rows are the
/// different cosets of the code. The first column contains what is known as a
/// coset leader - that is the element of a coset with minimal weight. The
/// fundamental steps of the algorithm that produces this table are the
/// following:
/// 1. For the first coset, put the codewords and set their leader as the null
/// vector. We will call this set \f$C\f$.
/// 2. Out the remaining codewords, select the one with minimal weight and name
/// it coset leader. We will call this vector \f$e\f$.
/// 3. Produce the next coset by successively adding the leader to each
/// codeword, that is \f$C' = C + e := \{ c + e | c \in C \}\f$.
/// 4. If now the table contains of vectors of \f$F_{2}^{n}\f$, then the
/// procedure is complete.
/// 5. Else, go to 2.
///
void
linearcode::prepare_slepian_table () const
{
  const std::size_t n = properties ().word_size;
  const std::size_t k = properties ().basis_size;
  const std::size_t num_rows = 1 << (n - k);
  const std::size_t num_words = 1 << n;

  std::vector<coset> slepian_table;
  slepian_table.reserve (num_rows);

  if (!m_lazy_codewords.has_value ())
    prepare_codewords ();

  // The first row (coset) of the Slepian table contains the codewords
  // themselves.
  slepian_table.emplace_back ([&] () {
    std::vector<codeword> non_null_codewords;
    non_null_codewords.resize (m_lazy_codewords->size () - 1);
    std::copy (m_lazy_codewords->cbegin () + 1, m_lazy_codewords->cend (),
               non_null_codewords.begin ());
    return coset{ .leader = codeword{ 0, n },
                  .columns = std::move (non_null_codewords) };
  }());

  const auto &table_header_leader = slepian_table.front ().leader;
  const auto &table_header_words = slepian_table.front ().columns;

  std::vector<bool> used (num_words, false);

  auto next_leader = [&] () {
    std::size_t min_weight = n;
    codeword min_c{ 0, n };
    for (auto i = 0ull; i < num_words; ++i)
      {
        if (used[i])
          continue;
        codeword c{ i, n };
        if (auto c_weight = c.weight (); c_weight < min_weight)
          {
            min_weight = c_weight;
            min_c = c;
          }
      }

    return min_c;
  };

  used[table_header_leader.to_ullong ()] = true;
  for (const auto &c : table_header_words)
    used[c.to_ullong ()] = true;

  // Start off from 1, because we already emplaced the first row.
  for (std::size_t i = 1; i < num_rows; ++i)
    {
      codeword leader = next_leader ();
      used[leader.to_ullong ()] = true;

      std::vector<codeword> words;
      std::transform (table_header_words.cbegin (), table_header_words.cend (),
                      std::back_inserter (words), [&] (const codeword &c) {
                        const codeword c_ = c + leader;
                        used[c_.to_ullong ()] = true;
                        return c_;
                      });
      slepian_table.emplace_back (
          coset{ .leader = leader, .columns = std::move (words) });
    }

  m_lazy_slepian_table.emplace (std::move (slepian_table));
}

[[nodiscard]] linearcode::decoding_result
linearcode::decode_with_slepian (const codeword &cword)
{
  if (!m_lazy_slepian_table)
    prepare_slepian_table ();

  const std::size_t num_rows
      = 1 << (properties ().word_size - properties ().basis_size);
  /// Safety: That's a property of the Slepian table.
  assert (m_lazy_slepian_table->size () == num_rows);

  const codeword &topleft = m_lazy_slepian_table->front ().leader;
  const std::vector<codeword> &codewords
      = m_lazy_slepian_table->front ().columns;

  codeword corrected_cword;
  codeword correction;
  for (const coset &row : *m_lazy_slepian_table)
    {
      if (cword == row.leader)
        {
          corrected_cword = topleft;
          correction = row.leader;
          break;
        }
      const auto it = std::ranges::find (row.columns, cword);
      if (it != std::cend (row.columns))
        {
          const std::size_t column_idx
              = std::distance (std::begin (row.columns), it);
          corrected_cword = codewords.at (column_idx);
          correction = row.leader;
          break;
        }
    }

  /// Safety: cword is of size that may be part of the linear code subspace.
  /// The Slepian table contains all words that are part of \f$F_{2}^{n}\f$, so
  /// at some point the `find()` inside the loop body will reach it.
  assert (corrected_cword.vec.size () > 0 && correction.vec.size () > 0);

  const std::size_t t = m_properties.max_errors_detect;

  if (const auto num_errors_found = correction.weight (); num_errors_found > t)
    throw linearcode_exception{ fmt::format (
        "Cannot decode codeword '{}' because more errors were "
        "found then the maximum that could be recovered - {} > {}.",
        cword, num_errors_found, t) };

  /// The conversion from a codeword of size N to its infoword of size K is
  /// based on the fact that the encoding is systematic, meaning that the
  /// positions of the codeword which contain information are the same as kept
  /// the same as in the infoword. Therefore, we have to _cut_ the codeword and
  /// get only the K rightmost of them. The rightmost are used because of the
  /// requirement that we made on the generator matrix, that it is in _standard
  /// form_ G = (E|A).
  const std::size_t k = properties ().basis_size;
  return decoding_result{ .iword
                          = infoword{ corrected_cword.vec.leftCols (k) },
                          .error = correction };
}

void
linearcode::prepare_syndrome_table () const
{
  const std::size_t n = properties ().word_size;
  const std::size_t k = properties ().basis_size;
  const std::size_t num_rows = 1 << (n - k);
  const std::size_t num_words = 1 << n;

  std::vector<bool> used (num_words, false);

  auto out = fmt::output_file ("debug-syndromes-leader.log");
  auto next_leader = [&] () {
    std::size_t min_weight = n;
    codeword min_c{ 0, n };
    for (auto i = 0ull; i < num_words; ++i)
      {
        if (used[i])
          continue;
        codeword c{ i, n };
        if (auto c_weight = c.weight (); c_weight < min_weight)
          {
            min_weight = c_weight;
            min_c = c;
          }
      }

    out.print ("next_leader -> {}\n", min_c);
    return min_c;
  };

  syndrome_table_type table;
  table.reserve (num_rows);

  while (table.size () < num_rows)
    {
      auto leader = next_leader ();
      used[leader.to_ullong ()] = true;
      const auto syndr = syndrome_of (leader);
      table.try_emplace (syndr, leader);
    }

  m_lazy_syndrome_table.emplace (std::move (table));
}

[[nodiscard]] linearcode::decoding_result
linearcode::decode_with_syndromes (const codeword &cword)
{
  if (!m_lazy_syndrome_table)
    prepare_syndrome_table ();

  const std::size_t num_rows
      = 1 << (properties ().word_size - properties ().basis_size);
  /// Safety: That's a property of the syndrome table.
  assert (m_lazy_syndrome_table->size () == num_rows);
  const auto &syndrome_table = *m_lazy_syndrome_table;

  const syndrome s = syndrome_of (cword);
  const codeword error = syndrome_table.at (s);
  const codeword corrected_cword = cword + error;

  const std::size_t k = properties ().basis_size;
  return decoding_result{ .iword
                          = infoword{ corrected_cword.vec.leftCols (k) },
                          .error = error };
}

} // namespace patrick
