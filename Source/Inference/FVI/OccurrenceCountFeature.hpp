/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 1/30/26.
//

#ifndef OPTIFOL_OCCURRENCECOUNTFEATURE_HPP
#define OPTIFOL_OCCURRENCECOUNTFEATURE_HPP

#include <variant>

#include "../../IHashable.hpp"
#include "../../IR/Sentences/Literal.hpp"

namespace optifol
{

class IProcessedTerm;
class Literal;

class OccurrenceCountFeature : public IHashable
{
public:
    explicit OccurrenceCountFeature(bool polarity);

    explicit OccurrenceCountFeature(const IProcessedTerm * target, bool polarity);

    explicit OccurrenceCountFeature(const Literal * target, bool polarity);

    [[nodiscard]] std::size_t hash() const noexcept override;

    [[nodiscard]] bool operator==(const OccurrenceCountFeature& other_feature) const noexcept;

    [[nodiscard]] bool operator==(bool other_polarity) const noexcept;

    [[nodiscard]] bool operator==(const Literal * literal_candidate) const noexcept;

    [[nodiscard]] bool operator==(const IProcessedTerm * term_candidate) const noexcept;

    [[nodiscard]] bool operator<(const OccurrenceCountFeature& other_feature) const noexcept;

private:
    struct EqualityComparator
    {
        [[nodiscard]] bool operator()(bool polarity_lhs, bool polarity_rhs) const noexcept;

        [[nodiscard]] bool operator()(bool polarity_lhs, const IProcessedTerm * term_rhs) const noexcept;

        [[nodiscard]] bool operator()(bool polarity_lhs, const Literal * literal_rhs) const noexcept;

        [[nodiscard]] bool operator()(const IProcessedTerm * term_lhs, const IProcessedTerm * term_rhs) const noexcept;

        [[nodiscard]] bool operator()(const IProcessedTerm * term_lhs, const Literal * literal_rhs) const noexcept;

        [[nodiscard]] bool operator()(const IProcessedTerm * term_lhs, bool polarity_rhs) const noexcept;

        [[nodiscard]] bool operator()(const Literal * literal_lhs, const IProcessedTerm * term_rhs) const noexcept;

        [[nodiscard]] bool operator()(const Literal * literal_lhs, const Literal * literal_rhs) const noexcept;

        [[nodiscard]] bool operator()(const Literal * literal_lhs, bool polarity_rhs) const noexcept;
    };

    struct LessThanComparator
    {
        [[nodiscard]] bool operator()(const IProcessedTerm * term_lhs, const IProcessedTerm * term_rhs) const noexcept;

        [[nodiscard]] bool operator()(const IProcessedTerm * term_lhs, const Literal * literal_rhs) const noexcept;

        [[nodiscard]] bool operator()(const Literal * literal_lhs, const IProcessedTerm * term_rhs) const noexcept;

        [[nodiscard]] bool operator()(const Literal * literal_lhs, const Literal * literal_rhs) const noexcept;
    };

    std::optional<std::variant<const IProcessedTerm *, const Literal *>> target;
    bool polarity;
};

} // namespace optifol

template<>
struct std::hash<optifol::OccurrenceCountFeature> : std::hash<optifol::IHashable>
{
    [[nodiscard]] std::size_t operator()(const optifol::OccurrenceCountFeature& feature) const noexcept
    {
        return feature.hash();
    }

    [[nodiscard]] std::size_t operator()(const bool polarity) const noexcept
    {
        return polarity;
    }

    [[nodiscard]] std::size_t operator()(const optifol::Literal * const literal) const noexcept
    {
        return std::hash<optifol::Literal>{}(*literal);
    }

    [[nodiscard]] std::size_t operator()(const optifol::IProcessedTerm * const term) const noexcept
    {
        return std::hash<optifol::IProcessedTerm>{}(*term);
    }
};

#endif // OPTIFOL_OCCURRENCECOUNTFEATURE_HPP
