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

std::optional<Substitution> UnifyCandidateBase::unify(const PredicationNode &predicate)
{
    return std::move(unify_substitution);
}

std::optional<Substitution> UnifyCandidateBase::unify(const ITermNode &variable)
{
    return std::move(unify_substitution);
}

bool UnifyCandidateBase::unify_work(const ITermNode &term)
{
    return false;
}

bool UnifyCandidateBase::unify_work(const PredicationNode &predicate)
{
    return false;
}

bool UnifyCandidateBase::unify_work(const FunctionNode &function)
{
    return false;
}

bool UnifyCandidateBase::unify_work(const VariableNode &variable)
{
    return false;
}

}
