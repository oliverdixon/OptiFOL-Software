/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the Constant Term IR node
 * @author Oliver Dixon
 * @date 2025-06-18
 * @version Development
 */

#ifndef CONSTANT_HPP
#define CONSTANT_HPP

#include "IProcessedTerm.hpp"

namespace optifol
{

/**
 * @class Constant
 * @brief A Constant is a first-order logic immutable term denoting a symbol that is not a bound variable and represents
 *  a ground truth. The immutable variant should be detained by a central SymbolRepository.
 * @see MutableConstant for the mutable dual
 */
class Constant : public IProcessedTerm
{
public:
    /**
     * @brief Construct a new immutable Constant IR node with a fixed name
     * @param name The fixed name of the Constant
     */
    explicit Constant(std::string name);

    [[nodiscard]] std::string to_string() const override;

    [[nodiscard]] std::string_view get_disambiguated_name() const override;

    [[nodiscard]] bool accept(UnificationVisitor &unification_visitor, const IProcessedTerm &term) const override;
    [[nodiscard]] bool accept(UnificationVisitor &unification_visitor, const Constant &constant) const override;
    [[nodiscard]] bool accept(UnificationVisitor &unification_visitor, const Variable &variable) const override;

    /**
     * @copydoc IProcessedTerm::is_self_nested
     * @return Always false, as Constant objects are not nestable.
     */
    [[nodiscard]] bool is_self_nested(const IProcessedTerm &search_term) const noexcept override;

    [[nodiscard]] const IProcessedTerm *accept(
            const UnificationApplicationVisitor &unification_application_visitor) const override;

    [[nodiscard]] bool operator==(const IProcessedTerm &other) const noexcept override;

private:
    const std::string name;
};

} // namespace optifol

#endif // CONSTANT_HPP
