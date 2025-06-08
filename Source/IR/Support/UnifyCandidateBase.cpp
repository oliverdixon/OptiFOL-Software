/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the Unifying Candidate base class
 * @author Oliver Dixon
 * @date 2025-06-07
 * @version Development
 */

#include "UnifyCandidateBase.hpp"

namespace optifol
{

const std::optional<Substitution> &UnifyCandidateBase::unify(const PredicationNode &predicate)
{
    unify_with_me(predicate);
    return unify_substitution;
}

const std::optional<Substitution> &UnifyCandidateBase::unify(const ITermNode &variable)
{
    return unify_substitution;
}

bool UnifyCandidateBase::unify_with_me(ITermNode &term)
{
    return false;
}

bool UnifyCandidateBase::unify_with_me(const PredicationNode &predicate)
{
    return false;
}

bool UnifyCandidateBase::unify_with_me(const FunctionNode &function)
{
    return false;
}

bool UnifyCandidateBase::unify_with_me(const VariableNode &variable)
{
    return false;
}

} // namespace optifol
