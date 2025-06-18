/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the Identity Sentence IR node
 * @author Oliver Dixon
 * @date 2025-06-18
 * @version Development
 */

#include "Identity.hpp"

namespace optifol
{

Identity::Identity(const IProcessedTerm * const lhs, const IProcessedTerm * const rhs, const bool is_positive) :
    lhs(lhs),
    rhs(rhs),
    is_positive(is_positive)
{
}

bool Identity::is_negative_polarity() const noexcept
{
    return !is_positive;
}

const IProcessedTerm *Identity::observe_lhs_operand() const noexcept
{
    return lhs;
}

const IProcessedTerm *Identity::observe_rhs_operand() const noexcept
{
    return rhs;
}

} // namespace optifol
