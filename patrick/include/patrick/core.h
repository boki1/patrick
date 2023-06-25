/// \file

#ifndef PATRICK_CORE_H_INCLUDED
#define PATRICK_CORE_H_INCLUDED

#include <optional>
#include <stdexcept>
#include <string>
#include <utility>

#include <eigen3/Eigen/Dense>

#include <fmt/core.h>

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

  using codeword = std::size_t;
  using infoword = std::size_t;

  struct properties_type
  {
    std::size_t word_size;
    std::size_t basis_size;
    std::size_t min_distance;
    std::size_t max_errors_detect;
    std::size_t max_errors_correct;
  };

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
  from_parity_equations (const Eigen::MatrixXd &parity_equations);

  ///
  /// \param generator_matrix A \f$kxn\f$ matrix whose rows are the basis
  /// vectors of a linear subspace.
  /// \return Constructed \ref linearcode instance which is described by
  /// the provided generator.
  ///
  [[nodiscard]] static linearcode
  from_generator (const Eigen::MatrixXd &generator_matrix);

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
  linearcode (const Eigen::MatrixXd &generator_matrix);

public:
  ///
  /// Accessors
  ///

  [[nodiscard]] const properties_type &
  properties () const &noexcept
  {
    return m_properties;
  }

  ///
  /// \brief New \ref linearcode instance that stores the dual code of the
  /// current one.
  ///
  [[nodiscard]] linearcode dual () const noexcept;

private:
  ///
  /// \param generator_matrix Representation of the linear code that is being
  /// inspected.
  /// \return A populated \ref properties instances.
  /// \note This is called in the ctor of \ref linearcode.
  ///
  static properties_type
  evaluate_properties_of (const Eigen::MatrixXd &generator_matrix);

  ///
  /// \brief Use method for decoding that is based on the
  /// [standard (or
  /// Slepian)array](https://en.wikipedia.org/wiki/Standard_array).
  ///
  [[nodiscard]] std::optional<infoword>
  decode_with_slepian (const codeword &cword) const;

  ///
  /// \brief Use method for decoding that is based on the
  /// [standard (or
  /// Slepian)array](https://en.wikipedia.org/wiki/Standard_array).
  ///
  [[nodiscard]] std::optional<infoword>
  decode_with_syndromes (const codeword &cword) const;

public:
  ///
  /// Operations
  ///

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
  /// \return Either the decoded \ref infoword, or an empty value.
  ///
  template <enum decoding_strategy Strategy = decoding_strategy::SlepyanTable>
  [[nodiscard]] std::optional<infoword>
  decode (const codeword &cword) const
  {
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
  const Eigen::MatrixXd m_generator;

  ///
  /// \brief The basic properties of the linear code that is
  /// represented by the instance.
  ///
  const properties_type m_properties;
};

} // namespace patrick

#endif // PATRICK_CORE_H_INCLUDED
