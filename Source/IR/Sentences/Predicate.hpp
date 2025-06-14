/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

#ifndef PREDICATE_HPP
#define PREDICATE_HPP

#include <vector>

#include "ISentence.hpp"

namespace optifol
{

class IProcessedTerm;
class UnificationVisitor;

class Predicate :
        public ISentence
{
public:
    explicit Predicate(std::string name, bool is_positive = true, std::initializer_list<const IProcessedTerm *> arguments = {});

    explicit Predicate(std::string name, std::initializer_list<const IProcessedTerm *> arguments = {}, bool is_positive = true);

    std::string get_name() const;

    void flip_polarity() noexcept override;

    [[nodiscard]] bool is_negative_polarity() const noexcept override;

    std::ostream &serialise(std::ostream &ostream) const override;

    bool accept(UnificationVisitor &visitor, const Predicate &target) const;

    bool operator==(const std::unique_ptr<Predicate>& other) const noexcept
    {
        return other->hash() == hash();
    }

    bool operator==(const std::shared_ptr<Predicate>& other) const noexcept
    {
        return other->hash() == hash();
    }

    [[nodiscard]] std::size_t hash() const noexcept override;

    const std::vector<const IProcessedTerm *> &observe_arguments() const noexcept;

private:
    const std::string name;

    std::vector<const IProcessedTerm *> arguments;

    bool is_positive = true;
};

}

#endif
