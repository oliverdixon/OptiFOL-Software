/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the Variable-Term Substitution Map
 * @author Oliver Dixon
 * @date 2025-06-07
 * @version Development
 */

#ifndef SUBSTITUTION_HPP
#define SUBSTITUTION_HPP

#include <functional>
#include <unordered_map>

namespace optifol
{

class VariableNode;
class ITermNode;

class Substitution
{
private:
    struct KeyRefHashingFunctor
    {
        using is_transparent = void;

        std::size_t operator()(std::reference_wrapper<const VariableNode> object) const noexcept;
    };

    struct KeyRefEqualityFunctor
    {
        using is_transparent = void;

        bool operator()(std::reference_wrapper<const VariableNode> lhs_object,
                        std::reference_wrapper<const VariableNode> rhs_object) const noexcept;
    };

public:
    Substitution() = default;

    explicit Substitution(std::initializer_list<std::pair<const VariableNode&, const ITermNode&>> entries);

    // TODO
    std::unordered_map<std::reference_wrapper<const VariableNode>, std::reference_wrapper<const ITermNode>,
        KeyRefHashingFunctor, KeyRefEqualityFunctor> bindings;

    bool operator==(const Substitution& substitution) const;
};

}

#endif //SUBSTITUTION_HPP
