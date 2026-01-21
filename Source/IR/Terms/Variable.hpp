/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the Variable IR node
 * @author Oliver Dixon
 * @date 2025-06-15
 * @version Development
 */

#ifndef VARIABLE_HPP
#define VARIABLE_HPP

#include <optional>

#include "IProcessedTerm.hpp"

namespace optifol
{

/**
 * @class Variable
 * @brief A Variable is a non-owning IR node representing a first-order logic non-free variable.
 * @see MutableVariable for the owning, mutable dual; MutableVariable also contains more documentation of the semantics
 *  of an Optifol first-order logic variable.
 */
class Variable : public IProcessedTerm
{
public:
    /**
     * @brief Create a new Variable with a fixed display name
     * @param name The fixed name of the variable
     */
    explicit Variable(std::string name);

    /**
     * @brief Create a new Variable with a fixed display name and disambiguated name
     * @param name The fixed name of the variable
     * @param disambiguated_name The fixed disambiguated name for the variable
     * @warning No uniqueness check is done for the disambiguated name upon construction
     */
    explicit Variable(std::string name, const std::string &disambiguated_name);

    [[nodiscard]] std::string to_string() const override;

    [[nodiscard]] std::string_view get_disambiguated_name() const override;

    [[nodiscard]] bool operator==(const Variable & other) const;

    [[nodiscard]] bool accept(UnificationVisitor &unification_visitor, const IProcessedTerm &term) const override;
    [[nodiscard]] bool accept(UnificationVisitor &unification_visitor, const Constant &constant) const override;
    [[nodiscard]] bool accept(UnificationVisitor &unification_visitor, const Function &function) const override;
    [[nodiscard]] bool accept(UnificationVisitor &unification_visitor, const Variable &variable) const override;

    /**
     * @copydoc IProcessedTerm::is_self_nested
     * @return Always false, as Variable objects are not nestable.
     */
    [[nodiscard]] bool is_self_nested(const IProcessedTerm &search_term) const noexcept override;

    const IProcessedTerm *accept(const UnificationApplicationVisitor &unification_application_visitor) const override;

    [[nodiscard]] bool operator==(const IProcessedTerm &other) const noexcept override;

private:
    const std::string name;
    const std::optional<std::string> disambiguated_name;
};

} // namespace optifol

#endif
