/// \file

#ifndef PATRICK_CORE_H_INCLUDED
#define PATRICK_CORE_H_INCLUDED

#include <cstdint>
#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>

#include <Eigen/Dense>
#include <fmt/core.h>

#include <patrick/word.h>

namespace patrick
{

///
/// \class linearcode_exception
/// \brief Indicates an exceptional behaviour during
///        an operation of a \a linearcode instance.
///
class linearcode_exception : public std::runtime_error
{
public:
  explicit linearcode_exception (const std::string &msg)
      : std::runtime_error{ fmt::format ("patrick: {}", msg) }
  {
  }
};

///
/// \class linearcode
/// \brief Represents a linear \f$[n, k, d]\f$ code.
/// \details Linear code \f$C\f$ is usually defined as a subspace of the linear
/// space \f$F^{n}_{2}\f$. If its dimension is \f$k\f$, then \f$C\f$ is said to
/// be a \f$[n, k]\f$ code. The third parameter \f$d\f$ denotes the minimum
/// distance of the code. In the special case of linear codes, that is usually
/// defined as the minimum weight of a non-zero vector of \f$C\f$.
///
class linearcode final
{
public:
  ///
  /// Related types
  ///

  struct properties_type
  {
    std::string special_name{ "Linear" };
    std::size_t word_size{ 0 };
    std::size_t basis_size{ 0 };
    std::size_t min_distance{ 0 };
    std::size_t max_errors_detect{ 0 };
    std::size_t max_errors_correct{ 0 };
  };

  struct coset
  {
    codeword leader;
    std::vector<codeword> columns;
  };

  ///
  /// \brief Represents a result from the decoding strategy used.
  ///
  struct decoding_result
  {
    infoword iword;
    codeword error;
  };

  using syndrome_table_type = std::unordered_map<syndrome, codeword>;

  ///
  /// Constructors
  ///

  ///
  /// \param parity_equations Set of pairs that give the redundency positions
  /// in terms of the information positions.
  /// \return Constructed
  /// \ref linearcode instance which is described by the provided equations.
  ///
  [[nodiscard]] [[maybe_unused]] static linearcode
  from_parity_equations (const Eigen::MatrixXi &parity_equations);

  ///
  /// \param generator_matrix A \f$kxn\f$ matrix whose rows are the basis
  /// vectors of a linear subspace.
  /// \return Constructed \ref linearcode instance which is described by
  /// the provided generator.
  ///
  [[nodiscard]] static linearcode
  from_generator (const Eigen::MatrixXi &generator_matrix);

  ///
  /// \param dual A linear code
  /// \return Constructed \ref linearcode instance which is dual to the
  /// provided code.
  ///
  [[nodiscard]] static linearcode from_dual (const linearcode &code);

private:
  ///
  /// \brief This constructor should remain private, since the
  /// internal representation of the linear code is a \a secret :D. Anyways,
  /// both the parity equations and the generator are actually matrices, so for
  /// API clarity I think that the most suitable way for creating instances is
  /// my using the named ctors \a from_*(). \param generator_matrix The linear
  /// code represented by a generator matrix.
  ///
  explicit linearcode (const Eigen::MatrixXi &generator_matrix);

public:
  ///
  /// Observers
  ///

  [[nodiscard]] const properties_type &
  properties () const &noexcept
  {
    return m_properties;
  }

  ///
  /// \brief Calculates the parity matrix for this code.
  ///
  const Eigen::MatrixXi &
  parity_matrix () const
  {
    if (!m_lazy_parity_matrix.has_value ())
      prepare_parity_matrix ();
    // We either had it before or we just evaluated the parity matrix.
    assert (m_lazy_parity_matrix);
    return *m_lazy_parity_matrix;
  }

  ///
  /// \brief Calculates the parity matrix for this code.
  ///
  const Eigen::MatrixXi &
  generator_matrix () const
  {
    return m_generator;
  }

  const std::optional<std::vector<codeword> > &
  codewords () const noexcept
  {
    if (!m_lazy_codewords.has_value ())
      prepare_codewords ();
    // We either had it before or we just evaluated it.
    assert (m_lazy_codewords);
    return m_lazy_codewords;
  }

  const std::optional<std::vector<coset> > &
  slepian_table () const noexcept
  {
    if (!m_lazy_slepian_table.has_value ())
      prepare_slepian_table ();
    // We either had it before or we just evaluated it.
    assert (m_lazy_slepian_table);
    return m_lazy_slepian_table;
  }

  const std::optional<syndrome_table_type> &
  syndrome_table () const noexcept
  {
    if (!m_lazy_syndrome_table.has_value ())
      prepare_syndrome_table ();
    // We either had it before or we just evaluated it.
    assert (m_lazy_syndrome_table);
    return m_lazy_syndrome_table;
  }

private:
  ///
  /// \param generator_matrix Representation of the linear code that is being
  /// inspected.
  /// \note This is called in the ctor of \ref linearcode.
  ///
  void evaluate_properties_of ();

private:
  ///
  /// \brief Use method for decoding that is based on the
  /// [standard (or
  /// Slepian)array](https://en.wikipedia.org/wiki/Standard_array).
  ///
  [[nodiscard]] decoding_result decode_with_slepian (const codeword &cword);

  ///
  /// \brief Use method for decoding that is based on the
  /// [standard (or
  /// Slepian)array](https://en.wikipedia.org/wiki/Standard_array).
  ///
  [[nodiscard]] decoding_result decode_with_syndromes (const codeword &cword);

  ///
  /// \brief Exhausts all valid codewords and stores them in \ref
  /// m_lazy_codewords.
  /// \note May get called by `evaluate_properties_of()` and
  /// `prepare_slepian_table()`.
  ///
  void prepare_codewords () const;

  ///
  /// \brief Creates the parity matrix of the linear code.
  /// \note Called when trying to access the \ref parity_matrix member and it
  /// has not been populated yet.
  ///
  void prepare_parity_matrix () const;

  ///
  /// \brief Creates the Slepian table that is used for decoding with \ref
  /// decoding_strategy::SlepianTable.
  /// \note Called when \ref decode<SlepianTable> is being performed and the
  /// table has not been populated yet.
  ///
  void prepare_slepian_table () const;

  void prepare_syndrome_table () const;

public:
  ///
  /// Operations
  ///

  void
  set_special_name (const std::string &t_special_name)
  {
    m_properties.special_name = t_special_name;
  }

  ///
  /// \brief Check whether a given codeword is in the code. That is equivalent,
  /// to the fact that the vector is in the vector subspace that is this code.
  ///
  [[nodiscard]] bool contains (const codeword &cword) const;

  ///
  /// \brief Calculates the syndrome of a given codeword.
  ///
  [[nodiscard]] syndrome syndrome_of (const codeword &cword) const;

  ///
  /// \brief Encodes an information word by adding redundency bits.
  /// \return The encoded code word.
  ///
  [[nodiscard]] codeword encode (const infoword &) const;

  enum class decoding_strategy
  {
    SlepyanTable,
    Syndromes
  };

  ///
  /// \brief Tries to decode a code word into its corresponding
  /// information word. The algorithm is based on maximum likelihood decoding.
  /// \tparam strategy The decoding strategy to be used.
  /// \param cword Any codeword \f$c \in C\f$.
  /// \throws \ref linearcode_exception if it cannot be decoded in any way.
  /// That could happen if the word size of the linear code's code words is
  /// different then the input's size.
  /// \return Either the decoded \ref
  /// infoword, or an empty value.
  ///
  template <enum decoding_strategy Strategy = decoding_strategy::SlepyanTable>
  [[nodiscard]] decoding_result
  decode (const codeword &cword)
  {
    // Safety: This invariant is established during instantiation.
    assert (!m_generator.isZero ());

    using enum decoding_strategy;
    if constexpr (Strategy == SlepyanTable)
      return decode_with_slepian (cword);
    if constexpr (Strategy == Syndromes)
      return decode_with_syndromes (cword);

    /// There are only two valid values for an enumerator of \ref
    /// decoding_strategy. If this line is reached (and the if statements
    /// actually exhaust all values), then \ref decode has been called
    /// in a semantically correct way such as
    /// `decode<static_cast<[...]>(42)>([...]);`
    throw linearcode_exception{ fmt::format (
        "Invalid decoding strategy '{}' used with linear code.",
        static_cast<std::uint8_t> (Strategy)) };
  }

private:
  ///
  /// \brief The internal representation of a linear code is based
  /// on a generator matrix.
  ///
  const Eigen::MatrixXi m_generator;

  ///
  /// \brief The basic properties of the linear code that is
  /// represented by the instance.
  ///
  properties_type m_properties;

  // TODO: Make these lazy_loaded<T, LoadFunc, Args ...>

  ///
  /// \brief All codewords that are part of the linear code. They are stored is
  /// sorted order, relative to their order.
  ///
  mutable std::optional<std::vector<codeword> > m_lazy_codewords;
  mutable std::optional<Eigen::MatrixXi> m_lazy_parity_matrix;
  mutable std::optional<std::vector<coset> > m_lazy_slepian_table;
  mutable std::optional<syndrome_table_type> m_lazy_syndrome_table;
};

} // namespace patrick

template <>
struct fmt::formatter<patrick::linearcode::properties_type>
    : fmt::formatter<std::string_view>
{
  template <typename FormatContext>
  auto
  format (const patrick::linearcode::properties_type &props,
          FormatContext &ctx)
  {
    return format_to (
        ctx.out (), "{} [{}, {}, {}] code: Detects {} and corrects {} errors.",
        props.special_name, props.word_size, props.basis_size,
        props.min_distance, props.max_errors_detect, props.max_errors_correct);
  }
};

#endif // PATRICK_CORE_H_INCLUDED
