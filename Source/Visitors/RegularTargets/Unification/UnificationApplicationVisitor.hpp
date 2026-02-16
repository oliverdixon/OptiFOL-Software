/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the FOL substitution-application visitor
 * @author Oliver Dixon
 * @date 2026-01-21
 * @version Development
 */

#ifndef OPTIFOL_UNIFICATIONAPPLICATIONVISITOR_HPP
#define OPTIFOL_UNIFICATIONAPPLICATIONVISITOR_HPP

#include <optional>
#include <vector>

#include "../../../Inference/Unifier.hpp"

namespace optifol
{

class SymbolRepository;

class Function;
class BinaryConnected;
class Identity;
class Literal;
class SentenceRoot;

class UnificationApplicationVisitor
{
public:
    explicit UnificationApplicationVisitor(std::shared_ptr<Unifier> unifier,
        std::shared_ptr<SymbolRepository> symbol_repository);

    [[nodiscard]] const IProcessedTerm *visit(const Variable &node) const;

    [[nodiscard]] const IProcessedTerm *visit(const Function &node) const;

    [[nodiscard]] const Literal *visit(const Literal &node) const;

    void discard_new_symbols();

    void keep_new_symbols();

private:
    [[nodiscard]] std::optional<std::vector<const IProcessedTerm *>> apply_to_term_vector(
            const std::vector<const IProcessedTerm *> &terms) const;

    std::shared_ptr<Unifier> substitutions;

    const std::shared_ptr<SymbolRepository> existing_symbol_repository;
    std::unique_ptr<SymbolRepository> new_symbol_repository;
};

} // namespace optifol

#endif // OPTIFOL_UNIFICATIONAPPLICATIONVISITOR_HPP
